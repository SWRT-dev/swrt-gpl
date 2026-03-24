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

#ifndef __MTLK_OSAL_DBG_H__
#define __MTLK_OSAL_DBG_H__

#define LOG_LOCAL_GID   GID_OSAL
#define LOG_LOCAL_FID   2

MTLK_DECLARE_OBJPOOL(g_objpool);

static __INLINE void* mtlk_osal_mem_alloc_objpool (uint32 size, uint32 tag)
{
  return malloc_objpool(size, tag);
}

static __INLINE void mtlk_osal_mem_free_objpool (void* buffer)
{
  free_objpool(buffer);
}

static __INLINE void* mtlk_osal_mem_alloc_debug (uint32 size, uint32 tag,
                                                 mtlk_slid_t caller_slid)
{
  MTLK_ASSERT(0 != size);
  /* All the OBJPOOL-related functionality is already built in to _kmalloc_tag() */
  return _kmalloc_tag(size, GFP_ATOMIC, tag, caller_slid);
}

static __INLINE void mtlk_osal_mem_free_debug(void* buffer)
{
  /* All the OBJPOOL-related functionality is already built in to _kfree_tag() */
  _kfree_tag(buffer);
}

#define mtlk_osal_mem_alloc(size, tag)                  \
  mtlk_osal_mem_alloc_debug(size, tag, MTLK_SLID)

#define mtlk_osal_mem_free_(buffer)                    \
  mtlk_osal_mem_free_debug(buffer)

static __INLINE int mtlk_osal_timer_init_debug(mtlk_osal_timer_t *timer,
                                               mtlk_osal_timer_f  clb,
                                               mtlk_handle_t      clb_usr_data,
                                               mtlk_slid_t        caller_slid) {
  int res = mtlk_osal_timer_init(timer, clb, clb_usr_data);
  if(MTLK_ERR_OK == res) {
    mtlk_objpool_add_object(&g_objpool, &((timer)->objpool_ctx), MTLK_TIMER_OBJ, caller_slid);
  }

  return res;
}

static __INLINE void mtlk_osal_timer_cleanup_debug(mtlk_osal_timer_t *timer)
{
  mtlk_objpool_remove_object(&g_objpool, &((timer)->objpool_ctx), MTLK_TIMER_OBJ);
  mtlk_osal_timer_cleanup(timer);
}

#define mtlk_osal_timer_init(timer, clb, clb_user_data) \
  mtlk_osal_timer_init_debug(timer, clb, clb_user_data, MTLK_SLID)

#define mtlk_osal_timer_cleanup(timer) \
  mtlk_osal_timer_cleanup_debug(timer)

static __INLINE int mtlk_osal_lock_init_debug(mtlk_osal_spinlock_t* spinlock, 
                                              mtlk_slid_t caller_slid)
{
  int res = mtlk_osal_lock_init(spinlock);

  return res;
}

static __INLINE void mtlk_osal_lock_cleanup_debug(mtlk_osal_spinlock_t* spinlock)
{
  mtlk_osal_lock_cleanup(spinlock);
}

/* Special internal functions for ObjPool usage, */ 
/* to avoid chicken and egg problem.             */
static __INLINE int mtlk_osal_lock_init_objpool(mtlk_osal_spinlock_t* spinlock)
{
  return mtlk_osal_lock_init(spinlock);
}

static __INLINE void mtlk_osal_lock_cleanup_objpool(mtlk_osal_spinlock_t* spinlock)
{
  mtlk_osal_lock_cleanup(spinlock);
}

#define mtlk_osal_lock_init(spinlock) \
  mtlk_osal_lock_init_debug(spinlock, MTLK_SLID)

#define mtlk_osal_lock_cleanup(spinlock) \
  mtlk_osal_lock_cleanup_debug(spinlock)

static __INLINE int mtlk_osal_event_init_debug(mtlk_osal_event_t* event, 
                                               mtlk_slid_t caller_slid)
{
  int res = mtlk_osal_event_init(event);
  if(MTLK_ERR_OK == res) {
    mtlk_objpool_add_object(&g_objpool, &((event)->objpool_ctx), MTLK_EVENT_OBJ, caller_slid);
  }

  return res;
}

static __INLINE void mtlk_osal_event_cleanup_debug(mtlk_osal_event_t* event)
{
  mtlk_objpool_remove_object(&g_objpool, &((event)->objpool_ctx), MTLK_EVENT_OBJ);
  mtlk_osal_event_cleanup(event);
}

#define mtlk_osal_event_init(spinlock) \
  mtlk_osal_event_init_debug(spinlock, MTLK_SLID)

#define mtlk_osal_event_cleanup(spinlock) \
  mtlk_osal_event_cleanup_debug(spinlock)

static __INLINE int mtlk_osal_mutex_init_debug(mtlk_osal_mutex_t* mutex,
                                               mtlk_slid_t caller_slid)
{
  int res = mtlk_osal_mutex_init(mutex);
  if(MTLK_ERR_OK == res) {
    mtlk_objpool_add_object(&g_objpool, &((mutex)->objpool_ctx), MTLK_MUTEX_OBJ, caller_slid);
  }

  return res;
}

static __INLINE void mtlk_osal_mutex_cleanup_debug(mtlk_osal_mutex_t* mutex)
{
  mtlk_objpool_remove_object(&g_objpool, &((mutex)->objpool_ctx), MTLK_MUTEX_OBJ);
  mtlk_osal_mutex_cleanup(mutex);
}

#define mtlk_osal_mutex_init(mutex) \
  mtlk_osal_mutex_init_debug(mutex, MTLK_SLID)

#define mtlk_osal_mutex_cleanup(mutex) \
  mtlk_osal_mutex_cleanup_debug(mutex)

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* !__MTLK_OSAL_DBG_H__ */
