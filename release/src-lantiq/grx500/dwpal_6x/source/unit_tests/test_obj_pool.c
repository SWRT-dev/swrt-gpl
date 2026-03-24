/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "obj_pool.h"
#include "unitest_helper.h"

typedef struct _dummy_struct {
	int a;
	int b;
	int c;
	int d;
	int e;
} dummy_struct;

UNIT_TEST_DEFINE(1, alloc and put with max size)

	size_t *a[1000], *b;
	size_t i;
	obj_pool *pool;

	pool = obj_pool_init("objpool 1", sizeof(int), 8, sizeof(a) / sizeof(a[0]), 0);
	if (!pool)
		UNIT_TEST_FAILED("obj_pool_init retuned NULL");

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		a[i] = obj_pool_alloc_object(pool);
		if (!a[i])
			UNIT_TEST_FAILED("obj_pool_alloc_object retuned NULL, i=%zu", i);
		*a[i] = i;
	}

	b = obj_pool_alloc_object(pool);
	if (b)
		UNIT_TEST_FAILED("obj_pool_alloc_object didn't NULL");

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		if (*a[i] != i)
			UNIT_TEST_FAILED("*a[%zu] != i, *a[i]=%zu", i, *a[i]);
	}

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		obj_pool_put_object(pool, a[i]);
	}

	obj_pool_destroy(pool);

UNIT_TEST_CLEANUP_ON_ERRR
	if (pool)
		obj_pool_destroy(pool);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, alloc and put without size limit)

	dummy_struct *a[10000];
	size_t i;
	obj_pool *pool;

	pool = obj_pool_init("objpool 2", sizeof(dummy_struct), 16, 0, 0);
	if (!pool)
		UNIT_TEST_FAILED("obj_pool_init retuned NULL");

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		a[i] = obj_pool_alloc_object(pool);
		if (!a[i])
			UNIT_TEST_FAILED("obj_pool_alloc_object retuned NULL, i=%zu", i);
		a[i]->d = i;
	}

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		if (a[i]->d != (int)i)
			UNIT_TEST_FAILED("*a[i] != i, *a[i]=%d", a[i]->d);
	}

	for (i = 0; i < sizeof(a) / sizeof(a[0]); i++) {
		obj_pool_put_object(pool, a[i]);
	}

	obj_pool_destroy(pool);

UNIT_TEST_CLEANUP_ON_ERRR
	if (pool)
		obj_pool_destroy(pool);
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(obj_pool)
	ADD_TEST(1)
	ADD_TEST(2)
UNIT_TEST_MODULE_DEFINITION_DONE
