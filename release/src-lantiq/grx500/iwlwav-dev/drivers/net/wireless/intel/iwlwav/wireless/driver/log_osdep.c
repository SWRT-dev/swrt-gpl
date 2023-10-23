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

#ifndef CPTCFG_IWLWAV_SILENT

#include "mtlk_snprintf.h"

#define LOG_LOCAL_GID   GID_LOG
#define LOG_LOCAL_FID   1

#define MAX_CLOG_LEN  2048
#define MAX_CLOG_BUFS 3

struct clog_buf 
{
  struct list_head lentry;
  char             buf[MAX_CLOG_LEN];
};

struct clog_aux
{
  struct list_head list;
  spinlock_t       lock;
  struct clog_buf  bufs[MAX_CLOG_BUFS];
};

static struct clog_aux clog_aux;

void __MTLK_IFUNC
log_osdep_init (void)
{
  int i;

  INIT_LIST_HEAD(&clog_aux.list);
  spin_lock_init(&clog_aux.lock);

  for (i = 0; i < MAX_CLOG_BUFS; i++) {
    list_add(&clog_aux.bufs[i].lentry, &clog_aux.list);
  }
}

void __MTLK_IFUNC
log_osdep_cleanup (void)
{

}

inline void
_log_osdep_emergency_vprintk (const char *fname, int line_no, 
                              const char *fmt, va_list args)
{
  printk("[%010u] mtlk **EMERGENCY** (%s:%d): ",
         mtlk_log_get_timestamp(), fname, line_no);
  vprintk(fmt, args);
  printk("\n");
}

inline char *
_log_osdep_get_cbuf (void)
{
  char             *buf = NULL;
  struct list_head *e;

  spin_lock_bh(&clog_aux.lock);
  if (!list_empty(&clog_aux.list)) {
    e = clog_aux.list.next;
    list_del(e);

    buf = list_entry(e, struct clog_buf, lentry)->buf;
  }
  spin_unlock_bh(&clog_aux.lock);
  return buf;
}

inline void
_log_osdep_put_cbuf (char *buf)
{
  struct clog_buf *cb = container_of(buf, struct clog_buf, buf[0]);

  spin_lock_bh(&clog_aux.lock);
  list_add(&cb->lentry, &clog_aux.list);
  spin_unlock_bh(&clog_aux.lock);
}

#ifdef CPTCFG_IWLWAV_TSF_TIMER_TIMESTAMPS_IN_DEBUG_PRINTOUTS
uint32 __MTLK_IFUNC get_hw_time_stamp(void);
#endif /* CPTCFG_IWLWAV_TSF_TIMER_TIMESTAMPS_IN_DEBUG_PRINTOUTS */

void __MTLK_IFUNC
log_osdep_do (const char *fname, 
              int         line_no, 
              const char *level, 
              const char *fmt, 
              ...)
{
  char *cmsg_str = _log_osdep_get_cbuf();
  va_list args;

  va_start(args, fmt);
  if (__LIKELY(cmsg_str != NULL)) {
    int cmsg_ln__ = 
#ifdef CPTCFG_IWLWAV_TSF_TIMER_TIMESTAMPS_IN_DEBUG_PRINTOUTS
      mtlk_snprintf(cmsg_str, MAX_CLOG_LEN,
      "[%010u|%010u] mtlk%s(%s:%d): ",
      mtlk_log_get_timestamp(), get_hw_time_stamp(), level, fname, line_no);
#else
      mtlk_snprintf(cmsg_str, MAX_CLOG_LEN,
      "[%010u] mtlk%s(%s:%d): ",
      mtlk_log_get_timestamp(), level, fname, line_no);
#endif /* CPTCFG_IWLWAV_TSF_TIMER_TIMESTAMPS_IN_DEBUG_PRINTOUTS */
    mtlk_vsnprintf(cmsg_str + cmsg_ln__, MAX_CLOG_LEN - cmsg_ln__,
                  fmt, args);
    printk("%s\n", cmsg_str);
    _log_osdep_put_cbuf(cmsg_str);
  }
  else {
    _log_osdep_emergency_vprintk(fname, line_no, fmt, args);
  } 
  va_end(args);
}
#endif

