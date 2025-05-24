/*
 *	xt_connmark - Netfilter module to operate on connection marks
 *
 *	Copyright (C) 2002,2004 MARA Systems AB <http://www.marasystems.com>
 *	by Henrik Nordstrom <hno@marasystems.com>
 *	Copyright © CC Computer Consultants GmbH, 2007 - 2008
 *	Jan Engelhardt <jengelh@medozas.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/>.
 */

#include <linux/module.h>
#include <linux/skbuff.h>
#include <net/netfilter/nf_conntrack.h>
#include <net/netfilter/nf_conntrack_ecache.h>
#include <linux/netfilter/x_tables.h>
#include <linux/netfilter/xt_connmark.h>

MODULE_AUTHOR("Henrik Nordstrom <hno@marasystems.com>");
MODULE_DESCRIPTION("Xtables: connection mark operations");
MODULE_LICENSE("GPL");
MODULE_ALIAS("ipt_CONNMARK");
MODULE_ALIAS("ip6t_CONNMARK");
MODULE_ALIAS("ipt_connmark");
MODULE_ALIAS("ip6t_connmark");

static unsigned int
connmark_tg_v0(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_connmark_target_info *markinfo = par->targinfo;
	struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;
	u_int32_t diff;
	u_int32_t mark;
	u_int32_t newmark;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct) {
		switch(markinfo->mode) {
		case XT_CONNMARK_SET:
			newmark = (ct->mark & ~markinfo->mask) | markinfo->mark;
			if (newmark != ct->mark) {
				ct->mark = newmark;
				nf_conntrack_event_cache(IPCT_MARK, ct);
			}
			break;
		case XT_CONNMARK_SAVE:
			newmark = (ct->mark & ~markinfo->mask) |
				  (skb->mark & markinfo->mask);
			if (ct->mark != newmark) {
				ct->mark = newmark;
				nf_conntrack_event_cache(IPCT_MARK, ct);
			}
			break;
		case XT_CONNMARK_RESTORE:
			mark = skb->mark;
			diff = (ct->mark ^ mark) & markinfo->mask;
			skb->mark = mark ^ diff;
			break;
		}
	}

	return XT_CONTINUE;
}

static unsigned int
connmark_tg(struct sk_buff *skb, const struct xt_action_param *par)
{
	const struct xt_connmark_tginfo1 *info = par->targinfo;
	enum ip_conntrack_info ctinfo;
	struct nf_conn *ct;
	u_int32_t newmark;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct == NULL)
		return XT_CONTINUE;

	switch (info->mode) {
	case XT_CONNMARK_SET:
		newmark = (ct->mark & ~info->ctmask) ^ info->ctmark;
		if (ct->mark != newmark) {
			ct->mark = newmark;
			nf_conntrack_event_cache(IPCT_MARK, ct);
		}
		break;
	case XT_CONNMARK_SAVE:
		newmark = (ct->mark & ~info->ctmask) ^
		          (skb->mark & info->nfmask);
		if (ct->mark != newmark) {
			ct->mark = newmark;
			nf_conntrack_event_cache(IPCT_MARK, ct);
		}
		break;
	case XT_CONNMARK_RESTORE:
		newmark = (skb->mark & ~info->nfmask) ^
		          (ct->mark & info->ctmask);
		skb->mark = newmark;
		break;
	}

	return XT_CONTINUE;
}

static int connmark_tg_check_v0(const struct xt_tgchk_param *par)
{
	const struct xt_connmark_target_info *matchinfo = par->targinfo;

	if (matchinfo->mode == XT_CONNMARK_RESTORE) {
		if (strcmp(par->table, "mangle") != 0) {
			printk(KERN_WARNING "CONNMARK: restore can only be "
			       "called from \"mangle\" table, not \"%s\"\n",
			       par->table);
			return -EINVAL;
		}
	}
	if (matchinfo->mark > 0xffffffff || matchinfo->mask > 0xffffffff) {
		printk(KERN_WARNING "CONNMARK: Only supports 32bit mark\n");
		return -EINVAL;
	}
	if (nf_ct_l3proto_try_module_get(par->family) < 0) {
		printk(KERN_WARNING "can't load conntrack support for "
				    "proto=%u\n", par->family);
		return -EINVAL;
	}
	return 0;
}

static int connmark_tg_check(const struct xt_tgchk_param *par)
{
	int ret;

	ret = nf_ct_l3proto_try_module_get(par->family);
	if (ret < 0)
		pr_info("cannot load conntrack support for proto=%u\n",
			par->family);
	return ret;
}

static void connmark_tg_destroy(const struct xt_tgdtor_param *par)
{
	nf_ct_l3proto_module_put(par->family);
}

static bool
connmark_mt(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_connmark_mtinfo1 *info = par->matchinfo;
	enum ip_conntrack_info ctinfo;
	const struct nf_conn *ct;

	ct = nf_ct_get(skb, &ctinfo);
	if (ct == NULL)
		return false;

	return ((ct->mark & info->mask) == info->mark) ^ info->invert;
}


static bool
connmark_mt_v0(const struct sk_buff *skb, struct xt_action_param *par)
{
	const struct xt_connmark_info *info = par->matchinfo;
	const struct nf_conn *ct;
	enum ip_conntrack_info ctinfo;

	ct = nf_ct_get(skb, &ctinfo);
	if (!ct)
		return false;

	return ((ct->mark & info->mask) == info->mark) ^ info->invert;
}

static int connmark_mt_check_v0(const struct xt_mtchk_param *par)
{
	const struct xt_connmark_info *cm = par->matchinfo;

	if (cm->mark > 0xffffffff || cm->mask > 0xffffffff) {
		printk(KERN_WARNING "connmark: only support 32bit mark\n");
		return -EINVAL;
	}
	if (nf_ct_l3proto_try_module_get(par->family) < 0) {
		printk(KERN_WARNING "can't load conntrack support for "
				    "proto=%u\n", par->family);
		return -EINVAL;
	}
	return 0;
}


static int connmark_mt_check(const struct xt_mtchk_param *par)
{
	int ret;

	ret = nf_ct_l3proto_try_module_get(par->family);
	if (ret < 0)
		pr_info("cannot load conntrack support for proto=%u\n",
			par->family);
	return ret;
}

static void connmark_mt_destroy(const struct xt_mtdtor_param *par)
{
	nf_ct_l3proto_module_put(par->family);
}

#ifdef CONFIG_COMPAT
struct compat_xt_connmark_target_info {
	compat_ulong_t	mark, mask;
	u_int8_t	mode;
	u_int8_t	__pad1;
	u_int16_t	__pad2;
};

static void connmark_tg_compat_from_user_v0(void *dst, const void *src)
{
	const struct compat_xt_connmark_target_info *cm = src;
	struct xt_connmark_target_info m = {
		.mark	= cm->mark,
		.mask	= cm->mask,
		.mode	= cm->mode,
	};
	memcpy(dst, &m, sizeof(m));
}

static int connmark_tg_compat_to_user_v0(void __user *dst, const void *src)
{
	const struct xt_connmark_target_info *m = src;
	struct compat_xt_connmark_target_info cm = {
		.mark	= m->mark,
		.mask	= m->mask,
		.mode	= m->mode,
	};
	return copy_to_user(dst, &cm, sizeof(cm)) ? -EFAULT : 0;
}

struct compat_xt_connmark_info {
	compat_ulong_t	mark, mask;
	u_int8_t	invert;
	u_int8_t	__pad1;
	u_int16_t	__pad2;
 };
 
static void connmark_mt_compat_from_user_v0(void *dst, const void *src)
{
	const struct compat_xt_connmark_info *cm = src;
	struct xt_connmark_info m = {
		.mark	= cm->mark,
		.mask	= cm->mask,
		.invert	= cm->invert,
	};
	memcpy(dst, &m, sizeof(m));
}

static int connmark_mt_compat_to_user_v0(void __user *dst, const void *src)
{
	const struct xt_connmark_info *m = src;
	struct compat_xt_connmark_info cm = {
		.mark	= m->mark,
		.mask	= m->mask,
		.invert	= m->invert,
	};
	return copy_to_user(dst, &cm, sizeof(cm)) ? -EFAULT : 0;
}


#endif /* CONFIG_COMPAT */


static struct xt_target connmark_tg_reg[] __read_mostly = {
	{
		.name		= "CONNMARK",
		.revision	= 0,
		.family		= NFPROTO_UNSPEC,
		.checkentry	= connmark_tg_check_v0,
		.destroy	= connmark_tg_destroy,
		.target		= connmark_tg_v0,
		.targetsize	= sizeof(struct xt_connmark_target_info),
#ifdef CONFIG_COMPAT
		.compatsize	= sizeof(struct compat_xt_connmark_target_info),
		.compat_from_user = connmark_tg_compat_from_user_v0,
		.compat_to_user	= connmark_tg_compat_to_user_v0,
#endif
		.me		= THIS_MODULE
	},
{	.name           = "CONNMARK",
	.revision       = 1,
	.family         = NFPROTO_UNSPEC,
	.checkentry     = connmark_tg_check,
	.target         = connmark_tg,
	.targetsize     = sizeof(struct xt_connmark_tginfo1),
	.destroy        = connmark_tg_destroy,
	.me             = THIS_MODULE,
}
};

static struct xt_match connmark_mt_reg[] __read_mostly = {
	{
		.name		= "connmark",
		.revision	= 0,
		.family		= NFPROTO_UNSPEC,
		.checkentry	= connmark_mt_check_v0,
		.match		= connmark_mt_v0,
		.destroy	= connmark_mt_destroy,
		.matchsize	= sizeof(struct xt_connmark_info),
#ifdef CONFIG_COMPAT
		.compatsize	= sizeof(struct compat_xt_connmark_info),
		.compat_from_user = connmark_mt_compat_from_user_v0,
		.compat_to_user	= connmark_mt_compat_to_user_v0,
#endif
		.me		= THIS_MODULE
	},	{
		.name           = "connmark",
		.revision       = 1,
		.family         = NFPROTO_UNSPEC,
		.checkentry     = connmark_mt_check,
		.match          = connmark_mt,
		.matchsize      = sizeof(struct xt_connmark_mtinfo1),
		.destroy        = connmark_mt_destroy,
		.me             = THIS_MODULE,
	},
};

static int __init connmark_mt_init(void)
{
	int ret;

	ret = xt_register_targets(connmark_tg_reg,
	       ARRAY_SIZE(connmark_tg_reg));
	if (ret < 0)
		return ret;
	ret = xt_register_matches(connmark_mt_reg,
	       ARRAY_SIZE(connmark_mt_reg));
	if (ret < 0) {
		xt_unregister_targets(connmark_tg_reg, ARRAY_SIZE(connmark_tg_reg));
		return ret;
	}
	return 0;
}

static void __exit connmark_mt_exit(void)
{
	xt_unregister_matches(connmark_mt_reg, ARRAY_SIZE(connmark_mt_reg));
	xt_unregister_targets(connmark_tg_reg, ARRAY_SIZE(connmark_tg_reg));
}

module_init(connmark_mt_init);
module_exit(connmark_mt_exit);
