/*******************************************************************************
         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/
/**
 * @file fapi_mcast.c
 * @brief Functions to communicate  with mcast_helper
 *
 */


/* Header files */
#include "stdio.h"
#include "string.h"
#include "fapi_mcast.h"
#include "ulogging.h"
#include "ltq_api_include.h"

/* Defines  */
#define MCAST_HELPER_DEVICE	"/dev/mcast"
#define MCH_MAGIC 'M'
#define MCH_MEMBER_ENTRY_ADD _IOR(MCH_MAGIC, 0, char *)
#define MCH_MEMBER_ENTRY_UPDATE _IOR(MCH_MAGIC, 1, char *)
#define MCH_MEMBER_ENTRY_REMOVE _IOR(MCH_MAGIC, 2, char *)
#define MCH_SEVER_ENTRY_GET _IOR(MCH_MAGIC, 3, char *)


/* Logging LEVEL/TYPE INIT */
#ifndef LOG_LEVEL
uint16_t LOGLEVEL = SYS_LOG_DEBUG + 1;
#else
uint16_t LOGLEVEL = LOG_LEVEL + 1;
#endif
#ifndef LOG_TYPE
uint16_t LOGTYPE = SYS_LOG_TYPE_FILE;
#else
uint16_t LOGTYPE = LOG_TYPE;
#endif


/* ===================================================================================
 *  Function Name : fapi_mch_ioctl_cmd
 *  Description   : IOCTL function to communicate with mcast helper module
 * ================================================================================= */


static int fapi_mch_ioctl_cmd(int ioctl_cmd, void *data)
{
	int ret = UGW_SUCCESS;
	int fd = 0;

	if ((fd = open(MCAST_HELPER_DEVICE, O_RDWR)) < 0) {
		ret = UGW_FAILURE;
	} else {
		if (ioctl(fd, ioctl_cmd, data) < 0) {
			ret = UGW_FAILURE;
		}
		close(fd);
	}
	return ret;
}

/* ===================================================================================
 *  Function Name : fapi_mch_add_entry
 *  Description   : FAPI for adding entry in multicast helper kernel module
 * ================================================================================= */



int  fapi_mch_add_entry(MCAST_MEMBER_t *mch_mem)
{
	int ret = UGW_FAILURE;
	if(mch_mem==NULL)
		return ret;
	ret = fapi_mch_ioctl_cmd(MCH_MEMBER_ENTRY_ADD, mch_mem);
	return ret;
}

/* ===================================================================================
 *  Function Name : fapi_mch_del_entry
 *  Description   : FAPI for deleting entry in multicast helper kernel module
 * ================================================================================= */


int  fapi_mch_del_entry(MCAST_MEMBER_t *mch_mem)
{
	int ret = UGW_FAILURE;
	if(mch_mem==NULL)
		return ret;
	ret = fapi_mch_ioctl_cmd(MCH_MEMBER_ENTRY_REMOVE, mch_mem);
	return ret;
}

/* ===================================================================================
 *  Function Name : fapi_mch_update_entry
 *  Description   : FAPI for updating  entry in multicast helper kernel module
 * ================================================================================= */


int  fapi_mch_update_entry(MCAST_MEMBER_t *mch_mem)
{
	int ret = UGW_FAILURE;
	if(mch_mem==NULL)
		return ret;
	ret = fapi_mch_ioctl_cmd(MCH_MEMBER_ENTRY_UPDATE, mch_mem);
	return ret;
}

int  fapi_mch_get_lanserver(MCAST_MEMBER_t *mch_mem)
{
	int ret = UGW_FAILURE;
	if(mch_mem==NULL)
		return ret;
	ret = fapi_mch_ioctl_cmd(MCH_SEVER_ENTRY_GET, mch_mem);
	return ret;
}


