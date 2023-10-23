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
 * Written by: Andriy Fidrya
 *
 */
#ifndef __LOG_OSDEP_H__
#define __LOG_OSDEP_H__

#define LOG_CONSOLE_TEXT_INFO __FUNCTION__

void __MTLK_IFUNC log_osdep_init(void);
void __MTLK_IFUNC log_osdep_cleanup(void);
void __MTLK_IFUNC log_osdep_do(const char *fname, 
                               int         line_no, 
                               const char *level, 
                               const char *fmt, ...);

#define CLOG(fname, line_no, log_level, fmt, ...)                               \
  mtlk_osdep_log(MTLK_OSLOG_INFO, fname, line_no, #log_level, (fmt), ## __VA_ARGS__)
#define CINFO(fname, line_no, fmt, ...)                                         \
  mtlk_osdep_log(MTLK_OSLOG_INFO, "", 0, "", (fmt), ## __VA_ARGS__)
#define CERROR(fname, line_no, fmt, ...)                                        \
  mtlk_osdep_log(MTLK_OSLOG_ERR, fname, line_no, "E", (fmt), ## __VA_ARGS__)
#define CWARNING(fname, line_no, fmt, ...)                                      \
  mtlk_osdep_log(MTLK_OSLOG_WARN, fname, line_no, "W", (fmt), ## __VA_ARGS__)

#ifndef IWLWAV_RTLOG_FLAGS
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#define IWLWAV_RTLOG_FLAGS (RTLF_REMOTE_ENABLED | RTLF_CONSOLE_ENABLED)
#else
#define IWLWAV_RTLOG_FLAGS (RTLF_CONSOLE_ENABLED)
#endif
#endif

#ifndef IWLWAV_RTLOG_MAX_DLEVEL
#if defined(CPTCFG_IWLWAV_SILENT)
#define IWLWAV_RTLOG_MAX_DLEVEL IWLWAV_RTLOG_SILENT_DLEVEL
#elif defined(CPTCFG_IWLWAV_MAX_DLEVEL)
#define IWLWAV_RTLOG_MAX_DLEVEL CPTCFG_IWLWAV_MAX_DLEVEL
#else
#error Wrong configuration settings for Log level!
#endif
#endif

#if defined (CPTCFG_IWLWAV_RT_LOGGER_INLINES)
/* Console & Remote - use inlines */
#define __MTLK_FLOG static __INLINE
#elif defined(CPTCFG_IWLWAV_RT_LOGGER_FUNCTIONS)
/* Console & Remote - use function calls */
#define __MTLK_FLOG
#else
#error Wrong RTLOGGER configuration!
#endif

#ifndef IWLWAV_RTLOG_DLEVEL_VAR
#define IWLWAV_RTLOG_DLEVEL_VAR debug
#endif
extern int IWLWAV_RTLOG_DLEVEL_VAR;

#define IWLWAV_RTLOG_DLEVEL_VAR_REMOTE debug_remote
extern int IWLWAV_RTLOG_DLEVEL_VAR_REMOTE;

#define mtlk_log_get_timestamp()    (uint32)mtlk_osal_timestamp()

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

struct _mtlk_log_buf_entry_t
{
  uint8  *data; /* pointer to data buffer */
  uint32 size;  /* data size */
} __MTLK_IDATA;

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#include "log.h"
#include "mtlk_rtlog_app_comm.h"

#endif /* __LOG_OSDEP_H__ */
