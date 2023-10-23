/*
 * infra_debugfs.c
 * Description: PP Infrastructure Driver Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/bitops.h>
#include <linux/debugfs.h>
#include <linux/pp_api.h>
#include <linux/parser.h>
#include "pp_logger.h"
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "infra.h"
#include "infra_internal.h"

/* Infra Debug FS directory */
static struct dentry *dbgfs;

/**
 * @brief Prints into seq_file PP boot and config relevant
 *        registers information
 * @param f seq_file
 * @note Used ONLY for debugfs prints
 */
static void __infra_bootcfg_show(struct seq_file *f)
{
	u32 version;
	bool sess_cache, cntrs_cache;
	bool rpb_done, port_dist_done;
	bool eg_uc_timers, ing_uc_timers;

	version        = infra_version_get();
	sess_cache     = infra_cls_sess_cache_en_get();
	cntrs_cache    = infra_cls_cntrs_cache_en_get();
	rpb_done       = infra_rpb_init_done_get();
	port_dist_done = infra_port_dist_init_done_get();
	eg_uc_timers   = infra_eg_uc_timers_clk_en_get();
	ing_uc_timers  = infra_ing_uc_timers_clk_en_get();

	seq_printf(f, "  %-25s: \t0x%x\n", "Boot config version", version);
	seq_printf(f, "  %-25s: \t%s\n", "Session cache", BOOL2EN(sess_cache));
	seq_printf(f, "  %-25s: \t%s\n", "Counters cache",
		   BOOL2EN(cntrs_cache));
	seq_printf(f, "  %-25s: \t%s\n", "Port Dist Memory Init",
		   port_dist_done ? "Done" : "No done");
	seq_printf(f, "  %-25s: \t%s\n", "RPB Memory Init",
		   rpb_done ? "Done" : "No done");
	seq_printf(f, "  %-25s: \t%s\n", "Egress UC Timers",
		   BOOL2EN(eg_uc_timers));
	seq_printf(f, "  %-25s: \t%s\n", "Ingress UC Timers",
		   BOOL2EN(ing_uc_timers));
}

/**
 * @brief Prints into buffer PP clock control relevant registers
 *        information
 * @param f seq_file
 * @param offset
 * @note Used ONLY for debugfs prints
 */
static void __infra_clock_ctrl_show(struct seq_file *f)
{
	u32 state, m;

	for_each_infra_cru_module(m) {
		infra_cru_mod_state_get(m, &state);
		seq_printf(f, "  %-13s CRU State  : \t%s(%u)\n",
			   infra_cru_module_str(m), infra_cru_state_str(state),
			   state);
	}
}

static void __infra_dynamic_clk_show(struct seq_file *f)
{
	u32 m, grace;
	bool en, on;

	infra_dclk_grace_get(&grace);
	seq_printf(f, "  %-25s: \t%u\n", "Grace", grace);

	for_each_infra_dclk_mod(m) {
		infra_mod_dclk_get(m, &en);
		infra_mod_dclk_status_get(m, &on);
		seq_printf(f, "  %-10s Dynamic Clock : \t%-7s, clock status: %s\n",
			   infra_dclk_module_str(m), BOOL2EN(en),
			   on ? "on" : "off");
	}
}

/**
 * @brief Debugfs status attribute show API, prints driver
 *        status including relevant registers information.
 * @param f seq_file
 * @param offset
 * @note Used ONLY for debugfs prints
 */
static void __infra_status_show(struct seq_file *f)
{
	seq_puts(f, "\n");
	seq_puts(f, " Infrastructure\n");
	seq_puts(f, " ===============================\n");
	seq_puts(f, "  Boot and Config Info\n");
	seq_puts(f, "  ------------------------------\n");
	__infra_bootcfg_show(f);
	seq_puts(f, "  ------------------------------\n");
	seq_puts(f, "  Clock Control Info\n");
	seq_puts(f, "  ------------------------------\n");
	__infra_clock_ctrl_show(f);
	seq_puts(f, "  ------------------------------\n");
	seq_puts(f, "  Dynamic Clock Info\n");
	seq_puts(f, "  ------------------------------\n");
	__infra_dynamic_clk_show(f);
	seq_puts(f, "  ------------------------------\n");
	seq_puts(f, "\n\n");
}

PP_DEFINE_DEBUGFS(status, __infra_status_show, NULL);

enum mod_cru_rst_opts {
	mod_cru_rst_opt_help = 1,
	mod_cru_rst_opt_mod,
};

static const match_table_t mod_cru_rst_tokens = {
	{mod_cru_rst_opt_help, "help"},
	{mod_cru_rst_opt_mod,  "mod=%u"},
};

/**
 * @brief Session delete help
 */
static void __infra_mod_cru_rst_help(void)
{
	u32 m;

	pr_info("\n");
	pr_info(" Usage: echo <option>[=value] > cru\n");
	pr_info(" Options:\n");
	pr_info("   help - print this help function\n");
	pr_info("   mod  - module's id\n");
	for_each_infra_cru_module(m)
		pr_info("          %u - %s\n", m, infra_cru_module_str(m));
	pr_info(" Examples:\n");
	pr_info("   reset module 3 cru:\n");
	pr_info("     echo mod=3 > mod_reset\n\n");
}

/**
 * @brief Set modules dynamic clock
 */
void __infra_mod_cru_reset(char *cmd_buf, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum mod_cru_rst_opts opt;
	char *tok, *args;
	int val, ret;
	enum cru_module m;

	m    = CRU_MOD_NUM;
	args = cmd_buf;
	args = strim(args);
	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, mod_cru_rst_tokens, substr);
		switch (opt) {
		case mod_cru_rst_opt_help:
			__infra_mod_cru_rst_help();
			return;
		case mod_cru_rst_opt_mod:
			if (match_int(&substr[0], &val))
				goto parse_err;
			m = (enum cru_module)val;
			break;
		default:
			goto parse_err;
		}
	}

	ret = infra_reset_hw(m);
	if (!ret)
		pr_info("%s CRU was reset\n", infra_cru_module_str(m));
	else
		pr_info("failed to reset %s CRU\n", infra_cru_module_str(m));
	return;

parse_err:
	pr_err("failed to parse %s\n", tok);
}

PP_DEFINE_DEBUGFS(reset_mod, NULL, __infra_mod_cru_reset);

enum mod_dclk_opts {
	mod_dclk_opt_help = 1,
	mod_dclk_opt_mod,
	mod_dclk_opt_en,
	mod_dclk_opt_grace
};

static const match_table_t mod_dclk_tokens = {
	{mod_dclk_opt_help,  "help"},
	{mod_dclk_opt_mod,   "mod=%u"},
	{mod_dclk_opt_en,    "en=%u"},
	{mod_dclk_opt_grace, "grace=%u"},
};

/**
 * @brief Session delete help
 */
static void __infra_mod_dclk_set_help(void)
{
	u32 m;

	pr_info("\n");
	pr_info(" Usage: echo <option>[=value] > dclock\n");
	pr_info(" Options:\n");
	pr_info("   help  - print this help function\n");
	pr_info("   en    - enable/disable\n");
	pr_info("   grace - grace cycles\n");
	pr_info("   mod   - module's id\n");
	for_each_infra_dclk_mod(m)
		pr_info("          %u - %s\n", m, infra_dclk_module_str(m));
	pr_info(" Examples:\n");
	pr_info("   enable module 5 dynamic clock:\n");
	pr_info("     echo mod=5 en=1 > dclock\n\n");
	pr_info("   change grace cycles:\n");
	pr_info("     echo grace=30 > dclock\n");
}

/**
 * @brief Set modules dynamic clock
 */
void __infra_mod_dclk_set(char *cmd_buf, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum mod_dclk_opts opt;
	char *tok, *args;
	int val, ret;
	enum dclk_module m;
	bool en;

	en    = false;
	m     = DCLK_MOD_NUM;
	args  = cmd_buf;
	args  = strim(args);
	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, mod_dclk_tokens, substr);
		switch (opt) {
		case mod_dclk_opt_help:
			__infra_mod_dclk_set_help();
			return;
		case mod_dclk_opt_mod:
			if (match_int(&substr[0], &val))
				goto parse_err;
			m = (enum dclk_module)val;
			break;
		case mod_dclk_opt_en:
			if (match_int(&substr[0], &val))
				goto parse_err;
			en = !!val;
			break;
		case mod_dclk_opt_grace:
			if (match_int(&substr[0], &val))
				goto parse_err;
			infra_dclk_grace_set((u32)val);
			return;
		default:
			goto parse_err;
		}
	}

	ret = infra_mod_dclk_set(m, en);
	if (!ret)
		pr_info("%s dynamic clock %s\n", infra_dclk_module_str(m),
			BOOL2EN(en));
	else
		pr_info("failed to %s %s dynamic clock\n", BOOL2EN(en),
			infra_dclk_module_str(m));
	return;

parse_err:
	pr_err("failed to parse %s\n", tok);
}

PP_DEFINE_DEBUGFS(mod_dclk, NULL, __infra_mod_dclk_set);

static struct debugfs_file debugfs_files[] = {
	{ "status",       &PP_DEBUGFS_FOPS(status) },
	{ "reset_module", &PP_DEBUGFS_FOPS(reset_mod) },
	{ "dclock",       &PP_DEBUGFS_FOPS(mod_dclk) }
};

s32 infra_dbg_init(struct dentry *parent)
{
	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	/* infra debugfs dir */
	return pp_debugfs_create(parent, "infra", &dbgfs, debugfs_files,
				 ARRAY_SIZE(debugfs_files), NULL);
}

s32 infra_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;

	return 0;
}
