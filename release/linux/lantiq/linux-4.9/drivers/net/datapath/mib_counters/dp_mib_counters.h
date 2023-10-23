// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2020 Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DP_MIB_COUNTERS
#define DP_MIB_COUNTERS

/*!
 *@brief mib_cnt_get_ethtool_stats_31 - function used by callback
 * dp_net_dev_get_ethtool_stats to retrieve counters' values specyfic for
 * gswip31
 *@param[in] dev: net device
 *@param[out] stats: for dumping NIC-specific statistics
 *@param[out] data: for dumping counters values sequence
 */
void mib_cnt_get_ethtool_stats_31(struct net_device *dev,
				  struct ethtool_stats *stats, u64 *data);

#endif
