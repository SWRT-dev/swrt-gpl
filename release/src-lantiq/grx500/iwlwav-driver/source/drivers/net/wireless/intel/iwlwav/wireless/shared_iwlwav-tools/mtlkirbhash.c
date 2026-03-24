/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "mtlkirbhash.h"
#include "mtlkhash.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_MTLKIRBAHASH
#define LOG_LOCAL_FID   1

MTLK_MHASH_DECLARE_ENTRY_T(irb, mtlk_guid_t);

struct mtlk_irb_evt_handler
{
  MTLK_MHASH_ENTRY_T(irb) hentry;
  mtlk_irb_hash_t        *hash;
  void                   *func;
  mtlk_handle_t           context;
  uint32                  owner;
};

MTLK_MHASH_DECLARE_STATIC(irb, mtlk_guid_t);

#define IRB_HASH_MAX_IDX 10

static __INLINE uint32
_irb_hash_hash_func (const mtlk_guid_t *key, uint32 nof_buckets)
{
  MTLK_UNREFERENCED_PARAM(nof_buckets);

  return key->data1 % IRB_HASH_MAX_IDX;
}

static __INLINE int
_irb_hash_key_cmp_func (const mtlk_guid_t *key1,
                        const mtlk_guid_t *key2)
{
  return mtlk_guid_compare(key1, key2);
}

MTLK_MHASH_DEFINE_STATIC(irb, 
                         mtlk_guid_t,
                         _irb_hash_hash_func,
                         _irb_hash_key_cmp_func);

static int
__mtlk_irb_hash_add_owner_node (mtlk_irb_hash_t       *irb_hash,
                                mtlk_handle_t          owner,
                                const mtlk_guid_t     *evt,
                                void                  *handler,
                                mtlk_handle_t          context)
{
  struct mtlk_irb_evt_handler *info =
      (struct mtlk_irb_evt_handler *)mtlk_osal_mem_alloc(sizeof(*info), MTLK_MEM_TAG_IRB);

  if (!info) {
    ELOG_V("Can't allocate IRB node!");
    return MTLK_ERR_NO_MEM;
  }

  memset(info, 0, sizeof(*info));
  info->func    = handler;
  info->context = context;
  info->owner   = owner;

  mtlk_osal_mutex_acquire(&irb_hash->lock);
  mtlk_mhash_insert_irb(&irb_hash->hash, evt, &info->hentry);
  mtlk_osal_mutex_release(&irb_hash->lock);

  return MTLK_ERR_OK;
}

static  __INLINE void
__mtlk_irb_hash_remove_all_nodes (mtlk_irb_hash_t *irb_hash)
{
  mtlk_mhash_enum_t        ctx;
  MTLK_MHASH_ENTRY_T(irb) *h;

  h = mtlk_mhash_enum_first_irb(&irb_hash->hash, &ctx);
  while (h) {
    struct mtlk_irb_evt_handler *info = 
      MTLK_CONTAINER_OF(h, struct mtlk_irb_evt_handler, hentry);
    mtlk_mhash_remove_irb(&irb_hash->hash, &info->hentry);
    mtlk_osal_mem_free(info);

    h = mtlk_mhash_enum_next_irb(&irb_hash->hash, &ctx);
  }
}

static  __INLINE BOOL
_mtlk_irb_remove_owner_nodes (mtlk_irb_hash_t *irb_hash, mtlk_handle_t owner)
{
  mtlk_mhash_enum_t        ctx;
  MTLK_MHASH_ENTRY_T(irb) *h;

  h = mtlk_mhash_enum_first_irb(&irb_hash->hash, &ctx);
  while (h) {
    struct mtlk_irb_evt_handler *info = 
      MTLK_CONTAINER_OF(h, struct mtlk_irb_evt_handler, hentry);
    if (info->owner == owner) {
      mtlk_mhash_remove_irb(&irb_hash->hash, &info->hentry);
      mtlk_osal_mem_free(info);
      break;
    }
    h = mtlk_mhash_enum_next_irb(&irb_hash->hash, &ctx);
  }

  return (h != NULL);
}

MTLK_INIT_STEPS_LIST_BEGIN(irb_hash)
  MTLK_INIT_STEPS_LIST_ENTRY(irb_hash, IRB_HASH_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(irb_hash, IRB_HASH_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(irb_hash, IRB_HASH_FILL)
MTLK_INIT_INNER_STEPS_BEGIN(irb_hash)
MTLK_INIT_STEPS_LIST_END(irb_hash);

static __INLINE int
__mtlk_irb_hash_nop_init (mtlk_irb_hash_t *irb_hash)
{
  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_irb_hash_init (mtlk_irb_hash_t *irb_hash)
{
  MTLK_INIT_TRY(irb_hash, MTLK_OBJ_PTR(irb_hash))
    MTLK_INIT_STEP(irb_hash, IRB_HASH_LOCK, MTLK_OBJ_PTR(irb_hash), mtlk_osal_mutex_init, (&irb_hash->lock));
    MTLK_INIT_STEP(irb_hash, IRB_HASH_HASH, MTLK_OBJ_PTR(irb_hash), mtlk_mhash_init_irb, (&irb_hash->hash, IRB_HASH_MAX_IDX));
    MTLK_INIT_STEP(irb_hash, IRB_HASH_FILL, MTLK_OBJ_PTR(irb_hash), __mtlk_irb_hash_nop_init, (irb_hash));
  MTLK_INIT_FINALLY(irb_hash, MTLK_OBJ_PTR(irb_hash))
    mtlk_osal_atomic_set(&irb_hash->owner_id, 0);
  MTLK_INIT_RETURN(irb_hash, MTLK_OBJ_PTR(irb_hash), _mtlk_irb_hash_cleanup, (irb_hash))
}

void __MTLK_IFUNC
_mtlk_irb_hash_cleanup (mtlk_irb_hash_t *irb_hash)
{
  MTLK_CLEANUP_BEGIN(irb_hash, MTLK_OBJ_PTR(irb_hash))
    MTLK_CLEANUP_STEP(irb_hash, IRB_HASH_FILL, MTLK_OBJ_PTR(irb_hash), __mtlk_irb_hash_remove_all_nodes, (irb_hash));
    MTLK_CLEANUP_STEP(irb_hash, IRB_HASH_HASH, MTLK_OBJ_PTR(irb_hash), mtlk_mhash_cleanup_irb, (&irb_hash->hash));
    MTLK_CLEANUP_STEP(irb_hash, IRB_HASH_LOCK, MTLK_OBJ_PTR(irb_hash), mtlk_osal_mutex_cleanup, (&irb_hash->lock));
  MTLK_CLEANUP_END(irb_hash, MTLK_OBJ_PTR(irb_hash))
}

mtlk_handle_t __MTLK_IFUNC
_mtlk_irb_hash_register (mtlk_irb_hash_t   *irb_hash,
                         const mtlk_guid_t *evts,
                         uint32             nof_evts,
                         void              *handler,
                         mtlk_handle_t      context)
{
  int    err   = MTLK_ERR_UNKNOWN;
  uint32 owner;

  MTLK_ASSERT(irb_hash != NULL);
  MTLK_ASSERT(evts != NULL);
  MTLK_ASSERT(nof_evts != 0);
  MTLK_ASSERT(handler != NULL);

  if (!irb_hash || !evts || !nof_evts || !handler || nof_evts > IRB_MAX_NOF_EVTS)
    return HANDLE_T(0);

  owner = mtlk_osal_atomic_inc(&irb_hash->owner_id);

  while (nof_evts) {
    err = __mtlk_irb_hash_add_owner_node(irb_hash,
                                         owner,
                                         &evts[0],
                                         handler,
                                         context);
    if (err != MTLK_ERR_OK) {
      goto end;
    }

    evts++;
    nof_evts--;
  }

  err = MTLK_ERR_OK;

end:
  if (err != MTLK_ERR_OK) {
    _mtlk_irb_hash_unregister(irb_hash, owner);
    owner = 0;
  }

  return HANDLE_T(owner);
}

void __MTLK_IFUNC
_mtlk_irb_hash_unregister (mtlk_irb_hash_t *irb_hash,
                           mtlk_handle_t    owner)
{
  MTLK_ASSERT(irb_hash);
  if (!irb_hash)
    return;

  if (owner) {
    mtlk_osal_mutex_acquire(&irb_hash->lock);
    _mtlk_irb_remove_owner_nodes(irb_hash, owner);
    mtlk_osal_mutex_release(&irb_hash->lock);
  }
}

void __MTLK_IFUNC
_mtlk_irb_hash_on_evt (mtlk_irb_hash_t   *irb_hash,
                       const mtlk_guid_t *evt,
                       void              *buffer,
                       uint32            *size,
                       mtlk_handle_t      contex)
{
  mtlk_mhash_find_t        ctx;
  MTLK_MHASH_ENTRY_T(irb) *h;

  MTLK_ASSERT(irb_hash);
  MTLK_ASSERT(evt);
  if (!irb_hash || !evt)
    return;

  mtlk_osal_mutex_acquire(&irb_hash->lock);
  h = mtlk_mhash_find_first_irb(&irb_hash->hash, evt, &ctx);
  while (h) {
    struct mtlk_irb_evt_handler *info = 
      MTLK_CONTAINER_OF(h, struct mtlk_irb_evt_handler, hentry);
    _mtlk_irb_call_handler(contex, info->func, info->context, evt, buffer, size);
    h = mtlk_mhash_find_next_irb(&irb_hash->hash, evt, &ctx);
  }
  mtlk_osal_mutex_release(&irb_hash->lock);
}


