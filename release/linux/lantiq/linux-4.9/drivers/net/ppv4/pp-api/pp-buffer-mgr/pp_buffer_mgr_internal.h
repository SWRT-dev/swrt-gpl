/*
 * pp_bm_internal.h
 * Description: Packet Processor Buffer Manager Driver Internal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#ifndef _BM_DRV_INTERNAL_H_
#define _BM_DRV_INTERNAL_H_

#include <linux/debugfs.h>
#include <linux/device.h>
#include <linux/pp_api.h>
#include "pp_buffer_mgr.h"
#include "bm.h"

/**
 * @brief Shortcut to iterate through BM pools
 * @param p loop iterator
 */
#define BM_FOR_EACH_POOL(_db, p) \
	for (p = 0; p < (_db)->cfg.max_pools; p++)

/**
 * @brief Shortcut to iterate through BM policies
 * @param p loop iterator
 */
#define BM_FOR_EACH_POLICY(_db, p) \
	for (p = 0; p < (_db)->cfg.max_policies; p++)

/**
 * @brief Shortcut to iterate through BM groups
 * @param g loop iterator
 */
#define BM_FOR_EACH_GROUP(_db, g) \
	for (g = 0; g < (_db)->cfg.max_groups; g++)

/**
 * @brief BM_MIN_POOL_BUFFER_SIZE Minimum buffer size in
 *      configured pool
 */
#define BM_MIN_POOL_BUFFER_SIZE                       (64)

/**
 * @brief BM_POLICY_RESET_DELAY_MS
 */
#define BM_POLICY_RESET_DELAY_MS                      (10)

/**
 * @brief This structure holds the pool database
 */
struct bmgr_pool_db_entry {
	struct pp_bmgr_pool_params params; /*! pools params */
	ulong  policies_bmap[BITS_TO_LONGS(PP_BM_MAX_POLICIES)];
	u32    num_allocated_buffers;   /*! Num allocated buffers */
	u32    num_deallocated_buffers; /*! Num deallocated buffers */
	u32    policy_ref_cnt;
	u8     pool_id;
	void *virt; /*! pool virtual address, used when the pool is
			   *  allocated internally by the buffer manager
			   */
	dma_addr_t phys;  /*! pool physical address */
	size_t sz;        /*! pool size */
	void      *internal_ptrs_virt; /*! Ptrs table to be used in HW */
	dma_addr_t internal_ptrs_phys;
	size_t     internal_ptrs_sz;
};

/**
 * @brief This structure holds the policy database
 * @policy_params Policy params
 * @pools_bmap bit per pool specified which pools are used by the policy
 * @num_allocated_buffers Num allocated buffers
 * @num_deallocated_buffers Num deallocated buffers
 * @is_busy Is entry in used
 */
struct bmgr_policy_db_entry {
	struct pp_bmgr_policy_params params;
	bool   is_isolated;
	u32    num_allocated_buffers;
	u32    num_deallocated_buffers;
	u8     group_id;
};

/**
 * @brief This structure holds the buffer manager driver statistics
 * @num_succ_pop_req Number of successful pop requests
 * @num_fail_pop_req Number of failed pop requests
 * @num_succ_push_req Number of successful push requests
 * @num_fail_push_req Number of failed push requests
 */
struct bmgr_driver_stats {
	atomic_t    num_succ_pop_req;
	atomic_t    num_fail_pop_req;
	atomic_t    num_succ_push_req;
	atomic_t    num_fail_push_req;
};

/**
 * @brief This structure holds the buffer manager database
 * @cfg Initial configuration
 * @pools Pools information
 * @policies Policies information
 * @group Group information
 * @stats driver stats for pop and push requests
 * @ready specify if the buffer manager ready to be called
 * @db_lock
 * @num_pools Number of active pools
 * @num_policies Number of active policies
 */
struct bmgr_driver_db {
	struct pp_bmgr_init_param   cfg;
	struct bmgr_pool_db_entry   pools[PP_BM_MAX_POOLS];
	struct bmgr_policy_db_entry policies[PP_BM_MAX_POLICIES];
	struct bmgr_group_db_entry  groups[BM_MAX_GROUPS];
	struct bmgr_driver_stats    stats;
	bool        ready;
	spinlock_t  db_lock;
	u32         num_pools;
	u32         num_policies;
	u32         num_groups;
	ulong  policies_bmap[BITS_TO_LONGS(PP_BM_MAX_POLICIES)];
	ulong  pools_bmap[BITS_TO_LONGS(PP_BM_MAX_POOLS)];
	ulong  groups_bmap[BITS_TO_LONGS(BM_MAX_GROUPS)];
};

/**
 * @brief Get num of successful pop requests
 * @return Successful pop requests count
 */
u32 bmgr_get_succ_pop_req(void);

/**
 * @brief Get num of failed pop requests
 * @return Failed pop requests count
 */
u32 bmgr_get_fail_pop_req(void);

/**
 * @brief Get num of successful push requests
 * @return Successful push requests count
 */
u32 bmgr_get_succ_push_req(void);

/**
 * @brief Get num of failed push requests
 * @return Failed push requests count
 */
u32 bmgr_get_fail_push_req(void);

/**
 * @brief Get buffer manager config registers
 * @param config struct
 * @return 0 on success, other error code on failure
 */
s32 bmgr_get_cfg_regs(struct bmgr_cfg_regs * const regs);

/**
 * @brief Get buffer manager group statistics
 * @param group stats statistics struct
 * @param group_id group id
 * @return 0 on success, other error code on failure
 */
s32 bmgr_group_stats_get(struct bmgr_group_stats * const stats, u8 group_id);

/**
 * @brief Verify the pool doesn't contain duplicate buffers or
 *        buffers which are out of the pool addresses range
 * @note this should not run under traffic
 * @param id pool id to verify
 * @return s32 0 if the pool is ok, error code if something is wrong
 *             or the verification failed
 */
s32 pp_bmgr_pool_verification(u32 id);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief bm debug init
 * @param db db structure
 * @return s32 0 for success, non-zero otherwise
 */
s32 bm_dbg_init(struct bmgr_driver_db *db, struct dentry *parent);

/**
 * @brief bm debug cleanup
 * @param void
 * @return s32 0 for success, non-zero otherwise
 */
void bm_dbg_clean(void);
#else
static inline s32 bm_dbg_init(struct bmgr_driver_db *db, struct dentry *parent)
{
	return 0;
}

static inline void bm_dbg_clean(void)
{}
#endif

#endif /* _BM_DRV_INTERNAL_H_ */
