#include "stdio.h"
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <errno.h>
#include <ctype.h>
#include <ezxml.h>
#include "qosal_inc.h"
#include "sysapi.h"
#include "qosal_utils.h"
#include "ppa_eng.h"
#include <sys/ioctl.h>
#include <net/ppa_api.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

#define PPA_DEVICE   "/dev/ifx_ppa"
#define DEV_LIST_NAME_MAX_LEN	128
#define LAN_PORT_NUM	4
uint32_t port_rates[QOS_IF_CATEGORY_MAX + 1] = {0};
uint32_t lan_port_rates[LAN_PORT_NUM + 1] = {0};
uint32_t ing_port_shaper[NUM_INGRESS_GROUPS + 1] = {0};
uint32_t ing_port_shaper_rate[NUM_INGRESS_GROUPS + 1] = {0};
uint32_t port_shape = 0;
#define MAX_WLAN_NUM 3
uint32_t port_shape_wlan[MAX_WLAN_NUM] = {0,0,0};
uint8_t wlan_index = MAX_WLAN_NUM;
uint8_t lan_index = LAN_PORT_NUM + 1;
uint32_t port_pir = 0;
uint32_t port_pir_lan = 0;
uint32_t port_cir = 0;
uint32_t port_pbs = 0;
uint32_t port_cbs = 0;
uint32_t def_burst = 8000;
uint8_t def_shaper_mode = 1;
int32_t high_q_limit = 9;
iftype_t g_iftype;
bool xml_ifinggrp_en = false;
char xml_ifinggrp[NUM_INGRESS_GROUPS][MAX_IF_PER_INGRESS_GROUP][MAX_NAME_LEN];
ifinggrp_t xml_lookup_ingress_group(const char *ifname);

char *lan_dev[4] = {"eth0_1","eth0_2","eth0_3","eth0_4"};
static char *sDefLanIf = "br-lan";
char * lan_str = "eth0,br-lan,eth0_1,eth0_2,eth0_3,eth0_4";
char * lanbase_str = "eth0,br-lan";
char * lansep_str = "eth0_1,eth0_2,eth0_3,eth0_4";

static int32_t ppa_eng_port_add (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_shaper_t  *shaper, uint32_t priority, uint32_t weight,uint32_t flags);
static int32_t ppa_eng_port_del (char *ifname,char *orig_ifname, iftype_t iftype, qos_shaper_t  *shaper, uint32_t priority, uint32_t weight,uint32_t flags);
static int32_t add_qos_queue(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg,int32_t q_num);
static int32_t delete_qos_queue(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg,int32_t q_num);
static inline void util_atoi(int32_t *dst, char *src)
{
        *dst = atoi(src);
}

static int32_t get_qcfg_from_xml(qos_queue_cfg_t *q_cfg, uint32_t cfg_flags)
{
	ENTRY
	int ret = LTQ_SUCCESS;
	ezxml_t qos_ctxt, qnode_ctxt, node_ctxt;
	qos_ctxt = qnode_ctxt = node_ctxt = NULL;
	int32_t i, nRet, q_count,j,k;
	q_count = nRet = i = j = k = 0;
	const char *tmp_par;
	char *param;
	int32_t tc_val[16];
	char sName[128] = {0};

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	if (cfg_flags & WMM_QOS_CFG)
		qnode_ctxt = ezxml_child(qos_ctxt, "wmm-queues");
	else if (cfg_flags & DOS_QOS_CFG)
		qnode_ctxt = ezxml_child(qos_ctxt, "dos-queues");

	if (qnode_ctxt == NULL) {
		goto exit_lbl;
	}
	tmp_par = ezxml_attr(qnode_ctxt, "queues_count");

	if (tmp_par != NULL) {
		q_count = atoi(tmp_par);
		if (q_count < 0 || q_count > MAX_TC_NUM) {
			goto exit_lbl;
		}
	}

	for (i = 0; i < q_count; i++) {
		if (cfg_flags & WMM_QOS_CFG)
			snprintf(sName, sizeof(sName), "wmm-q%d", (i + 1));
		else if (cfg_flags & DOS_QOS_CFG)
			snprintf(sName, sizeof(sName), "dos-q%d", (i + 1));

		qnode_ctxt = ezxml_child(qos_ctxt, sName);
		if (qnode_ctxt == NULL)
			continue;
		node_ctxt = ezxml_child(qnode_ctxt, "Name");
		if (node_ctxt != NULL) {
			strncpy_s(q_cfg[i].name, MAX_Q_NAME_LEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, MAX_Q_NAME_LEN) + 1);
		}

		node_ctxt = ezxml_child(qnode_ctxt, "Queue_id");
		if (node_ctxt != NULL) {
			util_atoi((int *)&q_cfg[i].queue_id, node_ctxt->txt);
		}

		node_ctxt = ezxml_child(qnode_ctxt, "Tc_map");
		if (node_ctxt != NULL) {
			param = node_ctxt->txt;
			if (param != NULL) {
				q_cfg[i].tc_map[0] = atoi(node_ctxt->txt);
				j = qosd_str_to_int(param,tc_val);
				for (k = 0; k< j; k++)
				{
					q_cfg[i].tc_map[k] = tc_val[k];
					LOGF_LOG_DEBUG(" Info from default WMM xml : tc_val[%d] = %d : no. of TC set = %d\n",k,tc_val[k],j);
				}
			}
		}
		node_ctxt = ezxml_child(qnode_ctxt, "Priority");
		if (node_ctxt != NULL) {
			util_atoi(&q_cfg[i].priority, node_ctxt->txt);
		}
		node_ctxt = ezxml_child(qnode_ctxt, "Sched");
		if (node_ctxt != NULL) {
			if (strcmp(node_ctxt->txt, "SP") == 0)
				q_cfg[i].sched = QOS_SCHED_SP;
			else if (strcmp(node_ctxt->txt, "WFQ") == 0)
			{
				q_cfg[i].sched = QOS_SCHED_WFQ;
				node_ctxt = ezxml_child(qnode_ctxt, "Weight");
				util_atoi(&q_cfg[i].weight, node_ctxt->txt);
			}
		}
		node_ctxt = ezxml_child(qnode_ctxt, "shaper_en");
		if (node_ctxt != NULL) {
			util_atoi(&q_cfg[i].shaper.enable, node_ctxt->txt);
		}
#if 0 /* Shaper disabled for default WMM Queues */
		node_ctxt = ezxml_child(qnode_ctxt, "pir");
		util_atoi(&q_cfg[i].shaper.pir, node_ctxt->txt);
		node_ctxt = ezxml_child(qnode_ctxt, "cir");
		util_atoi(&q_cfg[i].shaper.cir, node_ctxt->txt);
		node_ctxt = ezxml_child(qnode_ctxt, "shaper_mode");
		util_atoi(&q_cfg[i].shaper.mode, node_ctxt->txt);
		node_ctxt = ezxml_child(qnode_ctxt, "pbs");
		util_atoi(&q_cfg[i].shaper.pbs, node_ctxt->txt);
		node_ctxt = ezxml_child(qnode_ctxt, "cbs");
		util_atoi(&q_cfg[i].shaper.cbs, node_ctxt->txt);
#endif
		node_ctxt = ezxml_child(qnode_ctxt, "Ingress_group");
		if (node_ctxt != NULL) {
			util_atoi((int *)&q_cfg[i].inggrp, node_ctxt->txt);
			q_cfg[i].flags |= QOS_Q_F_INGRESS;
		}

		node_ctxt = ezxml_child(qnode_ctxt, "RedThreshold");
		if (node_ctxt != NULL) {
			util_atoi((int *)&q_cfg[i].drop.wred.drop_th1, node_ctxt->txt);
			q_cfg[i].drop.flags |= (QOS_OP_F_QUEUE_DROP | QOS_DT_F_RED_THRESHOLD);
			q_cfg[i].drop.mode = QOS_DROP_TAIL;
			q_cfg[i].drop.enable = QOSAL_ENABLE;
		}
		q_cfg[i].enable = 1;
	}

exit_lbl:
	return ret;
}

static int32_t get_metercfg_from_xml(qos_meter_t *meter_cfg)
{
	ENTRY
	int ret;
	ezxml_t qos_ctxt, meter_node_ctxt, node_ctxt;
	int32_t nRet, meter_count, i, j, k;
	const char *tmp_par;
	char sName[128];

	ret = LTQ_SUCCESS;
	qos_ctxt = meter_node_ctxt = node_ctxt = NULL;
	meter_count = nRet = i = j = k = 0;

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	meter_node_ctxt = ezxml_child(qos_ctxt, "dos-meters");

	if (meter_node_ctxt == NULL) {
		goto exit_lbl;
	}
	tmp_par = ezxml_attr(meter_node_ctxt, "meter_count");

	if (tmp_par != NULL) {
		meter_count = atoi(tmp_par);
		if (meter_count < 0 || meter_count > MAX_METER_NUM) {
			goto exit_lbl;
		}
	}

	for (i = 0; i < meter_count; i++) {
		snprintf(sName, sizeof(sName), "dos-mtr%d", (i + 1));
		meter_node_ctxt = ezxml_child(qos_ctxt, sName);
		if (meter_node_ctxt == NULL)
			continue;

		node_ctxt = ezxml_child(meter_node_ctxt, "Mode");
		if (node_ctxt != NULL) {
			util_atoi((int *)&meter_cfg[i].mode, node_ctxt->txt);
		}
		node_ctxt = ezxml_child(meter_node_ctxt, "Enable");
		if (node_ctxt != NULL) {
			util_atoi((int *)&meter_cfg[i].enable, node_ctxt->txt);
		}
		node_ctxt = ezxml_child(meter_node_ctxt, "CIR");
		if (node_ctxt != NULL) {
			util_atoi((int *)&meter_cfg[i].cir, node_ctxt->txt);
		}
		node_ctxt = ezxml_child(meter_node_ctxt, "PIR");
		if (node_ctxt != NULL) {
			util_atoi((int *)&meter_cfg[i].pir, node_ctxt->txt);
		}
		node_ctxt = ezxml_child(meter_node_ctxt, "CBS");
		if (node_ctxt != NULL) {
			util_atoi((int *)&meter_cfg[i].cbs, node_ctxt->txt);
		}
		node_ctxt = ezxml_child(meter_node_ctxt, "PBS");
		if (node_ctxt != NULL) {
			util_atoi((int *)&meter_cfg[i].pbs, node_ctxt->txt);
		}
		node_ctxt = ezxml_child(meter_node_ctxt, "MeterID");
		if (node_ctxt != NULL) {
			util_atoi((int *)&meter_cfg[i].meterid, node_ctxt->txt);
		}
	}

exit_lbl:
	EXIT
	return ret;
}

int32_t xml_parse_ingress_groups(void)
{
	int i, j, nRet = LTQ_SUCCESS;
	ezxml_t qos_ctxt, node_ctxt, if_node;
	const char *attr;
	char *name;
	int32_t nIfCnt = 0, iIndex = 0;

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	if_node = ezxml_child(qos_ctxt, "ifgroup");
	if(if_node == NULL) {
		goto exit_lbl;
	}
	attr = ezxml_attr(if_node, "inuse");
	if(attr == NULL || (attr != NULL && atoi(attr) == 0)) {
		LOGF_LOG_DEBUG("XML Ingress Groups disabled\n");
		goto exit_lbl;
	}
	xml_ifinggrp_en = true;
	for (node_ctxt = ezxml_child(if_node, "group"); node_ctxt; node_ctxt = node_ctxt->next){
		nIfCnt = 0;
		name = node_ctxt->txt;

		attr = ezxml_attr(node_ctxt, "id");
		if(attr == NULL || name == NULL) {
			continue;
		}
		iIndex =  atoi(attr);
		if ( iIndex >= 0 && (size_t) iIndex < sizeof(xml_ifinggrp)/sizeof(*xml_ifinggrp) )
			splitStr(name, ",", xml_ifinggrp[iIndex], &nIfCnt);
	}

	LOGF_LOG_DEBUG("XML Ingress Groups:\n");
	for (i = 0; i < NUM_INGRESS_GROUPS; i++) {
		LOGF_LOG_DEBUG("Ingress Group %d:\n", i);
		for (j = 0; j < MAX_IF_PER_INGRESS_GROUP && xml_ifinggrp[i][j][0]; j++) {
			LOGF_LOG_DEBUG("  %s\n", xml_ifinggrp[i][j]);
		}
	}

exit_lbl:
	return nRet;
}

ifinggrp_t xml_lookup_ingress_group(const char *ifname)
{
	int i, j;

	if (!xml_ifinggrp_en)
		return QOS_IF_CAT_INGGRP_MAX;

	for(i = 0; i < NUM_INGRESS_GROUPS; i++) {
		for (j = 0; j < MAX_IF_PER_INGRESS_GROUP; j++) {
			if (!strncmp(xml_ifinggrp[i][j], ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)))
				return (ifinggrp_t)i;
		}
	}

	return QOS_IF_CAT_INGGRP_MAX;
}

static int32_t del_wmm_qos_queue(char *ifname, iftype_t iftype, qos_queue_cfg_t *qcfg, int q_num)
{
	PPA_CMD_QOS_QUEUE_INFO q_info;
	int ret = LTQ_SUCCESS;

        memset_s(&q_info, sizeof(PPA_CMD_QOS_QUEUE_INFO), 0x00);

	UNUSED(iftype);
	q_info.queue_num = q_num;
	q_info.shaper_num = q_num;
	q_info.priority = qcfg->priority;
	strncpy_s(q_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);

	if(qcfg->flags & QOS_Q_F_DEFAULT)
	q_info.flags |= PPA_QOS_Q_F_DEFAULT;
	if(qcfg->flags & QOS_Q_F_INGRESS)
	q_info.flags |= PPA_QOS_Q_F_INGRESS;
	if(iftype == QOS_IF_CATEGORY_WLANNDP)
	q_info.flags |= PPA_QOS_Q_F_WLANDP;
	ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_DEL_WMM_QOS_QUEUE, &q_info);

	if( ret != LTQ_SUCCESS ) {
		LOGF_LOG_ERROR("Ioctl Del QoS Queue Failed with [%d] \n", ret);
		ret = QOS_PPA_ENG_CFG_FAIL;
	}
	
	return ret;
}

static int32_t add_wmm_qos_queue(char *ifname,iftype_t iftype,qos_queue_cfg_t *qcfg,int q_num)
{
	/* Declare PPA structures:START */
	int ret = LTQ_SUCCESS,i,k=0;
	PPA_CMD_QOS_QUEUE_INFO q_info;
	PPA_CMD_RATE_INFO rate_info;
	qos_shaper_t  port_shaper;
        
	UNUSED(iftype);
	memset_s(&q_info, sizeof(PPA_CMD_QOS_QUEUE_INFO), 0x00);
        memset_s(&rate_info, sizeof(PPA_CMD_RATE_INFO), 0x00);
        memset_s(&port_shaper, sizeof(qos_shaper_t), 0x00);
	/* Declare PPA structures:END */
	strncpy_s(q_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
	q_info.queue_num = q_num;
	q_info.shaper_num = q_num;
	q_info.sched = qcfg->sched;
	q_info.weight = qcfg->weight;
	
	if(qcfg->flags & QOS_Q_F_INGRESS)
		q_info.flags |= PPA_QOS_Q_F_INGRESS;
	if(qcfg->flags & QOS_Q_F_DEFAULT)
		q_info.flags |= PPA_QOS_Q_F_DEFAULT;
	q_info.priority = (qcfg->priority);
	for(i = 0; i < DEFAULT_TC; i++)
	{
		q_info.tc_map[i] = (DEFAULT_TC - qcfg->tc_map[i]);
		LOGF_LOG_DEBUG(" Add WMM Queue with tc_map = %d \n",q_info.tc_map[i]);
		if (qcfg->tc_map[i] != 0) /*Should actually check against -1 once the def. value set in tc_map[] by the framework is -1*/
			k++;
	}
	q_info.tc_no = k;
	q_info.shaper.enable = qcfg->shaper.enable;
	ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_ADD_WMM_QOS_QUEUE, &q_info);
	if(ret != LTQ_SUCCESS)
	{
		LOGF_LOG_ERROR("Ioctl Add QoS Queue Failed with [%d]\n", ret);
		ret = QOS_PPA_ENG_CFG_FAIL;
		goto Err_Hndl;
	}
	/* Filling Shaper & Rate  Info */
	/* No Queue Shaper for Wmm Queues */
Err_Hndl:
	return ret;
}

int32_t init_wmm_queue_cfg(uint32_t oper_flag)
{
	int ret = LTQ_SUCCESS,i=0;
	int32_t nRet, q_count,q_num=100;
	qos_queue_cfg_t *q_cfg = NULL;
	ezxml_t qos_ctxt, qnode_ctxt;
	ezxml_t node_ctxt;
	q_count = nRet = 0;
	const char *tmp_par;
	char ifname[16] = {0};
	char sName[128] = {0};
	iftype_t iftype = QOS_IF_CATEGORY_WLANDP;

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	qnode_ctxt = ezxml_child(qos_ctxt, "wmm-queues");
	if(qnode_ctxt == NULL) {
		goto exit_lbl;
	}
	tmp_par = ezxml_attr(qnode_ctxt, "queues_count");
	if(tmp_par != NULL) {
		q_count = atoi(tmp_par);
	}

/*
	// No Port Shaper for Wmm Queues 
	qnode_ctxt = ezxml_child(qos_ctxt, "wmm-ports");
	tmp_par = ezxml_attr(qnode_ctxt, "ports_count");
	p_count = atoi(tmp_par);
*/	
	if(q_count <= 0 || q_count > MAX_TC_NUM) {
		goto exit_lbl;
	}
	q_cfg = (qos_queue_cfg_t *)os_calloc(1,(q_count * sizeof(qos_queue_cfg_t)));
	get_qcfg_from_xml(q_cfg, WMM_QOS_CFG);

	i=0;	
	for(i=0;i<q_count;i++)
	{
		snprintf(sName, sizeof(sName), "wmm-q%d", (i + 1));
		qnode_ctxt = ezxml_child(qos_ctxt, sName);
		if(qnode_ctxt == NULL) {
			continue;
		}
		node_ctxt = ezxml_child(qnode_ctxt, "Interface");
		if(node_ctxt != NULL) {
			strncpy_s(ifname, MAX_IF_NAME_LEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, MAX_IF_NAME_LEN) + 1);
		}
		node_ctxt = ezxml_child(qnode_ctxt, "Queue_id");
		if(node_ctxt != NULL) {
			util_atoi(&q_num, node_ctxt->txt);
		}
		/* calladdqueue */
		if(oper_flag == 1)
		add_wmm_qos_queue(ifname, iftype, &q_cfg[i],q_num);
		else if(oper_flag == 0)
		del_wmm_qos_queue(ifname, iftype, &q_cfg[i],q_num);
	}
	os_free(q_cfg);
exit_lbl:
	return ret;
}

int32_t dos_q_cfg(uint32_t oper_flag) {
	ENTRY
	int32_t nRet = LTQ_SUCCESS;
	int i, q_count;
	ezxml_t qos_ctxt, qnode_ctxt;
	ezxml_t node_ctxt;
	q_count = i = 0;
	qos_queue_cfg_t *q_cfg = NULL;
	const char *tmp_par;
	char ifname[MAX_IFNAME_LEN] = {0};
	char sName[MAX_Q_NAME_LEN] = {0};
	iftype_t iftype = QOS_IF_CATEGORY_LOCAL;

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	qnode_ctxt = ezxml_child(qos_ctxt, "dos-queues");
	if (qnode_ctxt == NULL) {
		goto exit_lbl;
	}
	tmp_par = ezxml_attr(qnode_ctxt, "queues_count");
	if (tmp_par != NULL) {
		q_count = atoi(tmp_par);
	}

	if (q_count <= 0 || q_count > MAX_TC_NUM) {
		goto exit_lbl;
	}
	q_cfg = (qos_queue_cfg_t *)os_calloc(1,(q_count * sizeof(qos_queue_cfg_t)));
	if (q_cfg == NULL) {
		LOGF_LOG_CRITICAL("Memory Allocation Failed\n");
		nRet = LTQ_FAIL;
		goto exit_lbl;
	}
	get_qcfg_from_xml(q_cfg, DOS_QOS_CFG);

	for(i = 0; i < q_count; i++)
	{
		snprintf(sName, sizeof(sName), "dos-q%d", (i + 1));
		qnode_ctxt = ezxml_child(qos_ctxt, sName);
		if (qnode_ctxt == NULL) {
			continue;
		}
		node_ctxt = ezxml_child(qnode_ctxt, "Interface");
		if (node_ctxt != NULL) {
			strncpy_s(ifname, MAX_IF_NAME_LEN, node_ctxt->txt, strnlen_s(node_ctxt->txt, MAX_IF_NAME_LEN) + 1);
		}

		if (oper_flag == QOSAL_ENABLE) {
			qosd_if_abs_set(iftype, ifname, QOS_OP_F_ADD);
			qosd_if_base_set(ifname, ifname, ifname, QOS_OP_F_ADD);
			add_qos_queue(ifname, iftype, q_cfg[i].inggrp, &q_cfg[i], q_cfg[i].queue_id);
		}
		else if (oper_flag == QOSAL_DISABLE)
			delete_qos_queue(ifname, iftype, q_cfg[i].inggrp, &q_cfg[i], q_cfg[i].queue_id);
	}
	os_free(q_cfg);
exit_lbl:
	EXIT
	return nRet;
}

int32_t dos_meter_cfg(uint32_t oper_flag) {
	ENTRY
	int32_t nRet = LTQ_SUCCESS;
	int i, meter_count;
	ezxml_t qos_ctxt, meter_node_ctxt;
	meter_count = i = 0;
	qos_meter_t *meter_cfg = NULL;
	const char *tmp_par;
	char sName[128];

	qos_ctxt = ezxml_parse_file(QOS_DEF_Q_FILE);

	meter_node_ctxt = ezxml_child(qos_ctxt, "dos-meters");
	if (meter_node_ctxt == NULL) {
		goto exit_lbl;
	}
	tmp_par = ezxml_attr(meter_node_ctxt, "meter_count");
	if (tmp_par != NULL) {
		meter_count = atoi(tmp_par);
		if (meter_count <= 0 || meter_count > MAX_METER_NUM) {
			goto exit_lbl;
		}
	}

	meter_cfg = (qos_meter_t *)os_calloc(1,(meter_count * sizeof(qos_meter_t)));
	if (meter_cfg == NULL) {
		LOGF_LOG_CRITICAL("Memory Allocation Failed\n");
		nRet = LTQ_FAIL;
		goto exit_lbl;
	}
	get_metercfg_from_xml(meter_cfg);

	for(i = 0; i < meter_count; i++)
	{
		snprintf(sName, sizeof(sName), "dos-mtr%d", (i + 1));
		meter_node_ctxt = ezxml_child(qos_ctxt, sName);
		if(meter_node_ctxt == NULL) {
			continue;
		}

		if (oper_flag == QOSAL_ENABLE)
			meter_cfg[i].flags |= PPA_QOS_OP_F_ADD;
		else if (oper_flag == QOSAL_DISABLE)
			meter_cfg[i].flags |= PPA_QOS_OP_F_DELETE;

		ppa_eng_meter_set(&meter_cfg[i]);
	}
	os_free(meter_cfg);

exit_lbl:
	EXIT
	return nRet;
}

/* =============================================================================
 *  Function Name : 
 *  Description   : 
 * ============================================================================*/
#if 0
static uint32_t get_prio_from_inggroup(uint32_t flags)
{
	uint32_t prio=0;

	if(flags & QOS_Q_F_INGGRP1)
	prio = 5; 
	else if(flags & QOS_Q_F_INGGRP2)
	prio = 4; 
	else if(flags & QOS_Q_F_INGGRP3)
	prio = 3; 
	else if(flags & QOS_Q_F_INGGRP4)
	prio = 2; 
	else if(flags & QOS_Q_F_INGGRP5)
	prio = 1; 
	else if(flags & QOS_Q_F_INGGRP6)
	prio = 0;

	return prio; 
}
#endif

static void get_wlan_index(char *ifname)
{
	if (strcmp(ifname, "wlan0"))
	{
       		wlan_index = 0;
        }
        else if (strcmp(ifname, "wlan2") || strcmp(ifname, "wlan1"))
        {
		wlan_index = 1;
	}
	else
        {
		wlan_index = 2;
        }
}

static void get_lan_index(char *ifname)
{
	uint8_t k = 0;
	for (k = 0; k < LAN_PORT_NUM; k++)
	{
		if (strcmp(ifname, lan_dev[k]) == 0) {
			lan_index = k;
		}
	}
}
/* =============================================================================
 *  Function Name : ppa_eng_port_add
 *  Description   : API to do port add operation.
 * ============================================================================*/
static int32_t ppa_eng_port_add (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_shaper_t  *shaper, uint32_t priority, uint32_t weight,uint32_t flags)
{
	PPA_CMD_RATE_INFO rate_info;
	int ret = LTQ_SUCCESS,k = 0;
	char *ifname_lan;
	char *substr = NULL;
	PPA_CMD_QOS_INGGRP_INFO g_info;
	memset_s(&rate_info, sizeof(PPA_CMD_RATE_INFO), 0x00);
	memset_s(&g_info, sizeof(PPA_CMD_QOS_INGGRP_INFO), 0x00);
	
	UNUSED(priority);
	UNUSED(weight);
	UNUSED(ifinggrp);
	strncpy_s(rate_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
	rate_info.queueid = -1;
	rate_info.shaperid = -1;
	rate_info.shaper.enable = shaper->enable;
	rate_info.shaper.mode = shaper->mode;

	port_cir = rate_info.shaper.cir = shaper->pir;
	port_cbs = rate_info.shaper.cbs = def_burst;
	port_pbs = rate_info.shaper.pbs = def_burst;
	LOGF_LOG_INFO( " Adding Port shaper for of [ %d ] kbps to [ %s ] \n", shaper->pir, ifname);
	if(flags & QOS_OP_F_INGRESS) {
		rate_info.flags |= PPA_QOS_OP_F_INGRESS;
		if (IFINGGRP_INVALID(ifinggrp) && xml_ifinggrp_en) {
			LOGF_LOG_DEBUG("Invalid ingress group, try to fetch from XML\n");
			ifinggrp = xml_lookup_ingress_group(ifname);
			if (IFINGGRP_INVALID(ifinggrp)) {
				LOGF_LOG_ERROR("Invalid XML ingress group\n");
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
		}
		LOGF_LOG_DEBUG("iface %s belongs to ingress group %d\n", ifname, ifinggrp);
		strncpy_s(g_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
		g_info.ingress_group = (PPA_QOS_INGGRP)ifinggrp;
		ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_INGGRP,&g_info);
		if( ret != LTQ_SUCCESS ) {
			LOGF_LOG_ERROR("Ioctl for set Ingress QoS Failed with [%d]\n", ret);
			ret = QOS_PPA_ENG_CFG_FAIL;
			goto Err_Hndl;
		}
		ing_port_shaper_rate[ifinggrp] = rate_info.shaper.pir = shaper->pir;
	} else {
		/* Fill Global variables for Port rate*/
		if (iftype == QOS_IF_CATEGORY_WLANDP)
		{
			get_wlan_index(ifname);
			port_shape_wlan[wlan_index] = rate_info.shaper.pir = shaper->pir;
		}
		else if (iftype == QOS_IF_CATEGORY_ETHLAN)
		{
			if ((strstr_s(lanbase_str, strnlen_s(lanbase_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK)) {
				port_rates[iftype] = rate_info.shaper.pir = shaper->pir;
			}
			else {
				get_lan_index(ifname);
				lan_port_rates[lan_index] = rate_info.shaper.pir = shaper->pir;
			}
		}
		else
			port_rates[iftype] = rate_info.shaper.pir = shaper->pir;
	}

	rate_info.shaper.flags = shaper->flags;
	rate_info.rate = shaper->pir;
	//rate_info.burst = shaper->pbs;
	rate_info.burst = def_burst;
	rate_info.flags = flags;

	if (strstr_s(lan_str, strnlen_s(lan_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) != EOK)
	{
		/* Port Add for WAN interface */
		if (flags & QOS_OP_F_MODIFY) {
			rate_info.flags |= PPA_QOS_OP_F_MODIFY;
			ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
			if (ret != LTQ_SUCCESS)
			{
				LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed with [%d]\n", ret);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
		} else {
			ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_SHAPER, &rate_info);
			if (ret != LTQ_SUCCESS)
			{
				LOGF_LOG_ERROR("Ioctl Set QoS Shaper Failed with [%d]\n", ret);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
			else
				ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
			if (ret != LTQ_SUCCESS)
			{
				LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed with [%d]\n", ret);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
		}
	}
	else
	{
		/* Port Add for Lan interface*/
		if (strstr_s(lansep_str, strnlen_s(lansep_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK)
		{
			if (flags & QOS_OP_F_MODIFY) {
				rate_info.flags |= PPA_QOS_OP_F_MODIFY;
				ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
				if (ret != LTQ_SUCCESS)
				{
					LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed For Lan port with [%d]\n", ret);
					ret = QOS_PPA_ENG_CFG_FAIL;
					goto Err_Hndl;
				}
			} else {
				ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_SHAPER, &rate_info);
				if (ret != LTQ_SUCCESS)
				{
					LOGF_LOG_ERROR("Ioctl Set QoS Shaper For Lan port Failed with [%d]\n", ret);
					ret = QOS_PPA_ENG_CFG_FAIL;
					goto Err_Hndl;
				}
				else
				ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
				if (ret != LTQ_SUCCESS)
				{
					LOGF_LOG_ERROR("Ioctl Set QoS Rate for Lan port Failed with [%d]\n", ret);
					ret = QOS_PPA_ENG_CFG_FAIL;
					goto Err_Hndl;
				}
			}
		}
		if (strstr_s(lanbase_str, strnlen_s(lanbase_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK)
		{
			for(k=0;k<4;k++)
			{
				ifname_lan = lan_dev[k];
				strncpy_s(rate_info.ifname, PPA_IF_NAME_SIZE, ifname_lan, PPA_IF_NAME_SIZE);
				if (flags & QOS_OP_F_MODIFY) {
					rate_info.flags |= PPA_QOS_OP_F_MODIFY;
					ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
					if (ret != LTQ_SUCCESS)
					{
						LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed For LAN Port with [%d]\n", ret);
						ret = QOS_PPA_ENG_CFG_FAIL;
						goto Err_Hndl;
					}
				} else {
					ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_SHAPER, &rate_info);
					if (ret != LTQ_SUCCESS)
					{
						LOGF_LOG_ERROR("Ioctl Set QoS Shaper For Lan port Failed with [%d]\n", ret);
						ret = QOS_PPA_ENG_CFG_FAIL;
						goto Err_Hndl;
					}
					else
					ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
					if (ret != LTQ_SUCCESS)
					{
						LOGF_LOG_ERROR("Ioctl Set QoS Rate for Lan port Failed with [%d]\n", ret);
						ret = QOS_PPA_ENG_CFG_FAIL;
						goto Err_Hndl;
					}
				}
			}
		}
	}

Err_Hndl:
	return ret;
}
#if 0
/* =============================================================================
 *  Function Name : ppa_eng_port_mod
 *  Description   : API to do port mod operation.
 * ============================================================================*/
static int32_t ppa_eng_port_mod (char *ifname, iftype_t iftype,qos_shaper_t  *shaper,uint32_t weight, uint32_t priority)
{
}
#endif
/* =============================================================================
 *  Function Name : ppa_eng_port_del
 *  Description   : API to do port delete  operation.
 * ============================================================================*/
static int32_t ppa_eng_port_del (char *ifname,char *orig_ifname, iftype_t iftype, qos_shaper_t  *shaper, uint32_t priority, uint32_t weight,uint32_t flag)
{
	int ret = LTQ_SUCCESS;
	int k = 0;
	uint32_t count = 0;
	char *ifname_lan;
	char *substr = NULL;
	PPA_CMD_RATE_INFO rate_info;
	qos_queue_cfg_t *q_ptr = NULL;
	qos_queue_cfg_t q_cfg[PPA_MAX_Q];
	memset_s(&rate_info, sizeof(PPA_CMD_RATE_INFO), 0x00);

	UNUSED(iftype);
	UNUSED(priority);
	UNUSED(weight);

	if (strstr_s(lan_str, strnlen_s(lan_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) != EOK)
	{
		q_ptr = q_cfg;
		qosd_queue_get(orig_ifname, NULL, &count, &q_ptr, flag);
	}
	rate_info.queueid = -1;
	rate_info.shaperid = -1;
	rate_info.shaper.enable = 1; // Always shaper is enabled
	rate_info.shaper.mode = shaper->mode;
	port_pir = 0;
	port_pir_lan = 0;
	port_cir = 0;
	port_cbs = 0;
	port_pbs = 0;
	rate_info.shaper.flags = shaper->flags;
	rate_info.rate = shaper->pir;
	//rate_info.burst = shaper->pbs;
	rate_info.burst = def_burst;
	strncpy_s(rate_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
	rate_info.flags = flag;
	if(flag & QOS_OP_F_INGRESS)
		rate_info.flags |= PPA_QOS_OP_F_INGRESS;
	else if (!(flag & QOS_NOT_ADD_DEF_SHAPER)) {
		/* Don't flush the shaper rate if default queue is deleted.
		 * Above flag is only passed when default queue is deleted.
		 */
		if (iftype == QOS_IF_CATEGORY_ETHLAN)
		{
			if ((strstr_s(lanbase_str, strnlen_s(lanbase_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK)) {
				/* For br-lan interface - update in iftype array */
				port_rates[iftype] = 0;
			}
			else {
				/* For individual lan-port - update in lan port array */
				get_lan_index(ifname);
				lan_port_rates[lan_index] = 0;
			}
		}
		else {
			/* Update in iftype array */
			port_rates[iftype] = 0;
		}
	}

	if (strstr_s(lan_str, strnlen_s(lan_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) != EOK)
	{
		/* Port del for Wan interface*/
		ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_RESET_QOS_RATE,&rate_info);
		if(ret != LTQ_SUCCESS)
		{
			LOGF_LOG_ERROR("Ioctl Set QoS Reset Rate Wan Failed with [%d] for [ %s ]\n", ret, rate_info.ifname);
			ret = QOS_PPA_ENG_CFG_FAIL;
			goto Err_Hndl;
		}
		if (flag & PPA_QOS_OP_F_DELETE && !(flag & QOS_OP_F_INGRESS) && !(flag & QOS_NOT_ADD_DEF_SHAPER)) {
			sysapi_iface_rate_get(ifname, iftype, &(rate_info.shaper.pir));
			rate_info.rate = rate_info.shaper.cir = rate_info.shaper.pir;
			rate_info.shaper.cbs = rate_info.shaper.pbs = def_burst;
			ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_SHAPER, &rate_info);
			if (ret != LTQ_SUCCESS)
			{
				LOGF_LOG_ERROR("Ioctl Set QoS Set Rate Wan Failed with [%d] for [ %s ]\n", ret, rate_info.ifname);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
			else
			ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
			if(ret != LTQ_SUCCESS)
			{
				LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed with [%d]\n", ret);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
		}
	}
	else
	{
		if (strstr_s(lansep_str, strnlen_s(lansep_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK)
		{
			ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_RESET_QOS_RATE,&rate_info);
			if(ret != LTQ_SUCCESS)
			{
				LOGF_LOG_ERROR("Ioctl Set QoS Reset Rate Lan Failed with [%d] for [ %s ] \n", ret, rate_info.ifname);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
			if (flag & PPA_QOS_OP_F_DELETE && !(flag & QOS_OP_F_INGRESS) && !(flag & QOS_NOT_ADD_DEF_SHAPER)) {
				sysapi_iface_rate_get(ifname, iftype, &(rate_info.shaper.pir));
				rate_info.rate = rate_info.shaper.cir = rate_info.shaper.pir;
				rate_info.shaper.cbs = rate_info.shaper.pbs = def_burst;
				ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_SHAPER, &rate_info);
				if (ret != LTQ_SUCCESS)
				{
					LOGF_LOG_ERROR("Ioctl Set QoS Set Rate Lan Failed with [%d] for [ %s ]\n", ret, rate_info.ifname);
					ret = QOS_PPA_ENG_CFG_FAIL;
					goto Err_Hndl;
				}
				else
				ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
				if(ret != LTQ_SUCCESS)
				{
					LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed with [%d]\n", ret);
					ret = QOS_PPA_ENG_CFG_FAIL;
					goto Err_Hndl;
				}
			}
		}
		if((strstr_s(lanbase_str, strnlen_s(lanbase_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK))
		{
			/* Port del for Lan interface*/
			for(k=0;k<4;k++)
			{
				ifname_lan = lan_dev[k];
				strncpy_s(rate_info.ifname, PPA_IF_NAME_SIZE, ifname_lan, PPA_IF_NAME_SIZE);
				ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_RESET_QOS_RATE,&rate_info);
				if(ret != LTQ_SUCCESS)
				{
					LOGF_LOG_ERROR("Ioctl Set QoS Reset Rate Lan Failed with [%d] for iface %s\n", ret, rate_info.ifname);
					ret = QOS_PPA_ENG_CFG_FAIL;
					goto Err_Hndl;
				}
				if (flag & PPA_QOS_OP_F_DELETE && !(flag & QOS_OP_F_INGRESS) && !(flag & QOS_NOT_ADD_DEF_SHAPER)) {
					sysapi_iface_rate_get(ifname_lan, iftype, &(rate_info.shaper.pir));
					rate_info.rate = rate_info.shaper.cir = rate_info.shaper.pir;
					rate_info.shaper.cbs = rate_info.shaper.pbs = def_burst;
					ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_SHAPER, &rate_info);
					if (ret != LTQ_SUCCESS)
					{
						LOGF_LOG_ERROR("Ioctl Set QoS Set Rate Lan Failed with [%d] for [ %s ]\n", ret, rate_info.ifname);
						ret = QOS_PPA_ENG_CFG_FAIL;
						goto Err_Hndl;
					}
					else
					ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE, &rate_info);
					if(ret != LTQ_SUCCESS)
					{
						LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed with [%d]\n", ret);
						ret = QOS_PPA_ENG_CFG_FAIL;
						goto Err_Hndl;
					}
				}
			}
		}
	}


Err_Hndl:
	return ret;
}


int32_t ppa_eng_port_set (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_shaper_t  *shaper, uint32_t priority, uint32_t weight, uint32_t flag)
{
	qos_queue_cfg_t *q_ptr = NULL;
	qos_queue_cfg_t q_cfg[PPA_MAX_Q];
	char *ifname2;
	uint32_t flags = 0, i = 0, count = 0;
	int ret = LTQ_SUCCESS;

	char base_if[MAX_IF_NAME_LEN], logical_if[MAX_IF_NAME_LEN];
	PPA_CMD_QOS_INGGRP_INFO g_info;

	UNUSED(i);
	UNUSED(priority);
	UNUSED(weight);
	UNUSED(ifinggrp);

	memset_s(base_if, sizeof(base_if), 0);
	memset_s(logical_if, sizeof(logical_if), 0);
	memset_s(&g_info, sizeof(PPA_CMD_QOS_INGGRP_INFO), 0x00);
	/* Reconfigure queues with shaped port rate*/
	q_ptr = q_cfg;
	qosd_queue_get(ifname, NULL, &count, &q_ptr, flag);
	ifname2 = ifname;
	LOGF_LOG_INFO("Port Shaper %s on [ %s ]\n",flag & QOS_OP_F_ADD?"ADD":"DELETE", ifname);

	strncpy_s(base_if, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
	if((iftype == QOS_IF_CATEGORY_ETHWAN) || (iftype == QOS_IF_CATEGORY_PTMWAN) || (iftype == QOS_IF_CATEGORY_ATMWAN))
	{
		ret = qosd_if_base_get(ifname,base_if,logical_if,0);
		ifname = base_if;
		if(ret != LTQ_SUCCESS)
			LOGF_LOG_ERROR("Failed to get base interface for given interface [%s]\n", ifname);
	}

	if(flag & QOS_OP_F_INGRESS) {
		flags |= PPA_QOS_OP_F_INGRESS;
		if (IFINGGRP_INVALID(ifinggrp) && xml_ifinggrp_en) {
			LOGF_LOG_DEBUG("Invalid ingress group, try to fetch from XML\n");
			ifinggrp = xml_lookup_ingress_group(ifname);
			if (IFINGGRP_INVALID(ifinggrp)) {
				LOGF_LOG_ERROR("Invalid XML ingress group\n");
				ret = QOS_PPA_ENG_CFG_FAIL;
				return ret;
			}
		}
		LOGF_LOG_DEBUG("iface %s belongs to ingress group %d\n", ifname, ifinggrp);
		strncpy_s(g_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
		g_info.ingress_group = (PPA_QOS_INGGRP)ifinggrp;
		ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_INGGRP,&g_info);
		if( ret != LTQ_SUCCESS ) {
			LOGF_LOG_ERROR("Ioctl for set Ingress QoS Failed with [%d]\n", ret);
			ret = QOS_PPA_ENG_CFG_FAIL;
			return ret;
		}
	}
	
	/* QOS_OP_F_ADD flag is used for both ADD operation as well as for MODIFY
	 * operation from CLI.
	 */
	if (flag & QOS_OP_F_ADD)
	{
		if (shaper->enable == QOSAL_DISABLE)
			flags |= PPA_QOS_OP_F_DELETE;
		else
			flags |= PPA_QOS_OP_F_ADD;
		ppa_eng_port_del (ifname, ifname2, iftype, shaper, priority, weight, flags);
		ppa_eng_port_add (ifname, iftype, ifinggrp, shaper, priority, weight, flags);
		if(flag & QOS_OP_F_INGRESS)
			ing_port_shaper[ifinggrp] = 1;
		else
		port_shape = 1;
		g_iftype = iftype;
	}
	else if (flag & QOS_OP_F_DELETE)
	{
		flags |= PPA_QOS_OP_F_DELETE;	
		ppa_eng_port_del (ifname, ifname2,iftype, shaper,priority,weight,flags);
		if(flag & QOS_OP_F_INGRESS)
			ing_port_shaper[ifinggrp] = 0;
		else
			port_shape = 0;
		g_iftype = iftype;
	}
	else if (flag & QOS_OP_F_MODIFY) {
		flags |= PPA_QOS_OP_F_MODIFY;
		LOGF_LOG_DEBUG("QoS Shaper Modify\n")
		ppa_eng_port_add (ifname, iftype, ifinggrp, shaper, priority, weight, flags);
	}
#if 0
	else if(flag & QOS_OP_F_MOD)
	{
		ppa_eng_port_mod (ifname, iftype, shaper, weight, priority);
		g_iftype = iftype;
	}
#endif
	else
	{
		if((port_shape == 1) && (g_iftype == iftype)) {
			/* already port is shaped by user. */
			goto dont_change_queues;
		}
		else {
		 	/* retrieve rate from system and set port rate. */
			sysapi_iface_rate_get(ifname,iftype, &(shaper->pir));
			shaper->cir = shaper->pir;
			shaper->pbs = def_burst;
			shaper->cbs = def_burst;
			shaper->enable = 1;
			shaper->mode = def_shaper_mode;
			ppa_eng_port_add(ifname, iftype, ifinggrp, shaper, priority, weight, flags);
		}
	}

dont_change_queues:
	return ret;
}

/* =============================================================================
 *  Function Name : add_qos_queue
 *  Description   : Callback API of PPA engine to do queue add operation.
 * ============================================================================*/
static int32_t add_qos_queue(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg,int32_t q_num)
{
	PPA_CMD_QOS_QUEUE_INFO q_info;
	PPA_CMD_QOS_INGGRP_INFO g_info;
	PPA_CMD_RATE_INFO rate_info;
	qos_shaper_t  port_shaper;
	PPA_CMD_SHAPER_INFO shaper_info;
	memset_s(&shaper_info, sizeof(PPA_CMD_SHAPER_INFO), 0x00);
	int ret = LTQ_SUCCESS,i,k=0;
	uint32_t priority=0,weight=0;
	char *substr = NULL;
	UNUSED(priority);
	UNUSED(weight);
	UNUSED(ifinggrp);
	UNUSED(iftype);

	memset_s(&q_info, sizeof(PPA_CMD_QOS_QUEUE_INFO), 0x00);
	memset_s(&g_info, sizeof(PPA_CMD_QOS_INGGRP_INFO), 0x00);
	memset_s(&rate_info, sizeof(PPA_CMD_RATE_INFO), 0x00);
	memset_s(&port_shaper, sizeof(qos_shaper_t), 0x00);

	strncpy_s(q_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
	q_info.queue_num = q_num;
	q_info.shaper_num = q_num;
	q_info.sched = qcfg->sched;
	q_info.weight = qcfg->weight;
	q_info.drop.enable = qcfg->drop.enable; 
	q_info.drop.mode = qcfg->drop.mode;
	switch(qcfg->drop.mode)
	{
		case QOS_DROP_TAIL:
			q_info.qlen = qcfg->qlen;
#ifdef PLATFORM_XRX500
			q_info.drop.wred.max_th0 = qcfg->drop.wred.max_th0;
			q_info.drop.wred.max_th1 = qcfg->drop.wred.max_th1;
			q_info.drop.wred.drop_th1 = qcfg->drop.wred.drop_th1;
			q_info.drop.flags |= qcfg->drop.flags;
#endif
			break;
		case QOS_DROP_RED:
			break;
		case QOS_DROP_WRED:
			q_info.drop.wred.weight = qcfg->drop.wred.weight;
			q_info.drop.wred.min_th0 = qcfg->drop.wred.min_th0;
			q_info.drop.wred.max_th0 = qcfg->drop.wred.max_th0;
			q_info.drop.wred.max_p0 = qcfg->drop.wred.max_p0;
			q_info.drop.wred.min_th1 = qcfg->drop.wred.min_th1;
			q_info.drop.wred.max_th1 = qcfg->drop.wred.max_th1;
			q_info.drop.wred.max_p1 = qcfg->drop.wred.max_p1;
			break;
		default:
			q_info.qlen = qcfg->qlen;
			break;
	}
	if(qcfg->flags & QOS_Q_F_INGRESS)
	{
		q_info.flags |= PPA_QOS_Q_F_INGRESS;
		if (IFINGGRP_INVALID(ifinggrp) && xml_ifinggrp_en) {
			LOGF_LOG_DEBUG("Invalid ingress group, try to fetch from XML\n");
			ifinggrp = xml_lookup_ingress_group(ifname);
			if (IFINGGRP_INVALID(ifinggrp)) {
				LOGF_LOG_ERROR("Invalid XML ingress group\n");
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
			LOGF_LOG_DEBUG("Using XML setting: iface %s belongs to ingress group %d\n", ifname, ifinggrp);
		}
		strncpy_s(g_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
		g_info.ingress_group = (PPA_QOS_INGGRP)ifinggrp;
		ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_INGGRP,&g_info);
		if( ret != LTQ_SUCCESS ) {
			LOGF_LOG_ERROR("Ioctl for set Ingress QoS Failed with [%d]\n", ret);
			ret = QOS_PPA_ENG_CFG_FAIL;
			goto Err_Hndl;
		}
	}
	if(qcfg->flags & QOS_Q_F_WFQ_UPDATE)
	{
		LOGF_LOG_DEBUG("QOS_Q_F_WFQ_UPDATE is set - flags:[%d]\n", qcfg->flags);
		q_info.flags |= PPA_QOS_Q_F_WFQ_UPDATE;
	}

	if(qcfg->flags & QOS_Q_F_DEFAULT)
		q_info.flags |= PPA_QOS_Q_F_DEFAULT;
	/* Set Flag with sepcial value for DL Interface & pass to TMU to set Queue Map */
	if(iftype == QOS_IF_CATEGORY_WLANNDP)
		q_info.flags |= PPA_QOS_Q_F_WLANDP;
	q_info.priority = (qcfg->priority);
	for(i=0;i<MAX_TC_NUM;i++) {
		if(qcfg->tc_map[i] <= DEFAULT_TC)
			q_info.tc_map[i] = (DEFAULT_TC - qcfg->tc_map[i]);
		else
			q_info.tc_map[i] = (MAX_TC_NUM + DEFAULT_TC - qcfg->tc_map[i]);
		LOGF_LOG_DEBUG("Adding Queue on %s with tc_map = %d \n",ifname, q_info.tc_map[i]);
		if(qcfg->tc_map[i] != 0) { /*Should actually check against -1 once the def. value set in tc_map[] by the framework is -1*/
			k++;
		}
	}
	q_info.tc_no = k;

	/* Shaper should always be enabled internally because it could either be Port rate or Queue rate */
	q_info.shaper.enable = 1;
	/* End of Filling Queue Info */

	ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_ADD_QOS_QUEUE,&q_info);
	if(ret != LTQ_SUCCESS)
	{
		LOGF_LOG_ERROR("Ioctl Add QoS Queue Failed with [%d]\n", ret);
		ret = QOS_PPA_ENG_CFG_FAIL;
		goto Err_Hndl;
	}
	/* Filling Shaper & Rate  Info */
	LOGF_LOG_INFO("Setting  Port Shaper for [%s] \n", ifname);
	if((qcfg->flags & QOS_Q_F_DEFAULT) && !(qcfg->flags & QOS_Q_F_INGRESS)) {
		if((port_shape == 1) && (g_iftype == iftype))
		{
            if (iftype == QOS_IF_CATEGORY_WLANDP)
			{
				get_wlan_index(ifname);
				port_pir = port_shape_wlan[wlan_index];
			}
			else if (iftype == QOS_IF_CATEGORY_ETHLAN)
			{
				port_pir = port_rates[iftype];
				if ((strstr_s(lanbase_str, strnlen_s(lansep_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK))
				{
						get_lan_index(ifname);
						if (lan_port_rates[lan_index])
							port_pir = lan_port_rates[lan_index];
				}
			}
			else
				port_pir = port_rates[iftype];
		}
		else
		{
			if (iftype == QOS_IF_CATEGORY_WLANDP)
            {
				get_wlan_index(ifname);
				if (port_shape_wlan[wlan_index] ==  0)
					sysapi_iface_rate_get(ifname,iftype, &port_pir);
				else
                 	port_pir = port_shape_wlan[wlan_index];
			}
			else if (iftype == QOS_IF_CATEGORY_ETHLAN)
			{
				if (port_rates[iftype] == 0)
					sysapi_iface_rate_get(ifname, iftype, &port_pir);
				else
					port_pir = port_rates[iftype];
				if ((strstr_s(lanbase_str, strnlen_s(lansep_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, PPA_IF_NAME_SIZE), &substr) == EOK))
				{
					get_lan_index(ifname);
					if (port_rates[iftype])
						port_pir = port_rates[iftype];
					else if (lan_port_rates[lan_index] ==  0)
						sysapi_iface_rate_get(ifname,iftype, &port_pir);
					else
						port_pir = lan_port_rates[lan_index];
				}
			}
			else
			{
				if (port_rates[iftype] == 0)
					sysapi_iface_rate_get(ifname,iftype, &port_pir);
				else
					port_pir = port_rates[iftype];
			}
		}
/* Need to create seperate sihaper instance assign pir value and pass to port_ad */
		port_shaper.pir = port_pir;
		port_shaper.cir = port_pir;
		port_shaper.pbs = def_burst;	
		port_shaper.cbs = def_burst;	
		port_shaper.enable = 1;	
		port_shaper.mode = def_shaper_mode;	
		ppa_eng_port_add (ifname, iftype, ifinggrp, &port_shaper, priority, weight, q_info.flags);
	} else if((qcfg->flags & QOS_Q_F_DEFAULT) && (qcfg->flags & QOS_Q_F_INGRESS)) {

		if (IFINGGRP_INVALID(ifinggrp) && xml_ifinggrp_en) {
			LOGF_LOG_DEBUG("Invalid ingress group, try to fetch from XML\n");
			ifinggrp = xml_lookup_ingress_group(ifname);
			if (IFINGGRP_INVALID(ifinggrp)) {
				LOGF_LOG_ERROR("Invalid XML ingress group\n");
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
			LOGF_LOG_DEBUG("Using XML setting: iface %s belongs to ingress group %d\n", ifname, ifinggrp);
		}
		if (ing_port_shaper_rate[ifinggrp] != 0) {
			port_shaper.pir = ing_port_shaper_rate[ifinggrp];
			port_shaper.cir = ing_port_shaper_rate[ifinggrp];
			port_shaper.pbs = def_burst;
			port_shaper.cbs = def_burst;
			port_shaper.enable = 1;
			port_shaper.mode = def_shaper_mode;
			ppa_eng_port_add (ifname, iftype, ifinggrp, &port_shaper, priority, weight, q_info.flags);
		}
	}

	if(ret == LTQ_SUCCESS)
	{
			strncpy_s(rate_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
	    	rate_info.shaperid = q_num;
	    	rate_info.queueid = q_num;
		if(qcfg->shaper.enable == 1)
		{
			if (qcfg->flags & QOS_Q_F_WFQ_UPDATE)
				rate_info.flags |= PPA_QOS_Q_F_WFQ_UPDATE;
			if (qcfg->flags & QOS_Q_F_INGRESS)
				rate_info.flags |= PPA_QOS_Q_F_INGRESS;
	    		rate_info.shaper.flags = qcfg->shaper.flags;
	    		rate_info.shaper.enable = qcfg->shaper.enable;
	    		rate_info.shaper.mode = qcfg->shaper.mode;
	    		rate_info.shaper.pir = qcfg->shaper.pir;
	    		rate_info.shaper.cir = qcfg->shaper.pir;
	    		//rate_info.shaper.cbs = qcfg->shaper.cbs;
	    		//rate_info.shaper.pbs = qcfg->shaper.pbs;
	    		rate_info.shaper.cbs = def_burst;
	    		rate_info.shaper.pbs = def_burst;
	    		rate_info.rate = qcfg->shaper.pir;
	    		//rate_info.burst = qcfg->shaper.pbs;
	    		rate_info.burst = def_burst;
			ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_SHAPER,&rate_info);
			if(ret != LTQ_SUCCESS)
			{
				LOGF_LOG_ERROR("Ioctl Set QoS Shaper Failed with [%d]\n", ret);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
			ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_RATE,&rate_info);
			if(ret != LTQ_SUCCESS)
			{	
				LOGF_LOG_ERROR("Ioctl Set QoS Rate Failed with [%d]\n", ret);
				ret = QOS_PPA_ENG_CFG_FAIL;
				goto Err_Hndl;
			}
		} 
	/* End of Filling Shaper & Rate  Info */
	}

Err_Hndl:
	return ret;
}

/* =============================================================================
 *  Function Name : ppa_eng_queue_add
 *  Description   : Callback API of PPA engine to do queue add operation.
 * ============================================================================*/
int32_t ppa_eng_queue_add(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg)
{

	int32_t ret = LTQ_SUCCESS, k=0;
	char base_if[MAX_IF_NAME_LEN], logical_if[MAX_IF_NAME_LEN];
	int32_t no_of_lans;
	char *substr = NULL;
	memset_s(base_if, sizeof(base_if), 0);
	memset_s(logical_if, sizeof(logical_if), 0);

	no_of_lans=sysapi_no_of_lan_ports();
	strncpy_s(base_if, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
	if((iftype == QOS_IF_CATEGORY_ETHWAN) || (iftype == QOS_IF_CATEGORY_PTMWAN) || (iftype == QOS_IF_CATEGORY_ATMWAN))
	{
		ret = qosd_if_base_get(ifname,base_if,logical_if,0);
		if(ret != LTQ_SUCCESS)
			LOGF_LOG_ERROR("Failed to get base interface for given interface [%s]\n", ifname);
	}
	else if((iftype == QOS_IF_CATEGORY_WLANDP) || (iftype == QOS_IF_CATEGORY_WLANNDP))
	{
		strncpy_s(base_if, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
		if(qcfg->priority <= 4)
		{
			ret = QOS_Q_WLAN_PRIO_INVALID;
			goto Err_Hndl;
		}
	}

	/* Filling Queue Info */
	/* Check if Input interface is Wan Interface*/
	if (strstr_s(lan_str, strnlen_s(lan_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, MAX_IF_NAME_LEN), &substr) != EOK)
	{
		ret = add_qos_queue(base_if,iftype,ifinggrp,qcfg,qcfg->queue_id);
	}
	else
	{
		/* Check if Input interface is Lan port seperated Interface*/
		if (strstr_s(lansep_str, strnlen_s(lansep_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, MAX_IF_NAME_LEN), &substr) == EOK)
		{
			ret = add_qos_queue(ifname,iftype,ifinggrp,qcfg,qcfg->queue_id);
		}
		/* Check if Input interface is br0 Interface*/
		if(strstr_s(lanbase_str, strnlen_s(lanbase_str, DEV_LIST_NAME_MAX_LEN), ifname, strnlen_s(ifname, MAX_IF_NAME_LEN), &substr) == EOK)
		{
			LOGF_LOG_DEBUG( "In PPA ADD QUEUE : processing given interface as LAN\n");
			if (qcfg->flags & QOS_Q_F_INGRESS) {
				/* For Ingress QoS, we create a single queue for bridge interface */
				ret = add_qos_queue(lan_dev[0],iftype,ifinggrp,qcfg,qcfg->queue_id);
			} else {
				for(k=0;k< no_of_lans;k++)
					ret = add_qos_queue(lan_dev[k],iftype,ifinggrp,qcfg,qcfg->queue_id);
			}
		}
		
	}

	tc_q_map_add(ifname, iftype, qcfg);

Err_Hndl:
	return ret;
}

/* =============================================================================
 *  Function Name : ppa_eng_queue_modify
 *  Description   : Callback API of PPA engine to do queue modify operation.
 * ============================================================================*/
int32_t ppa_eng_queue_modify(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg)
{
	int32_t ret = LTQ_SUCCESS;
	qos_queue_cfg_t q_cfg;
	qos_queue_cfg_t *q_ptr = &q_cfg;
	uint32_t flags = 0, count = 0;

	/* Filling Queue Info */
	if((qcfg->flags & QOS_Q_F_INGRESS) == QOS_Q_F_INGRESS)
	flags |= QOS_OP_F_INGRESS;
	ret = qosd_queue_get(ifname, qcfg->name, &count, &q_ptr, flags);
	if ( ret == LTQ_SUCCESS ) {
		ret=ppa_eng_queue_delete(ifname, iftype, ifinggrp, q_ptr);
		if(ret != LTQ_SUCCESS) {
			ret = QOS_PPA_ENG_CFG_FAIL;
			LOGF_LOG_ERROR("PPA Queue Modify Failed, No Queue Instance Found to Modify\n");
			goto Err_Hndl;
		}
	}
	ret = ppa_eng_queue_add(ifname,iftype,ifinggrp,qcfg);
	if(ret != LTQ_SUCCESS)
	{
		if (ret != QOS_Q_RATE_INVALID)
			ret = QOS_PPA_ENG_CFG_FAIL;
		LOGF_LOG_ERROR("PPA Queue Add Failed\n");
		goto Err_Hndl;
	}
#if 0
	if (strstr(lan_str, ifname) == NULL)
	{
		ret = modify_qos_queue(ifname,iftype,qcfg,qcfg->queue_id);
	}
	else
	{
		for(k=0;k<4;k++)
		{
		ret = modify_qos_queue(lan_dev[k],iftype,qcfg,qcfg->queue_id);
		}
		
	}
#endif
Err_Hndl:
	return ret;

}

/* =============================================================================
 *  Function Name : ppa_eng_get_queue_stats
 *  Description   : Callback API of PPA engine to get queue statistics.
 * ============================================================================*/
int32_t ppa_eng_get_queue_stats (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, char *qname, qos_queue_stats_t* qstats)
{
	PPA_CMD_QOS_MIB_INFO q_mib;
	int ret = LTQ_SUCCESS;
	qos_queue_cfg_t *q_ptr = NULL;
	qos_queue_cfg_t q_cfg = {0};
	char base_if[MAX_IF_NAME_LEN], logical_if[MAX_IF_NAME_LEN];
	uint32_t flags = 0, count = 0;
	UNUSED(ifinggrp);

	memset_s(&q_mib, sizeof(PPA_CMD_QOS_MIB_INFO), 0x00);
	memset_s(base_if, sizeof(base_if), 0);
	memset_s(logical_if, sizeof(logical_if), 0);
	q_ptr = &q_cfg;
	qosd_queue_get(ifname, qname, &count, &q_ptr, flags);

	if((iftype == QOS_IF_CATEGORY_ETHWAN) || (iftype == QOS_IF_CATEGORY_PTMWAN) || (iftype == QOS_IF_CATEGORY_ATMWAN))
	ret = qosd_if_base_get(ifname,base_if,logical_if,0);
	else if((iftype == QOS_IF_CATEGORY_WLANDP) || (iftype == QOS_IF_CATEGORY_WLANNDP))
	strncpy_s(base_if, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);

	q_mib.queue_num = q_cfg.queue_id;
	strncpy_s(q_mib.ifname, PPA_IF_NAME_SIZE, base_if, PPA_IF_NAME_SIZE);
	ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_GET_QOS_MIB,&q_mib);
//	printf("%s %d,rx:%llu, tx:%llu, ret is %d\n ",__func__,__LINE__,q_mib.mib.total_rx_pkt, q_mib.mib.total_tx_pkt,ret);
	qstats->rx_pkt = q_mib.mib.total_rx_pkt;
	qstats->flags |= QOS_Q_STAT_RX_PKT;
	qstats->tx_pkt = q_mib.mib.total_tx_pkt;
	qstats->flags |= QOS_Q_STAT_TX_PKT;

	return ret;
	
}
/* =============================================================================
 *  Function Name : delete_qos_queue
 *  Description   : Callback API of PPA engine to do queue delete operation.
 * ============================================================================*/
static int32_t delete_qos_queue(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg,int32_t q_num)
{
	PPA_CMD_QOS_QUEUE_INFO q_info;
	int ret = LTQ_SUCCESS;

        memset_s(&q_info, sizeof(PPA_CMD_QOS_QUEUE_INFO), 0x00);

	UNUSED(iftype);
	UNUSED(ifinggrp);

	q_info.queue_num = q_num;
	q_info.shaper_num = q_num;
	q_info.priority = qcfg->priority;
	strncpy_s(q_info.ifname, PPA_IF_NAME_SIZE, ifname, PPA_IF_NAME_SIZE);
	q_info.drop.enable = qcfg->drop.enable; 
	q_info.drop.mode = qcfg->drop.mode;
	switch(qcfg->drop.mode)
	{
		case QOS_DROP_TAIL:
			q_info.qlen = qcfg->qlen;
#ifdef PLATFORM_XRX500
			q_info.drop.wred.max_th0 = qcfg->drop.wred.max_th0;
			q_info.drop.wred.max_th1 = qcfg->drop.wred.max_th1;
			q_info.drop.wred.drop_th1 = qcfg->drop.wred.drop_th1;
			q_info.drop.flags |= qcfg->drop.flags;
#endif
			break;
		case QOS_DROP_RED:
			break;
		case QOS_DROP_WRED:
			q_info.drop.wred.weight = qcfg->drop.wred.weight;
			q_info.drop.wred.min_th0 = qcfg->drop.wred.min_th0;
			q_info.drop.wred.max_th0 = qcfg->drop.wred.max_th0;
			q_info.drop.wred.max_p0 = qcfg->drop.wred.max_p0;
			q_info.drop.wred.min_th1 = qcfg->drop.wred.min_th1;
			q_info.drop.wred.max_th1 = qcfg->drop.wred.max_th1;
			q_info.drop.wred.max_p1 = qcfg->drop.wred.max_p1;
			break;
		default:
			q_info.qlen = qcfg->qlen;
			break;
	}
	if(qcfg->flags & QOS_Q_F_WFQ_UPDATE)
	{
		LOGF_LOG_DEBUG("QOS_Q_F_WFQ_UPDATE flag is set - flags:[%d]\n", qcfg->flags);
		q_info.flags |= PPA_QOS_Q_F_WFQ_UPDATE;
	}
	if(qcfg->flags & QOS_Q_F_DEFAULT)
		q_info.flags |= PPA_QOS_Q_F_DEFAULT;
	if(qcfg->flags & QOS_Q_F_INGRESS)
		q_info.flags |= PPA_QOS_Q_F_INGRESS;
	if(iftype == QOS_IF_CATEGORY_WLANNDP)
		q_info.flags |= PPA_QOS_Q_F_WLANDP;
	ret = qos_call_ioctl(PPA_DEVICE, PPA_CMD_DEL_QOS_QUEUE,&q_info);

	if( ret != LTQ_SUCCESS ) {
		LOGF_LOG_ERROR("Ioctl Del QoS Queue Failed with [%d] \n", ret);
		ret = QOS_PPA_ENG_CFG_FAIL;
	}

	return ret;

}

/* =============================================================================
 *  Function Name : ppa_eng_queue_delete
 *  Description   : Callback API of PPA engine to do queue delete operation.
 * ============================================================================*/
int32_t ppa_eng_queue_delete(char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t *qcfg)
{
	uint32_t priority=0,weight=0,flags=0;
	int32_t ret = LTQ_SUCCESS, k=0;
	qos_shaper_t  port_shaper;
	char base_if[MAX_IF_NAME_LEN], logical_if[MAX_IF_NAME_LEN];
	int32_t no_of_lans;
	char *substr = NULL;
	ENTRY

	memset_s(base_if, sizeof(base_if), 0);
	memset_s(logical_if, sizeof(logical_if), 0);
    memset_s(&port_shaper, sizeof(qos_shaper_t), 0x00);

	no_of_lans = sysapi_no_of_lan_ports();	
	strncpy_s(base_if, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
	if((iftype == QOS_IF_CATEGORY_ETHWAN) || (iftype == QOS_IF_CATEGORY_PTMWAN) || (iftype == QOS_IF_CATEGORY_ATMWAN))
	{
		ret = qosd_if_base_get(ifname,base_if,logical_if,0);
		if(ret != LTQ_SUCCESS)
			LOGF_LOG_ERROR("Failed to get base interface for [%s]\n", ifname);
	}
	else if((iftype == QOS_IF_CATEGORY_WLANDP) || (iftype == QOS_IF_CATEGORY_WLANNDP)) {
		strncpy_s(base_if, MAX_IF_NAME_LEN, ifname, MAX_IF_NAME_LEN);
	}

	if(iftype == QOS_IF_CATEGORY_ETHLAN && (qcfg->flags & QOS_Q_F_LINK_STATE_CHANGE)) {
		/* check if all LAN ports are down */
		if (strncmp(sDefLanIf, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) == 0) {
			LOGF_LOG_DEBUG("Queue deletion failed. Some LAN ports are UP and queue deletion on individual LAN port is not supported.");
			return QOS_CFG_DENY_FROM_LINK_CHK;
		}
	}

	LOGF_LOG_DEBUG("Deleting queue from [  %s: %s ] \n",ifname,base_if);
	
	/* Filling Queue Info */
	/* Check if Input interface is Wan Interface*/
	if (strstr_s(lan_str, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN), &substr) != EOK)
	{
		ret = delete_qos_queue(base_if,iftype,ifinggrp,qcfg,qcfg->queue_id);
	}
	else
	{
		/* Check if Input interface is Lan port seperated Interface*/
		if (strstr_s(lansep_str, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN), &substr) == EOK)
		{
			ret = delete_qos_queue(ifname,iftype,ifinggrp,qcfg,qcfg->queue_id);
		}
		/* Check if Input interface is br0 Interface & loop for all lan seperated Interfaces*/
		if (strstr_s(lanbase_str, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN), &substr) == EOK)
		{
			if (qcfg->flags & QOS_Q_F_INGRESS) {
				ret = delete_qos_queue(lan_dev[0],iftype,ifinggrp,qcfg,qcfg->queue_id);
			}
			else
			{
				for(k=0;k< no_of_lans;k++)
				{
					ret = delete_qos_queue(lan_dev[k],iftype,ifinggrp,qcfg,qcfg->queue_id);
				}
			}
		}
		
	}
	/* Delete Port rate shaper once Default Queue is deleted*/
	port_shaper.pbs = def_burst;	
	port_shaper.cbs = def_burst;	
	port_shaper.enable = 1;	
	port_shaper.mode = def_shaper_mode;	
	port_shaper.pir = 0;	
	port_shaper.cir = 0;
	if(qcfg->flags & QOS_Q_F_WFQ_UPDATE)
		flags |= PPA_QOS_Q_F_WFQ_UPDATE;
	if(qcfg->flags & QOS_Q_F_INGRESS)
		flags |= PPA_QOS_Q_F_INGRESS;
	if((qcfg->flags & QOS_Q_F_DEFAULT) && !(qcfg->flags & QOS_Q_F_INGRESS)) {
		flags |= PPA_QOS_OP_F_DELETE | QOS_NOT_ADD_DEF_SHAPER;
		ret = ppa_eng_port_del (base_if,ifname, iftype,&port_shaper,priority,weight,flags);
		if( ret != LTQ_SUCCESS ) {
			ret = QOS_PPA_ENG_CFG_FAIL;
			LOGF_LOG_ERROR("PPA Port Shaper deletion Failed for [ %s:%s ]\n", base_if,ifname);
		}
	}

	tc_q_map_delete(ifname, iftype, qcfg);
	EXIT
	return ret;
}

/* =============================================================================
 *  Function Name : ppa_eng_meter_set
 *  Description   : Callback API of PPA engine to do meter set operation.
 * ============================================================================*/
int32_t ppa_eng_meter_set(qos_meter_t *meter_cfg) {
	ENTRY
	int32_t nRet = LTQ_SUCCESS;
	PPA_QOS_METER_INFO mtr_info;

	memset_s(&mtr_info, sizeof(PPA_QOS_METER_INFO), 0x00);

	mtr_info.type = meter_cfg->mode;
	mtr_info.enable = meter_cfg->enable;
	mtr_info.cir = meter_cfg->cir;
	mtr_info.pir = meter_cfg->pir;
	if (meter_cfg->cbs == 0)
		mtr_info.cbs = (meter_cfg->cir * 8) / 10;
	else
		mtr_info.cbs = meter_cfg->cbs;
	if (meter_cfg->pbs == 0)
		mtr_info.pbs = (meter_cfg->pir * 8) / 10;
	else
		mtr_info.pbs = meter_cfg->pbs;
	mtr_info.flags = meter_cfg->flags;
	mtr_info.meterid = meter_cfg->meterid;
	mtr_info.flags = meter_cfg->flags;

	nRet = qos_call_ioctl(PPA_DEVICE, PPA_CMD_SET_QOS_METER, &mtr_info);
	if ( nRet != LTQ_SUCCESS ) {
		LOGF_LOG_ERROR("Ioctl for add QoS Meter failed with [%d]\n", nRet);
		nRet = QOS_PPA_ENG_CFG_FAIL;
	}

	EXIT
	return nRet;
}
