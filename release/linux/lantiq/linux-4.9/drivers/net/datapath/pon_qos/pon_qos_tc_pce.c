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
#include "pon_qos_tc_pce.h"

/** Mark PCE rule index as reserved */
#define PON_QOS_PCE_RESERVED 1
/** Mark PCE rule index as unreserved */
#define PON_QOS_PCE_UNRESERVED 0

/** Maximum numbers of entries per PCE block */
#define PON_QOS_PCE_BLOCK_ENTRIES_MAX 64
/** Special value used for common block */
#define PON_QOS_PCE_BLOCK_COMMON -1
/** PCE sub-block size */
#define TC_PCE_SUBBLK_SIZE 32

/** This structure represent list element storing relation between
 *  TC command and PCE rule.
 */
struct pce_node {
	/** TC command handle used as dual identificator */
	s32 handle;
	/** TC command preference used as dual identificator */
	int pref;
	/** Logical port id, common block is using -1 id. */
	s64 portid;
	/** CTP id, used for PCE block management,
	 *  Common block is using -1 id.
	 */
	s64 subif;
	/** PCE rule index used in hardware */
	u32 index;
	/** Sub-Block Type ID */
	int subblk_id;
	/** Sub-Block type */
	GSW_PCE_RuleRegion_t region;
	struct list_head list;
};

static LIST_HEAD(tc_pce_list);

static int get_subif_data(struct net_device *dev,
			  enum pce_type type,
			  int *portid,
			  int *subif)
{
	dp_subif_t dp_subif;
	s32 ret;

	switch (type) {
	case PCE_COMMON:
		*portid = PON_QOS_PCE_BLOCK_COMMON;
		*subif = PON_QOS_PCE_BLOCK_COMMON;
		pr_debug("%s: portid: %d, subif: %d\n", __func__,
			 PON_QOS_PCE_BLOCK_COMMON, PON_QOS_PCE_BLOCK_COMMON);
		break;
	case PCE_UNCOMMON:
	case PCE_MIRR_VLAN_FWD:
	case PCE_MIRR_VLAN_DROP:
	case PCE_MIRR_VLAN_WILD:
	case PCE_MIRR_UNTAG_FWD:
	case PCE_MIRR_UNTAG_DROP:
		ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &dp_subif, 0);
		if (ret != DP_SUCCESS)
			return -ENODEV;

		pr_debug("%s: portid: %u, subif_groupid: %u\n", __func__,
			 dp_subif.port_id, dp_subif.subif_groupid);
		*portid = dp_subif.port_id;
		*subif = dp_subif.subif_groupid;
		break;
	default:
		pr_err("%s: PCE type unknown\n", __func__);
		return -EINVAL;
	}

	return 0;
}

static int
subblk_id_find(struct net_device *dev,
	       GSW_PCE_RuleRegion_t region,
	       int portid, int subif)
{
	struct pce_node *p;

	list_for_each_entry(p, &tc_pce_list, list) {
		if (p->region == GSW_PCE_RULE_COMMMON &&
		    p->region == region) {
			netdev_dbg(dev, "%s: subblk_id found in COMMON region: %d\n",
				   __func__, p->subblk_id);
			return p->subblk_id;
		}
		if (p->region == GSW_PCE_RULE_CTP &&
		    p->region == region &&
		    p->portid == portid &&
		    p->subif == subif) {
			netdev_dbg(dev, "%s: subblk_id found in CTP region(%lld|%lld): %d\n",
				   __func__, p->portid, p->subif, p->subblk_id);
			return p->subblk_id;
		}
	}

	netdev_dbg(dev, "%s: subblk_id not found\n",
		   __func__);
	return 0;
}

static int pce_index_range_get(enum pce_type type,
			       u32 *start,
			       u32 *stop)
{
	switch (type) {
	case PCE_COMMON:
		*start = 0;
		*stop = PON_QOS_PCE_BLOCK_ENTRIES_MAX;
		break;
	case PCE_UNCOMMON:
		*start = 0;
		*stop = TC_PCE_SUBBLK_SIZE;
		break;
	case PCE_MIRR_VLAN_FWD:
		*start = 0;
		*stop = 16;
		break;
	case PCE_MIRR_VLAN_DROP:
		*start = 16;
		*stop = 26;
		break;
	case PCE_MIRR_VLAN_WILD:
		*start = 26;
		*stop = 27;
		break;
	case PCE_MIRR_UNTAG_FWD:
		*start = 27;
		*stop = 28;
		break;
	case PCE_MIRR_UNTAG_DROP:
		*start = 28;
		*stop = 29;
		break;
	default:
		pr_err("%s: Range not found!\n", __func__);
		return -EINVAL;
	}

	pr_debug("%s: Range assigned, start: %u, stop: %u\n", __func__,
		 *start, *stop);
	return 0;
}

int pon_qos_pce_rule_create(struct net_device *dev,
			    u32 tc_handle, int pref,
			    enum pce_type type,
			    GSW_PCE_rule_t *pce_rule)
{
	struct dp_pce_blk_info *pce_blk_info;
	struct pce_node *tc_pce = NULL;
	struct dp_pce_user_data user_data = {0};
	int portid, subif;
	u32 index;
	s32 ret;

	ret = get_subif_data(dev, type, &portid, &subif);
	if (ret != 0)
		return -ENODEV;

	pce_blk_info = kzalloc(sizeof(*pce_blk_info), GFP_ATOMIC);
	if (!pce_blk_info)
		return -ENOMEM;

	if (type != PCE_COMMON) {
		pce_rule->logicalportid = portid;
		pce_rule->subifidgroup = subif;
		pce_rule->region = GSW_PCE_RULE_CTP;

		pce_blk_info->info.portid = portid;
		pce_blk_info->info.subif = subif;
		pce_blk_info->region = GSW_PCE_RULE_CTP;
		pce_blk_info->info.subblk_size = TC_PCE_SUBBLK_SIZE;
		strlcpy(pce_blk_info->info.subblk_name, "MIRRED",
			sizeof(pce_blk_info->info.subblk_name));
	}

	/* All PCE rules for each region should be put in the same subblock,
	 * take subblk_id from the first rule on the list. If there are no
	 * rules in storage, leave subblk_id equal to 0, to make DP create new
	 * subblock. */
	pce_blk_info->info.subblk_id = subblk_id_find(dev, pce_rule->region,
						      portid, subif);

	ret = pce_index_range_get(type, &user_data.idx_s, &user_data.idx_e);
	if (ret != 0) {
		kfree(pce_blk_info);
		return ret;
	}

	if (type == PCE_COMMON)
		pce_blk_info->info.subblk_size = 8;
	else
		user_data.idx_en = true;

	pce_blk_info->info.user_data = &user_data;

	ret = dp_pce_rule_add(pce_blk_info, pce_rule);
	if (ret < 0) {
		netdev_err(dev, "%s: dp_pce_rule_add failed err %d\n",
			   __func__, ret);
		kfree(pce_blk_info);
		return -EINVAL;
	}

	index = (u32)ret;
	netdev_dbg(dev, "%s: pce rule added with index %d\n", __func__, index);

	tc_pce = kzalloc(sizeof(*tc_pce), GFP_KERNEL);
	if (!tc_pce) {
		kfree(pce_blk_info);
		return ret;
	}

	tc_pce->handle = tc_handle;
	tc_pce->pref = pref;
	tc_pce->index = index;
	tc_pce->portid = portid;
	tc_pce->subif = subif;
	tc_pce->subblk_id = pce_blk_info->info.subblk_id;
	tc_pce->region = pce_blk_info->region;

	list_add(&tc_pce->list, &tc_pce_list);
	kfree(pce_blk_info);

	pr_debug("%s: PCE rule created for lpid/gpid %u/%u\n",
		 __func__, portid, subif);
	return 0;
}

int pon_qos_pce_rule_delete(u32 tc_handle, int pref)
{
	struct dp_pce_blk_info *pce_blk_info;
	struct pce_node *p, *n;
	bool deletion = false;
	GSW_PCE_ruleDelete_t pce_rule = {0};
	GSW_return_t ret;

	list_for_each_entry_safe(p, n, &tc_pce_list, list) {
		if (tc_handle == p->handle && pref == p->pref) {
			deletion = true;
			break;
		}
	}

	if (!deletion) {
		pr_debug("%s: Nothing to delete\n", __func__);
		return -ENOENT;
	}

	pce_blk_info = kzalloc(sizeof(*pce_blk_info), GFP_ATOMIC);
	if (!pce_blk_info) {
		return -ENOMEM;
	}

	if (p->subif != PON_QOS_PCE_BLOCK_COMMON &&
	    p->portid != PON_QOS_PCE_BLOCK_COMMON) {
		pce_rule.logicalportid = p->portid;
		pce_blk_info->info.portid = p->portid;
		pce_rule.subifidgroup = p->subif;
		pce_blk_info->info.subif = p->subif;
		pce_rule.region = GSW_PCE_RULE_CTP;
		pce_blk_info->region = GSW_PCE_RULE_CTP;
	}

	pce_blk_info->info.subblk_id = p->subblk_id;

	pce_rule.nIndex = p->index;

	ret = dp_pce_rule_del(pce_blk_info, &pce_rule);
	if (ret < 0) {
		pr_err("%s: dp_pce_rule_del failed err %d\n", __func__, ret);
		return ret;
	}
	pr_debug("%s: deleted pce rule with index %d\n", __func__,
		 pce_rule.nIndex);


	list_del(&p->list);
	kfree(p);
	kfree(pce_blk_info);

	pr_debug("%s: PCE rule deleted\n", __func__);
	return 0;
}
