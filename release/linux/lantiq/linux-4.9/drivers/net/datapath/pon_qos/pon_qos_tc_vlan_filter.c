// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <linux/list.h>
#include <linux/list_sort.h>
#include <linux/netdevice.h>
#include <linux/version.h>
#include <net/pkt_cls.h>
#include <net/flow_dissector.h>
#include <net/tc_act/tc_vlan.h>
#include <net/tc_act/tc_gact.h>
#include <net/tc_act/tc_mirred.h>
#include <net/datapath_api.h>
#include <net/datapath_api_vlan.h>
#include <uapi/linux/tc_act/tc_vlan.h>
#include "pon_qos_tc_flower.h"
#include "pon_qos_tc_vlan_prepare.h"
#include "pon_qos_tc_vlan_storage.h"
#include "pon_qos_tc_vlan_filter.h"
#include "pon_qos_tc_lct.h"
#include "pon_qos_tc_ext_vlan.h"
#include "pon_qos_trace.h"

#define PON_QOS_TC_FILTER_DEF_PRIO_THRESHOLD 64000

static LIST_HEAD(tc_vlan_filter_storage);

static int untagged_add(struct net_device *dev,
			struct tc_cls_flower_offload *f,
			struct pon_qos_vlan_storage_node *node,
			struct dp_tc_vlan *dp_vlan)
{
	struct dp_vlan0 *rule;
	int err = 0;

	rule = kzalloc(sizeof(*rule), GFP_KERNEL);
	if (!rule)
		return -ENOMEM;

	err = pon_qos_tc_vlan_untagged_flt_prepare(dev, f, dp_vlan, rule);
	if (err) {
		kfree(rule);
		return err;
	}

	err = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_VLAN_FILTER,
					    node, rule);
	if (err < 0) {
		kfree(rule);
		return err;
	}

	if (rule->prio > PON_QOS_TC_FILTER_DEF_PRIO_THRESHOLD)
		rule->def = DP_VLAN_DEF_RULE;

	list_add(&rule->list, &node->rules);

	node->rule_cnt++;

	return 0;
}

static int single_tagged_add(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     struct pon_qos_vlan_storage_node *node)
{
	struct dp_vlan1 *rule;
	int err;

	rule = kzalloc(sizeof(*rule), GFP_KERNEL);
	if (!rule)
		return -ENOMEM;

	err = pon_qos_tc_vlan_single_tagged_flt_prepare(dev, f, rule);
	if (err) {
		kfree(rule);
		return err;
	}

	err = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_VLAN_FILTER,
					    node, rule);
	if (err < 0) {
		kfree(rule);
		return err;
	}

	if (rule->prio > PON_QOS_TC_FILTER_DEF_PRIO_THRESHOLD)
		rule->def = DP_VLAN_DEF_RULE;

	list_add(&rule->list, &node->rules);

	node->rule_cnt++;

	return 0;
}

static int double_tagged_add(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     struct pon_qos_vlan_storage_node *node)
{
	struct dp_vlan2 *rule;
	int err;

	rule = kzalloc(sizeof(*rule), GFP_KERNEL);
	if (!rule)
		return -ENOMEM;

	err = pon_qos_tc_vlan_double_tagged_flt_prepare(dev, f, rule);
	if (err) {
		kfree(rule);
		return err;
	}

	err = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_VLAN_FILTER,
					    node, rule);
	if (err < 0) {
		kfree(rule);
		return err;
	}

	if (rule->prio > PON_QOS_TC_FILTER_DEF_PRIO_THRESHOLD)
		rule->def = DP_VLAN_DEF_RULE;

	list_add(&rule->list, &node->rules);

	node->rule_cnt++;

	return 0;
}

static void dp_vlan_list_update(struct net_device *dev,
				struct dp_tc_vlan *dp_vlan,
				struct pon_qos_vlan_storage_node *node)
{
	switch (node->tag) {
	case TC_VLAN_UNTAGGED:
		dp_vlan->vlan0_head = &node->rules;
		dp_vlan->n_vlan0 = node->rule_cnt;
		break;
	case TC_VLAN_SINGLE_TAGGED:
		dp_vlan->vlan1_head = &node->rules;
		dp_vlan->n_vlan1 = node->rule_cnt;
		break;
	case TC_VLAN_DOUBLE_TAGGED:
		dp_vlan->vlan2_head = &node->rules;
		dp_vlan->n_vlan2 = node->rule_cnt;
		break;
	default:
		break;
	}
}

static int dp_update(struct net_device *dev,
		     struct dp_tc_vlan *dp_vlan,
		     bool ingress)
{
	struct pon_qos_vlan_storage_node *p;

	list_for_each_entry(p, &tc_vlan_filter_storage, list) {
		if (dev == p->dev &&
		    ingress == p->ingress) {
			dp_vlan_list_update(dev, dp_vlan, p);
		}
	}

	if (dp_vlan_set(dp_vlan, 0) == DP_FAILURE) {
		netdev_err(dev, "dp_vlan_set failed!\n");
		return -EPERM;
	}

	return 0;
}

int pon_qos_tc_vlan_filter_add(struct net_device *dev,
			       struct tc_cls_flower_offload *f,
			       bool ingress)
{
	struct dp_tc_vlan dp_vlan = {0};
	struct pon_qos_vlan_storage_node *node;
	enum tc_flower_vlan_tag tag;
	int err = 0;

	netdev_dbg(dev, "VLAN filter add, flower: %p\n", f);

	trace_vf_add_enter(dev, NULL, -1, -1, ingress);

	tag = pon_qos_tc_vlan_tag_get(dev, f);
	if (tag == TC_VLAN_UNKNOWN)
		return -EIO;

	node = pon_qos_tc_vlan_storage_get(dev, ingress, tag, false,
					   &tc_vlan_filter_storage);
	if (!node) {
		err = pon_qos_tc_vlan_storage_crt(dev, ingress, tag,
						  false, &node);
		if (err)
			return err;
		list_add(&node->list, &tc_vlan_filter_storage);
	}

	pon_qos_tc_dp_vlan_prepare(dev, &dp_vlan, ingress);

	switch (tag) {
	case TC_VLAN_UNTAGGED:
		err = untagged_add(dev, f, node, &dp_vlan);
		if (err)
			goto err;
		break;
	case TC_VLAN_SINGLE_TAGGED:
		err = single_tagged_add(dev, f, node);
		if (err)
			goto err;
		break;
	case TC_VLAN_DOUBLE_TAGGED:
		err = double_tagged_add(dev, f, node);
		if (err)
			goto err;
		break;
	default:
		err = -EIO;
		goto err;
	}

	err = dp_update(dev, &dp_vlan, ingress);

	trace_vf_add_exit(dev, node, node->tag, node->rule_cnt, ingress);

	return err;
err:
	pon_qos_tc_vlan_storage_del(dev, node);
	return err;
}

int pon_qos_tc_vlan_filter_del(struct net_device *dev,
			       void *vlan_storage,
			       void *rule)
{
	struct dp_tc_vlan dp_vlan = {0};
	struct pon_qos_vlan_storage_node *node;
	struct dp_vlan0 *rule0;
	struct dp_vlan1 *rule1;
	struct dp_vlan2 *rule2;
	bool ingress;
	int ret = 0;

	netdev_dbg(dev, "VLAN filter del, node: %p, rule: %p\n",
		   vlan_storage, rule);

	if (!vlan_storage || !rule)
		return -EIO;

	node = vlan_storage;
	ingress = node->ingress;

	trace_vf_del_enter(dev, node, node->tag, node->rule_cnt, ingress);

	pon_qos_tc_dp_vlan_prepare(dev, &dp_vlan, ingress);

	switch (node->tag) {
	case TC_VLAN_UNTAGGED:
		rule0 = (struct dp_vlan0 *)rule;
		pon_qos_lct_cleanup(rule0);
		list_del(&rule0->list);
		break;
	case TC_VLAN_SINGLE_TAGGED:
		rule1 = (struct dp_vlan1 *)rule;
		list_del(&rule1->list);
		break;
	case TC_VLAN_DOUBLE_TAGGED:
		rule2 = (struct dp_vlan2 *)rule;
		list_del(&rule2->list);
		break;
	default:
		return -EIO;
	}

	kfree(rule);
	node->rule_cnt--;
	pon_qos_tc_vlan_storage_del(dev, node);

	ret = dp_update(dev, &dp_vlan, ingress);

	trace_vf_del_exit(dev, NULL, -1, -1, ingress);

	return ret;
}

static bool pon_qos_tc_lookup_by_cvid(struct pon_qos_vlan_storage_node *node,
				      int cvid)
{
	struct dp_vlan1 *p1;
	struct dp_vlan2 *p2;

	switch (node->tag) {
	case TC_VLAN_UNTAGGED:
		break;
	case TC_VLAN_SINGLE_TAGGED:
		list_for_each_entry(p1, &node->rules, list) {
			if (p1->outer.vid == cvid)
				return true;
		}
		break;
	case TC_VLAN_DOUBLE_TAGGED:
		list_for_each_entry(p2, &node->rules, list) {
			if (p2->inner.vid == cvid)
				return true;
		}
		break;
	default:
		break;
	}

	return false;
}

static struct net_device *pon_qos_tc_search_pmapper_on_list(int cvid,
						      bool ingress,
						      struct list_head *storage)
{
	struct pon_qos_vlan_storage_node *p;

	list_for_each_entry(p, storage, list) {
		rtnl_lock();
		if (p->ingress == ingress &&
		    dp_is_pmapper_check(p->dev)) {
			if (pon_qos_tc_lookup_by_cvid(p, cvid)) {
				rtnl_unlock();
				return p->dev;
			}
		}
		rtnl_unlock();
	}

	return NULL;
}

struct net_device *pon_qos_tc_find_pmapper_via_cvid(int cvid, bool ingress)
{
	struct net_device *dev = NULL;

	dev = pon_qos_tc_search_pmapper_on_list(cvid, ingress,
						&tc_vlan_filter_storage);

	if (dev == NULL)
		dev = pon_qos_tc_search_pmapper_on_list(cvid, ingress,
					pon_qos_tc_get_ext_vlan_storage());
	return dev;
}
