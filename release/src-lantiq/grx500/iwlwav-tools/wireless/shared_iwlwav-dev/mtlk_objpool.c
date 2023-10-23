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
* Written by: Dmitry Fleytman
*
*/

#include "mtlkinc.h"
#include "mtlk_osal.h"
#include "mtlkhash.h"

#define LOG_LOCAL_GID   GID_OBJPOOL
#define LOG_LOCAL_FID   1

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

struct _mtlk_objpool_memory_t
{
  struct _mtlk_objpool_memory_alarm_t alarm;
  uint32                              allocated;
};

struct _mtlk_objpool_t
{
  mtlk_osal_spinlock_t          hash_lock;
  mtlk_hash_t                   objects_hash;
  struct _mtlk_objpool_memory_t memory;

  MTLK_DECLARE_INIT_STATUS;
};

#define _MTLK_OBJPOOL_HASH_NOF_BUCKETS  (32)
#define _MTLK_OBJPOOL_HASH_PRIME_FACTOR (31)

typedef uint64 _mtlk_objpool_hash_key_t;
#define _MTLK_MAKE_OBJPOOL_HASH_KEY(objtype, slid) ( (((uint64)(objtype)) << 32) | slid )

static __INLINE uint32
_mtlk_hash_objpool_hashval (const _mtlk_objpool_hash_key_t *key, uint32 nof_buckets)
{
  /* Key shortened to 32 bits intentionally because 64 bits modulo      */
  /* is not supported by all platforms. When it comes to hash balancing */
  /* this does not make any difference.                                 */
  return (((uint32)(*key)) * _MTLK_OBJPOOL_HASH_PRIME_FACTOR) % nof_buckets;
}

static __INLINE int
_mtlk_hash_objpool_keycmp (const _mtlk_objpool_hash_key_t *key1, const _mtlk_objpool_hash_key_t *key2)
{
  return *key1 != *key2;
}

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

MTLK_HASH_DECLARE_ENTRY_T(objpool, _mtlk_objpool_hash_key_t);

MTLK_HASH_DECLARE_EXTERN(objpool, _mtlk_objpool_hash_key_t);
MTLK_HASH_DEFINE_EXTERN(objpool, _mtlk_objpool_hash_key_t,
                        _mtlk_hash_objpool_hashval,
                        _mtlk_hash_objpool_keycmp);
MTLK_HASH_DEFINE_OBJPOOL_EXTENSIONS_EXTERN(objpool);

MTLK_INIT_STEPS_LIST_BEGIN(objpool)
  MTLK_INIT_STEPS_LIST_ENTRY(objpool, HASH_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(objpool, OBJPOOL_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(objpool, OBJPOOL_DUMP)
MTLK_INIT_INNER_STEPS_BEGIN(objpool)
MTLK_INIT_STEPS_LIST_END(objpool);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

typedef struct __mtlk_obj_factory_t
{
  MTLK_HASH_ENTRY_T(objpool) hentry;
  mtlk_objtypes_t            type;
  mtlk_slid_t                creator_slid;
  uint32                     objects_number;
  uint32                     total_size;
} _mtlk_obj_factory_t;

void __MTLK_IFUNC mtlk_objpool_cleanup(mtlk_objpool_t* objpool)
{
  MTLK_ASSERT(NULL != objpool);

  MTLK_CLEANUP_BEGIN(objpool, MTLK_OBJ_PTR(objpool))
    MTLK_CLEANUP_STEP(objpool, OBJPOOL_DUMP, MTLK_OBJ_PTR(objpool),
                      mtlk_objpool_dump, (objpool));
    MTLK_CLEANUP_STEP(objpool, OBJPOOL_HASH, MTLK_OBJ_PTR(objpool),
                      mtlk_hash_cleanup_objpool_objpool, (&objpool->objects_hash));
    MTLK_CLEANUP_STEP(objpool, HASH_LOCK, MTLK_OBJ_PTR(objpool),
                      mtlk_osal_lock_cleanup_objpool, (&objpool->hash_lock));
  MTLK_CLEANUP_END(objpool, MTLK_OBJ_PTR(objpool));
}

int __MTLK_IFUNC mtlk_objpool_init(mtlk_objpool_t* objpool)
{
  MTLK_ASSERT(NULL != objpool);

  MTLK_INIT_TRY(objpool, MTLK_OBJ_PTR(objpool))
    MTLK_INIT_STEP(objpool, HASH_LOCK, MTLK_OBJ_PTR(objpool),
                   mtlk_osal_lock_init_objpool, (&objpool->hash_lock));
    MTLK_INIT_STEP(objpool, OBJPOOL_HASH, MTLK_OBJ_PTR(objpool),
                   mtlk_hash_init_objpool_objpool,
                   (&objpool->objects_hash, _MTLK_OBJPOOL_HASH_NOF_BUCKETS));
    MTLK_INIT_STEP_VOID(objpool, OBJPOOL_DUMP, MTLK_OBJ_PTR(objpool),
                        MTLK_NOACTION, ());
    memset(&objpool->memory, 0, sizeof(objpool->memory));
  MTLK_INIT_FINALLY(objpool, MTLK_OBJ_PTR(objpool))
  MTLK_INIT_RETURN(objpool, MTLK_OBJ_PTR(objpool), mtlk_objpool_cleanup, (objpool))
}

const char * __MTLK_IFUNC
mtlk_objpool_get_type_name (mtlk_objtypes_t object_type)
{
  switch(object_type) {
  case MTLK_MEMORY_OBJ:
    return "Memory";
  case MTLK_TIMER_OBJ:
    return "Timer";
  case MTLK_SPINLOCK_OBJ:
    return "Spinlock";
  case MTLK_EVENT_OBJ:
    return "Event";
  case MTLK_MUTEX_OBJ:
    return "Mutex";
  case MTLK_SLIST_OBJ:
    return "Slist";
  case MTLK_LSLIST_OBJ:
    return "Lslist";
  case MTLK_DLIST_OBJ:
    return "Dlist";
  case MTLK_LDLIST_OBJ:
    return "Ldlist";
  case MTLK_THREAD_OBJ:
    return "Thread";
  case MTLK_TXMM_OBJ:
    return "TXMM slot";
#ifdef CPTCFG_IWLWAV_ENABLE_NBUF_OBJPOOL
  case MTLK_NET_BUF_OBJ:
    return "NetBuf";
#endif
  case MTLK_WSS_OBJ:
    return "WSS";
  default:
    mtlk_osal_emergency_print("Unknown object type %d", object_type);
    MTLK_ASSERT(FALSE);
    return "Unknown";
  }
}

void __MTLK_IFUNC mtlk_objpool_dump(mtlk_objpool_t* objpool)
{
  mtlk_hash_enum_t e;
  uint32 obj_counter = 0;
  MTLK_HASH_ENTRY_T(objpool) *h;

  MTLK_ASSERT(NULL != objpool);

  mtlk_osal_lock_acquire(&objpool->hash_lock);

  h = mtlk_hash_enum_first_objpool(&objpool->objects_hash, &e);
  while (h) {
    _mtlk_obj_factory_t *objfact = MTLK_CONTAINER_OF(h, _mtlk_obj_factory_t, hentry);

    mtlk_osal_emergency_print("objpool: %d objects of type \"%s\", created at " MTLK_SLID_FMT,
                              objfact->objects_number,
                              mtlk_objpool_get_type_name(objfact->type),
                              MTLK_SLID_ARGS(objfact->creator_slid));
    obj_counter += objfact->objects_number;

    h = mtlk_hash_enum_next_objpool(&objpool->objects_hash, &e);
  }

  mtlk_osal_lock_release(&objpool->hash_lock);

  if (0 != obj_counter) {
    mtlk_osal_emergency_print("objpool: %d object(s) are still in object pool.", obj_counter);
  }
}

static void
_mtlk_objpool_on_memory_allocated(struct _mtlk_objpool_memory_t* memory_stats, uint32 size)
{
  memory_stats->allocated += size;
  if (memory_stats->alarm.limit &&
    memory_stats->allocated >= memory_stats->alarm.limit)
  {
      memory_stats->alarm.clb(memory_stats->alarm.usr_ctx, memory_stats->allocated);
  }
};

static void
_mtlk_objpool_on_memory_released(struct _mtlk_objpool_memory_t* memory_stats, uint32 size)
{
  memory_stats->allocated -= size;
};

void __MTLK_IFUNC
mtlk_objpool_add_object_ex(mtlk_objpool_t* objpool, 
                           mtlk_objpool_context_t* objpool_ctx_ptr,
                           mtlk_objtypes_t object_type,
                           mtlk_slid_t creator_slid,
                           uint32 additional_allocation_size)
{
  MTLK_HASH_ENTRY_T(objpool) *entry;
  _mtlk_objpool_hash_key_t   new_obj_hash_key;

  MTLK_ASSERT(NULL != objpool);
  MTLK_ASSERT(NULL != objpool_ctx_ptr);

  MTLK_ASSERT(object_type > MTLK_OBJTYPES_START);
  MTLK_ASSERT(object_type < MTLK_OBJTYPES_END);

  new_obj_hash_key = _MTLK_MAKE_OBJPOOL_HASH_KEY(object_type, creator_slid);
  *objpool_ctx_ptr = creator_slid;

  mtlk_osal_lock_acquire(&objpool->hash_lock);

  entry = mtlk_hash_find_objpool(&objpool->objects_hash, &new_obj_hash_key);
  if(entry)
  {
    _mtlk_obj_factory_t *objfact = MTLK_CONTAINER_OF(entry, _mtlk_obj_factory_t, hentry);
    objfact->objects_number++;
    objfact->total_size += additional_allocation_size;
  }
  else
  {
    /* Allocate new object allocator context */
    _mtlk_obj_factory_t* objfact = (_mtlk_obj_factory_t*) mtlk_osal_mem_alloc_objpool(sizeof(_mtlk_obj_factory_t), MTLK_MEM_TAG_OBJPOOL);
    if(NULL != objfact)
    {
      /* Fill it */
      objfact->type = object_type;
      objfact->creator_slid = creator_slid;
      objfact->objects_number = 1;
      objfact->total_size = additional_allocation_size;

      /* Put to hash */
      entry = mtlk_hash_insert_objpool(&objpool->objects_hash, &new_obj_hash_key, &objfact->hentry);
    }
    else
    {
      mtlk_osal_emergency_print("Failed to allocate object header.");
    }
  }

  mtlk_osal_lock_release(&objpool->hash_lock);

  if(object_type == MTLK_MEMORY_OBJ)
  {
    _mtlk_objpool_on_memory_allocated(&objpool->memory, additional_allocation_size);
  }
}

void __MTLK_IFUNC mtlk_objpool_remove_object_ex(mtlk_objpool_t* objpool,
                                                mtlk_objpool_context_t* objpool_ctx_ptr,
                                                mtlk_objtypes_t object_type,
                                                uint32 additional_allocation_size)
{
  MTLK_HASH_ENTRY_T(objpool)  *entry;
  _mtlk_objpool_hash_key_t   obj_hash_key;
  _mtlk_obj_factory_t  *objfact;
  MTLK_ASSERT(NULL != objpool);
  MTLK_ASSERT(NULL != objpool_ctx_ptr);
  MTLK_ASSERT(0 != *objpool_ctx_ptr);

  obj_hash_key = _MTLK_MAKE_OBJPOOL_HASH_KEY(object_type, *objpool_ctx_ptr);

  if(object_type == MTLK_MEMORY_OBJ)
    _mtlk_objpool_on_memory_released(&objpool->memory, additional_allocation_size);

  mtlk_osal_lock_acquire(&objpool->hash_lock);

  entry = mtlk_hash_find_objpool(&objpool->objects_hash, &obj_hash_key);
  MTLK_ASSERT(NULL != entry);
  if(!entry) {
    mtlk_osal_lock_release(&objpool->hash_lock);
    ELOG_DD("Can't find entry for object %u:0x%X", object_type, *objpool_ctx_ptr);
    return;
  }

  objfact = MTLK_CONTAINER_OF(entry, _mtlk_obj_factory_t, hentry);
  objfact->objects_number--;
  MTLK_ASSERT((int32)objfact->objects_number >= 0);
  objfact->total_size -= additional_allocation_size;
  MTLK_ASSERT((int32)objfact->total_size >= 0);

  if(0 == objfact->objects_number)
  {
    MTLK_ASSERT(0 == objfact->total_size);
    mtlk_hash_remove_objpool(&objpool->objects_hash, entry);
    mtlk_osal_mem_free_objpool(objfact);
  }

  mtlk_osal_lock_release(&objpool->hash_lock);
}

mtlk_slid_t __MTLK_IFUNC
mtlk_objpool_get_creator_slid (mtlk_objpool_t* objpool,
                               mtlk_objpool_context_t* objpool_ctx_ptr)
{
  MTLK_ASSERT(NULL != objpool);
  MTLK_ASSERT(NULL != objpool_ctx_ptr);
  MTLK_ASSERT(0 != *objpool_ctx_ptr);

  MTLK_UNREFERENCED_PARAM(objpool);

  return (mtlk_slid_t) *objpool_ctx_ptr;
}

void __MTLK_IFUNC
mtlk_objpool_enum_by_type (mtlk_objpool_t* objpool,
                           mtlk_objtypes_t object_type,
                           mtlk_objpool_enum_f clb,
                           mtlk_handle_t context)
{
  MTLK_HASH_ENTRY_T(objpool) *h;
  mtlk_hash_enum_t e;

  MTLK_ASSERT(NULL != objpool);
  MTLK_ASSERT(NULL != clb);

  mtlk_osal_lock_acquire(&objpool->hash_lock);

  h = mtlk_hash_enum_first_objpool(&objpool->objects_hash, &e);
  while (h) {
    _mtlk_obj_factory_t *objfact = MTLK_CONTAINER_OF(h, _mtlk_obj_factory_t, hentry);

    if (object_type == objfact->type) {
      clb(objpool, objfact->creator_slid, objfact->objects_number, objfact->total_size, context);
    }

    h = mtlk_hash_enum_next_objpool(&objpool->objects_hash, &e);
  }

  mtlk_osal_lock_release(&objpool->hash_lock);
}

uint32 __MTLK_IFUNC
mtlk_objpool_get_memory_allocated (mtlk_objpool_t* objpool)
{
  return objpool->memory.allocated;
}

void __MTLK_IFUNC
mtlk_objpool_set_memory_alarm (mtlk_objpool_t* objpool, 
                               const mtlk_objpool_memory_alarm_t *alarm_info)
{
  mtlk_osal_lock_acquire(&objpool->hash_lock);
  if (alarm_info) {
    objpool->memory.alarm = *alarm_info;
  }
  else {
    memset(&objpool->memory.alarm, 0, sizeof(objpool->memory.alarm));
  }
  mtlk_osal_lock_release(&objpool->hash_lock);
}

mtlk_objpool_t g_objpool;

#endif /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */
