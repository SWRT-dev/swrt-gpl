/*
 * Copyright (c) 2018, Intel Corporation.
 *
 * Author: Waldemar Rymarkiewicz <waldemarx.rymarkiewicz@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 */
#include <linux/platform_device.h>
#include <linux/err.h>
#include <linux/of.h>
#include <linux/platform_device.h>

static int __init virtual_platdev_init(void)
{
	struct device_node *np;
	int ret;

	for_each_compatible_node(np, NULL, "intel,reg-virt-platdev") {
		struct device_node *reg_np;
		struct platform_device_info devinfo;

		reg_np = of_parse_phandle(np, "regulator", 0);
		if (!reg_np) {
			pr_err("Looking up regulator property in node %s failed",
				reg_np->full_name);
			ret = -ENODEV;
			goto err;
		}

		memset(&devinfo, 0, sizeof(devinfo));
		devinfo.name = "reg-virt-consumer";
		devinfo.data = of_get_property(reg_np, "regulator-name", NULL);
		devinfo.size_data = strlen(devinfo.name) + 1;
		devinfo.id = PLATFORM_DEVID_AUTO;

		of_node_put(reg_np);
		of_node_put(np);

		platform_device_register_full(&devinfo);
	}

err:
	of_node_put(np);
	return ret;
}
late_initcall(virtual_platdev_init);
