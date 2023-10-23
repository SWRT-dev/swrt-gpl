// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 -2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <net/pkt_cls.h>
#include <net/flow_dissector.h>
#include <net/tc_act/tc_vlan.h>
#include <net/tc_act/tc_gact.h>
#include <net/tc_act/tc_mirred.h>
#include <net/datapath_api_vlan.h>
#include <linux/version.h>
#include <uapi/linux/tc_act/tc_vlan.h>
#include "pon_qos_tc_vlan_storage.h"
#include "pon_qos_tc_vlan_prepare.h"
#include "pon_qos_tc_lct.h"

/* This file provides functions which parse the Linux flower (VLAN) dissectors
 * and actions, these information are then covered into the matching dp manager
 * struct dp_vlan(0,1,2) structures.
 */

void pon_qos_tc_dp_vlan_prepare(struct net_device *dev,
				struct dp_tc_vlan *dp_vlan,
				bool ingress)
{
	dp_vlan->dev = dev;
	dp_vlan->dir = ingress ? DP_DIR_INGRESS : DP_DIR_EGRESS;
	pon_qos_lct_lan_ctp(dev, dp_vlan);
}

static enum tc_flower_vlan_tag
__pon_qos_tc_vlan_tag_get(struct net_device *dev,
			  struct tc_cls_flower_offload *f)
{
	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_CVLAN))
		return TC_VLAN_DOUBLE_TAGGED;

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_VLAN))
		return TC_VLAN_SINGLE_TAGGED;

	return TC_VLAN_UNKNOWN;
}

enum tc_flower_vlan_tag
pon_qos_tc_vlan_tag_get(struct net_device *dev,
			struct tc_cls_flower_offload *f)
{
	struct flow_dissector_key_basic *key = NULL;
	struct flow_dissector_key_basic *mask = NULL;
	struct flow_dissector *d = f->dissector;

	if (eth_type_vlan(f->common.protocol))
		return __pon_qos_tc_vlan_tag_get(dev, f);

	if (ntohs(f->common.protocol) == ETH_P_ALL)
		return TC_VLAN_UNTAGGED;

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_BASIC)) {
		key = skb_flow_dissector_target(d, FLOW_DISSECTOR_KEY_BASIC,
						f->key);
		mask = skb_flow_dissector_target(d, FLOW_DISSECTOR_KEY_BASIC,
						 f->mask);

		if (mask->n_proto && !eth_type_vlan(key->n_proto)) {
			netdev_dbg(dev, "%s: 0-tag rule\n", __func__);
			return TC_VLAN_UNTAGGED;
		}
	}

	netdev_dbg(dev, "%s: unknown tag\n", __func__);
	return TC_VLAN_UNKNOWN;
}

static int pon_qos_tc_get_ethtype(struct net_device *dev,
				  struct tc_cls_flower_offload *f)
{
	struct flow_dissector_key_basic *key = NULL;
	struct flow_dissector_key_basic *mask = NULL;
	struct flow_dissector *d = f->dissector;

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_BASIC)) {
		key = skb_flow_dissector_target(d, FLOW_DISSECTOR_KEY_BASIC,
						f->key);
		mask = skb_flow_dissector_target(d, FLOW_DISSECTOR_KEY_BASIC,
						 f->mask);

		if (mask->n_proto) {
			netdev_dbg(dev, "%s:etype %#x\n",
				   __func__, ntohs(key->n_proto));
			return ntohs(key->n_proto);
		}
	}

	return DP_VLAN_PATTERN_NOT_CARE;
}

static void vlan_pattern_default_set(struct dp_pattern_vlan *patt)
{
	patt->proto = DP_VLAN_PATTERN_NOT_CARE;
	patt->prio = DP_VLAN_PATTERN_NOT_CARE;
	patt->vid = DP_VLAN_PATTERN_NOT_CARE;
	patt->tpid = DP_VLAN_PATTERN_NOT_CARE;
	patt->dei = DP_VLAN_PATTERN_NOT_CARE;
}

static void vlan_action_default_set(struct dp_act_vlan *act)
{
	act->act = 0;
	act->pop_n = 0;
	act->push_n = 0;
}

static bool fwd_drop_action_parse(struct net_device *dev,
				  struct tc_cls_flower_offload *f,
				  struct dp_act_vlan *act)
{
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);
#else
	int i;
#endif

#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list) {
#else
	tcf_exts_for_each_action(i, a, f->exts) {
#endif
		/* DROP and PASS make no sense together */
		if (is_tcf_gact_shot(a)) {
			act->act = DP_VLAN_ACT_DROP;
			netdev_dbg(dev, "DROP action detected, act: %x\n",
				   act->act);
			return true;
		}
		if (a->ops && a->ops->type == TCA_ACT_GACT &&
		    ((struct tcf_gact *)a)->tcf_action == TC_ACT_OK) {
			act->act = DP_VLAN_ACT_FWD;
			netdev_dbg(dev, "PASS action detected, act: %x\n",
				   act->act);
			return true;
		}
	}

	return false;
}

static int __vlan_parse(struct net_device *dev,
			struct tc_cls_flower_offload *f,
			struct dp_pattern_vlan *patt,
			enum flow_dissector_key_id key_id)
{
	struct flow_dissector_key_vlan *key;
	struct flow_dissector_key_vlan *mask;

	if (!dissector_uses_key(f->dissector, key_id))
		return -EIO;

	key = skb_flow_dissector_target(f->dissector,
					key_id,
					f->key);
	mask = skb_flow_dissector_target(f->dissector,
					 key_id,
					 f->mask);

	if (mask && key) {
		if (mask->vlan_id == 0xfff)
			patt->vid = key->vlan_id;
		if (mask->vlan_priority == 0x7)
			patt->prio = key->vlan_priority;
		patt->tpid = ntohs(key->vlan_tpid);
	}

	netdev_dbg(dev, "VLAN parsed, vid: %d, tpid: %x, prio: %d\n",
		   patt->vid, patt->tpid, patt->prio);

	return 0;
}

static int vlan_parse(struct net_device *dev,
		      struct tc_cls_flower_offload *f,
		      struct dp_pattern_vlan *patt)
{
	return __vlan_parse(dev, f, patt, FLOW_DISSECTOR_KEY_VLAN);
}

static int cvlan_parse(struct net_device *dev,
		       struct tc_cls_flower_offload *f,
		       struct dp_pattern_vlan *patt)
{
	return __vlan_parse(dev, f, patt, FLOW_DISSECTOR_KEY_CVLAN);
}

static void cookie_parse(struct net_device *dev,
			 struct tc_cls_flower_offload *f,
			 struct dp_act_vlan *act)
{
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);
#else
	int i;
#endif

#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list) {
#else
	tcf_exts_for_each_action(i, a, f->exts) {
#endif
		if (a->act_cookie) {
			u8 *p = a->act_cookie->data;
			u64 hi = 0;
			u64 lo = 0;
			int i;

			if (a->act_cookie->len < 16) {
				netdev_warn(dev, "%s: cookie is too short!\n",
					    __func__);
				continue;
			}

			for (i = 7; i > -1; i--) {
				u64 cookie_byte = *p;

				hi |= (cookie_byte << i * 8);
				p++;
			}

			for (i = 7; i > -1; i--) {
				u64 cookie_byte = *p;

				lo |= (cookie_byte << i * 8);
				p++;
			}

			act->sort_key = hi & FILTER_SORT_MASK;
			act->merge_key.hi = hi;
			act->merge_key.lo = lo;

			netdev_dbg(dev, "Cookie parsed: %llx\n",
				   act->sort_key);
			return;
		}
	}
}

#define IP_TOS_TO_DSCP 0xFC
static void dscp_parse(struct net_device *dev,
		       struct tc_cls_flower_offload *f,
		       struct dp_act_vlan *act,
		       const struct tc_action *a)
{
	struct flow_dissector_key_ip *key;
	u32 dscp_value;

	if (!dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_IP))
		return;

	key = skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_IP,
					f->key);

	dscp_value = (key->tos & IP_TOS_TO_DSCP) >> 2;
	act->dscp_pcp_map[dscp_value] = tcf_vlan_push_prio(a);
	act->dscp_pcp_map_valid[dscp_value] = true;
	act->prio[act->push_n] = DERIVE_FROM_DSCP;
	netdev_dbg(dev, "DSCP detected, value: %u, prio: %u\n",
		   dscp_value, act->dscp_pcp_map[dscp_value]);
}

static void vlan_action_modify_prio_parse(struct net_device *dev,
					  const struct tc_action *a,
					  enum tc_flower_vlan_tag tag,
					  int *prio)
{
	u8 flags = tcf_vlan_push_flags(a);

	if (flags & ACTVLAN_PUSH_F_PRIO) {
		*prio = tcf_vlan_push_prio(a);
		return;
	}

	if (tag == TC_VLAN_SINGLE_TAGGED)
		*prio = CP_FROM_INNER;
	else if (tag == TC_VLAN_DOUBLE_TAGGED)
		*prio = CP_FROM_OUTER;
}

static void vlan_action_vid_parse(struct net_device *dev,
				  const struct tc_action *a,
				  enum tc_flower_vlan_tag tag,
				  int *vid)
{
	u8 flags = tcf_vlan_push_flags(a);

	if (flags & ACTVLAN_PUSH_F_ID) {
		*vid = tcf_vlan_push_vid(a);
		return;
	}

	if (tag == TC_VLAN_SINGLE_TAGGED)
		*vid = CP_FROM_INNER;
	else if (tag == TC_VLAN_DOUBLE_TAGGED)
		*vid = CP_FROM_OUTER;
}

static void vlan_action_modify_proto_parse(struct net_device *dev,
					   const struct tc_action *a,
					   enum tc_flower_vlan_tag tag,
					   int *tpid)
{
	u8 flags = tcf_vlan_push_flags(a);

	if (flags & ACTVLAN_PUSH_F_PROTO) {
		*tpid = ntohs(tcf_vlan_push_proto(a));
		return;
	}

	if (tag == TC_VLAN_SINGLE_TAGGED)
		*tpid = CP_FROM_INNER;
	else if (tag == TC_VLAN_DOUBLE_TAGGED)
		*tpid = CP_FROM_OUTER;
}

static int vlan_action_parse(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     struct dp_act_vlan *act,
			     enum tc_flower_vlan_tag tag)
{
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);
#else
	int i;
#endif

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	if (tc_no_actions(f->exts)) {
#else
	if (!tcf_exts_has_actions(f->exts)) {
#endif
		netdev_dbg(dev, "TC no actions!\n");
		return -EIO;
	}
	netdev_dbg(dev, "TC actions count: %d\n",
		   (f->exts)->nr_actions);

#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list) {
#else
	tcf_exts_for_each_action(i, a, f->exts) {
#endif
		if (!is_tcf_vlan(a))
			continue;

		if (tcf_vlan_action(a) == TCA_VLAN_ACT_POP) {
			act->act |= DP_VLAN_ACT_POP;
			act->pop_n++;
			netdev_dbg(dev, "POP action detected, pop_n: %d, act: %x\n",
				   act->pop_n, act->act);
			continue;
		}

		if (act->push_n >= DP_VLAN_NUM) {
			netdev_warn(dev, "%s: push action overlod\n", __func__);
			continue;
		}

		if (tcf_vlan_action(a) == TCA_VLAN_ACT_PUSH) {
			act->act |= DP_VLAN_ACT_PUSH;

			act->prio[act->push_n] = tcf_vlan_push_prio(a);
			act->tpid[act->push_n] = ntohs(tcf_vlan_push_proto(a));
			netdev_dbg(dev, "PUSH action detected, push_n: %d, act: %x\n",
				   act->push_n + 1, act->act);
		} else if (tcf_vlan_action(a) == TCA_VLAN_ACT_MODIFY) {
			act->act = DP_VLAN_ACT_POP | DP_VLAN_ACT_PUSH;
			act->pop_n++;
			vlan_action_modify_prio_parse(dev, a, tag,
						      &act->prio[act->push_n]);
			vlan_action_modify_proto_parse(dev, a, tag,
						      &act->tpid[act->push_n]);
			netdev_dbg(dev, "MODIFY action detected, push_n: %d, pop_n: %d, act: %x\n",
				   act->push_n + 1, act->pop_n, act->act);
		}

		vlan_action_vid_parse(dev, a, tag, &act->vid[act->push_n]);
		act->dei[act->push_n] = 0;

		netdev_dbg(dev, "PUSH action parsed, vid: %d, tpid: %x, prio: %d, dei: %d\n",
			   act->vid[act->push_n],
			   act->tpid[act->push_n],
			   act->prio[act->push_n],
			   act->dei[act->push_n]);

		dscp_parse(dev, f, act, a);

		act->push_n++;
	}

	return 0;
}

static void default_prio_parse(struct net_device *dev,
			       struct tc_cls_flower_offload *f,
			       int *prio)
{
	*prio = f->common.prio >> 16;
	netdev_dbg(dev, "Default prio: %d\n", *prio);
}

int pon_qos_tc_vlan_untagged_prepare(struct net_device *dev,
				     struct tc_cls_flower_offload *f,
				     struct dp_tc_vlan *dp_vlan,
				     struct dp_vlan0 *rule)
{
	u32 proto = ntohs(f->common.protocol);
	bool drop;
	int err;

	vlan_pattern_default_set(&rule->outer);
	vlan_action_default_set(&rule->act);

	if (proto != ETH_P_ALL)
		rule->outer.proto = proto;

	/* Can not drop the untagged packets from/to the LCT port because then
	 * the LCT port would not work any more.
	 */
	if (fwd_drop_action_parse(dev, f, &rule->act)) {
		drop = (rule->act.act == DP_VLAN_ACT_DROP) ? true : false;
		if (drop && (pon_qos_lct_configure(dev, dp_vlan, rule) == 0))
			rule->act.act = DP_VLAN_ACT_FWD;
	} else {
		err = vlan_action_parse(dev, f, &rule->act,
					TC_VLAN_UNTAGGED);
		if (err)
			return err;
	}

	cookie_parse(dev, f, &rule->act);
	default_prio_parse(dev, f, &rule->prio);

	return 0;
}

int pon_qos_tc_vlan_untagged_flt_prepare(struct net_device *dev,
					 struct tc_cls_flower_offload *f,
					 struct dp_tc_vlan *dp_vlan,
					 struct dp_vlan0 *rule)
{
	u32 proto = ntohs(f->common.protocol);
	int err;

	vlan_pattern_default_set(&rule->outer);
	vlan_action_default_set(&rule->act);

	if (proto != ETH_P_ALL)
		rule->outer.proto = proto;

	if (!fwd_drop_action_parse(dev, f, &rule->act))
		return -EINVAL;

	err = vlan_action_parse(dev, f, &rule->act, TC_VLAN_UNTAGGED);
	if (err)
		return err;

	default_prio_parse(dev, f, &rule->prio);

	return 0;
}

int pon_qos_tc_vlan_single_tagged_prepare(struct net_device *dev,
					  struct tc_cls_flower_offload *f,
					  struct dp_vlan1 *rule)
{
	int err;

	vlan_pattern_default_set(&rule->outer);
	vlan_action_default_set(&rule->act);

	fwd_drop_action_parse(dev, f, &rule->act);

	err = vlan_parse(dev, f, &rule->outer);
	if (err)
		return err;

	err = vlan_action_parse(dev, f, &rule->act,
				TC_VLAN_SINGLE_TAGGED);
	if (err)
		return err;

	rule->outer.proto = pon_qos_tc_get_ethtype(dev, f);

	cookie_parse(dev, f, &rule->act);
	default_prio_parse(dev, f, &rule->prio);

	return 0;
}

int pon_qos_tc_vlan_single_tagged_flt_prepare(struct net_device *dev,
					      struct tc_cls_flower_offload *f,
					      struct dp_vlan1 *rule)
{
	int err;

	vlan_pattern_default_set(&rule->outer);
	vlan_action_default_set(&rule->act);

	fwd_drop_action_parse(dev, f, &rule->act);

	if (!fwd_drop_action_parse(dev, f, &rule->act))
		return -EINVAL;

	err = vlan_parse(dev, f, &rule->outer);
	if (err)
		return err;

	rule->outer.tpid = DP_VLAN_PATTERN_NOT_CARE;

	default_prio_parse(dev, f, &rule->prio);

	return 0;
}

int pon_qos_tc_vlan_double_tagged_prepare(struct net_device *dev,
					  struct tc_cls_flower_offload *f,
					  struct dp_vlan2 *rule)
{
	int err;

	vlan_pattern_default_set(&rule->outer);
	vlan_pattern_default_set(&rule->inner);
	vlan_action_default_set(&rule->act);

	fwd_drop_action_parse(dev, f, &rule->act);

	err = vlan_parse(dev, f, &rule->outer);
	if (err)
		return err;

	err = cvlan_parse(dev, f, &rule->inner);
	if (err)
		return err;

	err = vlan_action_parse(dev, f, &rule->act,
				TC_VLAN_DOUBLE_TAGGED);
	if (err)
		return err;

	rule->outer.proto = pon_qos_tc_get_ethtype(dev, f);

	cookie_parse(dev, f, &rule->act);
	default_prio_parse(dev, f, &rule->prio);

	return 0;
}

int pon_qos_tc_vlan_double_tagged_flt_prepare(struct net_device *dev,
					      struct tc_cls_flower_offload *f,
					      struct dp_vlan2 *rule)
{
	int err;

	vlan_pattern_default_set(&rule->outer);
	vlan_pattern_default_set(&rule->inner);
	vlan_action_default_set(&rule->act);

	if (!fwd_drop_action_parse(dev, f, &rule->act))
		return -EINVAL;

	err = vlan_parse(dev, f, &rule->outer);
	if (err)
		return err;

	rule->outer.tpid = DP_VLAN_PATTERN_NOT_CARE;

	default_prio_parse(dev, f, &rule->prio);

	return 0;
}
