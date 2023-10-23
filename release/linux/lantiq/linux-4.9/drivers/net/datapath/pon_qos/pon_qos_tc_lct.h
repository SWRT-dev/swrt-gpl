/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef _PON_QOS_TC_LCT_
#define _PON_QOS_TC_LCT_
#include <linux/if.h>

struct dp_vlan0;
struct dp_tc_vlan;

/**
 *  Configure LCT associated with device for LAN interfaces.
 *  Update "rule" for bridge port reassignment (working only for CTP).
 *
 *  \param[in] dev	Network device.
 *  \param[out] dp_vlan VLAN configuration for base device.
 *  \param[out] rule	Untagged extended VLAN rule.
 */
int32_t pon_qos_lct_configure(struct net_device *dev,
			      struct dp_tc_vlan *dp_vlan,
			      struct dp_vlan0 *rule);

/**
 *  Cleanup allocated resources used for LCT configuration.
 *
 *  \param[in] rule	Untagged extended VLAN rule.
 */
void pon_qos_lct_cleanup(struct dp_vlan0 *rule);

/**
 * Configure LAN interfaces rules associated with LCT on CTP.
 *
 *  \param[in] dev	Network device.
 *  \param[out] dp_vlan VLAN configuration for base device.
 */
void pon_qos_lct_lan_ctp(struct net_device *dev,
			 struct dp_tc_vlan *dp_vlan);
#endif
