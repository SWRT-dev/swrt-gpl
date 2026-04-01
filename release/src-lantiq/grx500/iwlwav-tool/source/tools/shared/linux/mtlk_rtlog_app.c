/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"

#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog_app.h"
#include <linux/version.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include "nl.h"

#define LOG_LOCAL_GID   GID_RTLOG_APP
#define LOG_LOCAL_FID   1

static rtlog_app_info_t *rtlog_info = NULL;

static inline void
__set_is_terminated (void)
{
  if (NULL != rtlog_info) {
    rtlog_info->terminated_status = MTLK_RTLOG_APP_TERMINATED;
  }
}

static inline void
__set_is_killed (void)
{
  if (NULL != rtlog_info) {
    rtlog_info->terminated_status = MTLK_RTLOG_APP_KILLED;
  }
}

static void
_on_sighup (int sig)
{
  ILOG1_V("Received SIGHUP: terminating");
  __set_is_terminated();
}

static void
_on_sigchld (int sig)
{
  while (waitpid(-1, NULL, WNOHANG) > 0);
  signal(SIGCHLD, _on_sigchld);
}

static void
_on_sigint (int sig)
{
  ILOG1_V("Received SIGINT: terminating");
  __set_is_terminated();
}

static void
_on_sigterm (int sig)
{
  ILOG1_V("Received SIGTERM: terminating");
  __set_is_terminated();
}

static inline void
__setup_signals (void)
{
  signal(SIGHUP,  _on_sighup);
  signal(SIGCHLD, _on_sigchld);
  signal(SIGINT,  _on_sigint);
  signal(SIGTERM, _on_sigterm);
  signal(SIGPIPE, SIG_IGN);
  signal(SIGALRM, SIG_IGN);
}

int __MTLK_IFUNC
mtlk_rtlog_app_send_log_msg (/*rtlog_app_info_t *info,*/
        const uint32 ids, /* LID, OID, GID, FID */
        const uint32 timestamp,
        const void *data, const uint16 data_len)
{
  int res;
  void *msg;
  size_t msglen;
  rtlog_app_drv_msghdr_t *msgbody_hdr;
  rtlog_app_drv_msgpay_t *msgbody_appname;
  void *msgbody_logdata;
  rtlog_app_info_t *info = rtlog_info;

  if ((0 == data_len) || (NULL == info)) {
    return 0;
  }

  msglen = (size_t)data_len + sizeof(rtlog_app_drv_msg_t);
  msg = malloc(msglen);
  if (NULL == msg) {
    return -1;
  }

  msgbody_hdr = (rtlog_app_drv_msghdr_t *)msg;
  msgbody_appname = (rtlog_app_drv_msgpay_t *)(msgbody_hdr + 1);
  msgbody_logdata = (void *)(msgbody_appname + 1);

  /* Prepare message header */
  msgbody_hdr->cmd_id = (uint16)IWLWAV_RTLOG_APP_DRV_CMDID_LOG;
  msgbody_hdr->length = data_len + sizeof(*msgbody_appname);
  msgbody_hdr->pid = (uint32)info->app_pid;
  msgbody_hdr->log_info = ids;
  msgbody_hdr->wlan_if = info->wlan_if;
  msgbody_hdr->log_time = timestamp;

  /* Prepare message's payload */
  wave_strcopy(msgbody_appname->name, info->app_name, sizeof(msgbody_appname->name));
  wave_memcpy(msgbody_logdata, (size_t)data_len, data, (size_t)data_len);

  /* Send message */
  res = wave_nlink_send_brd_msg(&info->nl_socket,
                                msg,
                                msglen,
                                NL_DRV_CMD_RTLOG_NOTIFY);
  free(msg);
  return res;
}

static int
_send_notification_msg (rtlog_app_info_t *info, const uint8 cmd)
{
  int res;
  rtlog_app_drv_msg_t msg;
  rtlog_app_drv_msghdr_t *hdr = &msg.hdr;
  rtlog_app_drv_msgpay_t *data = &msg.data;

  /* Prepare message's header */
  memset(hdr, 0, sizeof(*hdr));
  hdr->cmd_id = (uint16)cmd;
  hdr->length = sizeof(*data);
  hdr->pid = (uint32)info->app_pid;

  /* Prepare message's payload */
  wave_strcopy(data->name, info->app_name, sizeof(data->name));

  /* Send message */
  res = wave_nlink_send_brd_msg(&info->nl_socket,
                                (void *)&msg,
                                sizeof(msg),
                                NL_DRV_CMD_RTLOG_NOTIFY);
  if (res < 0) {
    ELOG_SD("Netlink socket transmit failed: %s (%d)", strerror(res), res);
  }
  return res;
}

static inline BOOL
__is_debug_lvl_changed (rtlog_drv_app_msgpay_t *data)
{
  if (((int)data->cdbg_lvl != IWLWAV_RTLOG_DLEVEL_VAR) ||
      ((int)data->rdbg_lvl != IWLWAV_RTLOG_DLEVEL_VAR_REMOTE)) {
    return TRUE;
  }
  return FALSE;
}

static void
_parse_notification_msg (void *param, void *packet)
{
  uint8 oid;
  BOOL is_cfg_changed = FALSE;
  rtlog_app_info_t *info = (rtlog_app_info_t *)param;
  rtlog_drv_app_msg_t *msg = (rtlog_drv_app_msg_t *)packet;
  rtlog_app_drv_msghdr_t *hdr = &msg->hdr;
  rtlog_drv_app_msgpay_t *data = &msg->data;

  MTLK_ASSERT(sizeof(rtlog_drv_app_msgpay_t) == hdr->length);

  oid = LOG_INFO_GET_BFILED_OID(hdr->log_info);

  ILOG3_DDDDD("Notification message received from mtlkroot: "
              "cmd_id=0x%04x, data_len=%d, oid=%d, params=[cdebug=%d, rdebug=%d].",
              hdr->cmd_id, hdr->length, oid, data->cdbg_lvl, data->rdbg_lvl);

  switch (hdr->cmd_id)
  {
    case IWLWAV_RTLOG_DRV_APP_CMDID_CONFIG_INIT:
    {
      /* Check received PID, if invalid then ignore message */
      if (hdr->pid == (uint32)info->app_pid) {
        /* Save OID */
        info->app_oid = oid;
        is_cfg_changed = TRUE;
      }
      else {
   	    ILOG3_DD("Notification message ignored from mtlkroot due to invalid PID: "
                 "own_pid=%d, received_pid=%d.", info->app_pid, hdr->pid);
      }
      break;
    }
    case IWLWAV_RTLOG_DRV_APP_CMDID_CONFIG_NEW:
    {
      /* Check received OID, if invalid then ignore message */
      if (oid == (uint32)info->app_oid) {
        /* Check that debug level is changed, if not changed then ignore the message */
        is_cfg_changed = __is_debug_lvl_changed(data);
        if (!is_cfg_changed) {
          ILOG2_V("Notification message ignored from mtlkroot due to unchanged debug level.");
        }
      }
      else {
   	    ILOG3_DD("Notification message ignored from mtlkroot due to invalid OID: "
                 "own_oid=%d, received_oid=%d.", info->app_oid, oid);
      }
      break;
    }
    case IWLWAV_RTLOG_DRV_APP_CMDID_CLEANUP:
    {
      /* Disable remote debug */
      data->cdbg_lvl = (int8)IWLWAV_RTLOG_DLEVEL_VAR;
      data->rdbg_lvl = -1;
      is_cfg_changed = __is_debug_lvl_changed(data);
      break;
    }
    default: break;
  }

  if (is_cfg_changed) {
    /* FIXME: check if lock is required */
    IWLWAV_RTLOG_DLEVEL_VAR = (int)data->cdbg_lvl;
    IWLWAV_RTLOG_DLEVEL_VAR_REMOTE = (int)data->rdbg_lvl;
    /* FIXME: check if unlock is required */

    ILOG2_DD("Notification message from mtlkroot processed. "
             "Debug levels set to: [cdebug=%d, rdebug=%d].",
             IWLWAV_RTLOG_DLEVEL_VAR, IWLWAV_RTLOG_DLEVEL_VAR_REMOTE);
#if 0
    ILOG0_V("Debug level #0 is enabled.");
    ILOG1_V("Debug level #1 is enabled.");
    ILOG2_V("Debug level #2 is enabled.");
#endif
  }
}

static void *
_notification_thread_proc (void *param)
{
  int res;
  rtlog_app_info_t *info = (rtlog_app_info_t *)param;

  res = mtlk_nlink_create(&info->nl_socket, MTLK_NETLINK_RTLOG_APPS_GROUP_NAME, _parse_notification_msg, info);

  if (res < 0) {
    ELOG_SD("Failed to create Netlink socket: %s (%d)", strerror(res), res);
    goto end;
  }

  res = pipe(info->stop_pipe_fd);
  if (res) {
    ELOG_SD("Failed to create pipe: %s (%d)", strerror(res), res);
    goto end_clean;
  }

  info->app_pid = getpid();
  res = _send_notification_msg(info, IWLWAV_RTLOG_APP_DRV_CMDID_STARTUP);
  if (res < 0) {
    goto end_clean;
  }

  rtlog_info = info;

  res = mtlk_nlink_receive_loop(&info->nl_socket, info->stop_pipe_fd[0]);
  if (res < 0) {
    ELOG_SD("Netlink socket receive failed: %s (%d)", strerror(res), res);
  }

  rtlog_info = NULL;

  _send_notification_msg(info, IWLWAV_RTLOG_APP_DRV_CMDID_CLEANUP);

end_clean:
  mtlk_nlink_cleanup(&info->nl_socket);

end:
  pthread_exit(&res);
}

static int
_notification_thread_create (rtlog_app_info_t *info)
{
  int res;
  pthread_attr_t attr;

  /* Initialize and set thread detached attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  res = pthread_create(&info->notification_thread, &attr, _notification_thread_proc, info);
  if (res != 0) {
    ELOG_SD("Failed to create Notification thread: %s (%d)", strerror(res), res);
  }
  else {
    ILOG1_D("Succeed to create Notification thread: (%d)", res);
  }

  pthread_attr_destroy(&attr);

  return res;
}

static inline void
__notification_thread_stop (rtlog_app_info_t *info)
{
  int res;
  void *status;

  /* Signal to the notification thread to stop */
  write(info->stop_pipe_fd[1], "x", 1);
  /* Wait until the notification thread processed the signal */
  res = pthread_join(info->notification_thread, &status);
  if (0 != res) {
    ELOG_SD("Failed to terminate Notification thread: %s (%d)", strerror(res), res);
  }
  else {
    ILOG1_D("Succeed to terminate Notification thread: (%d)", res);
  }

  close(info->stop_pipe_fd[0]);
  close(info->stop_pipe_fd[1]);
}

int __MTLK_IFUNC
mtlk_assign_logger_hw_iface(rtlog_app_info_t *info, const char *ifname)
{
  char *first_digit;
  int if_num;

  first_digit = strpbrk(ifname, "0123456789");
  if (!first_digit) {
    ELOG_V("Error occured getting interface number");
    return MTLK_ERR_UNKNOWN;
  }

  if_num = (int)(*first_digit - '0') / 2;
  if (if_num < 0) {
    ELOG_V("Sanity check error occured getting HW interface number");
    return MTLK_ERR_UNKNOWN;
  }

  info->wlan_if = if_num;

  ILOG1_D("HW interface ID: %d", info->wlan_if);
  return MTLK_ERR_OK;
}


int __MTLK_IFUNC
mtlk_rtlog_app_init (rtlog_app_info_t *info, const char *name)
{
  memset(info, 0, sizeof(rtlog_app_info_t));
  wave_strcopy(info->app_name, name, sizeof(info->app_name));
  info->app_pid = (pid_t)(-1);
  info->app_oid = IWLWAV_RTLOG_OID_INVALID;

  __setup_signals();
  if (_notification_thread_create(info)) {
    return MTLK_ERR_UNKNOWN;
  }

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_rtlog_app_cleanup (rtlog_app_info_t *info)
{
  if (NULL != rtlog_info) {
    __notification_thread_stop(info);
  }
}

int __MTLK_IFUNC
mtlk_rtlog_app_get_terminated_status (void)
{
  int flag = MTLK_RTLOG_APP_RUNNING;
  if (NULL != rtlog_info) {
    flag = rtlog_info->terminated_status;
  }

  return flag;
}
#endif /* CONFIG_WAVE_RTLOG_REMOTE */
