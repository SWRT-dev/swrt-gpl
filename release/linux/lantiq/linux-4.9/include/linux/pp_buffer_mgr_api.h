/*
 * pp_buffer_mgr_api.h
 * Description: Packet Processor Buffer Manager API
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#ifndef __PP_BUFFER_MGR_API_H__
#define __PP_BUFFER_MGR_API_H__

#include <linux/types.h>

/**
 * @file
 * @brief Buffer Manager API
 */

/**
 * @define PP_BM_INVALID_POOL_ID
 */
#define PP_BM_INVALID_POOL_ID                (U8_MAX)

/**
 * @define PP_BM_INVALID_POLICY_ID
 */
#define PP_BM_INVALID_POLICY_ID              (U16_MAX)

/**
 * @brief PP_BM_MAX_POOLS
 *       Max supported pools. Real max defined in the DTS
 */
#define PP_BM_MAX_POOLS                      (16)

/**
 * @define PP_BM_MAX_POOLS_PER_PLCY
 * @brief Max pools per policy
 */
#define PP_BM_MAX_POOLS_PER_PLCY             (4)

/**
 * @define pool_flags_bitmap
 * @brief  bmgr pools flags (Used in
 *        pp_bmgr_pool_params.flags)
 *        POOL_ENABLE_FOR_MIN_GRNT_POLICY_CALC When set pool
 *        will take part in policy minimum guaranteed
 *        calculation
 * @note Deprecated, this will be determined by the pp internally
 */
#define POOL_ENABLE_FOR_MIN_GRNT_POLICY_CALC BIT(0)

/**
 * @define pool_flags_bitmap
 * @brief POOL_ISOLATED When set this pool will be allowed to be
 *        associated with only 1 policy
 */
#define POOL_ISOLATED                        BIT(1)

#define BMGR_NUM_FLAGS 2

/**
 * @struct pp_bmgr_pool_params
 * @brief This structure is used in bmgr_pool_configure API in
 *        parameter
 */
struct pp_bmgr_pool_params {
	u16 flags;          /*! Pool flags (pool_flags_bitmap) */
	u32 num_buffers;    /*! Amount of buffers in pool */
	u32 size_of_buffer; /*! Buff size for pool (in bytes). Min is 64B */
	u32 base_addr_low;  /*! Base address of the pool (low) */
	u32 base_addr_high; /*! Base address of the pool (high) */
};

/**
 * @struct pp_bmgr_pool_in_policy_info
 * @brief This structure is used in policy_params struct and
 *      holds the information about the pools in policy
 */
struct pp_bmgr_pool_in_policy_info {
	u8  pool_id;     /*! Pool id */
	u32 max_allowed; /*! Max allowed per pool per policy */
};

/**
 * @struct pp_bmgr_policy_params
 * @brief This structure is used in bmgr_policy_configure API in
 *      parameter
 */
struct pp_bmgr_policy_params {
	/*! Policy maximum allowed */
	u32    max_allowed;

	/*! Policy minimum guaranteed */
	u32    min_guaranteed;

	/*! Pools information. Sorted from high priority (index 0) to lowest */
	struct pp_bmgr_pool_in_policy_info
		pools_in_policy[PP_BM_MAX_POOLS_PER_PLCY];

	/*! Number of pools in pools_in_policy */
	u8     num_pools_in_policy;
};

/**
 * @struct pp_bmgr_pool_stats
 * @brief bmgr pool statistics
 */
struct pp_bmgr_pool_stats {
	u32 pool_size;                /*! Number of Buffers in pool */
	u32 pcu_fifo_base_addr;       /*! PCU FIFO Base Addr */
	u32 pcu_fifo_size;            /*! Pool Size in SRAM */
	u32 pcu_fifo_occupancy;       /*! FIFO Occupancy in SRAM */
	u32 pcu_fifo_prog_empty;      /*! Programmable Empty Threshold */
	u32 pcu_fifo_prog_full;       /*! Programmable Full Threshold */
	u64 ext_fifo_base_addr;       /*! Pool Base Address in DDR */
	u32 ext_fifo_occupancy;       /*! External FIFO Occupancy */
	u32 pool_allocated_ctr;       /*! Current allocated buffers */
	u32 pool_pop_ctr;             /*! Number of popped buffers */
	u32 pool_push_ctr;            /*! Number of buffers pushed back */
	u32 pool_ddr_burst_write_ctr; /*! Number of DDR Write bursts */
	u32 pool_ddr_burst_read_ctr;  /*! Number of DDR Read bursts */
	u32 pool_watermark_low_thr;   /*! WaterMark Low Threshold */
	u32 pool_watermark_low_ctr;   /*! WaterMark Low Counter */
};

/**
 * @struct pp_bmgr_policy_stats
 * @brief bmgr policy statistics
 */
struct pp_bmgr_policy_stats {
	/*! Number of failed buffer pops */
	u32 policy_null_ctr;

	/*! Maximum allowed */
	u32 policy_max_allowed;

	/*! Minimum guaranteed */
	u32 policy_min_guaranteed;

	/*! Group associated with this Policy */
	u32 policy_grp_association;

	/*! Mapped pools */
	ulong pools_bmap[BITS_TO_LONGS(PP_BM_MAX_POOLS)];

	/*! Total allocated buffers */
	u32 policy_alloc_buff;

	/*! Maximum allowed per pool */
	u32 policy_max_allowed_per_pool[PP_BM_MAX_POOLS_PER_PLCY];

	/*! Allocated buffers per pool in policy */
	u32 policy_alloc_buff_per_pool[PP_BM_MAX_POOLS_PER_PLCY];
};

/**
 * @brief Configure a Buffer Manager pool
 * @param pool_params: Pool param from user
 * @param pool_id[IN/OUT]: set to PP_BM_INVALID_POOL_ID for
 *      automatic Pool ID
 * @return 0 on success, other error code on failure
 */
s32
pp_bmgr_pool_configure(const struct pp_bmgr_pool_params * const pool_params,
		       u8 *pool_id);

/**
 * @brief Get pool configuration
 * @param pool_params: Pool param to user
 * @param pool_id: Pool id to get config from
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_pool_conf_get(u8 pool_id,
			  struct pp_bmgr_pool_params * const pool_params);

/**
 * @brief Disable pop from a pool
 * @param pool_id Pool to disable pop from
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_pool_pop_disable(u8 pool_id);

/**
 * @brief Remove pool
 * @param pool_id Pool to remove
 * @note must be called after pool pop disabled and all
 *      resources are back to pool
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_pool_remove(u8 pool_id);

/**
 * @brief Configure a Buffer Manager policy
 * @param policy_params: Policy param from user
 * @param policy_id[IN/OUT]: set to PP_BM_INVALID_POLICY_ID for
 *      automatic Policy ID
 * @return 0 on success, other error code on failure
 */
s32
pp_bmgr_policy_configure(
	const struct pp_bmgr_policy_params * const policy_params,
	u16 *policy_id);

/**
 * @brief Get policy configuration
 * @param policy_params: Policy param to user
 * @param policy_id: Policy id to get config from
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_policy_conf_get(u16 policy_id,
			    struct pp_bmgr_policy_params * const policy_params);

/**
 * @brief Remove a Buffer Manager policy
 * @param policy_id Policy ID to remove
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_policy_remove(u16 policy_id);

/**
 * @brief Resets an isolated Buffer Manager policy
 * @param policy_id Policy ID to reset
 * @param cb Callback for operation result notifiaction
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_policy_reset(u16 policy_id, void (*cb)(s32 ret));

/**
 * @brief Get buffer manager pool statistics
 * @param pool stats statistics struct
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_pool_stats_get(struct pp_bmgr_pool_stats * const stats,
			   u8 pool_id);

/**
 * @brief Get buffer manager policy statistics
 * @param policy stats statistics struct
 * @return 0 on success, other error code on failure
 */

s32 pp_bmgr_policy_stats_get(struct pp_bmgr_policy_stats * const stats,
			     u16 policy_id);

/**
 * @brief Check if policy is active
 * @param policy Policy ID
 * @return bool true if policy active, false otherwise
 */
bool pp_bmgr_is_policy_active(u16 policy);

/**
 * @brief Set ammount of active pools
 * @param num_active_pools number of active pools in the system
 * @return 0 on success, other error code on failure
 */
s32 pp_bmgr_set_total_active_pools(u32 num_active_pools);

/**
 * @brief Pop a buffer from a policy. This API shall not be used for data flows
 * @param policy policy id to pop from
 * @param pool the pool id the buffer was popped from
 * @param buff the buffer pointer
 * @return s32 0 on success, error code otherwise
 */
s32 pp_bmgr_pop_buffer(u32 policy, u32 *pool, u64 *buff);

/**
 * @brief Push a buffer to a policy and pool. This API shall not be used for data flows
 * @param policy policy id to push to
 * @param pool pool id to push to
 * @param buff the buffer pointer
 * @return s32 0 on success, error code otherwise
 */
s32 pp_bmgr_push_buffer(u32 policy, u32 pool, u64 buff);
#endif /* __PP_BUFFER_MGR_API_H__ */
