// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/
#include <net/pkt_cls.h>
#include <linux/list.h>
#include <linux/version.h>
#include <net/datapath_api.h>
#include <net/flow_dissector.h>
#include <net/tc_act/tc_mirred.h>
#include <net/tc_act/tc_gact.h>
#include <net/pon_qos_tc.h>
#include "pon_qos_tc_qos.h"
#include "pon_qos_tc_flower.h"

struct flower_cls_map {
	__be16 proto;
	u32 pref;
	u32 classid;
	char tc_cookie;
	bool ingress;
	struct flow_dissector_key_vlan key;
	struct flow_dissector_key_vlan mask;
	struct net_device *dev;
	int in_ifi;
	int tc;
	bool mapped;
	struct list_head list;
};

static LIST_HEAD(tc_class_list);

static int pon_qos_tc_parse_flower_action(struct tcf_exts *exts,
					  char *tc_cookie)
{
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);
#else
	int i;
#endif
	int act_nr = 0;
	int act_ok_nr = 0;
	int offset;

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	if (tc_no_actions(exts))
#else
	if (!tcf_exts_has_actions(exts))
#endif
		return -EINVAL;

#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	tcf_exts_to_list(exts, &actions);
	list_for_each_entry(a, &actions, list) {
#else
	tcf_exts_for_each_action(i, a, exts) {
#endif
		if (a->ops && a->ops->type == TCA_ACT_GACT &&
		    to_gact(a)->tcf_action == TC_ACT_OK)
			act_ok_nr++;
		act_nr++;

		if (a->act_cookie) {
			/* Maximum supported value is 15, so there is not need
			 * to store whole act_cookie->data. Copying whole
			 * unsigned char to signed char with such low values
			 * won't cause any conversion problem, and in addition
			 * will reset sign bit, which later will be used to
			 * check if cookie was set or not.
			 */
			offset = a->act_cookie->len - sizeof(*tc_cookie);
			memcpy(tc_cookie, a->act_cookie->data + offset,
			       sizeof(*tc_cookie));
		}
	}

	if (act_nr != 1 && act_ok_nr != 1)
		return -EINVAL;

	return 0;
}

static int pon_qos_parse_tc_flower(struct tc_cls_flower_offload *f,
				   struct flower_cls_map **flt,
				   bool ingress)
{
	int ret = 0;

	*flt = kzalloc(sizeof(**flt), GFP_KERNEL);
	if (!*flt)
		return -ENOMEM;

	if (f->dissector->used_keys &
			~(BIT(FLOW_DISSECTOR_KEY_CONTROL) |
				BIT(FLOW_DISSECTOR_KEY_BASIC) |
				BIT(FLOW_DISSECTOR_KEY_ETH_ADDRS) |
				BIT(FLOW_DISSECTOR_KEY_VLAN) |
				BIT(FLOW_DISSECTOR_KEY_IPV4_ADDRS) |
				BIT(FLOW_DISSECTOR_KEY_IPV6_ADDRS) |
				BIT(FLOW_DISSECTOR_KEY_IP) |
				BIT(FLOW_DISSECTOR_KEY_PORTS))) {
		pr_debug("%s: Unsupported key used: 0x%x\n", __func__,
			 f->dissector->used_keys);
		kfree(*flt);
		return -EINVAL;
	}
	pr_debug("%s: Supported key used: 0x%x\n", __func__,
		 f->dissector->used_keys);

	(*flt)->pref = f->common.prio >> 16;
	(*flt)->proto = f->common.protocol;
	(*flt)->classid = f->classid;
	(*flt)->ingress = ingress;

	/* Classification/Matching arguments parsing */
	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_VLAN)) {
		struct flow_dissector_key_vlan *key =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_VLAN,
						  f->key);
		struct flow_dissector_key_vlan *mask =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_VLAN,
						  f->mask);
		pr_debug("%s: match vid: %#x/%#x pcp: %#x\n", __func__,
			 key->vlan_id,
			 key->vlan_priority,
			 mask->vlan_id);
		(*flt)->key = *key;
		(*flt)->mask = *mask;
	}

	return ret;
}

static int pon_qos_get_subif_idx(struct net_device *dev, int *subif)
{
	dp_subif_t dp_subif = {0};
	int ret;

	if (!dev)
		return -EINVAL;

	if (pon_qos_is_netdev_cpu_port(dev)) {
		*subif = 0;
		return 0;
	}

	ret = dp_get_netif_subifid(dev, NULL, NULL, 0, &dp_subif, 0);
	if (ret < 0) {
		netdev_err(dev, "%s: subif idx get failed\n", __func__);
		return -ENODEV;
	}

	*subif = dp_subif.subif;

	return 0;
}

static int __pon_qos_tc_map(struct net_device *dev,
			    struct flower_cls_map *map,
			    struct net_device *indev,
			    int tc, bool en)
{
	struct pon_qos_qmap_tc qmap = { 0 };
	int ret;

	qmap.indev = indev;

	if (en) {
		if (qmap.indev) {
			ret = pon_qos_get_subif_idx(qmap.indev,
						    &qmap.tc);
			if (ret < 0)
				return -ENODEV;
		} else {
			qmap.tc = map->key.vlan_priority;
		}
	} else {
		qmap.tc = tc;
	}

	/* when Q is removed we should unmap the same classid */
	qmap.handle = map->classid;
	qmap.tc_cookie = map->tc_cookie;
	qmap.ingress = map->ingress;
	ret = pon_qos_update_qmap(dev, &qmap, en);
	if (ret < 0) {
		netdev_err(dev, "%s: queue unmap fail\n", __func__);
		return -EINVAL;
	}

	map->tc = qmap.tc;
	map->mapped = en;

	return 0;
}

int pon_qos_tc_map(struct net_device *dev, struct tc_cls_flower_offload *f,
		   bool ingress)
{
	struct flower_cls_map *map = NULL;
	struct net_device *indev = NULL;
	char tc_cookie = PON_QOS_TC_COOKIE_EMPTY;
	int ifi = FL_FLOW_KEY_IFINDEX(f);
	int ret = 0;

	if (ifi) {
		indev = dev_get_by_index(dev_net(dev), ifi);
		if (!indev)
			return -ENODEV;
	}

	ret = pon_qos_tc_parse_flower_action(f->exts, &tc_cookie);
	if (ret < 0)
		goto err_put_indev;

	ret = pon_qos_parse_tc_flower(f, &map, ingress);
	if (ret < 0)
		goto err_put_indev;

	if (!indev && !map->mask.vlan_priority &&
	    tc_cookie == PON_QOS_TC_COOKIE_EMPTY) {
		netdev_err(dev, "%s: no class specified\n", __func__);
		ret = -EINVAL;
		goto err;
	}

	map->dev = dev;
	map->in_ifi = ifi;
	map->tc_cookie = tc_cookie;

	ret = __pon_qos_tc_map(dev, map, indev, 0, true);
	if (ret < 0) {
		netdev_err(dev, "%s: queue map fail\n", __func__);
		goto err;
	}

	ret = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_QUEUE,
					    (void *)map, NULL);
	if (ret < 0)
		goto err;

	list_add(&map->list, &tc_class_list);
	if (indev)
		dev_put(indev);

	return 0;

err:
	kfree(map);
err_put_indev:
	if (indev)
		dev_put(indev);

	return ret;
}

int pon_qos_tc_unmap(struct net_device *dev, void *list_node)
{
	struct flower_cls_map *map;
	struct net_device *indev = NULL;
	int ret = 0;

	if (!list_node)
		return -EIO;

	map = list_node;

	if (map->mapped) {
		if (map->in_ifi)
			indev = dev_get_by_index(dev_net(dev), map->in_ifi);
		ret = __pon_qos_tc_map(dev, map, indev, map->tc, false);
		if (indev)
			dev_put(indev);
		if (ret < 0) {
			netdev_err(dev, "%s: queue unmap fail\n", __func__);
			return ret;
		}
	}

	list_del(&map->list);
	kfree(map);
	return 0;
}

int pon_qos_tc_classid_unmap(u32 classid)
{
	struct flower_cls_map *p, *n;
	int ret;

	list_for_each_entry_safe(p, n, &tc_class_list, list) {
		if (p->classid != classid)
			continue;

		if (!p->mapped)
			continue;

		ret = __pon_qos_tc_map(p->dev, p, NULL, p->tc, false);
		if (ret < 0) {
			netdev_err(p->dev, "%s: queue unmap fail\n", __func__);
			return ret;
		}
	}

	return 0;
}
