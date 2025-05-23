// SPDX-License-Identifier: GPL-2.0-or-later
/*
 *	Device handling code
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 */

#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/netpoll.h>
#include <linux/etherdevice.h>
#include <linux/ethtool.h>
#include <linux/list.h>
#include <linux/netfilter_bridge.h>
#if IS_ENABLED(CONFIG_NF_FLOW_TABLE)
#include <linux/netfilter.h>
#include <net/netfilter/nf_flow_table.h>
#endif

#include <linux/uaccess.h>
#include "br_private.h"

#define COMMON_FEATURES (NETIF_F_SG | NETIF_F_FRAGLIST | NETIF_F_HIGHDMA | \
			 NETIF_F_GSO_MASK | NETIF_F_HW_CSUM)

const struct nf_br_ops __rcu *nf_br_ops __read_mostly;
EXPORT_SYMBOL_GPL(nf_br_ops);

/* net device transmit always called with BH disabled */
netdev_tx_t br_dev_xmit(struct sk_buff *skb, struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_fdb_entry *dst;
	struct net_bridge_mdb_entry *mdst;
	struct pcpu_sw_netstats *brstats = this_cpu_ptr(br->stats);
	const struct nf_br_ops *nf_ops;
	const unsigned char *dest;
	u16 vid = 0;

	memset(skb->cb, 0, sizeof(struct br_input_skb_cb));

	memset(skb->cb, 0, sizeof(struct br_input_skb_cb));

	rcu_read_lock();
	nf_ops = rcu_dereference(nf_br_ops);
	if (nf_ops && nf_ops->br_dev_xmit_hook(skb)) {
		rcu_read_unlock();
		return NETDEV_TX_OK;
	}

	u64_stats_update_begin(&brstats->syncp);
	brstats->tx_packets++;
	brstats->tx_bytes += skb->len;
	u64_stats_update_end(&brstats->syncp);

	br_switchdev_frame_unmark(skb);
	BR_INPUT_SKB_CB(skb)->brdev = dev;
	BR_INPUT_SKB_CB(skb)->frag_max_size = 0;

	skb_reset_mac_header(skb);
	skb_pull(skb, ETH_HLEN);

	if (!br_allowed_ingress(br, br_vlan_group_rcu(br), skb, &vid))
		goto out;

	if (IS_ENABLED(CONFIG_INET) &&
	    (eth_hdr(skb)->h_proto == htons(ETH_P_ARP) ||
	     eth_hdr(skb)->h_proto == htons(ETH_P_RARP)) &&
	    br_opt_get(br, BROPT_NEIGH_SUPPRESS_ENABLED)) {
		br_do_proxy_suppress_arp(skb, br, vid, NULL);
	} else if (IS_ENABLED(CONFIG_IPV6) &&
		   skb->protocol == htons(ETH_P_IPV6) &&
		   br_opt_get(br, BROPT_NEIGH_SUPPRESS_ENABLED) &&
		   pskb_may_pull(skb, sizeof(struct ipv6hdr) +
				 sizeof(struct nd_msg)) &&
		   ipv6_hdr(skb)->nexthdr == IPPROTO_ICMPV6) {
			struct nd_msg *msg, _msg;

			msg = br_is_nd_neigh_msg(skb, &_msg);
			if (msg)
				br_do_suppress_nd(skb, br, vid, NULL, msg);
	}

	dest = eth_hdr(skb)->h_dest;
	if (is_broadcast_ether_addr(dest)) {
		br_flood(br, skb, BR_PKT_BROADCAST, false, true, false);
	} else if (is_multicast_ether_addr(dest)) {
		if (unlikely(netpoll_tx_running(dev))) {
			br_flood(br, skb, BR_PKT_MULTICAST, false, true, false);
			goto out;
		}
		if (br_multicast_rcv(br, NULL, skb, vid)) {
			kfree_skb(skb);
			goto out;
		}

		mdst = br_mdb_get(br, skb, vid);
		if ((mdst || BR_INPUT_SKB_CB_MROUTERS_ONLY(skb)) &&
		    br_multicast_querier_exists(br, eth_hdr(skb)))
			br_multicast_flood(mdst, skb, false, true);
		else
			br_flood(br, skb, BR_PKT_MULTICAST, false, true, false);
	} else if ((dst = br_fdb_find_rcu(br, dest, vid)) != NULL) {
		br_forward(dst->dst, skb, false, true);
	} else {
		br_flood(br, skb, BR_PKT_UNICAST, false, true, true);
	}
out:
	rcu_read_unlock();
	return NETDEV_TX_OK;
}

static struct lock_class_key bridge_netdev_addr_lock_key;

static void br_set_lockdep_class(struct net_device *dev)
{
	lockdep_set_class(&dev->addr_list_lock, &bridge_netdev_addr_lock_key);
}

static int br_dev_init(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);
	int err;

	br->stats = netdev_alloc_pcpu_stats(struct pcpu_sw_netstats);
	if (!br->stats)
		return -ENOMEM;

	err = br_fdb_hash_init(br);
	if (err) {
		free_percpu(br->stats);
		return err;
	}

	err = br_mdb_hash_init(br);
	if (err) {
		free_percpu(br->stats);
		br_fdb_hash_fini(br);
		return err;
	}

	err = br_vlan_init(br);
	if (err) {
		free_percpu(br->stats);
		br_mdb_hash_fini(br);
		br_fdb_hash_fini(br);
		return err;
	}

	err = br_multicast_init_stats(br);
	if (err) {
		free_percpu(br->stats);
		br_vlan_flush(br);
		br_mdb_hash_fini(br);
		br_fdb_hash_fini(br);
	}

	br_set_lockdep_class(dev);
	return err;
}

static void br_dev_uninit(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	br_multicast_dev_del(br);
	br_multicast_uninit_stats(br);
	br_vlan_flush(br);
	br_mdb_hash_fini(br);
	br_fdb_hash_fini(br);
	free_percpu(br->stats);
}

static int br_dev_open(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	netdev_update_features(dev);
	netif_start_queue(dev);
	br_stp_enable_bridge(br);
	br_multicast_open(br);

	if (br_opt_get(br, BROPT_MULTICAST_ENABLED))
		br_multicast_join_snoopers(br);

	return 0;
}

static void br_dev_set_multicast_list(struct net_device *dev)
{
}

static void br_dev_change_rx_flags(struct net_device *dev, int change)
{
	if (change & IFF_PROMISC)
		br_manage_promisc(netdev_priv(dev));
}

static int br_dev_stop(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	br_stp_disable_bridge(br);
	br_multicast_stop(br);

	if (br_opt_get(br, BROPT_MULTICAST_ENABLED))
		br_multicast_leave_snoopers(br);

	netif_stop_queue(dev);

	return 0;
}

static void br_get_stats64(struct net_device *dev,
			   struct rtnl_link_stats64 *stats)
{
	struct net_bridge *br = netdev_priv(dev);
	struct pcpu_sw_netstats tmp, sum = { 0 };
	unsigned int cpu;

	for_each_possible_cpu(cpu) {
		unsigned int start;
		const struct pcpu_sw_netstats *bstats
			= per_cpu_ptr(br->stats, cpu);
		do {
			start = u64_stats_fetch_begin_irq(&bstats->syncp);
			memcpy(&tmp, bstats, sizeof(tmp));
		} while (u64_stats_fetch_retry_irq(&bstats->syncp, start));
		sum.tx_bytes   += tmp.tx_bytes;
		sum.tx_packets += tmp.tx_packets;
		sum.rx_bytes   += tmp.rx_bytes;
		sum.rx_packets += tmp.rx_packets;
	}

	netdev_stats_to_stats64(stats, &dev->stats);
	stats->tx_bytes   = sum.tx_bytes;
	stats->tx_packets = sum.tx_packets;
	stats->rx_bytes   = sum.rx_bytes;
	stats->rx_packets = sum.rx_packets;
}

static int br_change_mtu(struct net_device *dev, int new_mtu)
{
	struct net_bridge *br = netdev_priv(dev);

	dev->mtu = new_mtu;

	/* this flag will be cleared if the MTU was automatically adjusted */
	br_opt_toggle(br, BROPT_MTU_SET_BY_USER, true);
#if IS_ENABLED(CONFIG_BRIDGE_NETFILTER)
	/* remember the MTU in the rtable for PMTU */
	dst_metric_set(&br->fake_rtable.dst, RTAX_MTU, new_mtu);
#endif

	return 0;
}

/* Allow setting mac address to any valid ethernet address. */
static int br_set_mac_address(struct net_device *dev, void *p)
{
	struct net_bridge *br = netdev_priv(dev);
	struct sockaddr *addr = p;

	if (!is_valid_ether_addr(addr->sa_data))
		return -EADDRNOTAVAIL;

	/* dev_set_mac_addr() can be called by a master device on bridge's
	 * NETDEV_UNREGISTER, but since it's being destroyed do nothing
	 */
	if (dev->reg_state != NETREG_REGISTERED)
		return -EBUSY;

	spin_lock_bh(&br->lock);
	if (!ether_addr_equal(dev->dev_addr, addr->sa_data)) {
		/* Mac address will be changed in br_stp_change_bridge_id(). */
		br_stp_change_bridge_id(br, addr->sa_data);
	}
	spin_unlock_bh(&br->lock);

	return 0;
}

static void br_getinfo(struct net_device *dev, struct ethtool_drvinfo *info)
{
	strlcpy(info->driver, "bridge", sizeof(info->driver));
	strlcpy(info->version, BR_VERSION, sizeof(info->version));
	strlcpy(info->fw_version, "N/A", sizeof(info->fw_version));
	strlcpy(info->bus_info, "N/A", sizeof(info->bus_info));
}

static netdev_features_t br_fix_features(struct net_device *dev,
	netdev_features_t features)
{
	struct net_bridge *br = netdev_priv(dev);

	return br_features_recompute(br, features);
}

#ifdef CONFIG_NET_POLL_CONTROLLER
static void br_poll_controller(struct net_device *br_dev)
{
}

static void br_netpoll_cleanup(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_port *p;

	list_for_each_entry(p, &br->port_list, list)
		br_netpoll_disable(p);
}

static int __br_netpoll_enable(struct net_bridge_port *p)
{
	struct netpoll *np;
	int err;

	np = kzalloc(sizeof(*p->np), GFP_KERNEL);
	if (!np)
		return -ENOMEM;

	err = __netpoll_setup(np, p->dev);
	if (err) {
		kfree(np);
		return err;
	}

	p->np = np;
	return err;
}

int br_netpoll_enable(struct net_bridge_port *p)
{
	if (!p->br->dev->npinfo)
		return 0;

	return __br_netpoll_enable(p);
}

static int br_netpoll_setup(struct net_device *dev, struct netpoll_info *ni)
{
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_port *p;
	int err = 0;

	list_for_each_entry(p, &br->port_list, list) {
		if (!p->dev)
			continue;
		err = __br_netpoll_enable(p);
		if (err)
			goto fail;
	}

out:
	return err;

fail:
	br_netpoll_cleanup(dev);
	goto out;
}

void br_netpoll_disable(struct net_bridge_port *p)
{
	struct netpoll *np = p->np;

	if (!np)
		return;

	p->np = NULL;

	__netpoll_free(np);
}

#endif

static int br_add_slave(struct net_device *dev, struct net_device *slave_dev,
			struct netlink_ext_ack *extack)

{
	struct net_bridge *br = netdev_priv(dev);

	return br_add_if(br, slave_dev, extack);
}

static int br_del_slave(struct net_device *dev, struct net_device *slave_dev)
{
	struct net_bridge *br = netdev_priv(dev);

	return br_del_if(br, slave_dev);
}

static const struct ethtool_ops br_ethtool_ops = {
	.get_drvinfo    = br_getinfo,
	.get_link	= ethtool_op_get_link,
};

#if IS_ENABLED(CONFIG_NF_FLOW_TABLE)
static int br_flow_offload_check(struct flow_offload_hw_path *path)
{
	struct net_device *dev = path->dev;
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_fdb_entry *dst;

	if (!(path->flags & FLOW_OFFLOAD_PATH_ETHERNET))
		return -EINVAL;

	dst = br_fdb_find_rcu(br, path->eth_dest, path->vlan_id);
	if (!dst || !dst->dst)
		return -ENOENT;

	path->dev = dst->dst->dev;
	if (path->dev->netdev_ops->ndo_flow_offload_check)
		return path->dev->netdev_ops->ndo_flow_offload_check(path);

	return 0;
}
#endif /* CONFIG_NF_FLOW_TABLE */

static const struct net_device_ops br_netdev_ops = {
	.ndo_open		 = br_dev_open,
	.ndo_stop		 = br_dev_stop,
	.ndo_init		 = br_dev_init,
	.ndo_uninit		 = br_dev_uninit,
	.ndo_start_xmit		 = br_dev_xmit,
	.ndo_get_stats64	 = br_get_stats64,
	.ndo_set_mac_address	 = br_set_mac_address,
	.ndo_set_rx_mode	 = br_dev_set_multicast_list,
	.ndo_change_rx_flags	 = br_dev_change_rx_flags,
	.ndo_change_mtu		 = br_change_mtu,
	.ndo_do_ioctl		 = br_dev_ioctl,
#ifdef CONFIG_NET_POLL_CONTROLLER
	.ndo_netpoll_setup	 = br_netpoll_setup,
	.ndo_netpoll_cleanup	 = br_netpoll_cleanup,
	.ndo_poll_controller	 = br_poll_controller,
#endif
	.ndo_add_slave		 = br_add_slave,
	.ndo_del_slave		 = br_del_slave,
	.ndo_fix_features        = br_fix_features,
	.ndo_fdb_add		 = br_fdb_add,
	.ndo_fdb_del		 = br_fdb_delete,
	.ndo_fdb_dump		 = br_fdb_dump,
	.ndo_fdb_get		 = br_fdb_get,
	.ndo_bridge_getlink	 = br_getlink,
	.ndo_bridge_setlink	 = br_setlink,
	.ndo_bridge_dellink	 = br_dellink,
	.ndo_features_check	 = passthru_features_check,
#if IS_ENABLED(CONFIG_NF_FLOW_TABLE)
	.ndo_flow_offload_check	 = br_flow_offload_check,
#endif
};

static struct device_type br_type = {
	.name	= "bridge",
};

void br_dev_setup(struct net_device *dev)
{
	struct net_bridge *br = netdev_priv(dev);

	eth_hw_addr_random(dev);
	ether_setup(dev);

	dev->netdev_ops = &br_netdev_ops;
	dev->needs_free_netdev = true;
	dev->ethtool_ops = &br_ethtool_ops;
	SET_NETDEV_DEVTYPE(dev, &br_type);
	dev->priv_flags = IFF_EBRIDGE | IFF_NO_QUEUE;

	dev->features = COMMON_FEATURES | NETIF_F_LLTX | NETIF_F_NETNS_LOCAL |
			NETIF_F_HW_VLAN_CTAG_TX | NETIF_F_HW_VLAN_STAG_TX;
	dev->hw_features = COMMON_FEATURES | NETIF_F_HW_VLAN_CTAG_TX |
			   NETIF_F_HW_VLAN_STAG_TX;
	dev->vlan_features = COMMON_FEATURES;

	br->dev = dev;
	spin_lock_init(&br->lock);
	INIT_LIST_HEAD(&br->port_list);
	INIT_HLIST_HEAD(&br->fdb_list);
	spin_lock_init(&br->hash_lock);

	br->bridge_id.prio[0] = 0x80;
	br->bridge_id.prio[1] = 0x00;

	ether_addr_copy(br->group_addr, eth_stp_addr);

	br->stp_enabled = BR_NO_STP;
	br->group_fwd_mask = BR_GROUPFWD_DEFAULT;
	br->group_fwd_mask_required = BR_GROUPFWD_DEFAULT;

	br->designated_root = br->bridge_id;
	br->bridge_max_age = br->max_age = 20 * HZ;
	br->bridge_hello_time = br->hello_time = 2 * HZ;
	br->bridge_forward_delay = br->forward_delay = 15 * HZ;
	br->bridge_ageing_time = br->ageing_time = BR_DEFAULT_AGEING_TIME;
	dev->max_mtu = ETH_MAX_MTU;

	br_netfilter_rtable_init(br);
	br_stp_timer_init(br);
	br_multicast_init(br);
	INIT_DELAYED_WORK(&br->gc_work, br_fdb_cleanup);
}
