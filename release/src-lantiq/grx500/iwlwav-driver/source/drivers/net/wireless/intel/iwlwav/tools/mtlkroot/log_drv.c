/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 *
 *
 *
 * Written by: Andriy Fidrya
 *
 */

/* Here we override user-defined rtlogger flags */
/* because log driver may print to console only */

/******************************************************************************/
/*! \file       log_drv.c
 *  \brief      LOG driver module.
 *
 */
#define IWLWAV_RTLOG_FLAGS (RTLF_CONSOLE_ENABLED)

#include "mtlkinc.h"

#include "log_drv.h"
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog.h"
#include "mtlk_rtlog_app_comm.h"
#endif
#include "nlmsgs.h"

#ifdef MTLK_USE_DIRECTPATH_API /* defined in mtlkinc.h */
#include <net/ppa_api.h>
#include <net/ppa_hook.h>
#if !defined(CONFIG_LTQ_PPA_GRX500) && !defined(MTLK_PUMA_LITEPATH)
  #include <asm/mach-lantiq/xway/switch-api/lantiq_ethsw_api.h>
#endif
#endif /* MTLK_USE_DIRECTPATH_API */

#include "drvver.h"

#define IWLWAV_RTLOG_ONE_LOG_PER_BUF   /* FIXME: temp solution for one LOG message per Logger buffer */

#ifdef MTLK_USE_DIRECTPATH_API
//#if defined(CONFIG_LTQ_PPA_GRX500) && CONFIG_LTQ_PPA_GRX500
#if defined(CONFIG_LTQ_PPA_API_DIRECTPATH_HAS_NEW_API)
#define PPE_DIRECTPATH_LEGACY_NON_LEGACY 0
#else
#define PPE_DIRECTPATH_LEGACY_NON_LEGACY PPE_DIRECTPATH_LEGACY
#endif
#endif

// -------------
// Configuration
// -------------

#define LOG_LOCAL_GID   GID_LOGDRV
#define LOG_LOCAL_FID   1

#define LOG_GET_DLVL_CON(dbg_lvl)      (dbg_lvl.console)
#define LOG_GET_DLVL_RT(dbg_lvl)       (dbg_lvl.remote)
#define LOG_GET_DLVL_CAP(dbg_lvl)      (dbg_lvl.capwap)

#define LOG_SET_DLVL_CON(dbg_lvl, val) { dbg_lvl.console = val; }
#define LOG_SET_DLVL_RT(dbg_lvl, val)  { dbg_lvl.remote  = val; }
#define LOG_SET_DLVL_CAP(dbg_lvl, val) { dbg_lvl.capwap  = val; }

#define LOG_DLVL_ALLOWED(lvl)   ((LOG_DLVL_MIN <= (lvl)) && ((lvl) <= LOG_DLVL_MAX))

#ifdef MTLK_USE_DIRECTPATH_API
#define rtlog_ppa_netdev_name "rtlog_radio"
#endif

// --------------------
// Forward declarations
// --------------------

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static inline uint32 buf_space_left(mtlk_log_buf_entry_t *buf);
static void put_ready_buf_and_notify(mtlk_log_buf_entry_t *buf);
static uint32 __MTLK_IFUNC log_timer(
    mtlk_osal_timer_t *timer, mtlk_handle_t clb_usr_data);
#endif

// ----------------
// Global variables
// ----------------

mtlk_log_data_t log_data = { 0 };

#ifdef CONFIG_WAVE_RTLOG_REMOTE
rtlog_cfg_t     g_rtlog_cfg;    /* config data */

typedef struct rtlog_info_app_entry {
  const char  *name;    /* application's name */
  const uint8 oid;      /* application's OID */
                        /* if valid, PID indicates that application is active */
  pid_t       pid[IWLWAV_RTLOG_HW_MAX_WLAN_IF];
} rtlog_info_app_entry_t;

#define IWLWAV_RTLOG_APP_PID_INVALID ((pid_t)(-1))
static rtlog_info_app_entry_t rtlog_info_app_array[] =
{
  {IWLWAV_RTLOG_APP_NAME_DRVHLPR, IWLWAV_RTLOG_APP_OID_DRVHLPR, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {IWLWAV_RTLOG_APP_NAME_DUMPHANDLER, IWLWAV_RTLOG_APPGROUP_OID_MTTOOLS, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {IWLWAV_RTLOG_APP_NAME_WHMHANDLER, IWLWAV_RTLOG_APPGROUP_OID_MTTOOLS, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {IWLWAV_RTLOG_APP_NAME_BCLSOCKSERVER, IWLWAV_RTLOG_APPGROUP_OID_MTTOOLS, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {IWLWAV_RTLOG_APP_NAME_DUTSERVER, IWLWAV_RTLOG_APPGROUP_OID_MTTOOLS, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {IWLWAV_RTLOG_APP_NAME_LOGSERVER, IWLWAV_RTLOG_APPGROUP_OID_MTTOOLS, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {IWLWAV_RTLOG_APP_NAME_HOSTAPD, IWLWAV_RTLOG_APPGROUP_OID_HOSTAPD, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {IWLWAV_RTLOG_APP_NAME_WPASUPPLICANT, IWLWAV_RTLOG_APPGROUP_OID_HOSTAPD, {IWLWAV_RTLOG_APP_PID_INVALID}},
  {NULL}
};

/*--------------
 Implementation
--------------*/

static inline BOOL
_rtlog_drv_cfg_is_active(void)
{
    return g_rtlog_cfg.drv_cfg.is_active;
}

static void
_mtlk_log_detach_active_buff(mtlk_log_data_t* plog_data);

static int
_mtlk_log_add_one_buffer(mtlk_log_data_t* plog_data)
{
  /* Warning this internal function is intended to be called on startup only. */
  /* It does not use locking, therefore.                                      */

  mtlk_log_buf_entry_t *pbuf_descr =
    mtlk_osal_mem_alloc(sizeof(mtlk_log_buf_entry_t) + buf_size, MTLK_MEM_TAG_LOGGER);

  if (NULL == pbuf_descr) {
    return MTLK_ERR_NO_MEM;
  }

  pbuf_descr->refcount  = 0;
  pbuf_descr->data_size = 0;
  pbuf_descr->expired   = 0;

  mtlk_dlist_push_back(&plog_data->bufs_free, (mtlk_ldlist_entry_t *) pbuf_descr);
  return MTLK_ERR_OK;
}

static void
_mtlk_log_drop_one_buffer(mtlk_log_data_t* plog_data)
{
  /* Warning this internal function is intended to be called on cleanup only. */
  /* It does not use locking and assumes there are packets in the free list.  */

  mtlk_log_buf_entry_t *pbuf_descr =
    MTLK_LIST_GET_CONTAINING_RECORD(mtlk_dlist_pop_front(&log_data.bufs_free),
                                    mtlk_log_buf_entry_t,
                                    entry);
  MTLK_ASSERT(NULL != pbuf_descr);
  mtlk_osal_mem_free(pbuf_descr);
};
#endif /* CONFIG_WAVE_RTLOG_REMOTE */

MTLK_INIT_STEPS_LIST_BEGIN(log_drv)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, FREE_BUFFS_LIST)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, READY_BUFFS_LIST)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, FREE_BUFFS_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, READY_BUFFS_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, FREE_BUFFS)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, DATA_LOCK)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, TIMER_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, BUFF_RM_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, BUFF_RM_LOCK_WAIT)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, BUFF_RM_LOCK_ACQ)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, LOG_OSDEP)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, DETACH_ACTIVE_BUFF)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, LOG_TIMER)
  MTLK_INIT_STEPS_LIST_ENTRY(log_drv, TIMER_SET)
#endif
MTLK_INIT_INNER_STEPS_BEGIN(log_drv)
MTLK_INIT_STEPS_LIST_END(log_drv);

void __MTLK_IFUNC
mtlk_log_cleanup(void)
{
  MTLK_CLEANUP_BEGIN(log_drv, MTLK_OBJ_PTR(&log_data))
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_CLEANUP_STEP(log_drv, TIMER_SET, MTLK_OBJ_PTR(&log_data),
                      mtlk_osal_timer_cancel_sync, (&log_data.timer));
    MTLK_CLEANUP_STEP(log_drv, LOG_TIMER, MTLK_OBJ_PTR(&log_data),
                      mtlk_osal_timer_cleanup, (&log_data.timer));
    MTLK_CLEANUP_STEP(log_drv, DETACH_ACTIVE_BUFF, MTLK_OBJ_PTR(&log_data),
                      _mtlk_log_detach_active_buff, (&log_data));
#endif
    MTLK_CLEANUP_STEP(log_drv, LOG_OSDEP, MTLK_OBJ_PTR(&log_data),
                      mtlk_log_on_cleanup, ());
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_CLEANUP_STEP(log_drv, BUFF_RM_LOCK_ACQ, MTLK_OBJ_PTR(&log_data),
                      mtlk_rmlock_release, (&log_data.buff_rm_lock));
    MTLK_CLEANUP_STEP(log_drv, BUFF_RM_LOCK_WAIT, MTLK_OBJ_PTR(&log_data),
                      mtlk_rmlock_wait, (&log_data.buff_rm_lock));
    MTLK_CLEANUP_STEP(log_drv, BUFF_RM_LOCK, MTLK_OBJ_PTR(&log_data),
                      mtlk_rmlock_cleanup, (&log_data.buff_rm_lock));
    MTLK_CLEANUP_STEP(log_drv, TIMER_LOCK, MTLK_OBJ_PTR(&log_data),
                      mtlk_osal_lock_cleanup, (&log_data.timer_lock));
#endif
    MTLK_CLEANUP_STEP(log_drv, DATA_LOCK, MTLK_OBJ_PTR(&log_data),
                      mtlk_osal_lock_cleanup, (&log_data.data_lock));
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    while (MTLK_CLEANUP_ITERATONS_LEFT(MTLK_OBJ_PTR(&log_data), FREE_BUFFS) > 0) {
      MTLK_CLEANUP_STEP_LOOP(log_drv, FREE_BUFFS, MTLK_OBJ_PTR(&log_data),
                             _mtlk_log_drop_one_buffer, (&log_data));
    }
    MTLK_CLEANUP_STEP(log_drv, READY_BUFFS_LOCK, MTLK_OBJ_PTR(&log_data),
                      mtlk_osal_lock_cleanup, (&log_data.bufs_ready_lock));
    MTLK_CLEANUP_STEP(log_drv, FREE_BUFFS_LOCK, MTLK_OBJ_PTR(&log_data),
                      mtlk_osal_lock_cleanup, (&log_data.bufs_free_lock));
    MTLK_CLEANUP_STEP(log_drv, READY_BUFFS_LIST, MTLK_OBJ_PTR(&log_data),
                      mtlk_dlist_cleanup, (&log_data.bufs_ready));
    MTLK_CLEANUP_STEP(log_drv, FREE_BUFFS_LIST, MTLK_OBJ_PTR(&log_data),
                      mtlk_dlist_cleanup, (&log_data.bufs_free));
#endif
  MTLK_CLEANUP_END(log_drv, MTLK_OBJ_PTR(&log_data))
}

int __MTLK_IFUNC
mtlk_log_init(void)
{
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  int i;
#endif

  MTLK_INIT_TRY(log_drv, MTLK_OBJ_PTR(&log_data))
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP_VOID(log_drv, FREE_BUFFS_LIST, MTLK_OBJ_PTR(&log_data),
                        mtlk_dlist_init, (&log_data.bufs_free));
    MTLK_INIT_STEP_VOID(log_drv, READY_BUFFS_LIST, MTLK_OBJ_PTR(&log_data),
                        mtlk_dlist_init, (&log_data.bufs_ready));
    MTLK_INIT_STEP(log_drv, FREE_BUFFS_LOCK, MTLK_OBJ_PTR(&log_data),
                   mtlk_osal_lock_init, (&log_data.bufs_free_lock));
    MTLK_INIT_STEP(log_drv, READY_BUFFS_LOCK, MTLK_OBJ_PTR(&log_data),
                    mtlk_osal_lock_init, (&log_data.bufs_ready_lock));
    for (i = 0; i < bufs_no; ++i) {
      MTLK_INIT_STEP_LOOP(log_drv, FREE_BUFFS, MTLK_OBJ_PTR(&log_data),
                          _mtlk_log_add_one_buffer, (&log_data))
    }
#endif
    MTLK_INIT_STEP(log_drv, DATA_LOCK, MTLK_OBJ_PTR(&log_data),
                   mtlk_osal_lock_init, (&log_data.data_lock));
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP(log_drv, TIMER_LOCK, MTLK_OBJ_PTR(&log_data),
                   mtlk_osal_lock_init, (&log_data.timer_lock));
    MTLK_INIT_STEP(log_drv, BUFF_RM_LOCK, MTLK_OBJ_PTR(&log_data),
                   mtlk_rmlock_init, (&log_data.buff_rm_lock));
    MTLK_INIT_STEP_VOID(log_drv, BUFF_RM_LOCK_WAIT, MTLK_OBJ_PTR(&log_data),
                        MTLK_NOACTION, ());
    MTLK_INIT_STEP_VOID(log_drv, BUFF_RM_LOCK_ACQ, MTLK_OBJ_PTR(&log_data),
                        mtlk_rmlock_acquire, (&log_data.buff_rm_lock));
#endif
    MTLK_INIT_STEP(log_drv, LOG_OSDEP, MTLK_OBJ_PTR(&log_data),
                   mtlk_log_on_init, ());
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP_VOID(log_drv, DETACH_ACTIVE_BUFF, MTLK_OBJ_PTR(&log_data),
                        MTLK_NOACTION, ());
    MTLK_INIT_STEP(log_drv, LOG_TIMER, MTLK_OBJ_PTR(&log_data),
                   mtlk_osal_timer_init, (&log_data.timer, log_timer, HANDLE_T(NULL)));
#endif
    /* TODO: This step should become a part of start sequence */
    mtlk_log_set_defconf(cdebug, rdebug);
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP(log_drv, TIMER_SET, MTLK_OBJ_PTR(&log_data),
                   mtlk_osal_timer_set, (&log_data.timer, buf_swap_tmout));
#endif
  MTLK_INIT_FINALLY(log_drv, MTLK_OBJ_PTR(&log_data))
  MTLK_INIT_RETURN(log_drv, MTLK_OBJ_PTR(&log_data), mtlk_log_cleanup, ());
}

#define LOG_DFLT_FMT    "Default log levels: cdebug = %d (console), rdebug = %d (remote), capdebug = %d (capture)\n"
#define LOG_OID_FMT     "GID  c  r  cap for OID %d\n"
#define LOG_ALL_FMT     "all %2d %2d %2d\n"
#define LOG_GID_FMT     "%3d %2d %2d %2d\n"

#define LOG_PRINT_DFLT(c, r, cap)     seq_printf(seq, LOG_DFLT_FMT, c, r, cap);
#define LOG_PRINT_OID(oid)            seq_printf(seq, LOG_OID_FMT, oid);
#define LOG_PRINT_GID(gid, c, r, cap) seq_printf(seq, LOG_GID_FMT, gid, c, r, cap);
#define LOG_PRINT_ALL(c, r, cap)      seq_printf(seq, LOG_ALL_FMT, c, r, cap);

void __MTLK_IFUNC
mtlk_log_get_conf (struct seq_file *seq)
{
  int gid, oid;
  int clvl, rlvl, caplvl;

  LOG_PRINT_DFLT(LOG_DEFAULT_CDEBUG, LOG_DEFAULT_RDEBUG, LOG_DEFAULT_CAPDEBUG);

  for (oid = 0; oid < MAX_OID; oid++) {
    BOOL print_all  = TRUE;
    int  clvl_all   = LOG_GET_DLVL_CON(log_data.dbg_level[oid][0]);
    int  rlvl_all   = LOG_GET_DLVL_RT(log_data.dbg_level[oid][0]);
    int  caplvl_all = LOG_GET_DLVL_CAP(log_data.dbg_level[oid][0]);

    LOG_PRINT_OID(oid);
    for (gid = 1; gid < MAX_GID; gid++) {
      clvl = LOG_GET_DLVL_CON(log_data.dbg_level[oid][gid]);
      rlvl = LOG_GET_DLVL_RT(log_data.dbg_level[oid][gid]);
      caplvl = LOG_GET_DLVL_CAP(log_data.dbg_level[oid][gid]);
      if ((clvl_all != clvl) || (rlvl_all != rlvl) || (caplvl_all != caplvl)) {
        print_all = FALSE;
        break;
      }
    }

    if (print_all) {
      if ((LOG_DEFAULT_CDEBUG == clvl_all) && (LOG_DEFAULT_RDEBUG == rlvl_all) && (LOG_DEFAULT_CAPDEBUG == caplvl_all)) {
      } else {
        LOG_PRINT_ALL(clvl_all, rlvl_all, caplvl_all);
      }
    } else {
      for (gid = 0; gid < MAX_GID; gid++) {
        clvl   = LOG_GET_DLVL_CON(log_data.dbg_level[oid][gid]);
        rlvl   = LOG_GET_DLVL_RT(log_data.dbg_level[oid][gid]);
        caplvl = LOG_GET_DLVL_CAP(log_data.dbg_level[oid][gid]);
        if ((LOG_DEFAULT_CDEBUG != clvl) || (LOG_DEFAULT_RDEBUG != rlvl) || (LOG_DEFAULT_CAPDEBUG != caplvl)) {
          LOG_PRINT_GID(gid, clvl, rlvl, caplvl);
        }
      }
    }
  }
}

void __MTLK_IFUNC
mtlk_log_set_cdebug_conf (int oid, int cdebug)
{
  int i;

  for (i = 0; i < MAX_GID; i++) {
    LOG_SET_DLVL_CON(log_data.dbg_level[oid][i], cdebug);
  }
}

void __MTLK_IFUNC
mtlk_log_set_rdebug_conf (int oid, int rdebug)
{
  int i;

  for (i = 0; i < MAX_GID; i++) {
    LOG_SET_DLVL_RT(log_data.dbg_level[oid][i], rdebug);
  }
}

void __MTLK_IFUNC
mtlk_log_set_capwap_conf (int oid, int capdebug)
{
  int i;

  for (i = 0; i < MAX_GID; i++) {
    LOG_SET_DLVL_CAP(log_data.dbg_level[oid][i], capdebug);
  }
}

void __MTLK_IFUNC
mtlk_log_set_defconf (int cdebug, int rdebug)
{
  int i, j;

  for (j = 0; j < MAX_OID; j++) {
    for (i = 0; i < MAX_GID; i++) {
      LOG_SET_DLVL_CON(log_data.dbg_level[j][i], cdebug);
      LOG_SET_DLVL_RT(log_data.dbg_level[j][i], rdebug);
      LOG_SET_DLVL_CAP(log_data.dbg_level[j][i], rdebug);
    }
  }
}

#define LOG_PARSE_ERROR "Log configuration parse error: "

static int
log_read_int (char **p)
{
  char *endp;
  int result;

  if (!isdigit(**p)) {
    ELOG_C(LOG_PARSE_ERROR "'%c' unexpected", **p);
    goto err_end;
  }
  endp = NULL;
  result = (uint8)simple_strtoul(*p, &endp, 10);
  if (*p == endp) {
    ELOG_V(LOG_PARSE_ERROR "value expected");
    goto err_end;
  }
  *p = endp;
  return result;
err_end:
  return -1;
}

static int
log_read_dlevel (char **p, char *target)
{
  char *endp;
  int result;

  if (!*p) {
    ELOG_V(LOG_PARSE_ERROR "missed log target: c or r");
    goto err_end;
  }
  if (**p != 'c' && **p != 'r') {
    ELOG_C(LOG_PARSE_ERROR "invalid log target: %c", **p);
    goto err_end;
  }
  *target = **p;
  (*p)++;
  if (!*p || !isdigit(**p)) {
    ELOG_V(LOG_PARSE_ERROR "invalid debug level");
    goto err_end;
  }
  endp = NULL;
  result = (uint8)simple_strtoul(*p, &endp, 10);
  if (*p == endp) {
    ELOG_V(LOG_PARSE_ERROR "value expected");
    goto err_end;
  }
  *p = endp;
  return result;
err_end:
  return -1;
}

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static inline void
__rtlog_info_app_reset_entry (void)
{
  int i;
  rtlog_info_app_entry_t *entry = rtlog_info_app_array;

  while (NULL != entry->name) {
    for (i = 0; i < IWLWAV_RTLOG_HW_MAX_WLAN_IF; i++) {
      entry->pid[i] = IWLWAV_RTLOG_APP_PID_INVALID;
    }
    entry++;
  }
}

static inline int
__rtlog_info_app_get_oid_by_name (const char *name, uint8 *oid)
{
  rtlog_info_app_entry_t *entry = rtlog_info_app_array;

  while (NULL != entry->name) {
    if (0 == strcmp(name, entry->name)) {
      *oid = entry->oid;
      return MTLK_ERR_OK;
    }
    entry++;
  }
  return MTLK_ERR_NO_ENTRY;
}

static inline int
__rtlog_info_app_get_name_by_pid (const pid_t pid, char **name)
{
  int i;
  rtlog_info_app_entry_t *entry = rtlog_info_app_array;

  while (NULL != entry->name) {
    i = 0;
    while (i < IWLWAV_RTLOG_HW_MAX_WLAN_IF) {
      if (pid == entry->pid[i]) {
        *name = (char*)entry->name;
        return MTLK_ERR_OK;
      }
      i++;
    }
    entry++;
  }
  return MTLK_ERR_NO_ENTRY;
}

static inline int
__rtlog_info_app_set_pid_by_name (const char *name, const pid_t pid)
{
  int i;
  rtlog_info_app_entry_t *entry = rtlog_info_app_array;

  while (NULL != entry->name) {
    if (0 == strcmp(name, entry->name)) {
      i = 0;
      while (i < IWLWAV_RTLOG_HW_MAX_WLAN_IF) {
        if (IWLWAV_RTLOG_APP_PID_INVALID == entry->pid[i]) {
          entry->pid[i] = pid;
          return MTLK_ERR_OK;
        }
        i++;
      }
      return MTLK_ERR_NO_RESOURCES;
    }
    entry++;
  }
  return MTLK_ERR_NO_ENTRY;
}

static inline int
__rtlog_info_app_reset_pid_by_name (const char *name, const pid_t pid)
{
  int i;
  rtlog_info_app_entry_t *entry = rtlog_info_app_array;

  while (NULL != entry->name) {
    if (0 == strcmp(name, entry->name)) {
      i = 0;
      while (i < IWLWAV_RTLOG_HW_MAX_WLAN_IF) {
        if (pid == entry->pid[i]) {
          entry->pid[i] = IWLWAV_RTLOG_APP_PID_INVALID;
          return MTLK_ERR_OK;
        }
        i++;
      }
      return MTLK_ERR_NO_ENTRY;
    }
    entry++;
  }
  return MTLK_ERR_NO_ENTRY;
}

static inline BOOL
__rtlog_info_app_is_one_active_by_oid (const uint8 oid)
{
  int i;
  rtlog_info_app_entry_t *entry = rtlog_info_app_array;

  while (NULL != entry->name) {
    if (oid == entry->oid) {
      i = 0;
      while (i < IWLWAV_RTLOG_HW_MAX_WLAN_IF) {
        if (IWLWAV_RTLOG_APP_PID_INVALID != entry->pid[i]) {
          return TRUE;
        }
        i++;
      }
    }
    entry++;
  }
  return FALSE;
}

static inline BOOL
__rtlog_info_app_is_one_active (void)
{
  int i;
  rtlog_info_app_entry_t *entry = rtlog_info_app_array;

  while (NULL != entry->name) {
    i = 0;
    while (i < IWLWAV_RTLOG_HW_MAX_WLAN_IF) {
      if (IWLWAV_RTLOG_APP_PID_INVALID != entry->pid[i]) {
        return TRUE;
      }
      i++;
    }
    entry++;
  }
  return FALSE;
}

static inline void
__mtlk_log_get_conf_on_set_conf (const int oid, const int8 debug_lvl,
        const BOOL is_cdebug, rtlog_drv_app_msgpay_t *outbuf)
{
  /* Get configuration (gid=0, means for all GIDs of this OID) */
  if (is_cdebug) {
    outbuf->cdbg_lvl = debug_lvl;
    outbuf->rdbg_lvl = LOG_GET_DLVL_RT(log_data.dbg_level[oid][0]);
  }
  else {
    outbuf->cdbg_lvl = LOG_GET_DLVL_CON(log_data.dbg_level[oid][0]);
    outbuf->rdbg_lvl = debug_lvl;
  }
}
#endif /* CONFIG_WAVE_RTLOG_REMOTE */

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static inline int
__mtlk_log_send_notification_msg (const rtlog_drv_app_msg_t *inbuf)
{
  return mtlk_nl_send_brd_msg((void *)inbuf,
                              sizeof(*inbuf),
                              GFP_ATOMIC,
                              NETLINK_RTLOG_APPS_GROUP,
                              NL_DRV_CMD_RTLOG_NOTIFY);
}

static void
_mtlk_log_on_set_conf (const int oid, const int8 debug_lvl, const BOOL is_cdebug)
{
  int res;
  rtlog_drv_app_msg_t msg;
  rtlog_app_drv_msghdr_t *hdr = &msg.hdr;
  rtlog_drv_app_msgpay_t *data = &msg.data;

  /* Check that application is active - don't send configuration if not */
  if (!__rtlog_info_app_is_one_active_by_oid(oid)) {
    ILOG2_D("No application (OID %d) is registered for RT Logger.", oid);
    return;
  }
  ILOG1_D("Notify application(s) (OID %d) that Debug-level is changed.", oid);

  /* Prepare message's header */
  memset(hdr, 0, sizeof(*hdr));
  hdr->cmd_id = IWLWAV_RTLOG_DRV_APP_CMDID_CONFIG_NEW;
  hdr->length = sizeof(*data);
  hdr->log_info = LOG_INFO_MAKE_WORD(0, oid, 0, 0);

  /* Get configuration */
  __mtlk_log_get_conf_on_set_conf(oid, debug_lvl, is_cdebug, data);
  /* Send configuration to application */
  res = __mtlk_log_send_notification_msg(&msg);
  if (MTLK_ERR_OK != res) {
    ELOG_DDD("Unable to notify application that Debug-level is changed: "
             "oid=%d, msgid=%d, res=%d.", oid, NL_DRV_CMD_RTLOG_NOTIFY, res);
  }
}

static inline int
__mtlk_log_reg_app_on_rx_msg (const char *app_name, const pid_t app_pid, uint8 *app_oid)
{
  int res;

  res = __rtlog_info_app_get_oid_by_name(app_name , app_oid);
  if (MTLK_ERR_OK != res) {
    WLOG_SD("Application is unknown. Don't send actual Debug-level to it: "
            "app_name=%s, res=%d.", app_name, res);
    return res;
  }

  res = __rtlog_info_app_set_pid_by_name(app_name, app_pid);
  if (MTLK_ERR_OK != res) {
    ELOG_SDD("Unable to register application for RT Logger. Don't send actual Debug-level to it: "
             "app_name=%s, app_pid=%d, res=%d.", app_name, app_pid, res);
    return res;
  }
  ILOG1_SD("Application is registered for RT Logger. Send actual Debug-level to it: "
           "app_name=%s, app_pid=%d.", app_name, app_pid);

  return res;
}

static void
_mtlk_log_unreg_app_on_rx_msg (const char *app_name, const pid_t app_pid)
{
  int res;

  res = __rtlog_info_app_reset_pid_by_name(app_name, app_pid);
  if (MTLK_ERR_OK != res) {
    ELOG_SDD("Unable to unregister application for RT Logger: "
             "app_name=%s, app_pid=%d, res=%d.", app_name, app_pid, res);
  }
  else {
    ILOG1_SD("Application is unregistered for RT Logger: "
             "app_name=%s, app_pid=%d.", app_name, app_pid);
  }
}

static inline void
__mtlk_log_get_conf_on_rx_msg (const int oid, const int8 debug_lvl,
        const BOOL is_cdebug, rtlog_drv_app_msgpay_t *outbuf)
{
  mtlk_osal_lock_acquire(&log_data.data_lock);
  __mtlk_log_get_conf_on_set_conf(oid, debug_lvl, is_cdebug, outbuf);
  mtlk_osal_lock_release(&log_data.data_lock);
}

static void
_mtlk_log_send_actual_conf (const char *app_name, const pid_t app_pid)
{
  int res;
  uint8 oid;
  rtlog_drv_app_msg_t msg;
  rtlog_app_drv_msghdr_t *hdr = &msg.hdr;
  rtlog_drv_app_msgpay_t *data = &msg.data;

  /* Try to register application for RT Logger */
  if (MTLK_ERR_OK != __mtlk_log_reg_app_on_rx_msg(app_name, app_pid, &oid)) {
    return;
  }

  /* Prepare message's header */
  memset(hdr, 0, sizeof(*hdr));
  hdr->cmd_id = IWLWAV_RTLOG_DRV_APP_CMDID_CONFIG_INIT;
  hdr->length = sizeof(*data);
  hdr->pid = (uint32)app_pid;
  hdr->log_info = LOG_INFO_MAKE_WORD(0, oid, 0, 0);

  /* Get configuration (gid=0, means for all GIDs of this OID) */
  __mtlk_log_get_conf_on_rx_msg(oid, LOG_GET_DLVL_CON(log_data.dbg_level[oid][0]), TRUE, data);
  /* Send configuration to application */
  res = __mtlk_log_send_notification_msg(&msg);
  if (MTLK_ERR_OK != res) {
    ELOG_DDSDDD("Unable to notify application about actual Debug-level [cdebug=%d, rdebug=%d]: "
                "app_name=%s, oid=%d, msgid=%d, res=%d.", data->cdbg_lvl, data->rdbg_lvl,
                app_name, oid, NL_DRV_CMD_RTLOG_NOTIFY, res);
  }
}

void __MTLK_IFUNC
wave_log_before_cleanup (void)
{
  int res;
  rtlog_drv_app_msg_t msg;
  rtlog_app_drv_msghdr_t *hdr = &msg.hdr;
  rtlog_drv_app_msgpay_t *data = &msg.data;

  /* Check that at least one application is active - don't send message if not */
  if (!__rtlog_info_app_is_one_active()) {
    return;
  }

  /* Unregister all applications for RT Logger */
  __rtlog_info_app_reset_entry();

  /* Prepare message's header */
  memset(hdr, 0, sizeof(*hdr));
  hdr->cmd_id = IWLWAV_RTLOG_DRV_APP_CMDID_CLEANUP;
  hdr->length = sizeof(*data);
  hdr->pid = (uint32)IWLWAV_RTLOG_APP_PID_INVALID;

  /* Disable console and remote debug */
  data->cdbg_lvl = -1;
  data->rdbg_lvl = -1;

  /* Send configuration to all applications */
  res = __mtlk_log_send_notification_msg(&msg);
  if (MTLK_ERR_OK != res) {
    mtlk_osal_emergency_print("Unable to notify all applications about Driver's cleanup: "
                              "msgid=%d, res=%d.", NL_DRV_CMD_RTLOG_NOTIFY, res);
  }
}

#define PACKET_TYPE_TSF 0xFFFFFFFF

void __MTLK_IFUNC
mtlk_log_tsf_sync_msg(int hw_idx, uint32 drv_ts, int32 fw_offset)
{
  uint8 *pdata;
  mtlk_log_buf_entry_t *pbuf;

  pbuf = mtlk_log_new_pkt_reserve(sizeof(mtlk_tsf_event_t), &pdata);

  if (NULL != pbuf) {
    mtlk_tsf_event_t tsf_event;
    uint8 *p = pdata;

    MTLK_ASSERT(NULL != pdata);

    /* Replace user-space time-stamp by Kernel-space time-stamp */
    tsf_event.msg_type = PACKET_TYPE_TSF;
    tsf_event.drv_timestamp = drv_ts;
    tsf_event.fw_offset = fw_offset;
    tsf_event.hw_idx = hw_idx;

    /* Put message header to buffer */
    logpkt_memcpy(p, sizeof(mtlk_tsf_event_t), &tsf_event, sizeof(mtlk_tsf_event_t));
    p += sizeof(mtlk_tsf_event_t);

    /* Send message */
    MTLK_ASSERT(p == pdata + sizeof(mtlk_tsf_event_t));
    mtlk_log_new_pkt_release(pbuf);
  }
}

EXPORT_SYMBOL(mtlk_log_tsf_sync_msg);

static void
_mtlk_log_forward_rx_msg(rtlog_app_drv_msghdr_t *hdr, rtlog_app_drv_msgpay_t *data)
{
  /* TODO: verify on HW */
  char *aname;
  size_t anamelen, datalen;
  uint8 *pdata;
  mtlk_log_buf_entry_t *pbuf;

  /* Discard empty message */
  if (sizeof(rtlog_app_drv_msgpay_t) >= hdr->length) {
    return;
  }

  /* Process message from expected application only */
  /* aname = data->name; */
  if (MTLK_ERR_OK != __rtlog_info_app_get_name_by_pid(hdr->pid, &aname)) {
    /* Drop message if application not registered for RT Logger */
    ILOG1_D("Log-message dropped from application (app_pid=%d): "
            "it's not registered for RT Logger.", hdr->pid);
    return;
  }
  /* name is initialized in static rtlog_info_app_array, hence it's NUL-terminated and cannot be NULL */
  anamelen = wave_strlen(aname, IWLWAV_RTLOG_APP_NAME_MAX_LEN) + 1;
/* for debug purpose
  __dump((void *)data, (size_t)hdr->length, "payload:");
*/
  datalen = (size_t)hdr->length - sizeof(*data);

  pbuf = mtlk_log_new_pkt_reserve(LOGPKT_EVENT_HDR_SIZE + datalen, &pdata);

  if (NULL != pbuf) {
    mtlk_log_event_t log_event;
    uint8 *p = pdata;

    MTLK_ASSERT(NULL != pdata);
    MTLK_ASSERT(2047 >= datalen);

    /* Prepare header */
    LOG_MAKE_INFO(log_event, /* ver */ 0, datalen, hdr->wlan_if,
                  0, 0, 0, /* dest, prior, exp, */
                  LOG_INFO_GET_BFILED_OID(hdr->log_info),
                  LOG_INFO_GET_BFILED_GID(hdr->log_info),
                  LOG_INFO_GET_BFILED_FID(hdr->log_info),
                  LOG_INFO_GET_BFILED_LID(hdr->log_info));
    /* Replace user-space time-stamp by Kernel-space time-stamp */
    log_event.timestamp = mtlk_log_get_timestamp();

    /* Put message header to buffer */
    logpkt_memcpy(p, LOGPKT_EVENT_HDR_SIZE, &log_event, LOGPKT_EVENT_HDR_SIZE);
    p += LOGPKT_EVENT_HDR_SIZE;

    /* Put incoming payload */
    logpkt_memcpy(p, datalen, (uint8*)(data + 1), datalen);
    p += datalen;

    /* Put application name
    LOGPKT_PUT_STRING(aname);*/

    /* Send message */
    MTLK_ASSERT(p == pdata + datalen + LOGPKT_EVENT_HDR_SIZE);
    mtlk_log_new_pkt_release(pbuf);
  }
}

void __MTLK_IFUNC
mtlk_log_parse_notification_msg (void *param, void *packet)
{
  rtlog_app_drv_msg_t *msg = (rtlog_app_drv_msg_t *)packet;
  rtlog_app_drv_msghdr_t *hdr = &msg->hdr;
  rtlog_app_drv_msgpay_t *data = &msg->data;

  MTLK_ASSERT(sizeof(rtlog_app_drv_msgpay_t) <= hdr->length);

  ILOG2_SDDD("Notification message received from application: "
             "app_name=%s, app_pid=%d, cmd_id=0x%04x, data_len=%d.",
             data->name, hdr->pid, hdr->cmd_id, hdr->length);

  switch (hdr->cmd_id)
  {
    case IWLWAV_RTLOG_APP_DRV_CMDID_STARTUP:
    {
      /* Register application for RT Logger */
      /* Get and send actual configuration */
      _mtlk_log_send_actual_conf(data->name, hdr->pid);
      break;
    }
    case IWLWAV_RTLOG_APP_DRV_CMDID_CLEANUP:
    {
      /* Unregister application for RT Logger */
      _mtlk_log_unreg_app_on_rx_msg(data->name, hdr->pid);
      break;
    }
    case IWLWAV_RTLOG_APP_DRV_CMDID_LOG:
    {
      /* Forward log-message to network device */
      _mtlk_log_forward_rx_msg(hdr, data);
      break;
    }
    default: break;
  }
}
#endif /* CONFIG_WAVE_RTLOG_REMOTE */

void __MTLK_IFUNC
mtlk_log_set_conf(const char *conf)
{
  char *p, *str, target, *dbg_str;
  uint8 gid, enable;
  int val, i, oid;
  int log_level;
  BOOL log_level_set = FALSE;

  if (!conf || *conf == '\0')
    return;
  /* remove all blanks from the configuration string */
  /* conf is NUL-terminated */
  if (!(p = str = mtlk_osal_mem_alloc(wave_strlen(conf, RSIZE_MAX_STR) + 1, MTLK_MEM_TAG_LOGGER)))
    return;
  while (*conf) {
    if ((*conf == ' ') || (*conf == '\t') || (*conf == '\r') || (*conf == '\n')) {
    } else {
      *p++ = *conf;
    }
    conf++;
  }
  *p = '\0';
  /* read oid now */
  p = str;
  oid = log_read_int(&p);
  if (oid < 0 || oid >= MAX_OID) {
    ELOG_V(LOG_PARSE_ERROR "invalid OID");
    goto end;
  }

  if ((dbg_str = strstr(p, "rdebug="))) {
    if (sscanf(dbg_str, "rdebug=%d", &log_level) != 1) {
      ELOG_V("Missing rdebug value");
      goto end;
    } else {
      if (!LOG_DLVL_ALLOWED(log_level)) {
        ELOG_D("Wrong rdebug value %d", log_level);
        goto end;
      } else {
        ILOG0_DD("Using value of %d for all remote debug levels, OID %d", log_level, oid);
        mtlk_log_set_rdebug_conf(oid, log_level);
        log_level_set = TRUE;
#ifdef CONFIG_WAVE_RTLOG_REMOTE
        _mtlk_log_on_set_conf(oid, (int8)log_level, FALSE);
#endif
      }
    }
  }

  if ((dbg_str = strstr(p, "cdebug="))) {
    if (sscanf(dbg_str, "cdebug=%d", &log_level) != 1) {
      ELOG_V("Missing cdebug value");
      goto end;
    } else {
      if (!LOG_DLVL_ALLOWED(log_level)) {
        ELOG_D("Wrong cdebug value %d", log_level);
        goto end;
      } else {
        ILOG0_DD("Using value of %d for all console debug levels, OID %d", log_level, oid);
        mtlk_log_set_cdebug_conf(oid, log_level);
        log_level_set = TRUE;
#ifdef CONFIG_WAVE_RTLOG_REMOTE
        _mtlk_log_on_set_conf(oid, (int8)log_level, TRUE);
#endif
      }
    }
  }

  if ((dbg_str = strstr(p, "capdebug="))) {
    if (sscanf(dbg_str, "capdebug=%d", &log_level) != 1) {
      ELOG_V("Missing capdebug value");
      goto end;
    } else {
      if (!LOG_DLVL_ALLOWED(log_level)) {
        ELOG_D("Wrong capdebug value %d", log_level);
        goto end;
      } else {
        ILOG0_DD("Using value of %d for all capture debug levels, OID %d", log_level, oid);
        mtlk_log_set_capwap_conf(oid, log_level);
        log_level_set = TRUE;
      }
    }
  }

  if (log_level_set)
    goto end;
  /* read all settings */
  if (*p == '\0') {
    ILOG0_D("Logging disabled for OID %d", oid);
    mtlk_log_set_cdebug_conf(oid, LOG_DLVL_DISABLE);
    mtlk_log_set_rdebug_conf(oid, LOG_DLVL_DISABLE);
    mtlk_log_set_capwap_conf(oid, LOG_DLVL_DISABLE);
    goto end;
  }
  while (*p) {
    if (*p == '-')
      enable = 0;
    else if (*p == '+')
      enable = 1;
    else {
      ELOG_C(LOG_PARSE_ERROR "+ or - expected '%c'", *p);
      break;
    }
    if (*(++p) == '\0') {
      ELOG_V(LOG_PARSE_ERROR "GID is missing");
      goto end;
    }
    /* here comes GID */
    val = log_read_int(&p);
    if (val < 0 || val >= MAX_GID) {
      ELOG_V(LOG_PARSE_ERROR "invalid GID");
      goto end;
    }
    gid = val;
    if (!enable) {
      LOG_SET_DLVL_CON(log_data.dbg_level[oid][gid], LOG_DLVL_DISABLE)
      LOG_SET_DLVL_RT(log_data.dbg_level[oid][gid], LOG_DLVL_DISABLE);
      LOG_SET_DLVL_CAP(log_data.dbg_level[oid][gid], LOG_DLVL_DISABLE);
      continue;
    }
    /* here come target specifier and debug level: c or r or both */
    for (i = 0; i < 2; i++) {
      if (*p == '\0' && !i) {
        ELOG_V(LOG_PARSE_ERROR "target missed");
        break;
      }
      if (*p == '\0' || *p == '+' || *p == '-' || isdigit(*p))
        break;
      val = log_read_dlevel(&p, &target);
      if (val < 0)
        continue;
      if (target == 'c')
        LOG_SET_DLVL_CON(log_data.dbg_level[oid][gid], val)
      else
        LOG_SET_DLVL_RT(log_data.dbg_level[oid][gid], val);
    }
  }
end:
  if (str)
    mtlk_osal_mem_free(str);
}
EXPORT_SYMBOL(mtlk_log_set_conf);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
BOOL __MTLK_IFUNC
mtlk_log_check_version (uint16 major, uint16 minor)
{
  BOOL res = FALSE;

  if (major != RTLOGGER_VER_MAJOR) {
    CERROR(__FILE__, __LINE__, "Incompatible Logger version (%d != %d)", major, RTLOGGER_VER_MAJOR);
  }
  else if (minor != RTLOGGER_VER_MINOR) {
    CWARNING(__FILE__, __LINE__, "Different minor Logger version (%d != %d)", minor, RTLOGGER_VER_MINOR);
    res = TRUE;
  }
  else {
    CINFO(__FILE__, __LINE__, "Logger version %d.%d", major, minor);
    res = TRUE;
  }

  return res;
}
#endif

int __MTLK_IFUNC
mtlk_log_get_flags(int level, int oid, int gid)
{
  mtlk_log_level_t  dbg_lvl;
  int flags = 0;

  if((oid >= MAX_OID || oid < 0) || (gid >= MAX_GID || gid < 0))
    return flags;

  mtlk_osal_lock_acquire(&log_data.data_lock);
  dbg_lvl = log_data.dbg_level[oid][gid];
  if (level <= LOG_GET_DLVL_CON(dbg_lvl))
    flags |= LOG_TARGET_CONSOLE;
  if (level <= LOG_GET_DLVL_RT(dbg_lvl))
    flags |= LOG_TARGET_REMOTE;
  if (level <= LOG_GET_DLVL_CAP(dbg_lvl))
    flags |= LOG_TARGET_CAPWAP;
  mtlk_osal_lock_release(&log_data.data_lock);
  return flags;
}

#ifdef CONFIG_WAVE_RTLOG_REMOTE
mtlk_log_buf_entry_t * __MTLK_IFUNC
mtlk_log_get_free_buf(void)
{
  mtlk_log_buf_entry_t *res;

  mtlk_osal_lock_acquire(&log_data.bufs_free_lock);
  res =  MTLK_LIST_GET_CONTAINING_RECORD(mtlk_dlist_pop_front(&log_data.bufs_free),
                                         mtlk_log_buf_entry_t,
                                         entry);
  if(NULL != res)
  {
    mtlk_rmlock_acquire(&log_data.buff_rm_lock);
    log_data.log_buff_allocations_succeeded++;
  } else {
    log_data.log_buff_allocations_failed++;
  }

  mtlk_osal_lock_release(&log_data.bufs_free_lock);

  return res;
}

mtlk_log_buf_entry_t * __MTLK_IFUNC
mtlk_log_get_ready_buf(void)
{
  mtlk_log_buf_entry_t *res;

  mtlk_osal_lock_acquire(&log_data.bufs_ready_lock);
  res =  MTLK_LIST_GET_CONTAINING_RECORD(mtlk_dlist_pop_front(&log_data.bufs_ready),
                                         mtlk_log_buf_entry_t,
                                         entry);
  mtlk_osal_lock_release(&log_data.bufs_ready_lock);

  return res;
}

int __MTLK_IFUNC
mtlk_log_put_free_buf(mtlk_log_buf_entry_t *buf)
{
  // Note: reset is done in put_free_buf and not in get_free_buf because
  // get is usually called in more time-critical situations
  buf->data_size = 0;
  buf->expired = 0;

  mtlk_osal_lock_acquire(&log_data.bufs_free_lock);
  mtlk_dlist_push_back(&log_data.bufs_free, (mtlk_ldlist_entry_t *) buf);
  mtlk_rmlock_release(&log_data.buff_rm_lock);
  mtlk_osal_lock_release(&log_data.bufs_free_lock);
  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
mtlk_log_put_ready_buf(mtlk_log_buf_entry_t *buf)
{
  mtlk_osal_lock_acquire(&log_data.bufs_ready_lock);
  mtlk_dlist_push_back(&log_data.bufs_ready, (mtlk_ldlist_entry_t *) buf);
  mtlk_osal_lock_release(&log_data.bufs_ready_lock);
}


/* Must Set (re-set) buffer expiration timer if this function returns TRUE,
 * This will be done outside this function as this function is called under
 * spinlock, which is also used in timer callback. */
static BOOL
_mtlk_log_unsafe_active_buf_alloc (mtlk_log_data_t *p_log_data)
{
  p_log_data->unsafe_active_buf = mtlk_log_get_free_buf();

  if (NULL != p_log_data->unsafe_active_buf)
    return TRUE;

  return FALSE;
}

mtlk_log_buf_entry_t * __MTLK_IFUNC
mtlk_log_new_pkt_reserve(uint32 pkt_size, uint8 **ppdata)
{
  mtlk_log_buf_entry_t *buf = NULL;
  mtlk_log_buf_entry_t *ready_buf = NULL;
  mtlk_log_data_t *p_log_data = &log_data;
  BOOL set_log_timer = FALSE;

  MTLK_ASSERT(NULL != ppdata);
  MTLK_ASSERT(pkt_size <= (uint32)buf_size);

  if (!_rtlog_drv_cfg_is_active())
      return NULL;

  mtlk_osal_lock_acquire(&p_log_data->data_lock);

  if (!p_log_data->unsafe_active_buf && !(set_log_timer = _mtlk_log_unsafe_active_buf_alloc(p_log_data))) {
      goto cs_end;
  }

try_again:
#ifdef IWLWAV_RTLOG_ONE_LOG_PER_BUF /* use whole buffer */
  if (buf_space_left(p_log_data->unsafe_active_buf) >= (uint32)buf_size) {
#else
  if (buf_space_left(p_log_data->unsafe_active_buf) >= pkt_size) {
#endif
    buf = p_log_data->unsafe_active_buf;
    ++buf->refcount;
    *ppdata = _mtlk_log_buf_get_data_buffer(buf) + buf->data_size;
    buf->data_size += pkt_size;
  } else {
    /* Not enough space is available to store the packet */
    if (p_log_data->unsafe_active_buf->refcount == 0) {
      ready_buf = p_log_data->unsafe_active_buf;
    }

    if ((set_log_timer = _mtlk_log_unsafe_active_buf_alloc(p_log_data))) {
      goto try_again;
    }
  }
cs_end:
  mtlk_osal_lock_release(&p_log_data->data_lock);

  if (set_log_timer) {
    /* This function can be called in parallel from different contexts
     * Lock must be taken for serializing mtlk_osal_timer_set()
     * This lock MUST NOT be used in timer callback function */
    mtlk_osal_lock_acquire(&p_log_data->timer_lock);
    mtlk_osal_timer_set(&p_log_data->timer, buf_swap_tmout);
    mtlk_osal_lock_release(&p_log_data->timer_lock);
  }

  if (ready_buf != NULL)
    put_ready_buf_and_notify(ready_buf);

  (NULL != buf) ? p_log_data->log_pkt_reservations_succeeded++
                : p_log_data->log_pkt_reservations_failed++;

  return buf;
}

void __MTLK_IFUNC
mtlk_log_new_pkt_release(mtlk_log_buf_entry_t *buf)
{
  mtlk_log_buf_entry_t *ready_buf = NULL;

  mtlk_osal_lock_acquire(&log_data.data_lock);
  /* cs */ ASSERT(buf->refcount >= 1);
  /* cs */ --buf->refcount;
  /* cs */ if (buf->refcount == 0) {
  /* cs */   if (buf != log_data.unsafe_active_buf) {
  /* cs */     // No one can obtain references to this buffer anymore,
  /* cs */     // we were the only owner: buf can be moved to ready pool
  /* cs */     ready_buf = buf;
  /* cs */   } else if (buf->expired) {
  /* cs */     // Timer marked this buffer as expired and no one holds
  /* cs */     // references to it except us: move buf to ready pool
  /* cs */     log_data.unsafe_active_buf = NULL;
  /* cs */     ready_buf = buf;
  /* cs */   }
  /* cs */ }
  mtlk_osal_lock_release(&log_data.data_lock);

  if (ready_buf)
    put_ready_buf_and_notify(ready_buf);
}

BOOL __MTLK_IFUNC
mtlk_log_new_pkt_reserve_multi(uint32 pkt_size, mtlk_log_multi_buffer_t **ppdata)
{
  mtlk_log_multi_buffer_t *current_buffer = NULL;
  mtlk_log_buf_entry_t *ready_buf = NULL;
  mtlk_log_data_t *p_log_data = &log_data;
  uint32 nof_buffers;
  int res = TRUE;
  BOOL set_log_timer = FALSE;

  MTLK_ASSERT(NULL != ppdata);

  if (!_rtlog_drv_cfg_is_active())
    return FALSE;

  nof_buffers = (pkt_size / buf_size) + 3; /* first part + <n> full buffers + last part + null buffer */
  current_buffer = *ppdata = (mtlk_log_multi_buffer_t *)mtlk_osal_mem_alloc
    (nof_buffers * sizeof(mtlk_log_multi_buffer_t), MTLK_MEM_TAG_LOGGER);
  if (NULL == current_buffer) {
    res = FALSE;
    goto end;
  }

  mtlk_osal_lock_acquire(&p_log_data->data_lock);

  if (!p_log_data->unsafe_active_buf && !(set_log_timer = _mtlk_log_unsafe_active_buf_alloc(p_log_data))) {
    goto cs_end;
  }

  while (pkt_size > 0) {
    mtlk_log_buf_entry_t *buf;
    uint32 part_size;

    if (current_buffer == *ppdata                          /* if this is first packet part */
        && p_log_data->unsafe_active_buf                   /* and active buffer exists */
        && p_log_data->unsafe_active_buf->refcount == 0) { /* and active buffer is not in use */
      if (buf_space_left(p_log_data->unsafe_active_buf)    /* then if space left is enough for signal message header */
#ifdef IWLWAV_RTLOG_ONE_LOG_PER_BUF /* use whole buffer */
          >= (uint32)buf_size) {
#else
          >= (LOGPKT_EVENT_HDR_SIZE_VER2 + LOGPKT_SIGNAL_HDR_SIZE)) {
#endif
        /* use space left in current buffer */
        buf       = p_log_data->unsafe_active_buf;
        part_size = MIN(pkt_size, buf_space_left(buf));
      } else { /* mark buffer as ready and get new free buffer */
        ready_buf = p_log_data->unsafe_active_buf;
        p_log_data->unsafe_active_buf = NULL;
        continue;
      }
    } else {
      if (!(set_log_timer = _mtlk_log_unsafe_active_buf_alloc(p_log_data))) {
        p_log_data->log_pkt_reservations_failed++;
        goto cs_end;
      }
      buf       = p_log_data->unsafe_active_buf;
      part_size = MIN(pkt_size, (uint32)buf_size);
    }
    current_buffer->buf       = buf;
    current_buffer->data      = _mtlk_log_buf_get_data_buffer(buf) + buf->data_size;
    current_buffer->data_size = part_size;
    buf->refcount            += 1;
    buf->data_size           += part_size;
    pkt_size                 -= part_size;
    current_buffer           += 1;

    p_log_data->log_pkt_reservations_succeeded++;
  }

cs_end:
  mtlk_osal_lock_release(&p_log_data->data_lock);

  if (set_log_timer) {
    /* This function can be called in parallel from different contexts
     * Lock must be taken for serializing mtlk_osal_timer_set()
     * This lock MUST NOT be used in timer callback function */
    mtlk_osal_lock_acquire(&p_log_data->timer_lock);
    mtlk_osal_timer_set(&p_log_data->timer, buf_swap_tmout);
    mtlk_osal_lock_release(&p_log_data->timer_lock);
  }

  current_buffer->buf       = NULL;
  current_buffer->data      = NULL;
  current_buffer->data_size = 0;

  if (ready_buf)
    put_ready_buf_and_notify(ready_buf);

end:
  return res;
}

void __MTLK_IFUNC
mtlk_log_new_pkt_release_multi(mtlk_log_multi_buffer_t *ppdata)
{
  mtlk_log_multi_buffer_t *current_buffer = ppdata;

  mtlk_osal_lock_acquire(&log_data.data_lock);
  mtlk_osal_lock_acquire(&log_data.bufs_ready_lock);

  while (current_buffer->buf) {
    mtlk_log_buf_entry_t *buf = current_buffer->buf;

    ASSERT(buf->refcount >= 1);
    --buf->refcount;
    if (buf != log_data.unsafe_active_buf) {
      mtlk_dlist_push_back(&log_data.bufs_ready, (mtlk_ldlist_entry_t *)current_buffer->buf);
    }

    ++current_buffer;
  }

  mtlk_osal_lock_release(&log_data.bufs_ready_lock);
  mtlk_osal_lock_release(&log_data.data_lock);

  if (MTLK_ERR_OK != mtlk_log_on_buf_ready()) {
    mtlk_osal_emergency_print("ERROR: Failed to initiate ready logger buffer processing");
  }

  mtlk_osal_mem_free(ppdata);
}

// ---------------
// Local functions
// ---------------

static inline uint32
buf_space_left(mtlk_log_buf_entry_t *buf)
{
  return buf_size - buf->data_size;
}

static void
put_ready_buf_and_notify(mtlk_log_buf_entry_t *buf)
{
  int rslt;

  mtlk_log_put_ready_buf(buf);
  rslt = mtlk_log_on_buf_ready();
  if (rslt != MTLK_ERR_OK) {
    mtlk_osal_emergency_print("ERROR: Failed to initiate ready logger buffer processing");
  }

  // TODO: reschedule the timer here or it can trigger too soon
}

static uint32 __MTLK_IFUNC
log_timer(mtlk_osal_timer_t *timer, mtlk_handle_t clb_usr_data)
{
  mtlk_log_buf_entry_t *ready_buf = NULL;

  mtlk_osal_lock_acquire(&log_data.data_lock);
  /* cs */ if (!log_data.unsafe_active_buf)
  /* cs */   goto cs_end; // Error already reported, just silently ignore
  /* cs */
  /* cs */ // ASSERT if expired buffer wasn't processed by it's last owner
  /* cs */ ASSERT( ! ( log_data.unsafe_active_buf->refcount == 0 &&
  /* cs */             log_data.unsafe_active_buf->expired) );
  /* cs */
  /* cs */ if (log_data.unsafe_active_buf->refcount == 0) {
  /* cs */   if (log_data.unsafe_active_buf->data_size == 0) {
  /* cs */     // The buffer is empty and no one is writing to it now,
  /* cs */     // don't touch it
  /* cs */     goto cs_end;
  /* cs */   }
  /* cs */   ready_buf = log_data.unsafe_active_buf;
  /* cs */   log_data.unsafe_active_buf = mtlk_log_get_free_buf();
  /* cs */   if (!log_data.unsafe_active_buf) {
  /* cs */     goto cs_end;
  /* cs */   }
  /* cs */ } else {
  /* cs */   // Someone is holding a lock on active buffer: schedule the buffer
  /* cs */   // for moving to ready pool after refcount reaches zero
  /* cs */   // (mtlk_log_new_pkt_release function will do this).
  /* cs */   log_data.unsafe_active_buf->expired = 1;
  /* cs */ }
cs_end:
  mtlk_osal_lock_release(&log_data.data_lock);

  if (ready_buf != NULL)
    put_ready_buf_and_notify(ready_buf);

  return (uint32)buf_swap_tmout;
}

static void
_mtlk_log_detach_active_buff(mtlk_log_data_t* plog_data)
{
  mtlk_log_buf_entry_t *detached_buf = NULL;

  mtlk_osal_lock_acquire(&plog_data->data_lock);
  /* cs */ if (!plog_data->unsafe_active_buf)
  /* cs */   goto cs_end;
  /* cs */
  /* cs */ // ASSERT if expired buffer wasn't processed by it's last owner
  /* cs */ ASSERT( ! ( plog_data->unsafe_active_buf->refcount == 0 &&
  /* cs */             plog_data->unsafe_active_buf->expired) );
  /* cs */
  /* cs */ if (plog_data->unsafe_active_buf->refcount == 0) {
  /* cs */   detached_buf = plog_data->unsafe_active_buf;
  /* cs */   plog_data->unsafe_active_buf = NULL;
  /* cs */ } else {
  /* cs */   // Someone is holding a lock on active buffer: schedule the buffer
  /* cs */   // for moving to ready pool after refcount reaches zero
  /* cs */   // (mtlk_log_new_pkt_release function will do this).
  /* cs */   plog_data->unsafe_active_buf->expired = 1;
  /* cs */ }
cs_end:
  mtlk_osal_lock_release(&plog_data->data_lock);

  if (NULL != detached_buf) {
    (detached_buf->data_size > 0) ? put_ready_buf_and_notify(detached_buf)
                                  : mtlk_log_put_free_buf(detached_buf);
  }
}
#endif /* CONFIG_WAVE_RTLOG_REMOTE */

/* ----------------------------------------------------------------- */
/* Sync Log Timestamp with FW TSF */
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED

#define __TIMESPEC_TO_US(ts)     (((ts)->tv_sec * USEC_PER_SEC) + \
                                  (((ts)->tv_nsec + (MSEC_PER_SEC / 2)) / MSEC_PER_SEC))


uint32 mtlk_log_get_timestamp(void)
{
    struct timespec ts;
    uint32 t_usec;

    getnstimeofday(&ts);
    t_usec  = __TIMESPEC_TO_US(&ts);

    return t_usec;
}
EXPORT_SYMBOL(mtlk_log_get_timestamp);

#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */

#ifdef CONFIG_WAVE_RTLOG_REMOTE
/* ----------------------------------------------------------------- */
/* HW RT-Logger API
 */

void __MTLK_IFUNC   mtlk_rtlog_set_iph_check(struct iphdr *iph);    /* FIXME: prototype */

/* === Implementation === */

static void
rtlog_init_drv_cfg (rtlog_drv_cfg_t *drv_cfg)
{
    drv_cfg->is_active  = FALSE;
    drv_cfg->ip_ident   = 0; /* reset */
}

static void
rtlog_init_fw_cfg (rtlog_fw_cfg_t *fw_cfg, uint32 wlan_if)
{
    int i;

    fw_cfg->is_active   = FALSE;
    fw_cfg->wlan_if     = wlan_if;

#if 1    /* FIXME: default value */
    fw_cfg->max_nof_streams  = 1;
    fw_cfg->is_active = TRUE;
#endif

    /* Reset Streams */
    for (i = 0; i < IWLWAV_RTLOG_FW_MAX_STREAM; i++) {
        rtlog_fw_stream_cfg_t *str_cfg = &fw_cfg->stream_cfg[i];

        str_cfg->cfg_id     = IWLWAV_RTLOG_CFG_REQ_ADD_STREAM;
        str_cfg->str_id     = i;
        str_cfg->wlan_if    = wlan_if;
        str_cfg->is_active  = FALSE;
    }

    /* Reset Filters */
    for (i = 0; i < IWLWAV_RTLOG_FW_MAX_FILTER; i++) {
        rtlog_fw_filter_cfg_t *flt_cfg = &fw_cfg->filter_cfg[i];

        flt_cfg->cfg_id     = IWLWAV_RTLOG_CFG_REQ_ADD_FILTER;
        flt_cfg->flt_type  = i;
        flt_cfg->is_active = FALSE;
    }

}

static void
rtlog_init_fw_clb (rtlog_clb_entry_t *fw_clb)
{
    fw_clb->clb = NULL;
    fw_clb->ctx = 0;
}

/* DEBUG: Pre-config Logger Streams for Driver and FW */
#undef  IWLWAV_RTLOG_DEBUG_PRECONFIG_DRIVER
#undef  IWLWAV_RTLOG_DEBUG_PRECONFIG_FW_STR

#if defined(IWLWAV_RTLOG_DEBUG_PRECONFIG_DRIVER) || defined (IWLWAV_RTLOG_DEBUG_PRECONFIG_FW_STR)
#define IWLWAV_RTLOG_DEBUG_PRECONFIG
#endif

#ifdef  IWLWAV_RTLOG_DEBUG_PRECONFIG
int rtlog_cfg_line_parse(char *line);   /* prototype */

/* DEBUG: pre-config for host ip 192.168.1.100 */
static char *_dbg_pre_config_data[] =
{
#ifdef  IWLWAV_RTLOG_DEBUG_PRECONFIG_DRIVER
    "Driver 0x00000000 0x00000000 0x00000000 0x08004500 0x00000000 0x00004011 0x0000c0a8 0x0101c0a8 0x016407d8 0x07d90000 0x0000BEEF",
#endif

#ifdef IWLWAV_RTLOG_DEBUG_PRECONFIG_FW_STR
    "FwMaxStream 0 1",
    "FwAddStream 0 0   0xfff  1024 " /* wlan_if, stream_id, fifos and buf_thr; Packet header on the next line */
           "0x00000000 0x00004041 0x42434445 0x08004500 0x00000000 0x00008011 0x0000c0a8 0x0101c0a8 0x016407d9 0x07d90000 0x0000CAF3",
#endif
};

static void
_rtlog_dbg_pre_config (void)
{
    int  i, res;
    char *line;

    for (i = 0; i < ARRAY_SIZE(_dbg_pre_config_data); i++)
    {
        line = _dbg_pre_config_data[i];
        res = rtlog_cfg_line_parse(line);
        mtlk_osal_emergency_print("%s: n=%d: res=%d, CfgLine: %s", __FUNCTION__, i+1, res, line);
    }
}

#endif /* IWLWAV_RTLOG_DEBUG_PRECONFIG */

/* ----------------------------------------------------------------- */
/* HW RT-Logger network device
 */

extern struct net_device        *rtlog_netdev;

int __MTLK_IFUNC
rtlog_on_init (void)
{
    int i;

    memset(&g_rtlog_cfg, 0, sizeof(g_rtlog_cfg));

    mtlk_osal_lock_init(&g_rtlog_cfg.data_lock);

    //mtlk_osal_lock_acquire(&g_rtlog_cfg.data_lock);

    rtlog_init_drv_cfg(&g_rtlog_cfg.drv_cfg);

    for (i = 0; i < IWLWAV_RTLOG_HW_MAX_WLAN_IF; i++) {
        rtlog_init_fw_cfg(&g_rtlog_cfg.fw_cfg[i], i);
        rtlog_init_fw_clb(&g_rtlog_cfg.fw_clb[i]);
    }

    //mtlk_osal_lock_release(&g_rtlog_cfg.data_lock);

#ifdef  IWLWAV_RTLOG_DEBUG_PRECONFIG
    _rtlog_dbg_pre_config();
#endif

    __rtlog_info_app_reset_entry();

    return MTLK_ERR_OK;
}

/* ----------------------------------------------------------------- */
/* HW RT-Logger configuration
 */

#define IWLWAV_RTLOG_MAX_NUM_PARAMS    21

/* type of function for cfg line parsing */
typedef int (*cfg_parse_t)(int argc, char **argv, uint32 *argn);

typedef struct rtlog_cfg_fmt {
    char        *cmd;
    cfg_parse_t func;
    uint32      nof_args;   /* num of args total */
    uint32      nof_ints;   /* i.e. numberic */
} rtlog_cfg_fmt_t;

static inline void copy_word_array(uint32 *dst, uint32 *src, int size)
{
    while (size > 0) {
        *dst++ = *src++;
        size--;
    }
}

/* convert from host to network byte order */
static inline void htonl_word_array(uint32 *dst, uint32 *src, int size)
{
    while (size > 0) {
        *dst++ = htonl(*src++);
        size--;
    }
}

static BOOL _check_fixed_value(uint32 val, uint32 fixed, char *name)
{
    if (val == fixed) {
        return TRUE;
    } else {
        mtlk_osal_emergency_print("Wrong value of \"%s\": 0x%02x (expected 0x%02x)", name, val, fixed);
        return FALSE;
    }
}

#ifndef from32to16
static inline unsigned short from32to16(unsigned int x)
{
        /* add up 16-bit and 16-bit for 16+c bit */
        x = (x & 0xffff) + (x >> 16);
        /* add up carry.. */
        x = (x & 0xffff) + (x >> 16);
        return x;
}
#endif /* from32to16 */

/* DEBUG */
uint32 vv_from32to16(unsigned int x)
{
    return from32to16(x);
}

/* add a word with carry */
static inline uint32 csum_add_word(uint32 csum, uint32 word)
{
    int carry;

    csum += word;
    carry = (csum < word);
    csum += carry;
    return csum;
}

/* DEBUG */
uint32 vv_csum_add_word(uint32 csum, uint32 word)
{
    return csum_add_word(csum, word);
}

/* Calculate check summ of packet header */
/* static */ uint32 rtlog_cfg_packet_header_chksum(rtlog_pck_hdr_t *pck_hdr)
{
    uint32  csum = 0;

    /* IPv + IHL + TOS */
    csum += *((uint16 *)&pck_hdr->iph);

    /* Flags + Fragment offset */
    csum += pck_hdr->iph.frag_off;

#if 0
    /* IP proto should be IPPROTO_UDP */
    csum += IPPROTO_UDP;
#else
    /* FW requested: TTL + IP proto */
    csum += *((uint16 *)&pck_hdr->iph.ttl);
#endif

    /* Source IP addr */
    csum = csum_add_word(csum, pck_hdr->iph.saddr);

    /* Destination IP addr */
    csum = csum_add_word(csum, pck_hdr->iph.daddr);

    return from32to16(csum);
}

/* check for correctness of packet header */
static BOOL rtlog_cfg_check_packet_header(rtlog_pck_hdr_t *pck_hdr)
{
    BOOL res;
    struct iphdr    *iph = &pck_hdr->iph;

    res = (_check_fixed_value(ntohs(pck_hdr->eth.h_proto),  ETH_P_IP,     "ETH_P_IP") &&
           _check_fixed_value(      pck_hdr->iph.version,   4,            "IP version") &&
           _check_fixed_value(      pck_hdr->iph.ihl,       5,            "IP header len") &&
           _check_fixed_value(      pck_hdr->iph.protocol,  IPPROTO_UDP,  "IPPROTO_UDP") &&
           _check_fixed_value(ntohs(pck_hdr->iph.frag_off), 0,            "IP frag_off")
           // _check_fixed_value(ntohs(pck_hdr->iph.check),    0,            "IP check")
          );

    mtlk_rtlog_set_iph_check(iph);

    return res;
}

static void rtlog_cfg_copy_packet_header(rtlog_pck_hdr_t *dst, rtlog_pck_hdr_t *src)
{
    copy_word_array(dst->words, src->words, IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS);
}

static BOOL rtlog_cfg_set_packet_header(rtlog_pck_hdr_t *pck_hdr, uint32 *words)
{
    BOOL res;
    rtlog_pck_hdr_t temp;

    /* convert from host to network byte order */
    htonl_word_array(temp.words, words, ARRAY_SIZE(temp.words));

    /* check the header and copy it if Ok */
    res = rtlog_cfg_check_packet_header(&temp);
    if (res) {
        copy_word_array(pck_hdr->words, temp.words, ARRAY_SIZE(temp.words));
    } else {
        mtlk_osal_emergency_print("Wrong packet header");
    }

    return res;
}

#ifdef MTLK_USE_DIRECTPATH_API
typedef struct _rtlog_ppa_stats_t
{
  uint32 sent_accepted;
  uint32 sent_rejected;
  uint32 recv_for_rx;
  uint32 recv_for_tx;
  uint32 recv_dropped;
} rtlog_ppa_stats_t;

typedef struct _rtlog_ppa_t
{
  PPA_DIRECTPATH_CB     clb;
  PPA_SUBIF             if_id;
  rtlog_ppa_stats_t     stats;
  BOOL            is_ppa_enabled;
  /** Special radio net_device for PPA */
  PPA_NETIF             *radio;
} rtlog_ppa_t;
#endif /* MTLK_USE_DIRECTPATH_API */

typedef struct rtlog_ndev_cfg {
    ip4_addr_t      dev_ip4;
    mac_addr_t      dev_mac;
#ifdef MTLK_USE_DIRECTPATH_API
    rtlog_ppa_t     ppa;
#endif  /* MTLK_USE_DIRECTPATH_API */
} rtlog_ndev_cfg_t;

/* Global data */
rtlog_ndev_cfg_t    g_rtlog_ndev_cfg;


#ifdef MTLK_USE_DIRECTPATH_API

#define IWLWAV_RTLOG_DEBUG_PPA_XMIT 0    /* Debug printout packets from PPA */

extern void dump_skb(struct sk_buff *skb);

static int
_rtlog_ppa_start_xmit (struct net_device *rx_dev,
                       struct net_device *tx_dev,
                       struct sk_buff *skb,
                       int len)
{

#if IWLWAV_RTLOG_DEBUG_PPA_XMIT
    mtlk_osal_emergency_print("%s: rx_dev %px, tx_dev %px, skb %px, len %d", __FUNCTION__, rx_dev, tx_dev, skb, len);
    dump_skb(skb);
#endif

    /* statistic */
    if (tx_dev != NULL) {
        ++g_rtlog_ndev_cfg.ppa.stats.recv_for_tx;
    } else if (rx_dev != NULL) {
        ++g_rtlog_ndev_cfg.ppa.stats.recv_for_rx;
    } else {
        ++g_rtlog_ndev_cfg.ppa.stats.recv_dropped;
    }

    /* just drop */
    dev_kfree_skb_any(skb);

    return 0;
}

BOOL __MTLK_IFUNC
_rtlog_ppa_is_available (void)
{
  return (ppa_hook_directpath_ex_register_dev_fn != NULL);
}

BOOL __MTLK_IFUNC
_rtlog_ppa_is_registered (void)
{
  return (NULL != g_rtlog_ndev_cfg.ppa.clb.rx_fn);
}


static int _rtlog_ppa_port_id_get (void)
{
  uint32 res;
  PPA_DIRECTPATH_CB clb;

  if (g_rtlog_ndev_cfg.ppa.radio == NULL) {
    g_rtlog_ndev_cfg.ppa.radio = mtlk_osal_mem_alloc(sizeof(PPA_NETIF),
      MTLK_MEM_TAG_PPA);
    if (g_rtlog_ndev_cfg.ppa.radio == NULL)
      return -1;
    memset(g_rtlog_ndev_cfg.ppa.radio, 0, sizeof(PPA_NETIF));
    wave_strcopy(g_rtlog_ndev_cfg.ppa.radio->name, rtlog_ppa_netdev_name,
                 sizeof(g_rtlog_ndev_cfg.ppa.radio->name));
    g_rtlog_ndev_cfg.ppa.if_id.port_id = -1;
    g_rtlog_ndev_cfg.ppa.if_id.subif = -1;
    memset(&clb, 0, sizeof(clb));
    /* if rx_fn is NULL, registration fails with error invalid argument */
    clb.rx_fn = _rtlog_ppa_start_xmit;
    res = ppa_hook_directpath_ex_register_dev_fn(&g_rtlog_ndev_cfg.ppa.if_id,
        g_rtlog_ndev_cfg.ppa.radio, &clb,
        PPA_F_DIRECTPATH_REGISTER | PPA_F_DIRECTPATH_ETH_IF |
        PPE_DIRECTPATH_LEGACY_NON_LEGACY);
    if (res != PPA_SUCCESS) {
      ELOG_SD("Can't register PPA radio device %s (err=%d)", g_rtlog_ndev_cfg.ppa.radio->name, res);
      mtlk_osal_mem_free(g_rtlog_ndev_cfg.ppa.radio);
      g_rtlog_ndev_cfg.ppa.radio = NULL;
      return -1;
    }
    ILOG0_SDD("PPA radio device %s registered (port id=%d, subif %d)",
      g_rtlog_ndev_cfg.ppa.radio->name,
      g_rtlog_ndev_cfg.ppa.if_id.port_id, g_rtlog_ndev_cfg.ppa.if_id.subif);
  }
  return g_rtlog_ndev_cfg.ppa.if_id.port_id;
}

static void _rtlog_ppa_port_id_release (void)
{
  uint32 res;

  if (g_rtlog_ndev_cfg.ppa.radio != NULL) {
    g_rtlog_ndev_cfg.ppa.if_id.subif = -1;
    res = ppa_hook_directpath_ex_register_dev_fn(&g_rtlog_ndev_cfg.ppa.if_id,
      g_rtlog_ndev_cfg.ppa.radio, NULL,
      PPE_DIRECTPATH_LEGACY_NON_LEGACY/*PPA_F_DIRECTPATH_DEREGISTER*/);
    if (res == PPA_SUCCESS) {
      ILOG0_D("PPA radio device unregistered (port id=%d)",
        g_rtlog_ndev_cfg.ppa.if_id.port_id);
      mtlk_osal_mem_free(g_rtlog_ndev_cfg.ppa.radio);
      g_rtlog_ndev_cfg.ppa.radio = NULL;
    }
    else {
      ELOG_DD("Can't unregister PPA radio device (port id=%d, err=%d)",
        g_rtlog_ndev_cfg.ppa.if_id.port_id, res);
    }
  }
}

int __MTLK_IFUNC
rtlog_ppa_register (void)
{
  int    res = MTLK_ERR_UNKNOWN;
  uint32 ppa_res;

  if (g_rtlog_ndev_cfg.ppa.is_ppa_enabled) {
       mtlk_osal_emergency_print("PPA is already enabled");
       return MTLK_ERR_OK;  /* ERR_BUSY ??? */
  }

  if (_rtlog_ppa_is_available() == FALSE) {
    res = MTLK_ERR_NOT_SUPPORTED;
    goto end;
  }

  g_rtlog_ndev_cfg.ppa.if_id.subif = -1;
  g_rtlog_ndev_cfg.ppa.if_id.port_id = _rtlog_ppa_port_id_get();
  if (g_rtlog_ndev_cfg.ppa.if_id.port_id == -1) {
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  memset(&g_rtlog_ndev_cfg.ppa.clb, 0, sizeof(g_rtlog_ndev_cfg.ppa.clb));

  g_rtlog_ndev_cfg.ppa.clb.rx_fn = _rtlog_ppa_start_xmit;
  /* get sub-interface id */
  ppa_res =
    ppa_hook_directpath_ex_register_dev_fn(&g_rtlog_ndev_cfg.ppa.if_id,
                                        rtlog_netdev, &g_rtlog_ndev_cfg.ppa.clb,
                                        PPA_F_DIRECTPATH_REGISTER |
                                        PPA_F_DIRECTPATH_ETH_IF |
                                        PPE_DIRECTPATH_LEGACY_NON_LEGACY);

  if (ppa_res != PPA_SUCCESS)
  {
    g_rtlog_ndev_cfg.ppa.clb.rx_fn = NULL;
    ELOG_D("Can't register PPA device function (err=%d)", ppa_res);
    res = MTLK_ERR_UNKNOWN;
    _rtlog_ppa_port_id_release();
    goto end;
  }

  /* At this point we can send packets via PPA */
  g_rtlog_ndev_cfg.ppa.is_ppa_enabled = TRUE;

  mtlk_osal_emergency_print("PPA device function is registered (port id=%d, sub-interface id=%d)",
    g_rtlog_ndev_cfg.ppa.if_id.port_id, g_rtlog_ndev_cfg.ppa.if_id.subif);
  res = MTLK_ERR_OK;

end:
  return res;
}

void __MTLK_IFUNC
rtlog_ppa_unregister (void)
{
  uint32 ppa_res;

  if (!g_rtlog_ndev_cfg.ppa.is_ppa_enabled) {
       mtlk_osal_emergency_print("PPA is already disabled");
       return;
  }

  /* Stop sending packets before unregistration */
  g_rtlog_ndev_cfg.ppa.is_ppa_enabled = FALSE;

  ppa_res =
    ppa_hook_directpath_ex_register_dev_fn(&g_rtlog_ndev_cfg.ppa.if_id,
                                        rtlog_netdev, NULL,
                                        PPE_DIRECTPATH_LEGACY_NON_LEGACY);
  if (ppa_res == PPA_SUCCESS) {
    mtlk_osal_emergency_print("PPA device function is unregistered (port id=%d, sub-interface id=%d)",
      g_rtlog_ndev_cfg.ppa.if_id.port_id, g_rtlog_ndev_cfg.ppa.if_id.subif);
    g_rtlog_ndev_cfg.ppa.clb.rx_fn = NULL;
    _rtlog_ppa_port_id_release();
  }
  else {
    ELOG_D("Can't unregister PPA device function (err=%d)", ppa_res);
    g_rtlog_ndev_cfg.ppa.is_ppa_enabled = TRUE;
  }
}

int rtlog_cfg_set_ndev_ppa(int argc, char **argv, uint32 *argn)
{
    BOOL    action;

    action = !!(*argn);

    if (action) { /* On */
        rtlog_ppa_register();
    } else { /* Off */
        rtlog_ppa_unregister();
    }

    return 0;
}

#endif  /* MTLK_USE_DIRECTPATH_API */

/* FIXME: should be moved into mtlkndev.c */

#ifdef MTLK_USE_DIRECTPATH_API
BOOL
rtlog_ppa_send_packet(struct sk_buff *skb)
{
    int     res = MTLK_ERR_OK;
    BOOL    ppa_flag;

    ppa_flag = (ppa_hook_directpath_ex_send_fn && _rtlog_ppa_is_registered() &&
      g_rtlog_ndev_cfg.ppa.is_ppa_enabled);

    if (ppa_flag) {

    /* set raw pointer for proper work if directpath is disabled */
    skb_reset_mac_header(skb);

    /* send packets to the ppa */
    if (ppa_hook_directpath_ex_send_fn(&g_rtlog_ndev_cfg.ppa.if_id,
                                    skb, skb->len,
                                    PPE_DIRECTPATH_LEGACY_NON_LEGACY) == PPA_SUCCESS) {

      ++g_rtlog_ndev_cfg.ppa.stats.sent_accepted;
      goto end;
    }

    res = MTLK_ERR_UNKNOWN;
    ++g_rtlog_ndev_cfg.ppa.stats.sent_rejected;

    goto end;

    /** \note: In PPA mode we should not cleanup packet on sending failure.
     *    Also frame can not be transferred in to the IP stack.
     *    Otherwise packet will corrupt per CPU queue with NULL pointer access during dequeue.
     *  \tocheck: Can be a PPA problem or not a problem at all. */
  }

end:
    if (MTLK_ERR_OK != res) {
        mtlk_osal_emergency_print("PPA failure");
    }

    return ppa_flag;
}
#endif /* MTLK_USE_DIRECTPATH_API */

/* ---------------------------------------------------- */

void __MTLK_IFUNC
rtlog_before_cleanup (void)
{
#ifdef MTLK_USE_DIRECTPATH_API
    rtlog_ppa_unregister();
#endif /* MTLK_USE_DIRECTPATH_API */
}

void __MTLK_IFUNC
rtlog_on_cleanup (void)
{
    //rtlog_netdev_cleanup();

    /* ... */

    g_rtlog_cfg.drv_cfg.is_active = FALSE;

    mtlk_osal_lock_cleanup(&g_rtlog_cfg.data_lock);
}

/* ---------------------------------------------------- */

/* Driver cfg */
int rtlog_cfg_set_drv(int argc, char **argv, uint32 *argn)
{
    rtlog_drv_cfg_t    *drv_cfg = &g_rtlog_cfg.drv_cfg;

    if (!rtlog_cfg_set_packet_header(&drv_cfg->pck_hdr, argn))
        return -1;

    drv_cfg->is_active = TRUE;

    return 0;
}


static BOOL rtlog_cfg_check_wlan_if(uint32 wlan_if)
{
    if (wlan_if > IWLWAV_RTLOG_HW_MAX_WLAN_IF - 1) {
        mtlk_osal_emergency_print("Invalid wlan_if %d (expected 0 .. %d)", wlan_if, IWLWAV_RTLOG_HW_MAX_WLAN_IF - 1);
        return FALSE;
    }

    return TRUE;
}

static BOOL rtlog_cfg_fw_check_flt_num(uint32 num)
{
    if (num > IWLWAV_RTLOG_FW_MAX_FILTER - 1) {
        mtlk_osal_emergency_print("Invalid FilterType %d (expected 0 .. %d)", num, IWLWAV_RTLOG_FW_MAX_FILTER - 1);
        return FALSE;
    }

    return TRUE;
}

/* checks num in range */
static BOOL rtlog_cfg_fw_check_stream_num(int num, int min, int max)
{
    if (min <= num && num <= max) {
        return TRUE;
    } else {
        mtlk_osal_emergency_print("Invalid stream num %d (expected %d .. %d)", num, min, max);
        return FALSE;
    }
}

static int
rtlog_fw_cfg_notify_changes (int wlan_if, rtlog_cfg_id_t req_id, void *data, uint32 size)
{
    rtlog_clb_entry_t *fw_clb;

    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return MTLK_ERR_NO_RESOURCES;
    }

    fw_clb = &g_rtlog_cfg.fw_clb[wlan_if];
    if (fw_clb->clb) {
        fw_clb->clb(fw_clb->ctx, req_id, data, size);
    } else {
        mtlk_osal_emergency_print("WARN: wlan_if %d: clb is not set", wlan_if);
    }

    return MTLK_ERR_OK;
}

int rtlog_cfg_set_fw_max_stream(int argc, char **argv, uint32 *argn)
{
    rtlog_fw_cfg_t  *fw_cfg;
    uint32           wlan_if;
    uint32           num;

    wlan_if = *argn++;
    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return -1;
    }

    /* values from 1 to IWLWAV_RTLOG_FW_MAX_STREAM are allowed */
    num = *argn;
    if (!rtlog_cfg_fw_check_stream_num(num, 1, IWLWAV_RTLOG_FW_MAX_STREAM)) {
        return -1;
    }

    fw_cfg = &g_rtlog_cfg.fw_cfg[wlan_if];
    fw_cfg->max_nof_streams = num;

    fw_cfg->is_active = TRUE;

    return 0;
}

int rtlog_cfg_set_fw_rem_stream(int argc, char **argv, uint32 *argn)
{
    rtlog_fw_cfg_t         *fw_cfg;
    rtlog_fw_stream_cfg_t  *str_cfg;
    uint32                  wlan_if;
    uint32                  num;

    wlan_if = *argn++;
    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return -1;
    }

    fw_cfg = &g_rtlog_cfg.fw_cfg[wlan_if];

    /* Note: we sure that fw_cfg->max_nof_streams > 0 */
    /* stream_id values from 0 to (max_nof_streams - 1) are allowed */
    num = *argn;
    if (!rtlog_cfg_fw_check_stream_num(num, 0, fw_cfg->max_nof_streams - 1)) {
        return -1;
    }

    str_cfg = &fw_cfg->stream_cfg[num];

    if (str_cfg->is_active) {
        rtlog_fw_stream_rem_t   cfg_data;

        cfg_data.cfg_id = IWLWAV_RTLOG_CFG_REQ_REM_STREAM;
        cfg_data.str_id = str_cfg->str_id;
        rtlog_fw_cfg_notify_changes (wlan_if, IWLWAV_RTLOG_CFG_REQ_REM_STREAM, &cfg_data, sizeof(cfg_data));

        str_cfg->is_active = FALSE;
    }

    return 0;
}

/* FIXME: work around for HW csum calculation. Network endian is expected for csum value */
#define IWLWAV_RTLOG_HW_CSUM_NETWORK_ENDIAN

static void _rtlog_cfg_set_fw_stream_csum(rtlog_fw_stream_cfg_t *str_cfg)
{
    str_cfg->chk_sum = rtlog_cfg_packet_header_chksum(&str_cfg->pck_hdr);

#ifdef  IWLWAV_RTLOG_HW_CSUM_NETWORK_ENDIAN
    {
        uint16 cs16;

        cs16 = str_cfg->chk_sum;
        cs16 = htons(cs16);
#ifdef CONFIG_WAVE_DEBUG
        mtlk_osal_emergency_print("swPreCalcChecksum 0x%04x (0x%04x in network endian)", str_cfg->chk_sum, cs16);
#endif
        str_cfg->chk_sum = cs16;
    }
#else
#ifdef CONFIG_WAVE_DEBUG
    mtlk_osal_emergency_print("swPreCalcChecksum 0x%04x", str_cfg->chk_sum);
#endif
#endif
}

int rtlog_cfg_set_fw_add_stream(int argc, char **argv, uint32 *argn)
{
    rtlog_fw_cfg_t         *fw_cfg;
    rtlog_fw_stream_cfg_t  *str_cfg;
    uint32                  wlan_if;
    uint32                  num;

    wlan_if = *argn++;
    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return -1;
    }

    fw_cfg = &g_rtlog_cfg.fw_cfg[wlan_if];

    /* stream_id values from 0 to (max_nof_streams - 1) are allowed */
    num = *argn++;
    if (!rtlog_cfg_fw_check_stream_num(num, 0, fw_cfg->max_nof_streams - 1)) {
        return -1;
    }

    str_cfg = &fw_cfg->stream_cfg[num];
    str_cfg->wlan_if = wlan_if;

    if (str_cfg->is_active) {
        return -1;
    }

    /* AssignedFIFOs bitmap */
    num = *argn++;
            /* to be checked */
    str_cfg->fifos = num;

    /* Buffer threshold */
    num = *argn++;
            /* to be checked */
    str_cfg->buf_thr = num;

    if (!rtlog_cfg_set_packet_header(&str_cfg->pck_hdr, argn))
        return -1;

    _rtlog_cfg_set_fw_stream_csum(str_cfg);

    str_cfg->is_active = TRUE;

    rtlog_fw_cfg_notify_changes (wlan_if, IWLWAV_RTLOG_CFG_REQ_ADD_STREAM, str_cfg, sizeof(*str_cfg));

    return 0;
}

/* FwRemFilter: WlanIf, FilterType */
int rtlog_cfg_set_fw_rem_filter(int argc, char **argv, uint32 *argn)
{
    rtlog_fw_cfg_t         *fw_cfg;
    rtlog_fw_filter_cfg_t  *flt_cfg;
    uint32                  wlan_if;
    uint32                  num;

    wlan_if = *argn++;
    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return -1;
    }

    fw_cfg = &g_rtlog_cfg.fw_cfg[wlan_if];

    /* Check FilterType */
    num = *argn++;
    if (!rtlog_cfg_fw_check_flt_num(num)) {
        return -1;
    }

    flt_cfg = &fw_cfg->filter_cfg[num];

    if (flt_cfg->is_active) {
        rtlog_fw_filter_rem_t   cfg_data;

        cfg_data.cfg_id   = IWLWAV_RTLOG_CFG_REQ_REM_FILTER;
        cfg_data.flt_type = flt_cfg->flt_type;
        rtlog_fw_cfg_notify_changes (wlan_if, IWLWAV_RTLOG_CFG_REQ_REM_FILTER, &cfg_data, sizeof(cfg_data));

        flt_cfg->is_active = FALSE;
    }

    return 0;
}

/* FwAddFilter: WlanIf, FilterType, LogLevel OR ModulesBitmap */
int rtlog_cfg_set_fw_add_filter(int argc, char **argv, uint32 *argn)
{
    rtlog_fw_cfg_t         *fw_cfg;
    rtlog_fw_filter_cfg_t  *flt_cfg;
    uint32                  wlan_if;
    uint32                  num;

    wlan_if = *argn++;
    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return -1;
    }

    fw_cfg = &g_rtlog_cfg.fw_cfg[wlan_if];

    /* Check FilterType */
    num = *argn++;
    if (!rtlog_cfg_fw_check_flt_num(num)) {
        return -1;
    }

    flt_cfg = &fw_cfg->filter_cfg[num];
    flt_cfg->flt_type = num;

    /* LogLevel OR ModulesBitmap */
    num = *argn++;
    if (flt_cfg->flt_type == IWLWAV_RTLOG_FW_FLT_TYPE_LOG_LEVEL) {
        flt_cfg->log_lvl = num; /* to be checked */
        flt_cfg->bitmap  = 0;   /* reset */
    } else if (flt_cfg->flt_type == IWLWAV_RTLOG_FW_FLT_TYPE_MOD_BITMAP) {
        flt_cfg->log_lvl = 0;   /* reset */
        flt_cfg->bitmap  = num;
    }

    flt_cfg->is_active = TRUE;

    rtlog_fw_cfg_notify_changes (wlan_if, IWLWAV_RTLOG_CFG_REQ_ADD_FILTER, flt_cfg, sizeof(*flt_cfg));

    return 0;
}

/* FwAddFilter: WlanIf, FilterType, LogLevel OR ModulesBitmap */

/******************************************************************************/
/*!  \brief      Get interface number from name.
 *
 *  \param[in]   str     Pointer to string.
 *
 *  \return      An interface number.
 */

uint32 __MTLK_IFUNC mtlk_rtlog_get_wlanif_from_string(char *str); /* prototype */

int rtlog_cfg_set_log_hw_fifo(int argc, char **argv, uint32 *argn)
{
    rtlog_hw_fifo_cfg_t    *hw_fifo_cfg;
    uint32                  wlan_if;
    int                     i;

    wlan_if = mtlk_rtlog_get_wlanif_from_string(argv[0]);
    argn++;

    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return -1;
    }

    hw_fifo_cfg = &g_rtlog_cfg.fw_cfg[wlan_if].hw_fifo_cfg;

    for (i = 0; i < IWLWAV_RTLOG_HW_FIFOS; i++) {
        hw_fifo_cfg->config[i] = *argn++;
    }

    /* without notification */

    return 0;
}

/*---------------------------------------------*/
/* Unified format for FW/Driver/Config/Hostapd
*/

#define IWLWAV_RTLOG_CFG_STREAM_ARGS       3 /* CID, wlan_if, stream_num */
#define IWLWAV_RTLOG_CFG_PCK_HDR_ARGS      6 /* Packet header */
#define IWLWAV_RTLOG_CFG_FW_SPEC_ARGS      3 /* mode, buf_thr, fifos */

typedef enum {
    IWLWAV_RTLOG_COMPONENT_ID_FW = 0,
    IWLWAV_RTLOG_COMPONENT_ID_DRIVER,

#if 0 /* is not supporet yet */
    IWLWAV_RTLOG_COMPONENT_ID_CONFIG,
    IWLWAV_RTLOG_COMPONENT_ID_HOSTAPD,
#endif

    IWLWAV_RTLOG_COMPONENT_ID_LAST
} rtlog_component_id_t;


/*---------------------------------------------*/

static BOOL rtlog_cfg_check_cid(rtlog_component_id_t cid)
{
    if (cid < IWLWAV_RTLOG_COMPONENT_ID_LAST) {
        return TRUE;
    } else {
        mtlk_osal_emergency_print("Invalid ComponentID %d (expected 0 .. %d)", cid, IWLWAV_RTLOG_COMPONENT_ID_LAST - 1);
        return FALSE;
    }
}

/* Check stream_num by CID and wlan_if */
static BOOL rtlog_cfg_log_check_stream_num(rtlog_component_id_t cid, uint32 wlan_if, uint32 str_num)
{
    rtlog_fw_cfg_t         *fw_cfg;
    uint32                  str_max = 0;

    if (rtlog_cfg_check_cid(cid) &&
        rtlog_cfg_check_wlan_if(wlan_if))
    {
        /* Get str_max from FW cfg */
        /* Note: we sure that fw_cfg->max_nof_streams > 0 */
        /* stream_id values from 0 to (max_nof_streams - 1) are allowed */
        if (cid == IWLWAV_RTLOG_COMPONENT_ID_FW) {
            fw_cfg = &g_rtlog_cfg.fw_cfg[wlan_if];
            str_max = fw_cfg->max_nof_streams - 1;
        }

        return rtlog_cfg_fw_check_stream_num(str_num, 0, str_max);
    }

    return FALSE;
}

static BOOL
_mtlk_parse_mac_addr(const char *addr_str, uint8 *addr_bin)
{
#define _MAC_ARG_SCAN(x) &(x)[0],&(x)[1],&(x)[2],&(x)[3],&(x)[4],&(x)[5]
  uint32 mac_addr_bytes[ETH_ALEN];

  if(ETH_ALEN > sscanf(addr_str, MAC_PRINTF_FMT, _MAC_ARG_SCAN(mac_addr_bytes)))
  {
    mtlk_osal_emergency_print("Invalid MAC addr: %s", addr_str);
    return FALSE;
  }
  else
  {
    int i;
    for(i = 0; i < ETH_ALEN; i++)
      addr_bin[i] = mac_addr_bytes[i];

    return TRUE;
  }
#undef _MAC_ARG_SCAN
}

/* FIXME: move to rtlog.h */
#define IP_ADDRESS(a, b, c, d)     ((d) | (c) << 8 | (b) << 16 | (a) << 24)
typedef uint16      udp_port_t;

static BOOL
_mtlk_parse_ip4_addr(const char *addr_str, ip4_addr_t *ip_addr)
{
  uint32    a, b, c, d; /* IP4 addr */

  if(4 > sscanf(addr_str, IP4_PRINTF_FMT, &a, &b, &c, &d))
  {
    goto FAIL;
  }
  else
  {
    /* validate */
    if (255 < (a | b | c | d))
        goto FAIL;

    *ip_addr = htonl(IP_ADDRESS(a, b, c, d));

    return TRUE;
  }

FAIL:
  mtlk_osal_emergency_print("Invalid IP4 addr: %s", addr_str);

  return FALSE;
}

static BOOL
_mtlk_parse_udp_port(uint32 val, udp_port_t *port)
{
    /* FIXME: set ports range properly */
    if (/* min */ 1024 <= val && val <= 0xffff /* max */) {
        *port = htons((udp_port_t)val);
        return TRUE;
    } else {
        mtlk_osal_emergency_print("Invalid UDP port number %d", val);
        return FALSE;
    }
}

static BOOL rtlog_cfg_parse_packet_header(rtlog_pck_hdr_t *pck_hdr, char **argv, uint32 *argn)
{
    rtlog_pck_hdr_t tmp_hdr;

    memset(&tmp_hdr, 0, sizeof(rtlog_pck_hdr_t));

    if (
        _mtlk_parse_ip4_addr(argv[0], &tmp_hdr.iph.saddr)     &&
        _mtlk_parse_mac_addr(argv[1], &tmp_hdr.eth.h_source[0]) &&
        /* port numbers already extracted from string */
        _mtlk_parse_udp_port(argn[2], &tmp_hdr.udph.source) &&

        _mtlk_parse_ip4_addr(argv[3], &tmp_hdr.iph.daddr)     &&
        _mtlk_parse_mac_addr(argv[4], &tmp_hdr.eth.h_dest[0]) &&
        _mtlk_parse_udp_port(argn[5], &tmp_hdr.udph.dest))
    {
        /* copy to dest */
        rtlog_cfg_copy_packet_header(pck_hdr, &tmp_hdr);
        return TRUE;
    }

    return FALSE;
}

static void rtlog_cfg_fillup_packet_header(rtlog_pck_hdr_t *pck_hdr)
{
    struct iphdr    *iph = &pck_hdr->iph;

    /* all fields are cleared before, set only non-zero */
    pck_hdr->eth.h_proto  = htons(ETH_P_IP);
    pck_hdr->iph.version  = 4;      /* IP version */
    pck_hdr->iph.ihl      = 5;      /* IP header len */
    pck_hdr->iph.protocol = IPPROTO_UDP;
    pck_hdr->iph.tos      = 0x00;   /* TypeOfService */
    pck_hdr->iph.ttl      = 0x40;   /* TTL */

    mtlk_rtlog_set_iph_check(iph);

    /* padding: 0xCAFE */
    pck_hdr->pad2[0]      = 0xCA;
    pck_hdr->pad2[1]      = 0xFE;
}

int rtlog_cfg_set_log_add_stream(int argc, char **argv, uint32 *argn)
{
    rtlog_component_id_t       cid;
    uint32                  wlan_if;
    uint32                  str_num;
    rtlog_fw_cfg_t         *fw_cfg;
    rtlog_fw_stream_cfg_t  *str_cfg = NULL;
    rtlog_pck_hdr_t        *pck_hdr = NULL;
    BOOL                   *ptr_flag = NULL;

    /* Params: CID, WLAN, StreamNum */
    cid     = argn[0];
    wlan_if = mtlk_rtlog_get_wlanif_from_string(argv[1]);
    str_num = argn[2];

    if (!rtlog_cfg_log_check_stream_num(cid, wlan_if, str_num)) {
        goto fail;
    }

    argc -= IWLWAV_RTLOG_CFG_STREAM_ARGS;  /* args are passed */
    argn += IWLWAV_RTLOG_CFG_STREAM_ARGS;
    argv += IWLWAV_RTLOG_CFG_STREAM_ARGS;

    switch(cid)
    {
    case IWLWAV_RTLOG_COMPONENT_ID_FW:
      {
        fw_cfg  = &g_rtlog_cfg.fw_cfg[wlan_if];
        str_cfg = &fw_cfg->stream_cfg[str_num];

        ptr_flag = &str_cfg->is_active;
        pck_hdr  = &str_cfg->pck_hdr;
      }
      break;

    case IWLWAV_RTLOG_COMPONENT_ID_DRIVER:
      {
        rtlog_drv_cfg_t         *drv_cfg = &g_rtlog_cfg.drv_cfg;

        ptr_flag = &drv_cfg->is_active;
        pck_hdr  = &drv_cfg->pck_hdr;
      }
      break;

    default:
        /* Should not be here */
        goto fail;
    } /* end of switch */

    /* Reset active flag */
    *ptr_flag = FALSE;

    if (argc < IWLWAV_RTLOG_CFG_PCK_HDR_ARGS)
        goto few_args;

    /* get packet header */
    if (!rtlog_cfg_parse_packet_header(pck_hdr, argv, argn))
        goto fail;

    rtlog_cfg_fillup_packet_header(pck_hdr);

    argc -= IWLWAV_RTLOG_CFG_PCK_HDR_ARGS;  /* args are passed */
    argn += IWLWAV_RTLOG_CFG_PCK_HDR_ARGS;
    argv += IWLWAV_RTLOG_CFG_PCK_HDR_ARGS;

    /* get additional params and set active flags */
    switch(cid)
    {
    case IWLWAV_RTLOG_COMPONENT_ID_FW:
      if (!str_cfg->is_active)
      {
        if (argc != IWLWAV_RTLOG_CFG_FW_SPEC_ARGS)
            goto inv_args;

        str_cfg->wlan_if = wlan_if;
        str_cfg->str_id  = str_num;
        str_cfg->mode    = *argn++;
        str_cfg->buf_thr = *argn++;
        str_cfg->fifos   = *argn++;

        mtlk_osal_emergency_print("Switch On FW wlan %d Stream %d", wlan_if, str_num);
        _rtlog_cfg_set_fw_stream_csum(str_cfg);

        str_cfg->is_active = TRUE;

        rtlog_fw_cfg_notify_changes (wlan_if, IWLWAV_RTLOG_CFG_REQ_ADD_STREAM, str_cfg, sizeof(*str_cfg));
      }
      break;

    case IWLWAV_RTLOG_COMPONENT_ID_DRIVER:
      {
        mtlk_osal_emergency_print("Switch On Driver Stream");
      }
      break;

    default:
        /* Should not be here */
        goto fail;
    } /* end of switch */

    /* Set active flag */
    *ptr_flag = TRUE;

    return 0;

inv_args:
few_args:
    mtlk_osal_emergency_print("Invalid number of args");
fail:
    return -1;
}

int rtlog_cfg_set_log_del_stream(int argc, char **argv, uint32 *argn)
{
    rtlog_component_id_t       cid;
    uint32                  wlan_if;
    uint32                  str_num;

    /* Params: CID, WLAN, StreamNum */
    cid     = argn[0];
    wlan_if = mtlk_rtlog_get_wlanif_from_string(argv[1]);
    str_num = argn[2];

    if (!rtlog_cfg_log_check_stream_num(cid, wlan_if, str_num)) {
        return -1;
    }

    switch(cid)
    {
    case IWLWAV_RTLOG_COMPONENT_ID_FW:
      {
        rtlog_fw_cfg_t          *fw_cfg  = &g_rtlog_cfg.fw_cfg[wlan_if];
        rtlog_fw_stream_cfg_t   *str_cfg = &fw_cfg->stream_cfg[str_num];

        if (str_cfg->is_active) {
            rtlog_fw_stream_rem_t   cfg_data;

            cfg_data.cfg_id = IWLWAV_RTLOG_CFG_REQ_REM_STREAM;
            cfg_data.str_id = str_cfg->str_id;
            rtlog_fw_cfg_notify_changes (wlan_if, IWLWAV_RTLOG_CFG_REQ_REM_STREAM, &cfg_data, sizeof(cfg_data));

            str_cfg->is_active = FALSE;
        }

        mtlk_osal_emergency_print("Switch off FW wlan %d Stream %d", wlan_if, str_num);
      }
      break;

    case IWLWAV_RTLOG_COMPONENT_ID_DRIVER:
      {
        rtlog_drv_cfg_t         *drv_cfg = &g_rtlog_cfg.drv_cfg;

        drv_cfg->is_active = FALSE;
        mtlk_osal_emergency_print("Switch off Driver Stream");
      }
      break;

    default:
        /* Should not be here */
        return -1;
    } /* end of switch */

    return 0;
}

/*---------------------------------------------*/

static void _print_packet_header(struct seq_file *seq, rtlog_pck_hdr_t *pck_hdr)
{
    int i;

    seq_printf(seq, "    PacketHeader:");
    for (i = 0; i < IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS; i++) {
        seq_printf(seq, " 0x%02x", ntohl(pck_hdr->words[i])); /* network endian */
    }
    seq_printf(seq, "\n");
}

#define PRT_MSG(msg)        seq_printf(seq, msg "\n")
#define PRT_VAL(msg, val)   seq_printf(seq, msg ": %u\n", val)

static void rtlog_cfg_get_ndev(struct seq_file *seq, rtlog_ndev_cfg_t *ndev_cfg)
{
#ifdef MTLK_USE_DIRECTPATH_API
    rtlog_ppa_t     *ppa;
#endif /* MTLK_USE_DIRECTPATH_API */

    PRT_MSG("\nNetDevice:");

#ifdef MTLK_USE_DIRECTPATH_API
    ppa = &ndev_cfg->ppa;
    PRT_VAL("PPA enabled", ppa->is_ppa_enabled);
    PRT_MSG("PPA statistic:");
    PRT_VAL("Sent to PPA accepted  ",    ppa->stats.sent_accepted);
    PRT_VAL("Sent to PPA rejected  ",    ppa->stats.sent_rejected);
    PRT_VAL("Recv from PPA for RX  ",    ppa->stats.recv_for_rx);
    PRT_VAL("Recv from PPA for TX  ",    ppa->stats.recv_for_tx);
    PRT_VAL("Recv from PPA dropped ",    ppa->stats.recv_dropped);

#else
    PRT_MSG("PPA is not supported");
#endif /* MTLK_USE_DIRECTPATH_API */
}

#undef PRT_MSG
#undef PRT_VAL

static void rtlog_cfg_get_drv(struct seq_file *seq, rtlog_drv_cfg_t *drv_cfg)
{
    if (!drv_cfg->is_active) {
        seq_printf(seq, "\nDriver: is not set\n");
    } else {
        seq_printf(seq, "\nDriver:\n");

        _print_packet_header(seq, &drv_cfg->pck_hdr);
    }
}

static void rtlog_cfg_get_fw_stream(struct seq_file *seq, rtlog_fw_stream_cfg_t *fw_str_cfg)
{
    seq_printf(seq, "StreamID %d:", fw_str_cfg->str_id);

    if (fw_str_cfg->is_active) {
        seq_printf(seq, "  BufThr 0x%04x, FIFOs 0x%08x\n", fw_str_cfg->buf_thr, fw_str_cfg->fifos);
        _print_packet_header(seq, &fw_str_cfg->pck_hdr);
    } else {
        seq_printf(seq, " is not set\n");
    }
}

static void rtlog_cfg_get_fw_filter(struct seq_file *seq, rtlog_fw_filter_cfg_t *fw_flt_cfg)
{
    seq_printf(seq, "FilterType %d:", fw_flt_cfg->flt_type);

    if (fw_flt_cfg->is_active) {
        seq_printf(seq, " LogLevel %d, ModBitmap 0x%02x\n", fw_flt_cfg->log_lvl, fw_flt_cfg->bitmap);
    } else {
        seq_printf(seq, " is not set\n");
    }
}

static void rtlog_cfg_get_fw(struct seq_file *seq, rtlog_fw_cfg_t *fw_cfg)
{
    int i;

    if (!fw_cfg->is_active) {
        seq_printf(seq, "\nFW %d: is not set\n", fw_cfg->wlan_if);
    } else {
        seq_printf(seq, "\nFW %d:\n", fw_cfg->wlan_if);
        seq_printf(seq, "max_nof_streams %d\n", fw_cfg->max_nof_streams);

        seq_printf(seq, "HwModulesFifo:");
        for (i = 0; i < IWLWAV_RTLOG_HW_FIFOS; i++) {
            seq_printf(seq, " 0x%04x", fw_cfg->hw_fifo_cfg.config[i]);
        }
        seq_printf(seq, "\n");

        for (i = 0; i < fw_cfg->max_nof_streams; i++) {
            rtlog_cfg_get_fw_stream(seq, &fw_cfg->stream_cfg[i]);
        }

        for (i = 0; i < IWLWAV_RTLOG_FW_MAX_FILTER; i++) {
            rtlog_cfg_get_fw_filter(seq, &fw_cfg->filter_cfg[i]);
        }
    }
}

/*-------------------------------------------*/
/* Table of config parameters */
static rtlog_cfg_fmt_t  rtlog_cfg_fmt_array[] =
{
    /* Cmd          Function             args ints */
    { "Driver",     rtlog_cfg_set_drv,      0 + IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS,
                                            0 + IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS },

    { "FwMaxStream", rtlog_cfg_set_fw_max_stream,      2, 2 },
    { "FwRemStream", rtlog_cfg_set_fw_rem_stream,      2, 2 },
    { "FwAddStream", rtlog_cfg_set_fw_add_stream,      5 + IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS,
                                                       5 + IWLWAV_RTLOG_PCK_HDR_LEN_IN_WORDS },

    { "FwRemFilter", rtlog_cfg_set_fw_rem_filter,      2, 2 },
    { "FwAddFilter", rtlog_cfg_set_fw_add_filter,      3, 3 },

    /* New API for Unified format */
    { "LogRemStream", rtlog_cfg_set_log_del_stream,     IWLWAV_RTLOG_CFG_STREAM_ARGS, /* CID, wlan_if, stream_num */
                                                        IWLWAV_RTLOG_CFG_STREAM_ARGS - 1 /* without wlan_if */ },

    { "LogAddStream", rtlog_cfg_set_log_add_stream,     IWLWAV_RTLOG_CFG_STREAM_ARGS, /* w/o header */
                                                        // IWLWAV_RTLOG_CFG_STREAM_ARGS + IWLWAV_RTLOG_CFG_PCK_HDR_ARGS, /* with header */
                                                        0 /* without checking */ },

    { "LogHwModuleFifo", rtlog_cfg_set_log_hw_fifo,     1 + IWLWAV_RTLOG_HW_FIFOS_MIN, /* per wlan_if */
                                                        0 /* without checking */ },

#ifdef MTLK_USE_DIRECTPATH_API
    { "LogDevPpa",  rtlog_cfg_set_ndev_ppa, 1, 1 },
#endif  /* MTLK_USE_DIRECTPATH_API */

    { NULL }    /* end of table */
};

/*-------------------------------------------*/

/* split a line to args */
static int line_split(char *line, char *argv[], int size)
{
    int     argc = 0;
    char    *cp = line;
    char    delim[] = " ,\t";

    --size; /* reserve for NULL */
    cp = strsep(&cp, "\n\r#"); /* truncate line */

    while (cp != NULL && argc < size) {
        cp += strspn(cp, delim);
        if (*cp == '\0')
            break;

        argv[argc++] = strsep(&cp, delim);
    }

    if (size >= 0)
        argv[argc] = NULL;

    return argc;
}

/* Convert arg strings to numberic values */
/* Use value (~0) if argv is not a number */
int argv_to_argn(int argc, char *argv[], uint32 argn[])
{
    int val;
    int i = 0;
    int n = 0;

    while (i < argc) {
        if (0 == sscanf(argv[i], "%i", &val)) {
            val = ~0;   /* fail */
        } else {
            n++;        /* success */
        }

        argn[i++] = (uint32)val;
    }

    return n;
}

int rtlog_cfg_line_parse(char *line)
{
    char      *argv[IWLWAV_RTLOG_MAX_NUM_PARAMS + 1]; /* + 1 for NULL */
    uint32     argn[IWLWAV_RTLOG_MAX_NUM_PARAMS] = { 0 };

    rtlog_cfg_fmt_t *fmt;
    char      *cmd;
    char     **argp = argv;
    int        argc, ints;

#ifdef CPTCFG_IWLWAV_DEBUG
    mtlk_osal_emergency_print("%s: %s", __FUNCTION__, line);
#endif

    argc = line_split(line, argv, sizeof(argv)/sizeof(argv[0]));
    if (0 == argc)
        return 0;

    cmd = *argp++;
    argc--;

    for (fmt = rtlog_cfg_fmt_array; fmt->cmd; fmt++) {
        if (0 == strcmp(cmd, fmt->cmd)) {
            if (argc < (int)fmt->nof_args) {
                mtlk_osal_emergency_print("ERROR: cmd %s: num of args %d (min %d expected)", cmd, argc, fmt->nof_args);
                return -1;
            }

            ints = argv_to_argn(argc, argp, argn);
            if (fmt->nof_ints != 0 && ints != (int)fmt->nof_ints) {
                mtlk_osal_emergency_print("ERROR: cmd %s: parsed int args %d (%d expected)", cmd, ints, fmt->nof_ints);
                return -1;
            }

            if (fmt->func) {
                int rc = fmt->func(argc, argp, argn);
                if (rc) {
                    mtlk_osal_emergency_print("ERROR: cmd %s, res %d", cmd, rc);
                }
            }

            return 0; /* found */
        }
    };

    mtlk_osal_emergency_print("ERROR: Unknown cmd %s", cmd);

    return -1; /* not found */
}

void __MTLK_IFUNC
mtlk_log_set_rtlog(char *line)
{
    if (0 != rtlog_cfg_line_parse(line))
    {
        mtlk_osal_emergency_print("ERROR: rtlog_cfg_line_parse failed");
    }
}

/*-------------------------------------------*/

void __MTLK_IFUNC
mtlk_log_get_rtlog (struct seq_file *seq)
{
    int     i;

    /* FIXME: under lock */
    seq_printf(seq, "RT Logger config:\n");

    rtlog_cfg_get_ndev(seq, &g_rtlog_ndev_cfg);

    rtlog_cfg_get_drv(seq, &g_rtlog_cfg.drv_cfg);

    for (i = 0; i < IWLWAV_RTLOG_HW_MAX_WLAN_IF; i++) {
        rtlog_cfg_get_fw(seq, &g_rtlog_cfg.fw_cfg[i]);
    }
}

/* ========================== API ========================== */

/* Get RT-Logger packet header (copy to word buffer) */
/* Use Driver config */
int __MTLK_IFUNC
mtlk_rtlog_get_packet_header (uint32 *wbuf)
{
    /* wbuf should be word aligned */
    MTLK_ASSERT(wbuf);
    MTLK_ASSERT(0 == (0x03 & (long) wbuf));

    if (!_rtlog_drv_cfg_is_active()) {
        /* mtlk_osal_emergency_print("WARN: Driver Logger config is not set"); */
        return MTLK_ERR_BUSY;
    }

    copy_word_array(wbuf, g_rtlog_cfg.drv_cfg.pck_hdr.words, ARRAY_SIZE(g_rtlog_cfg.drv_cfg.pck_hdr.words));

    return MTLK_ERR_OK;
}

/* Get RT-Logger FW config by wlan if number */
int __MTLK_IFUNC
mtlk_rtlog_fw_get_config (uint32 wlan_if, rtlog_fw_cfg_t *cfg)
{
    MTLK_ASSERT(NULL != cfg);

    if (rtlog_cfg_check_wlan_if(wlan_if)) {
        // FIXME: under lock
        *cfg = g_rtlog_cfg.fw_cfg[wlan_if];
        return MTLK_ERR_OK;
    } else {
        return MTLK_ERR_NO_RESOURCES;
    }
}
EXPORT_SYMBOL(mtlk_rtlog_fw_get_config);

/* Get RT-Logger HW FIFO config by wlan if number */
int __MTLK_IFUNC
mtlk_rtlog_fw_get_hw_fifo (uint32 wlan_if, rtlog_hw_fifo_cfg_t *cfg)
{
    MTLK_ASSERT(NULL != cfg);

    if (rtlog_cfg_check_wlan_if(wlan_if)) {
        // FIXME: under lock
        *cfg = g_rtlog_cfg.fw_cfg[wlan_if].hw_fifo_cfg;
        return MTLK_ERR_OK;
    } else {
        return MTLK_ERR_NO_RESOURCES;
    }
}
EXPORT_SYMBOL(mtlk_rtlog_fw_get_hw_fifo);

int __MTLK_IFUNC
rtlog_fw_clb_register (int wlan_if, rtlog_clb_t clb, mtlk_handle_t ctx)
{
    rtlog_clb_entry_t *fw_clb;

    ILOG2_DPP("wlan_if %d: clb %p, ctx %p", wlan_if, clb, ctx);

    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return MTLK_ERR_NO_RESOURCES;
    }

    fw_clb = &g_rtlog_cfg.fw_clb[wlan_if];
    fw_clb->clb = clb;
    fw_clb->ctx = ctx;

    return MTLK_ERR_OK;
}
EXPORT_SYMBOL(rtlog_fw_clb_register);

void __MTLK_IFUNC
rtlog_fw_clb_unregister (int wlan_if)
{
    rtlog_clb_entry_t *fw_clb;

    ILOG2_D("wlan_if %d", wlan_if);

    if (!rtlog_cfg_check_wlan_if(wlan_if)) {
        return;
    }

    fw_clb = &g_rtlog_cfg.fw_clb[wlan_if];
    fw_clb->clb = NULL;
    fw_clb->ctx = 0;
}
EXPORT_SYMBOL(rtlog_fw_clb_unregister);
#endif /* CONFIG_WAVE_RTLOG_REMOTE */

uint32 __MTLK_IFUNC
mtlk_rtlog_get_wlanif_from_string(char *str)
{
    uint32  wlan_if;

    if (1 != sscanf(str, "wlan%u", &wlan_if)) {
        mtlk_osal_emergency_print("Can't fetch wlan_if from string \"%s\"", str);
        wlan_if = IWLWAV_RTLOG_WLAN_IF_INVALID;
    }

    return wlan_if;
}
EXPORT_SYMBOL(mtlk_rtlog_get_wlanif_from_string);

#ifdef CONFIG_WAVE_RTLOG_REMOTE
#define OFFLINE_RTLOGGER_MODE 1

#ifdef CONFIG_WAVE_DEBUG
/* Return TRUE if any of specified interfaces FW streams has offline mode enabled */
BOOL __MTLK_IFUNC
rtlog_fw_offline_logging_enabled (const char *ifname)
{
  int i;
  uint32 wlan_if;
  rtlog_fw_cfg_t *fw_cfg;
  rtlog_fw_stream_cfg_t *str_cfg;

  wlan_if = mtlk_rtlog_get_wlanif_from_string((char *)ifname);

  if (IWLWAV_RTLOG_WLAN_IF_INVALID == wlan_if) {
    ELOG_S("Could not get wlan interface number from %s", ifname);
    return FALSE;
  }

  fw_cfg = &g_rtlog_cfg.fw_cfg[wlan_if];

  if (!fw_cfg->is_active)
    return FALSE;

  for (i = 0; i < IWLWAV_RTLOG_FW_MAX_STREAM; i++) {
    str_cfg = &fw_cfg->stream_cfg[i];

    if (str_cfg->is_active && (OFFLINE_RTLOGGER_MODE == str_cfg->mode))
      return TRUE;
  }

  return FALSE;
}
EXPORT_SYMBOL(rtlog_fw_offline_logging_enabled);
#endif /* CONFIG_WAVE_DEBUG */
#endif /* CONFIG_WAVE_RTLOG_REMOTE */
