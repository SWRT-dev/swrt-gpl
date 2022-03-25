/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include<linux/init.h>
#include<linux/module.h>
#include <linux/platform_device.h>
#include <linux/of_address.h>
#include <net/datapath_api.h>
#include "datapath.h"

int dp_probe(struct platform_device *pdev)
{
	if (pdev) {
		/*Note: if pdev NULL, it means no device tree support and
		 *datapath lib use non-standard way to call it.
		 *Datapath Device tree example:
		 * datapath {
		 *			 compatible = "lantiq,datapath-lib";
		 *			 a-cell-property = <1 2 3 4>;
		 *		 };
		 *struct device_node *node = pdev->dev.of_node;
		 *platform_get_resource(pdev, IORESOURCE_MEM, i)
		 *of_irq_to_resource_table(node, irqres, CBM_NUM_INTERRUPTS);
		 */
	}
	return dp_init_module();
}

static int dp_release(struct platform_device *pdev)
{
	dp_cleanup_module();
	return 0;
}

static const struct of_device_id dp_match[] = {
	{.compatible = "lantiq,datapath-lib", },
	{},
};

struct platform_driver dp_driver = {
	.probe = dp_probe,
	.remove = dp_release,
	.driver = {
			.name = "dp-lib",
			.owner = THIS_MODULE,
			.of_match_table = dp_match,
		},
};

int __init dp_init(void)
{
	dp_basic_proc();
	return platform_driver_register(&dp_driver);
}

module_init(dp_init);

