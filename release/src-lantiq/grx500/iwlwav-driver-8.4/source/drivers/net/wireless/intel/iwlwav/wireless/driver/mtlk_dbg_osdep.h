/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_DBG_OSDEP_H__
#define __MTLK_DBG_OSDEP_H__

#if defined (CONFIG_ARCH_STR9100) /* Star boards */

#include <linux/time.h>

extern u32 APB_clock;

/************************************************************
 * These definition are taken from STAR 9100 kernel:
 * arch/arm/mach-str9100/str9100_timer.c
 ************************************************************/
#define uSECS_PER_TICK       (1000000 / APB_clock)
#define TICKS_PER_uSEC       (APB_clock / 1000000)
/************************************************************/

void str9100_counter_on_init( void );
void str9100_counter_on_cleanup( void );

typedef uint32 mtlk_dbg_hres_ts_t;

#define MTLK_DBG_HRES_INIT() \
  str9100_counter_on_init(); \
  return MTLK_ERR_OK;

#define MTLK_DBG_HRES_CLEANUP() \
  str9100_counter_on_cleanup()

#define MTLK_DBG_HRES_TS(ts)                        \
  do {                                              \
    *(ts) = (mtlk_dbg_hres_ts_t)TIMER2_COUNTER_REG; \
  } while (0)

#define MTLK_DBG_HRES_DIFF_UINT64(ts_later, ts_earlier) \
  return ( ((ts_later) - (ts_earlier))/(TICKS_PER_uSEC) )

#define MTLK_DBG_HRES_US_TO_UINT64(us) \
  (((uint64)(us)) * TICKS_PER_uSEC)

#define MTLK_DBG_HRES_GET_UNIT_NAME() \
  return "us";

#else /* Using Linux default - do_gettimeofday() - precision is NOT GUARANTEED! */

#include <linux/time.h>

#define MTLK_DBG_HRES_INIT() \
  return MTLK_ERR_OK

#define MTLK_DBG_HRES_CLEANUP()

typedef struct timespec mtlk_dbg_hres_ts_t;

#define MTLK_DBG_HRES_TS(ts)                    \
  getnstimeofday(ts)

#define __MTLK_DBG_HRES_TS_TO_US(ts)            \
  ((((uint64)(ts)->tv_sec) * NSEC_PER_SEC) + (ts)->tv_nsec)

#define MTLK_DBG_HRES_DIFF_UINT64(ts_later, ts_earlier) \
  return (__MTLK_DBG_HRES_TS_TO_US(ts_later) - __MTLK_DBG_HRES_TS_TO_US(ts_earlier))

#define MTLK_DBG_HRES_US_TO_UINT64(us) \
  return (((uint64)(us)) * NSEC_PER_USEC)

#define MTLK_DBG_HRES_GET_UNIT_NAME() \
  return "ns";

#endif 

#endif /* __MTLK_DBG_OSDEP_H__ */

