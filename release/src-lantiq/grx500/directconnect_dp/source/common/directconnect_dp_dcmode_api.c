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
//#include <net/directconnect_dp_api.h>
#include <directconnect_dp_dcmode_api.h>
#include "directconnect_dp_device.h"

static struct dc_dp_dcmode *g_dc_dp_dcmode[DC_DP_DCMODE_MAX];
extern int sw_dcmode1_init(void);
extern void sw_dcmode1_exit(void);

struct dc_dp_dcmode **
dc_dp_get_dcmode_head(void)
{
    return &g_dc_dp_dcmode[0];
}

int32_t
dc_dp_register_dcmode(struct dc_dp_dcmode *dcmode, uint32_t flags)
{
	if (!dcmode)
		return -1;

	if ( (flags & DC_DP_F_DCMODE_DEREGISTER) ) {
		if ( (dcmode->dcmode_cap & DC_DP_F_DCMODE_HW ) && g_dc_dp_dcmode[DC_DP_DCMODE_HW] != NULL)
			g_dc_dp_dcmode[DC_DP_DCMODE_HW] = NULL;
		else if ( (dcmode->dcmode_cap & DC_DP_F_DCMODE_SW ) && g_dc_dp_dcmode[DC_DP_DCMODE_SW] != NULL)
			g_dc_dp_dcmode[DC_DP_DCMODE_SW] = NULL;
		else
			return -1;
	} else {
		if ( (dcmode->dcmode_cap & DC_DP_F_DCMODE_HW ) && g_dc_dp_dcmode[DC_DP_DCMODE_HW] == NULL)
			g_dc_dp_dcmode[DC_DP_DCMODE_HW] = dcmode;
		else if ( (dcmode->dcmode_cap & DC_DP_F_DCMODE_SW ) && g_dc_dp_dcmode[DC_DP_DCMODE_SW] == NULL)
			g_dc_dp_dcmode[DC_DP_DCMODE_SW] = dcmode;
		else
			return -1;
	}
	return 0;
}
EXPORT_SYMBOL(dc_dp_register_dcmode);

int32_t
dc_dp_get_dcmode(void *ctx, int32_t port_id, struct dc_dp_res *res, struct dc_dp_dcmode **pp_dcmode, uint32_t flags)
{
    int32_t ret = -1;

    if (!pp_dcmode)
        goto err_out;

    /* NOTE : currently supports only one HW and one SW DC mode */
    if (is_hw_port(port_id) && g_dc_dp_dcmode[DC_DP_DCMODE_HW]) {
	*pp_dcmode = g_dc_dp_dcmode[DC_DP_DCMODE_HW];
        ret = 0;
    } else if ( (flags & (DC_DP_F_ALLOC_SW_TX_RING | DC_DP_F_ALLOC_SW_RX_RING) ) ) {
        /* FIXME : for a fastpath (hw) port, we may require SW DC mode at the same time??? */
        if (g_dc_dp_dcmode[DC_DP_DCMODE_SW]) {
            *pp_dcmode = g_dc_dp_dcmode[DC_DP_DCMODE_SW];
            ret = 0;
        }
    }

err_out:
    return ret;
}

int32_t
dc_dp_register_dcmode_device(struct module *owner, int32_t port_id, struct net_device *dev, void *p_dcmode_devinfo, uint32_t flags)
{
    int32_t ret = -1;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret)
        goto err_out;

    if (flags & DC_DP_DCMODE_DEV_DEREGISTER) {
        p_devinfo->priv = NULL;
        return 0;
    }

    p_devinfo->priv = p_dcmode_devinfo;

err_out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_register_dcmode_device);

int32_t
dc_dp_get_dcmode_device(int32_t port_id, void **pp_devinfo)
{
    int32_t ret = -1;
    struct dc_dp_priv_dev_info *p_devinfo = NULL;

    if (!pp_devinfo)
        goto err_out;

    ret = dc_dp_get_device_by_port(port_id, &p_devinfo);
    if (ret)
        goto err_out;

    *pp_devinfo = p_devinfo->priv;
    ret = 0;

err_out:
    return ret;
}
EXPORT_SYMBOL(dc_dp_get_dcmode_device);

int32_t
dc_dp_dcmode_init(void)
{
    int32_t ret = 0;

#ifdef SW_DCMODE1_BUILTIN
    ret = sw_dcmode1_init();
#endif

    return ret;
}

int32_t
dc_dp_dcmode_exit(void)
{
    int32_t ret = 0;

#ifdef SW_DCMODE1_BUILTIN
    sw_dcmode1_exit();
#endif

    return ret;
}
