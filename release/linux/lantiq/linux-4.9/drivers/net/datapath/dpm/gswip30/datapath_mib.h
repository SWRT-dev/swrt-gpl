/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DATAPATH_MIB_H
#define DATAPATH_MIB_H

int dp_reset_mib(u32 flag);
int set_gsw_itf(u8 ep, u8 ena, int start);
struct gsw_itf *get_free_itf(u8 ep, u32 flag);
int reset_gsw_itf(u8 ep);
int dp_get_port_vap_mib_30(dp_subif_t *subif, void *priv,
			   struct rtnl_link_stats64 *net_mib, u32 flag);
int dp_clear_netif_mib_30(dp_subif_t *subif, void *priv, u32 flag);
#endif
