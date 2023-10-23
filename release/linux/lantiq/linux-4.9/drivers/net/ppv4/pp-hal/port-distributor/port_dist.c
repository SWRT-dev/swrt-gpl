/*
 * port_dist.c
 * Description: Packet Processor Port Distributor Driver
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/errno.h>
#include <linux/bitfield.h>
#include <linux/bitops.h>
#include "pp_regs.h"
#include "pp_common.h"
#include "port_dist.h"
#include "port_dist_internal.h"

u64 port_dist_base_addr;

s32 port_dist_port_map_set(const struct port_dist_map *map)
{
	u32 reg_val = 0;
	u16 src;

	if (unlikely((!map)                                 ||
		     (!PP_IS_PORT_VALID(map->src_port))     ||
		     (!PP_IS_RPB_PORT_VALID(map->rpb_port)) ||
		     (map->tc > PP_MAX_TC))) {
		pr_err("Invalid parameters\n");
		return -EINVAL;
	}

	reg_val |= FIELD_PREP(RPB_PORT_FIELD, map->rpb_port);
	if (PP_IS_TC_VALID(map->tc)) {
		reg_val |= FIELD_PREP(TC_NUM_FIELD, map->tc);
		reg_val |= FIELD_PREP(TC_SEL_FIELD, 1);
	}

	/* Write to register */
	src = map->src_port;
	PP_REG_WR32(PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_REG_IDX(src),
		    reg_val);

	return 0;
}

s32 port_dist_port_map_get(struct port_dist_map *map)
{
	u32 reg_val = 0;
	u16 src;

	if (unlikely((!map) || (!PP_IS_PORT_VALID(map->src_port)))) {
		pr_err("Invalid parameters\n");
		return -EINVAL;
	}

	/* Read from register */
	src = map->src_port;
	reg_val =
		PP_REG_RD32(PP_PORT_DIST_SOURCE_PORT_TO_RPB_PORT_REG_IDX(src));

	map->rpb_port = FIELD_GET(RPB_PORT_FIELD, reg_val);

	if (FIELD_GET(TC_SEL_FIELD, reg_val))
		map->tc = FIELD_GET(TC_NUM_FIELD, reg_val);
	else
		map->tc = PP_MAX_TC;

	return 0;
}

void port_dist_default_set(void)
{
	struct port_dist_map map = { 0 };

	/* RPB last port */
	map.rpb_port = PORT_DIST_DEFAULT_RPB;
	/* PP_MAX_TC means use the STW TC */
	map.tc = PORT_DIST_DEFAULT_TC;
	/* Map all ports */
	for (; map.src_port < PP_MAX_PORT; map.src_port++)
		port_dist_port_map_set(&map);
}

s32 port_dist_tc_stat_get(u16 rpb_port, u16 tc, struct pp_stats *stat)
{
	if (unlikely((!stat)               ||
		     (!PP_IS_TC_VALID(tc)) ||
		     (!PP_IS_RPB_PORT_VALID(rpb_port)))) {
		pr_err("Invalid parameters\n");
		return -EINVAL;
	}

	/* Read packets counter */
	stat->packets =
		(u64)PP_REG_RD32(PORT_DIST_TC_PKT_CNT_REG(rpb_port, tc));
	/* Read bytes counter */
	stat->bytes =
		(u64)PP_REG_RD32(PORT_DIST_TC_BYTE_CNT_REG(rpb_port, tc));

	return 0;
}

s32 port_dist_port_stat_get(u16 src_port, struct pp_stats *stat)
{
	u32 high, low;

	if (unlikely((!stat) || (!PP_IS_PORT_VALID(src_port)))) {
		pr_err("Invalid parameters\n");
		return -EINVAL;
	}

	/* Read bytes low & high counters */
	low  = PP_REG_RD32(PP_PORT_DIST_PORT_BYTE_CNT_LO_REG_IDX(src_port));
	high = PP_REG_RD32(PP_PORT_DIST_PORT_BYTE_CNT_HI_REG_IDX(src_port));
	stat->bytes = PP_BUILD_64BIT_WORD(low, high);

	/* Read packets low & high counters */
	low  = PP_REG_RD32(PP_PORT_DIST_PORT_PKT_CNT_LO_REG_IDX(src_port));
	high = PP_REG_RD32(PP_PORT_DIST_PORT_PKT_CNT_HI_REG_IDX(src_port));
	stat->packets = PP_BUILD_64BIT_WORD(low, high);

	return 0;
}

s32 port_dist_stats_get(struct pp_stats *stat)
{
	struct pp_stats port_stats;
	s32 stat_ret;
	u32 port_idx;

	if (unlikely((!stat))) {
		pr_err("Invalid parameters\n");
		return -EINVAL;
	}

	memset(stat, 0, sizeof(struct pp_stats));

	for (port_idx = 0 ; port_idx < PP_MAX_PORT ; port_idx++) {
		stat_ret = port_dist_port_stat_get(port_idx, &port_stats);
		if (stat_ret) {
			pr_err("port_dist_port_stat_get for port %d failed\n",
			       port_idx);
			return stat_ret;
		}

		stat->packets += port_stats.packets;
		stat->bytes += port_stats.bytes;
	}

	return 0;
}

void port_dist_reset_stat(void)
{
	u32 port, tc;

	/* Reset all ports counters */
	for (port = 0; port < PP_MAX_PORT; port++) {
		PP_REG_WR32(PP_PORT_DIST_PORT_BYTE_CNT_LO_REG_IDX(port), 0);
		PP_REG_WR32(PP_PORT_DIST_PORT_BYTE_CNT_HI_REG_IDX(port), 0);
		PP_REG_WR32(PP_PORT_DIST_PORT_PKT_CNT_LO_REG_IDX(port), 0);
		PP_REG_WR32(PP_PORT_DIST_PORT_PKT_CNT_HI_REG_IDX(port), 0);
	}

	/* Reset all RPB TC counters */
	for (port = 0; port < PP_MAX_RPB_PORT; port++) {
		for (tc = 0; tc < PP_MAX_TC; tc++) {
			PP_REG_WR32(PORT_DIST_TC_PKT_CNT_REG(port, tc), 0);
			PP_REG_WR32(PORT_DIST_TC_BYTE_CNT_REG(port, tc), 0);
		}
	}
}

void port_dist_exit(void)
{
	port_dist_dbg_clean();
}

s32 port_dist_init(struct port_dist_init_param *init_param)
{
	if (!init_param->valid)
		return -EINVAL;

	port_dist_base_addr = init_param->port_dist_base;

	if (unlikely(pp_reg_poll(PP_PORT_DIST_INIT_DONE_REG,
				 PP_PORT_DIST_INIT_DONE_MSK, 1))) {
		pr_err("Failed polling on init done register\n");
		return -1;
	}

	port_dist_default_set();

	if (unlikely(port_dist_dbg_init(init_param->dbgfs))) {
		pr_err("Failed to init debug stuff\n");
		return -1;
	}

	pr_debug("pp port distributor was initialized\n");

	return 0;
}
