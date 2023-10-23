/******************************************************************************

                         Copyright (c) 2012, 2014, 2015
                        Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef __QOSAL_IPC_MSG_H__
#define __QOSAL_IPC_MSG_H__

#define MAX_ERR_STR 256
#include "qosal_cl_api.h"

typedef struct{
	qos_cfg_t qoscfg;
	uint32_t uiFlags;
}x_QIPC_qoscfgReq;

typedef struct{
        qos_class_cfg_t clscfg;
        uint32_t uiFlags;
}x_QIPC_clscfgReq;

typedef enum{
	/* QAPI message type uses 1 to 10. Class IPC should use 10-20 */
	QIPC_QSET_REQ=1,
	QIPC_QGET_REQ,
	QIPC_QSTATSGET_REQ,
	QIPC_QGET_RSP,
	QIPC_PSET_REQ,
	QIPC_PGET_REQ,
	QIPC_PGET_RSP,
	QIPC_PUPDATE_REQ,
	QIPC_PUPDATE_RSP,
	QIPC_GEN_RSP,
	QIPC_STATUS_SET, /* this and below enums are not used for q object.
			however qosd_process_msg expects all QIPC enums to be in one order.
			so these two are added here. */
	QIPC_STATUS_GET,
	QIPC_IF_ABS_SET,
	QIPC_IF_ABS_GET,
	QIPC_IF_BASE_SET,
	QIPC_CLASS_CFG_SET,
	QIPC_CLASS_CFG_GET,
	QIPC_IF_INGGRP_SET,
	QIPC_IF_INGGRP_GET
}e_QIPC_MsgType;

typedef struct{
	char acIfName[MAX_IF_NAME_LEN];
	qos_queue_cfg_t xqCfg;
	uint32_t uiFlags;
}x_QIPC_qSetReq;

typedef struct{
	char acIfName[MAX_IF_NAME_LEN];
	char acqName[MAX_Q_NAME_LEN];
	uint32_t uiFlags;
}x_QIPC_qGetReq;

typedef struct{
	char acIfName[MAX_IF_NAME_LEN];
  qos_shaper_t xshaper;
	int32_t iWeight;
  int32_t iPriority;
	uint32_t uiFlags;
}x_QIPC_pSetReq;

typedef struct{
	char acIfName[MAX_IF_NAME_LEN];
	uint32_t uiFlags;
}x_QIPC_pGetorUpdateReq;

typedef struct{
	int32_t iRetVal;
	char acErrStr[MAX_ERR_STR];
}x_QIPC_GenRsp;

typedef struct{
	int32_t iRetVal;
	qos_cfg_t qosCfg;
}x_QIPC_qosCfgRsp;

typedef struct{
	x_QIPC_GenRsp xGenRsp;
  qos_shaper_t xshaper;
	int32_t iWeight;
  int32_t iPriority;
}x_QIPC_pGetRsp;

typedef struct{
	x_QIPC_GenRsp xGenRsp;
	int32_t iNumQs;
	qos_queue_cfg_t *pxqCfg;
}x_QIPC_qGetRsp;

#endif /*__QOSAL_IPC_MSG_H__*/
