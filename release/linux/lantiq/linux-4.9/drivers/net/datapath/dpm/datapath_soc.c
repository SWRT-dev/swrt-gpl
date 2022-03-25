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
#include <linux/kernel.h>
#include <linux/version.h>
#include <linux/if_ether.h>
#include <linux/skbuff.h>
#include <linux/ethtool.h>
#include <net/datapath_api.h>
#include "datapath.h"
#include "datapath_instance.h"
#include "datapath_swdev_api.h"

int request_dp(u32 flag)
{
	struct dp_inst_info info;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32) || \
	IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
	int i = 0;
	u32 mac_ifcnt = gsw_get_mac_subifcnt(0);
#endif

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	info.type = GSWIP32_TYPE;
	info.ver = GSWIP32_VER;
	info.ops[0] = gsw_get_swcore_ops(0);
	info.ops[1] = gsw_get_swcore_ops(0);
	info.mac_ops[0] = NULL;
	info.mac_ops[1] = NULL;
	for (i = 0; i < mac_ifcnt; i++)
		info.mac_ops[i + 2] = gsw_get_mac_ops(0, i + 2);
#elif IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
	info.type = GSWIP31_TYPE;
	info.ver = GSWIP31_VER;
	info.ops[0] = gsw_get_swcore_ops(0);
	info.ops[1] = gsw_get_swcore_ops(0);
	info.mac_ops[0] = NULL;
	info.mac_ops[1] = NULL;
	for (i = 0; i < mac_ifcnt; i++)
		info.mac_ops[i + 2] = gsw_get_mac_ops(0, i + 2);
#else
	info.type = GSWIP30_TYPE;
	info.ver = GSWIP30_VER;
	info.ops[0] = gsw_get_swcore_ops(0);
	info.ops[1] = gsw_get_swcore_ops(1);
#endif
	info.cbm_inst = 0;
	info.qos_inst = 0;
	if (dp_request_inst(&info, flag)) {
		pr_err("dp_request_inst failed\n");
		return -1;
	}
	return 0;
}

int register_dp_cap(u32 flag)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32)
	register_dp_cap_gswip32(0);
#endif

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
	register_dp_cap_gswip31(0);
#endif

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	register_dp_cap_gswip30(0);
#endif
	return 0;
}
