/*
 * pp_bm_tests.c
 * Description: Packet Processor unit tests
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/string.h>
#include <linux/init.h>
#include <linux/device.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/bitops.h>
#include "../pput.h"
#include "bm.h"
#include "pp_buffer_mgr.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_BM_TEST] %s:%d: " fmt, __func__, __LINE__
#endif

struct pp_bmgr_init_param bmgr_cfg;
ulong active_policies[BITS_TO_LONGS(PP_BM_MAX_POLICIES)];
ulong active_pools[BITS_TO_LONGS(PP_BM_MAX_POOLS)];
ulong isolated_pools[BITS_TO_LONGS(PP_BM_MAX_POOLS)];

/**
 * @brief Get information from the buffers manager driver
 *        and set some stuff to enable the tests
 * @param data not used
 * @return s32
 */
static s32 pp_bm_pre_test(void *data)
{
	struct pp_bmgr_pool_params pool_prms;
	u32 pool_id;
	s32 ret;

	ret = pp_bmgr_config_get(&bmgr_cfg);
	if (ret) {
		pr_err("failed to get buffer manager config, ret %d\n", ret);
		return PP_UT_FAIL;
	}

	/* get all active pools and policies */
	bmgr_get_active_policies(active_policies, PP_BM_MAX_POLICIES);
	bmgr_get_active_pools(active_pools, PP_BM_MAX_POOLS);
	/* create 2 separate bitmaps for isolated and non-isolated pools */
	for_each_set_bit(pool_id, active_pools, PP_BM_MAX_POOLS) {
		ret = pp_bmgr_pool_conf_get(pool_id, &pool_prms);
		if (ret) {
			pr_err("failed to get pool %u params, ret %d\n",
			       pool_id, ret);
			return PP_UT_FAIL;
		}

		if (pool_prms.flags & POOL_ISOLATED) {
			set_bit(pool_id, isolated_pools);
			clear_bit(pool_id, active_pools);
		}
	}

	/* set number of pools per policy to more than 1
	 * for testing the group creation logic that
	 * prevent from 2 policies to share a subset of pools
	 */
	if (bmgr_cfg.max_pools_in_policy == 1)
		bmgr_pools_per_policy_set(2);

	return PP_UT_PASS;
}

/**
 * @brief Revert everything done in the pre test
 * @param data not used
 * @return s32
 */
static s32 pp_bm_post_test(void *data)
{
	/* revert max pools in policy back */
	bmgr_pools_per_policy_set(bmgr_cfg.max_pools_in_policy);

	return PP_UT_PASS;
}

/**
 * @brief Test if isolated pool can be used by more than 1 policy
 * @param data
 * @return s32 PP_UT_PASS in case it can't, PP_UT_FAIL otherwise
 */
static s32 pp_bm_isolated_pool_test(void *data)
{
	struct pp_bmgr_policy_params policy_cfg = { 0 };
	u16 policy_id;
	u8 pool_id;

	if (bitmap_weight(isolated_pools, PP_BM_MAX_POOLS) == 0) {
		pr_debug("failed to found isolated pool, skipping test\n");
		return PP_UT_PASS;
	}

	/* get an isolated pool */
	pool_id = find_first_bit(isolated_pools, PP_BM_MAX_POOLS);
	/* Re-configure last policy as isolated */
	policy_cfg.num_pools_in_policy = 1;
	policy_cfg.pools_in_policy[0].pool_id = pool_id;
	policy_cfg.max_allowed = 1;
	policy_cfg.min_guaranteed = 0;
	if (!pp_bmgr_policy_configure(&policy_cfg, &policy_id)) {
		pr_err("Configured 2 policies to use an isolated pool %u, test fail\n",
			pool_id);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Finds a policy which is part of a group with minimum guarantee
 *        configured and, to reduce the test time, finds a policy with
 *        the minimum number of buffers to pop for testing the
 *        minimum guarantee
 * @param n set to the number of buffers to pop to test the minimum
 *          guarantee
 * @return u32 group id if one exist, PP_BM_INVALID_GROUP_ID otherwise
 */
static u16 find_min_guarantee_policy(u32 *n)
{
	struct bmgr_group_db_entry grp_info;
	struct pp_bmgr_policy_params policy_prms;
	u32 grp_id, actual_allowed, min_policy_id, min_actual_allowed;
	u32 policy_id, tmp;
	s32 ret;

	min_actual_allowed = U32_MAX;
	min_policy_id = PP_BM_INVALID_POLICY_ID;
	for (grp_id = 0; grp_id < bmgr_cfg.max_groups; grp_id++) {
		ret = pp_bmgr_group_db_info_get(grp_id, &grp_info);
		if (ret) /* skip if the group isn't active */
			continue;

		if (!grp_info.reserved)
			continue;

		/* calc maximum number of buffers available to pop
		 * from the group
		 */
		tmp = grp_info.available - grp_info.reserved;
		pr_debug("checking group %u, tmp %u\n", grp_id, tmp);

		/* find the policy with the least amount of buffers
		 * to pop to test the minimum guarantee
		 */
		for_each_set_bit(policy_id, grp_info.policies_bmap,
				  PP_BM_MAX_POLICIES) {
			pp_bmgr_policy_conf_get(policy_id, &policy_prms);

			/* number of buffers needed to pop */
			actual_allowed = tmp + policy_prms.min_guaranteed;
			pr_debug("policy %u, actual_allowed %u, max_allowed %u, min guarantee %u\n",
				 policy_id, actual_allowed,
				 policy_prms.max_allowed,
				 policy_prms.min_guaranteed);

			/* check if the policy can pop enough buffers to
			 * test the minimum guarantee
			 */
			if (actual_allowed >= policy_prms.max_allowed) {
				pr_debug("not enough buffers to pop\n");
				continue;
			}

			if (actual_allowed < min_actual_allowed) {
				min_actual_allowed = actual_allowed;
				min_policy_id = policy_id;
			}
		}
	}

	if (min_policy_id == PP_BM_INVALID_POLICY_ID) {
		pr_info("Failed to find policy with minimum guarantee, skip test\n");
		return PP_BM_INVALID_POLICY_ID;
	}

	if (n)
		*n = min_actual_allowed;

	return min_policy_id;
}

struct buff_info {
	u32 pool;
	u64 ptr;
};

/**
 * @brief Search for a group of policies with minimum guarantee
 *        configured and try to allocate more than allowed
 *        on one of the policies
 * @param data no used
 * @return s32 PP_UT_PASS if the minimum guarantee rules were
 *             properly enforced, PP_UT_FAIL otherwise
 */
static s32 pp_bm_min_guarantee_test(void *data)
{
	struct buff_info *buffers_info;
	u32 actual_allowed, i, popped_buffers;
	u16 policy_id;

	/* find a group of pools with minimum guarantee configured */
	policy_id = find_min_guarantee_policy(&actual_allowed);
	if (policy_id == PP_BM_INVALID_POLICY_ID)
		return PP_UT_PASS;

	pr_info("Try to pop %u buffers from policy %u\n", actual_allowed + 1,
		 policy_id);

	buffers_info =
		kzalloc(sizeof(*buffers_info) * (actual_allowed + 1), GFP_KERNEL);
	if (!buffers_info) {
		pr_err("Failed to allocate memory for buffers pointers\n");
		return PP_UT_FAIL;
	}

	/* try to pop more buffers then the actual allowed */
	popped_buffers = 0;
	for (i = 0; i < actual_allowed + 1; i++) {
		bm_pop_buffer(policy_id, &buffers_info[i].pool,
			      &buffers_info[i].ptr);
		if (!buffers_info[i].ptr)
			break;
		pr_debug("popped buffer %#llx from policy %u, pool %u\n",
			buffers_info[i].ptr, policy_id, buffers_info[i].pool);
		popped_buffers++;
	}
	/* return all buffers back */
	for (i = 0; i < popped_buffers; i++)
		bm_push_buffer(policy_id, buffers_info[i].pool,
			       buffers_info[i].ptr);

	pr_info("%u buffers popped from policy %u\n", popped_buffers,
		 policy_id);

	kfree(buffers_info);
	/* if we managed to popped all the buffers, minimum guarantee
	 * don't work
	 */
	if (actual_allowed < popped_buffers) {
		pr_info("popped %u buffers from policy %u, maximum expected is %u\n",
			popped_buffers, policy_id, actual_allowed);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test whether 2 policies can be configured with same subset
 *        of pools
 * @param data no used
 * @return s32 PP_UT_PASS if the configuration fails, PP_UT_FAIL otherwise
 */
static s32 pp_bm_pools_subset_test(void *data)
{
	struct pp_bmgr_policy_params policy_prms;
	u8 pool_id, pool_id2;
	u16 policy_id;
	s32 ret;

	/* there must be at least 2 non-isolated pools exist */
	if (bitmap_weight(active_pools, PP_BM_MAX_POOLS) < 2) {
		pr_info("test requires at least 2 non-isolated pools, skip\n");
		return PP_UT_PASS;
	}

	pool_id  = find_first_bit(active_pools, PP_BM_MAX_POOLS);
	pool_id2 = find_next_bit(active_pools, PP_BM_MAX_POOLS, pool_id + 1);
	pr_info("pool_id %u, pool_id2 %u\n", pool_id, pool_id2);

	/* try to create new policy with 1 more pool */
	policy_prms.max_allowed = 2;
	policy_prms.min_guaranteed = 0;
	policy_prms.num_pools_in_policy = 2;
	policy_prms.pools_in_policy[0].max_allowed = 1;
	policy_prms.pools_in_policy[0].pool_id = pool_id;
	policy_prms.pools_in_policy[1].max_allowed = 1;
	policy_prms.pools_in_policy[1].pool_id = pool_id2;

	policy_id = PP_BM_INVALID_POLICY_ID;
	ret = pp_bmgr_policy_configure(&policy_prms, &policy_id);
	if (!ret || ret != -EINVAL) {
		pr_info("policy params valid failed with pools subset, test fail\n");
		pp_bmgr_policy_remove(policy_id);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test whether a policy can be configured with an inactive pool
 * @param data no used
 * @return s32 PP_UT_PASS if the configuration fails, PP_UT_FAIL otherwise
 */
static s32 pp_bm_inactive_pool_test(void *data)
{
	ulong all_pools[BITS_TO_LONGS(PP_BM_MAX_POOLS)];
	struct pp_bmgr_policy_params policy_prms;
	u16 policy_id;
	u8 pool_id;
	s32 ret;

	bitmap_or(all_pools, active_pools, isolated_pools, PP_BM_MAX_POOLS);
	pool_id = find_first_zero_bit(all_pools, PP_BM_MAX_POOLS);
	pr_info("inactive pool %u\n", pool_id);

	policy_prms.max_allowed = 1;
	policy_prms.min_guaranteed = 0;
	policy_prms.num_pools_in_policy = 1;
	policy_prms.pools_in_policy[0].max_allowed = 1;
	policy_prms.pools_in_policy[0].pool_id = pool_id;

	policy_id = PP_BM_INVALID_POLICY_ID;
	ret = pp_bmgr_policy_configure(&policy_prms, &policy_id);
	if (!ret || ret != -EINVAL) {
		pr_info("policy params valid failed with inactive pool, test fail\n");
		pp_bmgr_policy_remove(policy_id);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test whether a policy can be configured with a minimum
 *        guarantee value which is greather than the pool size
 * @param data no used
 * @return s32 PP_UT_PASS if the configuration fails, PP_UT_FAIL otherwise
 */
static s32 pp_bm_min_guarantee_overbooked_test(void *data)
{
	struct pp_bmgr_policy_params policy_prms;
	struct pp_bmgr_pool_params pool_prms;
	u16 policy_id;
	u8 pool_id;
	s32 ret;

	pool_id = find_first_bit(active_pools, PP_BM_MAX_POOLS);
	ret = pp_bmgr_pool_conf_get(pool_id, &pool_prms);
	if (ret) {
		pr_err("failed to get pool %u params, ret %d\n", pool_id, ret);
		return PP_UT_FAIL;
	}

	policy_prms.min_guaranteed = pool_prms.num_buffers + 1;
	policy_prms.max_allowed = pool_prms.num_buffers + 1;
	policy_prms.num_pools_in_policy = 1;
	policy_prms.pools_in_policy[0].max_allowed = pool_prms.num_buffers + 1;
	policy_prms.pools_in_policy[0].pool_id = pool_id;

	policy_id = PP_BM_INVALID_POLICY_ID;
	ret = pp_bmgr_policy_configure(&policy_prms, &policy_id);
	if (!ret || ret != -EINVAL) {
		pr_info("policy params valid failed with overbooked min guarantee, test fail\n");
		pp_bmgr_policy_remove(policy_id);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test a whether a policy can be configured with
 *        max allowed more than the pool size for every pool
 * @param data not used
 * @return s32
 */
static s32 pp_bm_max_allowed_overbooked_test(void *data)
{
	struct pp_bmgr_policy_params policy_prms;
	struct pp_bmgr_pool_params pool_prms;
	u16 policy_id;
	u8 pool_id;
	s32 ret;

	pool_id = find_first_bit(active_pools, PP_BM_MAX_POOLS);
	ret = pp_bmgr_pool_conf_get(pool_id, &pool_prms);
	if (ret) {
		pr_err("failed to get pool %u params, ret %d\n", pool_id, ret);
		return PP_UT_FAIL;
	}

	policy_prms.pools_in_policy[0].max_allowed = pool_prms.num_buffers + 1;
	policy_prms.pools_in_policy[0].pool_id = pool_id;
	policy_prms.num_pools_in_policy = 1;
	policy_prms.max_allowed = policy_prms.pools_in_policy[0].max_allowed;
	policy_prms.min_guaranteed = 0;

	policy_id = PP_BM_INVALID_POLICY_ID;
	ret = pp_bmgr_policy_configure(&policy_prms, &policy_id);
	/* we should also verify ret != -EINVAL to make sure
	 * we failed due to a wrong configuration and not something else
	 * but, due to cqm driver wrong configuration this specific checker
	 * isn't enable.
	 * @todo enable the checker in the driver and verify also ret != -EINVAL
	 */
	if (!ret) {
		pr_info("policy params valid failed with overbooked max allowed, test fail\n");
		pp_bmgr_policy_remove(policy_id);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

/**
 * @brief Test a whether a policy can be configured with
 *        max allowed smaller than the minimum guarantee, such
 *        configuration doesn't make any sense
 * @param data not used
 * @return s32
 */
static s32 pp_bm_max_allowed_min_guarantee_test(void *data)
{
	struct pp_bmgr_policy_params policy_prms;
	struct pp_bmgr_pool_params pool_prms;
	u16 policy_id;
	u8 pool_id;
	s32 ret;

	pool_id = find_first_bit(active_pools, PP_BM_MAX_POOLS);
	ret = pp_bmgr_pool_conf_get(pool_id, &pool_prms);
	if (ret) {
		pr_err("failed to get pool %u params, ret %d\n", pool_id, ret);
		return PP_UT_FAIL;
	}

	/* assuming here minimum pool size if */
	policy_prms.pools_in_policy[0].max_allowed = 1;
	policy_prms.pools_in_policy[0].pool_id = pool_id;
	policy_prms.num_pools_in_policy = 1;
	policy_prms.max_allowed = policy_prms.pools_in_policy[0].max_allowed;
	policy_prms.min_guaranteed = 2;

	policy_id = PP_BM_INVALID_POLICY_ID;
	ret = pp_bmgr_policy_configure(&policy_prms, &policy_id);
	if (!ret || ret != -EINVAL) {
		pr_info("policy params valid failed with overbooked max allowed, test fail\n");
		pp_bmgr_policy_remove(policy_id);
		return PP_UT_FAIL;
	}

	return PP_UT_PASS;
}

#ifdef PP_BM_ALLOCATION_TEST
static bool __validate_allocation(u8 pool_id, u16 policy_id, u32 num_buffers)
{
	struct pp_bmgr_pool_stats pool_stats;
	struct pp_bmgr_policy_stats policy_stats;

	/* Get Pool Stats */
	if (unlikely(pp_bmgr_pool_stats_get(&pool_stats, pool_id))) {
		pr_info("Getting pool %d stats failed\n", pool_id);
		goto validate_allocation_fail;
	}

	/* Get Policy Stats */
	if (unlikely(pp_bmgr_policy_stats_get(&policy_stats, policy_id))) {
		pr_info("Getting policy %d stats failed\n", policy_id);
		goto validate_allocation_fail;
	}

	/* Verify Pool Allocated Statistics */
	if (unlikely(pool_stats.pool_allocated_ctr != num_buffers)) {
		pr_info("Pool Allocated is %d instead of %d\n",
			pool_stats.pool_allocated_ctr, num_buffers);
		goto validate_allocation_fail;
	}

	/* Verify Policy Allocated Statistics */
	if (unlikely(policy_stats.policy_alloc_buff != num_buffers)) {
		pr_info("Policy Allocated is %d instead of %d\n",
			policy_stats.policy_alloc_buff, num_buffers);
		goto validate_allocation_fail;
	}

	return true;

validate_allocation_fail:
	return false;
}

static s32 __bm_validate_buffer(struct pp_bmgr_buff_info *buff_info,
				u8 pool_id)
{
	/* Verify pool is valid */
	if (unlikely(buff_info->pool_id[0] != pool_id)) {
		pr_info("Pool id is %d instead of %d\n",
			buff_info->pool_id[0], pool_id);
		return -1;
	}

	/* Make sure buffer is in address range */
	if ((buff_info->addr_low[0] <
	     DDR_BM_BUFF_POOL_BASE(pool_id)) ||
	    (buff_info->addr_low[0] >
	     DDR_BM_BUFF_POOL_BASE(pool_id + 1))) {
		pr_info("Buff 0x%x is not in pool %d range\n",
			buff_info->addr_low[0], pool_id);
		return -1;
	}

	return 0;
}

static s32 pp_bm_allocation_test(void *data)
{
	struct pp_bmgr_buff_info *buff_info;
	struct pp_bmgr_policy_params policy_params;
	u32 idx;
	u16 policy_id;
	u8  pool_id;

	buff_info = kcalloc(BM_TEST_MAX_ALLOWED_PER_POOL + 1,
			    sizeof(struct pp_bmgr_buff_info),
			    GFP_KERNEL);

	/* Pop all buffers from policy */
	for (policy_id = 0; policy_id < cfg.max_policies; policy_id++) {
		if (unlikely(pp_bmgr_policy_conf_get(policy_id,
						     &policy_params))) {
			pr_info("Config get for policy %d failed\n",
				policy_id);
			goto pp_bm_allocation_test_fail;
		}

		pool_id = policy_params.pools_in_policy[0].pool_id;

		/* Pop buffers */
		for (idx = 0; idx < BM_TEST_MAX_ALLOWED_PER_POOL; idx++) {
			buff_info[idx].num_allocs = 1;
			buff_info[idx].policy_id = policy_id;
			if (unlikely(bmgr_pop_buffer(&buff_info[idx]))) {
				pr_info("Pop from policy %d failed\n",
					policy_id);
				goto pp_bm_allocation_test_fail;
			}

			if (unlikely(__bm_validate_buffer(&buff_info[idx],
							  pool_id))) {
				pr_info("Pop from policy %d failed\n",
					policy_id);
				goto pp_bm_allocation_test_fail;
			}
		}

		/* Try popping extra buffer */
		buff_info[idx].num_allocs = 1;
		buff_info[idx].policy_id = policy_id;
		if (unlikely(bmgr_pop_buffer(&buff_info[idx]))) {
			pr_info("Pop from policy %d failed\n", policy_id);
			goto pp_bm_allocation_test_fail;
		}

		/* Pop should fail */
		if (unlikely(!__bm_validate_buffer(&buff_info[idx],
						   pool_id))) {
			pr_info("Error: Extra pop from policy %d succeeded\n",
				policy_id);
			goto pp_bm_allocation_test_fail;
		}

		if (unlikely(!__validate_allocation(pool_id, policy_id,
					   BM_TEST_MAX_ALLOWED_PER_POOL))) {
			pr_info("Allocation validation failed\n");
			goto pp_bm_allocation_test_fail;
		}

		/* Push buffers back */
		for (idx = 0; idx < BM_TEST_MAX_ALLOWED_PER_POOL; idx++) {
			if (unlikely(bmgr_push_buffer(&buff_info[idx]))) {
				pr_info("Pop from policy %d failed\n",
					policy_id);
				goto pp_bm_allocation_test_fail;
			}
		}

		if (unlikely(!__validate_allocation(pool_id, policy_id, 0))) {
			pr_info("Allocation validation failed\n");
			goto pp_bm_allocation_test_fail;
		}
	}

	kfree(buff_info);
	return PP_UT_PASS;

pp_bm_allocation_test_fail:
	kfree(buff_info);
	return PP_UT_FAIL;
}
#endif

/**
 * Register all pp_bm tests to pput
 */
void pp_bm_tests_init(void)
{
	s32 test_idx;

	struct pp_test_t pp_bm_tests[] = {
		{
			.component = PP_BM_TESTS,
			.test_name = "pre_test",
			.level_bitmap = PP_UT_LEVEL_FULL |
					PP_UT_LEVEL_MUST_PASS |
					PP_UT_LEVEL_MUST_RUN,
			.test_data = NULL,
			.fn = pp_bm_pre_test,
		},
		{
			.component = PP_BM_TESTS,
			.test_name = "isolated_pool_test",
			.level_bitmap =
				PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
			.test_data = NULL,
			.fn = pp_bm_isolated_pool_test,
		},
		{
			.component = PP_BM_TESTS,
			.test_name = "min_guarantee_test",
			.level_bitmap =
				PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
			.test_data = NULL,
			.fn = pp_bm_min_guarantee_test,
		},
		{
			.component = PP_BM_TESTS,
			.test_name = "pools_subset_test",
			.level_bitmap =
				PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
			.test_data = NULL,
			.fn = pp_bm_pools_subset_test,
		},
		{
			.component = PP_BM_TESTS,
			.test_name = "inactive_pool_test",
			.level_bitmap =
				PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
			.test_data = NULL,
			.fn = pp_bm_inactive_pool_test,
		},
		{
			.component = PP_BM_TESTS,
			.test_name = "max_allowed_min_guarantee_test",
			.level_bitmap =
				PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
			.test_data = NULL,
			.fn = pp_bm_max_allowed_min_guarantee_test,
		},
		{
			.component = PP_BM_TESTS,
			.test_name = "min_guarantee_overbooked_tes",
			.level_bitmap =
				PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
			.test_data = NULL,
			.fn = pp_bm_min_guarantee_overbooked_test,
		},
		{
			.component = PP_BM_TESTS,
			.test_name = "max_allowed_overbooked_test",
			.level_bitmap =
				PP_UT_LEVEL_FULL | PP_UT_LEVEL_MUST_PASS,
			.test_data = NULL,
			.fn = pp_bm_max_allowed_overbooked_test,
		},
#ifdef PP_BM_ALLOCATION_TEST
		{
			.component = PP_BM_TESTS,
			.test_name = "pp_bm_allocation_test",
			.level_bitmap = PP_UT_LEVEL_FULL,
			.test_data = NULL,
			.fn = pp_bm_allocation_test,
		},
#endif
		{
			.component = PP_BM_TESTS,
			.test_name = "pp_bm_pre_test",
			.level_bitmap = PP_UT_LEVEL_FULL |
					PP_UT_LEVEL_MUST_PASS |
					PP_UT_LEVEL_MUST_RUN,
			.test_data = NULL,
			.fn = pp_bm_post_test,
		},
	};

	/* Add Tests */
	for (test_idx = 0 ; test_idx < ARRAY_SIZE(pp_bm_tests); test_idx++)
		pp_register_test(&pp_bm_tests[test_idx]);
}
