/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include <linux/pp_qos_api.h>
#include "../datapath.h"
#include "datapath_misc.h"

#define FLUSH_RESTORE_LOOKUP BIT(0)

static struct limit_map limit_maps[] = {
	{QOS_NO_BANDWIDTH_LIMIT, DP_NO_SHAPER_LIMIT},
	{QOS_MAX_BANDWIDTH_LIMIT, DP_MAX_SHAPER_LIMIT}
};

static struct arbi_map arbi_maps[] = {
	{PP_QOS_ARBITRATION_WSP, ARBITRATION_WSP},
	{PP_QOS_ARBITRATION_WRR, ARBITRATION_WRR},
	{PP_QOS_ARBITRATION_WFQ, ARBITRATION_WFQ},
	{PP_QOS_ARBITRATION_WRR, ARBITRATION_NULL}
};

static void dp_wred_def(struct pp_qos_queue_conf *conf)
{
	if (!conf)
		return;

#if 1

	conf->wred_enable = 0;
	conf->wred_min_guaranteed = DEF_QRED_MIN_ALLOW;
	conf->wred_max_allowed = DEF_QRED_MAX_ALLOW;
#else
	conf->wred_enable = 1;
	conf->wred_fixed_drop_prob_enable = 0;
	conf->wred_min_guaranteed = DEF_QRED_MIN_ALLOW;
	conf->wred_max_allowed = DEF_QRED_MAX_ALLOW;

	conf->wred_fixed_drop_prob_yellow = 0;
	conf->wred_min_avg_yellow = DEF_QRED_MIN_ALLOW/DEF_WRED_RATIO;
	conf->wred_max_avg_yellow = DEF_QRED_MAX_ALLOW/DEF_WRED_RATIO;
	conf->wred_slope_yellow = DEF_QRED_SLOP_YELLOW;

	conf->wred_fixed_drop_prob_green = 0;
	conf->wred_min_avg_green = (DEF_WRED_RATIO - 1) * DEF_QRED_MIN_ALLOW /
				    DEF_WRED_RATIO;
	conf->wred_max_avg_green = (DEF_WRED_RATIO - 1) * DEF_QRED_MAX_ALLOW /
				    DEF_WRED_RATIO;
	conf->wred_slope_green = DEF_QRED_SLOP_GREEN;
#endif
}

int qos_platform_set_32(int cmd_id, void *node, int flag)
{
	struct dp_node_link *node_link = (struct dp_node_link *)node;
	int inst;
	struct hal_priv *priv;
	int res = DP_FAILURE;

	if (!node)
		return DP_FAILURE;
	inst = node_link->inst;
	priv = HAL(inst);
	if (!priv->qdev) {
		pr_err("qdev NULL with inst=%d\n", inst);
		return DP_FAILURE;
	}

	switch (cmd_id) {
	case NODE_LINK_ADD:
		res = dp_node_link_add_32((struct dp_node_link *)node, flag);
		break;
	case NODE_LINK_GET:
		res = dp_node_link_get_32((struct dp_node_link *)node, flag);
		break;
	case NODE_LINK_EN_GET:
		res = dp_node_link_en_get_32((struct dp_node_link_enable *)node,
					     flag);
		break;
	case NODE_LINK_EN_SET:
		res = dp_node_link_en_set_32((struct dp_node_link_enable *)node,
					     flag);
		break;
	case NODE_UNLINK:
		res = dp_node_unlink_32((struct dp_node_link *)node, flag);
		break;
	case LINK_ADD:
		res = dp_link_add_32((struct dp_qos_link *)node, flag);
		break;
	case LINK_GET:
		res = dp_link_get_32((struct dp_qos_link *)node, flag);
		break;
	case LINK_PRIO_SET:
		res = dp_qos_link_prio_set_32((struct dp_node_prio *)node,
					      flag);
		break;
	case LINK_PRIO_GET:
		res = dp_qos_link_prio_get_32((struct dp_node_prio *)node,
					      flag);
		break;
	case QUEUE_CFG_SET:
		res = dp_queue_conf_set_32((struct dp_queue_conf *)node, flag);
		break;
	case QUEUE_CFG_GET:
		res = dp_queue_conf_get_32((struct dp_queue_conf *)node, flag);
		break;
	case SHAPER_SET:
		res = dp_shaper_conf_set_32((struct dp_shaper_conf *)node,
					    flag);
		break;
	case SHAPER_GET:
		res = dp_shaper_conf_get_32((struct dp_shaper_conf *)node,
					    flag);
		break;
	case NODE_ALLOC:
		res = dp_node_alloc_32((struct dp_node_alloc *)node, flag);
		break;
	case NODE_FREE:
		res = dp_node_free_32((struct dp_node_alloc *)node, flag);
		break;
	case NODE_CHILDREN_FREE:
		res =
		dp_free_children_via_parent_32((struct dp_node_alloc *)node,
					       flag);
		break;
	case DEQ_PORT_RES_GET:
		res = dp_deq_port_res_get_32((struct dp_dequeue_res *)node,
					     flag);
		break;
	case COUNTER_MODE_SET:
		res = dp_counter_mode_set_32((struct dp_counter_conf *)node,
					     flag);
		break;
	case COUNTER_MODE_GET:
		res = dp_counter_mode_set_32((struct dp_counter_conf *)node,
					     flag);
		break;
	case QUEUE_MAP_GET:
		res = dp_queue_map_get_32((struct dp_queue_map_get *)node,
					  flag);
		break;
	case QUEUE_MAP_SET:
		res = dp_queue_map_set_32((struct dp_queue_map_set *)node,
					  flag);
		break;
	case NODE_CHILDREN_GET:
		res = dp_children_get_32((struct dp_node_child *)node, flag);
		break;
	case QOS_LEVEL_GET:
		res = dp_qos_level_get_32((struct dp_qos_level *)node, flag);
		break;
	case QOS_GLOBAL_CFG_GET:
		res = dp_qos_global_info_get_32((struct dp_qos_cfg_info *)node,
						flag);
	case QOS_Q_LOGIC:
		res = dp_get_queue_logic_32((struct dp_qos_q_logic *)node,
					    flag);
		break;
	case QOS_PORT_CFG_SET:
		res = dp_qos_port_conf_set_32((struct dp_port_cfg_info *)node,
					      flag);
		break;
	default:
		pr_err("no support yet cmd_id %d\n", cmd_id);
		break;
	}
	return res;
}

#define MBPS_2_KBPS 1000
/* convert pp shaper limit to dp shaper limit */
static int limit_pp2dp(u32 pp_limit, u32 *dp_limit)
{
	int i;

	if (!dp_limit) {
		pr_err("dp_limit is NULL!\n");
		return DP_FAILURE;
	}

	if (pp_limit > QOS_MAX_BANDWIDTH_LIMIT) {
		pr_err("Wrong pp shaper limit: %u\n", pp_limit);
		return DP_FAILURE;
	}

	for (i = 0; i < ARRAY_SIZE(limit_maps); i++) {
		if (limit_maps[i].pp_limit == pp_limit) {
			*dp_limit = limit_maps[i].dp_limit;
			return DP_SUCCESS;
		}
	}
	*dp_limit = pp_limit * MBPS_2_KBPS;/* mbps to kbps */

	if ((*dp_limit <= 0) || (*dp_limit > DP_MAX_SHAPER_LIMIT)) {
		pr_err("Wrong dp shaper limit: %u\n", *dp_limit);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}

/* convert dp shaper limit to pp shaper limit */
static int limit_dp2pp(u32 dp_limit, u32 *pp_limit)
{
	int i;

	if (!pp_limit) {
		pr_err("pp_limit is NULL!\n");
		return DP_FAILURE;
	}

	if ((dp_limit > DP_MAX_SHAPER_LIMIT) || (dp_limit == 0)) {
		pr_err("Wrong dp shaper limit: %u\n", dp_limit);
		return DP_FAILURE;
	}

	for (i = 0; i < ARRAY_SIZE(limit_maps); i++) {
		if (limit_maps[i].dp_limit == dp_limit) {
			*pp_limit = limit_maps[i].pp_limit;
			return DP_SUCCESS;
		}
	}

	if (dp_limit % MBPS_2_KBPS)
		*pp_limit = dp_limit / MBPS_2_KBPS + 1;/* kbps to mbps */
	else
		*pp_limit = dp_limit / MBPS_2_KBPS;

	if (*pp_limit > QOS_MAX_BANDWIDTH_LIMIT) {
		pr_err("Wrong dp shaper limit: %u\n", *pp_limit);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}

/* convert PP arbitrate to DP arbitrate */
int arbi_pp2dp_32(int pp_arbi)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(arbi_maps); i++) {
		if (arbi_maps[i].pp_arbi == pp_arbi)
			return arbi_maps[i].dp_arbi;
	}
	return DP_FAILURE;
}

/* convert DP arbitrate to PP arbitrate */
int arbi_dp2pp_32(int dp_arbi)
{
	int i;

	for (i = 0; i < ARRAY_SIZE(arbi_maps); i++) {
		if (arbi_maps[i].dp_arbi == dp_arbi)
			return arbi_maps[i].pp_arbi;
	}
	pr_err("Wrong dp_arbitrate: %d\n", dp_arbi);
	return DP_FAILURE;
}

/* get_qid_by_node API
 * checks for queue node id
 * upon Success
 *    return physical id of queue
 *    else return DP_FAILURE
 */
static int get_qid_by_node(int inst, int node_id, int flag)
{
	int i;
	struct hal_priv *priv = HAL(inst);

	for (i = 0; i < MAX_QUEUE; i++) {
		if (node_id == priv->qos_queue_stat[i].node_id)
			return i;
	}
	return DP_FAILURE;
}

/* get_cqm_deq_port_by_node API
 * checks for qos deque port
 * upon Success
 *    return physical cqm_deq_port id
 *    else return DP_FAILURE
 */
static int get_cqm_deq_port_by_node(int inst, int node_id, int flag)
{
	int i;
	struct hal_priv *priv = HAL(inst);

	for (i = 0; i < MAX_CQM_DEQ; i++) {
		if (node_id == priv->deq_port_stat[i].node_id)
			return i;
	}
	return DP_FAILURE;
}

#ifndef DP_FLUSH_VIA_AUTO
static int cqm_queue_flush_32(int cqm_inst, int cqm_drop_port, int qid)
{
/* Before call this API, the queue is already unmapped in lookup table,
 * For the queue itself, it is blocked and resume.
 * Also attached to drop port
 */
	/* need call low level CQM API */

	cqm_qos_queue_flush(cqm_inst, cqm_drop_port, qid);

	DP_DEBUG(DP_DBG_FLAG_QOS, "cqm_queue_flush_32 done\n");
	return DP_SUCCESS;
}
#endif

/* Note: When this API is returned,make sure the queue is in suspend/block
 *      since the queue may need to move to other scheduler/port after flush.
 *      node_id is logical node it here
 */
static int queue_flush_32(int inst, int node_id, int flag)
{
	struct hal_priv *priv = HAL(inst);
	struct pp_qos_queue_conf queue_cfg = {0};
#ifdef DP_FLUSH_VIA_AUTO
	u32 qocc;
	int times = 10000;
#else
	struct pp_qos_queue_conf tmp_q_cfg = {0};
#endif
	int qid = get_qid_by_node(inst, node_id, 0);
	int res = DP_SUCCESS;
	struct dp_lookup_entry *lookup = NULL;
	struct cbm_lookup info = {0};

	if (qid < 0) {
		pr_err("no physical qid for q_node=%d\n", node_id);
		res = DP_FAILURE;
		goto EXIT;
	}
	if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
		pr_err("qos_queue_conf_get_32 fail: q[%d/%d]\n",
		       qid, node_id);
		res = DP_FAILURE;
		goto EXIT;
	}
	lookup = kzalloc(sizeof(*lookup), GFP_ATOMIC);
	if (!lookup) {
		res = DP_FAILURE;
		pr_err("kmalloc fail to alloc %zd bytes\n", sizeof(*lookup));
		goto EXIT;
	}
	/* map to drop queue and save the changed lookup entries for recover */
	if (dp_map_to_drop_q_32(inst, qid, lookup)) {
		pr_err("failed to dp_map_to_drop_q_32 for Q:%d\n", qid);
		res = DP_FAILURE;
		goto EXIT;
	}
	/* block/disable: ensure to drop all coming enqueue packet */
	if (queue_cfg.blocked == 0) { /* to block */
		if (pp_qos_queue_block(priv->qdev, node_id)) {
			pr_err("pp_qos_queue_block fail: q[%d/%d]\n",
			       qid, node_id);
			res = DP_FAILURE;
			goto EXIT;
		}
	}
#ifdef DP_FLUSH_VIA_AUTO
	while (times--) {
		get_q_qocc_32(inst, qid, &qocc);
		if (qocc == 0) /* no packet in the queue */
			break;
		udelay(10);
	};
	if (qocc)
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Auto Q[%d] Flushing failed:qocc=%u ???\n",
			 qid, qocc);
	else
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Auto Q[%d] Flushing ok:qocc=%u\n", qid, qocc);
#else /* flush queue via CQM API */
	if (queue_cfg.queue_child_prop.parent == priv->ppv4_drop_p) {
		/* already attached to drop queue and can directly flush */
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Flush:Q[%d] already under drop port[/%d]\n",
			 qid, priv->ppv4_drop_p);

		cqm_queue_flush_32(dp_port_prop[inst].cbm_inst,
				   priv->cqm_drop_p, qid);
	} else {
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Queue movement before flush");

		/*move to drop port and set block and resume the queue */
		qos_queue_conf_set_default_32(&tmp_q_cfg); /*use new variable */
		dp_wred_def(&tmp_q_cfg);
		tmp_q_cfg.queue_child_prop.parent = priv->ppv4_drop_p;
		if (qos_queue_set_32(priv->qdev, node_id, &tmp_q_cfg)) {
			pr_err(
			    "qos_queue_set_32 fail for queue=%d to parent=%d\n",
			    qid, tmp_q_cfg.queue_child_prop.parent);
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Flush:Move Q[%d] to drop port[/%d]\n",
			 qid,
			 tmp_q_cfg.queue_child_prop.parent);

		cqm_queue_flush_32(dp_port_prop[inst].cbm_inst,
				   priv->cqm_drop_p, qid);
#ifdef DP_FLUSH_SUSPEND_Q
		/* set to suspend again before move back to original parent */
		if (pp_qos_queue_suspend(priv->qdev, node_id)) {
			pr_err("pp_qos_queue_suspend fail q[%d] to parent=%d\n",
			       qid, tmp_q_cfg.queue_child_prop.parent);
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "suspend queue[%d/%d]\n", qid, node_id);
#endif
		/* move back the queue to original parent
		 * with orignal variable queue_cfg
		 */
		if (qos_queue_set_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("qos_queue_conf_get_32 fail: q[%d/%d]\n",
			       qid, node_id);
			res = DP_FAILURE;
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Flush:Move Q[%d] back to port[/%d]\n",
			 qid, queue_cfg.queue_child_prop.parent);
	}
#endif /* end of DP_FLUSH_VIA_AUTO */

#ifdef DP_FLUSH_SUSPEND_Q
	if (pp_qos_queue_suspend(priv->qdev, node_id)) {
		pr_err("qos_queue_set_32 fail\n");
		goto EXIT;
	}
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "suspend queue[%d/%d]\n", qid, node_id);
#endif
	/* restore lookup entry mapping for this qid if needed */
	if (flag & FLUSH_RESTORE_LOOKUP) {
		int i;

		if (lookup->num) {
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "Try to restore qid[%d] lookup entry: %d\n",
				 qid, lookup->num);
			for (i = 0; i < lookup->num; i++) {
				info.index = lookup->entry[i];
				info.qid = qid;
				dp_set_lookup_qid_via_index(&info);
			}
		}
	}
EXIT:
	kfree(lookup);
	lookup = NULL;
	return res;
}

/* get_node_type_by_node_id API
 * get node_type node_id in sch global table
 * upon Success
 *    return node_type of node_id
 */
static int get_node_type_by_node_id(int inst, int node_id, int flag)
{
	struct hal_priv *priv = HAL(inst);

	return priv->qos_sch_stat[node_id].type;
}

/* get_free_child_idx API
 * check free flag for child in parent's table and return index
 * else return DP_FAILURE
 */
static int get_free_child_idx(int inst, int node_id, int flag)
{
	int i;
	struct hal_priv *priv = HAL(inst);

	for (i = 0; i < DP_MAX_CHILD_PER_NODE; i++) {
		if (priv->qos_sch_stat[node_id].child[i].flag == PP_NODE_FREE)
			return i;
	}
	return DP_FAILURE;
}

/* get_parent_node API
 * check parent flag in node global table if active retrun parent id
 * else return DP_FAILURE
 */
static int get_parent_node(int inst, int node_id, int flag)
{
	struct hal_priv *priv = HAL(inst);
	int type = get_node_type_by_node_id(inst, node_id, 0);

	if ((priv->qos_sch_stat[node_id].parent.flag) &&
	    (type != DP_NODE_PORT))
		return priv->qos_sch_stat[node_id].parent.node_id;
	return DP_FAILURE;
}

/* get_child_idx_node_id API
 * check free flag in parent's global table and return index
 * else return DP_FAILURE
 */
static int get_child_idx_node_id(int inst, int node_id, int flag)
{
	struct hal_priv *priv = HAL(inst);
	int i, p_id;

	p_id = priv->qos_sch_stat[node_id].parent.node_id;

	for (i = 0; i < DP_MAX_CHILD_PER_NODE; i++) {
		if (node_id == priv->qos_sch_stat[p_id].child[i].node_id)
			return i;
	}
	return DP_FAILURE;
}

/* node_queue_dec API
 * for queue id = node_id, flag = DP_NODE_DEC
 * Set Queue flag from PP_NODE_ACTIVE to PP_NODE_ALLOC
 * else return DP_FAILURE
 */
static int node_queue_dec(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int phy_id, pid, idx;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	phy_id = get_qid_by_node(inst, node_id, flag);
	if (phy_id == DP_FAILURE) {
		pr_err("get_qid_by_node failed\n");
		return DP_FAILURE;
	}

	pid = get_parent_node(inst, node_id, flag);
	if (pid == DP_FAILURE) {
		pr_err("get_parent_node failed for Q:%d\n", phy_id);
		return DP_FAILURE;
	}
	DP_DEBUG(DP_DBG_FLAG_QOS, "parent:%d of Q:%d\n", pid, phy_id);

	idx = get_child_idx_node_id(inst, node_id, 0);
	if (idx == DP_FAILURE) {
		pr_err("get_child_idx_node_id failed for Q:%d\n", phy_id);
		return DP_FAILURE;
	}
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_queue_dec: parent:%d of Q:[%d/%d] child idx:%d\n",
		 pid, phy_id, node_id, idx);

	if (!(priv->qos_queue_stat[phy_id].flag & PP_NODE_ACTIVE)) {
		pr_err("Wrong Q[%d] Stat(%d):Expect ACTIVE\n", phy_id,
		       priv->qos_queue_stat[phy_id].flag);
		return DP_FAILURE;
	}
	if (!priv->qos_sch_stat[node_id].parent.flag) {
		pr_err("Wrong Q[%d]'s Parent Stat(%d):Expect ACTIVE\n",
		       node_id, priv->qos_sch_stat[node_id].parent.flag);
		return DP_FAILURE;
	}
	if (pid == priv->qos_sch_stat[node_id].parent.node_id) {
		priv->qos_sch_stat[pid].child[idx].flag = PP_NODE_FREE;
		priv->qos_sch_stat[pid].child[idx].node_id = 0;
		priv->qos_sch_stat[pid].child[idx].type = 0;
	}
	priv->qos_sch_stat[node_id].parent.node_id = 0;
	priv->qos_sch_stat[node_id].parent.type = 0;
	priv->qos_sch_stat[node_id].parent.flag = 0;
	priv->qos_queue_stat[phy_id].flag |= PP_NODE_ALLOC;
	priv->qos_sch_stat[node_id].p_flag |= PP_NODE_ALLOC;
	return DP_SUCCESS;
}

/* node_queue_inc API
 * for queue id = node_id, flag = DP_NODE_INC
 * Set Queue flag from PP_NODE_ALLOC to PP_NODE_ACTIVE
 * else return DP_FAILURE
 */
static int node_queue_inc(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int phy_id, pid, idx = 0;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	phy_id = get_qid_by_node(inst, node_id, flag);
	if (phy_id == DP_FAILURE) {
		pr_err("get_qid_by_node failed\n");
		return DP_FAILURE;
	}

	pid = get_parent_node(inst, node_id, flag);
	if (pid == DP_FAILURE) {
		pr_err("get_parent_node failed for Q:%d\n", phy_id);
		return DP_FAILURE;
	}
	DP_DEBUG(DP_DBG_FLAG_QOS, "parent:%d of Q:%d\n", pid, phy_id);

	idx = get_free_child_idx(inst, pid, 0);
	if (idx == DP_FAILURE) {
		pr_err("get_free_child_idx failed for Q:%d\n", phy_id);
		return DP_FAILURE;
	}
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_queue_inc: parent:%d of Q:[%d/%d] child idx:%d\n",
		 pid, phy_id, node_id, idx);

	if (!(priv->qos_queue_stat[phy_id].flag & PP_NODE_ALLOC)) {
		pr_err("Wrong Q[%d] Stat(%d):Expect ALLOC\n", phy_id,
		       priv->qos_queue_stat[phy_id].flag);
		return DP_FAILURE;
	}
	if (pid == priv->qos_sch_stat[node_id].parent.node_id) {
		priv->qos_sch_stat[pid].child[idx].flag = PP_NODE_ACTIVE;
		priv->qos_sch_stat[pid].child[idx].node_id = node_id;
		priv->qos_sch_stat[pid].child[idx].type = DP_NODE_QUEUE;
	}
	priv->qos_queue_stat[phy_id].flag |= PP_NODE_ACTIVE;
	priv->qos_sch_stat[node_id].p_flag |= PP_NODE_ACTIVE;
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "Q:[%d] type:%d idx:%d attach to parent:%d\n",
		 CHILD(pid, idx).node_id,
		 CHILD(pid, idx).type, idx, pid);
	return DP_SUCCESS;
}

/* node_queue_rst API
 * for queue id = node_id, flag = DP_NODE_RST
 * Set Queue flag from PP_NODE_ALLOC to PP_NODE_FREE
 * Set allocated memory free
 * else return DP_FAILURE
 */
static int node_queue_rst(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int phy_id = get_qid_by_node(inst, node_id, flag);
	int dp_port, resv_idx;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (phy_id == DP_FAILURE) {
		pr_err("get_qid_by_node failed\n");
		return DP_FAILURE;
	}
	dp_port = priv->qos_queue_stat[phy_id].dp_port;
	resv_idx = priv->qos_queue_stat[phy_id].resv_idx;

	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_queue_rst: Q:[%d/%d] resv_idx:%d\n",
		 phy_id, node_id, resv_idx);

	if (!(priv->qos_queue_stat[phy_id].flag & PP_NODE_ALLOC)) {
		pr_err("Wrong Q[%d] Stat(%d):Expect ALLOC\n", phy_id,
		       priv->qos_queue_stat[phy_id].flag);
		return DP_FAILURE;
	}

	/* Check for Reserve resource */
	if (priv->qos_queue_stat[phy_id].flag & PP_NODE_RESERVE) {
		priv->resv[dp_port].resv_q[resv_idx].flag = PP_NODE_FREE;
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "node_queue_rst:Q:[%d/%d] resv_idx:%d of EP:%d free\n",
			 phy_id, node_id, resv_idx, dp_port);
	}
	/* Remove resource from global table */
	memset(&priv->qos_queue_stat[phy_id], 0,
	       sizeof(priv->qos_queue_stat[phy_id]));
	memset(&priv->qos_sch_stat[node_id], 0,
	       sizeof(priv->qos_sch_stat[node_id]));
	priv->qos_queue_stat[phy_id].resv_idx = INV_RESV_IDX;
	priv->qos_sch_stat[node_id].resv_idx = INV_RESV_IDX;
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_queue_rst:%s Q:[%d/%d] resv_idx:%d dp_port=%d\n",
		 "After mem free", phy_id, node_id, resv_idx, dp_port);
	return DP_SUCCESS;
}

/* node_sched_dec API
 * for scheduler id = node_id, flag = DP_NODE_DEC
 * Set Sched flag from PP_NODE_ACTIVE to PP_NODE_ALLOC
 * else return DP_FAILURE
 */
static int node_sched_dec(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int pid, idx;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (flag & C_FLAG) {
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "node_sched_dec: parentSCH:[%d]\n", node_id);

		if (!priv->qos_sch_stat[node_id].child_num ||
		    !(priv->qos_sch_stat[node_id].c_flag & PP_NODE_ACTIVE)) {
			pr_err("Wrong Sch[%d] Stat(%d)/child_num(%d):%s\n",
			       node_id, priv->qos_sch_stat[node_id].c_flag,
			       priv->qos_sch_stat[node_id].child_num,
			       "Expect ACTIVE Or non-zero child_num");
			return DP_FAILURE;
		}
		priv->qos_sch_stat[node_id].child_num--;
		if (!priv->qos_sch_stat[node_id].child_num)
			priv->qos_sch_stat[node_id].c_flag |= PP_NODE_ALLOC;
		return DP_SUCCESS;
	} else if (flag & P_FLAG) {
		pid = get_parent_node(inst, node_id, flag);
		if (pid == DP_FAILURE) {
			pr_err("get_parent_node failed for sched:%d\n",
			       node_id);
			return DP_FAILURE;
		}

		idx = get_child_idx_node_id(inst, node_id, flag);
		if (idx == DP_FAILURE) {
			pr_err("get_child_idx_node_id failed for sched:%d\n",
			       node_id);
			return DP_FAILURE;
		}

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "node_sched_dec: parent:%d of SCH:[%d] child idx:%d\n",
			 pid, node_id, idx);

		if (!(priv->qos_sch_stat[node_id].p_flag & PP_NODE_ACTIVE)) {
			pr_err("Wrong Sch[%d] Stat(%d):Expect ACTIVE\n",
			       node_id, priv->qos_sch_stat[node_id].p_flag);
			return DP_FAILURE;
		}
		if (!priv->qos_sch_stat[node_id].parent.flag) {
			pr_err("Wrong SCH[%d] Parent Stat(%d):Expect ACTIV\n",
			       node_id,
			       priv->qos_sch_stat[node_id].parent.flag);
			return DP_FAILURE;
		}
		if (pid == priv->qos_sch_stat[node_id].parent.node_id) {
			priv->qos_sch_stat[pid].child[idx].flag = PP_NODE_FREE;
			priv->qos_sch_stat[pid].child[idx].node_id = 0;
			priv->qos_sch_stat[pid].child[idx].type = 0;
		}
		priv->qos_sch_stat[node_id].parent.node_id = 0;
		priv->qos_sch_stat[node_id].parent.type = 0;
		priv->qos_sch_stat[node_id].parent.flag = 0;
		priv->qos_sch_stat[node_id].p_flag |= PP_NODE_ALLOC;
		return DP_SUCCESS;
	}
	return DP_FAILURE;
}

/* node_sched_inc API
 * for scheduler id = node_id, flag = DP_NODE_INC
 * Set Sched flag from PP_NODE_ALLOC to PP_NODE_ACTIVE
 * else return DP_FAILURE
 */
static int node_sched_inc(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int pid, idx;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (flag & C_FLAG) {
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "node_sched_inc: parent SCH:[%d]\n", node_id);

		if (priv->qos_sch_stat[node_id].child_num &&
		    !(priv->qos_sch_stat[node_id].c_flag & PP_NODE_ACTIVE)) {
			pr_err("Wrong Sch[%d] Stat(%d)/child_num(%d):%s\n",
			       node_id, priv->qos_sch_stat[node_id].c_flag,
			       priv->qos_sch_stat[node_id].child_num,
			       "Expect ACTIVE And Non-Zero child_num");
			return DP_FAILURE;
		}
		if (!priv->qos_sch_stat[node_id].child_num &&
		    !(priv->qos_sch_stat[node_id].c_flag & PP_NODE_ALLOC)) {
			pr_err("Wrong Sch[%d] Stat(%d)/child_num(%d):%s\n",
			       node_id, priv->qos_sch_stat[node_id].c_flag,
			       priv->qos_sch_stat[node_id].child_num,
			       "Expect ALLOC And zero child_num");
			return DP_FAILURE;
		}
		priv->qos_sch_stat[node_id].child_num++;
		priv->qos_sch_stat[node_id].c_flag |= PP_NODE_ACTIVE;
		return DP_SUCCESS;
	} else if (flag & P_FLAG) {
		pid = get_parent_node(inst, node_id, flag);
		if (pid == DP_FAILURE) {
			pr_err("get_parent_node failed for sched:%d\n",
			       node_id);
			return DP_FAILURE;
		}

		idx = get_free_child_idx(inst, pid, 0);
		if (idx == DP_FAILURE) {
			pr_err("get_free_child_idx failed for sched:%d\n",
			       node_id);
			return DP_FAILURE;
		}

		if (!(priv->qos_sch_stat[node_id].p_flag & PP_NODE_ALLOC)) {
			pr_err("Wrong Sch[%d] Stat(%d):Expect ALLOC\n",
			       node_id, priv->qos_sch_stat[node_id].p_flag);
			return DP_FAILURE;
		}

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "node_sched_inc: parent:%d of SCH:[%d] child idx:%d\n",
			 pid, node_id, idx);

		if (pid == priv->qos_sch_stat[node_id].parent.node_id) {
			CHILD(pid, idx).flag = PP_NODE_ACTIVE;
			CHILD(pid, idx).node_id = node_id;
			CHILD(pid, idx).type = DP_NODE_SCH;
		}
		priv->qos_sch_stat[node_id].p_flag |= PP_NODE_ACTIVE;
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "SCH:[%d] type:%d idx:%d attach to parent:%d\n",
			 CHILD(pid, idx).node_id,
			 CHILD(pid, idx).type, idx, pid);
		return DP_SUCCESS;
	}
	return DP_FAILURE;
}

/* node_sched_rst API
 * for scheduler id = node_id, flag = DP_NODE_RST
 * sanity check for child_num and both c and p_flag in alloc state
 * then reset whole sched
 * Set Sched flag from PP_NODE_ALLOC to PP_NODE_FREE
 * else return DP_FAILURE
 */
static int node_sched_rst(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int dp_port, resv_idx;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	dp_port = priv->qos_sch_stat[node_id].dp_port;
	resv_idx = priv->qos_sch_stat[node_id].resv_idx;

	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_sched_rst:dp_port=%d SCH:%d resv_idx:%d\n",
		 dp_port, node_id, resv_idx);
	/* sanity check for child_num and both c and p_flag in alloc state
	 * then reset whole sched
	 */
	if (priv->qos_sch_stat[node_id].child_num ||
	    !(priv->qos_sch_stat[node_id].c_flag & PP_NODE_ALLOC) ||
	    !(priv->qos_sch_stat[node_id].p_flag & PP_NODE_ALLOC)) {
		pr_err("Wrong Sch[%d] c_flag(%d)/p_flag(%d)/child_num(%d):%s\n",
		       node_id, priv->qos_sch_stat[node_id].c_flag,
		       priv->qos_sch_stat[node_id].p_flag,
		       priv->qos_sch_stat[node_id].child_num,
		       "Expect c_flag OR p_flag ALLOC OR Non-zero child_num");
		return DP_FAILURE;
	}
	/* Free Reserved Resource */
	if (priv->qos_sch_stat[node_id].p_flag & PP_NODE_RESERVE) {
		priv->resv[dp_port].resv_sched[resv_idx].flag = PP_NODE_FREE;
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "node_sch_rst:Sch:[%d] resv_idx:%d of EP:%d free\n",
			 node_id, resv_idx, dp_port);
	}
	/* Free Global Resource */
	memset(&priv->qos_sch_stat[node_id], 0,
	       sizeof(priv->qos_sch_stat[node_id]));
	priv->qos_sch_stat[node_id].resv_idx = INV_RESV_IDX;
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_queue_rst:%s SCH:[/%d] resv_idx:%d dp_port=%d\n",
		 "After mem free", node_id, resv_idx, dp_port);
	return DP_SUCCESS;
}

/* node_port_dec API
 * Check for child_num and active flag
 * for port logical node_id, flag = DP_NODE_DEC
 * Set Port flag from PP_NODE_ACTIVE to PP_NODE_ALLOC
 * else return DP_FAILURE
 */
static int node_port_dec(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int phy_id;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	phy_id = get_cqm_deq_port_by_node(inst, node_id, flag);
	if (phy_id == DP_FAILURE) {
		pr_err("get_cqm_deq_port_by_node failed\n");
		return DP_FAILURE;
	}

	if (!priv->deq_port_stat[phy_id].child_num ||
	    !(priv->deq_port_stat[phy_id].flag & PP_NODE_ACTIVE)) {
		pr_err("Wrong P[%d] Stat(%d)/child_num(%d):%s\n",
		       phy_id, priv->deq_port_stat[phy_id].flag,
		       priv->deq_port_stat[phy_id].child_num,
		       "Expect ACTIVE Or non-zero child_num");
		return DP_FAILURE;
	}
	priv->qos_sch_stat[node_id].child_num--;
	priv->deq_port_stat[phy_id].child_num--;
	if (!priv->deq_port_stat[phy_id].child_num)
		priv->deq_port_stat[phy_id].flag = PP_NODE_ALLOC;
	return DP_SUCCESS;
}

/* node_port_inc API
 * for port logical node_id, flag = DP_NODE_INC
 * Set Port flag from PP_NODE_ALLOC to PP_NODE_ACTIVE
 * else return DP_FAILURE
 */
static int node_port_inc(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int phy_id;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	phy_id = get_cqm_deq_port_by_node(inst, node_id, flag);
	if (phy_id == DP_FAILURE) {
		pr_err("get_cqm_deq_port_by_node failed\n");
		return DP_FAILURE;
	}

	if (priv->deq_port_stat[phy_id].child_num &&
	    !(priv->deq_port_stat[phy_id].flag & PP_NODE_ACTIVE)) {
		pr_err("Wrong P[%d] Stat(%d)/child_num(%d):%s\n", phy_id,
		       priv->deq_port_stat[phy_id].flag,
		       priv->deq_port_stat[phy_id].child_num,
		       "Expect ACTIVE And Non-Zero child_num");
		return DP_FAILURE;
	}
	if (!priv->deq_port_stat[phy_id].child_num &&
	    !(priv->deq_port_stat[phy_id].flag & PP_NODE_ALLOC)) {
		pr_err("Wrong P[%d] Stat(%d)/child_num(%d):%s\n", phy_id,
		       priv->deq_port_stat[phy_id].flag,
		       priv->deq_port_stat[phy_id].child_num,
		       "Expect ALLOC And Zero child_num");
		return DP_FAILURE;
	}
	priv->qos_sch_stat[node_id].child_num++;
	priv->deq_port_stat[phy_id].child_num++;
	priv->deq_port_stat[phy_id].flag = PP_NODE_ACTIVE;
	return DP_SUCCESS;
}

/* node_port_rst API
 * Check for child_num and alloc flag
 * for port logical node_id, flag = DP_NODE_RST
 * Set Port flag from PP_NODE_ALLOC to PP_NODE_FREE
 * else return DP_FAILURE
 */
static int node_port_rst(int inst, int node_id, int flag)
{
	struct hal_priv *priv;
	int phy_id;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	phy_id = get_cqm_deq_port_by_node(inst, node_id, flag);
	if (phy_id == DP_FAILURE) {
		pr_err("get_cqm_deq_port_by_node failed\n");
		return DP_FAILURE;
	}

	if (priv->deq_port_stat[phy_id].child_num ||
	    !(priv->deq_port_stat[phy_id].flag & PP_NODE_ALLOC)) {
		pr_err("Wrong P[%d] Stat(%d)/child_num(%d):%s\n", phy_id,
		       priv->deq_port_stat[phy_id].flag,
		       priv->deq_port_stat[phy_id].child_num,
		       "Expect ALLOC Or non-zero child_num");
		return DP_FAILURE;
	}
	memset(&priv->deq_port_stat[phy_id], 0,
	       sizeof(priv->deq_port_stat[phy_id]));
	memset(&priv->qos_sch_stat[node_id], 0,
	       sizeof(priv->qos_sch_stat[node_id]));
	return DP_SUCCESS;
}

/* node_stat_update API
 * node_id is logical node id
 * if flag = DP_NODE_DEC
 *           update flag PP_NODE_ACTIVE to PP_NODE_ALLOC if needed
 *           update child info
 * else if flag = DP_NODE_INC
 *           update flag PP_NODE_ALLOC to PP_NODE_ACTIVE
 * else if flag = DP_NODE_RST
 *           update flag PP_NODE_ALLOC to PP_NODE_FREE
 *           reset table info
 * else return DP_FAILURE
 */
static int node_stat_update(int inst, int node_id, int flag)
{
	int node_type = get_node_type_by_node_id(inst, node_id, 0);

	if (flag & DP_NODE_DEC) {
		if (node_type == DP_NODE_QUEUE)
			return node_queue_dec(inst, node_id, flag);
		else if (node_type == DP_NODE_SCH)
			return node_sched_dec(inst, node_id, flag);
		else if (node_type == DP_NODE_PORT)
			return node_port_dec(inst, node_id, flag);
		return DP_FAILURE;
	} else if (flag & DP_NODE_INC) {
		if (node_type == DP_NODE_QUEUE)
			return node_queue_inc(inst, node_id, flag);
		else if (node_type == DP_NODE_SCH)
			return node_sched_inc(inst, node_id, flag);
		else if (node_type == DP_NODE_PORT)
			return node_port_inc(inst, node_id, flag);
		return DP_FAILURE;
	} else if (flag & DP_NODE_RST) {
		if (node_type == DP_NODE_QUEUE)
			return node_queue_rst(inst, node_id, flag);
		else if (node_type == DP_NODE_SCH)
			return node_sched_rst(inst, node_id, flag);
		else if (node_type == DP_NODE_PORT)
			return node_port_rst(inst, node_id, flag);
		return DP_FAILURE;
	}
	return DP_FAILURE;
}

#ifdef DP_TMP_DEL
/* dp_link_unset API
 * call node_stat_update with DP_NODE_DEC flag
 * upon success unlinks node to parent and returns DP_SUCCESS
 * else return DP_FAILURE
 */
static int dp_link_unset(struct dp_node_link *info, int flag)
{
	int node_id, p_id;
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct hal_priv *priv;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (info->node_type == DP_NODE_QUEUE) {
		node_id = priv->qos_queue_stat[info->node_id.q_id].node_id;
		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("failed to qos_queue_conf_get_32\n");
			return DP_FAILURE;
		}
		if (queue_cfg.queue_child_prop.parent ==
						*(int *)&info->p_node_id) {
			if (qos_queue_remove_32(priv->qdev, node_id)) {
				pr_err("failed to qos_queue_remove_32\n");
				return DP_FAILURE;
			}
		}
		goto EXIT;
	} else if (info->node_type == DP_NODE_SCH) {
		if (qos_sched_conf_get_32(priv->qdev, info->node_id.sch_id,
					  &sched_cfg)) {
			pr_err("failed to qos_queue_conf_get_32\n");
			return DP_FAILURE;
		}
		if (sched_cfg.sched_child_prop.parent ==
						*(int *)&info->p_node_id) {
			if (qos_sched_remove_32(priv->qdev,
						info->node_id.sch_id)) {
				pr_err("failed to qos_sched_remove_32\n");
				return DP_FAILURE;
			}
		}
		goto EXIT;
	}
	return DP_FAILURE;

EXIT:
	/* Child flag update after unlink */
	node_stat_update(info->inst, *(int *)&info->node_id,
			 DP_NODE_DEC | P_FLAG);
	/* reduce child_num in parent's global table */
	if (info->p_node_type == DP_NODE_SCH)
		node_stat_update(info->inst, PARENT(queue_cfg),
				 DP_NODE_DEC | C_FLAG);
	else /* convert parent to cqm_deq_port */
		p_id = get_cqm_deq_port_by_node(info->inst,
						*(int *)&info->p_node_id,
						flag);
		node_stat_update(info->inst, p_id, DP_NODE_DEC);
	return DP_SUCCESS;
}
#endif

/* dp_node_alloc_resv_pool API
 * Checks for flag and input node
 * upon success allocate resource from reserve table
 * otherwise return failure
 */
static int dp_node_alloc_resv_pool(struct dp_node_alloc *node, int flag)
{
	int i, cnt, phy_id, node_id;
	struct hal_priv *priv;
	struct resv_q *resv_q;

	if (!node) {
		pr_err("node is  NULL\n");
		return DP_FAILURE;
	}
	priv = node ? HAL(node->inst) : NULL;
	if (!priv) {
		pr_err("priv is NULL\n");
		return DP_FAILURE;
	}
	resv_q = priv->resv[node->dp_port].resv_q;

	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "inst=%d dp_port=%d num_resv_q=%d num_resv_sched=%d\n",
		 node->inst,
		 node->dp_port,
		 priv->resv[node->dp_port].num_resv_q,
		 priv->resv[node->dp_port].num_resv_sched);

	if (node->type == DP_NODE_QUEUE) {
		cnt = priv->resv[node->dp_port].num_resv_q;
		if (!cnt)
			return DP_FAILURE;
		DP_DEBUG(DP_DBG_FLAG_QOS, "try to look for resv queue...\n");
		for (i = 0; i < cnt; i++) {
			if (resv_q[i].flag != PP_NODE_FREE)
				continue;
			phy_id = resv_q[i].physical_id;
			node_id = resv_q[i].id;
			resv_q[i].flag = PP_NODE_ALLOC;
			priv->qos_queue_stat[phy_id].flag = PP_NODE_RESERVE |
								PP_NODE_ALLOC;
			priv->qos_queue_stat[phy_id].node_id = node_id;
			priv->qos_queue_stat[phy_id].resv_idx = i;
			priv->qos_queue_stat[phy_id].dp_port = node->dp_port;
			priv->qos_sch_stat[node_id].dp_port = node->dp_port;
			priv->qos_sch_stat[node_id].resv_idx = i;
			priv->qos_sch_stat[node_id].type = DP_NODE_QUEUE;
			priv->qos_sch_stat[node_id].p_flag = PP_NODE_RESERVE |
								PP_NODE_ALLOC;
			node->id.q_id = phy_id;
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "queue[%d/%d]:Resv_idx=%d\n",
				 phy_id, node_id,
				 priv->qos_queue_stat[phy_id].resv_idx);
			return DP_SUCCESS;
		}
	} else if (node->type == DP_NODE_SCH) {
		struct resv_sch *resv_sched;

		cnt = priv->resv[node->dp_port].num_resv_sched;
		if (!cnt)
			return DP_FAILURE;
		resv_sched = priv->resv[node->dp_port].resv_sched;
		for (i = 0; i < cnt; i++) {
			if (resv_sched[i].flag != PP_NODE_FREE)
				continue;
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "try to look for resv sche...\n");
			node_id = resv_sched[i].id;
			resv_sched[i].flag = PP_NODE_ALLOC;
			priv->qos_sch_stat[node_id].c_flag = PP_NODE_RESERVE |
								PP_NODE_ALLOC;
			priv->qos_sch_stat[node_id].p_flag = PP_NODE_RESERVE |
								PP_NODE_ALLOC;
			priv->qos_sch_stat[node_id].resv_idx = i;
			priv->qos_sch_stat[node_id].child_num = 0;
			priv->qos_sch_stat[node_id].dp_port = node->dp_port;
			priv->qos_sch_stat[node_id].type = DP_NODE_SCH;
			node->id.sch_id = node_id;
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "Sched[/%d]: Resv_idx=%d\n",
				 resv_sched[i].id,
				 priv->qos_sch_stat[node_id].resv_idx);
			return DP_SUCCESS;
		}
	}
	return DP_FAILURE;
}

/* dp_node_alloc_global_pool API
 * Checks for flag and input node
 * upon success allocate resource from global table
 * otherwise return failure
 */
static int dp_node_alloc_global_pool(struct dp_node_alloc *node, int flag)
{
	int id, phy_id;
	struct pp_qos_queue_info *qos_queue_info = NULL;
	struct pp_qos_queue_conf *q_conf = NULL;
	struct hal_priv *priv;
	int res = DP_FAILURE;

	if (!node) {
		pr_err("node is  NULL\n");
		goto EXIT;
	}

	priv = HAL(node->inst);
	if (!priv) {
		pr_err("priv is NULL\n");
		goto EXIT;
	}

	q_conf = kzalloc(sizeof(*q_conf), GFP_KERNEL);
	if (!q_conf) {
		pr_err("fail to alloc %zd bytes\n", sizeof(*q_conf));
		goto EXIT;
	}

	qos_queue_info = kzalloc(sizeof(*qos_queue_info), GFP_KERNEL);
	if (!qos_queue_info) {
		pr_err("fail to alloc %zd bytes\n", sizeof(*qos_queue_info));
		goto EXIT;
	}

	if (node->type == DP_NODE_QUEUE) {
		if (qos_queue_allocate_32(priv->qdev, &id)) {
			pr_err("qos_queue_allocate_32 failed\n");
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS, "qos_queue_allocate_32: %d\n", id);
#define WORKAROUND_PORT_SET
#define WORKAROUND_DROP_PORT
#ifdef WORKAROUND_PORT_SET /* workaround to get physcal queue id */
		/* workarpound: in order to get queue's physical queue id,
		 * it is a must call pp_qos_queue_set first,
		 * then call qos_queue_info_get_32 to get physical queue id
		 */
		qos_queue_conf_set_default_32(q_conf);
		dp_wred_def(q_conf);
#ifdef WORKAROUND_DROP_PORT /* use drop port */
		q_conf->queue_child_prop.parent = priv->ppv4_drop_p;
#else
		q_conf->queue_child_prop.parent = priv->ppv4_tmp_p;
#endif /* WORKAROUND_DROP_PORT */
		if (qos_queue_set_32(priv->qdev, id, q_conf)) {
			pr_err(
			    "qos_queue_set_32 fail for queue=%d to parent=%d\n",
			    id, q_conf->queue_child_prop.parent);
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Workaround queue(/%d)-> tmp parent(/%d)\n",
			 id, q_conf->queue_child_prop.parent);
		/* workarpound end ---- */
#endif /* WORKAROUND_PORT_SET */
		if (qos_queue_info_get_32(priv->qdev, id, qos_queue_info)) {
			qos_queue_remove_32(priv->qdev, id);
			pr_err("qos_queue_info_get_32: %d\n", id);
			goto EXIT;
		}

		phy_id = qos_queue_info->physical_id;
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "queue info: %d/%d to parent(/%d) dp_port=%d\n",
			 phy_id, id,
			 q_conf->queue_child_prop.parent, node->dp_port);
		priv->qos_queue_stat[phy_id].flag = PP_NODE_ALLOC;
		priv->qos_queue_stat[phy_id].node_id = id;
		priv->qos_queue_stat[phy_id].resv_idx = INV_RESV_IDX;
		priv->qos_queue_stat[phy_id].dp_port = node->dp_port;
		priv->qos_sch_stat[id].dp_port = node->dp_port;
		priv->qos_sch_stat[id].resv_idx = INV_RESV_IDX;
		priv->qos_sch_stat[id].type = DP_NODE_QUEUE;
		priv->qos_sch_stat[id].p_flag = PP_NODE_ALLOC;
		node->id.q_id = phy_id;
		res = DP_SUCCESS;
		goto EXIT;
	} else if (node->type == DP_NODE_SCH) {
		if (qos_sched_allocate_32(priv->qdev, &id)) {
			pr_err("failed to qos_sched_allocate_32\n");
			qos_sched_remove_32(priv->qdev, id);
			goto EXIT;
		}
		priv->qos_sch_stat[id].c_flag = PP_NODE_ALLOC;
		priv->qos_sch_stat[id].p_flag = PP_NODE_ALLOC;
		priv->qos_sch_stat[id].resv_idx = INV_RESV_IDX;
		priv->qos_sch_stat[id].dp_port = node->dp_port;
		priv->qos_sch_stat[id].child_num = 0;
		priv->qos_sch_stat[id].type = DP_NODE_SCH;
		node->id.sch_id = id;
		res = DP_SUCCESS;
		goto EXIT;
	} else {
		pr_err("Unknown node type %d\n", node->type);
	}

EXIT:
	kfree(q_conf);
	kfree(qos_queue_info);
	return res;
}

/* dp_alloc_qos_port API
 * upon success returns qos_deq_port
 * otherwise return failure
 */
static int dp_alloc_qos_port(struct dp_node_alloc *node, int flag)
{
	unsigned int qos_port;
	int cqm_deq_port;
	int inst;
	struct pp_qos_port_conf port_cfg;
	struct hal_priv *priv;

	if (!node) {
		pr_err("node NULL\n");
		goto EXIT;
	}
	inst = node->inst;
	priv = HAL(node->inst);
	if (!priv) {
		pr_err("priv NULL\n");
		goto EXIT;
	}
	cqm_deq_port = node->id.cqm_deq_port;
	DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
		 "inst=%d dp_port=%d cqm_deq_port=%d\n",
		 node->inst, node->dp_port, cqm_deq_port);
	if (cqm_deq_port == DP_NODE_AUTO_ID) {
		pr_err("cqm_deq_port wrong: %d\n", cqm_deq_port);
		goto EXIT;
	}
	if (priv->deq_port_stat[cqm_deq_port].flag != PP_NODE_FREE) {
		DP_DEBUG(DP_DBG_FLAG_QOS, "cqm_deq_port[%d] already init:\n",
			 cqm_deq_port);
		return priv->deq_port_stat[cqm_deq_port].node_id;
	}
	if (qos_port_allocate_32(priv->qdev, cqm_deq_port, &qos_port)) {
		pr_err("failed to qos_port_allocate_32:%d\n", cqm_deq_port);
		goto EXIT;
	}
	/* pr_info("qos_port_alloc succeed: %d/%d\n",
	 *	   cqm_deq_port, qos_port);
	 */
	/* Configure QOS dequeue port */
	qos_port_conf_set_default_32(&port_cfg);
	port_cfg.ring_address =
		dp_deq_port_tbl[inst][cqm_deq_port].txpush_addr_qos;
	port_cfg.ring_size = dp_deq_port_tbl[inst][cqm_deq_port].tx_ring_size;
	port_cfg.credit = dp_deq_port_tbl[inst][cqm_deq_port].tx_pkt_credit;
	if (port_cfg.credit)
		port_cfg.packet_credit_enable = 1;
	port_cfg.port_parent_prop.arbitration = PP_QOS_ARBITRATION_WSP;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (unlikely(dp_dbg_flag & DP_DBG_FLAG_QOS)) {
		DP_DEBUG(DP_DBG_FLAG_QOS, "qos_port_set_32 parameter: %d/%d\n",
			 cqm_deq_port, qos_port);
		DP_DEBUG(DP_DBG_FLAG_QOS, "  ring_address_push=0x%lx\n",
			 port_cfg.ring_address);
		DP_DEBUG(DP_DBG_FLAG_QOS, "  ring_size=%d\n",
			 port_cfg.ring_size);
		DP_DEBUG(DP_DBG_FLAG_QOS, "  credit=%d\n",
			 port_cfg.credit);
		DP_DEBUG(DP_DBG_FLAG_QOS, "  packet_credit_enable=%d\n",
			 port_cfg.packet_credit_enable);
		DP_DEBUG(DP_DBG_FLAG_QOS, "  Arbitration=%d\n",
			 port_cfg.port_parent_prop.arbitration);
		DP_DEBUG(DP_DBG_FLAG_QOS, "priv->qdev=0x%px\n",
			 priv->qdev);
	}
#endif
	if (qos_port_set_32(priv->qdev, qos_port, &port_cfg)) {
		pr_err("qos_port_set_32 fail for port %d/%d\n",
		       cqm_deq_port, qos_port);
		qos_port_remove_32(priv->qdev, qos_port);
		goto EXIT;
	}
	priv->deq_port_stat[cqm_deq_port].flag = PP_NODE_ALLOC;
	priv->qos_sch_stat[qos_port].type = DP_NODE_PORT;
	priv->qos_sch_stat[qos_port].child_num = 0;
	priv->deq_port_stat[cqm_deq_port].node_id = qos_port;
	DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
		 "dp_alloc_qos_port ok: port=%d/%d for dp_port=%d\n",
		 cqm_deq_port, qos_port, node->dp_port);
	return qos_port;
EXIT:
	return DP_FAILURE;
}

/* dp_node_alloc_32 API
 * Checks for flag and input node type
 * upon success allocate node from global/reserve resource
 * otherwise return failure
 */
int dp_node_alloc_32(struct dp_node_alloc *node, int flag)
{
	struct hal_priv *priv;

	if (!node) {
		pr_err("node NULL\n");
		return DP_FAILURE;
	}

	priv = HAL(node->inst);
	if (!priv) {
		pr_err("priv is NULL cannot proceed!!\n");
		return DP_FAILURE;
	}

	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "allocate flag %d\n", flag);
	if (flag & DP_ALLOC_RESV_ONLY) {
	/* if can get from its reserved resource and return DP_SUCCESS.
	 *	Otherwise return DP_FAIL;
	 */
		return dp_node_alloc_resv_pool(node, flag);
	}
	if (flag & DP_ALLOC_GLOBAL_ONLY) {
	/* if can get from global pool and return DP_SUCCESS.
	 *	Otherwise return DP_FAILURE;
	 */
		return dp_node_alloc_global_pool(node, flag);
	}
	if (flag & DP_ALLOC_GLOBAL_FIRST) {
	/* if can get from the global pool, return DP_SUCCESS;
	 * if can get from its reserved resource and return DP_SUCCESS;
	 * return DP_FAILURE;
	 */
		if (dp_node_alloc_global_pool(node, flag) == DP_SUCCESS)
			return DP_SUCCESS;
		return dp_node_alloc_resv_pool(node, flag);
	}
	/* default order: reserved pool, */
	/* if can get from its reserved resource and return DP_SUCCESS
	 * if can get from the global pool, return DP_SUCCESS
	 * return DP_FAILURE
	 */
	if (dp_node_alloc_resv_pool(node, flag) == DP_SUCCESS)
		return DP_SUCCESS;
	return dp_node_alloc_global_pool(node, flag);
}

/* dp_map_to_drop_q_32 API
 * check index in lookup table for q_id
 * param q_buf: to save the lookup entries which mapped to this q_id
 * param num: q_buf size
 * set drop_q and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_map_to_drop_q_32(int inst, int q_id, struct dp_lookup_entry *lookup)
{
	u32 i, k = 0;
	struct hal_priv *priv = HAL(inst);
	struct cbm_lookup cbm_lookup = {0};

	for (i = 0; i < MAX_LOOKUP_TBL_SIZE; i++) {
		cbm_lookup.index = i;
		cbm_lookup.egflag = 0;
		if (q_id == dp_get_lookup_qid_via_index(&cbm_lookup)) {
			if (lookup) {
				lookup->entry[k] = i;
				k++;
			}
			cbm_lookup.index = i;
			cbm_lookup.qid = priv->ppv4_drop_q;
			dp_set_lookup_qid_via_index(&cbm_lookup);
		}
	}
	if (lookup)
		lookup->num = k;
	return DP_SUCCESS;
}

/* dp_smart_free_from_child_32 API
 * flush and unlink queue from its parent
 * check parent's child list if empty free parent recursively
 * else return DP_FAILURE
 */
static int dp_smart_free_from_child_32(struct dp_node_alloc *node, int flag)
{
	int id, res, f_free;
	struct dp_node_link info = {0};
	struct dp_node_alloc temp = {0};
	struct hal_priv *priv;

	if (!node) {
		pr_err("node is NULL cannot proceed!!\n");
		return DP_FAILURE;
	}

	priv = HAL(node->inst);
	if (!priv) {
		pr_err("priv is NULL cannot proceed!!\n");
		return DP_FAILURE;
	}

	DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
		 "dp_smart_free_from_child_32:type=%d q_id=%d\n",
		 node->type,
		 node->id.q_id);
	if (node->type == DP_NODE_QUEUE) {
		if ((node->id.q_id < 0) || (node->id.q_id >= MAX_QUEUE)) {
			pr_err("Wrong Parameter: QID[%d]Out Of Range\n",
			       node->id.q_id);
			return DP_FAILURE;
		}

		info.node_id.q_id = node->id.q_id;
		info.node_type = node->type;
		id = priv->qos_queue_stat[node->id.q_id].node_id;

		info.p_node_id.q_id = priv->qos_sch_stat[id].parent.node_id;
		info.p_node_type = priv->qos_sch_stat[id].parent.type;

		if (!info.p_node_id.q_id) {
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "current node doesnot have parent\n");
		} else if (dp_node_unlink_32(&info, 0)) {
			pr_err("dp_node_unlink_32 failed\n");
			return DP_FAILURE;
		}

		if (dp_node_free_32(node, 0)) {
			pr_err("failed to free Queue:[%d]\n", node->id.q_id);
			return DP_FAILURE;
		}
	} else if (node->type == DP_NODE_SCH) {
		if ((node->id.sch_id < 0) ||
		    (node->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Wrong Parameter: Sched[%d]Out Of Range\n",
			       node->id.sch_id);
			return DP_FAILURE;
		}

		info.node_id.sch_id = node->id.sch_id;
		info.node_type = node->type;
		id = node->id.sch_id;
		info.p_node_id.q_id = priv->qos_sch_stat[id].parent.node_id;
		info.p_node_type = priv->qos_sch_stat[id].parent.type;

		if (!info.p_node_id.sch_id) {
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "current node doesnot have parent\n");
			return DP_FAILURE;
		}

		if (dp_node_free_32(node, 0)) {
			pr_err("failed to free Sched:[%d]\n", node->id.sch_id);
			return DP_FAILURE;
		}
	} else if (node->type == DP_NODE_PORT) {
		if ((node->id.cqm_deq_port < 0) ||
		    (node->id.cqm_deq_port >= MAX_CQM_DEQ)) {
			pr_err("Wrong Parameter: Port[%d]Out Of Range\n",
			       node->id.cqm_deq_port);
			return DP_FAILURE;
		}

		if (dp_node_free_32(node, 0)) {
			pr_err("failed to free Port:[%d]\n",
			       node->id.cqm_deq_port);
			return DP_FAILURE;
		}
		return DP_SUCCESS;
	}

	while (1) {
		info.node_id = info.p_node_id;
		info.node_type = info.p_node_type;
		temp.id = info.p_node_id;
		temp.type = info.p_node_type;

		if (temp.type == DP_NODE_PORT) {
			temp.id.cqm_deq_port =
				get_cqm_deq_port_by_node(node->inst,
							 temp.id.cqm_deq_port,
							 flag);
			id = temp.id.cqm_deq_port;
			if ((id < 0) || (id >= MAX_CQM_DEQ)) {
				pr_err("Wrong Parameter: Port[%d]%s\n",
				       id, "Out Of Range");
				return DP_FAILURE;
			}
		} else {
			id = temp.id.sch_id;
			if ((id < 0) || (id >= QOS_MAX_NODES)) {
				pr_err("Wrong Parameter: Sched[%d]%s\n",
				       id, "Out Of Range");
				return DP_FAILURE;
			}
		}

		DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
			 "dp_smart_free_from_child_32:type=%d q_id=%d\n",
			 temp.type,
			 id);
		if ((temp.type == DP_NODE_SCH &&
		     priv->qos_sch_stat[id].child_num) ||
		    (temp.type == DP_NODE_PORT &&
		     priv->deq_port_stat[id].child_num)) {
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "parent node(%d) still have child\n", id);
			break;
		}
		f_free = (dp_node_link_get_32(&info, 0));
		res = dp_node_free_32(&temp, 0);
		if (res) {
			pr_err("failed to free node:%d res %d\n",
			       temp.id.q_id, res);
			return DP_FAILURE;
		}
		if (f_free) {
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "current node doesnot have parent\n");
			break;
		}
	}
	return DP_SUCCESS;
}

/* dp_free_children_via_parent_32 API
 * reset parent to free state
 * check parent's child list and free all resources recursively
 * else return DP_FAILURE
 */
int dp_free_children_via_parent_32(struct dp_node_alloc *node, int flag)
{
	int idx, id, pid, child_id;
	struct dp_node_alloc temp = {0};
	struct dp_node_link info = {0};
	struct hal_priv *priv;

	if (!node) {
		pr_err("node is NULL cannot proceed!!\n");
		return DP_FAILURE;
	}

	priv = HAL(node->inst);
	if (!priv) {
		pr_err("priv is NULL cannot proceed!!\n");
		return DP_FAILURE;
	}

	if (node->type == DP_NODE_PORT) {
		if ((node->id.cqm_deq_port < 0) ||
		    (node->id.cqm_deq_port >= MAX_CQM_DEQ)) {
			pr_err("Wrong Parameter: Port[%d]Out Of Range\n",
			       node->id.cqm_deq_port);
			return DP_FAILURE;
		}
		id = priv->deq_port_stat[node->id.cqm_deq_port].node_id;

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "parent Port(%d) have child num:%d\n",
			 node->id.cqm_deq_port,
			 priv->qos_sch_stat[id].child_num);

		for (idx = 0; idx < DP_MAX_CHILD_PER_NODE; idx++) {
			if (priv->qos_sch_stat[id].child[idx].flag &
			    PP_NODE_ACTIVE) {
				temp.type = CHILD(id, idx).type;
				child_id =
					get_qid_by_node(node->inst,
							CHILD(id, idx).node_id,
							0);
				if (CHILD(id, idx).type == DP_NODE_SCH)
					temp.id.q_id = CHILD(id, idx).node_id;
				else
					temp.id.q_id = child_id;
				if (dp_free_children_via_parent_32(&temp, 0)) {
					pr_err("fail %s=%d child:%d type=%d\n",
					       "to free Port's",
					       node->id.cqm_deq_port,
					       CHILD(id, idx).node_id,
					       CHILD(id, idx).type);
					return DP_FAILURE;
				}
			}
		}

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Port(%d)'s all children:%d freed!\n",
			 node->id.cqm_deq_port,
			 priv->qos_sch_stat[id].child_num);

		if (!priv->qos_sch_stat[id].child_num) {
			if (dp_node_free_32(node, 0)) {
				pr_err("failed to free Port:[%d]\n",
				       node->id.cqm_deq_port);
				return DP_FAILURE;
			}
		}
	} else if (node->type == DP_NODE_SCH) {
		if ((node->id.sch_id < 0) ||
		    (node->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Wrong Parameter: Sched[%d]Out Of Range\n",
			       node->id.sch_id);
			return DP_FAILURE;
		}
		id = node->id.sch_id;

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "parent SCH(%d) have child num:%d\n",
			 node->id.sch_id,
			 priv->qos_sch_stat[id].child_num);

		for (idx = 0; idx < DP_MAX_CHILD_PER_NODE; idx++) {
			if (priv->qos_sch_stat[id].child[idx].flag &
			    PP_NODE_ACTIVE) {
				temp.type = CHILD(id, idx).type;
				child_id =
					get_qid_by_node(node->inst,
							CHILD(id, idx).node_id,
							0);
				if (CHILD(id, idx).type == DP_NODE_SCH)
					temp.id.q_id = CHILD(id, idx).node_id;
				else
					temp.id.q_id = child_id;
				if (dp_free_children_via_parent_32(&temp, 0)) {
					pr_err("fail %s=%d child:%d type=%d\n",
					       "to free Sched's",
					       node->id.sch_id,
					       CHILD(id, idx).node_id,
					       CHILD(id, idx).type);
					return DP_FAILURE;
				}
				DP_DEBUG(DP_DBG_FLAG_QOS,
					 "Free SCH(%d)'s child:%d done!\n",
					 node->id.sch_id,
					 CHILD(id, idx).node_id);
			}
		}

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "SCH(%d)'s all children:%d freed!\n",
			 node->id.sch_id,
			 priv->qos_sch_stat[id].child_num);

		if (!priv->qos_sch_stat[id].child_num) {
			if (dp_node_free_32(node, 0)) {
				pr_err("failed to free Sched:[%d]\n",
				       node->id.sch_id);
				return DP_FAILURE;
			}
		}
	} else if (node->type == DP_NODE_QUEUE) {
		if ((node->id.q_id < 0) || (node->id.q_id >= MAX_QUEUE)) {
			pr_err("Wrong Parameter: QID[%d]Out Of Range\n",
			       node->id.q_id);
			return DP_FAILURE;
		}

		/* get parent node from global table and fill info */
		info.node_id.q_id = node->id.q_id;
		info.node_type = node->type;
		pid = priv->qos_queue_stat[node->id.q_id].node_id;
		info.p_node_id.q_id = priv->qos_sch_stat[pid].parent.node_id;
		info.p_node_type = priv->qos_sch_stat[pid].parent.type;

		if (priv->qos_queue_stat[node->id.q_id].flag != PP_NODE_FREE) {
			if (dp_node_unlink_32(&info, 0)) {
				pr_err("dp_node_unlink_32 failed\n");
				return DP_FAILURE;
			}

			if (dp_node_free_32(node, 0)) {
				pr_err("failed to free Queue:[%d]\n",
				       node->id.q_id);
				return DP_FAILURE;
			}
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "%s:Q[%d] Parent:%d type:%d\n",
				 "free_children_via_parent",
				 node->id.q_id,
				 info.p_node_id.q_id,
				 info.p_node_type);
		}
	} else {
		pr_err("Incorrect Parameter:%d\n", node->type);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}

struct link_free_var {
	struct pp_qos_queue_conf queue_cfg;
	struct pp_qos_queue_conf tmp_q;
	struct pp_qos_sched_conf sched_cfg;
	struct pp_qos_sched_conf tmp_sch;
	struct dp_node_link info;
	int sch_id, phy_id, node_id, parent_id, p_type;
	int dp_port, resv_flag;
};

/* dp_node_free_32 API
 * if (this node is not unlinked yet)
 * unlink it
 * If (this node is a reserved node)
 * return this node to this device's reserved node table
 * mark this node as Free in this device's reserved node table
 * else
 * return this node to the system global table
 * mark this node free in system global table
 */
int dp_node_free_32(struct dp_node_alloc *node, int flag)
{
	struct hal_priv *priv;
	struct link_free_var *t = NULL;
	int res = DP_FAILURE;

	if (!node) {
		pr_err("node is NULL cannot proceed!!\n");
		return DP_FAILURE;
	}

	priv = HAL(node->inst);
	if (!priv) {
		pr_err("priv is NULL cannot proceed!!\n");
		return DP_FAILURE;
	}
	t = kzalloc(sizeof(*t), GFP_ATOMIC);
	if (!t) {
		pr_err("fail to alloc %zd bytes\n", sizeof(*t));
		return DP_FAILURE;
	}

	if (flag == DP_NODE_SMART_FREE) {/* dont pass flag */
		res = dp_smart_free_from_child_32(node, 0);
		if (res == DP_FAILURE) {
			pr_err("dp_smart_free_from_child_32 failed\n");
			goto EXIT;
		}
	}

	if (node->type == DP_NODE_QUEUE) {
		t->node_id = priv->qos_queue_stat[node->id.q_id].node_id;
		t->dp_port = priv->qos_queue_stat[node->id.q_id].dp_port;
		t->resv_flag = priv->qos_queue_stat[node->id.q_id].flag;

		if (priv->qos_queue_stat[node->id.q_id].flag == PP_NODE_FREE) {
			pr_err("Node Q[%d] is already Free Stat\n",
			       node->id.q_id);
			goto EXIT;
		}

		if (qos_queue_conf_get_32(priv->qdev, t->node_id,
					  &t->queue_cfg)) {
			res = node_stat_update(node->inst, t->node_id,
					       DP_NODE_RST);
			if (res == DP_FAILURE) {
				pr_err("node_stat_update failed\n");
				goto EXIT;
			}
			if (qos_queue_remove_32(priv->qdev, t->node_id)) {
				pr_err("failed to qos_queue_remove_32\n");
				goto EXIT;
			}
			res = DP_SUCCESS;
			goto EXIT;
		}
		/* call node unlink api and set drop queue */
		t->info.inst = node->inst;
		t->info.node_id = node->id;
		t->info.node_type = node->type;
		if (dp_node_unlink_32(&t->info, 0)) {
			pr_err("failed to dp_node_unlink_32 for Q:%d\n",
			       node->id.q_id);
			goto EXIT;
		}

		t->parent_id = t->queue_cfg.queue_child_prop.parent;
		t->p_type = get_node_type_by_node_id(node->inst,
						     t->parent_id, flag);
		/* Remove Queue link only for global resource */
		if (!(t->resv_flag & PP_NODE_RESERVE)) {
			if (qos_queue_remove_32(priv->qdev, t->node_id)) {
				pr_err("failed to qos_queue_remove_32\n");
				goto EXIT;
			}
		}
		if (node_stat_update(node->inst, t->node_id, DP_NODE_DEC)) {
			pr_err("node_stat_update failed\n");
			goto EXIT;
		}
		/* call node_stat_update to update parent status */
		if (node_stat_update(node->inst, t->parent_id,
				     DP_NODE_DEC | C_FLAG)) {
			pr_err("stat update fail Q:[%d/%d]'s parent:%d\n",
			       node->id.q_id, t->node_id, t->parent_id);
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Q[%d] removed parent:[%d] stat updated\n",
			 node->id.q_id, t->parent_id);
		/* call node_Stat_update to free the node */
		if (node_stat_update(node->inst, t->node_id,
				     DP_NODE_RST)) {
			pr_err("node_stat_update failed\n");
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Queue[%d/%d] removed and stat updated\n",
			 node->id.q_id, t->node_id);
		/* Reserve Q temp attach to drop port */
		if (t->resv_flag & PP_NODE_RESERVE) {
			qos_queue_conf_set_default_32(&t->tmp_q);
			t->tmp_q.queue_child_prop.parent = priv->ppv4_drop_p;
			if (qos_queue_set_32(priv->qdev, t->node_id,
					     &t->tmp_q)) {
				pr_err("qos_queue_set_32 %s=%d to parent=%d\n",
				       "fail to reserve queue", t->node_id,
				       t->tmp_q.queue_child_prop.parent);
				goto EXIT;
			}
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "Q:%d/%d attached temp to Drop_port:%d\n",
				 node->id.q_id,
				 t->node_id, t->tmp_q.queue_child_prop.parent);
		}
		res = DP_SUCCESS;
		goto EXIT;
	} else if (node->type == DP_NODE_SCH) {
		t->sch_id = node->id.sch_id;
		t->dp_port = priv->qos_sch_stat[t->sch_id].dp_port;
		t->resv_flag = priv->qos_sch_stat[t->sch_id].p_flag;

		if (priv->qos_sch_stat[t->sch_id].child_num) {
			pr_err("Node Sch[%d] still have child num %d\n",
			       t->sch_id,
			       priv->qos_sch_stat[t->sch_id].child_num);
			goto EXIT;
		}

		if (priv->qos_sch_stat[t->sch_id].p_flag == PP_NODE_FREE) {
			pr_err("Node Sch[%d] is already Free Stat\n",
			       t->sch_id);
			goto EXIT;
		}

		if (qos_sched_conf_get_32(priv->qdev, t->sch_id,
					  &t->sched_cfg)) {
			if (node_stat_update(node->inst, t->sch_id,
					     DP_NODE_RST | P_FLAG)) {
				pr_err("node_stat_update failed\n");
				goto EXIT;
			}
			if (qos_sched_remove_32(priv->qdev, t->sch_id)) {
				pr_err("failed to qos_sched_remove_32\n");
				goto EXIT;
			}
			res = DP_SUCCESS;
			goto EXIT;
		}
		t->parent_id = t->sched_cfg.sched_child_prop.parent;
		t->p_type = get_node_type_by_node_id(node->inst,
						     t->parent_id, flag);
		/* Remove Sched link only if global resource */
		if (!(t->resv_flag & PP_NODE_RESERVE)) {
			if (qos_sched_remove_32(priv->qdev, t->sch_id)) {
				pr_err("failed to qos_sched_remove_32\n");
				goto EXIT;
			}
		}
		if (node_stat_update(node->inst, t->sch_id,
				     DP_NODE_DEC | P_FLAG)) {
			pr_err("node_stat_update failed\n");
			goto EXIT;
		}
		/* call node_stat_update to update parent status */
		if (node_stat_update(node->inst, t->parent_id,
				     DP_NODE_DEC | C_FLAG)) {
			pr_err("stat update fail Sch:[%d]'s parent:%d\n",
			       node->id.sch_id, t->parent_id);
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "SCH[%d] removed and parent:[%d] stat updated\n",
			node->id.sch_id, t->parent_id);
		/* call node_stat_update to free the node */
		if (node_stat_update(node->inst, t->sch_id,
				     DP_NODE_RST | P_FLAG)) {
			pr_err("Node Reset failed Sched[/%d]\n",
			       node->id.sch_id);
			goto EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Sched[%d] removed and stat updated\n",
			 node->id.sch_id);
		/* Reserve Sched temp attach to drop port */
		if (t->resv_flag & PP_NODE_RESERVE) {
			qos_sched_conf_set_default_32(&t->tmp_sch);
			t->tmp_sch.sched_child_prop.parent = priv->ppv4_drop_p;
			if (qos_sched_set_32(priv->qdev, t->sch_id,
					     &t->tmp_sch)) {
				pr_err("qos_sched_set_32 %s=%d to parent=%d\n",
				       "fail to reserve SCH", t->sch_id,
				       t->tmp_sch.sched_child_prop.parent);
				goto EXIT;
			}
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "SCH:%d attached temp to Drop_port:%d\n",
				 t->sch_id,
				 t->tmp_sch.sched_child_prop.parent);
		}
		res = DP_SUCCESS;
		goto EXIT;
	} else if (node->type == DP_NODE_PORT) {
		t->phy_id = node->id.cqm_deq_port;
		t->node_id = priv->deq_port_stat[t->phy_id].node_id;

		if (priv->deq_port_stat[t->phy_id].child_num) {
			pr_err("Node port[%d] still have child num %d\n",
			       t->phy_id,
			       priv->deq_port_stat[t->phy_id].child_num);
			goto EXIT;
		}
		if (priv->deq_port_stat[t->phy_id].flag & PP_NODE_ACTIVE) {
			res = node_stat_update(node->inst, t->node_id,
					       DP_NODE_DEC);
			if (res == DP_FAILURE) {
				pr_err("Wrong Port %d flag:0x%x\n", t->phy_id,
				       priv->deq_port_stat[t->phy_id].flag);
				goto EXIT;
			}
			goto EXIT;
		}
		/* No reset API call for Port should freed by child's call */
		if (priv->deq_port_stat[t->phy_id].flag & PP_NODE_ALLOC) {
			res = DP_SUCCESS;
			goto EXIT;
		}
		pr_err("Unexpect port %d flag %d\n",
		       t->phy_id, priv->deq_port_stat[t->phy_id].flag);
		goto EXIT;
	}
	pr_err("Unexpect node->type %d\n", node->type);

EXIT:
	if (res == DP_FAILURE)
		pr_err("failed to free node:%d res %d\n",
		       node->id.q_id, res);
	kfree(t);
	t = NULL;
	return res;
}

/* dp_qos_parent_chk API
 * checks for parent type
 * upon Success
 *    return parent node id
 * else return DP_FAILURE
 */
static int dp_qos_parent_chk(struct dp_node_link *info, int flag)
{
	struct dp_node_alloc node;

	if (info->p_node_type == DP_NODE_SCH) {
		if (info->p_node_id.sch_id == DP_NODE_AUTO_ID) {
			node.inst = info->inst;
			node.type = info->p_node_type;
			node.dp_port = info->dp_port;

			if ((dp_node_alloc_32(&node, flag)) == DP_FAILURE) {
				pr_err("dp_node_alloc_32 queue alloc fail\n");
				return DP_FAILURE;
			}
			info->p_node_id = node.id;
		}
		return info->p_node_id.sch_id;
	} else if (info->p_node_type == DP_NODE_PORT) {
		node.inst = info->inst;
		node.id = info->cqm_deq_port;
		node.type = info->p_node_type;
		node.dp_port = info->dp_port;
		return dp_alloc_qos_port(&node, flag);
	}
	return DP_FAILURE;
}

/* dp_node_link_get_32 API
 * upon success check node link info and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_node_link_get_32(struct dp_node_link *info, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct hal_priv *priv;
	int node_id;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (info->node_type == DP_NODE_QUEUE) {
		node_id = priv->qos_queue_stat[info->node_id.q_id].node_id;
		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("failed to qos_queue_conf_get_32\n");
			return DP_FAILURE;
		}
		if (!queue_cfg.queue_child_prop.parent)
			return DP_FAILURE;
		if (!(priv->qos_queue_stat[info->node_id.q_id].flag &
		      PP_NODE_ACTIVE)) {
			return DP_FAILURE;
		}
		info->p_node_id.sch_id =
			queue_cfg.queue_child_prop.parent;
		info->p_node_type = get_node_type_by_node_id(info->inst,
				queue_cfg.queue_child_prop.parent, flag);
		info->prio_wfq = queue_cfg.queue_child_prop.priority;
		info->arbi = 0;
		info->leaf = 0;
		return DP_SUCCESS;
	} else if (info->node_type == DP_NODE_SCH) {
		if (qos_sched_conf_get_32(priv->qdev, info->node_id.sch_id,
					  &sched_cfg)) {
			pr_err("failed to qos_sched_conf_get_32\n");
			return DP_FAILURE;
		}
		if (!sched_cfg.sched_child_prop.parent) {
			pr_err("sched child do not have parent\n");
			return DP_FAILURE;
		}
		if (!(priv->qos_sch_stat[info->node_id.sch_id].p_flag &
		      PP_NODE_ACTIVE)) {
			pr_err("sched id %d flag not active, flag %d\n",
			       info->node_id.sch_id,
			       priv->qos_sch_stat[info->node_id.sch_id].p_flag);
			return DP_FAILURE;
		}
		info->p_node_id.sch_id = sched_cfg.sched_child_prop.parent;
		info->p_node_type = get_node_type_by_node_id(info->inst,
				sched_cfg.sched_child_prop.parent, flag);
		info->prio_wfq = sched_cfg.sched_child_prop.priority;
		info->arbi =
			arbi_pp2dp_32(sched_cfg.sched_parent_prop.arbitration);
		info->leaf = 0;
		return DP_SUCCESS;
	}
	return DP_FAILURE;
}

/* dp_link_set API
 * upon success links node to parent and returns DP_SUCCESS
 * else return DP_FAILURE
 */
static int dp_link_set(struct dp_node_link *info, int parent_node, int flag)
{
	int node_id;
	int res = DP_FAILURE;
	int node_flag = DP_NODE_INC;
	struct hal_priv *priv = HAL(info->inst);
	struct pp_qos_queue_conf *queue_cfg;
	struct pp_qos_sched_conf *sched_cfg;

	queue_cfg = kzalloc(sizeof(*queue_cfg), GFP_KERNEL);
	sched_cfg = kzalloc(sizeof(*sched_cfg), GFP_KERNEL);
	if (!queue_cfg || !sched_cfg)
		goto ERROR_EXIT;

	if (info->node_type == DP_NODE_QUEUE) {
		qos_queue_conf_set_default_32(queue_cfg);
		queue_cfg->queue_child_prop.parent = parent_node;
		dp_wred_def(queue_cfg);
		queue_cfg->queue_child_prop.priority = info->prio_wfq;
		/* convert q_id to logical node id and pass it to
		 * low level api
		 */
		node_id = priv->qos_queue_stat[info->node_id.q_id].node_id;
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Try to link Q[%d/%d] to parent[%d/%d] port[%d]\n",
			 info->node_id.q_id,
			 node_id,
			 info->p_node_id.cqm_deq_port,
			 queue_cfg->queue_child_prop.parent,
			 info->cqm_deq_port.cqm_deq_port);
		if (qos_queue_set_32(priv->qdev, node_id, queue_cfg)) {
			pr_err("failed to qos_queue_set_32\n");
			qos_queue_remove_32(priv->qdev, node_id);
			goto ERROR_EXIT;
		}
		goto EXIT;
	} else if (info->node_type == DP_NODE_SCH) {
		qos_sched_conf_set_default_32(sched_cfg);
		sched_cfg->sched_child_prop.parent = parent_node;
		sched_cfg->sched_child_prop.priority = info->prio_wfq;
		sched_cfg->sched_parent_prop.arbitration =
						arbi_dp2pp_32(info->arbi);
		node_id = info->node_id.sch_id;

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Try to link SCH[/%d] to parent[%d/%d] port[%d]\n",
			 node_id,
			 info->p_node_id.cqm_deq_port,
			 sched_cfg->sched_child_prop.parent,
			 info->cqm_deq_port.cqm_deq_port);

		if (qos_sched_set_32(priv->qdev, node_id, sched_cfg)) {
			pr_err("failed to %s %d parent_node %d\n",
			       "qos_sched_set_32 node_id",
			       node_id, parent_node);
			qos_sched_remove_32(priv->qdev, node_id);
			goto ERROR_EXIT;
		}
		node_flag |= P_FLAG;
		goto EXIT;
	}
	goto ERROR_EXIT;
EXIT:
	res = DP_SUCCESS;
	/* fill parent info in child's global table */
	priv->qos_sch_stat[node_id].parent.node_id = parent_node;
	priv->qos_sch_stat[node_id].parent.type = info->p_node_type;
	priv->qos_sch_stat[node_id].parent.flag = PP_NODE_ACTIVE;
	/* increase child_num in parent's global table and status */
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_stat_update after dp_link_set start\n");
	node_stat_update(info->inst, node_id, node_flag);
	node_stat_update(info->inst, parent_node, DP_NODE_INC | C_FLAG);
	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "node_stat_update after dp_link_set end\n");
ERROR_EXIT:

	kfree(queue_cfg);

	kfree(sched_cfg);
	return res;
}

/* get_parent_arbi API
 * return parent's arbi of given node
 * else return DP_FAILURE
 */
static int get_parent_arbi(int inst, int node_id, int flag)
{
	int pid, arbi;
	struct hal_priv *priv = HAL(inst);
	struct pp_qos_sched_conf sched_cfg = {0};
	struct pp_qos_port_conf port_cfg = {0};

	if (priv->qos_sch_stat[node_id].parent.flag == PP_NODE_FREE) {
		pr_err("Parent is not set for node\n");
		return DP_FAILURE;
	}
	pid = priv->qos_sch_stat[node_id].parent.node_id;

	if (priv->qos_sch_stat[node_id].parent.type == DP_NODE_SCH) {
		if (qos_sched_conf_get_32(priv->qdev, pid, &sched_cfg)) {
			pr_err("fail to get sched config\n");
			return DP_FAILURE;
		}
		arbi = arbi_pp2dp_32(sched_cfg.sched_parent_prop.arbitration);
		if (arbi == DP_FAILURE)
			pr_err("Wrong pp_arbitrate: %d for %s:%d\n",
			       port_cfg.port_parent_prop.arbitration,
			       (priv->qos_sch_stat[node_id].type == DP_NODE_SCH)
			       ? "sched" : "Q",
			       node_id);
	} else if (priv->qos_sch_stat[node_id].parent.type == DP_NODE_PORT) {
		if (qos_port_conf_get_32(priv->qdev, pid, &port_cfg)) {
			pr_err("fail to get port config\n");
			return DP_FAILURE;
		}
		arbi = arbi_pp2dp_32(port_cfg.port_parent_prop.arbitration);
		if (arbi == DP_FAILURE)
			pr_err("Wrong pp_arbitrate: %d for %s:%d\n",
			       port_cfg.port_parent_prop.arbitration,
			       (priv->qos_sch_stat[node_id].type == DP_NODE_SCH)
			       ? "sched" : "Q",
			       node_id);
	} else {
		pr_err("incorrect parent type:0x%x for node:%d.\n",
		       priv->qos_sch_stat[node_id].parent.type,
		       node_id);
		return DP_FAILURE;
	}
	return arbi;
}

/* set_parent_arbi API
 * set arbitration of node_id's all children and return DP_SUCCESS
 * else return DP_FAILURE
 */
static int set_parent_arbi(int inst, int node_id, int arbi, int flag)
{
	int pid;
	struct hal_priv *priv = HAL(inst);
	struct pp_qos_sched_conf sched_cfg = {0};
	struct pp_qos_port_conf port_cfg = {0};

	if (priv->qos_sch_stat[node_id].parent.flag == PP_NODE_FREE) {
		pr_err("Parent is not set for node\n");
		return DP_FAILURE;
	}
	pid = priv->qos_sch_stat[node_id].parent.node_id;

	arbi = arbi_dp2pp_32(arbi);

	if (arbi == DP_FAILURE) {
		pr_err("Incorrect arbitration value provided:%d!\n", arbi);
		return DP_FAILURE;
	}

	if (priv->qos_sch_stat[node_id].parent.type == DP_NODE_SCH) {
		if (qos_sched_conf_get_32(priv->qdev, pid, &sched_cfg)) {
			pr_err("fail to get sched config\n");
			return DP_FAILURE;
		}
		sched_cfg.sched_parent_prop.arbitration = arbi;
		if (qos_sched_set_32(priv->qdev, pid, &sched_cfg)) {
			pr_err("fail to set arbi sched:%d parent of node:%d\n",
			       pid, node_id);
			return DP_FAILURE;
		}
	} else if (priv->qos_sch_stat[node_id].parent.type == DP_NODE_PORT) {
		if (qos_port_conf_get_32(priv->qdev, pid, &port_cfg)) {
			pr_err("fail to get port config\n");
			return DP_FAILURE;
		}
		port_cfg.port_parent_prop.arbitration = arbi;
		if (qos_port_set_32(priv->qdev, pid, &port_cfg)) {
			pr_err("fail to set arbi port:%d parent of node:%d\n",
			       pid, node_id);
			return DP_FAILURE;
		}
	} else {
		pr_err("incorrect parent type:0x%x for node:%d.\n",
		       priv->qos_sch_stat[node_id].parent.type,
		       node_id);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}

/* dp_qos_link_prio_set_32 API
 * set node_prio struct for link and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_qos_link_prio_set_32(struct dp_node_prio *info, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct hal_priv *priv;
	int node_id;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (info->type == DP_NODE_QUEUE) {
		if ((info->id.q_id < 0) || (info->id.q_id >= MAX_QUEUE)) {
			pr_err("Invalid Queue ID:%d\n", info->id.q_id);
			return DP_FAILURE;
		}
		if (priv->qos_queue_stat[info->id.q_id].flag == PP_NODE_FREE) {
			pr_err("Invalid Queue flag:0x%x\n",
			       priv->qos_queue_stat[info->id.q_id].flag);
			return DP_FAILURE;
		}
		node_id = priv->qos_queue_stat[info->id.q_id].node_id;
		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("fail to get queue prio and parent\n");
			return DP_FAILURE;
		}
		queue_cfg.queue_child_prop.priority = info->prio_wfq;
		DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
			 "Prio:%d paased to low level for queue[%d]\n",
			 info->prio_wfq, info->id.q_id);
		if (qos_queue_set_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("failed to qos_queue_set_32\n");
			return DP_FAILURE;
		}
		/* get parent conf and set arbi in parent */
		if (set_parent_arbi(info->inst, node_id, info->arbi, flag)) {
			pr_err("fail to set arbi:%d in Parent of Q:%d\n",
			       info->arbi, node_id);
			return DP_FAILURE;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
			 "dp_qos_link_prio_set_32:Q=%d arbi=%d prio=%d\n",
			 info->id.q_id, info->arbi, info->prio_wfq);
		return DP_SUCCESS;
	} else if (info->type == DP_NODE_SCH) {
		if ((info->id.sch_id < 0) ||
		    (info->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Invalid Sched ID:%d\n", info->id.sch_id);
			return DP_FAILURE;
		}
		if (priv->qos_sch_stat[info->id.sch_id].p_flag ==
		    PP_NODE_FREE) {
			pr_err("Invalid Sched flag:0x%x\n",
			       priv->qos_sch_stat[info->id.sch_id].p_flag);
			return DP_FAILURE;
		}
		if (qos_sched_conf_get_32(priv->qdev, info->id.sch_id,
					  &sched_cfg)) {
			pr_err("fail to get sched prio and parent\n");
			return DP_FAILURE;
		}
		sched_cfg.sched_child_prop.priority = info->prio_wfq;
		DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
			 "Prio:%d paased to low level for Sched[%d]\n",
			 info->prio_wfq, info->id.sch_id);
		if (qos_sched_set_32(priv->qdev, info->id.sch_id, &sched_cfg)) {
			pr_err("failed to qos_sched_set_32\n");
			return DP_FAILURE;
		}
		/* get parent conf and set arbi in parent */
		if (set_parent_arbi(info->inst, info->id.sch_id,
				    info->arbi, 0)) {
			pr_err("fail to set arbi:%d Parent of Sched:%d\n",
			       info->arbi, info->id.sch_id);
			return DP_FAILURE;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
			 "dp_qos_link_prio_set_32:Sched=%d arbi=%d prio=%d\n",
			 info->id.sch_id, info->arbi, info->prio_wfq);
		return DP_SUCCESS;
	}
	pr_err("incorrect info type provided:0x%x\n", info->type);
	return DP_FAILURE;
}

/* dp_qos_link_prio_get_32 API
 * get node_prio struct for link and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_qos_link_prio_get_32(struct dp_node_prio *info, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct hal_priv *priv;
	int node_id, arbi;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (info->type == DP_NODE_QUEUE) {
		if ((info->id.q_id < 0) || (info->id.q_id >= MAX_QUEUE)) {
			pr_err("Invalid Queue ID:%d\n", info->id.q_id);
			return DP_FAILURE;
		}
		if (priv->qos_queue_stat[info->id.q_id].flag == PP_NODE_FREE) {
			pr_err("Invalid Queue flag:%d\n",
			       priv->qos_queue_stat[info->id.q_id].flag);
			return DP_FAILURE;
		}
		node_id = priv->qos_queue_stat[info->id.q_id].node_id;
		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("fail to get queue prio_wfq value!\n");
			return DP_FAILURE;
		}

		arbi = get_parent_arbi(info->inst, node_id, flag);

		if (arbi == DP_FAILURE)
			return DP_FAILURE;

		info->arbi = arbi;
		info->prio_wfq = queue_cfg.queue_child_prop.priority;
		return DP_SUCCESS;
	} else if (info->type == DP_NODE_SCH) {
		if ((info->id.sch_id < 0) ||
		    (info->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Invalid Sched ID:%d\n", info->id.sch_id);
			return DP_FAILURE;
		}
		if (priv->qos_sch_stat[info->id.sch_id].p_flag ==
		    PP_NODE_FREE) {
			pr_err("Invalid Sched flag:%d\n",
			       priv->qos_sch_stat[info->id.sch_id].p_flag);
			return DP_FAILURE;
		}
		if (qos_sched_conf_get_32(priv->qdev, info->id.sch_id,
					  &sched_cfg)) {
			pr_err("fail to get sched arbi and prio values!\n");
			return DP_FAILURE;
		}

		arbi = get_parent_arbi(info->inst, info->id.sch_id, flag);

		if (arbi == DP_FAILURE)
			return DP_FAILURE;

		info->arbi = arbi;
		info->prio_wfq = sched_cfg.sched_child_prop.priority;
		return DP_SUCCESS;
	}
	pr_err("incorrect info type provided:0x%x\n", info->type);
	return DP_FAILURE;
}

/* dp_deq_port_res_get_32 API
 * Remove link of attached nodes and return DP_SUCCESS
 * else return DP_FAILURE
 */
union local_t {
	struct pp_qos_port_info p_info;
	struct pp_qos_queue_info q_info;
	struct pp_qos_queue_conf q_conf;
	struct pp_qos_sched_conf sched_conf;
};

static union local_t t;

int dp_deq_port_res_get_32(struct dp_dequeue_res *res, int flag)
{
	struct hal_priv *priv;
	u16 q_ids[MAX_Q_PER_PORT] = {0};
	unsigned int q_num;
	unsigned int q_size = MAX_Q_PER_PORT;
	int i, j, k;
	int port_num = 1;
	int p_id, idx;
	struct pmac_port_info *port_info;

	if (!res) {
		pr_err("res cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(res->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	port_info = get_dp_port_info(res->inst, res->dp_port);
	if (!port_info->deq_port_num)
		return DP_FAILURE;
	if (res->cqm_deq_idx == DEQ_PORT_OFFSET_ALL) {
		port_num = port_info->deq_port_num;
		res->cqm_deq_port = port_info->deq_port_base;
		res->num_deq_ports = port_info->deq_port_num;
	} else {
		res->cqm_deq_port = port_info->deq_port_base + res->cqm_deq_idx;
		res->num_deq_ports = 1;
	}
	DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
		 "dp_deq_port_res_get_32:dp_port=%d cqm_deq_port=(%d ~%d) %d\n",
		 res->dp_port,
		 res->cqm_deq_port, res->cqm_deq_port + port_num - 1,
		 port_info->deq_port_num);
	res->num_q = 0;
	idx = 0;
	for (k = res->cqm_deq_port;
	     k < (res->cqm_deq_port + port_num);
	     k++) {
		if (priv->deq_port_stat[k].flag == PP_NODE_FREE)
			continue;
		q_num = 0;
		if (qos_port_get_queues_32(priv->qdev,
					   priv->deq_port_stat[k].node_id,
					   q_ids, q_size, &q_num)) {
			pr_err("qos_port_get_queues_32: port[%d/%d]\n",
			       k,
			       priv->deq_port_stat[k].node_id);
			return DP_FAILURE;
		}
		res->num_q += q_num;
		if (!res->q_res)
			continue;
		DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL, "port[%d/%d] queue list\n",
			 k, priv->deq_port_stat[k].node_id);
		for (i = 0; i < q_num; i++)
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "  q[/%d]\n", q_ids[i]);
		for (i = 0; (i < q_num) && (idx < res->q_res_size); i++) {
			memset(&t.q_info, 0, sizeof(t.q_info));
			if (qos_queue_info_get_32(priv->qdev,
						  q_ids[i], &t.q_info)) {
				pr_err("qos_port_info_get_32 fail:q[/%d]\n",
				       q_ids[i]);
				continue;
			}
			res->q_res[idx].q_id = t.q_info.physical_id;
			res->q_res[idx].q_node = q_ids[i];
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL, "q[%d/%d]\n",
				 t.q_info.physical_id, q_ids[i]);
			res->q_res[idx].sch_lvl = 0;
			memset(&t.q_conf, 0, sizeof(t.q_conf));
			if (qos_queue_conf_get_32(priv->qdev,
						  q_ids[i], &t.q_conf)) {
				pr_err("qos_port_conf_get_32 fail:q[/%d]\n",
				       q_ids[i]);
				continue;
			}
			p_id = t.q_conf.queue_child_prop.parent;
			j = 0;
			do {
				if (priv->qos_sch_stat[p_id].type ==
				    DP_NODE_PORT) {/* port */
					res->q_res[idx].qos_deq_port = p_id;
					res->q_res[idx].cqm_deq_port = k;
					break;
				} else if (priv->qos_sch_stat[p_id].type !=
					   DP_NODE_SCH) {
					pr_err("wrong p[/%d] type:%d\n",
					       p_id,
					       priv->qos_sch_stat[p_id].type);
					break;
				}
				/* for sched as parent */
				res->q_res[idx].sch_id[j] = p_id;
				j++;
				res->q_res[idx].sch_lvl = j;
				/* get next parent */
				if (qos_sched_conf_get_32(priv->qdev, p_id,
							  &t.sched_conf)) {
					pr_err(
					    "qos_sched_conf_get_32 %s[/%d]\n",
					    "fail:sch", p_id);
					break;
				}
				p_id = t.sched_conf.sched_child_prop.parent;
			} while (1);
			idx++;
		}
	}
	return DP_SUCCESS;
}

/* dp_node_unlink_32 API
 * check child node keep queue in blocked state
 * flush queues and return DP_SUCCESS
 * Else return DP_FAILURE
 */
int dp_node_unlink_32(struct dp_node_link *info, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct hal_priv *priv;
	u16 queue_buf[MAX_Q_PER_PORT] = {0};
	int queue_size = MAX_Q_PER_PORT;
	int queue_num = 0;
	int i, node_id;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (info->node_type == DP_NODE_QUEUE) {
		node_id = priv->qos_queue_stat[info->node_id.q_id].node_id;
		/* Need to check ACTIVE Flag */
		if (!(priv->qos_queue_stat[info->node_id.q_id].flag &
		    PP_NODE_ACTIVE)) {
			pr_err("Wrong Queue[%d] Stat(%d):Expect ACTIVE\n",
			       info->node_id.q_id,
			       priv->qos_queue_stat[info->node_id.q_id].flag);
		}
		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg) == 0)
			queue_flush_32(info->inst, node_id, 0);
	} else if (info->node_type == DP_NODE_SCH) {
		if (!(priv->qos_sch_stat[info->node_id.sch_id].c_flag &
								PP_NODE_ACTIVE))
			pr_err("Wrong Sched FLAG Expect ACTIVE\n");
		if (qos_sched_conf_get_32(priv->qdev, info->node_id.sch_id,
					  &sched_cfg))
			return DP_FAILURE;
		if (qos_sched_get_queues_32(priv->qdev, info->node_id.sch_id,
					    queue_buf, queue_size, &queue_num))
			return DP_FAILURE;
		for (i = 0; i < queue_num; i++) {
			if (qos_queue_conf_get_32(priv->qdev, queue_buf[i],
						  &queue_cfg))
				continue;
			queue_flush_32(info->inst, queue_buf[i], 0);
		}
	}
	return DP_SUCCESS;
}

struct link_add_var {
	struct pp_qos_queue_conf queue_cfg;
	struct pp_qos_sched_conf sched_cfg;
	struct dp_node_alloc node;
	u16 queue_buf[MAX_Q_PER_PORT];
	int q_orig_block[MAX_Q_PER_PORT];
	int q_orig_suspend[MAX_Q_PER_PORT];
	int queue_size;
	int queue_num;
	int node_id;
	struct pp_node parent;
	int f_child_free;
	int f_parent_free;
	int f_sch_auto_id;
	int f_restore;
};

/* dp_node_link_add_32 API
 * check for parent type and allocate parent node
 * then check for child type and allocate child node
 * then call dp_link_set api to link child to parent
 * upon success links node to given parent and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_node_link_add_32(struct dp_node_link *info, int flag)
{
	#define DP_SUSPEND(t) ((t)->queue_cfg.common_prop.suspended)
	int i;
	int res = DP_SUCCESS;
	struct hal_priv *priv;
	struct link_add_var *t = NULL;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if ((!info->dp_port) && (info->dp_port != DP_PORT(info).dp_port)) {
		pr_err("Fix wrong dp_port from %d to %d\n",
		       info->dp_port, DP_PORT(info).dp_port);
		info->dp_port = DP_PORT(info).dp_port;
	}
	t = kzalloc(sizeof(*t), GFP_ATOMIC);
	if (!t) {
		pr_err("fail to alloc %zd bytes\n", sizeof(*t));
		return DP_FAILURE;
	}
	for (i = 0; i < ARRAY_SIZE(t->q_orig_block); i++) {
		t->q_orig_block[i] = -1;
		t->q_orig_suspend[i] = -1;
	}
	t->queue_size = MAX_Q_PER_PORT;

	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "inst=%d dp_port=%d\n",
		 info->inst, info->dp_port);
	/* Get Parent node_id after sanity check */
	if (info->p_node_type == DP_NODE_SCH &&
	    info->p_node_id.sch_id == DP_NODE_AUTO_ID)
		t->f_sch_auto_id = 1;
	i = dp_qos_parent_chk(info, flag);
	if (i == DP_FAILURE) {
		pr_err("dp_qos_parent_chk fail\n");
		goto EXIT_ERR;
	}
	t->parent.node_id = i;
	t->parent.type = info->p_node_type;
	t->parent.flag = 1;

	/* Check parent's children limit not exceeded */
	if (priv->qos_sch_stat[t->parent.node_id].child_num >=
	    DP_MAX_CHILD_PER_NODE) {
		pr_err("Child Num:%d is exceeding limit for Node:[%d]\n",
		       priv->qos_sch_stat[t->parent.node_id].child_num,
		       t->parent.node_id);
		goto EXIT_ERR;
	}

	DP_DEBUG(DP_DBG_FLAG_QOS,
		 "dp_qos_parent_chk succeed: parent node %d\n",
		 t->parent.node_id);
	/* workaround to pass parrent to queue allcoate api */
	priv->ppv4_tmp_p = t->parent.node_id;
	if (t->f_sch_auto_id)
		t->f_parent_free = 1;

	/* Get Child node after sanity check */
	if (info->node_type == DP_NODE_QUEUE) {
		if (info->node_id.q_id == DP_NODE_AUTO_ID) {
			t->node.inst = info->inst;
			t->node.dp_port = info->dp_port;
			t->node.type = info->node_type;
			if ((dp_node_alloc_32(&t->node, flag)) == DP_FAILURE) {
				pr_err("dp_node_alloc_32 queue alloc fail\n");
				goto EXIT_ERR;
			}
			info->node_id = t->node.id;
			t->f_child_free = 1;
		}
		/* add check for free flag and error */
		if (priv->qos_queue_stat[info->node_id.q_id].flag ==
		    PP_NODE_FREE) {
			pr_err("Queue ID:%d is in Free state:0x%x\n",
			       info->node_id.q_id,
			       priv->qos_queue_stat[info->node_id.q_id].flag);
			goto EXIT_ERR;
		}
		/* convert q_id to logical node id and pass it to
		 * low level api
		 */

		t->node_id = priv->qos_queue_stat[info->node_id.q_id].node_id;
		if (qos_queue_conf_get_32(priv->qdev, t->node_id,
					  &t->queue_cfg) == 0) {
			t->queue_num = 1;
			t->queue_buf[0] = t->node_id;
			/* save original block/suspend status */
			if (t->queue_cfg.blocked == 0)
				t->q_orig_block[0] = t->queue_cfg.blocked;

			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "qos_queue_flush_32 queue[%d]\n", t->node_id);
			queue_flush_32(info->inst, t->node_id,
				       FLUSH_RESTORE_LOOKUP);
			if (t->queue_cfg.queue_child_prop.parent !=
					 priv->ppv4_drop_p) {/* decrease stat */
				/* Child flag update before link */
				DP_DEBUG(DP_DBG_FLAG_QOS,
					 "node_stat_update for queue[%d]\n",
					 t->node_id);

				if (node_stat_update(info->inst, t->node_id,
						     DP_NODE_DEC)) {
					pr_err("node_stat_update fail\n");
					goto EXIT_ERR;
				}
				/* reduce child_num in parent's global table */
				DP_DEBUG(DP_DBG_FLAG_QOS,
					 "node_stat_update parent %d for q[%d]\n",
					 PARENT(t->queue_cfg), t->node_id);
				if (node_stat_update(info->inst,
						     PARENT(t->queue_cfg),
						     DP_NODE_DEC | C_FLAG)) {
					pr_err("node_stat_update fail\n");
					goto EXIT_ERR;
				}
			}
		}
		/* link set */
		/* if parent is same, but need to fill in other parameters for
		 * parents hence commenting below code
		 */
		/* if (info->p_node_id.sch_id == parent.node_id ||
		 *    info->p_node_id.cqm_deq_port == parent.node_id)
		 *	goto EXIT_ERR;
		 */
		if (dp_link_set(info, t->parent.node_id, flag)) {
			pr_err("dp_link_set fail to link to parent\n");
			goto EXIT_ERR;
		}
	} else if (info->node_type == DP_NODE_SCH) {
		if (info->node_id.sch_id == DP_NODE_AUTO_ID) {
			t->node.inst = info->inst;
			t->node.dp_port = info->dp_port;
			t->node.type = info->node_type;

			if ((dp_node_alloc_32(&t->node, flag)) == DP_FAILURE) {
				pr_err("dp_node_alloc_32 sched alloc fail\n");
				goto EXIT_ERR;
			}
			info->node_id = t->node.id;
			t->f_child_free = 1;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
			 "inst=%d dp_port=%d type=%d info->node_id %d\n",
			 t->node.inst, t->node.dp_port, t->node.type,
			 info->node_id.q_id);
		/* add check for free flag and error */
		if (priv->qos_sch_stat[info->node_id.sch_id].p_flag ==
		    PP_NODE_FREE) {
			pr_err("Sched:%d is in Free state:0x%x\n",
			       info->node_id.sch_id,
			       priv->qos_sch_stat[info->node_id.sch_id].p_flag);
			goto EXIT_ERR;
		}
		if ((t->f_child_free == 0) &&
		    qos_sched_conf_get_32(priv->qdev, info->node_id.sch_id,
					  &t->sched_cfg) == 0) {
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "info->node_id.sch_id %d\n",
				 info->node_id.sch_id);
			if (qos_sched_get_queues_32(priv->qdev,
						    info->node_id.sch_id,
						    t->queue_buf, t->queue_size,
						    &t->queue_num)) {
				pr_err("Can not get queues:%d\n",
				       info->node_id.sch_id);
				goto EXIT_ERR;
			}
			for (i = 0; i < t->queue_num; i++) {
				if (qos_queue_conf_get_32(priv->qdev,
							  t->queue_buf[i],
							  &t->queue_cfg))
					continue;
				if (t->queue_cfg.blocked == 0)
					t->q_orig_block[i] =
						t->queue_cfg.blocked;

				queue_flush_32(info->inst, t->queue_buf[i],
					       FLUSH_RESTORE_LOOKUP);
			}
			/* update flag for sch node */
			if (node_stat_update(info->inst, info->node_id.sch_id,
					     DP_NODE_DEC | P_FLAG)) {
				pr_err("node_stat_update fail\n");
				goto EXIT_ERR;
			}
			/* reduce child_num in parent's global table */
			if (node_stat_update(info->inst, PARENT_S(t->sched_cfg),
					     DP_NODE_DEC | C_FLAG)) {
				pr_err("node_stat_update fail\n");
				goto EXIT_ERR;
			}
		}
		/* if parent is same, but need to fill in other parameters for
		 * parents hence commenting below code
		 */
		/* if (info->p_node_id.sch_id == parent.node_id ||
		 *    info->p_node_id.cqm_deq_port == parent.node_id)
		 *	goto EXIT_ERR;
		 */
		if (dp_link_set(info, t->parent.node_id, flag)) {
			pr_err("dp_link_set failed to link to parent\n");
			goto EXIT_ERR;
		}
	}

	for (i = 0; i <= t->queue_num; i++) {
		if ((t->q_orig_block[i] < 0) &&/* non-valid block stat */
		    (t->q_orig_suspend[i] < 0))/* non-valid suspend stat */
			continue;
		if (qos_queue_conf_get_32(priv->qdev, t->queue_buf[i],
					  &t->queue_cfg))
			continue;
		t->f_restore = 0;
		if (t->q_orig_block[i] >= 0) {
			t->f_restore = 1;
			t->queue_cfg.blocked = t->q_orig_block[i];/* restore */
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "to unblock queue[%d/%d]:block=%d\n",
				 get_qid_by_node(info->inst,
						 t->queue_buf[i], 0),
				 t->queue_buf[i],
				 t->queue_cfg.blocked);
		}

		if (!t->f_restore)
			continue;
		if (qos_queue_set_32(priv->qdev, t->queue_buf[i],
				     &t->queue_cfg)) {
			pr_err("qos_queue_set_32 fail for q[/%d]\n",
			       t->queue_buf[i]);
			res = DP_FAILURE;
		}
	}
	kfree(t);
	t = NULL;
	return res;

EXIT_ERR:
	res = DP_FAILURE;
	if (t->f_child_free) {
		if (t->node.type == DP_NODE_QUEUE) {
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "queue remove id %d\n", t->node_id);
			qos_queue_remove_32(priv->qdev, t->node_id);
		} else if (t->node.type == DP_NODE_SCH) {
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "sched remove id %d\n", t->node.id.sch_id);
			qos_sched_remove_32(priv->qdev, t->node.id.sch_id);
		} else {
			pr_err("Unexpect node type %d\n", t->node.type);
		}
	}
	if (t->f_parent_free) {
		if (info->p_node_type == DP_NODE_PORT) {
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "port remove id %d\n", t->parent.node_id);
			qos_port_remove_32(priv->qdev, t->parent.node_id);
		} else if (info->p_node_type == DP_NODE_SCH) {
			DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
				 "sched remove id %d\n", t->parent.node_id);
			qos_sched_remove_32(priv->qdev, t->parent.node_id);
		} else {
			pr_err("Unexpect node type %d\n", t->node.type);
		}
	}

	kfree(t);
	t = NULL;
	return res;
}

/* dp_queue_conf_set_32 API
 * Set Current Queue config and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_queue_conf_set_32(struct dp_queue_conf *cfg, int flag)
{
	struct pp_qos_queue_conf *conf;
	struct hal_priv *priv;
	int node_id, res = DP_FAILURE;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	conf = kzalloc(sizeof(*conf), GFP_ATOMIC);
	if (!conf) {
		pr_err("fail to alloc %zd bytes\n", sizeof(*conf));
		return DP_FAILURE;
	}

	if ((cfg->q_id < 0) || (cfg->q_id >= MAX_QUEUE)) {
		pr_err("Invalid Queue ID:%d\n", cfg->q_id);
		goto EXIT;
	}
	if (priv->qos_queue_stat[cfg->q_id].flag == PP_NODE_FREE) {
		pr_err("Invalid Queue flag:%d\n",
		       priv->qos_queue_stat[cfg->q_id].flag);
		goto EXIT;
	}
	node_id = priv->qos_queue_stat[cfg->q_id].node_id;

	if (qos_queue_conf_get_32(priv->qdev, node_id, conf)) {
		pr_err("qos_queue_conf_get_32 fail:%d\n", cfg->q_id);
		goto EXIT;
	}
	if (flag & (cfg->act & DP_NODE_DIS))
		conf->blocked = 1;
	else if (flag & (cfg->act & DP_NODE_EN))
		conf->blocked = 0;

	if (flag & (cfg->drop == DP_QUEUE_DROP_WRED)) {
		conf->wred_enable = 1;
		conf->wred_max_avg_green = cfg->max_size[0];
		conf->wred_max_avg_yellow = cfg->max_size[1];
		conf->wred_min_avg_green = cfg->min_size[0];
		conf->wred_min_avg_yellow = cfg->min_size[1];
		conf->wred_slope_green = cfg->wred_slope[0];
		conf->wred_slope_yellow = cfg->wred_slope[1];
		conf->wred_min_guaranteed = cfg->wred_min_guaranteed;
		conf->wred_max_allowed = cfg->wred_max_allowed;
	} else if (flag & (cfg->drop == DP_QUEUE_DROP_TAIL)) {
		pr_err("Further check PPv4 Tail Drop Capability.\n");
		conf->wred_enable = 0;
		conf->wred_min_avg_green = cfg->min_size[0];
		conf->wred_min_avg_yellow = cfg->min_size[1];
		conf->wred_min_guaranteed = cfg->wred_min_guaranteed;
		conf->wred_max_allowed = cfg->wred_max_allowed;
	}
	if (qos_queue_set_32(priv->qdev, node_id, conf)) {
		pr_err("failed to qos_queue_set_32:%d\n", cfg->q_id);
		goto EXIT;
	}
	res = DP_SUCCESS;

EXIT:
	kfree(conf);
	conf = NULL;
	return res;
}

/* dp_queue_conf_get_32 API
 * Get Current Queue config and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_queue_conf_get_32(struct dp_queue_conf *cfg, int flag)
{
	int node_id, res = DP_FAILURE;
	struct pp_qos_queue_conf *conf;
	struct hal_priv *priv;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	conf = kzalloc(sizeof(*conf), GFP_ATOMIC);
	if (!conf) {
		pr_err("fail to alloc %zd bytes\n", sizeof(*conf));
		return DP_FAILURE;
	}

	if ((cfg->q_id < 0) || (cfg->q_id >= MAX_QUEUE)) {
		pr_err("Invalid Queue ID:%d\n", cfg->q_id);
		goto EXIT;
	}
	if (priv->qos_queue_stat[cfg->q_id].flag == PP_NODE_FREE) {
		pr_err("Invalid Queue flag:%d\n",
		       priv->qos_queue_stat[cfg->q_id].flag);
		goto EXIT;
	}
	node_id = priv->qos_queue_stat[cfg->q_id].node_id;

	if (qos_queue_conf_get_32(priv->qdev, node_id, conf)) {
		pr_err("qos_queue_conf_get_32 fail\n");
		goto EXIT;
	}

	if (conf->blocked)
		cfg->act = DP_NODE_DIS;
	else
		cfg->act = DP_NODE_EN;

	if (conf->wred_enable) {
		cfg->drop = DP_QUEUE_DROP_WRED;
		cfg->wred_slope[0] = conf->wred_slope_green;
		cfg->wred_slope[1] = conf->wred_slope_yellow;
		cfg->wred_slope[2] = 0;
		cfg->wred_max_allowed = conf->wred_max_allowed;
		cfg->wred_min_guaranteed = conf->wred_min_guaranteed;
		cfg->min_size[0] = conf->wred_min_avg_green;
		cfg->min_size[1] = conf->wred_min_avg_yellow;
		cfg->min_size[2] = 0;
		cfg->max_size[0] = conf->wred_max_avg_green;
		cfg->max_size[1] = conf->wred_max_avg_yellow;
		cfg->max_size[2] = 0;
		//cfg->unit = conf->max_burst;
		res = DP_SUCCESS;
		goto EXIT;
	}
	cfg->drop = DP_QUEUE_DROP_TAIL;
	cfg->min_size[0] = conf->wred_min_avg_green;
	cfg->min_size[1] = conf->wred_min_avg_yellow;
	cfg->max_size[0] = conf->wred_max_avg_green;
	cfg->max_size[1] = conf->wred_max_avg_yellow;
	//cfg->unit = conf->max_burst;
	res = DP_SUCCESS;

EXIT:
	kfree(conf);
	conf = NULL;
	return res;
}

/* dp_node_link_en_set_32 API
 * Enable current link node and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_node_link_en_set_32(struct dp_node_link_enable *en, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct pp_qos_port_conf port_cfg = {0};
	struct hal_priv *priv;
	int node_id;

	if (!en) {
		pr_err("en info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(en->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if ((en->en & DP_NODE_EN) && (en->en & DP_NODE_DIS)) {
		pr_err("enable & disable cannot be set together!\n");
		return DP_FAILURE;
	}
	if ((en->en & DP_NODE_SUSPEND) && (en->en & DP_NODE_RESUME)) {
		pr_err("suspend & resume cannot be set together!\n");
		return DP_FAILURE;
	}

	if (en->type == DP_NODE_QUEUE) {
		if (!(en->en & (DP_NODE_EN | DP_NODE_DIS | DP_NODE_SUSPEND |
				    DP_NODE_RESUME))) {
			pr_err("Incorrect commands provided!\n");
			return DP_FAILURE;
		}
		if ((en->id.q_id < 0) || (en->id.q_id >= MAX_QUEUE)) {
			pr_err("Wrong Parameter: QID[%d]Out Of Range\n",
			       en->id.q_id);
			return DP_FAILURE;
		}
		node_id = priv->qos_queue_stat[en->id.q_id].node_id;

		if (priv->qos_queue_stat[en->id.q_id].flag == PP_NODE_FREE) {
			pr_err("Node Q[%d] is not allcoated\n", en->id.q_id);
			return DP_FAILURE;
		}
		if (en->en & DP_NODE_EN) {
			if (pp_qos_queue_unblock(priv->qdev, node_id)) {
				pr_err("pp_qos_queue_unblock fail Queue[%d]\n",
				       en->id.q_id);
				return DP_FAILURE;
			}
		}
		if (en->en & DP_NODE_DIS) {
			if (pp_qos_queue_block(priv->qdev, node_id)) {
				pr_err("pp_qos_queue_block fail Queue[%d]\n",
				       en->id.q_id);
				return DP_FAILURE;
			}
		}

		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("qos_queue_conf_get_32 fail: q[%d]\n",
			       en->id.q_id);
			return DP_FAILURE;
		}
		if (en->en & DP_NODE_EN) {
			if (queue_cfg.blocked) {
				pr_err("Incorrect value set for Queue[%d]:%d\n",
				       en->id.q_id,
				       queue_cfg.blocked);
				return DP_FAILURE;
			}
		}
		if (en->en & DP_NODE_DIS) {
			if (!queue_cfg.blocked) {
				pr_err("Incorrect value set for Queue[%d]:%d\n",
				       en->id.q_id,
				       queue_cfg.blocked);
				return DP_FAILURE;
			}
		}

	} else if (en->type == DP_NODE_SCH) {
		if (!(en->en & (DP_NODE_SUSPEND | DP_NODE_RESUME))) {
			pr_err("Incorrect commands provided!\n");
			return DP_FAILURE;
		}
		if ((en->id.sch_id < 0) || (en->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Wrong Parameter: Sched[%d]Out Of Range\n",
			       en->id.sch_id);
			return DP_FAILURE;
		}
		if (priv->qos_sch_stat[en->id.sch_id].p_flag == PP_NODE_FREE) {
			pr_err("Node Sched[%d] is not allcoated\n",
			       en->id.sch_id);
			return DP_FAILURE;
		}

		if (qos_sched_conf_get_32(priv->qdev, en->id.sch_id,
					  &sched_cfg)) {
			pr_err("qos_sched_conf_get_32 fail: sch[%d]\n",
			       en->id.sch_id);
			return DP_FAILURE;
		}

	} else if (en->type == DP_NODE_PORT) {
		if (!(en->en & (DP_NODE_EN | DP_NODE_DIS | DP_NODE_SUSPEND |
				    DP_NODE_RESUME))) {
			pr_err("Incorrect commands provided!\n");
			return DP_FAILURE;
		}
		if ((en->id.cqm_deq_port < 0) ||
		    (en->id.cqm_deq_port >= MAX_CQM_DEQ)) {
			pr_err("Wrong Parameter: Port[%d]Out Of Range\n",
			       en->id.cqm_deq_port);
			return DP_FAILURE;
		}
		if (priv->deq_port_stat[en->id.cqm_deq_port].flag ==
		    PP_NODE_FREE) {
			pr_err("Node Port[%d] is not allcoated\n",
			       en->id.cqm_deq_port);
			return DP_FAILURE;
		}
		node_id = priv->deq_port_stat[en->id.cqm_deq_port].node_id;
		if (en->en & DP_NODE_EN) {
			if (pp_qos_port_unblock(priv->qdev, node_id)) {
				pr_err("pp_qos_port_unblock fail Port[%d]\n",
				       en->id.cqm_deq_port);
				return DP_FAILURE;
			}
		}
		if (en->en & DP_NODE_DIS) {
			if (pp_qos_port_block(priv->qdev, node_id)) {
				pr_err("pp_qos_port_block fail Port[%d]\n",
				       en->id.cqm_deq_port);
				return DP_FAILURE;
			}
		}
		if (en->en & DP_NODE_SUSPEND) {
			if (pp_qos_port_disable(priv->qdev, node_id)) {
				pr_err("pp_qos_port_disable fail Port[%d]\n",
				       en->id.cqm_deq_port);
				return DP_FAILURE;
			}
		}
		if (en->en & DP_NODE_RESUME) {
			if (pp_qos_port_enable(priv->qdev, node_id)) {
				pr_err("pp_qos_port_enable fail Port[%d]\n",
				       en->id.cqm_deq_port);
				return DP_FAILURE;
			}
		}
		if (qos_port_conf_get_32(priv->qdev, node_id, &port_cfg)) {
			pr_err("qos_port_conf_get_32 fail: port[%d]\n",
			       en->id.cqm_deq_port);
			return DP_FAILURE;
		}
		if (en->en & DP_NODE_SUSPEND) {
			if (!port_cfg.disable) {
				pr_err("Incorrect value set for Port[%d]:%d\n",
				       en->id.cqm_deq_port,
				       port_cfg.disable);
				return DP_FAILURE;
			}
		}
		if (en->en & DP_NODE_RESUME) {
			if (port_cfg.disable) {
				pr_err("Incorrect value set for Port[%d]:%d\n",
				       en->id.cqm_deq_port,
				       port_cfg.disable);
				return DP_FAILURE;
			}
		}
	}
	return DP_SUCCESS;
}

/* dp_node_link_en_get_32 API
 * Get status of link node and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_node_link_en_get_32(struct dp_node_link_enable *en, int flag)
{
	int node_id;
	struct hal_priv *priv = HAL(en->inst);

	if (!priv || !priv->qdev) {
		pr_err("priv or priv->qdev NULL\n");
		return DP_FAILURE;
	}
	if (!en) {
		pr_err("en info NULL\n");
		return DP_FAILURE;
	}
	if (en->type == DP_NODE_QUEUE) {
		struct pp_qos_queue_conf q_conf = {0};

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "en->id.q_id=%d\n", en->id.q_id);
		node_id = priv->qos_queue_stat[en->id.q_id].node_id;
		if (qos_queue_conf_get_32(priv->qdev, node_id, &q_conf)) {
			pr_err("qos_queue_conf_get_32 fail: q[%d]\n",
			       en->id.q_id);
			return DP_FAILURE;
		}
		if (q_conf.blocked)
			en->en |= DP_NODE_DIS;
		else
			en->en |= DP_NODE_EN;
	} else if (en->type == DP_NODE_SCH) {
		struct pp_qos_sched_conf sched_conf = {0};

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "en->id.sch_id=%d\n", en->id.sch_id);
		if (qos_sched_conf_get_32(priv->qdev, en->id.sch_id,
					  &sched_conf)) {
			pr_err("qos_sched_conf_get_32 fail: sched[/%d]\n",
			       en->id.sch_id);
			return DP_FAILURE;
		}
		en->en |= DP_NODE_EN;
	} else if (en->type == DP_NODE_PORT) {
		struct pp_qos_port_conf p_conf = {0};

		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "en->id.cqm_deq_port=%d\n", en->id.cqm_deq_port);
		node_id = priv->deq_port_stat[en->id.cqm_deq_port].node_id;
		if (qos_port_conf_get_32(priv->qdev, node_id, &p_conf)) {
			pr_err("qos_queue_conf_get_32 fail: port[%d]\n",
			       en->id.cqm_deq_port);
			return DP_FAILURE;
		}
		if (p_conf.disable)
			en->en |= DP_NODE_DIS;
		else
			en->en |= DP_NODE_EN;
	}
	return DP_SUCCESS;
}

/* dp_link_get_32 API
 * get full link based on queue and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_link_get_32(struct dp_qos_link *cfg, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct hal_priv *priv;
	int i, node_id;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	node_id = priv->qos_queue_stat[cfg->q_id].node_id;

	if (!(priv->qos_queue_stat[cfg->q_id].flag & PP_NODE_ACTIVE)) {
		pr_err("Incorrect queue:%d state:expect ACTIV\n", cfg->q_id);
		return DP_FAILURE;
	}

	if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
		pr_err("failed to qos_queue_conf_get_32\n");
		return DP_FAILURE;
	}
	cfg->q_arbi = get_parent_arbi(cfg->inst, node_id, 0);
	cfg->q_leaf = 0;
	cfg->n_sch_lvl = 0;
	cfg->q_prio_wfq = queue_cfg.queue_child_prop.priority;

	if (priv->qos_sch_stat[node_id].parent.type == DP_NODE_PORT) {
		cfg->cqm_deq_port = priv->qos_sch_stat[node_id].parent.node_id;
		return DP_SUCCESS;
	} else if (priv->qos_sch_stat[node_id].parent.type == DP_NODE_SCH) {
		for (i = 0; i < DP_MAX_SCH_LVL - 1; i++) {
			cfg->sch[i].id =
				priv->qos_sch_stat[node_id].parent.node_id;
			node_id = cfg->sch[i].id;
			cfg->n_sch_lvl = i + 1;
			cfg->sch[i].leaf = 0;
			cfg->sch[i].arbi = get_parent_arbi(cfg->inst,
							   cfg->sch[i].id, 0);
			cfg->sch[i + 1].id =
				priv->qos_sch_stat[node_id].parent.node_id;
			if (qos_sched_conf_get_32(priv->qdev, cfg->sch[i].id,
						  &sched_cfg)) {
				pr_err("dp_link_get:sched[/%d] conf get fail\n",
				       cfg->sch[i].id);
				return DP_FAILURE;
			}
			cfg->sch[i].prio_wfq =
					sched_cfg.sched_child_prop.priority;
			if (priv->qos_sch_stat[cfg->sch[i].id].parent.type ==
			    DP_NODE_PORT)
				break;
		}
		cfg->cqm_deq_port =
			priv->qos_sch_stat[cfg->sch[i].id].parent.node_id;
		return DP_SUCCESS;
	}
	return DP_FAILURE;
}

/* dp_link_add_32 API
 * configure end to end link and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_link_add_32(struct dp_qos_link *cfg, int flag)
{
	struct dp_node_link info = {0};
	int i;
	int f_q_free = 0;
	int f_q_auto = 0; /* flag if node is DP_NODE_AUTO_ID */
	struct f {
		u16 flag;
		u16 sch_id;
		u16 f_auto; /* flag if node is DP_NODE_AUTO_ID */
	};
	struct f f_sch_free[DP_MAX_SCH_LVL] = {0};

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}

	if (cfg->n_sch_lvl > DP_MAX_SCH_LVL) {
		pr_err("Incorrect sched_lvl:%s(%d) > %s(%d)\n",
		       "cfg->n_sch_lvl", cfg->n_sch_lvl,
		       "DP_MAX_SCH_LVL", DP_MAX_SCH_LVL);
		return DP_FAILURE;
	}

	info.inst = cfg->inst;
	info.dp_port = cfg->dp_port;
	info.node_id.q_id = cfg->q_id;
	info.cqm_deq_port.cqm_deq_port = cfg->cqm_deq_port;

	if (cfg->q_id == DP_NODE_AUTO_ID)
		f_q_auto = 1;

	DP_DEBUG(DP_DBG_FLAG_QOS_DETAIL,
		 "inst=%d dp_port=%d q_id=%d cqm_deq_port=%d n_sch_lvl=%d\n",
		 info.inst, info.dp_port, info.node_id.q_id,
		 info.cqm_deq_port.cqm_deq_port, cfg->n_sch_lvl);
	if (cfg->n_sch_lvl) {
		info.node_id.sch_id = cfg->sch[cfg->n_sch_lvl - 1].id;
		info.node_type = DP_NODE_SCH;
		info.p_node_id.cqm_deq_port = cfg->cqm_deq_port;
		info.p_node_type = DP_NODE_PORT;
		info.arbi = cfg->sch[cfg->n_sch_lvl - 1].arbi;
		info.leaf = cfg->sch[cfg->n_sch_lvl - 1].leaf;
		info.prio_wfq = cfg->sch[cfg->n_sch_lvl - 1].prio_wfq;
		f_sch_free[cfg->n_sch_lvl - 1].flag = 1;

		if (dp_node_link_add_32(&info, flag)) {
			pr_err("Failed to link Sch:%d to Port:%d\n",
			       cfg->sch[cfg->n_sch_lvl - 1].id,
			       cfg->cqm_deq_port);
			goto EXIT;
		}
		/* link sched to port */
		if (cfg->sch[cfg->n_sch_lvl - 1].id == DP_NODE_AUTO_ID)
			f_sch_free[cfg->n_sch_lvl - 1].f_auto = 1;

		f_sch_free[cfg->n_sch_lvl - 1].sch_id = info.node_id.sch_id;

		/* link sched to sched */
		for (i = (cfg->n_sch_lvl - 2); i >= 0; i--) {
			info.node_id.sch_id = cfg->sch[i].id;
			info.node_type = DP_NODE_SCH;
			info.p_node_id.sch_id = f_sch_free[i + 1].sch_id;
			info.p_node_type = DP_NODE_SCH;
			info.arbi = cfg->sch[i].arbi;
			info.leaf = cfg->sch[i].leaf;
			info.prio_wfq = cfg->sch[i].prio_wfq;
			f_sch_free[i].flag = 1;

			if (dp_node_link_add_32(&info, flag)) {
				pr_err("Failed to link Sch:%d to Sch:%d\n",
				       cfg->sch[i].id, cfg->sch[i + 1].id);
				goto EXIT;
			}
			if (cfg->sch[i].id == DP_NODE_AUTO_ID)
				f_sch_free[i].f_auto = 1;

			f_sch_free[i].sch_id = info.node_id.sch_id;
		}
		/* link Queue to sched */
		info.node_type = DP_NODE_QUEUE;
		info.node_id.q_id = cfg->q_id;
		info.p_node_id.sch_id = f_sch_free[0].sch_id;
		info.p_node_type = DP_NODE_SCH;
		info.arbi = cfg->sch[0].arbi;
		info.leaf = cfg->sch[0].leaf;
		info.prio_wfq = cfg->sch[0].prio_wfq;

		if (dp_node_link_add_32(&info, flag)) {
			pr_err("Failed to link Q:%d to Sch:%d\n",
			       cfg->q_id, cfg->sch[0].id);
			f_q_free = 1;
			goto EXIT;
		}
	} else {
		/* link Queue to Port */
		info.node_type = DP_NODE_QUEUE;
		info.p_node_id.cqm_deq_port = cfg->cqm_deq_port;
		info.p_node_type = DP_NODE_PORT;
		info.arbi = cfg->q_arbi;
		info.leaf = cfg->q_leaf;
		info.prio_wfq = cfg->q_prio_wfq;

		if (dp_node_link_add_32(&info, flag)) {
			pr_err("Failed to link Q:%d to Port:%d\n",
			       cfg->q_id, cfg->cqm_deq_port);
			f_q_free = 1;
			goto EXIT;
		}
	}
	return DP_SUCCESS;
EXIT:
	for (i = (cfg->n_sch_lvl - 1); i >= 0; i--) {
		if (!f_sch_free[i].flag)
			continue;
		/* sch is auto_alloc move it to FREE */
		if (f_sch_free[i].f_auto) {
			struct dp_node_alloc node = {0};

			node.id.sch_id = f_sch_free[i].sch_id;
			node.type = DP_NODE_SCH;
			node.dp_port = cfg->dp_port;
			node.inst = cfg->inst;
			dp_node_free_32(&node,
					flag);
		}
		/* sch provided by caller move it to ALLOC */
		if (node_stat_update(info.inst, f_sch_free[i].sch_id,
				     DP_NODE_DEC)) {
			pr_err("Failed to %s sched:%d DP_NODE_DEC\n",
			       "node_stat_update",
			       f_sch_free[i].sch_id);
			continue;
		}
	}
	if (f_q_free) {
		/* queue is auto_alloc move it to FREE */
		if (f_q_auto) {
			struct dp_node_alloc node = {0};

			node.id.q_id = cfg->q_id;
			node.type = DP_NODE_QUEUE;
			node.dp_port = cfg->dp_port;
			node.inst = cfg->inst;
			dp_node_free_32(&node, flag);
		}
		/* queue provided by caller move it to ALLOC */
		if (node_stat_update(info.inst, cfg->q_id, DP_NODE_DEC)) {
			pr_err("Failed to update stat qid %d DP_NODE_DEC\n",
			       cfg->q_id);
			return DP_FAILURE;
		}
	}
	return DP_FAILURE;
}

/* dp_shaper_conf_set_32 API
 * DP_NO_SHAPER_LIMIT no limit for shaper
 * DP_MAX_SHAPER_LIMIT max limit for shaper
 * configure shaper limit for node and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_shaper_conf_set_32(struct dp_shaper_conf *cfg, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct pp_qos_port_conf port_cfg = {0};
	struct hal_priv *priv;
	int node_id, res;
	u32 bw_limit;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (cfg->type == DP_NODE_QUEUE) {
		if ((cfg->id.q_id < 0) || (cfg->id.q_id >= MAX_QUEUE)) {
			pr_err("Invalid Queue ID:%d\n", cfg->id.q_id);
			return DP_FAILURE;
		}
		if (priv->qos_queue_stat[cfg->id.q_id].flag == PP_NODE_FREE) {
			pr_err("Invalid Queue flag:%d\n",
			       priv->qos_queue_stat[cfg->id.q_id].flag);
			return DP_FAILURE;
		}
		node_id = priv->qos_queue_stat[cfg->id.q_id].node_id;

		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("qos_queue_conf_get_32 fail:%d\n", cfg->id.q_id);
			return DP_FAILURE;
		}

		if ((cfg->cmd == DP_SHAPER_CMD_ADD) ||
		    (cfg->cmd == DP_SHAPER_CMD_ENABLE)) {
			res = limit_dp2pp(cfg->cir, &bw_limit);

			if (res == DP_FAILURE) {
				pr_err("Wrong dp shaper limit:%u\n", cfg->cir);
				return DP_FAILURE;
			}
			queue_cfg.common_prop.bandwidth_limit = bw_limit;
		} else if ((cfg->cmd == DP_SHAPER_CMD_REMOVE) ||
			   (cfg->cmd == DP_SHAPER_CMD_DISABLE)) {
			queue_cfg.common_prop.bandwidth_limit = 0;
		} else {
			pr_err("Incorrect command provided:%d\n", cfg->cmd);
			return DP_FAILURE;
		}

		if (qos_queue_set_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("qos_queue_set_32 fail:%d\n", cfg->id.q_id);
			return DP_FAILURE;
		}
		return DP_SUCCESS;
	} else if (cfg->type == DP_NODE_SCH) {
		if ((cfg->id.sch_id < 0) || (cfg->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Invalid Sched ID:%d\n", cfg->id.sch_id);
			return DP_FAILURE;
		}
		if (priv->qos_sch_stat[cfg->id.sch_id].p_flag ==
		    PP_NODE_FREE) {
			pr_err("Invalid Sched flag:%d\n",
			       priv->qos_sch_stat[cfg->id.sch_id].p_flag);
			return DP_FAILURE;
		}

		if (qos_sched_conf_get_32(priv->qdev, cfg->id.sch_id,
					  &sched_cfg)) {
			pr_err("qos_sched_conf_get_32 fail:%d\n",
			       cfg->id.sch_id);
			return DP_FAILURE;
		}

		if ((cfg->cmd == DP_SHAPER_CMD_ADD) ||
		    (cfg->cmd == DP_SHAPER_CMD_ENABLE)) {
			res = limit_dp2pp(cfg->cir, &bw_limit);

			if (res == DP_FAILURE) {
				pr_err("Wrong dp shaper limit:%u\n", cfg->cir);
				return DP_FAILURE;
			}
			sched_cfg.common_prop.bandwidth_limit = bw_limit;
		} else if ((cfg->cmd == DP_SHAPER_CMD_REMOVE) ||
			   (cfg->cmd == DP_SHAPER_CMD_DISABLE)) {
			sched_cfg.common_prop.bandwidth_limit = 0;
		} else {
			pr_err("Incorrect command provided:%d\n", cfg->cmd);
			return DP_FAILURE;
		}

		if (qos_sched_set_32(priv->qdev, cfg->id.sch_id, &sched_cfg)) {
			pr_err("qos_sched_set_32 fail:%d\n", cfg->id.sch_id);
			return DP_FAILURE;
		}
		return DP_SUCCESS;
	} else if (cfg->type == DP_NODE_PORT) {
		if ((cfg->id.cqm_deq_port < 0) ||
		    (cfg->id.cqm_deq_port >= MAX_CQM_DEQ)) {
			pr_err("Invalid Port ID:%d\n", cfg->id.cqm_deq_port);
			return DP_FAILURE;
		}
		if (priv->deq_port_stat[cfg->id.cqm_deq_port].flag ==
		    PP_NODE_FREE) {
			pr_err("Invalid Port flag:%d\n",
			       priv->deq_port_stat[cfg->id.cqm_deq_port].flag);
			return DP_FAILURE;
		}
		node_id = priv->deq_port_stat[cfg->id.cqm_deq_port].node_id;

		if (qos_port_conf_get_32(priv->qdev, node_id, &port_cfg)) {
			pr_err("qos_port_conf_get_32 fail:%d\n",
			       cfg->id.cqm_deq_port);
			return DP_FAILURE;
		}

		if ((cfg->cmd == DP_SHAPER_CMD_ADD) ||
		    (cfg->cmd == DP_SHAPER_CMD_ENABLE)) {
			res = limit_dp2pp(cfg->cir, &bw_limit);

			if (res == DP_FAILURE) {
				pr_err("Wrong dp shaper limit:%u\n", cfg->cir);
				return DP_FAILURE;
			}
			port_cfg.common_prop.bandwidth_limit = bw_limit;
		} else if ((cfg->cmd == DP_SHAPER_CMD_REMOVE) ||
			   (cfg->cmd == DP_SHAPER_CMD_DISABLE)) {
			port_cfg.common_prop.bandwidth_limit = 0;
		} else {
			pr_err("Incorrect command provided:%d\n", cfg->cmd);
			return DP_FAILURE;
		}

		if (qos_port_set_32(priv->qdev, node_id, &port_cfg)) {
			pr_err("qos_port_set_32 fail:%d\n",
			       cfg->id.cqm_deq_port);
			return DP_FAILURE;
		}
		return DP_SUCCESS;
	}
	pr_err("Unkonwn type provided:0x%x\n", cfg->type);
	return DP_FAILURE;
}

/* dp_shaper_conf_get_32 API
 * DP_NO_SHAPER_LIMIT no limit for shaper
 * DP_MAX_SHAPER_LIMIT max limit for shaper
 * get shaper limit for node fill struct and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_shaper_conf_get_32(struct dp_shaper_conf *cfg, int flag)
{
	struct pp_qos_queue_conf queue_cfg = {0};
	struct pp_qos_sched_conf sched_cfg = {0};
	struct pp_qos_port_conf port_cfg = {0};
	struct hal_priv *priv;
	int node_id, res;
	u32 bw_limit;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (cfg->type == DP_NODE_QUEUE) {
		if ((cfg->id.q_id < 0) || (cfg->id.q_id >= MAX_QUEUE)) {
			pr_err("Invalid Queue ID:%d\n", cfg->id.q_id);
			return DP_FAILURE;
		}
		if (priv->qos_queue_stat[cfg->id.q_id].flag == PP_NODE_FREE) {
			pr_err("Invalid Queue flag:%d\n",
			       priv->qos_queue_stat[cfg->id.q_id].flag);
			return DP_FAILURE;
		}
		node_id = priv->qos_queue_stat[cfg->id.q_id].node_id;

		if (qos_queue_conf_get_32(priv->qdev, node_id, &queue_cfg)) {
			pr_err("qos_queue_conf_get_32 fail:%d\n", cfg->id.q_id);
			return DP_FAILURE;
		}
		res = limit_pp2dp(queue_cfg.common_prop.bandwidth_limit,
				  &bw_limit);

		if (res == DP_FAILURE) {
			pr_err("Wrong pp shaper limit:%u\n",
			       queue_cfg.common_prop.bandwidth_limit);
			return DP_FAILURE;
		}
	} else if (cfg->type == DP_NODE_SCH) {
		if ((cfg->id.sch_id < 0) || (cfg->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Invalid Sched ID:%d\n", cfg->id.sch_id);
			return DP_FAILURE;
		}
		if (priv->qos_sch_stat[cfg->id.sch_id].p_flag ==
		    PP_NODE_FREE) {
			pr_err("Invalid Sched flag:%d\n",
			       priv->qos_sch_stat[cfg->id.sch_id].p_flag);
			return DP_FAILURE;
		}

		if (qos_sched_conf_get_32(priv->qdev, cfg->id.sch_id,
					  &sched_cfg)) {
			pr_err("qos_sched_conf_get_32 fail:%d\n",
			       cfg->id.sch_id);
			return DP_FAILURE;
		}

		res = limit_pp2dp(sched_cfg.common_prop.bandwidth_limit,
				  &bw_limit);

		if (res == DP_FAILURE) {
			pr_err("Wrong pp shaper limit:%u\n",
			       sched_cfg.common_prop.bandwidth_limit);
			return DP_FAILURE;
		}
	} else if (cfg->type == DP_NODE_PORT) {
		if ((cfg->id.cqm_deq_port < 0) ||
		    (cfg->id.cqm_deq_port >= MAX_CQM_DEQ)) {
			pr_err("Invalid Port ID:%d\n", cfg->id.cqm_deq_port);
			return DP_FAILURE;
		}
		if (priv->deq_port_stat[cfg->id.cqm_deq_port].flag ==
		    PP_NODE_FREE) {
			pr_err("Invalid Port flag:%d\n",
			       priv->deq_port_stat[cfg->id.cqm_deq_port].flag);
			return DP_FAILURE;
		}
		node_id = priv->deq_port_stat[cfg->id.cqm_deq_port].node_id;
		if (qos_port_conf_get_32(priv->qdev, node_id, &port_cfg)) {
			pr_err("qos_port_conf_get_32 fail:%d\n",
			       cfg->id.cqm_deq_port);
			return DP_FAILURE;
		}
		res = limit_pp2dp(port_cfg.common_prop.bandwidth_limit,
				  &bw_limit);

		if (res == DP_FAILURE) {
			pr_err("Wrong pp shaper limit:%u\n",
			       port_cfg.common_prop.bandwidth_limit);
			return DP_FAILURE;
		}
	} else {
		pr_err("Unkonwn type provided:0x%x\n", cfg->type);
		return DP_FAILURE;
	}

	cfg->cir = bw_limit;
	cfg->pir = 0;
	cfg->cbs = 0;
	cfg->pbs = 0;
	return DP_SUCCESS;
}

int dp_queue_map_get_32(struct dp_queue_map_get *cfg, int flag)
{
	struct hal_priv *priv;
	cbm_queue_map_entry_t *qmap_entry = NULL;
	s32 num_entry;
	int i;
	int res = DP_SUCCESS;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	if ((cfg->q_id < 0) || (cfg->q_id >= MAX_QUEUE)) {
		pr_err("Invalid Queue ID:%d\n", cfg->q_id);
		return DP_FAILURE;
	}
	if (priv->qos_queue_stat[cfg->q_id].flag == PP_NODE_FREE) {
		pr_err("Invalid Queue flag:%d\n",
		       priv->qos_queue_stat[cfg->q_id].flag);
		return DP_FAILURE;
	}

	if (cbm_queue_map_get(cfg->inst, cfg->q_id, cfg->egflag, &num_entry,
			      &qmap_entry, 0)) {
		pr_err("cbm_queue_map_get fail: qid=%d egflag=%d\n",
		       cfg->q_id, cfg->egflag);
		return DP_FAILURE;
	}

	cfg->num_entry = num_entry;

	if (!qmap_entry) {
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "queue map entry returned null value\n");
		if (num_entry) {
			pr_err("num_entry is not null:%d\n", num_entry);
			res = DP_FAILURE;
		}
		goto EXIT;
	}

	if (!cfg->qmap_entry)
		goto EXIT;

	if (num_entry > cfg->qmap_size) {
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "num_entry is greater than qmap_size:%d\n",
			 num_entry);
		goto EXIT;
	}

	for (i = 0; i < num_entry; i++) {
		cfg->qmap_entry[i].qmap.flowid = qmap_entry[i].flowid;
		cfg->qmap_entry[i].qmap.dec = qmap_entry[i].dec;
		cfg->qmap_entry[i].qmap.enc = qmap_entry[i].enc;
		cfg->qmap_entry[i].qmap.mpe1 = qmap_entry[i].mpe1;
		cfg->qmap_entry[i].qmap.mpe2 = qmap_entry[i].mpe2;
		cfg->qmap_entry[i].qmap.dp_port = qmap_entry[i].ep;
		cfg->qmap_entry[i].qmap.class = qmap_entry[i].tc;
		cfg->qmap_entry[i].qmap.subif = qmap_entry[i].sub_if_id;
	}

EXIT:
	if (!qmap_entry)
		kfree(qmap_entry);
	qmap_entry = NULL;
	return res;
}

int dp_queue_map_set_32(struct dp_queue_map_set *cfg, int flag)
{
	struct hal_priv *priv;
	cbm_queue_map_entry_t qmap_cfg = {0};
	u32 cqm_flags = 0;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	if ((cfg->q_id < 0) || (cfg->q_id >= MAX_QUEUE)) {
		pr_err("Invalid Queue ID:%d\n", cfg->q_id);
		return DP_FAILURE;
	}
	if (priv->qos_queue_stat[cfg->q_id].flag == PP_NODE_FREE) {
		pr_err("Invalid Queue flag:%d\n",
		       priv->qos_queue_stat[cfg->q_id].flag);
		return DP_FAILURE;
	}

	qmap_cfg.mpe1 = cfg->map.mpe1;
	qmap_cfg.mpe2 = cfg->map.mpe2;
	qmap_cfg.ep = cfg->map.dp_port;
	qmap_cfg.flowid = cfg->map.flowid;
	qmap_cfg.dec = cfg->map.dec;
	qmap_cfg.enc = cfg->map.enc;
	qmap_cfg.tc = cfg->map.class;
	qmap_cfg.sub_if_id = cfg->map.subif;
	if (cfg->mask.mpe1)
		cqm_flags |= CBM_QUEUE_MAP_F_MPE1_DONTCARE;
	if (cfg->mask.mpe2)
		cqm_flags |= CBM_QUEUE_MAP_F_MPE2_DONTCARE;
	if (cfg->mask.dp_port)
		cqm_flags |= CBM_QUEUE_MAP_F_EP_DONTCARE;
	if (cfg->mask.flowid) {
		cqm_flags |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE;
		cqm_flags |= CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE;
	}
	if (cfg->mask.dec)
		cqm_flags |= CBM_QUEUE_MAP_F_DE_DONTCARE;
	if (cfg->mask.enc)
		cqm_flags |= CBM_QUEUE_MAP_F_EN_DONTCARE;
	if (cfg->mask.class)
		cqm_flags |= CBM_QUEUE_MAP_F_TC_DONTCARE;
	if (cfg->mask.dp_port)
		cqm_flags |= CBM_QUEUE_MAP_F_EP_DONTCARE;
	if (cfg->mask.subif) {
		cqm_flags |= CBM_QUEUE_MAP_F_SUBIF_DONTCARE;
		if (get_dp_port_info(cfg->inst, cfg->map.dp_port)->gsw_mode ==
				GSW_LOGICAL_PORT_9BIT_WLAN)
			cqm_flags |= CBM_QUEUE_MAP_F_SUBIF_LSB_DONTCARE;
	}

	if (cbm_queue_map_set(cfg->inst, cfg->q_id, &qmap_cfg, cqm_flags)) {
		pr_err("cbm_queue_map_set fail for Q:%d\n", cfg->q_id);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}

int dp_counter_mode_set_32(struct dp_counter_conf *cfg, int flag)
{
	return DP_FAILURE;
}

int dp_counter_mode_get_32(struct dp_counter_conf *cfg, int flag)
{
	return DP_FAILURE;
}

int get_sch_level_32(int inst, int pid, int flag)
{
	struct hal_priv *priv;
	int level;

	priv = HAL(inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	for (level = 0; level < DP_MAX_SCH_LVL; level++) {
		if (priv->qos_sch_stat[pid].parent.type == DP_NODE_PORT) {
			level = level + 1;
			break;
		}
		pid = priv->qos_sch_stat[pid].parent.node_id;
	}
	return level;
}

/* dp_qos_level_get_32 API
 * get max scheduler level and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_qos_level_get_32(struct dp_qos_level *dp, int flag)
{
	struct hal_priv *priv;
	u16 i, id, pid, lvl_x = 0;

	if (!dp) {
		pr_err("dp cannot be NULL\n");
		return DP_FAILURE;
	}
	dp->max_sch_lvl = 0;
	priv = HAL(dp->inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	for (i = 0; i < MAX_QUEUE; i++) {
		if (priv->qos_queue_stat[i].flag & PP_NODE_FREE)
			continue;
		id = priv->qos_queue_stat[i].node_id;

		if (priv->qos_sch_stat[id].parent.type == DP_NODE_PORT) {
			continue;
		} else if (priv->qos_sch_stat[id].parent.type == DP_NODE_SCH) {
			pid = priv->qos_sch_stat[id].parent.node_id;
			lvl_x = get_sch_level_32(dp->inst, pid, 0);
		}
		if (lvl_x > dp->max_sch_lvl)
			dp->max_sch_lvl = lvl_x;
	}
	if (dp->max_sch_lvl >= 0)
		return DP_SUCCESS;
	else
		return DP_FAILURE;
}

#ifdef PP_QOS_LINK_EXAMPLE
/* Example: queue -> QOS/CQM dequeue port
 * inst: dp instance
 * dp_port: dp port id
 * t_conf: for pon, it is used to get cqm dequeue port via t-cont index
 *         for others, its value should be 0
 * q_node: queueu node id
 */

int ppv4_queue_port_example(int inst, int dp_port, int t_cont, int q_node)
{
	struct pp_qos_port_conf  port_cfg;
	struct pp_qos_queue_conf queue_cfg;
	int qos_port_node;
	int rc;
	int cqm_deq_port;
	struct pmac_port_info *port_info;

	port_info = get_dp_port_info(inst, dp_port);
	cqm_deq_port = port_info->deq_port_base + t_cont;

	/* Allocate qos dequeue port's node id via cqm_deq_port */
	rc = qos_port_allocate_32(priv->qdev, cqm_deq_port, &qos_port_node);
	if (rc) {
		pr_err("failed to qos_port_allocate_32\n");
		return DP_FAILURE;
	}

	/* Configure QOS dequeue port */
	qos_port_conf_set_default_32(&port_cfg);
	port_cfg.ring_address = (void *)(port_info->tx_ring_addr_txpush +
		port_info->tx_ring_offset * t_cont);
	port_cfg.ring_size = port_info->tx_ring_size;
	if (port_info->tx_pkt_credit) {
		port_cfg.packet_credit_enable = 1;
		port_cfg.credit = port_info->tx_pkt_credit;
	}
#ifdef TX_BYTE_CREDIT
	if (port_info->tx_b_credit) {
		port_cfg.byte_credit_enable = 1;
		port_cfg.byte_credit = port_info->tx_pkt_credit;
	}
#endif
#ifdef EXT_BW
	port_cfg.parent.arbitration = ARBITRATION_WRR;
	port_cfg.common.bandwidth_limit = 1000;
#endif
	rc = qos_port_set_32(priv->qdev, qos_port_node, &port_cfg);
	if (rc) {
		pr_err("failed to qos_port_set_32\n");
		qos_port_remove_32(priv->qdev, qos_port_node);
		return DP_FAILURE;
	}

	/* Attach queue to QoS port */
	qos_queue_conf_set_default_32(&queue_cfg);
	queue_cfg.queue_child_prop.parent = qos_port_node;
#ifdef EXT_BW
	queue_cfg.max_burst  = 64;
	queue_cfg.child.wrr_weight = 50;
	queue_cfg.wred_min_guaranteed = 1;
	queue_cfg.wred_max_allowed = 10;
#endif
	rc = qos_queue_set_32(priv->qdev, q_node, &queue_cfg);
	if (rc) {
		pr_err("failed to qos_queue_set_32\n");
		qos_port_remove_32(priv->qdev, qos_port_node);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}

/* Example: queue -> scheduler1- > scheduler 2 -> QOS/CQM dequeue port
 * inst: dp instance
 * dp_port: dp port id
 * t_conf: for pon, it is used to get cqm dequeue port via t-cont index
 *         for others, its value should be 0
 * q_node: queueu node id
 * sch_node1: schduler node which connected with queue
 * sch_node2: schduler node which connected with sch_node1
 */
int ppv4_queue_scheduler(int inst, int dp_port, int t_cont, int q_node,
			 int sch_node1, int sch_node2)
{
	struct pp_qos_port_conf  port_cfg;
	struct pp_qos_queue_conf queue_cfg;
	struct pp_qos_sched_conf sched_cfg;
	int qos_port_node;
	int rc;
	int cqm_deq_port;
	struct pmac_port_info *port_info;

	port_info = get_dp_port_info(inst, dp_port);
	cqm_deq_port = port_info->deq_port_base + t_cont;

	/* Allocate qos dequeue port's node id via cqm_deq_port */
	rc = qos_port_allocate_32(priv->qdev, cqm_deq_port, &qos_port_node);
	if (rc) {
		pr_err("failed to qos_port_allocate_32\n");
		return DP_FAILURE;
	}

	/* Configure QOS dequeue port */
	qos_port_conf_set_default_32(&port_cfg);
	port_cfg.ring_address = (void *)(port_info->tx_ring_addr_txpush +
		port_info->tx_ring_offset * t_cont);
	port_cfg.ring_size = port_info->tx_ring_size;
	if (port_info->tx_pkt_credit) {
		port_cfg.packet_credit_enable = 1;
		port_cfg.credit = port_info->tx_pkt_credit;
	}
	if (port_info->tx_pkt_credit) {
		port_cfg.byte_credit_enable = 1;
		port_cfg.byte_credit = port_info->tx_pkt_credit;
	}
#ifdef EXT_BW
	port_cfg.parent.arbitration = ARBITRATION_WRR;
	port_cfg.common.bandwidth_limit = 1000;
#endif
	rc = qos_port_set_32(priv->qdev, qos_port_node, &port_cfg);
	if (rc) {
		pr_err("failed to qos_port_set_32\n");
		qos_port_remove_32(priv->qdev, qos_port_node);
		return DP_FAILURE;
	}

	/* Attach queue to sch_node1 */
	qos_queue_conf_set_default_32(&queue_cfg);
	queue_cfg.queue_child_prop.parent = sch_node1;
#ifdef EXT_BW
	queue_cfg.max_burst  = 64;
	queue_cfg.child.wrr_weight = 50;
	queue_cfg.wred_min_guaranteed = 1;
	queue_cfg.wred_max_allowed = 10;
#endif
	rc = qos_queue_set_32(priv->qdev, q_node, &queue_cfg);
	if (rc) {
		pr_err("failed to qos_queue_set_32\n");
		qos_port_remove_32(priv->qdev, qos_port_node);
		return DP_FAILURE;
	}

	/* Attach sch_node1 to sch_node2 */
	qos_sched_conf_set_default_32(&sched_cfg);
	sched_cfg.sched_child_prop.parent = sch_node2;
	rc = qos_sched_set_32(priv->qdev, sch_node1, &sched_cfg);
	if (rc) {
		pr_err("failed to qos_sched_set_32\n");
		qos_port_remove_32(priv->qdev, qos_port_node);
		return DP_FAILURE;
	}

	/* Attach sch_node2 to qos/cqm dequeue port */
	qos_sched_conf_set_default_32(&sched_cfg);
	sched_cfg.sched_child_prop.parent = qos_port_node;
	rc = qos_sched_set_32(priv->qdev, sch_node2, &sched_cfg);
	if (rc) {
		pr_err("failed to qos_sched_set_32\n");
		qos_port_remove_32(priv->qdev, qos_port_node);
		return DP_FAILURE;
	}

	return DP_SUCCESS;
}

#endif /* PP_QOS_LINK_EXAMPLE */

static int get_children_list(int inst, struct dp_node *child, int node_id)
{
	int idx, num = 0, child_id;
	struct hal_priv *priv = HAL(inst);

	for (idx = 0; idx < DP_MAX_CHILD_PER_NODE; idx++) {
		child_id =
			get_qid_by_node(inst, CHILD(node_id, idx).node_id, 0);
		if (priv->qos_sch_stat[node_id].child[idx].flag &
		    PP_NODE_ACTIVE) {
			child[idx].type = CHILD(node_id, idx).type;
			if (child[idx].type == DP_NODE_SCH)
				child[idx].id.q_id =
						CHILD(node_id, idx).node_id;
			else
				child[idx].id.q_id = child_id;
			num++;
		}
	}
	return num;
}

/* dp_children_get_32 API
 * Get direct chldren and type of given node and return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_children_get_32(struct dp_node_child *cfg, int flag)
{
	int node_id, res = 0;
	struct hal_priv *priv;

	if (!cfg) {
		pr_err("cfg cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(cfg->inst);
	cfg->num = 0;

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	if (cfg->type == DP_NODE_SCH) {
		if ((cfg->id.sch_id < 0) || (cfg->id.sch_id >= QOS_MAX_NODES)) {
			pr_err("Invalid Sched ID:%d\n", cfg->id.sch_id);
			return DP_FAILURE;
		}
		if (priv->qos_sch_stat[cfg->id.sch_id].c_flag == PP_NODE_FREE) {
			pr_err("Invalid Sched flag:0x%x\n",
			       priv->qos_sch_stat[cfg->id.sch_id].c_flag);
			return DP_FAILURE;
		}
		node_id = cfg->id.sch_id;

	} else if (cfg->type == DP_NODE_PORT) {
		if ((cfg->id.cqm_deq_port < 0) ||
		    (cfg->id.cqm_deq_port >= MAX_CQM_DEQ)) {
			pr_err("Invalid Port ID:%d\n", cfg->id.cqm_deq_port);
			return DP_FAILURE;
		}
		if (priv->deq_port_stat[cfg->id.cqm_deq_port].flag ==
		    PP_NODE_FREE) {
			pr_err("Invalid Port flag:0x%x\n",
			       priv->deq_port_stat[cfg->id.cqm_deq_port].flag);
			return DP_FAILURE;
		}
		node_id = priv->deq_port_stat[cfg->id.cqm_deq_port].node_id;

	} else {
		pr_err("Unkonwn type provided:0x%x\n", cfg->type);
		return DP_FAILURE;
	}
	if (!priv->qos_sch_stat[node_id].child_num)
		return DP_SUCCESS;

	cfg->num = priv->qos_sch_stat[node_id].child_num;
	res = get_children_list(cfg->inst, cfg->child, node_id);

	if (cfg->num == res)
		return DP_SUCCESS;

	pr_err("child_num:[%d] not matched to res:[%d] for Node:%d\n",
	       cfg->num, res, cfg->id.sch_id);
	return DP_FAILURE;
}

/* #define DP_SUPPORT_RES_RESERVE */
int dp_node_reserve_32(int inst, int ep, struct dp_port_data *data, int flags)
{
	int i;
	unsigned int id;
	struct pp_qos_queue_info info;
	int len;
	struct resv_q *resv_q;
#ifdef WORKAROUND_DROP_PORT
	struct pp_qos_queue_conf q_conf;
#endif
	struct hal_priv *priv = HAL(inst);
	int res = DP_SUCCESS;

#ifndef DP_SUPPORT_RES_RESERVE /* immediately return */
	return res;
#endif

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}

	/* free resved queue/scheduler */
	if (flags == DP_F_DEREGISTER)
		goto FREE_EXIT;

	/* Need reserve for queue/scheduler */
/* #define DP_SUPPORT_RES_TEST */
#ifdef DP_SUPPORT_RES_TEST
	data->num_resv_q = 4;
	data->num_resv_sched = 4;
#endif
	/* to reserve the queue */
	if (data->num_resv_q <= 0)
		goto RESERVE_SCHED;
	len = sizeof(struct resv_q) * data->num_resv_q;
	priv->resv[ep].resv_q = kzalloc(len, GFP_ATOMIC);
	if (!priv->resv[ep].resv_q) {
		res = DP_FAILURE;
		goto FREE_EXIT;
	}
	DP_DEBUG(DP_DBG_FLAG_QOS, "queue size =%d for ep=%d\n", len, ep);
	resv_q = priv->resv[ep].resv_q;
	for (i = 0; i < data->num_resv_q; i++) {
		if (qos_queue_allocate_32(priv->qdev, &id)) {
			res = DP_FAILURE;
			pr_err("qos_queue_allocate_32 failed\n");
			goto FREE_EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "qos_queue_allocate_32: %d\n", id);
#ifdef WORKAROUND_DROP_PORT /* use drop port */
		qos_queue_conf_set_default_32(&q_conf);
		q_conf.wred_enable = 0;
		q_conf.wred_max_allowed = DEF_QRED_MAX_ALLOW;
		q_conf.queue_child_prop.parent = priv->ppv4_drop_p;
		if (qos_queue_set_32(priv->qdev, id, &q_conf)) {
			res = DP_FAILURE;
			pr_err(
			    "qos_queue_set_32 fail for queue=%d to parent=%d\n",
			    id, q_conf.queue_child_prop.parent);
			goto FREE_EXIT;
		}
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "Workaround queue(/%d)-> tmp parent(/%d)\n",
			 id, q_conf.queue_child_prop.parent);
#endif
		if (qos_queue_info_get_32(priv->qdev, id, &info)) {
			qos_queue_remove_32(priv->qdev, id);
			res = DP_FAILURE;
			pr_err("qos_queue_info_get_32: %d\n", id);
			goto FREE_EXIT;
		}
		resv_q[i].id = id;
		resv_q[i].physical_id = info.physical_id;
		priv->resv[ep].num_resv_q = i + 1;
		DP_DEBUG(DP_DBG_FLAG_QOS,
			 "reseve q[%d/%d]\n",
			 resv_q[i].id, resv_q[i].physical_id);
	}
RESERVE_SCHED:
	/* reserve scheduler */
	if (data->num_resv_sched > 0) {
		int len;
		struct resv_sch *p_sch = priv->resv[ep].resv_sched;

		len = sizeof(struct resv_sch) * data->num_resv_sched;
		priv->resv[ep].resv_sched = kzalloc(len, GFP_ATOMIC);
		DP_DEBUG(DP_DBG_FLAG_QOS, "sched size =%d for ep=%d\n",
			 len, ep);
		if (!priv->resv[ep].resv_sched) {
			res = DP_FAILURE;
			goto FREE_EXIT;
		}
		p_sch = priv->resv[ep].resv_sched;
		for (i = 0; i < data->num_resv_sched; i++) {
			if (qos_sched_allocate_32(priv->qdev, &id)) {
				res = DP_FAILURE;
				pr_err("qos_queue_allocate_32 failed\n");
				goto FREE_EXIT;
			}
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "qos_sched_allocate_32: %d\n", id);
			p_sch[i].id = id;
			priv->resv[ep].num_resv_sched = i + 1;
			DP_DEBUG(DP_DBG_FLAG_QOS,
				 "reseve sched[/%d]\n", resv_q[i].id);
		}
	}
	return res;

FREE_EXIT:
	if (priv->resv[ep].resv_q) {
		struct resv_q  *resv_q = priv->resv[ep].resv_q;

		for (i = 0; i < priv->resv[ep].num_resv_q; i++)
			qos_queue_remove_32(priv->qdev, resv_q[i].id);
		kfree(resv_q);
		priv->resv[ep].resv_q = NULL;
		priv->resv[ep].num_resv_q = 0;
	}
	if (priv->resv[ep].resv_sched) {
		struct resv_sch *resv_sch = priv->resv[ep].resv_sched;

		for (i = 0; i < priv->resv[ep].num_resv_sched; i++)
			qos_sched_remove_32(priv->qdev, resv_sch[i].id);
		kfree(resv_sch);
		priv->resv[ep].resv_sched = NULL;
		priv->resv[ep].num_resv_sched = 0;
	}
	return res;
}

int dp_get_q_logic_32(int inst, int qid)
{
	struct hal_priv *priv = HAL(inst);

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return PP_QOS_INVALID_ID;
	}
	return pp_qos_queue_id_get(priv->qdev, qid);
}

int dp_get_queue_logic_32(struct dp_qos_q_logic *cfg, int flag)
{
	struct hal_priv *priv = HAL(cfg->inst);
	int res = DP_SUCCESS;

	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	cfg->q_logic_id = dp_get_q_logic_32(cfg->inst, cfg->q_id);
	if (cfg->q_logic_id == PP_QOS_INVALID_ID)
		res = DP_FAILURE;
	return res;
}

/* dp_qos_global_info_get_32 API
 * Get global qos config information return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_qos_global_info_get_32(struct dp_qos_cfg_info *info, int flag)
{
	struct hal_priv *priv;
	unsigned int quanta = 0;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	/* TODO Need to add later once PP QOS has this prototype */
	#if 0
	if (pp_qos_get_quanta(priv->qdev, &quanta)) {
		pr_err("failed pp_qos_get_quanta\n");
		return DP_FAILURE;
	}
	#endif
	info->quanta = quanta;
	DP_DEBUG(DP_DBG_FLAG_QOS, "quanta=%d\n", quanta);

	return DP_SUCCESS;
}

/* dp_qos_port_conf_set_32 API
 * Get global qos config information return DP_SUCCESS
 * else return DP_FAILURE
 */
int dp_qos_port_conf_set_32(struct dp_port_cfg_info *info, int flag)
{
	struct hal_priv *priv;
	struct pp_qos_port_conf port_cfg = {0};
	int node_id;

	if (!info) {
		pr_err("info cannot be NULL\n");
		return DP_FAILURE;
	}
	priv = HAL(info->inst);
	if (!priv) {
		pr_err("priv cannot be NULL\n");
		return DP_FAILURE;
	}
	node_id = priv->deq_port_stat[info->pid].node_id;
	DP_DEBUG(DP_DBG_FLAG_QOS, "%s cqm_deq:%d, qos_port:%d\n",
		 __func__, info->pid, node_id);
	if (qos_port_conf_get(priv->qdev, node_id, &port_cfg)) {
		pr_err("failed qos_port_conf_get\n");
		return DP_FAILURE;
	}
	port_cfg.green_threshold = info->green_threshold;
	port_cfg.yellow_threshold = info->yellow_threshold;
	if (qos_port_set(priv->qdev, node_id, &port_cfg)) {
		pr_err("fail to set yellow:%d green:%d for node:%d\n",
		       info->yellow_threshold, info->green_threshold, node_id);
		return DP_FAILURE;
	}
	return DP_SUCCESS;
}
