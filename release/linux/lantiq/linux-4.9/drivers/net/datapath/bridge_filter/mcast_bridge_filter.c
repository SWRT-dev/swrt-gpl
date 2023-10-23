// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 MaxLinear, Inc.
 * Copyright (c) 2018 Intel Corporation
 *
 ******************************************************************************/
/* This intercepts all packets transferred on any bridge in the system and
 * drops the IPv4 IGMP and the IPv6 MLD packets.
 */
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <net/addrconf.h>
#include <net/mcast_bridge_filter.h>

/* max number of encapsulated vlan tags */
#define MAX_VLAN_TAGS	3

static bool dp_mcast_fliter_active;

void dp_mcast_filter_active(bool active)
{
	dp_mcast_fliter_active = active;
}
EXPORT_SYMBOL(dp_mcast_filter_active);

/* Iterates over the VLAN tags and increases nhoff for each VLAN tag.
 * returns 0 on success and -EINVAL in case of an parsing error.
 */
static int dp_mcast_vlan_iter(struct sk_buff *skb, __be16 *proto, int *nhoff)
{
	const struct vlan_hdr *vlan = NULL;
	struct vlan_hdr _vlan;
	int i;

	for (i = 0; i < MAX_VLAN_TAGS; i++) {
		vlan = skb_header_pointer(skb, *nhoff, sizeof(_vlan), &_vlan);
		if (!vlan)
			return -EINVAL;

		*proto = vlan->h_vlan_encapsulated_proto;
		*nhoff += sizeof(*vlan);
		if (!eth_type_vlan(*proto))
			break;
	}

	return 0;
}

static unsigned int dp_mcast_br_hook_ipv4(struct sk_buff *skb, int nhoff)
{
	struct iphdr *iph;

	if (!dp_mcast_fliter_active)
		return NF_ACCEPT;

	if (!pskb_network_may_pull(skb, nhoff + sizeof(struct iphdr)))
		return NF_ACCEPT;

	iph = (struct iphdr *)(skb_network_header(skb) + nhoff);
	if (!iph || iph->ihl * 4 < sizeof(struct iphdr)) {
		pr_debug("IPv4 packet too short, accepted");
		return NF_ACCEPT;
	}

	if (iph->protocol == IPPROTO_IGMP) {
		pr_debug("IPv4 IGMP packet dropped");
		return NF_DROP;
	}
	return NF_ACCEPT;
}

static unsigned int dp_mcast_br_hook_ipv6(struct sk_buff *skb, int nhoff)
{
	const struct ipv6hdr *iph;
	struct ipv6hdr _iph;
	__u8 nexthdr;
	__be16 frag_off;

	iph = skb_header_pointer(skb, nhoff, sizeof(_iph), &_iph);
	if (!iph) {
		pr_debug("IPv6 packet too short");
		return NF_ACCEPT;
	}

	nexthdr = iph->nexthdr;
	nhoff = ipv6_skip_exthdr(skb, nhoff + sizeof(_iph), &nexthdr,
				 &frag_off);
	if (nhoff < 0) {
		pr_debug("IPv6 packet too short, accepted");
		return NF_ACCEPT;
	}

	if (ipv6_is_mld(skb, nexthdr, nhoff) && dp_mcast_fliter_active) {
		pr_debug("IPv6 MLD packet dropped");
		return NF_DROP;
	}
	return NF_ACCEPT;
}

static unsigned int dp_mcast_br_hook(void *priv, struct sk_buff *skb,
				     const struct nf_hook_state *state)
{
	__be16 proto = skb->protocol;
	int nhoff = skb_network_offset(skb);
	int err;

	if (eth_type_vlan(proto)) {
		err = dp_mcast_vlan_iter(skb, &proto, &nhoff);
		if (err) {
			pr_debug("VLAN header broken, accepted");
			return NF_ACCEPT;
		}
	}

	switch (proto) {
	case htons(ETH_P_IP):
		return dp_mcast_br_hook_ipv4(skb, nhoff);
	case htons(ETH_P_IPV6):
		return dp_mcast_br_hook_ipv6(skb, nhoff);
	default:
		return NF_ACCEPT;
	}
}

static struct nf_hook_ops dp_mcast_ops = {
	.hook = dp_mcast_br_hook,
	.hooknum = NF_BR_PRE_ROUTING,
	.pf = NFPROTO_BRIDGE,
	.priority = NF_BR_PRI_BRNF,
};

static int __init dp_mcast_init(void)
{
	return nf_register_hook(&dp_mcast_ops);
}

static void __exit dp_mcast_fini(void)
{
	nf_unregister_hook(&dp_mcast_ops);
}
module_init(dp_mcast_init);
module_exit(dp_mcast_fini);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("PON multicast filter Driver");
