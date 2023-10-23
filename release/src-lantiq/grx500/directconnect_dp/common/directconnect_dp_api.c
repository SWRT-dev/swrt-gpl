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

/* Includes */
#include <linux/module.h>
#include <linux/netdevice.h>
#include <linux/skbuff.h>
#if IS_ENABLED(CONFIG_MCAST_HELPER)
#include <net/mcast_helper_api.h>
#endif
#include <linux/firmware.h>
#if IS_ENABLED(CONFIG_MXL_FW_DOWNLOAD)
#include <soc/mxl/mxl_sec.h>
#endif

//#include <net/directconnect_dp_api.h>
#include <directconnect_dp_api.h>
#include <directconnect_dp_dcmode_api.h>
#include <directconnect_dp_debug.h>
#include "directconnect_dp_proc.h"

#include "directconnect_dp_device.h"
#include "directconnect_dp_dcmode_wrapper.h"
#include "directconnect_dp_litepath.h"
#include "directconnect_dp_macdb.h"
#include "directconnect_dp_parser.h"

/* Defines */
#define DC_DP_DRV_MODULE_NAME    "directconnect_datapath"
#define DC_DP_DRV_MODULE_VERSION "2.0.2"

#define DC_DP_MAX_DESC_DW   4

/* Local variables */
static int32_t g_dc_dp_init_ok = 0;

/* Function prototypes */

/* Local */
static inline uint8_t
_dc_dp_get_class2devqos(struct dc_dp_priv_dev_info *p_devinfo, uint8_t class, bool is_class);
#if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG
static void
_dc_dp_dump_raw_data(char *buf, int len, char *prefix_str);
#endif /* #if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG */

/*
 * ========================================================================
 * Local Interface API
 * ========================================================================
 */

static inline uint8_t
_dc_dp_get_class2devqos(struct dc_dp_priv_dev_info *p_devinfo, uint8_t class, bool is_class)
{
    uint8_t devqos;
    uint8_t prio;

    if (is_class) {
        class = (class & 0x0F);
        prio = p_devinfo->class2prio[class];
    } else {
        prio = class;
    }

    prio = (prio & DC_DP_DEV_CLASS_MASK);
    devqos = p_devinfo->prio2qos[prio];

    return devqos;
}

#if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG
static void
_dc_dp_dump_raw_data(char *buf, int len, char *prefix_str)
{
    int i;
    int r;
    int line_num = 32;
    unsigned char *p = (unsigned char *)buf;

    if (!p) {
        pr_err("[%s:%d] p NULL ?\n", __func__, __LINE__);
        return ;
    }

    printk("%s in hex (len = %d) at 0x%p\n", prefix_str ? (char *)prefix_str : "Data", len, p);
    for (i = 0; i < len; i++) {
        r = i % line_num;
        if (r== 0)
            printk(" %04d: ", i);
        else if (r == (line_num/2))
            printk(" "); /*inser seperator*/
        printk("%02x ", p[i]);
        if (r == (line_num - 1))
            printk("\n"); /*insert new line */
    }
    printk("\n");
}
#endif /* #if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG */

/*
 * ========================================================================
 * Direct Connect Driver Common Interface APIs
 * ========================================================================
 */

int32_t
dc_dp_get_peripheral_config(enum dc_dp_dev_type dev_id, struct dc_dp_dev_config *dev_config)
{
    int32_t ret = -1;

    if (NULL == dev_config)
        goto out;

    memset(dev_config, 0, sizeof(struct dc_dp_dev_config));
    ret = dc_dp_dcmode_wrapper_get_peripheral_config(dev_id, dev_config);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_peripheral_config);

int32_t
dc_dp_get_host_capability(struct dc_dp_host_cap *cap, uint32_t flags)
{
    int32_t ret = -1;

    if (NULL == cap)
        goto out;

    memset(cap, 0, sizeof(struct dc_dp_host_cap));
    ret = dc_dp_dcmode_wrapper_get_host_capability(cap, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_host_capability);

int32_t
dc_dp_alloc_port(struct module *owner, uint32_t dev_port,
                 struct net_device *dev, int32_t port_id, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    int32_t ep_id;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "owner=0x%pK, dev_port=%u, dev=0x%pK, "
        "port_id=%d, flags=0x%08x\n", owner, dev_port, dev, port_id, flags);

    /* Validate input arguments */
    if (!owner) {
        DC_DP_ERROR("owner is NULL!\n");
        goto err_out;
    }

    /* De-register */
    if (flags & DC_DP_F_DEREGISTER) {
        /* Find the registered device */
        ret = dc_dp_get_device_by_module_port(owner, port_id, &p_devinfo);
        if (ret || !p_devinfo) {
            DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
            goto err_out;
        }

        if (p_devinfo->flags != DC_DP_DEV_PORT_ALLOCATED) {
            DC_DP_ERROR("port:%d context is in unknown state:%d!\n",
                port_id, p_devinfo->flags);
            ret = DC_DP_FAILURE;
            goto err_out;
        }

        /* De-allocate port */
        ret = dc_dp_dcmode_wrapper_dealloc_port(p_devinfo, owner, dev_port, dev, port_id, flags);
        if (ret != DC_DP_SUCCESS) {
            DC_DP_ERROR("failed to de-allocate port_id=%d from datapath library/core!!!\n", port_id);
            goto err_out;
        }

        /* Clear device specific private data structure */
        dc_dp_free_device(port_id, dev);

        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned status=%d\n", DC_DP_SUCCESS);
        return DC_DP_SUCCESS;
    }

    /* Allocate a port */
    ret = dc_dp_dcmode_wrapper_alloc_port(NULL, owner, dev_port, dev, port_id, flags);
    if (ret < 0) {
        DC_DP_ERROR("failed to allocate port!!!\n");
        goto err_out;
    }
    ep_id = ret;

    /* Allocate a new device context */
    ret = dc_dp_alloc_device(ep_id, dev, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("failed to allocate a device for port_id=%d!!!\n", ep_id);
        goto err_dealloc_port;
    }

    /* Initialize device specific private data structure */
    p_devinfo->owner = owner;
    p_devinfo->dev = dev;
    p_devinfo->dev_port = dev_port;
    p_devinfo->port_id = ep_id;
    if ( (flags & DC_DP_F_MULTI_PORT) && (flags & DC_DP_F_SHARED_RES) )
        p_devinfo->ref_port_id = port_id;
    else
        p_devinfo->ref_port_id = ep_id;
    p_devinfo->alloc_flags = flags;
    p_devinfo->flags = DC_DP_DEV_PORT_ALLOCATED;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned port_id=%d\n", ep_id);
    return ep_id;

err_dealloc_port:
    dc_dp_dcmode_wrapper_dealloc_port(NULL, owner, dev_port, dev, ep_id, DC_DP_F_DEREGISTER);

err_out:
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Failure, returned status=%d!\n", ret);
    return ret;
}
EXPORT_SYMBOL(dc_dp_alloc_port);

int32_t
dc_dp_register_dev(struct module *owner, uint32_t port_id,
                   struct net_device *dev, struct dc_dp_cb *datapathcb,
                   struct dc_dp_res *resources, struct dc_dp_dev *devspec, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    int32_t class_id;
    int32_t subif_idx;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "owner=0x%pK, port_id=%u, dev=0x%pK, "
        "datapathcb=0x%pK, resources=0x%pK, devspec=0x%pK, flags=0x%08x\n",
        owner, port_id, dev, datapathcb, resources, devspec, flags);

    /* Validate input parameter */
    if (!owner || !devspec) {
        DC_DP_ERROR("owner or devspec is NULL!\n");
        goto err_out;
    }

    /* Find the device index */
    ret = dc_dp_get_device_by_module_port(owner, port_id, &p_devinfo);
    if (ret || !p_devinfo) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto err_out;
    }

    /* De-register */
    if (flags & DC_DP_F_DEREGISTER) {
        if (p_devinfo->flags < DC_DP_DEV_REGISTERED) {
            DC_DP_ERROR("port:%d context is in unknown state:%d!\n",
                port_id, p_devinfo->flags);
            ret = DC_DP_FAILURE;
            goto err_out;
        }

        if (p_devinfo->num_subif > 0) {
            /* De-register subif from Datapath Library/Core, if any */
            struct dp_subif *subif_id = kmalloc(sizeof(struct dp_subif), GFP_ATOMIC);
            if (!subif_id)
                DC_DP_ERROR("failed to allocate subif_id!!!\n");
            for (subif_idx = 0; (subif_idx < DC_DP_MAX_SUBIF_PER_DEV) && subif_id; subif_idx++) {
                if (p_devinfo->subif_info[subif_idx].flags == DC_DP_SUBIF_REGISTERED) {
                    subif_id->port_id = port_id;
                    subif_id->subif = p_devinfo->subif_info[subif_idx].subif;

                    /* Delete all the accelerated session (if any) */
                    dc_dp_disconn_if(p_devinfo->subif_info[subif_idx].netif, subif_id, NULL, 0);

                    /* De-register subif from Litepath */
                    if (is_litepath_subif_registered(p_devinfo, subif_id)) {
                        ret = dc_dp_dcmode_wrapper_deregister_subif(p_devinfo,
                                  owner, p_devinfo->subif_info[subif_idx].netif,
                                  (const uint8_t *)p_devinfo->subif_info[subif_idx].device_name,
                                  subif_id, DC_DP_F_DEREGISTER_LITEPATH);
                        if (ret)
                            DC_DP_ERROR("failed to de-register subif=0x%x from litepath!!!\n", subif_id->subif);
                    }

                    /* De-register subif for Fastpath/Swpath */
                    if (p_devinfo->dcmode) {
                        ret = dc_dp_dcmode_wrapper_deregister_subif(p_devinfo,
                                  owner, p_devinfo->subif_info[subif_idx].netif,
                                  (const uint8_t *)p_devinfo->subif_info[subif_idx].device_name,
                                  subif_id, DC_DP_F_DEREGISTER);
                        if (ret)
                            DC_DP_ERROR("failed to de-register subif=0x%x from swpath/fastpath!!!\n", subif_id->subif);
                    }

                    memset(&p_devinfo->subif_info[subif_idx], 0, sizeof(struct dc_dp_priv_subif_info));
                    p_devinfo->num_subif--;
                }
            }
            if (subif_id)
                kfree(subif_id);
        }

        /* For the last subif, change the device state */
        if (p_devinfo->num_subif == 0)
            p_devinfo->flags = DC_DP_DEV_REGISTERED;

        if ((flags & DC_DP_F_QOS)) {
            ret = dc_dp_dcmode_wrapper_register_qos_class2prio_cb(p_devinfo, port_id, dev, NULL, DC_DP_F_QOS_DEV_DREG);
            if (ret)
                DC_DP_ERROR("failed to de-register class2prio callback from PPA!!!\n");
        }

        /* De-register device */
        ret = dc_dp_dcmode_wrapper_deregister_dev(p_devinfo, owner, port_id, dev, datapathcb, resources, devspec, flags);
        if (ret != DC_DP_SUCCESS) {
            DC_DP_ERROR("failed to de-register dev from SoC!!!\n");
            goto err_out;
        }

        p_devinfo->flags = DC_DP_DEV_PORT_ALLOCATED;

        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned status=%d.\n", DC_DP_SUCCESS);
        return DC_DP_SUCCESS;
    }

    /* Validate input arguments */
    if (!datapathcb || !resources) {
        DC_DP_ERROR("datapathcb or resources is NULL!\n");
        ret = DC_DP_FAILURE;
        goto err_out;
    }

    if (p_devinfo->flags != DC_DP_DEV_PORT_ALLOCATED) {
        DC_DP_ERROR("port:%d context is in unknown state:%d!\n",
            port_id, p_devinfo->flags);
        ret = DC_DP_FAILURE;
        goto err_out;
    }

    /* Register device */
    ret = dc_dp_dcmode_wrapper_register_dev(p_devinfo, owner, port_id, dev, datapathcb, resources, devspec, flags);
    if (ret != DC_DP_SUCCESS) {
        DC_DP_ERROR("failed to register dev!!!\n");
        goto err_out;
    }

    if ((flags & DC_DP_F_QOS)) {
        ret = dc_dp_dcmode_wrapper_register_qos_class2prio_cb(p_devinfo, port_id, dev, dc_dp_qos_class2prio, DC_DP_F_QOS_DEV_REG);
        if (ret) {
            DC_DP_ERROR("failed to register class2prio callback!!!\n");
            /* FIXME : in this case, should we use skb->priority? */
            ret = 0;
        }
    } else {
        /* FIXME : I think, it has no means? */
        for (class_id = 0; class_id < DC_DP_MAX_SOC_CLASS; class_id++)
            p_devinfo->class2prio[class_id] = (class_id >> 1);
    }

    /* Keep the callback registered by peripheral driver */
    p_devinfo->cb = *datapathcb;
    p_devinfo->dc_accel_used = devspec->dc_accel_used;
    p_devinfo->dc_tx_ring_used = devspec->dc_tx_ring_used;
    p_devinfo->dc_rx_ring_used = devspec->dc_rx_ring_used;
    p_devinfo->flags = DC_DP_DEV_REGISTERED;

err_out:
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Returned %d.\n", ret);
    return ret;
}
EXPORT_SYMBOL(dc_dp_register_dev);

int32_t
dc_dp_register_subif(struct module *owner, struct net_device *dev,
                     const uint8_t *subif_name, struct dp_subif *subif_id, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    int32_t subif_idx;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "owner=%p, dev=%p, subif_id=%p, flags=0x%08X\n",
                    owner, dev, subif_id, flags);

    /* Validate input arguments */
    if (!owner) {
        DC_DP_ERROR("owner is NULL!\n");
        goto err_out;
    }

    if ((!dev && !subif_name) || !subif_id) {
        DC_DP_ERROR("(dev and subif_name are NULL) or subif_id is NULL!\n");
        goto err_out;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_module_port(owner, subif_id->port_id, &p_devinfo);
    if (ret || !p_devinfo) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
        goto err_out;
    }

    /* De-register */
    if ( ( flags & (DC_DP_F_DEREGISTER | DC_DP_F_DEREGISTER_LITEPATH)) ) {

        if (p_devinfo->flags != DC_DP_DEV_SUBIF_REGISTERED) {
            DC_DP_ERROR("port:%d context is in unknown state:%d!\n",
                subif_id->port_id, p_devinfo->flags);
            ret = DC_DP_FAILURE;
            goto err_out;
        }

        /* Find the registered subif index (for multiple vap) */
        subif_idx = ((subif_id->subif >> DC_DP_SUBIFID_OFFSET) & DC_DP_SUBIFID_MASK);
        if (p_devinfo->subif_info[subif_idx].flags != DC_DP_SUBIF_REGISTERED) {
            DC_DP_ERROR("subif:%d context is in unknown state:%d!\n",
                subif_idx, p_devinfo->subif_info[subif_idx].flags);
            ret = DC_DP_FAILURE;
            goto err_out;
        }

        /* Delete all the accelerated session (if any) */
        dc_dp_disconn_if(dev, subif_id, NULL, 0);

        /* De-register subif */
        ret = dc_dp_dcmode_wrapper_deregister_subif(p_devinfo, owner, dev, (dev ? (const uint8_t *)dev->name : subif_name), subif_id, flags);
        if (ret != DC_DP_SUCCESS) {
            DC_DP_ERROR("failed to de-register subif from SoC - (port_id=%d, subif=0x%4x)!!!\n",
                         subif_id->port_id, subif_id->subif);
            goto err_out;
        }

        /* Reset private structure corresponding to the subif */
        if (((flags & DC_DP_F_DEREGISTER_LITEPATH) && !p_devinfo->dcmode) ||
            (!(flags & DC_DP_F_DEREGISTER_LITEPATH) && p_devinfo->dcmode))
            dc_dp_free_subif(&p_devinfo->subif_info[subif_idx]);

        p_devinfo->num_subif--;

        /* For the last subif, change the device state */
        if (p_devinfo->num_subif == 0)
            p_devinfo->flags = DC_DP_DEV_REGISTERED;

        DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned %d.\n", DC_DP_SUCCESS);
        return DC_DP_SUCCESS;
    }

    if (p_devinfo->flags < DC_DP_DEV_REGISTERED) {
        DC_DP_ERROR("port:%d context is in unknown state:%d!\n",
            subif_id->port_id, p_devinfo->flags);
        ret = DC_DP_FAILURE;
        goto err_out;
    }

#if 0
    if ((p_devinfo->num_subif + 1) > DC_DP_MAX_SUBIF_PER_DEV) {
        DC_DP_ERROR("failed to register subif as it reaches maximum subif limit (%d)!!!\n", DC_DP_MAX_SUBIF_PER_DEV);
        ret = DC_DP_FAILURE;
        goto err_out;
    }
#endif

    /* Register subif */
    ret = dc_dp_dcmode_wrapper_register_subif(p_devinfo, owner, dev, (dev ? (const uint8_t *)dev->name : subif_name), subif_id, flags);
    if (ret != DC_DP_SUCCESS) {
        DC_DP_ERROR("failed to register subif to Datapath Library/Core!!!\n");
        goto err_out;
    }

    /* Initialize private structure with device and subif information */
    subif_idx = ((subif_id->subif >> DC_DP_SUBIFID_OFFSET) & DC_DP_SUBIFID_MASK);
    p_devinfo->subif_info[subif_idx].flags = DC_DP_SUBIF_REGISTERED;
    p_devinfo->subif_info[subif_idx].netif = dev;
    p_devinfo->subif_info[subif_idx].subif = subif_id->subif;
    strcpy(p_devinfo->subif_info[subif_idx].device_name, (dev ? dev->name : (char *)subif_name));

    p_devinfo->flags = DC_DP_DEV_SUBIF_REGISTERED;
    p_devinfo->num_subif++;

    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Success, returned %d.\n", ret);
    return ret;

err_out:
    DC_DP_DEBUG(DC_DP_DBG_FLAG_DBG, "Failure, returned %d.\n", ret);
    return ret;
}
EXPORT_SYMBOL(dc_dp_register_subif);

void
dc_dp_set_subif_param(struct dc_dp_subif *dc_subif)
{
    int32_t ret;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!dc_subif) {
        DC_DP_ERROR("dc_subif is NULL!\n");
        return;
    }

    ret = dc_dp_get_device_by_port(dc_subif->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", dc_subif->port_id);
        return;
    }

    /* Encode subif */
    dc_dp_dcmode_wrapper_set_subif_param(p_devinfo, dc_subif);
}
EXPORT_SYMBOL(dc_dp_set_subif_param);

void
dc_dp_get_subif_param(struct dc_dp_subif *dc_subif)
{
    int32_t ret;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!dc_subif) {
        DC_DP_ERROR("dc_subif is NULL!\n");
        return;
    }

    ret = dc_dp_get_device_by_port(dc_subif->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", dc_subif->port_id);
        return;
    }

    /* Decode subif */
    dc_dp_dcmode_wrapper_get_subif_param(p_devinfo, dc_subif);
}
EXPORT_SYMBOL(dc_dp_get_subif_param);

int32_t
dc_dp_xmit(struct net_device *rx_if, struct dp_subif *rx_subif, struct dp_subif *tx_subif,
           struct sk_buff *skb, int32_t len, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!skb) {
        DC_DP_ERROR("skb is NULL!\n");
        goto out;
    }

    if (!tx_subif) {
        DC_DP_ERROR("tx_subif is NULL!\n");
        goto drop;
    }

    ret = dc_dp_get_device_by_port(tx_subif->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", tx_subif->port_id);
        goto drop;
    }

#if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG
    if (dc_dp_get_dbg_flag() & DC_DP_DBG_FLAG_DUMP_TX_DATA)
        _dc_dp_dump_raw_data(skb->data, skb->len, "Xmit Data");
#endif /* #if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG */

    /* Transmit */
    ret = dc_dp_dcmode_wrapper_xmit(p_devinfo, rx_if, rx_subif, tx_subif, skb, skb->len, flags);

    return ret;

drop:
    dev_kfree_skb_any(skb);
    /* FIXME : update Tx MIB */

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_xmit);

int32_t
dc_dp_handle_ring_sw(struct module *owner, uint32_t port_id,
                     struct net_device *dev, struct dc_dp_ring *ring, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    /* Validate input parameter */
    if (!owner || !ring) {
        DC_DP_ERROR("owner or ring is NULL!\n");
        goto out;
    }

    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_handle_ring_sw(p_devinfo, owner, port_id, dev, ring, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_handle_ring_sw);

int32_t
dc_dp_return_bufs(uint32_t port_id, void **buflist,
                  uint32_t buflist_len, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_return_bufs(p_devinfo, port_id, buflist, buflist_len, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_return_bufs);

struct sk_buff *
dc_dp_alloc_skb(uint32_t len, struct dp_subif *subif, uint32_t flags)
{
    int32_t ret;
    struct sk_buff *skb = NULL;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!subif || len == 0)
        goto out;

    ret = dc_dp_get_device_by_port(subif->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif->port_id);
        goto out;
    }

    skb = dc_dp_dcmode_wrapper_alloc_skb(p_devinfo, len, subif, flags);

out:
    return skb;
}
EXPORT_SYMBOL(dc_dp_alloc_skb);

int32_t
dc_dp_free_skb(struct dp_subif *subif, struct sk_buff *skb)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!subif || !skb)
        goto out;

    ret = dc_dp_get_device_by_port(subif->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif->port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_free_skb(p_devinfo, subif, skb);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_free_skb);

int32_t
dc_dp_alloc_mem(struct pci_dev *pcidev, uint32_t buf_len, void *buf,
                enum dc_dp_acc_type access, unsigned long attrs)
{
    /* TODO: Currently not supported */
    return DC_DP_FAILURE;
}
EXPORT_SYMBOL(dc_dp_alloc_mem);

void
dc_dp_free_mem(struct pci_dev *pcidev, uint32_t buf_len, void *buf)
{
    /* TODO: Currently not supported */
}
EXPORT_SYMBOL(dc_dp_free_mem);

int32_t
dc_dp_add_session_shortcut_forward(struct dp_subif *subif, struct sk_buff *skb, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!subif || !skb)
        goto out;

    ret = dc_dp_get_device_by_port(subif->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif->port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_add_session_shortcut_forward(p_devinfo, subif, skb, flags);

#if defined(CONFIG_NETWORK_EXTMARK) && !defined(CONFIG_X86_INTEL_LGM)
    if ( (flags & DC_DP_F_PREFORWARDING) ) {
        /* Set skb->extmark, to be derived from skb->priority */
        SET_DATA_FROM_MARK_OPT(skb->extmark, QUEPRIO_MASK, QUEPRIO_START_BIT_POS, (skb->priority + 1));
    }
#endif /* #ifdef CONFIG_NETWORK_EXTMARK */

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_add_session_shortcut_forward);

int32_t
dc_dp_disconn_if(struct net_device *netif, struct dp_subif *subif_id,
                 uint8_t mac_addr[MAX_ETH_ALEN], uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;
    struct dp_subif *subifid = NULL;
    int32_t subif_idx;
    uint8_t hw_addr[MAX_ADDR_LEN] = {0};

    DC_DP_DEBUG((DC_DP_DBG_FLAG_DUMP_TX | DC_DP_DBG_FLAG_DUMP_RX),
        "netif=0x%pK, subif_id=0x%pK, mac_addr=%pM, flags=0x%08x\n",
        netif, subif_id, mac_addr, flags);

    if (!netif && !subif_id) {
        DC_DP_ERROR("Wrong combination - netif=%p, subif_id=%p, mac_addr=%pM\n",
                    netif, subif_id, mac_addr);
        goto out;
    }

    if (subif_id) {
        ret = dc_dp_get_device_by_port(subif_id->port_id, &p_devinfo);
        if (ret) {
            DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
            goto out;
        }
        //FIXME : if (!netif)
    } else if (netif) {
        /* FIXME : not efficient, better to include port_id and optionally subifid??? */
        subif_idx = dc_dp_get_device_by_subif_netif(netif, &p_devinfo);
        if ((subif_idx < 0) || !p_devinfo) {
            DC_DP_ERROR("netif=%s does not exist!!!\n", (netif ? netif->name : "NULL"));
            goto out;
        }
        subifid = kmalloc(sizeof(struct dp_subif), GFP_ATOMIC);
        if (!subifid) {
            DC_DP_ERROR("failed to allocate subifid!!!\n");
            goto out;
        }
        subifid->port_id = p_devinfo->port_id;
        subifid->subif = p_devinfo->subif_info[subif_idx].subif;
        subif_id = subifid;
    }

    /* Remove the Subif/MAC from acceleration table */
    ret = dc_dp_dcmode_wrapper_disconn_if(p_devinfo, netif, subif_id, mac_addr, flags);

out:
    /* Remove the MAC from Linux bridge forwarding table */
    if (netif) {
#if IS_ENABLED(CONFIG_BRIDGE)
        if ((netif->priv_flags & IFF_BRIDGE_PORT))
            dc_dp_br_fdb_delete(netif, mac_addr);
#endif /* #if IS_ENABLED(CONFIG_BRIDGE) */
#if IS_ENABLED(CONFIG_MCAST_HELPER)
        if (mac_addr) {
            memcpy(hw_addr, mac_addr, MAX_ETH_ALEN);
            mcast_helper_register_module(netif, (p_devinfo? p_devinfo->owner : NULL),
                                         NULL, NULL, (void *)&hw_addr[0], MCH_F_DISCONN_MAC);
        }
#endif /* CONFIG_MCAST_HELPER */
    }

    if (subifid)
        kfree(subifid);

    DC_DP_DEBUG((DC_DP_DBG_FLAG_DUMP_TX|DC_DP_DBG_FLAG_DUMP_RX),
                    "Exit, returned=%d.\n", ret);
    return ret;
}
EXPORT_SYMBOL(dc_dp_disconn_if);

/*
 * ========================================================================
 * API/Callbacks used by SoC specific
 * ========================================================================
 */
int32_t
dc_dp_rx(struct net_device *rxif, struct net_device *txif, struct dp_subif *subif_id,
         struct sk_buff *skb, int32_t len, uint32_t flags)
{
    int32_t ret;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;
    dc_dp_rx_fn_t rx_cb;
    dc_dp_get_netif_subinterface_fn_t get_subif_cb;
    struct sk_buff **pskb = NULL;
    struct sk_buff *tx_skb = NULL;
    struct dp_subif *rx_subif = NULL;
    //struct dp_subif tx_subif = {0};
    //uint32_t pkt_subif_id;
    int32_t subif_idx;

    if (!skb) {
        DC_DP_ERROR("skb is NULL!\n");
        ret = -1;
        goto err_out;
    }
    pskb = &skb;

#if defined(CONFIG_DIRECTCONNECT_DP_DBG) && CONFIG_DIRECTCONNECT_DP_DBG
    if (dc_dp_get_dbg_flag() & DC_DP_DBG_FLAG_DUMP_RX_DATA)
        _dc_dp_dump_raw_data(skb->data, skb->len, "Recv Data");
#endif /* CONFIG_DIRECTCONNECT_DP_DBG */

    if (!subif_id) {
        if (!rxif && !txif) {
            DC_DP_ERROR("rxif and txif are NULL!\n");
            ret = -1;
            goto drop;
        }

        subif_idx = dc_dp_get_device_by_subif_netif((rxif ? rxif : txif), &p_devinfo);
        if ((subif_idx < 0) || !p_devinfo) {
            DC_DP_ERROR("device (%s) does not exist!!!\n",
                            (txif ? txif->name: rxif->name));
            ret = -1;
            goto drop;
        }
        rx_subif = kmalloc(sizeof(struct dp_subif), GFP_ATOMIC);
        if (!rx_subif) {
            DC_DP_ERROR("failed to allocate rx_subif!!!\n");
            ret = -1;
            goto drop;
        }
        rx_subif->port_id = p_devinfo->port_id;
        rx_subif->subif = p_devinfo->subif_info[subif_idx].subif;
        subif_id = rx_subif;
    } else {
        ret = dc_dp_get_device_by_port(subif_id->port_id, &p_devinfo);
        if (ret) {
            DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
            ret = -1;
            goto drop;
        }
    }
    //tx_subif = subif;
    rx_cb = p_devinfo->cb.rx_fn;
    get_subif_cb = p_devinfo->cb.get_subif_fn;

    /*
     * rx_if!= NULL : DC or SW or LP
     * tx_if!=NULL : LP
     */
    if (rx_cb) {
        if (dc_dp_pad_is_available(skb))
            dc_dp_parser_remove_pad(skb);

        ret = rx_cb(rxif, txif, subif_id, pskb, skb->len, flags);
    } else {
        DC_DP_ERROR("rx_fn callback is not registered on rxif->name=%s!!!\n",
                        (rxif ? rxif->name : "NA"));
        ret = -1;
        goto drop;
    }

    if (ret > 0 && *pskb != NULL) {
        tx_skb = *pskb;
#if 0
        if (txif) {
            /* Received from LITEPATH, so transmit to SWPATH; In any case, Lower layer should free the skb */
            if (dc_dp_get_mark_pkt(tx_subif.port_id, tx_skb, 0)) {
                dc_dp_get_subifid_pkt(tx_subif.port_id, tx_skb, &pkt_subif_id, 0);
                tx_subif.subif = pkt_subif_id;
            }
            ret = dc_dp_dcmode_wrapper_xmit(p_devinfo, rxif, &subif, &tx_subif, tx_skb, tx_skb->len, 0);
        } else if (rxif) {
            /* NOTE : Client Driver has to handle intra-subif traffic in FASTPATH or SWPATH */
            if (flags & DC_DP_F_RX_LITEPATH) {
                /* Received from LITEPATH, so transmit to SWPATH; In any case, Lower layer should free the skb */
                if (dc_dp_get_mark_pkt(tx_subif.port_id, tx_skb, 0)) {
                    dc_dp_get_subifid_pkt(tx_subif.port_id, tx_skb, &pkt_subif_id, 0);
                    tx_subif.subif = pkt_subif_id;
                }
                ret = dc_dp_dcmode_wrapper_xmit(p_devinfo, rxif, &subif, &tx_subif, tx_skb, tx_skb->len, 0);
            } else {
                /* Received from SWPATH, so transmit to LITEPATH; In any case, Lower layer should free the skb */
                ret = dc_dp_dcmode_wrapper_xmit(p_devinfo, rxif, &subif, &tx_subif, tx_skb, tx_skb->len, DC_DP_F_XMIT_LITEPATH);
            }
        } else
#endif
        {
            DC_DP_ERROR("Dropping packet (dev=%s), no shortcut transmit handling!!!\n",
                    ((*pskb)->dev ? (*pskb)->dev->name : NULL));
            ret = 0;
            dev_kfree_skb_any(tx_skb);
        }
    } else if (*pskb != NULL)
        DC_DP_ERROR("Client driver (dev=%s) should consume the skb. skb is freed???\n",
                    ((*pskb)->dev ? (*pskb)->dev->name : NULL));

    if (rx_subif)
        kfree(rx_subif);

    return ret;

drop:
    dev_kfree_skb_any(skb);
    if (rx_subif)
        kfree(rx_subif);

err_out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_rx);

int32_t
dc_dp_check_if_netif_fastpath(struct net_device *netif, char *ifname, uint32_t flags)
{
    struct dc_dp_priv_dev_info *p_devinfo = NULL;
    int32_t fastpath = 0;
    int32_t subif_idx = -1;

    if (!netif && !ifname) {
        DC_DP_ERROR("netif and ifname are NULL!\n");
        goto out;
    }

    /* FIXME : not efficient??? */
    if (netif)
        subif_idx = dc_dp_get_device_by_subif_netif(netif, &p_devinfo);
    else if (ifname)
        subif_idx = dc_dp_get_device_by_subif_ifname(ifname, &p_devinfo);

    if ((subif_idx >= 0) && p_devinfo && (p_devinfo->alloc_flags & DC_DP_F_FASTPATH))
        fastpath = 1;

    DC_DP_DEBUG((DC_DP_DBG_FLAG_DUMP_TX|DC_DP_DBG_FLAG_DUMP_RX),
        "ifname=%s, fastpath(directconnect)=%d.\n",
        (netif? netif->name : ifname), fastpath);

out:
    return fastpath;
}
EXPORT_SYMBOL(dc_dp_check_if_netif_fastpath);

int32_t
dc_dp_get_netif_subifid(struct net_device *netif, struct sk_buff *skb,
                        void *subif_data, uint8_t dst_mac[MAX_ETH_ALEN],
                        struct dp_subif *subif, uint32_t flags)
{
    int32_t ret = -1;
    dc_dp_get_netif_subinterface_fn_t get_subif_fn;
#if 0
    uint8_t macaddr[ETH_ALEN] = {0};
    uint8_t *mac_addr = NULL;
#endif
    struct dc_dp_priv_dev_info *p_devinfo = NULL;
    int32_t subifid_ret = 1;

    if (!subif) {
        DC_DP_ERROR("subif is NULL!\n");
        goto out;
    }

    ret = dc_dp_get_device_by_port(subif->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif->port_id);
        goto out;
    }

    get_subif_fn = p_devinfo->cb.get_subif_fn;

    /* Optionally, find meta-subif information for the MAC (e.g., WAVE500 Station ID) */
#if 0
    if (dst_mac) {
        memcpy(macaddr, dst_mac, ETH_ALEN);
        mac_addr = macaddr;
    } else if (skb) {
        memcpy(macaddr, eth_hdr(skb)->h_dest, ETH_ALEN);
        mac_addr = macaddr;
    }
#endif

    if (get_subif_fn) {
        ret = get_subif_fn(netif, skb, subif_data, dst_mac, subif, 0);
        if (ret)
            goto out;
    } else {
        DC_DP_ERROR("skip linear subif lookup for dev:%s!\n",
            (netif ? netif->name : "NA"));
        goto out;
    }

    subifid_ret = 0; /* according to SWAS, return 1 means subifid found */
    DC_DP_DEBUG((DC_DP_DBG_FLAG_DUMP_TX|DC_DP_DBG_FLAG_DUMP_RX),
        "subifid=0x%04x for netif->name=%s.\n", subif->subif, netif->name);

out:
    return subifid_ret;
}
EXPORT_SYMBOL(dc_dp_get_netif_subifid);

int32_t
dc_dp_qos_class2prio(int32_t port_id, struct net_device *netif, uint8_t *class2prio)
{
    int32_t ret;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!class2prio) {
        DC_DP_ERROR("class2prio is NULL!\n");
        return DC_DP_FAILURE;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        return DC_DP_FAILURE;
    }

    /* FIXME : Drop the request except for the device reg_flags=DC_DP_F_QOS */

    /* Update local QoS Class2Priority map table */
    memcpy(p_devinfo->class2prio, class2prio, sizeof(p_devinfo->class2prio));

    ret = dc_dp_dcmode_wrapper_map_class2devqos(p_devinfo, port_id, netif, p_devinfo->class2prio, p_devinfo->prio2qos);

    return ret;
}
EXPORT_SYMBOL(dc_dp_qos_class2prio);

int32_t
dc_dp_get_dev_specific_desc_info(int32_t port_id, struct sk_buff *skb,
                                 struct dc_dp_fields_value_dw *desc_fields, uint32_t flags)
{
    int32_t ret = -1;
    dc_dp_get_dev_specific_desc_info_t get_desc_info_fn;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    /* Validate input parameter */
    if ((port_id < 0) || !skb || !desc_fields) {
        DC_DP_ERROR("skb or desc_fields is NULL for port:%d!\n", port_id);
        goto out;
    }

    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    get_desc_info_fn = p_devinfo->cb.get_desc_info_fn;
    if (get_desc_info_fn)
        ret = get_desc_info_fn(port_id, skb, desc_fields, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_dev_specific_desc_info);

/*
 * ========================================================================
 * Misclleneous API
 * ========================================================================
 */
int32_t
dc_dp_register_mcast_module(struct net_device *dev, struct module *owner,
                            dc_dp_mcast_callback_fn_t cb, unsigned int flags)
{
#if IS_ENABLED(CONFIG_MCAST_HELPER)
    uint32_t mch_flags = 0;

    /* Validate input parameter */
    if (!owner || !dev) {
        DC_DP_ERROR("owner or dev is NULL!\n");
        return DC_DP_FAILURE;
    }

    if (flags & DC_DP_F_MC_REGISTER)
        mch_flags |= MCH_F_REGISTER;
    if (flags & DC_DP_F_MC_DEREGISTER)
        mch_flags |= MCH_F_DEREGISTER;
    if (flags & DC_DP_F_MC_UPDATE_MAC_ADDR)
        mch_flags |= MCH_F_UPDATE_MAC_ADDR;
    if (flags & DC_DP_F_MC_FW_RESET)
        mch_flags |= MCH_F_FW_RESET;
    if (flags & DC_DP_F_MC_NEW_STA)
        mch_flags |= MCH_F_NEW_STA;

    mcast_helper_register_module(dev, owner, NULL, (Mcast_module_callback_t *)cb, NULL, mch_flags);
    return DC_DP_SUCCESS;
#else
    return DC_DP_FAILURE;
#endif /* CONFIG_MCAST_HELPER */
}
EXPORT_SYMBOL(dc_dp_register_mcast_module);

int32_t
dc_dp_get_skb_gid(struct sk_buff *skb)
{
#if IS_ENABLED(CONFIG_MCAST_HELPER)
    return mcast_helper_get_skb_gid(skb);
#else
    return -1;
#endif /* CONFIG_MCAST_HELPER */
}
EXPORT_SYMBOL(dc_dp_get_skb_gid);

int32_t
dc_dp_map_prio_devqos_class(int32_t port_id, struct net_device *netif, uint8_t prio2qos[DC_DP_MAX_DEV_CLASS])
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!netif) {
        DC_DP_ERROR("netif is NULL!\n");
        goto out;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    /* Update local prio2qos map table */
    if (prio2qos)
        memcpy(p_devinfo->prio2qos, prio2qos, sizeof(p_devinfo->prio2qos));
    else
        memset(p_devinfo->prio2qos, 0, sizeof(p_devinfo->prio2qos));

    ret = dc_dp_dcmode_wrapper_map_class2devqos(p_devinfo, port_id, netif, p_devinfo->class2prio, p_devinfo->prio2qos);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_map_prio_devqos_class);

int32_t
dc_dp_mark_pkt_devqos(int32_t port_id, struct net_device *dst_netif, struct sk_buff *skb)
{
    int32_t ret;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;
#if !defined(CONFIG_X86_INTEL_LGM) && !defined(CONFIG_PRX300_CQM)
    struct dma_tx_desc_1 *desc_1 = NULL;
#endif /* #if !defined(CONFIG_X86_INTEL_LGM) */
    int32_t class_prio;
    uint32_t devqos;
    bool is_class = 1;

    /* Validate input parameter */
    /* dst_netif unused parameter */
    if ((port_id < 0) || !skb) {
        DC_DP_ERROR("skb is NULL for port:%d!\n", port_id);
        return DC_DP_FAILURE;
    }

#if !defined(CONFIG_X86_INTEL_LGM) && !defined(CONFIG_PRX300_CQM)
    desc_1 = (struct dma_tx_desc_1 *) &skb->DW1;
#endif /* #if !defined(CONFIG_X86_INTEL_LGM) */

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        return DC_DP_FAILURE;
    }

#if defined(CONFIG_NETWORK_EXTMARK) && !defined(CONFIG_X86_INTEL_LGM)
    /* Extract classid (aka, TC value) from skb->extmark */
    GET_DATA_FROM_MARK_OPT(skb->extmark, QUEPRIO_MASK, QUEPRIO_START_BIT_POS, class_prio);
    class_prio = class_prio ? (class_prio - 1) : 0;
#else /* #ifdef CONFIG_NETWORK_EXTMARK */
    class_prio = skb->priority;
    is_class = 1;
#endif /* #else */

    devqos = _dc_dp_get_class2devqos(p_devinfo, class_prio, is_class);
#if !defined(CONFIG_X86_INTEL_LGM) && !defined(CONFIG_PRX300_CQM)
    desc_1->field.resv1 = devqos;
#endif /* #if !defined(CONFIG_X86_INTEL_LGM) */

    if (devqos)
        dc_dp_dcmode_wrapper_mark_pkt_devqos(p_devinfo, devqos, skb);

    return devqos;
}
EXPORT_SYMBOL(dc_dp_mark_pkt_devqos);

int32_t
dc_dp_get_class2devqos(struct net_device *netif, struct dp_subif *subif_id,
    uint32_t class, uint8_t *devQoS)
{
    int32_t ret = DC_DP_SUCCESS;
    uint8_t devqos;
    bool is_class = 1;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (subif_id->port_id < 0) {
        DC_DP_ERROR("failed to get_class2devqos as port_id=[%d]\n",
            subif_id->port_id);
        return DC_DP_FAILURE;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(subif_id->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
        return DC_DP_FAILURE;
    }

    devqos = _dc_dp_get_class2devqos(p_devinfo, class, is_class);
    *devQoS = devqos;

    return ret;
}
EXPORT_SYMBOL(dc_dp_get_class2devqos);

int32_t
dc_dp_get_netif_stats(struct net_device *netif, struct dp_subif *subif_id,
                      struct rtnl_link_stats64 *if_stats, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!subif_id || !if_stats) {
        DC_DP_ERROR("subif_id or if_stats is NULL!\n");
        goto out;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(subif_id->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
        goto out;
    }

    memset(if_stats, 0, sizeof(struct rtnl_link_stats64));
    ret = dc_dp_dcmode_wrapper_get_netif_stats(p_devinfo, netif, subif_id, if_stats, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_netif_stats);

int32_t
dc_dp_clear_netif_stats(struct net_device *netif, struct dp_subif *subif_id, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!subif_id) {
        DC_DP_ERROR("subif_id is NULL!\n");
        goto out;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(subif_id->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_clear_netif_stats(p_devinfo, netif, subif_id, flags);

out:
   return ret;
}
EXPORT_SYMBOL(dc_dp_clear_netif_stats);

int32_t
dc_dp_get_property(struct net_device *netif,
                   struct dp_subif *subif_id, struct dc_dp_prop *property)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!subif_id) {
        DC_DP_ERROR("subif_id is NULL!\n");
        goto out;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(subif_id->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_get_property(p_devinfo, netif, subif_id, property);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_property);

int32_t
dc_dp_set_property(struct net_device *netif,
                   struct dp_subif *subif_id, struct dc_dp_prop *property)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!subif_id) {
        DC_DP_ERROR("subif_id is NULL!\n");
        goto out;
    }

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(subif_id->port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", subif_id->port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_set_property(p_devinfo, netif, subif_id, property);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_set_property);

int32_t
dc_dp_change_dev_status_in_soc(int32_t port_id, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_change_dev_status(p_devinfo, port_id, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_change_dev_status_in_soc);

int32_t
dc_dp_get_wol_cfg(int32_t port_id, struct dc_dp_wol_cfg *cfg, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_get_wol_cfg(p_devinfo, cfg, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_wol_cfg);

int32_t
dc_dp_set_wol_cfg(int32_t port_id, struct dc_dp_wol_cfg *cfg, uint32_t flags)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_set_wol_cfg(p_devinfo, cfg, flags);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_set_wol_cfg);

int32_t
dc_dp_set_wol_ctrl(int32_t port_id, uint32_t enable)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_set_wol_ctrl(p_devinfo, port_id, enable);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_set_wol_ctrl);

int32_t
dc_dp_get_wol_ctrl_status(int32_t port_id)
{
    int32_t ret;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    /* Find the registered device index */
    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret) {
        DC_DP_ERROR("port:%d is not allocated yet!\n", port_id);
        goto out;
    }

    ret = dc_dp_dcmode_wrapper_get_wol_ctrl_status(p_devinfo, port_id);

out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_wol_ctrl_status);

int
dc_dp_soc_request_firmware(
    const struct firmware **fw, char *name, struct device *device,
    size_t size, enum dc_dp_fw_opt_type opt)
{
#if IS_ENABLED(CONFIG_MXL_FW_DOWNLOAD)
    struct mxl_fw_dl_data data = {0};

    data.dev = device;
    data.sai = -1;
    data.opt = opt;

    return mxl_soc_request_firmware(fw, name, device, size, &data);
#else
    return request_firmware(fw, name, device);
#endif /* CONFIG_MXL_FW_DOWNLOAD */
}
EXPORT_SYMBOL(dc_dp_soc_request_firmware);

void
dc_dp_soc_release_firmware(
    const struct firmware *fw, struct device *device,
    const void *buf, size_t size,
    enum dc_dp_fw_opt_type opt)
{
#if IS_ENABLED(CONFIG_MXL_FW_DOWNLOAD)
    struct mxl_fw_dl_data data = {0};

    data.dev = device;
    data.sai = -1;
    data.opt = opt;

    mxl_soc_release_firmware(fw, device, buf, size, &data);
#else
    release_firmware(fw);
#endif /* CONFIG_MXL_FW_DOWNLOAD */
}
EXPORT_SYMBOL(dc_dp_soc_release_firmware);

int
dc_dp_soc_request_firmware_into_buf(
    const struct firmware **fw, char *name, struct device *device,
    void *buf, size_t size, enum dc_dp_fw_opt_type opt)
{
#if IS_ENABLED(CONFIG_MXL_FW_DOWNLOAD)
    struct mxl_fw_dl_data data = {0};

    data.dev = device;
    data.sai = -1;
    data.opt = opt;

    return mxl_soc_request_firmware_into_buf(fw, name, device,
                buf, size, &data);
#else
    return request_firmware_into_buf(fw, name, device, buf, size);
#endif /* CONFIG_MXL_FW_DOWNLOAD */
}
EXPORT_SYMBOL(dc_dp_soc_request_firmware_into_buf);

void *
dc_dp_soc_alloc_firmware_buf(struct device *device, size_t size,
    enum dc_dp_fw_opt_type opt)
{
#if IS_ENABLED(CONFIG_MXL_FW_DOWNLOAD)
    struct mxl_fw_dl_data data = {0};

    data.dev = device;
    data.sai = -1;
    data.opt = opt;

    return mxl_soc_alloc_firmware_buf(device, size, &data);
#else
    dma_addr_t dma_handle;

    return dma_alloc_coherent(device, size, &dma_handle, GFP_KERNEL);
#endif /* CONFIG_MXL_FW_DOWNLOAD */
}
EXPORT_SYMBOL(dc_dp_soc_alloc_firmware_buf);

void
dc_dp_soc_free_firmware_buf(struct device *device, const void *buf,
    size_t size, enum dc_dp_fw_opt_type opt)
{
#if IS_ENABLED(CONFIG_MXL_FW_DOWNLOAD)
    struct mxl_fw_dl_data data = {0};

    data.dev = device;
    data.sai = -1;
    data.opt = opt;

    mxl_soc_free_firmware_buf(device, buf, size, &data);
#else
    dma_addr_t dma_handle;

    dma_handle = dma_map_single(device, (void *)buf, size, DMA_TO_DEVICE);
    dma_free_coherent(device, size, (void *)buf, dma_handle);
#endif /* CONFIG_MXL_FW_DOWNLOAD */
}
EXPORT_SYMBOL(dc_dp_soc_free_firmware_buf);

int
dc_dp_soc_request_protect_buf(struct device *device, const void *buf,
    size_t size, enum dc_dp_fw_opt_type opt)
{
#if IS_ENABLED(CONFIG_MXL_FW_DOWNLOAD)
    struct mxl_fw_dl_data data = {0};

    data.dev = device;
    data.sai = -1;
    data.opt = opt;

    return mxl_soc_request_protect_buf(device, buf, size, &data);
#else
    return 0;
#endif /* CONFIG_MXL_FW_DOWNLOAD */
}
EXPORT_SYMBOL(dc_dp_soc_request_protect_buf);

/*
 * ========================================================================
 * Module Init/Exit API
 * ========================================================================
 */
static __init int dc_dp_init_module(void)
{
    int32_t ret = 0;

    if (!g_dc_dp_init_ok) {
        dc_dp_device_init();
        dc_dp_dcmode_init();
        dc_dp_notify_init();
        dc_dp_parser_init();

#ifdef CONFIG_PROC_FS
        dc_dp_proc_init();
#endif /* #ifdef CONFIG_PROC_FS */

        g_dc_dp_init_ok = 1;
    }

    return ret;
}

static __exit void dc_dp_exit_module(void)
{
    if (g_dc_dp_init_ok) {

#ifdef CONFIG_PROC_FS
        dc_dp_proc_exit();
#endif /* #ifdef CONFIG_PROC_FS */

        dc_dp_parser_exit();
        dc_dp_notify_exit();
        dc_dp_dcmode_exit();
        dc_dp_device_exit();
        g_dc_dp_init_ok = 0;
    }
}

module_init(dc_dp_init_module);
module_exit(dc_dp_exit_module);

MODULE_AUTHOR("Anath Bandhu Garai");
MODULE_DESCRIPTION("DirectConnect common datapath driver");
MODULE_LICENSE("GPL");
MODULE_VERSION(DC_DP_DRV_MODULE_VERSION);
