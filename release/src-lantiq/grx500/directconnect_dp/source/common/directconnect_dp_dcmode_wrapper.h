/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef _DIRECTCONNECT_DP_DCMODE_WRAPPER_H_
#define _DIRECTCONNECT_DP_DCMODE_WRAPPER_H_

#include <directconnect_dp_api.h>

extern int32_t
dc_dp_dcmode_wrapper_get_host_capability (
    struct dc_dp_host_cap *cap,
    uint32_t flags
);

int32_t
dc_dp_dcmode_wrapper_get_peripheral_config (
    enum dc_dp_dev_type dev_id,
    struct dc_dp_dev_config *dev_config
);

int32_t dc_dp_dcmode_wrapper_mark_pkt_devqos(
	struct dc_dp_priv_dev_info *dev_ctx,
	uint32_t devqos,
	struct sk_buff *skb
);

extern int32_t
dc_dp_dcmode_wrapper_alloc_port (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct module *owner,
    uint32_t dev_port,
    struct net_device *dev,
    int32_t port_id,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_dealloc_port (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct module *owner,
    uint32_t dev_port,
    struct net_device *dev,
    int32_t port_id,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_register_dev (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct module *owner,
    uint32_t port_id,
    struct net_device *dev,
    struct dc_dp_cb *datapathcb,
    struct dc_dp_res *resources,
    struct dc_dp_dev *devspec,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_deregister_dev (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct module *owner,
    uint32_t port_id,
    struct net_device *dev,
    struct dc_dp_cb *datapathcb,
    struct dc_dp_res *resources,
    struct dc_dp_dev *devspec,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_register_subif(
    struct dc_dp_priv_dev_info *dev_ctx,
    struct module *owner,
    struct net_device *dev,
    const uint8_t *subif_name,
    struct dp_subif *subif_id,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_deregister_subif(
    struct dc_dp_priv_dev_info *dev_ctx,
    struct module *owner,
    struct net_device *dev,
    const uint8_t *subif_name,
    struct dp_subif *subif_id,
    uint32_t flags
);

extern void
dc_dp_dcmode_wrapper_set_subif_param (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct dc_dp_subif *dc_subif
);

extern void
dc_dp_dcmode_wrapper_get_subif_param (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct dc_dp_subif *dc_subif
);

extern int32_t
dc_dp_dcmode_wrapper_xmit (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct net_device *rx_if,
    struct dp_subif *rx_subif,
    struct dp_subif *tx_subif,
    struct sk_buff *skb,
    int32_t len,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_handle_ring_sw (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct module *owner,
    uint32_t port_id,
    struct net_device *dev,
    struct dc_dp_ring *ring,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_return_bufs (
    struct dc_dp_priv_dev_info *dev_ctx,
    uint32_t port_id,
    void **buflist,
    uint32_t buflist_len,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_add_session_shortcut_forward (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct dp_subif *subif,
    struct sk_buff *skb,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_disconn_if (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct net_device *netif,
    struct dp_subif *subif_id,
    uint8_t mac_addr[MAX_ETH_ALEN],
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_get_netif_stats (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct net_device *netif,
    struct dp_subif *subif_id,
    struct rtnl_link_stats64 *if_stats,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_clear_netif_stats (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct net_device *netif,
    struct dp_subif *subif_id,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_get_property (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct net_device *netif,
    struct dp_subif *subif_id,
    struct dc_dp_prop *property
);

extern int32_t
dc_dp_dcmode_wrapper_set_property (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct net_device *netif,
    struct dp_subif *subif_id,
    struct dc_dp_prop *property
);

extern int32_t
dc_dp_dcmode_wrapper_register_qos_class2prio_cb (
    struct dc_dp_priv_dev_info *dev_ctx,
    int32_t port_id,
    struct net_device *netif,
    int (*cb)(int32_t port_id, struct net_device *netif, uint8_t class2prio[]),
    int32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_map_class2devqos (
    struct dc_dp_priv_dev_info *dev_ctx,
    int32_t port_id,
    struct net_device *netif,
    uint8_t class2prio[],
    uint8_t prio2devqos[]
);

extern struct sk_buff *
dc_dp_dcmode_wrapper_alloc_skb (
    struct dc_dp_priv_dev_info *dev_ctx,
    uint32_t len,
    struct dp_subif *subif,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_free_skb (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct dp_subif *subif,
    struct sk_buff *skb
);

extern int32_t
dc_dp_dcmode_wrapper_change_dev_status (
    struct dc_dp_priv_dev_info *dev_ctx,
    int32_t port_id,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_get_wol_cfg (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct dc_dp_wol_cfg *cfg,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_set_wol_cfg (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct dc_dp_wol_cfg *cfg,
    uint32_t flags
);

extern int32_t
dc_dp_dcmode_wrapper_set_wol_ctrl (
    struct dc_dp_priv_dev_info *dev_ctx,
    int32_t port_id,
    uint32_t enable
);

extern int32_t
dc_dp_dcmode_wrapper_get_wol_ctrl_status (
    struct dc_dp_priv_dev_info *dev_ctx,
    int32_t port_id
);

extern void
dc_dp_dcmode_wrapper_dump_proc (
    struct dc_dp_priv_dev_info *dev_ctx,
    struct seq_file *seq
);

#endif /* _DIRECTCONNECT_DP_DCMODE_WRAPPER_H_ */
