/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "obj_pool.h"

#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "logs.h"

#if defined(__GNUC__) && !defined(likely)
#define likely(x)       __builtin_expect(!!(x), 1)
#define unlikely(x)     __builtin_expect(!!(x), 0)
#else
#define likely(x)       (x)
#define unlikely(x)     (x)
#endif

#ifdef OBJPOOL_DEBUG
#define DEBUG(format, args...) LOG(2, "[%s()]  " format, __func__, args)
#else
#define DEBUG(format, args...)
#endif

#define DEBUG_VAR(format,var) DEBUG(#var ": " format, var)

#define container_of(ptr, type, member) ({			\
	const typeof(((type *)0)->member) * __mptr = (ptr);	\
	(type *)((char *)__mptr - offsetof(type, member)); })

typedef struct __attribute__((__packed__)) _pooled_obj {
	struct _pooled_obj *next;
	struct _obj_pool *pool;
	uint8_t allocated;
	uint8_t data[];
} pooled_obj;

struct _obj_pool {
	pooled_obj *available_objects;
	const char* name;

	size_t object_byte_size;
	size_t min_size;
	size_t max_size;
	size_t curr_size;

	size_t num_traveling_objects;

	int thread_safety_needed;
	pthread_mutex_t alloc_lock;
};

size_t obj_pool_destroy(obj_pool *pool)
{
	struct _pooled_obj *obj;
	size_t ret;

	if (pool == NULL) return 0;

#ifdef OBJPOOL_DEBUG
	int counter = 1;
#endif
	DEBUG("[pool:%s] destroy pool", pool->name);

	while ((obj = pool->available_objects)) {
		DEBUG("%i: pool:%s, pool->available_objects=%p, obj->next=%p",
		      counter++, pool->name, pool->available_objects, obj->next);
		pool->available_objects = obj->next;
		free(obj);
	}

	if (pool->thread_safety_needed)
		pthread_mutex_destroy(&pool->alloc_lock);

	ret = pool->num_traveling_objects;
	free(pool);

	return ret;
}

static int obj_pool_increase_size_to(obj_pool *pool, size_t new_size)
{
	size_t i, old_size = pool->curr_size;
#ifdef OBJPOOL_DEBUG
	int counter = 1;
#endif

	if (new_size <= old_size) return 1;
	if (pool->max_size)
		if (new_size > pool->max_size) {
			if (pool->max_size <= pool->curr_size)
				return 1;
			new_size = pool->max_size;
		}

	DEBUG("[pool:%s] old_size:%u, new_size:%u", pool->name, old_size, new_size);
	for (i = old_size; i < new_size; i++) {
		pooled_obj *obj = malloc(sizeof(pooled_obj) +
					 pool->object_byte_size);
		if (obj == NULL) return 1;

		obj->allocated = 0;
		obj->pool = pool;
		obj->next = pool->available_objects;
		pool->available_objects = obj;
		pool->curr_size++;
		DEBUG("%i: pool->available_objects:%p, obj->next:%p",
		      counter++, pool->available_objects, obj->next);
	}

	return 0;
}

obj_pool* obj_pool_init(const char *pool_name, size_t object_byte_size,
			size_t min_size, size_t max_size,
			int thread_safety_needed)
{
	obj_pool *pool = NULL;

	if (pool_name == NULL)
		return NULL;

	pool = (obj_pool*)malloc(sizeof(obj_pool));
	if (pool == NULL) return NULL;
	memset(pool, 0, sizeof(obj_pool));

	pool->name = pool_name;
	pool->object_byte_size = object_byte_size;
	pool->available_objects = NULL;
	pool->min_size = min_size;
	pool->max_size = max_size;
	pool->curr_size = 0;

	if (obj_pool_increase_size_to(pool, min_size))
		goto err;

	pool->thread_safety_needed = !!thread_safety_needed;
	if (pool->thread_safety_needed)
		pthread_mutex_init(&pool->alloc_lock, NULL);

	return pool;

err:
	obj_pool_destroy(pool);
	return NULL;
}

#define LOCK_ALLOCATION(__p, thread_safety_needed) \
if (thread_safety_needed) \
	{ pthread_mutex_lock(&__p->alloc_lock); }

#define UNLOCK_ALLOCATION(__p, thread_safety_needed) \
if (thread_safety_needed) \
	{ pthread_mutex_unlock(&__p->alloc_lock); }

OBJ obj_pool_alloc_object(obj_pool *pool)
{
	OBJ ret = NULL;
	int thread_safety_needed;

	if (pool == NULL) return NULL;

	thread_safety_needed = pool->thread_safety_needed;

	LOCK_ALLOCATION(pool, thread_safety_needed);
	if (pool->available_objects == NULL) {
		if (obj_pool_increase_size_to(pool, (int)((pool->curr_size * 3)/2) + 1))
			goto err;

		if (pool->available_objects == NULL)
			goto err;
	}

	DEBUG_VAR("%p", pool->available_objects);
	DEBUG_VAR("%p", pool->available_objects->next);

	ret = pool->available_objects->data;
	pool->available_objects->allocated = 1;
	pool->available_objects = pool->available_objects->next;
	pool->num_traveling_objects++;
	DEBUG("[pool:%s] object allocated", pool->name);

err:
	UNLOCK_ALLOCATION(pool, thread_safety_needed);
	return ret;
}

void obj_pool_put_object(obj_pool *pool, OBJ object)
{
	pooled_obj *p_obj = NULL;
	int thread_safety_needed;

	if (pool == NULL || object == NULL) return;

	thread_safety_needed = pool->thread_safety_needed;

	LOCK_ALLOCATION(pool, thread_safety_needed);
	p_obj = container_of(object, pooled_obj, data);

	DEBUG("[pool:%s] put object", pool->name);
	DEBUG_VAR("%p", p_obj);
	DEBUG_VAR("%p", p_obj->next);
	DEBUG_VAR("%p", pool->available_objects);
	DEBUG_VAR("%d", p_obj->allocated);

	if (unlikely(p_obj->pool != pool)) {
		BUG("This object doesn't belong to the pool '%s'", pool->name);
		goto failure;
	}

	if (unlikely(!p_obj->allocated)) {
		BUG("This object wasn't allocated yet");
		goto failure;
	}

	p_obj->allocated = 0;
	p_obj->next = pool->available_objects;
	pool->available_objects = p_obj;

	DEBUG_VAR("%p", p_obj->next);
	DEBUG_VAR("%p", pool->available_objects);

	pool->num_traveling_objects--;
failure:
	UNLOCK_ALLOCATION(pool, thread_safety_needed);
}
