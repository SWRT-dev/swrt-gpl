/*
 * pp_infra_tests.c
 * Description: Packet Processor unit tests
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/skb_extension.h>
#include <linux/pp_api.h>
#include "pput.h"
#include "pp_common.h"
#include "infra.h"
#include "pp_regs.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "%s:%d: " fmt, __func__, __LINE__
#endif

static int pp_infra_field_get_test(void *data);
static int pp_infra_field_prep_test(void *data);
static int pp_infra_invalid_field_prep_test(void *data);
static int pp_infra_field_mod_test(void *data);
static int pp_infra_invalid_field_mod_test(void *data);
static int infra_rx_hook_test(void *data);
static int infra_cookie_register_test(void *data);
static int infra_buf_fld_get_test(void *data);
static int infra_buf_fld_set_test(void *data);

static struct pp_test_t pp_infra_tests[] = {
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "pp_infra_field_get_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data     = NULL,
		.fn           = pp_infra_field_get_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "pp_infra_field_prep_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data     = NULL,
		.fn           = pp_infra_field_prep_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "pp_infra_invalid_field_prep_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data     = NULL,
		.fn           = pp_infra_invalid_field_prep_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "pp_infra_field_mod_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data     = NULL,
		.fn           = pp_infra_field_mod_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "pp_infra_invalid_field_mod_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data     = NULL,
		.fn           = pp_infra_invalid_field_mod_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "infra_buf_fld_get_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data     = NULL,
		.fn           = infra_buf_fld_get_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "infra_buf_fld_set_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data     = NULL,
		.fn           = infra_buf_fld_set_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "infra_cookie_register_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data     = NULL,
		.fn           = infra_cookie_register_test,
	},
	{
		.component    = PP_INFRA_TESTS,
		.test_name    = "infra_rx_hook_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data     = NULL,
		.fn           = infra_rx_hook_test,
	},
};

static u32 __field_get_test(u32 val, u32 h, u32 l, u32 expected)
{
	u32 mask = GENMASK(h, l);
	u32 res  = PP_FIELD_GET(mask, val);

	if (res != expected) {
		pr_err("Fail: val %x,mask %x,exp %x,res %x\n",
		       val, mask, expected, res);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Shortcut macro to test the functionality of PP_FIELD_GET and
 *        PP_FIELD_PREP, both are tested with mask as a constant and
 *        as a variable.
 */
#define __FIELD_GET_TEST(val, h, l, expected, ret, res)			       \
	({								       \
		/* check with GENMASK to use const version */		       \
		res = PP_FIELD_GET(GENMASK(h, l), val);			       \
		if (res != (expected)) {				       \
			pr_err("Fail: val %x,mask %lx,exp %x,res %x\n",	       \
				val, GENMASK(h, l), expected, res);	       \
			ret = PP_UT_FAIL;				       \
		}							       \
									       \
		/* check with mask as a variable to use variable version */    \
		if (__field_get_test(val, h, l, expected) != PP_UT_PASS)       \
			ret = PP_UT_FAIL;				       \
	})

/**
 * @brief Test the functionality of PP_FIELD_GET
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int pp_infra_field_get_test(void *data)
{
	u32 res, ret = PP_UT_PASS;

	__FIELD_GET_TEST(0xffff,      0,  0, 0x1,    ret, res);
	__FIELD_GET_TEST(0xf000,     12, 12, 0x1,    ret, res);
	__FIELD_GET_TEST(0xffff,      7,  0, 0xff,   ret, res);
	__FIELD_GET_TEST(0xf1f1,     11,  4, 0x1f,   ret, res);
	__FIELD_GET_TEST(0xabcde,     7,  0, 0xde,   ret, res);
	__FIELD_GET_TEST(0x12345678, 26, 12, 0x2345, ret, res);
	return ret;
}

static u32 __field_prep_test(u32 val, u32 h, u32 l, u32 expected)
{
	u32 mask = GENMASK(h, l);
	u32 res  = PP_FIELD_PREP(mask, val);

	if (res != expected) {
		pr_err("Fail: val %x,mask %x,exp %x,res %x\n",
		       val, mask, expected, res);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Shortcut macro to test the functionality of PP_FIELD_GET and
 *        PP_FIELD_PREP, both are tested with mask as a constant and
 *        as a variable.
 */
#define __FIELD_PREP_TEST(val, h, l, expected, ret, res)		       \
	({								       \
		/* check with GENMASK to use const version */		       \
		res = PP_FIELD_PREP(GENMASK(h, l), val);		       \
		if (res != (expected)) {				       \
			pr_err("Fail: val %x,mask %lx,exp %x,res %x\n",	       \
				val, GENMASK(h, l), expected, res);	       \
			ret = PP_UT_FAIL;				       \
		}							       \
									       \
		/* use function to use variable version */		       \
		if (__field_prep_test(val, h, l, expected) != PP_UT_PASS)      \
			ret = PP_UT_FAIL;				       \
	})

#define __FIELD_PREP_INVALID_TEST(val, h, l, expected, ret)		       \
	({								       \
		/* use function to use variable version */		       \
		if (__field_prep_test(val, h, l, expected) != PP_UT_PASS)      \
			ret = PP_UT_FAIL;				       \
	})

/**
 * @brief Test the functionality of PP_FIELD_PREP
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int pp_infra_field_prep_test(void *data)
{
	u32 res, ret = PP_UT_PASS;

	__FIELD_PREP_TEST(0xf1, 27, 20, 0xf100000, ret, res);
	__FIELD_PREP_TEST(1,    16, 16, 0x10000,   ret, res);
	__FIELD_PREP_TEST(0x1f,  9,  5, 0x3e0,     ret, res);
	return ret;
}

/**
 * @brief Test the functionality of PP_FIELD_PREP for invalid mask and
 *        value combinations
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int pp_infra_invalid_field_prep_test(void *data)
{
	u32 ret = PP_UT_PASS;

	__FIELD_PREP_INVALID_TEST(0xff,   5,  0, 0x0, ret);
	__FIELD_PREP_INVALID_TEST(7,     16, 15, 0x0, ret);
	__FIELD_PREP_INVALID_TEST(0xfff, 10,  0, 0x0, ret);

	return ret;
}

static u32 __field_mod_test(u32 dest, u32 val, u32 h, u32 l, u32 expected)
{
	u32 mask = GENMASK(h, l);
	u32 res  = PP_FIELD_MOD(mask, val, dest);
	u32 ret  = PP_UT_PASS;

	if (res != expected) {
		pr_err("Fail: dest %x,val %x,mask %x,exp %x,res %x\n",
		       dest, val, mask, expected, res);
		ret = PP_UT_FAIL;
	}

	return ret;
}
/**
 * @brief Shortcut macro to test the functionality of PP_FIELD_MOD, it
 *        is being tested with mask as a constant and as a variable.
 */
#define __FIELD_MOD_TEST(dest, val, h, l, expected, ret, res)		       \
	({								       \
		/* check with GENMASK to use const version */		       \
		res = PP_FIELD_MOD(GENMASK(h, l), val, dest);		       \
		if (res != (expected)) {				       \
			pr_err("Fail: dest %x,val %x,mask %lx,exp %x,res %x\n",\
				dest, val, GENMASK(h, l), expected, res);      \
			ret = PP_UT_FAIL;				       \
		}							       \
		/* check with mask as a variable to use variable version */    \
		if (__field_mod_test(dest, val, h, l, expected) != PP_UT_PASS) \
			ret = PP_UT_FAIL;				       \
	})

#define __FIELD_MOD_INVALID_TEST(dest, val, h, l, expected, ret)	       \
	({								       \
		/* check with mask as a variable to use variable version */    \
		if (__field_mod_test(dest, val, h, l, expected) != PP_UT_PASS) \
			ret = PP_UT_FAIL;				       \
	})

/**
 * @brief Test the functionality of FIELD_MOD
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int pp_infra_field_mod_test(void *data)
{
	u32 res, ret = PP_UT_PASS;

	__FIELD_MOD_TEST(0xff,   0xff, 27, 20, 0xff000ff, ret, res);
	__FIELD_MOD_TEST(0x123,  0x3,   3,  2, 0x12f,     ret, res);
	__FIELD_MOD_TEST(0x0,    0x1,  17, 17, 0x20000,   ret, res);
	__FIELD_MOD_TEST(0xfc3f, 0xf,   9,  6, 0xffff,    ret, res);

	return ret;
}

/**
 * @brief Test the functionality of FIELD_MOD invalid mask and
 *        value combinations
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int pp_infra_invalid_field_mod_test(void *data)
{
	u32 ret = PP_UT_PASS;

	__FIELD_MOD_INVALID_TEST(0xff00, 0xff,   5,  0, 0xff00, ret);
	__FIELD_MOD_INVALID_TEST(0xff,   7,     16, 15, 0xff,   ret);
	__FIELD_MOD_INVALID_TEST(0,      0xfff, 10,  0, 0x0,    ret);

	return ret;
}

enum test_fld {
	TEST_FLD_A,
	TEST_FLD_B,
	TEST_FLD_C,
	TEST_FLD_D,
	TEST_FLD_E,
	TEST_FLD_F,
	TEST_FLDS_NUM,
};

#define TEST_FLD_A_LSB		(0)
#define TEST_FLD_A_MSB		(7)
#define TEST_FLD_B_LSB		(8)
#define TEST_FLD_B_MSB		(15)
#define TEST_FLD_C_LSB		(16)
#define TEST_FLD_C_MSB		(39)
#define TEST_FLD_D_LSB		(40)
#define TEST_FLD_D_MSB		(59)
#define TEST_FLD_E_LSB		(60)
#define TEST_FLD_E_MSB		(103)
#define TEST_FLD_F_LSB		(104)
#define TEST_FLD_F_MSB		(127)

#define CHECK_FLD_VAL(fld, vals, exp_vals, res)                                \
	do {                                                                   \
		if ((vals)[fld] != (exp_vals)[fld]) {                          \
			pr_err("%s should be %#llx but actual val is %#llx\n", \
			       #fld, (exp_vals)[fld], (vals)[fld]);            \
			res = PP_UT_FAIL;                                      \
		}                                                              \
	} while (0)

/**
 * @brief Test functionality of buffer fields get API
 * @param data not used
 * @return int PP_UT_PASS in case the test pass, PP_UT_FAIL otherwise
 */
static int infra_buf_fld_get_test(void *data)
{
	struct buf_fld_info flds[TEST_FLDS_NUM];
	s64 expected_vals[TEST_FLDS_NUM];
	s64 vals[TEST_FLDS_NUM];
	u32 buf[4], i, res = PP_UT_PASS;

	/* create the fields dictionary */
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_A);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_B);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_C);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_D);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_E);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_F);

	/* create the buffer */
	buf[0] = 0x33221100;
	buf[1] = 0x77665544;
	buf[2] = 0xbbaa9988;
	buf[3] = 0xffeeddcc;

	/* set expected values */
	expected_vals[TEST_FLD_A] = 0x00;
	expected_vals[TEST_FLD_B] = 0x11;
	expected_vals[TEST_FLD_C] = 0x443322;
	expected_vals[TEST_FLD_D] = 0x76655;
	expected_vals[TEST_FLD_E] = 0xccbbaa99887;
	expected_vals[TEST_FLD_F] = 0xffeedd;

	/* extract fields from the buffer */
	for (i = 0; i < ARRAY_SIZE(vals); i++)
		vals[i] = buf_fld_get(&flds[i], buf);

	/* check values */
	CHECK_FLD_VAL(TEST_FLD_A, vals, expected_vals, res);
	CHECK_FLD_VAL(TEST_FLD_B, vals, expected_vals, res);
	CHECK_FLD_VAL(TEST_FLD_C, vals, expected_vals, res);
	CHECK_FLD_VAL(TEST_FLD_D, vals, expected_vals, res);
	CHECK_FLD_VAL(TEST_FLD_E, vals, expected_vals, res);
	CHECK_FLD_VAL(TEST_FLD_F, vals, expected_vals, res);

	if (res == PP_UT_FAIL)
		goto fail;

	return res;

fail:
	pr_err("Test fail\n");
	return res;
}

#define CHECK_BUF_VAL(buf, exp_buf, i, res)                                    \
	do {                                                                   \
		if ((buf)[i] != (exp_buf)[i]) {                                \
			pr_err("buf[%d] should be %#x but actual val is %#x\n",\
			       i, (exp_buf)[i], (buf)[i]);                     \
			res = PP_UT_FAIL;                                      \
		}                                                              \
	} while (0)

/**
 * @brief Test functionality of buffer fields get API
 * @param data not used
 * @return int PP_UT_PASS in case the test pass, PP_UT_FAIL otherwise
 */
static int infra_buf_fld_set_test(void *data)
{
	struct buf_fld_info flds[TEST_FLDS_NUM];
	s64 vals[TEST_FLDS_NUM];
	u32 buf[4], expected_buf[4], i, res = PP_UT_PASS;

	/* create the fields dictionary */
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_A);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_B);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_C);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_D);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_E);
	BUF_FLD_INIT_SIMPLE(flds, TEST_FLD_F);

	memset(buf, 0, sizeof(buf));
	/* create the expected buffer */
	expected_buf[0] = 0x33221100;
	expected_buf[1] = 0x77665544;
	expected_buf[2] = 0xbbaa9988;
	expected_buf[3] = 0xffeeddcc;

	/* set expected values */
	vals[TEST_FLD_A] = 0x00;
	vals[TEST_FLD_B] = 0x11;
	vals[TEST_FLD_C] = 0x443322;
	vals[TEST_FLD_D] = 0x76655;
	vals[TEST_FLD_E] = 0xccbbaa99887;
	vals[TEST_FLD_F] = 0xffeedd;

	/* set fields to the buffer */
	for (i = 0; i < ARRAY_SIZE(vals); i++)
		buf_fld_set(&flds[i], buf, vals[i]);

	/* check values */
	CHECK_BUF_VAL(buf, expected_buf, 0, res);
	CHECK_BUF_VAL(buf, expected_buf, 1, res);
	CHECK_BUF_VAL(buf, expected_buf, 2, res);
	CHECK_BUF_VAL(buf, expected_buf, 3, res);

	if (res == PP_UT_FAIL)
		goto fail;

	return res;

fail:
	pr_err("Test fail\n");
	return res;
}

/**
 * @brief Test if infra driver register PP and PS cookies
 *        successfully
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int infra_cookie_register_test(void *data)
{
	s32 pp_hnd;

	pp_hnd = skb_cookie_get_handle_by_name(PP_DESC_COOKIE_NAME);

	if (unlikely(pp_hnd == -EINVAL))
		return PP_UT_FAIL;

	return PP_UT_PASS;
}

/**
 * @brief Test if PP rx hook manage to fetch currectly PP and PS
 *        cookies including data validation
 * @return int PP_UT_PASS if test pass, PP_UT_FAIL otherwise
 */
static int infra_rx_hook_test(void *data)
{
	struct pp_desc *skb_desc;
	struct sk_buff *skb;
	u32             res = PP_UT_FAIL;

	skb = alloc_skb(2048, GFP_ATOMIC);
	if (unlikely(!skb)) {
		pr_err("Fail: skb allocation fail\n");
		return res;
	}

	memset(skb->head, 0, skb->truesize);
	(*(u32 *)skb->head) = 0xAAAAAAAA;

	skb->buf_base = skb->head;
	pp_rx_pkt_hook(skb);

	skb_desc = pp_pkt_desc_get(skb);
	if (unlikely(!skb_desc)) {
		pr_err("Fail: failed to get skb cookie\n");
		goto done;
	}

	if ((u64)skb_desc->ps != (u64)(*(u32 *)skb->head)) {
		pr_err("Fail: Found mismatch in PP cookie value: skb_desc->ps %llx, buff_ps %llx\n",
		       (u64)skb_desc->ps, (u64)(*(u32 *)skb->head));
		goto done;
	}

	res = PP_UT_PASS;
done:
	consume_skb(skb);

	return res;
}

/**
 * Register all pp_infra tests to pput
 */
void pp_infra_tests_init(void)
{
	int test_idx;

	/* Add Tests */
	for (test_idx = 0 ; test_idx < ARRAY_SIZE(pp_infra_tests); test_idx++)
		pp_register_test(&pp_infra_tests[test_idx]);
}
