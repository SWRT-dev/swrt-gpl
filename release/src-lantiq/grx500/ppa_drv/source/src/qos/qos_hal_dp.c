/*******************************************************************************
 **
 ** FILE NAME    : qos_hal_dp.c
 ** PROJECT      : QOS HAL
 ** MODULES      : (QoS Engine )
 **
 ** AUTHOR       : Purnendu Ghosh
 ** DESCRIPTION  : QOS HAL Layer
 ** COPYRIGHT : Copyright (c) 2020, MaxLinear, Inc.
 ** Copyright (c) 2017 - 2018 Intel Corporation
 ** Copyright (c) 2010 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
 **
 *******************************************************************************/

/*
 * Driver Header files
 */
#include "qos_mgr_stack_al.h"
#include "qos_mgr_api.h"
#include <uapi/net/qos_mgr_common.h>
#include <net/qos_mgr/qos_hal_api.h>
#include <net/qos_mgr/qos_mgr_hook.h>
#include <net/qos_mgr/qos_mgr_tc_hook.h>
#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
#include <net/intel_cbm_api.h>
#else
#include <net/lantiq_cbm_api.h>
#endif
#include <net/datapath_api.h>
#include <net/datapath_api_qos.h>
#include "qos_hal.h"
#include "qos_hal_debug.h"
#include "qos_hal_ll.h"

/**
 * version No
 */
#define VER_MAJOR       1
#define VER_MID         0
#define VER_MINOR       0

#define QOS_HAL_QUEUE_SHAPER_INGRESS_MPE   0x0F000000

#define QOS_HAL_NO_PRIO_MATCH 0xFFFFFFF0
#define QUEUE_FLUSH_SUPPORT
#define PPPOA_SUPPORT
#define WRAPAROUND_32_BITS 0xFFFFFFFF
#define QOS_HAL TMU_HAL
extern int g_qos_mgr_proc_dir_flag;
uint32_t high_prio_q_limit = 16;
uint32_t dp_reserve;
module_param(dp_reserve, int, 0000);
module_param(high_prio_q_limit, int, 0000);

uint32_t g_Inst;
uint32_t g_qos_dbg = QOS_DEBUG_ERR;

/* Directpath Egress */
uint32_t g_DpEgressQueueScheduler;
uint32_t g_Root_sched_id_DpEgress = 0xFF;

/* Directpath Ingress */
uint32_t g_IngressQueueScheduler;
uint32_t g_Root_sched_id_Ingress = 0xFF;

/* Directpath Ingress QOS Resources */
uint32_t g_Port_id_DpIngress;
uint32_t g_Queue_id_DpIngress;
uint32_t g_DpIngressQueueScheduler = 0xFF;

/** PPA Callback to delete ppv4 hw session when a queue is deleted */
int32_t (*pSess_del_cb)(uint32_t qid) = NULL;

enum qos_hal_errorcode qos_hal_flush_queue_if_qocc(struct net_device *netdev, char *dev_name, int32_t index, int32_t cqm_deq_port, int32_t remap_to_qid, uint32_t flags);
static int qos_hal_add_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags);
static int qos_hal_del_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags);

static int
qos_hal_add_queue(
		struct net_device *netdev,
		char *dev_name,
		char tc[MAX_TC_NUM],
		uint8_t no_of_tc,
		uint32_t schedid,
		uint32_t prio_type,
		uint32_t prio_level,
		uint32_t weight,
		uint32_t flowId,
		QOS_Q_ADD_CFG *param,
		uint32_t flags);

static int qos_hal_delete_queue(
		struct net_device *netdev,
		char *dev_name,
		uint32_t index,
		uint32_t priority,
		uint32_t scheduler_id,
		QOS_Q_DEL_CFG *param,
		uint32_t flags);

int qos_hal_modify_queue(struct net_device *netdev, QOS_Q_MOD_CFG *param);
int qos_hal_add_shaper_index(QOS_RATE_SHAPING_CFG *cfg);
int qos_hal_add_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, int32_t qos_q_idx, uint32_t flags);
int qos_hal_del_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, int32_t qos_q_idx, uint32_t flags);
int qos_hal_get_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags);
int qos_hal_get_user_index_from_qid(struct qos_hal_user_subif_abstract *subif_index, uint32_t qid);
enum qos_hal_errorcode qos_hal_scheduler_cfg_get(uint32_t index);

static int qos_hal_dp_egress_root_create(struct net_device *netdev);
static int qos_hal_dp_port_resources_get(uint32_t dp_port, struct qos_hal_dp_res_info *res_p);
static int qos_hal_get_queue_num(struct net_device *netdev, int32_t portid, int32_t *q_num);

/** Buffers to track the Scheduler info */
struct qos_hal_sched_track_info  qos_hal_sched_track[QOS_HAL_MAX_SCHEDULER];
/** Buffers to track the Port info */
struct qos_hal_port_track_info qos_hal_port_track[QOS_HAL_MAX_EGRESS_PORT];
/** Buffers to track the Queue info */
struct qos_hal_queue_track_info qos_hal_queue_track[QOS_HAL_MAX_EGRESS_QUEUES];
/** Buffers to track the Shaper info */
struct qos_hal_shaper_track_info qos_hal_shaper_track[QOS_HAL_MAX_SHAPER];
/** Buffers to track the User level request for Egress Queue  */
struct qos_hal_user_subif_abstract *qos_hal_user_sub_interface_info ;
/** Buffers to track the User level request for Ingress Queue  */
struct qos_hal_user_subif_abstract *qos_hal_user_sub_interface_ingress_info ;
/** Directpath resource buffer */
struct qos_hal_dp_res_info qos_hal_dp_res[16];
/** queue weights */
extern uint16_t qos_hal_equeue_weight[QOS_HAL_MAX_EGRESS_QUEUES];

struct qos_hal_qos_mib g_csum_ol_mib = {0};
int32_t qos_hal_get_qos_mib1(struct net_device *netdev, uint32_t portid, uint32_t queueid, QOS_MGR_MIB *mib, uint32_t flag);
int32_t
qos_hal_get_qos_mib(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct qos_hal_qos_stats *qos_mib,
		uint32_t flag);
int32_t
qos_hal_modify_subif_to_port(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port_id,
		uint32_t priority,
		uint32_t weight,
		uint32_t flags);
int32_t qos_hal_set_checksum_queue_map(uint32_t pmac_port);

int32_t get_qos_status(QOS_MGR_STATUS *stat)
{
	return QOS_MGR_SUCCESS;
}

/*================================================================================================ */

int32_t qos_hal_generic_hook(QOS_MGR_GENERIC_HOOK_CMD cmd, void *buffer, uint32_t flag)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "qos_hal_generic_hook cmd 0x%x\n", cmd);
	switch (cmd)  {
	case QOS_MGR_GENERIC_HAL_INIT: {
		return QOS_MGR_SUCCESS;
	}
	case QOS_MGR_GENERIC_HAL_EXIT:
		return QOS_MGR_SUCCESS;

	case QOS_MGR_GENERIC_HAL_QOS_INIT_CFG: {
		uint32_t res = QOS_MGR_SUCCESS;

		res = qos_hal_qos_init_cfg();
		return res;
	}
	case QOS_MGR_GENERIC_HAL_QOS_UNINIT_CFG: {
		uint32_t res = QOS_MGR_SUCCESS;

		res = qos_hal_qos_uninit_cfg();
		return res;
	}
	case QOS_MGR_GENERIC_HAL_GET_HAL_VERSION: {
		QOS_MGR_VERSION *v = (QOS_MGR_VERSION *)buffer;

		v->major = VER_MAJOR;
		v->mid = VER_MID;
		v->minor = VER_MINOR;

		return QOS_MGR_SUCCESS;
	}
	case QOS_MGR_GENERIC_HAL_GET_QOS_STATUS: {
		QOS_MGR_STATUS *buffer;

		return get_qos_status(buffer);
	}
	case QOS_MGR_GENERIC_HAL_GET_QOS_QUEUE_NUM: {
		int32_t q_count = 0, res = QOS_MGR_FAILURE;
		struct net_device *if_dev = NULL;
		QOS_MGR_COUNT_CFG *cfg;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "***QOS_MGR_GENERIC_HAL_GET_QOS_QUEUE_NUM***\n");
		cfg = (QOS_MGR_COUNT_CFG *) buffer;
		res = qos_hal_get_queue_num(if_dev, cfg->portid, &q_count);
		cfg->num = q_count;
		return res;
	}
	case QOS_MGR_GENERIC_HAL_GET_QOS_MIB: {
		struct net_device *if_dev = NULL;
		int32_t ret = QOS_HAL_STATUS_OK;
		QOS_MGR_MIB_INFO *mib_info = (QOS_MGR_MIB_INFO *)buffer;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_GET_QOS_MIB***\n");
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> inf =%s\n",
					__func__, mib_info->ifname);

		if_dev = dev_get_by_name(&init_net, mib_info->ifname);
		if (if_dev == NULL) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");
			return QOS_MGR_FAILURE;
		}

		ret = qos_hal_get_qos_mib1(if_dev, mib_info->portid, mib_info->queueid,
						&mib_info->mib, mib_info->flag);
		if (if_dev)
			dev_put(if_dev);
		return ret;
	}
	case QOS_MGR_GENERIC_HAL_QOS_ADDQUE_CFG: {
		struct net_device *if_dev = NULL;
		int32_t i = 0;
		QOS_Q_ADD_CFG *cfg;
		cfg = (QOS_Q_ADD_CFG *) buffer;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_QOS_ADDQUE_CFG***\n");
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> inf=%s dev_name=%s\n", __func__,
					cfg->ifname, cfg->dev_name);

		if_dev = dev_get_by_name(&init_net, cfg->ifname);
		if (if_dev == NULL) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");
			return QOS_MGR_FAILURE;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "name=%s index=%d mtu=%u\n",
				if_dev->name, if_dev->ifindex, if_dev->mtu);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "InterfaceId: %d intfId_en:%d \n",
				cfg->intfId, cfg->intfId_en);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "No Of Tc:%d\n", cfg->tc_no);

		for (i = 0; i < cfg->tc_no; i++) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, " tc[%d]: %d\n",
						i, cfg->tc_map[i]);
		}
		cfg->q_id = qos_hal_add_queue(if_dev,
				cfg->dev_name,
				cfg->tc_map,
				cfg->tc_no,
				cfg->intfId_en,
				cfg->q_type,
				cfg->priority,
				cfg->weight,
				cfg->intfId,
				cfg, cfg->flags);

		if (if_dev)
			dev_put(if_dev);
		return QOS_MGR_SUCCESS;
	}
	case QOS_MGR_GENERIC_HAL_QOS_MODQUE_CFG: {
		struct net_device *if_dev = NULL;
		int32_t ret = QOS_MGR_SUCCESS;
		QOS_Q_MOD_CFG *cfg;

		cfg = (QOS_Q_MOD_CFG *)buffer;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_QOS_MODQUE_CFG***\n");

		if_dev = dev_get_by_name(&init_net, cfg->ifname);
		if (if_dev == NULL) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");
			return QOS_MGR_FAILURE;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "name=%s index=%d mtu=%u\n",
				if_dev->name, if_dev->ifindex, if_dev->mtu);

		if (qos_hal_modify_queue(if_dev, cfg) != QOS_HAL_STATUS_OK)
			ret = QOS_MGR_FAILURE;

		if (if_dev)
			dev_put(if_dev);
		return ret;
	}
	case QOS_MGR_GENERIC_HAL_QOS_DELQUE_CFG: {
		struct net_device *if_dev = NULL;
		int32_t ret = QOS_HAL_STATUS_OK;
		char *dev_name;
		QOS_Q_DEL_CFG *cfg;

		cfg = (QOS_Q_DEL_CFG *)buffer;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_QOS_DELQUE_CFG***\n");
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "interface=%s dev_name=%s queue id=%d flags=%d\n",
					cfg->ifname, cfg->dev_name, cfg->q_id, cfg->flags);

		if ((cfg->flags & QOS_MGR_F_PPPOATM) != QOS_MGR_F_PPPOATM) {
			if_dev = dev_get_by_name(&init_net, cfg->ifname);
			if (if_dev == NULL) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");
				return QOS_MGR_FAILURE;
			}
		} else
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>PPPOATM Intf:%s\n",
						__func__, cfg->dev_name);
		if (if_dev != NULL) {
			if ((cfg->flags & QOS_MGR_F_PPPOATM) != QOS_MGR_F_PPPOATM)
				dev_name = if_dev->name;
			else
				dev_name = cfg->dev_name;
		} else
			dev_name = cfg->dev_name;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Priority=%d\n",
					__func__, cfg->priority);

		ret = qos_hal_delete_queue(if_dev, dev_name, cfg->q_id,
					cfg->priority, 0, cfg, cfg->flags);
		if (if_dev)
			dev_put(if_dev);
		return ret;
	}
	case QOS_MGR_GENERIC_HAL_MOD_SUBIF_PORT_CFG: {
		struct net_device *if_dev = NULL;
		int32_t ret = QOS_MGR_SUCCESS;
		QOS_MOD_SUBIF_PORT_CFG *cfg;

		cfg = (QOS_MOD_SUBIF_PORT_CFG *)buffer;
		printk("mod_subif: interface=%s priority=%d weight=%d\n",
			cfg->ifname, cfg->priority_level, cfg->weight);

		if_dev = dev_get_by_name(&init_net, cfg->ifname);
		if (if_dev == NULL) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");
			return QOS_MGR_FAILURE;
		}

		ret = qos_hal_modify_subif_to_port(if_dev, if_dev->name,
					cfg->port_id, cfg->priority_level,
					cfg->weight, cfg->flags);
		if (if_dev)
			dev_put(if_dev);
		return ret;
	}
	case QOS_MGR_GENERIC_HAL_SET_QOS_SHAPER_CFG: {
		QOS_RATE_SHAPING_CFG *cfg;

		cfg = (QOS_RATE_SHAPING_CFG *)buffer;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_SET_QOS_SHAPER_CFG***\n");
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "pir=%d cir=%d pbs=%d cbs=%d\n",
			cfg->shaper.pir, cfg->shaper.cir, cfg->shaper.pbs, cfg->shaper.cbs);

		cfg->phys_shaperid = qos_hal_add_shaper_index(cfg);

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Retnd physical Shaper=%d\n",
					 cfg->phys_shaperid);
		return QOS_MGR_SUCCESS;
	}
	case QOS_MGR_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG: {
		struct net_device *if_dev = NULL ;
		QOS_RATE_SHAPING_CFG *cfg;

		cfg = (QOS_RATE_SHAPING_CFG *)buffer;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_SET_QOS_RATE_SHAPING_CFG***\n");
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> inf=%s dev_name=%s \n",
				__func__, cfg->ifname, cfg->dev_name);

		if_dev = dev_get_by_name(&init_net, cfg->ifname);
		if (if_dev == NULL) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");
			return QOS_MGR_FAILURE;
		}

		if (cfg->queueid == -1)
			qos_hal_add_port_rate_shaper(if_dev, cfg, cfg->flag);
		else
			qos_hal_add_queue_rate_shaper_ex(if_dev, cfg, -1, cfg->flag);

		if (if_dev)
			dev_put(if_dev);

		return QOS_MGR_SUCCESS;
	}
	case QOS_MGR_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG: {
		struct net_device *if_dev = NULL;
		QOS_RATE_SHAPING_CFG *cfg;

		cfg = (QOS_RATE_SHAPING_CFG *)buffer;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_RESET_QOS_RATE_SHAPING_CFG***\n");
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> inf=%s dev_name=%s\n",
					__func__, cfg->ifname, cfg->dev_name);

		if ((cfg->flag & QOS_MGR_F_PPPOATM) != QOS_MGR_F_PPPOATM) {
			if_dev = dev_get_by_name(&init_net, cfg->ifname);
			if (if_dev == NULL)
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");

		} else
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>PPPOATM Intf:%s\n",
						__func__, cfg->dev_name);

		if (cfg->queueid == -1)
			qos_hal_del_port_rate_shaper(if_dev, cfg, cfg->flag);
		else {
			qos_hal_del_queue_rate_shaper_ex(if_dev, cfg, -1,
					(cfg->flag | QOS_HAL_DEL_SHAPER_CFG));
		}

		if (if_dev)
			dev_put(if_dev);
		return QOS_MGR_SUCCESS;
	}
	case QOS_MGR_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG: {
		struct net_device *if_dev = NULL;
		QOS_RATE_SHAPING_CFG *cfg;
		cfg = (QOS_RATE_SHAPING_CFG *)buffer;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "***QOS_MGR_GENERIC_HAL_GET_QOS_RATE_SHAPING_CFG***\n");

		if_dev = dev_get_by_name(&init_net, cfg->ifname);
		if (if_dev == NULL) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Invalid interface name\n");
			return QOS_MGR_FAILURE;
		}
		qos_hal_get_queue_rate_shaper_ex(if_dev, cfg, cfg->flag);
		if (if_dev)
			dev_put(if_dev);
	}
	default:
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "qos_mgr_hal_generic_hook not support cmd 0x%x\n", cmd);
		return QOS_MGR_FAILURE;
	}

	return QOS_MGR_FAILURE;
}

/** =================================  STATS API's ====================================== */
static int32_t update_queue_mib(uint32_t *curr, uint32_t *last,
				uint32_t *accumulated)
{
	int32_t delta;

	if (*curr >= *last) {
		delta = (*curr - *last);
		*accumulated += delta;
	} else {
		delta = (*curr + WRAPAROUND_32_BITS - *last);
		*accumulated += delta;
	}
	*last = *curr;
	return delta;
}

/*!
    \brief This is to get the mib couter for specified port and queue
    \param[in] portid the physical port id
    \param[in] queueid the queueid for the mib counter to get
    \param[out] mib the buffer for mib counter
    \param[in] flag reserved for future
    \return returns the queue number supported on this port.
*/
int32_t qos_hal_get_qos_mib1(
		struct net_device *netdev,
		uint32_t portid,
		uint32_t queueid,
		QOS_MGR_MIB *mib,
		uint32_t flag)
{

	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t index;
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	struct qos_hal_qos_mib qos_mib_temp = {0};
	struct dp_dequeue_res qos_res = {0};

	MEM_ALLOC(dp_subif_t, dp_subif);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Get MIB for QueueId %d\n",
				__func__, queueid);
	if (dp_get_netif_subifid(netdev, NULL, NULL, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Fail to Get Subif Id\n",
					 __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
#ifdef QOS_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = 0;
#endif

	dp_subif->subif  = dp_subif->subif >> 8;

	qos_res.dev = netdev;
	qos_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&qos_res, flag) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%dFail to Get QOS Resources\n",
					 __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (qos_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	qos_res.q_res = kmalloc(sizeof(struct dp_queue_res) * qos_res.num_q, GFP_KERNEL);
	if (qos_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	if (dp_deq_port_res_get(&qos_res, flag) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
					__func__, __LINE__);
		kfree(dp_subif);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q:%d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res.q_res->qos_deq_port,
			qos_res.q_res->sch_id[0], qos_res.q_res->q_id);

	subif_index = qos_hal_user_sub_interface_info +
			(qos_res.q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
			dp_subif->subif;

	if ((dp_subif->port_id != 0) && (qos_res.q_res->qos_deq_port == g_CPU_PortId)) {
		struct qos_hal_dp_res_info res = {0};
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Adding the shaper for Directpath interface!!!\n", __func__);
		qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n",
					__func__, res.dp_egress_res.dp_port);

		subif_index = qos_hal_user_sub_interface_ingress_info +
				(res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
				dp_subif->subif;
		port_subif_index = qos_hal_user_sub_interface_ingress_info +
				(res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);
	}
	kfree(dp_subif);
	index = qos_hal_get_user_index_from_qid(subif_index, queueid);
	if (index == QOS_HAL_STATUS_ERR) {
		ret = QOS_HAL_STATUS_INVALID_QID;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Invalid Queue Id\n",
					__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> User Queue Index =%d \n",
			__func__, index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Actual Queue Index %d\n",
			 __func__, subif_index->user_queue[index].queue_index);

	qos_hal_ll_get_qos_mib(qos_res.q_res->q_id, &qos_mib_temp, flag);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res.q_res);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return ret;
}

int32_t qos_hal_get_csum_ol_mib(
		struct qos_hal_qos_stats *csum_mib,
		uint32_t flag)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t delta = 0, mode = 0;
	struct dp_dequeue_res qos_res = {0};
	struct qos_hal_qos_mib qos_mib_temp = {0};
	struct qos_hal_user_subif_abstract *subif_index = NULL;

	if ((dp_deq_port_res_get(&qos_res, DP_F_CHECKSUM) != DP_SUCCESS) ||
		(qos_res.num_q == 0)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
					__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	qos_res.q_res = kmalloc(sizeof(struct dp_queue_res) * qos_res.num_q, GFP_KERNEL);
	if (qos_res.q_res == NULL)
		return QOS_HAL_STATUS_ERR;

	if (dp_deq_port_res_get(&qos_res, DP_F_CHECKSUM) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
					__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>QOS Port: %d SB: %d Q: %d\n",
			__func__, qos_res.q_res->qos_deq_port,
			qos_res.q_res->sch_id[0], qos_res.q_res->q_id);

	subif_index = qos_hal_user_sub_interface_info +
			(qos_res.q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);

	qos_hal_get_counter_mode(&mode);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Enq/Deq Counter Mode is=%s\n",
			__func__, (mode == 1) ? "Bytes" : "Packet");

	qos_hal_ll_get_qos_mib(qos_res.q_res->q_id, &qos_mib_temp, flag);
	if (mode == 0) {
		delta = update_queue_mib(&g_csum_ol_mib.enqPkts,
			&qos_mib_temp.enqPkts, &g_csum_ol_mib.enqPkts);
		csum_mib->enqPkts += delta;
		delta = update_queue_mib(&g_csum_ol_mib.deqPkts,
			&qos_mib_temp.deqPkts, &g_csum_ol_mib.deqPkts);
		csum_mib->deqPkts += delta;
	} else {
		delta = update_queue_mib(&g_csum_ol_mib.enqBytes,
			&qos_mib_temp.enqBytes, &g_csum_ol_mib.enqBytes);
		csum_mib->enqBytes += delta;
		delta = update_queue_mib(&g_csum_ol_mib.deqBytes,
			&qos_mib_temp.deqBytes, &g_csum_ol_mib.deqBytes);
		csum_mib->deqBytes += delta;
	}

QOS_RESOURCES_CLEANUP:
	kfree(qos_res.q_res);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return ret;
}

int32_t qos_hal_clear_csum_ol_mib(
		uint32_t flag)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	struct dp_dequeue_res qos_res = {0};
	struct qos_hal_user_subif_abstract *subif_index = NULL;

	if ((dp_deq_port_res_get(&qos_res, DP_F_CHECKSUM) != DP_SUCCESS) ||
		(qos_res.num_q == 0)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
					__func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}

	qos_res.q_res = kmalloc(sizeof(struct dp_queue_res) * qos_res.num_q, GFP_KERNEL);
	if (qos_res.q_res == NULL)
		return QOS_HAL_STATUS_ERR;

	if (dp_deq_port_res_get(&qos_res, DP_F_CHECKSUM) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
					__func__, __LINE__);
		ret = QOS_HAL_STATUS_ERR;
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> QOS Port: %d SB: %dQ: %d\n",
		__func__, qos_res.q_res->qos_deq_port, qos_res.q_res->sch_id[0],
		qos_res.q_res->q_id);

	subif_index = qos_hal_user_sub_interface_info +
			(qos_res.q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);

	qos_hal_ll_reset_qos_mib(qos_res.q_res->q_id, flag);
	memset(&g_csum_ol_mib, 0, sizeof(struct qos_hal_qos_mib));

QOS_RESOURCES_CLEANUP:
	kfree(qos_res.q_res);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return ret;
}

int32_t
qos_hal_get_qos_mib(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		struct qos_hal_qos_stats *qos_mib,
		uint32_t flag)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t index;
	uint32_t no_of_qos_ports = 0;
	int32_t mode;
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	struct qos_hal_qos_mib qos_mib_temp = {0};
	struct dp_dequeue_res *qos_res = NULL;

	if (!netdev && !subif_id) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR,  "netdev=%p subif_id=%p\n",
				netdev, subif_id);
		return QOS_MGR_FAILURE;
	}

	if (!netdev && (subif_id->port_id >= 16)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR,  "netdev=%p port_id=%d\n",
				netdev, subif_id->port_id);
		return QOS_MGR_FAILURE;
	}

	MEM_ALLOC(dp_subif_t, dp_subif);
	dp_subif->port_id = subif_id->port_id;
	dp_subif->subif = subif_id->subif;
	if (netdev)
		ret = qos_hal_get_q_res_from_netdevice(
			netdev,
			netdev->name,
			0,
			dp_subif,
			0,
			&no_of_qos_ports,
			(void **) &qos_res,
			flag);
	else
		ret = qos_hal_get_q_res_from_netdevice(
			NULL,
			NULL,
			0,
			dp_subif,
			0,
			&no_of_qos_ports,
			(void **) &qos_res,
			flag);

	if (ret != QOS_HAL_STATUS_OK) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Error: Failed to get resources from Netdevice\n",
					__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	dp_subif->subif  = dp_subif->subif >> 8;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->q_res->qos_deq_port,
			qos_res->q_res->sch_id[0], qos_res->q_res->q_id);

	subif_index = qos_hal_user_sub_interface_info +
			(qos_res->q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
			dp_subif->subif;

	if ((dp_subif->port_id != 0) && (qos_res->q_res->cqm_deq_port == g_CPU_PortId)) {
		struct qos_hal_dp_res_info res = {0};
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Adding the shaper for Directpath interface!!!\n", __func__);
		qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n",
					__func__, res.dp_egress_res.dp_port);

		subif_index = qos_hal_user_sub_interface_ingress_info +
				(res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
				dp_subif->subif;
		port_subif_index = qos_hal_user_sub_interface_ingress_info +
				(res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n",
				 __func__, subif_index);

	}
	qos_hal_get_counter_mode(&mode);
	if ((queueid != -1) && (queueid < EGRESS_QUEUE_ID_MAX)) { /* for individual queue */
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Get MIB for QueueId%d\n",
				__func__, queueid);
		index = qos_hal_get_user_index_from_qid(subif_index, queueid);
		if (index == QOS_HAL_STATUS_ERR) {
			ret = QOS_HAL_STATUS_INVALID_QID;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Invalid Queue Id\n",
						__func__, __LINE__);
			goto QOS_RESOURCES_CLEANUP;
		}

		qos_hal_ll_get_qos_mib(subif_index->user_queue[index].queue_index,
					&qos_mib_temp, flag);
		if (mode == 0) {
			qos_mib->enqPkts = qos_mib_temp.enqPkts;
			qos_mib->deqPkts = qos_mib_temp.deqPkts;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Subif Pkts ===> Rx:%lld TX:%lld\n",
						qos_mib->enqPkts, qos_mib->deqPkts);
		} else {
			qos_mib->enqBytes = qos_mib_temp.enqBytes;
			qos_mib->deqBytes = qos_mib_temp.deqBytes;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Subif Bytes ====> Rx:%lld TX:%lld\n",
						qos_mib->enqBytes, qos_mib->deqBytes);
		}
		qos_mib->dropPkts = qos_mib_temp.dropPkts;

	} else if (queueid == -1) {
		int32_t i, q_count;
		int32_t delta = 0;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Enq/Deq Counter Mode is =%s\n",
					__func__, (mode == 1) ? "Bytes" : "Packet");
		qos_hal_get_queue_num(netdev, subif_id->port_id, &q_count);
		for (i = 0; i < q_count; i++) {
			qos_hal_ll_get_qos_mib(subif_index->user_queue[i].queue_index, &qos_mib_temp, flag);
			if (mode == 0) {
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.enqPkts,
					&qos_mib_temp.enqPkts, &subif_index->user_queue[i].q_mib.enqPkts);
				subif_index->subif_mib.enqPkts += delta;
			} else {
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.enqBytes,
					&qos_mib_temp.enqBytes, &subif_index->user_queue[i].q_mib.enqBytes);
				subif_index->subif_mib.enqBytes += delta;

			}

			if (mode == 0) {
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.deqPkts,
					&qos_mib_temp.deqPkts, &subif_index->user_queue[i].q_mib.deqPkts);
				subif_index->subif_mib.deqPkts += delta;
			} else {
				delta = update_queue_mib(&subif_index->user_queue[i].q_mib.deqBytes,
					&qos_mib_temp.enqBytes, &subif_index->user_queue[i].q_mib.deqBytes);
				subif_index->subif_mib.deqPkts += delta;
			}

			delta = update_queue_mib(&subif_index->user_queue[i].q_mib.dropPkts,
				&qos_mib_temp.dropPkts, &subif_index->user_queue[i].q_mib.dropPkts);
			subif_index->subif_mib.dropPkts += delta;
		}
		if (mode == 0) {
			qos_mib->enqPkts = subif_index->subif_mib.enqPkts;
			qos_mib->deqPkts = subif_index->subif_mib.deqPkts;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Subif Pkts ===> Rx:%lld TX:%lld\n",
						qos_mib->enqPkts, qos_mib->deqPkts);
		} else {
			qos_mib->enqBytes = subif_index->subif_mib.enqBytes;
			qos_mib->deqBytes = subif_index->subif_mib.deqBytes;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Subif Bytes ====> Rx:%lld TX:%lld\n",
						qos_mib->enqBytes, qos_mib->deqBytes);
		}
		qos_mib->dropPkts = subif_index->subif_mib.dropPkts;
	} else
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Wrong Queue Id %d\n", __func__, queueid);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return ret;
}

int32_t
qos_hal_reset_qos_mib(
		struct net_device *netdev,
		dp_subif_t *subif_id,
		int32_t queueid,
		uint32_t flag)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t no_of_qos_ports, index;
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	struct dp_dequeue_res *qos_res = NULL;

	if (!netdev && !subif_id) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR,  "netdev=%p subif_id=%p\n", netdev, subif_id);
		return QOS_MGR_FAILURE;
	}

	if (!netdev && (subif_id->port_id >= 16)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR,  "netdev=%p port_id=%d\n", netdev, subif_id->port_id);
		return QOS_MGR_FAILURE;
	}

	MEM_ALLOC(dp_subif_t, dp_subif);
	dp_subif->port_id = subif_id->port_id;
	dp_subif->subif = subif_id->subif;

	if (netdev)
		ret = qos_hal_get_q_res_from_netdevice(
			netdev,
			netdev->name,
			0,
			dp_subif,
			0,
			&no_of_qos_ports,
			(void **)&qos_res,
			flag);
	else
		ret = qos_hal_get_q_res_from_netdevice(
			NULL,
			NULL,
			0,
			dp_subif,
			0,
			&no_of_qos_ports,
			(void **)&qos_res,
			flag);

	if (ret != QOS_HAL_STATUS_OK) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Error: Failed to get resources from Netdevice\n",
					__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
				__func__, dp_subif->port_id, dp_subif->subif, qos_res->q_res->qos_deq_port,
				qos_res->q_res->sch_id[0], qos_res->q_res->q_id);

	dp_subif->subif  = dp_subif->subif >> 8;
	subif_index = qos_hal_user_sub_interface_info +
			(qos_res->q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;

	if ((dp_subif->port_id != 0) && (qos_res->q_res->cqm_deq_port == g_CPU_PortId)) {
		struct qos_hal_dp_res_info res = {0};
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Adding the shaper for Directpath interface!!!\n", __func__);
		qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n",
					__func__, res.dp_egress_res.dp_port);

		subif_index = qos_hal_user_sub_interface_ingress_info +
				(res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		port_subif_index = qos_hal_user_sub_interface_ingress_info +
				(res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);
	}
	if ((queueid != -1) && (queueid < EGRESS_QUEUE_ID_MAX)) { /* for individual queue */
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Get MIB for QueueId %d\n", __func__, queueid);
		index = qos_hal_get_user_index_from_qid(subif_index, queueid);
		if (index == QOS_HAL_STATUS_ERR) {
			ret = QOS_HAL_STATUS_INVALID_QID;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Invalid Queue Id\n", __func__, __LINE__);
			goto QOS_RESOURCES_CLEANUP;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> User Queue Index =%d \n", __func__, index);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Actual Queue Index %d\n", __func__, subif_index->user_queue[index].queue_index);
		qos_hal_ll_reset_qos_mib(subif_index->user_queue[index].queue_index, flag);
	} else if (queueid == -1) { /* for that netdevice */
		int32_t i, q_count;

		qos_hal_get_queue_num(netdev, subif_id->port_id, &q_count);
		for (i = 0; i < q_count; i++) {
			qos_hal_ll_reset_qos_mib(subif_index->user_queue[i].queue_index, flag);
		}
		memset(&subif_index->subif_mib, 0, sizeof(struct qos_hal_subif_mib));
		for (i = 0; i < QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; i++)
			memset(&subif_index->user_queue[i].q_mib, 0, sizeof(struct qos_hal_qos_mib));
	} else
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Wrong Queue Id %d\n", __func__, queueid);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return ret;
}

/* ======================================================================================================= */

int
qos_hal_add_new_queue(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port,
		uint32_t policy_type,
		uint32_t priority,
		uint32_t prio_weight,
		uint32_t sched_id,
		struct qos_hal_user_subif_abstract *subif_index,
		enum qos_hal_errorcode *eErrCode,
		uint32_t flags)
{
	uint32_t q_index, q_new, priority_weight, node_policy;
	struct qos_hal_sched_track_info s_next = {0};
	struct qos_hal_equeue_create q_add;
	int ret = QOS_HAL_STATUS_OK;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port:%d sched:%d policy_type:%d\n",
			__func__, port, sched_id, policy_type);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>priority:%d priority_weight: %d\n",
			__func__, priority, prio_weight);
	s_next.id = 0xFF;
	if (sched_id < QOS_HAL_MAX_SCHEDULER)
		s_next = qos_hal_sched_track[sched_id];

	/* Get the free queue from the pool */
	if (dp_reserve)
		q_new = qos_hal_get_free_dp_queue();
	else
		q_new = qos_hal_get_free_queue();

	if (q_new == 0xFF) {
		*eErrCode =  QOS_HAL_STATUS_MAX_QUEUES_FOR_PORT;
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Exceed Max queue for Port\n",
				__func__, __LINE__);
		goto ADD_Q_ERR_HANDLER;
	}
	priority_weight = priority;
	if (sched_id < QOS_HAL_MAX_SCHEDULER) {
		if (s_next.priority_selector > 0) {
			if (priority > 7)
				priority_weight = priority-8;
			else
				priority_weight = priority;
		}
	} else { /* For valid port */
		if (priority > 7)
			priority_weight = priority-8;
		else
			priority_weight = priority;
	}

	memset(&q_add, 0, sizeof(struct qos_hal_equeue_create));
	q_add.index = q_new;
	q_add.egress_port_number = port;

	if (policy_type == QOS_HAL_POLICY_WSP) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Strict Priority Queue\n",
				__func__);
		q_add.iwgt = 0;
	} else if ((policy_type == QOS_HAL_POLICY_WFQ)
			|| (policy_type == QOS_HAL_POLICY_WRR)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Weighted Fair Queue\n",
				__func__);
		q_add.iwgt = prio_weight;
	}

	if (sched_id < QOS_HAL_MAX_SCHEDULER)
		node_policy = qos_hal_sched_track[sched_id].policy;
	else
		node_policy = qos_hal_port_track[port].policy;

	if ((node_policy == QOS_HAL_POLICY_WFQ)
		|| (node_policy == QOS_HAL_POLICY_WRR)) {
		uint8_t leaf;
		uint32_t free_wfq_leaf;
		if (qos_hal_scheduler_free_input_get(
					sched_id,
					&leaf) < 0) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d No Free Sched\n",
					__func__, __LINE__);

			qos_hal_free_wfq_scheduler_get(
					port,
					sched_id,
					&free_wfq_leaf);
			q_add.scheduler_input =  free_wfq_leaf;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free Sched input [%d]\n",
					__func__, free_wfq_leaf);
		} else
			q_add.scheduler_input =  sched_id << 3 | leaf;

		q_add.iwgt = (q_add.iwgt == 0) ? 1 : q_add.iwgt ;
		ret = qos_hal_egress_queue_create(&q_add);
	} else {
		uint32_t new_sched, sie = 0, sit = 0, qsid = 0;

		/* Get the input details of scheduler leaf */
		if (sched_id < QOS_HAL_MAX_SCHEDULER) {
			ret = qos_hal_scheduler_in_status_get(sched_id << 3 | priority_weight, &sie, &sit, &qsid);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>SBIN: [%d] is %s\n", __func__, sched_id << 3 | priority_weight, sie ? "enabled" : "disabled");
		} else {
			ret = qos_hal_node_in_status_get((port << 3 | priority_weight), DP_NODE_PORT, &sie, &sit, &qsid);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port: [%d] is %s\n", __func__, port << 3 | priority_weight, sie ? "enabled" : "disabled");
		}
		if ((node_policy == QOS_HAL_POLICY_WSP) &&
			((policy_type == QOS_HAL_POLICY_WFQ) ||
					(policy_type == QOS_HAL_POLICY_WRR))) {
			if (sched_id < QOS_HAL_MAX_SCHEDULER) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Create New WFQ Scheduler in SB:leaf [%d/%d]\n",
							sched_id, priority_weight);
				new_sched = create_new_scheduler(sched_id, 0,
								QOS_HAL_POLICY_WSP,
								s_next.level+1,
								priority_weight);
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Create New WFQ Scheduler in Port:leaf [%d/%d]\n",
							port, priority_weight);
				new_sched = create_new_scheduler(port, 1,
								QOS_HAL_POLICY_WSP,
								0, priority_weight);
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "New Scheduler ID is [%d]\n", new_sched);
			qos_hal_sched_track[new_sched].policy = QOS_HAL_POLICY_WRR;

			/* Add the new queue to the WFQ scheduler */
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Adding new queue %d to scheduler %d\n",
						q_new, new_sched);
			q_add.scheduler_input =  new_sched << 3 | 0; /*next priority leaf */
			q_add.iwgt = (prio_weight == 0) ? 1 : prio_weight;
			ret = qos_hal_egress_queue_create(&q_add);
			goto ADD_Q_ERR_HANDLER;
		}
		if ((flags & QOS_HAL_NO_PRIO_MATCH) == QOS_HAL_NO_PRIO_MATCH) {
			q_add.scheduler_input =  sched_id << 3 | priority_weight;
			q_add.prio =  priority_weight;
			ret = qos_hal_egress_queue_create(&q_add);
			goto ADD_Q_ERR_HANDLER;
		}

		/* If scheduler Input is disabled  and no entry in leaf mask of Scheduler */
		if (sie == 0) { /* no queue connected to this leaf */
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "No queue is attached to this leaf\n");

			q_add.scheduler_input =  sched_id << 3 | priority_weight;
			q_add.prio =  priority_weight;
			ret = qos_hal_egress_queue_create(&q_add);

		} else { /* already queue/scheduler is connected */
			if (sit == 0) { /* input is QID */
				struct qos_hal_equeue_create q_reconnect;
				uint32_t cfg_shaper, user_q_index;
				struct qos_hal_equeue_cfg q_param = {0};
				struct dp_queue_map_entry *q_map_get = NULL;
				int32_t no_entries = 0;
				struct qos_hal_token_bucket_shaper_cfg cfgShaper = {0};

				q_index = qsid; /* this is the QID */

				q_index = qos_hal_get_queue_index_from_qid(qsid);
				/* Here q_index EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
				if (q_index >= EGRESS_QUEUE_ID_MAX)
					goto ADD_Q_ERR_HANDLER;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is attached to this Queue %d\n",
						__func__, qos_hal_queue_track[q_index].tb_index, q_index);
				cfg_shaper = qos_hal_queue_track[q_index].tb_index;

				/* Delete the Queue */
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Already Q %d is attached to this leaf\n",
						q_index);

				qos_hal_get_q_map(g_Inst, q_index, &no_entries, &q_map_get, 0);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> %d No of entries is %d for queue=%d\n",
						__func__, __LINE__, no_entries, q_index);

				/* Create a WFQ scheduler and attach to the leaf of Scheduler */
				if (sched_id < QOS_HAL_MAX_SCHEDULER) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Create New WFQ Scheduler in SB:leaf [%d/%d]\n", sched_id, priority_weight);
					new_sched = create_new_scheduler(sched_id, 0, QOS_HAL_POLICY_WSP, s_next.level+1, priority_weight);
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Create New WFQ Scheduler in Port:leaf [%d/%d]\n", port, priority_weight);
					new_sched = create_new_scheduler(port, 1, QOS_HAL_POLICY_WSP, 0, priority_weight);
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "New Sched ID [%d]\n",
						new_sched);
				qos_hal_sched_track[new_sched].policy = QOS_HAL_POLICY_WFQ;

				/* Add the deleted queue to the WFQ/WRR scheduler */
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Reconnect Q [%d]->SB [%d]\n", q_index, new_sched);
				memset(&q_reconnect, 0, sizeof(struct qos_hal_equeue_create));
				q_reconnect.index = q_index;
				q_reconnect.egress_port_number = port;
				q_reconnect.scheduler_input =  new_sched << 3 | 0; /* highest priority leaf */
				q_reconnect.iwgt = (qos_hal_queue_track[q_index].prio_weight == 0) ? 1 : qos_hal_queue_track[q_index].prio_weight ;

				ret = qos_hal_queue_relink(&q_reconnect);

				if (cfg_shaper != 0xFF) {
					cfgShaper.enable = true;
					cfgShaper.mode = qos_hal_shaper_track[cfg_shaper].tb_cfg.mode;
					cfgShaper.cir = qos_hal_shaper_track[cfg_shaper].tb_cfg.cir;
					cfgShaper.pir = qos_hal_shaper_track[cfg_shaper].tb_cfg.pir;
					cfgShaper.cbs = qos_hal_shaper_track[cfg_shaper].tb_cfg.cbs;
					cfgShaper.pbs = qos_hal_shaper_track[cfg_shaper].tb_cfg.pbs;
					cfgShaper.index = cfg_shaper;

					qos_hal_token_bucket_shaper_cfg_set(&cfgShaper);
					/* Add the token to the scheduler input */
					qos_hal_token_bucket_shaper_create(cfg_shaper, q_reconnect.scheduler_input);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is created for scheduler input %d \n",
							__func__, cfg_shaper, q_reconnect.scheduler_input);
				}

				if (subif_index != NULL) {
					user_q_index = qos_hal_queue_track[q_index].user_q_idx;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Configure Drop params for mode %s \n",
							__func__, (subif_index->user_queue[user_q_index].ecfg.wred_enable == 0) ? "DT" : "WRED");
					q_param.index = q_index;
					q_param.enable = 1;
					q_param.wred_enable = subif_index->user_queue[user_q_index].ecfg.wred_enable;
					q_param.drop_threshold_green_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_min;
					q_param.drop_threshold_green_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max;
					q_param.drop_threshold_yellow_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_min;
					q_param.drop_threshold_yellow_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max;
					q_param.drop_probability_green = subif_index->user_queue[user_q_index].ecfg.drop_probability_green;
					q_param.drop_probability_yellow = subif_index->user_queue[user_q_index].ecfg.drop_probability_yellow;
					q_param.avg_weight = subif_index->user_queue[user_q_index].ecfg.weight;

					qos_hal_egress_queue_cfg_set(&q_param);
				}

				if (no_entries > 0) {
					int32_t j;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Q map pointer =%p\n", q_map_get);
					for (j = 0; j < no_entries; j++)
						qos_hal_add_q_map(g_Inst, q_index, &q_map_get[j], 0);
					no_entries = 0;
				}
				kfree(q_map_get);
				q_map_get = NULL;

				/* Add the new queue to the WFQ scheduler */
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Adding new queue %d to scheduler %d\n", q_new, new_sched);
				q_add.scheduler_input =  new_sched << 3 | 1; /*next priority leaf */
				q_add.iwgt = (prio_weight == 0) ? 1 : prio_weight ;
				ret = qos_hal_egress_queue_create(&q_add);

			} else if (sit == 1) { /* input is SBID */
				/** Already a scheduler is connected. Find the free leaf.
				  Then create a new queue to that leaf. */
				uint8_t leaf = 0, sched_id;
				sched_id = qos_hal_get_sched_index_from_schedid(qsid);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Attached Scheduler ID is [%d/%d]\n", sched_id, qsid);
				if (qos_hal_scheduler_free_input_get(
							sched_id,
							&leaf) < 0) {
					*eErrCode =  QOS_HAL_STATUS_NOT_AVAILABLE;
					ret = QOS_HAL_STATUS_ERR;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Free Schedular Not Available\n", __func__, __LINE__);
					goto ADD_Q_ERR_HANDLER;
				}

				q_add.scheduler_input =  sched_id << 3 | leaf;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Free leaf of scheduler [%d] is [%d]\n", sched_id, leaf);
				ret = qos_hal_egress_queue_create(&q_add);
			}
		}
	}

ADD_Q_ERR_HANDLER:
	if (ret == QOS_HAL_STATUS_OK) {
		qos_hal_queue_track[q_new].q_type = policy_type;
		*eErrCode =  QOS_HAL_STATUS_OK;
		return q_new;
	} else {
		q_new = 0xFF;
	}
	return ret;
}

int qos_hal_get_q_idx_of_same_prio(
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t sched,
		uint32_t priority,
		uint32_t *q_index,
		uint8_t *user_index,
		uint32_t *sb_to_connect)
{
	int i = 0;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue Count: %d\n", subif_index->queue_cnt);
	for (i = 0; i < subif_index->queue_cnt; i++) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> UserQ[%d].prio=%d priority: %d \n", __func__, i, subif_index->user_queue[i].prio_level, priority);

		if (subif_index->user_queue[i].prio_level == priority)
			break;
	}
	if (i == subif_index->queue_cnt) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<qos_hal_get_q_idx_of_same_prio> Queue Count on main interface=%d Queue Count on port =%d\n",
				subif_index->queue_cnt, qos_hal_port_track[subif_index->qos_port_idx].no_of_queues);
		if (subif_index->queue_cnt < qos_hal_port_track[subif_index->qos_port_idx].no_of_queues) {
			/**  For sure Queue is connected for sub interface also*/
			if (subif_index->is_logical == 0) {
				uint32_t sched_in, base_sched;
				int t;
				struct qos_hal_user_subif_abstract *temp_index = NULL;

				if (qos_hal_sched_track[sched].priority_selector == 1) {
					if (priority > 7)
						sched_in = qos_hal_sched_track[qos_hal_sched_track[sched].next_sched_id].peer_sched_id;
					else
						sched_in = qos_hal_sched_track[sched].next_sched_id;
				} else {
					sched_in = sched;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> scheduler input = %d\n", __func__, sched_in);
				temp_index = subif_index + 1;
				for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT; t++) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface %d User level %d connected to port input %d\n",
							__func__, t, temp_index->port_sched_in_user_lvl, temp_index->port_sched_in_egress);
					if (temp_index->port_sched_in_user_lvl == priority) {
						uint32_t sie, sit, qsid;

						*user_index = temp_index->port_sched_in_egress & 7;
						*sb_to_connect = sched_in;
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Same priority Sub-interface input match found for index %d\n",
									__func__, *user_index);

						/* Get the input details of scheduler leaf */
						qos_hal_scheduler_in_status_get(temp_index->port_sched_in_egress, &sie, &sit, &qsid);
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> qsid = %d\n", __func__, qsid);

						if (qos_hal_sched_track[qsid].policy == QOS_HAL_POLICY_WSP) {/* must be sub-interface is connected to this leaf */
							qos_hal_sched_blk_in_enable(temp_index->port_sched_in_egress, 0); /* Disable the scheduler input */
							qos_hal_sched_track[sched_in].leaf_mask &= ~(1 << *user_index);
							base_sched = create_new_scheduler(sched_in, 0, QOS_HAL_POLICY_WSP,
										qos_hal_sched_track[qsid].level,
										*user_index);
							qos_hal_sched_track[base_sched].policy = QOS_HAL_POLICY_WFQ;
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Schedular =%d\n", __func__, base_sched);

							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Remap SB = %d\n", qsid);
							qos_hal_sched_out_remap(qsid,
										qos_hal_sched_track[base_sched].level + 1,
										base_sched << 3, 1,
										(subif_index->subif_weight == 0) ? 1 : subif_index->subif_weight);
							/*QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface port scheduler input is changed from %d -> %d\n",
										__func__, temp_index->port_sched_in_egress, base_sched << 3);
							temp_index->port_sched_in_egress = base_sched << 3;*/
						}
						*q_index = 0xFF;
						return QOS_HAL_STATUS_OK;
					}
					temp_index += 1;
				}
				*q_index = 0xFF;
				*sb_to_connect = 0xFF;
				return QOS_HAL_STATUS_ERR;
			}
			goto NO_MATCH_FOUND;
		}
		goto NO_MATCH_FOUND;
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Same priority Queue match found for index %d\n", __func__, i);
		*q_index =  subif_index->user_queue[i].queue_index;
		*sb_to_connect =  subif_index->user_queue[i].sbin >> 3;
		*user_index = subif_index->user_queue[i].sbin & 7;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> User Index %d SB to connect: %d\n", __func__, *user_index, *sb_to_connect);
	}

	return QOS_HAL_STATUS_OK;
NO_MATCH_FOUND:
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No Match Found \n", __func__);
	*q_index = 0xFF;
	*sb_to_connect = 0xFF;
	return QOS_HAL_STATUS_ERR;
}

int
qos_hal_find_node_in_for_q_add(
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t node_id,
		uint32_t node_type,
		uint32_t priority,
		uint32_t *index,
		uint32_t *new_leaf)
{
	int i, j, k;
	uint32_t node_in, q_loc = 0, temp_index = 0, prev_index = 0;
	uint8_t leaf;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> ~~~~Enter ~~~~~~\n",
			__func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Node Id [%d]\n",
			__func__, node_id);

	leaf = 7;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Q Count: %d free_leaf %d\n",
			__func__, subif_index->queue_cnt, leaf);
	*new_leaf = leaf;
	j = 0;
	for (i = 7; i >= 0; i--) {
		uint32_t sie, sit, qsid;
		uint32_t q_index = 0;
		node_in = node_id << 3 | i;

		/* Get the input details of port/scheduler leaf */
		qos_hal_node_in_status_get(
				node_in,
				node_type,
				&sie,
				&sit,
				&qsid);

		j++;
		if (sie == DP_NODE_EN) { /* input is enabled */
			if (sit == 0) { /* input is QID */
				uint32_t usr_q_idx = 0;
				q_index = qos_hal_get_queue_index_from_qid(qsid);
				/* Here q_index EGRESS_QUEUE_ID_MAX means, it is not found int the DB */
				if (q_index >= EGRESS_QUEUE_ID_MAX)
					return QOS_HAL_STATUS_ERR;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>leaf: %d user_q_idx: %d User Prio level %d for Q index [%d:%d]\n",
						__func__, i,
						qos_hal_queue_track[q_index].user_q_idx,
						subif_index->user_queue[qos_hal_queue_track[q_index].user_q_idx].prio_level,
						q_index, qsid);
				usr_q_idx = qos_hal_queue_track[q_index].user_q_idx;
                                if (priority > subif_index->user_queue[usr_q_idx].prio_level) {
					qos_hal_queue_prio_get(qsid, &q_loc);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Actual Prio level is %d(q_loc) for Q index %d\n",
							__func__, q_loc, qsid);
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
				int32_t s_index, p_level = 0;

				s_index = qos_hal_get_sched_index_from_schedid(qsid);
				for (k = 0; k < subif_index->queue_cnt && subif_index->queue_cnt <= QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; k++) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "k %d sbin %d\n", k, subif_index->user_queue[k].sbin);
					if ((subif_index->user_queue[k].sbin >> 3) == s_index)
						break;
				}
				if (k < QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> User Queue Index %d is connected to SBIN %d\n ",
								__func__, k, subif_index->user_queue[k].sbin);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Prio level %d\n",
								__func__, subif_index->user_queue[k].prio_level);
					p_level = subif_index->user_queue[k].prio_level;
				}
				if (priority > p_level) {
					qos_hal_sched_prio_get(qsid, &q_loc);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Actual Prio level is %d(q_loc) for Q index %d\n",
							__func__, q_loc, qsid);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Prev index %d\n",
							__func__, prev_index);
					if (q_loc == 0)
						temp_index = 1;
					else if (q_loc == 7)
						temp_index = q_loc;
					else
						temp_index = q_loc + 1;
					if (temp_index > prev_index)
						prev_index = temp_index;
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
								QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface %d User level %d\n",
										__func__, t, temp_index->port_sched_in_user_lvl);
								if (temp_index->port_sched_in_user_lvl < priority) {
									goto QUEUE_ADD_INPUT_INDEX;
								}
								temp_index += 1;
							}
						}
					}
				} else {
					if (k < QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
						if (subif_index->user_queue[k].prio_level < priority)
							break;
					}
				}
			} /* sit == 1 */
		}
	}
QUEUE_ADD_INPUT_INDEX:
	if (i > 1 && j == leaf) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue add index :[%d]\n",
				leaf);
		*index = leaf;
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Queue add index [%d]\n",
				prev_index);
		*index = prev_index;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> ~~~~~~~~ Exit ~~~~~~~~\n",
			__func__);
	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_queue_add_sched_sp_q_sp(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port,
		uint32_t base_sched,
		uint32_t prio_type,
		uint32_t priority,
		uint32_t weight,
		struct qos_hal_user_subif_abstract *subif_index,
		enum qos_hal_errorcode *eErrCode,
		uint32_t flags)
{
	uint32_t leaf;
	uint32_t q_new = 0;
	uint32_t free_sched = 0xFF;
	uint32_t shift_idx = 0;
	int32_t ret = QOS_HAL_STATUS_OK;
	struct qos_hal_sched_track_info s_next;

	s_next.id = 0xFF;
	/* Initialize node to the base scheduler id/port */
	qos_hal_dump_sb_info(base_sched);
	s_next = qos_hal_sched_track[base_sched];
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Base sched Policy:%d\n",
			__func__, s_next.policy);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> SP Scheduler and SP Q\n",
			__func__);
	if (s_next.leaf_mask < 0xFF && s_next.priority_selector != 1) {
		free_sched = base_sched;
	} else {
		uint32_t omid, parent;
		struct qos_hal_sched_track_info Stemp;
		int32_t i;

		parent = base_sched;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "parent %d next_sched %d\n",
				parent, s_next.next_sched_id);
		if (s_next.leaf_mask == 0xFF
				&& qos_hal_sched_track[s_next.next_sched_id].priority_selector != 2) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add new level\n",
					__func__);
			ret = qos_hal_add_scheduler_level(
					port,
					base_sched,
					weight,
					&parent,
					prio_type,
					&omid);
			if (ret == QOS_HAL_STATUS_ERR) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Fail to add scheduler level.\n");
				*eErrCode = QOS_HAL_STATUS_ERR;
				return QOS_HAL_STATUS_ERR;
			}

			qos_hal_sched_track[parent].priority_selector = 1;
			qos_hal_sched_track[base_sched].priority_selector = 2;
			qos_hal_sched_track[omid].priority_selector = 3;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> netdev:%s\n",
					__func__, netdev->name);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> parent:%d\n",
					__func__, parent);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> base:%d\n",
					__func__, base_sched);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> omid:%d\n",
					__func__, omid);
			subif_index->base_sched_id_egress = parent;
			Stemp = qos_hal_sched_track[parent];
			for (i = 0; (i < subif_index->queue_cnt && i < QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE); i++) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "User qid:[%d]\n",
						subif_index->user_queue[i].qid);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "Q index:[%d]\n",
						subif_index->user_queue[i].queue_index);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "prio_level:[%d]\n",
						subif_index->user_queue[i].prio_level);
			}
			qos_hal_q_align_for_priority_selector(
					netdev,
					dev_name,
					port,
					subif_index,
					base_sched,
					qos_hal_sched_track[base_sched].peer_sched_id,
					QOS_HAL_Q_PRIO_LEVEL,
					flags);
		}
		Stemp = qos_hal_sched_track[parent];
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base sched %d Peer sched %d\n",
				__func__,
				qos_hal_sched_track[Stemp.next_sched_id].id,
				qos_hal_sched_track[Stemp.next_sched_id].peer_sched_id);

		qos_hal_free_prio_scheduler_get(parent, Stemp.next_sched_id, priority, &free_sched);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Free Priority scheduler %d mask:%x\n",
				__func__, free_sched, qos_hal_sched_track[free_sched].leaf_mask);
	}

	qos_hal_find_node_in_for_q_add(
			subif_index,
			free_sched,
			DP_NODE_SCH,
			priority,
			&shift_idx,
			&leaf);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "shift_idx:%u\n", shift_idx);
	ret = qos_hal_shift_down_sbin_conn(
			netdev,
			dev_name,
			port,
			subif_index,
			free_sched,
			shift_idx,
			flags);
	if (ret == QOS_HAL_STATUS_ERR) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Fail to shift down\n");
		*eErrCode = QOS_HAL_STATUS_ERR;
		return QOS_HAL_STATUS_ERR;
	}

	q_new = qos_hal_add_new_queue(
			netdev,
			dev_name,
			port,
			prio_type,
			shift_idx,
			weight,
			free_sched,
			subif_index,
			eErrCode,
			(flags | QOS_HAL_NO_PRIO_MATCH));

	return q_new;
}

int32_t qos_hal_queue_add_sched_wrr_q_wrr(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port,
		uint32_t base_sched,
		uint32_t prio_type,
		uint32_t priority,
		uint32_t weight,
		struct qos_hal_user_subif_abstract *subif_index,
		enum qos_hal_errorcode *eErrCode,
		uint32_t flags)
{
	uint32_t q_new = 0;
	struct qos_hal_sched_track_info s_next;
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t omid;

	s_next.id = 0xFF;
	/* Initialize node to the base scheduler id/port */
	qos_hal_dump_sb_info(base_sched);
	s_next = qos_hal_sched_track[base_sched];
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Base sched Policy: %d base_sched: %d\n",
				__func__, s_next.policy, base_sched);
	if (s_next.leaf_mask < 0xFF && s_next.priority_selector != 1) {
		q_new = qos_hal_add_new_queue(
				netdev,
				dev_name,
				port,
				prio_type,
				priority,
				weight,
				base_sched,
				subif_index,
				eErrCode,
				flags);
	} else {
		uint32_t parent;
		uint32_t free_wfq_sched;
		struct qos_hal_sched_track_info Stemp;
		parent = base_sched;
		Stemp = qos_hal_sched_track[s_next.next_sched_id];

		if (s_next.next_sched_id == 0xFF) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Adding a new level\n");
			ret = qos_hal_add_scheduler_level(
				port,
				base_sched,
				weight,
				&parent,
				prio_type,
				&omid);
			if (ret == QOS_HAL_STATUS_ERR) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Fail to add scheduler level.\n");
				*eErrCode = QOS_HAL_STATUS_ERR;
				return QOS_HAL_STATUS_ERR;
			}
			subif_index->base_sched_id_egress = parent;
			free_wfq_sched = omid;
			qos_hal_sched_track[parent].priority_selector = 1;
			qos_hal_sched_track[base_sched].priority_selector = 2;
			qos_hal_sched_track[omid].priority_selector = 3;
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port:%d parent:%d qos_hal_sched_track[parent].next_sched_id:%d\n", __func__, port, parent, qos_hal_sched_track[parent].next_sched_id);
			qos_hal_free_wfq_scheduler_get(
				port,
				qos_hal_sched_track[parent].next_sched_id,
				&free_wfq_sched);
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Free WFQ scheduler [%d:%d]\n",
				__func__,
				free_wfq_sched,
				qos_hal_sched_track[free_wfq_sched].id);

		q_new = qos_hal_add_new_queue(
				netdev,
				dev_name,
				port,
				prio_type,
				priority,
				weight,
				free_wfq_sched,
				subif_index,
				eErrCode,
				flags);

	}
	return q_new;
}

int qos_hal_queue_add_port_sp_q_sp(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port,
		uint32_t base_sched,
		uint32_t prio_type,
		uint32_t priority,
		uint32_t weight,
		struct qos_hal_user_subif_abstract *subif_index,
		enum qos_hal_errorcode *eErrCode,
		uint32_t flags)
{
	uint32_t leaf = 0;
	uint32_t q_new = 0;
	uint32_t free_sched = 0xFF;
	uint32_t shift_idx = 0;
	struct qos_hal_sched_track_info s_next = {0};

	s_next.id = 0xFF;

	if (qos_hal_port_track[port].leaf_mask < 0x3F) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "Port Policy: %d Mask:%x\n",
				qos_hal_port_track[port].policy,
				qos_hal_port_track[port].leaf_mask);
		qos_hal_find_node_in_for_q_add(
				subif_index,
				port,
				DP_NODE_PORT,
				priority,
				&shift_idx,
				&leaf);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s><%d> shift_idx:%d\n",
				__func__, __LINE__, shift_idx);
		qos_hal_shift_down_sbin_conn(
				netdev,
				dev_name,
				port,
				subif_index,
				s_next.id,
				shift_idx,
				flags);
		q_new = qos_hal_add_new_queue(
				netdev,
				dev_name,
				port,
				prio_type,
				shift_idx,
				weight,
				s_next.id,
				subif_index,
				eErrCode,
				(flags | QOS_HAL_NO_PRIO_MATCH));
	} else {
		uint32_t new_base_sched;
		uint32_t omid, parent;
		struct qos_hal_sched_track_info Stemp;

		/* Add a new scheduler to the port */
		if (qos_hal_port_track[port].policy == QOS_HAL_POLICY_WRR
			&&
			prio_type == QOS_HAL_POLICY_WRR) {
			new_base_sched = create_new_scheduler(
						port,
						1,
						QOS_HAL_POLICY_WRR,
						0,
						0);
			qos_hal_sched_track[new_base_sched].policy = QOS_HAL_POLICY_WRR;
		} else {
			new_base_sched = create_new_scheduler(port, 1, QOS_HAL_POLICY_WSP, 0, 0);
			qos_hal_sched_track[new_base_sched].policy = QOS_HAL_POLICY_WSP;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Base Scheduler:[%d]\n",
				__func__, new_base_sched);
		if (qos_hal_relink_child_to_new_parent(
				port,
				new_base_sched,
				DP_NODE_PORT,
				subif_index) != QOS_HAL_STATUS_OK) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>failed to relink\n",
					__func__);
			return QOS_HAL_STATUS_ERR;
		}

		s_next = qos_hal_sched_track[new_base_sched];
		parent = new_base_sched;
		subif_index->base_sched_id_egress = parent;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "parent %d next_sched_id %d\n", parent, s_next.next_sched_id);
		if (s_next.leaf_mask == 0xFF) {
			if (qos_hal_port_track[port].policy == QOS_HAL_POLICY_WRR && prio_type == QOS_HAL_POLICY_WRR) {
				uint32_t free_wfq_sched;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Both WRR\n");
				qos_hal_free_wfq_scheduler_get(port, new_base_sched, &free_wfq_sched);
				q_new = qos_hal_add_new_queue(
						netdev,
						dev_name,
						port,
						prio_type,
						priority, weight,
						free_wfq_sched,
						subif_index,
						eErrCode,
						flags);
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "parent %d next_sched_id %d\n", parent, s_next.next_sched_id);
				if (qos_hal_sched_track[s_next.next_sched_id].priority_selector != 2) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Adding a new level\n", __func__);
					qos_hal_add_scheduler_level(port, new_base_sched, weight, &parent, prio_type, &omid);
					qos_hal_sched_track[parent].priority_selector = 1;
					qos_hal_sched_track[new_base_sched].priority_selector = 2;
					qos_hal_sched_track[omid].priority_selector = 3;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Parent:%d omid:%d\n", __func__, parent, omid);
					subif_index->base_sched_id_egress = parent;
					Stemp = qos_hal_sched_track[parent];
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Parent: next_sched_id:%d\n", __func__, Stemp.next_sched_id);
					qos_hal_q_align_for_priority_selector(netdev, dev_name, port, subif_index,
							new_base_sched,
							qos_hal_sched_track[new_base_sched].peer_sched_id, QOS_HAL_Q_PRIO_LEVEL, flags);
				}
			}
		}
		Stemp = qos_hal_sched_track[parent];
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base sched %d Peer sched %d\n",
				__func__,
				qos_hal_sched_track[Stemp.next_sched_id].id,
				qos_hal_sched_track[Stemp.next_sched_id].peer_sched_id);

		qos_hal_free_prio_scheduler_get(parent, new_base_sched, priority, &free_sched);
		if (s_next.policy == QOS_HAL_POLICY_WSP) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Free Priority scheduler %d\n", free_sched);
			qos_hal_find_node_in_for_q_add(subif_index, free_sched, DP_NODE_SCH, priority, &shift_idx, &leaf);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "shift_idx  --> [%d]\n", shift_idx);
			qos_hal_shift_down_sbin_conn(netdev, dev_name, port, subif_index, free_sched, shift_idx, flags);
		} else
			shift_idx = 0;

		q_new = qos_hal_add_new_queue(
				netdev,
				dev_name,
				port,
				prio_type,
				shift_idx,
				weight,
				free_sched,
				subif_index,
				eErrCode,
				(flags | QOS_HAL_NO_PRIO_MATCH));
	}

	return q_new;
}

int qos_hal_queue_add(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port,
		uint32_t base_sched,
		uint32_t prio_type,
		uint32_t priority,
		uint32_t weight,
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t flags)
{
	uint32_t q_index, q_new = 0, sched_id, node;
	uint8_t uindex = 0;
	uint8_t upolicy = 0;
	struct qos_hal_sched_track_info s_next;
	enum qos_hal_errorcode eErr = QOS_HAL_STATUS_ERR;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port: %d base_sched: %d prio_type: %d priority: %d weight: %d\n",
			__func__, port, base_sched, prio_type, priority, weight);
	s_next.id = 0xFF;
	/* Initialize node to the base scheduler id/port */
	node = port;
	if (base_sched < QOS_HAL_MAX_SCHEDULER) {
		qos_hal_dump_sb_info(base_sched);
		s_next = qos_hal_sched_track[base_sched];
		node = s_next.id;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Base sched Policy:%d\n",
				__func__, s_next.policy);
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Node Id: %d\n",
			__func__, node);
	if (qos_hal_get_q_idx_of_same_prio(
			subif_index,
			node,
			priority,
			&q_index,
			&uindex,
			&sched_id) != QOS_HAL_STATUS_OK) {

		if (base_sched < QOS_HAL_MAX_SCHEDULER) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add new queue to base Sched: %d\n",
					__func__, node);
			upolicy = qos_hal_sched_track[base_sched].policy;
			if (upolicy == QOS_HAL_POLICY_WSP) {
				q_new = qos_hal_queue_add_sched_sp_q_sp(
						netdev,
						dev_name,
						port,
						base_sched,
						prio_type,
						priority,
						weight,
						subif_index,
						&eErr,
						flags);

			} else if (
				((upolicy == QOS_HAL_POLICY_WFQ)
					&& (prio_type == QOS_HAL_POLICY_WFQ))
					||
				((upolicy == QOS_HAL_POLICY_WRR)
					&& (prio_type == QOS_HAL_POLICY_WRR))) {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "WFQ/WRR Scheduler and WFQ/WRR Queue\n");
				q_new = qos_hal_queue_add_sched_wrr_q_wrr(
						netdev,
						dev_name,
						port,
						base_sched,
						prio_type,
						priority,
						weight,
						subif_index,
						&eErr,
						flags);
			}
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add new queue to Port: %d\n",
					__func__, port);
			q_new = qos_hal_queue_add_port_sp_q_sp(
					netdev,
					dev_name,
					port,
					base_sched,
					prio_type,
					priority,
					weight,
					subif_index,
					&eErr,
					flags);
		}
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "User Index:%d Scheduler Block: %d\n",
				uindex, sched_id);
		if (q_index == 0xFF) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected SB->SB: at the same priority %d:%d\n",
				__func__, sched_id,
				subif_index->user_queue[uindex].sbin & 7);
			q_new = qos_hal_add_new_queue(
					netdev,
					dev_name,
					port,
					prio_type,
					uindex,
					weight,
					sched_id,
					subif_index,
					&eErr,
					flags);
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connected Q->SB->SB: at the same priority %d:%d:%d\n",
				__func__, q_index,
				qos_hal_queue_track[q_index].connected_to_sched_id,
				subif_index->user_queue[uindex].sbin & 7);
			q_new = qos_hal_add_new_queue(netdev,
					dev_name,
					port,
					prio_type,
					uindex,
					weight,
					qos_hal_queue_track[q_index].connected_to_sched_id,
					subif_index, &eErr, flags);
		}
	}

	if (eErr == QOS_HAL_STATUS_OK) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Queue is %d\n", __func__, q_new);
		return q_new;
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue add failed\n", __func__);
		return QOS_HAL_STATUS_ERR;
	}
}

int qos_hal_is_priority_scheduler_for_port(uint32_t port)
{

	if (qos_hal_port_track[port].port_sched_id == 0xFF) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR No schedular is Attached to Port\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}
	return QOS_HAL_STATUS_OK;

}

int qos_hal_dump_subif_queue_info(struct qos_hal_user_subif_abstract *subif_index)
{

	int i = 0;
	int32_t shaper_idx = 0;
	char queue_type[4] = {0};

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "\tQueue Count = %5d \t\t\t\tInterface = %7s\n", subif_index->queue_cnt, subif_index->netdev->name);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "\t========================================================================================================\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "\t|     QID        |    PHY QID     |    Priority    |     Type      |     Weight     |       Shaper     |\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "\t--------------------------------------------------------------------------------------------------------\n");

	for (i = 0; i < subif_index->queue_cnt && subif_index->queue_cnt <= QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE; i++) {
		shaper_idx = qos_hal_queue_track[subif_index->user_queue[i].queue_index].tb_index;
		if (subif_index->user_queue[i].queue_type ==  QOS_HAL_POLICY_WSP)
			strcpy(queue_type, "SP");
		else if (subif_index->user_queue[i].queue_type ==  QOS_HAL_POLICY_WRR)
			strcpy(queue_type, "WRR");
		else
			strcpy(queue_type, "WFQ");

		if ((shaper_idx != 0xFF) && (shaper_idx != INVALID_SHAPER_ID)) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|%9d       |%9d[%d]   |%9d       |      %s\t   |%9d       | idx=%9d    |\n",
				subif_index->user_queue[i].qid,
				subif_index->user_queue[i].queue_index,
				qos_hal_queue_track[subif_index->user_queue[i].queue_index].queue_id,
				subif_index->user_queue[i].prio_level,
				queue_type,
				subif_index->user_queue[i].weight,
				qos_hal_shaper_track[shaper_idx].tb_cfg.index);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|\t\t\t\t\t\t\t\t\t\t    | mode=%9d   |\n", qos_hal_shaper_track[shaper_idx].tb_cfg.mode);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|\t\t\t\t\t\t\t\t\t\t    | cir=%9d    |\n", qos_hal_shaper_track[shaper_idx].tb_cfg.cir);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|\t\t\t\t\t\t\t\t\t\t    | pir=%9d    |\n", qos_hal_shaper_track[shaper_idx].tb_cfg.pir);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|\t\t\t\t\t\t\t\t\t\t    | pbs=%9d    |\n", qos_hal_shaper_track[shaper_idx].tb_cfg.pbs);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|\t\t\t\t\t\t\t\t\t\t    | cbs=%9d    |\n", qos_hal_shaper_track[shaper_idx].tb_cfg.cbs);
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|%9d       |%9d[%d]   |%9d       |      %s\t   |%9d       |         X        |\n",
				subif_index->user_queue[i].qid,
				subif_index->user_queue[i].queue_index,
				qos_hal_queue_track[subif_index->user_queue[i].queue_index].queue_id,
				subif_index->user_queue[i].prio_level,
				queue_type,
				subif_index->user_queue[i].weight);
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "\t--------------------------------------------------------------------------------------------------------\n");
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "\t========================================================================================================\n");
	return QOS_HAL_STATUS_OK;
}

int qos_hal_dump_subif_queue_map(struct qos_hal_user_subif_abstract *subif_index, uint32_t sys_q)
{

	int32_t i = 0, j = 0;
	char temp[50] = {0};

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\tQueue Count = %d Interface = %s\n", subif_index->queue_cnt, subif_index->netdev->name);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t =============================================================\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|    User QId    |      PHY QID    |            Tc            |\n");
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t -------------------------------------------------------------\n");
	for (i = 0; i < subif_index->queue_cnt; i++) {
		memset(temp, 0x0, sizeof(temp));
		if (sys_q == subif_index->user_queue[i].queue_index) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|%8d\t |%8d[%d]\t   |     System Default      |\n",
				subif_index->user_queue[i].qid,
				subif_index->user_queue[i].queue_index,
				qos_hal_queue_track[subif_index->user_queue[i].queue_index].queue_id);
		} else if (subif_index->default_q == subif_index->user_queue[i].queue_index) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|%8d\t |%8d[%d]\t   |      Qos Default        |\n",
				subif_index->user_queue[i].qid,
				subif_index->user_queue[i].queue_index,
				qos_hal_queue_track[subif_index->user_queue[i].queue_index].queue_id);
		} else {
			for (j = 0; j < subif_index->user_queue[i].no_of_tc; j++) {
				if (subif_index->user_queue[i].qmap[j] >= 0
					|| subif_index->user_queue[i].qmap[j] < MAX_TC_NUM) {
					sprintf(temp+strlen(temp), "%2d,",
						subif_index->user_queue[i].qmap[j]);
				} else {
					sprintf(temp+strlen(temp), "%s", "   ");
				}
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t|%8d\t |%8d[%d]\t   | TC Cnt:%d->%s \t|\n",
				subif_index->user_queue[i].qid,
				subif_index->user_queue[i].queue_index,
				qos_hal_queue_track[subif_index->user_queue[i].queue_index].queue_id,
				subif_index->user_queue[i].no_of_tc, temp);
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t -------------------------------------------------------------\n");
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "\t==============================================================\n");

	return QOS_HAL_STATUS_OK;
}

void qos_hal_dump_qos_mgr_info(int32_t index, char *type)
{
	if (strcmp(type, "queue") == 0) {
		if (index < 0)
			qos_hal_dump_all_queue_track();
		else
			qos_hal_dump_queue_info(index);
	} else if (strcmp(type, "port") == 0) {
		if (index < 0)
			qos_hal_dump_all_port_track();
		else
			qos_hal_dump_port_info(index);
	} else if (strcmp(type, "shaper") == 0) {
		if (index < 0)
			qos_hal_dump_all_shaper_track();
		else
			qos_hal_dump_shaper_info(index);
	} else if (strcmp(type, "sched") == 0) {
		if (index < 0)
			qos_hal_dump_all_sched_track();
		else
			qos_hal_dump_sb_info(index);
	} else {
		printk("<%s> Unsupported option.\n", __func__);
	}

	return;
}
EXPORT_SYMBOL(qos_hal_dump_qos_mgr_info);

int qos_hal_get_pppoa_if (struct net_device *netif, QOS_MGR_IFNAME base_ifname[QOS_MGR_IF_NAME_SIZE], uint32_t *flags)
{

#ifdef AARIF //temporary
	QOS_MGR_VCC *vcc = NULL;
	short vpi = 0;
	int vci = 0;

	if (qos_mgr_if_is_pppoa(netif, NULL) && qos_mgr_pppoa_get_vcc(netif, &vcc) == QOS_MGR_SUCCESS) {
		*flags |= QOS_MGR_F_PPPOATM;
		vpi = vcc->vpi;
		vci = vcc->vci;
		snprintf(base_ifname, QOS_MGR_IF_NAME_SIZE, "atm_%d%d", vpi, vci);
		return QOS_MGR_SUCCESS;
	} else
#endif
		return QOS_MGR_FAILURE;
}

int qos_hal_get_queue_info(struct net_device *netdev)
{

	uint32_t flags = 0;
	int32_t ret = QOS_HAL_STATUS_OK;
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index;
	struct qos_hal_user_subif_abstract *base_subif_index;
	QOS_MGR_VCC *vcc = NULL;
	char dev_name[QOS_MGR_IF_NAME_SIZE];
	struct dp_dequeue_res qos_res = {0};

	MEM_ALLOC(dp_subif_t, dp_subif);
	qos_mgr_br2684_get_vcc(netdev, &vcc);
	if (qos_hal_get_pppoa_if (netdev, dev_name, &flags) == QOS_MGR_SUCCESS) {
		if ((flags & QOS_MGR_F_PPPOATM) == QOS_MGR_F_PPPOATM) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Interface : %s , Base PPPOA Interface : %s \n", netdev->name, dev_name);
			if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Subif Id when VCC NULL \n", __func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
		}
	} else {
		if (dp_get_netif_subifid(netdev, NULL, vcc, 0,  dp_subif, 0) != QOS_MGR_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Subif Id\n", __func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
	}
	dp_subif->subif  = dp_subif->subif >> 8;
	if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Subif Id\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	qos_res.dev = netdev;
	qos_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (qos_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	qos_res.q_res = kmalloc(sizeof(struct dp_queue_res) * qos_res.num_q, GFP_KERNEL);
	if (qos_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	qos_res.q_res_size = qos_res.num_q;
	if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif,
			qos_res.q_res->cqm_deq_port, qos_res.q_res->sch_id[0], qos_res.q_res->q_id);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> cqm_deq_port:[%d]\n", __func__, qos_res.q_res->cqm_deq_port);
	base_subif_index = (qos_hal_user_sub_interface_info + qos_res.q_res->cqm_deq_port);
 
	qos_hal_get_subif_list_item(qos_res.q_res->cqm_deq_port,
			netdev,
			&subif_index,
			&base_subif_index->head);

	qos_hal_dump_subif_queue_info(subif_index);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res.q_res);
	kfree(dp_subif);

	return ret;
}
EXPORT_SYMBOL(qos_hal_get_queue_info);

int qos_hal_get_queue_map(struct net_device *netdev)
{

	uint32_t no_of_qos_ports, sys_q, flags = 0;
	int32_t ret = QOS_HAL_STATUS_OK;
	struct dp_dequeue_res qos_res = {0};
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index;
	struct qos_hal_user_subif_abstract *base_subif_index;
	QOS_MGR_VCC *vcc = NULL;
	char dev_name[QOS_MGR_IF_NAME_SIZE];

	MEM_ALLOC(dp_subif_t, dp_subif);
	qos_mgr_br2684_get_vcc(netdev, &vcc);
	if (qos_hal_get_pppoa_if (netdev, dev_name, &flags) == QOS_MGR_SUCCESS) {
		if ((flags & QOS_MGR_F_PPPOATM) == QOS_MGR_F_PPPOATM) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:Interface of type PPPOA : %s \n", __func__, dev_name);
			if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Subif Id when VCC NULL \n", __func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
		}
	} else {
		if (dp_get_netif_subifid(netdev, NULL, vcc, 0,  dp_subif, 0) != QOS_MGR_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Subif Id\n", __func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
	}

	qos_res.dev = netdev;
	qos_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (qos_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	qos_res.q_res = kmalloc(sizeof(struct dp_queue_res) * qos_res.num_q, GFP_KERNEL);
	if (qos_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	qos_res.q_res_size = qos_res.num_q;
	if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res.q_res->qos_deq_port, qos_res.q_res->sch_id[0], qos_res.q_res->q_id);

	no_of_qos_ports = qos_res.num_deq_ports;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> cqm_deq_port:[%d]\n", __func__, qos_res.q_res->cqm_deq_port);
	base_subif_index = (qos_hal_user_sub_interface_info + qos_res.q_res->cqm_deq_port);

	qos_hal_get_subif_list_item(qos_res.q_res->cqm_deq_port,
			netdev,
			&subif_index,
			&base_subif_index->head);
	sys_q = dp_subif->def_qid;

	qos_hal_dump_subif_queue_map(subif_index, sys_q);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res.q_res);
	kfree(dp_subif);

	return ret;
}
EXPORT_SYMBOL(qos_hal_get_queue_map);

int qos_hal_get_detailed_queue_map(uint32_t queue_index)
{
	struct dp_queue_map_entry *q_map_get = NULL;
	int32_t num_entries = 0;
	/*
	   int32_t i = 0;
	 */
	if (qos_hal_get_q_map(
				g_Inst,
				queue_index,
				&num_entries,
				&q_map_get, 0) == -1){
		QOS_HAL_DEBUG_ERR(QOS_DEBUG_ERR,  "Queue Map Get FAILURE: Queue %d could not be mapped properly !!\n", queue_index);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "Queue %d has mapped entries %d \n", queue_index, num_entries);
	/*
	   QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "[FlowId]    [DEC]    [ENC]    [MPE1]    [MPE2]    [TC]  \n");
	   for(i=0; i< num_entries; i++)
	   {
	   QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,  "|  %4d  |  %4d  |  %4d  |  %4d  |  %4d  |  %4d  |\n",
	   (q_map_get+i)->flowid,
	   (q_map_get+i)->dec,
	   (q_map_get+i)->enc,
	   (q_map_get+i)->mpe1,
	   (q_map_get+i)->mpe2,
	   (q_map_get+i)->tc);
	   }
	 */
	kfree(q_map_get);

	return QOS_HAL_STATUS_OK;
}
EXPORT_SYMBOL(qos_hal_get_detailed_queue_map);

int
qos_hal_find_min_prio_level_of_port(
			struct qos_hal_user_subif_abstract *subif_index,
			uint32_t *sched,
			uint32_t *prio_lvl)
{
	uint32_t base_sched;

	base_sched = subif_index->base_sched_id_egress;
	if (base_sched >= QOS_HAL_MAX_SCHEDULER)
		return QOS_HAL_STATUS_MAX_SCHEDULERS_REACHED;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sched Id:%d Priority selector:%d\n",
			__func__,
			base_sched,
			qos_hal_sched_track[base_sched].priority_selector);

	if (qos_hal_sched_track[base_sched].priority_selector == 0) {
		*sched = base_sched;
		*prio_lvl = 7;
	} else {
		*sched = qos_hal_sched_track[qos_hal_sched_track[base_sched].next_sched_id].peer_sched_id;
		*prio_lvl = QOS_HAL_MAX_PRIORITY_LEVEL-1;

	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sched Id:%d Priority Level:%d\n",
			__func__, *sched, *prio_lvl);
	return QOS_HAL_STATUS_OK;
}

/**
	subif_index -- sub interface index of the logical interface
	port_index -- index for the port interface details
	port_sched_id -- scheduler id of the Egress Port
	prio_level -- scheduler input where the logical interface should be connected to
	weight -- weightage among all in that prio_level
*/
int
qos_hal_connect_subif_to_port(
			struct net_device *netdev,
			char *dev_name,
			struct qos_hal_user_subif_abstract *subif_index,
			struct qos_hal_user_subif_abstract *port_index,
			uint32_t port_sched_id,
			uint32_t prio_level,
			uint32_t weight,
			uint32_t flags)
{
	uint32_t base_sched = 0xFF, omid = 0xFF;
	uint32_t q_index;
	uint32_t port_id;
	uint32_t sie, sit, qsid;
	struct qos_hal_equeue_create q_reconnect;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port_sched_id: [%d] prio_level: [%d]\n",
			__func__, port_sched_id, prio_level);
	port_id = port_sched_id;
	qos_hal_scheduler_in_status_get((port_sched_id << 3 | prio_level), &sie, &sit, &qsid);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> SBIN: %d is %s\n",
				__func__, ((port_id << 3) | prio_level),
				(sie == DP_NODE_EN)? "enabled" : "disabled");

	if (sie == DP_NODE_EN) { /* either queue/sbid is connected to this prio_level */
		if (sit == 0) { /* input is QID */
			omid = qsid;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Already Queue %d is added for the to the Port Scheduler In %d\n",
					__func__, qsid, port_sched_id << 3 | prio_level);
			q_index = qsid; /* this is the QID */
			qos_hal_safe_queue_delete(netdev, dev_name, q_index, -1, -1, flags);

			omid = create_new_scheduler(port_sched_id, 0, QOS_HAL_POLICY_WSP, qos_hal_sched_track[port_sched_id].level + 1, prio_level);
			qos_hal_sched_track[omid].policy = QOS_HAL_POLICY_WFQ;

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Reconnecting the queue %d to scheduler %d\n", __func__, q_index, omid);
			memset(&q_reconnect, 0, sizeof(struct qos_hal_equeue_create));
			q_reconnect.index = q_index;
			q_reconnect.egress_port_number = subif_index->qos_port_idx;
			q_reconnect.scheduler_input =  omid << 3 | 0; /* highest priority leaf */
			q_reconnect.iwgt = (qos_hal_queue_track[q_index].prio_weight == 0) ? 1 : qos_hal_queue_track[q_index].prio_weight ;
			qos_hal_egress_queue_create(&q_reconnect);

			base_sched = create_new_scheduler(omid, 0, QOS_HAL_POLICY_WFQ, qos_hal_sched_track[omid].level + 1, 1);
			qos_hal_sched_track[base_sched].policy = QOS_HAL_POLICY_WSP;

		} else if (sit == 1) { /* input is SBID */
			int32_t s_index = QOS_MGR_FAILURE;

			s_index = qos_hal_get_sched_index_from_schedid(qsid);
			if (s_index == QOS_MGR_FAILURE)
				return QOS_MGR_FAILURE;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Already Scheduler [%d:%d] is added for the to the Port Scheduler In %d\n",
					__func__, s_index, qsid, port_sched_id << 3 | prio_level);
			if (qos_hal_sched_track[s_index].policy == QOS_HAL_POLICY_WSP) {
				/* Some other sub-interface is already connected to this priority level. */
				uint32_t sched_in;
				int t;
				struct qos_hal_user_subif_abstract *temp_index = NULL;

				temp_index = port_index + 1;
				for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface %d User level %d port_sched_in_egress %d\n",
							__func__, t, temp_index->port_sched_in_user_lvl, temp_index->port_sched_in_egress);
					if ((temp_index->port_sched_in_user_lvl == prio_level) && (subif_index->port_sched_in_egress >> 3 != INVALID_SCHEDULER_BLOCK_ID)) {
						break;
					}
					temp_index += 1;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface %d matched priority %d\n",
						__func__, t, temp_index->port_sched_in_user_lvl);
				qos_hal_sched_blk_in_enable(port_sched_id << 3 | prio_level, 0); /* Disable the scheduler input */

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Sched level %d\n", __func__, qos_hal_sched_track[port_sched_id].level);
				sched_in = create_new_scheduler(port_sched_id, 0, QOS_HAL_POLICY_WSP, qos_hal_sched_track[port_sched_id].level + 1, prio_level);
				qos_hal_sched_track[sched_in].policy = QOS_HAL_POLICY_WFQ;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap SB = %d\n", __func__, s_index);
				qos_hal_sched_out_remap(s_index, qos_hal_sched_track[sched_in].level + 1, sched_in << 3, 1, 1);
				/*QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface port scheduler input is changed from %d -> %d\n",
				  __func__, temp_index->port_sched_in_egress, sched_in << 3);
				  temp_index->port_sched_in_egress = sched_in << 3;*/
				if (temp_index->port_shaper_sched != INVALID_SCHED_ID) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Sub interface Port Shaper scheduler input is changed from %d -> %d\n",
							__func__, qos_hal_sched_track[temp_index->port_shaper_sched].omid, sched_in << 3);
					qos_hal_sched_track[temp_index->port_shaper_sched].omid = sched_in << 3;
				}

				base_sched = create_new_scheduler(sched_in, 0, QOS_HAL_POLICY_WFQ, qos_hal_sched_track[sched_in].level + 1, 1 /*weight*/);
				qos_hal_sched_track[base_sched].policy = QOS_HAL_POLICY_WSP;

			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Already scheduler level is created for same priority %d\n", __func__, prio_level);
				base_sched = create_new_scheduler(s_index, 0, QOS_HAL_POLICY_WFQ, qos_hal_sched_track[s_index].level + 1, 1 /*weight*/);
				qos_hal_sched_track[base_sched].policy = QOS_HAL_POLICY_WSP;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Scheduler is %d\n", __func__, base_sched);
			subif_index->port_sched_in_egress = port_sched_id << 3 | prio_level;
		}
	} else {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> priority Level %d is free. Add Scheduler to the Port Scheduler %d\n",
				__func__, prio_level, port_sched_id);
		base_sched = create_new_scheduler(
					port_sched_id,
					0,
					QOS_HAL_POLICY_WSP,
					qos_hal_sched_track[port_sched_id].level + 1,
					prio_level);
		qos_hal_sched_track[base_sched].policy = QOS_HAL_POLICY_WSP;
		subif_index->port_sched_in_egress = ((port_sched_id << 3) | prio_level);
	}

	subif_index->base_sched_id_egress = base_sched;
	subif_index->subif_weight = 0;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler %d Port Scheduler input %d\n",
			__func__, subif_index->base_sched_id_egress, subif_index->port_sched_in_egress);

	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_get_subif_index_from_netdev(struct net_device *netdev, struct qos_hal_user_subif_abstract *port_subif_index)
{
	int32_t i = 0;
	struct qos_hal_user_subif_abstract *temp_index = NULL;

	temp_index = port_subif_index + 1;
	for (i = 0; i < QOS_HAL_MAX_SUB_IFID_PER_PORT; i++) {
		if (temp_index->netdev == netdev)
			break;
		temp_index = port_subif_index + 1;
	}
	if (i < QOS_HAL_MAX_SUB_IFID_PER_PORT) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "subif index for net device is %d\n", i);
		return i;
	}
	return QOS_HAL_STATUS_ERR;

}

/**
	netdev -- netdevice interface
	port_id -- if no netdevice then for this port id
	priority -- scheduler input where the logical interface should be connected to
	weight -- weightage among all in that prio_level
	flags -- flags
*/
int32_t
qos_hal_modify_subif_to_port(
		struct net_device *netdev,
		char *dev_name,
		uint32_t port_id,
		uint32_t priority,
		uint32_t weight,
		uint32_t flags)
{
	uint32_t no_of_qos_ports;
	struct dp_dequeue_res qos_res = {0};
	dp_subif_t *dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;
	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t qosport;
	struct qos_hal_user_subif_abstract *subif_index;
	struct qos_hal_user_subif_abstract *base_subif_index;
	struct qos_hal_user_subif_abstract *port_subif_index;

	MEM_ALLOC(dp_subif_t, dp_subif);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify sub interface %s to priority level %d weight %d\n",
			__func__, netdev->name, priority, weight);
	qos_mgr_br2684_get_vcc(netdev, &vcc);
	if (vcc == NULL)
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "VCC is NULL\n");
	if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Subif Id\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	dp_subif->subif  = dp_subif->subif >> 8;

	qos_res.dev = netdev;
	qos_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (qos_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	qos_res.q_res = kmalloc(sizeof(struct dp_queue_res) * qos_res.num_q, GFP_KERNEL);
	if (qos_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	if (dp_deq_port_res_get(&qos_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res.q_res->qos_deq_port, qos_res.q_res->sch_id[0], qos_res.q_res->q_id);

	qosport = qos_res.q_res->cqm_deq_port;
	no_of_qos_ports = qos_res.num_deq_ports;

	base_subif_index = (qos_hal_user_sub_interface_info + qosport);
	qos_hal_get_subif_list_item(qosport,
			netdev,
			&subif_index,
			&base_subif_index->head);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_subif_index: %p, subif_index:%p\n",
			__func__, base_subif_index, subif_index);

	if (flags & QOS_MGR_Q_F_INGRESS) {
		subif_index = qos_hal_user_sub_interface_ingress_info + (qos_res.q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
			dp_subif->subif;
		port_subif_index = qos_hal_user_sub_interface_ingress_info + (qos_res.q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
	} else {
		subif_index = qos_hal_user_sub_interface_info + (qos_res.q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
			dp_subif->subif;
		port_subif_index = qos_hal_user_sub_interface_info + (qos_res.q_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
	}

	/* For Directpath interface */
	if ((dp_subif->port_id != 0) && (qos_res.q_res->cqm_deq_port == g_CPU_PortId)) {
		struct qos_hal_dp_res_info res = {0};
		qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

		subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
			dp_subif->subif;
		port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);

	}

	if (no_of_qos_ports > 1) { /* For LAN interface */
		if (qos_hal_get_subif_index_from_netdev(netdev, port_subif_index) == QOS_HAL_STATUS_ERR) {
			port_subif_index = qos_hal_user_sub_interface_info + ((qos_res.q_res+1)->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
			if (qos_hal_get_subif_index_from_netdev(netdev, port_subif_index) == QOS_HAL_STATUS_ERR) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> No valid port index found for the subif \n", __func__);
				ret = QOS_HAL_STATUS_ERR;
				goto QOS_RESOURCES_CLEANUP;
			}
			qosport = (qos_res.q_res+1)->cqm_deq_port;
		}
	}
	if (dp_subif->subif != 0) {
		uint8_t uIndex;
		uint32_t port_prio_level;
		uint32_t user_prio;
		uint32_t port_sched;
		uint32_t sched_id;
		uint32_t q_index = 0, shift_idx;
		uint32_t omid, new_omid = 0xFF, new_port_omid;
		uint32_t base_sched, new_sched;
		uint32_t sie, sit, qsid;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_sched_id_egress=%d port_sched_in_egress=%d\n",
				__func__, subif_index->base_sched_id_egress, subif_index->port_sched_in_egress);

		port_sched = (subif_index->port_sched_in_egress >> 3);
		port_prio_level = user_prio = priority;

		if ((subif_index->port_sched_in_user_lvl == priority) || (priority == 0)) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>No change in priority of Logical interface \n",	__func__);
			omid = qos_hal_sched_track[subif_index->base_sched_id_egress].omid;
			qos_hal_scheduler_in_weight_update(omid, weight);
			subif_index->subif_weight = weight;
			goto QOS_RESOURCES_CLEANUP;
		}

		if (qos_hal_get_q_idx_of_same_prio(port_subif_index, port_sched, port_prio_level, &q_index, &uIndex, &sched_id) != QOS_HAL_STATUS_OK) {
			uint32_t leaf;
			qos_hal_find_node_in_for_q_add(port_subif_index, DP_NODE_SCH, port_sched, port_prio_level, &shift_idx, &leaf);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>shift_idx:%d\n", __func__, shift_idx);
			qos_hal_shift_down_sbin_conn(netdev, dev_name, qosport, port_subif_index, port_sched, shift_idx, flags);
			port_prio_level = shift_idx;
			new_port_omid = (subif_index->port_sched_in_egress & ~7) | port_prio_level;
			new_omid = new_port_omid;
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connect the logical interface to User Index:%d Scheduler Block: %d\n",
					__func__, uIndex, sched_id);
			port_prio_level = uIndex;
			new_port_omid = (subif_index->port_sched_in_egress & ~7) | port_prio_level;

			qos_hal_scheduler_in_status_get(new_port_omid, &sie, &sit, &qsid);
			/* QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Index=%d SBIN: %d is %s\n", __func__, j, (sched_input << 3 | j), sie ? "enabled":"disabled"); */
			if (sie == 0) { /* no queue connected to this leaf */
			} else {
				if (sit == 0) { /* input is QID */
					struct qos_hal_equeue_create q_reconnect;
					QOS_RATE_SHAPING_CFG cfg;
					uint32_t cfg_shaper;

					/* Check whether the same type of queue is connected to this leaf */
					/* If not then return QOS_HAL_STATUS_ERR */
					q_index = qsid; /* this is the QID */

					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is attached to this Queue\n", __func__, qos_hal_queue_track[q_index].tb_index);
					cfg_shaper = qos_hal_queue_track[q_index].tb_index;
					if (cfg_shaper != 0xFF) {
						memset(&cfg, 0, sizeof(QOS_RATE_SHAPING_CFG));
						cfg.shaperid = cfg_shaper;
						cfg.dev_name = dev_name;
						cfg.flag = flags;
						qos_hal_del_queue_rate_shaper_ex(netdev, &cfg, q_index, flags);
					}

					/* Delete the Queue */
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Already queue %d is attached to this leaf\n", q_index);
					qos_hal_safe_queue_delete(netdev, dev_name, q_index, -1, -1, flags);

					/* Create a WFQ scheduler and attach to the leaf of Scheduler */
					new_sched = create_new_scheduler(sched_id, 0, QOS_HAL_POLICY_WSP, qos_hal_sched_track[sched_id].level + 1, port_prio_level);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "New Scheduler ID is %d\n", new_sched);
					qos_hal_sched_track[new_sched].policy = QOS_HAL_POLICY_WFQ;

					/* Add the deleted queue to the WFQ scheduler */
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Reconnecting the queue %d to scheduler %d\n", q_index, new_sched);
					memset(&q_reconnect, 0, sizeof(struct qos_hal_equeue_create));
					q_reconnect.index = q_index;
					q_reconnect.egress_port_number = qosport;
					q_reconnect.scheduler_input =  new_sched << 3 | 0; /* highest priority leaf */
					q_reconnect.iwgt = (qos_hal_queue_track[q_index].prio_weight == 0) ? 1 : qos_hal_queue_track[q_index].prio_weight ;
					qos_hal_egress_queue_create(&q_reconnect);
					if (cfg_shaper != 0xFF) {
						memset(&cfg, 0, sizeof(QOS_RATE_SHAPING_CFG));
						cfg.shaperid = cfg_shaper;
						cfg.dev_name = dev_name;
						cfg.flag = flags;
						qos_hal_add_queue_rate_shaper_ex(netdev, &cfg, q_index, flags);
					}
					new_omid = (new_sched << 3 | 1);

				} else {
					uint32_t free_wfq_leaf;
					qos_hal_free_wfq_scheduler_get(qosport, qsid, &free_wfq_leaf);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Free leaf is %d\n", __func__, free_wfq_leaf);
					new_omid =  free_wfq_leaf;
				}
			}
		}

		subif_index->port_sched_in_user_lvl = user_prio;
		base_sched = subif_index->base_sched_id_egress;
		omid = qos_hal_sched_track[base_sched].omid;
		if ((omid >> 3) == subif_index->port_shaper_sched) {
			base_sched = subif_index->port_shaper_sched;
			omid = qos_hal_sched_track[base_sched].omid;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Subif_index: base_sched_id_egress: %d port_sched_in_egress: %d OMID: %d\n",
				__func__, base_sched, subif_index->port_sched_in_egress, omid);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Adding the Sub interface to the leaf %d\n", __func__, port_prio_level);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New OMID %d\n", __func__, new_omid);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Port OMID %d\n", __func__, new_port_omid);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler omid %d port_sched_in_egress= %d\n", __func__, omid, subif_index->port_sched_in_egress);


		qos_hal_sched_blk_in_enable(omid, 0);
		qos_hal_sched_track[omid >> 3].leaf_mask &= ~(1 << (omid & 7));
		qos_hal_sched_out_remap(base_sched, qos_hal_sched_track[new_omid >> 3].level + 1, new_omid, 1, (weight == 0) ? 1 : weight);
		subif_index->subif_weight = weight;
		if (omid == subif_index->port_sched_in_egress) {
			qos_hal_shift_up_sbin_conn(netdev, dev_name, qosport, port_subif_index, subif_index->port_sched_in_egress, flags);
		} else {
			qos_hal_shift_up_sbin_conn(netdev, dev_name, qosport, port_subif_index, omid, flags);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler leaf mask: %d\n", __func__, qos_hal_sched_track[omid >> 3].leaf_mask);
			if (qos_hal_sched_track[omid >> 3].leaf_mask == 0) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s:%d> Nothing connected to any leaf. Delete scheduler %d\n", __func__, __LINE__, (omid >> 3));
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Omid: %d\n", __func__, qos_hal_sched_track[omid >> 3].omid);
				qos_hal_sched_blk_in_enable(qos_hal_sched_track[omid >> 3].omid, 0);
				qos_hal_scheduler_delete(omid >> 3);
				qos_hal_shift_up_sbin_conn(netdev, dev_name, qosport, port_subif_index, subif_index->port_sched_in_egress, flags);
			}
		}
		subif_index->port_sched_in_egress = new_port_omid;
	}

QOS_RESOURCES_CLEANUP:
	kfree(qos_res.q_res);
	kfree(dp_subif);
	return QOS_HAL_STATUS_OK;
}

int32_t qos_hal_get_ingress_index(uint32_t flags)
{
	int32_t index = QOS_HAL_STATUS_ERR;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> flags %x\n ", __func__, flags);
	if ((flags & QOS_MGR_Q_F_INGGRP1) == QOS_MGR_Q_F_INGGRP1)
		index = 5; /* QOS_MGR_Q_F_INGGRP1 >> 7; */
	if ((flags & QOS_MGR_Q_F_INGGRP2) == QOS_MGR_Q_F_INGGRP2)
		index = 6; /* QOS_MGR_Q_F_INGGRP2 >> 7; */
	if ((flags & QOS_MGR_Q_F_INGGRP3) == QOS_MGR_Q_F_INGGRP3)
		index = 7; /* QOS_MGR_Q_F_INGGRP3 >> 7; */
	if ((flags & QOS_MGR_Q_F_INGGRP4) == QOS_MGR_Q_F_INGGRP4)
		index = 8; /* QOS_MGR_Q_F_INGGRP4 >> 7; */
	if ((flags & QOS_MGR_Q_F_INGGRP5) == QOS_MGR_Q_F_INGGRP5)
		index = 9; /* QOS_MGR_Q_F_INGGRP5 >> 7; */
	if ((flags & QOS_MGR_Q_F_INGGRP6) == QOS_MGR_Q_F_INGGRP6)
		index = 10; /* QOS_MGR_Q_F_INGGRP6 >> 7; */

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Group index is %d\n ",
			__func__, index);
	return index;
}

int32_t qos_hal_set_ingress_grp_qmap(
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t prio_level,
		uint32_t flags,
		uint32_t q_index,
		char *tc,
		uint8_t no_of_tc)
{
	struct dp_queue_map_entry q_map;
	int32_t q_map_mask = 0, i = 0;

	/** Configure the QMAP table to connect to the Egress queue*/
	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	if ((flags & QOS_MGR_Q_F_INGGRP1) == QOS_MGR_Q_F_INGGRP1) {
		q_map.qmap.flowid = 0;
		q_map.qmap.enc = 0;
		q_map.qmap.dec = 0;
		q_map.qmap.mpe1 = 0;
		q_map.qmap.mpe2 = 0;
	}

	if ((flags & QOS_MGR_Q_F_INGGRP2) == QOS_MGR_Q_F_INGGRP2) {
		q_map.qmap.flowid = 0;
		q_map.qmap.enc = 0;
		q_map.qmap.dec = 0;
		q_map.qmap.mpe1 = 0;
		q_map.qmap.mpe2 = 1;
	}
	if ((flags & QOS_MGR_Q_F_INGGRP3) == QOS_MGR_Q_F_INGGRP3) {
		q_map.qmap.flowid = 1;
		q_map.qmap.enc = 0;
		q_map.qmap.dec = 0;
		q_map.qmap.mpe1 = 0;
		q_map.qmap.mpe2 = 0;
	}
	if ((flags & QOS_MGR_Q_F_INGGRP4) == QOS_MGR_Q_F_INGGRP4) {
		q_map.qmap.flowid = 1;
		q_map.qmap.enc = 0;
		q_map.qmap.dec = 0;
		q_map.qmap.mpe1 = 0;
		q_map.qmap.mpe2 = 1;
	}
	if ((flags & QOS_MGR_Q_F_INGGRP5) == QOS_MGR_Q_F_INGGRP5) {
		q_map.qmap.flowid = 1;
		q_map.qmap.enc = 0;
		q_map.qmap.dec = 0;
		q_map.qmap.mpe1 = 1;
		q_map.qmap.mpe2 = 1;
	}
	if ((flags & QOS_MGR_Q_F_INGGRP6) == QOS_MGR_Q_F_INGGRP6) {
		q_map.qmap.flowid = 0;
		q_map.qmap.enc = 0;
		q_map.qmap.dec = 0;
		q_map.qmap.mpe1 = 1;
		q_map.qmap.mpe2 = 1;
	}

	/*
	 * Configure the QMAP table to connect the Ingress queue
	 */
	q_map.qmap.dp_port = 0;
	if (subif_index->queue_cnt < QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
		for (i = 0; i < no_of_tc; i++) {
			q_map.qmap.class =
				QOS_HAL_CPU_HIGH_PRIO_CLASS + (*(tc + i));

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add QMAP for Port %d and TC: %d\n",
					__func__, q_map.qmap.dp_port, q_map.qmap.class);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Index of New Q: %d\n",
					__func__, q_index);

			qos_hal_add_q_map(q_index, g_Inst, &q_map, q_map_mask);
			subif_index->user_queue[subif_index->queue_cnt].qmap[i] =
				QOS_HAL_CPU_HIGH_PRIO_CLASS + (*(tc + i));
		}
		subif_index->user_queue[subif_index->queue_cnt].no_of_tc = no_of_tc;
	}

	return QOS_HAL_STATUS_OK;
}

int qos_hal_add_ingress_queue(
		struct net_device *netdev,
		char *dev_name,
		char *tc,
		uint8_t no_of_tc,
		uint32_t schedid,
		uint32_t prio_type,
		uint32_t prio_level,
		uint32_t weight,
		uint32_t flowId,
		QOS_Q_ADD_CFG *params,
		uint32_t flags)
{
	uint32_t i = 0, base_sched, qid_last_assigned = 0;
	uint32_t no_of_qos_ports = 0;
	dp_subif_t *dp_subif = NULL;
	int32_t qosport = 0;
	uint32_t sched_algo = 0;
	int32_t ret = QOS_HAL_STATUS_OK;

	struct qos_hal_user_subif_abstract *ingress_subif_index = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *base_subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_index = NULL;
	struct qos_hal_user_subif_abstract *subif_index_port = NULL;
	struct dp_queue_res *cpu_qos_res = NULL;
	uint32_t nof_of_cpu_qos_ports = 0;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n",
			__func__);

	MEM_ALLOC(dp_subif_t, dp_subif);
	if (prio_type == QOS_MGR_SCHED_SP)
		sched_algo = QOS_HAL_POLICY_WSP;
	else if (prio_type == QOS_MGR_SCHED_WFQ)
		sched_algo = QOS_HAL_POLICY_WRR;
	else if (prio_type == QOS_MGR_SCHED_WRR)
		sched_algo = QOS_HAL_POLICY_WRR;

	qosport = qos_hal_get_ingress_index(flags);
	if (qosport == QOS_HAL_STATUS_ERR) {
		ret = QOS_MGR_FAILURE;
		goto QOS_RESOURCES_CLEANUP;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Group index is %d\n",
			__func__, qosport);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add Ingress Queue for Datapath Port %d\n",
			__func__, dp_subif->port_id);

	for (i = 0; i < no_of_qos_ports; i++) {
		(qos_hal_user_sub_interface_ingress_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->is_logical =
			(dp_subif->subif == 0) ? 0 : 1;
		(qos_hal_user_sub_interface_ingress_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif)->qos_port_idx = qosport;
	}

	base_subif_index = qos_hal_user_sub_interface_ingress_info + qosport;
	dp_subif->port_id = qosport;

	if (!g_IngressQueueScheduler) {
		uint32_t old_omid, sched_port, q_new;
		uint32_t q_index = 0xFF;
		struct qos_hal_sched_track_info Snext = {0};
		struct dp_queue_map_entry *q_map_get = NULL;
		int32_t no_entries = 0;

		if (qos_hal_get_q_res_from_netdevice(
				netdev,
				netdev->name,
				0 /*schedid*/,
				dp_subif,
				sched_algo,
				&nof_of_cpu_qos_ports,
				(void **)&cpu_qos_res,
				(flags | QOS_MGR_Q_F_INGRESS)) == QOS_HAL_STATUS_ERR) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Error: Failed to get resources from Netdevice\n",
					__func__, __LINE__);
			kfree(cpu_qos_res);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		qos_hal_get_subif_list_item(qosport,
			NULL,
			&ingress_subif_index,
			&base_subif_index->head);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Ingress Sub interface index %p\n",
				__func__, ingress_subif_index);

		/* Create the WFQ Root Scheduler */
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Create WFQ Scheduler for CPU Ingress Queue\n",
				__func__);

		old_omid = qos_hal_port_track[g_CPU_PortId].input_sched_id;
		if (old_omid < QOS_HAL_MAX_SCHEDULER)
			Snext = qos_hal_sched_track[old_omid];

		q_index = cpu_qos_res->q_id; /* this is the QID */
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Already queue %d is attached to this leaf\n",
				__func__, q_index);

		qos_hal_get_q_map(g_Inst, q_index, &no_entries, &q_map_get, 0);

		/* Now create the Port scheduler for the interface */
		sched_port = create_new_scheduler(
					cpu_qos_res->cqm_deq_port,
					1,
					QOS_HAL_POLICY_WRR,
					1,
					1);
		qos_hal_sched_track[sched_port].policy = sched_algo;
		g_Root_sched_id_Ingress = cpu_qos_res->cqm_deq_port;
		q_new = qos_hal_queue_add(
				netdev,
				netdev->name,
				cpu_qos_res->cqm_deq_port,
				sched_port,
				sched_algo,
				prio_level,
				weight,
				ingress_subif_index,
				flags);

		port_index = qos_hal_user_sub_interface_ingress_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT);
		port_index->base_sched_id_egress = sched_port;
		ingress_subif_index->base_sched_id_egress = sched_port;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Base SB:[%d]\n",
				__func__, ingress_subif_index->base_sched_id_egress);

		g_IngressQueueScheduler++;

		qid_last_assigned = ++(ingress_subif_index->qid_last_assigned);
		qos_hal_queue_track[q_new].user_q_idx = ingress_subif_index->queue_cnt;

		ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].queue_index = q_new;
		ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].queue_type = sched_algo;
		ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].prio_level = prio_level;
		ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].sbin = qos_hal_queue_track[q_new].sched_input;
		ingress_subif_index->user_queue[ingress_subif_index->queue_cnt].qid = qid_last_assigned;
		ingress_subif_index->queue_cnt++ ;

		ingress_subif_index->dp_subif.port_id = dp_subif->port_id;
		ingress_subif_index->dp_subif.subif = dp_subif->subif;
		if ((flags & QOS_MGR_F_PPPOATM) == QOS_MGR_F_PPPOATM)
			snprintf(ingress_subif_index->dp_subif.ifname, sizeof(ingress_subif_index->dp_subif.ifname), "%s", dev_name);
		else
			snprintf(ingress_subif_index->dp_subif.ifname, sizeof(ingress_subif_index->dp_subif.ifname), "%s", netdev->name);

		if ((flags & QOS_MGR_Q_F_DEFAULT) == QOS_MGR_Q_F_DEFAULT) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Priority Level for Default Q : %d\n",
					__func__, prio_level);
			ingress_subif_index->default_prio = prio_level;
		}
		ingress_subif_index->netdev = netdev;
		qos_hal_dump_subif_queue_info(ingress_subif_index);

		qos_hal_set_ingress_grp_qmap(
				ingress_subif_index,
				prio_level,
				flags,
				q_new,
				tc,
				no_of_tc);
		kfree(cpu_qos_res);
	} else {
		/* Root Scheduler is already connected */
		uint32_t q_new, sched_port;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Root Scheduler %d is already created\n",
				__func__, g_Root_sched_id_Ingress);

		qos_hal_get_subif_list_item(qosport,
				NULL,
				&port_index,
				&base_subif_index->head);
		ingress_subif_index = port_index;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_subif_index: %p, base_subif_index->head:%p port_index:%p\n",
				__func__, base_subif_index, base_subif_index->head, port_index);

		if (port_index->base_sched_id_egress == 0xFF) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Create Port Scheduler for the port\n",
					__func__);
			sched_port = create_new_scheduler(
						g_Root_sched_id_Ingress,
						0,
						QOS_HAL_POLICY_WFQ,
						qos_hal_sched_track[g_Root_sched_id_Ingress].level + 1,
						1);
			qos_hal_sched_track[sched_port].policy = QOS_HAL_POLICY_WSP;

			port_index->base_sched_id_egress = sched_port;
			for (i = 0; i < QOS_HAL_MAX_SUB_IFID_PER_PORT; i++) {
				(port_index + i)->port_sched_in_egress = sched_port;
			}
		} else
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Scheduler for the port %d is %d\n",
						__func__, dp_subif->port_id, ingress_subif_index->port_sched_in_egress);

		subif_index = ingress_subif_index;
		subif_index_port = port_index;
		if (dp_subif->subif != 0) {
			uint32_t port_prio_level;
			if (subif_index->queue_cnt == 0 &&
					subif_index->base_sched_id_egress == 0xFF) {
				uint8_t leaf;
				uint32_t port_sched;
				qos_hal_find_min_prio_level_of_port(
						subif_index_port,
						&port_sched,
						&port_prio_level);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add First Queue of the subif %d to the Port Scheduler %d\n",
						__func__, dp_subif->subif, port_sched);
				subif_index->port_sched_in_user_lvl = port_prio_level;

				if (qos_hal_scheduler_free_input_get(
							port_sched,
							&leaf) < 0) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No free leaf\n", __func__);
					kfree(dp_subif);
					return QOS_HAL_STATUS_ERR;
				} else
					port_prio_level = leaf;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Adding the Sub interface to the leaf %d\n",
						__func__, port_prio_level);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port_sched_in_egress= %d\n",
						__func__, subif_index->port_sched_in_egress);
				if (subif_index->port_sched_in_egress >> 3 == INVALID_SCHEDULER_BLOCK_ID) {
					qos_hal_connect_subif_to_port(
							netdev,
							netdev->name,
							subif_index,
							subif_index_port,
							port_sched,
							port_prio_level,
							weight,
							flags);
				} else
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler is already there\n",
							__func__);

				base_sched = subif_index->base_sched_id_egress;

			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Minimum 1 Queue is already added for the subif %d to the Port Scheduler \n",
							__func__, dp_subif->subif);
				base_sched = subif_index->base_sched_id_egress;
			}
		} else {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> For the main physical interface of the Port\n",
					__func__);
			base_sched = subif_index->base_sched_id_egress;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base scheduler for the interface : [%d]\n",
				__func__, base_sched);

		q_new = qos_hal_queue_add(
				netdev,
				netdev->name,
				g_CPU_PortId,
				base_sched,
				sched_algo,
				prio_level,
				weight,
				subif_index,
				flags);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> User Queue/Prio Level/Weight --> %d/%d/%d is mapped to Queue index-->%d\n",
				__func__, subif_index->queue_cnt, prio_level, weight, q_new);

		qid_last_assigned = ++(subif_index->qid_last_assigned);
		qos_hal_queue_track[q_new].user_q_idx = subif_index->queue_cnt;

		subif_index->user_queue[subif_index->queue_cnt].queue_index = q_new;
		subif_index->user_queue[subif_index->queue_cnt].queue_type = sched_algo;
		subif_index->user_queue[subif_index->queue_cnt].prio_level = prio_level;
		subif_index->user_queue[subif_index->queue_cnt].weight = weight;
		subif_index->user_queue[subif_index->queue_cnt].sbin =
			qos_hal_queue_track[q_new].sched_input;
		subif_index->user_queue[subif_index->queue_cnt].qid =
			qid_last_assigned;
		subif_index->netdev = netdev ;
		subif_index->queue_cnt++ ;
		subif_index->dp_subif.port_id = dp_subif->port_id;
		subif_index->dp_subif.subif = dp_subif->subif;

		if ((flags & QOS_MGR_F_PPPOATM) == QOS_MGR_F_PPPOATM)
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname), "%s", dev_name);
		else
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname), "%s", netdev->name);

		if ((flags & QOS_MGR_Q_F_DEFAULT) == QOS_MGR_Q_F_DEFAULT) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Priority Level for Default Q : %d\n",
					__func__, prio_level);
			subif_index->default_prio = prio_level;
		}
		qos_hal_dump_subif_queue_info(subif_index);
		qos_hal_set_ingress_grp_qmap(subif_index, prio_level, flags, q_new, tc, no_of_tc);
	}

QOS_RESOURCES_CLEANUP:
	kfree(dp_subif);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return qid_last_assigned ;

}

static void qos_hal_store_queue_wred_param(
		uint32_t drop_mode,
		struct qos_hal_equeue_cfg *q_param,
		struct qos_hal_equeue_cfg *subif_eqcfg)
{

	subif_eqcfg->drop_threshold_green_min =
			q_param->drop_threshold_green_min;
	subif_eqcfg->drop_threshold_green_max =
			q_param->drop_threshold_green_max;
	subif_eqcfg->drop_threshold_yellow_min =
			q_param->drop_threshold_yellow_min;
	subif_eqcfg->drop_threshold_yellow_max =
			q_param->drop_threshold_yellow_max;
	subif_eqcfg->drop_probability_green =
			q_param->drop_threshold_yellow_min;
	subif_eqcfg->drop_probability_yellow =
			q_param->drop_probability_yellow;
#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
	subif_eqcfg->drop_threshold_unassigned =
			q_param->drop_threshold_unassigned;
	subif_eqcfg->drop_threshold_red =
			q_param->drop_threshold_red;
#endif
	subif_eqcfg->enable = 1;
	if (drop_mode == QOS_MGR_DROP_WRED)
		subif_eqcfg->wred_enable = 1;
	else if (drop_mode == QOS_MGR_DROP_TAIL)
		subif_eqcfg->wred_enable = 0;
	else
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Wrong Mode\n",
				__func__);
}

int qos_hal_add_egress_queue(
		struct net_device *netdev,
		char *dev_name,
		char *tc, /*tc[MAX_TC_NUM],*/
		uint8_t no_of_tc,
		uint32_t schedid,
		uint32_t prio_type,
		uint32_t prio_level,
		uint32_t weight,
		uint32_t flowId,
		QOS_Q_ADD_CFG *param,
		uint32_t flags)
{
	uint32_t i = 0, base_sched, q_new, qid_last_assigned = 0;
	uint32_t no_of_qos_ports;
	uint32_t sched_algo = 0;
	struct dp_queue_res *qos_res = NULL;
	dp_subif_t *dp_subif = NULL;
	uint32_t port;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *subif_index_port;
	struct qos_hal_user_subif_abstract *base_subif_index;
	struct qos_hal_equeue_cfg q_param;
	int32_t qosport;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter -->\n",
			__func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Param portid = %d\n",
			__func__, param->portid);

	MEM_ALLOC(dp_subif_t, dp_subif);
	dp_subif->port_id = param->portid;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add Egress Q for netdevice %s\n",
			__func__, netdev->name);
#if IS_ENABLED(CONFIG_X86_INTEL_LGM) || IS_ENABLED(CONFIG_PRX300_HGU)
	if (dp_subif->port_id <= 0) {
		dp_subif->port_id = -1;
	}
#endif

	if (prio_type == QOS_MGR_SCHED_SP)
		sched_algo = QOS_HAL_POLICY_WSP;
	else if (prio_type == QOS_MGR_SCHED_WFQ)
		sched_algo = QOS_HAL_POLICY_WRR;
	else if (prio_type == QOS_MGR_SCHED_WRR)
		sched_algo = QOS_HAL_POLICY_WRR;

	if (qos_hal_get_q_res_from_netdevice(
			netdev,
			netdev->name,
			flowId,
			dp_subif,
			sched_algo,
			&no_of_qos_ports,
			(void **)&qos_res,
			flags) != QOS_HAL_STATUS_OK) {

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>:<%d> Fail to get resources\n",
						__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	port = dp_subif->port_id;
	qosport = qos_res->cqm_deq_port;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add Egress Q for DP Port %d\n",
			__func__, dp_subif->port_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No of Qs for Port %d is %d\n",
			__func__,
			qosport,
			qos_hal_port_track[qosport].no_of_queues);

	if (qos_hal_port_track[qosport].no_of_queues ==
				QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Max No of Qs reached for DP Port %d\n",
						__func__, dp_subif->port_id);
		kfree(qos_res);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> flags=%d\n", __func__, flags);
	if ((dp_subif->port_id != 0)
		&& (qosport != g_CPU_PortId)) { /* Egress handling except CPU port */

		subif_index_port = qos_hal_user_sub_interface_info + qosport;
		base_subif_index = (qos_hal_user_sub_interface_info + qosport);

		qos_hal_get_subif_list_item(qosport,
				netdev,
				&subif_index,
				&base_subif_index->head);

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> base_subif_index: %p, subif_index:%p\n",
				__func__, base_subif_index, subif_index);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No of Qs for subif is %d\n",
				__func__, subif_index->queue_cnt);

		if (subif_index->is_logical) { /* for the logical interface */
			uint32_t port_prio_level;
			uint32_t user_prio;
			uint32_t port_sched;

			/** By default the logical interface will be connected to the port priority
			  level of (QOS_HAL_MAX_PRIORITY_LEVEL - 1).  */
			qos_hal_find_min_prio_level_of_port(subif_index_port, &port_sched, &port_prio_level);
			port_prio_level = user_prio = (QOS_HAL_MAX_PRIORITY_LEVEL - 1);

			/** Depends on the value of high priority queue limit, the selection of port
			  schedler and qos port is required.
			  This selection is required for LAN port.
			 */
			if (port_prio_level > high_prio_q_limit) {
				if (no_of_qos_ports > 1) {
					port_sched = base_sched = (qos_res)->sch_id[0];
					qosport = (qos_res)->cqm_deq_port;
					subif_index = qos_hal_user_sub_interface_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connect logical interface to Low Priority Port:%d SB: %d \n",
							__func__, qosport, base_sched);
				}
			} else {
				if (no_of_qos_ports > 1) {
					port_sched = base_sched = (qos_res+1)->sch_id[0];
					qosport = (qos_res+1)->cqm_deq_port;
					subif_index = qos_hal_user_sub_interface_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connect logical interface to High  Priority Port:%d SB: %d \n",
							__func__, qosport, base_sched);
				}
			}

			if (qos_hal_is_priority_scheduler_for_port(qosport) == QOS_HAL_STATUS_OK)
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Scheduler for the port %d is %d\n",
						__func__, qosport, qos_hal_port_track[qosport].port_sched_id);
			else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add Port Scheduler for the port [%d]\n",
						__func__, qosport);
				if (qos_hal_create_port_scheduler(qosport, QOS_HAL_POLICY_WRR, &port_sched) == QOS_HAL_STATUS_ERR) {
					
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Create Port Scheduler failed\n",
							__func__);
					kfree(dp_subif);
					return QOS_HAL_STATUS_ERR;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Scheduler is [%d]\n",
						__func__, port_sched);
				subif_index_port->base_sched_id_egress = port_sched;
				qos_hal_port_track[qosport].port_sched_id = port_sched;
			}
			if (subif_index->queue_cnt == 0 && subif_index->base_sched_id_egress == 0xFF) {
				uint8_t leaf, uIndex;
				uint32_t sched_id;
				uint32_t q_index = 0, shift_idx;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add First Queue of the subif %d to the Port Scheduler %d\n",
						__func__, dp_subif->subif, port_sched);

				/** In case already 8 queues are already created for the base interface
				  then, there is no free leaf. We need to create the scheduler level.
				 */
				if (qos_hal_scheduler_free_input_get(
							port_sched,
							&leaf) < 0) {
					uint32_t omid, parent;
					uint32_t next_sched;
					struct qos_hal_sched_track_info Snext;
					struct qos_hal_sched_track_info Stemp;

					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port:%d has leaf mask:%d .. No Free leaf\n",
							__func__, port_sched, qos_hal_sched_track[port_sched].leaf_mask);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Creating a new level\n", __func__);
					Snext = qos_hal_sched_track[port_sched];
					parent = port_sched;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "parent %d next_sched_id %d\n", parent, Snext.next_sched_id);
					if (Snext.leaf_mask == 0xFF && qos_hal_sched_track[Snext.next_sched_id].priority_selector != 2) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Adding a new level for SB:%d\n", __func__, port_sched);
						qos_hal_add_scheduler_level(
								qosport,
								port_sched,
								0,
								&parent,
								sched_algo,
								&omid);
						qos_hal_sched_track[parent].priority_selector = 1;
						Snext.priority_selector = 2;
						qos_hal_sched_track[omid].priority_selector = 3;
						subif_index_port->base_sched_id_egress = parent;
						Stemp = qos_hal_sched_track[parent];
						next_sched = Stemp.next_sched_id;
						qos_hal_q_align_for_priority_selector(
								netdev,
								dev_name,
								qosport,
								subif_index_port,
								next_sched,
								qos_hal_sched_track[next_sched].peer_sched_id,
								QOS_HAL_Q_PRIO_LEVEL, flags);
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Sched:[%d]\n",
								__func__, omid);
						port_sched = omid;

					}

				}

				if (qos_hal_get_q_idx_of_same_prio(
							subif_index_port,
							port_sched,
							port_prio_level,
							&q_index,
							&uIndex,
							&sched_id) != QOS_HAL_STATUS_OK) {
					uint32_t p_level;
					qos_hal_find_node_in_for_q_add(subif_index_port, port_sched, DP_NODE_SCH, port_prio_level, &shift_idx, &p_level);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>shift_idx:%d\n", __func__, shift_idx);
					/* qos_hal_shift_down_sbin_conn(netdev, dev_name, qosport, subif_index_port, port_sched, shift_idx, flags);
					port_prio_level = shift_idx; */
					port_prio_level = p_level;
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Connect the logical interface to User Index:%d Scheduler Block: %d\n",
							__func__, uIndex, sched_id);
					port_prio_level = uIndex;
				}

				port_prio_level = leaf;

				subif_index->port_sched_in_user_lvl = user_prio;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Adding the Sub interface to the leaf %d\n",
						__func__, port_prio_level);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port_sched_in_egress= %d\n",
						__func__, subif_index->port_sched_in_egress);

				if (subif_index->port_sched_in_egress >> 3 == INVALID_SCHEDULER_BLOCK_ID) {
					qos_hal_connect_subif_to_port(
							netdev,
							dev_name,
							subif_index,
							subif_index_port,
							port_sched,
							port_prio_level,
							weight,
							flags);
				} else
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler is already there\n", __func__);

				base_sched = subif_index->base_sched_id_egress;

			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Minimum 1 Queue is already added for the subif %d to the Port Scheduler \n",
						__func__, dp_subif->subif);
				base_sched = subif_index->base_sched_id_egress;
			}
		} else {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> For the physical interface of the Port\n",
					__func__);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Prio level %d High Prio Limit %d\n",
					__func__, prio_level, high_prio_q_limit);
			base_sched = subif_index->base_sched_id_egress;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base sched for the interface is %d\n",
				__func__, base_sched);

		q_new = qos_hal_queue_add(
				netdev,
				dev_name,
				qosport,
				base_sched,
				sched_algo,
				prio_level,
				weight,
				subif_index,
				flags);
		if (q_new == QOS_HAL_STATUS_ERR) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue Add failed\n", __func__);
			kfree(dp_subif);
			return QOS_MGR_FAILURE;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "User Queue/Prio Level/Weight --> %d/%d/%d is mapped to Queue index-->%d \n",
				subif_index->queue_cnt, prio_level, weight, q_new);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH,  "flags=%d\n", flags);

		memset(&q_param, 0, sizeof(struct qos_hal_equeue_cfg));
		q_param.index = q_new;
		q_param.enable = 1;
		if (param->drop.mode == QOS_MGR_DROP_WRED) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify WRED params\n", __func__);
			q_param.drop_threshold_green_min = param->drop.wred.min_th0;
			q_param.drop_threshold_green_max = param->drop.wred.max_th0;
			q_param.drop_threshold_yellow_min = param->drop.wred.min_th1;
			q_param.drop_threshold_yellow_max = param->drop.wred.max_th1;
			q_param.drop_probability_green = param->drop.wred.max_p0;
			q_param.drop_probability_yellow = param->drop.wred.max_p1;
#if !IS_ENABLED(CONFIG_X86_INTEL_LGM)
			q_param.drop_threshold_unassigned = 500;
			q_param.drop_threshold_red = 8000;
#endif
			q_param.wred_enable = 1;
			q_param.avg_weight = param->drop.wred.weight;
		} else if (param->drop.mode == QOS_MGR_DROP_TAIL) {
			q_param.wred_enable = 0;
		}

		if (qos_hal_egress_queue_cfg_set(&q_param) != QOS_HAL_STATUS_OK) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Failed to config DT/WRED params\n",
					__func__);
			kfree(dp_subif);
			return QOS_MGR_FAILURE;
		}

		if ((flags & QOS_MGR_Q_F_DEFAULT) == QOS_MGR_Q_F_DEFAULT) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Priority Level for Default Q : %d\n", prio_level);
			subif_index->default_prio = prio_level;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Default queue prio=%d prio_level=%d \n",
					__func__, subif_index->default_prio, prio_level);

		qos_hal_handle_q_map(
				q_new,
				prio_level,
				schedid,
				tc,
				no_of_tc,
				flowId,
				subif_index,
				flags);

		qid_last_assigned = ++(subif_index->qid_last_assigned);
		qos_hal_queue_track[q_new].user_q_idx = subif_index->queue_cnt;

		subif_index->user_queue[subif_index->queue_cnt].queue_index = q_new;
		subif_index->user_queue[subif_index->queue_cnt].queue_type = sched_algo;
		subif_index->user_queue[subif_index->queue_cnt].prio_level = prio_level;
		subif_index->user_queue[subif_index->queue_cnt].weight = weight;
		subif_index->user_queue[subif_index->queue_cnt].sbin = qos_hal_queue_track[q_new].sched_input;
		subif_index->user_queue[subif_index->queue_cnt].qid = qid_last_assigned;

		qos_hal_store_queue_wred_param(
				param->drop.mode,
				&q_param,
				&subif_index->user_queue[subif_index->queue_cnt].ecfg);
		subif_index->netdev = netdev ;
		subif_index->queue_cnt++ ;
		subif_index->dp_subif.port_id = dp_subif->port_id;
		subif_index->dp_subif.subif = dp_subif->subif;
		if ((flags & QOS_MGR_F_PPPOATM) == QOS_MGR_F_PPPOATM) {
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname), "%s", dev_name);
		} else
			snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname), "%s", netdev->name);

		qos_hal_dump_subif_queue_info(subif_index);
		qos_hal_get_detailed_queue_map(q_new);

	} else { /* Egress handling for CPU Port = DP Egress */
		/* Directpath Egress */
		if (!g_DpEgressQueueScheduler) {
			/* Create the WFQ Scheduler */
			g_DpEgressQueueScheduler++;
		} else {
			/* Root Scheduler is already connected */
			uint32_t sched_port;
			struct qos_hal_dp_res_info res = {0};
			struct qos_hal_user_subif_abstract *ingress_subif_index = NULL;
			struct qos_hal_user_subif_abstract *port_index = NULL;
			struct dp_queue_map_entry q_map;
			int32_t q_map_mask = 0;

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Root Scheduler for directpath %d is already created\n", __func__, g_Root_sched_id_DpEgress);
			if (dp_get_netif_subifid(netdev, NULL, NULL, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __func__, __LINE__);
				if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __func__, __LINE__);
					if (qos_hal_get_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from qos_hal_get_subitf_via_ifname \n", __func__, __LINE__);
						kfree(dp_subif);
						return QOS_HAL_STATUS_ERR;
					}
				}
			}

			qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> QOS Port is %d \n", __func__, res.dp_egress_res.dp_port);
			port_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
			ingress_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port *
			QOS_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif >> 8);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "subif index is %p\n", ingress_subif_index);

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Base scheduler Id %d\n", port_index->base_sched_id_egress);
			if (port_index->base_sched_id_egress == 0xFF) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Create Port Scheduler for the port \n");
				sched_port = create_new_scheduler(g_Root_sched_id_DpEgress, 0, QOS_HAL_POLICY_WFQ, qos_hal_sched_track[g_Root_sched_id_DpEgress].level + 1, 1);

				port_index->base_sched_id_egress = sched_port;
				for (i = 0; i < QOS_HAL_MAX_SUB_IFID_PER_PORT; i++) {
					(port_index + i)->port_sched_in_egress = sched_port;
				}
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port Scheduler for the port %d is %d\n",
						__func__, dp_subif->port_id, ingress_subif_index->port_sched_in_egress);
			}
			subif_index = ingress_subif_index;
			subif_index_port = port_index;
			if (dp_subif->subif != 0) {
				uint32_t port_prio_level;
				if (subif_index->queue_cnt == 0 && subif_index->base_sched_id_egress == 0xFF) {
					uint8_t leaf, uIndex;
					uint32_t port_sched, sched_id;
					uint32_t q_index = 0, shift_idx;
					uint32_t user_prio;
					qos_hal_find_min_prio_level_of_port(subif_index_port, &port_sched, &port_prio_level);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Add First Queue of the subif %d to the Port Scheduler %d\n",
							__func__, (dp_subif->subif >> 8), port_sched);
					subif_index->port_sched_in_user_lvl = port_prio_level;
					port_prio_level = user_prio = QOS_HAL_MAX_PRIORITY_LEVEL - 1;

					/** In case already 8 queues are already created for the base interface
					  then, there is no free leaf. We need to create the scheduler level.
					 */
					if (qos_hal_scheduler_free_input_get(
								port_sched,
								&leaf) < 0) {
						uint32_t omid, parent;
						struct qos_hal_sched_track_info Snext;
						struct qos_hal_sched_track_info Stemp;

						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port:%d has leaf mask:%d .. No Free leaf\n",
								__func__, port_sched, qos_hal_sched_track[port_sched].leaf_mask);
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Creating a new level\n", __func__);
						Snext = qos_hal_sched_track[port_sched];
						parent = Snext.id;
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "parent %d next_sched_id %d\n", parent, Snext.next_sched_id);
						if (Snext.leaf_mask == 0xFF && qos_hal_sched_track[Snext.next_sched_id].priority_selector != 2) {
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Adding a new level for SB:%d\n", __func__, port_sched);
							qos_hal_add_scheduler_level(port, port_sched, 0, &parent, prio_type, &omid);
							qos_hal_sched_track[parent].priority_selector = 1;
							qos_hal_sched_track[Snext.id].priority_selector = 2;
							qos_hal_sched_track[omid].priority_selector = 3;
							subif_index_port->base_sched_id_egress = parent;
							Stemp = qos_hal_sched_track[parent];
							qos_hal_q_align_for_priority_selector(netdev, dev_name, port, subif_index_port,
									qos_hal_sched_track[Stemp.next_sched_id].id,
									qos_hal_sched_track[Stemp.next_sched_id].peer_sched_id, QOS_HAL_Q_PRIO_LEVEL, flags);
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Sched: %d\n", __func__, omid);
							port_sched = omid;

						}
					}

					if (qos_hal_get_q_idx_of_same_prio(subif_index_port, port_sched, port_prio_level, &q_index, &uIndex, &sched_id) != QOS_HAL_STATUS_OK) {
						uint32_t leaf;
						qos_hal_find_node_in_for_q_add(subif_index_port, DP_NODE_SCH, port_sched, port_prio_level, &shift_idx, &leaf);
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>shift_idx:%d\n", __func__, shift_idx);
						qos_hal_shift_down_sbin_conn(netdev, dev_name, qosport, subif_index_port, port_sched, shift_idx, flags);
						port_prio_level = shift_idx;
					} else {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "User Index:%d Scheduler Block: %d\n", uIndex, sched_id);
						port_prio_level = uIndex;
					}
					subif_index->port_sched_in_user_lvl = user_prio;

					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Adding the Sub interface to the leaf %d\n", __func__, port_prio_level);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port_sched_in_egress= %d\n", __func__, subif_index->port_sched_in_egress);
					if (subif_index->port_sched_in_egress >> 3 == INVALID_SCHEDULER_BLOCK_ID) {
						qos_hal_connect_subif_to_port(netdev, dev_name, subif_index, subif_index_port, port_sched, port_prio_level, weight, flags);
					} else {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler is already there\n", __func__);
					}

					base_sched = subif_index->base_sched_id_egress;

				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Minimum 1 Queue is already added for the subif %d to the Port Scheduler\n",
						dp_subif->subif);
					base_sched = subif_index->base_sched_id_egress;
				}
			} else {

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> For the Physical interface of the Port \n", __func__);
				base_sched = subif_index->base_sched_id_egress;
			}

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Base scheduler for the interface is %d \n", __func__, base_sched);

			q_new = qos_hal_queue_add(netdev, dev_name, g_CPU_PortId, base_sched, prio_type, prio_level, weight, subif_index, flags);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>User Queue/Prio Level/Weight --> %d/%d/%d is mapped to Queue index-->%d \n",
					__func__, subif_index->queue_cnt, prio_level, weight, q_new);

			memset(&q_param, 0, sizeof(struct qos_hal_equeue_cfg));
			q_param.index = q_new;
			q_param.enable = 1;
			if (param->drop.mode == QOS_MGR_DROP_WRED) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify WRED params \n", __func__);
				q_param.drop_threshold_green_min = param->drop.wred.min_th0;
				q_param.drop_threshold_green_max = param->drop.wred.max_th0;
				q_param.drop_threshold_yellow_min = param->drop.wred.min_th1;
				q_param.drop_threshold_yellow_max = param->drop.wred.min_th1;
				q_param.drop_probability_green = param->drop.wred.max_p0;
				q_param.drop_probability_yellow = param->drop.wred.max_p1;
				q_param.wred_enable = 1;
				q_param.avg_weight = param->drop.wred.weight;
			} else if (param->drop.mode == QOS_MGR_DROP_TAIL) {
				q_param.wred_enable = 0;
			}

			qos_hal_egress_queue_cfg_set(&q_param);

			qid_last_assigned = ++(subif_index->qid_last_assigned);
			qos_hal_queue_track[q_new].user_q_idx = subif_index->queue_cnt;

			subif_index->user_queue[subif_index->queue_cnt].queue_index = q_new;
			subif_index->user_queue[subif_index->queue_cnt].queue_type = prio_type;
			subif_index->user_queue[subif_index->queue_cnt].prio_level = prio_level;
			subif_index->user_queue[subif_index->queue_cnt].weight = weight;
			subif_index->user_queue[subif_index->queue_cnt].sbin = qos_hal_queue_track[q_new].sched_input;
			subif_index->user_queue[subif_index->queue_cnt].qid = qid_last_assigned;

			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_min = param->drop.wred.min_th0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_green_max = param->drop.wred.max_th0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_min = param->drop.wred.min_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_threshold_yellow_max = param->drop.wred.min_th1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_green = param->drop.wred.max_p0;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.drop_probability_yellow = param->drop.wred.max_p1;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.weight = param->drop.wred.weight;;
			subif_index->user_queue[subif_index->queue_cnt].ecfg.enable = 1;
			if (param->drop.mode == QOS_MGR_DROP_WRED) {
				subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 1;
			} else if (param->drop.mode == QOS_MGR_DROP_TAIL) {
				subif_index->user_queue[subif_index->queue_cnt].ecfg.wred_enable = 0;
			}
			subif_index->netdev = netdev ;
			subif_index->queue_cnt++ ;

			subif_index->dp_subif.port_id = dp_subif->port_id;
			subif_index->dp_subif.subif = dp_subif->subif;
			if ((flags & QOS_MGR_F_PPPOATM) == QOS_MGR_F_PPPOATM) {
				snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname), "%s", dev_name);
			} else
				snprintf(subif_index->dp_subif.ifname, sizeof(subif_index->dp_subif.ifname), "%s", netdev->name);

			qos_hal_dump_subif_queue_info(subif_index);

			if ((flags & QOS_MGR_Q_F_DEFAULT) == QOS_MGR_Q_F_DEFAULT) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Priority Level for Default Q : %d\n", prio_level);
				subif_index->default_prio = prio_level;
			}
			if ((prio_level == subif_index->default_prio) && ((flags & QOS_MGR_Q_F_DEFAULT) == QOS_MGR_Q_F_DEFAULT)) {
				struct dp_queue_map_entry q_map;
				int32_t q_map_mask = 0, x = 0;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Prio Level/No Of QOS ports --> %d/%d/ \n", __func__, prio_level, no_of_qos_ports);
				/** Configure the QMAP table to connect to the Egress queue */
				memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

				q_map.qmap.dp_port = dp_subif->port_id;

				if (dp_subif->subif != 0) { /* for the logical interface */
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Deafult Q --> Flow Id %d for VLAN interface subif: %d\n",
							__func__, flowId, dp_subif->subif);
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
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Map Default Q: %d\n", __func__, q_new);
					qos_hal_add_q_map(q_new, g_CPU_PortId, &q_map, q_map_mask);
					subif_index->default_q = q_new;
					for (x = 1; x < subif_index->queue_cnt; x++) {
						q_map_mask = 0;
						/** Configure the QMAP table to connect to the Egress queue*/
						memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

						q_map.qmap.dp_port = dp_subif->port_id;
						q_map.qmap.flowid = flowId;

						q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
						for (i = 0; i < MAX_TC_NUM; i++) {
							q_map.qmap.class = subif_index->user_queue[x].qmap[i];
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Q --> Add QMAP for Port %d and TC: %d\n",
									__func__, q_map.qmap.dp_port, q_map.qmap.class);
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Q --> User Index %d Q Index: %d\n",
									__func__, x, subif_index->user_queue[x].queue_index);
							qos_hal_add_q_map(subif_index->user_queue[x].queue_index, g_CPU_PortId, &q_map, q_map_mask);
						}
					}

				} else { /* default Q of Physical interface */
					q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
						CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
						CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_TC_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Map Default Q: %d\n", __func__, q_new);
					qos_hal_add_q_map(q_new, g_CPU_PortId, &q_map, q_map_mask);
					subif_index->default_q = q_new;
					for (x = 1; x < subif_index->queue_cnt; x++) {
						q_map_mask = 0;
						/** Configure the QMAP table to connect to the Egress queue*/
						memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

						q_map.qmap.dp_port = dp_subif->port_id;
						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;

						for (i = 0; i < MAX_TC_NUM; i++) {
							q_map.qmap.class = subif_index->user_queue[x].qmap[i];
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Deafult Q --> Add QMAP for Port %d and TC: %d\n",
									__func__, q_map.qmap.dp_port, q_map.qmap.class);
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Q --> User Index %d Q Index: %d\n",
									__func__, x, subif_index->user_queue[x].queue_index);
							qos_hal_add_q_map(subif_index->user_queue[x].queue_index, g_CPU_PortId, &q_map, q_map_mask);
						}
					}
				}
			} else { /* not for default queue */
				struct dp_queue_map_entry q_map;
				uint32_t q_map_mask = 0;
				/** Configure the QMAP table to connect to the Egress queue*/
				memset(&q_map, 0, sizeof(struct dp_queue_map_entry));
				q_map.qmap.dp_port = dp_subif->port_id;
				for (i = 0; i < MAX_TC_NUM; i++) {
					if (tc[i] != 0)
						q_map.qmap.class = *(tc+i);
					else
						break;

					if (dp_subif->subif != 0) { /*  for the logical interface */
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Flow Id %d for VLAN interface subif: %d\n", __func__, flowId,
							dp_subif->subif);
						q_map.qmap.flowid = flowId;
						if (flowId > 3) {
							q_map.qmap.class |= 8;
							q_map.qmap.flowid = (flowId & 0x3);
						}
						q_map_mask |= CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					} else {

						q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
							CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
							CBM_QUEUE_MAP_F_EN_DONTCARE |
							CBM_QUEUE_MAP_F_DE_DONTCARE |
							CBM_QUEUE_MAP_F_MPE1_DONTCARE |
							CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
					}
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add QMAP for Port %d and TC: %d\n", __func__, q_map.qmap.dp_port, q_map.qmap.class);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Index of New Q: %d\n", __func__, q_new);
					qos_hal_add_q_map(g_Inst, q_new, &q_map, q_map_mask);
					subif_index->user_queue[subif_index->queue_cnt].qmap[i] = *(tc+i);
				}

			}

			/** We need to make sure that the ingress queue mapping is retained
			  after changing the queue map for adding any queue.
			 */
			memset(&q_map, 0, sizeof(struct dp_queue_map_entry));
			q_map_mask = 0;

			q_map.qmap.dp_port = dp_subif->port_id;
			q_map.qmap.flowid = 0;
			q_map.qmap.enc = 1;
			q_map.qmap.dec = 1;
			q_map.qmap.mpe1 = 0;
			q_map.qmap.mpe2 = 0;
			q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Set QMAP for the Ingress Q: %d\n", __func__, res.q_ingress);
			qos_hal_add_q_map(res.q_ingress, g_CPU_PortId, &q_map, q_map_mask);

		}
	}
	kfree(dp_subif);
	kfree(qos_res);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Returned qid:%d -->\n", __func__, qid_last_assigned);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit -->\n", __func__);
	return qid_last_assigned ;
}

static int qos_hal_add_queue(
		struct net_device *netdev,
		char *dev_name,
		char *tc,
		uint8_t no_of_tc,
		uint32_t schedid,
		uint32_t prio_type,
		uint32_t prio_level,
		uint32_t weight,
		uint32_t flowId,
		QOS_Q_ADD_CFG *params,
		uint32_t flags)
{

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);

	if ((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS) {
		return qos_hal_add_ingress_queue(
				netdev,
				dev_name,
				tc,
				no_of_tc,
				schedid,
				prio_type,
				prio_level,
				weight,
				flowId,
				params,
				flags);
	} else { /* Handling for Egress interface */
		return qos_hal_add_egress_queue(
				netdev,
				dev_name,
				tc,
				no_of_tc,
				schedid,
				prio_type,
				prio_level,
				weight,
				flowId,
				params,
				flags);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return QOS_HAL_STATUS_OK;
}


int qos_hal_get_no_of_q_for_prio_lvl(struct qos_hal_user_subif_abstract *subif_index, uint32_t prio, uint32_t *qindex, struct qos_hal_user_subif_abstract *sub_index)
{
	int32_t count = 0, i, q_match = 0;
	struct qos_hal_user_subif_abstract *sub_match = NULL;

	if (subif_index->queue_cnt > QOS_HAL_MAX_QUEUE_PER_EGRESS_INTERFACE)
		return -1;

	for (i = 0; i < subif_index->queue_cnt; i++) {
		if (subif_index->user_queue[i].prio_level == prio) {
			count++;
			q_match = i;
		}
	}
	if (subif_index->is_logical == 0) {
		int t;
		struct qos_hal_user_subif_abstract *temp_index = NULL;

		temp_index = subif_index + 1;
		for (t = 1; t < QOS_HAL_MAX_SUB_IFID_PER_PORT ; t++) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> Sub interface %d User level %d\n", __func__, t, temp_index->port_sched_in_user_lvl);

			if ((temp_index->port_sched_in_egress & 7) == prio) {
				count++;
				sub_match = temp_index;

			}
			temp_index += 1;
		}
	}
	if (count == 1) {
		if (subif_index->is_logical == 0) {
			*qindex = subif_index->user_queue[q_match].queue_index;
			sub_index = sub_match;
		}
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Number of Queue of priority %d is %d\n", __func__, prio, count);
	return count;
}

int qos_hal_get_user_index_from_q_index(struct qos_hal_user_subif_abstract *subif_index, uint32_t qid)
{
	int i = 0;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif Queue count %d\n", __func__, subif_index->queue_cnt);
	for (i = 0; i < subif_index->queue_cnt; i++) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s> qid = %d user req qid =%d\n", __func__, subif_index->user_queue[i].queue_index, qid);
		if (subif_index->user_queue[i].queue_index == qid) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Matched qid =%d\n", __func__, i);
			return i;
		}
	}
	return QOS_HAL_STATUS_ERR;
}

int qos_hal_get_user_index_from_qid(struct qos_hal_user_subif_abstract *subif_index, uint32_t qid)
{
	int i = 0;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif Queue count %d\n", __func__, subif_index->queue_cnt);
	for (i = 0; i < subif_index->queue_cnt; i++) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> qid = %d user req qid =%d\n", __func__, subif_index->user_queue[i].qid, qid);
		if (subif_index->user_queue[i].qid == qid) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Matched qid =%d\n", __func__, i);
			return i;
		}
	}
	return QOS_HAL_STATUS_ERR;
}

/**
  This function is called when there is only 1 queue left for that priority level.
  So the WFQ scheduler which is used to connect all the same priority level queue,
  can be deleted and the only remaining queue can be connected to base scheduler leaf.

  This is function is called from qos_hal_delete_queue().
*/
int qos_hal_reconnect_q_to_high_sched_lvl(
		struct net_device *netdev,
		char *dev_name,
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t q_index,
		uint32_t port,
		uint32_t sched_connect_to,
		uint32_t *reconn_sched_in,
		uint32_t flags)
{
	struct qos_hal_equeue_create q_reconnect;
	uint32_t schedin, reconn_sched, index;
	uint32_t cfg_shaper, user_q_index, leaf;
	struct qos_hal_equeue_cfg q_param = {0};
	struct dp_queue_map_entry *q_map_get = NULL;
	int32_t no_entries = 0;
	struct qos_hal_token_bucket_shaper_cfg cfgShaper = {0};

	schedin = qos_hal_queue_track[q_index].connected_to_sched_id;

	reconn_sched = qos_hal_sched_track[sched_connect_to].omid ;
	leaf = qos_hal_sched_track[sched_connect_to].prio_weight;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Currently Q [%d] --> SB [%d] Omid [%d] leaf [%d]\n", __func__, q_index, sched_connect_to, reconn_sched, leaf);
	if (qos_hal_sched_track[sched_connect_to].level == 0)
		reconn_sched = 0xFF << 3;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is attached to this Queue\n", __func__, qos_hal_queue_track[q_index].tb_index);
	cfg_shaper = qos_hal_queue_track[q_index].tb_index;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue %d is connected to the Scheduler %d\n", __func__, q_index, schedin);

	qos_hal_get_q_map(g_Inst, q_index, &no_entries, &q_map_get, 0);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> %d No of entries is %d for queue=%d\n", __func__, __LINE__, no_entries, q_index);
	/*qos_hal_safe_queue_and_shaper_delete(netdev, dev_name, q_index, -1, cfg_shaper,  -1, flags);*/

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Scheduler %d\n", __func__, sched_connect_to);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Disabling Scheduler input %d\n", __func__, reconn_sched);
	qos_hal_sched_blk_in_enable(reconn_sched, 0);
	qos_hal_scheduler_delete(sched_connect_to);

	index = qos_hal_get_user_index_from_q_index(subif_index, q_index);
	if (index == QOS_HAL_STATUS_ERR) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Invalid Queue Id\n", __func__, __LINE__);
		return QOS_HAL_STATUS_INVALID_QID;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Reconnecting the queue %d to scheduler %d\n", q_index, sched_connect_to >> 3);
	memset(&q_reconnect, 0, sizeof(struct qos_hal_equeue_create));
	q_reconnect.index = q_index;
	q_reconnect.egress_port_number = port;
	q_reconnect.scheduler_input =  reconn_sched;
	q_reconnect.prio =  leaf;
	if ((subif_index->user_queue[qos_hal_queue_track[q_index].user_q_idx].queue_type) == QOS_HAL_POLICY_WSP) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Strict Priority Queue\n");
		q_reconnect.iwgt = 0;
	} else if ((subif_index->user_queue[qos_hal_queue_track[q_index].user_q_idx].queue_type) == QOS_HAL_POLICY_WFQ) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Weighted Fair Queue\n");
		q_reconnect.iwgt = 1000/(subif_index->user_queue[qos_hal_queue_track[q_index].user_q_idx].weight);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Reconnecting the queue %d of weight %d to scheduler input %d\n",
			q_index, q_reconnect.iwgt, q_reconnect.scheduler_input);

	qos_hal_queue_relink(&q_reconnect);

	if (cfg_shaper != 0xFF) {
		cfgShaper.enable = true;
		cfgShaper.mode = qos_hal_shaper_track[cfg_shaper].tb_cfg.mode;
		cfgShaper.cir = qos_hal_shaper_track[cfg_shaper].tb_cfg.cir;
		cfgShaper.pir = qos_hal_shaper_track[cfg_shaper].tb_cfg.pir;
		cfgShaper.cbs = qos_hal_shaper_track[cfg_shaper].tb_cfg.cbs;
		cfgShaper.pbs = qos_hal_shaper_track[cfg_shaper].tb_cfg.pbs;
		cfgShaper.index = cfg_shaper;

		qos_hal_token_bucket_shaper_cfg_set(&cfgShaper);
		/* Add the token to the scheduler input */
		qos_hal_token_bucket_shaper_create(cfg_shaper, q_reconnect.scheduler_input);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is created for scheduler input %d \n",
				__func__, cfg_shaper, q_reconnect.scheduler_input);
	}
	user_q_index = qos_hal_queue_track[q_index].user_q_idx;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Configure Drop params for mode %s \n",
			__func__, (subif_index->user_queue[user_q_index].ecfg.wred_enable == 0) ? "DT" : "WRED");
	q_param.index = q_index;
	q_param.enable = 1;
	q_param.wred_enable = subif_index->user_queue[user_q_index].ecfg.wred_enable;
	q_param.drop_threshold_green_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_min;
	q_param.drop_threshold_green_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_green_max;
	q_param.drop_threshold_yellow_min = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_min;
	q_param.drop_threshold_yellow_max = subif_index->user_queue[user_q_index].ecfg.drop_threshold_yellow_max;
	q_param.drop_probability_green = subif_index->user_queue[user_q_index].ecfg.drop_probability_green;
	q_param.drop_probability_yellow = subif_index->user_queue[user_q_index].ecfg.drop_probability_yellow;
	q_param.avg_weight = subif_index->user_queue[user_q_index].ecfg.weight;
	/*q_param->reservation_threshold;
	q_param->drop_threshold_unassigned;
	q_param->drop_threshold_red; */

	qos_hal_egress_queue_cfg_set(&q_param);

	if (no_entries > 0) {
		int32_t j;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Q map pointer =%p\n", q_map_get);
		for (j = 0; j < no_entries; j++)
			qos_hal_add_q_map(g_Inst, q_index, &q_map_get[j], 0);
		no_entries = 0;
	}
	kfree(q_map_get);
	q_map_get = NULL;

	qos_hal_queue_track[q_index].connected_to_sched_id = reconn_sched >> 3;
	qos_hal_queue_track[q_index].sched_input = reconn_sched;

	subif_index->user_queue[index].sbin = reconn_sched;
	*reconn_sched_in = reconn_sched;

	return QOS_HAL_STATUS_OK;
}

/* Find the connected scheduler id of the default queue. If it is not 0
 * then delete it.
 */
int qos_hal_check_and_clean_extra_resources(
		struct net_device *netdev,
		uint32_t qosport,
		struct qos_hal_user_subif_abstract *subif_index,
		uint32_t flags)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t input_sched;
	struct qos_hal_equeue_create q_reconnect;

	input_sched = qos_hal_queue_track[subif_index->default_q].connected_to_sched_id;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%d>input_sched:[%d]\n", __LINE__, input_sched);
	/* If there is a scheduler still present then delete the sub interface
	 * base scheduler and reconnect the default queue to the port.
	 */
	if ((input_sched != 0) && (input_sched < 0xFF)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%d>input_sched:[%d]\n", __LINE__, input_sched);
		memset(&q_reconnect, 0, sizeof(struct qos_hal_equeue_create));
		q_reconnect.index = subif_index->default_q;
		q_reconnect.egress_port_number = qosport;
		q_reconnect.scheduler_input =  0xFF << 3;
		q_reconnect.prio =  0;
		qos_hal_queue_relink(&q_reconnect);
	} else
		input_sched = qos_hal_port_track[qosport].input_sched_id;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "input_sched:[%d]\n", input_sched);
	if (input_sched <= 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Clean up not needed for scheduler [%d]\n", input_sched);
		return QOS_HAL_STATUS_OK;
	}

	ret = qos_hal_scheduler_delete(input_sched);
	if (ret == QOS_HAL_STATUS_ERR) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Failed to delete the scheduler [%d]\n", input_sched);
		return QOS_HAL_STATUS_ERR;
	}
	return QOS_HAL_STATUS_OK;
}

int qos_hal_delete_queue(
		struct net_device *netdev,
		char *dev_name,
		uint32_t q_id,
		uint32_t priority,
		uint32_t scheduler_id,
		QOS_Q_DEL_CFG *param,
		uint32_t flags)
{
	uint32_t i = 0, connected_sched, index;
	uint32_t no_of_qos_ports;
	uint32_t sched_algo = 0;
	int32_t prio;
	int32_t ret = QOS_HAL_STATUS_OK;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};
	struct dp_qos_q_logic q_logic = {0};
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	int32_t qosport, qos_q_sys;
	int32_t recnct = 255;
	int32_t qIndex;
	char *devname = NULL;

	struct dp_queue_map_entry *q_map_get = NULL;
	int32_t num_entries = 0;
	uint32_t sched_out  = 0;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> For netdev %s \n", __func__, netdev->name);

	MEM_ALLOC(dp_subif_t, dp_subif);
	devname = dev_name;
	dp_subif->port_id = param->portid;
#if IS_ENABLED(CONFIG_X86_INTEL_LGM) || IS_ENABLED(CONFIG_PRX300_HGU)
	if (dp_subif->port_id <= 0) {
		dp_subif->port_id = -1;
	}
#endif

	dq_res.inst = 0;
	dq_res.dev = netdev;

	if (dp_subif->port_id != -1) {
		dq_res.dev = NULL;
	} else {
		if (qos_hal_get_q_res_from_netdevice(
				netdev,
				netdev->name,
				param->intfId,
				dp_subif,
				sched_algo,
				&no_of_qos_ports,
				(void **)&qos_res,
				flags) != QOS_HAL_STATUS_OK) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>:<%d> Fail to get resources\n",
						__func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <- Port Id:[%d] CQM DEQ Idx: [%d] ->\n",
			__func__, dp_subif->port_id, param->intfId);
	dq_res.dp_port = dp_subif->port_id;
	dq_res.cqm_deq_idx = param->intfId;
	if (dp_deq_port_res_get(&dq_res, flags) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to Get QOS Resources\n",
				__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
	if (dq_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);

	dq_res.q_res_size = dq_res.num_q;
	if (dp_deq_port_res_get(&dq_res, flags) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to Get QOS Resources\n",
				__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Qid: %d\n", __func__, q_id);

	no_of_qos_ports = dq_res.num_deq_ports;
	qosport = qos_res->cqm_deq_port;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> CQM DEQ Port: %d\n", __func__, qosport);
	/* For Directpath interface */
	if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) {
		struct qos_hal_dp_res_info res = {0};
		qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

		subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
			dp_subif->subif;
		port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);
	}

	if (flags & QOS_MGR_Q_F_INGRESS) {
		qos_q_sys = g_CPU_Queue;
		qosport = g_CPU_PortId;
	} else
		qos_q_sys = dp_subif->def_qid;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> System Default Queue:[%d]\n", __func__, qos_q_sys);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> priority=%d high_prio_q_limit=%d\n", __func__, priority, high_prio_q_limit);
	if (!(flags & QOS_MGR_Q_F_INGRESS)) {
		if (priority > high_prio_q_limit) {
			int32_t base_sched;
			if (no_of_qos_ports > 1) {
				base_sched = (qos_res)->sch_id[0];
				qosport = (qos_res)->cqm_deq_port;
				qos_q_sys = (qos_res)->q_id;
				subif_index = qos_hal_user_sub_interface_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
				port_subif_index = qos_hal_user_sub_interface_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Low Priority Port:%d SB: %d \n", __func__, qosport, base_sched);
			}
		} else {
			int32_t base_sched;
			if (no_of_qos_ports > 1) {
				base_sched = (qos_res+1)->sch_id[0];
				qosport = (qos_res+1)->cqm_deq_port;
				qos_q_sys = (qos_res+1)->q_id;
				subif_index = qos_hal_user_sub_interface_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
				port_subif_index = qos_hal_user_sub_interface_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> High Priority Port:%d SB: %d \n", __func__, qosport, base_sched);
			}
		}
	}

	if (flags & QOS_MGR_Q_F_INGRESS) {
		qosport = qos_hal_get_ingress_index(flags);
		if (qosport == QOS_HAL_STATUS_ERR) {
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCES_CLEANUP;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Group index is %d\n ",
				__func__, qosport);

		port_subif_index = qos_hal_user_sub_interface_ingress_info + qosport;
	} else
		port_subif_index = qos_hal_user_sub_interface_info + qosport;

	qos_hal_get_subif_list_item(
			qosport,
			netdev,
			&subif_index,
			&port_subif_index->head);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif:[%p]\n",
				__func__, subif_index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port subif:[%p]\n",
				__func__, port_subif_index);
	index = qos_hal_get_user_index_from_qid(subif_index, q_id);
	if (index == QOS_HAL_STATUS_ERR) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Invalid Queue Id\n", __func__, __LINE__);
		ret = QOS_HAL_STATUS_INVALID_QID;
		goto QOS_RESOURCES_CLEANUP;
	}
	qIndex = subif_index->user_queue[index].queue_index;
	q_logic.q_id = qos_hal_queue_track[qIndex].queue_id;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete User Queue Index =%d \n", __func__, index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Actual Queue Index [%d]\n", __func__, qIndex);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Physical Queue Id is [%d]\n",  q_logic.q_id);

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
	/** Get the logical queue id, this will be required for
	  * flushing the system using pSess_del_cb callback
	 */
	if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to Logical Queue Id\n",
				__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
#endif
	prio = subif_index->user_queue[index].prio_level;

	/** If the delete is for Egress queue and for the physical interface
	  --> then don't allow to delete the reserved queue
	 */
	if ((!(flags & QOS_MGR_Q_F_INGRESS)) && subif_index->is_logical != 1) {
		if (subif_index->user_queue[index].queue_index < g_No_Of_QOS_Res_Queue) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Reserved Queue %d can't be deleted \n", __func__, subif_index->user_queue[index].queue_index);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCES_CLEANUP;
		}
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> User Queue sbin %d  Actual Queue SBIN:%d\n",
			__func__, subif_index->user_queue[index].sbin, qos_hal_queue_track[subif_index->user_queue[index].queue_index].sched_input);

	subif_index->user_queue[index].sbin = qos_hal_queue_track[subif_index->user_queue[index].queue_index].sched_input;

	if (subif_index->default_q != 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Deafult Queue %d \n", __func__, subif_index->default_q);
		recnct = subif_index->default_q;
	} else  {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>QoS Deafult Queue %d \n", __func__, qos_q_sys);
		recnct = qos_q_sys;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Lookup mode %d \n", __func__, subif_index->lookup_mode);
	if ((subif_index->lookup_mode == 1) || (flags & NETDEV_TYPE_TCONT)) {
		pr_debug("MUST BE FOR TCONT. NO NEED TO REMAP\n");
	} else {
		qos_hal_get_q_map(
			g_Inst,
			subif_index->user_queue[index].queue_index,
			&num_entries, &q_map_get, 0);

		if (prio == subif_index->default_prio) {
			int32_t qid;

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Prio Level/No Of QOS ports --> [%d/%d/] \n", prio, no_of_qos_ports);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Default Queue [%d] has mapped entries [%d]\n",
					__func__, subif_index->user_queue[index].queue_index, num_entries);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap to System Default Q: [%d]\n", __func__, qos_q_sys);
			qid = qos_hal_get_queue_index_from_qid(qos_q_sys);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap to System Default Q: [%d] qindex: [%d]\n", __func__, qos_q_sys, qid);
			if (num_entries > 0) {
				for (i = 0; i < num_entries; i++) {
					qos_hal_add_q_map(qid, g_CPU_PortId, &q_map_get[i], 0);
				}
			}

			subif_index->default_q = 0;
		} else {
			struct dp_queue_map_entry q_map;
			uint32_t q_map_mask = 0;
			for (i = 0; i < subif_index->user_queue[index].no_of_tc; i++) {
				/** Configure the QMAP table to connect to the DP egress queue*/
				memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

				q_map.qmap.dp_port = dp_subif->port_id;
				q_map.qmap.class = subif_index->user_queue[index].qmap[i];
				if (subif_index->lookup_mode == 2) {
					q_map.qmap.subif = subif_index->dp_subif.subif << 8;
					q_map.qmap.mpe1 = 0;
					q_map.qmap.mpe2 = 1;
					q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
						CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
						CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE ;
				}
				else {
					q_map.qmap.subif = subif_index->user_queue[index].qmap[i];
					q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
						CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
						CBM_QUEUE_MAP_F_EN_DONTCARE |
						CBM_QUEUE_MAP_F_DE_DONTCARE |
						CBM_QUEUE_MAP_F_MPE1_DONTCARE |
						CBM_QUEUE_MAP_F_MPE2_DONTCARE ;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete QMAP for Port %d and TC: [%d]\n", __func__, q_map.qmap.dp_port, q_map.qmap.class);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap to Q: [%d]\n", __func__, qos_q_sys);
				if (subif_index->default_q != 0) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap to default Q: [%d]\n", __func__, subif_index->default_q);
					qos_hal_add_q_map(subif_index->default_q, g_CPU_PortId, &q_map, q_map_mask);
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap to Q: [%d]\n", __func__, qos_q_sys);
					qos_hal_add_q_map(qos_q_sys, g_CPU_PortId, &q_map, q_map_mask);
				}
			} /* for loop no_of_tc */
		}
	}

	if (num_entries > 0) {
		num_entries = 0;
	}
	kfree(q_map_get);
	q_map_get = NULL;

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
	/** Call the PPA cb to flush all ppv4 hw sessions 
	*/
	if (pSess_del_cb)
		pSess_del_cb(q_logic.q_logic_id);
#endif
	ret = qos_hal_safe_queue_and_shaper_delete(
			netdev,
			devname,
			subif_index->user_queue[index].queue_index,
			-1,
			qos_hal_queue_track[subif_index->user_queue[index].queue_index].tb_index,
			-1,
			(flags | QOS_HAL_DEL_SHAPER_CFG));

	if (ret == QOS_HAL_STATUS_ERR)
		goto QOS_RESOURCES_CLEANUP;

	connected_sched = subif_index->user_queue[index].sbin >> 3;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue sbin %d sb:%d\n",
			__func__, subif_index->user_queue[index].sbin, connected_sched);

	ret = qos_hal_shift_up_sbin_conn(
			netdev,
			devname,
			qosport,
			subif_index,
			subif_index->user_queue[index].sbin,
			flags);
	if (ret == QOS_HAL_STATUS_ERR)
		goto QOS_RESOURCES_CLEANUP;
	subif_index->queue_cnt--;

	/** For all the logical interface if the last queue is getting deleted, then
	  1> Delete then scheduler
	  2> Disable the scheduler input of the connected scheduler
	  3> If the connected scheduler is Port scheduler then shift up the all the connected index of port scheduler to that index
	  4> Update the subif_index
	 */
	if (subif_index->is_logical == 1 && subif_index->queue_cnt == 0) {
		uint32_t omid, port_omid;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> No more queue for the sub interface\n", __func__);

		/* Delete the sub interface base scheduler */
		omid = qos_hal_sched_track[subif_index->base_sched_id_egress].omid;
		sched_out = qos_hal_sched_track[subif_index->base_sched_id_egress].sched_out_conn;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Scheduler subif_index: base_sched_id_egress: %d port_sched_in_egress: %d OMID: %d\n",
				__func__, subif_index->base_sched_id_egress, subif_index->port_sched_in_egress, omid);
		qos_hal_scheduler_delete(subif_index->base_sched_id_egress);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Omid leaf_mask:[%d] Parent of Sched:[%s]\n",
					__func__, qos_hal_sched_track[omid >> 3].leaf_mask,
					(sched_out == 1)? "Scheduler" : "Port");

		if ((omid == subif_index->port_sched_in_egress)) {
			if ((sched_out == 1) && (qos_hal_sched_track[omid >> 3].leaf_mask == 0)) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s:%d> Nothing connected to any leaf. Delete scheduler %d\n",
							__func__, __LINE__, (omid >> 3));
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Omid -> Omid: [%d]\n",
							__func__, qos_hal_sched_track[omid >> 3].omid);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Omid -> sched_out_conn: [%d]\n", 
							__func__, qos_hal_sched_track[omid >> 3].sched_out_conn);
				qos_hal_scheduler_delete(omid >> 3);

				subif_index->base_sched_id_egress = INVALID_SCHED_ID;
				subif_index->port_sched_in_egress = INVALID_SCHEDULER_INPUT_ID;
				port_subif_index->qos_port_idx = 0xFF;
				qos_hal_port_track[qosport].is_enabled  = 0;
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> One more VAP remaining, shift up of scheduler.\n", __func__);
				qos_hal_shift_up_sbin_conn(
					netdev,
					devname,
					qosport,
					port_subif_index,
					subif_index->port_sched_in_egress,
					flags);
			}
		} else if ((omid >> 3) == subif_index->port_shaper_sched) {
			QOS_RATE_SHAPING_CFG cfg;
			memset(&cfg, 0, sizeof(QOS_RATE_SHAPING_CFG));
			cfg.dev_name = devname;
			cfg.flag = flags;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Need to Delete Port rate shaper %d\n", __func__, (omid >> 3));
			/** Base scheduler is already deleted. So update the next_sched_id of the
			  port rate shaper scheduler even before deleting it.
			 */
			port_omid = qos_hal_sched_track[omid >> 3].omid;
			qos_hal_sched_track[omid >> 3].next_sched_id = INVALID_SCHED_ID;
			if (qos_hal_del_port_rate_shaper(netdev, &cfg, flags) == QOS_HAL_STATUS_OK) {
				qos_hal_sched_blk_in_enable(port_omid, 0);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shift the Port egress input up %d\n", __func__, subif_index->port_sched_in_egress);
				qos_hal_shift_up_sbin_conn(netdev, devname, qosport, port_subif_index, subif_index->port_sched_in_egress, flags);
			}
		} else {
			if (qos_hal_sched_track[omid >> 3].leaf_mask == 0) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s:%d> Nothing connected to any leaf. Delete scheduler %d\n", __func__, __LINE__, (omid >> 3));
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Omid: %d\n", __func__, qos_hal_sched_track[omid >> 3].omid);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> sched_out_conn: %d\n", __func__, qos_hal_sched_track[omid >> 3].sched_out_conn);
				qos_hal_scheduler_delete(omid >> 3);
				qos_hal_shift_up_sbin_conn(netdev, devname, qosport, port_subif_index, subif_index->port_sched_in_egress, flags);

				port_subif_index->qos_port_idx = 0xFF;
				qos_hal_port_track[qosport].is_enabled  = 0;
			}
		}
		subif_index->qos_port_idx = 0xFF;
		subif_index->base_sched_id_egress = INVALID_SCHED_ID;
		subif_index->port_sched_in_egress = INVALID_SCHEDULER_INPUT_ID;
		qos_hal_remove_subif_list_item(netdev, &port_subif_index->head);
		ret = QOS_HAL_STATUS_OK;
		goto QOS_RESOURCES_CLEANUP;
	} else if(flags & QOS_MGR_Q_F_INGRESS && (subif_index->queue_cnt == 0)) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "Default Q [%d:%d] Scheduler %d Port %d\n",
				subif_index->default_q,
				qos_hal_queue_track[subif_index->default_q].queue_id,
				qos_hal_queue_track[subif_index->default_q].connected_to_sched_id,
				qosport);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Queue sbin %d sb:%d\n",
				__func__,
				subif_index->user_queue[index].sbin,
				connected_sched);
		if (connected_sched < QOS_HAL_MAX_SCHEDULER) {
			ret = qos_hal_scheduler_delete(
					connected_sched);

			if (ret == QOS_HAL_STATUS_ERR) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "Failed to cleanup resources\n");
				goto QOS_RESOURCES_CLEANUP;
			}

			subif_index->base_sched_id_egress = INVALID_SCHED_ID;
			connected_sched = INVALID_SCHED_ID;

			subif_index->qos_port_idx = 0xFF;
			qos_hal_port_track[qosport].is_enabled  = 0;
			g_IngressQueueScheduler = 0;

			qos_hal_reset_queue_track(subif_index->default_q);
			qos_hal_remove_subif_list_item(netdev, &port_subif_index->head);
		}
	}

	/** Now after deleting the queue, we have to rearrange the user queue info buffer
	  1> Compaction --> move up all the queue index information till the deleted index.
	  2> If all the queues of that scheduler are deleted, then delete the scheduler and
	  adjust the priority level.
	 */
	if (subif_index->queue_cnt != 0) {
		uint32_t temp;
		uint32_t qindex = 0xFF;
		struct qos_hal_user_subif_abstract *sub_index = NULL;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Index %d Queue Count is %d\n", index, subif_index->queue_cnt);
		for (i = index; i < subif_index->queue_cnt; i++) {
			temp = subif_index->user_queue[index].sbin;
			memcpy(&subif_index->user_queue[i], &subif_index->user_queue[i+1], sizeof(struct qos_hal_user_queue_info));
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "i=%d temp sbin %d user sbin %d\n", i, temp, subif_index->user_queue[i].sbin);
			qos_hal_queue_track[subif_index->user_queue[i+1].queue_index].user_q_idx = i;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Q %d -> User Index %d\n",
					subif_index->user_queue[i+1].queue_index,
					qos_hal_queue_track[subif_index->user_queue[i+1].queue_index].user_q_idx);
		}
		subif_index->user_queue[i].queue_index = 0xFF;
		subif_index->user_queue[i].sbin = INVALID_SCHEDULER_INPUT_ID;
		subif_index->user_queue[i].prio_level = QOS_HAL_MAX_PRIORITY_LEVEL;
		subif_index->user_queue[i].qid = 0;

		if (connected_sched < QOS_HAL_MAX_SCHEDULER) {
			if (qos_hal_sched_track[connected_sched].leaf_mask == 0) {
				uint32_t next_sched, peer_sched = 0;
				uint32_t omid_tmp, con_type;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Base Scheduler is [%d]\n", __func__, subif_index->base_sched_id_egress);
				next_sched = qos_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id;
				if (next_sched < QOS_HAL_MAX_SCHEDULER)
					peer_sched = qos_hal_sched_track[next_sched].peer_sched_id;

				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Scheduler %d has no queue connected\n", __func__, connected_sched);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Priority Selector %d\n", qos_hal_sched_track[connected_sched].priority_selector);
				if (qos_hal_sched_track[connected_sched].priority_selector == 3) {
					if (qos_hal_sched_track[qos_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id].peer_sched_id == connected_sched) {
						qos_hal_del_scheduler_level(netdev, qosport, subif_index->base_sched_id_egress, connected_sched, devname, flags);
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Base Scheduler %d \n", __func__, next_sched);
						subif_index->base_sched_id_egress = next_sched;
					} else {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Something is wrong\n");
					}
				} else if (qos_hal_sched_track[connected_sched].priority_selector == 2) {
					if (qos_hal_sched_track[subif_index->base_sched_id_egress].next_sched_id == connected_sched) {
						qos_hal_del_scheduler_level(netdev, qosport, subif_index->base_sched_id_egress, connected_sched, devname, flags);
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> New Base Scheduler %d \n", __func__, peer_sched);
						subif_index->base_sched_id_egress = peer_sched;
					} else {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Something is wrong\n");
					}
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete scheduler %d of same level\n",
							__func__, connected_sched);
					omid_tmp = qos_hal_sched_track[connected_sched].omid;
					con_type = qos_hal_sched_track[connected_sched].sched_out_conn;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "omid of scheduler [%d:%d] con_type :%d\n",
							omid_tmp, (omid_tmp >> 3), con_type);
					qos_hal_scheduler_delete(connected_sched);
					/* if the omid is schedular then shift up leaf of omid
					 */
					if (con_type == 1) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "omid leaf mask %d\n",
								qos_hal_sched_track[omid_tmp >> 3].leaf_mask);
						if (qos_hal_sched_track[omid_tmp >> 3].leaf_mask != 0x0) {
							ret = qos_hal_shift_up_sbin_conn(
								netdev,
								devname,
								qosport,
								subif_index,
								omid_tmp,
								flags);
							if (ret == QOS_HAL_STATUS_ERR)
								goto QOS_RESOURCES_CLEANUP;
						}
					}
				}
			}
		}

		/* For the physical interface when all queues are deleted queue_cnt is 1.
		 * Before any user queue configuration started if already a scheduler is
		 * is configured between a port and queue, default_sched_id of the port
		 * tracking is already set to that scheduler id. If that is 0xFF then
		 * call the API qos_hal_check_and_clean_extra_resources() to cleanup
		 * the extra scheduler resource.
		 */
		if (((!(flags & QOS_MGR_Q_F_INGRESS)) &&
			(subif_index->is_logical == 0) &&
			(subif_index->queue_cnt == 1))
				||
			((flags & QOS_MGR_Q_F_INGRESS) &&
			(subif_index->queue_cnt == 0))
			) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Default Q [%d:%d] Scheduler %d Port %d\n",
					subif_index->default_q,
					qos_hal_queue_track[subif_index->default_q].queue_id,
					qos_hal_queue_track[subif_index->default_q].connected_to_sched_id,
					qosport);
			if (qos_hal_port_track[qosport].default_sched_id == 0xFF) {
				ret = qos_hal_check_and_clean_extra_resources(
						netdev,
						qosport,
						subif_index,
						0);

				if (ret == QOS_HAL_STATUS_ERR) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Failed to cleanup resources\n");
					goto QOS_RESOURCES_CLEANUP;
				}

				subif_index->base_sched_id_egress = INVALID_SCHED_ID;
				connected_sched = INVALID_SCHED_ID;

				subif_index->qos_port_idx = 0xFF;
				qos_hal_port_track[qosport].is_enabled  = 0;

				qos_hal_reset_queue_track(subif_index->default_q);
				qos_hal_remove_subif_list_item(netdev, &port_subif_index->head);
			}
		}

		if (qos_hal_get_no_of_q_for_prio_lvl(subif_index, prio, &qindex, sub_index) == 1) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Only 1 queue is connected this priority level of scheduler/Policy [%d:%d]\n",
					connected_sched, qos_hal_sched_track[connected_sched].policy);
			if (connected_sched < QOS_HAL_MAX_SCHEDULER) {
				if (qos_hal_sched_track[connected_sched].policy == QOS_HAL_POLICY_WFQ) {
					uint32_t new_sched_in;
					index = 0 ;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, " Connect the Queue %d to the higher scheduler level \n", qindex);
					qos_hal_reconnect_q_to_high_sched_lvl(netdev, devname, subif_index,
								qindex, qosport, connected_sched, &new_sched_in, flags);
				}
			}
		}
		qos_hal_dump_subif_queue_info(subif_index);
	}

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return ret;
}

int qos_hal_modify_queue(struct net_device *netdev, QOS_Q_MOD_CFG *param)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t index;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_equeue_cfg q_param;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	QOS_MGR_VCC *vcc = NULL;

	if (netdev) {
		MEM_ALLOC(dp_subif_t, dp_subif);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify QueueId %d\n", __func__, param->q_id);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Priority %d Weight %d \n", __func__, param->priority, param->weight);
		qos_mgr_br2684_get_vcc(netdev, &vcc);
		if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get Subif Id\n", __func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}

		dp_subif->subif  = dp_subif->subif >> 8;

		dq_res.dev = netdev;
		dq_res.dp_port = dp_subif->port_id;
		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		if (dq_res.num_q == 0){
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
		if (dq_res.q_res == NULL) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			kfree(dp_subif);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCES_CLEANUP;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
				__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

		subif_index = qos_hal_user_sub_interface_info + (qos_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, " Sub interface index %p\n", subif_index);

		if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) {
			struct qos_hal_dp_res_info res = {0};
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Adding the shaper for Directpath interface!!!\n", __func__);
			qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

			subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
				dp_subif->subif;
			port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);

		}
		kfree(dp_subif);

		index = qos_hal_get_user_index_from_qid(subif_index, param->q_id);
		if (index == QOS_HAL_STATUS_ERR) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Invalid Queue Id\n", __func__, __LINE__);
			ret = QOS_HAL_STATUS_INVALID_QID;
			goto QOS_RESOURCES_CLEANUP;
		}

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify User Queue Index =%d \n", __func__, index);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify Actual Queue Index %d\n", __func__, subif_index->user_queue[index].queue_index);

		if (subif_index->user_queue[index].prio_level != param->priority) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify Priority level to %d\n", __func__, param->priority);
			if (qos_hal_delete_queue(netdev, netdev->name, param->q_id, subif_index->user_queue[index].prio_level, 0, NULL, param->flags) == QOS_HAL_STATUS_OK) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Add a new Queue \n", __func__);
				/*param->q_id = qos_hal_add_queue(netdev, netdev->name, 0, 1, 1, param->priority, param->weight, param->priority, 0, param->flags);*/
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> After add queue : Q_ID = %d\n", __func__, param->q_id);
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Queue Deletion Failed\n", __func__, __LINE__);
				ret = QOS_HAL_STATUS_ERR;
				goto QOS_RESOURCES_CLEANUP;
			}
		}

	} else if (param->q_id) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Queue Modify request for Special Connection. \n", __func__);
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Invalid Queue Modify Request. \n", __func__);
		return QOS_HAL_STATUS_ERR;
	}
	memset(&q_param, 0, sizeof(struct qos_hal_equeue_cfg));

	if (param->q_id)
		q_param.spl_conn = 1;

	if (param->drop.mode == QOS_MGR_DROP_WRED) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Modify WRED params \n", __func__);
		q_param.drop_threshold_green_min = param->drop.wred.min_th0;
		q_param.drop_threshold_green_max = param->drop.wred.max_th0;
		q_param.drop_threshold_yellow_min = param->drop.wred.min_th1;
		q_param.drop_threshold_yellow_max = param->drop.wred.min_th1;
		q_param.drop_probability_green = param->drop.wred.max_p0;
		q_param.drop_probability_yellow = param->drop.wred.max_p1;
		q_param.wred_enable = 1;
		q_param.enable = 1;
		q_param.avg_weight = param->drop.wred.weight;
	} else if (param->drop.mode == QOS_MGR_DROP_TAIL) {
		q_param.enable = 1;
		q_param.wred_enable = 0;
		q_param.index = param->q_id; 
		q_param.q_len = param->qlen; 
	}

	qos_hal_egress_queue_cfg_set(&q_param);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	return ret;
}

int qos_hal_get_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t index, q_index;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};
	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	QOS_MGR_VCC *vcc = NULL;
	int32_t qosport = 0;


	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Add Shaper for the Queue ID %d\n", cfg->queueid);

	MEM_ALLOC(dp_subif_t, dp_subif);
	qos_mgr_br2684_get_vcc(netdev, &vcc);
	if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __func__, __LINE__);
		if (dp_get_port_subitf_via_ifname(cfg->dev_name, dp_subif) != QOS_MGR_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __func__, __LINE__);
			if (qos_hal_get_subitf_via_ifname(cfg->dev_name, dp_subif) != QOS_MGR_SUCCESS) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from qos_hal_get_subitf_via_ifname \n", __func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
		}
	}

#ifdef QOS_HAL_TEST
	dp_subif->ort_id = 15;
	dp_subif->subif = scheduler_id;

#endif

	dp_subif->subif  = dp_subif->subif >> 8;

	dq_res.inst = 0;
	dq_res.dev = netdev;
	dq_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
	if (dq_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);

	dq_res.q_res_size = dq_res.num_q;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

	if (flags & QOS_MGR_Q_F_INGRESS) {
		qosport = qos_hal_get_ingress_index(flags);
		if (qosport == QOS_HAL_STATUS_ERR) {
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCES_CLEANUP;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> qosport:[%d]\n", __func__, qosport);

		subif_index = qos_hal_user_sub_interface_ingress_info + qosport;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif Queue count %d\n", __func__, subif_index->queue_cnt);
		port_subif_index = qos_hal_user_sub_interface_ingress_info + qosport;
	} else {
		port_subif_index = qos_hal_user_sub_interface_info + qos_res->cqm_deq_port;
		qos_hal_get_subif_list_item(qos_res->cqm_deq_port, netdev, &subif_index, &port_subif_index->head);
	}

	index = qos_hal_get_user_index_from_qid(subif_index, cfg->queueid);
	if (index == QOS_HAL_STATUS_ERR) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Invalid Queue Id\n", __func__, __LINE__);
		ret = QOS_HAL_STATUS_INVALID_QID;
		goto QOS_RESOURCES_CLEANUP;
	}

	q_index = subif_index->user_queue[index].queue_index;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Actual Queue Index %d\n", __func__, subif_index->user_queue[index].queue_index);

	if (qos_hal_queue_track[q_index].sched_input == 1023) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Wrong Queue %d\n", __func__, __LINE__, q_index);
		ret = QOS_HAL_STATUS_ERR;
		goto QOS_RESOURCES_CLEANUP;
	} else {
		if (cfg->shaperid <= 0) {

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d No shaper instanec is created for Queue index %d\n", __func__, __LINE__, q_index);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCES_CLEANUP;
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper %d is already created \n", cfg->shaperid);
			cfg->shaper.phys_shaperid = qos_hal_shaper_track[cfg->shaperid].tb_cfg.index;
			cfg->shaper.enable = qos_hal_shaper_track[cfg->shaperid].tb_cfg.enable;
			cfg->shaper.mode = qos_hal_shaper_track[cfg->shaperid].tb_cfg.mode;
			cfg->shaper.cir = qos_hal_shaper_track[cfg->shaperid].tb_cfg.cir;
			cfg->shaper.pir = qos_hal_shaper_track[cfg->shaperid].tb_cfg.pir;
			cfg->shaper.cbs = qos_hal_shaper_track[cfg->shaperid].tb_cfg.cbs;
			cfg->shaper.pbs = qos_hal_shaper_track[cfg->shaperid].tb_cfg.pbs;
		}
	}

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);

	return ret;
}

/** Fill shaper cfg from shaper track and Create a new shaper instance or update it.
    ** 1> Fill shaper cfg structure from shaper tarck corresponding to input shaperid
    ** 2> Get New free shaper and Update the shaper tracking information.
    ** 3> Return the shaper index.
 */
int qos_hal_set_cfg_shaper_from_shaper_track(QOS_RATE_SHAPING_CFG *cfg)
{

	int32_t shaper_index;
	cfg->shaper.mode = qos_hal_shaper_track[cfg->shaperid].tb_cfg.mode;
	cfg->shaper.pir = qos_hal_shaper_track[cfg->shaperid].tb_cfg.pir;
	cfg->shaper.cir = qos_hal_shaper_track[cfg->shaperid].tb_cfg.cir;
	cfg->shaper.pbs = qos_hal_shaper_track[cfg->shaperid].tb_cfg.pbs;
	cfg->shaper.cbs = qos_hal_shaper_track[cfg->shaperid].tb_cfg.cbs;

	shaper_index = qos_hal_get_free_shaper();
	QOS_HAL_ASSERT(shaper_index >= QOS_HAL_MAX_SHAPER);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>New Shaper  %d\n", __func__, shaper_index);

	qos_hal_shaper_track[shaper_index].is_enabled = true;
	qos_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
	qos_hal_shaper_track[shaper_index].tb_cfg.mode = cfg->shaper.mode;
	qos_hal_shaper_track[shaper_index].tb_cfg.pir = cfg->shaper.pir;
	qos_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->shaper.cir;
	qos_hal_shaper_track[shaper_index].tb_cfg.pbs = cfg->shaper.pbs;
	qos_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->shaper.cbs;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return shaper_index;
}

/** Create a new shaper instance or update it.
    ** 1> if shaper id <=0, then get free shaper instance
    ** 2> Update the shaper tracking information for the new or given shaper index.
    ** 3> Return the shaper index.
 */
int qos_hal_add_shaper_index(QOS_RATE_SHAPING_CFG *cfg)
{
	int shaper_index;
	if (cfg->shaperid <= 0) {
		shaper_index = qos_hal_get_free_shaper();
	} else {
		shaper_index = cfg->shaperid;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Modify Shaper %d\n", __func__, shaper_index);
	}
	QOS_HAL_ASSERT(shaper_index >= QOS_HAL_MAX_SHAPER);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>New Shaper  %d\n", __func__, shaper_index);
	qos_hal_shaper_track[shaper_index].is_enabled = true;
	qos_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
	qos_hal_shaper_track[shaper_index].tb_cfg.mode = cfg->shaper.mode;
	qos_hal_shaper_track[shaper_index].tb_cfg.pir = cfg->shaper.pir;
	qos_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->shaper.cir;
	qos_hal_shaper_track[shaper_index].tb_cfg.pbs = cfg->shaper.pbs;
	qos_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->shaper.cbs;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return shaper_index;
}


/** Add the shaper for the queue.
    ** 1> Check if the tmu queue index is provided. if not then get it from the user queue index.
    ** 2> Check if the shaper instance is provided.
    ** 3> If not then --> Get free shaper index.
    ** 4> Create the shaper and configure it.
    ** 5> Update the shaper tracking information.
    ** 6> Configure the shaper for the queue scheduler input.
*/
int qos_hal_add_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, int32_t qos_q_idx, uint32_t flags)
{
	int32_t i = 0, shaper_index = 0;
	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t index = 0, qosport = 0;
	uint32_t q_index = 0;
	uint32_t no_of_qos_ports;
	uint32_t fail_cnt = 0;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};

	dp_subif_t *dp_subif = NULL;
	struct qos_hal_token_bucket_shaper_cfg cfg_shaper;
	struct qos_hal_user_subif_abstract *base_subif_index = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	QOS_MGR_VCC *vcc = NULL;
	char *dev_name = NULL;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Add Shaper for the Queue ID %d of Port Id:%d\n", cfg->queueid, cfg->portid);

	MEM_ALLOC(dp_subif_t, dp_subif);
	dev_name = cfg->dev_name;

	if (qos_q_idx == -1) {
		if (!(flags & QOS_MGR_Q_F_INGRESS)) {
			if (netdev) {
				if (netdev != NULL)
					qos_mgr_br2684_get_vcc(netdev, &vcc);
				if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __func__, __LINE__);
					if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __func__, __LINE__);
						if (qos_hal_get_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
							QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from qos_hal_get_subitf_via_ifname \n", __func__, __LINE__);
							kfree(dp_subif);
							return QOS_HAL_STATUS_ERR;
						}
					}
				}
			} else if (!netdev && (cfg->portid > 0)) {
				dp_subif->port_id = cfg->portid;
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR netdev and port id both are invalid \n", __func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
		}

#ifdef QOS_HAL_TEST
		dp_subif->port_id = 15;
		dp_subif->subif = scheduler_id;
#endif
		dp_subif->subif = dp_subif->subif >> 8;

		dq_res.inst = 0;
		dq_res.dev = netdev;
		dq_res.dp_port = dp_subif->port_id;

		if (flags & QOS_MGR_Q_F_INGRESS) {
			dq_res.dev = NULL;
			dq_res.dp_port = 0;
		}

		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		if (dq_res.num_q == 0) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
		if (dq_res.q_res == NULL) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);

		dq_res.q_res_size = dq_res.num_q;
		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			ret = QOS_HAL_STATUS_ERR;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			goto QOS_RESOURCES_CLEANUP;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
				__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

		no_of_qos_ports = dq_res.num_deq_ports;
		qosport = qos_res->cqm_deq_port;
		if (flags & QOS_MGR_Q_F_INGRESS) {
			 {/* for CPU Ingress queue */
				qosport = qos_hal_get_ingress_index(flags);
				if (qosport == QOS_HAL_STATUS_ERR) {
					ret = QOS_HAL_STATUS_ERR;
					goto QOS_RESOURCES_CLEANUP;
				}

			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> qosport:[%d]\n", __func__, qosport);

			base_subif_index = qos_hal_user_sub_interface_ingress_info + qosport;
			port_subif_index = qos_hal_user_sub_interface_ingress_info + qosport;

			qos_hal_get_subif_list_item(qosport,
					NULL,
					&subif_index,
					&base_subif_index->head);

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Ingres  Sub interface index %p\n", __func__, subif_index);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif Queue count %d\n", __func__, subif_index->queue_cnt);
		} else {
			port_subif_index = qos_hal_user_sub_interface_info + qos_res->cqm_deq_port;
			qos_hal_get_subif_list_item(qosport, netdev, &subif_index, &port_subif_index->head);
		}

		/* Find the user qid from the user queue array */
		for (i = 0; i < no_of_qos_ports; i++) {
			if ((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS) {
				index = qos_hal_get_user_index_from_qid(subif_index, cfg->queueid);
				if (index == QOS_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Qid Matched QId=%d\n", __func__, cfg->queueid);
					break;
				}
			} else {
				if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) {
					struct qos_hal_dp_res_info res = {0};
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Adding the shaper for Directpath interface!!!\n", __func__);
					qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

					subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port *
						QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);

				} /* else {
					subif_index = qos_hal_user_sub_interface_info + ((qos_res+i)->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
						dp_subif->subif;
					port_subif_index = qos_hal_user_sub_interface_info + ((qos_res+i)->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
				} */
				index = qos_hal_get_user_index_from_qid(subif_index, cfg->queueid);
				if (index == QOS_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Qid Matched QId=%d\n", __func__, cfg->queueid);
					break;
				}
			}
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Count=%d no_of_qos_ports=%d\n", __func__, fail_cnt, no_of_qos_ports);
		if (fail_cnt == no_of_qos_ports) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Invalid Queue Id %d\n", __func__, cfg->queueid);
			ret = QOS_HAL_STATUS_INVALID_QID;
			goto QOS_RESOURCES_CLEANUP;
		}
		/* QOS Queue Index */
		if (index >= 0)
			q_index = subif_index->user_queue[index].queue_index;
	} else
		q_index = qos_q_idx;

	if (q_index > QOS_HAL_MAX_EGRESS_QUEUES-1) {
		kfree(dp_subif);
		return -EINVAL;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Actual Queue Index %d\n", __func__, q_index);

	if (flags & QOS_MGR_Q_F_INGRESS) {
		if ((flags & QOS_HAL_QUEUE_SHAPER_INGRESS_MPE) == QOS_HAL_QUEUE_SHAPER_INGRESS_MPE) {
			/*for MPE Ingress Queue*/
			shaper_index = qos_hal_set_cfg_shaper_from_shaper_track(cfg);
			cfg->shaperid = shaper_index;
		}
	}
	memset(&cfg_shaper, 0, sizeof(struct qos_hal_token_bucket_shaper_cfg));
	if (qos_hal_queue_track[q_index].sched_input == 1023) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Wrong Queue %d\n", __func__, q_index);
		ret = QOS_HAL_STATUS_ERR;
		goto QOS_RESOURCES_CLEANUP;
	} else {
		if (cfg->shaperid <= 0) {
			/* Get free Shaper */
			shaper_index = qos_hal_get_free_shaper();
			if (shaper_index >= QOS_HAL_MAX_SHAPER) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> No shaper available !!!\n", __func__);
				ret = QOS_HAL_STATUS_ERR;
				goto QOS_RESOURCES_CLEANUP;
			}
			/* Add the token to the scheduler input */
			qos_hal_shaper_track[shaper_index].is_enabled = true;
			qos_hal_shaper_track[shaper_index].sb_input = qos_hal_queue_track[q_index].sched_input;
			qos_hal_queue_track[q_index].tb_index = shaper_index;
			cfg_shaper.index = shaper_index;
			cfg_shaper.cir = cfg->rate_in_kbps;

			qos_hal_shaper_track[shaper_index].tb_cfg.enable = true;
			qos_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
			qos_hal_shaper_track[shaper_index].tb_cfg.cir = cfg->rate_in_kbps ;
			qos_hal_shaper_track[shaper_index].tb_cfg.cbs = cfg->burst;
			qos_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
			/*Add the token to the scheduler input */
			qos_hal_token_bucket_shaper_create(shaper_index, qos_hal_queue_track[q_index].sched_input);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper %d is created for scheduler input %d \n",
					__func__, shaper_index, qos_hal_queue_track[q_index].sched_input);
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Shaper instance %d is already created \n", __func__, cfg->shaperid);
			cfg_shaper.index = cfg->shaperid;
			/* Add the token to the scheduler input */
			qos_hal_shaper_track[cfg->shaperid].tb_cfg.enable = true;
			qos_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
			qos_hal_queue_track[q_index].tb_index = cfg->shaperid;
			cfg_shaper.enable = true;
			cfg_shaper.mode = qos_hal_shaper_track[cfg->shaperid].tb_cfg.mode;
			cfg_shaper.cir = qos_hal_shaper_track[cfg->shaperid].tb_cfg.cir;
			cfg_shaper.pir = qos_hal_shaper_track[cfg->shaperid].tb_cfg.pir;
			cfg_shaper.cbs = qos_hal_shaper_track[cfg->shaperid].tb_cfg.cbs; ;
			cfg_shaper.pbs = qos_hal_shaper_track[cfg->shaperid].tb_cfg.pbs; ;
			shaper_index = cfg->shaperid;
			cfg_shaper.attach_to = qos_hal_shaper_track[cfg->shaperid].attach_to = DP_NODE_QUEUE;
			cfg_shaper.attach_to_id = qos_hal_shaper_track[cfg->shaperid].attach_to_id = q_index;
			qos_hal_token_bucket_shaper_cfg_set(&cfg_shaper);
			/* Add the token to the scheduler input */
			qos_hal_token_bucket_shaper_create(cfg_shaper.index, qos_hal_queue_track[q_index].sched_input);
		}
	}

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	if ((ret == QOS_HAL_STATUS_INVALID_QID) || (ret == QOS_HAL_STATUS_ERR))
		return QOS_MGR_FAILURE;
	return shaper_index;

}

int qos_hal_del_queue_rate_shaper_ex(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, int32_t qos_q_idx, uint32_t flags)
{
	int32_t i = 0, shaper_index, qosport = 0;
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t index, q_index, fail_cnt = 0;
	uint32_t no_of_qos_ports = 0;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};

	dp_subif_t *dp_subif = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;
	QOS_MGR_VCC *vcc = NULL;
	char *dev_name = NULL;

#ifdef QUEUE_FLUSH_SUPPORT
	struct dp_queue_map_entry *q_map_get = NULL;
	int32_t num_entries = 0;
#endif

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Delete Shaper for the Queue ID %d\n", __func__, cfg->queueid);

	MEM_ALLOC(dp_subif_t, dp_subif);
	QOS_HAL_ASSERT(cfg->shaperid >= QOS_HAL_MAX_SHAPER);
	dev_name = cfg->dev_name;

	if (qos_q_idx == -1) {
		if (cfg->portid < 0) {
			if (netdev != NULL)
				qos_mgr_br2684_get_vcc(netdev, &vcc);

			if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __func__, __LINE__);
				if (dp_get_port_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __func__, __LINE__);
					if (qos_hal_get_subitf_via_ifname(dev_name, dp_subif) != QOS_MGR_SUCCESS) {
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from qos_hal_get_subitf_via_ifname \n", __func__, __LINE__);
						kfree(dp_subif);
						return QOS_HAL_STATUS_ERR;
					}
				}
			}
		} else
			dp_subif->port_id = cfg->portid;

		dp_subif->subif  = dp_subif->subif >> 8;
		dq_res.inst = 0;
		dq_res.dev = netdev;
		dq_res.dp_port = dp_subif->port_id;

		if (flags & QOS_MGR_Q_F_INGRESS) {
			dq_res.dev = NULL;
			dq_res.dp_port = 0;
		}
		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		if (dq_res.num_q == 0) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
		if (dq_res.q_res == NULL) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);

		dq_res.q_res_size = dq_res.num_q;
		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			ret = QOS_HAL_STATUS_ERR;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			goto QOS_RESOURCES_CLEANUP;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
				__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

		qosport = qos_res->cqm_deq_port;
		no_of_qos_ports = dq_res.num_deq_ports;
		if (flags & QOS_MGR_Q_F_INGRESS) {
			qosport = qos_hal_get_ingress_index(flags);
			if (qosport == QOS_HAL_STATUS_ERR) {
				ret = QOS_HAL_STATUS_ERR;
				goto QOS_RESOURCES_CLEANUP;
			}
			port_subif_index = qos_hal_user_sub_interface_ingress_info + qosport;
		} else {
			port_subif_index = qos_hal_user_sub_interface_info + qosport;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> port_subif_index: [%p] head:[%p]\n",
				__func__, port_subif_index, port_subif_index->head);

		qos_hal_get_subif_list_item(
				qosport,
				netdev,
				&subif_index,
				&port_subif_index->head);

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif_index:[%p]\n",
				__func__, subif_index);

		for (i = 0; i < no_of_qos_ports; i++) {
			if ((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS) {
				index = qos_hal_get_user_index_from_qid(subif_index, cfg->queueid);
				if (index == QOS_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Qid Matched QId=%d\n", __func__, cfg->queueid);
					break;
				}
			} else {
				if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) {
					struct qos_hal_dp_res_info res = {0};
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Deleting the shaper for Directpath interface!!!\n", __func__);
					qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

					subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port *
					QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
					port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);

				} else {
					subif_index = qos_hal_user_sub_interface_info + ((qos_res+i)->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) +
						dp_subif->subif;
					port_subif_index = qos_hal_user_sub_interface_info + ((qos_res+i)->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
				}
				index = qos_hal_get_user_index_from_qid(subif_index, cfg->queueid);
				if (index == QOS_HAL_STATUS_ERR) {
					fail_cnt++;
				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Qid Matched QId=%d\n", __func__, cfg->queueid);
					break;
				}
			}
		}
		if (fail_cnt == no_of_qos_ports) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Invalid Queue Id %d\n", __func__, cfg->queueid);
			ret = QOS_HAL_STATUS_INVALID_QID;
			goto QOS_RESOURCES_CLEANUP;
		}
		/* Find the user queue index for user qid from the user queue array */
		index = qos_hal_get_user_index_from_qid(subif_index, cfg->queueid);
		if (index == QOS_HAL_STATUS_ERR) {
			ret = QOS_HAL_STATUS_INVALID_QID;
			goto QOS_RESOURCES_CLEANUP;
		}

		/* QOS Queue Index */
		q_index = subif_index->user_queue[index].queue_index;
	} else {
		q_index = qos_q_idx;
		kfree(dp_subif);
		return -EINVAL;
	}

	if (q_index > QOS_HAL_MAX_EGRESS_QUEUES-1) {
		kfree(dp_subif);
		return -EINVAL;
	}
	if (((flags & QOS_MGR_Q_F_INGRESS) == QOS_MGR_Q_F_INGRESS) && ((flags & QOS_HAL_QUEUE_SHAPER_INGRESS_MPE) == QOS_HAL_QUEUE_SHAPER_INGRESS_MPE))
		shaper_index = qos_hal_queue_track[q_index].tb_index;
	else
		shaper_index = cfg->shaperid;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Delete Shaper %d of Queue %d\n", __func__, shaper_index, q_index);
#ifdef QUEUE_FLUSH_SUPPORT
	qos_hal_get_q_map(g_Inst, q_index, &num_entries, &q_map_get, 0);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> %d No of entries is %d for queue=%d\n", __func__, __LINE__, num_entries, q_index);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Q map pointer =%p\n", __func__, q_map_get);
	if (qos_hal_flush_queue_if_qocc(netdev, dev_name, q_index, -1, -1, flags) != QOS_HAL_STATUS_OK) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR to flush qocc\n", __func__, __LINE__);
		ret = QOS_HAL_STATUS_ERR;
		goto QOS_RESOURCES_CLEANUP;
	}
#endif
	/* Delete the Tocken Bucket */
	qos_hal_token_bucket_shaper_delete(shaper_index, qos_hal_queue_track[q_index].sched_input);

	qos_hal_sched_blk_in_enable(qos_hal_queue_track[q_index].sched_input, 1);

	if ((flags & QOS_HAL_DEL_SHAPER_CFG) == QOS_HAL_DEL_SHAPER_CFG) {
		/* Delete the shaper instance */
		qos_hal_del_shaper_index(shaper_index);
		/* Enable the scheduler block input */
		qos_hal_sched_blk_in_enable(qos_hal_queue_track[q_index].sched_input, 1);
	}

#ifdef QUEUE_FLUSH_SUPPORT
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Enable Queue %d for netdev %s\n", __func__, q_index, netdev->name);
	if (qos_hal_enable_queue_after_flush(netdev, dev_name, q_index, -1, q_index, flags) != QOS_HAL_STATUS_OK) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR to enable queue after queue flush\n", __func__, __LINE__);
		ret = QOS_HAL_STATUS_ERR;
		goto QOS_RESOURCES_CLEANUP;
	}
	if (num_entries > 0) {
		int32_t j;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Q map pointer =%p no_entries=%d\n", q_map_get, num_entries);
		for (j = 0; j < num_entries; j++)
			qos_hal_add_q_map(g_Inst, q_index, &q_map_get[j], 0);
		num_entries = 0;
	}

#endif
	qos_hal_queue_track[q_index].tb_index = 0xFF;

QOS_RESOURCES_CLEANUP:
	kfree(q_map_get);
	q_map_get = NULL;
	kfree(qos_res);
	kfree(dp_subif);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return ret;

}

typedef struct {
	struct dp_queue_map_entry *q_map;
	int32_t no_of_entries;

} qos_hal_queue_qmap;

/** The qos_hal_add_ingress_port_rate_shaper adds the port rate shaper.
    ** 1> Get free Shaper index
    ** 2> Add a Shaper Scheduler for that logical port.
    ** 3> Remap the scheduler which is connected to the logical Port(scheduler) previously.
    ** 4> Update the logical Port(scheduler) tracking information.
    ** 5> Create the shaper and configure it.
    ** 6> Update the scheduler/shaper tracking information.
    ** 7> Configure the shaper.
*/
int qos_hal_add_cpu_ingress_port_rate_shaper(struct net_device *netdev,
				QOS_RATE_SHAPING_CFG *cfg,
				struct qos_hal_user_subif_abstract *subif_index_logical,
				uint32_t flags)
{
	uint8_t leaf = 0;
	uint32_t p_sched;
	int32_t sched_old, shaper_index, sched_new;
	struct qos_hal_token_bucket_shaper_cfg cfg_shaper = {0};
	uint32_t qosport;

	qosport = qos_hal_get_ingress_index(flags);
	if (qosport == QOS_HAL_STATUS_ERR)
		return QOS_MGR_FAILURE;

	sched_old = subif_index_logical->base_sched_id_egress;

	if (subif_index_logical->port_shaper_sched == 0xFF) {
		if (cfg->shaperid <= 0) {
			shaper_index = qos_hal_get_free_shaper();
			if (shaper_index >= QOS_HAL_MAX_SHAPER) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Invalid Shaper index. Shaper not available.!!!! \n", __func__);
				shaper_index = QOS_HAL_STATUS_ERR;
				return QOS_MGR_FAILURE;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>New Shaper %d is created \n", __func__, shaper_index);
		} else  {
			shaper_index = cfg->shaperid;
		}

		leaf = qos_hal_sched_track[sched_old].omid & 0x7;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port Scheduler Id %d connected to leaf %d\n",
				__func__, qos_hal_sched_track[sched_old].omid, leaf);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port Scheduler level is %d \n", __func__, qos_hal_sched_track[sched_old].level);
		sched_new = create_new_scheduler(qos_hal_sched_track[sched_old].omid >> 3,
				 0, QOS_HAL_POLICY_WSP,
				qos_hal_sched_track[sched_old].level, leaf);
		if (sched_new < 0) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR,  "ERRoR schedular Not available/ busy status=%d\n", sched_new);
			return QOS_HAL_STATUS_ERR;
		}
		if (sched_new == QOS_HAL_MAX_SCHEDULER)
			return -EINVAL;

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>New Port Shaper Scheduler %d\n", __func__, sched_new);

		qos_hal_sched_track[sched_new].policy = QOS_HAL_POLICY_WSP;

		subif_index_logical->port_shaper_sched = sched_new;
		qos_hal_sched_track[sched_new].next_sched_id = sched_old;

		/** Remap the already connected scheduler */
		p_sched = sched_new << 3;
		qos_hal_sched_out_remap(sched_old, qos_hal_sched_track[sched_old].level + 1, p_sched, 1, 0);

		/* Add the token to the scheduler input */
		qos_hal_token_bucket_shaper_create(shaper_index, p_sched);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Shaper %d is created for scheduler input %d \n", shaper_index, sched_new);


		qos_hal_shaper_track[shaper_index].is_enabled = true;
		qos_hal_shaper_track[shaper_index].sb_input = p_sched;

		subif_index_logical->shaper_idx = shaper_index;
		qos_hal_sched_track[sched_new].tbs = shaper_index;

	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Shaper %d is already created \n", __func__, subif_index_logical->shaper_idx);
		shaper_index = subif_index_logical->shaper_idx;
	}

	qos_hal_shaper_track[shaper_index].tb_cfg.enable = true;
	qos_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
	cfg_shaper.index = shaper_index;
	cfg_shaper.enable = true;
	cfg_shaper.mode = qos_hal_shaper_track[shaper_index].tb_cfg.mode;
	cfg_shaper.cir = qos_hal_shaper_track[shaper_index].tb_cfg.cir;
	cfg_shaper.pir = qos_hal_shaper_track[shaper_index].tb_cfg.pir;
	cfg_shaper.cbs = qos_hal_shaper_track[shaper_index].tb_cfg.cbs;
	cfg_shaper.pbs = qos_hal_shaper_track[shaper_index].tb_cfg.pbs;

	qos_hal_token_bucket_shaper_cfg_set(&cfg_shaper);

	return QOS_HAL_STATUS_OK;
}

int qos_hal_add_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	uint32_t qosport;
	int32_t cpu_shaper_index;
	struct qos_hal_user_subif_abstract *subif_index_logical = NULL;

	qosport = qos_hal_get_ingress_index(flags);
	if (qosport == QOS_HAL_STATUS_ERR)
		return QOS_MGR_FAILURE;

	subif_index_logical = qos_hal_user_sub_interface_ingress_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT);
	qos_hal_add_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical, flags);
	cpu_shaper_index = cfg->shaperid;

	return QOS_HAL_STATUS_OK;
}


/** The qos_hal_add_egress_port_rate_shaper adds the port rate shaper.
    ** 1> Get free Shaper index
    ** 2> Add a Shaper Scheduler for that port.
    ** 3> Remap the scheduler which is connected to the Port previously.
    ** 4> Update the Port tracking information.
    ** 5> Create the shaper and configure it.
    ** 6> Update the scheduler/shaper tracking information.
    ** 7> Configure the shaper.
*/
int qos_hal_add_egress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	int32_t shaper_index = -1, port = 0, count = 0, ret = 0;
	struct qos_hal_token_bucket_shaper_cfg cfg_shaper;
	uint32_t j = 0;
	uint32_t no_of_qos_ports;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};
	dp_subif_t *dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;
	struct qos_hal_user_subif_abstract *subif_index_logical = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *base_subif_index;
	int32_t sched_old, prev_shaper_id = 0;
	char *dev_name = NULL;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);

	dev_name = cfg->dev_name;

	MEM_ALLOC(dp_subif_t, dp_subif);
	if (netdev != NULL)
		qos_mgr_br2684_get_vcc(netdev, &vcc);

	if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __func__, __LINE__);
		if (dp_get_port_subitf_via_ifname(cfg->dev_name, dp_subif) != QOS_MGR_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __func__, __LINE__);
			if (qos_hal_get_subitf_via_ifname(cfg->dev_name, dp_subif) != QOS_MGR_SUCCESS) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from qos_hal_get_subitf_via_ifname \n", __func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
		}
	}

	dp_subif->subif = dp_subif->subif >> 8;

	dq_res.inst = 0;
	dq_res.dev = netdev;
	dq_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to Get QOS Resources\n",
				__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d No queue resources\n",
				__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
	if (dq_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);

	dq_res.q_res_size = dq_res.num_q;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to Get QOS Resources\n",
				__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}

	no_of_qos_ports = dq_res.num_deq_ports;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

	if (high_prio_q_limit < 17) {
		if (no_of_qos_ports > 1)
			count = 1;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Nof Of QOS Ports [%d] Count [%d]\n",
				__func__, no_of_qos_ports, count);

	base_subif_index = (qos_hal_user_sub_interface_info + qos_res->cqm_deq_port);
	qos_hal_get_subif_list_item(qos_res->cqm_deq_port,
				netdev,
				&subif_index,
				&base_subif_index->head);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "base_subif_index:%p,base_subif_index->head:%p subif_index:%p\n",
					base_subif_index, base_subif_index->head, subif_index);

	if (subif_index->is_logical == 0) {
		for (j = 0; j <= count; j++) { /* more than 1 port for per interface */
			int32_t temp_shaper_id = 0;

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Count=%d loop=%d\n", count, j);
			port = (qos_res + j)->cqm_deq_port;
			qos_hal_dump_port_info(port);

			if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) { /* for directpath interface */
				struct qos_hal_dp_res_info res = {0};
				struct qos_hal_user_subif_abstract *port_subif_index = NULL;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Adding the shaper for Directpath interface!!!\n", __func__);
				qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

				port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);
				port = res.dp_egress_res.dp_port;

			}
			/** For the 1st QOS port the shaper instance is already created by QOS MGR
			  For the 2nd QOS port shaper instance has to be created by the HAL
			 */
			if (j == 0)
				prev_shaper_id = temp_shaper_id = cfg->shaperid;

			/* If there is already a port shaper configured, update the rate */
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> temp_shaper_id %d\n",
				__func__, temp_shaper_id);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>subif_index->base_sched_id_eg is %d & port_sched_id_eg %d\n",
				__func__, subif_index->base_sched_id_egress, subif_index->port_sched_in_egress);
			if (qos_hal_port_track[port].is_enabled && (qos_hal_port_track[port].tbs != INVALID_SHAPER_ID)) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Shaper qos_hal_port_track[port].tbs != INVALID_SHAPER_ID \n", __func__);
				shaper_index = qos_hal_port_track[port].tbs;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Shaper [%d] is already there for the port [%d]\n",
						__func__, shaper_index, port);
				cfg_shaper.attach_to = DP_NODE_PORT;
				cfg_shaper.attach_to_id = port;
			/* If there is no portshaper configured, then use the input shaper index and set the port shaper rate.
				If the input shaper index is -1, allocate a new shaper instance and set the rate */
			} else if (qos_hal_port_track[port].tbs == INVALID_SHAPER_ID) {
				cfg_shaper.attach_to = DP_NODE_PORT;
				cfg_shaper.attach_to_id = port;
				/* Get free Shaper */
				if (temp_shaper_id <= 0) {
					shaper_index = qos_hal_get_free_shaper();
					if (shaper_index >= QOS_HAL_MAX_SHAPER)	{
						QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Free Shaper index not available\n",
								__func__);
						shaper_index = QOS_HAL_STATUS_ERR;
						goto QOS_RESOURCES_CLEANUP;
					}
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>New Shaper index [%d] is created\n",
							__func__, shaper_index);
					qos_hal_shaper_track[shaper_index].is_enabled = true;
					qos_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;
					qos_hal_shaper_track[shaper_index].tb_cfg.enable = false;
					qos_hal_shaper_track[shaper_index].tb_cfg.mode = qos_hal_shaper_track[prev_shaper_id].tb_cfg.mode;
					qos_hal_shaper_track[shaper_index].tb_cfg.pir = qos_hal_shaper_track[prev_shaper_id].tb_cfg.pir;
					qos_hal_shaper_track[shaper_index].tb_cfg.cir = qos_hal_shaper_track[prev_shaper_id].tb_cfg.cir;
					qos_hal_shaper_track[shaper_index].tb_cfg.pbs = qos_hal_shaper_track[prev_shaper_id].tb_cfg.pbs;
					qos_hal_shaper_track[shaper_index].tb_cfg.cbs = qos_hal_shaper_track[prev_shaper_id].tb_cfg.cbs;

				} else {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Shaper index [%d] is already created\n",
							__func__, cfg->shaperid);
					shaper_index = cfg->shaperid;
					prev_shaper_id = cfg->shaperid;
				}
				qos_hal_port_track[port].tbs = shaper_index;
			}

			if (temp_shaper_id <= 0) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> temp_shaper_id <= 0 \n", __func__);

				cfg_shaper.index = shaper_index;
				cfg_shaper.cir = cfg->rate_in_kbps;
				cfg_shaper.pir = cfg->rate_in_kbps;

				cfg_shaper.enable = true;
				qos_hal_shaper_track[shaper_index].tb_cfg.index = shaper_index;

				cfg_shaper.mode = qos_hal_shaper_track[shaper_index].tb_cfg.mode;
				cfg_shaper.cir = qos_hal_shaper_track[shaper_index].tb_cfg.cir;
				cfg_shaper.pir = qos_hal_shaper_track[shaper_index].tb_cfg.pir;
				cfg_shaper.cbs = qos_hal_shaper_track[shaper_index].tb_cfg.cbs;
				cfg_shaper.pbs = qos_hal_shaper_track[shaper_index].tb_cfg.pbs;
			} else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> temp_shaper_id > 0 \n", __func__);
				qos_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
				cfg_shaper.index = shaper_index;
				cfg_shaper.enable = true;
				cfg_shaper.mode = qos_hal_shaper_track[shaper_index].tb_cfg.mode;
				cfg_shaper.cir = qos_hal_shaper_track[shaper_index].tb_cfg.cir;
				cfg_shaper.pir = qos_hal_shaper_track[shaper_index].tb_cfg.pir;
				cfg_shaper.cbs = qos_hal_shaper_track[shaper_index].tb_cfg.cbs;
				cfg_shaper.pbs = qos_hal_shaper_track[shaper_index].tb_cfg.pbs;
			}
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Set the configuration for TBID %d\n",
					__func__, cfg_shaper.index);
			if (qos_hal_token_bucket_shaper_cfg_set(&cfg_shaper) != QOS_HAL_STATUS_OK) {
				ret = QOS_HAL_STATUS_ERR;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to configure shaper\n",
						__func__, __LINE__);
				goto QOS_RESOURCES_CLEANUP;
			}
			qos_hal_shaper_track[shaper_index].tb_cfg.enable = true;
			qos_hal_shaper_track[shaper_index].attach_to = cfg_shaper.attach_to;
			qos_hal_shaper_track[shaper_index].attach_to_id = cfg_shaper.attach_to_id;
			qos_hal_dump_port_info(port);
			qos_hal_dump_shaper_info(shaper_index);
		}
	} else { /* add port rate shaper for logical interface VLAN/VAP's */
		uint8_t leaf = 0;

		subif_index_logical = subif_index;
		if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) { /* for directpath interface */
			struct qos_hal_dp_res_info res = {0};
			struct qos_hal_user_subif_abstract *port_subif_index = NULL;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Adding the shaper for Directpath interface!!!\n", __func__);
			qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

			subif_index_logical = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port *
			QOS_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif);
			port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index logical is %p\n", __func__, subif_index_logical);
			port = res.dp_egress_res.dp_port;

		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> netdev:%s port_sched_in_egress : %d base_sched_id_egress: %d\n",
				__func__, netdev->name, subif_index_logical->port_sched_in_egress, subif_index_logical->base_sched_id_egress);

		sched_old = subif_index_logical->base_sched_id_egress;
		/* if subif interface port sched is not configured/0xFF, use the
		 * cfg shaper index or create a new shaper instance and attach
		 * it to the subif port sched. There is new sched instance creation
		 * for port sched. Shaper is property of the sched.
		 */

		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>subif_index_logical->base_sched_id_eg is %d & port_sched_id_eg %d\n",
				__func__, subif_index_logical->base_sched_id_egress, subif_index_logical->port_sched_in_egress);

		if (subif_index_logical->shaper_idx == INVALID_SHAPER_ID) {
			if (cfg->shaperid <= 0) {
				shaper_index = qos_hal_get_free_shaper();
				if (shaper_index >= QOS_HAL_MAX_SHAPER) {
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Shaper not available\n", __func__);
					shaper_index = QOS_HAL_STATUS_ERR;
					goto QOS_RESOURCES_CLEANUP;
				}
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>New Shaper %d is created\n",
					 __func__, shaper_index);
			} else
				shaper_index = cfg->shaperid;

			leaf = qos_hal_sched_track[sched_old].omid & 0x7;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Port Scheduler Id %d connected to leaf %d\n",
					__func__, qos_hal_sched_track[sched_old].omid, leaf);

			qos_hal_shaper_track[shaper_index].sb_input = subif_index_logical->port_sched_in_egress;

			subif_index_logical->shaper_idx = shaper_index;
			qos_hal_sched_track[subif_index_logical->port_sched_in_egress].tbs = shaper_index;
		} else {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Shaper %d is already created\n",
					__func__, subif_index_logical->shaper_idx);
			shaper_index = subif_index_logical->shaper_idx;
		}
		cfg_shaper.attach_to = DP_NODE_SCH;
		cfg_shaper.attach_to_id = subif_index_logical->base_sched_id_egress;

		qos_hal_shaper_track[shaper_index].tb_cfg.index = cfg->shaperid;
		cfg_shaper.index = shaper_index;
		cfg_shaper.enable = true;
		cfg_shaper.mode = qos_hal_shaper_track[shaper_index].tb_cfg.mode;
		cfg_shaper.cir = qos_hal_shaper_track[shaper_index].tb_cfg.cir;
		cfg_shaper.pir = qos_hal_shaper_track[shaper_index].tb_cfg.pir;
		cfg_shaper.cbs = qos_hal_shaper_track[shaper_index].tb_cfg.cbs;
		cfg_shaper.pbs = qos_hal_shaper_track[shaper_index].tb_cfg.pbs;

		if (qos_hal_token_bucket_shaper_cfg_set(&cfg_shaper) != QOS_HAL_STATUS_OK) {
			ret = QOS_HAL_STATUS_ERR;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to configure shaper\n",
					__func__, __LINE__);
			goto QOS_RESOURCES_CLEANUP;
		}
		qos_hal_shaper_track[shaper_index].tb_cfg.enable = true;
		qos_hal_shaper_track[shaper_index].attach_to = cfg_shaper.attach_to;
		qos_hal_shaper_track[shaper_index].attach_to_id = cfg_shaper.attach_to_id;

	}

QOS_RESOURCES_CLEANUP:
	if (dq_res.num_q != 0)
		kfree(qos_res);
	kfree(dp_subif);
	return shaper_index;
}

int qos_hal_add_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);

	if ((flags & QOS_MGR_OP_F_INGRESS) == QOS_MGR_OP_F_INGRESS) {
		return qos_hal_add_ingress_port_rate_shaper(
				netdev,
				cfg,
				flags);
	} else { /* Handling for Egress interface */
		return qos_hal_add_egress_port_rate_shaper(
				netdev,
				cfg,
				flags);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return QOS_HAL_STATUS_OK;

}

int qos_hal_del_cpu_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, struct qos_hal_user_subif_abstract *subif_index_logical, uint32_t flags)
{
	int32_t shaper_index;
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t qosport;
	uint32_t next_sched_to_shaper;
	int32_t omid = 0;
	uint32_t subif_lvl = 0;

	qosport = qos_hal_get_ingress_index(flags);
	if (qosport == QOS_HAL_STATUS_ERR)
		return QOS_MGR_FAILURE;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> netdev:%s port_sched_in_egress : %d \n",
			__func__, netdev->name, subif_index_logical->port_sched_in_egress);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Port Shaper Scheduler %d\n",
			__func__, subif_index_logical->port_shaper_sched);

	if (subif_index_logical->port_shaper_sched == INVALID_SCHED_ID) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Invalid Shaper Scheduler Id: %d \n",
				__func__, subif_index_logical->port_shaper_sched);
		ret = QOS_HAL_STATUS_ERR;
		return QOS_MGR_FAILURE;
	} else	{
		if (subif_index_logical->shaper_idx == INVALID_SHAPER_ID) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Invalid Shaper Id \n", __func__);
			ret = QOS_HAL_STATUS_ERR;
			return QOS_MGR_FAILURE;
		} else
			shaper_index = subif_index_logical->shaper_idx;
	}

	omid = qos_hal_sched_track[subif_index_logical->port_shaper_sched].omid;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Shaper Index %d\n", __func__, subif_index_logical->shaper_idx);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Port Shaper Scheduler %d\n", __func__, subif_index_logical->port_shaper_sched);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> OMID for Port Shaper Scheduler %d\n", __func__, omid);

	qos_hal_sched_blk_in_enable(subif_index_logical->port_shaper_sched << 3, 0);

	qos_hal_token_bucket_shaper_assign_set_to_invalid(shaper_index, subif_index_logical->port_shaper_sched << 3);
	/* Delete the Tocken Bucket */
	qos_hal_token_bucket_shaper_delete(shaper_index, subif_index_logical->port_shaper_sched << 3);
	next_sched_to_shaper = qos_hal_sched_track[subif_index_logical->port_shaper_sched].next_sched_id;
	subif_lvl = qos_hal_sched_track[subif_index_logical->port_shaper_sched].level;

	/* Delete the Scheduler */
	qos_hal_scheduler_delete(subif_index_logical->port_shaper_sched);
	if (next_sched_to_shaper != INVALID_SCHED_ID) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Remap SB %d --> OMID %d\n",
				__func__, next_sched_to_shaper, omid);
		qos_hal_sched_out_remap(next_sched_to_shaper, subif_lvl, omid, 1, 0);
	}

	/* Update the shaper tracking */
	qos_hal_shaper_track[shaper_index].is_enabled = false;
	qos_hal_shaper_track[shaper_index].sb_input = 0xFF;

	/* Update the scheduler tracking  */
	qos_hal_sched_track[subif_index_logical->port_shaper_sched].in_use = 0;
	qos_hal_sched_track[subif_index_logical->port_shaper_sched].tbs = INVALID_SHAPER_ID;
	qos_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask = 0;
	qos_hal_sched_track[subif_index_logical->port_shaper_sched].policy = QOS_HAL_POLICY_NULL;

	subif_index_logical->shaper_idx = INVALID_SHAPER_ID;
	subif_index_logical->port_shaper_sched = INVALID_SCHED_ID;

	return shaper_index;
}

int qos_hal_del_ingress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	uint32_t qosport;
	struct qos_hal_user_subif_abstract *subif_index_logical = NULL;

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);
	qosport = qos_hal_get_ingress_index(flags);
	if (qosport == QOS_HAL_STATUS_ERR)
		return QOS_MGR_FAILURE;

	subif_index_logical = qos_hal_user_sub_interface_ingress_info + (qosport * QOS_HAL_MAX_SUB_IFID_PER_PORT);
	qos_hal_del_cpu_ingress_port_rate_shaper(netdev, cfg, subif_index_logical, flags);

	return QOS_HAL_STATUS_OK;
}


/** The qos_hal_del_egress_port_rate_shaper deletes the port rate shaper.
 ** 1> Find the shaper scheduler id from the shaper index
 ** 2> Before deleting the token bucket make sure that all the packets
 are dequeued properly.
 ** 3> Remap the scheduler which is connected to the shaper scheduler previously.
 ** 4> Update the port tracking information.
 ** 5> Update the shaper tracking information.
 ** 6> Update the scheduler tracking information.
 */
int qos_hal_del_egress_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	int32_t shaper_index, port, count = 0, sys_q = 0;
	int32_t ret = QOS_HAL_STATUS_OK;
	uint32_t k = 0;
	uint32_t no_of_qos_ports;
	uint32_t sb_conn = 0, next_sched_to_shaper;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};

	dp_subif_t *dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;
	struct qos_hal_user_subif_abstract *subif_index_logical = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *base_subif_index;
	char *dev_name = NULL;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Delete Port rate Shaper \n");

	MEM_ALLOC(dp_subif_t, dp_subif);
	dev_name = cfg->dev_name;
	if (netdev != NULL)
		qos_mgr_br2684_get_vcc(netdev, &vcc);

	if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_netif_subifid \n", __func__, __LINE__);
		if (dp_get_port_subitf_via_ifname(cfg->dev_name, dp_subif) != QOS_MGR_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from dp_get_port_subitf_via_ifname \n", __func__, __LINE__);
			if (qos_hal_get_subitf_via_ifname(cfg->dev_name, dp_subif) != QOS_MGR_SUCCESS) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d ERROR Failed to Get Subif Id when VCC NULL from qos_hal_get_subitf_via_ifname \n", __func__, __LINE__);
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
		}
	}

	dp_subif->subif = dp_subif->subif >> 8;

	dq_res.inst = 0;
	dq_res.dev = netdev;
	dq_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Fail to Get Q Resources\n",
				__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
	if (dq_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);

	dq_res.q_res_size = dq_res.num_q;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Fail to Get Q Resources\n",
				__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	no_of_qos_ports = dq_res.num_deq_ports;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

	if (high_prio_q_limit < 17) {
		if (no_of_qos_ports > 1)
			count = 1;
	}

	base_subif_index = (qos_hal_user_sub_interface_info + qos_res->cqm_deq_port);
	qos_hal_get_subif_list_item(qos_res->cqm_deq_port,
			netdev,
			&subif_index,
			&base_subif_index->head);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "base_subif_index: %p, base_subif_index->head:%p subif_index:%p\n",
				base_subif_index, base_subif_index->head, subif_index);

	if (subif_index->is_logical == 0) {/* for the physical interface */
		for (k = 0; k <= count; k++) {
			port = (qos_res + k)->cqm_deq_port;
			sys_q = (qos_res + k)->q_id;

			qos_hal_dump_port_info(port);

			if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) { /* for directpath interface */
				struct qos_hal_dp_res_info res = {0};
				struct qos_hal_user_subif_abstract *subif_index = NULL;
				struct qos_hal_user_subif_abstract *port_subif_index = NULL;
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Deleting the shaper for Directpath interface!!!\n", __func__);
				qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

				subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port *
					QOS_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif >> 8);
				port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index is %p\n", __func__, subif_index);
				port = res.dp_egress_res.dp_port;
				sb_conn = qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].omid;

			}
			if (qos_hal_port_track[port].tbs == INVALID_SHAPER_ID) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Invalid Shaper [%d] for the port %d\n",
						__func__, qos_hal_port_track[port].tbs, port);
				ret = QOS_HAL_STATUS_ERR;
				goto QOS_RESOURCES_CLEANUP;
			} else	{
				shaper_index = qos_hal_port_track[port].tbs;
			}

			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Delete Shaper %d\n", shaper_index);
			qos_hal_dump_shaper_info(shaper_index);

			/* Delete the Tocken Bucket */
			qos_hal_token_bucket_shaper_delete(shaper_index, qos_hal_port_track[port].shaper_sched_id << 3);
			next_sched_to_shaper = qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].next_sched_id;
			/* Delete the Scheduler */
			/** Remap the next scheduler id that is connected to the shaper scheduler */
			if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) { /* for directpath interface */
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Remap SB %d --> OMID %d\n", __func__, next_sched_to_shaper, sb_conn);
				qos_hal_sched_out_remap(next_sched_to_shaper,
						qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].level,
						sb_conn, 1, 0);
			}
#if 0 
			else {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Remap SB %d --> OMID %d\n", qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].next_sched_id, port);
				qos_hal_sched_out_remap(next_sched_to_shaper, 0, port, 0, 0);
			}
			qos_hal_dump_sb_info(next_sched_to_shaper);
#endif
			/* Update the port tracking */
			qos_hal_port_track[port].input_sched_id = next_sched_to_shaper;

			/* Update the shaper tracking */
			qos_hal_shaper_track[shaper_index].is_enabled = false;
			qos_hal_shaper_track[shaper_index].sb_input = 0xFF;

			/* Update the scheduler tracking */
			/*qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].in_use = 0;
			qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].tbs = INVALID_SHAPER_ID;
			qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].leaf_mask = 0;
			qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].policy = QOS_HAL_POLICY_NULL;

			qos_hal_port_track[port].shaper_sched_id = INVALID_SCHED_ID; */
			qos_hal_port_track[port].tbs = INVALID_SHAPER_ID;

		}
	} else { /* for logical interface VLAN/VAP's */
		int32_t omid = 0;

		subif_index_logical = subif_index;
		if ((dp_subif->port_id != 0) && (qos_res->cqm_deq_port == g_CPU_PortId)) { /* for directpath interface */
			struct qos_hal_dp_res_info res = {0};
			struct qos_hal_user_subif_abstract *port_subif_index = NULL;
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Deleting the shaper for Directpath interface!!!\n", __func__);
			qos_hal_dp_port_resources_get(dp_subif->port_id, &res);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Port Id for Directpath interface is %d\n", __func__, res.dp_egress_res.dp_port);

			subif_index_logical = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port *
				QOS_HAL_MAX_SUB_IFID_PER_PORT) + (dp_subif->subif);
			port_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT);
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif index logical is %p\n", __func__, subif_index_logical);
			port = res.dp_egress_res.dp_port;
			sb_conn = qos_hal_sched_track[qos_hal_port_track[port].shaper_sched_id].omid;

		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> netdev:%s port_sched_in_egress : %d \n", __func__, netdev->name, subif_index_logical->port_sched_in_egress);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Port Shaper Scheduler %d\n", __func__, subif_index_logical->port_shaper_sched);

		if (subif_index_logical->shaper_idx == INVALID_SHAPER_ID) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Invalid Shaper Scheduler Id: %d for the logical interface %d\n",
					__func__, subif_index_logical->port_shaper_sched, dp_subif->subif);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCES_CLEANUP;
		} else	{
			if (subif_index_logical->shaper_idx == INVALID_SHAPER_ID) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s> Invalid Shaper Id \n", __func__);
				ret = QOS_HAL_STATUS_ERR;
				goto QOS_RESOURCES_CLEANUP;
			} else
				shaper_index = subif_index_logical->shaper_idx;
		}


		omid = qos_hal_sched_track[subif_index_logical->port_shaper_sched].omid;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Shaper Index %d\n", __func__, subif_index_logical->shaper_idx);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Delete Port Shaper Scheduler %d\n", __func__, subif_index_logical->port_shaper_sched);
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> OMID for Port Shaper Scheduler %d\n", __func__, omid);

		/* Delete the Tocken Bucket */
		qos_hal_token_bucket_shaper_delete(shaper_index, subif_index_logical->port_shaper_sched << 3);

		/* Update the shaper tracking */
		qos_hal_shaper_track[shaper_index].is_enabled = false;
		qos_hal_shaper_track[shaper_index].sb_input = 0xFF;

		/* Update the scheduler tracking */
		/*qos_hal_sched_track[subif_index_logical->port_shaper_sched].in_use = 0;
		qos_hal_sched_track[subif_index_logical->port_shaper_sched].tbs = INVALID_SHAPER_ID;
		qos_hal_sched_track[subif_index_logical->port_shaper_sched].leaf_mask = 0;
		qos_hal_sched_track[subif_index_logical->port_shaper_sched].policy = QOS_HAL_POLICY_NULL;*/

		/* subif_index_logical->port_sched_in_egress =  omid; */
		subif_index_logical->shaper_idx = INVALID_SHAPER_ID;
		subif_index_logical->port_shaper_sched = INVALID_SCHED_ID;

	}

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);
	return ret;
}

/** The qos_hal_del_port_rate_shaper deletes the port rate shaper.
 ** 1> Find the shaper scheduler id from the shaper index
 ** 2> Before deleting the token bucket make sure that all the packets
 are dequeued properly.
 ** 3> Remap the scheduler which is connected to the shaper scheduler previously.
 ** 4> Update the port tracking information.
 ** 5> Update the shaper tracking information.
 ** 6> Update the scheduler tracking information.
 */
int qos_hal_del_port_rate_shaper(struct net_device *netdev, QOS_RATE_SHAPING_CFG *cfg, uint32_t flags)
{
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Enter --> \n", __func__);

	if ((flags & QOS_MGR_OP_F_INGRESS) == QOS_MGR_OP_F_INGRESS) {
		return qos_hal_del_ingress_port_rate_shaper(
				netdev,
				cfg,
				flags);
	} else { /* Handling for Egress interface */
		return qos_hal_del_egress_port_rate_shaper(
				netdev,
				cfg,
				flags);
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> <-- Exit --> \n", __func__);
	return QOS_HAL_STATUS_OK;

}

int qos_hal_get_subif_from_netdev(
		struct net_device *netdev,
		uint32_t *port_id,
		uint32_t *subif_id)
{
	dp_subif_t *dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;
	uint32_t flags = 0;

	MEM_ALLOC(dp_subif_t, dp_subif);
	dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
	qos_mgr_br2684_get_vcc(netdev, &vcc);
	if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, flags) != QOS_MGR_SUCCESS) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	*port_id = dp_subif->port_id;
	*subif_id = dp_subif->subif;
	kfree(dp_subif);
	return QOS_HAL_STATUS_OK;
}

int qos_hal_get_queue_num(struct net_device *netdev, int32_t portid, int32_t *q_num)
{
	int ret = QOS_HAL_STATUS_OK;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};

	dp_subif_t *dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;

	MEM_ALLOC(dp_subif_t, dp_subif);
	if (netdev != NULL) {
		qos_mgr_br2684_get_vcc(netdev, &vcc);
		if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
	} else {
		dp_subif->port_id = portid;
		dp_subif->subif = 0;
	}

#ifdef QOS_HAL_TEST
	dp_subif->port_id = 15;
	dp_subif->subif = 0;
#endif

	dp_subif->subif = dp_subif->subif >> 8;

	dq_res.dev = netdev;
	dq_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	if (dq_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}

	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);


	subif_index = qos_hal_user_sub_interface_info + (qos_res->cqm_deq_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) + dp_subif->subif;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, " Sub interface index %p\n", subif_index);

	*q_num = subif_index->queue_cnt;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, " Number of Queues %d \n", *q_num);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);

	return QOS_HAL_STATUS_OK;
}

#if IS_ENABLED(CONFIG_X86_INTEL_LGM)
int32_t qos_mgr_get_mapped_queue(
	struct net_device *netdev,
	int32_t portid,
	uint32_t dir, /** 0 - Egress 1 - Ingress */
	struct qos_mgr_match_qid *qmap,
	int32_t (*qos_mgr_sess_del_cb)(uint32_t qid),
	int32_t flag)
{

	int ret = QOS_HAL_STATUS_OK;
	int i;
	int j;
	int queue_cnt_start_index = 0;
	int32_t mapped_qindex = -1;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};
	struct dp_qos_q_logic q_logic = {0};
	struct qos_hal_user_subif_abstract *base_subif_index = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	dp_subif_t *dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;

	MEM_ALLOC(dp_subif_t, dp_subif);
	if (netdev != NULL) {
		qos_mgr_br2684_get_vcc(netdev, &vcc);
		if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
	} else {
		dp_subif->port_id = portid;
		dp_subif->subif = 0;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Get Qid for Traffic class [%d] Netdev [%s]\n", __func__, qmap->class, netdev->name);

	dp_subif->subif = dp_subif->subif >> 8;

	dq_res.dev = netdev;
	dq_res.dp_port = dp_subif->port_id;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "%s:%d No Of Q: %d\n", __func__, __LINE__, dq_res.num_q);
	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_ATOMIC);
	if (dq_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);
	dq_res.q_res_size = dq_res.num_q;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

	base_subif_index = (qos_hal_user_sub_interface_info + qos_res->cqm_deq_port);
	qos_hal_get_subif_list_item(qos_res->cqm_deq_port,
			netdev,
			&subif_index,
			&base_subif_index->head);

	if ((dp_subif->alloc_flag & DP_F_FAST_WLAN) == DP_F_FAST_WLAN)
		queue_cnt_start_index = 0;
	else
		queue_cnt_start_index = 1;
	 QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "subif_index->default_q:%d subif_index->queue_cnt:%d queue_cnt_start_index:%d\n", subif_index->default_q, subif_index->queue_cnt, queue_cnt_start_index);
	if (subif_index->default_q != 0) {
		for (i = queue_cnt_start_index; i < subif_index->queue_cnt; i++) {
			for (j = 0; j < subif_index->user_queue[i].no_of_tc; j++) {
				QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif_index->user_queue[%d].qmap[%d]:%u qmap->class:%u\n",
						__func__, i, j, subif_index->user_queue[i].qmap[j], qmap->class);
				if (subif_index->user_queue[i].qmap[j] == qmap->class) {
					mapped_qindex = subif_index->user_queue[i].queue_index;
					QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Traffic class matched\n", __func__);
					goto found;
				}
			}
		}
found:
		if (mapped_qindex < 0) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Traffic class not found, Using Def Queue.\n", __func__);
			mapped_qindex = subif_index->default_q;
		}
		q_logic.q_id = qos_hal_queue_track[mapped_qindex].queue_id;
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> subif_index->user_queue[0].queue_index:[%u] qos_res->q_id[%u]\n",
				__func__, subif_index->user_queue[0].queue_index, qos_res->q_id);
		q_logic.q_id = dp_subif->def_qid;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Physical Queue Id is [%d]\n",
			__func__, q_logic.q_id);

	if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d Failed to Logical Queue Id\n",
				__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}

	ret = q_logic.q_logic_id;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Logical Queue Id is [%d] \n",
			__func__, ret);
	pSess_del_cb = qos_mgr_sess_del_cb;

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);

	return ret;
}
EXPORT_SYMBOL(qos_mgr_get_mapped_queue);
#endif

int32_t qos_mgr_set_mapped_queue(
		struct net_device *netdev,
		int32_t portid,
		int32_t deq_idx, /* Dequeue Index for PON netdevice*/
		uint32_t queue_id, /* Queue Id */
		uint32_t dir, /* 0 - Egress 1 - Ingress */
		uint32_t class, /* Traffic class/subif */
		int32_t flag)
{
	int32_t ret = QOS_HAL_STATUS_OK;
	int32_t index, mapped_qindex;
	uint32_t q_map_mask = 0;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};
	struct dp_queue_map_entry q_map = {0};
	dp_subif_t *dp_subif = NULL;
	QOS_MGR_VCC *vcc = NULL;
	struct qos_hal_user_subif_abstract *subif_index = NULL;
	struct qos_hal_user_subif_abstract *port_subif_index = NULL;

	MEM_ALLOC(dp_subif_t, dp_subif);
	dq_res.dev = netdev;
	if (netdev != NULL) {
		qos_mgr_br2684_get_vcc(netdev, &vcc);
		if ((flag & NETDEV_TYPE_TCONT) == NETDEV_TYPE_TCONT) {
			dq_res.dev = NULL;
			dp_subif->port_id = portid;
		} else {
			if (dp_get_netif_subifid(netdev, NULL, vcc, 0, dp_subif, 0) != QOS_MGR_SUCCESS) {
				kfree(dp_subif);
				return QOS_HAL_STATUS_ERR;
			}
		}
	} else {
		dp_subif->port_id = portid;
		dp_subif->subif = 0;
	}

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> netdev:[%s] deq_idx:[%d] class:[%d] queue_id:[%d]\n",
			__func__, netdev->name, deq_idx, class, queue_id);
	dq_res.dp_port = dp_subif->port_id;
	dq_res.cqm_deq_idx = deq_idx;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n",
				__func__, __LINE__);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "%s:%d No Of Q: %d\n",
			__func__, __LINE__, dq_res.num_q);
	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_ATOMIC);
	if (dq_res.q_res == NULL) {
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	memset(dq_res.q_res, 0, sizeof(struct dp_queue_res)*dq_res.num_q);
	dq_res.q_res_size = dq_res.num_q;
	if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> ERROR Failed to Get QOS Resources\n",
				__func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "%s:%d CQM Dequeue Port [%d] Lookup Mode: [%d]\n",
			__func__, __LINE__, qos_res->cqm_deq_port, dp_subif->lookup_mode);

	port_subif_index = qos_hal_user_sub_interface_info + qos_res->cqm_deq_port;
	qos_hal_get_subif_list_item(qos_res->cqm_deq_port, netdev, &subif_index, &port_subif_index->head);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, "base_subif_index: %p, base_subif_index->head:%p subif_index:%p\n",
			port_subif_index, port_subif_index->head, subif_index);

	QOS_HAL_DEBUG_MSG(QOS_DEBUG_HIGH, " Sub interface index %p\n", subif_index);

	/* Map to Drop queue if queue_id = 0 otherwise find the queue id from
	 * the user queue index.
	 * Right now Drop queue is hardcoded to 0. Once DP lib provides
	 * the interface it can be replaced with that.
	 */
	if (queue_id == 0) {
		mapped_qindex = 0;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Map to Drop Queue %d\n", mapped_qindex);
	} else {
		index = qos_hal_get_user_index_from_qid(subif_index, queue_id);
		if (index == QOS_HAL_STATUS_ERR) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "<%s>:<%d> Invalid Queue Id [%d]\n",
					__func__, __LINE__, queue_id);
			ret = QOS_HAL_STATUS_INVALID_QID;
			goto QOS_RESOURCES_CLEANUP;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> User Queue Index =%d \n", __func__, index);
		mapped_qindex = subif_index->user_queue[index].queue_index;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Mapped Queue Index %d\n",
			__func__, mapped_qindex);

	/** Configure the QMAP table to connect to the DP egress queue*/
	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	q_map.qmap.dp_port = dp_subif->port_id;

	q_map.qmap.subif = class;
	q_map.qmap.class = class;

	q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
		CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
		CBM_QUEUE_MAP_F_EN_DONTCARE |
		CBM_QUEUE_MAP_F_DE_DONTCARE |
		CBM_QUEUE_MAP_F_MPE1_DONTCARE |
		CBM_QUEUE_MAP_F_MPE2_DONTCARE;

	ret = qos_hal_add_q_map(mapped_qindex, 0, &q_map, q_map_mask);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);
	kfree(dp_subif);
	return ret;
}
EXPORT_SYMBOL(qos_mgr_set_mapped_queue);

/** ====================================== Directpath Connectivity ===============================  */

int qos_hal_dp_port_resources_get(
		uint32_t pmac_port,
		struct qos_hal_dp_res_info *res_p)
{
	*res_p = qos_hal_dp_res[pmac_port];
	return QOS_HAL_STATUS_OK;
}

int qos_hal_dp_egress_root_create(struct net_device *netdev)
{
	uint32_t old_omid, root_omid;
	struct qos_hal_sched_track_info Snext = {0};
	uint32_t no_of_qos_ports;
	struct dp_queue_res *qos_res = NULL;
	dp_subif_t *dp_subif = NULL;

	MEM_ALLOC(dp_subif_t, dp_subif);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Get Resources for Directpath Egress....\n", __func__);
	if (qos_hal_get_q_res_from_netdevice(netdev, netdev->name, 0, dp_subif, 1, &no_of_qos_ports, (void **)&qos_res, 0) ==
		QOS_HAL_STATUS_ERR) {
		kfree(qos_res);
		kfree(dp_subif);
		return QOS_HAL_STATUS_ERR;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> CPU QOS Port %d and Scheduler %d\n",
			__func__, qos_res->cqm_deq_port, qos_hal_port_track[qos_res->cqm_deq_port].input_sched_id);

	/* Create the WFQ Root Scheduler */
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Create WFQ Scheduler for Directpath Egress\n", __func__);

	old_omid = qos_hal_port_track[g_CPU_PortId].input_sched_id;
	Snext = qos_hal_sched_track[old_omid];
	root_omid = create_new_scheduler(old_omid, 0, QOS_HAL_POLICY_WSP, Snext.level+1, 1);
	qos_hal_sched_track[old_omid].next_sched_id = root_omid;
	qos_hal_sched_track[root_omid].policy = QOS_HAL_POLICY_WFQ;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>Configuring  Scheduler id of SB %d -->SB %d \n",
				__func__, root_omid, Snext.id);

	g_Root_sched_id_DpEgress = root_omid;
	g_DpEgressQueueScheduler++;
	kfree(qos_res);
	kfree(dp_subif);
	return QOS_HAL_STATUS_OK;

}

int qos_hal_setup_dp_egress_connectivity(
			struct net_device *netdev,
			uint32_t pmac_port)
{
	uint32_t port_sched, q_new, q_map_mask = 0;
	uint32_t dp_port, j = 0;
	enum qos_hal_errorcode eErr;
	struct dp_queue_map_entry q_map;
	struct qos_hal_user_subif_abstract *ingress_subif_index = NULL;

	if (pmac_port >= QOS_HAL_MAX_DP_RES)
		return -EINVAL;

	if (!g_DpEgressQueueScheduler) {
		if (qos_hal_dp_egress_root_create(netdev) == QOS_HAL_STATUS_ERR) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Failed to create DP Egress scheduler for netdev %s\n", __func__, netdev->name);
			return QOS_HAL_STATUS_ERR;
		}
	}

	dp_port = qos_hal_get_free_port();
	QOS_HAL_ASSERT(dp_port >= QOS_HAL_MAX_EGRESS_PORT);
	ingress_subif_index = qos_hal_user_sub_interface_ingress_info + (dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) ;

	/* Now create the Port scheduler for the interface */
	port_sched = create_new_scheduler(g_Root_sched_id_DpEgress, 0, QOS_HAL_POLICY_WFQ, qos_hal_sched_track[g_Root_sched_id_DpEgress].level + 1, 1);
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP Port scheduler is %d\n", __func__, port_sched);
	qos_hal_sched_track[port_sched].policy = QOS_HAL_POLICY_WSP;

	/* Add the Queue to the scheduler */
	q_new = qos_hal_add_new_queue(
			netdev,
			netdev->name,
			g_CPU_PortId,
			1,
			0,
			0,
			port_sched,
			ingress_subif_index,
			&eErr,
			0);

	/** Update the Egress Queue resource info */
	qos_hal_dp_res[pmac_port].netdev = netdev;
	qos_hal_dp_res[pmac_port].dp_egress_res.qidx = q_new;
	qos_hal_dp_res[pmac_port].dp_egress_res.dp_sched = port_sched;

	/** Configure the QMAP table to connect to the DP egress queue*/
	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	q_map.qmap.dp_port = pmac_port;
	q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
			CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
			CBM_QUEUE_MAP_F_EN_DONTCARE |
			CBM_QUEUE_MAP_F_DE_DONTCARE |
			CBM_QUEUE_MAP_F_MPE1_DONTCARE |
			CBM_QUEUE_MAP_F_MPE2_DONTCARE |
			CBM_QUEUE_MAP_F_TC_DONTCARE;
	qos_hal_add_q_map(q_new, g_CPU_PortId, &q_map, q_map_mask);

	/** This is a dummy QOS port. Only required for ingress sub interface index */
	qos_hal_dp_res[pmac_port].dp_egress_res.dp_port = dp_port;
	qos_hal_port_track[dp_port].is_enabled = 1;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> DP Port for pmac port %d is %d\n", __func__, pmac_port, qos_hal_dp_res[pmac_port].dp_egress_res.dp_port);

	qos_hal_port_track[dp_port].input_sched_id = port_sched;
	qos_hal_port_track[dp_port].shaper_sched_id = 0xFF;
	qos_hal_sched_track[port_sched].tbs = INVALID_SHAPER_ID;

	ingress_subif_index->is_logical = 0 ;
	ingress_subif_index->qos_port_idx = g_CPU_PortId;
	ingress_subif_index->base_sched_id_egress = port_sched;
	ingress_subif_index->user_queue[0].queue_index = q_new;
	ingress_subif_index->user_queue[0].queue_type = QOS_HAL_POLICY_WSP;
	ingress_subif_index->user_queue[0].qid = 0;
	ingress_subif_index->user_queue[0].prio_level = 9 ;
	ingress_subif_index->user_queue[0].sbin = port_sched << 3;
	ingress_subif_index->user_queue[0].qmap[0] = 0;
	ingress_subif_index->qid_last_assigned = 0;
	ingress_subif_index->default_q = q_new;
	ingress_subif_index->queue_cnt++;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> ingress_subif_index is %p Queue Count %d\n",
				__func__, ingress_subif_index, ingress_subif_index->queue_cnt);
	for (j = 1; j < QOS_HAL_MAX_SUB_IFID_PER_PORT; j++) {
		(ingress_subif_index + j)->is_logical = 1;
		(ingress_subif_index + j)->qos_port_idx = dp_port;
		(ingress_subif_index + j)->base_sched_id_egress = 0xFF;
		(ingress_subif_index + j)->port_sched_in_user_lvl = QOS_HAL_MAX_PRIORITY_LEVEL;
		(ingress_subif_index + j)->port_sched_in_egress = INVALID_SCHEDULER_INPUT_ID;
		(ingress_subif_index + j)->port_shaper_sched = INVALID_SCHED_ID;
		(ingress_subif_index + j)->shaper_idx = INVALID_SHAPER_ID;
		(ingress_subif_index + j)->default_q = 0;

	}


	return QOS_HAL_STATUS_OK;
}
EXPORT_SYMBOL(qos_hal_setup_dp_egress_connectivity);

int qos_hal_setup_dp_ingress_connectivity(
			struct net_device *netdev,
			uint32_t pmac_port)
{
	uint32_t q_new, q_map_mask = 0;
	int32_t ret = 0;
	enum qos_hal_errorcode eErr;
	struct dp_queue_map_entry q_map;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};
	dp_subif_t *dp_subif = NULL;

	if (pmac_port >= QOS_HAL_MAX_DP_RES)
		return QOS_HAL_STATUS_ERR;

	if (g_DpIngressQueueScheduler == 0xFF) {
		dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
		if (!dp_subif) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE,
				"<%s> DP subif allocation failed\n", __func__);
			return QOS_HAL_STATUS_ERR;
		}

		dp_get_netif_subifid(netdev, NULL, NULL, 0, dp_subif, 0);

		dq_res.dev = netdev;
		dq_res.dp_port = dp_subif->port_id;
		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		if (dq_res.num_q == 0) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
		if (dq_res.q_res == NULL) {
			kfree(dp_subif);
			return QOS_HAL_STATUS_ERR;
		}
		if (dp_deq_port_res_get(&dq_res, 0) != DP_SUCCESS) {
			QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
			kfree(dp_subif);
			ret = QOS_HAL_STATUS_ERR;
			goto QOS_RESOURCES_CLEANUP;
		}
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d subif: %d --> QOS Port: %d SB: %d Q: %d\n",
				__func__, dp_subif->port_id, dp_subif->subif, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

		g_DpIngressQueueScheduler = qos_res->sch_id[0];
		g_Port_id_DpIngress = qos_res->cqm_deq_port;
		g_Queue_id_DpIngress = qos_res->q_id;
		q_new = qos_res->q_id;
		qos_hal_sched_track[g_DpIngressQueueScheduler].policy = QOS_HAL_POLICY_WFQ;
		qos_hal_sched_track[g_DpIngressQueueScheduler].leaf_mask = 0x01;
		kfree(qos_res);
		kfree(dp_subif);
	} else {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s> Create Ingress Queue for Directpath Rx Port:%d ,SB:%d\n",
				__func__, g_Port_id_DpIngress, g_DpIngressQueueScheduler);
		q_new = qos_hal_add_new_queue(netdev, netdev->name, g_Port_id_DpIngress, 1, 0, 1, g_DpIngressQueueScheduler, NULL, &eErr, 0);
	}
	/** Update the Ingress Queue resource info */
	qos_hal_dp_res[pmac_port].q_ingress = q_new;

	/** Configure the QMAP table to connect to the ingress queue
	  FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:0 EP: 7-14 CLASS: XX
	 */
	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	q_map.qmap.dp_port = pmac_port;
	q_map.qmap.flowid = 0;
	q_map.qmap.enc = 1;
	q_map.qmap.dec = 1;
	q_map.qmap.mpe1 = 0;
	q_map.qmap.mpe2 = 0;
	q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE;
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "Q: %d\n", q_new);
	qos_hal_add_q_map(q_new, g_CPU_PortId, &q_map, q_map_mask);

	/** Configure the QMAP table to connect to the checksum queue
	  FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:1 EP: 7-14 CLASS: XX
	 */
	dq_res.dev = netdev;
	dq_res.dp_port = pmac_port;
	if (dp_deq_port_res_get(&dq_res, DP_F_CHECKSUM) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0)
		return QOS_HAL_STATUS_ERR;

	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
	if (dq_res.q_res == NULL)
		return QOS_HAL_STATUS_ERR;

	if (dp_deq_port_res_get(&dq_res, DP_F_CHECKSUM) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}
	QOS_HAL_DEBUG_MSG(QOS_DEBUG_TRACE, "<%s>For Port: %d --> QOS Port: %d SB: %d Q: %d\n",
			__func__, pmac_port, qos_res->qos_deq_port, qos_res->sch_id[0], qos_res->q_id);

	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	q_map.qmap.dp_port = pmac_port;
	q_map.qmap.flowid = 0;
	q_map.qmap.enc = 1;
	q_map.qmap.dec = 1;
	q_map.qmap.mpe2 = 1;
	q_map_mask |= CBM_QUEUE_MAP_F_MPE1_DONTCARE |
		CBM_QUEUE_MAP_F_TC_DONTCARE;

	qos_hal_add_q_map(g_Inst, qos_res->q_id, &q_map, q_map_mask);

	/** Added for Loopback port which is required for IPsec */
	if (!strcmp(netdev->name, "loopdev0")) {
		memset(&q_map, 0, sizeof(struct dp_queue_map_entry));
		q_map.qmap.dp_port = pmac_port;

		q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
			CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
			CBM_QUEUE_MAP_F_EN_DONTCARE |
			CBM_QUEUE_MAP_F_DE_DONTCARE |
			CBM_QUEUE_MAP_F_MPE1_DONTCARE |
			CBM_QUEUE_MAP_F_MPE2_DONTCARE |
			CBM_QUEUE_MAP_F_TC_DONTCARE;
		qos_hal_add_q_map(q_new, g_CPU_PortId, &q_map, q_map_mask);
	}

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);

	return QOS_HAL_STATUS_OK;
}
EXPORT_SYMBOL(qos_hal_setup_dp_ingress_connectivity);

int qos_hal_set_checksum_queue_map(uint32_t pmac_port)
{
	uint32_t q_map_mask = 0;
	int32_t ret = 0;
	struct dp_queue_map_entry q_map;
	struct dp_queue_res *qos_res = NULL;
	struct dp_dequeue_res dq_res = {0};

	if (pmac_port >= QOS_HAL_MAX_DP_RES)
		return QOS_HAL_STATUS_ERR;

	/** Configure the QMAP table to connect to the checksum queue
	  FLOW ID: 00 DEC:1 ENC:1 MPE1:0 MPE2:1 EP: 7-14 CLASS: XX
	 */
	dq_res.dp_port = pmac_port;
	if (dp_deq_port_res_get(&dq_res, DP_F_CHECKSUM) != DP_SUCCESS) {
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		return QOS_HAL_STATUS_ERR;
	}
	if (dq_res.num_q == 0)
		return QOS_HAL_STATUS_ERR;

	dq_res.q_res = qos_res = kmalloc(sizeof(struct dp_queue_res) * dq_res.num_q, GFP_KERNEL);
	if (dq_res.q_res == NULL)
		return QOS_HAL_STATUS_ERR;

	if (dp_deq_port_res_get(&dq_res, DP_F_CHECKSUM) != DP_SUCCESS) {
		ret = QOS_HAL_STATUS_ERR;
		QOS_HAL_DEBUG_MSG(QOS_DEBUG_ERR, "%s:%d ERROR Failed to Get QOS Resources\n", __func__, __LINE__);
		goto QOS_RESOURCES_CLEANUP;
	}

	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	q_map.qmap.dp_port = pmac_port;
	q_map.qmap.flowid = 0;
	q_map.qmap.enc = 1;
	q_map.qmap.dec = 1;
	q_map.qmap.mpe2 = 1;
	q_map_mask |= CBM_QUEUE_MAP_F_MPE1_DONTCARE |
		CBM_QUEUE_MAP_F_TC_DONTCARE;

	qos_hal_add_q_map(g_Inst, qos_res->q_id, &q_map, q_map_mask);

QOS_RESOURCES_CLEANUP:
	kfree(qos_res);

	return QOS_HAL_STATUS_OK;
}

int qos_hal_remove_dp_egress_connectivity(struct net_device *netdev, uint32_t pmac_port)
{
	int32_t q_map_mask = 0;
	struct dp_queue_map_entry q_map;
	struct qos_hal_dp_res_info res = {0};
	struct qos_hal_user_subif_abstract *ingress_subif_index = NULL;

	qos_hal_dp_port_resources_get(pmac_port, &res);

	/*Delete the Queue */
	qos_hal_safe_queue_delete(netdev, netdev->name, res.dp_egress_res.qidx, -1, -1, 0);

	/*Delete the Scheduler */
	qos_hal_scheduler_delete(res.dp_egress_res.dp_sched);


	/** Configure the QMAP table to connect to the ingress queue*/
	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	q_map.qmap.dp_port = pmac_port;
	q_map_mask |= CBM_QUEUE_MAP_F_FLOWID_L_DONTCARE |
			CBM_QUEUE_MAP_F_FLOWID_H_DONTCARE |
			CBM_QUEUE_MAP_F_EN_DONTCARE |
			CBM_QUEUE_MAP_F_DE_DONTCARE |
			CBM_QUEUE_MAP_F_MPE1_DONTCARE |
			CBM_QUEUE_MAP_F_MPE2_DONTCARE |
			CBM_QUEUE_MAP_F_TC_DONTCARE;

	/** Unmap the QMAP table for this egress queue*/
	qos_hal_del_q_map(res.dp_egress_res.qidx, g_CPU_PortId, &q_map, q_map_mask);

	qos_hal_add_q_map(g_CPU_Queue, g_CPU_PortId, &q_map, q_map_mask);

	ingress_subif_index = qos_hal_user_sub_interface_ingress_info + (res.dp_egress_res.dp_port * QOS_HAL_MAX_SUB_IFID_PER_PORT) ;
	ingress_subif_index->is_logical = 0 ;
	ingress_subif_index->qos_port_idx = 0xFF;
	ingress_subif_index->base_sched_id_egress = 0xFF;
	ingress_subif_index->user_queue[0].queue_index = 0xFF;
	ingress_subif_index->user_queue[0].queue_type = QOS_HAL_POLICY_NULL;
	ingress_subif_index->user_queue[0].qid = 0;
	ingress_subif_index->user_queue[0].prio_level = 0;
	ingress_subif_index->user_queue[0].sbin = INVALID_SCHEDULER_INPUT_ID;
	ingress_subif_index->default_q = 0;
	ingress_subif_index->qid_last_assigned = 0;
	ingress_subif_index->queue_cnt = 0;

	qos_hal_port_track[res.dp_egress_res.dp_port].is_enabled = 0;
	/** Update the Egress Queue resource info */
	qos_hal_dp_res[pmac_port].dp_egress_res.qidx = 0xFF;
	qos_hal_dp_res[pmac_port].dp_egress_res.dp_sched = 0xFF;
	qos_hal_dp_res[pmac_port].dp_egress_res.dp_port = 0xFF;

	return QOS_HAL_STATUS_OK;
}
EXPORT_SYMBOL(qos_hal_remove_dp_egress_connectivity);

int qos_hal_remove_dp_ingress_connectivity(struct net_device *netdev, uint32_t pmac_port)
{
	int32_t  q_map_mask = 0;
	struct qos_hal_dp_res_info res = {0};
	struct dp_queue_map_entry q_map;

	qos_hal_dp_port_resources_get(pmac_port, &res);

	/** Update the Ingress Queue resource info */
	qos_hal_dp_res[pmac_port].q_ingress = 0xFF;
	memset(&q_map, 0, sizeof(struct dp_queue_map_entry));

	q_map.qmap.dp_port = pmac_port;
	q_map.qmap.flowid = 0;
	q_map.qmap.enc = 1;
	q_map.qmap.dec = 1;
	q_map.qmap.mpe1 = 0;
	q_map.qmap.mpe2 = 0;
	q_map_mask |= CBM_QUEUE_MAP_F_TC_DONTCARE;

	/** Configure the QMAP table to connect to the ingress queue*/
	qos_hal_add_q_map(res.q_ingress, g_CPU_PortId, &q_map, q_map_mask);
	if (res.q_ingress != g_Queue_id_DpIngress) {
		qos_hal_egress_queue_delete(res.q_ingress);
	} else
		g_DpIngressQueueScheduler = 0xFF;

	return QOS_HAL_STATUS_OK;
}
EXPORT_SYMBOL(qos_hal_remove_dp_ingress_connectivity);

/** =========================  Initialize all the list ================= */
void qos_hal_buffers_init(void)
{
	int i;

	for (i = 0; i < QOS_HAL_MAX_EGRESS_PORT; i++) {
		qos_hal_port_track[i].is_enabled = 0;
		qos_hal_port_track[i].port_sched_id = 0xFF;
	}
	for (i = 0; i < QOS_HAL_MAX_SCHEDULER; i++) {
		qos_hal_sched_track[i].in_use = 0;
		qos_hal_sched_track[i].omid = INVALID_SCHED_ID;
		qos_hal_sched_track[i].tbs = INVALID_SHAPER_ID;
	}
	for (i = 0; i < QOS_HAL_MAX_EGRESS_QUEUES; i++) {
		qos_hal_queue_track[i].is_enabled = 0;
		qos_hal_queue_track[i].tb_index = INVALID_SHAPER_ID;
	}

}

int32_t __qos_hal_spl_conn_init(uint8_t conn_type, uint32_t queue_len)
{
	int ret = QOS_MGR_SUCCESS;
	struct dp_spl_cfg conn={0};
	struct dp_qos_q_logic q_logic = {0};
	QOS_Q_MOD_CFG param;

	switch (conn_type) {
		case DP_SPL_TOE: 	/* Set LRO queue length to 4096 */
			/*Get spl conn parameteres 0 means failure*/
			if (dp_spl_conn_get(0, conn_type, &conn, 1) != 0) {
				/*Egress port qid*/;
				q_logic.q_id = conn.egp[0].qid;

				/* physical to logical qid */
				if (dp_qos_get_q_logic(&q_logic, 0) == DP_FAILURE) {
					printk("%s:%d ERROR Failed to Logical Queue Id\n", __func__, __LINE__);
					return QOS_MGR_FAILURE;
				}

				printk(KERN_INFO"%s %d q_logic.q_id=%d q_logic.q_logic_id =%d\n", __FUNCTION__, __LINE__, q_logic.q_id, q_logic.q_logic_id);

				memset(&param, 0, sizeof(QOS_Q_MOD_CFG));
				param.q_id = q_logic.q_id;
				param.qlen = queue_len;
				param.drop.mode = QOS_MGR_DROP_TAIL;
				qos_hal_modify_queue(NULL, &param);
			} else {
				printk("%s:%d ERROR dp_spl_conn_get() failed.\n", __func__, __LINE__);
			}
			break;
	}

	return ret;
}

static int32_t qos_hal_spl_conn_init(void) {
	printk("QOS HAL Special connection Init.\n");

	__qos_hal_spl_conn_init(DP_SPL_TOE, QOS_HAL_QWRED_MAX_ALLOWED_LRO);

	return QOS_MGR_SUCCESS;
}

int32_t qos_hal_init(void) {
	printk("QOS HAL Init.\n");
	g_Inst = 0;
	qos_hal_buffers_init();
	if (qos_hal_ll_init() == QOS_HAL_STATUS_ERR) {
		pr_debug("Low level Init failed !!!!\n");
		return QOS_MGR_FAILURE;
	}

	qos_hal_proc_create();

	if (qos_hal_spl_conn_init() != QOS_MGR_SUCCESS) {
		pr_err("QoS HAL Special connection init failed.\n");
	}

	return QOS_MGR_SUCCESS;
}

int32_t qos_hal_exit(void) {
	qos_hal_proc_destroy();
	kfree(qos_hal_user_sub_interface_info);
	kfree(qos_hal_user_sub_interface_ingress_info);
	return QOS_MGR_SUCCESS;
}
