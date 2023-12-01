// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2018 MediaTek Inc.
 * Author: Sirui Zhao <Sirui.Zhao@mediatek.com>
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <net/genetlink.h>
#include <linux/of_mdio.h>
#include <linux/phylink.h>
#include <net/dsa.h>

#include "mt7530.h"
#include "mt7530_nl.h"

struct mt7530_nl_cmd_item {
	enum mt7530_cmd cmd;
	bool require_dev;
	int (*process)(struct genl_info *info);
	u32 nr_required_attrs;
	const enum mt7530_attr *required_attrs;
};

struct mt7530_priv *sw_priv;

static DEFINE_MUTEX(mt7530_devs_lock);

void mt7530_put(void)
{
	mutex_unlock(&mt7530_devs_lock);
}

void mt7530_lock(void)
{
	mutex_lock(&mt7530_devs_lock);
}

static int mt7530_nl_response(struct sk_buff *skb, struct genl_info *info);

static const struct nla_policy mt7530_nl_cmd_policy[] = {
	[MT7530_ATTR_TYPE_MESG] = { .type = NLA_STRING },
	[MT7530_ATTR_TYPE_PHY] = { .type = NLA_S32 },
	[MT7530_ATTR_TYPE_REG] = { .type = NLA_S32 },
	[MT7530_ATTR_TYPE_VAL] = { .type = NLA_S32 },
	[MT7530_ATTR_TYPE_DEV_NAME] = { .type = NLA_S32 },
	[MT7530_ATTR_TYPE_DEV_ID] = { .type = NLA_S32 },
	[MT7530_ATTR_TYPE_DEVAD] = { .type = NLA_S32 },
};

static const struct genl_ops mt7530_nl_ops[] = {
	{
		.cmd = MT7530_CMD_REQUEST,
		.doit = mt7530_nl_response,
		.flags = GENL_ADMIN_PERM,
	}, {
		.cmd = MT7530_CMD_READ,
		.doit = mt7530_nl_response,
		.flags = GENL_ADMIN_PERM,
	}, {
		.cmd = MT7530_CMD_WRITE,
		.doit = mt7530_nl_response,
		.flags = GENL_ADMIN_PERM,
	},
};

static struct genl_family mt7530_nl_family = {
	.name =		MT7530_DSA_GENL_NAME,
	.version =	MT7530_GENL_VERSION,
	.maxattr =	MT7530_NR_ATTR_TYPE,
	.ops =		mt7530_nl_ops,
	.n_ops =	ARRAY_SIZE(mt7530_nl_ops),
	.policy =	mt7530_nl_cmd_policy,
};

static int mt7530_nl_prepare_reply(struct genl_info *info, u8 cmd,
				   struct sk_buff **skbp)
{
	struct sk_buff *msg;
	void *reply;

	if (!info)
		return -EINVAL;

	msg = genlmsg_new(NLMSG_GOODSIZE, GFP_KERNEL);
	if (!msg)
		return -ENOMEM;

	/* Construct send-back message header */
	reply = genlmsg_put(msg, info->snd_portid, info->snd_seq,
			    &mt7530_nl_family, 0, cmd);
	if (!reply) {
		nlmsg_free(msg);
		return -EINVAL;
	}

	*skbp = msg;
	return 0;
}

static int mt7530_nl_send_reply(struct sk_buff *skb, struct genl_info *info)
{
	struct genlmsghdr *genlhdr = nlmsg_data(nlmsg_hdr(skb));
	void *reply = genlmsg_data(genlhdr);

	/* Finalize a generic netlink message (update message header) */
	genlmsg_end(skb, reply);

	/* reply to a request */
	return genlmsg_reply(skb, info);
}

static s32 mt7530_nl_get_s32(struct genl_info *info, enum mt7530_attr attr,
			     s32 defval)
{
	struct nlattr *na;

	na = info->attrs[attr];
	if (na)
		return nla_get_s32(na);

	return defval;
}

static int mt7530_nl_get_u32(struct genl_info *info, enum mt7530_attr attr,
			     u32 *val)
{
	struct nlattr *na;

	na = info->attrs[attr];
	if (na) {
		*val = nla_get_u32(na);
		return 0;
	}

	return -1;
}

static int mt7530_nl_reply_read(struct genl_info *info)
{
	struct sk_buff *rep_skb = NULL;
	s32 phy, devad, reg;
	int value;
	int ret = 0;

	phy = mt7530_nl_get_s32(info, MT7530_ATTR_TYPE_PHY, -1);
	devad = mt7530_nl_get_s32(info, MT7530_ATTR_TYPE_DEVAD, -1);
	reg = mt7530_nl_get_s32(info, MT7530_ATTR_TYPE_REG, -1);

	if (reg < 0)
		goto err;

	ret = mt7530_nl_prepare_reply(info, MT7530_CMD_READ, &rep_skb);
	if (ret < 0)
		goto err;

	if (phy >= 0) {
		if (devad < 0)
			value = mt7531_ind_c22_phy_read(sw_priv, phy, reg);
		else
			value = mt7531_ind_c45_phy_read(sw_priv, phy, devad, reg);
	} else
		value = mt7530_read(sw_priv, reg);

	ret = nla_put_s32(rep_skb, MT7530_ATTR_TYPE_REG, reg);
	if (ret < 0)
		goto err;

	ret = nla_put_s32(rep_skb, MT7530_ATTR_TYPE_VAL, value);
	if (ret < 0)
		goto err;

	return mt7530_nl_send_reply(rep_skb, info);

err:
	if (rep_skb)
		nlmsg_free(rep_skb);

	return ret;
}

static int mt7530_nl_reply_write(struct genl_info *info)
{
	struct sk_buff *rep_skb = NULL;
	s32 phy, devad, reg;
	u32 value;
	int ret = 0;

	phy = mt7530_nl_get_s32(info, MT7530_ATTR_TYPE_PHY, -1);
	devad = mt7530_nl_get_s32(info, MT7530_ATTR_TYPE_DEVAD, -1);
	reg = mt7530_nl_get_s32(info, MT7530_ATTR_TYPE_REG, -1);

	if (mt7530_nl_get_u32(info, MT7530_ATTR_TYPE_VAL, &value))
		goto err;

	if (reg < 0)
		goto err;

	ret = mt7530_nl_prepare_reply(info, MT7530_CMD_WRITE, &rep_skb);
	if (ret < 0)
		goto err;

	if (phy >= 0) {
		if (devad < 0)
			mt7531_ind_c22_phy_write(sw_priv, phy, reg, value);
		else
			mt7531_ind_c45_phy_write(sw_priv, phy, devad, reg, value);
	} else
		mt7530_write(sw_priv, reg, value);

	ret = nla_put_s32(rep_skb, MT7530_ATTR_TYPE_REG, reg);
	if (ret < 0)
		goto err;

	ret = nla_put_s32(rep_skb, MT7530_ATTR_TYPE_VAL, value);
	if (ret < 0)
		goto err;

	return mt7530_nl_send_reply(rep_skb, info);

err:
	if (rep_skb)
		nlmsg_free(rep_skb);

	return ret;
}

static const enum mt7530_attr mt7530_nl_cmd_read_attrs[] = {
	MT7530_ATTR_TYPE_REG
};

static const enum mt7530_attr mt7530_nl_cmd_write_attrs[] = {
	MT7530_ATTR_TYPE_REG,
	MT7530_ATTR_TYPE_VAL
};

static const struct mt7530_nl_cmd_item mt7530_nl_cmds[] = {
	{
		.cmd = MT7530_CMD_READ,
		.require_dev = true,
		.process = mt7530_nl_reply_read,
		.required_attrs = mt7530_nl_cmd_read_attrs,
		.nr_required_attrs = ARRAY_SIZE(mt7530_nl_cmd_read_attrs),
	}, {
		.cmd = MT7530_CMD_WRITE,
		.require_dev = true,
		.process = mt7530_nl_reply_write,
		.required_attrs = mt7530_nl_cmd_write_attrs,
		.nr_required_attrs = ARRAY_SIZE(mt7530_nl_cmd_write_attrs),
	}
};

static int mt7530_nl_response(struct sk_buff *skb, struct genl_info *info)
{
	struct genlmsghdr *hdr = nlmsg_data(info->nlhdr);
	const struct mt7530_nl_cmd_item *cmditem = NULL;
	u32 sat_req_attrs = 0;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(mt7530_nl_cmds); i++) {
		if (hdr->cmd == mt7530_nl_cmds[i].cmd) {
			cmditem = &mt7530_nl_cmds[i];
			break;
		}
	}

	if (!cmditem) {
		pr_info("mt7530-nl: unknown cmd %u\n", hdr->cmd);
		return -EINVAL;
	}

	for (i = 0; i < cmditem->nr_required_attrs; i++) {
		if (info->attrs[cmditem->required_attrs[i]])
			sat_req_attrs++;
	}

	if (sat_req_attrs != cmditem->nr_required_attrs) {
		pr_info("mt7530-nl: missing required attr(s) for cmd %u\n",
			hdr->cmd);
		return -EINVAL;
	}

	ret = cmditem->process(info);

	mt7530_put();

	return ret;
}

int mt7530_nl_init(struct mt7530_priv **priv)
{
	int ret;

	pr_info("mt7530-nl: genl_register_family_with_ops \n");

	sw_priv = *priv;
	ret = genl_register_family(&mt7530_nl_family);
	if (ret) {
		return ret;
	}

	return 0;
}

void mt7530_nl_exit()
{
	sw_priv = NULL;
	genl_unregister_family(&mt7530_nl_family);
}
