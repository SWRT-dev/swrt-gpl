/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef _PON_QOS_TC_EXT_VLAN_
#define _PON_QOS_TC_EXT_VLAN_

struct tc_cls_flower_offload;
struct net_device;
struct seq_file;

int pon_qos_tc_ext_vlan_add(struct net_device *dev,
			    struct tc_cls_flower_offload *f,
			    bool ingress);

int pon_qos_tc_ext_vlan_del(struct net_device *dev,
			    void *vlan_storage,
			    void *rule);

struct list_head *pon_qos_tc_get_ext_vlan_storage(void);

void pon_qos_tc_ext_vlan_debugfs(struct seq_file *file, void *ctx);

#endif
