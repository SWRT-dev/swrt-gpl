// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

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
	struct core_ops *gsw_ops;
	GSW_register_t reg;

	gsw_ops = gsw_get_swcore_ops(0);
	if (!gsw_ops) {
		pr_err("%s swcore ops is NULL\n", __func__);
		return DP_FAILURE;
	}

	reg.nRegAddr = GSWIP_VER_REG_OFFSET;
	if (gsw_ops->gsw_common_ops.RegisterGet(gsw_ops, &reg))
		return DP_FAILURE;

	if (reg.nData == 0x031) {
		info.type = GSWIP31_TYPE;
		info.ver = GSWIP31_VER;
	} else if (reg.nData == 0x032) {
		info.type = GSWIP32_TYPE;
		info.ver = GSWIP32_VER;
	} else if (reg.nData == 0x030) {
		info.type = GSWIP30_TYPE;
		info.ver = GSWIP30_VER;
	}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP32) || \
	IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
	info.ops[0] = gsw_get_swcore_ops(0);
	info.ops[1] = gsw_get_swcore_ops(0);
	info.mac_ops[0] = NULL;
	info.mac_ops[1] = NULL;
	for (i = 0; i < mac_ifcnt; i++)
		info.mac_ops[i + 2] = gsw_get_mac_ops(0, i + 2);
#else
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
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP30)
	register_dp_cap_gswip30(0);
	return 0;
#endif
	register_dp_capability(0);

	return 0;
}
