/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/etherdevice.h>
#include <net/datapath_api.h>
#include "../datapath_swdev.h"
#include "../datapath.h"
#include "datapath_misc.h"

int dp_swdev_alloc_bridge_id(int inst)
{
	GSW_return_t ret;
	GSW_BRIDGE_alloc_t br;
	struct core_ops *gsw_handle;

	gsw_handle = dp_port_prop[inst].ops[0];
	memset(&br, 0, sizeof(br));
	ret = gsw_handle->gsw_brdg_ops.Bridge_Alloc(gsw_handle, &br);
	if ((ret != GSW_statusOk) ||
	    (br.nBridgeId < 0)) {
		pr_err("Failed to get a FID\n");
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "FID=%d\n", br.nBridgeId);
	return br.nBridgeId;
}

int dp_swdev_bridge_port_cfg_set(struct br_info *br_item,
				 int inst, int bport, u32 flags)
{
	GSW_return_t ret;
	struct bridge_member_port *bport_list = NULL;
	GSW_BRIDGE_portConfig_t brportcfg;
	struct core_ops *gsw_handle;
	struct brdgport_ops *gsw_bp;

	gsw_handle = dp_port_prop[inst].ops[0];
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	/*To set other members to the current bport*/
	memset(&brportcfg, 0, sizeof(GSW_BRIDGE_portConfig_t));
	brportcfg.nBridgePortId = bport;
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "Set current BP=%d inst:%d\n",
		 brportcfg.nBridgePortId, inst);
	brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, &brportcfg);
	if (ret != GSW_statusOk) {
		pr_err("fail in getting bridge port config\r\n");
		return -1;
	}
	list_for_each_entry(bport_list, &br_item->bp_list, list) {
		if (bport_list->portid != bport) {

			/* ----- Downstream ------
			 * WAN Brodcast or Mcast Gem port, if the member
			 * going to add is a GPON or EPON subif
			 * (It can be pmapper or gemport or llid), then don't
			 * add the member to the portmap
			 */
			if ((flags & IFF_NO_QUEUE) &&
			    (bport_list->alloc_flag &
			     (DP_F_GPON | DP_F_EPON))) {

				DP_DEBUG(DP_DBG_FLAG_SWDEV,
					 "IFF_NO_QUEUE Don't add if to pmap\n");
				UNSET_BP_MAP(brportcfg.nBridgePortMap,
					     bport_list->portid);
				continue;
			}
			SET_BP_MAP(brportcfg.nBridgePortMap,
				   bport_list->portid);
		}
	}
	brportcfg.nBridgeId = br_item->fid;
	brportcfg.nBridgePortId = bport;
	brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
			  GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &brportcfg);
	if (ret != GSW_statusOk) {
		pr_err("Fail in allocating/configuring bridge port\n");
		return -1;
	}
	/* To set other member portmap with current bridge port map */
	list_for_each_entry(bport_list, &br_item->bp_list, list) {
		if (bport_list->portid == bport)
			continue;
		/* ----- Upstream ------
		 * If netdevice have Flag IFF_NO_QUEUE,
		 * dont add the interface to gswip bridge portmap
		 */
		if (flags & IFF_NO_QUEUE) {
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "IFF_NO_QUEUE Don't add if to pmap\n");
			continue;
		}
		memset(&brportcfg, 0, sizeof(GSW_BRIDGE_portConfig_t));
		brportcfg.nBridgePortId = bport_list->portid;
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "Set other BP=%d inst:%d\n",
			 brportcfg.nBridgePortId, inst);
		brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
		ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, &brportcfg);
		if (ret != GSW_statusOk) {
			pr_err("fail in getting br port config\r\n");
			return -1;
		}
		SET_BP_MAP(brportcfg.nBridgePortMap, bport);
		brportcfg.nBridgeId = br_item->fid;
		brportcfg.nBridgePortId = bport_list->portid;
		brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
				  GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
		ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &brportcfg);
		if (ret != GSW_statusOk) {
			pr_err("Fail alloc/cfg bridge port\n");
			return -1;
		}
	}
	return 0;
}

int dp_swdev_bridge_port_cfg_reset(struct br_info *br_item,
				   int inst, int bport)
{
	GSW_BRIDGE_portConfig_t brportcfg;
	struct bridge_member_port *bport_list = NULL;
	int i, cnt = 0, bp = 0;
	GSW_return_t ret;
	struct core_ops *gsw_handle;
	struct brdgport_ops *gsw_bp;

	gsw_handle = dp_port_prop[inst].ops[0];
	gsw_bp = &gsw_handle->gsw_brdgport_ops;
	memset(&brportcfg, 0, sizeof(GSW_BRIDGE_portConfig_t));
	brportcfg.nBridgePortId = bport;
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "Reset BP=%d inst:%d\n",
		 brportcfg.nBridgePortId, inst);
	brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	/*Reset other members from current bport map*/
	ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, &brportcfg);
	if (ret != GSW_statusOk) {
		/* Note: here may fail if this device is not removed from
		 * linux bridge via brctl delif but user try to un-regiser
		 * from DP. The correct flow to unregister is like below:
		 *  1) brctl delif xxx xxxx: remove this device from bridge
		 *  2) dp_register_subif_ext: to un-register device from DP
		 * Anyway, it will also work if not follow this propsal.
		 * The only side effect is this API call fail since GSWIP
		 * bridge port is already freed during subif_hw_reset before
		 * this API call
		 */
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "GSW_BRIDGE_portConfig_t fail:bp=%d\n", bport);
		return -1;
	}
	for (i = 0; i < MAX_BP_NUM; i++) {
		if (GET_BP_MAP(brportcfg.nBridgePortMap, i)) {
			bp = i;
			cnt++;
		}
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "cnt:%d last bp:%d\n", cnt, bp);
	list_for_each_entry(bport_list, &br_item->bp_list, list) {
		if (bport_list->portid != bport) {
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "reset other from current BP=%d inst:%d\n",
				 brportcfg.nBridgePortId, inst);
			UNSET_BP_MAP(brportcfg.nBridgePortMap,
				     bport_list->portid);
		}
	}
	brportcfg.nBridgeId = CPU_FID; /*reset of FID*/
	brportcfg.nBridgePortId = bport;
	brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
			  GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
	ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &brportcfg);
	if (ret != GSW_statusOk) {
		pr_err("Fail in configuring GSW_BRIDGE_portConfig_t in %s\r\n",
		       __func__);
		return -1;
	}
	/*Reset current bp from all other bridge port's port map*/
	list_for_each_entry(bport_list, &br_item->bp_list, list) {
		if (bport_list->portid == bport)
			continue;
		memset(&brportcfg, 0, sizeof(GSW_BRIDGE_portConfig_t));
		brportcfg.nBridgePortId = bport_list->portid;
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "reset current BP from other BP=%d inst:%d\n",
			 brportcfg.nBridgePortId, inst);
		brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP |
				  GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID;
		ret = gsw_bp->BridgePort_ConfigGet(gsw_handle, &brportcfg);
		if (ret != GSW_statusOk) {
			pr_err("failed getting br port cfg\r\n");
			return -1;
		}
		UNSET_BP_MAP(brportcfg.nBridgePortMap, bport);
		brportcfg.nBridgePortId = bport_list->portid;
		brportcfg.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_ID |
				  GSW_BRIDGE_PORT_CONFIG_MASK_BRIDGE_PORT_MAP;
		ret = gsw_bp->BridgePort_ConfigSet(gsw_handle, &brportcfg);
		if (ret != GSW_statusOk) {
			pr_err("Fail alloc/cfg br port\n");
			return -1;
		}
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "%s success\n", __func__);
	/*Remove bridge entry if no member in port map of
	 * current bport except CPU port
	 */
	if ((bp == 0 && cnt == 1) || (cnt == 0))
		return DEL_BRENTRY;

	return 0;
}

int dp_swdev_bridge_cfg_set(int inst, u16 fid)
{
	GSW_return_t ret;
	GSW_BRIDGE_config_t brcfg;
	GSW_BRIDGE_alloc_t br;
	struct core_ops *gsw_handle;

	gsw_handle = dp_port_prop[inst].ops[0];
	memset(&br, 0, sizeof(br));
	memset(&brcfg, 0, sizeof(brcfg));
	brcfg.nBridgeId = fid;
	brcfg.eMask = GSW_BRIDGE_CONFIG_MASK_FORWARDING_MODE;
	brcfg.eForwardBroadcast = GSW_BRIDGE_FORWARD_FLOOD;
	brcfg.eForwardUnknownMulticastNonIp = GSW_BRIDGE_FORWARD_FLOOD;
	brcfg.eForwardUnknownUnicast = GSW_BRIDGE_FORWARD_FLOOD;
	ret = gsw_handle->gsw_brdg_ops.Bridge_ConfigSet(gsw_handle, &brcfg);
	if (ret != GSW_statusOk) {
		pr_err("Failed to set bridge id(%d)\n", brcfg.nBridgeId);
		br.nBridgeId = fid;
		gsw_handle->gsw_brdg_ops.Bridge_Free(gsw_handle, &br);
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "FID(%d) cfg success for inst %d\n",
		 fid, inst);
	return 0;
}

int dp_swdev_free_brcfg(int inst, u16 fid)
{
	GSW_return_t ret;
	GSW_BRIDGE_alloc_t br;
	struct core_ops *gsw_handle;

	gsw_handle = dp_port_prop[inst].ops[0];
	memset(&br, 0, sizeof(br));
	br.nBridgeId = fid;
	ret = gsw_handle->gsw_brdg_ops.Bridge_Free(gsw_handle, &br);
	if (ret != GSW_statusOk) {
		pr_err("Failed to free bridge id(%d)\n", br.nBridgeId);
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "FID(%d) freed for inst:%d\n", fid, inst);
	return 0;
}

int dp_gswip_ext_vlan(int inst, int vap, int ep)
{
	struct core_ops *gsw_handle;
	struct ext_vlan_info *vlan;
	struct vlan_prop vlan_prop = {0};
	struct pmac_port_info *port;
	struct logic_dev *tmp = NULL;
	int flag = 0, ret, i = 0;
	int v1 = 0, v2 = 0;
	struct dp_subif_info *sif;

	gsw_handle = dp_port_prop[inst].ops[0];
	port = get_dp_port_info(inst, ep);
	vlan = kzalloc(sizeof(*vlan), GFP_KERNEL);
	if (!vlan) {
		pr_err("failed to alloc ext_vlan of %d bytes\n", sizeof(*vlan));
		return 0;
	}
	vlan->vlan2_list = kzalloc(sizeof(*vlan->vlan2_list), GFP_KERNEL);
	if (!vlan->vlan2_list) {
		pr_err("failed to alloc ext_vlan of %d bytes\n",
		       sizeof(*vlan->vlan2_list));
		goto EXIT;
	}
	vlan->vlan1_list = kzalloc(sizeof(*vlan->vlan1_list), GFP_KERNEL);
	if (!vlan->vlan1_list) {
		pr_err("failed to alloc ext_vlan of %d bytes\n",
		       sizeof(*vlan->vlan1_list));
		goto EXIT;
	}
	sif = get_dp_port_subif(port, vap);
	list_for_each_entry(tmp, &sif->logic_dev, list) {
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "tmp dev name:%s\n",
			 tmp->dev ? tmp->dev->name : "NULL");
		if (!tmp->dev) {
			pr_err("tmp->dev is NULL\n");
			goto EXIT;
		}
		ret = dp_swdev_chk_bport_in_br(tmp->dev, tmp->bp, inst);
		if (ret == 0) {
			get_vlan_via_dev(tmp->dev, &vlan_prop);
			if (vlan_prop.num == 2) {
				DP_DEBUG(DP_DBG_FLAG_SWDEV,
					 "VLAN Inner proto=%x, vid=%d\n",
					 vlan_prop.in_proto, vlan_prop.in_vid);
				DP_DEBUG(DP_DBG_FLAG_SWDEV,
					 "VLAN out proto=%x, vid=%d\n",
					 vlan_prop.out_proto,
					 vlan_prop.out_vid);
				vlan->vlan2_list[v2].outer_vlan.vid =
							vlan_prop.out_vid;
				vlan->vlan2_list[v2].outer_vlan.tpid =
							vlan_prop.out_proto;
				vlan->vlan2_list[v2].ether_type = 0;
				vlan->vlan2_list[v2].inner_vlan.vid =
							vlan_prop.in_vid;
				vlan->vlan2_list[v2].inner_vlan.tpid =
							vlan_prop.in_proto;
				vlan->vlan2_list[v2].bp = tmp->bp;
				v2 += 1;
			} else if (vlan_prop.num == 1) {
				DP_DEBUG(DP_DBG_FLAG_SWDEV,
					 "outer VLAN proto=%x, vid=%d\n",
					 vlan_prop.out_proto,
					 vlan_prop.out_vid);
				vlan->vlan1_list[v1].outer_vlan.vid =
							vlan_prop.out_vid;
				vlan->vlan1_list[v1].outer_vlan.tpid =
							vlan_prop.out_proto;
				vlan->vlan1_list[v1].bp = tmp->bp;
				v1 += 1;
			}
			i += 1;
		}
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "vlan1=%d vlan2=%d total vlan int=%d\n",
		 v1, v2, i);
	vlan->n_vlan1 = v1;
	vlan->n_vlan2 = v2;
	vlan->bp = sif->bp;
	vlan->logic_port = port->port_id;
	vlan->subif_grp = sif->subif;/*subif value*/

	if (sif->swdev_priv)
		vlan->priv = sif->swdev_priv;
	else
		vlan->priv = NULL;
	ret = set_gswip_ext_vlan(gsw_handle, vlan, flag);
	if (ret == 0)
		sif->swdev_priv = vlan->priv;
	else
		pr_err("set gswip ext vlan return error\n");

EXIT:
	kfree(vlan->vlan2_list);
	kfree(vlan->vlan1_list);
	kfree(vlan);
	return 0; /*return -EIO from GSWIP but later cannot fail swdev*/
}
