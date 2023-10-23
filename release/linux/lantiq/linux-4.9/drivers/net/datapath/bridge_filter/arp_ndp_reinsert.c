// SPDX-License-Identifier: BSD-3-Clause OR GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 MaxLinear, Inc.
 * Copyright (c) 2018 Intel Corporation
 *
 ******************************************************************************/
/* This intercepts all packets transferred on any bridge in the system and try
 * to reinsert all the ARP and NDP packets into the GSWIP again.
 */
#include <linux/kernel.h>
#include <linux/netfilter.h>
#include <linux/netfilter_ipv4.h>
#include <linux/netfilter_bridge.h>
#include <linux/if_vlan.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/skbuff.h>
#include <net/addrconf.h>
#include <net/ndisc.h>
#include <net/datapath_api.h>

/* max number of encapsulated vlan tags */
#define MAX_VLAN_TAGS	3

/* Reinsert the packet again into the GSWIP Datapath. This way it will flow
 * through the GSWIP again, but GSWIP will not trap the packet in the PCE rules.
 * The GSWIP will apply all VLAN forwarding or drop rules like it does for
 * other packets.
 */
static int arp_ndp_reinsert_packet(struct sk_buff *skb)
{
	struct dma_tx_desc_0 *dw0;
	struct dma_tx_desc_1 *dw1;
	struct sk_buff *nskb;
	dp_subif_t subif = {0};
	int ret;

	ret = dp_get_netif_subifid(skb->dev, NULL, NULL, NULL, &subif, 0);
	if (ret != DP_SUCCESS) {
		netdev_err(skb->dev, "%s: can not get subif: %d\n", __func__,
			   ret);
		return -EIO;
	}

	nskb = skb_clone(skb, GFP_ATOMIC);
	if (!nskb)
		return -ENOMEM;

	/* skb->data already points to the beginning of the IP header,
	 * move it back to the MAC header.
	 */
	skb_push(nskb, nskb->data - skb_mac_header(nskb));

	nskb = vlan_hwaccel_push_inside(nskb);
	if (WARN_ON(!nskb)) {
		dev_kfree_skb_any(nskb);
		return -ENOMEM;
	}

	dw0 = (struct dma_tx_desc_0 *)&nskb->DW0;
	dw0->field.dest_sub_if_id = subif.subif;
	dw1 = (struct dma_tx_desc_1 *)&nskb->DW1;
	dw1->field.ep = subif.port_id;

	ret = dp_xmit(skb->dev, &subif, nskb, nskb->len, DP_TX_INSERT);
	if (ret) {
		/* DPM can return busy on LGM then skb needs to be freed */
		if (ret == DP_TX_FN_BUSY)
			dev_kfree_skb_any(nskb);
		netdev_err(skb->dev, "%s: insert fail\n", __func__);
		return -EIO;
	}
	return 0;
}

/* Iterates over the VLAN tags and increases nhoff for each VLAN tag.
 * returns 0 on success and -EINVAL in case of an parsing error.
 */
static int arp_ndp_reinsert_vlan_iter(struct sk_buff *skb, __be16 *proto,
				      int *nhoff)
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

static unsigned int arp_ndp_reinsert_hook_arp(struct sk_buff *skb, int nhoff)
{
	int ret;

	ret = arp_ndp_reinsert_packet(skb);
	if (ret) {
		netdev_err(skb->dev, "%s: reinsert fail %i, accepted\n",
			   __func__, ret);
		return NF_ACCEPT;
	}
	return NF_DROP;
}

/* Returns true for an IPv6 NDP packet.
 */
static bool arp_ndp_reinsert_ipv6_is_ndp_pkt(struct sk_buff *skb, int nexthdr,
					     int nhoff)
{
	struct icmp6hdr *icmp6hdr;

	if (nexthdr != IPPROTO_ICMPV6 ||
	    !pskb_network_may_pull(skb, nhoff + sizeof(struct icmp6hdr)))
		return false;

	icmp6hdr = (struct icmp6hdr *)(skb_network_header(skb) + nhoff);

	switch (icmp6hdr->icmp6_type) {
	case NDISC_ROUTER_SOLICITATION:
	case NDISC_ROUTER_ADVERTISEMENT:
	case NDISC_NEIGHBOUR_SOLICITATION:
	case NDISC_NEIGHBOUR_ADVERTISEMENT:
	case NDISC_REDIRECT:
		return true;
	default:
		break;
	}
	return false;
}

static unsigned int arp_ndp_reinsert_hook_ipv6(struct sk_buff *skb, int nhoff)
{
	const struct ipv6hdr *iph;
	struct ipv6hdr _iph;
	__u8 nexthdr;
	__be16 frag_off;
	int ret;

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

	if (arp_ndp_reinsert_ipv6_is_ndp_pkt(skb, nexthdr, nhoff)) {
		ret = arp_ndp_reinsert_packet(skb);
		if (ret) {
			netdev_err(skb->dev, "%s: reinsert fail %i, accepted\n",
				   __func__, ret);
			return NF_ACCEPT;
		}
		return NF_DROP;
	}
	return NF_ACCEPT;
}

static unsigned int arp_ndp_reinsert_hook(void *priv, struct sk_buff *skb,
					  const struct nf_hook_state *state)
{
	__be16 proto = skb->protocol;
	int nhoff = skb_network_offset(skb);
	int err;

	if (eth_type_vlan(proto)) {
		err = arp_ndp_reinsert_vlan_iter(skb, &proto, &nhoff);
		if (err) {
			pr_debug("VLAN header broken, accepted");
			return NF_ACCEPT;
		}
	}

	switch (proto) {
	case htons(ETH_P_IPV6):
		return arp_ndp_reinsert_hook_ipv6(skb, nhoff);
	case htons(ETH_P_ARP):
		return arp_ndp_reinsert_hook_arp(skb, nhoff);
	default:
		return NF_ACCEPT;
	}
}

static struct nf_hook_ops arp_ndp_reinsert_ops = {
	.hook = arp_ndp_reinsert_hook,
	.hooknum = NF_BR_PRE_ROUTING,
	.pf = NFPROTO_BRIDGE,
	.priority = NF_BR_PRI_BRNF,
};

static int __init arp_ndp_reinsert_init(void)
{
	return nf_register_hook(&arp_ndp_reinsert_ops);
}

static void __exit arp_ndp_reinsert_exit(void)
{
	nf_unregister_hook(&arp_ndp_reinsert_ops);
}
module_init(arp_ndp_reinsert_init);
module_exit(arp_ndp_reinsert_exit);

MODULE_LICENSE("Dual BSD/GPL");
MODULE_DESCRIPTION("PON ARP and NDP reinsert driver");
