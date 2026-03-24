/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "linked_list.h"
#include "logs.h"
#include "obj_pool.h"

#include <stdlib.h>
#include <string.h>

l_list * list_init(void)
{
	l_list *lst;

	if ((lst = malloc(sizeof(l_list))) == NULL)
		return NULL;

	memset(lst, 0, sizeof(l_list));
	lst->node_pool = obj_pool_init("list node", sizeof(node), 4, 0, 0);
	if (lst->node_pool == NULL) {
		free(lst);
		return NULL;
	}

	return lst;
}

void list_clear(l_list *lst)
{
	if (lst == NULL) return;

	list_foreach_start(lst, obj, void)
		(void)obj;
		list_foreach_remove_current_entry();
	list_foreach_end
}

void list_free(l_list *lst)
{
	size_t unfreed = 0;

	if (lst == NULL) return;

	list_clear(lst);
	unfreed = obj_pool_destroy(lst->node_pool);
	if (unfreed)
		BUG("obj pool has %zu unreturned objects", unfreed);
	free(lst);
}

int list_push_front(l_list *lst, void *obj)
{
	node *n;

	if (lst == NULL || obj == NULL) return 1;

	n = (node*)obj_pool_alloc_object(lst->node_pool);
	if (n == NULL) return 1;

	n->obj = obj;
	n->next = lst->first;
	lst->first = n;
	if (lst->last == NULL) lst->last = n;

	lst->size++;
	return 0;
}

int list_push_back(l_list *lst, void *obj)
{
	node *n;

	if (lst == NULL || obj == NULL) return 1;

	if (!lst->last)
		return list_push_front(lst, obj);

	n = (node*)obj_pool_alloc_object(lst->node_pool);
	if (n == NULL) return 1;

	n->obj = obj;
	n->next = NULL;

	lst->last->next = n;
	lst->last = n;

	lst->size++;
	return 0;
}

void* list_pop_front(l_list *lst)
{
	node *n;
	void *ret = NULL;

	if (lst == NULL || lst->first == NULL)
		return NULL;

	n = lst->first;

	lst->first = n->next;
	lst->size--;

	if (lst->first == NULL)
		lst->last = NULL;

	ret = n->obj;
	obj_pool_put_object(lst->node_pool, n);
	return ret;
}

void* list_peek_front(l_list *lst)
{
	if (lst == NULL || lst->first == NULL)
		return NULL;

	return lst->first->obj;
}

int list_remove(l_list *lst, void *obj)
{
	if (lst == NULL || obj == NULL) return 1;

	list_foreach_start(lst, iter_obj, void)
		if (iter_obj == obj) {
			list_foreach_remove_current_entry();
			return 0;
		}
	list_foreach_end

	return 1;
}

size_t list_get_size(l_list *l)
{
	if (l == NULL) return 0;

	return l->size;
}

void* list_find_first(l_list *lst, int(*cmp)(void *obj, void *arg), void *arg)
{
	if (lst == NULL || cmp == NULL) return NULL;


	list_foreach_start(lst, iter_obj, void)
		if (cmp(iter_obj, arg))
			return iter_obj;
	list_foreach_end

	return NULL;
}
