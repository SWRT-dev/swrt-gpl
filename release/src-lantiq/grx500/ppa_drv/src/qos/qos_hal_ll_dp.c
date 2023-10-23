/*****************************************************************************
 **
 ** FILE NAME    : qos_hal_ll_dp.c
 ** PROJECT      : QOS DP HAL
 ** MODULES      : QOS (QoS Engine )
 **
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : QOS HAL Layer
 ** COPYRIGHT : Copyright (c) 2020, MaxLinear, Inc.
 ** Copyright (c) 2017 - 2018 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 **
 ****************************************************************************/

/*
 * Driver Header files
 */
#include "qos_mgr_stack_al.h"
#include <uapi/net/qos_mgr_common.h>
#include <net/qos_mgr/qos_mgr_tc_hook.h>
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
#include <net/intel_cbm_api.h>
#else
#include <net/lantiq_cbm_api.h>
#endif
#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include "qos_hal.h"
#include "qos_hal_ll.h"
#include "qos_hal_debug.h"
#include <net/switch_api/lantiq_gsw_api.h>

#define PPPOA_SUPPORT
#define NULL_EGRESS_PORT_ID 128
#define SCHEDULER_BLOCK_INPUT_ID_MAX 1023

int32_t g_Dp_Port;
int32_t g_Lookup_Mode;

/** CPU Port Resources */
int32_t g_CPU_PortId;
int32_t g_CPU_Sched;
int32_t g_CPU_Queue;

/** Global Resources */
uint32_t g_No_Of_QOS_Res_Port;
uint32_t g_No_Of_QOS_Res_Scheduler;
uint32_t g_No_Of_QOS_Res_Queue;
uint32_t g_No_Of_Ingress_Interface;

enum qos_hal_errorcode qos_hal_flush_queue_if_qocc(
			struct net_device *netdev,
			char *dev_name,
			int32_t index,
			int32_t qos_port,
			int32_t remap_to_qid,
			uint32_t flags);

int qos_hal_get_q_res_from_netdevice(
			struct net_device *netdev,
			char *dev_name,
			uint32_t sub_if,
			dp_subif_t *dp_if,
			uint32_t sched_type,
			uint32_t *qos_ports_cnt,
			void **res,
			uint32_t flags);

enum qos_hal_errorcode qos_hal_scheduler_cfg_get(uint32_t index);

int32_t qos_hal_get_node_id_from_prio(
			uint32_t prio,
			uint32_t type,
			uint32_t port,
			uint32_t sched,
			int32_t *nodeId);

int qos_hal_del_shaper_index(int shaper_index);

int32_t qos_hal_qos_init_cfg(void)
{
	int32_t ret = QOS_MGR_SUCCESS;

	return ret;
}
int32_t qos_hal_qos_uninit_cfg(void)
{
	int32_t ret = QOS_MGR_SUCCESS;
	return ret;
}

/* ========================================================================== */
int32_t qos_hal_alloc_queue(int32_t q_id)
{
	int i;
	struct dp_node_alloc node = {0};
	for (i = g_No_Of_QOS_Res_Queue; i < EGRESS_QUEUE_ID_MAX; i++) {
		if (qos_hal_queue_track[i].is_enabled == 0)
			break;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free Queue Index is [%d]\n", __func__, i);

	if ( i == EGRESS_QUEUE_ID_MAX ) {
		return QOS_MGR_FAILURE;
	}

	node.type = DP_NODE_QUEUE;
	node.id.q_id = q_id;
	if (dp_node_alloc(&node, 0) == DP_FAILURE)
		return QOS_HAL_STATUS_ERR;

	qos_hal_queue_track[i].queue_id = node.id.q_id;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Return Physical Queue ID is %d\n", __func__, i);
	return i;
}

int32_t qos_hal_get_free_queue_index(void)
{
	int32_t i = QOS_MGR_FAILURE;
	for (i = g_No_Of_QOS_Res_Queue; i < EGRESS_QUEUE_ID_MAX; i++) {
		if (qos_hal_queue_track[i].is_enabled == 0)
			break;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free Queue Index is [%d]\n", __func__, i);

	if (i == EGRESS_QUEUE_ID_MAX)
		return QOS_MGR_FAILURE;

	return i;
}

int32_t qos_hal_get_free_queue(void)
{
	int32_t i;
	struct dp_node_alloc node = {0};

	i = qos_hal_get_free_queue_index();
	if (i == QOS_MGR_FAILURE)
		return QOS_MGR_FAILURE;

	node.inst = g_Inst;
	node.dp_port = g_Dp_Port;
	node.type = DP_NODE_QUEUE;
	node.id.q_id = DP_NODE_AUTO_ID;
	if (dp_node_alloc(&node, 0) == DP_FAILURE)
		return QOS_HAL_STATUS_MAX_QUEUES_REACHED;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free Queue ID is %d\n", __func__, node.id.q_id);
	qos_hal_queue_track[i].queue_id = node.id.q_id;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Return Queue ID is %d\n", __func__, i);
	return i;

}

int32_t qos_hal_get_qid_from_queue_index(uint32_t index, uint32_t *qid)
{
	*qid = qos_hal_queue_track[index].queue_id;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Queue ID for index [%d] is [%d]\n", __func__, index, *qid);
	return QOS_MGR_SUCCESS;
}

int32_t qos_hal_get_queue_index_from_qid(uint32_t qid)
{
	int i;
	for (i = g_No_Of_QOS_Res_Queue; i < EGRESS_QUEUE_ID_MAX; i++) {
		if (qos_hal_queue_track[i].queue_id != 0) {
			if (qos_hal_queue_track[i].queue_id == qid) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Queue index for Physical Queue [%d]-->[%d]\n",
						__func__, qid, i);
				break;
			}
		}
	}
	return i;
}

int32_t qos_hal_get_free_dp_queue(void)
{
	int i;
	for (i = 0; i < g_No_Of_QOS_Res_Queue; i++) {
		if (qos_hal_queue_track[i].is_enabled == 0)
			break;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free Queue ID is %d\n", __func__, i);
	return i;
}

int32_t qos_hal_get_free_scheduler(void)
{
	int i;
	struct dp_node_alloc node = {0};
	for (i = g_No_Of_QOS_Res_Scheduler; i < QOS_HAL_MAX_SCHEDULER; i++) {
		if (qos_hal_sched_track[i].in_use == 0)
			break;
	}

	if ( i == QOS_HAL_MAX_SCHEDULER ){
		return INVALID_SCHED_ID;
	}

	node.inst = g_Inst;
	node.dp_port = g_Dp_Port;
	node.type = DP_NODE_SCH;
	node.id.sch_id = DP_NODE_AUTO_ID;
	if (dp_node_alloc(&node, 0) == DP_FAILURE)
		return QOS_HAL_STATUS_MAX_SCHEDULERS_REACHED;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Alloc Scheduler node is [%d]:[%d]\n", __func__, i, node.id.sch_id);
	qos_hal_sched_track[i].id = node.id.sch_id;
	qos_hal_sched_track[i].leaf_mask = 0;
	return i;
}

int32_t qos_hal_get_schedid_from_sched_index(uint32_t index, uint32_t *schedid)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Find sched ID for index %d\n", __func__, index);
	*schedid = qos_hal_sched_track[index].id;
	return QOS_MGR_SUCCESS;
}

int32_t qos_hal_get_sched_index_from_schedid(uint32_t schedid)
{
	int i;
	for (i = g_No_Of_QOS_Res_Scheduler; i < QOS_HAL_MAX_SCHEDULER; i++) {
		if (qos_hal_sched_track[i].id == schedid) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Scheduler index for Logical Node [%d] is [%d]\n", __func__, schedid, i);
			return i;
		}
	}
	return QOS_MGR_FAILURE;
}
/**
STATIC int qos_hal_get_free_dp_scheduler(void)
{
	int i;
	for (i = 0; i < g_No_Of_QOS_Res_Scheduler; i++) {
		if (qos_hal_sched_track[i].in_use == 0)
			break;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free Scheduler ID is %d\n", __func__, i);
	return i;
}
*/

int32_t qos_hal_get_free_port(void)
{
	int i;
	for (i = 0; i < QOS_HAL_MAX_EGRESS_PORT; i++) {
		if (qos_hal_port_track[i].is_enabled == 0)
			break;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Free Port ID is [%d]\n", i);
	return i;
}

int32_t qos_hal_get_free_shaper(void)
{
	int i;
	for (i = 1; i < QOS_HAL_MAX_SHAPER; i++) {
		if (qos_hal_shaper_track[i].is_enabled == 0)
			break;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free Shaper Index is %d\n", __func__, i);
	return i;
}

int32_t qos_hal_scheduler_free_input_get(const uint16_t idx, uint8_t *leaf)
{
	int i;
	uint8_t leaf_mask;
	if (idx >= QOS_HAL_MAX_SCHEDULER)
		return QOS_HAL_STATUS_MAX_SCHEDULERS_REACHED;

	leaf_mask = qos_hal_sched_track[idx].leaf_mask;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Scheduler [%d] has leaf mask [%x]\n",
			__func__, idx, leaf_mask);
	/* get any free leaf out of 0 to 7 */
	for (i = 0; i < 8; i++) {
		if (!((leaf_mask >> i) & 0x1)) {
			leaf_mask |= (1 << i);
			break;
		}
	}
	if (i >= 8)
		return -1;

	*leaf = i;

	return 0;
}

int32_t qos_hal_port_free_input_get(const uint16_t idx, uint8_t *leaf)
{
	int i;
	uint8_t leaf_mask;

	leaf_mask = qos_hal_port_track[idx].leaf_mask;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port [%d] has leaf mask [%x]\n", __func__, idx, leaf_mask);
	/* get any free leaf out of 0 to 7 */
	for (i = 0; i < 8; i++) {
		if (!((leaf_mask >> i) & 0x1)) {
			leaf_mask |= (1 << i);
			break;
		}
	}
	if (i >= 8)
		return -1;

	*leaf = i;

	return 0;
}

char *get_sb_policy(int idx)
{
	if (qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WFQ)
		return "WFQ";
	else if (qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WSP)
		return "WSP";
	else if (qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WRR)
		return "WRR";
	else
		return "Unknown";
}

int qos_hal_dump_sb_info(uint32_t sb_index)
{
	struct qos_hal_sched_track_info Sindex;

	QOS_HAL_ASSERT(sb_index >= QOS_HAL_MAX_SCHEDULER);

	Sindex = qos_hal_sched_track[sb_index];

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n ======== SCHED INFO =========\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SBID: %d\n", sb_index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "NODE: %d\n", Sindex.id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Used: %d\n", Sindex.in_use);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "LVL: %d\n", Sindex.level);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "LEAF-MASK: %x\n", Sindex.leaf_mask);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Policy: %s\n", get_sb_policy(sb_index));
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SB Out Conn Type: %s\n", (Sindex.sched_out_conn == 0) ? "Port" : "SB");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "OMID: %d(SB:%d)\n", Sindex.omid, (Sindex.omid >> 3));
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Next Sched ID: %d\n", Sindex.next_sched_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Peer Sched Id: %d\n", Sindex.peer_sched_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Selector: %d\n", Sindex.priority_selector);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n =================\n");

	return QOS_HAL_STATUS_OK;
}

int qos_hal_dump_port_info(uint32_t port_index)
{
	struct qos_hal_port_track_info Sindex;

	QOS_HAL_ASSERT(port_index >= QOS_HAL_MAX_EGRESS_PORT);

	Sindex = qos_hal_port_track[port_index];

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n ======== PORT TRACK INFO =========\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "ENABLE: %d\n", Sindex.is_enabled);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "LEAF-MASK: %d\n", Sindex.leaf_mask);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "POLICY: %d\n", Sindex.policy);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "DEF SCHED-ID: %d\n", Sindex.default_sched_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "INPUT SCHED-ID: %x\n", Sindex.input_sched_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "PORT SCHED-ID: %d\n", Sindex.port_sched_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SHAPER SCHED-ID: %d\n", Sindex.shaper_sched_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "No of Qs: %d\n", Sindex.no_of_queues);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "No of SCHEDs: %d\n", Sindex.no_of_schedular);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "No of MAX-LEVEL: %d\n", Sindex.no_of_max_level);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper Index: %d\n", Sindex.tbs);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n ==================================\n");

	return QOS_HAL_STATUS_OK;
}

int qos_hal_dump_shaper_info(uint32_t shaper_index)
{
	struct qos_hal_shaper_track_info Sindex;

	QOS_HAL_ASSERT(shaper_index >= QOS_HAL_MAX_SHAPER);

	Sindex = qos_hal_shaper_track[shaper_index];

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n ======= SHAPER INFO ==========\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "ENABLE: %d\n", Sindex.is_enabled);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SHAPER ID: %d\n", Sindex.shaper_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SB INPUT: %d\n", Sindex.sb_input);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "ATTACH-TO: %d\n", Sindex.attach_to);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "ATTACH-To-ID: %x\n", Sindex.attach_to_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB INDEX: %d\n", Sindex.tb_cfg.index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB ENABLE: %d\n", Sindex.tb_cfg.enable);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB MODE: %d\n", Sindex.tb_cfg.mode);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB CIR: %d\n", Sindex.tb_cfg.cir);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB PIR: %d\n", Sindex.tb_cfg.pir);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB CBS: %d\n", Sindex.tb_cfg.cbs);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB PBS: %d\n", Sindex.tb_cfg.pbs);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB ATTACH-TO: %d\n", Sindex.tb_cfg.attach_to);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "TB ATTACH-TO-ID: %d\n", Sindex.tb_cfg.attach_to_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n =============================\n");

	return QOS_HAL_STATUS_OK;
}

int qos_hal_dump_queue_info(uint32_t queue_index)
{
	struct qos_hal_queue_track_info Qindex;

	QOS_HAL_ASSERT(queue_index >= QOS_HAL_MAX_EGRESS_QUEUES);

	Qindex = qos_hal_queue_track[queue_index];

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n ======= QUEUE INFO ==========\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "ENABLE: %d\n", Qindex.is_enabled);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "PHY QUEUE ID: %d\n", Qindex.queue_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Connected to SCHED-ID: %d\n", Qindex.connected_to_sched_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SCHED INPUT: %d\n", Qindex.sched_input);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "ATTACH-TO: %d\n", Qindex.attach_to);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Q-TYPE: %x\n", Qindex.q_type);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "PRIO WEIGHT: %d\n", Qindex.prio_weight);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SHAPER INDEX: %d\n", Qindex.tb_index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "EGRESS PORT NO: %d\n", Qindex.epn);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "USER Q INDEX: %d\n", Qindex.user_q_idx);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n =============================\n");

	return QOS_HAL_STATUS_OK;
}

/** ================================================================================*/
int32_t
qos_hal_get_counter_mode(
		int32_t *mode)
{
	struct dp_counter_conf cnt_conf = {0};
	dp_counter_mode_get(&cnt_conf, 0);
	*mode = cnt_conf.mode;
	return QOS_HAL_STATUS_OK;
}

int32_t
qos_hal_ll_get_qos_mib(
		int32_t queueid,
		struct qos_hal_qos_mib *qos_mib,
		uint32_t flag)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return QOS_HAL_STATUS_OK;
}

int32_t
qos_hal_ll_reset_qos_mib(
		int32_t queueid,
		uint32_t flag)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return QOS_HAL_STATUS_OK;
}

/** ===============================================================================*/

int qos_hal_get_subitf_via_ifname(char *dev_name, dp_subif_t *dp_subif)
{
	uint32_t m = 0, n = 0;
	int32_t ret = QOS_HAL_STATUS_ERR;
	struct qos_hal_user_subif_abstract *subif_index = NULL;

	for (m = 0; m <= 25; m++) {
		for (n = 0; n < QOS_HAL_MAX_SUB_IFID_PER_PORT; n++) {
			subif_index = qos_hal_user_sub_interface_info + (m * QOS_HAL_MAX_SUB_IFID_PER_PORT) + n;
			if (strncmp(subif_index->dp_subif.ifname, dev_name, 16) == 0) {
				dp_subif->subif  = subif_index->dp_subif.subif << 8;
				dp_subif->port_id  = subif_index->dp_subif.port_id;
				return QOS_HAL_STATUS_OK;
			}
		}
	}
	return ret;
}

int qos_hal_get_q_map(
		uint32_t inst,
		uint32_t q_index,
		uint32_t *no_entries,
		struct dp_queue_map_entry **p_qmap,
		int32_t flag)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t no_of_entry = 0;
	int32_t qid = -1;
	struct dp_queue_map_get q_map = {0};

	qos_hal_get_qid_from_queue_index(q_index, &qid);
	if (qid == -1)
		return QOS_HAL_STATUS_ERR;

	q_map.inst = inst;
	q_map.q_id = qid;
	if (dp_queue_map_get(&q_map, 0) == DP_FAILURE) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> queue_map_get failed for Queue [%d]\n", __func__, __LINE__, qid);
		goto QMAP_ERR_HANDLER;
	}
	no_of_entry = q_map.num_entry;
	if (no_of_entry == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No Queue Map entry\n", __func__);
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Allocate resources for [%d] entries\n", __func__, no_of_entry);
		q_map.qmap_entry = kmalloc(sizeof(struct dp_queue_map_entry) * no_of_entry, GFP_KERNEL);
		if (q_map.qmap_entry == NULL) {
			ret = QOS_HAL_STATUS_ERR;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> Qmap %d number resource allocation failure\n", __func__, __LINE__, no_of_entry);
			goto QMAP_ERR_HANDLER;
		}
		q_map.qmap_size = no_of_entry;
		if (dp_queue_map_get(&q_map, 0) == DP_FAILURE) {
			ret = QOS_HAL_STATUS_ERR;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> dp_queue_map_get failed for Queue %d\n", __func__, __LINE__, q_index);
			goto QMAP_ERR_HANDLER;
		}
		*no_entries = q_map.num_entry;
		*p_qmap = q_map.qmap_entry;
	}
	return QOS_HAL_STATUS_OK;
QMAP_ERR_HANDLER:
	if (no_of_entry != 0)
		kfree(q_map.qmap_entry);
	return ret;
}

int qos_hal_add_q_map(uint32_t q_index, uint32_t pmac_port, struct dp_queue_map_entry *p_qmap, uint32_t bit_mask)
{
	uint32_t qid;
	struct dp_queue_map_set qmap_set = {0};
	struct dp_q_map_mask mask = {0};

	qos_hal_get_qid_from_queue_index(q_index, &qid);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add QMap for the queue: [%d]\n", __func__, qid);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> TC: [%d]\n", __func__, p_qmap->qmap.class);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif: [%d]\n", __func__, p_qmap->qmap.subif);

	qmap_set.inst = g_Inst;
	qmap_set.q_id = qid;

	(bit_mask & CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE) ? (mask.flowid = 1) : (mask.flowid = 0);
	(bit_mask & CBM_QUEUE_MAP_F_EN_DONTCARE) ? (mask.enc = 1) : (mask.enc = 0);
	(bit_mask & CBM_QUEUE_MAP_F_DE_DONTCARE) ? (mask.dec = 1) : (mask.dec = 0);
	(bit_mask & CBM_QUEUE_MAP_F_MPE1_DONTCARE) ? (mask.mpe1 = 1) : (mask.mpe1 = 0);
	(bit_mask & CBM_QUEUE_MAP_F_MPE2_DONTCARE) ? (mask.mpe2 = 1) : (mask.mpe2 = 0);
	(bit_mask & CBM_QUEUE_MAP_F_TC_DONTCARE) ? (mask.class = 1) : (mask.class = 0);
	(bit_mask & CBM_QUEUE_MAP_F_SUBIF_DONTCARE) ? (mask.subif = 1) : (mask.subif = 0);
	mask.egflag = 1;

	memcpy(&qmap_set.map, &p_qmap->qmap, sizeof(struct dp_q_map));
	memcpy(&qmap_set.mask, &mask, sizeof(struct dp_q_map_mask));

	if (dp_queue_map_set(&qmap_set, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> dp_queue_map_set failed for Queue [%d]\n", __func__, __LINE__, qid);
		return QOS_HAL_STATUS_ERR;
	}
	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_handle_q_map(
		uint32_t q_new,
		uint32_t prio_level,
		uint32_t schedid,
		char *tc,
		uint8_t no_of_tc,
		uint32_t flowId,
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t flags)
{
	int32_t i = 0;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Interface:[%s]\n",
		__func__, subif_index->netdev->name);

	if ((prio_level == subif_index->default_prio) && ((flags & QOS_MGR_Q_F_DEFAULT) == QOS_MGR_Q_F_DEFAULT)) {
		struct dp_queue_map_entry q_map;
		int32_t q_map_mask = 0, x = 0;

		/** Configure the QMAP table to connect to the Egress queue*/
		memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

		q_map.qmap.dp_port = subif_index->dp_subif.port_id;
		q_map.qmap.subif = subif_index->dp_subif.subif << 8;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP subif:[%d] Qmap subif:[0x%x]\n", __func__,
				subif_index->dp_subif.subif, q_map.qmap.subif);

		if (subif_index->is_logical) { /* for the logical interface */
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Deafult Q --> Flow Id %d for VLAN interface subif: %d\n",
					__func__, flowId, subif_index->dp_subif.subif);
			/** This is a hack right now. I am passing the cfg->intfId_en. This is required
			  for ATM as the interface with subif>0 could be the default */
			if (schedid == 0) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Hack for VLAN with cfg->intfId_en: %d\n", __func__, schedid);
				q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
					CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
					CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_TC_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
			} else {
				q_map.qmap.flowid = flowId;
				if (flowId > 3) {
					q_map.qmap.class |= 8;
					q_map.qmap.flowid = (flowId & 0x3);
				}
				q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_TC_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Map Default Q for logical interface: %d\n", __func__, q_new);
			qos_hal_add_q_map(q_new, g_CPU_PortId, &q_map, q_map_mask);
			subif_index->default_q = q_new;
			for (x = 1; x < subif_index->queue_cnt; x++) {
				q_map_mask = 0;
				/** Configure the QMAP table to connect to the Egress queue*/
				memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

				q_map.qmap.dp_port = subif_index->dp_subif.port_id;
				q_map.qmap.flowid = flowId;
				q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
				for (i = 0; i < subif_index->user_queue[x].no_of_tc; i++) {
					q_map.qmap.class = subif_index->user_queue[x].qmap[i];
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Q --> Add QMAP for Port %d and TC: %d\n",
							__func__, q_map.qmap.dp_port, q_map.qmap.class);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Q --> User Index %d Q Index: %d\n",
							__func__, x, subif_index->user_queue[x].queue_index);
					qos_hal_add_q_map(subif_index->user_queue[x].queue_index, g_CPU_PortId, &q_map, q_map_mask);
				}
			}

		} else { /* default Q of Physical interface */
			if (g_Lookup_Mode == 2) {
				q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
					CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
					CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_TC_DONTCARE;
				q_map.qmap.mpe1 = 0;
				q_map.qmap.mpe2 = 1;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Lookup Mode 2 with subif:[0x%x]\n",
						__func__, q_map.qmap.subif);
			}
			else {
				q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
					CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
					CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_TC_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Map Default Q: %d\n", __func__, q_new);
			qos_hal_add_q_map(q_new, g_CPU_PortId, &q_map, q_map_mask);
			subif_index->default_q = q_new;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue count: %d\n", __func__, subif_index->queue_cnt);
			for (x = 1; x < subif_index->queue_cnt; x++) {
				q_map_mask = 0;
				/** Configure the QMAP table to connect to the Egress queue*/
				memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

				q_map.qmap.dp_port = subif_index->dp_subif.port_id;
				q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
					CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
					CBM_QUEUE_MAP_F_EN_DONTCARE |
					CBM_QUEUE_MAP_F_DE_DONTCARE |
					CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;

				for (i = 0; i < subif_index->user_queue[x].no_of_tc; i++) {
					q_map.qmap.class = subif_index->user_queue[x].qmap[i];
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Deafult Q --> Add QMAP for Port %d and TC: %d\n",
							__func__, q_map.qmap.dp_port, q_map.qmap.class);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Q --> User Index %d Q Index: %d\n",
							__func__, x, subif_index->user_queue[x].queue_index);
					qos_hal_add_q_map(subif_index->user_queue[x].queue_index, g_CPU_PortId, &q_map, q_map_mask);
				}
			}
		}
	} else { /* not for default q */
		struct dp_queue_map_entry q_map;
		uint32_t q_map_mask = 0;
		/** Configure the QMAP table to connect to the Egress queue*/
		memset(&q_map, 0, sizeof(struct dp_queue_map_entry));
		q_map.qmap.dp_port = subif_index->dp_subif.port_id;
		q_map.qmap.subif = subif_index->dp_subif.subif << 8;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP subif:[%d] Qmap subif:[0x%x]\n", __func__,
				subif_index->dp_subif.subif, q_map.qmap.subif);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No Of TC: %d\n", __func__, no_of_tc);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> logical: %d\n", __func__, subif_index->is_logical);
		for (i = 0; i < no_of_tc; i++) {
			if (g_Lookup_Mode == 0 || g_Lookup_Mode == 2) {
				q_map.qmap.class = *(tc+i);
				if (subif_index->is_logical) { /* for the logical interface */
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Flow Id %d for VLAN interface subif: %d\n", __func__, flowId, subif_index->dp_subif.subif);
					/** This is a hack right now. I am passing the cfg->intfId_en. This is required
					  for ATM as the interface with subif>0 could be the default */
					if (schedid == 0) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Hack for VLAN with cfg->intfId_en: %d\n", __func__, schedid);
						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					} else {

						q_map.qmap.flowid = flowId;
						if (flowId > 3) {
							q_map.qmap.class |= 8;
							q_map.qmap.flowid = (flowId & 0x3);
						}
						q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					}
				} else {
					if (g_Lookup_Mode == 2) {
						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE;
						q_map.qmap.mpe1 = 0;
						q_map.qmap.mpe2 = 1;
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Lookup Mode 2 with subif:[0x%x]\n",
								__func__, q_map.qmap.subif);
					}
					else {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Hack for VLAN with cfg->intfId_en: %d is zero\n",
								__func__, schedid);

						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					}
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add QMAP for Port %d and TC: %d\n", __func__, q_map.qmap.dp_port, q_map.qmap.class);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Index of New Q: %d\n", __func__, q_new);
			} else if (g_Lookup_Mode == 1) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Lookup Mode is 1\n", __func__);
				q_map.qmap.subif = *(tc+i);
				q_map.qmap.dp_port = subif_index->dp_subif.port_id;

				q_map_mask |= CBM_QUEUE_MAP_F_MPE1_DONTCARE |
					CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
			}
			qos_hal_add_q_map(q_new, g_Dp_Port, &q_map, q_map_mask);
			subif_index->user_queue[subif_index->queue_cnt].qmap[i] = *(tc+i);
		}
	}

	subif_index->user_queue[subif_index->queue_cnt].no_of_tc = no_of_tc;
	/* Special handling for DL port */
	if ((flags & QOS_MGR_Q_F_WLANDP) == QOS_MGR_Q_F_WLANDP) {
		struct qos_hal_dp_res_info res = {0};
		struct dp_queue_map_entry q_map;
		uint32_t q_map_mask = 0;

		memset(&q_map, 0, sizeof(struct dp_queue_map_entry));
		q_map_mask = 0;

		q_map.qmap.dp_port = subif_index->dp_subif.port_id;
		q_map.qmap.flowid = 0;
		q_map.qmap.enc = 1;
		q_map.qmap.dec = 1;
		q_map.qmap.mpe1 = 0;
		q_map.qmap.mpe2 = 0;
		q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Set QMAP for the Ingress Q: %d\n", __func__, res.q_ingress);
		qos_hal_add_q_map(res.q_ingress, g_CPU_PortId, &q_map, q_map_mask);
	}
	return QOS_HAL_STATUS_OK;
}

int qos_hal_del_q_map(uint32_t q_index, uint32_t pmac_port, struct dp_queue_map_entry *p_qmap, uint32_t bit_mask)
{
	struct dp_queue_map_set qmap_set = {0};

	qmap_set.inst = 0;
	qmap_set.q_id = q_index;

	dp_queue_map_set(&qmap_set, 0);

	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_egress_queue_create function is used to assign the scheduler and
      egress port to an egress queue.
*/
enum qos_hal_errorcode
qos_hal_egress_queue_create(const struct qos_hal_equeue_create *param)
{
	uint16_t idx = 0, scheduler_input;
	uint8_t leaf;
	int32_t ret;
	uint32_t qid, p_sch_id;
	struct dp_node_link node_info = {0};
	struct dp_node_prio prio_info = {0};

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> [%d] [%d] [%d] [%d]\n",
			__func__,
			param->index,
			param->scheduler_input,
			param->prio,
			param->egress_port_number);

	QOS_HAL_ASSERT(param->index >= EGRESS_QUEUE_ID_MAX ||
			(param->egress_port_number >= QOS_HAL_MAX_EGRESS_PORT));
	node_info.inst = g_Inst;
	node_info.dp_port = g_Dp_Port;
	node_info.arbi = ARBITRATION_WSP; /* need to change */
	node_info.prio_wfq = leaf = param->prio;

	if ((param->scheduler_input >> 3) >= QOS_HAL_MAX_SCHEDULER) {
		node_info.p_node_type = DP_NODE_PORT;
		node_info.p_node_id.cqm_deq_port = param->egress_port_number;
		node_info.cqm_deq_port.cqm_deq_port = param->egress_port_number;
			ret = qos_hal_port_free_input_get(param->egress_port_number, &leaf);
			if (ret < 0) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Free Input\n", __func__, __LINE__);
				return QOS_HAL_STATUS_NOT_AVAILABLE;
			}
		if ((qos_hal_port_track[param->egress_port_number].policy == QOS_HAL_POLICY_WFQ)
			|| (qos_hal_port_track[param->egress_port_number].policy == QOS_HAL_POLICY_WRR)) {
			node_info.prio_wfq = param->iwgt;
		}
		qos_hal_queue_track[param->index].attach_to = QOS_HAL_INPUT_TO_PORT;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port: [%d]\n", __func__, node_info.p_node_id.cqm_deq_port);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Arbitration is: [%d]\n", __func__, qos_hal_port_track[param->egress_port_number].policy);
	} else {
		scheduler_input = param->scheduler_input;
		idx = param->scheduler_input >> 3;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler Id: [%d]\n", __func__, idx);
		QOS_HAL_ASSERT(idx >= QOS_HAL_MAX_SCHEDULER);

		if (qos_hal_sched_track[idx].in_use == 0) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d  Scheduler is not in use \n", __func__, __LINE__);
			return QOS_HAL_STATUS_NOT_AVAILABLE;
		}
		leaf = scheduler_input & 7;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> SB: [%d] leaf: [%d]\n", __func__, idx, leaf);

		if ((qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WFQ)
			|| (qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WRR)) {
			scheduler_input &= ~7;
			ret = qos_hal_scheduler_free_input_get(idx, &leaf);
			if (ret < 0) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Free Input\n",
						__func__, __LINE__);
				return QOS_HAL_STATUS_NOT_AVAILABLE;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Got free input [%d]\n", leaf);
			scheduler_input |= (uint32_t)leaf;

			node_info.prio_wfq = param->iwgt;
			node_info.arbi = ARBITRATION_WRR;
		}
		qos_hal_get_schedid_from_sched_index(idx, &p_sch_id);
		node_info.p_node_type = DP_NODE_SCH;
		node_info.p_node_id.sch_id = p_sch_id;
	}
	qos_hal_get_qid_from_queue_index(param->index, &qid);
	node_info.node_type = DP_NODE_QUEUE;
	node_info.node_id.q_id = qid;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>LL qos_egress_queue_create Q: [%d] SB: [%d] P: [%d] Wt: [%d]\n",
			__func__,
			qid,
			param->scheduler_input,
			param->egress_port_number,
			param->iwgt);

	if (dp_node_link_add(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d  Failed to add queue node\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}
	prio_info.inst = g_Inst;
	prio_info.id.q_id = qid;
	prio_info.type = DP_NODE_QUEUE;

	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get arbi of the node %d\n",
				__func__, __LINE__, qid);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue %d has arbitration: [%d]\n",
			__func__, qid, prio_info.arbi);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Configure Queue %d to priority:[%d] arbi:[%d]\n",
			__func__, qid, node_info.prio_wfq, node_info.arbi);

	prio_info.arbi = node_info.arbi;
	if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node [%d]\n",
				__func__, __LINE__, qid);
		return QOS_HAL_STATUS_ERR;
	}
	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node %d\n",
				__func__, __LINE__, qid);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue %d has priority: [%d]\n",
			__func__, qid, prio_info.prio_wfq);

	prio_info.prio_wfq = node_info.prio_wfq;
	if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set prio of the node [%d]\n",
				__func__, __LINE__, qid);
		return QOS_HAL_STATUS_ERR;
	}

	/** Remember total number of queues for that port  */
	qos_hal_port_track[param->egress_port_number].no_of_queues++;

	/** Remember egress port number  */
	qos_hal_queue_track[param->index].epn = param->egress_port_number;
	/** Remember scheduler id to which it is connected to of that queue  */
	qos_hal_queue_track[param->index].connected_to_sched_id = INVALID_SCHED_ID;
	/** Remember scheduler id input to which it is connected to of that queue  */
	qos_hal_queue_track[param->index].sched_input = param->scheduler_input;
	/** mark the queue as enabled */
	qos_hal_queue_track[param->index].is_enabled = true;
	/** Remember the queue weight  */
	qos_hal_queue_track[param->index].prio_weight = param->iwgt;

	if ((param->scheduler_input >> 3) >= QOS_HAL_MAX_SCHEDULER) {
		/** Remember the leaf mask of the Port */
		qos_hal_port_track[param->egress_port_number].leaf_mask |= (1 << leaf) ;
		qos_hal_queue_track[param->index].attach_to = QOS_HAL_INPUT_TO_PORT;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected Port:leaf_mask = [%d]:[%x]\n", __func__,
				qos_hal_queue_track[param->index].epn,
				qos_hal_port_track[param->egress_port_number].leaf_mask);
	} else {
		/** Remember the leaf mask of the Scheduler */
		qos_hal_sched_track[idx].leaf_mask |= (1 << leaf) ;
		qos_hal_queue_track[param->index].connected_to_sched_id = idx;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected SB:leaf_mask = [%d]:[%x] \n", __func__,
				qos_hal_queue_track[param->index].connected_to_sched_id,
				qos_hal_sched_track[idx].leaf_mask);
	}

	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_queue_relink(const struct qos_hal_equeue_create *param)
{
	uint16_t idx = 0, scheduler_input;
	uint8_t leaf = 0, out_type = 0;
	int ret;
	uint32_t qid, p_sch_id;
	struct dp_qos_link node_info = {0};
	struct dp_node_prio prio_info = {0};

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Enter -------------------------------->\n", __func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "[%s] [%d] -> [%d] -> [%d] -> [%d]\n",
			__func__,
			param->index,
			param->scheduler_input,
			param->prio,
			param->egress_port_number);

	QOS_HAL_ASSERT(param->index >= EGRESS_QUEUE_ID_MAX ||
			(param->egress_port_number >= QOS_HAL_MAX_EGRESS_PORT));

	node_info.inst = g_Inst;
	node_info.dp_port = g_Dp_Port;
	node_info.q_prio_wfq = leaf = param->prio;
	node_info.cqm_deq_port = param->egress_port_number;
	node_info.q_arbi = qos_hal_port_track[param->egress_port_number].policy;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port:[%d] -> Policy: [%d]\n",
				__func__,
				node_info.cqm_deq_port,
				qos_hal_port_track[param->egress_port_number].policy);

	if ((param->scheduler_input >> 3) >= QOS_HAL_MAX_SCHEDULER) {
		qos_hal_queue_track[param->index].attach_to = QOS_HAL_INPUT_TO_PORT;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port:[%d]\n", __func__, node_info.cqm_deq_port);
	} else {
		scheduler_input = param->scheduler_input;
		idx = param->scheduler_input >> 3;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Parent Scheduler Id Index: [%d] level: [%d]\n",
					__func__,
					idx,
					qos_hal_sched_track[idx].level);
		QOS_HAL_ASSERT(idx >= QOS_HAL_MAX_SCHEDULER);

		leaf = scheduler_input & 7;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> SB:[%d] leaf:[%d]\n", __func__, idx, leaf);
		if ((qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WRR)
			|| (qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WFQ)) {
			scheduler_input &= ~7;
			ret = qos_hal_scheduler_free_input_get(idx, &leaf);
			if (ret < 0) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Free Input\n", __func__, __LINE__);
				return QOS_HAL_STATUS_NOT_AVAILABLE;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "got free input [%d]\n", leaf);
			scheduler_input |= (uint32_t)leaf;
		}
		qos_hal_get_schedid_from_sched_index(idx, &p_sch_id);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Parent scheduler Id: [%d] prioity/weight: [%d]\n", p_sch_id, qos_hal_sched_track[idx].prio_weight);

		node_info.n_sch_lvl = 1;
		node_info.sch[0].id = p_sch_id;
		node_info.sch[0].arbi = qos_hal_sched_track[idx].policy;
		node_info.sch[0].prio_wfq = qos_hal_sched_track[idx].prio_weight;
	}
	qos_hal_get_qid_from_queue_index(param->index, &qid);
	node_info.q_id = qid;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Linking Q:[%d] -> SB:[%d] -> P:[%d] Wt:[%d] Arbi:[%d]\n",
			__func__,
			qid,
			param->scheduler_input >> 3,
			param->egress_port_number,
			param->iwgt,
			node_info.q_arbi);

	if ((param->scheduler_input >> 3) < QOS_HAL_MAX_SCHEDULER) {
		int32_t i = 0;
		uint32_t p_node, loc, index, level;
		uint32_t sch_idx = idx;
		level = qos_hal_sched_track[idx].level;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler level : [%d]\n", __func__, level);
		for (i = 1; i < level + 1; i++) {
			node_info.n_sch_lvl = i + 1;
			index =  qos_hal_sched_track[sch_idx].omid;
			p_node = qos_hal_sched_track[sch_idx].omid >> 3;
			loc = qos_hal_sched_track[sch_idx].omid & 0x7;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> SB: [%d] -> SB: [%d/%d:%d]\n",
						__func__,
						sch_idx,
						p_node,
						qos_hal_sched_track[p_node].id,
						loc);

			node_info.sch[i].id = qos_hal_sched_track[p_node].id;
			node_info.sch[i].arbi = qos_hal_sched_track[p_node].policy;
			node_info.sch[i].prio_wfq = loc;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Linking Level:[%d] SB:[%d] Arbi:[%d] Prio:[%d]\n",
				__func__,
				i,
				node_info.sch[i].id,
				node_info.sch[i].arbi,
				node_info.sch[i].prio_wfq);
			sch_idx = p_node;
		}

	}

	if (dp_link_add(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:%d  Failed to add the link.\n", __func__, __LINE__);
		return -1;
	}

	/** Set the Priority/Arbitration of the Queue --> Scheduler/Port */
	prio_info.inst = g_Inst;
	prio_info.id.q_id = qid;
	prio_info.type = DP_NODE_QUEUE;

	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n", __func__, __LINE__, qid);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Set the priority of Queue: [%d] to [%d]\n", __func__, qid, param->prio);

	/*prio_info.arbi = ARBITRATION_WSP;
	if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node [%d]\n", __func__, __LINE__, qid);
	}*/

	if (qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WRR)
		prio_info.prio_wfq = param->iwgt;
	else
		prio_info.prio_wfq = leaf;
	if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set priority of the Queue Node [%d]\n", __func__, __LINE__, qid);
	}

	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n", __func__, __LINE__, qid);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue: [%d] --> priority: [%d] Arbi: [%d]\n",
				__func__,
				qid,
				prio_info.prio_wfq,
				prio_info.arbi);

	/** Set the Priority/Arbitration of the scheduler chain till Port*/
	if ((param->scheduler_input >> 3) < QOS_HAL_MAX_SCHEDULER) {
		int32_t i = 0;
		uint32_t p_node = 0, loc = 0, level;
		uint32_t sch_idx = idx;
		level = qos_hal_sched_track[idx].level;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler level for setting Prio/Arbi : [%d]\n", __func__, level);

		prio_info.inst = g_Inst;
		prio_info.type = DP_NODE_SCH;
		for (i = 0; i < level + 1; i++) {
			if (qos_hal_sched_track[sch_idx].sched_out_conn == 1) {
				loc = qos_hal_sched_track[sch_idx].omid & 0x7;
				p_node = qos_hal_sched_track[sch_idx].omid >> 3;
			} else if (qos_hal_sched_track[sch_idx].sched_out_conn == 0) {
				loc = qos_hal_sched_track[sch_idx].prio_weight;
				p_node = qos_hal_sched_track[sch_idx].omid;
			}

			qos_hal_get_schedid_from_sched_index(sch_idx, &p_sch_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Prio/Arbi Set for SB: [%d/%d] -> [%d/%d:%d]\n",
						__func__, sch_idx, p_sch_id, p_node, qos_hal_sched_track[p_node].id, loc);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Parent scheduler Id: [%d:%d]\n", sch_idx, p_sch_id);

			prio_info.id.sch_id = p_sch_id;
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n", __func__, __LINE__, p_sch_id);
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Get the priority of SB: [%d] --> [%d]\n", __func__, p_sch_id, loc);

			if (qos_hal_sched_track[sch_idx].sched_out_conn == 1) {
				prio_info.arbi = qos_hal_sched_track[p_node].policy;
			} else if (qos_hal_sched_track[sch_idx].sched_out_conn == 0) {
				prio_info.arbi = qos_hal_port_track[p_node].policy;
			}
			if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node [%d]\n", __func__, __LINE__, p_sch_id);
			}
			if (qos_hal_sched_track[idx].policy == QOS_HAL_POLICY_WRR)
				prio_info.prio_wfq = param->iwgt;
			else
				prio_info.prio_wfq = loc;

			if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node [%d]\n", __func__, __LINE__, p_sch_id);
			}
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n", __func__, __LINE__, p_sch_id);
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> SB: [%d] --> priority: [%d] Arbi: [%d]\n",
							__func__,
							sch_idx,
							prio_info.prio_wfq,
							prio_info.arbi);
			sch_idx = p_node;
		}
	}

	/** Remember scheduler id input to which it is connected to of that queue  */
	qos_hal_queue_track[param->index].sched_input = param->scheduler_input;
	/** mark the queue as enabled */
	qos_hal_queue_track[param->index].is_enabled = true;
	/** Remember the queue weight  */
	qos_hal_queue_track[param->index].prio_weight = param->iwgt;

	if ((param->scheduler_input >> 3) >= QOS_HAL_MAX_SCHEDULER) {
		/** Remember the leaf mask of the Port */
		qos_hal_port_track[param->egress_port_number].leaf_mask |= (1 << leaf) ;
		qos_hal_sched_track[idx].leaf_mask &= ~(1 << leaf);
		/** Remember scheduler id to which it is connected to of that queue  */
		qos_hal_queue_track[param->index].connected_to_sched_id = INVALID_SCHED_ID;
		qos_hal_queue_track[param->index].attach_to = QOS_HAL_INPUT_TO_PORT;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected Port:leaf_mask = [%d]:[%d] \n", __func__,
				qos_hal_queue_track[param->index].epn,
				qos_hal_port_track[param->egress_port_number].leaf_mask);
	} else {
		/** Remember the leaf mask of the Scheduler */
		qos_hal_sched_track[idx].leaf_mask |= (1 << leaf);
		qos_hal_port_track[param->egress_port_number].leaf_mask &= ~(1 << leaf) ;
		/** Remember scheduler id to which it is connected to of that queue  */
		qos_hal_queue_track[param->index].connected_to_sched_id = idx;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected SB:leaf_mask = [%d]:[%d]\n", __func__,
				qos_hal_queue_track[param->index].connected_to_sched_id,
				qos_hal_sched_track[idx].leaf_mask);
		/* mark scheduler as used*/
		qos_hal_sched_track[idx].in_use = true;
		/* remember scheduler output connection type*/
		qos_hal_sched_track[idx].sched_out_conn = out_type;
		/* remember scheduler output connection */
		qos_hal_sched_track[idx].omid = param->egress_port_number;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Exit -------------------------------->\n", __func__);
	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_egress_queue_get function is used to read back structural attributes
    of a queue.
*/
enum qos_hal_errorcode
qos_hal_egress_queue_get(uint32_t queue_index,
		struct qos_hal_equeue_create *out)
{
	uint32_t qid;
	struct dp_node_link node_info = {0};

	QOS_HAL_ASSERT(queue_index >= EGRESS_QUEUE_ID_MAX);

	qos_hal_get_qid_from_queue_index(queue_index, &qid);
	node_info.node_type = DP_NODE_QUEUE;
	node_info.node_id.q_id = qid;

	if (dp_node_link_get(&node_info, 0) == DP_FAILURE)
		return QOS_HAL_STATUS_ERR;

	out->index = queue_index;
	if (node_info.p_node_type == DP_NODE_PORT)
		out->egress_port_number = node_info.p_node_id.cqm_deq_port;
	else
		out->egress_port_number = node_info.cqm_deq_port.cqm_deq_port;

	out->scheduler_input = qos_hal_queue_track[queue_index].sched_input;

	return QOS_HAL_STATUS_OK;
}


/** The qos_hal_enable_disable_queue function is used to enable or disable any queue
    */
enum qos_hal_errorcode
qos_hal_enable_disable_queue(int index, int enable)
{
	uint32_t qid;
	struct dp_node_link_enable node_en = {0};

	QOS_HAL_ASSERT(index >= EGRESS_QUEUE_ID_MAX);

	qos_hal_get_qid_from_queue_index(index, &qid);
	node_en.type = DP_NODE_QUEUE;
	node_en.id.q_id = qid;
	node_en.en = enable;

	if (dp_node_link_en_set(&node_en, 0) == DP_FAILURE)
		return QOS_HAL_STATUS_ERR;

	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_reset_queue_track function is used to reset the queue track
    database entry for a particular queue index.
	queue_index - queue index
*/
void
qos_hal_reset_queue_track(uint32_t queue_index)
{
	/* Reset egress port number  */
	qos_hal_queue_track[queue_index].epn = NULL_EGRESS_PORT_ID;
	/* Reset scheduler id to which it is connected to of that queue  */
	qos_hal_queue_track[queue_index].connected_to_sched_id = INVALID_SCHED_ID;
	/* Reset scheduler input to which it is connected to of that queue  */
	qos_hal_queue_track[queue_index].sched_input = INVALID_SCHEDULER_INPUT_ID;
	/* mark the queue as disabled */
	qos_hal_queue_track[queue_index].is_enabled = false;
	/* Reset physical queue id */
	qos_hal_queue_track[queue_index].queue_id = 0;
	/* Reset user queue index */
	qos_hal_queue_track[queue_index].user_q_idx = 0;
}

/** The qos_hal_egress_queue_delete function is used to delete the queue from
    scheduler input
	index - queue index
*/
enum qos_hal_errorcode
qos_hal_egress_queue_delete(uint32_t queue_index)
{
	uint8_t leaf;
	uint32_t qid;
	uint32_t conn_sched, eg_port;
	struct dp_node_alloc del_node = {0};

	QOS_HAL_ASSERT(queue_index >= EGRESS_QUEUE_ID_MAX);

	qos_hal_get_qid_from_queue_index(queue_index, &qid);

	eg_port = qos_hal_queue_track[queue_index].epn;
	conn_sched = qos_hal_queue_track[queue_index].connected_to_sched_id;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected SB:leaf_mask = %d:%d \n",
			__func__, conn_sched,
			qos_hal_sched_track[conn_sched].leaf_mask);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Queue --> [%d/%d]\n",
			__func__, queue_index, qid);
	del_node.type = DP_NODE_QUEUE;
	del_node.id.q_id = qid;
	if (dp_node_free(&del_node, 0) == DP_FAILURE)
		return QOS_HAL_STATUS_ERR;

	if (conn_sched < QOS_HAL_MAX_SCHEDULER) {
		leaf = qos_hal_queue_track[queue_index].sched_input & 7;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Queue sched input:leaf = [%d:%d] \n",
			__func__, qos_hal_queue_track[queue_index].sched_input, leaf);
		qos_hal_sched_track[conn_sched].leaf_mask &= ~(1<<leaf);
	} else {
		leaf = qos_hal_queue_track[queue_index].prio_weight;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Queue Port:leaf = [%d:%d] \n",
			__func__, eg_port, leaf);
		qos_hal_port_track[eg_port].leaf_mask &= ~(1<<leaf);
	}

	qos_hal_port_track[qos_hal_queue_track[queue_index].epn].no_of_queues--;

	qos_hal_reset_queue_track(queue_index);
	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_safe_queue_delete(struct net_device *netdev,
			char *dev_name,
			int32_t index,
			int32_t qos_port,
			int32_t remap_to_qid,
			uint32_t flags)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Queue %d for netdev [%s]\n",
			__func__, index, netdev->name);
	if (qos_hal_egress_queue_delete(index) == QOS_MGR_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to delete Queue\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_relink_queue_cfg_param(
			uint32_t qindex,
			struct qos_hal_user_subif_abstract *subif_index)
{
	uint32_t usr_q_idx = 0;
	struct qos_hal_equeue_cfg q_param = {0};
	struct qos_hal_equeue_cfg *eqcfg = NULL;

	eqcfg = &subif_index->user_queue[usr_q_idx].ecfg;
	usr_q_idx = qos_hal_queue_track[qindex].user_q_idx;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Config WRED params for queue:[%d]\n",
				__func__, usr_q_idx);

	q_param.index = qindex;
	q_param.enable = 1;

	if (eqcfg->wred_enable == 1) {
		q_param.drop_threshold_green_min =
				eqcfg->drop_threshold_green_min;
		q_param.drop_threshold_green_max =
				eqcfg->drop_threshold_green_max;
		q_param.drop_threshold_yellow_min =
				eqcfg->drop_threshold_yellow_min;
		q_param.drop_threshold_yellow_max =
				eqcfg->drop_threshold_yellow_max;
		q_param.drop_probability_green =
				eqcfg->drop_probability_green;
		q_param.drop_probability_yellow =
				eqcfg->drop_probability_yellow;
		q_param.drop_threshold_unassigned =
				eqcfg->drop_threshold_unassigned;
		q_param.drop_threshold_red =
				eqcfg->drop_threshold_red;
		q_param.wred_enable = 1;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "green_min:[%d] max:[%d]\n",
				q_param.drop_threshold_green_min,
				q_param.drop_threshold_green_max);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "yellow_min:[%d] max:[%d]\n",
				q_param.drop_threshold_yellow_min,
				q_param.drop_threshold_yellow_max);
	} else if (eqcfg->wred_enable == 0)
		q_param.wred_enable = 0;

	if (qos_hal_egress_queue_cfg_set(&q_param) != QOS_HAL_STATUS_OK) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Fail to config [Q:%d] DT/WRED params\n",
				__func__, q_param.index);
		return QOS_MGR_FAILURE;
	}

	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_relink_child_to_new_parent(
			uint32_t port,
			uint32_t sched,
			uint32_t from_node_type,
			struct qos_hal_user_subif_abstract *subif_index)
{
	uint32_t to_node = 0, i;
	int32_t temp = -1;
	struct dp_node_child node_info = {0};
	struct dp_node child_node = {0};
	struct qos_hal_equeue_create q_param = {0};

	node_info.type = from_node_type;
	if (node_info.type == DP_NODE_SCH) {
		node_info.id.sch_id = qos_hal_sched_track[sched].id;
		to_node = port;
	} else {
		node_info.id.cqm_deq_port = port;
		to_node = qos_hal_sched_track[sched].id;
	}

	if (dp_children_get(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to get the Children\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Relink %d child to %d Node\n", __func__, node_info.num, to_node);

	for (i = 0; i < node_info.num; i++) {
		uint32_t qindex;
		uint32_t q_loc = 0;

		child_node = node_info.child[i];
		if (child_node.type == DP_NODE_QUEUE) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> leaf:[%d] has child id:[%d] type:[%d]\n",
					__func__, i, child_node.id.q_id, child_node.type);

			memset(&q_param, 0, sizeof(struct qos_hal_equeue_create));
			qindex = qos_hal_get_queue_index_from_qid(child_node.id.q_id);
			/* Here qindex EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
			if (qindex >= EGRESS_QUEUE_ID_MAX)
				return QOS_HAL_STATUS_ERR;
			q_param.index = qindex;
			q_param.egress_port_number = port;
			qos_hal_queue_prio_get(child_node.id.q_id, &q_loc);
			q_param.scheduler_input = (sched << 3 | q_loc);
			q_param.prio = q_loc;

			if (from_node_type == DP_NODE_SCH)
				q_param.scheduler_input = QOS_HAL_MAX_SCHEDULER << 3;

			if (qos_hal_queue_relink(&q_param) != QOS_HAL_STATUS_OK) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> Failed to Relink Q:[%d]\n",
						__func__, __LINE__, child_node.id.q_id);
				return QOS_HAL_STATUS_ERR;
			}
			if (qos_hal_relink_queue_cfg_param(
					qindex,
					subif_index) != QOS_HAL_STATUS_OK) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> Failed to configure Q:[%d]\n",
						__func__, __LINE__, child_node.id.q_id);
				return QOS_HAL_STATUS_ERR;
			}

		} else if (child_node.type == DP_NODE_SCH) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>leaf:%d has child id:%d type:%d\n",
					__func__, i, child_node.id.sch_id, child_node.type);
			/*qos_hal_sched_out_remap(new_base_sched, qos_hal_sched_track[new_base_sched].level- 1, old_omid , 0, old_omid & 7); */
		}
	}
	qos_hal_get_node_id_from_prio(0, DP_NODE_SCH, port, sched, &temp);
	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_scheduler_in_weight_update function is used to configure
    the weight of a queue.
*/
enum qos_hal_errorcode
qos_hal_scheduler_in_weight_update(uint32_t sbin, uint32_t weight)
{
	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_egress_queue_cfg_set function is used to configure a priority queue.
*/
enum qos_hal_errorcode
qos_hal_egress_queue_cfg_set(const struct qos_hal_equeue_cfg *param)
{
	uint32_t qid;
	struct dp_queue_conf q_cfg = {0};

	QOS_HAL_ASSERT(param->index >= EGRESS_QUEUE_ID_MAX ||
			param->enable > 1 ||
			param->wred_enable > 1);

	QOS_HAL_ASSERT(param->drop_threshold_green_max <
			param->drop_threshold_green_min);
	QOS_HAL_ASSERT(param->drop_threshold_yellow_max <
			param->drop_threshold_yellow_min);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Get the Drop params for Q %d\n",
			__func__, param->index);

	if (!param->spl_conn) {
		qos_hal_get_qid_from_queue_index(param->index, &qid);
		q_cfg.inst = g_Inst;
		q_cfg.q_id = qid;
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> is called for special connection. \n",
				__func__);
		q_cfg.q_id = param->index;
	}

	if (dp_queue_conf_get(&q_cfg, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>:%d Failed in dp_queue_conf_get() \n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	if (param->wred_enable == 1) {
		q_cfg.min_size[0] = param->drop_threshold_green_min;
		q_cfg.max_size[0] = param->drop_threshold_green_max;
		q_cfg.min_size[1] = param->drop_threshold_yellow_min;
		q_cfg.max_size[1] = param->drop_threshold_yellow_max;
		q_cfg.wred_slope[0] = param->drop_probability_green;
		q_cfg.wred_slope[1] = param->drop_probability_yellow;
		q_cfg.drop = DP_QUEUE_DROP_WRED;
		q_cfg.act = BIT(param->enable);
		q_cfg.wred_min_guaranteed = param->drop_threshold_unassigned;
		q_cfg.wred_max_allowed = param->drop_threshold_red;
	} else if (param->wred_enable == 0) {
		q_cfg.act = BIT(param->enable);
		q_cfg.drop = DP_QUEUE_DROP_TAIL;
		q_cfg.min_size[0] = 0;
		q_cfg.min_size[1] = 0;
		q_cfg.wred_min_guaranteed = QOS_HAL_QWRED_MIN_GUARANTEED;
		if (param->q_len != 0) {
			q_cfg.wred_max_allowed = param->q_len;
		} else {
			q_cfg.wred_max_allowed = QOS_HAL_QWRED_MAX_ALLOWED;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Set the Drop Mode to %d\n",
				__func__, q_cfg.drop);
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Drop Mode not supported\n",
				__func__);
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "q_cfg.q_id:0x%x  q_cfg.wred_max_allowed:0x%x \n", q_cfg.q_id, q_cfg.wred_max_allowed);

	if (dp_queue_conf_set(&q_cfg, 1) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:%d Failed to set the node link\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_enable_queue_after_flush(struct net_device *netdev, char *dev_name, int32_t index, int32_t qos_port, int32_t remap_to_qid, uint32_t flags)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <----- Exit \n", __func__);
	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_flush_queue_if_qocc(struct net_device *netdev, char *dev_name, int32_t index, int32_t qos_port, int32_t remap_to_qid, uint32_t flags)
{

	return QOS_HAL_STATUS_OK;
}
/** The qos_hal_scheduler_cfg_get function is used to read back the scheduling
    policy of a scheduler in the Packet Engine hardware module.
*/
enum qos_hal_errorcode
qos_hal_scheduler_cfg_get(uint32_t index)
{
	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_scheduler_create function is used to create a scheduler ID in the
    scheduler table.
*/
/**
    The following parameters must be configured:
	- scheduler_id            : Managed Entity ID of the scheduler
				Return an error if this number is already in use
				(QOS_HAL_STATUS_NOT_AVAILABLE).
	- index                 : Identifies one of the available schedulers.
				Return an error, if no more scheduler is
				available (QOS_HAL_STATUS_NOT_AVAILABLE).
				Else return the selected value.
				The relationship between scheduler_id and
				index is stored in a software variable.
	- use_regular           : This parameter selects
				if the scheduler is attached to the egress port
				identified by port_idx (use_regular==1) or to
				the input of another scheduler identified by
				connected_scheduler_index (use_regular==0).
	- port_idx              : egress port, if use_regular==1, else ignore
	- connected_scheduler_index
				: if use_regular==0
				Scheduler index (range 0..127),
				Return an error code
				(QOS_HAL_STATUS_VALUE_RANGE_ERR),
				else continue with priority_weight
	- priority_weight       : Priority/Weight value is used to determine both
				the leaf of the connected_scheduler_index and
				the weight of that leaf, to be set in IWGT.
				The scheduler_leaf  will then be treated as a
				hidden resource from the pool of 8 inputs per
				scheduler. SW shall keep track of already
				assigned inputs, taking into consideration that
				each input will receive a weight (0 in case of
				strict priority) and the leaf number indicates
				the priority in case of strict priority.

	- scheduler_policy      : returns the scheduler's policy as
				QOS_HAL_POLICY_NULL: if nothing is connected to the
				scheduler's input
				QOS_HAL_POLICY_WSP : if a single
				queue or a single scheduler is connected to the
				scheduler's input
				QOS_HAL_POLICY_WFQ : if more than one
				queue/scheduler are connected to the scheduler's
				input
*/
enum qos_hal_errorcode
qos_hal_scheduler_create(const struct qos_hal_sched_create *param)
{
	enum qos_hal_errorcode ret = QOS_HAL_STATUS_OK;
	uint32_t omid;
	uint8_t  v;
	uint8_t leaf = 0;
	uint16_t weight;
	uint8_t idx = (uint8_t)param->connected_scheduler_index;
	uint32_t sch_id, p_sch_id;
	struct dp_node_link node_info = {0};
	struct dp_node_prio prio_info = {0};

	omid   = param->port_idx;
	v      = 0;
	weight = 0;
	leaf = param->priority_weight;

	node_info.inst = g_Inst;
	node_info.dp_port = g_Dp_Port;

	node_info.p_node_type = DP_NODE_PORT;
	node_info.p_node_id.cqm_deq_port = param->port_idx;
	node_info.cqm_deq_port.cqm_deq_port = param->port_idx;
	node_info.arbi = param->scheduler_policy;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "Level [%d] index [%d] arbitration [%d]\n",
				param->level, param->index, node_info.arbi);
	QOS_HAL_ASSERT(param->index >= QOS_HAL_MAX_SCHEDULER);
	QOS_HAL_ASSERT(param->level > QOS_HAL_MAX_SCHEDULER_LEVEL);

	if (qos_hal_sched_track[param->index].in_use)
		return QOS_HAL_STATUS_NOT_AVAILABLE;
	QOS_HAL_ASSERT(idx >= QOS_HAL_MAX_SCHEDULER);

	if (param->use_regular == 0) {
		qos_hal_get_schedid_from_sched_index(idx, &p_sch_id);
		node_info.p_node_id.cqm_deq_port = qos_hal_sched_track[idx].omid;
		node_info.cqm_deq_port.cqm_deq_port = qos_hal_sched_track[idx].omid;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected Scheduler [%d] -> Policy [%d]\n",
				__func__, idx, qos_hal_sched_track[idx].policy);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Logical Scheduler Node Id [%d]\n", __func__, p_sch_id);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected Scheduler OMID [%d]\n",
				__func__, node_info.p_node_id.cqm_deq_port);
		/* hierarchical scheduling */
		if (qos_hal_sched_track[idx].in_use == 0)
			return QOS_HAL_STATUS_NOT_AVAILABLE;

		switch (param->scheduler_policy) {
		case QOS_HAL_POLICY_NULL:
			/* connected scheduler is a head */
			return QOS_HAL_STATUS_NOT_AVAILABLE;
		case QOS_HAL_POLICY_WSP:
			QOS_HAL_ASSERT(param->priority_weight > 7);

			leaf = (uint8_t)param->priority_weight;
			weight = 0;
			break;
		case QOS_HAL_POLICY_WFQ:
		case QOS_HAL_POLICY_WRR:
			QOS_HAL_ASSERT(param->priority_weight > 1023);

			if (qos_hal_scheduler_free_input_get(
					param->connected_scheduler_index,
					&leaf) < 0)
				return QOS_HAL_STATUS_NOT_AVAILABLE;
			weight = param->priority_weight;
			break;
		default:
			QOS_HAL_ASSERT(1);
		}

		omid = (uint32_t) (
				(param->connected_scheduler_index << 3) |
				(leaf & 0x7));
		v = 1;
		node_info.p_node_type = DP_NODE_SCH;
		node_info.p_node_id.sch_id = p_sch_id;
		node_info.arbi = qos_hal_sched_track[idx].policy;
		node_info.prio_wfq = leaf;
	} else {
		ret = qos_hal_port_free_input_get(
					param->port_idx,
					&leaf);

		if (ret < 0) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Free Input\n",
					__func__, __LINE__);
			return QOS_HAL_STATUS_NOT_AVAILABLE;
		}
		node_info.prio_wfq = leaf;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n <%s> node_info.prio_wfq:0x%x\n", __func__, node_info.prio_wfq);
	}
	qos_hal_get_schedid_from_sched_index(param->index, &sch_id);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n =================\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SBID: %d\n", param->index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "NODE: %d\n", sch_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "LVL: %d\n", param->level);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "OMID: %d\n", omid);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Policy: %d\n", param->scheduler_policy);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "v: %d\n", v);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "weight: %d\n", weight);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n =================\n");

	/* mark scheduler as used*/
	qos_hal_sched_track[param->index].in_use = true;
	/* set scheduler id*/
	qos_hal_sched_track[param->index].id = sch_id;
	/* remember the policy */
	qos_hal_sched_track[param->index].policy = param->scheduler_policy;
	/* remember scheduler level*/
	qos_hal_sched_track[param->index].level = param->level;
	/* remember scheduler output connection type*/
	qos_hal_sched_track[param->index].sched_out_conn = v;
	/* remember scheduler output connection */
	qos_hal_sched_track[param->index].omid = omid;
	/* remember prio/weight to the parent node*/
	qos_hal_sched_track[param->index].prio_weight = leaf;
	qos_hal_sched_track[param->index].priority_selector = 0;

	qos_hal_sched_track[param->index].next_sched_id = INVALID_SCHED_ID;
	qos_hal_sched_track[param->index].peer_sched_id = INVALID_SCHED_ID;

	/* update the leaf mask of the connected scheduler  */
	if (param->use_regular == 0)
		qos_hal_sched_track[param->connected_scheduler_index].leaf_mask |= (1 << leaf) ;
	else
		qos_hal_port_track[param->port_idx].leaf_mask |= (1 << leaf);

	node_info.node_type = DP_NODE_SCH;
	node_info.node_id.sch_id = sch_id;

	/** Link the scheduler to its parent node */
	if (dp_node_link_add(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:%d Failed to add the node link\n", __func__, __LINE__);
		return QOS_HAL_STATUS_NOT_AVAILABLE;
	}

	prio_info.inst = g_Inst;
	prio_info.id.sch_id = sch_id;
	prio_info.type = DP_NODE_SCH;

	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node %d\n", __func__, __LINE__, sch_id);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler:[%d] has priority:[%d] arbi:[%d]\n", __func__, sch_id, prio_info.prio_wfq, prio_info.arbi);
#if 0
	prio_info.prio_wfq = param->priority_weight;
	if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node [%d]\n", __func__, __LINE__, sch_id);
	}
#endif
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Exit -------------> \n");
	return ret;
}

/** The qos_hal_scheduler_delete function is used to remove a scheduler from the
    scheduler hierarchy.
	- update the leaf mask of the omid of the scheduler
	- disable scheduler output
	- disable scheduler input
	- update the scheduler tracking information
*/
enum qos_hal_errorcode
qos_hal_scheduler_delete(uint32_t index)
{
	uint32_t leaf, connected_scheduler_index;
	uint32_t sch_id;
	uint32_t omid;
	struct dp_node_link node_info = {0};
	struct dp_node_alloc del_node = {0};

	QOS_HAL_ASSERT(index >= QOS_HAL_MAX_SCHEDULER);

	if (!qos_hal_sched_track[index].in_use) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler is not in use %d \n", __func__, index);
		return QOS_HAL_STATUS_NOT_AVAILABLE;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Scheduler %d \n", __func__, index);
	/* Update the leaf_mask of OMID if it is a Scheduler */
	if (qos_hal_sched_track[index].sched_out_conn == 1) {
		leaf = qos_hal_sched_track[index].omid & 7;
		connected_scheduler_index = qos_hal_sched_track[index].omid >> 3;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected Scheduler index %d leaf %d\n",
					__func__, connected_scheduler_index, leaf);
		qos_hal_sched_track[connected_scheduler_index].leaf_mask &=  ~(1<<leaf);
	} else {
		leaf = qos_hal_sched_track[index].prio_weight;
		omid = qos_hal_sched_track[index].omid;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected Port index %d leaf %d\n",
					__func__, omid, leaf);
		qos_hal_port_track[omid].leaf_mask &=  ~(1<<leaf);
		qos_hal_port_track[omid].port_sched_id = 0xFF;
	}

	qos_hal_get_schedid_from_sched_index(index, &sch_id);
	node_info.node_type = DP_NODE_SCH;
	node_info.node_id.sch_id = sch_id;
	if (dp_node_unlink(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> <%d> Failed to unlink the node %d\n",
						__func__, __LINE__, sch_id);
		return QOS_HAL_STATUS_ERR;
	}

	del_node.type = DP_NODE_SCH;
	del_node.id.sch_id = sch_id;
	if (dp_node_free(&del_node, DP_NODE_AUTO_FREE_RES) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> <%d> Failed to free the node %d\n",
					__func__, __LINE__, sch_id);
		return QOS_HAL_STATUS_ERR;
	}

	/* mark scheduler as unused */
	qos_hal_sched_track[index].in_use = false;
	/* reset scheduler output connection type*/
	qos_hal_sched_track[index].sched_out_conn = 0;
	/* reset scheduler output connection */
	qos_hal_sched_track[index].omid = 0xFF;
	/* reset the policy */
	qos_hal_sched_track[index].policy = QOS_HAL_POLICY_NULL;
	/* reset the priority selecteor */
	qos_hal_sched_track[index].priority_selector = 0;
	qos_hal_sched_track[index].id = 0xFF;
	qos_hal_sched_track[index].next_sched_id = 0xFF;
	qos_hal_sched_track[index].peer_sched_id = 0xFF;
	/* reset the leaf mask */
	qos_hal_sched_track[index].leaf_mask = 0;

	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_sched_blk_in_enable(uint32_t sbin, uint32_t enable)
{
	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_sched_blk_out_enable(uint32_t sbid, uint32_t enable)
{
	uint32_t sch_id;
	struct dp_node_link_enable node_en = {0};

	qos_hal_get_schedid_from_sched_index(sbid, &sch_id);

	node_en.type = DP_NODE_SCH;
	node_en.id.sch_id = sch_id;
	node_en.en = enable;
	/** Enable/Disable the scheduler node*/
	 if (dp_node_link_en_set(&node_en, enable) == DP_FAILURE)
		return -1;

	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_scheduler_in_enable_disable function is used to enable or disable
    any scheduler input.
*/
enum qos_hal_errorcode
qos_hal_scheduler_in_enable_disable(uint32_t sbin, uint32_t enable)
{

	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_sched_prio_get(
			uint32_t sch_in,
			uint32_t *prio)
{
	struct dp_node_prio prio_info = {0};
	prio_info.inst = g_Inst;
	prio_info.id.sch_id = sch_in;
	prio_info.type = DP_NODE_SCH;
	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node %d\n",
					__func__, __LINE__, sch_in);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sched [%d] --> priority: [%d]\n",
			__func__, sch_in, prio_info.prio_wfq);
	*prio = prio_info.prio_wfq;

	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_queue_prio_get(
			uint32_t q_in,
			uint32_t *prio)
{
	struct dp_node_prio prio_info = {0};
	prio_info.inst = g_Inst;
	prio_info.id.q_id = q_in;
	prio_info.type = DP_NODE_QUEUE;
	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node %d\n",
				__func__, __LINE__, q_in);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue [%d] --> priority: [%d]\n", __func__, q_in, prio_info.prio_wfq);
	*prio = prio_info.prio_wfq;

	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_get_node_id_from_prio(
			uint32_t prio,
			uint32_t type,
			uint32_t port,
			uint32_t sched,
			int32_t *nodeId)
{
	int32_t i = 0;
	uint32_t node_id;
	struct dp_node_prio prio_info = {0};
	struct dp_node_child node_info = {0};
	struct dp_node child_node = {0};

	node_info.type = type;
	if ((type == DP_NODE_SCH) && (sched < QOS_HAL_MAX_SCHEDULER)) {
		node_info.id.sch_id = qos_hal_sched_track[sched].id;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sched Node:[%d]\n",
				__func__, node_info.id.sch_id);
	} else {
		node_info.id.cqm_deq_port = port;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Node:[%d]\n",
				__func__, node_info.id.cqm_deq_port);
	}

	if (dp_children_get(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Fail to get the Children\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No of children: [%d]\n",
			__func__, node_info.num);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Find the Node id for Prio:[%d]\n",
			__func__, prio);

	for (i = 0; i < 8; i++) {
		child_node = node_info.child[i];
		if (child_node.type == DP_NODE_QUEUE) {
			node_id = child_node.id.q_id;

			prio_info.inst = g_Inst;
			prio_info.id.q_id = node_id;
			prio_info.type = DP_NODE_QUEUE;
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> %d Fail to get Prio of the node [%d]\n",
						__func__, __LINE__, node_id);
				return QOS_HAL_STATUS_ERR;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> DP child index:[%d] Q:[%d]->prio:[%d]\n",
						__func__, i, node_id, prio_info.prio_wfq);
			if (prio == prio_info.prio_wfq) {
				*nodeId = node_id;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Matched Node id for prio:[%d]->[%d]\n",
						__func__, prio, node_id);
			}

		} else if (child_node.type == DP_NODE_SCH) {
			node_id = child_node.id.sch_id;

			prio_info.inst = g_Inst;
			prio_info.id.sch_id = node_id;
			prio_info.type = DP_NODE_SCH;
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> %d Fail to get Prio of the node:[%d]\n",
						__func__, __LINE__, node_id);
				return QOS_HAL_STATUS_ERR;
			}
			if (prio == prio_info.prio_wfq) {
				*nodeId = child_node.id.sch_id;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Matched Node id for prio:[%d]->[%d]\n",
						__func__, prio, node_id);
			}
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> DP child index [%d] [No Child]\n",
					__func__, i);
		}
	}

	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_scheduler_in_status_get(
		uint32_t sb_input,
		uint32_t *sie,
		uint32_t *sit,
		uint32_t *qsid)
{
	uint32_t sbid, leaf;
	uint32_t node_id;
	int32_t i = 0;
	int32_t ret = QOS_HAL_STATUS_ERR;
	struct dp_node_prio prio_info = {0};
	struct dp_node_child node_info = {0};
	struct dp_node child_node = {0};
	struct dp_node_link_enable en = {0};

	/* Get the input details of scheduler leaf */
	sbid = (sb_input >> 3);
	leaf = (sb_input & 7);

	QOS_HAL_ASSERT(sbid >= QOS_HAL_MAX_SCHEDULER);

	node_info.type = DP_NODE_SCH;
	node_info.id.sch_id = qos_hal_sched_track[sbid].id;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Get the children for scheduler [%d/%d]\n",
				__func__, sbid, node_info.id.sch_id);
	if (dp_children_get(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> dp_children_get failed.\n", __func__);
		return QOS_HAL_STATUS_ERR;
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> dp_children_get passed.\n", __func__);
	}
	for (i = 0; i < 8; i++) {
		child_node = node_info.child[i];
		if (child_node.type == DP_NODE_QUEUE) {
			node_id = child_node.id.q_id;

			prio_info.inst = g_Inst;
			prio_info.id.q_id = node_id;
			prio_info.type = DP_NODE_QUEUE;
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> %d Failed to get Prio of the node [%d]\n", __func__, __LINE__, node_id);
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> DP child index [%d] Queue [%d] --> priority: [%d]\n", __func__, i, node_id, prio_info.prio_wfq);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP child index [%d] Queue [%d] --> priority: [%d]\n", __func__, i, node_id, prio_info.prio_wfq);
			if (leaf == prio_info.prio_wfq) {
				*qsid = node_id;
				*sit = 0;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> [Queue] Matched Node id for leaf: [%d] --> [%d]\n", __func__, leaf, node_id);

				en.inst = g_Inst;
				en.type = DP_NODE_QUEUE;
				en.id.q_id = node_id;
				dp_node_link_en_get(&en, 0);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SBIN: [%d] --> [%s]\n", __func__, sb_input, en.en ? "enabled" : "disabled");

				*sie = en.en;
				ret = QOS_HAL_STATUS_OK;
				break;
			}

		} else if (child_node.type == DP_NODE_SCH) {
			node_id = child_node.id.sch_id;

			prio_info.inst = g_Inst;
			prio_info.id.sch_id = node_id;
			prio_info.type = DP_NODE_SCH;
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> %d Failed to get Prio of the node [%d]\n", __func__, __LINE__, node_id);
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP child index [%d] Sched [%d] --> priority: [%d]\n", __func__, i, node_id, prio_info.prio_wfq);
			if (leaf == prio_info.prio_wfq) {
				*qsid = node_id;
				*sit = 1;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> [Sched] Matched Node id for leaf: [%d] --> [%d]\n", __func__, leaf, node_id);

				en.inst = g_Inst;
				en.type = DP_NODE_SCH;
				en.id.sch_id = node_id;
				dp_node_link_en_get(&en, 0);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SBIN: [%d] --> [%s]\n", __func__, sb_input, en.en ? "enabled" : "disabled");

				*sie = en.en;
				ret = QOS_HAL_STATUS_OK;
				break;
			}
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP child index [%d] [No Child]\n", __func__, i);
			ret = QOS_HAL_STATUS_NOT_AVAILABLE;
		}
	}

	return ret;
}

enum qos_hal_errorcode
qos_hal_node_in_status_get(
		uint32_t node_id,
		uint32_t type,
		uint32_t *sie,
		uint32_t *sit,
		uint32_t *qsid)
{
	uint32_t node, leaf;
	uint32_t nodeId;
	int32_t i = 0;
	int32_t ret = QOS_HAL_STATUS_ERR;
	struct dp_node_prio prio_info = {0};
	struct dp_node_child node_info = {0};
	struct dp_node child_node = {0};
	struct dp_node_link_enable en = {0};

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Node Id: %d type:%d\n",
			__func__, node_id, type);
	/* Get the input details of node leaf */
	node = node_id >> 3;
	leaf = node_id & 0x7;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Node: %d leaf:%d\n",
			__func__, node, leaf);

	node_info.type = type;
	if (type == DP_NODE_SCH)
		node_info.id.sch_id = qos_hal_sched_track[node].id;
	else
		node_info.id.cqm_deq_port = node;

	if (dp_children_get(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Fail to get children\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Node: %d has %d child\n",
			__func__, node, node_info.num);

	if (child_node.type == DP_NODE_QUEUE)
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>leaf: %d has child id: %d type:%d\n", __func__, leaf, child_node.id.q_id, child_node.type);
	else if (child_node.type == DP_NODE_SCH)
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>leaf: %d has child id: %d type:%d\n", __func__, leaf, child_node.id.sch_id, child_node.type);

	for (i = 0; i < 8; i++) {
		child_node = node_info.child[i];
		if (child_node.type == DP_NODE_QUEUE) {
			nodeId = child_node.id.q_id;

			prio_info.inst = g_Inst;
			prio_info.id.q_id = nodeId;
			prio_info.type = DP_NODE_QUEUE;
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n",
						__func__, __LINE__, nodeId);
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> DP child index [%d] Queue [%d] --> priority: [%d]\n",
					__func__, i, nodeId, prio_info.prio_wfq);
			if (leaf == prio_info.prio_wfq) {
				*qsid = nodeId;
				*sit = 0;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Matched Node id for leaf: [%d] --> [%d]\n",
						__func__, leaf, nodeId);

				en.inst = g_Inst;
				en.type = DP_NODE_QUEUE;
				en.id.q_id = nodeId;
				dp_node_link_en_get(&en, 0);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SBIN: [%d] --> [%s]\n",
						__func__, nodeId, en.en ? "enabled" : "disabled");

				*sie = en.en;
				ret = QOS_HAL_STATUS_OK;
				break;
			}

		} else if (child_node.type == DP_NODE_SCH) {
			nodeId = child_node.id.sch_id;

			prio_info.inst = g_Inst;
			prio_info.id.sch_id = nodeId;
			prio_info.type = DP_NODE_SCH;
			if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n",
						__func__, __LINE__, nodeId);
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP child index [%d] Scheduler [%d] --> priority: [%d]\n", __func__, i, nodeId, prio_info.prio_wfq);
			if (leaf == prio_info.prio_wfq) {
				*qsid = nodeId;
				*sit = 1;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Matched Node id for leaf: [%d] --> SB [%d]\n", __func__, leaf, nodeId);

				en.inst = g_Inst;
				en.type = DP_NODE_SCH;
				en.id.sch_id = nodeId;
				dp_node_link_en_get(&en, 0);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SBIN: [%d] --> [%s]\n",
						__func__, node_id, en.en ? "enabled" : "disabled");

				*sie = en.en;
				ret = QOS_HAL_STATUS_OK;
				break;
			}
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> DP child index [%d] [No Child]\n", __func__, i);
			ret = QOS_HAL_STATUS_NOT_AVAILABLE;
		}
	}

	return ret;
}

/** The qos_hal_update_level_recursive function is used to change the scheduler
    level recursively to the last level of connected scheduler.
    It is called from qos_hal_sched_out_remap function.
*/
int32_t qos_hal_update_level_recursive(uint32_t sbid, uint32_t sbid_start, uint32_t curr_lvl, uint32_t start_level)
{
	int32_t i, k = sbid << 3, level, new_lvl;
	uint32_t sie, sit, qsid;
	struct dp_node_child node_info = {0};
	struct dp_node child_node = {0};

	level = curr_lvl;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Current level for sbid %u is %u\n",
			__func__, sbid, curr_lvl);

	/*qos_hal_get_schedid_from_sched_index(index, &sch_id);*/
	node_info.type = DP_NODE_SCH;
	node_info.id.sch_id = qos_hal_sched_track[sbid].id;
	if (dp_children_get(&node_info, 0) == DP_FAILURE)
		return -1;

	for (i = 0; i < node_info.num; i++, k++) {
		child_node = node_info.child[i];
		if (sbid == sbid_start)
			level = start_level;
		qos_hal_scheduler_in_status_get(k, &sie, &sit, &qsid); /*traversal each leaf based on current sbid */

		if (sie == 0)  /*If scheduler Input is disabled, then check next sbin,ie, k+1 */
			continue;

		if (sit == 1) { /* means the input is SBID, not QID, then continue search its parent SB's queue */
			new_lvl = level+1;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Setting the level of QSID %u to %u\n", __func__, qsid, new_lvl);
			/** Remeber the level of the scheduler */
			qos_hal_sched_track[qsid].level = new_lvl;
			qos_hal_update_level_recursive(qsid, sbid_start, new_lvl, start_level); /*iterate all its parent sbid and update */
			continue;
		}
	}
	return QOS_HAL_STATUS_OK;
}


/** The qos_hal_sched_out_remap function is used to map the scheduler to
    the input of any other scheduler and recursively updated the level of
    all the higher level scheduler connected to it in hierarchy.
*/
enum qos_hal_errorcode
qos_hal_sched_out_remap(const uint32_t sbid, const uint8_t lvl,
			const uint32_t omid, const uint8_t v,
			const uint16_t weight)
{
	struct dp_node_link node_info = {0};
	uint32_t sch_id;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, " Remap SBID %u --> OMID %u\n ", sbid, omid);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t LVL: %u   Conn Type(v): %d  weight:  %d\n", lvl, v, weight);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n=================\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, " Modified SBIN 255->0 : Remap SBID %u --> OMID %u\n ", sbid, omid);
	/* Klocwork fix */
	if ((omid>>3) > QOS_HAL_MAX_SCHEDULER-1)
		return -EINVAL;

	/* Fill the parent node info */
	if (v == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port Link Set: Omid:%d   sbid: %d  \n", __func__, omid, sbid);
		node_info.p_node_type = DP_NODE_PORT;
		node_info.p_node_id.cqm_deq_port = omid;
		node_info.cqm_deq_port.cqm_deq_port = omid;
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SB Link Set: Omid:%d   sbid: %d  \n", __func__, omid, sbid);
		qos_hal_get_schedid_from_sched_index(omid >> 3, &sch_id);
		node_info.p_node_type = DP_NODE_SCH;
		node_info.p_node_id.sch_id = sch_id;
	}

	node_info.dp_port = g_Dp_Port;
	node_info.node_type = DP_NODE_SCH;
	node_info.node_id.sch_id = qos_hal_sched_track[sbid].id;
	node_info.prio_wfq = omid & 7;

	node_info.arbi = qos_hal_sched_track[sbid].policy;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> node_info.arbi: %d  \n", __func__, node_info.arbi);
	
	if (dp_node_link_add(&node_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:%d Failed to link scheduler node %d\n", __func__, __LINE__, node_info.node_id.sch_id);
		return QOS_HAL_STATUS_ERR;
	}
	if (v == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port Link Set: Omid:%d   sbid: %d  \n", __func__, omid, sbid);
		qos_hal_sched_track[sbid].prio_weight = weight;

	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SB Link Set: Omid:%d   sbid: %d  \n", __func__, omid, sbid);
		/** Remeber the next_sched_id of the omid */
		qos_hal_sched_track[omid >> 3].next_sched_id = sbid;
		qos_hal_sched_track[omid >> 3].leaf_mask |= (1 << (omid & 7)) ;
		qos_hal_sched_track[sbid].prio_weight = omid & 0x7;
	}

	/** Remeber the level of the scheduler */
	qos_hal_sched_track[sbid].level = lvl;
	/** Remeber the scheduler out connection of the scheduler */
	qos_hal_sched_track[sbid].sched_out_conn = v;
	/** Remeber the omid of the scheduler */
	qos_hal_sched_track[sbid].omid = omid ;

	/** Update the level of all the higher level schedulers */
	qos_hal_update_level_recursive(sbid, sbid, lvl, lvl);

	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_token_bucket_shaper_create function is used to attach a Token
    Bucket Shaper (TBS) to a scheduler input and enable it.
*/
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_create(uint32_t index, uint32_t tbs_scheduler_block_input)
{
	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_token_bucket_shaper_delete function is used to remove a Token
    Bucket Shaper (TBS) from a scheduler input and disable it.
*/
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_delete(uint32_t index, uint32_t tbs_scheduler_block_input)
{
	struct dp_shaper_conf shaper_conf = {0};

	QOS_HAL_ASSERT(index >= QOS_HAL_MAX_SHAPER);
	/*QOS_HAL_ASSERT(tbs_scheduler_block_input >=
			SCHEDULER_BLOCK_INPUT_ID_MAX);*/

	shaper_conf.cmd = DP_SHAPER_CMD_REMOVE;
	shaper_conf.type = qos_hal_shaper_track[index].attach_to;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Shaper index:[%d] type:[%d]\n",
			__func__,  index, shaper_conf.type);
	if (shaper_conf.type == DP_NODE_SCH) {
		qos_hal_get_schedid_from_sched_index(qos_hal_shaper_track[index].attach_to_id, &shaper_conf.id.sch_id);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Shaper attach SB id:[%d:%d]\n",
				__func__, qos_hal_shaper_track[index].attach_to_id, shaper_conf.id.sch_id);
	} else if (shaper_conf.type == DP_NODE_QUEUE) {
		qos_hal_get_qid_from_queue_index(
				qos_hal_shaper_track[index].attach_to_id,
				&shaper_conf.id.q_id);
	} else
		return QOS_HAL_STATUS_ERR;

	if (dp_shaper_conf_set(&shaper_conf, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Fail to set Shaper\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	return QOS_HAL_STATUS_OK;
}


/** The qos_hal_token_bucket_shaper_get function is used to read back the assignment
    of a Token Bucket Shaper (TBS) to a scheduler input.
*/
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_get(uint32_t index, uint32_t *tbs_scheduler_block_input)
{
	QOS_HAL_ASSERT(index >= QOS_HAL_MAX_SHAPER);

	*tbs_scheduler_block_input = qos_hal_shaper_track[index].attach_to_id;

	return QOS_HAL_STATUS_OK;
}


/** The qos_hal_token_bucket_shaper_cfg_set function is used to configure a Token
    Bucket Scheduler (TBS) for CIR/CBS or PIR/PBS, respectively. If only a
    single shaper shall be configured, both indexes are set to the same value.
*/
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_cfg_set(const struct qos_hal_token_bucket_shaper_cfg *param)
{
	struct dp_shaper_conf shaper_conf = {0};

	QOS_HAL_ASSERT(param->index >= QOS_HAL_MAX_SHAPER);

	shaper_conf.cmd = DP_SHAPER_CMD_ADD;
	shaper_conf.pir  = param->pir;
	shaper_conf.pbs  = param->pbs;
	shaper_conf.cir  = param->cir;
	shaper_conf.cbs  = param->cbs;

	shaper_conf.type = param->attach_to;
	if (shaper_conf.type == DP_NODE_SCH) {
		shaper_conf.id.sch_id = qos_hal_sched_track[param->attach_to_id].id;
	} else if (shaper_conf.type == DP_NODE_QUEUE) {
		shaper_conf.id.q_id = qos_hal_queue_track[param->attach_to_id].queue_id;
	} else if (shaper_conf.type == DP_NODE_PORT) {
		shaper_conf.id.cqm_deq_port = param->attach_to_id;
	} else
		return QOS_HAL_STATUS_ERR;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>PIR=%d, PBS=%d, CIR=%d, CBS=%d, for TBID %u param_attach_to= %d param_attach_to_id = %d \n",
		__func__, shaper_conf.pir, shaper_conf.pbs, shaper_conf.cir, shaper_conf.cbs, param->index, param->attach_to, param->attach_to_id);

	if (dp_shaper_conf_set(&shaper_conf, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to set the Shaper\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	return QOS_HAL_STATUS_OK;
}

/** The qos_hal_token_bucket_shaper_cfg_get function is used to read back the
    configuration of a Token Bucket Shaper (TBS).
*/
enum qos_hal_errorcode
qos_hal_token_bucket_shaper_cfg_get(uint32_t index,
		struct qos_hal_token_bucket_shaper_cfg *out)
{
	struct dp_shaper_conf shaper_conf = {0};

	QOS_HAL_ASSERT(index >= QOS_HAL_MAX_SHAPER);

	if (dp_shaper_conf_get(&shaper_conf, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to set the Shaper\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	out->index  = index;
	out->pir    = shaper_conf.pir;
	out->pbs    = shaper_conf.pbs;
	out->cir    = shaper_conf.cir;
	out->cbs    = shaper_conf.cbs;

	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_token_bucket_shaper_assign_set_to_invalid(uint32_t shaper_index, uint32_t sbin)
{
	return QOS_HAL_STATUS_OK;
}

/** ================================ HIGH LEVEL API,s ===============================  */

int32_t
create_new_scheduler(
		int32_t omid,
		int32_t input_type,
		int32_t policy,
		int32_t level,
		int32_t weight)
{
	uint32_t sched_id;
	struct qos_hal_sched_create sched_new;

	sched_id = qos_hal_get_free_scheduler();
	if (sched_id == INVALID_SCHED_ID) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "Invalid Sched Id= [%d]\n",
				sched_id);
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Link Sched [%d] to OMID [%d]\n",
				sched_id, omid);
	memset (&sched_new, 0, sizeof(struct qos_hal_sched_create));
	sched_new.index = sched_id;
	sched_new.level = level;

	if (input_type == 0)
		sched_new.connected_scheduler_index = omid;
	else
		sched_new.port_idx = omid;

	sched_new.use_regular = input_type; /* port_idx will be selected */
	sched_new.scheduler_policy = policy;
	sched_new.priority_weight = weight;

	if (QOS_HAL_STATUS_NOT_AVAILABLE == qos_hal_scheduler_create(&sched_new)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "Fail to create Sched Block\n");
		return QOS_HAL_STATUS_ERR;
	}
	return sched_id;
}

int qos_hal_free_prio_scheduler_get(
		uint32_t parent_sched,
		uint32_t base_sched,
		uint32_t priority,
		uint32_t *free_prio_sched)
{
	uint32_t omid = 0, i, j, k = 0, peer_id;
	struct qos_hal_sched_track_info Snext;
	Snext = qos_hal_sched_track[base_sched];
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Parent Sched:%d base_sched:%d\n",
				__func__, parent_sched, base_sched);

	if (priority > QOS_HAL_MAX_PRIORITY_LEVEL) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Priority can not > Max Prio Limit\n",
					__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	i = QOS_HAL_MAX_PRIORITY_LEVEL/QOS_HAL_Q_PRIO_LEVEL;
	j = priority/QOS_HAL_Q_PRIO_LEVEL;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "j = %d \n", j);
	do {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Snext.id = %d Snext.peer_sched.id = %d\n",
					Snext.id, Snext.peer_sched_id);
		if (j == k) {
			*free_prio_sched = base_sched;
			return QOS_HAL_STATUS_OK;
		} else {
			omid = Snext.peer_sched_id;
			Snext = qos_hal_sched_track[Snext.peer_sched_id];
			peer_id = Snext.peer_sched_id;
			k++;
		}
	} while (peer_id != 0xFF);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "j = %d k = %d\n", j, k);
	if (k < j)
		omid = create_new_scheduler(
				parent_sched,
				0,
				QOS_HAL_POLICY_WFQ,
				Snext.level,
				0);

	*free_prio_sched = omid;
	return QOS_HAL_STATUS_OK;
}

int qos_hal_add_scheduler_level(
			uint32_t port,
			uint32_t base_sched,
			uint32_t priority_weight,
			uint32_t *new_base_sched,
			uint32_t sched_type,
			uint32_t *new_omid)
{
	uint32_t omid = 0, omid1 = 0, old_omid, base_sched_new;
	struct qos_hal_sched_track_info Snext;

	if (base_sched < QOS_HAL_MAX_SCHEDULER) {
		Snext = qos_hal_sched_track[base_sched];
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler: %d\n", __func__, Snext.id);
	} else {

		base_sched_new = create_new_scheduler(port, 1, QOS_HAL_POLICY_WSP, 0, 0);
		if (base_sched_new == QOS_HAL_STATUS_ERR)
			return QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Base Scheduler: %d \n", __func__, base_sched_new);
		Snext = qos_hal_sched_track[base_sched_new];
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Create the Priority Selector level \n", __func__);
	old_omid = Snext.omid;
	if (Snext.sched_out_conn == 1) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> old_omid:%d qos_hal_sched_track[old_omid].leaf_mask:0x%x \n", __func__, old_omid, qos_hal_sched_track[old_omid].leaf_mask);
		omid = create_new_scheduler(old_omid >> 3, 0, sched_type, Snext.level, old_omid & 7);
		if (omid == QOS_HAL_STATUS_ERR)
			return QOS_HAL_STATUS_ERR;
		qos_hal_sched_track[old_omid >> 3].next_sched_id = omid;
		qos_hal_sched_track[omid].next_sched_id = Snext.id;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Configuring  Scheduler id of SB %d -->SB %d -->SB %d \n", __func__, Snext.id, omid, Snext.omid);
	} else if (Snext.sched_out_conn == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> old_omid:%d qos_hal_port_track[old_omid].leaf_mask:0x%x \n", __func__, old_omid, qos_hal_port_track[old_omid].leaf_mask);
		omid = create_new_scheduler(old_omid, 1, sched_type, Snext.level, 0);
		if (omid == QOS_HAL_STATUS_ERR)
			return QOS_HAL_STATUS_ERR;
		qos_hal_port_track[port].input_sched_id = omid;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Configuring  Scheduler id of SB %d -->SB %d -->Port %d \n", __func__, Snext.id, omid, Snext.omid);
	}

	qos_hal_sched_track[omid].policy = Snext.policy;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> qos_hal_sched_track[omid].policy: %d  \n", __func__, qos_hal_sched_track[omid].policy);

	if ((Snext.policy == QOS_HAL_POLICY_WRR) && (sched_type == QOS_HAL_POLICY_WRR)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> [WRR/WRR Case] Base Scheduler Policy:[%d] New Queue Policy:[%d] \n",
				__func__, Snext.policy, sched_type);
		qos_hal_sched_out_remap(base_sched, Snext.level+1, omid<<3, 1, 1);
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler Policy:[%d] New Queue Policy:[%d]\n",
				__func__, Snext.policy, sched_type);
		qos_hal_sched_out_remap(base_sched, Snext.level+1, omid<<3, 1, 0);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>  qos_hal_sched_track[old_omid].leaf_mask:0x%x \n", __func__, qos_hal_sched_track[old_omid].leaf_mask);
	qos_hal_dump_sb_info(base_sched);

	/*create a new scheduler and connect it to the second highest priority leaf.
	this scheduler input will be treated as priority 8-15 for that port.*/
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Create the Priority Selector Scheduler 8-15 \n", __func__);
	omid1 = create_new_scheduler(omid, 0, sched_type, Snext.level+1, 1);
	if (omid1 == QOS_HAL_STATUS_ERR)
		return QOS_HAL_STATUS_ERR;
	qos_hal_sched_track[omid1].policy = Snext.policy;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Priority Selector Scheduler 8-15 is : %d \n", __func__, omid1);
	qos_hal_sched_track[base_sched].peer_sched_id = omid1;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Configuring  Peer Scheduler config of SB %d -->SB %d \n",
						__func__, Snext.id, qos_hal_sched_track[base_sched].peer_sched_id);

	qos_hal_dump_sb_info(omid);
	qos_hal_dump_sb_info(base_sched);
	qos_hal_dump_sb_info(omid1);
	*new_base_sched = omid;
	*new_omid = omid1;
	return QOS_HAL_STATUS_OK;

}

int qos_hal_del_scheduler_level(
			struct net_device *netdev,
			uint32_t port,
			uint32_t base_sched,
			uint32_t level_sched,
			char *dev_name,
			uint32_t flags)
{
	uint8_t leaf;
	uint32_t old_omid, sched_del, new_base_sched, old_omid_prio;
	struct qos_hal_sched_track_info Snext = {0};
	struct dp_node_child node_child = {0};

	if (base_sched < QOS_HAL_MAX_SCHEDULER) {
		Snext = qos_hal_sched_track[base_sched];
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler: [%d]\n", __func__, Snext.id);
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete the Priority Selector level for port %d \n", __func__, port);
	old_omid = Snext.omid;
	old_omid_prio = qos_hal_sched_track[base_sched].prio_weight;
	if (qos_hal_sched_track[level_sched].priority_selector == 2) {
		new_base_sched = qos_hal_sched_track[qos_hal_sched_track[base_sched].next_sched_id].peer_sched_id;
		sched_del = qos_hal_sched_track[base_sched].next_sched_id;
	} else if (qos_hal_sched_track[level_sched].priority_selector == 3) {
		sched_del = qos_hal_sched_track[qos_hal_sched_track[base_sched].next_sched_id].peer_sched_id;
		new_base_sched = qos_hal_sched_track[base_sched].next_sched_id;
	} else {
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Disable omid %d and then Delete the Level Scheduler %d\n", __func__, qos_hal_sched_track[sched_del].omid, sched_del);
	qos_hal_sched_blk_in_enable(qos_hal_sched_track[sched_del].omid, 0);
	qos_hal_scheduler_delete(sched_del);

	if (Snext.sched_out_conn == 1) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Disbale OMID %d \n", __func__, qos_hal_sched_track[base_sched].omid);
		qos_hal_sched_blk_in_enable(qos_hal_sched_track[base_sched].omid, 0);
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>New Base Scheduler --> [%d]\n", __func__, new_base_sched);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Before Remap New Base Scheduler --> [%d]\n", __func__, new_base_sched);
	qos_hal_dump_sb_info(new_base_sched);

	if (Snext.sched_out_conn == 1) {
		qos_hal_sched_out_remap(new_base_sched,
				(qos_hal_sched_track[new_base_sched].level) - 1,
				old_omid, 1, old_omid & 7);
		qos_hal_sched_track[old_omid >> 3].next_sched_id = new_base_sched;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap Scheduler id of SB %d -->SB %d\n", __func__, new_base_sched, old_omid);
	} else if (Snext.sched_out_conn == 0) {
		if (qos_hal_port_free_input_get(
					old_omid,
					&leaf) < 0)
			return QOS_HAL_STATUS_NOT_AVAILABLE;

		qos_hal_port_track[old_omid].leaf_mask |= (1 << leaf);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n <%s> node_info.prio_wfq:0x%x\n", __func__, leaf);

		qos_hal_sched_out_remap(new_base_sched,
				(qos_hal_sched_track[new_base_sched].level) - 1,
				old_omid, 0, leaf);
		qos_hal_port_track[port].input_sched_id = new_base_sched;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap Scheduler id of SB %d --> Port %d\n", __func__, new_base_sched, old_omid);
	}

	node_child.type = DP_NODE_SCH;
	node_child.id.sch_id = qos_hal_sched_track[new_base_sched].id;
	if (dp_children_get(&node_child, 0) == DP_FAILURE)
		return -1;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Number of Children for SB [%d/%d] --> [%d]\n",
		__func__, new_base_sched, qos_hal_sched_track[new_base_sched].id, node_child.num);

	qos_hal_sched_track[new_base_sched].priority_selector = 0;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete the base Scheduler --> [%d]\n", __func__, base_sched);
	qos_hal_scheduler_delete(base_sched);

	qos_hal_dump_sb_info(new_base_sched);
	qos_hal_dump_sb_info(base_sched);
	qos_hal_dump_sb_info(sched_del);
	return QOS_HAL_STATUS_OK;
}

int qos_hal_free_wfq_leaf_get(uint32_t base_sched, uint32_t *free_wfq_leaf)
{
	uint32_t ret = QOS_HAL_STATUS_OK;
	uint8_t leaf = 0;

	ret = qos_hal_scheduler_free_input_get((uint16_t)base_sched, &leaf);
	if (ret < 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Free Input\n",
			__func__, __LINE__);
		return QOS_HAL_STATUS_NOT_AVAILABLE;
	}
	*free_wfq_leaf = leaf;

	return ret;
}

int qos_hal_free_wfq_scheduler_get(uint32_t port, uint32_t base_sched, uint32_t *free_wfq_sched_input)
{
	uint32_t omid, parent, i;
	uint32_t sie = 0, sit = 0, qsid = 0;
	struct qos_hal_sched_track_info Snext;

	Snext = qos_hal_sched_track[base_sched];
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base_sched value: [%d]\n", __func__, base_sched);
	for (i = 0; i < 8; i++) {
		/* Get the input details of scheduler leaf */
		sie = 0; sit = 0; qsid = 0;
		qos_hal_scheduler_in_status_get(base_sched << 3 | i, &sie, &sit, &qsid);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> sie:[%d] sit:[%d] qsid:[%d]\n", __func__, sie, sit, qsid);
		if (sie != DP_NODE_EN) { /* no queue connected to this leaf */
			*free_wfq_sched_input = (base_sched << 3 | i);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Free WFQ Sched input = [%d]\n", __func__, *free_wfq_sched_input);
			return QOS_HAL_STATUS_OK;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Snext.id = [%d]  leaf = [%d]\n", Snext.id, i);
	}
	omid = qos_hal_sched_track[base_sched].omid;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Omid = [%d]\n",
		__func__, omid);
	if ((qos_hal_sched_track[omid >> 3].policy == QOS_HAL_POLICY_WFQ)
		|| (qos_hal_sched_track[omid >> 3].policy == QOS_HAL_POLICY_WRR)) {
		uint32_t sbin, j;
		uint32_t free_wfq_leaf;
		uint32_t sie1, sit1, qsid1;
		int32_t nodeid = -1;
		for (j = 0; j < 8; j++) {
			/* Get the input details of scheduler leaf */
			sbin = ((omid & ~7) | j);
			sie1 = 0; sit1 = 0; qsid1 = 0;
			qos_hal_scheduler_in_status_get(sbin, &sie1, &sit1, &qsid1);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> sie1:[%d] sit1:[%d] qsid1:[%d]\n",
				__func__, sie1, sit1, qsid1);
			if ((sie1 == DP_NODE_EN) && (sit1 == 1)) {
				/* no queue connected to this leaf */
				nodeid = qos_hal_get_sched_index_from_schedid(qsid1);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <leaf %d> SB connected = %d nodeid: %d\n", __func__, j, qsid1, nodeid);
				if (qos_hal_free_wfq_leaf_get(nodeid, &free_wfq_leaf) == QOS_HAL_STATUS_OK) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Free Scheduler input  = %d\n", __func__, free_wfq_leaf);
					*free_wfq_sched_input = nodeid;
					return QOS_HAL_STATUS_OK;
				}
			} else {
				uint32_t new_omid;
				if (qos_hal_sched_track[omid >> 3].policy == QOS_HAL_POLICY_WFQ)
					new_omid = create_new_scheduler(omid >> 3, 0, QOS_HAL_POLICY_WFQ, (qos_hal_sched_track[omid >> 3].level + 1), j);
				else
					new_omid = create_new_scheduler(omid >> 3, 0, QOS_HAL_POLICY_WRR, (qos_hal_sched_track[omid >> 3].level + 1), j);
				*free_wfq_sched_input = new_omid;
				return QOS_HAL_STATUS_OK;
			}
		}
	} else if ((i == 8) && (qos_hal_sched_track[omid >> 3].policy == QOS_HAL_POLICY_WSP)) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Need to create WFQ level for same priority \n", __func__);
		qos_hal_add_scheduler_level(port, base_sched, 1, &parent, QOS_HAL_POLICY_WFQ, &omid);
		qos_hal_sched_track[omid].policy = QOS_HAL_POLICY_WFQ;
		qos_hal_sched_track[parent].policy = QOS_HAL_POLICY_WFQ;
		Snext.peer_sched_id = omid;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Parent = %d Omid = %d\n", __func__, parent, omid);
	}

	*free_wfq_sched_input = (omid << 3);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Free WFQ Sched input = %d\n", __func__, *free_wfq_sched_input);
	return QOS_HAL_STATUS_OK;
}

int qos_hal_del_shaper_index(int shaper_index)
{
	QOS_HAL_ASSERT(shaper_index >= QOS_HAL_MAX_SHAPER);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Shaper instance  [%d]\n", __func__, shaper_index);
	qos_hal_shaper_track[shaper_index].is_enabled = false;
	qos_hal_shaper_track[shaper_index].tb_cfg.index = 0xFF;
	qos_hal_shaper_track[shaper_index].tb_cfg.enable = false;
	qos_hal_shaper_track[shaper_index].tb_cfg.mode = 0;
	qos_hal_shaper_track[shaper_index].tb_cfg.pir = 0;
	qos_hal_shaper_track[shaper_index].tb_cfg.cir = 0;
	qos_hal_shaper_track[shaper_index].tb_cfg.pbs = 0;
	qos_hal_shaper_track[shaper_index].tb_cfg.cbs = 0;
	return QOS_HAL_STATUS_OK;
}

enum qos_hal_errorcode
qos_hal_safe_queue_and_shaper_delete(
		struct net_device *netdev,
		char *dev_name,
		int32_t index,
		int32_t qos_port,
		uint32_t shaper_index,
		int32_t remap_to_qid,
		uint32_t flags)
{

	if (shaper_index != 0xFF) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Disabling the shaper index %d from scheduler input  %d\n",
				__func__, shaper_index, qos_hal_queue_track[index].sched_input);
		qos_hal_token_bucket_shaper_delete(shaper_index, qos_hal_queue_track[index].sched_input);

		if ((flags & QOS_HAL_DEL_SHAPER_CFG) == QOS_HAL_DEL_SHAPER_CFG) {
			/* Delete the shaper instance */
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete the shaper instance %d  \n", __func__, shaper_index);
			qos_hal_del_shaper_index(shaper_index);
			qos_hal_queue_track[index].tb_index = 0xFF;
		}
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Queue %d for netdev %s\n", __func__, index, netdev->name);
	if (qos_hal_egress_queue_delete(index) == QOS_HAL_STATUS_ERR) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to delete Queue\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_shift_up_down_q(
		struct net_device *netdev,
		char *dev_name,
		int32_t port,
		uint32_t sched_input,
		int32_t q_in,
		uint32_t shaper_index,
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t leaf,
		uint32_t flags)
{
	struct qos_hal_equeue_create q_reconnect;
	struct dp_node_prio prio_info = {0};
	int ret = QOS_HAL_STATUS_OK;
	uint32_t user_q_index = 0, q_index, q_old_sbin, q_old_leaf;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <%d> Connect Phy Q:[%d] to leaf:[%d]\n",
			__func__, __LINE__, q_in, leaf);

	q_reconnect.scheduler_input =  sched_input << 3 | (leaf);

	prio_info.inst = g_Inst;
	prio_info.id.q_id = q_in;
	prio_info.type = DP_NODE_QUEUE;
	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Fail to get Prio of node:[%d]\n",
				__func__, __LINE__, q_in);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue [%d] has Priority: [%d]\n", __func__,
			q_in, prio_info.prio_wfq);

	prio_info.prio_wfq = leaf;
	if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Fail to set Prio of the node:[%d] to loc:[%d]\n",
				__func__, __LINE__, q_in, leaf);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Q:[%d] change to Prio:[%d]\n",
			__func__, q_in, prio_info.prio_wfq);

	q_index = qos_hal_get_queue_index_from_qid(q_in);
	/* Here q_index EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
	if (q_index >= EGRESS_QUEUE_ID_MAX)
		return QOS_HAL_STATUS_ERR;
	user_q_index = qos_hal_queue_track[q_index].user_q_idx;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Configure Drop params for mode %s \n",
			__func__, (subif_index->user_queue[user_q_index].ecfg.wred_enable == 0) ? "DT" : "WRED");

	q_old_sbin = qos_hal_queue_track[q_index].sched_input;
	q_old_leaf = q_old_sbin & 0x7;
	qos_hal_sched_track[sched_input].leaf_mask &= ~(1 << q_old_leaf);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <%d> Updating user_q_index: [%d] scheduler input to: [%d]\n",
			__func__, __LINE__, user_q_index, q_reconnect.scheduler_input);
	subif_index->user_queue[user_q_index].sbin = q_reconnect.scheduler_input;
	qos_hal_queue_track[q_index].sched_input = q_reconnect.scheduler_input;
	qos_hal_sched_track[sched_input].leaf_mask |= (1 << leaf) ;
	qos_hal_dump_sb_info(sched_input);
	return ret;
}

/** This function is used to delete and then add all the scheduler block
    input from sbin to all the lower priority input of that scheduler.
    In the caller function the queue is getting deleted at this sbin.
    So this function will be called to shift all the scheduler input
    up to this scheduler input (sbin).

	netdev: net device for which the operation is performed
	port: QOS port
	subif_index: pointer to sub interface database information
	sbin: shift up all the scheduler input to this sbin

	Step 1: Run a loop for j = sbin to 6 :
	Step 2: Check if whether the ( sbin + j )input is enabled
	Step 3: If input is disabled then do nothing
	Step 4: If Yes, then check if the scheduler input is a queue
		or another scheduler
	Step 5: If Queue then delete the queue from that input and
		connect to sbin
	Step 6: If the input is a scheduler then check the scheduler
		policy.
	Step 7: If scheduler policy is Strict and sub interface index
		for physical port (is_logical == 0) then that means one
		logical interface VLAN/VAP is connected to this. SO
		update the port scheduler input egress of the subif index
		of that logical interface.
	Step 8: If scheduler policy is a WFQ then loop for all the scheduler
		input. If the input is also a scheduler input, then it could
		only be a logical interface that is connected to this priority
		level. So whichever logical interface that is connected to this
		priority level, update the port_sched_in_egress.
	Step 9: Remap the scheduler with correct omid and level.
	Step 10: Loop is completed. Goto Step1.
*/
int qos_hal_shift_up_sbin_conn(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port,
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t sbin,
		uint32_t flags)
{
	int i, j;
	uint32_t q_in, node_type;
	uint32_t q_index;
	int32_t q_id, nodeid, sched_out_conn;
	uint32_t sched_input = sbin >> 3;
	uint32_t sie = 0, sit = 0, qsid = 0;
	int ret = QOS_HAL_STATUS_OK;
	struct dp_node_prio prio_info = {0};

	i = sbin & 7;
	if (i >= 8)
		return QOS_HAL_STATUS_ERR;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shift up to the index ->[%d]\n",
			__func__, i);

	for (j = i; j < 7; j++) {
		if (sched_input < QOS_HAL_MAX_SCHEDULER) {
			ret = qos_hal_scheduler_in_status_get(sched_input << 3 | (j + 1), &sie, &sit, &qsid);
			if (ret == QOS_HAL_STATUS_NOT_AVAILABLE) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "qos_hal_scheduler_in_status_get -no child\n");
				ret = QOS_HAL_STATUS_OK;
				break;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SBIN:[%d]-->[%s]\n",
					__func__,
					sched_input << 3 | (j+1),
					sie ? "enabled" : "disabled");
			node_type = DP_NODE_SCH;
			sched_out_conn = 1;
		} else {
			ret = qos_hal_node_in_status_get(port << 3 | (j + 1), DP_NODE_PORT, &sie, &sit, &qsid);
			if (ret == QOS_HAL_STATUS_NOT_AVAILABLE) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "qos_hal_node_in_status_get -no child\n");
				ret = QOS_HAL_STATUS_OK;
				break;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port:[%d] leaf:[%d] --> [%s]\n",
					__func__,
					port,
					(j+1),
					(sie == DP_NODE_EN) ? "enabled" : "disabled");
			node_type = DP_NODE_PORT;
			sched_out_conn = 0;
		}

		if (sie == DP_NODE_EN) { /* input is enabled */
			if (sit == 0) { /* input is QID */
				uint32_t cfg_shaper;

				q_in = qsid;
				q_index = qos_hal_get_queue_index_from_qid(q_in);
				/* Here q_index EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
				if (q_index >= EGRESS_QUEUE_ID_MAX)
					return QOS_HAL_STATUS_ERR;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is attached to this Q\n",
						__func__, qos_hal_queue_track[q_index].tb_index);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Q:%d has policy:%d\n",
						__func__, q_index, qos_hal_queue_track[q_index].q_type);
				cfg_shaper = qos_hal_queue_track[q_index].tb_index;
				q_id = -1;
				ret = qos_hal_get_node_id_from_prio(
						j + 1,
						node_type,
						port,
						sched_input,
						&q_id);
				if (ret != QOS_HAL_STATUS_ERR) {
					if ((q_id > -1) &&
					(qos_hal_queue_track[q_index].q_type == QOS_HAL_POLICY_WSP))
						ret = qos_hal_shift_up_down_q(
								netdev,
								dev_name,
								port,
								sched_input,
								q_id,
								cfg_shaper,
								subif_index,
								j, flags);
				}
			} else if (sit == 1) { /* input is SBID */
				/* must be one sub-interface is connected to this leaf */
				uint32_t wt = 0;
				struct qos_hal_user_subif_abstract *temp_index = NULL;
				struct qos_hal_user_subif_abstract *base_subif_index = NULL;

				nodeid = qos_hal_get_sched_index_from_schedid(qsid);
				base_subif_index = (qos_hal_user_sub_interface_info + port);
				qos_hal_lookup_subif_list_item(port, netdev, &temp_index, &base_subif_index->head);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif_index->is_logical:%d qos_hal_sched_track[nodeid].policy:%d\n", __func__, temp_index->is_logical, qos_hal_sched_track[nodeid].policy);

				if ((temp_index->is_logical == 0) &&
						(qos_hal_sched_track[nodeid].policy == QOS_HAL_POLICY_WSP)) {
					int t;

					for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d qsid = %d\n",
								__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress,
								temp_index->port_shaper_sched, qsid);
						if ((temp_index->base_sched_id_egress == qsid) || temp_index->port_shaper_sched == qsid) {
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d qsid = %d\n",
									__func__, temp_index->base_sched_id_egress, qsid);
							break;
						}
						temp_index += 1;
					}
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif_index->base_sched_id_egress:%d shift up scheduler: %d\n",
							__func__, subif_index->base_sched_id_egress, sched_input);
					if (subif_index->base_sched_id_egress == sched_input) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface port scheduler input is changed from %d -> %d\n",
								__func__, temp_index->port_sched_in_egress, sched_input << 3 | j);
						temp_index->port_sched_in_egress = sched_input << 3 | j;
					}
				} else {
					/* must be many queues or one or more logical interface are
						also connected to this leaf */
					uint32_t sie1 = 0, sit1 = 0, qsid1 = 0;
					struct qos_hal_user_subif_abstract *temp_index = NULL;
					int32_t w = 0, t = 0;
					int32_t nodeid1;

					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler %d has Policy : %d \n",
							__func__, nodeid, qos_hal_sched_track[nodeid].policy);

					for (w = 0; w < 8; w++) {
						qos_hal_scheduler_in_status_get(nodeid << 3 | w, &sie1, &sit1, &nodeid1);
						if (sit1 == 1) { /* input is SBID */
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler input %d : leaf %d\n",
									__func__, nodeid1, w);
							if (qos_hal_sched_track[qsid1].policy == QOS_HAL_POLICY_WSP) {
								temp_index = subif_index + 1;
								for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
									QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d qsid = %d\n",
											__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress,
											temp_index->port_shaper_sched, qsid1);
									if ((temp_index->base_sched_id_egress == qsid1) ||
											temp_index->port_shaper_sched == qsid1) {
										QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d qsid = %d\n",
												__func__, temp_index->base_sched_id_egress,
												temp_index->port_sched_in_egress, qsid1);
										wt = (subif_index->subif_weight == 0) ? 1 : subif_index->subif_weight;
										break;
									}
									temp_index += 1;
								}
								QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif_index->base_sched_id_egress:%d sched_input: %d",
										__func__, subif_index->base_sched_id_egress, sched_input);
								if (subif_index->base_sched_id_egress == sched_input) {
									QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub if %d port scheduler input is changed from %d -> %d\n",
											__func__, t, temp_index->port_sched_in_egress, sched_input << 3 | (j));
									temp_index->port_sched_in_egress = sched_input << 3 | j;
								}
							}
						}
					}
				}

				wt = (subif_index->subif_weight == 0) ? 1 : subif_index->subif_weight;

				prio_info.inst = g_Inst;
				prio_info.id.sch_id = qsid;
				prio_info.type = DP_NODE_SCH;

				if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n",
								__func__, __LINE__, qsid);
					return QOS_MGR_FAILURE;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> priority: [%d] arbi:[%d]\n",
							__func__, prio_info.prio_wfq, prio_info.arbi);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Set the priority of sched to:[%d]\n",
							__func__, j);
				prio_info.prio_wfq = j;
				if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set priority of the Queue Node [%d]\n",
							__func__, __LINE__, qsid);
					return QOS_MGR_FAILURE;
				}
				qos_hal_sched_track[sched_input].leaf_mask &= ~(1 << (j+1));
				qos_hal_sched_track[sched_input].leaf_mask |=  (1 << j);
				/* Remeber the omid of the scheduler */
				qos_hal_sched_track[nodeid].omid = sched_input << 3 | j;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Updated leaf mask of SB: %d : %d\n",
							sched_input,
							qos_hal_sched_track[sched_input].leaf_mask);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Updated omid: %d and leaf-mask:%d for nodeid:%d\n",
							qos_hal_sched_track[nodeid].omid, qos_hal_sched_track[nodeid].leaf_mask, nodeid);

			} /* input is SBID */
		} /* input is enabled */
	} /* for loop */
	return ret;

}

/** This function is used to delete and then add all the scheduler block
    input from sbin to all the lower priority input of that scheduler.
    In the caller function the queue is getting deleted at this sbin.
    So this function will be called to shift all the scheduler input
    down from this scheduler input (sbin).

	netdev: net device for which the operation is performed
	port: QOS port
	subif_index: pointer to sub interface database information
	sb_idx: shift down all the scheduler input from this sbin

	Step 1: Run a loop for j = sbin to 6 :
	Step 2: Check if whether the ( sbin + j )input is enabled
	Step 3: If input is disabled then do nothing
	Step 4: If Yes, then check if the scheduler input is a queue
		or another scheduler
	Step 5: If Queue then delete the queue from that input and
		connect to sbin
	Step 6: If the input is a scheduler then check the scheduler
		policy.
	Step 7: If scheduler policy is Strict and sub interface index
		for physical port (is_logical == 0) then that means one
		logical interface VLAN/VAP is connected to this. SO
		update the port scheduler input egress of the subif index
		of that logical interface.
	Step 8: If scheduler policy is a WFQ then loop for all the scheduler
		input. If the input is also a scheduler input, then it could
		only be a logical interface that is connected to this priority
		level. So whichever logical interface that is connected to this
		priority level, update the port_sched_in_egress.
	Step 9: Remap the scheduler with correct omid and level.
	Step 10: Loop is completed. Goto Step1.
*/
int qos_hal_shift_down_sbin_conn(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port,
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t sb_idx,
		uint32_t priority,
		uint32_t flags)
{
	int i, j;
	uint8_t leaf_mask;
	uint32_t q_in, node_type;
	int32_t q_id;
	uint32_t q_index;
	uint32_t sched_input = 0;
	uint32_t sie = 0, sit = 0, qsid = 0;
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t cfg_shaper;


	if (sb_idx < QOS_HAL_MAX_SCHEDULER) {
		sched_input = sb_idx;
		leaf_mask = qos_hal_sched_track[sb_idx].leaf_mask;
	} else
		leaf_mask = qos_hal_port_track[port].leaf_mask;

	/* get any free leaf out of 0 to 7 */
	for (i = 0; i < 8; i++) {
		if (!((leaf_mask >> i) & 0x1)) {
			leaf_mask |= (1 << i);
			break;
		}
	}
	if (i >= 8) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR No Free leaf\n",
				__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Free leaf=[%d] Index to add:[%d]\n",
			__func__, i, priority);

	for (j = 6; j >= priority; j--) {
		if (j < 0)
			return QOS_HAL_STATUS_OK;

		if (sched_input < QOS_HAL_MAX_SCHEDULER) {
			qos_hal_scheduler_in_status_get(
				sched_input << 3 | j,
				&sie,
				&sit,
				&qsid);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SBIN:[%d]->[%s]\n",
					__func__,
					sched_input << 3 | j,
					sie ? "enabled" : "disabled");
			node_type = DP_NODE_SCH;
		} else {
			qos_hal_node_in_status_get(
				port << 3 | j,
				DP_NODE_PORT,
				&sie,
				&sit,
				&qsid);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port:[%d] leaf:[%d]->[%s]\n",
					__func__,
					port,
					j,
					(sie == DP_NODE_EN) ? "enabled" : "disabled");
			node_type = DP_NODE_PORT;
		}
		if (sie == DP_NODE_EN) {
			if (sit == 0) { /* input is QID */
				q_in = qsid;

				q_index = qos_hal_get_queue_index_from_qid(q_in);
				/* Here q_index EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
				if (q_index >= EGRESS_QUEUE_ID_MAX)
					return QOS_HAL_STATUS_ERR;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is attached to the Q %d\n",
						__func__, qos_hal_queue_track[q_index].tb_index, q_index);
				cfg_shaper = qos_hal_queue_track[q_index].tb_index;
				q_id = -1;
				ret = qos_hal_get_node_id_from_prio(
						j,
						node_type,
						port,
						sched_input,
						&q_id);
				if (ret != QOS_HAL_STATUS_ERR) {
					if ((q_id > -1) &&
					(qos_hal_queue_track[q_index].q_type == QOS_HAL_POLICY_WSP))
						ret = qos_hal_shift_up_down_q(
								netdev,
								dev_name,
								port,
								sched_input,
								q_id,
								cfg_shaper,
								subif_index,
								(j + 1),
								flags);
				}
			} else if (sit == 1) { /* input is SBID */
				int32_t s_index = QOS_MGR_FAILURE;
				struct dp_node_prio prio_info = {0};

				s_index = qos_hal_get_sched_index_from_schedid(qsid);
				if (s_index == QOS_MGR_FAILURE)
					return QOS_MGR_FAILURE;
				if (subif_index->is_logical == 0 &&
					qos_hal_sched_track[s_index].policy == QOS_HAL_POLICY_WSP) {
					/* must be sub-interface is connected to this leaf
					 */
					int t;
					struct qos_hal_user_subif_abstract *temp_index = NULL;

					temp_index = subif_index + 1;
					for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT; t++) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d qsid = %d\n",
								__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress,
								temp_index->port_shaper_sched, qsid);
						if ((temp_index->base_sched_id_egress == qsid) || temp_index->port_shaper_sched == qsid) {
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d qsid = %d\n",
									__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, qsid);
							break;
						}
						temp_index += 1;
					}
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface %d port scheduler input is changed from %d -> %d\n",
							__func__, t, temp_index->port_sched_in_egress, sched_input << 3 | (j+1));
					temp_index->port_sched_in_egress = sched_input << 3 | (j+1);
				} else {
					/* WRR Shcedular is connected for same priority queue
					 */
					uint32_t sie1 = 0, sit1 = 0, qsid1 = 0;
					struct qos_hal_user_subif_abstract *temp_index = NULL;
					int32_t w = 0, t = 0;

					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler %d has Policy : %d\n", __func__, qsid, qos_hal_sched_track[qsid].policy);

					for (w = 0; w < 8; w++) {
						qos_hal_scheduler_in_status_get(s_index << 3 | w, &sie1, &sit1, &qsid1);
						if (sit1 == 1) { /* input is SBID */
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler input %d : leaf %d \n",
									__func__, qsid1, w);
							if (qos_hal_sched_track[qsid].policy == QOS_HAL_POLICY_WSP) {
								temp_index = subif_index + 1;
								for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
									QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d qsid = %d\n",
											__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress,
											temp_index->port_shaper_sched, qsid1);
									if ((temp_index->base_sched_id_egress == qsid1) || temp_index->port_shaper_sched == qsid1) {
										QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d qsid = %d\n",
												__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, qsid1);
										break;
									}
									temp_index += 1;
								}
								QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface %d port scheduler input is changed from %d -> %d\n",
										__func__, t, temp_index->port_sched_in_egress, sched_input << 3 | (j+1));
								temp_index->port_sched_in_egress = sched_input << 3 | (j + 1);
							}
						}
					}
				}
				prio_info.inst = g_Inst;
				prio_info.id.sch_id = qsid;
				prio_info.type = DP_NODE_SCH;

				if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node [%d]\n",
							__func__, __LINE__, qsid);
					return QOS_MGR_FAILURE;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> priority: [%d] arbi:[%d]\n",
							__func__, prio_info.prio_wfq, prio_info.arbi);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Set the priority of sched to:[%d]\n",
							__func__, j+1);

				prio_info.prio_wfq = j + 1;
				if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set priority of the Queue Node [%d]\n",
							__func__, __LINE__, qsid);
					return QOS_MGR_FAILURE;
				}
				qos_hal_sched_track[sched_input].leaf_mask &= ~(1 << j);
				qos_hal_sched_track[sched_input].leaf_mask |=  (1 << (j + 1));
				/* Remeber the omid of the scheduler */
				qos_hal_sched_track[s_index].omid = sched_input << 3 | (j + 1);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Updated leaf mask of SB: %d : %d\n",
							sched_input,
							qos_hal_sched_track[sched_input].leaf_mask);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Updated omid: %d\n",
							qos_hal_sched_track[s_index].omid);

			} /* sit is enabled */
		}
	} /* for loop */
	return ret;
}

int qos_hal_q_align_for_priority_selector(
			struct net_device *netdev,
			char *dev_name,
			uint32_t port,
			struct qos_hal_user_subif_abstract *subif_index,
			uint32_t sb1_idx,
			uint32_t sb2_idx,
			uint32_t priority,
			uint32_t flags)
{
	int i, j, k;
	uint8_t leaf_mask;
	uint32_t q_in;
	uint32_t sched_in;
	uint32_t sched_input = sb1_idx;
	uint32_t sched_input_new = sb2_idx;
	uint32_t sie = 0, sit = 0, qsid = 0;
	uint32_t q_loc = 0, temp_index = 0, prev_index = 0;
	int ret = QOS_HAL_STATUS_OK;

	leaf_mask = qos_hal_sched_track[sb1_idx].leaf_mask;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Find the index for priority [%d] SB [%d]\n", __func__, priority, sched_input);
	i = 0;
	for (j = 7; j >= 0; j--) {
		uint32_t q_index;
		sched_in = sb1_idx << 3 | j ;
		qos_hal_scheduler_in_status_get(sched_in, &sie, &sit, &qsid);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Node: [%d:%d] --> [%s]\n", __func__, sb1_idx, j, sie ? "enabled" : "disabled");
		i++;
		if (sit == 0) { /* input is QID */
			q_index = qos_hal_get_queue_index_from_qid(qsid);
			/* Here q_index EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
			if (q_index >= EGRESS_QUEUE_ID_MAX)
				return QOS_HAL_STATUS_ERR;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue index for physical queue :[%d] is [%d]\n", __func__, qsid, q_index);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Prio level %d for Q index %d\n",
					__func__, subif_index->user_queue[qos_hal_queue_track[q_index].user_q_idx].prio_level, q_index);
			if (priority > subif_index->user_queue[qos_hal_queue_track[q_index].user_q_idx].prio_level) {
				qos_hal_queue_prio_get(qsid, &q_loc);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Actual Prio level is %d for Q index %d\n", __func__, q_loc, qsid);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Prev index %d\n", __func__, prev_index);
				if (q_loc == 0)
					temp_index = 1;
				else if (q_loc == 7)
					temp_index = q_loc;
				else
					temp_index = q_loc + 1;
				if ((temp_index > prev_index)) {
					prev_index = temp_index;
				}
			}
		} else if (sit == 1) {/* input is SBID */
			for (k = 0; k < subif_index->queue_cnt; k++) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "k %d sbin %d\n", k, subif_index->user_queue[k].sbin);
				if ((subif_index->user_queue[k].sbin >> 3) == qsid)
					break;
			}
			if (k == subif_index->queue_cnt) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue Count on main interface=%d Queue Count on port =%d\n", subif_index->queue_cnt,
						qos_hal_port_track[subif_index->qos_port_idx].no_of_queues);
				if (subif_index->queue_cnt < qos_hal_port_track[subif_index->qos_port_idx].no_of_queues) {
					if (subif_index->is_logical == 0) {
						int t;
						struct qos_hal_user_subif_abstract *temp_index = NULL;

						temp_index = subif_index + 1;
						for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sub interface %d User level %d\n", t, temp_index->port_sched_in_user_lvl);
							if (temp_index->port_sched_in_user_lvl < priority) {
								goto Q_ALIGN_INDEX;
							}
							temp_index += 1;
						}
					}
				}
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "user Queue Index %d is connected to SBIN %d\n ", k, subif_index->user_queue[k].sbin);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Prio level %d\n", subif_index->user_queue[k].prio_level);
				if (subif_index->user_queue[k].prio_level < priority)
					break;
			}
		}
	}

Q_ALIGN_INDEX:
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> i =%d j = %d prev_index = [%d]\n", __func__, i, j, prev_index);

	if (j == 7)
		return QOS_HAL_STATUS_OK;

	k = 0;
	j += 1;
	j = prev_index;
	for ( ; j < 8; j++) {
		qos_hal_scheduler_in_status_get(sched_input << 3, &sie, &sit, &qsid);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "SBIN: %d is %s\n", sched_input << 3 | j, sie ? "enabled" : "disabled");
		if (sit == 0) { /* input is QID */
			uint32_t cfg_shaper;
			uint32_t qindex;
			int32_t q_id = -1;
			struct qos_hal_equeue_create q_param = {0};

			q_in = qsid;
			qos_hal_get_node_id_from_prio(
					j,
					DP_NODE_SCH,
					port,
					sched_input,
					&q_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is attached to this Queue\n", __func__, qos_hal_queue_track[q_in].tb_index);
			cfg_shaper = qos_hal_queue_track[q_in].tb_index;
			qindex = qos_hal_get_queue_index_from_qid(q_id);
			/* Here qindex EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
			if (qindex >= EGRESS_QUEUE_ID_MAX)
				return QOS_HAL_STATUS_ERR;
			q_param.index = qindex;
			q_param.egress_port_number = port;
			q_param.scheduler_input = (sched_input_new << 3 | k);
			q_param.prio = k;

			if (qos_hal_queue_relink(&q_param) != QOS_HAL_STATUS_OK) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> Failed to Relink Q:[%d]\n",
						__func__, __LINE__, q_id);
				return QOS_HAL_STATUS_ERR;
			}
			qos_hal_sched_track[sched_input].leaf_mask &= ~(1 << j);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> leaf mask of SB:%d is --> %x\n",
						__func__, sched_input, qos_hal_sched_track[sched_input].leaf_mask);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> leaf mask of SB:%d is --> %x\n",
						__func__, sched_input_new, qos_hal_sched_track[sched_input_new].leaf_mask);
		} else if (sit == 1) { /* input is SBID */
			uint32_t omid;
			uint32_t sb_in = (sched_input << 3 | j);
			omid = sched_input_new << 3 | k;
			if (subif_index->is_logical == 0 &&
					qos_hal_sched_track[qsid].policy == QOS_HAL_POLICY_WSP) {/* must be sub-interface is connected to this leaf */
				int t;
				struct qos_hal_user_subif_abstract *temp_index = NULL;

				temp_index = subif_index + 1;
				for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
					if ((temp_index->base_sched_id_egress == qsid) || temp_index->port_shaper_sched == qsid) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "base_sched_id: %d qsid = %d\n", temp_index->base_sched_id_egress, qsid);
						break;
					}
					temp_index += 1;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface port scheduler input is changed from %d -> %d\n",
						__func__, temp_index->port_sched_in_egress, omid);
				temp_index->port_sched_in_egress = omid;
			} else {
				uint32_t sie1 = 0, sit1 = 0, qsid1 = 0;
				struct qos_hal_user_subif_abstract *temp_index = NULL;
				int32_t w = 0, t = 0;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler %d has Policy : %d \n", __func__, qsid, qos_hal_sched_track[qsid].policy);

				for (w = 0; w < 8; w++) {
					qos_hal_scheduler_in_status_get(qsid << 3, &sie1, &sit1, &qsid1);
					if (sit1 == 1) { /* input is SBID */
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler input %d : leaf %d \n",
								__func__, qsid1, w);
						if (qos_hal_sched_track[qsid1].policy == QOS_HAL_POLICY_WSP) {
							temp_index = subif_index + 1;
							for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
								QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d port_shaper_sched %d qsid = %d\n",
										__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress,
										temp_index->port_shaper_sched, qsid1);
								if ((temp_index->base_sched_id_egress == qsid1) || temp_index->port_shaper_sched == qsid1) {
									QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id: %d port_sched_in %d qsid = %d\n",
											__func__, temp_index->base_sched_id_egress, temp_index->port_sched_in_egress, qsid1);
									break;
								}
								temp_index += 1;
							}
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface port scheduler input is changed from %d -> %d\n",
									__func__, temp_index->port_sched_in_egress, omid);
							temp_index->port_sched_in_egress = omid;

						}
					}
				}
			}

			qos_hal_sched_out_remap(qsid, qos_hal_sched_track[sched_input].level + 1, omid, 1, 0);

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Disable the input link of : %d \n", __func__, sb_in);
			qos_hal_scheduler_in_enable_disable(sb_in, 0);
			qos_hal_sched_track[sched_input].leaf_mask &= ~(1 << j);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Updated leaf mask of SB: %d : %d \n", __func__, sb_in, qos_hal_sched_track[sched_input].leaf_mask);

		}
		k++;
	}
	return ret;
}

/** The qos_hal_create_port_scheduler function is used to add a scheduler
    when a queue will be added for any logical interface of that port.
*/
int qos_hal_create_port_scheduler(
			uint32_t port,
			enum qos_hal_policy policy,
			uint32_t *sched)
{
	uint32_t sched_prio, omid;
	uint32_t remap_sched_id;
	int input_type, level;

	if (qos_hal_port_track[port].shaper_sched_id == 0xFF) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No Shaper scheduler is there\n", __func__);
		omid = port;
		input_type = 1;
		level = 0;
		remap_sched_id = qos_hal_port_track[port].input_sched_id;
	} else {/* Port Rate shaper is already there */
		omid = qos_hal_port_track[port].shaper_sched_id;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper scheduler [%d] is there\n", __func__, omid);
		input_type = 0;
		level = 1;
		remap_sched_id = qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].next_sched_id ;
	}

	//*sched = sched_prio = create_new_scheduler(omid, input_type, QOS_HAL_POLICY_WSP, level, 0);
	*sched = sched_prio = create_new_scheduler(omid, input_type, policy, level, 0);
	if (sched_prio == QOS_HAL_STATUS_ERR)
		return QOS_HAL_STATUS_ERR;
	qos_hal_port_track[port].port_sched_id = sched_prio;
	qos_hal_sched_track[sched_prio].policy = policy;
	qos_hal_sched_track[sched_prio].next_sched_id = remap_sched_id;

	if (remap_sched_id < QOS_HAL_MAX_SCHEDULER) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Scheduler that needs to Remap is [%d]\n", remap_sched_id);
		qos_hal_sched_out_remap(remap_sched_id, level + 1, sched_prio << 3, 1, 0);
	}

	return QOS_HAL_STATUS_OK;
}

/********* LIST & LOCK functions **********/
QOS_MGR_LOCK g_qos_hal_list_lock;

void qos_hal_subif_lock_list(void)
{
	qos_mgr_lock_get(&g_qos_hal_list_lock);
}

void qos_hal_subif_unlock_list(void)
{
	qos_mgr_lock_release(&g_qos_hal_list_lock);
}

struct qos_hal_user_subif_abstract *qos_hal_alloc_subif_item(void)
{
	struct qos_hal_user_subif_abstract  *obj;

	obj = (struct qos_hal_user_subif_abstract  *)qos_mgr_malloc(sizeof(*obj));
	if (obj) {
		qos_mgr_memset(obj, 0, sizeof(*obj));
		qos_mgr_atomic_set(&obj->count, 1);
	}
	return obj;
}

void __qos_hal_add_subif_list_item(struct qos_hal_user_subif_abstract *obj,
			struct qos_hal_user_subif_abstract **obj_head)
{
	/*qos_mgr_atomic_inc(&obj->count);*/
	obj->next = *obj_head;
	*obj_head = obj;
}

void qos_hal_free_subif_list_item(struct qos_hal_user_subif_abstract *obj)
{
	if (qos_mgr_atomic_dec(&obj->count) == 0) {
		pr_debug("Free the object\n");
		qos_mgr_free(obj);
	}
}

void qos_hal_remove_subif_list_item(struct net_device *dev,
		struct qos_hal_user_subif_abstract **obj_head)
{
	struct qos_hal_user_subif_abstract *p_prev, *p_cur;
	p_prev = NULL;

	qos_hal_subif_lock_list();
	for (p_cur = *obj_head; p_cur; p_prev = p_cur, p_cur = p_cur->next) {
		if ((strcmp(p_cur->netdev->name, dev->name) == 0)) {
			pr_debug("Remove subif--> %s\n", dev->name);
			if (!p_prev)
				*obj_head = p_cur->next;
			else
				p_prev->next = p_cur->next;

			qos_hal_free_subif_list_item(p_cur);
			break;
		}
	}
	qos_hal_subif_unlock_list();
}

void qos_hal_subif_list_item_put(struct qos_hal_user_subif_abstract *obj)
{
	if (obj)
		qos_hal_free_subif_list_item(obj);
}

int32_t qos_hal_lookup_subif_list_item(uint32_t qosport,
			struct net_device *dev,
			struct qos_hal_user_subif_abstract **obj,
			struct qos_hal_user_subif_abstract **obj_head)
{
	int32_t ret = QOS_MGR_ENOTAVAIL;
	struct qos_hal_user_subif_abstract *p;

	qos_hal_subif_lock_list();
	for (p = *obj_head; p; p = p->next) {
		if (p != NULL) {
			if ((p->netdev != NULL) && (dev != NULL)) {
				if ((strcmp(p->netdev->name, dev->name) == 0)) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> netdev match found\n",
							__func__);
					if (obj) {
						/*qos_mgr_atomic_inc(&p->count);*/
						*obj = p;
						ret = QOS_MGR_SUCCESS;
					}
					break;
				}
			} else if (dev == NULL) {
				if (p->qos_port_idx == qosport) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port index match found\n",
							__func__);
					if (obj) {
						/*qos_mgr_atomic_inc(&p->count);*/
						*obj = p;
						ret = QOS_MGR_SUCCESS;
					}
					break;
				}
			}
		}
	}
	qos_hal_subif_unlock_list();
	return ret;
}

int32_t qos_hal_get_subif_list_item(uint32_t qosport,
			struct net_device *dev,
			struct qos_hal_user_subif_abstract **obj,
			struct qos_hal_user_subif_abstract **obj_head)
{
	struct qos_hal_user_subif_abstract *p;

	if (qos_hal_lookup_subif_list_item(qosport, dev, obj, obj_head) == QOS_MGR_SUCCESS)
		return QOS_MGR_SUCCESS;

	p = qos_hal_alloc_subif_item();
	if (!p) {
		pr_err("Obj error\n");
		return QOS_MGR_FAILURE;
	}
	if (dev != NULL) {
		p->netdev = dev;
		p->qos_port_idx = 0xFF;
	} else {
		pr_debug("Added port index %d for Ingress\n", qosport);
		p->qos_port_idx = qosport;
		p->netdev = NULL;
	}

	pr_debug("Add to the list %p\n", p);
	__qos_hal_add_subif_list_item(p, obj_head);
	*obj = p;

	return QOS_MGR_SUCCESS;
}

/*level >=1: only get the specified level if possible
 *otherwise: -1 maximum
 */
struct net_device *get_base_dev(struct net_device *dev, int level)
{
	struct net_device *lower_dev, *tmp;
	struct list_head *iter;

	tmp = dev;
	lower_dev = NULL;
	do {
		netdev_for_each_lower_dev(tmp, lower_dev, iter)
			break;
		if (lower_dev) {
			tmp = lower_dev;
			lower_dev = NULL;
			level--;
			if (level == 0)
				break;
		} else {
			break;
		}
	} while (1);
	if (tmp == dev)
		return NULL;
	return tmp;
}

void dump_q_res(struct dp_queue_res *qos_res)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue Physical Node Id: [%d]\n", __func__, qos_res->q_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue Logical Node Id: [%d]\n", __func__, qos_res->q_id);
	if (qos_res->sch_lvl > 0)
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler Logical Node Id: [%d]\n", __func__, qos_res->sch_id[0]);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler lvl: [%d]\n", __func__, qos_res->sch_lvl);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler id at level: [%d] is [%d] \n", __func__, qos_res->sch_lvl, qos_res->sch_id[qos_res->sch_lvl]);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler Id[0]: [%d]\n", __func__, qos_res->sch_id[0]);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> CQM Dequeue Port: [%d]\n", __func__, qos_res->cqm_deq_port);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> QOS Dequeue Port: [%d]\n", __func__, qos_res->qos_deq_port);
}

int32_t qos_hal_dump_all_dp_link(void)
{
	struct dp_qos_link qoslink = {0};
	int i;

	qoslink.q_id = 11;
	if (dp_link_get(&qoslink, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get DP link\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : inst is [%d]\n", qoslink.inst);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : cqm_deq_port is [%d]\n", qoslink.cqm_deq_port);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : q_id is [%d]\n", qoslink.q_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : q_prio_wfq is [%d]\n", qoslink.q_prio_wfq);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : q_leaf is [%d]\n", qoslink.q_leaf);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : n_sch_lvl is [%d]\n", qoslink.n_sch_lvl);
	for (i = 0; i <= qoslink.n_sch_lvl; i++) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : sch.id is [%d]\n", qoslink.sch[i].id);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : sch.prio_wfq is [%d]\n", qoslink.sch[i].prio_wfq);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : sch.leaf is [%d]\n", qoslink.sch[i].leaf);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "QoS link : sch.arbi is [%d]\n", qoslink.sch[i].arbi);
	}
	return QOS_HAL_STATUS_OK;
}

void qos_hal_dump_all_queue_track(void)
{
	int i;
	for (i = 0; i < QOS_HAL_MAX_EGRESS_QUEUES; i++) {
		if (qos_hal_queue_track[i].is_enabled == 1) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Index: [%d]\n", i);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue is_enabled: [%d]\n", qos_hal_queue_track[i].is_enabled);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue queue_id: [%d]\n", qos_hal_queue_track[i].queue_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue connected_to_sched_id: [%d]\n", qos_hal_queue_track[i].connected_to_sched_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue sched_input: [%d]\n", qos_hal_queue_track[i].sched_input);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue attach_to: [%d]\n", qos_hal_queue_track[i].attach_to);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue q_type: [%d]\n", qos_hal_queue_track[i].q_type);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue prio_weight: [%d]\n", qos_hal_queue_track[i].prio_weight);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue tb_index: [%d]\n", qos_hal_queue_track[i].tb_index);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue epn: [%d]\n", qos_hal_queue_track[i].epn);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue user_q_idx: [%d]\n", qos_hal_queue_track[i].user_q_idx);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n");

		}
	}
}
void qos_hal_dump_all_port_track(void)
{
	int i;
	for (i = 0; i < QOS_HAL_MAX_EGRESS_PORT; i++) {
		if (qos_hal_port_track[i].is_enabled == 1) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Index: [%d]\n", i);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port is_enabled: [%d]\n", qos_hal_port_track[i].is_enabled);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port leaf_mask: [%d]\n", qos_hal_port_track[i].leaf_mask);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port policy: [%d]\n", qos_hal_port_track[i].policy);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port default_sched_id: [%d]\n", qos_hal_port_track[i].default_sched_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port input_sched_id: [%d]\n", qos_hal_port_track[i].input_sched_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port port_sched_id: [%d]\n", qos_hal_port_track[i].port_sched_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port shaper_sched_id: [%d]\n", qos_hal_port_track[i].shaper_sched_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port no_of_queues: [%d]\n", qos_hal_port_track[i].no_of_queues);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port no_of_schedular: [%d]\n", qos_hal_port_track[i].no_of_schedular);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port no_of_max_level: [%d]\n", qos_hal_port_track[i].no_of_max_level);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Port shaper id: [%d]\n", qos_hal_port_track[i].tbs);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n");
		}
	}
}
void qos_hal_dump_all_sched_track(void)
{
	int i;
	for (i = 0; i < QOS_HAL_MAX_SCHEDULER; i++) {
		if (qos_hal_sched_track[i].in_use == 1) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Index: [%d]\n", i);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched in_use: [%d]\n", qos_hal_sched_track[i].in_use);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched id: [%d]\n", qos_hal_sched_track[i].id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched leaf_mask: [%d]\n", qos_hal_sched_track[i].leaf_mask);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched policy: [%d]\n", qos_hal_sched_track[i].policy);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched level: [%d]\n", qos_hal_sched_track[i].level);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched peer_sched_id: [%d]\n", qos_hal_sched_track[i].peer_sched_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched next_sched_id: [%d]\n", qos_hal_sched_track[i].next_sched_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched sched_out_conn: [%d]\n", qos_hal_sched_track[i].sched_out_conn);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched omid: [%d]\n", qos_hal_sched_track[i].omid);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched priority_selector: [%d]\n", qos_hal_sched_track[i].priority_selector);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched tbs: [%d]\n", qos_hal_sched_track[i].tbs);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sched prio_weight: [%d]\n", qos_hal_sched_track[i].prio_weight);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\n");
		}
	}
}
void qos_hal_dump_all_shaper_track(void)
{
	int i;
	for (i = 0; i < QOS_HAL_MAX_SHAPER; i++) {
		if (qos_hal_shaper_track[i].is_enabled) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "START LOOP: [%d]\n", i);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper is_enabled: [%d]\n", qos_hal_shaper_track[i].is_enabled);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper shaper_id: [%d]\n", qos_hal_shaper_track[i].shaper_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper sb_input: [%d]\n", qos_hal_shaper_track[i].sb_input);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper attach_to: [%d]\n", qos_hal_shaper_track[i].attach_to);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper attach_to_id: [%d]\n", qos_hal_shaper_track[i].attach_to_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper index: [%d]\n", qos_hal_shaper_track[i].tb_cfg.index);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper enable: [%d]\n", qos_hal_shaper_track[i].tb_cfg.enable);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper mode: [%d]\n", qos_hal_shaper_track[i].tb_cfg.mode);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper cir: [%d]\n", qos_hal_shaper_track[i].tb_cfg.cir);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper pir: [%d]\n", qos_hal_shaper_track[i].tb_cfg.pir);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper cbs: [%d]\n", qos_hal_shaper_track[i].tb_cfg.cbs);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper pbs: [%d]\n", qos_hal_shaper_track[i].tb_cfg.pbs);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper attach_to: [%d]\n", qos_hal_shaper_track[i].tb_cfg.attach_to);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper attach_to_id: [%d]\n", qos_hal_shaper_track[i].tb_cfg.attach_to_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "END LOOP: [%d]\n", i);

		}
	}
}

int32_t qos_hal_config_wrr_sched_to_port(uint32_t port, uint32_t phy_q)
{
	uint32_t qindex;
	uint32_t connected_sched;
	struct qos_hal_equeue_create q_param = {0};
	struct dp_node_prio prio_info = {0};

	connected_sched = create_new_scheduler(port, 1, QOS_HAL_POLICY_WRR, 0, 0);

	memset(&q_param, 0, sizeof(struct qos_hal_equeue_create));
	qindex = qos_hal_get_queue_index_from_qid(phy_q);
	/* Here qindex EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
	if (qindex >= EGRESS_QUEUE_ID_MAX)
		return QOS_HAL_STATUS_ERR;
	q_param.index = qindex;
	q_param.egress_port_number = port;
	q_param.scheduler_input = (connected_sched << 3);
	q_param.prio = 0;
	q_param.iwgt = 0;

	if (qos_hal_queue_relink(&q_param) != QOS_HAL_STATUS_OK) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> <%d> Failed to relink the queue %d\n",
				__func__, __LINE__, phy_q);
		return QOS_HAL_STATUS_ERR;
	}

	memset(&prio_info, 0, sizeof(struct dp_node_prio));
	prio_info.inst = g_Inst;
	prio_info.id.q_id = phy_q;
	prio_info.type = DP_NODE_QUEUE;
	if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s><%d> Failed to get Prio of the node %d\n",
				__func__, __LINE__, phy_q);
	}
	prio_info.arbi = ARBITRATION_WRR;
	if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node %d\n",
				__func__, __LINE__, phy_q);
		return QOS_HAL_STATUS_ERR;
	}
	return connected_sched;
}

#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
int32_t qos_hal_port_conf_set(
		int32_t deq_port_id,
		struct qos_hal_port_cfg_info *port_cfg,
		int32_t flag)
{
	struct dp_port_cfg_info port_cfg_info = {0};

	if ((flag & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS)
		return QOS_HAL_STATUS_ERR;

	if (deq_port_id < 1)
		return QOS_HAL_STATUS_ERR;

	port_cfg_info.inst = 0;
	port_cfg_info.pid = deq_port_id;
	port_cfg_info.green_threshold = port_cfg->green_threshold;
	port_cfg_info.yellow_threshold = port_cfg->yellow_threshold;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> green_threshold [%d]\n",
				__func__, port_cfg_info.green_threshold);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> yellow_threshold [%d]\n",
				__func__, port_cfg_info.yellow_threshold);
	if (dp_qos_port_conf_set(&port_cfg_info, 0) == DP_FAILURE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>Fail to config port %d\n",
				__func__, deq_port_id);
		return QOS_HAL_STATUS_ERR;
	}
	return QOS_HAL_STATUS_OK;
}
#endif /*IS_ENABLED(CONFIG_X86_INTEL_LGM)*/
/****************************************************/

int qos_hal_get_q_res_from_netdevice(
		struct net_device *netdev,
		char *dev_name,
		uint32_t sub_if,
		dp_subif_t *dp_if,
		uint32_t sched_type,
		uint32_t *qos_ports_cnt,
		void **res,
		uint32_t flags)
{
	bool logical_dev = 0;
	uint32_t i = 0;
	uint32_t qos_port_index = 0;
	uint32_t ingress_port = 0;
	uint32_t no_of_qos_ports;
	uint32_t flag = 0;
	uint32_t netdev_def_q;
	int32_t nRet;
	dp_subif_t *dp_subif = NULL;
	dp_subif_t *p_dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;
	struct dp_dequeue_res dq_res = {0};
	struct dp_queue_res *qos_res = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *base_subif_index;
	struct qos_hal_user_subif_abstract *ingress_subif_index;
	struct net_device *base1;

	dq_res.inst = 0;
	dq_res.dev = netdev;

 	MEM_ALLOC(dp_subif_t, dp_subif);
	if ((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS) {
		if ((flags & DP_F_MPE_ACCEL) == DP_F_MPE_ACCEL)
			flag = DP_F_MPE_ACCEL;

		dq_res.dev = NULL;
		ingress_port = dp_if->port_id;
	} else {
		if (netdev) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Get the DP Port Id for netdevice %s\n",
					__func__, netdev->name);
			qos_mgr_br2684_get_vcc(netdev, &vcc);
			if (vcc == NULL)
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "VCC is NULL\n");

			if (dp_if->port_id != -1) {
				dp_subif->port_id = dp_if->port_id;
				dq_res.dev = NULL;
				g_Lookup_Mode = 1;
			} else if (!(flags & NETDEV_TYPE_TCONT)) {
				nRet = dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags);
				if ((dp_subif->alloc_flag & DP_F_VUNI) && (dp_subif->data_flag & DP_SUBIF_VANI)) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> PON interface\n",
							__func__);
					p_dp_subif =  kmalloc(sizeof(dp_subif_t), GFP_KERNEL);
					if (p_dp_subif == NULL) {
						kfree(dp_subif);
						return QOS_HAL_STATUS_ERR;
					}
					memset(p_dp_subif, 0, sizeof(dp_subif_t));

					if ((dp_get_netif_subifid(dp_subif->associate_netif, NULL, NULL, NULL,
									p_dp_subif, 0)) == QOS_MGR_SUCCESS) {
						dp_subif->subif = p_dp_subif->subif;
					}
					kfree(p_dp_subif);
				}
				else if (!((dp_subif->alloc_flag & DP_F_FAST_WLAN) == DP_F_FAST_WLAN) && (nRet != QOS_MGR_SUCCESS)) {
					if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
						if (qos_hal_get_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
							kfree(dp_subif);
							return QOS_HAL_STATUS_ERR;
						}
					}
					kfree(dp_subif);
					return QOS_HAL_STATUS_ERR;
				}

				dp_if->port_id = dp_subif->port_id;
				dp_if->subif = dp_subif->subif >> 8;
				g_Lookup_Mode = dp_subif->lookup_mode;
				netdev_def_q = dp_subif->def_qid;
				dp_if->def_qid = dp_subif->def_qid;
			}
		} else {
			dp_subif->port_id = dp_if->port_id;
			dp_subif->subif = dp_if->subif;
		}
	}
	dp_subif->subif  = dp_subif->subif >> 8;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> dp_subif->port_id =%d subif=%d\n",
			__func__, dp_subif->port_id, dp_subif->subif);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> lookup_mode =%d \n",
			__func__, g_Lookup_Mode);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> alloc_flag =%x \n",
			__func__, dp_subif->alloc_flag);
	g_Dp_Port = dp_subif->port_id;

	if (netdev) {
		if (is_vlan_dev(netdev)) { /* vlan device*/
			base1 = get_base_dev(netdev, 1);
			if (!base1) {
				pr_err("VLAN interface no base\n");
				kfree(dp_subif);
				return -1;
			}
			dq_res.dev = base1;
		}
	}
	dq_res.dp_port = dp_subif->port_id;
	dq_res.cqm_deq_idx = sub_if;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> dq_res.cqm_deq_idx =%d \n",
			__func__, dq_res.cqm_deq_idx);
	if (dp_deq_port_res_get(&dq_res, flags) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
				__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:%d No queue resources for netdevice [%s]\n",
				__func__, __LINE__, netdev->name);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "No of queue resources are ->[%d]\n",
			dq_res.num_q);
	dq_res.q_res = NULL;
	if (dq_res.num_q) {
		dq_res.q_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
		if (dq_res.q_res == NULL) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);
	}
	dq_res.q_res_size = dq_res.num_q;
	if (dp_deq_port_res_get(&dq_res, flag) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
				__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	*qos_ports_cnt = no_of_qos_ports = dq_res.num_deq_ports;
	*res = qos_res = dq_res.q_res;
	/*if (dq_res.q_res) {
	  for (n = 0; n < dq_res.num_q; n++) {
	  dump_q_res(qos_res + n);
	  }
	  }*/
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "No of DEQ Port:[%d] Max Port:%d\n",
			dq_res.num_deq_ports, g_No_Of_QOS_Res_Port);
	if (qos_res != NULL) {
		QOS_HAL_ASSERT(qos_res->cqm_deq_port > g_No_Of_QOS_Res_Port);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "QOS Port:[%d] CQM DEQ Port:[%d]\n",
				qos_res->qos_deq_port, qos_res->cqm_deq_port);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Qid : [%d]\n", qos_res->q_id);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Sch Lvl : [%d]\n", qos_res->sch_lvl);

		/*{
		  struct dp_node_child node_info = {0};
		  struct dp_node child_node = {0};

		  node_info.type = DP_NODE_PORT;
		  node_info.id.cqm_deq_port = qos_res->cqm_deq_port;

		  if (dp_children_get(&node_info, 0) == DP_FAILURE) {
		  QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to get the Children\n", __func__, __LINE__);
		  return QOS_HAL_STATUS_ERR;
		  }
		  for (i = 0; i < node_info.num; i++) {
		  child_node = node_info.child[i];
		  if (child_node.type == DP_NODE_QUEUE) {
		  printk("Qid:%d\n", child_node.id.q_id);
		  } else if (child_node.type == DP_NODE_SCH) {
		  printk("Sched:%d\n", child_node.id.sch_id);
		  }
		  }

		  printk("<%s>No of child node for Port:%d is %d\n", __func__, qos_res->cqm_deq_port, node_info.num);
		  }*/
	} else {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	for (i = 0; i < no_of_qos_ports; i++) {
		int32_t queue_idx = QOS_MGR_FAILURE;
		uint32_t sched_lvl = 0;
		uint32_t connected_sched = 0;

		queue_idx = (qos_res + i)->q_id;
		if ((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS) {
			base_subif_index = (qos_hal_user_sub_interface_ingress_info + ingress_port);
			qos_hal_get_subif_list_item(ingress_port,
					NULL,
					&ingress_subif_index,
					&base_subif_index->head);
			qos_port_index = ingress_subif_index->qos_port_idx;
		} else {
			base_subif_index = (qos_hal_user_sub_interface_info + (qos_res + i)->cqm_deq_port);
			qos_hal_get_subif_list_item((qos_res + i)->cqm_deq_port,
					netdev,
					&subif_index,
					&base_subif_index->head);
			qos_port_index = subif_index->qos_port_idx;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_subif_index: %p, base_subif_index->head:%p subif_index:%p\n",
					__func__, base_subif_index, base_subif_index->head, subif_index);
		}

		if (qos_port_index  == 0xFF) {
			struct dp_qos_link qoslink = {0};
			struct dp_node_prio prio_info = {0};
#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
			struct qos_hal_port_cfg_info port_cfg = {0};
#endif /*IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

			qoslink.q_id = (qos_res+i)->q_id;
			if (dp_link_get(&qoslink, 0) == DP_FAILURE) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get DP link\n",
						__func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "Arbitration of Parent Node : [%d] is [%d]\n",
					(qos_res + i)->cqm_deq_port,
					qoslink.q_arbi);

			if ((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS) {
				ingress_subif_index->is_logical = (dp_subif->subif == 0) ? 0 : 1;
				ingress_subif_index->qos_port_idx = qos_res->cqm_deq_port;
				if ((qos_res+i)->sch_lvl != 0) {
					ingress_subif_index->base_sched_id_egress = qos_res->sch_id[0];
					ingress_subif_index->user_queue[0].sbin = qos_res->sch_id[0] << 3;
					connected_sched = (qos_res+i)->sch_id[0];

					qos_hal_sched_track[connected_sched].in_use = 1;
					qos_hal_sched_track[connected_sched].id = (qos_res+i)->sch_id[0];
				} else {
					ingress_subif_index->base_sched_id_egress = 0xFF;
					ingress_subif_index->user_queue[0].sbin = 0xFF << 3;
				}
				ingress_subif_index->user_queue[0].queue_index = (qos_res+i)->q_id;
				ingress_subif_index->user_queue[0].queue_type = qoslink.q_arbi;
				ingress_subif_index->user_queue[0].qid = 0;
				ingress_subif_index->user_queue[0].prio_level = 0;
				ingress_subif_index->user_queue[0].qmap[0] = 0;
				ingress_subif_index->default_q = (qos_res+i)->q_id;
				ingress_subif_index->qid_last_assigned = 0;
				ingress_subif_index->queue_cnt++;

			} else {
				subif_index->netdev = netdev;
				subif_index->dp_subif.port_id = dp_subif->port_id;
				subif_index->dp_subif.subif = dp_subif->subif;
				subif_index->qos_port_idx = (qos_res+i)->cqm_deq_port;
				subif_index->lookup_mode = dp_subif->lookup_mode;

				if (netdev)
					logical_dev = is_vlan_dev(netdev);/* vlan device*/

				if ((dp_subif->alloc_flag & DP_F_FAST_WLAN) == DP_F_FAST_WLAN) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "For wlan netdevice %s\n", netdev->name);
					logical_dev = 1;						
				}

				if (logical_dev) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> For logical netdevice %s\n", __func__, netdev->name);
					subif_index->is_logical = 1;
					subif_index->base_sched_id_egress = 0xFF;
					subif_index->port_sched_in_user_lvl = QOS_HAL_MAX_PRIORITY_LEVEL;
					subif_index->port_sched_in_egress = INVALID_SCHEDULER_INPUT_ID;
					subif_index->port_shaper_sched = INVALID_SCHED_ID;
					subif_index->shaper_idx = INVALID_SHAPER_ID;
					subif_index->default_q = 0;
				} else {
					/** For the physical port */
					subif_index->is_logical = 0;
					if ((qos_res+i)->sch_lvl != 0) {
						subif_index->base_sched_id_egress = (qos_res+i)->sch_id[0];
						connected_sched = (qos_res+i)->sch_id[0];

						qos_hal_sched_track[connected_sched].in_use = 1;
						qos_hal_sched_track[connected_sched].id = (qos_res+i)->sch_id[0];
						sched_lvl++;
					} else {
						subif_index->base_sched_id_egress = 0xFF;
						subif_index->user_queue[0].sbin = 0xFF << 3;
					}
					subif_index->user_queue[0].queue_index = (qos_res+i)->q_id;
					subif_index->user_queue[0].queue_type = qoslink.q_arbi;
					subif_index->user_queue[0].prio_level = QOS_HAL_MAX_PRIORITY_LEVEL;
					subif_index->user_queue[0].qid = 0;
					subif_index->user_queue[0].qmap[0] = 0;
					subif_index->qid_last_assigned = 0;
					subif_index->default_q = (qos_res+i)->q_id;
					subif_index->default_prio = QOS_HAL_MAX_PRIORITY_LEVEL;
					subif_index->queue_cnt++;
				}
			}
			if (!((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS)) {
				if (base_subif_index->qos_port_idx  == 0xFF) {
					/* configuring the arbitration of port node to WSP */
					prio_info.inst = g_Inst;
					prio_info.id.q_id = (qos_res+i)->q_id;
					prio_info.type = DP_NODE_QUEUE;
					if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

						QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Fail to get Prio of node %d\n",
								__func__, __LINE__, (qos_res+i)->q_id);
						kfree(dp_subif);
						return QOS_MGR_FAILURE;
					}
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue [%d] has arbi:[%d]\n",
							__func__, (qos_res+i)->q_id, prio_info.arbi);

					prio_info.arbi = ARBITRATION_WSP;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Change Parent arbi to:[%d]\n",
							__func__, prio_info.arbi);
					if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

						QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbi of node %d\n",
								__func__, __LINE__, (qos_res+i)->q_id);
						kfree(dp_subif);
						return QOS_MGR_FAILURE;
					}
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue [%d] has Priority [%d] arbitration: [%d]\n",
							__func__, (qos_res+i)->q_id, prio_info.prio_wfq, prio_info.arbi);
					prio_info.prio_wfq = 7;
					if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set prio of node %d\n",
								__func__, __LINE__, (qos_res+i)->q_id);
						kfree(dp_subif);
						return QOS_MGR_FAILURE;
					}
				}

				if (!logical_dev) {
					if (qos_hal_queue_track[(qos_res + i)->q_id].is_enabled == 0)
						queue_idx = (qos_res + i)->q_id;
					else
						queue_idx = qos_hal_get_free_queue_index();

					if (queue_idx == QOS_MGR_FAILURE) {
						kfree(dp_subif);
						return QOS_MGR_FAILURE;
					}
					qos_hal_queue_track[queue_idx].is_enabled = 1;
					qos_hal_queue_track[queue_idx].queue_id = (qos_res + i)->q_id;

					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Queue Index:Qid --> [%d:%d]\n",
							__func__, queue_idx, (qos_res + i)->q_id);


#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
					port_cfg.netdev = netdev;
					port_cfg.green_threshold = 4*1024*1024;
					port_cfg.yellow_threshold = 2*1024*1024;

					if (qos_hal_port_conf_set(
								(qos_res+i)->cqm_deq_port,
								&port_cfg,
								0) == QOS_HAL_STATUS_ERR) {

						kfree(dp_subif);
						return QOS_MGR_FAILURE;
					}
#endif /*IS_ENABLED(CONFIG_X86_INTEL_LGM)*/

					/* Port cannot be configured in DRR mode. So scheduler node needs
					 * to be connected to port and configure the scheduler node to DRR.
					 * It is applicable for SP also. By default scheduler will be
					 * configured when first queue is added.
					 */
					if ((qos_res+i)->sch_lvl == 0) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "sch_lvl=0 and policy=%d\n",
								sched_type);
						if ((flags & QOS_MGR_Q_F_SP_WFQ) == QOS_MGR_Q_F_SP_WFQ)
							sched_type = QOS_HAL_POLICY_WSP;
						connected_sched = create_new_scheduler(
								(qos_res+i)->cqm_deq_port,
								1,
								sched_type,
								0,
								1);
						if (connected_sched == QOS_HAL_STATUS_ERR) {
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Failed to add scheduler\n",
									__func__);
							kfree(dp_subif);
							return QOS_MGR_FAILURE;

						}
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> connected_sched:[%d]\n",
								__func__, connected_sched);
						subif_index->base_sched_id_egress = connected_sched;
						qos_hal_sched_track[connected_sched].leaf_mask = 0x00;
						sched_lvl++;
					} else if ((qos_res+i)->sch_lvl == 1)
						qos_hal_sched_track[connected_sched].leaf_mask = 0x01;
					else {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Wrong [%d] level of scheduler node from DP\n",
								__func__, (qos_res+i)->sch_lvl);
						kfree(dp_subif);
						return QOS_MGR_FAILURE;
					}
				}
			}
		}
		if (base_subif_index->qos_port_idx  == 0xFF) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Changing configuration of base subif\n");
			base_subif_index->netdev = netdev;
			base_subif_index->qos_port_idx = (qos_res+i)->cqm_deq_port;
			if ((qos_res+i)->sch_lvl != 0)
				base_subif_index->base_sched_id_egress = (qos_res+i)->sch_id[0];
			else
				base_subif_index->base_sched_id_egress = connected_sched;

			base_subif_index->default_q = (qos_res+i)->q_id;
			if (subif_index != NULL)
				subif_index->default_q = base_subif_index->default_q;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_subif_index: %p, base_subif_index->base_sched_id_egress:%d\n",
					__func__, base_subif_index, base_subif_index->base_sched_id_egress);
		} else {
			if (logical_dev) {
				subif_index->default_q = base_subif_index->default_q;
			}
		}

		if (qos_hal_port_track[(qos_res + i)->cqm_deq_port].is_enabled  == 0) {

			struct dp_qos_link qoslink = {0};

			qoslink.q_id = (qos_res+i)->q_id;
			if (dp_link_get(&qoslink, 0) == DP_FAILURE) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get DP link\n",
						__func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Arbitration of Parent Node :[%d] is [%d]\n",
					__func__,
					(qos_res + i)->cqm_deq_port,
					qoslink.q_arbi);

			/** Update the Port info*/
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].is_enabled = 1;
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].input_sched_id = 0xFF;
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].default_sched_id = 0xFF;
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].shaper_sched_id = 0xFF;
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].port_sched_id = 0xFF;
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].no_of_queues = 1;
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].policy = ARBITRATION_WSP;
			qos_hal_port_track[(qos_res + i)->cqm_deq_port].tbs = INVALID_SHAPER_ID;

			if (sched_lvl) {
				struct dp_node_link sch_info = {0};

				sch_info.node_type = DP_NODE_SCH;
				sch_info.node_id.sch_id = qos_hal_sched_track[connected_sched].id;

				if (dp_node_link_get(&sch_info, 0) == DP_FAILURE) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s><%d> Failed to get %d scheduler node info\n",
							__func__, __LINE__, sch_info.node_id.sch_id);
				}

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s><%d> scheduler node: %d arbi %d\n",
						__func__, __LINE__, sch_info.node_id.sch_id, sch_info.arbi);
				/** Update the Scheduler info*/
				qos_hal_sched_track[connected_sched].level = 0;
				qos_hal_sched_track[connected_sched].policy = sched_type;
				qos_hal_sched_track[connected_sched].next_sched_id = 0xFF;
				qos_hal_sched_track[connected_sched].peer_sched_id = 0xFF;
				qos_hal_sched_track[connected_sched].sched_out_conn = 0; /* default connected to the port */
				qos_hal_sched_track[connected_sched].omid = (qos_res+i)->cqm_deq_port;
				qos_hal_sched_track[connected_sched].tbs = INVALID_SHAPER_ID;

				qos_hal_port_track[(qos_res + i)->cqm_deq_port].input_sched_id = connected_sched;
				qos_hal_port_track[(qos_res + i)->cqm_deq_port].port_sched_id = connected_sched;
				if (qos_res->sch_lvl != 0)
					qos_hal_port_track[(qos_res + i)->cqm_deq_port].default_sched_id =
									(qos_res+i)->sch_id[0];

			} else {
				qos_hal_port_track[(qos_res + i)->cqm_deq_port].leaf_mask = 0x01;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "Configuring leaf_mask %d for port %d\n",
						qos_hal_port_track[(qos_res + i)->cqm_deq_port].leaf_mask,
						(qos_res + i)->cqm_deq_port);
			}

			/** Update the Queue info*/
			qos_hal_queue_track[queue_idx].epn = (qos_res + i)->cqm_deq_port;
			qos_hal_queue_track[queue_idx].q_type = sched_type;
			qos_hal_queue_track[queue_idx].prio_weight = 0;
			qos_hal_queue_track[queue_idx].attach_to = QOS_HAL_INPUT_TO_PORT;
			if (qos_res->sch_lvl != 0) {
				qos_hal_queue_track[queue_idx].attach_to = QOS_HAL_INPUT_TO_SCHED;
				qos_hal_queue_track[queue_idx].connected_to_sched_id = (qos_res + i)->sch_id[0];
			}
			if (!((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS)) {
				if (subif_index != NULL) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "<%s:%d> Updating user queue queue_idx:%d\n", __func__, __LINE__, queue_idx);
					subif_index->default_q = queue_idx;
					subif_index->user_queue[0].queue_index = queue_idx;
				}
			}
		} /* loop for .default_sched_id=0xFF */
	} /* loop for no_of_qos_ports */

	kfree(dp_subif);
	return QOS_HAL_STATUS_OK;
}

/** =========================  Initialize all the list ================= */

/* This function updates all the list with the QOS reserved resources
*/
int qos_hal_ll_init(void)
{
	uint32_t port, queue, sched;
	uint32_t no_of_qos_ports;
	int32_t i;
	int32_t ret = QOS_HAL_STATUS_OK;
	struct dp_cap cap = {0};
	struct dp_dequeue_res qos_res = {0};

	cap.inst = 0;
	if (dp_get_cap(&cap, 0) == DP_FAILURE) {
		pr_debug("Failed to get capability\n");
		return QOS_HAL_STATUS_ERR;
	}

	port = cap.max_num_deq_ports;
	sched = 0;
	queue = 1;
	if (port == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "No of Ports cannot be 0\n");
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> QOS RESERVED RESOURCES Port: [%d] Scheduler: [%d]  Queue: [%d]\n", __func__, port, sched, queue);

	g_No_Of_QOS_Res_Port = port;
	g_No_Of_QOS_Res_Scheduler = sched;
	g_No_Of_QOS_Res_Queue = queue;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "************************dp_reserve=%d*****************\n", dp_reserve);
	if (dp_reserve) {
		g_No_Of_QOS_Res_Queue = queue + (QOS_HAL_MAX_DIRECTPATH_PORT * 2);
		g_No_Of_QOS_Res_Scheduler = sched + (QOS_HAL_MAX_DIRECTPATH_PORT + 2);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> g_No_Of_QOS_Res_Queue =%d g_No_Of_QOS_Res_Scheduler=%d\n", __func__, g_No_Of_QOS_Res_Queue,
				g_No_Of_QOS_Res_Scheduler);
		g_No_Of_QOS_Res_Port = port + QOS_HAL_MAX_DIRECTPATH_PORT;
	}
	g_No_Of_QOS_Res_Port = port;
	g_No_Of_Ingress_Interface = port;

	pr_debug("Total node : [%d]  and size of each node: [%zu]\n",
			g_No_Of_QOS_Res_Port,
			sizeof(struct qos_hal_user_subif_abstract));
	pr_debug("Total allocated size= [%zu]\n",
		g_No_Of_QOS_Res_Port * sizeof(struct qos_hal_user_subif_abstract));
	/* User Abstraction list for Egress Queues */
	qos_hal_user_sub_interface_info = (struct qos_hal_user_subif_abstract *)
		alloc_pages_exact(((g_No_Of_QOS_Res_Port+1) * sizeof(struct qos_hal_user_subif_abstract)), GFP_KERNEL);
	if (!qos_hal_user_sub_interface_info) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "Failed to allocate memory for Egress Queue Management Table\n");
		return QOS_HAL_STATUS_ERR;
	}
	pr_debug("qos_hal_user_sub_interface_info: %p\n", qos_hal_user_sub_interface_info);
	memset(qos_hal_user_sub_interface_info, 0,
		(g_No_Of_QOS_Res_Port+1) * sizeof(struct qos_hal_user_subif_abstract));

	for (i = 0; i <= g_No_Of_QOS_Res_Port + 1; i++) {
		(qos_hal_user_sub_interface_info + i)->qos_port_idx = 0xFF;
	}

	/* User Abstraction list for Ingress Queues */
	qos_hal_user_sub_interface_ingress_info = (struct qos_hal_user_subif_abstract *)
		alloc_pages_exact((g_No_Of_Ingress_Interface + 1) * sizeof(struct qos_hal_user_subif_abstract), GFP_KERNEL);
	if (!qos_hal_user_sub_interface_ingress_info) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "Failed to allocate memory for Ingress Queue Management Table\n");
		return QOS_HAL_STATUS_ERR;
	}
	pr_debug("Total node : %d  and size of each node: %zu\n",
			g_No_Of_Ingress_Interface,
			sizeof(struct qos_hal_user_subif_abstract));
	memset(qos_hal_user_sub_interface_ingress_info, 0,
		(g_No_Of_Ingress_Interface + 1) * sizeof(struct qos_hal_user_subif_abstract));

	for (i = 0; i <= port+1; i++) {
		qos_hal_port_track[i].is_enabled = 0;
		qos_hal_port_track[i].input_sched_id = 0xFF;
		qos_hal_port_track[i].default_sched_id = 0xFF;
		qos_hal_port_track[i].shaper_sched_id = 0xFF;
		qos_hal_port_track[i].port_sched_id = 0xFF;
		qos_hal_port_track[i].tbs = INVALID_SHAPER_ID;

		(qos_hal_user_sub_interface_info + i)->base_sched_id_egress = i;
	}
	for (i = 0; i <= g_No_Of_QOS_Res_Scheduler; i++) {
		qos_hal_sched_track[i].in_use = 1;
		qos_hal_sched_track[i].id = i;
		qos_hal_sched_track[i].level = 0;
		qos_hal_sched_track[i].policy = QOS_HAL_POLICY_WSP;
		qos_hal_sched_track[i].next_sched_id = INVALID_SCHED_ID;
		qos_hal_sched_track[i].peer_sched_id = INVALID_SCHED_ID;
		qos_hal_sched_track[i].sched_out_conn = 0; /* default connected to the port */
		qos_hal_sched_track[i].omid = i;
		qos_hal_sched_track[i].tbs = INVALID_SHAPER_ID;
		qos_hal_sched_track[i].priority_selector = 0;
	}
	for (i = 0; i <= g_No_Of_QOS_Res_Queue; i++) {
		qos_hal_queue_track[i].is_enabled = 0;
		qos_hal_queue_track[i].connected_to_sched_id = INVALID_SCHED_ID;
		qos_hal_queue_track[i].attach_to = QOS_HAL_INPUT_TO_SCHED;
		qos_hal_queue_track[i].q_type = QOS_HAL_POLICY_WFQ;
		qos_hal_queue_track[i].prio_weight = 0;
		qos_hal_queue_track[i].tb_index = INVALID_SHAPER_ID;
	}

	/* Blocked for Drop queue */
	qos_hal_queue_track[0].is_enabled = 0;
	qos_hal_queue_track[0].queue_id = 0;

	/* Get the QOS resources for CPU */
	qos_res.inst = 0;
	qos_res.dp_port = 0;
	qos_res.q_res = NULL;
	if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get DP Dequeue Resources\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}
	if (qos_res.num_q == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d No queue for CPU resources\n", __func__, __LINE__);
	} else {
		struct qos_hal_equeue_create q_create = {0};
		struct dp_node_prio prio_info = {0};
#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
		uint32_t q_map_mask = 0;
		struct dp_shaper_conf shaper;
		struct dp_queue_map_entry q_map = {0};
#endif

		g_CPU_Sched = 0xFF;
		qos_res.q_res = kmalloc(sizeof(struct dp_queue_res) * qos_res.num_q, GFP_KERNEL);
		if (qos_res.q_res == NULL) {
			return QOS_HAL_STATUS_ERR;
		}
		qos_res.q_res->sch_id[0] = 0xFF;
		qos_res.q_res_size = qos_res.num_q;
		if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCE_FREE;
		}
		no_of_qos_ports = qos_res.num_deq_ports;

		g_CPU_PortId = qos_res.q_res->cqm_deq_port;
		g_CPU_Sched = qos_res.q_res->sch_id[0];
		g_CPU_Queue = qos_res.q_res->q_id;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Number of CPU QoS Ports are: [%d]\n", __func__, no_of_qos_ports);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> CPU QoS resources are Port: [%d] ,Q: [%d]\n", __func__, g_CPU_PortId, g_CPU_Queue);

		if (qos_res.q_res->sch_lvl != 0) {
			QOS_HAL_DEBUG_MSG(QOS_ENABLE_ALL_DEBUG, "<%s> CPU QoS resources are SB: [%d]\n", __func__, g_CPU_Sched);
		}

		qos_hal_queue_track[qos_res.q_res->q_id].is_enabled = 1;
		qos_hal_queue_track[qos_res.q_res->q_id].queue_id = qos_res.q_res->q_id;
		qos_hal_queue_track[qos_res.q_res->q_id].epn = g_CPU_PortId;

		q_create.scheduler_input =  g_CPU_Sched << 3;

		/** configuring the arbitration of port/sched node to WSP */
		prio_info.inst = g_Inst;
		prio_info.id.q_id = qos_res.q_res->q_id;
		prio_info.type = DP_NODE_QUEUE;
		if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node %d\n",
					__func__, __LINE__, qos_res.q_res->q_id);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCE_FREE;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue [%d] has arbitration: [%d]\n",
				__func__, qos_res.q_res->q_id, prio_info.arbi);

		prio_info.arbi = ARBITRATION_WSP;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Change Parent arbitration to:[%d]\n",
				__func__, prio_info.arbi);
		if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node %d\n",
					__func__, __LINE__, qos_res.q_res->q_id);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCE_FREE;
		}

#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
		if (dp_qos_link_prio_get(&prio_info, 0) == DP_FAILURE) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to get Prio of the node %d\n",
					__func__, __LINE__, qos_res.q_res->q_id);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCE_FREE;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue [%d] has priority: [%d]\n",
				__func__, qos_res.q_res->q_id, prio_info.prio_wfq);

		prio_info.prio_wfq = 7;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Change priority to:[%d]\n",
				__func__, prio_info.prio_wfq);
		if (dp_qos_link_prio_set(&prio_info, 0) == DP_FAILURE) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set arbitration of the node %d\n",
					__func__, __LINE__, qos_res.q_res->q_id);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCE_FREE;
		}

		/* 
		 * Add shaper of 10Mbps on CPU port default Q.
		 */
		q_map.qmap.dp_port = 0;

		q_map.qmap.mpe1 = 0;
		q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
			CBM_QUEUE_MAP_F_DE_DONTCARE |
			CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
			CBM_QUEUE_MAP_F_SUBIF_DONTCARE |
			CBM_QUEUE_MAP_F_MPE2_DONTCARE ;

		qos_hal_add_q_map(qos_res.q_res->q_id, g_CPU_PortId, &q_map, q_map_mask);

		/* Add shaper. Shape the low prio queue to 10 Mbps */
		memset(&shaper, 0, sizeof(shaper));
		shaper.type = DP_NODE_QUEUE;
		shaper.id.q_id = qos_res.q_res->q_id;
		shaper.cir = 10000;
		shaper.pir = 10000;
		if (dp_shaper_conf_set(&shaper, 0) == DP_FAILURE) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> %d Failed to set shaper config for queue node %d\n",
					__func__, __LINE__, qos_res.q_res->q_id);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCE_FREE;
		}
#endif

QOS_RESOURCE_FREE:
		kfree(qos_res.q_res);
	}
	return ret;
}
