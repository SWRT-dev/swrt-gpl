/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_WSS_H__
#define __MTLK_WSS_H__

/* Wireless State Syndication Module */

#include "mhi_global_definitions.h"
#include "mtlkerr.h"
#include "mtlk_osal.h"
#include "mtlk_wss_id.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define MTLK_WSS_INVALID_VALUE_UINT32  MAX_UINT32
#define MTLK_WSS_INVALID_VALUE_UINT64  MAX_UINT64

typedef struct _mtlk_wss_t       mtlk_wss_t;

int          __MTLK_IFUNC mtlk_wss_module_init(void);
void         __MTLK_IFUNC mtlk_wss_module_cleanup(void);

#ifndef CPTCFG_IWLWAV_ENABLE_OBJPOOL
mtlk_wss_t * __MTLK_IFUNC mtlk_wss_create(const mtlk_wss_t *parent_wss, const mtlk_wss_stat_id_e *stat_ids, uint32 nof_stats_ids);
#else
mtlk_wss_t * __MTLK_IFUNC __mtlk_wss_create_objpool(const mtlk_wss_t *parent_wss, const mtlk_wss_stat_id_e *stat_ids, uint32 nof_stat_ids, mtlk_slid_t caller_slid);

#define mtlk_wss_create(parent_wss, stat_ids, nof_stat_ids) \
  __mtlk_wss_create_objpool((parent_wss), (stat_ids), (nof_stat_ids), MTLK_SLID)
#endif

void         __MTLK_IFUNC mtlk_wss_delete(mtlk_wss_t *wss);

uint32       __MTLK_IFUNC mtlk_wss_get_stat(mtlk_wss_t *wss, uint32 stat_id_idx);
void         __MTLK_IFUNC mtlk_wss_reset_stat(mtlk_wss_t *wss, uint32 stat_id_idx);
uint64       __MTLK_IFUNC mtlk_wss_get_stat_64(mtlk_wss_t *wss, uint32 stat_id_idx);
void         __MTLK_IFUNC mtlk_wss_reset_stat_64(mtlk_wss_t *wss, uint32 stat_id_idx);

typedef struct _mtlk_wss_stats_context_t
{
  mtlk_wss_stat_id_e stat_id;
  union {
    mtlk_atomic_t    value;
    mtlk_atomic64_t  value64;
  } u;
} __MTLK_IFUNC mtlk_wss_stats_context_t;

typedef struct _mtlk_wss_cntr_handle_t
{
  uint32                    nof_contexts;
  mtlk_wss_stats_context_t *contexts[1];
} __MTLK_IDATA mtlk_wss_cntr_handle_t;


/* struct for statistic updated by polling */
typedef struct _poll_stat_obj_t {
    uint16                  nof_cntrs_config;
    uint16                  nof_cntrs_actual;
    mtlk_wss_cntr_handle_t  **wss_hcntrs;
    uint32                  *last_val;
} poll_stat_obj_t;


mtlk_wss_cntr_handle_t * __MTLK_IFUNC mtlk_wss_cntr_open(mtlk_wss_t *wss, mtlk_wss_stat_id_e stat_id);
void                     __MTLK_IFUNC mtlk_wss_cntr_close(mtlk_wss_t *wss, mtlk_wss_cntr_handle_t *hcntr);

int                      __MTLK_IFUNC mtlk_wss_cntrs_open(mtlk_wss_t               *wss,
                                                          const mtlk_wss_stat_id_e *stat_ids, 
                                                          mtlk_wss_cntr_handle_t  **hctrls,
                                                          uint32                    nof_stats);
void                     __MTLK_IFUNC mtlk_wss_cntrs_close(mtlk_wss_t              *wss,
                                                           mtlk_wss_cntr_handle_t **hctrls,
                                                           uint32                   nof_stats);

/* 32-bit counters */

static __INLINE void
mtlk_wss_cntr_inc (mtlk_wss_cntr_handle_t *hcntr)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic_inc(&hcntr->contexts[i]->u.value);
  }
}

static __INLINE void
mtlk_wss_cntr_dec (mtlk_wss_cntr_handle_t *hcntr)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic_dec(&hcntr->contexts[i]->u.value);
  }
}

static __INLINE void
mtlk_wss_cntr_add (mtlk_wss_cntr_handle_t *hcntr, uint32 val)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic_add(&hcntr->contexts[i]->u.value, val);
  }
}

static __INLINE void
mtlk_wss_cntr_sub (mtlk_wss_cntr_handle_t *hcntr, uint32 val)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic_sub(&hcntr->contexts[i]->u.value, val);
  }
}

/* 64-bit counters */

static __INLINE void
mtlk_wss_cntr_inc64 (mtlk_wss_cntr_handle_t *hcntr)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic64_inc(&hcntr->contexts[i]->u.value64);
  }
}

static __INLINE void
mtlk_wss_cntr_dec64 (mtlk_wss_cntr_handle_t *hcntr)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic64_dec(&hcntr->contexts[i]->u.value64);
  }
}

static __INLINE void
mtlk_wss_cntr_add64 (mtlk_wss_cntr_handle_t *hcntr, uint64 val)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic64_add(&hcntr->contexts[i]->u.value64, val);
  }
}

static __INLINE void
mtlk_wss_cntr_sub64 (mtlk_wss_cntr_handle_t *hcntr, uint64 val)
{
  uint32 i = 0;

  for (; i < hcntr->nof_contexts; i++) {
    mtlk_osal_atomic64_sub(&hcntr->contexts[i]->u.value64, val);
  }
}

/**************/

static __INLINE void
mtlk_wss_poll_stat_stop (poll_stat_obj_t *obj)
{
    /* obj was checked by NULL during init */
    obj->nof_cntrs_actual = 0;
}

static __INLINE void
mtlk_wss_poll_stat_start (poll_stat_obj_t *obj)
{
    /* obj was checked by NULL during init */
    obj->nof_cntrs_actual = obj->nof_cntrs_config;
}

static __INLINE BOOL
mtlk_wss_poll_stat_is_started(poll_stat_obj_t *obj)
{
    return (0 != obj->nof_cntrs_actual);
}

void __MTLK_IFUNC
mtlk_wss_poll_stat_init(poll_stat_obj_t *obj, uint32 nof_cntrs, mtlk_wss_cntr_handle_t *wss_hcntrs[], uint32 *values);

void __MTLK_IFUNC
mtlk_poll_stat_update_cntrs(poll_stat_obj_t *obj, uint32 *values, uint32 values_num);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_WSS_H__ */
