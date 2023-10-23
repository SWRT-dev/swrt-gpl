/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <getopt.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <pwd.h>
#include <grp.h>

#include "qosal_inc.h"
#include "qosal_ipc.h"
#include "qosal_ipc_msg.h"

#include "safe_str_lib.h"
#include "safe_mem_lib.h"

const char gFapiVer[10] = "1.2.0";
extern uint16_t LOGTYPE, LOGLEVEL;

#define IPC_DIR_PATH	"/tmp/qosipc"
#define IPC_FILE_PATH	"/tmp/qosipc/MsgTo"

struct ucred {
	int pid;
	int uid;
	int gid;
};
typedef struct ucred cred;

typedef struct
{
	char *user_name;
	char *group_name;
}cli_user;

cli_user valid_users[] = {
	{"root", "root"},
	{"qosd", "qosd"},
	{"voipd", "voipd"},
	{"sscript", "voipd"}
};

static int32_t validate_creds(cred cr);
static int32_t validate_client(cred cr, const char *user_name, const char *group_name);
/* =============================================================================
 *  Function Name : qosd_process_msg
 *  Description   : Parses message read from IPC and delegates to
		    appropriate low-FAPI for processing. Based on response of
		    low-FAPI message (containing response of low-FAPI and required
		    information in case of GET operation) is sent back to
		    high-FAPI over IPC.
 * ============================================================================*/
static int32_t
qosd_process_msg(void *pvhandle,x_ipc_msg_t *pxMsg)
{
	int32_t iret=-1;
	char *data = NULL;
	int32_t id;
	x_ipc_msg_t xRsp;
	/*Fill rsp header*/
	if(strncpy_s((char *)xRsp.xhdr.aucfrom, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying string\n");
    		return LTQ_FAIL;
	}
	if(strncpy_s((char *)xRsp.xhdr.aucto, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
                LOGF_LOG_ERROR("Error copying string\n");
                return LTQ_FAIL;
        }
	switch(pxMsg->xhdr.nMsgType){
		case QIPC_STATUS_SET:
		{
			x_QIPC_qoscfgReq *pxqoscfgReq;
			x_QIPC_GenRsp *pxGenRsp=(x_QIPC_GenRsp*)xRsp.acmsg;
			pxqoscfgReq=(x_QIPC_qoscfgReq *)pxMsg->acmsg;
			pxGenRsp->iRetVal=qosd_cfg_set(&pxqoscfgReq->qoscfg);
			/* TODO: Send reply */
			if(pxGenRsp->iRetVal != LTQ_SUCCESS) {
			}
			else {
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_STATUS_GET:
		{
			x_QIPC_qoscfgReq *pxqoscfgReq = NULL;
			x_QIPC_qosCfgRsp *pxQoSRsp=(x_QIPC_qosCfgRsp *)xRsp.acmsg;
			pxqoscfgReq=(x_QIPC_qoscfgReq *)pxMsg->acmsg;
			pxQoSRsp->iRetVal=qosd_cfg_get(&pxqoscfgReq->qoscfg,pxqoscfgReq->uiFlags);
			/* TODO: Send reply */
			if(pxQoSRsp->iRetVal != LTQ_SUCCESS) {
			}
			else {
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
				memcpy_s(&(pxQoSRsp->qosCfg), sizeof(pxqoscfgReq->qoscfg), &(pxqoscfgReq->qoscfg), sizeof(pxqoscfgReq->qoscfg));
			}
			
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_QSET_REQ:
		{
			x_QIPC_qSetReq *pxqSetReq;
			x_QIPC_GenRsp *pxGenRsp=(x_QIPC_GenRsp*)xRsp.acmsg;
			pxqSetReq=(x_QIPC_qSetReq *)pxMsg->acmsg;
			pxGenRsp->iRetVal=qosd_queue_set(pxqSetReq->acIfName,&pxqSetReq->xqCfg,pxqSetReq->uiFlags);
			/* TODO: Send reply */
			if(pxGenRsp->iRetVal != LTQ_SUCCESS) {
				xRsp.xhdr.respCode = pxGenRsp->iRetVal;
			}
			else {
				xRsp.xhdr.respCode = LTQ_SUCCESS;
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_QGET_REQ:
		{
			x_QIPC_qGetReq *pxqGetReq;
			uint32_t iNumQs;
			qos_queue_cfg_t  *xqCfg = NULL;
			/* TODO: qosd_queue_get doesn't malloc and copy the queues (has to be updated),
			   therefore pointer to statically allocated space is passed to it */
			qos_queue_cfg_t  queues[MAX_QUEUE_COUNT];
			char *pcqName = NULL;
			xqCfg = queues;
			pxqGetReq=(x_QIPC_qGetReq *)pxMsg->acmsg;
			LOGF_LOG_DEBUG("q name pased to fapi is %s\n",pxqGetReq->acqName);
			if (strcmp(pxqGetReq->acqName, "all") != 0)
				pcqName = pxqGetReq->acqName;
			memset_s(queues, sizeof(queues), 0);
			iret=qosd_queue_get(pxqGetReq->acIfName,pcqName,
								&iNumQs,&xqCfg,pxqGetReq->uiFlags);
			xRsp.xhdr.nMsgType = QIPC_QGET_RSP;
			xRsp.xhdr.unmsgsize=sizeof(x_QIPC_qGetRsp);
			/* Copy queuess in Resp struct */
			if (ipc_get_memory(&xRsp.xhdr.shm_key, &id, &data, sizeof(uint32_t) + sizeof(qos_queue_cfg_t)*iNumQs, 1) != LTQ_SUCCESS) {
				LOGF_LOG_CRITICAL("Could not get memory \n");
				ipc_send_reply(pvhandle,&xRsp); /* can be put in end for all*/
				return LTQ_FAIL;
			}
			*((uint32_t *)data) = iNumQs;
			memcpy_s(data+sizeof(uint32_t), iNumQs * sizeof(qos_queue_cfg_t), queues, iNumQs * sizeof(qos_queue_cfg_t));
			ipc_delete_memory(id, &data, 0);
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_QSTATSGET_REQ:
		{
			x_QIPC_qGetReq *pxqGetReq;
			uint32_t iNumQs;
			qos_queue_stats_t  *pxqStats = NULL;
			char *pcqName = NULL;
			pxqGetReq=(x_QIPC_qGetReq *)pxMsg->acmsg;
			LOGF_LOG_DEBUG("q name pased to fapi is %s\n",pxqGetReq->acqName);
			if (strcmp(pxqGetReq->acqName, "all") != 0)
				pcqName = pxqGetReq->acqName;
			iret=qosd_queue_stats_get(pxqGetReq->acIfName,pcqName,
								(int32_t *)&iNumQs,&pxqStats,pxqGetReq->uiFlags);
			/* TODO: Send reply */
			if(iret != LTQ_SUCCESS) {
				LOGF_LOG_ERROR("Queue stats get failed for interface [%s] and queue name [%s]..\n",
						(pxqGetReq->acIfName != NULL)?pxqGetReq->acIfName:"", (pcqName != NULL)?pcqName:"");
			}
			else {
				xRsp.xhdr.nMsgType = QIPC_QGET_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_qGetRsp);
				/* Copy qstats in Resp struct */
				if (ipc_get_memory(&xRsp.xhdr.shm_key, &id, &data, sizeof(uint32_t) + sizeof(qos_queue_stats_t)*iNumQs, 1) != LTQ_SUCCESS) {
					LOGF_LOG_CRITICAL("Could not get memory \n");
					ipc_send_reply(pvhandle,&xRsp); /* can be put in end for all*/
					os_free(pxqStats);
					return LTQ_FAIL;
				}
				*((uint32_t *)data) = iNumQs;
				if(pxqStats != NULL) {
					memcpy_s(data+sizeof(uint32_t), iNumQs * sizeof(qos_queue_stats_t), pxqStats, iNumQs * sizeof(qos_queue_stats_t));
				}
				ipc_delete_memory(id, &data, 0);
			}
			if(pxqStats != NULL) {
				os_free(pxqStats);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;

		case QIPC_PSET_REQ:
		{
			x_QIPC_pSetReq *pxpSetReq;
			x_QIPC_GenRsp xGenRsp;
			pxpSetReq=(x_QIPC_pSetReq *)pxMsg->acmsg;
			xGenRsp.iRetVal=qosd_port_config_set(pxpSetReq->acIfName,&pxpSetReq->xshaper,
							pxpSetReq->iWeight,pxpSetReq->iPriority,pxpSetReq->uiFlags);
			/* TODO: Send reply */
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
				xRsp.xhdr.respCode = xGenRsp.iRetVal;
			}
			else {
				xRsp.xhdr.respCode = LTQ_SUCCESS;
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP; //aarif
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_PGET_REQ:
		{
			x_QIPC_pGetorUpdateReq *pxpUpdate;
			x_QIPC_pGetRsp *pxpGetRsp = NULL;
			pxpUpdate=(x_QIPC_pGetorUpdateReq*)pxMsg->acmsg;
			pxpGetRsp=(x_QIPC_pGetRsp*)xRsp.acmsg;
			pxpGetRsp->xGenRsp.iRetVal=qosd_port_config_get(pxpUpdate->acIfName,
					&pxpGetRsp->xshaper,&pxpGetRsp->iWeight,&pxpGetRsp->iPriority,pxpUpdate->uiFlags);
			if(pxpGetRsp->xGenRsp.iRetVal != LTQ_SUCCESS) {
				xRsp.xhdr.respCode = pxpGetRsp->xGenRsp.iRetVal;
			}
			else {
				xRsp.xhdr.respCode = LTQ_SUCCESS;
				xRsp.xhdr.nMsgType = QIPC_PGET_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_pGetRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_PUPDATE_REQ:
		{
			x_QIPC_pGetorUpdateReq *pxpUpdate;
			x_QIPC_GenRsp xGenRsp;
			pxpUpdate=(x_QIPC_pGetorUpdateReq*)pxMsg->acmsg;
			xGenRsp.iRetVal=qosd_port_update(pxpUpdate->acIfName,pxpUpdate->uiFlags);
			/* TODO: Send reply */
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
				xRsp.xhdr.respCode = xGenRsp.iRetVal;
			}
			else {
				xRsp.xhdr.respCode = LTQ_SUCCESS;
				xRsp.xhdr.nMsgType = QIPC_PUPDATE_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_IF_ABS_SET:
		{
			/* add/delete interface specified by ifnames[0] in specified interface group ifgroup */
			struct x_ifcfg_t *pxifcfg;
			x_QIPC_GenRsp xGenRsp;
			pxifcfg = (struct x_ifcfg_t *)pxMsg->acmsg;
			xGenRsp.iRetVal = qosd_if_abs_set(pxifcfg->ifgroup, pxifcfg->ifnames[0], pxifcfg->uiFlags);
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
				xRsp.xhdr.respCode = xGenRsp.iRetVal;
			}
			else {
				xRsp.xhdr.respCode = LTQ_SUCCESS;
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_IF_BASE_SET:
		{
			/* add/delete interface specified by ifnames[0] in specified interface group ifgroup */
			struct x_ifcfg_t *pxifcfg;
			pxifcfg = (struct x_ifcfg_t *)pxMsg->acmsg;
			xRsp.xhdr.respCode = qosd_if_base_set(pxifcfg->ifnames[0], pxifcfg->base_ifnames[0], pxifcfg->logical_base[0], pxifcfg->uiFlags);
			xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
			xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);

			ipc_send_reply(pvhandle,&xRsp);
		}
		break;

		case QIPC_IF_ABS_GET:
		{
			/* retrieve interface names configured under specified ifgroup */
			struct x_ifcfg_t *pxifcfg;
			x_QIPC_GenRsp xGenRsp;
			pxifcfg = (struct x_ifcfg_t *)pxMsg->acmsg;
			xGenRsp.iRetVal = qosd_if_abs_get(pxifcfg->ifgroup, &pxifcfg->num_entries, pxifcfg->ifnames, pxifcfg->uiFlags);
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
			}
			else {
				xRsp.xhdr.nMsgType = QIPC_IF_ABS_GET;
				xRsp.xhdr.unmsgsize=sizeof(struct x_ifcfg_t);
				memcpy_s(xRsp.acmsg, sizeof(struct x_ifcfg_t), pxMsg->acmsg, sizeof(struct x_ifcfg_t));
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_IF_INGGRP_SET:
		{
			/* add/delete interface specified by ifnames[0] in specified interface group ifgroup */
			struct x_ifcfg_t *pxifcfg;
			x_QIPC_GenRsp xGenRsp;
			pxifcfg = (struct x_ifcfg_t *)pxMsg->acmsg;
			xGenRsp.iRetVal = qosd_if_inggrp_set(pxifcfg->ifinggrp, pxifcfg->ifnames[0], pxifcfg->uiFlags);
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
				xRsp.xhdr.respCode = xGenRsp.iRetVal;
			}
			else {
				xRsp.xhdr.respCode = LTQ_SUCCESS;
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_IF_INGGRP_GET:
		{
			/* retrieve interface names configured under specified ifgroup */
			struct x_ifcfg_t *pxifcfg;
			x_QIPC_GenRsp xGenRsp;
			pxifcfg = (struct x_ifcfg_t *)pxMsg->acmsg;
			xGenRsp.iRetVal = qosd_if_inggrp_get(pxifcfg->ifinggrp, &pxifcfg->num_entries, pxifcfg->ifnames, pxifcfg->uiFlags);
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
			}
			else {
				xRsp.xhdr.nMsgType = QIPC_IF_INGGRP_GET;
				xRsp.xhdr.unmsgsize=sizeof(struct x_ifcfg_t);
				memcpy_s(xRsp.acmsg, sizeof(struct x_ifcfg_t), pxMsg->acmsg, sizeof(struct x_ifcfg_t));
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		case QIPC_CLASS_CFG_SET:
		{
			/* add/delete classifier entries */
			qos_class_cfg_t *pxclscfg;
			x_QIPC_GenRsp xGenRsp;
			pxclscfg = (qos_class_cfg_t*)pxMsg->acmsg;
			/* FIXME: the class_cfg->flags value is lost when the event
				QIPC_CLASS_CFG_SET reaches qosd, setting it again here
				by putting additional flag mptcp_flag */
			if (pxclscfg->filter.mptcp_flag == 1) {
				pxclscfg->filter.flags |= CL_ACTION_CL_MPTCP;
			}

			xGenRsp.iRetVal = qosd_classifier_set(pxclscfg->filter.order, pxclscfg, pxclscfg->filter.flags);
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
				xRsp.xhdr.respCode = xGenRsp.iRetVal;
			}
			else {
				xRsp.xhdr.respCode = LTQ_SUCCESS;
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;
		
		case QIPC_CLASS_CFG_GET:
		{
			/* Get classifier entries */
			qos_class_cfg_t *pxclscfg;
			qos_class_cfg_t *class_cfg = NULL;
			int32_t num_entries = 0;
			x_QIPC_GenRsp xGenRsp;
			pxclscfg = (qos_class_cfg_t*)pxMsg->acmsg;
			xGenRsp.iRetVal = qosd_classifier_get(pxclscfg->filter.order, &num_entries, &class_cfg, pxclscfg->filter.flags); 
			if(xGenRsp.iRetVal != LTQ_SUCCESS) {
			}
			else {
				xRsp.xhdr.nMsgType = QIPC_GEN_RSP;
				xRsp.xhdr.unmsgsize=sizeof(x_QIPC_GenRsp);
				LOGF_LOG_DEBUG("Received %d classifers\n",num_entries);
				/* Copy classifiers in Resp struct */
				if (ipc_get_memory(&xRsp.xhdr.shm_key, &id, &data, sizeof(int32_t) + sizeof(qos_class_cfg_t)*num_entries, 1) != LTQ_SUCCESS) {
					LOGF_LOG_CRITICAL("Could not get memory \n");
					ipc_send_reply(pvhandle,&xRsp); /* can be put in end for all*/
					os_free(class_cfg);
					return LTQ_FAIL;
				}
				*((int32_t *)data) = num_entries;
				if(class_cfg != NULL) {
					memcpy_s(data+sizeof(int32_t), sizeof(qos_class_cfg_t)*num_entries, class_cfg, sizeof(qos_class_cfg_t)*num_entries);
					os_free(class_cfg);
				}
				ipc_delete_memory(id, &data, 0);
			}
			ipc_send_reply(pvhandle,&xRsp);
		}
		break;

		
		default:
			LOGF_LOG_ERROR("Invalid msg type please check\n");
		break;
	}
	return LTQ_SUCCESS;
}

static void send_error_reply(void *pvhandle)
{
	ENTRY
	x_ipc_msg_t xRsp;
	x_QIPC_GenRsp *pxGenRsp = (x_QIPC_GenRsp*)xRsp.acmsg;
	if (strncpy_s((char *)xRsp.xhdr.aucfrom, MAX_QOS_NAME_LEN, "qosd", strnlen_s("qosd", MAX_QOS_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying string\n");
		return;
	}
	if (strncpy_s((char *)xRsp.xhdr.aucto, MAX_QOS_NAME_LEN, "fapi", strnlen_s("fapi", MAX_QOS_NAME_LEN)) != EOK) {
		LOGF_LOG_ERROR("Error copying string\n");
		return;
	}
	pxGenRsp->iRetVal = LTQ_FAIL;
	ipc_send_reply(pvhandle, &xRsp);
	EXIT
}

/* =============================================================================
 *  Function Name : qosd_daemon
 *  Description   : Rreates IPC listener for receiving messages from
		    high-FAPI and then runs forever to process each received message.
 * ============================================================================*/
static int32_t
qosd_daemon(void)
{
	void *pvhandle;
	int32_t iret;
	x_ipc_msg_t xmsg;

	pvhandle=ipc_create_listener("qosd");
	if(pvhandle==NULL){
		LOGF_LOG_ERROR("Unable to create handle\n");
		return LTQ_FAIL;
	}
	while(1){
		LOGF_LOG_DEBUG("Blocking on rcv\n");
		iret=ipc_recv(pvhandle,&xmsg);
		if (iret == IPC_INVALID_CALLER) {
			LOGF_LOG_ERROR("Access Denied!\n");
			send_error_reply(pvhandle);
			continue;
		} else if(iret<0){
			LOGF_LOG_ERROR("Receive failed\n");
			close(((x_ipc_handle *)pvhandle)->ifd);
			close(((x_ipc_handle *)pvhandle)->iconnfd);
			os_free(pvhandle);
			pvhandle = NULL;
			return LTQ_FAIL;
		}
		LOGF_LOG_DEBUG("From[%s]To[%s]Len[%d]\n",xmsg.xhdr.aucfrom,xmsg.xhdr.aucto,xmsg.xhdr.unmsgsize);
		qosd_process_msg(pvhandle,&xmsg);
	}
  return LTQ_SUCCESS;
}

static void qosd_sig_handler(int32_t nSig)
{
	UNUSED(nSig);
	if (nSig == SIGUSR1) {
		LOGF_LOG_DEBUG("Reload configuration\n");
		qosd_set_qoscfg();
		return;
	}
	/* remove PID file. */
	remove(QOSD_PID_FILE);

	/* exit. */
	exit(0);
}

/* =============================================================================
 *  Function Name : main
 *  Description   : entry point of QoSd
 * ============================================================================*/
int
main(void)
{
	// es_init();
	/* create PID file. */
	FILE *fp = fopen(QOSD_PID_FILE, "wx");

	if (fp != NULL) {
		fprintf(fp, "%ld\n", (long) getpid());
		fclose(fp);
	}

	/* catch the signal kill and stop the process */
	signal(SIGTERM, qosd_sig_handler);
	signal(SIGINT, qosd_sig_handler);
	signal(SIGUSR1, qosd_sig_handler);

	qosd_set_qoscfg();
	LOGF_LOG_DEBUG("Configuration from file done!\n");

	qosd_daemon(); /* run forever */

	return LTQ_SUCCESS;
}

int32_t
ipc_send_reply(void *pvhandle,
					x_ipc_msg_t *pxmsg)
{
	x_ipc_handle *pxhandle=pvhandle;

	if(pvhandle==NULL){
		LOGF_LOG_ERROR("Create a Event listener first\n");
		return LTQ_FAIL;
	}
	if (write(pxhandle->iconnfd, (char *)pxmsg, pxmsg->xhdr.unmsgsize + IPC_HDR_SIZE) < 0)
	{
		/* Error Writing Message to the FIFO */
		/* Add Debugs if required */
			LOGF_LOG_ERROR("write failed %d\n",pxhandle->iconnfd);
		return IPC_FAIL;
	}
	 close(pxhandle->iconnfd);
	return IPC_SUCCESS;
}

int32_t
ipc_recv(void *pvhandle,
				 x_ipc_msg_t *pxmsg)
{
	int16_t nbytes = 0;
	x_ipc_handle *pxhandle=pvhandle;
	cred cr = {0};
	socklen_t len = sizeof(cr);
	int32_t nRet = LTQ_FAIL;

	if(pvhandle==NULL){
		LOGF_LOG_DEBUG("Create a Event listener first\n");
		return LTQ_FAIL;
	}

	 if ( ( pxhandle->iconnfd = accept(pxhandle->ifd, NULL, NULL)) == -1) {
		perror("accept error");
		return LTQ_FAIL;
	 }

	if (getsockopt(pxhandle->iconnfd, SOL_SOCKET, SO_PEERCRED, &cr, &len) < 0) {
		LOGF_LOG_ERROR("peercred failed\n");
		return LTQ_FAIL;
	}
	LOGF_LOG_INFO("IPC peer credentials: UID = %d : GID = %d : PID = %d \n",cr.uid, cr.gid, cr.pid);

	/* Validate the client credentials */
	nRet = validate_creds(cr);
	if (nRet != LTQ_SUCCESS) {
		perror("Access Denied!\n");
		return IPC_INVALID_CALLER;
	}

	if ((nbytes = read(pxhandle->iconnfd, (char *) &pxmsg->xhdr,
										sizeof(x_ipc_msghdr_t))) < 0)
	{
		/* Error Reading Header */
		/* Add Debugs if required */
		return IPC_FAIL;
	}
	if (nbytes != sizeof(x_ipc_msghdr_t) || pxmsg->xhdr.unmsgsize > IPC_MAX_MSG_SIZE || pxmsg->xhdr.unmsgsize < 1)
	{
		return IPC_FAIL;
	}
	/* If non-root user */
	if (cr.uid != 0 || cr.gid != 0) {
		LOGF_LOG_DEBUG("Caller is non-root and Msgtype:[%d]\n", pxmsg->xhdr.nMsgType);
		switch(pxmsg->xhdr.nMsgType) {
			case QIPC_STATUS_GET:
			case QIPC_IF_ABS_SET:
			case QIPC_IF_BASE_SET:
			case QIPC_IF_INGGRP_SET:
			case QIPC_CLASS_CFG_SET:
				LOGF_LOG_INFO("Operation is allowed!\n");
				break;
			default:
				perror("Access Denied!");
				return IPC_INVALID_CALLER;
		}
	}

	LOGF_LOG_DEBUG("Reading the message\n");
	if ((nbytes = read(pxhandle->iconnfd, pxmsg->acmsg, pxmsg->xhdr.unmsgsize)) < 0)
	{
		/* Error Reading Header */
		/* Add Debugs if required */
		return IPC_FAIL;
	}
	return LTQ_SUCCESS;
}
static int32_t validate_client(cred cr, const char *user_name, const char *group_name) {
	ENTRY
	int32_t nRet = LTQ_FAIL;
	struct passwd *pwd = NULL;
	struct group *grp = NULL;

	if (user_name == NULL) {
		perror("User Name is NULL\n");
		nRet = LTQ_FAIL;
		goto exit_lbl;
	}

	if (group_name == NULL) {
		perror("Group Name is NULL\n");
		nRet = LTQ_FAIL;
		goto exit_lbl;
	}

	pwd = getpwnam(user_name);
	if (pwd == NULL) {
		perror("Failed to get uid details\n");
		nRet = LTQ_FAIL;
		goto exit_lbl;
	}

	grp = getgrnam(group_name);
	if (grp == NULL) {
		perror("Failed to get gid details\n");
		nRet = LTQ_FAIL;
		goto exit_lbl;
	}

	LOGF_LOG_DEBUG("Caller uid:[%d] %s-uid:[%d]\n",
			(uid_t)cr.uid, user_name, (uid_t)pwd->pw_uid);
	LOGF_LOG_DEBUG("Caller gid:[%d] %s-gid:[%d]\n",
			(gid_t)cr.gid, group_name, (gid_t)grp->gr_gid);

	if ((uid_t)cr.uid == (uid_t)pwd->pw_uid &&
			(gid_t)cr.gid == (gid_t)grp->gr_gid) {
		LOGF_LOG_INFO("Credentials Passed!\n");
		nRet = LTQ_SUCCESS;
	} else {
		perror("Credentials Failed!\n");
		nRet = LTQ_FAIL;
	}

exit_lbl:
	EXIT
	return nRet;
}

static int32_t validate_creds(cred cr) {
	int32_t nRet = LTQ_FAIL;
	uint32_t i = 0;
	ENTRY

	for (i = 0; i < sizeof(valid_users) / sizeof(cli_user); i++) {
		nRet = validate_client(cr, valid_users[i].user_name,
				valid_users[i].group_name);
		if (nRet == LTQ_SUCCESS) {
			LOGF_LOG_INFO("User:[%s] Group:[%s] is valid\n",
					valid_users[i].user_name, valid_users[i].group_name);
			break;
		}
	}

	EXIT
	return nRet;
}

static int32_t set_permissions(char *path, char *user, char *group, mode_t mode)
{
	ENTRY
	uid_t uid;
	gid_t gid;
	struct passwd *pwd;
	struct group *grp;

	if (path == NULL || user == NULL || group == NULL) {
		LOGF_LOG_ERROR("Params are empty!\n");
		return IPC_FAIL;
	}

	pwd = getpwnam(user);
	if (pwd == NULL) {
		LOGF_LOG_WARNING("Failed to get uid for user:[%s]\n", user);
		/* Setting uid for root in case of failure */
		uid = 0;
	}
	else {
		uid = pwd->pw_uid;
	}

	grp = getgrnam(group);
	if (grp == NULL) {
		LOGF_LOG_WARNING("Failed to get gid for group:[%s]\n", group);
		/* Setting gid for root in case of failure */
		gid = 0;
	}
	else {
		gid = grp->gr_gid;
	}

	/* Set file permissions for non-root user */
	if (uid > 0) {
		if (chown(path, uid, gid) == -1) {
			perror("chown fail: ");
		}

		if (chmod(path, mode) == -1) {
			perror("Setting permission failed: ");
		}
	}
	EXIT
	return IPC_SUCCESS;
}

void* ipc_create_listener(char *pucto)
{
	int32_t iret;
	char acbuf[MAX_NAME_LEN];
	struct sockaddr_un xaddr;
	x_ipc_handle *pxhandle;
	int optval;
	struct stat st = {0};

	if (stat(IPC_DIR_PATH, &st) == -1) {
		if (mkdir(IPC_DIR_PATH, 0700) == -1) {
			perror("Directory creation failed: ");
			return NULL;
		}
	}

	iret = set_permissions(IPC_DIR_PATH, "qosd", "qoscli", (S_IRWXU | S_IRGRP | S_IXGRP));
	if (iret != IPC_SUCCESS)
		return NULL;

	pxhandle = os_calloc(1,sizeof(x_ipc_handle));

	if (pxhandle == NULL) {
		LOGF_LOG_CRITICAL("memory allocation failed for listener\n");
		return NULL;
	}

	iret = snprintf(acbuf, MAX_NAME_LEN, IPC_FILE_PATH"%s", pucto);
	if (iret == MAX_NAME_LEN){
		LOGF_LOG_ERROR("from or to strings are too long consider shortening the name\n");
		os_free(pxhandle);
		return NULL;
	}
	/* Create a unix domain socket */
	pxhandle->ifd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (pxhandle->ifd < 0) {
			LOGF_LOG_ERROR("ipc creation failed - sock\n");
			os_free(pxhandle);
			return NULL;
	}
	memset_s(&xaddr, sizeof(xaddr), 0);
	xaddr.sun_family = AF_UNIX;
	strncpy_s(xaddr.sun_path, MAX_NAME_LEN, acbuf, sizeof(xaddr.sun_path)-1);

	unlink(acbuf);

	if (bind(pxhandle->ifd, (struct sockaddr*)&xaddr, sizeof(xaddr)) == -1) {
		LOGF_LOG_ERROR("ipc creation failed - bind\n");
		if (pxhandle->ifd >= 0)
			close(pxhandle->ifd);
		os_free(pxhandle);
		return NULL;
	}
	optval = 1;
	if (setsockopt(pxhandle->ifd, SOL_SOCKET, SO_PASSCRED, &optval, sizeof(optval)) == -1) {
		LOGF_LOG_ERROR("Error: setsockopt failed [%s]\n", strerror(errno));
		if (pxhandle->ifd >= 0)
			close(pxhandle->ifd);
		os_free(pxhandle);
		return NULL;
	}

	if (listen(pxhandle->ifd, 5) == -1) {
		LOGF_LOG_ERROR("ipc creation failed - listen\n");
		if (pxhandle->ifd >= 0)
			close(pxhandle->ifd);
		os_free(pxhandle);
		return NULL;
	}

	set_permissions(acbuf, "qosd", "qoscli", S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);

	return pxhandle;
}
