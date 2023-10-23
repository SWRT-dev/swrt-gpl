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
 * Clipboard between Core and DF
 *
 * Originally written by Andrii Tseglytskyi
 *
 */

#include "mtlkinc.h"
#include "mtlk_clipboard.h"
#include "mtlklist.h"

#define LOG_LOCAL_GID   GID_CLIPBOARD
#define LOG_LOCAL_FID   1

/* "clipboard" between DF UI and Core implementation */
struct _mtlk_clpb_t {
  mtlk_dlist_t data_list;
  mtlk_dlist_entry_t *iterator;

  MTLK_DECLARE_INIT_STATUS;
};

struct _mtlk_clpb_entry_t {
  mtlk_dlist_entry_t entry;
  void               *data;
  uint32             size;
};

/* steps for init and cleanup */
MTLK_INIT_STEPS_LIST_BEGIN(clpb)
  MTLK_INIT_STEPS_LIST_ENTRY(clpb, LIST_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(clpb, REWIND)
MTLK_INIT_INNER_STEPS_BEGIN(clpb)
MTLK_INIT_STEPS_LIST_END(clpb);

static void _mtlk_clpb_cleanup(mtlk_clpb_t *clpb)
{
  MTLK_ASSERT(NULL != clpb);
  MTLK_CLEANUP_BEGIN(clpb, MTLK_OBJ_PTR(clpb))
    MTLK_CLEANUP_STEP(clpb, REWIND, MTLK_OBJ_PTR(clpb), 
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(clpb, LIST_INIT, MTLK_OBJ_PTR(clpb), 
                      mtlk_dlist_cleanup, (&clpb->data_list));
  MTLK_CLEANUP_END(clpb, MTLK_OBJ_PTR(clpb));
}

static int _mtlk_clpb_init(mtlk_clpb_t *clpb)
{
  MTLK_ASSERT(NULL != clpb);
  MTLK_INIT_TRY(clpb, MTLK_OBJ_PTR(clpb))
    MTLK_INIT_STEP_VOID(clpb, LIST_INIT, MTLK_OBJ_PTR(clpb),
                       mtlk_dlist_init, (&clpb->data_list));
    MTLK_INIT_STEP_VOID(clpb, REWIND, MTLK_OBJ_PTR(clpb),
                       mtlk_clpb_enum_rewind, (clpb));
  MTLK_INIT_FINALLY(clpb, MTLK_OBJ_PTR(clpb))    
  MTLK_INIT_RETURN(clpb, MTLK_OBJ_PTR(clpb), _mtlk_clpb_cleanup, (clpb))
}

mtlk_clpb_t* __MTLK_IFUNC
mtlk_clpb_create(void)
{
  mtlk_clpb_t *clpb;

  if (NULL == (clpb = mtlk_osal_mem_alloc(sizeof(mtlk_clpb_t), MTLK_MEM_TAG_CLPB))) {
    ELOG_V("Can't allocate clipboard structure");
    goto err_no_mem;
  }
  memset(clpb, 0, sizeof(mtlk_clpb_t));

  if (MTLK_ERR_OK != _mtlk_clpb_init(clpb)) {
    ELOG_V("Can't init clipboard structure");
    goto err_init_clpb;
  }
  return clpb;

err_init_clpb:
  mtlk_osal_mem_free(clpb);
err_no_mem:
  return NULL;
}

void __MTLK_IFUNC
mtlk_clpb_delete(mtlk_clpb_t *clpb)
{
  MTLK_ASSERT(NULL != clpb);

  mtlk_clpb_purge(clpb);

  mtlk_dlist_cleanup(&clpb->data_list);

  mtlk_osal_mem_free(clpb);
}

void __MTLK_IFUNC
mtlk_clpb_purge(mtlk_clpb_t *clpb)
{
  mtlk_dlist_entry_t *lentry;
  struct _mtlk_clpb_entry_t *clpb_entry;

  MTLK_ASSERT(NULL != clpb);

  while(NULL != (lentry = mtlk_dlist_pop_front(&clpb->data_list))) {
    clpb_entry = MTLK_CONTAINER_OF(lentry, struct _mtlk_clpb_entry_t, entry);
    mtlk_osal_mem_free(clpb_entry->data);
    mtlk_osal_mem_free(clpb_entry);
  }
  mtlk_clpb_enum_rewind(clpb);
}

/*

  This function will pass data buffer to clipboard.
  Caller can pass only one of two buffer pointers: data_src
  or data_preallocated. Other pointer must be NULL.

  When data_src is used, the function will allocate new buffer and copy data
  from data_src to that buffer. Buffer pointed by data_src always remains
  owned by caller.

  When data_preallocated is specified, the function will NOT allocate new
  data buffer. The buffer pointed by data_preallocated will be used by
  clipboard directly. When function returns MTLK_ERR_OK, the buffer will be
  owned by clipboard. The caller has no control over it anymore.
  When function returns MTLK_ERR_NO_MEM, the caller still owns the buffer and is
  responsible for memory deallocation.

  This function is not intended to be used directly. Please use macros
  "mtlk_clpb_push" and "mtlk_clpb_push_nocopy" where appropriate.

*/

int __MTLK_IFUNC
mtlk_clpb_push_ex(mtlk_clpb_t *clpb,
                  const void *data_src,
                  void *data_preallocated,
                  uint32 size)
{
  struct _mtlk_clpb_entry_t *clpb_entry;

  MTLK_ASSERT(NULL != clpb);
  MTLK_ASSERT((NULL != data_src) ^ (NULL != data_preallocated));

  if (NULL == (clpb_entry = mtlk_osal_mem_alloc(sizeof(struct _mtlk_clpb_entry_t),
                                                MTLK_MEM_TAG_CLPB))) {
    ELOG_V("Can't allocate clipboard entry");
    goto err_no_mem_for_entry;
  }

  if(data_src) {
    if (NULL == (clpb_entry->data = mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_CLPB))) {
      ELOG_V("Can't allocate clipboard data");
      goto err_no_mem_for_data;
    }
    memmove(clpb_entry->data, data_src, size);
  } else {
    clpb_entry->data = data_preallocated;
  }
  clpb_entry->size = size;

  mtlk_dlist_push_back(&clpb->data_list, &clpb_entry->entry);
  return MTLK_ERR_OK;

err_no_mem_for_data:
  mtlk_osal_mem_free(clpb_entry);
err_no_mem_for_entry:
  return MTLK_ERR_NO_MEM;
}

/* API for enumeration of "clipboard" */
void __MTLK_IFUNC
mtlk_clpb_enum_rewind(mtlk_clpb_t *clpb)
{
  MTLK_ASSERT(NULL != clpb);
  clpb->iterator = mtlk_dlist_head(&clpb->data_list);
}

void* __MTLK_IFUNC
mtlk_clpb_enum_get_next(mtlk_clpb_t *clpb, uint32* size)
{
  MTLK_ASSERT(NULL != clpb);

  clpb->iterator = mtlk_dlist_next(clpb->iterator);

  if (mtlk_dlist_head(&clpb->data_list) != clpb->iterator) {
    if(NULL != size)
    {
      *size = MTLK_CONTAINER_OF(clpb->iterator, 
                                struct _mtlk_clpb_entry_t,
                                entry)->size;
    }
    return MTLK_CONTAINER_OF(clpb->iterator, 
                             struct _mtlk_clpb_entry_t,
                             entry)->data;
  }
  
  return NULL;
}

uint32 __MTLK_IFUNC
mtlk_clpb_get_num_of_elements(mtlk_clpb_t *clpb)
{
  MTLK_ASSERT(NULL != clpb);
  return mtlk_dlist_size(&clpb->data_list);
}

BOOL __MTLK_IFUNC
mtlk_clpb_check_data (void *clpb_data, size_t data_size, size_t expected_size, const char* fn_name, int n_line)
{
  if (__UNLIKELY(NULL == clpb_data)) {
    ELOG_SD("%s(%d): failed to get data from clipboard", fn_name, n_line);
    return FALSE;
  }
  if (__UNLIKELY(data_size != expected_size)) {
    ELOG_SDDD("%s(%d): incorrect clipboard data size %u (expected:%u)", fn_name, n_line, data_size, expected_size);
    return FALSE;
  }
  return TRUE;
}
