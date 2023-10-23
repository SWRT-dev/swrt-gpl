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
#include <linux/vmalloc.h>

void *__mtlk_kmalloc(size_t size, int flags);
void  __mtlk_kfree(void *p);

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

#include "mem_leak.h"

/* Special versions for objpool */
static __INLINE void *
malloc_objpool (size_t size, unsigned int tag)
{
  return kmalloc(size, GFP_ATOMIC);
}

static __INLINE void
free_objpool (void *buffer)
{
  kfree(buffer);
}

/* Normal allocator that must be monitored by objpool */
static __INLINE void *
_kmalloc_tag (size_t size, gfp_t gfp, unsigned int tag, mtlk_slid_t caller_slid)
{
  void *buf = kmalloc(mem_leak_get_full_allocation_size(size), gfp);
  return mem_leak_handle_allocated_buffer(buf, size, caller_slid);
}

static __INLINE void 
_kfree_tag (void *buffer)
{
  void *buf = mem_leak_handle_buffer_to_free(buffer);
  kfree(buf);
}

#define kmalloc_tag(size, gfp, tag) _kmalloc_tag((size), (gfp), (tag), MTLK_SLID)
#define kfree_tag(buffer)           _kfree_tag(buffer)

static __INLINE void *
_vmalloc_tag (size_t size, unsigned int tag, mtlk_slid_t caller_slid)
{
  void *buf = vmalloc(mem_leak_get_full_allocation_size(size));
  return mem_leak_handle_allocated_buffer(buf, size, caller_slid);
}

static __INLINE void
_vfree_tag (void *buffer)
{
  void *buf = mem_leak_handle_buffer_to_free(buffer);
  vfree(buf);
}

#define vmalloc_tag(size, tag)      _vmalloc_tag((size), (tag), MTLK_SLID)
#define vfree_tag(buffer)           _vfree_tag(buffer)

#else /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */

#define kmalloc_tag(a, b, c) __mtlk_kmalloc((a), (b))
#define kfree_tag(p)         __mtlk_kfree((p))

#define vmalloc_tag(a, b)    vmalloc((a))
#define vfree_tag(p)         vfree((p))

#endif /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */

#endif /* __MEM_DEFS_H__ */
