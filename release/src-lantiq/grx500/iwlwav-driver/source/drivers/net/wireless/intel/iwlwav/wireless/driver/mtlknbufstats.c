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
#include "mtlknbufpriv.h"
#include "mtlkstartup.h"

#define LOG_LOCAL_GID   GID_NBUFSTATS
#define LOG_LOCAL_FID   1

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS

#include "mtlklist.h"

static const char *stat_names[] =
{
  "oid",
  "ac",
  "flags",
  "data_size",
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  "ts_sq_in",
  "ts_fw_in",
  "ts_fw_out",
#endif
};

typedef struct
{
  mtlk_nbuf_priv_stats_t data;
  mtlk_dlist_entry_t     lentry;
} mtlk_nbuf_priv_stats_entry_t;

typedef struct
{
  uint32                        max_entries;
  mtlk_osal_spinlock_t          lock;
  mtlk_dlist_t                  list;
  MTLK_DECLARE_INIT_STATUS;
} mtlk_nbuf_priv_stats_collector_t;

static mtlk_nbuf_priv_stats_collector_t collector;
static mtlk_nbuf_priv_stats_collector_t *c = &collector;

static void
_mtlk_nbuf_priv_stats_adjust_max_entries (uint32 max_entries, BOOL set_max_entries)
{
  while (mtlk_dlist_size(&c->list) > max_entries) {
    mtlk_dlist_entry_t           *e = mtlk_dlist_pop_front(&c->list);
    mtlk_nbuf_priv_stats_entry_t *s = MTLK_CONTAINER_OF(e, mtlk_nbuf_priv_stats_entry_t, lentry);
    mtlk_osal_mem_free(s);
  }

  if (set_max_entries) {
    c->max_entries = max_entries;
  }
}

MTLK_INIT_STEPS_LIST_BEGIN(nbuf_stats)
  MTLK_INIT_STEPS_LIST_ENTRY(nbuf_stats, INIT_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(nbuf_stats, INIT_LIST)
  MTLK_INIT_STEPS_LIST_ENTRY(nbuf_stats, FILL_ENTRIES)
MTLK_INIT_INNER_STEPS_BEGIN(nbuf_stats)
MTLK_INIT_STEPS_LIST_END(nbuf_stats);

int __MTLK_IFUNC
mtlk_nbuf_priv_stats_init (void)
{
  MTLK_ASSERT(ARRAY_SIZE(stat_names) == MTLK_NBUF_STATS_LAST);

  MTLK_INIT_TRY(nbuf_stats, MTLK_OBJ_PTR(c))
    c->max_entries = 0;
    MTLK_INIT_STEP(nbuf_stats, INIT_LOCK, MTLK_OBJ_PTR(c),
                   mtlk_osal_lock_init, (&c->lock));
    MTLK_INIT_STEP_VOID(nbuf_stats, INIT_LIST, MTLK_OBJ_PTR(c),
                        mtlk_dlist_init, (&c->list));
    MTLK_INIT_STEP_VOID(nbuf_stats, FILL_ENTRIES, MTLK_OBJ_PTR(c),
                        MTLK_NOACTION, ());
  MTLK_INIT_FINALLY(nbuf_stats, MTLK_OBJ_PTR(c))
  MTLK_INIT_RETURN(nbuf_stats, MTLK_OBJ_PTR(c), mtlk_nbuf_priv_stats_cleanup, ())
}

void __MTLK_IFUNC
mtlk_nbuf_priv_stats_cleanup (void)
{
  MTLK_CLEANUP_BEGIN(nbuf_stats, MTLK_OBJ_PTR(c))
    MTLK_CLEANUP_STEP(nbuf_stats, FILL_ENTRIES, MTLK_OBJ_PTR(c),
                      _mtlk_nbuf_priv_stats_adjust_max_entries, (0, TRUE));
    MTLK_CLEANUP_STEP(nbuf_stats, INIT_LIST, MTLK_OBJ_PTR(c),
                      mtlk_dlist_cleanup, (&c->list));
    MTLK_CLEANUP_STEP(nbuf_stats, INIT_LOCK, MTLK_OBJ_PTR(c),
                      mtlk_osal_lock_cleanup, (&c->lock));
  MTLK_CLEANUP_END(nbuf_stats, MTLK_OBJ_PTR(c))
}

const char* __MTLK_IFUNC
mtlk_nbuf_priv_stats_get_name (mtlk_nbuf_priv_stats_type_e type)
{
  MTLK_ASSERT(type >= 0 && type < MTLK_NBUF_STATS_LAST);
  return stat_names[type];
}

void __MTLK_IFUNC
mtlk_nbuf_priv_stats_reset (void)
{
  mtlk_osal_lock_acquire(&c->lock);
  _mtlk_nbuf_priv_stats_adjust_max_entries(0, FALSE);
  mtlk_osal_lock_release(&c->lock);
}

void __MTLK_IFUNC
mtlk_nbuf_priv_stats_set_max_entries (uint32 nof_entries)
{
  mtlk_osal_lock_acquire(&c->lock);
  _mtlk_nbuf_priv_stats_adjust_max_entries(nof_entries, TRUE);
  mtlk_osal_lock_release(&c->lock);
}

uint32 __MTLK_IFUNC
mtlk_nbuf_priv_stats_get_max_entries (void)
{
  uint32 res;
  mtlk_osal_lock_acquire(&c->lock);
  res = c->max_entries;
  mtlk_osal_lock_release(&c->lock);
  return res;
}

mtlk_nbuf_priv_stats_t * __MTLK_IFUNC
mtlk_nbuf_priv_stats_get_array (uint32 *stats_array_size)
{
  mtlk_nbuf_priv_stats_t *res = NULL;
  
  mtlk_osal_lock_acquire(&c->lock);
  *stats_array_size = mtlk_dlist_size(&c->list);
  if (*stats_array_size) {
    res = mtlk_osal_mem_alloc((*stats_array_size) * sizeof(mtlk_nbuf_priv_stats_t), 
                               MTLK_MEM_TAG_NBUF_STATS);
    if (res) {
      mtlk_dlist_entry_t *e;
      mtlk_dlist_entry_t *head;
      uint32              i = 0;
      mtlk_dlist_foreach(&c->list, e, head) {
        mtlk_nbuf_priv_stats_entry_t *stats = MTLK_CONTAINER_OF(e, mtlk_nbuf_priv_stats_entry_t, lentry);
        res[i++] = stats->data;
      }
    }
  }
  mtlk_osal_lock_release(&c->lock);

  return res;
}

mtlk_handle_t __MTLK_IFUNC
_mtlk_nbuf_priv_stats_alloc (void)
{
  mtlk_nbuf_priv_stats_entry_t *res = 
    mtlk_osal_mem_alloc(sizeof(*res), MTLK_MEM_TAG_NBUF_STATS);
  if (res) {
    memset(res, 0, sizeof(*res));
  }

  return HANDLE_T(res);
}

void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_free (mtlk_handle_t _stats)
{
  mtlk_nbuf_priv_stats_entry_t *stats = HANDLE_T_PTR(mtlk_nbuf_priv_stats_entry_t, _stats);

  mtlk_osal_lock_acquire(&c->lock);
  if (c->max_entries) {
    if (mtlk_dlist_size(&c->list) >= c->max_entries) {
      mtlk_dlist_entry_t           *e = mtlk_dlist_pop_front(&c->list);
      mtlk_nbuf_priv_stats_entry_t *s = MTLK_CONTAINER_OF(e, mtlk_nbuf_priv_stats_entry_t, lentry);
      mtlk_osal_mem_free(s);
    }
    mtlk_dlist_push_back(&c->list, &stats->lentry);
  }
  else {
    mtlk_osal_mem_free(stats);
  }
  mtlk_osal_lock_release(&c->lock);
}

void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_set (mtlk_handle_t _stats, mtlk_nbuf_priv_stats_type_e type, uint32 val)
{
  mtlk_nbuf_priv_stats_entry_t *stats = HANDLE_T_PTR(mtlk_nbuf_priv_stats_entry_t, _stats);

  MTLK_ASSERT(type >= 0 && type < MTLK_NBUF_STATS_LAST);
  mtlk_osal_lock_acquire(&c->lock);
  stats->data.val[type] = val;
  mtlk_osal_lock_release(&c->lock);
}

#ifdef CPTCFG_IWLWAV_PRINT_PER_PACKET_STATS

void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_dump(mtlk_handle_t _stats)
{
  mtlk_nbuf_priv_stats_entry_t *stats = HANDLE_T_PTR(mtlk_nbuf_priv_stats_entry_t, _stats);
  uint32 stats_values[MTLK_NBUF_STATS_LAST];

  mtlk_osal_lock_acquire(&c->lock);
  wave_memcpy(stats_values, sizeof(stats_values), stats->data.val, sizeof(stats->data.val));
  mtlk_osal_lock_release(&c->lock);

  ILOG1_DDDDDDD("Packet stats: WLAN ID: %u, AC: %u, FLAGS: 0x%X, SIZE: %u, TS_SQ_IN: %u, TS_FW_IN: %u, TS_FW_OUT: %u",
        stats_values[MTLK_NBUF_STATS_OID],
        stats_values[MTLK_NBUF_STATS_AC],
        stats_values[MTLK_NBUF_STATS_FLAGS],
        stats_values[MTLK_NBUF_STATS_DATA_SIZE],
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
        stats_values[MTLK_NBUF_STATS_TS_SQ_IN],
        stats_values[MTLK_NBUF_STATS_TS_FW_IN],
        stats_values[MTLK_NBUF_STATS_TS_FW_OUT]
#else
        0,
        0,
        0
#endif
        );
}

#endif

void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_inc(mtlk_handle_t _stats, mtlk_nbuf_priv_stats_type_e type)
{
  mtlk_nbuf_priv_stats_entry_t *stats = HANDLE_T_PTR(mtlk_nbuf_priv_stats_entry_t, _stats);

  MTLK_ASSERT(type >= 0 && type < MTLK_NBUF_STATS_LAST);
  mtlk_osal_lock_acquire(&c->lock);
  ++stats->data.val[type];
  mtlk_osal_lock_release(&c->lock);
}

#endif /*NBUF_DEBUG_STATS*/
