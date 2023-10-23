/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2021 MaxLinear, Inc.
 *
 *****************************************************************************/

/**
 *  Drop packets based on IP address using PCE rules.
 *
 *  \param[in]	dev		Network device
 *  \param[in]	f		Flower offload
 *  \param[in]	tc_handle	TC command identifier
 */
int pon_qos_ip_drop_offload(struct net_device *dev,
			    struct tc_cls_flower_offload *f,
			    uint32_t tc_handle);

/**
 *  Remove packets drop by removing PCE rule.
 *
 *  \param[in]	dev		Network device
 *  \param[in]	f		Flower offload
 *  \param[in]	tc_handle	TC command identifier
 */
int pon_qos_ip_drop_unoffload(struct net_device *dev,
			      struct tc_cls_flower_offload *f,
			      uint32_t tc_handle);
