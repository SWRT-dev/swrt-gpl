// SPDX-License-Identifier: GPL-2.0+
/*
 * net/dsa/tag_mxl862xx.c - DSA driver Special Tag support for MaxLinear 862xx switch chips
 *
 * Copyright (C) 2024 MaxLinear Inc.
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * as published by the Free Software Foundation; either version 2
 * of the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 *
 */

#include <linux/bitops.h>
#include <linux/etherdevice.h>
#include <linux/skbuff.h>
#include <net/dsa.h>

#ifndef LINUX_VERSION_CODE
#include <linux/version.h>
#else
#define KERNEL_VERSION(a, b, c) (((a) << 16) + ((b) << 8) + (c))
#endif

#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 0))
#include "dsa_priv.h"
#else
#include "tag.h"
#endif

#define MXL862_NAME	"mxl862xx"

/* To define the outgoing port and to discover the incoming port a special
 * tag is used by the GSW1xx.
 *
 *       Dest MAC       Src MAC    special TAG        EtherType
 * ...| 1 2 3 4 5 6 | 1 2 3 4 5 6 | 1 2 3 4 5 6 7 8 | 1 2 |...
 *                                |<--------------->|
 */

/* special tag in TX path header */
#define MXL862_TX_HEADER_LEN 8

#define MXL862_RX_HEADER_LEN 8

/* Byte 7 */
#define MXL862_IGP_EGP_SHIFT 0
#define MXL862_IGP_EGP_MASK GENMASK(3, 0)

static struct sk_buff *mxl862_tag_xmit(struct sk_buff *skb,
				       struct net_device *dev)
{
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
	int err;
#endif
#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 7, 0))
	struct dsa_port *dp = dsa_slave_to_port(dev);
#else
	struct dsa_port *dp = dsa_user_to_port(dev);
#endif
	struct dsa_port *cpu_dp = dp->cpu_dp;
	unsigned int cpu_port = cpu_dp->index + 1;
	unsigned int usr_port = dp->index + 1;

	u8 *mxl862_tag;

	if (skb == NULL)
		return skb;

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 10, 0))
	err = skb_cow_head(skb, MXL862_TX_HEADER_LEN);
	if (err)
		return NULL;
#endif

	/* provide additional space 'MXL862_TX_HEADER_LEN' bytes */
	skb_push(skb, MXL862_TX_HEADER_LEN);

	/* shift MAC address to the beginnig of the enlarged buffer,
	 * releasing the space required for DSA tag (between MAC address and Ethertype) */
	memmove(skb->data, skb->data + MXL862_TX_HEADER_LEN, 2 * ETH_ALEN);

	/* special tag ingress */
	mxl862_tag = skb->data + 2 * ETH_ALEN;
	mxl862_tag[0] = 0x88;
	mxl862_tag[1] = 0xc3;
	mxl862_tag[2] = 0;
	mxl862_tag[3] = 0;
	mxl862_tag[4] = 0;
	mxl862_tag[5] = usr_port + 16 - cpu_port;
	mxl862_tag[6] = 0;
	mxl862_tag[7] = (cpu_port)&MXL862_IGP_EGP_MASK;

	return skb;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 15, 0))
static struct sk_buff *mxl862_tag_rcv(struct sk_buff *skb,
				      struct net_device *dev,
				      struct packet_type *pt)
#else
static struct sk_buff *mxl862_tag_rcv(struct sk_buff *skb,
				      struct net_device *dev)
#endif
{
	int port;
	u8 *mxl862_tag;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 0))
	struct dsa_port *dp;
#endif

	if (unlikely(!pskb_may_pull(skb, MXL862_RX_HEADER_LEN))) {
		dev_warn_ratelimited(&dev->dev,
				     "Dropping packet, cannot pull SKB\n");
		return NULL;
	}

	mxl862_tag = skb->data - 2;

	if ((mxl862_tag[0] != 0x88) && (mxl862_tag[1] != 0xc3)) {
		dev_warn_ratelimited(
			&dev->dev,
			"Dropping packet due to invalid special tag marker\n");
		dev_warn_ratelimited(
			&dev->dev,
			"Rx Packet Tag: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
			mxl862_tag[0], mxl862_tag[1], mxl862_tag[2],
			mxl862_tag[3], mxl862_tag[4], mxl862_tag[5],
			mxl862_tag[6], mxl862_tag[7]);
		return NULL;
	}

	/* Get source port information */
	port = (mxl862_tag[7] & MXL862_IGP_EGP_MASK) >> MXL862_IGP_EGP_SHIFT;
	port = port - 1;
#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 7, 0))
	skb->dev = dsa_master_find_slave(dev, 0, port);
#else
	skb->dev = dsa_conduit_find_user(dev, 0, port);
#endif
	if (!skb->dev) {
		dev_warn_ratelimited(
			&dev->dev,
			"Dropping packet due to invalid source port\n");
		dev_warn_ratelimited(
			&dev->dev,
			"Rx Packet Tag: 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x, 0x%x\n",
			mxl862_tag[0], mxl862_tag[1], mxl862_tag[2],
			mxl862_tag[3], mxl862_tag[4], mxl862_tag[5],
			mxl862_tag[6], mxl862_tag[7]);
		return NULL;
	}

	/* remove the MxL862xx special tag between the MAC addresses and the current ethertype field. */
	skb_pull_rcsum(skb, MXL862_RX_HEADER_LEN);
	memmove(skb->data - ETH_HLEN,
		skb->data - (ETH_HLEN + MXL862_RX_HEADER_LEN), 2 * ETH_ALEN);
#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 0))
	dp = dsa_slave_to_port(skb->dev);
	if (dp->bridge_dev)
		skb->offload_fwd_mark = 1;
#else
	dsa_default_offload_fwd_mark(skb);
#endif

	return skb;
}

#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 2, 0))
const struct dsa_device_ops mxl862_netdev_ops = {
	.xmit = mxl862_tag_xmit,
	.rcv = mxl862_tag_rcv,
};
#else

static const struct dsa_device_ops mxl862_netdev_ops = {
	.name = "mxl862",
	.proto = DSA_TAG_PROTO_MXL862,
	.xmit = mxl862_tag_xmit,
	.rcv = mxl862_tag_rcv,
#if (LINUX_VERSION_CODE < KERNEL_VERSION(5, 14, 0))
	.overhead = MXL862_RX_HEADER_LEN,
#else
	.needed_headroom = MXL862_RX_HEADER_LEN,
#endif
};

MODULE_LICENSE("GPL");
#if (LINUX_VERSION_CODE < KERNEL_VERSION(6, 2, 0))
MODULE_ALIAS_DSA_TAG_DRIVER(DSA_TAG_PROTO_MXL862);
#else
MODULE_ALIAS_DSA_TAG_DRIVER(DSA_TAG_PROTO_MXL862, MXL862_NAME);
#endif

module_dsa_tag_driver(mxl862_netdev_ops);
#endif

MODULE_LICENSE("GPL");
