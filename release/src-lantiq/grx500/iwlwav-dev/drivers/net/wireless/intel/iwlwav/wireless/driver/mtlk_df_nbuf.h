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
 * Driver framework NBUF API.
 *
 */

#ifndef __DF_NBUF_H__
#define __DF_NBUF_H__

#include "mtlk_vap_manager.h"
#include "mtlknbufpriv.h"

/**********************************************************************
 * DF Network Buffer (former - Payload) Interface
 *  - The DF sub-interface which is provided OS depended functionality
 *    for Network Buffer manipulations
 **********************************************************************/

static __INLINE void*
mtlk_df_nbuf_get_virt_addr(mtlk_nbuf_t *nbuf);
static __INLINE uint32
mtlk_df_nbuf_get_tail_room_size(mtlk_nbuf_t *nbuf);
static __INLINE void
mtlk_df_nbuf_reserve(mtlk_nbuf_t *nbuf, uint32 len);
static __INLINE void*
mtlk_df_nbuf_put(mtlk_nbuf_t *nbuf, uint32 len);
static __INLINE void
mtlk_df_nbuf_trim(mtlk_nbuf_t *nbuf, uint32 len);
static __INLINE void*
mtlk_df_nbuf_pull(mtlk_nbuf_t *nbuf, uint32 len);
/*!
\brief   Read data from data buffer.

\param   nbuf           Pointer to data buffer
\param   offset         Offset from buffer beginning
\param   length         Number of bytes to read
\param   destination    Destination buffer

\return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
*/
static __INLINE int
mtlk_df_nbuf_read(mtlk_nbuf_t *nbuf, uint32 offset, uint32 length, uint8 *destination);

/*! TODO: GS: unused - delete it
\brief   Write data to data buffer.

\param   nbuf           Pointer to data buffer
\param   offset         Offset from buffer beginning
\param   length         Number of bytes to write
\param   destination    Source buffer

\return  MTLK_ERR... values, MTLK_ERR_OK if succeeded
*/
static __INLINE int
mtlk_df_nbuf_write(mtlk_nbuf_t *nbuf, uint32 offset, uint32 length, uint8 *source);

/*! TODO: GS: unused - delete it
\brief   Query total data buffer length.

\param   nbuf        Pointer to data buffer

\return  Number of bytes
*/
static __INLINE uint32
mtlk_df_nbuf_get_data_length(mtlk_nbuf_t *nbuf);

/*!
\brief   Get packet priority supplied by OS.

\param   nbuf        Pointer to data buffer

\return  Priority value
*/
static __INLINE uint16
mtlk_df_nbuf_get_priority(mtlk_nbuf_t *nbuf);

/*!
\brief   Set packet's priority for OS.

\param   nbuf        Pointer to data buffer
\param   priority    Paket priority value
*/
static __INLINE void
mtlk_df_nbuf_set_priority(mtlk_nbuf_t *nbuf, uint16 priority);

static __INLINE mtlk_nbuf_priv_t*
mtlk_nbuf_priv(mtlk_nbuf_t *nbuf);

/**********************************************************************
 * DF Network Buffers lists (doubly linked)
 **********************************************************************/
/*!
  \brief   Function initializes list object
  \param   pbuflist List object
 */
static __INLINE void
mtlk_df_nbuf_list_init(mtlk_nbuf_list_t *pbuflist);

/*!
  \brief   Function cleanups list object, also asserts in case list is not empty.
  \param   pbuflist List object
 */
static __INLINE void
mtlk_df_nbuf_list_cleanup(mtlk_nbuf_list_t *pbuflist);

/*!
  \brief   Function adds entry to the beginning of a list
  \param   pbuflist List object
           pentry   List entry
 */
static __INLINE void
mtlk_df_nbuf_list_push_front(mtlk_nbuf_list_t *pbuflist, mtlk_nbuf_list_entry_t *pentry);

/*!
  \brief   Function removes entry from the beginning of a list
  \param   pbuflist List object
  \return  Removed entry or NULL if list is empty
 */
static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_pop_front(mtlk_nbuf_list_t *pbuflist);

/*!
  \fn      mtlk_nbuf_list_remove_entry(mtlk_nbuf_list_t* pbuflist, mtlk_nbuf_list_entry_t* pentry)
  \brief   Function removes entry from the list

  \param   pbuflist List object
           pentry   List entry to be removed

  \return  Next entry after removed
 */
static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_remove_entry(mtlk_nbuf_list_t       *pbuflist,
                               mtlk_nbuf_list_entry_t *pentry);

/*!
  \brief   Function adds entry to the end of a list

  \param   pbuflist List object
           pentry   List entry
 */
static __INLINE void
mtlk_df_nbuf_list_push_back(mtlk_nbuf_list_t *pbuflist, mtlk_nbuf_list_entry_t *pentry);

/*!
  \brief   Function returns head of the list

  \param   pbuflist List object

 */
static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_head (mtlk_nbuf_list_t *pbuflist);

/*!
  \brief   Function returns next entry of the list

  \param   pentry   List entry

 */
static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_list_next (mtlk_nbuf_list_entry_t *pentry);

/*!
  \brief   Function checks if list is empty

  \param   pbuflist List object

  \return  1 if list is empty, 0 - otherwise
 */
static __INLINE int8
mtlk_df_nbuf_list_is_empty(mtlk_nbuf_list_t *pbuflist);

/*!
  \brief   Function returns count of elements which are in list

  \param   pbuflist List object

  \return  Count of elements which are in list
 */
static __INLINE uint32
mtlk_df_nbuf_list_size(mtlk_nbuf_list_t* pbuflist);

/*!
\brief   Returns pointer to packet's buffer list entry.

\param   nbuf        Pointer to data buffer

\return  Pointer to buffer list entry
*/
static __INLINE mtlk_nbuf_list_entry_t *
mtlk_df_nbuf_get_list_entry(mtlk_nbuf_t *nbuf);

/*!
\brief   Returns pointer to packet by its buffer list entry.

\return  Pointer to buffer list entry
*/

static __INLINE mtlk_nbuf_t *
mtlk_df_nbuf_get_by_list_entry(mtlk_nbuf_list_entry_t *pentry);

/*!
\brief   Reset MAC header.

\param   nbuf        Pointer to data buffer
*/
static __INLINE void
mtlk_df_nbuf_reset_mac_header (mtlk_nbuf_t *nbuf);

#define  SAFE_PLACE_TO_INCLUDE_DF_NBUF_IMPL
#include "mtlk_df_nbuf_impl.h"

#if !defined(CPTCFG_IWLWAV_ENABLE_OBJPOOL) && defined(CPTCFG_IWLWAV_ENABLE_NBUF_OBJPOOL)
#error ENABLE_OBJPOOL must be defined for ENABLE_NBUF_OBJPOOL
#endif

#ifdef CPTCFG_IWLWAV_ENABLE_NBUF_OBJPOOL
#define mtlk_objpool_add_nbuf(nbuf, caller_slid) \
  if(NULL != nbuf) {  \
    mtlk_objpool_add_object(&g_objpool, \
                            &( ((struct mtlk_nbuf_priv_internal*)mtlk_nbuf_priv(nbuf))->objpool_ctx ), \
                            MTLK_NET_BUF_OBJ, caller_slid); \
  }

#define mtlk_objpool_remove_nbuf(nbuf) \
  if(NULL != nbuf) {  \
    mtlk_objpool_remove_object(&g_objpool, \
                             &( ((struct mtlk_nbuf_priv_internal*)mtlk_nbuf_priv(nbuf))->objpool_ctx ), \
                             MTLK_NET_BUF_OBJ); \
  }

#else
#define mtlk_objpool_add_nbuf(nbuf, caller_slid) MTLK_UNREFERENCED_PARAM(caller_slid)
#define mtlk_objpool_remove_nbuf(nbuf)
#endif /* CPTCFG_IWLWAV_ENABLE_NBUF_OBJPOOL */

#if MTLK_USE_DIRECTCONNECT_DP_API
mtlk_nbuf_t  *_mtlk_df_dcdp_nbuf_alloc(mtlk_df_t *df, uint32 size);

static __INLINE mtlk_nbuf_t *
__mtlk_df_dcdp_nbuf_alloc (mtlk_df_t *df, uint32 size, mtlk_slid_t caller_slid)
{
  mtlk_nbuf_t * nbuf = _mtlk_df_dcdp_nbuf_alloc(df, size);
  mtlk_objpool_add_nbuf(nbuf, caller_slid);
  return nbuf;
}

#define mtlk_df_dcdp_nbuf_alloc(df, size) __mtlk_df_dcdp_nbuf_alloc((df), (size), MTLK_SLID)

#endif /* MTLK_USE_DIRECTCONNECT_DP_API */

static __INLINE mtlk_nbuf_t *
__mtlk_df_nbuf_alloc (uint32 size, mtlk_slid_t caller_slid)
{
  mtlk_nbuf_t * nbuf = _mtlk_df_nbuf_alloc_osdep(size);
  mtlk_objpool_add_nbuf(nbuf, caller_slid);
  return nbuf;
}

#define mtlk_df_nbuf_alloc(size) __mtlk_df_nbuf_alloc((size), MTLK_SLID)

static __INLINE void mtlk_df_nbuf_free(mtlk_nbuf_t *nbuf)
{
  mtlk_objpool_remove_nbuf(nbuf);
  _mtlk_df_nbuf_free_osdep(nbuf);
}

static __INLINE mtlk_nbuf_t *
__mtlk_df_nbuf_clone_no_priv (mtlk_nbuf_t *nbuf, mtlk_slid_t caller_slid)
{
  mtlk_nbuf_t *nbuf_new = _mtlk_df_nbuf_clone_no_priv_osdep(nbuf);
  mtlk_objpool_add_nbuf(nbuf_new, caller_slid);
  return nbuf_new;
}

#define mtlk_df_nbuf_clone_no_priv(nbuf) __mtlk_df_nbuf_clone_no_priv((nbuf), MTLK_SLID)

static __INLINE mtlk_nbuf_t *
__mtlk_df_nbuf_clone_with_priv (mtlk_nbuf_t *nbuf, mtlk_slid_t caller_slid)
{
  mtlk_nbuf_t *nbuf_new = _mtlk_df_nbuf_clone_with_priv_osdep(nbuf);
  mtlk_objpool_add_nbuf(nbuf_new, caller_slid);
  return nbuf_new;
}

#define mtlk_df_nbuf_clone_with_priv(nbuf) __mtlk_df_nbuf_clone_with_priv((nbuf), MTLK_SLID)

static __INLINE void
__mtlk_nbuf_start_tracking (mtlk_nbuf_t *nbuf, mtlk_slid_t caller_slid)
{
#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
  if (__LIKELY(nbuf)) {
    mtlk_nbuf_priv_attach_stats(mtlk_nbuf_priv(nbuf));
  }
#endif
  mtlk_objpool_add_nbuf(nbuf, caller_slid);
}

#define mtlk_nbuf_start_tracking(nbuf) __mtlk_nbuf_start_tracking((nbuf), MTLK_SLID)

static __INLINE void mtlk_nbuf_stop_tracking(mtlk_nbuf_t *nbuf)
{
  mtlk_objpool_remove_nbuf(nbuf);
}

#endif /* __DF_NBUF_H__ */
