#ifndef __QOS_MGR_TC_HOOK_H
#define __QOS_MGR_TC_HOOK_H
/******************************************************************************
 *
 * Copyright (C) 2017-2018 Intel Corporation
 * Copyright (C) 2010-2016 Lantiq Beteiligungs-GmbH & Co. KG
 * Author  : Purnendu Ghosh
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * SPDX-License-Identifier: GPL-2.0
 *
 *******************************************************************************/
/*! \file qos_mgr_tc_hook.h
  \brief This file contains TC exported HOOK API to linux Kernel builtin.
 */
/** \addtogroup  QOS_MGR_TC_HOOK_API */
/*@{*/
/*
 * ####################################
 *             Declaration
 * ####################################
 */
#ifdef __KERNEL__
#define NETDEV_TYPE_TCONT 0x100000
extern int32_t (*qos_mgr_hook_setup_tc)(struct net_device *dev, u32 handle, __be16 protocol, struct tc_to_netdev *tc);
extern int32_t (*qos_mgr_hook_setup_tc_ext)(struct net_device *dev, u32 handle, __be16 protocol, struct tc_to_netdev *tc, int32_t deq_idx, int32_t port_id);
#endif /*end of __KERNEL__*/
/* @} */
#endif  /*  __QOS_MGR_TC_HOOK_H */
