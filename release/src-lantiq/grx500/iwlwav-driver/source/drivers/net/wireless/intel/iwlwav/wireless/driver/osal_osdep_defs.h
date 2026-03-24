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

#include <linux/in.h>
#include <linux/crypto.h>

#include <linux/hardirq.h> /* in_atomic() is here for 2.6 */

#include <linux/ctype.h>
#include <linux/string.h>
#include <linux/ip.h>
#include <linux/ipv6.h>
#include <net/ipv6.h>
#include <linux/hrtimer.h>

#include "mem_leak.h"
#include "compat.h"
#include "mtlk_snprintf.h"

#define LOG_LOCAL_GID   GID_MTLK_OSAL
#define LOG_LOCAL_FID   0

#ifdef ARRAY_SIZE
  #define MTLK_ARRAY_SIZE(arr) ARRAY_SIZE(arr)
#else /* ARRAY_SIZE */
  #define MTLK_ARRAY_SIZE(arr) (sizeof(arr) / sizeof((arr)[0]))
#endif /* ARRAY_SIZE  */

#ifndef BITS_PER_BYTE
#define BITS_PER_BYTE                       (8)
#endif /* BITS_PER_BYTE */

#ifndef DIV_ROUND_UP
#define DIV_ROUND_UP(n, d)                  (((n) + (d) - 1) / (d))
#endif /* DIV_ROUND_UP */

#define BITS_TO_BYTES(n)                    DIV_ROUND_UP(n, BITS_PER_BYTE)


#define mtlk_osal_emergency_print(fmt, ...) printk("[MTLKD] " fmt "\n", ##__VA_ARGS__)

MTLK_DECLARE_OBJPOOL(g_objpool);

#define MTLK_STATIC_ASSERT(x)           BUILD_BUG_ON(!(x))
#define MTLK_STATIC_ASSERT_MSG(x, msg)  BUILD_BUG_ON_MSG(!(x),(msg))
#define MTLK_STATIC_ERROR(msg)          BUILD_BUG_ON_MSG(1,(msg))

typedef enum
{
  MTLK_DATA_BIDIRECTIONAL = PCI_DMA_BIDIRECTIONAL,
  MTLK_DATA_FROM_DEVICE = PCI_DMA_FROMDEVICE,
  MTLK_DATA_TO_DEVICE = PCI_DMA_TODEVICE
} mtlk_data_direction_e;


#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
static __INLINE void*
_mtlk_osal_mem_ddr_dma_pages_alloc (unsigned long size, mtlk_slid_t caller_slid)
{
  unsigned long aligned_size = MTLK_ALIGN_SIZE(size);
  unsigned long full_size = mem_leak_get_full_allocation_size(aligned_size);
  void *buf = (void*)__get_free_pages(GFP_DMA, get_order(full_size));
  return mem_leak_handle_allocated_buffer(buf, aligned_size, caller_slid);
}
#define mtlk_osal_mem_ddr_dma_pages_alloc(size) _mtlk_osal_mem_ddr_dma_pages_alloc(size, MTLK_SLID)
#else
static __INLINE void*
mtlk_osal_mem_ddr_dma_pages_alloc (unsigned long size)
{
  return (void*)__get_free_pages(GFP_DMA, get_order(size));
}
#endif

#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAVE600_NOSNOOP
/* Assumes that memory has been allocated via mtlk_osal_mem_ddr_dma_pages_alloc.
 * Availability of this function depends on X86_PAT compilation option in kernel */
static __INLINE int
mtlk_osal_mem_ddr_dma_pages_set_uncacheable(void* ddr_pages, unsigned long size, int uncacheable)
{
  if (uncacheable)
    return set_memory_uc((unsigned long)ddr_pages, 1 << get_order(size));
  else
    return set_memory_wb((unsigned long)ddr_pages, 1 << get_order(size));
}
#endif

static __INLINE void
mtlk_osal_mem_ddr_dma_pages_free (void* ddr_pages, unsigned long size)
{
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
  void *buf = mem_leak_handle_buffer_to_free(ddr_pages);
  unsigned long full_size = mem_leak_get_full_allocation_size(size);
  free_pages((unsigned long)buf, get_order(full_size));
#else
  free_pages((unsigned long)ddr_pages, get_order(size));
#endif
}

#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAVE600_NOSNOOP
void __MTLK_IFUNC mtlk_osal_sync_cache (void *buffer, uint32 size, mtlk_data_direction_e direction);
#endif

/* Give ownership of the memory area to PCIe device */
uint32  __MTLK_IFUNC
mtlk_osal_map_to_phys_addr (void                  *dev_ctx,
                            void                  *buffer,
                            uint32                 size,
                            mtlk_data_direction_e  direction);

/* Return ownership of the memory area back to CPU */
static __INLINE void
mtlk_osal_unmap_phys_addr (void                  *dev_ctx,
                           uint32                 addr,
                           uint32                 size,
                           mtlk_data_direction_e  direction)
{
  /*  mtlk_osal_map_to_phys_addr() may return 0x0. For such case we don't need to unmap memory area. */
  if (addr) {
    dma_unmap_single(dev_ctx, addr, size, direction);
  }
}

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

#define mtlk_osal_mem_dma_alloc(size, tag)  _mtlk_osal_mem_dma_alloc(size, tag, MTLK_SLID)

static __INLINE void*
_mtlk_osal_mem_dma_alloc(uint32 size, uint32 tag, mtlk_slid_t caller_slid)
{
    return _kmalloc_tag(MTLK_ALIGN_SIZE(size), GFP_ATOMIC | __GFP_DMA, tag, caller_slid);
}

#else

static __INLINE void*
mtlk_osal_mem_dma_alloc (uint32 size, uint32 tag)
{
  return kmalloc_tag(MTLK_ALIGN_SIZE(size), GFP_ATOMIC | __GFP_DMA, tag);
}
#endif /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */


/* Check the 32-bit DMA availability. Must be called during driver initialization */
static __INLINE int
mtlk_osal_mem_dma_check_availability (void *dev_ctx)
{
  /* Wave device is 32-bit addressing device*/
#ifdef CPTCFG_IWLWAV_X86_PUMA7_WAVE600_NOSNOOP
  return (dma_set_mask(dev_ctx, DMA_BIT_MASK(32))) ? MTLK_ERR_NO_RESOURCES : MTLK_ERR_OK;
#else
  return (dma_set_mask_and_coherent(dev_ctx, DMA_BIT_MASK(32))) ? MTLK_ERR_NO_RESOURCES : MTLK_ERR_OK;
#endif
}

/* Allocate DMA coherent memory which is visible for both CPU and device.
 * Typical usage is mailbox, DMA descriptors, rings, etc. */
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
#define mtlk_osal_mem_dma_alloc_coherent(drv_ctx, size, dma_addr)  _mtlk_osal_mem_dma_alloc_coherent((drv_ctx), (size), (dma_addr), (MTLK_SLID))

static __INLINE void*
_mtlk_osal_mem_dma_alloc_coherent (void *dev_ctx, uint32 size, dma_addr_t *dma_addr, mtlk_slid_t caller_slid)
{
  size_t full_size = mem_leak_get_full_allocation_size(size);
  void *buf = dma_alloc_coherent(dev_ctx, full_size, dma_addr, GFP_ATOMIC | GFP_DMA);
  void *out_buf = mem_leak_handle_allocated_buffer(buf, size, caller_slid);
  uintptr_t guard_offset = out_buf - buf;
  (*dma_addr) += guard_offset;
  return out_buf;
}

#else
static __INLINE void*
mtlk_osal_mem_dma_alloc_coherent (void *dev_ctx, uint32 size, dma_addr_t *dma_addr)
{
  return dma_alloc_coherent(dev_ctx, size, dma_addr, GFP_ATOMIC | GFP_DMA);
}
#endif /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */

/* Free memory previously allocated by mtlk_osal_mem_dma_alloc_coherent(). */
static __INLINE void
mtlk_osal_mem_dma_free_coherent (void *dev_ctx, uint32 size, void *buffer, dma_addr_t dma_addr)
{
  if (buffer) {
    /* Unlike with CPU memory allocators, calling dma_free_coherent() with a NULL buffer is not safe. */
    #ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
      void *buf = mem_leak_handle_buffer_to_free(buffer);
      uintptr_t guard_offset = buffer - buf;
      size_t full_size = mem_leak_get_full_allocation_size(size);
      dma_addr -= guard_offset;
      dma_free_coherent(dev_ctx, full_size, buf, dma_addr);
    #else
      dma_free_coherent(dev_ctx, size, buffer, dma_addr);
    #endif
  }
}

static __INLINE void*
mtlk_osal_mem_alloc (uint32 size, uint32 tag)
{
  MTLK_ASSERT(0 != size);
  return kmalloc_tag(size, GFP_ATOMIC, tag);
}

static __INLINE void
mtlk_osal_mem_free (void* buffer)
{
  MTLK_ASSERT(NULL != buffer);
  kfree_tag(buffer);
}

static __INLINE void*
mtlk_osal_vmem_alloc (uint32 size, uint32 tag)
{
  MTLK_ASSERT(0 != size);
  return vmalloc_tag(size, tag);
}

static __INLINE void
mtlk_osal_vmem_free (void* buffer)
{
  MTLK_ASSERT(NULL != buffer);
  vfree_tag(buffer);
}

static __INLINE int
mtlk_osal_lock_init (mtlk_osal_spinlock_t* spinlock)
{
  spin_lock_init(&spinlock->lock);
#ifdef CPTCFG_IWLWAV_DEBUG
  atomic_set(&spinlock->acquired, 0);
#endif
  return MTLK_ERR_OK;
}

static __INLINE void
mtlk_osal_lock_acquire (mtlk_osal_spinlock_t* spinlock)
{
  spin_lock_bh(&spinlock->lock);
#ifdef CPTCFG_IWLWAV_DEBUG
  {
    uint32 acquired = 1;
    acquired = mtlk_osal_atomic_xchg(&spinlock->acquired, acquired);
    MTLK_ASSERT(0 == acquired);
  }
#endif
}

static __INLINE mtlk_handle_t
mtlk_osal_lock_acquire_irq (mtlk_osal_spinlock_t* spinlock)
{
  unsigned long res = 0;
  spin_lock_irqsave(&spinlock->lock, res);
#ifdef CPTCFG_IWLWAV_DEBUG
  {
    uint32 acquired = 1;
    acquired = mtlk_osal_atomic_xchg(&spinlock->acquired, acquired);
    MTLK_ASSERT(0 == acquired);
  }
#endif
  return HANDLE_T(res);
}

static __INLINE void
mtlk_osal_lock_release (mtlk_osal_spinlock_t* spinlock)
{
#ifdef CPTCFG_IWLWAV_DEBUG
  {
    uint32 acquired = 0;
    acquired = mtlk_osal_atomic_xchg(&spinlock->acquired, acquired);
    MTLK_ASSERT(1 == acquired);
  }
#endif
  spin_unlock_bh(&spinlock->lock);
}

static __INLINE void
mtlk_osal_lock_release_irq (mtlk_osal_spinlock_t* spinlock,
                            mtlk_handle_t         acquire_val)
{
#ifdef CPTCFG_IWLWAV_DEBUG
  {
    uint32 acquired = 0;
    acquired = mtlk_osal_atomic_xchg(&spinlock->acquired, acquired);
    MTLK_ASSERT(1 == acquired);
  }
#endif
  spin_unlock_irqrestore(&spinlock->lock, (unsigned long)acquire_val);
}

static __INLINE void
mtlk_osal_lock_cleanup (mtlk_osal_spinlock_t* spinlock)
{
#ifdef CPTCFG_IWLWAV_DEBUG
  MTLK_ASSERT(0 == atomic_read(&spinlock->acquired));
#endif
}

static __INLINE int
mtlk_osal_event_init (mtlk_osal_event_t* event)
{
  event->wait_flag = 0;
  event->terminate_flag = 0;
  mtlk_osal_lock_init(&event->wait_lock);
  init_waitqueue_head(&event->wait_queue);
  return MTLK_ERR_OK;
}

extern int __MTLK_IFUNC _mtlk_osal_event_wait (mtlk_osal_event_t* event, uint32 msec);
static __INLINE int
mtlk_osal_event_wait (mtlk_osal_event_t* event, uint32 msec)
{
  return _mtlk_osal_event_wait(event, msec);
}

static __INLINE void
mtlk_osal_event_set (mtlk_osal_event_t* event)
{
  mtlk_osal_lock_acquire(&event->wait_lock);
  event->wait_flag = 1;
  wake_up(&event->wait_queue);
  mtlk_osal_lock_release(&event->wait_lock);
}

static __INLINE void
mtlk_osal_event_terminate (mtlk_osal_event_t* event)
{
  mtlk_osal_lock_acquire(&event->wait_lock);
  event->wait_flag = 1;
  event->terminate_flag = 1;
  wake_up(&event->wait_queue);
  mtlk_osal_lock_release(&event->wait_lock);
}

static __INLINE void
mtlk_osal_event_reset (mtlk_osal_event_t* event)
{
  mtlk_osal_lock_acquire(&event->wait_lock);
  event->wait_flag = 0;
  event->terminate_flag = 0;
  mtlk_osal_lock_release(&event->wait_lock);
}

static __INLINE void
mtlk_osal_event_cleanup (mtlk_osal_event_t* event)
{
  mtlk_osal_lock_cleanup(&event->wait_lock);
}

static __INLINE int
mtlk_osal_mutex_init (mtlk_osal_mutex_t* mutex)
{
  sema_init(&mutex->sem, 1);
  return MTLK_ERR_OK;
}

static __INLINE void
mtlk_osal_mutex_acquire (mtlk_osal_mutex_t* mutex)
{
  down(&mutex->sem);
}

static __INLINE void
mtlk_osal_mutex_release (mtlk_osal_mutex_t* mutex)
{
  up(&mutex->sem);
}

static __INLINE void
mtlk_osal_mutex_cleanup (mtlk_osal_mutex_t* mutex)
{

}

static __INLINE void
mtlk_osal_msleep (uint32 msec)
{
  msleep(msec);
}

BOOL __MTLK_IFUNC _mtlk_osal_timer_is_stopped(mtlk_osal_timer_t *timer);
BOOL __MTLK_IFUNC _mtlk_osal_timer_is_cancelled(mtlk_osal_timer_t *timer);
int  __MTLK_IFUNC _mtlk_osal_timer_init(mtlk_osal_timer_t *timer,
                                        mtlk_osal_timer_f  clb,
                                        mtlk_handle_t      clb_usr_data);
int  __MTLK_IFUNC _mtlk_osal_timer_set(mtlk_osal_timer_t *timer,
                                       uint32             msec);
int  __MTLK_IFUNC _mtlk_osal_timer_cancel(mtlk_osal_timer_t *timer);
int  __MTLK_IFUNC _mtlk_osal_timer_cancel_sync(mtlk_osal_timer_t *timer);
void __MTLK_IFUNC _mtlk_osal_timer_cleanup(mtlk_osal_timer_t *timer);

BOOL __MTLK_IFUNC _mtlk_osal_hrtimer_is_stopped(mtlk_osal_hrtimer_t *timer);
BOOL __MTLK_IFUNC _mtlk_osal_hrtimer_is_cancelled(mtlk_osal_hrtimer_t *timer);
int  __MTLK_IFUNC _mtlk_osal_hrtimer_init(mtlk_osal_hrtimer_t  *timer,
                                          mtlk_osal_hrtimer_f   clb,
                                          mtlk_handle_t         clb_usr_data,
                                          mtlk_hrtimer_mode_e   mode);
int  __MTLK_IFUNC _mtlk_osal_hrtimer_set(mtlk_osal_hrtimer_t *timer,
                                         int64                nsec,
                                         mtlk_hrtimer_mode_e  mode);
int  __MTLK_IFUNC _mtlk_osal_hrtimer_cancel(mtlk_osal_hrtimer_t *timer);
int  __MTLK_IFUNC _mtlk_osal_hrtimer_cancel_sync(mtlk_osal_hrtimer_t *timer);
void __MTLK_IFUNC _mtlk_osal_hrtimer_cleanup(mtlk_osal_hrtimer_t *timer);


static __INLINE BOOL
mtlk_osal_timer_is_stopped (mtlk_osal_timer_t *timer)
{
  return _mtlk_osal_timer_is_stopped(timer);
}

static __INLINE BOOL
mtlk_osal_timer_is_cancelled (mtlk_osal_timer_t *timer)
{
  return _mtlk_osal_timer_is_cancelled(timer);
}

static __INLINE int
mtlk_osal_timer_init (mtlk_osal_timer_t *timer,
                     mtlk_osal_timer_f  clb,
                     mtlk_handle_t      clb_usr_data)
{
  return _mtlk_osal_timer_init(timer, clb, clb_usr_data);
}

static __INLINE int
mtlk_osal_timer_set (mtlk_osal_timer_t *timer,
                     uint32             msec)
{
  return _mtlk_osal_timer_set(timer, msec);
}

static __INLINE int
mtlk_osal_timer_cancel (mtlk_osal_timer_t *timer)
{
  return _mtlk_osal_timer_cancel(timer);
}

static __INLINE int
mtlk_osal_timer_cancel_sync (mtlk_osal_timer_t *timer)
{
  return _mtlk_osal_timer_cancel_sync(timer);
}

static __INLINE void
mtlk_osal_timer_cleanup (mtlk_osal_timer_t *timer)
{
  _mtlk_osal_timer_cleanup(timer);
}


static __INLINE BOOL
mtlk_osal_hrtimer_is_stopped (mtlk_osal_hrtimer_t *timer)
{
  return _mtlk_osal_hrtimer_is_stopped(timer);
}

static __INLINE BOOL
mtlk_osal_hrtimer_is_cancelled (mtlk_osal_hrtimer_t *timer)
{
  return _mtlk_osal_hrtimer_is_cancelled(timer);
}

static __INLINE int
mtlk_osal_hrtimer_init (mtlk_osal_hrtimer_t *timer,
                        mtlk_osal_hrtimer_f  clb,
                        mtlk_handle_t        clb_usr_data,
                        mtlk_hrtimer_mode_e  mode)
{
  return _mtlk_osal_hrtimer_init(timer, clb, clb_usr_data, mode);
}

static __INLINE int
mtlk_osal_hrtimer_set (mtlk_osal_hrtimer_t *timer,
                       int64                nsec,
                       mtlk_hrtimer_mode_e  mode)
{
  return _mtlk_osal_hrtimer_set(timer, nsec, mode);
}

static __INLINE int
mtlk_osal_hrtimer_cancel (mtlk_osal_hrtimer_t *timer)
{
  return _mtlk_osal_hrtimer_cancel(timer);
}

static __INLINE int
mtlk_osal_hrtimer_cancel_sync (mtlk_osal_hrtimer_t *timer)
{
  return _mtlk_osal_hrtimer_cancel_sync(timer);
}

static __INLINE void
mtlk_osal_hrtimer_cleanup (mtlk_osal_hrtimer_t *timer)
{
  _mtlk_osal_hrtimer_cleanup(timer);
}

/* hr_timestamp */

static __INLINE mtlk_osal_hr_timestamp_t
mtlk_osal_hr_timestamp_ns (void)
{
  return ktime_get_ns();
}

static __INLINE mtlk_osal_hr_timestamp_t
mtlk_osal_hr_timestamp_us (void)
{
  mtlk_osal_hr_timestamp_t timestamp = mtlk_osal_hr_timestamp_ns();
  do_div(timestamp, MTLK_OSAL_NSEC_IN_USEC);
  return timestamp;
}

/* timestamp */

static __INLINE mtlk_osal_timestamp_t
mtlk_osal_timestamp (void)
{
  return jiffies;
}

static __INLINE mtlk_osal_msec_t
mtlk_osal_timestamp_to_ms (mtlk_osal_timestamp_t timestamp)
{
  return jiffies_to_msecs(timestamp);
}

static __INLINE mtlk_osal_usec_t
mtlk_osal_timestamp_to_us (mtlk_osal_timestamp_t timestamp)
{
  return jiffies_to_usecs(timestamp);
}

static __INLINE mtlk_osal_timestamp_t
mtlk_osal_ms_to_timestamp (mtlk_osal_msec_t msecs)
{
  return msecs_to_jiffies(msecs);
}

static __INLINE int
mtlk_osal_time_after (mtlk_osal_timestamp_t tm1, mtlk_osal_timestamp_t tm2)
{
  return time_after(tm1, tm2);
}

static __INLINE mtlk_osal_ms_diff_t
mtlk_osal_ms_time_diff (mtlk_osal_msec_t tm1, mtlk_osal_msec_t tm2)
{
  return ( (mtlk_osal_ms_diff_t) ( (uint32)(tm1) - (uint32)(tm2) ) );
}

static __INLINE mtlk_osal_timestamp_diff_t
mtlk_osal_timestamp_diff (mtlk_osal_timestamp_t tm1, mtlk_osal_timestamp_t tm2)
{
  return ( (mtlk_osal_timestamp_diff_t) ( (uint32)(tm1) - (uint32)(tm2) ) );
}

static __INLINE mtlk_osal_timestamp_diff_t
mtlk_osal_time_passed(mtlk_osal_timestamp_t tm)
{
  return mtlk_osal_timestamp_diff(mtlk_osal_timestamp(), tm);
}

static __INLINE uint32
mtlk_osal_time_passed_ms(mtlk_osal_timestamp_t tm)
{
  return mtlk_osal_timestamp_to_ms(mtlk_osal_time_passed(tm));
}

static __INLINE int
mtlk_osal_time_get_mseconds_ago(mtlk_osal_timestamp_t tm)
{
  return mtlk_osal_time_passed_ms(tm) % MTLK_OSAL_MSEC_IN_SEC;
}

static __INLINE int
mtlk_osal_time_get_seconds_ago(mtlk_osal_timestamp_t timestamp)
{
  return (mtlk_osal_time_passed_ms(timestamp) / MTLK_OSAL_MSEC_IN_SEC)
    % MTLK_OSAL_SEC_IN_MIN;
}

static __INLINE int
mtlk_osal_time_get_minutes_ago(mtlk_osal_timestamp_t timestamp)
{
  return (mtlk_osal_time_passed_ms(timestamp) / MTLK_OSAL_MSEC_IN_MIN)
      % MTLK_OSAL_MIN_IN_HOUR;
}

static __INLINE int
mtlk_osal_time_get_hours_ago(mtlk_osal_timestamp_t timestamp)
{
  return (mtlk_osal_time_passed_ms(timestamp) /
    (MTLK_OSAL_MSEC_IN_MIN * MTLK_OSAL_MIN_IN_HOUR));
}

#include <linux/etherdevice.h>

static __INLINE void mtlk_osal_zero_ip4_address (void* addr)
{
  uint16 *a = (uint16 *) addr;
  a[0] = a[1] = 0;
}

static __INLINE void mtlk_osal_copy_ip4_addresses (void* dst, const void* src)
{
  u16 *a = (u16 *) dst;
  const u16 *b = (const u16 *) src;
  a[0] = b[0]; a[1] = b[1];
}

static __INLINE void mtlk_osal_zero_eth_address(u8 *addr)
{
    eth_zero_addr(addr);
}

static __INLINE void
mtlk_osal_copy_eth_addresses (uint8* dst,
                              const uint8* src)
{
  ether_addr_copy(dst, src);
}

static __INLINE unsigned
mtlk_osal_compare_eth_addresses (const uint8* addr1,
                                 const uint8* addr2)
{
  return compare_ether_addr(addr1, addr2);
}

static __INLINE BOOL
mtlk_osal_is_zero_address (const uint8* addr)
{
  return is_zero_ether_addr(addr);
}

static __INLINE BOOL
mtlk_osal_eth_is_multicast (const uint8* addr)
{
  return is_multicast_ether_addr(addr);
}

static __INLINE BOOL
mtlk_osal_eth_is_broadcast (const uint8* addr)
{
  return is_broadcast_ether_addr(addr);
}

/**
 * Check that the Ethernet address (MAC) is not 00:00:00:00:00:00, is not
 * a multicast address, and is not FF:FF:FF:FF:FF:FF.
 */
static __INLINE BOOL
mtlk_osal_is_valid_ether_addr(const uint8* addr)
{
  return is_valid_ether_addr(addr);
}

static __INLINE BOOL
mtlk_osal_ipv4_is_multicast (uint32 addr)
{
  return ipv4_is_multicast(addr);
}

static __INLINE BOOL
mtlk_osal_ipv4_is_local_multicast (uint32 addr)
{
  return ipv4_is_local_multicast(addr);
}

static __INLINE BOOL
mtlk_osal_eth_is_group_addr (const uint8* addr)
{
  return (((uint8*)(addr))[0] & 0x01);
}

static __INLINE
BOOL mtlk_osal_eth_is_directed (const uint8* addr)
{
  return !mtlk_osal_eth_is_group_addr(addr);
}

static __INLINE void
mtlk_osal_eth_apply_mask(uint8* dst, uint8* src, const uint8* mask)
{
  dst[0] = src[0] & mask[0];
  dst[1] = src[1] & mask[1];
  dst[2] = src[2] & mask[2];
  dst[3] = src[3] & mask[3];
  dst[4] = src[4] & mask[4];
  dst[5] = src[5] & mask[5];
}

/* Functions below return ZERO if addresses are equal */

static __INLINE int
mtlk_osal_ipv4_addr_cmp (const struct in_addr *a1, const struct in_addr *a2)
{
  return (a1->s_addr != a2->s_addr);
}

static __INLINE int
mtlk_osal_ipv4_masked_addr_cmp (const struct in_addr *a1, const struct in_addr *m,
                                const struct in_addr *a2)
{
  return !!(((a1->s_addr ^ a2->s_addr) & m->s_addr));
}

static __INLINE int
mtlk_osal_ipv6_addr_cmp (const struct in6_addr *a1, const struct in6_addr *a2)
{
  return ipv6_addr_cmp(a1, a2);
}

static __INLINE int
mtlk_osal_ipv6_masked_addr_cmp (const struct in6_addr *a1, const struct in6_addr *m,
                                const struct in6_addr *a2)
{
  return ipv6_masked_addr_cmp(a1, m, a2);
}


/* atomic counters */

#define __MTLK_ATOMIC_API      static __INLINE
#include "osal_atomic_defs.h"

static __INLINE uint32
mtlk_osal_atomic_add (mtlk_atomic_t* val, uint32 i)
{
  return __mtlk_osal_atomic_add(val, i);
}

static __INLINE uint32
mtlk_osal_atomic_sub (mtlk_atomic_t* val, uint32 i)
{
  return __mtlk_osal_atomic_sub(val, i);
}

static __INLINE uint32
mtlk_osal_atomic_inc (mtlk_atomic_t* val)
{
  return __mtlk_osal_atomic_inc(val);
}

static __INLINE uint32
mtlk_osal_atomic_dec (mtlk_atomic_t* val)
{
  return __mtlk_osal_atomic_dec(val);
}

static __INLINE void
mtlk_osal_atomic_set (mtlk_atomic_t* target, uint32 value)
{
  return __mtlk_osal_atomic_set(target, value);
}

static __INLINE uint32
mtlk_osal_atomic_get (const mtlk_atomic_t* val)
{
  return __mtlk_osal_atomic_get(val);
}

static __INLINE uint32
mtlk_osal_atomic_xchg (mtlk_atomic_t* target, uint32 value)
{
  return __mtlk_osal_atomic_xchg(target, value);
}

#define __MTLK_ATOMIC64_API      static __INLINE
#ifndef ATOMIC64_INIT
  extern mtlk_osal_spinlock_t  mtlk_osal_atomic64_lock;
#endif
#include "osal_atomic64_defs.h"

static __INLINE uint64
mtlk_osal_atomic64_add (mtlk_atomic64_t* val, uint64 i)
{
  return __mtlk_osal_atomic64_add(val, i);
}

static __INLINE uint64
mtlk_osal_atomic64_sub (mtlk_atomic64_t* val, uint64 i)
{
  return __mtlk_osal_atomic64_sub(val, i);
}

static __INLINE uint64
mtlk_osal_atomic64_inc (mtlk_atomic64_t* val)
{
  return __mtlk_osal_atomic64_inc(val);
}

static __INLINE uint64
mtlk_osal_atomic64_dec (mtlk_atomic64_t* val)
{
  return __mtlk_osal_atomic64_dec(val);
}

static __INLINE void
mtlk_osal_atomic64_set (mtlk_atomic64_t* target, uint64 value)
{
  return __mtlk_osal_atomic64_set(target, value);
}

static __INLINE uint64
mtlk_osal_atomic64_get (mtlk_atomic64_t* val)
{
  return __mtlk_osal_atomic64_get(val);
}

static __INLINE void
mtlk_osal_kernel_panic (const char * pMsg)
{
  char msg[128];
  mtlk_snprintf(msg, sizeof(msg), "Wireless LAN firmware/hardware fatal error - %s", pMsg);
  panic(msg);
}

static __INLINE uint64
mtlk_osal_atomic64_xchg (mtlk_atomic64_t* target, uint64 value)
{
  return __mtlk_osal_atomic64_xchg(target, value);
}


#include "mtlkrmlock.h"

struct _mtlk_osal_timer_t
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  struct timer_list    os_timer;
  mtlk_osal_timer_f    clb;
  mtlk_handle_t        clb_usr_data;
  mtlk_rmlock_t        rmlock;
  mtlk_osal_spinlock_t sync_lock;
  volatile BOOL        stop;
  volatile BOOL        cancelled;
  unsigned long        diff_time;
};

struct _mtlk_osal_hrtimer_t
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  struct hrtimer         hr_timer;
  mtlk_osal_hrtimer_f    clb;
  mtlk_handle_t          clb_usr_data;
  mtlk_rmlock_t          rmlock;
  mtlk_osal_spinlock_t   sync_lock;
  volatile BOOL          stop;
  volatile BOOL          cancelled;
  unsigned long          diff_time;
  struct tasklet_struct  hrtimer_tasklet;
};


#define mtlk_osal_str_atol(s) simple_strtol(s, NULL, 0)
#define mtlk_osal_str_memchr(s, c, n) memchr(s, c, n)
#define mtlk_osal_strlcpy(dst, src, n) strlcpy(dst, src, n)
#define mtlk_osal_strnlen(s, n) strnlen(s, n)

#include <linux/crc32.h>
#define mtlk_osal_crc32(c, p, l) crc32(c, p, l)

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID
