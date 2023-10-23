/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef _PON_QOS_TC_VLAN_PREPARE_
#define _PON_QOS_TC_VLAN_PREPARE_

struct tc_cls_flower_offload;

enum tc_flower_vlan_tag {
	TC_VLAN_UNKNOWN = 0,
	TC_VLAN_UNTAGGED = 1,
	TC_VLAN_SINGLE_TAGGED = 2,
	TC_VLAN_DOUBLE_TAGGED = 3,
};

void pon_qos_tc_dp_vlan_prepare(struct net_device *dev,
				struct dp_tc_vlan *dp_vlan,
				bool ingress);

enum tc_flower_vlan_tag
pon_qos_tc_vlan_tag_get(struct net_device *dev,
			struct tc_cls_flower_offload *f);

int pon_qos_tc_vlan_untagged_prepare(struct net_device *dev,
				     struct tc_cls_flower_offload *f,
				     struct dp_tc_vlan *dp_vlan,
				     struct dp_vlan0 *rule);
int pon_qos_tc_vlan_untagged_flt_prepare(struct net_device *dev,
					 struct tc_cls_flower_offload *f,
					 struct dp_tc_vlan *dp_vlan,
					 struct dp_vlan0 *rule);

int pon_qos_tc_vlan_single_tagged_prepare(struct net_device *dev,
					  struct tc_cls_flower_offload *f,
					  struct dp_vlan1 *rule);
int pon_qos_tc_vlan_single_tagged_flt_prepare(struct net_device *dev,
					      struct tc_cls_flower_offload *f,
					      struct dp_vlan1 *rule);

int pon_qos_tc_vlan_double_tagged_prepare(struct net_device *dev,
					  struct tc_cls_flower_offload *f,
					  struct dp_vlan2 *rule);
int pon_qos_tc_vlan_double_tagged_flt_prepare(struct net_device *dev,
					      struct tc_cls_flower_offload *f,
					      struct dp_vlan2 *rule);

#endif
