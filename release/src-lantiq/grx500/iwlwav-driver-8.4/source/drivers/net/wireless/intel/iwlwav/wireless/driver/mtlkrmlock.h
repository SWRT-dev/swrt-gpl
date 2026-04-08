/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef _MTLK_RMLOCK_H_
#define _MTLK_RMLOCK_H_

/** 
*\file mtlkrmlock.h

*\brief Remove Lock API
*\brief Cross platform API to manipulate OSAL-based Remove Locks.

*\defgroup OSAL OSAL-based modules
*\{
*/

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

typedef struct
{
  mtlk_atomic_t     ref_cnt; /* Reference counter     */
  mtlk_osal_event_t evt;     /* Counter-is-zero event */
} __MTLK_IDATA mtlk_rmlock_t;

/*! 
\fn      static __INLINE int mtlk_rmlock_init(mtlk_rmlock_t *rmlock)
\brief   Initializes Remove Lock object

\param   rmlock        Pointer to Remove Lock object

\return  error code (MTLK_ERR_...)
*/
static __INLINE int
mtlk_rmlock_init (mtlk_rmlock_t *rmlock)
{
  mtlk_osal_atomic_set(&rmlock->ref_cnt, 0);
  return mtlk_osal_event_init(&rmlock->evt);
}

/*! 
\fn      static __INLINE void mtlk_rmlock_acquire_ex(mtlk_rmlock_t *rmlock, uint32 nof_reasons)
\brief   Acquires remove lock specified number of times

\param   rmlock        Pointer to Remove Lock object
\param   nof_reasons   Number of times to acquire
*/
static __INLINE void
mtlk_rmlock_acquire_ex (mtlk_rmlock_t *rmlock, uint32 nof_reasons)
{
  mtlk_osal_atomic_add(&rmlock->ref_cnt, nof_reasons);
}

/*! 
\fn      static __INLINE void mtlk_rmlock_release_ex(mtlk_rmlock_t *rmlock, uint32 nof_reasons)
\brief   Releases remove lock specified number of times

\param   rmlock        Pointer to Remove Lock object
\param   nof_reasons   Number of times to release
*/
static __INLINE void
mtlk_rmlock_release_ex (mtlk_rmlock_t *rmlock, uint32 nof_reasons)
{
  if (mtlk_osal_atomic_sub(&rmlock->ref_cnt, nof_reasons) == 0) {
    mtlk_osal_event_set(&rmlock->evt);
  }
}

/*! 
\fn      void mtlk_rmlock_acquire(mtlk_rmlock_t *rmlock)
\brief   Acquires remove lock

\param   rmlock        Pointer to Remove Lock object
*/
#define mtlk_rmlock_acquire(rmlock) mtlk_rmlock_acquire_ex((rmlock), 1)

/*! 
\fn      void mtlk_rmlock_release(mtlk_rmlock_t *rmlock)
\brief   Releases remove lock

\param   rmlock        Pointer to Remove Lock object
*/
#define mtlk_rmlock_release(rmlock) mtlk_rmlock_release_ex((rmlock), 1)

/*! 
\fn      static __INLINE int mtlk_rmlock_wait(mtlk_rmlock_t *rmlock)
\brief   Waits for Remove Lock to be fully released

\param   rmlock        Pointer to Remove Lock object

\return  error code (MTLK_ERR_...)
*/
static __INLINE int
mtlk_rmlock_wait (mtlk_rmlock_t *rmlock)
{
  return mtlk_osal_event_wait(&rmlock->evt,
                              MTLK_OSAL_EVENT_INFINITE);
}

/*! 
\fn      static __INLINE void mtlk_rmlock_reset(mtlk_rmlock_t *rmlock)
\brief   Resets remove lock event

\param   rmlock        Pointer to Remove Lock object
*/
static __INLINE void
mtlk_rmlock_reset (mtlk_rmlock_t *rmlock)
{
  mtlk_osal_event_reset(&rmlock->evt);
}

/*! 
\fn      static __INLINE void mtlk_rmlock_cleanup(mtlk_rmlock_t *rmlock)
\brief   Cleans up remove lock event

\param   rmlock        Pointer to Remove Lock object
*/
static __INLINE void
mtlk_rmlock_cleanup (mtlk_rmlock_t *rmlock)
{
  mtlk_osal_event_cleanup(&rmlock->evt);
}

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

/*\}*/

#endif /* !_MTLK_RMLOCK_H_ */
