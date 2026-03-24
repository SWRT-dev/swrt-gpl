/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_NBUF_PRIV_H__
#define __MTLK_NBUF_PRIV_H__

#include "stadb.h"

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_NBUF
#define LOG_LOCAL_FID   0

#define MTLK_NBUFF_DIRECTED    0x0001   /* this unit receiver address (802.11n ADDR1)        */
#define MTLK_NBUFF_UNICAST     0x0002   /* unicast destination address (802.3 DA)            */
#define MTLK_NBUFF_MULTICAST   0x0004   /* multicast destination address (802.3 DA)          */
#define MTLK_NBUFF_BROADCAST   0x0008   /* broadcast destination address (802.3 DA)          */
#define MTLK_NBUFF_FORWARD     0x0010   /* sk_buff should be forwarded                       */
#define MTLK_NBUFF_CONSUME     0x0020   /* sk_buff should be consumed by OS                  */
#define MTLK_NBUFF_SHORTCUT    0x0040   /* learn acceleration subsystem to shortcut src/dest */

struct mtlk_nbuf_priv_internal
{
  uint8                   rsn_bits;
  uint8                   reserved[1];
  uint16                  flags;

  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
  mtlk_handle_t           stats;
#endif
#ifdef MTLK_DEBUG_CHARIOT_OOO
  uint16                  seq_num;
  uint8                   seq_qos;
#endif
};

typedef struct _mtlk_nbuf_priv_t
{
  uint8 private[sizeof(struct mtlk_nbuf_priv_internal)];
} __MTLK_IDATA mtlk_nbuf_priv_t;

static __INLINE uint16
mtlk_nbuf_priv_check_flags (const mtlk_nbuf_priv_t *priv,
                            uint16                  flags)
{
  const struct mtlk_nbuf_priv_internal *p = 
    (const struct mtlk_nbuf_priv_internal *)priv->private;
  return (p->flags & flags);
}

static __INLINE void
mtlk_nbuf_priv_set_flags (mtlk_nbuf_priv_t *priv,
                          uint16            flags)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;
  p->flags |= flags;
}

static __INLINE uint16
mtlk_nbuf_priv_get_flags (mtlk_nbuf_priv_t *priv)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;
  return p->flags;
}

static __INLINE void
mtlk_nbuf_priv_reset_flags (mtlk_nbuf_priv_t *priv,
                            uint16            flags)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;

  p->flags &= ~flags;
}


static __INLINE void
mtlk_nbuf_priv_set_rcn_bits (mtlk_nbuf_priv_t *priv,
                             uint8             rcn_bits)
{
  struct mtlk_nbuf_priv_internal *p = 
    (struct mtlk_nbuf_priv_internal *)priv->private;

  p->rsn_bits = rcn_bits;
}

static __INLINE uint8
mtlk_nbuf_priv_get_rcn_bits (const mtlk_nbuf_priv_t *priv)
{
  const struct mtlk_nbuf_priv_internal *p = 
    (const struct mtlk_nbuf_priv_internal *)priv->private;

  return p->rsn_bits;
}


/****************************************************************
 * NOTE: DF is responsible to initiate/cleanup NBUF's private 
 *       data prior to passing NBUF to CORE or OS.
 *       Thus, CORE is always working with initialized NBUF's 
 *       private data and MUST not call these functions.
 ****************************************************************/
static __INLINE void
mtlk_nbuf_priv_init (mtlk_nbuf_priv_t *priv)
{
  memset(priv, 0, sizeof(*priv));
}

static __INLINE void
mtlk_nbuf_priv_cleanup (mtlk_nbuf_priv_t *priv)
{
#ifndef DONT_PERFORM_PRIV_CLEANUP
  memset(priv, 0, sizeof(*priv));
#endif /* DONT_PERFORM_PRIV_CLEANUP */
}

static __INLINE void
  mtlk_nbuf_priv_cleanup_for_reuse (mtlk_nbuf_priv_t *priv)
{
#ifndef DONT_PERFORM_PRIV_CLEANUP
#ifdef CPTCFG_IWLWAV_ENABLE_NBUF_OBJPOOL
  /* Clean all private section exclude objdump structure */
  memset(priv, 0, MTLK_OFFSET_OF(struct mtlk_nbuf_priv_internal, objpool_ctx));
  memset((uint8 *)priv + MTLK_OFFSET_OF(struct mtlk_nbuf_priv_internal, objpool_ctx) + 
          MTLK_FIELD_SIZEOF(struct mtlk_nbuf_priv_internal, objpool_ctx), 0, 
          sizeof(*priv) - MTLK_OFFSET_OF(struct mtlk_nbuf_priv_internal, objpool_ctx) - 
          MTLK_FIELD_SIZEOF(struct mtlk_nbuf_priv_internal, objpool_ctx));
#else
  memset(priv, 0, sizeof(*priv));
#endif
#endif /* DONT_PERFORM_PRIV_CLEANUP */
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
#include "mtlknbufstats.h"
#endif

#endif /* __MTLK_NBUF_PRIV_H__ */
