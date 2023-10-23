#include <linux/netdevice.h>
#include <linux/if_arp.h>
#include <linux/if.h>

#include <net/ppa/ppa_api.h>

#include "ppa_stat_helper_xrx500.h"

struct rtnl_link_stats64*
ppa_stat_logical_intf_get_extended_stats64(struct net_device *dev,
									 struct rtnl_link_stats64 *storage)
{
	PPA_NETIF_ACCEL_STATS ppa_stats;

	/* NOTE : Linux driver maintains only MIPS counters */

	memset(&ppa_stats, 0, sizeof(PPA_NETIF_ACCEL_STATS));

	/* get PPA HW and SW accelerated session stats */
#if defined(PPA_IF_MIB) && PPA_IF_MIB
	ppa_get_netif_accel_stats(dev->name, &ppa_stats, (PPA_F_NETIF_HW_ACCEL | PPA_F_NETIF_SW_ACCEL), storage);
#endif
	/* add with ppa HW and SW accelerated stats */
	storage->rx_bytes += ppa_stats.hw_accel_stats.rx_bytes + ppa_stats.sw_accel_stats.rx_bytes;
	storage->tx_bytes += ppa_stats.hw_accel_stats.tx_bytes + ppa_stats.sw_accel_stats.tx_bytes;

	storage->rx_packets += ppa_stats.hw_accel_stats.rx_pkts + ppa_stats.sw_accel_stats.rx_pkts;
	storage->tx_packets += ppa_stats.hw_accel_stats.tx_pkts + ppa_stats.sw_accel_stats.tx_pkts;

	return storage;
}
