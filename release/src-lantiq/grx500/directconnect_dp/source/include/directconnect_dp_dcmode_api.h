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

#ifndef _DIRECTCONNECT_DP_DCMODE_API_H_
#define _DIRECTCONNECT_DP_DCMODE_API_H_

#include <directconnect_dp_api.h>
#include <directconnect_dp_debug.h>

#define DC_DP_F_DCMODE_DEREGISTER    0x0001

#define DC_DP_DCMODE_DEV_DEREGISTER  0x0001

enum {
    DC_DP_DCMODE_HW = 0,
    DC_DP_DCMODE_SW = 1,
    DC_DP_DCMODE_MAX
};

#define DC_DP_F_DCMODE_HW    0x0001
#define DC_DP_F_DCMODE_SW    0x0002

#define DC_DP_F_DCMODE_0    0x0010
#define DC_DP_F_DCMODE_1    0x0020

#define DC_DP_MAX_SOC_CLASS	16

struct dc_dp_dcmode_ops {
    int32_t (*get_peripheral_config)(int32_t port_id, enum dc_dp_dev_type dev_id,
                                     struct dc_dp_dev_config *dev_config);
    int32_t (*get_host_capability)(struct dc_dp_host_cap *cap, uint32_t flags);
    int32_t (*mark_pkt_devqos)(uint32_t devqos, struct sk_buff *skb);
    int32_t (*register_dev)(void *dev_ctx,
                    struct module *owner, uint32_t port_id,
                    struct net_device *dev, struct dc_dp_cb *datapathcb,
                    struct dc_dp_res *resources, struct dc_dp_dev *devspec, uint32_t flags);
    int32_t (*register_dev_ex)(void *dev_ctx,
                    struct module *owner, uint32_t port_id,
                    struct net_device *dev, struct dc_dp_cb *datapathcb,
                    struct dc_dp_res *resources, struct dc_dp_dev *devspec,
                    int32_t ref_port_id, uint32_t alloc_flags, uint32_t flags);
    int32_t (*register_subif)(void *dev_ctx,
                    struct module *owner, struct net_device *dev,
                    const uint8_t *subif_name, struct dp_subif *subif_id, uint32_t flags);
    void (*set_subif_param)(void *dev_ctx, struct dc_dp_subif *dc_subif);
    void (*get_subif_param)(void *dev_ctx, struct dc_dp_subif *dc_subif);
    int32_t (*xmit)(void *dev_ctx, struct net_device *rx_if, struct dp_subif *rx_subif, struct dp_subif *tx_subif,
                    struct sk_buff *skb, int32_t len, uint32_t flags);
    int32_t (*handle_ring_sw)(void *dev_ctx, struct module *owner, uint32_t port_id, struct net_device *dev,
                    struct dc_dp_ring *ring, uint32_t flags);
    int32_t (*return_bufs)(void *dev_ctx, uint32_t port_id, void **buflist, uint32_t buflist_len, uint32_t flags);
    int32_t (*add_session_shortcut_forward)(void *dev_ctx, struct dp_subif *subif, struct sk_buff *skb, uint32_t flags);
    int32_t (*disconn_if)(void *dev_ctx, struct net_device *netif, struct dp_subif *subif_id,
                    uint8_t mac_addr[MAX_ETH_ALEN], uint32_t flags);
    int32_t (*get_netif_stats)(void *dev_ctx, struct net_device *netif, struct dp_subif *subif_id,
                    struct rtnl_link_stats64 *if_stats, uint32_t flags);
    int32_t (*clear_netif_stats)(void *dev_ctx, struct net_device *netif, struct dp_subif *subif_id,
                    uint32_t flags);
    int32_t (*get_property)(void *dev_ctx, struct net_device *netif, struct dp_subif *subif_id,
                    struct dc_dp_prop *property);
    int32_t (*set_property)(void *dev_ctx, struct net_device *netif, struct dp_subif *subif_id,
                    struct dc_dp_prop *property);
    int32_t (*register_qos_class2prio_cb)(void *dev_ctx, int32_t port_id, struct net_device *netif,
                    int (*cb)(int32_t port_id, struct net_device *netif, uint8_t class2prio[DC_DP_MAX_SOC_CLASS]),
                    int32_t flags);
    int32_t (*map_class2devqos)(void *dev_ctx, int32_t port_id, struct net_device *netif,
                                uint8_t class2prio[DC_DP_MAX_SOC_CLASS], uint8_t prio2devqos[]);
    struct sk_buff * (*alloc_skb)(void *dev_ctx, uint32_t len, struct dp_subif *subif, uint32_t flags);
    int32_t (*free_skb)(void *dev_ctx, struct dp_subif *subif, struct sk_buff *skb);
    int32_t (*change_dev_status)(void *dev_ctx, int32_t port_id, uint32_t flags);
    int32_t (*get_wol_cfg)(void *dev_ctx, int32_t port_id, struct dc_dp_wol_cfg *cfg, uint32_t flags);
    int32_t (*set_wol_cfg)(void *dev_ctx, int32_t port_id, struct dc_dp_wol_cfg *cfg, uint32_t flags);
    int32_t (*set_wol_ctrl)(void *dev_ctx, int32_t port_id, uint32_t enable);
    int32_t (*get_wol_ctrl_status)(void *dev_ctx, int32_t port_id);
    void (*dump_proc)(void *dev_ctx, struct seq_file *seq);
};

struct dc_dp_dcmode {
    uint32_t dcmode_cap;
    struct dc_dp_dcmode_ops *dcmode_ops;
};

struct dc_dp_dev_mib {
    unsigned long rx_fn_rxif_pkts;	/* total packets received	*/
    unsigned long tx_pkts;		/* total packets transmitted	*/
    unsigned long rx_fn_txif_pkts;	/* total packets transmitted	*/
    unsigned long rx_fn_rxif_bytes;	/* total bytes received 	*/
    unsigned long tx_bytes;		/* total bytes transmitted	*/
    unsigned long rx_fn_txif_bytes;	/* total bytes received		*/
    unsigned long rx_fn_rxif_dropped;	/* no space in linux buffers	*/
    unsigned long tx_dropped;		/* no space available in linux	*/
    unsigned long rx_fn_txif_dropped;	/* no space available in linux	*/
    unsigned long rx_fn_rxif_errs;	/* bad packets received		*/
    unsigned long tx_errs;		/* packet transmit problems	*/
    unsigned long rx_fn_txif_errs;	/* packet transmit problems	*/
};

int32_t
dc_dp_register_dcmode(struct dc_dp_dcmode *dcmode, uint32_t flags);

int32_t dc_dp_dcmode_init(void);
int32_t dc_dp_dcmode_exit(void);

/* Common layer function */
extern int32_t
dc_dp_rx(struct net_device *rxif, struct net_device *txif, struct dp_subif *rx_subif,
         struct sk_buff *skb, int32_t len, uint32_t flags);

extern int32_t
dc_dp_check_if_netif_fastpath(struct net_device *netif, char *ifname, uint32_t flags);

extern int32_t
dc_dp_get_netif_subifid(struct net_device *netif, struct sk_buff *skb,
                        void *subif_data, uint8_t dst_mac[MAX_ETH_ALEN],
                        struct dp_subif *subif, uint32_t flags);

extern int32_t
dc_dp_qos_class2prio(int32_t port_id, struct net_device *netif, uint8_t *class2prio);

int32_t
dc_dp_get_dev_specific_desc_info(int32_t port_id, struct sk_buff *skb,
                                 struct dc_dp_fields_value_dw *desc_fields, uint32_t flags);

extern int32_t
dc_dp_register_dcmode_device(struct module *owner, int32_t port_id, struct net_device *dev, void *p_dcmode_devinfo, uint32_t flags);

extern int32_t
dc_dp_get_dcmode_device(int32_t port_id, void **p_devinfo);

extern int32_t
dc_dp_get_dcmode(void *ctx, int32_t port_id, struct dc_dp_res *res, struct dc_dp_dcmode **pp_dcmode, uint32_t flags);

#endif
