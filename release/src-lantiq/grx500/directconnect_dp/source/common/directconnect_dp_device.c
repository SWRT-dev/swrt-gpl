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
//#include "directconnect_dp_api.h"
#include "directconnect_dp_device.h"

#define MAX(x, y) (x > y ? x : y)

/*!
    \brief DC DP device block synchronization
*/
struct dc_dp_dev_lock {
    spinlock_t        lock;  /*!< DC DP lock */
    unsigned long     flags; /*!< flag */
    uint32_t          cnt;   /*!< lock counter */
};

/* Global variables */
static struct dc_dp_priv_dev_info g_dc_dp_hw_dev[DC_DP_MAX_HW_DEVICE];
static struct dc_dp_priv_dev_info g_dc_dp_sw_dev[DC_DP_MAX_SW_DEVICE];
static struct dc_dp_dev_lock g_dc_dp_dev_lock;

static inline int32_t
dc_dp_lock_init(struct dc_dp_dev_lock *p_lock)
{
    spin_lock_init(&p_lock->lock);
    return 0;
}

static inline void
dc_dp_lock_destroy(struct dc_dp_dev_lock *p_lock)
{
    return;
}

static inline void
dc_dp_lock_device_list(struct dc_dp_dev_lock *p_lock)
{
    spin_lock_bh(&p_lock->lock);
}

static inline void
dc_dp_unlock_device_list(struct dc_dp_dev_lock *p_lock)
{
    spin_unlock_bh(&p_lock->lock);
}

static int32_t
__dc_dp_get_device(int32_t port_id, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = -1;
    int32_t dev_idx;

    if (port_id >= DC_DP_HW_PORT_RANGE_START && port_id <= DC_DP_HW_PORT_RANGE_END) {
        dev_idx = port_id;
        if (g_dc_dp_hw_dev[dev_idx].flags != DC_DP_DEV_FREE) {
            if (pp_devinfo)
                *pp_devinfo = &g_dc_dp_hw_dev[dev_idx];
            ret = 0;
        }
    }
    else if (port_id >= DC_DP_SW_PORT_RANGE_START && port_id <= DC_DP_SW_PORT_RANGE_END)
    {
        dev_idx = port_id - DC_DP_SW_PORT_RANGE_START;
        if (g_dc_dp_sw_dev[dev_idx].flags != DC_DP_DEV_FREE) {
            if (pp_devinfo)
                *pp_devinfo = &g_dc_dp_sw_dev[dev_idx];
            ret = 0;
        }
    }
    else {
        if (pp_devinfo)
            *pp_devinfo = NULL;
    }

    return ret;
}

static int32_t
__dc_dp_find_device(int32_t port_id, struct net_device *dev, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = -1;
    int32_t dev_idx;

    if (pp_devinfo)
        *pp_devinfo = NULL;

    if (port_id >= DC_DP_HW_PORT_RANGE_START && port_id <= DC_DP_HW_PORT_RANGE_END) {
        dev_idx = port_id;
        if ((g_dc_dp_hw_dev[dev_idx].flags != DC_DP_DEV_FREE) &&
            (g_dc_dp_hw_dev[dev_idx].dev == dev) && (g_dc_dp_hw_dev[dev_idx].port_id == port_id)) {
            if (pp_devinfo) {
                *pp_devinfo = &g_dc_dp_hw_dev[dev_idx];
                ret = 0;
            }
        }
    }
    else if (port_id >= DC_DP_SW_PORT_RANGE_START && port_id <= DC_DP_SW_PORT_RANGE_END)
    {
        dev_idx = port_id - DC_DP_SW_PORT_RANGE_START;
        if ((g_dc_dp_sw_dev[dev_idx].flags != DC_DP_DEV_FREE) &&
            (g_dc_dp_sw_dev[dev_idx].dev == dev) && (g_dc_dp_sw_dev[dev_idx].port_id == port_id)) {
            if (pp_devinfo) {
                *pp_devinfo = &g_dc_dp_sw_dev[dev_idx];
                ret = 0;
            }
        }
    }

    return ret;
}

static int32_t
__dc_dp_alloc_device(int32_t port_id, struct net_device *dev, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = -1;
    int32_t dev_idx;

    if (pp_devinfo)
        *pp_devinfo = NULL;

    if (port_id >= DC_DP_HW_PORT_RANGE_START && port_id <= DC_DP_HW_PORT_RANGE_END) {
        dev_idx = port_id;
        if (g_dc_dp_hw_dev[dev_idx].flags == DC_DP_DEV_FREE) {
           if (pp_devinfo) {
               *pp_devinfo = &g_dc_dp_hw_dev[dev_idx];
               ret = 0;
           }
        }
    }
    else if (port_id >= DC_DP_SW_PORT_RANGE_START && port_id <= DC_DP_SW_PORT_RANGE_END)
    {
        dev_idx = port_id - DC_DP_SW_PORT_RANGE_START;
        if (g_dc_dp_sw_dev[dev_idx].flags == DC_DP_DEV_FREE) {
            if (pp_devinfo) {
                *pp_devinfo = &g_dc_dp_sw_dev[dev_idx];
                ret = 0;
            }
        }
    }

    return ret;
}

static int32_t
__dc_dp_free_device(int32_t port_id, struct net_device *dev)
{
    int32_t ret = -1;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if ( (ret = __dc_dp_find_device(port_id, dev, &p_devinfo)) == 0 )
        memset(p_devinfo, 0, sizeof(struct dc_dp_priv_dev_info));

    return ret;
}

static int32_t
__dc_dp_alloc_subif(struct dc_dp_priv_dev_info *p_devinfo, struct dp_subif *subif, struct dc_dp_priv_subif_info **pp_subifinfo)
{
    int32_t ret = -1;
    int32_t subif_id;
    int32_t start = 0;
    int32_t end = DC_DP_MAX_SUBIF_PER_DEV;

    if (pp_subifinfo)
        *pp_subifinfo = NULL;

    if (subif && subif->subif >= 0) {
        start = dc_dp_get_subifidx(subif->subif);
        end = MAX((start + 1), DC_DP_MAX_SUBIF_PER_DEV);
    }

    for (subif_id = start; subif_id < end; subif_id++) {
        if (p_devinfo->subif_info[subif_id].flags == DC_DP_SUBIF_FREE) {
           if (pp_subifinfo) {
               p_devinfo->subif_info[subif_id].subif = ((subif_id & DC_DP_SUBIFID_MASK) << DC_DP_SUBIFID_OFFSET);
               p_devinfo->subif_info[subif_id].flags = DC_DP_SUBIF_REGISTERED;
               *pp_subifinfo = &p_devinfo->subif_info[subif_id];
               ret = 0;
           }
           break;
        }
    }

    return ret;
}

static int32_t
__dc_dp_free_subif(struct dc_dp_priv_subif_info *p_subifinfo)
{
    memset(p_subifinfo, 0, sizeof(struct dc_dp_priv_subif_info));

    return 0;
}

static void
dc_dp_free_device_list(void)
{
    int32_t dev_idx;

    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    /* Reset HW device array */
    for ( dev_idx = 0; dev_idx < DC_DP_MAX_HW_DEVICE; dev_idx ++ )
        memset(&g_dc_dp_hw_dev[dev_idx], 0, sizeof(struct dc_dp_priv_dev_info));

    /* Reset SW device array */
    for ( dev_idx = 0; dev_idx < DC_DP_MAX_SW_DEVICE; dev_idx ++ )
        memset(&g_dc_dp_sw_dev[dev_idx], 0, sizeof(struct dc_dp_priv_dev_info));

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);
}

/*
 * ####################################
 *           Exported API
 * ####################################
 */
int32_t
dc_dp_alloc_device(int32_t port_id, struct net_device *dev, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = -1;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    if ( (ret = __dc_dp_alloc_device(port_id, dev, &p_devinfo)) == 0 )
    {
        p_devinfo->port_id = port_id;
        p_devinfo->dev = dev;
        p_devinfo->flags = DC_DP_DEV_PORT_ALLOCATED;
    }

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);

    if (pp_devinfo)
        *pp_devinfo = p_devinfo;

    return ret;
}

void
dc_dp_free_device(int32_t port_id, struct net_device *dev)
{
    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    __dc_dp_free_device(port_id, dev);

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);
}

int32_t
dc_dp_alloc_subif(struct dc_dp_priv_dev_info *p_devinfo, struct net_device *dev,
                  struct dp_subif *subif, struct dc_dp_priv_subif_info **pp_subifinfo)
{
    int32_t ret = -1;

    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    if ( (ret = __dc_dp_alloc_subif(p_devinfo, subif, pp_subifinfo)) == 0 )
    {
        (*pp_subifinfo)->netif = dev;
    }

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);

    return ret;
}

void
dc_dp_free_subif(struct dc_dp_priv_subif_info *p_subifinfo)
{
    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    __dc_dp_free_subif(p_subifinfo);

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);
}

int32_t
dc_dp_get_device_by_module_port(struct module *owner, int32_t port_id, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = -1;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if ( pp_devinfo )
        *pp_devinfo = NULL;

    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    if ( (ret = __dc_dp_get_device(port_id, &p_devinfo)) == 0) {
        if ( (p_devinfo->owner == owner) && (p_devinfo->port_id == port_id) )
        {
            if ( pp_devinfo ) {
                *pp_devinfo = p_devinfo;
                ret = 0;
            }
        }
    }

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);

    return ret;
}

int32_t
dc_dp_get_device_by_port(int32_t port_id, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = -1;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if ( pp_devinfo )
        *pp_devinfo = NULL;

    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    if ((ret = __dc_dp_get_device(port_id, &p_devinfo)) == 0 ) {
        if ( pp_devinfo )
            *pp_devinfo = p_devinfo;
    }

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);

    return ret;
}

int32_t
dc_dp_get_device_by_subif_netif(struct net_device *netif, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;
    int32_t dev_idx;
    int32_t subif_idx;

    if (!pp_devinfo)
        goto out;

    *pp_devinfo = NULL;

    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    for ( dev_idx = 0; dev_idx < DC_DP_MAX_HW_DEVICE; dev_idx++ )
    {
        p_devinfo = &g_dc_dp_hw_dev[dev_idx];
        if (p_devinfo->flags != DC_DP_DEV_SUBIF_REGISTERED)
            continue;

        for (subif_idx = 0; subif_idx < DC_DP_MAX_SUBIF_PER_DEV; subif_idx++)
        {
            if (p_devinfo->subif_info[subif_idx].flags == DC_DP_SUBIF_FREE)
                continue;

            if ( p_devinfo->subif_info[subif_idx].netif == netif )
            {
                *pp_devinfo = p_devinfo;
                ret = subif_idx;
                break;
            }
        }

        if (*pp_devinfo)
            break;
    }

    if (NULL == *pp_devinfo) {
        for ( dev_idx = 0; dev_idx < DC_DP_MAX_SW_DEVICE; dev_idx++ )
        {
            p_devinfo = &g_dc_dp_sw_dev[dev_idx];
            if (p_devinfo->flags != DC_DP_DEV_SUBIF_REGISTERED)
                continue;

            for (subif_idx = 0; subif_idx < DC_DP_MAX_SUBIF_PER_DEV; subif_idx++)
            {
                if (p_devinfo->subif_info[subif_idx].flags == DC_DP_SUBIF_FREE)
                    continue;

                if ( p_devinfo->subif_info[subif_idx].netif == netif )
                {
                    *pp_devinfo = p_devinfo;
                    ret = subif_idx;
                    break;
                }
            }

            if (*pp_devinfo)
                break;
        }
    }

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);

out:
    return ret;
}

int32_t
dc_dp_get_device_by_subif_ifname(char *ifname, struct dc_dp_priv_dev_info **pp_devinfo)
{
    int32_t ret = DC_DP_FAILURE;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;
    int32_t dev_idx;
    int32_t subif_idx;

    if (!pp_devinfo)
        goto out;

    *pp_devinfo = NULL;

    dc_dp_lock_device_list(&g_dc_dp_dev_lock);

    for ( dev_idx = 0; dev_idx < DC_DP_MAX_HW_DEVICE; dev_idx++ )
    {
        p_devinfo = &g_dc_dp_hw_dev[dev_idx];
        if (p_devinfo->flags != DC_DP_DEV_SUBIF_REGISTERED)
            continue;

        for (subif_idx = 0; subif_idx < DC_DP_MAX_SUBIF_PER_DEV; subif_idx++)
        {
            if (p_devinfo->subif_info[subif_idx].flags == DC_DP_SUBIF_FREE)
                continue;

            if ( strncmp(p_devinfo->subif_info[subif_idx].device_name, ifname, IFNAMSIZ) == 0 )
            {
                *pp_devinfo = p_devinfo;
                ret = subif_idx;
                break;
            }
        }

        if (*pp_devinfo)
            break;
    }

    if (NULL == *pp_devinfo) {
        for ( dev_idx = 0; dev_idx < DC_DP_MAX_SW_DEVICE; dev_idx++ )
        {
            p_devinfo = &g_dc_dp_sw_dev[dev_idx];
            if (p_devinfo->flags != DC_DP_DEV_SUBIF_REGISTERED)
                continue;

            for (subif_idx = 0; subif_idx < DC_DP_MAX_SUBIF_PER_DEV; subif_idx++)
            {
                if (p_devinfo->subif_info[subif_idx].flags == DC_DP_SUBIF_FREE)
                    continue;

                if ( strncmp(p_devinfo->subif_info[subif_idx].device_name, ifname, IFNAMSIZ) == 0 )
                {
                    *pp_devinfo = p_devinfo;
                    ret = subif_idx;
                    break;
                }
            }

            if (*pp_devinfo)
                break;
        }
    }

    dc_dp_unlock_device_list(&g_dc_dp_dev_lock);

out:
    return ret;
}

int32_t
is_hw_port(int32_t port_id)
{
    if (port_id >= DC_DP_HW_PORT_RANGE_START && port_id <= DC_DP_HW_PORT_RANGE_END)
        return 1;
    else
        return 0;
}

int32_t
is_sw_port(int32_t port_id)
{
    if (port_id >= DC_DP_SW_PORT_RANGE_START && port_id <= DC_DP_SW_PORT_RANGE_END)
        return 1;
    else
        return 0;
}

struct dc_dp_priv_dev_info *
dc_dp_get_hw_device_head(void)
{
    return &g_dc_dp_hw_dev[0];
}

struct dc_dp_priv_dev_info *
dc_dp_get_sw_device_head(void)
{
    return &g_dc_dp_sw_dev[0];
}

int32_t
dc_dp_device_init(void)
{
    dc_dp_lock_init(&g_dc_dp_dev_lock);
    dc_dp_free_device_list();

    return 0;
}

void
dc_dp_device_exit(void)
{
    dc_dp_free_device_list();
    dc_dp_lock_destroy(&g_dc_dp_dev_lock);
}
