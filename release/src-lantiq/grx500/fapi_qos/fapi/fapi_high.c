/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include<ctype.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#include "commondefs.h"
#include "qosal_debug.h"
#include "qosal_ipc.h"
#include "qosal_queue_api.h"
#include "qosal_cl_api.h"
#include "qosal_ipc_msg.h"
#include "qosal_utils.h"
#include "qosal_ugw_debug.h"
#include "fapi_debug.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

#define QOSD_NOT_RUN_STR "qosd may not be running plz check\n"
#define QOS_NOT_ENA	"QoS is not enabled\n"

typedef struct 
{
        int errorcode;
        char * msg;
}errormsg;

errormsg printmsg[] = {
			{QOS_ENG_QCNT_EQ_LT, "Number of queues has exceeded capacity."},
			{QOS_IF_ABS_SET_DEP_FAIL, "Dependency check failed for interface abstraction set."},
			{QOS_ENTRY_EXISTS, "Configuration already exists with this name."},
			{QOS_Q_TC_CLASH, "Queue already exists with this traffic class."},
			{QOS_Q_PRIO_CLASH, "Strict Priority queue already exists with this priority."},
			{QOS_Q_PRIO_NOT_SUPP, "Queue priority not supported for selected configuration."},
			{QOS_CLASSIFIER_ENTRY_EXISTS, "Classifier entry already exists."},
			{QOS_IFMAP_LOOKUP_FAIL, "Interface look up failed."},
			{QOS_IF_NOT_DEF_WAN, "Queue creation is allowed only on default WAN interface."},
			{QOS_IF_ADD_FAIL, "Inteface add failed."},
			{QOS_ENG_CFG_FAIL, "Engine configuration failed."},
			{QOS_SWITCH_ENG_CFG_FAIL, "Switch engine configuration failed."},
			{QOS_PPA_ENG_CFG_FAIL, "PPA engine configuration failed."},
			{QOS_CPU_ENG_CFG_FAIL, "CPU engine configuration failed."},
			{QOS_CL_ORDER_CLASH, "Classifier entry order clash found."},
			{QOS_MEM_ALLOC_FAIL, "Memory allocation failed."},
			{QOS_IF_NOT_FOUND, "Interface not found."},
			{QOS_QUEUE_NOT_FOUND, "Classifier entry not found."},
			{QOS_CLASSIFIER_NOT_FOUND, "Classifier entry not found."},
			{QOS_ENTRY_NOT_FOUND, "Entry not found."},
			{QOS_DEF_Q_DEP_IN_ADD_FAIL, "Default queue (having sub-string 'def_queue' in name) should be added first."},
			{QOS_DEF_Q_DEP_IN_DEL_FAIL, "Default queue (having sub-string 'def_queue' in name) should be deleted last."},
			{QOS_RATE_VALID_FAIL, "Peak/commit rate can not be more than link rate."},
			{QOS_Q_RESERVED, "Queue is reserved for management traffic. Please select any other queue."},
			{QOS_CL_POLICER_UNSUP_PARAM, "Some/all of the configured filters are not supported with policer action."},
			{QOS_Q_MOD_FAIL_CL_DEP, "Classifiers are associated with queue. Disable or delete the classifiers before changing Name/Traffic class/precedence of queue."},
			{QOS_Q_WLAN_PRIO_INVALID, "Priority of the Wlan Queues should be between 5 to 8. Priority 1 to 4 Queues on wlan are created by default on QoS Enable."},
			{QOS_Q_RATE_INVALID, "Create rate shaper with rate greater than 16 Kbps."},
			{QOS_Q_TC_ZERO_INVALID, "Traffic Class value is invalid."},
			{QOS_CL_SIP_ADDR_INVALID, "Source IP address is not valid."},
			{QOS_CL_SIP_MASK_INVALID, "Source IP mask is not valid."},
			{QOS_CL_DIP_ADDR_INVALID, "Destination IP address is not valid."},
			{QOS_CL_DIP_MASK_INVALID, "Destination IP mask is not valid."},
			{QOS_INVALID_PARAM, "INVALID PARAM."},
			{QOS_INVALID_Q_SH_PARAM, "Invalid Shaper Rate."},
			{QOS_INVALID_Q_SCHED, "Invalid Sched Algo or Weight"},
			{QOS_INVALID_CL_ORDER_RANGE, "Invalid Classifier Order Range, should be 1-20."},
			{QOS_INVALID_SPORT, "Invalid Source Port should be in Range of 0-65535."},
			{QOS_INVALID_SPORT_RANGE, "Invalid Source Port Range should be in Range of 0-65535."},
			{QOS_INVALID_DPORT, "Invalid Destination Port should be in Range of 0-65535."},
			{QOS_INVALID_DPORT_RANGE, "Invalid Destination Port Range should be in Range of 0-65535."},
			{QOS_INVALID_SMAC_RANGE, "Invalid Source Mac Address."},
			{QOS_INVALID_SMAC_MASK, "Invalid Source Mac MASK."},
			{QOS_INVALID_DMAC_RANGE, "Invalid Destination Mac Address."},
			{QOS_INVALID_DMAC_MASK, "Invalid Destination Mac MASK."},
			{QOS_CL_PROTO_NOT_SET, "Protocol is required while setting the port values."},
			{QOS_CL_INVALID_TC, "Traffic Class mark values are not valid"},
			{QOS_SHAPER_NAME_CLASH, "Shaper exists with same name"},
			{QOS_SHAPER_ENTRY_EXISTS, "Shaper already present on interface"},
			{QOS_INVALID_DSCP_MARK, "Invalid DSCP Mark value"},
			{QOS_INVALID_DSCP, "Invalid DSCP Value"}
		      };

int print_error_message(IN int respCode);
int find_error_message(IN int respCode, OUT char *sErrMsg);

int print_error_message(IN int respCode)
{
        uint32_t i;
        for(i=0;i<sizeof(printmsg)/sizeof(errormsg);i++)
        {
                if(respCode == printmsg[i].errorcode)
                {
                        LOGF_LOG_ERROR("%s \n", printmsg[i].msg);
			break;
                }
        }
        return LTQ_SUCCESS;
}

int find_error_message(IN int respCode, OUT char *sErrMsg)
{
        uint32_t i;
	int32_t nRet = LTQ_FAIL;
        for(i=0;i<sizeof(printmsg)/sizeof(errormsg);i++)
        {
                if(respCode == printmsg[i].errorcode)
                {
                        snprintf(sErrMsg, 128, "%s", printmsg[i].msg);
			nRet = LTQ_SUCCESS;
			break;
                }
        }
        return nRet;
}


static int32_t validate_qcfg_threshold(IN qos_queue_cfg_t *pxqcfg)
{
	int nRet = LTQ_SUCCESS;
	if (pxqcfg->drop.flags & QOS_DT_F_GREEN_THRESHOLD) {
		if (pxqcfg->drop.wred.max_th0 > QOS_MAX_Q_LENGTH_THRESHOLD)
			return LTQ_FAIL;
		pxqcfg->drop.wred.max_th0 = pxqcfg->drop.wred.max_th0/8 + ((pxqcfg->drop.wred.max_th0 % 8) > 0 ?1:0) ;
	}
	if (pxqcfg->drop.flags & QOS_DT_F_YELLOW_THRESHOLD) {
		if (pxqcfg->drop.wred.max_th1 > QOS_MAX_Q_LENGTH_THRESHOLD)
			return LTQ_FAIL;
		pxqcfg->drop.wred.max_th1 = pxqcfg->drop.wred.max_th1/8 + ((pxqcfg->drop.wred.max_th1 % 8) > 0 ?1:0) ;
	}
	if (pxqcfg->drop.flags & QOS_DT_F_RED_THRESHOLD) {
		if (pxqcfg->drop.wred.drop_th1 > QOS_MAX_Q_LENGTH_THRESHOLD)
			return LTQ_FAIL;
		pxqcfg->drop.wred.drop_th1 = pxqcfg->drop.wred.drop_th1/8 + ((pxqcfg->drop.wred.drop_th1 % 8) > 0 ?1:0) ;
	}
	return nRet;
}

/* =============================================================================
 *  Function Name : fapi_qos_validateqcfg
 *  Description   : Validates queue structure, for use within FAPI layer only.
		    It will do checks to ensure key parameters are set and
		    dependencies across parameters are met.
 * ============================================================================*/
static int32_t
fapi_qos_validateqcfg(IN qos_queue_cfg_t *pxqcfg, uint32_t flags)
{
	int32_t nRet = LTQ_SUCCESS;
	UNUSED(nRet);
	if (pxqcfg->sched < QOS_SCHED_SP || pxqcfg->sched > QOS_SCHED_WFQ) {
		printf("Invalid scheduler\n");
		return LTQ_FAIL;
	}
	else {
		if((pxqcfg->weight<0) || (pxqcfg->priority<0 || pxqcfg->priority > MAX_PRIO)){
			printf("Invalid Weight or prio\n");
			return LTQ_FAIL;
		}
	}
	/* Check for drop validity */
	if((pxqcfg->flags&QOS_OP_F_QUEUE_DROP) == QOS_OP_F_QUEUE_DROP){
		if (pxqcfg->drop.mode < QOS_DROP_TAIL || pxqcfg->drop.mode > QOS_DROP_CODEL) {
			printf("Invalid drop mode\n");
			return LTQ_FAIL;
		}
	}
	/* Check for shaper validity */
	if((pxqcfg->flags&QOS_OP_F_QUEUE_SHAPER)==QOS_OP_F_QUEUE_SHAPER){
		if (pxqcfg->shaper.mode < QOS_SHAPER_NONE || pxqcfg->shaper.mode > QOS_SHAPER_LOOSE_COLOR_BLIND) {
			printf("Invalid shaper mode\n");
			return LTQ_FAIL;
		}
	}
	/* Check if qlen is set */
	if(pxqcfg->qlen==0){
		printf("Invalid qlen\n");
		return LTQ_FAIL;
	}
	if((flags & QOS_OP_F_ADD)||(flags & QOS_OP_F_MODIFY)) {
		if(pxqcfg->qlen!=102400){
			printf("Warning : Default Software TC Qdisc length is set to 102400 bytes\n");
			return LTQ_FAIL;
		}
		nRet=validate_qcfg_threshold(pxqcfg);
		if(nRet != LTQ_SUCCESS) {
			printf("Invalid Threshold value: should be set in 0-2304\n");
			return LTQ_FAIL;
		}
	}

	if (pxqcfg->sched == QOS_SCHED_WFQ && pxqcfg->shaper.cir == 0 && pxqcfg->weight == 0) {
		printf("Weight and CIR both can not be zero for WFQ\n");
		return LTQ_FAIL;
	}

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_init
 *  Description   : FAPI to initialize QoS daemon.
		    to initialize, it will start qosd if not running already.
		    And internally call another FAPI to enable QoS and
		    set default log settings to LOG_FILE.
 * ============================================================================*/
int32_t
fapi_qos_init(IN qos_cfg_t *qoscfg, IN uint32_t flags)
{

	int32_t nAttempt = 0;

	ENTRY

	if((flags & QOS_DONT_USE_DEF_CFG) == QOS_DONT_USE_DEF_CFG) {
		/* just start qos daemon and dont apply default configuration */
	}
	else {
		int32_t iret = -1;
		x_ipc_msg_t xmsg;
		x_QIPC_qoscfgReq *pxqoscfgReq;
		
		if(sizeof(x_QIPC_qoscfgReq)>IPC_MAX_MSG_SIZE){
			printf("Message size too big\n");
			return LTQ_FAIL;
		}

		if (qoscfg->ena == QOSAL_ENABLE) {
			/* Populate Msg Body */
			pxqoscfgReq=(x_QIPC_qoscfgReq*)xmsg.acmsg;
			pxqoscfgReq->qoscfg.ena = qoscfg->ena;
			pxqoscfgReq->qoscfg.wmm_ena = qoscfg->wmm_ena;
		}
		else {
			/* Apply default configuration as disable */
			/* Populate Msg Body */
			pxqoscfgReq=(x_QIPC_qoscfgReq*)xmsg.acmsg;
			pxqoscfgReq->qoscfg.ena = 0;
		}
		pxqoscfgReq->qoscfg.log_level = qoscfg->log_level;
		pxqoscfgReq->qoscfg.log_type = qoscfg->log_type;
		memset_s(pxqoscfgReq->qoscfg.log_file, sizeof(pxqoscfgReq->qoscfg.log_file), 0);
		if(strncpy_s(pxqoscfgReq->qoscfg.log_file, MAX_QOS_NAME_LEN, LOG_FILE_LOC, strnlen_s(LOG_FILE_LOC, MAX_QOS_NAME_LEN)) != EOK) {
			LOGF_LOG_ERROR("Error copying log file name string\n");
                        return LTQ_FAIL;
		}

		pxqoscfgReq->uiFlags=flags;
		/*  Populate Msg Header */
		if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
			LOGF_LOG_ERROR("Error copying string\n");
			return LTQ_FAIL;
		}
		if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
                        LOGF_LOG_ERROR("Error copying string\n");
                        return LTQ_FAIL;
                }
		xmsg.xhdr.nMsgType = QIPC_STATUS_SET;
		xmsg.xhdr.unmsgsize=sizeof(x_QIPC_qoscfgReq);
		while(1) {
			if(xmsg.xhdr.unmsgsize > 0 && xmsg.xhdr.unmsgsize <= IPC_MAX_MSG_SIZE) {
				iret=ipc_send_request(&xmsg);
			}
			if(nAttempt < 10 && iret < 0) {
				nAttempt++;
				usleep(500000);
				continue;
			}
			else
				break;
		}
		if(iret < 0) {
			printf(QOSD_NOT_RUN_STR);
			return LTQ_FAIL;
		}
		/* Response if of Generic type so typecast it and dump */
		LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
	}

	EXIT

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_cfg_set
 *  Description   : FAPI to enable/disable QoS configuration. Start/stop of qosd
		    is not handled. If qosd is not running and this FAPI is called
		    then IPC will fail.
 * ============================================================================*/
int32_t
fapi_qos_cfg_set(IN qos_cfg_t *qoscfg,
		IN uint32_t flags)
{
	int32_t iret;
	x_ipc_msg_t xmsg;
	x_QIPC_qoscfgReq *pxqoscfgReq;
	
	if(sizeof(x_QIPC_qoscfgReq)>IPC_MAX_MSG_SIZE){
		printf("Message size too big\n");
		return LTQ_FAIL;
	}
	/* Populate Msg Body */
	pxqoscfgReq=(x_QIPC_qoscfgReq*)xmsg.acmsg;
	memcpy_s(&pxqoscfgReq->qoscfg, sizeof(qos_cfg_t), qoscfg, sizeof(qos_cfg_t));
	pxqoscfgReq->uiFlags=flags;
	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
	    LOGF_LOG_ERROR("Error copying string\n");
	    return LTQ_FAIL;
	}
	if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
	    LOGF_LOG_ERROR("Error copying string\n");
	    return LTQ_FAIL;
	}
	xmsg.xhdr.nMsgType = QIPC_STATUS_SET;
	xmsg.xhdr.unmsgsize=sizeof(x_QIPC_qoscfgReq);
	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_cfg_get
 *  Description   : FAPI to retrieve present QoS configuration.
 * ============================================================================*/
int32_t
fapi_qos_cfg_get(IN_OUT qos_cfg_t *qoscfg,
		IN uint32_t flags)
{
	int32_t iret;
	x_ipc_msg_t xmsg;
	x_QIPC_qoscfgReq *pxqoscfgReq;
	x_QIPC_qosCfgRsp *pxqosCfgRsp;
	
	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
	pxqoscfgReq=(x_QIPC_qoscfgReq *)xmsg.acmsg;
	pxqoscfgReq->uiFlags = flags;
	xmsg.xhdr.nMsgType = QIPC_STATUS_GET;
	xmsg.xhdr.unmsgsize = sizeof(x_QIPC_qoscfgReq);
	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	pxqosCfgRsp =(x_QIPC_qosCfgRsp *)xmsg.acmsg;
	memcpy_s(qoscfg, sizeof(qos_cfg_t), &pxqosCfgRsp->qosCfg, sizeof(qos_cfg_t));
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
	return LTQ_SUCCESS;
}

/* Queuing APIs */
/* =============================================================================
 *  Function Name : fapi_qos_queue_set
 *  Description   : FAPI to set QoS Queue Configuration involves adding a new queue or
		    modifying an existing queue, or deleting an existing queue.
		    It does basic parameter level validation of queue specified.
 * ============================================================================*/
int32_t
fapi_qos_queue_set(IN char	*ifname,
		IN qos_queue_cfg_t *q,
		IN uint32_t flags)
{
	int32_t iret;
	x_ipc_msg_t xmsg;
	x_QIPC_qSetReq *pxqSetReq;
	struct x_qoscfg_t xcfg;
	char *substr = NULL;
	memset_s(&xcfg, sizeof(xcfg), 0);

	iret=fapi_qos_validateqcfg(q,flags);
	if(iret<0){
		printf("Invalid parameters in q config\n");
		return LTQ_FAIL;
	}
	if(sizeof(x_QIPC_qSetReq)>IPC_MAX_MSG_SIZE){
		printf("Message size too big\n");
		return LTQ_FAIL;
	}
	/* Populate Msg Body */
	pxqSetReq=(x_QIPC_qSetReq *)xmsg.acmsg;
	if(strncpy_s(pxqSetReq->acIfName, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying ifname string\n");
		return LTQ_FAIL;
	}
	memcpy_s(&pxqSetReq->xqCfg, sizeof(qos_queue_cfg_t), q, sizeof(qos_queue_cfg_t));
	pxqSetReq->uiFlags=flags;
	/* If this is default queue then pass the same flag to qosd */

	if (strstr_s(q->name, MAX_QOS_NAME_LEN, QOS_DEFAULT_QUEUE_NAME, strnlen_s(QOS_DEFAULT_QUEUE_NAME, MAX_QOS_NAME_LEN), &substr) == EOK) {
		pxqSetReq->xqCfg.flags |= QOS_Q_F_DEFAULT;
	}	

	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
  	xmsg.xhdr.nMsgType = QIPC_QSET_REQ;
	xmsg.xhdr.unmsgsize=sizeof(x_QIPC_qSetReq);

	fapi_qos_cfg_get(&xcfg.qoscfg, xcfg.uiflags);
	if(xcfg.qoscfg.ena != QOSAL_ENABLE)
	{
		LOGF_LOG_CRITICAL(QOS_NOT_ENA);
		return LTQ_FAIL; 
	}
	

	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

        if(xmsg.xhdr.respCode != LTQ_SUCCESS)
        {
                print_error_message(xmsg.xhdr.respCode);
        }

	return xmsg.xhdr.respCode;
}

/* =============================================================================
 *  Function Name : fapi_qos_queue_stats_get
 *  Description   : FAPI to get one or all QoS Queue Statistics on given interface.
 * ============================================================================*/
int32_t
fapi_qos_queue_stats_get(
		char			*ifname,
		char*			queue_name,
		int32_t			*num_queues,
		qos_queue_stats_t	**qstats,
		uint32_t 		flags)
{
	int32_t iret;
	int32_t id;
	x_ipc_msg_t xmsg;
  x_QIPC_qGetReq *pxqGetReq;
	char *data=NULL;
	/* Fill the message */
	pxqGetReq=(x_QIPC_qGetReq *)xmsg.acmsg;
	if(strncpy_s(pxqGetReq->acIfName, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying ifname string\n");
		return LTQ_FAIL;
	}
	pxqGetReq->uiFlags=flags;
	if(strncpy_s(pxqGetReq->acqName, MAX_Q_NAME_LEN, queue_name, strnlen_s(queue_name, MAX_Q_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying Q name string\n");
		return LTQ_FAIL;
	}
	/* Fill the header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
  xmsg.xhdr.nMsgType = QIPC_QSTATSGET_REQ;
	xmsg.xhdr.unmsgsize=sizeof(x_QIPC_qGetReq);
	LOGF_LOG_DEBUG("Request: From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	
	/* Read response from shared memory */
	if (xmsg.xhdr.shm_key == 0 || ipc_get_memory(&xmsg.xhdr.shm_key, &id, &data, 0, 0) != LTQ_SUCCESS) {
		printf("Could not read memory...\n");
		return LTQ_FAIL;
	}
	*num_queues = *((uint32_t *)data);
	qos_queue_stats_t *q_stats = (qos_queue_stats_t*) (data + sizeof(uint32_t));

	*qstats = (qos_queue_stats_t*) os_calloc(1, sizeof(qos_queue_stats_t) * *num_queues);
	memcpy_s(*qstats, sizeof(qos_queue_stats_t) * *num_queues, q_stats, sizeof(qos_queue_stats_t) * *num_queues);
	ipc_delete_memory(id, &data, 1);

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_queue_get
 *  Description   : FAPI to et one of all QoS Queue Configuration on given interface.
 * ============================================================================*/
int32_t
fapi_qos_queue_get(IN char *ifname,
		IN char* queue_name,
		OUT int32_t	*num_queues,
		OUT qos_queue_cfg_t	**q,
		IN uint32_t flags)
{
	int32_t iret;
	int32_t id;
	x_ipc_msg_t xmsg;
  x_QIPC_qGetReq *pxqGetReq;
	char *data=NULL;
	/* Fill the message */
	pxqGetReq=(x_QIPC_qGetReq *)xmsg.acmsg;
	if(strncpy_s(pxqGetReq->acIfName, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
                LOGF_LOG_ERROR("Error copying ifname string\n");
                return LTQ_FAIL;
        }
	pxqGetReq->uiFlags=flags;
	if(strncpy_s(pxqGetReq->acqName, MAX_Q_NAME_LEN, queue_name, strnlen_s(queue_name, MAX_Q_NAME_LEN)) != EOK) {
                LOGF_LOG_ERROR("Error copying Q name string\n");
                return LTQ_FAIL;
        }
	/* Fill the header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
  xmsg.xhdr.nMsgType = QIPC_QGET_REQ;
	xmsg.xhdr.unmsgsize=sizeof(x_QIPC_qGetReq);
	LOGF_LOG_DEBUG("Request: From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	
	/* Read response from shared memory */
	if (xmsg.xhdr.shm_key == 0 || ipc_get_memory(&xmsg.xhdr.shm_key, &id, &data, 0, 0) != LTQ_SUCCESS) {
		printf("Could not read memory...\n");
		return LTQ_FAIL;
	}
	*num_queues = *((uint32_t *)data);
	qos_queue_cfg_t *queues = (qos_queue_cfg_t*) (data + sizeof(uint32_t));

	*q = (qos_queue_cfg_t*) os_calloc(1, sizeof(qos_queue_cfg_t) * *num_queues);
	memcpy_s(*q, sizeof(qos_queue_cfg_t) * *num_queues, queues, sizeof(qos_queue_cfg_t) * *num_queues);
	ipc_delete_memory(id, &data, 1);

	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_port_config_set
 *  Description   : FAPI to configure interface characteristics like shaper,
		    and/or weight/priority if this port is cascaded into another scheduler.
 * ============================================================================*/
int32_t
fapi_qos_port_config_set(IN char *ifname,
			OUT qos_shaper_t *shaper,
			OUT int32_t	weight,
			OUT int32_t	priority,
			IN uint32_t flags)
{
	int32_t iret;
	x_ipc_msg_t xmsg;
	x_QIPC_pSetReq *pxpSetReq;
        struct x_qoscfg_t xcfg;
	memset_s(&xcfg, sizeof(xcfg), 0);

	/* Fill msg content */
	pxpSetReq=(x_QIPC_pSetReq *)xmsg.acmsg;
	if(strncpy_s(pxpSetReq->acIfName, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
                LOGF_LOG_ERROR("Error copying ifname string\n");
                return LTQ_FAIL;
        }
	pxpSetReq->uiFlags=flags;
	memcpy_s(&pxpSetReq->xshaper, sizeof(qos_shaper_t), shaper, sizeof(qos_shaper_t));
	pxpSetReq->iWeight=weight;
	pxpSetReq->iPriority=priority;
	/* Fill the header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
  xmsg.xhdr.nMsgType = QIPC_PSET_REQ;
	xmsg.xhdr.unmsgsize=sizeof(x_QIPC_pSetReq);

        fapi_qos_cfg_get(&xcfg.qoscfg, xcfg.uiflags);
        if(xcfg.qoscfg.ena != QOSAL_ENABLE)
        {
		LOGF_LOG_CRITICAL(QOS_NOT_ENA);
                return LTQ_FAIL;
        }

	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* TODO: Interpret the response and handle */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

        if(xmsg.xhdr.respCode != LTQ_SUCCESS)
        {
                print_error_message(xmsg.xhdr.respCode);
        }

	return xmsg.xhdr.respCode;
}

/* =============================================================================
 *  Function Name : fapi_qos_port_config_get
 *  Description   : FAPI to get port QoS characteristics like rate shaper, and/or
		    weight/priority if this port is cascaded into another scheduler.
 * ============================================================================*/
int32_t
fapi_qos_port_config_get(IN char *ifname,
		IN qos_shaper_t *shaper,
		IN int32_t *weight,
		IN int32_t	*priority,
		IN uint32_t flags)
{
	int32_t iret;
	x_ipc_msg_t xmsg;
  x_QIPC_pGetorUpdateReq *pxpUpdate;

	UNUSED(shaper);
	UNUSED(weight);
	UNUSED(priority);
	/* Fill the content */
	pxpUpdate=(x_QIPC_pGetorUpdateReq*)xmsg.acmsg;
	if(strncpy_s(pxpUpdate->acIfName, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
                LOGF_LOG_ERROR("Error copying ifname string\n");
                return LTQ_FAIL;
        }
	pxpUpdate->uiFlags=flags;
	/* Fill the header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
  xmsg.xhdr.nMsgType = QIPC_PGET_REQ;
	xmsg.xhdr.unmsgsize=sizeof(x_QIPC_pGetorUpdateReq);
	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
  /*TODO:Extract the required data to passed variables and return */
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_port_update
 *  Description   : FAPI to handle notification on port add or delete in the system from higher
		    layers/system management. This allows qos_al to add/del queues from template
		    files to system.
 * ============================================================================*/
int32_t
fapi_qos_port_update(IN char *ifname,
		IN uint32_t flags)
{
	int32_t iret;
	x_ipc_msg_t xmsg;
	x_QIPC_pGetorUpdateReq *pxpUpdate;
	/* Fill the content */
	pxpUpdate=(x_QIPC_pGetorUpdateReq*)xmsg.acmsg;
	if(strncpy_s(pxpUpdate->acIfName, MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
                LOGF_LOG_ERROR("Error copying ifname string\n");
                return LTQ_FAIL;
        }
	pxpUpdate->uiFlags=flags;
	/* Fill the header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
	xmsg.xhdr.nMsgType = QIPC_PUPDATE_REQ;
	xmsg.xhdr.unmsgsize=sizeof(x_QIPC_pGetorUpdateReq);
	iret=ipc_send_request(&xmsg);
	if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
	/* TODO: Interpret and handle the response */ 

        if(xmsg.xhdr.respCode != LTQ_SUCCESS)
        {
                print_error_message(xmsg.xhdr.respCode);
        }

	return xmsg.xhdr.respCode;
}

/* =============================================================================
 *  Function Name : fapi_qos_if_base_set
 *  Description   : FAPI to set (add/delete) QoS AL Interface base inetrface like ptm0/eth1/nas0 
                    for pppoe inetrface
 * ============================================================================*/
int32_t
fapi_qos_if_base_set(
	IN char		*ifname,
	IN char		*base_ifname,
	IN char		*logical_base,
	IN uint32_t 	flags)
{
	int32_t ret = LTQ_SUCCESS;
	x_ipc_msg_t xmsg;
	struct x_ifcfg_t *pxifcfgReq;
        struct x_qoscfg_t xcfg;
	memset_s(&xcfg, sizeof(xcfg), 0);
	
	if(sizeof(struct x_ifcfg_t)>IPC_MAX_MSG_SIZE){
		printf("Message size too big\n");
		return LTQ_FAIL;
	}
	if(ifname == NULL || strcmp(ifname, "") == 0) {
		LOGF_LOG_ERROR("Interface name is mandatory for interface-category mapping.\n");
		return LTQ_FAIL;
	}
	/* Populate Msg Body
	   Fix interface count as 1 and copy given interface to ifnames array
	   ES FAPI is expected to read the interface count and read ifnames array
	*/
	pxifcfgReq=(struct x_ifcfg_t *)xmsg.acmsg;
	if(strncpy_s(pxifcfgReq->ifnames[0], MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying ifname string\n");
            return LTQ_FAIL;
        }
	if(strncpy_s(pxifcfgReq->base_ifnames[0], MAX_IF_NAME_LEN, base_ifname, strnlen_s(base_ifname, MAX_IF_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying base_ifname string\n");
		return LTQ_FAIL;
	}
	if(strncpy_s(pxifcfgReq->logical_base[0], MAX_IF_NAME_LEN, logical_base, strnlen_s(logical_base, MAX_IF_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying logical_base string\n");
                return LTQ_FAIL;
	}
	pxifcfgReq->num_entries = 1;
	pxifcfgReq->uiFlags=flags;
	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
	xmsg.xhdr.nMsgType = QIPC_IF_BASE_SET;
	xmsg.xhdr.unmsgsize=sizeof(struct x_ifcfg_t);
	ret=ipc_send_request(&xmsg);
	if(ret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

	if(xmsg.xhdr.respCode != LTQ_SUCCESS)
	{
		print_error_message(xmsg.xhdr.respCode);
	}

	return xmsg.xhdr.respCode;
}
/* =============================================================================
 *  Function Name : fapi_qos_if_abs_set
 *  Description   : FAPI to set QoS AL Interface Group Abstraction like LAN, WAN, WLAN that
		    involves adding a new interface to an interface group or deleting.
 * ============================================================================*/
int32_t
fapi_qos_if_abs_set(
	IN iftype_t	ifgroup,
	IN char		*ifname,
	IN uint32_t 	flags)
{
//	int i=0;
	int32_t ret = LTQ_SUCCESS;
	x_ipc_msg_t xmsg;
	struct x_ifcfg_t *pxifcfgReq;
        struct x_qoscfg_t xcfg;
	memset_s(&xcfg, sizeof(xcfg), 0);
	
	if(sizeof(struct x_ifcfg_t)>IPC_MAX_MSG_SIZE){
		printf("Message size too big\n");
		return LTQ_FAIL;
	}
	if(ifname == NULL || strcmp(ifname, "") == 0) {
		LOGF_LOG_ERROR("Interface name is mandatory for interface-category mapping.\n");
		return LTQ_FAIL;
	}
	/* Populate Msg Body
	   Fix interface count as 1 and copy given interface to ifnames array
	   ES FAPI is expected to read the interface count and read ifnames array
	*/
	pxifcfgReq=(struct x_ifcfg_t *)xmsg.acmsg;
	pxifcfgReq->ifgroup = ifgroup;
	if(strncpy_s(pxifcfgReq->ifnames[0], MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying ifname string\n");
            return LTQ_FAIL;
        }
	pxifcfgReq->num_entries = 1;
	pxifcfgReq->uiFlags=flags;
	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
	xmsg.xhdr.nMsgType = QIPC_IF_ABS_SET;
	xmsg.xhdr.unmsgsize=sizeof(struct x_ifcfg_t);

#if 0
        fapi_qos_cfg_get(&xcfg.qoscfg, xcfg.uiflags);
        if(xcfg.qoscfg.ena != QOSAL_ENABLE)
        {
		LOGF_LOG_ERROR(QOS_NOT_ENA);
                return LTQ_FAIL;
        }
#endif

	ret=ipc_send_request(&xmsg);
	if(ret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

	if(xmsg.xhdr.respCode != LTQ_SUCCESS)
	{
		print_error_message(xmsg.xhdr.respCode);
	}

	if(ret != LTQ_SUCCESS)
		return ret;
	else
		return xmsg.xhdr.respCode;
}

/* =============================================================================
 *  Function Name : fapi_qos_if_abs_get
 *  Description   : FAPI to get QoS AL Interface Group Abstraction member interfaces
		    like for LAN, WAN, WLAN.
 * ============================================================================*/
int32_t
fapi_qos_if_abs_get(
	IN iftype_t	ifgroup,
	OUT int32_t	*num_entries,
	OUT char	ifnames[MAX_IF_PER_GROUP][MAX_IFNAME_LEN],
	IN uint32_t 	flags)
{
	int32_t ret = LTQ_SUCCESS;
	x_ipc_msg_t xmsg;
	struct x_ifcfg_t *pxifcfgReq;
	
	/* Populate Msg Body */
	pxifcfgReq=(struct x_ifcfg_t *)xmsg.acmsg;
	pxifcfgReq->ifgroup = ifgroup;
	pxifcfgReq->uiFlags=flags;
	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
	xmsg.xhdr.nMsgType = QIPC_IF_ABS_GET;
	xmsg.xhdr.unmsgsize=sizeof(struct x_ifcfg_t);
	ret=ipc_send_request(&xmsg);
	if(ret < 0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

	/* copy information read from IPC into structure */
	pxifcfgReq=(struct x_ifcfg_t *)xmsg.acmsg;
	*num_entries = pxifcfgReq->num_entries;
	memcpy_s(ifnames, sizeof(pxifcfgReq->ifnames), pxifcfgReq->ifnames, sizeof(pxifcfgReq->ifnames));
	return ret;
}

/* =============================================================================
 *  Function Name : fapi_qos_if_inggrp_set
 *  Description   : FAPI to set QoS ingress group for an interface  
 * ============================================================================*/
int32_t
fapi_qos_if_inggrp_set(
        IN ifinggrp_t ifinggrp,
        IN char *ifname,
        IN uint32_t flags)
{
	int32_t ret = LTQ_SUCCESS;
	x_ipc_msg_t xmsg;
	struct x_ifcfg_t *pxifcfgReq;
	struct x_qoscfg_t xcfg;

	memset_s(&xcfg, sizeof(xcfg), 0);
	
	if(sizeof(struct x_ifcfg_t)>IPC_MAX_MSG_SIZE){
		printf("Message size too big\n");
		return LTQ_FAIL;
	}

	if(ifname == NULL || strcmp(ifname, "") == 0) {
		printf("Interface name is mandatory for interface-ingress group mapping.\n");
		return LTQ_FAIL;
	}

	/* Populate Msg Body
	   Fix interface count as 1 and copy given interface to ifnames array
	   ES FAPI is expected to read the interface count and read ifnames array
	*/
	pxifcfgReq=(struct x_ifcfg_t *)xmsg.acmsg;
	pxifcfgReq->ifinggrp = ifinggrp;
	if(strncpy_s(pxifcfgReq->ifnames[0], MAX_IF_NAME_LEN, ifname, strnlen_s(ifname, MAX_IF_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying ifname string\n");
            return LTQ_FAIL;
        }
	pxifcfgReq->num_entries = 1;
	pxifcfgReq->uiFlags=flags;
	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
	xmsg.xhdr.nMsgType = QIPC_IF_INGGRP_SET;
	xmsg.xhdr.unmsgsize=sizeof(struct x_ifcfg_t);

	ret=ipc_send_request(&xmsg);
	if(ret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

	if(xmsg.xhdr.respCode != LTQ_SUCCESS)
	{
		print_error_message(xmsg.xhdr.respCode);
	}

	if(ret != LTQ_SUCCESS)
		return ret;
	else
		return xmsg.xhdr.respCode;
}


/* =============================================================================
 *  Function Name : fapi_qos_if_inggrp_get
 *  Description   : FAPI to get QoS ingress group for an interface  
 * ============================================================================*/

int32_t
fapi_qos_if_inggrp_get(
        IN ifinggrp_t ifinggrp,
        OUT int32_t *num_entries,
        OUT char ifnames[MAX_IF_PER_INGRESS_GROUP][MAX_IFNAME_LEN],
        IN uint32_t flags)
{
	int32_t ret = LTQ_SUCCESS;
	x_ipc_msg_t xmsg;
	struct x_ifcfg_t *pxifcfgReq;

	/* Populate Msg Body */
	pxifcfgReq=(struct x_ifcfg_t *)xmsg.acmsg;
	pxifcfgReq->ifinggrp = ifinggrp;
	pxifcfgReq->uiFlags=flags;
	/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
	xmsg.xhdr.nMsgType = QIPC_IF_INGGRP_GET;
	xmsg.xhdr.unmsgsize=sizeof(struct x_ifcfg_t);
	ret=ipc_send_request(&xmsg);
	if(ret < 0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
		return LTQ_FAIL;
	}
	/* Response if of Generic type so typecast it and dump */
	LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

	/* copy information read from IPC into structure */
	pxifcfgReq=(struct x_ifcfg_t *)xmsg.acmsg;
	*num_entries = pxifcfgReq->num_entries;
	memcpy_s(ifnames, sizeof(pxifcfgReq->ifnames), pxifcfgReq->ifnames, sizeof(pxifcfgReq->ifnames));
	
	return ret;
}

/* =============================================================================
 *  Function Name : fapi_qos_classifier_set
 *  Description   : FAPI to set QoS calssifier  
 * ============================================================================*/

int32_t
fapi_qos_classifier_set(
        IN int32_t                 order,
        IN cl_match_filter_t       filter,
        IN cl_action_t             action,
        IN uint32_t                flags ) 
{
	int32_t ret = LTQ_SUCCESS;
	x_ipc_msg_t xmsg;
        qos_class_cfg_t *pxclscfgReq;
        struct x_qoscfg_t xcfg;
	memset_s(&xcfg, sizeof(xcfg), 0);

        if(sizeof(qos_class_cfg_t)>IPC_MAX_MSG_SIZE){
                printf("Message size too big\n");
                return -1;
        }
        /* Populate Msg Body
        */
        pxclscfgReq=(qos_class_cfg_t *)xmsg.acmsg;
        pxclscfgReq->filter = filter;
        pxclscfgReq->action = action;
        pxclscfgReq->filter.flags=flags;
        pxclscfgReq->filter.order=order;
        /*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        xmsg.xhdr.nMsgType = QIPC_CLASS_CFG_SET;
        xmsg.xhdr.unmsgsize=sizeof(qos_class_cfg_t);

        fapi_qos_cfg_get(&xcfg.qoscfg, xcfg.uiflags);
        if(xcfg.qoscfg.ena != QOSAL_ENABLE)
        {
		LOGF_LOG_CRITICAL(QOS_NOT_ENA);
                return LTQ_FAIL;
        }

        ret=ipc_send_request(&xmsg);
        if(ret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
                return LTQ_FAIL;
        }
        /* Response if of Generic type so typecast it and dump */
        LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);

        if(xmsg.xhdr.respCode != LTQ_SUCCESS)
        {
                print_error_message(xmsg.xhdr.respCode);
        }

        return xmsg.xhdr.respCode;
}


/* =============================================================================
 *  Function Name : fapi_qos_classifier_get
 *  Description   : FAPI to get QoS calssifier  
 * ============================================================================*/

int32_t
fapi_qos_classifier_get(
        IN uint32_t                order,
        OUT int32_t                 *num_entries,
        OUT cl_match_filter_t       **filter,
        OUT cl_action_t             **action,
        IN uint32_t                flags )
{
	int32_t i = 0;
	int32_t iret = LTQ_SUCCESS;
	int32_t id;
	char *data=NULL;
	x_ipc_msg_t xmsg;
	x_QIPC_clscfgReq *pxclscfgReq; 
/*  Populate Msg Header */
	if(strncpy_s((char *)xmsg.xhdr.aucfrom, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        if(strncpy_s((char *)xmsg.xhdr.aucto, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
            LOGF_LOG_ERROR("Error copying string\n");
            return LTQ_FAIL;
        }
        pxclscfgReq=(x_QIPC_clscfgReq *)xmsg.acmsg;
        pxclscfgReq->uiFlags = flags;
        pxclscfgReq->clscfg.filter.order = order;
        xmsg.xhdr.nMsgType = QIPC_CLASS_CFG_GET;
        xmsg.xhdr.unmsgsize = sizeof(x_QIPC_clscfgReq);
        iret=ipc_send_request(&xmsg);
        if(iret<0){
		LOGF_LOG_CRITICAL(QOSD_NOT_RUN_STR);
                return -1;
        }
        pxclscfgReq=(x_QIPC_clscfgReq*)xmsg.acmsg;
        /* Response if of Generic type so typecast it and dump */
        LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
				
	/* Read response from shared memory */
	if (xmsg.xhdr.shm_key == 0 || ipc_get_memory(&xmsg.xhdr.shm_key, &id, &data, 0, 0) != LTQ_SUCCESS) {
		printf("Could not read memory...\n");
		return LTQ_FAIL;
	}
	*num_entries = *((uint32_t *)data);
	qos_class_cfg_t *classifiers = (qos_class_cfg_t*) (data + sizeof(int32_t));

	*filter = (cl_match_filter_t*) os_calloc(1, sizeof(cl_match_filter_t) * (*num_entries));
	*action = (cl_action_t*) os_calloc(1, sizeof(cl_action_t) * (*num_entries));
	for (i=0; i< *num_entries; i++) {
		memcpy_s(*filter + i, sizeof(cl_match_filter_t), &(classifiers+i)->filter, sizeof(cl_match_filter_t));
		memcpy_s(*action + i, sizeof(cl_action_t), &(classifiers+i)->action, sizeof(cl_action_t));
	}
	ipc_delete_memory(id, &data, 1);

  return iret;
}
