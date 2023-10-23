/*
 * port_dist.h
 * Description: Packet Processor Port Distributor
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018 Intel Corporation
 */
#ifndef __PORT_DIST_H__
#define __PORT_DIST_H__
#include <linux/types.h>
#include <linux/pp_api.h>
#include <linux/debugfs.h>

/**
 * @brief Port Distributor  Init Config
 * @valid params valid
 * @port_dist_base port dist base address
 * @port_dist_phys_base port dist physical base address
 * @dbgfs debugfs parent directory
 */
struct port_dist_init_param {
	bool valid;
	u64 port_dist_base;
	struct dentry *dbgfs;
};

#ifdef CONFIG_PPV4_LGM
/**
 * @brief definition of a source port mapping to PP RPB port
 *
 * @src_port pp source port id
 * @rpb_port rpb port id
 * @tc traffic class, PP_MAX_TC for STW TC
 */
struct port_dist_map {
	u16 src_port;
	u16 rpb_port;
	u16 tc;
};

#define PORT_DIST_DEFAULT_RPB (PP_MAX_RPB_PORT - 1)
#define PORT_DIST_DEFAULT_TC  (PP_MAX_TC)

/**
 * @brief set source port mapping to RPB module
 *
 * @param map mapping data structure to set
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 port_dist_port_map_set(const struct port_dist_map *map);

/**
 * @brief get PP port mapping to RBP module
 *
 * @param map mapping data structure from database
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 port_dist_port_map_get(struct port_dist_map *map);

/**
 * @brief set default PP port mapping to RPB module, this API
 *        mapped all the source port to lowest RBB (3) port with
 *        lowest TC (3)
 *
 * @param void
 */
void port_dist_default_set(void);

/**
 * @brief get RPB port statistics per TC
 *
 * @param rpb_port RPB port id
 * @param tc traffic class [0..PP_MAX_TC]
 * @param stat pp statistics structure bytes & packets
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 port_dist_tc_stat_get(u16 rpb_port, u16 tc, struct pp_stats *stat);

/**
 * @brief get source port statistics
 *
 * @param src_port source port id
 * @param stat pp statistics structure bytes & packets
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 port_dist_port_stat_get(u16 src_port, struct pp_stats *stat);

/**
 * @brief get rx statistics
 *
 * @param stat pp statistics structure bytes & packets
 *
 * @return s32 0 for success, non-zero otherwise
 */
s32 port_dist_stats_get(struct pp_stats *stat);

/**
 * @brief reset all HW statistics counters
 *
 * @param void
 */
void port_dist_reset_stat(void);

/**
 * @brief Module init function
 * @param init_param initial parameters
 * @return s32 0 on success, non-zero value otherwise
 */
s32 port_dist_init(struct port_dist_init_param *init_param);

/**
 * @brief Module exit function, clean all resources
 *
 * @return void
 */
void port_dist_exit(void);
#else
static inline s32 port_dist_init(struct port_dist_init_param *cfg)
{
	if (cfg->valid)
		return -EINVAL;
	else
		return 0;
}

static inline void port_dist_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __PORT_DIST_H__ */
