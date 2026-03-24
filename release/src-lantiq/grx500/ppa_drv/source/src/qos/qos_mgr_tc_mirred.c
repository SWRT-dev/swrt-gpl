/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

#include <linux/list.h>
#include "qos_mgr_stack_al.h"
#include <net/qos_mgr_common.h>
#include <net/switch_api/gsw_types.h>
#include <net/switch_api/lantiq_gsw.h>
#include <net/switch_api/gsw_dev.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#include <net/flow_dissector.h>
#include <net/pkt_cls.h>
#include <net/tc_act/tc_mirred.h>
#include <net/tc_act/tc_gact.h>
#include "qos_mgr_tc_pce.h"
#include "qos_mgr_tc_mirred.h"

struct qos_mgr_mirr_filter {
	int proto;
	struct flow_dissector_key_vlan key;
	struct flow_dissector_key_vlan mask;
	bool drop;
	enum pce_type pce_type;
};

struct mirr_node {
	struct net_device *dev;
	struct list_head list;
};

static LIST_HEAD(mirr_list);

static int qos_mgr_parse_flower(struct tc_cls_flower_offload *f,
				struct qos_mgr_mirr_filter *flt)
{

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
		return -EINVAL;
	}
	pr_debug("%s: Supported key used: 0x%x\n", __func__,
		f->dissector->used_keys);

	memset(flt, 0, sizeof(*flt));

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
		pr_debug("%s: match vid: %#x/%#x pcp: %#x\n", __func__,
			 key->vlan_id,
			 key->vlan_priority,
			 mask->vlan_id);
		flt->key = *key;
		flt->mask = *mask;
	}

	return 0;
}

static int qos_mgr_parse_act_mirred(struct net_device *dev,
				    struct tcf_exts *exts,
				    bool *drop_act)
{
	const struct tc_action *a;
	LIST_HEAD(actions);
	struct net_device *mirr_dev;
	struct mirr_node *mirr = NULL;
	int ifindex;
	int ret = -EINVAL;

	*drop_act = false;

	if (tc_no_actions(exts))
		return -EINVAL;

	tcf_exts_to_list(exts, &actions);
	list_for_each_entry(a, &actions, list) {
		if (is_tcf_mirred_redirect(a)) {
			ifindex = tcf_mirred_ifindex(a);
			mirr_dev = dev_get_by_index(dev_net(dev),
						    ifindex);
			if (!mirr_dev)
				continue;
			mirr = kzalloc(sizeof(struct mirr_node), GFP_KERNEL);
			if (!mirr)
				return QOS_MGR_FAILURE;
			mirr->dev = mirr_dev;
			list_add(&mirr->list, &mirr_list);
			ret = QOS_MGR_SUCCESS;
		}
		if (is_tcf_gact_shot(a))
			*drop_act =  true;
	}

	pr_debug("%s: ret: %d\n", __func__, ret);
	return ret;
}

static int qos_mgr_flow_get_bp_id(struct net_device *dev)
{
	dp_subif_t *subif = NULL;
	int err, br;

	subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
	if (!subif) {
		pr_debug("%s Memory Allocatio Failed\n", __func__);
		return QOS_MGR_FAILURE;
	}

	err = dp_get_netif_subifid(dev, NULL, NULL, NULL, subif, 0);
	if (err != DP_SUCCESS) {
		kfree(subif);
		return QOS_MGR_FAILURE;
	}

	br = subif->bport;
	kfree(subif);
	return br;
}

static int qos_mgr_vlan_to_pce_single(struct qos_mgr_mirr_filter *flt,
				      GSW_PCE_rule_t *rule)
{
	rule->pattern.bSLAN_Vid = 1;
	rule->pattern.nSLAN_Vid = flt->key.vlan_id;
	rule->pattern.nOuterVidRange = 0x0;
	rule->pattern.bSVidRange_Select = 0;
	rule->pattern.bOuterVid_Original = 0;
	rule->action.ePortFilterType_Action = 1;

	if (flt->drop) {
		pr_debug("%s: Mirring VLAN forward\n", __func__);
		flt->pce_type = PCE_MIRR_VLAN_DROP;
	} else {
		pr_debug("%s: Mirring VLAN drop\n", __func__);
		flt->pce_type = PCE_MIRR_VLAN_FWD;
	}

	return 0;
}

static int qos_mgr_vlan_to_pce_wildcard(struct qos_mgr_mirr_filter *flt,
					GSW_PCE_rule_t *rule)
{
	rule->pattern.bSLAN_Vid = 1;
	rule->pattern.nSLAN_Vid = 0;
	rule->pattern.nOuterVidRange = 4095;
	rule->pattern.bSVidRange_Select = 1;
	rule->pattern.bOuterVid_Original = 0;
	rule->action.ePortFilterType_Action = 1;

	pr_debug("%s: Mirring VLAN wildcard\n", __func__);
	flt->pce_type = PCE_MIRR_VLAN_WILD;

	return 0;
}

static int qos_mgr_vlan_to_pce(struct qos_mgr_mirr_filter *flt,
			       GSW_PCE_rule_t *rule)
{
	int ret = QOS_MGR_FAILURE;

	if (!flt->mask.vlan_id && !flt->mask.vlan_id)
		ret = qos_mgr_vlan_to_pce_wildcard(flt, rule);
	else if (flt->key.vlan_id && flt->mask.vlan_id == 0xfff)
		ret = qos_mgr_vlan_to_pce_single(flt, rule);

	return ret;
}

static int qos_mgr_all_to_pce(struct qos_mgr_mirr_filter *flt,
			      GSW_PCE_rule_t *rule)
{
	rule->pattern.bSLAN_Vid = 1;
	rule->pattern.nSLAN_Vid = 0;
	rule->pattern.nOuterVidRange = 4095;
	rule->pattern.bSLANVid_Exclude = 1;
	rule->pattern.bSVidRange_Select = 1;
	rule->pattern.bOuterVid_Original = 0;
	rule->action.ePortFilterType_Action = 1;

	if (flt->drop) {
		pr_debug("%s: Mirring VLAN untag drop\n", __func__);
		flt->pce_type = PCE_MIRR_UNTAG_DROP;
	} else {
		pr_debug("%s: Mirring VLAN untag forward\n", __func__);
		flt->pce_type = PCE_MIRR_UNTAG_FWD;
	}

	return 0;
}

static int qos_mgr_tc_to_pce(struct net_device *dev,
			     struct net_device *mirr_dev,
			     struct qos_mgr_mirr_filter *flt,
			     GSW_PCE_rule_t *pceRule)
{
	dp_subif_t *dp_subif = NULL;
	int ret = 0;
	int bp = 0, pidx;

	if (!flt->drop) {
		bp = qos_mgr_flow_get_bp_id(mirr_dev);
		if (bp < 0) {
			pr_err("%s: cannot get BP id from DPM\n", __func__);
			return QOS_MGR_FAILURE;
		}
	}

	dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
	if (!dp_subif) {
		pr_debug("%s Memory Allocatio Failed\n", __func__);
		return QOS_MGR_FAILURE;
	}
	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, dp_subif, 0);
	if (ret != DP_SUCCESS) {
		pr_err("%s: dp_get_netif_subifid failed, dev: %s\n", __func__,
			dev->name);
		kfree(dp_subif);
		return QOS_MGR_FAILURE;
	}

	pr_debug("%s: using port: %d subif: %d\n",
		__func__, dp_subif->port_id, dp_subif->subif);

	pceRule->pattern.bEnable = 1;
	pceRule->pattern.bPortIdEnable = 1;
	pceRule->pattern.nPortId = dp_subif->port_id;
	pceRule->pattern.bSubIfIdEnable = 1;
	pceRule->pattern.nSubIfId = dp_subif->subif;

	kfree(dp_subif);

	if (flt->proto == ETH_P_ALL)
		ret = qos_mgr_all_to_pce(flt, pceRule);
	else if (eth_type_vlan(flt->proto))
		ret = qos_mgr_vlan_to_pce(flt, pceRule);
	else {
		pr_err("%s: Unsupported mirred TC protocol\n", __func__);
		return QOS_MGR_FAILURE;
	}

	if (ret != QOS_MGR_SUCCESS)
		return ret;

	pidx = ((bp / 16)) % 16;
	if (flt->drop)
		pceRule->action.nForwardPortMap[pidx] = 0;
	else
		pceRule->action.nForwardPortMap[pidx] = 1 << (bp - (pidx << 4));

	pr_debug("%s: PCE rule prepared\n", __func__);
	return ret;
}

int qos_mgr_mirred_offload(struct net_device *dev,
			   struct tc_cls_flower_offload *f,
			   uint32_t tc_handle)
{
	struct qos_mgr_mirr_filter flt = {0};
	GSW_PCE_rule_t *pceRule = NULL;
	struct mirr_node *p, *n;
	int pref = f->common.prio >> 16;
	int ret = 0;

	ret = qos_mgr_parse_flower(f, &flt);
	if (ret != QOS_MGR_SUCCESS)
		return QOS_MGR_FAILURE;

	ret = qos_mgr_parse_act_mirred(dev, f->exts, &flt.drop);
	if (ret != QOS_MGR_SUCCESS)
		goto err;

	list_for_each_entry_safe(p, n, &mirr_list, list) {
		pceRule = kzalloc(sizeof(*pceRule), GFP_KERNEL);
		if (!pceRule) {
			ret = QOS_MGR_FAILURE;
			goto err;
		}

		ret = qos_mgr_tc_to_pce(dev, p->dev, &flt, pceRule);
		if (ret != QOS_MGR_SUCCESS) {
			kfree(pceRule);
			goto err;
		}

		ret = qos_mgr_pce_rule_create(dev, tc_handle, pref,
					flt.pce_type, pceRule);
		if (ret != QOS_MGR_SUCCESS) {
			kfree(pceRule);
			goto err;
		}

		kfree(pceRule);
	}

err:
	list_for_each_entry_safe(p, n, &mirr_list, list) {
		dev_put(p->dev);
		list_del(&p->list);
		kfree(p);
	}

	return ret;
}

int qos_mgr_mirred_unoffload(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     uint32_t tc_handle)
{
	int pref = f->common.prio >> 16;
	int ret = 0;

	ret = qos_mgr_pce_rule_delete(tc_handle, pref);
	return ret;
}
