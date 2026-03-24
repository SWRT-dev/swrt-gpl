/*************************************************************************
 **
 ** FILE NAME    : qos_mgr_tc_prio.c
 ** PROJECT      : QOS MGR
 ** MODULES      : QOS MGR (TC APIs)
 **
 ** DATE         : 28 APR 2019
 ** AUTHOR       : vijaykumar
 ** DESCRIPTION  : QOS MGR TC PRIO Offload Implementation
 ** COPYRIGHT : Copyright (c) 2019 Intel Corporation
 **
 *************************************************************************/
#include "qos_mgr_tc.h"

static void qos_mgr_store_prio_handle(struct _tc_qos_mgr_db *dev_handle,
					uint32_t handle,
					uint32_t bands,
					struct tc_prio_qopt_offload *prio)
{
	int i = 0;

	for (i = 0; i < bands; i++) {
		dev_handle->qos_info.q_info[i].p_handle = (handle | (i + 1));
		dev_handle->qos_info.q_info[i].priority = i;
		pr_debug("p_handle:[%x] qos_info.q_info[i].priority:[%d]\n",
			dev_handle->qos_info.q_info[i].p_handle,
			dev_handle->qos_info.q_info[i].priority);
	}
	memcpy(&dev_handle->qos_info.prio, prio,
			sizeof(struct tc_prio_qopt_offload));
	dev_handle->qos_info.sched_type = QOS_MGR_TC_PRIO;
}

static int32_t qos_mgr_delete_prio_handle(struct _tc_qos_mgr_db *dev_handle,
		struct tc_prio_qopt_offload *prio,
		uint32_t bands)
{
	int i = 0;

	if (dev_handle->qos_info.no_of_queue != 0) {
		pr_debug("All the queues are not yet deleted.\n");
		return PPA_FAILURE;
	}

	for (i = 0; i < bands; i++)
		dev_handle->qos_info.q_info[i].p_handle = 0;

	return PPA_SUCCESS;
}

int32_t qos_mgr_tc_qdisc_prio_offload(struct net_device *dev,
					u32 handle,
					struct tc_to_netdev *tc)
{
	int32_t err = PPA_FAILURE;
	struct _tc_qos_mgr_db *db_handle = NULL;
	struct tc_prio_qopt_offload *opt = tc->sch_prio;

	pr_debug("PRIO: offload starting\n");
	if (opt->replace_params.bands > QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("tc prio offload - out of range bands\n");
		return err;
	}
	switch (opt->command) {
	case TC_PRIO_REPLACE:
		pr_debug("parent: %#x class/handle: %#x bands: %#x\n",
			opt->parent, opt->handle, opt->replace_params.bands);

		err = qos_mgr_get_db_handle(dev, 1, &db_handle);
		if (err == PPA_FAILURE)
			return PPA_FAILURE;

		qos_mgr_store_prio_handle(db_handle, opt->handle,
					opt->replace_params.bands, opt);
		break;
	case TC_PRIO_DESTROY:
		pr_debug("parent: %#x class/handle: %#x bands: %#x\n",
			opt->parent, opt->handle, opt->replace_params.bands);

		err = qos_mgr_get_db_handle(dev, 1, &db_handle);
		if (err == PPA_FAILURE)
			return PPA_FAILURE;

		err = qos_mgr_delete_prio_handle(db_handle, opt,
						opt->replace_params.bands);
		if (err == PPA_SUCCESS)
			qos_mgr_free_db_handle(db_handle);
		break;
	case TC_PRIO_STATS:
		/* TODO */
		return -EOPNOTSUPP;
	case TC_PRIO_GRAFT:
		/* TODO */
		return -EOPNOTSUPP;
	default:
		break;
	}

	return err;
}
