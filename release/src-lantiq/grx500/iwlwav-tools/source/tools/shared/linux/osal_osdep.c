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
#include "mtlk_osal.h"

/* All osdep files must use the same GID */
#define LOG_LOCAL_GID   GID_OSAL
#define LOG_LOCAL_FID   1

MTLK_DECLARE_OBJPOOL(g_objpool);

struct mtlk_osal_obj mtlk_osal_global;

int  __MTLK_IFUNC
_mtlk_linux_app_osal_init (void)
{
  int res = MTLK_ERR_UNKNOWN;

  res = mtlk_objpool_init(&g_objpool);
  if (res != MTLK_ERR_OK) {
    ELOG_D("OBJPOOL init error#%d", res);
    goto objpool_init_failed;
  }
  
#ifndef HAVE_BUILTIN_ATOMIC
  res = mtlk_osal_mutex_init(&mtlk_osal_global.atomic_lock);
  if (res != MTLK_ERR_OK) {
    ELOG_D("OSAL Atomic Lock init error#%d", res);
    goto mutex_init_failed;
  }
#endif

  return MTLK_ERR_OK;

#ifndef HAVE_BUILTIN_ATOMIC
mutex_init_failed:
  mtlk_objpool_cleanup(&g_objpool);
#endif
objpool_init_failed:

  return res;
}

void __MTLK_IFUNC
_mtlk_linux_app_osal_cleanup (void)
{
#ifndef HAVE_BUILTIN_ATOMIC
  mtlk_osal_mutex_cleanup(&mtlk_osal_global.atomic_lock);
#endif
  mtlk_objpool_cleanup(&g_objpool);
}

static void *
_mtlk_osal_thread_proc (void* param)
{
  mtlk_osal_thread_t *thread = (mtlk_osal_thread_t *)param;

  MTLK_ASSERT(thread != NULL);
  MTLK_ASSERT(thread->proc != NULL);

  thread->result = thread->proc(thread->context);

  pthread_exit(&thread->result);
}

int  
_mtlk_osal_thread_init (mtlk_osal_thread_t* thread)
{
  memset(thread, 0, sizeof(*thread));
  return MTLK_ERR_OK;
}

int
_mtlk_osal_thread_run (mtlk_osal_thread_t     *thread,
                       mtlk_osal_thread_proc_f proc,
                       mtlk_handle_t           context)
{
  int            res;
  pthread_attr_t attr;

  thread->proc    = proc;
  thread->context = context;

  /* Initialize attribute */
  pthread_attr_init(&attr);
  pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE);

  /* Run thread */
  res = pthread_create(&thread->os_thread, 
                       &attr,
                       _mtlk_osal_thread_proc,
                       thread);

  /* Free resources */
  pthread_attr_destroy(&attr);

  if (0 != res) {
    ELOG_SD("Failed to create thread: %s (%d)",
      strerror(res), res);
    memset(thread, 0, sizeof(*thread));
    res = MTLK_ERR_UNKNOWN;
  }
  else {
    res = MTLK_ERR_OK;
  }

  return res;
}

int
_mtlk_osal_thread_wait (mtlk_osal_thread_t* thread,
                        int32              *thread_res)
{
  int   res  = MTLK_ERR_OK;
  void *tres = NULL;

  /* Wait for the notification thread to process the signal */
  res = pthread_join(thread->os_thread, &tres);

  if(0 != res) {
    ELOG_SD("Failed to terminate thread: %s (%d)",
      strerror(res), res);
    res = MTLK_ERR_UNKNOWN;
  }
  else if (thread_res) {
    *thread_res = *((int32 *)tres);
  }

  return res;
}

void
_mtlk_osal_thread_cleanup (mtlk_osal_thread_t* thread)
{
  memset(thread, 0, sizeof(*thread));
}

/* internal implementation of standard function strlcpy */
size_t
mtlk_osal_strlcpy (char *dest, const char *src, size_t size)
{
  const char *sptr = src;
  size_t count = size;

  /* Copy source string */
  if (count) {
    while (--count) {
      if ((*dest++ = *sptr++) == '\0')
        break;
    }
  }

  if (!count) {
    if (size) *dest = '\0'; /* force NULL termination */
    while (*sptr++) ;       /* to calculate source string size */
  }

  return (sptr - src - 1);
}

/* internal implementation of standard function strnlen */
size_t
mtlk_osal_strnlen (const char *src, const size_t maxlen) {
  size_t count;
  for (count = 0; count < maxlen; count++) {
    if (src[count] == '\0') {
      break;
    }
  }
  return count;
}
