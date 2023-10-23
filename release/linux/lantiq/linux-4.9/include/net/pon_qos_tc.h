/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#ifndef _PON_QOS_TC_
#define _PON_QOS_TC_

int pon_qos_setup_tc(struct net_device *dev,
		      u32 handle,
		      __be16 protocol,
		      struct tc_to_netdev *tc,
		      int port_id,
		      int deq_idx);

int pon_qos_setup_tc_gen(struct net_device *dev,
			 u32 handle,
			 __be16 protocol,
			 struct tc_to_netdev *tc);

extern int (*pon_qos_setup_tc_fn)(struct net_device *dev,
			  u32 handle,
			  __be16 protocol,
			  struct tc_to_netdev *tc,
			  int port_id,
			  int deq_idx);
#endif
