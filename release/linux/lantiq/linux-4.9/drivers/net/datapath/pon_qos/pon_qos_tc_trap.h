/* SPDX-License-Identifier: GPL-2.0 */
/******************************************************************************
 *
 * Copyright (c) 2020 Intel Corporation
 *
 *****************************************************************************/

/**
 *  Offload trap actions to HW using PCE rules.
 *
 *  \param[in]	dev		Network device
 *  \param[in]	f		Flower offload
 *  \param[in]	tc_handle	TC command identifier
 */
int pon_qos_trap_offload(struct net_device *dev,
			 const struct tc_cls_flower_offload *f,
			 uint32_t tc_handle);

/**
 *  Remove trap actions from HW by removing PCE rule.
 *
 *  \param[in]	dev		Network device
 *  \param[in]	f		Flower offload
 *  \param[in]	tc_handle	TC command identifier
 */
int pon_qos_trap_unoffload(struct net_device *dev,
			   const struct tc_cls_flower_offload *f,
			   uint32_t tc_handle);

