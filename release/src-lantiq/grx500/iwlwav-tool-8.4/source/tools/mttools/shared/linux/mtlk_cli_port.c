/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include "mtlk_cli_port.h"

#include <unistd.h>

#define LOG_LOCAL_GID   GID_CLI
#define LOG_LOCAL_FID   1

/*! \file  mtlk_cli_port.c

    \brief MTLK CLI port number import/export impelentation.

    MTLK CLI port number import/export auxiliary module Linux-specific impelentation.
    The logic behind this implementation is following: 
    - server side creates a tmp text file with the name that is build from a special
      template and contains the server's name
    - this text file contains 2 numbers: <port_no>,<server_pid>
    - client side searches for the file using the server's name
    - if file exists, client checks the PID and verifies the server's process state.
    - if server process is alive, client gets the port read from tmp text file
*/

#define MTLK_CLI_PORT_FNAME_FMT  "/tmp/mtlk_cli_port_%s"
#define MTLK_CLI_PORT_NO_FMT     "%u,%u\n"
#define MTLK_CLI_PORT_FNAME_SIZE 256
#define PID_FNAME_FMT           "/proc/%u/exe"

struct mtlk_cli_port_export
{
  char fname[MTLK_CLI_PORT_FNAME_SIZE];
  MTLK_DECLARE_INIT_STATUS;
};

MTLK_INIT_STEPS_LIST_BEGIN(cli_port)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_port, GET_FNAME)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_port, CHECK_EXPORT_FNAME_NOT_BUSY)
MTLK_INIT_INNER_STEPS_BEGIN(cli_port)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_port, OPEN_EXPORT_FILE)
  MTLK_INIT_STEPS_LIST_ENTRY(cli_port, EXPORT_PORT_NO)
MTLK_INIT_STEPS_LIST_END(cli_port);

static struct mtlk_cli_port_export port_export_object;

static int
_mtlk_cli_file_exists (const char *fname)
{
  int   res = MTLK_ERR_NOT_IN_USE;
  FILE *fd  = fopen(fname, "r");
  if (fd) {
    fclose(fd);
    res = MTLK_ERR_OK;
  }

  return res;
}

static int
_mtlk_cli_port_get_fname (const char *name, char *fname, size_t size)
{
  int res;

  /* Avoid path traversal. Only simple names are allowed */
  if (strpbrk(name, "\\/.%"))
    return MTLK_ERR_PARAMS;

  res = snprintf(fname, size, MTLK_CLI_PORT_FNAME_FMT, name);
  if (res < 0)
    return MTLK_ERR_PARAMS;
  if (res >= size)
    return MTLK_ERR_BUF_TOO_SMALL;

  return MTLK_ERR_OK;
}

static int
_mtlk_cli_port_server_alive (const char *fname, uint16 *port)
{
  int          res      = MTLK_ERR_UNKNOWN;
  unsigned int port_val = 0;
  unsigned int pid_val  = 0;
  FILE        *fd       = NULL;
  char         pid_fname[MTLK_CLI_PORT_FNAME_SIZE];

  fd = fopen(fname, "r");
  if (!fd) {
    res = MTLK_ERR_PROHIB;
    goto end;
  }

  res = fscanf(fd, MTLK_CLI_PORT_NO_FMT, &port_val, &pid_val);
  if (res != 2) {
    res = MTLK_ERR_FILEOP;
    goto end;
  }

  res = snprintf(pid_fname, sizeof(pid_fname), PID_FNAME_FMT, pid_val);
  if (res >= sizeof(pid_fname)) {
    res = MTLK_ERR_BUF_TOO_SMALL;
    goto end;
  }

  if (!port_val ||
      port_val <= 1024 ||
      port_val > (unsigned int)((uint16)-1)) {
    res = MTLK_ERR_VALUE;
    goto end;
  }

  res = _mtlk_cli_file_exists(pid_fname);
  if (res != MTLK_ERR_OK) { /* PID file doesn't exists, so server isn't running (crashed?) */
    goto end;
  }

  if (port) {
    *port = (uint16)port_val;
  }
  res = MTLK_ERR_OK;
  
end:
  if (fd) {
    fclose(fd);
  }

  return res;
}

int __MTLK_IFUNC
mtlk_cli_port_export_init (uint16 port_no, const char *name)
{
  struct mtlk_cli_port_export *exp_obj = &port_export_object;
  int   tmp;
  FILE *fd = NULL;

  MTLK_ASSERT(port_no != 0);
  MTLK_ASSERT(name != NULL);
  MTLK_ASSERT(name[0] != 0);

  memset(exp_obj, 0, sizeof(*exp_obj));

  MTLK_INIT_TRY(cli_port, MTLK_OBJ_PTR(exp_obj))
    MTLK_INIT_STEP(cli_port, GET_FNAME, MTLK_OBJ_PTR(exp_obj),
                   _mtlk_cli_port_get_fname, (name, exp_obj->fname, sizeof(exp_obj->fname)));
    MTLK_INIT_STEP_EX(cli_port, CHECK_EXPORT_FNAME_NOT_BUSY, MTLK_OBJ_PTR(exp_obj),
                      _mtlk_cli_port_server_alive, (exp_obj->fname, NULL),
                      tmp,
                      tmp != MTLK_ERR_OK, /* we can re-use the file only if there is no other server alive */
                      MTLK_ERR_BUSY);
    MTLK_INIT_STEP_EX(cli_port, OPEN_EXPORT_FILE, MTLK_OBJ_NONE,
                      fopen, (exp_obj->fname, "w"),
                      fd,
                      fd != NULL,
                      MTLK_ERR_PROHIB);
    MTLK_INIT_STEP_EX(cli_port, EXPORT_PORT_NO, MTLK_OBJ_NONE,
                      fprintf, (fd, MTLK_CLI_PORT_NO_FMT, (unsigned int)port_no, (unsigned int)getpid()),
                      tmp,
                      tmp >= 0,
                      MTLK_ERR_FILEOP);
  MTLK_INIT_FINALLY(cli_port, MTLK_OBJ_PTR(exp_obj))
    MTLK_CLEANUP_STEP(cli_port, EXPORT_PORT_NO, MTLK_OBJ_NONE,
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(cli_port, OPEN_EXPORT_FILE, MTLK_OBJ_NONE,
                      fclose, (fd));
  MTLK_INIT_RETURN(cli_port, MTLK_OBJ_PTR(exp_obj), mtlk_cli_port_export_cleanup, ())
}

void __MTLK_IFUNC
mtlk_cli_port_export_cleanup (void)
{
  struct mtlk_cli_port_export *exp_obj = &port_export_object;

  MTLK_CLEANUP_BEGIN(cli_port, MTLK_OBJ_PTR(exp_obj))
    MTLK_CLEANUP_STEP(cli_port, CHECK_EXPORT_FNAME_NOT_BUSY, MTLK_OBJ_PTR(exp_obj),
                      unlink, (exp_obj->fname));
    MTLK_CLEANUP_STEP(cli_port, GET_FNAME, MTLK_OBJ_PTR(exp_obj),
                      MTLK_NOACTION, ());
  MTLK_CLEANUP_END(cli_port, MTLK_OBJ_PTR(exp_obj))
}

int __MTLK_IFUNC
mtlk_cli_port_import (const char *name, uint16 *port_no)
{
  int  res  = MTLK_ERR_UNKNOWN;
  char fname[MTLK_CLI_PORT_FNAME_SIZE];

  MTLK_ASSERT(port_no != NULL);
  MTLK_ASSERT(name != NULL);
  MTLK_ASSERT(name[0] != 0);

  res = _mtlk_cli_port_get_fname(name, fname, sizeof(fname));
  if (res != MTLK_ERR_OK) {
    goto end;
  }

  res = _mtlk_cli_port_server_alive(fname, port_no);
  if (res != MTLK_ERR_OK) {
    goto end;
  }

  res = MTLK_ERR_OK;

end:
  return res;
}
