/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MEM_DEFS_H__
#define __MEM_DEFS_H__

#include "memtags.h"

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

#include "mem_leak.h"

/* Special versions for objpool */
static __INLINE void *
malloc_objpool (size_t size, unsigned int tag)
{
  return malloc(size);
}

static __INLINE void
free_objpool (void *buffer)
{
  free(buffer);
}

/* Normal allocator that must be monitored by objpool */
static __INLINE void *
_malloc_tag (size_t size, unsigned int tag, mtlk_slid_t caller_slid)
{
  void *buf = malloc(mem_leak_get_full_allocation_size(size));
  return mem_leak_handle_allocated_buffer(buf, size, caller_slid);
}

static __INLINE void 
_free_tag (void *buffer)
{
  void *buf = mem_leak_handle_buffer_to_free(buffer);
  free(buf);
}

#define malloc_tag(size, tag) _malloc_tag((size), (tag), MTLK_SLID)
#define free_tag(buffer)      _free_tag(buffer)

#else /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */

#define malloc_tag(a, b)    malloc((a))
#define free_tag            free

#endif /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */

#endif /* __MEM_DEFS_H__ */
