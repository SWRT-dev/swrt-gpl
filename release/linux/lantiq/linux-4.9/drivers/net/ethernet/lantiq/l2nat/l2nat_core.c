/*
  GPL LICENSE SUMMARY

  Copyright(c) 2016 Intel Corporation.

  This program is free software; you can redistribute it and/or modify
  it under the terms of version 2 of the GNU General Public License as
  published by the Free Software Foundation.

  This program is distributed in the hope that it will be useful, but
  WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 51 Franklin St - Fifth Floor, Boston, MA 02110-1301 USA.
  The full GNU General Public License is included in this distribution
  in the file called LICENSE.GPL.

  Contact Information:
    Intel Corporation
    2200 Mission College Blvd.
    Santa Clara, CA  97052
*/

#define pr_fmt(fmt) "L2NAT:%s:%d " fmt, __func__, __LINE__

#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/netdevice.h>
#include <linux/if_vlan.h>
#include <linux/if_arp.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <linux/stat.h>
#include <linux/netfilter_bridge.h>
#include <net/arp.h>
#include <net/ipx.h>
#include <net/llc_pdu.h>
#include <asm/checksum.h>
#include <net/ppa/ppa_api.h>
#include <net/ppa/ppa_stack_al.h>
#include <net/ppa/ppa_hook.h>
#include "l2nat.h"

#define DRV_MODULE_VERSION "0.0.1"

/* l2nat global statistics */
struct l2nat_global_stats glbl_stats;

/* nf hooks registered */
bool hooks_registered;

/* RFC 2132: 9.1. Requested IP Address
 * RFC 2132: 9.7. Server Identifier
 */
struct dhcpopt_hdr {
	u8 code;
#define DHCP_OPT_REQ_IP		50
#define DHCP_OPT_MSG_TYPE	53
#define DHCP_OPT_SRV_ID		54
#define DHCP_OPT_PAD		0
#define DHCP_OPT_END		255

	u8 len;
} __packed __aligned(1);

struct dhcpopt_ip {
	struct dhcpopt_hdr hdr;
	u32 ip;
} __packed __aligned(1);

struct dhcpopt_msg_type {
	struct	dhcpopt_hdr hdr;
#define DHCPREQUEST	3
	u8	type;
} __packed __aligned(1);

/* magic cookie for DHCP, put at the start of the options field.
 * it is in network byte order */
#define DHCP_MAGIC		0x63825363

/* udp ports used in client-server communication */
#define BOOTP_SERVER_PORT	67

/* 1 ARP request per 300ms */
#define ARP_REQ_SEND_RATE_MS	300

static void recv_fake_arp_req(struct l2nat_dev *l2nat, __be32 sip,
			      unsigned char *smac, __be32 dip)
{
	struct sk_buff *skb = arp_create(ARPOP_REQUEST, ETH_P_ARP, dip,
					 l2nat->dev, sip, NULL, smac, NULL);

	if (!skb) {
		l2n_warn("failed to create arp req for %s\n", l2nat->dev->name);
		return;
	}

	skb_reset_mac_header(skb);
	skb->protocol = eth_type_trans(skb, l2nat->dev);

	netif_rx(skb);

	l2nat->stats.arp_gen++;

	l2n_dbg("receiving arp request: who has %pI4 tell %pI4 in %pM via %s\n",
		&dip, &sip, smac, l2nat->dev->name);
}

void __gen_fake_arp_req(struct l2nat_dev *l2nat, __be32 sip,
			unsigned char *smac, __be32 dip)
{
	if (unlikely(!sip || !dip))
		return;

	/* kind of rate limiting for the messages */
	if (jiffies - l2nat->last_arp_sent_ts < L2NAT_SEND_ARP_LIMIT)
		return;

	/* Recv fake ARP request */
	recv_fake_arp_req(l2nat, sip, smac, dip);

	l2nat->last_arp_sent_ts = jiffies;
}

void gen_fake_arp_req(struct l2nat_dev *l2nat, __be32 sip,
		      unsigned char *smac, __be32 dip)
{
	spin_lock_bh(&l2nat->lock);
	__gen_fake_arp_req(l2nat, sip, smac, dip);
	spin_unlock_bh(&l2nat->lock);
}

static inline struct dhcpopt_hdr *dhcp_req_find_opt(struct sk_buff *skb,
						    struct dhcpopt_hdr *hdr,
						    u8 opt_code)
{
	l2n_dbg_dhcp("Looking for %d\n", opt_code);
	while (hdr->code != DHCP_OPT_END && hdr->code != DHCP_OPT_PAD &&
	       hdr->code != opt_code && (u8 *)hdr < skb_tail_pointer(skb)) {

		l2n_dbg_dhcp("option %d (0x%02x), length %d\n",
			     hdr->code, hdr->code, hdr->len);

		hdr = (struct dhcpopt_hdr *)((u8 *)hdr +
					     hdr->len +
					     sizeof(*hdr));
	}
	if (hdr->code == opt_code)
		return hdr;
	return NULL;
}

static inline int dhcp_req_update(struct l2nat_dev *l2nat, struct sk_buff *skb,
				  ptrdiff_t offset)
{
	struct iphdr *iph;
	struct udphdr *udph;
	struct dhcphdr *dhcph;
	struct dhcpopt_hdr *opt_hdr;
	struct dhcpopt_ip *opt_ip;
	int extra_size;
	bool add_req_ip_opt;
	bool add_srv_id_opt;

	iph = (struct iphdr *)(skb_mac_header(skb) + offset);

	/* check the protocol to be UDP */
	if (iph->protocol != IPPROTO_UDP)
		return 0;

	udph = (struct udphdr *)((char *)iph + iph->ihl * 4);

	/* destination port should be 67 (bootps) */
	if (likely(udph->dest != __constant_htons(BOOTP_SERVER_PORT)))
		return 0;

	dhcph = (struct dhcphdr *)((char *)udph + sizeof(*udph));

	/* should be bootp request, no broadcast flag set,
	 * and have dhcp magic number in .options */
	if (dhcph->op != BOOTREQUEST || dhcph->htype != ARPHRD_ETHER ||
	    dhcph->magic != __constant_ntohl(DHCP_MAGIC))
		return 0;

	add_req_ip_opt = false;
	add_srv_id_opt = false;
	extra_size = 0;

	/* adding new options in case when DstAddr!=Broadcast */
	if (iph->daddr == __constant_htonl(INADDR_BROADCAST)) {
		/*
		 * Don't add DHCP_OPT_REQ_IP and DHCP_OPT_SRV_ID to DHCP
		 * request with limited broadcast destination IP
		 * (REBOOT-INIT DHCP state).
		 */

		/* check DHCP message type */
		opt_hdr = dhcp_req_find_opt(skb,
					   (struct dhcpopt_hdr *)dhcph->options,
					   DHCP_OPT_MSG_TYPE);
		if (opt_hdr) {
			l2n_dbg_dhcp("DHCP message type is %d %pI4 %pI4\n",
			    ((struct dhcpopt_msg_type *)opt_hdr)->type,
			    &iph->saddr, &iph->daddr);
		}

	} else {

		/* no options */
		opt_hdr = dhcp_req_find_opt(skb,
					   (struct dhcpopt_hdr *)dhcph->options,
					   DHCP_OPT_END);
		if (!opt_hdr)
			goto dhcp_opt_done;

		/* check DHCP message type */
		opt_hdr = dhcp_req_find_opt(skb,
					   (struct dhcpopt_hdr *)dhcph->options,
					   DHCP_OPT_MSG_TYPE);
		if (!opt_hdr)
			goto dhcp_opt_done;

		l2n_dbg_dhcp("DHCP message type is %d %pI4 %pI4\n",
			    ((struct dhcpopt_msg_type *)opt_hdr)->type,
			    &iph->saddr, &iph->daddr);

		if (((struct dhcpopt_msg_type *)opt_hdr)->type != DHCPREQUEST)
			goto dhcp_opt_done;

		/* Check if the options should be added*/
		if (!dhcp_req_find_opt(skb,
				      (struct dhcpopt_hdr *)dhcph->options,
				      DHCP_OPT_REQ_IP)) {

			add_req_ip_opt = true;
			extra_size += sizeof(struct dhcpopt_ip);
		}

		if (!dhcp_req_find_opt(skb,
				      (struct dhcpopt_hdr *)dhcph->options,
				      DHCP_OPT_SRV_ID)) {

			add_srv_id_opt = true;
			extra_size += sizeof(struct dhcpopt_ip);
		}

		if (skb->len + extra_size > 65535) {
			add_req_ip_opt = false;
			add_srv_id_opt = false;
			extra_size = 0;
			goto dhcp_opt_done;
		}
dhcp_opt_done:
		if (!extra_size)
			extra_size = skb_tailroom(skb);

		if (pskb_expand_head(skb, 0, extra_size - skb_tailroom(skb),
				     GFP_ATOMIC)) {
			l2n_err("cannot expand skb, dropping packet\n");
			return -1;
		}
	}

	/* update pointers after expand */
	iph = (struct iphdr *)(skb_mac_header(skb) + offset);
	udph = (struct udphdr *)((char *)iph + iph->ihl * 4);
	dhcph = (struct dhcphdr *)((char *)udph + sizeof(*udph));

	if (!(dhcph->flags & __constant_htons(BOOTP_BRD_FLAG))) {
		/* set the broadcast flag */

		dhcph->flags |= __constant_ntohs(BOOTP_BRD_FLAG);
		/* change checksum according to added broadcast flag.
		 * ip_decrease_ttl was used as an example
		 */
		if (udph->check) {
			u32 check;

			check = (u32)udph->check;
			check += (u32)__constant_ntohs(~((u16)BOOTP_BRD_FLAG));
			udph->check = (u16)(check +
					    ((check >= 0xffff) ? 1 : 0));

			/* in udp 0 csum means no checksumming,
			 * so if csum is 0 it should be set to 0xffff */
			if (!udph->check)
				udph->check = 0xffff;
		}
	}

	/* At this point we always have enough space for additional DHCP
	   options */

	/* all DHCP request packet modification which is done below are required
	 * to force DHCP server to send DHCP ACK as broadcast message*/

	/* adjust to new size */
	skb_put(skb, extra_size);
	extra_size = 0;

	/* Add additional field "requested IP" and "server id" -
	* it's resolved issues with HW like Linksys WRT610N, WRT160*/
	if (true == add_req_ip_opt) {

		l2n_dbg_dhcp("Adding requested ip %pI4\n", &dhcph->ciaddr);
		opt_hdr = dhcp_req_find_opt(skb,
					   (struct dhcpopt_hdr *)dhcph->options,
					   DHCP_OPT_END);

		opt_ip = (struct dhcpopt_ip *)opt_hdr;
		/* 9.1. Requested IP Address */
		opt_ip[0].hdr.code = DHCP_OPT_REQ_IP;
		opt_ip[0].hdr.len = 4;
		opt_ip[0].ip = dhcph->ciaddr;
		opt_ip[1].hdr.code = DHCP_OPT_END;

		extra_size += sizeof(struct dhcpopt_ip);
	}

	if (true == add_srv_id_opt) {

		l2n_dbg_dhcp("Adding server id %pI4\n", &iph->daddr);

		opt_hdr = dhcp_req_find_opt(skb,
					   (struct dhcpopt_hdr *)dhcph->options,
					   DHCP_OPT_END);
		opt_ip = (struct dhcpopt_ip *)opt_hdr;
		/* 9.7. Server Identifier */
		opt_ip[0].hdr.code = DHCP_OPT_SRV_ID;
		opt_ip[0].hdr.len = 4;
		opt_ip[0].ip = iph->daddr;
		opt_ip[1].hdr.code = DHCP_OPT_END;

		extra_size += sizeof(struct dhcpopt_ip);
	}

	/* cleanup CI address fields DHCP Request - it's resolved issues with
	 * HW like Cisco E2000 router */
	dhcph->ciaddr = 0;

	/* Update size fields in headers */
	iph->tot_len = htons(ntohs(iph->tot_len) + extra_size);
	udph->len = htons(ntohs(udph->len) + extra_size);

	/* recalculate check sums */
	iph->check = 0;
	iph->check = ip_fast_csum((unsigned char *)iph, iph->ihl);
	if (udph->check) {
		int data_len = ntohs(udph->len);
		udph->check = 0;
		udph->check = csum_tcpudp_magic(iph->saddr,
						iph->daddr, data_len,
						IPPROTO_UDP,
						csum_partial((char *)udph,
							     data_len, 0));
		if (udph->check == 0)
			udph->check = -1;
	}

	l2nat->stats.dhcp++;

	return 0;
}

int l2nat_tx(struct l2nat_dev *l2nat, struct sk_buff *skb)
{
	struct ethhdr *eth;
	struct vlan_hdr *vlanh;
	struct ipxhdr *ipxh;
	struct arphdr *arph;
	struct iphdr *iph;
	struct snap_hdr *snaph;

	u16 proto;
	u8 *p;
	u32 ip;
	ptrdiff_t offset = sizeof(struct ethhdr);

	/* get the type of the frame and set ip-mac pair to be checked
	 * (and possibly added) in table
	 */
	eth = eth_hdr(skb);
	proto = eth->h_proto;

analyse:
	switch (proto) {

	case __constant_htons(ETH_P_8021Q):
		vlanh = (struct vlan_hdr *)(skb_mac_header(skb) + offset);
		proto = vlanh->h_vlan_encapsulated_proto;
		offset += sizeof(struct vlan_hdr);
		goto analyse;

	case __constant_htons(ETH_P_IP):
		iph = (struct iphdr *)(skb_mac_header(skb) + offset);

		/* check in the table, add entry if needed.
		 * ip_header->saddr assumed to be 4-bytes aligned
		 */
		l2nat_entry_check_add(l2nat, iph->saddr, eth->h_source);

		/* if dhcp req set broadcast flag,  add DHCP opts if needed
		 * XXX: eth and iph are invalid after this point */
		if (dhcp_req_update(l2nat, skb, offset))
			return -1;

		break;

	case __constant_htons(ETH_P_ARP):
		arph = (struct arphdr *)(skb_mac_header(skb) + offset);

		if (arph->ar_pro != htons(ETH_P_IP) ||
		    arph->ar_hrd != htons(ARPHRD_ETHER))
			break;

		p = (char *)arph + sizeof(*arph);

		/* save ip and mac */

		/* sender IP is 2-bytes aligned, so copy it to temporal storage
		 * to prevent unaligned access with subsequent data abort
		 * handling
		 */
		memcpy(&ip, p + ETH_ALEN, sizeof(ip));

		/* check in the table, add entry if needed */
		l2nat_entry_check_add(l2nat, ip, p);

		/* change src mac */
		memcpy(p, l2nat->dev->dev_addr, ETH_ALEN);

		l2nat->stats.tx_arp++;
		break;

	case __constant_htons(ETH_P_IPX):
		ipxh = (struct ipxhdr *)(skb_mac_header(skb) + offset);

		/* set source node to wilreless MAC address */
		memcpy(ipxh->ipx_source.node, l2nat->dev->dev_addr, ETH_ALEN);
		break;

	default:
		if (ntohs(proto) > ETH_DATA_LEN)
			break;

		/* 802.3 frame header with Length in T/E field */
		proto = *(u16 *)(skb_mac_header(skb) + offset);
		switch (proto) {
		case 0xFFFF:
			/* checksum of IPX over 802.3 with raw encapsulation */
			proto = __constant_htons(ETH_P_IPX);
			goto analyse;
		case 0xE0E0:
			/* part of LLC header used with IPX over
			   802.2 Novel encapsulation */
			proto = __constant_htons(ETH_P_IPX);
			 /* skip LLC header */
			offset += sizeof(struct llc_pdu_un);
			goto analyse;
		case 0xAAAA:
			/* part oh LLC/SNAP header used with (possible)
			   IPX over 802.2 SNAP encapsulation */
			snaph = (struct snap_hdr *)(skb_mac_header(skb) +
						    offset +
						    sizeof(struct llc_pdu_un));
			proto = snaph->ether_type;
			/* skip LLC/SNAP header */
			offset += sizeof(struct llc_pdu_un) +
				  sizeof(struct snap_hdr);
			goto analyse;
		default:
			break;
		}
		break;
	}

	memcpy(eth_hdr(skb)->h_source, l2nat->dev->dev_addr, ETH_ALEN);
	return 0;
}


int l2nat_rx(struct l2nat_dev *l2nat, struct sk_buff *skb)
{
	struct hash_entry *ent;
	struct ethhdr *eth;
	struct vlan_hdr *vlanh;
	struct arphdr *arph;
	struct iphdr  *iph;
	struct ipxhdr *ipxh;
	struct snap_hdr *snaph;
	u32 ip;
	u16 proto;
	unsigned char *p, *dnat_mac = NULL;
	ptrdiff_t offset = sizeof(struct ethhdr);
	int ret = 0;

	eth = eth_hdr(skb);
	proto = eth->h_proto;


	/* do not touch multicast / broadcast*/
	if (is_multicast_ether_addr(eth->h_dest))
		return ret;

	/* use device mac address as default dest address*/
	dnat_mac = l2nat->default_host;

analyse:
	switch (proto) {
	case __constant_htons(ETH_P_PAE):
		spin_unlock_bh(&l2nat->lock);
		return ret;
	case __constant_htons(ETH_P_8021Q):
		vlanh = (struct vlan_hdr *)(skb_mac_header(skb) + offset);
		proto = vlanh->h_vlan_encapsulated_proto;
		offset += sizeof(struct vlan_hdr);
		goto analyse;
	case __constant_htons(ETH_P_IP):

		iph = (struct iphdr *)(skb_mac_header(skb) + offset);

		ent = l2nat_entry_get(l2nat, iph->daddr);
		if (unlikely(!ent)) {
			gen_fake_arp_req(l2nat, iph->saddr, eth->h_source,
					 iph->daddr);
			/* drop this packet to avoid wrong learning */
			ret = -1;
			break;
		}

		if (unlikely(atomic_read(&l2nat->arp_info_needed))) {

			/* copy IP and MAC to be used when generating*/
			spin_lock_bh(&l2nat->lock);

			memcpy(&l2nat->mac_for_arp, eth->h_source, ETH_ALEN);
			l2nat->ip_for_arp = iph->saddr;
			l2nat->last_arp_info_ts = jiffies;
			atomic_set(&l2nat->arp_info_needed, 0);

			spin_unlock_bh(&l2nat->lock);
		}

		dnat_mac = ent->mac;

		ent->rx_packets++;
		l2nat_entry_put(ent);
		break;
	case __constant_htons(ETH_P_ARP):

		arph = (struct arphdr *)(skb_mac_header(skb) + offset);
		if (arph->ar_pro != htons(ETH_P_IP) ||
		    arph->ar_hrd != htons(ARPHRD_ETHER))
			break;

		l2nat->stats.rx_arp++;

		/* set p to start of arp payload*/
		p = (u8 *)arph + sizeof(*arph);
		/* copy target ip into local variable */
		memcpy(&ip, p + 2 * ETH_ALEN + sizeof(ip), sizeof(ip));

		/* find entry, set dest mac */
		ent = l2nat_entry_get(l2nat, ip);
		if (!ent) {
			gen_fake_arp_req(l2nat, *(u32 *)(p + ETH_ALEN) , p, ip);
			/* drop this packet to avoid wrong learning */
			ret = -1;
			break;
		}

		switch (ntohs(arph->ar_op)) {
		case ARPOP_REPLY:
			/* change target hw addr to one from found entry */
			memcpy(p + ETH_ALEN + sizeof(ip), &ent->mac, ETH_ALEN);
			/* do not break, so that the dnat_mac will be set */
		case ARPOP_REQUEST: /* arp solicit requests */
			dnat_mac = ent->mac;
			break;
		default:
			break;
		}

		ent->rx_packets++;
		l2nat_entry_put(ent);
		break;

	case __constant_htons(ETH_P_IPX):

		if (unlikely(dnat_mac == NULL))
			break;

		ipxh = (struct ipxhdr *)(skb_mac_header(skb) + offset);
		/* set dest node node to default GW MAC address
		   (dnat_mac is already set to it) */
		memcpy(ipxh->ipx_dest.node, dnat_mac, IPX_NODE_LEN);
		break;

	default:
		if (ntohs(proto) > ETH_DATA_LEN)
			break;

		/* 802.3 frame header with Length in T/E field */
		proto = *(u16 *)(skb_mac_header(skb) + offset);
		switch (proto) {
		case 0xFFFF:
			/* checksum of IPX over 802.3 with raw encapsulation */
			proto = __constant_htons(ETH_P_IPX);
			goto analyse;
		case 0xE0E0:
			/* part of LLC header used with IPX over802.2
			   Novel encapsulation */
			proto = __constant_htons(ETH_P_IPX);
			/* skip LLC header */
			offset += sizeof(struct llc_pdu_un);
			goto analyse;
		case 0xAAAA:
			/* part of LLC/SNAP header used with (possible)
			   IPX over 802.2 SNAP encapsulation */
			snaph = (struct snap_hdr *)(skb_mac_header(skb) +
						    offset +
						    sizeof(struct llc_pdu_un));
			proto = snaph->ether_type;
			/* skip LLC/SNAP header */
			offset += sizeof(struct llc_pdu_un) +
				  sizeof(struct snap_hdr);
			goto analyse;
		default:
			break;
		}
		break;
	}

	if (likely(dnat_mac != NULL))
		memcpy(&eth->h_dest, dnat_mac, ETH_ALEN);

	return ret;
}


static unsigned int l2nat_nf_br_pre_routing(void *priv,
					     struct sk_buff *skb,
					     const struct nf_hook_state *state)
{
	struct l2nat_dev *l2nat;
	

	glbl_stats.preroute++;

	if (unlikely(!state->in)) {
		l2n_err("in device null in prerouting hook\n");
		goto done;
	}

	l2nat = l2nat_dev_get(state->in);

	/* this is not an l2nat device*/
	if (!l2nat)
		goto done;

	/* make writeable in case skb is shared */
	if (!skb_make_writable(skb, skb->len))
		goto put_drop;

	if (l2nat_rx(l2nat, skb) < 0)
		goto put_drop;

	l2nat->stats.rx_packets++;
	l2nat->stats.rx_bytes += skb->len;

	l2nat_dev_put(l2nat);
done:
	return NF_ACCEPT;

put_drop:
	l2nat_dev_put(l2nat);
	l2nat->stats.rx_dropped++;
	return NF_DROP;
}

static unsigned int l2nat_nf_br_post_routing(void *priv,
					     struct sk_buff *skb,
					     const struct nf_hook_state *state)
{
	struct l2nat_dev *l2nat;

	glbl_stats.postroute++;

	if (unlikely(!state->out)) {
		l2n_err("out device null in postrouting hook\n");
		goto done;
	}

	l2nat = l2nat_dev_get(state->out);

	/* this is not an l2nat device*/
	if (!l2nat)
		goto done;

	/* make writeable in case skb is shared */
	if (!skb_make_writable(skb, skb->len))
		goto put_drop;

	if (l2nat_tx(l2nat, skb) < 0)
		goto put_drop;

	l2nat->stats.tx_packets++;
	l2nat->stats.tx_bytes += skb->len;

	l2nat_dev_put(l2nat);
done:
	return NF_ACCEPT;

put_drop:
	l2nat_dev_put(l2nat);
	l2nat->stats.rx_dropped++;
	return NF_DROP;
}

enum nf_l2nat_hook_priorities {
	NF_BR_PRI_L2NAT = -400,
};

static struct nf_hook_ops l2nat_nf_br_ops[] __read_mostly = {
	{
		.hook = l2nat_nf_br_pre_routing,
		.pf = NFPROTO_BRIDGE,
		.hooknum = NF_BR_PRE_ROUTING,
		.priority = NF_BR_PRI_L2NAT,
	},
	{
		.hook = l2nat_nf_br_post_routing,
		.pf = NFPROTO_BRIDGE,
		.hooknum = NF_BR_POST_ROUTING,
		.priority = NF_BR_PRI_L2NAT,
	},
};

int l2nat_add_device(struct net_device *dev)
{

	int ret = l2nat_dev_add(dev);

	if (ret < 0)
		return ret;

	if (hooks_registered)
		return 0;

	ret = nf_register_hooks(l2nat_nf_br_ops, ARRAY_SIZE(l2nat_nf_br_ops));
	if (ret) {
		l2n_err("failed to register nf hooks\n");
		return ret;
	}

	hooks_registered = true;
	l2n_info("nf hooks registered\n");

	return 0;
}

void l2nat_del_device(struct net_device *dev)
{

	int dev_count = l2nat_dev_count();

	if (l2nat_dev_del(dev))
		return;

	if (dev_count != 1 || !hooks_registered)
		return;

	nf_unregister_hooks(l2nat_nf_br_ops, ARRAY_SIZE(l2nat_nf_br_ops));

	hooks_registered = false;
	l2n_info("nf hooks unregistered\n");
}

int32_t l2nat_check_device(struct net_device *dev, char *name, uint32_t flags)
{
	struct net *net;
	struct l2nat_dev *l2nat;

	if (!dev && !name)
		return 0;

	if (!dev) {
		for_each_net(net) {
			dev = dev_get_by_name(net, name);
			if (dev)
				break;
		}
	}

	if (!dev)
		return 0;

	l2nat = l2nat_dev_get(dev);

	if (!l2nat)
		return 0;

	l2nat_dev_put(l2nat);

	return 1;
}

/* netdev event handler */
static int l2nat_netdev_event(struct notifier_block *this, unsigned long event,
			      void *ptr)
{
	struct net_device *netdev = ptr;

	/* try to remove this device, will do nothing if this is not an l2nat
	   device */
	if (event == NETDEV_UNREGISTER)
		l2nat_del_device(netdev);

	return NOTIFY_DONE;
}

/* notifier block for netdev unregister */
static struct notifier_block dev_notifier = {
	.notifier_call = l2nat_netdev_event
};

int l2nat_init(void)
{
	int ret;

	l2nat_dev_list_init();

	ppa_check_if_netif_l2nat_fn = l2nat_check_device;

	l2nat_proc_init();

	ret = register_netdevice_notifier(&dev_notifier);
	if (ret) {
		l2n_err("register_netdevice_notifier failed (ret=%d)\n", ret);
		goto notifier_err;
	}

	l2n_info("l2nat module loaded\n");

	return 0;

notifier_err:
	l2nat_proc_exit();
	l2nat_dev_list_exit();

	return ret;
}

void l2nat_exit(void)
{
	unregister_netdevice_notifier(&dev_notifier);

	if (hooks_registered) {
		nf_unregister_hooks(l2nat_nf_br_ops,
				    ARRAY_SIZE(l2nat_nf_br_ops));
	}

	l2nat_proc_exit();

	ppa_check_if_netif_l2nat_fn = NULL;

	l2nat_dev_list_exit();
	l2n_info("l2nat module unloaded\n");
}

module_init(l2nat_init);
module_exit(l2nat_exit);


MODULE_AUTHOR("Yair Weiss <yair.weiss@intel.com>");
MODULE_DESCRIPTION("intel layer 2 nat driver)");
MODULE_LICENSE("GPL");

