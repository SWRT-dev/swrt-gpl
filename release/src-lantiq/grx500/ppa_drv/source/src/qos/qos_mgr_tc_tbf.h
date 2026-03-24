/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

#include <linux/if.h>

/**
 *  Offload flower flows with mirred action to HW
 *
 *  \param[in] dev		network device.
 *  \param[in] sch_tbf_off	tbf qdisc offload params
 */
int32_t qos_mgr_tbf_offload(
		struct net_device *dev,
		struct tc_tbf_qopt_offload *sch_tbf_off);

int32_t qos_mgr_add_shaper(
		struct net_device *dev,
		uint32_t parentid,
		uint32_t q_id,
		int32_t shaper_index,
		QOS_QUEUE_LIST_ITEM *p_item,
		int32_t flags);

int32_t qos_mgr_del_shaper(
		struct net_device *dev,
		uint32_t parentid,
		uint32_t q_id,
		int32_t shaper_id,
		QOS_QUEUE_LIST_ITEM *p_item,
		int32_t flags);
