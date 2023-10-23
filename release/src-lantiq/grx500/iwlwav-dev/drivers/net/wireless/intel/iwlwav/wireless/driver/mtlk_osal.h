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
* $Id$
*
* 
*
*/

#ifndef __MTLK_OSAL_H__
#define __MTLK_OSAL_H__

/*! \file  mtlk_osal.h
    \brief Driver OS Abstraction Layer definitions
 
    The following abstractions are defined:
    - memory allocation/freeing interface
    - spin lock interface
    - waitable event interface
    - timer interface
    - timestamp interface
    
    Prerequisites (osal_defs.h):
    - OS-dependent OSAL definitions header file (osal_defs.h) must be 
      written and placed under a OS-specific folder
    - mtlk_osal_spinlock_t type defined (usually within the osal_defs.h)
    - mtlk_osal_event_t type defined (usually within the osal_defs.h)
    - mtlk_osal_timer_t type defined (usually within the osal_defs.h)
    - mtlk_osal_msec_t type defined (usually within the osal_defs.h)
    - mtlk_osal_ms_diff_t type defined (usually within the osal_defs.h)
    - mtlk_osal_timestamp_diff_t type defined (usually within the osal_defs.h)
    - mtlk_osal_timestamp_t type defined (usually within the osal_defs.h)
    - optionally: one or more OSAL_... defines defined (usually within 
      the osal_defs, see the Implementation notes below for more info)
 
    Implementation notes:
    - Each OSAL function can be implemented as a function or as a inline 
      function wrapped macro. Inline function wrapped macros are used 
      to garantee the functions prototypes compilation time checking.
    - To implement a mtlk_osal...() function as function just put the
      function's body somewhere in your code. For example, write the
      osal_defs.c under your OS-specific folder.
    - To implement a mtlk_osal...() function as inline function wrapped
      macro, put the appropriate OSAL... define within your osal_defs.h 
 */

#include "mtlkerr.h"
#include "memtags.h"
#include "mtlk_objpool.h"

#define  SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_DECLS
#include "osal_osdep_decls.h"

#include "memdefs.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/**********************************************************************
 * OSAL init/cleanup abstraction
***********************************************************************/
/*! 
  \fn      int __MTLK_IFUNC mtlk_osal_init(void)
  \brief   Initiates the OSAL. 

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded

  \warning This routine must be called prior to any other OSAL call
 */
/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_cleanup(void)
  \brief   Cleans the OSAL up. 

  \warning This routine must be called after all other OSAL calls
 */
int  __MTLK_IFUNC mtlk_osal_init(void);
void __MTLK_IFUNC mtlk_osal_cleanup(void);

/**********************************************************************
 * Memory allocation/freeing abstraction
***********************************************************************/
/*! 
  \fn      void* __MTLK_IFUNC mtlk_osal_mem_alloc(uint32 size, uint32 tag)
  \brief   Allocates memory. 

  \param   size Size of requested memory region
  \param   tag  Tag for memory allocation. Used for memory leaks debugging. See memtags.h for more info.

  \return  Pointer to allocated buffer, NULL if failed

  \warning This routine allocates memory that can not be used for DMA transactions
 */

/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_mem_free(void* buffer)
  \brief   Frees memory. 

  \param   buffer Memory buffer previously allocated by mtlk_osal_alloc_memory()
 */

static __INLINE void* mtlk_osal_mem_alloc(uint32 size, uint32 tag);
static __INLINE void mtlk_osal_mem_free(void* buffer);
/**********************************************************************/

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
static __INLINE void*
_mtlk_osal_mem_ddr_dma_pages_alloc (unsigned long size, mtlk_slid_t caller_slid);
#else
static __INLINE void*
mtlk_osal_mem_ddr_dma_pages_alloc (unsigned long size);
#endif

static __INLINE void
mtlk_osal_mem_ddr_dma_pages_free (void* ddr_pages, unsigned long size);

/**********************************************************************
 * Spinlock abstraction
***********************************************************************/
/*! 
  \fn      int __MTLK_IFUNC mtlk_osal_lock_init(mtlk_osal_spinlock_t* spinlock)
  \brief   Initializes spinlock object. 

  \param   spinlock Spinlock object

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */

/*! 
  \fn      mtlk_handle_t __MTLK_IFUNC mtlk_osal_lock_acquire(mtlk_osal_spinlock_t* spinlock)
  \brief   Acquires spinlock object. 

  \param   spinlock Spinlock object

  \return  Handle value to be passed to mtlk_osal_lock_release
 */

/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_lock_release(mtlk_osal_spinlock_t* spinlock, mtlk_handle_t acquire_val)
  \brief   Releases spinlock object. 

  \param   spinlock    Spinlock object
  \param   acquire_val Handle value returned by mtlk_osal_lock_acquire
 */

/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_lock_cleanup(mtlk_osal_spinlock_t* spinlock)
  \brief   Cleans up spinlock object. 

  \param   spinlock Spinlock object
 */

static __INLINE int           mtlk_osal_lock_init(mtlk_osal_spinlock_t* spinlock);
static __INLINE void          mtlk_osal_lock_acquire(mtlk_osal_spinlock_t* spinlock);
static __INLINE mtlk_handle_t mtlk_osal_lock_acquire_irq(mtlk_osal_spinlock_t* spinlock);
static __INLINE void          mtlk_osal_lock_release(mtlk_osal_spinlock_t* spinlock);
static __INLINE void          mtlk_osal_lock_release_irq(mtlk_osal_spinlock_t* spinlock, 
                                                         mtlk_handle_t acquire_val);
static __INLINE void          mtlk_osal_lock_cleanup(mtlk_osal_spinlock_t* spinlock);
/**********************************************************************/

/**********************************************************************
 * Event abstraction
***********************************************************************/
#ifndef MTLK_OSAL_EVENT_INFINITE
#error MTLK_OSAL_EVENT_INFINITE must be defined within the osal_defs.h
#endif
/*! 
  \fn      int __MTLK_IFUNC mtlk_osal_event_init(mtlk_osal_event_t* event);
  \brief   Initializes event object. 

  \param   event Event object

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */

/*! 
  \fn      int __MTLK_IFUNC mtlk_osal_event_wait(mtlk_osal_event_t* event, uint32 msec);
  \brief   Waits for event object to be set with timeout. 

  \param   event Event object
  \param   msec  Maximal time to wait (in milliseconds)

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded, MTLK_ERR_TIMEOUT if timed out
 */

/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_event_set(mtlk_osal_event_t* event);
  \brief   Sets event object. 

  \param   event Event object

  \warning This function must not be called from the ISR.
 */

/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_event_reset(mtlk_osal_event_t* event);
  \brief   Resets event object. 

  \param   event Event object
 */

/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_event_cleanup(mtlk_osal_event_t* event);
  \brief   Cleans up event object. 

  \param   event Event object
 */

static __INLINE int  mtlk_osal_event_init(mtlk_osal_event_t* event);
static __INLINE int  mtlk_osal_event_wait(mtlk_osal_event_t* event, uint32 msec);
static __INLINE void mtlk_osal_event_set(mtlk_osal_event_t* event);
static __INLINE void mtlk_osal_event_reset(mtlk_osal_event_t* event);
static __INLINE void mtlk_osal_event_cleanup(mtlk_osal_event_t* event);
/**********************************************************************/

/**********************************************************************
 * Mutex abstraction
***********************************************************************/
static __INLINE int  mtlk_osal_mutex_init(mtlk_osal_mutex_t* mutex);
static __INLINE void mtlk_osal_mutex_acquire(mtlk_osal_mutex_t* mutex);
static __INLINE void mtlk_osal_mutex_release(mtlk_osal_mutex_t* mutex);
static __INLINE void mtlk_osal_mutex_cleanup(mtlk_osal_mutex_t* mutex);
/**********************************************************************/

/**********************************************************************
 * Auxiliary functions
***********************************************************************/
static __INLINE void mtlk_osal_msleep(uint32 msec);
/**********************************************************************/

/**********************************************************************
 * Timer abstraction
***********************************************************************/
/*! 
  \fn      int __MTLK_IFUNC mtlk_osal_timer_init(mtlk_osal_timer_t *timer)
  \brief   Initializes timer object. 

  \param   timer Timer object

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */

/*! 
  \fn      int __MTLK_IFUNC mtlk_osal_timer_set(mtlk_osal_timer_t *timer,
                                                uint32             msec,
                                                mtlk_osal_timer_f  clb,
                                                mtlk_handle_t      clb_usr_data)
  \brief   Sets (requests) timer. 

  \param   timer        Timer object
  \param   msec         Requested time-out value in milliseconds
  \param   clb          Callback to be notified when the time-out value elapses
  \param   clb_usr_data User data to be passed to the callback

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */

/*! 
  \fn      int __MTLK_IFUNC mtlk_osal_timer_cancel(mtlk_osal_timer_t *timer)
  \brief   Cancels the timer. 

  \param   timer Timer object

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */


/*! 
  \fn      void __MTLK_IFUNC mtlk_osal_timer_cleanup(mtlk_osal_timer_t *timer)
  \brief   Cleans up the timer. 

  \param   timer Timer object
 */


/*! 
  \typedef typedef uint32 (*mtlk_osal_timer_f)(mtlk_osal_timer_t *timer, 
                                               mtlk_handle_t      clb_usr_data);
  \brief   Timer callback function
    
  \param   timer       Timer object
  \param   clb_usr_dat User data passed to mtlk_osal_timer_set

  \return  milliseconds to re-arm the timer, 0 to stop re-arming
*/

/* NOTE: return value is msecs to re-arm the timer, 0 if not required */
typedef uint32 (__MTLK_IFUNC *mtlk_osal_timer_f)(mtlk_osal_timer_t *timer, 
                                                 mtlk_handle_t      clb_usr_data); 

static __INLINE BOOL mtlk_osal_timer_is_stopped(mtlk_osal_timer_t *timer);
static __INLINE int  mtlk_osal_timer_init(mtlk_osal_timer_t *timer,
                                          mtlk_osal_timer_f  clb,
                                          mtlk_handle_t      clb_usr_data);
static __INLINE int  mtlk_osal_timer_set(mtlk_osal_timer_t *timer,
                                         uint32             msec);
static __INLINE int  mtlk_osal_timer_cancel(mtlk_osal_timer_t *timer);
static __INLINE int  mtlk_osal_timer_cancel_sync(mtlk_osal_timer_t *timer);
static __INLINE void mtlk_osal_timer_cleanup(mtlk_osal_timer_t *timer);

/**********************************************************************
 * High resolution Timer abstraction
***********************************************************************/
/*!
  \fn      int __MTLK_IFUNC mtlk_osal_hrtimer_init(mtlk_osal_hrtimer_t *timer)
  \brief   Initializes timer object.

  \param   timer Timer object

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */

/*!
  \fn      int __MTLK_IFUNC mtlk_osal_hrtimer_set(mtlk_osal_hrtimer_t *timer,
                                                int64                  nsec,
                                                mtlk_hrtimer_mode_e    mode)
  \brief   Sets (requests) timer.

  \param   timer        Timer object
  \param   nsec         Requested time-out value in nanoseconds
  \param   mode         Timer mode

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */

/*!
  \fn      int __MTLK_IFUNC mtlk_osal_hrtimer_cancel(mtlk_osal_hrtimer_t *timer)
  \brief   Cancels the timer.

  \param   timer Timer object

  \return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
 */


/*!
  \fn      void __MTLK_IFUNC mtlk_osal_hrtimer_cleanup(mtlk_osal_hrtimer_t *timer)
  \brief   Cleans up the timer.

  \param   timer Timer object
 */


/*!
  \typedef typedef uint32 (*mtlk_osal_hrtimer_f)(mtlk_osal_hrtimer_t *timer,
                                                 mtlk_handle_t        clb_usr_data);
  \brief   Timer callback function

  \param   timer       Timer object
  \param   clb_usr_dat User data passed to mtlk_osal_timer_set

  \return  milliseconds to re-arm the timer, 0 to stop re-arming
*/

/* NOTE: return nanoseconds to re-arm the hrtimer, 0 if not required */
typedef int (__MTLK_IFUNC *mtlk_osal_hrtimer_f)(mtlk_osal_hrtimer_t *timer,
                                                  mtlk_handle_t      clb_usr_data);

static __INLINE BOOL mtlk_osal_hrtimer_is_stopped(mtlk_osal_hrtimer_t *timer);
static __INLINE int  mtlk_osal_hrtimer_init(mtlk_osal_hrtimer_t *timer,
                                            mtlk_osal_hrtimer_f  clb,
                                            mtlk_handle_t        clb_usr_data,
                                            mtlk_hrtimer_mode_e  mode);
static __INLINE int  mtlk_osal_hrtimer_set(mtlk_osal_hrtimer_t *timer,
                                           int64                nsec,
                                           mtlk_hrtimer_mode_e  mode);
static __INLINE int  mtlk_osal_hrtimer_cancel(mtlk_osal_hrtimer_t *timer);
static __INLINE int  mtlk_osal_hrtimer_cancel_sync(mtlk_osal_hrtimer_t *timer);
static __INLINE void mtlk_osal_hrtimer_cleanup(mtlk_osal_hrtimer_t *timer);

/**********************************************************************/

/**********************************************************************
 * Timestamp abstraction
***********************************************************************/
/*! 
  \fn      mtlk_osal_timestamp_t __MTLK_IFUNC mtlk_osal_timestamp()
  \brief   Produces timestamp. 

  \return  Timestamp for the current time
 */

/*! 
  \fn      uint32 __MTLK_IFUNC mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp_t timestamp)
  \brief   Converts timestamp value to the number of milliseconds. 

  \param   timestamp    Timestamp value to be converted

  \return  Number of milliseconds
 */

/*! 
\fn      mtlk_osal_timestamp_t __MTLK_IFUNC mtlk_osal_ms_to_timestamp(uint32 msecs)
\brief   Converts number of milliseconds to timestamp value.

\param   msecs    Number of msecs to be converted

\return  Timestamp value
*/

/*! 
\fn      int __MTLK_IFUNC mtlk_osal_time_after(mtlk_osal_timestamp_t tm1, mtlk_osal_timestamp_t tm2)
\brief   Checks whether time moment tm1 occurred after time moment tm2.

\param   tm1    First time stamp
\param   tm2    Second time stamp

\return  NON-ZERO if tm1 occurred after tm2, ZERO otherwise
*/

/*! 
\fn      mtlk_osal_ms_diff_t __MTLK_IFUNC mtlk_osal_ms_time_diff(mtlk_osal_msec_t tm1, mtlk_osal_msec_t tm2)
\brief   Returns number of milliseconds passed between 2 timestamps.

\param   tm1    First number of msecs
\param   tm2    Second number of msecs

\return  Number of milliseconds
*/

/*! 
\fn      mtlk_osal_timestamp_diff_t __MTLK_IFUNC mtlk_osal_timestamp_diff(mtlk_osal_timestamp_t tm1, mtlk_osal_timestamp_t tm2)
\brief   Returns time difference between 2 timestamps.

\param   tm1    First time stamp
\param   tm2    Second time stamp

\return  Amount of time passed between two timestamps in timestamp slices
*/

static __INLINE mtlk_osal_timestamp_t      mtlk_osal_timestamp(void);
static __INLINE mtlk_osal_msec_t           mtlk_osal_timestamp_to_ms(mtlk_osal_timestamp_t timestamp);
static __INLINE mtlk_osal_timestamp_t      mtlk_osal_ms_to_timestamp(mtlk_osal_msec_t msecs);
static __INLINE int                        mtlk_osal_time_after(mtlk_osal_timestamp_t tm1, mtlk_osal_timestamp_t tm2);
static __INLINE mtlk_osal_ms_diff_t        mtlk_osal_ms_time_diff(mtlk_osal_msec_t tm1, mtlk_osal_msec_t tm2);
static __INLINE mtlk_osal_timestamp_diff_t mtlk_osal_timestamp_diff(mtlk_osal_timestamp_t tm1, mtlk_osal_timestamp_t tm2);

#define MTLK_OSAL_USEC_IN_MSEC       (1000)
#define MTLK_OSAL_NSEC_IN_USEC       (1000)
#define MTLK_OSAL_MSEC_IN_SEC        (1000)
#define MTLK_OSAL_SEC_IN_MIN         (60)
#define MTLK_OSAL_MIN_IN_HOUR        (60)
#define MTLK_OSAL_MSEC_IN_MIN        (MTLK_OSAL_SEC_IN_MIN * MTLK_OSAL_MSEC_IN_SEC)
#define MTLK_OSAL_USEC_IN_SEC        (MTLK_OSAL_USEC_IN_MSEC * MTLK_OSAL_MSEC_IN_SEC)
#define MTLK_OSAL_NSEC_IN_MSEC       (MTLK_OSAL_NSEC_IN_USEC * MTLK_OSAL_USEC_IN_MSEC)

/*! 
\brief   Returns time difference between current time and timestamp given.
\param   tm    Timestamp
\return  Amount of time passed from "tm" moment of time
*/
static __INLINE mtlk_osal_timestamp_diff_t mtlk_osal_time_passed(mtlk_osal_timestamp_t tm);

/*! 
\brief   Returns time difference in milliseconds between current time and timestamp given.
\param   tm    Timestamp
\return  Number of milliseconds passed from "tm" moment of time
*/
static __INLINE uint32 mtlk_osal_time_passed_ms(mtlk_osal_timestamp_t tm);

/*
 * The set of functions below convert timestamp to the common time format */
static __INLINE int mtlk_osal_time_get_mseconds_ago(mtlk_osal_timestamp_t tm);
static __INLINE int mtlk_osal_time_get_seconds_ago(mtlk_osal_timestamp_t tm);
static __INLINE int mtlk_osal_time_get_minutes_ago(mtlk_osal_timestamp_t tm);
static __INLINE int mtlk_osal_time_get_hours_ago(mtlk_osal_timestamp_t tm);

/**********************************************************************/

/**********************************************************************
* Ethernet addresses abstraction
***********************************************************************/
static const uint8 mtlk_osal_eth_zero_addr[ETH_ALEN] = {0, 0, 0, 0, 0, 0};
/*! 
\fn      unsigned __MTLK_IFUNC mtlk_osal_compare_eth_addresses(const uint8* addr1, const uint8* addr2);
\brief   Compares Ethernet addresses.

\param   addr1    First address
\param   addr2    Second address

\return  zero if addresses are equal, non-zero otherwise
*/
static __INLINE unsigned mtlk_osal_compare_eth_addresses(const uint8* addr1, const uint8* addr2);
static __INLINE void mtlk_osal_copy_eth_addresses(uint8* dst, const uint8* src);
static __INLINE BOOL mtlk_osal_is_zero_address(const uint8* addr);
static __INLINE BOOL mtlk_osal_eth_is_multicast(const uint8* addr);
static __INLINE BOOL mtlk_osal_eth_is_broadcast(const uint8* addr);
static __INLINE BOOL mtlk_osal_is_valid_ether_addr(const uint8* addr);
static __INLINE BOOL mtlk_osal_eth_is_group_addr(const uint8* addr);
static __INLINE BOOL mtlk_osal_eth_is_directed(const uint8* addr);
static __INLINE void mtlk_osal_eth_apply_mask(uint8* dst, uint8* src, const uint8* mask);

static __INLINE void mtlk_osal_zero_ip4_address(void* addr);
static __INLINE void mtlk_osal_copy_ip4_addresses(void* dst, const void* src);
/**********************************************************************/

/**********************************************************************
* IPC4 addresses abstraction
***********************************************************************/
static __INLINE BOOL mtlk_osal_ipv4_is_multicast(uint32 addr);       /* Big-Endian uint32 address */
static __INLINE BOOL mtlk_osal_ipv4_is_local_multicast(uint32 addr); /* Big-Endian uint32 address */
/**********************************************************************/

/**********************************************************************
* Atomic operations abstraction
***********************************************************************/
/*! 
\fn      uint32 __MTLK_IFUNC mtlk_osal_atomic_inc(mtlk_atomic_t* val)
\brief   Increments a caller-supplied variable as an atomic operation.

\param   val    Pointer to a variable to be incremented

\return  the incremented value
*/

/*! 
\fn      uint32 __MTLK_IFUNC mtlk_osal_atomic_dec(mtlk_atomic_t* val)
\brief   Decrements a caller-supplied variable as an atomic operation.

\param   val    Pointer to a variable to be decremented

\return  the decremented value
*/

/*! 
\fn      uint32 __MTLK_IFUNC mtlk_osal_atomic_get(mtlk_atomic_t* val)
\brief   Reads value of caller-supplied variable as an atomic operation.

\param   val    Pointer to a variable to be read

\return  the value of variable
*/

/*! 
\fn      uint32 __MTLK_IFUNC mtlk_osal_atomic_set(mtlk_atomic_t* target, uint32 value)
\brief   Sets value of caller-supplied variable to a given value as an atomic operation.

\param   target    Pointer to a variable to be set
\param   value     Specifies the value to which the variable will be set

\return  the value of the variable at target when the call occurred
*/
static __INLINE uint32 mtlk_osal_atomic_add(mtlk_atomic_t* val, uint32 i);
static __INLINE uint32 mtlk_osal_atomic_sub(mtlk_atomic_t* val, uint32 i);
static __INLINE uint32 mtlk_osal_atomic_inc(mtlk_atomic_t* val);
static __INLINE uint32 mtlk_osal_atomic_dec(mtlk_atomic_t* val);
static __INLINE void   mtlk_osal_atomic_set(mtlk_atomic_t* target, uint32 value);
static __INLINE uint32 mtlk_osal_atomic_get(const mtlk_atomic_t* val);
static __INLINE uint32 mtlk_osal_atomic_xchg(mtlk_atomic_t* target, uint32 value);

static __INLINE uint64 mtlk_osal_atomic64_add(mtlk_atomic64_t* val, uint64 i);
static __INLINE uint64 mtlk_osal_atomic64_sub(mtlk_atomic64_t* val, uint64 i);
static __INLINE uint64 mtlk_osal_atomic64_inc(mtlk_atomic64_t* val);
static __INLINE uint64 mtlk_osal_atomic64_dec(mtlk_atomic64_t* val);
static __INLINE void   mtlk_osal_atomic64_set(mtlk_atomic64_t* target, uint64 value);
static __INLINE uint64 mtlk_osal_atomic64_get(mtlk_atomic64_t* val);
static __INLINE uint64 mtlk_osal_atomic64_xchg(mtlk_atomic64_t* target, uint64 value);
/**********************************************************************/

/**********************************************************************
* String operations abstraction
***********************************************************************/
/*!
\fn      uint32 __MTLK_IFUNC mtlk_osal_str_atol(const char *s)
\brief   Function converts string to signed long

\param   val    Pointer to a string representation of long value

\return  signed long value
*/
int32 mtlk_osal_str_atol(const char *s);


/*!
\fn       void *mtlk_osal_str_memchr(const void *s, int c, size_t n)
\brief    Find a character in an area of memory.

\param    s The memory area.
\param    c The byte to search for.
\param    n The size of the area.

\return the address of the first occurrence of \a c, or NULL if \a c is not found
*/
void*  mtlk_osal_str_memchr(const void *s, int c, size_t n);


/*!
\fn       size_t mtlk_osal_strlcpy(char *dest, const char *src, size_t n)
\brief    function copies up to size-1 characters from the NUL-terminated string src to dst.
          Resulting string is always NUL-terminated.

\param    src The sources string.
\param    dst The destination buffer.
\param    n The size of the destination buffer.

\return the lentgh of src.
*/
size_t mtlk_osal_strlcpy(char *dest, const char *src, size_t n);


/*!
\fn       size_t mtlk_osal_strnlen(const char *src, size_t maxlen)
\brief    function returns the number of characters in the string src (excluding terminating NUL char),
          but not more then maxlen

\param    src The sources string.
\param    maxlen Then maximal value

\return the resulting number of characters
*/
size_t mtlk_osal_strnlen(const char *src, size_t maxlen);
/**********************************************************************/

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#define  SAFE_PLACE_TO_INCLUDE_OSAL_OSDEP_DEFS
#include "osal_osdep_defs.h"

#ifndef mtlk_osal_emergency_print
#error mtlk_osal_emergency_print(fmt, ...) must be defined by odep OSAL
#endif

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
#include "mtlk_osal_dbg.h"
#endif //CPTCFG_IWLWAV_ENABLE_OBJPOOL

#endif /* !__MTLK_OSAL_H__ */
