// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <linux/list.h>
#include <linux/slab.h>
#include <linux/netdevice.h>
#include <net/datapath_api_vlan.h>
#include "pon_qos_tc_vlan_prepare.h"
#include "pon_qos_tc_vlan_storage.h"

struct pon_qos_vlan_storage_node
*pon_qos_tc_vlan_storage_get(struct net_device *dev,
			     bool ingress,
			     enum tc_flower_vlan_tag tag,
			     bool mcc,
			     struct list_head *head)
{
	struct pon_qos_vlan_storage_node *p;

	list_for_each_entry(p, head, list) {
		if (dev == p->dev &&
		    ingress == p->ingress &&
		    tag == p->tag &&
		    mcc == p->mcc) {
			netdev_dbg(dev, "VLAN storage found: %p\n", p);
			return p;
		}
	}

	netdev_dbg(dev, "VLAN storage not found\n");
	return NULL;
}

int pon_qos_tc_vlan_storage_crt(struct net_device *dev,
				bool ingress,
				enum tc_flower_vlan_tag tag,
				bool mcc,
				struct pon_qos_vlan_storage_node **node)
{
	*node = kzalloc(sizeof(**node), GFP_KERNEL);
	if (!*node) {
		netdev_err(dev, "Out of memory!\n");
		return -ENOMEM;
	}

	(*node)->dev = dev;
	(*node)->ingress = ingress;
	(*node)->tag = tag;
	(*node)->mcc = mcc;
	INIT_LIST_HEAD(&(*node)->rules);

	netdev_dbg(dev, "VLAN storage created: %p\n", *node);
	return 0;
}

void pon_qos_tc_vlan_storage_del(struct net_device *dev,
				 struct pon_qos_vlan_storage_node *node)
{
	if (node->rule_cnt > 0)
		return;

	if (!list_empty(&node->rules))
		netdev_warn(dev, "%s: rules list not empty!\n", __func__);

	netdev_dbg(dev, "VLAN storage deleted: %p\n", node);
	list_del(&node->list);
	kfree(node);
}

/* This sorting method although not strictly following the OMCI standard
 * is preferred and enabled by default. When sorting, it will compare by VID
 * first and then by priority.
 */
static int cookie_cmp(u64 a, u64 b)
{
	unsigned short vid_a, vid_b;

	if (((FILTER_OUTER_PRIO(a) == 15) || (FILTER_OUTER_PRIO(a) == 14)) &&
	    ((FILTER_INNER_PRIO(a) == 15) || (FILTER_INNER_PRIO(a) == 14))) {
		vid_a = 4097;
	} else {
		if ((FILTER_OUTER_VID(a) < 4096) &&
		    (FILTER_OUTER_PRIO(a) != 15)) {
			vid_a = FILTER_OUTER_VID(a);
		} else {
			vid_a = FILTER_INNER_VID(a);
		}
	}

	if (((FILTER_OUTER_PRIO(b) == 15) || (FILTER_OUTER_PRIO(b) == 14)) &&
	    ((FILTER_INNER_PRIO(b) == 15) || (FILTER_INNER_PRIO(b) == 14))) {
		vid_b = 4097;
	} else {
		if ((FILTER_OUTER_VID(b) < 4096) &&
		    (FILTER_OUTER_PRIO(b) != 15)) {
			vid_b = FILTER_OUTER_VID(b);
		} else {
			vid_b = FILTER_INNER_VID(b);
		}
	}

	if (vid_a != vid_b)
		return (vid_a - vid_b);
	else if (FILTER_OUTER_PRIO(a) != FILTER_OUTER_PRIO(b))
		return (FILTER_OUTER_PRIO(a) - FILTER_OUTER_PRIO(b));
	else if (FILTER_INNER_PRIO(a) != FILTER_INNER_PRIO(b))
		return (FILTER_INNER_PRIO(a) - FILTER_INNER_PRIO(b));
	else if (FILTER_OUTER_TPID(a) != FILTER_OUTER_TPID(b))
		return (FILTER_OUTER_TPID(b) - FILTER_OUTER_TPID(a));
	else if (FILTER_INNER_TPID(a) != FILTER_INNER_TPID(b))
		return (FILTER_INNER_TPID(b) - FILTER_INNER_TPID(a));

	return (FILTER_ETHERTYPE(b) - FILTER_ETHERTYPE(a));
}

static int prio_cmp(int a, int b)
{
	if (a < b)
		return -1;

	if (a > b)
		return 1;

	return 0;
}

#define PON_QOS_TC_FILTER_DEF_PRIO_THRESHOLD 64000

enum rule_sort_category {
	/* Non default rules are placed first */
	SORT_NONDEFAULT = 0,
	/* Then we have rules with
	 * prio > QOS_MGR_TC_FILTER_DEF_PRIO_THRESHOLD
	 * defined by the userspace application.
	 */
	SORT_DEFAULT = 1,
	/* Then there is a zero tagged default rule defined by ITU-T G.988 */
	SORT_DEFAULT_G988_ZERO = 2,
	/* Then there is a single tagged default rule defined by ITU-T G.988 */
	SORT_DEFAULT_G988_SINGLE = 3,
	/* Then there is a double tagged default rule defined by ITU-T G.988 */
	SORT_DEFAULT_G988_DOUBLE = 4,
};

/* When we sort rules, we can't only use a cookie. It matters if
 * the rule is the non-default, default zero, default single or
 * default double tagged. The default rules shall go always after non
 * default rules to ensure correct behavior.
 * See enum rule_sort_category for details
 */
static enum rule_sort_category rule_sort_category_get(int prio, u64 cookie_prio)
{
	u64 filter_outer_prio = FILTER_OUTER_PRIO(cookie_prio);
	u64 filter_outer_vid = FILTER_OUTER_VID(cookie_prio);
	u64 filter_outer_tpid = FILTER_OUTER_TPID(cookie_prio);
	u64 filter_inner_prio = FILTER_INNER_PRIO(cookie_prio);
	u64 filter_inner_vid = FILTER_INNER_VID(cookie_prio);
	u64 filter_inner_tpid = FILTER_INNER_TPID(cookie_prio);
	u64 filter_ethertype = FILTER_ETHERTYPE(cookie_prio);

	/* If non-default rule */
	if (prio <= PON_QOS_TC_FILTER_DEF_PRIO_THRESHOLD)
		return SORT_NONDEFAULT;

	/* If default untagged Ext. Vlan rule */
	if (filter_outer_prio == FILTER_PRIO_IGNORE &&
	    filter_inner_prio == FILTER_PRIO_IGNORE &&
	    filter_ethertype == FILTER_TPID_ANY)
		return SORT_DEFAULT_G988_ZERO;

	/* If default single tagged Ext. Vlan rule */
	if (filter_outer_prio == FILTER_PRIO_IGNORE &&
	    filter_inner_prio == FILTER_PRIO_DEFAULT &&
	    filter_inner_vid == FILTER_VID_ANY &&
	    filter_inner_tpid == FILTER_TPID_ANY &&
	    filter_ethertype == FILTER_ETHERTYPE_ANY)
		return SORT_DEFAULT_G988_SINGLE;

	/* If default double tagged Ext. Vlan rule */
	if (filter_outer_prio == FILTER_PRIO_DEFAULT &&
	    filter_outer_vid == FILTER_VID_ANY &&
	    filter_outer_tpid == FILTER_TPID_ANY &&
	    filter_inner_prio == FILTER_PRIO_DEFAULT &&
	    filter_inner_vid == FILTER_VID_ANY &&
	    filter_inner_tpid == FILTER_TPID_ANY &&
	    filter_ethertype == FILTER_ETHERTYPE_ANY)
		return SORT_DEFAULT_G988_DOUBLE;

	/* If a rule is default, but doesn't match the above
	 * 3 categories then we assign it to SORT_DEFAULT category, so
	 * that it is placed after the non-default rules but before
	 * last-resort default rules.
	 */
	return SORT_DEFAULT;
}

static int sort_category_cmp(int prio_a, u64 a, int prio_b, u64 b)
{
	return rule_sort_category_get(prio_a, a) -
	       rule_sort_category_get(prio_b, b);
}

int pon_qos_tc_cookie_cmp0(void *priv, struct list_head *lh_a,
			   struct list_head *lh_b)
{
	struct dp_vlan0 *la = list_entry(lh_a, struct dp_vlan0, list);
	struct dp_vlan0 *lb = list_entry(lh_b, struct dp_vlan0, list);
	int ret;

	ret = sort_category_cmp(la->prio, la->act.sort_key, lb->prio,
				lb->act.sort_key);
	if (ret != 0)
		return ret;

	if (la->act.sort_key != 0 && lb->act.sort_key != 0)
		return cookie_cmp(la->act.sort_key, lb->act.sort_key);

	return prio_cmp(la->prio, lb->prio);
}

int pon_qos_tc_cookie_cmp1(void *priv, struct list_head *lh_a,
			   struct list_head *lh_b)
{
	struct dp_vlan1 *la = list_entry(lh_a, struct dp_vlan1, list);
	struct dp_vlan1 *lb = list_entry(lh_b, struct dp_vlan1, list);
	int ret;

	ret = sort_category_cmp(la->prio, la->act.sort_key, lb->prio,
				lb->act.sort_key);
	if (ret != 0)
		return ret;

	if (la->act.sort_key != 0 && lb->act.sort_key != 0)
		return cookie_cmp(la->act.sort_key, lb->act.sort_key);

	return prio_cmp(la->prio, lb->prio);
}

int pon_qos_tc_cookie_cmp2(void *priv, struct list_head *lh_a,
			   struct list_head *lh_b)
{
	struct dp_vlan2 *la = list_entry(lh_a, struct dp_vlan2, list);
	struct dp_vlan2 *lb = list_entry(lh_b, struct dp_vlan2, list);
	int ret;

	ret = sort_category_cmp(la->prio, la->act.sort_key, lb->prio,
				lb->act.sort_key);
	if (ret != 0)
		return ret;

	if (la->act.sort_key != 0 && lb->act.sort_key != 0)
		return cookie_cmp(la->act.sort_key, lb->act.sort_key);

	return prio_cmp(la->prio, lb->prio);
}
