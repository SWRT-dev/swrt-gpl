/*************************************************************************
 **
 ** FILE NAME    : qos_mgr_tc_mqprio.c
 ** PROJECT      : QOS MGR
 ** MODULES      : QOS MGR (TC APIs)
 **
 ** DESCRIPTION  : TC MQPRIO Offload Implementation
 ** COPYRIGHT 	 : Copyright (c) 2019 - 2020 Intel Corporation
 **
 *************************************************************************/
#include "qos_mgr_tc.h"

static int32_t qos_mgr_delete_mqprio_handle(struct _tc_qos_mgr_db *db_handle)
{
	int32_t i = 0;

	if (db_handle->qos_info.no_of_queue != 0) {
		pr_debug("All the queues are not yet deleted.\n");
		return QOS_MGR_FAILURE;
	}

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		db_handle->qos_info.q_info[i].p_handle = 0;
		db_handle->qos_info.q_info[i].priority = 0;
	}
	memset(&db_handle->qos_info.mqprio, 0, sizeof(struct tc_mqprio_qopt));

	db_handle->qos_info.port_id = -1;
	db_handle->qos_info.deq_idx = 0;
	db_handle->flags = 0;

	return QOS_MGR_SUCCESS;
}

static int32_t qos_mgr_find_and_store_mqprio_handle(struct _tc_qos_mgr_db *db_handle,
						uint32_t handle,
						struct tc_mqprio_qopt *mqprio)
{
	int32_t i = 0;

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		db_handle->qos_info.q_info[i].p_handle = (handle | (i + 1));
		db_handle->qos_info.q_info[i].priority = i;
		db_handle->qos_info.q_info[i].sched_type =
								QOS_MGR_TC_MQPRIO;
	}
	db_handle->flags |= QOS_MGR_TC_SP_WRR;
	memcpy(&db_handle->qos_info.mqprio, mqprio, sizeof(struct tc_mqprio_qopt));

	return QOS_MGR_SUCCESS;
}

int32_t qos_mgr_tc_mqprio_offoad(struct net_device *dev,
				u32 handle,
				struct tc_mqprio_qopt *mqprio)
{
	int32_t ret = QOS_MGR_SUCCESS;
	struct _tc_qos_mgr_db *db_handle = NULL;

	ret = qos_mgr_get_db_handle(dev, 1, &db_handle);
	if (ret == QOS_MGR_FAILURE)
		return QOS_MGR_FAILURE;

	if (mqprio->num_tc == 0) {
		pr_debug("MQPRIO DEL\n");
		ret = qos_mgr_delete_mqprio_handle(db_handle);
		if (ret == QOS_MGR_SUCCESS)
			qos_mgr_free_db_handle(db_handle);
	} else if (mqprio->num_tc <= QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("MQPRIO ADD\n");
		db_handle->dev = dev;
		qos_mgr_find_and_store_mqprio_handle(db_handle, handle, mqprio);
	} else {
		pr_err("out of range num_tc\n");
		qos_mgr_free_db_handle(db_handle);
		return QOS_MGR_FAILURE;
	}

	return QOS_MGR_SUCCESS;
}
