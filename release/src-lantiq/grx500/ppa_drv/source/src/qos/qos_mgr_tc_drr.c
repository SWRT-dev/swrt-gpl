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

#define MAX_QUANTUM 255 * 4096
#define KB 1024
#define MIN_WEIGHT 1
#define MAX_WEIGHT 255

static int32_t qos_mgr_find_and_store_drr_handle(struct _tc_qos_mgr_db *dev_handle,
						uint32_t handle,
						struct tc_drr_qopt_offload *drr)
{
	int32_t i = 0, j = 0;
	struct qos_mgr_tc_csched *sched = NULL;
	/* this is not root drr qdisc,find the node which has drr parent as
	 * handle and create and store child wrr schedular and attached
	 * to that node
	 */
	if ((drr->parent != TC_H_ROOT) && (drr->set_params.quantum == 0)) {
		for (j = 0; j < QOS_MGR_MAX_QUEUE_IFACE; j++) {
			if (dev_handle->qos_info.q_info[j].p_handle ==
								drr->parent)
				break;
		}
		if (j >= QOS_MGR_MAX_QUEUE_IFACE) {
			pr_debug("drr handle not found\n");
			return QOS_MGR_FAILURE;
		}
		sched = (struct qos_mgr_tc_csched *)qos_mgr_malloc(
				sizeof(struct qos_mgr_tc_csched));
		if (!sched) {
			pr_debug("creating child wrr schedular fialed\n");
			return QOS_MGR_FAILURE;
		}
		memset(sched, 0, sizeof(struct qos_mgr_tc_csched));
		dev_handle->qos_info.q_info[j].cshed = sched;
		if ((dev_handle->flags & QOS_MGR_TC_SP_WRR) == QOS_MGR_TC_SP_WRR)
			dev_handle->flags |= QOS_MGR_Q_F_SP_WFQ;

		sched->p_handle = drr->parent;
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			sched->q_info[i].p_handle = (handle | (i + 1));
			sched->q_info[i].priority = j;
			sched->q_info[i].queue_id = -1;
			sched->q_info[i].quantum = 0;
			sched->q_info[i].sched_type = QOS_MGR_TC_DRR;
			pr_debug("p_handle:[%x] cshed.q_info[i].priority:[%d]\n",
				sched->q_info[i].p_handle,
				sched->q_info[i].priority);
		}
	} else {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			dev_handle->qos_info.q_info[i].p_handle = (handle | (i + 1));
			dev_handle->qos_info.q_info[i].priority = i;
			dev_handle->qos_info.q_info[i].sched_type = QOS_MGR_TC_DRR;
			pr_debug("p_handle:[%x] dev_handle->qos_info.q_info[i].priority:[%d]\n",
					dev_handle->qos_info.q_info[i].p_handle,
					dev_handle->qos_info.q_info[i].priority);
		}
		memcpy(&dev_handle->qos_info.drr, drr, sizeof(struct tc_drr_qopt_offload));
	}
	return QOS_MGR_SUCCESS;
}

static int32_t qos_mgr_find_valid_drr_handle_store_quantum(
						struct _tc_qos_mgr_db *dev_handle,
						struct tc_drr_qopt_offload *drr)
{
	int32_t i = QOS_MGR_FAILURE;
	uint32_t weight;
	int j = 0;

	if (drr->parent) {
		/* quantum value from 1-4095 should be 1 and
		   anything more than (255 * 4096) should be 255 */
		if (drr->set_params.quantum < (KB * dev_handle->qos_info.quanta))
			weight = MIN_WEIGHT;
		else if (drr->set_params.quantum > MAX_QUANTUM)
			weight = MAX_WEIGHT;
		else
			weight = drr->set_params.quantum /
					(KB * dev_handle->qos_info.quanta);

		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			pr_debug("drr->parent:[%x] dev_handle->qos_info.q_info[i].p_handle:[%x]\n",
				drr->parent, dev_handle->qos_info.q_info[i].p_handle);
			if (drr->handle == dev_handle->qos_info.q_info[i].p_handle) {
				pr_debug("Found Q handler for index %d = %d weight\n", i, weight);
				dev_handle->qos_info.q_info[i].quantum = weight;
				break;
			} else if (dev_handle->qos_info.q_info[i].cshed != NULL) {
				for (j = 0; j < QOS_MGR_MAX_QUEUE_IFACE; j++) {
					pr_debug("p_handle:[%x]\n",
							dev_handle->qos_info.q_info[i].cshed->q_info[j].p_handle);
					if (drr->handle == dev_handle->qos_info.q_info[i].cshed->q_info[j].p_handle) {
						pr_debug("Found Q handler for index %d\n", j);
						dev_handle->qos_info.q_info[i].cshed->q_info[j].quantum = weight;
						return j;
					}
				}
			}
		}
	}
	if (i == QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("<%s> Valid Queue handler not found.\n", __func__);
		return QOS_MGR_FAILURE;
	}
	return i;
}

static int32_t qos_mgr_find_valid_drr_handle_reset_quantum(
						struct _tc_qos_mgr_db *dev_handle,
						struct tc_drr_qopt_offload *drr)
{
	int i = 0;
	int j = 0;
	if (drr->parent) {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++) {
			pr_debug("drr->parent:[%x] dev_handle->qos_info.q_info[i].p_handle:[%x]\n",
				drr->parent, dev_handle->qos_info.q_info[i].p_handle);
			if (drr->handle == dev_handle->qos_info.q_info[i].p_handle) {
				pr_debug("Found Q handler for index %d\n", i);
				dev_handle->qos_info.q_info[i].quantum = 0;
				break;
			} else if (dev_handle->qos_info.q_info[i].cshed != NULL) {
				for (j = 0; j < QOS_MGR_MAX_QUEUE_IFACE; j++) {
					pr_debug("p_handle:[%x]\n",
							dev_handle->qos_info.q_info[i].cshed->q_info[j].p_handle);
					if (drr->handle == dev_handle->qos_info.q_info[i].cshed->q_info[j].p_handle) {
						pr_debug("Found Q handler for index %d\n", j);
						dev_handle->qos_info.q_info[i].cshed->q_info[j].quantum = 0;
						return j;
					}
				}
			}
		}
	}
	if (i == QOS_MGR_MAX_QUEUE_IFACE) {
		pr_debug("<%s> Valid Queue handler not found.\n", __func__);
		return QOS_MGR_FAILURE;
	}
	return QOS_MGR_SUCCESS;
}

static int32_t qos_mgr_delete_drr_handle(struct _tc_qos_mgr_db *dev_handle,
					struct tc_drr_qopt_offload *drr)
{
	int32_t i = 0, j = 0;

	if (dev_handle->qos_info.no_of_queue != 0) {
		pr_debug("All the queues are not yet deleted.\n");
		return QOS_MGR_FAILURE;
	}
	if ((drr->parent != TC_H_ROOT) && (drr->set_params.quantum == 0)) {
		for (j = 0; j < QOS_MGR_MAX_QUEUE_IFACE; j++) {
			if (dev_handle->qos_info.q_info[j].p_handle ==
								drr->parent)
				break;
		}
		if (j >= QOS_MGR_MAX_QUEUE_IFACE) {
			pr_debug("drr handle not found\n");
			return QOS_MGR_FAILURE;
		}
		if (dev_handle->qos_info.q_info[j].cshed != NULL) {
			pr_debug("Delete and free the child wrr schedular memory\n");
			qos_mgr_free(dev_handle->qos_info.q_info[j].cshed);
			dev_handle->qos_info.q_info[j].cshed = NULL;
		}
	} else {
		for (i = 0; i < QOS_MGR_MAX_QUEUE_IFACE; i++)
			dev_handle->qos_info.q_info[i].p_handle = 0;
	}
	/* memset(&dev_handle->qos_info.drr, 0, sizeof(struct tc_drr_qopt_offload)); */

	dev_handle->qos_info.port_id = -1;
	dev_handle->qos_info.deq_idx = 0;
	return QOS_MGR_SUCCESS;
}

int32_t qos_mgr_tc_drr_offload(struct net_device *dev,
			struct tc_drr_qopt_offload *sch_drr)
{
	int32_t err = QOS_MGR_FAILURE;
	struct _tc_qos_mgr_db *db_handle = NULL;
	struct tc_drr_qopt_offload *opt = sch_drr;

	pr_debug("DRR: offload starting\n");

	switch (opt->command) {
	case TC_DRR_REPLACE:
		pr_debug("parent: %#x class/handle: %#x quantum: %#x\n",
			opt->parent, opt->handle, opt->set_params.quantum);

		err = qos_mgr_get_db_handle(dev, 1, &db_handle);
		if (err == QOS_MGR_FAILURE)
			return QOS_MGR_FAILURE;

		if (opt->parent == TC_H_ROOT)
			err = qos_mgr_find_and_store_drr_handle(db_handle,
								opt->handle, opt);
		else if (opt->set_params.quantum == 0x0)
			err = qos_mgr_find_and_store_drr_handle(db_handle,
								opt->handle, opt);
		else
			err = qos_mgr_find_valid_drr_handle_store_quantum(
								db_handle, opt);
		break;
	case TC_DRR_DESTROY:
		pr_debug("parent: %#x class/handle: %#x quantum: %#x\n",
			opt->parent, opt->handle, opt->set_params.quantum);

		err = qos_mgr_get_db_handle(dev, 1, &db_handle);
		if (err == QOS_MGR_FAILURE)
			return QOS_MGR_FAILURE;

		if (opt->parent == TC_H_ROOT || opt->set_params.quantum == 0x0) {
			err = qos_mgr_delete_drr_handle(db_handle, opt);
			if (err == QOS_MGR_SUCCESS)
				err = qos_mgr_free_db_handle(db_handle);
		} else {
			err = qos_mgr_find_valid_drr_handle_reset_quantum(
								db_handle, opt);
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

