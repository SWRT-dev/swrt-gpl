/*
 * pp_logger_debugfs.c
 * Description: Packet Processor logger Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */

#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "pp_logger.h"
#include "pp_logger_internal.h"

/* PP Logger Debug FS directory */
static struct dentry *dbgfs;

/**
 * @brief Debugfs status attribute show API, print log level and
 *        components status (enable/disable)
 */
void __logger_status_show(struct seq_file *f)
{
	enum log_level l;
	u32 en[LOGGER_COMP_NUM] = { 0 };
	u32 comp;

	/* get all info before printing something */
	LOGGER_FOR_ALL_COMPS(comp)
		if (logger_comp_get(comp, &en[comp]))
			return;

	if (logger_level_get(&l))
		return;

	seq_puts(f, "\n");
	seq_puts(f, " Packet Processor Logger\n");
	seq_puts(f, " -----------------------\n");
	seq_printf(f, "  %-20s ==> %s(%u)\n", "Log Level",
		__logger_level_str(l), l);

	LOGGER_FOR_ALL_COMPS(comp) {
		seq_printf(f, "  %2u - %-15s ==> %s\n", comp,
			__logger_comp_str(comp),
			en[comp] ? "Enable" : "Disable");
	}
	seq_puts(f, "\n\n");
}

PP_DEFINE_DEBUGFS(status, __logger_status_show, NULL);

/**
 * @brief Debugfs comp attribute set API, toggle
 *        component status
 * @param data
 * @param val comp index
 * @return
 */
static int __logger_comp_set(void *data, u64 val)
{
	enum logger_comp c = (enum logger_comp) val;
	u32 en  = 0;
	u32 ret = 0;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	ret = logger_comp_get(c, &en);
	if (unlikely(ret))
		return ret;

	return logger_comp_set(c, !en);
}

/**
 * @brief Debugfs level attribute set API, set log level
 * @param data
 * @param val level
 * @return 0 on success, non-zero value otherwise
 */
static int __logger_level_set(void *data, u64 val)
{
	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	return logger_level_set((enum log_level) val);
}

/**
 * @brief Debugfs level attribute get API, set log level
 * @param data
 * @param val level
 * @return 0 on success, non-zero value otherwise
 */
static int __logger_level_get(void *data, u64 *val)
{
	enum log_level l;
	s32 ret;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	ret = logger_level_get(&l);
	if (unlikely(ret))
		return ret;

	*val = (u64) l;
	return 0;
}

PP_DEFINE_DBGFS_ATT(comp_fops, NULL, __logger_comp_set);
PP_DEFINE_DBGFS_ATT(level_fops, __logger_level_get, __logger_level_set);

static struct debugfs_file debugfs_files[] = {
	{"comp", &PP_DEBUGFS_FOPS(comp_fops)},
	{"level", &PP_DEBUGFS_FOPS(level_fops)},
	{"status", &PP_DEBUGFS_FOPS(status)},
};

s32 logger_dbg_init(struct dentry *parent)
{
	s32 ret;

	/* logger debugfs dir */
	ret = pp_debugfs_create(parent, "logger", &dbgfs, debugfs_files,
				ARRAY_SIZE(debugfs_files), NULL);
	if (unlikely(ret))
		pr_err("Failed to create logger debugfs dir\n");

	return ret;
}

s32 logger_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);

	return 0;
}
