/*
 * pp_misc.h
 * Description: PP misc h file
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#ifndef __PP_MISC_H__
#define __PP_MISC_H__
#include <linux/types.h>
#include <linux/init.h>
#include <linux/pp_qos_api.h>
#include <linux/debugfs.h>
#include "infra.h"
#include "port_dist.h"
#include "rpb.h"
#include "parser.h"
#include "classifier.h"
#include "checker.h"
#include "modifier.h"
#include "rx_dma.h"
#include "pp_session_mgr.h"
#include "pp_port_mgr.h"
#include "pp_buffer_mgr.h"
#include "pp_qos_utils.h"
#include "uc.h"

/**
 * @brief Misc module initial configuration parameters
 */
struct pp_misc_init_param {
	bool        valid;
	u32         hw_clk;       /*! hw_clk HW Clock in MHz */
	u32         ppv4_regs_sz; /*! size of ppv4 io registers in bytes */
	phys_addr_t ppv4_base;    /*! PP HW phyisical base addres */
	struct dentry *root_dbgfs;
	struct dentry *hal_dbgfs;
	struct kobject *sysfs;
};

/**
 * @brief Misc stats
 */
struct misc_stats {
	/*! number of skb passed through RX hook with last slow path pkt mark */
	atomic_t rx_hook_lspp_pkt_cnt;
	
	/*! number of skb passed through RX hook */
	atomic_t rx_hook_skb_cnt;

	/*! number of null skb passed through RX hook */
	atomic_t rx_hook_null_skb_cnt;

	/*! number of skb passed through TX hook */
	atomic_t tx_hook_skb_cnt;

	/*! number of null skb passed through TX hook */
	atomic_t tx_hook_null_skb_cnt;
};

#ifdef CONFIG_PPV4_LGM
#define HIGH_CLOCK 600
#define MED_CLOCK  300
#define LOW_CLOCK  150

struct pp_driver_stats {
	struct pmgr_stats    pmgr_stats;
	struct smgr_stats    smgr_stats;
	struct smgr_sq_stats syncq_stats;
	struct bmgr_stats    bmgr_stats;
	struct misc_stats    misc_stats;
	struct qos_drv_stats qos_stats;
};

struct pp_global_stats {
	struct pp_stats             port_dist_stats;
	struct rpb_stats            rpb_stats;
	struct prsr_stats           parser_stats;
	struct cls_stats            cls_stats;
	struct chk_stats            chk_stats;
	struct mod_stats            mod_stats;
	struct rx_dma_stats         rx_dma_stats;
	struct pp_qos_stats         qos_stats;
	struct mcast_stats          mcast_stats;
	struct smgr_sq_stats        syncq_stats;
	struct reassembly_stats     reass_stats;
	struct frag_stats           frag_stats;
};

/**
 * @struct pp_active_sess_stats
 * @brief pp packets and bytes statistics per session id
 */
struct pp_active_sess_stats {
	u32             id;
	struct pp_stats stats;
};

/**
 * @brief Print PP resources status into a buffer
 * @param buf the buffer to print to
 * @param sz buffer size
 * @param n pointer to return number of bytes written
 * @return s32
 */
s32 pp_resource_stats_show(char *buf, size_t sz, size_t *n);

/**
 * @brief Print PP version into a buffer
 * @param buf the buffer to print to
 * @param sz buffer size
 * @param n pointer to return number of bytes written
 * @param drv driver version
 * @param fw fw version
 * @param hw hw version
 * @return s32 0 on success, error code otherwise
 */
s32 pp_version_show(char *buf, size_t sz, size_t *n, struct pp_version *drv,
		    struct pp_version *fw, struct pp_version *hw);

/**
 * @brief exit function for the PP misc driver
 */
void pp_misc_exit(void);

/**
 * @brief Reset PP global statistics
 */
void pp_global_stats_reset(void);

/**
 * @brief Reset PP driver statistics
 */
void pp_driver_stats_reset(void);

/**
 * @brief Calculate pp global stats delta
 * @param pre
 * @param post
 * @param delta
 * @return s32 0 on success, non-zero value otherwise
 */
s32 pp_global_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			 void *delta, u32 num_delta, void *data);

/**
 * @brief Calculate pp driver stats delta
 * @param pre
 * @param post
 * @param delta
 * @return s32 0 on success, non-zero value otherwise
 */
s32 pp_driver_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			 void *delta, u32 num_delta, void *data);

/**
 * @brief Get PP global statistics, gather statistics from all
 *        PP modules
 * @param stats pointer to the pp statistics structure
 * @return s32 0 on success, non-zero value incase of getting
 *         statistics error
 */
s32 pp_global_stats_get(void *stats, u32 num_stats, void *data);

/**
 * @brief Get PP driver statistics
 * @param stats pointer to the pp statistics structure
 * @return s32 0 on success, non-zero value otherwise
 */
s32 pp_driver_stats_get(void *stats, u32 num_stats, void *data);

/**
 * @brief Get PP hal global statistics string
 * @note Its the caller responsibility free the returned buffer
 * @param buf buffer to write into
 * @param sz buf size
 * @param n where to update the number of bytes written to the buffer
 * @param stats pp statistics structure
 * @return s32 0 on success, non-zero value incase of memory
 *         allocation failure
 */
s32 pp_hal_global_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			     u32 num_stats, void *data);

/**
 * @brief Get PP global statistics string
 * @note Its the caller responsibility free the returned buffer
 * @param buf buffer to write into
 * @param sz buf size
 * @param n where to update the number of bytes written to the buffer
 * @param stats pp statistics structure
 * @return s32 0 on success, non-zero value incase of memory
 *         allocation failure
 */
s32 pp_global_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			 u32 num_stats, void *data);

/**
 * @brief Get PP driver statistics string
 * @param buf the buffer to write to
 * @param sz buffer size
 * @param n pointer to add the number of bytes written the buffer
 * @param stats the pp statistics
 * @return s32 0 on success, non-zero value otherwise
 */
s32 pp_driver_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			 u32 num_stats, void *data);

/**
 * @brief Is nf enabled
 * @return True if nf is enabled, False otherwise
 */
bool pp_misc_is_nf_en(enum pp_nf_type nf);

#endif /* CONFIG_PPV4_LGM */

/**
 * @brief init function for the PP misc driver
 * @param init_param
 * @return s32 0 on success
 */
s32 pp_misc_init(struct pp_misc_init_param *init_param);

/**
 * @brief exit function for the PP misc driver
 */
void pp_misc_exit(void);
#endif /* __PP_MISC_H__ */
