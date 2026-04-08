/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlkerr.h"
#include "mtlk_objpool.h"
#include "mtlk_osal.h"
#include "mtlk_cli_port.h"
#include "mtlk_cli_defs.h"

#include <stdarg.h>

#include "mtlk_cli_client.h"

#define LOG_LOCAL_GID   GID_MTLK_CLI
#define LOG_LOCAL_FID   1

#define APP_NAME "mtlk_cli"

static __INLINE void
MTLK_PRINT (const char *fmt, ...)
{
  va_list va;

  va_start(va, fmt);
  vfprintf(stdout, fmt, va);
  va_end(va);
  fprintf(stdout, "\n");
}

static __INLINE void
MTLK_ERROR (const char *fmt, ...)
{
  va_list va;

  fprintf(stderr, "ERROR: ");
  va_start(va, fmt);
  vfprintf(stderr, fmt, va);
  va_end(va);
  fprintf(stderr, "\n");
}

static __INLINE const char *
MTLK_MAP_ERROR (int err)
{
  static char err_str[256];

  switch (err) {
  case MTLK_ERR_NOT_SUPPORTED:
    return "command is not supported";
  default:
    snprintf(err_str, sizeof(err_str), "unknown error (%d)", err);
    return err_str;
  }
}

static void
_print_help (const char *app_name)
{
  const char *app_fname = strrchr(app_name, '/');

  if (!app_fname) {
    app_fname = app_name;
  }
  else {
    ++app_fname; /* skip '/' */
  }

  printf("Usage: %s <cli_server_id> <cmd> [arg1 [arg2 [...]]]\n", app_fname);
}

int
main_client (int argc, char *argv[])
{
  int             res     = MTLK_ERR_UNKNOWN;
  uint16          port    = 0;
  mtlk_cli_clt_t *clt     = NULL;
  mtlk_cli_rsp_t *rsp     = NULL;
  int             started = 0;
  int             i       = 0;
  const char     *cmd_name= MTLK_CLI_LS_CMD; /* default command - list all commands */

  if (argc == 1) {
    MTLK_ERROR("Server must be specified!");
    _print_help(argv[0]);
    goto end;
  }

  res = mtlk_cli_port_import(argv[1], &port);
  if (res != MTLK_ERR_OK) {
    MTLK_ERROR("No such server: '%s'", argv[1]);
    goto end;
  }

  clt = mtlk_cli_clt_create();
  if (!clt) {
    res = MTLK_ERR_NO_MEM;
    MTLK_ERROR("[internal] Can't create client object");
    goto end;
  }

  res = mtlk_cli_clt_start(clt, port);
  if (res != MTLK_ERR_OK) {
    MTLK_ERROR("[internal] Can't start client (err=%d port=%u)", res, port);
    goto end;
  }
  started = 1;

  if (argc > 2) {
    cmd_name = argv[2];
  }

  res = mtlk_cli_clt_send_cmd_chunk(clt, cmd_name);
  if (res != MTLK_ERR_OK) {
    MTLK_ERROR("[internal] Can't send cmd name (err=%d)", res);
    goto end;
  }

  for (i = 3; i < argc; i++) {
    res = mtlk_cli_clt_send_cmd_chunk(clt, argv[i]);
    if (res != MTLK_ERR_OK) {
      MTLK_ERROR("[internal] Can't send cmd chunk (err=%d)", res);
      goto end;
    }
  }

  rsp = mtlk_cli_clt_send_cmd_done(clt);
  if (!rsp) {
    res = MTLK_ERR_TIMEOUT;
    MTLK_ERROR("No responce received");
    goto end;
  }

  for (i = 0; i < mtlk_cli_rsp_get_nof_strings(rsp); i++) {
    const char *str = mtlk_cli_rsp_get_string(rsp, i);
    MTLK_PRINT("%s", str);
  }
  res = mtlk_cli_rsp_get_err(rsp);

  if (res != MTLK_ERR_OK) {
    const char *err_str = MTLK_MAP_ERROR(res);
    MTLK_ERROR("%s", err_str);
  }

end:
  if (rsp) {
    mtlk_cli_rsp_release(rsp);
  }

  if (started) {
    mtlk_cli_clt_stop(clt);
  }

  if (clt) {
    mtlk_cli_clt_delete(clt);
  }

  return res;
}

MTLK_DECLARE_OBJPOOL(g_objpool);

int
main (int argc, char *argv[])
{
  int    res;
  int    log_inited = 0;
  int    objpool_inited = 0;

  res = _mtlk_osdep_log_init(APP_NAME);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init OSDEP log (err=%d)", res);
    goto end;
  }
  log_inited = 1;

  res = mtlk_objpool_init(&g_objpool);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init objpool (err=%d)", res);
    goto end;
  }
  objpool_inited = 1;

  res = main_client(argc, argv);

end:
  if (objpool_inited) {
    mtlk_objpool_cleanup(&g_objpool);
  }

  if (log_inited) {
    _mtlk_osdep_log_cleanup();
  }

  return (res == MTLK_ERR_OK)?0:-1;
}

