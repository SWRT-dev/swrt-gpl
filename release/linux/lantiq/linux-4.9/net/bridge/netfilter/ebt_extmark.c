/*
 *  ebt_extmark
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

/* The extmark target can be used in any chain,
 * I believe adding a mangle table just for extmarking is total overkill.
 * Marking a frame doesn't really change anything in the frame anyway.
 */

#include <linux/module.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter_bridge/ebtables.h>
#include <linux/netfilter_bridge/ebt_extmark_t.h>

static unsigned int
ebt_extmark_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct ebt_extmark_t_info *info = par->targinfo;
	int action = info->target & -16;

	if (action == EXTMARK_SET_VALUE)
		skb->extmark = info->extmark;
	else if (action == EXTMARK_OR_VALUE)
		skb->extmark |= info->extmark;
	else if (action == EXTMARK_AND_VALUE)
		skb->extmark &= info->extmark;
	else
		skb->extmark ^= info->extmark;

	return info->target | ~EBT_VERDICT_BITS;
}

static int ebt_extmark_tg_check(const struct xt_tgchk_param *par)
{
	const struct ebt_extmark_t_info *info = par->targinfo;
	int tmp;

	tmp = info->target | ~EBT_VERDICT_BITS;
	if (BASE_CHAIN && tmp == EBT_RETURN)
		return -EINVAL;
	if (tmp < -NUM_STANDARD_TARGETS || tmp >= 0)
		return -EINVAL;
	tmp = info->target & ~EBT_VERDICT_BITS;
	if (tmp != EXTMARK_SET_VALUE && tmp != EXTMARK_OR_VALUE &&
	    tmp != EXTMARK_AND_VALUE && tmp != EXTMARK_XOR_VALUE)
		return -EINVAL;
	return 0;
}

#ifdef CONFIG_COMPAT
struct compat_ebt_extmark_t_info {
	compat_ulong_t extmark;
	compat_uint_t target;
};

static void extmark_tg_compat_from_user(void *dst, const void *src)
{
	const struct compat_ebt_extmark_t_info *user = src;
	struct ebt_extmark_t_info *kern = dst;

	kern->extmark = user->extmark;
	kern->target = user->target;
}

static int extmark_tg_compat_to_user(void __user *dst, const void *src)
{
	struct compat_ebt_extmark_t_info __user *user = dst;
	const struct ebt_extmark_t_info *kern = src;

	if (put_user(kern->extmark, &user->extmark) ||
	    put_user(kern->target, &user->target))
		return -EFAULT;
	return 0;
}
#endif

static struct xt_target ebt_extmark_tg_reg __read_mostly = {
	.name		= "extmark",
	.revision	= 0,
	.family		= NFPROTO_BRIDGE,
	.target		= ebt_extmark_tg,
	.checkentry	= ebt_extmark_tg_check,
	.targetsize	= sizeof(struct ebt_extmark_t_info),
#ifdef CONFIG_COMPAT
	.compatsize	= sizeof(struct compat_ebt_extmark_t_info),
	.compat_from_user = extmark_tg_compat_from_user,
	.compat_to_user	= extmark_tg_compat_to_user,
#endif
	.me		= THIS_MODULE,
};

static int __init ebt_extmark_init(void)
{
	return xt_register_target(&ebt_extmark_tg_reg);
}

static void __exit ebt_extmark_fini(void)
{
	xt_unregister_target(&ebt_extmark_tg_reg);
}

module_init(ebt_extmark_init);
module_exit(ebt_extmark_fini);
MODULE_DESCRIPTION("Ebtables: Packet extmark modification");
MODULE_LICENSE("GPL");
