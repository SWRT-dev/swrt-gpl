/*
 * pp_misc_debugfs.c
 * Description: PP Misc Driver Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/device.h>
#include <linux/init.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include <linux/atomic.h>
#include <linux/pp_api.h>
#include <linux/jiffies.h>
#include <linux/parser.h>
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_buffer_mgr.h"
#include "pp_dev.h"
#include "pp_misc.h"
#include "pp_misc_internal.h"
#include "rpb.h"
#include "bm.h"
#include "pp_desc.h"
#include "pp_session_mgr.h"

/* PP infra Debug FS directory */
static struct dentry *dbgfs;

#if IS_ENABLED(CONFIG_OF)
#define REG_INVALID (0xFFFFFFFF)

static phys_addr_t ppv4_reg_base;

void regs_dump_rd(struct seq_file *f)
{
	seq_puts(f,
		 "<physical address> <num regs (optional)> <regs per line>\n");
}

void regs_dump_wr(char *cmd_buf, void *data)
{
	unsigned long addr;
	u32 num_regs = 1;
	u32 regs_per_line = 4;
	u32 reg, i, n = 0;
	char kbuf[128];
	void *virt;
	s32 ret;

	ret = sscanf(cmd_buf, "%lx %u %u", &addr, &num_regs, &regs_per_line);
	if (ret < 2) {
		ret = kstrtoul(cmd_buf, 0, &addr);
		if (ret) {
			pr_err("kstrtou32 err (%d)\n", ret);
			return;
		}
	}

	/* limit number of bytes to print */
	num_regs = num_regs > 4096 ? 4096 : num_regs;

	kbuf[0] = '\0';
	for (i = 0; i < num_regs; i++, addr += 4) {
		if (!(i % regs_per_line)) {
			if (i)
				pr_info("%s\n", kbuf);
			n = scnprintf(kbuf, sizeof(kbuf), "0x%08lx:\t", addr);
		}

		if (addr >= ppv4_reg_base) {
			/* For PPv4 registers use offset from virtual base */
			virt = pp_phys_to_virt(addr);
			if (virt) {
				reg = PP_REG_RD32(virt);
			} else {
				virt = ioremap(addr, 4);
				if (virt) {
					reg = PP_REG_RD32(virt);
					iounmap(virt);
				} else {
					reg = REG_INVALID;
				}
			}
		} else {
			/* For DDR use phys_to_virt */
			reg = *(volatile u32 *)phys_to_virt(addr);
		}

		n += scnprintf(kbuf + n, sizeof(kbuf) - n, "%08X ", reg);
	}
	pr_info("%s\n\n", kbuf);
}

void reg_write_32_rd(struct seq_file *f)
{
	seq_puts(f, "<physical address> <value>\n");
}

void reg_write_32_wr(char *cmd_buf, void *data)
{
	unsigned long addr;
	void *virt;
	u32 val;

	if (sscanf(cmd_buf, "%lx %x", &addr, &val) != 2) {
		pr_err("sscanf err\n");
		return;
	}

	if (addr >= ppv4_reg_base) {
		/* For PPv4 registers use offset from virtual base */
		virt = pp_phys_to_virt(addr);
		if (virt) {
			PP_REG_WR32(virt, val);
		} else {
			virt = ioremap(addr, 4);
			if (virt) {
				PP_REG_WR32(virt, val);
				iounmap(virt);
			}
		}
	} else {
		/* For DDR use phys_to_virt */
		*((u32 *)phys_to_virt(addr)) = val;
	}
}

PP_DEFINE_DEBUGFS(regs_rd, regs_dump_rd, regs_dump_wr);
PP_DEFINE_DEBUGFS(regs_wr, reg_write_32_rd, reg_write_32_wr);
#endif

#if IS_ENABLED(CONFIG_PPV4_LGM)
/**
 * @brief Set brief max lines
 */
static void __brief_max_cnt_set(char *cmd_buf, void *data)
{
	u32 cnt;

	if (kstrtou32(cmd_buf, 10, &cnt))
		pr_info("failed to parse '%s'\n", cmd_buf);

	pr_info("Brief max lines: %u\n", cnt);
	pp_misc_set_brief_cnt(cnt);
}

/**
 * @brief Reset all PP HW (HAL) statistics
 */
void __pp_global_reset(char *cmd_buf, void *data)
{
	if (strncmp(cmd_buf, "0", strlen("0")))
		return;

	pp_global_stats_reset();
}

static void __pp_pps_stats_show(struct seq_file *f)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return;

	pp_pps_show_seq(f, sizeof(struct pp_global_stats), 1,
			pp_global_stats_get, pp_global_stats_diff,
			pp_global_stats_show, qdev);
}

PP_DEFINE_DEBUGFS(pps, __pp_pps_stats_show, __pp_global_reset);

/**
 * @brief Debugfs stats attribute show API, prints global PP
 *        statistics
 */
static void __pp_global_show(struct seq_file *f)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return;

	pp_stats_show_seq(f, sizeof(struct pp_global_stats), 1,
			  pp_global_stats_get, pp_global_stats_show, qdev);
}

PP_DEFINE_DEBUGFS(global, __pp_global_show, __pp_global_reset);

static void __pp_hal_pps_stats_show(struct seq_file *f)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return;

	pp_pps_show_seq(f, sizeof(struct pp_global_stats), 1,
			pp_global_stats_get, pp_global_stats_diff,
			pp_hal_global_stats_show, qdev);
}

PP_DEFINE_DEBUGFS(hal_pps, __pp_hal_pps_stats_show, __pp_global_reset);

static void __pp_brief_show(struct seq_file *f)
{
	struct smgr_stats stats;

	if (pp_smgr_stats_get(&stats))
		return;

	pp_pps_show_seq(f, sizeof(struct pp_active_sess_stats),
		atomic_read(&stats.sess_open), pp_brief_stats_get,
		pp_brief_stats_diff, pp_brief_stats_show, NULL);
}

PP_DEFINE_DEBUGFS(brief, __pp_brief_show, __brief_max_cnt_set);

/**
 * @brief Debugfs stats attribute show API, prints global PP
 *        statistics
 */
static void __pp_hal_global_show(struct seq_file *f)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return;

	pp_stats_show_seq(f, sizeof(struct pp_global_stats), 1,
			  pp_global_stats_get, pp_hal_global_stats_show, qdev);
}

PP_DEFINE_DEBUGFS(hal_global, __pp_hal_global_show, __pp_global_reset);

/**
 * @brief Debugfs stats attribute show API, prints global PP
 *        statistics
 */
static void __pp_driver_stats_show(struct seq_file *f)
{
	struct pp_qos_dev *qdev;

	qdev = pp_qos_dev_open(PP_QOS_INSTANCE_ID);
	if (unlikely(ptr_is_null(qdev)))
		return;

	pp_stats_show_seq(f, sizeof(struct pp_driver_stats), 1,
			  pp_driver_stats_get, pp_driver_stats_show, qdev);
}

void __pp_driver_stats_reset(char *cmd_buf, void *data)
{
	if (strncmp(cmd_buf, "0", strlen("0")))
		return;

	pp_driver_stats_reset();
}

PP_DEFINE_DEBUGFS(dstats, __pp_driver_stats_show, __pp_driver_stats_reset);

static void __pp_resource_stats_show(struct seq_file *f)
{
	pp_resource_stats_show(f->buf, f->size, &f->count);
}

PP_DEFINE_DEBUGFS(resources, __pp_resource_stats_show, NULL);

/**
 * @brief Debugfs version attribute show API, prints pp version
 */
void __pp_ver_show(struct seq_file *f)
{
	struct pp_version drv, fw, hw;
	s32 ret = 0;

	ret |= pp_version_get(&drv, PP_VER_TYPE_DRV);
	ret |= pp_version_get(&fw, PP_VER_TYPE_FW);
	ret |= pp_version_get(&hw, PP_VER_TYPE_HW);

	if (unlikely(ret)) {
		pr_err("Failed to get PP driver version\n");
		return;
	}

	ret = pp_version_show(f->buf, f->size, &f->count, &drv, &fw, &hw);
	if (unlikely(ret))
		pr_err("Failed to show PP driver version\n");
}

PP_DEFINE_DEBUGFS(version, __pp_ver_show, NULL);

/**
 * @brief Print descriptor format from DDR physical address
 * @param data unused
 * @param val DDR physical address of descriptor
 * @return 0 on success
 */
static int __pp_desc_show(void *data, u64 val)
{
	struct pp_desc desc;
	struct pp_hw_desc hw_desc;

	memset(&hw_desc, 0, sizeof(hw_desc));
	memcpy((void *)&hw_desc, phys_to_virt(val),
	       sizeof(hw_desc));
	pp_desc_decode(&desc, &hw_desc);
	pp_desc_dump(&desc);

	return 0;
}

PP_DEFINE_DBGFS_ATT(desc, NULL, __pp_desc_show);

/**
 * @brief Print start of descriptor format from Debug registers
 *        physical address
 * @return 0 on success
 */
void __pp_rxdma_desc_show(struct seq_file *f)
{
	struct pp_desc desc;
	struct pp_hw_desc hw_desc;

	rx_dma_desc_dbg_get((struct pp_hw_dbg_desc *)(&hw_desc));
	pp_desc_top_decode(&desc, &hw_desc);
	pp_desc_top_dump(&desc);
}

PP_DEFINE_DEBUGFS(rxdma_desc, __pp_rxdma_desc_show, NULL);

void __pp_clk_freq_status_show(struct seq_file *f)
{
	u32 dbg_ignore_updates;

	dbg_ignore_updates = ignore_clk_updates_get();
	if (dbg_ignore_updates)
		seq_puts(f, "Listen to clk freq updates is disabled\n");
	else
		seq_puts(f, "Listen to clk freq updates is enabled\n");
}

void __pp_clk_freq_status_set(char *cmd_buf, void *data)
{
	u32 dbg_ignore_updates;

	if (kstrtou32(cmd_buf, 10, &dbg_ignore_updates))
		pr_info("failed to parse '%s'\n", cmd_buf);

	if (dbg_ignore_updates != 0 && dbg_ignore_updates != 1) {
		pr_info("/sys/kernel/debug/pp/misc/ignore_clk_updates help:\n");
		pr_info("echo 0 > /sys/kernel/debug/pp/misc/ignore_clk_updates : Allow updates to be processed by PPv4 driver\n");
		pr_info("echo 1 > /sys/kernel/debug/pp/misc/ignore_clk_updates : Ignore updates from being processed by PPv4 driver\n");
		return;
	}

	ignore_clk_updates_set(dbg_ignore_updates);
}

PP_DEFINE_DEBUGFS(ignore_clk_updates, __pp_clk_freq_status_show,
		  __pp_clk_freq_status_set);

void __free_running_ctr_show(struct seq_file *f)
{
	unsigned long addr;
	void *virt;
	u32  val = 0;

	addr = 0xF1050438;
	virt = pp_phys_to_virt(addr);
	if (virt) {
		val = PP_REG_RD32(virt);
	} else {
		virt = ioremap(addr, 4);
		if (virt) {
			val = PP_REG_RD32(virt);
			iounmap(virt);
		}
	}

	seq_printf(f, "Free Running Counter: %u\n", val);
}

PP_DEFINE_DEBUGFS(free_running_ctr, __free_running_ctr_show, NULL);

/**
 * @brief Set PP driver hw clock frequency
 */
static int __misc_dbg_clk_set(void *data, u64 val)
{
	return pp_hw_clock_freq_set((u32)val);
}

/**
 * @brief Get PP driver hw clock frequency
 */
static int __misc_dbg_clk_get(void *data, u64 *val)
{
	u32 clk = 0;
	s32 ret;

	ret  = pp_hw_clock_freq_get(&clk);
	*val = (u64)clk;
	return ret;
}

PP_DEFINE_DBGFS_ATT(clock, __misc_dbg_clk_get, __misc_dbg_clk_set);

enum nf_opts {
	nf_opt_help = 1,
	nf_opt_type,
	nf_opt_pid,
	nf_opt_dst_q,
	nf_opt_c2_q,
};

static const match_table_t nf_tokens = {
	{nf_opt_help,  "help"},
	{nf_opt_type,  "type=%u"},
	{nf_opt_pid,   "pid=%u"},
	{nf_opt_dst_q, "dst_q=%u"},
	{nf_opt_c2_q,  "c2_q=%u"},
};

static const char *pp_nf_str[PP_NF_NUM] = {
	[PP_NF_REASSEMBLY] = "Reassembly",
	[PP_NF_TURBODOX] = "TurboDox",
	[PP_NF_FRAGMENTER] = "Fragmenter",
	[PP_NF_MULTICAST] = "Multicast",
	[PP_NF_IPSEC] = "IPSEC",
};

/**
 * @brief SGC file help
 */
static void __dbg_nf_set_help(void)
{
	u32 type = 0;

	pr_info("\n");
	pr_info(" Brief: Set PP network function\n");
	pr_info(" Usage: echo [type] [pid] [dst_q] [c2_q] > nf_set\n");
	pr_info("   help  - print this help\n");
	pr_info("   pid   - PP pid\n");
	pr_info("   dst_q - destination queue\n");
	pr_info("   c2_q - cycle 2 queue\n");
	pr_info("   type  - nf type\n");

	for (type = 0; type < PP_NF_NUM; type++)
		pr_info("      %u - %s\n", type, pp_nf_str[type]);
}

static s32 __dbg_nf_set_args_parse(char *args_str, enum pp_nf_type *type,
				   struct pp_nf_info *info, bool *help)
{
	substring_t substr[MAX_OPT_ARGS];
	enum nf_opts opt;
	char *tok;
	s32 val;

	args_str = strim(args_str);
	/* iterate over user arguments */
	while ((tok = strsep(&args_str, " \t\n")) != NULL) {
		if (!*tok)
			continue;
		opt = match_token(tok, nf_tokens, substr);
		switch (opt) {
		case nf_opt_help:
			*help = true;
			return 0;
		case nf_opt_type:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			*type = (enum pp_nf_type)val;
			break;
		case nf_opt_pid:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			info->pid = (u16)val;
			break;
		case nf_opt_dst_q:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			info->q = (u16)val;
			break;
		case nf_opt_c2_q:
			if (match_int(&substr[0], &val))
				goto opt_parse_err;
			info->cycl2_q = (u16)val;
			break;
		}
	}
	return 0;

opt_parse_err:
	pr_err("Failed to parse %s value\n", tok);
	return -EINVAL;
}

static void __dbg_nf_set(char *args_str, void *data)
{
	enum pp_nf_type type;
	struct pp_nf_info info;
	bool help;
	s32 ret = 0;

	help = false;
	type = PP_NF_NUM;
	info.pid = U16_MAX;
	info.q = U16_MAX;

	ret = __dbg_nf_set_args_parse(args_str, &type, &info, &help);
	if (unlikely(ret))
		return;

	if (help) {
		__dbg_nf_set_help();
		return;
	}

	if (unlikely(pp_nf_set(type, &info)))
		pr_err("failed to set nf, ret %d\n", ret);
}

/**
 * @brief Dump the nf multicast info
 */
static void __dbg_nf_get(struct seq_file *f)
{
	struct pp_nf_info info = { 0 };
	enum pp_nf_type type = 0;
	s32 ret;

	seq_puts(f, "+------------|-------|------------|--------------+\n");
	seq_printf(f, "| %-10s | %-5s | %-10s | %-12s |\n", "Type", "PID",
		   "Dest Queue", "Cycle2 Queue");
	seq_puts(f, "+------------|-------|------------|--------------+\n");

	for (type = 0; type < PP_NF_NUM; type++) {
		ret = pp_nf_get(type, &info);
		if (unlikely(ret)) {
			seq_printf(f, "failed to get %s nf info, ret %d\n",
				   pp_nf_str[type], ret);
			return;
		}
		seq_printf(f, "| %-10s | %-5u | %-10u | %-12u |\n",
			   pp_nf_str[type], info.pid, info.q, info.cycl2_q);
	}
	seq_puts(f, "+------------|-------|------------|--------------+\n");
}

PP_DEFINE_DEBUGFS(nf, __dbg_nf_get, __dbg_nf_set);

#ifdef CONFIG_PPV4_HW_MOD_REGS_LOGS
/**
 * @brief Set PP registers write logs on/off
 */
static s32 mod_reg_log_en_set(void *data, u64 val)
{
	ulong bmap = (ulong)val;

	return pp_regs_mod_log_en_set(&bmap, HW_MOD_CNT);
}

/**
 * @brief Get PP registers write logs status
 */
static s32 mod_reg_log_en_get(void *data, u64 *val)
{
	ulong bmap = (ulong)*val;
	s32 ret;

	ret  = pp_regs_mod_log_en_get(&bmap, HW_MOD_CNT);
	*val = (u64)bmap;
	return ret;
}

PP_DEFINE_DBGFS_ATT_FMT(mod_reg_log_en, mod_reg_log_en_get, mod_reg_log_en_set,
			"%llx\n");

#endif /* CONFIG_PPV4_HW_MOD_REGS_LOGS */

#endif /* CONFIG_PPV4_LGM */

/**
 * @brief files to create in the misc folder
 */
static struct debugfs_file files[] = {
#if IS_ENABLED(CONFIG_PPV4_LGM)
	{"clock",     &PP_DEBUGFS_FOPS(clock)},
	{"desc_show", &PP_DEBUGFS_FOPS(desc)},
	{"free_running_ctr", &PP_DEBUGFS_FOPS(free_running_ctr)},
	{"nf",        &PP_DEBUGFS_FOPS(nf)},
	{"ignore_clk_updates", &PP_DEBUGFS_FOPS(ignore_clk_updates)},
#endif
#if IS_ENABLED(CONFIG_PPV4_HW_MOD_REGS_LOGS)
	{"module_reg_log_en", &PP_DEBUGFS_FOPS(mod_reg_log_en)},
#endif
};

#if IS_ENABLED(CONFIG_PPV4_LGM)
/**
 * @brief files to create in the root folder
 */
static struct debugfs_file drv_files[] = {
	{"brief",        &PP_DEBUGFS_FOPS(brief)},
	{"version",      &PP_DEBUGFS_FOPS(version)},
	{"driver_stats", &PP_DEBUGFS_FOPS(dstats)},
	{"global",       &PP_DEBUGFS_FOPS(global)},
	{"pps",          &PP_DEBUGFS_FOPS(pps)},
	{"resources",    &PP_DEBUGFS_FOPS(resources)},
};
#endif

/**
 * @brief files to create in the hal folder
 */
static struct debugfs_file hal_files[] = {
	{"read",    &PP_DEBUGFS_FOPS(regs_rd)},
	{"write32", &PP_DEBUGFS_FOPS(regs_wr)},
#if IS_ENABLED(CONFIG_PPV4_LGM)
	{"global",          &PP_DEBUGFS_FOPS(hal_global)},
	{"pps",             &PP_DEBUGFS_FOPS(hal_pps)},
	{"resources",       &PP_DEBUGFS_FOPS(resources)},
	{"rxdma_desc_show", &PP_DEBUGFS_FOPS(rxdma_desc)},
#endif
};

s32 pp_misc_dbg_init(struct pp_misc_init_param *init_param)
{
	s32 ret;

	if (unlikely(!debugfs_initialized())) {
		pr_err("Debugfs not initialized yet\n");
		return -EPERM;
	}

#if IS_ENABLED(CONFIG_OF)
	ppv4_reg_base = init_param->ppv4_base;
#endif

	ret = pp_debugfs_create(init_param->root_dbgfs, "misc", &dbgfs, files,
				ARRAY_SIZE(files), NULL);
	if (unlikely(ret))
		return ret;

#if IS_ENABLED(CONFIG_PPV4_LGM)
	ret = pp_debugfs_create(init_param->root_dbgfs, NULL, NULL, drv_files,
				ARRAY_SIZE(drv_files), NULL);
	if (unlikely(ret))
		return ret;
#endif

#if IS_ENABLED(CONFIG_OF) || IS_ENABLED(CONFIG_PPV4_LGM)
	return pp_debugfs_create(init_param->hal_dbgfs, NULL, NULL, hal_files,
				 ARRAY_SIZE(hal_files), NULL);
	if (unlikely(ret))
		return ret;
#endif

	return ret;
}

s32 pp_misc_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);

	return 0;
}
