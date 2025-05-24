/*
 *	GRE over IPv4 demultiplexer driver
 *
 *	Authors: Dmitry Kozlov (xeb@mail.ru)
 *
 *	This program is free software; you can redistribute it and/or
 *	modify it under the terms of the GNU General Public License
 *	as published by the Free Software Foundation; either version
 *	2 of the License, or (at your option) any later version.
 *
 */

#define pr_fmt(fmt) KBUILD_MODNAME ": " fmt

#include <linux/module.h>
#include <linux/if.h>
#include <linux/icmp.h>
#include <linux/kernel.h>
#include <linux/kmod.h>
#include <linux/skbuff.h>
#include <linux/in.h>
#include <linux/ip.h>
#include <linux/netdevice.h>
#include <linux/if_tunnel.h>
#include <linux/spinlock.h>
#include <net/protocol.h>
#include <net/gre.h>

#include <net/icmp.h>
#include <net/route.h>
#include <net/xfrm.h>

#define GREPROTO_CNT \
       (GREPROTO_MAX + GREPROTO_NONSTD_MAX - GREPROTO_NONSTD_BASE)

static const struct gre_protocol __rcu *gre_proto[GREPROTO_CNT] __read_mostly;

int gre_add_protocol(const struct gre_protocol *proto, u8 version)
{
	if (version >= GREPROTO_NONSTD_BASE && version < GREPROTO_NONSTD_MAX)
		version -= GREPROTO_NONSTD_BASE - GREPROTO_MAX;
	else
		if (version >= GREPROTO_MAX)
			return -EINVAL;

	return (cmpxchg((const struct gre_protocol **)&gre_proto[version], NULL, proto) == NULL) ?
		0 : -EBUSY;
}
EXPORT_SYMBOL_GPL(gre_add_protocol);

int gre_del_protocol(const struct gre_protocol *proto, u8 version)
{
	int ret;

	if (version >= GREPROTO_NONSTD_BASE && version < GREPROTO_NONSTD_MAX)
		version -= GREPROTO_NONSTD_BASE - GREPROTO_MAX;
	else
		if (version >= GREPROTO_MAX)
			return -EINVAL;

	ret = (cmpxchg((const struct gre_protocol **)&gre_proto[version], proto, NULL) == proto) ?
		0 : -EBUSY;

	if (ret)
		return ret;

	synchronize_rcu();
	return 0;
}
EXPORT_SYMBOL_GPL(gre_del_protocol);

static int gre_rcv(struct sk_buff *skb)
{
	const struct gre_protocol *proto;
	u8 ver;
	int ret;

	/* the standard GRE header is 12 octets; the EOIP header is 8
	 * 4 octets long ethernet packet can not be valid
	 */
	if (!pskb_may_pull(skb, 12))
		goto drop;

	/* check for custom EOIP header */
	if (skb->data[0] == 0x20 && skb->data[1] == 0x01 &&
		skb->data[2] == 0x64 && skb->data[3] == 0x00)
		ver = GREPROTO_NONSTD_EOIP - GREPROTO_NONSTD_BASE
			+ GREPROTO_MAX;
	else {
		ver = skb->data[1]&0x7f;
		if (ver >= GREPROTO_MAX)
			goto drop;
	}

	rcu_read_lock();
	proto = rcu_dereference(gre_proto[ver]);
	if (!proto || !proto->handler)
		goto drop_unlock;
	ret = proto->handler(skb);
	rcu_read_unlock();
	return ret;

drop_unlock:
	rcu_read_unlock();
drop:
	kfree_skb(skb);
	return NET_RX_DROP;
}

static void gre_err(struct sk_buff *skb, u32 info)
{
	const struct gre_protocol *proto;
	const struct iphdr *iph = (const struct iphdr *)skb->data;
	u8 ver;

	if (skb->data[(iph->ihl<<2) + 0] == 0x20 &&
		skb->data[(iph->ihl<<2) + 1] == 0x01 &&
		skb->data[(iph->ihl<<2) + 2] == 0x64 &&
		skb->data[(iph->ihl<<2) + 3] == 0x00)
		ver = GREPROTO_NONSTD_EOIP - GREPROTO_NONSTD_BASE
			+ GREPROTO_MAX;
	else {
		ver = skb->data[(iph->ihl<<2) + 1]&0x7f;
		if (ver >= GREPROTO_MAX)
			return;
	}

	rcu_read_lock();
	proto = rcu_dereference(gre_proto[ver]);
	if (proto && proto->err_handler)
		proto->err_handler(skb, info);
	rcu_read_unlock();
}

static const struct net_protocol net_gre_protocol = {
	.handler     = gre_rcv,
	.err_handler = gre_err,
	.netns_ok    = 1,
};

static int __init gre_init(void)
{
	pr_info("GRE over IPv4 demultiplexor driver\n");

	if (inet_add_protocol(&net_gre_protocol, IPPROTO_GRE) < 0) {
		pr_err("can't add protocol\n");
		return -EAGAIN;
	}
	return 0;
}

static void __exit gre_exit(void)
{
	inet_del_protocol(&net_gre_protocol, IPPROTO_GRE);
}

module_init(gre_init);
module_exit(gre_exit);

MODULE_DESCRIPTION("GRE over IPv4 demultiplexer driver");
MODULE_AUTHOR("D. Kozlov (xeb@mail.ru)");
MODULE_LICENSE("GPL");
