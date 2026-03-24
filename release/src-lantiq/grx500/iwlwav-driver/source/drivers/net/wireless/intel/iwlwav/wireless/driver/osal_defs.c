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

#define LOG_LOCAL_GID   GID_OSAL
#define LOG_LOCAL_FID   3

static void
__mtlk_osal_timer_clb (struct timer_list *t)
{
  mtlk_osal_timer_t *timer = from_timer(timer, t, os_timer);
  uint32             msec  = 0;

  mtlk_osal_lock_acquire(&timer->sync_lock);

  if (__LIKELY(!timer->stop)) {
    ASSERT(timer->clb != NULL);

    msec = timer->clb(timer, timer->clb_usr_data);
  }

  if (!msec) {
    /* Last hop or timer should stop =>
     * don't re-arm it and release the RM Lock .
     */
    timer->stop = TRUE; /* Mark timer stopped */
    mtlk_rmlock_release(&timer->rmlock);
  }
  else {
    /* Another hop required => 
     * re-arm the timer, RM Lock still acquired. 
     */
    mod_timer(&timer->os_timer, jiffies + msecs_to_jiffies(msec));
  }
  mtlk_osal_lock_release(&timer->sync_lock);
}

BOOL __MTLK_IFUNC
_mtlk_osal_timer_is_stopped (mtlk_osal_timer_t *timer)
{
  return timer->stop;
}

BOOL __MTLK_IFUNC
_mtlk_osal_timer_is_cancelled (mtlk_osal_timer_t *timer)
{
  return timer->cancelled;
}

int __MTLK_IFUNC
_mtlk_osal_timer_init (mtlk_osal_timer_t *timer,
                       mtlk_osal_timer_f  clb,
                       mtlk_handle_t      clb_usr_data)
{
  ASSERT(clb != NULL);

  memset(timer, 0, sizeof(*timer));
  mtlk_rmlock_init(&timer->rmlock);
  mtlk_rmlock_acquire(&timer->rmlock); /* Acquire RM Lock initially */
  mtlk_osal_lock_init(&timer->sync_lock);
  timer_setup(&timer->os_timer, __mtlk_osal_timer_clb, 0);

  timer->clb               = clb;
  timer->clb_usr_data      = clb_usr_data;
  timer->stop              = TRUE;
  timer->cancelled         = TRUE;

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_osal_timer_set (mtlk_osal_timer_t *timer,
                      uint32             msec)
{
  _mtlk_osal_timer_cancel_sync(timer);

  timer->os_timer.expires  = jiffies + msecs_to_jiffies(msec);

  timer->stop = FALSE;                 /* Mark timer active         */
  timer->cancelled = FALSE;
  mtlk_rmlock_acquire(&timer->rmlock); /* Acquire RM Lock on arming */
  add_timer(&timer->os_timer);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_osal_timer_cancel (mtlk_osal_timer_t *timer)
{
  timer->stop = TRUE; /* Mark timer stopped */
  timer->cancelled = TRUE;
  if (del_timer(&timer->os_timer)) {
    /* del_timer_sync (and del_timer) returns whether it has 
     * deactivated a pending timer or not. (ie. del_timer() 
     * of an inactive timer returns 0, del_timer() of an
     * active timer returns 1.)
     * So, we're releasing RMLock for active (set) timers only.
     */
    mtlk_rmlock_release(&timer->rmlock);
  }
  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_osal_timer_cancel_sync (mtlk_osal_timer_t *timer)
{
  mtlk_osal_lock_acquire(&timer->sync_lock);
  timer->stop = TRUE; /* Mark timer stopped */
  timer->cancelled = TRUE;
  mtlk_osal_lock_release(&timer->sync_lock);
  if (del_timer_sync(&timer->os_timer)) {
    /* del_timer_sync (and del_timer) returns whether it has 
     * deactivated a pending timer or not. (ie. del_timer() 
     * of an inactive timer returns 0, del_timer() of an
     * active timer returns 1.)
     * So, we're releasing RMLock for active (set) timers only.
     */
    mtlk_rmlock_release(&timer->rmlock);
  }
  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
_mtlk_osal_timer_cleanup (mtlk_osal_timer_t *timer)
{
  _mtlk_osal_timer_cancel_sync(timer); /* Cancel Timer           */
  mtlk_osal_lock_cleanup(&timer->sync_lock);
  mtlk_rmlock_release(&timer->rmlock); /* Release last reference */
  mtlk_rmlock_wait(&timer->rmlock);    /* Wait for completion    */
  mtlk_rmlock_cleanup(&timer->rmlock); /* Cleanup the RM Lock    */
}

static void __mtlk_osal_hrtimer_tasklet(mtlk_handle_t data)
{
  mtlk_osal_hrtimer_t  *timer = (mtlk_osal_hrtimer_t *)data;

  ASSERT(timer != NULL);
  ASSERT(timer->clb != NULL);

  mtlk_osal_lock_acquire(&timer->sync_lock);

  timer->clb(timer, timer->clb_usr_data);
  timer->stop = TRUE; /* Mark timer stopped */

  mtlk_rmlock_release(&timer->rmlock);
  mtlk_osal_lock_release(&timer->sync_lock);
}

static enum hrtimer_restart
__mtlk_osal_hrtimer_hw_interrupt_clb (struct hrtimer *hrtimer)
{
  mtlk_osal_hrtimer_t  *timer = MTLK_CONTAINER_OF(hrtimer, mtlk_osal_hrtimer_t, hr_timer);

  tasklet_schedule(&timer->hrtimer_tasklet);

  return HRTIMER_NORESTART;
}

BOOL __MTLK_IFUNC
_mtlk_osal_hrtimer_is_stopped (mtlk_osal_hrtimer_t *timer)
{
  return timer->stop;
}

BOOL __MTLK_IFUNC
_mtlk_osal_hrtimer_is_cancelled (mtlk_osal_hrtimer_t *timer)
{
  return timer->cancelled;
}

int __MTLK_IFUNC
_mtlk_osal_hrtimer_init (mtlk_osal_hrtimer_t    *timer,
                         mtlk_osal_hrtimer_f     clb,
                         mtlk_handle_t           clb_usr_data,
                         mtlk_hrtimer_mode_e     mode)
{
  ASSERT(clb != NULL);

  memset(timer, 0, sizeof(*timer));
  mtlk_rmlock_init(&timer->rmlock);
  mtlk_rmlock_acquire(&timer->rmlock); /* Acquire RM Lock initially */
  mtlk_osal_lock_init(&timer->sync_lock);
  hrtimer_init(&timer->hr_timer, CLOCK_MONOTONIC, mode);
  timer->hr_timer.function = __mtlk_osal_hrtimer_hw_interrupt_clb;

  timer->clb               = clb;
  timer->clb_usr_data      = clb_usr_data;
  timer->stop              = TRUE;
  timer->cancelled         = TRUE;
  tasklet_init(&timer->hrtimer_tasklet, __mtlk_osal_hrtimer_tasklet, (mtlk_handle_t) timer);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_osal_hrtimer_set (mtlk_osal_hrtimer_t *timer,
                        int64                nsec,
                        mtlk_hrtimer_mode_e  mode)
{

  _mtlk_osal_hrtimer_cancel_sync(timer);

  timer->stop = FALSE;                 /* Mark timer active         */
  timer->cancelled = FALSE;
  mtlk_rmlock_acquire(&timer->rmlock); /* Acquire RM Lock on arming */
  hrtimer_start(&timer->hr_timer, nsec, mode);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_osal_hrtimer_cancel (mtlk_osal_hrtimer_t *timer)
{
  timer->stop = TRUE; /* Mark timer stopped */
  timer->cancelled = TRUE;
  if (hrtimer_try_to_cancel(&timer->hr_timer)) {
    /* hrtimer_try_to_cancel returns whether it has
     * deactivated a pending timer or not. (ie. del_timer()
     * of an inactive timer returns 0, del_timer() of an
     * active timer returns 1.)
     * So, we're releasing RMLock for active (set) timers only.
     */
    mtlk_rmlock_release(&timer->rmlock);
  }
  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_osal_hrtimer_cancel_sync (mtlk_osal_hrtimer_t *timer)
{
  mtlk_osal_lock_acquire(&timer->sync_lock);
  timer->stop = TRUE; /* Mark timer stopped */
  timer->cancelled = TRUE;
  mtlk_osal_lock_release(&timer->sync_lock);
  if (hrtimer_cancel(&timer->hr_timer)) {
    /* hrtimer_cancel (equivalent to del_timer_sync) returns
     * 0 when the timer was not active 1 when the timer was active
     * So, we're releasing RMLock for active (set) timers only.
     */
    mtlk_rmlock_release(&timer->rmlock);
  }
  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
_mtlk_osal_hrtimer_cleanup (mtlk_osal_hrtimer_t *timer)
{
  _mtlk_osal_hrtimer_cancel_sync(timer); /* Cancel Timer           */
  mtlk_rmlock_release(&timer->rmlock); /* Release last reference */
  mtlk_rmlock_wait(&timer->rmlock);    /* Wait for completion    */
  tasklet_kill(&timer->hrtimer_tasklet);
  mtlk_osal_lock_cleanup(&timer->sync_lock);
  mtlk_rmlock_cleanup(&timer->rmlock); /* Cleanup the RM Lock    */
}

/**************************************************************************/
/*                                                                        */
/*                     *** NON_INLINE RATIONALE ***                       */
/*                                                                        */
/* There is a bug in GCC compiler. kmalloc sometimes produces link-time   */
/* error referencing the undefined symbol __you_cannot_kmalloc_that_much. */
/* This is a known problem. For example, see:                             */
/*   http://gcc.gnu.org/ml/gcc-patches/1998-09/msg00632.html              */
/*   http://lkml.indiana.edu/hypermail/linux/kernel/0901.3/01060.html     */
/* The root cause is in __builtin_constant_p which is GCC built-in that   */
/* provides information whether specified value is compile-time constant. */
/* Sometimes it may claim non-constant values as constants and break      */
/* kmalloc logic.                                                         */
/* Robust work around for this problem is non-inline wrapper for kmalloc. */
/* For this reason mtlk_osal_mem* functions made NON-INLINE.              */
/*                                                                        */
/* Tracked as WLS-2011                                                    */
/*                                                                        */
/**************************************************************************/

/* These macros let us overwrite memory allocation functions during klocwork
 * analysis in order to improve its memory tracking. */
#define __MTLK_KMALLOC(size, flags) kmalloc(size, flags)
#define __MTLK_KFREE(size) kfree(size)

void *
__mtlk_kmalloc (size_t size, int flags)
{
  MTLK_ASSERT(0 != size);
  return __MTLK_KMALLOC(size, flags);
}

void
__mtlk_kfree (void *p)
{
  MTLK_ASSERT(NULL != p);
  __MTLK_KFREE(p);
}

#undef __MTLK_KMALLOC
#undef __MTLK_KFREE

#ifndef ATOMIC64_INIT
  mtlk_osal_spinlock_t  mtlk_osal_atomic64_lock;
#endif

int __MTLK_IFUNC
mtlk_osal_init (void)
{
  int res = mtlk_objpool_init(&g_objpool);
#ifndef ATOMIC64_INIT
  if (MTLK_ERR_OK == res) {
    res = mtlk_osal_lock_init(&mtlk_osal_atomic64_lock);
  }
#endif
  return res;
}

void __MTLK_IFUNC
mtlk_osal_cleanup (void)
{
#ifndef ATOMIC64_INIT
  mtlk_osal_lock_cleanup(&mtlk_osal_atomic64_lock);
#endif
  mtlk_objpool_cleanup(&g_objpool);
}

#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAVE600_NOSNOOP
/* Synchronize cache. Used when passing a memory buffer from SoC to PCIe device operating
 * in the no snooping mode. This approach is tested on Puma7 SoC, which is based on Atom
 * Z8000 series. This CPU has 64 byte cache lines (at all the cache levels) and doesn't
 * have predictive read ahead logic for data.
 * Don't pollute CPU's cache for this memory region until the device finished writting
 * to DDRAM. */
void __MTLK_IFUNC
mtlk_osal_sync_cache (void *buffer, uint32 size, mtlk_data_direction_e  direction)
{
  /* Write back and invalidate cache. The operation is performed on a 64-byte
   * (cache line size) granularity */
  clflush_cache_range(buffer, size);
  if (direction != MTLK_DATA_FROM_DEVICE) {
    void* last = (char*)buffer + ((size - 1) & ~(sizeof(uint32) - 1));
    readl(last); /* Read the last value in order to ensure that the data have reached DDRAM */
    if (direction == MTLK_DATA_BIDIRECTIONAL)
      clflush_cache_range(last, sizeof(uint32)); /* flush cache line once again */
  }
}
#endif


/* Give ownership of the memory area to PCIe device */
uint32 __MTLK_IFUNC
mtlk_osal_map_to_phys_addr (void                  *dev_ctx,
                            void                  *buffer,
                            uint32                 size,
                            mtlk_data_direction_e  direction)
{
  dma_addr_t dma_addr;
#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAVE600_NOSNOOP
  /* Under x86, when using no-snoop transactions on PCIe, the responsibility
   * to maintain cache coherence is on us. */
  mtlk_osal_sync_cache(buffer, size, direction);
#endif
  dma_addr = dma_map_single(dev_ctx, buffer, size, direction);
  if (dma_mapping_error(dev_ctx, dma_addr)) {
    /* DMA mapping error. We have to unmap this buffer and return NULL */
    dma_unmap_single(dev_ctx, dma_addr, size, direction);
    return 0x0;
  }
  if (sizeof(dma_addr_t) > sizeof(uint32) && 0 != (dma_addr >> 32)) {
#ifdef CONFIG_WAVE_DEBUG
    ELOG_PP("dma_addr not in 32-bit range! virt:%p: dma_addr:%p", buffer, (void*)dma_addr);
#else
    ELOG_V("dma_addr not in 32-bit range!");
#endif
    /* DMA mapping error: on 64-bit system we got 64-bit address.
       We have to unmap this buffer and return NULL */
    dma_unmap_single(dev_ctx, dma_addr, size, direction);
    return 0x0;
  }
  return dma_addr;
}

static __INLINE int
__mtlk_osal_event_wait_uninterruptible (mtlk_osal_event_t* event, unsigned long wait_time)
{
  int res = wait_time ? wait_event_timeout(event->wait_queue,
                            event->wait_flag,
                            wait_time):event->wait_flag;

  mtlk_osal_lock_acquire(&event->wait_lock);
  if (event->wait_flag) {
    if (event->terminate_flag)
      res = MTLK_ERR_TIMEOUT;
    else
      res = MTLK_ERR_OK;
  }
  else if (res == 0)
    res = MTLK_ERR_TIMEOUT;
  else {
    /* make sure we cover all cases */
    printk(KERN_ALERT "wait_event_timeout returned %d", res);
    MTLK_ASSERT(FALSE);
  }
  mtlk_osal_lock_release(&event->wait_lock);
  return res;
}

int __MTLK_IFUNC
_mtlk_osal_event_wait (mtlk_osal_event_t* event, uint32 msec)
{
  unsigned long wait_time = msecs_to_jiffies(msec);
  unsigned long start_time = jiffies;
  unsigned long time_passed = 0;
  int wait_flag, terminate_flag;
  int res;

  if (signal_pending(current)) {
    /* In case of signals we fallback to uninterruptible wait */
    mtlk_osal_emergency_print("mtlk_osal_event_wait: signal is pending, using wait_uninterruptible");
    return __mtlk_osal_event_wait_uninterruptible(event, wait_time);
  }
  res = msec ? wait_event_interruptible_timeout(event->wait_queue,
                                                event->wait_flag,
                                                wait_time):event->wait_flag;
  /* Read flags */
  mtlk_osal_lock_acquire(&event->wait_lock);
  wait_flag = event->wait_flag;
  terminate_flag = event->terminate_flag;
  mtlk_osal_lock_release(&event->wait_lock);
  if (wait_flag) {
    /* Event has come */
    if (terminate_flag)
      return MTLK_ERR_TIMEOUT;
    else
      return MTLK_ERR_OK;
  }
  else if (res == 0) {
    /* Timeout has occured */
    return MTLK_ERR_TIMEOUT;
  }
  else if (res > 0) {
      /* Event in reset state */
    return MTLK_ERR_OK;
  }
  else {
    /* Interruptable wait may return control earlier because of system signal.
      * Fallback to uninterruptible wait */
    mtlk_osal_emergency_print("mtlk_osal_event_wait: wait_event_interruptible_timeout() returned %d. Using uninterruptible", res);
    if (msec != MTLK_OSAL_EVENT_INFINITE) {
      time_passed = jiffies - start_time;
      if (time_passed >= wait_time) {
        return MTLK_ERR_TIMEOUT;
      }
      wait_time -= time_passed;
    }
    return __mtlk_osal_event_wait_uninterruptible(event, wait_time);
  }
  return res;
}
