/*
 * bm.h
 * Description: PP Buffer Manager Hal Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef _BM_H_
#define _BM_H_

#include <linux/debugfs.h>
#include <linux/device.h>
#include "pp_buffer_mgr.h"

/**
 * @brief BM_MAX_GROUPS
 *       Max supoorted groups. Real max defined in the DTS
 */
#define BM_MAX_GROUPS          16

/**
 * @brief Invalid group id definition
 */
#define PP_BM_INVALID_GROUP_ID U8_MAX

/**
 * @brief BM_MAX_ISOLATED_GROUPS
 *       Max isolated groups is max groups
 */
#define BM_MAX_ISOLATED_GROUPS BM_MAX_GROUPS

/**
 * \brief This structure is used for allocate and deallocate
 *      buffer
 * @address buffer ptr
 * @policy_id policy to allocate from
 * @pool_id pool for deallocate buffer back
 */
struct bmgr_buff_info {
	u64 address;
	u16 policy_id;
	u8  pool_id;
	u8  reserved;
};

/**
 * @brief bmgr group statistics
 * @grp_avail_buff A counter of the number of currently
 *      available buffers in this Group
 * @grp_rsrvd_buff A counter of the number of currently reserved
 *      buffers in this Group
 */
struct bmgr_group_stats {
	u32 grp_avail_buff;
	u32 grp_rsrvd_buff;
};

/**
 * @brief This struct defines the driver's cfg regs
 */
struct bmgr_cfg_regs {
	u32 ctrl_reg;
	u32 min_grant_reg;
	u32 pool_enable_reg;
	u32 pool_fifo_reset_reg;
	u32 ocpm_burst_size_reg;
	u32 ocpm_num_bursts_reg;
	u32 status_reg;
};

/**
 * @brief Pool Configuration
 * @param num_buffers number of buffers in pool
 * @param ptr_table_addr_low buffer pointer table - low addr
 * @param ptr_table_addr_high buffer pointer table - high addr
 * @param min_guaranteed_enable enable pool minimum guaranteed
 */
struct bmgr_pool_cfg {
	u32 num_buffers;
	u32 ptr_table_addr_low;
	u32 ptr_table_addr_high;
	bool min_guaranteed_enable;
};

/**
 * @brief Policy Configuration
 * @param group_id associated group id
 * @param max_allowed policy maximum allowed
 * @param min_guaranteed policy minimum guaranteed
 */
struct bmgr_policy_cfg {
	u8 group_id;
	u32 max_allowed;
	u32 min_guaranteed;
};

/**
 * \brief Buffer manager initial configuration
 * @valid params valid
 * @bm_base bm base address
 * @bm_ram_base bm ram base address
 * @max_pools Max number of pools
 * @max_groups Max number of groups
 * @max_policies Max number of policies
 * @pcu_fifo_sz Number of buffer pointers rows BM cache can
 *        contain. Each row contains 4 pointers
 */
struct pp_bm_init_param {
	bool valid;
	u64  bm_base;
	u64  bm_ram_base;
	u32  max_pools;
	u32  max_groups;
	u32  max_policies;
	u32  pcu_fifo_sz;
	phys_addr_t bm_pop_push_addr;
};

/**
 * @brief Set ammount of active pools
 * @param num_active_pools number of active pools in the system
 */
s32 bm_set_total_active_pools(u32 num_active_pools);

/**
 * @brief Get cfg registers
 * @param regs registers structure
 */
void bm_get_cfg_regs(struct bmgr_cfg_regs * const regs);

/**
 * @brief Poll bm ctrl registers for operation completion
 * @return 0 on success, other error code on failure
 */
s32 bm_ctrl_poll(void);

/**
 * @brief Enable/disable pool's minimum guarantee
 * @param pool_id pool id
 * @param en enable/disable
 * @return 0 on success, other error code on failure
 */
s32 bm_pool_min_guarantee_set(u8 pool_id, bool en);

/**
 * @brief Pool Configuration
 * @param pool_id pool id
 * @param cfg pool configuration
 * @return 0 on success, other error code on failure
 */
s32 bm_pool_configure(u8 pool_id, const struct bmgr_pool_cfg * const cfg);

/**
 * @brief Set pop enable/disable
 * @param enable when set, pop from pool is enabled
 * @return 0 on success, other error code on failure
 */
s32 bm_pool_pop_enable(u8 pool_id, bool enable);

/**
 * @brief Get pop enable/disable status
 * @param enable pool pop status
 * @return 0 on success, other error code on failure
 */
s32 bm_pool_pop_status_get(u8 pool_id, bool *enable);

/**
 * @brief Set pool enable/disable
 * @param enable when set, pool is enabled
 * @return 0 on success, other error code on failure
 */
s32 bm_pool_enable(u8 pool_id, bool enable);

/**
 * @brief Get pool statistics
 * @param stats statistics structure
 * @param pool_id pool id
 * @return 0 on success, other error code on failure
 */
s32 bm_pool_stats_get(struct pp_bmgr_pool_stats * const stats, u8 pool_id);

/**
 * @brief Policy Configuration
 * @param policy_id policy id
 * @param cfg policy configuration
 * @return 0 on success, other error code on failure
 */
s32 bm_policy_configure(u16 policy_id, const struct bmgr_policy_cfg *cfg);

/**
 * @brief Maps pool to policy (With priority)
 * @param policy_id Policy ID
 * @param pool_id Pool ID
 * @param priority 0 (highest priority) - 3 scale
 * @param max_allowed maximum allowed per pool
 * @param map_pool_to_policy true to map, false to unmap
 * @return 0 on success, other error code on failure
 */
s32 bm_policy_pool_mapping_set(u16 policy_id,
			       u8 pool_id,
			       u8 priority,
			       u32 max_allowed,
			       bool map_pool_to_policy);

/**
 * @brief Set policy default configuration
 * @param policy_id Policy ID
 * @return 0 on success, other error code on failure
 */
s32 bm_policy_default_set(u16 policy_idx);

/**
 * @brief Get policy statistics
 * @param stats statistics structure
 * @param policy_id policy id
 * @return 0 on success, other error code on failure
 */
s32 bm_policy_stats_get(struct pp_bmgr_policy_stats * const stats,
			u16 policy_id);

/**
 * @brief Set group available buffers
 * @param group_id group id
 * @param available available buffers
 * @return 0 on success, other error code on failure
 */
s32 bm_group_available_set(u8 group_id, u32 available);

/**
 * @brief Get group available buffers
 * @param group_id group id
 * @param available available buffers
 * @return 0 on success, other error code on failure
 */
s32 bm_group_available_get(u8 group_id, u32 *available);

/**
 * @brief Set group reserved buffers
 * @param group_id group id
 * @param reserved reserved buffers
 * @return 0 on success, other error code on failure
 */
s32 bm_group_reserved_set(u8 group_id, u32 reserved);

/**
 * @brief Get group reserved buffers
 * @param group_id group id
 * @param reserved reserved buffers
 * @return 0 on success, other error code on failure
 */
s32 bm_group_reserved_get(u8 group_id, u32 *reserved);

/**
 * @brief Get group statistics
 * @param group_id group id
 * @param stats statistics structure
 * @param group_id group id
 * @return 0 on success, other error code on failure
 */
s32 bm_group_stats_get(u8 group_id, struct bmgr_group_stats * const stats);

/**
 * @brief Pop a buffer from a policy
 * @param policy policy id to pop from
 * @param pool the pool id the buffer was popped from
 * @param buff the buffer pointer
 * @return s32 0 on success, error code otherwise
 */
s32 bm_pop_buffer(u32 policy, u32 *pool, u64 *buff);

/**
 * @brief Push a buffer to a policy and pool
 * @param policy policy id to push to
 * @param pool pool id to push to
 * @param buff the buffer pointer
 * @return s32 0 on success, error code otherwise
 */
s32 bm_push_buffer(u32 policy, u32 pool, u64 buff);

/**
 * @brief Set default configuration
 * @param base_addr module's base address
 * @param cfg Buffer manager configuration
 */
s32 bm_init(const struct pp_bm_init_param * const cfg);

/**
 * @brief Clean resources
 */
void bm_exit(void);

#endif /* _BM_H_ */
