// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include <linux/types.h>
#include <net/pkt_cls.h>

int pon_qos_police_offload(struct net_device *dev,
			   struct tc_cls_flower_offload *f,
			   bool ingress);

int pon_qos_police_unoffload(struct net_device *dev,
			     void *meter_cfg_ptr, void *flags_ptr);
