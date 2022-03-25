/* Kernel module to match the port-ranges, trigger related port-ranges, 
  * and alters the destination to a local IP address. 
  * 
  * Copyright (C) 2003, CyberTAN Corporation 
  * All Rights Reserved. 
  * 
  * Description: 
  *   This is kernel module for port-triggering. 
  * 
  *   The module follows the Netfilter framework, called extended packet  
  *   matching modules.  
  *
  * This program is free software; you can redistribute it and/or modify
  * it under the terms of the GNU General Public License version 2 as
  * published by the Free Software Foundation.
  */

#include <linux/types.h>
#include <linux/ip.h>
#include <linux/tcp.h>
#include <linux/timer.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/netdevice.h>
#include <linux/if.h>
#include <linux/inetdevice.h>
#include <net/protocol.h>
#include <net/checksum.h>

#include <linux/netfilter_ipv4.h>
#include <linux/netfilter/x_tables.h>
#include <net/netfilter/nf_nat.h>

#include <net/netfilter/nf_conntrack.h>
#include <linux/netfilter_ipv4/ipt_TRIGGER.h>

DEFINE_RWLOCK(ip_conntrack_lock);

struct ipt_trigger {
	struct list_head list;	/* Trigger list */
	struct timer_list timeout;	/* Timer for list destroying */
	u_int32_t srcip;	/* Outgoing source address */
	u_int32_t dstip;	/* Outgoing destination address */
	u_int16_t mproto;	/* Trigger protocol */
	u_int16_t rproto;	/* Related protocol */
	struct ipt_trigger_ports ports;	/* Trigger and related ports */
	u_int8_t reply;		/* Confirm a reply connection */
};

LIST_HEAD(trigger_list);

static void trigger_refresh(struct ipt_trigger *trig, unsigned long extra_jiffies)
{
	NF_CT_ASSERT(trig);
	write_lock_bh(&ip_conntrack_lock);
	/* Need del_timer for race avoidance (may already be dying). */
	if (del_timer(&trig->timeout)) {
		trig->timeout.expires = jiffies + extra_jiffies;
		add_timer(&trig->timeout);
	}

	write_unlock_bh(&ip_conntrack_lock);
}

static void __del_trigger(struct ipt_trigger *trig)
{
	NF_CT_ASSERT(trig);

	/* delete from 'trigger_list' */
	list_del(&trig->list);
	kfree(trig);
}

static void trigger_timeout(unsigned long ul_trig)
{
	struct ipt_trigger *trig = (void *)ul_trig;

	pr_debug("trigger list %p timed out\n", trig);
	write_lock_bh(&ip_conntrack_lock);
	__del_trigger(trig);
	write_unlock_bh(&ip_conntrack_lock);
}

static unsigned int add_new_trigger(struct ipt_trigger *trig)
{
	struct ipt_trigger *new;

	write_lock_bh(&ip_conntrack_lock);
	new = kmalloc(sizeof(struct ipt_trigger), GFP_KERNEL);

	if (!new) {
		write_unlock_bh(&ip_conntrack_lock);
		pr_err("%s: OOM allocating trigger list\n", __FUNCTION__);
		return -ENOMEM;
	}
	memset(new, 0, sizeof(*trig));
	INIT_LIST_HEAD(&new->list);
	memcpy(new, trig, sizeof(*trig));

	/* add to global table of trigger */
	list_add(&new->list, &trigger_list);
	/* add and start timer if required */
	init_timer(&new->timeout);
	new->timeout.data = (unsigned long)new;
	new->timeout.function = trigger_timeout;
	new->timeout.expires = jiffies + (TRIGGER_TIMEOUT * HZ);
	add_timer(&new->timeout);

	write_unlock_bh(&ip_conntrack_lock);
	return 0;
}

static inline int trigger_out_matched(const struct ipt_trigger *i, const u_int16_t proto, const u_int16_t dport)
{
	return ((i->mproto == proto) && (i->ports.mport[0] <= dport)
		&& (i->ports.mport[1] >= dport));
}

static unsigned int trigger_out(struct sk_buff **pskb, const struct xt_action_param *par)
{
	const struct ipt_trigger_info *info = par->targinfo;
	struct ipt_trigger trig, *found;
	const struct iphdr *iph = ip_hdr(*pskb);	//(*pskb)->nh.iph; 
	struct tcphdr *tcph = (void *)iph + iph->ihl * 4;	/* Might be TCP, UDP */

	/* Check if the trigger range has already existed in 'trigger_list'. */
	list_for_each_entry(found, &trigger_list, list) {
		if (trigger_out_matched(found, iph->protocol, ntohs(tcph->dest))) {
			/* Yeah, it exists. We need to update(delay) the destroying timer. */
			trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
			/* In order to allow multiple hosts use the same port range, we update 
			   the 'saddr' after previous trigger has a reply connection. */
			if (found->reply)
				found->srcip = iph->saddr;
			return XT_CONTINUE;	/* We don't block any packet. */
		}
	}
	/* Create new trigger */
	memset(&trig, 0, sizeof(trig));
	trig.srcip = iph->saddr;
	trig.mproto = iph->protocol;
	trig.rproto = info->proto;
	memcpy(&trig.ports, &info->ports, sizeof(struct ipt_trigger_ports));
	add_new_trigger(&trig);	/* Add the new 'trig' to list 'trigger_list'. */
	return XT_CONTINUE;	/* We don't block any packet. */
}

static inline int trigger_in_matched(const struct ipt_trigger *i, const u_int16_t proto, const u_int16_t dport)
{
	u_int16_t rproto = i->rproto;

	if (!rproto)
		rproto = proto;

	return ((rproto == proto) && (i->ports.rport[0] <= dport)
		&& (i->ports.rport[1] >= dport));
}

static unsigned int trigger_in(struct sk_buff **pskb, const struct xt_action_param *par)
{
	struct ipt_trigger *found;
	const struct iphdr *iph = ip_hdr(*pskb);	//(*pskb)->nh.iph; 
	struct tcphdr *tcph = (void *)iph + iph->ihl * 4;	/* Might be TCP, UDP */
	/* Check if the trigger-ed range has already existed in 'trigger_list'. */
	list_for_each_entry(found, &trigger_list, list) {
		if (trigger_in_matched(found, iph->protocol, ntohs(tcph->dest))) {
			/* Yeah, it exists. We need to update(delay) the destroying timer. */
			trigger_refresh(found, TRIGGER_TIMEOUT * HZ);
			return NF_ACCEPT;	/* Accept it, or the imcoming packet could be  
						   dropped in the FORWARD chain */
		}
	}
	return XT_CONTINUE;	/* Our job is the interception. */
}

static unsigned int trigger_dnat(struct sk_buff **pskb, const struct xt_action_param *par)
{
	struct ipt_trigger *found;
	const struct iphdr *iph = ip_hdr(*pskb);	//(*pskb)->nh.iph; 
	struct tcphdr *tcph = (void *)iph + iph->ihl * 4;	/* Might be TCP, UDP */
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	const struct nf_nat_ipv4_multi_range_compat *mr = par->targinfo;
	struct nf_nat_range newrange;

	NF_CT_ASSERT(par->hooknum == NF_INET_PRE_ROUTING);
	/* Check if the trigger-ed range has already existed in 'trigger_list'. */
	list_for_each_entry(found, &trigger_list, list) {
		if (trigger_in_matched(found, iph->protocol, ntohs(tcph->dest))) {
			if (!found->srcip)
				return XT_CONTINUE;
			found->reply = 1;	/* Confirm there has been a reply connection. */
			ct = nf_ct_get(*pskb, &ctinfo);
			NF_CT_ASSERT(ct && (ctinfo == IP_CT_NEW));

			/* Alter the destination of incoming packet. */
			newrange.flags = mr->range[0].flags | NF_NAT_RANGE_MAP_IPS;
			newrange.min_addr.ip = found->srcip;
			newrange.max_addr.ip = found->srcip;
			newrange.min_proto = mr->range[0].min;
			newrange.max_proto = mr->range[0].max;

			/* Hand modified range to generic setup. */
			return nf_nat_setup_info(ct, &newrange, NF_NAT_MANIP_DST);
		}
	}

	return XT_CONTINUE;	/* We don't block any packet. */
}

static unsigned int target_trigger(struct sk_buff *skb, const struct xt_action_param *par)
{

	const struct ipt_trigger_info *info = par->targinfo;
	const struct iphdr *iph = ip_hdr(skb);	//(*pskb)->nh.iph; 
	struct sk_buff *pskb;
	pskb = (struct sk_buff *)skb;

	pr_debug("%s: type = %s\n", __FUNCTION__, (info->type == IPT_TRIGGER_DNAT) ? "dnat" : (info->type == IPT_TRIGGER_IN) ? "in" : "out");

	/* The Port-trigger only supports TCP and UDP. */
	if ((iph->protocol != IPPROTO_TCP) && (iph->protocol != IPPROTO_UDP))
		return XT_CONTINUE;

	if (info->type == IPT_TRIGGER_OUT)
		return trigger_out(&pskb, par);
	else if (info->type == IPT_TRIGGER_IN)
		return trigger_in(&pskb, par);
	else if (info->type == IPT_TRIGGER_DNAT)
		return trigger_dnat(&pskb, par);
	return XT_CONTINUE;

}

static int checkentry_trigger(const struct xt_tgchk_param *par)
{
	const struct ipt_trigger_info *info = par->targinfo;
	struct list_head *cur_item, *tmp_item;

	if ((strcmp(par->table, "mangle") == 0)) {
		pr_err("trigger_check: bad table `%s'.\n", par->table);
		return 0;
	}
	if (par->hook_mask & ~((1 << NF_INET_PRE_ROUTING) | (1 << NF_INET_FORWARD))) {
		pr_err("trigger_check: bad hooks %x.\n", par->hook_mask);
		return 0;
	}
	if (info->proto) {
		if (info->proto != IPPROTO_TCP && info->proto != IPPROTO_UDP) {
			pr_err("trigger_check: bad proto %d.\n", info->proto);
			return 0;
		}
	}
	if (info->type == IPT_TRIGGER_OUT) {
		if (!info->ports.mport[0] || !info->ports.rport[0]) {
			pr_err("trigger_check: Try 'iptbles -j TRIGGER -h' for help.\n");
			return 0;
		}
	}

	/* Empty the 'trigger_list' */
	list_for_each_safe(cur_item, tmp_item, &trigger_list) {
		struct ipt_trigger *trig = (void *)cur_item;

		pr_debug("%s: list_for_each_safe(): %p.\n", __FUNCTION__, trig);
		del_timer(&trig->timeout);
		__del_trigger(trig);
	}

	return 0;
}

static struct xt_target ipt_trigger_reg = {
	.name = "TRIGGER",
	.family = NFPROTO_IPV4,
	.target = target_trigger,
	.targetsize = sizeof(struct ipt_trigger_info),
	.checkentry = checkentry_trigger,
	.me = THIS_MODULE,
};

static int __init init(void)
{
	return xt_register_target(&ipt_trigger_reg);
}

static void __exit fini(void)
{
	xt_unregister_target(&ipt_trigger_reg);
}

module_init(init);
module_exit(fini);
MODULE_LICENSE("GPL");
