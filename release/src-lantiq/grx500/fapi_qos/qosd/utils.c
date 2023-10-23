/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <libubox/list.h>
#include "qosal_debug.h"
#include <ezxml.h>
#include "qosal_flow.h"
#include "qosal_es.h"
#include "qosal_utils.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

#define MAX_LEN 128

extern uint16_t LOGTYPE, LOGLEVEL;
static iftype_t qosd_show_iftype (if_category_map_t *ifmap);
int32_t qosd_show_engines(es_tables_t *tbl) __attribute__((unused));

/* =============================================================================
 *  Function Name : qosd_get_engine_node
 *  Description   : Internal API to get engine node from the list
 * ============================================================================*/
qos_class_rule_t*
qosd_get_engine_node(struct list_head *engine_list_head, qos_engines_t engine)
{
	qos_class_rule_t *engine_list_node = NULL;
	struct list_head *list_tmp = NULL;

	ENTRY
	if (engine_list_head == NULL){
		LOGF_LOG_ERROR("Engine list is not intialized\n")
		return NULL;
	}

	list_for_each(list_tmp, engine_list_head){
		engine_list_node = list_entry(list_tmp, qos_class_rule_t, list);
		if (engine_list_node->engine == engine)
			return engine_list_node;
	}

	EXIT
	return NULL;
}

/* =============================================================================
 *  Function Name : qosd_get_class_node
 *  Description   : Internal API to get class node from the list
 * ============================================================================*/
qos_class_node_t*
qosd_get_class_node(struct list_head *class_list_head, char *name,char *rx_if)
{
	qos_class_node_t *class_list_node = NULL;
	struct list_head *list_tmp = NULL;

	ENTRY
	if (class_list_head == NULL){
		LOGF_LOG_ERROR("Can't get requested classifier. Classifier list is not initialized\n")
		return NULL;
	}

	list_for_each(list_tmp, class_list_head){
		class_list_node = list_entry(list_tmp, qos_class_node_t, list);
		if ((strcmp (class_list_node->class_cfg.filter.name, name) == 0) && (strcmp (class_list_node->class_cfg.filter.rx_if, rx_if) == 0))
			return class_list_node;
	}

	EXIT
	return NULL;
}
/* =============================================================================
 *  Function Name : qosd_get_if_node
 *  Description   : Returns interface node matching specified queue name
		    in interface linked list.
 * ============================================================================*/
qos_if_node_t*
qosd_get_if_node(struct list_head *if_list_head, char *ifname)
{
	qos_if_node_t *if_list_node = NULL;
	struct list_head *list_tmp = NULL;

	ENTRY
	if (if_list_head == NULL){
		LOGF_LOG_ERROR("Can't get requested interface %s, Interface list is not initialized\n", ifname)
		return NULL;
	}

	list_for_each(list_tmp, if_list_head){
		if_list_node = list_entry(list_tmp, qos_if_node_t, list);
		if (strcmp (if_list_node->ifname, ifname) == 0)
			return if_list_node;
	}

	EXIT
	return NULL;
}

/* =============================================================================
 *  Function Name : qosd_get_q_node
 *  Description   : Returns queue node matching specified queue name
		    in queue linked list.
		    Since there are two queue linked lists (each for ingress and egress)
		    caller has to pass the appropriate linked list to this API.
 * ============================================================================*/
qos_queue_node_t*
qosd_get_q_node(struct list_head *q_list_head, char *qname)
{
	qos_queue_node_t *q_node = NULL;
	struct list_head *list_tmp = NULL;

	ENTRY
	if (q_list_head == NULL){
		LOGF_LOG_ERROR("Can't get requested queue. Queue list is not initialized\n")
		return NULL;
	}

	list_for_each(list_tmp, q_list_head){
		q_node = list_entry(list_tmp, qos_queue_node_t, list);
		if (!strcmp(q_node->queue_cfg.name, qname))
			return q_node;
	}

	EXIT
	return NULL;
}

/*All add node functions should be merged into one*/
/* =============================================================================
 *  Function Name : qosd_add_class_node
 *  Description   : API to append new interface node to interface linked list in ES context.
 * ============================================================================*/
int32_t  
qosd_add_engine_rule(struct list_head **engine_rule_head, qos_class_rule_t **engine_rule_node)
{
	ENTRY
	if (*engine_rule_head == NULL){
		*engine_rule_head = (struct list_head *)os_calloc(1, sizeof(struct list_head));
		if(*engine_rule_head == NULL) {
			LOGF_LOG_CRITICAL("Memory allocation for interface node failed\n");
			return LTQ_FAIL;
		}
		INIT_LIST_HEAD(*engine_rule_head);
	}

	*engine_rule_node = (qos_class_rule_t *)os_calloc(1, sizeof(qos_class_rule_t));
	if(*engine_rule_node == NULL) {
		LOGF_LOG_CRITICAL("Memory allocation for interface node failed\n");
		return LTQ_FAIL;
	}
	INIT_LIST_HEAD(&((*engine_rule_node)->list));
 	list_add_tail( &((*engine_rule_node)->list), *engine_rule_head);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_del_engine_rule
 *  Description   : Internal API to delete node for engine rule
 * ============================================================================*/
int32_t
qosd_del_engine_rule(struct list_head **engine_rules_list_head, qos_class_rule_t *engine_rule_node)
{
	ENTRY
	if(engine_rule_node != NULL)
		_list_del(&engine_rule_node->list);

	os_free(engine_rule_node);
	engine_rule_node = NULL;

	/* Free the head if list becomes empty*/
	if (list_empty(*engine_rules_list_head)) {
		os_free(*engine_rules_list_head);
		*engine_rules_list_head = NULL;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_del_class_node
 *  Description   : Internal API to delete class node from the list
 * ============================================================================*/
int32_t
qosd_del_class_node(struct list_head **class_list_head, qos_class_node_t *class_list_node)
{
	ENTRY
	if(class_list_node != NULL)
		_list_del(&class_list_node->list);

	os_free(class_list_node);
	class_list_node = NULL;

	/* Free the head if list becomes empty*/
	if (list_empty(*class_list_head)) {
		os_free(*class_list_head);
		*class_list_head = NULL;
	}

	EXIT
	return LTQ_SUCCESS;
}
/* =============================================================================
 *  Function Name : qosd_add_class_node
 *  Description   : API to append new interface node to interface linked list in ES context.
 * ============================================================================*/
int32_t  
qosd_add_class_node(struct list_head **class_list_head, qos_class_node_t **class_list_node)
{
	ENTRY
	if (*class_list_head == NULL){
		*class_list_head = (struct list_head *)os_calloc(1, sizeof(struct list_head));
		if(*class_list_head == NULL) {
			LOGF_LOG_CRITICAL("Memory allocation for interface node failed\n");
			return LTQ_FAIL;
		}
		INIT_LIST_HEAD(*class_list_head);
	}

	*class_list_node = (qos_class_node_t *)os_calloc(1, sizeof(qos_class_node_t));
	if(*class_list_node == NULL) {
		LOGF_LOG_CRITICAL("Memory allocation for interface node failed\n");
		return LTQ_FAIL;
	}
	INIT_LIST_HEAD(&((*class_list_node)->list));
 	list_add_tail( &((*class_list_node)->list), *class_list_head);

	EXIT
	return LTQ_SUCCESS;
}
/* =============================================================================
 *  Function Name : qosd_add_if_node
 *  Description   : API to append new interface node to interface linked list in ES context.
 * ============================================================================*/
int32_t  
qosd_add_if_node(struct list_head **if_list_head, char *ifname, qos_if_node_t **if_list_node)
{
	ENTRY
	if (*if_list_head == NULL){
		*if_list_head = (struct list_head *)os_calloc(1, sizeof(struct list_head));
		if(*if_list_head == NULL) {
			LOGF_LOG_CRITICAL("Memory allocation for interface node failed\n");
			return LTQ_FAIL;
		}
		INIT_LIST_HEAD(*if_list_head);
	}
	else {
		*if_list_node = qosd_get_if_node(*if_list_head, ifname);
		if(*if_list_node != NULL) {
			LOGF_LOG_INFO("Interface add failed. Interface [%s] already exists\n", ifname);
			return QOS_ENTRY_EXISTS;
		}
	}

	*if_list_node = (qos_if_node_t *)os_calloc(1, sizeof(qos_if_node_t));
	if(*if_list_node == NULL) {
		LOGF_LOG_CRITICAL("Memory allocation for interface node failed\n");
		return LTQ_FAIL;
	}
	INIT_LIST_HEAD(&((*if_list_node)->list));
 	list_add_tail( &((*if_list_node)->list), *if_list_head);

	if(strncpy_s((*if_list_node)->ifname, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN) != EOK) {
		LOGF_LOG_ERROR("Error copying ifname string\n");
		return LTQ_FAIL;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_add_q_node
 *  Description   : API to append new queue node in queue linked list in ES context.
		    Since there are two queue linked lists (each for ingress and egress)
		    caller has to pass the appropriate linked list to this API.
 * ============================================================================*/
qos_queue_node_t*  
qosd_add_q_node(struct list_head **q_list_head)
{
	qos_queue_node_t *q_list_node = NULL;

	ENTRY
	if (*q_list_head == NULL){
		*q_list_head = (struct list_head *)os_calloc(1, sizeof(struct list_head));
		if(*q_list_head == NULL) {
			LOGF_LOG_CRITICAL("Memory allocation for queue node failed\n");
			return NULL;
		}
		INIT_LIST_HEAD(*q_list_head);
	}

	q_list_node = (qos_queue_node_t *)os_calloc(1, sizeof(qos_queue_node_t));
	list_add_tail( &(q_list_node->list), *q_list_head);

	EXIT
	return q_list_node;
}

/* TODO: Can we move act_if_cnt handling to caller ?*/
/* =============================================================================
 *  Function Name : qosd_del_if_node
 *  Description   : Removes an interface node from interface linked list
		    stored in ES context.
		    Handling dependent objects such as queue / classifier nodes
		    are to be handled by caller before calling this API.
 * ============================================================================*/
int32_t  
qosd_del_if_node(struct list_head **if_list_head, char *ifname, es_tables_t *es_tables)
{
	qos_if_node_t *if_list_node = NULL;

	ENTRY
	if_list_node = qosd_get_if_node(*if_list_head, ifname);
	if (if_list_node == NULL){
		LOGF_LOG_ERROR("Interface remove failed. Interface [%s] does not exist\n", ifname);
		return QOS_ENTRY_NOT_FOUND;
	}
	else {
		if(es_tables->act_if_cnt == 1) {
			/* only one entry in the list, need to take care of head ptr also */
			/* below loop will be executed only once */
			list_del_init(*if_list_head);
		}
		else {
			/* removing entry in between the list. TODO check for failure when entry is last in the list */
			_list_del(&if_list_node->list);
		}
		os_free(if_list_node);
		if_list_node = NULL;
		es_tables->act_if_cnt--;
	}

	/* Free the head if list becomes empty*/
	if (list_empty(*if_list_head)) {
		os_free(*if_list_head);
		*if_list_head = NULL;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_del_q_node
 *  Description   : Removes a queue node from queue linked list stored
		    in ES context.
		    Since there are two queue linked lists (each for ingress and egress)
		    caller has to pass the appropriate linked list to this API.
 * ============================================================================*/
int32_t
qosd_del_q_node(qos_if_node_t* if_list_node, qos_queue_cfg_t *q)
{
	qos_queue_node_t *q_list_node = NULL;

	ENTRY
	q_list_node = qosd_get_q_node(if_list_node->q_list_head, q->name);
	if(if_list_node->q_count == 1) {
		/* only one entry in the list, need to take care of head ptr also */
		/* below loop will be executed only once */
		if(q_list_node != NULL)
			list_del_init(if_list_node->q_list_head);
		if_list_node->q_list_head = NULL;
	}
	else {
		/* removing entry in between the list. TODO check for failure when entry is last in the list */
		if(q_list_node != NULL)
			_list_del(&q_list_node->list);
	}
	os_free(q_list_node);
	q_list_node = NULL;

	/* Free the head if list becomes empty*/
	if(if_list_node->q_list_head != NULL) {
		if (list_empty(if_list_node->q_list_head)) {
			os_free(if_list_node->q_list_head);
			if_list_node->q_list_head = NULL;
		}
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_get_iftype
 *  Description   : Maps given interface to one of interface category
		    available from ES context.
 * ============================================================================*/
iftype_t qosd_get_iftype (char *ifname, if_category_map_t *ifmap)
{
	iftype_t ret = QOS_IF_CATEGORY_MAX;
	int32_t i = 0, j = 0;
	char *substr;

	ENTRY
	for (i=0; i<QOS_IF_CATEGORY_MAX; i++){
		for(j=0; j<ifmap[i].ifcount; j++) {
			if (strstr_s(ifmap[i].ifnames[j], MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN), &substr) == EOK)
				ret = (iftype_t) i;
		}
	}

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_get_ifinggrp
 *  Description   : Maps given interface to one of ingress groups
		    available from ES context.
 * ============================================================================*/
ifinggrp_t qosd_get_ifinggrp (char *ifname, if_inggrp_map_t *if_inggrp_map)
{
	ifinggrp_t ret = QOS_IF_CAT_INGGRP_MAX;
	int32_t i = 0, j = 0;

	ENTRY
	for (i=0; i<QOS_IF_CAT_INGGRP_MAX; i++) {
		for(j = 0; j < MAX_IF_PER_INGRESS_GROUP; j++) {
			if (!strncmp(if_inggrp_map[i].ifnames[j], ifname, MAX_IF_NAME_LEN)) {
				ret = (ifinggrp_t)if_inggrp_map[i].inggrp;
				break;
			}
		}
	}

	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : qosd_show_iftype
 *  Description   : Show Map between interfaces and interface category
		    available from ES context.
 * ============================================================================*/
static iftype_t qosd_show_iftype (if_category_map_t *ifmap)
{
	iftype_t ret = QOS_IF_CATEGORY_MAX;
	int32_t i = 0, j = 0;

	ENTRY
	LOGF_LOG_INFO("\nActive Interfaces\n");
	LOGF_LOG_INFO("----------------------------\n");
	LOGF_LOG_INFO("%10s | %15s | %15s\n", "Name", "Type", "Base Interface");

	for (i=0; i<QOS_IF_CATEGORY_MAX; i++) {
		for(j=0; j<ifmap[i].ifcount; j++) {
			LOGF_LOG_INFO("%10s | %15d | %15s\n", ifmap[i].ifnames[j], i, ifmap[i].base_ifnames[j]);
		}
	}

	EXIT
	return ret;
}


/* =============================================================================
 *  Function Name : qosd_str_to_int
 *  Description   : Returns number in string format inside integer variable.
 * ============================================================================*/
int32_t qosd_str_to_int(char const *param, int32_t *data)
{
	int32_t k = 0;
	char *ptr = NULL;
	char *str = NULL;
	char *next_token;
	rsize_t strmax;

	str = (char *) os_malloc(strnlen_s(param, MAX_QOS_NAME_LEN)+1);
	if(str == NULL) {
		LOGF_LOG_CRITICAL("Memory allocation failed\n");
		return LTQ_FAIL;
	}

	if(strncpy_s(str, MAX_QOS_NAME_LEN, param, MAX_QOS_NAME_LEN) != EOK) {
		LOGF_LOG_ERROR("Error copying string\n");
		return LTQ_FAIL;
	}

	strmax = strnlen_s(str, MAX_QOS_NAME_LEN);
	ptr = strtok_s(str, &strmax, ",", &next_token);
	while (ptr != NULL) {
		data[k++] = atoi(ptr);
		ptr = strtok_s(NULL, &strmax, ",", &next_token);
	}

	os_free(str);

	return k;
}

/* =============================================================================
 *  Function Name : qosd_get_str_drop_mode
 *  Description   : Returns string form of drop algorithm.
 * ============================================================================*/
char * qosd_get_str_drop_mode(qos_drop_mode_t drop_mode)
{
	switch(drop_mode) {
		case QOS_DROP_TAIL:
			return "Tail Drop";
			break;
		case QOS_DROP_RED:
			return "Random Early Detection";
			break;
		case QOS_DROP_WRED:
			return "Weighted Random Early Detection";
			break;
		case QOS_DROP_CODEL:
			return "Controlled Delay";
			break;
	}
	return NULL;
}

/* =============================================================================
 *  Function Name : qosd_get_str_sched_mode
 *  Description   : Returns string form of scheduling algorithm.
 * ============================================================================*/
char * qosd_get_str_sched_mode(qos_sched_mode_t sched_mode)
{
	switch(sched_mode) {
		case QOS_SCHED_SP:
			return "Strict priority";
			break;
		case QOS_SCHED_WFQ:
			return "Weighted Fair Queuing";
			break;
	}
	return NULL;
}

/* =============================================================================
 *  Function Name : qosd_get_str_shaper
 *  Description   : Returns string form of shaper mode.
 * ============================================================================*/
char * qosd_get_str_shaper(qos_shaper_mode_t shaper)
{
	switch(shaper) {
		case QOS_SHAPER_NONE:
			return "None";
			break;
		case QOS_SHAPER_COLOR_BLIND:
			return "Color blind";
			break;
		case QOS_SHAPER_TR_TCM:
			return "Three color marking";
			break;
		case QOS_SHAPER_TR_TCM_RFC4115:
			return "Three color marking as in RFC4115";
			break;
		case QOS_SHAPER_LOOSE_COLOR_BLIND:
			return "Loose color blind";
			break;
	}
	return NULL;
}

/* =============================================================================
 *  Function Name : qosd_show_cfg
 *  Description   : Prints complete ES context. That is,
		    a. platform name
		    b. interface categories
		    c. supported engines along with individual registration status
		    d. datapaths between each possible pair of interface categories
		    e. queues and classifiers details with parent interface
 * ============================================================================*/
int32_t qosd_show_cfg(es_tables_t *tbl)
{
	int32_t i = 0;

	/* print platform name */
	LOGF_LOG_INFO("\n--------------------------------------------------------------------------------\n");
	LOGF_LOG_INFO("Platform name [%s]\n", tbl->plat_name);

	/* print interface categories */
	LOGF_LOG_INFO("\n-----------------------------------------------\n");
	LOGF_LOG_INFO("Interface categories available on this platform\n");
	for(i=0; i<tbl->number_of_engines; i++) {
		LOGF_LOG_INFO("%d. %s\n", i+1, tbl->ifcat[i])
	}

	/* print engines */
	LOGF_LOG_INFO("\nEngines available on this platform\n");
	LOGF_LOG_INFO("----------------------------\n")
	LOGF_LOG_INFO("%10s | %15s\n", "Name", "Registered")
	for(i=0; i<tbl->number_of_engines; i++) {
		LOGF_LOG_INFO("%10s | %15s\n", tbl->eng_names[i], (tbl->engine_info[i].status)?"Yes":"No")
	}
	LOGF_LOG_INFO("%10s | %15s\n", "Name", "Q Capability")
	for(i=0; i<tbl->number_of_engines; i++) {
		LOGF_LOG_INFO("%10s | %u\n", tbl->eng_names[i], (tbl->engine_info[i].engine.engine_capab.q_cap));
	}
	LOGF_LOG_INFO("%10s | %15s\n", "Name", "Class Capability")
	for(i=0; i<tbl->number_of_engines; i++) {
		LOGF_LOG_INFO("%10s | %u\n", tbl->eng_names[i], (tbl->engine_info[i].engine.engine_capab.cl_cap.cap));
	}
	LOGF_LOG_INFO("----------------------------\n")

	/* print system datapaths */
	qosd_show_dps(tbl);

	/* print datapaths between various interface categories */
	LOGF_LOG_INFO("\n--------------------------------------------------------------------------------\n");
	qosd_show_dps_bw_ifaces(tbl);

	if(qosd_show_iftype(tbl->ifmap) != LTQ_SUCCESS) { }

	/* print interfaces with queues and classifiers configured on each */

	LOGF_LOG_INFO("\n--------------------------------------------------------------------------------\n");

	if(qosd_show_queues() != LTQ_SUCCESS) { }

	if(qosd_show_classifers() != LTQ_SUCCESS) { }

	LOGF_LOG_INFO("--------------------------------------------------------------------------------\n");
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_show_engines
 *  Description   : Prints list of supported engines.
 * ============================================================================*/
int32_t qosd_show_engines(es_tables_t *tbl)
{
	int32_t i = 0;
	for(i=0; i<tbl->number_of_engines; i++) {
		LOGF_LOG_INFO("engine name [%d] is [%s]\n", i, tbl->eng_names[i])
	}
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_show_dps
 *  Description   : Prints generic system datapaths.
 * ============================================================================*/
int32_t
qosd_show_dps(es_tables_t *tbl)
{
	ENTRY
	int32_t i = 0, j = 0;
	int32_t iIndex = 0, jIndex = 0;

	/* as QOS_DATAPATH_MAX is in enum which starts with '0', check should
	 * be done for value equal to MAX.
	 */
	if(tbl->number_of_datapaths > MAX_SYS_DATAPATHS) {
		LOGF_LOG_ERROR("Number of datapaths defined in platform XML exceeds capability of QoS ES.\n");
		return LTQ_FAIL;
	}
	for (i=0; i<tbl->number_of_datapaths; i++) {
		iIndex = tbl->sys_datapaths[i].datapath_type;
		if((iIndex > 0) && strnlen_s(tbl->dp_types[iIndex - 1], MAX_NAME_LEN) > 0) {
			LOGF_LOG_DEBUG("\nSystem datapath : Id [%d], Type [%s] -->> ", tbl->sys_datapaths[i].datapath_id, tbl->dp_types[iIndex - 1])
		}
		j = 0;
		while (tbl->sys_datapaths[i].datapath[j]){
			jIndex = tbl->sys_datapaths[i].datapath[j];
			if(jIndex > 0) {
				LOGF_LOG_DEBUG("%s ",  tbl->eng_names[jIndex-1])
			}
			j++;
		}
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_show_dps_bw_ifaces
 *  Description   : Prints list of datapaths between given pair of interfaces.
 * ============================================================================*/
int32_t
qosd_show_dps_bw_ifaces(es_tables_t *tbl)
{
	ENTRY
	int32_t i = 0, j = 0, k = 0;

	for (i=0; i<tbl->act_ifgrp_cnt; i++) {
		for (j=0; j<tbl->act_ifgrp_cnt; j++) {
			k = 0;
			if (tbl->datapath_tbl[i][j][0])
				LOGF_LOG_DEBUG("\nSystem datapaths (ids) between interfaces %s and %s : -->> ",tbl->ifcat[i], tbl->ifcat[j])
			while (tbl->datapath_tbl[i][j][k]){
				LOGF_LOG_DEBUG ("%d ",tbl->datapath_tbl[i][j][k])
				k++;
			}
		}
	}
	LOGF_LOG_DEBUG("\n")

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : qosd_populate_tbls
 *  Description   : Reads platform config file and records within
		    ES context.
 * ============================================================================*/
int32_t
qosd_populate_tbls(char *file, es_tables_t *tbl)
{
	ENTRY
	const char *id;
	const char *name;
	const char *param;
	int32_t i = 0;
	int32_t ingress = 0, egress = 0;
	ezxml_t qos, engines, sys_dps, dps_bw_ifs, node, dp_type, ifcat;


	qos = ezxml_parse_file(file);

	node = ezxml_child(qos, "platform");
	if(node != NULL) {
		if(strncpy_s(tbl->plat_name, MAX_NAME_LEN, node->txt, MAX_NAME_LEN) != EOK) {
			LOGF_LOG_ERROR("Error copying string\n");
			return LTQ_FAIL;
		}
	}

	ifcat = ezxml_child(qos, "ifcategory");
	param = ezxml_attr(ifcat, "ifcat_count");

	for (node = ezxml_child(ifcat, "ifcat"); node; node = node->next) {
		id = ezxml_attr(node, "ifcat_id");
		name = node->txt;
		if(id != NULL) {
			i = atoi(id);
			if(i >= 0 && i < QOS_IF_CATEGORY_MAX) {
				strncpy_s(tbl->ifcat[i], MAX_IF_NAME_LEN, name, MAX_IF_NAME_LEN);
			}
			else {
				LOGF_LOG_ERROR("interface category supplied [%d] is not supported\n", i);
			}
		}
	}

	engines = ezxml_child(qos, "engines");
	param = ezxml_attr(engines, "engines_count");
	if(param != NULL) {
		tbl->number_of_engines = atoi(param);
	}
	else {
		tbl->number_of_engines = 0;
	}

	i = 0;
	for (node = ezxml_child(engines, "engine"); node; node = node->next){
		id = ezxml_attr(node, "engine_id");
		name = node->txt;
		snprintf(tbl->eng_names[i], MAX_NAME_LEN, "%s", name);
		if(id != NULL) {
			tbl->eng_ids[i++] = atoi(id);
		}
		else {
			tbl->eng_ids[i++] = 0;
		}
	}

	i = 0;
	dp_type = ezxml_child(qos, "datapath-type");
	for (node = ezxml_child(dp_type, "dp-type"); node; node = node->next){
		name = node->txt;
		snprintf(tbl->dp_types[i++], MAX_NAME_LEN, "%s", name);
	}

	sys_dps = ezxml_child(qos, "system-datapaths");
	param = ezxml_attr(sys_dps, "dp_count");

	if(param != NULL) {
		tbl->number_of_datapaths = atoi (param); //instead of static llocation we can use heap based on this number
		if (tbl->number_of_datapaths > MAX_SYS_DATAPATHS) {
			LOGF_LOG_ERROR("datapaths in the system are more than limit\n")
			tbl->number_of_datapaths = 0;
		}
	}
	else {
		tbl->number_of_datapaths = 0;
	}

	i = 0;
	for (node = ezxml_child(sys_dps, "datapath"); node; node = node->next) {
		param  = ezxml_attr(node, "dp_id");
		if(param != NULL) {
			tbl->sys_datapaths[i].datapath_id = atoi(param);
		}
		else {
			tbl->sys_datapaths[i].datapath_id = 0;
		}

		param  = ezxml_attr(node, "dp_type");
		if(param != NULL) {
			tbl->sys_datapaths[i].datapath_type = atoi(param);//put validation
		}
		else {
			tbl->sys_datapaths[i].datapath_type = 0;//put validation
		}

		param = node->txt;
		tbl->sys_datapaths[i].number_of_engines = qosd_str_to_int(param, tbl->sys_datapaths[i].datapath);
		i++;
	}

	dps_bw_ifs = ezxml_child(qos, "datapaths-bw-interfaces");
	param = ezxml_attr(dps_bw_ifs, "count");
	if(param != NULL) {
		tbl->number_of_datapaths_bw_ifs = atoi(param); //instead of static llocation we can use heap based on this number
	}
	else {
		tbl->number_of_datapaths_bw_ifs = 0;
	}

	i = 0;
	for (node = ezxml_child(dps_bw_ifs, "datapaths"); node; node = node->next){
		param  = ezxml_attr(node, "ingress");
		if(param != NULL) {
			ingress = atoi(param);
		}

		param  = ezxml_attr(node, "egress");
		if(param != NULL) {
			egress = atoi(param);
		}

		param = node->txt;

		if( (ingress > 0) && (ingress <= QOS_IF_CATEGORY_MAX) &&
		    (egress > 0) && (egress <= QOS_IF_CATEGORY_MAX) ) {
			qosd_str_to_int(param, tbl->datapath_tbl[ingress-1][egress-1]);
		}
		else {
			LOGF_LOG_ERROR("interface category supplied [%d/%d] is not supported\n", ingress, egress);
		}

		i++;
	}
	ezxml_free(qos);

	EXIT
	return LTQ_SUCCESS;
}
