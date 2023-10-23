/*
 * Description: PP micro-comtroller definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#ifndef __PP_UC_H__
#define __PP_UC_H__

#include <linux/types.h>
#include <linux/debugfs.h>      /* struct dentry */
#include <linux/pp_api.h>
#include "uc_common.h"

/**
 * @define used as parameter to the uc API for egress uc
 *         operations
 */
#define UC_IS_EGR        true

/**
 * @define used as parameter to the uc API for ingress uc
 *         operations
 */
#define UC_IS_ING        !UC_IS_EGR

/**
 * @define maximum uc cpus in cluster
 */
#define UC_CPUS_MAX      (4)

/**
 * @struct uc_cpu_params
 */
struct uc_cpu_params {
	u32 max_cpu;               /*! number of cpus     */
	u32 cpu_prof[UC_CPUS_MAX]; /*! cpus profile array */
};

/**
 * @struct uc_egr_init_params
 */
struct uc_egr_init_params {
	bool   valid;
	struct uc_cpu_params cpus; /*! cpus information            */
	u64    uc_base;            /*! uc base address             */
	phys_addr_t chk_base;      /*! chk base address            */
	phys_addr_t chk_ctrs_cache_base_addr; /*! chk counter cache base addr */
	phys_addr_t rxdma_base; /*! rxdma base address          */
	phys_addr_t bm_base;       /*! bm base address             */
	phys_addr_t qm_base;       /*! qm base address             */
	phys_addr_t wred_base;     /*! wred base address           */
	phys_addr_t client_base;   /*! wred client base address    */
	phys_addr_t txm_cred_base; /*! TX-MGR credit base address  */
	phys_addr_t cls_base;      /*! classifier base address     */
};

/**
 * @struct uc_ing_init_params
 */
struct uc_ing_init_params {
	bool   valid;
	struct uc_cpu_params cpus; /*! cpus information   */
	u64    uc_base;            /*! uc base address    */
};

struct uc_init_params {
	struct uc_ing_init_params ing;
	struct uc_egr_init_params egr;
	struct dentry *dbgfs;
};

#ifdef CONFIG_PPV4_LGM
/**
 * @define maximum of supported turbodox sessions
 */
#define UC_MAX_TDOX_SESSIONS  (512)

/**
 * @enum tdox uc states
 */
enum tdox_uc_state {
	/*! Newly created */
	TDOX_NEW,
	/*! Session tagged as qualified but not flowing through uc */
	TDOX_QUALIFIED_UC_INACTIVE,
	/*! Session tagged as qualified and flowing through uc */
	TDOX_QUALIFIED_UC_ACTIVE,
	/*! Session tagged as non qualified but not flowing through uc */
	TDOX_NONQUALIFIED_UC_INACTIVE,
	/*! Session tagged as non qualified and flowing through uc */
	TDOX_NONQUALIFIED_UC_ACTIVE,
};

/**
 * @enum uc_mbox_set
 * @brief mailbox set, 2 sets of mailbox registers are supported
 */
enum uc_mbox_set {
	/*! mailbox registers set 0 */
	UC_EGR_MBOX0,
	/*! TODO: not supported in driver */
	UC_EGR_MBOX1,
};

/**
 * @brief get the multicast packet counters per uc CPU
 * @param cid cpu index
 * @param stats multicast packet statistics
 * @return s32 0 on success, error code otherwise
 */
s32 uc_mcast_cpu_stats_get(u32 cid, struct mcast_stats *stats);

/**
 * @brief get the ipsec packet counters per uc CPU
 * @param cid cpu index
 * @param stats ipsec packet statistics
 * @return s32 0 on success, error code otherwise
 */
s32 uc_ipsec_cpu_stats_get(u32 cid, struct ipsec_stats *stats);

/**
 * @brief get the reassembly packet counters per uc CPU
 * @param cid cpu index
 * @param stats reassembly statistics
 * @return s32 0 on success, error code otherwise
 */
s32 uc_reass_cpu_stats_get(u32 cid, struct reassembly_stats *stats);

/**
 * @brief get the fragmentation packet counters per uc CPU
 * @param cid cpu index
 * @param stats fragmentation statistics
 * @return s32 0 on success, error code otherwise
 */
s32 uc_frag_cpu_stats_get(u32 cid, struct frag_stats *stats);

/**
 * @brief Get the total multicast statistics
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 uc_mcast_stats_get(struct mcast_stats *stats);

/**
 * @brief Get the total ipsec statistics
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 uc_ipsec_stats_get(struct ipsec_stats *stats);

/**
 * @brief Get the total reassembly statistics
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 uc_reass_stats_get(struct reassembly_stats *stats);

/**
 * @brief Get the total fragmentation statistics
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 uc_frag_stats_get(struct frag_stats *stats);

/**
 * @brief send a mailbox command to the egress(only) uc
 * @param type command type
 * @param param command parameter (optional)
 * @param in_buf buffer to send (optional)
 * @param in_len in_buf size in bytes
 * @param out_buf buffer to send (optional)
 * @param out_len out_buf size in bytes
 * @return s32 0 on success, error code otherwise
 */
s32 uc_egr_mbox_cmd_send(enum uc_mbox_cmd_type type, u32 param,
			 const void *in_buf, u16 in_len,
			 void *out_buf, u16 out_len);

/**
 * @brief allocate a DDR buffer for mailbox command
 * @param phys allocated buffer physical address
 * @param sz buffer size
 * @return void* allocated buffer cpu address, NULL if failed
 */
void *uc_egr_mbox_buff_alloc(dma_addr_t *phys, size_t sz);

/**
 * @brief free the mailbox command DDR buffer
 * @param virt buffer cpu address
 * @param phys buffer physical address
 * @param sz buffer size
 */
void uc_egr_mbox_buff_free(void *virt, dma_addr_t phys, size_t sz);

/**
 * @brief set uc network function parameters
 * @param nf network function type
 * @param pid port id (phy)
 * @param qos_port qos port id (phy)
 * @param tx_queue nf tx queue (phy)
 * @param dflt_hif default host interface to use for diverting
 *        packets to the host
 * @return s32 0 on success, error code otherwise
 */
s32 uc_nf_set(enum pp_nf_type nf, u16 pid, u16 qos_port, u16 tx_queue,
	      u16 dflt_hif);

/**
 * @brief Read tdox state
 * @param tdox_id tdox record id
 * @param state record state
 * @return s32 0 on success, error code otherwise
 */
s32 uc_tdox_state_get(u16 tdox_id, enum tdox_uc_state *state);

/**
 * @brief check if cpu is active
 * @param uc_is_egr select the uc cluster
 * @param cid cpu index
 * @return bool true if valid, fasle otherwise
 */
bool uc_is_cpu_active(bool uc_is_egr, u32 cid);

/**
 * @brief check if cluster is valid (at least 1 cpu is valid)
 * @param uc_is_egr select the uc cluster
 * @return bool true if valid, fasle otherwise
 */
bool uc_is_cluster_valid(bool uc_is_egr);

/**
 * @brief get ccu GP register value
 * @param uc_is_egr select the uc cluster
 * @param idx gp register index
 * @param gpreg gp register value
 * @return s32 0 on success, error code otherwise
 */
s32 uc_ccu_gpreg_get(bool uc_is_egr, u32 idx, u32 *gpreg);

/**
 * @brief set ccu GP register value
 * @param uc_is_egr select the uc cluster
 * @param idx gp register index
 * @param gpreg gp register value
 * @return s32 0 on success, error code otherwise
 */
s32 uc_ccu_gpreg_set(bool uc_is_egr, u32 idx, u32 gpreg_val);

/**
 * @brief Resets the uc statistics
 */
void uc_stats_reset(void);

/**
 * @brief initialized the uc module
 * @param init_params initial parameters
 * @return s32 0 on success, error code otherwise
 */
s32 uc_init(struct uc_init_params *init_params);

/**
 * @brief exit the uc module
 */
void uc_exit(void);
#else
static inline s32 uc_init(struct uc_init_params *init_params)
{
	if (init_params->egr.valid || init_params->ing.valid)
		return -EINVAL;
	else
		return 0;
}

static inline void uc_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __PP_UC_H__ */
