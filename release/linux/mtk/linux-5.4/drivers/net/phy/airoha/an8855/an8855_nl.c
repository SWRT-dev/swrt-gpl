// SPDX-License-Identifier: GPL-2.0
/*
 * Copyright (c) 2023 Airoha Inc.
 * Author: Min Yao <min.yao@airoha.com>
 */

#include <linux/types.h>
#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/init.h>
#include <net/genetlink.h>

#include "an8855.h"
#include "an8855_nl.h"

struct an8855_nl_cmd_item {
	enum an8855_cmd cmd;
	bool require_dev;
	int (*process)(struct genl_info *info, struct gsw_an8855 *gsw);
	u32 nr_required_attrs;
	const enum an8855_attr *required_attrs;
};

static int an8855_nl_response(struct sk_buff *skb, struct genl_info *info);

static const struct nla_policy an8855_nl_cmd_policy[] = {
	[AN8855_ATTR_TYPE_MESG] = { .type = NLA_STRING },
	[AN8855_ATTR_TYPE_PHY] = { .type = NLA_S32 },
	[AN8855_ATTR_TYPE_REG] = { .type = NLA_U32 },
	[AN8855_ATTR_TYPE_VAL] = { .type = NLA_U32 },
	[AN8855_ATTR_TYPE_DEV_NAME] = { .type = NLA_S32 },
	[AN8855_ATTR_TYPE_DEV_ID] = { .type = NLA_S32 },
	[AN8855_ATTR_TYPE_DEVAD] = { .type = NLA_S32 },
};

static const struct genl_ops an8855_nl_ops[] = {
	{
		.cmd = AN8855_CMD_REQUEST,
		.doit = an8855_nl_response,
//		.policy = an8855_nl_cmd_policy,
		.flags = GENL_ADMIN_PERM,
	}, {
		.cmd = AN8855_CMD_READ,
		.doit = an8855_nl_response,
//		.policy = an8855_nl_cmd_policy,
		.flags = GENL_ADMIN_PERM,
	}, {
		.cmd = AN8855_CMD_WRITE,
		.doit = an8855_nl_response,
//		.policy = an8855_nl_cmd_policy,
		.flags = GENL_ADMIN_PERM,
	},
};

static struct genl_family an8855_nl_family = {
	.name =		AN8855_GENL_NAME,
	.version =	AN8855_GENL_VERSION,
	.maxattr =	AN8855_NR_ATTR_TYPE,
	.ops =		an8855_nl_ops,
	.n_ops =	ARRAY_SIZE(an8855_nl_ops),
	.policy =	an8855_nl_cmd_policy,
};

static int an8855_nl_list_devs(char *buff, int size)
{
	struct gsw_an8855 *gsw;
	int len, total = 0;
	char buf[80];

	memset(buff, 0, size);

	an8855_lock_gsw();

	list_for_each_entry(gsw, &an8855_devs, list) {
		len = snprintf(buf, sizeof(buf),
				   "id: %d, model: %s, node: %s\n",
				   gsw->id, gsw->name, gsw->dev->of_node->name);
		if (len == strlen(buf)) {
			if (size - total > 0)
				strncat(buff, buf, size - total);
			total += len;
		}
	}

	an8855_put_gsw();

	return total;
}

static int an8855_nl_prepare_reply(struct genl_info *info, u8 cmd,
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
				&an8855_nl_family, 0, cmd);
	if (!reply) {
		nlmsg_free(msg);
		return -EINVAL;
	}

	*skbp = msg;
	return 0;
}

static int an8855_nl_send_reply(struct sk_buff *skb, struct genl_info *info)
{
	struct genlmsghdr *genlhdr = nlmsg_data(nlmsg_hdr(skb));
	void *reply = genlmsg_data(genlhdr);

	/* Finalize a generic netlink message (update message header) */
	genlmsg_end(skb, reply);

	/* reply to a request */
	return genlmsg_reply(skb, info);
}

static s32 an8855_nl_get_s32(struct genl_info *info, enum an8855_attr attr,
				 s32 defval)
{
	struct nlattr *na;

	na = info->attrs[attr];
	if (na)
		return nla_get_s32(na);

	return defval;
}

static int an8855_nl_get_u32(struct genl_info *info, enum an8855_attr attr,
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

static struct gsw_an8855 *an8855_nl_parse_find_gsw(struct genl_info *info)
{
	struct gsw_an8855 *gsw;
	struct nlattr *na;
	int gsw_id;

	na = info->attrs[AN8855_ATTR_TYPE_DEV_ID];
	if (na) {
		gsw_id = nla_get_s32(na);
		if (gsw_id >= 0)
			gsw = an8855_get_gsw(gsw_id);
		else
			gsw = an8855_get_first_gsw();
	} else {
		gsw = an8855_get_first_gsw();
	}

	return gsw;
}

static int an8855_nl_get_swdevs(struct genl_info *info, struct gsw_an8855 *gsw)
{
	struct sk_buff *rep_skb = NULL;
	char dev_info[512];
	int ret;

	ret = an8855_nl_list_devs(dev_info, sizeof(dev_info) - 1);
	if (!ret) {
		pr_info("No switch registered\n");
		return -EINVAL;
	}

	ret = an8855_nl_prepare_reply(info, AN8855_CMD_REPLY, &rep_skb);
	if (ret < 0)
		goto err;

	ret = nla_put_string(rep_skb, AN8855_ATTR_TYPE_MESG, dev_info);
	if (ret < 0)
		goto err;

	return an8855_nl_send_reply(rep_skb, info);

err:
	if (rep_skb)
		nlmsg_free(rep_skb);

	return ret;
}

static int an8855_nl_reply_read(struct genl_info *info, struct gsw_an8855 *gsw)
{
	struct sk_buff *rep_skb = NULL;
	s32 phy, devad;
	u32 reg = 0;
	int value = 0;
	int ret = 0;

	phy = an8855_nl_get_s32(info, AN8855_ATTR_TYPE_PHY, -1);
	devad = an8855_nl_get_s32(info, AN8855_ATTR_TYPE_DEVAD, -1);

	if (an8855_nl_get_u32(info, AN8855_ATTR_TYPE_REG, &reg))
		goto err;

	ret = an8855_nl_prepare_reply(info, AN8855_CMD_READ, &rep_skb);
	if (ret < 0)
		goto err;

	if (phy >= 0) {
		if (devad < 0)
			value = gsw->mii_read(gsw, phy, reg);
		else
			value = gsw->mmd_read(gsw, phy, devad, reg);
	} else {
		value = an8855_reg_read(gsw, reg);
	}

	ret = nla_put_u32(rep_skb, AN8855_ATTR_TYPE_REG, reg);
	if (ret < 0)
		goto err;

	ret = nla_put_u32(rep_skb, AN8855_ATTR_TYPE_VAL, value);
	if (ret < 0)
		goto err;

	return an8855_nl_send_reply(rep_skb, info);

err:
	if (rep_skb)
		nlmsg_free(rep_skb);

	return ret;
}

static int an8855_nl_reply_write(struct genl_info *info, struct gsw_an8855 *gsw)
{
	struct sk_buff *rep_skb = NULL;
	s32 phy, devad;
	u32 value = 0, reg = 0;
	int ret = 0;

	phy = an8855_nl_get_s32(info, AN8855_ATTR_TYPE_PHY, -1);
	devad = an8855_nl_get_s32(info, AN8855_ATTR_TYPE_DEVAD, -1);
	if (an8855_nl_get_u32(info, AN8855_ATTR_TYPE_REG, &reg))
		goto err;

	if (an8855_nl_get_u32(info, AN8855_ATTR_TYPE_VAL, &value))
		goto err;

	ret = an8855_nl_prepare_reply(info, AN8855_CMD_WRITE, &rep_skb);
	if (ret < 0)
		goto err;

	if (phy >= 0) {
		if (devad < 0)
			gsw->mii_write(gsw, phy, reg, value);
		else
			gsw->mmd_write(gsw, phy, devad, reg, value);
	} else {
		an8855_reg_write(gsw, reg, value);
	}

	ret = nla_put_u32(rep_skb, AN8855_ATTR_TYPE_REG, reg);
	if (ret < 0)
		goto err;

	ret = nla_put_u32(rep_skb, AN8855_ATTR_TYPE_VAL, value);
	if (ret < 0)
		goto err;

	return an8855_nl_send_reply(rep_skb, info);

err:
	if (rep_skb)
		nlmsg_free(rep_skb);

	return ret;
}

static const enum an8855_attr an8855_nl_cmd_read_attrs[] = {
	AN8855_ATTR_TYPE_REG
};

static const enum an8855_attr an8855_nl_cmd_write_attrs[] = {
	AN8855_ATTR_TYPE_REG,
	AN8855_ATTR_TYPE_VAL
};

static const struct an8855_nl_cmd_item an8855_nl_cmds[] = {
	{
		.cmd = AN8855_CMD_REQUEST,
		.require_dev = false,
		.process = an8855_nl_get_swdevs
	}, {
		.cmd = AN8855_CMD_READ,
		.require_dev = true,
		.process = an8855_nl_reply_read,
		.required_attrs = an8855_nl_cmd_read_attrs,
		.nr_required_attrs = ARRAY_SIZE(an8855_nl_cmd_read_attrs),
	}, {
		.cmd = AN8855_CMD_WRITE,
		.require_dev = true,
		.process = an8855_nl_reply_write,
		.required_attrs = an8855_nl_cmd_write_attrs,
		.nr_required_attrs = ARRAY_SIZE(an8855_nl_cmd_write_attrs),
	}
};

static int an8855_nl_response(struct sk_buff *skb, struct genl_info *info)
{
	struct genlmsghdr *hdr = nlmsg_data(info->nlhdr);
	const struct an8855_nl_cmd_item *cmditem = NULL;
	struct gsw_an8855 *gsw = NULL;
	u32 sat_req_attrs = 0;
	int i, ret;

	for (i = 0; i < ARRAY_SIZE(an8855_nl_cmds); i++) {
		if (hdr->cmd == an8855_nl_cmds[i].cmd) {
			cmditem = &an8855_nl_cmds[i];
			break;
		}
	}

	if (!cmditem) {
		pr_info("an8855-nl: unknown cmd %u\n", hdr->cmd);
		return -EINVAL;
	}

	for (i = 0; i < cmditem->nr_required_attrs; i++) {
		if (info->attrs[cmditem->required_attrs[i]])
			sat_req_attrs++;
	}

	if (sat_req_attrs != cmditem->nr_required_attrs) {
		pr_info("an8855-nl: missing required attr(s) for cmd %u\n",
			hdr->cmd);
		return -EINVAL;
	}

	if (cmditem->require_dev) {
		gsw = an8855_nl_parse_find_gsw(info);
		if (!gsw) {
			pr_info("an8855-nl: failed to find switch dev\n");
			return -EINVAL;
		}
	}

	ret = cmditem->process(info, gsw);

	an8855_put_gsw();

	return ret;
}

int an8855_gsw_nl_init(void)
{
	int ret;

	ret = genl_register_family(&an8855_nl_family);
	if (ret) {
		pr_info("an8855-nl: genl_register_family_with_ops failed\n");
		return ret;
	}

	return 0;
}

void an8855_gsw_nl_exit(void)
{
	genl_unregister_family(&an8855_nl_family);
}
