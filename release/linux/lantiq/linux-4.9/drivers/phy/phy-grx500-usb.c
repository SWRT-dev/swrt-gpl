/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2016 ~ 2017 Intel Corporation.
 */

#include <linux/module.h>
#include <linux/platform_device.h>
#include <linux/dma-mapping.h>
#include <linux/phy/phy.h>
#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_address.h>
#include <linux/of_device.h>
#include <linux/clk.h>
#include <linux/interrupt.h>
#include <linux/regulator/consumer.h>
#include <linux/pinctrl/consumer.h>
#include <linux/workqueue.h>
#include <linux/reset.h>

#define SECOND_TIME_DELAY 30

struct ltq_dwc3_phy;

struct ltq_oc_ops {
	int (*enable)(struct ltq_dwc3_phy *);
	int (*disable)(struct ltq_dwc3_phy *);
	int (*init)(struct ltq_dwc3_phy *);
};

struct ltq_dwc3_phy_cfg {
	u8 tx_invert_ovrd;
	u8 rx_invert_ovrd;
	u32 tx_ovrd_offset;
	u32 rx_ovrd_offset;
};

struct ltq_dwc3_phy {
	struct device	*dev;
	void __iomem	*base;
	struct phy *phy;

	struct reset_control *phy_rst;
	struct regulator	*vbus_reg;
	struct clk *clk;

	const struct ltq_dwc3_phy_cfg *cfg;
	bool is_tx_pol_inv;
	bool is_rx_pol_inv;

	/* OCP */
	bool is_enable_oc;
	unsigned int irq;
	struct workqueue_struct *workq;
	struct delayed_work oc_recover_w;
	const struct ltq_oc_ops *oc_ops;
};

static const struct ltq_dwc3_phy_cfg grx500_phy_cfg = {
	.tx_invert_ovrd = 3,
	.rx_invert_ovrd = 1,
	.tx_ovrd_offset = 0x4000,
	.rx_ovrd_offset = 0x4014,
};

static int ltq_grx500_oc_enable(struct ltq_dwc3_phy *dwc3_phy)
{
	struct pinctrl *p;

	p = devm_pinctrl_get_select(
		dwc3_phy->dev, PINCTRL_STATE_IDLE);
	if (IS_ERR(p))
		return PTR_ERR(p);
	msleep(50);

	return 0;
}

static int ltq_grx500_oc_disable(struct ltq_dwc3_phy *dwc3_phy)
{
	struct pinctrl *p;

	p = devm_pinctrl_get_select(
		dwc3_phy->dev, PINCTRL_STATE_DEFAULT);
	if (IS_ERR(p))
		return PTR_ERR(p);
	msleep(50);

	return 0;
}

void ltq_oc_recovery_worker(struct work_struct *work)
{
	struct ltq_dwc3_phy *dwc3_phy = container_of(work,
		struct ltq_dwc3_phy, oc_recover_w.work);

	dev_info(dwc3_phy->dev, "Turn on the power again to recover\n");

	/* Restart OCP to recover */
	dwc3_phy->oc_ops->disable(dwc3_phy);
	dwc3_phy->oc_ops->enable(dwc3_phy);

	irq_set_irq_type(dwc3_phy->irq, IRQ_TYPE_LEVEL_LOW);
}

static irqreturn_t ltq_oc_irq(int irq, void *_dwc3_phy)
{
	struct ltq_dwc3_phy *dwc3_phy = _dwc3_phy;

	dev_info(dwc3_phy->dev, "USB OC protection is triggered\n");

	irq_set_irq_type(irq, IRQ_TYPE_EDGE_FALLING);
	queue_delayed_work(dwc3_phy->workq,
		&dwc3_phy->oc_recover_w, SECOND_TIME_DELAY*HZ);

	return IRQ_HANDLED;
}

static int ltq_grx500_oc_init(struct ltq_dwc3_phy *dwc3_phy)
{
	int ret;
	struct device *dev = dwc3_phy->dev;
	struct platform_device *pdev = to_platform_device(dev);

	dwc3_phy->irq = platform_get_irq(pdev, 0);
	if (dwc3_phy->irq < 0) {
		dev_err(dev, "missing IRQ resource\n");
		return -EINVAL;
	}

	dwc3_phy->workq = create_singlethread_workqueue("usb ocp wq");
	if (!dwc3_phy->workq) {
		dev_err(dev, "create_singlethread_workqueue() failed\n");
		return -ENOMEM;
	}
	INIT_DELAYED_WORK(&dwc3_phy->oc_recover_w, ltq_oc_recovery_worker);

	if (dwc3_phy->vbus_reg) {
		ret = regulator_enable(dwc3_phy->vbus_reg);
		if (ret) {
			dev_err(dev, "regulator enable failed\n");
			return ret;
		}
	}

	ret = devm_request_irq(dev, dwc3_phy->irq, &ltq_oc_irq,
		IRQ_TYPE_LEVEL_LOW, "usb ocp", dwc3_phy);
	if (ret) {
		dev_err(dev, "failed to request IRQ #%d --> %d\n",
			dwc3_phy->irq, ret);
		return ret;
	}

	return 0;
}

static const struct ltq_oc_ops grx500_ltq_oc_ops = {
	.enable = ltq_grx500_oc_enable,
	.disable = ltq_grx500_oc_disable,
	.init = ltq_grx500_oc_init,
};

static inline u32 ltq_dwc3_phy_readl(void __iomem *base, u32 offset)
{
	return readl(base + offset);
}

static inline void ltq_dwc3_phy_writel(
	void __iomem *base, u32 offset, u32 value)
{
	writel(value, base + offset);
}

static void ltq_dwc3_invert_phy_polarity(
	void __iomem *base, u32 offset, u32 set)
{
	u32 value = 0;

	value = ltq_dwc3_phy_readl(base, offset);
	value |= set;
	ltq_dwc3_phy_writel(base, offset, value);
}

static int ltq_dwc3_phy_init(struct phy *phy)
{
	struct ltq_dwc3_phy *dwc3_phy = phy_get_drvdata(phy);

	if (dwc3_phy->is_tx_pol_inv)
		ltq_dwc3_invert_phy_polarity(dwc3_phy->base,
			dwc3_phy->cfg->tx_ovrd_offset,
			BIT(dwc3_phy->cfg->tx_invert_ovrd));
	if (dwc3_phy->is_rx_pol_inv)
		ltq_dwc3_invert_phy_polarity(dwc3_phy->base,
			dwc3_phy->cfg->rx_ovrd_offset,
			BIT(dwc3_phy->cfg->rx_invert_ovrd));

	return 0;
}

static int ltq_dwc3_phy_power_on(struct phy *phy)
{
	int ret;
	struct ltq_dwc3_phy *dwc3_phy = phy_get_drvdata(phy);
	struct device *dev = dwc3_phy->dev;

	ret = clk_prepare_enable(dwc3_phy->clk);
	if (ret) {
		dev_err(dev, "Failed to enable PHY clock\n");
		return ret;
	}
	msleep(100);

	/* RCU RST_REQ2 - USB PHY0/1 released from reset */
	reset_control_deassert(dwc3_phy->phy_rst);
	msleep(100);

	if (dwc3_phy->vbus_reg) {
		ret = regulator_enable(dwc3_phy->vbus_reg);
		if (ret) {
			dev_err(dev, "regulator enable failed\n");
			return ret;
		}
	}

	if (dwc3_phy->is_enable_oc)
		dwc3_phy->oc_ops->enable(dwc3_phy);

	return 0;
}

static int ltq_dwc3_phy_power_off(struct phy *phy)
{
	struct ltq_dwc3_phy *dwc3_phy = phy_get_drvdata(phy);

	if (dwc3_phy->is_enable_oc)
		dwc3_phy->oc_ops->disable(dwc3_phy);

	if (dwc3_phy->vbus_reg)
		regulator_disable(dwc3_phy->vbus_reg);

	reset_control_assert(dwc3_phy->phy_rst);
	clk_disable_unprepare(dwc3_phy->clk);

	return 0;
}

static int ltq_usb_read_dt(struct ltq_dwc3_phy *dwc3_phy,
						struct platform_device *pdev)
{
	struct device *dev = &pdev->dev;

	dwc3_phy->clk = devm_clk_get(dev, NULL);
	if (IS_ERR(dwc3_phy->clk)) {
		dev_err(dev, "Failed to get clk\n");
		return PTR_ERR(dwc3_phy->clk);
	}

	dwc3_phy->vbus_reg = devm_regulator_get(dev, "vbus");
	if (IS_ERR(dwc3_phy->vbus_reg)) {
		dev_err(dev, "Failed to init vbus\n");
		return PTR_ERR(dwc3_phy->vbus_reg);
	}

	dwc3_phy->phy_rst = devm_reset_control_get(dev, "phy");
	if (IS_ERR(dwc3_phy->phy_rst)) {
		dev_err(dev, "Failed to retrieve reset controller\n");
		return PTR_ERR(dwc3_phy->phy_rst);
	}

	dwc3_phy->is_rx_pol_inv = of_property_read_bool(dev->of_node,
													"invert-rx-polarity");
	dwc3_phy->is_tx_pol_inv = of_property_read_bool(dev->of_node,
													"invert-tx-polarity");
	dwc3_phy->is_enable_oc = of_property_read_bool(dev->of_node,
													"enable-over-current");

	return 0;
}

static struct phy_ops ltq_dwc3_phy_ops = {
	.init		= ltq_dwc3_phy_init,
	.power_on	= ltq_dwc3_phy_power_on,
	.power_off	= ltq_dwc3_phy_power_off,
	.owner		= THIS_MODULE,
};

static int ltq_dwc3_phy_probe(struct platform_device *pdev)
{
	struct ltq_dwc3_phy *dwc3_phy;
	struct device *dev = &pdev->dev;
	struct resource		*res;
	void __iomem		*base;
	struct phy_provider *provider;
	int ret;

	dwc3_phy = devm_kzalloc(dev, sizeof(*dwc3_phy), GFP_KERNEL);
	if (!dwc3_phy)
		return -ENOMEM;

	dwc3_phy->cfg = of_device_get_match_data(dev);

	res = platform_get_resource(pdev, IORESOURCE_MEM, 0);
	if (!res) {
		dev_err(dev, "missing memory base resource\n");
		return -EINVAL;
	}

	base = devm_ioremap_nocache(dev, res->start, resource_size(res));
	if (!base) {
		dev_err(dev, "devm_ioremap_nocache failed\n");
		return -ENOMEM;
	}

	ret = ltq_usb_read_dt(dwc3_phy, pdev);
	if (ret)
		return ret;

	dwc3_phy->base = base;
	dwc3_phy->dev = dev;
	if (dwc3_phy->is_enable_oc)
		dwc3_phy->oc_ops = &grx500_ltq_oc_ops;

	platform_set_drvdata(pdev, dwc3_phy);

	dwc3_phy->phy = devm_phy_create(dev, dev->of_node, &ltq_dwc3_phy_ops);
	if (IS_ERR(dwc3_phy->phy)) {
		dev_err(dev, "Failed to create PHY\n");
		return PTR_ERR(dwc3_phy->phy);
	}

	phy_set_drvdata(dwc3_phy->phy, dwc3_phy);

	if (dwc3_phy->is_enable_oc)
		dwc3_phy->oc_ops->init(dwc3_phy);

	provider = devm_of_phy_provider_register(dev, of_phy_simple_xlate);

	return PTR_ERR_OR_ZERO(provider);
}

static const struct of_device_id of_ltq_dwc3_phy_match[] = {
	{
		.compatible = "lantiq,grx500-phy",
		.data = &grx500_phy_cfg,
	},
	{ },
};
MODULE_DEVICE_TABLE(of, of_ltq_dwc3_phy_match);

static struct platform_driver ltq_dwc3_phy_driver = {
	.probe          = ltq_dwc3_phy_probe,
	.driver         = {
		.name   = "ltq-grx500-phy",
		.owner  = THIS_MODULE,
		.pm = NULL,
		.of_match_table	= of_match_ptr(of_ltq_dwc3_phy_match),
	},
};

module_platform_driver(ltq_dwc3_phy_driver);

MODULE_AUTHOR("Arec Kao <arec.kao@intel.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("Intel DWC3 PHY Driver");
