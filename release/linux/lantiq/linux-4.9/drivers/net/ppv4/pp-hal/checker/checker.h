/*
 * Description: Packet Processor Checker
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __CHECKER_H__
#define __CHECKER_H__

#include <linux/types.h>
#include <linux/pp_api.h>
#include <linux/debugfs.h>
#include "pp_common.h"
#include "pp_si.h"

#define CHK_NUM_EXCEPTION_SESSIONS      (64)
#define CHK_DEFAULT_SESSION_ID          (63)
#define CHK_HOH_MAX                     (4)
#define CHK_EX_SESS_CLASS_MAX           (4096)
#define CHK_EXCEPTION_RECIPE            (0xFF)
#define CHK_EXCEPTION_TMP_UD_SZ         (32)

/**
 * @define Minimum crawler iteration size
 * @brief Minimum number of sessions to scan
 */
#define CHK_CRWLR_MIN_ITR_SIZE          (128U)

/**
 * @define Maximum crawler iteration size
 * @brief Maximum number of sessions to scan
 */
#define CHK_CRWLR_MAX_ITR_SIZE          (8192U)

/**
 * @brief Total number of TBMs we have in the HW.
 */
#define CHK_TBM_NUM                     (512U)

/**
 * @brief Total number of dual TBMs the HW can support
 */
#define CHK_DUAL_TBM_NUM                (CHK_TBM_NUM / 2)

/**
 * @define Shortcut macro to check if exception id is
 *        valid
 * @param id
 */
#define PP_IS_EXCEPTION_ID_VALID(id) \
	(0 <= (id) && (id) < CHK_NUM_EXCEPTION_SESSIONS)

/**
 * @define Shortcut macro to check if hoh number is valid
 * @param hoh
 */
#define PP_IS_HOH_VALID(hoh) \
	(0 <= (hoh) && (hoh) < CHK_HOH_MAX)

/* Group counters RAM 0 groups, below values describes the HW, DO NOT CHANGE */
#define SGC_GRP0_CNTRS_NUM                         (1024)
#define SGC_GRP3_CNTRS_NUM                         (1024)
#define SGC_GRP2_CNTRS_NUM                         (256)
#define SGC_GRP5_CNTRS_NUM                         (256)
/* Group counters RAM 1 groups, below values describes the HW, DO NOT CHANGE */
#define SGC_GRP1_CNTRS_NUM                         (64)
#define SGC_GRP4_CNTRS_NUM                         (64)
#define SGC_GRP6_CNTRS_NUM                         (64)
#define SGC_GRP7_CNTRS_NUM                         (8)

/**
 * @define Session's default checker flags
 */
#define SESS_CHCK_DFLT_FLAGS               \
	(BIT(SI_CHCK_FLAG_DVRT_L3_OPTS)  | \
	 BIT(SI_CHCK_FLAG_DVRT_TTL_EXP)  | \
	 BIT(SI_CHCK_FLAG_DVRT_TCP_CTRL) | \
	 BIT(SI_CHCK_FLAG_UPDT_SESS_CNT) | \
	 BIT(SI_CHCK_FLAG_DYN_SESS))

/**
 * @struct Checker module init parameters
 * @valid params valid
 * @chk_base checker base address
 * @chk_ram_base checker ram base address
 * @chk_cache_base checker cache base address
 * @num_sessions number of supported sessions
 * @num_syncqs number of supported syncqs in HW
 * @hw_clk HW Clock in MHz
 * @dbgfs debugfs parent directory
 * @dbgfs driver root debugfs directory
 * @sysfs sysfs parent directory
 */
struct pp_chk_init_param {
	bool valid;
	u64 chk_base;
	u64 chk_ram_base;
	u64 chk_cache_base;
	u32 num_sessions;
	u32 num_syncqs;
	u32 hw_clk;
	struct dentry  *dbgfs;
	struct dentry  *root_dbgfs;
	struct kobject *sysfs;
};

#ifdef CONFIG_PPV4_LGM
/**
 * @struct PP Exception session config
 * @id exception session id
 * @port egress port
 * @queue egress queue (physical)
 * @color color to set on descriptor
 * @sgc optional, sgc id's
 * @tbm optional, tbm id's
 * @flags checker flags
 */
struct exception_session_cfg {
	u8 id;
	u16 port;
	u16 queue;
	u8 color;
	u16 *sgc;
	u16 *tbm;
	u16 flags;
};

struct chk_stats {
	u32 exception_pkt_cnt;
	u32 dflt_session_pkt_cnt;
	u32 exception_byte_cnt;
	u32 dflt_session_byte_cnt;
	u32 pkts_rcvd_from_cls;
	u32 accelerated_pkts;
};

/**
 * @brief Crawler hw configurations
 */
struct crawler_cfg {
	/*! Timer init val */
	u32 timer_init_val;
	/*! mailbox Crawler Timer pace. Dec' value for Crawler internal Timer */
	u16 timer_pace;
	/*! command counter */
	u16 command_counter;
	/*! mailbox Crawler DDR Mode */
	bool ddr_mode;
	/*! mailbox Crawler Reset */
	bool reset;
	/*! idle status */
	bool idle_status;
	/*! wait status */
	bool wait_status;
	/*! wait for final stale status */
	bool wait_for_final_status;
	/*! crawler DSI Sync Loss Error (sticky) */
	bool dsi_sync_loss_error;
};

/**
 * @brief TBM hw configurations
 */
struct tbm_cfg {
	/*! specify if the TBM is coupled to the adjacent TBM */
	bool coupling;
	/*! specify if the TBM is enabled or not */
	bool enable;
	/*! number of credits accumulated so far */
	u32 credits;
	/*! last timestamp */
	u32 timestamp;
	/*! maximum accumulated credits, credits cannot exceed this value */
	u32 max_burst;
	/*! exponent */
	u8  exponent;
	/*! mantissa */
	u16 mantissa;
	/*! color aware/blind mode */
	enum pp_tbm_mode mode;
	/*! packet length type for calculations */
	enum pp_tbm_len_type len_type;
};

/**
 * @brief Get the exception session statistics
 * @param id exception session id
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_stats_get(u8 id, struct pp_stats *stats);

/**
 * @brief Get the packet statistics
 * @param stats
 * @return s32 0 on success, error code otherwise
 */
s32 chk_stats_get(struct chk_stats *stats);

/**
 * @brief Reset checker statistics
 */
void chk_stats_reset(void);

/**
 * @brief Set network function queue
 * @param nf id of network function
 * @param queue physical queue id
 * @return s32 0 on success, error code otherwise
 */
s32 chk_nf_set(u16 queue, enum pp_nf_type nf);

/**
 * @brief Set port MTU
 * @param port egress port
 * @param mtu
 * @return s32 0 on success, error code otherwise
 */
s32 chk_mtu_set(u16 port, u32 mtu);

/**
 * @brief Set new exception session
 * @param cfg session cfg
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_session_set(struct exception_session_cfg *cfg);

/**
 * @brief Get exception session
 * @param cfg session cfg
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_session_get(struct exception_session_cfg *cfg);

/**
 * @brief Get exception session si
 * @param id session id
 * @param alt_si si returned structure
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_session_si_get(u32 id, struct pp_si *alt_si);

/**
 * @brief Get exception mapping
 * @param port ingress port
 * @param tc ingress tc
 * @param hoh hash of hash
 * @param id exception id
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_session_map_set(u16 port, u8 tc, u8 hoh, u8 id);

/**
 * @brief Get exception mapping
 * @param port ingress port
 * @param tc ingress tc
 * @param hoh hash of hash
 * @param id returned exception id
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_session_map_get(u16 port, u8 tc, u8 hoh, u8 *id);

/**
 * @brief Set all exception mappings for port
 * @param port ingress port
 * @param id array of exception id's
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_session_port_map_set(u16 port,
				       u8 id[PP_MAX_TC][CHK_HOH_MAX]);

/**
 * @brief Get all exception mappings for port
 * @param port ingress port
 * @param id array of exception id's
 * @return s32 0 on success, error code otherwise
 */
s32 chk_exception_session_port_map_get(u16 port,
				       u8 id[PP_MAX_TC][CHK_HOH_MAX]);

/**
 * @brief Get session DSI info
 * @param id session id
 * @param dsi dsi info buffer
 * @return s32 0 on success, error code otherwise
 */
s32 chk_session_dsi_get(u32 id, struct pp_hw_dsi *dsi);

/**
 * @brief Update session DSI destination queue
 * @param id session id
 * @param q queue physical id
 * @return s32 0 on success, error code otherwise
 */
s32 chk_session_dsi_q_update(u32 id, u16 q);

/**
 * @brief Reset session counters
 * @note not supported for exception
 * @param id session id
 * @return s32 0 on success, error code otherwise
 */
s32 chk_session_stats_reset(u32 id);

/**
 * @brief Modify session/exception session counters
 * @param id session id
 * @param inc specify whether to increment or decrement the counters,
 *             use 'false' to decrement and 'true' to increment
 * @param pkts number of packets to dec/inc
 * @param bytes number of bytes to dec/inc
 * @param excp specify if the session is exception or not, use 'true'
 *             for exception sessions, 'false' otherwise
 * @return s32 return 0 on success, error code otherwise
 */
s32 chk_session_stats_mod(u32 id, bool inc, u8 pkts, u32 bytes, bool excp);

/**
 * @brief Test if the specified SGC group id is valid
 * @param grp the id to test
 * @return bool true if the id is valid, false otherwise
 */
bool chk_sgc_is_group_valid(u8 grp);

/**
 * @brief Reserve an entire group of SGCs.
 *        The SGCs from the specified group can ONLY
 *        be allocated/freed/modified with the returned
 *        owner id
 * @param grp group id
 * @param owner allocated owner id to use later for
 *              allocated/freed/modified SGCs from the specifeid group
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_group_reserve(u8 grp, u32 *owner);

/**
 * @brief Unreserve SGC group, ONLY the owner of the group can do it
 * @param grp the group
 * @param owner the owner
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_group_unreserve(u8 grp, u32 owner);

/**
 * @brief Get SGC group's info
 * @param grp the group
 * @param n_cntrs number of counters in the group
 * @param owner the owner
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_group_info_get(u8 grp, u32 *n_cntrs, u32 *owner);

/**
 * @brief Get session group counter packets and bytes counters
 * @param grp counter group
 * @param cntr counter index within the group
 * @param pkts buffer to get packets counters value
 * @param bytes buffer to get bytes counters value
 * @param number of sessions that use the specified counter
 * @return s32 0 on success, error value otherwise
 */
s32 chk_sgc_get(u8 grp, u16 cntr, u64 *pkts, u64 *bytes, u32 *n_sessions);

/**
 * @brief Modify session group counter
 * @param owner the group owner identifier allocated by pp_sgc_group_res
 * @param grp counter group index
 * @param cntr counter index within the group
 * @param act action to take
 * @param pkts number of packets to modify, valid only for increment
 *             or decrement
 * @param bytes number of bytes to modify, , valid only for increment
 *             or decrement
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_mod(u32 owner, u8 grp, u16 cntr, enum pp_stats_op act, u32 pkts,
		u32 bytes);

/**
 * @brief Allocate new SGC counter
 * @param owner the group owner identifier allocated by pp_sgc_group_res
 * @param grp group index
 * @param cntr pointer to return counter index on success
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_alloc(u32 owner, u8 grp, u16 *cntr);

/**
 * @brief Free SGC counter
 * @note Counter can only be freed ONLY if he isn't used by an active
 *       sessions
 * @param owner the group owner identifier allocated by pp_sgc_group_res
 * @param grp group index
 * @param cntr counter index
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_free(u32 owner, u8 grp, u16 cntr);

/**
 * @brief Attach session to an active counter
 * @note The counter MUST be allocated via chk_sgc_alloc first
 * @param grp group index
 * @param cntr counter index
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_session_attach(u8 grp, u16 cntr);

/**
 * @brief Detach session from an active counter
 * @note The counter MUST be allocated via chk_sgc_alloc and has
 *       sessions attached
 * @param grp group index
 * @param cntr counter index
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_session_detach(u8 grp, u16 cntr);

/**
 * @brief Get dual tbm configuration
 * @param idx tbm index
 * @param tbm configuration buffer
 * @return s32 0 on success, error value otherwise
 */
s32 chk_dual_tbm_get(u16 idx, struct pp_dual_tbm *tbm);

/**
 * @brief Set dual tbm configuration
 * @param idx tbm index
 * @param tbm the configuration to set
 * @return s32
 */
s32 chk_dual_tbm_set(u16 idx, struct pp_dual_tbm *tbm);

/**
 * @brief Allocate new dual tbm to be used by PP sessions
 * @note the index returned cannot be used in the SI,
 *       use chk_dual_tbm_to_si_idx() to get the right one
 * @param idx tbm index buffer
 * @return s32 0 on successful allocation, errorcode otherwise
 */
s32 chk_dual_tbm_alloc(u16 *idx);

/**
 * @brief Free dual tbm which was allocated by chk_tbm_alloc
 * @param idx tbm index to free
 * @return s32 0 on successful freeing, error code otherwise
 */
s32 chk_dual_tbm_free(u16 idx);

/**
 * @brief Attach session to an active dual tbm
 * @note The tbm MUST be allocated via chk_tbm_alloc first
 * @param tbm tbm index
 * @return s32 0 on success, error code otherwise
 */
s32 chk_dual_tbm_session_attach(u16 tbm);

/**
 * @brief Detach session from an active dual tbm
 * @note The tbm MUST be allocated via chk_sgc_alloc and has
 *       sessions attached
 * @param tbm tbm index
 * @return s32 0 on success, error code otherwise
 */
s32 chk_dual_tbm_session_detach(u16 tbm);

/**
 * @brief Get TBM mode description string
 * @param m mode index
 * @return const char* const mode description
 */
const char *const chk_tbm_mode_str(enum pp_tbm_mode m);

/**
 * @brief Get TBM packet length type description string
 * @param t length type index
 * @return const char* const length type description
 */
const char *const chk_tbm_len_type_str(enum pp_tbm_len_type t);

/**
 * @brief Test whether the specified dual tbm is allocated.
 * @param idx tbm index to check
 * @return bool true if the tbm is allocated, false otherwise
 */
bool chk_dual_tbm_is_allocated(u16 idx);

/**
 * @brief Get dual TBM number of attached sessions
 * @param idx tbm index
 * @param num_sessions number of sessions buffers
 * @return s32 0 on success, error code otherwise
 */
s32 chk_dual_tbm_num_sessions_get(u16 idx, u32 *num_sessions);

/**
 * @brief Enable the sync queue entry
 * @param id sync queue entry id
 * @param session session id
 * @param queue queue id
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sq_entry_enable(u8 id, u32 session, u16 queue);

/**
 * @brief Disable the sync queue entry
 * @param id sync queue entry id
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sq_entry_disable(u8 id);

/**
 * @brief Calculate scan time for n_sessions based on crawler's
 *        current configuration and hw clock frequency
 * @param n_sessions number of sessions, the API sets this pointer
 *                   with the real number of sessions that will be
 *                   scanned
 * @param time pointer to return scan time
 * @return u32 iteration time is microsecnods on success, error code
 *         otherwise
 */
s32 chk_scan_time_get(u32 *n_sessions, u32 *time);

/**
 * @brief Triggers a sessions scan to find stale sessions by the
 *        crawler HW
 * @note the scan is preform on a power of 2 number of sessions only,
 *       with minimum of CHK_CRWLR_MIN_ITR_SIZE and maximum of
 *       CHK_CRWLR_MAX_ITR_SIZE
 * @note the user is resposible for syncing crawler requests
 * @param base_id session id to start the scan from
 * @param n_sessions number of sessions to scan, this will be rounded
 *                   up to the next power of 2
 * @param phy_map bitmap for getting staled sessions indexes phyisical
 *                address
 * @sz bitmap size in bytes
 * @return s32 0 on success scan triggering, error code otherwise
 */
s32 chk_crawler_start(u32 base_id, u32 n_sessions, dma_addr_t phy_map,
		      size_t sz);

/**
 * @brief Wait loop for crawler to be idle
 * @note This function implements busy loop by reading crawler status
 *       register and wait for idle bit to be set
 * @param attempts if non-null ptr is provided, the number of attempts
 *                 the register was polled till expected value was
 *                 seen is returned
 * @return s32 0 on success, -EBUSY in case crawler stuck in busy
 *         state
 */
s32 chk_crawler_idle_wait(u32 *attempts);

/**
 * @brief Get the current clock in MHz which is used by checker driver
 * @param clk_MHz clock buffer
 * @return s32 0 on success, error code otherwise
 */
s32 chk_clock_get(u32 *clk_MHz);

/**
 * @brief Set a new clock to checker driver
 *        This updates all necessary SW/HW configuration which
 *        are based on the clock frequency
 * @note This doesn't change the clock, but ONLY update the configuration
 * @param clk_MHz new clock in MHz
 * @return s32 0 on success, error code otherwise
 */
s32 chk_clock_set(u32 clk_MHz);

/**
 * @brief Set full tbm configuration to HW registers
 * @param idx tbm index
 * @param tbm configuration to set
 * @return s32 0 on success, error code otherwise
 */
s32 chk_tbm_set(u16 idx, struct tbm_cfg *tbm);

/**
 * @brief Get full tbm configuration from HW registers
 * @param idx tbm index
 * @param tbm configuration buffer
 * @return s32 0 on success, error code otherwise
 */
s32 chk_tbm_get(u16 idx, struct tbm_cfg *tbm);

/**
 * @brief Set full crawler configuration to HW registers
 * @param crawler configuration to set
 * @return s32 0 on success, error code otherwise
 */
s32 chk_crawler_set(struct crawler_cfg *crawler);

/**
 * @brief Get full crawler configuration from HW registers
 * @param crawler configuration buffer
 * @return s32 0 on success, error code otherwise
 */
s32 chk_crawler_get(struct crawler_cfg *crawler);

/**
 * @brief Get exception SI base address
 * @return dma_addr_t
 */
dma_addr_t chk_excp_si_phys_base_get(void);

/**
 * @brief Get exception SI shadow DB base address
 * @return dma_addr_t
 */
dma_addr_t chk_sh_excp_si_phys_base_get(void);

/**
 * @brief Get DSI base address
 * @return dma_addr_t
 */
dma_addr_t chk_dsi_phys_base_get(void);

/**
 * @brief Module init function
 * @param init_param initial parameters
 * @return s32 0 on success, error code otherwise
 */
s32 chk_init(struct pp_chk_init_param *init_param);

/**
 * @brief Module exit function, clean all resources
 * @return void
 */
void chk_exit(void);
#else
static inline s32 chk_init(struct pp_chk_init_param *init_param)
{
	if (init_param->valid)
		return -EINVAL;
	else
		return 0;
}

static inline void chk_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __CHECKER_H__ */
