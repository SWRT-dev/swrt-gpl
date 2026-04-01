/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "mem_leak.h"
#include "mtlklist.h"

#define LOG_LOCAL_GID   GID_MEM_LEAK
#define LOG_LOCAL_FID   1

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL

MTLK_DECLARE_OBJPOOL(g_objpool);

/********************************************************************************
 * Private memory leaks debug related stuff
 *
 * Structure of memory chunks:
 *
 *    __ 1st cache line __                               __ last cache line __
 *   /                    \                             /                     \
 *  ---------------------------------------------------------------------------
 *  |  struct   |  front  |                payload               |    back    |
 *  |  mem_obj  |  guard  |  (size requested during allocation)  |    guard   |
 *  ---------------------------------------------------------------------------
 *
 *  The length if back guard is not lesser than MIN_BACK_GUARD_SIZE bytes. It always
 *  spans to the cache line boundary. Provided that MIN_BACK_GUARD_SIZE=4 and cache
 *  line size is 32 bytes the length of back guard is from 4 to 35 bytes.
 *
 ********************************************************************************/

struct mem_obj
{
  mtlk_objpool_context_t objpool_ctx;
  uint32 size;
  uint8  front_guard[1]; /* The actual length is FRONT_GUARD_LEN, to have the total struct size MTLK_MEM_ALLOC_ALIGN */
};

#define MIN_BACK_GUARD_SIZE (4)          /* The minimum number of back guard characters */
#define FREED_MEM_FILL_CHAR (0x0C)
#define FRONT_GUARD_CHAR    (0xF0)
#define BACK_GUARD_CHAR     (0xBA)

#define GET_GUARDED_BY_MEM(mem) ((uint8*)(mem) + MTLK_MEM_ALLOC_ALIGN)
#define GET_MEM_BY_GUARDED(buf) ((struct mem_obj *)((uint8*)(buf) - MTLK_MEM_ALLOC_ALIGN))

#define FRONT_GUARD_LEN         (MTLK_MEM_ALLOC_ALIGN - MTLK_OFFSET_OF(struct mem_obj, front_guard))
#define GET_FGUARD_POS(mem)     (mem->front_guard)
#define GET_BGUARD_POS(mem)     (((uint8*)mem) + MTLK_MEM_ALLOC_ALIGN + mem->size)

uint32 __MTLK_IFUNC
mem_leak_get_full_allocation_size (uint32 size)
{
  /* First MTLK_MEM_ALLOC_ALIGN include mem_dbg structure + forward guard butes
   * Then requested size follows up
   * And finally at least MIN_BACK_GUARD_SIZE back guard bytes */
  uint32 required_size = MTLK_MEM_ALLOC_ALIGN + size + MIN_BACK_GUARD_SIZE;
  return MTLK_ALIGN_SIZE(required_size); /* Round up to have a multiple of cache lines */
}

static __INLINE uint32
get_back_guard_len(struct mem_obj *mem)
{
  return mem_leak_get_full_allocation_size(mem->size) - MTLK_MEM_ALLOC_ALIGN - mem->size;
}

static void 
guards_set (struct mem_obj *mem)
{
  memset(GET_FGUARD_POS(mem), FRONT_GUARD_CHAR, FRONT_GUARD_LEN);
  memset(GET_BGUARD_POS(mem), BACK_GUARD_CHAR,  get_back_guard_len(mem));
}

static void 
guards_check (struct mem_obj *mem)
{
  mtlk_slid_t slid = mtlk_objpool_get_creator_slid(&g_objpool, &mem->objpool_ctx);
  uint8 *p, *p_end;

  MTLK_UNREFERENCED_PARAM(slid);

  /* Validate front guard integrity */
  p_end = (uint8*)mem + MTLK_MEM_ALLOC_ALIGN;
  for (p = GET_FGUARD_POS(mem); p < p_end; p++) {
    if (*p != FRONT_GUARD_CHAR) {
      ELOG_DDDPD("FGUARD corruption (G:%d F:%d L:%d, ptr: %p, size %u)",
                 mtlk_slid_get_gid(slid), mtlk_slid_get_fid(slid), mtlk_slid_get_lid(slid),
                 GET_GUARDED_BY_MEM(mem), mem->size);
      MTLK_ASSERT(FALSE);
      break;
    }
  }

  /* Validate back guard integrity */
  p_end = (uint8*)mem + mem_leak_get_full_allocation_size(mem->size);
  for (p = GET_BGUARD_POS(mem); p < p_end; p++) {
    if (*p != BACK_GUARD_CHAR) {
      ELOG_DDDPD("BGUARD corruption (G:%d F:%d L:%d, ptr: %p, size %u)",
                 mtlk_slid_get_gid(slid), mtlk_slid_get_fid(slid), mtlk_slid_get_lid(slid),
                 GET_GUARDED_BY_MEM(mem), mem->size);
      MTLK_ASSERT(FALSE);
      break;
    }
  }
}

void * __MTLK_IFUNC
mem_leak_handle_allocated_buffer (void *mem_dbg_buffer, uint32 size,
                                  mtlk_slid_t caller_slid)
{
  struct mem_obj *mem = (struct mem_obj *)mem_dbg_buffer;

  if (!mem) {
    return NULL;
  }

  mem->size = size;

  mtlk_objpool_add_object_ex(&g_objpool, &mem->objpool_ctx, MTLK_MEMORY_OBJ,
                             caller_slid, HANDLE_T(mem->size));
  guards_set(mem);

  ILOG5_PPD("%p (%p %d)", GET_GUARDED_BY_MEM(mem), mem, mem->size);

  return GET_GUARDED_BY_MEM(mem);
}

void * __MTLK_IFUNC
mem_leak_handle_buffer_to_free (void *buffer)
{
  struct mem_obj *mem;
 
  /* Sanity check for NULL pointer*/
  if (!buffer) return NULL;

  mem = GET_MEM_BY_GUARDED(buffer);
  ILOG5_PPD("%p (%p %d)", buffer, mem, mem->size);

  mtlk_objpool_remove_object_ex(&g_objpool, &mem->objpool_ctx, MTLK_MEMORY_OBJ, HANDLE_T(mem->size));
  guards_check(mem);
  memset(mem, FREED_MEM_FILL_CHAR, mem_leak_get_full_allocation_size(mem->size));

  return mem;
}

uint32 __MTLK_IFUNC
mem_leak_dbg_get_allocated_size (void)
{
  return mtlk_objpool_get_memory_allocated(&g_objpool);
}

struct mem_leak_dbg_printf_ctx
{
  mem_leak_dbg_printf_f printf_func;
  mtlk_handle_t         printf_ctx;
  uint32                total_size;
  uint32                total_count;
};

static BOOL __MTLK_IFUNC
_mem_leak_dbg_printf_allocator_clb (mtlk_objpool_t* objpool,
                                    mtlk_slid_t     allocator_slid,
                                    uint32          objects_number,
                                    uint32          additional_allocations_size,
                                    mtlk_handle_t   context)
{
  struct mem_leak_dbg_printf_ctx *ctx = 
    HANDLE_T_PTR(struct mem_leak_dbg_printf_ctx, context);

  ctx->printf_func(ctx->printf_ctx, "| %7u | %3u | G:%3d F:%2d L:%5d",
                   additional_allocations_size, objects_number,
                   mtlk_slid_get_gid(allocator_slid),
                   mtlk_slid_get_fid(allocator_slid),
                   mtlk_slid_get_lid(allocator_slid));

  ctx->total_size  += additional_allocations_size;
  ctx->total_count += objects_number;

  return TRUE;
}

void __MTLK_IFUNC
mem_leak_dbg_print_allocators_info (mem_leak_dbg_printf_f printf_func,
                                    mtlk_handle_t         printf_ctx)
{
  struct mem_leak_dbg_printf_ctx ctx;

  MTLK_ASSERT(printf_func != NULL);

  ctx.printf_func = printf_func;
  ctx.printf_ctx  = printf_ctx;
  ctx.total_count = 0;
  ctx.total_size  = 0;

  printf_func(printf_ctx, "Allocations dump:");
  printf_func(printf_ctx, "---------------------------------------------");
  printf_func(printf_ctx, "|  size   | cnt |        SLID");
  printf_func(printf_ctx, "---------------------------------------------");

  mtlk_objpool_enum_by_type(&g_objpool, MTLK_MEMORY_OBJ, _mem_leak_dbg_printf_allocator_clb,
                            HANDLE_T(&ctx));

  printf_func(printf_ctx, "=============================================");
  printf_func(printf_ctx, " Total: %u allocations = %u bytes",
              ctx.total_count, ctx.total_size);
  printf_func(printf_ctx, "=============================================");
}

#endif /* CPTCFG_IWLWAV_ENABLE_OBJPOOL */
