/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/*
 * $Id$
 */

#include "mtlkinc.h"
#include "mtlk_fast_mem.h"
#include "mem_leak.h"
#include "memdefs.h"

#if defined(CONFIG_ARCH_STR9100) && defined(CONFIG_CPU_DSPAD_ENABLE)
#include <linux/str9100/str9100_dspad.h>
#endif

#define LOG_LOCAL_GID   GID_MTLK_FAST_MEM
#define LOG_LOCAL_FID   1

void* __MTLK_IFUNC
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
_mtlk_fast_mem_alloc_objpool(uint32           caller_id, 
                             size_t           size,
                             mtlk_slid_t      caller_slid)
#else
mtlk_fast_mem_alloc(uint32 caller_id, size_t size)
#endif
{
  void *result;
  uint32 is_in_fast_pool;
  size_t payload_size;
  size_t full_allocation_size;
  
  MTLK_ASSERT(0 != size);

  payload_size = size + sizeof(uint32);
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
  full_allocation_size = mem_leak_get_full_allocation_size(payload_size);
#else
  full_allocation_size = payload_size;
#endif

#if defined(CONFIG_ARCH_STR9100) && defined(CONFIG_CPU_DSPAD_ENABLE)
  if(0 != caller_id)
  {
    result = str9100_dspad_alloc(full_allocation_size);
    is_in_fast_pool = 1;
  }
  else
#endif
  {
    result = __mtlk_kmalloc(full_allocation_size, GFP_ATOMIC);
    is_in_fast_pool = 0;
  }

  if(NULL == result)
  {
    return NULL;
  }

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
  result = mem_leak_handle_allocated_buffer(result, payload_size,
                                            caller_slid);
#endif

  ((uint32*) result)[0] = is_in_fast_pool;

  return &((uint32*) result)[1];
}

void __MTLK_IFUNC
mtlk_fast_mem_free(void* addr)
{
  void* orig_addr;

#if defined(CONFIG_ARCH_STR9100) && defined(CONFIG_CPU_DSPAD_ENABLE)
  uint32 is_in_fast_pool;
#endif

  MTLK_ASSERT(NULL != addr);
  
  orig_addr = ((uint32*) addr) - 1;
#if defined(CONFIG_ARCH_STR9100) && defined(CONFIG_CPU_DSPAD_ENABLE)
  is_in_fast_pool = ((uint32*) orig_addr)[0];
#endif

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
  orig_addr = mem_leak_handle_buffer_to_free(orig_addr);
#endif

#if defined(CONFIG_ARCH_STR9100) && defined(CONFIG_CPU_DSPAD_ENABLE)
  if(is_in_fast_pool)
  {
    str9100_dspad_free(orig_addr);
  }
  else
#endif
  {
    __mtlk_kfree(orig_addr);
  }
}

void __MTLK_IFUNC
mtlk_fast_mem_print_info(void)
{
#if defined(CONFIG_ARCH_STR9100) && defined(CONFIG_CPU_DSPAD_ENABLE)
  ILOG0_V("Using D-scratchpad for hot context");
#else
  ILOG0_V("Using normal memory for hot context");
#endif
}
