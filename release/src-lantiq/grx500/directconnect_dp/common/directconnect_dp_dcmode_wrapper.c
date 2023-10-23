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

#include <linux/module.h>
#include <directconnect_dp_dcmode_api.h>
#include <directconnect_dp_platform_cfg.h>
#include "directconnect_dp_device.h"
#include "directconnect_dp_swpath.h"
#include "directconnect_dp_litepath.h"

/* NOTE : Generic Datapath driver MUST have a common macro. */
#if defined(HAVE_DATAPATH_EXT_API)
    #define DP_F_FASTPATH DP_F_ACA
#elif defined(HAVE_DATAPATH_API)
    #define DP_F_FASTPATH DP_F_FAST_WLAN
#endif

extern struct dc_dp_dcmode **dc_dp_get_dcmode_head(void);
/* FIXME: FAST_WLAN port context, which is allocated last */
static int32_t fast_wlan_port_ctx = 0;
static struct dc_dp_dcmode *g_dcmode_hw_ctx = NULL;

int32_t
dc_dp_dcmode_wrapper_get_peripheral_config(enum dc_dp_dev_type dev_id,
                                           struct dc_dp_dev_config *dev_config)
{
    int32_t ret = 0;
    int32_t dcmode_idx;
    struct dc_dp_dcmode **dcmode_head = NULL;
    struct dc_dp_dcmode *dcmode = NULL;

    dev_config->txin_ring_size = DC_DP_CFG_MAX_TXIN_RING_SZ;
    dev_config->txout_ring_size = DC_DP_CFG_MAX_TXOUT_RING_SZ;
    dev_config->rxin_ring_size = DC_DP_CFG_MAX_RXIN_RING_SZ;
    dev_config->rxout_ring_size = DC_DP_CFG_MAX_RXOUT_RING_SZ;

    if (dev_id == DC_DP_DEV_WAV_600_CDB) {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_600_CDB;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_600_CDB;
    } else if (dev_id == DC_DP_DEV_WAV_600_24G) {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_600_24G;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_600_24G;
    } else if (dev_id == DC_DP_DEV_WAV_600_5G) {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_600_5G;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_600_5G;
    } else if (dev_id == DC_DP_DEV_WAV_600_6G) {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_600_6G;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_600_6G;
    } else {
        dev_config->rx_bufs = DC_DP_CFG_MAX_RX_BUFS_WAV_500;
        dev_config->tx_bufs = DC_DP_CFG_MAX_TX_BUFS_WAV_500;
    }

    if (fast_wlan_port_ctx && is_hw_port(fast_wlan_port_ctx)) {
        dcmode_head = dc_dp_get_dcmode_head();

        for (dcmode_idx = 0; dcmode_idx < DC_DP_DCMODE_MAX; dcmode_idx++) {
            dcmode = dcmode_head[dcmode_idx];
            if (dcmode != NULL && dcmode->dcmode_ops != NULL && dcmode->dcmode_ops->get_peripheral_config) {
                /* FXIME: Assuming port already allocated and saved as fast_wlan_port_ctx */
                ret = dcmode->dcmode_ops->get_peripheral_config(fast_wlan_port_ctx, dev_id, dev_config);
            }
        }
    }

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_get_host_capability(struct dc_dp_host_cap *cap, uint32_t flags)
{
    int32_t dcmode_idx;
    struct dc_dp_dcmode **dcmode_head = NULL;
    struct dc_dp_dcmode *dcmode = NULL;

    dcmode_head = dc_dp_get_dcmode_head();

    for (dcmode_idx = 0; dcmode_idx < DC_DP_DCMODE_MAX; dcmode_idx++) {
        dcmode = dcmode_head[dcmode_idx];
        if (dcmode != NULL && dcmode->dcmode_ops != NULL) {
            dcmode->dcmode_ops->get_host_capability(cap, flags);
        }
    }

    cap->litepath.support = 1;
    return 0;
}

int32_t dc_dp_dcmode_wrapper_mark_pkt_devqos(
	struct dc_dp_priv_dev_info *dev_ctx,
        uint32_t devqos,
        struct sk_buff *skb)
{
	int32_t ret = DC_DP_SUCCESS;

	if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->mark_pkt_devqos)
		ret = dev_ctx->dcmode->dcmode_ops->mark_pkt_devqos(devqos, skb);

	return ret;
}

int32_t
dc_dp_dcmode_wrapper_alloc_port(struct dc_dp_priv_dev_info *dev_ctx,
                                struct module *owner, uint32_t dev_port,
                                struct net_device *dev, int32_t port_id, uint32_t flags)
{
    int32_t ret = -1;
    uint32_t dp_alloc_flags = 0;
    int32_t dp_port_id = port_id;

    /* Allocate a port (HW or SW) */

    /* FIXME : DP_F_FAST_DSL flag use in DP API? */
    if ( (flags & (DC_DP_F_FASTPATH | DC_DP_F_FAST_DSL)) ) {
        dp_alloc_flags = DP_F_FASTPATH;

#if !defined(HAVE_DATAPATH_MULTI_PORT)
        /* Allocate a DCDP port for Fastpath multiport/shared device */
        if ( (flags & DC_DP_F_MULTI_PORT) && (flags & DC_DP_F_SHARED_RES) )
            ret = dp_alloc_sw_port(owner, dev, dev_port, port_id, flags);
        else
#endif /* #if !defined(HAVE_DATAPATH_MULTI_PORT) */

        /* Otherwise, allocate a DP port */
        {
#if defined(HAVE_DATAPATH_EXT_API)
            /* Convert the alloc flags */
            if ( (flags & DC_DP_F_FAST_WLAN) )
                dp_alloc_flags |= DP_F_FAST_WLAN;
            else if ( (flags & DC_DP_F_FAST_DSL) )
                dp_alloc_flags |= DP_F_FAST_DSL;
            else
                dp_alloc_flags |= DP_F_FAST_WLAN;

#if defined(HAVE_DATAPATH_MULTI_PORT)
            if ( (flags & DC_DP_F_MULTI_PORT) ) {
                if ( (flags & DC_DP_F_SHARED_RES) )
                    dp_port_id = port_id + 1;
                dp_alloc_flags |= DP_F_SHARE_RES;
            }
#endif /* #if defined(HAVE_DATAPATH_MULTI_PORT) */

            ret = dp_alloc_port_ext(0, owner, dev, dev_port, dp_port_id, NULL, NULL, dp_alloc_flags);

#elif defined(HAVE_DATAPATH_API) /* #if defined(HAVE_DATAPATH_EXT_API) */
            /* DP alloc flags */
            if ( (flags & DC_DP_F_FAST_DSL) )
                dp_alloc_flags |= DP_F_FAST_DSL;

            ret = dp_alloc_port(owner, dev, dev_port, dp_port_id, NULL, dp_alloc_flags);
#endif /* #elif defined(HAVE_DATAPATH_API) */
        }
    }

    /* Allocate a DCDP port for Litepath/Swpath device */
    if ((ret < 0) && (flags & (DC_DP_F_LITEPATH | DC_DP_F_SWPATH)))
        ret = dp_alloc_sw_port(owner, dev, dev_port, port_id, flags);

    if (flags & DC_DP_F_FAST_WLAN)
        fast_wlan_port_ctx = ret;

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_dealloc_port(struct dc_dp_priv_dev_info *dev_ctx,
                                  struct module *owner, uint32_t dev_port,
                                  struct net_device *dev, int32_t port_id, uint32_t flags)
{
    int32_t ret = -1;

    /* De-allocate a port (HW or SW) */
    if (is_hw_port(port_id)) {
#if defined(HAVE_DATAPATH_EXT_API)
        ret = dp_alloc_port_ext(0, owner, dev, dev_port, port_id, NULL, NULL, DP_F_DEREGISTER);
#elif defined(HAVE_DATAPATH_API)
        ret = dp_alloc_port(owner, dev, dev_port, port_id, NULL, DP_F_DEREGISTER);
#endif /* #ifdef HAVE_DATAPATH_API */
    } else
        ret = dp_dealloc_sw_port(owner, dev, dev_port, port_id, 0);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_register_dev(struct dc_dp_priv_dev_info *dev_ctx,
                       struct module *owner, uint32_t port_id,
                       struct net_device *dev, struct dc_dp_cb *datapathcb,
                       struct dc_dp_res *resources, struct dc_dp_dev *devspec, uint32_t flags)
{
    int32_t ret = -1;
    int32_t litepath_ret = -1;
    struct dc_dp_dcmode *dcmode = NULL;

    if (!dev_ctx)
        goto err_out;

    ret = dc_dp_get_dcmode((void *)dev_ctx, dev_ctx->ref_port_id, resources, &dcmode, flags);
    if (ret)
        goto litepath_reg_dev;

    dev_ctx->dcmode = dcmode;

    if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->register_dev_ex)
        ret = dev_ctx->dcmode->dcmode_ops->register_dev_ex(dev_ctx->priv, owner, port_id, dev, datapathcb, resources, devspec,
                                                           dev_ctx->ref_port_id, dev_ctx->alloc_flags, flags);
    else if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->register_dev)
        ret = dev_ctx->dcmode->dcmode_ops->register_dev(dev_ctx->priv, owner, port_id, dev, datapathcb, resources, devspec, flags);

litepath_reg_dev:
    /* Register as LITEPATH mode */
    if (is_sw_port(port_id) && (dev_ctx->alloc_flags & DC_DP_F_LITEPATH))
        litepath_ret = dc_dp_litepath_register_dev(dev_ctx, owner, port_id, dev, datapathcb, devspec, flags);

    if (ret)
        ret = litepath_ret;

err_out:
    return ret;
}

int32_t
dc_dp_dcmode_wrapper_deregister_dev(struct dc_dp_priv_dev_info *dev_ctx,
                       struct module *owner, uint32_t port_id,
                       struct net_device *dev, struct dc_dp_cb *datapathcb,
                       struct dc_dp_res *resources, struct dc_dp_dev *devspec, uint32_t flags)
{
    int32_t ret = -1;

    if (!dev_ctx)
        goto err_out;

    if (is_litepath_dev_registered(dev_ctx, port_id, dev))
        ret = dc_dp_litepath_deregister_dev(dev_ctx, owner, port_id, dev, datapathcb, devspec, flags);

    if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->register_dev_ex)
            ret = dev_ctx->dcmode->dcmode_ops->register_dev_ex(dev_ctx->priv, owner, port_id, dev, datapathcb, resources, devspec,
                                                               dev_ctx->ref_port_id, dev_ctx->alloc_flags, flags);
    else if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->register_dev)
        ret = dev_ctx->dcmode->dcmode_ops->register_dev(dev_ctx->priv, owner, port_id, dev, datapathcb, resources, devspec, flags);

err_out:
    return ret;
}

int32_t
dc_dp_dcmode_wrapper_register_subif(struct dc_dp_priv_dev_info *dev_ctx,
                         struct module *owner, struct net_device *dev,
                         const uint8_t *subif_name, struct dp_subif *subif_id, uint32_t flags)
{
    int32_t ret = -1;
    struct dc_dp_priv_subif_info *p_subifinfo = NULL;

    if (!dev_ctx)
        goto err_out;

    if (flags & DC_DP_F_REGISTER_LITEPATH) {
        if (!dev_ctx->dcmode) {
            /* Litepath only */
            if (dc_dp_alloc_subif(dev_ctx, dev, subif_id, &p_subifinfo))
                goto err_out;

            subif_id->subif = p_subifinfo->subif;
        }
        ret = dc_dp_litepath_register_subif(dev_ctx, owner, dev, subif_name, subif_id, flags);
        if (ret < 0 && !dev_ctx->dcmode)
            dc_dp_free_subif(p_subifinfo);
    } else {
        if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->register_subif)
            ret = dev_ctx->dcmode->dcmode_ops->register_subif(dev_ctx->priv, owner, dev, subif_name, subif_id, flags);
    }

err_out:
    return ret;
}

int32_t
dc_dp_dcmode_wrapper_deregister_subif(struct dc_dp_priv_dev_info *dev_ctx,
                         struct module *owner, struct net_device *dev,
                         const uint8_t *subif_name, struct dp_subif *subif_id, uint32_t flags)
{
    int32_t ret = -1;

    if (!dev_ctx)
        goto err_out;

    if (is_litepath_subif_registered(dev_ctx, subif_id)) {

        ret = dc_dp_litepath_deregister_subif(dev_ctx, owner, dev, subif_name, subif_id, DC_DP_F_DEREGISTER_LITEPATH);
        if (!dev_ctx->dcmode) {
            int32_t subif_idx;

            subif_idx = ((subif_id->subif >> DC_DP_SUBIFID_OFFSET) & DC_DP_SUBIFID_MASK);

            /* Litepath only */
            dc_dp_free_subif(&dev_ctx->subif_info[subif_idx]);
        }
    } else {
        if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->register_subif)
            ret = dev_ctx->dcmode->dcmode_ops->register_subif(dev_ctx->priv, owner, dev, subif_name, subif_id, flags);
    }

err_out:
    return ret;
}

void
dc_dp_dcmode_wrapper_set_subif_param(struct dc_dp_priv_dev_info *dev_ctx, struct dc_dp_subif *dc_subif)
{
        if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->xmit)
            dev_ctx->dcmode->dcmode_ops->set_subif_param(dev_ctx->priv, dc_subif);
}

void
dc_dp_dcmode_wrapper_get_subif_param(struct dc_dp_priv_dev_info *dev_ctx, struct dc_dp_subif *dc_subif)
{
        if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->xmit)
            dev_ctx->dcmode->dcmode_ops->get_subif_param(dev_ctx->priv, dc_subif);
}

int32_t
dc_dp_dcmode_wrapper_xmit(struct dc_dp_priv_dev_info *dev_ctx,
                          struct net_device *rx_if, struct dp_subif *rx_subif, struct dp_subif *tx_subif,
                          struct sk_buff *skb, int32_t len, uint32_t flags)
{
    int32_t ret = -1;

    if (!dev_ctx)
        goto err_out;

    if ( (flags & DC_DP_F_XMIT_LITEPATH) )
        ret = dc_dp_litepath_xmit(dev_ctx, rx_if, rx_subif, tx_subif, skb, skb->len, flags);
    else {
        if (is_hw_port(tx_subif->port_id)) {
            dc_dp_set_ep_pkt(skb, tx_subif->port_id, 0);
            dc_dp_set_subifid_pkt(tx_subif->port_id, skb, tx_subif->subif, 0);
        }
        if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->xmit)
            ret = dev_ctx->dcmode->dcmode_ops->xmit(dev_ctx->priv, rx_if, rx_subif, tx_subif, skb, len, flags);
    }

err_out:
    return ret;
}

int32_t
dc_dp_dcmode_wrapper_handle_ring_sw(struct dc_dp_priv_dev_info *dev_ctx, struct module *owner, uint32_t port_id,
                     struct net_device *dev, struct dc_dp_ring *ring, uint32_t flags)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->handle_ring_sw)
        ret = dev_ctx->dcmode->dcmode_ops->handle_ring_sw(dev_ctx->priv, owner, port_id, dev, ring, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_return_bufs(struct dc_dp_priv_dev_info *dev_ctx, uint32_t port_id,
                                 void **buflist, uint32_t buflist_len, uint32_t flags)
{
    int32_t ret = 0;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->return_bufs)
        ret = dev_ctx->dcmode->dcmode_ops->return_bufs(dev_ctx->priv, port_id, buflist, buflist_len, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_add_session_shortcut_forward(struct dc_dp_priv_dev_info *dev_ctx, struct dp_subif *subif, struct sk_buff *skb, uint32_t flags)
{
    int32_t ret = 0;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->add_session_shortcut_forward)
        ret = dev_ctx->dcmode->dcmode_ops->add_session_shortcut_forward(dev_ctx->priv, subif, skb, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_disconn_if(struct dc_dp_priv_dev_info *dev_ctx, struct net_device *netif, struct dp_subif *subif_id,
                 uint8_t mac_addr[MAX_ETH_ALEN], uint32_t flags)
{
    int32_t ret = 0;
    struct dc_dp_dcmode *dcmode = NULL;

    if (dev_ctx)
        dcmode = dev_ctx->dcmode;
    else if (g_dcmode_hw_ctx)
        dcmode = g_dcmode_hw_ctx;
    else {
        int32_t dcmode_idx;
        struct dc_dp_dcmode **dcmode_head;
        struct dc_dp_dcmode *dcmode_iter = NULL;

        /* Find available dcmode hw, if any */
        dcmode_head = dc_dp_get_dcmode_head();

        for (dcmode_idx = 0; dcmode_idx < DC_DP_DCMODE_MAX; dcmode_idx++) {
            dcmode_iter = dcmode_head[dcmode_idx];
            if (dcmode_iter && (dcmode_iter->dcmode_cap & DC_DP_F_DCMODE_HW)) {
                g_dcmode_hw_ctx = dcmode_iter;
                dcmode = g_dcmode_hw_ctx;
                break;
            }
        }
    }

#if defined(CONFIG_PPA_XRX330) && CONFIG_PPA_XRX330
    if (dev_ctx && subif_id && is_litepath_subif_registered(dev_ctx, subif_id))
        ret = dc_dp_litepath_disconn_if(dev_ctx, netif, subif_id, mac_addr, flags);
#endif /* defined(CONFIG_PPA_XRX330) && CONFIG_PPA_XRX330 */

    if (dcmode && dcmode->dcmode_ops && dcmode->dcmode_ops->disconn_if) {
        ret = dcmode->dcmode_ops->disconn_if((dev_ctx ? dev_ctx->priv : NULL),
                  netif, subif_id, mac_addr, flags);
    }

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_get_netif_stats(struct dc_dp_priv_dev_info *dev_ctx,
                          struct net_device *netif, struct dp_subif *subif_id,
                          struct rtnl_link_stats64 *if_stats, uint32_t flags)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->get_netif_stats)
        ret = dev_ctx->dcmode->dcmode_ops->get_netif_stats(dev_ctx->priv, netif, subif_id, if_stats, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_clear_netif_stats(struct dc_dp_priv_dev_info *dev_ctx,
                            struct net_device *netif, struct dp_subif *subif_id,
                            uint32_t flags)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->clear_netif_stats)
        ret = dev_ctx->dcmode->dcmode_ops->clear_netif_stats(dev_ctx->priv, netif, subif_id, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_get_property(struct dc_dp_priv_dev_info *dev_ctx,
                            struct net_device *netif, struct dp_subif *subif_id,
                            struct dc_dp_prop *property)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->get_property)
        ret = dev_ctx->dcmode->dcmode_ops->get_property(dev_ctx->priv, netif, subif_id, property);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_set_property(struct dc_dp_priv_dev_info *dev_ctx,
                            struct net_device *netif, struct dp_subif *subif_id,
                            struct dc_dp_prop *property)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->set_property)
        ret = dev_ctx->dcmode->dcmode_ops->set_property(dev_ctx->priv, netif, subif_id, property);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_register_qos_class2prio_cb(struct dc_dp_priv_dev_info *dev_ctx, int32_t port_id, struct net_device *netif,
                                     int (*cb)(int32_t port_id, struct net_device *netif, uint8_t class2prio[]),
                                     int32_t flags)
{
    int32_t ret = DC_DP_SUCCESS;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->register_qos_class2prio_cb)
        ret = dev_ctx->dcmode->dcmode_ops->register_qos_class2prio_cb(dev_ctx->priv, port_id, netif, cb, flags);

    /* FIXME : Required only for DC Fastpath (DEVQoS). Currently lower GRX350 DC mode handles this. Later it should come in GRX750 as well */

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_map_class2devqos(struct dc_dp_priv_dev_info *dev_ctx,
                           int32_t port_id, struct net_device *netif, uint8_t class2prio[], uint8_t prio2devqos[])
{
    int32_t ret = DC_DP_SUCCESS;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->map_class2devqos)
        ret = dev_ctx->dcmode->dcmode_ops->map_class2devqos(dev_ctx->priv, port_id, netif, class2prio, prio2devqos);

    return ret;
}

struct sk_buff *
dc_dp_dcmode_wrapper_alloc_skb(struct dc_dp_priv_dev_info *dev_ctx, uint32_t len, struct dp_subif *subif, uint32_t flags)
{
    struct sk_buff *skb = NULL;

    if (!dev_ctx)
        goto err_out;

    if (is_litepath_subif_registered(dev_ctx, subif)) {
        skb = dc_dp_litepath_alloc_skb(dev_ctx, len, subif, flags);
    } else
        if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->alloc_skb)
            skb = dev_ctx->dcmode->dcmode_ops->alloc_skb(dev_ctx->priv, len, subif, flags);

    if (!skb)
        skb = dev_alloc_skb(len);

err_out:
    return skb;
}

int32_t
dc_dp_dcmode_wrapper_free_skb(struct dc_dp_priv_dev_info *dev_ctx, struct dp_subif *subif, struct sk_buff *skb)
{
    int32_t ret = -1;

    if (!dev_ctx)
        goto err_out;

    if (is_litepath_subif_registered(dev_ctx, subif)) {
        ret = dc_dp_litepath_free_skb(dev_ctx, subif, skb);
    } else
        if (dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->free_skb)
            ret = dev_ctx->dcmode->dcmode_ops->free_skb(dev_ctx->priv, subif, skb);

    if (ret) {
        dev_kfree_skb(skb);
        ret = 0;
    }

err_out:
    return ret;
}

int32_t
dc_dp_dcmode_wrapper_change_dev_status(struct dc_dp_priv_dev_info *dev_ctx, int32_t port_id, uint32_t flags)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->change_dev_status)
        ret = dev_ctx->dcmode->dcmode_ops->change_dev_status(dev_ctx->priv, port_id, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_get_wol_cfg(struct dc_dp_priv_dev_info *dev_ctx, int32_t port_id, struct dc_dp_wol_cfg *cfg, uint32_t flags)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->get_wol_cfg)
        ret = dev_ctx->dcmode->dcmode_ops->get_wol_cfg(dev_ctx->priv, port_id, cfg, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_set_wol_cfg(struct dc_dp_priv_dev_info *dev_ctx, int32_t port_id, struct dc_dp_wol_cfg *cfg, uint32_t flags)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->set_wol_cfg)
        ret = dev_ctx->dcmode->dcmode_ops->set_wol_cfg(dev_ctx->priv, port_id, cfg, flags);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_set_wol_ctrl(struct dc_dp_priv_dev_info *dev_ctx, int32_t port_id, uint32_t enable)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->set_wol_ctrl)
        ret = dev_ctx->dcmode->dcmode_ops->set_wol_ctrl(dev_ctx->priv, port_id, enable);

    return ret;
}

int32_t
dc_dp_dcmode_wrapper_get_wol_ctrl_status(struct dc_dp_priv_dev_info *dev_ctx, int32_t port_id)
{
    int32_t ret = -1;

    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->get_wol_ctrl_status)
        ret = dev_ctx->dcmode->dcmode_ops->get_wol_ctrl_status(dev_ctx->priv, port_id);

    return ret;
}

void
dc_dp_dcmode_wrapper_dump_proc(struct dc_dp_priv_dev_info *dev_ctx, struct seq_file *seq)
{
    if (dev_ctx && dev_ctx->dcmode && dev_ctx->dcmode->dcmode_ops && dev_ctx->dcmode->dcmode_ops->dump_proc)
        dev_ctx->dcmode->dcmode_ops->dump_proc(dev_ctx->priv, seq);

    return;
}

