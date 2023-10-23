/*
 * pput.h
 * Description: ppv4 driver definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only or BSD-3-Clause
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef PPUT_H_
#define PPUT_H_

#include <linux/init.h>
#include <linux/types.h>
#include <linux/bitops.h>

#define PP_UT_NAME_LEN_MAX (32)
#define PP_UT_PASS (1)
#define PP_UT_FAIL (0)

#define PP_UT_NUM_LEVELS       (3)
#define PP_UT_LEVEL_SANITY     BIT(0)
#define PP_UT_LEVEL_REGRESSION BIT(1)
#define PP_UT_LEVEL_FULL       (PP_UT_LEVEL_SANITY | PP_UT_LEVEL_REGRESSION)

#define PP_UT_LEVEL_MUST_PASS  BIT(16)
#define PP_UT_LEVEL_MUST_RUN   BIT(17)

/**
 * Componenet id
 */
enum test_component {
	PP_INFRA_TESTS,
	PP_SESSION_TESTS,
	PP_PORT_TESTS,
	PP_QOS_TESTS,
	PP_BM_TESTS,
	PP_PRSR_TESTS,
	PP_UT_NUM_COMPONENTS
};

struct pp_test_t {
	enum test_component component;
	char test_name[PP_UT_NAME_LEN_MAX];
	u32  level_bitmap;
	void *test_data;
	int  (*fn)(void *data);
};

int pput_init(struct dentry *parent);
void pput_exit(void);

/**
 * Register static tests to pput
 */
void cookie_tests_init(void);
void learning_tests_init(void);
void pp_infra_tests_init(void);
void pp_session_tests_init(void);
void pp_port_tests_init(void);
void pp_bm_tests_init(void);
void pp_qos_tests_init(void);
void pp_prsr_tests_init(void);

/**
 * Register a test to pput
 *
 * @param pp_test test to add
 *
 * @return int - zero for success
 */
extern int pp_register_test(struct pp_test_t *pp_test);

/**
 * Unregister a test from pput
 *
 * @param pp_test test to remove
 *
 * @return int - zero for success
 */
extern int pp_unregister_test(struct pp_test_t *pp_test);
#endif /* PPUT_H_ */
