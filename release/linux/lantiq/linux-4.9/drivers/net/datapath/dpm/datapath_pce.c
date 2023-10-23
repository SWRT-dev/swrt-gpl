// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 *
 *****************************************************************************/

#include <linux/list.h>
#include <net/datapath_api.h>
#include "datapath.h"
#include "datapath_pce.h"
#include <net/datapath_proc_api.h>	/* for proc api */

/* PCE Flow Engine Common Region start index */
#define PCE_TFCR_START_IDX	0x46E

/* PCE Flow Engine Common Region Entry Numbers */
#define PCE_TFCR_NUM		0x46F

/* PCE Flow Engine Common Region Entry Numbers */
#define PCE_MAX_ENTRIES		512

/* Invalid Sub-Block specified as 0,
 * Passing this value means Framework will create a sub-block and pass back
 * the ID
 */
#define DP_INVALID_SUB_BLK 0

/* Global PCE Rule struct */
struct dp_pce_rule pce_rule;

/* There are 2 purpose for this API
 * 1. checking whether continuous free from start index to size
 * 2. if not free, increment the start_index upto the next free element,
 *    it maynot be same as size
 */
static bool contiguous_free_idx(u32 *entries, int *start_idx, int size)
{
	u32 i;
	int idx = *start_idx;
	u32 not_continuous = 0;

	for (i = idx; i < (idx + size); i++) {
		if (entries[i]) {
			(*start_idx)++;
			not_continuous = 1;
		}
	}

	if (not_continuous)
		return false;
	return true;
}

/* Check whether entries from subblk_firstidx to subblk_size is free
 * subblk_firstidx - from 0 .. blk max
 * subblk_size - from 0 .. blk max
 */
static int check_subblk_entry_avail(struct dp_blk_entry *blk_entry,
				    struct dp_subblk_info *blk_info)
{
	struct dp_subblk_entry *subblk_entry = NULL;
	u32 *rules;
	int i = 0, start_free_idx = blk_info->subblk_firstidx;
	int start_idx = 0;

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: subblk_size: %d, subblk_firstidx: %d\n",
		 __func__, blk_info->subblk_size, blk_info->subblk_firstidx);

	if (blk_info->subblk_firstidx > blk_entry->blk_rules_max)
		return -1;

	if ((blk_info->subblk_firstidx + blk_info->subblk_size) >
	    blk_entry->blk_rules_max)
		return -1;

	rules = kzalloc((sizeof(u32) * blk_entry->blk_rules_max), GFP_ATOMIC);
	if (!rules)
		return -1;

	/* Mark the entries as used */
	list_for_each_entry(subblk_entry, &blk_entry->subblk_list, list) {
		start_idx = subblk_entry->subblk_firstindex -
			    blk_entry->blk_firstindex;
		for (i = start_idx;
		     i < (start_idx + subblk_entry->subblk_rules_max);
		     i++) {
			rules[i] = 1;
		}
	}

	if (!contiguous_free_idx(rules, &start_free_idx, blk_info->subblk_size)) {
		kfree(rules);
		return -1;
	}

	kfree(rules);

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: start_free_idx: %d\n",
		 __func__,
		 (blk_entry->blk_firstindex + blk_info->subblk_firstidx));

	return (blk_entry->blk_firstindex + blk_info->subblk_firstidx);
}

/* Return the first free continuous entry inside the block */
static int get_subblk_free_idx(struct dp_blk_entry *blk_entry, int subblk_size)
{
	struct dp_subblk_entry *subblk_entry = NULL;
	u32 *rules;
	int i = 0, start_free_idx = -1;
	int start_idx = 0, idx = 0;

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: subblk_size: %d\n",
		 __func__, subblk_size);

	rules = kzalloc((sizeof(u32) * blk_entry->blk_rules_max), GFP_ATOMIC);
	if (!rules)
		return -1;

	/* Mark the entries as used */
	list_for_each_entry(subblk_entry, &blk_entry->subblk_list, list) {
		start_idx = subblk_entry->subblk_firstindex - blk_entry->blk_firstindex;
		for (i = start_idx;
		     i < (start_idx + subblk_entry->subblk_rules_max);
		     i++) {
			rules[i] = 1;
		}
	}

	/* Find the first continuous free index */
	while ((idx + subblk_size) <= blk_entry->blk_rules_max) {
		if (!contiguous_free_idx(rules, &idx, subblk_size))
			continue;
		start_free_idx = idx;
		break;
	}

	kfree(rules);

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s %d: start_free_idx: %d\n",
		 __func__, __LINE__,
		 (blk_entry->blk_firstindex + start_free_idx));

	if (start_free_idx == -1)
		return -1;
	else
		return (blk_entry->blk_firstindex + start_free_idx);
}

/* For Global if Type matches get the entry
 * For CTP, based on portid and subif id get the block
 */
static struct dp_blk_entry *get_blk_entry(struct dp_subblk_info *blk_info,
					  GSW_PCE_RuleRegion_t region)
{
	struct dp_blk_entry *blk_entry = NULL;
	struct dp_subblk_info *c_blk_info;

	list_for_each_entry(blk_entry, &pce_rule.blk_list, list) {
		if (blk_entry->info.region != region)
			continue;

		if (region == GSW_PCE_RULE_CTP) {
			c_blk_info = &blk_entry->info.info;
			if ((c_blk_info->portid == blk_info->portid) &&
			    (c_blk_info->subif == blk_info->subif))
				return blk_entry;
		} else {
			return blk_entry;
		}
	}

	return NULL;
}

/* Get the sub-block based on the PCE Type ID */
static struct dp_subblk_entry *get_subblk_entry(struct dp_blk_entry *blk_entry,
						int subblk_id)
{
	struct dp_subblk_entry *subblk_entry = NULL;

	/* if sub-block id is <=0, need to create a new sub-block entry */
	if (subblk_id <= DP_INVALID_SUB_BLK)
		return NULL;

	list_for_each_entry(subblk_entry, &blk_entry->subblk_list, list) {
		if (subblk_entry->subblk_id != subblk_id)
			continue;
		return subblk_entry;
	}

	return NULL;
}

/* Add a block, If already existing return the entry
 * blk_info : Information passed by User
 * blk_size : Global Block size from GSWIP register,
 *            For CTP, block size is passed during alloc
 * Return   : Block Entry Allocated/Already present
 */
static struct dp_blk_entry *blk_add(struct dp_pce_blk_info *pce_blk_info,
				    int blk_size)
{
	GSW_PCE_rule_alloc_t alloc = {0};
	struct dp_blk_entry *blk_entry;
	struct core_ops *ops = gsw_get_swcore_ops(0);
	GSW_CTP_portConfig_t ctp = {0};
	struct dp_subblk_info *blk_info;

	if (!ops)
		return NULL;

	blk_info = &pce_blk_info->info;

	/* if already have return the entry */
	blk_entry = get_blk_entry(blk_info, pce_blk_info->region);
	if (blk_entry)
		return blk_entry;

	blk_entry = kzalloc(sizeof(*blk_entry), GFP_ATOMIC);
	if (!blk_entry)
		return NULL;

	/* Alloc is used only for CTP region not for Global Rule */
	alloc.num_of_rules = blk_size;

	if (pce_blk_info->region == GSW_PCE_RULE_CTP) {
		if (ops->gsw_tflow_ops.TFLOW_PceRuleAlloc(ops, &alloc)) {
			pr_err("%s: TFLOW_PceRuleAlloc failed\n", __func__);
			kfree(blk_entry);
			return NULL;
		}

		DP_DEBUG(DP_DBG_FLAG_PCE,
			 "%s: RuleAlloc success, rules: %u, blk_id: %u\n",
			 __func__, alloc.num_of_rules, alloc.blockid);

		ctp.nLogicalPortId = blk_info->portid;
		ctp.nSubIfIdGroup = blk_info->subif;
		ctp.nFirstFlowEntryIndex = alloc.blockid;
		ctp.eMask = GSW_CTP_PORT_CONFIG_FLOW_ENTRY;

		if (ops->gsw_ctp_ops.CTP_PortConfigSet(ops, &ctp)) {
			pr_err("%s: CTP_PortConfigSet failed\n", __func__);
			ops->gsw_tflow_ops.TFLOW_PceRuleFree(ops, &alloc);
			kfree(blk_entry);
			return NULL;
		}

		DP_DEBUG(DP_DBG_FLAG_PCE,
			 "%s: CTP_PortCfgSet success, Lpid: %u, EntryIdx: %u\n",
			 __func__, ctp.nLogicalPortId,
			 ctp.nFirstFlowEntryIndex);
	} else {
		alloc.blockid = pce_rule.glbl_firstindex;
		alloc.num_of_rules = pce_rule.glbl_rules_max;
	}

	blk_entry->info.region = pce_blk_info->region;
	memcpy(&blk_entry->info.info, blk_info, sizeof(*blk_info));

	blk_entry->blk_firstindex = alloc.blockid;
	blk_entry->blk_rules_max = alloc.num_of_rules;

	sprintf(blk_entry->blk_name, "blk-%d", blk_entry->blk_firstindex);

	INIT_LIST_HEAD(&blk_entry->subblk_list);
	list_add(&blk_entry->list, &pce_rule.blk_list);

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %s %d %s %d %s %s %s %d RuleMax %d RuleUsed %d\n",
		 __func__,
		 "Success",
		 "Blk Portid",
		 blk_entry->info.info.portid,
		 "Subif",
		 blk_entry->info.info.subif,
		 "Region",
		 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 "FirstIdx",
		 blk_entry->blk_firstindex,
		 blk_entry->blk_rules_max,
		 blk_entry->blk_rules_used);

	return blk_entry;
}

/* Reset CTP flow index before pce blk_del */
static int reset_ctp_pce_flow_index(struct dp_blk_entry *blk_entry)
{
	struct core_ops *ops = gsw_get_swcore_ops(0);
	GSW_CTP_portConfig_t ctp = {0};
	struct bp_pmapper *bp_info;
	struct ctp_dev *ctp_entry;

	if (!ops)
		return -1;

	ctp.nLogicalPortId = blk_entry->info.info.portid;
	ctp.nSubIfIdGroup = blk_entry->info.info.subif;
	ctp.eMask = GSW_CTP_PORT_CONFIG_MASK_BRIDGE_PORT_ID;
	if (ops->gsw_ctp_ops.CTP_PortConfigGet(ops, &ctp)) {
		pr_err("CTP(%d) CfgGet fail for ep=%d\n",
		       ctp.nSubIfIdGroup, ctp.nLogicalPortId);
		return -1;
	}
	bp_info = get_dp_bp_info(0, ctp.nBridgePortId);
	if ((!bp_info) || (!bp_info->ref_cnt)) {
		ctp.nFirstFlowEntryIndex = CTP_FLOW_ENTRY_IDX_RESET;
		ctp.eMask = GSW_CTP_PORT_CONFIG_FLOW_ENTRY;

		if (ops->gsw_ctp_ops.CTP_PortConfigSet(ops, &ctp)) {
			pr_err("%s: CfgSet failed, Lpid: %u, nFirstIdx: %u\n",
			       __func__, ctp.nLogicalPortId,
			       ctp.nFirstFlowEntryIndex);
			return -1;
		}
		DP_DEBUG(DP_DBG_FLAG_PCE,
			 "%s: CTPPortCfgSet Succ nSubifIdGroup:%u Bp:%u %s%u\n",
			 __func__, ctp.nSubIfIdGroup, ctp.nBridgePortId,
			 "Lpid:", ctp.nLogicalPortId);
		return 0;
	}
	/* Retrieve the CTP dev list for the pmapper bridge port */
	list_for_each_entry(ctp_entry, &bp_info->ctp_dev, list) {
		if (!ctp_entry->dev)
			continue;
		ctp.nSubIfIdGroup = ctp_entry->ctp;
		ctp.nFirstFlowEntryIndex = CTP_FLOW_ENTRY_IDX_RESET;
		ctp.eMask = GSW_CTP_PORT_CONFIG_FLOW_ENTRY;

		if (ops->gsw_ctp_ops.CTP_PortConfigSet(ops, &ctp)) {
			pr_err("%s: CfgSet failed, ctp: %u, nFirstIdx: %u\n",
			       __func__, ctp.nSubIfIdGroup,
			       ctp.nFirstFlowEntryIndex);
			return -1;
		}
		DP_DEBUG(DP_DBG_FLAG_PCE,
			 "%s: CTPPortCfgSet Succ nSubifIdGroup:%u BP:%u %s%u\n",
			 __func__, ctp.nSubIfIdGroup, ctp.nBridgePortId,
			 "Lpid:", ctp.nLogicalPortId);
	}
	return 0;
}

/* Delete a block
 * blk_entry : Block Entry Allocated
 * Return    : -1 on Failure, 0 on Success
 */
static int blk_del(struct dp_blk_entry *blk_entry)
{
	GSW_PCE_rule_alloc_t alloc = {0};
	struct core_ops *ops = gsw_get_swcore_ops(0);

	if (!ops)
		return -1;

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %d %s %d %s %s FirstIdx %d RuleMax %d RuleUsed %d\n",
		 __func__,
		 "Blk Portid",
		 blk_entry->info.info.portid,
		 "Subif",
		 blk_entry->info.info.subif,
		 "Region",
		 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 blk_entry->blk_firstindex,
		 blk_entry->blk_rules_max,
		 blk_entry->blk_rules_used);

	if (blk_entry->blk_rules_used) {
		DP_DEBUG(DP_DBG_FLAG_PCE,
			 "%s: Someone Still using this %s blk, Cannot del\n",
			 __func__,
			 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
			 "CTP" : "Global");
		return DP_SUCCESS;
	}

	if (blk_entry->info.region == GSW_PCE_RULE_CTP) {

		if (reset_ctp_pce_flow_index(blk_entry)) {
			pr_err("%s: reset CTP pce flow index failed\n",
			       __func__);
			return -1;
		}

		alloc.blockid = blk_entry->blk_firstindex;
		if (ops->gsw_tflow_ops.TFLOW_PceRuleFree(ops, &alloc)) {
			pr_err("%s: TFLOW_PceRuleFree failed, subblk_id: %u\n",
			       __func__, alloc.blockid);
			return -1;
		}
	}

	/* Free the block entry */
	list_del(&blk_entry->list);
	kfree(blk_entry);

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: Success\n", __func__);

	return DP_SUCCESS;
}

/* Add a Sub-Block, If already existing return the entry
 * blk_entry : Block Entry Allocated
 * info      : Block Information passed by User
 * Return    : Sub-Block Entry Allocated/Already present
 */
static struct dp_subblk_entry *subblk_add(struct dp_blk_entry *blk_entry,
					  struct dp_subblk_info *blk_info)
{
	struct dp_subblk_entry *subblk_entry;
	int rules_left = 0, firstindex = 0;
# if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
	struct core_ops *ops = gsw_get_swcore_ops(0);
	GSW_PCE_GlobalBitMap_t gbitmap = {0};
#endif

	/* if already have return the entry */
	subblk_entry = get_subblk_entry(blk_entry, blk_info->subblk_id);
	if (subblk_entry)
		return subblk_entry;

	/* Give a valid sub-block size, if not given will assume as 1 */
	if (blk_info->subblk_size <= 0) {
		DP_DEBUG(DP_DBG_FLAG_PCE,
			 "%s: Give a valid Sub-Block Size %d, assuming as 1\n",
			 __func__, blk_info->subblk_size);
		blk_info->subblk_size = 1;
	}

	rules_left = blk_entry->blk_rules_max - blk_entry->blk_rules_used;

	if (blk_info->subblk_size > rules_left) {
		pr_err("%s: No Space to add sub-block Already %d/%d used\n",
		       __func__, blk_entry->blk_rules_used,
		       blk_entry->blk_rules_max);
		return NULL;
	}

	/* Allocate and block num of rules in Global entries
	 * Sub-block Free index will be found out by GSWIP API entry
	 */
	if (blk_entry->info.region == GSW_PCE_RULE_COMMMON) {
	# if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
		gbitmap.num_of_rules = blk_info->subblk_size;
		if (ops->gsw_tflow_ops.TFLOW_GlobalRule_BitMapAlloc(ops, &gbitmap)) {
			pr_err("%s: For Global rules cont %d space not avail\n",
			       __func__, blk_info->subblk_size);
			return NULL;
		}
		firstindex = gbitmap.base_index;
	#endif
	} else {
		/* if subblk_firstidx > 0, Use this as first index to allocate
		 * else, find a free index
		 */
		if (blk_info->subblk_firstidx > 0) {
			firstindex = check_subblk_entry_avail(blk_entry, blk_info);
			if (firstindex == -1) {
				pr_err("%s: Sub-Blk specified FirstIdx %d not avail\n",
				       __func__, blk_info->subblk_firstidx);
				return NULL;
			}
		} else {
			firstindex = get_subblk_free_idx(blk_entry, blk_info->subblk_size);
			if (firstindex == -1) {
				pr_err("%s: No continuous Space Avail %d/%d used\n",
				       __func__,
				       blk_entry->blk_rules_used,
				       blk_entry->blk_rules_max);
				return NULL;
			}
		}
	}

	subblk_entry = kzalloc(sizeof(*subblk_entry), GFP_ATOMIC);
	if (!subblk_entry)
		return NULL;

	subblk_entry->subblk_firstindex = firstindex;
	subblk_entry->subblk_rules_max = blk_info->subblk_size;

	/* While adding create a sub-blk id and return back in same struct
	 * subblk first idx will be a unique number always use that as ID
	 */
	blk_info->subblk_id = subblk_entry->subblk_firstindex + 1;
	blk_info->subblk_firstidx = subblk_entry->subblk_firstindex;

	subblk_entry->subblk_id = blk_info->subblk_id;

	if (!strlen(blk_info->subblk_name)) {
		sprintf(subblk_entry->subblk_name, "subblk-%d",
			subblk_entry->subblk_id);
	} else {
		strncpy(subblk_entry->subblk_name, blk_info->subblk_name,
			sizeof(subblk_entry->subblk_name) - 1);
	}

	blk_entry->blk_rules_used += blk_info->subblk_size;

	/* User can explicity change the protection mode during delete
	 * if 1 - this sub-blk won't be removed even after all rules deleted
	 * if 0 - this sub-blk will be removed after all rules removed
	 */
	subblk_entry->subblk_protected = blk_info->subblk_protected;

	list_add(&subblk_entry->list, &blk_entry->subblk_list);

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s SubBlk FirstIdx %d RuleMax %d RuleUsed %d\n",
		 __func__,
		 "Success",
		 subblk_entry->subblk_firstindex,
		 subblk_entry->subblk_rules_max,
		 subblk_entry->subblk_rules_used);
	return subblk_entry;
}

/* Delete a sub-block
 * blk_entry    : Block Entry Allocated
 * subblk_entry : Sub-Block Entry Allocated
 * Return       : -1 on Failure, 0 on Success
 */
static int subblk_del(struct dp_blk_entry *blk_entry,
		      struct dp_subblk_entry *subblk_entry)
{
# if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
	struct core_ops *ops = gsw_get_swcore_ops(0);
	GSW_PCE_GlobalBitMap_t gbitmap = {0};
#endif

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %d %s %s %s %d %s %d %s %d %s %d\n",
		 __func__,
		 "SubBlkId", subblk_entry->subblk_id,
		 "Name", subblk_entry->subblk_name,
		 "FirstIdx", subblk_entry->subblk_firstindex,
		 "RuleMax", subblk_entry->subblk_rules_max,
		 "RuleUsed", subblk_entry->subblk_rules_used,
		 "protected", subblk_entry->subblk_protected);

	/* Delete the sub-block only when all rules are removed and sub-block is
	 * not protected
	 */
	if (!subblk_entry->subblk_rules_used &&
	    !subblk_entry->subblk_protected) {
	# if IS_ENABLED(CONFIG_INTEL_DATAPATH_HAL_GSWIP31)
		if (blk_entry->info.region == GSW_PCE_RULE_COMMMON) {
			/* Mark all the subblk rules as free free in GSWIP API */
			gbitmap.num_of_rules = subblk_entry->subblk_rules_max;
			gbitmap.base_index = subblk_entry->subblk_firstindex;
			if (ops->gsw_tflow_ops.TFLOW_GlobalRule_BitMapFree(ops, &gbitmap)) {
				pr_err("%s: TFLOW_GlobalRule_BitMapFree failed\n", __func__);
				return -1;
			}
		}
	#endif
		blk_entry->blk_rules_used -= subblk_entry->subblk_rules_max;
		list_del(&subblk_entry->list);
		kfree(subblk_entry);
	}

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: Success\n", __func__);

	return DP_SUCCESS;
}

/* Pce Rule Operations
 * pce_rule     : Pce rule Entry to del
 * subblk_entry : Sub-Block on which rule need to be deleted
 */
static int pce_rule_del(GSW_PCE_ruleDelete_t *pce,
			struct dp_blk_entry *blk_entry,
			struct dp_subblk_entry *subblk_entry)
{
	struct core_ops *ops = gsw_get_swcore_ops(0);
	int index = pce->nIndex;

	if (!ops)
		return -1;

	if (pce->nIndex > subblk_entry->subblk_rules_max) {
		pr_err("%s: PCE Index for Block %d should be within 0 to %d, nIndex: %u\n",
		       __func__, subblk_entry->subblk_id,
		       subblk_entry->subblk_rules_max,
		       pce->nIndex);
		return -1;
	}

	if (!subblk_entry->subblk_rules_used) {
		DP_DEBUG(DP_DBG_FLAG_PCE,
			 "%s: Entries in sub-blk freed, subblk protect %d\n",
			 __func__, subblk_entry->subblk_protected);
		return DP_SUCCESS;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s %d %s %d %s %d\n",
		 __func__,
		 "subblk_firstindex", subblk_entry->subblk_firstindex,
		 "blk_firstindex", blk_entry->blk_firstindex,
		 "nIndex", pce->nIndex);

	pce->nIndex =
		(subblk_entry->subblk_firstindex - blk_entry->blk_firstindex) +
		pce->nIndex;

	if (ops->gsw_tflow_ops.TFLOW_PceRuleDelete(ops, pce)) {
		pr_err("%s: TFLOW_PceRule Delete failed, nIndex: %u\n",
		       __func__, pce->nIndex);
		return -1;
	}

	/* update sub-block bitmask control */
	clear_bit(index, subblk_entry->_used);

	if (subblk_entry->subblk_rules_used)
		subblk_entry->subblk_rules_used--;

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: Success Pce Rule Del from %s %d %s %d\n",
		 __func__,
		 "HW Idx", (blk_entry->blk_firstindex + pce->nIndex),
		 "Sub-Block rules used", subblk_entry->subblk_rules_used);

	return DP_SUCCESS;
}

static int pce_rule_add_index_get(struct dp_subblk_entry *subblk_entry,
				  struct dp_pce_user_data *user_data)
{
	int i;
	u32 start = 0;
	u32 end = subblk_entry->subblk_rules_max;

	if (user_data && user_data->idx_en) {
		DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s %d %s %u %s %u\n",
			 __func__,
			 "user_idx_en", user_data->idx_en,
			 "user_idx_s", user_data->idx_s,
			 "user_idx_e", user_data->idx_e);

		start = user_data->idx_s;
		end = user_data->idx_e;

		if (start >= end || end > subblk_entry->subblk_rules_max) {
			pr_err("%s: Sub-block range incorrect %s %u %s %u %s %u\n",
			       __func__,
			       "start", user_data->idx_s,
			       "end", user_data->idx_e,
			       "max", subblk_entry->subblk_rules_max);
			return DP_FAILURE;
		}
	}

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s %u %s %u\n",
		 __func__,
		 "start", start,
		 "end", end);

	for (i = start; i < end; i++) {
		if (test_bit(i, subblk_entry->_used))
			continue;
		return i;
	}

	return DP_FAILURE;
}

/* Pce Rule Operations
 * pce_rule     : Pce rule Entry to add
 * subblk_entry : Sub-Block on which rule need to be added
 * ret : Sub-Block relative index on which rule was added
 */
static int pce_rule_add(GSW_PCE_rule_t *pce,
			struct dp_blk_entry *blk_entry,
			struct dp_subblk_entry *subblk_entry,
			struct dp_pce_user_data *user_data)
{
	struct core_ops *ops = gsw_get_swcore_ops(0);
	int index;

	if (!ops)
		return -1;

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s %d %s %d %s %d %s %d\n",
		 __func__,
		 "subblk_firstindex", subblk_entry->subblk_firstindex,
		 "blk_firstindex", blk_entry->blk_firstindex,
		 "subblk_rules_used", subblk_entry->subblk_rules_used,
		 "subblk_rules_max", subblk_entry->subblk_rules_max);

	pce->pattern.bEnable = 1;
	pce->pattern.nIndex =
		subblk_entry->subblk_firstindex - blk_entry->blk_firstindex;

	/* prevent bitmask overflow */
	if (subblk_entry->subblk_rules_max > PCE_SUBBLK_SIZE_MAX) {
		pr_err("%s: Sub-block max rules(%u) > max default(%d)\n",
		       __func__, subblk_entry->subblk_rules_max,
		       PCE_SUBBLK_SIZE_MAX);
		return -1;
	}

	index = pce_rule_add_index_get(subblk_entry, user_data);
	if (index < 0) {
		pr_err("%s: Sub-block index not available!\n",
		       __func__);
		return -1;
	}

	pce->pattern.nIndex += index;
	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: subblk_index %d, pce_index %d\n",
		 __func__, index, pce->pattern.nIndex);

	if (ops->gsw_tflow_ops.TFLOW_PceRuleWrite(ops, pce)) {
		pr_err("%s: TFLOW_PceRuleWite failed, nIndex: %u\n",
		       __func__, pce->pattern.nIndex);
		return -1;
	}

	/* update sub-block bitmask control */
	set_bit(index, subblk_entry->_used);

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: Success Pce Rule Added to HW Idx %d\n",
		 __func__, pce->pattern.nIndex);

	/* save sub-block relative index for user inside PCE structure */
	pce->pattern.nIndex = index;

	return index;
}

/* Pce Rule Operations
 * pce_rule     : Pce rule Entry to Read
 * subblk_entry : Sub-Block on which rule need to be read
 */
static int pce_rule_get(GSW_PCE_rule_t *pce,
			struct dp_blk_entry *blk_entry,
			struct dp_subblk_entry *subblk_entry)
{
	struct core_ops *ops = gsw_get_swcore_ops(0);

	if (!ops)
		return -1;

	if (pce->pattern.nIndex > subblk_entry->subblk_rules_max) {
		pr_err("%s: PCE Index for Block %d should be within 0 to %d, nIndex: %u\n",
		       __func__, subblk_entry->subblk_id,
		       subblk_entry->subblk_rules_max,
		       pce->pattern.nIndex);
		return -1;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s %d %s %d %s %d\n",
		 __func__,
		 "subblk_firstindex", subblk_entry->subblk_firstindex,
		 "blk_firstindex", blk_entry->blk_firstindex,
		 "nIndex", pce->pattern.nIndex);

	pce->pattern.nIndex =
		(subblk_entry->subblk_firstindex - blk_entry->blk_firstindex) +
		pce->pattern.nIndex;

	if (ops->gsw_tflow_ops.TFLOW_PceRuleRead(ops, pce)) {
		pr_err("%s: TFLOW_PceRuleRead failed, nIndex: %u\n",
		       __func__, pce->pattern.nIndex);
		return -1;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: Success Pce Rule Get from HW Idx %d\n",
		 __func__, pce->pattern.nIndex);

	return DP_SUCCESS;
}

/* Pce Rule update Operations
 * pce_rule     : Pce rule Entry to update
 * subblk_entry : Sub-Block on which rule need to be read
 * update       : Update operation Enable/Disable or Rewrite Rule
 */
static int pce_rule_update(GSW_PCE_rule_t *pce,
			   struct dp_blk_entry *blk_entry,
			   struct dp_subblk_entry *subblk_entry,
			   enum DP_PCE_RULE_UPDATE update)
{
	struct core_ops *ops = gsw_get_swcore_ops(0);

	if (!ops)
		return -1;

	if (update <= DP_PCE_RULE_NOCHANGE) {
		pr_err("%s: update value is no change or wrong %d\n",
		       __func__, update);
		return -1;
	}

	if (pce->pattern.nIndex > subblk_entry->subblk_rules_max) {
		pr_err("%s: PCE Index for Block %d should be within 0 to %d, nIndex: %u\n",
		       __func__, subblk_entry->subblk_id,
		       subblk_entry->subblk_rules_max,
		       pce->pattern.nIndex);
		return -1;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s %d %s %d %s %d %s %d\n",
		 __func__,
		 "subblk_firstindex", subblk_entry->subblk_firstindex,
		 "blk_firstindex", blk_entry->blk_firstindex,
		 "nIndex", pce->pattern.nIndex,
		 "Update", update);

	pce->pattern.nIndex =
		(subblk_entry->subblk_firstindex - blk_entry->blk_firstindex) +
		pce->pattern.nIndex;

	switch (update) {
	case DP_PCE_RULE_EN:
		if (ops->gsw_tflow_ops.TFLOW_PceRuleEnable(ops, pce)) {
			pr_err("%s: TFLOW_PceRuleEnable failed, nIndex: %u\n",
			       __func__, pce->pattern.nIndex);
			return -1;
		}
		break;
	case DP_PCE_RULE_DIS:
		if (ops->gsw_tflow_ops.TFLOW_PceRuleDisable(ops, pce)) {
			pr_err("%s: TFLOW_PceRuleDisable failed, nIndex: %u\n",
			       __func__, pce->pattern.nIndex);
			return -1;
		}
		break;
	case DP_PCE_RULE_REWRITE:
		if (ops->gsw_tflow_ops.TFLOW_PceRuleWrite(ops, pce)) {
			pr_err("%s: TFLOW_PceRuleWite failed, nIndex: %u\n",
			       __func__, pce->pattern.nIndex);
			return -1;
		}
		break;
	default:
		break;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: Success Pce Rule update for HW Idx %d\n",
		 __func__, pce->pattern.nIndex);

	return DP_SUCCESS;
}

/* API will create Block and Sub-Block
 * Pass proper portid, subif, subblock id, subblock size, region
 * return -1 fail, 0 - Success
 */
int dp_pce_blk_create(struct dp_pce_blk_info *pce_blk_info)
{
	struct pmac_port_info *port_info;
	struct dp_blk_entry *blk_entry;
	struct dp_subblk_entry *subblk_entry;
	struct dp_subblk_info *blk_info;
	int blk_size = 0, ret = DP_FAILURE;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if (!pce_blk_info) {
		pr_err("%s: blk_info %px Null\n", __func__, pce_blk_info);
		return DP_FAILURE;
	}

	blk_info = &pce_blk_info->info;
	port_info = get_dp_port_info(0, blk_info->portid);

	if (pce_blk_info->region == GSW_PCE_RULE_CTP) {
		blk_size = port_info->blk_size;
		if (!blk_size)
			blk_size = PCE_MAX_BLK_SIZE_CTP;
	} else {
		blk_size = pce_rule.glbl_rules_max;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: Block Size %d Region %d Portid %d Subif %d\n",
		 __func__, blk_size, pce_blk_info->region, blk_info->portid,
		 blk_info->subif);

	DP_LIB_LOCK(&dp_lock);
	blk_entry = blk_add(pce_blk_info, blk_size);
	if (!blk_entry) {
		pr_err("%s: No Block present or created\n", __func__);
		goto EXIT;
	}

	/* Check whether sub-block is created, if not create */
	subblk_entry = subblk_add(blk_entry, blk_info);
	if (!subblk_entry) {
		pr_err("%s: No Sub-Block present or created\n", __func__);
		blk_del(blk_entry);
		goto EXIT;
	}

	ret = DP_SUCCESS;

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %s %s %d %s %d %s %d %s %s %d %s %d %s %d\n",
		 __func__,
		 "Success",
		 "Blk",
		 "FirstIndex", blk_entry->blk_firstindex,
		 "RuleMax", blk_entry->blk_rules_max,
		 "RuleUsed", blk_entry->blk_rules_used,
		 "Sub-Block",
		 "FirstIndex", subblk_entry->subblk_firstindex,
		 "RuleMax", subblk_entry->subblk_rules_max,
		 "RuleUsed", subblk_entry->subblk_rules_used);
EXIT:
	DP_LIB_UNLOCK(&dp_lock);
	return ret;
}
EXPORT_SYMBOL(dp_pce_blk_create);

/* API will delete All the pce entries, sub-block and blk
 * Pass proper portid, subif, subblock id, region and protected or not
 * return -1 fail, 0 - Success
 */
int dp_pce_blk_del(struct dp_pce_blk_info *pce_blk_info)
{
	struct dp_blk_entry *blk_entry;
	struct dp_subblk_entry *subblk_entry;
	struct dp_subblk_info *blk_info;
	int ret = DP_FAILURE, i, rules_used;
	GSW_PCE_ruleDelete_t pce;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}
	if (!pce_blk_info) {
		pr_err("%s: blk_info %px Null\n", __func__, pce_blk_info);
		return DP_FAILURE;
	}

	blk_info = &pce_blk_info->info;

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %s %s %d %s %d %s %d %s %d %s %d\n",
		 __func__,
		 "Region", (pce_blk_info->region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 "Portid", blk_info->portid,
		 "Subif", blk_info->subif,
		 "subblk_size", blk_info->subblk_size,
		 "Protected", blk_info->subblk_protected,
		 "FirstIndex", blk_info->subblk_firstidx);

	DP_LIB_LOCK(&dp_lock);
	blk_entry = get_blk_entry(blk_info, pce_blk_info->region);
	if (!blk_entry) {
		pr_err("%s: Cannot find the blk_entry for portid=%d subif=%d\n",
		       __func__, blk_info->portid, blk_info->subif);
		goto EXIT;
	}

	subblk_entry = get_subblk_entry(blk_entry, blk_info->subblk_id);
	if (!subblk_entry) {
		pr_err("%s: Cannot find the subblk_entry for subblk_id=%d\n",
		       __func__, blk_info->subblk_id);
		goto EXIT;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: subblk_entry->subblk_rules_used %d\n", __func__,
		 subblk_entry->subblk_rules_used);

	pce.logicalportid = blk_info->portid;
	pce.subifidgroup = blk_info->subif;
	pce.region = pce_blk_info->region;

	rules_used = subblk_entry->subblk_rules_used;
	for (i = 0; i < rules_used; i++) {
		pce.nIndex = i;
		if (pce_rule_del(&pce, blk_entry, subblk_entry)) {
			pr_err("%s: pce_rule_del %d failed\n", __func__,
			       pce.nIndex);
			goto EXIT;
		}
	}

	/* User can explicity change the protection mode during delete
	 * if 1 - this sub-blk wont be removed even after all rules deleted
	 * if 0 - this sub-blk will be removed after all rules removed
	 */
	subblk_entry->subblk_protected = blk_info->subblk_protected;
	subblk_del(blk_entry, subblk_entry);

	if (blk_del(blk_entry)) {
		pr_err("%s: blk_del failed\n", __func__);
		goto EXIT;
	}

	ret = DP_SUCCESS;
EXIT:
	DP_LIB_UNLOCK(&dp_lock);
	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s\n", __func__,
		 (ret == DP_SUCCESS) ? "Success" : "Fail");
	return ret;
}
EXPORT_SYMBOL(dp_pce_blk_del);

/* PCE rule add API to call within DPM without lock */
int dp_pce_rule_add_priv(struct dp_pce_blk_info *pce_blk_info,
			 GSW_PCE_rule_t *pce)
{
	struct dp_subblk_entry *subblk_entry;
	struct core_ops *ops;
	struct dp_subblk_info *blk_info;
	struct pmac_port_info *port_info;
	struct dp_blk_entry *blk_entry;
	int blk_size = 0, ret = DP_FAILURE;

	if (!pce || !pce_blk_info) {
		pr_err("%s: pce_rule %px pce_blk_info %px Failed\n",
		       __func__, pce, pce_blk_info);
		return ret;
	}
	blk_info = &pce_blk_info->info;

	ops = gsw_get_swcore_ops(0);
	if (!ops)
		return ret;

	if (!blk_info->portid)
		blk_info->portid = pce->logicalportid;

	if (!blk_info->subif)
		blk_info->subif = pce->subifidgroup;

	if (!pce_blk_info->region)
		pce_blk_info->region = pce->region;

	/* Check whether block is created by someone else, if not create
	 * Block is only for CTP not Global
	 */
	port_info = get_dp_port_info(0, blk_info->portid);

	if (pce_blk_info->region == GSW_PCE_RULE_CTP) {
		blk_size = port_info->blk_size;
		if (!blk_size)
			blk_size = PCE_MAX_BLK_SIZE_CTP;
	} else {
		blk_size = pce_rule.glbl_rules_max;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %d %s %s %s %d %s %d %s %d %s %d %s %d\n",
		 __func__,
		 "block Size", blk_size,
		 "Region", (pce_blk_info->region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 "Portid", blk_info->portid,
		 "Subif", blk_info->subif,
		 "subblk_size", blk_info->subblk_size,
		 "Protected", blk_info->subblk_protected,
		 "FirstIndex", blk_info->subblk_firstidx);

	blk_entry = blk_add(pce_blk_info, blk_size);
	if (!blk_entry) {
		pr_err("%s: No Block present or created\n", __func__);
		goto EXIT;
	}

	/* Check whether sub-block is created, if not create */
	subblk_entry = subblk_add(blk_entry, blk_info);
	if (!subblk_entry) {
		pr_err("%s: No Sub-Block present or created\n", __func__);
		blk_del(blk_entry);
		goto EXIT;
	}

	if (subblk_entry->subblk_rules_max ==
	    subblk_entry->subblk_rules_used) {
		pr_err("%s: Sub-block already Full %d/%d\n", __func__,
		       subblk_entry->subblk_rules_used,
		       subblk_entry->subblk_rules_max);
		goto EXIT;
	}

	/* Add PCE rule inside sub-block entry */
	ret = pce_rule_add(pce, blk_entry, subblk_entry, blk_info->user_data);
	if (ret < 0) {
		pr_err("%s: pce_rule_add failed\n", __func__);
		subblk_del(blk_entry, subblk_entry);
		blk_del(blk_entry);
		goto EXIT;
	}

	subblk_entry->subblk_rules_used++;

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %s %s %d %s %d %s %d %s %d %s %s %d %s %d %s %d\n",
		 __func__,
		 (ret == DP_FAILURE) ? "fail" : "Success",
		 "Blk",
		 "FirstIndex", blk_entry->blk_firstindex,
		 "RuleMax", blk_entry->blk_rules_max,
		 "RuleUsed", blk_entry->blk_rules_used,
		 "PceIdx", ret,
		 "Sub-Block",
		 "FirstIndex", subblk_entry->subblk_firstindex,
		 "RuleMax", subblk_entry->subblk_rules_max,
		 "RuleUsed", subblk_entry->subblk_rules_used);
EXIT:
	return ret;
}

/* Add the PCE rule inside Sub-Block
 * API will create Block, Sub-Block and Store PCE rule in HW
 * Pass proper portid, subif, subblock id, subblock size, region
 * Returns back the PCE Index where rule is added
 */
int dp_pce_rule_add(struct dp_pce_blk_info *pce_blk_info,
		    GSW_PCE_rule_t *pce)
{
	int ret = DP_FAILURE;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);
	ret = dp_pce_rule_add_priv(pce_blk_info, pce);
	DP_LIB_UNLOCK(&dp_lock);

	return ret;
}
EXPORT_SYMBOL(dp_pce_rule_add);

/* PCE rule delete API to call within DPM without lock */
int dp_pce_rule_del_priv(struct dp_pce_blk_info *pce_blk_info,
			 GSW_PCE_ruleDelete_t *pce)
{
	struct dp_blk_entry *blk_entry;
	struct dp_subblk_entry *subblk_entry;
	struct dp_subblk_info *blk_info;
	int ret = DP_FAILURE;

	if (!pce || !pce_blk_info) {
		pr_err("%s: pce_rule %px pce_blk_info %px Failed\n",
		       __func__, pce, pce_blk_info);
		return ret;
	}
	blk_info = &pce_blk_info->info;

	if (pce->nIndex > pce_rule.hw_rules_max) {
		pr_err("%s: PCE Index %d is Invalid\n", __func__,
		       pce->nIndex);
		return ret;
	}

	if (!blk_info->portid)
		blk_info->portid = pce->logicalportid;

	if (!blk_info->subif)
		blk_info->subif = pce->subifidgroup;

	if (!pce_blk_info->region)
		pce_blk_info->region = pce->region;

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: Region %d Portid %d Subif %d Block ID %d, protected %d\n",
		 __func__, pce_blk_info->region, blk_info->portid,
		 blk_info->subif, blk_info->subblk_id,
		 blk_info->subblk_protected);

	blk_entry = get_blk_entry(blk_info, pce_blk_info->region);
	if (!blk_entry) {
		pr_err("%s: Cannot find the blk_entry for portid=%d subif=%d\n",
		       __func__, blk_info->portid, blk_info->subif);
		goto EXIT;
	}

	subblk_entry = get_subblk_entry(blk_entry, blk_info->subblk_id);
	if (!subblk_entry) {
		pr_err("%s: Cannot find the subblk_entry for subblk_id=%d\n",
		       __func__, blk_info->subblk_id);
		goto EXIT;
	}

	if (pce_rule_del(pce, blk_entry, subblk_entry)) {
		pr_err("%s: pce_rule_del failed\n", __func__);
		goto EXIT;
	}

	/* User can explicity change the protection mode during delete
	 * if 1 - this sub-blk wont be removed even after all rules deleted
	 * if 0 - this sub-blk will be removed after all rules removed
	 */
	subblk_entry->subblk_protected = blk_info->subblk_protected;
	subblk_del(blk_entry, subblk_entry);

	if (blk_del(blk_entry)) {
		pr_err("%s: blk_del failed\n", __func__);
		goto EXIT;
	}

	ret = DP_SUCCESS;
EXIT:
	DP_DEBUG(DP_DBG_FLAG_PCE, "%s: %s\n", __func__,
		 (ret == DP_SUCCESS) ? "Success" : "Fail");
	return ret;
}

/* Delete the PCE rule inside Sub-Block
 * Pass Subblk_Id,
 * Pass portid and subifid if it is CTP
 * Pass Index to delete in sub-block specify in pce->pattern.nIndex
 */
int dp_pce_rule_del(struct dp_pce_blk_info *pce_blk_info,
		    GSW_PCE_ruleDelete_t *pce)
{
	int ret = DP_FAILURE;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);
	ret = dp_pce_rule_del_priv(pce_blk_info, pce);
	DP_LIB_UNLOCK(&dp_lock);

	return ret;
}
EXPORT_SYMBOL(dp_pce_rule_del);

/* PCE rule get API to call within DPM without lock */
int dp_pce_rule_get_priv(struct dp_pce_blk_info *pce_blk_info,
			 GSW_PCE_rule_t *pce)
{
	struct dp_blk_entry *blk_entry;
	struct dp_subblk_entry *subblk_entry;
	struct dp_subblk_info *blk_info;
	int ret = DP_FAILURE;

	if (!pce || !pce_blk_info) {
		pr_err("%s: Pce Rule Info to delete is Null\n", __func__);
		return ret;
	}
	blk_info = &pce_blk_info->info;

	if (pce->pattern.nIndex > pce_rule.hw_rules_max) {
		pr_err("%s: PCE Index %d is Invalid\n", __func__,
		       pce->pattern.nIndex);
		return ret;
	}

	if (!blk_info->portid)
		blk_info->portid = pce->logicalportid;

	if (!blk_info->subif)
		blk_info->subif = pce->subifidgroup;

	if (!pce_blk_info->region)
		pce_blk_info->region = pce->region;

	blk_entry = get_blk_entry(blk_info, pce_blk_info->region);
	if (!blk_entry) {
		pr_err("%s: Cannot find the blk_entry for portid=%d subif=%d\n",
		       __func__, blk_info->portid, blk_info->subif);
		goto EXIT;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %d %s %s %s %d %s %d %s %d\n",
		 __func__,
		 "Pce Idx", pce->pattern.nIndex,
		 "Region",
		 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 "Portid", blk_entry->info.info.portid,
		 "Subifid", blk_entry->info.info.subif,
		 "Block ID", blk_info->subblk_id);

	subblk_entry = get_subblk_entry(blk_entry, blk_info->subblk_id);
	if (!subblk_entry) {
		pr_err("%s: Cannot find the subblk_entry for subblk_id=%d\n",
		       __func__, blk_info->subblk_id);
		goto EXIT;
	}

	if (pce_rule_get(pce, blk_entry, subblk_entry)) {
		pr_err("%s: pce_rule_get failed\n", __func__);
		goto EXIT;
	}

	ret = DP_SUCCESS;
	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %s %d %s %s %s %d %s %d %s %s %d %s %d %s %d\n",
		 __func__,
		 (ret == DP_SUCCESS) ? "Success" : "Fail",
		 "Pce Idx", pce->pattern.nIndex,
		 "Region",
		 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 "Portid", blk_entry->info.info.portid,
		 "Subifid", blk_entry->info.info.subif,
		 "Sub-Blk",
		 "FirstIdx", subblk_entry->subblk_firstindex,
		 "RuleMax", subblk_entry->subblk_rules_max,
		 "RuleUsed", subblk_entry->subblk_rules_used);
EXIT:
	return ret;
}

/* Get the PCE rule inside Sub-Block
 * Pass Subblk_Id,
 * Pass portid and subifid if it is CTP
 * Pass Index to get in sub-block specify in pce->pattern.nIndex
 */
int dp_pce_rule_get(struct dp_pce_blk_info *pce_blk_info,
		    GSW_PCE_rule_t *pce)
{
	int ret = DP_FAILURE;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);
	ret = dp_pce_rule_get_priv(pce_blk_info, pce);
	DP_LIB_UNLOCK(&dp_lock);

	return ret;
}
EXPORT_SYMBOL(dp_pce_rule_get);

/* PCE rule update API to call within DPM without lock */
int dp_pce_rule_update_priv(struct dp_pce_blk_info *pce_blk_info,
			    GSW_PCE_rule_t *pce, enum DP_PCE_RULE_UPDATE update)
{
	struct dp_blk_entry *blk_entry;
	struct dp_subblk_entry *subblk_entry;
	struct dp_subblk_info *blk_info;
	int ret = DP_FAILURE;

	if (!pce || !pce_blk_info) {
		pr_err("%s: Pce Rule Info to delete is Null\n", __func__);
		return ret;
	}
	blk_info = &pce_blk_info->info;

	if (pce->pattern.nIndex > pce_rule.hw_rules_max) {
		pr_err("%s: PCE Index %d is Invalid\n", __func__,
		       pce->pattern.nIndex);
		return ret;
	}

	if (!blk_info->portid)
		blk_info->portid = pce->logicalportid;

	if (!blk_info->subif)
		blk_info->subif = pce->subifidgroup;

	if (!pce_blk_info->region)
		pce_blk_info->region = pce->region;

	blk_entry = get_blk_entry(blk_info, pce_blk_info->region);
	if (!blk_entry) {
		pr_err("%s: Cannot find the blk_entry for portid=%d subif=%d\n",
		       __func__, blk_info->portid, blk_info->subif);
		goto EXIT;
	}

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %d %s %s %s %d %s %d %s %d\n",
		 __func__,
		 "Pce Idx", pce->pattern.nIndex,
		 "Region",
		 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 "Portid", blk_entry->info.info.portid,
		 "Subifid", blk_entry->info.info.subif,
		 "Block ID", blk_info->subblk_id);

	subblk_entry = get_subblk_entry(blk_entry, blk_info->subblk_id);
	if (!subblk_entry) {
		pr_err("%s: Cannot find the subblk_entry for subblk_id=%d\n",
		       __func__, blk_info->subblk_id);
		goto EXIT;
	}

	if (pce_rule_update(pce, blk_entry, subblk_entry, update)) {
		pr_err("%s: pce_rule_get failed\n", __func__);
		goto EXIT;
	}

	ret = DP_SUCCESS;
	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %s %d %s %s %s %d %s %d %s %s %d %s %d %s %d\n",
		 __func__,
		 (ret == DP_SUCCESS) ? "Success" : "Fail",
		 "Pce Idx", pce->pattern.nIndex,
		 "Region",
		 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
		 "CTP" : "GLOBAL",
		 "Portid", blk_entry->info.info.portid,
		 "Subifid", blk_entry->info.info.subif,
		 "Sub-Blk",
		 "FirstIdx", subblk_entry->subblk_firstindex,
		 "RuleMax", subblk_entry->subblk_rules_max,
		 "RuleUsed", subblk_entry->subblk_rules_used);
EXIT:
	return ret;
}

/* Get the PCE rule inside Sub-Block
 * Pass Subblk_Id,
 * Pass portid and subifid if it is CTP
 * Pass Index to get in sub-block specify in pce->pattern.nIndex
 */
int dp_pce_rule_update(struct dp_pce_blk_info *pce_blk_info,
		       GSW_PCE_rule_t *pce, enum DP_PCE_RULE_UPDATE update)
{
	int ret = DP_FAILURE;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);
	ret = dp_pce_rule_update_priv(pce_blk_info, pce, update);
	DP_LIB_UNLOCK(&dp_lock);

	return ret;
}
EXPORT_SYMBOL(dp_pce_rule_update);

/* Initialize the PCE rules table
 * Get the Global Rules firstIndex and Global Rules Max from GSWIP
 */
int dp_init_pce(void)
{
	struct core_ops *ops;
	GSW_register_t reg;

	ops = gsw_get_swcore_ops(0);
	if (!ops)
		return -1;

	reg.nRegAddr = PCE_TFCR_NUM;
	ops->gsw_common_ops.RegisterGet(ops, &reg);
	pce_rule.glbl_rules_max = reg.nData * 4;

	reg.nRegAddr = PCE_TFCR_START_IDX;
	ops->gsw_common_ops.RegisterGet(ops, &reg);
	pce_rule.glbl_firstindex = reg.nData;

	pce_rule.hw_rules_max = PCE_MAX_ENTRIES;

	INIT_LIST_HEAD(&pce_rule.blk_list);

	return 0;
}

int proc_pce_dump(struct seq_file *s, int pos)
{
	struct dp_blk_entry *blk_entry = NULL;
	struct dp_subblk_entry *subblk_entry = NULL;
	int subblk = 0, i = 0, blk = 0;

	seq_puts(s, "\n");
	seq_printf(s, "%-10s%-10s%-10d\n", "PCE-Rule",
		   "HW Max", pce_rule.hw_rules_max);
	seq_puts(s, "\n");
	seq_printf(s, "%-10s%-10d%-10d\n",
		   "Global", pce_rule.glbl_firstindex,
		   pce_rule.glbl_rules_max);
	seq_puts(s, "\n");

	blk = 0;
	list_for_each_entry(blk_entry, &pce_rule.blk_list, list) {
		if (blk == 0) {
			seq_printf(s,
				   "%-10s%-10s%-15s%-10s%-10s%-10s%-10s%-10s%-10s\n",
				   "Block", "", "Block Name", "Region", "PortID",
				   "SubIfId", "FirstIdx", "RuleMax",
				   "RuleAllocated");

			for (i = 0; i < 100; i++)
				seq_puts(s, "=");
			seq_puts(s, "\n");
			blk = 1;
		}
		seq_printf(s,
			   "%-10s%-10s%-15s%-10s%-10d%-10d%-10d%-10d%-10d\n",
			   "", "",
			   blk_entry->blk_name,
			   (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
			   "CTP" : "GLOBAL",
			   blk_entry->info.info.portid,
			   blk_entry->info.info.subif,
			   blk_entry->blk_firstindex,
			   blk_entry->blk_rules_max,
			   blk_entry->blk_rules_used);
	}

	seq_puts(s, "\n");
	subblk = 0;
	list_for_each_entry(blk_entry, &pce_rule.blk_list, list) {
		seq_puts(s, "\n");
		if (subblk == 0) {
			seq_printf(s,
				   "%-10s%-10s%-10s%-10s%-20s%-10s%-10s%-10s%-10s%-10s\n",
				   "Sub-Block", "", "Region", "Blk Name",
				   "Sub-Blk Name", "ID",
				   "FirstIdx", "RuleMax", "RuleUsed",
				   "Protected");

			subblk = 1;
			for (i = 0; i < 110; i++)
				seq_puts(s, "=");
			seq_puts(s, "\n");
		}
		list_for_each_entry(subblk_entry, &blk_entry->subblk_list, list) {
			seq_printf(s,
				   "%-10s%-10s%-10s%-10s%-20s%-10d%-10d%-10d%-10d%-10d\n",
				   "", "",
				   (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
				   "CTP" : "GLOBAL",
				   blk_entry->blk_name,
				   subblk_entry->subblk_name,
				   subblk_entry->subblk_id,
				   subblk_entry->subblk_firstindex,
				   subblk_entry->subblk_rules_max,
				   subblk_entry->subblk_rules_used,
				   subblk_entry->subblk_protected);
		}
	}
	seq_puts(s, "\n");
	return -1;
}

enum _pce_ops {
	PCE_ADD = 0,
	PCE_DEL,
	PCE_GET,
	PCE_BLK_CRE,
	PCE_BLK_DEL,
};

#define PATH_PCE "/sys/kernel/debug/dp/pce"

ssize_t proc_pce_write(struct file *file, const char *buf, size_t count,
		       loff_t *ppos)
{
	char str[100];
	char *param_list[16];
	int num, i = 0;
	size_t len;
	int portid = 0, subifid = 0, region = 0, pceidx = 0, blkid = 0;
	int subblkize = 0, protected = 0, subblk_firstidx = 0;
	int op = -1;
	struct dp_pce_blk_info pce_blk_info = {0};
	struct dp_subblk_info *blk_info;
	GSW_PCE_rule_t *pce;
	GSW_PCE_ruleDelete_t pce_del;
	char subblk_name[32] = {'\0'};
	struct dp_blk_entry *blk_entry;
	struct dp_subblk_entry *subblk_entry;

	if (!capable(CAP_NET_ADMIN))
		return -EPERM;
	len = (sizeof(str) > count) ? count : sizeof(str) - 1;
	len -= copy_from_user(str, buf, len);
	str[len] = 0;

	num = dp_split_buffer(str, param_list, ARRAY_SIZE(param_list));
	i = 0;

	while (1) {
		if (num <= 1 || (!dp_strncmpi(param_list[i], "help",
					      strlen("help")))) {
			goto help;
		} else if (!dp_strncmpi(param_list[i], "blk_cre",
					strlen("blk_cre"))) {
			op = PCE_BLK_CRE;
			i++;
		} else if (!dp_strncmpi(param_list[i], "blk_del",
					strlen("blk_del"))) {
			op = PCE_BLK_DEL;
			i++;
		} else if (!dp_strncmpi(param_list[i], "add",
					strlen("add"))) {
			op = PCE_ADD;
			i++;
		} else if (!dp_strncmpi(param_list[i], "del",
					strlen("del"))) {
			op = PCE_DEL;
			i++;
		} else if (!dp_strncmpi(param_list[i], "get",
					strlen("get"))) {
			op = PCE_GET;
			i++;
		} else if (!dp_strncmpi(param_list[i], "portid",
					strlen("portid"))) {
			if (param_list[i + 1])
				portid = dp_atoi(param_list[i + 1]);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "subifid",
					strlen("subifid"))) {
			if (param_list[i + 1])
				subifid = dp_atoi(param_list[i + 1]);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "region",
					strlen("region"))) {
			if (param_list[i + 1])
				region = dp_atoi(param_list[i + 1]);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "pceidx",
					strlen("pceidx"))) {
			if (param_list[i + 1])
				pceidx = dp_atoi(param_list[i + 1]);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "subblksize",
					strlen("subblksize"))) {
			if (param_list[i + 1])
				subblkize = dp_atoi(param_list[i + 1]);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "blkid",
					strlen("blkid"))) {
			if (param_list[i + 1])
				blkid = dp_atoi(param_list[i + 1]);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "protected",
					strlen("protected"))) {
			if (param_list[i + 1])
				protected = dp_atoi(param_list[i + 1]);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "subblk_name",
					strlen("subblk_name"))) {
			strncpy(subblk_name, param_list[i + 1],
				sizeof(subblk_name) - 1);
			i += 2;
		} else if (!dp_strncmpi(param_list[i], "subblk_firstidx",
					strlen("subblk_firstidx"))) {
			if (param_list[i + 1])
				subblk_firstidx = dp_atoi(param_list[i + 1]);
			i += 2;
		} else {
			break;
		}

		if (i >= num)
			break;
	}

	pce = kzalloc(sizeof(*pce), GFP_ATOMIC);
	if (!pce)
		return -1;

	blk_info = &pce_blk_info.info;

	blk_info->subblk_size = subblkize;
	blk_info->portid = portid;
	blk_info->subif = subifid;

	strncpy(blk_info->subblk_name, subblk_name,
		sizeof(blk_info->subblk_name) - 1);

	pce->logicalportid = portid;
	pce->subifidgroup = subifid;
	pce->region = region;

	pce_del.logicalportid = portid;
	pce_del.subifidgroup = subifid;
	pce_del.region = region;

	blk_entry = get_blk_entry(blk_info, region);
	if (blk_entry) {
		subblk_entry = get_subblk_entry(blk_entry, blkid);
		pce_blk_info.region = blk_entry->info.region;

		if (subblk_entry) {
			DP_DEBUG(DP_DBG_FLAG_PCE,
				 "%s: %s %s %s %d %s %d %s %d\n",
				 __func__,
				 "Region",
				 (blk_entry->info.region == GSW_PCE_RULE_CTP) ?
				 "CTP" : "GLOBAL",
				 "Portid", blk_entry->info.info.portid,
				 "Subifid", blk_entry->info.info.subif,
				 "Block ID", subblk_entry->subblk_id);
		}

		if (subblk_entry)
			memcpy(blk_info, &blk_entry->info.info,
			       sizeof(struct dp_subblk_info));
	}

	blk_info->subblk_id = blkid;
	blk_info->subblk_protected = protected;
	blk_info->subblk_firstidx = subblk_firstidx;

	DP_DEBUG(DP_DBG_FLAG_PCE,
		 "%s: %s %s %s %d %s %d %s %d %s %d %s %d\n",
		 __func__,
		 "Region",
		 (region == GSW_PCE_RULE_CTP) ? "CTP" : "GLOBAL",
		 "Portid", blk_info->portid,
		 "Subifid", blk_info->subif,
		 "subblk_protected", blk_info->subblk_protected,
		 "subblk_firstidx", blk_info->subblk_firstidx,
		 "Block ID", blk_info->subblk_id);

	switch (op) {
	case PCE_BLK_CRE:
		dp_pce_blk_create(&pce_blk_info);
		break;
	case PCE_ADD:
		dp_pce_rule_add(&pce_blk_info, pce);
		break;
	case PCE_DEL:
		pce_del.nIndex = pceidx;
		dp_pce_rule_del(&pce_blk_info, &pce_del);
		break;
	case PCE_GET:
		pce->pattern.nIndex = pceidx;
		dp_pce_rule_get(&pce_blk_info, pce);
		break;
	case PCE_BLK_DEL:
		dp_pce_blk_del(&pce_blk_info);
		break;
	default:
		break;
	};

	kfree(pce);

	return count;
help:
	pr_info("usage:\n");
	pr_info("  echo blk_cre portid x subifid y region z blkid a");
	pr_info(" subblksize c protected x subblk_name X > %s\n", PATH_PCE);
	pr_info("  echo blk_del portid x subifid y region z blkid a");
	pr_info(" subblksize c protected x > %s\n", PATH_PCE);
	pr_info("  echo add portid x subifid y region z pceidx b blkid a");
	pr_info(" subblksize b protected x subblk_name X > %s\n", PATH_PCE);
	pr_info("  echo del portid x subifid y region z pceidx b blkid a");
	pr_info(" subblksize b protected x > %s\n", PATH_PCE);
	pr_info("  echo get portid x subifid y region z pceidx b blkid a");
	pr_info(" subblksize b > %s\n", PATH_PCE);

	return count;
}
