/*
 * Description: Packet Processor Checker Internal
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#ifndef __CHECKER_INTERNAL_H__
#define __CHECKER_INTERNAL_H__
#include <linux/types.h>
#include <linux/pp_api.h>
#include "pp_si.h"
#include "checker_regs.h"
#include "checker.h"
#include "counter_cache_regs.h"

extern u64 chk_base_addr;
extern u64 chk_ram_base_addr;
extern u64 chk_cache_base_addr;
#define CHK_BASE_ADDR chk_base_addr
#define CHK_RAM_BASE_ADDR chk_ram_base_addr
#define CNTR_CACHE_BASE_ADDR chk_cache_base_addr

/**
 * @define define the offset to an exception SI base address
 */
#define CHK_EXCEPTION_SI_OFF(id) ((id) << 5)

/**
 * @define Sync Queue - session id
 */
#define CHK_SYNCQ_SESSION_RAM_BASE                 (CHK_RAM_BASE_ADDR)
#define CHK_SYNCQ_SESSION(ent)                    \
	(CHK_SYNCQ_SESSION_RAM_BASE + ((ent) << 2))
#define CHK_SYNCQ_SESSION_ID_MSK                   GENMASK(23, 0)
#define CHK_SYNCQ_SESSION_VALID_MSK                BIT(31)

/**
 * @define Sync Queue - queue id
 */
#define CHK_SYNCQ_QUEUE_RAM_BASE                   (CHK_RAM_BASE_ADDR + 0x10000)
#define CHK_SYNCQ_QUEUE(ent)                    \
	(CHK_SYNCQ_QUEUE_RAM_BASE + ((ent) << 2))
#define CHK_SYNCQ_QUEUE_ID_MSK                     GENMASK(11, 0)
#define CHK_SYNCQ_QUEUE_STATE_MSK                  BIT(12)

/**
 * @define Exception Mapping
 */
#define CHK_ALT_SI_MAP_RAM_BASE                    (CHK_RAM_BASE_ADDR + 0x20000)
#define CHK_ALT_SI_MAP_RAM(port, tc)                           \
	(CHK_ALT_SI_MAP_RAM_BASE + ((((port) << 2) | (tc)) << 2))

/**
 * @define MTU
 */
#define CHK_MTU_RAM_BASE                           (CHK_RAM_BASE_ADDR + 0x30000)
#define CHK_MTU_RAM(port)                \
	(CHK_MTU_RAM_BASE + ((port) << 2))

/**
 * @define Exception SI
 */
#define CHK_ALT_SI_RAM_BASE                        (CHK_RAM_BASE_ADDR + 0x40000)
#define CHK_ALT_SI_RAM(exception_session)                \
	(CHK_ALT_SI_RAM_BASE + CHK_EXCEPTION_SI_OFF(exception_session))

/**
 * @define Exception session counter read
 *      [31-0] [RW]: Indirect read
 */
#define CHK_ALT_SI_CNTS_RAM_BASE                   (CHK_RAM_BASE_ADDR + 0x50000)
#define CHK_ALT_SI_CNTS_RAM(exception_session)                \
	(CHK_ALT_SI_CNTS_RAM_BASE + ((exception_session) << 2))

#define PP_CHK_ALT_SI_CNTS_BYTE_CNT_L_WRD0            GENMASK(31, 0)
#define PP_CHK_ALT_SI_CNTS_BYTE_CNT_H_WRD1            GENMASK(19, 0)
#define PP_CHK_ALT_SI_CNTS_PKT_CNT_L_WRD1             GENMASK(31, 22)
#define PP_CHK_ALT_SI_CNTS_PKT_CNT_H_WRD2             GENMASK(31, 0)

/**
 * @define TBM
 */
#define CHK_TBM_RAM_BASE                           (CHK_RAM_BASE_ADDR + 0x60000)

/**
 * @define GCNTS_SRAM0
 */
#define CHK_GCNTS0_RAM_BASE                        (CHK_RAM_BASE_ADDR + 0x70000)

/**
 * @define GCNTS_SRAM1
 */
#define CHK_GCNTS1_RAM_BASE                        (CHK_RAM_BASE_ADDR + 0x74000)

/**
 * @define Checker mailbox command kick register
 * @brief any write to this register will start the command handling
 *        by the checker
 */
#define CHK_MBOX_KICK_REG                          (CHK_RAM_BASE_ADDR + 0x80000)

enum chk_mbox_cmd {
	/*! command for updating session's DSI */
	CHK_MBOX_CMD_NORMAL,
	/*! command for reading session's DSI */
	CHK_MBOX_CMD_READ,
	/*! command for getting staled/inactive sessions */
	CHK_MBOX_CMD_CRAWLER,
	/*! command for updating session or exception session counters */
	CHK_MBOX_CMD_COUNTERS,
};

/**
 * @brief Checker mailbox commands fields encoding
 */
#define CHK_CMD_TYPE_MSK                           GENMASK(1, 0)
/* Normal command */
#define CHK_CMD_NORMAL_CNT_CLR_MSK                 BIT(10)
#define CHK_CMD_NORMAL_DQ_WR_MSK                   BIT(11)
#define CHK_CMD_NORMAL_DST_Q_MSK                   GENMASK(23, 12)
/* Crawler command */
#define CHK_CMD_CRWLR_ITER_SZ_MSK                  GENMASK(4, 2)
/* Counters command */
#define CHK_CMD_CNTS_EXP_SESS_MSK                  BIT(3)
#define CHK_CMD_CNTS_PKTS_MSK                      GENMASK(7, 4)
#define CHK_CMD_CNTS_BYTES_MSK                     GENMASK(31, 8)
/* 0 to decrement, 1 to increment */
#define CHK_CMD_CNTS_INC_MSK                       BIT(2)
/* session group counters command */
#define CHK_CMD_SGC_INC_MSK                        BIT(0)
#define CHK_CMD_SGC_RAM_MSK                        BIT(1)
#define CHK_CMD_SGC_IDX_MSK                        GENMASK(13, 2)
#define CHK_CMD_SGC_PKTS_MSK                       GENMASK(15, 14)
#define CHK_CMD_SGC_BYTES_MSK                      GENMASK(31, 16)

/**
 * @define Crawler status register bit fields
 * @see PP_CHK_MBX_CRWLR_STATUS_REG
 */
#define CHK_CRWLR_STATUS_IDLE                      BIT(0)
#define CHK_CRWLR_STATUS_WAIT                      BIT(1)
#define CHK_CRWLR_STATUS_WAIT_FINAL                BIT(2)
#define CHK_CRWLR_STATUS_DSI_SYNC_LOSS             BIT(3)
#define CHK_CRWLR_STATUS_CMD_CNTR                  GENMASK(31, 16)

/**
 * @brief SGC HW entry size in number of 32bit words
 */
#define CHK_SGC_NUM_WORDS                          (3)

/**
 * @define DSI DDR Mapping
 */
#define CHK_DSI_OFFSET_GET(session_id)   \
	((session_id) << PP_DSI_WORDS_CNT)

/**
 * @struct hw_sgc
 * @brief Defines the SGC hw structure which is basically 3
 *        32bits words
 */
struct hw_sgc {
	u32 buf[CHK_SGC_NUM_WORDS];
};

/**
 * @brief TBM database info
 *        This driver only implements dual TBM support as the
 *        HW dictates, due to that we always allocate pairs of
 *        TBMs.
 *        e.g. (0,256), (1,257), (2,258) ...
 */
struct dual_tbm_db_info {
	/*! list node to use with free or busy list */
	struct list_head node;
	/*! specify if the tbm is allocated or not */
	bool allocated;
	/*! number of associated sessions, tbm cannot be freed if this
	 *  counter is not 0
	 */
	u32 n_sessions;
	/* the committed tbm index, this index is returned to the user
	 * as the dual tbm index and should be used in the si
	 */
	u16 ctbm_idx;
	/* the original committed rate that the TBM was configured to, this
	 * will be used to restore the original rate upon clock change
	 */
	u32 org_cir;
	/* the peak tbm index */
	u16 ptbm_idx;
	/* the original peak rate that the TBM was configured to, this
	 * will be used to restore the original rate upon clock change
	 */
	u32 org_pir;
};

/**
 * @brief Encode packets and bytes counters into an SGC buffer
 * @param hw buffer to encode to
 * @param pkts packets counter
 * @param bytes bytes counter
 */
void sgc_encode(struct hw_sgc *hw, u64 pkts, u64 bytes);

/**
 * @brief Decode an SGC buffer to packets and bytes counters
 * @param hw buffer to decode from
 * @param pkts packets counter pointer
 * @param bytes bytes counter pointer
 */
void sgc_decode(struct hw_sgc *hw, u64 *pkts, u64 *bytes);

/**
 * @brief Initialize all checker session group counters stuff
 * @return s32 0 on success, error code otherwise
 */
s32 chk_sgc_init(void);

/**
 * @brief Checker session group counters exit function, clean all
 *        related resources
 */
void chk_sgc_exit(void);

/**
 * @brief Initialize all checker token bucket meters stuff
 * @param init_args checker's init arguments
 * @return s32 0 on success, error code otherwise
 */
s32 chk_tbm_init(struct pp_chk_init_param *init_args);

/**
 * @brief Checker token bucket meters exit function, clean all
 *        related resources
 */
void chk_tbm_exit(void);

/**
 * @brief Get exceptions stats
 * @param stats where to save the stats
 * @param num_ports number of stats entries
 * @return s32
 */
s32 chk_exceptions_stats_get(void *stats, u32 num_stats, void *data);

/**
 * @brief Calculates the difference between exceptions stats
 * @param pre pre stats
 * @param num_pre number of pre stats
 * @param post post stats
 * @param num_post number of post stats
 * @param delta stats to save the delta
 * @param num_delta number of delta stats
 * @note all stats array MUST be with same size
 * @return s32
 */
s32 chk_exceptions_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			      void *delta, u32 num_delta, void *data);

/**
 * @brief Prints exceptions stats into a buffer
 * @param buf the buffer to print into
 * @param sz buffer size
 * @param n pointer to update number of written bytes
 * @param stats the stats
 * @param num_stats number of stats
 */
s32 chk_exceptions_stats_show(char *buf, size_t sz, size_t *n, void *stats,
			      u32 num_stats, void *data);

/**
 * @brief port manager sysfs init
 * @return s32 0 for success, non-zero otherwise
 */
s32 chk_sysfs_init(struct pp_chk_init_param *init_param);

/**
 * @brief port manager sysfs cleanup
 */
void chk_sysfs_clean(void);

/**
 * @brief Update TBMs data with new clock frequency
 * @param clk_MHz the new clock frequency in MHz
 * @return s32 0 on success, error code in case the TBM cannot work
 *             with the new clock
 */
s32 chk_tbm_hw_clock_update(u32 clk_MHz);

#ifdef CONFIG_DEBUG_FS
/**
 * @brief Checker debug init
 * @return s32 0 on success, error code otherwise
 */
s32 chk_dbg_init(struct pp_chk_init_param *init_param);

/**
 * @brief Checker debug cleanup
 */
void chk_dbg_clean(void);

/**
 * @brief Get dual TBM database info
 * @param idx dual tbm index
 * @param dtbm db info buffer
 * @return s32
 */
s32 chk_dual_tbm_db_info_get(u16 idx, struct dual_tbm_db_info *dtbm);

/**
 * @brief Get counters cache bypass status
 * @note this cache is used for caching the dynamic session information (DSI)
 * @return bool true is the cache is in bypass mode, false otherwise
 */
bool chk_cntr_cache_bypass_get(void);

/**
 * @brief Set counters cache byepass
 * @note this cache is used for caching the dynamic session information (DSI)
 * @param bypass
 */
void chk_cntr_cache_bypass_set(bool bypass);

/**
 * @brief Get counters cache counters
 * @note this cache is used for caching the dynamic session information (DSI)
 * @param read_trans number of read transactions
 * @param read_hits number of cache hits
 * @return s32 0 on success, error code otherwise
 */
s32 chk_cntr_cache_stats_get(u64 *read_trans, u64 *read_hits);

#else /* !CONFIG_DEBUG_FS */
static inline s32 chk_dbg_init(struct pp_chk_init_param *init_param)
{
	return 0;
}

static inline void chk_dbg_clean(void)
{
}
#endif /* CONFIG_DEBUG_FS */
#endif /* __CHECKER_INTERNAL_H__ */

