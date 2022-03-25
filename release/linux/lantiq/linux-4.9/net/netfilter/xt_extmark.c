/*
 *	xt_extmark - Netfilter module to match NFEXTMARK value
 *
 *	(C) 1999-2001 Marc Boucher <marc@mbsi.ca>
 *	Copyright © CC Computer Consultants GmbH, 2007 - 2008
 *	Jan Engelhardt <jengelh@medozas.de>
 *
 *	This program is free software; you can redistribute it and/or modify
 *	it under the terms of the GNU General Public License version 2 as
 *	published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/skbuff.h>

#include <linux/netfilter/xt_extmark.h>
#include <linux/netfilter/x_tables.h>

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Marc Boucher <marc@mbsi.ca>");
MODULE_DESCRIPTION("Xtables: packet extmark operations");
MODULE_ALIAS("ipt_extmark");
MODULE_ALIAS("ip6t_extmark");
MODULE_ALIAS("ipt_EXTMARK");
MODULE_ALIAS("ip6t_EXTMARK");

static unsigned int
extmark_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_extmark_tginfo2 *info = par->targinfo;

	skb->extmark = (skb->extmark & ~info->mask) ^ info->extmark;
	return XT_CONTINUE;
}

static bool
extmark_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_extmark_mtinfo1 *info = par->matchinfo;

	return ((skb->extmark & info->mask) == info->extmark) ^ info->invert;
}

static struct xt_target extmark_tg_reg __read_mostly = {
	.name           = "EXTMARK",
	.revision       = 2,
	.family         = NFPROTO_UNSPEC,
	.target         = extmark_tg,
	.targetsize     = sizeof(struct xt_extmark_tginfo2),
	.me             = THIS_MODULE,
};

static struct xt_match extmark_mt_reg __read_mostly = {
	.name           = "extmark",
	.revision       = 1,
	.family         = NFPROTO_UNSPEC,
	.match          = extmark_mt,
	.matchsize      = sizeof(struct xt_extmark_mtinfo1),
	.me             = THIS_MODULE,
};

static int __init extmark_mt_init(void)
{
	int ret;

	ret = xt_register_target(&extmark_tg_reg);
	if (ret < 0)
		return ret;
	ret = xt_register_match(&extmark_mt_reg);
	if (ret < 0) {
		xt_unregister_target(&extmark_tg_reg);
		return ret;
	}
	return 0;
}

static void __exit extmark_mt_exit(void)
{
	xt_unregister_match(&extmark_mt_reg);
	xt_unregister_target(&extmark_tg_reg);
}

module_init(extmark_mt_init);
module_exit(extmark_mt_exit);
