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
 * $$
 *
 *
 *
 * Core abilities manager
 *
 * Originally written by Andrii Tseglytskyi
 *
 */

#include "mtlkinc.h"
#include "mtlk_ab_manager.h"
#include "mtlk_coreui.h"

#define LOG_LOCAL_GID   GID_ABMANAGER
#define LOG_LOCAL_FID   1

/** 
*\file mtlk_abmgr.c

*\brief Manager of Core "abilities", which are exported to DF

*/

struct _mtlk_abmgr_t {
  mtlk_hash_t          ability_states_hash;
  mtlk_osal_spinlock_t hash_lock;

  MTLK_DECLARE_INIT_STATUS;
  MTLK_DECLARE_INIT_LOOP(STATES_INIT);
};

#define _MTLK_ABMGR_HASH_NOF_BUCKETS  (32)
#define _MTLK_ABMGR_HASH_PRIME_FACTOR (31)

static __INLINE uint32
_mtlk_hash_abmgr_hashval (const mtlk_ability_id_t *key, uint32 nof_buckets)
{
  return (*key * _MTLK_ABMGR_HASH_PRIME_FACTOR) % nof_buckets;
}

static __INLINE int
_mtlk_hash_abmgr_keycmp (const mtlk_ability_id_t *key1, const mtlk_ability_id_t *key2)
{
  return *key1 != *key2;
}

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

MTLK_HASH_DECLARE_ENTRY_T(abmgr, mtlk_ability_id_t);

MTLK_HASH_DECLARE_EXTERN(abmgr, mtlk_ability_id_t);
MTLK_HASH_DEFINE_EXTERN(abmgr, mtlk_ability_id_t,
                        _mtlk_hash_abmgr_hashval,
                        _mtlk_hash_abmgr_keycmp);

MTLK_INIT_STEPS_LIST_BEGIN(abmgr)
  MTLK_INIT_STEPS_LIST_ENTRY(abmgr, HASH_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(abmgr, ABMGR_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(abmgr, REG_AB_NONE)
  MTLK_INIT_STEPS_LIST_ENTRY(abmgr, EN_AB_NONE)
MTLK_INIT_INNER_STEPS_BEGIN(abmgr)
MTLK_INIT_STEPS_LIST_END(abmgr);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

typedef struct __mtlk_ab_record_t
{
  MTLK_HASH_ENTRY_T(abmgr) hentry;
  BOOL                     is_enabled;
} _mtlk_ab_record_t;

static void
_mtlk_abmgr_cleanup(mtlk_abmgr_t *abmgr)
{
  MTLK_ASSERT(NULL != abmgr);

  MTLK_CLEANUP_BEGIN(abmgr, MTLK_OBJ_PTR(abmgr))
    MTLK_CLEANUP_STEP(abmgr, EN_AB_NONE, MTLK_OBJ_PTR(abmgr),
                      mtlk_abmgr_disable_ability, (abmgr, MTLK_ABILITY_NONE));
    MTLK_CLEANUP_STEP(abmgr, REG_AB_NONE, MTLK_OBJ_PTR(abmgr),
                      mtlk_abmgr_unregister_ability, (abmgr, MTLK_ABILITY_NONE));
    MTLK_CLEANUP_STEP(abmgr, ABMGR_HASH, MTLK_OBJ_PTR(abmgr), 
                      mtlk_hash_cleanup_abmgr, (&abmgr->ability_states_hash));
    MTLK_CLEANUP_STEP(abmgr, HASH_LOCK, MTLK_OBJ_PTR(abmgr), 
                      mtlk_osal_lock_cleanup, (&abmgr->hash_lock));
  MTLK_CLEANUP_END(abmgr, MTLK_OBJ_PTR(abmgr));
}

static int
_mtlk_abmgr_init(mtlk_abmgr_t *abmgr)
{
  MTLK_ASSERT(NULL != abmgr);

  MTLK_INIT_TRY(abmgr, MTLK_OBJ_PTR(abmgr))
    MTLK_INIT_STEP(abmgr, HASH_LOCK, MTLK_OBJ_PTR(abmgr),
                   mtlk_osal_lock_init, (&abmgr->hash_lock));
    MTLK_INIT_STEP(abmgr, ABMGR_HASH, MTLK_OBJ_PTR(abmgr),
                   mtlk_hash_init_abmgr, (&abmgr->ability_states_hash, _MTLK_ABMGR_HASH_NOF_BUCKETS));
    MTLK_INIT_STEP(abmgr, REG_AB_NONE, MTLK_OBJ_PTR(abmgr),
                   mtlk_abmgr_register_ability, (abmgr, MTLK_ABILITY_NONE));
    MTLK_INIT_STEP_VOID(abmgr, EN_AB_NONE, MTLK_OBJ_PTR(abmgr),
                        mtlk_abmgr_enable_ability, (abmgr, MTLK_ABILITY_NONE));
  MTLK_INIT_FINALLY(abmgr, MTLK_OBJ_PTR(abmgr))
  MTLK_INIT_RETURN(abmgr, MTLK_OBJ_PTR(abmgr), _mtlk_abmgr_cleanup, (abmgr))
}

int __MTLK_IFUNC
mtlk_abmgr_register_ability(mtlk_abmgr_t *abmgr,
                                 mtlk_ability_id_t ab_id)
{
  MTLK_HASH_ENTRY_T(abmgr) *entry;
  _mtlk_ab_record_t *ab_record;

  MTLK_ASSERT(NULL != abmgr);

  ab_record = mtlk_osal_mem_alloc(sizeof(_mtlk_ab_record_t), MEM_TAG_ABILITY_RECORD);
  if(NULL == ab_record)
  {
    return MTLK_ERR_NO_MEM;
  }

  memset(ab_record, 0, sizeof(*ab_record));
  ab_record->is_enabled = FALSE;

  mtlk_osal_lock_acquire(&abmgr->hash_lock);
  entry = mtlk_hash_insert_abmgr(&abmgr->ability_states_hash, &ab_id, &ab_record->hentry);
  mtlk_osal_lock_release(&abmgr->hash_lock);

  if(NULL != entry)
  {
    ELOG_D("Multiple registration attempt for ability 0x%X", ab_id);
    MTLK_ASSERT(!"Ability already registered");
  }

  return MTLK_ERR_OK;
}
#ifdef UNUSED_CODE
BOOL __MTLK_IFUNC
_mtlk_abmgr_is_ability_registered(mtlk_abmgr_t *abmgr,
                                       mtlk_ability_id_t ab_id)
{
  MTLK_HASH_ENTRY_T(abmgr) *entry;
  MTLK_ASSERT(NULL != abmgr);

  mtlk_osal_lock_acquire(&abmgr->hash_lock);
  entry = mtlk_hash_find_abmgr(&abmgr->ability_states_hash, &ab_id);
  mtlk_osal_lock_release(&abmgr->hash_lock);

  return (NULL != entry) ? TRUE : FALSE;
}
#endif
BOOL __MTLK_IFUNC
_mtlk_abmgr_is_ability_enabled(mtlk_abmgr_t *abmgr,
                                    mtlk_ability_id_t ab_id)
{
  BOOL is_enabled = FALSE;
  MTLK_HASH_ENTRY_T(abmgr) *entry;
  MTLK_ASSERT(NULL != abmgr);

  mtlk_osal_lock_acquire(&abmgr->hash_lock);
  entry = mtlk_hash_find_abmgr(&abmgr->ability_states_hash, &ab_id);

  if(NULL != entry)
  {
    _mtlk_ab_record_t *ab_record = MTLK_CONTAINER_OF(entry, _mtlk_ab_record_t, hentry);
    is_enabled = ab_record->is_enabled;
  }

  mtlk_osal_lock_release(&abmgr->hash_lock);


  return is_enabled;
}

void __MTLK_IFUNC
mtlk_abmgr_unregister_ability(mtlk_abmgr_t *abmgr,
                                   mtlk_ability_id_t ab_id)
{
  MTLK_HASH_ENTRY_T(abmgr) *entry;
  _mtlk_ab_record_t *ab_record;
  MTLK_ASSERT(NULL != abmgr);

  mtlk_osal_lock_acquire(&abmgr->hash_lock);

  entry = mtlk_hash_find_abmgr(&abmgr->ability_states_hash, &ab_id);
  MTLK_ASSERT(NULL != entry);
  if (entry) {
    ab_record = MTLK_CONTAINER_OF(entry, _mtlk_ab_record_t, hentry);
    MTLK_ASSERT(!ab_record->is_enabled);
    mtlk_hash_remove_abmgr(&abmgr->ability_states_hash, entry);
    mtlk_osal_mem_free(ab_record);
  }

  mtlk_osal_lock_release(&abmgr->hash_lock);
}

static void
_mtlk_abmgr_set_ability_state(mtlk_abmgr_t*     abmgr,
                                   mtlk_ability_id_t      ab_id,
                                   BOOL                   ab_state)
{
  _mtlk_ab_record_t *ab_record;

  MTLK_HASH_ENTRY_T(abmgr) *entry;
  MTLK_ASSERT(NULL != abmgr);

  mtlk_osal_lock_acquire(&abmgr->hash_lock);

  entry = mtlk_hash_find_abmgr(&abmgr->ability_states_hash, &ab_id);
  MTLK_ASSERT(NULL != entry);
  if (entry) {
    ab_record = MTLK_CONTAINER_OF(entry, _mtlk_ab_record_t, hentry);
    ab_record->is_enabled = ab_state;
  }

  mtlk_osal_lock_release(&abmgr->hash_lock);
}

void __MTLK_IFUNC
mtlk_abmgr_enable_ability(mtlk_abmgr_t*     abmgr,
                               mtlk_ability_id_t  ab_id)
{
  _mtlk_abmgr_set_ability_state(abmgr, ab_id, TRUE);
}

void __MTLK_IFUNC
mtlk_abmgr_disable_ability(mtlk_abmgr_t*    abmgr,
                                mtlk_ability_id_t  ab_id)
{
  _mtlk_abmgr_set_ability_state(abmgr, ab_id, FALSE);
}

int __MTLK_IFUNC
mtlk_abmgr_register_ability_set(mtlk_abmgr_t *abmgr,
                                     const mtlk_ability_id_t* ab_id_list,
                                     uint32 ab_id_num)
{
  unsigned i;
  int res = MTLK_ERR_OK;

  for(i = 0; i < ab_id_num; i++)
  {
    res = mtlk_abmgr_register_ability(abmgr, ab_id_list[i]);
    if(MTLK_ERR_OK != res)
    {
        mtlk_abmgr_unregister_ability_set(abmgr, ab_id_list, i);
        break;
    }
  }

  return res;
}

void __MTLK_IFUNC
mtlk_abmgr_unregister_ability_set(mtlk_abmgr_t *abmgr,
                                       const mtlk_ability_id_t* ab_id_list,
                                       uint32 ab_id_num)
{
  unsigned i;
  for(i = 0; i < ab_id_num; i++)
  {
    mtlk_abmgr_unregister_ability(abmgr, ab_id_list[i]);
  }
}

void __MTLK_IFUNC
mtlk_abmgr_enable_ability_set(mtlk_abmgr_t *abmgr,
                                   const mtlk_ability_id_t* ab_id_list,
                                   uint32 ab_id_num)
{
  unsigned i;
  for(i = 0; i < ab_id_num; i++)
  {
    mtlk_abmgr_enable_ability(abmgr, ab_id_list[i]);
  }
}

void __MTLK_IFUNC
mtlk_abmgr_disable_ability_set(mtlk_abmgr_t *abmgr,
                                    const mtlk_ability_id_t* ab_id_list,
                                    uint32 ab_id_num)
{
  unsigned i;
  for(i = 0; i < ab_id_num; i++)
  {
    mtlk_abmgr_disable_ability(abmgr, ab_id_list[i]);
  }
}

mtlk_abmgr_t* __MTLK_IFUNC
mtlk_abmgr_create(void)
{
  mtlk_abmgr_t *abmgr = mtlk_osal_mem_alloc(sizeof(*abmgr), MTLK_MEM_TAG_ABMGR);
  if(NULL == abmgr) {
    return NULL;
  }

  memset(abmgr, 0, sizeof(*abmgr));

  if (MTLK_ERR_OK != _mtlk_abmgr_init(abmgr)) {
    mtlk_osal_mem_free(abmgr);
    return NULL;
  }

  return abmgr;
}

void __MTLK_IFUNC
mtlk_abmgr_delete (mtlk_abmgr_t *abmgr)
{
  _mtlk_abmgr_cleanup(abmgr);
  mtlk_osal_mem_free(abmgr);
}

