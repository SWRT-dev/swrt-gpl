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
 * Driver API
 *
 */

#include "mtlkinc.h"

#include "mtlkirba.h"
#include "dataex.h"
#include "dutserver.h"
#include "mtlkwlanirbdefs.h"
#include "mtlk_pathutils.h"
#include "mhi_dut.h"
#include "argv_parser.h"
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog_app.h"
#endif

#include <sys/stat.h>
#include <fcntl.h>

#define LOG_LOCAL_GID   GID_DUT_SRV_DRIVER_API
#define LOG_LOCAL_FID   1

/* For running DUT script
#define DUT_RUN_SCRIPT
#define IGNORE_SCRIPT_RES
*/

#ifdef CONFIG_WAVE_RTLOG_REMOTE
static rtlog_app_info_t rtlog_info_data;
#endif

#ifdef DUT_RUN_SCRIPT
static const char _DUT_DRVCTRL_SCRIPT[]         = "/etc/rc.d/wave_wlan_dut_drvctrl.sh";
static const char _DUT_DRVCTRL_CMD_START[]      = "start";
static const char _DUT_DRVCTRL_CMD_STOP[]       = "stop";
static const char _DUT_DRVCTRL_CMD_START_HLPR[] = "start_helper";
static const char _DUT_DRVCTRL_CMD_STOP_HLPR[]  = "stop_helper";
#endif /* DUT_RUN_SCRIPT  */

#define _DUT_DRVCTRL_MAX_CMD_LEN (MAX_PATH + 100)
#define _DUT_PLATFORM_MAX_CMD_LEN (64)
/* Possible interface indexes 0, 2, 4, 6 */
#define MAX_INTERFACE_INDEX ((MTLK_MAX_HW_ADAPTERS_SUPPORTED - 1) * 2)

static const char _DUT_PLATFORM_FILE_PATH[] = "/var/run/dutserver/platform.config";

static const mtlk_guid_t _IRBE_DUT_FW_CMD         = MTLK_IRB_GUID_DUT_FW_CMD;
static const mtlk_guid_t _IRBE_DUT_DRV_CMD        = MTLK_IRB_GUID_DUT_DRV_CMD;
static const mtlk_guid_t _IRBE_DUT_PROGMODEL_CMD  = MTLK_IRB_GUID_DUT_PROGMODEL_CMD;
static const mtlk_guid_t _IRBE_DUT_START_CMD      = MTLK_IRB_GUID_DUT_START_CMD;
static const mtlk_guid_t _IRBE_DUT_STOP_CMD       = MTLK_IRB_GUID_DUT_STOP_CMD;
static const mtlk_guid_t _IRBE_DUT_ADD_VAP_CMD    = MTLK_IRB_GUID_DUT_ADD_VAP_CMD;
static const mtlk_guid_t _IRBE_DUT_REMOVE_VAP_CMD = MTLK_IRB_GUID_DUT_REMOVE_VAP_CMD;
static const mtlk_guid_t _IRBE_DUT_SET_BSS_CMD    = MTLK_IRB_GUID_DUT_SET_BSS_CMD;
static const mtlk_guid_t _IRBE_DUT_SET_WMM_PARAMETERS_CMD = 
                          MTLK_IRB_GUID_DUT_SET_WMM_PARAMETERS_CMD;
static const mtlk_guid_t _IRBE_DUT_STOP_VAP_TRAFFIC_CMD   =
                          MTLK_IRB_GUID_DUT_STOP_VAP_TRAFFIC_CMD;
static const mtlk_guid_t _IRBE_DUT_DRIVER_FW_GENERAL_CMD   =
                          MTLK_IRB_GUID_DUT_DRIVER_FW_GENERAL_CMD;
static const mtlk_guid_t _IRBE_DUT_PLATFORM_DATA_FIELDS_CMD =
                          MTLK_IRB_GUID_DUT_PLATFORM_DATA_FIELDS_CMD;

static const struct mtlk_argv_param_info_ex param_script = {
  {
    NULL,
    "drv-ctrl",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "drv-ctrl script",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_help = {
  {
    "h",
    "help",
    MTLK_ARGV_PINFO_FLAG_HAS_NO_DATA
  },
  "print this help",
  MTLK_ARGV_PTYPE_OPTIONAL
};

typedef struct _dut_api_t
{
  struct
  {
    mtlk_irba_t* irba;
    BOOL is_connected;
  } irba_connections[MAX_INTERFACE_INDEX];

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_START_STATUS;
  MTLK_DECLARE_START_LOOP(DUT_IRBA_FIND_DEVICES);
  MTLK_DECLARE_START_LOOP(DUT_IRBA_ENABLE);
} _dut_api_t;

static _dut_api_t g_the_dut_api;
static BOOL g_isStarted = FALSE;
const char *_dut_api_drvctrl_script = NULL;

static void
_dut_api_help (const char *app_name)
{
  const struct mtlk_argv_param_info_ex *all_params[] = {
    &param_script,
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
                       "Metalink DUT Server v." MTLK_SOURCE_VERSION,
                       all_params,
                       (uint32)ARRAY_SIZE(all_params));
}

static int
_dut_api_process_commandline (int argc, char *argv[])
{
  int                res    = MTLK_ERR_UNKNOWN;
  BOOL               inited = FALSE;
  mtlk_argv_param_t *param  = NULL;
  mtlk_argv_parser_t argv_parser;

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

  param = mtlk_argv_parser_param_get(&argv_parser, &param_script.info);
  if (param) {
    _dut_api_drvctrl_script = mtlk_argv_parser_param_get_str_val(param);
    mtlk_argv_parser_param_release(param);
    if (!_dut_api_drvctrl_script) {
      ELOG_S("%s must be specified", param_script.desc);
      res = MTLK_ERR_VALUE;
      goto end;
    }
  }

  res = MTLK_ERR_OK;

end:
  if (inited) {
    mtlk_argv_parser_cleanup(&argv_parser);
  }

  if (res != MTLK_ERR_OK) {
    _dut_api_help(argv[0]);
  }

  return res;
}

static void __MTLK_IFUNC
_dut_api_irba_rm_handler (mtlk_irba_t   *irba,  mtlk_handle_t  context)
{
  MTLK_UNREFERENCED_PARAM(irba);
  MTLK_UNREFERENCED_PARAM(context);
}

static int
_dut_api_connect_to_hw(_dut_api_t *_dut_obj, int hw_idx)
{
  int res;
  char dut_irb_node_name[MTLK_IRB_VAP_NAME_MAX_LEN];

  MTLK_ASSERT(hw_idx < ARRAY_SIZE(_dut_obj->irba_connections));

  ILOG0_D("Looking for HW #%d...", hw_idx);

  _dut_obj->irba_connections[hw_idx].is_connected = FALSE;

  if(0 > sprintf_s(dut_irb_node_name, sizeof(dut_irb_node_name), "%s%d", MTLK_IRB_VAP_NAME, hw_idx))
  {
    ELOG_V("Failed to build DUT IRB node name");
    return MTLK_ERR_UNKNOWN;
  }

  _dut_obj->irba_connections[hw_idx].irba = mtlk_irba_alloc();
  if(NULL == _dut_obj->irba_connections[hw_idx].irba)
  {
    ELOG_V("Failed to allocate IRB node");
    return MTLK_ERR_NO_MEM;
  }
  ILOG0_S("irb connecting to interface %s...", dut_irb_node_name);
  res = mtlk_irba_init(_dut_obj->irba_connections[hw_idx].irba, dut_irb_node_name, _dut_api_irba_rm_handler, HANDLE_T(0));

  switch(res)
  {
  case MTLK_ERR_NO_ENTRY:
    /* No HW or HW not configured as DUT */
    mtlk_irba_free(_dut_obj->irba_connections[hw_idx].irba);
    _dut_obj->irba_connections[hw_idx].irba = NULL;
    return MTLK_ERR_OK;
  case MTLK_ERR_OK:
    /* Connected successfully */
    _dut_obj->irba_connections[hw_idx].is_connected = TRUE;
    ILOG0_D("Successfully connected to HW #%d", hw_idx);
    return MTLK_ERR_OK;
  default:
    ELOG_D("Failed to attach to DUT IRB node, error: %d", res);
    mtlk_irba_free(_dut_obj->irba_connections[hw_idx].irba);
    _dut_obj->irba_connections[hw_idx].irba = NULL;
    return res;
  }
}

static void
_dut_api_disconnect_from_hw(_dut_api_t *_dut_obj, int hw_idx)
{
  if(dut_api_is_connected_to_hw(hw_idx))
  {
    mtlk_irba_cleanup(_dut_obj->irba_connections[hw_idx].irba);
    mtlk_irba_free(_dut_obj->irba_connections[hw_idx].irba);
    _dut_obj->irba_connections[hw_idx].irba = NULL;
    _dut_obj->irba_connections[hw_idx].is_connected = FALSE;
  }
}

static int
_dut_api_enable(_dut_api_t *_dut_obj, int hw_idx)
{
  int res = MTLK_ERR_OK;

  if (dut_api_is_connected_to_hw(hw_idx))
  {
    res = mtlk_irba_call_drv(_dut_obj->irba_connections[hw_idx].irba,
                             &_IRBE_DUT_START_CMD, (void*)NULL, 0);

    if (res != MTLK_ERR_OK)
    {
      ELOG_DD("DUT: Failed to enable DUT mode: %d interface, error %d", hw_idx, res);
    }
  }

  return res;
}

static void
_dut_api_disable(_dut_api_t *_dut_obj, int hw_idx)
{
  int res = MTLK_ERR_OK;

  if (dut_api_is_connected_to_hw(hw_idx))
  {
    res = mtlk_irba_call_drv(_dut_obj->irba_connections[hw_idx].irba,
                             &_IRBE_DUT_STOP_CMD, (void*)NULL, 0);

    if (res != MTLK_ERR_OK)
    {
      ELOG_DD("DUT: Failed to disable DUT mode: %d interface, error %d", hw_idx, res);
    }
  }
}

#ifdef DUT_RUN_SCRIPT
static int
_dut_api_run_drv_ctrl(const char *command)
{
  int res;
  char drv_ctrl_cmd[_DUT_DRVCTRL_MAX_CMD_LEN];

  res = sprintf_s(drv_ctrl_cmd, ARRAY_SIZE(drv_ctrl_cmd), "%s %s",
                  (_dut_api_drvctrl_script ? _dut_api_drvctrl_script : _DUT_DRVCTRL_SCRIPT),
                  command);
  if (res < 0)
  {
    ELOG_D("Failed to prepare executable script, return value is %d", res);
    return res;
  }

  ILOG0_S("Running script: %s", drv_ctrl_cmd);
  res = system(drv_ctrl_cmd);

#ifndef IGNORE_SCRIPT_RES
  ILOG0_D("Return script result: %d.", res);
#else
  ILOG0_D("Ignore script result: %d, and return OK.", res);
  res = MTLK_ERR_OK;
#endif /* IGNORE_SCRIPT_RES */

  return res;
}
#endif /* DUT_RUN_SCRIPT  */

MTLK_START_STEPS_LIST_BEGIN(dut_api)
  MTLK_START_STEPS_LIST_ENTRY(dut_api, DUT_IRBA_CONNECT)
  MTLK_START_STEPS_LIST_ENTRY(dut_api, DUT_IRBA_START)
  MTLK_START_STEPS_LIST_ENTRY(dut_api, DUT_IRBA_FIND_DEVICES)
  MTLK_START_STEPS_LIST_ENTRY(dut_api, DUT_IRBA_ENABLE)
MTLK_START_INNER_STEPS_BEGIN(dut_api)
MTLK_START_STEPS_LIST_END(dut_api)

static void
_dut_api_stop(_dut_api_t *_dut_obj)
{
  int i;

  MTLK_STOP_BEGIN(dut_api, MTLK_OBJ_PTR(_dut_obj))
    for (i = 0; MTLK_STOP_ITERATIONS_LEFT(MTLK_OBJ_PTR(_dut_obj), DUT_IRBA_ENABLE) > 0; i++)
    {
      MTLK_STOP_STEP_LOOP(dut_api, DUT_IRBA_ENABLE, MTLK_OBJ_PTR(_dut_obj),
                          _dut_api_disable, (_dut_obj, i));
    }
    for (i = 0; MTLK_STOP_ITERATIONS_LEFT(MTLK_OBJ_PTR(_dut_obj), DUT_IRBA_FIND_DEVICES) > 0; i++)
    {
      MTLK_STOP_STEP_LOOP(dut_api, DUT_IRBA_FIND_DEVICES, MTLK_OBJ_PTR(_dut_obj),
                          _dut_api_disconnect_from_hw, (_dut_obj, i));
    }
    MTLK_STOP_STEP(dut_api, DUT_IRBA_START, MTLK_OBJ_PTR(_dut_obj),
                   mtlk_irba_app_stop, ());
    MTLK_STOP_STEP(dut_api, DUT_IRBA_CONNECT, MTLK_OBJ_PTR(_dut_obj),
                   mtlk_irba_app_cleanup, ());
  MTLK_STOP_END(dut_api, MTLK_OBJ_PTR(_dut_obj))
}

static int
_dut_api_start(_dut_api_t *_dut_obj)
{
  int i;

  MTLK_START_TRY(dut_api, MTLK_OBJ_PTR(_dut_obj))
    MTLK_START_STEP(dut_api, DUT_IRBA_CONNECT, MTLK_OBJ_PTR(_dut_obj),
                    mtlk_irba_app_init, (_dut_api_irba_rm_handler, HANDLE_T(0)));
    MTLK_START_STEP(dut_api, DUT_IRBA_START, MTLK_OBJ_PTR(_dut_obj),
                    mtlk_irba_app_start, ());
    for (i = 0; i < ARRAY_SIZE(_dut_obj->irba_connections); ++i)
    {
      MTLK_START_STEP_LOOP(dut_api, DUT_IRBA_FIND_DEVICES,  MTLK_OBJ_PTR(_dut_obj),
                           _dut_api_connect_to_hw, (_dut_obj, i));
    }
    for (i = 0; i < ARRAY_SIZE(_dut_obj->irba_connections); ++i)
    {
      MTLK_START_STEP_LOOP(dut_api, DUT_IRBA_ENABLE,  MTLK_OBJ_PTR(_dut_obj),
                           _dut_api_enable, (_dut_obj, i));
    }
  MTLK_START_FINALLY(dut_api, MTLK_OBJ_PTR(_dut_obj))
  MTLK_START_RETURN(dut_api, MTLK_OBJ_PTR(_dut_obj), _dut_api_stop, (_dut_obj))
}

MTLK_INIT_STEPS_LIST_BEGIN(dut_api)
  MTLK_INIT_STEPS_LIST_ENTRY(dut_api, DUT_OSDEP_LOG)
  MTLK_INIT_STEPS_LIST_ENTRY(dut_api, DUT_OSDEP_LOG_OPTS)
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  MTLK_INIT_STEPS_LIST_ENTRY(dut_api, DUT_RTLOG_APP)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(dut_api, DUT_OSAL_INIT)
MTLK_INIT_INNER_STEPS_BEGIN(dut_api)
  MTLK_INIT_STEPS_LIST_ENTRY(dut_api, DUT_PARSE_CMD_LINE)
MTLK_INIT_STEPS_LIST_END(dut_api)

static void
_dut_api_cleanup(_dut_api_t *_dut_obj)
{
  MTLK_CLEANUP_BEGIN(dut_api, MTLK_OBJ_PTR(_dut_obj))
    MTLK_CLEANUP_STEP(dut_api, DUT_OSAL_INIT, MTLK_OBJ_PTR(_dut_obj),
                      mtlk_osal_cleanup, ());
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_CLEANUP_STEP(dut_api, DUT_RTLOG_APP, MTLK_OBJ_PTR(_dut_obj),
                      mtlk_rtlog_app_cleanup, (&rtlog_info_data));
#endif
    MTLK_CLEANUP_STEP(dut_api, DUT_OSDEP_LOG_OPTS, MTLK_OBJ_PTR(_dut_obj),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(dut_api, DUT_OSDEP_LOG, MTLK_OBJ_PTR(_dut_obj),
                      _mtlk_osdep_log_cleanup, ());
  MTLK_CLEANUP_END(dut_api, MTLK_OBJ_PTR(_dut_obj))
}

static int
_dut_api_init(_dut_api_t *_dut_obj, int argc, char *argv[])
{
  MTLK_INIT_TRY(dut_api, MTLK_OBJ_PTR(_dut_obj))
    MTLK_INIT_STEP(dut_api, DUT_OSDEP_LOG, MTLK_OBJ_PTR(_dut_obj),
                   _mtlk_osdep_log_init, (IWLWAV_RTLOG_APP_NAME_DUTSERVER));
    MTLK_INIT_STEP_VOID(dut_api, DUT_OSDEP_LOG_OPTS, MTLK_OBJ_PTR(_dut_obj),
                        mtlk_osdep_log_enable_stderr_all, ());
#ifdef CONFIG_WAVE_RTLOG_REMOTE
    MTLK_INIT_STEP(dut_api, DUT_RTLOG_APP, MTLK_OBJ_PTR(_dut_obj),
                   mtlk_rtlog_app_init, (&rtlog_info_data, IWLWAV_RTLOG_APP_NAME_DUTSERVER));
#endif
    MTLK_INIT_STEP(dut_api, DUT_OSAL_INIT, MTLK_OBJ_PTR(_dut_obj),
                   mtlk_osal_init, ());
    MTLK_INIT_STEP(dut_api, DUT_PARSE_CMD_LINE, MTLK_OBJ_NONE,
                   _dut_api_process_commandline, (argc, argv));
  MTLK_INIT_FINALLY(dut_api, MTLK_OBJ_PTR(_dut_obj))
    MTLK_CLEANUP_STEP(dut_api, DUT_PARSE_CMD_LINE, MTLK_OBJ_NONE,
                      MTLK_NOACTION, ());
  MTLK_INIT_RETURN(dut_api, MTLK_OBJ_PTR(_dut_obj), _dut_api_cleanup, (_dut_obj))
}

void __MTLK_IFUNC
dut_api_cleanup(void)
{
  _dut_api_cleanup(&g_the_dut_api);
}

int __MTLK_IFUNC
dut_api_init(int argc, char *argv[])
{
  return _dut_api_init(&g_the_dut_api, argc, argv);
}

int __MTLK_IFUNC
dut_api_driver_stop(BOOL doReset)
{
#ifdef DUT_RUN_SCRIPT
  int res;
#endif /* DUT_RUN_SCRIPT  */

  if (TRUE == g_isStarted)
  {
    _dut_api_stop(&g_the_dut_api);
    g_isStarted = FALSE;
  }

#ifdef DUT_RUN_SCRIPT
  if (doReset)
  {
    ILOG0_S("Stopping driver with command %s", _DUT_DRVCTRL_CMD_STOP);
    res = _dut_api_run_drv_ctrl(_DUT_DRVCTRL_CMD_STOP);
    if (res != 0)
    {
      ELOG_D("Failed to stop driver, return value is %d", res);
      return MTLK_ERR_UNKNOWN;
    }
  }
  else
  {
    ILOG0_S("Stopping DUT helper with command %s", _DUT_DRVCTRL_CMD_STOP_HLPR);
    res = _dut_api_run_drv_ctrl(_DUT_DRVCTRL_CMD_STOP_HLPR);
    if (res != 0)
    {
      ELOG_D("Failed to stop DUT helper, return value is %d", res);
      return MTLK_ERR_UNKNOWN;
    }
  }
#endif /* DUT_RUN_SCRIPT  */

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
dut_api_driver_start(BOOL doReset)
{
  int res;

#ifdef DUT_RUN_SCRIPT
  if (doReset)
  {
    ILOG0_S("Starting driver with command %s", _DUT_DRVCTRL_CMD_START);
    res = _dut_api_run_drv_ctrl(_DUT_DRVCTRL_CMD_START);
    if (res != 0)
    {
      ELOG_D("Failed to start driver, return value is %d", res);
      return MTLK_ERR_UNKNOWN;
    }
  }
  else
  {
    ILOG0_S("New Starting DUT helper with command %s", _DUT_DRVCTRL_CMD_START_HLPR);
    res = _dut_api_run_drv_ctrl(_DUT_DRVCTRL_CMD_START_HLPR);
    if (res != 0)
    {
      ELOG_D("Failed to start DUT helper, return value is %d", res);
      return MTLK_ERR_UNKNOWN;
    }
  }
#endif /* DUT_RUN_SCRIPT  */

  if (FALSE == g_isStarted)
  {
    res = _dut_api_start(&g_the_dut_api);
    if (MTLK_ERR_OK != res)
    {
      ELOG_DS("Failed to start DUT API, error %d (%s)", res, mtlk_get_error_text(res));
#ifdef DUT_RUN_SCRIPT
      if (doReset)
        (void)_dut_api_run_drv_ctrl(_DUT_DRVCTRL_CMD_STOP);
      else
        (void)_dut_api_run_drv_ctrl(_DUT_DRVCTRL_CMD_STOP_HLPR);
#endif /* DUT_RUN_SCRIPT  */
    }
    else
    {
      g_isStarted = TRUE;
      res = MTLK_ERR_OK;
    }
  } 
  else 
  {
    res = MTLK_ERR_OK;
  }

  return res;
}

int __MTLK_IFUNC
dut_api_upload_progmodel (dut_progmodel_t *data, int length, int hw_idx)
{
  int res;

  MTLK_ASSERT(dut_api_is_connected_to_hw(hw_idx));

  res = mtlk_irba_call_drv(g_the_dut_api.irba_connections[hw_idx].irba,
                           &_IRBE_DUT_PROGMODEL_CMD,
                           (void*) data, length);
  if (res != MTLK_ERR_OK)
  {
    ELOG_D("DUT: Failed to load progmodel file, error %d", res);
  } else {
    res = data->status;
  }
  return res;
}

static int 
dut_api_get_irba_cmd_from_msg_id(
      dutDriverMessagesId_e msg_id, const mtlk_guid_t **pp_cmd)
{
  int res;
  static const mtlk_guid_t  *svr_msg_to_irbe[] = {
    [DUT_SERVER_MSG_ADD_VAP] = &_IRBE_DUT_ADD_VAP_CMD,
    [DUT_SERVER_MSG_REMOVE_VAP] = &_IRBE_DUT_REMOVE_VAP_CMD,
    [DUT_SERVER_MSG_SET_BSS] = &_IRBE_DUT_SET_BSS_CMD,
    [DUT_SERVER_MSG_SET_WMM_PARAMETERS] = &_IRBE_DUT_SET_WMM_PARAMETERS_CMD,
    [DUT_SERVER_MSG_STOP_VAP_TRAFFIC] = &_IRBE_DUT_STOP_VAP_TRAFFIC_CMD,
    [DUT_SERVER_MSG_DRIVER_FW_GENERAL] = &_IRBE_DUT_DRIVER_FW_GENERAL_CMD,
    [DUT_SERVER_MSG_PLATFORM_DATA_FIELDS] = &_IRBE_DUT_PLATFORM_DATA_FIELDS_CMD,
  };
  
  if ((msg_id <= DUT_SERVER_MSG_DRIVER_GENERAL) || (msg_id >= DUT_SERVER_MSG_CNT)) {
    *pp_cmd = NULL;
    res = MTLK_ERR_PARAMS;
  } else {
    *pp_cmd = svr_msg_to_irbe[msg_id];
    res = MTLK_ERR_OK;
  }
  return res;
}

int __MTLK_IFUNC
dut_api_send_dut_core_command(
      dutDriverMessagesId_e in_msg_id, 
      const char *data, int length, int hw_idx)
{
  int res;
  static const mtlk_guid_t *p_cmd;

  res = dut_api_get_irba_cmd_from_msg_id(in_msg_id, &p_cmd);
  if (MTLK_ERR_OK != res)
  {
    ELOG_D("Failed to translate message to IRB command, error %d", res);
  } 
  else 
  {
    res = mtlk_irba_call_drv(g_the_dut_api.irba_connections[hw_idx].irba,
                           p_cmd, (void*) data, length);
    if (res != MTLK_ERR_OK)
    {
      ELOG_D("DUT: Failed to send DUT FW request, error %d", res);
    }
  }
  return res;
}

int __MTLK_IFUNC
dut_api_send_fw_command(const char *data, int length, int hw_idx)
{
  int res;

  MTLK_ASSERT(dut_api_is_connected_to_hw(hw_idx));

  res = mtlk_irba_call_drv(g_the_dut_api.irba_connections[hw_idx].irba,
                           &_IRBE_DUT_FW_CMD, (void*) data, length);
  if (res != MTLK_ERR_OK)
  {
    ELOG_D("DUT: Failed to send DUT FW request, error %d", res);
  }
  return res;
}

int __MTLK_IFUNC
dut_api_send_drv_command(const char *data, int length, int hw_idx)
{
  int res;

  MTLK_ASSERT(dut_api_is_connected_to_hw(hw_idx));

  res = mtlk_irba_call_drv(g_the_dut_api.irba_connections[hw_idx].irba,
                           &_IRBE_DUT_DRV_CMD, (void*) data, length);
  if (res != MTLK_ERR_OK)
  {
    ELOG_D("DUT: Failed to send DUT FW request, error %d", res);
  }
  return res;
}

int __MTLK_IFUNC
dut_api_send_platform_command(const char *data, int length, int hw_idx)
{
  mtlk_error_t res = MTLK_ERR_OK;
  int fd = -1;
  int size, len, st;
  FILE *fp = NULL;
  char delim[] = "= ";
  char *ptr = NULL, *path = NULL;
  char cmd[_DUT_PLATFORM_MAX_CMD_LEN];
  char dut_plat_script[_DUT_PLATFORM_MAX_CMD_LEN];
  dutMessage_t *msg;
  msg = (dutMessage_t*)data;

  switch(DUT_TO_HOST16(msg->msgId))
  {
  case DUT_PGM_GET_CV_REQ:
    msg->msgId   = HOST_TO_DUT16(DUT_PGM_GET_CV_CFM);
    msg->status  = HOST_TO_DUT16(DUT_STATUS_PASS);

    fd = open(_DUT_PLATFORM_FILE_PATH, O_RDONLY);
    if (-1 == fd)
    {
      ELOG_S("DUT: Cannot open platform file [%s]", _DUT_PLATFORM_FILE_PATH);
      res = MTLK_ERR_FILEOP;
      goto end;
    }
    else
    {
      size = read(fd, cmd, _DUT_PLATFORM_MAX_CMD_LEN);
      if (size <= 0)
      {
        ELOG_S("Failed to read message from FILE :%s", _DUT_PLATFORM_FILE_PATH);
        res = MTLK_ERR_FILEOP;
        goto end;
      }
      cmd[size-1] = '\0';

      if (-1 == close(fd))
      {
        ELOG_S("Cannot close platform file [%s]",
                strerror(errno));
        return MTLK_ERR_FILEOP;
      }
    }

    ptr = strtok(cmd, delim);
    while(ptr != NULL)
    {
      path = ptr;
      ptr = strtok(NULL, delim);
    }

    if(path == NULL)
    {
      res = MTLK_ERR_FILEOP;
      goto end;
    }

    sprintf_s(dut_plat_script, sizeof(dut_plat_script), "%s dut_get_cv", path);

    fp = popen(dut_plat_script, "r");
    if (fp == NULL)
    {
      ELOG_V("DUT: Cannot open pipe for output");
      res = MTLK_ERR_FILEOP;
      goto end;
    }
    else
    {
      len = fread(msg->data, sizeof(char), DUT_MSG_DATA_LENGTH_IN_WORDS, fp);
      if (len == 0)
      {
        ELOG_V("Failed to read message from FILE");
        res = MTLK_ERR_FILEOP;
        goto end;
      }
      else
      {
        msg->data[len - 1] = '\0';
      }
      st = pclose(fp);
      if (WIFEXITED(st))
      {
        if (!WEXITSTATUS(st))
        {
          return MTLK_ERR_OK;
        }
        else
        {
          ELOG_SD("DUT: popen: cmd: %s exit code: %i", dut_plat_script, WEXITSTATUS(st));
          return MTLK_ERR_OK;
        }
      }
    }
    res = MTLK_ERR_OK;
    break;
  case DUT_PGM_COLLECT_DEBUG_INFO_REQ:
    {
      uint8 ip[4];
      uint32 hostIP;
      char strIP[32]={'\0'};
      char output[1024];
      dutCollectDebugInfoReq_t *dbgReq = NULL;
      dutDebugInfoType_e dgbType;

      msg->msgId = HOST_TO_DUT16(DUT_PGM_COLLECT_DEBUG_INFO_CFM);
      msg->status = HOST_TO_DUT16(DUT_STATUS_PASS);
      dbgReq = (dutCollectDebugInfoReq_t*)(msg->data);
      hostIP = HOST_TO_DUT32(dbgReq->hostIP);
      dgbType = dbgReq->type;
      ip[0] = ((hostIP & 0xFF000000) >> 0x18);
      ip[1] = ((hostIP & 0x00FF0000) >> 0x10);
      ip[2] = ((hostIP & 0x0000FF00) >> 0x8);
      ip[3] = (hostIP & 0x000000FF);

      sprintf_s(strIP, sizeof(strIP), "%d.%d.%d.%d", ip[3], ip[2], ip[1], ip[0]);
      if ((fd = open(_DUT_PLATFORM_FILE_PATH, O_RDONLY)) == -1)
      {
        ELOG_S("DUT: Cannot open platform file [%s]", _DUT_PLATFORM_FILE_PATH);
        res = MTLK_ERR_FILEOP;
        goto end;
      }
      else
      {
        size = read(fd, cmd, _DUT_PLATFORM_MAX_CMD_LEN);
        if (size <= 0)
        {
          ELOG_S("DUT: Failed to read message from FILE :%s", _DUT_PLATFORM_FILE_PATH);
          res = MTLK_ERR_FILEOP;
          goto end;
        }
        cmd[size-1] = '\0';

        if (-1 == close(fd))
        {
          ELOG_S("DUT: Cannot close platform file [%s]",  strerror(errno));
          return MTLK_ERR_FILEOP;
        }
      }

      ptr = strtok(cmd, delim);
      while (ptr != NULL)
      {
        path = ptr;
        ptr = strtok(NULL, delim);
      }

      if (path == NULL)
      {
        res = MTLK_ERR_FILEOP;
        goto end;
      }

      switch(dgbType)
      {
        case DUT_DEBUGINFO_DEFAULT:
          sprintf_s(dut_plat_script, sizeof(dut_plat_script), "%s wcdd %s", path, strIP);
          break;
        case DUT_DEBUGINFO_FWASSERT:
          sprintf_s(dut_plat_script, sizeof(dut_plat_script), "%s wcda %s", path, strIP);
          break;
        case DUT_DEBUGINFO_EXTENSION:
          sprintf_s(dut_plat_script, sizeof(dut_plat_script), "%s wcde %s", path, strIP);
          break;
      }

      ILOG0_S("DUT: Run debug log collect cmd:%s", dut_plat_script);
      if (!(fp = popen(dut_plat_script, "r")))
      {
        ELOG_V("DUT: Cannot open pipe for output");
        res = MTLK_ERR_FILEOP;
        goto end;
      }
      else
      {
        if (!(len = fread(output, sizeof(char), 1024, fp)))
        {
          ELOG_V("DUT: Failed to read message from FILE");
          goto end;
        }
        else
        {
          output[len - 1] = '\0';
        }
        st = pclose(fp);
        if (WIFEXITED(st))
        {
          if (!WEXITSTATUS(st))
          {
            return MTLK_ERR_OK;
          }
          else
          {
            ELOG_SD("DUT: popen: cmd: %s exit code: %i", dut_plat_script, WEXITSTATUS(st));
            return MTLK_ERR_OK;
          }
        }
      }

      res = MTLK_ERR_OK;
      break;
  }
  default:
    break;
  }
  return res;
end:

  if (-1 == close(fd))
  {
    ELOG_S("Cannot close platform file [%s]", _DUT_PLATFORM_FILE_PATH);
  }

  if (fp)
  {
    st = pclose(fp);
    if (WIFEXITED(st))
    {
      ELOG_SD("popen: cmd: %s exit code: %i", dut_plat_script, WEXITSTATUS(st));
      return MTLK_ERR_FILEOP;
    }
  }
  return res;
}

int __MTLK_IFUNC
dut_api_eeprom_data_on_flash_prepare(uint32 size, int hw_idx)
{
  char f_name[0x20];
  dutMessage_t msg;
  int res;
  struct stat st;
  int fd = -1;
  uint8 b[64];

  MTLK_ASSERT(dut_api_is_connected_to_hw(hw_idx));

  if (0 == size)
    return MTLK_ERR_OK; /* nothing to do */

  memset(&msg, 0, sizeof(msg));
  msg.msgId = HOST_TO_DUT16(DUT_DGM_GET_NV_FLASH_NAME_REQ);
  msg.msgLength = HOST_TO_DUT16(sizeof(msg.data) >> 1);

  res = mtlk_irba_call_drv(g_the_dut_api.irba_connections[hw_idx].irba,
                           &_IRBE_DUT_DRV_CMD, (void*) &msg, sizeof(msg));
  if (res != MTLK_ERR_OK)
  {
    ELOG_D("DUT: Failed to send DUT DRV request, error %d", res);
    return res;
  }

  sprintf_s(f_name, sizeof(f_name), "/tmp/%s", (char*)msg.data);

  if (0 == stat(f_name, &st))
  {
    ELOG_S("DUT: EEPROM data file '%s' already exists", f_name);
    return MTLK_ERR_OK; /* already exists */
  }

  fd = open(f_name, O_CREAT | O_WRONLY | O_EXCL, S_IRUSR | S_IWUSR);
  if (-1 == fd)
  {
    ELOG_SS("DUT: Can not create EEPROM data file '%s' [%s]",
            f_name, strerror(errno));
    return MTLK_ERR_EEPROM;
  }

  memset(b, 0, sizeof(b));

  while(size > 0)
  {
    int to_write = sizeof(b);

    if (size < to_write)
      to_write = size;

    write(fd, b, to_write);

    size -= to_write;
  }

  if (-1 == close(fd))
  {
    ELOG_SS("DUT: Can not close EEPROM data file '%s' [%s]",
            f_name, strerror(errno));
    return MTLK_ERR_EEPROM;
  }

  return MTLK_ERR_OK;
}

BOOL __MTLK_IFUNC
dut_api_is_connected_to_hw(int hw_idx)
{
  MTLK_ASSERT(hw_idx < ARRAY_SIZE(g_the_dut_api.irba_connections));
  return g_the_dut_api.irba_connections[hw_idx].is_connected;
};

