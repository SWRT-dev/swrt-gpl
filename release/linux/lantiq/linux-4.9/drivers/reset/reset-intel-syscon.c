// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (C) 2017 Intel Corporation.
 * Lei Chuanhua <Chuanhua.lei@intel.com>
 */
#include <linux/bitops.h>
#include <linux/io.h>
#include <linux/init.h>
#include <linux/mfd/syscon.h>
#include <linux/of_device.h>
#include <linux/platform_device.h>
#include <linux/reboot.h>
#include <linux/regmap.h>
#include <linux/reset-controller.h>

#define RCU_RST_STAT		0x0024
#define RCU_RST_REQ		0x0048

/* reset platform data */
#define to_reset_data(x)	container_of(x, struct intel_reset_data, rcdev)

struct intel_reset_soc_data {
	int legacy;
};

struct intel_reset_data {
	struct reset_controller_dev rcdev;
	struct notifier_block restart_nb;
	struct device *dev;
	struct regmap *regmap;
	const struct intel_reset_soc_data *soc_data;
	u32 reboot_id;
};

static u32 intel_stat_reg_off(struct intel_reset_data *data, u32 req_off)
{
	if (data->soc_data->legacy && req_off == RCU_RST_REQ)
		return RCU_RST_STAT;
	else
		return req_off + 0x4;
}

static int intel_assert_device(struct reset_controller_dev *rcdev,
			       unsigned long id)
{
	int ret;
	u32 val;
	struct intel_reset_data *data = to_reset_data(rcdev);
	u32 regoff = id >> 8;
	u32 regbit = id & 0x1f;
	u32 regstoff;

	ret = regmap_update_bits(data->regmap, regoff,
				 BIT(regbit), BIT(regbit));
	if (ret) {
		dev_err(data->dev, "Failed to set reset assert bit %d\n", ret);
		return ret;
	}

	regstoff = intel_stat_reg_off(data, regoff);
	return regmap_read_poll_timeout(data->regmap, regstoff, val,
					!!(val & BIT(regbit)), 20, 200);
}

static int intel_deassert_device(struct reset_controller_dev *rcdev,
				 unsigned long id)
{
	int ret;
	u32 val;
	struct intel_reset_data *data = to_reset_data(rcdev);
	u32 regoff = id >> 8;
	u32 regbit = id & 0x1f;
	u32 regstoff;

	ret = regmap_update_bits(data->regmap, regoff,
				 BIT(regbit), 0 << regbit);
	if (ret) {
		dev_err(data->dev,
			"Failed to set reset deassert bit %d\n", ret);
		return ret;
	}
	regstoff = intel_stat_reg_off(data, regoff);
	return regmap_read_poll_timeout(data->regmap, regstoff, val,
					!(val & BIT(regbit)), 20, 200);
}

static int intel_reset_device(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	struct intel_reset_data *data = to_reset_data(rcdev);
	u32 regoff = id >> 8;
	u32 regbit = id & 0x1f;
	u32 regstoff;
	unsigned int val = 0;
	int ret;

	ret = regmap_write(data->regmap, regoff, BIT(regbit));
	if (ret)
		return ret;

	regstoff = intel_stat_reg_off(data, regoff);
	return regmap_read_poll_timeout(data->regmap, regstoff, val,
					val & BIT(regbit), 20, 20000);
}

static int intel_reset_status(struct reset_controller_dev *rcdev,
			      unsigned long id)
{
	struct intel_reset_data *data = to_reset_data(rcdev);
	u32 regoff = id >> 8;
	u32 regbit = id & 0x1f;
	u32 regstoff;
	unsigned int val;
	int ret;

	regstoff = intel_stat_reg_off(data, regoff);
	ret = regmap_read(data->regmap, regstoff, &val);
	if (ret)
		return ret;

	return !!(val & BIT(regbit));
}

static const struct reset_control_ops intel_reset_ops = {
	.reset = intel_reset_device,
	.assert = intel_assert_device,
	.deassert = intel_deassert_device,
	.status = intel_reset_status,
};

static int intel_reset_xlate(struct reset_controller_dev *rcdev,
			     const struct of_phandle_args *spec)
{
	unsigned int offset, bit;

	offset = spec->args[0];
	bit = spec->args[1];

	return (offset << 8) | (bit & 0x1f);
}

static int intel_reset_restart_handler(struct notifier_block *nb,
				       unsigned long action, void *data)
{
	struct intel_reset_data *reset_data =
		container_of(nb, struct intel_reset_data, restart_nb);

	intel_assert_device(&reset_data->rcdev, reset_data->reboot_id);

	return NOTIFY_DONE;
}

static int intel_reset_probe(struct platform_device *pdev)
{
	int err;
	struct device_node *np = pdev->dev.of_node;
	struct device *dev = &pdev->dev;
	struct intel_reset_data *data;
	struct regmap *regmap;
	u32 rb_id[2];

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	regmap = syscon_node_to_regmap(np);
	if (IS_ERR(regmap)) {
		dev_err(dev, "Failed to get reset controller regmap\n");
		return PTR_ERR(regmap);
	}

	data->soc_data = of_device_get_match_data(dev);
	if (!data->soc_data) {
		dev_err(dev, "Failed to find soc data!\n");
		return -ENODEV;
	}

	if (device_property_read_u32_array(dev,
					   "intel,global-reset", rb_id, 2)) {
		dev_err(dev, "Failed to get global reset offset!\n");
		return -EINVAL;
	}

	data->dev = dev;
	data->reboot_id = (rb_id[0] << 8) | rb_id[1];
	data->regmap = regmap;
	data->rcdev.of_node = np;
	data->rcdev.owner = dev->driver->owner;
	data->rcdev.ops = &intel_reset_ops;
	data->rcdev.of_reset_n_cells = 2;
	data->rcdev.of_xlate = intel_reset_xlate;

	err = devm_reset_controller_register(&pdev->dev, &data->rcdev);
	if (err)
		return err;

	data->restart_nb.notifier_call = intel_reset_restart_handler;
	data->restart_nb.priority = 128;

	err = register_restart_handler(&data->restart_nb);
	if (err)
		dev_warn(dev, "Failed to register restart handler\n");

	return 0;
}

struct intel_reset_soc_data grx500_data = {
	.legacy = 1,
};

/* If some SoCs have different property, customized in data field */
static const struct of_device_id intel_reset_match[] = {
	{
		.compatible = "lantiq,rcu-grx500",
		.data = &grx500_data,
	},
	{ /* sentinel */ },
};

static struct platform_driver intel_reset_driver = {
	.probe = intel_reset_probe,
	.driver = {
		.name = "intel-reset-syscon",
		.of_match_table = of_match_ptr(intel_reset_match),
	},
};

static int __init intel_reset_init(void)
{
	return platform_driver_register(&intel_reset_driver);
}
postcore_initcall(intel_reset_init);
