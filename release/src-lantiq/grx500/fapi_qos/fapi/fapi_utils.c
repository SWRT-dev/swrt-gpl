/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "commondefs.h"
#include "qosal_ipc.h"
#include "qosal_queue_api.h"
#include "qosal_ipc_msg.h"
#include "qosal_debug.h"

typedef struct{
  char acname[MAX_NAME_LEN];
  int32_t ival;
}x_nameval_t;

/* NOTE: This should match the enum values of mode variables defined in qosal_queue_api.h */
x_nameval_t axschednv[]={
  {"wfq",QOS_SCHED_WFQ},
  {"sp",QOS_SCHED_SP},
  {"\0",0}
};

x_nameval_t axshapernv[]={
  {"cb",QOS_SHAPER_COLOR_BLIND},
  {"trtcm",QOS_SHAPER_TR_TCM},
  {"trtcm_4115",QOS_SHAPER_TR_TCM_RFC4115},
  {"lcb",QOS_SHAPER_LOOSE_COLOR_BLIND},
  {"\0",0}
};

x_nameval_t axdropnv[]={
  {"tail",QOS_DROP_TAIL},
  {"red",QOS_DROP_RED},
  {"wred",QOS_DROP_WRED},
  {"codel",QOS_DROP_CODEL},
  {"\0",0}
};

static char * fapi_qos_getNameFromVal(int32_t ival,x_nameval_t *pxnv);
static int32_t fapi_qos_printShaper(qos_shaper_t *pxshpr);
static int32_t fapi_qos_printDrop(qos_drop_cfg_t *pxdrop);
static int32_t fapi_qos_printQCfg(int32_t inumqs, qos_queue_cfg_t *pxqcfg);
static void fapi_qos_PrintGenRsp(x_QIPC_GenRsp *pxGenRsp);
int32_t fapi_qos_getValFromName(char *pcname,x_nameval_t *pxnv);
void fapi_qos_PrintPGetRsp(x_QIPC_pGetRsp *pxpGetRsp) __attribute__((unused));
void fapi_qos_PrintqSetRsp(x_QIPC_qGetRsp *pxqGetRsp) __attribute__((unused));
void fapi_qos_PrintqSetReq(x_QIPC_qSetReq *pxqSetReq) __attribute__((unused));
void fapi_qos_PrintqGetReq(x_QIPC_qGetReq *pxqGetReq) __attribute__((unused));
void fapi_qos_PrintpSetReq(x_QIPC_pSetReq *pxpSetReq) __attribute__((unused));
void fapi_qos_PrintpGetReq(x_QIPC_pGetorUpdateReq *pxpGetReq) __attribute__((unused));

/* =============================================================================
 *  Function Name : fapi_qos_getValFromName
 *  Description   : Returns index matching specified name in index-name pairs.
 * ============================================================================*/
int32_t
fapi_qos_getValFromName(char *pcname,x_nameval_t *pxnv)
{
	x_nameval_t *pxtmp=pxnv;
	while(pxtmp&&pxtmp->acname){
		// printf("[%s]==[%s]\n",pxtmp->acname,pcname);
		if(strcasecmp(pxtmp->acname,pcname)==0){
			printf("Returning Val: %d\n",pxtmp->ival);
			return pxtmp->ival;
		}
		pxtmp++;
	}
	return LTQ_FAIL;
}

/* =============================================================================
 *  Function Name : fapi_qos_getNameFromVal
 *  Description   : Returns name matching specified index in index-name pairs.
 * ============================================================================*/
static char *
fapi_qos_getNameFromVal(int32_t ival,x_nameval_t *pxnv)
{
	x_nameval_t *pxtmp=pxnv;
	while(pxtmp&&(pxtmp->ival>=0)){
		if(pxtmp->ival==ival){
			LOGF_LOG_DEBUG("Returning Name: %s\n",pxtmp->acname);
			return pxtmp->acname;
		}
		pxtmp++;
	}
	return NULL;
}

/* =============================================================================
 *  Function Name : fapi_qos_printShaper
 *  Description   : Prints specified shaper configuration.
 * ============================================================================*/
static int32_t
fapi_qos_printShaper(qos_shaper_t *pxshpr)
{
	char* pcName = fapi_qos_getNameFromVal(pxshpr->mode,axshapernv);
	printf("\tShaper Config:\n");
	printf("\t\tMode: %s\n",pcName ? pcName : "Nill");
	if(pxshpr->flags & QOS_SHAPER_F_PIR){
		printf("\t\tPeak Info rate: %d byte/sec\n",pxshpr->cir);
		printf("\t\tPeak burst size rate: %d byte\n",pxshpr->cbs);
	}
	if(pxshpr->flags & QOS_SHAPER_F_CIR){
		printf("\t\tcommitted Info rate: %d byte/sec\n",pxshpr->pir);
		printf("\t\tcommitted burst size rate: %d byte\n",pxshpr->pbs);
	}
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_printDrop
 *  Description   : Prints specified drop algorithm configuration.
 * ============================================================================*/
static int32_t
fapi_qos_printDrop(qos_drop_cfg_t *pxdrop)
{
	char* pcName = NULL;
	if(pxdrop->enable != 0){
		pcName = fapi_qos_getNameFromVal(pxdrop->mode,axdropnv);
		printf("\tDrop Config:\n");
		printf("\t\tState: %s\n",pxdrop->enable?"enable":"disable");
		printf("\t\tMode: %s\n",pcName ? pcName : "Nill");
		printf("\t\tWRED Config:\n");
		printf("\t\t\tWeight: %u\n",pxdrop->wred.weight);
		printf("\t\t\tMin Threshold for curve 0 in percent qlen: %u\n",pxdrop->wred.min_th0);
		printf("\t\t\tMax Threshold for curve 0 in percent qlen: %u\n",pxdrop->wred.max_th0);
		printf("\t\t\tMax Drop Probability in percent at max threshold 0 for WRED curve 0: %u\n",pxdrop->wred.max_p0);
		printf("\t\t\tMin Threshold for curve 1 in percent qlen: %u\n",pxdrop->wred.min_th1);
		printf("\t\t\tMax Threshold for curve 1 in percent qlen: %u\n",pxdrop->wred.max_th1);
		printf("\t\t\tMax Drop Probability in percent at max threshold 1 for WRED curve 1: %u\n",pxdrop->wred.max_p1);
	}
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_printQCfg
 *  Description   : Prints specified queues configuration.
 * ============================================================================*/
static int32_t
fapi_qos_printQCfg(int32_t inumqs, qos_queue_cfg_t *pxqcfg)
{
	int32_t i,j;
	char* pcName = NULL;
	for(i=0;i<inumqs;i++){
		printf("Queue %s\n",pxqcfg->name);
		printf("\tState: %s\n",pxqcfg->enable?"enable":"disable");
		printf("\tStatus: %u\n",pxqcfg->status);
		printf("\tOwner: %u\n",pxqcfg->owner);
		printf("\tFlags: %u\n",pxqcfg->flags);
		printf("\tqid: %u\n",pxqcfg->queue_id);
		printf("\tTC Map:\t");
		for(j=0;j<MAX_TC_NUM;j++){
			printf("%d,",pxqcfg->tc_map[j]);
		}
		printf("\n");
		printf("\tqlen: %d\n",pxqcfg->qlen);
		pcName = fapi_qos_getNameFromVal(pxqcfg->sched,axschednv);
		printf("\tScheduler: %s\n",pcName ? pcName : "Nill");
		printf("\tweight: %d\n",pxqcfg->weight);
		printf("\tpriority: %d\n",pxqcfg->priority);
		fapi_qos_printDrop(&pxqcfg->drop);
		fapi_qos_printShaper(&pxqcfg->shaper);
		pxqcfg++;
	}
	return LTQ_SUCCESS;
}

/* =============================================================================
 *  Function Name : fapi_qos_PrintGenRsp
 *  Description   : Prints generic response of FAPI.
 * ============================================================================*/
static void
fapi_qos_PrintGenRsp(x_QIPC_GenRsp *pxGenRsp)
{
	printf("Return Value is [%d] ErrStr is [%s]\n",pxGenRsp->iRetVal,pxGenRsp->acErrStr);
}

/* =============================================================================
 *  Function Name : fapi_qos_PrintPGetRsp
 *  Description   : Prints response from port get FAPI.
 * ============================================================================*/
void
fapi_qos_PrintPGetRsp(x_QIPC_pGetRsp *pxpGetRsp)
{
	fapi_qos_PrintGenRsp(&pxpGetRsp->xGenRsp);
	fapi_qos_printShaper(&pxpGetRsp->xshaper);
	printf("Weight [%d]\n",pxpGetRsp->iWeight);
	printf("Priority [%d]\n",pxpGetRsp->iPriority);
}

/* =============================================================================
 *  Function Name : fapi_qos_PrintqSetRsp
 *  Description   : Prints response from queue set FAPI.
 * ============================================================================*/
void
fapi_qos_PrintqSetRsp(x_QIPC_qGetRsp *pxqGetRsp)
{
	fapi_qos_PrintGenRsp(&pxqGetRsp->xGenRsp);
	fapi_qos_printQCfg(pxqGetRsp->iNumQs,pxqGetRsp->pxqCfg); 
}

/* =============================================================================
 *  Function Name : fapi_qos_PrintqSetReq
 *  Description   : Prints queue set FAPI arguments.
 * ============================================================================*/
void
fapi_qos_PrintqSetReq(x_QIPC_qSetReq *pxqSetReq)
{
	printf("Interface [%s]\n",pxqSetReq->acIfName);
	fapi_qos_printQCfg(1,&pxqSetReq->xqCfg); 
	printf("Flags [%u]\n",pxqSetReq->uiFlags);
}

/* =============================================================================
 *  Function Name : fapi_qos_PrintqGetReq
 *  Description   : Prints queue get FAPI arguments.
 * ============================================================================*/
void
fapi_qos_PrintqGetReq(x_QIPC_qGetReq *pxqGetReq)
{
	printf("Interface [%s]\n",pxqGetReq->acIfName);
	printf("Q Name [%s]\n",pxqGetReq->acqName);
	printf("Flags [%u]\n",pxqGetReq->uiFlags);
}

/* =============================================================================
 *  Function Name : fapi_qos_PrintpSetReq
 *  Description   : Prints port set FAPI arguments.
 * ============================================================================*/
void
fapi_qos_PrintpSetReq(x_QIPC_pSetReq *pxpSetReq)
{
	printf("Interface [%s]\n",pxpSetReq->acIfName);
	fapi_qos_printShaper(&pxpSetReq->xshaper); 
	printf("Weight [%d]\n",pxpSetReq->iWeight);
	printf("Priority [%d]\n",pxpSetReq->iPriority);
	printf("Flags [%u]\n",pxpSetReq->uiFlags);
}

/* =============================================================================
 *  Function Name : fapi_qos_PrintpGetReq
 *  Description   : Prints port get FAPI arguments.
 * ============================================================================*/
void
fapi_qos_PrintpGetReq(x_QIPC_pGetorUpdateReq *pxpGetReq)
{
	printf("Interface [%s]\n",pxpGetReq->acIfName);
	printf("Flags [%u]\n",pxpGetReq->uiFlags);
}

