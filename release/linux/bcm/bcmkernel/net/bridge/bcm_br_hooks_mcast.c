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

#if IS_ENABLED(CONFIG_BCM_MCAST)

#include <linux/netfilter.h>
#include <linux/bcm_br_mcast.h>

#include "bcm_br_hooks_mcast.h"
#include "br_private.h"

br_bcm_mcast_receive_hook br_bcm_mcast_receive = NULL;
br_bcm_mcast_should_deliver_hook br_bcm_mcast_should_deliver = NULL;

int br_bcm_mcast_bind(br_bcm_mcast_receive_hook bcm_rx_hook, br_bcm_mcast_should_deliver_hook bcm_should_deliver_hook)
{
    br_bcm_mcast_receive = bcm_rx_hook;
    br_bcm_mcast_should_deliver = bcm_should_deliver_hook;
    return 0;
}
EXPORT_SYMBOL(br_bcm_mcast_bind);

/* must be called with rcu_read_lock */
int br_bcm_mcast_flood_forward(struct net_device *dev, struct sk_buff *skb)
{
    if (IFF_EBRIDGE & dev->priv_flags)
    {
        struct net_bridge *br = netdev_priv(dev);
        if (br)
        {
            /* TODO: make sure that correct behavior preserved on the transition 4.1 -> 4.19
             * API br_flood_forward(br, skb, NULL, 0) was removed in the new kernel.
             * Need to verify that parameters of the br_flood() API are correct. */
            br_flood(br, skb, BR_PKT_MULTICAST, 0, 0);
            return 0;
        }
    }
    return -EINVAL;
}
EXPORT_SYMBOL(br_bcm_mcast_flood_forward);

unsigned int mcast_receive(struct sk_buff *skb)
{
    struct net_bridge_port *p;

	if (br_bcm_mcast_receive)
	{
        int rv;

        p = br_port_get_rcu(skb->dev);
        if (!p)
            return NF_ACCEPT;

		rv = br_bcm_mcast_receive(p->br->dev->ifindex, skb, 0);
		if (rv < 0)
		{
			/* there was an error with the packet */
			return NF_DROP;
		}
		else if (rv > 0)
		{
			/* the packet was consumed */
			return NF_STOLEN;
		}
		/* continue */
	}
    return NF_ACCEPT;
}

unsigned int mcast_should_deliver(struct sk_buff *skb, const struct nf_hook_state *state)
{
    struct net_bridge_port *p;
   
	if (!br_bcm_mcast_should_deliver)
        return NF_ACCEPT;

    p = br_port_get_rcu(skb->dev);
    if (!p)
        return NF_ACCEPT;

    /* The source device is stored in the state->in pointer for forwarded packets,
     * for local out packets the source device is the bridge device */
    if (0 == br_bcm_mcast_should_deliver(p->br->dev->ifindex, skb, state->in ?: p->br->dev,
#if defined(CONFIG_BRIDGE_IGMP_SNOOPING)
        p->multicast_router == 2 || (p->multicast_router == 1 && timer_pending(&p->multicast_router_timer))))
#else
		false))
#endif
	{
		return NF_DROP;
	}
    return NF_ACCEPT;
}

#endif
