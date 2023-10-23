/*
 * Description: PP session manager debugfs interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_SESS_MGR_DBG]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/debugfs.h>
#include <linux/types.h>
#include <linux/notifier.h>
#include <linux/bitops.h>
#include <linux/parser.h>
#include <linux/string.h>
#include <linux/jiffies.h>
#include <linux/pp_api.h>
#include <linux/pktprs.h>

#include "classifier.h"
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_si.h"
#include "checker.h"
#include "modifier.h"
#include "uc.h"
#include "pp_session_mgr.h"
#include "pp_session_mgr_internal.h"

/**
 * @brief Session manager debugfs dir
 */
static struct dentry *dbgfs;
static u32            dbg_sess_id;
static unsigned long  req_id;
static u16 mirroring_orig_q = U16_MAX;
static u16 mirroring_sess_id = U16_MAX;

static void __smgr_dbg_inactive_cb(struct pp_inactive_list_cb_args *args)
{
	u32 i, sz, n;
	char *buf;
	ulong scan_time;

	scan_time = get_jiffies_64() - args->base.req_id;
	sz  = 100;                   /* 100 chars for misc stuff             */
	sz += args->n_sessions * 10; /* need 10 char to print session id     */
	sz += args->n_sessions * 1;  /* space or new line after each session */
	buf = kzalloc(sz, GFP_KERNEL);
	if (unlikely(!buf)) {
		pr_err("failed to allocate %u memory\n", sz);
		goto free_inact;
	}

	if (args->n_sessions == 0) {
		pr_buf(buf, sz, n, "No inactive Sessions - Scan time %u ms\n",
		       jiffies_to_msecs(scan_time));
		goto done;
	}

	pr_buf(buf, sz, n, "\n");
	pr_buf_cat(buf, sz, n,
		   " %u PP Inactive Sessions Found - Scan time %u ms\n",
		   args->n_sessions, jiffies_to_msecs(scan_time));
	pr_buf_cat(buf, sz, n,
		   "===================================================\n");

	for (i = 1; i <= args->n_sessions; i++) {
		pr_buf_cat(buf, sz, n, "%u ", args->inact_sess[i - 1]);
		if (!(i % 10))
			pr_buf_cat(buf, sz, n, "\n");
	}

done:
	pr_info("%s", buf);
	kfree(buf);
free_inact:
	kfree(args->inact_sess);
}

/**
 * @brief Debug callback for accepting session manager events
 * @param args event arguments
 */
static void __smgr_dbg_cb(struct pp_cb_args *args)
{
	if (unlikely(ptr_is_null(args)))
		return;

	if (unlikely(!__pp_is_event_valid(args->ev)))
		return;

	pr_info("PP event %s(%u): args %p, req_id %lu, ret %d\n",
		PP_EVENT_STR(args->ev), args->ev,
		args, args->req_id, args->ret);

	switch (args->ev) {
	case PP_SESS_CREATE:
	{
		struct pp_sess_create_cb_args *cr_args;

		cr_args = container_of(args, struct pp_sess_create_cb_args,
				       base);
		if (cr_args->base.ret == -EEXIST) {
			pr_info("Session exist, id %u\n", cr_args->sess_id);
		} else if (!cr_args->base.ret) {
			pr_info("Session %u created\n", cr_args->sess_id);
			dbg_sess_id = cr_args->sess_id;
		} else {
			pr_info("Failed to create session\n");
		}
		break;
	}
	case PP_SESS_DELETE:
	{
		struct pp_sess_delete_cb_args *del_args;

		del_args = container_of(args, struct pp_sess_delete_cb_args,
					base);
		if (del_args->base.ret == -ENOENT)
			pr_info("Session %u not exist\n",
				del_args->sess_id);
		else if (!del_args->base.ret)
			pr_info("Session %u deleted\n",
				del_args->sess_id);
		else
			pr_info("Failed to delete session %u\n",
				del_args->sess_id);
		break;
	}
	case PP_INACTIVE_LIST:
	{
		struct pp_inactive_list_cb_args *inact_args;

		inact_args = container_of(args, struct pp_inactive_list_cb_args,
					  base);
		if (inact_args->base.ret)
			pr_err("failed to get inactive sessions list\n");
		else
			__smgr_dbg_inactive_cb(inact_args);
		break;
	}
	default:
		pr_err("Event %s not supported\n", PP_EVENT_STR(args->ev));
		break;
	}
}

/**
 * @brief Dump all PP opened sessions
 */
void __smgr_dbg_sessions_dump(struct seq_file *f)
{
	unsigned long *bmap;
	u32 n_sessions;
	s32 ret;

	ret = smgr_sessions_bmap_alloc(&bmap, &n_sessions);
	if (unlikely(ret))
		return;

	ret = smgr_open_sessions_bmap_get(bmap, n_sessions);
	if (unlikely(ret))
		goto done;

	seq_printf(f, "%*pbl\n", n_sessions, bmap);

done:
	kfree(bmap);
}

PP_DEFINE_DEBUGFS(sessions, __smgr_dbg_sessions_dump, NULL);

/**
 * @brief Set session ID to use for dumping the session info
 */
void __smgr_dbg_sessions_scan_state_set(char *cmd_buf, void *data)
{
	bool en;

	if (kstrtobool(cmd_buf, &en))
		pr_info("failed to parse '%s'\n", cmd_buf);

	smgr_sessions_scan_state_set(en);
}

/**
 * @brief Dump all PP inactive sessions
 */
void __smgr_dbg_inact_sessions_dump(struct seq_file *f)
{
	struct pp_request req;
	u32 *sessions, n_sessions;
	s32 ret;

	ret = smgr_sessions_arr_alloc(&sessions, &n_sessions);
	if (unlikely(ret))
		return;

	req.cb       = __smgr_dbg_cb;
	req.req_prio = 0;
	req.req_id   = get_jiffies_64();
	ret = pp_inactive_sessions_get(&req, sessions, n_sessions);
}

PP_DEFINE_DEBUGFS(inact_sessions, __smgr_dbg_inact_sessions_dump,
		  __smgr_dbg_sessions_scan_state_set);

/**
 * @brief Set session ID to use for dumping the session info
 */
void __smgr_dbg_session_set(char *cmd_buf, void *data)
{
	if (kstrtou32(cmd_buf, 10, &dbg_sess_id))
		pr_info("failed to parse '%s'\n", cmd_buf);
}

/**
 * @brief Dump specific session si top data, uses session ID
 *        that was set by xsession file
 */
void __smgr_dbg_si_top_dump(struct seq_file *f, struct pp_si *si)
{
	s32 i;

	seq_printf(f, "\n Session %u SI Info\n", dbg_sess_id);
	seq_puts(f,   " ==============================\n");
	seq_printf(f, " %-20s: %u\n",   "sess_id     ", si->sess_id);
	seq_printf(f, " %-20s: %u\n",   "recipe_idx  ", si->recipe_idx);
	seq_printf(f, " %-20s: %#x\n",  "dpu_start   ", si->dpu_start);
	seq_printf(f, " %-20s: %#x\n",  "sce_start   ", si->sce_start);
	seq_printf(f, " %-20s: %u\n",   "fv_sz       ", si->fv_sz);
	seq_printf(f, " %-20s: %#x\n",  "bce_start   ", si->bce_start);
	seq_printf(f, " %-20s: %u\n",   "bce_sz      ", si->bce_sz);
	seq_printf(f, " %-20s: %u\n",   "bce_ext     ", si->bce_ext);
	seq_printf(f, " %-20s: %u\n",   "si_ps_sz    ", si->si_ps_sz);
	seq_printf(f, " %-20s: %u\n",   "si_ud_sz    ", si->si_ud_sz);
	seq_printf(f, " %-20s: %u\n",   "ext_reassembly", si->ext_reassembly);
	seq_printf(f, " %-20s: %u\n",   "int_reassembly", si->int_reassembly);
	seq_printf(f, " %-20s: %u\n",   "ps_off      ", si->ps_off);
	seq_printf(f, " %-20s: %u\n",   "base_policy ", si->base_policy);
	seq_printf(f, " %-20s: %u\n",   "color       ", si->color);
	seq_printf(f, " %-20s: %u\n",   "tdox_flow   ", si->tdox_flow);
	seq_printf(f, " %-20s: %u\n",   "dst_q       ", si->dst_q);
	seq_printf(f, " %-20s: %u\n",   "eg_port     ", si->eg_port);
	seq_printf(f, " %-20s: %u\n",   "trim_l3_id  ", si->trim_l3_id);
	seq_printf(f, " %-20s: %#x\n",  "chck_flags  ", si->chck_flags);
	seq_printf(f, " %-20s: %u\n",   "pl2p        ", si->pl2p);
	seq_printf(f, " %-20s: %u\n",   "tmpl_ud_sz  ", si->tmpl_ud_sz);
	seq_printf(f, " %-20s: %u\n",   "ps_copy     ", si->ps_copy);
	seq_printf(f, " %-20s: %d\n",   "pkt_len_diff", si->pkt_len_diff);
	seq_printf(f, " %-20s: %#lx\n", "sgc_en_map  ", si->sgc_en_map);
	for_each_set_bit(i, &si->sgc_en_map, PP_SI_SGC_MAX)
		seq_printf(f, " sgc[%u]%-14s: %u\n", i, " ", si->sgc[i]);
	seq_printf(f, " %-20s: %#lx\n", "tbm_en_map  ", si->tbm_en_map);
	for_each_set_bit(i, &si->tbm_en_map, PP_SI_TBM_MAX)
		seq_printf(f, " tbm[%u]%-14s: %u\n", i, " ", si->tbm[i]);
	seq_printf(f, " %-20s: %#x\n", "policies_map ", si->policies_map);
	seq_printf(f, " %-20s: %s\n",  "ext_df_mask",
		   BOOL2STR(si->ext_df_mask));
	seq_printf(f, " %-20s: %s\n",  "int_df_mask",
		   BOOL2STR(si->int_df_mask));
	seq_printf(f, " %-20s: %s\n",  "seg_en ", BOOL2STR(si->seg_en));
	seq_printf(f, " %-20s: %u\n",  "fsqm_prio  ", si->fsqm_prio);
}

/**
 * @brief Dump specific session si data, uses session ID
 *        that was set by xsession file
 */
void __smgr_dbg_session_si_dump(struct seq_file *f)
{
	struct pp_dsi dsi;
	struct pp_si si;
	char buf[256];
	s32 i;
	struct sess_db_info *info;
	struct si_ud_frag_info *frag_info;

	memset(&si, 0, sizeof(si));

	if (smgr_session_si_get(dbg_sess_id, &si))
		return;

	/* Dump SI */
	__smgr_dbg_si_top_dump(f, &si);

	if (SI_IS_DPU_EXIST(&si)) {
		seq_printf(f, " %-20s: %u\n",
			   pp_si_fld2str(SI_FLD_DPU_NAT_SZ),
			   si.dpu.nat_sz);
		seq_printf(f, " %-20s: %u\n",
			   pp_si_fld2str(SI_FLD_DPU_NHDR_SZ),
			   si.dpu.nhdr_sz);
		seq_printf(f, " %-20s: %u\n",
			   pp_si_fld2str(SI_FLD_DPU_NHDR_L3_OFF),
			   si.dpu.nhdr_l3_off);
		seq_printf(f, " %-20s: %u\n",
			   pp_si_fld2str(SI_FLD_DPU_LYR_FLD_OFF),
			   si.dpu.lyr_fld_off);
		seq_printf(f, " %-20s: %u\n",
			   pp_si_fld2str(SI_FLD_DPU_PPPOE_OFF),
			   si.dpu.pppoe_off);
	}
	if (SI_IS_SCE_EXIST(&si)) {
		seq_printf(f, " %-20s: %#x\n",
			   pp_si_fld2str(SI_FLD_SCE_L3_CSUM),
			   si.sce.l3_csum_delta);
		seq_printf(f, " %-20s: %#x\n",
			   pp_si_fld2str(SI_FLD_SCE_L4_CSUM),
			   si.sce.l4_csum_delta);
		seq_printf(f, " %-20s: %#x\n",
			   pp_si_fld2str(SI_FLD_SCE_L4_CSUM_ZERO),
			   si.sce.l4_csum_zero);
		seq_printf(f, " %-20s: %#x\n",
			   pp_si_fld2str(SI_FLD_SCE_NHDR_CSUM),
			   si.sce.nhdr_csum);
		seq_printf(f, " %-20s: %#x\n",
			   pp_si_fld2str(SI_FLD_SCE_DSCP),
			   si.sce.dscp);
		seq_printf(f, " %-20s: %u\n",
			   pp_si_fld2str(SI_FLD_SCE_IP_LEN_DIFF),
			   si.sce.tot_len_diff);
		seq_printf(f, " %-20s: %u(%#x)\n",
			   pp_si_fld2str(SI_FLD_SCE_DST_PORT),
			   si.sce.new_dst_port, si.sce.new_dst_port);
		seq_printf(f, " %-20s: %u(%#x)\n",
			   pp_si_fld2str(SI_FLD_SCE_SRC_PORT),
			   si.sce.new_src_port, si.sce.new_src_port);
		seq_printf(f, " %-20s: %u(%#x)\n",
			   pp_si_fld2str(SI_FLD_SCE_TTL_DIFF),
			   si.sce.ttl_diff, si.sce.ttl_diff);
		seq_printf(f, " %-20s: %u(%#x)\n",
			   pp_si_fld2str(SI_FLD_SCE_PPPOE_DIFF),
			   si.sce.pppoe_diff, si.sce.pppoe_diff);
		seq_printf(f, " %-20s: %u(%#x)\n",
			   pp_si_fld2str(SI_FLD_SCE_L2_ORG_VAL),
			   si.sce.l2_org_val, si.sce.l2_org_val);
	}
	if (SI_IS_BCE_EXIST(&si)) {
		if (si.dpu.nat_sz == sizeof(si.bce.nat.v4)) {
			seq_printf(f, " %-20s: %pI4b\n", "bce.nat.saddr",
				   &si.bce.nat.v4.saddr);
			seq_printf(f, " %-20s: %pI4b\n", "bce.nat.daddr",
				   &si.bce.nat.v4.daddr);
		} else if (si.dpu.nat_sz == sizeof(si.bce.nat.v6)) {
			seq_printf(f, " %-20s: %pI6\n", "bce.nat.saddr",
				   &si.bce.nat.v6.saddr);
			seq_printf(f, " %-20s: %pI6\n", "bce.nat.daddr",
				   &si.bce.nat.v6.daddr);
		}
		hex_dump_to_buffer(si.bce.nhdr, si.dpu.nhdr_sz, 32, 1,
				   buf, sizeof(buf), false);
		(void)strreplace(buf, '\n', ' ');
		seq_printf(f, " %-20s: %s\n", "New Header", buf);
	}
	if (si.si_ud_sz || si.si_ps_sz) {
		hex_dump_to_buffer(si.ud, max_t(u8, si.si_ud_sz, si.si_ps_sz),
				   16, 1, buf, sizeof(buf), false);
		(void)strreplace(buf, '\n', ' ');
		seq_printf(f, " %-20s: %s\n", "UD", buf);

		info = kzalloc(sizeof(*info), GFP_KERNEL);
		if (!info)
			return;

		if (smgr_session_info_get(dbg_sess_id, info)) {
			kfree(info);
			return;
		}
		if (test_bit(SESS_FLAG_MTU_CHCK, &info->flags)) {
			frag_info = (struct si_ud_frag_info *)
				    &si.ud[PP_PS_REGION_SZ];
			seq_printf(f, " %-20s: %u (INT_DF %d (IGNORE %d), EXT_DF %d (IGNORE %d), FRAG_EXT %d, IPV4 %d, PPPOE %d)\n",
				   "frag.flags", frag_info->flags,
				   !!(frag_info->flags &
				      FRAG_INFO_FLAG_INT_DF),
				   !!(frag_info->flags &
				      FRAG_INFO_FLAG_IGNORE_INT_DF),
				   !!(frag_info->flags &
				      FRAG_INFO_FLAG_EXT_DF),
				   !!(frag_info->flags &
				      FRAG_INFO_FLAG_IGNORE_EXT_DF),
				   !!(frag_info->flags &
				      FRAG_INFO_FLAG_FRAG_EXT),
				   !!(frag_info->flags & FRAG_INFO_FLAG_IPV4),
				   !!(frag_info->flags &
				      FRAG_INFO_FLAG_PPPOE));
			seq_printf(f, " %-20s: %u\n", "frag.l3_off",
				   frag_info->l3_off);
			seq_printf(f, " %-20s: %u\n", "frag.dst_q",
				   frag_info->dst_q);
			seq_printf(f, " %-20s: %u\n", "frag.max_pkt_size",
				   frag_info->max_pkt_size);
			kfree(info);
		}
	}
	for (i = 0; i < (sizeof(si.fv) / sizeof(u32)); i++)
		seq_printf(f, " FV WORD%-13u: %#010x\n",
			   i, ((u32 *)(&si.fv))[i]);

	if (smgr_session_dsi_get(dbg_sess_id, &dsi))
		return;

	seq_printf(f, "\n Session %u DSI Info\n", dbg_sess_id);
	seq_puts(f,   " ==============================\n");
	seq_printf(f, " %-20s: %s\n",   "valid",   BOOL2STR(dsi.valid));
	seq_printf(f, " %-20s: %s\n",   "active",  BOOL2STR(dsi.active));
	seq_printf(f, " %-20s: %s\n",   "stale",   BOOL2STR(dsi.stale));
	seq_printf(f, " %-20s: %s\n",   "divert",  BOOL2STR(dsi.divert));
	seq_printf(f, " %-20s: %u\n",   "dst_q",   dsi.dst_q);
	seq_printf(f, " %-20s: %llu\n", "bytes",   dsi.bytes_cnt);
	seq_printf(f, " %-20s: %llu\n", "packets", dsi.pkts_cnt);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(session_si, __smgr_dbg_session_si_dump, NULL);

/**
 * @brief Dump specific exception session si data, uses session
 *        ID that was set by xsession file
 */
void __smgr_dbg_exception_session_si_dump(struct seq_file *f)
{
	struct pp_si si;
	s32 ret;

	memset(&si, 0, sizeof(si));

	ret = chk_exception_session_si_get(dbg_sess_id, &si);
	if (unlikely(ret)) {
		pr_err("Failed fetching SI for exception %u\n", dbg_sess_id);
		return;
	}

	/* Dump SI */
	__smgr_dbg_si_top_dump(f, &si);
}

PP_DEFINE_DEBUGFS(exception_session_si,
		  __smgr_dbg_exception_session_si_dump,
		  NULL);

/**
 * @brief Dump fragmentation stats
 */
void __smgr_dbg_frag_stats(struct seq_file *f)
{
	struct frag_stats stats[UC_CPUS_MAX], *it;
	s32 ret;
	u32 cid, i;
	char **str;
	static const char * const cntrs_str[] = {
		"rx_pkt",
		"tx_pkt",
		"total_drops",
		"bmgr_drops",
		"df_drops",
		"max_frags_drops",
	};

	for (cid = 0; cid < UC_CPUS_MAX; cid++) {
		ret = uc_frag_cpu_stats_get(cid, &stats[cid]);
		if (unlikely(ret)) {
			pr_err("failed to get frag uc cpu %u counters\n", cid);
			return;
		}
	}

	seq_puts(f, "\n");
	seq_puts(f, "|=====================================================================|\n");
	seq_puts(f, "|                   Fragmentation UC Statistics                       |\n");
	seq_puts(f, "|=====================================================================|\n");
	seq_printf(f, "| %-15s ", "Counter");
	for (cid = 0; cid < UC_CPUS_MAX; cid++)
		seq_printf(f, "|  CPU%u      ", cid);
	seq_puts(f, "|\n");
	seq_puts(f, "|-----------------+------------+------------+------------+------------|\n");

	i = 0;
	for_each_arr_entry(str, cntrs_str, ARRAY_SIZE(cntrs_str), i++) {
		seq_printf(f, "| %-15s ", *str);
		for_each_arr_entry(it, stats, ARRAY_SIZE(stats)) {
			seq_printf(f, "| %10llu ", *(((u64 *)it) + i));
		}
		seq_puts(f, "|\n");
	}
	seq_puts(f, "|---------------------------------------------------------------------|\n\n");
}

PP_DEFINE_DEBUGFS(frag_stats, __smgr_dbg_frag_stats, NULL);

/**
 * @brief Dump fragmentation stats
 */
void __smgr_dbg_tdox_stats(struct seq_file *f)
{
	struct smgr_tdox_stats tdox_stats;
	s32 ret;

	ret = smgr_tdox_stats_get(&tdox_stats);
	if (ret)
		return;

	seq_puts(f, "\n");
	seq_puts(f, "|==============================|\n");
	seq_puts(f, "|        Tdox Statistics       |\n");
	seq_puts(f, "|==============================|\n");
	seq_printf(f, "| %-15s ", "uC Stats");
	seq_puts(f, "|\n");
	seq_puts(f, "|-----------------|------------|\n");
	seq_printf(f, "| %-15s ", " uC Rx ");
	seq_printf(f, "| %10u |\n", tdox_stats.uc_rx_pkt);
	seq_printf(f, "| %-15s ", " uC Tx ");
	seq_printf(f, "| %10u |\n", tdox_stats.uc_tx_pkt);
	seq_puts(f, "|------------------------------|\n");
	seq_printf(f, "| %-15s ", "Entry Stats");
	seq_puts(f, "|\n");
	seq_puts(f, "|-----------------|------------|\n");
	seq_printf(f, "| %-15s ", " Free Entries ");
	seq_printf(f, "| %10u |\n", tdox_stats.tdox_sess_free);
	seq_printf(f, "| %-15s ", " Free Cand ");
	seq_printf(f, "| %10u |\n", tdox_stats.tdox_free_candidates);
	seq_puts(f, "|------------------------------|\n");
	seq_printf(f, "| %-15s ", "Session Stats");
	seq_puts(f, "|\n");
	seq_puts(f, "|-----------------|------------|\n");
	seq_printf(f, "| %-15s ", " Tdox Full ");
	seq_printf(f, "| %10u |\n", tdox_stats.tdox_sess_full_fail);
	seq_printf(f, "| %-15s ", " TdoxCand Full ");
	seq_printf(f, "| %10u |\n", tdox_stats.tdox_candidate_full_fail);
	seq_printf(f, "| %-15s ", " Create Fail ");
	seq_printf(f, "| %10u |\n", tdox_stats.tdox_create_args_err);
	seq_puts(f, "|-----------------|------------|\n");
	seq_printf(f, "| %-15s ", "Lists Stats");
	seq_puts(f, "|\n");
	seq_puts(f, "|-----------------|------------|\n");
	seq_printf(f, "| %-15s ", " free ");
	seq_printf(f, "| %10u |\n", tdox_stats.free_list_cnt);
	seq_printf(f, "| %-15s ", " busy ");
	seq_printf(f, "| %10u |\n", tdox_stats.busy_list_cnt);
	seq_printf(f, "| %-15s ", " obsolete ");
	seq_printf(f, "| %10u |\n", tdox_stats.obsolete_list_cnt);
	seq_printf(f, "| %-15s ", " cand_free ");
	seq_printf(f, "| %10u |\n", tdox_stats.cand_free_list_cnt);
	seq_printf(f, "| %-15s ", " cand_busy ");
	seq_printf(f, "| %10u |\n", tdox_stats.cand_busy_list_cnt);
	seq_printf(f, "| %-15s ", " update_free ");
	seq_printf(f, "| %10u |\n", tdox_stats.sess_update_free_list_cnt);
	seq_printf(f, "| %-15s ", " update_busy ");
	seq_printf(f, "| %10u |\n", tdox_stats.sess_update_busy_list_cnt);
	seq_puts(f, "|------------------------------|\n");
}

PP_DEFINE_DEBUGFS(tdox_stats, __smgr_dbg_tdox_stats, NULL);

static int __smgr_dbg_tdox_en_set(void *data, u64 val)
{
	smgr_tdox_enable_set(val);
	return 0;
}

static int __smgr_dbg_tdox_en_get(void *data, u64 *val)
{
	u32 tout;

	tout = smgr_tdox_enable_get();
	*val = (u64)tout;
	return 0;
}

PP_DEFINE_DBGFS_ATT(tdox_en_fops, __smgr_dbg_tdox_en_get,
		    __smgr_dbg_tdox_en_set);

/**
 * @brief Dump specific session si and dsi raw data, uses session ID
 *        that was set by xsession file
 */
void __smgr_dbg_session_si_raw_dump(struct seq_file *f)
{
	struct pp_hw_dsi hw_dsi;
	struct pp_hw_si hw_si;
	u32 sess_id;
	s32 ret, i;

	memset(&hw_si,  0, sizeof(hw_si));
	memset(&hw_dsi, 0, sizeof(hw_dsi));

	ret = cls_session_si_get(dbg_sess_id, &hw_si);
	if (ret)
		return;
	ret = chk_session_dsi_get(dbg_sess_id, &hw_dsi);
	if (ret)
		return;
	ret = pp_si_fld_get(&hw_si, SI_FLD_SESS_ID, (s32 *)&sess_id);
	if (ret)
		return;

	seq_printf(f, "\n Session %u SI Info\n", sess_id);
	seq_puts(f,   " =========================\n");
	for (i = 0; i < ARRAY_SIZE(hw_si.word); i++)
		seq_printf(f, " SI WORD%-4u(%#4x): %#010x\n", i,
			   i * (u32)sizeof(u32), hw_si.word[i]);
	for (i = 0; i < ARRAY_SIZE(hw_dsi.word); i++)
		seq_printf(f, " DSI WORD%-3u(%#4x): %#010x\n", i,
			   i * (u32)sizeof(u32), hw_dsi.word[i]);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(session_si_raw, __smgr_dbg_session_si_raw_dump, NULL);

static void __smgr_pr_session_flags(struct seq_file *f, ulong flags)
{
	const char *flags_str[SMGR_FLAGS_NUM] = {
		"ROUTED",
		"SYNCQ",
		"MTU_CHECK",
		"MCAST_GRP",
		"MCAST_DST",
		"SESS_FLAG_TDOX",
		"SESS_FLAG_TDOX_SUPP",
	};
	u32 flag;

	if (!flags)
		return;

	seq_printf(f, " %-20s = %#lx ", "Flags", flags);
	for_each_set_bit(flag, &flags, SMGR_FLAGS_NUM)
		seq_printf(f, "[%s]", flags_str[flag]);
	seq_puts(f, "\n");
}

static void __smgr_pr_si_chck_flags(struct seq_file *f, ulong flags)
{
	u32 flag;

	if (!flags)
		return;

	seq_printf(f, " %-20s = %#lx ", "Checker Flags", flags);
	for_each_set_bit(flag, &flags, SI_CHCK_FLAGS_NUM)
		seq_printf(f, "[%s]", pp_si_chck_flag_to_str(flag));
	seq_puts(f, "\n");
}

static void __smgr_pr_mac(struct seq_file *f, struct pktprs_hdr *h, u8 lvl)
{
	if (unlikely(!h))
		return;

	if (PKTPRS_IS_MAC(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "ETH header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_MAC, lvl));
		seq_printf(f, " %-20s = %pM\n", "Source MAC",
			   pktprs_eth_hdr(h, lvl)->h_source);
		seq_printf(f, " %-20s = %pM\n", "Dest MAC",
			   pktprs_eth_hdr(h, lvl)->h_dest);
		seq_printf(f, " %-20s = %s %u[%#x]\n", "Ether Type",
			   PP_FV_ETHTYPE_STR(pktprs_eth_hdr(h, lvl)->h_proto),
			   ntohs(pktprs_eth_hdr(h, lvl)->h_proto),
			   ntohs(pktprs_eth_hdr(h, lvl)->h_proto));
	}
	if (PKTPRS_IS_VLAN0(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "VLAN header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_VLAN0, lvl));
		seq_printf(f, " %-20s = %u[%#x]\n", "External VLAN",
			   ntohs(pktprs_vlan_hdr(h, lvl, 0)->h_vlan_TCI),
			   ntohs(pktprs_vlan_hdr(h, lvl, 0)->h_vlan_TCI));
	}
	if (PKTPRS_IS_VLAN1(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "VLAN header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_VLAN1, lvl));
		seq_printf(f, " %-20s = %u[%#x]\n", "Internal VLAN",
			   ntohs(pktprs_vlan_hdr(h, lvl, 1)->h_vlan_TCI),
			   ntohs(pktprs_vlan_hdr(h, lvl, 1)->h_vlan_TCI));
	}
	if (PKTPRS_IS_PPPOE(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "PPPOE header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_PPPOE, lvl));
		seq_printf(f, " %-20s = %u[%#x]\n", "PPPoE ID",
			   ntohs(pktprs_pppoe_hdr(h, lvl)->sid),
			   ntohs(pktprs_pppoe_hdr(h, lvl)->sid));
	}
}

static void __smgr_pr_ipv4(struct seq_file *f, struct pktprs_hdr *h, u8 lvl)
{
	struct iphdr *v4 = pktprs_ipv4_hdr(h, lvl);

	if (unlikely(!v4))
		return;

	seq_printf(f, " %-20s = %u\n", "IPV4 header Off",
		   pktprs_hdr_off(h, PKTPRS_PROTO_IPV4, lvl));
	seq_printf(f, " %-20s = %pI4b[%#x]\n", "Source IP",
		   &v4->saddr, ntohl(v4->saddr));
	seq_printf(f, " %-20s = %pI4b[%#x]\n", "Dest IP",
		   &v4->daddr, ntohl(v4->daddr));
	seq_printf(f, " %-20s = %s %u[%#x]\n", "L4 Protocol",
		   PP_FV_L3_PROTOCOL_STR(v4->protocol),
		   v4->protocol, v4->protocol);
	seq_printf(f, " %-20s = %u[%#x]\n", "TOS",
		   v4->tos, v4->tos);
	if (pktprs_ipv4_first_frag(h, lvl))
		seq_printf(f, " %-20s = %s\n", "Frag", "First");
	else if (ip_is_fragment(v4))
		seq_printf(f, " %-20s = %s\n", "Frag", "None-First");
}

static void __smgr_pr_ipv6(struct seq_file *f, struct pktprs_hdr *h, u8 lvl)
{
	struct ipv6hdr *v6 = pktprs_ipv6_hdr(h, lvl);

	u32 flow = 0;
	u8  tc   = 0;

	if (unlikely(!v6))
		return;

	tc   = PP_FIELD_GET(GENMASK(7, 4),   v6->flow_lbl[0]);
	tc   = PP_FIELD_MOD(GENMASK(7, 4),   v6->priority,           tc);
	flow = PP_FIELD_MOD(GENMASK(19, 16), v6->flow_lbl[0] & 0x0f, flow);
	flow = PP_FIELD_MOD(GENMASK(15, 8),  v6->flow_lbl[1],        flow);
	flow = PP_FIELD_MOD(GENMASK(7, 0),   v6->flow_lbl[2],        flow);
	seq_printf(f, " %-20s = %u\n", "IPV6 header Off",
		   pktprs_hdr_off(h, PKTPRS_PROTO_IPV6, lvl));
	seq_printf(f, " %-20s = %pI6\n",    "Source IP",  &v6->saddr);
	seq_printf(f, " %-20s = %pI6\n",    "Dest IP",    &v6->daddr);
	seq_printf(f, " %-20s = %s %u[%#x]\n", "Next Header",
		   PP_FV_L3_PROTOCOL_STR(v6->nexthdr),
		   v6->nexthdr, v6->nexthdr);
	seq_printf(f, " %-20s = %u[%#x]\n", "TC",         tc, tc);
	seq_printf(f, " %-20s = %u[%#x]\n", "Flow Label", flow, flow);
	if (PKTPRS_IS_FRAG_OPT(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "IPV6 FRAG header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_FRAG_OPT, lvl));
		if (pktprs_ipv6_first_frag(h, lvl))
			seq_printf(f, " %-20s = %s\n", "Frag", "First");
		else if (pktprs_frag_opt_hdr(h, lvl)->frag_off &
			 htons(IP6_OFFSET))
			seq_printf(f, " %-20s = %s\n", "Frag", "None-First");
	}
}

static void __smgr_pr_l3(struct seq_file *f, struct pktprs_hdr *h, u8 lvl)
{
	if (unlikely(!h))
		return;

	if (PKTPRS_IS_IPV4(h, lvl))
		__smgr_pr_ipv4(f, h, lvl);
	else if (PKTPRS_IS_IPV6(h, lvl))
		__smgr_pr_ipv6(f, h, lvl);
}

static void __smgr_pr_l4(struct seq_file *f, struct pktprs_hdr *h, u8 lvl)
{
	union proto_ptr p;

	if (unlikely(!h))
		return;

	if (PKTPRS_IS_TCP(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "TCP header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_TCP, lvl));
		p.tcp = pktprs_tcp_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "TCP Source Port",
			   ntohs(p.tcp->source), ntohs(p.tcp->source));
		seq_printf(f, " %-20s = %u[%#x]\n", "TCP Dest Port",
			   ntohs(p.tcp->dest), ntohs(p.tcp->dest));
	} else if (PKTPRS_IS_UDP(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "UDP header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_UDP, lvl));
		p.udp = pktprs_udp_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "UDP Source Port",
			   ntohs(p.udp->source), ntohs(p.udp->source));
		seq_printf(f, " %-20s = %u[%#x]\n", "UDP Dest Port",
			   ntohs(p.udp->dest), ntohs(p.udp->dest));
	} else if (PKTPRS_IS_SCTP(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "SCTP header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_SCTP, lvl));
		p.sctp = pktprs_sctp_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "SCTP Source Port",
			   ntohs(p.sctp->source), ntohs(p.sctp->source));
		seq_printf(f, " %-20s = %u[%#x]\n", "SCTP Dest Port",
			   ntohs(p.sctp->dest), ntohs(p.sctp->dest));
	} else if (PKTPRS_IS_ICMP(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "ICMP header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_ICMP, lvl));
		p.icmp = pktprs_icmp_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "ICMP ID",
			   ntohs(p.icmp->un.echo.id),
			   ntohs(p.icmp->un.echo.id));
		seq_printf(f, " %-20s = %u[%#x]\n", "ICMP Type",
			   p.icmp->type, p.icmp->type);
	} else if (PKTPRS_IS_ICMP6(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "ICMP6 header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_ICMP6, lvl));
		p.icmp6 = pktprs_icmp6_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "ICMP6 ID",
			   ntohs(p.icmp6->icmp6_dataun.u_echo.identifier),
			   ntohs(p.icmp6->icmp6_dataun.u_echo.identifier));
		seq_printf(f, " %-20s = %u[%#x]\n", "ICMP6 Type",
			   pktprs_icmp6_hdr(h, lvl)->icmp6_type,
			   pktprs_icmp6_hdr(h, lvl)->icmp6_type);
	} else if (PKTPRS_IS_ESP(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "ESP header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_ESP, lvl));
		p.esp = pktprs_esp_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "ESP SPI",
			   ntohl(p.esp->spi), ntohl(p.esp->spi));
	}
}

static void __smgr_pr_tunn(struct seq_file *f, struct pktprs_hdr *h, u8 lvl)
{
	union proto_ptr p;

	if (unlikely(!h))
		return;

	seq_printf(f, " %-20s = ", "Tunnel Type");
	if (PKTPRS_IS_PPPOE(h, lvl))
		seq_puts(f, "[PPPoE]");
	if (PKTPRS_IS_L2TP_OUDP(h, lvl))
		seq_puts(f, "[UDP][L2TP]");
	if (PKTPRS_IS_VXLAN(h, lvl))
		seq_puts(f, "[UDP][VXLAN]");
	if (PKTPRS_IS_GENEVE(h, lvl))
		seq_puts(f, "[UDP][GENEVE]");
	if (PKTPRS_IS_SCTP(h, lvl))
		seq_puts(f, "[SCTP]");
	if (PKTPRS_IS_L2TP_OIP(h, lvl))
		seq_puts(f, "[L2TPIP]");
	if (PKTPRS_IS_IP_GRE(h))
		seq_puts(f, "[IPoGRE]");
	if (PKTPRS_IS_L2_GRE(h))
		seq_puts(f, "[EoGRE]");
	if (PKTPRS_IS_DSLITE(h))
		seq_puts(f, "[DSLITE]");
	if (PKTPRS_IS_SIXRD(h))
		seq_puts(f, "[SIXRD]");
	seq_puts(f, "\n");

	if (PKTPRS_IS_VXLAN(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "VXLAN header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_VXLAN, lvl));
		p.vxlan = pktprs_vxlan_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "VNI",
			   ntohl(p.vxlan->vx_flags), ntohl(p.vxlan->vx_flags));
	} else if (PKTPRS_IS_GENEVE(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "GENEVE header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_GENEVE, lvl));
		p.geneve = pktprs_geneve_hdr(h, lvl);
		seq_printf(f, " %-20s = %#02x%02x%02x\n", "VNI",
			   p.geneve->vni[2],
			   p.geneve->vni[1],
			   p.geneve->vni[0]);
	} else if (PKTPRS_IS_L2TP_OIP(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "L2TP_OIP header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_L2TP_OIP, lvl));
		p.l2tp = pktprs_l2tp_oip_hdr(h, lvl);
		seq_printf(f, " %-20s = %u[%#x]\n", "Session ID",
			   ntohl(p.l2tp->v3_oip.sess_id),
			   ntohl(p.l2tp->v3_oip.sess_id));
	} else if (PKTPRS_IS_L2TP_OUDP(h, lvl)) {
		seq_printf(f, " %-20s = %u\n", "L2TP_OUDP header Off",
			   pktprs_hdr_off(h, PKTPRS_PROTO_L2TP_OUDP, lvl));
		p.l2tp = pktprs_l2tp_oudp_hdr(h, lvl);
		if ((p.l2tp->v2.flags & L2TP_HDR_VER_MASK) == L2TP_HDR_VER_2) {
			if (p.l2tp->v3_oudp.flags & L2TP_HDRFLAG_L) {
				seq_printf(f, " %-20s = %u[%#x]\n",
					   "Session ID", p.l2tp->v2_len.sess_id,
					   p.l2tp->v2_len.sess_id);
				seq_printf(f, " %-20s = %u[%#x]\n", "Tunnel ID",
					   p.l2tp->v2_len.tunnel,
					   p.l2tp->v2_len.tunnel);
			} else {
				seq_printf(f, " %-20s = %u[%#x]\n",
					   "Session ID", p.l2tp->v2.sess_id,
					   p.l2tp->v2.sess_id);
				seq_printf(f, " %-20s = %u[%#x]\n", "Tunnel ID",
					   p.l2tp->v2.tunnel,
					   p.l2tp->v2.tunnel);
			}
		} else {
			seq_printf(f, " %-20s = %u[%#x]\n", "Session ID",
				   p.l2tp->v3_oudp.sess_id,
				   p.l2tp->v3_oudp.sess_id);
		}
	}
}

static void __smgr_pr_pkt(struct seq_file *f, struct pktprs_hdr *h)
{
	if (unlikely(!h))
		return;

	seq_printf(f, " %-20s = %u\n", "Header Length", h->buf_sz);
	__smgr_pr_mac(f,  h, HDR_OUTER);
	__smgr_pr_l3(f,   h, HDR_OUTER);
	__smgr_pr_l4(f,   h, HDR_OUTER);
	__smgr_pr_tunn(f, h, HDR_OUTER);
	__smgr_pr_mac(f,  h, HDR_INNER);
	__smgr_pr_l3(f,   h, HDR_INNER);
	__smgr_pr_l4(f,   h, HDR_INNER);
}

static void __smgr_pr_sess_mod(struct seq_file *f, ulong mod_flags,
			       struct pp_si *si)
{
	char buf[256];
	u32 flag;

	if (unlikely(!si))
		return;

	if (!(SI_IS_SCE_EXIST(si) || SI_IS_DPU_EXIST(si)))
		return;

	seq_puts(f, " Modification Record\n");
	seq_puts(f, "=====================\n");
	seq_printf(f, " %-20s = %#lx ", "Modification Flags", mod_flags);
	for_each_set_bit(flag, &mod_flags, MOD_FLAG_NUM)
		seq_printf(f, "[%s]", mod_flag_to_str(flag));
	seq_puts(f, "\n");

	if (SI_IS_DPU_EXIST(si)) {
		seq_printf(f, " %-20s = %u[%#x]\n", "NAT Size",
			   si->dpu.nat_sz, si->dpu.nat_sz);
		seq_printf(f, " %-20s = %u[%#x]\n", "New Header Size",
			   si->dpu.nhdr_sz, si->dpu.nhdr_sz);
		seq_printf(f, " %-20s = %u[%#x]\n", "New Header L3 Offset",
			   si->dpu.nhdr_l3_off, si->dpu.nhdr_l3_off);
		seq_printf(f, " %-20s = %u[%#x]\n", "Layer Field Offset",
			   si->dpu.lyr_fld_off, si->dpu.lyr_fld_off);
		seq_printf(f, " %-20s = %u[%#x]\n", "PPPoE Offset",
			   si->dpu.pppoe_off, si->dpu.pppoe_off);
	}
	if (si->dpu.nhdr_sz) {
		hex_dump_to_buffer(si->bce.nhdr, si->dpu.nhdr_sz, 32, 4,
				   buf, sizeof(buf), false);
		(void)strreplace(buf, '\n', ' ');
		seq_printf(f, " %-20s = %s\n", "New Header", buf);
	}

	if (si->dpu.nat_sz == sizeof(si->bce.nat.v4)) {
		seq_printf(f, " %-20s = %pI4b[%#x]\n", "Source IP",
			   &si->bce.nat.v4.saddr, si->bce.nat.v4.saddr);
		seq_printf(f, " %-20s = %pI4b[%#x]\n", "Dest IP",
			   &si->bce.nat.v4.daddr, si->bce.nat.v4.daddr);
	} else if (si->dpu.nat_sz == sizeof(si->bce.nat.v6)) {
		seq_printf(f, " %-20s = %pI6\n", "Source IP",
			   &si->bce.nat.v6.saddr);
		seq_printf(f, " %-20s = %pI6\n", "Dest IP",
			   &si->bce.nat.v6.daddr);
	}

	if (SI_IS_SCE_EXIST(si)) {
		seq_printf(f, " %-20s = %u[%#x]\n", "Source Port",
			   si->sce.new_src_port, si->sce.new_src_port);
		seq_printf(f, " %-20s = %u[%#x]\n", "Dest Port",
			   si->sce.new_dst_port, si->sce.new_dst_port);
		seq_printf(f, " %-20s = %#x\n", "L3 Checksum Delta",
			   si->sce.l3_csum_delta);
		seq_printf(f, " %-20s = %#x\n", "L4 Checksum Delta",
			   si->sce.l4_csum_delta);
		seq_printf(f, " %-20s = %#x\n", "New Header Checksum",
			   si->sce.nhdr_csum);
		seq_printf(f, " %-20s = %#x\n", "TOS",
			   si->sce.dscp);
		seq_printf(f, " %-20s = %d\n", "Packet Length Diff",
			   si->pkt_len_diff);
		seq_printf(f, " %-20s = %d\n", "IP Length Diff",
			   si->sce.tot_len_diff);
		seq_printf(f, " %-20s = %u\n", "TTL/Hop Limit Diff",
			   si->sce.ttl_diff);
		seq_printf(f, " %-20s = %u\n", "PPPoE Diff",
			   si->sce.pppoe_diff);
	}
}

/**
 * @brief Dump specific session info including packets and bytes
 *        counters
 */
void __smgr_dbg_session_dump(struct seq_file *f)
{
	struct sess_db_info *info;
	struct pp_dsi dsi;
	struct pp_si si;
	char buf[128];
	s32 n;

	memset(&si,   0, sizeof(si));
	memset(&dsi,  0, sizeof(dsi));

	info = kzalloc(sizeof(*info), GFP_KERNEL);
	if (!info)
		return;

	if (smgr_session_info_get(dbg_sess_id, info))
		goto done;
	if (smgr_session_si_get(dbg_sess_id, &si))
		goto done;
	if (smgr_session_dsi_get(dbg_sess_id, &dsi))
		goto done;

	pr_buf(buf, sizeof(buf), n, "Session %u Info", info->sess_id);
	seq_puts(f,   "##############################\n");
	seq_printf(f, "##  %-22s  ##\n", buf);
	seq_puts(f,   "##############################\n");
	seq_printf(f, " %-20s = %s\n", "Valid", BOOL2STR(dsi.valid));
	seq_printf(f, " %-20s = %s\n", "Active", BOOL2STR(dsi.active));
	seq_printf(f, " %-20s = %s\n", "Stale", BOOL2STR(dsi.stale));
	seq_printf(f, " %-20s = %s\n", "Type",
		   smgr_is_sess_routed(info) ? "Routed" : "Bridge");
	seq_printf(f, " %-20s = %s\n",   "Divert", BOOL2STR(dsi.divert));
	__smgr_pr_session_flags(f, info->flags);
	seq_printf(f, " %-20s = %u - %s\n", "Recipe", si.recipe_idx,
		   mod_rcp_to_str(si.recipe_idx));
	seq_printf(f, " %-20s = %u - %s\n", "Color", si.color,
		   PP_COLOR_TO_STR(si.color));
	seq_printf(f, " %-20s = %u\n", "FV Size", si.fv_sz);
	seq_printf(f, " %-20s = %u\n", "TDOX Flow/LRO Info", si.tdox_flow);
	__smgr_pr_si_chck_flags(f, si.chck_flags);
	seq_puts(f,   "\n");

	seq_puts(f,   " Stats\n");
	seq_puts(f,   "=======\n");
	seq_printf(f, " %-20s = %llu\n", "Packets", dsi.pkts_cnt);
	seq_printf(f, " %-20s = %llu\n", "Bytes",   dsi.bytes_cnt);
	seq_puts(f,   "\n");

	seq_puts(f,   " Ingress Packet\n");
	seq_puts(f,   "================\n");
	seq_printf(f, " %-20s = %u\n", "Port", info->in_port);
	__smgr_pr_pkt(f, &info->rx);
	seq_puts(f,   "\n");

	seq_puts(f,   " Egress Packet\n");
	seq_puts(f,   "===============\n");
	seq_printf(f, " %-20s = %u\n", "Port", si.eg_port);
	seq_printf(f, " %-20s = %u\n", "Dest Q", si.dst_q);
	seq_printf(f, " %-20s = %u\n", "Dynamic Dest Q", dsi.dst_q);
	__smgr_pr_pkt(f, &info->tx);
	seq_puts(f,   "\n");

	__smgr_pr_sess_mod(f, info->mod_flags, &si);
	seq_puts(f,   "\n");

	seq_puts(f,   " Hash\n");
	seq_puts(f,   "======\n");
	seq_printf(f, " %-20s = %#x\n", "H1", info->hash.h1);
	seq_printf(f, " %-20s = %#x\n", "H2", info->hash.h2);
	seq_printf(f, " %-20s = %#x\n", "Signature", info->hash.sig);
	seq_puts(f, "\n");

	if (smgr_is_sess_mcast_dst(info) || smgr_is_sess_mcast_grp(info)) {
		seq_puts(f,   " Multicast\n");
		seq_puts(f,   "==========\n");
		seq_printf(f, " %-20s = %#x\n", "Group-ID",
			   info->mcast.grp_idx);
		if (smgr_is_sess_mcast_dst(info)) {
			seq_printf(f, " %-20s = %#x\n",
				   "Dest-ID", info->mcast.dst_idx);
		}
		seq_puts(f, "\n");
	}
done:
	kfree(info);
}

PP_DEFINE_DEBUGFS(session, __smgr_dbg_session_dump, __smgr_dbg_session_set);

void __smgr_dbg_stats_dump(struct seq_file *f)
{
	struct smgr_stats stats;
	u32 n_sessions;

	if (pp_max_sessions_get(&n_sessions))
		return;
	memset(&stats, 0, sizeof(stats));
	if (pp_smgr_stats_get(&stats))
		return;

	seq_puts(f,   " +---------------------------------------------------------------------------------------+\n");
	seq_puts(f,   " |                              Session Manager Statistics                               |\n");
	seq_puts(f,   " +------------------------------+------------+------------------------------+------------+\n");

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Max Supported Sessions", n_sessions,
		   "Free Sessions", atomic_read(&stats.sess_free));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Open Sessions",
		   atomic_read(&stats.sess_open),
		   "Open Sessions High Watermark",
		   atomic_read(&stats.sess_open_hi_wm));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Session Create Requests",
		   atomic_read(&stats.sess_create_req),
		   "Sessions Created",
		   atomic_read(&stats.sess_created));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Session Delete Requests",
		   atomic_read(&stats.sess_delete_req),
		   "Deleted Sessions",
		   atomic_read(&stats.sess_deleted));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Port Flush Requests",
		   atomic_read(&stats.port_flush_req),
		   "Ports Flushed",
		   atomic_read(&stats.port_flushed));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Flush All Requests",
		   atomic_read(&stats.flush_all_req),
		   "Flush All Done",
		   atomic_read(&stats.flush_all_done));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Inactive Sessions Request",
		   atomic_read(&stats.inactive_req),
		   "Inactive Sessions Req Done",
		   atomic_read(&stats.inactive_done));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Lookup Success",
		   atomic_read(&stats.sess_lu_succ),
		   "Lookup Fail",
		   atomic_read(&stats.sess_lu_fail));
	seq_printf(f, " | %-28s | %-10u | %-28s | %-10s |\n",
		   "Wait for Crawler Idle",
		   atomic_read(&stats.crwlr_idle_wait),
		   "", "");

	seq_puts(f,   " +------------------------------+------------+------------------------------+------------+\n");
	seq_puts(f,   " |                      Errors                                                           |\n");
	seq_puts(f,   " +------------------------------+------------+------------------------------+------------+\n");
	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Session Create Fail",
		   atomic_read(&stats.sess_create_fail),
		   "Session Delete Fail",
		   atomic_read(&stats.sess_delete_fail));
	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Port Flush Fail",
		   atomic_read(&stats.port_flush_fail),
		   "Flush All Fail",
		   atomic_read(&stats.flush_all_fail));

	seq_printf(f, " | %-28s | %-10u | %-28s | %-10s |\n",
		   "Inactive Sessions Req Fail",
		   atomic_read(&stats.inactive_fail), "", "");

	seq_puts(f,   " +------------------------------+------------+------------------------------+------------+\n");
	seq_puts(f,   " |                      Error Cause                                                      |\n");
	seq_puts(f,   " +------------------------------+------------+------------------------------+------------+\n");
	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Invalid Arguments",
		   atomic_read(&stats.invalid_args),
		   "Lookup Error",
		   atomic_read(&stats.sess_lookup_err));
	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Unsupported Protocols",
		   atomic_read(&stats.sess_not_supported),
		   "SI Error",
		   atomic_read(&stats.sess_si_create_err));
	seq_printf(f, " | %-28s | %-10u | %-28s | %-10u |\n",
		   "Cache Error",
		   atomic_read(&stats.work_args_cache_err),
		   "HW Error",
		   atomic_read(&stats.hw_err));
	seq_puts(f,   " +------------------------------+------------+------------------------------+------------+\n");
	seq_puts(f,   "\n");
}

void __smgr_dbg_stats_reset(char *cmd_buf, void *data)
{
	if (!strncmp(cmd_buf, "0", strlen("0")))
		pp_smgr_stats_reset();
}

PP_DEFINE_DEBUGFS(stats,
		  __smgr_dbg_stats_dump,
		  __smgr_dbg_stats_reset);

enum sess_del_opts {
	sess_del_opt_help = 1,
	sess_del_opt_id,
	sess_del_opt_prio,
	sess_del_opt_async,
};

static const match_table_t sess_del_tokens = {
	{sess_del_opt_help, "help"},
	{sess_del_opt_id,   "id=%u"},
	{sess_del_opt_prio, "prio=%u"},
	{sess_del_opt_async, "async"},
};

/**
 * @brief Session delete help
 */
static void __smgr_dbg_sess_del_help(void)
{
	pr_info("\n");
	pr_info(" Usage: echo <option>[=value] > delete_session\n");
	pr_info(" Options:\n");
	pr_info("   help - print this help function\n");
	pr_info("   id   - session id to delete\n");
	pr_info("   prio - session delete priority\n");
	pr_info(" Examples:\n");
	pr_info("   delete session 200:\n");
	pr_info("     echo id=200 > delete_session\n");
}

/**
 * @brief Delete session
 */
void __smgr_dbg_sess_delete(char *cmd_buf, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	struct pp_request req;
	enum sess_del_opts opt;
	char *tok, *args;
	int val, ret;
	u32 sess_id;

	memset(&req, 0, sizeof(req));
	req.req_id = req_id++;
	sess_id = SMGR_INVALID_SESS_ID;
	args = cmd_buf;
	args = strim(args);
	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, sess_del_tokens, substr);
		switch (opt) {
		case sess_del_opt_help:
			if (strcmp(tok, "help"))
				pr_info("Error: Invalid option '%s'\n", tok);
			__smgr_dbg_sess_del_help();
			return;
		case sess_del_opt_id:
			if (match_int(&substr[0], &val))
				return;
			sess_id = (u32)val;
			break;
		case sess_del_opt_prio:
			if (match_int(&substr[0], &val))
				return;
			req.req_prio = (unsigned long)val;
			break;
		case sess_del_opt_async:
			req.cb = __smgr_dbg_cb;
			break;
		default:
			break;
		}
	}
	if (sess_id == SMGR_INVALID_SESS_ID)
		return;

	ret = pp_session_delete(sess_id, (req.cb ? &req : NULL));
	if (ret == -ENOENT)
		pr_info("Session %u not exist\n", sess_id);
	else if (!ret)
		pr_info("Session %u deleted\n", sess_id);
	else
		pr_info("Delete session %u failed, ret %d\n", sess_id, ret);
}

PP_DEFINE_DEBUGFS(sess_del, NULL, __smgr_dbg_sess_delete);

enum flush_opts {
	flush_opt_help,
	flush_opt_all,
	flush_opt_pid,
	flush_opt_async,
};

static const match_table_t flush_tokens = {
	{flush_opt_help,  "help"},
	{flush_opt_all,   "all"},
	{flush_opt_pid,   "pid=%u"},
	{flush_opt_async, "async"},
};

/**
 * @brief Session delete help
 */
static void __smgr_dbg_flush_help(void)
{
	pr_info("\n");
	pr_info(" Usage: echo <option>[=value] > flush\n");
	pr_info(" Options:\n");
	pr_info("   help - print this help function\n");
	pr_info("   pid  - port id to flush (GPID)\n");
	pr_info("   all  - flush all pp sessions\n");
	pr_info(" Examples:\n");
	pr_info("   flush all sessions:\n");
	pr_info("     echo all > flush\n");
	pr_info("   flush all port 5 sessions:\n");
	pr_info("     echo pid=5 > flush\n\n");
}

/**
 * @brief Change parser global configuration setting using debugfs
 *        file
 */
void __smgr_dbg_flush(char *cmd_buf, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	struct pp_request req;
	enum flush_opts opt;
	char *tok, *args;
	int val, ret;
	u16 port_id;

	memset(&req, 0, sizeof(req));
	req.req_id = req_id++;
	port_id    = PP_PORT_INVALID;
	args = cmd_buf;
	args = strim(args);
	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, flush_tokens, substr);
		switch (opt) {
		case flush_opt_help:
			if (strcmp(tok, "help"))
				pr_info("Error: Invalid option '%s'\n", tok);
			__smgr_dbg_flush_help();
			return;
		case flush_opt_all:
			port_id = PP_MAX_PORT;
			break;
		case flush_opt_pid:
			if (match_int(&substr[0], &val))
				return;
			port_id = (u16)val;
			break;
		case flush_opt_async:
			req.cb = __smgr_dbg_cb;
			break;
		default:
			break;
		}
	}

	if (port_id == PP_PORT_INVALID)
		return;

	if (port_id == PP_MAX_PORT) {
		ret = pp_flush_all((req.cb ? &req : NULL));
		pr_info("pp_flush_all %s, ret = %d\n",
			ret ? "failed" : "done", ret);
	} else {
		ret = pp_port_flush(port_id, (req.cb ? &req : NULL));
		pr_info("pp_port_flush %s, ret = %d\n",
			ret ? "failed" : "done", ret);
	}
}

PP_DEFINE_DEBUGFS(flush, NULL, __smgr_dbg_flush);

/**
 * @brief Get session manager enable/disable state
 * @param data unused
 * @param val enable/disable
 * @return 0 on success, non-zero value otherwise
 */
static int __smgr_dbg_enable_get(void *data, u64 *val)
{
	*val = (u64)(smgr_state_get() == SMGR_ENABLE);
	return 0;
}

/**
 * @brief Enable/Disable session manager
 * @param data not used
 * @param val 1 to enable, 0 to disable
 * @return 0 on success, non-zero value otherwise
 */
static int __smgr_dbg_enable_set(void *data, u64 val)
{
	return smgr_state_set(val ? SMGR_ENABLE : SMGR_DISABLE);
}

PP_DEFINE_DBGFS_ATT(enable, __smgr_dbg_enable_get, __smgr_dbg_enable_set);

enum sess_set_dq_opts {
	sess_set_dq_opt_help,
	sess_set_dq_opt_id,
	sess_set_dq_opt_queue,
};

static const match_table_t sess_set_dq_tokens = {
	{sess_set_dq_opt_help,  "help"},
	{sess_set_dq_opt_id,    "id=%u"},
	{sess_set_dq_opt_queue, "queue=%u"},
};

/**
 * @brief Session delete help
 */
static void __smgr_dbg_sess_set_dq_help(void)
{
	pr_info("\n");
	pr_info(" Brief: Change session's destination queue\n");
	pr_info(" Usage: echo <option>[=value] > session_dest_queue\n");
	pr_info(" Options:\n");
	pr_info("   help  - print this help function\n");
	pr_info("   id    - session id\n");
	pr_info("   queue - new destination queue logical id\n");
	pr_info(" Examples:\n");
	pr_info("   set session 5 dest queue to 10:\n");
	pr_info("     echo id=5 queue=10 > session_dest_queue\n");
}

/**
 * @brief Set session's destination queue
 */
void __smgr_dbg_session_dst_q_set(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum sess_set_dq_opts opt;
	char *tok;
	int val, ret;
	u32 sess_id;
	u16 queue = PP_QOS_INVALID_ID; /* expect logical id */

	sess_id = SMGR_INVALID_SESS_ID;
	args = strim(args);
	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, sess_set_dq_tokens, substr);
		switch (opt) {
		case sess_set_dq_opt_help:
			if (strcmp(tok, "help"))
				pr_info("Error: Invalid option '%s'\n", tok);
			__smgr_dbg_sess_set_dq_help();
			goto opt_parse_err;
		case sess_set_dq_opt_id:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			sess_id = (u32)val;
			break;
		case sess_set_dq_opt_queue:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			queue = (u16)val;
			break;
		default:
			break;
		}
	}
	if (sess_id == SMGR_INVALID_SESS_ID)
		return;

	ret = pp_session_dst_queue_update(sess_id, queue);
	if (unlikely(ret))
		pr_info("Failed to set session %u dest queue to %u, ret %d\n",
			sess_id, queue, ret);
	else
		pr_info("Session %u dest queue set to %u\n", sess_id, queue);
	return;

opt_parse_err:
	pr_err("Failed to parse input\n");
}

PP_DEFINE_DEBUGFS(sess_dst_q_set, NULL, __smgr_dbg_session_dst_q_set);

enum sess_stats_mod_opts {
	sess_stats_mod_opt_help,
	sess_stats_mod_opt_id,
	sess_stats_mod_opt_act,
	sess_stats_mod_opt_pkts,
	sess_stats_mod_opt_bytes,
};

static const match_table_t sess_stats_mod_tokens = {
	{sess_stats_mod_opt_help,  "help"},
	{sess_stats_mod_opt_id,    "id=%u"},
	{sess_stats_mod_opt_act,   "act=%u"},
	{sess_stats_mod_opt_pkts,  "pkts=%u"},
	{sess_stats_mod_opt_bytes, "bytes=%u"},
};

/**
 * @brief Session delete help
 */
static void __smgr_dbg_sess_stats_mod_help(void)
{
	pr_info("\n");
	pr_info(" Brief: Modify session's stats\n");
	pr_info(" Usage: echo <option>[=value] ... > session_stats_mod\n");
	pr_info(" Options:\n");
	pr_info("   help  - print this help function\n");
	pr_info("   id    - session id\n");
	pr_info("   act   - specify which modification to do\n");
	pr_info("           %u - decrement\n", PP_STATS_SUB);
	pr_info("           %u - increment\n", PP_STATS_ADD);
	pr_info("           %u - reset\n",     PP_STATS_RESET);
	pr_info("   pkts  - number of packets to dec/inc\n");
	pr_info("   bytes - number of bytes to dec/inc\n");
	pr_info(" Examples:\n");
	pr_info("   reset session 5 stats:\n");
	pr_info("     echo id=5 act=2 > session_stats_mod\n");
	pr_info("   increment session 5 by 10 packets and 640 bytes:\n");
	pr_info("     echo id=5 act=1 pkts=10 bytes=640 > session_stats_mod\n");
	pr_info("   decrement session 5 by 10 packets and 640 bytes:\n");
	pr_info("     echo id=5 act=0 pkts=10 bytes=640 > session_stats_mod\n");
}

/**
 * @brief Modify session stats
 */
void __smgr_dbg_session_stats_mod(char *args, void *data)

{
	substring_t substr[MAX_OPT_ARGS];
	enum sess_stats_mod_opts opt;
	enum pp_stats_op act;
	char *tok;
	int val, ret;
	u32 sess_id, bytes;
	u8  pkts;

	sess_id = SMGR_INVALID_SESS_ID;
	act     = PP_STATS_OP_CNT;
	bytes   = 0;
	pkts    = 0;
	args = strim(args);
	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, sess_stats_mod_tokens, substr);
		switch (opt) {
		case sess_stats_mod_opt_help:
			if (strcmp(tok, "help"))
				pr_info("Error: Invalid option '%s'\n", tok);
			__smgr_dbg_sess_stats_mod_help();
			return;
		case sess_stats_mod_opt_id:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			sess_id = (u32)val;
			break;
		case sess_stats_mod_opt_act:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			act = (enum pp_stats_op)val;
			break;
		case sess_stats_mod_opt_pkts:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			pkts = (u8)val;
			break;
		case sess_stats_mod_opt_bytes:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			bytes = (u32)val;
			break;
		default:
			break;
		}
	}
	if (sess_id == SMGR_INVALID_SESS_ID)
		return;

	ret = pp_session_stats_mod(sess_id, act, pkts, bytes);
	if (unlikely(ret))
		pr_err("Failed to modify session %u stats, ret %d\n",
		       sess_id, ret);
	else
		pr_info("Session %u stats modified!\n", sess_id);

	return;

opt_parse_err:
	pr_err("Failed to parse input\n");
}

PP_DEFINE_DEBUGFS(sess_stats_mod, NULL, __smgr_dbg_session_stats_mod);

enum dtbm_opts {
	dtbm_opt_help = 1,
	dtbm_opt_get,
	dtbm_opt_set,
	dtbm_opt_alloc,
	dtbm_opt_free,
	dtbm_opt_idx,
	dtbm_opt_enable,
	dtbm_opt_mode,
	dtbm_opt_len_type,
	dtbm_opt_cir,
	dtbm_opt_cir_hi,
	dtbm_opt_cbs,
	dtbm_opt_pir,
	dtbm_opt_pir_hi,
	dtbm_opt_pbs,
};

enum dtbm_action {
	DTBM_ACT_GET,
	DTBM_ACT_SET,
	DTBM_ACT_ALLOC,
	DTBM_ACT_FREE,
	DTBM_NUM_ACTIONS
};

static const match_table_t dtbm_tokens = {
	{dtbm_opt_help,      "help"},
	{dtbm_opt_get,       "get"},
	{dtbm_opt_set,       "set"},
	{dtbm_opt_alloc,     "alloc"},
	{dtbm_opt_free,      "free"},
	{dtbm_opt_idx,       "idx=%u"},
	{dtbm_opt_enable,    "en=%u"},
	{dtbm_opt_mode,      "mode=%u"},
	{dtbm_opt_len_type,  "type=%u"},
	{dtbm_opt_cir,       "cir=%x"},
	{dtbm_opt_cir_hi,    "cir_hi=%x"},
	{dtbm_opt_cbs,       "cbs=%u"},
	{dtbm_opt_pir,       "pir=%x"},
	{dtbm_opt_pir_hi,    "pir_hi=%x"},
	{dtbm_opt_pbs,       "pbs=%u"},
};

/**
 * @brief dual TBM file help
 */
static void __smgr_dbg_dtbm_help(void)
{
	u32 i;

	pr_info("\n");
	pr_info(" Brief: set/get/alloc/free dual TBMs\n");
	pr_info(" Usage: echo <action> [option=value] ... > tbm\n");
	pr_info("    Print all allocated dual TBMs:\n");
	pr_info("        cat tbm\n");
	pr_info(" Actions:\n");
	pr_info("   help   - print this help\n");
	pr_info("   get    - print dual tbm configuration\n");
	pr_info("   set    - set dual tbm configuration\n");
	pr_info("   alloc  - alloc dual tbm\n");
	pr_info("   free   - free dual tbm\n");
	pr_info(" Options:\n");
	pr_info("   idx    - dual tbm index\n");
	pr_info("   en     - toggle enable/disable\n");
	pr_info("   cir    - committed info rate 32 lower bit, use hex value\n");
	pr_info("   cir_hi - committed info rate 32 higher bits, use hex value\n");
	pr_info("   cbs    - committed burst size\n");
	pr_info("   pir    - peak info rate 32 lower bits, use hex value\n");
	pr_info("   pir_hi - peak info rate 32 higher bits, use hex value\n");
	pr_info("   pbs    - peak burst size\n");
	pr_info("   mode   - color aware/blind mode\n");
	for (i = 0; i < PP_TBM_MODES_NUM; i++)
		pr_info("            %u - RFC %s\n", i, chk_tbm_mode_str(i));
	pr_info("   type   - packet length type to use\n");
	for (i = 0; i < PP_TBM_LEN_TYPES_NUM; i++)
		pr_info("            %u - %s length\n", i,
			chk_tbm_len_type_str(i));

	pr_info(" Examples:\n");
}

/**
 * @brief TBM get/set/alloc/free
 */
static void __smgr_dbg_dtbm(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum dtbm_opts opt;
	char *tok;
	int val, ret = 0;
	u16 idx;
	enum dtbm_action action;
	struct pp_dual_tbm cfg = { 0 };
	u64 cir, pir;

	idx       = CHK_DUAL_TBM_NUM;
	action    = DTBM_NUM_ACTIONS;
	args      = strim(args);
	cir       = 0;
	pir       = 0;

	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, dtbm_tokens, substr);
		switch (opt) {
		case dtbm_opt_help:
			__smgr_dbg_dtbm_help();
			return;
		case dtbm_opt_get:
			action = DTBM_ACT_GET;
			break;
		case dtbm_opt_set:
			action = DTBM_ACT_SET;
			break;
		case dtbm_opt_alloc:
			action = DTBM_ACT_ALLOC;
			break;
		case dtbm_opt_free:
			action = DTBM_ACT_FREE;
			break;
		case dtbm_opt_idx:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			idx = (u16)val;

			if (action == DTBM_ACT_SET) {
				ret = chk_dual_tbm_get(idx, &cfg);
				if (unlikely(ret)) {
					pr_err("failed to get tbm %u config\n",
					       idx);
					return;
				}
			}
			break;
		case dtbm_opt_enable:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.enable = !!val;
			break;
		case dtbm_opt_mode:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.mode = val;
			break;
		case dtbm_opt_len_type:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.len_type = val;
			break;
		case dtbm_opt_cir:
			if (match_hex(&substr[0], &val))
				goto opt_parse_err;
			cir     |= (u32)val;
			cfg.cir  = cir;
			break;
		case dtbm_opt_cir_hi:
			if (match_hex(&substr[0], &val))
				goto opt_parse_err;
			cir     |= (u64)val << BITS_PER_U32;
			cfg.cir  = cir;
			break;
		case dtbm_opt_cbs:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.cbs = (u32)val;
			break;
		case dtbm_opt_pir:
			if (match_hex(&substr[0], &val))
				goto opt_parse_err;
			pir     |= (u32)val;
			cfg.pir  = pir;
			break;
		case dtbm_opt_pir_hi:
			if (match_hex(&substr[0], &val))
				goto opt_parse_err;
			pir     |= (u64)val << BITS_PER_U32;
			cfg.pir |= pir;
			break;
		case dtbm_opt_pbs:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.pbs = (u32)val;
			break;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			return;
		}
	}

	switch (action) {
	case DTBM_ACT_ALLOC:
		ret = pp_dual_tbm_alloc(&idx, &cfg);
		if (unlikely(ret))
			pr_info("failed to allocate dual tbm, ret %d\n", ret);
		else
			pr_info("dual tbm %u allocated\n", idx);
		break;
	case DTBM_ACT_FREE:
		ret = pp_dual_tbm_free(idx);
		if (unlikely(ret))
			pr_info("failed to free dual tbm %u, ret %d\n", idx,
				ret);
		else
			pr_info("dual tbm %u freed\n", idx);
		break;
	case DTBM_ACT_SET:
		ret = pp_dual_tbm_set(idx, &cfg);
		if (unlikely(ret))
			pr_info("failed to set dual tbm %u config, ret %d\n",
				idx, ret);
		else
			pr_info("dual tbm %u config set\n", idx);
		break;
	case DTBM_ACT_GET:
		ret = pp_dual_tbm_get(idx, &cfg);
		if (unlikely(ret)) {
			pr_info("failed to get dual tbm %u config, ret %d\n",
				idx, ret);
		} else {
			pr_info("dual tbm %03u: enable %s, cir %llu cbs %u, pir %llu, pbs %u, mode %u, len_type %u\n",
				idx, BOOL2STR(cfg.enable), cfg.cir, cfg.cbs,
				cfg.pir, cfg.pbs, cfg.mode, cfg.len_type);
		}
		break;
	default:
		break;
	}

	if (unlikely(ret))
		pr_err("operation failed, ret = %d\n", ret);
	return;

opt_parse_err:
	pr_err("Failed to parse input\n");
}

/**
 * @brief Search which sessions, from the bitmap, are using the
 *        specified tbm
 * @param tbm the tbm
 * @param bmap the bitmap, the bitmap will be changed to contains
 *        only the sessions who uses the tbm
 * @param n_sessions number of sessions
 */
void __smgr_dbg_tbm_sessions_get(u16 tbm, ulong *bmap, u32 n_sessions)
{
	struct pp_si si = { 0 };
	u32 sess;
	s32 ret;
	u16 *si_tbm_idx;
	bool found;

	/* get all opened sessions */
	ret = smgr_open_sessions_bmap_get(bmap, n_sessions);
	if (unlikely(ret))
		return;

	/* iterate over all sessions and check who is using the tbm */
	for_each_set_bit(sess, bmap, n_sessions) {
		smgr_session_si_get(sess, &si);
		/* check if the tbm is used by the session */
		found = false;
		for_each_arr_entry(si_tbm_idx, si.tbm, ARRAY_SIZE(si.tbm)) {
			/* tbm is found, break the loop */
			if (*si_tbm_idx == tbm) {
				found = true;
				break;
			}
		}
		if (!found)
			clear_bit(sess, bmap);
	}
}

void __smgr_dbg_dtbm_show_all(struct seq_file *f)
{
	struct pp_dual_tbm cfg = { 0 };
	ulong *sessions;
	s32 ret;
	u32 n_tbm_sess, n_sessions;
	u16 i;

	ret = smgr_sessions_bmap_alloc(&sessions, &n_sessions);
	if (unlikely(ret))
		return;

	seq_puts(f, "\n");
	seq_puts(f,
		 " +----------------------------------------------------------------------------------------------------------------------------------------+\n");
	seq_printf(f, " | %-3s | %-6s | %-20s | %-10s | %-20s | %-10s | %-16s | %-17s | %-8s |\n",
		   "Idx", "Enable", "CIR(bps)", "CBS(Bps)", "PIR(bps)",
		   "PBS(Bps)", "Mode", "Len Type", "Sessions");
	seq_puts(f,
		 " +----------------------------------------------------------------------------------------------------------------------------------------+\n");

	for (i = 0; i < CHK_DUAL_TBM_NUM; i++) {
		if (!chk_dual_tbm_is_allocated(i))
			continue;

		ret = chk_dual_tbm_get(i, &cfg);
		if (unlikely(ret))
			goto done;

		ret = chk_dual_tbm_num_sessions_get(i, &n_tbm_sess);
		if (unlikely(ret))
			goto done;

		__smgr_dbg_tbm_sessions_get(i, sessions, n_sessions);

		seq_printf(f, " | %-3u | %-6s", i, BOOL2STR(cfg.enable));
		seq_printf(f, " | %-20llu | %-10u", cfg.cir * BITS_PER_BYTE,
			   cfg.cbs);
		seq_printf(f, " | %-20llu | %-10u", cfg.pir * BITS_PER_BYTE,
			   cfg.pbs);
		seq_printf(f, " | %-16s", chk_tbm_mode_str(cfg.mode));
		seq_printf(f, " | %-17s", chk_tbm_len_type_str(cfg.len_type));
		seq_printf(f, " | %-8u: ", n_tbm_sess);
		seq_printf(f, "%*pbl", n_sessions, sessions);
		seq_puts(f, "\n");
		seq_puts(f, " +-----+--------+----------------------+------------+----------------------+------------+------------------+-------------------+----------+\n");
	}
done:
	seq_puts(f, "\n");
	kfree(sessions);
}

PP_DEFINE_DEBUGFS(dual_tbm, __smgr_dbg_dtbm_show_all, __smgr_dbg_dtbm);

enum sgc_opts {
	sgc_opt_help = 1,
	sgc_opt_owner,
	sgc_opt_grp,
	sgc_opt_cntr,
	sgc_opt_reset,
	sgc_opt_pkts,
	sgc_opt_bytes,
	sgc_opt_print,
	sgc_opt_alloc,
	sgc_opt_free,
	sgc_opt_res,
	sgc_opt_unres,
	sgc_opt_add,
	sgc_opt_sub,
};

enum sgc_action {
	SGC_ACT_HELP,
	SGC_ACT_GET,
	SGC_ACT_MOD,
	SGC_ACT_ALLOC,
	SGC_ACT_FREE,
	SGC_ACT_RES,
	SGC_ACT_UNRES,
	SGC_NUM_ACTIONS
};

struct sgc_dbg_args {
	enum pp_stats_op op;
	enum sgc_action act;
	struct pp_stats stats;
	u32 n_sessions;
	u32 owner;
	u8  grp;
	u16 cntr;
};

static const match_table_t sgc_tokens = {
	{sgc_opt_help,  "help"},
	{sgc_opt_print, "print"},
	{sgc_opt_alloc, "alloc"},
	{sgc_opt_free,  "free"},
	{sgc_opt_res,   "res"},
	{sgc_opt_unres, "unres"},
	{sgc_opt_add,   "add"},
	{sgc_opt_sub,   "sub"},
	{sgc_opt_reset, "reset"},
	{sgc_opt_owner, "owner=%u"},
	{sgc_opt_grp,   "grp=%u"},
	{sgc_opt_cntr,  "cntr=%u"},
	{sgc_opt_pkts,  "pkts=%u"},
	{sgc_opt_bytes, "bytes=%u"},
};

/**
 * @brief SGC file help
 */
static void __smgr_dbg_sgc_help(void)
{
	pr_info("\n");
	pr_info(" Brief: Modify/print/alloc/free SGC\n");
	pr_info(" Usage: echo <action> <option>[=value] ... > sgc\n");
	pr_info(" Actions:\n");
	pr_info("   help  - print this help\n");
	pr_info("   add   - add to sgc\n");
	pr_info("   sub   - sub from sgc\n");
	pr_info("   reset - reset sgc\n");
	pr_info("   print - print sgc\n");
	pr_info("   alloc - alloc sgc\n");
	pr_info("   free  - free sgc\n");
	pr_info("   res   - reserved group\n");
	pr_info(" Options:\n");
	pr_info("   owner - owner index\n");
	pr_info("   grp   - group index\n");
	pr_info("   cntr  - counter index within the group\n");
	pr_info("   pkts  - number of packets to dec/inc\n");
	pr_info("   bytes - number of bytes to dec/inc\n");
	pr_info(" Examples:\n");
	pr_info("   allocate new sgc from group 3:\n");
	pr_info("     echo alloc grp=3 > sgc\n");
	pr_info("   free sgc 1 from group 3:\n");
	pr_info("     echo free grp=3 cntr=1 > sgc\n");
	pr_info("   reset counter 5 in group 1 stats:\n");
	pr_info("     echo grp=1 cntr=5 act=2 > sgc\n");
	pr_info("   add 10 packets and 640 bytes to counter 5 in group 1:\n");
	pr_info("     echo add grp=1 cntr=5 pkts=10 bytes=640 > sgc\n");
	pr_info("   reserved group 5\n");
	pr_info("     echo res grp=5 > sgc\n");
}

static s32 __smgr_dbg_sgc_args_parse(char *args_str, struct sgc_dbg_args *args)
{
	substring_t substr[MAX_OPT_ARGS];
	enum sgc_opts opt;
	char *tok;
	s32 val;

	args_str = strim(args_str);
	/* iterate over user arguments */
	while ((tok = strsep(&args_str, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, sgc_tokens, substr);
		switch (opt) {
		case sgc_opt_help:
			args->act = SGC_ACT_HELP;
			break;
		case sgc_opt_owner:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			args->owner = (u32)val;
			break;
		case sgc_opt_grp:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			args->grp = (u8)val;
			break;
		case sgc_opt_cntr:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			args->cntr = (u16)val;
			break;
		case sgc_opt_pkts:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			args->stats.packets = (u64)val;
			break;
		case sgc_opt_bytes:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			args->stats.bytes = (u64)val;
			break;
		case sgc_opt_add:
			args->act = SGC_ACT_MOD;
			args->op = PP_STATS_ADD;
			break;
		case sgc_opt_sub:
			args->act = SGC_ACT_MOD;
			args->op = PP_STATS_SUB;
			break;
		case sgc_opt_reset:
			args->act = SGC_ACT_MOD;
			args->op = PP_STATS_RESET;
			break;
		case sgc_opt_print:
			args->act = SGC_ACT_GET;
			break;
		case sgc_opt_alloc:
			args->act = SGC_ACT_ALLOC;
			break;
		case sgc_opt_free:
			args->act = SGC_ACT_FREE;
			break;
		case sgc_opt_res:
			args->act = SGC_ACT_RES;
			break;
		case sgc_opt_unres:
			args->act = SGC_ACT_UNRES;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			return -EINVAL;
		}
	}
	return 0;

opt_parse_err:
	pr_err("Failed to parse %s value\n", tok);
	return -EINVAL;
}

/**
 * @brief Modify SGC
 */
static void __smgr_dbg_sgc(char *args_str, void *data)
{
	struct sgc_dbg_args args = { 0 };
	s32 ret = 0;

	args.grp   = PP_SI_SGC_MAX;
	args.cntr  = PP_SGC_INVALID;
	args.op    = PP_STATS_OP_CNT;
	args.act   = SGC_NUM_ACTIONS;
	args.owner = PP_SGC_SHARED_OWNER;

	ret = __smgr_dbg_sgc_args_parse(args_str, &args);
	if (unlikely(ret))
		return;

	switch (args.act) {
	case SGC_ACT_HELP:
		__smgr_dbg_sgc_help();
		break;
	case SGC_ACT_GET:
		ret = pp_sgc_get(args.grp, args.cntr, &args.stats,
				 &args.n_sessions);
		if (unlikely(ret))
			pr_info("Failed to read SGC [%u][%04u]\n", args.grp,
				args.cntr);
		else
			pr_info("SGC [%u][%04u]: packets %llu, bytes %llu, num sessions %u\n",
				args.grp, args.cntr, args.stats.packets,
				args.stats.bytes, args.n_sessions);
		break;
	case SGC_ACT_MOD:
		ret = pp_sgc_mod(args.owner, args.grp, args.cntr, args.op,
				 (u32)args.stats.packets,
				 (u32)args.stats.bytes);
		if (unlikely(ret))
			pr_err("Failed to modify SGC [%u][%04u]\n", args.grp,
			       args.cntr);
		else
			pr_info("SGC [%u][%04u] modified!\n", args.grp,
				args.cntr);
		break;
	case SGC_ACT_ALLOC:
		ret = pp_sgc_alloc(args.owner, args.grp, &args.cntr, 1);
		if (unlikely(ret))
			pr_info("Failed to allocate SGC from group %u\n",
				args.grp);
		else
			pr_info("SGC [%u][%04u] allocated\n", args.grp,
				args.cntr);
		break;
	case SGC_ACT_FREE:
		ret = pp_sgc_free(args.owner, args.grp, &args.cntr, 1);
		if (unlikely(ret))
			pr_info("Failed to free SGC [%u][%04u]\n", args.grp,
				args.cntr);
		else
			pr_info("SGC [%u][%04u] freed\n", args.grp, args.cntr);
		break;
	case SGC_ACT_RES:
		ret = pp_sgc_group_reserve(args.grp, &args.owner);
		if (unlikely(ret))
			pr_info("Failed to reserve SGC group %u\n", args.grp);
		else
			pr_info("SGC group %u reserve to owner %u\n", args.grp,
				args.owner);
		break;
	case SGC_ACT_UNRES:
		ret = pp_sgc_group_unreserve(args.grp, args.owner);
		if (unlikely(ret))
			pr_info("Failed to unreserve SGC group %u\n", args.grp);
		else
			pr_info("SGC group %u unreserved to owner %u\n",
				args.grp, args.owner);
		break;
	default:
		pr_info("invalid action %u\n", args.act);
		break;
	};

	if (unlikely(ret))
		pr_err("operation failed, ret = %d\n", ret);
}

void __smgr_dbg_sgc_show_all(struct seq_file *f)
{
	struct pp_stats stats = { 0 };
	ulong *sessions;
	u32 grp, cntr, cntr_sessions, owner, n_cntrs, n_sessions;
	s32 ret = 0;

	ret = smgr_sessions_bmap_alloc(&sessions, &n_sessions);
	if (unlikely(ret))
		return;

	seq_puts(f, "\n");
	seq_puts(f,
		 " +-------+-------+-------+-----------------+----------------------+------------+\n");
	seq_printf(f, " | %-5s | %-5s | %-5s | %-15s | %-20s | %-10s |\n",
		   "Group", "Index", "Owner", "Packets", "Bytes", "Sessions");
	seq_puts(f,
		 " +-------+-------+-------+-----------------+----------------------+------------+\n");
	for (grp = 0; grp < PP_SI_SGC_MAX; grp++) {
		ret = chk_sgc_group_info_get(grp, &n_cntrs, &owner);
		if (unlikely(ret))
			goto done;

		for (cntr = 0; cntr < n_cntrs; cntr++) {
			ret |= pp_sgc_get(grp, cntr, &stats, &cntr_sessions);
			ret |= smgr_sgc_sessions_bmap_get(grp, cntr, sessions,
							  n_sessions);
			if (unlikely(ret))
				goto done;

			/* if no session is using the counter and no packets
			 * were counted, skip it
			 */
			if (!stats.bytes && !stats.packets && !cntr_sessions)
				continue;

			seq_printf(f, " | %-5u | %-5u | %-5u", grp, cntr,
				   owner);
			seq_printf(f, " | %-15llu | %-20llu", stats.packets,
				   stats.bytes);
			seq_printf(f, " | %-5u: ", cntr_sessions);
			seq_printf(f, "%*pbl", n_sessions, sessions);
			seq_puts(f, "\n");
		}
	}
done:
	seq_puts(f,
		 " +-------+-------+-------+-----------------+----------------------+------------+\n");
	kfree(sessions);
}

PP_DEFINE_DEBUGFS(sgc, __smgr_dbg_sgc_show_all, __smgr_dbg_sgc);

s32 __smgr_reass_stats_get(void *_stats, u32 num_stats, void *data)
{
	struct reassembly_stats *stats;
	s32 ret = 0;
	u32 cid;

	stats = (struct reassembly_stats *)_stats;
	for (cid = 0; cid < num_stats; cid++) {
		ret = uc_reass_cpu_stats_get(cid, &stats[cid]);
		if (unlikely(ret)) {
			pr_err("failed to get reassembly uc cpu %u counters\n",
			       cid);
			break;
		}
	}

	return ret;
}

s32 __smgr_reass_stats_diff(void *pre, u32 num_pre, void *post, u32 num_post,
			    void *delta, u32 num_delta, void *data)
{
	struct reassembly_stats *__pre, *__post, *__delta;
	u32 i;

	if (unlikely(ptr_is_null(pre) || ptr_is_null(post) ||
		     ptr_is_null(delta)))
		return -EINVAL;

	__pre   = pre;
	__post  = post;
	__delta = delta;
	for (i = 0; i < num_pre; i++)
		U32_STRUCT_DIFF(&__pre[i], &__post[i], &__delta[i]);

	return 0;
}

s32 __smgr_reass_stats_show(char *buf, size_t sz, size_t *n, void *s, u32 num,
			    void *data)
{
	struct reassembly_stats *stats, *it;
	u32 cid;
	u64 total;
	char **str;
	static const char * const cntrs_str[] = {
		"rx_pkts",
		"tx_pkts",
		"reassembled",
		"accelerated",
		"diverted",
		"early_diverted",
		"matched",
		"not_matched",
		"cntxs_starv",
		"cntxs_busy",
		"cntx_overflow",
		"timedout",
		"timeout_err",
		"frags_starv",
		"duplicates",
		"unsupported_frag",
		"unsupported_ext",
		"unsupported_proto",
		"unsupported_pmac",
		"unsupported_pre_l2",
		"ext_overflow",
		"invalid_len",
		"dropped",
		"bm_null_buff",
	};

	pr_buf(buf, sz, *n, "\n");
	pr_buf_cat(buf, sz, *n,
		   "|=====================================================================================|\n");
	pr_buf_cat(buf, sz, *n,
		   "|                 Reassembly UC Statistics                                            |\n");
	pr_buf_cat(buf, sz, *n,
		   "|=====================================================================================|\n");
	pr_buf_cat(buf, sz, *n,
		   "| %-18s ", "Counter");
	for (cid = 0; cid < num; cid++)
		pr_buf_cat(buf, sz, *n, "|  CPU%u      ", cid);
	pr_buf_cat(buf, sz, *n, "|  Total     |\n");
	pr_buf_cat(buf, sz, *n,
		   "|--------------------+------------+------------+------------+------------+------------|\n");

	cid = 0;
	stats = (struct reassembly_stats *)s;
	for_each_arr_entry(str, cntrs_str, ARRAY_SIZE(cntrs_str), cid++) {
		total = 0;
		pr_buf_cat(buf, sz, *n, "| %-18s ", *str);
		for_each_arr_entry(it, stats, num) {
			pr_buf_cat(buf, sz, *n, "| %10llu ",
				   *(((u64 *)it) + cid));
			total += *(((u64 *)it) + cid);
		}
		pr_buf_cat(buf, sz, *n, "| %10llu |\n", total);
	}
	pr_buf_cat(buf, sz, *n,
		   "|-------------------------------------------------------------------------------------|\n\n");

	return 0;
}

void __smgr_dbg_reass_stats_show(struct seq_file *f)
{
	pp_stats_show_seq(f, sizeof(struct reassembly_stats),
			  UC_CPUS_MAX, __smgr_reass_stats_get,
			  __smgr_reass_stats_show, NULL);
}

PP_DEFINE_DEBUGFS(reass_stats, __smgr_dbg_reass_stats_show, NULL);

void __smgr_dbg_reass_pps_show(struct seq_file *f)
{
	pp_pps_show_seq(f, sizeof(struct reassembly_stats), UC_CPUS_MAX,
			__smgr_reass_stats_get, __smgr_reass_stats_diff,
			__smgr_reass_stats_show, NULL);
}

PP_DEFINE_DEBUGFS(reass_pps, __smgr_dbg_reass_pps_show, NULL);

static int __smgr_dbg_reass_timeout_set(void *data, u64 val)
{
	return uc_egr_mbox_cmd_send(UC_CMD_REASSEMBLY_TIMEOUT_THR, (u32)val,
				    NULL, 0, NULL, 0);
}

PP_DEFINE_DBGFS_ATT(reass_timeout, NULL, __smgr_dbg_reass_timeout_set);

#define PP_MAX_FV_WORD_SIZE    32
#define PP_MAX_FV_OFFSET_IN_SI 32

/**
 * @brief Dump FV comparison from xsi and the classifier
 */
void __smgr_dbg_session_compare_last_fv_dump(struct seq_file *f)
{
	struct pp_hw_si hw_si;
	u32 sess_id;
	s32 ret, i, offset;
	u32 fv[PP_MAX_FV_WORD_SIZE];
	u32 fv_sz;
	bool full_match = true;
	bool match;
	struct sess_db_info info;

	memset(&hw_si,  0, sizeof(hw_si));

	if (smgr_session_info_get(dbg_sess_id, &info)) {
		pr_err("Session %u does not exist!", dbg_sess_id);
		return;
	}

	ret = cls_session_si_get(dbg_sess_id, &hw_si);
	if (ret)
		return;

	ret = pp_si_fld_get(&hw_si, SI_FLD_SESS_ID, (s32 *)&sess_id);
	if (ret)
		return;

	ret = pp_si_fld_get(&hw_si, SI_FLD_FV_SZ,  (s32 *)&fv_sz);
	if (ret)
		return;

	seq_printf(f, "\n Session %u FV size %u\n", sess_id, fv_sz);
	seq_puts(f,   " =========================\n");

	if (unlikely(cls_dbg_last_fv_get(fv))) {
		pr_err("failed to get last fv\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +---------------+------------+------------+-----+\n");
	seq_puts(f, " |               |     SI     |    CLSS    | DIF |\n");
	seq_puts(f, " +---------------+------------+------------+-----+\n");
	offset = PP_MAX_FV_WORD_SIZE - (fv_sz >> 2);
	for (i = offset; i < PP_MAX_FV_WORD_SIZE; i++) {
		match = true;
		if (hw_si.word[i + PP_MAX_FV_OFFSET_IN_SI] != fv[i]) {
			full_match = false;
			match = false;
		}
		seq_printf(f, " |   FV WORD%02u   | %#010x | %#010x |  %c  |\n",
			   i, hw_si.word[i + PP_MAX_FV_OFFSET_IN_SI],
			   fv[i], match ? ' ' : 'X');
	}
	seq_puts(f, " +---------------+------------+------------+-----+\n\n");

	seq_puts(f, " +--------------------+\n");
	if (full_match)
		seq_puts(f, " |      FULL MATCH    |\n");
	else
		seq_puts(f, " |      NO MATCH !    |\n");

	seq_puts(f, " +--------------------+\n\n");
}

PP_DEFINE_DEBUGFS(session_compare_last_fv,
		  __smgr_dbg_session_compare_last_fv_dump, NULL);

s32 sess_update_get_si(char *field, u32 id, struct pp_hw_si *si)
{
	/* Make sure first property is the session id */
	if (strncmp(field, "id", strlen("id"))) {
		pr_err("First prop (%s) must be id\n", field);
		return -EINVAL;
	}

	/* Get session SI */
	if (unlikely(cls_session_si_get(id, si))) {
		pr_err("couldn't get si for session %u", id);
		return -EINVAL;
	}

	return 0;
}

void sess_update_set(char *cmd_buf, void *data)
{
	u8 *field;
	char *tok, *ptr, *pval;
	long res;
	u32 id = 0;
	u32 first_prop = 1;
	struct pp_hw_si si = {0};
	enum pp_si_fld si_fld;

	field = kzalloc(PP_DBGFS_WR_STR_MAX, GFP_KERNEL);
	if (!field)
		return;

	ptr = (char *)cmd_buf;

	while ((tok = strsep(&ptr, " \t\n\r")) != NULL) {
		if (tok[0] == '\0')
			continue;

		strcpy(field, tok);
		pval = strchr(field, '=');
		if (!pval) {
			pr_err("Wrong format for prop %s\n", tok);
			goto out;
		}

		*pval = '\0';
		pval++;

		if (kstrtol(pval, 0, &res)) {
			pr_err("kstrtol failed\n");
			goto out;
		}

		if (first_prop) {
			first_prop = 0;
			if (sess_update_get_si(field, res, &si))
				goto out;
			id = res;
		} else {
			if (!pp_si_str2fld(field, &si_fld))
				pp_si_fld_set(&si, si_fld, res);
			else
				pr_err("Unsupported field %s\n", field);
		}
	}

	if (smgr_session_update(id, &si))
		pr_err("pp_session_update failed (id %u)", id);

out:
	kfree(field);
}

void sess_update_help(struct seq_file *f)
{
	u32 idx;

	seq_puts(f, "echo id=[sess_id] [fld]=[value]... > sess_update\n");
	seq_puts(f, "Supported fields:\n");
	seq_puts(f, "=================\n");

	for (idx = 0; idx < SI_FLD_LAST; idx++)
		seq_printf(f, "%s\n", pp_si_fld2str(idx));
}

PP_DEFINE_DEBUGFS(sess_update, sess_update_help, sess_update_set);

enum sess_mirroring_opts {
	sess_mirroring_opt_enable = 1,
	sess_mirroring_opt_id,
	sess_mirroring_opt_q,
	sess_mirroring_opt_gpid,
	sess_mirroring_opt_ps,
};

static const match_table_t sess_mirroring_tokens = {
	{sess_mirroring_opt_enable,  "en=%u"},
	{sess_mirroring_opt_id, "id=%u"},
	{sess_mirroring_opt_q, "q=%u"},
	{sess_mirroring_opt_gpid,  "gpid=%u"},
	{sess_mirroring_opt_ps,  "ps=%x"},
};

void sess_mirroring_set(char *cmd_buf, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum sess_mirroring_opts opt;
	char *tok;
	s32 val;
	s32 ret = 0;
	struct pp_dsi dsi;
	struct mcast_sess_mirroring_cmd cmd = {0};
	u32 sess_id = 0;
	struct pp_nf_info nf;
	struct pp_port_cfg cfg;

	cmd_buf = strim(cmd_buf);
	/* iterate over user arguments */
	while ((tok = strsep(&cmd_buf, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, sess_mirroring_tokens, substr);
		switch (opt) {
		case sess_mirroring_opt_enable:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cmd.enable = (u32)val;
		case sess_mirroring_opt_id:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			sess_id = (u32)val;
			break;
		case sess_mirroring_opt_q:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cmd.q = (u16)val;
			break;
		case sess_mirroring_opt_gpid:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cmd.gpid = (u16)val;
			break;
		case sess_mirroring_opt_ps:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cmd.ps0 = (u16)val;
			cmd.ps_valid = 1;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			return;
		}
	}

	if (cmd.enable) {
		if (mirroring_orig_q != U16_MAX) {
			pr_info("Error: Session %u mirroring alrady in use\n",
				mirroring_sess_id);
			return;
		}

		/* Get DSI Q */
		if (unlikely(smgr_session_dsi_get(sess_id, &dsi))) {
			pr_err("couldn't get dsi for session %u", sess_id);
			return;
		}

		cmd.orig_q = dsi.dst_q;
		mirroring_orig_q = cmd.orig_q;
		mirroring_sess_id = sess_id;

		if (unlikely(pp_port_get(cmd.gpid, &cfg))) {
			pr_err("pp_port_get eg_port err\n");
			return;
		}

		cmd.eg_port_hr = cfg.tx.headroom_size;
		cmd.eg_port_tr = cfg.tx.tailroom_size;
		cmd.base_policy = cfg.tx.base_policy;
		cmd.policies_map = cfg.tx.policies_map;
		cmd.flags = 0;

		if (cfg.tx.wr_desc)
			cmd.flags |= BUFF_EX_FLAG_WR_DESC;
		if (cfg.tx.prel2_en)
			cmd.flags |= BUFF_EX_FLAG_PREL2_EN;

		ret = uc_egr_mbox_cmd_send(UC_CMD_MCAST_SESS_MIRRORING, 0,
					   (const void *)&cmd, sizeof(cmd),
					   NULL, 0);
		if (ret) {
			pr_err("UC_CMD_MCAST_SESS_MIRRORING command failed\n");
			return;
		}

		/* get the multicast nf info */
		ret = pp_nf_get(PP_NF_MULTICAST, &nf);
		if (ret) {
			pr_err("failed to get the multicast nf info\n");
			mirroring_orig_q = U16_MAX;
			cmd.enable = 0;
			uc_egr_mbox_cmd_send(UC_CMD_MCAST_SESS_MIRRORING, 0,
					     (const void *)&cmd, sizeof(cmd),
					     NULL, 0);
			mirroring_sess_id = U16_MAX;
			return;
		}

		ret = pp_session_dst_queue_update(sess_id, nf.q);
		if (ret) {
			pr_err("failed to set dst queue\n");
			mirroring_orig_q = U16_MAX;
			cmd.enable = 0;
			uc_egr_mbox_cmd_send(UC_CMD_MCAST_SESS_MIRRORING, 0,
					     (const void *)&cmd, sizeof(cmd),
					     NULL, 0);
			mirroring_sess_id = U16_MAX;
			return;
		}
	} else {
		ret = chk_session_dsi_q_update(mirroring_sess_id,
					       mirroring_orig_q);
		if (unlikely(ret)) {
			pr_err("failed to update session %u dest queue to %u\n",
			       mirroring_sess_id, mirroring_orig_q);
			return;
		}

		mirroring_orig_q = U16_MAX;

		ret = uc_egr_mbox_cmd_send(UC_CMD_MCAST_SESS_MIRRORING, 0,
					   (const void *)&cmd, sizeof(cmd),
					   NULL, 0);
		if (ret) {
			pr_err("UC_CMD_MCAST_SESS_MIRRORING command failed\n");
			return;
		}

		mirroring_sess_id = U16_MAX;
	}

	return;

opt_parse_err:
	pr_err("Failed to parse %s value\n", tok);
}

void sess_mirroring_help(struct seq_file *f)
{
	seq_puts(f, "Enable:\n");
	seq_puts(f, "echo en=1 id=[sess_id] q=[new_dst_q] gpid=[new_gpid] > sess_mirroring\n");
	seq_puts(f, "Disable:\n");
	seq_puts(f, "echo en=0 > sess_mirroring\n");
}

PP_DEFINE_DEBUGFS(sess_mirroring, sess_mirroring_help, sess_mirroring_set);

static struct debugfs_file debugfs_files[] = {
	{ "sessions", &PP_DEBUGFS_FOPS(sessions) },
	{ "inact_sessions", &PP_DEBUGFS_FOPS(inact_sessions) },
	{ "xsession", &PP_DEBUGFS_FOPS(session) },
	{ "xsi", &PP_DEBUGFS_FOPS(session_si) },
	{ "exception_xsi", &PP_DEBUGFS_FOPS(exception_session_si) },
	{ "xsi_raw", &PP_DEBUGFS_FOPS(session_si_raw) },
	{ "stats", &PP_DEBUGFS_FOPS(stats) },
	{ "enable", &PP_DEBUGFS_FOPS(enable) },
	{ "delete_session", &PP_DEBUGFS_FOPS(sess_del) },
	{ "flush", &PP_DEBUGFS_FOPS(flush) },
	{ "session_dstq_set", &PP_DEBUGFS_FOPS(sess_dst_q_set) },
	{ "session_stats_mod", &PP_DEBUGFS_FOPS(sess_stats_mod) },
	{ "dual_tbm", &PP_DEBUGFS_FOPS(dual_tbm) },
	{ "sgc", &PP_DEBUGFS_FOPS(sgc) },
	{ "frag_stats", &PP_DEBUGFS_FOPS(frag_stats) },
	{ "tdox_stats", &PP_DEBUGFS_FOPS(tdox_stats) },
	{ "tdox_en",   &PP_DEBUGFS_FOPS(tdox_en_fops) },
	{ "compare_last_fv", &PP_DEBUGFS_FOPS(session_compare_last_fv) },
	{ "sess_update", &PP_DEBUGFS_FOPS(sess_update) },
	{ "sess_mirroring", &PP_DEBUGFS_FOPS(sess_mirroring) },
};

static struct debugfs_file reassembly_files[] = {
	{ "stats", &PP_DEBUGFS_FOPS(reass_stats) },
	{ "pps", &PP_DEBUGFS_FOPS(reass_pps) },
	{ "timeout", &PP_DEBUGFS_FOPS(reass_timeout) },
};

/**
 * @brief Set the multicast destination
 */
static void __smgr_dbg_dst_enable_set(char *cmd_buf, void *data)
{
	u16 grp;
	u8 dst, enable;

	if (unlikely(sscanf(cmd_buf, "%hu %hhu %hhu",
			    &grp, &dst, &enable) != 3)) {
		pr_err("sscanf error\n");
		return;
	}

	if (unlikely(pp_mcast_dst_set(grp, dst, !!enable))) {
		pr_err("failed to set multicast dst, grp=%hu, dst=%hhu\n",
		       grp, dst);
		return;
	}

	pr_info("MCAST_GROUP[%hu] DST_ID[%hhu] STATUS[%s]\n",
		grp, dst, BOOL2EN(enable));
}

/**
 * @brief Dump the multicast destination command help
 */
static void __smgr_dbg_dst_enable_help(struct seq_file *f)
{
	seq_puts(f, " <group-index> <dst-index> <0=disable, 1=enable>\n");
}

PP_DEFINE_DEBUGFS(dst_enable,
		  __smgr_dbg_dst_enable_help,
		  __smgr_dbg_dst_enable_set);

/**
 * @brief Dump the multicast group info
 */
static void __smgr_dbg_group_show(char *cmd_buf, void *data)
{
	u16 grp;
	struct pp_mcast_grp_info info;
	u32 dst;

	if (unlikely(kstrtou16(cmd_buf, 10, &grp))) {
		pr_info("failed to parse '%s'\n", cmd_buf);
		return;
	}

	if (unlikely(pp_mcast_group_info_get(grp, &info))) {
		pr_err("failed to get multicast group %hu info\n", grp);
		return;
	}

	pr_info("Group ID                  = %hu\n", grp);
	if (smgr_is_sess_id_valid(info.base_sess))
		pr_info("Base session ID           = %u\n", info.base_sess);
	else
		pr_info("Base session ID           = %s\n", "INVALID");
	for_each_set_bit(dst, &info.dst_bmap, PP_MCAST_DST_MAX) {
		if (smgr_is_sess_id_valid(info.dst_sess[dst])) {
			pr_info("Destination %u session ID  = %u\n",
				dst, info.dst_sess[dst]);
		} else {
			pr_info("Destination %u session ID  = %s\n",
				dst, "INVALID");
		}
	}
	pr_info("\n");
}

/**
 * @brief Dump the multicast group help
 */
static void __smgr_dbg_group_help(struct seq_file *f)
{
	seq_puts(f, " <group-index>\n");
}

PP_DEFINE_DEBUGFS(group, __smgr_dbg_group_help, __smgr_dbg_group_show);

/**
 * @brief Dump multicast packet counters
 */
static void __smgr_dbg_pkt_stats_show(struct seq_file *f)
{
	struct mcast_stats stats[UC_CPUS_MAX];
	u32 i;
	u64 total_rx  = 0;
	u64 total_tx  = 0;
	u64 total_drp = 0;
	u64 total_mirror_tx  = 0;
	u64 total_mirror_drp = 0;

	seq_puts(f, "\n");
	seq_puts(f, " +--------------------------------------------+\n");
	seq_puts(f, " |                MCAST STATS                 |\n");
	seq_puts(f, " +--------------------------------------------+\n");
	seq_puts(f, " |  CPU ID  |  COUNTER  |        VALUE        |\n");
	seq_puts(f, " +----------+-----------+---------------------+\n");

	for (i = 0; i < UC_CPUS_MAX; i++) {
		if (!uc_is_cpu_active(UC_IS_EGR, i))
			continue;

		if (uc_mcast_cpu_stats_get(i, &stats[i])) {
			seq_puts(f, "failed to get mcast counters\n");
			return;
		}
		seq_printf(f, " | %-8u | %-9s | %-19llu |\n", i, "RX",
			   stats[i].rx_pkt);
		seq_printf(f, " | %-8u | %-9s | %-19llu |\n", i, "TX",
			   stats[i].tx_pkt);
		seq_printf(f, " | %-8u | %-9s | %-19llu |\n", i, "DROP",
			   stats[i].drop_pkt);
		seq_printf(f, " | %-8u | %-9s | %-19llu |\n", i, "MIRROR TX",
			   stats[i].mirror_tx_pkt);
		seq_printf(f, " | %-8u | %-9s | %-19llu |\n", i, "MIRROR DROP",
			   stats[i].mirror_drop_pkt);
		seq_puts(f, " +----------+-----------+---------------------+\n");
		total_rx  += stats[i].rx_pkt;
		total_tx  += stats[i].tx_pkt;
		total_drp += stats[i].drop_pkt;
		total_mirror_tx  += stats[i].mirror_tx_pkt;
		total_mirror_drp += stats[i].mirror_drop_pkt;
	}
	seq_printf(f, " | %-8s | %-9s | %-19llu |\n", "TOTAL", "RX", total_rx);
	seq_printf(f, " | %-8s | %-9s | %-19llu |\n", "TOTAL", "TX", total_tx);
	seq_printf(f, " | %-8s | %-9s | %-19llu |\n", "TOTAL", "DROP",
		   total_drp);
	seq_printf(f, " | %-8s | %-9s | %-19llu |\n", "TOTAL", "MIRROR TX",
		   total_mirror_tx);
	seq_printf(f, " | %-8s | %-9s | %-19llu |\n", "TOTAL", "MIRROR DROP",
		   total_mirror_drp);
	seq_puts(f, " +----------+-----------+---------------------+\n");
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(pkt_stats, __smgr_dbg_pkt_stats_show, NULL);

static struct debugfs_file debugfs_mcast_files[] = {
	{"dst_enable", &PP_DEBUGFS_FOPS(dst_enable)},
	{"group",      &PP_DEBUGFS_FOPS(group)},
	{"pkt_stats",  &PP_DEBUGFS_FOPS(pkt_stats)},
};

/**
 * @brief Debugfs synch queue db show API
 */
void __smgr_dbg_sq_dump_show(struct seq_file *f)
{
	smgr_sq_dbg_dump();
}

PP_DEFINE_DEBUGFS(sq_dump, __smgr_dbg_sq_dump_show, NULL);

/**
 * @brief Set the multicast destination
 */
static void __smgr_dbg_sq_alloc_set(char *cmd_buf, void *data)
{
	u32 sess, dstq_id;

	if (unlikely(sscanf(cmd_buf, "%u %u", &sess, &dstq_id) != 2)) {
		pr_err("sscanf error\n");
		return;
	}

	sq_alloc(sess, dstq_id);
}

/**
 * @brief Dump the multicast destination command help
 */
static void __smgr_dbg_sq_alloc_help(struct seq_file *f)
{
	seq_puts(f, " <session-id> <dstQ-id>\n");
}

PP_DEFINE_DEBUGFS(sq_alloc, __smgr_dbg_sq_alloc_help, __smgr_dbg_sq_alloc_set);

static int __smgr_dbg_sq_start_set(void *data, u64 val)
{
	sq_start(val);
	return 0;
}

PP_DEFINE_DBGFS_ATT(sq_start_fops, NULL, __smgr_dbg_sq_start_set);

static int __smgr_dbg_sq_lspp_set(void *data, u64 val)
{
	smgr_sq_lspp_rcv(val);
	return 0;
}

PP_DEFINE_DBGFS_ATT(sq_lspp_fops, NULL, __smgr_dbg_sq_lspp_set);

static int __smgr_dbg_sq_del_set(void *data, u64 val)
{
	sq_del(val);
	return 0;
}

PP_DEFINE_DBGFS_ATT(sq_del_fops, NULL, __smgr_dbg_sq_del_set);

static int __smgr_dbg_sq_sync_tout_set(void *data, u64 val)
{
	smgr_sq_dbg_sync_tout_set(val);
	return 0;
}

static int __smgr_dbg_sq_sync_tout_get(void *data, u64 *val)
{
	u32 tout;

	smgr_sq_dbg_sync_tout_get(&tout);
	*val = (u64)tout;
	return 0;
}

PP_DEFINE_DBGFS_ATT(sq_sync_tout_fops, __smgr_dbg_sq_sync_tout_get,
		    __smgr_dbg_sq_sync_tout_set);

static int __smgr_dbg_sq_done_tout_set(void *data, u64 val)
{
	smgr_sq_dbg_done_tout_set(val);
	return 0;
}

static int __smgr_dbg_sq_done_tout_get(void *data, u64 *val)
{
	u32 tout;

	smgr_sq_dbg_done_tout_get(&tout);
	*val = (u64)tout;
	return 0;
}

PP_DEFINE_DBGFS_ATT(sq_done_tout_fops, __smgr_dbg_sq_done_tout_get,
		     __smgr_dbg_sq_done_tout_set);

static int __smgr_dbg_sq_lspp_tout_set(void *data, u64 val)
{
	smgr_sq_dbg_lspp_tout_set(val);
	return 0;
}

static int __smgr_dbg_sq_lspp_tout_get(void *data, u64 *val)
{
	u32 tout;

	smgr_sq_dbg_lspp_tout_get(&tout);
	*val = (u64)tout;
	return 0;
}

PP_DEFINE_DBGFS_ATT(sq_lspp_tout_fops, __smgr_dbg_sq_lspp_tout_get,
		    __smgr_dbg_sq_lspp_tout_set);

static int __smgr_dbg_sq_qlen_set(void *data, u64 val)
{
	smgr_sq_dbg_qlen_set(val);
	return 0;
}

static int __smgr_dbg_sq_qlen_get(void *data, u64 *val)
{
	u32 qlen;

	smgr_sq_dbg_qlen_get(&qlen);
	*val = (u64)qlen;
	return 0;
}

PP_DEFINE_DBGFS_ATT(sq_qlen_fops, __smgr_dbg_sq_qlen_get,
		    __smgr_dbg_sq_qlen_set);

static void __smgr_dbg_sq_stats_show(struct seq_file *f)
{
	struct smgr_sq_stats stats;

	smgr_sq_dbg_stats_get(&stats, false);
	seq_puts(f,   "\n");
	seq_puts(f,   " +-------------------------------------------+\n");
	seq_puts(f,   " |           Sync Queues Statistics          |\n");
	seq_puts(f,   " +--------------------------+----------------+\n");
	seq_printf(f, " | %-24s | %-14u |\n", "accepted packets",
		   stats.packets_accepted);
	seq_printf(f, " | %-24s | %-14u |\n", "accepted bytes",
		   stats.bytes_accepted);
	seq_printf(f, " | %-24s | %-14u |\n", "dropped packets",
		   stats.packets_dropped);
	seq_printf(f, " | %-24s | %-14u |\n", "dropped bytes",
		   stats.bytes_dropped);
	seq_printf(f, " | %-24s | %-14u |\n", "sq allocated",
		   stats.allocated);
	seq_printf(f, " | %-24s | %-14u |\n", "sq freed",
		   stats.freed);
	seq_printf(f, " | %-24s | %-14u |\n", "lspp timeout events",
		   stats.lspp_timeout_events);
	seq_printf(f, " | %-24s | %-14u |\n", "invalid sq state error",
		   stats.err_invalid_state);
	seq_printf(f, " | %-24s | %-14u |\n", "queue disconnect error",
		   stats.err_q_disconnect_failed);
	seq_puts(f,   " +--------------------------+----------------+\n");
	seq_puts(f,   "\n");
}

PP_DEFINE_DEBUGFS(sq_stats, __smgr_dbg_sq_stats_show, NULL);

static void __smgr_dbg_sq_reset_stats(struct seq_file *f)
{
	struct smgr_sq_stats stats;

	smgr_sq_dbg_stats_get(&stats, true);
}

PP_DEFINE_DEBUGFS(sq_reset_stats, __smgr_dbg_sq_reset_stats, NULL);

static struct debugfs_file debugfs_syncq_files[] = {
	{"alloc",       &PP_DEBUGFS_FOPS(sq_alloc)},
	{"start",       &PP_DEBUGFS_FOPS(sq_start_fops)},
	{"lspp",        &PP_DEBUGFS_FOPS(sq_lspp_fops)},
	{"del",         &PP_DEBUGFS_FOPS(sq_del_fops)},
	{"tout_sync",   &PP_DEBUGFS_FOPS(sq_sync_tout_fops)},
	{"tout_done",   &PP_DEBUGFS_FOPS(sq_done_tout_fops)},
	{"tout_lspp",   &PP_DEBUGFS_FOPS(sq_lspp_tout_fops)},
	{"qlen",        &PP_DEBUGFS_FOPS(sq_qlen_fops)},
	{"show",        &PP_DEBUGFS_FOPS(sq_dump)},
	{"stats",       &PP_DEBUGFS_FOPS(sq_stats)},
	{"reset_stats", &PP_DEBUGFS_FOPS(sq_reset_stats)},
};

s32 smgr_dbg_init(struct dentry *parent)
{
	s32 ret;

	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	/* session manager debugfs dir */
	ret = pp_debugfs_create(parent, "sess_mgr", &dbgfs, debugfs_files,
				ARRAY_SIZE(debugfs_files), NULL);
	if (unlikely(ret))
		goto done;

	ret = pp_debugfs_create(dbgfs, "reassembly", NULL, reassembly_files,
				ARRAY_SIZE(reassembly_files), NULL);
	if (unlikely(ret))
		goto done;

	ret = pp_debugfs_create(dbgfs, "mcast", NULL, debugfs_mcast_files,
				ARRAY_SIZE(debugfs_mcast_files), NULL);
	if (unlikely(ret))
		goto done;

	ret = pp_debugfs_create(dbgfs, "syncq", NULL, debugfs_syncq_files,
				ARRAY_SIZE(debugfs_syncq_files), NULL);
	if (unlikely(ret))
		goto done;

done:
	if (unlikely(ret))
		pr_err("debugfs file error %d\n", ret);

	return ret;
}

s32 smgr_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;

	return 0;
}
