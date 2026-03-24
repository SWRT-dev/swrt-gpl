/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "linked_list.h"
#include "unitest_helper.h"

UNIT_TEST_DEFINE(1, push and pop sanity)
	l_list *lst = NULL;
	int i, j;
	size_t size;
	int a[20000];

	lst = list_init();
	if (!lst)
		UNIT_TEST_FAILED("list_init retuned NULL");

	for (j = 0; j < 5; j++) {
		for (i = 0; i < (int)(sizeof(a) / sizeof(a[0])); i++) {
			a[i] = i;
			if (list_push_front(lst, (void*)&a[i]))
				UNIT_TEST_FAILED("list_push retuned err, i=%d, j=%d", i, j);
		}

		size = list_get_size(lst);
		if (size != sizeof(a) / sizeof(a[0]) || list_get_size(lst) != size)
			UNIT_TEST_FAILED("list_get_size retuned %zu, j=%d", size, j);

		for (i = (int)((sizeof(a) / sizeof(a[0])) - 1); i >= 0; i--) {
			int *b = (int*)list_pop_front(lst);
			if (!b)
				UNIT_TEST_FAILED("list_pop retuned NULL, i=%d, j=%d", i, j);
			if (*b != a[i])
				UNIT_TEST_FAILED("list_pop retuned wrong value, i=%d,j=%d, *b=%d", i, j, *b);
		}

		size = list_get_size(lst);
		if (size != 0 || list_get_size(lst) != size)
			UNIT_TEST_FAILED("list_get_size retuned %zu", size);
	}

	list_free(lst);

UNIT_TEST_CLEANUP_ON_ERRR
	if (lst)
		list_free(lst);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, push back)
	l_list *lst = list_init();
	size_t i, j;
	size_t size;
	int a[20000];

	if (!lst)
		UNIT_TEST_FAILED("list_init retuned NULL");

	for (j = 0; j < 5; j++) {
		for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
			a[i] = i;
			if (list_push_back(lst, (void*)&a[i]))
				UNIT_TEST_FAILED("list_push retuned err, i=%zu, j=%zu", i, j);
		}

		size = list_get_size(lst);
		if (size != sizeof(a) / sizeof(a[0]) || list_get_size(lst) != size)
			UNIT_TEST_FAILED("list_get_size retuned %zu, j=%zu", size, j);

		for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
			int *b = (int*)list_pop_front(lst);
			if (!b)
				UNIT_TEST_FAILED("list_pop retuned NULL, i=%zu, j=%zu", i, j);
			if (*b != a[i])
				UNIT_TEST_FAILED("list_pop retuned wrong value, i=%zu, j=%zu, *b=%d", i, j, *b);
		}

		size = list_get_size(lst);
		if (size != 0 || list_get_size(lst) != size)
			UNIT_TEST_FAILED("list_get_size retuned %zu, j=%zu", size, j);
	}

	list_free(lst);

UNIT_TEST_CLEANUP_ON_ERRR
	if (lst)
		list_free(lst);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(3, over pop)
	l_list *lst = list_init();
	size_t size;
	int a[] = { 1, 2, 3 };
	int *b;

	if (!lst)
		UNIT_TEST_FAILED("list_init retuned NULL");

	size = list_get_size(lst);
	ILOG( "size = %zu", size);
	if (size != 0 || list_get_size(lst) != size)
		UNIT_TEST_FAILED("list_get_size retuned %zu", size);

	if (list_push_front(lst, (void*)&a[0]))
		UNIT_TEST_FAILED("list_push retuned err");

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");

	if (*b != a[0])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (b)
		UNIT_TEST_FAILED("list_pop didn't retuned NULL");

	if (list_push_front(lst, (void*)&a[0]))
		UNIT_TEST_FAILED("list_push retuned err");

	if (list_push_front(lst, (void*)&a[1]))
		UNIT_TEST_FAILED("list_push retuned err");

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[1])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[0])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (b)
		UNIT_TEST_FAILED("list_pop didn't retuned NULL");

	if (list_push_front(lst, (void*)&a[0]))
		UNIT_TEST_FAILED("list_push retuned err");
	if (list_push_front(lst, (void*)&a[1]))
		UNIT_TEST_FAILED("list_push retuned err");
	if (list_push_front(lst, (void*)&a[2]))
		UNIT_TEST_FAILED("list_push retuned err");

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[2])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[1])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[0])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (b)
		UNIT_TEST_FAILED("list_pop didn't retuned NULL");

	if (list_push_back(lst, (void*)&a[0]))
		UNIT_TEST_FAILED("list_push retuned err");
	if (list_push_back(lst, (void*)&a[1]))
		UNIT_TEST_FAILED("list_push retuned err");
	if (list_push_back(lst, (void*)&a[2]))
		UNIT_TEST_FAILED("list_push retuned err");

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[0])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[1])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (!b)
		UNIT_TEST_FAILED("list_pop retuned NULL");
	if (*b != a[2])
		UNIT_TEST_FAILED("list_pop retuned wrong value, *b=%d", *b);

	b = (int*)list_pop_front(lst);
	if (b)
		UNIT_TEST_FAILED("list_pop didn't retuned NULL");

	list_free(lst);

UNIT_TEST_CLEANUP_ON_ERRR
	if (lst)
		list_free(lst);
UNIT_TEST_DEFINITION_DONE

static int int_cmp(void *obj, void *arg) {
	long *a = (long*)obj;
	long b = (long)arg;

	if (*a == b)
		return 1;

	return 0;
}

UNIT_TEST_DEFINE(4, check find function)

	l_list *lst = list_init();
	size_t i;
	long a[6000];

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++)
		a[i] = i;

	for (i = 0; i < (sizeof(a) / sizeof(a[0])) / 2; i++)
		if (list_push_front(lst, (void*)&a[i]))
			UNIT_TEST_FAILED("list_push retuned err, i=%zu", i);

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		long *b = (long*)list_find_first(lst, int_cmp, (void*)a[i]);
		if (i < (sizeof(a) / sizeof(a[0])) / 2) {
			if (b != &a[i])
				UNIT_TEST_FAILED("list_find_first retuned %p, i=%zu", b, i);
		} else if (b != NULL)
			UNIT_TEST_FAILED("list_find_first retuned %p, i=%zu", b, i);
	}

	list_free(lst);
UNIT_TEST_CLEANUP_ON_ERRR
	if (lst)
		list_free(lst);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(5, check remove function)
	l_list *lst = list_init();
	size_t i;
	size_t size;
	int a[7000];

	for (i = 0; i < (sizeof(a) / sizeof(a[0])) / 2; i++) {
		a[i] = i;
		if (list_push_front(lst, (void*)&a[i]))
			UNIT_TEST_FAILED("list_push retuned err, i=%zu", i);
	}

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		int b = list_remove(lst, (void*)&a[i]);
		if (i < (sizeof(a) / sizeof(a[0])) / 2) {
			if (b)
				UNIT_TEST_FAILED("list_remove retuned %d, i=%zu", b, i);
		} else if (!b)
			UNIT_TEST_FAILED("list_remove retuned %d, i=%zu", b, i);
	}

	size = list_get_size(lst);
	ILOG( "size = %zu", size);
	if (size != 0 || list_get_size(lst) != size)
		UNIT_TEST_FAILED("list_get_size retuned %zu", size);

	list_free(lst);
UNIT_TEST_CLEANUP_ON_ERRR
	if (lst)
		list_free(lst);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(linked_list)
	ADD_TEST(1)
	ADD_TEST(2)
	ADD_TEST(3)
	ADD_TEST(4)
	ADD_TEST(5)
UNIT_TEST_MODULE_DEFINITION_DONE
