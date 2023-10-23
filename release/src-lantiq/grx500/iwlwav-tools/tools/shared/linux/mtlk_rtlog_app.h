/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_RTLOG_APP_H__
#define __MTLK_RTLOG_APP_H__

#include "mtlknlink.h"

#define MTLK_RTLOG_APP_RUNNING      (0)
#define MTLK_RTLOG_APP_TERMINATED   (1)
#define MTLK_RTLOG_APP_KILLED       (-1)

typedef struct rtlog_app_info
{
  pthread_t    notification_thread; /* thread's handler */
  volatile int terminated_status;   /* threads's termination flag */
  int          stop_pipe_fd[2];
  char         app_name[IFNAMSIZ];  /* application's name */
  pid_t        app_pid;             /* application's PID */
  uint8        app_oid;             /* application's OID */
  uint32       wlan_if;             /* HW interface */
  mtlk_nlink_socket_t nl_socket;    /* net-link socket data structure */
} rtlog_app_info_t;

int __MTLK_IFUNC mtlk_rtlog_app_init(rtlog_app_info_t *info, const char *name);
void __MTLK_IFUNC mtlk_rtlog_app_cleanup(rtlog_app_info_t *info);
int __MTLK_IFUNC mtlk_rtlog_app_send_log_msg(/*rtlog_app_info_t *info,*/
                                             const uint32 ids, /* LID, OID, GID, FID */
                                             const uint32 timestamp,
                                             const void *data, const uint16 data_len);
int __MTLK_IFUNC mtlk_rtlog_app_get_terminated_status(void);
int __MTLK_IFUNC mtlk_assign_logger_hw_iface(rtlog_app_info_t *info, const char *ifname);

#endif /* __MTLK_RTLOG_APP_H__ */
