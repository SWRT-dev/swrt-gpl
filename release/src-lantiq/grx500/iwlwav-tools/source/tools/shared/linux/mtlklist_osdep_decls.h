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
 * Linked lists implementation for Linux driver:
 *   1. Singly-linked lists
 *   2. Thread-safe singly-linked lists
 */

#if !defined (SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DECLS)
#error "You shouldn't include this file directly!"
#endif /* SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_... */

#undef SAFE_PLACE_TO_INCLUDE_LIST_OSDEP_DECLS

typedef struct _mtlk_slist_entry_t 
{
  struct _mtlk_slist_entry_t *next;
} mtlk_slist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_slist_entry_t head;
  uint32             depth;
} mtlk_slist_t;

typedef mtlk_slist_entry_t mtlk_lslist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_slist_t         list;
  mtlk_osal_spinlock_t lock;
} mtlk_lslist_t;

typedef struct _mtlk_dlist_entry_t
{
  struct _mtlk_dlist_entry_t *prev;
  struct _mtlk_dlist_entry_t *next;
} mtlk_dlist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_dlist_entry_t head;
  uint32             depth;
} mtlk_dlist_t;

typedef mtlk_dlist_entry_t mtlk_ldlist_entry_t;

typedef struct
{
  MTLK_DECLARE_OBJPOOL_CTX(objpool_ctx);

  mtlk_dlist_t         list;
  mtlk_osal_spinlock_t lock;
} mtlk_ldlist_t;
