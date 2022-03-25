// SPDX-License-Identifier: GPL-2.0
#include <linux/module.h>
#include <linux/init.h>
#include <linux/kernel.h>
#include <linux/skbuff.h>
#include <linux/rtnetlink.h>
#include <net/netlink.h>
#include <net/pkt_sched.h>

#include <linux/tc_act/tc_colmark.h>
#include <net/tc_act/tc_colmark.h>

#define COLMARK_TAB_MASK	256

static unsigned int colmark_net_id;
static struct tc_action_ops act_colmark_ops;

static int tcf_colmark(struct sk_buff *skb, const struct tc_action *a,
		       struct tcf_result *res)
{
	struct tcf_colmark *d = to_colmark(a);

	/* TODO: add real implementation in the future. Currently only offload
	 * path is supported
	 */

	return d->tcf_action;
}

static const struct nla_policy colmark_policy[TCA_COLMARK_MAX + 1] = {
	[TCA_COLMARK_PARMS]		= { .len = sizeof(struct tc_colmark) },
	[TCA_COLMARK_MODE]		= { .len = NLA_U8 },
	[TCA_COLMARK_DROP_PRECEDENCE]	= { .len = NLA_U8 },
	[TCA_COLMARK_METER_TYPE]	= { .len = NLA_U8 },
};

static int tcf_colmark_init(struct net *net, struct nlattr *nla,
			    struct nlattr *est, struct tc_action **a,
			    int ovr, int bind)
{
	struct tc_action_net *tn = net_generic(net, colmark_net_id);
	struct nlattr *tb[TCA_COLMARK_MAX + 1];
	struct tc_colmark *parm;
	struct tcf_colmark *d;
	u32 flags = 0;
	u8 *mode = NULL, *precedence = NULL, *type = NULL;
	bool exists = false;
	int ret = 0, err;

	if (!nla)
		return -EINVAL;

	err = nla_parse_nested(tb, TCA_COLMARK_MAX, nla, colmark_policy);
	if (err < 0)
		return err;

	if (!tb[TCA_COLMARK_PARMS])
		return -EINVAL;

	if (tb[TCA_COLMARK_MODE]) {
		flags |= COLMARK_F_MODE;
		mode = nla_data(tb[TCA_COLMARK_MODE]);
	}

	if (tb[TCA_COLMARK_DROP_PRECEDENCE]) {
		flags |= COLMARK_F_DROP_PRECEDENCE;
		precedence = nla_data(tb[TCA_COLMARK_DROP_PRECEDENCE]);
	}

	if (tb[TCA_COLMARK_METER_TYPE]) {
		flags |= COLMARK_F_METER_TYPE;
		type = nla_data(tb[TCA_COLMARK_METER_TYPE]);
	}

	parm = nla_data(tb[TCA_COLMARK_PARMS]);

	exists = tcf_hash_check(tn, parm->index, a, bind);
	if (exists && bind)
		return 0;

	if (!flags) {
		if (exists)
			tcf_hash_release(*a, bind);
		return -EINVAL;
	}

	if (!exists) {
		ret = tcf_hash_create(tn, parm->index, est, a,
				      &act_colmark_ops, bind, false);
		if (ret)
			return ret;

		d = to_colmark(*a);
		ret = ACT_P_CREATED;
	} else {
		d = to_colmark(*a);
		tcf_hash_release(*a, bind);
		if (!ovr)
			return -EEXIST;
	}

	d->flags = flags;
	if (flags & COLMARK_F_MODE)
		d->blind = *mode  ? true : false;
	if (flags & COLMARK_F_DROP_PRECEDENCE)
		d->precedence = *precedence;
	if (flags & COLMARK_F_METER_TYPE)
		d->type = *type;

	d->tcf_action = parm->action;

	if (ret == ACT_P_CREATED)
		tcf_hash_insert(tn, *a);
	return ret;
}

static int tcf_colmark_dump(struct sk_buff *skb, struct tc_action *a,
			    int bind, int ref)
{
	unsigned char *b = skb_tail_pointer(skb);
	struct tcf_colmark *d = to_colmark(a);
	struct tc_colmark opt = {
		.index   = d->tcf_index,
		.refcnt  = d->tcf_refcnt - ref,
		.bindcnt = d->tcf_bindcnt - bind,
		.action  = d->tcf_action,
	};
	struct tcf_t t;

	if (nla_put(skb, TCA_COLMARK_PARMS, sizeof(opt), &opt))
		goto nla_put_failure;
	if ((d->flags & COLMARK_F_MODE) &&
	    nla_put_u8(skb, TCA_COLMARK_MODE, d->blind ? 1 : 0))
		goto nla_put_failure;
	if ((d->flags & COLMARK_F_DROP_PRECEDENCE) &&
	    nla_put_u8(skb, TCA_COLMARK_DROP_PRECEDENCE, d->precedence))
		goto nla_put_failure;
	if ((d->flags & COLMARK_F_METER_TYPE) &&
	    nla_put_u8(skb, TCA_COLMARK_METER_TYPE, d->type))
		goto nla_put_failure;

	tcf_tm_dump(&t, &d->tcf_tm);
	if (nla_put_64bit(skb, TCA_COLMARK_TM, sizeof(t), &t, TCA_COLMARK_PAD))
		goto nla_put_failure;
	return skb->len;

nla_put_failure:
	nlmsg_trim(skb, b);
	return -1;
}

static int tcf_colmark_walker(struct net *net, struct sk_buff *skb,
			      struct netlink_callback *cb, int type,
			      const struct tc_action_ops *ops)
{
	struct tc_action_net *tn = net_generic(net, colmark_net_id);

	return tcf_generic_walker(tn, skb, cb, type, ops);
}

static int tcf_colmark_search(struct net *net, struct tc_action **a, u32 index)
{
	struct tc_action_net *tn = net_generic(net, colmark_net_id);

	return tcf_hash_search(tn, a, index);
}

static struct tc_action_ops act_colmark_ops = {
	.kind		=	"colmark",
	.type		=	TCA_ACT_COLMARK,
	.owner		=	THIS_MODULE,
	.act		=	tcf_colmark,
	.dump		=	tcf_colmark_dump,
	.init		=	tcf_colmark_init,
	.walk		=	tcf_colmark_walker,
	.lookup		=	tcf_colmark_search,
	.size		=	sizeof(struct tcf_colmark),
};

static __net_init int colmark_init_net(struct net *net)
{
	struct tc_action_net *tn = net_generic(net, colmark_net_id);

	return tc_action_net_init(tn, &act_colmark_ops, COLMARK_TAB_MASK);
}

static void __net_exit colmark_exit_net(struct net *net)
{
	struct tc_action_net *tn = net_generic(net, colmark_net_id);

	tc_action_net_exit(tn);
}

static struct pernet_operations colmark_net_ops = {
	.init = colmark_init_net,
	.exit = colmark_exit_net,
	.id   = &colmark_net_id,
	.size = sizeof(struct tc_action_net),
};

static int __init colmark_init_module(void)
{
	return tcf_register_action(&act_colmark_ops, &colmark_net_ops);
}

static void __exit colmark_cleanup_module(void)
{
	tcf_unregister_action(&act_colmark_ops, &colmark_net_ops);
}

module_init(colmark_init_module);
module_exit(colmark_cleanup_module);

MODULE_DESCRIPTION("Packet color marking");
MODULE_LICENSE("GPL");
