/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include <net/ppa/ppa_stack_al.h>
#include <uapi/net/ppa_api.h>
#include <net/datapath_api_vlan.h>
#include "qos_mgr_tc_lct.h"

struct qos_mgr_lct_control qos_mgr_lct_ctrl[QOS_MGR_LCT_MAX] = {
	{"eth0_0", "eth0_0_1_lct", QOS_MGR_PCE_NOT_CONFIGURED, 40, 41, 42, 43,
	 true},
	{"eth0_1", "eth0_1_1_lct", QOS_MGR_PCE_NOT_CONFIGURED, 44, 45, 46, 47,
	 true},
};

static int32_t pce_rule_lct_mac_to_cpu(struct net_device *dev,
				       struct core_ops *gsw_handle,
				       dp_subif_t dp_subif,
				       uint32_t idx)
{
	GSW_PCE_rule_t pceRule = {0};
	GSW_return_t ret;
	int32_t i;

	pceRule.pattern.nIndex = idx;
	pceRule.pattern.bEnable = 1;
	pceRule.pattern.bPortIdEnable = 1;
	pceRule.pattern.nPortId = dp_subif.port_id;
	pceRule.pattern.bSubIfIdEnable = 1;
	pceRule.pattern.nSubIfId = dp_subif.subif;
	pceRule.pattern.bMAC_DstEnable = 1;
	for (i = 0; i < 6; i++)
		pceRule.pattern.nMAC_Dst[i] = dev->dev_addr[i];

	pceRule.action.ePortMapAction = GSW_PCE_ACTION_PORTMAP_CPU;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleWrite(gsw_handle,
							   &pceRule);
	if (ret != GSW_statusOk) {
		pr_err("%s: TFLOW_PceRuleWrite [LCT MAC to CPU] for interface \"%s\" failed: %d\n",
			__func__, dev->name, ret);
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

static int32_t pce_rule_lct_mc_bc_to_cpu(struct net_device *dev,
					 struct core_ops *gsw_handle,
					 dp_subif_t dp_subif,
					 uint32_t idx)
{
	GSW_PCE_rule_t pceRule = {0};
	GSW_return_t ret;

	pceRule.pattern.nIndex = idx;
	pceRule.pattern.bEnable = 1;
	pceRule.pattern.bPortIdEnable = 1;
	pceRule.pattern.nPortId = dp_subif.port_id;
	pceRule.pattern.bSubIfIdEnable = 1;
	pceRule.pattern.nSubIfId = dp_subif.subif;
	pceRule.pattern.bMAC_DstEnable = 1;
	pceRule.pattern.nMAC_Dst[0] = 0x33;
	pceRule.pattern.nMAC_Dst[1] = 0x33;
	pceRule.pattern.nMAC_DstMask = 0xFFF;
	pceRule.pattern.bEtherTypeEnable = 1;
	pceRule.pattern.nEtherType = ETH_P_IPV6;

	pceRule.action.ePortMapAction = GSW_PCE_ACTION_PORTMAP_CPU;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleWrite(gsw_handle,
							   &pceRule);
	if (ret != GSW_statusOk) {
		pr_err("%s: TFLOW_PceRuleWrite [LCT Multicast Broadcast to CPU] for interface \"%s\" failed: %d\n",
			__func__, dev->name, ret);
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

static int32_t pce_rule_lct_arp_bc_to_cpu(struct net_device *dev,
					  struct core_ops *gsw_handle,
					  dp_subif_t dp_subif,
					  uint32_t idx)
{
	GSW_PCE_rule_t pceRule = {0};
	GSW_return_t ret;
	int32_t i;

	pceRule.pattern.nIndex = idx;
	pceRule.pattern.bEnable = 1;
	pceRule.pattern.bPortIdEnable = 1;
	pceRule.pattern.nPortId = dp_subif.port_id;
	pceRule.pattern.bSubIfIdEnable = 1;
	pceRule.pattern.nSubIfId = dp_subif.subif;
	pceRule.pattern.bMAC_DstEnable = 1;
	for (i = 0; i < 6; i++)
		pceRule.pattern.nMAC_Dst[i] = 0xFF;
	pceRule.pattern.bEtherTypeEnable = 1;
	pceRule.pattern.nEtherType = ETH_P_ARP;

	pceRule.action.ePortMapAction = GSW_PCE_ACTION_PORTMAP_CPU;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleWrite(gsw_handle,
							   &pceRule);
	if (ret != GSW_statusOk) {
		pr_err("%s: TFLOW_PceRuleWrite [LCT ARP Broadcast to CPU] for interface \"%s\" failed: %d\n",
			__func__, dev->name, ret);
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

static int32_t pce_rule_lct_ptp_to_cpu(struct net_device *dev,
				       struct core_ops *gsw_handle,
				       dp_subif_t dp_subif,
				       uint32_t idx)
{
	GSW_PCE_rule_t pceRule = {0};
	GSW_return_t ret;

	pceRule.pattern.nIndex = idx;
	pceRule.pattern.bEnable = 1;
	pceRule.pattern.bPortIdEnable = 1;
	pceRule.pattern.nPortId = dp_subif.port_id;
	pceRule.pattern.bSubIfIdEnable = 1;
	pceRule.pattern.nSubIfId = dp_subif.subif;
	pceRule.pattern.bEtherTypeEnable = 1;
	pceRule.pattern.nEtherType = ETH_P_1588;

	pceRule.action.ePortMapAction = GSW_PCE_ACTION_PORTMAP_CPU;
	pceRule.action.eTrafficClassAction = 2;
	/* Use high priroity queue for PTP next to OMCI */
	pceRule.action.nTrafficClassAlternate = 9;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleWrite(gsw_handle,
							   &pceRule);
	if (ret != GSW_statusOk) {
		pr_err("%s: TFLOW_PceRuleWrite [PTP to CPU] for interface \"%s\" failed: %d\n",
			__func__, dev->name, ret);
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

static void pce_rule_remove(uint32_t idx)
{
	struct core_ops *gsw_handle;
	GSW_PCE_ruleDelete_t pceRule = {0};
	GSW_return_t ret;

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle)
		return;

	pceRule.nIndex = idx;

	ret = gsw_handle->gsw_tflow_ops.TFLOW_PceRuleDelete(gsw_handle,
							    &pceRule);
	if (ret != GSW_statusOk)
		pr_err("%s: TFLOW_PceRuleDelete with idx \"%u\" failed: %d\n",
			__func__, idx, ret);
}

static int32_t pce_rules_lct_to_cpu(struct net_device *dev,
				    uint8_t lct_idx)
{
	struct core_ops *gsw_handle;
	dp_subif_t dp_subif;
	int32_t ret;

	if (qos_mgr_lct_ctrl[lct_idx].configured == QOS_MGR_PCE_CONFIGURED)
		return PPA_SUCCESS;

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle)
		return PPA_FAILURE;

	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &dp_subif, 0);
	if (ret != DP_SUCCESS)
		return PPA_FAILURE;

	ret = pce_rule_lct_mac_to_cpu(dev, gsw_handle, dp_subif,
				      qos_mgr_lct_ctrl[lct_idx].mac_rule_idx);
	if (ret != PPA_SUCCESS)
		return PPA_FAILURE;

	ret = pce_rule_lct_mc_bc_to_cpu(dev, gsw_handle, dp_subif,
					qos_mgr_lct_ctrl[lct_idx].mc_rule_idx);
	if (ret != PPA_SUCCESS) {
		pce_rule_remove(qos_mgr_lct_ctrl[lct_idx].mac_rule_idx);
		return PPA_FAILURE;
	}

	ret = pce_rule_lct_arp_bc_to_cpu(dev, gsw_handle, dp_subif,
					qos_mgr_lct_ctrl[lct_idx].arp_rule_idx);
	if (ret != PPA_SUCCESS) {
		pce_rule_remove(qos_mgr_lct_ctrl[lct_idx].mac_rule_idx);
		pce_rule_remove(qos_mgr_lct_ctrl[lct_idx].mc_rule_idx);
		return PPA_FAILURE;
	}

	ret = pce_rule_lct_ptp_to_cpu(dev, gsw_handle, dp_subif,
				      qos_mgr_lct_ctrl[lct_idx].ptp_rule_idx);
	if (ret != PPA_SUCCESS) {
		pce_rule_remove(qos_mgr_lct_ctrl[lct_idx].mac_rule_idx);
		pce_rule_remove(qos_mgr_lct_ctrl[lct_idx].mc_rule_idx);
		pce_rule_remove(qos_mgr_lct_ctrl[lct_idx].arp_rule_idx);
		return PPA_FAILURE;
	}

	qos_mgr_lct_ctrl[lct_idx].configured = QOS_MGR_PCE_CONFIGURED;
	return PPA_SUCCESS;
}

/* TODO: Remove this function when DP is fixed */
static int32_t cpu_port_disconnect(struct net_device *dev)
{
	struct core_ops *gsw_handle;
	GSW_BRIDGE_portConfig_t bp;
	dp_subif_t dp_subif;
	GSW_return_t ret;
	int32_t err;

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle)
		return PPA_FAILURE;

	err = dp_get_netif_subifid(dev, NULL, NULL, NULL, &dp_subif, 0);
	if (err != GSW_statusOk)
		return PPA_FAILURE;

	bp.nBridgePortId = dp_subif.bport;
	bp.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_handle->gsw_brdgport_ops.BridgePort_ConfigGet(gsw_handle,
								&bp);
	if (ret != GSW_statusOk) {
		pr_err("%s: GSW_BRIDGE_PORT_CONFIG_GET failed: %d\n", __func__,
			ret);
		return PPA_FAILURE;
	}

	bp.nBridgePortMap[0] = 0;
	ret = gsw_handle->gsw_brdgport_ops.BridgePort_ConfigSet(gsw_handle,
								&bp);
	if (ret != GSW_statusOk) {
		pr_err("%s: GSW_BRIDGE_PORT_CONFIG_SET failed: %d\n", __func__,
			ret);
		return PPA_FAILURE;
	}

	return PPA_SUCCESS;
}

int32_t qos_mgr_lct_configure(struct net_device *dev,
			      struct dp_vlan0 *rule)
{
	struct net_device *lct_dev;
	int32_t ret, i;

	for (i = 0; i < QOS_MGR_LCT_MAX; i++) {
		if (!strncmp(qos_mgr_lct_ctrl[i].ifname, dev->name,
			    sizeof(qos_mgr_lct_ctrl[i].ifname)))
			break;
	}

	if (!(i < QOS_MGR_LCT_MAX))
		return PPA_FAILURE;

	lct_dev = dev_get_by_name(&init_net, qos_mgr_lct_ctrl[i].lctname);
	if (!lct_dev)
		return PPA_FAILURE;

	if (qos_mgr_lct_ctrl[i].cpu_port_mapped) {
		ret = cpu_port_disconnect(lct_dev);
		if (ret != PPA_SUCCESS)
			return ret;
		qos_mgr_lct_ctrl[i].cpu_port_mapped = false;
	}

	ret = pce_rules_lct_to_cpu(lct_dev, i);
	if (ret != PPA_SUCCESS)
		return PPA_FAILURE;

	rule->act.ract.act = DP_BP_REASSIGN;
	rule->act.ract.bp_dev = lct_dev;

	return PPA_SUCCESS;
}

void qos_mgr_lct_cleanup(struct dp_vlan0 *rule_head,
			 int32_t rule_cnt)
{
	struct dp_vlan0 *rule = NULL;
	int32_t idx;

	for (idx = 0; idx < rule_cnt; idx++) {
		rule = rule_head + idx;
		if (rule->act.ract.bp_dev)
			dev_put(rule->act.ract.bp_dev);
	}
}

void qos_mgr_lct_lan_ctp(struct net_device *dev,
			 struct dp_tc_vlan *dp_vlan)
{
	int32_t i;

	for (i = 0; i < QOS_MGR_LCT_MAX; i++) {
		if (!strncmp(qos_mgr_lct_ctrl[i].ifname, dev->name,
			    sizeof(qos_mgr_lct_ctrl[i].ifname)))
			break;
	}

	if (!(i < QOS_MGR_LCT_MAX))
		return;

	dp_vlan->def_apply = DP_VLAN_APPLY_CTP;
}
