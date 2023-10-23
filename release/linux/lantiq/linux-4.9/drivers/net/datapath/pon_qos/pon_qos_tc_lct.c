// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include <net/datapath_api_vlan.h>
#include <net/datapath_api.h>
#include "pon_qos_tc_lct.h"

/** Maximum number of supported LAN ports */
#define PON_QOS_LCT_MAX 2
/** PCE rule not configured */
#define PON_QOS_PCE_NOT_CONFIGURED 0
/** PCE rule configured */
#define PON_QOS_PCE_CONFIGURED 1

/** LCT configuration */
struct pon_qos_lct_control {
	/** LAN interface name */
	char ifname[IFNAMSIZ];
	/** LCT interface name */
	char lctname[IFNAMSIZ];
	/** PCE rules for LCT configured */
	u8 configured;
	/** PCE MAC rule index */
	s32 mac_rule_idx;
	/** PCE Multicast Broadcast rule index */
	s32 mc_rule_idx;
	/** PCE ARP broadcast rule index */
	s32 arp_rule_idx;
	/** PCE IEEE1588 (PTP) rule index */
	s32 ptp_rule_idx;
	/** CPU port is mapped in LCT (HW default: true) */
	u8 cpu_port_mapped;
	/* TODO: Remove manual mapping control when DP is fixed */
};

static struct pon_qos_lct_control pon_qos_lct_db[PON_QOS_LCT_MAX] = {
	{"eth0_0", "eth0_0_1_lct", PON_QOS_PCE_NOT_CONFIGURED, 40, 41, 42, 43,
	 true},
	{"eth0_1", "eth0_1_1_lct", PON_QOS_PCE_NOT_CONFIGURED, 44, 45, 46, 47,
	 true},
};

/* TODO: Remove this function when DP is fixed */
static int cpu_port_disconnect(struct net_device *dev)
{
	struct core_ops *gsw_handle;
	GSW_BRIDGE_portConfig_t bp;
	dp_subif_t dp_subif;
	GSW_return_t ret;
	s32 err;

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle)
		return -EINVAL;

	err = dp_get_netif_subifid(dev, NULL, NULL, NULL, &dp_subif, 0);
	if (err != GSW_statusOk)
		return -ENODEV;

	bp.nBridgePortId = dp_subif.bport;
	bp.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_handle->gsw_brdgport_ops.BridgePort_ConfigGet(gsw_handle,
								&bp);
	if (ret != GSW_statusOk) {
		pr_err("%s: GSW_BRIDGE_PORT_CONFIG_GET failed: %d\n", __func__,
		       ret);
		return ret;
	}

	bp.nBridgePortMap[0] = 0;
	ret = gsw_handle->gsw_brdgport_ops.BridgePort_ConfigSet(gsw_handle,
								&bp);
	if (ret != GSW_statusOk) {
		pr_err("%s: GSW_BRIDGE_PORT_CONFIG_SET failed: %d\n", __func__,
		       ret);
		return ret;
	}

	return 0;
}

int pon_qos_lct_configure(struct net_device *dev,
			  struct dp_tc_vlan *dp_vlan,
			  struct dp_vlan0 *rule)
{
	struct net_device *lct_dev;
	s32 ret, i;

	if (dp_vlan->def_apply != DP_VLAN_APPLY_CTP)
		return -EINVAL;

	for (i = 0; i < PON_QOS_LCT_MAX; i++) {
		if (!strncmp(pon_qos_lct_db[i].ifname, dev->name,
			     sizeof(pon_qos_lct_db[i].ifname)))
			break;
	}

	if (!(i < PON_QOS_LCT_MAX))
		return -EINVAL;

	lct_dev = dev_get_by_name(&init_net, pon_qos_lct_db[i].lctname);
	if (!lct_dev)
		return -ENODEV;

	if (pon_qos_lct_db[i].cpu_port_mapped) {
		ret = cpu_port_disconnect(lct_dev);
		if (ret != 0)
			return ret;
		pon_qos_lct_db[i].cpu_port_mapped = false;
	}

	rule->act.ract.act = DP_BP_REASSIGN;
	rule->act.ract.bp_dev = lct_dev;

	return 0;
}

void pon_qos_lct_cleanup(struct dp_vlan0 *rule)
{
	if (rule->act.ract.bp_dev)
		dev_put(rule->act.ract.bp_dev);
}

void pon_qos_lct_lan_ctp(struct net_device *dev,
			 struct dp_tc_vlan *dp_vlan)
{
	int i;

	for (i = 0; i < PON_QOS_LCT_MAX; i++) {
		if (!strncmp(pon_qos_lct_db[i].ifname, dev->name,
			     sizeof(pon_qos_lct_db[i].ifname)))
			break;
	}

	if (!(i < PON_QOS_LCT_MAX))
		return;

	dp_vlan->def_apply = dp_vlan->dir == DP_DIR_INGRESS ?
			     DP_VLAN_APPLY_CTP : 0;
}
