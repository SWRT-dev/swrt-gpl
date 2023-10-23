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
#include "mtlksighandler.h"
#include "mtlkstartup.h"
#include "mtlk_osal.h"

#include <pthread.h>
#include <signal.h>

#define LOG_LOCAL_GID   GID_MTLKSHANDLER
#define LOG_LOCAL_FID   0

typedef struct
{
  mtlk_shandler_clb_f clb;
  mtlk_handle_t       usr_ctx;
  sigset_t            signal_mask;
  pthread_t           sig_thread;
  MTLK_DECLARE_INIT_STATUS;
} mtlk_shandler_t;

static mtlk_shandler_t gshandler = {0};

MTLK_INIT_STEPS_LIST_BEGIN(shandler)
  MTLK_INIT_STEPS_LIST_ENTRY(shandler, INIT_SIGMASK)
  MTLK_INIT_STEPS_LIST_ENTRY(shandler, SET_SIGMASK)
  MTLK_INIT_STEPS_LIST_ENTRY(shandler, SHANDLER_THREAD)
MTLK_INIT_INNER_STEPS_BEGIN(shandler)
MTLK_INIT_STEPS_LIST_END(shandler);

static void
_mtlk_shandler_init_mask (mtlk_shandler_t *h)
{
  MTLK_ASSERT(h != NULL);

  sigemptyset(&h->signal_mask);
  sigaddset(&h->signal_mask, SIGINT);
  sigaddset(&h->signal_mask, SIGTERM);
}

static void *
_mtlk_shandler_signal_thread (void *arg)
{
  mtlk_shandler_t *h = (mtlk_shandler_t *)arg;
  int              signo;

  while (1) {
    int os_res = sigwait(&h->signal_mask, &signo);
    if (os_res == 0) {
      break;
    }
    mtlk_osal_emergency_print("sigwait returned %d", os_res);
  }
  
  switch (signo)
  {
  case SIGINT:
  case SIGTERM:
    h->clb(h->usr_ctx, signo);
    break;
  default:
    /* should normally not happen */
    mtlk_osal_emergency_print("Unexpected signal %d", signo);
    break;
  }

  return NULL;
}

int __MTLK_IFUNC
mtlk_shandler_init (mtlk_shandler_clb_f clb, mtlk_handle_t usr_ctx)
{
  int              os_res = -EINVAL;
  mtlk_shandler_t *h      = &gshandler;

  MTLK_ASSERT(clb != NULL);

  h->clb     = clb;
  h->usr_ctx = usr_ctx;

  MTLK_INIT_TRY(shandler, MTLK_OBJ_PTR(h))
    MTLK_INIT_STEP_VOID(shandler, INIT_SIGMASK, MTLK_OBJ_PTR(h),
                        _mtlk_shandler_init_mask, (h));
    MTLK_INIT_STEP_EX(shandler, SET_SIGMASK, MTLK_OBJ_PTR(h),
                      pthread_sigmask, (SIG_BLOCK, &h->signal_mask, NULL),
                      os_res, os_res == 0, MTLK_ERR_SYSTEM);
    MTLK_INIT_STEP_EX(shandler, SHANDLER_THREAD, MTLK_OBJ_PTR(h),
                      pthread_create, (&h->sig_thread, NULL, _mtlk_shandler_signal_thread, h),
                      os_res, os_res == 0, MTLK_ERR_SYSTEM);
  MTLK_INIT_FINALLY(shandler, MTLK_OBJ_PTR(h))
  MTLK_INIT_RETURN(shandler, MTLK_OBJ_PTR(h), mtlk_shandler_cleanup, ())
}

void __MTLK_IFUNC
mtlk_shandler_cleanup (void)
{
  mtlk_shandler_t *h = &gshandler;

  MTLK_CLEANUP_BEGIN(shandler, MTLK_OBJ_PTR(h))
    MTLK_CLEANUP_STEP(shandler, SHANDLER_THREAD, MTLK_OBJ_PTR(h),
                      pthread_kill, (h->sig_thread, 0));
    MTLK_CLEANUP_STEP(shandler, SET_SIGMASK, MTLK_OBJ_PTR(h),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(shandler, INIT_SIGMASK, MTLK_OBJ_PTR(h),
                      MTLK_NOACTION, ());
  MTLK_CLEANUP_END(shandler, MTLK_OBJ_PTR(h))
}
