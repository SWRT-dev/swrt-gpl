/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef _PON_QOS_TC_FLOWER_
#define _PON_QOS_TC_FLOWER_

/*  Access the indev_ifindex member which is the first element in
 *  struct fl_flow_key, but struct fl_flow_key is not defined in
 *  a header file, but only in net/sched/cls_flower.c
 */
#define FL_FLOW_KEY_IFINDEX(f) ((int)*(int *)(f)->key)

enum pon_qos_tc_flower_type {
	TC_TYPE_UNKNOWN = 0,
	TC_TYPE_EXT_VLAN = 1,
	TC_TYPE_VLAN_FILTER = 2,
	TC_TYPE_QUEUE = 3,
	TC_TYPE_MIRRED = 4,
	TC_TYPE_COLMARK = 5,
	TC_TYPE_TRAP = 6,
	TC_TYPE_POLICE = 7,
	TC_TYPE_IP_DROP = 8,
};

int pon_qos_tc_flower_storage_add(struct net_device *dev,
				  unsigned long cookie,
				  enum pon_qos_tc_flower_type type,
				  void *arg1, void *arg2);

#endif
