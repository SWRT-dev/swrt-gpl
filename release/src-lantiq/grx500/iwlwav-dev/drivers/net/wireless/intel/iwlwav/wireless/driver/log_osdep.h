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

#ifndef CPTCFG_IWLWAV_SILENT
void __MTLK_IFUNC log_osdep_init(void);
void __MTLK_IFUNC log_osdep_cleanup(void);
void __MTLK_IFUNC log_osdep_do(const char *fname, 
                               int         line_no, 
                               const char *level, 
                               const char *fmt, ...);
#else
#define log_osdep_init()
#define log_osdep_cleanup()
#define log_osdep_do(...)
#endif

#define CLOG(fname, line_no, log_level, fmt, ...)     \
  log_osdep_do(fname, line_no, #log_level, fmt, ## __VA_ARGS__)
#define CINFO(fname, line_no, fmt, ...)                         \
  log_osdep_do("", 0, "", fmt, ## __VA_ARGS__)
#define CERROR(fname, line_no, fmt, ...)                        \
  log_osdep_do(fname, line_no, "E", fmt, ## __VA_ARGS__)
#define CWARNING(fname, line_no, fmt, ...)                      \
  log_osdep_do(fname, line_no, "W", fmt, ## __VA_ARGS__)

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

/* Sync Log Timestamp with FW TSF */
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
extern uint32 mtlk_log_get_timestamp(void);
#else
#define mtlk_log_get_timestamp()    (uint32)mtlk_osal_timestamp()
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */

#define IWLWAV_RTLOG_WLAN_IF_INVALID   (~0u)

#include "mtlklist.h"

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

struct _mtlk_log_buf_entry_t
{
  mtlk_ldlist_entry_t entry; // List entry data
  uint32 refcount;           // Reference counter
  uint32 data_size;          // Data size in buffer
  uint32 expired;            // Buffer swap timer expiration flag
} __MTLK_IDATA;

static __INLINE void*
_mtlk_log_buf_get_data_buffer(struct _mtlk_log_buf_entry_t* log_buffer)
{
  return (void*)(log_buffer + 1);
}

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#define LOG_CONSOLE_TEXT_INFO __FUNCTION__

#include "log.h"
#ifdef CONFIG_WAVE_RTLOG_REMOTE
#include "mtlk_rtlog_app_comm.h"
#endif

#endif /* __LOG_OSDEP_H__ */
