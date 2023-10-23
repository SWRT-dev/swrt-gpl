/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <glob.h>
#include <dlfcn.h>
#include <netdb.h>
#include "qosal_inc.h"
#include "qosal_utils.h"
#include "qosal_ugw_debug.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

es_tables_t es_tables;
int32_t eng_count = 0;
extern char gFapiVer[10];

int32_t qosd_is_engine_excluded(qos_engines_t engine, uint32_t engines_to_exclude[]) __attribute__((unused));
static int32_t qosd_is_engine_present_in_datapath(uint32_t dp_id, qos_engines_t engine, uint32_t forward);
static int32_t
qosd_match_q_capab(qos_engines_t engine, uint32_t q_count, qos_q_capab_t *q_capab);
static int32_t
qosd_match_cl_capab(qos_engines_t engine, uint32_t cl_count, qos_cl_capab_t *cl_capab);
static int32_t parse_plat_file(void);
static int32_t remove_queues (struct list_head *if_list_head);
static int32_t init_engines(void);
static int32_t shut_engines(void);

/* Returns N/D with rounding off to nearest natural number */
#define WEIGHT_APPROXIMATION(N, D) (((N) == 0) ? 0:(((N * 10)/D) + 5)/10)

/* =============================================================================
 *  Function Name : es_log_init
 *  Description   : Initialize log type and log level as specified by caller.
 * ============================================================================*/
void es_log_init(qos_cfg_t *qoscfg)
{
	if (qoscfg->log_type) {
		log_type = qoscfg->log_type;
		LOGTYPE = qoscfg->log_type;
	}
	if(log_type == QOS_LOG_FILE && strnlen_s(qoscfg->log_file, MAX_NAME_LEN) != 0)
		strncpy_s(log_file, MAX_NAME_LEN, qoscfg->log_file, MAX_NAME_LEN);

	es_tables.log_level = qoscfg->log_level;
	LOGLEVEL = qoscfg->log_level;

	return;
}

/* =============================================================================
 *  Function Name : qosd_get_dps_for_iface
 *  Description   : Returns list of datapaths (of all types including
		    CPU and accelerated ones for routed and bridged flows) between
		    given pair of interface categories.
 * ============================================================================*/
int32_t
qosd_get_dps_for_iface(es_tables_t *tbl, iftype_t from, iftype_t to, int32_t dps[], qos_dp_type_t filter)
{
	ENTRY
  	int32_t k = 0, l = 0;
	iftype_t fromIndex, toIndex;
    iftype_t *counter = NULL;

    if (from == QOS_IF_CATEGORY_MAX) {
    	fromIndex = from - 1;
        counter = &fromIndex;
        toIndex = to;
    }
    else if (to == QOS_IF_CATEGORY_MAX) {
        fromIndex = from;
        toIndex = to - 1; 
        counter = &toIndex;
    }  

    if (counter == NULL) {
        LOGF_LOG_ERROR("Invalid inputs to identify datapaths. From/To interface category is incorrect.\n")
        return LTQ_FAIL;
    }

    for ((*counter)=0; (int32_t)(*counter)<tbl->act_ifgrp_cnt; (*counter)++) {
             k = 0;
             if (tbl->datapath_tbl[fromIndex][toIndex][0])
                 LOGF_LOG_DEBUG("datapaths between given interfaces: -->> \n")
             while (k < FLOWS_BW_IFS && tbl->datapath_tbl[fromIndex][toIndex][k]) {
                 LOGF_LOG_DEBUG ("%d\n",tbl->datapath_tbl[fromIndex][toIndex][k])
                 if (filter == QOS_DATAPATH_MAX ||
                     tbl->sys_datapaths[tbl->datapath_tbl[fromIndex][toIndex][k]-1].datapath_type - 1 == filter)
                    dps[l++] = tbl->datapath_tbl[fromIndex][toIndex][k];
                 k++;
             }
    }

    EXIT
    return LTQ_SUCCESS;
}

int32_t
qosd_is_engine_excluded(qos_engines_t engine, uint32_t engines_to_exclude[])
{
	int32_t ret = LTQ_FAIL;
	uint32_t i = 0;
	ENTRY

	if (engines_to_exclude == NULL)
		goto exit;

	while (engines_to_exclude[i]) {
		if (engine == engines_to_exclude[i]) {
			ret = LTQ_SUCCESS;
			break;
		}
		i++;
	}

exit:
	EXIT
	return ret;
}

static int32_t
qosd_is_engine_present_in_datapath(uint32_t dp_id, qos_engines_t engine, uint32_t forward)
{
	uint32_t i = 0;
	int32_t ret = LTQ_FAIL;
	qos_engines_t engine_tmp;

	ENTRY

	if (dp_id < 1) {
		LOGF_LOG_ERROR("Invalid datapath id [%d]. Can't ascertain presence of engine in datapath.\n", dp_id)
		return ret;
	}

	if (forward == 0)
		i = es_tables.sys_datapaths[dp_id - 1].number_of_engines -1;

	do {
		if (i > QOS_ENGINE_MAX)
			goto exit;

		engine_tmp = es_tables.sys_datapaths[dp_id - 1].datapath[i];
		//LOGF_LOG_DEBUG ("engine_tmp is %d, engine id %d, i is %d\n", engine_tmp,engine,i)
		if (engine_tmp == engine) {
			ret = LTQ_SUCCESS;
			break;
		}
		forward ? i++ :i--;
	} while (engine_tmp - 1 != QOS_ENGINE_CPU); /* Assuming CPU is always present in a slow path */

exit:
	EXIT
	return ret;
}

static int32_t
qosd_match_q_capab(qos_engines_t engine, uint32_t q_count, qos_q_capab_t *q_capab)
{
	int32_t ret = LTQ_FAIL;

	ENTRY

	if (engine < 1) {
		LOGF_LOG_ERROR("Invalid engine id [%d]. Can't check queue capabilities match.\n", engine)
		return ret;
	}

	LOGF_LOG_DEBUG("comparing req cap [%u] with [%d]'s cap [%u]\n", q_capab->q_cap, engine, es_tables.engine_info[engine - 1].engine.engine_capab.q_cap);
	if (((q_capab->q_cap & es_tables.engine_info[engine - 1].engine.engine_capab.q_cap) == q_capab->q_cap)
			&& (q_count <= es_tables.engine_info[engine - 1].engine.engine_capab.q_max) ) {
		LOGF_LOG_INFO("Needed capabilities mathed with engine %d\n",engine);
		q_capab->q_point = engine -1;
		ret = LTQ_SUCCESS;
	}
	else {
		LOGF_LOG_INFO ("Needed capabilities did NOT match with engine %d\n",engine);
	}

	EXIT
	return ret;
}

static int32_t
qosd_match_cl_capab(qos_engines_t engine, uint32_t cl_count, qos_cl_capab_t *cl_capab)
{
	int32_t ret = LTQ_FAIL;

	ENTRY

	if (engine < 1) {
		LOGF_LOG_ERROR("Invalid engine id [%d]. Can't check classifier capabilities match.\n", engine)
		return ret;
	}

	LOGF_LOG_DEBUG("comparing req cap:ext_cap [%u:%u] with [%d]'s cap:ext_cap [%u:%u]\n",
		cl_capab->cl_cap.cap, cl_capab->cl_cap.ext_cap, engine,
		es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.cap, es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.ext_cap);
	LOGF_LOG_DEBUG("comparing req cap excl:ext_cap excl [%u:%u] with [%d]'s cap excl:ext_cap excl [%u:%u]\n",
		cl_capab->cl_cap.cap_exclude, cl_capab->cl_cap.ext_cap_exclude, engine,
		es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.cap_exclude, es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.ext_cap_exclude);

	if (((cl_capab->cl_cap.cap & es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.cap) == cl_capab->cl_cap.cap) &&
	     ((cl_capab->cl_cap.cap_exclude & es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.cap_exclude) == cl_capab->cl_cap.cap_exclude) &&
	     ((cl_capab->cl_cap.ext_cap & es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.ext_cap) == cl_capab->cl_cap.ext_cap) &&
	     ((cl_capab->cl_cap.ext_cap_exclude & es_tables.engine_info[engine - 1].engine.engine_capab.cl_cap.ext_cap_exclude) == cl_capab->cl_cap.ext_cap_exclude)
			&& (cl_count <= es_tables.engine_info[engine - 1].engine.engine_capab.cl_max) ) {
		LOGF_LOG_INFO ("Needed capabilities matched with engine %d\n",engine);
		cl_capab->cl_points[0] = engine -1;
		ret = LTQ_SUCCESS;
	}
	else {
		LOGF_LOG_INFO ("Needed capabilities did NOT match with engine %d\n",engine);
	}

	EXIT
	return ret;
}
/* =============================================================================
 *  Function Name : qosd_get_action_point
 *  Description   : API to identify common engine amongst specified datapaths provided,
		    a. engine satisifies required capabilities and
		    b. it has highest weight amongst list of matching engines if any.
 * ============================================================================*/
int32_t
qosd_get_action_point(int32_t dps[], qos_capab_cfg_t *capab, int32_t forward, uint32_t starting_engine)
{
	int32_t i = 0, j = 0;
	int32_t ret = LTQ_FAIL;
	uint32_t start = 0;
	qos_engines_t engine = QOS_ENGINE_MAX;

	ENTRY
	if (forward == 0)
		j = es_tables.sys_datapaths[dps[0]-1].number_of_engines - 1;

	do {
		/* Compare engines of first datapath with those of other datapaths */
		if (j < 0 || j > QOS_ENGINE_MAX)
			goto exit;
		engine = es_tables.sys_datapaths[dps[0]-1].datapath[j];

		/* Skip initial engine until we reach starting engine passed in */
		if (engine == starting_engine)
			start = 1;

		if (starting_engine != 0 && start == 0) {
			forward ? j++ :j--;
			continue;
		}

		i = 1;
		while (dps[i]){
			if (qosd_is_engine_present_in_datapath(dps[i], engine, forward) != LTQ_SUCCESS) {
				LOGF_LOG_DEBUG("Engine %d is NOT present in %s leg of datapath %d\n", engine, forward?"ingress":"egress", dps[i]);
				if (engine != QOS_ENGINE_CPU_DP)
					break;
				LOGF_LOG_DEBUG("Ignoring for CPU engine!\n");
			}
			i++;
		}

		if ( dps[i] == 0) {
			LOGF_LOG_INFO("Engine %d is common across the given datapaths\n",engine);
			if (capab->module == QOS_MODULE_QUEUE) {
				ret = qosd_match_q_capab(engine, capab->capab_cfg.q_capab.q_count, &capab->capab_cfg.q_capab);
			}
			else if (capab->module == QOS_MODULE_CLASSIFIER) {
				ret = qosd_match_cl_capab(engine, capab->capab_cfg.cl_capab.cl_count, &capab->capab_cfg.cl_capab);
			}

			//if ((ret == LTQ_SUCCESS) && (qosd_is_engine_excluded(engine, engines_to_exclude) == LTQ_FAIL))
			if (ret == LTQ_SUCCESS)
				break;
		}

		forward ? j++ :j--;
	} while (engine - 1 != QOS_ENGINE_CPU); /* Assuming CPU is always present in a slow path */

exit:
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_disable_datapaths
 *  Description   : wrapper API to disable specified datapaths.
		    That interfaces with a system API for required action.
 * ============================================================================*/
int32_t
qosd_disable_datapaths(char *ifname, iftype_t iftype, int32_t dps_to_disable[])
{
	ENTRY
	UNUSED(dps_to_disable);
	UNUSED(ifname);
	sysapi_set_ppefp(ifname, iftype,  0);
	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_enable_datapaths
 *  Description   : wrapper API to enable specified datapaths.
		    That interfaces with a system API for required action.
 * ============================================================================*/
int32_t
qosd_enable_datapaths(char *ifname, iftype_t iftype, int32_t dps_to_enable[])
{
	ENTRY
	UNUSED(dps_to_enable);
	UNUSED(ifname);
	sysapi_set_ppefp(ifname, iftype,  1);
	EXIT
	return LTQ_SUCCESS;
}

int32_t
qosd_filter_common_dps(int32_t super[], int32_t sub[], int32_t filter[])
{
	int32_t i = 0, j = 0, match = 0,k = 0;

	ENTRY
	while (super[i]){
		match = 0;
		j = 0;
		while (sub[j]){
			if (super[i] == sub[j]) {
				match = 1;
				break;
			}
			j++;
		}
		if (match == 0)
			filter[k++] = super[i];
		i++;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_get_dps_wo_given_engine
 *  Description   : Identifies datapathas to be excluded from given list,
		    which contain specified engine within them.
		    This is required to know list of datpaths that are to be disabled
		    in a specific configuration scenario.
		    For example, out of possible datapaths those which have acceleration
		    engine are to be disabled when classifier action is not possible
		    to be done in acceleration engine.
 * ============================================================================*/
int32_t
qosd_get_dps_wo_given_engine(int32_t dps[], qos_engines_t engine, IN int32_t ingress, int32_t dps_to_disable[])
{
	ENTRY
	int32_t i = 0, num = 0, j = 0, match = 0, k = 0, nLimit = 0;

	while (dps[i]){
		num = es_tables.sys_datapaths[dps[i]-1].number_of_engines;
		match = 0;
		j = (ingress == 1)?0:(num - 1);
		nLimit = (ingress == 1)?num:-1;
		while(j != nLimit) {
			LOGF_LOG_DEBUG("j %d, engine is %d in dp %d, given engine %d\n",j,es_tables.sys_datapaths[dps[i]-1].datapath[j] -1, dps[i], engine)
			if (es_tables.sys_datapaths[dps[i]-1].datapath[j] -1 == (int32_t)engine)
				match = 1;

			if ((es_tables.sys_datapaths[dps[i]-1].datapath[j] - 1 == QOS_ENGINE_CPU))
				break;
			j = (ingress == 1)?(j + 1):(j - 1);
		}
		if (match == 0) {
			LOGF_LOG_DEBUG("disable dp [%d]\n", dps[i]);
			dps_to_disable[k++] = dps[i];
		}
		i++;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_prepare_needed_capab
 *  Description   : Prepares required capabilities for a queue object.
 * ============================================================================*/
int32_t
qosd_prepare_needed_capab(qos_queue_cfg_t *q, iftype_t iftype, uint32_t *needed_capab)
{
	ENTRY
	if(iftype == QOS_IF_CATEGORY_ATMWAN)
		*needed_capab |= Q_CAP_ATM_IF;

	if(iftype == QOS_IF_CATEGORY_PTMWAN)
		*needed_capab |= Q_CAP_PTM_IF;

	if(iftype == QOS_IF_CATEGORY_ETHLAN) 
		*needed_capab |= Q_CAP_ETHL_IF;
	
	if(iftype == QOS_IF_CATEGORY_ETHWAN) 
		*needed_capab |= Q_CAP_ETHW_IF;
	
	if(iftype == QOS_IF_CATEGORY_LTEWAN)
		*needed_capab |= Q_CAP_LTE_IF;

	if (q->drop.mode == QOS_DROP_RED)
		*needed_capab |= Q_CAP_RED;

	if (q->drop.mode == QOS_DROP_WRED)
		*needed_capab |= Q_CAP_WRED;

	if (q->drop.mode == QOS_DROP_TAIL)
		*needed_capab |= Q_CAP_DT;

	if (q->drop.mode == QOS_DROP_CODEL)
		*needed_capab |= Q_CAP_CODEL;

	if (q->sched == QOS_SCHED_SP)
		*needed_capab |= Q_CAP_SP;

	if (q->sched == QOS_SCHED_WFQ)
		*needed_capab |= Q_CAP_WFQ;

	//*needed_capab |= Q_CAP_WRR;
	//*needed_capab |= Q_CAP_LENGTH;
	//*needed_capab |= Q_CAP_SUBQUEUEING;

	if (q->drop.enable == 1) //TODO: change it appropriate macro
		*needed_capab |= Q_CAP_SHAPE;

	if (q->shaper.mode == QOS_SHAPER_COLOR_BLIND)
		*needed_capab |= Q_CAP_COL_BLIND;

	if (q->shaper.mode == QOS_SHAPER_TR_TCM)
		*needed_capab |= Q_CAP_TR_TCM;

	if (q->shaper.mode == QOS_SHAPER_TR_TCM_RFC4115)
		*needed_capab |= Q_CAP_TR_TCM_RFC4115;

	if (q->shaper.mode == QOS_SHAPER_LOOSE_COLOR_BLIND)
		*needed_capab |= Q_CAP_LOOSE_COL_BLIND;

	if (q->flags & QOS_Q_F_INGRESS)
		*needed_capab |= Q_CAP_INGRESS;

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_prepare_needed_capab_port
 *  Description   : Prepares required capabilities for shaper object.
 * ============================================================================*/
int32_t
qosd_prepare_needed_capab_port(qos_shaper_t *shaper, iftype_t iftype, uint32_t *needed_capab)
{
	ENTRY

	/* use Q capabilities of interfaces supported in here also. */
	if(iftype == QOS_IF_CATEGORY_ATMWAN)
		*needed_capab |= Q_CAP_ATM_IF;

	if(iftype == QOS_IF_CATEGORY_PTMWAN)
		*needed_capab |= Q_CAP_PTM_IF;

	if(iftype == QOS_IF_CATEGORY_ETHLAN) 
		*needed_capab |= Q_CAP_ETHL_IF;
	
	if(iftype == QOS_IF_CATEGORY_ETHWAN) 
		*needed_capab |= Q_CAP_ETHW_IF;
	
	if(iftype == QOS_IF_CATEGORY_LTEWAN)
		*needed_capab |= Q_CAP_LTE_IF;

	if (shaper->mode != QOS_SHAPER_NONE )
		*needed_capab |= PORT_CAP_SHAPE;

	if (shaper->mode == QOS_SHAPER_COLOR_BLIND)
		*needed_capab |= Q_CAP_COL_BLIND;

	if (shaper->mode == QOS_SHAPER_TR_TCM)
		*needed_capab |= Q_CAP_TR_TCM;

	if (shaper->mode == QOS_SHAPER_TR_TCM_RFC4115)
		*needed_capab |= Q_CAP_TR_TCM_RFC4115;

	if (shaper->mode == QOS_SHAPER_LOOSE_COLOR_BLIND)
		*needed_capab |= Q_CAP_LOOSE_COL_BLIND;

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : parse_plat_file
 *  Description   : Wrapper API that reads platform config file and records within
		    ES context.
 * ============================================================================*/
static int32_t parse_plat_file(void)
{
	LOGF_LOG_INFO ("Initializing Engine Selector...\n");
	/* maximum interface category */
	es_tables.act_ifgrp_cnt = QOS_IF_CATEGORY_MAX; /* 0-QOS_IF_CATEGORY_MAX used to identify ingress and egress interface type for a datapath */
	qosd_populate_tbls(PLATFORM_CONFIG, &es_tables);
	qosd_show_dps(&es_tables);
	qosd_show_dps_bw_ifaces(&es_tables);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : remove_queues
 *  Description   : Removes user configuration of queues and classifiers details of which
 is read from context stored within ES.
 * ============================================================================*/
static int32_t remove_queues (struct list_head *if_list_head)
{
	qos_if_node_t *if_list_node = NULL;
	struct list_head *list_node = NULL, *if_list_tmp = NULL, *tmp_list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;
	iftype_t iftype = QOS_IF_CATEGORY_MAX;
	ifinggrp_t ifinggrp = QOS_IF_CAT_INGGRP_MAX;
	qos_engines_t engine = QOS_ENGINE_MAX;
	struct list_head *class_list_node = NULL;
	qos_class_rule_t *engine_rule;
	int32_t i = 0;
	int32_t ret = 0;
	ENTRY

		if(if_list_head != NULL) {
			list_for_each(if_list_tmp, if_list_head){
				if_list_node = list_entry(if_list_tmp, qos_if_node_t, list);
				iftype = qosd_get_iftype(if_list_node->ifname, es_tables.ifmap);
				ifinggrp = qosd_get_ifinggrp(if_list_node->ifname, es_tables.ifinggrp_map);
				LOGF_LOG_DEBUG("no of classifiers on this interface %d\n",if_list_node->class_count);
				i = if_list_node->class_count;
				while (( i > 0) && (if_list_node->classifiers[i-1] != NULL)) {
					LOGF_LOG_INFO("Deleting Classifer %s\n",if_list_node->classifiers[i-1]->class_cfg.filter.name);
					list_for_each(class_list_node, if_list_node->classifiers[i-1]->engine_rules_list_head) {
						engine_rule = list_entry(class_list_node, qos_class_rule_t, list);
						LOGF_LOG_DEBUG("Deleting Classifier at engine %s and order is %d\n",es_tables.eng_names[engine_rule->engine],engine_rule->order);
						QOSD_ENGINE_CLASS_DEL(engine_rule->engine, iftype, ifinggrp, engine_rule->order, &engine_rule->parent->class_cfg, engine_rule->flags);

						es_tables.engine_info[engine_rule->engine].rules_count--;
						LOGF_LOG_DEBUG("Deleting classifier node \n");
						ret = qosd_del_class_node(&es_tables.class_list_head, if_list_node->classifiers[i-1]);
						if_list_node->class_count--;

					}

					i--;
				}
				engine = if_list_node->common_q_point;
				/* TODO : need to add classifier count and handle classifier deletion */
				if((if_list_node->q_list_head != NULL) && (if_list_node->q_count > 0)) {
					iftype = qosd_get_iftype(if_list_node->ifname, es_tables.ifmap);
					ifinggrp = qosd_get_ifinggrp(if_list_node->ifname, es_tables.ifinggrp_map);
					list_for_each(list_node, if_list_node->q_list_head){
						q_list_node = list_entry(list_node, qos_queue_node_t, list);
						LOGF_LOG_DEBUG ("Deleting queue [%s] in engine %s\n", q_list_node->queue_cfg.name, es_tables.eng_names[engine])
							es_tables.engine_info[engine].engine.queue_delete(if_list_node->ifname, iftype, ifinggrp, &q_list_node->queue_cfg);
					}
					list_for_each_safe(list_node, tmp_list_node, if_list_node->q_list_head){
						q_list_node = list_entry(list_node, qos_queue_node_t, list);
						list_del(list_node);
						free(q_list_node);
					}
					/* handle last queue for this interface */
					if(if_list_node->q_count == 1) {
						INIT_LIST_HEAD(if_list_node->q_list_head);
						if_list_node->q_count = 0;
					}
				}
			}

			/* TODO : remove interface map entry before removing the interface itself */
			list_for_each_safe(list_node, tmp_list_node, if_list_head){
				if_list_node = list_entry(list_node, qos_if_node_t, list);
				list_del(list_node);
				free(if_list_node);
			}
			INIT_LIST_HEAD(if_list_head);
		}
	EXIT
		return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : shut_engines
 *  Description   : Removes user configuration of queues and classifiers,
		    de-registers registered engines details of which is read from
		    context stored within ES.
 * ============================================================================*/
static int32_t shut_engines(void)
{
	ENTRY
	/* shut */
	/* 1. remove all configured queues and classifiers
	2. loop through each engine supported
	3. identify if engine's shut function is available and invoke (through function pointer and dlysm)
	4. free local data structure if any
	5. exit after sending reply to caller
	*/
	qos_engine_t eng;
	int32_t i=0, ret=0;
	uint32_t j = 0;
	//qos_engines_t engine = QOS_ENGINE_MAX;
	glob_t globbuf;
	char lN[100], sTmp[100];
	int32_t (*fp)(qos_module_type_t);
	void *dlH = NULL;
	char *sEngName = NULL;
	char *sPrefix = NULL;

	memset_s(&globbuf, sizeof(globbuf), 0);

	remove_queues (es_tables.egress_if_list_head);
	remove_queues (es_tables.ingress_if_list_head);

	/* for each registered engine, call fini */
	snprintf(sTmp, sizeof(sTmp), "%s*.so", ENG_LIB_PATH);
	glob(sTmp, GLOB_ERR, NULL, &globbuf);
	for(j=0; j<globbuf.gl_pathc && globbuf.gl_pathv != NULL; j++) {
		ret = LTQ_SUCCESS;
		memset_s(&eng, sizeof(eng), 0);
		snprintf(lN, sizeof(lN), "%s", globbuf.gl_pathv[j]);
		dlH = dlopen(lN, RTLD_LAZY);
		if(dlH == NULL) {
			continue;
		}
		sEngName = (char *)dlsym(dlH, "sEngName");
		sPrefix = (char *)dlsym(dlH, "sPrefix");
		if(sEngName == NULL || sPrefix == NULL) {
			continue;
		}
		for(i=0; i<es_tables.number_of_engines; i++) {
			ret = LTQ_SUCCESS;
			memset_s(&eng, sizeof(eng), 0);

			if(strcmp(es_tables.eng_names[i], sEngName) == 0 &&
				es_tables.engine_info[es_tables.eng_ids[i] - 1].status == 1) {
				snprintf(lN, sizeof(lN), "%s_%s", sPrefix, ENG_FINI_CB_POSTFIX);
				fp = dlsym(dlH, lN);
				if(fp != NULL) {
					ret = (*fp)(QOS_MODULE_BOTH);
					LOGF_LOG_DEBUG("De-registered engine [%s]\n", es_tables.eng_names[i])
				}
			}
		}
	}

	EXIT
	globfree(&globbuf);
	return ret;
}

/* =============================================================================
 *  Function Name : init_engines
 *  Description   : Reads platform config file and registers supported engines.
		    Configuration read from platform config file would be stored in
		    different members of es_tables data structure which is the core
		    configuration context within engine selector (ES).
 * ============================================================================*/
static int32_t init_engines(void)
{
	ENTRY
	/* init */
	qos_engine_t eng;
	int32_t i=0, ret=0;
	uint32_t j = 0;
	qos_engines_t engine = QOS_ENGINE_MAX;
	glob_t globbuf;
	char lN[100], sTmp[100];
	int32_t (*fp)(qos_engine_t *);

	memset_s(&globbuf, sizeof(globbuf), 0);

	snprintf(sTmp, sizeof(sTmp), "%s*.so", ENG_LIB_PATH);
	glob(sTmp, GLOB_ERR, NULL, &globbuf);
	for(j=0; j<globbuf.gl_pathc && globbuf.gl_pathv != NULL; j++) {
		ret = LTQ_FAIL;
		memset_s(&eng, sizeof(eng), 0);
		snprintf(lN, sizeof(lN), "%s", globbuf.gl_pathv[j]);
		void *dlH = dlopen(lN, RTLD_LAZY);
		if(dlH != NULL) {
			char *sEngName = (char *)dlsym(dlH, "sEngName");
			char *sPrefix = (char *)dlsym(dlH, "sPrefix");
			if(sEngName != NULL && sPrefix != NULL) {
				LOGF_LOG_INFO("Found engine with name [%s]\n", sEngName);
				snprintf(lN, sizeof(lN), "%s_%s", sPrefix, ENG_REG_CB_POSTFIX);
				fp = dlsym(dlH, lN);
				if(fp != NULL) {
					LOGF_LOG_DEBUG("Calling register function exposed by engine [%s]\n", lN);
					for(i=0; i<es_tables.number_of_engines; i++) {
						if(!strcmp(es_tables.eng_names[i], sEngName)) {
							engine = (es_tables.eng_ids[i] - 1);
							ret = (*fp)(&eng);
							if(ret == LTQ_SUCCESS && engine != QOS_ENGINE_MAX) {
								LOGF_LOG_INFO("Registered %s engine...\n",es_tables.eng_names[i]);
								memcpy_s(&es_tables.engine_info[engine].engine, sizeof(qos_engine_t), &eng, sizeof(qos_engine_t));
								es_tables.engine_info[engine].status = 1;
								es_tables.engine_info[engine].cl_wt = DEF_CL_WT;
								es_tables.engine_info[engine].q_wt = DEF_Q_WT;
								eng_count++;
							}
						}
					}
				}
			}
			dlclose(dlH);
		}
	}
	globfree(&globbuf);

	EXIT
	return LTQ_SUCCESS;
}

#if 0 /* TODO */
int32_t internal_rules_cfg()
{
	/* define queues for local->WAN and WAN->local traffic prioritization
	..such as DevM, web, SIP/RTP etc
	*/
	int32_t int_rules_cnt = 4, i = 0;

	qos_queue_cfg_t int_rules[5] = {
		{
			.enable = 1,
			.name = "CPU-N-Q1",
			.flags = QOS_Q_F_CPU,
			.tc_map[0] = 1,
			.priority = 1,
			.drop.enable = 1,
			.drop.mode = QOS_DROP_TAIL,
			.sched = QOS_SCHED_SP
		},
		{
			.enable = 1,
			.name = "CPU-N-Q2",
			.flags = QOS_Q_F_CPU,
			.tc_map[0] = 2,
			.priority = 2,
			.drop.enable = 1,
			.drop.mode = QOS_DROP_TAIL,
			.sched = QOS_SCHED_SP
		},
		{
			.enable = 1,
			.name = "CPU-I-Q1",
			.flags = QOS_Q_F_CPU | QOS_Q_F_INGRESS,
			.tc_map[0] = 1,
			.priority = 1,
			.drop.enable = 1,
			.drop.mode = QOS_DROP_TAIL,
			.sched = QOS_SCHED_SP
		},
		{
			.enable = 1,
			.name = "CPU-I-Q2",
			.flags = QOS_Q_F_CPU | QOS_Q_F_INGRESS,
			.tc_map[0] = 2,
			.priority = 2,
			.drop.enable = 1,
			.drop.mode = QOS_DROP_TAIL,
			.sched = QOS_SCHED_SP
		}
	};

	for(i=0; i<int_rules_cnt; i++) {
		qosd_queue_set(NULL, &int_rules[i], QOS_OP_F_ADD);
	}
	
	return LTQ_SUCCESS;
}
#endif

/* =============================================================================
 *  Function Name : es_shut
 *  Description   : API which de-initializes ES in term of,
		    removing user configuration and templates
		    and de-registering supported engines.
 * ============================================================================*/
int32_t es_shut(void)
{
	ENTRY

	shut_engines();
	/*Disable QoS in es_tables*/
	es_tables.qos_enable = QOSAL_DISABLE;

	/* print engines registered */
#if 0
	list_for_each_entry(eng_tmp, &engines->list, list) {
		for(i=0; i<eng_count; i++) {
			LOGF_LOG_INFO("[%u] --> \n", es_tables.eng_tmp)
		}
	}
#endif

	/* TODO configure internal rules -- replace with RULE_GEN_LOGIC */
	// internal_rules_cfg();
	EXIT

	LOGF_LOG_INFO("Engine selector shut successfully done..\n");
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : es_init
 *  Description   : API which initialization of ES in terms of,
		    reading platform config file, registering supported engines
		    and configure rules from templates.
 * ============================================================================*/
int32_t es_init(qos_cfg_t *qoscfg)
{
	ENTRY

	int32_t i = 0;

	/* initialize DS */
	memset_s(&es_tables, sizeof(es_tables_t), 0);

	es_tables.qos_enable = qoscfg->ena;
	strncpy_s(es_tables.version, MAX_NAME_LEN, gFapiVer, strnlen_s(gFapiVer, MAX_NAME_LEN));

	es_log_init(qoscfg);

	for(i=0; i<QOS_IF_CATEGORY_MAX; i++) {
		es_tables.ifmap[i].ifcat = i;
	}

	for(i=0; i<QOS_IF_CAT_INGGRP_MAX; i++) {
		es_tables.ifinggrp_map[i].inggrp = i; 
	}

	/* read platform config file and populate interface, flow, engine tables */
	parse_plat_file();

	/* validate status of interfaces by using system API */

	/* validate flow table to mark active/inactive based on interface status */

	init_engines();

	/* print engines registered */
#if 0
	list_for_each_entry(eng_tmp, &engines->list, list) {
		for(i=0; i<eng_count; i++) {
			LOGF_LOG_INFO("[%u] --> \n", es_tables.eng_tmp)
		}
	}
#endif

	/* TODO configure internal rules -- replace with RULE_GEN_LOGIC */
	// internal_rules_cfg();
	EXIT

	LOGF_LOG_INFO("Engine selector init successfully done..\n");
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : q_wt_n_rate_derive
 *  Description   : API to identify actual rate to be configured on the queue based on,
		    scheduler algorithm, shaper configuration and port/interface/link rate.
		    This API should be called by ES before calling plugin, which will
		    overwrite value of weight parameter with the value returned an be used for both SP and WFQ.
 * ============================================================================*/
int32_t q_wt_n_rate_derive(iftype_t iftype, qos_if_node_t *if_list_node, qos_queue_cfg_t *qcfg)
{
	uint32_t sum_cirs_pct = 0, port_rate = 0;
	struct list_head *list_node = NULL;
	qos_queue_node_t *q_list_node = NULL;

	ENTRY
	if(if_list_node == NULL) {
		LOGF_LOG_ERROR ("Can't dervie queue weight. Interface node is not available.\n")
		return LTQ_FAIL;
	}

	if(qcfg == NULL || (qcfg != NULL && qcfg->sched == QOS_SCHED_WFQ)) {

		if (if_list_node->q_list_head == NULL && qcfg != NULL ) {
			LOGF_LOG_INFO ("First WFQ, weight is assigned 100\n")
			qcfg->weight = 100;
			return LTQ_SUCCESS;
		}
 
 		if (if_list_node->port_shaper != NULL && if_list_node->port_shaper->shaper.enable == QOSAL_ENABLE )
			port_rate = if_list_node->port_shaper->shaper.pir;
		else
			sysapi_iface_rate_get(if_list_node->ifname,iftype, &port_rate);

		if (port_rate == 0 || port_rate > MAX_IF_RATE) {
			LOGF_LOG_ERROR("Interface Rate invalid!\n");
			return LTQ_FAIL;
		}

		sum_cirs_pct = (if_list_node->sum_cirs * 100 ) / port_rate;

		if (if_list_node->q_list_head != NULL) {
			list_for_each(list_node, if_list_node->q_list_head) {
				q_list_node = list_entry(list_node, qos_queue_node_t, list);
				if (q_list_node->enable == QOSAL_ENABLE && q_list_node->queue_cfg.sched == QOS_SCHED_WFQ
						&& (if_list_node->sum_wts != 0)) {
					/* even for WFQ with cir and no wt, wt would be calculated based on cir
					 * and sum_wts would have been updated. so for 2nd queue onwards, sum_wts
					 * would not be zero.
					 */
					if (q_list_node->queue_cfg.shaper.cir == 0 && if_list_node->sum_wts != 0) {
						q_list_node->queue_cfg.weight = WEIGHT_APPROXIMATION((q_list_node->orig_wt * (100 - sum_cirs_pct)), if_list_node->sum_wts);
					}
					else if (port_rate != 0){
						q_list_node->queue_cfg.weight = WEIGHT_APPROXIMATION((q_list_node->queue_cfg.shaper.cir * 100), port_rate);
					}
					LOGF_LOG_DEBUG("For given WFQ, derived wt is %d and user given wt is %d\n", q_list_node->queue_cfg.weight, q_list_node->orig_wt)
				}
			}
		}

		if (qcfg != NULL && port_rate != 0 && qcfg->shaper.cir != 0) {
			qcfg->weight = WEIGHT_APPROXIMATION((qcfg->shaper.cir * 100),  port_rate);
		}
		else if (qcfg != NULL && if_list_node->sum_wts != 0) {
			qcfg->weight = WEIGHT_APPROXIMATION((qcfg->weight * (100 - sum_cirs_pct)), if_list_node->sum_wts);
		}

	}
	else {
		LOGF_LOG_DEBUG("queue mode is sp\n")
		qcfg->weight = 0;
	}

	EXIT
	return LTQ_SUCCESS;
}


/* =============================================================================
 *  Function Name : qosd_prepare_needed_cl_capab
 *  Description   : Prepares required capabilities for a classifier object.
 * ============================================================================*/
int32_t
qosd_prepare_needed_cl_capab(qos_class_cfg_t *class_cfg, iftype_t iftype, cl_cap_t *needed_capab)
{
	ENTRY

	LOGF_LOG_DEBUG("Following parameters are set for classifier\n");
	if(iftype == QOS_IF_CATEGORY_ETHLAN) {
		LOGF_LOG_DEBUG("ethlan set\n");
		needed_capab->cap |= CL_CAP_ETHL_IF;
	}

	if(iftype == QOS_IF_CATEGORY_ATMWAN) {
		LOGF_LOG_DEBUG("atmwan set\n");
		needed_capab->cap |= CL_CAP_ATM_IF;
	}

	if(iftype == QOS_IF_CATEGORY_PTMWAN) {
		LOGF_LOG_DEBUG("ptmwan set\n");
		needed_capab->cap |= CL_CAP_PTM_IF;
	}

	if(iftype == QOS_IF_CATEGORY_ETHWAN) {
		LOGF_LOG_DEBUG("ethwan set\n");
		needed_capab->cap |= CL_CAP_ETHW_IF;
	}

	if(iftype == QOS_IF_CATEGORY_LTEWAN) {
		LOGF_LOG_DEBUG("ltewan set\n");
		needed_capab->cap |= CL_CAP_LTE_IF;
	}

	if (CL_DMAC_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("dmac set\n");
		needed_capab->cap |= CL_CAP_MAC_ADDR;
		if (CL_DMAC_EX(class_cfg) > 0)
			needed_capab->cap_exclude |= CL_CAP_MAC_ADDR;
	}

	if (CL_SMAC_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("smac set\n");
		needed_capab->cap |= CL_CAP_MAC_ADDR;
		if (CL_SMAC_EX(class_cfg) > 0)
			needed_capab->cap_exclude |= CL_CAP_MAC_ADDR;
	}

/* make sure caller has put default values where appropriate. */
	if (CL_ETH_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("eth type set\n");
		needed_capab->ext_cap |= CL_CAP_ETH_TYPE;
		if(CL_ETH_EX(class_cfg) > 0) {
			needed_capab->ext_cap_exclude |= CL_CAP_ETH_TYPE;
		}
	}
#if 0
	if (class_cfg->filter.ethhdr.dsap > -1 || class_cfg->filter.ethhdr.ssap > -1) {
		LOGF_LOG_DEBUG("eth sap set\n");
		needed_capab->cap |= CL_CAP_ETH_SAP;
		if (class_cfg->filter.ethhdr.dsap_exclude || class_cfg->filter.ethhdr.ssap_exclude)
			needed_capab->cap_exclude |= CL_CAP_ETH_SAP;
	}

	if (class_cfg->filter.ethhdr.llc_control > -1) {
		LOGF_LOG_DEBUG("llc set\n");
		needed_capab->cap |= CL_CAP_ETH_LLC_CONTROL;
	}

	if (class_cfg->filter.ethhdr.oui > -1) {
		LOGF_LOG_DEBUG("eth out set\n");
		needed_capab->cap |= CL_CAP_ETH_OUI;
	}

	if (class_cfg->filter.ethhdr.vlan_dei > -1) {
		LOGF_LOG_DEBUG("vlan dei set\n");
		needed_capab->cap |= CL_CAP_ETH_VLAN_DEI;
	}

	if (class_cfg->filter.ethhdr.inner_prio > -1) {
		LOGF_LOG_DEBUG("inner prio set\n");
		needed_capab->cap |= CL_CAP_ETH_VLAN_PRIO;
		needed_capab->cap |= CL_CAP_ETH_INNER_VLAN;
	}

	if (class_cfg->filter.ethhdr.inner_vlanid > -1) {
		LOGF_LOG_DEBUG("inner vlan id set\n");
		needed_capab->cap |= CL_CAP_ETH_VLAN_ID;
		needed_capab->cap |= CL_CAP_ETH_INNER_VLAN;
	}

	if (class_cfg->filter.ethhdr.inner_vlan_dei > -1) {
		LOGF_LOG_DEBUG("inner vlan dei set\n");
		needed_capab->cap |= CL_CAP_ETH_VLAN_DEI;
		needed_capab->cap |= CL_CAP_ETH_INNER_VLAN;
	}
#endif

	if(CL_ETHHDR_SEL(class_cfg) > 0 && CL_VPRIO_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("vlan prio set\n");
		needed_capab->cap |= CL_CAP_ETH_VLAN_PRIO;
		if (CL_VPRIO_EX(class_cfg) > 0)
			needed_capab->cap_exclude |= CL_CAP_ETH_VLAN_PRIO;
	}

	if(CL_ETHHDR_SEL(class_cfg) > 0 && CL_VID_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("vlan id set\n");
		needed_capab->cap |= CL_CAP_ETH_VLAN_ID;
		if (CL_VID_EX(class_cfg) > 0)
			needed_capab->cap_exclude |= CL_CAP_ETH_VLAN_ID;
	}

	if(CL_O_SIP_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("src ip set\n");
		if(util_get_ipaddr_family(class_cfg->filter.outer_ip.src_ip.ipaddr) == AF_INET) {
			LOGF_LOG_DEBUG("ip is v4\n");
			needed_capab->cap |= CL_CAP_L3_IPv4_ADDR; //check if add is IPv4/v6 and set cap
		}
		else {
			LOGF_LOG_DEBUG("ip is v6\n");
			needed_capab->cap |= CL_CAP_L3_IPv6_ADDR; //check if add is IPv4/v6 and set cap
		}
		if (CL_O_SIP_EX(class_cfg)) {
			if(util_get_ipaddr_family(class_cfg->filter.outer_ip.src_ip.ipaddr) == AF_INET)
				needed_capab->cap_exclude |= CL_CAP_L3_IPv4_ADDR; //check if add is IPv4/v6 and set cap
			else
				needed_capab->cap_exclude |= CL_CAP_L3_IPv6_ADDR; //check if add is IPv4/v6 and set cap
		}
	}

	if(CL_O_DIP_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("dst ip set\n");
		if(util_get_ipaddr_family(class_cfg->filter.outer_ip.dst_ip.ipaddr) == AF_INET) {
			LOGF_LOG_DEBUG("ip is v4\n");
			needed_capab->cap |= CL_CAP_L3_IPv4_ADDR; //check if add is IPv4/v6 and set cap
		}
		else {
			LOGF_LOG_DEBUG("ip is v6\n");
			needed_capab->cap |= CL_CAP_L3_IPv6_ADDR; //check if add is IPv4/v6 and set cap
		}
		if (CL_O_DIP_EX(class_cfg)) {
			if(util_get_ipaddr_family(class_cfg->filter.outer_ip.dst_ip.ipaddr) == AF_INET)
				needed_capab->cap_exclude |= CL_CAP_L3_IPv4_ADDR; //check if add is IPv4/v6 and set cap
			else
				needed_capab->cap_exclude |= CL_CAP_L3_IPv6_ADDR; //check if add is IPv4/v6 and set cap
		}
	}

#if 0
	if (class_cfg->filter.outer_ip.src_ip.mask || class_cfg->filter.outer_ip.dst_ip.mask) {//check if mask is set and valid
		needed_capab->cap |= CL_CAP_L3_IPv4_MASK;
	}
#endif

	if (CL_O_PROTO_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("proto set\n");
		needed_capab->cap |= CL_CAP_L4_PROTO_N_PORT;
		if(CL_O_PROTO_EX(class_cfg)) {
			needed_capab->cap_exclude |= CL_CAP_L4_PROTO_N_PORT;
		}
	}

	if (CL_O_SPORT_SEL(class_cfg) > 0 || CL_O_DPORT_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("ports set\n");
		needed_capab->cap |= CL_CAP_L4_PROTO_N_PORT;
		if(CL_O_SPORT_EX(class_cfg) > 0 || CL_O_DPORT_EX(class_cfg) > 0) {
			needed_capab->cap_exclude |= CL_CAP_L4_PROTO_N_PORT;
		}
	}

#if 0
	if (((class_cfg->filter.outer_ip.src_port.start_port > -1)
		&& (class_cfg->filter.outer_ip.src_port.end_port > class_cfg->filter.outer_ip.src_port.start_port))
		|| ((class_cfg->filter.outer_ip.dst_port.start_port > -1)
		&& (class_cfg->filter.outer_ip.dst_port.end_port > class_cfg->filter.outer_ip.dst_port.start_port))) {
		needed_capab->cap |= CL_CAP_L3_PORT_RANGE;
	}
#endif

	if (strnlen_s(class_cfg->filter.inner_ip.src_ip.ipaddr, IP_ADDRSTRLEN) > 0 || strnlen_s(class_cfg->filter.inner_ip.dst_ip.ipaddr, IP_ADDRSTRLEN) > 0) {
		LOGF_LOG_DEBUG("inner ip set\n");
		needed_capab->cap |= CL_CAP_L3_IPv4_ADDR; //check if add is IPv4/v6 and set cap
		needed_capab->cap |= CL_CAP_L3_INNER_IP;
	}

	if (strnlen_s(class_cfg->filter.inner_ip.src_ip.mask, IP_ADDRSTRLEN) > 0 || strnlen_s(class_cfg->filter.inner_ip.dst_ip.mask, IP_ADDRSTRLEN) > 0) {//check if mask is set and valid
#if 0
		needed_capab->cap |= CL_CAP_L3_IPv4_MASK;
#endif
		needed_capab->cap |= CL_CAP_L3_INNER_IP;
	}

	if (class_cfg->filter.inner_ip.ip_p > 0) {
		LOGF_LOG_DEBUG("inner protocol set\n");
		needed_capab->cap |= CL_CAP_L3_PROTO;
		needed_capab->cap |= CL_CAP_L3_INNER_IP;
	}

	if (class_cfg->filter.inner_ip.src_port.start_port > 0 || class_cfg->filter.inner_ip.dst_port.start_port > 0) {//check if mask is set and valid
		LOGF_LOG_DEBUG("inner ports set\n");
		needed_capab->cap |= CL_CAP_L4_PROTO_N_PORT;
		needed_capab->cap |= CL_CAP_L3_INNER_IP;
	}

	if (CL_IPHDR_SEL(class_cfg) > 0) {
		if( (CL_IPHDR_SEL(class_cfg) & CLSCFG_PARAM_IP_HDR_LEN_MIN) ||
			(CL_IPHDR_SEL(class_cfg) & CLSCFG_PARAM_IP_HDR_LEN_MAX)) {
			LOGF_LOG_DEBUG("pkt length set\n");
			needed_capab->cap |= CL_CAP_L3_PKTLEN;
			if( (CL_IPHDR_EX(class_cfg) & CLSCFG_PARAM_IP_HDR_LEN_MIN) ||
				(CL_IPHDR_EX(class_cfg) & CLSCFG_PARAM_IP_HDR_LEN_MAX)) {
				needed_capab->cap_exclude |= CL_CAP_L3_PKTLEN;
			}
		}
		if(CL_IPHDR_SEL(class_cfg) & CLSCFG_PARAM_IP_HDR_DSCP) {
			LOGF_LOG_DEBUG("dscp set\n");
			needed_capab->cap |= CL_CAP_L3_DSCP;
			if(CL_IPHDR_EX(class_cfg) & CLSCFG_PARAM_IP_HDR_DSCP) {
				needed_capab->cap_exclude |= CL_CAP_L3_DSCP;
			}
		}
	}

	if (CL_TCPHDR_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("tcp flags set\n");
		needed_capab->cap |= CL_CAP_L4_TCP_FL;
	}

	if (CL_L7_PROTO_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("L7 proto set\n");
		needed_capab->cap |= CL_CAP_L7_PROTO;
	}

	if (CL_TXIF_SEL(class_cfg) > 0) {
		LOGF_LOG_DEBUG("Outgoing interface set\n");
		needed_capab->ext_cap |= CL_CAP_TX_IF;
	}

	if (class_cfg->action.flags & CL_ACTION_TC) {
		LOGF_LOG_DEBUG("tc set\n");
		needed_capab->cap |= CL_CAP_ACT_TC;
	}

	if (class_cfg->action.flags & CL_ACTION_DROP) {
		LOGF_LOG_DEBUG("drop set\n");
		needed_capab->cap |= CL_CAP_ACT_DROP;
	}

	if (class_cfg->action.flags & CL_ACTION_DSCP) {
		LOGF_LOG_DEBUG("dscp set\n");
		needed_capab->cap |= CL_CAP_ACT_DSCP_REMARK;
	}

	if (class_cfg->action.flags & CL_ACTION_VLAN) {
		LOGF_LOG_DEBUG("vlan set\n");
		needed_capab->cap |= CL_CAP_ACT_VLAN_REMARK;
	}

	if (class_cfg->action.flags & CL_ACTION_POLICER) {
		LOGF_LOG_DEBUG("policer set\n");
		needed_capab->cap |= CL_CAP_ACT_POLICE;
	}

	if (class_cfg->action.flags & CL_ACTION_POLICY_ROUTE) {
		LOGF_LOG_DEBUG("policy route set\n");
		needed_capab->cap |= CL_CAP_ACT_POLICY_RT;
	}

	if (class_cfg->action.flags & CL_ACTION_ACCELERATION) {
		LOGF_LOG_DEBUG("acceleration set\n");
		needed_capab->ext_cap |= CL_CAP_ACT_ACCL; 
	}

	if (class_cfg->filter.flags & CL_ACTION_CL_MPTCP) {
		LOGF_LOG_DEBUG("mptcp capability set\n");
		needed_capab->ext_cap |= CL_CAP_MPTCP;
	}
	if (class_cfg->action.flags & CL_ACTION_CL_SW_ENGINE) {
		LOGF_LOG_DEBUG("Action Ingress SW engine capability set\n");
		needed_capab->ext_cap |= CL_CAP_ACT_INGRS_SW;
	}
	if (class_cfg->action.flags & CL_ACTION_CL_HW_ENGINE) {
		LOGF_LOG_DEBUG("Action Ingress HW engine capability set\n");
		needed_capab->ext_cap |= CL_CAP_ACT_INGRS_HW;
	}

	EXIT
	return LTQ_SUCCESS;
}
