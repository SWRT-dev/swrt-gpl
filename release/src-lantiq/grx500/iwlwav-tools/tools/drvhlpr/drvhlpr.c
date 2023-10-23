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
 * Driver Helper tool.
 *
 * Originally written by Andrey Fidrya.
 *
 */
#include "mtlkinc.h"

#include "mhi_ieee_address.h"

#include "mtlk_osal.h"
#include "mtlknlink.h"
#include "mtlkirba.h"
#include "argv_parser.h"
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog_app.h"
#endif

#include "drvhlpr.h"
#include "driver_api.h"

#include "iniparseraux.h"
#include "mtlkcontainer.h"

#include "fshlpr.h"

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
#include "mtlk_objpool.h"
MTLK_DECLARE_OBJPOOL(g_objpool);
#endif

#include <signal.h>
#include <stdarg.h>
#include <string.h>
#include <limits.h>

#define LOG_LOCAL_GID   GID_DRVHLPR
#define LOG_LOCAL_FID   1

#define APP_NAME IWLWAV_RTLOG_APP_NAME_DRVHLPR

/************************************************************************************
 * CFG-file parameters handling
 ************************************************************************************/
#define PNAME_RF_MGMT_TYPE             "RFMgmtEnable"
#define PNAME_RF_MGMT_REFRESH_TIME     "RFMgmtRefreshTime"
#define PNAME_RF_MGMT_KEEPALIVE_TMOUT  "RFMgmtKeepAliveTimeout"
#define PNAME_RF_MGMT_AVERAGING_APLHA  "RFMgmtAveragingAlpha"
#define PNAME_RF_MGMT_MARGIN_THRESHOLD "RFMgmtMetMarginThreshold"

#define DEFAULT_LED_RESOLUTION      1 /* sec */
#define DEFAULT_SECURITY_MODE       0

mtlk_irba_t             *mtlk_irba_wlan     = NULL;

static uint32            cfg_mask           = 0;
static char              wlanitrf[IFNAMSIZ] = "";
static const char       *params_conf_file   = NULL;

static pthread_t         signal_thread;
static mtlk_osal_event_t close_evt;
static BOOL              close_evt_set      = FALSE;
static int               close_status       = 0;

static BOOL dut_mode = FALSE;

static char *recovery_script_path = NULL;
static const char _DRVHLPR_SCRIPT_CMD_START[]   = "fw_crash";
static const char _DRVHLPR_SCRIPT_CMD_STOP[]    = "fw_recovery_end";
static const char _DRVHLPR_SCRIPT_CMD_RESTART_HOSTAPD[]   = "hostapd_restart";

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static rtlog_app_info_t rtlog_info_data;
#endif

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

enum mem_alarm_type_e
{
  MAT_PRINT_INFO,
  MAT_PRINT_ALLOC_INFO_ONCE,
  MAT_PRINT_ALLOC_INFO_CONTINUOUSLY,
  MAT_PRINT_ALLOC_INFO_AND_ASSERT,
  MAT_LAST
};

static uint32            mem_alarm_limit    = 0;
static uint32            mem_alarm_type     = MAT_PRINT_ALLOC_INFO_ONCE;
static volatile BOOL     mem_alarm_fired    = FALSE;
static volatile uint32   mem_alarm_prints   = 0;
#endif

static volatile BOOL     term_signal_noticed = FALSE;

/* 
 * Enum with indexes of driver helper components array.
 * This enum is needed to provide direct reference to
 * components (during configuration loading).
 *
 * Note: after any change in drvhelper_components[] (addition,
 *       reordering etc.) this enum must be also updated to 
 *       provide synchronization
 */
typedef enum {
  MTLK_FS_COMPONENT_IDX,
} mtlk_drvhelper_components_idx;

/*
 * Components of driver helper
 */
static mtlk_component_t drvhelper_components[] = {
  {
    .api  = &irb_fs_hlpr_api,
    .name = "fs_hlpr"
  },
};

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
static int __MTLK_IFUNC
_print_mem_alloc_dump (mtlk_handle_t printf_ctx,
                       const char   *format,
                       ...)
{
  int     res;
  va_list valst;
  char    buf[512];

  va_start(valst, format);
  res = vsnprintf(buf, sizeof(buf), format, valst);
  va_end(valst);

  ELOG_S("%s", buf);

  return res;
}

static void
print_mem_alloc_dump (void)
{
  mem_leak_dbg_print_allocators_info(_print_mem_alloc_dump, 
                                     HANDLE_T(0));
}
#endif

static void
initiate_app_shutdown (int status)
{
  if (close_evt_set)
    return;

  close_status = status;
  mtlk_osal_event_set(&close_evt);
  close_evt_set = TRUE;
}

/* The signal thread is most likely not necessary at all.
 * Signals can interrupt any thread, this one just starts the application shutdown.
 */
static void *
_signal_thread_proc (void* param)
{
  while (1) {
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    switch (mtlk_rtlog_app_get_terminated_status()) {
      case MTLK_RTLOG_APP_TERMINATED:
        term_signal_noticed = TRUE;
        break;
      case MTLK_RTLOG_APP_KILLED:
        return NULL;
    }
#endif

    if (term_signal_noticed)
      initiate_app_shutdown(EVENT_SIGTERM);

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    if (mem_alarm_fired) {
      switch (mem_alarm_type)
      {
      case MAT_PRINT_INFO:
        break;
      case MAT_PRINT_ALLOC_INFO_ONCE:
        if (!mem_alarm_prints) {
          ++mem_alarm_prints;
          print_mem_alloc_dump();
        }
        break;
      case MAT_PRINT_ALLOC_INFO_CONTINUOUSLY:
        print_mem_alloc_dump();
        break;
      case MAT_PRINT_ALLOC_INFO_AND_ASSERT:
        print_mem_alloc_dump();
        MTLK_ASSERT(0);
        break;
      case MAT_LAST:
      default:
        WLOG_D("Incorrect Memory Alarm Type: %d", mem_alarm_type);
        break;
      }
    }
    mtlk_osal_msleep(20);
#else
    sleep(1);
#endif
  }

  return NULL;
}

static void
_kill_signal_thread (void)
{
  pthread_kill(signal_thread, 0);
}

static int
_create_signal_thread (void)
{
  int            res = MTLK_ERR_UNKNOWN;
  pthread_attr_t attr;

  /* Initialize attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);

  /* Run thread */
  if (pthread_create(&signal_thread, 
                     &attr,
                     _signal_thread_proc,
                     NULL) == 0) {
    res = MTLK_ERR_OK;
  }

  /* Free resources */
  pthread_attr_destroy(&attr);

  return res;
}

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

static void __MTLK_IFUNC 
memory_alarm_clb (mtlk_handle_t usr_ctx,
                  uint32        allocated)
{
  MTLK_UNREFERENCED_PARAM(usr_ctx);
  MTLK_UNREFERENCED_PARAM(allocated);

  ELOG_DD("Memory alarm limit reached: %d >= %d", allocated, mem_alarm_limit);

  mem_alarm_fired = TRUE;
}

static void
install_memory_alarm (BOOL set)
{
  if (set) {
    mtlk_objpool_memory_alarm_t memory_alarm_info;

    memory_alarm_info.limit   = mem_alarm_limit;
    memory_alarm_info.clb     = memory_alarm_clb;
    memory_alarm_info.usr_ctx = HANDLE_T(0);

    mtlk_objpool_set_memory_alarm(&g_objpool, &memory_alarm_info);
  }
  else {
    mtlk_objpool_set_memory_alarm(&g_objpool, NULL);
  }
}
#endif

int drvhlpr_set_script_path (char** dest, const char* str)
{
  int size;
  char *p_buf;

  size = strlen(str) + 1;
  if (size > PATH_MAX) {
    ELOG_D("Script path is too long! It's length is %d.", size);
    return MTLK_ERR_BUF_TOO_SMALL;
  }
  p_buf = malloc(size);
  if (NULL == p_buf) {
    ELOG_V("Malloc memory for script path fail!");
    return MTLK_ERR_NO_MEM;
  }
  wave_memcpy(p_buf, size, str, size);

  *dest = p_buf;
  ILOG0_S("set script path: %s", *dest);
  return MTLK_ERR_OK;
}

static int 
load_cfg (const char *cfg_fname)
{
  int         res  = MTLK_ERR_UNKNOWN;
  dictionary *dict = NULL;
  const char *tmp = NULL;
  char *tmp_cmd = NULL;

  MTLK_ASSERT(cfg_fname != NULL);
  MTLK_ASSERT(strlen(cfg_fname) != 0);

  dict = iniparser_load(cfg_fname);
  if (!dict) {
    ELOG_S("Can't load CFG file (%s)", cfg_fname);
    res = MTLK_ERR_PARAMS;
    goto end;
  }

  if (TRUE == dut_mode)
  {
    /* FS helper available in dud mode only, and vice versa */
    tmp = iniparser_getstr(dict, iniparser_aux_gkey_const("f_saver"));
    if (!tmp) {
      ELOG_V("'f_saver' parameter reading error");
      goto end;
    }

    tmp_cmd = strdup(tmp);
    if (!tmp_cmd) {
      ELOG_V("'f_saver' allocate str error");
      res = MTLK_ERR_NO_MEM;
      goto end;
    }

    res = fs_hlpr_set_saver(tmp_cmd);
    if (res != MTLK_ERR_OK) {
      ELOG_D("'f_saver' parameter set error #%d", res);
    }
    free(tmp_cmd);
    goto end;
  }

  tmp = iniparser_getstr(dict, iniparser_aux_gkey_const("recovery_script_path"));
  if (tmp) {
    res = drvhlpr_set_script_path(&recovery_script_path, tmp);
    if (res != MTLK_ERR_OK) {
      ELOG_D("'recovery_script_path' parameter set error #%d", res);
      goto end;
    }
    cfg_mask |= DHFLAG_RECOVERY_HANDLING;
  }
  else {
    WLOG_V("Can't find the 'recovery_script_path' optional parameter");
  }

  res = iniparser_aux_fillstr(dict,
                              iniparser_aux_gkey_const("interface"),
                              wlanitrf,
                              sizeof(wlanitrf));
  if (res != MTLK_ERR_OK) {
    ELOG_D("'interface' parameter reading error: %d", res);
    goto end;
  }

  if (iniparser_getint(dict, iniparser_aux_gkey_const("Debug_SoftwareWatchdogEnable"), 1) == 0) {
    cfg_mask |= DHFLAG_NO_DRV_HUNG_HANDLING;
  }

end:
  if (dict) {
    iniparser_freedict(dict);
  }

  return res;
}
/*************************************************************************************/


typedef struct _network_interface
{
    char name[IFNAMSIZ];
} network_interface;


#define DRV_RETRY_CONNECTION_SEC 1

static const mtlk_guid_t IRBE_RMMOD                 = MTLK_IRB_GUID_RMMOD;
static const mtlk_guid_t IRBE_HANG                  = MTLK_IRB_GUID_HANG;
static const mtlk_guid_t IRBE_RECOVERY_START_DUMP   = MTLK_IRB_GUID_RECOVERY_START_DUMP;
static const mtlk_guid_t IRBE_RECOVERY_RCVRY_FINISH = MTLK_IRB_GUID_RECOVERY_RCVRY_FINISH;
static const mtlk_guid_t IRBE_RECOVERY_RESTART_HOSTAPD   = MTLK_IRB_GUID_RECOVERY_RESTART_HOSTAPD;

static mtlk_guid_t MAIN_IRBE_LIST[] = 
{
  MTLK_IRB_GUID_RMMOD,
  MTLK_IRB_GUID_HANG,
  MTLK_IRB_GUID_RECOVERY_START_DUMP,
  MTLK_IRB_GUID_RECOVERY_RCVRY_FINISH,
  MTLK_IRB_GUID_RECOVERY_RESTART_HOSTAPD,
};


/************************************************/

static void
sigterm_handler (int sig)
{
  /* It is illegal to call pthread_cond_signal from a signal handler, so we just set a flag
   * and let the "signal thread" itself from its own context do the honors... */
  term_signal_noticed = TRUE;
}

static void
_uninstall_sigactions (void)
{
  int              res;
  struct sigaction action;

  sigemptyset(&action.sa_mask);

  action.sa_handler = SIG_IGN;
  action.sa_flags   = 0;

  res = sigaction(SIGTERM, &action, NULL);
  if (res != 0) {
    ELOG_D("return error from sigaction: %d", errno);
    MTLK_ASSERT(0);
  }
}

static int
_install_sigactions (void)
{
  int              res;
  struct sigaction action;

  sigemptyset(&action.sa_mask);
  sigaddset(&action.sa_mask, SIGTERM); /* prevent handler re-entrance */

  action.sa_handler = sigterm_handler;
  action.sa_flags   = 0;

  res = sigaction(SIGTERM, &action, NULL);
  if (res != 0) {
    ELOG_D("return error from sigaction: %d", errno);
    res = MTLK_ERR_NO_RESOURCES;
    goto end;
  }

  res = MTLK_ERR_OK;

end:
  return res;
}

static const struct mtlk_argv_param_info_ex param_cfg_file = {
  {
    "p",
    NULL,
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "configuration file",
  MTLK_ARGV_PTYPE_MANDATORY
};

static const struct mtlk_argv_param_info_ex param_dlevel = {
  {
    "d",
    "debug-level",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "debug level (DEBUG verion only)",
  MTLK_ARGV_PTYPE_OPTIONAL
};

#ifdef MTLK_DEBUG
static const struct mtlk_argv_param_info_ex param_step_to_fail = {
  {
    NULL,
    "step-to-fail",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "step to simulate failure (start up macros, DEBUG version only)",
  MTLK_ARGV_PTYPE_OPTIONAL
};
#endif

static const struct mtlk_argv_param_info_ex param_stderr_err = {
  {
    NULL,
    "stderr-err",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "duplicate ELOG printouts to stderr",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_stderr_warn = {
  {
    NULL,
    "stderr-warn",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "duplicate ELOG and WLOG printouts to stderr",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_stderr_all = {
  {
    "e",
    "stderr-all",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "duplicate all (ELOG, WLOG, LOG) printouts to stderr",
  MTLK_ARGV_PTYPE_OPTIONAL
};

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
static const struct mtlk_argv_param_info_ex param_mem_alarm_limit = {
  {
    NULL,
    "mem-alarm-limit",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "alarm when allocated memory size becomes more then this limit",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_mem_alarm_type = {
  {
    NULL,
    "mem-alarm-type",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "how to alarm when allocated memory size becomes more then this limit:\n"
  "           0 - print message\n"
  "           1 - print allocations once\n"
  "           2 - print allocations continuously\n"
  "           3 - print allocations once and assert",
  MTLK_ARGV_PTYPE_OPTIONAL
};
#endif

static const struct mtlk_argv_param_info_ex param_dut = {
  {
    NULL,
    "dut",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "helper for dutserver",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_help =  {
  {
    "h",
    "help",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "print this help",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static void
_print_help (const char *app_name)
{
  const struct mtlk_argv_param_info_ex *all_params[] = {
    &param_cfg_file,
    &param_dlevel,
    &param_stderr_err,
    &param_stderr_warn,
    &param_stderr_all,
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    &param_mem_alarm_limit,
    &param_mem_alarm_type,
#endif
    &param_dut,
    &param_help
  };
  const char *app_fname = strrchr(app_name, '/');

  if (!app_fname) {
    app_fname = app_name;
  }
  else {
    ++app_fname; /* skip '/' */
  }

  mtlk_argv_print_help(stdout,
                       app_fname,
                       "Metalink Driver Helper v." MTLK_SOURCE_VERSION,
                       all_params,
                       (uint32)ARRAY_SIZE(all_params));
}


static int
process_commandline (int argc, char *argv[])
{
  int                res    = MTLK_ERR_UNKNOWN;
  BOOL               inited = FALSE;
  mtlk_argv_parser_t argv_parser;
  mtlk_argv_param_t *param  = NULL;

  if (argc < 2) {
    res  = MTLK_ERR_PARAMS;
    goto end;
  }

  res = mtlk_argv_parser_init(&argv_parser, argc, argv);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init argv parser (err=%d)", res);
    goto end;
  }
  inited = TRUE;

  param = mtlk_argv_parser_param_get(&argv_parser, &param_help.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_cfg_file.info);
  if (param) {
    params_conf_file = mtlk_argv_parser_param_get_str_val(param); /* We can use this pointer after release
                                                                   * since it actually points to argv.
                                                                   */
    mtlk_argv_parser_param_release(param);
  }

  if (!params_conf_file) {
    ELOG_V("CFG file must be specified!");
    res = MTLK_ERR_PROHIB;
    goto end;  
  }

#ifdef MTLK_DEBUG
  param = mtlk_argv_parser_param_get(&argv_parser, &param_step_to_fail.info);
  if (param) {
    uint32 v = mtlk_argv_parser_param_get_uint_val(param, (uint32)-1);
    mtlk_argv_parser_param_release(param);
    if (v != (uint32)-1) {
      mtlk_startup_set_step_to_fail(v);
    }
    else {
      ELOG_V("Invalid step-to-fail!");
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }
#endif

  param = mtlk_argv_parser_param_get(&argv_parser, &param_dlevel.info);
  if (param) {
    uint32 v = mtlk_argv_parser_param_get_uint_val(param, (uint32)-1);
    mtlk_argv_parser_param_release(param);
    if (v != (uint32)-1) {
      debug = v;
    }
    else {
      ELOG_V("Invalid debug-level!");
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_stderr_err.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_ERR, TRUE);
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_stderr_warn.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_ERR, TRUE);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_WARN, TRUE);
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_stderr_all.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_ERR, TRUE);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_WARN, TRUE);
    _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_INFO, TRUE);
  }

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
  param = mtlk_argv_parser_param_get(&argv_parser, &param_mem_alarm_limit.info);
  if (param) {
    mem_alarm_limit = mtlk_argv_parser_param_get_uint_val(param, mem_alarm_limit);
    mtlk_argv_parser_param_release(param);
  }

  param = mtlk_argv_parser_param_get(&argv_parser, &param_mem_alarm_type.info);
  if (param) {
    mem_alarm_type = mtlk_argv_parser_param_get_uint_val(param, mem_alarm_type);
    mtlk_argv_parser_param_release(param);
    if (mem_alarm_type >= MAT_LAST) {
      ELOG_V("Invalid mem_alarm_type!");
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }
#endif

  param = mtlk_argv_parser_param_get(&argv_parser, &param_dut.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    dut_mode = TRUE;
  }

  res = MTLK_ERR_OK;

end:
  if (inited) {
    mtlk_argv_parser_cleanup(&argv_parser);
  }

  if (res != MTLK_ERR_OK) {
    _print_help(argv[0]);
  }

  return res;
}

static int
_drvhlpr_run_script (const char *command)
{
  int res;
  char script_cmd[MTLK_RECOVERY_MAX_CMD_LEN];

  res = snprintf(script_cmd, ARRAY_SIZE(script_cmd), "%s %s %s",
                 recovery_script_path, wlanitrf, command);

  if (res < 0) {
    ELOG_D("Failed to prepare executable script, return value is %d", res);
    return res;
  }

  ILOG1_S("Executing command: %s", script_cmd);

  res = system(script_cmd);
  return res;
}

static const mtlk_guid_t IRBE_RECOVERY_START_DUMP_CFM = MTLK_IRB_GUID_RECOVERY_START_DUMP_CFM;
static const mtlk_guid_t IRBE_RECOVERY_STOP_DUMP_CFM  = MTLK_IRB_GUID_RECOVERY_STOP_DUMP_CFM;

static const mtlk_guid_t IRBE_RECOVERY_RESTART_HOSTAPD_CFM   = MTLK_IRB_GUID_RECOVERY_RESTART_HOSTAPD_CFM;

static __INLINE int
mtlk_guid_to_string (const mtlk_guid_t* guid, char *buffer, uint32 size)
{
  return sprintf_s(buffer,
                   size,
                   "{%08x-%04x-%04x-%02x%02x-%02x%02x%02x%02x%02x%02x}",
                   guid->data1,
                   guid->data2,
                   guid->data3,
                   guid->data4[0],
                   guid->data4[1],
                   guid->data4[2],
                   guid->data4[3],
                   guid->data4[4],
                   guid->data4[5],
                   guid->data4[6],
                   guid->data4[7]);
}

static void __MTLK_IFUNC 
main_irb_handler (mtlk_irba_t       *irba,
                  mtlk_handle_t      context,
                  const mtlk_guid_t *evt,
                  void              *buffer,
                  uint32            size)
{
  if (mtlk_guid_compare(evt, &IRBE_HANG) == 0) {
    ILOG1_V("Got HANG event\n");
    if (!(cfg_mask & DHFLAG_NO_DRV_HUNG_HANDLING)){
      initiate_app_shutdown(EVENT_REQ_RESET);
    }
  }
  else if (mtlk_guid_compare(evt, &IRBE_RMMOD) == 0) {
    if (strncmp((char *) buffer, wlanitrf, IFNAMSIZ)) {
      return; /* this event is for another interface - do not handle */
    }
    ILOG1_V("Got RMMOD event\n");
    if (!(cfg_mask & DHFLAG_NO_DRV_RMMOD_HANDLING)){
      initiate_app_shutdown(EVENT_REQ_RMMOD);
    }
  }
  else if (mtlk_guid_compare(evt, &IRBE_RECOVERY_START_DUMP) == 0) {
    int res = 0;
    if (strncmp((char *) buffer, wlanitrf, IFNAMSIZ)) {
      return; /* this event is for another interface - do not handle */
    }
    ILOG1_V("Got RECOVERY start dump event");

    res = mtlk_irba_call_drv(mtlk_irba_wlan, &IRBE_RECOVERY_START_DUMP_CFM, (void*)NULL, 0);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Failed to send RECOVERY start dump confirmation to driver, return value is %d", res);
      return;
    }

    if (cfg_mask & DHFLAG_RECOVERY_HANDLING) {
      res = _drvhlpr_run_script(_DRVHLPR_SCRIPT_CMD_START);
      if (res != 0) {
        ELOG_SD("Failed to run recovery script %s, return value is %d", _DRVHLPR_SCRIPT_CMD_START, res);
      }
    }

    res = mtlk_irba_call_drv(mtlk_irba_wlan, &IRBE_RECOVERY_STOP_DUMP_CFM, (void*)NULL, 0);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Failed to send RECOVERY stop dump confirmation to driver, return value is %d", res);
    }
  }
  else if (mtlk_guid_compare(evt, &IRBE_RECOVERY_RCVRY_FINISH) == 0) {
    int res = 0;
    if (strncmp((char *) buffer, wlanitrf, IFNAMSIZ)) {
      return; /* this event is for another interface - do not handle */
    }
    ILOG1_V("Got RECOVERY finish event");

    if (cfg_mask & DHFLAG_RECOVERY_HANDLING) {
      res = _drvhlpr_run_script(_DRVHLPR_SCRIPT_CMD_STOP);
      if (res != 0) {
        ELOG_SD("Failed to run recovery script %s, return value is %d", _DRVHLPR_SCRIPT_CMD_STOP, res);
      }
    }
  }
  else if (mtlk_guid_compare(evt, &IRBE_RECOVERY_RESTART_HOSTAPD) == 0) {
    int res;
    const char *script_cmd;

    ILOG2_V("Got RECOVERY restart hostapd event");
    if (strncmp((char *) buffer, wlanitrf, IFNAMSIZ)) {
      ELOG_SS(" %s: Ignore hostapd stop event for %s", wlanitrf, buffer);
      return; /* this event is for another interface - do not handle */
    }

    res = mtlk_irba_call_drv(mtlk_irba_wlan, &IRBE_RECOVERY_RESTART_HOSTAPD_CFM, (void*)NULL, 0);
    if (res != MTLK_ERR_OK) {
      ELOG_D("Failed to send RECOVERY stop hostapd confirmation to driver, return value is %d", res);
      return;
    }

    script_cmd = _DRVHLPR_SCRIPT_CMD_RESTART_HOSTAPD;
    ILOG2_SS("%s: Executing command: %s", wlanitrf, script_cmd);
    res = _drvhlpr_run_script(script_cmd);
    if (res != 0) {
      ELOG_SD("Failed to run `%s', return value is %d", script_cmd, res);
      return;
    }

    ILOG2_V("Finish processing restart hostapd event");
  }
  else {
    char guid_str[MTLK_GUID_STR_SIZE];
    mtlk_guid_to_string(evt, guid_str, sizeof(guid_str));
    ELOG_S("Unexpected GUID: %s", guid_str);
  }
}
typedef struct _drvhlpr_main_t
{
  mtlk_irba_handle_t *main_irb_client;
  mtlk_container_t    container;
  MTLK_DECLARE_INIT_STATUS;
} drvhlpr_main_t;

MTLK_INIT_STEPS_LIST_BEGIN(drvhlpr_main)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, OSDEP_LOG_INIT)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, RTLOG_APP_INIT)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, OSAL_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, CLOSE_EVT_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, START_SIGNALS_THREAD)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, INSTALL_SIGACTIONS)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, PARSE_CMD_LINE)
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, INSTALL_MEMORY_ALARM)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, PARSE_CFG_FILE)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, CONNECT_TO_DRV)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, IRBA_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, IRBA_START)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, IRBA_WLAN_ALLOC)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, IRBA_WLAN_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, IRB_REG)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, CONTAINER_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(drvhlpr_main, COMPONENTS_REG)
MTLK_INIT_INNER_STEPS_BEGIN(drvhlpr_main)
MTLK_INIT_STEPS_LIST_END(drvhlpr_main);

static void
_disconnect_from_drv (void)
{
  if (driver_connected()) {
    driver_close_connection();
  }
}

static int
_connect_to_drv (void)
{
  int res = MTLK_ERR_OK;

  if (TRUE == dut_mode)
  {
    snprintf(wlanitrf, sizeof(wlanitrf), "%s", "wlan0");
    return res;
  }
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  res = mtlk_assign_logger_hw_iface(&rtlog_info_data, wlanitrf);
  if (MTLK_ERR_OK != res) goto end;
#endif

  while (!driver_connected()) {
    if (0 != driver_setup_connection(wlanitrf)) {
      int wres = mtlk_osal_event_wait(&close_evt, DRV_RETRY_CONNECTION_SEC * 1000);
      if (wres == MTLK_ERR_OK) {
        res = MTLK_ERR_PROHIB;
        goto end;
      }
    }
  }

  ILOG2_S("Connected to driver, interface name = %s", wlanitrf);

end:
  return res;
}

static int
_drvhlpr_register_components (drvhlpr_main_t *_drvhlpr)
{
  int res = MTLK_ERR_OK;
  int i;

  for (i = 0; i < ARRAY_SIZE(drvhelper_components); ++i) {
    /* FS helper available in dud mode only, and vice versa */
    if ((TRUE == dut_mode) != (MTLK_FS_COMPONENT_IDX == i)) {
      WLOG_V("Drvhlpr is not in DUT mode. Ignore component fs_hlpr.");
      continue;
    }

    /* add only valid components */
    if (NULL == drvhelper_components[i].api) {
      WLOG_D("Ignore empty component[%d].", i);
      continue;
    }
    res = mtlk_container_register(&_drvhlpr->container, &drvhelper_components[i]);
    if (res != MTLK_ERR_OK) {
      ELOG_S("Component added fail: %s", drvhelper_components[i].name);
      break;
    } else {
      ILOG0_S("Component added: %s", drvhelper_components[i].name);
    }
  }

  return res;
}

static void __MTLK_IFUNC
irba_rm_handler (mtlk_irba_t   *irba,
                 mtlk_handle_t  context)
{
  WLOG_V("IRBA is going to disappear...");
}

static void free_rcvry_script_path(void)
{
  if (NULL != recovery_script_path)
    free(recovery_script_path);
}


static void
_drvhlpr_main_cleanup (drvhlpr_main_t *_drvhlpr)
{
  int tmp;

  MTLK_CLEANUP_BEGIN(drvhlpr_main, MTLK_OBJ_PTR(_drvhlpr))
    MTLK_CLEANUP_STEP(drvhlpr_main, COMPONENTS_REG, MTLK_OBJ_PTR(_drvhlpr),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(drvhlpr_main, CONTAINER_INIT, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_container_cleanup, (&_drvhlpr->container));
    MTLK_CLEANUP_STEP(drvhlpr_main, IRB_REG, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_irba_unregister, (MTLK_IRBA_ROOT, HANDLE_T_PTR(mtlk_irba_handle_t, _drvhlpr->main_irb_client)));
    MTLK_CLEANUP_STEP(drvhlpr_main, IRBA_WLAN_INIT, MTLK_OBJ_PTR(_drvhlpr), 
                      mtlk_irba_cleanup, (mtlk_irba_wlan));
    MTLK_CLEANUP_STEP(drvhlpr_main, IRBA_WLAN_ALLOC, MTLK_OBJ_PTR(_drvhlpr), 
                      mtlk_irba_free, (mtlk_irba_wlan));
    MTLK_CLEANUP_STEP(drvhlpr_main, IRBA_START, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_irba_app_stop, ());
    MTLK_CLEANUP_STEP(drvhlpr_main, IRBA_INIT, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_irba_app_cleanup, ());
    MTLK_CLEANUP_STEP(drvhlpr_main, CONNECT_TO_DRV, MTLK_OBJ_PTR(_drvhlpr),
                      _disconnect_from_drv, ());
    MTLK_CLEANUP_STEP(drvhlpr_main, PARSE_CFG_FILE, MTLK_OBJ_PTR(_drvhlpr),
                      free_rcvry_script_path, ());
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    MTLK_CLEANUP_STEP(drvhlpr_main, INSTALL_MEMORY_ALARM, MTLK_OBJ_PTR(_drvhlpr),
                      install_memory_alarm, (FALSE));
#endif
    MTLK_CLEANUP_STEP(drvhlpr_main, PARSE_CMD_LINE, MTLK_OBJ_PTR(_drvhlpr),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(drvhlpr_main, INSTALL_SIGACTIONS, MTLK_OBJ_PTR(_drvhlpr),
                      _uninstall_sigactions, ());
    MTLK_CLEANUP_STEP(drvhlpr_main, START_SIGNALS_THREAD, MTLK_OBJ_PTR(_drvhlpr),
                      _kill_signal_thread, ());
    MTLK_CLEANUP_STEP(drvhlpr_main, CLOSE_EVT_INIT, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_osal_event_cleanup, (&close_evt));
    MTLK_CLEANUP_STEP(drvhlpr_main, OSAL_INIT, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_osal_cleanup, ());
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_CLEANUP_STEP(drvhlpr_main, RTLOG_APP_INIT, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_rtlog_app_cleanup, (&rtlog_info_data));
#endif
    MTLK_CLEANUP_STEP(drvhlpr_main, OSDEP_LOG_INIT, MTLK_OBJ_PTR(_drvhlpr),
                      _mtlk_osdep_log_cleanup, ());
  MTLK_CLEANUP_END(drvhlpr_main, MTLK_OBJ_PTR(_drvhlpr))
}


static int
_drvhlpr_main_init (drvhlpr_main_t *_drvhlpr, int argc, char *argv[])
{
  int  tmp;

  MTLK_INIT_TRY(drvhlpr_main, MTLK_OBJ_PTR(_drvhlpr))
    MTLK_INIT_STEP(drvhlpr_main, OSDEP_LOG_INIT, MTLK_OBJ_PTR(_drvhlpr),
                   _mtlk_osdep_log_init, (APP_NAME));
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP(drvhlpr_main, RTLOG_APP_INIT, MTLK_OBJ_PTR(_drvhlpr),
                   mtlk_rtlog_app_init, (&rtlog_info_data, APP_NAME));
#endif
    MTLK_INIT_STEP(drvhlpr_main, OSAL_INIT, MTLK_OBJ_PTR(_drvhlpr),
                   mtlk_osal_init, ());
    MTLK_INIT_STEP(drvhlpr_main, CLOSE_EVT_INIT, MTLK_OBJ_PTR(_drvhlpr),
                   mtlk_osal_event_init, (&close_evt));
    MTLK_INIT_STEP(drvhlpr_main, START_SIGNALS_THREAD, MTLK_OBJ_PTR(_drvhlpr),
                   _create_signal_thread, ());
    MTLK_INIT_STEP(drvhlpr_main, INSTALL_SIGACTIONS, MTLK_OBJ_PTR(_drvhlpr),
                   _install_sigactions, ());
    MTLK_INIT_STEP(drvhlpr_main, PARSE_CMD_LINE, MTLK_OBJ_PTR(_drvhlpr),
                   process_commandline, (argc, argv));
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    MTLK_INIT_STEP_VOID(drvhlpr_main, INSTALL_MEMORY_ALARM, MTLK_OBJ_PTR(_drvhlpr),
                        install_memory_alarm, (TRUE));
#endif
    MTLK_INIT_STEP(drvhlpr_main, PARSE_CFG_FILE, MTLK_OBJ_PTR(_drvhlpr),
                   load_cfg, (params_conf_file));
    MTLK_INIT_STEP(drvhlpr_main, CONNECT_TO_DRV, MTLK_OBJ_PTR(_drvhlpr),
                   _connect_to_drv, ());
    MTLK_INIT_STEP(drvhlpr_main, IRBA_INIT, MTLK_OBJ_PTR(_drvhlpr),
                   mtlk_irba_app_init, (irba_rm_handler, HANDLE_T(0)));
    MTLK_INIT_STEP(drvhlpr_main, IRBA_START, MTLK_OBJ_PTR(_drvhlpr),
                   mtlk_irba_app_start, ());
    MTLK_INIT_STEP_EX(drvhlpr_main, IRBA_WLAN_ALLOC,  MTLK_OBJ_PTR(_drvhlpr), 
                      mtlk_irba_alloc, (),
                      mtlk_irba_wlan,
                      mtlk_irba_wlan != NULL,
                      MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(drvhlpr_main, IRBA_WLAN_INIT, MTLK_OBJ_PTR(_drvhlpr),
                   mtlk_irba_init, (mtlk_irba_wlan, wlanitrf, irba_rm_handler, HANDLE_T(0)));
    MTLK_INIT_STEP_EX(drvhlpr_main, IRB_REG, MTLK_OBJ_PTR(_drvhlpr),
                      mtlk_irba_register, (MTLK_IRBA_ROOT,
                                           MAIN_IRBE_LIST, 
                                           ARRAY_SIZE(MAIN_IRBE_LIST), 
                                           main_irb_handler, 
                                           0),
                      _drvhlpr->main_irb_client,
                      _drvhlpr->main_irb_client != NULL,
                      MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(drvhlpr_main, CONTAINER_INIT, MTLK_OBJ_PTR(_drvhlpr),
                   mtlk_container_init, (&_drvhlpr->container));
    
    MTLK_INIT_STEP(drvhlpr_main, COMPONENTS_REG, MTLK_OBJ_PTR(_drvhlpr),
                   _drvhlpr_register_components, (_drvhlpr));
  MTLK_INIT_FINALLY(drvhlpr_main, MTLK_OBJ_PTR(_drvhlpr))
  MTLK_INIT_RETURN(drvhlpr_main, MTLK_OBJ_PTR(_drvhlpr), _drvhlpr_main_cleanup, (_drvhlpr))
}

int
main (int argc, char *argv[])
{
  int            retval = 0;
  drvhlpr_main_t drvhlpr;

  memset(&drvhlpr, 0, sizeof(drvhlpr));

  retval = _drvhlpr_main_init(&drvhlpr, argc, argv);
  if (retval != MTLK_ERR_OK) {
    retval = EVENT_INT_ERR;
    goto end;
  }

  do {
    close_status = EVENT_DO_NOTHING;
    mtlk_osal_event_reset(&close_evt);
    mtlk_container_run(&drvhlpr.container, &close_evt);
  } while (close_status == EVENT_REQ_RESTART);
  retval = close_status;

  _drvhlpr_main_cleanup(&drvhlpr);

end:
  return retval;
}

