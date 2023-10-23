/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#ifndef _PON_QOS_TC_MAIN_
#define _PON_QOS_TC_MAIN_

int pon_qos_tc_drr_offload(struct net_device *dev,
			   void *type_data,
			   int port_id,
			   int deq_idx);

int pon_qos_tc_prio_offload(struct net_device *dev,
			    void *type_data,
			    int port_id,
			    int deq_idx);

int pon_qos_tc_mqprio_offload(struct net_device *dev,
			      u32 handle,
			      void *type_data,
			      int port_id,
			      int deq_idx);

int pon_qos_tc_flower_offload(struct net_device *dev,
			      u32 handle,
			      void *type_data);

int pon_qos_tc_red_offload(struct net_device *dev,
			   void *type_data);

int pon_qos_tc_tbf_offload(struct net_device *dev,
			   void *type_data);

int pon_qos_tc_block_offload(struct net_device *dev,
			     void *type_data);
#endif
