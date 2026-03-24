/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "work_serializer.h"
#include "unitest_helper.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <pthread.h>

static int __sum1 = 0;
static int __sum2 = 0;
static int __can_stop = 0;
static int __can_stop2 = 0;

#define SPECIAL_SIGNAL ((int)-85541236)
#define SPECIAL_SIGNAL2 ((int)-54531222)

typedef struct _integer_obj {
	int num;
} integer_obj;

static int do_work1(work_serializer *s, void *work_obj, void *ctx)
{
	integer_obj *work = (integer_obj*)work_obj;
	(void)ctx;
	(void)s;

	if (work->num == SPECIAL_SIGNAL) {
		TLOG("started work on the SPECIAL_SIGNAL")
		__can_stop = 1;
		return 0;
	}

	__sum1 += work->num;
	return 0;
}

static int do_work2(work_serializer *s, void *work_obj, void *ctx)
{
	integer_obj *work = (integer_obj*)work_obj;
	(void)ctx;
	(void)s;

	if (work->num == SPECIAL_SIGNAL2) {
		TLOG("started work on the SPECIAL_SIGNAL2")
		__can_stop2 = 1;
		return 0;
	}

	__sum2 += work->num;
	return 0;
}

static int clean_work_obj(void *work_obj, void *ctx)
{
	integer_obj *work = (integer_obj*)work_obj;
	(void)ctx;
	free(work);
	return 0;
}

enum {
	TEST_WORK_1,
	TEST_WORK_2,

	/* keep last */
	NUM_CMD_WORK_TYPES,
};

static work_ops_t cmd_work_ops[] = {
	[TEST_WORK_1] = { do_work1, clean_work_obj, NULL },
	[TEST_WORK_2] = { do_work2, clean_work_obj, NULL },
};

static int __num_work = 100000;
static void* work_pusher(void *obj)
{
	int i;
	work_serializer *serializer = (work_serializer*)obj;

	for (i = 0; i < __num_work; i++) {
		integer_obj *integer = (integer_obj*)malloc(sizeof(integer_obj));
		if (integer == NULL)
			return (void*)-1;

		integer->num = 1;
		if (serializer_exec_work_async(serializer, TEST_WORK_1, integer, NULL)) {
			ELOG("push work returned err i=%d", i);
			return (void*)-1;
		}
	}

	return NULL;
}

UNIT_TEST_DEFINE(1, create & destroy * N)
	work_serializer *serializer;
	int i;

	for (i = 0; i < 1000; i++) {
		serializer = serializer_create(cmd_work_ops, NUM_CMD_WORK_TYPES, 0);
		if (!serializer)
			UNIT_TEST_FAILED("serializer start failed i=%i", i)

		if (serializer_destroy(serializer))
			UNIT_TEST_FAILED("serializer stop returned err i=%i", i)
	}

UNIT_TEST_CLEANUP_ON_ERRR
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(2, do work * N times)
	work_serializer *serializer;
	integer_obj *integer, *stop_integer;
	int i, local_sum = 0;
	int sleep_count = 0;

	__sum1 = 0;
	serializer = serializer_create(cmd_work_ops, NUM_CMD_WORK_TYPES, 1);
	if (!serializer)
		UNIT_TEST_FAILED("serializer start failed")

	for (i = 0; i < 100000; i++) {
		local_sum += i;
		integer = (integer_obj*)malloc(sizeof(integer_obj));
		if (!integer)
			UNIT_TEST_FAILED("malloc");

		integer->num = i;
		if (serializer_exec_work_async(serializer, TEST_WORK_1, integer, NULL))
			UNIT_TEST_FAILED("push work returned err i=%d", i);
	}

	stop_integer = (integer_obj*)malloc(sizeof(integer_obj));
	if (!stop_integer)
		UNIT_TEST_FAILED("malloc");

	stop_integer->num = SPECIAL_SIGNAL;

	__can_stop = 0;
	if (serializer_exec_work_async(serializer, TEST_WORK_1, stop_integer, NULL))
		UNIT_TEST_FAILED("push stop work returned err");

	i = 0;
	while (!__can_stop) {
		usleep(1000);
		i++;

		if (++sleep_count > 6000)
			UNIT_TEST_FAILED("timeout");
	}

	ILOG( "local_sum=%d __sum1=%d i=%d", local_sum, __sum1, i);

	if (serializer_destroy(serializer))
		UNIT_TEST_FAILED("serializer stop returned err i=%i", i)

	if (local_sum != __sum1)
		UNIT_TEST_FAILED("local_sum(%d) != __sum1(%d)", local_sum, __sum1);

UNIT_TEST_CLEANUP_ON_ERRR
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(3, do work * N times with k serializers)
	work_serializer *serializer1, *serializer2;
	integer_obj *integer, *stop_integer;
	int i, local_sum = 0;
	int sleep_count = 0;

	__can_stop = 0;
	__can_stop2 = 0;
	__sum1 = 0;
	__sum2 = 0;
	serializer1 = serializer_create(cmd_work_ops, NUM_CMD_WORK_TYPES, 0);
	serializer2 = serializer_create(cmd_work_ops, NUM_CMD_WORK_TYPES, 1);
	if (!serializer1 || !serializer2)
		UNIT_TEST_FAILED("serializer start failed")

	for (i = 0; i < 100000; i++) {
		local_sum += i;
		integer = (integer_obj*)malloc(sizeof(integer_obj));
		if (!integer) UNIT_TEST_FAILED("malloc");

		integer->num = i;
		if (serializer_exec_work_async(serializer1, TEST_WORK_1, integer, NULL))
			UNIT_TEST_FAILED("push work returned err i=%d", i);

		local_sum += i;
		integer = (integer_obj*)malloc(sizeof(integer_obj));
		if (!integer) UNIT_TEST_FAILED("malloc");

		integer->num = i;
		if (serializer_exec_work_async(serializer2, TEST_WORK_2, integer, NULL))
			UNIT_TEST_FAILED("push work returned err i=%d", i);
	}

	stop_integer = (integer_obj*)malloc(sizeof(integer_obj));
	if (!stop_integer) UNIT_TEST_FAILED("malloc");

	stop_integer->num = SPECIAL_SIGNAL;
	if (serializer_exec_work_async(serializer1, TEST_WORK_1, stop_integer, NULL))
		UNIT_TEST_FAILED("push stop work returned err");

	stop_integer = (integer_obj*)malloc(sizeof(integer_obj));
	if (!stop_integer) UNIT_TEST_FAILED("malloc");

	stop_integer->num = SPECIAL_SIGNAL2;
	if (serializer_exec_work_async(serializer2, TEST_WORK_2, stop_integer, NULL))
		UNIT_TEST_FAILED("push stop work returned err");

	i = 0;
	while (!__can_stop || !__can_stop2) {
		usleep(1000);
		i++;

		if (++sleep_count > 6000)
			UNIT_TEST_FAILED("timeout");
	}

	ILOG( "local_sum=%d __sum1=%d __sum2=%d i=%d",
		local_sum, __sum1, __sum2, i);

	if (serializer_destroy(serializer1))
		UNIT_TEST_FAILED("serializer1 stop returned err")
	if (serializer_destroy(serializer2))
		UNIT_TEST_FAILED("serializer2 stop returned err")

	if (local_sum != __sum1 + __sum2)
		UNIT_TEST_FAILED("local_sum(%d) != __sum1+2(%d)",
				local_sum, __sum1 + __sum2);

UNIT_TEST_CLEANUP_ON_ERRR
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_DEFINE(4, from k threads: do work * N times)
	work_serializer *serializer;
	integer_obj *stop_integer;
	int i, local_sum = 0;
	pthread_t threads[4];
	void *result;
	int sleep_count = 0;

	serializer = serializer_create(cmd_work_ops, NUM_CMD_WORK_TYPES, 1);
	if (!serializer)
		UNIT_TEST_FAILED("serializer start failed")

	__sum1 = 0;
	__can_stop = 0;

	pthread_create(&threads[0], NULL, work_pusher, serializer);
	pthread_create(&threads[1], NULL, work_pusher, serializer);
	pthread_create(&threads[2], NULL, work_pusher, serializer);
	pthread_create(&threads[3], NULL, work_pusher, serializer);

	pthread_join(threads[0], &result);
	if (result != NULL)
		UNIT_TEST_FAILED("thread1 returned error");

	pthread_join(threads[1], &result);
	if (result != NULL)
		UNIT_TEST_FAILED("thread2 returned error");

	pthread_join(threads[2], &result);
	if (result != NULL)
		UNIT_TEST_FAILED("thread3 returned error");

	pthread_join(threads[3], &result);
	if (result != NULL)
		UNIT_TEST_FAILED("thread4 returned error");

	stop_integer = (integer_obj*)malloc(sizeof(integer_obj));
	if (!stop_integer) UNIT_TEST_FAILED("malloc");

	stop_integer->num = SPECIAL_SIGNAL;
	if (serializer_exec_work_async(serializer, TEST_WORK_1, stop_integer, NULL))
		UNIT_TEST_FAILED("push stop work returned err");

	i = 0;
	while (!__can_stop) {
		usleep(1000);
		i++;

		if (++sleep_count > 6000)
			UNIT_TEST_FAILED("timeout");
	}

	local_sum = 4 * __num_work;
	ILOG( "local_sum=%d __sum1=%d i=%d", local_sum, __sum1, i);

	if (serializer_destroy(serializer))
		UNIT_TEST_FAILED("serializer stop returned err i=%i", i)

	if (local_sum != __sum1)
		UNIT_TEST_FAILED("local_sum(%d) != __sum1(%d)", local_sum, __sum1);

UNIT_TEST_CLEANUP_ON_ERRR
UNIT_TEST_DEFINITION_DONE

UNIT_TEST_MODULE_DEFINE(work_serializer)
	ADD_TEST(1)
	ADD_TEST(2)
	ADD_TEST(3)
	ADD_TEST(4)
UNIT_TEST_MODULE_DEFINITION_DONE
