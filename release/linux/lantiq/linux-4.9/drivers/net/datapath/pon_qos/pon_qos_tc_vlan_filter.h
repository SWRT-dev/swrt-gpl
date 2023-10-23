/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef _PON_QOS_TC_VLAN_FILTER_
#define _PON_QOS_TC_VLAN_FILTER_

int pon_qos_tc_vlan_filter_add(struct net_device *dev,
			       struct tc_cls_flower_offload *f,
			       bool ingress);

int pon_qos_tc_vlan_filter_del(struct net_device *dev,
			       void *vlan_storage,
			       void *rule);

#endif
