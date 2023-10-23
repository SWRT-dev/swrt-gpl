/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __UNITEST_HELPER__H__
#define __UNITEST_HELPER__H__

#include <sys/time.h>
#include <time.h>
#include <sys/wait.h>
#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

static inline int is_running_in_valgrind(void)
{
	char *ld_preload_env = getenv("LD_PRELOAD");

	return (!ld_preload_env || !strstr(ld_preload_env, "valgrind")) ? 0 : 1;
}

#define _BCYN "\033[1;36m"
#define __MAG "\033[0;35m"
#define __RED "\033[0;31m"
#define __GRN "\033[0;32m"
#define __YLW "\033[0;33m"
#define _BBLU "\033[1;34m"
#define __RST "\033[0m"

#define ILOG(fmt, ...) do { printf("%s(%d): " __RST fmt __RST "\n", __func__, __LINE__, ##__VA_ARGS__); } while (0);
#define WLOG(fmt, ...) do { printf("%s(%d): " __YLW fmt __RST "\n", __func__, __LINE__, ##__VA_ARGS__); } while (0);
#define ELOG(fmt, ...) do { printf("%s(%d): " __RED fmt __RST "\n", __func__, __LINE__, ##__VA_ARGS__); } while (0);
#define SLOG(fmt, ...) do { printf("%s(%d): " __GRN fmt __RST "\n", __func__, __LINE__, ##__VA_ARGS__); } while (0);
#define DLOG(fmt, ...) do { printf("%s(%d): " _BCYN fmt __RST "\n", __func__, __LINE__, ##__VA_ARGS__); } while (0);
#define TLOG(fmt, ...) do { printf("%s(%d): " __MAG fmt __RST "\n", __func__, __LINE__, ##__VA_ARGS__); } while (0);

/* TIMING HELPER */

#define CALC_TIMESPEC_DIFF(__srt, __stp, __res) {				\
    if ((__stp.tv_nsec - __srt.tv_nsec) < 0) {					\
        __res.tv_sec = __stp.tv_sec - __srt.tv_sec - 1;				\
        __res.tv_nsec = __stp.tv_nsec - __srt.tv_nsec + 1000000000L;		\
    } else {									\
        __res.tv_sec = __stp.tv_sec - __srt.tv_sec;				\
        __res.tv_nsec = __stp.tv_nsec - __srt.tv_nsec;				\
    }										\
}

/* MODULE UNIT TEST DEFINES */

typedef int (*test_fnc)(void);

#define UNIT_TEST_MODULE_DEFINE(__module_name) 					\
int unit_test_module_##__module_name(char *tests_str);				\
int unit_test_module_##__module_name(char *tests_str) {				\
	test_fnc __tests[100] = { NULL };					\
	int __num_failed = 0, __tests_ran = 0;					\
	struct timespec __start_t, __end_t, __result_t;				\
	clock_gettime(7, &__start_t);						\
	DLOG("### Starting module: '%s' Unit Testing", #__module_name);

#define ADD_TEST(__test_id)							\
	__tests[__test_id] = unit_test_number_##__test_id;

#define EXECUTE_ALL_TESTS							\
	{									\
		unsigned int i;								\
		for (i = 0; i < sizeof(__tests) / sizeof(__tests[0]); i++) {	\
			if (__tests[i]) {					\
				__tests_ran++;					\
				__num_failed += __tests[i]();			\
			}							\
		}								\
	}

#define EXECUTE_TESTS_BASED_ON_INPUT						\
	{									\
		char *__start_b = strchr(tests_str, '{');			\
		char *__end_b;							\
		char *__next_c;							\
		if (!__start_b)	{						\
			ELOG("__start_b is NULL");				\
			return -1;						\
		}								\
		__end_b = strchr(__start_b, '}');				\
		if (!__end_b)	{						\
			ELOG("__end_b is NULL");				\
			return -1;						\
		}								\
		__start_b[0] = '\0';						\
		__start_b++;							\
		do {								\
			int __test_num = 0;					\
			__next_c = strchr(__start_b, ',');			\
			if (__next_c) {						\
				__next_c[0] = '\0';				\
				__test_num = atoi(__start_b);			\
				__start_b = __next_c + 1;			\
			} else {						\
				__end_b[0] = '\0';				\
				__test_num = atoi(__start_b);			\
			}							\
			if (__test_num < 100 && __tests[__test_num]) {		\
				__tests_ran++;					\
				__num_failed += __tests[__test_num]();		\
			}							\
		} while(__next_c);						\
	}

#define UNIT_TEST_MODULE_DEFINITION_DONE					\
	if (tests_str == NULL ||						\
	    !strchr(tests_str, '{') || !strchr(tests_str, '}')) {		\
		EXECUTE_ALL_TESTS						\
	} else {								\
		EXECUTE_TESTS_BASED_ON_INPUT					\
	}									\
	clock_gettime(7, &__end_t);						\
	CALC_TIMESPEC_DIFF(__start_t, __end_t, __result_t);			\
	ILOG("### Unit Testing finished. took %ld.%09ld seconds in total",	\
	     __result_t.tv_sec, __result_t.tv_nsec)				\
	ILOG( "### Performed %d tests.", __tests_ran)				\
	if (__num_failed < __tests_ran) {					\
		SLOG("### Passed %d Tests.", __tests_ran - __num_failed)	\
	}									\
	if (__num_failed) {							\
		ELOG("### Failed %d Tests.", __num_failed)			\
	} else if (!__tests_ran) {						\
		WLOG("### No tests were performed.")				\
	} else {								\
		SLOG("### All Tests passed!.")					\
	}									\
	printf("\n");								\
	return __num_failed;							\
}

/* SINGLE TEST DEFINES */

#define UNIT_TEST_DEFINE(__test_id, __test_name)				\
static int unit_test_number_##__test_id(void) {					\
	char *__test_name_str = #__test_name;					\
	struct timespec __start_t, __end_t, __result_t;				\
	struct timespec __h_timing_start;					\
	int __test_failed = 0;							\
	int __fork_pid = -1, __did_fork = 0;					\
	clock_gettime(7, &__start_t);						\
	ILOG("# Test id: '%d' name: " _BBLU "'%s'" __RST " started!",		\
	     __test_id, __test_name_str)

#define UNIT_TEST_TIMER_START							\
	clock_gettime(7, &__h_timing_start);					\

#define UNIT_TEST_TIMER_STOP(__segm_name)					\
	{									\
		struct timespec ___result, __h_timing_end;			\
		clock_gettime(7, &__h_timing_end);				\
		CALC_TIMESPEC_DIFF(__h_timing_start, __h_timing_end, ___result);\
		ILOG(__YLW "# segment [" #__segm_name "] took %ld.%09ld seconds" __RST,	\
		     ___result.tv_sec, ___result.tv_nsec)			\
	}

#define UNIT_TEST_FAILED(fmt, ...) {						\
	ELOG(fmt, ##__VA_ARGS__)						\
	__test_failed = 1;							\
	goto __clean_up;							\
}

#define UNIT_TEST_FORK								\
	__fork_pid = fork();							\
	if (__fork_pid == -1) {							\
		UNIT_TEST_FAILED("fork");					\
	} else if (__fork_pid == 0)						\
		goto __forked_child;						\
	else {									\
		__did_fork = 1;							\
		goto __forked_parent;						\
	}

#define UNIT_TEST_FORKED_CHILD							\
__forked_child:

#define UNIT_TEST_FORKED_PARENET						\
__forked_parent:

#define UNIT_TEST_CLEANUP_ON_ERRR						\
__clean_up:									\
	if (__test_failed == 0) goto __test_done;				\
	if (__did_fork) kill(__fork_pid, SIGKILL);

#define UNIT_TEST_DEFINITION_DONE						\
__test_done:									\
	if (__did_fork) {							\
		int __status = 0, __wait_res;					\
		do {								\
			usleep(1000);						\
			__wait_res = waitpid(__fork_pid, &__status, WNOHANG);	\
		} while (__wait_res == 0);					\
		ILOG("client exited with status=%d wait_res=%d",		\
		     __status, __wait_res);					\
		if (__status) {							\
			ELOG("forked child returned with err=%d", __status);	\
			__test_failed = 1;					\
		}								\
	}									\
	clock_gettime(7, &__end_t);						\
	CALC_TIMESPEC_DIFF(__start_t, __end_t, __result_t);			\
	ILOG( "# Test finished after %ld.%09ld seconds",			\
	     __result_t.tv_sec, __result_t.tv_nsec)				\
	if (__test_failed) {							\
		ELOG("# Test \"%s\" Failed!\n", __test_name_str)		\
	} else { 								\
		SLOG("# Test \"%s\" finished successfuly!\n", __test_name_str)	\
	}									\
	(void)__h_timing_start;							\
	return __test_failed;							\
}

#endif
