/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_RTLOG_APP_COMM_H__
#define __MTLK_RTLOG_APP_COMM_H__

#include <linux/if.h>
#include "mtlkbfield.h"

/* Application's name maximal length*/
#define IWLWAV_RTLOG_APP_NAME_MAX_LEN          63

/* Application's name and OID */
#define IWLWAV_RTLOG_APP_NAME_DRVHLPR          "drvhlpr"
#define IWLWAV_RTLOG_APP_OID_DRVHLPR           3
#define IWLWAV_RTLOG_APP_NAME_DUMPHANDLER      "dump_handler"
#define IWLWAV_RTLOG_APP_NAME_WHMHANDLER       "whm_handler"
#define IWLWAV_RTLOG_APP_NAME_BCLSOCKSERVER    "bclsockserver"
#define IWLWAV_RTLOG_APP_NAME_DUTSERVER        "dutserver"
#define IWLWAV_RTLOG_APP_NAME_LOGSERVER        "logserver"
#define IWLWAV_RTLOG_APPGROUP_OID_MTTOOLS      6
#define IWLWAV_RTLOG_APP_NAME_HOSTAPD          "hostapd"
#define IWLWAV_RTLOG_APP_NAME_WPASUPPLICANT    "wpa_supplicant"
#define IWLWAV_RTLOG_APPGROUP_OID_HOSTAPD      9
#define IWLWAV_RTLOG_OID_INVALID               255

/* Application and Driver command's ID */
enum {
  IWLWAV_RTLOG_APP_DRV_CMDID_STARTUP = 1,  /* Application's event - Startup */
  IWLWAV_RTLOG_APP_DRV_CMDID_CLEANUP,      /* Application's event - Cleanup */
  IWLWAV_RTLOG_APP_DRV_CMDID_LOG,          /* Application's event - Log-message */
  IWLWAV_RTLOG_DRV_APP_CMDID_CONFIG_INIT,  /* Driver's event - RT Logger's initial Configuration */
  IWLWAV_RTLOG_DRV_APP_CMDID_CONFIG_NEW,   /* Driver's event - RT Logger's new Configuration */
  IWLWAV_RTLOG_DRV_APP_CMDID_CLEANUP       /* Driver's event - Cleanup */
};

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

/* Application's and Driver's message header */
typedef struct rtlog_app_drv_msghdr {
  uint16 cmd_id;        /* command's id */
  uint16 length;        /* length of payload */
  uint32 pid;           /* application's PID (type of pid_t) */
  uint32 log_info;      /* LID, OID, GID, FID */
  uint32 wlan_if;       /* interface/card ID */
  uint32 log_time;      /* time-stamp of Log-message */
} __MTLK_IDATA rtlog_app_drv_msghdr_t;

/* Application's message payload */
typedef struct rtlog_app_drv_msgpay {
  char name[IFNAMSIZ];  /* application's name */
} __MTLK_IDATA rtlog_app_drv_msgpay_t;

/* Driver's message payload */
typedef struct rtlog_drv_app_msgpay {
  int8 cdbg_lvl;        /* level of console debug */
  int8 rdbg_lvl;        /* level of remote debug */
} __MTLK_IDATA rtlog_drv_app_msgpay_t;

/* Application's message */
typedef struct rtlog_app_drv_msg {
  rtlog_app_drv_msghdr_t hdr;
  rtlog_app_drv_msgpay_t data;
} __MTLK_IDATA rtlog_app_drv_msg_t;

/* Driver's message */
typedef struct rtlog_drv_app_msg {
  rtlog_app_drv_msghdr_t hdr;
  rtlog_drv_app_msgpay_t data;
} __MTLK_IDATA rtlog_drv_app_msg_t;

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

/* Bit-fields for log-info word (32 bits) of message's header */
#define LOG_INFO_BFIELD_LID            MTLK_BFIELD_INFO(17, 14)
#define LOG_INFO_BFIELD_OID            MTLK_BFIELD_INFO(12,  5)
#define LOG_INFO_BFIELD_GID            MTLK_BFIELD_INFO( 5,  7)
#define LOG_INFO_BFIELD_FID            MTLK_BFIELD_INFO( 0,  5)

/* Make log-info word (32 bits) */
#define LOG_INFO_MAKE_WORD(lid, oid, gid, fid) \
  (MTLK_BFIELD_VALUE(LOG_INFO_BFIELD_LID, (lid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_BFIELD_OID, (oid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_BFIELD_GID, (gid), uint32) | \
   MTLK_BFIELD_VALUE(LOG_INFO_BFIELD_FID, (fid), uint32))

/* Parse log-info word (32 bits) */
#define LOG_INFO_GET_BFILED_LID(var)   MTLK_BFIELD_GET(var, LOG_INFO_BFIELD_LID)
#define LOG_INFO_GET_BFILED_OID(var)   MTLK_BFIELD_GET(var, LOG_INFO_BFIELD_OID)
#define LOG_INFO_GET_BFILED_GID(var)   MTLK_BFIELD_GET(var, LOG_INFO_BFIELD_GID)
#define LOG_INFO_GET_BFILED_FID(var)   MTLK_BFIELD_GET(var, LOG_INFO_BFIELD_FID)

#endif /* __MTLK_RTLOG_APP_COMM_H__ */
