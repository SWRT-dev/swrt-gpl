#ifndef __QOS_HAL_API_H
#define __QOS_HAL_API_H
/***************************************************************************
 *
 * Copyright (C) 2017-2018 Intel Corporation
 * Copyright (C) 2010-2016 Lantiq Beteiligungs-GmbH & Co. KG
 * Author  : Purnendu Ghosh
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 ***************************************************************************/
/*! \file qos_hal_api.h
  \brief This file contains: QOS HAL Stats API.
 */
#include <net/ppa/ppa_api_common.h>
struct qos_hal_qos_stats {
	uint64_t enqPkts; /* Enqueued packets Count */
	uint64_t enqBytes; /* Enqueued Bytes Count */
	uint64_t deqPkts; /* Dequeued packets Count */
	uint64_t deqBytes; /* Dequeued Bytes Count */
	uint64_t dropPkts; /* Dropped Packets Count */
	uint64_t dropBytes; /* Dropped Bytes Count - UNUSED for now */
	uint32_t qOccPkts; /* Queue Occupancy Packets Count -
				Only at Queue level */
};
extern int32_t (*qos_hal_get_csum_ol_mib_hook_fn)(
		struct qos_hal_qos_stats *csum_mib,
		uint32_t flag);
extern int32_t (*qos_hal_clear_csum_ol_mib_hook_fn)(
		uint32_t flag);
extern int32_t(*qos_hal_get_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct qos_hal_qos_stats *qos_mib,
		uint32_t flag);
extern int32_t (*qos_hal_clear_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		uint32_t flag);

int qos_hal_setup_dp_ingress_connectivity(
		struct net_device *netdev,
		uint32_t pmac_port);
int qos_hal_setup_dp_egress_connectivity(
		struct net_device *netdev,
		uint32_t pmac_port);
int qos_hal_remove_dp_egress_connectivity(
		struct net_device *netdev,
		uint32_t pmac_port);
int qos_hal_remove_dp_ingress_connectivity(
		struct net_device *netdev,
		uint32_t pmac_port);

int32_t qos_mgr_set_mapped_queue(
		struct net_device *netdev,
		int32_t portid,
		int32_t deq_idx,
		uint32_t queue_id, /* Queue Id */
		uint32_t dir, /* 0 - Egress 1 - Ingress */
		uint32_t class, /* Traffic class */
		int32_t flag);

#endif  /*  __QOS_HAL_API_H*/
