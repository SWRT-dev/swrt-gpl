/*
 * rx_Dma_debugfs.c
 * Description: Packet Processor RX-DMA Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include <linux/parser.h>
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_misc.h"
#include "rx_dma.h"
#include "rx_dma_internal.h"

static struct dentry *dbgfs;

#define FSQM_BUFF_SIZE   (128)
#define FSQM_NUM_BUFFERS (4)
#define FSQM_DESC_SIZE   (16)

static void *fsqm_buffers;
static phys_addr_t fsqm_buffers_phys;
static size_t fsqm_buff_sz;
static void *fsqm_descriptors;
static phys_addr_t fsqm_descriptors_phys;
static size_t fsqm_desc_sz;

struct buf_fld_info rx_dma_stw_flds[STW_RX_DMA_FLDS_NUM];

struct pp_hw_stw_rx_dma {
	u32 word[STW_RX_DMA_WORD_CNT];
};

struct pp_stw_rx_dma_values {
	u32 val[STW_RX_DMA_FLDS_NUM];
};

const char * const rx_dma_stats_str[] = {
	"Hw Parser Drop",
	"Parser uC Drop",
	"Hw Classifier Drop",
	"Classifier uC Drop",
	"PPRS Drop",
	"Checker Drop",
	"Parser Drop",
	"WRED Drop",
	"Bmgr Drop",
	"Bmgr No Match Drop",
	"Robustness Violation Drop",
	"Zero Len Drop",
	"FSQM Max Pkt Len Drop",
	"Wred Pkt Candidate",
	"FSQM Null Buffers",
	"Rx Packet Count",
	"Rx Dma Byte Counter"
};

s32 pp_stw_rx_dma_get(struct pp_stw_rx_dma_values *stw_rx_dma_vals)
{
	struct pp_hw_stw_rx_dma stw_hw;
	u32 fld_idx;

	if (unlikely(ptr_is_null(stw_rx_dma_vals)))
		return -EINVAL;

	PP_REG_RD_REP(PP_RX_DMA_STATUS_DBG_REG, &stw_hw, sizeof(stw_hw));

	for (fld_idx = 0 ; fld_idx < STW_RX_DMA_FLDS_NUM; fld_idx++) {
		stw_rx_dma_vals->val[fld_idx] =
			buf_fld_get(&rx_dma_stw_flds[fld_idx],
				    (void *)(&stw_hw));
	}

	return 0;
}

/**
 * @brief Dump status word
 */
void __rx_dma_stw_show(struct seq_file *f)
{
	struct pp_stw_rx_dma_values rx_dma_vals;
	struct buf_fld_info        *fld;
	u32 fld_idx, val;
	s32 ret = 0;

	memset(&rx_dma_vals, 0, sizeof(rx_dma_vals));
	ret = pp_stw_rx_dma_get(&rx_dma_vals);
	if (unlikely(ret))
		return;

	seq_puts(f, "\n RX DMA Status Word:\n");
	seq_puts(f, " ===================\n");
	for (fld_idx = 0 ; fld_idx < STW_RX_DMA_FLDS_NUM ; fld_idx++) {
		fld = &rx_dma_stw_flds[fld_idx];
		val = rx_dma_vals.val[fld_idx];

		switch (fld_idx) {
		case STW_COMMON_FLD_DATA_OFFSET:
		case STW_RX_DMA_FLD_HDR_SZ_DIFF:
			seq_printf(f, " %-35s: %d [%#x]\n", fld->desc, (s32)val,
				   val);
			break;

		case STW_RX_DMA_FLD_DROP_PKT:
			seq_printf(f, " %-35s: %s (%u)\n", fld->desc,
				   STW_DROP_PKT_STR(val), val);
			break;

		default:
			if (fld->msb == fld->lsb)
				seq_printf(f, " %-35s: %s\n", fld->desc,
					   BOOL2STR(val));
			else
				seq_printf(f, " %-35s: %u [%#x]\n", fld->desc,
					   val, val);
		}
	}
}

PP_DEFINE_DEBUGFS(stw_show, __rx_dma_stw_show, NULL);

/**
 * @brief show version
 *
 * @param f file pointer
 * @param unused unused
 *
 * @return int 0 on success, non-zero value otherwise
 */
void dbg_version_show(struct seq_file *f)
{
	u32 version;

	rx_dma_hw_version_get(&version);
	seq_printf(f, "RX-DMA Version: %d\n", version);
}

PP_DEFINE_DEBUGFS(version, dbg_version_show, NULL);

/**
 * @brief show stats
 */
void dbg_stats_show(struct seq_file *f)
{
	struct rx_dma_hw_stats stats;
	u32 idx = 0;
	u64 *cnt;

	rx_dma_hw_stats_get(&stats);

	seq_puts(f, "+---------------------------+-------------------------+\n");
	seq_puts(f, "|          Name             |          Count          |\n");
	seq_puts(f, "+---------------------------+-------------------------+\n");
	for_each_struct_mem(&stats, cnt, idx++)
		seq_printf(f, "| %-25s | %-23llu |\n", rx_dma_stats_str[idx],
			   *cnt);
	seq_puts(f, "+---------------------------+-------------------------+\n");
}

PP_DEFINE_DEBUGFS(stats_show, dbg_stats_show, NULL);

/**
 * @brief show configuration
 */
void dbg_conf_show(struct seq_file *f)
{
	u32    idx;
	struct pp_rx_dma_init_params cfg;

	rx_dma_config_get(&cfg);
	seq_puts(f, "Buff size pools:\n");
	for (idx = 0 ; idx < RX_DMA_MAX_POOLS ; idx++)
		seq_printf(f, "Pool[%d]: %d\n", idx, cfg.buffer_size[idx]);
}

PP_DEFINE_DEBUGFS(conf_show, dbg_conf_show, NULL);

/**
 * @brief print port configuration
 *
 * @param data unused
 * @param val value from user (port id)
 *
 * @return int 0 on success, non-zero value otherwise
 */
static int dbg_port_show(void *data, u64 val)
{
	struct rx_dma_port_cfg port_cfg;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	/* Verify input arguments */
	if (val >= PP_MAX_PORT) {
		pr_err("Invalid Port Id %d\n", (u16)val);
		return -EINVAL;
	}

	port_cfg.port_id = (u16)val;
	rx_dma_port_get(&port_cfg);
	pr_info("Port %d Configuration:\n", port_cfg.port_id);
	pr_info("* Headroom:     %d\n", port_cfg.headroom_size);
	pr_info("* Tailroom:     %d\n", port_cfg.tailroom_size);
	pr_info("* Wr Desc:      %d\n", port_cfg.wr_desc);
	pr_info("* pkt_only:     %d\n", port_cfg.pkt_only);
	pr_info("* min_pkt_size: %d\n", port_cfg.min_pkt_size);

	return 0;
}

PP_DEFINE_DBGFS_ATT(dbg_port_show_fops, NULL, dbg_port_show);

void dbg_port_set_help(struct seq_file *f)
{
	seq_puts(f, "\"<port_id> <hr> <tr> <wr> <po> <mpi>\"\n");
}

void dbg_port_set(char *cmd_buf, void *data)
{
	struct rx_dma_port_cfg port_cfg;
	u32 min_pkt_sz;

	if (sscanf(cmd_buf, "%hu %hu %hu %hhu %hhu %u",
		   &port_cfg.port_id, &port_cfg.headroom_size,
		   &port_cfg.tailroom_size, &port_cfg.wr_desc,
		   &port_cfg.pkt_only, &min_pkt_sz) != 6) {
		pr_err("sscanf err\n");
		return;
	}

	port_cfg.min_pkt_size = min_pkt_sz;

	if (rx_dma_port_set(&port_cfg)) {
		pr_err("rx_dma_set_port failed\n");
		return;
	}
}

PP_DEFINE_DEBUGFS(port_set, dbg_port_set_help, dbg_port_set);

/**
 * @brief fsqm memory allocation
 */
void dbg_fsqm_alloc(struct seq_file *f)
{
	u32 buff_idx;
	s32 ret;
	u32 buffer_ptr_offset;
	u32 buffer_addr_offset;
	struct fsqm_cfg cfg;

	ret = rx_dma_fsqm_config_get(&cfg);
	if (unlikely(ret)) {
		seq_puts(f, "Failed fetching fsqm config\n");
		goto err;
	}

	if (unlikely(cfg.enable)) {
		seq_puts(f, "FSQM already enabled\n");
		goto err;
	}

	/* Allocate 128B segment for fsqm */
	fsqm_buff_sz = PAGE_ALIGN((FSQM_BUFF_SIZE + 4) * FSQM_NUM_BUFFERS);
	fsqm_buffers = dma_zalloc_coherent(pp_dev_get(),
					   fsqm_buff_sz,
					   &fsqm_buffers_phys,
					   GFP_KERNEL);
	if (unlikely(!fsqm_buffers)) {
		seq_printf(f, "Failed to allocate %zu bytes for fsqm buffers\n",
			   fsqm_buff_sz);
		goto err;
	}

	for (buff_idx = 0; buff_idx < 4; buff_idx++) {
		buffer_ptr_offset = 4 * buff_idx;
		buffer_addr_offset = (FSQM_NUM_BUFFERS * 4) +
			(FSQM_BUFF_SIZE * buff_idx);

		*(u32 *)((unsigned long)fsqm_buffers + buffer_ptr_offset) =
			 fsqm_buffers_phys + buffer_addr_offset;
	}

	/* Allocate fsqm descriptor */
	fsqm_desc_sz = PAGE_ALIGN(FSQM_DESC_SIZE + 4);
	fsqm_descriptors = dma_zalloc_coherent(pp_dev_get(),
					       fsqm_desc_sz,
					       &fsqm_descriptors_phys,
					       GFP_KERNEL);
	if (unlikely(!fsqm_buffers)) {
		seq_printf(f, "Failed to allocate %zu bytes for fsqm descriptors\n",
			   fsqm_desc_sz);
		goto err;
	}

	cfg.fsqm_buff_addr = fsqm_buffers_phys;
	cfg.fsqm_desc_addr = fsqm_descriptors_phys;
	cfg.fsqm_max_pkt_sz = 10240;
	cfg.segmentation_size = RX_DMA_FSQM_DFLT_SEG_SIZE;
	cfg.enable = 1;

	ret = rx_dma_fsqm_config_set(&cfg);
	if (unlikely(ret)) {
		seq_puts(f, "Failed setting fsqm config\n");
		goto err;
	}

	seq_printf(f, "FSQM configured. (Buff %#llx. Desc %#llx)\n",
		   fsqm_buffers_phys, fsqm_descriptors_phys);
	return;

err:
	pr_err("");
	if (fsqm_buffers) {
		dma_free_coherent(pp_dev_get(), fsqm_buff_sz,
				  fsqm_buffers, fsqm_buffers_phys);
	}

	if (fsqm_descriptors) {
		dma_free_coherent(pp_dev_get(), fsqm_desc_sz,
				  fsqm_descriptors, fsqm_descriptors_phys);
	}
}

PP_DEFINE_DEBUGFS(fsqm_alloc, dbg_fsqm_alloc, NULL);

/**
 * @brief fsqm free memory allocation
 */
void dbg_fsqm_free(struct seq_file *f)
{
	s32 ret;
	struct fsqm_cfg cfg;

	ret = rx_dma_fsqm_config_get(&cfg);
	if (unlikely(ret)) {
		seq_puts(f, "Failed fetching fsqm config\n");
		return;
	}

	cfg.fsqm_buff_addr = 0;
	cfg.fsqm_desc_addr = 0;
	cfg.fsqm_max_pkt_sz = 0;
	cfg.segmentation_size = 0;
	cfg.enable = 0;

	ret = rx_dma_fsqm_config_set(&cfg);
	if (unlikely(ret)) {
		seq_puts(f, "Failed setting fsqm config\n");
		return;
	}

	if (fsqm_buffers) {
		dma_free_coherent(pp_dev_get(), fsqm_buff_sz,
				  fsqm_buffers, fsqm_buffers_phys);
	}

	if (fsqm_descriptors) {
		dma_free_coherent(pp_dev_get(), fsqm_desc_sz,
				  fsqm_descriptors, fsqm_descriptors_phys);
	}
}

PP_DEFINE_DEBUGFS(fsqm_free, dbg_fsqm_free, NULL);

enum global_opts {
	global_opt_help,
	global_opt_max_burst,
	global_opt_ddr_align,
	global_opt_fv_wnp_disable,
	global_opt_fv_max_npend,
	global_opt_fv_buffer_exchange,
};

static const match_table_t global_tokens = {
	{global_opt_help,               "help"},
	{global_opt_max_burst,          "max_burst=%u"},
	{global_opt_ddr_align,          "ddr_align=%u"},
	{global_opt_fv_wnp_disable,     "wnp_disable=%u"},
	{global_opt_fv_max_npend,       "max_npend=%u"},
	{global_opt_fv_buffer_exchange, "buffer_exchange=%u"},
};

/**
 * @brief Print rx-dma configuration help, how to change global
 *        configuration settings
 */
static void __rxdma_cfg_help(void)
{
	pr_info("\n");
	pr_info(" Usage: echo <option>[=value] > global\n");
	pr_info(" Options:\n");
	pr_info("   help            - print this help function\n");
	pr_info("   max_burst       - Maximum burst size\n");
	pr_info("   ddr_align       - Enable DDR alignment\n");
	pr_info("   wnp_disable     - Disable 'write non posted'\n");
	pr_info("   max_npend       - set the max nPendingTransaction\n");
	pr_info("   buffer_exchange - Enable buffer exchange\n");
}

/**
 * @brief Print the rxdma configuration
 */
void __rxdma_cfg_show(struct seq_file *f)
{
	struct rx_dma_global_cfg cfg;

	rx_dma_global_config_get(&cfg);
	seq_puts(f, "\n");
	seq_puts(f, " +-------------------------------------+------------+\n");
	seq_puts(f, " | Config                              | Value      |\n");
	seq_puts(f, " +-------------------------------------+------------+\n");
	seq_printf(f, " | %-35s | %-10u |\n", "DDR align", cfg.ddr_align);
	seq_printf(f, " | %-35s | %-10u |\n", "WNP disable", cfg.wnp_disable);
	seq_printf(f, " | %-35s | %-10u |\n", "Buffer exchange enable",
		   cfg.buffer_exchange);
	seq_printf(f, " | %-35s | %-10u |\n", "Max burst", cfg.max_burst);
	seq_printf(f, " | %-35s | %-10u |\n", "Max Npend", cfg.max_npend);
	seq_puts(f, " +-------------------------------------+------------+\n");
	seq_puts(f, "\n");
}

/**
 * @brief Change rxdma configuration setting using debugfs
 *        file
 */
void __rxdma_cfg_set(char *cmd_buf, void *data)
{
	substring_t args[MAX_OPT_ARGS];
	struct rx_dma_global_cfg cfg;
	enum global_opts opt;
	char *tok, *params;
	int val;

	rx_dma_global_config_get(&cfg);

	params = cmd_buf;
	/* iterate over user params */
	while ((tok = strsep(&params, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, global_tokens, args);
		switch (opt) {
		case global_opt_help:
			__rxdma_cfg_help();
			return;
		case global_opt_max_burst:
			if (match_int(&args[0], &val))
				return;
			cfg.max_burst = val;
			break;
		case global_opt_ddr_align:
			if (match_int(&args[0], &val))
				return;
			cfg.ddr_align = val;
			break;
		case global_opt_fv_wnp_disable:
			if (match_int(&args[0], &val))
				return;
			cfg.wnp_disable = val;
			break;
		case global_opt_fv_max_npend:
			if (match_int(&args[0], &val))
				return;
			cfg.max_npend = val;
			break;
		case global_opt_fv_buffer_exchange:
			if (match_int(&args[0], &val))
				return;
			cfg.buffer_exchange = val;
			break;
		default:
			break;
		}
	}

	rx_dma_global_config_set(&cfg);
}

PP_DEFINE_DEBUGFS(rxdma_cfg, __rxdma_cfg_show, __rxdma_cfg_set);

static struct debugfs_file fsqm_dbg_files[] = {
	{"fsqm_ddr_alloc", &PP_DEBUGFS_FOPS(fsqm_alloc)},
	{"fsqm_ddr_free", &PP_DEBUGFS_FOPS(fsqm_free)},
};

static struct debugfs_file rx_dma_dbg_files[] = {
	{ "version", &PP_DEBUGFS_FOPS(version) },
	{ "stats", &PP_DEBUGFS_FOPS(stats_show) },
	{ "conf_show", &PP_DEBUGFS_FOPS(conf_show) },
	{ "stw_show", &PP_DEBUGFS_FOPS(stw_show) },
	{ "port_show", &PP_DEBUGFS_FOPS(dbg_port_show_fops) },
	{ "port_set", &PP_DEBUGFS_FOPS(port_set) },
	{ "global", &PP_DEBUGFS_FOPS(rxdma_cfg) },
};

s32 rx_dma_dbg_init(struct dentry *parent)
{
	s32 ret;

	/* rx dma debugfs dir */
	ret = pp_debugfs_create(parent, "rx_dma", &dbgfs, rx_dma_dbg_files,
				ARRAY_SIZE(rx_dma_dbg_files), NULL);
	if (unlikely(ret)) {
		pr_err("Failed to create rx_dma debugfs dir\n");
		return ret;
	}

	/* fsqm debugfs dir */
	ret = pp_debugfs_create(dbgfs, "fsqm", NULL, fsqm_dbg_files,
				ARRAY_SIZE(fsqm_dbg_files), NULL);
	if (unlikely(ret)) {
		pr_err("Failed to create rx_dma fsqm debugfs dir\n");
		return ret;
	}

	/* Init status word common fields */
	STW_INIT_COMMON_FLDS(rx_dma_stw_flds);
	/* Init rx dma specific status word fields */
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_ERR_IND);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_RPB_CLID);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_DST_QUEUE);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_USER_DEF_LEN);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_HDR_SZ_DIFF);
	rx_dma_stw_flds[STW_RX_DMA_FLD_HDR_SZ_DIFF].from_hw = fld_diff_from_hw;
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_HDR_LEN);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_NEW_POOL_POLICY);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_PASS_PRE_L2);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_DROP_PKT);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_EGRESS_PORT);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_PLEN_BITMAP);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_BSL_PRIORITY);
	BUF_FLD_INIT_DESC(rx_dma_stw_flds, STW_RX_DMA_FLD_BSL_SEG_A);

	return 0;
}

s32 rx_dma_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);

	return 0;
}
