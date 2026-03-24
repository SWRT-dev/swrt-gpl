/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_ASSERT_H__
#define __MTLK_ASSERT_H__

#include "mtlk_slid.h"

/******************************************************************************/
/*! \file       mtlk_assert.h
 *  \brief      Assert functionality interface
 */

/******************************************************************************/
/*! \brief      Invoke Linux kernel BUG() function on the
 *              occasion that unlikely should happen
 *
 *  \param[in]  slid     An MTLK_SLID bitfield containing OID, GID, FID and LID of a string
 *                       where __mtlk_assert was called from.
 *
 *  \return     void
 */

void __MTLK_IFUNC __mtlk_assert(mtlk_slid_t slid);

/* During the Klocwork analysis ASSERT's should be disabled,
 * otherwise analyzer may not detect problems hidden by ASSERT call
 */

#if defined(CPTCFG_IWLWAV_DEBUG) && !defined(__KLOCWORK__)
#define __MTLK_ASSERT(expr, slid)  \
  do {                             \
    if (__UNLIKELY(!(expr))) {     \
      __mtlk_assert(slid);         \
    }                              \
  } while (0)

#else
#define __MTLK_ASSERT(expr, slid)
#endif

#define ASSERT(expr)      __MTLK_ASSERT(expr, MTLK_SLID)
#define MTLK_ASSERT(expr) __MTLK_ASSERT(expr, MTLK_SLID)

void __MTLK_IFUNC mtlk_assert_log_s(mtlk_slid_t caller_slid, const char *msg);
void __MTLK_IFUNC mtlk_assert_log_ss(mtlk_slid_t caller_slid, const char *msg1, const char *msg2);

#endif /* __MTLK_ASSERT_H__ */

