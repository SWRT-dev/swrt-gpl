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
#include "mtlkerr.h"

#include <stdarg.h>
#include <syslog.h>

#define LOG_LOCAL_GID   GID_UTILS
#define LOG_LOCAL_FID   0

int debug = 0;
int debug_remote = -1;
static char module_name[IFNAMSIZ]; 

#define MAX_PRINT_BUFF_SIZE 512

#define MTLK_PRINT_USE_SYSLOG

int  __MTLK_IFUNC
_mtlk_osdep_log_init (const char *app_name)
{
#ifdef MTLK_PRINT_USE_SYSLOG
  openlog(app_name, LOG_PID, LOG_USER);
#endif
  if (!wave_strcopy(module_name, app_name, sizeof(module_name)))
    return MTLK_ERR_BUF_TOO_SMALL;

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
_mtlk_osdep_log_cleanup (void)
{
#ifdef MTLK_PRINT_USE_SYSLOG
  closelog();
#endif
}

struct mtlk_osdep_level_info
{
  const char *prefix;
  int         syslog_priority;
  int         duplicate_to_stderr;
};

static struct mtlk_osdep_level_info
osdep_level_info[] = {
  {
    .prefix              = "ERROR: ",
    .syslog_priority     = LOG_ERR,
    .duplicate_to_stderr = 0
  },
  {
    .prefix              = "WARNING: ",
    .syslog_priority     = LOG_WARNING,
    .duplicate_to_stderr = 0
  },
  {
    .prefix              = "",
    .syslog_priority     = LOG_USER,
    .duplicate_to_stderr = 0
  },
};

BOOL __MTLK_IFUNC
_mtlk_osdep_log_is_enabled_stderr (mtlk_osdep_level_e level)
{
  if (level < ARRAY_SIZE(osdep_level_info)) {
    return osdep_level_info[level].duplicate_to_stderr;
  }
  return FALSE;
}

int __MTLK_IFUNC 
_mtlk_osdep_log_enable_stderr (mtlk_osdep_level_e level,
                               BOOL               enable)
{
  if (level < ARRAY_SIZE(osdep_level_info)) {
    osdep_level_info[level].duplicate_to_stderr = (enable != FALSE);
    return MTLK_ERR_OK;
  }

  return MTLK_ERR_PARAMS;
}

void __MTLK_IFUNC
mtlk_osdep_log_enable_stderr_all (void)
{
  _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_ERR, TRUE);
  _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_WARN, TRUE);
  _mtlk_osdep_log_enable_stderr(MTLK_OSLOG_INFO, TRUE);
}

void __MTLK_IFUNC 
mtlk_osdep_log (mtlk_osdep_level_e level,
                const char        *func,
                int                line,
                const char        *log_level,
                const char        *fmt,
                ...)
{
  const struct mtlk_osdep_level_info *linfo;
  char    buff[MAX_PRINT_BUFF_SIZE] = {0};
  int     n = 0, offs = 0;
  va_list ap;

  MTLK_ASSERT(level < ARRAY_SIZE(osdep_level_info));

  linfo = &osdep_level_info[level];

  n = sprintf_s(buff + offs, sizeof(buff) - offs,
                "[%010lu] %s%s[%s:%d]: ",
                timestamp(), module_name, log_level, func, line);
  if (n >= sizeof(buff) - offs) {
    goto end;
  }
  offs += n;

  va_start(ap, fmt);
  vsnprintf_s(buff + offs, sizeof(buff) - offs,
             fmt,
             ap);
  va_end(ap);

end:
  buff[sizeof(buff) - 1] = 0;

#ifdef MTLK_PRINT_USE_SYSLOG
  syslog(linfo->syslog_priority, "%s", buff);
  if (linfo->duplicate_to_stderr) {
    fprintf(stderr, "%s\n", buff);
  }
#else
  fprintf(stderr, "%s\n", buff);
#endif
}

int __MTLK_IFUNC
mtlk_get_current_executable_name(char* buf, size_t size)
{
  char linkname[64]; /* /proc/<pid>/exe */
  pid_t pid;
  int res;
  char *outname;

  /* Get our PID and build the name of the link in /proc */
  pid = getpid();

  if (sprintf_s(linkname, sizeof(linkname), "/proc/%i/exe", pid) < 0)
  {
    /* This should only happen on large word systems. I'm not sure
       what the proper response is here.
       It really is an assert-like condition. */
    MTLK_ASSERT(FALSE);
    return MTLK_ERR_UNKNOWN;
  }

  /* Now read the canonicalized absolute pathname */
  outname = realpath(linkname, NULL);

  /* In case of an error, leave the handling up to the caller */
  if (!outname)
    return MTLK_ERR_UNKNOWN;

  /* Copy result into output buffer */
  res = wave_strcopy(buf, outname, size);
  /* Deallocate resulting string */
  free(outname);

  /* Report insufficient buffer size */
  if (!res) {
    return MTLK_ERR_BUF_TOO_SMALL;
  }

  return MTLK_ERR_OK;
}
