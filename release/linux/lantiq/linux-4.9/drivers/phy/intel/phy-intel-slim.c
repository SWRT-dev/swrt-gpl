// SPDX-License-Identifier: GPL-2.0
/*
 * Intel Slim PCIe PHY driver for legacy SoCs
 *
 * Copyright (C) 2018 Intel Corporation.
 */

#include <linux/clk.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/mfd/syscon.h>
#include <linux/of_address.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/platform_device.h>
#include <linux/phy/phy.h>
#include <linux/regmap.h>
#include <linux/reset.h>

/* PCIe 2.0 PDI PHY register definition */
#define PCIE_PHY_CFG0			0xc004
#define PCIE_PHY_CFG1			0xc008
#define PCIE_PHY_CFG2			0xc00c
#define PCIE_PHY_CFG3			0xc010
#define PCIE_PHY_CFG4			0xc014
#define PCIE_PHY_PIPE_PD		14
#define PCIE_PHY_PIPE_PD_O		15

/* xRX500 LCPLL SSC */
#define PCIE_LCPLL_CFG0			0x0094
#define PLL_RESET			BIT(0)
#define LCPLL_CFG0_LOCKED		BIT(1)
#define PCIE_LCPLL_CFG1			0x0098
#define PLL_SSC				BIT(24)
#define PLL_PLK_OV			BIT(20)
#define PCIE_LCPLL_SSC_CTRL		0x009C
#define PCIE_LCPLL_SSC_SCALE		0x00A0
#define PCIE_LCPLL_COEF_0		0x00A4
#define PCIE_LCPLL_COEF_1		0x00A8
#define PCIE_LCPLL_COEF_2		0x00AC
#define PCIE_LCPLL_COEF_3		0x00B0
#define PCIE_LCPLL_COEF_4		0x00B4
#define PCIE_LCPLL_COEF_5		0x00B8
#define PCIE_LCPLL_COEF_6		0x00BC
#define PCIE_LCPLL_COEF_7		0x00C0

/* CHIPTOP REG */
#define PCIE_IFMUX_CFG			0x120
#define PCIE_RCLK_OFF(x)		((x) + 22)
#define PCIE_RCLK(x)			BIT(PCIE_RCLK_OFF(x))

static atomic_t ssc_count;

struct intel_slim_phy {
	int id;
	void __iomem *phy_base;
	struct platform_device *pdev;
	struct device	*dev;
	struct regmap	*ssccfg;
	struct regmap	*phycfg;
	struct phy *phy;
	struct reset_control *rst;
	int ssc_en;
	int ssc_status;
};

enum {
	SSC_DISABLED = 0,
	SSC_ENABLED = 1,
};

static inline u32 phy_r32(void __iomem *base, unsigned int reg)
{
	return readl(base + reg);
}

static inline void phy_w32(void __iomem *base, u32 val, unsigned int reg)
{
	writel(val, base + reg);
}

static inline void phy_w32_off_mask(void __iomem *base, u32 off,
				    u32 mask, u32 set, unsigned int reg)
{
	u32 val;

	val = phy_r32(base, reg) & (~(mask << off));
	val |= (set & mask) << off;
	phy_w32(base, val, reg);
}

static void reg_dbg_dump(struct intel_slim_phy *sphy)
{
	struct device *dev = sphy->dev;
	u32 val;
	int i;

	regmap_read(sphy->ssccfg, PCIE_LCPLL_CFG0, &val);
	dev_dbg(dev, "LCPLL CFG0: 0x%8x\n", val);
	regmap_read(sphy->ssccfg, PCIE_LCPLL_CFG1, &val);
	dev_dbg(dev, "LCPLL CFG1: 0x%8x\n", val);
	regmap_read(sphy->ssccfg, PCIE_LCPLL_SSC_CTRL, &val);
	dev_dbg(dev, "SSC CTRL: 0x%8x\n", val);
	regmap_read(sphy->ssccfg, PCIE_LCPLL_SSC_SCALE, &val);
	dev_dbg(dev, "SSC SCALE: 0x%8x\n", val);

	for (i = 0; i < 8; i++) {
		regmap_read(sphy->ssccfg, PCIE_LCPLL_COEF_0 + i * 4, &val);
		dev_dbg(dev, "LCPLL COEF[%d]: 0x%8x\n", i, val);
	}

	regmap_read(sphy->phycfg, PCIE_IFMUX_CFG, &val);
	dev_dbg(dev, "PCIE PHY IFMUX[22-24]: 0x%8x\n", val);

	dev_dbg(dev, "====================================================\n");
}

static void pcie_phy_power_down(struct intel_slim_phy *sphy)
{
	phy_w32_off_mask(sphy->phy_base,
			 PCIE_PHY_PIPE_PD_O, 1, 1, PCIE_PHY_CFG4);
	mdelay(1);
}

static void pcie_phy_power_up(struct intel_slim_phy *sphy)
{
	phy_w32_off_mask(sphy->phy_base,
			 PCIE_PHY_PIPE_PD_O, 1, 0, PCIE_PHY_CFG4);
	mdelay(1);
}

static void phy_ssc_enable(struct intel_slim_phy *sphy)
{
	int ret;
	u32 val;

	if (sphy->ssc_status == SSC_ENABLED || !sphy->ssc_en) {
		dev_info(sphy->dev,
			 "ssc no enable action: ssc enable: %u, status: %d\n",
			 sphy->ssc_en, sphy->ssc_status);
		return;
	}

	if (atomic_inc_return(&ssc_count) > 1) {
		sphy->ssc_status = SSC_ENABLED;
		dev_info(sphy->dev, "SSC has been enabled, count: %d!\n",
			 atomic_read(&ssc_count));
		return;
	}

	/* Enable SSC and LCPLL */
	regmap_write(sphy->ssccfg, PCIE_LCPLL_CFG1, 0x10003004);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_CFG1, 0x10103004);

	/* Need to bit 0 from 0 to 1 */
	regmap_write(sphy->ssccfg, PCIE_LCPLL_CFG0, 0x00000190);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_CFG0, 0x00000191);

	ret = regmap_read_poll_timeout(sphy->ssccfg, PCIE_LCPLL_CFG0, val,
				       !!(val & LCPLL_CFG0_LOCKED), 100,
				       100 * USEC_PER_MSEC);
	if (ret)
		dev_err(sphy->dev, "%s LCPLL not locked yet\n", __func__);

	/* Fixed Coefficient parameters */
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_0, 0x0000FF60);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_1, 0x0000FF60);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_2, 0x0000FF60);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_3, 0x0000FF60);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_4, 0x000000A0);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_5, 0x000000A0);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_6, 0x000000A0);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_COEF_7, 0x000000A0);
	/* Program DIV and len parameters */
	regmap_write(sphy->ssccfg, PCIE_LCPLL_SSC_CTRL, 0x0000FF1C);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_SSC_CTRL, 0x0000FF1F);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_SSC_CTRL, 0x0000FF1C);
	regmap_write(sphy->ssccfg, PCIE_LCPLL_SSC_CTRL, 0x0000FF1D);
	mdelay(1);

	sphy->ssc_status = SSC_ENABLED;
	dev_info(sphy->dev, "PCIe LCPLL SSC Mode Enabled\n");
}

static void phy_ssc_disable(struct intel_slim_phy *sphy)
{
	int ret;
	u32 val;

	if (sphy->ssc_status == SSC_DISABLED || !sphy->ssc_en) {
		dev_info(sphy->dev,
			 "ssc no disable action: ssc enable: %u, status: %d\n",
			 sphy->ssc_en, sphy->ssc_status);
		return;
	}

	if (atomic_dec_return(&ssc_count) > 0) {
		sphy->ssc_status = SSC_DISABLED;
		dev_info(sphy->dev, "SSC in use, count: %d!\n",
			 atomic_read(&ssc_count));
		return;
	}

	/* Disable SSC CTRL */
	regmap_write(sphy->ssccfg, PCIE_LCPLL_SSC_CTRL, 0x0);
	mdelay(1);

	/* Disalbe SSC and LCPLL */
	regmap_update_bits(sphy->ssccfg, PCIE_LCPLL_CFG1,
			   PLL_SSC | PLL_PLK_OV, PLL_SSC);

	/* reset PLL */
	regmap_update_bits(sphy->ssccfg, PCIE_LCPLL_CFG0, PLL_RESET, 0);
	regmap_update_bits(sphy->ssccfg, PCIE_LCPLL_CFG0, PLL_RESET, 1);

	ret = regmap_read_poll_timeout(sphy->ssccfg, PCIE_LCPLL_CFG0, val,
				       !!(val & LCPLL_CFG0_LOCKED), 100,
				       100 * USEC_PER_MSEC);
	if (ret)
		dev_err(sphy->dev, "%s LCPLL not locked yet\n", __func__);

	sphy->ssc_status = SSC_DISABLED;
}

static void phy_reset_deassert(struct intel_slim_phy *sphy)
{
	reset_control_deassert(sphy->rst);
}

static void phy_reset_assert(struct intel_slim_phy *sphy)
{
	reset_control_assert(sphy->rst);
}

static void intel_slim_phy_reset(struct intel_slim_phy *sphy)
{
	phy_reset_assert(sphy);
	phy_reset_deassert(sphy);
	udelay(1);
}

static void phy_ref_clk_enable(struct intel_slim_phy *sphy)
{
	/* 0 is enable, 1 is disable */
	regmap_update_bits(sphy->phycfg, PCIE_IFMUX_CFG,
			   PCIE_RCLK(sphy->id), 0);
}

static void phy_ref_clk_disable(struct intel_slim_phy *sphy)
{
	/* 0 is enable, 1 is disable */
	regmap_update_bits(sphy->phycfg, PCIE_IFMUX_CFG,
			   PCIE_RCLK(sphy->id), PCIE_RCLK(sphy->id));
}

static int intel_slim_phy_init(struct phy *phy)
{
	struct intel_slim_phy *sphy;

	sphy = phy_get_drvdata(phy);

	pcie_phy_power_down(sphy);
	phy_ssc_enable(sphy);
	pcie_phy_power_up(sphy);

	/* Reset PHY */
	intel_slim_phy_reset(sphy);

	phy_ref_clk_enable(sphy);
	reg_dbg_dump(sphy);

	return 0;
}

static int intel_slim_phy_exit(struct phy *phy)
{
	struct intel_slim_phy *sphy;

	sphy = phy_get_drvdata(phy);

	/* PHY down will halt the whole system, ignore PHY */
	phy_ssc_disable(sphy);

	/* phy clk disable */
	phy_ref_clk_disable(sphy);
	reg_dbg_dump(sphy);

	return 0;
}

static const struct phy_ops intel_slim_phy_ops = {
	.init = intel_slim_phy_init,
	.exit = intel_slim_phy_exit,
};

static int intel_pcie_slim_phy_probe(struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;
	struct device_node *np = dev->of_node;
	struct intel_slim_phy *priv;
	struct resource *res;
	struct phy_provider *phy_provider;
	int id;
	u32 prop;

	priv = devm_kzalloc(dev, sizeof(*priv), GFP_KERNEL);
	if (!priv)
		return -ENOMEM;

	id = of_alias_get_id(np, "pciephy");
	if (id < 0) {
		dev_err(dev, "PCIe phy id error!\n");
		return -ENODEV;
	}

	priv->id = id;
	priv->pdev = pdev;
	priv->dev = dev;
	platform_set_drvdata(pdev, priv);

	res = platform_get_resource_byname(pdev, IORESOURCE_MEM, "phy");
	if (!res) {
		dev_err(dev, "Failed to get iomem resource!\n");
		return -EINVAL;
	}

	priv->phy_base = devm_ioremap_resource(dev, res);
	if (!priv->phy_base) {
		dev_err(dev, "Failed to remap phy iomem!\n");
		return -ENOMEM;
	}

	priv->ssccfg = syscon_regmap_lookup_by_phandle(np, "intel,ssc-syscon");
	if (IS_ERR(priv->ssccfg)) {
		dev_err(dev, "No CGU syscon phandle!\n");
		return -EINVAL;
	}

	if (!device_property_read_u32(dev, "intel,ssc_enable", &prop))
		priv->ssc_en = !!prop;
	priv->ssc_status = SSC_DISABLED;

	priv->phycfg = syscon_regmap_lookup_by_phandle(np, "intel,syscon");
	if (IS_ERR(priv->phycfg)) {
		dev_err(dev, "No CHIPTOP syscon phandle!\n");
		return -EINVAL;
	}

	priv->rst = devm_reset_control_get_exclusive(dev, "phy");
	if (IS_ERR(priv->rst)) {
		dev_err(dev, "Failed to get reset!\n");
		return -EINVAL;
	}

	priv->phy = devm_phy_create(dev, np, &intel_slim_phy_ops);
	if (IS_ERR(priv->phy)) {
		dev_err(dev, "Failed to create intel slim-phy!\n");
		goto __phy_err;
	}

	phy_set_drvdata(priv->phy, priv);
	phy_provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);
	if (IS_ERR(phy_provider))
		dev_err(dev, "Failed to register phy provider!\n");

	intel_slim_phy_reset(priv);
	dev_info(dev, "PCIe slim phy[%d] init success!\n", id);

	return 0;

__phy_err:
	regmap_exit(priv->ssccfg);
	return -EINVAL;
}

static const struct of_device_id of_intel_pcie_slim_phy_match[] = {
	{ .compatible = "intel,grx500-pciephy"},
	{}
};

static struct platform_driver intel_pcie_slim_phy_driver = {
	.probe = intel_pcie_slim_phy_probe,
	.driver = {
		.name = "intel-pcie-slim-phy",
		.of_match_table = of_match_ptr(of_intel_pcie_slim_phy_match),
	}
};

builtin_platform_driver(intel_pcie_slim_phy_driver);

