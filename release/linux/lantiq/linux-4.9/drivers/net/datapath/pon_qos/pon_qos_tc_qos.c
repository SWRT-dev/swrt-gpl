// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <linux/netdevice.h>
#include <linux/version.h>
#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include <net/pon_qos_tc.h>
#include "pon_qos_tc_qos.h"
#include "pon_qos_tc_qmap.h"
#include "pon_qos_trace.h"

static LIST_HEAD(port_list);

struct pon_qos_port *pon_qos_port_get(struct net_device *dev)
{
	struct pon_qos_port *p, *n;

	list_for_each_entry_safe(p, n, &port_list, list) {
		if (p->dev == dev)
			return p;
	}

	return NULL;
}

struct pon_qos_port *pon_qos_port_alloc(struct net_device *dev)
{
	struct pon_qos_port *port = NULL;

	port = kzalloc(sizeof(*port), GFP_KERNEL);
	if (!port)
		return NULL;

	port->dev = dev;
	INIT_RADIX_TREE(&port->qdiscs, GFP_KERNEL);

	list_add(&port->list, &port_list);

	return port;
}

int pon_qos_port_delete(struct pon_qos_port *port)
{
	list_del(&port->list);
	kfree(port);

	return 0;
}

#define PORT_GREEN_TH (32 * 1024 * 1024)
#define PORT_YELLOW_TH (16 * 1024 * 1024)
static int pon_qos_overwrite_port_thresholds(struct pon_qos_qdisc *sch)
{
	struct dp_port_cfg_info port_cfg_info = {
		.inst = sch->inst,
		.pid = sch->epn,
		.green_threshold = PORT_GREEN_TH,
		.yellow_threshold = PORT_YELLOW_TH,
	};
	int flags = DP_PORT_CFG_GREEN_THRESHOLD | DP_PORT_CFG_YELLOW_THRESHOLD;

	if (dp_qos_port_conf_set(&port_cfg_info, flags) == DP_FAILURE) {
		netdev_err(sch->dev, "%s: dp_qos_port_conf_set() failed\n",
			   __func__);
		return -EINVAL;
	}

	return 0;
}

int pon_qos_fill_port_data(struct pon_qos_qdisc *sch)
{
	dp_subif_t subif = {0};
	struct dp_dequeue_res deq = {0};
	static struct dp_queue_res q_res[PON_QOS_TC_MAX_Q];
	int ret;

	if (sch->deq_idx < 0) {
		ret = dp_get_netif_subifid(sch->dev, NULL, NULL, 0, &subif, 0);
		if (ret < 0)
			return -ENODEV;
		/* This is PON DS port so mark this */
		sch->port = subif.port_id;
		sch->deq_idx = 0;
		sch->ds = true;
	}

	deq.dp_port = sch->port;
	deq.cqm_deq_idx = sch->deq_idx;
	deq.q_res = q_res;
	deq.q_res_size = ARRAY_SIZE(q_res);
	ret = dp_deq_port_res_get(&deq, 0);
	if (ret < 0)
		return ret;

	/* save hw egress port settings */
	sch->inst = subif.inst;
	sch->def_q = q_res[0].q_id;
	sch->epn = deq.cqm_deq_port;

	return 0;
}

int pon_qos_get_port_info(struct pon_qos_qdisc *sch)
{
	dp_subif_t subif = {0};
	int ret = 0;

	if (sch->deq_idx < 0 || sch->port < 0) {
		ret = dp_get_netif_subifid(sch->dev, NULL, NULL, 0, &subif, 0);
		if (ret < 0)
			return -ENODEV;
		/* This is PON DS port so mark this */
		sch->port = subif.port_id;
		sch->deq_idx = 0;
		sch->ds = true;
	}

	return 0;
}

int pon_qos_alloc_qdisc(struct pon_qos_qdisc **qdisc)
{
	struct pon_qos_qdisc *qp;

	qp = kzalloc(sizeof(**qdisc), GFP_KERNEL);
	if (!qp)
		return -ENOMEM;

	*qdisc = qp;

	return 0;
}

void pon_qos_free_qdisc(struct pon_qos_qdisc *qdisc)
{
	kfree(qdisc);
}

struct pon_qos_qdisc *pon_qos_qdisc_find(struct pon_qos_port *port, u32 handle)
{
	return radix_tree_lookup(&port->qdiscs, TC_H_MAJ(handle));
}

int pon_qos_get_sch_by_handle(struct net_device *dev,
			      u32 handle,
			      struct pon_qos_qdisc **sch)
{
	struct pon_qos_port *port = NULL;

	port = pon_qos_port_get(dev);
	if (!port)
		return -ENODEV;

	*sch = pon_qos_qdisc_find(port, handle);
	if (!*sch)
		return -ENODEV;

	return 0;
}

/**
 * This function gets the the queue number from grandparent. For example if
 * we have such hierarchy:
 *
 * tc class add dev tcont32768 parent 500c: handle 500c:3 drr quantum 8k
 * tc qdisc add dev tcont32768 parent 500c:3 handle 1008: red limit 13650 \
 *	min 6820 max 13650 avpkt 1k burst 9 probability 0.004
 * tc qdisc add dev tcont32768 parent 1008:1 handle 2008: red limit 6820 \
 *	min 3410 max 6820 avpkt 1k burst 4 probability 0.004
 * tc filter add dev tcont32768 ingress handle 0x5 protocol all flower \
 *	skip_sw classid 2008:1 indev gem5 action pass
 *
 * Then, we should take the queue from red grandparent (so for 2008:1 we should
 * find red with handle 2008:, then take its parent 1008:
 * and then see that its parent is 500c:3 and the queue should be :3)
 */
static int pon_qos_get_grandparent_queue_by_handle(struct net_device *dev,
						   u32 handle,
						   struct pon_qos_q_data **qid)
{
	struct pon_qos_qdisc *qdisc_parent = NULL;
	struct pon_qos_qdata_params *p;
	struct pon_qos_q_data *qid_iter;
	struct pon_qos_qdisc *qdisc;
	int ret;
	int i;

	ret = pon_qos_get_sch_by_handle(dev, handle, &qdisc);
	if (ret < 0)
		return ret;

	for (i = 0; i < PON_QOS_TC_MAX_Q; i++) {
		qid_iter = &qdisc->qids[i];

		if (!(qid_iter->qid))
			continue;
		list_for_each_entry(p, &qid_iter->params, list) {
			if (TC_H_MAJ(p->handle) != TC_H_MAJ(handle))
				continue;

			ret = pon_qos_get_sch_by_handle(dev, p->parent,
							&qdisc_parent);
			if (ret < 0)
				return ret;
			*qid = pon_qos_qdata_qid_get(dev, qdisc_parent,
						     p->parent);
			if (*qid)
				return 0;
		}
	}
	return -ENOENT;
}

int pon_qos_get_queue_by_handle(struct net_device *dev,
				u32 handle,
				struct pon_qos_q_data **qid)
{
	struct pon_qos_qdisc *qdisc = NULL;
	int idx = TC_H_MIN(handle) - 1;
	int ret;

	ret = pon_qos_get_sch_by_handle(dev, handle, &qdisc);
	if (ret < 0)
		return ret;

	if (idx < 0 || idx >= PON_QOS_TC_MAX_Q)
		return -EINVAL;

	if (!qdisc->qids[idx].qid)
		return -EINVAL;

	*qid = &qdisc->qids[idx];

	return 0;
}

int pon_qos_add_sched(struct pon_qos_qdisc *sch, int prio)
{
	struct dp_node_link node = {0};
	struct dp_node_alloc anode = {0};
	struct dp_node_prio prio_info = {0};
	int ret;

	if (sch->parent != TC_H_ROOT)
		return -EINVAL;

	trace_pon_qos_add_sched_exit(sch, 0);

	/* change def queue prio */
	prio_info.inst = sch->inst;
	prio_info.id.q_id = sch->def_q;
	prio_info.type = DP_NODE_QUEUE;
	ret = dp_qos_link_prio_get(&prio_info, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, " failed to get prio node\n");
		return -EINVAL;
	}

	/* Re-set input and policy */
	prio_info.arbi = ARBITRATION_WSP;
	prio_info.prio_wfq = 7;
	ret = dp_qos_link_prio_set(&prio_info, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "set input of link failed\n");
		return -EINVAL;
	}

	/* Allocate sched */
	anode.inst = sch->inst;
	anode.dp_port = sch->port;
	anode.type = DP_NODE_SCH;
	anode.id.sch_id = DP_NODE_AUTO_ID;
	ret = dp_node_alloc(&anode, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "sch_id alloc fialed\n");
		return -ENOMEM;
	}
	sch->sch_id = anode.id.sch_id;

	/* Link SP/WRR sched to port/sched */
	netdev_dbg(sch->dev, "adding sched id %u\n", node.node_id.sch_id);
	node.inst = sch->inst;
	node.dp_port = sch->port;
	node.p_node_type = DP_NODE_PORT;
	node.p_node_id.cqm_deq_port = sch->epn;
	node.arbi = (sch->type == PON_QOS_QDISC_DRR) ?
		    ARBITRATION_WRR : ARBITRATION_WSP;
	node.prio_wfq = prio;
	node.node_type = DP_NODE_SCH;
	node.node_id.sch_id = sch->sch_id;
	node.cqm_deq_port.cqm_deq_port = sch->epn;
	if (dp_node_link_add(&node, 0) == DP_FAILURE) {
		netdev_err(sch->dev, "failed to link WRR sched to port\n");
		dp_node_free(&anode, DP_NODE_AUTO_FREE_RES);
		return -ENODEV;
	}

	if (sch->deq_idx >= 0) {
		if (pon_qos_overwrite_port_thresholds(sch)) {
			netdev_err(sch->dev, "%s: pon_qos_overwrite_port_thresholds() failed\n",
				   __func__);
			return -EINVAL;
		}
	}

	trace_pon_qos_add_sched_exit(sch, 0);

	return 0;
}

int pon_qos_add_staged_sched(struct pon_qos_qdisc *psch,
			     struct pon_qos_qdisc *csch,
			     int prio)
{
	struct dp_node_link node = {0};
	struct dp_node_alloc anode = {0};
	int ret;

	trace_pon_qos_add_sched_enter(csch, psch->sch_id);

	/* Allocate sched */
	anode.inst = csch->inst;
	anode.dp_port = csch->port;
	anode.type = DP_NODE_SCH;
	anode.id.sch_id = DP_NODE_AUTO_ID;
	ret = dp_node_alloc(&anode, 0);
	if (ret == DP_FAILURE) {
		netdev_err(csch->dev, "sch_id alloc fialed\n");
		return -ENOMEM;
	}
	csch->sch_id = anode.id.sch_id;

	/* Link SP/WRR sched to port/sched */
	/*node.inst = csch->inst;*/
	node.dp_port = csch->port;
	node.p_node_type = DP_NODE_SCH;
	node.p_node_id.sch_id = psch->sch_id;
	node.arbi = (psch->type == PON_QOS_QDISC_DRR) ?
		    ARBITRATION_WRR : ARBITRATION_WSP;
	node.prio_wfq = prio;
	node.node_type = DP_NODE_SCH;
	node.node_id.sch_id = csch->sch_id;
	node.cqm_deq_port.cqm_deq_port = csch->epn;
	netdev_dbg(csch->dev, "%s: adding sched id %u\n",
		   __func__, node.node_id.sch_id);
	if (dp_node_link_add(&node, 0) == DP_FAILURE) {
		netdev_err(csch->dev,
			   "failed to link sched %d to sched %d in: %d\n",
			   csch->sch_id, psch->sch_id, prio);
		dp_node_free(&anode, DP_NODE_AUTO_FREE_RES);
		return -ENODEV;
	}

	trace_pon_qos_add_sched_exit(csch, psch->sch_id);

	return 0;
}

int pon_qos_sched_del(struct pon_qos_qdisc *sch)
{
	struct dp_node_link node = {0};
	struct dp_node_alloc anode = {0};
	int ret;

	trace_pon_qos_sched_del_enter(sch, 0);

	node.node_type = DP_NODE_SCH;
	node.node_id.sch_id = sch->sch_id;
	ret = dp_node_unlink(&node, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "sched id %d unlink failed\n",
			   node.node_id.sch_id);
		return -ENODEV;
	}

	anode.type = DP_NODE_SCH;
	anode.id.sch_id = sch->sch_id;
	ret = dp_node_free(&anode, DP_NODE_AUTO_FREE_RES);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "sched id %d free failed\n",
			   node.node_id.sch_id);
		return -ENODEV;
	}

	sch->use_cnt = 0;

	trace_pon_qos_sched_del_exit(sch, 0);

	return 0;
}

int pon_qos_add_child_qdisc(struct net_device *dev,
			    struct pon_qos_port *port,
			    enum pon_qos_qdisc_type type,
			    u32 parent,
			    u32 handle)
{
	struct pon_qos_qdisc *qdisc = NULL;
	struct pon_qos_qdisc *csch = NULL;
	struct pon_qos_qdisc *psch = NULL;
	int idx = TC_H_MIN(parent) - 1;
	int prio_w = idx;
	int ret;

	if (!dev || !port)
		return -EINVAL;

	if (parent == TC_H_ROOT)
		idx = 0;

	if (idx < 0 || idx >= PON_QOS_TC_MAX_Q)
		return -EINVAL;

	netdev_dbg(dev, "searching qdisc %#x idx: %d\n", parent, idx);
	if (parent != TC_H_ROOT) {
		psch = pon_qos_qdisc_find(port, parent);
		if (!psch) {
			netdev_err(dev, "%s: parent not found\n", __func__);
			return -ENODEV;
		}
	} else {
		psch = &port->root_qdisc;
	}

	/* if there is a queue on this input then delete it */
	if (psch->qids[idx].qid) {
		netdev_dbg(dev, "parent: %#x handle: %#x prio: %d\n",
			   psch->parent, psch->handle, psch->qids[idx].p_w);
		prio_w = psch->qids[idx].p_w;
		ret = pon_qos_queue_del(psch, idx);
		if (ret < 0) {
			netdev_err(dev,
				   "%s: queue delete failed\n", __func__);
			return ret;
		}
	}
	/* if there is a sched on this input return with error for now */
	csch = psch->children[idx];
	if (csch && csch->sch_id) {
		netdev_err(dev, "%s input allocated by sched\n", __func__);
		return -EINVAL;
	}

	if (parent != TC_H_ROOT) {
		/* ready to add the sched */
		ret = pon_qos_alloc_qdisc(&qdisc);
		if (ret < 0)
			return -ENOMEM;

		psch->children[idx] = qdisc;
		psch->num_children++;
		qdisc->p_w = prio_w;
		/* get port data from parent qdisc */
		qdisc->dev = dev;
		qdisc->port = psch->port;
		qdisc->deq_idx = psch->deq_idx;
		qdisc->inst = psch->inst;
		qdisc->def_q = psch->def_q;
		qdisc->epn = psch->epn;
	} else {
		qdisc = psch;
		qdisc->dev = dev;
		qdisc->p_w = PON_QOS_UNUSED;
		ret = pon_qos_fill_port_data(qdisc);
		if (ret < 0) {
			netdev_err(dev,
				   "%s: failed getting port hw config %d\n",
				   __func__, ret);
			goto err_free_qdisc;
		}
	}

	qdisc->type = type;
	qdisc->handle = handle;
	qdisc->parent = parent;
	qdisc->use_cnt = 1;

	if (parent == TC_H_ROOT) {
		ret = pon_qos_add_sched(psch, idx);
		if (ret < 0) {
			netdev_err(dev, "%s: add sched failed\n", __func__);
			goto err_free_qdisc;
		}
		netdev_dbg(dev, "%s: add root sch id %d\n",
			   __func__, psch->sch_id);
	} else {
		netdev_dbg(dev, "root id: %d parent id: %d idx: %d <=> %d\n",
			   port->root_qdisc.sch_id, psch->sch_id, idx, prio_w);
		ret = pon_qos_add_staged_sched(psch, qdisc, prio_w);
		if (ret < 0) {
			netdev_err(dev, "%s: add sched failed\n", __func__);
			goto err_free_qdisc;
		}
	}

	/* add to radix tree here */
	ret = radix_tree_insert(&port->qdiscs, TC_H_MAJ(handle), qdisc);
	if (ret) {
		netdev_err(dev, "qdisc insertion to radix tree failed: %d\n",
			   ret);
		goto err_free_sched;
	}

	return 0;

err_free_sched:
	pon_qos_sched_del(qdisc);
err_free_qdisc:
	if (parent != TC_H_ROOT) {
		netdev_err(dev, "%s: freeing child qdisc\n", __func__);
		psch->children[idx] = NULL;
		psch->num_children--;
		pon_qos_free_qdisc(qdisc);
	}
	return ret;
}

static int pon_qos_sched_policy_update(struct pon_qos_qdisc *sch,
				       int arbi, int prio_w, int idx)
{	struct dp_node_prio prio_info = {0};
	int ret;

	/* update parent sched policy and set prio_w correctly */
	prio_info.id.sch_id = sch->qids[idx].qid;
	prio_info.type = DP_NODE_QUEUE;
	ret = dp_qos_link_prio_get(&prio_info, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "%s :get prio node failed\n", __func__);
		return -EINVAL;
	}

	/* Re-set policy */
	prio_info.arbi = (sch->type == PON_QOS_QDISC_DRR) ?
		    ARBITRATION_WRR : ARBITRATION_WSP;
	ret = dp_qos_link_prio_set(&prio_info, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "%s: set sch arbi failed\n", __func__);
		return -EINVAL;
	}

	prio_info.id.sch_id = sch->qids[idx].qid;
	prio_info.type = DP_NODE_QUEUE;
	ret = dp_qos_link_prio_get(&prio_info, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "%s :get prio node failed\n", __func__);
		return -EINVAL;
	}

	prio_info.prio_wfq = prio_w;
	ret = dp_qos_link_prio_set(&prio_info, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "%s: set sch arbi failed\n", __func__);
		return -EINVAL;
	}

	return 0;
}

#define WRED_GREEN_MAX_TH (500 * 1024)
#define WRED_GREEN_MIN_TH ((WRED_GREEN_MAX_TH * 90) / 100)
#define WRED_GREEN_MAX_P 100
#define WRED_YELLOW_MAX_TH (250 * 1024)
#define WRED_YELLOW_MIN_TH ((WRED_YELLOW_MAX_TH * 90) / 100)
#define WRED_YELLOW_MAX_P 100

#define WRED_UNASSIGNED_TH 500
#define WRED_RED_TH 8000

static int pon_qos_queue_wred_defaults_set(struct pon_qos_qdisc *sch, int idx)
{
	struct dp_queue_conf q_cfg = {
		.q_id = sch->qids[idx].qid,
		.inst = sch->inst
	};

	/** Read */
	if (dp_queue_conf_get(&q_cfg, 0) == DP_FAILURE) {
		netdev_err(sch->dev, "%s: dp_queue_conf_get() failed\n",
			   __func__);
		return -EINVAL;
	}

	/** Modify */
	q_cfg.act = DP_NODE_EN;
	q_cfg.drop = DP_QUEUE_DROP_WRED;

	q_cfg.min_size[0] = WRED_GREEN_MIN_TH;
	q_cfg.max_size[0] = WRED_GREEN_MAX_TH;
	q_cfg.wred_slope[0] = WRED_GREEN_MAX_P;
	q_cfg.min_size[1] = WRED_YELLOW_MIN_TH;
	q_cfg.max_size[1] = WRED_YELLOW_MAX_TH;
	q_cfg.wred_slope[1] = WRED_YELLOW_MAX_P;

	q_cfg.wred_min_guaranteed = WRED_UNASSIGNED_TH;
	q_cfg.wred_max_allowed = WRED_RED_TH;

	/** Write */
	if (dp_queue_conf_set(&q_cfg, 1) == DP_FAILURE) {
		netdev_err(sch->dev, "%s: dp_queue_conf_set() failed\n",
			   __func__);
		return -EINVAL;
	}

	return 0;
}

int pon_qos_queue_add(struct pon_qos_qdisc *sch, int arbi, int prio_w, int idx)
{
	struct dp_node_link node = {0};
	struct dp_node_alloc anode = {0};
	int ret;

	if (!sch)
		return -EINVAL;

	trace_pon_qos_queue_add_enter(sch, &sch->qids[idx], idx);

	anode.inst = sch->inst;
	anode.dp_port = sch->port;
	anode.type = DP_NODE_QUEUE;
	anode.id.q_id = DP_NODE_AUTO_ID;
	ret = dp_node_alloc(&anode, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "failed to alloc queue\n");
		return -ENOMEM;
	}

	sch->qids[idx].qid = anode.id.q_id;
	sch->qids[idx].arbi = arbi;
	sch->qids[idx].p_w = prio_w;
	INIT_LIST_HEAD(&sch->qids[idx].params);

	node.arbi = (arbi == PON_QOS_QDISC_DRR) ?
		    ARBITRATION_WRR : ARBITRATION_WSP;
	node.prio_wfq = prio_w;
	node.node_type = DP_NODE_QUEUE;
	node.node_id.q_id = sch->qids[idx].qid;
	node.p_node_type = DP_NODE_SCH;
	node.p_node_id.sch_id = sch->sch_id;
	if (dp_node_link_add(&node, 0) == DP_FAILURE) {
		netdev_err(sch->dev, "failed to link queue\n");
		anode.type = DP_NODE_QUEUE;
		anode.id.q_id = sch->qids[idx].qid;
		ret = dp_node_free(&anode, DP_NODE_AUTO_FREE_RES);
		if (ret == DP_FAILURE)
			netdev_err(sch->dev, "qid %d free failed\n",
				   anode.id.q_id);
		memset(&sch->qids[idx], 0, sizeof(struct pon_qos_q_data));
		return -ENODEV;
	}

	sch->num_q++;
	ret = pon_qos_sched_policy_update(sch, arbi, prio_w, idx);
	if (ret < 0)
		netdev_err(sch->dev, "%s:policy set fail\n", __func__);

	ret = pon_qos_queue_wred_defaults_set(sch, idx);
	if (ret < 0)
		netdev_err(sch->dev, "%s:wred defaults set fail\n", __func__);

	trace_pon_qos_queue_add_exit(sch, &sch->qids[idx], idx);

	return 0;
}

static int pon_qos_qdata_child_remove(struct net_device *dev,
				      struct pon_qos_q_data *qid,
				      u32 handle);

int pon_qos_queue_del(struct pon_qos_qdisc *sch, int idx)
{
	struct dp_node_alloc anode = {0};
	int ret;

	if (!sch)
		return -EINVAL;

	if (idx < 0 || idx > PON_QOS_TC_MAX_Q - 1)
		return -EINVAL;

	trace_pon_qos_queue_del_enter(sch, &sch->qids[idx], idx);

	ret = pon_qos_qdata_child_remove(sch->dev, &sch->qids[idx],
					 sch->handle);
	if (ret)
		return ret;

	anode.type = DP_NODE_QUEUE;
	anode.id.q_id = sch->qids[idx].qid;
	if (!anode.id.q_id)
		return -EINVAL;

	ret = dp_node_free(&anode, DP_NODE_AUTO_FREE_RES);
	if (ret == DP_FAILURE)
		netdev_err(sch->dev, "qid %d free failed\n",
			   anode.id.q_id);

	trace_pon_qos_queue_del_exit(sch, &sch->qids[idx], idx);

	memset(&sch->qids[idx], 0, sizeof(struct pon_qos_q_data));
	sch->num_q--;

	netdev_dbg(sch->dev, "qid: %i deleted\n", anode.id.q_id);
	return 0;
}

int pon_qos_tc_qdisc_unlink(struct pon_qos_port *p, struct pon_qos_qdisc *sch)
{
	int idx = TC_H_MIN(sch->parent) - 1;
	struct pon_qos_qdisc *psch = NULL;
	int ret = 0;

	if (!p || !sch)
		return -EINVAL;

	if (idx < 0 || idx >= PON_QOS_TC_MAX_Q)
		return -EINVAL;

	psch = pon_qos_qdisc_find(p, sch->parent);
	if (!psch)
		return -ENODEV;

	psch->children[idx] = NULL;
	psch->num_children--;

	/* We have a parent e.g. prio then re-add missing queue/band. */
	ret = pon_qos_queue_add(psch, psch->type, sch->p_w, idx);
	if (ret < 0) {
		netdev_err(psch->dev, "re-add queue fail\n");
		return -ECANCELED;
	}

	return 0;
}

int pon_qos_qdisc_tree_del(struct pon_qos_port *p, struct pon_qos_qdisc *root)
{
	int i, ret;

	for (i = 0; i < PON_QOS_TC_MAX_Q; i++) {
		struct pon_qos_qdisc *sch = root->children[i];

		if (sch && sch->use_cnt) {
			netdev_dbg(sch->dev, "%s: delete child %u of %u\n",
				   __func__,
				   sch->sch_id,
				   root->sch_id);
			ret = pon_qos_qdisc_tree_del(p, sch);
			if (ret < 0)
				return ret;
		}
	}

	netdev_dbg(root->dev, "%s: del sched id: %d use: %d\n",
		   __func__, root->sch_id, root->use_cnt);
	if (!root->use_cnt)
		return 0;

	/* try removing the queues */
	for (i = 0; i < PON_QOS_TC_MAX_Q; i++) {
		if (root->qids[i].qid) {
			netdev_dbg(root->dev, "%s: deleting qid: %d\n",
				   __func__, root->qids[i].qid);
			ret = pon_qos_queue_del(root, i);
			if (ret < 0)
				return ret;
		}
	}

	ret = pon_qos_sched_del(root);
	if (ret < 0)
		return ret;

	p->sch_num--;
	if (root->parent != TC_H_ROOT && TC_H_MIN(root->parent)) {
		ret = pon_qos_tc_qdisc_unlink(p, root);
		if (ret < 0)
			netdev_err(root->dev, "%s: sch[%d] unlink failed\n",
				   __func__, root->sch_id);
	}

	WARN_ON(radix_tree_delete(&p->qdiscs, TC_H_MAJ(root->handle)) != root);
	if (root->parent != TC_H_ROOT)
		pon_qos_free_qdisc(root);
	else
		pon_qos_port_delete(p);

	return 0;
}

static void pon_qos_dump_qdisc(struct pon_qos_qdisc *sch)
{
	netdev_dbg(sch->dev, "hdl:%#x pid:%#x sta:%d numq:%d num sched:%d\n",
		   sch->handle, sch->parent, sch->use_cnt,
		   sch->num_q, sch->num_children);
}

int pon_qos_tc_sched_status(struct pon_qos_port *p, struct pon_qos_qdisc *root)
{
	int i;

	pon_qos_dump_qdisc(root);

	for (i = 0; i < PON_QOS_TC_MAX_Q; i++) {
		struct pon_qos_qdisc *sch = root->children[i];

		if (!sch)
			continue;

		pon_qos_tc_sched_status(p, sch);
	}

	return 0;
}

static int pon_qos_set_qmap(u32 qid, int port, int subif,
			    int tc_cookie, bool en)
{
	struct dp_queue_map_set qmap_set = {0};
	int ret;

	qmap_set.inst = 0;
	qmap_set.q_id = en ? qid : 0;

	qmap_set.map.dp_port = port;
	qmap_set.map.subif = subif;
	qmap_set.mask.flowid = 1;

	/* If tc cookie is not set, assume class is same as subif */
	if (tc_cookie == PON_QOS_TC_COOKIE_EMPTY)
		qmap_set.map.class = subif;
	else
		qmap_set.map.class = tc_cookie;

	if (!pon_qos_is_cpu_port(port)) {
		/* Do not set the flags below for the CPU port otherwise the
		 * lookup table entry for the re-insert queue is overwritten.
		 * The re-inserted packets carry mpe1 = 0 and
		 * mpe = 0 in the DMA descriptor and they have to be kept.
		 */
		qmap_set.mask.mpe1 = 1;
		qmap_set.mask.mpe2 = 1;
	}

	ret = dp_queue_map_set(&qmap_set, 0);
	if (ret == DP_FAILURE) {
		pr_err("%s: queue map set failed\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int pon_qos_config_cpu_port(struct pon_qos_q_data *qid,
				   int port, int subif, char tc_cookie, bool en)
{
	int ret;

	ret = pon_qos_set_qmap(qid->qid, port, subif, tc_cookie, en);
	if (ret != 0)
		return ret;

	if (!en)
		atomic_dec(&qid->ref_cnt);
	else
		atomic_inc(&qid->ref_cnt);

	return ret;
}

static int pon_qos_set_qmap_extraction(struct pon_qos_q_data *qid, int port,
				       bool en)
{
	int ret;
	struct dp_queue_map_set qmap_set = {
		.inst = 0,
		.q_id = en ? qid->qid : 0,
		.map = {
			.dp_port = port,
			.mpe1 = 0,
			.mpe2 = 1,
		},
		.mask = {
			.subif = 1,
			.class = 1,
			.flowid = 1,
		}
	};

	ret = dp_queue_map_set(&qmap_set, 0);
	if (ret == DP_FAILURE) {
		pr_err("%s: queue map set failed\n", __func__);
		return -EINVAL;
	}

	if (!en)
		atomic_dec(&qid->ref_cnt);
	else
		atomic_inc(&qid->ref_cnt);

	return 0;
}

static int pon_qos_force_tc(struct net_device *dev, int pqn, bool en)
{
	struct dp_tc_cfg tc_n = {};
	int ret;

	if (en) {
		tc_n.dev = dev;
		/* Force the TC based on the PQ number i.e.,
		 * PQN = 1 e [1..8] goes to TC = 7 e [0..7]
		 */
		tc_n.tc = (PON_QOS_8021P_HIGHEST_PRIO - (pqn - 1));
		tc_n.force = 1;
	} else {
		tc_n.dev = dev;
		tc_n.tc = 0;
		tc_n.force = 0;
	}

	ret = dp_ingress_ctp_tc_map_set(&tc_n, 0);
	if (ret != 0)
		netdev_err(dev, "%s: force tc fail\n", __func__);

	return ret;
}

static int pon_qos_reassign_tc(struct pon_qos_qdisc *sch, int tc, bool en)
{
	int ret = -1;

	if (en)
		ret = pon_qos_ev_tc_assign(sch, tc);
	else
		ret = pon_qos_ev_tc_unassign(sch, tc);

	return ret;
}

static int pon_qos_map_qid(struct pon_qos_q_data *qid,
			   struct pon_qos_qdisc *sch,
			   int subif,
			   char tc_cookie,
			   bool en)
{
	int ret;

	ret = pon_qos_set_qmap(qid->qid, sch->port, subif, tc_cookie, en);
	if (ret != 0)
		return -EFAULT;

	if (!en)
		atomic_dec(&qid->ref_cnt);
	else
		atomic_inc(&qid->ref_cnt);

	return ret;
}

/* We need this function so that the command:
 *
 *    tc filter add dev eth0_0 ingress pref 1 protocol 802.1q flower \
 *        skip_sw vlan_prio 0 classid 8002:1 action ok
 *
 * would trigger mappings for each subif, which for example effectively means
 * that it will "replicate" the mappings for eth0_0 subinterfaces (like:
 * eth0_0_1, eth0_0_2, ...)
 */
static int pon_qos_replicate_mappings_for_subifs(struct net_device *dev,
						 struct pon_qos_q_data *qid,
						 struct pon_qos_qdisc *sch,
						 int tc, bool en)
{
	struct dp_port_prop prop;
	int i = 0;
	int ret = 0;
	dp_subif_t subif = {0};
	struct net_device *netdev;

	if (dp_get_port_prop(sch->inst, sch->port, &prop))
		return -EINVAL;

	read_lock(&dev_base_lock);

	/* Perform (subif, tc) -> (queue) mappings for each sub interface */
	for_each_netdev(&init_net, netdev) {
		if (dp_get_netif_subifid(netdev, NULL, NULL, NULL, &subif, 0))
			continue;
		if (subif.port_id != sch->port)
			continue;

		/*
		 * If en == false, this should restore the default queue
		 * mappings. If en is false in pon_qos_set_qmap() we will map
		 * to qid=0 which is the drop queue.
		 * So when we stop the omcid the normal traffic is dropped, but
		 * the default setting done by the eth driver is to make the
		 * traffic pass by default.
		 * Therefore we should map to default queue, instead of drop
		 * queue, if en == false.
		 */
		if (en)
			ret = pon_qos_set_qmap(qid->qid, sch->port, subif.subif,
					       tc, en);
		else
			ret = pon_qos_set_qmap(sch->def_q, sch->port,
					       subif.subif, tc, true);
		if (ret)
			goto cleanup;
		i++;
	}

cleanup:
	if (ret) {
		en = !en;
		for_each_netdev(&init_net, netdev) {
			if (dp_get_netif_subifid(netdev, NULL, NULL, NULL, &subif, 0))
				continue;
			if (subif.port_id != sch->port)
				continue;
			if (i <= 0)
				break;

			if (en)
				ret = pon_qos_set_qmap(qid->qid, sch->port,
						       subif.subif, tc, en);
			else
				ret = pon_qos_set_qmap(sch->def_q, sch->port,
						       subif.subif, tc, true);
			i--;
		}
	}

	read_unlock(&dev_base_lock);
	return ret;
}

/* classid option identifies the queue id.
 *
 * Queue mapping scenarios PON upstream:
 *  - GEM port is mapped to specific queue: The GEM port netdev is passed using
 *    the indev option and defines the subif/CTP which is mapped to the qid.
 *    "... flower indev gem1023 classid 100:1 action ok"
 *
 *  - GEM port + Traffic Class to specific queue: The GEM port netdev is passed
 *    using the indev option and defines the subif/CTP and the tc action cookie
 *    defines the traffic class [0..15] which are mapped to the queue id.
 *    "... flower indev gem1029 classid 100:1 action cookie 07 ok"
 *
 * Queue mapping scenarios PON downstream:
 *  - GEM port is mapped to specific queue: The GEM port netdev is passed using
 *    the indev option and defines the subif/CTP which force traffic class
 *    configuration is set according to the classid number i.e. queue priority.
 *    "... flower indev gem1023 classid 100:1 action ok"
 *
 *  - Traffic Class is mapped to specific queue: The traffic class is passed
 *    either using the vlan_prio option [0..7] where each pbit=0 -> TC=0 and so
 *    on or using the tc action cookie.
 *    "... flower classid 100:1 action cookie 07 ok"
 *    "... flower vlan_prio 7 classid 100:1 action ok"
 *
 *  In downstream additionally extVLAN rules to reassign the TC after the vlan
 *  operations are configured. The reassignment maps pbit=0 -> TC=0 and so on.
 *
 *  The above queue mappings can be applied independently for any netdev and
 *  direction if required.
 */

int pon_qos_update_ds_qmap(struct net_device *dev,
			   struct pon_qos_qdisc *sch,
			   struct pon_qos_q_data *qid,
			   struct pon_qos_qmap_tc *q_tc,
			   bool en)
{
	int pqn;
	int ret;

	if (q_tc->indev) {
		/* Get the queue id from the handle. In PON pq = 1 is
		 * the one with the highest priority e.g.
		 * handle 100:1 has higher prio than 100:7
		 */
		pqn = TC_H_MIN(q_tc->handle);
		return pon_qos_force_tc(q_tc->indev, pqn, en);
	}

	ret = pon_qos_reassign_tc(sch, q_tc->tc, en);
	if (ret)
		return ret;

	/* Perform the mapping also for all subinterfaces */
	ret = pon_qos_replicate_mappings_for_subifs(dev, qid, sch, q_tc->tc,
						    en);
	if (ret)
		return ret;

	return 0;
}

int pon_qos_update_us_qmap(struct net_device *dev, struct pon_qos_qdisc *sch,
			   struct pon_qos_q_data *qid,
			   struct pon_qos_qmap_tc *q_tc, bool en)
{
	int ret;

	ret = pon_qos_map_qid(qid, sch, q_tc->tc, q_tc->tc_cookie, en);
	if (ret) {
		netdev_err(dev, "%s: qid map err\n", __func__);
		return ret;
	}

	return 0;
}

/*
 * If mapping is egress and it has indev specified as cpu port,
 * then it is an extraction mapping
 */
static bool pon_qos_is_extraction(struct pon_qos_qmap_tc *q_tc)
{
	if (q_tc->ingress)
		return false;
	if (!q_tc->indev)
		return false;

	return pon_qos_is_netdev_cpu_port(q_tc->indev);
}

/*
 * Does the qmap update for extraction mapping (mapping for which
 * pon_qos_is_extraction() returns true)
 */
static int pon_qos_update_qmap_extraction(struct net_device *dev,
					  struct pon_qos_qmap_tc *q_tc, bool en)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *sch = NULL;
	struct pon_qos_q_data *qid = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port)
		return -ENODEV;

	sch = &port->root_qdisc;

	ret = pon_qos_get_grandparent_queue_by_handle(q_tc->indev, q_tc->handle,
						      &qid);
	if (ret < 0 || !qid) {
		ret = pon_qos_get_queue_by_handle(q_tc->indev, q_tc->handle,
						  &qid);
		if (ret < 0 || !qid) {
			netdev_err(dev, "%s: en=%d, handle %#x not found err %d\n",
				   __func__, en, q_tc->handle, ret);
			return -EINVAL;
		}
	}

	netdev_dbg(dev, "port:%d deq_i:%d q:%d cls:%#x tc:%d/%u ds:%s en:%d\n",
		   sch->port, sch->deq_idx, qid->qid, q_tc->handle,
		   q_tc->tc, q_tc->tc_cookie,
		   sch->ds ? "true" : "false", en);

	trace_pon_qos_update_qmap_enter(dev, sch, qid, q_tc, en);

	ret = pon_qos_set_qmap_extraction(qid, sch->port, en);
	if (ret != 0) {
		netdev_err(dev, "%s: qid map err\n", __func__);
		return -EFAULT;
	}

	trace_pon_qos_update_qmap_exit(dev, sch, qid, q_tc, en);

	return ret;
}

/* Updates the qmap for most of the queue mapping cases (except extraction) */
static int pon_qos_update_qmap_default(struct net_device *dev,
				       struct pon_qos_qmap_tc *q_tc, bool en)
{
	struct pon_qos_port *port = NULL;
	struct pon_qos_qdisc *sch = NULL;
	struct pon_qos_q_data *qid = NULL;
	int ret;

	port = pon_qos_port_get(dev);
	if (!port)
		return -ENODEV;

	sch = &port->root_qdisc;

	ret = pon_qos_get_grandparent_queue_by_handle(dev, q_tc->handle, &qid);
	if (ret < 0 || !qid) {
		ret = pon_qos_get_queue_by_handle(dev, q_tc->handle, &qid);
		if (ret < 0 || !qid) {
			netdev_err(dev, "%s: en=%d, handle %#x not found err %d\n",
				   __func__, en, q_tc->handle, ret);
			return -EINVAL;
		}
	}

	netdev_dbg(dev, "port:%d deq_i:%d q:%d cls:%#x tc:%d/%u ds:%s en:%d\n",
		   sch->port, sch->deq_idx, qid->qid, q_tc->handle,
		   q_tc->tc, q_tc->tc_cookie,
		   sch->ds ? "true" : "false", en);

	trace_pon_qos_update_qmap_enter(dev, sch, qid, q_tc, en);

	/* TODO: DPM uses port zero currently as the CPU port */
	if (pon_qos_is_cpu_port(sch->port)) {
		ret = pon_qos_config_cpu_port(qid, sch->port, 0,
					      q_tc->tc_cookie, en);
		trace_pon_qos_update_qmap_exit(dev, sch, qid, q_tc, en);
		return ret;
	}

	if (sch->ds) {
		ret = pon_qos_update_ds_qmap(dev, sch, qid, q_tc, en);
		trace_pon_qos_update_qmap_exit(dev, sch, qid, q_tc, en);
		return ret;
	}

	ret = pon_qos_update_us_qmap(dev, sch, qid, q_tc, en);
	trace_pon_qos_update_qmap_exit(dev, sch, qid, q_tc, en);

	return ret;
}

int pon_qos_update_qmap(struct net_device *dev, struct pon_qos_qmap_tc *q_tc,
			bool en)
{
	if (pon_qos_is_extraction(q_tc))
		return pon_qos_update_qmap_extraction(dev, q_tc, en);

	return pon_qos_update_qmap_default(dev, q_tc, en);
}

int pon_qos_ports_cleanup(void)
{
	struct pon_qos_port *p, *n;
	int ret;

	list_for_each_entry_safe(p, n, &port_list, list) {
		ret = pon_qos_qdisc_tree_del(p, &p->root_qdisc);
		if (ret < 0) {
			pr_err("%s: error freeing port\n", __func__);
			return ret;
		}
		pon_qos_port_delete(p);
	}

	return 0;
}

int pon_qos_qdata_get_by_type(struct net_device *dev,
			      struct pon_qos_q_data *qid,
			      enum pon_qos_qdata_type type,
			      u32 *handle, u32 *parent)
{
	struct pon_qos_qdata_params *p;

	list_for_each_entry(p, &qid->params, list) {
		if (p->type == type) {
			netdev_dbg(dev, "%s: pid:%#x class/handle:%#x\n",
				   __func__, p->parent, p->handle);
			*handle = p->handle;
			*parent = p->parent;
			return 0;
		}
	}

	netdev_dbg(dev, "%s: type not found\n", __func__);
	return -EINVAL;
}

int pon_qos_qdata_add(struct net_device *dev, struct pon_qos_q_data *qid,
		      u32 handle, u32 parent, enum pon_qos_qdata_type type,
		      int (*destroy)(struct net_device *dev, u32 handle,
				     u32 parent))
{
	struct pon_qos_qdata_params *params;
	u32 tmp_h, tmp_p;

	/* Only one queue setting type allowed */
	if (!pon_qos_qdata_get_by_type(dev, qid, type, &tmp_h, &tmp_p)) {
		netdev_err(dev, "%s: Duplicated setting pid:%#x class/handle:%#x\n",
			   __func__, parent, handle);
		return -ENODEV;
	}

	params = kzalloc(sizeof(*params), GFP_KERNEL);
	if (!params)
		return -ENOMEM;

	params->handle = handle;
	params->parent = parent;
	params->type = type;
	params->destroy = destroy;
	params->qid = qid;

	list_add(&params->list, &qid->params);

	return 0;
}

struct pon_qos_q_data *pon_qos_qdata_qid_get(struct net_device *dev,
					     struct pon_qos_qdisc *qdisc,
					     u32 parent)
{
	struct pon_qos_q_data *qid;
	struct pon_qos_qdata_params *p;
	int idx;

	for (idx = 0; idx < PON_QOS_TC_MAX_Q; idx++) {
		qid = &qdisc->qids[idx];
		if (!(qid->qid))
			continue;
		list_for_each_entry(p, &qid->params, list) {
			if (TC_H_MAJ(p->handle) == TC_H_MAJ(parent))
				return qid;
		}
	}

	return NULL;
}

static int __pon_qos_qdata_remove(struct net_device *dev,
				  struct pon_qos_q_data *qid,
				  struct pon_qos_qdata_params *p);

static int pon_qos_qdata_child_remove(struct net_device *dev,
				      struct pon_qos_q_data *qid,
				      u32 handle)
{
	struct pon_qos_qdata_params *p, *n;
	int ret;

	list_for_each_entry_safe(p, n, &qid->params, list) {
		if (TC_H_MAJ(p->parent) == TC_H_MAJ(handle)) {
			netdev_dbg(dev, "%s: pid:%#x class/handle:%#x\n",
				   __func__, p->parent, p->handle);
			ret = __pon_qos_qdata_remove(dev, qid, p);
			if (ret)
				return ret;
		}
	}

	return 0;
}

static int __pon_qos_qdata_remove(struct net_device *dev,
				  struct pon_qos_q_data *qid,
				  struct pon_qos_qdata_params *p)
{
	int ret;

	netdev_dbg(dev, "%s: pid:%#x class/handle:%#x\n",
		   __func__, p->parent, p->handle);

	ret = pon_qos_tc_classid_unmap(p->handle);
	if (ret)
		return ret;

	/* Remove all successors before removing ancestor */
	ret = pon_qos_qdata_child_remove(dev, qid, p->handle);
	if (ret)
		return ret;

	ret = p->destroy(dev, p->handle, p->parent);
	if (ret) {
		netdev_err(dev, "%s: failed to destroy qdata parameter\n",
			   __func__);
		return ret;
	}

	list_del(&p->list);
	kfree(p);

	return 0;
}

int pon_qos_qdata_remove(struct net_device *dev, struct pon_qos_q_data *qid,
			 u32 handle, u32 parent)
{
	struct pon_qos_qdata_params *p;

	list_for_each_entry(p, &qid->params, list) {
		if (p->handle == handle && p->parent == parent)
			return __pon_qos_qdata_remove(dev, qid, p);
	}

	netdev_dbg(dev, "%s: failed to find qdata parameter\n",
		   __func__);
	return 0;
}

static void pon_qos_shaper_config_init(struct pon_qos_qdisc *sch,
				       struct pon_qos_q_data *qdata,
				       enum dp_shaper_cmd cmd,
				       struct dp_shaper_conf *cfg)
{
	cfg->inst = sch->inst;
	cfg->type = DP_NODE_QUEUE;
	cfg->cmd = cmd;
	cfg->id.q_id = (int)qdata->qid;
}

int pon_qos_shaper_add(struct pon_qos_qdisc *sch,
		       struct pon_qos_q_data *qdata,
		       struct tc_tbf_qopt_offload_params *params)
{
	struct dp_shaper_conf cfg = {0};
	int ret;

	pon_qos_shaper_config_init(sch, qdata, DP_SHAPER_CMD_ADD, &cfg);

	netdev_dbg(sch->dev, "%s: inst: %d type: %d cmd: %d q_id: %d sch_id: %d cqm_deq_port: %d\n",
		   __func__, cfg.inst, cfg.type, cfg.cmd, cfg.id.q_id,
		   cfg.id.sch_id, cfg.id.cqm_deq_port);

	/* Convert from bytes to kilobits */
	cfg.cir = div_u64(params->rate * 8, 1000);
	cfg.pir = div_u64(params->prate * 8, 1000);

	cfg.cbs = params->burst;
	cfg.pbs = params->pburst;

	netdev_dbg(sch->dev, "%s: cir: %d pir: %d cbs: %d pbs: %d\n",
		   __func__, cfg.cir, cfg.pir, cfg.cbs, cfg.pbs);

	ret = dp_shaper_conf_set(&cfg, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "%s: shaper config set failed %d\n",
			   __func__, ret);
		return -EINVAL;
	}

	return 0;
}

int pon_qos_shaper_remove(struct pon_qos_qdisc *sch,
			  struct pon_qos_q_data *qdata)
{
	struct dp_shaper_conf cfg = {0};
	int ret;

	pon_qos_shaper_config_init(sch, qdata, DP_SHAPER_CMD_REMOVE, &cfg);

	netdev_dbg(sch->dev, "%s: inst: %d type: %d cmd: %d q_id: %d sch_id: %d cqm_deq_port: %d\n",
		   __func__, cfg.inst, cfg.type, cfg.cmd, cfg.id.q_id,
		   cfg.id.sch_id, cfg.id.cqm_deq_port);

	ret = dp_shaper_conf_set(&cfg, 0);
	if (ret == DP_FAILURE) {
		netdev_err(sch->dev, "%s: shaper config set failed %d\n",
			   __func__, ret);
		return -EINVAL;
	}

	return 0;
}

bool pon_qos_is_cpu_port(int port)
{
	return port == PON_QOS_CPU_PORT;
}

bool pon_qos_is_netdev_cpu_port(struct net_device *dev)
{
	struct pon_qos_port *port = NULL;

	port = pon_qos_port_get(dev);
	if (!port)
		return false;

	return pon_qos_is_cpu_port(port->root_qdisc.port);
}
