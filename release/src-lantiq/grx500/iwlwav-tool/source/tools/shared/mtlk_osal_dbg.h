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
* $Id: mtlk_osal_dbg.h 7337 2009-07-06 10:46:15Z antonn $
*
* 
*
*/

#ifndef __MTLK_OSAL_DBG_H__
#define __MTLK_OSAL_DBG_H__

#define LOG_LOCAL_GID   GID_OSAL
#define LOG_LOCAL_FID   4

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
  return _malloc_tag(size, tag, caller_slid);
}

static __INLINE void mtlk_osal_mem_free_debug(void* buffer)
{
  /* All the OBJPOOL-related functionality is already built in to _kfree_tag() */
  _free_tag(buffer);
}

#define mtlk_osal_mem_alloc(size, tag)                  \
  mtlk_osal_mem_alloc_debug(size, tag, MTLK_SLID)

#define mtlk_osal_mem_free_(buffer)                    \
  mtlk_osal_mem_free_debug(buffer)

static __INLINE int mtlk_osal_lock_init_debug(mtlk_osal_spinlock_t* spinlock,
                                              mtlk_slid_t caller_slid)
{
  int res = mtlk_osal_lock_init(spinlock);
  if(MTLK_ERR_OK == res) {
    mtlk_objpool_add_object(&g_objpool, &((spinlock)->objpool_ctx), MTLK_SPINLOCK_OBJ, caller_slid);
  }

  return res;
}

static __INLINE void mtlk_osal_lock_cleanup_debug(mtlk_osal_spinlock_t* spinlock)
{
  mtlk_objpool_remove_object(&g_objpool, &((spinlock)->objpool_ctx), MTLK_SPINLOCK_OBJ);
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

static __INLINE int mtlk_osal_thread_init_debug(mtlk_osal_thread_t *thread,
                                                mtlk_slid_t caller_slid)
{
  int res = mtlk_osal_thread_init(thread);
  if(MTLK_ERR_OK == res) {
    mtlk_objpool_add_object(&g_objpool, &((thread)->objpool_ctx), MTLK_THREAD_OBJ, caller_slid);
  }

  return res;
}

static __INLINE void mtlk_osal_thread_cleanup_debug(mtlk_osal_thread_t *thread)
{
  mtlk_objpool_remove_object(&g_objpool, &((thread)->objpool_ctx), MTLK_THREAD_OBJ);
  mtlk_osal_thread_cleanup(thread);
}

#define mtlk_osal_thread_init(thread) \
  mtlk_osal_thread_init_debug(thread, MTLK_SLID)

#define mtlk_osal_thread_cleanup(thread) \
  mtlk_osal_thread_cleanup_debug(thread)

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID


#endif /* !__MTLK_OSAL_DBG_H__ */
