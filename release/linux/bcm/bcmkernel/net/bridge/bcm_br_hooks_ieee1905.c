/*
*    Copyright (c) 2003-2019 Broadcom
*    All Rights Reserved
*
<:label-BRCM:2019:DUAL/GPL:standard

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License, version 2, as published by
the Free Software Foundation (the "GPL").

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.


A copy of the GPL is available at http://www.broadcom.com/licenses/GPLv2.php, or by
writing to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
Boston, MA 02111-1307, USA.

:>
*/

#if IS_ENABLED(CONFIG_BCM_IEEE1905) && defined(CONFIG_NETFILTER_FAMILY_BRIDGE)
// #define DEBUG
#include <linux/skbuff.h>
#include <linux/netfilter.h>
#include "br_private.h"

// handle bridged packets 
unsigned int bcm_br_ieee1905_nf(struct sk_buff *skb)
{
	/* do not bridge multicast 1905 packets when 1905 is compiled */
	if (skb->protocol == htons(0x893a) &&
			skb->pkt_type == PACKET_MULTICAST) {
                pr_debug("dropping multicast packet with proto 0x893a\n");
		return NF_DROP;
        }
	return NF_ACCEPT;
}

// pre-handle routed packets 
// copy of static int br_handle_local_finish(...)
static int bcm_br_ieee1905_rcv(struct sk_buff *skb, struct net_device *dev,
		struct packet_type *pt, struct net_device *orig_dev)
{
	struct net_bridge_port *p = br_port_get_check_rcu(skb->dev);
	u16 vid = 0;

	if (p && p->state != BR_STATE_DISABLED) {
		/* check if vlan is allowed, to avoid spoofing */
		if (p->flags & BR_LEARNING && br_should_learn(p, skb, &vid)) {
                        pr_debug("updating fdb for packet with proto 0x%x\n",
                                        ntohs(skb->protocol));
			br_fdb_update(p->br, p, eth_hdr(skb)->h_source, vid, false);
                }
	}
	
	kfree_skb(skb);
	return NET_RX_SUCCESS;
}

static struct packet_type bcm_br_pt_ieee1905 __read_mostly = {
	.type = htons(0x893a),
	.func = bcm_br_ieee1905_rcv,
};

void bcm_br_ieee1905_pt_add(void)
{
	dev_add_pack(&bcm_br_pt_ieee1905);
}

void bcm_br_ieee1905_pt_del(void)
{
	dev_remove_pack(&bcm_br_pt_ieee1905);
}
#endif // #if IS_ENABLED(CONFIG_BCM_IEEE1905) && defined(CONFIG_NETFILTER_FAMILY_BRIDGE)
