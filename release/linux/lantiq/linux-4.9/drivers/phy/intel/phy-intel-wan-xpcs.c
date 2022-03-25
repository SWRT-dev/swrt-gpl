/*
 * Intel WAN XPCS PHY driver
 *
 * Copyright (C) 2018 Intel, Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h>
#include <linux/clk.h>
#include <linux/reset.h>
#include <linux/mfd/syscon.h>
#include <linux/regmap.h>
#include <linux/debugfs.h>
#include <linux/uaccess.h>

/* chiptop aon/pon config; this is platform specific */
#define CHIP_TOP_IFMUX_CFG 0x120
#define WAN_MUX_AON        0x1
#define WAN_MUX_MASK       0x1
#define TX_MAIN_CUR			0x4008
#define TX_MAIN_CUR_MASK		0x3F
#define TX_MAIN_CUR_OFF			9
#define MAIN_CURSOR_OVRD		15
#define TX_PRE_CUR			0x400C
#define TX_PRE_CUR_MASK			0x1F
#define TX_PRE_CUR_OFF			0
#define POST_OVRD_EN			6
#define TX_POST_CUR			0x400C
#define TX_POST_CUR_MASK		0x3F
#define TX_POST_CUR_OFF			7
#define PRE_OVRD_EN			13

enum {
	PHY_RST,
	WANSS_RST,
	MAX_RST
};

static const char *rst_name[MAX_RST] = {
	"phy", "wanss"
};

struct intel_wan_xpcs_phy {
	struct phy             *phy;
	struct platform_device *pdev;
	struct device          *dev;
	struct clk             *clk;
	u32                    clk_freq;
	struct regmap          *syscfg;
	struct reset_control   *resets[MAX_RST];
	void __iomem *base;
	struct dentry *debugfs;
};

static u32 intel_wan_xpcs_r32(void __iomem *base, u32 reg)
{
	return readl(base + reg);
}

static void intel_wan_xpcs_w32(void __iomem *base, u32 val,  u32 reg)
{
	writel(val, base + reg);
}

static void intel_wan_xpcs_w32_off_mask(void __iomem *base, u32 off,
					       u32 mask, u32 set, u32 reg)
{
	u32 val;

	val = intel_wan_xpcs_r32(base, reg) & (~(mask << off));
	val |= (set & mask) << off;
	intel_wan_xpcs_w32(base, val, reg);
}

static void intel_wan_xpcs_reg_bit_set(void __iomem *base, u32 off, u32 reg)
{
	intel_wan_xpcs_w32_off_mask(base, off, 1, 1, reg);
}

#if IS_ENABLED(CONFIG_DEBUG_FS)
static ssize_t
intel_wan_xpcs_phy_serdes_write(struct file *s, const char __user *buffer,
				size_t count, loff_t *pos)
{
	struct intel_wan_xpcs_phy *priv = file_inode(s)->i_private;
	u32 main_cur = 0, pre_cur = 0, post_cur = 0;
	char buf[32] = {0};
	size_t buf_size;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (count > sizeof(buf) -1)
		return -EINVAL;

	memset(buf, 0, sizeof(buf));
	buf_size = min(count, sizeof(buf) -1);
	if (copy_from_user(buf, buffer, buf_size))
		return -EFAULT;

	if (strcmp(buf, "help") == 0)
		goto __write_help;

	if (sscanf(buf, "%u %u %u", &main_cur, &pre_cur, &post_cur) != 3)
		goto __write_help;

	/* Cursor */
	intel_wan_xpcs_w32_off_mask(priv->base, TX_MAIN_CUR_OFF,
				    TX_MAIN_CUR_MASK, main_cur, TX_MAIN_CUR);
	intel_wan_xpcs_w32_off_mask(priv->base, TX_PRE_CUR_OFF,
				    TX_PRE_CUR_MASK, pre_cur, TX_PRE_CUR);
	intel_wan_xpcs_w32_off_mask(priv->base, TX_POST_CUR_OFF,
				    TX_POST_CUR_MASK, post_cur, TX_POST_CUR);

	/* Override */
	intel_wan_xpcs_reg_bit_set(priv->base, MAIN_CURSOR_OVRD,
				   TX_MAIN_CUR);
	intel_wan_xpcs_reg_bit_set(priv->base, POST_OVRD_EN, TX_PRE_CUR);
	intel_wan_xpcs_reg_bit_set(priv->base, PRE_OVRD_EN, TX_POST_CUR);

	return count;

__write_help:
	dev_info(priv->dev, "echo <main_cur> <pre_cur> <post_cur> > /sys/kernel/debug/phy/wan_xpcs_serdes\n");
	return count;
}

static int intel_wan_xpcs_phy_seq_read(struct seq_file *s, void *v)
{
	struct intel_wan_xpcs_phy *priv = s->private;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	dev_info(priv->dev, "TX_MAIN_CUR\t%u\n",
		 ((intel_wan_xpcs_r32(priv->base, TX_MAIN_CUR) &
		 (TX_MAIN_CUR_MASK << TX_MAIN_CUR_OFF)) >> TX_MAIN_CUR_OFF));
	dev_info(priv->dev, "TX_PRE_CUR\t%u\n",
		 ((intel_wan_xpcs_r32(priv->base, TX_PRE_CUR) &
		 (TX_PRE_CUR_MASK << TX_PRE_CUR_OFF)) >> TX_PRE_CUR_OFF));
	dev_info(priv->dev, "TX_POST_CUR\t%u\n",
		 ((intel_wan_xpcs_r32(priv->base, TX_POST_CUR) &
		 (TX_POST_CUR_MASK << TX_POST_CUR_OFF)) >> TX_POST_CUR_OFF));

	return 0;
}

static int intel_wan_xpcs_phy_seq_open(struct inode *inode, struct file *file)
{
	return single_open(file, intel_wan_xpcs_phy_seq_read, inode->i_private);
}

static const struct file_operations intel_wan_xpcs_fops = {
	.owner = THIS_MODULE,
	.open = intel_wan_xpcs_phy_seq_open,
	.read = seq_read,
	.write = intel_wan_xpcs_phy_serdes_write,
	.llseek = seq_lseek,
	.release = single_release,
};

static int intel_wan_xpcs_phy_debugfs_init(struct intel_wan_xpcs_phy *priv)
{
	char wan_xpcs_dir[64] = {0};

	strlcpy(wan_xpcs_dir, priv->dev->of_node->name, sizeof(wan_xpcs_dir));
	priv->debugfs = debugfs_create_dir(wan_xpcs_dir, NULL);
	if (IS_ERR_OR_NULL(priv->debugfs))
		return -ENOMEM;

	if (!debugfs_create_file("wan_xpcs_serdes", 0600, priv->debugfs,
				 priv, &intel_wan_xpcs_fops))
		goto __debugfs_err;

	return 0;

__debugfs_err:
	debugfs_remove_recursive(priv->debugfs);
	return -ENOMEM;
}

static int intel_wan_xpcs_phy_debugfs_exit(struct intel_wan_xpcs_phy *priv)
{
	debugfs_remove_recursive(priv->debugfs);
	priv->debugfs = NULL;
	return 0;
}
#else
static int intel_wan_xpcs_phy_debugfs_init(struct intel_wan_xpcs_phy *priv)
{
	return 0;
}

static int intel_wan_xpcs_phy_debugfs_exit(struct intel_wan_xpcs_phy *priv)
{
	return 0;
}
#endif /* CONFIG_DEBUG_FS */

static int intel_wan_xpcs_phy_init(struct phy *phy)
{
	struct intel_wan_xpcs_phy *priv = phy_get_drvdata(phy);

	dev_dbg(priv->dev, "Initializing intel wan xpcs phy\n");

	/* set WAN_MUX to AON mode */
	regmap_update_bits(priv->syscfg, CHIP_TOP_IFMUX_CFG, WAN_MUX_MASK,
			   WAN_MUX_AON);
	return 0;
}

static int intel_wan_xpcs_phy_power_on(struct phy *phy)
{
	int ret;
	struct intel_wan_xpcs_phy *priv = phy_get_drvdata(phy);
	struct device *dev = priv->dev;

	dev_dbg(priv->dev, "Power on intel wan xpcs phy\n");

	ret = clk_prepare_enable(priv->clk);
	if (ret) {
		dev_err(dev, "Failed to enable PHY gate clock\n");
		return ret;
	}

	if (priv->clk_freq)
		clk_set_rate(priv->clk, priv->clk_freq);

	ret = reset_control_deassert(priv->resets[PHY_RST]);
	if (ret) {
		dev_err(dev, "Failed to deassert phy reset\n");
		return ret;
	}

	ret = reset_control_deassert(priv->resets[WANSS_RST]);
	if (ret) {
		dev_err(dev, "Failed to deassert wanss reset\n");
		return ret;
	}
	udelay(2);

	return 0;
}

static int intel_wan_xpcs_phy_power_off(struct phy *phy)
{
	int ret;
	struct intel_wan_xpcs_phy *priv = phy_get_drvdata(phy);
	struct device *dev = priv->dev;

	dev_dbg(priv->dev, "Power off intel xpcs phy\n");

	ret = reset_control_assert(priv->resets[PHY_RST]);
	if (ret) {
		dev_err(dev, "Failed to assert phy reset\n");
		return ret;
	}

	ret = reset_control_assert(priv->resets[WANSS_RST]);
	if (ret) {
		dev_err(dev, "Failed to assert WANSS reset\n");
		return ret;
	}
	clk_disable_unprepare(priv->clk);

	return 0;
}

static int intel_wan_xpcs_phy_dt_parse(struct intel_wan_xpcs_phy *priv)
{
	struct device *dev = priv->dev;
	struct device_node *np = dev->of_node;
	struct resource *res;
	int i;

	priv->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(priv->clk)) {
		dev_err(dev, "Failed to retrieve clk\n");
		return PTR_ERR(priv->clk);
	}

	if (device_property_read_u32(dev, "clock-frequency", &priv->clk_freq))
		priv->clk_freq = 0;

	for (i = 0; i < MAX_RST; i++) {
		priv->resets[i] = devm_reset_control_get(dev, rst_name[i]);
		if (IS_ERR(priv->resets[i])) {
			dev_err(dev, "Failed to retrieve rst controller\n");
			return PTR_ERR(priv->resets[i]);
		}
	}

	/* get chiptop regmap */
	priv->syscfg =
		syscon_regmap_lookup_by_phandle(np,
						"intel,syscon");
	if (IS_ERR(priv->syscfg)) {
		dev_err(dev, "No phandle specified for xpcs-phy syscon\n");
		return PTR_ERR(priv->syscfg);
	}

	res = platform_get_resource(priv->pdev, IORESOURCE_MEM, 0);

	if (!res) {
		dev_err(dev, "Failed to get wan_serdes iomem resource!\n");
		return PTR_ERR(res);
	}

	priv->base = devm_ioremap_resource(dev, res);

	if (IS_ERR(priv->base)) {
		dev_err(dev, "Failed to ioremap resource: %p\n", res);
		return PTR_ERR(priv->base);
	}

	return 0;
}

static const struct phy_ops ops = {
	.init		= intel_wan_xpcs_phy_init,
	.power_on	= intel_wan_xpcs_phy_power_on,
	.power_off	= intel_wan_xpcs_phy_power_off,
	.owner		= THIS_MODULE,
};

static int intel_wan_xpcs_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct intel_wan_xpcs_phy *priv;
	struct phy_provider *phy_provider;
	int ret;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	priv->dev = dev;
	priv->pdev = pdev;
	platform_set_drvdata(pdev, priv);

	ret = intel_wan_xpcs_phy_dt_parse(priv);
	if (ret)
		return ret;

	priv->phy = devm_phy_create(dev, dev->of_node, &ops);
	if (IS_ERR(priv->phy)) {
		dev_err(dev, "Failed to create PHY\n");
		return PTR_ERR(priv->phy);
	}

	phy_set_drvdata(priv->phy, priv);

	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(phy_provider)) {
		dev_err(dev, "Failed to register phy provider!\n");
		return PTR_ERR(phy_provider);
	}

	if (intel_wan_xpcs_phy_debugfs_init(priv))
		return -EINVAL;

	return 0;
}

static int intel_wan_xpcs_phy_remove(struct platform_device *pdev)
{
	return intel_wan_xpcs_phy_debugfs_exit(platform_get_drvdata(pdev));
}

static const struct of_device_id of_intel_wan_xpcs_phy_match[] = {
	{ .compatible = "intel,wanxpcsphy-prx300" },
	{}
};
MODULE_DEVICE_TABLE(of, of_intel_wan_xpcs_phy_match);

static struct platform_driver intel_wan_xpcs_phy_driver = {
	.probe = intel_wan_xpcs_phy_probe,
	.remove = intel_wan_xpcs_phy_remove,
	.driver = {
		.name = "intel-wan-xpcs-phy",
		.of_match_table = of_match_ptr(of_intel_wan_xpcs_phy_match),
	}
};

builtin_platform_driver(intel_wan_xpcs_phy_driver);

MODULE_AUTHOR("Peter Harliman Liem <peter.harliman.liem@intel.com>");
MODULE_DESCRIPTION("Intel WAN XPCS PHY driver");
MODULE_LICENSE("GPL v2");
