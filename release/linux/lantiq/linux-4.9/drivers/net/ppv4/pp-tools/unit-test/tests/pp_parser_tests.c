/*
 * pp_port_tests.c
 * Description: Packet Processor unit tests
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#include <linux/types.h>
#include <linux/pp_api.h>
#include "pput.h"
#include "pp_common.h"
#include "parser_internal.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_PRSR_TEST] %s:%d: " fmt, __func__, __LINE__
#endif

struct table_test_info {
	struct table *tbl;
	u32 n_ent;
	u32 n_rsrv;
	u32 rsrv_base;
};
static struct table_test_info test_tbl;

static int prsr_table_pre_test(void *unused);
static int prsr_table_post_test(void *unused);
static int prsr_table_reg_entrs_test(void *unused);
static int prsr_table_rsrv_entrs_test(void *unused);
static int prsr_table_invalid_test(void *unused);
static int prsr_protocol_test(void *unused);

static struct pp_test_t pp_prsr_tests[] = {
	{
		.component    = PP_PRSR_TESTS,
		.test_name    = "prsr_table_pre_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = prsr_table_pre_test,
	},
	{
		.component    = PP_PRSR_TESTS,
		.test_name    = "prsr_table_reg_entrs_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = prsr_table_reg_entrs_test,
	},
	{
		.component    = PP_PRSR_TESTS,
		.test_name    = "prsr_table_rsrv_entrs_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = prsr_table_rsrv_entrs_test,
	},
	{
		.component    = PP_PRSR_TESTS,
		.test_name    = "prsr_table_invalid_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = prsr_table_invalid_test,
	},
	{
		.component    = PP_PRSR_TESTS,
		.test_name    = "prsr_table_post_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS |
				PP_UT_LEVEL_MUST_RUN,
		.test_data    = NULL,
		.fn           = prsr_table_post_test,
	},
	{
		.component    = PP_PRSR_TESTS,
		.test_name    = "prsr_protocol_test",
		.level_bitmap = PP_UT_LEVEL_FULL |
				PP_UT_LEVEL_MUST_PASS,
		.test_data    = NULL,
		.fn           = prsr_protocol_test,
	},
};

/**
 * @brief Create a test table
 */
static int prsr_table_pre_test(void *unused)
{
	test_tbl.rsrv_base = 500;
	test_tbl.n_rsrv    = 250;
	test_tbl.n_ent     = 1500;
	test_tbl.tbl = table_create("test_table", test_tbl.n_ent,
				    test_tbl.n_rsrv, test_tbl.rsrv_base);
	if (IS_ERR(test_tbl.tbl)) {
		pr_info("failed to create test table, ret %ld\n",
			PTR_ERR(test_tbl.tbl));
		test_tbl.tbl = NULL;
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Destroy the test table
 */
static int prsr_table_post_test(void *unused)
{
	if (test_tbl.tbl)
		table_destroy(test_tbl.tbl);

	return PP_UT_PASS;
}

/**
 * @brief regular entries allocation test
 */
static int prsr_table_reg_entrs_test(void *unused)
{
	const struct table_entry *ent;
	struct table *tbl = test_tbl.tbl;
	u16 idx, i, n;
	s32 ret, res;

	res = PP_UT_PASS;
	/* allocate regular entries and make sure they are regular */
	n   = 50;
	idx = TABLE_ENTRY_INVALID_IDX;
	ret = table_alloc_entries(tbl, NULL, NULL, n, &idx, false);
	if (unlikely(ret)) {
		pr_info("Error: failed to allocate %u regular entries\n", n);
		return PP_UT_FAIL;
	}
	/* try to allocate the same entries again */
	ret = table_alloc_entries(tbl, NULL, NULL, n, &idx, false);
	if (unlikely(!ret)) {
		pr_info("Error: same entries were allocated again\n");
		return PP_UT_FAIL;
	}
	/*
	 * Check that all allocated entries:
	 * 1. marked as used
	 * 2. marked as regular, cause we asked for regular
	 * 3. ref counter check
	 */
	for (i = idx; i < idx + n; i++) {
		ent = table_entry_get(tbl, i);
		if (!ent) {
			pr_info("Error: failed to get entry %u info\n", i);
			res = PP_UT_FAIL;
			continue;
		}
		if (!ent->used) {
			pr_info("Error: entry %u isn't marked as used\n", i);
			res = PP_UT_FAIL;
		}
		if (!test_bit(i, tbl->entrs_map)) {
			pr_info("Error: entry %u isn't marked as used in the bitmap\n",
				i);
			res = PP_UT_FAIL;
		}
		if (!test_bit(i, tbl->reg.map)) {
			pr_info("Error: entry %u isn't marked as used in the regular bitmap\n",
				i);
			res = PP_UT_FAIL;
		}
		if (ent->ent_rsrv) {
			pr_info("Error: entry %u marked as reserved\n", i);
			res = PP_UT_FAIL;
		}
		/* increment reference count */
		table_entry_ref_inc(tbl, i);
		table_entry_ref_inc(tbl, i);
		if (ent->ref_cnt != 3) {
			pr_info("Error: entry %u ref count %u, expect 3\n",
				i, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
		/* 1st try to free the entry */
		ret = table_free_entry(tbl, i);
		if (unlikely(ret != -EEXIST)) {
			pr_info("Error: free entry %u failed to return -EEXIST, ref %u\n",
				i, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
		/* 2nd try to free the entry */
		ret = table_free_entry(tbl, i);
		if (unlikely(ret != -EEXIST)) {
			pr_info("Error: free entry %u failed to return -EEXIST, ref %u\n",
				i, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
		/* 3rd and last try to free the entry */
		ret = table_free_entry(tbl, i);
		if (unlikely(ret)) {
			pr_info("Error: free entry %u failed to ret %d, ref %u\n",
				i, ret, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
	}
	return res;
}

/**
 * @brief Reserved entries allocation test
 */
static int prsr_table_rsrv_entrs_test(void *unused)
{
	const struct table_entry *ent;
	struct table *tbl = test_tbl.tbl;
	u16 idx, i, n;
	s32 ret, res;

	res = PP_UT_PASS;
	/* allocate reserved entries and make sure they are regular */
	n   = 50;
	idx = TABLE_ENTRY_INVALID_IDX;
	ret = table_alloc_entries(tbl, NULL, NULL, n, &idx, true);
	if (unlikely(ret)) {
		pr_info("Error: failed to allocate %u regular entries\n", n);
		return PP_UT_FAIL;
	}
	/* try to allocate an entry again */
	ret = table_alloc_entries(tbl, NULL, NULL, n, &idx, true);
	if (unlikely(!ret)) {
		pr_info("Error: same entries were allocated again\n");
		return PP_UT_FAIL;
	}
	/*
	 * Check that all allocated entries:
	 * 1. marked as used
	 * 2. marked as reserved, cause we asked for regular
	 * 3. ref counter
	 */
	for (i = idx; i < idx + n; i++) {
		ent = table_entry_get(tbl, i);
		if (!ent) {
			pr_info("Error: failed to get entry %u info\n", i);
			res = PP_UT_FAIL;
			continue;
		}
		if (!ent->used) {
			pr_info("Error: entry %u isn't marked as used\n", i);
			res = PP_UT_FAIL;
		}
		if (!test_bit(i, tbl->entrs_map)) {
			pr_info("Error: entry %u isn't marked as used in the bitmap\n",
				i);
			res = PP_UT_FAIL;
		}
		if (!test_bit(i, tbl->rsrv.map)) {
			pr_info("Error: entry %u isn't marked as used in the reserved bitmap\n",
				i);
			res = PP_UT_FAIL;
		}
		if (!ent->ent_rsrv) {
			pr_info("Error: entry %u marked as regular\n", i);
			res = PP_UT_FAIL;
		}
		/* increment reference count */
		table_entry_ref_inc(tbl, i);
		table_entry_ref_inc(tbl, i);
		if (ent->ref_cnt != 3) {
			pr_info("Error: entry %u ref count %u, expect 3\n",
				i, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
		/* 1st try to free the entry */
		ret = table_free_entry(tbl, i);
		if (unlikely(ret != -EEXIST)) {
			pr_info("Error: free entry %u failed to return -EEXIST, ref %u\n",
				i, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
		/* 2nd try to free the entry */
		ret = table_free_entry(tbl, i);
		if (unlikely(ret != -EEXIST)) {
			pr_info("Error: free entry %u failed to return -EEXIST, ref %u\n",
				i, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
		/* 3rd and last try to free the entry */
		ret = table_free_entry(tbl, i);
		if (unlikely(ret)) {
			pr_info("Error: free entry %u failed to ret %d, ref %u\n",
				i, ret, ent->ref_cnt);
			res = PP_UT_FAIL;
		}
	}
	return res;
}

/**
 * @brief Invalid allocations test
 */
static int prsr_table_invalid_test(void *unused)
{
	struct table *tbl = test_tbl.tbl;
	u16 idx, n;
	s32 ret, res;

	res = PP_UT_PASS;
	/* try to allocate an invalid index */
	idx = test_tbl.n_ent;
	n   = 1;
	ret = table_alloc_entries(tbl, NULL, NULL, 1, &idx, false);
	if (unlikely(!ret)) {
		pr_info("Error: invalid index %u was allocated\n", idx);
		res = PP_UT_FAIL;
	}
	/* try to allocate too much entries */
	idx = 0;
	n   = test_tbl.n_ent + 1;
	ret = table_alloc_entries(tbl, NULL, NULL, n, &idx, false);
	if (unlikely(!ret)) {
		pr_info("Error: %u entries was allocated at idx %u\n", n, idx);
		res = PP_UT_FAIL;
	}
	/* try to allocate regular entry from reserved region */
	idx = test_tbl.rsrv_base;
	n   = 1;
	ret = table_alloc_entries(tbl, NULL, NULL, n, &idx, false);
	if (unlikely(!ret)) {
		pr_info("Error: regular entry allocated from reserved region at idx %u\n",
			idx);
		res = PP_UT_FAIL;
	}
	/* try to allocate reserved entry from regular region */
	idx = 0;
	n   = 1;
	ret = table_alloc_entries(tbl, NULL, NULL, n, &idx, true);
	if (unlikely(!ret)) {
		pr_info("Error: reserved entry allocated from regular region at idx %u\n",
			idx);
		res = PP_UT_FAIL;
	}

	return res;
}

/**
 * @brief Protocol test, configure a new protocol in the parser driver
 */
static int prsr_protocol_test(void *unused)
{
	struct prsr_up_layer_proto_params params;
	const struct proto_db_e  *proto;
	const struct table_entry *ent;
	struct np_info *np;
	enum prsr_proto_id id;
	int ret, res, n;

	res = PP_UT_PASS;
	ret = prsr_proto_id_alloc(&id);
	if (unlikely(ret)) {
		pr_info("Error: failed to allocate proto id\n");
		return PP_UT_FAIL;
	}
	pr_debug("Proto id %u allocated\n", id);

	memset(&params, 0, sizeof(params));
	n = strscpy(params.name, "Test Protocol", sizeof(params.name));
	/* parsing info */
	params.proto_id   = 0xfefe;
	params.hw_idx     = TABLE_ENTRY_INVALID_IDX;
	params.hdr_len    = 2;
	params.entry_type = ETYPE_NONE;
	params.ignore     = false;
	/* next protocol info */
	np = &params.np;
	np->logic = NP_LOGIC_END;
	np->off   = 5;
	np->len   = 5;
	np->logic = NP_LOGIC_TUNN;
	np->dflt  = PRSR_PROTO_PAYLOAD;
	/* header skip info */
	params.skip.logic = SKIP_LOGIC_IM;
	params.skip.hdr_len_im = 16;
	/* preceding protocols */
	params.n_pre = 1;
	params.pre[0].np_logic = NP_LOGIC_UDP;
	params.pre[0].val      = 2000;
	/*extract info */
	params.ext.n_fld = 1;
	params.ext.fld[0].fv_idx = 30;
	params.ext.fld[0].off    = 0;
	params.ext.fld[0].len    = 10;
	params.ext.fld[0].mask   = 0;

	/* add the protocol w/o the debugfs related files */
	ret = __prsr_up_layer_proto_add(id, false, &params);
	if (unlikely(ret)) {
		pr_info("Error: failed to add new protocol\n");
		res = PP_UT_FAIL;
		goto test_done;
	}

	proto = prsr_proto_info_get(id);
	if (unlikely(!proto)) {
		pr_info("Error: failed to add protocol %u\n", id);
		res = PP_UT_FAIL;
		goto test_done;
	}

	if (strncmp(params.name, proto->name, strlen(params.name))) {
		pr_info("Error: protocol name mismatch %s != %s\n",
			params.name, proto->name);
		res = PP_UT_FAIL;
	}

	ent = table_entry_get(prsr_get_tbl(PRSR_IMEM_TBL), proto->imem_idx);
	if (!ent) {
		pr_info("Error: failed to get proto imem idx %u\n",
			proto->imem_idx);
		res = PP_UT_FAIL;
		goto test_done;
	}

	if (((void *)proto) != ent->data) {
		pr_info("Error: protocol imem entry reference mismatch %p != %p\n",
			proto, ent->data);
		res = PP_UT_FAIL;
	}

	if (strncmp(params.name, ent->name, strlen(params.name))) {
		pr_info("Error: protocol imem entry name mismatch %s != %s\n",
			params.name, ent->name);
		res = PP_UT_FAIL;
	}

	ent = table_entry_get(prsr_get_tbl(PRSR_PIT_TBL), proto->pit.base);
	if (!ent) {
		pr_info("Error: failed to get proto pit idx %u\n",
			proto->pit.base);
		res = PP_UT_FAIL;
	}

	if (!strnstr(params.name, ent->name, strlen(ent->name))) {
		pr_info("Error: protocol name pit entry mismatch %s !~= %s\n",
			params.name, ent->name);
		res = PP_UT_FAIL;
	}

test_done:
	ret = prsr_proto_del(id);
	if (unlikely(ret)) {
		pr_info("Error: failed to delete protocol %s(%u)\n",
			params.name, id);
		res = PP_UT_FAIL;
	}

	return res;
}

/**
 * Register all pp_prsr tests to pput
 */
void pp_prsr_tests_init(void)
{
	int test_idx;

	/* Add Tests */
	for (test_idx = 0 ; test_idx < ARRAY_SIZE(pp_prsr_tests); test_idx++)
		pp_register_test(&pp_prsr_tests[test_idx]);
}
