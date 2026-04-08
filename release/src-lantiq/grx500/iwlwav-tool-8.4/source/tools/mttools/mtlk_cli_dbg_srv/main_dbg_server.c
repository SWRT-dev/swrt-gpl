/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlkerr.h"
#include "mtlk_osal.h"
#include "mtlk_cli_server.h"
#include "mtlk_cli_port.h"
#include "argv_parser.h"

#include <stdarg.h>

#define LOG_LOCAL_GID   GID_MAIN_DBG_SERVER
#define LOG_LOCAL_FID   1

static const struct mtlk_argv_param_info_ex param_step_to_fail = {
  {
    NULL,
    "step_to_fail",
    MTLK_ARGV_PINFO_FLAG_HAS_INT_DATA
  },
  "step to simulate failure (start up macros, DEBUG version only)",
  MTLK_ARGV_PTYPE_OPTIONAL
};

static const struct mtlk_argv_param_info_ex param_cli_id =  {
  {
    "c",
    "cli-id",
    MTLK_ARGV_PINFO_FLAG_HAS_STR_DATA
  },
  "MTLK CLI ID string used to determine port number",
  MTLK_ARGV_PTYPE_MANDATORY
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
    &param_step_to_fail,
    &param_cli_id,
    &param_help,
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
                       "MTLK CLI debug server application",
                       all_params,
                       (uint32)ARRAY_SIZE(all_params));
}

static void __MTLK_IFUNC
cli_srv_clb_on_exit (mtlk_cli_srv_t           *srv,
                     const mtlk_cli_srv_cmd_t *cmd,
                     mtlk_cli_srv_rsp_t       *rsp,
                     mtlk_handle_t             ctx)
{
  volatile int *stop = HANDLE_T_PTR(volatile int, ctx);
  ILOG0_S("Exit callback received: %s!", mtlk_cli_srv_cmd_get_name(cmd));
  mtlk_cli_srv_rsp_add_str(rsp, "Exiting...");
  *stop = 1;
}

int
main_server (int argc, char *argv[])
{
  int                 res     = MTLK_ERR_UNKNOWN;
  mtlk_cli_srv_t     *srv     = NULL;
  const char         *cli_id  = NULL;
  int                 started = 0;
  int                 exported = 0;
  mtlk_cli_srv_clb_t *srv_clb = NULL;
  volatile int        stop    = 0; 
  mtlk_argv_parser_t  argv_parser;
  mtlk_argv_param_t  *param   = NULL;
  int                 argvparser_inited = 0;
  BOOL                print_help    = FALSE;

  res = mtlk_argv_parser_init(&argv_parser, argc, argv);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init argv parser (err=%d)", res);
    goto end;
  }
  argvparser_inited = 1;

  param = mtlk_argv_parser_param_get(&argv_parser, &param_help.info);
  if (param) {
    mtlk_argv_parser_param_release(param);
    print_help = TRUE;
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
      print_help = TRUE;
      goto end;
    }
  }
#endif

  param = mtlk_argv_parser_param_get(&argv_parser, &param_cli_id.info);
  if (param) {
    cli_id = mtlk_argv_parser_param_get_str_val(param);
    mtlk_argv_parser_param_release(param);

    if (NULL == cli_id) {
      ELOG_V("Invalid CLI ID specified");
      goto end;
    }
  }
  else {
    ELOG_V("CLI ID must be specified");
    print_help = TRUE;
    goto end;
  }

  srv = mtlk_cli_srv_create();
  if (!srv) {
    res = MTLK_ERR_NO_MEM;
    ELOG_V("Can't create server object");
    goto end;
  }

  res = mtlk_cli_srv_start(srv, 0);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't start server (err=%d)", res);
    goto end;
  }
  started = 1;

  ILOG0_D("Server is bound on port#%u", (unsigned)mtlk_cli_srv_get_bound_port(srv));

  res = mtlk_cli_port_export_init(mtlk_cli_srv_get_bound_port(srv), cli_id);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't export port (err=%d)", res);
    goto end;
  }
  exported = 1;

  srv_clb = mtlk_cli_srv_register_cmd_clb(srv,
                                          "exit",
                                          cli_srv_clb_on_exit,
                                          HANDLE_T(&stop));
  if (!srv_clb) {
    ELOG_V("Can't register Exit callback");
    goto end;   
  }

  while (!stop) {
    mtlk_osal_msleep(20);
  }

end:
  if (srv_clb) {
    mtlk_cli_srv_unregister_cmd_clb(srv_clb);
  }

  if (exported) {
    mtlk_cli_port_export_cleanup();
  }

  if (started) {
    mtlk_cli_srv_stop(srv);
  }

  if (srv) {
    mtlk_cli_srv_delete(srv);
  }

  if (print_help) {
    _print_help(argv[0]);
  }

  if (argvparser_inited) {
    mtlk_argv_parser_cleanup(&argv_parser);
  }

  return res;
}

MTLK_DECLARE_OBJPOOL(g_objpool);

int
main (int argc, char *argv[])
{
  int    res;
  int    objpool_inited = 0;

  res = mtlk_objpool_init(&g_objpool);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init objpool (err=%d)", res);
    goto end;
  }
  objpool_inited = 1;

  res = main_server(argc, argv);

end:
  if (objpool_inited) {
    mtlk_objpool_cleanup(&g_objpool);
  }

  return (res == MTLK_ERR_OK)?0:-1;
}

