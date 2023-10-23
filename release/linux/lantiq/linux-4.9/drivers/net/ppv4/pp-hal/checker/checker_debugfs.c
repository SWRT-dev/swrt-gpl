/*
 * Description: Packet Processor Checker Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[CHK_DBG]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/parser.h>
#include <linux/debugfs.h>
#include "pp_debugfs_common.h"
#include "checker.h"
#include "checker_internal.h"
#include "pp_misc.h"

/**
 * @brief main checker debugfs dir
 */
static struct dentry *dbgfs;

void sq_enable_help(struct seq_file *f)
{
	seq_puts(f, "<entry id> <session id> <phy queue id>\n");
}

void sq_enable_set(char *cmd_buf, void *data)
{
	u32 sess_id;
	u16 queue;
	u8 ent_id;

	if (sscanf(cmd_buf, "%hhu %u %hu", &ent_id, &sess_id, &queue) != 3) {
		pr_err("sscanf err\n");
		return;
	}

	if (chk_sq_entry_enable(ent_id, sess_id, queue))
		pr_err("chk_sq_entry_enable failed\n");
}

PP_DEFINE_DEBUGFS(sq_enable, sq_enable_help, sq_enable_set);

void sq_disable_help(struct seq_file *f)
{
	seq_puts(f, "<entry id>\n");
}

void sq_disable_set(char *cmd_buf, void *data)
{
	u8 ent_id;

	if (sscanf(cmd_buf, "%hhu", &ent_id) != 1) {
		pr_err("sscanf err\n");
		return;
	}

	if (chk_sq_entry_disable(ent_id))
		pr_err("chk_sq_entry_disable failed\n");
}

PP_DEFINE_DEBUGFS(sq_disable, sq_disable_help, sq_disable_set);

void exception_map_set_help(struct seq_file *f)
{
	seq_puts(f, "<ingress port> <tc> <hoh> <exception session id>\n");
}

void exception_map_set(char *cmd_buf, void *data)
{
	u16 port;
	u8 tc, hoh, id;

	if (sscanf(cmd_buf, "%hu %hhu %hhu %hhu",
		   &port, &tc, &hoh, &id) != 4) {
		pr_err("sscanf err\n");
		return;
	}

	if (chk_exception_session_map_set(port, tc, hoh, id)) {
		pr_err("chk_exception_session_map_set failed\n");
		return;
	}
}

PP_DEFINE_DEBUGFS(map_set, exception_map_set_help, exception_map_set);

void exception_map_get_help(struct seq_file *f)
{
	seq_puts(f, "<ingress port> <tc> <hoh>\n");
}

void exception_map_get(char *cmd_buf, void *data)
{
	u16 port;
	u8 tc, hoh, id;

	if (sscanf(cmd_buf, "%hu %hhu %hhu",
		   &port, &tc, &hoh) != 3) {
		pr_err("sscanf err\n");
		return;
	}

	if (chk_exception_session_map_get(port, tc, hoh, &id)) {
		pr_err("chk_exception_session_map_get failed\n");
		return;
	}

	pr_info("Exception Id: %d\n", id);
}

PP_DEFINE_DEBUGFS(map_get, exception_map_get_help, exception_map_get);

void excep_port_map_set_help(struct seq_file *f)
{
	seq_puts(f, "<ingress port> #1 #2 #3...#16\n");
	seq_puts(f, "#1 - id for tc=0 hoh=0\n");
	seq_puts(f, "#2 - id for tc=0 hoh=1\n");
	seq_puts(f, "#3 - id for tc=0 hoh=2\n");
	seq_puts(f, "#4 - id for tc=0 hoh=3\n");
	seq_puts(f, "#5 - id for tc=1 hoh=0\n");
	seq_puts(f, "...\n");
	seq_puts(f, "#16 - id for tc=3 hoh=3\n");
}

void excep_port_map_set(char *cmd_buf, void *data)
{
	u16 port;
	u8 id[PP_MAX_TC][CHK_HOH_MAX];

	if (sscanf(cmd_buf, "%hu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu %hhu",
		   &port, &id[0][0], &id[0][1], &id[0][2], &id[0][3],
		   &id[1][0], &id[1][1], &id[1][2], &id[1][3],
		   &id[2][0], &id[2][1], &id[2][2], &id[2][3],
		   &id[3][0], &id[3][1], &id[3][2], &id[3][3]) != 17) {
		pr_err("sscanf err\n");
		return;
	}

	if (chk_exception_session_port_map_set(port, id)) {
		pr_err("chk_exception_session_port_map_set failed\n");
		return;
	}
}

PP_DEFINE_DEBUGFS(port_map_set, excep_port_map_set_help, excep_port_map_set);

void excep_port_map_get_help(struct seq_file *f)
{
	seq_puts(f, "<ingress port>\n");
}

void excep_port_map_get(char *cmd_buf, void *data)
{
	u16 port;
	u8 id[PP_MAX_TC][CHK_HOH_MAX];

	if (sscanf(cmd_buf, "%hu", &port) != 1) {
		pr_err("sscanf err\n");
		return;
	}

	if (chk_exception_session_port_map_get(port, id)) {
		pr_err("chk_exception_session_port_map_get failed\n");
		return;
	}

	pr_info("Exception Id's:\n");
	pr_info("tc 0 (hoh 0-3): %d %d %d %d\n",
		id[0][0], id[0][1], id[0][2], id[0][3]);
	pr_info("tc 1 (hoh 0-3): %d %d %d %d\n",
		id[1][0], id[1][1], id[1][2], id[1][3]);
	pr_info("tc 2 (hoh 0-3): %d %d %d %d\n",
		id[2][0], id[2][1], id[2][2], id[2][3]);
	pr_info("tc 3 (hoh 0-3): %d %d %d %d\n",
		id[3][0], id[3][1], id[3][2], id[3][3]);
}

PP_DEFINE_DEBUGFS(port_map_get, excep_port_map_get_help, excep_port_map_get);

void exception_set_help(struct seq_file *f)
{
	seq_puts(f, "<id> <port> <queue> <color> <flags_bitmap>\n");
}

void exception_set(char *cmd_buf, void *data)
{
	struct exception_session_cfg chk_cfg;
	u16  sgc[PP_SI_SGC_MAX];
	u16  tbm[PP_SI_TBM_MAX];
	u32  idx;

	if (sscanf(cmd_buf, "%hhu %hu %hu %hhu %hx",
		   &chk_cfg.id,
		   &chk_cfg.port,
		   &chk_cfg.queue,
		   &chk_cfg.color,
		   &chk_cfg.flags) != 5) {
		pr_err("sscanf err\n");
		return;
	}

	for (idx = 0 ; idx < PP_SI_SGC_MAX ; idx++)
		sgc[idx] = PP_SGC_INVALID;

	for (idx = 0 ; idx < PP_SI_TBM_MAX ; idx++)
		tbm[idx] = PP_TBM_INVALID;

	chk_cfg.sgc   = sgc;
	chk_cfg.tbm   = tbm;

	if (chk_exception_session_set(&chk_cfg)) {
		pr_err("chk_exception_session_set failed\n");
		return;
	}
}

PP_DEFINE_DEBUGFS(excep_set, exception_set_help, exception_set);

void exception_tbm_set(char *cmd_buf, void *data)
{
	struct exception_session_cfg chk_cfg;
	u16  org_tbm[PP_SI_TBM_MAX];
	u16  new_tbm[PP_SI_TBM_MAX];
	u16  sgc[PP_SI_SGC_MAX];

	if (sscanf(cmd_buf, "%hhu %hu %hu %hu %hu %hu",
		   &chk_cfg.id,
		   &new_tbm[0],
		   &new_tbm[1],
		   &new_tbm[2],
		   &new_tbm[3],
		   &new_tbm[4]) != 6) {
		pr_err("sscanf err\n");
		return;
	}

	chk_cfg.tbm = org_tbm;
	chk_cfg.sgc = sgc;

	if (chk_exception_session_get(&chk_cfg)) {
		pr_err("chk_exception_session_get failed for id %u\n",
			chk_cfg.id);
		return;
	}

	chk_cfg.tbm = new_tbm;

	if (chk_exception_session_set(&chk_cfg)) {
		pr_err("chk_exception_session_set failed\n");
		return;
	}
}

void exception_tbm_set_help(struct seq_file *f)
{
	seq_puts(f, "<id> <tbm0> <tbm1> <tbm2> <tbm3> <tbm4>\n");
	seq_puts(f, "PP_TBM_INVALID = 65535 \n");
	seq_puts(f, "Example: To set profiles 1 and 2 for exception 62 run the following -\n");
	seq_puts(f, "echo 62 1 2 65535 65535 65535 > exception_tbm_set\n");
}

PP_DEFINE_DEBUGFS(excep_tbm_set, exception_tbm_set_help, exception_tbm_set);

/**
 * @brief print exception configuration
 *
 * @param data unused
 * @param val value from user (exception id)
 *
 * @return int 0 on success, non-zero value otherwise
 */
static int exception_get(void *data, u64 val)
{
	struct exception_session_cfg chk_cfg;
	u16 sgc[PP_SI_SGC_MAX];
	u16 tbm[PP_SI_TBM_MAX];
	u32 idx;

	for (idx = 0 ; idx < PP_SI_SGC_MAX ; idx++)
		sgc[idx] = PP_SGC_INVALID;

	for (idx = 0 ; idx < PP_SI_TBM_MAX ; idx++)
		tbm[idx] = PP_TBM_INVALID;

	chk_cfg.sgc = sgc;
	chk_cfg.tbm = tbm;
	chk_cfg.id = (u8)val;

	if (chk_exception_session_get(&chk_cfg)) {
		pr_err("chk_exception_session_get failed\n");
		return 0;
	}

	pr_info("Exception %d Cfg:\n", chk_cfg.id);
	pr_info("* Eg Port:  %d\n", chk_cfg.port);
	pr_info("* Queue:    %d\n", chk_cfg.queue);
	pr_info("* Color:    %s\n",
		chk_cfg.color == 0 ? "N/A" :
		chk_cfg.color == 1 ? "Green" :
		chk_cfg.color == 2 ? "Yellow" :
		chk_cfg.color == 3 ? "Red" : "N/A");
	pr_info("* SGC:      %4x %4x %4x %4x\n",
		chk_cfg.sgc[0],
		chk_cfg.sgc[1],
		chk_cfg.sgc[2],
		chk_cfg.sgc[3]);
	pr_info("* TBM:      %4x %4x %4x %4x\n",
		chk_cfg.tbm[0],
		chk_cfg.tbm[1],
		chk_cfg.tbm[2],
		chk_cfg.tbm[3]);
	pr_info("checker flags %#x\n", chk_cfg.flags);

	return 0;
}

PP_DEFINE_DBGFS_ATT(exception_get, NULL, exception_get);

/**
 * @brief print exception configuration
 *
 * @param data unused
 * @param val value from user (exception id)
 *
 * @return int 0 on success, non-zero value otherwise
 */
static int stats_get(void *data, u64 val)
{
	struct pp_stats stats;
	u8 id;

	id = (u8)val;

	if (chk_exception_stats_get(id, &stats)) {
		pr_err("chk_exception_stats_get failed\n");
		return 0;
	}

	pr_info("Exception %d Stats:\n", id);
	pr_info("* Packets:  %llu\n", stats.packets);
	pr_info("* Bytes:    %llu\n", stats.bytes);

	return 0;
}

PP_DEFINE_DBGFS_ATT(stats, NULL, stats_get);

enum tbm_opts {
	tbm_opt_help = 1,
	tbm_opt_get,
	tbm_opt_set,
	tbm_opt_idx,
	tbm_opt_coupling,
	tbm_opt_enable,
	tbm_opt_credits,
	tbm_opt_timestamp,
	tbm_opt_max_burst,
	tbm_opt_exponent,
	tbm_opt_mantissa,
	tbm_opt_mode,
	tbm_opt_len_type,
};

enum tbm_action {
	TBM_ACT_GET,
	TBM_ACT_SET,
	TBM_NUM_ACTIONS
};

static const match_table_t tbm_tokens = {
	{tbm_opt_help,      "help"},
	{tbm_opt_get,       "get"},
	{tbm_opt_set,       "set"},
	{tbm_opt_idx,       "idx=%u"},
	{tbm_opt_coupling,  "cp=%u"},
	{tbm_opt_enable,    "en=%u"},
	{tbm_opt_credits,   "credits=%u"},
	{tbm_opt_timestamp, "ts=%x"},
	{tbm_opt_max_burst, "burst=%u"},
	{tbm_opt_exponent,  "exponent=%u"},
	{tbm_opt_mantissa,  "mantissa=%u"},
	{tbm_opt_mode,      "mode=%u"},
	{tbm_opt_len_type,  "type=%u"},
};

/**
 * @brief TBM file help
 */
static void __chk_dbg_tbm_help(void)
{
	u32 i;

	pr_info("\n");
	pr_info(" Brief: set/get TBMs configurations\n");
	pr_info(" Usage: echo <action> [option=value] ... > tbm\n");
	pr_info("    Print all enabled TBMs:\n");
	pr_info("        cat tbm\n");
	pr_info(" Actions:\n");
	pr_info("   help   - print this help\n");
	pr_info("   get    - print tbm configuration\n");
	pr_info("   set    - set tbm configuration\n");
	pr_info(" Options:\n");
	pr_info("   idx      - tbm index\n");
	pr_info("   en       - toggle enable/disable tbm\n");
	pr_info("   cp       - toggle coupling\n");
	pr_info("   credits  - credits\n");
	pr_info("   ts       - last timestamp\n");
	pr_info("   burst    - maximum number of credits, 1 credit = 16 bytes\n");
	pr_info("   exponent - exponent\n");
	pr_info("   mantissa - mantisa\n");
	pr_info("   mode     - color aware/blind mode\n");
	for (i = 0; i < PP_TBM_MODES_NUM; i++)
		pr_info("              %u - RFC %s\n", i, chk_tbm_mode_str(i));
	pr_info("   type   - packet length type to use\n");
	for (i = 0; i < PP_TBM_LEN_TYPES_NUM; i++)
		pr_info("              %u - %s length\n", i,
			chk_tbm_len_type_str(i));
	pr_info(" Examples:\n");
}

/**
 * @brief TBM get/set/alloc/free
 */
static void __chk_dbg_tbm(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum tbm_opts opt;
	char *tok;
	int val, ret = 0;
	u16 idx;
	enum tbm_action action;
	struct tbm_cfg cfg = { 0 };

	idx       = CHK_TBM_NUM;
	action    = TBM_NUM_ACTIONS;
	args      = strim(args);

	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, tbm_tokens, substr);
		switch (opt) {
		case tbm_opt_help:
			__chk_dbg_tbm_help();
			return;
		case tbm_opt_get:
			action = TBM_ACT_GET;
			break;
		case tbm_opt_set:
			action = TBM_ACT_SET;
			break;
		case tbm_opt_idx:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			idx = (u16)val;

			if (action == TBM_ACT_SET) {
				ret = chk_tbm_get(idx, &cfg);
				if (unlikely(ret)) {
					pr_err("failed to get tbm %u config\n",
					       idx);
					return;
				}
			}
			break;
		case tbm_opt_coupling:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.coupling = !!val;
			break;
		case tbm_opt_enable:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.enable = !!val;
			break;
		case tbm_opt_credits:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.credits = (u32)val;
			break;
		case tbm_opt_timestamp:
			if (match_hex(&substr[0], &val))
				goto opt_parse_err;
			cfg.timestamp = (u32)val;
			break;
		case tbm_opt_max_burst:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.max_burst = (u32)val;
			break;
		case tbm_opt_exponent:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.exponent = (u8)val;
			break;
		case tbm_opt_mantissa:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.mantissa = (u16)val;
			break;
		case tbm_opt_mode:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.mode = val;
			break;
		case tbm_opt_len_type:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.len_type = val;
			break;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			return;
		}
	}

	switch (action) {
	case TBM_ACT_SET:
		ret = chk_tbm_set(idx, &cfg);
		if (unlikely(ret))
			pr_info("failed to set tbm %u hw config\n", idx);
		else
			pr_info("tbm %u hw config set\n", idx);
		break;
	case TBM_ACT_GET:
		ret = chk_tbm_get(idx, &cfg);
		if (unlikely(ret)) {
			pr_info("failed to get tbm %u hw config\n", idx);
		} else {
			pr_info("tbm %03u: enable %s, coupled %s, credits %u, ts %#x, max_burst %u\n",
				idx, BOOL2STR(cfg.enable),
				BOOL2STR(cfg.coupling), cfg.credits,
				cfg.timestamp, cfg.max_burst);
			pr_info("         exponent %u, mantisa %u, mode %u, len_type %u\n",
				cfg.exponent, cfg.mantissa, cfg.mode,
				cfg.len_type);
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

static inline void __pr_tbm(struct seq_file *f, u32 idx, struct tbm_cfg *tbm)
{
	seq_printf(f, " | %-3u | %-6s", idx, BOOL2STR(tbm->enable));
	seq_printf(f, " | %-8s", BOOL2STR(tbm->coupling));
	seq_printf(f, " | %-10u", tbm->credits);
	seq_printf(f, " | %#-10x", tbm->timestamp);
	seq_printf(f, " | %-10u | %-8u", tbm->max_burst, tbm->exponent);
	seq_printf(f, " | %-7u", tbm->mantissa);
	seq_printf(f, " | %-16s", chk_tbm_mode_str(tbm->mode));
	seq_printf(f, " | %-17s", chk_tbm_len_type_str(tbm->len_type));
	seq_puts(f, " |\n");
}

void __chk_dbg_tbm_show_all(struct seq_file *f)
{
	struct dual_tbm_db_info dtbm = { 0 };
	struct tbm_cfg          ctbm = { 0 };
	struct tbm_cfg          ptbm = { 0 };
	s32 ret;
	u16 i;

	seq_puts(f, "\n");
	seq_puts(f, " +----------------------------------------------------------------------------------------------------------------------------+\n");
	seq_printf(f, " | %-3s | %-6s | %-8s | %-10s | %-10s | %-10s | %-8s | %-7s | %-16s | %-17s |\n",
		   "Idx", "Enable", "Coupling", "Credits", "Timestamp",
		   "Max Burst", "Exponent", "Mantisa", "Mode", "Len Type");
	seq_puts(f, " +----------------------------------------------------------------------------------------------------------------------------+\n");

	for (i = 0; i < CHK_DUAL_TBM_NUM; i++) {
		ret = chk_dual_tbm_db_info_get(i, &dtbm);
		if (unlikely(ret))
			return;

		ret = chk_tbm_get(dtbm.ctbm_idx, &ctbm);
		if (unlikely(ret))
			return;
		ret = chk_tbm_get(dtbm.ptbm_idx, &ptbm);
		if (unlikely(ret))
			return;

		/* print both committed and peak tbms if the
		 * committed is enable
		 */
		if (!ctbm.enable)
			continue;
		__pr_tbm(f, dtbm.ctbm_idx, &ctbm);
		__pr_tbm(f, dtbm.ptbm_idx, &ptbm);
		seq_puts(f, " +-----+--------+----------+------------+------------+------------+----------+---------+------------------+-------------------+\n");
	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(tbm, __chk_dbg_tbm_show_all, __chk_dbg_tbm);

enum crawler_opts {
	crawler_opt_help = 1,
	crawler_opt_timer_pace,
	crawler_opt_ddr_mode,
	crawler_opt_reset,
	crawler_opt_timer_init_val,
};

static const match_table_t crawler_tokens = {
	{crawler_opt_help,            "help"},
	{crawler_opt_timer_pace,      "timer_pace=%u"},
	{crawler_opt_ddr_mode,        "ddr_mode=%u"},
	{crawler_opt_reset,           "reset=%u"},
	{crawler_opt_timer_init_val,  "timer_val=%u"},
};

/**
 * @brief Crawler file help
 */
static void chk_dbg_crawler_help(void)
{
	pr_info("\n");
	pr_info(" Brief: set/get Crawler configurations\n");
	pr_info(" Usage: echo [option=value] ... > crawler\n");
	pr_info("        For Printing entire Crawler configuration :\n");
	pr_info("        cat crawler\n\n");
	pr_info(" Options:\n");
	pr_info("   timer_pace     - Mailbox Crawler Timer pace. Decrement value for Crawler internal Timer\n");
	pr_info("   ddr_mode       - 0 : No Limitation\n");
	pr_info("                    1 : Stale bitmap area Limited to 8K sessions and wraps around.\n");
	pr_info("   reset          - Mailbox Crawler Soft Reset\n");
	pr_info("   timer_val      - last timestamp\n");
	pr_info("   cmd_counter     (RO) - Command Counter\n");
	pr_info("   idle            (RO) - Crawler is Idle\n");
	pr_info("   wait            (RO) - Crawler in Wait State (Timer)\n");
	pr_info("   wait_for_final  (RO) - Crawler in Waiting for Final Stale State\n");
	pr_info("   sync_loss_error (RO) - Crawler DSI Sync Loss Error (sticky)\n");
}

/**
 * @brief Crawler configuration get/set
 */
static void chk_dbg_crawler(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum crawler_opts opt;
	char *tok;
	int val, ret = 0;
	struct crawler_cfg cfg = { 0 };

	args = strim(args);
	ret = chk_crawler_get(&cfg);
	if (unlikely(ret)) {
		pr_err("failed to get crawler config\n");
		return;
	}

	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, crawler_tokens, substr);
		switch (opt) {
		case crawler_opt_help:
			chk_dbg_crawler_help();
			return;
		case crawler_opt_timer_pace:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.timer_pace = (u16)val;
			break;
		case crawler_opt_ddr_mode:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.ddr_mode = !!val;
			break;
		case crawler_opt_reset:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.reset = !!val;
			break;
		case crawler_opt_timer_init_val:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			cfg.timer_init_val = (u32)val;
			break;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			return;
		}
	}

	ret = chk_crawler_set(&cfg);
	if (unlikely(ret))
		pr_info("failed to set crawler hw config\n");
	else
		pr_info("crawler hw config set\n");

	return;

opt_parse_err:
	pr_err("Failed to parse input\n");
}

void crawler_show_all(struct seq_file *f)
{
	struct crawler_cfg cfg;
	s32 ret;

	ret = chk_crawler_get(&cfg);
	if (unlikely(ret))
		return;

	seq_puts(f, "\n");
	seq_puts(f, " Crawler Configuration:\n");
	seq_puts(f, " ----------------------\n");
	seq_printf(f, " Idle           :  %s\n", cfg.idle_status ?
	"True" : "False");

	seq_printf(f, " Wait           :  %s\n", cfg.wait_status ?
	"True" : "False");

	seq_printf(f, " Wait For Final :  %s\n", cfg.wait_for_final_status ?
	"True" : "False");

	seq_printf(f, " DSI Sync Error :  %s\n", cfg.dsi_sync_loss_error ?
	"True" : "False");

	seq_printf(f, " Timer Init Val :  %u\n", cfg.timer_init_val);
	seq_printf(f, " Timer Pace     :  %u\n", cfg.timer_pace);
	seq_printf(f, " Cmd Counter    :  %u\n", cfg.command_counter);
	seq_printf(f, " DDR Mode       :  %s\n", cfg.ddr_mode ?
	"True" : "False");

	seq_printf(f, " Reset          :  %s\n", cfg.reset ? "True" : "False");
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(crawler, crawler_show_all, chk_dbg_crawler);

/**
 * @brief Set checker driver hw clock frequency
 */
static int __chk_dbg_clk_set(void *data, u64 val)
{
	return chk_clock_set((u32)val);
}

/**
 * @brief Get checker driver hw clock frequency
 */
static int __chk_dbg_clk_get(void *data, u64 *val)
{
	u32 clk = 0;
	s32 ret;

	ret  = chk_clock_get(&clk);
	*val = (u64)clk;
	return ret;
}

PP_DEFINE_DBGFS_ATT(clock, __chk_dbg_clk_get, __chk_dbg_clk_set);

/**
 * @brief Debugfs session cache stats show API
 */
static void chk_dbg_cntr_cache_stats_show(struct seq_file *f)
{
	u64 trans, hits;

	if (unlikely(chk_cntr_cache_stats_get(&trans, &hits))) {
		pr_err("failed to get session cache stats\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, "   Counters Cache Stats\n");
	seq_puts(f, " +----------------------+\n");
	seq_printf(f, "   Transactions: %llu\n", trans);
	seq_printf(f, "   Hits        : %llu\n", hits);
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(cntr_cache_stats_show, chk_dbg_cntr_cache_stats_show, NULL);

static s32 __chk_cntr_cache_bypass_get(void *data, u64 *val)
{
	bool en;

	en = chk_cntr_cache_bypass_get();
	*val = (u64)en;
	return 0;
}

static s32 __chk_cntr_cache_bypass_set(void *data, u64 val)
{
	chk_cntr_cache_bypass_set(!!val);
	return 0;
}

PP_DEFINE_DBGFS_ATT(cntr_cache_bypass, __chk_cntr_cache_bypass_get,
		    __chk_cntr_cache_bypass_set);

/**
 * @brief Debugfs stats show API, prints module statistics
 */
void __chk_exceptions_stats_show(struct seq_file *f)
{
	pp_stats_show_seq(f, sizeof(struct pp_stats),
			  CHK_NUM_EXCEPTION_SESSIONS, chk_exceptions_stats_get,
			  chk_exceptions_stats_show, NULL);
}

PP_DEFINE_DEBUGFS(exceptions_stats, __chk_exceptions_stats_show, NULL);

void __chk_exceptions_pps_show(struct seq_file *f)
{
	pp_pps_show_seq(f, sizeof(struct pp_stats), CHK_NUM_EXCEPTION_SESSIONS,
			chk_exceptions_stats_get, chk_exceptions_stats_diff,
			chk_exceptions_stats_show, NULL);
}

PP_DEFINE_DEBUGFS(exceptions_pps, __chk_exceptions_pps_show, NULL);

static struct debugfs_file chk_dbg_files[] = {
	{"crawler",                &PP_DEBUGFS_FOPS(crawler)},
	{"exceptions_pps",         &PP_DEBUGFS_FOPS(exceptions_pps)},
	{"exceptions_stats",       &PP_DEBUGFS_FOPS(exceptions_stats)},
	{"exception_tbm_set",      &PP_DEBUGFS_FOPS(excep_tbm_set)},
	{"exception_set",          &PP_DEBUGFS_FOPS(excep_set)},
	{"exception_get",          &PP_DEBUGFS_FOPS(exception_get)},
	{"exception_map_set",      &PP_DEBUGFS_FOPS(map_set)},
	{"exception_map_get",      &PP_DEBUGFS_FOPS(map_get)},
	{"excep_port_map_set", &PP_DEBUGFS_FOPS(port_map_set)},
	{"excep_port_map_get", &PP_DEBUGFS_FOPS(port_map_get)},
	{"stats",                  &PP_DEBUGFS_FOPS(stats)},
	{"syncq_enable",           &PP_DEBUGFS_FOPS(sq_enable)},
	{"syncq_disable",          &PP_DEBUGFS_FOPS(sq_disable)},
	{"tbm",                    &PP_DEBUGFS_FOPS(tbm)},
	{"clock",                  &PP_DEBUGFS_FOPS(clock)},
	{"cache_stats",            &PP_DEBUGFS_FOPS(cntr_cache_stats_show)},
	{"cache_bypass",           &PP_DEBUGFS_FOPS(cntr_cache_bypass)},
};

static struct debugfs_file drv_dbg_files[] = {
	{"exceptions_pps",    &PP_DEBUGFS_FOPS(exceptions_pps)},
	{"exceptions_stats",  &PP_DEBUGFS_FOPS(exceptions_stats)},
};

s32 chk_dbg_init(struct pp_chk_init_param *init_param)
{
	s32 ret;

	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	ret = pp_debugfs_create(init_param->dbgfs, "checker", &dbgfs,
				chk_dbg_files, ARRAY_SIZE(chk_dbg_files), NULL);
	if (unlikely(ret))
		return ret;

	return pp_debugfs_create(init_param->root_dbgfs, NULL, NULL,
				 drv_dbg_files, ARRAY_SIZE(drv_dbg_files),
				 NULL);
}

void chk_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;
}
