/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

#include <linux/if.h>

/** Maximum number of supported LAN ports */
#define QOS_MGR_LCT_MAX 2
/** PCE rule not configured */
#define QOS_MGR_PCE_NOT_CONFIGURED 0
/** PCE rule configured */
#define QOS_MGR_PCE_CONFIGURED 1

/** LCT configuration */
struct qos_mgr_lct_control {
	/** LAN interface name */
	char ifname[IFNAMSIZ];
	/** LCT interface name */
	char lctname[IFNAMSIZ];
	/** PCE rules for LCT configured */
	uint8_t configured;
	/** PCE MAC rule index */
	int32_t mac_rule_idx;
	/** PCE Multicast Broadcast rule index */
	int32_t mc_rule_idx;
	/** PCE ARP broadcast rule index */
	int32_t arp_rule_idx;
	/** PCE IEEE1588 (PTP) rule index */
	int32_t ptp_rule_idx;
	/** CPU port is mapped in LCT (HW default: true) */
	uint8_t cpu_port_mapped;
	/* TODO: Remove manual mapping control when DP is fixed */
};

struct dp_vlan0;
struct dp_tc_vlan;

/**
 *  Configure LCT associated with device for LAN interfaces.
 *  Update "rule" for bridge port reassignment (working only for CTP).
 *
 *  \param[in] dev	Network device.
 *  \param[out] rule	Untagged extended VLAN rule.
 */
int32_t qos_mgr_lct_configure(struct net_device *dev,
			      struct dp_vlan0 *rule);

/**
 *  Cleanup allocated resources used for LCT configuration.
 *
 *  \param[in] rule_head	Head of untagged extended VLAN rule list.
 *  \param[in] rule_cnt		Number of untagged extended VLAN rules.
 */
void qos_mgr_lct_cleanup(struct dp_vlan0 *rule_head,
			 int32_t rule_cnt);

/**
 * Configure LAN interfaces rules associated with LCT on CTP.
 *
 *  \param[in] dev	Network device.
 *  \param[out] dp_vlan VLAN configuration for base device.
 */
void qos_mgr_lct_lan_ctp(struct net_device *dev,
			 struct dp_tc_vlan *dp_vlan);
