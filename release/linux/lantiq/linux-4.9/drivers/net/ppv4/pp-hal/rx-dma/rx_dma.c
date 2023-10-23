/*
 * rx_dma.c
 * Description: Packet Processor RX-DMA Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include <linux/spinlock.h>
#include "pp_regs.h"
#include "pp_common.h"
#include "rx_dma.h"
#include "rx_dma_internal.h"

/**
 * @brief Defines all statistics required for implementing SW reset
 *        statistics
 */
struct rx_dma_rstats {
	u32 rx_dma_rcvd_pkts;
};

/**
 * @brief Used to prevent parallel access
 */
spinlock_t lock;
u64 rx_dma_base_addr;

/**
 * @brief module database
 */
static struct rx_dma_database *db;
static struct rx_dma_rstats rstats;

s32 rx_dma_hw_version_get(u32 *hw_version)
{
	if (!hw_version) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	*hw_version = PP_REG_RD32(PP_RX_DMA_RX_VER_REG);
	return 0;
}

static bool __rx_dma_validate_pool_sort(struct pp_rx_dma_init_params *cfg)
{
	u32  idx;
	bool prev_buff_size_zero = false;

	for (idx = 1 ; idx < RX_DMA_MAX_POOLS ; idx++) {
		if (prev_buff_size_zero) {
			/* Make sure all buff sizes are zero */
			if (cfg->buffer_size[idx] != 0) {
				pr_err("Buffer sizes not sorted\n");
				return false;
			}
		} else if (cfg->buffer_size[idx] == 0) {
			prev_buff_size_zero = true;
		} else if (cfg->buffer_size[idx] < cfg->buffer_size[idx - 1]) {
			pr_err("Buffer sizes are not sorted\n");
			return false;
		}
	}

	return true;
}

void rx_dma_global_config_get(struct rx_dma_global_cfg *cfg)
{
	u32 reg_val = PP_REG_RD32(PP_RX_DMA_CFG_REG);

	cfg->ddr_align = FIELD_GET(PP_RX_DMA_CFG_DDR_ALIGN_EN_MSK, reg_val);
	cfg->wnp_disable = FIELD_GET(PP_RX_DMA_CFG_WNP_DIS_MSK, reg_val);
	cfg->buffer_exchange = FIELD_GET(
		PP_RX_DMA_CFG_BUFFER_EXCHANGE_POLICIES_IDENTICAL_I_MSK,
		reg_val);
	cfg->max_burst = FIELD_GET(PP_RX_DMA_CFG_MAX_BURST_MSK, reg_val);
	cfg->max_npend =
		FIELD_GET(PP_RX_DMA_CFG_MAX_NPENDINGTRANS_MSK, reg_val);
}

void rx_dma_global_config_set(struct rx_dma_global_cfg *cfg)
{
	u32 reg_val = PP_REG_RD32(PP_RX_DMA_CFG_REG);

	reg_val = PP_FIELD_MOD(PP_RX_DMA_CFG_DDR_ALIGN_EN_MSK, !!cfg->ddr_align,
			       reg_val);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_CFG_WNP_DIS_MSK, !!cfg->wnp_disable,
			       reg_val);
	reg_val = PP_FIELD_MOD(
		PP_RX_DMA_CFG_BUFFER_EXCHANGE_POLICIES_IDENTICAL_I_MSK,
		!!cfg->buffer_exchange, reg_val);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_CFG_MAX_BURST_MSK, cfg->max_burst,
			       reg_val);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_CFG_MAX_NPENDINGTRANS_MSK,
			       cfg->max_npend, reg_val);

	PP_REG_WR32(PP_RX_DMA_CFG_REG, reg_val);
}

static s32 __rx_dma_config_set(struct pp_rx_dma_init_params *cfg)
{
	s32 ret = 0;
	u32 idx;
	u32 reg_val;
	u32 min_pkt_len[] = {PP_MIN_TX_PKT_LEN_VAL_NONE,
			     PP_MIN_TX_PKT_LEN_VAL_60B,
			     PP_MIN_TX_PKT_LEN_VAL_64B,
			     PP_MIN_TX_PKT_LEN_VAL_128B};
	struct fsqm_cfg fsqm_config;

	if (!cfg) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	spin_lock_bh(&lock);
	if (!__rx_dma_validate_pool_sort(cfg)) {
		pr_err("Pool size validation failed\n");
		spin_unlock_bh(&lock);
		return -EINVAL;
	}

	/* Configure buffer size per bit */
	for (idx = 0 ; idx < RX_DMA_MAX_POOLS ; idx++) {
		PP_REG_WR32(PP_RX_DMA_POOL_SIZE_REG_IDX(idx),
			    cfg->buffer_size[idx]);
	}

	/* Configure base addressses */
	PP_REG_WR32(PP_RX_DMA_BMGR_BASE_REG,
		    db->init_params.bmgr_pop_push_addr >> 4);

	PP_REG_WR32(PP_RX_DMA_WRED_BASE_REG,
		    db->init_params.qos_query_addr >> 4);

	PP_REG_WR32(PP_RX_DMA_WRED_ADDR_RESP_REG,
		    db->init_params.wred_response_addr >> 4);

	PP_REG_WR32(PP_RX_DMA_Q_MANAGER_BASE_REG,
		    db->init_params.qmgr_push_addr >> 4);


	/* Configure ownership bit to 0 */
	reg_val = PP_REG_RD32(PP_RX_DMA_OWNERSHIP_BIT_REG);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_OWNERSHIP_BIT_OWNERSHIP_BIT_I_MSK,
			       0, reg_val);
	PP_REG_WR32(PP_RX_DMA_OWNERSHIP_BIT_REG, reg_val);

	/* Configure minimum port packet sizes */
	for (idx = 0 ; idx < PP_NUM_TX_MIN_PKT_LENS ; idx++) {
		reg_val = min_pkt_len[idx];
		PP_REG_WR32(
			PP_RX_DMA_PORT_PKT_LENGTH_MIN_CFG_SIZE_REG_IDX(idx),
			reg_val);
	}

	/* Configure DMA alignment and burst size */
	reg_val = PP_REG_RD32(PP_RX_DMA_CFG_REG);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_CFG_DDR_ALIGN_EN_MSK, 1, reg_val);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_CFG_MAX_BURST_MSK,
			       RX_DMA_CFG_MAX_BURST, reg_val);
	PP_REG_WR32(PP_RX_DMA_CFG_REG, reg_val);
	spin_unlock_bh(&lock);

	/* Set default fsqm configuration */
	fsqm_config.fsqm_buff_addr    = db->init_params.fsqm_buff_addr;
	fsqm_config.fsqm_desc_addr    = db->init_params.fsqm_desc_addr;
	fsqm_config.fsqm_sram_addr    = db->init_params.fsqm_sram_addr;
	fsqm_config.fsqm_sram_sz      = db->init_params.fsqm_sram_sz;
	fsqm_config.fsqm_max_pkt_sz   = db->init_params.fsqm_max_pkt_sz;
	fsqm_config.fsqm_buff_offset  = db->init_params.fsqm_buff_offset;
	fsqm_config.desc_burst        = RX_DMA_FSQM_DESC_BURST;
	fsqm_config.buff_request_thr  = RX_DMA_FSQM_BUFF_THR;
	fsqm_config.num_buff_requests = RX_DMA_FSQM_BUFF_BURST_REQ;

	/* Do not enable fsqm fetch since CQM is not ready yet */
	fsqm_config.enable            = 0;
	fsqm_config.segmentation_size = 0;
	ret = rx_dma_fsqm_config_set(&fsqm_config);

	if (unlikely(ret)) {
		pr_err("Failed configuring fsqm\n");
		return -EINVAL;
	}

	return ret;
}

s32 rx_dma_config_get(struct pp_rx_dma_init_params *cfg)
{
	if (!cfg) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	memcpy(cfg, &db->init_params, sizeof(struct pp_rx_dma_init_params));

	return 0;
}

s32 rx_dma_fsqm_config_set(struct fsqm_cfg *cfg)
{
	u32 mask;
	u32 reg_val;
	u32 fsqm_buf_fetch;

	if (!cfg) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Verify input arguments */
	fsqm_buf_fetch = cfg->buff_request_thr + cfg->num_buff_requests;
	if ((fsqm_buf_fetch > RX_DMA_FSQM_MAX_BUF_FETCH) ||
	    (cfg->desc_burst > RX_DMA_FSQM_MAX_BURST)) {
		pr_err("Invalid FSQM configuration\n");
		return -EINVAL;
	}

	spin_lock_bh(&lock);

	/* FSQM buffer address lower 32 bits */
	PP_REG_WR32(PP_RX_DMA_FSQM_BUFFER_BASE_REG,
		    cfg->fsqm_buff_addr >> 4);

	/* FSQM buffers offset (Higher 4 MSBits of each buffer) */
	mask = PP_RX_DMA_FSQM_BUFFER_MSB_BASE_FSQM_BUFF_BASE_I_MSK;
	reg_val = PP_REG_RD32(PP_RX_DMA_FSQM_BUFFER_MSB_BASE_REG);
	reg_val = PP_FIELD_MOD(mask, cfg->fsqm_buff_offset, reg_val);
	PP_REG_WR32(PP_RX_DMA_FSQM_BUFFER_MSB_BASE_REG, reg_val);

	/* FSQM sram */
	PP_REG_WR32(PP_RX_DMA_SRAM_START_ADDR_REG, cfg->fsqm_sram_addr >> 4);
	PP_REG_WR32(PP_RX_DMA_SRAM_SIZE_REG, cfg->fsqm_sram_sz);

	/* FSQM Descriptor address */
	PP_REG_WR32(PP_RX_DMA_FSQM_DESC_ADDR_REG,
		    cfg->fsqm_desc_addr >> 4);

	/* FSQM max packet size */
	mask = PP_RX_DMA_FSQM_PKT_LEN_MAX_FSQM_PKT_LEN_MAX_I_MSK;
	reg_val = PP_REG_RD32(PP_RX_DMA_FSQM_PKT_LEN_MAX_REG);
	reg_val = PP_FIELD_MOD(mask, cfg->fsqm_max_pkt_sz, reg_val);
	PP_REG_WR32(PP_RX_DMA_FSQM_PKT_LEN_MAX_REG, reg_val);

	/* Set segmentation size - default granularity will be 128B */
	mask = PP_RX_DMA_SEGMENTATION_SIZE_SEGMENTATION_SIZE_I_MSK;
	reg_val = PP_REG_RD32(PP_RX_DMA_SEGMENTATION_SIZE_REG);
	reg_val = PP_FIELD_MOD(mask, cfg->segmentation_size, reg_val);
	PP_REG_WR32(PP_RX_DMA_SEGMENTATION_SIZE_REG, reg_val);

	reg_val = PP_REG_RD32(PP_RX_DMA_FSQM_CFG_REG);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_FSQM_CFG_FSQM_DESC_BURST_I_MSK,
			       cfg->desc_burst, reg_val);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_FSQM_CFG_INTERNAL_BUFFER_TH_REQ_I_MSK,
			       cfg->buff_request_thr, reg_val);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_FSQM_CFG_FSQM_BURST_REQ_I_MSK,
			       cfg->num_buff_requests, reg_val);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_FSQM_CFG_FSQM_READY_I_MSK,
			       cfg->enable ? 1 : 0, reg_val);
	PP_REG_WR32(PP_RX_DMA_FSQM_CFG_REG, reg_val);

	memcpy(&db->fsqm_config, cfg, sizeof(*cfg));
	spin_unlock_bh(&lock);

	return 0;
}

s32 rx_dma_fsqm_config_get(struct fsqm_cfg *cfg)
{
	if (!cfg) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	memcpy(cfg, &db->fsqm_config, sizeof(*cfg));

	return 0;
}

s32 rx_dma_fsqm_requests_enable(bool enable)
{
	u32 reg_val;

	spin_lock_bh(&lock);
	reg_val = PP_REG_RD32(PP_RX_DMA_FSQM_CFG_REG);
	reg_val = PP_FIELD_MOD(PP_RX_DMA_FSQM_CFG_FSQM_READY_I_MSK,
			       enable ? 1 : 0, reg_val);
	PP_REG_WR32(PP_RX_DMA_FSQM_CFG_REG, reg_val);
	spin_unlock_bh(&lock);

	return 0;
}

s32 rx_dma_port_set(struct rx_dma_port_cfg *port_cfg)
{
	u32 reg_val = 0;

	if (!port_cfg) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Verify input arguments */
	if ((port_cfg->headroom_size > RX_DMA_PORT_CFG_HEADROOM_MAX) ||
	    (port_cfg->tailroom_size > RX_DMA_PORT_CFG_TAILROOM_MAX) ||
	    (port_cfg->min_pkt_size >= PP_NUM_TX_MIN_PKT_LENS) ||
	    (port_cfg->port_id >= PP_MAX_PORT)) {
		if (port_cfg->headroom_size > RX_DMA_PORT_CFG_HEADROOM_MAX) {
			pr_err("Invalid headroom_size %d\n",
			       port_cfg->headroom_size);
		}

		if (port_cfg->tailroom_size > RX_DMA_PORT_CFG_TAILROOM_MAX) {
			pr_err("Invalid tailroom_size %d\n",
			       port_cfg->tailroom_size);
		}

		if (port_cfg->port_id >= PP_MAX_PORT)
			pr_err("Invalid Port Id %d\n", port_cfg->port_id);

		return -EINVAL;
	}

	reg_val |= FIELD_PREP(PP_RX_DMA_EGRESS_PORT_CFG_PKT_ONLY_MSK,
			      port_cfg->pkt_only ? 1 : 0);
	reg_val |= FIELD_PREP(PP_RX_DMA_EGRESS_PORT_CFG_ERROR_BIT_MSK, 0);
	reg_val |= FIELD_PREP(PP_RX_DMA_EGRESS_PORT_CFG_MIN_PKT_SIZE_MSK,
			      port_cfg->min_pkt_size);
	reg_val |= FIELD_PREP(PP_RX_DMA_EGRESS_PORT_CFG_DESC_TO_BUFFER_MSK,
			      port_cfg->wr_desc ? 1 : 0);
	reg_val |= FIELD_PREP(PP_RX_DMA_EGRESS_PORT_CFG_TAILROOM_MSK,
			      port_cfg->tailroom_size);
	reg_val |= FIELD_PREP(PP_RX_DMA_EGRESS_PORT_CFG_HEADROOM_MSK,
			      port_cfg->headroom_size);
	PP_REG_WR32(PP_RX_DMA_EGRESS_PORT_CFG_REG_IDX(port_cfg->port_id),
		    reg_val);

	return 0;
}

s32 rx_dma_port_get(struct rx_dma_port_cfg *port_cfg)
{
	u32 reg_val;

	if (!port_cfg) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	/* Verify input arguments */
	if (port_cfg->port_id >= PP_MAX_PORT) {
		pr_err("Invalid Port Id %d\n", port_cfg->port_id);
		return -EINVAL;
	}

	reg_val = PP_REG_RD32(
		PP_RX_DMA_EGRESS_PORT_CFG_REG_IDX(port_cfg->port_id));
	port_cfg->pkt_only =
		FIELD_GET(PP_RX_DMA_EGRESS_PORT_CFG_PKT_ONLY_MSK, reg_val);
	port_cfg->min_pkt_size =
		FIELD_GET(PP_RX_DMA_EGRESS_PORT_CFG_MIN_PKT_SIZE_MSK, reg_val);
	port_cfg->wr_desc =
		FIELD_GET(PP_RX_DMA_EGRESS_PORT_CFG_DESC_TO_BUFFER_MSK,
			  reg_val);
	port_cfg->tailroom_size =
		FIELD_GET(PP_RX_DMA_EGRESS_PORT_CFG_TAILROOM_MSK, reg_val);
	port_cfg->headroom_size =
		FIELD_GET(PP_RX_DMA_EGRESS_PORT_CFG_HEADROOM_MSK, reg_val);

	return 0;
}

s32 rx_dma_hw_stats_get(struct rx_dma_hw_stats *stats)
{
	if (!stats) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	stats->hw_parser_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_HW_PARSER_DROP_CNT_REG);
	stats->parser_uc_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_PARSER_UC_DROP_CNT_REG);
	stats->hw_classifier_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_HW_CLS_DROP_CNT_REG);
	stats->classifier_uc_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_CLS_UC_DROP_CNT_REG);
	stats->pre_pp_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_PPRS_DROP_CNT_REG);
	stats->checker_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_CHECKER_DROP_CNT_REG);
	stats->parser_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_PARSER_DROP_CNT_REG);
	stats->wred_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_WRED_PKT_DROP_CNT_REG);
	stats->bmgr_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_BMGR_PKT_DROP_CNT_REG);
	stats->bmgr_no_match_pkt_drop =
		PP_REG_RD32(PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_REG);
	stats->fsqm_pkt_len_max_drop =
		PP_REG_RD32(PP_RX_DMA_FSQM_PKT_LEN_MAX_DROP_CNT_REG);
	stats->wred_pkt_candidate =
		PP_REG_RD32(PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_REG);
	stats->fsqm_null_ptr_counter =
		PP_REG_RD32(PP_RX_DMA_FSQM_BUFFER_NULL_CNT_REG);
	stats->robustness_violation_drop =
		PP_REG_RD32(PP_RX_DMA_ERROR_CNT_REG);
	stats->zero_len_drop =
		PP_REG_RD32(PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_REG);
	stats->rx_dma_byte_counter =
		PP_BUILD_64BIT_WORD(PP_REG_RD32(PP_RX_DMA_BYTE_CNT_L_REG),
				    PP_REG_RD32(PP_RX_DMA_BYTE_CNT_H_REG));
	/* this is the only counter we cannot reset by hw, so we keep
	 * a snapshot from last reset request and use it here
	 */
	stats->rx_dma_rcvd_pkts = PP_REG_RD32(PP_RX_DMA_PKT_CNT_DBG_REG) -
		rstats.rx_dma_rcvd_pkts;
	return 0;
}

s32 rx_dma_stats_get(struct rx_dma_stats *stats)
{
	if (!stats) {
		pr_err("NULL argument\n");
		return -EINVAL;
	}

	rx_dma_hw_stats_get(&stats->hw);

	stats->rx_pkts = stats->hw.rx_dma_rcvd_pkts;
	stats->tx_pkts = stats->hw.wred_pkt_candidate;
	stats->drop_pkts =
		stats->hw.bmgr_no_match_pkt_drop +
		stats->hw.bmgr_pkt_drop +
		stats->hw.checker_pkt_drop +
		stats->hw.classifier_uc_pkt_drop +
		stats->hw.hw_classifier_pkt_drop +
		stats->hw.hw_parser_pkt_drop +
		stats->hw.parser_pkt_drop +
		stats->hw.parser_uc_pkt_drop +
		stats->hw.pre_pp_pkt_drop +
		stats->hw.robustness_violation_drop +
		stats->hw.wred_pkt_drop +
		stats->hw.zero_len_drop +
		stats->hw.fsqm_pkt_len_max_drop;

	return 0;
}

void rx_dma_stats_reset(void)
{
	u32 reset_val = 0xFFFFFFFF;

	/* reset all counters we can by HW */
	PP_REG_WR32(PP_RX_DMA_HW_PARSER_DROP_CNT_REG,       reset_val);
	PP_REG_WR32(PP_RX_DMA_PARSER_UC_DROP_CNT_REG,       reset_val);
	PP_REG_WR32(PP_RX_DMA_HW_CLS_DROP_CNT_REG,          reset_val);
	PP_REG_WR32(PP_RX_DMA_CLS_UC_DROP_CNT_REG,          reset_val);
	PP_REG_WR32(PP_RX_DMA_PPRS_DROP_CNT_REG,            reset_val);
	PP_REG_WR32(PP_RX_DMA_CHECKER_DROP_CNT_REG,         reset_val);
	PP_REG_WR32(PP_RX_DMA_PARSER_DROP_CNT_REG,          reset_val);
	PP_REG_WR32(PP_RX_DMA_WRED_PKT_DROP_CNT_REG,        reset_val);
	PP_REG_WR32(PP_RX_DMA_BMGR_PKT_DROP_CNT_REG,        reset_val);
	PP_REG_WR32(PP_RX_DMA_DROP_CNT_NO_POLICY_CNT_REG,   reset_val);
	PP_REG_WR32(PP_RX_DMA_WRED_PKT_CANDIDATE_CNT_REG,   reset_val);
	PP_REG_WR32(PP_RX_DMA_ERROR_CNT_REG,                reset_val);
	PP_REG_WR32(PP_RX_DMA_HEADER_LEN_ZERO_DROP_CNT_REG, reset_val);
	PP_REG_WR32(PP_RX_DMA_BYTE_CNT_L_REG,               reset_val);
	PP_REG_WR32(PP_RX_DMA_BYTE_CNT_H_REG,               reset_val);
	/* save snapshot of the only 1 counter we can reset by HW */
	rstats.rx_dma_rcvd_pkts = PP_REG_RD32(PP_RX_DMA_PKT_CNT_DBG_REG);
}

s32 rx_dma_desc_dbg_get(struct pp_hw_dbg_desc *dbg_desc)
{
	u32 idx;

	if (unlikely(ptr_is_null(dbg_desc)))
		return -EINVAL;

	for (idx = 0; idx < RX_DMA_DESC_WORDS_CNT; idx++) {
		dbg_desc->word[idx] =
			PP_REG_RD32(PP_RX_DMA_DESC_DBG_REG_IDX(idx));
	}

	return 0;
}

void rx_dma_base_addr_set(u64 rx_dma_base)
{
	rx_dma_base_addr = rx_dma_base;
}

void rx_dma_exit(void)
{
	rx_dma_dbg_clean();
	kfree(db);
	db = NULL;
}

s32 rx_dma_init(struct pp_rx_dma_init_params *init_params)
{
	s32 ret = 0;

	if (!init_params->valid)
		return -EINVAL;

	rx_dma_base_addr = init_params->rx_dma_base;

	/* database */
	db = kzalloc(sizeof(*db), GFP_KERNEL);
	if (unlikely(!db)) {
		pr_err("cannot allocate database\n");
		return -ENOMEM;
	}

	memcpy(&db->init_params, init_params, sizeof(*init_params));
	spin_lock_init(&lock);

	ret = __rx_dma_config_set(init_params);
	if (unlikely(ret)) {
		pr_err("Failed to init rx dma\n");
		return ret;
	}

	ret = rx_dma_dbg_init(init_params->dbgfs);
	if (unlikely(ret)) {
		pr_err("Failed to init debug stuff\n");
		return ret;
	}

	pr_debug("pp rx-dma was initialized\n");

	return ret;
}
