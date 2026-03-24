/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_DBG_OSDEP_H__
#define __MTLK_DBG_OSDEP_H__


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


#endif /* __MTLK_DBG_OSDEP_H__ */

