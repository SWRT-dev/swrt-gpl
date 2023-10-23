/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

/**
 *  Offload mirred actions to HW using PCE rules.
 *
 *  \param[in]	dev		Network device
 *  \param[in]	f		Flower offload
 *  \param[in]	tc_handle	TC command identifier
 *
 *  \return     QOS_MGR_SUCCESS or QOS_MGR_FAILURE
 */
int qos_mgr_mirred_offload(struct net_device *dev,
			   struct tc_cls_flower_offload *f,
			   uint32_t tc_handle);

/**
 *  Remove mirred actions from HW by removing PCE rule.
 *
 *  \param[in]	dev		Network device
 *  \param[in]	f		Flower offload
 *  \param[in]	tc_handle	TC command identifier
 *
 *  \return     QOS_MGR_SUCCESS or QOS_MGR_FAILURE
 */
int qos_mgr_mirred_unoffload(struct net_device *dev,
			     struct tc_cls_flower_offload *f,
			     uint32_t tc_handle);
