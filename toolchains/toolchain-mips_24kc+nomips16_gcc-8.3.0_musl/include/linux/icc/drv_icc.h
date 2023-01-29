#ifndef _DRV_UAPI_ICC_H
#define _DRV_UAPI_ICC_H
/******************************************************************************

                              Copyright (c) 2012
                            Lantiq Deutschland GmbH
                             http://www.lantiq.com

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

****************************************************************************
   Module      : drv_icc.h
   Description : This file contains the defines, the structures declarations
                 the tables declarations and the global functions declarations.
*******************************************************************************/
#include "drv_mps.h"
/* ============================= */
/* ICC Common defines            */
/* ============================= */
/*---------------------------------------------------------------------------*/
#define MAX_MMAP 16
#define MAX_DEPTH 16
#define MIN_THRESHOLD 4
#define MAX_THRESHOLD 12
#define ICC_MSG_FLOW_CONTROL 0x1
#define SEC_STORE_ID 0x3


/*DebugInfo*/
#define DBG_LEVEL_OFF      4
#define DBG_LEVEL_HIGH     3
#define DBG_LEVEL_NORMAL   2
#define DBG_LEVEL_LOW      1

#define CREATE_TRACE_GROUP(name) unsigned int G_nTraceGroup##name = DBG_LEVEL_HIGH
#define DECLARE_TRACE_GROUP(name) extern unsigned int G_nTraceGroup##name
#define PRINTF printk 
#define TRACE(name,level,message) do {if(level >= G_nTraceGroup##name) \
      { PRINTF message ; } } while(0)

/*---------------------------------------------------------------------------*/
/* Device connection structure                                               */
/*---------------------------------------------------------------------------*/
typedef struct{
 uint8_t src_client_id;
 uint8_t dst_client_id;
 uint8_t msg_id;
 uint8_t param_attr;
 uint32_t param[MAX_UPSTRM_DATAWORDS];
} icc_msg_t;

typedef struct{
	uint32_t address[MAX_UPSTRM_DATAWORDS];
  uint32_t length[MAX_UPSTRM_DATAWORDS];
  uint32_t offset[MAX_UPSTRM_DATAWORDS];
	uint32_t count;
}icc_commit_t;

/*Always keep your new clients within InvalidClient and MAX_CLIENT*/
typedef enum
{
   ICC_Client,/*Icc client Id*/
	 IA,/*Image authenticator*/
	 SR,/*System reset driver*/
	 IR,/*Ioctl redirector*/
	 PR,/*procfs redirector*/
	 SEC_STORE_SERVICE = 10, /*Secure store service */
   MAX_CLIENT=21/*MAX_CLIENT_ID*/
} icc_devices;/*enum for all the possible clients*/
/******************************************************************************
 * Exported functions
 ******************************************************************************/
/** magic number */
#define ICC_MAGIC 'S'
#define ICC_IOC_REG_CLIENT _IOW(ICC_MAGIC,1,uint32_t)
#define ICC_IOC_MEM_COMMIT _IOW(ICC_MAGIC,2,icc_commit_t)
#define ICC_IOC_MEM_INVALIDATE _IOW(ICC_MAGIC,3,icc_commit_t)

#define ICC_BOOTCORE_UP 0x1

#ifdef __LIBRARY__
int icc_init(void);
int icc_raw_read (icc_msg_t * rw);
int icc_write (icc_devices type, icc_msg_t * rw);
#endif
#endif /* _DRV_UAPI_ICC_H */
