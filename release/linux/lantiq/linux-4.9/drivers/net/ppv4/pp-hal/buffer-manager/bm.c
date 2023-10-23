/*
 * bm.c
 * Description: PP Buffer Manager Hal Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

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
#include <linux/dma-mapping.h>
#include <linux/bitfield.h>

#include "pp_common.h"
#include "bm.h"
#include "bm_internal.h"
#include "pp_qos_utils.h"

/* Each cache row contains 4 pointers which are 16 bytes */
#define BM_BURST_BYTES_TO_CACHE_ROWS(sz, num) (((sz) * (num)) >> 4)

struct bmgr_db *db;
u64 bm_base_addr;
u64 bm_ram_base_addr;

s32 __bm_pool_burst_size_set(u8 pool_id, u8 burst_size)
{
	u32 reg_val;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id,
		       db->max_pools);
		return -EINVAL;
	}

	if (burst_size > BM_OCPM_BURST_SIZE_512B) {
		pr_err("Burst size %d exceeds maximum of %d\n",
		       burst_size,
		       BM_OCPM_BURST_SIZE_512B);
		return -EINVAL;
	}

	/* Read value */
	reg_val = PP_REG_RD32(PP_BM_OCPM_BURST_SIZE_REG);
	reg_val = PP_FIELD_MOD(BM_OCPM_BURST_SIZE_FIELD(pool_id),
			       burst_size,
			       reg_val);
	PP_REG_WR32(PP_BM_OCPM_BURST_SIZE_REG, reg_val);

	return 0;
}

s32 __bm_pool_num_bursts_set(u8 pool_id, u8 num_bursts)
{
	u32 reg_val;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	if (num_bursts > BM_OCPM_NUM_OF_BURSTS_4) {
		pr_err("Num bursts %d exceeds maximum of %d\n",
		       num_bursts, BM_OCPM_NUM_OF_BURSTS_4);
		return -EINVAL;
	}

	/* Read value */
	reg_val = PP_REG_RD32(PP_BM_OCPM_NUM_OF_BURSTS_REG);
	reg_val = PP_FIELD_MOD(BM_OCPM_NUM_BURSTS_FIELD(pool_id),
			       num_bursts,
			       reg_val);
	PP_REG_WR32(PP_BM_OCPM_NUM_OF_BURSTS_REG, reg_val);

	return 0;
}

s32 bm_init(const struct pp_bm_init_param * const cfg)
{
	u32 idx, reg_val = 0;
	s32 ret = 0;
	u32 burst_sz, burst_sz_val, burst_num, burst_num_val;

	if (!cfg->valid)
		return -EINVAL;

	bm_base_addr = cfg->bm_base;
	bm_ram_base_addr = cfg->bm_ram_base;

	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (!db)
		return -ENOMEM;

	/* Get config */
	db->bm_pop_push_addr = cfg->bm_pop_push_addr;
	db->max_pools = cfg->max_pools;
	db->max_groups = cfg->max_groups;
	db->max_policies = cfg->max_policies;
	db->total_configured_pcu_fifo_sz = cfg->pcu_fifo_sz;
	db->total_free_pcu_fifo_sz = 0;
	db->pcu_pool_fifo_sz = 0;
	db->pcu_fifo_sz_alignment = 0;

	/* Clear all pool enable bits */
	PP_REG_WR32(PP_BM_PCU_POOL_EN_REG, 0);

	/* Set control enable */
	reg_val |= FIELD_PREP(PP_BM_BMNGR_CTRL_BMNG_CLNT_EN_MSK, 1);
	reg_val |= FIELD_PREP(PP_BM_BMNGR_CTRL_POP_DIS_MSK, 0);
	reg_val |= FIELD_PREP(PP_BM_BMNGR_CTRL_PUSH_DIS_MSK, 0);
	reg_val |= FIELD_PREP(PP_BM_BMNGR_CTRL_PM_ARB_DIS_MSK, 0);
	PP_REG_WR32(PP_BM_BMNGR_CTRL_REG, reg_val);

	burst_sz = BM_OCPM_BURST_SIZE_256B;
	burst_num = BM_OCPM_NUM_OF_BURSTS_2;
	for (idx = 0; idx < db->max_pools; idx++) {
		ret = __bm_pool_burst_size_set(idx, burst_sz);
		if (ret)
			goto bm_init_done;

		ret = __bm_pool_num_bursts_set(idx, burst_num);
		if (ret)
			goto bm_init_done;
	}

	burst_sz_val = BM_OCPM_BURST_SIZE_VAL(burst_sz);
	burst_num_val = BM_OCPM_NUM_OF_BURSTS_VAL(burst_num);

	/* Calculate number of cache rows alignment requirement */
	db->pcu_fifo_sz_alignment =
		BM_BURST_BYTES_TO_CACHE_ROWS(burst_sz_val, burst_num_val);

	/* Reset group reserved buffers */
	for (idx = 0; idx < db->max_groups; idx++)
		PP_REG_WR32(PP_BM_GRP_RES_REG_IDX(idx), 0);

	pr_debug("pp bm was initialized\n");

bm_init_done:
	return ret;
}

void bm_exit(void)
{
	kfree(db);
	db = NULL;
}

s32 bm_set_total_active_pools(u32 num_active_pools)
{
	u32 fifo_sz_not_aligned;
	u32 fifo_sz_quarter;

	if (db->pcu_pool_fifo_sz) {
		pr_err("Number of active pools is already configured\n");
		return -EINVAL;
	}

	if (!db->pcu_fifo_sz_alignment) {
		pr_err("Fifo size alignment must not be 0\n");
		return -EINVAL;
	}

	/* Distribute total available cache rows equally among active pools */
	fifo_sz_not_aligned =
		db->total_configured_pcu_fifo_sz / num_active_pools;

	/* Fifo sz quarter must be aligned with pcu_pool_fifo_sz_alignment */
	fifo_sz_quarter =
		rounddown(fifo_sz_not_aligned >> 2, db->pcu_fifo_sz_alignment);

	/* Obtain the aligned fifo sz */
	db->pcu_pool_fifo_sz = fifo_sz_quarter * 4;
	db->total_free_pcu_fifo_sz = db->pcu_pool_fifo_sz * num_active_pools;

	pr_debug("Pool fifo size %u (utilization %u%%)\n", db->pcu_pool_fifo_sz,
		 db->total_free_pcu_fifo_sz * 100 /
		 db->total_configured_pcu_fifo_sz);

	return 0;
}

s32 bm_policy_default_set(u16 policy_idx)
{
	u8 pool_idx;
	unsigned long addr;

	if (policy_idx >= db->max_policies) {
		pr_err("policy_idx %d exceeds maximum of %d\n",
		       policy_idx, db->max_policies);
		return -EINVAL;
	}

	PP_REG_WR32(BM_PLCY_MAX_ALLOWED(policy_idx), 0);
	PP_REG_WR32(BM_PLCY_MIN_GUARANTEED(policy_idx), 0);
	PP_REG_WR32(BM_PLCY_GRP_ASSOCIATED(policy_idx), 0);
	PP_REG_WR32(BM_PLCY_POOLS_MAPPING(policy_idx), 0);
	PP_REG_WR32(BM_PLCY_ALLOC_BUFF_CTR(policy_idx), 0);

	/* Iterate pool per policy */
	for (pool_idx = 0; pool_idx < PP_BM_MAX_POOLS_PER_PLCY; pool_idx++) {
		PP_REG_WR32(BM_PLCY_MAX_ALLOW_PER_POOL(policy_idx, pool_idx),
			    0);

		addr = BM_PLCY_ALOC_BUF_PR_POL_CTR(pool_idx, policy_idx);
		PP_REG_WR32(addr, 0);
	}

	return 0;
}

s32 bm_group_stats_get(u8 group_id, struct bmgr_group_stats * const stats)
{
	u32 reg_val;

	if (group_id >= db->max_groups) {
		pr_err("group_id %d exceeds maximum of %d\n",
		       group_id, db->max_groups);
		return -EINVAL;
	}

	reg_val = PP_REG_RD32(PP_BM_GRP_RES_REG_IDX(group_id));
	stats->grp_rsrvd_buff = FIELD_GET(PP_BM_GRP_RES_GRP_RESERVED_MSK,
					  reg_val);

	reg_val = PP_REG_RD32(PP_BM_GRP_AVL_REG_IDX(group_id));
	stats->grp_avail_buff =
		FIELD_GET(PP_BM_GRP_AVL_GRP_AVAILABLE_MSK, reg_val);

	return 0;
}

s32 bm_policy_stats_get(struct pp_bmgr_policy_stats * const stats,
			u16 policy_id)
{
	u8 pool_idx;
	u32 reg_val;

	if (policy_id >= db->max_policies) {
		pr_err("policy_idx %d exceeds maximum of %d\n",
		       policy_id, db->max_policies);
		return -EINVAL;
	}

	reg_val = PP_REG_RD32(PP_BM_NULL_CNT_REG_IDX(policy_id));
	stats->policy_null_ctr = FIELD_GET(PP_BM_NULL_CNT_MSK, reg_val);

	stats->policy_alloc_buff =
		PP_REG_RD32(BM_PLCY_ALLOC_BUFF_CTR(policy_id));

	for (pool_idx = 0 ; pool_idx < PP_BM_MAX_POOLS_PER_PLCY ; pool_idx++) {
		stats->policy_alloc_buff_per_pool[pool_idx] =
			PP_REG_RD32(BM_PLCY_ALOC_BUF_PR_POL_CTR(pool_idx,
								policy_id));
	}

	return 0;
}

s32 bm_pool_stats_get(struct pp_bmgr_pool_stats * const stats, u8 pool_id)
{
	u32 low, high, reg_val = 0;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	reg_val = PP_REG_RD32(PP_BM_POOL_SIZE_REG_IDX(pool_id));
	stats->pool_size = FIELD_GET(PP_BM_POOL_SIZE_MSK, reg_val);

	reg_val = PP_REG_RD32(PP_BM_PCU_FIFO_BASE_ADDR_REG_IDX(pool_id));
	stats->pcu_fifo_base_addr =
		FIELD_GET(PP_BM_PCU_FIFO_BASE_ADDR_MSK, reg_val);

	reg_val = PP_REG_RD32(PP_BM_PCU_FIFO_SIZE_REG_IDX(pool_id));
	stats->pcu_fifo_size =
		FIELD_GET(PP_BM_PCU_FIFO_SIZE_MSK, reg_val);

	reg_val = PP_REG_RD32(PP_BM_PCU_FIFO_OCC_REG_IDX(pool_id));
	stats->pcu_fifo_occupancy =
		FIELD_GET(PP_BM_PCU_FIFO_OCC_MSK, reg_val);

	reg_val = PP_REG_RD32(PP_BM_PCU_FIFO_PROG_EMPTY_REG_IDX(pool_id));
	stats->pcu_fifo_prog_empty =
		FIELD_GET(PP_BM_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_MSK,
			  reg_val);

	reg_val = PP_REG_RD32(PP_BM_PCU_FIFO_PROG_FULL_REG_IDX(pool_id));
	stats->pcu_fifo_prog_full =
		FIELD_GET(PP_BM_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_MSK,
			  reg_val);

	low = PP_REG_RD32(PP_BM_EXT_FIFO_BASE_ADDR_LOW_REG_IDX(pool_id));
	high = PP_REG_RD32(PP_BM_EXT_FIFO_BASE_ADDR_HIGH_REG_IDX(pool_id));
	stats->ext_fifo_base_addr = PP_BUILD_64BIT_WORD(low, high);

	stats->ext_fifo_occupancy =
		PP_REG_RD32(PP_BM_EXT_FIFO_OCC_REG_IDX(pool_id));

	reg_val = PP_REG_RD32(PP_BM_POOL_CNT_REG_IDX(pool_id));
	stats->pool_allocated_ctr =
		FIELD_GET(PP_BM_POOL_CNT_POOL_ALLOC_CNT_MSK, reg_val);

	reg_val = PP_REG_RD32(PP_BM_POP_CNT_REG_IDX(pool_id));
	stats->pool_pop_ctr = FIELD_GET(PP_BM_POP_CNT_MSK, reg_val);

	reg_val = PP_REG_RD32(PP_BM_PUSH_CNT_REG_IDX(pool_id));
	stats->pool_push_ctr = FIELD_GET(PP_BM_PUSH_CNT_MSK, reg_val);

	stats->pool_ddr_burst_read_ctr =
		PP_REG_RD32(PP_BM_BURST_WR_CNT_REG_IDX(pool_id));

	stats->pool_ddr_burst_write_ctr =
		PP_REG_RD32(PP_BM_BURST_RD_CNT_REG_IDX(pool_id));

	stats->pool_watermark_low_thr =
		PP_REG_RD32(PP_BM_WATERMARK_LOW_THRSH_REG_IDX(pool_id));

	stats->pool_watermark_low_ctr =
		PP_REG_RD32(PP_BM_WATERMARK_LOW_CNT_REG_IDX(pool_id));

	return 0;
}

void bm_get_cfg_regs(struct bmgr_cfg_regs * const regs)
{
	regs->ctrl_reg = PP_REG_RD32(PP_BM_BMNGR_CTRL_REG);
	regs->min_grant_reg = PP_REG_RD32(PP_BM_POOL_MIN_GRNT_MASK_REG);
	regs->pool_enable_reg = PP_REG_RD32(PP_BM_PCU_POOL_EN_REG);
	regs->pool_fifo_reset_reg = PP_REG_RD32(PP_BM_POOL_RESET_REG);
	regs->ocpm_burst_size_reg = PP_REG_RD32(PP_BM_OCPM_BURST_SIZE_REG);
	regs->ocpm_num_bursts_reg = PP_REG_RD32(PP_BM_OCPM_NUM_OF_BURSTS_REG);
	regs->status_reg = PP_REG_RD32(PP_BM_STATUS_REG);
}

s32 bm_group_reserved_set(u8 group_id, u32 reserved)
{
	u32 val = 0;

	if (group_id >= db->max_groups) {
		pr_err("group id %d exceeds maximum of %d\n",
		       group_id, db->max_groups);
		return -EINVAL;
	}

	val = PP_FIELD_MOD(PP_BM_GRP_RES_GRP_RESERVED_MSK, reserved, val);
	PP_REG_WR32(PP_BM_GRP_RES_REG_IDX(group_id), val);

	return 0;
}

s32 bm_group_reserved_get(u8 group_id, u32 *reserved)
{
	u32 reg_val;

	if (ptr_is_null(reserved))
		return -EINVAL;

	if (group_id >= db->max_groups) {
		pr_err("group id %d exceeds maximum of %d\n",
		       group_id, db->max_groups);
		return -EINVAL;
	}

	reg_val = PP_REG_RD32(PP_BM_GRP_RES_REG_IDX(group_id));
	*reserved = FIELD_GET(PP_BM_GRP_RES_GRP_RESERVED_MSK, reg_val);

	return 0;
}

s32 bm_group_available_set(u8 group_id, u32 available)
{
	u32  reg_val = 0;

	if (group_id >= db->max_groups) {
		pr_err("group id %d exceeds maximum of %d\n",
		       group_id, db->max_groups);
		return -EINVAL;
	}

	reg_val = FIELD_PREP(PP_BM_GRP_AVL_GRP_AVAILABLE_MSK, available);
	PP_REG_WR32(PP_BM_GRP_AVL_REG_IDX(group_id), reg_val);

	return 0;
}

s32 bm_group_available_get(u8 group_id, u32 *available)
{
	u32 reg_val;

	if (ptr_is_null(available))
		return -EINVAL;

	if (group_id >= db->max_groups) {
		pr_err("group id %d exceeds maximum of %d\n",
		       group_id, db->max_groups);
		return -EINVAL;
	}

	reg_val = PP_REG_RD32(PP_BM_GRP_AVL_REG_IDX(group_id));
	*available = FIELD_GET(PP_BM_GRP_AVL_GRP_AVAILABLE_MSK, reg_val);

	return 0;
}

s32 bm_pool_pop_enable(u8 pool_id, bool enable)
{
	u32 reg_val;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	/* Read value */
	reg_val = PP_REG_RD32(PP_BM_POOL_POP_EN_REG);
	reg_val = PP_FIELD_MOD(BM_POOL_POP_ENABLE_FIELD(pool_id),
			       enable ? 1 : 0,
			       reg_val);
	PP_REG_WR32(PP_BM_POOL_POP_EN_REG, reg_val);

	return 0;
}

s32 bm_pool_pop_status_get(u8 pool_id, bool *enable)
{
	u32 reg_val;
	u32 mask;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	/* Read value */
	reg_val = PP_REG_RD32(PP_BM_POOL_POP_EN_REG);
	mask = BM_POOL_POP_ENABLE_FIELD(pool_id);

	if (reg_val & mask)
		*enable = true;
	else
		*enable = false;

	return 0;
}

s32 bm_pool_enable(u8 pool_id, bool enable)
{
	u32 reg_val;
	u32 mask;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	reg_val = PP_REG_RD32(PP_BM_PCU_POOL_EN_REG);
	mask = BM_POOL_ENABLE_FIELD(pool_id);

	if (enable) {
		/* Pool Enable */
		reg_val |= mask;
	} else {
		/* Pool disable */
		mask = ~mask;
		reg_val &= mask;
	}
	PP_REG_WR32(PP_BM_PCU_POOL_EN_REG, reg_val);

	return 0;
}

s32 bm_policy_pool_mapping_set(u16 policy_id,
			       u8 pool_id,
			       u8 priority,
			       u32 max_allowed,
			       bool map_pool_to_policy)
{
	u32 reg_val;
	u32 mask;

	if ((pool_id >= db->max_pools) &&
	    (pool_id != PP_BM_INVALID_POOL_ID)) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	if (policy_id >= db->max_policies) {
		pr_err("policy_id %d exceeds maximum of %d\n",
		       policy_id, db->max_policies);
		return -EINVAL;
	}

	if (priority >= BM_POLICY_POOL_PRIORITY_MAX) {
		pr_err("priority %d exceeds maximum of BM_POLICY_POOL_PRIORITY_MAX\n",
		       priority);
		return -EINVAL;
	}

	PP_REG_WR32(BM_PLCY_MAX_ALLOW_PER_POOL(policy_id, priority),
		    max_allowed);

	reg_val = PP_REG_RD32(BM_PLCY_POOLS_MAPPING(policy_id));

	/* Clear relevant byte */
	mask = BM_POOL_PRIORITY_IN_POLICY_MASK(priority);
	reg_val &= ~mask;

	/* Prepare the value to be wriiten */
	if (map_pool_to_policy)
		reg_val |= PP_FIELD_PREP(mask, pool_id);

	PP_REG_WR32(BM_PLCY_POOLS_MAPPING(policy_id), reg_val);

	return 0;
}

s32 bm_policy_configure(u16 policy_id,
			const struct bmgr_policy_cfg *cfg)
{
	struct bmgr_policy_cfg tmp = { 0 };

	if (policy_id >= db->max_policies) {
		pr_err("policy_id %d exceeds maximum of %d\n",
		       policy_id, db->max_policies);
		return -EINVAL;
	}

	if (cfg) {
		tmp.max_allowed = cfg->max_allowed;
		/* there isn't an invalid value for a group in HW
		 * so, for setting invalid group id, we set group id 0
		 * and minimum guarantee to 0
		 */
		if (cfg->group_id != PP_BM_INVALID_GROUP_ID) {
			tmp.group_id = cfg->group_id;
			tmp.min_guaranteed = cfg->min_guaranteed;
		}
	}

	PP_REG_WR32(BM_PLCY_GRP_ASSOCIATED(policy_id), tmp.group_id);
	PP_REG_WR32(BM_PLCY_MIN_GUARANTEED(policy_id), tmp.min_guaranteed);
	PP_REG_WR32(BM_PLCY_MAX_ALLOWED(policy_id), tmp.max_allowed);

	return 0;
}

s32 bm_ctrl_poll(void)
{
	if (pp_reg_poll(PP_BM_STATUS_REG, PP_BM_STATUS_INIT_MSK, 0)) {
		pr_err("pp_reg_poll failed\n");
		return -EBUSY;
	}

	return 0;
}

s32 bm_pool_min_guarantee_set(u8 pool_id, bool en)
{
	u32 reg_val = 0;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	/* Set minimum guaranteed bit per pool */
	reg_val = PP_REG_RD32(PP_BM_POOL_MIN_GRNT_MASK_REG);
	reg_val = PP_FIELD_MOD(BM_POOL_MIN_GRNT_FIELD(pool_id), en, reg_val);
	PP_REG_WR32(PP_BM_POOL_MIN_GRNT_MASK_REG, reg_val);

	return 0;
}

s32 bm_pool_configure(u8 pool_id, const struct bmgr_pool_cfg * const cfg)
{
	u32 reg_val = 0;
	u32 fifo_sz;

	if (pool_id >= db->max_pools) {
		pr_err("pool id %d exceeds maximum of %d\n",
		       pool_id, db->max_pools);
		return -EINVAL;
	}

	if (!db->pcu_pool_fifo_sz) {
		pr_err("Total active pools is not configured\n");
		return -EINVAL;
	}

	if (cfg->num_buffers) {
		/* Configure a new pool */
		if (db->total_free_pcu_fifo_sz < db->pcu_pool_fifo_sz) {
			pr_err("Not enough pcu fifo resources (free %u, needed %u)\n",
			       db->total_free_pcu_fifo_sz,
			       db->pcu_pool_fifo_sz);
			return -EINVAL;
		}

		fifo_sz = db->pcu_pool_fifo_sz;
		db->total_free_pcu_fifo_sz -= db->pcu_pool_fifo_sz;
		pr_debug("Configuring pool %u with fifo size %u (free %u)\n",
			 pool_id, fifo_sz, db->total_free_pcu_fifo_sz);
	} else {
		/* Removing pool */
		fifo_sz = 0;
		db->total_free_pcu_fifo_sz += db->pcu_pool_fifo_sz;
	}

	/* Sets number of buffers in pools */
	reg_val = FIELD_PREP(PP_BM_POOL_SIZE_MSK, cfg->num_buffers);
	PP_REG_WR32(PP_BM_POOL_SIZE_REG_IDX(pool_id), reg_val);

	/* Set Pool Pointer Table */
	PP_REG_WR32(PP_BM_EXT_FIFO_BASE_ADDR_LOW_REG_IDX(pool_id),
		    cfg->ptr_table_addr_low);

	PP_REG_WR32(PP_BM_EXT_FIFO_BASE_ADDR_HIGH_REG_IDX(pool_id),
		    cfg->ptr_table_addr_high);

	PP_REG_WR32(PP_BM_EXT_FIFO_OCC_REG_IDX(pool_id), cfg->num_buffers);

	/* Set FIFO base address */
	reg_val = FIELD_PREP(PP_BM_PCU_FIFO_BASE_ADDR_MSK,
			     fifo_sz ?
			     (BM_START_PCU_FIFO_SRAM_ADDR +
			     (pool_id * fifo_sz)) : 0);

	PP_REG_WR32(PP_BM_PCU_FIFO_BASE_ADDR_REG_IDX(pool_id), reg_val);

	/* Set FIFO size */
	reg_val = FIELD_PREP(PP_BM_PCU_FIFO_SIZE_MSK, fifo_sz);
	PP_REG_WR32(PP_BM_PCU_FIFO_SIZE_REG_IDX(pool_id), reg_val);

	/* Set FIFO occupancy */
	reg_val = FIELD_PREP(PP_BM_PCU_FIFO_OCC_MSK, 0);
	PP_REG_WR32(PP_BM_PCU_FIFO_OCC_REG_IDX(pool_id), reg_val);

	/* Set empty threshold */
	reg_val = FIELD_PREP(PP_BM_PCU_FIFO_PROG_EMPTY_PCU_FIFO_P_EMPTY_MSK,
			     BM_DFLT_PCU_FIFO_LOW_THR(fifo_sz));
	PP_REG_WR32(PP_BM_PCU_FIFO_PROG_EMPTY_REG_IDX(pool_id), reg_val);

	/* Set full threshold */
	reg_val = FIELD_PREP(PP_BM_PCU_FIFO_PROG_FULL_PCU_FIFO_P_FULL_MSK,
			     BM_DFLT_PCU_FIFO_HIGH_THR(fifo_sz));
	PP_REG_WR32(PP_BM_PCU_FIFO_PROG_FULL_REG_IDX(pool_id), reg_val);

	PP_REG_WR32(PP_BM_WATERMARK_LOW_THRSH_REG_IDX(pool_id),
		    BM_DFLT_WATERMARK_LOW_THR(fifo_sz));

	/* Set minimum guaranteed bit per pool */
	bm_pool_min_guarantee_set(pool_id, cfg->min_guaranteed_enable);

	/* Reset Pool FIFO - Set the reset bit to 0 */
	reg_val = PP_REG_RD32(PP_BM_POOL_RESET_REG);
	reg_val = PP_FIELD_MOD(BM_POOL_FIFO_RESET_FIELD(pool_id), 0, reg_val);
	PP_REG_WR32(PP_BM_POOL_RESET_REG, reg_val);

	return 0;
}

#define BM_POP_NULL_ADDRESS 0xFFFFFFFFFULL
#define BM_POP_ADDR_OFFSET(policy) (policy << 8)

s32 bm_pop_buffer(u32 policy, u32 *pool, u64 *buff)
{
	phys_addr_t bm_addr_phys;
	struct pp_qos_dev *qdev;
	s32 ret;
	u32 hi, low;

	if (ptr_is_null(buff))
		return -EINVAL;

	*buff = 0;

	if (policy >= db->max_policies) {
		pr_err("invalid policy id %u, maximum is %d\n",
		       policy, db->max_policies - 1);
		return -EINVAL;
	}

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	/* Write the Policy */
	bm_addr_phys = db->bm_pop_push_addr + BM_POP_ADDR_OFFSET(policy);
	ret = qos_uc_mcdma_copy_from_hw(qdev, bm_addr_phys, buff,
					sizeof(*buff));
	if (ret)
		return ret;

	hi = le32_to_cpu(*buff >> 32);
	low = le32_to_cpu((u32)*buff);
	*buff = le64_to_cpu(PP_BUILD_64BIT_WORD(low, hi));

	if (pool)
		*pool = (*buff & 0xFF00000000000000ULL) >> 56;

	*buff &= 0xFFFFFFFFFULL;
	if (*buff == BM_POP_NULL_ADDRESS)
		*buff = 0;

	return 0;
}

s32 bm_push_buffer(u32 policy, u32 pool, u64 buff)
{
	phys_addr_t bm_addr_phys;
	struct pp_qos_dev *qdev;
	u32 hi, low;

	if (policy >= db->max_policies) {
		pr_err("invalid policy id %u, maximum is %d\n",
			policy, db->max_policies - 1);
		return -EINVAL;
	}

	if (pool >= db->max_pools) {
		pr_err("invalid pool id %u, maximum is %d\n",
			policy, db->max_pools - 1);
		return -EINVAL;
	}

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return -EINVAL;

	bm_addr_phys = db->bm_pop_push_addr + BM_POP_ADDR_OFFSET(policy);
	hi = cpu_to_le32((pool << 24) | (u32)(buff >> 32));
	low = cpu_to_le32((u32)buff);
	buff = cpu_to_le64(PP_BUILD_64BIT_WORD(low, hi));
	return qos_uc_mcdma_copy_to_hw(qdev, &buff, bm_addr_phys, sizeof(buff));
}
