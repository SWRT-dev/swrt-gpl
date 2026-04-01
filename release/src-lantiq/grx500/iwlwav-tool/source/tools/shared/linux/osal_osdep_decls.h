/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/**********************************************************************
 * Driver OS Abstraction Layer implementation
 * 
 * This file:
 * [*] defines system-dependent implementation of OSAL data types 
 *     and interfaces.
 * [*] is included in mtlk_osal.h only. No other files must include it!
 *
 * NOTE (MUST READ!!!): 
 *  OSAL_... macros (if defined) are designed for OSAL internal 
 *  usage only (see mtlk_osal.h for more information). They can not 
 *  and must not be used anywhere else.
***********************************************************************/

/* NLD MTLK OSAL */
#if !defined(SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_DECLS) /* forward declarations - types etc. */
#error "You shouldn't include this file directly!"
#endif /* SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_... */

#undef SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_DECLS

#include <stdlib.h>
#include <pthread.h>
#include <stdint.h>
#include <time.h>

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  pthread_mutex_t lock;
} mtlk_osal_spinlock_t;

typedef struct 
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  pthread_mutex_t mutex;
  pthread_cond_t  cond;
  int             wait_flag;
} mtlk_osal_event_t;

#define MTLK_OSAL_EVENT_INFINITE ((uint32)-1)

typedef struct 
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  pthread_mutex_t mutex;
} mtlk_osal_mutex_t;

//Type for timestamps
typedef uint64 mtlk_osal_timestamp_t;

typedef struct
{
  volatile uint32 counter;
} mtlk_atomic_t;

typedef struct
{ 
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);
  
  pthread_t     os_thread;
  int32         (__MTLK_IFUNC * proc)(mtlk_handle_t context);
  mtlk_handle_t context;
  int32         result;
} mtlk_osal_thread_t;


