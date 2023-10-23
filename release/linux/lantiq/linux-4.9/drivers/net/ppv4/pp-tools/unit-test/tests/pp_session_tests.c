/*
 * Description: Packet Processor unit tests
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_SESS_TEST] %s:%d: " fmt, __func__, __LINE__

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/random.h>
#include <linux/pktprs.h>
#include <linux/pp_api.h>
#include "../pput.h"
#include "uc_regs.h"
#include "pp_si.h"
#include "pp_common.h"
#include "pp_port_mgr.h"
#include "checker.h"
#include "pp_session_mgr.h"
#include "uc.h"

struct sgc_grp_test_info {
	u32 n_cntrs, owner;
	u8  idx;
};

/* number of clocks rate to test in the tbm tests */
#define NUM_CLOCKS 3

/* QoS port */
#define PP_SESSION_TEST_QOS_PORT (PP_QOS_MAX_PORTS - 1)

struct tbm_calc_data {
	u64 rate;
	u16 expected_mantissa[NUM_CLOCKS];
	u16 expected_exponent[NUM_CLOCKS];
};

/* clocks frequencies in MHz */
static u32 clocks[NUM_CLOCKS] = { 5U, 11U, 600U };

/* 1Gbps test data */
static struct tbm_calc_data data_1G = {
	.rate = 1000000000 / BITS_PER_BYTE,
	.expected_mantissa = { 3200, 2909, 3413 },
	.expected_exponent = { 3, 4, 10 },
};

/* 1Mbps test data */
static struct tbm_calc_data data_1M = {
	.rate = 1000000 / BITS_PER_BYTE,
	.expected_mantissa = { 3276, 2978, 3495 },
	.expected_exponent = { 13, 14, 20 },
};

static struct sgc_grp_test_info groups[PP_SI_SGC_MAX];
static u32 created_sessions[32];
static u32 *created_tdox_sessions;
static u32 pp_port = U32_MAX;
static u32 qos_queue = U32_MAX;
static u32 qos_port = U32_MAX;
struct pp_hash tdox_hash;

static int pp_si_encode_decode_test(void *data);
static int pp_sess_pre_test(void *data);
static int pp_sess_post_test(void *data);
static int pp_sess_delete_test(void *data);
static int pp_sess_create_test(void *unused);
static int pp_sgc_init_test(void *data);
static int pp_sgc_alloc_test(void *data);
static int pp_sgc_clean(void *data);
static int pp_sgc_sessions_list_test(void *data);
static int pp_sgc_mod_test(void *data);
static int pp_sgc_invalid_args_test(void *data);
static int pp_sgc_free_test(void *data);
static int pp_dual_tbm_alloc_test(void *data);
static int pp_dual_tbm_mod_test(void *data);
static int pp_dual_tbm_calc_test(void *data);
static int pp_dual_tbm_invalid_args_test(void *data);
static int pp_dual_tbm_free_test(void *data);
static int pp_tdox_max_session_test(void *unused);
static int pp_tdox_candidate_test(void *unused);
static int pp_tdox_nonqulaified_test(void *unused);
static int pp_tdox_session_remove_test(void *unused);

static struct pp_test_t pp_session_tests[] = {
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_si_encode_decode_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_si_encode_decode_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sess_pre_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data = NULL,
		.fn = pp_sess_pre_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sgc_init_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_sgc_init_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sgc_alloc_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_sgc_alloc_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_tbm_alloc_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_dual_tbm_alloc_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sess_create_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_sess_create_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sgc_sessions_list_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_sgc_sessions_list_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sess_delete_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_sess_delete_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sgc_modification_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_sgc_mod_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sgc_invalid_args_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_sgc_invalid_args_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_dual_tbm_modification_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_dual_tbm_mod_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_dual_1G_tbm_calc_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = &data_1G,
		.fn = pp_dual_tbm_calc_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_dual_1M_tbm_calc_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = &data_1M,
		.fn = pp_dual_tbm_calc_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_dual_tbm_invalid_args_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_dual_tbm_invalid_args_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_tdox_max_session_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_tdox_max_session_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_tdox_candidate_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_tdox_candidate_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_tdox_nonqulaified_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_tdox_nonqulaified_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_tdox_session_remove_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
		.test_data = NULL,
		.fn = pp_tdox_session_remove_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sess_post_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_RUN,
		.test_data = NULL,
		.fn = pp_sess_post_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sgc_free_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_RUN,
		.test_data = NULL,
		.fn = pp_sgc_free_test,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_sgc_clean",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_RUN,
		.test_data = NULL,
		.fn = pp_sgc_clean,
	},
	{
		.component = PP_SESSION_TESTS,
		.test_name = "pp_tbm_free_test",
		.level_bitmap = PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_RUN,
		.test_data = NULL,
		.fn = pp_dual_tbm_free_test,
	},
};

/**
 * @brief Sessions tests pre function to init all necessary stuff
 *        to create session
 * @param data
 * @return int
 */
static int pp_sess_pre_test(void *data)
{
	struct pp_qos_queue_conf queue_conf;
	struct pp_qos_port_conf port_conf;
	struct pp_qos_dev *qdev;
	struct pp_port_cfg cfg;

	/* find free pp port */
	for (pp_port = 0; pp_port < PP_MAX_PORT; pp_port++) {
		if (!pmgr_port_is_active(pp_port))
			break;
	}
	if (pp_port == PP_MAX_PORT) {
		pr_err("failed to find free pp port\n");
		return PP_UT_FAIL;
	}

	pr_debug("found non active pp port %u\n", pp_port);
	/* create PP port */
	memset(&cfg, 0, sizeof(struct pp_port_cfg));
	cfg.rx.flow_ctrl_en = 1;
	cfg.tx.max_pkt_size = 2000;
	cfg.tx.policies_map = (u8)BIT(0);
	cfg.tx.wr_desc = true;
	cfg.tx.headroom_size = 64;
	if (unlikely(pp_port_add(pp_port, &cfg))) {
		pr_err("pp_port_add failed, port%u\n", pp_port);
		return PP_UT_FAIL;
	}

	/* get qos device ref */
	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev))) {
		pr_err("failed to get qos dev\n");
		return PP_UT_FAIL;
	}

	/* allocate qos port */
	if (unlikely(pp_qos_port_allocate(qdev, PP_SESSION_TEST_QOS_PORT,
					  &qos_port))) {
		pr_err("failed to allocate qos port\n");
		return PP_UT_FAIL;
	}
	pp_qos_port_conf_set_default(&port_conf);
	port_conf.ring_address = 0xF4000000;
	port_conf.ring_size = 1;
	port_conf.packet_credit_enable = 1;
	port_conf.credit = 0x64;
	if (unlikely(pp_qos_port_set(qdev, qos_port, &port_conf))) {
		pr_err("failed to configure qos port %u\n", qos_port);
		return PP_UT_FAIL;
	}
	pr_debug("qos port %u allocated\n", qos_port);

	/* allocate qos queue */
	if (unlikely(pp_qos_queue_allocate(qdev, &qos_queue))) {
		pr_err("failed to allocate qos queue\n");
		return PP_UT_FAIL;
	}
	pp_qos_queue_conf_set_default(&queue_conf);
	queue_conf.queue_child_prop.parent = qos_port;
	queue_conf.wred_max_allowed = 0x400;
	if (unlikely(pp_qos_queue_set(qdev, qos_queue, &queue_conf))) {
		pr_err("failed to configure qos ueue %u\n", qos_queue);
		return PP_UT_FAIL;
	}
	pr_debug("qos queue %u allocated\n", qos_queue);

	created_tdox_sessions =
		kcalloc(UC_MAX_TDOX_SESSIONS * 2, sizeof(u32), GFP_KERNEL);

	return PP_UT_PASS;
}

/**
 * @brief Create a custom full SI structure
 */
static void __test_si_create1(struct pp_si *si)
{
	u32 i;

	si->sess_id           = 555;
	si->recipe_idx        = 50;
	si->fv_sz             = sizeof(si->fv);
	si->bce_ext           = false;
	si->si_ud_sz          = 8;
	si->si_ps_sz          = 0;
	si->ps_off            = PP_INVALID_PS_OFF;
	si->ext_df_mask       = true;
	si->int_df_mask       = true;
	si->base_policy       = 5;
	si->color             = PP_COLOR_YELLOW;
	si->tdox_flow         = 0;
	si->dst_q             = 0;
	si->eg_port           = 0;
	si->trim_l3_id        = 0;
	si->chck_flags        = SESS_CHCK_DFLT_FLAGS;
	si->pl2p              = false;
	si->tmpl_ud_sz        = 0;
	si->ps_copy           = 0;
	si->pkt_len_diff      = -10;
	si->sgc[0]            = 0;
	si->sgc[1]            = 1;
	si->sgc[2]            = 2;
	si->sgc[3]            = 3;
	si->sgc[4]            = 4;
	si->sgc[5]            = 5;
	si->sgc[6]            = 6;
	si->sgc[7]            = 3;
	si->sgc_en_map        = 0xff;
	si->tbm[0]            = 0;
	si->tbm[1]            = 1;
	si->tbm[2]            = 2;
	si->tbm[3]            = 3;
	si->tbm[4]            = 4;
	si->tbm_en_map        = 0x1f;
	si->seg_en            = false;
	si->fsqm_prio         = 0;
	si->policies_map      = 0x1;
	si->dpu.nat_sz        = 8;
	si->dpu.nhdr_sz       = 8;
	si->dpu.lyr_fld_off   = 24;
	si->sce.l3_csum_delta = 1;
	si->sce.l4_csum_delta = 2;
	si->sce.l4_csum_zero  = 6;
	si->sce.nhdr_csum     = 3;
	si->sce.dscp          = 4;
	si->sce.tot_len_diff  = 5;
	si->sce.new_dst_port  = 6;
	si->sce.new_src_port  = 7;
	si->sce.valid         = true;
	si->sce.ttl_diff      = 16;
	si->bce.nat.v4.daddr  = htonl(0x64656667);
	si->bce.nat.v4.saddr  = htonl(0xc8c9cacb);
	memset(si->ud, 0xa, si->si_ud_sz);
	memset(si->bce.nhdr, 0xb, si->dpu.nhdr_sz);

	for (i = 0; i < si->fv_sz; i++)
		((u8 *)&si->fv)[i] = (u8)i;
}

/**
 * @brief Create a custom SI structure w/o dpu, sce and bce regions
 */
static void __test_si_create2(struct pp_si *si)
{
	u32 i;

	si->sess_id           = 555;
	si->recipe_idx        = 50;
	si->fv_sz             = sizeof(si->fv);
	si->bce_ext           = false;
	si->si_ud_sz          = 8;
	si->si_ps_sz          = 4;
	si->ps_off            = 2;
	si->ext_df_mask       = true;
	si->int_df_mask       = true;
	si->base_policy       = 5;
	si->color             = PP_COLOR_YELLOW;
	si->tdox_flow         = 0;
	si->dst_q             = 0;
	si->eg_port           = 0;
	si->trim_l3_id        = 0;
	si->chck_flags        = SESS_CHCK_DFLT_FLAGS;
	si->pl2p              = false;
	si->tmpl_ud_sz        = 0;
	si->ps_copy           = 0;
	si->pkt_len_diff      = -10;
	si->sgc[0]            = 0;
	si->sgc[1]            = 1;
	si->sgc[2]            = 2;
	si->sgc[3]            = 3;
	si->sgc[4]            = 4;
	si->sgc[5]            = 5;
	si->sgc[6]            = 6;
	si->sgc[7]            = 3;
	si->sgc_en_map        = 0xff;
	si->tbm[0]            = 0;
	si->tbm[1]            = 1;
	si->tbm[2]            = 2;
	si->tbm[3]            = 3;
	si->tbm[4]            = 4;
	si->tbm_en_map        = 0x1f;
	si->seg_en            = false;
	si->fsqm_prio         = 0;
	si->policies_map      = 0x1;
	memset(si->ud, 0xb, si->si_ps_sz);
	memset(si->ud + si->si_ps_sz, 0xa, si->si_ud_sz);

	for (i = 0; i < si->fv_sz; i++)
		((u8 *)&si->fv)[i] = (u8)i;
}

/**
 * @brief Shortcut for comparing a field of 2 structures, and in
 *        case the fields aren't equal, print their values
 * @a first struct
 * @b second struct
 * @fld field to compare
 * @fmt field print specifier
 */
#define FIELD_COMP(a, b, fld, fmt)                                             \
	do {                                                                   \
		if (memcmp(&(a)->fld, &(b)->fld, sizeof((a)->fld))) {          \
			pr_debug("%s.%s " fmt " != %s.%s " fmt "\n", #a, #fld, \
				 (a)->fld, #b, #fld, (b)->fld);                \
		}                                                              \
	} while (0)

/**
 * @brief Same as FIELD_COMP, just for arrays
 */
#define ARRAY_COMP(a, b, fld)                                                  \
	do {                                                                   \
		if (memcmp(&(a)->fld, &(b)->fld, sizeof((a)->fld))) {          \
			print_hex_dump_bytes(#a "->" #fld ":",                 \
					     DUMP_PREFIX_NONE, &(a)->fld,      \
					     sizeof((a)->fld));                \
			print_hex_dump_bytes(#b "->" #fld ":",                 \
					     DUMP_PREFIX_NONE, &(b)->fld,      \
					     sizeof((b)->fld));                \
		}                                                              \
	} while (0)

/**
 * @brief Test if 2 SIs are equals, in case they are not, print which
 *        values are not equal
 * @param a
 * @param b
 */
static void __test_print_diff_si(struct pp_si *a, struct pp_si *b)
{
	FIELD_COMP(a, b, sess_id,           "%u");
	FIELD_COMP(a, b, recipe_idx,        "%u");
	FIELD_COMP(a, b, dpu_start,         "%u");
	FIELD_COMP(a, b, sce_start,         "%u");
	FIELD_COMP(a, b, fv_sz,             "%u");
	FIELD_COMP(a, b, bce_start,         "%u");
	FIELD_COMP(a, b, bce_sz,            "%u");
	FIELD_COMP(a, b, bce_ext,           "%u");
	FIELD_COMP(a, b, si_ud_sz,          "%u");
	FIELD_COMP(a, b, si_ps_sz,          "%u");
	FIELD_COMP(a, b, ps_off,            "%u");
	FIELD_COMP(a, b, ext_df_mask,       "%u");
	FIELD_COMP(a, b, int_df_mask,       "%u");
	FIELD_COMP(a, b, base_policy,       "%u");
	FIELD_COMP(a, b, color,             "%u");
	FIELD_COMP(a, b, tdox_flow,         "%u");
	FIELD_COMP(a, b, dst_q,             "%u");
	FIELD_COMP(a, b, eg_port,           "%u");
	FIELD_COMP(a, b, trim_l3_id,        "%u");
	FIELD_COMP(a, b, chck_flags,        "%u");
	FIELD_COMP(a, b, pl2p,              "%u");
	FIELD_COMP(a, b, tmpl_ud_sz,        "%u");
	FIELD_COMP(a, b, ps_copy,           "%u");
	FIELD_COMP(a, b, pkt_len_diff,      "%d");
	FIELD_COMP(a, b, sgc_en_map,        "%#lx");
	FIELD_COMP(a, b, tbm_en_map,        "%#lx");
	FIELD_COMP(a, b, seg_en,            "%u");
	FIELD_COMP(a, b, fsqm_prio,         "%u");
	FIELD_COMP(a, b, policies_map,      "%#x");
	FIELD_COMP(a, b, dpu.nat_sz,        "%u");
	FIELD_COMP(a, b, dpu.nhdr_sz,       "%u");
	FIELD_COMP(a, b, dpu.lyr_fld_off,   "%u");
	FIELD_COMP(a, b, sce.l3_csum_delta, "%x");
	FIELD_COMP(a, b, sce.l4_csum_delta, "%x");
	FIELD_COMP(a, b, sce.l4_csum_zero,  "%x");
	FIELD_COMP(a, b, sce.nhdr_csum,     "%x");
	FIELD_COMP(a, b, sce.dscp,          "%u");
	FIELD_COMP(a, b, sce.tot_len_diff,  "%u");
	FIELD_COMP(a, b, sce.new_dst_port,  "%u");
	FIELD_COMP(a, b, sce.new_src_port,  "%u");
	FIELD_COMP(a, b, sce.valid,         "%u");
	FIELD_COMP(a, b, sce.ttl_diff,      "%u");

	ARRAY_COMP(a, b, tbm);
	ARRAY_COMP(a, b, sgc);
	ARRAY_COMP(a, b, ud);
	ARRAY_COMP(a, b, bce);
	ARRAY_COMP(a, b, fv);

}

/**
 * @brief Test SI decode and encode functionality by encoding an SI
 *        and than decode it and compare the result
 */
static int __si_encode_decode_test(struct pp_si *si)
{
	struct pp_hw_si hw_si;
	struct pp_si    dec_si;
	s32 ret, i;

	memset(&hw_si, 0, sizeof(hw_si));
	memset(&dec_si, 0, sizeof(dec_si));

	ret = pp_si_encode(&hw_si, si);
	if (unlikely(ret)) {
		pr_info("Failed to encode the SI, ret = %d\n", ret);
		return PP_UT_FAIL;
	}

	for (i = 0; i < ARRAY_SIZE(hw_si.word); i++)
		pr_debug("SI offset %#2x: %#8x\n", (i * 4), hw_si.word[i]);

	ret = pp_si_decode(&dec_si, &hw_si);
	if (unlikely(ret)) {
		pr_info("Failed to decode the SI back, ret = %d\n", ret);
		return PP_UT_FAIL;
	}

	if (memcmp(si, &dec_si, sizeof(*si))) {
		pr_info("encoded and decoded SIs don't match\n");
		__test_print_diff_si(si, &dec_si);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test SI decode and encode functionality by encoding an SI
 *        and than decode it and compare the result
 */
static int pp_si_encode_decode_test(void *unused)
{
	struct pp_si si;
	int res = PP_UT_PASS;

	memset(&si, 0, sizeof(si));
	__test_si_create1(&si);
	if (__si_encode_decode_test(&si) == PP_UT_FAIL) {
		pr_info("si test 1 failed\n");
		res = PP_UT_FAIL;
	}

	memset(&si, 0, sizeof(si));
	__test_si_create2(&si);
	if (__si_encode_decode_test(&si) == PP_UT_FAIL) {
		pr_info("si test 2 failed\n");
		res = PP_UT_FAIL;
	}

	return res;
}

/**
 * @brief Test callback for accepting session manager events during
 *        the test
 * @param args event arguments
 */
static void __smgr_test_cb(struct pp_cb_args *args)
{
	if (unlikely(ptr_is_null(args)))
		return;

	if (unlikely(!__pp_is_event_valid(args->ev)))
		return;

	pr_debug("PP event %u: args %p, req_id %lu, ret %d\n",
		 args->ev, args, args->req_id, args->ret);

	if (args->ev == PP_SESS_CREATE) {
		struct pp_sess_create_cb_args *cr_args;
		u32 sess_id;

		cr_args = container_of(args,
				       struct pp_sess_create_cb_args,
				       base);
		sess_id = cr_args->sess_id;
		if (cr_args->base.ret == -EEXIST) {
			pr_err("Session already exist, id %u\n", sess_id);
		} else if (!cr_args->base.ret) {
			pr_debug("Session %u created\n", sess_id);
			created_sessions[args->req_id] = sess_id;
		} else {
			pr_err("Failed to create session\n");
		}
	} else if (args->ev == PP_SESS_DELETE) {
		struct pp_sess_delete_cb_args *del_args;
		u32 sess_id;

		del_args = container_of(args,
				       struct pp_sess_delete_cb_args,
				       base);
		sess_id = del_args->sess_id;
		if (del_args->base.ret == -ENOENT)
			pr_err("Session %u doesn't exist\n", sess_id);
		else if (!del_args->base.ret)
			pr_info("Session %u deleted\n", sess_id);
		else
			pr_err("Failed to delete session %u\n", sess_id);
	}
}

static void __smgr_test_pkt_set(struct pktprs_hdr *h)
{
	h->buf_sz = 54;
	set_bit(PKTPRS_PROTO_MAC, &h->proto_bmap[0]);
	h->proto_info[PKTPRS_PROTO_MAC][0].off = 0;
	set_bit(PKTPRS_PROTO_IPV4, &h->proto_bmap[0]);
        h->proto_info[PKTPRS_PROTO_IPV4][0].off = 14;
	set_bit(PKTPRS_PROTO_TCP, &h->proto_bmap[0]);
        h->proto_info[PKTPRS_PROTO_TCP][0].off = 34;
	set_bit(PKTPRS_PROTO_PAYLOAD, &h->proto_bmap[0]);
        h->proto_info[PKTPRS_PROTO_PAYLOAD][0].off = 54;
}

static int __sess_create_test_sync(void)
{
	u32 i, max_tries = 100;

	do {
		/* wait for 100ms till all workqueues will finish */
		msleep(100);

		for (i = 0; i < ARRAY_SIZE(created_sessions); i++)
			if (created_sessions[i] == U32_MAX)
				break;

		if (i == ARRAY_SIZE(created_sessions))
			return PP_UT_PASS;

		/* keep waiting for all sessions to be created
		 * with maximum of 10 seconds (100ms * 100)
		 */
	} while (max_tries--);

	return PP_UT_FAIL;
}

static void __sess_args_default_set(struct pp_sess_create_args *args,
				    struct pktprs_hdr *rx,
				    struct pktprs_hdr *tx)
{
	u32 i;

	memset(args, 0, sizeof(*args));
	memset(rx, 0, sizeof(*rx));
	memset(tx, 0, sizeof(*tx));

	__smgr_test_pkt_set(rx);
	__smgr_test_pkt_set(tx);

	args->color = PP_COLOR_GREEN;
	args->rx = rx;
	args->tx = tx;
	args->dst_q = qos_queue;
	args->dst_q_high = qos_queue;
	args->eg_port = pp_port;
	args->in_port = pp_port;

	/* create all session with each group sgc configured to sgc 0 */
	for (i = 0; i < ARRAY_SIZE(args->sgc); i++)
		args->sgc[i] = 0;
	for (i = 0; i < ARRAY_SIZE(args->tbm); i++)
		args->tbm[i] = PP_TBM_INVALID;
}

static int pp_sess_create_test(void *unused)
{
	struct pp_sess_create_args args;
	u32 sess_id, i;
	u16 rand;
	int ret;
	struct pp_request req;
	struct pktprs_hdr rx; /* ingress packet */
	struct pktprs_hdr tx; /* egress packet  */

	memset(&req,    0, sizeof(req));
	__sess_args_default_set(&args, &rx, &tx);
	req.cb       = __smgr_test_cb;

	for (i = 0; i < ARRAY_SIZE(created_sessions); i++) {
		/* set session id to invalid */
		created_sessions[i] = U32_MAX;
		req.req_id = i;
		/* randomly choose to use sync or async */
		get_random_bytes(&rand, sizeof(rand));
		rand %= 2;

		pktprs_eth_hdr(&rx, 0)->h_source[0] += 1;
		pktprs_eth_hdr(&tx, 0)->h_source[0] += 1;
		args.hash.h1++;
		args.hash.h2++;
		args.hash.sig++;

		if (rand)
			/* sync */
			ret = pp_session_create(&args, &sess_id, NULL);
		else
			/* async */
			ret = pp_session_create(&args, NULL, &req);

		if (rand && ret == -EEXIST) {
			pr_err("Session already exist, id %u\n", sess_id);
			return PP_UT_FAIL;
		} else if (rand && !ret) {
			pr_debug("Session %u created\n", sess_id);
			created_sessions[i] = sess_id;
		} else if (ret) {
			pr_err("Session create failed, ret = %d\n", ret);
			return PP_UT_FAIL;
		}
	}

	/* wait for all sessions to be created, the test fails if
	 * not all were created
	 */
	return __sess_create_test_sync();
}

static int pp_sess_delete_test(void *data)
{
	s32 ret;
	u32 i;

	for (i = 0; i < ARRAY_SIZE(created_sessions); i++) {
		ret = pp_session_delete(created_sessions[i], NULL);
		if (likely(!ret)) {
			pr_debug("session %i deleted\n", created_sessions[i]);
			continue;
		}

		pr_err("failed to delete session %u\n", created_sessions[i]);
		return PP_UT_FAIL;
	}
	return PP_UT_PASS;
}

/**
 * @brief clean all leftovers in case not all sessions were not deleted
 */
static int pp_sess_post_test(void *data)
{
	struct pp_qos_dev *qdev;
	struct pp_request req;

	/* delete all sessions */
	memset(&req, 0, sizeof(req));
	req.cb = __smgr_test_cb;
	pp_flush_all(&req);

	/* get qos device ref */
	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev))) {
		pr_err("failed to get qos dev\n");
		return PP_UT_FAIL;
	}

	/* delete qos port and queue */
	if (qos_queue != U32_MAX)
		pp_qos_queue_remove(qdev, qos_queue);
	if (qos_port != U32_MAX)
		pp_qos_port_remove(qdev, qos_port);

	/* remove pp port */
	if (pp_port != U32_MAX)
		pmgr_port_del(pp_port, false);

	kfree(created_tdox_sessions);

	return PP_UT_PASS;
}

/**
 * @brief Initialize groups info necessary for the tests,
 *        and reserve some of them
 */
static int pp_sgc_init_test(void *data)
{
	struct sgc_grp_test_info *grp;
	u32 idx = 0;
	s32 ret = 0;

	/* reserve some of the groups and get their info */
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups), idx++) {
		grp->idx = idx;
		if (idx == 0 || idx == 2 || idx ==5) {
			ret = pp_sgc_group_reserve(idx, &grp->owner);
			if (unlikely(ret)) {
				pr_err("fail to reserve group %u\n", idx);
				return PP_UT_FAIL;
			}
			/* try to reserve again */
			ret = pp_sgc_group_reserve(idx, &grp->owner);
			if (unlikely(!ret)) {
				pr_err("reserve group %u twice\n", idx);
				return PP_UT_FAIL;
			}
		}

		ret = chk_sgc_group_info_get(idx, &grp->n_cntrs, &grp->owner);
		if (unlikely(ret)) {
			pr_err("failed to get group %u info, ret %d\n", idx,
			       ret);
			return PP_UT_FAIL;
		}
	}
	return PP_UT_PASS;
}

/**
 * @brief Unreserve all groups
 */
static int pp_sgc_clean(void *data)
{
	struct sgc_grp_test_info *grp;
	s32 ret;

	/* get all groups info */
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		if (grp->owner == PP_SGC_SHARED_OWNER)
			continue;
		ret = pp_sgc_group_unreserve(grp->idx, grp->owner);
		if (unlikely(ret)) {
			pr_err("failed to unreserve group %u, ret %d\n",
			       grp->idx, ret);
			return PP_UT_FAIL;
		}
	}
	return PP_UT_PASS;
}

/**
 * @brief Allocate all SGCs the HW support using their real owner
 *        and expect everthing will be allocated.
 */
static int pp_sgc_alloc_test(void *data)
{
	struct sgc_grp_test_info *grp;
	u16 cntrs[SGC_GRP2_CNTRS_NUM];
	u16 cntr, tmp;
	s32 ret;
	u32 real_owner, owner;

	/* allocate all counters 1 by 1 for all groups other than #2 */
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		if (grp->idx == 2) {
			ret = pp_sgc_alloc(grp->owner, grp->idx, cntrs,
					   grp->n_cntrs);
			if (likely(!ret))
				continue;

			pr_err("Failed to allocate %u counters from group %u\n",
			       grp->n_cntrs, grp->idx);
			return PP_UT_FAIL;
		}
		for (cntr = 0; cntr < grp->n_cntrs; cntr++) {
			ret = pp_sgc_alloc(grp->owner, grp->idx, &tmp, 1);
			if (likely(!ret))
				continue;
			pr_err("Allocate %u out of %u counters from group %u ,ret %d\n",
			       cntr, grp->n_cntrs, grp->idx, ret);
			return PP_UT_FAIL;
		}
	}

	/* check if we can allocate more than HW supports */
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		ret = pp_sgc_alloc(grp->owner, grp->idx, &tmp, 1);
		if (likely(ret))
			continue;
		pr_err("Too much counters allocated from group %u\n", grp->idx);
		return PP_UT_FAIL;
	}

	/* find real owner */
	real_owner = PP_SGC_SHARED_OWNER;
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		if (grp->owner != PP_SGC_SHARED_OWNER) {
			real_owner = grp->owner;
			break;
		}
	}

	/* try to allocate with real owner for groups which has no owner
	 * and with invalid owner for the ones who have owner
	 */
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		if (grp->owner == PP_SGC_SHARED_OWNER)
			owner = real_owner;
		else
			owner = PP_SGC_SHARED_OWNER;

		/* we expect the alloc to fail cause we already allocate
		 * all the SGCs, but we expect it will fail due to wrong
		 * owner, so the expected return code should be EPERM
		 */
		ret = pp_sgc_alloc(owner, grp->idx, &tmp, 1);
		if (likely(ret == -EPERM))
			continue;
		pr_err("Owner check for group %u with bad owner %u fail, group owner is %u\n",
		       grp->idx, owner, grp->owner);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

static int pp_sgc_sessions_list_test(void *unused)
{
	u32 *sessions, n_sessions, i, j, res;
	s32 ret;

	ret = smgr_sessions_arr_alloc(&sessions, &n_sessions);
	if (unlikely(ret)) {
		pr_err("failed to allocate sessions array, ret %d\n", ret);
		return PP_UT_FAIL;
	}

	ret = pp_sgc_sessions_get(0, 0, sessions, &n_sessions);
	if (unlikely(ret)) {
		pr_err("failed to get sgc[0][0] sessions list, ret %d\n", ret);
		return PP_UT_FAIL;
	}

	if (ARRAY_SIZE(created_sessions) != n_sessions) {
		pr_info("sgc[0][0] is used only by %u instead of %u sessions\n",
			n_sessions, (u32)ARRAY_SIZE(created_sessions));
		return PP_UT_FAIL;
	}

	/* search all of the created session*/
	res = PP_UT_PASS;
	for (i = 0; i < ARRAY_SIZE(created_sessions); i++) {
		for (j = 0; j < n_sessions; j++) {
			if (created_sessions[i] == sessions[j])
				/* session found */
				break;
		}
		if (j < n_sessions)
			continue;
		pr_info("sgc[0][0] should be used by session %u, but its not\n",
			created_sessions[i]);
		res = PP_UT_FAIL;
	}

	return res;
}

/**
 * @brief Free all SGCs the HW support and expect everthing will
 *        be freed.
 */
static int pp_sgc_free_test(void *data)
{
	struct sgc_grp_test_info *grp;
	u32 real_owner, owner;
	u16 cntr;
	s32 ret;

	/* find real owner */
	real_owner = PP_SGC_SHARED_OWNER;
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		if (grp->owner != PP_SGC_SHARED_OWNER) {
			real_owner = grp->owner;
			break;
		}
	}

	/* free all counters */
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		/* get wrong owner */
		if (grp->owner == PP_SGC_SHARED_OWNER)
			owner = real_owner;
		else
			owner = PP_SGC_SHARED_OWNER;

		/* try to free the first sgc with wrong owner */
		cntr = 0;
		ret = pp_sgc_free(owner, grp->idx, &cntr, 1);
		if (unlikely(!ret)) {
			pr_err("sgc[%u][%u] was freed with owner %u instead of %u\n",
				grp->idx, cntr, PP_SGC_SHARED_OWNER,
				grp->owner);
			return PP_UT_FAIL;
		}
		/* free all sgcs */
		for (cntr = 0; cntr < grp->n_cntrs; cntr++) {
			/* free sgc with group's owner */
			ret = pp_sgc_free(grp->owner, grp->idx, &cntr, 1);
			if (unlikely(ret)) {
				pr_err("Failed to free sgc[%u][%u]\n", grp->idx,
				       cntr);
				return PP_UT_FAIL;
			}
		}
	}

	return PP_UT_PASS;
}

/**
 * @brief Allocate SGC, modify it and verify it was modified properly
 */
static int __pp_sgc_mod_test(u8 grp, u16 cntr)
{
	struct sgc_grp_test_info *info = &groups[grp];
	struct pp_stats stats = { 0 };
	s32 ret;

	/* get the sgc stats */
	ret = pp_sgc_get(grp, cntr, &stats, NULL);
	if (unlikely(ret)) {
		pr_err("failed to get sgc[%u][%u] stats\n", grp, cntr);
		return PP_UT_FAIL;
	}

	/* stats of newly allocated sgc should be zero */
	if (stats.bytes != 0 || stats.packets != 0) {
		pr_err("non-zero sgc[%u][%u] allocated\n", grp, cntr);
		return PP_UT_FAIL;
	}

	/* modify the sgc */
	ret = pp_sgc_mod(info->owner, grp, cntr, PP_STATS_ADD, 2000, 20000);
	if (unlikely(ret)) {
		pr_err("failed to modify sgc[%u][%u]\n", grp, cntr);
		return PP_UT_FAIL;
	}

	ret = pp_sgc_mod(info->owner, grp, cntr, PP_STATS_SUB, 1000, 10000);
	if (unlikely(ret)) {
		pr_err("failed to modify sgc[%u][%u]\n", grp, cntr);
		return PP_UT_FAIL;
	}

	ret = pp_sgc_get(grp, cntr, &stats, NULL);
	if (unlikely(ret)) {
		pr_err("failed to get sgc[%u][%u] stats\n", grp, cntr);
		return PP_UT_FAIL;
	}

	if (stats.bytes != 10000 || stats.packets != 1000) {
		pr_err("non-zero sgc[%u][%u] allocated\n", grp, cntr);
		return PP_UT_FAIL;
	}

	/* reset the sgc */
	ret = pp_sgc_mod(info->owner, grp, cntr, PP_STATS_RESET, 0, 0);
	if (unlikely(ret)) {
		pr_err("failed to reset sgc[%u][%u]\n", grp, cntr);
		return PP_UT_FAIL;
	}

	ret = pp_sgc_get(grp, cntr, &stats, NULL);
	if (unlikely(ret)) {
		pr_err("failed to get sgc[%u][%u] stats\n", grp, cntr);
		return PP_UT_FAIL;
	}

	if (stats.bytes != 0 || stats.packets != 0) {
		pr_err("failed to reset sgc[%u][%u]\n", grp, cntr);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test that all sgc modification works for all groups
 */
static int pp_sgc_mod_test(void *data)
{
	struct sgc_grp_test_info *grp;
	s32 ret;
	u16 cntr;

	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		for (cntr = 0; cntr < grp->n_cntrs; cntr++) {
			ret = __pp_sgc_mod_test(grp->idx, cntr);
			if (unlikely(ret != PP_UT_PASS))
				return ret;
		}
	}

	return PP_UT_PASS;
}

/**
 * @brief Call SGC APIs with invalid arguments
 */
static int pp_sgc_invalid_args_test(void *data)
{
	struct sgc_grp_test_info *grp;
	u16 cntr;
	s32 res = PP_UT_PASS;

	/* try to allocate from invalid group */
	if (pp_sgc_alloc(PP_SGC_SHARED_OWNER, PP_SI_SGC_MAX, &cntr, 1) == 0) {
		pr_err("Invalid SGC[%u][%u] allocated\n", PP_SI_SGC_MAX, cntr);
		res = PP_UT_FAIL;
	}
	/* try to free invalid counter idx from group 0 */
	cntr = SGC_GRP0_CNTRS_NUM;
	if (pp_sgc_free(groups[0].owner, 0, &cntr, 1) == 0) {
		pr_err("Invalid SGC[%u][%u] freed\n", 0, cntr);
		res = PP_UT_FAIL;
	}

	/* invalid modification type */
	if (pp_sgc_mod(groups[0].owner, 0, 0, PP_STATS_OP_CNT, 0, 0) == 0) {
		pr_err("Invalid modification succeed\n");
		res = PP_UT_FAIL;
	}

	/* find owner different than group 0 owner */
	for_each_arr_entry(grp, groups, ARRAY_SIZE(groups)) {
		if (grp->owner != groups[0].owner)
			break;
	}
	/* modification with invalid owner */
	if (pp_sgc_mod(grp->owner, 0, 0, PP_STATS_RESET, 0, 0) == 0) {
		pr_err("modification with invalid owner succeed\n");
		res = PP_UT_FAIL;
	}

	return res;
}

/**
 * @brief Allocate all TBMs the HW support and expect everthing will
 *        be allocated.
 */
static int pp_dual_tbm_alloc_test(void *data)
{
	s32 ret;
	u16 i, idx;

	/* allocate all existing TBMs */
	for (i = 0; i < CHK_DUAL_TBM_NUM; i++) {
		ret = pp_dual_tbm_alloc(&idx, NULL);
		if (likely(!ret))
			continue;

		pr_err("Failed to allocate tbm, i = %u\n", i);
		/* allocation failed, not to continue */
		return PP_UT_FAIL;
	}

	/* check if we can allocate more the HW supports */
	ret = pp_dual_tbm_alloc(&idx, NULL);
	if (unlikely(!ret)) {
		pr_err("Too much TBMs allocated\n");
		return PP_UT_FAIL;
	};

	return PP_UT_PASS;
}

/**
 * @brief Set dual TBM configuration and verify it was proprely set
 */
static int __pp_dual_tbm_mod_test(u16 idx)
{
	struct pp_dual_tbm expected = { 0 };
	struct pp_dual_tbm actual   = { 0 };
	s32 ret;

	expected.enable   = true;
	expected.cbs      = 100;
	expected.cir      = 50;
	expected.pbs      = 200;
	expected.pir      = 150;
	expected.len_type = PP_TBM_NEW_LEN;
	expected.mode     = PP_TBM_MODE_2698_CA;

	ret = pp_dual_tbm_set(idx, &expected);
	if (unlikely(ret)) {
		pr_err("Failed to set dual tbm %u configuration\n", idx);
		return PP_UT_FAIL;
	}

	ret = pp_dual_tbm_get(idx, &actual);
	if (unlikely(ret)) {
		pr_err("Failed to get dual tbm %u configuration\n", idx);
		return PP_UT_FAIL;
	}

	if (likely(!memcmp(&expected, &actual, sizeof(actual))))
		return PP_UT_PASS;

	FIELD_COMP(&expected, &actual, enable,   "%u");
	FIELD_COMP(&expected, &actual, cbs,      "%u");
	FIELD_COMP(&expected, &actual, cir,      "%llu");
	FIELD_COMP(&expected, &actual, pbs,      "%u");
	FIELD_COMP(&expected, &actual, pir,      "%llu");
	FIELD_COMP(&expected, &actual, len_type, "%u");
	FIELD_COMP(&expected, &actual, mode,     "%u");

	return PP_UT_FAIL;
}

/**
 * @brief Test all TBM modification logic on all TBMs
 */
static int pp_dual_tbm_mod_test(void *data)
{
	s32 res;
	u16 i;

	res = PP_UT_PASS;
	for (i = 0; i < CHK_DUAL_TBM_NUM; i++) {
		if (unlikely(__pp_dual_tbm_mod_test(i) != PP_UT_PASS))
			res = PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Configure dual tbm to a specific rate and verify
 *        the mantissa and the exponent are well configured
 * @param data test data
 * @return int
 */
static int pp_dual_tbm_calc_test(void *data)
{
	struct pp_dual_tbm cfg = { 0 };
	struct tbm_cfg res_cfg = { 0 };
	struct tbm_calc_data *_data;
	u32 org_clk, i;
	s32 ret, res;

	res = PP_UT_PASS;

	(void)chk_clock_get(&org_clk);

	_data = (struct tbm_calc_data *)data;

	for (i = 0; i < ARRAY_SIZE(clocks); i++) {
		if (unlikely(chk_clock_set(clocks[i])))
			return PP_UT_FAIL;
		/* configure only the cir for the test purpose */
		cfg.cir = _data->rate;
		ret = pp_dual_tbm_set(0, &cfg);
		if (unlikely(ret)) {
			pr_err("failed to configure dual TBM 0\n");
			res = PP_UT_FAIL;
			break;
		}
		/* read the tbm configuration directly from the checker */
		ret = chk_tbm_get(0, &res_cfg);
		if (unlikely(ret)) {
			pr_err("failed to read TBM 0 configuration\n");
			res = PP_UT_FAIL;
			break;
		}
		/* verify mantissa and exponent are well configured */
		if (res_cfg.exponent != _data->expected_exponent[i]) {
			pr_err("Clock %uMHz, rate %llu(bps) test: exponent %u, expected %u\n",
			       clocks[i], _data->rate * BITS_PER_BYTE,
			       res_cfg.exponent, _data->expected_exponent[i]);
			res = PP_UT_FAIL;
			continue;
		}
		if (res_cfg.mantissa != _data->expected_mantissa[i]) {
			pr_err("Clock %uMHz, rate %llu(bps) test: mantissa %u, expected %u\n",
			       clocks[i], _data->rate * BITS_PER_BYTE,
			       res_cfg.mantissa, _data->expected_mantissa[i]);
			res = PP_UT_FAIL;
			continue;
		}
	}

	/* set back the original clock */
	if (unlikely(chk_clock_set(org_clk))) {
		pr_err("Failed to set the clock back to %uMHz\n", org_clk);
		res = PP_UT_FAIL;
	}

	return res;
}

static int pp_dual_tbm_invalid_args_test(void *data)
{
	struct pp_dual_tbm cfg = { 0 };
	s32 ret, res;

	res = PP_UT_PASS;

	/* try with NULL pointer */
	ret = pp_dual_tbm_set(0, NULL);
	if (unlikely(!ret)) {
		/* we will probably crash before reaching here :-) */
		pr_err("Set dual TBM with NULL configuration pointer!\n");
		res = PP_UT_FAIL;
	}

	/* try with invalid index */
	ret = pp_dual_tbm_set(CHK_DUAL_TBM_NUM, &cfg);
	if (unlikely(!ret)) {
		pr_err("Set configuration for invalid dual tbm index %u\n",
		       CHK_DUAL_TBM_NUM);
		res = PP_UT_FAIL;
	}

	return res;
}

/**
 * @brief Free all allocated TBMs
 */
static int pp_dual_tbm_free_test(void *data)
{
	s32 ret;
	u16 i;

	for (i = 0; i < CHK_DUAL_TBM_NUM; i++) {
		ret = pp_dual_tbm_free(i);
		if (likely(!ret))
			continue;

		pr_err("Failed to free tbm %u, ret %d\n", i, ret);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test max tdox sessions
 */
static int pp_tdox_max_session_test(void *data)
{
	struct pp_sess_create_args args;
	struct smgr_tdox_stats stats;
	struct pktprs_hdr rx; /* ingress packet */
	struct pktprs_hdr tx; /* egress packet  */
	s32 ret;
	u32 i;

	if (!smgr_tdox_enable_get())
		return PP_UT_PASS;

	uc_nf_set(PP_NF_TURBODOX, 0, 0, 0, 0);
	smgr_tdox_late_init();

	__sess_args_default_set(&args, &rx, &tx);
	args.flags = PP_SESS_FLAG_TDOX_MSK;

	/** 1. Create Max tdox sessions
	 *  2. Make sure all sessions attached tdox
	 */

	/* Create maximum Tdox sessions */
	for (i = 0; i < UC_MAX_TDOX_SESSIONS; i++) {
		tdox_hash.h1++;
		tdox_hash.h2++;
		tdox_hash.sig++;
		args.hash = tdox_hash;
		ret = pp_session_create(&args, &created_tdox_sessions[i], NULL);
		if (ret) {
			pr_err("Failed creating session\n");
			return PP_UT_FAIL;
		}
	}
	smgr_tdox_stats_get(&stats);
	if (stats.tdox_sess_free != 0) {
		pr_err("Expected 0 free tdox sessions\n");
		return PP_UT_FAIL;
	}
	if (stats.tdox_free_candidates != UC_MAX_TDOX_SESSIONS) {
		pr_err("Expected %u free candidate tdox sessions\n",
		       UC_MAX_TDOX_SESSIONS);
		return PP_UT_FAIL;
	}
	return PP_UT_PASS;
}

/**
 * @brief Test max candidates
 */
static int pp_tdox_candidate_test(void *data)
{
	struct pp_sess_create_args args;
	struct smgr_tdox_stats stats;
	struct pktprs_hdr rx; /* ingress packet */
	struct pktprs_hdr tx; /* egress packet  */
	s32 ret;
	u32 i;

	if (!smgr_tdox_enable_get())
		return PP_UT_PASS;

	__sess_args_default_set(&args, &rx, &tx);
	args.flags = PP_SESS_FLAG_TDOX_MSK;

	/** 1. Create Additional Max tdox sessions
	 *  2. Make sure all session attached to candidates
	 */

	/* Create maximum Tdox candidate sessions */
	for (i = UC_MAX_TDOX_SESSIONS; i < UC_MAX_TDOX_SESSIONS * 2; i++) {
		tdox_hash.h1++;
		tdox_hash.h2++;
		tdox_hash.sig++;
		args.hash = tdox_hash;
		ret = pp_session_create(&args, &created_tdox_sessions[i], NULL);
		if (ret) {
			pr_err("Failed creating session (ret %d)\n", ret);
			return PP_UT_FAIL;
		}
	}
	smgr_tdox_stats_get(&stats);
	if (stats.tdox_sess_free != 0) {
		pr_err("Expected 0 free tdox sessions\n");
		return PP_UT_FAIL;
	}
	if (stats.tdox_free_candidates != 0) {
		pr_err("Expected 0 free candidate tdox sessions\n");
		return PP_UT_FAIL;
	}
	return PP_UT_PASS;
}

/**
 * @brief Test tdox non qualified
 */
static int pp_tdox_nonqulaified_test(void *data)
{
	struct smgr_tdox_stats stats;
	s32 ret;
	u32 i;

	if (!smgr_tdox_enable_get())
		return PP_UT_PASS;

	/** 1. All active tdox sessions should be removed
	 *  2. Candidates will take over
	 */
	/* Set all records as non qualified */
	for (i = 0; i < UC_MAX_TDOX_SESSIONS; i++) {
		pp_session_stats_mod(created_tdox_sessions[i], PP_STATS_ADD, 15,
				     15000);
		pp_session_stats_mod(created_tdox_sessions[i], PP_STATS_ADD, 15,
				     15000);
		pp_session_stats_mod(created_tdox_sessions[i], PP_STATS_ADD, 15,
				     15000);
	}

	pr_info("Wait 10 seconds...\n");
	msleep(10000);

	smgr_tdox_stats_get(&stats);
	if (stats.tdox_sess_free != 0) {
		pr_err("Expected %u free tdox sessions. got %u\n",
		       0, stats.tdox_sess_free);
		return PP_UT_FAIL;
	}
	if (stats.tdox_free_candidates != UC_MAX_TDOX_SESSIONS) {
		pr_err("Expected %u free candidate tdox sessions. got %u\n",
		       UC_MAX_TDOX_SESSIONS, stats.tdox_free_candidates);
		return PP_UT_FAIL;
	}

	/** 1. All active tdox sessions should be removed
	 *  2. No candidates to take over
	 */
	for (i = UC_MAX_TDOX_SESSIONS; i < UC_MAX_TDOX_SESSIONS * 2; i++) {
		pp_session_stats_mod(created_tdox_sessions[i], PP_STATS_ADD, 15,
				     15000);
		pp_session_stats_mod(created_tdox_sessions[i], PP_STATS_ADD, 15,
				     15000);
		pp_session_stats_mod(created_tdox_sessions[i], PP_STATS_ADD, 15,
				     15000);
	}

	pr_info("Wait 10 seconds...\n");
	msleep(10000);

	smgr_tdox_stats_get(&stats);
	if (stats.tdox_sess_free != UC_MAX_TDOX_SESSIONS) {
		pr_err("Expected %u free tdox sessions. got %u\n",
		       UC_MAX_TDOX_SESSIONS, stats.tdox_sess_free);
		return PP_UT_FAIL;
	}
	if (stats.tdox_free_candidates != UC_MAX_TDOX_SESSIONS) {
		pr_err("Expected %u free candidate tdox sessions. got %u\n",
		       UC_MAX_TDOX_SESSIONS, stats.tdox_free_candidates);
		return PP_UT_FAIL;
	}

	/* Delete all sessions */
	for (i = 0; i < UC_MAX_TDOX_SESSIONS * 2; i++) {
		ret = pp_session_delete(created_tdox_sessions[i], NULL);
		if (ret) {
			pr_err("Failed deleting session\n");
			return PP_UT_FAIL;
		}
	}
	return PP_UT_PASS;
}

/**
 * @brief Test tdox session remove
 */
static int pp_tdox_session_remove_test(void *data)
{
	struct pp_sess_create_args args;
	struct smgr_tdox_stats stats;
	struct pktprs_hdr rx; /* ingress packet */
	struct pktprs_hdr tx; /* egress packet  */
	s32 ret;
	u32 i;

	if (!smgr_tdox_enable_get())
		return PP_UT_PASS;

	__sess_args_default_set(&args, &rx, &tx);
	args.flags = PP_SESS_FLAG_TDOX_MSK;

	/** 1. Create Max tdox sessions
	 *  2. Make sure sessions are created
	 *  3. Delete all sessions
	 *  4. Make sure all tdox are free
	 */

	/* Create maximum Tdox sessions */
	for (i = 0; i < UC_MAX_TDOX_SESSIONS; i++) {
		args.hash.h1++;
		args.hash.h2++;
		args.hash.sig++;
		ret = pp_session_create(&args, &created_tdox_sessions[i], NULL);
		if (ret) {
			pr_err("Failed creating session\n");
			return PP_UT_FAIL;
		}
	}
	smgr_tdox_stats_get(&stats);
	if (stats.tdox_sess_free != 0) {
		pr_err("Expected 0 free tdox sessions\n");
		return PP_UT_FAIL;
	}
	if (stats.tdox_free_candidates != UC_MAX_TDOX_SESSIONS) {
		pr_err("Expected %u free candidate tdox sessions\n",
		       UC_MAX_TDOX_SESSIONS);
		return PP_UT_FAIL;
	}

	/* Delete all sessions */
	for (i = 0; i < UC_MAX_TDOX_SESSIONS; i++) {
		ret = pp_session_delete(created_tdox_sessions[i], NULL);
		if (ret) {
			pr_err("Failed deleting session\n");
			return PP_UT_FAIL;
		}
	}

	pr_info("Wait 10 seconds...\n");
	msleep(10000);

	smgr_tdox_stats_get(&stats);
	if (stats.tdox_sess_free != UC_MAX_TDOX_SESSIONS) {
		pr_err("Expected %u free tdox sessions. got %u\n",
		       UC_MAX_TDOX_SESSIONS, stats.tdox_sess_free);
		return PP_UT_FAIL;
	}
	if (stats.tdox_free_candidates != UC_MAX_TDOX_SESSIONS) {
		pr_err("Expected %u free candidate tdox sessions. got %u\n",
		       UC_MAX_TDOX_SESSIONS, stats.tdox_free_candidates);
		return PP_UT_FAIL;
	}
	return PP_UT_PASS;
}

/**
 * Register all pp_session tests to pput
 */
void pp_session_tests_init(void)
{
	int test_idx;

	/* Add Tests */
	for (test_idx = 0 ; test_idx < ARRAY_SIZE(pp_session_tests); test_idx++)
		pp_register_test(&pp_session_tests[test_idx]);
}
