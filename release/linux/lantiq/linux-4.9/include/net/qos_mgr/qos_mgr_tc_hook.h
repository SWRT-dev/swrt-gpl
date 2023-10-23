// SPDX-License-Identifier: GPL-2.0
/* Copyright (C) 2019~2020 Intel Corporation.
 */
#ifndef __QOS_MGR_TC_HOOK_H
#define __QOS_MGR_TC_HOOK_H
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
#define NETDEV_TYPE_TCONT 0x100000
#define QOS_MGR_PON_TCONT_DEV 0x100000
#define QOS_MGR_PON_CPU_PORT 0x200000
extern int32_t (*qos_mgr_hook_setup_tc)(struct net_device *dev, u32 handle, __be16 protocol, struct tc_to_netdev *tc);
extern int32_t (*qos_mgr_hook_setup_tc_ext)(struct net_device *dev, u32 handle, __be16 protocol, struct tc_to_netdev *tc, int32_t deq_idx, int32_t port_id, int32_t flags);
/* @} */
#endif  /*  __QOS_MGR_TC_HOOK_H */
