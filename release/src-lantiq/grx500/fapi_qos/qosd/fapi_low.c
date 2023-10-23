/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "qosal_inc.h"
#include "qosal_utils.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

#define SHOW_IF_Q_DETAILS(if_list_head, ingress) { \
		struct list_head *pxIfListMac = NULL; \
		qos_if_node_t *pxIfListNode = NULL; \
		qos_queue_node_t *pxQListNode = NULL; \
		int32_t nKey = 0; \
		LOGF_LOG_INFO("############## Interface and %s queue list\n", (ingress == 1)?"ingress":"egress"); \
		nKey = 0; \
		list_for_each(pxIfListMac, if_list_head){ \
			pxIfListNode = list_entry(pxIfListMac, qos_if_node_t, list); \
			LOGF_LOG_INFO("Interface name [%s]\n", pxIfListNode->ifname); \
			if (pxIfListNode->q_list_head != NULL ) { \
				list_for_each(q_list_tmp, pxIfListNode->q_list_head) { \
					pxQListNode = list_entry(q_list_tmp, qos_queue_node_t, list); \
					LOGF_LOG_INFO("\tQ [%d] : name:id:prio [%s:%d:%d]\n", nKey++, pxQListNode->queue_cfg.name, pxQListNode->queue_cfg.queue_id, pxQListNode->queue_cfg.priority); \
				} \
			} \
		} \
	}

extern es_tables_t es_tables;
extern uint16_t LOGTYPE, LOGLEVEL;

static char iftype_str[QOS_IF_CATEGORY_MAX][MAX_NAME_LEN] = {"ETHLAN", "ETHWAN", "PTMWAN", "ATMWAN", "LTEWAN", "WLANDP", "WLANNDP", "LOCAL", "CPU"};

/* =============================================================================
 *  Function Name : qosd_cfg_set
 *  Description   : Low FAPI to set the QoS configuration
 *  Input Params  : 1. Pointer to QoS cfg
 *                  2. flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t qosd_cfg_set(IN qos_cfg_t *qoscfg)
{
	ENTRY
	int32_t ret = LTQ_FAIL;
	uint8_t dos_qos_status = QOSAL_DISABLE;

	if (es_tables.dos_qos_ena == QOSAL_ENABLE)
		dos_qos_status = QOSAL_ENABLE;

	if(es_tables.qos_enable == QOSAL_DISABLE && qoscfg->ena == QOSAL_ENABLE) {
		/* call es_init */
		es_init(qoscfg);
		es_tables.dos_qos_ena = dos_qos_status;
	}
	if((es_tables.wmm_ena == QOSAL_DISABLE && qoscfg->wmm_ena == QOSAL_ENABLE) ||
		(es_tables.wmm_ena == QOSAL_ENABLE && qoscfg->wmm_ena == QOSAL_DISABLE)) {
		/* Default config update */
		ret = default_qos_cfg(qoscfg->wmm_ena);
		if (ret == LTQ_SUCCESS) {
			es_tables.wmm_ena = qoscfg->wmm_ena;
		}
	}
	if(es_tables.qos_enable == QOSAL_ENABLE && qoscfg->ena == QOSAL_DISABLE) {
		/* call es_shut */
		es_shut();
	}

	es_log_init(qoscfg);

	if(es_tables.qos_enable == QOSAL_DISABLE) {
		es_tables.wmm_ena = QOSAL_DISABLE;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : default_qos_cfg
 *  Description   : API to configure default QoS config in supported engines
 *  Input Params  : Configuration enable/disable flag
 *  Return value  : LTQ_SUCCESS(0)
 * ============================================================================*/
int32_t default_qos_cfg(int32_t nEna)
{
	ENTRY

	int32_t engine = 0, ret = LTQ_SUCCESS;

	for(engine=0; engine<es_tables.number_of_engines; engine++) {
		if(es_tables.engine_info[es_tables.eng_ids[engine] - 1].status == 1) {
			if (es_tables.engine_info[es_tables.eng_ids[engine] - 1].engine.engine_capab.q_cap & Q_CAP_DOS) {
				if (nEna == QOSAL_ENABLE && es_tables.dos_qos_ena == QOSAL_DISABLE) {
					QOSD_ENGINE_INIT(engine, QOS_MODULE_DOS)
					es_tables.dos_qos_ena = QOSAL_ENABLE;
				}
			}
			if(es_tables.engine_info[es_tables.eng_ids[engine] - 1].engine.engine_capab.q_cap & Q_CAP_WMM) {
				if(nEna == QOSAL_ENABLE) {
					QOSD_ENGINE_INIT(engine, QOS_MODULE_WMM)
				}
				else if(nEna == QOSAL_DISABLE) {
					QOSD_ENGINE_FINI(engine, QOS_MODULE_WMM)
				}
			}
		}
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_cfg_get
 *  Description   : Low FAPI to get the QoS configuration
 *  Input Params  : 1. Pointer to QoS cfg
 *                  2. flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t qosd_cfg_get(OUT qos_cfg_t *qoscfg, IN uint32_t flags)
{
	ENTRY
	if(flags & QOS_OP_CFG_DUMP) {
		/* dump context stored within ES */
		qosd_show_cfg(&es_tables);
	}
	else {
		memset_s(qoscfg, sizeof(qos_cfg_t), 0);
		qoscfg->ena = es_tables.qos_enable;
		qoscfg->log_level = es_tables.log_level;
		qoscfg->log_type = log_type;
		qoscfg->wmm_ena = es_tables.wmm_ena;
		if(log_type == QOS_LOG_FILE)
			if(strncpy_s(qoscfg->log_file, MAX_NAME_LEN, log_file, MAX_NAME_LEN) != EOK) {
				LOGF_LOG_ERROR("Error copying log file name string\n");
				return LTQ_FAIL;
			}
		if(strncpy_s(qoscfg->version, MAX_NAME_LEN, es_tables.version, strnlen_s(es_tables.version, MAX_NAME_LEN)) != EOK) {
			LOGF_LOG_ERROR("Error copying version string\n");
			return LTQ_FAIL;
		}
	}
	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_engine_fini
 *  Description   : Internal API to un-initialize engine
 *  Input Params  : 1. Engine ID
 *									2. module type, queue/class/both
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_engine_fini(IN qos_engines_t engine, IN qos_module_type_t  module)
{
	struct list_head *if_list_tmp = NULL;
	qos_if_node_t *if_list_node = NULL;
	uint32_t engine_q_usage_count = 0;
	int32_t ret = LTQ_FAIL;
	ENTRY


	if(engine >= QOS_ENGINE_MAX) {
		LOGF_LOG_ERROR("Engine selected [%d] for in qosd_engine_fini for un-initializing is not valid\n", engine);
		return ret;
	}
	/* does it make sense to put below code inside a function/macro? */
	/* Count the egress interfaces using this engine for queueing/shaping */
	if (es_tables.egress_if_list_head != NULL ) {
		list_for_each(if_list_tmp, es_tables.egress_if_list_head){
			if_list_node = list_entry(if_list_tmp, qos_if_node_t, list);
			/* If engine is used for port shaping */
			if (if_list_node->common_q_point == engine && if_list_node->port_shaper != NULL ) {
				engine_q_usage_count ++;
				break;
			}
			/* If engine is used for queueing */
			if (if_list_node->q_count != 0 ) {
				engine_q_usage_count ++;
				break;
			}
		}
	}

	/* Count the ingress interfaces using this engine for queueing/shaping */
	if (es_tables.ingress_if_list_head != NULL ) {
		list_for_each(if_list_tmp, es_tables.ingress_if_list_head){
			if_list_node = list_entry(if_list_tmp, qos_if_node_t, list);
			/* If engine is used for port shaping */
			if (if_list_node->common_q_point == engine && if_list_node->port_shaper != NULL ) {
				engine_q_usage_count ++;
				break;
			}
			/* If engine is used for queueing/classification */
			if (if_list_node->q_count != 0) {
				engine_q_usage_count++;
				break;
			}
		}
	}

	if((engine_q_usage_count == 0)
		&& (es_tables.engine_info[engine].q_initialized == 1)
		&& (module == QOS_MODULE_QUEUE || module == QOS_MODULE_BOTH)) {
		/* no ingress/egress queues are configured on 'engine'. */
		QOSD_ENGINE_FINI(engine, module)
		/* Reset the q initialized flag for the engine. */
		es_tables.engine_info[engine].q_initialized = 0;
	}
	if((es_tables.engine_info[engine].rules_count == 0)
		&& (es_tables.engine_info[engine].cl_initialized == 1)
		&& (module == QOS_MODULE_CLASSIFIER || module == QOS_MODULE_BOTH)) {
		/* no classifiers are configured on 'engine'. */
		QOSD_ENGINE_FINI(engine, module)
		/* Reset the classifier initialized flag for the engine. */
		es_tables.engine_info[engine].cl_initialized = 0;
	}

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_engine_init
 *  Description   : Internal API to initialize engine
 *  Input Params  : 1. Engine ID
 *									2. module type, queue/class/both
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_engine_init(IN qos_engines_t engine, IN qos_module_type_t module)
{
	int32_t ret = LTQ_FAIL;
	ENTRY
	if(engine >= QOS_ENGINE_MAX) {
		LOGF_LOG_ERROR("Engine selected [%d] in qosd_engine_init for initialization is not valid\n", engine);
		return ret;
	}
	if ((module == QOS_MODULE_QUEUE || module == QOS_MODULE_BOTH )
				&& es_tables.engine_info[engine].q_initialized == 0) {
		LOGF_LOG_DEBUG("Initializing engine [%s]\n", es_tables.eng_names[engine]);
		QOSD_ENGINE_INIT(engine, QOS_MODULE_QUEUE)

		// Set the q initialized flag for the engine
		es_tables.engine_info[engine].q_initialized = 1;
	}

	if ((module == QOS_MODULE_CLASSIFIER || module == QOS_MODULE_BOTH )
				&& es_tables.engine_info[engine].cl_initialized == 0) {
		LOGF_LOG_DEBUG("Initializing engine [%s]\n", es_tables.eng_names[engine]);
		QOSD_ENGINE_INIT(engine, QOS_MODULE_CLASSIFIER)

		// Set the classifier initialized flag for the engine
		es_tables.engine_info[engine].cl_initialized = 1;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_get_common_point
 *  Description   : Internal API to calculate common queue point from queue points
 *									of individual queues
 *  Input Params  : 1. Pointer to Interface node in the linked list for interface info
 *  Output Params : 1. Pointer to common queue point
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_get_common_point(IN qos_if_node_t *if_list_node, IN_OUT qos_engines_t *common_q_point)
{
	struct list_head *list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;
	qos_resource_t resource = QOS_RESOURCE_MAX;

	ENTRY
	list_for_each(list_node, if_list_node->q_list_head){
		q_list_node = list_entry(list_node, qos_queue_node_t, list);
		/* Considered enabled node only */
		if (q_list_node->q_point < QOS_ENGINE_MAX ) {
			if (q_list_node->enable == QOSAL_ENABLE) {
				/* get lowest enum from qos_resource_t */
				LOGF_LOG_DEBUG("Queuing point for queue %s is %d\n", q_list_node->queue_cfg.name, q_list_node->q_point);
				if (es_tables.engine_info[q_list_node->q_point].engine.used_resource < resource) {
					resource = es_tables.engine_info[q_list_node->q_point].engine.used_resource;
					*common_q_point = q_list_node->q_point;

					/* break if we reached CPU, the lowest weight recource */
					if (resource == QOS_RESOURCE_CPU )
						goto exit;
				}
			}
		}
	}

	if ( (if_list_node->port_shaper != NULL && if_list_node->port_shaper->shaping_point < QOS_ENGINE_MAX)
				&& (es_tables.engine_info[if_list_node->port_shaper->shaping_point].engine.used_resource
				> es_tables.engine_info[*common_q_point].engine.used_resource ))
		*common_q_point = if_list_node->port_shaper->shaping_point;

	LOGF_LOG_DEBUG("New queuing common point is %d\n",*common_q_point);
exit:
	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_get_dps_to_disable
 *  Description   : Internal API to find out datapaths to be disabled based on 
 *									given engine
 *  Input Params  : 1. Interface Category
 *                : 2. Engine Id
 *                : 3. Ingress flag
 *  Output Params : 4. Array of datapaths to disable
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t 
qosd_get_dps_to_disable(IN iftype_t iftype, IN qos_engines_t engine, IN int32_t ingress, IN_OUT int32_t dps_to_disable[])
{
	int32_t dps[MAX_SYS_DATAPATHS] = {0};
	int32_t i = 0;

	ENTRY
	if (ingress)
		qosd_get_dps_for_iface(&es_tables, iftype, QOS_IF_CATEGORY_MAX, dps, QOS_DATAPATH_MAX);
		//qosd_get_dps_for_iface(&es_tables, QOS_IF_CATEGORY_MAX, iftype, dps, QOS_DATAPATH_MAX);
	else 
		qosd_get_dps_for_iface(&es_tables, QOS_IF_CATEGORY_MAX, iftype, dps, QOS_DATAPATH_MAX);

	LOGF_LOG_DEBUG ("All datapaths for interface caterogy [%d] are -->\n", iftype);
	QOSD_SHOW_DATAPATHS(dps, i);

	LOGF_LOG_DEBUG("Identify datapaths that doesn't have engine [%d]\n", engine);
	qosd_get_dps_wo_given_engine(dps, engine, ingress, dps_to_disable);
	LOGF_LOG_DEBUG ("datapaths to disable for engine %d:%s of action -->\n", engine, es_tables.eng_names[engine]);
	QOSD_SHOW_DATAPATHS(dps_to_disable, i);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_get_queueing_point
 *  Description   : Internal API to get queueing/shaping point
 * ============================================================================*/
static qos_engines_t 
qosd_get_queueing_point(IN iftype_t iftype, IN uint32_t q_count, IN uint32_t ingress, IN void *config)
{
	int32_t dps[MAX_SYS_DATAPATHS] = {0};
	uint32_t i = 0;
	qos_capab_cfg_t needed_capab = {0};
	ENTRY
	
	needed_capab.module = QOS_MODULE_QUEUE;
	needed_capab.capab_cfg.q_capab.q_count = q_count;
	needed_capab.capab_cfg.q_capab.q_point = QOS_ENGINE_MAX;

	/* Since Egress towards LO/CPU is termend as Ingress for overall system */
	if (ingress && iftype != QOS_IF_CATEGORY_LOCAL) {
		qosd_get_dps_for_iface(&es_tables, iftype, QOS_IF_CATEGORY_MAX, dps, QOS_DATAPATH_SLOW);
	}
	else {
		qosd_get_dps_for_iface(&es_tables, QOS_IF_CATEGORY_MAX, iftype, dps, QOS_DATAPATH_SLOW);
	}

	LOGF_LOG_DEBUG ("Slow datapaths for interface category [%d] are -->\n", iftype);
	QOSD_SHOW_DATAPATHS(dps, i);

	if (dps[0] == 0){
		LOGF_LOG_DEBUG ("No slow paths are available for this interface\n");
		goto exit;
	}

	if (q_count == 0 )
		qosd_prepare_needed_capab_port((qos_shaper_t *)config, iftype, &needed_capab.capab_cfg.q_capab.q_cap);
	else
		qosd_prepare_needed_capab((qos_queue_cfg_t *)config, iftype, &needed_capab.capab_cfg.q_capab.q_cap);

	qosd_get_action_point(dps, &needed_capab, ingress, 0);

exit:
	EXIT
	return needed_capab.capab_cfg.q_capab.q_point;
}
/* =============================================================================
 *  Function Name : qosd_update_datapaths_status
 *  Description   : Internal API to update disabled datapaths, it checks list os disabled
 *									datapaths for old engine, and datapaths to be disabled for new engine
 *  Input Params  : 1. Interface Category
 *                : 2. Ingress flag
 *                : 3. Old Engine Id
 *                : 4. New Engine Id
 *  Output Params : 4. Array of datapaths to disable
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t 
qosd_update_datapaths_status(IN char *ifname, IN int32_t ingress, IN qos_engines_t from, IN qos_engines_t to)
{
	ENTRY
	int32_t disabled_datapaths[MAX_SYS_DATAPATHS] = {0};
	int32_t datapaths_to_disable[MAX_SYS_DATAPATHS] = {0};
	int32_t datapaths[MAX_SYS_DATAPATHS] = {0};
	int32_t i = 0;
	int32_t ret = LTQ_SUCCESS;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
    QOSD_GET_IFTYPE(ifname, es_tables.ifmap);
 	
	/* Get already disabled path for previous queue point. TODO: should be stored in interface node*/
	if ( from != QOS_ENGINE_MAX )
		qosd_get_dps_to_disable(iftype, from, ingress, disabled_datapaths);

	/* Get datapaths needs to be disabled for new queue point */
	if ( to != QOS_ENGINE_MAX )
		qosd_get_dps_to_disable(iftype, to, ingress, datapaths_to_disable);

	/* with present platforms (eg GRX330), below conditions can be used to optimise (either disable or enable disabled datapaths)
	   1. If we move from higher weight engine to lower weight engine, we would need to disable some of the datapaths,
	   2. If we move from lower weight engine to higher weight engine, we would need to enable some of the disabled datapaths
		 but above conditions may not hold true for newer platforms, so, let's keep it generic. */

	/* Check if there are some datapaths to be disabled while moving "from" -> "to" */
	if (disabled_datapaths[0]) {
		qosd_filter_common_dps(disabled_datapaths, datapaths_to_disable, datapaths);
		LOGF_LOG_DEBUG ("datapaths to enable are -->");
		QOSD_SHOW_DATAPATHS(datapaths, i);
		if (datapaths[0]) 
			qosd_enable_datapaths(ifname, iftype, datapaths);
	}

	/* Check if there are some datapaths to be enabled while moving "from" -> "to" */
	if (datapaths_to_disable[0]) {
		memset_s(datapaths, sizeof(datapaths), 0);
		qosd_filter_common_dps(datapaths_to_disable, disabled_datapaths, datapaths);
		LOGF_LOG_DEBUG ("datapaths to disable are -->");
		QOSD_SHOW_DATAPATHS(datapaths, i);
		if (datapaths[0]) 
			qosd_disable_datapaths(ifname, iftype, datapaths);
	}
exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_is_queue_unique
 *  Description   : Internal API to check if queue isunique
 * ============================================================================*/
static int32_t qosd_is_queue_unique(IN qos_if_node_t *if_list_node, IN qos_queue_cfg_t *q)
{
	struct list_head *list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;
	int32_t i = 0, j = 0;
	bool tc_value = false;

	ENTRY
	if (if_list_node != NULL && if_list_node->q_count > 0) {
		list_for_each(list_node, if_list_node->q_list_head) {
			q_list_node = list_entry(list_node, qos_queue_node_t, list);
			if (q->sched == QOS_SCHED_SP && q_list_node->queue_cfg.priority == q->priority)
				return QOS_Q_PRIO_CLASH;
			if (!strcmp(q_list_node->queue_cfg.name, q->name))
			{
				if(((q->flags & QOS_Q_F_INGRESS) && (q_list_node->queue_cfg.inggrp == q->inggrp)) || !(q->flags & QOS_Q_F_INGRESS))
				return QOS_ENTRY_EXISTS;
			}

			/* Check for TC being used by some other Queue */
			for (i = 0; i <  MAX_TC_NUM; i++) {
				if (q->tc_map[i] != 0) {
					tc_value = true;
					j = 0;
					while ((j < MAX_TC_NUM) && (q_list_node->queue_cfg.tc_map[j])) {
						LOGF_LOG_DEBUG("Checking q_list_tc:[%d] with q_tc:[%d]\n", q_list_node->queue_cfg.tc_map[j], q->tc_map[i]);
						if((uint32_t)(q_list_node->queue_cfg.tc_map[j]) == (uint32_t)(q->tc_map[i]))
						{
							LOGF_LOG_ERROR("Traffic Class:[%d] already being used by some other queue.\n", q_list_node->queue_cfg.tc_map[j]);
							return QOS_Q_TC_CLASH;
						}
						j++;
					}
				}
			}
			if(!tc_value)
				return QOS_Q_TC_ZERO_INVALID;
		}
	}

	EXIT
	return LTQ_SUCCESS;
}


/* =============================================================================
 *  Function Name : qosd_move_queues
 *  Description   : Internal API to move queues from one engine to others
 *  Input Params  : 1. Pointer to Interface node in the linked list for interface info
 *                  2. Engine Index from where queues to be deleted
 *                  3. Engine Index to where queues to be added
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t 
qosd_move_queues(IN qos_if_node_t *if_list_node, IN qos_engines_t from, IN qos_engines_t to)
{
	struct list_head *list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;
	int32_t ret = LTQ_SUCCESS;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;

	ENTRY

	if (from == QOS_ENGINE_MAX || to == QOS_ENGINE_MAX) {
		LOGF_LOG_ERROR("Invalid input of to/from [%d:%d] engine, can not move queues\n", to, from);
		ret = LTQ_FAIL;
		goto exit;
	}

	QOSD_VALIDATE_IF_NODE(if_list_node);
	QOSD_GET_IFTYPE(if_list_node->ifname, es_tables.ifmap);
	if (if_list_node->ingress)
		QOSD_GET_IFINGGRP(if_list_node->ifname, es_tables.ifinggrp_map); 
	/* Remove Queues from old engine, "from" */
	list_for_each(list_node, if_list_node->q_list_head){
		q_list_node = list_entry(list_node, qos_queue_node_t, list);
		if (q_list_node->enable == QOSAL_ENABLE && q_list_node->queue_cfg.sched == QOS_SCHED_WFQ
				&& (if_list_node->sum_wts != 0)) {
			q_list_node->queue_cfg.flags |= QOS_Q_F_WFQ_UPDATE;
		}
		QOSD_ENGINE_QUEUE_DELETE(from, if_list_node->ifname, iftype, ifinggrp, &q_list_node->queue_cfg);
		q_list_node->enable = QOSAL_DISABLE;
	}

	/* Un-initialize old engine and initialize new engine */
	if (from != to ) {
		qosd_engine_fini(from, QOS_MODULE_QUEUE);
		qosd_engine_init(to, QOS_MODULE_QUEUE);
	}

	/* Add Queues to old engine, "to" */
	list_for_each(list_node, if_list_node->q_list_head){
		q_list_node = list_entry(list_node, qos_queue_node_t, list);
		q_list_node->enable = QOSAL_ENABLE;
		if (q_list_node->enable == QOSAL_ENABLE && q_list_node->queue_cfg.sched == QOS_SCHED_WFQ
				&& (if_list_node->sum_wts != 0)) {
			q_list_node->queue_cfg.flags |= QOS_Q_F_WFQ_UPDATE;
		}

		QOSD_ENGINE_QUEUE_ADD(to, if_list_node->ifname, iftype, ifinggrp, &q_list_node->queue_cfg);

		if (q_list_node->enable == QOSAL_ENABLE && q_list_node->queue_cfg.sched == QOS_SCHED_WFQ
				&& (if_list_node->sum_wts != 0)) {
			q_list_node->queue_cfg.flags &= ~QOS_Q_F_WFQ_UPDATE;
		}
	}

	/* List of datapaths need to be disabled might change after move operation, Let's update it */
	if (from != to )
		qosd_update_datapaths_status(if_list_node->ifname, if_list_node->ingress, from, to);

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_queue_add
 *  Description   : Internal API to add queues on the interface
 *  Input Params  : 1. Interface name
 *                  2. Queue Properties
 *                  3. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_queue_add(IN char *ifname,
		IN qos_queue_cfg_t	*q,
		IN uint32_t flags)
{
	int32_t ingress = 0, ret = LTQ_SUCCESS;
	uint32_t orig_wt = 0;
	qos_engines_t engine = QOS_ENGINE_MAX, q_point = QOS_ENGINE_MAX;
	qos_if_node_t *if_list_node = NULL;
	struct list_head *q_list_tmp = NULL, **if_list_head = NULL;
	qos_queue_node_t *q_list_node = NULL;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	char base_if[MAX_IF_NAME_LEN] = {0}, logical_if[MAX_IF_NAME_LEN] = {0};

	ENTRY
	QOSD_GET_IFTYPE(ifname, es_tables.ifmap);

	if (flags & QOS_OP_F_INGRESS) {
		QOSD_GET_IFINGGRP(ifname, es_tables.ifinggrp_map);
		ingress = 1;
		if_list_head = &es_tables.ingress_if_list_head;
		LOGF_LOG_DEBUG ("Configuring ingress queue %s for ingress group %d\n", q->name, ifinggrp);
	}
	else {
		if_list_head = &es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Configuring egress queue %s for interface category %d\n", q->name, iftype);
	}

	if((iftype == QOS_IF_CATEGORY_ETHWAN) || (iftype == QOS_IF_CATEGORY_PTMWAN) || (iftype == QOS_IF_CATEGORY_ATMWAN)) {
		if (qosd_if_base_get(ifname,base_if,logical_if,0) == LTQ_FAIL ) {
			LOGF_LOG_ERROR("Base interface is not provided for [%s]\n", ifname);
			ret = QOS_IFMAP_LOOKUP_FAIL;
			goto exit;
		}
	}

	if_list_node = qosd_get_if_node(*if_list_head, ifname);
	if (if_list_node == NULL) {
		ret = qosd_add_if_node(if_list_head, ifname, &if_list_node);
		if(ret != LTQ_SUCCESS ) {
			LOGF_LOG_ERROR("Adding interface node for [%s] failed", ifname);
			ret = QOS_IF_ADD_FAIL;
			goto exit;
		}
		if(strncpy_s(if_list_node->ifname, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN) != EOK) {
			LOGF_LOG_ERROR("Error copying ifname string\n");
			ret = QOS_IF_ADD_FAIL;
                        goto exit;
		}
		if_list_node->common_q_point = engine;
		if_list_node->ingress = ingress;
		es_tables.act_if_cnt++;
	}

	if ((if_list_node->q_count == 0) && (!(q->flags & QOS_Q_F_DEFAULT))) {
		LOGF_LOG_ERROR("Default queue should be added first for an interface\n");
		ret = QOS_DEF_Q_DEP_IN_ADD_FAIL;
		goto exit;
	}

	ret = qosd_is_queue_unique(if_list_node, q);
	if(ret != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Queue [name: %s, Priority: %d] is not unique\n",q->name,q->priority);
		goto exit;
	}

	engine = qosd_get_queueing_point(iftype, if_list_node->q_count + 1, ingress, (void*)q);
	if (engine >= QOS_ENGINE_MAX){
		LOGF_LOG_ERROR("Queue add failed. Queuing common point is not found\n");
		ret = LTQ_FAIL;
		goto exit;
	}
	q_point = engine;

	/* Assign queue id for this queue, no need to have check for overflowing of counter. */
	q->queue_id = ++(es_tables.q_counter);

	if (q->sched == QOS_SCHED_WFQ) {
		orig_wt = q->weight;
		if (q->shaper.cir != 0)
			if_list_node->sum_cirs += q->shaper.cir;
		else
			if_list_node->sum_wts += orig_wt; /* If add fails, we have undo this update */
	}
	if (q_wt_n_rate_derive(iftype, if_list_node, q) != LTQ_SUCCESS) {
		ret = LTQ_FAIL;
		goto exit;
	}
	if (if_list_node->q_count > 0) {
		/* CPU doent need to delete and add all queues if any q is configured with cir, if other engines need we can remove "q->shaper.cir == 0" from below check */
		if (es_tables.engine_info[if_list_node->common_q_point].engine.used_resource > es_tables.engine_info[engine].engine.used_resource
					|| (q->sched == QOS_SCHED_WFQ)) {
		
			if (es_tables.engine_info[if_list_node->common_q_point].engine.used_resource < es_tables.engine_info[engine].engine.used_resource)
				engine = if_list_node->common_q_point;

			/* if previous common point is at higher weight or more preferable (like hw) than current one, 
				 then move previous common point, delete all queues from prev point and add at new one OR
				 q to be add is wfq, we need to modify weights of existing configured queues*/

			LOGF_LOG_DEBUG ("Moving queue point from %s to %s\n",es_tables.eng_names[if_list_node->common_q_point], es_tables.eng_names[engine]);
			qosd_move_queues(if_list_node, if_list_node->common_q_point, engine);
			if_list_node->common_q_point = engine;
		}
		else if (es_tables.engine_info[if_list_node->common_q_point].engine.used_resource < es_tables.engine_info[engine].engine.used_resource){
			/* if previous common point is at lower weight (like cpu) than current one, 
				 then add present queue on same point, no need to move previous common point */
			engine = if_list_node->common_q_point;
		}
	}
	else {
		/* first queue add. consider this queue's engine as common_q_point for queue's interface */
		if_list_node->common_q_point = engine;
		qosd_engine_init(engine, QOS_MODULE_QUEUE);
	}

	//add just current queue
	QOSD_ENGINE_QUEUE_ADD(engine, ifname, iftype, ifinggrp, q);
	q_list_node = qosd_add_q_node(&if_list_node->q_list_head);
	if(q_list_node == NULL) {
		LOGF_LOG_ERROR("Failed to add queue to list\n");
		return LTQ_FAIL;
	}

	q_list_node->q_point = q_point;
	q_list_node->enable = QOSAL_ENABLE;
	memcpy_s((void *)&q_list_node->queue_cfg, sizeof(qos_queue_cfg_t), (void *)q, sizeof(qos_queue_cfg_t));
	if_list_node->q_count++;

	if (q->sched == QOS_SCHED_WFQ && q->shaper.cir == 0) {
		q_list_node->orig_wt = orig_wt;
	}

	/* identify datapaths to be disabled if adding first queue else it would be taken care while moving queues */
	if (if_list_node->q_count == 1 && ((q->flags & QOS_Q_F_INT_Q) == 0))
		qosd_update_datapaths_status(ifname, ingress, QOS_ENGINE_MAX, engine);

	LOGF_LOG_INFO("Successfully added queue [%s] on interface [%s]..\n", q->name, ifname);

	SHOW_IF_Q_DETAILS(*if_list_head, ingress)

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_queue_modify
 *  Description   : Internal API to modify queues on the interface
 *  Input Params  : 1. Interface name
 *                  2. Queue Properties
 *                  3. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_queue_modify(IN char *ifname,
		IN qos_queue_cfg_t	*q,
		IN uint32_t flags)
{
	int32_t ret = LTQ_SUCCESS, ingress = 0;
	uint32_t orig_wt = 0;
	qos_engines_t engine = QOS_ENGINE_MAX;
	qos_engines_t new_common_q_point = QOS_ENGINE_MAX;
	qos_if_node_t *if_list_node = NULL;
	struct list_head *q_list_tmp = NULL, *if_list_head = NULL;
	qos_queue_node_t *q_list_node = NULL;
	qos_queue_node_t old_q_list_node;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	
	ENTRY
	QOSD_GET_IFTYPE(ifname, es_tables.ifmap);

	if (flags & QOS_OP_F_INGRESS) {
		QOSD_GET_IFINGGRP(ifname, es_tables.ifinggrp_map);
		if_list_head = es_tables.ingress_if_list_head;
		ingress = 1;
		LOGF_LOG_DEBUG ("Configuring ingress queue %s for interface category %d\n", q->name, iftype);
	}
	else {
		if_list_head = es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Configuring egress queue %s for interface category %d\n", q->name, iftype);
	}

	if_list_node = qosd_get_if_node(if_list_head, ifname);

	if (if_list_node == NULL || if_list_node->q_count == 0){
		LOGF_LOG_ERROR("No queues are configured on Interface %s \n",ifname);
		ret = LTQ_FAIL;
		goto exit;
	}

	/* identify queue node to be deleted under this ifname */
	q_list_node = qosd_get_q_node(if_list_node->q_list_head, q->name);

	if(q_list_node == NULL ) {
		LOGF_LOG_ERROR("Given queue with id:name [%d:%s] not configured on specified interface %s\n", q->queue_id, q->name, ifname);
		ret = LTQ_FAIL;
		goto exit;
	}

	q_list_node->enable = QOSAL_ENABLE;

	/* Update queue id in queue cfg, later q node cfg will be overwritten with q cfg */
	q->queue_id = q_list_node->queue_cfg.queue_id;

	engine = qosd_get_queueing_point(iftype, if_list_node->q_count, ingress, (void*)q);
	if (engine >= QOS_ENGINE_MAX){
		LOGF_LOG_ERROR("Queue modify failed. Queuing common point is not found\n");
		ret = LTQ_FAIL;
		goto exit;
	}

	/* If queue to modify was configured in wfq, recalculate wt of other queue after removing its wt*/
	if (q_list_node->queue_cfg.sched == QOS_SCHED_WFQ) {
		if (q_list_node->queue_cfg.shaper.cir != 0)
	  	if_list_node->sum_cirs -= q_list_node->queue_cfg.shaper.cir; /* If add fails, we have undo this update */
		else 
	  	if_list_node->sum_wts -= q_list_node->orig_wt; /* If add fails, we have undo this update */
		q_list_node->orig_wt = 0;
		q_list_node->enable = QOSAL_DISABLE;
		if (q_wt_n_rate_derive(iftype, if_list_node, NULL) != LTQ_SUCCESS) {
			ret = LTQ_FAIL;
			goto exit;
		}
		q_list_node->enable = QOSAL_ENABLE;
	}
	/* If queue is modified to wfq, recalculate wt of all queues again */
	if (q->sched == QOS_SCHED_WFQ) {
		orig_wt = q->weight;
		if (q->shaper.cir != 0)
			if_list_node->sum_cirs += q->shaper.cir;
		else
			if_list_node->sum_wts += orig_wt; /* If add fails, we have undo this update */
	}
	if (q_wt_n_rate_derive(iftype, if_list_node, q) != LTQ_SUCCESS) {
		ret = LTQ_FAIL;
		goto exit;
	}
	/* Take backup of old current q node and update with comdified config */
	memcpy_s(&old_q_list_node, sizeof(qos_queue_node_t), q_list_node, sizeof(qos_queue_node_t));

	/* check if queuing point has moved because of current operation */
	if (old_q_list_node.q_point != engine) {
		q_list_node->q_point = engine;
		qosd_get_common_point(if_list_node, &new_common_q_point);
		if (new_common_q_point != if_list_node->common_q_point ) {
			/* if new common point is different than current one,
			 then move previous common point, delete all queues from prev point and add at new one */
			memcpy_s(&q_list_node->queue_cfg, sizeof(qos_queue_cfg_t), q, sizeof(qos_queue_cfg_t));
			LOGF_LOG_DEBUG ("Moving queue point from %s to %s\n",es_tables.eng_names[if_list_node->common_q_point], es_tables.eng_names[new_common_q_point]);
			qosd_move_queues(if_list_node, if_list_node->common_q_point, new_common_q_point);
			qosd_update_datapaths_status(ifname, flags & QOS_OP_F_INGRESS ?1 :0, if_list_node->common_q_point, new_common_q_point);
			if_list_node->common_q_point = new_common_q_point;
			goto label;
		}
	}

	LOGF_LOG_DEBUG("Old q point: %d, new q point: %d, new common q point: %d\n",old_q_list_node.q_point,q_list_node->q_point, if_list_node->common_q_point);

	/* atleast one queue in wfq mode, is below logic foolproof to identify it? */
	if (if_list_node->sum_wts != 0 ) {
		LOGF_LOG_DEBUG ("Updating queue weights for engine %s\n",es_tables.eng_names[if_list_node->common_q_point]);
		memcpy_s(&q_list_node->queue_cfg, sizeof(qos_queue_cfg_t), q, sizeof(qos_queue_cfg_t));
		qosd_move_queues(if_list_node, if_list_node->common_q_point, if_list_node->common_q_point);
	}
	else {
		//Call engine modify callback with common q point
		LOGF_LOG_DEBUG("Call engine modify callback with common q point (%d) and update in q node", if_list_node->common_q_point);
		QOSD_ENGINE_QUEUE_MODIFY(if_list_node->common_q_point, ifname, iftype, ifinggrp, q);
		/* copying modified q to q node is done at multiple places so that q node would be updated after calling engine's modify */
		memcpy_s(&q_list_node->queue_cfg, sizeof(qos_queue_cfg_t), q, sizeof(qos_queue_cfg_t));
	}

label:
	if (q->sched == QOS_SCHED_WFQ && q->shaper.cir == 0) {
		q_list_node->orig_wt = orig_wt;
	}

	SHOW_IF_Q_DETAILS(if_list_head, ingress)

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_queue_del
 *  Description   : Internal API to del queues on the interface
 *  Input Params  : 1. Interface name
 *                  2. Queue Properties
 *                  3. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_queue_del(IN char *ifname,
		IN qos_queue_cfg_t	*q,
		IN uint32_t flags)
{
	int32_t ingress = 0, ret = LTQ_SUCCESS;
	uint32_t queues_moved = 0;
	qos_engines_t new_common_q_point = QOS_ENGINE_MAX, q_point = QOS_ENGINE_MAX;
	qos_if_node_t *if_list_node = NULL;
	struct list_head *q_list_tmp = NULL, *if_list_head = NULL;
	qos_queue_node_t *q_list_node = NULL;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	
	ENTRY
	QOSD_GET_IFTYPE(ifname, es_tables.ifmap);

	if (flags & QOS_OP_F_INGRESS) {
		QOSD_GET_IFINGGRP(ifname, es_tables.ifinggrp_map);
		ingress = 1;
		if_list_head = es_tables.ingress_if_list_head;
		LOGF_LOG_DEBUG ("Configuring ingress queue %s for interface category %d\n", q->name, iftype);
	}
	else {
		if_list_head = es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Configuring egress queue %s for interface category %d\n", q->name, iftype);
	}

	if_list_node = qosd_get_if_node(if_list_head, ifname);

	if (if_list_node == NULL || if_list_node->q_count == 0){
		LOGF_LOG_ERROR("No queues are configured on Interface %s \n",ifname);
		ret = QOS_QUEUE_NOT_FOUND;
		goto exit;
	}

	if ((if_list_node->q_count > 1) && (q->flags & QOS_Q_F_DEFAULT) && ((q->flags & QOS_Q_F_SKIP_DEF_Q_CHK) == 0)) {
		LOGF_LOG_ERROR("Default queue should be deleted last for an interface\n");
		ret = QOS_DEF_Q_DEP_IN_DEL_FAIL;
		goto exit;
	}

	/* identify queue node to be deleted under this ifname */
	q_list_node = qosd_get_q_node(if_list_node->q_list_head, q->name);

	if(q_list_node != NULL ) {
#if 0 /* need fix. */
		if (is_cl_present_for_q(q) == LTQ_SUCCESS)
		{
			LOGF_LOG_ERROR("Can't delete queue [%s], there is a classifier present with this queue\n", q->name);
			return QOS_CLASSIFIER_FOUND;
		}
#endif

		/* Update queue id in queue cfg, later q node cfg will be overwritten with q cfg */
		q->queue_id = q_list_node->queue_cfg.queue_id;

		QOSD_ENGINE_QUEUE_DELETE(if_list_node->common_q_point, ifname, iftype, ifinggrp, q);
		q_point = q_list_node->q_point;
		if (q_list_node->queue_cfg.sched == QOS_SCHED_WFQ) {
			if (q_list_node->queue_cfg.shaper.cir != 0)
				if_list_node->sum_cirs -= q_list_node->queue_cfg.shaper.cir; /* If add fails, we have undo this update */
			else
				if_list_node->sum_wts -= q_list_node->orig_wt; /* If add fails, we have undo this update */
		}
		qosd_del_q_node(if_list_node, q);
		if_list_node->q_count--;
	}
	else {
		LOGF_LOG_ERROR("Given queue with id:name [%d:%s] not configured on specified interface %s\n", q->queue_id, q->name, ifname);
		ret = QOS_QUEUE_NOT_FOUND;
		goto exit;
	}

	if (if_list_node->q_count == 0 ) {
		qosd_engine_fini(if_list_node->common_q_point, QOS_MODULE_QUEUE);
		qosd_update_datapaths_status(ifname, flags & QOS_OP_F_INGRESS ?1 :0, if_list_node->common_q_point, QOS_ENGINE_MAX);
	}
	else { 
		if (q_wt_n_rate_derive(iftype, if_list_node, NULL) != LTQ_SUCCESS) {
			ret = LTQ_FAIL;
			goto exit;
		}
		/* check if queuing point has moved because of current operation */
		if (if_list_node->common_q_point == q_point ) {
			qosd_get_common_point(if_list_node, &new_common_q_point);
			if (new_common_q_point != if_list_node->common_q_point ) {
				/* if new common point is different than current one (ideally new will be at higher weight as its a delete operation ), 
				 then move previous common point, delete all queues from prev point and add at new one */
				LOGF_LOG_DEBUG ("Moving queue point from %s to %s\n",es_tables.eng_names[if_list_node->common_q_point], es_tables.eng_names[new_common_q_point]);
				qosd_move_queues(if_list_node, if_list_node->common_q_point, new_common_q_point);
				if_list_node->common_q_point = new_common_q_point;
				queues_moved = 1;
			}
		}

		/* atleast one queue in wfq mode, is below logic foolproof to identify it? */
		if (queues_moved == 0 && if_list_node->sum_wts != 0 ) {
			LOGF_LOG_DEBUG ("Updating queue weights for engine %s\n",es_tables.eng_names[if_list_node->common_q_point]);
			qosd_move_queues(if_list_node, if_list_node->common_q_point, if_list_node->common_q_point);
		}
	}

	SHOW_IF_Q_DETAILS(if_list_head, ingress)

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_queue_set
 *  Description   : Low FAPI to add/del/modify queues on the interface based on flag
 *  Input Params  : 1. Interface name
 *                  2. Queue Properties
 *                  3. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_queue_set(IN char *ifname,
		IN qos_queue_cfg_t	*q,
		IN uint32_t flags)
{
	int32_t ret = LTQ_SUCCESS;
	if(flags & QOS_OP_F_ADD) {
		ret = qosd_queue_add(ifname, q, flags);
	}
	else if(flags & QOS_OP_F_DELETE) {
		ret = qosd_queue_del(ifname, q, flags);
	}
	else if(flags & QOS_OP_F_MODIFY) {
		ret = qosd_queue_modify(ifname, q, flags);
	}
	else {
		LOGF_LOG_DEBUG("Operation not specified, flag is %x\n",flags);
	}

	return ret;
}

/* =============================================================================
 *  Function Name : qosd_queue_get
 *  Description   : Low FAPI to get queues on the interface
 *  Input Params  : 1. Interface name
 *                  2. Queue name, if NULL all queue on the interface are returned
 *                  5. Flags
 *  Output Params : 3. Number of Queues
 *                  4. Pointer to list of queues, if NULL only q count is returned
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_queue_get(IN char			*ifname,
							IN	char*     queue_name,
							OUT	uint32_t		*num_queues,
							OUT	qos_queue_cfg_t		**q,
							IN	uint32_t 		flags)
{
	ENTRY
	struct list_head *list_node = NULL, *if_list_head = NULL;
	qos_if_node_t *if_list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;
	uint32_t i = 0;
	uint32_t j = 0;
	qos_queue_cfg_t		*q_tmp=NULL;

	q_tmp = *q;
	if (q_tmp == NULL)
		LOGF_LOG_DEBUG("memory is not allocated\n");

	if (flags & QOS_OP_F_INGRESS) {
		if_list_head = es_tables.ingress_if_list_head;
		LOGF_LOG_DEBUG ("Fetching ingress queue for %s\n",ifname);
	}
	else {
		if_list_head = es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Fetching egress queue for %s\n",ifname);
	}

	if_list_node = qosd_get_if_node(if_list_head, ifname);
	if (if_list_node == NULL || if_list_node->q_list_head == NULL){
		LOGF_LOG_INFO("Queues are not configured for interface %s\n",ifname);
		*num_queues = 0;
		return LTQ_SUCCESS;
	}

	list_for_each(list_node, if_list_node->q_list_head) {
		q_list_node = list_entry(list_node, qos_queue_node_t, list);
		if (q_list_node->enable == QOSAL_ENABLE) {
			if (queue_name == NULL){
				if (q_tmp != NULL) {
					memcpy_s((void *) (q_tmp+i), sizeof(qos_queue_cfg_t), (void *)&q_list_node->queue_cfg, sizeof(qos_queue_cfg_t));
				}
				i++;
			}
			else if (strcmp(q_list_node->queue_cfg.name, queue_name) == 0) {
				if (q_tmp != NULL) {
					memcpy_s((void *) (q_tmp+j), sizeof(qos_queue_cfg_t), (void *)&q_list_node->queue_cfg, sizeof(qos_queue_cfg_t));
				}
				j++;
				break;
			}
		}
	}
	*num_queues = queue_name ? j : i;

	LOGF_LOG_DEBUG("Queue count is %d\n",*num_queues);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_queue_stats_get
 *  Description   : FAPI to get one or all QoS Queue Statistics on given interface.
 * ============================================================================*/
int32_t
qosd_queue_stats_get(
		char			*ifname,
		char*			queue_name,
		int32_t			*num_queues,
		qos_queue_stats_t	**qstats,
		uint32_t 		flags)
{
	int32_t ret = LTQ_SUCCESS;
	struct list_head *list_node = NULL, *if_list_head = NULL;
	qos_if_node_t *if_list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;

	ENTRY

	*num_queues = 0;
	QOSD_GET_IFTYPE(ifname, es_tables.ifmap);

	if (flags & QOS_OP_F_INGRESS) {
		QOSD_GET_IFINGGRP(ifname, es_tables.ifinggrp_map);
		if_list_head = es_tables.ingress_if_list_head;
		LOGF_LOG_DEBUG ("Fetching ingress queue for %s\n",ifname);
	}
	else {
		if_list_head = es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Fetching egress queue for %s\n",ifname);
	}

	if_list_node = qosd_get_if_node(if_list_head, ifname);
	if (if_list_node == NULL || if_list_node->q_list_head == NULL){
		LOGF_LOG_INFO ("Queues are not configured for interface %s\n",ifname);
		return LTQ_SUCCESS;
	}

	list_for_each(list_node, if_list_node->q_list_head) {
		q_list_node = list_entry(list_node, qos_queue_node_t, list);
		if (q_list_node->enable == QOSAL_ENABLE) {
			if (queue_name == NULL){
				*num_queues += 1;
				*qstats = (qos_queue_stats_t *) realloc((void *) *qstats, sizeof(qos_queue_stats_t)*(*num_queues)); //change it to os_realloc
				if (*qstats == NULL ) {
					LOGF_LOG_CRITICAL("Could not get memory of [%lu]\n", (long unsigned int) sizeof(qos_queue_stats_t)*(*num_queues));
					break;
				}
				memset_s((*qstats + *num_queues - 1), sizeof(qos_queue_stats_t), 0);
				QOSD_ENGINE_QUEUE_STATS_GET(if_list_node->common_q_point, if_list_node->ifname, iftype, ifinggrp, q_list_node->queue_cfg.name, *qstats + *num_queues - 1);
				if(strncpy_s((*qstats + *num_queues - 1)->name, MAX_Q_NAME_LEN, q_list_node->queue_cfg.name, MAX_Q_NAME_LEN) != EOK) {
					LOGF_LOG_ERROR("Error copying Q name string\n");
					return LTQ_FAIL;
				}
			}
			else if (strcmp(q_list_node->queue_cfg.name, queue_name) == 0) {
				*num_queues = 1;
				*qstats = (qos_queue_stats_t *) os_calloc(1, sizeof(qos_queue_stats_t));
				QOSD_ENGINE_QUEUE_STATS_GET(if_list_node->common_q_point, if_list_node->ifname, iftype, ifinggrp, queue_name, *qstats);
				if(strncpy_s((*qstats)->name, MAX_Q_NAME_LEN, q_list_node->queue_cfg.name, MAX_Q_NAME_LEN) != EOK) {
					LOGF_LOG_ERROR("Error copying Q name string\n");
                                        return LTQ_FAIL;
				}
				break;
			}
		}
	}

	if (*qstats != NULL ) {
		LOGF_LOG_DEBUG("\nTx Bytes are %llu\n\n", (unsigned long long)(*qstats)->tx_bytes);
	}
	LOGF_LOG_DEBUG("Queue count is %d\n",*num_queues);

exit:
	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_show_queues
 *  Description   : API to show queue configuration
 * ============================================================================*/
int32_t qosd_show_queues()
{
	struct list_head *if_list_tmp = NULL, *q_list_tmp = NULL, *if_list_head = NULL;
	qos_if_node_t *if_list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;
	int32_t i = 0;

	i = 0;
	while(i <= 1) {
		if(i == 1) {
			if_list_head = es_tables.egress_if_list_head;
			LOGF_LOG_INFO("----Egress queue configuration----\n");
		}
		else {
			if_list_head = es_tables.ingress_if_list_head;
			LOGF_LOG_INFO("----Ingress queue configuration----\n");
		}
		if(if_list_head != NULL) {
			list_for_each(if_list_tmp, if_list_head) {
				if_list_node = list_entry(if_list_tmp, qos_if_node_t, list);
				LOGF_LOG_INFO("Interface details\n");
				LOGF_LOG_INFO("\tName - %s\n", if_list_node->ifname);
				LOGF_LOG_INFO("\t\tCommon queuing point\t: %s\n", es_tables.eng_names[if_list_node->common_q_point]);
				LOGF_LOG_INFO("\t\tInterface rate\t\t: %d\n", if_list_node->port_rate);
				/* print queues configured with complete details under this interface */
				if (if_list_node->q_list_head != NULL) {
					list_for_each(q_list_tmp, if_list_node->q_list_head){
						q_list_node = list_entry(q_list_tmp, qos_queue_node_t, list);
						LOGF_LOG_INFO("\tQueue [%d]\n", q_list_node->queue_cfg.queue_id);
						LOGF_LOG_INFO("\t\tName\t\t\t: %s\n", q_list_node->queue_cfg.name);
						LOGF_LOG_INFO("\t\tPriority\t\t: %d\n", q_list_node->queue_cfg.priority);
						LOGF_LOG_INFO("\n");
						LOGF_LOG_INFO("\t\tLength\t\t\t: %d\n", q_list_node->queue_cfg.qlen);
						LOGF_LOG_INFO("\t\tWeight\t\t\t: %d\n", q_list_node->queue_cfg.weight);
						LOGF_LOG_INFO("\t\tDrop algorithm\t\t: %s\n", qosd_get_str_drop_mode(q_list_node->queue_cfg.drop.mode));
						LOGF_LOG_INFO("\t\tScheduling algorithm\t: %s\n", qosd_get_str_sched_mode(q_list_node->queue_cfg.sched));
						LOGF_LOG_INFO("\t\tShaper configuration\t: %s\n", qosd_get_str_shaper(q_list_node->queue_cfg.shaper.mode));
						if(q_list_node->queue_cfg.shaper.cir)
							LOGF_LOG_INFO("\t\tCommitted information rate\t: %d\n", q_list_node->queue_cfg.shaper.cir);
						if(q_list_node->queue_cfg.shaper.cbs)
							LOGF_LOG_INFO("\t\tCommitted burst size\t: %d\n", q_list_node->queue_cfg.shaper.cbs);
						if(q_list_node->queue_cfg.shaper.pir)
							LOGF_LOG_INFO("\t\tPeak information rate\t: %d\n", q_list_node->queue_cfg.shaper.pir);
						if(q_list_node->queue_cfg.shaper.pbs)
							LOGF_LOG_INFO("\t\tPeak burst size\t: %d\n", q_list_node->queue_cfg.shaper.pbs);
					}
				}
				else {
					LOGF_LOG_INFO("\t\tQueues are not configured for this interface\n");
				}
			}
		}
		i++;
	}
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_port_config_add
 *  Description   : Internal API to set Port configuration like shaper
 *  Input Params  : 1. Interface name
 *                  2. Shaper Properties
 *                  3. Port Weight in case its output is sent to another scheduler
 *                  4. Port Priority in case its output is sent to another scheduler
 *                  5. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_port_config_add(IN char *ifname,
			IN qos_shaper_t *shaper,
			IN int32_t weight,
			IN int32_t priority,
			IN uint32_t flags)
{
	struct list_head **if_list_head = NULL;
	qos_if_node_t *if_list_node = NULL;
	qos_engines_t engine = QOS_ENGINE_MAX;
	int32_t ingress = 0, ret = LTQ_SUCCESS;
	uint32_t moved = 0;
	char base_if[MAX_IF_NAME_LEN] = {0}, logical_if[MAX_IF_NAME_LEN] = {0};
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	
	ENTRY
	QOSD_GET_IFTYPE(ifname, es_tables.ifmap);

	if (flags & QOS_OP_F_INGRESS) {
		QOSD_GET_IFINGGRP(ifname, es_tables.ifinggrp_map);
		ingress = 1;
		if_list_head = &es_tables.ingress_if_list_head;
		LOGF_LOG_DEBUG ("Ingress port config requested.\n");
	}
	else {
		if_list_head = &es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Egress port config requested.\n");
	}

	if((iftype == QOS_IF_CATEGORY_ETHWAN) || (iftype == QOS_IF_CATEGORY_PTMWAN) || (iftype == QOS_IF_CATEGORY_ATMWAN)) {
		if (qosd_if_base_get(ifname,base_if,logical_if,0) == LTQ_FAIL ) {
			LOGF_LOG_ERROR("Base interface is not provided for [%s]\n", ifname);
			ret = QOS_IFMAP_LOOKUP_FAIL;
			goto exit;
		}
	}

	engine = qosd_get_queueing_point(iftype, 0, ingress, (void*)shaper);
	if (engine >= QOS_ENGINE_MAX){
		LOGF_LOG_ERROR("Shaper add failed. Common point is not found\n");
		ret = LTQ_FAIL;
		goto exit;
	}

	if_list_node = qosd_get_if_node(*if_list_head, ifname);

	if (if_list_node == NULL) {
		ret = qosd_add_if_node(if_list_head, ifname, &if_list_node);
		if(ret != LTQ_SUCCESS ) {
			LOGF_LOG_ERROR("Adding interface node for [%s] failed", ifname);
			ret = QOS_IF_ADD_FAIL;
			goto exit;
		}
		if(strncpy_s(if_list_node->ifname, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN) != EOK) {
			LOGF_LOG_ERROR("Error copying ifname string\n");
                        ret = QOS_IF_ADD_FAIL;
                        goto exit;
		}
		es_tables.act_if_cnt++;
	}

	QOSD_VALIDATE_IF_NODE(if_list_node);
	LOGF_LOG_DEBUG ("shaper pir is %d, cir:%d\n",shaper->pir, shaper->cir);
	if ( if_list_node->port_shaper == NULL )
		if_list_node->port_shaper = (qos_port_shaper_node_t *) os_calloc(1, sizeof(qos_port_shaper_node_t));

	if(if_list_node->port_shaper == NULL) {
		LOGF_LOG_CRITICAL("Memory not allocated for shaper on interface [%s]\n", ifname);
		return QOS_MEM_ALLOC_FAIL;
	}

	memcpy_s((void *)&if_list_node->port_shaper->shaper, sizeof(qos_shaper_t), (void *)shaper, sizeof(qos_shaper_t));
	if_list_node->port_shaper->priority = priority;
	if_list_node->port_shaper->weight = weight;

	if (if_list_node->q_count != 0) {
		/* TODO consider other shaper properties also */
		//if_list_node->port_rate = shaper->pir;
		/* Update weight calculated from CIR, as port rate is changed */
		if (if_list_node->sum_cirs != 0) {
			if (q_wt_n_rate_derive(iftype, if_list_node, NULL) != LTQ_SUCCESS) {
				ret = LTQ_FAIL;
				goto exit;
			}
		}

		if (es_tables.engine_info[if_list_node->common_q_point].engine.used_resource > es_tables.engine_info[engine].engine.used_resource) {
			/* if previous common point is at high weight (like hw) than current one, 
				 then move previous common point, delete all queues from prev point and add at new one */

			LOGF_LOG_INFO ("Moving queue point from %s to %s\n",es_tables.eng_names[if_list_node->common_q_point], es_tables.eng_names[engine]);
			qosd_move_queues(if_list_node, if_list_node->common_q_point, engine);
			moved = 1;
		}
		else {
			engine = if_list_node->common_q_point;
		}
		if (moved == 0 && if_list_node->sum_cirs != 0)
			qosd_move_queues(if_list_node, if_list_node->common_q_point, if_list_node->common_q_point);
	}

	if_list_node->common_q_point = engine;
	if_list_node->port_shaper->shaping_point = engine;
	qosd_engine_init(engine, QOS_MODULE_QUEUE);
	//Call engine callback to shape the port for engine

	QOSD_ENGINE_PORT_SET(engine, ifname, iftype, ifinggrp, shaper, priority, weight, flags);
	if (if_list_node->q_count == 0)
		qosd_update_datapaths_status(ifname, ingress, QOS_ENGINE_MAX, engine);

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_port_config_del
 *  Description   : Internal API to set Port configuration like shaper
 *  Input Params  : 1. Interface name
 *                  2. Shaper Properties
 *                  3. Port Weight in case its output is sent to another scheduler
 *                  4. Port Priority in case its output is sent to another scheduler
 *                  5. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
static int32_t
qosd_port_config_del(IN char *ifname,
			IN qos_shaper_t *shaper,
			IN int32_t weight,
			IN int32_t priority,
			IN uint32_t flags)
{
	struct list_head *if_list_head = NULL;
	qos_if_node_t *if_list_node = NULL;
	qos_engines_t new_common_q_point = QOS_ENGINE_MAX;
	int32_t ret = LTQ_SUCCESS;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	
	ENTRY
	UNUSED(weight);
	UNUSED(priority);

	QOSD_GET_IFTYPE(ifname, es_tables.ifmap);

	if (flags & QOS_OP_F_INGRESS) {
		QOSD_GET_IFINGGRP(ifname, es_tables.ifinggrp_map);
		if_list_head = es_tables.ingress_if_list_head;
		LOGF_LOG_DEBUG ("Ingress port config requested.\n");
	}
	else {
		if_list_head = es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Egress port config requested.\n");
	}

	if_list_node = qosd_get_if_node(if_list_head, ifname);
	QOSD_VALIDATE_IF_NODE(if_list_node);
	LOGF_LOG_DEBUG ("shaper pir is %d, cir:%d\n",shaper->pir, shaper->cir);

	if ( if_list_node->port_shaper == NULL ) {
		ret = QOS_ENTRY_NOT_FOUND;
		goto exit;
	}
	
	//Call engine callback to shape the port for engine
	QOSD_ENGINE_PORT_SET(if_list_node->common_q_point, ifname, iftype, ifinggrp, shaper, priority, weight, flags);
	qosd_engine_fini(if_list_node->common_q_point, QOS_MODULE_QUEUE);

	if (if_list_node->q_count != 0 && if_list_node->common_q_point == if_list_node->port_shaper->shaping_point ) {
		qosd_get_common_point(if_list_node, &new_common_q_point);
		if (new_common_q_point != if_list_node->common_q_point ) {
			/* if new common point is different than current one (ideally new will be at higher weight as its a delete operation ), 
				 then move previous common point, delete all queues from prev point and add at new one */

			LOGF_LOG_INFO ("Moving queue point from %s to %s\n",es_tables.eng_names[if_list_node->common_q_point], es_tables.eng_names[new_common_q_point]);
			qosd_move_queues(if_list_node, if_list_node->common_q_point, new_common_q_point);
			if_list_node->common_q_point = new_common_q_point;

			/* TODO: Identify datapaths to enable */
			//qosd_enable_datapaths(dps_to_enable);
		}
	}

	if ( if_list_node->port_shaper != NULL ) {
		os_free(if_list_node->port_shaper);
		if_list_node->port_shaper = NULL;
	}
	
	/* Update weight calculated from CIR, as port rate is changed */
	if (if_list_node->q_count != 0 && if_list_node->sum_cirs != 0) {
		if (q_wt_n_rate_derive(iftype, if_list_node, NULL) != LTQ_SUCCESS) {
			ret = LTQ_FAIL;
			goto exit;
		}
		qosd_move_queues(if_list_node, if_list_node->common_q_point, if_list_node->common_q_point);
	}

	if (if_list_node->q_count == 0)
		qosd_update_datapaths_status(ifname, flags & QOS_OP_F_INGRESS ?1 :0, if_list_node->common_q_point, QOS_ENGINE_MAX);

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_port_config_set
 *  Description   : Low FAPI to set Port configuration like shaper
 *  Input Params  : 1. Interface name
 *                  2. Shaper Properties
 *                  3. Port Weight in case its output is sent to another scheduler
 *                  4. Port Priority in case its output is sent to another scheduler
 *                  5. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_port_config_set(IN char *ifname,
			IN qos_shaper_t *shaper,
			IN int32_t weight,
			IN int32_t priority,
			IN uint32_t flags)
{
	int32_t ret = LTQ_SUCCESS;
	if((flags & QOS_OP_F_ADD) || (flags & QOS_OP_F_MODIFY)) {
		ret = qosd_port_config_add(ifname, shaper, weight, priority, flags);
	}
	else {
		ret = qosd_port_config_del(ifname, shaper, weight, priority, flags);
	}
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_port_config_get
 *  Description   : Low FAPI to get Port configuration like shaper
 *  Input Params  : 1. Interface name
 *                  5. Flags
 *  Output Params : 2. Shaper Properties
 *                  3. Port Weight in case its output is sent to another scheduler
 *                  4. Port Priority in case its output is sent to another scheduler
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_port_config_get(IN char *ifname,
			OUT qos_shaper_t *shaper,
			OUT int32_t *weight,
			OUT int32_t *priority,
			IN uint32_t flags)
{
	struct list_head *if_list_head = NULL;
	qos_if_node_t *if_list_node = NULL;
	int32_t ret = LTQ_FAIL;

	ENTRY

	if(shaper == NULL && weight == NULL && priority == NULL) {
		LOGF_LOG_ERROR("shaper pointer is NULL\n");
		return LTQ_FAIL;
	}

	if (flags & QOS_OP_F_INGRESS) {
		//ingress = 1;
		if_list_head = es_tables.ingress_if_list_head;
		LOGF_LOG_DEBUG ("Fetching ingress port config\n");
	}
	else {
		if_list_head = es_tables.egress_if_list_head;
		LOGF_LOG_DEBUG ("Fetching egress port config\n");
	}


	if_list_node = qosd_get_if_node(if_list_head, ifname);

	if (if_list_node == NULL || if_list_node->port_shaper == NULL) {
		LOGF_LOG_ERROR("Port shaper is not configured\n");
		return ret;
	}

	if(shaper != NULL)
		memcpy_s((void *)shaper, sizeof(qos_shaper_t), (void *)&if_list_node->port_shaper->shaper, sizeof(qos_shaper_t));
	if(priority != NULL)
		*priority  = if_list_node->port_shaper->priority;
	if(weight != NULL)
		*weight = if_list_node->port_shaper->weight;

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_port_update
 *  Description   : Low FAPI to update Port state, e.g. UP/DOWN
 *  Input Params  : 1. Interface name
 *                  2. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_port_update(IN char *ifname,
		IN uint32_t flags)
{
	ENTRY
	int32_t ret, ret2, i;
	qos_engines_t engine;
	qos_if_node_t *if_list_node = NULL;
	qos_shaper_t shaper;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	uint32_t t_flags = 0;
	
	ret = ret2 = LTQ_SUCCESS;
	memset_s(&shaper, sizeof(shaper), 0);

	QOSD_GET_IFTYPE(ifname, es_tables.ifmap);

	/* interface does not exist */
	if (flags & QOS_OP_F_ADD) {
		/* add interace to list */

		/* is it needed now ?*/
		if((flags & QOS_OP_F_INGRESS) == QOS_OP_F_INGRESS)
		{
		ret2 = qosd_add_if_node(&es_tables.ingress_if_list_head, ifname, &if_list_node);
		}
		else
		ret = qosd_add_if_node(&es_tables.egress_if_list_head, ifname, &if_list_node);
		if(ret != LTQ_SUCCESS || ret2 != LTQ_SUCCESS) {
			LOGF_LOG_ERROR(" qosd_port_update: qosd_add_if_node failed \n");
		}
		else {
			LOGF_LOG_INFO("interface [%s] added successfully\n", ifname);
			strncpy_s(if_list_node->ifname, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
			es_tables.act_if_cnt++;
		}
	}
	else if (flags & QOS_OP_F_DELETE) {
		/* delete interface from list */

		if((flags & QOS_OP_F_INGRESS) == QOS_OP_F_INGRESS)
		ret2 = qosd_del_if_node(&es_tables.ingress_if_list_head, ifname, &es_tables);
		else
		ret = qosd_del_if_node(&es_tables.egress_if_list_head, ifname, &es_tables);
		if(ret != LTQ_SUCCESS || ret2 != LTQ_SUCCESS) {

		}
		else {
			LOGF_LOG_INFO("interface [%s] removed successfully\n", ifname);
		}
	}
	else {
		/* interface rate change event */
		i = 0;
		while(i <= 1) {
			/* need to traverse through ingress and egress queues, which are separate lists inside if node.
			   and for queue, q node itself has flags to differentiate if queue is ingress/egress.
			   so no differentiation is required in queue delete/add operation code.
			*/
			if(i == 1) {
				if_list_node = qosd_get_if_node(es_tables.egress_if_list_head, ifname);
			}
			else {
				if_list_node = qosd_get_if_node(es_tables.ingress_if_list_head, ifname);
			}
		
			/* check if any queues exist on this interface.
			   TODO take care of checking classifiers in future.
			*/
			if(if_list_node != NULL && if_list_node->q_list_head != NULL && if_list_node->q_count > 0) {
				if (if_list_node->ingress)
					QOSD_GET_IFINGGRP(ifname, es_tables.ifinggrp_map);
				engine = if_list_node->common_q_point;
				t_flags = 0; /* to indicate that engine needs to read port rate by itself from system. */
				QOSD_ENGINE_PORT_SET(engine, ifname, iftype, ifinggrp, &shaper, if_list_node->port_shaper->priority, if_list_node->port_shaper->weight, t_flags);
			}
			else {
				/* nothing to do. */
			}
			/* Update weight calculated from CIR, as port rate is changed */
			if (if_list_node != NULL && if_list_node->q_count != 0 && if_list_node->sum_cirs != 0) {
				if (q_wt_n_rate_derive(iftype, if_list_node, NULL) != LTQ_SUCCESS) {
					ret = LTQ_FAIL;
					goto exit;
				}
				qosd_move_queues(if_list_node, if_list_node->common_q_point, if_list_node->common_q_point);
			}

			i++;
		}
	}

exit:
	EXIT
	return ret;
}

/* for add operation, add interface to the group. irrespective of interface being the first in the group or not,
   no action is required w.r.t. configuring classifiers/queues.
   as assumption is that caller will explicitly configure the interface to interface group mapping first,
   and then configure classifiers/queues on that interface explicitly.

   for delete operation, (case 1) check if interface is present for specified group.
   before removing the mapping, check if any unique classifiers/queues are configured on this interface.
   then those unique classifiers/queues need to be removed first and then only this interface mapping can be removed.
   so API would return failure in such case and assume caller to explicitly remove any unique classifiers/queues
   configured on such interface.

   (case 2) in case classifiers/queues is configured on multiple interfaces including the interface going to be deleted,
   then deletion of the interface can be allowed if those classifiers/queues can be accommodated.

   Also invoke interface abstraction callback at engine, for any specific
   handling based on interface type - like set default priority (if other than 0)
   for traffic thru specified interface.
*/
/* =============================================================================
 *  Function Name : qosd_if_abs_set
 *  Description   : Low FAPI to set interface and its category
 *  Input Params  : 1. Interface Group
 *                : 2. Interface name
 *                  3. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_if_abs_set(
	IN iftype_t	ifgroup,
	IN char		*ifname,
	IN uint32_t 	flags)
{
	int32_t i = 0, j = 0;
	qos_if_node_t *if_list_node = NULL;
	int32_t engine, ret = LTQ_SUCCESS;

	if(flags & QOS_OP_F_ADD) {
		/* just add interface to DS */
		for(i=0; i<QOS_IF_CATEGORY_MAX; i++) {
			if(es_tables.ifmap[i].ifcat == (int8_t)ifgroup) {
				/* check if interface already exists in this group to avoid duplicate entry */
				for(j=0; j<es_tables.ifmap[i].ifcount; j++) {
					if(!strncmp(es_tables.ifmap[i].ifnames[j], ifname, MAX_IF_NAME_LEN)){
						ret = QOS_ENTRY_EXISTS;
						goto enghandle;
					}
				}
				/* ... interface does'nt exist in this group. add */
				strncpy_s(es_tables.ifmap[i].ifnames[es_tables.ifmap[i].ifcount], MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
				es_tables.ifmap[i].ifcount++;
			}
		}
	}
	else {
		/* QOS_OP_F_DELETE flag is set */
		for(i=0; i<QOS_IF_CATEGORY_MAX; i++) {
			if(es_tables.ifmap[i].ifcat == (int8_t)ifgroup) {
				for(j=0; j<es_tables.ifmap[i].ifcount; j++) {
					if(!strcmp(ifname, es_tables.ifmap[i].ifnames[j])) {
						/* check if any queues/classifiers are configured on this ifgroup.
						if so then deny requested operation.
						*/
						if_list_node = qosd_get_if_node(es_tables.egress_if_list_head, ifname);
						//if_list_node = qosd_get_if_node(es_tables.ingress_if_list_head, ifname);

						if(if_list_node != NULL && 
							((if_list_node->q_count > 0) || (if_list_node->class_count > 0))) {
							/* queues/classifiers are configured on this interface */
							LOGF_LOG_ERROR("Interface map deletion failed. Queues/classifiers may exist on [%s]\n",
								ifname);
							ret = QOS_IF_ABS_SET_DEP_FAIL;
							goto exitlbl;
						}
						memset_s(es_tables.ifmap[i].ifnames[j], sizeof(es_tables.ifmap[i].ifnames[j]), 0);
						es_tables.ifmap[i].ifcount--;
						break;
					}
				}
			}
		}
	}

	/* interface abstraction callback at engine, for any specific
	 * handling based on interface type - like set default priority (if other than 0)
	 * for traffic thru specified interface.
	 */
enghandle:
	LOGF_LOG_DEBUG("Invoke interface abstraction callback of each engine, if available.\n");
	for(engine=0; engine<es_tables.number_of_engines; engine++) {
		if(es_tables.engine_info[es_tables.eng_ids[engine] - 1].status == 1) {
			if(flags & QOS_OP_F_ADD) {
				QOSD_ENGINE_IFABS_SET(engine, ifgroup, ifname, 1)
			}
			else {
				QOSD_ENGINE_IFABS_SET(engine, ifgroup, ifname, 0)
			}
		}
	}

	for(i=0; i<QOS_IF_CATEGORY_MAX; i++) {
		LOGF_LOG_INFO("Interface category [%s]\n", iftype_str[i]);
		for(j=0; j<es_tables.ifmap[i].ifcount; j++) {
			LOGF_LOG_INFO("\tinterface [%s]\n", es_tables.ifmap[i].ifnames[j]);
		}
	}
exitlbl:
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_if_base_get
 *  Description   : Low FAPI to get interface and its base interface
 *  Input Params  : 1. Interface name
 *                : 2. base nterface name
 *                  3. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_if_base_get(
	IN char		*ifname,
	OUT char	*base_ifname,
	OUT char	*logical_base,
	IN uint32_t 	flags)
{
	int32_t i = 0, j = 0;
	int32_t ret = LTQ_FAIL;

	UNUSED(flags);
	for(i=0; i<QOS_IF_CATEGORY_MAX; i++) {
		for(j=0; j<es_tables.ifmap[i].ifcount; j++) {
			if(!strncmp(es_tables.ifmap[i].ifnames[j], ifname, MAX_IF_NAME_LEN)) {
				strncpy_s(base_ifname, MAX_IF_NAME_LEN, es_tables.ifmap[i].base_ifnames[j], MAX_IF_NAME_LEN);
				strncpy_s(logical_base, MAX_IF_NAME_LEN, es_tables.ifmap[i].logical_base[j], MAX_IF_NAME_LEN);
				ret = LTQ_SUCCESS;
				break;
			}
		}
		if (ret == LTQ_SUCCESS)
			break;
	}

	if ((ret == LTQ_SUCCESS) && (i<QOS_IF_CATEGORY_MAX)) {
		LOGF_LOG_INFO("Base:logical interface [%s:%s] for interface [%s]\n", base_ifname, logical_base, es_tables.ifmap[i].ifnames[j]);
	}
	else {
		LOGF_LOG_INFO("Base interface is not provided for interface [%s]\n", ifname);
	}

	return ret;
}


/* =============================================================================
 *  Function Name : qosd_if_base_set
 *  Description   : Low FAPI to set interface and its base interface
 *  Input Params  : 1. Interface name
 *                : 2. base nterface name
 *                  3. Flags
 *  Return value  : always LTQ_SUCCESS(0)
 * ============================================================================*/
int32_t
qosd_if_base_set(
	IN char		*ifname,
	IN char		*base_ifname,
	IN char		*logical_base,
	IN uint32_t 	flags)
{
	int32_t i = 0, j = 0;

	ENTRY

	for(i=0; i<QOS_IF_CATEGORY_MAX; i++) {
		/* check if base interface already exists for this interface to avoid duplicate entry */
		for(j=0; j<es_tables.ifmap[i].ifcount; j++) {
			if(!strncmp(es_tables.ifmap[i].ifnames[j], ifname, MAX_IF_NAME_LEN)) {
				if(flags & QOS_OP_F_ADD) {
					/* Update base irrespective of its existence */
					strncpy_s(es_tables.ifmap[i].base_ifnames[j], MAX_IF_NAME_LEN, base_ifname, MAX_IF_NAME_LEN);
					strncpy_s(es_tables.ifmap[i].logical_base[j], MAX_IF_NAME_LEN, logical_base, MAX_IF_NAME_LEN);
				}
				else
					memset_s(es_tables.ifmap[i].base_ifnames[j], MAX_IF_NAME_LEN, 0);
				LOGF_LOG_INFO("Updated base:logical interface [%s:%s] for interface [%s]\n",
					es_tables.ifmap[i].base_ifnames[j], es_tables.ifmap[i].logical_base[j], es_tables.ifmap[i].ifnames[j]);
				break;
			}
		}
	}

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_if_abs_get
 *  Description   : Low FAPI to get interfaces of the category
 *  Input Params  : 1. Interface Group
 *                  4. Flags
 *  Output Params : 2. Number of interfaces in the category
 *                : 3. Interface list in the category
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_if_abs_get(
	IN iftype_t	ifgroup,
	OUT int32_t	*num_entries,
	OUT char	(*ifnames)[MAX_IF_NAME_LEN],
	IN uint32_t 	flags)
{
	uint32_t i=0;
	int32_t j=0;

	UNUSED(flags);

	for(i=0; i<QOS_IF_CATEGORY_MAX; i++) {
		if (i == ifgroup) {
			*num_entries = es_tables.ifmap[i].ifcount;
			for(j=0; j<es_tables.ifmap[i].ifcount; j++) {
				memcpy_s(ifnames[j], MAX_IF_NAME_LEN, es_tables.ifmap[i].ifnames[j], MAX_IF_NAME_LEN);
			}
			break;
		}
	}

	if (i<QOS_IF_CATEGORY_MAX) {
		LOGF_LOG_DEBUG("Number of interfaces for category %s: %d\n",iftype_str[i], *num_entries);
	}
	else {
		LOGF_LOG_DEBUG("No interfaces are added for category %s\n",iftype_str[ifgroup]);
	}

	return LTQ_SUCCESS;
}

if_inggrp_map_t *inggrp_map_get_by_ifname(const char *ifname)
{
	int i, j;

	if (!ifname)
		return NULL;

	for (i = 0; i < QOS_IF_CAT_INGGRP_MAX; i++)
		for (j = 0; j < MAX_IF_PER_INGRESS_GROUP; j++)
			if (!strncmp(es_tables.ifinggrp_map[i].ifnames[j], ifname, MAX_IF_NAME_LEN))
				return &es_tables.ifinggrp_map[i];

	return NULL;
}

static inline if_inggrp_map_t *inggrp_map_get_by_inggrp(ifinggrp_t ifinggrp)
{
	int i;

	if (IFINGGRP_INVALID(ifinggrp))
		return NULL;

	for (i = 0; i < QOS_IF_CAT_INGGRP_MAX; i++)
		if (es_tables.ifinggrp_map[i].inggrp == (int8_t)ifinggrp)
			return &es_tables.ifinggrp_map[i];

	return NULL;
}

static inline int ifinggrp_map_add_if(ifinggrp_t ifinggrp, const char *ifname)
{
	if_inggrp_map_t *pmap;
	int i;

	if (inggrp_map_get_by_ifname(ifname))
		return QOS_ENTRY_EXISTS;

	pmap = inggrp_map_get_by_inggrp(ifinggrp);
	if (!pmap)
		return QOS_IFMAP_LOOKUP_FAIL;
	
	for (i = 0; i < MAX_IF_PER_INGRESS_GROUP; i++)
		if (!pmap->ifnames[i][0]) {
			LOGF_LOG_DEBUG("Add %s to inggrp%d_map[%d]\n", ifname, pmap->inggrp, i);
			strncpy_s(pmap->ifnames[i], MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
			return LTQ_SUCCESS;
		}

	/* no more room */
	return QOS_IF_ADD_FAIL;
}

static inline int ifinggrp_map_del_if(const char *ifname)
{
	if_inggrp_map_t *pmap;
	int i;

	pmap = inggrp_map_get_by_ifname(ifname);
	if (!pmap)
		return QOS_IFMAP_LOOKUP_FAIL;

	for (i = 0; i < MAX_IF_PER_INGRESS_GROUP; i++)
		if (!strncmp(pmap->ifnames[i], ifname, MAX_IF_NAME_LEN)) {
			LOGF_LOG_DEBUG("Delete %s from inggrp%d_map[%d]\n", ifname, pmap->inggrp, i);
			memset_s(pmap->ifnames[i], MAX_IF_NAME_LEN, 0);
			return LTQ_SUCCESS;
		}

	/* not found */
	return QOS_IFMAP_LOOKUP_FAIL;
}

static inline void ifinggrp_map_dump(void)
{
	if_inggrp_map_t *pmap;
	int i,j;

	for (i = 0; i < QOS_IF_CAT_INGGRP_MAX; i++) {
		pmap = &es_tables.ifinggrp_map[i];
		LOGF_LOG_DEBUG("es tables inggrp%d interfaces:\n", pmap->inggrp);
		for (j = 0; j < MAX_IF_PER_INGRESS_GROUP; j++)
			if (pmap->ifnames[j][0])
				LOGF_LOG_DEBUG("%s\n", pmap->ifnames[j]);
	}
}

/* =============================================================================
 *  Function Name : qosd_if_inggrp_set
 *  Description   : Low FAPI to set interface and its ingress group
 *  Input Params  : 1. Interface Ingress Group
 *                : 2. Interface name
 *                  3. Flags
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_if_inggrp_set(
	IN ifinggrp_t	ifinggrp,
	IN char		*ifname,
	IN uint32_t	flags)
{
	int ret = LTQ_SUCCESS;

	if (flags & QOS_OP_F_ADD) {
		ret = ifinggrp_map_add_if(ifinggrp, ifname);
	} else if (flags & QOS_OP_F_DELETE) {
		ret = ifinggrp_map_del_if(ifname);
	} else { /* modify */
		ifinggrp_map_del_if(ifname);
		ret = ifinggrp_map_add_if(ifinggrp, ifname);
	}

	ifinggrp_map_dump();
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_if_inggrp_get
 *  Description   : Low FAPI to get interfaces of the ingress group
 *  Input Params  : 1. Ingress Group
 *                  4. Flags
 *  Output Params : 2. Number of interfaces in the ingress group
 *                : 3. Interface list in the ingress group
 *  Return value  : LTQ_SUCCESS(0) on success else LTQ_FAIL(-1)
 * ============================================================================*/
int32_t
qosd_if_inggrp_get(
	IN ifinggrp_t ifinggrp,
	OUT int32_t	*num_entries,
	OUT char	(*ifnames)[MAX_IF_NAME_LEN],
	IN uint32_t flags)
{
	if_inggrp_map_t *pmap; 
	char *pos = ifnames[0];
	int i;

	UNUSED(flags);

	*num_entries = 0;
	pmap = inggrp_map_get_by_inggrp(ifinggrp);
	if (!pmap)
		return QOS_IFMAP_LOOKUP_FAIL;

	for (i = 0; i < MAX_IF_PER_INGRESS_GROUP; i++)
		if (pmap->ifnames[i][0]) {
			LOGF_LOG_DEBUG("inggrp%d[%d]=%s\n", pmap->inggrp, i, pmap->ifnames[i]);
			if(strncpy_s(pos, MAX_IF_NAME_LEN, pmap->ifnames[i], MAX_IF_NAME_LEN) != EOK) {
				LOGF_LOG_ERROR("error copying string\n");
				return LTQ_FAIL;
			}
			pos += MAX_IF_NAME_LEN;
			(*num_entries)++;
		}

	return LTQ_SUCCESS;
}

///////////////////////////Classification starts //////////////////////////////////
/* =============================================================================
 *  Function Name : qosd_get_if_rule_count
 *  Description   : Internal API to get interface rule count
 * ============================================================================*/
static uint32_t
qosd_get_if_rule_count(IN qos_class_node_t* rules[])
{
	uint32_t i = 0;

	ENTRY
	while (rules[i] != NULL)
		i++;

	LOGF_LOG_DEBUG("Number of rules are %d\n",i);
	EXIT
	return i;
}

/* =============================================================================
 *  Function Name : qosd_insert_array_member
 *  Description   : Internal API to insert an array member at given location
 * ============================================================================*/
static int32_t
qosd_insert_array_member(IN_OUT intptr_t arr[], IN intptr_t member, IN uint32_t pos, IN uint32_t arr_size)
{
	ENTRY

	if (pos < 1 || pos > arr_size) {
		LOGF_LOG_ERROR("Invalid position to insert [%u:%u]\n", pos, arr_size);
		return LTQ_FAIL;
	}

	memmove_s(&arr[pos], sizeof(int32_t)*(arr_size-pos), &arr[pos-1], sizeof(int32_t)*(arr_size-pos));
	arr[pos-1] = member;

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_is_member
 *  Description   : Internal API to check if engine is part engine list
 * ============================================================================*/
static int32_t
qosd_is_member(IN qos_engines_t arr[], IN uint32_t count, IN qos_engines_t member)
{
	uint32_t i = 0;
	int32_t ret = LTQ_FAIL;

	ENTRY
	for (i=0; i<count; i++) {
		if (arr[i] == member) {
			ret = LTQ_SUCCESS;
			goto exit;
		}
	}

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_remove_array_member
 *  Description   : Internal API to remove a member from given array
 * ============================================================================*/
static int32_t
qosd_remove_array_member(IN_OUT intptr_t arr[], IN intptr_t member, IN uint32_t arr_size)
{
	uint32_t i = 0;
	int32_t ret = LTQ_FAIL;
	ENTRY
	for (i=0; i<arr_size -1; i++) {
		if (arr[i] == member) {
			memmove_s(&arr[i], sizeof(int32_t)*(arr_size-1-i), &arr[i+1], sizeof(int32_t)*(arr_size-1-i));
			ret = LTQ_SUCCESS;
			goto exit;
		}
	}

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_add_engine_classifier
 *  Description   : Internal API to configure engine classifer
 * ============================================================================*/
static int32_t
qosd_add_engine_classifier(IN iftype_t iftype, IN ifinggrp_t ifinggrp, IN_OUT qos_class_node_t* classifier, IN qos_engines_t engine, IN uint32_t flags, IN uint32_t shifted_engines_count)
{
	qos_class_rule_t *engine_rule = NULL;
	int32_t ret = LTQ_SUCCESS;
	qos_engines_t ingress_engine = QOS_ENGINE_CPU;

	ENTRY
	UNUSED(flags);

	if (engine == QOS_ENGINE_MAX) {
		LOGF_LOG_ERROR("Invalid Engine Index [%d]. Value is more than max value [%d].\n", engine, QOS_ENGINE_MAX);
		return LTQ_FAIL;
	}

	LOGF_LOG_DEBUG("Adding classifier at engine %s \n",es_tables.eng_names[engine]);
	/* Add node in linked list for engine rule */
	ret = qosd_add_engine_rule(&classifier->engine_rules_list_head, &engine_rule);
	if (engine_rule == NULL) {
		LOGF_LOG_ERROR("Could not add engine rule\n");
		return LTQ_FAIL;
	}
	qosd_engine_init(engine, QOS_MODULE_CLASSIFIER);
	/* if Ingress classifier is set init the cpu engine also*/
	if (((classifier->class_cfg.action.flags & CL_ACTION_TC_INGRESS_EGRESS) || (classifier->class_cfg.action.flags & CL_ACTION_INGRESS_TC_SET)) && iftype != QOS_IF_CATEGORY_CPU)
		qosd_engine_init(ingress_engine, QOS_MODULE_CLASSIFIER);
	
	/* Set params of engine rule node */
	engine_rule->engine = engine;
	//engine_rule->order = 1 + es_tables.engine_info[engine].rules_count - shifted_engines_count;
	engine_rule->order = 1 + qosd_get_if_rule_count(es_tables.engine_info[engine].rules[iftype]) - shifted_engines_count;
	/* needed_capab has to divided among engines in case of partial classififcation. */
	engine_rule->flags = 0; // TODO consider partial classification
	engine_rule->parent = classifier;
	engine_rule->parent->class_cfg.intermediate_filter = 0; //TODO calculate, and how to pass this to engine? pass engine rule struct/define new struct?
	engine_rule->parent->class_cfg.intermediate_result = 0; //TODO
	LOGF_LOG_DEBUG("Adding Classifier at engine %s, at engine order %d \n",es_tables.eng_names[engine_rule->engine], engine_rule->order);
#if 0 /* enable when TC support is available. */
	if ((classifier->class_cfg.action.tc != 0) && is_q_present_for_tc(classifier->class_cfg.action.tc) != LTQ_SUCCESS)
	{
		LOGF_LOG_ERROR("Couldn't find a queue with same traffic class \n");
		return QOS_QUEUE_NOT_FOUND;
	}
#endif
	QOSD_ENGINE_CLASS_ADD(engine_rule->engine, iftype, ifinggrp, engine_rule->order, &engine_rule->parent->class_cfg, engine_rule->flags);
	qosd_insert_array_member((intptr_t *)es_tables.engine_info[engine].rules[iftype], (intptr_t) classifier, engine_rule->order, MAX_RULES);
	es_tables.engine_info[engine].rules[iftype][engine_rule->order-1] = classifier;
	es_tables.engine_info[engine].rules_count++;

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_change_engine_order
 *  Description   : Internal API to modify engine order of a classifer
 * ============================================================================*/
static int32_t
qosd_change_engine_order(IN iftype_t iftype, IN ifinggrp_t ifinggrp, IN_OUT qos_class_rule_t* engine_rule, IN int32_t delta)
{
	int32_t ret = LTQ_SUCCESS;

	ENTRY
	if(engine_rule->engine >= QOS_ENGINE_MAX) {
		LOGF_LOG_ERROR("Engine selected [%d] in qosd_change_engine_order is not valid\n", engine_rule->engine);
		return ret;
	}
	LOGF_LOG_DEBUG("Updating engine order of %s from %d to %d\n", engine_rule->parent->class_cfg.filter.name, engine_rule->order, engine_rule->order+delta);

	if (!(es_tables.engine_info[engine_rule->engine].engine.engine_capab.cl_cap.cap & CL_CAP_INS )) {
		LOGF_LOG_INFO("Engine %d doesn't have insert capab\n",engine_rule->engine+delta);
		/* Set the flag to indicate engine that del/add are called to update order only. if possible, engine can use this flag and optimize */
		engine_rule->flags |= CL_FILTER_ORDER_UPDATE;
		QOSD_ENGINE_CLASS_DEL(engine_rule->engine, iftype, ifinggrp, engine_rule->order, &engine_rule->parent->class_cfg, engine_rule->flags);
		QOSD_ENGINE_CLASS_ADD(engine_rule->engine, iftype, ifinggrp, engine_rule->order+delta, &engine_rule->parent->class_cfg, engine_rule->flags);
		engine_rule->flags &= ~CL_FILTER_ORDER_UPDATE;
	}
	if(engine_rule->order + delta > 0) {
		engine_rule->order += delta;
	}

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_change_engine_order_all
 *  Description   : Internal API to update engine order of all the classifiers at given engine
 * ============================================================================*/
static int32_t
qosd_change_engine_order_all(IN iftype_t iftype, IN ifinggrp_t ifinggrp, IN qos_engines_t engine, IN int32_t delta, IN uint32_t order, OUT uint32_t* shifted_engines_count)
{
	uint32_t l = 0;
	qos_class_rule_t *engine_rule = NULL;
	int32_t ret = LTQ_SUCCESS;

	ENTRY
	l = qosd_get_if_rule_count(es_tables.engine_info[engine].rules[iftype]);
	LOGF_LOG_DEBUG("%d rules are configured for given interface category at engine. \n",l);
	while (l > 0 && es_tables.engine_info[engine].rules[iftype][l-1]) {
		if (es_tables.engine_info[engine].rules[iftype][l-1]->class_cfg.filter.order >= order) {
			engine_rule = qosd_get_engine_node(es_tables.engine_info[engine].rules[iftype][l-1]->engine_rules_list_head, engine);
			if (engine_rule != NULL) {
				/* This check should be moved above to while loop*/
				qosd_change_engine_order(iftype, ifinggrp, engine_rule, delta);
				(*shifted_engines_count)++;
			}
		}
		l--;
	}
	LOGF_LOG_DEBUG("Updated engine order of %d classifers configured at %s\n",*shifted_engines_count,es_tables.eng_names[engine]);

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_get_classification_points
 *  Description   : Internal API to get classification point(s)
 * ============================================================================*/
static int32_t
qosd_get_classification_points(IN iftype_t iftype, OUT qos_class_cfg_t *class_cfg, IN_OUT qos_capab_cfg_t *needed_capab, IN uint32_t starting_point)
{
	int32_t dps[MAX_SYS_DATAPATHS] = {0};
	uint32_t i = 0;
	int32_t ret = LTQ_SUCCESS;
	ENTRY
	
	needed_capab->module = QOS_MODULE_CLASSIFIER;
	qosd_get_dps_for_iface(&es_tables, iftype, QOS_IF_CATEGORY_MAX, dps, QOS_DATAPATH_SLOW);

	memset_s(needed_capab->capab_cfg.cl_capab.cl_points, sizeof(needed_capab->capab_cfg.cl_capab.cl_points), 0xff);

	LOGF_LOG_DEBUG ("slow datapaths for interface [%d] are -->", iftype);
	QOSD_SHOW_DATAPATHS(dps, i);

	if (dps[0] == 0){
		LOGF_LOG_DEBUG ("No slow paths are available for this interface\n");
		ret = LTQ_FAIL;
		goto exit;
	}

	qosd_prepare_needed_cl_capab(class_cfg, iftype, &needed_capab->capab_cfg.cl_capab.cl_cap);

	qosd_get_action_point(dps, needed_capab, 1, starting_point);

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_show_classifers
 *  Description   : Debugging API to show classifiers configured in the system
 * ============================================================================*/
int32_t qosd_show_classifers ()
{
	struct list_head *list_node = NULL;
	struct list_head *sub_list_node = NULL;
	struct list_head *eng_list_node = NULL;
	qos_class_node_t *class_list_node = NULL;
	qos_class_node_t *sub_rules_list_node = NULL;
	qos_class_rule_t *engine_rule = NULL;
	uint32_t j=0, k=0;
	qos_engines_t i=QOS_ENGINE_PPA;
	struct list_head *if_list_tmp = NULL;
	qos_if_node_t *if_list_node = NULL;

	ENTRY
	// LOGF_LOG_DEBUG("\n\nclassifers configuration:, head is %p\n",es_tables.class_list_head);
	if (es_tables.class_list_head != NULL) {
		list_for_each(list_node, es_tables.class_list_head) {
			class_list_node = list_entry(list_node, qos_class_node_t, list);
			LOGF_LOG_INFO("\nClassifier name: %s, User Order: %d\n", class_list_node->class_cfg.filter.name, class_list_node->class_cfg.filter.order);
			LOGF_LOG_INFO("Divided into %d System classifiers\n",class_list_node->sub_rules_count);
			if(class_list_node->sub_rules_list_head) {
				list_for_each(sub_list_node, class_list_node->sub_rules_list_head) {
					sub_rules_list_node = list_entry(sub_list_node, qos_class_node_t, list);
					LOGF_LOG_INFO("\tSystem Order: %d\n", sub_rules_list_node->class_cfg.filter.order);
					LOGF_LOG_INFO("\tConfigured at %d Engines\n",sub_rules_list_node->engine_rules_count);
					if (sub_rules_list_node->engine_rules_list_head != NULL ) {
						list_for_each(eng_list_node, sub_rules_list_node->engine_rules_list_head){
							engine_rule = list_entry(eng_list_node, qos_class_rule_t, list);
							LOGF_LOG_INFO("\t\tEngine Order for %s: %d @ %s\n", sub_rules_list_node->class_cfg.filter.rx_if, engine_rule->order, es_tables.eng_names[engine_rule->engine]);
						}
					}
				}
			}
		}
	}

	LOGF_LOG_INFO("\nClassification configuration as per engine \n");
	for (i=0; i<QOS_ENGINE_MAX; i++) {
		if (es_tables.engine_info[i].rules_count){
			LOGF_LOG_INFO("Classifers for engine %d, rule count is %d: \n",i,es_tables.engine_info[i].rules_count);
		}
		else {
			continue;
		}
		j = 0;
		while (j<QOS_IF_CATEGORY_MAX) {
			if (es_tables.engine_info[i].rules[j][0]) {
				LOGF_LOG_INFO("\twith Iftype: %d\n",j);
			}
			k = 0;
			while ((k<MAX_RULES) && (es_tables.engine_info[i].rules[j][k] != NULL)) {
				LOGF_LOG_INFO("\t\tClassifer name is %s\n",es_tables.engine_info[i].rules[j][k]->class_cfg.filter.name);
				k++;
			}
			j++;
		}
	}

	LOGF_LOG_INFO("\nClassification configuration as per interface \n");
	if (es_tables.ingress_if_list_head != NULL ) {
		list_for_each(if_list_tmp, es_tables.ingress_if_list_head){
			if_list_node = list_entry(if_list_tmp, qos_if_node_t, list);
			/* If engine is used for classification */
			LOGF_LOG_INFO("\tFor Interface: %s\n",if_list_node->ifname);
			i = 0;
			while ((i<MAX_RULES) && (if_list_node->classifiers[i] != NULL)) {
				LOGF_LOG_INFO("\t\tClassifer name is %s\n",if_list_node->classifiers[i]->class_cfg.filter.name);
				i++;
			}
		}
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_del_classifier
 *  Description   : Internal API to delete classifer from engine, doesn't delete engine node
 * ============================================================================*/
static int32_t
qosd_del_classifier(IN iftype_t iftype, IN ifinggrp_t ifinggrp ,IN_OUT qos_class_node_t *classifier)
{
	struct list_head *list_node = NULL;
	qos_class_rule_t *engine_rule;
	int32_t ret = LTQ_SUCCESS;
	ENTRY

	list_for_each(list_node, classifier->engine_rules_list_head) {
		engine_rule = list_entry(list_node, qos_class_rule_t, list);
		LOGF_LOG_DEBUG("Deleting Classifier at engine %s\n",es_tables.eng_names[engine_rule->engine]);
		QOSD_ENGINE_CLASS_DEL(engine_rule->engine, iftype, ifinggrp, engine_rule->order, &engine_rule->parent->class_cfg, engine_rule->flags);
		/* Remove this classifier from engine */
		es_tables.engine_info[engine_rule->engine].rules_count--;
		qosd_remove_array_member((intptr_t*)es_tables.engine_info[engine_rule->engine].rules[iftype], (intptr_t) classifier, MAX_RULES);

		if (es_tables.engine_info[engine_rule->engine].rules_count == 0)
			qosd_engine_fini(engine_rule->engine, QOS_MODULE_CLASSIFIER);
	}

	classifier->classification_points[0] = 0;

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_add_classifier
 *  Description   : Internal API to add classifer to an engine. doesn't add engine node
 * ============================================================================*/
static int32_t
qosd_add_classifier(IN iftype_t iftype, IN ifinggrp_t ifinggrp, IN_OUT qos_class_node_t *classifier, IN qos_engines_t to[], IN uint32_t order)
{
	struct list_head *list_node = NULL;
	qos_class_rule_t *engine_rule;
	int32_t ret = LTQ_SUCCESS;
	uint32_t shifted_engines_count = 0, count = 0;
	ENTRY

	if (to[0] == QOS_ENGINE_MAX) {
		LOGF_LOG_ERROR("Invalid To Engine Index [%d]. Value is more than max value [%d].\n", to[0], QOS_ENGINE_MAX);
		return LTQ_FAIL;
	}

	list_for_each(list_node, classifier->engine_rules_list_head) {
		engine_rule = list_entry(list_node, qos_class_rule_t, list);
		engine_rule->engine = to[0];
		LOGF_LOG_DEBUG("Adding Classifier at engine %s\n",es_tables.eng_names[to[0]]);
		count = qosd_get_if_rule_count(es_tables.engine_info[to[0]].rules[iftype]);
		if (order != 0 && order <= count) {
			/* insert operation */
			qosd_change_engine_order_all(iftype, ifinggrp, to[0], 1, order, &shifted_engines_count);
			engine_rule->order = order;
			qosd_insert_array_member((intptr_t *)es_tables.engine_info[to[0]].rules[iftype], 0, engine_rule->order, MAX_RULES);
		}
		else {
			/* append operation */
			engine_rule->order = count + 1;
		}

		if (es_tables.engine_info[to[0]].rules_count == 0)
			qosd_engine_init(to[0], QOS_MODULE_CLASSIFIER);

		QOSD_ENGINE_CLASS_ADD(engine_rule->engine, iftype, ifinggrp, engine_rule->order, &engine_rule->parent->class_cfg, engine_rule->flags);
		es_tables.engine_info[to[0]].rules[iftype][engine_rule->order-1] = classifier;
		es_tables.engine_info[to[0]].rules_count++;
	}

	classifier->classification_points[0] = to[0];

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_move_classifier
 *  Description   : Internal API to move classifer from one engine to another
 * ============================================================================*/
static int32_t
qosd_move_classifier(IN iftype_t iftype, IN ifinggrp_t ifinggrp, IN_OUT qos_class_node_t *classifier, IN qos_engines_t to[], IN uint32_t order)
{
	ENTRY

	if (to[0] == QOS_ENGINE_MAX) {
		LOGF_LOG_DEBUG("Invalid To Engine Index [%d]. Value is more than max value [%d].\n", to[0], QOS_ENGINE_MAX);
		return LTQ_FAIL;
	}

	LOGF_LOG_DEBUG("Moving classifer from %s to %s with order [%d:%d]\n", es_tables.eng_names[classifier->classification_points[0]],es_tables.eng_names[to[0]], classifier->class_cfg.filter.order, order);
	qosd_del_classifier(iftype, ifinggrp, classifier);
	qosd_add_classifier(iftype, ifinggrp, classifier, to, order);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_move_classifier
 *  Description   : Internal API to move classifers from one engine to another,
 *			where in all classifiers are removed first and then added.
 *			Here the classifiers list has to be sorted in descending
 *			order using which deletion happens from lowest to highest
 *			order and additions happens in reverse manner.
 * ============================================================================*/
static int32_t
qosd_cfg_classifiers(IN iftype_t iftype, IN ifinggrp_t ifinggrp, IN_OUT qos_class_node_t *classifiers_to_move[], IN qos_engines_t to[], int32_t nClCnt, int32_t nOp)
{
	int32_t l = 0, nOrder = 1;
	ENTRY

	if (to[0] == QOS_ENGINE_MAX) {
		LOGF_LOG_DEBUG("Invalid To Engine Index [%d]. Value is more than max value [%d].\n", to[0], QOS_ENGINE_MAX);
		return LTQ_FAIL;
	}

	/* nOp : 1 - add, 2 - delete, 3 - modify */
	if(nOp == 2 || nOp == 3) {
		while(l<nClCnt) {
			if (classifiers_to_move[l]->classification_points[0] < to[0]){
				LOGF_LOG_DEBUG("Moving classifer from %s to %s with order [%d]\n", es_tables.eng_names[classifiers_to_move[l]->classification_points[0]],es_tables.eng_names[to[0]], classifiers_to_move[l]->class_cfg.filter.order);
				qosd_del_classifier(iftype, ifinggrp, classifiers_to_move[l]);
			}
			l++;
		}
	}

	if(nOp == 1 || nOp == 3) {
		l = nClCnt - 1;
		while(l >= 0) {
			/* if classifier to move is at earlier engine than the engine new classifer to be added on, move this classifer to new engine */
			if (classifiers_to_move[l]->classification_points[0] < to[0]){
				LOGF_LOG_DEBUG("Moving classifer from %s to %s with order [%d:%d]\n", es_tables.eng_names[classifiers_to_move[l]->classification_points[0]],es_tables.eng_names[to[0]], classifiers_to_move[l]->class_cfg.filter.order, l+1);
				qosd_add_classifier(iftype, ifinggrp, classifiers_to_move[l], to, nOrder++);
			}
			l--;
		}
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_delete_rules
 *  Description   : Internal API to delete all nodes for a classifier
 * ============================================================================*/
static int32_t
qosd_delete_rules(IN char *name, IN char *rx_if)
{
	struct list_head *sub_list_node = NULL;
	struct list_head *sub_list_node2 = NULL;
	struct list_head *head_temp = NULL;
	qos_class_node_t *sub_rules_list_node = NULL;
	qos_class_node_t *class_list_node = NULL;
	qos_class_node_t *class_node = NULL;
	struct list_head *list_node = NULL;
	int32_t ret = LTQ_SUCCESS;

	ENTRY
	class_list_node = qosd_get_class_node(es_tables.class_list_head, name, rx_if);

	if (class_list_node == NULL) {
		LOGF_LOG_ERROR("Classifier with name [%s] is not found on interface [%s]\n", name, (rx_if != NULL)?rx_if:"");
		return QOS_CLASSIFIER_NOT_FOUND; 
	}

	LOGF_LOG_DEBUG("Class node is %p, sub head %p \n",class_list_node,class_list_node->sub_rules_list_head);
	head_temp = class_list_node->sub_rules_list_head;
	list_for_each_safe(sub_list_node, sub_list_node2, head_temp) {
		sub_rules_list_node = list_entry(sub_list_node, qos_class_node_t, list);
		LOGF_LOG_DEBUG("Deleting sub node %p \n",sub_rules_list_node);
		ret = qosd_del_class_node(&class_list_node->sub_rules_list_head, sub_rules_list_node);
	}

	list_for_each(list_node, es_tables.class_list_head) {
		class_node = list_entry(list_node, qos_class_node_t, list);
		if ((strcmp (class_node->class_cfg.filter.rx_if, class_list_node->class_cfg.filter.rx_if) == 0)
				&& (class_node->class_cfg.filter.order > class_list_node->class_cfg.filter.order))
			class_node->class_cfg.filter.order -= 1;
	}

	LOGF_LOG_DEBUG("Deleting node %p \n",class_list_node);
	ret = qosd_del_class_node(&es_tables.class_list_head, class_list_node);
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_generate_rules
 *  Description   : API to generate rules for agiven classifier
 * ============================================================================*/
static int32_t
qosd_generate_rules(IN int32_t order, IN_OUT qos_class_cfg_t *class_cfg, OUT qos_class_node_t *if_classifiers[])
{
	qos_class_node_t *class_list_node = NULL;
	qos_class_node_t *class_node = NULL;
	struct list_head *list_node = NULL;
	qos_class_node_t *sub_rules_list_node = NULL;
	qos_if_node_t *if_list_node = NULL;
	int32_t ret = LTQ_SUCCESS;
	ENTRY

	/* TODO: Order has to be unfied, class node order, class cfg order, filter order */
	ret = qosd_add_class_node(&es_tables.class_list_head, &class_list_node);

	if (ret != LTQ_SUCCESS) {
		if (es_tables.class_list_head == NULL) {
			LOGF_LOG_ERROR("Could not inititalize list head\n");
			ret = LTQ_FAIL;
			goto exit;
		}

		if (class_list_node == NULL) {
			LOGF_LOG_ERROR("Could not add the class node\n");
			ret = QOS_CLASSIFIER_NOT_FOUND;
			goto exit;
		}
	}

	memcpy_s(&class_list_node->class_cfg, sizeof(qos_class_cfg_t), class_cfg, sizeof(qos_class_cfg_t));

	if_list_node = qosd_get_if_node(es_tables.ingress_if_list_head, class_cfg->filter.rx_if);
	if (if_list_node == NULL) {
		ret = qosd_add_if_node(&es_tables.ingress_if_list_head, class_cfg->filter.rx_if, &if_list_node);
		if(ret != LTQ_SUCCESS ) {
			LOGF_LOG_ERROR("Adding interface node for [%s] failed", class_cfg->filter.rx_if);
			ret = QOS_IF_ADD_FAIL;
			goto exit;
		}
		if(strncpy_s(if_list_node->ifname, MAX_IF_NAME_LEN, class_cfg->filter.rx_if, MAX_IF_NAME_LEN) != EOK) {
			LOGF_LOG_ERROR("Erroro copying ifname string\n");
			ret = QOS_IF_ADD_FAIL;
							goto exit;
		}
	}

	/* Set the user order of the classifer if order is not passed from user */
	if (order == 0 ) {
		LOGF_LOG_DEBUG("Classifer is configured with 0 order, so appending\n");
		class_cfg->filter.order = if_list_node->class_count + 1;
	}
	else {
		/* Update order of higher order classifiers */
		list_for_each(list_node, es_tables.class_list_head) {
			class_node = list_entry(list_node, qos_class_node_t, list);
			if ((strcmp (class_node->class_cfg.filter.rx_if, class_cfg->filter.rx_if) == 0)
					&& (class_node->class_cfg.filter.order >= class_cfg->filter.order)
					&& (class_node != class_list_node))
				class_node->class_cfg.filter.order += 1;
		}
	}

	class_list_node->class_cfg.filter.order = class_cfg->filter.order;
	ret = qosd_add_class_node(&class_list_node->sub_rules_list_head, &sub_rules_list_node);
	if (sub_rules_list_node != NULL) {
		/* Since we one system classifier per user classifier, system order = user order */
		memcpy_s(&sub_rules_list_node->class_cfg, sizeof(qos_class_cfg_t), &class_list_node->class_cfg, sizeof(qos_class_cfg_t));
		if_classifiers[0] = sub_rules_list_node;
		class_list_node->sub_rules_count = 1;
		LOGF_LOG_DEBUG("class node %p, sub_rules_list_node is %p, head %p\n",class_list_node, sub_rules_list_node,class_list_node->sub_rules_list_head);
	}

exit:
	EXIT
	return ret;
}
/* =============================================================================
 *  Function Name : qosd_get_classifier_by_order
 *  Description   : Internal API to get classifier at given order
 * ============================================================================*/
static int32_t
qosd_get_classifier_by_order(IN uint32_t order, IN qos_class_node_t *classifiers[], OUT qos_class_node_t **classifier)
{
	uint32_t i = 0;
	ENTRY

	LOGF_LOG_DEBUG("Fetching Classifers with system order %d\n",order);
	while ((i<MAX_RULES) && (classifiers[i] != NULL)) {
		if (classifiers[i]->class_cfg.filter.order == order) {
			*classifier = classifiers[i];
			break;
		}
		i++;
	}

	if((*classifier)) {
		LOGF_LOG_DEBUG("Classifier is %p, name %s\n",*classifier, (*classifier)->class_cfg.filter.name);
	} else {
		LOGF_LOG_DEBUG("No classifier found with system order %d\n", order);
	}

	EXIT
	return LTQ_SUCCESS;
}


/* =============================================================================
 *  Function Name : qosd_get_higher_order_classifers
 *  Description   : Internal API to get list of classifers configred at higher than order.
 *		    In order to identify higher order classifiers, user defined order is not used,
 *		    but engine order generated by ES based on user order is used.
 *		    Classifiers list would be sorted in ascending order of 'order'
 *		    field values. For example, 3 2 1.
 *		    This would avoid multiple re-ordering steps, when all
 *		    classifiers are deleted at once in sequence.
 * ============================================================================*/
static int32_t
qosd_get_higher_order_classifers(IN uint32_t order, IN qos_class_node_t *classifiers[], OUT qos_class_node_t *classifiers_to_move[], OUT int32_t *nClCnt)
{
	uint32_t i = 0, j = 0, k = 0;
	qos_class_node_t *class_tmp = NULL;
	ENTRY

	LOGF_LOG_DEBUG("Fetching Classifers with system order equal or more than %d\n",order);
	while ((i<MAX_RULES) && (classifiers[i] != NULL)) {
		/* does this handle scenario when classifier is split across multiple engines ? */
		if (classifiers[i]->class_cfg.filter.order >= order) {
			classifiers_to_move[k++] = classifiers[i];
		}
		i++;
	}

	if(k == 0) {
		goto exit_lbl;
	}

	*nClCnt = k;
	i = k - 1;
	/* bubble sort to re-order classifiers_tp_move list based on 'order' field values */
	while(i > 0) {
		j = 0;
		while(j < i) {
			if(classifiers_to_move[j+1]->class_cfg.filter.order > classifiers_to_move[j]->class_cfg.filter.order) {
				/* swap */
				class_tmp = classifiers_to_move[j];
				classifiers_to_move[j] = classifiers_to_move[j+1];
				classifiers_to_move[j+1] = class_tmp;
			}
			j++;
		}
		i--;
	}

exit_lbl:
	LOGF_LOG_DEBUG("%d classifers are configured at order %d or higher\n",*nClCnt,order);
	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_classifier_add
 *  Description   : Internal API to add a classifier in the system
 * ============================================================================*/
static int32_t
qosd_classifier_add(
	IN int32_t	order,
	IN qos_class_cfg_t *class_cfg,
	IN uint32_t 	flags)
{
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	qos_engines_t *cl_points = NULL;
	qos_if_node_t *if_list_node = NULL;
	qos_class_node_t *class_list_node = NULL;
	qos_class_node_t* if_classifiers[MAX_RULES] = {0};
	qos_class_node_t* classifiers_to_move[MAX_RULES] = {0};
	qos_capab_cfg_t needed_capab = {0};
	uint32_t starting_point = 0, shifted_engines_count = 0;
	uint32_t j=0, k=0;
	//qos_engines_t from = QOS_ENGINE_MAX, to = QOS_ENGINE_MAX;
	int32_t ret = LTQ_SUCCESS, nClCnt = 0;
	
	ENTRY
	UNUSED(order);
	UNUSED(flags);

        class_list_node = qosd_get_class_node(es_tables.class_list_head, class_cfg->filter.name,class_cfg->filter.rx_if);
        if (class_list_node != NULL) {
                LOGF_LOG_ERROR("Classifier add failed. Given classifier with name [%s] is already configured on specified interface %s\n",
					class_cfg->filter.name, class_cfg->filter.rx_if);
                ret = QOS_ENTRY_EXISTS;
                goto exit1;
        }

	LOGF_LOG_DEBUG("Adding classifier with order %d, name %s, and interface %s\n\n",class_cfg->filter.order, class_cfg->filter.name, class_cfg->filter.rx_if);
	ret = qosd_generate_rules(class_cfg->filter.order, class_cfg, if_classifiers);

	if (ret != LTQ_SUCCESS ) {
		LOGF_LOG_ERROR("Classifier rule generation failed\n");
		goto exit;
	}

	/* Assuming sub-classifiers will be generated for same interface, else this code has to put in while loop below also */
	if_list_node = qosd_get_if_node(es_tables.ingress_if_list_head, class_cfg->filter.rx_if);
	if (if_list_node == NULL) {
		LOGF_LOG_ERROR("Interface node for [%s] is not added by rule generator\n", class_cfg->filter.rx_if);
		ret = LTQ_FAIL;
		goto exit;
	}

	while (if_classifiers[j]) {
		/* Get interface node for the interface classifer is added to */
		QOSD_GET_IFTYPE(if_classifiers[j]->class_cfg.filter.rx_if, es_tables.ifmap);
		if (if_classifiers[j]->class_cfg.action.flags & CL_ACTION_TC_INGRESS)
			QOSD_GET_IFINGGRP(if_classifiers[j]->class_cfg.filter.tx_if, es_tables.ifinggrp_map);

		/* If classifers for a interface are configured at multiple engines, higher order classifer can not be added at lower level engines */
		if (if_list_node->common_cl_points_count > 0) {
			/* correct starting ppoint calculation in case of insert */
			if (if_classifiers[j]->class_cfg.filter.order > if_list_node->class_count) {
				starting_point = if_list_node->common_cl_points[if_list_node->common_cl_points_count-1] + 1;
			}
			else {
				/* get list of classifiers for order needs to be changes because of insert operation */
				if (if_classifiers[j]->class_cfg.filter.order > 1) {
					qosd_get_classifier_by_order(if_classifiers[j]->class_cfg.filter.order - 1, if_list_node->classifiers, &classifiers_to_move[0]);
					if(classifiers_to_move[0]) {
						/* Setting starting point of new classifiew same as classifier with lower order */
						starting_point = classifiers_to_move[0]->classification_points[0] + 1;
					} else {
						/* Setting starting point based on common_cl_points of interface */
						starting_point = if_list_node->common_cl_points[if_list_node->common_cl_points_count-1] + 1;
					}
				}
				qosd_get_higher_order_classifers(if_classifiers[j]->class_cfg.filter.order, if_list_node->classifiers, classifiers_to_move, &nClCnt);
			}
			//from = if_list_node->common_cl_points[if_list_node->common_cl_points_count-1];
		}
		else {
			memset_s(if_list_node->common_cl_points, sizeof(qos_engines_t)*QOS_ENGINE_MAX, 0xff);
		}

		/* Get classification point to configure classifer */
		LOGF_LOG_DEBUG("Starting point is %d\n", starting_point);
		qosd_get_classification_points(iftype, &if_classifiers[j]->class_cfg, &needed_capab, starting_point);
		if (needed_capab.capab_cfg.cl_capab.cl_points[0] == 0xffffffff) {
			LOGF_LOG_ERROR("Given classifier [%s] can not be added\n", class_cfg->filter.name);
			ret = LTQ_FAIL;
			/*TODO Cleanup*/
			goto exit;
		}
		else {
			cl_points = needed_capab.capab_cfg.cl_capab.cl_points;
		}

		/* Copy classification point list in class node, list will have only one entry unles classifier isconfigured partially at multiple engines */
		memcpy_s(if_classifiers[j]->classification_points, sizeof(qos_engines_t)*QOS_ENGINE_MAX, cl_points, sizeof(qos_engines_t)*QOS_ENGINE_MAX);

		if (if_list_node->class_count == 0 ) {
			if_list_node->common_cl_points[0] = cl_points[0];
			if_list_node->common_cl_points_count = 1;
			LOGF_LOG_DEBUG("Adding first classifer for this interface, common cl point is %d\n",if_list_node->common_cl_points[0]);
		}
		else if (if_list_node->class_count > 0 ) {
			/* update classification point for the classifier to be added as higher weighted classifier should come at same/later stage (lower weight) only */
			LOGF_LOG_DEBUG("Multiple classifers are configured for this interface, common_cl_points_count is %d\n",if_list_node->common_cl_points_count);
			LOGF_LOG_DEBUG("Common cl point is %d, adding new classifer at %d\n",if_list_node->common_cl_points[if_list_node->common_cl_points_count-1], cl_points[0]);
			if (qosd_is_member(if_list_node->common_cl_points, if_list_node->common_cl_points_count, cl_points[0]) != LTQ_SUCCESS) {
				//if (es_tables.engine_info[cl_points[0]].engine.used_resource < es_tables.engine_info[if_list_node->common_cl_points[0]].engine.used_resource)
				LOGF_LOG_DEBUG("Adding classifier at next engine\n");
				if_list_node->common_cl_points[if_list_node->common_cl_points_count] = cl_points[0];
				if_list_node->common_cl_points_count++;
			}
			else {
				LOGF_LOG_DEBUG("Adding classifier at same engine\n");
			}

			if (classifiers_to_move[0] == NULL) {
				LOGF_LOG_INFO("No need to update order of any classifers configured at %s interface\n",if_classifiers[j]->class_cfg.filter.rx_if);
			}
			else {
				LOGF_LOG_DEBUG("Updating System order of classifers configured at order %d or higher\n", if_classifiers[j]->class_cfg.filter.order);
				qosd_cfg_classifiers(iftype, ifinggrp, classifiers_to_move, cl_points, nClCnt, 3);
				LOGF_LOG_DEBUG("Updated System order of %d classifers\n",nClCnt);

				/* Move the classifers configured at same engine and configured at higher order */
				shifted_engines_count = 0;
				LOGF_LOG_DEBUG("Engine id  is %d\n",cl_points[0]);
				qosd_change_engine_order_all(iftype, ifinggrp, cl_points[0], 1, if_classifiers[j]->class_cfg.filter.order, &shifted_engines_count);
			}
		}

		/* Loop through number of engines needed to configure given classifer */
		while (k < QOS_ENGINE_MAX && cl_points[k] != 0xffffffff) { /* Should we limit number of engines to configure a classifier partially */
			ret = qosd_add_engine_classifier(iftype, ifinggrp, if_classifiers[j], cl_points[k], needed_capab.capab_cfg.cl_capab.cl_cap.cap, shifted_engines_count);
			if (ret != LTQ_SUCCESS) {
				goto exit;
			}
			k++;
		}
		if_classifiers[j]->engine_rules_count = k;

		/* Add class node pointer in interfcae list node */
		if_list_node->classifiers[if_list_node->class_count] = if_classifiers[j];
		if_list_node->class_count++;

#if 0
		to = if_list_node->common_cl_points[if_list_node->common_cl_points_count-1];

		LOGF_LOG_DEBUG("Old highest Classification point is %d, new is %d\n", from, to);
		if (from != to)
			qosd_update_datapaths_status(iftype, 1, from, to);
#endif

		j++;
	}

exit:
	if (ret != LTQ_SUCCESS)
		qosd_delete_rules(class_cfg->filter.name,class_cfg->filter.rx_if);
exit1:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_classifier_del
 *  Description   : Internal API to delete classifier from the system
 * ============================================================================*/
static int32_t
qosd_classifier_del(
	IN int32_t	order,
	IN qos_class_cfg_t *class_cfg,
	IN uint32_t 	flags)
{
	qos_if_node_t *if_list_node = NULL;
	qos_if_node_t *cl_if_list_node = NULL;
	qos_class_node_t* if_classifiers[MAX_RULES] = {0};
	qos_class_rule_t *engine_rule = NULL;
	qos_class_node_t *class_list_node = NULL;
	qos_class_node_t *class_node = NULL;
	qos_class_node_t* classifiers_to_move[MAX_RULES] = {0}, *classifiers_to_add[MAX_RULES] = {0};
	qos_engines_t *cl_points;
	qos_engines_t engine = 0;
	qos_capab_cfg_t needed_capab = {0};
	iftype_t iftype = QOS_IF_CATEGORY_MAX, cl_iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	struct list_head *list_tmp = NULL;
	uint32_t no_move = 0, order_loc =0, engine_order_loc =0;
	struct list_head *list_node = NULL;
	int32_t j=0,l=0,k=0;
	//qos_engines_t from = QOS_ENGINE_MAX, to = QOS_ENGINE_MAX;
	int32_t ret = LTQ_SUCCESS, nClCnt = 0;
	uint32_t i;
	bool classifiers_present = false;
	
	ENTRY
	UNUSED(order);
	UNUSED(flags);
	class_list_node = qosd_get_class_node(es_tables.class_list_head, class_cfg->filter.name,class_cfg->filter.rx_if);
	if (class_list_node == NULL) {
		LOGF_LOG_ERROR("Given classifier with name [%s] not configured on specified interface %s\n", class_cfg->filter.name, class_cfg->filter.rx_if);
		ret = QOS_CLASSIFIER_NOT_FOUND;
		goto exit;
	}
	else {
		LOGF_LOG_DEBUG("Deleting classifier with order %d, name %s, and interface %s\n\n",class_list_node->class_cfg.filter.order,
							class_list_node->class_cfg.filter.name, class_list_node->class_cfg.filter.rx_if);
		/* TODO: get list of sub classifiers in case it's an abstract classifer */
		if_classifiers[0] = qosd_get_class_node(class_list_node->sub_rules_list_head, class_cfg->filter.name,class_cfg->filter.rx_if);
	}

	if_list_node = qosd_get_if_node(es_tables.ingress_if_list_head, class_cfg->filter.rx_if);
	if (if_list_node == NULL) {
		LOGF_LOG_ERROR("No classifers exist for interface [%s]", class_cfg->filter.rx_if);
		ret = LTQ_FAIL;
		goto exit;
	}

	while (if_classifiers[j]) {
		LOGF_LOG_DEBUG("System order of classifier is %d\n",if_classifiers[j]->class_cfg.filter.order);

		/* Validate if_classifiers[j] is present at if_list_node->classifiers[] */
		QOSD_GET_IFTYPE(if_classifiers[j]->class_cfg.filter.rx_if, es_tables.ifmap);
		if (if_classifiers[j]->class_cfg.action.flags & CL_ACTION_TC_INGRESS)
			QOSD_GET_IFINGGRP(if_classifiers[j]->class_cfg.filter.tx_if, es_tables.ifinggrp_map);
		//from = if_list_node->common_cl_points[if_list_node->common_cl_points_count-1];
		if (if_classifiers[j]->engine_rules_list_head == NULL ) {
			LOGF_LOG_DEBUG("Engine rules are NOT configured for this classifier \n");
		}
		else {
			list_for_each(list_node, if_classifiers[j]->engine_rules_list_head){
				engine_rule = list_entry(list_node, qos_class_rule_t, list);
				LOGF_LOG_DEBUG("Deleting Classifier at engine %s with order [%d]\n",es_tables.eng_names[engine_rule->engine], engine_rule->order);
				engine_rule->parent->class_cfg.action.iflags=class_cfg->action.iflags;
				QOSD_ENGINE_CLASS_DEL(engine_rule->engine, iftype, ifinggrp, engine_rule->order, &engine_rule->parent->class_cfg, engine_rule->flags);
				engine = engine_rule->engine;
				engine_order_loc = engine_rule->order;
				es_tables.engine_info[engine].rules_count--;
				qosd_remove_array_member((intptr_t*)es_tables.engine_info[engine].rules[iftype], (intptr_t) if_classifiers[j], MAX_RULES);
			}
			/* If partial classification is supprted, call this api for all rules*/
			qosd_del_engine_rule(&if_classifiers[j]->engine_rules_list_head, engine_rule);
		}

		/* Remove this classifier from if_list_node->classifiers[] */
		qosd_remove_array_member((intptr_t*)if_list_node->classifiers, (intptr_t) if_classifiers[j], MAX_RULES);

		/* Delete class node pointer from sub rule list of the clssifer node */
		order_loc = if_classifiers[j]->class_cfg.filter.order;
		ret = qosd_del_class_node(&class_list_node->sub_rules_list_head, if_classifiers[j]);
		if_list_node->class_count--;

		/* assumed that engine handles classifiers of iftype in single group
		 * i.e. c1 with order 1 on br-lan having iftype QOS_IF_CATEGORY_ETHLAN, c2 with order 2 on eth0_1 having iftype QOS_IF_CATEGORY_ETHLAN
		 *	in CPU engine would be configured under LAN_CL chain
		 *	in PPA/switch engine would be configured with same portid/group
		 * so for any change of order in higher order classifier, all classifiers with lower order on other interfaces
		 * but of same iftype have to be re-configured to update order
		 */
		cl_iftype = iftype;
		list_for_each(list_tmp, es_tables.ingress_if_list_head) {
			cl_if_list_node = list_entry(list_tmp, qos_if_node_t, list);
			QOSD_GET_IFTYPE(cl_if_list_node->ifname, es_tables.ifmap);
			if (cl_if_list_node->ingress) 
				QOSD_GET_IFINGGRP(cl_if_list_node->ifname, es_tables.ifinggrp_map);
			if(iftype == cl_iftype) {
				no_move = 0;
				/* Check if we need to move any classifier */
				if (cl_if_list_node->class_count > 0) {
					for(l=0; l<MAX_RULES; l++) {
						classifiers_to_move[l] = NULL;
					}
					/* get list of classifiers for order needs to be changes because of delete operation */
					qosd_get_higher_order_classifers(order_loc, cl_if_list_node->classifiers, classifiers_to_move, &nClCnt);

					l = 0;
					while (classifiers_to_move[l] != NULL) {
					/* Update order of all the classifers in class node */
						LOGF_LOG_DEBUG("Updating system order of %s from %d to %d\n",classifiers_to_move[l]->class_cfg.filter.name,
									classifiers_to_move[l]->class_cfg.filter.order,classifiers_to_move[l]->class_cfg.filter.order-1);
						/*If some rules remains on the engine then update those */
						LOGF_LOG_DEBUG("Rule deleted with engine order %d, common cl point count is %d\n",engine_order_loc,cl_if_list_node->common_cl_points_count);
						if (engine_order_loc == 1) {
							/* Consider starting point to get classification point */
							qosd_get_classification_points(iftype, &classifiers_to_move[l]->class_cfg, &needed_capab, 0);
							cl_points = needed_capab.capab_cfg.cl_capab.cl_points;
							LOGF_LOG_DEBUG("old cl point: %d, new cl point for this class is %d, cl got deleted from engine %d\n",
												classifiers_to_move[l]->classification_points[0], cl_points[0], engine);
							if (no_move == 0 && cl_points[0] != classifiers_to_move[l]->classification_points[0]) {
								/* do only delete here, add should be done later
								 * starting with highest order.
								 */
								classifiers_to_add[l] = classifiers_to_move[l];
								qosd_del_classifier(iftype, ifinggrp, classifiers_to_move[l]);
							}
							else {
								no_move = 1;
							}
						}
						if ((no_move == 1 || engine_order_loc != 1)
								&& (engine == classifiers_to_move[l]->classification_points[0])) {
							//update engine order
							list_for_each(list_node, classifiers_to_move[l]->engine_rules_list_head){
								engine_rule = list_entry(list_node, qos_class_rule_t, list);
								/* There should be engine check also once partial classififation is supported */
								qosd_change_engine_order(iftype, ifinggrp, engine_rule, -1);
							}
						}
						l++;
					}
					l = MAX_RULES - 1;
					k = 0;
					/* add classifiers in descending order of 'order' field values */
					while(l >= 0) {
						if(classifiers_to_add[l] != NULL) {
							qosd_get_classification_points(iftype, &classifiers_to_add[l]->class_cfg, &needed_capab, 0);
							cl_points = needed_capab.capab_cfg.cl_capab.cl_points;
							qosd_add_classifier(iftype, ifinggrp, classifiers_to_add[l], cl_points, k+1);
						}
						l--;
						k++;
					}
				}
			}
		}

		/* Check if any classifier configured in PCE to avoid deleting matching
		 * iptable rule for the DS_QOS rule in PCE.
		 */
		for (i=0; i<QOS_ENGINE_MAX; i++) {
			if (es_tables.engine_info[i].rules_count){
				classifiers_present = true;
				LOGF_LOG_DEBUG("Classifiers present in engine:[%d]\n", es_tables.engine_info[i].engine.engine_id);
				break;
			}
		}

		if (es_tables.engine_info[engine].rules_count == 0) {
			LOGF_LOG_DEBUG("No classifiers left on engine:[%d]\n", engine);
			if (classifiers_present == false)
				qosd_engine_fini(engine, QOS_MODULE_CLASSIFIER);
			qosd_remove_array_member((intptr_t*)if_list_node->common_cl_points, (intptr_t) engine, QOS_ENGINE_MAX);
			if_list_node->common_cl_points_count--;
		}
		
#if 0
		if (if_list_node->common_cl_points_count > 0)
			to = if_list_node->common_cl_points[if_list_node->common_cl_points_count-1];

		LOGF_LOG_DEBUG("Old highest Classification point is %d, new is %d\n", from, to);
		if (from != to)
			qosd_update_datapaths_status(iftype, 1, from, to);
#endif
		j++;
	}
	
	list_for_each(list_node, es_tables.class_list_head) {
		class_node = list_entry(list_node, qos_class_node_t, list);
		if ((strcmp (class_node->class_cfg.filter.rx_if, class_list_node->class_cfg.filter.rx_if) == 0)
				&& (class_node->class_cfg.filter.order > class_list_node->class_cfg.filter.order))
			class_node->class_cfg.filter.order -= 1;
	}

	/* Delete class node pointer from es_tables.class_list_head */
	ret = qosd_del_class_node(&es_tables.class_list_head, class_list_node);

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_classifier_modify
 *  Description   : Internal API to modify a configured classifier
 * ============================================================================*/
static int32_t
qosd_classifier_modify(
	IN int32_t	order,
	IN qos_class_cfg_t *class_cfg,
	IN uint32_t 		flags)
{
	qos_if_node_t *if_list_node = NULL;
	qos_class_node_t* if_classifiers[MAX_RULES] = {0};
	qos_class_rule_t *engine_rule = NULL;
	qos_class_node_t *class_list_node = NULL;
	qos_class_node_t* classifiers_to_move[MAX_RULES] = {0};
	qos_engines_t *cl_points;
	qos_engines_t engine = 0;
	qos_capab_cfg_t needed_capab = {0};
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	uint32_t no_move = 0;
	struct list_head *list_node = NULL;
	uint32_t j=0,l=0, k=0;
	uint32_t starting_point = 0;
	//qos_engines_t from = QOS_ENGINE_MAX, to = QOS_ENGINE_MAX;
	int32_t ret = LTQ_SUCCESS, nClCnt = 0;
	int32_t i;
	bool classifiers_present = false;
	
	ENTRY
	UNUSED(order);

	class_list_node = qosd_get_class_node(es_tables.class_list_head, class_cfg->filter.name,class_cfg->filter.rx_if);
	if (class_list_node == NULL) {
		LOGF_LOG_ERROR("Given classifier with name [%s] not configured on specified interface %s\n", class_cfg->filter.name, class_cfg->filter.rx_if);
		ret = QOS_CLASSIFIER_NOT_FOUND;
		goto exit;
	}
	else {
		LOGF_LOG_DEBUG("Modifying classifier with order %d, name %s, and interface %s\n\n",class_list_node->class_cfg.filter.order,
							class_list_node->class_cfg.filter.name, class_list_node->class_cfg.filter.rx_if);
		/* TODO: If it's an abstract classifer, need to check if all sub rules are changed completely, in that case delete/add can be called */
		if_classifiers[0] = qosd_get_class_node(class_list_node->sub_rules_list_head, class_cfg->filter.name,class_cfg->filter.rx_if);
		/* Call Delete and Add if it's order update */
		if (class_cfg->filter.order > 0 && class_cfg->filter.order != class_list_node->class_cfg.filter.order) {
			LOGF_LOG_DEBUG("Order update request: Deleting with old order: %d and adding with new order:%d\n", class_list_node->class_cfg.filter.order, class_cfg->filter.order);
			qosd_classifier_del(class_list_node->class_cfg.filter.order, &class_list_node->class_cfg, flags);
			qosd_classifier_add(class_cfg->filter.order, class_cfg, flags);
			goto exit;
		}
	}

	while (if_classifiers[j]) {
		LOGF_LOG_DEBUG("System order of classifier is %d\n",if_classifiers[j]->class_cfg.filter.order);
		if_list_node = qosd_get_if_node(es_tables.ingress_if_list_head, if_classifiers[j]->class_cfg.filter.rx_if);
		if (if_list_node == NULL) {
			LOGF_LOG_DEBUG("No classifers exist for interface [%s]", if_classifiers[j]->class_cfg.filter.rx_if);
			ret = LTQ_FAIL;
			goto exit;
		}

		/* Validate if_classifiers[j] is present at if_list_node->classifiers[] */
		QOSD_GET_IFTYPE(if_classifiers[j]->class_cfg.filter.rx_if, es_tables.ifmap);
		if (if_classifiers[j]->class_cfg.action.flags & CL_ACTION_TC_INGRESS)
			QOSD_GET_IFINGGRP(if_classifiers[j]->class_cfg.filter.tx_if, es_tables.ifinggrp_map);

		engine_rule = qosd_get_engine_node(if_classifiers[j]->engine_rules_list_head, if_classifiers[j]->classification_points[0]);
		if(engine_rule == NULL) {
			LOGF_LOG_ERROR("Could not generate classifier rule.\n");
			ret = LTQ_FAIL;
			goto exit;
		}
		/* If classifers for a interface are configured at multiple engines, higher order classifer can not be added at lower level engines */
		if (if_list_node->common_cl_points_count > 1) {
			/* If it's first classifier on the engine and engine is not first classification point for the interface, then starting point = prev cl point */
			if (if_classifiers[j]->classification_points[0] != if_list_node->common_cl_points[0] && engine_rule->order == 1 ) {
				for (k=1; k < if_list_node->common_cl_points_count; k++) {
					starting_point = if_list_node->common_cl_points[k-1] + 1;
					if (if_list_node->common_cl_points[k] == if_classifiers[j]->classification_points[0])
						break;
				}
			}
			else {
				starting_point = if_classifiers[j]->classification_points[0] + 1;
			}
		}
		else {
			/* if classifier to modify is first then consider all engines, ie. starting_point=0 */
			if (engine_rule->order == 1 )
				starting_point = 0;
			else
				starting_point = if_list_node->common_cl_points[0] + 1;
		}

		/* Get classification point to configure classifer */
		engine = if_classifiers[j]->classification_points[0];
		qosd_get_classification_points(iftype, class_cfg, &needed_capab, starting_point);
			cl_points = needed_capab.capab_cfg.cl_capab.cl_points;

		/* classifier node should not be updated at the time of calling engine modify*/
		class_cfg->filter.order = if_classifiers[j]->class_cfg.filter.order;
		if (cl_points[0] == if_classifiers[j]->classification_points[0]) {
			/* Call engine modify callback */
			LOGF_LOG_DEBUG("No change in cl points, Call engine modify callback\n");
			QOSD_ENGINE_CLASS_MODIFY(engine_rule->engine, iftype, ifinggrp, if_classifiers[j]->class_cfg.filter.order, &if_classifiers[j]->class_cfg, engine_rule->order, class_cfg, engine_rule->flags);
			memcpy_s(&if_classifiers[j]->class_cfg, sizeof(qos_class_cfg_t), class_cfg, sizeof(qos_class_cfg_t));
		}
		else if (cl_points[0] > if_classifiers[j]->classification_points[0]) {
			LOGF_LOG_DEBUG("cl points advanced \n");
			/* Move this classifier and other higher order classifiers to new engine. if we are moving some classifiiers to higher engine,
					these have to be inserted before existing rules */
			l = 0;
			/* Check if classifier is added at new engine */
			if (es_tables.engine_info[cl_points[0]].rules_count == 1) {
				if_list_node->common_cl_points[if_list_node->common_cl_points_count] = cl_points[0];
				if_list_node->common_cl_points_count++;
			}
			qosd_get_higher_order_classifers(if_classifiers[j]->class_cfg.filter.order, if_list_node->classifiers, classifiers_to_move, &nClCnt);
			while (classifiers_to_move[l] != NULL) {
				if (cl_points[0] != classifiers_to_move[l]->classification_points[0] &&
					classifiers_to_move[l]->class_cfg.filter.order != if_classifiers[j]->class_cfg.filter.order) {
					qosd_move_classifier(iftype, ifinggrp, classifiers_to_move[l], cl_points, nClCnt);
				}
				else {
					break;
				}
				nClCnt--;
				l++;
			}
			qosd_del_classifier(iftype, ifinggrp, if_classifiers[j]);
			memcpy_s(&if_classifiers[j]->class_cfg, sizeof(qos_class_cfg_t), class_cfg, sizeof(qos_class_cfg_t));
			qosd_add_classifier(iftype, ifinggrp, if_classifiers[j], cl_points, 1);
		}
		else if (cl_points[0] < if_classifiers[j]->classification_points[0]) {
			LOGF_LOG_DEBUG("cl points retreated \n");
			/* If given classifier is at first order of old engine, move it back and check if further classifiers configured on old engine can be moved back */
			if (engine_rule->order == 1 ) {
				qosd_del_classifier(iftype, ifinggrp, if_classifiers[j]);
				memcpy_s(&if_classifiers[j]->class_cfg, sizeof(qos_class_cfg_t), class_cfg, sizeof(qos_class_cfg_t));
				qosd_add_classifier(iftype, ifinggrp, if_classifiers[j], cl_points, 0);
				if (es_tables.engine_info[cl_points[0]].rules_count == 1) {
					qosd_insert_array_member((intptr_t *)if_list_node->common_cl_points, (intptr_t)cl_points[0], if_list_node->common_cl_points_count, QOS_ENGINE_MAX);
					if_list_node->common_cl_points_count++;
				}
				memset_s(classifiers_to_move, sizeof(classifiers_to_move), 0);
				qosd_get_higher_order_classifers(if_classifiers[j]->class_cfg.filter.order, if_list_node->classifiers, classifiers_to_move, &nClCnt);
				l = 0;
				while (classifiers_to_move[l] != NULL) {
					/* Get classification point to configure classifer */
					qosd_get_classification_points(iftype, &classifiers_to_move[l]->class_cfg, &needed_capab, starting_point);
						cl_points = needed_capab.capab_cfg.cl_capab.cl_points;
					if (no_move == 0 && cl_points[0] != classifiers_to_move[l]->classification_points[0]) {
						LOGF_LOG_DEBUG("Moving Classifier from %d to %d\n",classifiers_to_move[l]->classification_points[0], cl_points[0]);
						/* pass apropriate order to move api */
						qosd_move_classifier(iftype, ifinggrp, classifiers_to_move[l], cl_points, classifiers_to_move[l]->class_cfg.filter.order);
					}
					else {
						//update engine order
						LOGF_LOG_DEBUG("Updating order of classfier \n");
						no_move = 1;
						list_for_each(list_node, classifiers_to_move[l]->engine_rules_list_head){
							engine_rule = list_entry(list_node, qos_class_rule_t, list);
							/* There should be engine check also once partial classififation is supported */
							qosd_change_engine_order(iftype, ifinggrp, engine_rule, -1);
						}
					}
					l++;
				}
			}
		}

		/* Check if any classifier configured in PCE to avoid deleting matching
		 * iptable rule for the DS_QOS rule in PCE.
		 */
		for (i=0; i<QOS_ENGINE_MAX; i++) {
			if (es_tables.engine_info[i].rules_count){
				classifiers_present = true;
				LOGF_LOG_DEBUG("Classifiers present in engine:[%d]\n", es_tables.engine_info[i].engine.engine_id);
				break;
			}
		}

		/* Decrease common cl point count if all rules are moved out from an engine */
		if (es_tables.engine_info[engine].rules_count == 0) {
			LOGF_LOG_DEBUG("No classifiers left on engine:[%d]\n", engine);
			if (classifiers_present == false)
				qosd_engine_fini(engine, QOS_MODULE_CLASSIFIER);
			qosd_remove_array_member((intptr_t*)if_list_node->common_cl_points, (intptr_t) engine, QOS_ENGINE_MAX);
			if_list_node->common_cl_points_count--;
		}

		j++;
	}

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : 
 *  Description   : FAPI to get list of classifiers 
 * ============================================================================*/
int32_t
qosd_classifier_get(
	IN uint32_t	order,
	OUT int32_t	*num_entries,
	OUT qos_class_cfg_t **class_cfg,
	IN uint32_t	flags)
{
	struct list_head *list_node = NULL;
	qos_class_node_t *class_list_node = NULL;

	ENTRY
	UNUSED(flags);
	*num_entries = 0;
	LOGF_LOG_DEBUG("\n\nFetching classifiers configuration for order=%d\n",order);
	if (es_tables.class_list_head == NULL) {
		LOGF_LOG_INFO("No classifiers are configured\n");
		*num_entries = 0;
		return LTQ_SUCCESS;
	}

	list_for_each(list_node, es_tables.class_list_head) {
		class_list_node = list_entry(list_node, qos_class_node_t, list);
		LOGF_LOG_DEBUG("\nClassifier name: %s, User Order: %d\n", class_list_node->class_cfg.filter.name, class_list_node->class_cfg.filter.order);
		if (order == 0 ) {
			*num_entries += 1;
			*class_cfg = (qos_class_cfg_t *) realloc((void *) *class_cfg, sizeof(qos_class_cfg_t)*(*num_entries)); //change it to os_realloc
			if (*class_cfg == NULL ) {
				LOGF_LOG_CRITICAL("Could not get memory for [%lu]bytes\n", (long unsigned int) sizeof(qos_class_cfg_t)*(*num_entries));
				break;
			}
			memcpy_s((*class_cfg + *num_entries-1), sizeof(qos_class_cfg_t), &class_list_node->class_cfg, sizeof(qos_class_cfg_t));
		}
		else if (class_list_node->class_cfg.filter.order == order ) {
			*num_entries = 1;
			*class_cfg = (qos_class_cfg_t *) os_calloc(1, sizeof(qos_class_cfg_t));
			memcpy_s(class_cfg[*num_entries-1], sizeof(qos_class_cfg_t), &class_list_node->class_cfg, sizeof(qos_class_cfg_t));
			break;
		}
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_classifier_set
 *  Description   : FAPI to set (add/modify) a classifeier
 * ============================================================================*/
int32_t
qosd_classifier_set(
	IN int32_t			order,
	IN qos_class_cfg_t *class_cfg,
	IN uint32_t 		flags)
{
	int32_t ret = LTQ_SUCCESS;
	//iftype_t iftype = QOS_IF_CATEGORY_MAX;
	
	ENTRY
	//if (filter->rx_if == NULL) {
		// get list of interfaces and calculate list of engines needed so that traffic from all the interfaces can get classified. 
	//}
	if (flags & CL_FILTER_ADD) {
		ret = qosd_classifier_add(order, class_cfg, flags);
	}
	else if (flags & CL_FILTER_DELETE) {
		qosd_classifier_del(order, class_cfg, flags);
	}
	else if (flags & CL_FILTER_MODIFY) {
		qosd_classifier_modify(order, class_cfg, flags);
	}

	/* Show Configured classifiers */
	qosd_show_classifers();
	EXIT
	return ret;
}


/* =============================================================================
 *  Function Name : validate_q_rate 
 *  Description   : Checks if sum of CR is greater than line rate 
 * ============================================================================*/

int32_t validate_q_rate(IN char *ifname, IN iftype_t new_q_iftype, IN uint32_t new_commit_rate, IN uint32_t new_peak_rate)
{
	
        struct list_head *list_node = NULL;
        qos_queue_node_t *q_list_node = NULL;
	struct list_head *if_list_tmp = NULL;
	struct list_head *if_list_head = NULL;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	uint32_t commit_rate = 0, port_rate = 0;
	int32_t  i=0, ret = LTQ_SUCCESS;
	qos_if_node_t *if_list_node = NULL; 
	qos_shaper_t            shaper;
	
	ENTRY	

	while(i <= 1) {
		/* need to traverse through ingress and egress queues, which are separate lists inside if node.
		and for queue, q node itself has flags to differentiate if queue is ingress/egress.
		so no differentiation is required in queue delete/add operation code.
		*/
		if(i == 1) {
			 if_list_head =  es_tables.egress_if_list_head;
			qosd_port_config_get(ifname, &shaper, NULL, NULL, 0);
			if(shaper.enable != QOSAL_ENABLE)
				sysapi_iface_rate_get(ifname,new_q_iftype, &port_rate);
			else
				port_rate = shaper.pir;
		}
		else 
		{
			if_list_head =  es_tables.ingress_if_list_head;
			qosd_port_config_get(ifname, &shaper, NULL, NULL, QOS_OP_F_INGRESS);
			if(shaper.enable != QOSAL_ENABLE)
				sysapi_iface_rate_get(ifname,new_q_iftype, &port_rate);
			else
				port_rate = shaper.pir;
		}

		/* check if any queues exist on this interface.
		TODO take care of checking classifiers in future.
		*/
		if(if_list_head != NULL) {
			list_for_each(if_list_tmp, if_list_head){
				if_list_node = list_entry(if_list_tmp, qos_if_node_t, list); 
				if(if_list_node != NULL && if_list_node->q_list_head != NULL && if_list_node->q_count > 0) {
					QOSD_GET_IFTYPE(if_list_node->ifname, es_tables.ifmap);
					if(iftype == new_q_iftype) {
						list_for_each(list_node, if_list_node->q_list_head) {
							q_list_node = list_entry(list_node, qos_queue_node_t, list);
							if (q_list_node->enable == QOSAL_ENABLE)
								commit_rate += q_list_node->queue_cfg.shaper.cir;
						}
					}
				}
				else {
					/* nothing to do. */
				}
			}
		}
		i++;
	}

	if((commit_rate + new_commit_rate)  > port_rate) 
	{
		return QOS_RATE_VALID_FAIL;
	}
	if(new_peak_rate > port_rate) 
	{
		return QOS_RATE_VALID_FAIL;
	}
exit :

        EXIT
        return ret;
}

/* =============================================================================
 *  Function Name : is_q_present_for_tc 
 *  Description   : check if any q has tc matching for the classifier's tc 
 * ============================================================================*/

int32_t is_q_present_for_tc(IN uint32_t tc)
{

	int32_t ret =  LTQ_FAIL, i = 0, j = 0;
        struct list_head *list_node = NULL;
        qos_queue_node_t *q_list_node = NULL;
        struct list_head *if_list_tmp = NULL;
        struct list_head *if_list_head = NULL;
        qos_if_node_t *if_list_node = NULL;


	while(i <= 1) {
		/* need to traverse through ingress and egress queues, which are separate lists inside if node.
		and for queue, q node itself has flags to differentiate if queue is ingress/egress.
		so no differentiation is required in queue delete/add operation code.
		*/
		if(i == 1) {
			if_list_head =  es_tables.egress_if_list_head;
		}
		else {
			if_list_head =  es_tables.ingress_if_list_head;
		}

                /* check if any queues exist on this interface.
                   TODO take care of checking classifiers in future.
                */

                if(if_list_head != NULL) {
                        list_for_each(if_list_tmp, if_list_head){
                                if_list_node = list_entry(if_list_tmp, qos_if_node_t, list);
                                if(if_list_node != NULL && if_list_node->q_list_head != NULL && if_list_node->q_count > 0) {
                                	list_for_each(list_node, if_list_node->q_list_head) {
	                                	q_list_node = list_entry(list_node, qos_queue_node_t, list);
						while ((j < MAX_TC_NUM) && (q_list_node->queue_cfg.tc_map[j])) {
							/*assumption that tc value of '0' is not valid*/
	                                       		if((uint32_t)(q_list_node->queue_cfg.tc_map[j] - '0') == tc)
							{ 
								ret = LTQ_SUCCESS;
								goto exit;
							}
							j++;
						}
                                        }
                                }
                	}
		}
		i++;
	}

exit :
        EXIT
        return ret;
}
/* =============================================================================
 *  Function Name : is_cl_present_for_q 
 *  Description   : check if q has any classifiers configured 
 * ============================================================================*/

int32_t is_cl_present_for_q(IN qos_queue_cfg_t *q)
{

        struct list_head *list_node = NULL;
        qos_class_node_t *class_list_node = NULL;
        uint32_t j=0;
	int32_t ret = LTQ_FAIL;


        if (es_tables.class_list_head != NULL) {
                list_for_each(list_node, es_tables.class_list_head) {
                        class_list_node = list_entry(list_node, qos_class_node_t, list);
			if(class_list_node != NULL) {
				j = 0;
				while ((j < MAX_TC_NUM) && (q->tc_map[j])) {
					if(((uint32_t)(q->tc_map[j] - '0') == class_list_node->class_cfg.action.tc)) 
					{ 
						ret = LTQ_SUCCESS;
						goto exit;
					}
					j++;
				}
			}
		}
	}

exit :
        EXIT
        return ret;
}
