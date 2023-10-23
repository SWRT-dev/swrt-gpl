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
#include "cpu_n_eng.h"
#include "sysapi.h"
#include <sys/ioctl.h>
#include <net/ppa_api.h>

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

#define PPA_DEVICE   "/dev/ifx_ppa"
cl_adapt_t cl_adapt;
char * IPT;

uint32_t giLeafCount = 0;
int32_t wfq_prio = CPU_N_MAX_Q;
char *qdiscs[2] = {"ifb0", "ifb1"};

static char *sDefLanIf = "br-lan";

static uint32_t get_qdisc_index(iftype_t iftype);
static int32_t set_pkts_redirection(iftype_t iftype, char *from, char *to, uint32_t enable);
static int32_t add_qdisc(char* pcQDiscDev, uint32_t uiPortRate, int32_t major, qdisc_tree_t* qdisc_tree);
static int32_t sw_queue_add_sp (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiSP, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[]);
static int32_t sw_queue_add_wfq (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiWFQ, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[]);
static int32_t sw_queue_add_sp_wfq (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiSP1, uint32_t uiWFQ, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[]);
static int32_t sw_queue_add_sp_wfq_sp (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiSP1, uint32_t uiWFQ, uint32_t uiSP2, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[]);
static int8_t sw_queue_show(void) __attribute__((unused));
static int32_t sw_queue_configure (char* pcQDiscDev, iftype_t iftype, q_cfg_info_t *if_q_config);
static int32_t cpu_n_eng_port_del (char *ifname, iftype_t iftype, qos_shaper_t  *shaper);
static int32_t cpu_n_eng_port_add (char *ifname, iftype_t iftype, qos_shaper_t  *shaper);
static int32_t cpu_n_eng_wan_init(void);
static int32_t cpu_basicIfRulesCfg(int32_t key, struct blob_buf *b, uint32_t *rule_idx);
static int32_t del_def_wmm_dscp_class(void);
static int32_t add_def_wmm_dscp_class(void);

static int32_t del_def_wmm_dscp_class(void)
{
	ENTRY
#ifdef PLATFORM_XRX500
	return qos_call_ioctl(PPA_DEVICE, PPA_CMD_QOS_DSCP_CLASS_RESET, NULL);
#else
	return LTQ_SUCCESS;
#endif
}
static int32_t add_def_wmm_dscp_class(void)
{
	ENTRY
#ifdef PLATFORM_XRX500
	return qos_call_ioctl(PPA_DEVICE, PPA_CMD_QOS_DSCP_CLASS_SET, NULL);
#else
	return LTQ_SUCCESS;
#endif
}

uint32_t is_wan_interface(iftype_t iftype)
{
	if (iftype == QOS_IF_CATEGORY_ETHWAN ||
			iftype == QOS_IF_CATEGORY_ATMWAN ||
			iftype == QOS_IF_CATEGORY_PTMWAN ||
			iftype == QOS_IF_CATEGORY_LTEWAN)
		return QOSAL_TRUE;
	else
		return QOSAL_FALSE;
}

static uint32_t get_qdisc_index(iftype_t iftype)
{
	if (iftype == QOS_IF_CATEGORY_ETHLAN )
		return 1;
	else
		return 0;
}

static int32_t set_pkts_redirection(iftype_t iftype, char *from, char *to, uint32_t enable)
{
	char base_if[MAX_IF_NAME_LEN] = {0}, logical_if[MAX_IF_NAME_LEN] = {0};

	if (is_wan_interface(iftype) == QOSAL_TRUE)
		qosd_if_base_get(from,base_if,logical_if,0);
    else
        strncpy_s(base_if, MAX_IF_NAME_LEN, from, MAX_IF_NAME_LEN);

	LOGF_LOG_DEBUG("\nsetting redirect from %s (for logical interface %s) to %s\n",base_if, from, to);

#if defined(PLATFORM_XRX330)
	if (strncmp(base_if, "br-lan", MAX_IF_NAME_LEN) == 0)
		strncpy_s(base_if, MAX_IF_NAME_LEN, "eth0", MAX_IF_NAME_LEN);
#endif

	sysapi_set_pkts_redirection(base_if, to, enable);

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : getQueueMode
 *  Description   : Identify scheduling algorithm as SP/WFQ for given queues.
 * ============================================================================*/
static QueueMode getQueueMode(uint32_t *puiSP1, uint32_t *puiWFQ, uint32_t *puiSP2, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[])
{
	uint32_t i = 0, j = 0, k = 0, l = 0;
	QueueMode eMode = QUEUEING_MODE_NONE;

	ENTRY
	LOGF_LOG_DEBUG ("Number of queues is %d\n",if_q_config->uiNoOfQueues);

	wfq_prio = CPU_N_MAX_Q;
	/* Get the priority of least prio wfq, later all wfqs will be configured at this prio level */
	for (i=0; i<if_q_config->uiNoOfQueues; i++){
		if (if_q_config->q_cfg[i].sched == QOS_SCHED_WFQ 
				&& if_q_config->q_cfg[i].priority < wfq_prio)
			wfq_prio = if_q_config->q_cfg[i].priority;
	}

	//need to check for other scheduling mode eg WRR
	for (i=0; i<if_q_config->uiNoOfQueues; i++){
		if (if_q_config->q_cfg[i].sched == QOS_SCHED_SP){
			if (if_q_config->q_cfg[i].priority <= wfq_prio)
				(*puiSP1)++;
			else
				(*puiSP2)++;
		}
		else if (if_q_config->q_cfg[i].sched == QOS_SCHED_WFQ){
			/* Set Prio of All WFQs to prio of least prio wfq  */
			//if_q_config->q_cfg[i].priority = wfq_prio;
			(*puiWFQ)++;
		}
	}

	/* Arrange queues in order -> SP1-WFQ-SP2*/
	for (i=0; i<if_q_config->uiNoOfQueues; i++){
		if (if_q_config->q_cfg[i].sched == QOS_SCHED_SP) {
			if (if_q_config->q_cfg[i].priority <= wfq_prio)
				q_list[j++] = &if_q_config->q_cfg[i];
			else
				q_list[*puiSP1 + *puiWFQ + k++] = &if_q_config->q_cfg[i];
		}
		else if (if_q_config->q_cfg[i].sched == QOS_SCHED_WFQ) {
			q_list[*puiSP1 + l++] = &if_q_config->q_cfg[i];
		}
	}
	
	if (*puiSP1 != 0 && *puiWFQ != 0 && *puiSP2 != 0)
		eMode = QUEUEING_MODE_SP_WFQ_SP;
	
	else if (*puiSP1 != 0 && *puiWFQ != 0 && *puiSP2 == 0)
		eMode = QUEUEING_MODE_SP_WFQ;

	else if (*puiSP1 != 0 && *puiWFQ == 0 && *puiSP2 == 0)
		eMode = QUEUEING_MODE_SP;

	else if (*puiSP1 == 0 && *puiWFQ != 0 && *puiSP2 == 0)
		eMode = QUEUEING_MODE_WFQ;

	LOGF_LOG_DEBUG("queue mode:%d, sp1:%d, wfq:%d, sp2:%d\n",eMode, *puiSP1,*puiWFQ,*puiSP2);
	EXIT
	return eMode;
}

/* =============================================================================
 *  Function Name : add_qdisc
 *  Description   : Add a queuing descipine with given details.
		    Creates single hierarchy scheduler.
 * ============================================================================*/
static int32_t add_qdisc(char* pcQDiscDev, uint32_t uiPortRate, int32_t major, qdisc_tree_t* qdisc_tree)
{
	int32_t localPrio;
	int32_t clMinor = 0, qdMinor=  0;
	char qdisc[100] = {0};
	uint32_t limit = 100;//Limit is qlen
	uint32_t i = 0, j = 0, qmin_th = 0, qmax_th = 0, qburst = 0;
	int32_t tc = 0;
	float prob = 0.0;
	char cmd_buf[MAX_DATA_LEN];
	uint32_t uiCeilRate = uiPortRate;
	uint32_t uiCommitRate = 1;
	uint32_t cir = uiPortRate;
	uint32_t burst = 1600;
	uint32_t cburst = 1600;

	ENTRY

	memset_s(cmd_buf, sizeof(cmd_buf), 0);

	/* Apply port shaping if it's configured*/
	LOGF_LOG_DEBUG("major:%d, Shaper pointer is %p\n",major, qdisc_tree->port_shaper);
	if (major == 1 && qdisc_tree->port_shaper != NULL) {
		uiPortRate = qdisc_tree->port_shaper->pir;
		LOGF_LOG_DEBUG("port is shaped at rate %d\n",qdisc_tree->port_shaper->pir);
		cir = qdisc_tree->port_shaper->cir ? qdisc_tree->port_shaper->cir : uiPortRate;
		burst = qdisc_tree->port_shaper->pbs ? qdisc_tree->port_shaper->pbs : burst;
		cburst = qdisc_tree->port_shaper->cbs ? qdisc_tree->port_shaper->cbs : cburst;
		uiCeilRate = uiPortRate;
	}

	snprintf(cmd_buf, sizeof(cmd_buf), "tc class add dev %s parent %d: classid %d:%d htb rate %dkbit ceil %dkbit burst %db cburst %db ;\n\n",
										pcQDiscDev, major, major, major, cir, uiPortRate, burst, cburst);
	RUN_CMD(cmd_buf)

	for(i=0; i<qdisc_tree->num_branches; i++) {
		uiCeilRate = uiPortRate;

		if (major < 10){
			clMinor = 10 * major * (i+1);
			qdMinor = clMinor;
		}
		else{
			clMinor = giLeafCount+1;
			qdMinor = 10 * major + clMinor;
		}

		memset_s(qdisc, sizeof(qdisc), 0);
		if (qdisc_tree->q_cfg[i] != NULL) {
			if (qdisc_tree->q_cfg[i]->qlen)
				limit =  qdisc_tree->q_cfg[i]->qlen;
			if (qdisc_tree->q_cfg[i]->drop.mode == QOS_DROP_RED){
				qmin_th = (qdisc_tree->q_cfg[i]->drop.wred.min_th0 * limit ) / 100;
				qmax_th = (qdisc_tree->q_cfg[i]->drop.wred.max_th0 * limit ) / 100;
				if (qdisc_tree->q_cfg[i]->drop.wred.max_p0)
					prob = (float) qdisc_tree->q_cfg[i]->drop.wred.max_p0 / 100.0 ;
				else prob = DEFAULT_RED_PROBABILITY;
				CALCULATE_THRESOLD_RED(qmin_th, limit, qmax_th, qburst);
				snprintf (qdisc, sizeof(qdisc), "red limit %ub min %u max %u avpkt %u burst %u probability %f",
												limit, qmin_th, qmax_th, QQ_AVPKT, qburst, prob);
			}
			else {
				snprintf (qdisc, sizeof(qdisc), "bfifo limit %ub", limit);
			}
			giLeafCount++; //Revisit, if multiple htb at 2nd level
		}
		else{
			memcpy_s(qdisc, sizeof(qdisc), "htb", 3);
		}

		if (qdisc_tree->q_cfg[i] != NULL ) {
			/* Engine Selector has prio 1-8 (high->low), while software has 0-7 */
			localPrio = qdisc_tree->q_cfg[i]->priority - 1;
			if (qdisc_tree->q_cfg[i]->sched == QOS_SCHED_SP){
				uiCommitRate = 1;
			}
			else if (qdisc_tree->q_cfg[i]->sched == QOS_SCHED_WFQ){
				uiCommitRate = (qdisc_tree->q_cfg[i]->weight * (uiPortRate))/100 ;
				if (uiCommitRate == 0)
					uiCommitRate = 1;
				localPrio = wfq_prio - 1;
			}
			/* If Shaper is configured, update commit/ceil rate with queue shaper values */
			if ( qdisc_tree->q_cfg[i]->shaper.mode != QOS_SHAPER_NONE) {
				if (qdisc_tree->q_cfg[i]->shaper.cir != 0)
					uiCommitRate = qdisc_tree->q_cfg[i]->shaper.cir;
				if (qdisc_tree->q_cfg[i]->shaper.pir != 0)
					uiCeilRate =  qdisc_tree->q_cfg[i]->shaper.pir;
			}
		}
		else {
			localPrio = qdisc_tree->prio[i];
			uiCommitRate = 1;
		}

		/* Commit rate should not be greater than peak rate */
		if (uiCommitRate > uiCeilRate)
			uiCommitRate = uiCeilRate;

		snprintf(cmd_buf, sizeof(cmd_buf), "tc class add dev %s parent %d:%d classid %d:%d htb rate %dkbit ceil %dkbit prio %d;\n", 
						pcQDiscDev, major, major, major, clMinor, uiCommitRate, uiCeilRate, localPrio);
		RUN_CMD(cmd_buf);

		snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s parent %d:%d handle %d: %s;\n", pcQDiscDev, major, clMinor, qdMinor, qdisc);
		RUN_CMD(cmd_buf);

		/* No filter rule for least prio as it is default queue */
		//if (qdisc_tree->q_cfg[i] && (localPrio != CPU_N_MAX_Q)){
		/* Using queue name to identify to default queue, it has to be replaced with some other mechanism */
		/* if (qdisc_tree->q_cfg[i] && (strcmp(qdisc_tree->q_cfg[i]->name, "def_queue") != 0)){ */
		if (qdisc_tree->q_cfg[i] && (isQueueDefault(qdisc_tree->q_cfg[i]) == QOSAL_FALSE)){
			/* We are using queue TC as a criteria to select packets for a class */
			j = 0;
			while ( j < MAX_TC_NUM && (qdisc_tree->q_cfg[i]->tc_map[j])) {
				if (qdisc_tree->q_cfg[i]->tc_map[j] <= DEFAULT_TC)
					tc = (DEFAULT_TC + 1 - qdisc_tree->q_cfg[i]->tc_map[j]) << TC_START_BIT_POS;
				else
					tc = (MAX_TC_NUM + DEFAULT_TC + 1 - qdisc_tree->q_cfg[i]->tc_map[j]) << TC_START_BIT_POS;
				//revisit for 3 levels
				if (major > 10 && (major%10) > 0){
					snprintf(cmd_buf, sizeof(cmd_buf), "tc filter add dev %s parent %d: protocol all u32 match %s 0x%x 0x%x flowid %d:%d;\n",
								pcQDiscDev, major/10, mark, tc, TC_MASK, major/10, major%10);
					RUN_CMD(cmd_buf);
				}

				if (major > 10){
					snprintf(cmd_buf, sizeof(cmd_buf), "tc filter add dev %s parent 1: protocol all u32 match %s 0x%x 0x%x flowid 1:%d;\n",
								pcQDiscDev, mark, tc, TC_MASK, major/10);
					RUN_CMD(cmd_buf);
				}

				snprintf(cmd_buf, sizeof(cmd_buf), "tc filter add dev %s parent %d: protocol all u32 match %s 0x%x 0x%x flowid %d:%d;\n\n",
							pcQDiscDev, major, mark, tc, TC_MASK, major, clMinor);
				RUN_CMD(cmd_buf);
				j++;
			}
		}
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : sw_queue_add_sp
 *  Description   : Add specified number of queues (uiSP) with scheduler algorithm
		    as strict priority on specified device (pcQDiscDev).
 * ============================================================================*/
static int32_t sw_queue_add_sp (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiSP, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[])
{
	qdisc_tree_t qdisc_tree;
	uint32_t uiNumOfBranches = 0;
	int32_t major = 0;
	uint32_t i = 0;
	char cDefault[20] = {0};
	char cmd_buf[MAX_DATA_LEN];

	ENTRY

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	memset_s(&qdisc_tree, sizeof(qdisc_tree_t), 0);

	giLeafCount = 0;

 //Level 1
	qdisc_tree.num_branches = uiSP;
	qdisc_tree.port_shaper = if_q_config->port_shaper;
	for (i=0; i<qdisc_tree.num_branches; i++){
		qdisc_tree.q_cfg[i] = q_list[i];
		qdisc_tree.prio[i] = 0;
		if (isQueueDefault(qdisc_tree.q_cfg[i]) == QOSAL_TRUE)
			snprintf(cDefault, sizeof(cDefault), "default %d",(i+1)*10);
	}

	major = 1;
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s root handle 1: htb %s;\n", pcQDiscDev,cDefault);
	RUN_CMD(cmd_buf);

	LOGF_LOG_DEBUG("Queue count: %d, uiPortRate:%d\n",uiNumOfBranches, uiPortRate);
	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : sw_queue_add_wfq
 *  Description   : Add specified number of queues (uiSP) with scheduler algorithm
		    as weighted fair queuing on specified device (pcQDiscDev).
 * ============================================================================*/
static int32_t sw_queue_add_wfq (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiWFQ, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[])
{
	qdisc_tree_t qdisc_tree;
	int32_t major = 0;
	uint32_t i = 0;
	char cDefault[20] = {0};
	char cmd_buf[MAX_DATA_LEN];

	ENTRY

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	memset_s(&qdisc_tree, sizeof(qdisc_tree_t), 0);
	giLeafCount = 0;

 //Level 1
	qdisc_tree.num_branches = uiWFQ;
	qdisc_tree.port_shaper = if_q_config->port_shaper;
	for (i=0; i<qdisc_tree.num_branches; i++){
		qdisc_tree.q_cfg[i] = q_list[i];
		qdisc_tree.prio[i] = 0;
		if (isQueueDefault(qdisc_tree.q_cfg[i]) == QOSAL_TRUE)
			snprintf(cDefault, sizeof(cDefault), "default %d",(i+1)*10);
	}

	major = 1;
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s root handle 1: htb %s;\n", pcQDiscDev,cDefault);
	RUN_CMD(cmd_buf);

	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : sw_queue_add_sp_wfq
 *  Description   : Add specified number of queues (uiSP1 and uiWFQ) for each
		    scheduler algorithm on specified device (pcQDiscDev).
 * ============================================================================*/
static int32_t sw_queue_add_sp_wfq (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiSP1, uint32_t uiWFQ, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[])
{
	qdisc_tree_t qdisc_tree;
	int32_t major = 0;
	uint32_t i = 0;
	char cDefault[20] = {0};
	char cmd_buf[MAX_DATA_LEN];

	ENTRY

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	memset_s(&qdisc_tree, sizeof(qdisc_tree_t), 0);

	giLeafCount = 0;


 //Level 1
	qdisc_tree.num_branches = uiSP1;
	qdisc_tree.port_shaper = if_q_config->port_shaper;
	for (i=0; i<qdisc_tree.num_branches; i++){
		qdisc_tree.q_cfg[i] = q_list[i];
		qdisc_tree.prio[i] = 0;
		if (isQueueDefault(qdisc_tree.q_cfg[i]) == QOSAL_TRUE)
			snprintf(cDefault, sizeof(cDefault), "default %d",(i+1)*10);
	}
	/* Add one extra node to attach WFQ tree at prio = uiSP1 */
	qdisc_tree.num_branches += 1;
	qdisc_tree.prio[i] = uiSP1;

	if (strnlen_s(cDefault, sizeof(cDefault)) == 0) {
		snprintf(cDefault, sizeof(cDefault), "default %d",(i+1)*10);
	}

	major = 1;
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s root handle 1: htb %s;\n", pcQDiscDev,cDefault);
	RUN_CMD(cmd_buf);
	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);
	
 //Level 2
 	memset_s(qdisc_tree.q_cfg, sizeof(qdisc_tree.q_cfg), 0);
	memset_s(cDefault, sizeof(cDefault), 0);
	qdisc_tree.num_branches = uiWFQ;
	for (i=0; i<qdisc_tree.num_branches; i++){
		qdisc_tree.q_cfg[i] = q_list[uiSP1+i];
		qdisc_tree.prio[i] = 0;
		if (isQueueDefault(qdisc_tree.q_cfg[i]) == QOSAL_TRUE)
			snprintf(cDefault, sizeof(cDefault), "default %d", giLeafCount+i+1);
	}
	major = 10 * (uiSP1 +1);

	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s parent 1:%d handle %d: htb %s;\n", pcQDiscDev, major, major, cDefault);
	RUN_CMD(cmd_buf);

	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);
	
	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : sw_queue_add_sp_wfq_sp
 *  Description   : Add specified number of queues (uiSP1, uiWFQ and uiSP2) for each
		    scheduler algorithm on specified device (pcQDiscDev).
 * ============================================================================*/
int32_t sw_queue_add_sp_wfq_sp (char* pcQDiscDev, uint32_t uiPortRate, uint32_t uiSP1, uint32_t uiWFQ, uint32_t uiSP2, q_cfg_info_t *if_q_config, qos_queue_cfg_t *q_list[])
{
	qdisc_tree_t qdisc_tree;
	int32_t major = 0;
	uint32_t i = 0, j = 0;
	char cmd_buf[MAX_DATA_LEN];

	ENTRY

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	memset_s(&qdisc_tree, sizeof(qdisc_tree_t), 0);

	giLeafCount = 0;
	
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s root handle 1: htb default 20;\n", pcQDiscDev);
	printf("dev is %s, ptr :%p\n",pcQDiscDev,pcQDiscDev);
	RUN_CMD(cmd_buf)

 //Level 1 : Prio 1 -> def_queue and Prio 0 -> Other Q's
	qdisc_tree.num_branches = 2;
	qdisc_tree.port_shaper = if_q_config->port_shaper;
	for (i=0; i<qdisc_tree.num_branches; i++){
		qdisc_tree.q_cfg[i] = NULL;
		qdisc_tree.prio[i] = i;
	}
	major = 1;
	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);

 //Level 2-1
 	memset_s(qdisc_tree.q_cfg, sizeof(qdisc_tree.q_cfg), 0);
	qdisc_tree.num_branches = uiSP1;
	for (i=0; i<qdisc_tree.num_branches; i++){
		qdisc_tree.q_cfg[i] = q_list[i];
		qdisc_tree.prio[i] = 0;
	}
	/* Add one extra node to attach WFQ tree */
	qdisc_tree.num_branches += 1;
	major = 10 * major * 1;
	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);
	
 //Level 3
	j=0;
 	memset_s(qdisc_tree.q_cfg, sizeof(qdisc_tree.q_cfg), 0);
	qdisc_tree.num_branches = uiWFQ;
	for (i=uiSP1; i<(uiSP1+uiWFQ); i++){
		qdisc_tree.q_cfg[j] = q_list[i];
		qdisc_tree.prio[j] = 0;
		j++;
	}
	major = 10 * major + uiSP1 +1;
	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);
	
 //Level 2-2
	j=0;
 	memset_s(qdisc_tree.q_cfg, sizeof(qdisc_tree.q_cfg), 0);
	qdisc_tree.num_branches = uiSP2;
	for (i=uiSP1+uiWFQ; i<(uiSP1+uiWFQ+uiSP2); i++){
		qdisc_tree.q_cfg[j] = q_list[i];
		qdisc_tree.prio[j] = 0;
		j++;
	}
	major = 20;
	add_qdisc(pcQDiscDev, uiPortRate, major, &qdisc_tree);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : sw_queue_show
 *  Description   : Prints complete scheduler configuration of softare queues.
 * ============================================================================*/
static int8_t sw_queue_show(void)
{
	char cmd_buf[MAX_DATA_LEN];
	uint32_t index = 0;
 
	ENTRY

	memset_s(cmd_buf, sizeof(cmd_buf), 0);

	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc show dev %s;\n",qdiscs[index]);
	RUN_CMD(cmd_buf)

	snprintf(cmd_buf, sizeof(cmd_buf), "tc class show dev %s;\n",qdiscs[index]);
	RUN_CMD(cmd_buf)

	snprintf(cmd_buf, sizeof(cmd_buf), "tc filter show dev %s;\n",qdiscs[index]);
	RUN_CMD(cmd_buf)

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : is_queue_unique
 *  Description   : Internal API to check if queue is already present in list of queues
 * ============================================================================*/
static int32_t 
is_queue_unique(qos_queue_cfg_t* pxq_cfg, qos_queue_cfg_t queues[], uint32_t count)
{
	int32_t ret = LTQ_FAIL;
	uint32_t i = 0;
	ENTRY
	
	for (i=0; i<count; i++) {
		/* TODO Criteria to figure out uniqueness should be queue_id based */
		if (strcmp (pxq_cfg->name, queues[i].name) == 0 )
			break;
	}

	if (i < count)
		ret = LTQ_SUCCESS;

	EXIT
	return ret;
}
	
static int32_t sw_queue_configure (char* pcQDiscDev, iftype_t iftype, q_cfg_info_t *if_q_config)
{
	uint32_t uiSP1=0, uiWFQ=0, uiSP2=0;
	QueueMode eQueueMode = QUEUEING_MODE_NONE;
	uint32_t uiPortRate = 0;
	qos_queue_cfg_t *q_list[CPU_N_MAX_Q];

	ENTRY

	sysapi_iface_rate_get(pcQDiscDev,iftype, &uiPortRate);
	if (uiPortRate == 0 || uiPortRate > MAX_IF_RATE) {
		LOGF_LOG_ERROR("Invalid Interface Rate!\n");
		return LTQ_FAIL;
	}
	eQueueMode = getQueueMode(&uiSP1, &uiWFQ, &uiSP2, if_q_config, q_list);

	if (eQueueMode == QUEUEING_MODE_SP_WFQ)
		sw_queue_add_sp_wfq(pcQDiscDev, uiPortRate, uiSP1, uiWFQ, if_q_config, q_list);

	else if (eQueueMode == QUEUEING_MODE_SP)
		sw_queue_add_sp(pcQDiscDev, uiPortRate, uiSP1, if_q_config, q_list);

	else if (eQueueMode == QUEUEING_MODE_WFQ)
		sw_queue_add_wfq(pcQDiscDev, uiPortRate, uiWFQ, if_q_config, q_list);

	else if (eQueueMode == QUEUEING_MODE_SP_WFQ_SP)
		sw_queue_add_sp_wfq_sp(pcQDiscDev, uiPortRate, uiSP1, uiWFQ, uiSP2, if_q_config, q_list);

	else {
		LOGF_LOG_ERROR("Configured queue structure is not supported\n");
		return LTQ_FAIL;
	}

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_queue_add
 *  Description   : Callback API of CPU-N engine to add specified queue.
 * ============================================================================*/
int32_t cpu_n_eng_queue_add (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t* pxq_cfg)
{
	char cmd_buf[MAX_DATA_LEN];
	uint32_t qdisc_index = 0;
	uint32_t flags = 0;
	int32_t weight = 0, prio = 0;
	qos_shaper_t port_shaper_local = {0};
	q_cfg_info_t if_q_config = {0};
	qos_queue_cfg_t *pxq_cfg_local=NULL;
	UNUSED(ifinggrp);

	ENTRY

	qdisc_index = get_qdisc_index(iftype);

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc del dev %s parent root;\n",qdiscs[qdisc_index]);
	RUN_CMD(cmd_buf)
	
	pxq_cfg_local=if_q_config.q_cfg;
	qosd_queue_get(ifname, NULL, &if_q_config.uiNoOfQueues, &pxq_cfg_local, flags);

	if (if_q_config.uiNoOfQueues == 0 ) {
		/* it should be invoked when adding first queue on the interface, 
		 this scrip add redirection rules at default wan for upstream, at br0 for downstream. */
		set_pkts_redirection(iftype, ifname, qdiscs[qdisc_index], QOSAL_TRUE);
	}

	//Add the current queue in the currently configured queues
	if (is_queue_unique(pxq_cfg, if_q_config.q_cfg, if_q_config.uiNoOfQueues)) {
		memcpy_s(&if_q_config.q_cfg[if_q_config.uiNoOfQueues], sizeof(qos_queue_cfg_t), pxq_cfg, sizeof(qos_queue_cfg_t));
		if_q_config.uiNoOfQueues++;
	}

	qosd_port_config_get(ifname, &port_shaper_local, &weight, &prio, flags);
	if (port_shaper_local.enable == QOSAL_ENABLE)
		if_q_config.port_shaper = &port_shaper_local;

	sw_queue_configure(qdiscs[qdisc_index], iftype, &if_q_config);
	tc_q_map_add(ifname, iftype, pxq_cfg);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_queue_delete
 *  Description   : Callback API of CPU-N engine to delete specified queue.
 * ============================================================================*/
int32_t cpu_n_eng_queue_delete (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t* pxq_cfg)
{
	char cmd_buf[MAX_DATA_LEN];
	uint32_t i = 0;
	uint32_t qdisc_index = 0;
	uint32_t flags = 0;
	int32_t weight = 0, prio = 0;
	qos_shaper_t port_shaper_local = {0};
	q_cfg_info_t if_q_config = {0};
	qos_queue_cfg_t *pxq_cfg_local=NULL;
	UNUSED(ifinggrp);
	ENTRY

	qdisc_index = get_qdisc_index(iftype);

	pxq_cfg_local=if_q_config.q_cfg;
	qosd_queue_get(ifname, NULL, &if_q_config.uiNoOfQueues, &pxq_cfg_local, flags);

	if (if_q_config.uiNoOfQueues == 0){
		return LTQ_SUCCESS;
	}

	if(iftype == QOS_IF_CATEGORY_ETHLAN && (pxq_cfg->flags & QOS_Q_F_LINK_STATE_CHANGE)) {
		/* check if all LAN ports are down */
		if (strncmp(sDefLanIf, ifname, strnlen_s(ifname, MAX_NAME_LEN)) == 0) {
			LOGF_LOG_DEBUG("Queue deletion failed. Some LAN ports are UP and queue deletion on individual LAN port is not supported.");
			return QOS_CFG_DENY_FROM_LINK_CHK;
		}
	}

	//Remove the queue to be deleted from the list of currently configured queues
	for (i=0; i<((if_q_config.uiNoOfQueues)-1); i++){
		if (pxq_cfg->priority == if_q_config.q_cfg[i].priority){
			memmove_s(&if_q_config.q_cfg[i], sizeof(qos_queue_cfg_t)* (if_q_config.uiNoOfQueues-1-i), &if_q_config.q_cfg[i+1], sizeof(qos_queue_cfg_t)* (if_q_config.uiNoOfQueues-1-i));
			if_q_config.uiNoOfQueues--;
			break;
		}
	}

	//if queue to be deleted is last
	if (i == if_q_config.uiNoOfQueues-1 && pxq_cfg->priority == if_q_config.q_cfg[i].priority)
		if_q_config.uiNoOfQueues--;

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	if (if_q_config.uiNoOfQueues == 0){
		set_pkts_redirection(iftype, ifname, qdiscs[qdisc_index], QOSAL_FALSE);
		snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc del dev %s parent root;\n",qdiscs[qdisc_index]);
		RUN_CMD(cmd_buf)
		return LTQ_SUCCESS;
	}
	
	//Add if atleast one queue remains after this delete request
	else if (if_q_config.uiNoOfQueues > 0){
		snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc del dev %s parent root;\n",qdiscs[qdisc_index]);
		RUN_CMD(cmd_buf)
	}

	qosd_port_config_get(ifname, &port_shaper_local, &weight, &prio, flags);
	if (port_shaper_local.enable == QOSAL_ENABLE)
		if_q_config.port_shaper = &port_shaper_local;

	sw_queue_configure(qdiscs[qdisc_index], iftype, &if_q_config);
	tc_q_map_delete(ifname, iftype, pxq_cfg);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_port_del
 *  Description   : API to delete port configuration.
 * ============================================================================*/
static int32_t cpu_n_eng_port_del (char *ifname, iftype_t iftype, qos_shaper_t  *shaper)
{
	char cmd_buf[MAX_DATA_LEN];
	uint32_t qdisc_index = 0; //get index
	uint32_t flags = 0;
	qos_queue_cfg_t *pxq_cfg_local=NULL;
	q_cfg_info_t if_q_config = {0};

	ENTRY

	UNUSED(shaper);

	qdisc_index = get_qdisc_index(iftype);
	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc del dev %s parent root;\n",qdiscs[qdisc_index]);
	RUN_CMD(cmd_buf)

	pxq_cfg_local=if_q_config.q_cfg;
	qosd_queue_get(ifname, NULL, &if_q_config.uiNoOfQueues, &pxq_cfg_local, flags);

	if (if_q_config.uiNoOfQueues == 0 ) {
		set_pkts_redirection(iftype, ifname, qdiscs[qdisc_index], QOSAL_FALSE);
		goto exit;
	}

	sw_queue_configure(qdiscs[qdisc_index], iftype, &if_q_config);

exit:

	EXIT
	return LTQ_SUCCESS;
}


/* =============================================================================
 *  Function Name : cpu_n_eng_port_add
 *  Description   : API to add port configuration.
 * ============================================================================*/
static int32_t cpu_n_eng_port_add (char *ifname, iftype_t iftype, qos_shaper_t  *shaper)
{
	char cmd_buf[MAX_DATA_LEN];
	uint32_t qdisc_index = 0; //get index
	uint32_t flags = 0;
	qos_queue_cfg_t *pxq_cfg_local=NULL;
	q_cfg_info_t if_q_config = {0};

	ENTRY

	qdisc_index = get_qdisc_index(iftype);
	LOGF_LOG_DEBUG("shaper pir is %d, cir:%d\n",shaper->pir, shaper->cir);
	if (shaper->mode != QOS_SHAPER_NONE && shaper->pir != 0) {
		if_q_config.port_shaper = shaper;
	}
	else {
		return LTQ_FAIL;
	}

	LOGF_LOG_DEBUG("2shaper pir is %d, cir:%d\n",if_q_config.port_shaper->pir,if_q_config.port_shaper->cir);
	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc del dev %s parent root;\n",qdiscs[qdisc_index]);
	RUN_CMD(cmd_buf)

	pxq_cfg_local=if_q_config.q_cfg;
	qosd_queue_get(ifname, NULL, &if_q_config.uiNoOfQueues, &pxq_cfg_local, flags);

	if (if_q_config.uiNoOfQueues == 0 ) {
		set_pkts_redirection(iftype, ifname, qdiscs[qdisc_index], QOSAL_TRUE);
		memset_s(cmd_buf, sizeof(cmd_buf), 0);
		snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc add dev %s root handle 1: htb;\n", qdiscs[qdisc_index]);
		RUN_CMD(cmd_buf)
		add_qdisc(qdiscs[qdisc_index], 0, 1, NULL);
		goto exit;
	}


	sw_queue_configure(qdiscs[qdisc_index], iftype, &if_q_config);

exit:

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_port_set
 *  Description   : Callbak API of CPU-N engine to do port configuration (add/delete)
 * ============================================================================*/
int32_t cpu_n_eng_port_set (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_shaper_t  *shaper, uint32_t priority, uint32_t weight, uint32_t flag)
{
	ENTRY
	UNUSED(ifinggrp);
	UNUSED(priority);
	UNUSED(weight);
	if (flag & QOS_OP_F_ADD)
		cpu_n_eng_port_add (ifname, iftype, shaper);
	else if (flag & QOS_OP_F_DELETE)
		cpu_n_eng_port_del (ifname, iftype, shaper);

	EXIT
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_queue_modify
 *  Description   : Callback API of CPU-N engine to do queue modification.
 * ============================================================================*/
int32_t cpu_n_eng_queue_modify (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, qos_queue_cfg_t* pxq_cfg)
{
	char cmd_buf[MAX_DATA_LEN];
	uint32_t qdisc_index = 0; //get index
	uint32_t i = 0;
	uint32_t flags = 0;
	int32_t weight = 0, prio = 0;
	qos_shaper_t port_shaper_local = {0};
	q_cfg_info_t if_q_config = {0};
	qos_queue_cfg_t *pxq_cfg_local=NULL;
	UNUSED(ifinggrp);
	ENTRY

	qdisc_index = get_qdisc_index(iftype);

	memset_s(cmd_buf, sizeof(cmd_buf), 0);
	snprintf(cmd_buf, sizeof(cmd_buf), "tc qdisc del dev %s parent root;\n",qdiscs[qdisc_index]);
	RUN_CMD(cmd_buf)
	
	pxq_cfg_local=if_q_config.q_cfg;
	qosd_queue_get(ifname, NULL, &if_q_config.uiNoOfQueues, &pxq_cfg_local, flags);
	//Update the current queue in the currently configured queues
	for (i=0; i<if_q_config.uiNoOfQueues && i<CPU_N_MAX_Q; i++) {
		if (strcmp(if_q_config.q_cfg[i].name, pxq_cfg->name) == 0) {
			memcpy_s(&if_q_config.q_cfg[i], sizeof(qos_queue_cfg_t), pxq_cfg, sizeof(qos_queue_cfg_t));
			break;
		}
	}

	if (i == if_q_config.uiNoOfQueues) {
		LOGF_LOG_ERROR("Queue %s is not configured software\n",pxq_cfg->name);
		return QOS_QUEUE_NOT_FOUND;
	}

	qosd_port_config_get(ifname, &port_shaper_local, &weight, &prio, flags);
	if (port_shaper_local.enable == QOSAL_ENABLE)
		if_q_config.port_shaper = &port_shaper_local;

	sw_queue_configure(qdiscs[qdisc_index], iftype, &if_q_config);

	//check if map is updated
	tc_q_map_delete(ifname, iftype, &if_q_config.q_cfg[i]);
	tc_q_map_add(ifname, iftype, pxq_cfg);

	EXIT

	return LTQ_SUCCESS;
}

static uint32_t get_counters(char *buf, char *tag)
{
	char *ptr = NULL;
	char temp[10] = "\0";
	uint32_t i = 0;

	ptr = strstr(buf, tag);
	if (ptr == NULL)
		return LTQ_SUCCESS;

	ptr += strnlen_s(tag, 20);
	while (*(ptr + i) != ' ') {
		temp[i] = *(ptr + i);
		i++;
	}

	return atoi (temp);
}

static uint32_t get_q_stats(char *dev, char *q_id, qos_queue_stats_t* qstats)
{
	FILE *fd = NULL;
	char buf[5000] = {0};
	char cmd[100] = "\0";
	char tag[20] = "\0";
	char *ptr = NULL;

	snprintf (cmd, sizeof(cmd), "tc -s class show dev %s", dev);
	fd = popen(cmd, "r");

	if (fd != NULL) {
		if(fread(buf, 1, (sizeof(buf) - 1), fd) <= 0) {
			LOGF_LOG_ERROR("Could not get queue counters for device [%s]..\n", dev);
			pclose(fd);
			return 0;
		}
		buf[sizeof(buf) - 1] = '\0';
		pclose(fd);
	}

	snprintf(tag, sizeof(tag), "class htb %s",q_id);
	ptr = strstr(buf, tag);
	if (ptr == NULL)
		return LTQ_SUCCESS;

	qstats->tx_bytes = get_counters(ptr, "Sent ");
	qstats->flags |= QOS_Q_STAT_TX_BYTE;
	qstats->tx_pkt = get_counters(ptr, "bytes ");
	qstats->flags |= QOS_Q_STAT_TX_PKT;
	qstats->drop_pkt = get_counters(ptr, "dropped ");
	qstats->flags |= QOS_Q_STAT_DROP_PKT;

	return 0;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_queue_delete
 *  Description   : Callback API of CPU-N engine to delete specified queue.
 * ============================================================================*/
int32_t cpu_n_eng_stats_get (char *ifname, iftype_t iftype, ifinggrp_t ifinggrp, char *qname, qos_queue_stats_t* qstats)
{
	uint32_t qdisc_index = 0;
	uint32_t flags = 0, i = 0;
	uint32_t uiSP1=0, uiWFQ=0, uiSP2=0;
	char q_id[24] = {0};
	QueueMode eQueueMode = QUEUEING_MODE_NONE;
	qos_queue_cfg_t *q_list[CPU_N_MAX_Q] = {0};
	q_cfg_info_t if_q_config = {0};
	qos_queue_cfg_t *pxq_cfg_local=NULL;
	UNUSED(ifinggrp);
	ENTRY

	qdisc_index = get_qdisc_index(iftype);

	pxq_cfg_local=if_q_config.q_cfg;
	qosd_queue_get(ifname, NULL, &if_q_config.uiNoOfQueues, &pxq_cfg_local, flags);
	eQueueMode = getQueueMode(&uiSP1, &uiWFQ, &uiSP2, &if_q_config, q_list);

	for (i=0; i<CPU_N_MAX_Q && q_list[i]; i++) {
		if (strcmp(qname, q_list[i]->name) == 0)
			break;
	}

	if (i >= if_q_config.uiNoOfQueues) {
		LOGF_LOG_ERROR("Queue with name [%s] is not available in software\n", qname);
		return LTQ_FAIL;
	}

	if (eQueueMode == QUEUEING_MODE_SP || eQueueMode == QUEUEING_MODE_WFQ) {
		snprintf(q_id, sizeof(q_id), "1:%d", (i+1)*10);
	}
	else if (eQueueMode == QUEUEING_MODE_SP_WFQ) {
		if(i<CPU_N_MAX_Q) {
			if (q_list[i]->sched == QOS_SCHED_WFQ)
				snprintf(q_id, sizeof(q_id), "%d:%d", 10*(uiSP1+1), (uiSP1+1+i));
			else
				snprintf(q_id, sizeof(q_id), "1:%d", (i+1)*10);
		}
	}

	get_q_stats(qdiscs[qdisc_index], q_id, qstats);

	EXIT

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_wan_init
 *  Description   : API to do initialization for WAN egress scenarios.
 * ============================================================================*/
static int32_t cpu_n_eng_wan_init(void)
{
	/* TODO up IFB device */
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : cpu_basicIfRulesCfg
 *  Description   : Configures rules to match interface specific rules
 * ============================================================================*/
static int32_t cpu_basicIfRulesCfg(int32_t key, struct blob_buf *b, uint32_t *rule_idx)
{
	char cl_act[MAX_DATA_LEN];
	int32_t retval = LTQ_SUCCESS;

	memset_s(cl_act, sizeof(cl_act), 0);

	switch(key) {
		case 1:
			/* CL_LIST -> no rules to add. */
			break;
		case 2:
			/* LAN_CL -> add rules to redirect packets from br0, eth0 to LAN_CL. */
			snprintf(cl_act, sizeof(cl_act), "-i %s -j LAN_CL", CPU_N_INTERFACE_LAN_BRIDGE);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, CL_LIST, -1, cl_act, b, (*rule_idx)++);
			snprintf(cl_act, sizeof(cl_act), "-i %s -j LAN_CL", CPU_N_INTERFACE_LAN);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, CL_LIST, -1, cl_act, b, (*rule_idx)++);
			break;
		case 3:
			/* WAN_CL -> add rules to redirect packets from nas*, eth1*, ppp*, ptm* to WAN_CL. */
			snprintf(cl_act, sizeof(cl_act), "-i %s -j WAN_CL", CPU_N_INTERFACE_WAN_ATM);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, CL_LIST, -1, cl_act, b, (*rule_idx)++);
			snprintf(cl_act, sizeof(cl_act), "-i %s -j WAN_CL", CPU_N_INTERFACE_WAN);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, CL_LIST, -1, cl_act, b, (*rule_idx)++);
			snprintf(cl_act, sizeof(cl_act), "-i %s -j WAN_CL", CPU_N_INTERFACE_WAN_PTM);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, CL_LIST, -1, cl_act, b, (*rule_idx)++);
			snprintf(cl_act, sizeof(cl_act), "-i %s -j WAN_CL", CPU_N_INTERFACE_WAN_ETH);
			retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, CL_LIST, -1, cl_act, b, (*rule_idx)++);
			break;
	}

	return retval;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_init
 *  Description   : Callback API of CPU-N engine to do initialization of engine.
 * ============================================================================*/
int32_t cpu_n_eng_init(qos_module_type_t mtype)
{
	ENTRY
	int32_t retval = LTQ_SUCCESS, loopKey = 0, i = 0;
	char rootChName[MAX_NAME_LEN], cl_act[MAX_DATA_LEN];
	struct blob_buf b = {};
	uint32_t rule_idx = 0;

	retval = blob_buf_init(&b, 0);
	if (retval != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return retval;
	}

	if((mtype == QOS_MODULE_QUEUE) || (mtype == QOS_MODULE_BOTH))  {
		cpu_n_eng_wan_init();
		tc_q_map_init();
	}
	if(mtype == QOS_MODULE_CLASSIFIER)
		del_def_wmm_dscp_class();

	if((mtype == QOS_MODULE_CLASSIFIER) || (mtype == QOS_MODULE_BOTH))  {
		cl_adapt = CL_ADAPT_IPTEBT; /* by default, use iptables/ebtables adapter. */

		for(i=0; i<2; i++)
		{
			if(i == 0)
				IPT = IPV4T;
			else if(i == 1)
				IPT = IPV6T;
	
			switch(cl_adapt) {
				case CL_ADAPT_IPTEBT:
					/* need to setup chain structure. */
					loopKey = 1;
					while(loopKey < 6) {
						switch(loopKey) {
							case 1:
								snprintf(rootChName, sizeof(rootChName), "%s", CL_LIST);
								break;
							case 2:
								snprintf(rootChName, sizeof(rootChName), "%s", LAN_CL);
								break;
							case 3:
								snprintf(rootChName, sizeof(rootChName), "%s", WAN_CL);
								break;
							case 4:
								/* create default classifier chain for LAN. */
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, DEF_LAN_CL, -1, NULL, &b, rule_idx++);
								/* mark all packets in this chain with default queue priority. */
								snprintf(cl_act, sizeof(cl_act), "-j %s --set-mark %d", MARK, DEF_US_MARK);
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, DEF_LAN_CL, -1, cl_act, &b, rule_idx++);
								/* and accept the packets. */
								snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j ACCEPT", mark, DEF_US_MARK, DEF_US_MARK);
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, DEF_LAN_CL, -1, cl_act, &b, rule_idx++);
								snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j %s",mark, CLASSIFICATION_NOT_DONE_MASK, CLASSIFICATION_NOT_DONE_MASK, DEF_LAN_CL);
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, LAN_CL, -1, cl_act, &b, rule_idx++);
						
								/* create default classifier chain for WAN. */
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, DEF_WAN_CL, -1, NULL, &b, rule_idx++);
								/* mark all packets in this chain with default queue priority. */
								snprintf(cl_act, sizeof(cl_act), "-j %s --set-mark %d", MARK, DEF_DS_MARK);
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_INS, DEF_WAN_CL, -1, cl_act, &b, rule_idx++);
								/* and accept the packets. */
								snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j ACCEPT", mark, DEF_DS_MARK, DEF_DS_MARK);
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, DEF_WAN_CL, -1, cl_act, &b, rule_idx++);
								/* link this chain to end of LAN classifier list. */
								snprintf(cl_act, sizeof(cl_act), "-m %s --mark %d/%d -j %s",mark, CLASSIFICATION_NOT_DONE_MASK,CLASSIFICATION_NOT_DONE_MASK,DEF_WAN_CL);
								retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, WAN_CL, -1, cl_act, &b, rule_idx++);

								retval = LTQ_SUCCESS;

								loopKey++;
								continue; /* must to do.. */
								break;
							case 5:
								/* flush and remove chain. */
								retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_FL, CL_LIST, -1, NULL, &b, rule_idx++);
								retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_REM, CL_LIST, -1, NULL, &b, rule_idx++);

								/* create the chain. */
								retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_CREAT, CL_LIST, -1, NULL, &b, rule_idx++);
							
								/* hook up this chain to end of FORWARD chain in filter table. */
								snprintf(cl_act, sizeof(cl_act), "-j %s", CL_LIST);
								retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, RULE_APP, EBT_QOS_STD_HOOK, -1, cl_act, &b, rule_idx++);
								loopKey++;
								continue; /* must to do.. */
								break;	
							default:
								break;	
						}
			
						/* create the chain. */
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_CREAT, rootChName, -1, NULL, &b, rule_idx++);
				
						/* hook up this chain to beginning of PREROUTING chain in mangle table. */
						if(!strncmp(rootChName, CL_LIST, strnlen_s(rootChName, MAX_NAME_LEN))) {
							snprintf(cl_act, sizeof(cl_act), "-j %s", rootChName);
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_APP, IPT_QOS_STD_HOOK, -1, cl_act, &b, rule_idx++);
						}
			
						/* LAN_CL and WAN_CL chains should have basic rules to identify
						 * interfaces type.
						 */
						cpu_basicIfRulesCfg(loopKey, &b, &rule_idx);
			
						loopKey++;
					}
					break;
				default:
					break;
			}
		}
	}
	send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_INIT);

	EXIT
	return retval;
}

/* =============================================================================
 *  Function Name : cpu_n_eng_fini
 *  Description   : Callback API of CPU-N engine to do un-initialization of engine.
		    Assumption that caller of this API has removed all configuration
		    from this engine before coming here.
 * ============================================================================*/
int32_t cpu_n_eng_fini(qos_module_type_t mtype)
{
	ENTRY
	int32_t retval = LTQ_SUCCESS, loopKey = 0;
	char rootChName[MAX_NAME_LEN], cl_act[MAX_DATA_LEN];
	struct blob_buf b = {};
	uint32_t rule_idx = 0;

	retval = blob_buf_init(&b, 0);
	if (retval != LTQ_SUCCESS) {
		LOGF_LOG_ERROR("Message Init Failed\n");
		return retval;
	}

	if((mtype == QOS_MODULE_QUEUE) || (mtype == QOS_MODULE_BOTH))  {
		tc_q_map_fini();
	}
	if(mtype == QOS_MODULE_CLASSIFIER)
		add_def_wmm_dscp_class();
	if((mtype == QOS_MODULE_CLASSIFIER) || (mtype == QOS_MODULE_BOTH))  {
		/* remove basic iptables chains. */
		switch(cl_adapt) {
			case CL_ADAPT_IPTEBT:
				loopKey = 1;
				while(loopKey < 6) {
					switch(loopKey) {
						case 1:
							snprintf(rootChName, sizeof(rootChName), "%s", CL_LIST);
							break;
						case 2:
							snprintf(rootChName, sizeof(rootChName), "%s", LAN_CL);
							break;
						case 3:
							snprintf(rootChName, sizeof(rootChName), "%s", WAN_CL);
							break;
						case 4:
							/* flush and remove default classifier chain for LAN. */
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, DEF_LAN_CL, -1, NULL, &b, rule_idx++);
							/* remove this chain association with LAN_CL chain. */
							snprintf(cl_act, sizeof(cl_act), "-j %s", DEF_LAN_CL);
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, LAN_CL, -1, cl_act, &b, rule_idx++);
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, DEF_LAN_CL, -1, NULL, &b, rule_idx++);
		
							/* flush and remove default classifier chain for WAN. */
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, DEF_WAN_CL, -1, NULL, &b, rule_idx++);
							/* remove this chain association with LAN_CL chain. */
							snprintf(cl_act, sizeof(cl_act), "-j %s", DEF_WAN_CL);
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, WAN_CL, -1, cl_act, &b, rule_idx++);
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, DEF_WAN_CL, -1, NULL, &b, rule_idx++);
							
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, DEF_INGRESS_CL, -1, NULL, &b, rule_idx++);
							snprintf(cl_act, sizeof(cl_act), "-j %s",DEF_INGRESS_CL);
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, WAN_CL, -1, cl_act, &b, rule_idx++);
							retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, DEF_INGRESS_CL, -1, NULL, &b, rule_idx++);
							
							loopKey++;
							continue; /* must to do.. */
							break;	
						case 5:
							/* remove root chain in ebtables. */
							snprintf(rootChName, sizeof(rootChName), "%s", CL_LIST);
							/* flush and remove chain. */
							retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_FL, CL_LIST, -1, NULL, &b, rule_idx++);
							/* remove this chain association with FORWARD chain in filter table. */
							snprintf(cl_act, sizeof(cl_act), "-j %s", CL_LIST);
							retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_FL, EBT_QOS_STD_HOOK, -1, cl_act, &b, rule_idx++);
							retval = set_ebt_ipt_cmd_wrap(EBT_QOS_TBL, CH_REM, CL_LIST, -1, NULL, &b, rule_idx++);
							loopKey++;
							continue; /* must to do.. */
							break;
						default:
							break;	
					}
					/* flush and remove chain. */
					retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_FL, rootChName, -1, NULL, &b, rule_idx++);
					/* remove this chain association with PREROUTING chain in mangle table. */
					if(!strncmp(rootChName, CL_LIST, strnlen_s(rootChName, MAX_NAME_LEN))) {
						snprintf(cl_act, sizeof(cl_act), "-j %s", rootChName);
						retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, RULE_DEL, IPT_QOS_STD_HOOK, -1, cl_act, &b, rule_idx++);
					}
					retval = set_ebt_ipt_cmd_wrap(IPT_QOS_TBL, CH_REM, rootChName, -1, NULL, &b, rule_idx++);
			
					loopKey++;
				}
				break;
			default:
				break;
		}

		send_iptable_msg(b, FIREWALLD_UBUS_METHOD_NOTIFY_QOS_INIT);
		/* removing TC rules. TODO */
	}

	EXIT
	return retval;
}
