/*
 * port_dist_internal.h
 * Description: Packet Processor Port Distributor Internal Definitions
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#ifndef __PORT_DIST_INTERNAL_H__
#define __PORT_DIST_INTERNAL_H__
#include <linux/types.h>
#include <linux/bitmap.h>
#include "pp_regs.h"
#include "port_distr_regs.h"

#ifdef pr_fmt
#undef pr_fmt
#define pr_fmt(fmt) "[PP_PORT_DIST]:%s:%d: " fmt, __func__, __LINE__
#endif

extern u64 port_dist_base_addr;
#define PORT_DIST_BASE_ADDR port_dist_base_addr

/**
 * @brief port_distr_source_port_to_rpb_port - mapping src port
 *        to rpb port and tc
 *      [31-5] [RO]: Reserved
 *      [4-3]  [RW]: TC
 *      [2]    [RW]: TC select (0-take tc from STW, 1-from table)
 *      [1-0]  [RW]: RPB port
 */
#define PORT_DIST_MAP_REG_BASE                     port_dist_base_addr
#define RPB_PORT_FIELD                             GENMASK(1, 0)
#define TC_SEL_FIELD                               BIT(2)
#define TC_NUM_FIELD                               GENMASK(4, 3)

/**
 * @brief port_distr_pkt_cnt - packets counter per rpb port per
 *        tc
 *      [31-0] [RW]: packet counter
 */
#define PORT_DIST_TC_PKT_CNT_REG(_rpb, _tc)                           \
	(PP_PORT_DIST_PKT_CNT_REG_IDX(((u32)(_rpb) << 2) + (u32)_tc))

/**
 * @brief port_distr_byte_cnt - bytes counter per rpb port per
 *        tc
 *      [31-0] [RW]: bytes counter
 */
#define PORT_DIST_TC_BYTE_CNT_REG(_rpb, _tc)                           \
	(PP_PORT_DIST_BYTE_CNT_REG_IDX(((u32)(_rpb) << 2) + (u32)_tc))

#ifdef CONFIG_DEBUG_FS
/**
 * @brief port distributor debug init
 *
 * @param void
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 port_dist_dbg_init(struct dentry *parent);

/**
 * @brief port distributor debug cleanup
 *
 * @param void
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 port_dist_dbg_clean(void);
#else
static inline s32 port_dist_dbg_init(struct dentry *parent)
{
	return 0;
}

static inline s32 port_dist_dbg_clean(void)
{
	return 0;
}
#endif
#endif /* __PORT_DIST_INTERNAL_H__ */
