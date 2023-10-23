/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

enum pce_type {
	/* PCE rule will be placed in common block */
	PCE_COMMON = 0,
	/* PCE rule will be placed in CTP block */
	PCE_UNCOMMON = 1,
	PCE_MIRR_VLAN_FWD = 2,
	PCE_MIRR_VLAN_DROP = 3,
	PCE_MIRR_VLAN_WILD = 4,
	PCE_MIRR_UNTAG_FWD = 5,
	PCE_MIRR_UNTAG_DROP = 6,
};

/**
 *  Create PCE rule in hardware.
 *
 *  \param[in]	tc_handle	TC command identifier
 *  \param[in]	pref		TC command preference
 *  \param[in]	pce_rule	PCE rule derived from TC command
 */
int pon_qos_pce_rule_create(struct net_device *dev,
			    u32 tc_handle, int pref,
			    enum pce_type type,
			    GSW_PCE_rule_t *pce_rule);

/**
 *  Delete PCE rule from hardware.
 *
 *  \param[in]	tc_handle	TC command identifier
 *  \param[in]	pref		TC command preference
 */
int pon_qos_pce_rule_delete(u32 tc_handle, int pref);
