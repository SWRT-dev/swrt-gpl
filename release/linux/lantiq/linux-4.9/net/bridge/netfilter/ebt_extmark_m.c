/*
 *  ebt_extmark_m
 *
 *	Authors:
 *	Bart De Schuymer <bdschuym@pandora.be>
 *
 *  July, 2002
 *
 */

/* This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_extmark_m.h>

static bool
ebt_extmark_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct ebt_extmark_m_info *info = par->matchinfo;

	if (info->bitmask & EBT_EXTMARK_OR)
		return !!(skb->extmark & info->mask) ^ info->invert;
	return ((skb->extmark & info->mask) == info->extmark) ^ info->invert;
}

static int ebt_extmark_mt_check(const struct xt_mtchk_param *par)
{
	const struct ebt_extmark_m_info *info = par->matchinfo;

	if (info->bitmask & ~EBT_EXTMARK_MASK)
		return -EINVAL;
	if ((info->bitmask & EBT_EXTMARK_OR) &&
	    (info->bitmask & EBT_EXTMARK_AND))
		return -EINVAL;
	if (!info->bitmask)
		return -EINVAL;
	return 0;
}

#ifdef CONFIG_COMPAT
struct compat_ebt_extmark_m_info {
	compat_ulong_t extmark, mask;
	uint8_t invert, bitmask;
};

static void extmark_mt_compat_from_user(void *dst, const void *src)
{
	const struct compat_ebt_extmark_m_info *user = src;
	struct ebt_extmark_m_info *kern = dst;

	kern->extmark = user->extmark;
	kern->mask = user->mask;
	kern->invert = user->invert;
	kern->bitmask = user->bitmask;
}

static int extmark_mt_compat_to_user(void __user *dst, const void *src)
{
	struct compat_ebt_extmark_m_info __user *user = dst;
	const struct ebt_extmark_m_info *kern = src;

	if (put_user(kern->extmark, &user->extmark) ||
	    put_user(kern->mask, &user->mask) ||
	    put_user(kern->invert, &user->invert) ||
	    put_user(kern->bitmask, &user->bitmask))
		return -EFAULT;
	return 0;
}
#endif

static struct xt_match ebt_extmark_mt_reg __read_mostly = {
	.name		= "extmark_m",
	.revision	= 0,
	.family		= NFPROTO_BRIDGE,
	.match		= ebt_extmark_mt,
	.checkentry	= ebt_extmark_mt_check,
	.matchsize	= sizeof(struct ebt_extmark_m_info),
#ifdef CONFIG_COMPAT
	.compatsize	= sizeof(struct compat_ebt_extmark_m_info),
	.compat_from_user = extmark_mt_compat_from_user,
	.compat_to_user	= extmark_mt_compat_to_user,
#endif
	.me		= THIS_MODULE,
};

static int __init ebt_extmark_m_init(void)
{
	return xt_register_match(&ebt_extmark_mt_reg);
}

static void __exit ebt_extmark_m_fini(void)
{
	xt_unregister_match(&ebt_extmark_mt_reg);
}

module_init(ebt_extmark_m_init);
module_exit(ebt_extmark_m_fini);
MODULE_DESCRIPTION("Ebtables: Packet extmark match");
MODULE_LICENSE("GPL");
