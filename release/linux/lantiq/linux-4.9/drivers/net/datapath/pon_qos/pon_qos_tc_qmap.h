/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#ifndef _PON_QOS_TC_QMAP_
#define _PON_QOS_TC_QMAP_

int pon_qos_tc_map(struct net_device *dev, struct tc_cls_flower_offload *f,
		   bool ingress);
int pon_qos_tc_unmap(struct net_device *dev, void *list_node);
int pon_qos_tc_classid_unmap(u32 classid);

#endif
