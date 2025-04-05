/* SPDX-License-Identifier: GPL-2.0
 *
 * Copyright (c) 2020 MediaTek Inc.
 * Author: Landen Chao <landen.chao@mediatek.com>
 */

#include <linux/of_device.h>
#include <net/netfilter/nf_flow_table.h>

#include "hnat.h"

int hnat_dsa_get_port(struct net_device **dev)
{
#if defined(CONFIG_NET_DSA)
	struct dsa_port *dp;

	dp = dsa_port_from_netdev(*dev);
	if (IS_ERR(dp))
		return -ENODEV;

	*dev = dp->cpu_dp->master;

	return dp->index;
#else
	return -ENODEV;
#endif
}

int hnat_dsa_fill_stag(const struct net_device *netdev,
		       struct foe_entry *entry,
		       struct flow_offload_hw_path *hw_path,
		       u16 eth_proto,
		       int mape)
{
#if defined(CONFIG_NET_DSA)
	const unsigned int *port_reg;
	const struct dsa_port *dp;
	struct net_device *ndev;
	int port_index;
	u16 dsa_tag;

	if (hw_path->flags & FLOW_OFFLOAD_PATH_VLAN)
		ndev = hw_path->dev;
	else
		ndev = (struct net_device *)netdev;

	port_reg = of_get_property(ndev->dev.of_node, "reg", NULL);
	if (unlikely(!port_reg))
		return -EINVAL;

	port_index = be32_to_cpup(port_reg);

	/* In the case MAPE LAN --> WAN, binding entry is to CPU.
	 * Do not add special tag.
	 */
	if (IS_WAN(ndev) && mape)
		return port_index;

	dp = dsa_port_from_netdev(ndev);
	if (IS_ERR(dp))
		return -ENODEV;

	if (IS_DSA_TAG_PROTO_MXL862_8021Q(dp)) {
		dsa_tag = port_index + BIT(11);

		if (IS_IPV4_GRP(entry)) {
			/* PPE can only be filled up to 2 VLAN layers,
			 * outer VLAN(vlan1) is preserved for stag.
			 */
			if (unlikely(entry->ipv4_hnapt.vlan2))
				return -EINVAL;
			else if (entry->ipv4_hnapt.vlan1)
				/* Move to inner VLAN if it's already set */
				entry->ipv4_hnapt.vlan2 = entry->ipv4_hnapt.vlan1;
			entry->ipv4_hnapt.vlan1 = dsa_tag;

			entry->bfib1.vlan_layer = (entry->ipv4_hnapt.vlan1 != 0) +
						  (entry->ipv4_hnapt.vlan2 != 0);
		} else {
			if (unlikely(entry->ipv6_5t_route.vlan2))
				return -EINVAL;
			else if (entry->ipv6_5t_route.vlan1)
				/* Move to inner VLAN if it's already set */
				entry->ipv6_5t_route.vlan2 = entry->ipv6_5t_route.vlan1;
			entry->ipv6_5t_route.vlan1 = dsa_tag;

			entry->bfib1.vlan_layer = (entry->ipv6_5t_route.vlan1 != 0) +
						  (entry->ipv6_5t_route.vlan2 != 0);
		}

		entry->bfib1.vpm = 1;
	} else {
		dsa_tag = BIT(port_index);

		if (!entry->bfib1.vlan_layer)
			entry->bfib1.vlan_layer = 1;
		else
			/* VLAN existence indicator */
			dsa_tag |= BIT(8);

		if (IS_IPV4_GRP(entry))
			entry->ipv4_hnapt.etype = dsa_tag;
		else
			entry->ipv6_5t_route.etype = dsa_tag;

		entry->bfib1.vpm = 0;
	}

	return port_index;
#else
	return -EINVAL;
#endif
}
