/*
 * bm_internal.h
 * Description: PP Buffer Manager Hal Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#ifndef _BM_INTERNAL_H_
#define _BM_INTERNAL_H_

#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/pp_api.h>
#include "pp_regs.h"
#include "buffer_manager_regs.h"
#include "pp_buffer_mgr.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[BM]:%s:%d: " fmt, __func__, __LINE__
#endif

extern u64 bm_base_addr;
extern u64 bm_ram_base_addr;
#define BM_BASE_ADDR bm_base_addr
#define PP_BM_RAM_BASE bm_ram_base_addr

/*! \def BM_DRIVER_VERSION */
#define BM_DRIVER_VERSION                             "1.0.1"

/*! \def BM_START_PCU_SRAM
 *       Start PCU address in SRAM. Used in configure pool
 */
#define BM_START_PCU_FIFO_SRAM_ADDR                   (0)

/*! \def BM_DEFAULT_PCU_FIFO_LOW_THR
 *       PCU fifo low threshold - If BM buffer pointers rows in cache are less
 *          than this threshold, BM will start fetching buffers from
 *          DDR to cache
 *          ammount of buffers is (NUM_BURSTS * BURST_SIZE / 4)
 *          Threshold configured to fifo size / 4
 */
#define BM_DFLT_PCU_FIFO_LOW_THR(sz)                  (sz >> 2)

/*! \def BM_DEFAULT_PCU_FIFO_HIGH_THR
 *       PCU fifo high threshold - If BM buffer pointers rows in cache are more
 *          than this threshold, BM will start returning buffers from
 *          cache to DDR
 *          ammount of buffers is (NUM_BURSTS * BURST_SIZE / 4)
 *          Threshold configured to fifo size * 3/4
 */
#define BM_DFLT_PCU_FIFO_HIGH_THR(sz)                 ((sz >> 2) * 3)

/*! \def BM_DEFAULT_WATERMARK_LOW_THR
 *       Watermark low threshold - Counts how many times BM cache
 *       contained less than this threshold
 *       Threshold configured to fifo size / 2
 */
#define BM_DFLT_WATERMARK_LOW_THR(sz)                 (sz >> 1)

/**
 * @brief enum to describe the pool's priority
 * @bm_policy_pool_priority_high High priority
 * @bm_policy_pool_priority_mid_high Mid-High priority
 * @bm_policy_pool_priority_mid_low Mid-Low priority
 * @bm_policy_pool_priority_low Low priority
 * @bm_policy_pool_priority_max Last priority
 */
enum bm_policy_pools_priority_e {
	BM_POLICY_POOL_PRIORITY_HIGH,
	BM_POLICY_POOL_PRIORITY_MID_HIGH,
	BM_POLICY_POOL_PRIORITY_MID_LOW,
	BM_POLICY_POOL_PRIORITY_LOW,
	BM_POLICY_POOL_PRIORITY_MAX
};

struct bmgr_db {
	u32 max_pools;
	u32 max_groups;
	u32 max_policies;
	u32 total_configured_pcu_fifo_sz;
	u32 total_free_pcu_fifo_sz;
	u32 pcu_pool_fifo_sz;
	u32 pcu_fifo_sz_alignment;
	phys_addr_t bm_pop_push_addr;
};

#define BM_OFFSET_OF_PLCY(policy)                     (policy<<2)
#define BM_OFFSET_OF_POOL(pool)                       (pool<<2)
#define BM_OFFSET_OF_GRP(group)                       (group<<2)
#define BM_OFFSET_OF_POOL_IN_PLCY(pool, policy)                  \
	((BM_OFFSET_OF_PLCY(policy) * PP_BM_MAX_POOLS_PER_PLCY) \
	+ BM_OFFSET_OF_POOL(pool))

/**
 * @brief Min Guaranteed Mask Per Pool
 * Bitmask per pool which one should be Calculated in Minimum
 * Guaranteee
 */
#define BM_POOL_MIN_GRNT_FIELD(pool)                  (BIT(pool))

/* Pool Enable */
/* Register value differs between Falcon & LGM */
/* The 2 MSBytes will be written in both cases although in */
/* LGM case these bytes are reserved and read-only */
#define BM_POOL_ENABLE_FIELD(pool)                    \
	((BIT(pool) << 16) | BIT(pool))

/**
 * @brief Pool FIFO Reset
 * Reset FIFOs (PU/EXT) Bits[15:0] per Pools[15:0] active high
 */
#define BM_POOL_FIFO_RESET_FIELD(pool)                (BIT(pool))

/**
 * @brief OCP Master Burst Size
 * 2 bits per pool pool[0]=bits[1:0] etc...
 *      0 - 64B Burst
 *      1 - 128B Burst
 *      2 - 256B Burst
 *      3 - 512B Burst
 */
#define BM_OCPM_BURST_SIZE_64B                        (0)
#define BM_OCPM_BURST_SIZE_128B                       (1)
#define BM_OCPM_BURST_SIZE_256B                       (2)
#define BM_OCPM_BURST_SIZE_512B                       (3)

/* Convert burst size index to actual value */
#define BM_OCPM_BURST_SIZE_VAL(sz)                    (BIT((sz) + 6))

#define BM_OCPM_BURST_SIZE_FIELD(pool)      \
	GENMASK((pool << 1) + 1, pool << 1)

/**
 * @brief OCP Master Number Of Bursts
 * 2 bits per pool pool[0]=bits[1:0] etc...
 *      0 - 1 Burst
 *      1 - 2 Bursts
 *      2 - 3 Bursts
 *      3 - 4 Bursts
 */
#define BM_OCPM_NUM_OF_BURSTS_1                       (0)
#define BM_OCPM_NUM_OF_BURSTS_2                       (1)
#define BM_OCPM_NUM_OF_BURSTS_3                       (2)
#define BM_OCPM_NUM_OF_BURSTS_4                       (3)

/* Convert num bursts index to actual value */
#define BM_OCPM_NUM_OF_BURSTS_VAL(num)                ((num) + 1)

#define BM_OCPM_NUM_BURSTS_FIELD(pool)      \
	GENMASK((pool << 1) + 1, pool << 1)

/**
 * @brief Pool pop enable
 * Pool pop Enable Bits[15:0] per Pools[15:0]
 */
#define BM_POOL_POP_ENABLE_FIELD(pool)                (BIT(pool))

/**
 * @brief Maximum Allowed Per Policy
 *      [31-0] [RW]: Maximum Allowed Per Policy
 */
#define BM_PLCY_MAX_ALLOWED_BASE                      (PP_BM_RAM_BASE + 0x0000)
#define BM_PLCY_MAX_ALLOWED(policy)                            \
	(BM_PLCY_MAX_ALLOWED_BASE + BM_OFFSET_OF_PLCY(policy))

/**
 * @brief Minimum Guaranteed Per Policy
 *      [31-0] [RW]: Minimum Guaranteed Per Policy
 */
#define BM_PLCY_MIN_GUARANTEED_BASE                   (PP_BM_RAM_BASE + 0x1000)
#define BM_PLCY_MIN_GUARANTEED(policy)                            \
	(BM_PLCY_MIN_GUARANTEED_BASE + BM_OFFSET_OF_PLCY(policy))

/**
 * @brief The Group associated with this Policy
 *      [31-0] [RW]: The Group associated with this Policy
 */
#define BM_PLCY_GRP_ASSOCIATED_BASE                   (PP_BM_RAM_BASE + 0x2000)
#define BM_PLCY_GRP_ASSOCIATED(policy)                            \
	(BM_PLCY_GRP_ASSOCIATED_BASE + BM_OFFSET_OF_PLCY(policy))

/**
 * @brief Mapping Pools to Policies
 *      [31-0] [RW]: 32bit are 4 bytes, each byte describes pool number
.*      Policy[0] = {8'b7, 8'b0, 8'b1, 8'b2}. This means that
 *      policy[0] has pools 2,1,0,7 where 2 has highest priority
 */
#define BM_PLCY_POOLS_MAPPING_BASE                    (PP_BM_RAM_BASE + 0x3000)
#define BM_PLCY_POOLS_MAPPING(policy)                            \
	(BM_PLCY_POOLS_MAPPING_BASE + BM_OFFSET_OF_PLCY(policy))
#define BM_POOL_PRIORITY_IN_POLICY_MASK(priority)                \
	(U8_MAX << (BITS_PER_BYTE * priority))

/**
 * @brief Maximum Allowed Per Pool Per Policy
 *      [31-0] [RW]: 4 pools per policy, 4 RAMs n=[0..3],
.*      one RAM per pool, where n is pool[n] per policy
 */
#define BM_PLCY_MAX_ALLOW_PER_POOL_BASE               (PP_BM_RAM_BASE + 0x4000)
#define BM_PLCY_MAX_ALLOW_PER_POOL(policy, pool)                       \
	(BM_PLCY_MAX_ALLOW_PER_POOL_BASE + BM_OFFSET_OF_PLCY(policy) + \
	(pool * 0x1000))

/**
 * @brief Allocated buffers per policy
 *      [31-0] [RO]: Allocated buffers per policy
 */
#define BM_PLCY_ALLOC_BUFF_CTR_BASE                   (PP_BM_RAM_BASE + 0xA000)
#define BM_PLCY_ALLOC_BUFF_CTR(policy)                            \
	(BM_PLCY_ALLOC_BUFF_CTR_BASE + BM_OFFSET_OF_PLCY(policy))

/**
 * @brief Allocated buffers per pool per policy
 *      [31-0] [RO]: Allocated buffers per pool per policy
 */
#define BM_PLCY_ALOC_BUF_PR_POL_CTR_BASE              (PP_BM_RAM_BASE + 0xB000)
#define BM_PLCY_ALOC_BUF_PR_POL_CTR(pool, policy) \
	(BM_PLCY_ALOC_BUF_PR_POL_CTR_BASE +       \
	BM_OFFSET_OF_POOL_IN_PLCY(pool, policy))

#endif /* _BM_INTERNAL_H_ */
