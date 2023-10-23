/*
 * rpb_debugfs.h
 * Description: PP receive packet buffer debugfs interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/types.h>
#include <linux/pp_api.h>
#include "pp_logger.h"
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_misc.h"
#include "rpb.h"
#include "rpb_internal.h"

static struct dentry *dbgfs;
struct buf_fld_info rpb_stw_flds[STW_RPB_FLDS_NUM];

struct pp_hw_stw_rpb {
	u32 word[STW_RPB_WORD_CNT];
};

struct pp_stw_rpb_values {
	u32 val[STW_RPB_FLDS_NUM];
};

s32 pp_stw_rpb_get(struct pp_stw_rpb_values *stw_rpb_vals)
{
	struct pp_hw_stw_rpb stw_hw;
	u32 fld_idx;
	s32 reg_idx;
	u32 stw_hw_idx = 0;

	if (unlikely(ptr_is_null(stw_rpb_vals)))
		return -EINVAL;

	/* Read RPB STW backwards */
	for (reg_idx = STW_RPB_WORD_CNT - 1; reg_idx >= 0 ; reg_idx--) {
		stw_hw.word[stw_hw_idx] =
			PP_REG_RD32(PP_RPB_STW_W0_REG + (4 * reg_idx));
		stw_hw_idx++;
	}

	for (fld_idx = 0 ; fld_idx < STW_RPB_FLDS_NUM; fld_idx++) {
		stw_rpb_vals->val[fld_idx] =
			buf_fld_get(&rpb_stw_flds[fld_idx], (void *)(&stw_hw));
	}

	return 0;
}

/**
 * @brief Debugfs status attribute show API, print log level and
 *        components status (enable/disable)
 * @param f seq_file
 * @param offset
 * @return int
 */
void __rpb_cfg_show(struct seq_file *f)
{
	struct rpb_profile_cfg cfg;
	u32 dma_th, dma_to, port, tc, mem_sz, max_pending_pkts;
	u64 bmgr_addr;
	s32 ret = 0;

	ret |= rpb_dma_coal_threshold_get(&dma_th);
	ret |= rpb_dma_coal_timeout_get(&dma_to);
	ret |= rpb_bmgr_addr_get(&bmgr_addr);
	ret |= rpb_profile_cfg_get(&cfg);
	ret |= rpb_memory_sz_get(&mem_sz);
	ret |= rpb_max_pending_pkts_get(&max_pending_pkts);
	if (unlikely(ret))
		return;

	seq_puts(f, "\n");
	seq_puts(f, "|========================================|\n");
	seq_puts(f, "|  Receive Packet Buffer Configuration   |\n");
	seq_puts(f, "|========================================|\n");
	seq_printf(f, "|  %-25s = %-8llx  |\n", "Buffer Manager Base",
		   bmgr_addr);
	seq_printf(f, "|  %-25s = %-8u  |\n", "DMA Coalescing Threshold",
		   dma_th);
	seq_printf(f, "|  %-25s = %-8u  |\n", "DMA Coalescing Timeout",
		   dma_to);
	seq_printf(f, "|  %-25s = %-8u  |\n", "Memory Size (bytes)",
		   mem_sz);
	seq_printf(f, "|  %-25s = %-8u  |\n", "Bytes High Threshold",
		   cfg.high_thr);
	seq_printf(f, "|  %-25s = %-8u  |\n", "Bytes Low Threshold",
		   cfg.low_thr);
	seq_printf(f, "|  %-25s = %-8u  |\n", "Max Pending Packets",
		   max_pending_pkts);
	seq_printf(f, "|  %-25s = %-8u  |\n", "Packets High Threshold",
		   cfg.pkt_high_thr);
	seq_printf(f, "|  %-25s = %-8u  |\n", "Packets Low Threshold",
		   cfg.pkt_low_thr);

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_puts(f, "========================================|");
	seq_puts(f, "\n");
	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  Port%u %32s|", port, "");
	seq_puts(f, "\n");
	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_puts(f, "========================================|");
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-8u  |", "Shared Pool Size",
			   cfg.port[port].pool_sz);
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-8u  |", "Bytes High Threshold",
			   cfg.port[port].high_thr);
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-8u  |", "Bytes Low Threshold",
			   cfg.port[port].low_thr);
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-8u  |", "Mapped PP Ports",
			   cfg.port[port].num_of_pp_ports);
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-9s |", "Port Type",
			   cfg.port[port].is_mem ?
			   "MEMORY" : "STREAMING");
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_puts(f, "----------------------------------------+");
	seq_puts(f, "\n");

	RPB_FOR_EACH_TC(tc) {
		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-8s  |",
				   tc, "Flow Control",
				   (cfg.port[port].tc[tc].flow_ctrl ?
				    "Enabled" : "Disabled"));
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-8s  |",
				   tc, "High Priority",
				   (cfg.port[port].tc[tc].hi_prio ?
				    "Enabled" : "Disabled"));
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-8u  |",
				   tc, "Dedicated Pool Size",
				   cfg.port[port].tc[tc].dpool_sz);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-8u  |",
				   tc, "Bytes Dpool High Thr",
				   cfg.port[port].tc[tc].dpool_high_thr);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-8u  |",
				   tc, "Bytes Dpool Low Thr",
				   cfg.port[port].tc[tc].dpool_low_thr);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-8u  |",
				   tc, "Bytes Spool High Thr",
				   cfg.port[port].tc[tc].spool_high_thr);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-8u  |",
				   tc, "Bytes Spool Low Thr",
				   cfg.port[port].tc[tc].spool_low_thr);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_puts(f, "----------------------------------------+");
		seq_puts(f, "\n");
	}
	seq_puts(f, "\n\n");
}

PP_DEFINE_DEBUGFS(cfg_show, __rpb_cfg_show, NULL);

/**
 * @brief Debugfs status attribute show API, print log level and
 *        components status (enable/disable)
 * @param f seq_file
 * @param offset
 * @return int
 */
void __rpb_stats_show(struct seq_file *f)
{
	struct rpb_hw_stats stats;
	u32 port, tc, reg, free_clids, free_cc;
	s32 ret = 0;
	bool port_back_pres;

	ret = rpb_hw_stats_get(&stats);
	if (unlikely(ret))
		return;

	reg = PP_REG_RD32(PP_RPB_GEN_DBG_CNT_REG);
	free_clids = PP_FIELD_GET(PP_RPB_GEN_DBG_CNT_FREE_CLIDS_MSK, reg);
	free_cc    = PP_FIELD_GET(PP_RPB_GEN_DBG_CNT_FREE_CC_MSK, reg);

	seq_puts(f, "\n");
	seq_puts(f, "   Receive Packet Buffer Statistics\n");
	seq_puts(f, "|==========================================|\n");
	seq_printf(f, "|  %-25s = %-10u  |\n", "Global Bytes Used",
		   stats.total_bytes_used);
	seq_printf(f, "|  %-25s = %-10u  |\n", "Global Packets Used",
		   stats.total_pkts_used);
	seq_printf(f, "|  %-25s = %-10u  |\n", "Free CLIDs",
		   free_clids);
	seq_printf(f, "|  %-25s = %-10u  |\n", "Free CCs",
		   free_cc);

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_puts(f, "==========================================|");
	seq_puts(f, "\n");
	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  Port%u %34s|", port, "");
	seq_puts(f, "\n");
	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_puts(f, "==========================================|");
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-10u  |", "RX Packets",
			   stats.port[port].rx_packets);
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-10u  |", "Total Bytes Used",
			   stats.port[port].total_bytes_used);
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_printf(f, "  %-25s = %-10u  |", "Shared Bytes Used",
			   stats.port[port].spool_bytes_used);
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port) {
		ret = rpb_port_back_pressure_status_get(port, &port_back_pres);
		if (unlikely(ret))
			pr_err("Failed to get port%u back pressure, ret %d\n",
			       port, ret);

		seq_printf(f, "  %-25s = %-10s  |", "Back Pressure",
			   port_back_pres ? "xOff" : "xOn");
	}
	seq_puts(f, "\n");

	seq_puts(f, "|");
	RPB_FOR_EACH_PORT(port)
		seq_puts(f, "------------------------------------------+");
	seq_puts(f, "\n");

	RPB_FOR_EACH_TC(tc) {
		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-10u  |",
				   tc, "Dedicated Bytes Used",
				   stats.port[port].tc[tc].dpool_bytes_used);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-10u  |",
				   tc, "Shared Bytes Used",
				   stats.port[port].tc[tc].spool_bytes_used);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-10u  |",
				   tc, "Dropped bytes",
				   stats.port[port].tc[tc].drop_bytes);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_printf(f, "  TC%u %-21s = %-10u  |",
				   tc, "Dropped Packets",
				   stats.port[port].tc[tc].drop_packets);
		seq_puts(f, "\n");

		seq_puts(f, "|");
		RPB_FOR_EACH_PORT(port)
			seq_puts(f, "------------------------------------------+");
		seq_puts(f, "\n");
	}
	seq_puts(f, "\n\n");
}

PP_DEFINE_DEBUGFS(stats_show, __rpb_stats_show, NULL);

/**
 * @brief Dump status word
 * @param f seq_file
 * @param offset
 * @return int
 */
void __rpb_stw_show(struct seq_file *f)
{
	struct pp_stw_rpb_values    rpb_vals;
	struct buf_fld_info        *fld;
	u32 fld_idx, val;
	s32 ret = 0;

	ret = pp_stw_rpb_get(&rpb_vals);
	if (unlikely(ret))
		return;

	seq_puts(f, " RPB Status Word:\n");
	seq_puts(f, " ================\n");
	for (fld_idx = 0 ; fld_idx < STW_RPB_FLDS_NUM ; fld_idx++) {
		fld = &rpb_stw_flds[fld_idx];
		val = rpb_vals.val[fld_idx];

		if (fld->msb == fld->lsb)
			seq_printf(f, " %-35s: %s\n",
				   fld->desc, BOOL2STR(val));
		else
			seq_printf(f, " %-35s: %u [%#x]\n",
				   fld->desc, val, val);
	}
}

PP_DEFINE_DEBUGFS(stw_show, __rpb_stw_show, NULL);

/**
 * @brief Debugfs TC dedicated pool set API
 * @param data port and tc mapping
 * @param val dedicated pool new size
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_dpool_sz_set(void *data, u64 val)
{
	u32 port, tc;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	return rpb_tc_dpool_sz_set(port, tc, (u32)val);
}

/**
 * @brief Debugfs TC dedicated pool get API
 * @param data port and tc mapping
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_dpool_sz_get(void *data, u64 *val)
{
	u32 port, tc, sz = 0;
	s32 ret;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	ret  = rpb_tc_dpool_sz_get(port, tc, &sz);
	*val = (u64)sz;
	return ret;
}

/**
 * @brief Debugfs TC high priority set API
 * @param data port and tc mapping
 * @param val high priority new value
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_high_prio_set(void *data, u64 val)
{
	u32 port, tc;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	return rpb_tc_high_prio_set(port, tc, val ? true : false);
}

/**
 * @brief Debugfs TC high priority get API
 * @param data port and tc mapping
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_high_prio_get(void *data, u64 *val)
{
	u32 port, tc;
	s32 ret;
	bool high_prio = false;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	ret  = rpb_tc_high_prio_get(port, tc, &high_prio);
	*val = (u64)high_prio;
	return ret;
}

/**
 * @brief Debugfs TC flow control set API
 * @param data port and tc mapping
 * @param val enable/disable flow control
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_flow_ctrl_set(void *data, u64 val)
{
	u32 port, tc;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	return rpb_tc_flow_ctrl_set(port, tc, val ? true : false);
}

/**
 * @brief Debugfs TC flow control get API
 * @param data port and tc mapping
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_flow_ctrl_get(void *data, u64 *val)
{
	u32 port, tc;
	s32 ret;
	bool flow_ctrl = false;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	ret  = rpb_tc_flow_ctrl_get(port, tc, &flow_ctrl);
	*val = (u64)flow_ctrl;
	return ret;
}

/**
 * @brief Debugfs TC high threshold set API
 * @param data port and tc mapping
 * @param val high threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_dpool_high_thr_set(void *data, u64 val)
{
	u32 port, tc;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	return rpb_tc_dpool_high_threshold_set(port, tc, (u32)val);
}

/**
 * @brief Debugfs TC high threshold get API
 * @param data port and tc mapping
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_dpool_high_thr_get(void *data, u64 *val)
{
	u32 port, tc, thr = 0;
	s32 ret;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	ret  = rpb_tc_dpool_high_threshold_get(port, tc, &thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs TC high threshold set API
 * @param data port and tc mapping
 * @param val high threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_dpool_low_thr_set(void *data, u64 val)
{
	u32 port, tc;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	return rpb_tc_dpool_low_threshold_set(port, tc, (u32)val);
}

/**
 * @brief Debugfs TC high threshold get API
 * @param data port and tc mapping
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_dpool_low_thr_get(void *data, u64 *val)
{
	u32 port, tc, thr = 0;
	s32 ret;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	ret  = rpb_tc_dpool_low_threshold_get(port, tc, &thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs TC high threshold set API
 * @param data port and tc mapping
 * @param val high threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_spool_high_thr_set(void *data, u64 val)
{
	u32 port, tc;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	return rpb_tc_spool_high_threshold_set(port, tc, (u32)val);
}

/**
 * @brief Debugfs TC high threshold get API
 * @param data port and tc mapping
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_spool_high_thr_get(void *data, u64 *val)
{
	u32 port, tc, thr = 0;
	s32 ret;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	ret  = rpb_tc_spool_high_threshold_get(port, tc, &thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs TC low threshold set API
 * @param data port and tc mapping
 * @param val high threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_spool_low_thr_set(void *data, u64 val)
{
	u32 port, tc;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	return rpb_tc_spool_low_threshold_set(port, tc, (u32)val);
}

/**
 * @brief Debugfs TC low threshold get API
 * @param data port and tc mapping
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_tc_spool_low_thr_get(void *data, u64 *val)
{
	u32 port, tc, thr = 0;
	s32 ret;

	port = ((unsigned long)data) / PP_MAX_RPB_PORT;
	tc   = ((unsigned long)data) % PP_MAX_RPB_PORT;
	ret  = rpb_tc_spool_low_threshold_get(port, tc, &thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs port shared pool set API
 * @param data RPB port id
 * @param val shared pool new size
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_port_spool_sz_set(void *data, u64 val)
{
	return rpb_port_spool_sz_set((unsigned long)data, (u32)val);
}

/**
 * @brief Debugfs port shared pool get API
 * @param data RPB port id
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_port_spool_sz_get(void *data, u64 *val)
{
	u32 sz = 0;
	s32 ret;

	ret  = rpb_port_spool_sz_get((unsigned long)data, &sz);
	*val = (u64)sz;
	return ret;
}

/**
 * @brief Debugfs port high threshold set API
 * @param data RPB port id
 * @param val high threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_port_high_thr_set(void *data, u64 val)
{
	return rpb_port_high_threshold_set((unsigned long)data, (u32)val);
}

/**
 * @brief Debugfs port high threshold get API
 * @param data RPB port id
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_port_high_thr_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_port_high_threshold_get((unsigned long)data, &thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs port low threshold set API
 * @param data RPB port id
 * @param val low threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_port_low_thr_set(void *data, u64 val)
{
	return rpb_port_low_threshold_set((unsigned long)data, (u32)val);
}

/**
 * @brief Debugfs port low threshold get API
 * @param data RPB port id
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_port_low_thr_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_port_low_threshold_get((unsigned long)data, &thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs global high threshold set API
 * @param data not used
 * @param val high threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_high_thr_set(void *data, u64 val)
{
	return rpb_glb_high_threshold_set((u32)val);
}

/**
 * @brief Debugfs global high threshold get API
 * @param data not used
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_high_thr_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_glb_high_threshold_get(&thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs global low threshold set API
 * @param data not used
 * @param val low threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_low_thr_set(void *data, u64 val)
{
	return rpb_glb_low_threshold_set((u32)val);
}

/**
 * @brief Debugfs global low threshold get API
 * @param data not used
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_low_thr_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_glb_low_threshold_get(&thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs global packets high threshold set API
 * @param data not used
 * @param val high threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_pkts_high_thr_set(void *data, u64 val)
{
	return rpb_glb_pkts_high_threshold_set((u32)val);
}

/**
 * @brief Debugfs global packets high threshold get API
 * @param data not used
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_pkts_high_thr_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_glb_pkts_high_threshold_get(&thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs global packets low threshold set API
 * @param data not used
 * @param val low threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_pkts_low_thr_set(void *data, u64 val)
{
	return rpb_glb_pkts_low_threshold_set((u32)val);
}

/**
 * @brief Debugfs global packets low threshold get API
 * @param data not used
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_glb_pkts_low_thr_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_glb_pkts_low_threshold_get(&thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs dma coalescing threshold set API
 * @param data not used
 * @param val low threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_dma_coal_thr_set(void *data, u64 val)
{
	return rpb_dma_coal_threshold_set((u32)val);
}

/**
 * @brief Debugfs dma coalescing threshold get API
 * @param data not used
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_dma_coal_thr_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_dma_coal_threshold_get(&thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs dma coalescing timeout set API
 * @param data not used
 * @param val low threshold
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_dma_coal_to_set(void *data, u64 val)
{
	return rpb_dma_coal_timeout_set((u32)val);
}

/**
 * @brief Debugfs dma coalescing timeout get API
 * @param data not used
 * @param val
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_dma_coal_to_get(void *data, u64 *val)
{
	u32 thr = 0;
	s32 ret;

	ret  = rpb_dma_coal_timeout_get(&thr);
	*val = (u64)thr;
	return ret;
}

/**
 * @brief Debugfs RPB pofile set API
 * @param data unused
 * @param val profile id
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_profile_id_set(void *data, u64 val)
{
	return rpb_profile_set((enum rpb_profile_id)val);
}

/**
 * @brief Debugfs RPB profile get API
 * @param data unused
 * @param val profile id
 * @return 0 on success, non-zero value otherwise
 */
static int __rpb_profile_id_get(void *data, u64 *val)
{
	enum rpb_profile_id id = RPB_PROFILE_NUM;
	s32 ret;

	ret  = rpb_profile_id_get(&id);
	*val = (u64)id;
	return ret;
}

PP_DEFINE_DBGFS_ATT(tc_dpool_sz, __rpb_tc_dpool_sz_get, __rpb_tc_dpool_sz_set);
PP_DEFINE_DBGFS_ATT(tc_high_prio, __rpb_tc_high_prio_get,
		    __rpb_tc_high_prio_set);
PP_DEFINE_DBGFS_ATT(tc_flow_ctrl, __rpb_tc_flow_ctrl_get,
		    __rpb_tc_flow_ctrl_set);
PP_DEFINE_DBGFS_ATT(tc_dpool_high_thr, __rpb_tc_dpool_high_thr_get,
		    __rpb_tc_dpool_high_thr_set);
PP_DEFINE_DBGFS_ATT(tc_dpool_low_thr, __rpb_tc_dpool_low_thr_get,
		    __rpb_tc_dpool_low_thr_set);
PP_DEFINE_DBGFS_ATT(tc_spool_high_thr, __rpb_tc_spool_high_thr_get,
		    __rpb_tc_spool_high_thr_set);
PP_DEFINE_DBGFS_ATT(tc_spool_low_thr, __rpb_tc_spool_low_thr_get,
		    __rpb_tc_spool_low_thr_set);
PP_DEFINE_DBGFS_ATT(port_spool_sz, __rpb_port_spool_sz_get,
		    __rpb_port_spool_sz_set);
PP_DEFINE_DBGFS_ATT(port_high_thr, __rpb_port_high_thr_get,
		    __rpb_port_high_thr_set);
PP_DEFINE_DBGFS_ATT(port_low_thr, __rpb_port_low_thr_get,
		    __rpb_port_low_thr_set);

PP_DEFINE_DBGFS_ATT(glb_high_thr, __rpb_glb_high_thr_get,
		    __rpb_glb_high_thr_set);
PP_DEFINE_DBGFS_ATT(glb_low_thr, __rpb_glb_low_thr_get, __rpb_glb_low_thr_set);
PP_DEFINE_DBGFS_ATT(glb_pkts_high_thr, __rpb_glb_pkts_high_thr_get,
		    __rpb_glb_pkts_high_thr_set);
PP_DEFINE_DBGFS_ATT(glb_pkts_low_thr, __rpb_glb_pkts_low_thr_get,
		    __rpb_glb_pkts_low_thr_set);
PP_DEFINE_DBGFS_ATT(dma_coal_thr, __rpb_dma_coal_thr_get,
		    __rpb_dma_coal_thr_set);
PP_DEFINE_DBGFS_ATT(dma_coal_to, __rpb_dma_coal_to_get, __rpb_dma_coal_to_set);
PP_DEFINE_DBGFS_ATT(rpb_profile, __rpb_profile_id_get, __rpb_profile_id_set);

static struct debugfs_file rpb_dbg_files[] = {
	{ "high_threshold", &PP_DEBUGFS_FOPS(glb_high_thr) },
	{ "low_threshold", &PP_DEBUGFS_FOPS(glb_low_thr) },
	{ "pkts_high_threshold", &PP_DEBUGFS_FOPS(glb_pkts_high_thr) },
	{ "pkts_low_threshold", &PP_DEBUGFS_FOPS(glb_pkts_low_thr) },
	{ "dma_coal_threshold", &PP_DEBUGFS_FOPS(dma_coal_thr) },
	{ "dma_coal_timeout", &PP_DEBUGFS_FOPS(dma_coal_to) },
	{ "rpb_profile", &PP_DEBUGFS_FOPS(rpb_profile) },
	{ "stats", &PP_DEBUGFS_FOPS(stats_show) },
	{ "config", &PP_DEBUGFS_FOPS(cfg_show) },
	{ "stw_show", &PP_DEBUGFS_FOPS(stw_show) },
};

static struct debugfs_file ports_files[] = {
	{ "spool_size", &PP_DEBUGFS_FOPS(port_spool_sz) },
	{ "high_threshold", &PP_DEBUGFS_FOPS(port_high_thr) },
	{ "low_threshold", &PP_DEBUGFS_FOPS(port_low_thr) },
};

static struct debugfs_file tcs_files[] = {
	{ "dpool_size", &PP_DEBUGFS_FOPS(tc_dpool_sz) },
	{ "high_prio", &PP_DEBUGFS_FOPS(tc_high_prio) },
	{ "flow_control", &PP_DEBUGFS_FOPS(tc_flow_ctrl) },
	{ "dpool_high_threshold", &PP_DEBUGFS_FOPS(tc_dpool_high_thr) },
	{ "dpool_low_threshold", &PP_DEBUGFS_FOPS(tc_dpool_low_thr) },
	{ "spool_high_threshold", &PP_DEBUGFS_FOPS(tc_spool_high_thr) },
	{ "spool_low_threshold", &PP_DEBUGFS_FOPS(tc_spool_low_thr) },
};

static s32 rpb_dbg_tc_init(u32 port, u32 tc, struct dentry *port_dbgfs)
{
	struct dentry *tc_dbgfs = NULL;
	unsigned long tc_port_mapping;
	char buf[32];
	s32 ret;

	if (!port_dbgfs || !__rpb_is_port_valid(port) || !__rpb_is_tc_valid(tc))
		return -EINVAL;

	scnprintf(buf, sizeof(buf), "tc%u", tc);
	tc_port_mapping = (unsigned long)((port * PP_MAX_RPB_PORT) + tc);

	ret = pp_debugfs_create(port_dbgfs, buf, &tc_dbgfs, tcs_files,
				ARRAY_SIZE(tcs_files), (void *)tc_port_mapping);
	if (unlikely(ret))
		pr_err("Failed to create the %s debugfs dir\n", buf);

	return ret;
}

static s32 rpb_dbg_port_init(u32 port, struct dentry *rpb_dbgfs)
{
	struct dentry *port_dbgfs = NULL;
	char buf[32];
	unsigned long id = (unsigned long)port;
	u32 tc, ret;

	if (!rpb_dbgfs || !__rpb_is_port_valid(port))
		return -EINVAL;

	scnprintf(buf, sizeof(buf), "port%u", port);

	ret = pp_debugfs_create(rpb_dbgfs, buf, &port_dbgfs, ports_files,
				ARRAY_SIZE(ports_files),  (void *)id);
	if (unlikely(ret)) {
		pr_err("Failed to create the %s debugfs dir\n", buf);
		return ret;
	}

	RPB_FOR_EACH_TC(tc) {
		ret = rpb_dbg_tc_init(port, tc, port_dbgfs);
		if (unlikely(ret))
			return ret;
	}

	return 0;
}

s32 rpb_dbg_init(struct dentry *parent)
{
	u32 port;
	s32 ret;

	/* RPB debugfs dir */
	ret = pp_debugfs_create(parent, "rpb", &dbgfs, rpb_dbg_files,
				ARRAY_SIZE(rpb_dbg_files), NULL);
	if (unlikely(ret)) {
		pr_err("Error: Failed to create the RPB debugfs dir\n");
		return ret;
	}

	RPB_FOR_EACH_PORT(port) {
		ret = rpb_dbg_port_init(port, dbgfs);
		if (unlikely(ret))
			return ret;
	}

	/* Init status word common fields */
	STW_INIT_COMMON_FLDS(rpb_stw_flds);
	/* Init rpb specific status word fields */
	BUF_FLD_INIT_DESC(rpb_stw_flds, STW_RPB_FLD_ERR_IND);
	BUF_FLD_INIT_DESC(rpb_stw_flds, STW_RPB_FLD_DISCARD_IND);

	return 0;
}

s32 rpb_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	return 0;
}
