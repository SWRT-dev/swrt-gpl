/*************************************************************************
 **
 ** FILE NAME    : qos_mgr_tc_drr.c
 ** PROJECT      : QOS MGR
 ** MODULES      : QOS MGR (TC APIs)
 **
 ** DATE         : 28 APR 2019
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : QOS MGR TC DRR Offload Implementation
 ** COPYRIGHT : Copyright (c) 2019 Intel Corporation
 **
 *************************************************************************/
#include "qos_mgr_tc.h"

static void qos_mgr_find_and_store_drr_handle(struct _tc_qos_mgr_db *dev_handle,
						uint32_t handle,
						struct tc_drr_qopt_offload *drr)
{
	int32_t i = 0;

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		dev_handle->qos_info.q_info[i].p_handle = (handle | (i + 1));
		dev_handle->qos_info.q_info[i].priority = i;
		pr_debug("<%s> p_handle:[%x] dev_handle->qos_info.q_info[i].priority:[%d]\n",
			__func__, dev_handle->qos_info.q_info[i].p_handle, dev_handle->qos_info.q_info[i].priority);
	}
	memcpy(&dev_handle->qos_info.drr, drr, sizeof(struct tc_drr_qopt_offload));
	dev_handle->qos_info.sched_type = QOS_MGR_TC_DRR;

}

static int32_t qos_mgr_find_valid_drr_handle_store_quantum(
						struct _tc_qos_mgr_db *dev_handle,
						struct tc_drr_qopt_offload *drr)
{
	int32_t i = PPA_FAILURE;

	if (drr->parent) {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			pr_debug("drr->parent:[%x] dev_handle->qos_info.q_info[i].p_handle:[%x]\n",
				drr->parent, dev_handle->qos_info.q_info[i].p_handle);
			if (drr->handle == dev_handle->qos_info.q_info[i].p_handle) {
				pr_debug("Found Q handler for index %d\n", i);
				dev_handle->qos_info.q_info[i].quantum = drr->set_params.quantum;
				break;
			}
		}
	}
	if (i == QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("<%s> Valid Queue handler not found.\n", __func__);
		return PPA_FAILURE;
	}
	return i;
}

static int32_t qos_mgr_find_valid_drr_handle_reset_quantum(
						struct _tc_qos_mgr_db *dev_handle,
						struct tc_drr_qopt_offload *drr)
{
	int32_t i = 0;

	if (drr->parent) {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			pr_debug("drr->parent:[%x] dev_handle->qos_info.q_info[i].p_handle:[%x]\n",
				drr->parent, dev_handle->qos_info.q_info[i].p_handle);
			if (drr->handle == dev_handle->qos_info.q_info[i].p_handle) {
				pr_debug("Found Q handler for index %d\n", i);
				dev_handle->qos_info.q_info[i].quantum = 0;
				break;
			}
		}
	}
	if (i == QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("<%s> Valid Queue handler not found.\n", __func__);
		return PPA_FAILURE;
	}
	return PPA_SUCCESS;
}

static int32_t qos_mgr_delete_drr_handle(struct _tc_qos_mgr_db *dev_handle,
					struct tc_drr_qopt_offload *drr)
{
	int32_t i = 0;

	if (dev_handle->qos_info.no_of_queue != 0) {
		pr_debug("All the queues are not yet deleted.\n");
		return PPA_FAILURE;
	}

	for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
		dev_handle->qos_info.q_info[i].p_handle = 0;
	}
	/* memset(&dev_handle->qos_info.drr, 0, sizeof(struct tc_drr_qopt_offload)); */

	dev_handle->qos_info.port_id = -1;
	dev_handle->qos_info.deq_idx = 0;
	return PPA_SUCCESS;
}

int32_t qos_mgr_tc_drr_offload(struct net_device *dev,
				u32 handle,
				struct tc_to_netdev *tc)
{
	int32_t err = PPA_FAILURE;
	struct _tc_qos_mgr_db *db_handle = NULL;
	struct tc_drr_qopt_offload *opt = tc->sch_drr;

	pr_debug("DRR: offload starting\n");

	switch (opt->command) {
		case TC_DRR_REPLACE:
			pr_debug("parent: %#x class/handle: %#x quantum: %#x\n",
					opt->parent, opt->handle, opt->set_params.quantum);

			err = qos_mgr_get_db_handle(dev, 1, &db_handle);
			if (err == PPA_FAILURE)
				return PPA_FAILURE;

			if (opt->parent == 0xffffffff)
				qos_mgr_find_and_store_drr_handle(db_handle, opt->handle, opt);
			else
				err = qos_mgr_find_valid_drr_handle_store_quantum(db_handle, opt);
			break;
		case TC_DRR_DESTROY:
			pr_debug("Destroy\n");
			pr_debug("parent: %#x class/handle: %#x quantum: %#x\n",
					opt->parent, opt->handle, opt->set_params.quantum);

			err = qos_mgr_get_db_handle(dev, 1, &db_handle);
			if (err == PPA_FAILURE)
				return PPA_FAILURE;

			if (opt->parent == 0xffffffff)
				err = qos_mgr_delete_drr_handle(db_handle, opt);
			if (err == PPA_SUCCESS)
				qos_mgr_free_db_handle(db_handle);
			else {
				err = qos_mgr_find_valid_drr_handle_reset_quantum(db_handle, opt);
			}
			break;
		case TC_DRR_STATS:
			/* TODO */
			return -EOPNOTSUPP;
		case TC_DRR_GRAFT:
			/* TODO */
			return -EOPNOTSUPP;
		default:
			break;
	}

	return err;
}

