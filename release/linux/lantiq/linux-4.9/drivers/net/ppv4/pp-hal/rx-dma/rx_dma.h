/*
 * rx_dma.h
 * Description: Packet Processor RX-DMA
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#ifndef __RX_DMA_H__
#define __RX_DMA_H__
#include <linux/types.h>
#include <linux/pp_api.h>
#include <linux/debugfs.h>
#include "pp_regs.h"

#define RX_DMA_MAX_POOLS (5)

/**
 * @define DBG_DESC_WORDS_CNT
 * @brief dbg descriptor size in 32bit words granularity
 */
#define RX_DMA_DESC_WORDS_CNT             (4)

/**
 * \brief Buffer manager initial configuration
 * @valid params valid
 * @rx_dma_base rx dma base address
 * @qmgr_push_addr
 * @qos_query_addr
 * @wred_response_addr
 * @bmgr_pop_push_addr
 * @fsqm_buff_addr
 * @fsqm_desc_addr
 * @fsqm_sram_addr
 * @fsqm_sram_sz
 * @fsqm_max_pkt_sz
 * @fsqm_buff_offset
 * @buffer_size buffer size per bit [STW.PLenBitmap]
 *        mapping of each bit to buffer size (from small to
 *        big)
 * @dbgfs debugfs parent directory
 */
struct pp_rx_dma_init_params {
	bool        valid;
	u64         rx_dma_base;
	phys_addr_t qmgr_push_addr;
	phys_addr_t qos_query_addr;
	phys_addr_t wred_response_addr;
	phys_addr_t bmgr_pop_push_addr;
	phys_addr_t fsqm_buff_addr;
	phys_addr_t fsqm_desc_addr;
	phys_addr_t fsqm_sram_addr;
	u32         fsqm_sram_sz;
	u32         fsqm_max_pkt_sz;
	u32         fsqm_buff_offset;
	u32 buffer_size[RX_DMA_MAX_POOLS];
	struct dentry *dbgfs;
};

#ifdef CONFIG_PPV4_LGM
/**
 * @brief rx-dma port configuration
 *
 * @port_id pp port id (GPID)

 * @headroom_size this determines the starting offset for
 *         the packet write in a new buffer before sending
 *         packet to egress peripherals
 * @tailroom_size when selecting a buffer size RXDMA will
 *         make sure there are at least Tailroom bytes from the
 *         end of the packet (incl. timestamp) to the end of the
 *         buffer
 * @wr_desc if set, the RX DMA will write the descriptor
 *          to the buffer
 * @pkt_only if set, the RXDMA will report data offset
 *         and packet len to include the packet only, even if
 *         PreL2 and/or timestamp exists
 * @min_pkt_size if the packet is shorter (without PreL2 &
 *         timestamp), then this size, then the packet is to be
 *         extended to the minimum size (just length, no
 *         padding)
 */
struct rx_dma_port_cfg {
	u16                    port_id;
	u16                    headroom_size;
	u16                    tailroom_size;
	u8                     wr_desc;
	u8                     pkt_only;
	enum pp_min_tx_pkt_len min_pkt_size;
};

/* Granularity of 16Bytes - Fetching 8x16 = 128Bytes */
#define RX_DMA_FSQM_DFLT_SEG_SIZE (8)
#define RX_DMA_FSQM_MAX_BUF_FETCH (16)
#define RX_DMA_FSQM_MAX_BURST     (0xF)

/**
 * @brief Fsqm Configuration
 *
 * @fsqm_buff_addr
 * @fsqm_desc_addr
 * @fsqm_sram_addr
 * @fsqm_sram_sz
 * @fsqm_max_pkt_sz
 * @fsqm_buff_offset
 * @desc_burst burst size for fsqm descriptor writes
 * @buff_request_thr number of buffers that rx_dma keep in its
.*         internal FIFOs
 * @num_buff_requests fsqm buffer requests burst size
 * @enable enable/disable fsqm
 * @segmentation_size segment size
 */
struct fsqm_cfg {
	phys_addr_t fsqm_buff_addr;
	phys_addr_t fsqm_desc_addr;
	phys_addr_t fsqm_sram_addr;
	u32         fsqm_sram_sz;
	u32         fsqm_max_pkt_sz;
	u32         fsqm_buff_offset;
	u8          desc_burst;
	u8          buff_request_thr;
	u8          num_buff_requests;
	u8          enable;
	u8          segmentation_size;
};

/**
 * @brief Dma manual transaction configuration
 *
 * @src_addr source address
 * @dst_addr destination address
 * @done_addr if done enabled the DMA will send OCP transaction
 *        with this address field
 * @done_data if done enabled the DMA will send OCP transaction
 *        with this data field
 * size Packet size in bytes
 */
struct dma_transaction {
	u32 src_addr;
	u32 dst_addr;
	u32 done_addr;
	u32 done_data;
	u32 size;
};

/**
 * @brief RX-DMA HW statistics
 */
struct rx_dma_hw_stats {
	u64 hw_parser_pkt_drop;
	u64 parser_uc_pkt_drop;
	u64 hw_classifier_pkt_drop;
	u64 classifier_uc_pkt_drop;
	u64 pre_pp_pkt_drop;
	u64 checker_pkt_drop;
	u64 parser_pkt_drop;
	u64 wred_pkt_drop;
	u64 bmgr_pkt_drop;
	u64 bmgr_no_match_pkt_drop;
	u64 robustness_violation_drop;
	u64 zero_len_drop;
	u64 fsqm_pkt_len_max_drop;
	u64 wred_pkt_candidate;
	u64 fsqm_null_ptr_counter;
	u64 rx_dma_rcvd_pkts;
	u64 rx_dma_byte_counter;

};

/**
 * @brief RX-DMA statistics
 */
struct rx_dma_stats {
	struct rx_dma_hw_stats hw;
	u64 rx_pkts;
	u64 tx_pkts;
	u64 drop_pkts;
};

/**
 * @struct pp_hw_dbg_desc
 * @brief  PP HW descriptor definition it is impossible to map
 *         the dbg descriptor hw structure exactly so we just
 *         define an array of 4 32bit registers
 */
struct pp_hw_dbg_desc {
	u32 word[RX_DMA_DESC_WORDS_CNT];
};

/**
 * @brief Get rx-dma hw version
 * @param hw_version returned hw version
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_hw_version_get(u32 *hw_version);

/**
 * @brief Get module configuration
 * @param rx-dma configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_config_get(struct pp_rx_dma_init_params *cfg);

/**
 * @brief Set rx-dma port configuration
 * @param rx-dma port configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_port_set(struct rx_dma_port_cfg *port_cfg);

/**
 * @brief Get rx-dma port configuration
 * @param rx-dma port configuration placeholder
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_port_get(struct rx_dma_port_cfg *port_cfg);

/**
 * @brief Configure Fsqm
 * @param fsqm_cfg configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_fsqm_config_set(struct fsqm_cfg *cfg);

/**
 * @brief Get fsqm configuration
 * @param fsqm_cfg configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_fsqm_config_get(struct fsqm_cfg *cfg);

/**
 * @brief Enable/Disable fsqm requests
 * @param enable if set enables fsqm requests
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_fsqm_requests_enable(bool enable);

/**
 * @brief Get statistics from rx-dma
 * @param stats statistics to return
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_stats_get(struct rx_dma_stats *stats);

/**
 * @brief Reset rx-dma statistics
 */
void rx_dma_stats_reset(void);

/**
 * @brief Get debug desc
 */
s32 rx_dma_desc_dbg_get(struct pp_hw_dbg_desc *dbg_desc);

/**
 * @brief Set module base addresses
 * @param rx_dma_base rx dma base address
 */
void rx_dma_base_addr_set(u64 rx_dma_base);

/**
 * @brief Module init function
 * @param init_params initial parameters
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rx_dma_init(struct pp_rx_dma_init_params *init_params);

/**
 * @brief Module exit function, clean all resources
 *
 * @return void
 */
void rx_dma_exit(void);
#else
static inline s32 rx_dma_init(struct pp_rx_dma_init_params *init_params)
{
	if (init_params->valid)
		return -EINVAL;
	else
		return 0;
}

static inline void rx_dma_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __RX_DMA_H__ */
