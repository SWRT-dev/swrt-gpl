/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id: $
 *
 * 
 *  
 * Linked lists implementation for Linux driver:
 *   1. Singly-linked lists
 *   2. Thread-safe singly-linked lists
 */

#if !defined (SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DECLS)
#error "You shouldn't include this file directly!"
#endif /* SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_... */

#undef SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DECLS

#include <linux/list.h>

typedef struct _mtlk_slist_entry_t 
{
  struct _mtlk_slist_entry_t *next;
} mtlk_slist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_slist_entry_t head;
  u32                depth;
} mtlk_slist_t;

typedef mtlk_slist_entry_t mtlk_lslist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_slist_t      list;
  spinlock_t        lock;
} mtlk_lslist_t;

typedef struct list_head mtlk_dlist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_dlist_entry_t head;
  u32                depth;
} mtlk_dlist_t;

typedef mtlk_dlist_entry_t mtlk_ldlist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_dlist_t       list;
  spinlock_t         lock;
} mtlk_ldlist_t;
