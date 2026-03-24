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
*/


#ifndef __MTLKLIST_DBG_H__
#define __MTLKLIST_DBG_H__

MTLK_DECLARE_OBJPOOL(g_objpool);

static __INLINE void mtlk_slist_init_debug(mtlk_slist_t *slist,
                                           mtlk_slid_t caller_slid)
{
  mtlk_slist_init(slist);
  mtlk_objpool_add_object(&g_objpool, &((slist)->objpool_ctx), MTLK_SLIST_OBJ, caller_slid);
}

static __INLINE void mtlk_slist_cleanup_debug(mtlk_slist_t *slist)
{
  mtlk_objpool_remove_object(&g_objpool, &((slist)->objpool_ctx), MTLK_SLIST_OBJ);
  mtlk_slist_cleanup(slist);
}

#define mtlk_slist_init(slist) \
  mtlk_slist_init_debug(slist, MTLK_SLID)

#define mtlk_slist_cleanup(slist) \
  mtlk_slist_cleanup_debug(slist)

static __INLINE void mtlk_lslist_init_debug(mtlk_lslist_t *lslist,
                                            mtlk_slid_t caller_slid)
{
  mtlk_lslist_init(lslist);
  mtlk_objpool_add_object(&g_objpool, &((lslist)->objpool_ctx), MTLK_LSLIST_OBJ, caller_slid);
}

static __INLINE void mtlk_lslist_cleanup_debug(mtlk_lslist_t *lslist)
{
  mtlk_objpool_remove_object(&g_objpool, &((lslist)->objpool_ctx), MTLK_LSLIST_OBJ);
  mtlk_lslist_cleanup(lslist);
}

#define mtlk_lslist_init(lslist) \
  mtlk_lslist_init_debug(lslist, MTLK_SLID)

#define mtlk_lslist_cleanup(lslist) \
  mtlk_lslist_cleanup_debug(lslist)

static __INLINE void mtlk_dlist_init_debug(mtlk_dlist_t *dlist,
                                           mtlk_slid_t caller_slid)
{
  mtlk_dlist_init(dlist);
  mtlk_objpool_add_object(&g_objpool, &((dlist)->objpool_ctx), MTLK_DLIST_OBJ, caller_slid);
}

static __INLINE void mtlk_dlist_cleanup_debug(mtlk_dlist_t *dlist)
{
  mtlk_objpool_remove_object(&g_objpool, &((dlist)->objpool_ctx), MTLK_DLIST_OBJ);
  mtlk_dlist_cleanup(dlist);
}

static __INLINE void mtlk_dlist_init_objpool(mtlk_dlist_t *dlist)
{
  return mtlk_dlist_init(dlist);
}

static __INLINE void mtlk_dlist_cleanup_objpool(mtlk_dlist_t *dlist)
{
  mtlk_dlist_cleanup(dlist);
}

#define mtlk_dlist_init(dlist) \
  mtlk_dlist_init_debug(dlist, MTLK_SLID)

#define mtlk_dlist_cleanup(dlist) \
  mtlk_dlist_cleanup_debug(dlist)

static __INLINE void mtlk_ldlist_init_debug(mtlk_ldlist_t *ldlist,
                                            mtlk_slid_t caller_slid)
{
  mtlk_ldlist_init(ldlist);
  mtlk_objpool_add_object(&g_objpool, &((ldlist)->objpool_ctx), MTLK_LDLIST_OBJ, caller_slid);
}

static __INLINE void mtlk_ldlist_cleanup_debug(mtlk_ldlist_t *ldlist)
{
  mtlk_objpool_remove_object(&g_objpool, &((ldlist)->objpool_ctx), MTLK_LDLIST_OBJ);
  mtlk_ldlist_cleanup(ldlist);
}

#define mtlk_ldlist_init(ldlist) \
  mtlk_ldlist_init_debug(ldlist, MTLK_SLID)

#define mtlk_ldlist_cleanup(ldlist) \
  mtlk_ldlist_cleanup_debug(ldlist)

#endif /* !__MTLKLIST_DBG_H__ */
