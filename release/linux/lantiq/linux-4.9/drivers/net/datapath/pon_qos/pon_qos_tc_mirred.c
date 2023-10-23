// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/list.h>
#include <net/switch_api/gsw_types.h>
#include <net/switch_api/lantiq_gsw.h>
#include <net/switch_api/gsw_dev.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#include <net/datapath_api.h>
#include <net/flow_dissector.h>
#include <net/pkt_cls.h>
#include <net/tc_act/tc_mirred.h>
#include <linux/version.h>
#include "pon_qos_tc_flower.h"
#include <net/tc_act/tc_gact.h>
#include "pon_qos_tc_pce.h"
#include "pon_qos_tc_parser.h"
#include "pon_qos_tc_mirred.h"

struct pon_qos_mirr_filter {
	int proto;
	struct flow_dissector_key_vlan key;
	struct flow_dissector_key_vlan mask;
	bool drop;
	enum pce_type pce_type;
};

static int pon_qos_parse_flower(struct net_device *dev,
				struct tc_cls_flower_offload *f,
				struct pon_qos_mirr_filter *flt)
{
	memset(flt, 0, sizeof(*flt));

	if (f->dissector->used_keys &
			~(BIT(FLOW_DISSECTOR_KEY_CONTROL) |
				BIT(FLOW_DISSECTOR_KEY_BASIC) |
				BIT(FLOW_DISSECTOR_KEY_VLAN) |
				BIT(FLOW_DISSECTOR_KEY_ICMP))) {
		pr_debug("%s: Unsupported key used: 0x%x\n", __func__,
			 f->dissector->used_keys);
		return -EINVAL;
	}
	pr_debug("%s: Supported key used: 0x%x\n", __func__,
		 f->dissector->used_keys);

	flt->proto = f->common.protocol;
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
		netdev_dbg(dev, "%s: match vid: %#x/%#x pcp: %#x\n", __func__,
			   key->vlan_id,
			   key->vlan_priority,
			   mask->vlan_id);
		flt->key = *key;
		flt->mask = *mask;
	}

	return 0;
}

static int pon_qos_parse_act_mirred(struct net_device *dev,
				    struct tcf_exts *exts,
				    struct net_device **mirr_dev,
				    bool *drop_act)
{
	const struct tc_action *a;
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	LIST_HEAD(actions);
	int ifindex;
#else
	int i;
#endif
	int ret = -EINVAL;

	*drop_act = false;

#if (KERNEL_VERSION(4, 14, 0) > LINUX_VERSION_CODE)
	if (tc_no_actions(exts))
#else
	if (!tcf_exts_has_actions(exts))
#endif
		return -EINVAL;

#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	tcf_exts_to_list(exts, &actions);
	list_for_each_entry(a, &actions, list) {
		if (is_tcf_mirred_redirect(a) && ret != 0) {
			ifindex = tcf_mirred_ifindex(a);
			*mirr_dev = dev_get_by_index(dev_net(dev),
						     ifindex);
#else
	tcf_exts_for_each_action(i, a, exts) {
		if (is_tcf_mirred_egress_redirect(a) && ret != 0) {
			*mirr_dev = tcf_mirred_dev(a);
#endif
			if (!(*mirr_dev))
				continue;
			ret = 0;
		}
		if (is_tcf_gact_shot(a))
			*drop_act =  true;
	}

	netdev_dbg(dev, "%s: ret: %d\n", __func__, ret);
	return ret;
}

static int pon_qos_flow_get_bp_id(struct net_device *dev)
{
	dp_subif_t subif;
	int err;

	err = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
	if (err != DP_SUCCESS)
		return -ENODEV;

	return subif.bport;
}

static int pon_qos_vlan_to_pce_single(struct net_device *dev,
				      struct pon_qos_mirr_filter *flt,
				      GSW_PCE_rule_t *rule)
{
	rule->pattern.bSLAN_Vid = 1;
	rule->pattern.nSLAN_Vid = flt->key.vlan_id;
	rule->pattern.nOuterVidRange = 0x0;
	rule->pattern.bSVidRange_Select = 0;
	rule->pattern.bOuterVid_Original = 0;

	if (flt->drop) {
		pr_debug("%s: Mirring VLAN drop\n", __func__);
		flt->pce_type = PCE_MIRR_VLAN_DROP;
	} else {
		pr_debug("%s: Mirring VLAN forward\n", __func__);
		flt->pce_type = PCE_MIRR_VLAN_FWD;
	}

	return 0;
}

static int pon_qos_vlan_to_pce_wildcard(struct net_device *dev,
					struct pon_qos_mirr_filter *flt,
					GSW_PCE_rule_t *rule)
{
	rule->pattern.bSLAN_Vid = 1;
	rule->pattern.nSLAN_Vid = 0;
	rule->pattern.nOuterVidRange = 4095;
	rule->pattern.bSVidRange_Select = 1;
	rule->pattern.bOuterVid_Original = 0;

	netdev_dbg(dev, "%s: Mirring VLAN wildcard\n", __func__);
	flt->pce_type = PCE_MIRR_VLAN_WILD;

	return 0;
}

static int pon_qos_vlan_to_pce(struct net_device *dev,
			       struct pon_qos_mirr_filter *flt,
			       GSW_PCE_rule_t *rule)
{
	if (!flt->mask.vlan_id)
		return pon_qos_vlan_to_pce_wildcard(dev, flt, rule);

	if (flt->mask.vlan_id == 0xfff)
		return pon_qos_vlan_to_pce_single(dev, flt, rule);

	return -EIO;
}

static int pon_qos_all_to_pce(struct net_device *dev,
			      struct pon_qos_mirr_filter *flt,
			      GSW_PCE_rule_t *rule)
{
	rule->pattern.bSLAN_Vid = 1;
	rule->pattern.nSLAN_Vid = 0;
	rule->pattern.nOuterVidRange = 4095;
	rule->pattern.bSLANVid_Exclude = 1;
	rule->pattern.bSVidRange_Select = 1;
	rule->pattern.bOuterVid_Original = 0;

	if (flt->drop) {
		netdev_dbg(dev, "%s: Mirring VLAN untag drop\n", __func__);
		flt->pce_type = PCE_MIRR_UNTAG_DROP;
	} else {
		netdev_dbg(dev, "%s: Mirring VLAN untag forward\n", __func__);
		flt->pce_type = PCE_MIRR_UNTAG_FWD;
	}

	return 0;
}

static int pon_qos_tc_to_pce(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     struct net_device *mirr_dev,
			     struct pon_qos_mirr_filter *flt,
			     GSW_PCE_rule_t *pce_rule)
{
	int ret = 0;
	int bp = 0, pidx;

	pce_rule->pattern.bEnable = 1;
	flt->pce_type = PCE_COMMON;

	pon_qos_tc2pce_subif_parse(dev, pce_rule, FL_FLOW_KEY_IFINDEX(f));
	pon_qos_tc2pce_proto_parse(dev, f, pce_rule);

	if (pon_qos_tc_parse_is_mcc(dev, f)) {
		pon_qos_tc2pce_vlan_parse(dev, f, pce_rule);
		pon_qos_tc2pce_icmp_parse(dev, f, pce_rule);
		/* use only on reinserted packets */
		pce_rule->pattern.bInsertionFlag_Enable = 1;
		pce_rule->pattern.nInsertionFlag = 1;
	} else if (ntohs(flt->proto) == ETH_P_ALL) {
		ret = pon_qos_all_to_pce(dev, flt, pce_rule);
	} else if (eth_type_vlan(flt->proto)) {
		ret = pon_qos_vlan_to_pce(dev, flt, pce_rule);
	} else {
		netdev_err(dev, "%s: Unsupported mirred TC protocol\n",
			   __func__);
		return -EPROTONOSUPPORT;
	}

	if (ret != 0)
		return ret;

	if (pon_qos_tc_parse_is_mcc(dev, f))
		pce_rule->action.ePortMapAction =
			GSW_PCE_ACTION_PORTMAP_ALTERNATIVE;
	else
		pce_rule->action.ePortFilterType_Action =
			GSW_PCE_PORT_FILTER_ACTION_1;

	if (flt->drop) {
		netdev_dbg(dev, "%s: PCE rule prepared\n", __func__);
		return ret;
	}

	bp = pon_qos_flow_get_bp_id(mirr_dev);
	if (bp < 0) {
		netdev_err(dev, "%s: cannot get BP id from DPM\n",
			   __func__);
		return bp;
	}

	/* Set destination bridge port id in 256 bit nForwardPortMap
	 * which is split into 16 u16 blocks.
	 */
	pidx = ((bp / 16)) % 16;
	pce_rule->action.nForwardPortMap[pidx] = 1 << (bp - (pidx << 4));

	netdev_dbg(dev, "%s: PCE rule prepared\n", __func__);
	return ret;
}

int pon_qos_mirred_offload(struct net_device *dev,
			   struct tc_cls_flower_offload *f,
			   uint32_t tc_handle)
{
	struct pon_qos_mirr_filter flt = {0};
	struct net_device *mirr_dev = NULL;
	GSW_PCE_rule_t *pce_rule = NULL;
	int pref = f->common.prio >> 16;
	int ret = 0;

	ret = pon_qos_parse_flower(dev, f, &flt);
	if (ret != 0)
		return ret;

	ret = pon_qos_parse_act_mirred(dev, f->exts, &mirr_dev, &flt.drop);
	if (ret != 0)
		return ret;

	pce_rule = kzalloc(sizeof(*pce_rule), GFP_KERNEL);
	if (!pce_rule) {
		ret = -1;
		goto err;
	}

	ret = pon_qos_tc_to_pce(dev, f, mirr_dev, &flt, pce_rule);
	if (ret != 0) {
		kfree(pce_rule);
		goto err;
	}

	ret = pon_qos_pce_rule_create(dev, tc_handle, pref,
				      flt.pce_type, pce_rule);
	if (ret != 0) {
		kfree(pce_rule);
		goto err;
	}

	kfree(pce_rule);

	ret = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_MIRRED,
					    NULL, NULL);
	if (ret < 0) {
		(void)pon_qos_pce_rule_delete(tc_handle, pref);
		goto err;
	}

err:
#if (KERNEL_VERSION(4, 19, 0) > LINUX_VERSION_CODE)
	dev_put(mirr_dev);
#endif

	return ret;
}

int pon_qos_mirred_unoffload(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     uint32_t tc_handle)
{
	int pref = f->common.prio >> 16;
	int ret = 0;

	ret = pon_qos_pce_rule_delete(tc_handle, pref);
	return ret;
}
