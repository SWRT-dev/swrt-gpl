/*
 *	Forwarding decision
 *	Linux ethernet bridge
 *
 *	Authors:
 *	Lennert Buytenhek		<buytenh@gnu.org>
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 */

#include <linux/err.h>
#include <linux/slab.h>
#include <linux/kernel.h>
#include <linux/netdevice.h>
#include <linux/netpoll.h>
#include <linux/skbuff.h>
#include <linux/if_vlan.h>
#include <linux/netfilter_bridge.h>
#include "br_private.h"

#if IS_ENABLED(CONFIG_MCAST_HELPER)
#include <net/ip.h>
#include <linux/mroute6.h>

void (*five_tuple_br_info_ptr)(struct sk_buff *skb) = NULL;
void (*five_tuple_br_info_hook)(struct sk_buff *skb) = NULL;
EXPORT_SYMBOL(five_tuple_br_info_ptr);
EXPORT_SYMBOL(five_tuple_br_info_hook);
atomic_t mch_br_capture_pkt = ATOMIC_INIT(0);
EXPORT_SYMBOL(mch_br_capture_pkt);

/* Called only from multicast flood path */
static void mcast_helper_learning_hook(struct sk_buff *skb)
{
	struct dst_entry *dst = skb_dst(skb);

	/* Send five tuple info to mcast helper */
	if (five_tuple_br_info_ptr && atomic_read(&mch_br_capture_pkt)) {
		if ((skb->protocol == htons(ETH_P_IP) &&
		    (ip_hdr(skb)->protocol == IPPROTO_UDP)) ||
		    ((skb->protocol == htons(ETH_P_IPV6)) &&
		    (ipv6_hdr(skb)->nexthdr == IPPROTO_UDP))) {
			five_tuple_br_info_ptr(skb);
		}
	}

#if IS_ENABLED(CONFIG_MCAST_SNOOPING)
	/* Hook the UDP multicast to mcast helper */
	if (!bridge_lanserver_hook)
		return;
#endif

	if (five_tuple_br_info_hook == NULL)
		return;

	if (((skb->protocol == htons(ETH_P_IP)) &&
	    (ip_hdr(skb)->protocol == IPPROTO_UDP)) ||
	    ((skb->protocol == htons(ETH_P_IPV6)) &&
	    (ipv6_hdr(skb)->nexthdr == IPPROTO_UDP))) {
		if (!(dst && ((dst->input == ip_mr_input) ||
		    (dst->input == ip6_mc_input)))) {
			five_tuple_br_info_hook(skb);
		}
	}
}
#endif

/* Don't forward packets to originating port or forwarding disabled */
static inline int should_deliver(const struct net_bridge_port *p,
				 const struct sk_buff *skb)
{
	struct net_bridge_vlan_group *vg;

	vg = nbp_vlan_group_rcu(p);
	return ((p->flags & BR_HAIRPIN_MODE) || skb->dev != p->dev) &&
		br_allowed_egress(vg, skb) && p->state == BR_STATE_FORWARDING &&
		nbp_switchdev_allowed_egress(p, skb) &&
		!br_skb_isolated(p, skb);
}

int br_dev_queue_push_xmit(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	skb_push(skb, ETH_HLEN);
	if (!is_skb_forwardable(skb->dev, skb))
		goto drop;

	br_drop_fake_rtable(skb);

	if (skb->ip_summed == CHECKSUM_PARTIAL &&
	    (skb->protocol == htons(ETH_P_8021Q) ||
	     skb->protocol == htons(ETH_P_8021AD))) {
		int depth;

		if (!__vlan_get_protocol(skb, skb->protocol, &depth))
			goto drop;

		skb_set_network_header(skb, depth);
	}

	dev_queue_xmit(skb);

	return 0;

drop:
	kfree_skb(skb);
	return 0;
}
EXPORT_SYMBOL_GPL(br_dev_queue_push_xmit);

int br_forward_finish(struct net *net, struct sock *sk, struct sk_buff *skb)
{
	return NF_HOOK(NFPROTO_BRIDGE, NF_BR_POST_ROUTING,
		       net, sk, skb, NULL, skb->dev,
		       br_dev_queue_push_xmit);

}
EXPORT_SYMBOL_GPL(br_forward_finish);

static void __br_forward(const struct net_bridge_port *to,
			 struct sk_buff *skb, bool local_orig)
{
	struct net_bridge_vlan_group *vg;
	struct net_device *indev;
	struct net *net;
	int br_hook;

	vg = nbp_vlan_group_rcu(to);
	skb = br_handle_vlan(to->br, vg, skb);
	if (!skb)
		return;

	indev = skb->dev;
	skb->dev = to->dev;
	if (!local_orig) {
		if (skb_warn_if_lro(skb)) {
			kfree_skb(skb);
			return;
		}
		br_hook = NF_BR_FORWARD;
		skb_forward_csum(skb);
		net = dev_net(indev);
	} else {
		if (unlikely(netpoll_tx_running(to->br->dev))) {
			skb_push(skb, ETH_HLEN);
			if (!is_skb_forwardable(skb->dev, skb))
				kfree_skb(skb);
			else
				br_netpoll_send_skb(to, skb);
			return;
		}
		br_hook = NF_BR_LOCAL_OUT;
		net = dev_net(skb->dev);
		indev = NULL;
	}

	NF_HOOK(NFPROTO_BRIDGE, br_hook,
		net, NULL, skb, indev, skb->dev,
		br_forward_finish);
}

static int deliver_clone(const struct net_bridge_port *prev,
			 struct sk_buff *skb, bool local_orig)
{
	struct net_device *dev = BR_INPUT_SKB_CB(skb)->brdev;

	skb = skb_clone(skb, GFP_ATOMIC);
	if (!skb) {
		dev->stats.tx_dropped++;
		return -ENOMEM;
	}

	__br_forward(prev, skb, local_orig);
	return 0;
}

/**
 * br_forward - forward a packet to a specific port
 * @to: destination port
 * @skb: packet being forwarded
 * @local_rcv: packet will be received locally after forwarding
 * @local_orig: packet is locally originated
 *
 * Should be called with rcu_read_lock.
 */
void br_forward(const struct net_bridge_port *to,
		struct sk_buff *skb, bool local_rcv, bool local_orig)
{
	if (to->flags & BR_ISOLATED)
		to = NULL;

	if (to && should_deliver(to, skb)) {
		if (local_rcv)
			deliver_clone(to, skb, local_orig);
		else
			__br_forward(to, skb, local_orig);
		return;
	}

	if (!local_rcv)
		kfree_skb(skb);
}
EXPORT_SYMBOL_GPL(br_forward);

static struct net_bridge_port *maybe_deliver(
	struct net_bridge_port *prev, struct net_bridge_port *p,
	struct sk_buff *skb, bool local_orig)
{
	int err;

	if (!should_deliver(p, skb))
		return prev;

	if (!prev)
		goto out;

#if IS_ENABLED(CONFIG_MCAST_SNOOPING)
	if ((bridge_igmp_snooping || bridge_mld_snooping) &&
	    is_multicast_ether_addr(eth_hdr(skb)->h_dest) &&
	    (br_selective_flood(prev, skb) == 0)) {
		prev = p;
		return p;
	}
#endif

	err = deliver_clone(prev, skb, local_orig);
	if (err)
		return ERR_PTR(err);

out:
	return p;
}

static void maybe_deliver_addr(struct net_bridge_port *p, struct sk_buff *skb,
			       const unsigned char *addr, bool local_orig)
{
	struct net_device *dev = BR_INPUT_SKB_CB(skb)->brdev;
	const unsigned char *src = eth_hdr(skb)->h_source;

	if (!should_deliver(p, skb))
		return;

	/* Even with hairpin, no soliloquies - prevent breaking IPv6 DAD */
	if (skb->dev == p->dev && ether_addr_equal(src, addr))
		return;

	skb = skb_copy(skb, GFP_ATOMIC);
	if (!skb) {
		dev->stats.tx_dropped++;
		return;
	}

	memcpy(eth_hdr(skb)->h_dest, addr, ETH_ALEN);
	__br_forward(p, skb, local_orig);
}

/* called under rcu_read_lock */
void br_flood(struct net_bridge *br, struct sk_buff *skb,
	      enum br_pkt_type pkt_type, bool local_rcv, bool local_orig)
{
	u8 igmp_type = br_multicast_igmp_type(skb);
	struct net_bridge_port *prev = NULL;
	struct net_bridge_port *p;

#if IS_ENABLED(CONFIG_MCAST_SNOOPING)
#if IS_ENABLED(CONFIG_MCAST_HELPER)
	if (is_multicast_ether_addr(eth_hdr(skb)->h_dest) &&
	    !is_broadcast_ether_addr(eth_hdr(skb)->h_dest))
		mcast_helper_learning_hook(skb);
#endif
#endif

	list_for_each_entry_rcu(p, &br->port_list, list) {
		if (!local_orig && (p->flags & BR_ISOLATED))
			continue;
		/* Do not flood unicast traffic to ports that turn it off */
		if (pkt_type == BR_PKT_UNICAST && !(p->flags & BR_FLOOD))
			continue;
		/* Do not flood if mc off, except for traffic we originate */
		if (pkt_type == BR_PKT_MULTICAST &&
		    !(p->flags & BR_MCAST_FLOOD) && skb->dev != br->dev)
			continue;

		/* Do not flood to ports that enable proxy ARP */
		if (p->flags & BR_PROXYARP)
			continue;
		if ((p->flags & BR_PROXYARP_WIFI) &&
		    BR_INPUT_SKB_CB(skb)->proxyarp_replied)
			continue;

		prev = maybe_deliver(prev, p, skb, local_orig);
		if (IS_ERR(prev))
			goto out;
		if (prev == p)
			br_multicast_count(p->br, p, skb, igmp_type,
					   BR_MCAST_DIR_TX);
	}

	if (!prev)
		goto out;

	if (local_rcv) {
#if IS_ENABLED(CONFIG_MCAST_SNOOPING)
	if ((bridge_igmp_snooping || bridge_mld_snooping) &&
	    is_multicast_ether_addr(eth_hdr(skb)->h_dest) &&
	    (br_selective_flood(prev, skb) == 0)) {}
	else
#endif
		deliver_clone(prev, skb, local_orig);
	} else {
#if IS_ENABLED(CONFIG_MCAST_SNOOPING)
	if ((bridge_igmp_snooping || bridge_mld_snooping) &&
	    is_multicast_ether_addr(eth_hdr(skb)->h_dest) &&
	    (br_selective_flood(prev, skb) == 0))
		kfree_skb(skb);
	else
#endif
		__br_forward(prev, skb, local_orig);
	}
	return;

out:
	if (!local_rcv)
		kfree_skb(skb);
}

#ifdef CONFIG_BRIDGE_IGMP_SNOOPING
/* called with rcu_read_lock */
void br_multicast_flood(struct net_bridge_mdb_entry *mdst,
			struct sk_buff *skb,
			bool local_rcv, bool local_orig)
{
	struct net_device *dev = BR_INPUT_SKB_CB(skb)->brdev;
	u8 igmp_type = br_multicast_igmp_type(skb);
	struct net_bridge *br = netdev_priv(dev);
	struct net_bridge_port *prev = NULL;
	struct net_bridge_port_group *p;
	struct hlist_node *rp;

#if IS_ENABLED(CONFIG_MCAST_HELPER)
	mcast_helper_learning_hook(skb);
#endif

	rp = rcu_dereference(hlist_first_rcu(&br->router_list));
	p = mdst ? rcu_dereference(mdst->ports) : NULL;
	while (p || rp) {
		struct net_bridge_port *port, *lport, *rport;

		lport = p ? p->port : NULL;
		rport = rp ? hlist_entry(rp, struct net_bridge_port, rlist) :
			     NULL;

		if ((unsigned long)lport > (unsigned long)rport) {
			port = lport;

			if (p->flags & MDB_PG_FLAGS_MCAST_TO_UCAST) {
				maybe_deliver_addr(lport, skb, p->eth_addr,
						   local_orig);
				goto delivered;
			}
		} else {
			port = rport;
		}

		prev = maybe_deliver(prev, port, skb, local_orig);
delivered:
		if (IS_ERR(prev))
			goto out;
		if (prev == port)
			br_multicast_count(port->br, port, skb, igmp_type,
					   BR_MCAST_DIR_TX);

		if ((unsigned long)lport >= (unsigned long)port)
			p = rcu_dereference(p->next);
		if ((unsigned long)rport >= (unsigned long)port)
			rp = rcu_dereference(hlist_next_rcu(rp));
	}

	if (!prev)
		goto out;

	if (local_rcv)
		deliver_clone(prev, skb, local_orig);
	else
		__br_forward(prev, skb, local_orig);
	return;

out:
	if (!local_rcv)
		kfree_skb(skb);
}
#endif
