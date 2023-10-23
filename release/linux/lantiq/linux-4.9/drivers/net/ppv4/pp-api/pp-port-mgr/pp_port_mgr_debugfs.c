/*
 * pp_port_mgr_debugfs.h
 * Description: PP port manager debugfs interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/types.h>
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_buffer_mgr.h"
#include "pp_port_mgr.h"
#include "pp_port_mgr_internal.h"

/**
 * @brief main port manager debugfs dir
 */
static struct dentry *dbgfs;

/**
 * @brief ports debugfs dir
 */
static struct dentry *ports_dbgfs;

/**
 * @brief datapaths debugfs dir
 */
static struct dentry *dps_dbgfs;

/**
 * @brief per port debugfs dir
 */
static struct dentry *portx[PP_MAX_PORT];

/**
 * @brief per datapath debugfs dir
 */
static struct dentry *dpx[PMGR_HIF_DB_DP_MAX];

/**
 * @brief Debugfs stats show API, prints module statistics
 */
void __pmgr_stats_show(struct seq_file *f)
{
	struct pmgr_stats stats;

	if (unlikely(pmgr_stats_get(&stats)))
		return;

	seq_puts(f, "\n");
	seq_puts(f, "+---------------------------------------------+\n");
	seq_puts(f, "|          Port Manager Statistics            |\n");
	seq_puts(f, "+---------------------------------------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Active Ports",
		   stats.port_count);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Active Host IF DPs",
		   stats.hif_dps_count);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "NULL Error",
		   stats.null_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "HAL Error",
		   stats.hal_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port Invalid Error",
		   stats.port_invalid_err);
	seq_printf(f, "| %-30s | %-10u |\n", "Port Invalid Headroom",
		   stats.port_invalid_headroom);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port Active Error",
		   stats.port_act_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port Inactive Error",
		   stats.port_inact_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port Update Error",
		   stats.port_update_no_changes_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port Classification Error",
		   stats.port_cls_param_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port Policy Error",
		   stats.port_policy_param_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port TX cfg Error",
		   stats.port_tx_cfg_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Port RX cfg Error",
		   stats.port_rx_cfg_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Host IF datapath Error",
		   stats.hif_dp_param_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Host IF classification Error",
		   stats.hif_cls_param_err);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "Host IF datapath Port Error",
		   stats.hif_dp_port_err);
	seq_puts(f, "+--------------------------------+------------+\n");
}

PP_DEFINE_DEBUGFS(pmgr_stats, __pmgr_stats_show, NULL);

/**
 * @brief Debugfs stats show API, prints module statistics
 */
void __pmgr_ports_stats_show(struct seq_file *f)
{
	pp_stats_show_seq(f, sizeof(struct pp_stats), PP_MAX_PORT,
			  pmgr_ports_stats_get, pmgr_ports_stats_show, NULL);
}

PP_DEFINE_DEBUGFS(ports_stats, __pmgr_ports_stats_show, NULL);

/**
 * @brief Debugfs stats show API, prints module statistics
 */
void __pmgr_ports_pps_show(struct seq_file *f)
{
	pp_pps_show_seq(f, sizeof(struct pp_stats), PP_MAX_PORT,
			pmgr_ports_stats_get, pmgr_ports_stats_diff,
			pmgr_ports_stats_show, NULL);
}

PP_DEFINE_DEBUGFS(ports_pps, __pmgr_ports_pps_show, NULL);

/**
 * @brief Debugfs port add API, adding new pp port
 */
static void __pmgr_dbg_port_add_set(char *cmd_buf, void *data)
{
	u16 id;
	struct pp_port_cfg cfg;

	if (unlikely(sscanf(cmd_buf,
			    "%hu %hu %u %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
			    &id, &cfg.tx.base_policy,
			    &cfg.rx.parse_type, &cfg.rx.cls.n_flds,
			    &cfg.rx.cls.cp[0].stw_off,
			    &cfg.rx.cls.cp[0].copy_size,
			    &cfg.rx.cls.cp[1].stw_off,
			    &cfg.rx.cls.cp[1].copy_size,
			    &cfg.rx.cls.cp[2].stw_off,
			    &cfg.rx.cls.cp[2].copy_size,
			    &cfg.rx.cls.cp[3].stw_off,
			    &cfg.rx.cls.cp[3].copy_size) != 12)) {
		pr_err("sscanf error\n");
		return;
	}

	cfg.rx.flow_ctrl_en = true;
	cfg.rx.mem_port_en = false;
	cfg.rx.policies_map = 0;
	cfg.tx.headroom_size = 64;
	cfg.tx.tailroom_size = 32;
	cfg.tx.max_pkt_size = 4000;
	cfg.tx.min_pkt_len = 0;
	cfg.tx.pkt_only_en = false;
	cfg.tx.policies_map = 0xF;
	cfg.tx.seg_en = false;
	cfg.tx.wr_desc = true;

	if (unlikely(pp_port_add(id, &cfg))) {
		pr_err("failed to add port %hu\n", id);
		return;
	}

	pr_info("PORT[%hu] ADDED\n", id);
}

static void __pmgr_dbg_port_add_help(struct seq_file *f)
{
	seq_puts(f, " <port-id>\n");
	seq_puts(f, " <base-policy>\n");
	seq_puts(f, " <parsing [0=L2_PARSE, 1=IP_PARSE, 2=IP_PARSE]>\n");
	seq_puts(f, " <cls-number-of-fields ,[up to 4]>\n");
	seq_puts(f, " <cls-field0 stw-off    [bits]\n");
	seq_puts(f, " <cls-field0 szie       [bits]\n");
	seq_puts(f, " <cls-field1 stw-off    [bits]\n");
	seq_puts(f, " <cls-field1 szie       [bits]\n");
	seq_puts(f, " <cls-field2 stw-off    [bits]\n");
	seq_puts(f, " <cls-field2 szie       [bits]\n");
	seq_puts(f, " <cls-field3 stw-off    [bits]\n");
	seq_puts(f, " <cls-field3 szie       [bits]\n");
}

PP_DEFINE_DEBUGFS(port_add, __pmgr_dbg_port_add_help, __pmgr_dbg_port_add_set);

/**
 * @brief Debugfs stats show API, prints module statistics
 */
void __pmgr_dp_stats_show(struct seq_file *f)
{
	struct pp_stats stats;
	u32 dp_idx = (unsigned long)f->private;
	s32 ret;

	seq_puts(f, "\n");
	seq_puts(f, "+----------+--------------+--------------+\n");
	seq_puts(f, "|    DP    |   Packets    |    Bytes     |\n");
	seq_puts(f, "+----------+--------------+--------------+\n");
	memset(&stats, 0, sizeof(stats));
	ret = chk_exception_stats_get(dp_idx, &stats);
	if (ret) {
		pr_err("Error fetching dp %u stats\n", dp_idx);
		return;
	}

	seq_printf(f, "| %-7u  | %-12llu | %-12llu |\n",
		   dp_idx, stats.packets, stats.bytes);
	seq_puts(f, "+----------+--------------+--------------+\n");
}

PP_DEFINE_DEBUGFS(dp_stats, __pmgr_dp_stats_show, NULL);

/**
 * @brief Debugfs stats show API, prints module statistics
 */
void __pmgr_dps_stats_show(struct seq_file *f)
{
	struct pp_stats stats;
	u32 dp_idx;
	s32 ret;

	seq_puts(f, "\n");
	seq_puts(f, "+----------+--------------+--------------+\n");
	seq_puts(f, "|    DP    |   Packets    |    Bytes     |\n");
	seq_puts(f, "+----------+--------------+--------------+\n");

	for (dp_idx = 0; dp_idx < CHK_NUM_EXCEPTION_SESSIONS; dp_idx++) {
		memset(&stats, 0, sizeof(stats));
		ret = chk_exception_stats_get(dp_idx, &stats);
		if (ret) {
			pr_err("Error fetching dp %u stats\n", dp_idx);
			return;
		}

		if (!stats.packets)
			continue;

		seq_printf(f, "| %-7u  | %-12llu | %-12llu |\n",
			   dp_idx, stats.packets, stats.bytes);
	}
	seq_puts(f, "+----------+--------------+--------------+\n");
}

PP_DEFINE_DEBUGFS(dps_stats, __pmgr_dps_stats_show, NULL);

/**
 * @brief Debugfs port show API, prints port configuration
 */
void __pmgr_dbg_port_cfg_show(struct seq_file *f)
{
	struct pp_port_cfg cfg;
	u32 i, p = (unsigned long)f->private;
	char buf[64];
	u32 rpb_port;

	if (unlikely(pp_port_get(p, &cfg)))
		return;

	/* Print Port Configuration */
	seq_puts(f, "\n");
	seq_puts(f, "+---------------------------------------------+\n");
	seq_printf(f, "|          Port %-3u Configuration             |\n", p);
	seq_puts(f, "+---------------------------------------------+\n");
	seq_printf(f, "| %-30s | %-10hhu |\n", "Classification fields",
		   cfg.rx.cls.n_flds);
	seq_puts(f, "+--------------------------------+------------+\n");

	for (i = 0; i < cfg.rx.cls.n_flds; i++) {
		scnprintf(buf, sizeof(buf), "fld[%u] Cls stw offset", i);
		seq_printf(f, "| %-30s | %-10hhu |\n",
			   buf, cfg.rx.cls.cp[i].stw_off);
		seq_puts(f, "+--------------------------------+------------+\n");
		scnprintf(buf, sizeof(buf), "fld[%u] Cls field size", i);
		seq_printf(f, "| %-30s | %-10hhu |\n",
			   buf, cfg.rx.cls.cp[i].copy_size);
		seq_puts(f, "+--------------------------------+------------+\n");
	}

	seq_printf(f, "| %-30s | %-10s |\n", "Port type",
		   cfg.rx.mem_port_en ? "MEMORY" : "STREAM");
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10s |\n", "Flow control",
		   BOOL2EN(cfg.rx.flow_ctrl_en));
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10s |\n", "Parsing Type",
		   (cfg.rx.parse_type == L2_PARSE ? "L2" :
		    (cfg.rx.parse_type == IP_PARSE ? "IP" : "None")));
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-#10x |\n", "Rx Policies Map",
		   cfg.rx.policies_map);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10hu |\n", "Max packet size",
		   cfg.tx.max_pkt_size);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10hu |\n", "Min packet size",
		   (u16)cfg.tx.min_pkt_len);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10hu |\n", "Headroom size",
		   cfg.tx.headroom_size);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10hu |\n", "Tailroom size",
		   cfg.tx.tailroom_size);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10u |\n", "TX Base Policy",
		   cfg.tx.base_policy);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-#10x |\n", "TX Policies Map",
		   cfg.tx.policies_map);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10s |\n", "Packet only mode",
		   BOOL2STR(cfg.tx.pkt_only_en));
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10s |\n", "Segmentation",
		   BOOL2STR(cfg.tx.seg_en));
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10s |\n", "PreL2",
		   BOOL2STR(cfg.tx.prel2_en));
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10s |\n", "Wr Desc",
		   BOOL2STR(cfg.tx.wr_desc));
	seq_puts(f, "+--------------------------------+------------+\n");
	if (unlikely(pmgr_port_rpb_map_get(p, &rpb_port)))
		seq_puts(f, "  ERROR: Can't get RPB mapping\n");
	else
		seq_printf(f, "| %-30s | %-10u |\n", "RPB port mapping",
			   rpb_port);
	seq_puts(f, "+--------------------------------+------------+\n");
}

PP_DEFINE_DEBUGFS(port_cfg, __pmgr_dbg_port_cfg_show, NULL);

static u16 create_port_props(struct pp_dbg_prop *props, u16 size,
			     u32 *id, struct pp_port_cfg *pconf)
{
	u16 num = 0;

	pp_dbg_add_prop(props, &num, size, "id",
			"id. Must exist as the first property!", id,
			sizeof(*id));
	pp_dbg_add_prop(props, &num, size, "max_pkt", "Max packet size",
			&pconf->tx.max_pkt_size,
			sizeof(pconf->tx.max_pkt_size));
	pp_dbg_add_prop(props, &num, size, "hr", "Headroom size",
			&pconf->tx.headroom_size,
			sizeof(pconf->tx.headroom_size));
	pp_dbg_add_prop(props, &num, size, "tr", "Tailroom size",
			&pconf->tx.tailroom_size,
			sizeof(pconf->tx.tailroom_size));
	pp_dbg_add_prop(props, &num, size, "min_pkt",
			"min pkt len (0 - None, 1 - 60B , 2 - 64B, 3 - 128B)",
			&pconf->tx.min_pkt_len,
			sizeof(pconf->tx.min_pkt_len));
	pp_dbg_add_prop(props, &num, size, "base_policy", "Base policy",
			&pconf->tx.base_policy,
			sizeof(pconf->tx.base_policy));
	pp_dbg_add_prop(props, &num, size, "policies_map", "Policies map",
			&pconf->tx.policies_map,
			sizeof(pconf->tx.policies_map));
	pp_dbg_add_prop(props, &num, size, "pkt_only_en", "Pkt only en",
			&pconf->tx.pkt_only_en,
			sizeof(pconf->tx.pkt_only_en));
	pp_dbg_add_prop(props, &num, size, "seg_en", "Segmentation en",
			&pconf->tx.seg_en,
			sizeof(pconf->tx.seg_en));
	pp_dbg_add_prop(props, &num, size, "prel2_en", "Prel2 en",
			&pconf->tx.prel2_en,
			sizeof(pconf->tx.prel2_en));
	pp_dbg_add_prop(props, &num, size, "wr_desc", "Write Descriptor",
			&pconf->tx.wr_desc,
			sizeof(pconf->tx.wr_desc));

	return num;
}

static s32 port_first_prop_cb(char *field, u32 val, void *user_data)
{
	/* Make sure first property is the port id */
	if (strncmp(field, "id", strlen("id"))) {
		pr_err("First prop (%s) must be id\n", field);
		return -EINVAL;
	}

	if (pp_port_get(val, (struct pp_port_cfg *)user_data) != 0) {
		pr_err("pp_port_get failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

static s32 port_done_props_cb(u32 val, void *user_data)
{
	if (pp_port_update(val, (struct pp_port_cfg *)user_data) != 0) {
		pr_err("pp_port_update failed (id %u)", val);
		return -EINVAL;
	}

	return 0;
}

void ports_cfg_set(char *cmd_buf, void *data)
{
	struct pp_port_cfg conf;
	struct pp_dbg_props_cbs cbs = {port_first_prop_cb, port_done_props_cb};
	u32 id = UINT_MAX;
	u16 num_props;
	struct pp_dbg_prop *props;

	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct pp_dbg_prop),
			      GFP_KERNEL | __GFP_ZERO);
	if (!props)
		return;

	num_props = create_port_props(props, DBG_MAX_PROPS, &id, &conf);
	pp_dbg_props_set(cmd_buf, &cbs, props, num_props, &conf);
	kfree(props);
}

void ports_cfg_help(struct seq_file *f)
{
	struct pp_port_cfg conf;
	const char *name = "set port cfg";
	const char *format = "echo id=[port_id] [field]=[value]... > config";
	struct pp_dbg_prop *props;
	u16 num_props;
	u32 id = UINT_MAX;

	props = kmalloc_array(DBG_MAX_PROPS, sizeof(struct pp_dbg_prop),
			      GFP_KERNEL);
	if (!props)
		return;

	num_props = create_port_props(props, DBG_MAX_PROPS, &id, &conf);
	pp_dbg_props_help(f, name, format, props, num_props);
	kfree(props);
}

PP_DEFINE_DEBUGFS(ports_cfg, ports_cfg_help, ports_cfg_set);

/**
 * @brief Debugfs port stat show API, prints port stats
 */
void __pmgr_dbg_port_stats_show(struct seq_file *f)
{
	u32 p = (unsigned long)f->private;
	s32 ret;
	struct pp_stats stats;

	/* Print Port Statistics */
	seq_puts(f, "\n");

	ret = pp_port_stats_get(p, &stats);
	if (unlikely(ret)) {
		seq_printf(f, " Port %u RX stats not available:\n\n", p);
		return;
	}

	seq_puts(f, "+---------------------------------------------+\n");
	seq_printf(f, "|          Port %-3u RX Statistics             |\n", p);
	seq_puts(f, "+---------------------------------------------+\n");
	seq_printf(f, "| %-30s | %-10llu |\n", "packets", stats.packets);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10llu |\n", "bytes", stats.bytes);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_puts(f, " For TX port statistics use QoS dbgfs\n\n");
}

PP_DEFINE_DEBUGFS(port_stats, __pmgr_dbg_port_stats_show, NULL);

/**
 * @brief Debugfs port hostif show API, prints port hostif
 */
void __pmgr_dbg_port_hostif_show(struct seq_file *f)
{
	u32 p = (unsigned long)f->private;
	u8 tc, hoh, hostif;

	/* Print Port host interface mapping */
	seq_puts(f, "\n");
	seq_puts(f, "+--------------------------------------------+\n");
	seq_printf(f, "|        Port %-3u Host Interface Map         |\n", p);
	seq_puts(f, "+--------------------------------------------+\n");
	seq_puts(f, "|      TC      |     HoH      |    DP ID     |\n");
	seq_puts(f, "+--------------+--------------+--------------+\n");

	for (tc = 0; tc < PP_MAX_TC; tc++) {
		PMGR_HIF_FOR_EACH_DP_HOH(hoh) {
			hostif = pmgr_port_hif_map_get(p, tc, hoh);
			seq_printf(f, "| %-12hhu | %-12hhu | %-12hhu |\n",
				   tc, hoh, hostif);
			seq_puts(f, "+--------------+--------------+--------------+\n");
		}
	}
}

PP_DEFINE_DEBUGFS(port_hostif, __pmgr_dbg_port_hostif_show, NULL);

/**
 * @brief Debugfs datapath show API, prints datapath config
 * @note Used ONLY for debugfs prints
 * @param f seq_file
 * @param unused
 * @return s32 0 on success, non-zero value otherwise
 */
void __pmgr_dbg_dp_cfg_show(struct seq_file *f)
{
	struct pp_hif_datapath dp;
	u32 refc, i, dp_idx = (unsigned long)f->private;
	char buf[64];

	if (unlikely(pmgr_hif_dp_get(dp_idx, &dp)))
		return;

	/* Print Port Configuration */
	seq_puts(f, "\n");
	seq_puts(f, "+---------------------------------------------+\n");
	seq_printf(f, "|    Host Interface DP %-3u Configuration      |\n",
		   dp_idx);
	seq_puts(f, "+---------------------------------------------+\n");
	seq_printf(f, "| %-30s | %-10hu |\n", "Host PP Port", dp.eg[0].pid);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10hu |\n", "Host QoS Queue", dp.eg[0].qos_q);
	seq_puts(f, "+--------------------------------+------------+\n");
	seq_printf(f, "| %-30s | %-10s |\n", "Color",
		   PP_COLOR_TO_STR(dp.color));
	seq_puts(f, "+--------------------------------+------------+\n");

	PMGR_HIF_FOR_EACH_DP_SGC(i) {
		if (dp.sgc[i] != PP_SGC_INVALID) {
			scnprintf(buf, sizeof(buf), "SGC Counter%u", i);
			seq_printf(f, "| %-30s | %-10hu |\n", buf, dp.sgc[i]);
			seq_puts(f, "+--------------------------------+------------+\n");
		}
	}

	PMGR_HIF_FOR_EACH_DP_TBM(i) {
		if (dp.tbm[i] != PP_TBM_INVALID) {
			scnprintf(buf, sizeof(buf), "TBM Meter%u", i);
			seq_printf(f, "| %-30s | %-10hu |\n", buf, dp.tbm[i]);
			seq_puts(f, "+--------------------------------+------------+\n");
		}
	}

	if (unlikely(pmgr_hif_dp_refc_get(dp_idx, &refc)))
		seq_puts(f, "  ERROR: Can't get datapath refc\n");
	else
		seq_printf(f, "| %-30s | %-10u |\n", "DP reference count",
			   refc);
	seq_puts(f, "+---------------------------------------------+\n\n");
}

PP_DEFINE_DEBUGFS(dp_cfg, __pmgr_dbg_dp_cfg_show, NULL);

static struct debugfs_file port_debugfs_files[] = {
	{ "config", &PP_DEBUGFS_FOPS(port_cfg) },
	{ "stats", &PP_DEBUGFS_FOPS(port_stats) },
	{ "hostif", &PP_DEBUGFS_FOPS(port_hostif) },
};

static struct debugfs_file ports_debugfs_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(ports_stats) },
	{ "pps", &PP_DEBUGFS_FOPS(ports_pps) },
	{ "add", &PP_DEBUGFS_FOPS(port_add) },
	{ "config", &PP_DEBUGFS_FOPS(ports_cfg) },
};

static struct debugfs_file dp_debugfs_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(dp_stats) },
	{ "config", &PP_DEBUGFS_FOPS(dp_cfg) }
};

static struct debugfs_file dps_debugfs_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(dps_stats) },
};

static struct debugfs_file debugfs_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(pmgr_stats) }
};

static struct debugfs_file drv_debugfs_files[] = {
	{ "ports_stats", &PP_DEBUGFS_FOPS(ports_stats) },
	{ "ports_pps", &PP_DEBUGFS_FOPS(ports_pps) },
};

s32 pmgr_port_dbg_clean(u16 pid)
{
	debugfs_remove_recursive(portx[pid]);
	portx[pid] = NULL;

	return 0;
}

s32 pmgr_port_dbg_init(u16 pid)
{
	char buf[16];
	unsigned long id = (unsigned long)pid;

	if (unlikely(portx[pid])) {
		pr_info("debugfs dir exist, delete old dir for port%hu\n",
			pid);
		pmgr_port_dbg_clean(pid);
	}

	scnprintf(buf, sizeof(buf), "port%hu", pid);

	return pp_debugfs_create(ports_dbgfs, buf, &portx[pid],
				 port_debugfs_files,
				 ARRAY_SIZE(port_debugfs_files), (void *)id);
}

s32 pmgr_dp_dbg_clean(u8 dp)
{
	debugfs_remove_recursive(dpx[dp]);
	dpx[dp] = NULL;

	return 0;
}

s32 pmgr_dp_dbg_init(u8 dp)
{
	char buf[16];
	unsigned long id = (unsigned long)dp;

	if (unlikely(dpx[dp])) {
		pr_info("debugfs dir exist, delete old dir for dp%hhu\n", dp);
		pmgr_dp_dbg_clean(dp);
	}

	scnprintf(buf, sizeof(buf), "dp%hhu", dp);

	return pp_debugfs_create(dps_dbgfs, buf, &dpx[dp],
				 dp_debugfs_files,
				 ARRAY_SIZE(dp_debugfs_files), (void *)id);
}

s32 pmgr_dbg_init(struct dentry *parent)
{
	s32 ret;

	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	/* Create ports stats and pps under root directory */
	ret = pp_debugfs_create(parent, NULL, NULL, drv_debugfs_files,
				ARRAY_SIZE(drv_debugfs_files), NULL);
	if (unlikely(ret)) {
		pr_err("Failed to create ports debugfs dir\n");
		return ret;
	}

	/* Port manager debugfs dir */
	ret = pp_debugfs_create(parent, "port_mgr", &dbgfs, debugfs_files,
				ARRAY_SIZE(debugfs_files), NULL);
	if (unlikely(ret)) {
		pr_err("Failed to create the port_mgr debugfs dir\n");
		return ret;
	}

	/* Create main ports dir under port_mgr dir */
	ret = pp_debugfs_create(dbgfs, "ports", &ports_dbgfs,
				ports_debugfs_files,
				ARRAY_SIZE(ports_debugfs_files), NULL);
	if (unlikely(ret)) {
		pr_err("Failed to create ports debugfs dir\n");
		return ret;
	}

	/* Create main dps dir under port_mgr dir */
	ret = pp_debugfs_create(dbgfs, "dps", &dps_dbgfs, dps_debugfs_files,
				ARRAY_SIZE(dps_debugfs_files), NULL);
	if (unlikely(ret))
		pr_err("Failed to create dp's debugfs dir\n");

	return ret;
}

s32 pmgr_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;

	return 0;
}
