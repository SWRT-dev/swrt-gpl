/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DATAPATH_SWITCHDEV_H_
#define DATAPATH_SWITCHDEV_H_

int dp_swdev_alloc_bridge_id_32(int inst);
int dp_swdev_bridge_port_cfg_set_32(struct br_info *br_item,
				    int inst, int bport, u32 flags);
int dp_swdev_bridge_port_cfg_reset_32(struct br_info *br_item,
				      int inst, int bport);
int dp_swdev_bridge_cfg_set_32(int inst, u16 fid);
int dp_swdev_free_brcfg_32(int inst, u16 fid);
int dp_gswip_ext_vlan_32(int inst, int vap, int ep);
#endif
