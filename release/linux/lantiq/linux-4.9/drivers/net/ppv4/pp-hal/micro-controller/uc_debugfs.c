/*
 * Description: PP micro-controllers Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2019-2020 Intel Corporation
 */

#define pr_fmt(fmt) "[PP_UC_DBG]:%s:%d: " fmt, __func__, __LINE__

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/string.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include <linux/dma-mapping.h>
#include <linux/parser.h>
#include <linux/pp_api.h>

#include "pp_common.h"
#include "pp_regs.h"
#include "pp_debugfs_common.h"
#include "infra.h"
#include "uc.h"
#include "uc_regs.h"
#include "uc_internal.h"

/**
 * @brief main uc debugfs dir
 */
static struct dentry *dbgfs;

static u32 gcid, gtid;

static void uc_dbg_nf_cfg_set(char *cmd_buf, void *data)
{
	u16 pid, qos_port, tx_queue, host_q;
	u32 nf;

	if (unlikely(sscanf(cmd_buf, "%u %hu %hu %hu %hu", &nf, &pid, &qos_port,
			    &tx_queue, &host_q) != 5)) {
		pr_err("sscanf error\n");
		return;
	}

	if (unlikely(uc_nf_set(nf, pid, qos_port, tx_queue, host_q))) {
		pr_err("failed to set nf\n");
		return;
	}

	pr_info("UC_TYPE[EGRESS] NF[%u] PID[%hu] QOS_RX_PORT[%hu] QOS_TX_QUEUE[%hu]\n",
		nf, pid, qos_port, tx_queue);
}

static void uc_dbg_nf_cfg_help(struct seq_file *f)
{
	seq_puts(f, "<nf 0=REASS, 1=TDOX, 2=FRAG, 3=MCAST, 4=IPSEC> <pid> <rx_qos_phy_port> <tx_qos_phy_queue>\n");
}

PP_DEFINE_DEBUGFS(nf_cfg, uc_dbg_nf_cfg_help, uc_dbg_nf_cfg_set);

static void uc_dbg_core_run_halt_set(char *cmd_buf, void *data)
{
	u8 uc_type, cpu_id, enable;

	if (unlikely(sscanf(cmd_buf, "%hhu %hhu %hhu",
			    &uc_type, &cpu_id, &enable) != 3)) {
		pr_err("sscanf error\n");
		return;
	}

	if (unlikely(uc_type > 1)) {
		pr_err("invalid uc cluster %hhu\n", uc_type);
		return;
	}

	uc_run_set(!!uc_type, (u32)cpu_id, !!enable);

	pr_info("UC_TYPE[%s] CPU_ID[%hhu] ACTION[%s]\n",
		uc_type ? "EGRESS" : "INGRESS", cpu_id,
		enable ?  "RUN"    : "HALT");
}

static void uc_dbg_core_run_halt_show(struct seq_file *f)
{
	seq_puts(f, "<uc cluster 0=ING, 1=EGR> <cpu id> <action 0=halt, 1=run>\n");
}

PP_DEFINE_DEBUGFS(core_run_halt, uc_dbg_core_run_halt_show,
		  uc_dbg_core_run_halt_set);

static void uc_dbg_uc_enable_set(char *cmd_buf, void *data)
{
	u8 uc_type, cpu_id, enable;

	if (unlikely(sscanf(cmd_buf, "%hhu %hhu %hhu",
			    &uc_type, &cpu_id, &enable) != 3)) {
		pr_err("Command format error\n");
		pr_err("<uc cluster 0=ING, 1=EGR> <cpu id> <action 0=disable, 1=enable>\n");
		return;
	}

	if (unlikely(uc_type > 1)) {
		pr_err("invalid uc cluster %hhu\n", uc_type);
		return;
	}

	uc_ccu_enable_set(!!uc_type, (u32)cpu_id, !!enable);

	pr_info("UC_TYPE[%s] CPU_ID[%hhu] ACTION[%s]\n",
		uc_type ? "EGRESS" : "INGRESS", cpu_id,
		BOOL2EN(enable));
}

static void uc_dbg_uc_enable_show(struct seq_file *f)
{
	u32 i;
	bool act;
	u8 hw_max_cpu, uc_types = 2;

	if (unlikely(!uc_is_cluster_valid(UC_IS_EGR) &&
		     !uc_is_cluster_valid(UC_IS_ING))) {
		seq_puts(f, "all uc cpus are disabled\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +---------------------------------+\n");
	seq_puts(f, " |          UC CPU STATUS          |\n");
	seq_puts(f, " +---------------------------------+\n");
	seq_puts(f, " |  CLUSTER  |  CPU ID  |  STATUS  |\n");
	seq_puts(f, " +-----------+----------+----------+\n");
	while (uc_types--) {
		if (unlikely(!uc_is_cluster_valid(!!uc_types)))
			continue;

		if (uc_ccu_maxcpus_get(!!uc_types, &hw_max_cpu)) {
			seq_printf(f, "failed to get the %s max cpus\n",
				   !!uc_types ? "egress" : "ingress");
			return;
		}

		for (i = 0; i < hw_max_cpu; i++) {
			act = uc_is_cpu_active(!!uc_types, i);
			seq_printf(f, " | %-9s |  %-6u  | %-8s |\n",
				   !!uc_types ? "EGRESS" : "INGRESS", i,
				   act ? "ACTIVE" : "INACTIVE");
			seq_puts(f, " +-----------+----------+----------+\n");
		}
	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(uc_enable, uc_dbg_uc_enable_show, uc_dbg_uc_enable_set);

static void uc_dbg_fat_set(char *cmd_buf, void *data)
{
	u8 uc_type;
	u32 ent, val, en;
	s32 ret;

	if (unlikely(sscanf(cmd_buf, "%hhu %x %x %u",
			    &uc_type, &ent, &val, &en) != 4)) {
		pr_err("Command format error\n");
		pr_err("<uc cluster 0=ING, 1=EGR> <fat entry> <val> <action 0=disable, 1=enable>\n");
		return;
	}

	if (unlikely(uc_type > 1)) {
		pr_err("invalid uc cluster %hhu\n", uc_type);
		return;
	}

	ret = uc_fat_set(!!uc_type, ent, val, !!en);
	if (unlikely(ret)) {
		pr_err("failed to set the cluster bridge fat\n");
		return;
	}

	pr_info("UC_TYPE[%s] FAT_ENT[%#x] VAL[%#x] ACTION[%s]\n",
		uc_type ? "EGRESS" : "INGRESS", ent, val,
		BOOL2EN(en));
}

static void uc_dbg_fat_show(struct seq_file *f)
{
	u32 i, max_ent, val;
	bool enable;

	if (pp_silicon_step_get() == PP_SSTEP_A)
		max_ent = UC_A_FAT_MAX_ENT;
	else
		max_ent = UC_B_FAT_ALL_MAX_ENT;

	if (uc_is_cluster_valid(UC_IS_EGR)) {
		seq_puts(f, " +-------------------------------+\n");
		seq_puts(f, " | EGRESS CLUSTER BRIDGE FAT     |\n");
		seq_puts(f, " +-------------------------------+\n");
		seq_puts(f, " | ENTRY | IS_ENABLE |   VALUE   |\n");
		seq_puts(f, " +-------+-----------+-----------+\n");

		for (i = 0; i < max_ent; i++) {
			if (uc_fat_get(UC_IS_EGR, i, &val, &enable)) {
				seq_printf(f, "failed to get fat ent %u\n", i);
				break;
			}
			seq_printf(f, " | 0x%-2X  | %-9s | 0x%-8X|\n",
				   i, BOOL2EN(enable), val);
			seq_puts(f, " +-------+-----------+-----------+\n");
		}
	}
	seq_puts(f, "\n");
	if (uc_is_cluster_valid(UC_IS_ING)) {
		seq_puts(f, " +-------------------------------+\n");
		seq_puts(f, " | INGRESS CLUSTER BRIDGE FAT    |\n");
		seq_puts(f, " +-------------------------------+\n");
		seq_puts(f, " | ENTRY | IS_ENABLE |   VALUE   |\n");
		seq_puts(f, " +-------+-----------+-----------+\n");

		for (i = 0; i < max_ent; i++) {
			if (uc_fat_get(UC_IS_ING, i, &val, &enable)) {
				seq_printf(f, "failed to get fat ent %u\n", i);
				break;
			}
			seq_printf(f, " | 0x%-2X  | %-9s | 0x%-8X|\n",
				   i, BOOL2EN(enable), val);
			seq_puts(f, " +-------+-----------+-----------+\n");
		}
	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(fat, uc_dbg_fat_show, uc_dbg_fat_set);

static void uc_dbg_ccu_gpreg_set(char *cmd_buf, void *data)
{
	u8 uc_type;
	u32 gpreg_id, val;

	if (unlikely(sscanf(cmd_buf, "%hhu %u %u",
			    &uc_type, &gpreg_id, &val) != 3)) {
		pr_err("Command format error\n");
		pr_err("<uc cluster 0=ING, 1=EGR> <gpreg id> <val>\n");
		return;
	}

	if (unlikely(uc_type > 1)) {
		pr_err("invalid uc cluster %hhu\n", uc_type);
		return;
	}

	if (uc_ccu_gpreg_set(!!uc_type, gpreg_id, val)) {
		pr_err("failed to set gpreg %u\n", gpreg_id);
		return;
	}

	pr_info("UC_TYPE[%s] GPREG_ID[%u] VALUE[%u]\n",
		uc_type ? "EGRESS" : "INGRESS", gpreg_id, val);
}

static void uc_dbg_ccu_gpreg_show(struct seq_file *f)
{
	u32 i, val;
	u8 uc_types = 2;

	if (unlikely(!uc_is_cluster_valid(UC_IS_EGR) &&
		     !uc_is_cluster_valid(UC_IS_ING))) {
		seq_puts(f, "uc clusters are disabled\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +---------------------------------+\n");
	seq_puts(f, " |          UC CCU GPREG           |\n");
	seq_puts(f, " +---------------------------------+\n");
	seq_puts(f, " |  CLUSTER  | GPREG ID |  VALUE   |\n");
	seq_puts(f, " +-----------+----------+----------+\n");
	while (uc_types--) {
		if (unlikely(!uc_is_cluster_valid(!!uc_types)))
			continue;

		for (i = 0; i < UC_CCU_GPREG_MAX; i++) {
			if (unlikely(uc_ccu_gpreg_get(!!uc_types, i, &val)))
				return;
			seq_printf(f, " | %-9s |  %-6u  |0x%08X|\n",
				   !!uc_types ? "EGRESS" : "INGRESS", i, val);
			seq_puts(f, " +-----------+----------+----------+\n");
		}
	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(ccu_gpreg, uc_dbg_ccu_gpreg_show, uc_dbg_ccu_gpreg_set);

static void uc_dbg_ccu_irr_show(struct seq_file *f)
{
	u32 irr;
	u8 uc_types = 2;

	if (unlikely(!uc_is_cluster_valid(UC_IS_EGR) &&
		     !uc_is_cluster_valid(UC_IS_ING))) {
		seq_puts(f, "uc clusters are disabled\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +---------------------------------+\n");
	seq_puts(f, " | UC CCU INTERRUPT REQUEST STATUS |\n");
	seq_puts(f, " +---------------------------------+\n");
	seq_puts(f, " |  CLUSTER  | CCU IRR REG VAL     |\n");
	seq_puts(f, " +-----------+---------------------+\n");
	while (uc_types--) {
		if (unlikely(!uc_is_cluster_valid(!!uc_types)))
			continue;

		if (uc_ccu_irr_get(!!uc_types, &irr)) {
			seq_printf(f, "failed to get the %s irr value\n",
				   !!uc_types ? "egress" : "ingress");
			return;
		}
		seq_printf(f, " | %-9s |     0x%08X      |\n",
			   !!uc_types ? "EGRESS" : "INGRESS", irr);
		seq_puts(f, " +-----------+---------------------+\n");
	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(ccu_irr, uc_dbg_ccu_irr_show, NULL);

static void uc_dbg_reader_irr_show(struct seq_file *f)
{
	u32 irr;
	u8 uc_types = 2;

	if (unlikely(!uc_is_cluster_valid(UC_IS_EGR) &&
		     !uc_is_cluster_valid(UC_IS_ING))) {
		seq_puts(f, "uc clusters are disabled\n");
		return;
	}

	seq_puts(f, "\n");
	seq_puts(f, " +------------------------------------+\n");
	seq_puts(f, " | UC READER INTERRUPT REQUEST STATUS |\n");
	seq_puts(f, " +------------------------------------+\n");
	seq_puts(f, " |  CLUSTER  | READER IRR REG VAL     |\n");
	seq_puts(f, " +-----------+------------------------+\n");
	while (uc_types--) {
		if (unlikely(!uc_is_cluster_valid(!!uc_types)))
			continue;

		if (uc_reader_irr_get(!!uc_types, &irr)) {
			seq_printf(f, "failed to get the %s irr value\n",
				   !!uc_types ? "egress" : "ingress");
			return;
		}
		seq_printf(f, " | %-9s |     0x%08X         |\n",
			   !!uc_types ? "EGRESS" : "INGRESS", irr);
		seq_puts(f, " +-----------+------------------------+\n");
	}
	seq_puts(f, "\n");
}

PP_DEFINE_DEBUGFS(reader_irr, uc_dbg_reader_irr_show, NULL);

static void uc_dbg_version_show(struct seq_file *f)
{
	u32 ver;

	seq_puts(f, "\n");
	if (likely(!uc_ver_get(UC_IS_EGR, &ver))) {
		seq_printf(f, "EGRESS  : MAJOR[%u], MINOR[%u], BUILD[%u]\n",
			   (ver >> 16) & U8_MAX, (ver >> 8) & U8_MAX,
			   ver & U8_MAX);
	}

	if (likely(!uc_ver_get(UC_IS_ING, &ver))) {
		seq_printf(f, "INGRESS : MAJOR[%u], MINOR[%u], BUILD[%u]\n",
			   (ver >> 16) & U8_MAX, (ver >> 8) & U8_MAX,
			   ver & U8_MAX);
	}
}

PP_DEFINE_DEBUGFS(version, uc_dbg_version_show, NULL);

enum uc_log_opts {
	uc_log_opt_help = 1,
	uc_log_opt_reset,
	uc_log_opt_level,
};

static const match_table_t uc_log_tokens = {
	{ uc_log_opt_help,  "help" },
	{ uc_log_opt_reset, "reset" },
	{ uc_log_opt_level, "level=%u" },
};

static void __uc_log_help(void)
{
	pr_info("\n");
	pr_info(" Brief: Print/configure UC logger\n");
	pr_info(" Usage: echo <option>[=value] ... > log\n");
	pr_info(" Options:\n");
	pr_info("   help   - print this help\n");
	pr_info("   reset  - delete all logs\n");
	pr_info("   level  - set log level\n");
	pr_info(" Examples:\n");
	pr_info("   print log: cat log\n");
	pr_info("   reset log: echo reset > log\n");
}

static void __uc_log_set(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum uc_log_opts opt;
	char *tok;
	s32 val;

	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, uc_log_tokens, substr);
		switch (opt) {
		case uc_log_opt_help:
			__uc_log_help();
			break;
		case uc_log_opt_reset:
			uc_log_reset();
			break;
		case uc_log_opt_level:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			uc_log_level_set((enum uc_log_level)val);
			break;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			break;
		}
	}

	return;

opt_parse_err:
	pr_err("Failed to parse %s value\n", tok);
}

static void uc_dbg_log_show(struct seq_file *f)
{
	static const char *const level_str[] = { "FATAL", "ERROR", "WARN",
						 "INFO", "DEBUG" };
	struct uc_log_msg *msgs, *msg;
	void *buff;
	size_t sz;

	uc_log_buff_info_get(&buff, &sz);

	msgs = kzalloc(sz, GFP_KERNEL);
	if (unlikely(!msgs)) {
		pr_err("failed to allocate %zu bytes buffer\n", sz);
		return;
	}

	memcpy(msgs, buff, sz);
	seq_puts(f, "  UC Log\n");
	seq_puts(f, "==========\n");
	for_each_arr_entry(msg, msgs, sz / sizeof(*msg)) {
		if (strlen(msg->str) == 0)
			continue;
		seq_printf(f, "[%8x]:[%-5s]:[%u.%02u][%s]: %s %u, %#x\n",
			   msg->ts, level_str[msg->level], msg->cid, msg->tid,
			   msg->func, msg->str, msg->val, msg->val);
	}
	seq_puts(f, "\n");

	kfree(msgs);
}

PP_DEFINE_DEBUGFS(uc_log, uc_dbg_log_show, __uc_log_set);

enum uc_aux_reg_opts {
	uc_aux_reg_help = 1,
	uc_aux_reg_rd,
	uc_aux_reg_wr,
	uc_aux_reg_id,
	uc_aux_reg_val,
	uc_aux_reg_max
};

static const match_table_t uc_aux_reg_tokens = {
	{ uc_aux_reg_help, "help" },
	{ uc_aux_reg_rd,   "rd" },
	{ uc_aux_reg_wr,   "wr" },
	{ uc_aux_reg_id,   "id=%u" },
	{ uc_aux_reg_val,  "val=%u" },
};

static void __uc_aux_reg_help(void)
{
	pr_info("\n");
	pr_info(" Brief: Read/Write egress UC auxiliary registers\n");
	pr_info("        for full list of auxiliary registers see Figure 3-5\n");
	pr_info("        in ARCv2 Programmer's Reference Manual\n");
	pr_info(" Usage: echo <action> [option][=value] ... > eg_aux_reg\n");
	pr_info(" actions:\n");
	pr_info("   help   - print this help\n");
	pr_info("   rd     - read register\n");
	pr_info("   write  - write register\n");
	pr_info(" options:\n");
	pr_info("   id     - register id\n");
	pr_info("   value  - value to write\n");
	pr_info(" Examples:\n");
	pr_info("   read: echo read id=0x11 > aux_reg\n");
	pr_info("   reset echo write id=0xc5 0x50 > aux_reg\n");
}

static void __uc_aux_reg(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum uc_aux_reg_opts opt;
	enum uc_aux_reg_opts act = uc_aux_reg_max;
	char *tok;
	s32 tmp, ret;
	u32 reg = 0;
	u32 val;
	bool val_valid = false;

	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, uc_aux_reg_tokens, substr);
		switch (opt) {
		case uc_aux_reg_help:
		case uc_aux_reg_rd:
		case uc_aux_reg_wr:
			act = opt;
			break;
		case uc_aux_reg_id:
			if (match_int(&substr[0], &tmp))
				goto opt_parse_err;
			reg = tmp;
			break;
		case uc_aux_reg_val:
			if (match_int(&substr[0], &tmp))
				goto opt_parse_err;
			val = tmp;
			val_valid = true;
			break;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			break;
		}
	}

	switch (act) {
	case uc_aux_reg_help:
		__uc_aux_reg_help();
		break;
	case uc_aux_reg_rd:
		ret = uc_aux_reg_read(reg, &val);
		if (ret)
			pr_info("Failed to read aux reg %#x\n", reg);
		else
			pr_info("Auxiliary register %#x ==> %#x\n", reg,
				val);
		break;
	case uc_aux_reg_wr:
		if (!val_valid) {
			pr_info("value wasn't specified\n");
			break;
		}
		ret = uc_aux_reg_write(reg, val);
		if (ret)
			pr_info("Failed to write %#x to aux reg %#x\n", val,
				reg);
		else
			pr_info("Auxiliary register %#x <== %#x\n", reg,
				val);
		break;
	default:
		break;
	}

	return;

opt_parse_err:
	pr_err("Failed to parse %s value\n", tok);
}

PP_DEFINE_DEBUGFS(uc_aux_reg, NULL, __uc_aux_reg);

static void uc_tasks_show(bool uc_is_egr, struct seq_file *f)
{
	u8 cid, tid, num_cpu, num_tasks;
	u32 pc, status, queue;
	bool ready, active;

	uc_ccu_maxcpus_get(uc_is_egr, &num_cpu);
	num_tasks = uc_is_egr ? UC_EGR_MAXTASK : UC_ING_MAXTASK;

	seq_puts(f, " +---------------------------------------------------------+\n");
	seq_puts(f, " | Task ID | PC         | STATUS32   | Ready | POPA Active |\n");
	seq_puts(f, " +---------+------------+------------+-------+-------------+\n");
	for (cid = 0; cid < num_cpu; cid++) {
		for (tid = 0; tid < num_tasks; tid++) {
			uc_task_pc_get(uc_is_egr, cid, tid, &pc);
			uc_task_status32_get(uc_is_egr, cid, tid, &status);
			uc_task_ready_get(uc_is_egr, cid, tid, &ready);
			uc_task_popa_active_get(uc_is_egr, cid, tid, &active,
						&queue);

			/** 0xe4000098 is a pc value for tasks which aren't
			 *  any tasks, it may change in the future
			 */
			if (pc == 0xe4000098)
				continue;

			seq_puts(f, " |");
			seq_printf(f, " CPU%u.%02u |", cid, tid);
			seq_printf(f, " %#010x |", pc);
			seq_printf(f, " %#010x |", status);
			seq_printf(f, " %-5s |", BOOL2STR(ready));
			if (active)
				seq_printf(f, " %-8s Q%u |\n", BOOL2STR(active),
					   queue);
			else
				seq_printf(f, " %-11s |\n", BOOL2STR(active));
		}
		seq_puts(f, " +---------+------------+------------+-------+-------------+\n");
	}
	seq_puts(f, " +---------------------------------------------------------+\n");
}

static void uc_egr_tasks_show(struct seq_file *f)
{
	uc_tasks_show(true, f);
}
PP_DEFINE_DEBUGFS(egr_tasks, uc_egr_tasks_show, NULL);

static void uc_ing_tasks_show(struct seq_file *f)
{
	uc_tasks_show(false, f);
}
PP_DEFINE_DEBUGFS(ing_tasks, uc_ing_tasks_show, NULL);

enum uc_task_set_opts {
	uc_task_set_tok_help = 1,
	uc_task_set_tok_cid,
	uc_task_set_tok_tid,
};

static const match_table_t uc_task_set_tokens = {
	{ uc_task_set_tok_help, "help" },
	{ uc_task_set_tok_cid,  "cid=%u" },
	{ uc_task_set_tok_tid,  "tid=%u" },
};

static void uc_task_set_help(void)
{
	pr_info("\n");
	pr_info(" Usage: echo cid=<cpu id> tid=<task id> > egr_task\n");
	pr_info(" Usage: echo cid=<cpu id> tid=<task id> > ing_task\n");
}

static void uc_task_set(char *args, void *data)
{
	substring_t substr[MAX_OPT_ARGS];
	enum uc_task_set_opts opt;
	char *tok;
	s32 tmp;

	/* iterate over user arguments */
	while ((tok = strsep(&args, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, uc_task_set_tokens, substr);
		switch (opt) {
		case uc_task_set_tok_help:
			uc_task_set_help();
			return;
		case uc_task_set_tok_cid:
			if (match_int(&substr[0], &tmp))
				goto opt_parse_err;
			gcid = (u32)tmp;
			break;
		case uc_task_set_tok_tid:
			if (match_int(&substr[0], &tmp))
				goto opt_parse_err;
			gtid = (u32)tmp;
			break;
		default:
			pr_info("Error: Invalid option '%s'\n", tok);
			break;
		}
	}

	return;

opt_parse_err:
	pr_err("Failed to parse %s value\n", tok);
}

struct status32 {
	u32 halt:1,
	    irq_prio:4,
	    exception:1,
	    delay_branch:1,
	    user_mode:1,
	    overflow:1,
	    carry:1,
	    negative:1,
	    zero:1,
	    zero_oh:1,
	    zero_div_exp_en:1,
	    stack_check:1,
	    table_pend:1,
	    reg_bank:3,
	    align_check:1,
	    user_sleep_en:1,
	    secure_mode:1,
	    rsrv:9,
	    irq_en:1;
};

static void uc_task_show(bool uc_is_egr, struct seq_file *f)
{
	struct status32 status;
	u32 pc, queue, regs[32], i;
	bool ready, active;
	s32 ret = 0;

	seq_puts(f, " +---------------------------------------------------------+\n");
	seq_puts(f, " | Task ID | PC         | STATUS32   | Ready | POPA Active |\n");
	seq_puts(f, " +---------+------------+------------+-------+-------------+\n");
	ret |= uc_task_pc_get(uc_is_egr, gcid, gtid, &pc);
	ret |= uc_task_status32_get(uc_is_egr, gcid, gtid, (u32 *)&status);
	ret |= uc_task_ready_get(uc_is_egr, gcid, gtid, &ready);
	ret |= uc_task_popa_active_get(uc_is_egr, gcid, gtid, &active, &queue);
	ret |= uc_task_regs_get(uc_is_egr, gcid, gtid, regs, ARRAY_SIZE(regs));

	if (ret)
		return;

	seq_puts(f, " |");
	seq_printf(f, " CPU%u.%02u |", gcid, gtid);
	seq_printf(f, " %#010x |", pc);
	seq_printf(f, " %#010x |", *((u32 *)&status));
	seq_printf(f, " %-5s |", BOOL2STR(ready));
	if (active)
		seq_printf(f, " %-8s Q%u |\n", BOOL2STR(active),
				queue);
	else
		seq_printf(f, " %-11s |\n", BOOL2STR(active));

	seq_puts(f, " +---------+------------+------------+-------+-------------+\n");
	seq_puts(f, " |    Status 32           |\n");
	seq_puts(f, " +--------------+---------+\n");
	seq_printf(f, " | %-12s | %-7s |\n", "Halt", BOOL2STR(status.halt));
	seq_printf(f, " | %-12s | %-7s |\n", "Overflow",
		   BOOL2STR(status.overflow));
	seq_printf(f, " | %-12s | %-7s |\n", "Exception",
		   BOOL2STR(status.exception));
	seq_printf(f, " | %-12s | %-7s |\n", "Zero", BOOL2STR(status.zero));
	seq_printf(f, " | %-12s | %-7s |\n", "Zero_div_exp",
		   BOOL2EN(status.zero_div_exp_en));
	seq_printf(f, " | %-12s | %-7s |\n", "Stack_check",
		   BOOL2EN(status.stack_check));
	seq_puts(f, " +--------------+---------+\n");
	seq_puts(f, " |    Registers           |\n");
	seq_puts(f, " +-----+------------------+\n");
	for (i = 0; i < ARRAY_SIZE(regs); i++)
		seq_printf(f, " | R%02u | %#010x       |\n", i, regs[i]);
	seq_puts(f, " +-----+------------------+\n");
}

static void uc_egr_task_show(struct seq_file *f)
{
	uc_task_show(UC_IS_EGR, f);
}
PP_DEFINE_DEBUGFS(egr_task, uc_egr_task_show, uc_task_set);

static void uc_ing_task_show(struct seq_file *f)
{
	uc_task_show(UC_IS_ING, f);
}
PP_DEFINE_DEBUGFS(ing_task, uc_ing_task_show, uc_task_set);

static void uc_egr_writer_show(struct seq_file *f)
{
	u32 port, cid, queue, credits;
	bool en;
	u8 num_cpu;
	static const char *const port_str[UC_EGR_MAX_PORT] = {
		"MULTICAST", "REASSEMBLY", "FRAGMENTATION",
		"TURBODOX",  "IPSEC",
	};

	uc_ccu_maxcpus_get(UC_IS_EGR, &num_cpu);

	seq_puts(f,
		 " +---------------------------------------------------------------+\n");
	seq_printf(f, " | %-16s", "Port");
	seq_printf(f, " | %-7s", "Enable");
	seq_printf(f, " | %-6s", "Credit");
	seq_printf(f, " | %-8s", "Mapping");
	seq_printf(f, " | %12s |\n", "Queue Credit");
	seq_puts(f,
		 " +------------------+---------+--------+----------+--------------+\n");

	for (port = 0; port < UC_EGR_MAX_PORT; port++) {
		uc_egr_writer_port_en_get(port, &en);
		uc_egr_writer_port_credit_get(port, &credits);
		seq_puts(f, " |");
		seq_printf(f, " %-2u-%-13s |", port, port_str[port]);
		seq_printf(f, " %-7s |", BOOL2EN(en));
		seq_printf(f, " %-6u |", credits);

		for (cid = 0; cid < (u32)num_cpu; cid++) {
			if (cid != 0)
				seq_puts(f,
					 " |                  |         |        |");
			uc_egr_writer_map_get(cid, port, &queue);
			uc_egr_writer_queue_credit_get(cid, port, &credits);
			seq_printf(f, " CPU%u: Q%u |", cid, queue);
			seq_printf(f, " %-12u |\n", credits);
		}
		seq_puts(f,
			 " |------------------+---------+--------+----------+--------------|\n");
	}
}

PP_DEFINE_DEBUGFS(egr_writer, uc_egr_writer_show, NULL);

static struct debugfs_file uc_dbg_files[] = {
	{"nf_cfg", &PP_DEBUGFS_FOPS(nf_cfg)},
	{"core_run_halt", &PP_DEBUGFS_FOPS(core_run_halt)},
	{"uc_enable", &PP_DEBUGFS_FOPS(uc_enable)},
	{"fat", &PP_DEBUGFS_FOPS(fat)},
	{"ccu_gpreg", &PP_DEBUGFS_FOPS(ccu_gpreg)},
	{"ccu_irr", &PP_DEBUGFS_FOPS(ccu_irr)},
	{"reader_irr", &PP_DEBUGFS_FOPS(reader_irr)},
	{"log", &PP_DEBUGFS_FOPS(uc_log)},
	{"aux_reg", &PP_DEBUGFS_FOPS(uc_aux_reg)},
	{"version", &PP_DEBUGFS_FOPS(version)},
	{"egr_tasks", &PP_DEBUGFS_FOPS(egr_tasks)},
	{"egr_task", &PP_DEBUGFS_FOPS(egr_task)},
	{"ing_tasks", &PP_DEBUGFS_FOPS(ing_tasks)},
	{"ing_task", &PP_DEBUGFS_FOPS(ing_task)},
	{"egr_writer", &PP_DEBUGFS_FOPS(egr_writer)},
};

s32 uc_dbg_init(struct dentry *parent)
{
	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

	/* uc debugfs dir */
	return pp_debugfs_create(parent, "uc", &dbgfs, uc_dbg_files,
				 ARRAY_SIZE(uc_dbg_files), NULL);
}

void uc_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);
	dbgfs = NULL;
}
