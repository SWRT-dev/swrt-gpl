/*
 * rpb.h
 * Description: PP receive packet buffer h file
 *
 * SPDX-License-Identifier: GPL-2.0-only
 * Copyright (C) 2018-2020 Intel Corporation
 */

#ifndef __RPB_H__
#define __RPB_H__
#include <linux/pp_api.h>
#include <linux/debugfs.h>
#include "pp_common.h"

enum rpb_profile_id {
	RPB_PROFILE_FIRST,
	RPB_PROFILE_DEFAULT = RPB_PROFILE_FIRST,
	RPB_PROFILE_DROP,
	RPB_PROFILE_COSTUM,
	RPB_PROFILE_NUM,
};

/**
 * \brief RPB Init Config
 * @valid params valid
 * @mem_sz RPB internal memory size
 * @num_pending_pkts maximum number of packets the rpb can
 *                   can store in his internal memory
 * @rpb_drop_base rpb drop counters base address
 * @rpb_base rpb base address
 * @bm_phys_base base address of the PP buffer manager HW
 *        module, the RPB need this address to return the
 *        buffers of dropped packets
 * @rpb_profile rpb profile
 * @dbgfs debugfs parent directory
 */
struct rpb_init_param {
	bool valid;
	u32  mem_sz;
	u32  num_pending_pkts;
	u64  rpb_drop_base;
	u64  rpb_base;
	phys_addr_t bm_phys_base;
	enum rpb_profile_id rpb_profile;
	struct dentry *dbgfs;
};

#ifdef CONFIG_PPV4_LGM

/**
 * @brief RPB TC configuration structure
 * @dpool_sz dedicated pool size in bytes
 * @dpool_high_thr dedicated pool high threshold, number of bytes the
 *                 tc allow to use from his dedicated pool before
 *                 starting to drop or back pressure will be asserted.
 * @dpool_low_thr dedicated pool low threshold, number of bytes the tc
 *                should use from his dedicated pool before stopping
 *                drops or back pressure will be de-asserted
 *                (hysteresis)
 * @spool_high_thr shared pool high threshold, number of bytes the
 *                 tc allow to use from the port's pool before
 *                 starting to drop or back pressure will be asserted
 * @spool_low_thr shared pool low threshold, number of bytes the tc
 *                should use from the port's pool in order to stop
 *                dropping or back pressure indication (hysteresis)
 * @hi_prio specify if the TC is high priority or not
 * @flow_ctrl specify if the TC support flow control or not
 */
struct rpb_tc_cfg {
	u32  dpool_sz;
	u32  dpool_high_thr;
	u32  dpool_low_thr;
	u32  spool_high_thr;
	u32  spool_low_thr;
	bool hi_prio;
	bool flow_ctrl;
};

/**
 * @brief RPB port configuration struct
 * @pool_sz port's shared pool size in bytes, shared between his TCs
 * @high_thr shared pool high watermark, number of bytes the port's
 *           TCs allow to use before starting to drop or back pressure
 *           will be asserted
 * @low_thr shared pool low watermark, number of bytes the port's TCs
 *          should use in order to stop dropping or back pressure
 *          indication (hysteresis)
 * @flow_ctrl specify if the TC support flow control or not
 * @is_mem specify if the port support memory ports or not
 * @num_of_pp_ports number of mapped PP ports
 * @tc per TC configuration
 */
struct rpb_port_cfg {
	u32 pool_sz;
	u32 high_thr;
	u32 low_thr;
	bool flow_ctrl;
	bool is_mem;
	u32 num_of_pp_ports;
	struct rpb_tc_cfg tc[PP_MAX_TC];
};

/**
 * @brief RPB configuration profile, the profile defines how much
 *        bytes out of the total size of the RPB memory will be
 *        allocated to each port, each TC.
 * @high_thr global high threshold, number of bytes all ports and TCs
 *           are allow to use from the RPB memory before starting to
 *           drop or back pressure will be asserted
 * @low_thr global low threshold, number of bytes all ports and TCs
 *          should use from the RPB memory in order to stop dropping
 *          or back pressure indication (hysteresis)
 * @pkt_high_thr packets high threshold, same as high_thr just with
 *               number packets
 * @pkt_low_thr packets low threshold, same as low_thr just for number
 *              of packets
 * @port_cfg per port configuration
 */
struct rpb_profile_cfg {
	u32  high_thr;
	u32  low_thr;
	u32  pkt_high_thr;
	u32  pkt_low_thr;
	struct rpb_port_cfg port[PP_MAX_RPB_PORT];
};

/**
 * @brief RPB TC statistics
 * @drop_bytes
 * @drop_packets
 * @dpool_bytes_used current number of bytes used by the tc from the
 *                   dedicated pool
 * @spool_bytes_used current number of bytes used by the tc from the
 *                  shared pool
 */
struct rpb_tc_stats {
	u32 drop_bytes;
	u32 drop_packets;
	u32 dpool_bytes_used;
	u32 spool_bytes_used;
};

/**
 * @brief RPB port statistics
 * @tc TCs statistics
 * @total_bytes_used Total number of bytes currently used by the port,
 *          includes all TCs shared and dedicated bytes used, 32 bit
 * @spool_bytes_used Number of bytes currently from the port's shared
 *          pool, includes all TCs shared bytes used, 32 bit
 * @rx_packets total number of packets received from port distributor
 */
struct rpb_port_stats {
	struct rpb_tc_stats tc[PP_MAX_TC];
	u32 total_bytes_used;
	u32 spool_bytes_used;
	u32 rx_packets;
};

/**
 * @brief RPB statistics
 * @pkts_rcvd_from_port_dist Total number of packets received from
 *                  port distributor
 * @total_packets_dropped Total number of packets dropped
 * @total_packets_rx_dma_recycled Total number of packets
 *        recycled by rx dma
 */
struct rpb_stats {
	u64 pkts_rcvd_from_port_dist;
	u64 total_packets_dropped;
	u64 total_packets_rx_dma_recycled;
};

/**
 * @brief RPB hw statistics
 * @port Ports statistics
 * @total_pkts_used Total number of packets currently stored in the
 *                  RPB, 32 bit
 * @total_bytes_used Total number of bytes currently stored in the
 *                  RPB, 32 bit
 */
struct rpb_hw_stats {
	struct rpb_port_stats port[PP_MAX_RPB_PORT];
	u32 total_pkts_used;
	u32 total_bytes_used;
	u32 total_packets_rx_dma_recycled;
};

/**
 * @brief Shortcut to iterate through RPB ports
 * @param p loop iterator
 */
#define RPB_FOR_EACH_PORT(p) \
	for (p = 0; p < PP_MAX_RPB_PORT; p++)

/**
 * @brief Shortcut to iterate through RPB TCs
 * @param tc loop iterator
 */
#define RPB_FOR_EACH_TC(tc) \
	for (tc = 0; tc < PP_MAX_TC; tc++)

/**
 * @brief Start RPB HW
 * @param cfg rpb init parameters
 * @return s32 0 on success, error code otherwise
 */
s32 rpb_start(struct rpb_init_param *init_param);

/**
 * @brief Read RPB configuration from RPB registers.
 * @note To set a specific configuration in the RPB one should use
 *       this APIs to read entire configuration, modify relevant
 *       fields and write set the entire configuration.
 * @param prof RPB configuration buffer to store the configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_profile_cfg_get(struct rpb_profile_cfg *prof_cfg);

/**
 * @brief Write RPB configuration profile to RPB registers.
 *
 * @param prof RPB profile
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_profile_cfg_set(const struct rpb_profile_cfg *prof_cfg);

/**
 * @brief Get RPB dma coalescing timeout
 * @param to buffer to store the timeout
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_dma_coal_timeout_get(u32 *to);

/**
 * @brief Configure RPB dma coalescing timeout
 * @param to number of micro seconds RPB should wait before
 *                exiting from dma coalescing
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_dma_coal_timeout_set(u32 to);

/**
 * @brief Read RPB dma coalescing threshold
 *
 * @param thr buffer to store the threshold
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_dma_coal_threshold_get(u32 *thr);

/**
 * @brief Configure RPB dma coalescing threshold, when RPB number of
 *        stored bytes pass this threshold the RPB exit from dma
 *        coalescing mode
 * @param thr threshold for exit from dma coalescing mode, 0 -
 *                  1023 in 1KB units, 0 for disabling dma coalescing.
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_dma_coal_threshold_set(u32 thr);

/**
 * @brief Get RPB TC statistics
 * @param port RPB port id
 * @param tc tc id
 * @param stats buffer to store the statistics
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_tc_stats_get(u32 port, u32 tc, struct rpb_tc_stats *stats);

/**
 * @brief Get RPB port statistics
 * @param port RPB port id
 * @param stats buffer to store the statistics
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_stats_get(u32 port, struct rpb_port_stats *stats);

/**
 * @brief Get RPB port back pressure indication
 * @param port RPB port id
 * @param back_pres buffer to store back pressure status, true for
 *                  xOff, false for xOn
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_port_back_pressure_status_get(u32 port, bool *back_pres);

/**
 * @brief Get RPB statistics
 * @param stats buffer to store the statistics
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_stats_get(struct rpb_stats *stats);

/**
 * @brief Get RPB memory size
 * @param sz
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_memory_sz_get(u32 *sz);

/**
 * @brief Get RPB max number of pending packets
 * @param max
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_max_pending_pkts_get(u32 *max);

/**
 * @brief Get hw RPB statistics
 * @param stats buffer to store the statistics
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_hw_stats_get(struct rpb_hw_stats *stats);

/**
 * @brief Reset RPB statistics
 * @param void
 */
void rpb_stats_reset(void);

/**
 * @brief Set a custom RPB profile
 * @note Used for debug ONLY
 * @param prof profile configuration
 * @return s32 0 on success, non-zero value on failure
 */
s32 rpb_custom_profile_set(struct rpb_profile_cfg *prof);

/**
 * @brief Set RPB profile, RPB profile defines the memory shared
 *        amount the different RPB ports and traffic classes
 * @param profile_id RPB profile id
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_profile_set(enum rpb_profile_id profile_id);

/**
 * @brief Get RPB profile, RPB profile defines the memory shared
 *        amount the different RPB ports and traffic classes
 * @param prof_id RPB profile id
 * @param prof profile configuration
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_profile_get(enum rpb_profile_id prof_id, struct rpb_profile_cfg *prof);
/**
 * @brief Get current RPB profile id
 * @param curr_id buffer to store current profile id
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_profile_id_get(enum rpb_profile_id *curr_id);

/**
 * @brief mapp pp port to rpb port according to the port type
 *        and load balancing between all ports from same type
 * @param is_mem if the port is a memory port or streaming port
 * @param flow_ctrl if the port support flow control
 * @param rpb returned rpb port id
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_pp_port_map(bool is_mem, bool flow_ctrl, u16 *rpb);

/**
 * @brief unmapp pp port from rpb port
 * @param rpb rpb port id
 * @return s32 0 on success, non-zero value otherwise
 */
s32 rpb_pp_port_unmap(u16 rpb);

/**
 * @brief Set the RPB RCU egress
 * @param enable true to enable, false to disable
 */
void rpb_rcu_egress_set(bool enable);

/**
 * @brief RPB init function
 * @param cfg rpb init parameters
 * @return s32 0 on success
 */
s32 rpb_init(struct rpb_init_param *init_param);

/**
 * RPB exit function
 * @return int 0 on success
 */
void rpb_exit(void);
#else
static inline s32 rpb_init(struct rpb_init_param *init_param)
{
	if (init_param->valid)
		return -EINVAL;
	else
		return 0;
}

static inline s32 rpb_start(struct rpb_init_param *init_param)
{
	if (init_param->valid)
		return -EINVAL;
	else
		return 0;
}

static inline void rpb_exit(void)
{
}
#endif /* CONFIG_PPV4_LGM */
#endif /* __RPB_H__ */
