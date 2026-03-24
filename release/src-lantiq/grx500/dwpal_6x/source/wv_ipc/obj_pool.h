/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __OBJ_POOL__H__
#define __OBJ_POOL__H__

#include <stddef.h>
#include <stdint.h>

typedef struct _obj_pool obj_pool;

/* Define OBJPOOL_DEBUG for extended debug printouts */
/* #define OBJPOOL_DEBUG */

#define OBJ void*

size_t obj_pool_destroy(obj_pool *pool);

obj_pool* obj_pool_init(const char *pool_name, size_t object_byte_size,
			size_t min_size, size_t max_size,
			int thread_safety_needed);

OBJ obj_pool_alloc_object(obj_pool *pool);

void obj_pool_put_object(obj_pool *pool, OBJ object);

#endif /* __OBJ_POOL__H__ */
