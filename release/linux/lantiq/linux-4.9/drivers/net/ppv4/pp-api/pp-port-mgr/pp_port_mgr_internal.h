/*
 * pp_port_mgr_internal.h
 * Description: Packet Processor Port Manager Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#ifndef __PP_PORT_MGR_INTERNAL_H__
#define __PP_PORT_MGR_INTERNAL_H__
#include <linux/types.h>
#include <linux/spinlock.h>
#include <linux/pp_api.h>
#include "pp_common.h"
#include "checker.h"
#include "pp_port_mgr.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_PORT_MGR]:%s:%d: " fmt, __func__, __LINE__
#endif

/**
 * @define number of supported datapaths for host interface
 * @note HAL exception session 63 is assign to drop errors
 */
#define PMGR_HIF_DB_DP_MAX             (CHK_NUM_EXCEPTION_SESSIONS - 1)

/**
 * @define used to mark dp number as invalid
 */
#define PMGR_HIF_DP_INVALID            PMGR_HIF_DB_DP_MAX

/**
 * @define check if dp number is valid
 */
#define PMGR_HIF_IS_DP_VALID(dp)       \
	(0 <= (dp) && (dp) < PMGR_HIF_DP_INVALID)

/**
 * @define going trough all the host interfaces in database
 *         Don't do it for dp 0/1 since they are driver internal
 */
#define PMGR_HIF_DB_FOR_EACH_DP(i)     \
	for (i = 2; i < PMGR_HIF_DB_DP_MAX; i++)

/**
 * @define for host interface datapaths database, only queue
 *         index 0 is in used (for using the same data structure
 *         in database
 */
#define PMGR_HIF_DB_EGRESS_IDX         (0)

/**
 * @define going trough all the host interface hash of hash
 *         entries
 */
#define PMGR_HIF_FOR_EACH_DP_HOH(i)    \
	for (i = 0; i < CHK_HOH_MAX; i++)

/**
 * @define going trough all the host interface queues
 */
#define PMGR_HIF_FOR_EACH_DP_EGRESS(i) for (i = 0; i < PP_HOSTIF_EG_MAX; i++)

/**
 * @define going trough all the host interface sgcs
 */
#define PMGR_HIF_FOR_EACH_DP_SGC(i)    for (i = 0; i < PP_SI_SGC_MAX; i++)

/**
 * @define going trough all the host interface tbms
 */
#define PMGR_HIF_FOR_EACH_DP_TBM(i)    for (i = 0; i < PP_SI_TBM_MAX; i++)

/**
 * @define find the datapath index in datapath array by the
 *         number of datapaths and the hash of hash value
 */
#define PMGR_HOH_TO_DP_IDX(hoh, dp_cnt)    \
	(((hoh) * (dp_cnt)) / CHK_HOH_MAX)

/**
 * @struct module ports database
 * @cfg port configuration
 * @rpb_map the RPB port mapping
 * @active port status
 * @map_id exception session mappings.
 *         When port goes to protected mode, all mapping is saved here.
 *         Once it goes back to normal mode, the original mapping are applied
 * @protected is port protected
 */
struct pmgr_db_port {
	struct pp_port_cfg cfg;
	u32                rpb_map;
	bool               active;
	u8                 map_id[PP_MAX_TC][CHK_HOH_MAX];
	bool               protected;
};

/**
 * @struct module database
 */
struct pmgr_db {
	struct pp_hif_datapath hif_dp[PMGR_HIF_DB_DP_MAX];
	u32                    hif_refc[PMGR_HIF_DB_DP_MAX];
	bool                   dflt_dp_set;
	struct pmgr_db_port    port[PP_MAX_PORT];
	struct pmgr_stats      stats;
	spinlock_t             hif_lock;
	spinlock_t             port_lock;
};

/**
 * @brief Get ports stats
 * @param stats pointer to save the stats
 * @param num_stats number of stats entries
 * @param data user data
 * @return s32 0 on success, error code otherwise
 */
s32 pmgr_ports_stats_get(void *stats, u32 num_stats, void *data);

/**
 * @brief Calculates the difference between ports stats
 * @param pre pre stats
 * @param num_pre number of pre stats
 * @param post post stats
 * @param num_post number of post stats
 * @param delta stats to save the delta
 * @param num_delta number of delta stats
 * @note all stats array MUST be with same size
 * @return s32 0 on success, error code otherwise
 */
s32 pmgr_ports_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			  void *delta, u32 num_delta, void *data);

/**
 * @brief Prints ports stats into a buffer
 * @param buf the buffer to print into
 * @param sz buffer size
 * @param n pointer to update number of written bytes
 * @param stats the stats
 * @param num_stats number of stats
 * @return s32 0 on success, error code otherwise
 */
s32 pmgr_ports_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			  u32 num_stats, void *data);

/**
 * @brief get the port mapping in RPB
 * @param pid port number
 * @param rpb_port RPB port map
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_port_rpb_map_get(u16 pid, u32 *rpb_port);

/**
 * @brief get the host interface datapath from databse
 * @param dp_idx datapath index in database
 * @param dp datapath configuration
 * @return S32 0 for success, non-zero otherwise
 */
s32 pmgr_hif_dp_get(u8 dp_idx, struct pp_hif_datapath *dp);

/**
 * @brief get the host interface datapath refc from databse
 * @param dp_idx datapath index in database
 * @param refc reference count
 * @return S32 0 for success, non-zero otherwise
 */
s32 pmgr_hif_dp_refc_get(u8 dp_idx, u32 *refc);

/**
 * @brief get the host interface mapping per ingress
 *        port/tc/hoh
 * @param pid port number [0..255]
 * @param tc traffic class value [0..3]
 * @param hoh FV hash of hash result [0..3]
 * @return u8 host interface datapath ID, PMGR_HIF_DB_DP_MAX for
 *         error
 */
u8 pmgr_port_hif_map_get(u16 pid, u8 tc, u8 hoh);

/**
 * @brief port manager sysfs init
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_sysfs_init(struct kobject *parent);

/**
 * @brief port manager sysfs cleanup
 */
void pmgr_sysfs_clean(void);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief port manager debug init
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_dbg_init(struct dentry *parent);

/**
 * @brief port manager debug cleanup
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_dbg_clean(void);

/**
 * @brief port debug init
 * @param pid port number
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_port_dbg_init(u16 pid);

/**
 * @brief port debug cleanup
 * @param pid port number
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_port_dbg_clean(u16 pid);

/**
 * @brief hostif dp debug init
 * @param dp dp number
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_dp_dbg_init(u8 dp);

/**
 * @brief hostif dp debug cleanup
 * @param dp dp number
 * @return s32 0 for success, non-zero otherwise
 */
s32 pmgr_dp_dbg_clean(u8 dp);
#else /* !CONFIG_DEBUG_FS */
static inline s32 pmgr_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 pmgr_dbg_clean(void)
{
	return 0;
}

static inline s32 pmgr_port_dbg_init(u16 pid)
{
	return 0;
}

static inline s32 pmgr_port_dbg_clean(u16 pid)
{
	return 0;
}

static inline s32 pmgr_dp_dbg_init(u8 dp)
{
	return 0;
}

static inline s32 pmgr_dp_dbg_clean(u8 dp)
{
	return 0;
}
#endif /* CONFIG_DEBUG_FS */
#endif /* __PP_PORT_MGR_INTERNAL_H__ */
