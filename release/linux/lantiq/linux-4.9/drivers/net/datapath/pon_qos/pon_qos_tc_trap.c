// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include <linux/types.h>
#include <linux/version.h>
#include <net/datapath_api.h>
#include <net/pkt_cls.h>
#include <net/datapath_api_qos.h>
#include <net/switch_api/lantiq_gsw_flow.h>
#include "pon_qos_tc_flower.h"
#include "pon_qos_tc_vlan_prepare.h"
#include "pon_qos_tc_pce.h"
#include "pon_qos_tc_trap.h"
#include "pon_qos_tc_parser.h"

LIST_HEAD(tc_trap_storage);

struct pce_rule_storage {
	/** TC command handle used as dual identificator */
	s32 handle;
	/** TC command preference used as dual identificator */
	int pref;
	/** Logical Port Id. The valid range is hardware dependent */
	u32 logicalportid;
	/** Sub interface ID group,
	 *  The valid range is hardware/protocol dependent
	 */
	u32 subifidgroup;
	/** PCE TABLE Region */
	GSW_PCE_RuleRegion_t	region;
	/** Rule Index in the PCE Table */
	u32	nIndex;
	/** Sub-Block Type ID */
	int	subblk_id;
	struct list_head list;
};

#define TC_TRAP_SUBBLK_SIZE 32

static void pon_qos_trap_set_default(struct net_device *dev,
				     GSW_PCE_rule_t *pce_rule,
				     struct dp_pce_blk_info *pce_blk_info)
{
	struct pce_rule_storage *p;

	/* Set default values for trap action */
	pce_rule->action.ePortMapAction = GSW_PCE_ACTION_PORTMAP_CPU;
	pce_rule->pattern.bEnable = 1;
	pce_rule->pattern.bInsertionFlag_Enable = 1;
	pce_rule->pattern.nInsertionFlag = 0;
	pce_rule->pattern.bSLAN_Vid = 0;
	pce_rule->region = GSW_PCE_RULE_COMMMON;

	pce_blk_info->region = pce_rule->region;
	pce_blk_info->info.subblk_size = TC_TRAP_SUBBLK_SIZE;
	strncpy(pce_blk_info->info.subblk_name, "TRAP",
		sizeof(pce_blk_info->info.subblk_name));

	/* All PCE rules for trap action should be put in the same subblock,
	 * take subblk_id from the first rule on the list. If there are no
	 * rules in storage, leave subblk_id equal to 0, to make DP create new
	 * subblock for trap action rules. */
	list_for_each_entry(p, &tc_trap_storage, list) {
		pce_blk_info->info.subblk_id = p->subblk_id;
	}

	return;
}

static void pon_qos_trap_parse_key_eth(struct net_device *dev,
				       const struct tc_cls_flower_offload *f,
				       GSW_PCE_rule_t *pce_rule)
{
	int i = 0;
	int idx = 0;

	if (dissector_uses_key(f->dissector, FLOW_DISSECTOR_KEY_ETH_ADDRS)) {
		struct flow_dissector_key_eth_addrs *key =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_ETH_ADDRS,
						  f->key);
		struct flow_dissector_key_eth_addrs *mask =
			skb_flow_dissector_target(f->dissector,
						  FLOW_DISSECTOR_KEY_ETH_ADDRS,
						  f->mask);

		pce_rule->pattern.bMAC_DstEnable =
						!is_zero_ether_addr(key->dst);
		pce_rule->pattern.bMAC_SrcEnable =
						!is_zero_ether_addr(key->src);

		netdev_dbg(dev, "%s: bMAC_DstEnable = %d bMAC_SrcEnable = %d\n",
			   __func__, pce_rule->pattern.bMAC_DstEnable,
			   pce_rule->pattern.bMAC_SrcEnable);

		/* Set bit in mask for each nibble different from F */
		for (i = ETH_ALEN - 1; i >= 0; i--, idx += 2) {
			if ((mask->dst[i] & 0x0f) != 0xf)
				pce_rule->pattern.nMAC_DstMask |= 1 << idx;
			if ((mask->dst[i] & 0xf0) != 0xf0)
				pce_rule->pattern.nMAC_DstMask |=
					1 << (idx + 1);

			if ((mask->src[i] & 0x0f) != 0xf)
				pce_rule->pattern.nMAC_SrcMask |= 1 << idx;
			if ((mask->src[i] & 0xf0) != 0xf0)
				pce_rule->pattern.nMAC_SrcMask |=
					1 << (idx + 1);
		}

		if (pce_rule->pattern.bMAC_DstEnable == 1) {
			for (i = 0; i < ARRAY_SIZE(key->dst); i++) {
				pce_rule->pattern.nMAC_Dst[i] = key->dst[i];
				netdev_dbg(dev, "%s: pce_rule->pattern.nMAC_Dst[i] = %d\n",
					   __func__,
					   pce_rule->pattern.nMAC_Dst[i]);
			}
		}

		if (pce_rule->pattern.bMAC_SrcEnable == 1) {
			for (i = 0; i < ARRAY_SIZE(key->src); i++) {
				pce_rule->pattern.nMAC_Src[i] = key->src[i];
				netdev_dbg(dev, "%s: pce_rule->pattern.nMAC_Src[i] = %d\n",
					   __func__,
					   pce_rule->pattern.nMAC_Src[i]);
			}
		}
	}
}

static void pon_qos_trap_set_traffic_class(struct net_device *dev,
					   GSW_PCE_rule_t *pce_rule,
					   unsigned int classid)
{
	netdev_dbg(dev, "%s: eTrafficClassAction = 0x%x\n",
		   __func__, classid);
	if (classid != 0) {
		pce_rule->action.eTrafficClassAction =
			GSW_PCE_ACTION_TRAFFIC_CLASS_ALTERNATIVE;
		pce_rule->action.nTrafficClassAlternate = classid & 0xf;

		netdev_dbg(dev, "%s: eTrafficClassAction = %d\n",
			   __func__, pce_rule->action.eTrafficClassAction);
		netdev_dbg(dev, "%s: nTrafficClassAlternate = %d\n",
			   __func__, pce_rule->action.nTrafficClassAlternate);
	}
}

static int pon_qos_trap_storage_add(struct net_device *dev,
				    GSW_PCE_rule_t *pce_rule,
				    struct dp_pce_blk_info *pce_blk_info,
				    uint32_t tc_handle,
				    int pref)
{
	struct pce_rule_storage *pce_rule_storage;

	pce_rule_storage = kzalloc(sizeof(*pce_rule_storage), GFP_KERNEL);
	if (!pce_rule_storage)
		return -ENOMEM;

	pce_rule_storage->handle = tc_handle;
	pce_rule_storage->pref = pref;
	pce_rule_storage->nIndex = pce_rule->pattern.nIndex;
	pce_rule_storage->logicalportid = pce_rule->logicalportid;
	pce_rule_storage->subifidgroup = pce_rule->subifidgroup;
	pce_rule_storage->region = pce_blk_info->region;
	pce_rule_storage->subblk_id = pce_blk_info->info.subblk_id;

	list_add(&pce_rule_storage->list, &tc_trap_storage);

	return 0;
}

int pon_qos_trap_offload(struct net_device *dev,
			 const struct tc_cls_flower_offload *f,
			 uint32_t tc_handle)
{
	struct dp_pce_blk_info *pce_blk_info;
	GSW_PCE_rule_t *pce_rule;
	int pref = f->common.prio >> 16;
	int ret, index;

	pce_rule = kzalloc(sizeof(*pce_rule), GFP_ATOMIC);
	if (!pce_rule)
		return -ENOMEM;

	pce_blk_info = kzalloc(sizeof(*pce_blk_info), GFP_ATOMIC);
	if (!pce_blk_info) {
		kfree(pce_rule);
		return -ENOMEM;
	}

	pon_qos_trap_set_default(dev, pce_rule, pce_blk_info);

	/* Set subif and portid */
	pon_qos_tc2pce_subif_parse(dev, pce_rule, FL_FLOW_KEY_IFINDEX(f));
	pce_blk_info->info.portid = pce_rule->pattern.nPortId;
	pce_blk_info->info.subif = pce_rule->pattern.nSubIfId;

	/* Set protocol based on ip_proto and ethertype based on n_proto */
	pon_qos_tc2pce_proto_parse(dev, f, pce_rule);

	/* Set MAC source and destination */
	pon_qos_trap_parse_key_eth(dev, f, pce_rule);

	/* Set SLAN_Vid */
	pon_qos_tc2pce_vlan_parse(dev, f, pce_rule);

	/* Set ICMP type */
	pon_qos_tc2pce_icmp_parse(dev, f, pce_rule);

	/* Parse hw_tc */
	pon_qos_trap_set_traffic_class(dev, pce_rule, f->classid);

	index = dp_pce_rule_add(pce_blk_info, pce_rule);
	if (index < 0) {
		netdev_err(dev, "%s: dp_pce_rule_add failed err %d\n",
			   __func__, index);
		kfree(pce_rule);
		kfree(pce_blk_info);
		return -EINVAL;
	}
	netdev_dbg(dev, "%s: pce rule added with index %d\n", __func__, index);

	/* Save data needed for rule deletion */
	ret = pon_qos_trap_storage_add(dev, pce_rule,
				       pce_blk_info,
				       tc_handle, pref);
	if (ret != DP_SUCCESS) {
		kfree(pce_rule);
		kfree(pce_blk_info);
		return ret;
	}

	kfree(pce_rule);
	kfree(pce_blk_info);

	ret = pon_qos_tc_flower_storage_add(dev, f->cookie,
					    TC_TYPE_TRAP,
					    NULL, NULL);
	if (ret < 0)
		(void)pon_qos_trap_unoffload(dev, NULL, tc_handle);

	return 0;
}

int pon_qos_trap_unoffload(struct net_device *dev,
			   const struct tc_cls_flower_offload *f,
			   uint32_t tc_handle)
{
	struct dp_pce_blk_info pce_blk_info = {0};
	struct pce_rule_storage *p;
	GSW_PCE_ruleDelete_t *pce_rule;
	int pref = f->common.prio >> 16;
	bool deletion = false;
	int ret;

	list_for_each_entry(p, &tc_trap_storage, list) {
		if (tc_handle == p->handle && pref == p->pref) {
			deletion = true;
			break;
		}
	}

	if (!deletion) {
		pr_debug("%s: nothing to delete\n", __func__);
		return -EINVAL;
	}

	pce_rule = kzalloc(sizeof(*pce_rule), GFP_ATOMIC);
	if (!pce_rule)
		return -ENOMEM;

	pce_rule->logicalportid = p->logicalportid;
	pce_blk_info.info.portid = p->logicalportid;
	netdev_dbg(dev, "%s: logicalportid  = %d\n", __func__,
		   pce_rule->logicalportid);

	pce_rule->subifidgroup = p->subifidgroup;
	pce_blk_info.info.subif = p->subifidgroup;
	netdev_dbg(dev, "%s: subifidgroup  = %d\n", __func__,
		   pce_rule->subifidgroup);

	pce_rule->region = p->region;
	pce_blk_info.region = p->region;
	netdev_dbg(dev, "%s: region  = %d\n", __func__,
		   pce_rule->region);

	pce_rule->nIndex = p->nIndex;
	netdev_dbg(dev, "%s: nIndex  = %d\n", __func__,
		   pce_rule->nIndex);

	pce_blk_info.info.subblk_id = p->subblk_id;
	netdev_dbg(dev, "%s: subblk_id  = %d\n", __func__,
		   pce_blk_info.info.subblk_id);

	ret = dp_pce_rule_del(&pce_blk_info, pce_rule);
	if (ret < 0) {
		netdev_err(dev, "%s: dp_pce_rule_del failed err %d\n",
			   __func__, ret);
		return ret;
	}
	netdev_dbg(dev, "%s: deleted pce rule with index %d\n", __func__,
		   pce_rule->nIndex);

	list_del(&p->list);
	kfree(p);
	kfree(pce_rule);

	return 0;
}
