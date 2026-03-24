/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef SAFE_PLACE_TO_INCLUDE_DF_NBUF_IMPL
#error "You shouldn't include this file directly!"
#endif

#include "hw_mmb.h"
#include "mtlkhal.h"

#define LOG_LOCAL_GID   GID_NBUF
#define LOG_LOCAL_FID   1

/**********************************************************************
 * DF Network Buffer API OS depended implementation
 * - and -
 * DF Network Buffers lists (doubly linked) OS depended implementation
 **********************************************************************
 *
 * Since current driver implementation doesn't set NETIF_F_SG and NETIF_F_FRAGLIST
 * feature flags, kernel network subsystem linearizes all buffers
 * before calling driver's transmit function, therefore standard
 * memory access API (like memcpy()) can be used with DF Network Buffer.
 *
 **********************************************************************/

static __INLINE mtlk_nbuf_priv_t *
mtlk_nbuf_priv(mtlk_nbuf_t *nbuf)
{
  MTLK_ASSERT(sizeof(mtlk_nbuf_priv_t) <= MTLK_FIELD_SIZEOF(struct sk_buff, cb));
  return (mtlk_nbuf_priv_t *)nbuf->cb;
}

static __INLINE mtlk_nbuf_t *
_mtlk_df_nbuf_alloc_osdep (uint32 size)
{
  mtlk_nbuf_t *nbuf = dev_alloc_skb(size);
  if (__LIKELY(nbuf)) {
    mtlk_nbuf_priv_init(mtlk_nbuf_priv(nbuf));
  }
  return nbuf;
}

static __INLINE void
_mtlk_df_nbuf_free_osdep (mtlk_nbuf_t *nbuf)
{
#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
  if (__LIKELY(nbuf)) {
    mtlk_nbuf_priv_cleanup(mtlk_nbuf_priv(nbuf));
  }
#endif
  dev_kfree_skb(nbuf);
}

static __INLINE mtlk_nbuf_t *
_mtlk_df_nbuf_clone_no_priv_osdep (mtlk_nbuf_t *nbuf)
{
  return skb_clone(nbuf, GFP_ATOMIC);
}

static __INLINE mtlk_nbuf_t *
_mtlk_df_nbuf_clone_with_priv_osdep (mtlk_nbuf_t *nbuf)
{
  return skb_clone(nbuf, GFP_ATOMIC);
}

static __INLINE void *
mtlk_df_nbuf_get_virt_addr (mtlk_nbuf_t *nbuf)
{
  return nbuf->data;
}

#define mtlk_df_nbuf_map_to_phys_addr(ctx, nbuf, size, direction) \
  mtlk_osal_map_to_phys_addr(ctx, nbuf->data, size, direction)

#define  mtlk_df_nbuf_unmap_phys_addr(ctx, nbuf, addr, size, direction) \
  mtlk_osal_unmap_phys_addr(ctx, addr, size, direction)

static __INLINE void 
mtlk_df_nbuf_reserve (mtlk_nbuf_t *nbuf, uint32 len)
{
  skb_reserve(nbuf, len);
}

static __INLINE void *
mtlk_df_nbuf_put (mtlk_nbuf_t *nbuf, uint32 len)
{
  return skb_put(nbuf, len);
}

static __INLINE void
mtlk_df_nbuf_trim (mtlk_nbuf_t *nbuf, uint32 len)
{
  skb_trim(nbuf, len);
}

static __INLINE void *
mtlk_df_nbuf_pull (mtlk_nbuf_t *nbuf, uint32 len)
{
  return skb_pull(nbuf, len);
}

static __INLINE void *
mtlk_df_nbuf_push (mtlk_nbuf_t *nbuf, uint32 len)
{
  return skb_push(nbuf, len);
}

static __INLINE uint32
mtlk_df_nbuf_get_tail_room_size (mtlk_nbuf_t *nbuf)
{
  return skb_end_pointer(nbuf) - skb_tail_pointer(nbuf);
}

static __INLINE int
mtlk_df_nbuf_read (mtlk_nbuf_t *nbuf, 
                   uint32       offset, 
                   uint32       length, 
                   uint8       *destination)
{
  ASSERT(nbuf != NULL);

  if (unlikely(skb_copy_bits(nbuf, 
                             offset, 
                             destination, 
                             length)))
      return MTLK_ERR_UNKNOWN;

  return MTLK_ERR_OK;
}

static __INLINE int
mtlk_df_nbuf_write (mtlk_nbuf_t *nbuf, 
                    uint32       offset, 
                    uint32       length, 
                    uint8       *source)
{
  ASSERT(nbuf != NULL);

  if (unlikely(skb_store_bits(nbuf,
                             offset,
                             source,
                             length)))
      return MTLK_ERR_UNKNOWN;

  return MTLK_ERR_OK;
}

/* Again, skb is assumed to be linearized (i.e. skb->data_len == 0) */
static __INLINE uint32
mtlk_df_nbuf_get_data_length (mtlk_nbuf_t *nbuf)
{
  ASSERT(nbuf != NULL);
  return nbuf->len;
}

static __INLINE uint16
mtlk_df_nbuf_get_priority (mtlk_nbuf_t *nbuf)
{
  ASSERT(nbuf != NULL);
  return nbuf->priority;
}

static __INLINE void
mtlk_df_nbuf_set_priority (mtlk_nbuf_t *nbuf, 
                           uint16       priority)
{
  ASSERT(nbuf != NULL);
  nbuf->priority = priority;
}

static __INLINE void
mtlk_df_nbuf_list_init (mtlk_nbuf_list_t *pbuflist)
{
  ASSERT(pbuflist != NULL);
  skb_queue_head_init(pbuflist);
}

static __INLINE void
mtlk_df_nbuf_list_cleanup (mtlk_nbuf_list_t *pbuflist)
{
  ASSERT(pbuflist != NULL);
  ASSERT(skb_queue_empty(pbuflist));
  skb_queue_head_init(pbuflist);
}

static __INLINE void
mtlk_df_nbuf_list_push_front (mtlk_nbuf_list_t       *pbuflist,
                              mtlk_nbuf_list_entry_t *pentry)
{
  ASSERT(pbuflist != NULL);
  ASSERT(pentry != NULL);
  __skb_queue_head(pbuflist, pentry);
}

static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_pop_front (mtlk_nbuf_list_t *pbuflist)
{
  ASSERT(pbuflist != NULL);
  return __skb_dequeue(pbuflist);
}

static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_remove_entry (mtlk_nbuf_list_t       *pbuflist,
                                mtlk_nbuf_list_entry_t *pentry)
{
  struct sk_buff *ret_skb;
  ASSERT(pbuflist != NULL);
  ASSERT(pentry != NULL);
  ret_skb = pentry->next;
  __skb_unlink(pentry, pbuflist);
  return ret_skb;
}

static __INLINE void
mtlk_df_nbuf_list_push_back (mtlk_nbuf_list_t       *pbuflist,
                             mtlk_nbuf_list_entry_t *pentry)
{
  ASSERT(pbuflist != NULL);
  ASSERT(pentry != NULL);
  __skb_queue_tail(pbuflist, pentry);
}

static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_head (mtlk_nbuf_list_t *pbuflist)
{
  ASSERT(pbuflist != NULL);
  return (mtlk_nbuf_list_entry_t*)pbuflist;
}

static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_next (mtlk_nbuf_list_entry_t *pentry)
{
  ASSERT(pentry != NULL);
  return pentry->next;
}

static __INLINE int8
mtlk_df_nbuf_list_is_empty (mtlk_nbuf_list_t *pbuflist)
{
  ASSERT(pbuflist != NULL);
  return skb_queue_empty(pbuflist);
}

static __INLINE uint32
mtlk_df_nbuf_list_size (mtlk_nbuf_list_t* pbuflist)
{
  ASSERT(pbuflist != NULL);
  return skb_queue_len(pbuflist);
}

static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_get_list_entry (mtlk_nbuf_t *nbuf)
{
  ASSERT(nbuf != NULL);
  return nbuf;
}

static __INLINE mtlk_nbuf_t *
mtlk_df_nbuf_get_by_list_entry (mtlk_nbuf_list_entry_t *pentry)
{
  ASSERT(pentry != NULL);
  return pentry;
}

static __INLINE void
mtlk_df_nbuf_reset_mac_header (mtlk_nbuf_t *nbuf)
{
    ASSERT(nbuf != NULL);
    skb_reset_mac_header(nbuf);
}

static __INLINE mtlk_error_t
mtlk_df_nbuf_get_vlan_type (mtlk_nbuf_t *nbuf, uint16 * type, uint32 *vlan_len)
{
  uint16 proto_type;
  uint32 nbuf_len;
  uint32 len = 0;
  struct ethhdr *ether_header;

  ASSERT(nbuf != NULL);
  ASSERT(nbuf->data != NULL);

  nbuf_len = mtlk_df_nbuf_get_data_length(nbuf);
  if (sizeof(*ether_header) > nbuf_len)
    return MTLK_ERR_NOT_IN_USE;

  ether_header = (struct ethhdr *)nbuf->data;
  proto_type = ntohs(ether_header->h_proto);

  while (eth_type_vlan(proto_type)) {
    struct vlan_hdr *vh;

    if ((len + VLAN_HLEN + sizeof(*ether_header)) > nbuf_len)
      return MTLK_ERR_NOT_IN_USE;

    vh = (struct vlan_hdr *)(nbuf->data + sizeof(*ether_header) + len);
    proto_type = ntohs(vh->h_vlan_encapsulated_proto);
    len += VLAN_HLEN;
  }

  *vlan_len = len;
  *type = proto_type;
  return MTLK_ERR_OK;
}

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID
