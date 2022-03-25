/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2016 ~ 2017 Intel Corporation.
 */

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/platform_device.h>
#include <linux/pm_runtime.h>

#include <linux/delay.h>
#include <linux/of.h>
#include <linux/of_platform.h>
#include <linux/of_address.h>

#include <linux/mfd/syscon.h>
#include <linux/regmap.h>

struct dwc3_ltq_syscfg_bits {
	u8 host_endian;
	u8 dev_endian;
};

static const struct dwc3_ltq_syscfg_bits grx500_ltq_syscfg_bits = {
	.host_endian = 10,
	.dev_endian = 9,
};

struct dwc3_ltq {
	struct device		*dev;
	struct regmap	*syscfg;
	unsigned int	syscfg_reg_offset;

	const struct dwc3_ltq_syscfg_bits *reg_bits;
};

static void ltq_usb_syscfg_mask(struct regmap *syscfg, u32 reg_off,
		u32 set, u32 clr)
{
	u32 val = 0;

	regmap_read(syscfg, reg_off, &val);
	val = (val & ~(BIT(clr))) | (BIT(set));
	regmap_write(syscfg, reg_off, val);

}

int ltq_usb_set_axi_host_to_be(struct dwc3_ltq *dwc)
{

	/* Select AXI Host Port Endian Mode.
	 * 0: Little 1: Big
	 */
	ltq_usb_syscfg_mask(
					   dwc->syscfg,
					   dwc->syscfg_reg_offset,
					   dwc->reg_bits->host_endian,
					   dwc->reg_bits->dev_endian);
	msleep(100);

	return 0;
}

static int dwc3_ltq_probe(struct platform_device *pdev)
{
	struct device_node	*node = pdev->dev.of_node;
	struct dwc3_ltq		*dwc;
	struct device		*dev = &pdev->dev;
	const __be32		*offset;

	int	ret = 0;
	int usb_id;

	if (!node) {
		dev_err(dev, "device node not found\n");
		return -EINVAL;
	}

	dwc = devm_kzalloc(dev, sizeof(*dwc), GFP_KERNEL);
	if (!dwc) {
		dev_err(dev, "not enough memory\n");
		return -ENOMEM;
	}

	dwc->reg_bits = of_device_get_match_data(dev);

	usb_id = of_alias_get_id(node, "usb");
	if (usb_id < 0) {
		dev_err(dev, "Failed to get alias id, errno %d\n", usb_id);
		return -ENODEV;
	}

	dwc->syscfg = syscon_regmap_lookup_by_phandle(dev->of_node,
						      "intel,syscon");
	if (IS_ERR(dwc->syscfg)) {
		dev_err(dev, "Failed to retrieve syscon regmap\n");
		return PTR_ERR(dwc->syscfg);
	}

	offset = of_get_address(dev->of_node, 0, NULL, NULL);
	if (!offset) {
		dev_err(dev, "Failed to get syscon reg offset\n");
		return -ENOENT;
	}
	dwc->syscfg_reg_offset = __be32_to_cpu(*offset);

	dwc->dev = dev;
	platform_set_drvdata(pdev, dwc);

	/* CHIP_TOP Set AXI to Big Endian Mode */
	ret = ltq_usb_set_axi_host_to_be(dwc);
	if (ret) {
		dev_err(dwc->dev, "Can't set usb%d host port endian\n", usb_id);
		return -EINVAL;
	}

	ret = of_platform_populate(node, NULL, NULL, dev);
	if (ret) {
		dev_err(dwc->dev, "Unable to populate dwc3 core devices\n");
		return ret;
	}

	return ret;
}

static int dwc3_ltq_remove(struct platform_device *pdev)
{
	struct dwc3_ltq	*dwc = platform_get_drvdata(pdev);

	of_platform_depopulate(dwc->dev);

	return 0;
}

static const struct of_device_id of_dwc3_core_ltq_match[] = {
	{
		.compatible = "lantiq,dwc3-grx500",
		.data = &grx500_ltq_syscfg_bits,
	},
	{ },
};

MODULE_DEVICE_TABLE(of, of_dwc3_core_ltq_match);

static struct platform_driver dwc3_core_driver = {
	.probe		= dwc3_ltq_probe,
	.remove		= dwc3_ltq_remove,
	.driver		= {
		.name	= "ltq-dwc3-grx500",
		.of_match_table	= of_match_ptr(of_dwc3_core_ltq_match),
		.pm	= NULL,
	},
};

module_platform_driver(dwc3_core_driver);

MODULE_ALIAS("platform:dwc3-grx500");
MODULE_AUTHOR("Arec Kao <arec.kao@intel.com>");
MODULE_LICENSE("GPL v2");
MODULE_DESCRIPTION("DesignWare USB3 Intel Glue Layer");
