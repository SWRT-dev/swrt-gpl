/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __WAVE_LIST__H__
#define __WAVE_LIST__H__

#include "obj_pool.h"

#include <stddef.h>

typedef struct _node {
	void *obj;
	struct _node *next;
} node;

typedef struct _l_list {
	obj_pool *node_pool;
	node *first;
	node *last;
	size_t size;
} l_list;

l_list * list_init(void);

void list_free(l_list *lst);

void list_clear(l_list *lst);

int list_push_front(l_list *lst, void *obj);

int list_push_back(l_list *lst, void *obj);

void* list_pop_front(l_list *lst);

void* list_peek_front(l_list *lst);

int list_remove(l_list *lst, void *obj);

size_t list_get_size(l_list *lst);

void* list_find_first(l_list *lst, int(*cmp)(void *obj, void *arg), void *arg);

#define list_foreach_start(_list, _obj, _type) {			     \
	l_list *__list = _list;						     \
	int deleted = 0;						     \
	_type * _obj;							     \
	node *__prev = NULL, *__curr;					     \
	node __deleted __attribute__((unused)) ;			     \
	for (__curr = _list->first; __curr != NULL; __curr = __curr->next) { \
		_obj = (_type*)__curr->obj;				     \
		deleted = 0;

#define list_foreach_remove_current_entry() {			\
	__deleted.next = __curr->next;				\
								\
	if (__prev) __prev->next = __curr->next;		\
	else __list->first = __curr->next;			\
	if (__curr == __list->last) __list->last = __prev;	\
								\
	obj_pool_put_object(__list->node_pool, __curr);		\
	__list->size--;						\
	__curr = &__deleted;					\
	deleted = 1;						\
}

#define list_foreach_insert_before_current(new_entry) ({	\
	int __err = 1;						\
	node *__new_node =					\
		(node*)obj_pool_alloc_object(__list->node_pool);\
	if (__new_node) {					\
		__new_node->obj = new_entry;			\
		__new_node->next = __curr;			\
		if (__prev)					\
			__prev->next = __new_node;		\
		else						\
			__list->first = __new_node;		\
		__list->size++;					\
		__prev = __new_node;				\
		__err = 0;					\
	}							\
	__err;							\
})

#define list_foreach_end					\
		if (!deleted) { __prev = __curr; }		\
	}							\
	(void)__list;						\
	(void)__prev;						\
}

#endif /* __WAVE_LIST__H__ */
