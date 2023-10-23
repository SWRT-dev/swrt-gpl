/*
 * pp_bm.c
 * Description: Packet Processor Buffer Manager Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2017-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_BMGR]:%s:%d: " fmt, __func__, __LINE__

#include <linux/platform_device.h>
#include <linux/device.h>
#include <linux/kernel.h>
#include <linux/debugfs.h>
#include <linux/moduleparam.h>
#include <linux/slab.h>
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/module.h>
#include <linux/delay.h>
#include <linux/time.h>
#include <linux/workqueue.h>
#include <linux/dma-mapping.h>
#include <linux/bitfield.h>

#include "pp_common.h"
#include "pp_qos_utils.h"
#include "pp_regs.h"
#include "pp_buffer_mgr.h"
#include "bm.h"
#include "pp_buffer_mgr_internal.h"

static u64 qos_uc_base_addr;

#define QOS_UC_MCDM0_OFF     (0x10000)
#define MCDMA_SRC_OFFSET     (0)
#define MCDMA_DST_OFFSET     (0x4)
#define MCDMA_CONTROL_OFFSET (0x8)
#define MCDMA_ACTIVE_MASK    (BIT(30))

static void __bmgr_work_handler(struct work_struct *w);

static struct bmgr_driver_db *db;
static struct workqueue_struct *bmgr_wq;

/**
 * @brief This structure is used to pass data to the work queue
 * @work work queue structure
 * @policy_id policy to reset
 */
struct policy_reset_action {
	struct delayed_work work;
	u16    policy_id;
	void   (*cb)(s32 ret);
};

/**
 * @brief This function locks the DB in a recursion-save manner
 * @return void
 */
static inline void __bmgr_db_lock(void)
{
	spin_lock_bh(&db->db_lock);
}

/**
 * @brief This function unlocks the DB in a recursion-save manner
 * @return void
 */
static inline void __bmgr_db_unlock(void)
{
	spin_unlock_bh(&db->db_lock);
}

static inline bool __bmgr_is_ready(void)
{
	if (likely(db && db->ready))
		return true;

	pr_err("PP buffer manager isn't ready!\n");
	return false;
}

static inline bool bmgr_is_group_id_valid(u32 id)
{
	return (id < db->cfg.max_groups);
}

static inline bool bmgr_is_pool_id_valid(u32 id)
{
	if (likely(id < db->cfg.max_pools))
		return true;

	pr_err("Invalid pool id %d\n", id);
	return false;
}

static inline bool bmgr_is_policy_id_valid(u32 id)
{
	if (likely(id < db->cfg.max_policies))
		return true;

	pr_err("Invalid policy id %d\n", id);
	return false;
}

/**
 * @brief Check wheather pool parameters are valid
 * @param pool_params: Pool param from user
 * @return 0 on success, other error code on failure
 */
static s32
__bmgr_is_pool_params_valid(const struct pp_bmgr_pool_params *const pool_params)
{
	/* Validity check */
	if (unlikely(!pool_params)) {
		pr_err("pool_params is NULL\n");
		return -EINVAL;
	}

	if (unlikely(pool_params->size_of_buffer < BM_MIN_POOL_BUFFER_SIZE)) {
		pr_err("size_of_buffer %d should be larger than %d\n",
		       pool_params->size_of_buffer,
		       BM_MIN_POOL_BUFFER_SIZE);
		return -EINVAL;
	}

	if (unlikely(pool_params->size_of_buffer % BM_MIN_POOL_BUFFER_SIZE)) {
		pr_err("size_of_buffer %d must be aligned to %d bytes\n",
		       pool_params->size_of_buffer,
		       BM_MIN_POOL_BUFFER_SIZE);
		return -EINVAL;
	}

	if (unlikely(pool_params->base_addr_low & 0x3F)) {
		pr_err("base_addr_low %d must be aligned to %d bytes\n",
		       pool_params->base_addr_low,
		       BM_MIN_POOL_BUFFER_SIZE);
		return -EINVAL;
	}

	if (!IS_ALIGNED(pool_params->num_buffers, 64)) {
		pr_err("Number of buffers must be aligned to 64\n");
		return -EINVAL;
	}

	/* Num_buffers can be up to 2^24 */
	if (unlikely(pool_params->num_buffers >= 0x1000000)) {
		pr_err("Number of buffers can be up to 0x1000000\n");
		return -EINVAL;
	}

	return 0;
}

/**
 * @brief Free buffer pool
 * @param pool the pool id
 * @return s32
 */
static void free_pool(u32 pool)
{
	clear_bit(pool, db->pools_bmap);
	db->num_pools--;
}

/**
 * @brief Test whether pool is used or not
 * @param pool the pool id
 * @return bool true if the pool is used, false otherwise
 */
static bool is_pool_busy(u32 pool)
{
	return test_bit(pool, db->pools_bmap);
}

/**
 * @brief Allocate free pool
 *        If a valid pool is specified, try to allocate it
 *        otherwise, try to allocate a free pool
 * @param pool the allocated pool id
 * @return s32 0 on succesfull allocation, error code otherwise
 */
static s32 alloc_pool(u8 *pool)
{
	if (db->num_pools == db->cfg.max_pools) {
		pr_err("no free pool available\n");
		return -ENOENT;
	}

	if (*pool == PP_BM_INVALID_POOL_ID)
		*pool = find_first_zero_bit(db->pools_bmap, PP_BM_MAX_POOLS);
	if (!bmgr_is_pool_id_valid(*pool))
		return -EINVAL;
	if (is_pool_busy(*pool)) {
		pr_err("Pool %d id active\n", *pool);
		return -EBUSY;
	}

	set_bit(*pool, db->pools_bmap);
	db->num_pools++;

	return 0;
}

/**
 * @brief Test whether 2 policies params are equal
 *        Policies params are considered to be equal if the following apply:
 *        1. The share the exact same pools
 * @param a policy a params
 * @param b policy b params
 * @return bool true if both are equal, false otherwise
 */
static bool policy_params_cmp(const struct pp_bmgr_policy_params *a,
			      const struct pp_bmgr_policy_params *b)
{
	u32 i;
	bool ret = true;

	if (a->num_pools_in_policy != b->num_pools_in_policy) {
		pr_err("num_pools_in_policy mismatch\n");
		return false;
	}

	for (i = 0; i < a->num_pools_in_policy; i++) {
		if (a->pools_in_policy[i].pool_id !=
		    b->pools_in_policy[i].pool_id) {
			pr_err("a->pools[%u] = %u != b->pools[%u] = %u\n", i,
			       a->pools_in_policy[i].pool_id, i,
			       b->pools_in_policy[i].pool_id);
			ret = false;
		}
	}

	return ret;
}

/**
 * @brief Check wheather policy parameters are valid
 * @param prms Policy params from user
 * @param is_isolated specify if the policy is isolated or not
 * @return 0 on success, other error code on failure
 */
static s32 bmgr_is_policy_params_valid(const struct pp_bmgr_policy_params *prms,
				       bool *is_isolated)
{
	ulong policies[BITS_TO_LONGS(PP_BM_MAX_POLICIES)] = { 0 };
	ulong pools[BITS_TO_LONGS(PP_BM_MAX_POOLS)] = { 0 };
	struct pp_bmgr_policy_params *other;
	struct bmgr_pool_db_entry *pool;
	u32 i, pool_id, other_id, isolated_pools;
	u32 total_buff, total_max_allowed, total_min_guarantee;
	s32 ret = 0;

	if (unlikely(ptr_is_null(prms)))
		return -EINVAL;

	if (unlikely(!prms->num_pools_in_policy)) {
		pr_err("policy must have at least 1 pool\n");
		return -EINVAL;
	}
	if (unlikely(prms->num_pools_in_policy > db->cfg.max_pools_in_policy)) {
		pr_err("num_pools_in_policy %d should be up to %d\n",
		       prms->num_pools_in_policy,
		       db->cfg.max_pools_in_policy);
		ret = -EINVAL;
	}

	isolated_pools = 0;
	total_buff = 0;
	total_max_allowed = 0;
	for (i = 0; i < prms->num_pools_in_policy; i++) {
		pool_id = prms->pools_in_policy[i].pool_id;
		set_bit(pool_id, pools);

		/* Verify pool in policy is valid */
		if (unlikely(!bmgr_is_pool_id_valid(pool_id))) {
			ret = -EINVAL;
			continue;
		}

		/* Verify pool in policy is active */
		if (unlikely(!is_pool_busy(pool_id))) {
			pr_err("pool_id %d is not active\n", pool_id);
			ret = -EINVAL;
			continue;
		}

		pool = &db->pools[pool_id];
		/* Verify isolated pool is not used by any policy */
		if (pool->params.flags & POOL_ISOLATED) {
			if (pool->policy_ref_cnt) {
				pr_err("Isolated Pool %d already used by other policy\n",
				       pool_id);
				ret = -EINVAL;
				continue;
			}
			isolated_pools++;
		}

		total_max_allowed += prms->pools_in_policy[i].max_allowed;
		total_buff += pool->params.num_buffers;
		/* verify the policy max allowed from the current pool
		 * isn't bigger then the pool size
		 */
		if (prms->pools_in_policy[i].max_allowed >
		    pool->params.num_buffers) {
			pr_err("max allowed(%u) cannot be bigger than the pool%u size(%u)\n",
			       prms->pools_in_policy[i].max_allowed,
			       pool->pool_id, pool->params.num_buffers);
			// ret = -EINVAL;
		}

		if (pool->policy_ref_cnt) {
			/* gather all the policies that uses the same
			 * set of pools into 1 bitmap
			 */
			bitmap_or(policies, policies,
				  pool->policies_bmap, PP_BM_MAX_POLICIES);

			/* verify the policy uses the same pools of a policy
			 * that already use the current pool
			 */
			other_id = find_first_bit(pool->policies_bmap,
						  PP_BM_MAX_POLICIES);
			other = &db->policies[other_id].params;
			if (!policy_params_cmp(other, prms)) {
				pr_err("pools mismatch with policy %u\n",
				       other_id);
				ret = -EINVAL;
			}
		}
	}

	/* sum the total minimum guerantee of all policies
	 * that uses the same set of pools
	 */
	total_min_guarantee = prms->min_guaranteed;
	for_each_set_bit(i, policies, PP_BM_MAX_POLICIES)
		total_min_guarantee += db->policies[i].params.min_guaranteed;

	/* verify the policy max allowed from all pools isn't bigger then
	 * total number of buffers of the pools
	 */
	if (total_max_allowed > total_buff) {
		pr_err("total max allowed of %u is bigger then %u buffers available in pools %*pbl\n",
		       total_max_allowed, total_buff, PP_BM_MAX_POOLS, pools);
		// ret = -EINVAL;
	}

	/* verify the policy minimum guarantee isn't bigger then the
	 * max allowed
	 */
	if (total_max_allowed < prms->min_guaranteed) {
		pr_err("total max allowed of %u cannot be less then the minimum guarantee of %u\n",
		       total_max_allowed, prms->min_guaranteed);
		ret = -EINVAL;
	}

	if (total_buff < total_min_guarantee) {
		pr_err("min guarantee of %u is causing overbooked on pools %*pbl\n",
		       total_min_guarantee, PP_BM_MAX_POOLS, pools);
		ret = -EINVAL;
	}

	/* If there is an isolated pool, all pools should be isolated */
	if (isolated_pools) {
		if (isolated_pools != prms->num_pools_in_policy) {
			pr_err("cannot mix isolated and non-isolated pools\n");
			ret = -EINVAL;
		}

		if (is_isolated)
			*is_isolated = true;
	} else {
		if (is_isolated)
			*is_isolated = false;
	}

	return ret;
}

static s32 alloc_group(u8 *group)
{
	if (db->num_groups == db->cfg.max_groups)
		return -ENOENT;

	*group = find_first_zero_bit(db->groups_bmap, BM_MAX_GROUPS);
	set_bit(*group, db->groups_bmap);
	db->num_groups++;

	return 0;
}

static s32 free_group(u32 group)
{
	clear_bit(group, db->groups_bmap);
	db->num_groups--;

	return 0;
}

static bool is_group_busy(u32 group)
{
	return test_bit(group, db->groups_bmap);
}

/**
 * @brief Enable pool pop
 * @param pool_id: Pool ID
 * @param enable: True to enable, false to disable
 * @return 0 on success, other error code on failure
 */
static s32 __bmgr_pool_pop_enable(u8 pool_id, bool enable)
{
	/* Pool Pop Enable */
	if (db->cfg.pool_pop_hw_en)
		return bm_pool_pop_enable(pool_id, enable);

	return 0;
}

/**
 * @brief get pool pop status
 * @param pool_id: Pool ID
 * @param enable: out arg, true if enabled, false if disabled
 * @return 0 on success, other error code on failure
 */
static s32 bmgr_pool_pop_status_get(u8 pool_id, bool *enable)
{
	*enable = true;

	/* Pool Pop Enable */
	if (db->cfg.pool_pop_hw_en)
		return bm_pool_pop_status_get(pool_id, enable);

	return 0;
}

/**
 * @brief Pause a group, disable popping buffers from the pool
 * @param pool the pool DB ptr
 * @param pause to pause or not
 * @param validate validate if all resources are back in the pool
 * @return s32 0 on success, error code otherwise
 */
static s32 pool_pause(struct bmgr_pool_db_entry *pool, bool pause,
		      bool validate)
{
	struct pp_bmgr_pool_stats pool_stats;
	bool pop_enable, pool_pop_enable;
	s32 ret = 0;

	/* pausing pool means to disable popping from it, so, pause
	 * means disable pop (false), resume means enable pop (true)
	 */
	pop_enable = pause ? false : true;

	/* Get the pool current pop enable status */
	ret = bmgr_pool_pop_status_get(pool->pool_id, &pool_pop_enable);
	if (unlikely(ret)) {
		pr_err("failed to get pool %u pop enable status\n",
			pool->pool_id);
		return ret;
	}

	/* if the pool is already in the right state, do nothing */
	if (pop_enable == pool_pop_enable)
		return 0;

	ret = __bmgr_pool_pop_enable(pool->pool_id, pop_enable);
	if (unlikely(ret)) {
		pr_err("failed to %s pool %u pop\n", BOOL2EN(pop_enable),
		       pool->pool_id);
		return ret;
	}

	/* if we enabled to pool or doesn't need to validate the resources,
	 * finish
	 */
	if (pop_enable || !validate)
		return 0;

	/* Verify the pool gained back its resources */
	ret = pp_bmgr_pool_stats_get(&pool_stats, pool->pool_id);
	if (unlikely(ret)) {
		pr_err("Failed to get pool %d stats\n", pool->pool_id);
		return ret;
	} else if (pool_stats.pool_allocated_ctr != 0) {
		pr_err("Pool %d resources are still in use\n", pool->pool_id);
		return -EBUSY;
	}

	return ret;
}

#define group_pause(id) group_pause_set(id, true, false)
#define group_pause_validate(id) group_pause_set(id, true, true)
#define group_resume(id) group_pause_set(id, false, false)

/**
 * @brief Pause/Unpause buffer allocation in pools associated
 *        with a group
 * @note This function should be called under db lock
 * @param group_id groop id to pause
 * @param cfg pause config
 * @return 0 on success, other error code on failure
 */
static s32 group_pause_set(u8 group_id, bool pause, bool validate)
{
	struct bmgr_group_db_entry *group;
	struct bmgr_pool_db_entry *pool;
	u32 id;
	s32 ret = 0;

	/* silently ignore */
	if (!bmgr_is_group_id_valid(group_id))
		return 0;

	/* Group entry in db */
	group = &db->groups[group_id];

	/* pause/continue all pools in group */
	for_each_set_bit(id, group->pools_bmap, PP_BM_MAX_POOLS) {
		pool = &db->pools[id];
		if (!is_pool_busy(pool->pool_id)) {
			pr_err("Pool %d in Group %d is not active\n",
			       pool->pool_id, group_id);
			continue;
		}

		ret = pool_pause(pool, pause, validate);
		if (unlikely(ret)) {
			pr_err("failed to %s pool %u, ret %d\n",
			       pause ? "pause" : "resume", pool->pool_id, ret);
			return ret;
		}
	}

	return 0;
}

#define policy_pause(id) policy_pause_set(id, true, false)
#define policy_pause_validate(id) policy_pause_set(id, true, true)
#define policy_resume(id) policy_pause_set(id, false, false)

/**
 * @brief Pause/resume buffer allocation all pools used by the
 *        specified policy
 * @note This function should be called under db lock
 * @param policy_id
 * @param pause whether to pause or resume
 * @param validate whether to validate if all resources are back
 * @return 0 on success, other error code on failure
 */
static s32 policy_pause_set(u8 policy_id, bool pause, bool validate)
{
	struct bmgr_policy_db_entry *policy;
	struct bmgr_pool_db_entry *pool;
	u32 i;
	s32 ret = 0;

	/* Group entry in db */
	policy = &db->policies[policy_id];

	/* pause/continue all pools in group */
	for (i = 0; i < policy->params.num_pools_in_policy; i++) {
		pool = &db->pools[policy->params.pools_in_policy[i].pool_id];
		if (!is_pool_busy(pool->pool_id)) {
			pr_err("Pool %u in policy %u is not active\n",
			       pool->pool_id, policy_id);
			continue;
		}

		ret = pool_pause(pool, pause, validate);
		if (unlikely(ret)) {
			pr_err("failed to %s pool %u, ret %d\n",
			       pause ? "pause" : "resume", pool->pool_id, ret);
			return ret;
		}
	}

	return 0;
}

/**
 * @brief Add pool to group
 * @note This function should be called under db lock
 * @param group_id group id to add pool to
 * @param pool_id pool id to add
 * @return 0 on success, other error code on failure
 */
static s32 add_pool_to_group(u8 group_id, u8 pool_id)
{
	struct bmgr_group_db_entry *grp;
	struct bmgr_pool_db_entry *pool;

	if (!is_group_busy(group_id))
		return -EINVAL;

	grp = &db->groups[group_id];
	pool = &db->pools[pool_id];

	if (test_bit(pool_id, grp->pools_bmap)) {
		pr_err("pool %u is already part of a group\n", pool_id);
		return -EINVAL;
	}


	grp->available += pool->params.num_buffers;
	set_bit(pool_id, grp->pools_bmap);

	return bm_group_available_set(group_id, grp->available);
}

/**
 * @brief Add pools to policy
 * @note This function should be called under db lock and assume
 *       group is not active
 * @param policy_id policy id to add pool to
 * @return 0 on success, other error code on failure
 */
static s32 add_pools_to_policy(u16 policy_id)
{
	struct pp_bmgr_policy_params *prms;
	u32 max = 0;
	s32    ret = 0;
	u8     pool_id;
	u8     idx = 0;

	prms = &db->policies[policy_id].params;

	/* Group entry in db */
	for (idx = 0; idx < prms->num_pools_in_policy; idx++) {
		pool_id = prms->pools_in_policy[idx].pool_id;

		/* Update pool/policy reference count */
		db->pools[pool_id].policy_ref_cnt++;
		set_bit(policy_id, db->pools[pool_id].policies_bmap);

		max = prms->pools_in_policy[idx].max_allowed;
		ret = bm_policy_pool_mapping_set(policy_id, pool_id,
						 idx, max, true);
		if (unlikely(ret)) {
			pr_err("bm_policy(%d)_pool(%d)_mapping_set Failed\n",
			       policy_id, pool_id);
			return ret;
		}
	}

	return ret;
}

/**
 * @brief Add pools to policy
 * @note This function should be called under db lock and assume
 *       group is not active
 * @param policy_id policy id to add pool to
 * @param group_id groop id to add pool to
 * @return 0 on success, other error code on failure
 */
static s32 add_policy_to_group(u16 policy_id, u8 group_id)
{
	struct bmgr_group_db_entry *group;
	struct pp_bmgr_policy_params *prms;
	s32 ret = 0;
	u8  i, pool_id;

	/* silently ignore */
	if (!bmgr_is_group_id_valid(group_id))
		goto done;

	/* Group entry in db */
	group = &db->groups[group_id];
	prms = &db->policies[policy_id].params;

	/* add all the policy's pool to the group */
	for (i = 0; i < prms->num_pools_in_policy; i++) {
		pool_id = prms->pools_in_policy[i].pool_id;

		if (db->pools[pool_id].policy_ref_cnt > 1)
			continue;
		/* Update group if it's the first time this pool is in use */
		ret = add_pool_to_group(group_id, pool_id);
		if (unlikely(ret)) {
			pr_err("__add_pool(%d)_to_group(%d) failed\n", pool_id,
			       group_id);
			return ret;
		}
	}

	set_bit(policy_id, group->policies_bmap);
	/* Set the group's reserved buffers */
	group->reserved += prms->min_guaranteed;
	if (bitmap_weight(group->policies_bmap, PP_BM_MAX_POLICIES) > 1) {
		ret = bm_group_reserved_set(group_id, group->reserved);
		if (unlikely(ret))
			pr_err("bm_group(%d)_reserved_set(%d) failed\n",
			       group_id, group->reserved);
	}

done:
	db->policies[policy_id].group_id = group_id;

	return ret;
}

 /**
 * @brief Remove a pool from a group
 * @note This function should be called under db lock
 * @param group_id group id to remove pool from
 * @param pool_id pool id to remove from group
 * @return 0 on success, other error code on failure
 */
static s32 remove_pool_from_group(u8 group_id, u8 pool_id)
{
	struct bmgr_group_db_entry *grp;
	struct bmgr_pool_db_entry *pool;
	u32 available = 0;

	if (!is_group_busy(group_id))
		return -EINVAL;

	pool = &db->pools[pool_id];
	grp = &db->groups[group_id];

	if (!test_bit(pool_id, grp->pools_bmap)) {
		pr_err("pool %u isn't part of any group\n", pool_id);
		return -EINVAL;
	}

	bm_group_available_get(group_id, &available);
	available -= pool->params.num_buffers;
	clear_bit(pool_id, grp->pools_bmap);

	/* Group's rsrvd buffers will be updated when configuring the policy */
	return bm_group_available_set(group_id, available);
}

/**
 * @brief Remove pools from policy
 * @note This function should be called under db lock and assume
 *       group is not active
 * @param policy_id policy id to remove pools from
 * @return 0 on success, other error code on failure
 */
static s32 remove_pools_from_policy(u16 policy_id)
{
	struct pp_bmgr_policy_params *prms;
	s32    ret = 0;
	u8     pool_id;
	u8     idx = 0;

	prms = &db->policies[policy_id].params;
	/* Traverse pools in policy */
	for (idx = 0; idx < prms->num_pools_in_policy; idx++) {
		pool_id = prms->pools_in_policy[idx].pool_id;
		db->pools[pool_id].policy_ref_cnt--;
		clear_bit(policy_id, db->pools[pool_id].policies_bmap);

		ret = bm_policy_pool_mapping_set(policy_id,
						 PP_BM_INVALID_POOL_ID,
						 idx, 0, true);
		if (unlikely(ret)) {
			pr_err("bm_policy(%d)_pool(%d)_mapping_set Failed\n",
			       policy_id, PP_BM_INVALID_POOL_ID);
			break;
		}
	}

	return ret;
}

/**
 * @brief Remove policy from a group
 * @note This function should be called under db lock and assume
 *       group is not active
 * @param policy_id policy id to remove pools from
 * @param group_id groop id to remove pools from
 * @return 0 on success, other error code on failure
 */
static s32 remove_policy_from_group(u16 policy_id, u8 group_id)
{
	struct pp_bmgr_policy_params *prms;
	struct bmgr_group_db_entry *group;
	u32 reserved, pool_id, idx;
	s32 ret = 0;

	/* silently ignore */
	if (!bmgr_is_group_id_valid(group_id))
		return 0;

	prms = &db->policies[policy_id].params;
	/* Traverse pools in policy */
	for (idx = 0; idx < prms->num_pools_in_policy; idx++) {
		pool_id = prms->pools_in_policy[idx].pool_id;

		/* Update group only if pool is not used by any policy */
		if (db->pools[pool_id].policy_ref_cnt == 1) {
			ret = remove_pool_from_group(group_id, pool_id);
			if (unlikely(ret)) {
				pr_err("__remove_pool(%d)_from_group(%d) failed\n",
				       pool_id, group_id);
				goto done;
			}
		}
	}

	/* Group entry in db */
	group = &db->groups[group_id];

	/* Reduce the group's reserved buffers */
	bm_group_reserved_get(group_id, &reserved);
	if (reserved < prms->min_guaranteed) {
		/* Sanity check in order not to go below 0 */
		pr_err("group %d rsrvd buffs (%d) < policy %d min guaranteed\n",
		       group_id,
		       reserved,
		       prms->min_guaranteed);
		reserved = 0;
	} else {
		reserved -= prms->min_guaranteed;
	}

	ret = bm_group_reserved_set(group_id, reserved);
	if (unlikely(ret))
		pr_err("bm_group(%d)_reserved_set(%d) failed\n", group_id,
		       reserved);

	db->policies[policy_id].group_id = PP_BM_INVALID_GROUP_ID;
	clear_bit(policy_id, group->policies_bmap);

done:
	return ret;
}

/**
 * @brief Enable pool
 * @param pool_id: Pool ID
 * @param enable: True to enable, false to disable
 * @return 0 on success, other error code on failure
 */
static s32 __bmgr_pool_enable(u8 pool_id, bool enable)
{
	s32 ret = 0;

	ret = bm_pool_enable(pool_id, enable);
	if (unlikely(ret)) {
		pr_err("bm_pool_enable %d failed\n", pool_id);
		return -EINVAL;
	}

	ret = __bmgr_pool_pop_enable(pool_id, enable);
	if (unlikely(ret))
		pr_err("__bmgr_pool_pop_enable %d failed\n", pool_id);

	return ret;
}

/**
 * @brief Convert buffer pointer of a specific pool to an index
 * @param pool_id the pool
 * @param buff the buffer to convert
 * @return u32 the index
 */
static inline u32 buff_ptr_to_index(u32 pool_id, u64 buff)
{
	struct bmgr_pool_db_entry *pool = &db->pools[pool_id];
	u64 index = buff - (u64)pool->phys;

	do_div(index, pool->params.size_of_buffer);

	return index;
}

/**
 * @brief Convert buffer index of a specific pool to a pointer
 * @param pool_id the pool
 * @param index the index
 * @return u64 the pointer
 */
static inline u64 buff_index_to_ptr(u32 pool_id, u32 index)
{
	struct bmgr_pool_db_entry *pool = &db->pools[pool_id];

	return (index * pool->params.size_of_buffer) + pool->phys;
}

/**
 * @brief Test whether a buffer pointer is in the range of a pool
 *        addresses range
 * @param pool_id the pool
 * @param buff the buffer
 * @return bool True if it is in the range, false otherwise
 */
static inline bool bmgr_is_buff_in_pool_range(u32 pool_id, u64 buff)
{
	struct bmgr_pool_db_entry *pool = &db->pools[pool_id];
	u64 base, end;

	base = end = pool->phys;
	end += pool->params.num_buffers * pool->params.size_of_buffer;

	return (base <= buff && buff < end);
}

/**
 * @brief Disable/Enable pop for all the policy's pools specified by a bitmap
 * @param policy the policy
 * @param bmap bitmap specified for which pools the pop should be
 *             disabled/enabled
 */
static s32 bmgr_policy_pool_pop_en(u32 policy_id, ulong bmap, bool en)
{
	struct bmgr_policy_db_entry *policy;
	struct pp_bmgr_pool_in_policy_info *pool_in_policy;
	s32 ret;
	u32 i;

	policy = &db->policies[policy_id];
	for (i = 0; i < policy->params.num_pools_in_policy; i++) {
		pool_in_policy = &policy->params.pools_in_policy[i];

		/* if the pool isn't set in the bitmap, do nothing */
		if (!test_bit(pool_in_policy->pool_id, &bmap))
			continue;

		/* else, disable/enable the pool */
		ret = __bmgr_pool_pop_enable(pool_in_policy->pool_id, en);
		if (!ret)
			continue;
		pr_err("fail to disable pool %u pop\n",
		       pool_in_policy->pool_id);
	}

	return 0;
}

/**
 * @brief Pops all the buffers from the specified policy and test:
 *          1. if the actual pool is as expected
 *          2. if all the buffers are in range
 *          3. if there are duplicates buffers
 *        It returns 4 values:
 *          1. bitmap contains all the popped buffers, bit per buffer
 *          2. number of out of range buffers
 *          3. number of duplicates buffers
 *          4. number of valid popped buffers
 * @param policy the policy to pop from
 * @param pool the expected pool
 * @param bmap the bitmap
 * @param out_of_range out of range counter
 * @param duplicates duplicates counter
 * @param num valid counter
 * @return s32 0 on successfull run, error code otherwise
 */
static s32 bmgr_pool_policy_verify(u32 policy, u32 pool, ulong *bmap,
				   u32 *out_of_range, u32 *duplicates,
				   u32 *num)
{
	u64 buff = 0;
	u32 real_pool;
	s32 ret;

	do {
		ret = bm_pop_buffer(policy, &real_pool, &buff);
		if (ret) {
			pr_err("failed to pop buffer from policy %u, ret %d\n",
				policy, ret);
			return ret;
		}
		if (!buff)
			break;

		pr_debug("pop buffer %#llx, bit %u\n", buff,
			 buff_ptr_to_index(pool, buff));

		if (real_pool != pool) {
			pr_debug("wrong pool %u, expected %u, policy %u\n",
			       real_pool, pool, policy);
			return -EINVAL;
		}

		/* check if buffer is in the pool range */
		if (!bmgr_is_buff_in_pool_range(pool, buff)) {
			pr_debug("found out of range buff %#llx\n", buff);
			if (out_of_range)
				(*out_of_range)++;
			continue;
		}
		/* check if buffer is duplicated */
		if (test_bit(buff_ptr_to_index(pool, buff), bmap)) {
			pr_debug("found duplicate buff %#llx\n", buff);
			if (duplicates)
				(*duplicates)++;
			continue;
		}

		set_bit(buff_ptr_to_index(pool, buff), bmap);
		if (num)
			(*num)++;

	} while (buff);

	return 0;
}

/**
 * @brief Try to push 'num' amount of buffers specified as indexes
 *        by 'bitmap' to 'policy' and 'pool'
 *        Returns tha actual number of buffers pushed
 * @param policy the policy
 * @param pool the pool
 * @param bmap the bitmap
 * @param num number of buffers to push
 * @param actual actual num of buffers pushed
 * @return s32 0 on success regardless of the actual pushed buffers,
 *             error code otherwise
 */
static s32 bmgr_pool_push_buffers(u32 policy, u32 pool, ulong *bmap, u32 num,
				  u32 *actual)
{
	u64 buff;
	u32 index, pool_size;
	s32 ret;

	pool_size = db->pools[pool].params.num_buffers;
	*actual   = 0;
	for_each_set_bit(index, bmap, pool_size) {
		if (!num)
			return 0;

		buff = buff_index_to_ptr(pool, index);
		ret = bm_push_buffer(policy, pool, buff);
		if (ret) {
			pr_err("failed to push buff %#llx to policy %u pool %u, ret %d\n",
			       buff, policy, pool, ret);
			return ret;
		}
		pr_debug("push buffer %#llx, bit %u\n", buff, index);

		num--;
		(*actual)++;
		clear_bit(index, bmap);
	}

	return 0;
}

s32 pp_bmgr_pool_verification(u32 id)
{
	struct pp_bmgr_policy_stats stats;
	struct bmgr_policy_db_entry *policy;
	struct pp_bmgr_pool_in_policy_info *pool_in_policy;
	struct bmgr_pool_db_entry *pool;
	u32 num_pop[PP_BM_MAX_POLICIES]; /* per policy num of buffers popped */
	ulong *buf_bmap;
	u32 i, policy_id, out_of_range, duplicates, total;
	s32 ret;

	if (unlikely(!bmgr_is_pool_id_valid(id)))
		return -EINVAL;

	__bmgr_db_lock();

	pool = &db->pools[id];
	if (unlikely(!is_pool_busy(id))) {
		pr_err("pool %u isn't active\n", id);
		__bmgr_db_unlock();
		return -EINVAL;
	}

	/* Sum the number of buffers can be popped from all policies
	 * from the specified pool
	 */
	total = 0;
	for_each_set_bit(policy_id, pool->policies_bmap, PP_BM_MAX_POLICIES) {
		bm_policy_stats_get(&stats, policy_id);
		policy = &db->policies[policy_id];

		for (i = 0; i < policy->params.num_pools_in_policy; i++) {
			pool_in_policy = &policy->params.pools_in_policy[i];
			if (pool_in_policy->pool_id == id)
				total += pool_in_policy->max_allowed;
		}
	}
	if (total < pool->params.num_buffers)
		pr_warn("Warning: bad configuration, results may not be accurate\n");

	/* allocate bitmap of num_buffers bits, to mark every buffer we
	 * found inside the pool to detect duplicate buffers
	 */
	buf_bmap = kcalloc(BITS_TO_LONGS(pool->params.num_buffers),
			   sizeof(*buf_bmap), GFP_ATOMIC);
	if (!buf_bmap) {
		pr_err("failed to allocate %u bits bitmap for pool %u\n",
		       pool->params.num_buffers, id);
		__bmgr_db_unlock();
		return -ENOMEM;
	}

	pr_info("Start verification of pool %u\n", pool->pool_id);
	pr_info("\tpool base %#llx\n", (u64)pool->phys);
	pr_info("\tnumber of buffers %u\n", pool->params.num_buffers);
	pr_info("\tbuffers to pop %u\n", total);
	pr_info("\tbuffer size %u\n", pool->params.size_of_buffer);
	pr_info("\tused by policies %*pbl\n", PP_BM_MAX_POLICIES,
		pool->policies_bmap);
	/* iterate over all policies that uses the pool pop all the buffers */
	memset(num_pop, 0, sizeof(num_pop));
	out_of_range = 0;
	duplicates = 0;
	total = 0;
	for_each_set_bit(policy_id, pool->policies_bmap, PP_BM_MAX_POLICIES) {
		pr_info("Verifying policy %u\n", policy_id);

		/* disable pop for all the pools used by current policy
		 * other than our pool
		 */
		ret = bmgr_policy_pool_pop_en(policy_id, ~BIT(pool->pool_id),
					      false);
		if (ret)
			goto free_mem;

		bmgr_pool_policy_verify(policy_id, pool->pool_id, buf_bmap,
					&out_of_range, &duplicates,
					&num_pop[policy_id]);

		total += num_pop[policy_id];

		pr_info("popped %u buffers from policy %u\n",
			num_pop[policy_id], policy_id);

		/* enable back the pop for all the pools in policy */
		ret = bmgr_policy_pool_pop_en(policy_id, ~BIT(pool->pool_id),
					      true);
		if (ret)
			goto free_mem;
	}

	/* return all the buffers back to the pool */
	for_each_set_bit(policy_id, pool->policies_bmap, PP_BM_MAX_POLICIES) {
		bmgr_pool_push_buffers(policy_id, pool->pool_id, buf_bmap,
				       num_pop[policy_id], &num_pop[policy_id]);
		pr_info("push %u buffers to policy %u\n", num_pop[policy_id],
			policy_id);
	}

	pr_info("Pool %u: pool size %u, total popped %u, missing %u, out of range %u, duplicates %u\n",
		pool->pool_id, pool->params.num_buffers, total,
		pool->params.num_buffers - total, out_of_range,
		duplicates);

free_mem:
	__bmgr_db_unlock();
	kfree(buf_bmap);

	return 0;
}

static s32
__bmgr_pool_configure(const struct pp_bmgr_pool_params * const pool_params,
		      u8 *pool_id, bool lock)
{
	struct bmgr_pool_db_entry *pool;
	struct bmgr_pool_cfg cfg;
	u32 *temp_pointers_table_ptr = NULL;
	u32  idx = 0;
	u32  low_addr, high_addr;
	s32  ret = 0;
	u64  user_array_ptr;
	bool pool_enable;

	/* Validity check */
	ret = __bmgr_is_pool_params_valid(pool_params);
	if (unlikely(ret))
		return ret;

	if (lock)
		__bmgr_db_lock();

	/* Find next available slot in pools db */
	ret = alloc_pool(pool_id);
	if (ret)
		goto unlock;

	pr_debug("Configuring pool %d\n", *pool_id);
	pool = &db->pools[*pool_id];

	if (lock)
		__bmgr_db_unlock();

	/* if the base address is zero, allocate the pool memory */
	if (!pool_params->base_addr_high && !pool_params->base_addr_low) {
		pool->sz =
			pool_params->num_buffers * pool_params->size_of_buffer;
		pool->sz = PAGE_ALIGN(pool->sz);
		pool->virt = pp_dma_alloc(pool->sz, GFP_ATOMIC, &pool->phys, true);

		if (!pool->virt) {
			pr_err("Could not allocate %u buffers of %u size\n",
			       pool_params->num_buffers,
			       pool_params->size_of_buffer);
			ret = -ENOMEM;
			goto free_memory;
		}
		pr_info("allocate pool buffers @ %#llx\n", (u64)pool->phys);
		low_addr = (u32)pool->phys;
#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
		high_addr = pool->phys >> BITS_PER_U32;
#else
		high_addr = 0;
#endif
	} else {
		low_addr = pool_params->base_addr_low;
		high_addr = pool_params->base_addr_high;
		pool->phys = PP_BUILD_64BIT_WORD(low_addr, high_addr);
	}

	/* Allocate pool_param->pool_num_of_buff * POINTER_SIZE bytes array */
	pool->internal_ptrs_sz = PAGE_ALIGN(sizeof(unsigned int) *
					    pool_params->num_buffers);
	pool->internal_ptrs_virt = pp_dma_alloc(pool->internal_ptrs_sz,
						GFP_ATOMIC,
						&pool->internal_ptrs_phys, true);

	if (!pool->internal_ptrs_virt) {
		pr_err("Could not allocate %u bytes for pool %u pointers\n",
		       (u32)pool->internal_ptrs_sz, *pool_id);
		/* Mark pool as not used since pool allocation failed */
		ret = -ENOMEM;
		goto free_memory;
	}

	if (lock)
		__bmgr_db_lock();

	temp_pointers_table_ptr = (unsigned int *)pool->internal_ptrs_virt;
	user_array_ptr = (low_addr) | ((u64)high_addr << BITS_PER_U32);

	for (idx = 0; idx < pool_params->num_buffers; idx++) {
		*temp_pointers_table_ptr = user_array_ptr >> 6;
		temp_pointers_table_ptr++;
		user_array_ptr += pool_params->size_of_buffer;
	}
	pp_cache_writeback(pool->internal_ptrs_virt, pool->internal_ptrs_sz);

	cfg.num_buffers = pool_params->num_buffers;
	cfg.ptr_table_addr_low = (u32)pool->internal_ptrs_phys;
	cfg.ptr_table_addr_high = (u32)((u64)pool->internal_ptrs_phys >> 32);
	cfg.min_guaranteed_enable = false;
	ret = bm_pool_configure(*pool_id, &cfg);
	if (unlikely(ret)) {
		pr_err("bm_pool_configure %d failed\n", *pool_id);
		goto free_memory;
	}

	pool_enable = true;
	ret = __bmgr_pool_enable(*pool_id, pool_enable);
	if (unlikely(ret)) {
		pr_err("__bmgr_pool_enable %d failed\n", *pool_id);
		goto free_memory;
	}

	/* Update pool's DB */
	memcpy(&pool->params, pool_params,
	       sizeof(struct pp_bmgr_pool_params));

	if (bm_ctrl_poll()) {
		pr_err("bm_ctrl_poll failed\n");
		ret = -EBUSY;
	}

unlock:
	if (lock)
		__bmgr_db_unlock();

	pr_debug("done (%d)\n", ret);
	return ret;

free_memory:
	/* free pointers_table */
	if (pool->internal_ptrs_virt) {
		pp_dma_free(pool->internal_ptrs_sz,
			    (void *)pool->internal_ptrs_virt,
			    &pool->internal_ptrs_phys, true);
		pool->internal_ptrs_sz = 0;
	}
	if (pool->virt) {
		pp_dma_free(pool->sz, pool->virt, &pool->phys, true);
		pool->virt = 0;
	}
	free_pool(*pool_id);
	if (lock)
		__bmgr_db_unlock();

	return ret;
}

s32
pp_bmgr_pool_configure(const struct pp_bmgr_pool_params * const pool_params,
		       u8 *pool_id)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	return __bmgr_pool_configure(pool_params, pool_id, true);
}
EXPORT_SYMBOL(pp_bmgr_pool_configure);

s32 pp_bmgr_pool_pop_disable(u8 pool_id)
{
	bool pool_pop_enable;
	s32 ret = 0;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	/* Verify pool if is valid */
	if (!bmgr_is_pool_id_valid(pool_id))
		return -EINVAL;

	__bmgr_db_lock();

	/* Verify pool is active */
	if (!is_pool_busy(pool_id)) {
		pr_err("Pool %d id not active\n", pool_id);
		ret = -EINVAL;
		goto unlock;
	}

	/* Verify pool is not used by any policy */
	if (unlikely(db->pools[pool_id].policy_ref_cnt)) {
		pr_err("Pool %d is in use by some policies\n", pool_id);
		ret = -EBUSY;
		goto unlock;
	}

	/* Disable pool pop */
	pool_pop_enable = false;
	ret = __bmgr_pool_pop_enable(pool_id, pool_pop_enable);
	if (unlikely(ret)) {
		pr_err("bmgr_pool_pop_enable %d failed\n", pool_id);
		goto unlock;
	}

	if (bm_ctrl_poll()) {
		pr_err("bm_ctrl_poll failed\n");
		ret = -EBUSY;
	}

unlock:
	__bmgr_db_unlock();

	return ret;
}
EXPORT_SYMBOL(pp_bmgr_pool_pop_disable);

/**
 * @brief Removes a pool
 * @param pool_id pool id to remove
 * @param lock remove under db lock or not
 * @param validate validate all resources are back in the pool
 * @return s32 0 on success, error code otherwise
 */
static s32 __bmgr_pool_remove(u8 pool_id, bool lock, bool validate)
{
	struct pp_bmgr_pool_stats pool_stats;
	struct bmgr_pool_db_entry *pool;
	struct bmgr_pool_cfg cfg;
	bool pool_enable;
	bool pool_pop_enabled;
	s32  ret = 0;

	pr_debug("Removing pool %d\n", pool_id);

	/* Verify pool if is valid */
	if (unlikely(!bmgr_is_pool_id_valid(pool_id)))
		return -EINVAL;

	if (lock)
		__bmgr_db_lock();

	/* Verify pool is active */
	if (unlikely(!is_pool_busy(pool_id))) {
		pr_err("Pool %d id not active\n", pool_id);
		ret = -EINVAL;
		goto unlock;
	}

	/* Verify pool pop is disabled */
	ret = bmgr_pool_pop_status_get(pool_id, &pool_pop_enabled);
	if (unlikely(ret)) {
		pr_err("bmgr_pool_pop_status_get %d failed\n", pool_id);
		goto unlock;
	}

	if (pool_pop_enabled) {
		pr_err("Pool %d pop is not disabled\n", pool_id);
		ret = -EBUSY;
		goto unlock;
	}

	/* Verify pool gained back its resources */
	if (validate) {
		ret = pp_bmgr_pool_stats_get(&pool_stats, pool_id);
		if (unlikely(ret)) {
			pr_err("Pool %d get stats error\n", pool_id);
			ret = -EINVAL;
			goto unlock;
		} else if (pool_stats.pool_allocated_ctr != 0) {
			pr_err("Pool %d resources are still in use\n", pool_id);
			ret = -EBUSY;
			goto unlock;
		}
	}

	/* Disable pool */
	pool_enable = false;
	ret = __bmgr_pool_enable(pool_id, pool_enable);
	if (unlikely(ret)) {
		pr_err("__bmgr_pool_enable %d failed\n", pool_id);
		goto unlock;
	}

	pool = &db->pools[pool_id];

	/* free pointers_table */
	pp_dma_free(pool->internal_ptrs_sz,
		    pool->internal_ptrs_virt,
		    &pool->internal_ptrs_phys, true);

	/* free the pool memory */
	if (pool->virt) {
		pr_info("free pool buffers @ %#llx\n", (u64)pool->phys);
		pp_dma_free(pool->sz, pool->virt, &pool->phys, true);
		pool->virt = 0;
	}

	/* Reset pool config */
	cfg.min_guaranteed_enable = false;
	cfg.num_buffers = 0;
	cfg.ptr_table_addr_high = 0;
	cfg.ptr_table_addr_low = 0;
	ret = bm_pool_configure(pool_id, &cfg);
	if (unlikely(ret)) {
		pr_err("bm_pool_configure Failed\n");
		goto unlock;
	}

	/* Update pool's DB */
	memset(&db->pools[pool_id], 0,
	       sizeof(struct bmgr_pool_db_entry));
	db->pools[pool_id].pool_id = pool_id;
	free_pool(pool_id);

	if (bm_ctrl_poll()) {
		pr_err("bm_ctrl_poll failed\n");
		ret = -EBUSY;
	}

unlock:
	if (lock)
		__bmgr_db_unlock();

	pr_debug("done (%d)\n", ret);
	return ret;
}

s32 pp_bmgr_pool_remove(u8 pool_id)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	return __bmgr_pool_remove(pool_id, true, true);
}
EXPORT_SYMBOL(pp_bmgr_pool_remove);

static s32 free_policy(u32 policy)
{
	clear_bit(policy, db->policies_bmap);
	db->num_policies--;

	return 0;
}

static bool is_policy_busy(u32 policy)
{
	return test_bit(policy, db->policies_bmap);
}

static s32 alloc_policy(u16 *policy)
{
	if (db->num_policies == db->cfg.max_policies) {
		pr_err("No free policy available\n");
		return -ENOENT;
	}

	if (*policy == PP_BM_INVALID_POLICY_ID)
		*policy = find_first_zero_bit(db->policies_bmap,
					      PP_BM_MAX_POLICIES);
	if (!bmgr_is_policy_id_valid(*policy))
		return -EINVAL;
	if (is_policy_busy(*policy)) {
		pr_err("policy %d id active\n", *policy);
		return -EBUSY;
	}

	set_bit(*policy, db->policies_bmap);
	db->num_policies++;

	return 0;
}

static s32 bmgr_policy_configure(const struct pp_bmgr_policy_params *prms,
				 u16 *policy_id, bool lock)
{
	struct bmgr_pool_db_entry *pool;
	struct bmgr_policy_cfg cfg;
	u8  group_id = PP_BM_INVALID_GROUP_ID;
	u16 first_id = PP_BM_INVALID_POLICY_ID;
	bool is_isolated = false;
	s32 ret = 0;

	if (lock)
		__bmgr_db_lock();

	/* Validity check */
	ret = bmgr_is_policy_params_valid(prms, &is_isolated);
	if (unlikely(ret))
		goto unlock;

	ret = alloc_policy(policy_id);
	if (unlikely(ret))
		goto unlock;

	pr_debug("Configuring policy %d, group_id %u\n", *policy_id, group_id);

	db->policies[*policy_id].is_isolated = is_isolated;
	/* Groups logic:
	 *   policies which use same set of pools will be associated to a group
	 *   1st policy will not be associated to any group.
	 *   2nd policy will create a group and both policies will be associated
	 *   3rd policy and above will be associated to the group
	 */
	pool = &db->pools[prms->pools_in_policy[0].pool_id];
	if (pool->policy_ref_cnt == 1) {
		/* we add the second policy, create a group,
		 * add the 1st policy and enable the pool's minimum guarantee
		 */
		ret = alloc_group(&group_id);
		if (unlikely(ret))
			goto free_policy;

		ret = bm_pool_min_guarantee_set(pool->pool_id, true);
		if (unlikely(ret))
			goto free_policy;

		/* get the first policy id */
		first_id =
			find_first_bit(pool->policies_bmap, PP_BM_MAX_POLICIES);

		/* pause the policy before adding it to the group */
		ret = policy_pause_validate(first_id);
		if (unlikely(ret))
			goto free_policy;

		/* add the first policy and all the pools to the group */
		ret = add_policy_to_group(first_id, group_id);
		if (unlikely(ret)) {
			pr_err("Fail to add policy %u to group %u\n",
			       first_id, group_id);
			goto free_policy;
		}
		cfg.group_id = group_id;
		cfg.max_allowed = db->policies[first_id].params.max_allowed;
		cfg.min_guaranteed =
			db->policies[first_id].params.min_guaranteed;
		ret = bm_policy_configure(first_id, &cfg);
		if (unlikely(ret)) {
			pr_err("bm_policy(%d)_configure failed\n", first_id);
			goto free_policy;
		}
	} else if (pool->policy_ref_cnt > 1) {
		/* third policy and above, take the group id from
		 * one of the existing policies
		 */
		first_id =
			find_first_bit(pool->policies_bmap, PP_BM_MAX_POLICIES);
		group_id = db->policies[first_id].group_id;
	}

	/* Pause group before policy config */
	ret = group_pause_validate(group_id);
	if (unlikely(ret)) {
		pr_err("Group %d pause failed\n", group_id);
		goto group_resume;
	}

	/* Update Policy DB */
	memcpy(&db->policies[*policy_id].params, prms,
	       sizeof(*prms));

	/* Add policy pools and update group */
	ret = add_pools_to_policy(*policy_id);
	if (unlikely(ret)) {
		pr_err("Fail to add pools to policy %u\n", *policy_id);
		goto group_resume;
	}

	/* add only the policy to the group, the pools were already added */
	ret = add_policy_to_group(*policy_id, group_id);
	if (unlikely(ret)) {
		pr_err("Fail to add policy %u to group %u\n", *policy_id,
		       group_id);
		goto group_resume;
	}

	cfg.group_id = group_id;
	cfg.max_allowed = prms->max_allowed;
	cfg.min_guaranteed = prms->min_guaranteed;
	ret = bm_policy_configure(*policy_id, &cfg);
	if (unlikely(ret)) {
		pr_err("bm_policy(%d)_configure failed\n", *policy_id);
		goto group_resume;
	}

	if (bm_ctrl_poll()) {
		pr_err("bm_ctrl_poll failed\n");
		ret = -EBUSY;
	}

group_resume:
	/* Resume group */
	group_resume(group_id);

free_policy:
	if (ret)
		free_policy(*policy_id);
unlock:
	if (lock)
		__bmgr_db_unlock();

	pr_debug("done (%d)\n", ret);
	return ret;
}

s32 pp_bmgr_policy_configure(const struct pp_bmgr_policy_params *plcy_params,
			     u16 *policy_id)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	return bmgr_policy_configure(plcy_params, policy_id, true);
}
EXPORT_SYMBOL(pp_bmgr_policy_configure);

static s32 bmgr_policy_remove(u16 policy_id, bool lock)
{
	struct pp_bmgr_policy_params *prms;
	struct bmgr_pool_db_entry *pool;
	struct bmgr_policy_cfg cfg;
	s32 ret = 0;
	u16 last_id;
	u8 group_id;

	pr_debug("Removing policy %d\n", policy_id);

	/* Verify policy id is valid */
	if (unlikely(!bmgr_is_policy_id_valid(policy_id)))
		return -EINVAL;

	if (lock)
		__bmgr_db_lock();

	/* Verify policy is active in db */
	if (unlikely(!is_policy_busy(policy_id))) {
		pr_err("policy %d is not active\n", policy_id);
		ret = -EINVAL;
		goto unlock;
	}

	group_id = db->policies[policy_id].group_id;

	/* Pause group before policy config */
	ret = group_pause_validate(group_id);
	if (unlikely(ret))
		goto group_resume;

	/* Fetch policy params from db */
	prms = &db->policies[policy_id].params;

	ret = remove_policy_from_group(policy_id, group_id);
	if (unlikely(ret))
		goto group_resume;

	ret = remove_pools_from_policy(policy_id);
	if (unlikely(ret))
		goto group_resume;

	/* Reset policy hw registers */
	ret = bm_policy_configure(policy_id, NULL);
	if (unlikely(ret)) {
		pr_err("bm_policy_configure %d failed\n", policy_id);
		goto group_resume;
	}

	/* if there is only 1 policy left who uses the pools,
	 * remove the last policy from the group and delete the group
	 */
	pool = &db->pools[prms->pools_in_policy[0].pool_id];
	if (pool->policy_ref_cnt == 1) {
		/* get the last policy that uses that set of pools */
		last_id =
			find_first_bit(pool->policies_bmap, PP_BM_MAX_POLICIES);
		/* add the first policy and all the pools to the group */
		ret = remove_policy_from_group(last_id, group_id);
		if (unlikely(ret))
			goto group_resume;

		free_group(group_id);
		group_id = PP_BM_INVALID_GROUP_ID;
		cfg.group_id = db->policies[last_id].group_id;
		cfg.max_allowed = db->policies[last_id].params.max_allowed;
		cfg.min_guaranteed =
			db->policies[last_id].params.min_guaranteed;
		ret = bm_policy_configure(last_id, &cfg);
		if (unlikely(ret)) {
			pr_err("bm_policy(%d)_configure failed\n", last_id);
			goto group_resume;
		}

		ret = bm_pool_min_guarantee_set(pool->pool_id, false);
		if (unlikely(ret))
			goto group_resume;

		/* Resume all the pools in the policy, the group doesn't
		 * exist any more so we need resume the policy
		 */
		ret = policy_resume(last_id);
		if (unlikely(ret))
			pr_err("policy %d resume failed\n", last_id);
	}

	/* Update Policy DB */
	free_policy(policy_id);
	memset(&db->policies[policy_id], 0,
	       sizeof(struct bmgr_policy_db_entry));

	if (bm_ctrl_poll()) {
		pr_err("bm_ctrl_poll failed\n");
		ret = -EBUSY;
		goto unlock;
	}

group_resume:
	ret = group_resume(group_id);
	if (unlikely(ret))
		pr_err("Group %d resume failed\n", group_id);

unlock:
	if (lock)
		__bmgr_db_unlock();

	pr_debug("done (%d)\n", ret);
	return ret;
}

s32 pp_bmgr_policy_remove(u16 policy_id)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	return bmgr_policy_remove(policy_id, true);
}
EXPORT_SYMBOL(pp_bmgr_policy_remove);

static void __bmgr_work_handler(struct work_struct *w)
{
	struct bmgr_policy_db_entry *policy;
	struct policy_reset_action *act;
	u32 pool_id, i, buff_idx, num_buffers;
	u64 buff;
	s32 ret;

	/* Extract action structure */
	act = container_of(w, struct policy_reset_action, work.work);
	policy = &db->policies[act->policy_id];

	pr_debug("Reset policy %u WQ starts\n", act->policy_id);
	__bmgr_db_lock();

	/* pop all buffers from all the pools */
	do {
		ret = bm_pop_buffer(act->policy_id, NULL, &buff);
		if (ret) {
			pr_err("failed to pop buffer from policy %u, ret %d\n",
			       act->policy_id, ret);
			goto unlock;
		}
		if (buff)
			pr_debug("pop buffer %#llx\n", buff);
	} while (buff);

	/* push all buffers from all pools back into the policy */
	for (i = 0; i < policy->params.num_pools_in_policy; i++) {
		pool_id = policy->params.pools_in_policy[i].pool_id;
		num_buffers = db->pools[pool_id].params.num_buffers;

		for (buff_idx = 0; buff_idx < num_buffers; buff_idx++) {
			buff = buff_index_to_ptr(pool_id, buff_idx);
			ret = bm_push_buffer(act->policy_id, pool_id, buff);
			if (!ret) {
				pr_debug("push buffer %#llx\n", buff);
				continue;
			}

			pr_err("failed to push buffer %#llx to policy %u pool %u, ret %d\n",
			       buff, act->policy_id, pool_id, ret);
		}
	}

unlock:
	__bmgr_db_unlock();
	if (act->cb)
		(*act->cb)(ret);

	kfree(act);
	pr_debug("done (%d)\n", ret);
}

s32 pp_bmgr_policy_reset(u16 policy_id, void (*cb)(s32 ret))
{
	struct policy_reset_action *action;
	s32 ret = 0;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	/* Verify policy id is valid */
	if (unlikely(!bmgr_is_policy_id_valid(policy_id)))
		return -EINVAL;

	pr_debug("Resetting policy %d\n", policy_id);

	__bmgr_db_lock();

	/* Verify policy is active in db */
	if (unlikely(!is_policy_busy(policy_id))) {
		pr_err("policy %d is not active\n", policy_id);
		ret = -EINVAL;
		goto unlock;
	}

	/* Verify policy is isolated */
	if (unlikely(db->policies[policy_id].is_isolated == 0)) {
		pr_err("policy %d is not isolated\n", policy_id);
		ret = -EINVAL;
		goto unlock;
	}

	action = kzalloc(sizeof(*action), GFP_ATOMIC);
	if (!action) {
		ret = -ENOMEM;
		goto unlock;
	}

	__bmgr_db_unlock();

	/* Trigger delayed work queue */
	INIT_DELAYED_WORK(&action->work, __bmgr_work_handler);
	action->policy_id = policy_id;
	if (cb)
		action->cb = cb;

	queue_delayed_work(bmgr_wq,
			   &action->work,
			   msecs_to_jiffies(BM_POLICY_RESET_DELAY_MS));

	pr_debug("done (%d)\n", ret);
	return ret;

unlock:
	__bmgr_db_unlock();
	pr_debug("done (%d)\n", ret);
	return ret;
}
EXPORT_SYMBOL(pp_bmgr_policy_reset);

/**
 * @brief Get buffer manager configuration
 * @param regs returned config structure
 * @return 0 on success, other error code on failure
 */
s32 bmgr_get_cfg_regs(struct bmgr_cfg_regs * const regs)
{
	if (unlikely(!regs)) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	bm_get_cfg_regs(regs);

	return 0;
}

s32 pp_bmgr_pool_conf_get(u8 pool_id,
			  struct pp_bmgr_pool_params * const pool_params)
{
	s32 ret = 0;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (unlikely(!pool_params)) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Validity check */
	if (unlikely(!bmgr_is_pool_id_valid(pool_id)))
		return -EINVAL;

	__bmgr_db_lock();

	/* Active check */
	if (unlikely(!is_pool_busy(pool_id))) {
		pr_debug("pool_id %d is not active\n", pool_id);
		ret = -EINVAL;
		goto unlock;
	}

	memcpy(pool_params,
	       &db->pools[pool_id].params,
	       sizeof(struct pp_bmgr_pool_params));

unlock:
	__bmgr_db_unlock();
	return ret;
}
EXPORT_SYMBOL(pp_bmgr_pool_conf_get);

s32 pp_bmgr_pool_stats_get(struct pp_bmgr_pool_stats * const stats, u8 pool_id)
{
	s32 ret = 0;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (unlikely(!stats)) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Validity check */
	if (unlikely(!bmgr_is_pool_id_valid(pool_id)))
		return -EINVAL;

	/* Active check */
	if (unlikely(!is_pool_busy(pool_id))) {
		pr_err("pool_id %d is not active\n", pool_id);
		return -EINVAL;
	}

	ret = bm_pool_stats_get(stats, pool_id);
	if (unlikely(ret))
		pr_err("bm_pool_stats_get %d failed\n", pool_id);

	return ret;
}
EXPORT_SYMBOL(pp_bmgr_pool_stats_get);

s32 pp_bmgr_policy_conf_get(u16 policy_id,
			    struct pp_bmgr_policy_params * const policy_params)
{
	s32 ret = 0;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (unlikely(!policy_params)) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Validity check */
	if (unlikely(!bmgr_is_policy_id_valid(policy_id)))
		return -EINVAL;

	__bmgr_db_lock();

	/* Active check */
	if (unlikely(!is_policy_busy(policy_id))) {
		pr_debug("policy_id %d is not active\n", policy_id);
		ret = -EINVAL;
		goto unlock;
	}

	memcpy(policy_params, &db->policies[policy_id].params,
	       sizeof(*policy_params));

unlock:
	__bmgr_db_unlock();
	return ret;
}
EXPORT_SYMBOL(pp_bmgr_policy_conf_get);

s32 pp_bmgr_policy_stats_get(struct pp_bmgr_policy_stats * const stats,
			     u16 policy_id)
{
	struct pp_bmgr_policy_params *param;
	s32 ret = 0;
	u8 i;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (unlikely(!stats)) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Validity check */
	if (unlikely(!bmgr_is_policy_id_valid(policy_id)))
		return -EINVAL;

	/* Active check */
	if (unlikely(!is_policy_busy(policy_id))) {
		pr_err("policy_id %d is not active\n", policy_id);
		return -EINVAL;
	}

	memset(stats, 0, sizeof(*stats));
	ret = bm_policy_stats_get(stats, policy_id);
	if (unlikely(ret))
		pr_err("bm_policy_stats_get %d failed\n", policy_id);

	/* read configuration from DB only */
	param = &db->policies[policy_id].params;
	stats->policy_max_allowed = param->max_allowed;
	stats->policy_min_guaranteed = param->min_guaranteed;
	stats->policy_grp_association = db->policies[policy_id].group_id;

	for (i = 0 ; i < param->num_pools_in_policy ; i++) {
		set_bit(param->pools_in_policy[i].pool_id,
			stats->pools_bmap);
		stats->policy_max_allowed_per_pool[i] =
			param->pools_in_policy[i].max_allowed;
	}

	if (db->policies[policy_id].group_id == PP_BM_INVALID_GROUP_ID)
		stats->policy_grp_association = PP_BM_INVALID_GROUP_ID;

	return ret;
}
EXPORT_SYMBOL(pp_bmgr_policy_stats_get);

s32 bmgr_group_stats_get(struct bmgr_group_stats * const stats, u8 group_id)
{
	s32 ret = 0;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (unlikely(!stats)) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	ret = bm_group_stats_get(group_id, stats);
	if (unlikely(ret))
		pr_err("bm_group_stats_get %d failed\n", group_id);

	return ret;
}

s32 pp_bmgr_stats_get(struct bmgr_stats *stats)
{
	if (unlikely(ptr_is_null(stats)))
		return -EINVAL;

	stats->active_pools    = db->num_pools;
	stats->active_policies = db->num_policies;

	return 0;
}

s32 pp_bmgr_group_db_info_get(u8 grp_id, struct bmgr_group_db_entry *info)
{
	if (ptr_is_null(info))
		return -EINVAL;

	if (!bmgr_is_group_id_valid(grp_id)) {
		pr_err("Invalid group id %u\n", grp_id);
		return -EINVAL;
	}

	if (!is_group_busy(grp_id))
		return -EINVAL;

	memcpy(info, &db->groups[grp_id], sizeof(*info));

	return 0;
}

void bmgr_pools_per_policy_set(u32 pools_per_policy)
{
	if (unlikely(!__bmgr_is_ready()))
		return;

	__bmgr_db_lock();
	db->cfg.max_pools_in_policy = pools_per_policy;
	__bmgr_db_unlock();
}

s32 bmgr_get_active_policies(ulong *bmap, size_t size)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (size < PP_BM_MAX_POLICIES)
		return -ENOSPC;

	__bmgr_db_lock();
	memcpy(bmap, db->policies_bmap, sizeof(db->policies_bmap));
	__bmgr_db_unlock();

	return 0;
}

s32 bmgr_get_active_pools(ulong *bmap, size_t size)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (size < PP_BM_MAX_POOLS)
		return -ENOSPC;

	__bmgr_db_lock();
	memcpy(bmap, db->pools_bmap, sizeof(db->pools_bmap));
	__bmgr_db_unlock();

	return 0;
}

s32 bmgr_config_set(const struct pp_bmgr_init_param * const cfg)
{
	s32 ret = 0;
	u32 policy_id;
	u8  pool_id;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (unlikely(!cfg)) {
		pr_err("Null cfg\n");
		return -EINVAL;
	}

	pr_debug("Ext-config: pools(%d), groups(%d), policies(%d)\n",
		 cfg->max_pools, cfg->max_groups, cfg->max_policies);


	if (unlikely(cfg->max_pools > PP_BM_MAX_POOLS)) {
		pr_err("Max pools %d exceeds maximum of %d\n",
		       cfg->max_pools,
		       PP_BM_MAX_POOLS);
		ret = -EINVAL;
		goto reset_cfg_db;
	}

	if (unlikely(cfg->max_groups > BM_MAX_GROUPS)) {
		pr_err("Max groups %d exceeds maximum of %d\n",
		       cfg->max_groups,
		       BM_MAX_GROUPS);
		ret = -EINVAL;
		goto reset_cfg_db;
	}

	if (unlikely(cfg->max_policies > PP_BM_MAX_POLICIES)) {
		pr_err("Max groups %d exceeds maximum of %d\n",
		       cfg->max_policies,
		       PP_BM_MAX_POLICIES);
		ret = -EINVAL;
		goto reset_cfg_db;
	}

	__bmgr_db_lock();

	memcpy(&db->cfg, cfg, sizeof(*cfg));
	/* Init RAM */
	BM_FOR_EACH_POLICY(db, policy_id) {
		ret = bm_policy_default_set(policy_id);
		if (unlikely(ret)) {
			pr_err("bm_policy_default_set %d failed\n", policy_id);
			goto unlock;
		}
	}

	BM_FOR_EACH_POOL(db, pool_id)
		db->pools[pool_id].pool_id = pool_id;

unlock:
	__bmgr_db_unlock();

	return ret;

reset_cfg_db:
	memset(&db->cfg, 0, sizeof(struct pp_bmgr_init_param));
	return ret;
}

s32 pp_bmgr_config_get(struct pp_bmgr_init_param * const cfg)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (unlikely(!cfg)) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Store configuration in db */
	cfg->max_pools = db->cfg.max_pools;
	cfg->max_groups = db->cfg.max_groups;
	cfg->max_policies = db->cfg.max_policies;
	cfg->pool_pop_hw_en = db->cfg.pool_pop_hw_en;
	cfg->max_pools_in_policy = db->cfg.max_pools_in_policy;

	return 0;
}
EXPORT_SYMBOL(pp_bmgr_config_get);

bool pp_bmgr_is_policy_active(u16 policy)
{
	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	/* Verify policy id is valid */
	if (unlikely(!bmgr_is_policy_id_valid(policy)))
		return false;

	/* Check if policy is active in db */
	return is_policy_busy(policy);
}
EXPORT_SYMBOL(pp_bmgr_is_policy_active);

s32 pp_bmgr_set_total_active_pools(u32 num_active_pools)
{
	s32 ret = 0;

	if (unlikely(!__bmgr_is_ready()))
		return -EPERM;

	if (num_active_pools > db->cfg.max_pools) {
		pr_err("Number active pools %d is not valid\n",
		       num_active_pools);
		return -EINVAL;
	}

	__bmgr_db_lock();
	ret = bm_set_total_active_pools(num_active_pools);
	if (unlikely(ret))
		goto unlock;

unlock:
	__bmgr_db_unlock();

	return ret;
}
EXPORT_SYMBOL(pp_bmgr_set_total_active_pools);

s32 pp_bmgr_push_buffer(u32 policy, u32 pool, u64 buff)
{
	s32 ret;

	ret = bm_push_buffer(policy, pool, buff);
	if (ret) {
		atomic_inc(&db->stats.num_fail_push_req);
		return ret;
	}

	atomic_inc(&db->stats.num_succ_push_req);
	return 0;
}
EXPORT_SYMBOL(pp_bmgr_push_buffer);

s32 pp_bmgr_pop_buffer(u32 policy, u32 *pool, u64 *buff)
{
	s32 ret;

	if (ptr_is_null(buff)) {
		pr_err("buff pointer is NULL. Unable to pop buffer.\n");
		return -EINVAL;
	}

	if (ptr_is_null(pool)) {
		pr_err("pool pointer is NULL. Unable to pop buffer.\n");
		return -EINVAL;
	}

	ret = bm_pop_buffer(policy, pool, buff);
	if ((ret) || (*buff == 0)) {
		atomic_inc(&db->stats.num_fail_pop_req);
		return -EINVAL;
	}

	atomic_inc(&db->stats.num_succ_pop_req);
	return 0;
}
EXPORT_SYMBOL(pp_bmgr_pop_buffer);

u32 bmgr_get_succ_pop_req(void)
{
	return atomic_read(&db->stats.num_succ_pop_req);
}
EXPORT_SYMBOL(bmgr_get_succ_pop_req);

u32 bmgr_get_fail_pop_req(void)
{
	return atomic_read(&db->stats.num_fail_pop_req);
}
EXPORT_SYMBOL(bmgr_get_fail_pop_req);

u32 bmgr_get_succ_push_req(void)
{
	return atomic_read(&db->stats.num_succ_push_req);
}
EXPORT_SYMBOL(bmgr_get_succ_push_req);

u32 bmgr_get_fail_push_req(void)
{
	return atomic_read(&db->stats.num_fail_push_req);
}
EXPORT_SYMBOL(bmgr_get_fail_push_req);

s32 pp_bmgr_init(const struct pp_bmgr_init_param *init_param)
{
	if (!init_param->valid)
		return -EINVAL;

	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db))
		return -ENOMEM;

	spin_lock_init(&db->db_lock);

	if (unlikely(bm_dbg_init(db, init_param->dbgfs))) {
		pr_err("Failed to init debug stuff\n");
		return -ENOMEM;
	}

	bmgr_wq = alloc_workqueue("pp-buffer-mgr", 0, 1);
	qos_uc_base_addr = init_param->qos_uc_base;
	db->ready = true;

	if (unlikely(bmgr_config_set(init_param))) {
		pr_err("Failed to set bmgr config\n");
		db->ready = false;
		return -EINVAL;
	}

	atomic_set(&db->stats.num_fail_pop_req, 0);
	atomic_set(&db->stats.num_succ_pop_req, 0);
	atomic_set(&db->stats.num_fail_pop_req, 0);
	atomic_set(&db->stats.num_succ_pop_req, 0);
	return 0;
}
EXPORT_SYMBOL(pp_bmgr_init);

void pp_bmgr_exit(void)
{
	pr_debug("start\n");
	if (unlikely(!db))
		return;

	db->ready = false;
	if (bmgr_wq)
		destroy_workqueue(bmgr_wq);

	kfree(db);
	db = NULL;
	bm_dbg_clean();

	pr_debug("done\n");
}
EXPORT_SYMBOL(pp_bmgr_exit);
