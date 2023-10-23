/*
 * classifier.h
 * Description: Packet Processor Classifier
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#ifndef __CLASSIFIER_H__
#define __CLASSIFIER_H__
#include <linux/types.h>
#include <linux/pp_api.h>
#include <linux/debugfs.h>
#include "pp_si.h"

#define CLS_SESSION_INVALID U32_MAX

/**
 * @struct Classifier module init parameters
 * @valid params valid
 * @cls_base classifier base address
 * @num_sessions number of supported sessions by pp
 * @num_lu_threads number of lookup threads
 * @dbgfs debugfs parent directory
 */
struct pp_cls_init_param {
	bool valid;
	u64 cls_base;
	u64 cls_cache_base;
	u32 num_sessions;
	u32 num_lu_threads;
	struct dentry *dbgfs;
};

#ifdef CONFIG_PPV4_LGM
#include <linux/cuckoo_hash.h>

struct cls_lu_thrd_stats {
	u32 request;
	u32 match;
	u32 long_lu;
	u32 state;
};

struct cls_stats {
	u32 pkts_rcvd_from_parser;
	u32 pkts_rcvd_from_uc;
	u32 total_lu_requests;
	u32 total_lu_matches;
};

/**
 * @brief Get SI base address
 * @return dma_addr_t
 */
dma_addr_t cls_si_phys_base_get(void);

/**
 * @brief Add a session to hash tables
 * @param si session information
 * @param hash session hash results
 * @return s32 0 on success, error code otherwise
 */
s32 cls_session_add(void *si, struct pp_hash *hash);

/**
 * @brief Update the destination queue for a session
 * @param id session index
 * @param dst_q new dstination queue
 * @param clr_cntr if to clear session counters
 * @return s32 0 on success, error code otherwise
 */
s32 cls_session_dsi_update(u32 id, u16 dst_q, bool clr_cntr);

/**
 * @brief Delete a session from the hash tables
 * @param id session index
 * @param fv_sz FV size
 * @param hash session hash results
 * @return s32 0 on success, error code otherwise
 */
s32 cls_session_del(u32 id, u32 fv_sz, struct pp_hash *hash);

/**
 * @brief Update existing session from the hash tables
 * @param id session index
 * @param fv_sz FV size
 * @param hash session hash results
 * @param new_si New hw si to set
 * @return s32 0 on success, error code otherwise
 */
s32 cls_session_update(u32 id, u32 fv_sz, struct pp_hash *hash,
		       struct pp_hw_si *new_si);

/**
 * @brief Lookup for a session in hash tables
 * @param fv session FV
 * @param fv_sz FV size
 * @param hash session hash results
 * @param id session id, returned by the function, if session
 *           not found this value will be CLS_SESSION_INVALID
 * @return s32 0 on success, error code otherwise
 */
s32 cls_session_lookup(void *fv, u32 fv_sz, struct pp_hash *hash, u32 *id);

/**
 * @brief Lookup for a session in hash tables (verbose version)
 * @param fv session FV
 * @param fv_sz FV size
 * @param hash session hash results
 * @param id session id, returned by the function, if session
 *           not found this value will be CLS_SESSION_INVALID
 * @param tbl_idx if item found, return the table index
 * @param slot_idx if item found, return the slot index
 * @return s32 0 on success, error code otherwise
 */
s32 cls_session_lookup_verbose(void *fv,
			       u32 fv_sz,
			       struct pp_hash *hash,
			       u32 *id,
			       u8 *tbl_idx,
			       u8 *slot_idx);

/**
 * @brief Flush all sessions in the hash tables
 * @param void
 * @return s32 0 on success, error code otherwise
 */
/* TODO: s32 cls_hash_flush(void); */

/**
 * @brief Get the classifier HW version
 * @param ver version
 * @return s32 0 on success, error code otherwise
 */
s32 cls_hw_ver_get(struct pp_version *ver);

/**
 * @brief Set the FV mask, received 128 bits, each bit masked a
 *        FV byte
 * @param mask FV mask
 * @return s32 0 on success, error code otherwise
 */
s32 cls_fv_mask_set(void *mask);

/**
 * @brief Get the FV mask, returned 128 bits, each bit masked a
 *        FV byte
 * @param mask FV mask
 * @return s32 0 on success, error code otherwise
 */
s32 cls_fv_mask_get(void *mask);

/**
 * @brief Set the hash selection, select the hash function
 * @param sig select the signature hash function [0..3]
 * @param hash select the hash1 and hash2 functions [0..15]
 * @return s32 0 on success, error code otherwise
 */
s32 cls_hash_select_set(u32 sig, u32 hash);

/**
 * @brief Set the hash selection, select the hash function
 * @param sig select the signature hash function [0..3]
 * @param hash select the hash1 and hash2 functions [0..15]
 * @return s32 0 on success, error code otherwise
 */
s32 cls_hash_select_get(u32 *sig, u32 *hash);

/**
 * @brief Get last field vector constructed by the HW
 * @param fv field vector buffer
 * @return s32 0 on success, error code otherwise
 */
s32 cls_dbg_last_fv_get(void *fv);

/**
 * @brief Get last 3 hash results calculated by the HW
 * @param hash 3 hash results
 * @return s32 0 on success, error code otherwise
 */
s32 cls_dbg_hash_get(struct pp_hash *hash);

/**
 * @brief Get lookup counters per lookup thread
 * @param thrd thread index
 * @param stats thread statistics
 * @return s32 0 on success, error code otherwise
 */
s32 cls_lu_thrd_stats_get(u32 thrd, struct cls_lu_thrd_stats *stats);

/**
 * @brief Get the hash tables statistics
 * @param stats hash tables statistics
 * @return s32 0 on success, error code otherwise
 */
s32 cls_hash_stats_get(struct cuckoo_hash_stats *stats);

/**
 * @brief Get the packet statistics
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 cls_stats_get(struct cls_stats *stats);

/**
 * @brief Reset the classifier statistics
 */
void cls_stats_reset(void);

/**
 * @brief Get session SI from HW
 * @param id session id
 * @param si buffer to save session si
 * @return s32 0 on success, error code otherwise
 */
s32 cls_session_si_get(u32 id, struct pp_hw_si *si);

/**
 * @brief calc hash by fv
 * @note B step only
 * @param fv field vector
 * @param fv_sz field vector size in bytes. Up to 4 * CLS_DBG_FV_NUM_REGS
 * @param hash [OUT] calculated Hash
 * @return s32 0 on success, error code otherwise
 */
s32 cls_hash_calc(void *fv, u32 fv_sz, struct pp_hash *hash);

/**
 * @brief Module init function
 * @param init_param initial parameters
 * @return s32 0 on success, error code otherwise
 */
s32 cls_init(struct pp_cls_init_param *init_param);

/**
 * @brief Module exit function, clean all resources
 * @return void
 */
void cls_exit(void);
#else
static inline s32 cls_init(struct pp_cls_init_param *init_param)
{
	if (init_param->valid)
		return -EINVAL;
	else
		return 0;
}

static inline void cls_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __CLASSIFIER_H__ */
