/******************************************************************************
 * Copyright (c) 2021, MaxLinear, Inc.
 *
 ******************************************************************************/

/* This file should be moved into DPM feeds later, or rewrite the inteface */
#include <linux/version.h>

#include <net/datapath_api.h>

void (*dp_dev_get_ethtool_stats_fn)(struct net_device *dev,
				    struct ethtool_stats *stats,
				    u64 *data) = NULL;
EXPORT_SYMBOL(dp_dev_get_ethtool_stats_fn);

void dp_set_ethtool_stats_fn(int inst, void (*cb)(struct net_device *dev,
			     struct ethtool_stats *stats, u64 *data))
{
	dp_dev_get_ethtool_stats_fn = cb;
}
EXPORT_SYMBOL(dp_set_ethtool_stats_fn);

int (*dp_get_dev_stat_strings_count_fn)(struct net_device *dev) = NULL;
EXPORT_SYMBOL(dp_get_dev_stat_strings_count_fn);

void dp_set_ethtool_stats_strings_cnt_fn(int inst,
					 int (*cb)(struct net_device *dev))
{
	dp_get_dev_stat_strings_count_fn = cb;
}
EXPORT_SYMBOL(dp_set_ethtool_stats_strings_cnt_fn);

void (*dp_get_dev_ss_stat_strings_fn)(struct net_device *dev,
				      u8 *data) = NULL;
EXPORT_SYMBOL(dp_get_dev_ss_stat_strings_fn);

/* Below three API will be called by PON MIB counter module */
void dp_set_ethtool_stats_strings_fn(int inst, void (*cb)(struct net_device *dev,
				     u8 *data))
{
	dp_get_dev_ss_stat_strings_fn = cb;
}
EXPORT_SYMBOL(dp_set_ethtool_stats_strings_fn);


/* Below three API will be called by ethernet driver or VUNI driver */

/* ethtool statistics support */
void dp_net_dev_get_ss_stat_strings(struct net_device *dev, u8 *data)
{
	/* This is for the Mib counter wraparound module */
	if (dp_get_dev_ss_stat_strings_fn)
		dp_get_dev_ss_stat_strings_fn(dev, data);
}
EXPORT_SYMBOL(dp_net_dev_get_ss_stat_strings);

int dp_net_dev_get_ss_stat_strings_count(struct net_device *dev)
{
	/* This is for the Mib counter wraparound module */
	if (dp_get_dev_stat_strings_count_fn)
		return dp_get_dev_stat_strings_count_fn(dev);

	return 0;
}
EXPORT_SYMBOL(dp_net_dev_get_ss_stat_strings_count);

void dp_net_dev_get_ethtool_stats(struct net_device *dev,
				  struct ethtool_stats *stats, u64 *data)
{
	/* This is for the Mib counter wraparound module */
	if (dp_dev_get_ethtool_stats_fn)
		return dp_dev_get_ethtool_stats_fn(dev, stats, data);
}
EXPORT_SYMBOL(dp_net_dev_get_ethtool_stats);


