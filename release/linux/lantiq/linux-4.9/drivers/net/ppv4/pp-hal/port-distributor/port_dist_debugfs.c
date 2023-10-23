/*
 * port_dist_debugfs.c
 * Description: Packet Processor Port Distributor Debug FS Interface
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/debugfs.h>
#include "pp_common.h"
#include "pp_debugfs_common.h"
#include "port_dist.h"
#include "port_dist_internal.h"

static struct dentry *dbgfs;

void rpb_tc_stat_show_help(struct seq_file *f)
{
	seq_puts(f, "<rpb port> <tc>\n");
}

void rpb_tc_stat_show(char *cmd_buf, void *data)
{
	struct pp_stats stat;
	u16 rpb_port, tc;

	if (unlikely(sscanf(cmd_buf, "%hu %hu", &rpb_port, &tc) != 2)) {
		pr_err("sscanf error\n");
		return;
	}

	if (unlikely(port_dist_tc_stat_get(rpb_port, tc, &stat))) {
		pr_err("port_dist_tc_stat_get failed\n");
		return;
	}

	pr_info("RPB[%hu] TC[%hu] stat: packets %llu bytes %llu\n",
		rpb_port, tc, stat.packets, stat.bytes);
}

/**
 * @brief print packets and bytes statistics for RPB port
 *        specific TC
 */
PP_DEFINE_DEBUGFS(rpb_tc_stat, rpb_tc_stat_show_help, rpb_tc_stat_show);

void port_map_set_help(struct seq_file *f)
{
	seq_puts(f, "<src port> <rpb port> <tc>\n");
}

void port_map_set(char *cmd_buf, void *data)
{
	struct port_dist_map map;

	if (unlikely(sscanf(cmd_buf, "%hu %hu %hu",
			    &map.src_port, &map.rpb_port, &map.tc) != 3)) {
		pr_err("sscanf error\n");
		return;
	}

	if (unlikely(port_dist_port_map_set(&map))) {
		pr_err("port_dist_port_map_set failed\n");
		return;
	}

	pr_info("SRC_PORT[%hu] mapped to RPB[%hu] TC[%d]\n",
		map.src_port, map.rpb_port, map.tc);
}

/**
 * @brief set source port mapping information
 */
PP_DEFINE_DEBUGFS(port_map, port_map_set_help, port_map_set);

/**
 * @brief print source port mapping information
 *
 * @param data unused
 * @param val value from user (source port id)
 *
 * @return int 0 on success, non-zero value otherwise
 */
static int dbg_port_map_show(void *data, u64 val)
{
	struct port_dist_map map;

	map.src_port = (u16)val;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	if (unlikely(port_dist_port_map_get(&map))) {
		pr_err("port_dist_port_map_get failed\n");
		return -EINVAL;
	}

	pr_info("SRC_PORT[%hu] mapped to RPB[%hu] TC[%d]\n",
		map.src_port, map.rpb_port, map.tc);

	return 0;
}

PP_DEFINE_DBGFS_ATT(port_map_show, NULL, dbg_port_map_show);

/**
 * @brief show all source ports mapping information
 *
 * @param f file pointer
 * @param unused unused
 *
 * @return int 0 on success, non-zero value otherwise
 */
void dbg_map_all_show(struct seq_file *f)
{
	u16 i;
	struct port_dist_map map;

	for (i = 0; i < PP_MAX_PORT; i++) {
		map.src_port = i;
		if (unlikely(port_dist_port_map_get(&map))) {
			pr_err("port_dist_port_map_get failed\n");
			return;
		}
		seq_printf(f, "PORT[%03hu] --> RPB[%hu] TC[%d]\n",
			   i, map.rpb_port, map.tc);
	}
}

PP_DEFINE_DEBUGFS(map_all_show, dbg_map_all_show, NULL);

/**
 * @brief print source port packets and bytes statistics
 *
 * @param data unused
 * @param val value from user (source port id)
 *
 * @return int 0 on success, non-zero value otherwise
 */
static int dbg_port_stat_show(void *data, u64 val)
{
	struct pp_stats stat;

	if (!capable(CAP_SYS_PACCT))
		return -EPERM;

	if (unlikely(port_dist_port_stat_get((u16)val, &stat))) {
		pr_err("port_dist_port_stat_get failed\n");
		return -EINVAL;
	}

	pr_info("SRC_PORT[%hu] stat: packets %llu bytes %llu\n",
		(u16)val, stat.packets, stat.bytes);

	return 0;
}

PP_DEFINE_DBGFS_ATT(port_stat, NULL, dbg_port_stat_show);

/**
 * @brief set default mapping values for all source ports
 *
 */
void dbg_default_map_set(struct seq_file *f)
{
	port_dist_default_set();
}

PP_DEFINE_DEBUGFS(default_map_set, dbg_default_map_set, NULL);

/**
 * @brief set default mapping values for all source ports
 *
 */
void dbg_reset_stat(struct seq_file *f)
{
	port_dist_reset_stat();
}

PP_DEFINE_DEBUGFS(reset_stat, dbg_reset_stat, NULL);

static struct debugfs_file port_dist_dbg_files[] = {
	{ "port_map_set", &PP_DEBUGFS_FOPS(port_map) },
	{ "rpb_tc_stat_show", &PP_DEBUGFS_FOPS(rpb_tc_stat) },
	{ "port_map_show", &PP_DEBUGFS_FOPS(port_map_show) },
	{ "port_stat_show", &PP_DEBUGFS_FOPS(port_stat) },
	{ "port_map_all_show", &PP_DEBUGFS_FOPS(map_all_show) },
	{ "default_map_set", &PP_DEBUGFS_FOPS(default_map_set) },
	{ "reset_stat", &PP_DEBUGFS_FOPS(reset_stat) },
};

s32 port_dist_dbg_init(struct dentry *parent)
{
	/* port distributor debugfs dir */
	return pp_debugfs_create(parent, "port_dist", &dbgfs,
				 port_dist_dbg_files,
				 ARRAY_SIZE(port_dist_dbg_files), NULL);
}

s32 port_dist_dbg_clean(void)
{
	debugfs_remove_recursive(dbgfs);

	return 0;
}
