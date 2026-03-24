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
#include "qos_mgr_tc_pce.h"

/** Mark PCE rule index as reserved */
#define QOS_MGR_PCE_RESERVED 1
/** Mark PCE rule index as unreserved */
#define QOS_MGR_PCE_UNRESERVED 0

/** Maximum numbers of entries per PCE block */
#define QOS_MGR_PCE_BLOCK_ENTRIES_MAX 64
/** Special value used for common block */
#define QOS_MGR_PCE_BLOCK_COMMON -1

/** This structure represent list element storing relation between
 *  TC command and PCE rule.
 */
struct pce_node {
	/** TC command handle used as dual identificator */
	uint32_t handle;
	/** TC command preference used as dual identificator */
	int pref;
	/** Logical port id, common block is using -1 id. */
	int64_t portid;
	/** CTP id, used for PCE block management,
	 *  Common block is using -1 id.
	 */
	int64_t subif;
	/** PCE rule index used in hardware */
	uint32_t index;
	struct list_head list;
};

/** This structure represent list element storing PCE rules reservation arrays
 *  for common and ctp PCE rules blocks.
 */
struct pce_block_node {
	/** Logical port id, Common block is using -1 id. */
	int64_t portid;
	/** CTP id, used for PCE block management,
	 *  Common block is using -1 id.
	 */
	int64_t subif;
	/** Reserved block id,
	 *  used for non-common block management
	 */
	int64_t blockid;
	/** Reservation table per block */
	uint8_t reserved[QOS_MGR_PCE_BLOCK_ENTRIES_MAX];
	/** Reservation counter,
	 *  used for non-common block management cleanup
	 */
	uint8_t reservation_cnt;
	struct list_head list;
};

static LIST_HEAD(tc_pce_list);
static LIST_HEAD(tc_pce_block_list);

static int32_t pce_block_delete(int portid, int subif,
				struct pce_block_node *block);

static int32_t pce_index_range_get(enum pce_type type,
				   uint32_t *start,
				   uint32_t *stop)
{
	switch (type) {
	case PCE_COMMON:
		*start = 0;
		*stop = QOS_MGR_PCE_BLOCK_ENTRIES_MAX;
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
		return QOS_MGR_FAILURE;
	}

	pr_debug("%s: Range assigned, start: %u, stop: %u\n", __func__,
		*start, *stop);
	return QOS_MGR_SUCCESS;
}

static int32_t pce_free_index_get(uint32_t *index,
				  enum pce_type type,
				  int portid, int subif,
				  struct pce_block_node *block)
{
	struct core_ops *gsw_handle;
	GSW_PCE_rule_t pceRule;
	GSW_return_t ret;
	uint32_t i, j;

	ret = pce_index_range_get(type, &i, &j);
	if (ret != QOS_MGR_SUCCESS)
		return ret;

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle) {
		pr_err("%s: gsw_get_swcore_ops failed\n", __func__);
		return QOS_MGR_FAILURE;
	}

	for (; i < j; i++) {
		if (block->reserved[i] == QOS_MGR_PCE_RESERVED)
			continue;

		pceRule.pattern.nIndex = i;
		if (type != PCE_COMMON) {
			pceRule.logicalportid = portid;
			pceRule.subifidgroup = subif;
			pceRule.region = GSW_PCE_RULE_CTP;
		}

		ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleRead(
				gsw_handle, &pceRule);
		if (ret != GSW_statusOk) {
			pr_err("%s: TFLOW_PceRuleRead failed, nIndex: %u, ret: %d\n",
				__func__, pceRule.pattern.nIndex, ret);
			return QOS_MGR_FAILURE;
		}

		if (!pceRule.pattern.bEnable) {
			*index = i;
			pr_debug("%s: Free index found, index: %u\n", __func__,
				*index);
			return QOS_MGR_SUCCESS;
		}
	}

	pr_err("%s: Free index not found\n", __func__);
	return QOS_MGR_FAILURE;
}

static int32_t pce_rule_write(GSW_PCE_rule_t *pceRule)
{
	struct core_ops *gsw_handle;
	GSW_return_t ret;

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle)
		return QOS_MGR_FAILURE;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleWrite(
			gsw_handle, pceRule);
	if (ret != GSW_statusOk) {
		pr_err("%s: TFLOW_PceRuleWrite failed, nIndex: %u, ret: %d\n",
			__func__, pceRule->pattern.nIndex, ret);
		return QOS_MGR_FAILURE;
	}

	pr_debug("%s: TFLOW_PceRuleWrite success, nIndex: %u\n",
		__func__, pceRule->pattern.nIndex);
	return QOS_MGR_SUCCESS;
}

static int32_t get_subif_data(struct net_device *dev,
			      enum pce_type type,
			      int *portid,
			      int *subif)
{
	dp_subif_t *dp_subif = NULL;
	int32_t ret;

	switch (type) {
	case PCE_COMMON:
		*portid = QOS_MGR_PCE_BLOCK_COMMON;
		*subif = QOS_MGR_PCE_BLOCK_COMMON;
		pr_debug("%s: portid: %u, subif: %u\n", __func__,
			QOS_MGR_PCE_BLOCK_COMMON, QOS_MGR_PCE_BLOCK_COMMON);
		break;
	case PCE_MIRR_VLAN_FWD:
	case PCE_MIRR_VLAN_DROP:
	case PCE_MIRR_VLAN_WILD:
	case PCE_MIRR_UNTAG_FWD:
	case PCE_MIRR_UNTAG_DROP:
		dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
		if (!dp_subif) {
			pr_debug("%s: DP subif allocation failed\n", __func__);
			return QOS_MGR_FAILURE;
		}

		ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, dp_subif, 0);
		if (ret != DP_SUCCESS) {
			kfree(dp_subif);
			return QOS_MGR_FAILURE;
		}
		pr_debug("%s: portid: %u, subif: %u\n", __func__,
			dp_subif->port_id, dp_subif->subif);
		*portid = dp_subif->port_id;
		*subif = dp_subif->subif;
		kfree(dp_subif);
		break;
	default:
		pr_err("%s: PCE type unknown\n", __func__);
		return QOS_MGR_FAILURE;
	}

	return QOS_MGR_SUCCESS;
}

static int32_t pce_block_create(int portid, int subif,
				enum pce_type type,
				struct pce_block_node **block)
{
	struct core_ops *gsw_handle;
	GSW_PCE_rule_alloc_t alloc = {0};
	GSW_CTP_portConfig_t ctp = {0};
	struct pce_block_node *pce_block;
	GSW_return_t ret;

	if (type != PCE_COMMON) {
		gsw_handle = gsw_get_swcore_ops(0);
		if (!gsw_handle) {
			pr_err("%s: gsw_get_swcore_ops failed\n", __func__);
			return QOS_MGR_FAILURE;
		}

		alloc.num_of_rules = QOS_MGR_PCE_BLOCK_ENTRIES_MAX;

		ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleAlloc(
				gsw_handle, &alloc);
		if (ret != GSW_statusOk) {
			pr_err("%s: TFLOW_PceRuleAlloc failed, num_of_rules: %u, ret: %d\n",
				__func__, alloc.num_of_rules, ret);
			return QOS_MGR_FAILURE;
		}

		pr_debug("%s: TFLOW_PceRuleAlloc success, num_of_rules: %u, block_id: %u\n",
			__func__, alloc.num_of_rules, alloc.blockid);

		ctp.nLogicalPortId = portid;
		ctp.nFirstFlowEntryIndex = alloc.blockid;
		ctp.eMask |= GSW_CTP_PORT_CONFIG_FLOW_ENTRY;

		ret = gsw_handle->gsw_ctp_ops.CTP_PortConfigSet(
				gsw_handle, &ctp);
		if (ret != GSW_statusOk) {
			pr_err("%s: CTP_PortConfigSet failed, nLogicalPortId: %u, nFirstFlowEntryIndex: %u, ret: %d\n",
				__func__, ctp.nLogicalPortId,
				ctp.nFirstFlowEntryIndex, ret);
			return QOS_MGR_FAILURE;
		}

		pr_debug("%s: CTP_PortConfigSet success, nLogicalPortId: %u, nFirstFlowEntryIndex: %u\n",
			__func__, ctp.nLogicalPortId, ctp.nFirstFlowEntryIndex);
	}

	pce_block = kzalloc(sizeof(struct pce_block_node), GFP_KERNEL);
	if (!pce_block)
		return QOS_MGR_FAILURE;

	pce_block->subif = subif;
	pce_block->portid = portid;
	if (type != PCE_COMMON)
		pce_block->blockid = alloc.blockid;
	else
		pce_block->blockid = QOS_MGR_PCE_BLOCK_COMMON;
	memset(pce_block->reserved, 0, sizeof(pce_block->reserved));
	pce_block->reservation_cnt = 0;

	list_add(&pce_block->list, &tc_pce_block_list);

	*block = pce_block;

	pr_debug("%s: PCE block created\n", __func__);
	return QOS_MGR_SUCCESS;
}

static int32_t pce_block_get(int portid, int subif,
			     struct pce_block_node **block) {
	struct pce_block_node *p, *n;

	list_for_each_entry_safe(p, n, &tc_pce_block_list, list) {
		if (p->subif == subif && p->portid == portid) {
			*block = p;
			pr_debug("%s: PCE block found\n", __func__);
			return QOS_MGR_SUCCESS;
		}
	}

	pr_debug("%s: PCE block not found\n", __func__);
	return QOS_MGR_FAILURE;
}

int32_t qos_mgr_pce_rule_create(struct net_device *dev,
				uint32_t tc_handle, int pref,
				enum pce_type type,
				GSW_PCE_rule_t *pceRule)
{
	uint32_t index;
	int32_t ret;
	struct pce_node *tc_pce = NULL;
	struct pce_block_node *block;
	int portid, subif;

	ret = get_subif_data(dev, type, &portid, &subif);
	if (ret != QOS_MGR_SUCCESS)
		return ret;

	ret = pce_block_get(portid, subif, &block);
	if (ret != QOS_MGR_SUCCESS) {
		ret = pce_block_create(portid, subif, type, &block);
		if (ret != QOS_MGR_SUCCESS)
			return ret;
	}

	ret = pce_free_index_get(&index, type, portid, subif, block);
	if (ret != QOS_MGR_SUCCESS)
		goto error_block;

	pceRule->pattern.nIndex = index;

	if (type != PCE_COMMON) {
		pceRule->logicalportid = portid;
		pceRule->subifidgroup = subif;
		pceRule->region = GSW_PCE_RULE_CTP;
	}

	ret = pce_rule_write(pceRule);
	if (ret != QOS_MGR_SUCCESS)
		goto error_block;

	tc_pce = kzalloc(sizeof(struct pce_node), GFP_KERNEL);
	if (!tc_pce)
		goto error_block;

	tc_pce->handle = tc_handle;
	tc_pce->pref = pref;
	tc_pce->index = index;
	tc_pce->portid = portid;
	tc_pce->subif = subif;

	list_add(&tc_pce->list, &tc_pce_list);

	block->reserved[index] = QOS_MGR_PCE_RESERVED;
	block->reservation_cnt++;

	pr_debug("%s: PCE rule created\n", __func__);
	return QOS_MGR_SUCCESS;

error_block:
	if (!block->reservation_cnt)
		(void)pce_block_delete(portid, subif, block);

	return ret;
}

static int32_t pce_block_delete(int portid, int subif,
				struct pce_block_node *block)
{
	struct core_ops *gsw_handle;
	GSW_PCE_rule_alloc_t alloc = {0};
	GSW_CTP_portConfig_t ctp = {0};
	GSW_return_t ret;

	if (block->blockid == QOS_MGR_PCE_BLOCK_COMMON) {
		pr_debug("%s: Common block\n", __func__);
		return QOS_MGR_SUCCESS;
	}

	ctp.nLogicalPortId = portid;
	ctp.nFirstFlowEntryIndex = 0xFFFFFFFF;
	ctp.eMask |= GSW_CTP_PORT_CONFIG_FLOW_ENTRY;

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle) {
		pr_err("%s: gsw_get_swcore_ops failed\n", __func__);
		return QOS_MGR_FAILURE;
	}

	ret = gsw_handle->gsw_ctp_ops.CTP_PortConfigSet(
			gsw_handle, &ctp);
	if (ret != GSW_statusOk) {
		pr_err("%s: CTP_PortConfigSet failed, nLogicalPortId: %u, nFirstFlowEntryIndex: %u, ret: %d\n",
			__func__, ctp.nLogicalPortId, ctp.nFirstFlowEntryIndex,
			ret);
		return QOS_MGR_FAILURE;
	}

	pr_debug("%s: CTP_PortConfigSet success, nLogicalPortId: %u, nFirstFlowEntryIndex: %u\n",
		__func__, ctp.nLogicalPortId, ctp.nFirstFlowEntryIndex);
	alloc.blockid = block->blockid;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleFree(
			gsw_handle, &alloc);
	if (ret != GSW_statusOk) {
		pr_err("%s: TFLOW_PceRuleFree failed, block_id: %u, ret: %d\n",
			__func__, alloc.blockid, ret);
		return QOS_MGR_FAILURE;
	}

	list_del(&block->list);
	kfree(block);

	pr_debug("%s: PCE block deleted\n", __func__);
	return QOS_MGR_SUCCESS;
}

int32_t qos_mgr_pce_rule_delete(uint32_t tc_handle, int pref)
{
	struct core_ops *gsw_handle;
	struct pce_node *p, *n;
	bool deletion = false;
	GSW_PCE_ruleDelete_t pceRule = {0};
	struct pce_block_node *block;
	GSW_return_t ret;

	list_for_each_entry_safe(p, n, &tc_pce_list, list) {
		if (tc_handle == p->handle && pref == p->pref) {
			deletion = true;
			break;
		}
	}

	if (!deletion) {
		pr_debug("%s: Nothing to delete\n", __func__);
		return QOS_MGR_FAILURE;
	}

	ret = pce_block_get(p->portid, p->subif, &block);
	if (ret != QOS_MGR_SUCCESS)
		return ret;

	if (p->subif != QOS_MGR_PCE_BLOCK_COMMON &&
		p->portid != QOS_MGR_PCE_BLOCK_COMMON) {
		pceRule.logicalportid = p->portid;
		pceRule.subifidgroup = p->subif;
		pceRule.region = GSW_PCE_RULE_CTP;
	}

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle) {
		pr_err("%s: gsw_get_swcore_ops failed\n", __func__);
		return QOS_MGR_FAILURE;
	}

	pceRule.nIndex = p->index;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleDelete(
			gsw_handle, &pceRule);
	if (ret != GSW_statusOk) {
		pr_err("%s: TFLOW_PceRuleDelete failed, nIndex: %u, ret: %d\n",
			__func__, pceRule.nIndex, ret);
		return QOS_MGR_FAILURE;
	}

	block->reserved[p->index] = QOS_MGR_PCE_UNRESERVED;
	block->reservation_cnt--;

	if (!block->reservation_cnt) {
		ret = pce_block_delete(p->portid, p->subif, block);
		if (ret != QOS_MGR_SUCCESS)
			return ret;
	}

	list_del(&p->list);
	kfree(p);

	pr_debug("%s: PCE rule deleted\n", __func__);
	return QOS_MGR_SUCCESS;
}
