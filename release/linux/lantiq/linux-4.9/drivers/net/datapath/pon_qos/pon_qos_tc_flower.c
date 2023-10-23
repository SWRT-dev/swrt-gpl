// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <net/pkt_cls.h>
#include <net/tc_act/tc_mirred.h>
#include <net/tc_act/tc_colmark.h>
#include <net/tc_act/tc_police.h>
#include <net/tc_act/tc_gact.h>
#include <net/flow_dissector.h>
#include <linux/version.h>
#include "pon_qos_tc_flower.h"
#include "pon_qos_tc_ext_vlan.h"
#include "pon_qos_tc_vlan_filter.h"
#include "pon_qos_tc_qmap.h"
#include "pon_qos_tc_mirred.h"
#include "pon_qos_tc_police.h"
#include "pon_qos_tc_trap.h"
#include "pon_qos_tc_ip_drop.h"
#include "pon_qos_trace.h"

struct pon_qos_storage_node {
	/** Unique key for TC flower - flower cookie */
	unsigned long key;
	/** Type of TC flower */
	enum pon_qos_tc_flower_type type;
	/** Pointer to structure of specific type */
	void *arg1;
	/** Pointer to structure of specific type */
	void *arg2;
	/** Hash table identifier */
	struct hlist_node node;
};

static DEFINE_HASHTABLE(pon_qos_storage, 16);

static char *flower_type_str_get(enum pon_qos_tc_flower_type type)
{
	switch (type) {
	case TC_TYPE_EXT_VLAN:
		return "TC_TYPE_EXT_VLAN";
	case TC_TYPE_VLAN_FILTER:
		return "TC_TYPE_VLAN_FILTER";
	case TC_TYPE_QUEUE:
		return "TC_TYPE_QUEUE";
	case TC_TYPE_MIRRED:
		return "TC_TYPE_MIRRED";
	case TC_TYPE_POLICE:
		return "TC_TYPE_POLICE";
	case TC_TYPE_COLMARK:
		return "TC_TYPE_COLMARK";
	case TC_TYPE_TRAP:
		return "TC_TYPE_TRAP";
	case TC_TYPE_IP_DROP:
		return "TC_TYPE_DROP";
	default:
		return "TC_TYPE_UNKNOWN";
	}
}

int pon_qos_tc_flower_storage_add(struct net_device *dev,
				  unsigned long cookie,
				  enum pon_qos_tc_flower_type type,
				  void *arg1, void *arg2)
{
	struct pon_qos_storage_node *entry;

	entry = kzalloc(sizeof(*entry), GFP_KERNEL);
	if (!entry)
		return -ENOMEM;

	entry->key = cookie;
	entry->type = type;
	entry->arg1 = arg1;
	entry->arg2 = arg2;

	hash_add(pon_qos_storage, &entry->node, entry->key);
	netdev_dbg(dev, "PON flower storage add, cookie: %lx, type: %s, arg1: %p, arg2: %p\n",
		   entry->key, flower_type_str_get(entry->type),
		   entry->arg1, entry->arg2);

	return 0;
}

static int flower_remove(struct net_device *dev,
			 u32 handle,
			 struct tc_cls_flower_offload *f,
			 enum pon_qos_tc_flower_type type,
			 void *arg1, void *arg2)
{
	switch (type) {
	case TC_TYPE_EXT_VLAN:
		return pon_qos_tc_ext_vlan_del(dev, arg1, arg2);
	case TC_TYPE_VLAN_FILTER:
		return pon_qos_tc_vlan_filter_del(dev, arg1, arg2);
	case TC_TYPE_QUEUE:
		return pon_qos_tc_unmap(dev, arg1);
	case TC_TYPE_MIRRED:
		return pon_qos_mirred_unoffload(dev, f, handle);
	case TC_TYPE_IP_DROP:
		return pon_qos_ip_drop_unoffload(dev, f, handle);
	case TC_TYPE_POLICE:
	case TC_TYPE_COLMARK:
		return pon_qos_police_unoffload(dev, arg1, arg2);
	case TC_TYPE_TRAP:
		return pon_qos_trap_unoffload(dev, f, handle);
	default:
		return -EOPNOTSUPP;
	}

	return -EOPNOTSUPP;
}

static int pon_qos_tc_flower_storage_del(struct net_device *dev,
					 u32 handle,
					 struct tc_cls_flower_offload *f)
{
	struct pon_qos_storage_node *entry;
	struct hlist_node *tmp;

	hash_for_each_possible_safe(pon_qos_storage, entry, tmp, node,
				    f->cookie) {
		if (f->cookie != entry->key)
			continue;
		flower_remove(dev, handle, f, entry->type, entry->arg1,
			      entry->arg2);
		netdev_dbg(dev, "PON flower storage del, cookie: %lx, type: %s\n",
			   entry->key, flower_type_str_get(entry->type));
		hash_del(&entry->node);
		kfree(entry);
	}

	return 0;
}

#if (KERNEL_VERSION(4, 10, 0) > LINUX_VERSION_CODE)
static bool is_type_mirred(struct net_device *dev,
			   struct tc_cls_flower_offload *f)
{
	const struct tc_action *a;
	LIST_HEAD(actions);

	if (tc_no_actions(f->exts))
		return false;

	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list)
		if (is_tcf_mirred_redirect(a))
			return true;

	return false;
}
#elif (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
static bool is_type_mirred(struct net_device *dev,
			   struct tc_cls_flower_offload *f)
{
	const struct tc_action *a;
	LIST_HEAD(actions);

	if (!tcf_exts_has_actions(f->exts))
		return false;

	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list)
		if (is_tcf_mirred_egress_redirect(a))
			return true;

	return false;
}
#else
static bool is_type_mirred(struct net_device *dev,
			   struct tc_cls_flower_offload *f)
{
	const struct tc_action *a;
	int i;

	if (!tcf_exts_has_actions(f->exts))
		return false;

	tcf_exts_for_each_action(i, a, f->exts)
		if (is_tcf_mirred_egress_redirect(a))
			return true;

	return false;
}
#endif

static bool is_type_ip_drop(struct net_device *dev,
			    struct tc_cls_flower_offload *f)
{
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);
#else
	int i;
#endif

	if (!dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_BASIC) ||
	    !dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_CONTROL))
		return false;

	if (!dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_IPV4_ADDRS) &&
	    !dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_IPV6_ADDRS))
		return false;

#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	if (tc_no_actions(f->exts))
		return false;

	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list)
#else
	if (!tcf_exts_has_actions(f->exts))
		return false;

	tcf_exts_for_each_action(i, a, f->exts)
#endif
		if (is_tcf_gact_shot(a))
			return true;

	return false;
}

static bool is_type_colmark(struct net_device *dev,
			    struct tc_cls_flower_offload *f)
{
	/* This driver supports only single actions for now */
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	if (tc_no_actions(f->exts))
#else
	if (!tcf_exts_has_actions(f->exts))
#endif
		return false;

	return is_tcf_colmark(f->exts->actions[0]);
}

static bool is_type_police(struct net_device *dev,
			    struct tc_cls_flower_offload *f)
{
	/* This driver supports only single actions for now */
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	if (tc_no_actions(f->exts))
#else
	if (!tcf_exts_has_actions(f->exts))
#endif
		return false;

	return is_tcf_police(f->exts->actions[0]);
}


static bool is_type_trap(struct net_device *dev,
			 struct tc_cls_flower_offload *f)
{
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);

	if (tc_no_actions(f->exts))
		return false;

	tcf_exts_to_list(f->exts, &actions);
	list_for_each_entry(a, &actions, list)
#else
	int i;

	if (!tcf_exts_has_actions(f->exts))
		return false;

	tcf_exts_for_each_action(i, a, f->exts)
#endif
		if (((struct tcf_gact *)a)->tcf_action == TC_ACT_TRAP)
			return true;

	return false;
}

static bool is_type_queue(struct net_device *dev,
			  struct tc_cls_flower_offload *f)
{
	return !!f->classid;
}

static bool is_type_vlan_filter(struct net_device *dev,
				struct tc_cls_flower_offload *f)
{
	struct net_device *indev = NULL;
	int ifi = (int)*(int *)f->key;

	if (ifi) {
		indev = dev_get_by_index(dev_net(dev), ifi);
		if (!indev)
			return false;

		if (!strncmp(dev->name, indev->name, strlen(dev->name))) {
			dev_put(indev);
			return true;
		}
		dev_put(indev);
	}

	return false;
}

static bool is_type_ext_vlan(struct net_device *dev,
			     struct tc_cls_flower_offload *f)
{
	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_BASIC) ||
	    dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_VLAN) ||
	    dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_CVLAN))
		return true;

	return false;
}

static enum pon_qos_tc_flower_type
flower_type_get(struct net_device *dev, struct tc_cls_flower_offload *f)
{
	if (is_type_trap(dev, f))
		return TC_TYPE_TRAP;

	if (is_type_ip_drop(dev, f))
		return TC_TYPE_IP_DROP;

	if (is_type_mirred(dev, f))
		return TC_TYPE_MIRRED;

	if (is_type_police(dev, f))
		return TC_TYPE_POLICE;

	if (is_type_colmark(dev, f))
		return TC_TYPE_COLMARK;

	if (is_type_queue(dev, f))
		return TC_TYPE_QUEUE;

	if (is_type_vlan_filter(dev, f))
		return TC_TYPE_VLAN_FILTER;

	if (is_type_ext_vlan(dev, f))
		return TC_TYPE_EXT_VLAN;

	return TC_TYPE_UNKNOWN;
}

static int pon_qos_tc_flower_replace(struct net_device *dev,
				     u32 handle,
				     bool ingress,
				     struct tc_cls_flower_offload *f)
{
	enum pon_qos_tc_flower_type type;

	type = flower_type_get(dev, f);
	netdev_dbg(dev, "TC TYPE: %s", flower_type_str_get(type));

	switch (type) {
	case TC_TYPE_EXT_VLAN:
		return pon_qos_tc_ext_vlan_add(dev, f, ingress);
	case TC_TYPE_VLAN_FILTER:
		return pon_qos_tc_vlan_filter_add(dev, f, ingress);
	case TC_TYPE_QUEUE:
		return pon_qos_tc_map(dev, f, ingress);
	case TC_TYPE_IP_DROP:
		return pon_qos_ip_drop_offload(dev, f, handle);
	case TC_TYPE_MIRRED:
		return pon_qos_mirred_offload(dev, f, handle);
	case TC_TYPE_POLICE:
	case TC_TYPE_COLMARK:
		return pon_qos_police_offload(dev, f, ingress);
	case TC_TYPE_TRAP:
		return pon_qos_trap_offload(dev, f, handle);
	default:
		return -EOPNOTSUPP;
	}

	return -EOPNOTSUPP;
}

static int pon_qos_tc_flower_destroy(struct net_device *dev,
				     u32 handle,
				     struct tc_cls_flower_offload *f)
{
	return pon_qos_tc_flower_storage_del(dev, handle, f);
}

#if (KERNEL_VERSION(4, 14, 0) < LINUX_VERSION_CODE)
int pon_qos_tc_flower_offload(struct net_device *dev,
			      u32 handle,
			      bool ingress,
			      void *type_data)
#else
int pon_qos_tc_flower_offload(struct net_device *dev,
			      u32 handle,
			      void *type_data)
#endif
{
	int ret = 0;
#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	struct tc_cls_flower_offload *f =
		((struct tc_to_netdev *)type_data)->cls_flower;
	bool ingress = TC_H_MIN(f->common.classid) != TC_H_MIN(TC_H_MIN_EGRESS);
#else
	struct tc_cls_flower_offload *f =
		(struct tc_cls_flower_offload *)type_data;
#endif

	ASSERT_RTNL();
	netdev_dbg(dev, "%s:start %d\n", __func__, ret);

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	if (TC_H_MAJ(f->common.classid) != TC_H_MAJ(TC_H_CLSACT))
		return -EOPNOTSUPP;
#endif

	switch (f->command) {
	case TC_CLSFLOWER_REPLACE:
	{
		netdev_dbg(dev, "%s:replace:\n", __func__);
		trace_pon_qos_tc_flower_enter(dev, f, flower_type_get(dev, f));
		ret = pon_qos_tc_flower_replace(dev, handle, ingress, f);
		break;
	}
	case TC_CLSFLOWER_DESTROY:
	{
		netdev_dbg(dev, "%s:destroy:\n", __func__);
		trace_pon_qos_tc_flower_enter(dev, f, -1);
		ret = pon_qos_tc_flower_destroy(dev, handle, f);
		break;
	}
	case TC_CLSFLOWER_STATS:
	{
		netdev_dbg(dev, "%s:stats:\n", __func__);
		trace_pon_qos_tc_flower_enter(dev, f, -1);
		return -EOPNOTSUPP;
	}
	default:
		return -EOPNOTSUPP;
	}

	netdev_dbg(dev, "%s:exit %d\n", __func__, ret);
	trace_pon_qos_tc_flower_exit(dev, f, -1);
	return ret;
}

#if (KERNEL_VERSION(4, 14, 0) < LINUX_VERSION_CODE)
int pon_qos_tc_block_cb_ingress(enum tc_setup_type type,
				void *type_data,
				void *cb_priv)
{
	int ret = 0;
	struct net_device *dev = (struct net_device *)cb_priv;

	ASSERT_RTNL();
	netdev_dbg(dev, "%s:start\n", __func__);

	if (!tc_cls_can_offload_and_chain0(dev, type_data))
		return -EOPNOTSUPP;

	if (type == TC_SETUP_CLSFLOWER) {
		struct tc_cls_flower_offload *cls_flower =
			(struct tc_cls_flower_offload *)type_data;

		ret = pon_qos_tc_flower_offload(dev,
						cls_flower->handle,
						true,
						cls_flower);
	}

	netdev_dbg(dev, "%s:exit %d\n", __func__, ret);
	return ret;
}

int pon_qos_tc_block_cb_egress(enum tc_setup_type type,
			       void *type_data,
			       void *cb_priv)
{
	int ret = 0;
	struct net_device *dev = (struct net_device *)cb_priv;

	ASSERT_RTNL();
	netdev_dbg(dev, "%s:start\n", __func__);

	if (!tc_cls_can_offload_and_chain0(dev, type_data))
		return -EOPNOTSUPP;

	if (type == TC_SETUP_CLSFLOWER) {
		struct tc_cls_flower_offload *cls_flower =
			(struct tc_cls_flower_offload *)type_data;

		ret = pon_qos_tc_flower_offload(dev,
						cls_flower->handle,
						false,
						cls_flower);
	}

	netdev_dbg(dev, "%s:exit %d\n", __func__, ret);
	return ret;
}

int pon_qos_tc_block_offload(struct net_device *dev,
			     void *type_data)
{
	int ret = 0;
	tc_setup_cb_t *cb;
	struct tc_block_offload *f = (struct tc_block_offload *)type_data;

	ASSERT_RTNL();
	netdev_dbg(dev, " tc block offload starting - binder type %d\n",
		   f->binder_type);

	if (f->binder_type == TCF_BLOCK_BINDER_TYPE_CLSACT_INGRESS)
		cb = pon_qos_tc_block_cb_ingress;
	else if (f->binder_type == TCF_BLOCK_BINDER_TYPE_CLSACT_EGRESS)
		cb = pon_qos_tc_block_cb_egress;
	else
		return -EOPNOTSUPP;

	switch (f->command) {
	case TC_BLOCK_BIND:
		netdev_dbg(dev, "CLS BIND\n");
		ret = tcf_block_cb_register(f->block, cb, (void *)dev,
					    (void *)dev, f->extack);
		if (ret)
			return ret;

		return 0;
	case TC_BLOCK_UNBIND:
		netdev_dbg(dev, "CLS UNBIND\n");
		tcf_block_cb_unregister(f->block, cb, (void *)dev);

		return 0;
	default:
		return -EOPNOTSUPP;
	}

	netdev_dbg(dev, "%s:exit %d\n", __func__, ret);
	return ret;
}
#endif
