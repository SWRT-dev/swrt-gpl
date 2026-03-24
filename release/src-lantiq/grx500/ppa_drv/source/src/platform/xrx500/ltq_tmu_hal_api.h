#ifndef __TMU_HAL_API_H__20081119_1007__
#define __TMU_HAL_API_H__20160621_1007__
/*******************************************************************************
 **
 ** FILE NAME    : ltq_tmu_hal_api.h
 ** PROJECT      : PPA
 ** MODULES      : PPA API (Routing/Bridging Acceleration APIs)
 **
 ** DATE         : 21 JUN 2016
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : PPA Protocol Stack Hook for TMU HAL API Functions Header
 **                File
 ** COPYRIGHT    : Copyright (c) 2017 Intel Corporation
 ** Copyright (c) 2016 Lantiq Beteiligungs-GmbH & Co. KG
 ** HISTORY
 ** $Date        $Author         $Comment
 ** 21 JUN 2016  PURNENDU GHOSH  Initiate Version
 *******************************************************************************/
/*! \file tmu_hal_api.h
  \brief This file contains: TMU HAL api.
 */
#include <net/ppa/ppa_api_common.h>
#if defined(CONFIG_SOC_GRX500) && CONFIG_SOC_GRX500
#include <net/datapath_api.h>
#endif
#if defined(CONFIG_PPA_TMU_MIB_SUPPORT)
struct tmu_hal_qos_stats {
	uint64_t enqPkts; /* Enqueued packets Count */
	uint64_t enqBytes; /* Enqueued Bytes Count */
	uint64_t deqPkts; /* Dequeued packets Count */
	uint64_t deqBytes; /* Dequeued Bytes Count */
	uint64_t dropPkts; /* Dropped Packets Count */
	uint64_t dropBytes; /* Dropped Bytes Count - UNUSED for now */
	uint32_t qOccPkts; /* Queue Occupancy Packets Count - Only at Queue level */
};
extern int32_t (*tmu_hal_get_csum_ol_mib_hook_fn)(
		struct tmu_hal_qos_stats *csum_mib,
		uint32_t flag);
extern int32_t (*tmu_hal_clear_csum_ol_mib_hook_fn)(
		uint32_t flag);
extern int32_t(*tmu_hal_get_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct tmu_hal_qos_stats *qos_mib,
		uint32_t flag);
extern int32_t (*tmu_hal_clear_qos_mib_hook_fn)(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		uint32_t flag);
#endif
extern int(*tmu_hal_set_checksum_queue_map_hook_fn)
		(uint32_t pmac_port);
extern int(*tmu_hal_set_lro_queue_map_hook_fn)
		(uint32_t pmac_port);
extern int (*tmu_hal_remove_dp_egress_connectivity_hook_fn)
		(struct net_device *netdev, uint32_t pmac_port);
extern int (*tmu_hal_setup_dp_egress_connectivity_hook_fn)
		(struct net_device *netdev, uint32_t pmac_port);
extern int (*tmu_hal_remove_dp_ingress_connectivity_hook_fn)
		(struct net_device *netdev, uint32_t pmac_port);
extern int (*tmu_hal_setup_dp_ingress_connectivity_hook_fn)
		(struct net_device *netdev, uint32_t pmac_port);
#endif  /*  __TMU_HAL_API_H__20160621_1007__*/
