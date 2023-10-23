// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/version.h>
#include <net/datapath_api.h>
#include <net/pkt_cls.h>
#include <net/datapath_api_qos.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#include <uapi/linux/icmpv6.h>
#include "pon_qos_tc_parser.h"

#define PON_PORT_ID 2

void pon_qos_tc2pce_subif_parse(struct net_device *dev,
				GSW_PCE_rule_t *pce_rule,
				int ifindex)
{
	struct net_device *indev = NULL;
	dp_subif_t dp_subif = {0};
	int ret;

	if (ifindex)
		indev = dev_get_by_index(dev_net(dev), ifindex);

	if (indev) {
		ret = dp_get_netif_subifid(indev, NULL, NULL, NULL,
					   &dp_subif, 0);
		dev_put(indev);
	} else {
		ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &dp_subif, 0);
	}

	/* If dp_get_netif_subifid() fails assume that this rule should be
	 * configured for all sub interfaces. This is for example the case
	 * for pon0.
	 */
	if (ret != DP_SUCCESS) {
		if (dev != indev)
			return;
		/* If dev is equal indev assume that this rule should be
		 * configured only for PON sub interface.
		 */
		pce_rule->pattern.bPortIdEnable = 1;
		pce_rule->pattern.nPortId = PON_PORT_ID;
		return;
	}

	pce_rule->logicalportid = dp_subif.port_id;
	pce_rule->pattern.bPortIdEnable = 1;
	pce_rule->pattern.nPortId = dp_subif.port_id;

	if (dp_subif.flag_pmapper) {
		pce_rule->pattern.eSubIfIdType =
			GSW_PCE_SUBIFID_TYPE_BRIDGEPORT;

		pce_rule->subifidgroup = dp_subif.bport;
		pce_rule->pattern.bSubIfIdEnable = 1;
		pce_rule->pattern.nSubIfId = dp_subif.bport;
	} else {
		pce_rule->pattern.eSubIfIdType =
			GSW_PCE_SUBIFID_TYPE_GROUP;
		pce_rule->subifidgroup = dp_subif.subif_groupid;
		pce_rule->pattern.bSubIfIdEnable = 1;
		pce_rule->pattern.nSubIfId = dp_subif.subif;
	}

	netdev_dbg(dev, "%s: nPortId = %d\n", __func__,
		   pce_rule->pattern.nPortId);
	netdev_dbg(dev, "%s: nSubIfId = %d\n", __func__,
		   pce_rule->pattern.nSubIfId);
}

void pon_qos_tc2pce_proto_parse(struct net_device *dev,
				const struct tc_cls_flower_offload *f,
				GSW_PCE_rule_t *pce_rule)
{
	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_BASIC)) {
		struct flow_dissector_key_basic *key =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_BASIC,
						  f->key);
		struct flow_dissector_key_basic *mask =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_BASIC,
						  f->mask);
		if (mask->ip_proto) {
			pce_rule->pattern.bProtocolEnable = 1;
			pce_rule->pattern.nProtocol = key->ip_proto;
			netdev_dbg(dev, "%s: bProtocolEnable = %d\n",
				   __func__, pce_rule->pattern.bProtocolEnable);
			netdev_dbg(dev, "%s: nProtocol = %#x\n",
				   __func__, pce_rule->pattern.nProtocol);
		}
		if (mask->n_proto) {
			pce_rule->pattern.bEtherTypeEnable = 1;
			pce_rule->pattern.nEtherType = ntohs(key->n_proto);
			netdev_dbg(dev, "%s: bEtherTypeEnable = %d\n",
				   __func__,
				   pce_rule->pattern.bEtherTypeEnable);
			netdev_dbg(dev, "%s: nEtherType = %#x\n",
				   __func__, pce_rule->pattern.nEtherType);
		}
	}
}

static bool is_mac_set(GSW_PCE_rule_t *pce_rule)
{
	return pce_rule->pattern.bMAC_SrcEnable ||
	       pce_rule->pattern.bMAC_DstEnable;
}

void pon_qos_tc2pce_vlan_parse(struct net_device *dev,
			       const struct tc_cls_flower_offload *f,
			       GSW_PCE_rule_t *pce_rule)
{
	struct flow_dissector_key_vlan *key = NULL;
	struct flow_dissector_key_vlan *mask = NULL;

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_VLAN)) {
		key = skb_flow_dissector_target(f->dissector,
						FLOW_DISSECTOR_KEY_VLAN,
						f->key);
		mask = skb_flow_dissector_target(f->dissector,
						 FLOW_DISSECTOR_KEY_VLAN,
						 f->mask);

		if (key && key->vlan_id && mask && mask->vlan_id == 0xfff) {
			pce_rule->pattern.nSLAN_Vid = key->vlan_id;
			pce_rule->pattern.bSLAN_Vid = 1;
			return;
		}
	}

	if (f->common.protocol == htons(ETH_P_ALL) &&
	    (is_mac_set(pce_rule) || pce_rule->pattern.bSubIfIdEnable))
		return;

	pce_rule->pattern.nOuterVidRange = 4095;
	pce_rule->pattern.bSVidRange_Select = 1;
	pce_rule->pattern.bSLAN_Vid = 1;

	if (eth_type_vlan(f->common.protocol))
		pce_rule->pattern.bSLANVid_Exclude = 0;
	else
		pce_rule->pattern.bSLANVid_Exclude = 1;

	netdev_dbg(dev, "%s: nOuterVidRange = %d\n",
		   __func__, pce_rule->pattern.nOuterVidRange);
	netdev_dbg(dev, "%s: bSVidRange_Select = %d\n",
		   __func__, pce_rule->pattern.bSVidRange_Select);
	netdev_dbg(dev, "%s: bSLANVid_Exclude = %d\n",
		   __func__, pce_rule->pattern.bSLANVid_Exclude);
}

void pon_qos_tc2pce_icmp_parse(struct net_device *dev,
			       const struct tc_cls_flower_offload *f,
			       GSW_PCE_rule_t *pce_rule)
{
	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_ICMP)) {
		struct flow_dissector_key_icmp *key =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_ICMP,
						  f->key);
		struct flow_dissector_key_icmp *mask =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_ICMP,
						  f->mask);

		if (mask->icmp) {
			pce_rule->pattern.bAppDataMSB_Enable = 1;
			pce_rule->pattern.nAppDataMSB = key->icmp;
			pce_rule->pattern.nAppMaskRangeMSB = 0x3;
			netdev_dbg(dev, "%s: nAppDataMSB = %#x\n",
				   __func__, pce_rule->pattern.nAppDataMSB);
		}
	}
}

static int check_mld_type(struct net_device *dev,
			  const struct tc_cls_flower_offload *f)
{
	struct flow_dissector_key_icmp *key = NULL;

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_ICMP)) {
		key = skb_flow_dissector_target(f->dissector,
						FLOW_DISSECTOR_KEY_ICMP,
						f->key);

		switch (key->type) {
		case ICMPV6_MGM_QUERY:
		case ICMPV6_MGM_REPORT:
		case ICMPV6_MGM_REDUCTION:
		case ICMPV6_NI_REPLY:
			netdev_dbg(dev, "MLD type: %u\n", key->type);
			return 0;
		default:
			netdev_dbg(dev, "Invalid MLD type\n");
			return -EIO;
		}
	}

	netdev_dbg(dev, "Missing MLD type\n");
	return -EIO;
}

bool pon_qos_tc_parse_is_mcc(struct net_device *dev,
			     const struct tc_cls_flower_offload *f)
{
	struct flow_dissector_key_basic *key = NULL;
	struct flow_dissector *d = f->dissector;
	int ret;

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_BASIC)) {
		key = skb_flow_dissector_target(d, FLOW_DISSECTOR_KEY_BASIC,
						f->key);

		if (key->ip_proto == IPPROTO_IGMP)
			return true;

		if (key->ip_proto == IPPROTO_ICMPV6) {
			ret = check_mld_type(dev, f);
			if (ret == 0)
				return true;
		}
	}

	return false;
}
