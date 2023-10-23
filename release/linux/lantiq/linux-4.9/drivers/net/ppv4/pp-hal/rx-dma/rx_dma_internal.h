/*
 * rx_dma_internal.h
 * Description: Packet Processor RX-DMA Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#ifndef __RX_DMA_INTERNAL_H__
#define __RX_DMA_INTERNAL_H__
#include <linux/types.h>
#include <linux/bitmap.h>
#include "pp_regs.h"
#include "rx_dma_regs.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[RX_DMA]:%s:%d: " fmt, __func__, __LINE__
#endif

extern u64 rx_dma_base_addr;
#define RX_DMA_BASE_ADDR rx_dma_base_addr

#define RX_DMA_FSQM_DESC_BURST           (1)
#define RX_DMA_FSQM_BUFF_THR             (1)
#define RX_DMA_FSQM_BUFF_BURST_REQ       (1)

#define RX_DMA_CFG_MAX_BURST             (16)

#define RX_DMA_PORT_CFG_HEADROOM_MAX     \
	(BIT(PP_RX_DMA_EGRESS_PORT_CFG_HEADROOM_LEN))

#define RX_DMA_PORT_CFG_TAILROOM_MAX      \
	(BIT(PP_RX_DMA_EGRESS_PORT_CFG_TAILROOM_LEN))

#define STW_RX_DMA_WORD_CNT              (3 + STW_COMMON_WORD_CNT)

/**
 * @brief Rx-dma global config
 * @ddr_align: ddr_align_enable- the DMA will deivide the
 *             transaction to burst which align to DDR line
 * @wnp_disable: WNP disable at end of the packet ; DMA will not
 *               wait for response before send descriptor to Q-mng
 * @buffer_exchange: do buffer-exchange if the policies are identical
 * @max_burst: max burst - the DMA will deivide the transaction
 *             to several burst with mac burst size
 * @max_npend: set the max nPendingTransaction (outstanding)
 *             support by the RX_DMA , can be setbetween 1-16
 */
struct rx_dma_global_cfg {
	bool ddr_align;
	bool wnp_disable;
	bool buffer_exchange;
	u16  max_burst;
	u8   max_npend;
};

/**
 * @brief STW rx dma fields enumaration
 * @STW_RX_DMA_FLD_ERR_IND
 * @STW_RX_DMA_FLD_RPB_CLID
 * @STW_RX_DMA_FLD_DST_QUEUE: path through
 * @STW_RX_DMA_FLD_USER_DEF_LEN: 16B granularity
 * @STW_RX_DMA_FLD_HDR_SZ_DIFF: twos compliment
 * @STW_RX_DMA_FLD_HDR_LEN
 * @STW_RX_DMA_FLD_NEW_POOL_POLICY
 * @STW_RX_DMA_FLD_PASS_PRE_L2: 0 - do not pass PRE L2
 *        information , 1 pass  PRE L2 information
 * @STW_RX_DMA_FLD_DROP_PKT:
 *        0x0 no drop
 *        0x1 dropped in HW parser to uC
 *        0x2 dropped in parser uC
 *        0x3 dropped in HW classifier to uC
 *        0x4 dropped in classifer uC
 *        0x5 dropped in PPRS
 *        0x6 dropped in checker
 *        0x7 dropped in parser
 * @STW_RX_DMA_FLD_EGRESS_PORT
 * @STW_RX_DMA_FLD_PLEN_BITMAP
 * @STW_RX_DMA_FLD_BSL_PRIORITY
 * @STW_RX_DMA_FLD_BSL_SEG_A
 */
enum pp_rx_dma_stw_fld {
	STW_RX_DMA_FLD_FIRST = STW_COMMON_FLDS_NUM,
	STW_RX_DMA_FLD_ERR_IND = STW_RX_DMA_FLD_FIRST,
	STW_RX_DMA_FLD_RPB_CLID,
	STW_RX_DMA_FLD_DST_QUEUE,
	STW_RX_DMA_FLD_USER_DEF_LEN,
	STW_RX_DMA_FLD_HDR_SZ_DIFF,
	STW_RX_DMA_FLD_HDR_LEN,
	STW_RX_DMA_FLD_NEW_POOL_POLICY,
	STW_RX_DMA_FLD_PASS_PRE_L2,
	STW_RX_DMA_FLD_DROP_PKT,
	STW_RX_DMA_FLD_EGRESS_PORT,
	STW_RX_DMA_FLD_PLEN_BITMAP,
	STW_RX_DMA_FLD_BSL_PRIORITY,
	STW_RX_DMA_FLD_BSL_SEG_A,
	STW_RX_DMA_FLD_LAST = STW_RX_DMA_FLD_BSL_SEG_A,
	STW_RX_DMA_FLDS_NUM
};

/**
 * @brief STW rx dma fields masks
 */
#define STW_RX_DMA_FLD_ERR_IND_MSB           (128)
#define STW_RX_DMA_FLD_ERR_IND_LSB           (128)
#define STW_RX_DMA_FLD_RPB_CLID_MSB          (138)
#define STW_RX_DMA_FLD_RPB_CLID_LSB          (129)
#define STW_RX_DMA_FLD_DST_QUEUE_MSB         (128 + 22)
#define STW_RX_DMA_FLD_DST_QUEUE_LSB         (128 + 11)
#define STW_RX_DMA_FLD_USER_DEF_LEN_MSB      (128 + 26)
#define STW_RX_DMA_FLD_USER_DEF_LEN_LSB      (128 + 23)
#define STW_RX_DMA_FLD_HDR_SZ_DIFF_MSB       (160 + 4)
#define STW_RX_DMA_FLD_HDR_SZ_DIFF_LSB       (155)
#define STW_RX_DMA_FLD_HDR_LEN_MSB           (160 + 14)
#define STW_RX_DMA_FLD_HDR_LEN_LSB           (160 + 5)
#define STW_RX_DMA_FLD_NEW_POOL_POLICY_MSB   (160 + 22)
#define STW_RX_DMA_FLD_NEW_POOL_POLICY_LSB   (160 + 15)
#define STW_RX_DMA_FLD_PASS_PRE_L2_MSB       (160 + 23)
#define STW_RX_DMA_FLD_PASS_PRE_L2_LSB       (160 + 23)
#define STW_RX_DMA_FLD_DROP_PKT_MSB          (160 + 26)
#define STW_RX_DMA_FLD_DROP_PKT_LSB          (160 + 24)
#define STW_RX_DMA_FLD_EGRESS_PORT_MSB       (194)
#define STW_RX_DMA_FLD_EGRESS_PORT_LSB       (187)
#define STW_RX_DMA_FLD_PLEN_BITMAP_MSB       (192 + 7)
#define STW_RX_DMA_FLD_PLEN_BITMAP_LSB       (192 + 3)
#define STW_RX_DMA_FLD_BSL_PRIORITY_MSB      (192 + 9)
#define STW_RX_DMA_FLD_BSL_PRIORITY_LSB      (192 + 8)
#define STW_RX_DMA_FLD_BSL_SEG_A_MSB         (192 + 10)
#define STW_RX_DMA_FLD_BSL_SEG_A_LSB         (192 + 10)

/**
 * @brief STW rx dma fields description
 */
#define STW_RX_DMA_FLD_ERR_IND_DESC          "Error"
#define STW_RX_DMA_FLD_RPB_CLID_DESC         "RPB CLID"
#define STW_RX_DMA_FLD_DST_QUEUE_DESC        "Destination Queue"
#define STW_RX_DMA_FLD_USER_DEF_LEN_DESC     "UD Length"
#define STW_RX_DMA_FLD_HDR_SZ_DIFF_L_DESC    "Header Size Diff Low"
#define STW_RX_DMA_FLD_HDR_SZ_DIFF_DESC      "Header Size Diff"
#define STW_RX_DMA_FLD_HDR_LEN_DESC          "Header Size"
#define STW_RX_DMA_FLD_NEW_POOL_POLICY_DESC  "New Pool Policy"
#define STW_RX_DMA_FLD_PASS_PRE_L2_DESC      "Pass Pre L2 Info"
#define STW_RX_DMA_FLD_DROP_PKT_DESC         "Drop"
#define STW_RX_DMA_FLD_EGRESS_PORT_L_DESC    "Egress Port Low"
#define STW_RX_DMA_FLD_EGRESS_PORT_DESC      "Egress Port"
#define STW_RX_DMA_FLD_PLEN_BITMAP_DESC      "Policies Bitmap"
#define STW_RX_DMA_FLD_BSL_PRIORITY_DESC     "BSL Priority"
#define STW_RX_DMA_FLD_BSL_SEG_A_DESC        "BSL Segmentation"

struct rx_dma_database {
	struct pp_rx_dma_init_params init_params;
	struct fsqm_cfg fsqm_config;
};

/**
 * @brief Get hw statistics from rx-dma
 * @param stats statistics to return
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_hw_stats_get(struct rx_dma_hw_stats *stats);

/**
 * @brief Set global rx-dma config
 * @param cfg rx-dma global config
 */
void rx_dma_global_config_set(struct rx_dma_global_cfg *cfg);

/**
 * @brief Get global rx-dma config
 * @param cfg rx-dma global config placeholder
 */
void rx_dma_global_config_get(struct rx_dma_global_cfg *cfg);
#ifdef CONFIG_DEBUG_FS
/**
 * @brief rx dma debug init
 *
 * @param void
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 rx_dma_dbg_init(struct dentry *parent);

/**
 * @brief rx dma debug cleanup
 *
 * @param void
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 rx_dma_dbg_clean(void);
#else
static inline s32 rx_dma_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 rx_dma_dbg_clean(void)
{
	return 0;
}
#endif
#endif /* __RX_DMA_INTERNAL_H__ */
