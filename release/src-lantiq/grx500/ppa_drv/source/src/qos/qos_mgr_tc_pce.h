/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

enum pce_type {
	PCE_COMMON = 0,
	PCE_MIRR_VLAN_FWD = 1,
	PCE_MIRR_VLAN_DROP = 2,
	PCE_MIRR_VLAN_WILD = 3,
	PCE_MIRR_UNTAG_FWD = 4,
	PCE_MIRR_UNTAG_DROP = 5,
};

/**
 *  Create PCE rule in hardware.
 *
 *  \param[in]	tc_handle	TC command identifier
 *  \param[in]	pref		TC command preference
 *  \param[in]	pceRule		PCE rule derived from TC command
 *
 *  \return     QOS_MGR_SUCCESS or QOS_MGR_FAILURE
 */
int32_t qos_mgr_pce_rule_create(struct net_device *dev,
				uint32_t tc_handle, int pref,
				enum pce_type type,
				GSW_PCE_rule_t *pceRule);

/**
 *  Delete PCE rule from hardware.
 *
 *  \param[in]	tc_handle	TC command identifier
 *  \param[in]	pref		TC command preference
 *
 *  \return     QOS_MGR_SUCCESS or QOS_MGR_FAILURE
 */
int32_t qos_mgr_pce_rule_delete(uint32_t tc_handle, int pref);
