// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright 2021 MaxLinear, Inc.
 *
 *****************************************************************************/

#include <net/switch_api/gsw_types.h>
#include <net/switch_api/lantiq_gsw.h>
#include <net/switch_api/gsw_dev.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#include <net/datapath_api.h>
#include <net/flow_dissector.h>
#include <net/pkt_cls.h>
#include <net/ipv6.h>
#include "pon_qos_tc_flower.h"
#include "pon_qos_tc_pce.h"
#include "pon_qos_tc_ip_drop.h"

static int parse_mask(struct net_device *dev,
		      GSW_PCE_rule_t *rule,
		      u32 *mask,
		      int mask_size)
{
	int m, i, idx = 0;
	u32 mask_cpy;

	if (!mask)
		return -EIO;

	if (mask_size != 1 && mask_size != 4)
		return -EIO;

	/* Convert mask to nimble */
	for (m = 0; m < mask_size; m++) {
		mask_cpy = htonl(mask[m]);
		for (i = 0; i < 4; i++, idx += 2) {
			if (!(mask_cpy & 0x0f))
				rule->pattern.nDstIP_Mask |= 1 << idx;
			if (!(mask_cpy & 0xf0))
				rule->pattern.nDstIP_Mask |= 1 << (idx + 1);
			mask_cpy >>= 8;
		}
	}

	/* Reorder bytes in case of IPv6 masks because network byte order
	 * is expected. Not required in case of IPv4 as only the lowest
	 * significant byte is used and all others are ignored.
	 */
	if (mask_size == 4)
		rule->pattern.nDstIP_Mask = htonl(rule->pattern.nDstIP_Mask);

	netdev_dbg(dev, "%s: nDstIP_Mask: %x\n",
		   __func__, rule->pattern.nDstIP_Mask);

	return 0;
}

static int parse_ipv4(struct net_device *dev,
		      struct tc_cls_flower_offload *f,
		      GSW_PCE_rule_t *rule)
{
	struct flow_dissector_key_ipv4_addrs *key;
	struct flow_dissector_key_ipv4_addrs *mask;

	if (!dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_IPV4_ADDRS))
		return -EIO;

	key = skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_IPV4_ADDRS,
					f->key);
	mask = skb_flow_dissector_target(f->dissector,
					 FLOW_DISSECTOR_KEY_IPV4_ADDRS,
					 f->mask);

	rule->pattern.eDstIP_Select = GSW_PCE_IP_V4;
	rule->pattern.nDstIP.nIPv4 = htonl(key->dst);
	if (ipv4_is_multicast(key->dst))
		rule->action.ePortFilterType_Action =
			GSW_PCE_PORT_FILTER_ACTION_6;

	return parse_mask(dev, rule, &mask->dst, 1);
}

static int parse_ipv6(struct net_device *dev,
		      struct tc_cls_flower_offload *f,
		      GSW_PCE_rule_t *rule)
{
	struct flow_dissector_key_ipv6_addrs *key;
	struct flow_dissector_key_ipv6_addrs *mask;
	int i;

	if (!dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_IPV6_ADDRS))
		return -EIO;

	key = skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_IPV6_ADDRS,
					f->key);
	mask = skb_flow_dissector_target(f->dissector,
					 FLOW_DISSECTOR_KEY_IPV6_ADDRS,
					 f->mask);

	rule->pattern.eDstIP_Select = GSW_PCE_IP_V6;
	/* IPv6 addresses inside structure GSW_PCE_rule_t are stored
	 * in an array of 16bit values
	 */
	for (i = 0; i < 8; ++i) {
		((uint16_t *)rule->pattern.nDstIP.nIPv6)[i] =
			htons(((uint16_t *)&key->dst)[i]);
	}
	if (ipv6_addr_is_multicast(&key->dst))
		rule->action.ePortFilterType_Action =
			GSW_PCE_PORT_FILTER_ACTION_6;

	return parse_mask(dev, rule, mask->dst.in6_u.u6_addr32, 4);
}

static int parse_ip(struct net_device *dev,
		    struct tc_cls_flower_offload *f,
		    GSW_PCE_rule_t *pce_rule)
{
	u16 addr_type;
	struct flow_dissector_key_control *key;

	if (!dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_CONTROL))
		return -EIO;

	key = skb_flow_dissector_target(f->dissector,
					FLOW_DISSECTOR_KEY_CONTROL,
					f->key);
	addr_type = key->addr_type;

	pce_rule->action.ePortFilterType_Action = GSW_PCE_PORT_FILTER_ACTION_1;

	switch (addr_type) {
	case FLOW_DISSECTOR_KEY_IPV4_ADDRS:
		return parse_ipv4(dev, f, pce_rule);
	case FLOW_DISSECTOR_KEY_IPV6_ADDRS:
		return parse_ipv6(dev, f, pce_rule);
	default:
		return -EIO;
	}

	return -EIO;
}

static int pon_qos_tc_to_pce(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     GSW_PCE_rule_t *pce_rule)
{
	dp_subif_t dp_subif;
	int ret = 0;

	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &dp_subif, 0);
	if (ret != DP_SUCCESS) {
		netdev_err(dev, "%s: dp_get_netif_subifid failed\n",
			   __func__);
		return -ENODEV;
	}

	netdev_dbg(dev, "%s: using port: %d subif: %d\n",
		   __func__, dp_subif.port_id, dp_subif.subif_groupid);

	pce_rule->pattern.bEnable = 1;
	pce_rule->pattern.bPortIdEnable = 1;
	pce_rule->pattern.nPortId = dp_subif.port_id;
	pce_rule->pattern.bSubIfIdEnable = 1;
	pce_rule->pattern.nSubIfId = dp_subif.subif_groupid;
	pce_rule->pattern.bSLAN_Vid = 1;
	pce_rule->pattern.nSLAN_Vid = 0;
	pce_rule->pattern.nOuterVidRange = 4095;
	pce_rule->pattern.bSVidRange_Select = 1;
	pce_rule->pattern.bInsertionFlag_Enable = 1;
	pce_rule->pattern.nInsertionFlag = 0;

	if (!eth_type_vlan(f->common.protocol))
		pce_rule->pattern.bSLANVid_Exclude = 1;

	ret = parse_ip(dev, f, pce_rule);
	if (ret < 0)
		return ret;

	pce_rule->action.nForwardPortMap[0] = 0x0000;

	netdev_dbg(dev, "%s: PCE rule prepared\n", __func__);
	return ret;
}

int pon_qos_ip_drop_offload(struct net_device *dev,
			    struct tc_cls_flower_offload *f,
			    uint32_t tc_handle)
{
	GSW_PCE_rule_t *pce_rule = NULL;
	int pref = f->common.prio >> 16;
	int ret = 0;

	pce_rule = kzalloc(sizeof(*pce_rule), GFP_KERNEL);
	if (!pce_rule) {
		ret = -ENOMEM;
		goto err;
	}

	ret = pon_qos_tc_to_pce(dev, f, pce_rule);
	if (ret != 0) {
		kfree(pce_rule);
		goto err;
	}

	ret = pon_qos_pce_rule_create(dev, tc_handle, pref,
				      PCE_UNCOMMON, pce_rule);
	if (ret != 0) {
		kfree(pce_rule);
		goto err;
	}

	kfree(pce_rule);

	ret = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_IP_DROP,
					    NULL, NULL);
	if (ret < 0) {
		(void)pon_qos_pce_rule_delete(tc_handle, pref);
		goto err;
	}

err:
	return ret;
}

int pon_qos_ip_drop_unoffload(struct net_device *dev,
			      struct tc_cls_flower_offload *f,
			      uint32_t tc_handle)
{
	int pref = f->common.prio >> 16;
	int ret = 0;

	ret = pon_qos_pce_rule_delete(tc_handle, pref);
	return ret;
}
