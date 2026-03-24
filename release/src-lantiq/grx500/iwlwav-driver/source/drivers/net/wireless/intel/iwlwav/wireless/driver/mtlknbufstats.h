/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_NBUF_STATS_H__
#define __MTLK_NBUF_STATS_H__

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
#include "mtlknbufpriv.h"
#include "mtlk_vap_manager.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

typedef enum
{
  MTLK_NBUF_STATS_OID,
  MTLK_NBUF_STATS_AC,
  MTLK_NBUF_STATS_FLAGS,
  MTLK_NBUF_STATS_DATA_SIZE,
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  MTLK_NBUF_STATS_TS_SQ_IN,
  MTLK_NBUF_STATS_TS_FW_IN,
  MTLK_NBUF_STATS_TS_FW_OUT,
#endif
  MTLK_NBUF_STATS_LAST
} mtlk_nbuf_priv_stats_type_e;

int __MTLK_IFUNC
mtlk_nbuf_priv_stats_init(void);
void __MTLK_IFUNC
mtlk_nbuf_priv_stats_cleanup(void);

const char* __MTLK_IFUNC
mtlk_nbuf_priv_stats_get_name(mtlk_nbuf_priv_stats_type_e type);

void __MTLK_IFUNC
mtlk_nbuf_priv_stats_reset(void);

void __MTLK_IFUNC
mtlk_nbuf_priv_stats_set_max_entries(uint32 nof_entries);
uint32 __MTLK_IFUNC
mtlk_nbuf_priv_stats_get_max_entries(void);

typedef struct
{
  uint32 val[MTLK_NBUF_STATS_LAST];
} __MTLK_IDATA mtlk_nbuf_priv_stats_t;

mtlk_nbuf_priv_stats_t * __MTLK_IFUNC
mtlk_nbuf_priv_stats_get_array(uint32 *stats_array_size);

mtlk_handle_t __MTLK_IFUNC
_mtlk_nbuf_priv_stats_alloc(void);
void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_free(mtlk_handle_t stats);

void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_set(mtlk_handle_t stats, mtlk_nbuf_priv_stats_type_e type, uint32 val);

#ifdef CPTCFG_IWLWAV_PRINT_PER_PACKET_STATS

void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_dump(mtlk_handle_t stats);

#endif

void __MTLK_IFUNC
_mtlk_nbuf_priv_stats_inc(mtlk_handle_t stats, mtlk_nbuf_priv_stats_type_e type);

static __INLINE void
mtlk_nbuf_priv_attach_stats (mtlk_nbuf_priv_t *priv)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;

  p->stats = _mtlk_nbuf_priv_stats_alloc();
}

static __INLINE void
_mtlk_nbuf_priv_detach_stats (mtlk_nbuf_priv_t *priv)
{
  const struct mtlk_nbuf_priv_internal *p = 
    (const struct mtlk_nbuf_priv_internal *)priv->private;

  if (p->stats) {
    _mtlk_nbuf_priv_stats_free(p->stats);
  }
}


static __INLINE void
mtlk_nbuf_priv_stats_set (mtlk_nbuf_priv_t *priv, mtlk_nbuf_priv_stats_type_e type, uint32 val)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;
  if (p->stats) {
    _mtlk_nbuf_priv_stats_set(p->stats, type, val);
  }
}

#ifdef CPTCFG_IWLWAV_PRINT_PER_PACKET_STATS

static __INLINE void
mtlk_nbuf_priv_stats_dump (mtlk_nbuf_priv_t *priv)
{
  struct mtlk_nbuf_priv_internal *p =
    (struct mtlk_nbuf_priv_internal *)priv->private;
  if (p->stats) {
    _mtlk_nbuf_priv_stats_dump(p->stats);
  }
}

#endif

static __INLINE void
mtlk_nbuf_priv_stats_inc (mtlk_nbuf_priv_t *priv, mtlk_nbuf_priv_stats_type_e type)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;
  if (p->stats) {
    _mtlk_nbuf_priv_stats_inc(p->stats, type);
  }
}

static __INLINE void
mtlk_nbuf_priv_set_flags_with_stats (mtlk_nbuf_priv_t *priv,
                                     uint16            flags)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;
  mtlk_nbuf_priv_set_flags(priv, flags);
  mtlk_nbuf_priv_stats_set(priv, MTLK_NBUF_STATS_FLAGS, (uint32)p->flags);
}


static __INLINE void
mtlk_nbuf_priv_reset_flags_with_stats (mtlk_nbuf_priv_t *priv,
                                       uint16            flags)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;
  mtlk_nbuf_priv_reset_flags(priv, flags);
  mtlk_nbuf_priv_stats_set(priv, MTLK_NBUF_STATS_FLAGS, (uint32)p->flags);
}

static __INLINE void
mtlk_nbuf_priv_set_vap_handle_with_stats (mtlk_nbuf_priv_t *priv,
                                          mtlk_vap_handle_t vap_handle)
{
  mtlk_nbuf_priv_stats_set(priv, MTLK_NBUF_STATS_OID, (uint32)mtlk_vap_get_oid(vap_handle));
}

static __INLINE void
mtlk_nbuf_priv_cleanup_with_stats (mtlk_nbuf_priv_t *priv)
{
  _mtlk_nbuf_priv_detach_stats(priv);
  mtlk_nbuf_priv_cleanup(priv);
}

#define mtlk_nbuf_priv_set_flags(p,f)     mtlk_nbuf_priv_set_flags_with_stats((p),(f))
#define mtlk_nbuf_priv_reset_flags(p,f)   mtlk_nbuf_priv_reset_flags_with_stats((p),(f))
#define mtlk_nbuf_priv_set_vap_index(p,h) mtlk_nbuf_priv_set_vap_handle_with_stats((p),(h))
#define mtlk_nbuf_priv_cleanup(p)         mtlk_nbuf_priv_cleanup_with_stats(p)

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* CPTCFG_IWLWAV_PER_PACKET_STATS */

#endif /* __MTLK_NBUF_PRIV_H__ */
