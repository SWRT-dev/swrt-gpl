/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_reflim_H__
#define __MTLK_reflim_H__

#include "mtlk_osal.h"

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_REFLIM
#define LOG_LOCAL_FID   0

typedef struct  
{
  uint32        max;
  mtlk_atomic_t cur;
} mtlk_reflim_t;

static __INLINE void
mtlk_reflim_init (mtlk_reflim_t *reflim, uint32 max)
{
  MTLK_ASSERT(reflim != NULL);

  mtlk_osal_atomic_set(&reflim->cur, 0);
  reflim->max = max;
}

static __INLINE void
mtlk_reflim_cleanup (mtlk_reflim_t *reflim)
{
  MTLK_ASSERT(reflim != NULL);
  MTLK_ASSERT(mtlk_osal_atomic_get(&reflim->cur) == 0);
  memset(&reflim, 0, sizeof(reflim));
}

static __INLINE BOOL
mtlk_reflim_try_ref (mtlk_reflim_t *reflim)
{
  uint32 cur;

  MTLK_ASSERT(reflim != NULL);

  cur = mtlk_osal_atomic_inc(&reflim->cur);
  if (cur <= reflim->max) {
    return TRUE;
  }

  mtlk_osal_atomic_dec(&reflim->cur);
  return FALSE;
}

static __INLINE void
mtlk_reflim_unref (mtlk_reflim_t *reflim)
{
  uint32 cur;

  MTLK_ASSERT(reflim != NULL);

  cur = mtlk_osal_atomic_dec(&reflim->cur);

  MTLK_ASSERT(cur < reflim->max);
  MTLK_UNREFERENCED_PARAM(cur); /* For release compilation */
}

static __INLINE uint32
mtlk_reflim_get_max (mtlk_reflim_t *reflim)
{
  MTLK_ASSERT(reflim != NULL);

  return reflim->max;
}

static __INLINE uint32
mtlk_reflim_get_cur (mtlk_reflim_t *reflim)
{
  MTLK_ASSERT(reflim != NULL);

  return mtlk_osal_atomic_get(&reflim->cur);
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /*__MTLK_reflim_H__*/
