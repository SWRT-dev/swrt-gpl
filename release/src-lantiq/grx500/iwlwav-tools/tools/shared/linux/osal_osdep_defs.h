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

#if !defined (SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_DEFS) /* definitions - functions etc. */
#error "You shouldn't include this file directly!"
#endif /* SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_... */

#undef SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_DEFS

#include <time.h>
#include <unistd.h>
#include <errno.h>

/* All osdep files must use the same GID */
#define LOG_LOCAL_GID   GID_OSAL
#define LOG_LOCAL_FID   0

#define mtlk_osal_emergency_print(fmt, ...) fprintf(stderr, "[MTLKA] " fmt "\n", ##__VA_ARGS__)

struct mtlk_osal_obj
{
#ifndef HAVE_BUILTIN_ATOMIC
  mtlk_osal_mutex_t atomic_lock;
#endif
};

extern struct mtlk_osal_obj mtlk_osal_global;

#define __MTLK_OSAL_GRANULARITY_MS 20 

#define MS_PER_S  1000
#define US_PER_MS 1000
#define NS_PER_US 1000
#define NS_PER_MS (US_PER_MS * NS_PER_US)

#ifdef CPTCFG_IWLWAV_DEBUG
#define __MTLK_CALL_ASSERT_RESULT(call, text)           \
  for (;;) {                                            \
    int err__ = (call);                                 \
    if (err__ != 0) {                                   \
    	fprintf(stderr, text " failed! err=%d", err__);   \
      MTLK_ASSERT(0);                                   \
    }                                                   \
    break;                                              \
  }
#else
#define __MTLK_CALL_ASSERT_RESULT(call, text) (call)
#endif

static __INLINE int
__MTLK_MAP_ERR (int evalue)
{
  int res = MTLK_ERR_UNKNOWN;

  switch (evalue) {
  case 0:
    res = MTLK_ERR_OK;
    break;
  case EAGAIN:
  case ENOMEM:
    res = MTLK_ERR_NO_RESOURCES;
    break;
  case EPERM:
    res = MTLK_ERR_PROHIB;
    break;
  case EBUSY:
    res = MTLK_ERR_NOT_READY;
    break;
  case ETIMEDOUT:
    res = MTLK_ERR_TIMEOUT;
    break;
  case EINVAL:
    res = MTLK_ERR_PARAMS;
    break;
  default:
    break;
  }

  return res;
}

int  __MTLK_IFUNC _mtlk_linux_app_osal_init(void);
void __MTLK_IFUNC _mtlk_linux_app_osal_cleanup(void);

static __INLINE int
mtlk_osal_init (void)
{
  return _mtlk_linux_app_osal_init();
}

static __INLINE
void mtlk_osal_cleanup (void)
{
  _mtlk_linux_app_osal_cleanup();
}

static __INLINE void* 
mtlk_osal_mem_alloc (uint32 size, uint32 tag)
{
  MTLK_ASSERT(0 != size);
  return malloc_tag(size, tag);
}

static __INLINE void 
mtlk_osal_mem_free (void* buffer)
{
  MTLK_ASSERT(NULL != buffer);
  free_tag(buffer);
}

static __INLINE int
mtlk_osal_lock_init (mtlk_osal_spinlock_t* spinlock)
{
  return __MTLK_MAP_ERR( pthread_mutex_init(&spinlock->lock, NULL) );
}

static __INLINE void
mtlk_osal_lock_acquire (mtlk_osal_spinlock_t* spinlock)
{
  __MTLK_CALL_ASSERT_RESULT(
                            pthread_mutex_lock(&spinlock->lock),
                            "Mutex lock");
}

static __INLINE void
mtlk_osal_lock_release (mtlk_osal_spinlock_t* spinlock)
{
  __MTLK_CALL_ASSERT_RESULT(pthread_mutex_unlock(&spinlock->lock),
                           "Mutex unlock");
}

static __INLINE void
mtlk_osal_lock_cleanup (mtlk_osal_spinlock_t* spinlock)
{
  __MTLK_CALL_ASSERT_RESULT(pthread_mutex_destroy(&spinlock->lock),
                            "Mutex destroy");
}

static __INLINE int
mtlk_osal_event_init (mtlk_osal_event_t* event)
{
  int res = 0;

  res = pthread_mutex_init(&event->mutex, NULL);
  if (res != 0) {
    goto end;
  }

  res = pthread_cond_init(&event->cond, 0);
  if (res != 0) {
    goto end;
  }

  event->wait_flag = 0;

end:
  return __MTLK_MAP_ERR(res);
}

static __INLINE int
mtlk_osal_event_wait (mtlk_osal_event_t* event, uint32 msec)
{
  int res = 0;
  struct timespec wait_tp;

  clock_gettime(CLOCK_REALTIME, &wait_tp);

  wait_tp.tv_sec  += msec / MS_PER_S;
  wait_tp.tv_nsec += (msec % MS_PER_S) * NS_PER_MS;
  
  __MTLK_CALL_ASSERT_RESULT(
                            pthread_mutex_lock(&event->mutex),
                           "Mutex (ev) lock");

  while (!event->wait_flag) {
    res = pthread_cond_timedwait(&event->cond, 
                                 &event->mutex,
                                 &wait_tp);
    if (res != 0)
      break;
  }

  __MTLK_CALL_ASSERT_RESULT(
                            pthread_mutex_unlock(&event->mutex),
                           "Mutex (ev) unlock");

  return __MTLK_MAP_ERR(res);
}

static __INLINE void 
mtlk_osal_event_set (mtlk_osal_event_t* event)
{
  __MTLK_CALL_ASSERT_RESULT(
                            pthread_mutex_lock(&event->mutex),
                           "Mutex (ev) lock");
  event->wait_flag = 1;
  pthread_cond_signal(&event->cond);
 __MTLK_CALL_ASSERT_RESULT(
                           pthread_mutex_unlock(&event->mutex),
                          "Mutex (ev) unlock");
}

static __INLINE void
mtlk_osal_event_reset (mtlk_osal_event_t* event)
{
  event->wait_flag = 0;
}

static __INLINE void
mtlk_osal_event_cleanup (mtlk_osal_event_t* event)
{
  __MTLK_CALL_ASSERT_RESULT(
                            pthread_cond_destroy(&event->cond),
                           "Cond (ev) destroy");

  __MTLK_CALL_ASSERT_RESULT(
                            pthread_mutex_destroy(&event->mutex),
                           "Mutex (ev) destroy");
}

static __INLINE int
mtlk_osal_mutex_init (mtlk_osal_mutex_t* mutex)
{
  return __MTLK_MAP_ERR( pthread_mutex_init(&mutex->mutex, NULL) );
}

static __INLINE void
mtlk_osal_mutex_acquire (mtlk_osal_mutex_t* mutex)
{
  __MTLK_CALL_ASSERT_RESULT(
                            pthread_mutex_lock(&mutex->mutex),
                            "Mutex lock");
}

static __INLINE void
mtlk_osal_mutex_release (mtlk_osal_mutex_t* mutex)
{
  __MTLK_CALL_ASSERT_RESULT(pthread_mutex_unlock(&mutex->mutex),
                           "Mutex unlock");
}

static __INLINE void
mtlk_osal_mutex_cleanup (mtlk_osal_mutex_t* mutex)
{
  __MTLK_CALL_ASSERT_RESULT(pthread_mutex_destroy(&mutex->mutex),
                            "Mutex destroy");
}

static __INLINE void
mtlk_osal_msleep (uint32 msec)
{
  struct timespec ts[2];
  int             req_idx = 0;
  
  ts[req_idx].tv_sec  = msec / MS_PER_S; 
  ts[req_idx].tv_nsec = (msec % MS_PER_S) * NS_PER_MS;

  while (nanosleep(&ts[req_idx], &ts[1 - req_idx]) == -1) {
    req_idx = 1 - req_idx;
  }
}

/*
  pause - suspend the thread until a signal is received

  The pause() function shall suspend the calling thread
  until delivery of a signal whose action is either to execute a
  signal-catching function or to terminate the process.
  If the action is to terminate the process, pause() shall not return.
  If the action is to execute a signal-catching function,
  pause() shall return after the signal-catching function returns.
*/
static __INLINE int
mtlk_osal_pause (void)
{
  return pause();
}

static __INLINE mtlk_osal_timestamp_t
mtlk_osal_timestamp (void)
{
  struct timespec ts;

  clock_gettime(CLOCK_REALTIME, &ts);

  return ((uint64)ts.tv_sec) * MS_PER_S + ts.tv_nsec / NS_PER_MS;
}

static __INLINE uint32
mtlk_osal_timestamp_to_ms (mtlk_osal_timestamp_t timestamp)
{
  return (uint32)timestamp;
}

static __INLINE mtlk_osal_timestamp_t
mtlk_osal_ms_to_timestamp (uint32 msecs)
{
  return (uint64)msecs;
}

static __INLINE BOOL
mtlk_osal_time_after (mtlk_osal_timestamp_t tm1, mtlk_osal_timestamp_t tm2)
{
  return ((int64)tm2 - (int64)tm1 < 0);
}

#include <linux/if_ether.h>

static __INLINE
void mtlk_osal_zero_ip4_address (void* addr)
{
  uint16 *a = (uint16 *) addr;
  a[0] = a[1] = 0;
}

static __INLINE void
mtlk_osal_copy_ip4_addresses (void* dst, const void* src)
{
  uint16 *a = (uint16 *) dst;
  const uint16 *b = (const uint16 *) src;
  a[0] = b[0]; a[1] = b[1];
}

static __INLINE void
mtlk_osal_zero_eth_address(uint8 *addr)
{
  memset(addr, 0x00, ETH_ALEN);
}

static __INLINE void 
mtlk_osal_copy_eth_addresses (uint8* dst, 
                              const uint8* src)
{
  dst[0] = src[0]; dst[1] = src[1]; dst[2] = src[2];
  dst[3] = src[3]; dst[4] = src[4]; dst[5] = src[5];
}

static __INLINE unsigned
mtlk_osal_compare_eth_addresses (const uint8* addr1, 
                                 const uint8* addr2)
{
  const uint16 *a = (const uint16 *) addr1;
  const uint16 *b = (const uint16 *) addr2;

  return ((a[0] ^ b[0]) | (a[1] ^ b[1]) | (a[2] ^ b[2])) != 0;
}

static __INLINE BOOL
mtlk_osal_is_zero_address (const uint8* addr)
{
  return !(addr[0] | addr[1] | addr[2] | addr[3] | addr[4] | addr[5]);
}

static __INLINE BOOL 
mtlk_osal_eth_is_multicast (const uint8* addr)
{
  return (0x01 & addr[0]);
}

static __INLINE BOOL
mtlk_osal_eth_is_broadcast (const uint8* addr)
{
  return (addr[0] & addr[1] & addr[2] & addr[3] & addr[4] & addr[5]) == 0xff;
}

static __INLINE BOOL
mtlk_osal_eth_is_group_addr (const uint8* addr)
{
  return (((uint8*)(addr))[0] & 0x01);
}

static __INLINE BOOL
mtlk_osal_eth_is_directed (const uint8* addr)
{
  return !mtlk_osal_eth_is_group_addr(addr);
}

/* atomic counters */

static __INLINE uint32
mtlk_osal_atomic_add (mtlk_atomic_t* val, uint32 i)
{
#ifndef HAVE_BUILTIN_ATOMIC
  uint32 res;

  mtlk_osal_mutex_acquire(&mtlk_osal_global.atomic_lock);
  val->counter += i;
  res           = val->counter;
  mtlk_osal_mutex_release(&mtlk_osal_global.atomic_lock);

  return res;
#else
  return __sync_add_and_fetch(&val->counter, i);
#endif
}

static __INLINE uint32
mtlk_osal_atomic_sub (mtlk_atomic_t* val, uint32 i)
{
#ifndef HAVE_BUILTIN_ATOMIC
  uint32 res;

  mtlk_osal_mutex_acquire(&mtlk_osal_global.atomic_lock);
  val->counter -= i;
  res           = val->counter;
  mtlk_osal_mutex_release(&mtlk_osal_global.atomic_lock);

  return res;
#else
  return __sync_sub_and_fetch(&val->counter, i);
#endif
}

static __INLINE uint32
mtlk_osal_atomic_inc (mtlk_atomic_t* val)
{
  return mtlk_osal_atomic_add(val, 1);
}

static __INLINE uint32
mtlk_osal_atomic_dec (mtlk_atomic_t* val)
{
  return mtlk_osal_atomic_sub(val, 1);
}

static __INLINE void
mtlk_osal_atomic_set (mtlk_atomic_t* target, uint32 value)
{
  mtlk_osal_atomic_xchg(target, value);
}

static __INLINE uint32
mtlk_osal_atomic_get (const mtlk_atomic_t* val)
{
  return val->counter;
}

static __INLINE uint32
mtlk_osal_atomic_xchg (mtlk_atomic_t* target, uint32 value)
{
#ifndef HAVE_BUILTIN_ATOMIC
  uint32 res;

  mtlk_osal_mutex_acquire(&mtlk_osal_global.atomic_lock);
  res             = target->counter;
  target->counter = value;
  mtlk_osal_mutex_release(&mtlk_osal_global.atomic_lock);

  return res;
#else
  return __sync_lock_test_and_set(&target->counter, value);
#endif
}

int  _mtlk_osal_thread_init(mtlk_osal_thread_t *thread);
int  _mtlk_osal_thread_run(mtlk_osal_thread_t     *thread,
                           mtlk_osal_thread_proc_f proc,
                           mtlk_handle_t           context);
int  _mtlk_osal_thread_wait(mtlk_osal_thread_t *thread,
                            int32              *thread_res);
void _mtlk_osal_thread_cleanup(mtlk_osal_thread_t *thread);

static __INLINE int
mtlk_osal_thread_init (mtlk_osal_thread_t *thread)
{
  return _mtlk_osal_thread_init(thread);
}

static __INLINE int
mtlk_osal_thread_run (mtlk_osal_thread_t     *thread,
                      mtlk_osal_thread_proc_f proc,
                      mtlk_handle_t           context)
{
  return _mtlk_osal_thread_run(thread, proc, context);
}

static __INLINE int
mtlk_osal_thread_wait (mtlk_osal_thread_t *thread, 
                       int32              *thread_res)
{
  return _mtlk_osal_thread_wait(thread, thread_res);
}

static __INLINE void
mtlk_osal_thread_cleanup (mtlk_osal_thread_t *thread)
{
 _mtlk_osal_thread_cleanup(thread);
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID
