// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#include <net/switch_api/gsw_flow_ops.h>
#include <net/datapath_api.h>
#include <net/datapath_api_vlan.h>
#include "datapath.h"
#include "datapath_instance.h"
#include "datapath_swdev.h"
#include "datapath_pce.h"
#include <linux/if_bridge.h>
#include <../net/bridge/br_private.h>

#define MAX_FID		64 /* Only 6 bits for FiD */

enum BR_VLAN_FLAG {
	BR_VLAN_LIST_TOP_ENTRY = BIT(0),
	BR_VLAN_DEREGISTER = BIT(1)
};

static int dp_del_vlan_entry(struct br_info *br_info,
			     struct vlist_entry *vlist_entry, dp_subif_t *subif,
			     u16 vlanid, int flags);

/* Check Bridge have the entry added through Self
 * if vlanid=0, returns first entry
 * if valid vlanid, returns entry pointed by VLAN ID
 */
struct vlist_entry *get_vlist_entry_in_br(struct br_info *br_info,
					  u16 vlanid, int flags)
{
	struct vlist_entry *list_entry = NULL;

	/* Returning the top entry in list or Return the exact matching Entry */
	if (flags & BR_VLAN_LIST_TOP_ENTRY) {
		list_for_each_entry(list_entry, &br_info->br_vlan_list, list) {
			if (!list_entry->vlan_entry)
				continue;
			return list_entry;
		}
	} else {
		list_for_each_entry(list_entry, &br_info->br_vlan_list, list) {
			if (!list_entry->vlan_entry ||
			    (vlanid != list_entry->vlan_entry->vlan_id))
				continue;
			return list_entry;
		}
	}
	return NULL;
}

/* Check through all Bridge Member port have the entry added through Master
 * if have return the entry
 */
struct vlist_entry *get_vlist_entry_in_all_bp(struct br_info *br_info,
					      u16 vlanid)
{
	struct vlist_entry *list_entry = NULL;
	struct bridge_member_port *bmp;

	/* Check Bridge Member port have the entry added through Master */
	list_for_each_entry(bmp, &br_info->bp_list, list) {
		list_for_each_entry(list_entry, &bmp->bport_vlan_list, list) {
			if (!list_entry->vlan_entry ||
			    (vlanid != list_entry->vlan_entry->vlan_id))
				continue;
			return list_entry;
		}
	}
	return NULL;
}

/* Get the List Head for the BP Vlan List */
struct list_head *get_vlan_entry_head_in_bp(struct br_info *br_info,
					    int bport)
{
	struct bridge_member_port *bmp;

	list_for_each_entry(bmp, &br_info->bp_list, list) {
		if (bmp->bportid != bport)
			continue;
		return &bmp->bport_vlan_list;
	}
	return NULL;
}

/* Check through Bridge Member port specified have the entry added
 * through Master if have return the entry
 */
struct vlist_entry *get_vlist_entry_in_bp(struct br_info *br_info,
					  int bport, u16 vlanid, int flags)
{
	struct vlist_entry *list_entry = NULL;
	struct bridge_member_port *bmp;

	/* Returning the top entry in list or Return the exact matching Entry */
	if (flags & BR_VLAN_LIST_TOP_ENTRY) {
		list_for_each_entry(bmp, &br_info->bp_list, list) {
			if (bmp->bportid != bport)
				continue;
			list_for_each_entry(list_entry, &bmp->bport_vlan_list,
					    list) {
				if (!list_entry->vlan_entry)
					continue;
				return list_entry;
			}
		}
	} else {
		list_for_each_entry(bmp, &br_info->bp_list, list) {
			if (bmp->bportid != bport)
				continue;
			list_for_each_entry(list_entry, &bmp->bport_vlan_list,
					    list) {
				if (!list_entry->vlan_entry ||
				    (vlanid != list_entry->vlan_entry->vlan_id))
					continue;
				return list_entry;
			}
		}
	}
	return NULL;
}

/* Get pce entry from vlan entry */
struct pce_entry *get_pce_entry_from_ventry(struct vlan_entry *ventry,
					    int portid, int subifid)
{
	struct pce_entry *pentry;
	struct dp_subblk_info *info;

	list_for_each_entry(pentry, &ventry->pce_list, list) {
		if (!pentry->blk_info)
			continue;
		if (pentry->blk_info->region == GSW_PCE_RULE_CTP) {
			info = &pentry->blk_info->info;
			if ((info->portid == portid) &&
			    (info->subif == subifid))
				return pentry;
		}
	}
	return NULL;
}

/* Get pce entry from Bp */
static
struct pce_entry *get_pce_entry_from_bp(struct br_info *br_info,
					dp_subif_t *subif, int vlanid)
{
	struct bridge_member_port *bmp;
	struct vlist_entry *v;
	struct pce_entry *pce;
	struct pmac_port_info *port_info;
	int vap;

	port_info = get_dp_port_info(subif->inst, subif->port_id);
	vap = GET_VAP(subif->subif, port_info->vap_offset,
			      port_info->vap_mask);

	list_for_each_entry(bmp, &br_info->bp_list, list) {
		if (bmp->bportid != subif->bport)
			continue;
		list_for_each_entry(v, &bmp->bport_vlan_list, list) {
			if (!v->vlan_entry ||
			    vlanid != v->vlan_entry->vlan_id)
				continue;
			pce = get_pce_entry_from_ventry(v->vlan_entry,
							subif->port_id,
							vap);
			if (pce)
				return pce;
		}
	}
	return NULL;
}

/* Get pce entry from all vlan entries */
static
struct pce_entry *get_pce_entry_all_ventry(struct br_info *br_info,
					   int portid, int subifid)
{
	struct pce_entry *pentry = NULL;
	struct vlist_entry *v = NULL;
	struct bridge_member_port *bmp;

	list_for_each_entry(bmp, &br_info->bp_list, list) {
		list_for_each_entry(v, &bmp->bport_vlan_list, list) {
			if (!v->vlan_entry)
				continue;
			pentry = get_pce_entry_from_ventry(v->vlan_entry,
							   portid, subifid);
			if (pentry)
				return pentry;
		}
	}
	return NULL;
}

/* When Bridge Entry is created/deleted in DPM we check whether this bridge is
 * VLAN Aware and change ndo_bridge ops
 */
int dp_register_br_vlan(struct br_info *br_info, int flags)
{
	int offset;
	int flag = DP_OPS_NETDEV | flags;
	struct net_device *br_dev;
	struct vlist_entry *list_entry = NULL;
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
	struct net_bridge *br;
#endif

	if (!br_info)
		return DP_FAILURE;

	if (!br_info->max_vlan_limit) {
		br_info->max_vlan_limit = DP_DEF_MAX_VLAN_LIMIT;
		br_info->max_brid_limit = DP_DEF_MAX_VLAN_LIMIT;
	}

	br_dev = br_info->dev;

	if (!netif_is_bridge_master(br_dev)) {
		pr_err("%s: dev %s is not bridge dev\n", __func__,
		       br_dev->name);
		return DP_FAILURE;
	}

	/* Netdev ops need to register only if VLAN Aware */
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 14, 0)
	br = netdev_priv(br_dev);
	if (!br_vlan_enabled(br)) {
#else
	if (!br_vlan_enabled(br_dev)) {
#endif
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: Bridge %s is not VLAN Aware\n", __func__,
			 br_dev->name);
		return DP_SUCCESS;
	}

	/* Using br_vlan_en flag to check bridge ops registered */
	if (flags & DP_OPS_RESET) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: Un-Register br %s netdev ops\n",
			 __func__, br_dev->name);
		br_info->br_vlan_en = false; /* Mark as ops unreg */

		while ((list_entry =
				get_vlist_entry_in_br(br_info, 0,
						      BR_VLAN_LIST_TOP_ENTRY))) {
			dp_del_vlan_entry(br_info,
					  list_entry, 0,
					  list_entry->vlan_entry->vlan_id,
					  BRIDGE_FLAGS_SELF);
		}
	} else if (br_info->br_vlan_en) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: Already ops registered %s\n", __func__,
			 br_dev->name);
		goto EXIT;	/* Already ops registered for this bridge */
	} else {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: Register br %s netdev ops\n", __func__,
			 br_dev->name);
		br_info->br_vlan_en = true; /* mark as ops regd */
	}

	offset = offsetof(const struct net_device_ops, ndo_bridge_setlink);
	dp_set_net_dev_ops_priv(br_dev, &dp_ndo_bridge_setlink, offset,
				flag);

	offset = offsetof(const struct net_device_ops, ndo_bridge_dellink);
	dp_set_net_dev_ops_priv(br_dev, &dp_ndo_bridge_dellink, offset,
				flag);
EXIT:
	return DP_SUCCESS;
}

/* When Bridge Port Entry is created/deleted in DPM
 * we check whether this bridge is VLAN Aware and change ndo_bridge ops
 */
int dp_register_bport_vlan(struct br_info *br_info,
			   struct net_device *dev, int bport, int flags)
{
	int offset;
	int flag = DP_OPS_NETDEV | flags;
	struct vlist_entry *list_entry = NULL;
	dp_subif_t subif;

	if (!dev || !br_info)
		return DP_FAILURE;

	if (!br_info->max_vlan_limit) {
		br_info->max_vlan_limit = DP_DEF_MAX_VLAN_LIMIT;
		br_info->max_brid_limit = DP_DEF_MAX_VLAN_LIMIT;
	}

	if (!netif_is_bridge_port(dev)) {
		pr_err("%s: dev %s is not bridge port\n", __func__, dev->name);
		return DP_FAILURE;
	}

	/* If not VLAN Aware we are not registering ops for this device */
	if (!br_info->br_vlan_en) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: Vlan Aware is not enabled for this Bridge %s\n",
			 __func__, br_info->dev->name);
		return DP_SUCCESS;
	}

	if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
		pr_err("%s: dp_get_netif_subifid failed\n", __func__);
		return DP_FAILURE;
	}

	if (flags & DP_OPS_RESET) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: %s dev=%s in br=%s bport=%d netdev ops\n",
			 __func__, "Un-Register", dev->name, br_info->dev->name,
			 bport);

		while ((list_entry =
				get_vlist_entry_in_bp(br_info, bport, 0,
						      BR_VLAN_LIST_TOP_ENTRY))) {
			dp_del_vlan_entry(br_info, list_entry, &subif,
					  list_entry->vlan_entry->vlan_id,
					  BRIDGE_FLAGS_MASTER);
		}
	} else {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: Register dev=%s in br=%s netdev ops\n", __func__,
			 dev->name, br_info->dev->name);
	}

	/* Register member port ndo_bridge ops callback */
	offset = offsetof(const struct net_device_ops, ndo_bridge_setlink);
	dp_set_net_dev_ops_priv(dev, &dp_ndo_bridge_setlink, offset,
				flag);

	offset = offsetof(const struct net_device_ops, ndo_bridge_dellink);
	dp_set_net_dev_ops_priv(dev, &dp_ndo_bridge_dellink, offset,
				flag);

	return DP_SUCCESS;
}

/* Copy the Bridge Settings of bridge to the new Vlan Aware FiD's */
static int dp_gsw_bridge_cfg_copy(int inst, u16 src_fid, u16 dst_fid)
{
	GSW_return_t ret = DP_SUCCESS;
	GSW_BRIDGE_config_t brcfg;
	struct core_ops *gsw_handle;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: src_fid=%d dst_fid=%d\n",
		 __func__, src_fid, dst_fid);

	gsw_handle = dp_port_prop[inst].ops[0];
	if (!gsw_handle)
		return -1;
	memset(&brcfg, 0, sizeof(brcfg));

	brcfg.nBridgeId = src_fid;
	ret = gsw_handle->gsw_brdg_ops.Bridge_ConfigGet(gsw_handle, &brcfg);
	if (ret)
		return ret;

	brcfg.nBridgeId = dst_fid;
	ret = gsw_handle->gsw_brdg_ops.Bridge_ConfigSet(gsw_handle, &brcfg);
	if (ret)
		return ret;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "FID(%d to %d) copy success\n",
		 src_fid, dst_fid);
	return ret;
}

/* Free the Bridge FiD created in GSWIP */
static int deallocate_br_vlan_fid(struct br_info *br_info,
				  u16 vlanid, u16 fid)
{
	struct inst_info *dp_info = get_dp_prop_info(0);
	int ret = 0;

	ret = dp_info->swdev_free_brcfg(0, fid);
	if (ret) {
		pr_err("%s: swdev_free_brcfg Failed for FiD %d\n",
		       __func__, fid);
		return DP_FAILURE;
	}

	br_info->num_fid--;
	return DP_SUCCESS;
}

/* Allocate new VLAN Aware Bridge FiD in GSWIP */
static int allocate_br_vlan_fid(struct br_info *br_info, u16 vlanid)
{
	u16 fid = 0;
	struct inst_info *dp_info = get_dp_prop_info(0);
	int new_brid;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: Br=%s Br_Fid=%d vlanid=%d num_vlan=%d num_fid=%d\n",
		 __func__, br_info->dev->name, br_info->fid, vlanid,
		 br_info->num_vlan, br_info->num_fid);

	if (br_info->num_vlan > br_info->max_vlan_limit) {
		pr_err("%s: Cannot Support more than %d VLAN ID's \n",
		       __func__, br_info->max_vlan_limit);
		return DP_FAILURE;
	}

	if (br_info->num_fid > br_info->max_brid_limit) {
		pr_err("%s: Cannot Support more than %d Bridge ID's \n",
		       __func__, br_info->max_brid_limit);
		return DP_FAILURE;
	}

	new_brid = dp_info->swdev_alloc_bridge_id(br_info->inst);
	if (new_brid <= 0) {
		pr_err("%s: Switch bridge alloc failed\n", __func__);
		return DP_FAILURE;
	}

	fid = new_brid;

	if (new_brid > MAX_FID) {
		dp_info->swdev_free_brcfg(br_info->inst, fid);
		pr_err("%s: VLAN Aware support max 64 FiD (VlanID + BriID)\n",
		       __func__);
		return DP_FAILURE;
	}

	if (dp_gsw_bridge_cfg_copy(br_info->inst, br_info->fid, fid)) {
		dp_info->swdev_free_brcfg(br_info->inst, fid);
		pr_err("%s: Bridge Cfg Copy for Fid %d to Fid %d failed\n",
		       __func__, br_info->fid, fid);
		return DP_FAILURE;
	}

	br_info->num_fid++;
	return fid;
}

static bool is_pmapper_vlan_aware(int inst, int bp)
{
	struct bp_pmapper *bp_info;
	struct net_device *br_dev;
	struct br_info *br_info;
	struct vlist_entry *list_entry = NULL;
	struct bridge_member_port *bmp;

	bp_info = get_dp_bp_info(inst, bp);
	rcu_read_lock();
	br_dev = netdev_master_upper_dev_get_rcu(bp_info->dev);
	if (!br_dev) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: pmapper dev(%s) not in bridge\n", __func__,
			 bp_info->dev->name ? bp_info->dev->name : "NULL");
		rcu_read_unlock();
		return false;
	}

	br_info = dp_swdev_bridge_entry_lookup_rcu(br_dev);
	if (!br_info) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: br_info not added to DPM\n", __func__);
		rcu_read_unlock();
		return false;
	}
	rcu_read_unlock();

	if (!br_info->br_vlan_en) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: br=%s is not VLAN Aware\n",
			 __func__, br_dev->name ? br_dev->name : "NULL");
		return false;
	}

	/* Check if Bridge Member port have any VLAN entry added */
	list_for_each_entry(bmp, &br_info->bp_list, list) {
		if (bmp->bportid != bp)
			continue;
		list_for_each_entry(list_entry, &bmp->bport_vlan_list, list) {
			if (!list_entry->vlan_entry)
				continue;
			if (list_entry->vlan_entry->vlan_id)
				return true;
		}
	}
	return false;
}

/* In case of Pmapper, Copies FirstFlowEntryIndex to all its CTP ports in GSWIP */
int dp_gsw_copy_ctp_cfg(int inst, int bp, struct dp_subblk_info *subblk_info,
			int portid)
{
	struct ctp_dev *ctp_entry;
	struct bp_pmapper *bp_info;
	GSW_CTP_portConfig_t ctpcfg = {0};
	struct core_ops *gsw_ops;
	int flow_index;

	gsw_ops = dp_port_prop[inst].ops[0];

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: bp=%d, is pmapper\n", __func__, bp);

	/* check if pmapper is VLAN aware before PCE block copy to other CTP */
	if (!is_pmapper_vlan_aware(inst, bp)) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: pmapper is not vlan aware\n",
			 __func__);
		return DP_SUCCESS;
	}

	bp_info = get_dp_bp_info(inst, bp);
	if ((!bp_info) || (!bp_info->ref_cnt))
		return DP_FAILURE;


	/* Retrieve the CTP dev list for the pmapper bridge port */
	list_for_each_entry(ctp_entry, &bp_info->ctp_dev, list) {
		if (!ctp_entry->dev)
			continue;
		ctpcfg.nLogicalPortId = portid;
		ctpcfg.nSubIfIdGroup = ctp_entry->ctp;
		if (gsw_ops->gsw_ctp_ops.CTP_PortConfigGet(gsw_ops, &ctpcfg)) {
			pr_err("CTP(%d) CfgGet fail for bp=%d for ep=%d\n",
			       ctp_entry->ctp, bp, portid);
			return DP_FAILURE;
		}
		/* if FlowEntryIndex exists, dont configure */
		if (ctpcfg.nFirstFlowEntryIndex) {
			/* store flow index into BP pmapper table  to use during
			 * register/update subif flow when subblk info is not
			 * valid
			 */
			bp_info->ctp_flow_index = ctpcfg.nFirstFlowEntryIndex;
			continue;
		}
		/* subblk_info is NULL, if CTP config copy is called from
		 * DP register/update subif info
		 */
		if (subblk_info)
			flow_index = subblk_info->subblk_firstidx;
		else
			flow_index = bp_info->ctp_flow_index;
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: set flow entry(%d) into ctp=%d\n",
			 __func__, flow_index, ctp_entry->ctp);
		ctpcfg.nFirstFlowEntryIndex = flow_index;
		ctpcfg.eMask = GSW_CTP_PORT_CONFIG_FLOW_ENTRY;
		if (gsw_ops->gsw_ctp_ops.CTP_PortConfigSet(gsw_ops, &ctpcfg)) {
			pr_err("CTP(%d) CfgSet fail for bp=%d for ep=%d\n",
			       ctp_entry->ctp, bp, portid);
			return DP_FAILURE;
		}
	}
	return DP_SUCCESS;
}

/* Update current PCE rule index with enable/disable */
static int dp_update_pce_rule(struct pce_entry *pentry,
			      struct vlan_entry *ventry,
			      enum DP_PCE_RULE_UPDATE update)
{
	GSW_PCE_rule_t *pce;
	int ret = DP_FAILURE;

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_DBG)
	if (dp_dbg_flag & DP_DBG_FLAG_BR_VLAN) {
		if (update == DP_PCE_RULE_EN)
			DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
				 "%s: %s pce idx (%d)\n",
				 __func__, "Enable", pentry->idx);
		else if (update == DP_PCE_RULE_DIS)
			DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
				 "%s: %s pce idx (%d)\n",
				 __func__, "Disable", pentry->idx);
	}
#endif

	pce = kzalloc(sizeof(*pce), GFP_ATOMIC);
	if (!pce)
		return -1;

	pce->logicalportid = pentry->blk_info->info.portid;
	pce->subifidgroup = pentry->blk_info->info.subif;
	pce->region = pentry->blk_info->region;
	pce->pattern.nIndex = pentry->idx;

	if (dp_pce_rule_get_priv(pentry->blk_info, pce)) {
		pr_err("%s: Get PCE rule %d failed\n",
		       __func__, pce->pattern.nIndex);
		goto EXIT;
	}

	/* PCE struct is reset except for pattern and action after get */
	pce->logicalportid = pentry->blk_info->info.portid;
	pce->subifidgroup = pentry->blk_info->info.subif;
	pce->region = pentry->blk_info->region;
	pce->pattern.nIndex = pentry->idx;

	/* Allow update for VLAN ID and FiD in PCE rule */
	if (update == DP_PCE_RULE_REWRITE) {
		pce->pattern.nSLAN_Vid = ventry->vlan_id;
		pce->action.nFId = ventry->fid;
	}

	if (dp_pce_rule_update_priv(pentry->blk_info, pce, update)) {
		pr_err("%s: Updating PCE rule %d failed\n",
		       __func__, pce->pattern.nIndex);
		goto EXIT;
	}

	if (update == DP_PCE_RULE_EN || update == DP_PCE_RULE_REWRITE)
		pentry->disable = false;
	else if (update == DP_PCE_RULE_DIS)
		pentry->disable = true;

	ret = DP_SUCCESS;
EXIT:
	kfree(pce);
	return ret;
}

/* Delete rule */
static int dp_del_rule(struct vlan_entry *v, struct pce_entry *p)
{
	GSW_PCE_ruleDelete_t pce_del;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: %s pce idx (%d) for vid:%d\n",
		 __func__, "Delete", p->idx, v->vlan_id);

	pce_del.logicalportid = p->blk_info->info.portid;
	pce_del.subifidgroup = p->blk_info->info.subif;
	pce_del.region = p->blk_info->region;
	pce_del.nIndex = p->idx;

	if (dp_pce_rule_del_priv(p->blk_info, &pce_del)) {
		pr_err("%s: %s for pceidx %d Failed\n",
		       __func__, "dp_pce_rule_del_priv", p->idx);
		return DP_FAILURE;
	}
	list_del(&p->list);
	kfree(p->blk_info);
	kfree(p);

	return 0;
}

/* Alloc PCE BLK for CTP region and create a new pce rule */
static int dp_add_rule(struct br_info *br_info, struct vlan_entry *ventry,
		       dp_subif_t *subif)
{
	GSW_PCE_rule_t *pce_rule;
	struct dp_subblk_info *blk_info;
	int idx = 0;
	struct pce_entry *pce = NULL, *pce_entry = NULL;
	struct pmac_port_info *port_info;
	int vap;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: Creating PCE rule Bport %d vlanid %d\n",
		 __func__, subif->bport, ventry->vlan_id);

	pce = kzalloc(sizeof(*pce), GFP_ATOMIC);
	if (!pce)
		return DP_FAILURE;

	pce->blk_info = kzalloc(sizeof(*pce->blk_info), GFP_ATOMIC);
	if (!pce->blk_info) {
		kfree(pce);
		return DP_FAILURE;
	}

	pce_rule = kzalloc(sizeof(*pce_rule), GFP_ATOMIC);
	if (!pce_rule)
		goto ERR_EXIT;

	port_info = get_dp_port_info(subif->inst, subif->port_id);
	vap = GET_VAP(subif->subif, port_info->vap_offset, port_info->vap_mask);

	/* if already have in any vlan enty use the existing pce blk info */
	pce_entry = get_pce_entry_all_ventry(br_info, subif->port_id, vap);
	if (pce_entry)
		memcpy(pce->blk_info, pce_entry->blk_info,
		       sizeof(struct dp_pce_blk_info));

	blk_info = &pce->blk_info->info;
	blk_info->subif = vap;
	blk_info->subblk_size = VLAN_PCE_MAX_SUBBLK_SIZE;
	blk_info->portid = subif->port_id;

	strncpy(blk_info->subblk_name, "dp_br_vlan",
		sizeof(blk_info->subblk_name) - 1);

	pce->blk_info->region = GSW_PCE_RULE_CTP;

	pce_rule->logicalportid = subif->port_id;
	pce_rule->subifidgroup = vap;

	/* pce_rule pattern setting */
	pce_rule->region = GSW_PCE_RULE_CTP;
	pce_rule->pattern.bEnable = 1;
	pce_rule->pattern.bSubIfIdEnable = 1;
	pce_rule->pattern.eSubIfIdType = GSW_PCE_SUBIFID_TYPE_BRIDGEPORT;
	pce_rule->pattern.nSubIfId = subif->bport;
	pce_rule->pattern.bSLAN_Vid = 1;
	pce_rule->pattern.nSLAN_Vid = ventry->vlan_id;

	/* action setting */
	pce_rule->action.bFidEnable = 1;
	pce_rule->action.nFId = ventry->fid;

	idx = dp_pce_rule_add_priv(pce->blk_info, pce_rule);
	kfree(pce_rule);

	if (idx < 0) {
		pr_err("%s: dp_pce_rule_add_priv for Vlan %d port %d failed\n",
		       __func__, ventry->vlan_id, subif->port_id);
		goto ERR_EXIT;
	}
	if (subif->flag_pmapper) {
		/* copy the firstflow entry index to all pmapper CTP */
		if (dp_gsw_copy_ctp_cfg(subif->inst, subif->bport, blk_info,
					subif->port_id)) {
			pr_err("%s: gsw_ctp_cfg copy bp=%d failed\n",
			       __func__, subif->bport);
			goto ERR_EXIT;
		}
	}

	pce->idx = idx;
	list_add(&pce->list, &ventry->pce_list);

	/* If FiD is not yet created mark this PCE rule as disabled */
	if (!ventry->fid) {
		if (dp_update_pce_rule(pce, ventry, DP_PCE_RULE_DIS)) {
			dp_del_rule(ventry, pce);
			return DP_FAILURE;
		}
	}

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: For vlan_id %d added PCE Rule %d and Status (%s)\n",
		 __func__, ventry->vlan_id, pce->idx,
		 pce->disable ? "Disable" : "Enable");

	return idx;

ERR_EXIT:
	kfree(pce->blk_info);
	kfree(pce);

	return -1;
}

/* Add a new pce rule, If new vlan FID is created
 * For Master go through the exact bp and add rule or enable rule
 * For Self go through all the pce entries and Enable all entries
 * in the vlan entry
 * subif is NULL for Self
 */
static int dp_add_vlan_pce_rule(struct vlan_entry *ventry,
				struct br_info *br_info, dp_subif_t *subif,
				int flags)
{
	struct pce_entry *p;

	if (flags & BRIDGE_FLAGS_MASTER) {
		/* Add new pce rule for this bridge port/Enable it */
		p = get_pce_entry_from_bp(br_info, subif, ventry->vlan_id);
		if (!p) {
			if (dp_add_rule(br_info, ventry, subif) < 0)
				return DP_FAILURE;
		} else {
			if (p->disable) {
				if (dp_update_pce_rule(p, ventry,
						       DP_PCE_RULE_EN))
					return DP_FAILURE;
			}
		}
	} else if (flags & BRIDGE_FLAGS_SELF) {
		list_for_each_entry(p, &ventry->pce_list, list) {
			if (!p->blk_info)
				continue;
			if (p->disable) {
				/* There is a possibility that FiD is generated
				 * later so need to rewrite the rule to update
				 * fid
				 */
				if (dp_update_pce_rule(p, ventry,
						       DP_PCE_RULE_REWRITE))
					return DP_FAILURE;
			}
		}
	}
	return DP_SUCCESS;
}

/* Add a VLAN Entry, If already have in BP or Bridge, reuse the VLAN entry
 * In 1 Bridge Each VLAN have only 1 uniqueue FiD
 * NewFID = VlanID + BridgeID
 */
static struct vlan_entry *dp_add_vlan_entry(struct br_info *br_info,
		struct vlist_entry *br_ventry,
		struct vlist_entry *bp_ventry,
		dp_subif_t *subif,
		u16 vlanid, int flags)
{
	struct vlist_entry *vlist_entry, *curr_bp_ventry = NULL;
	struct vlan_entry *ventry;
	struct list_head *vhead;
	int fid = 0;
	bool alloc_fid = false;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: dev=%s vlanid=%d flags=%08x\n", __func__,
		 br_info->dev->name, vlanid, flags);

	if (flags & BRIDGE_FLAGS_MASTER)
		curr_bp_ventry = get_vlist_entry_in_bp(br_info, subif->bport,
						       vlanid, 0);
	if (br_ventry && bp_ventry && curr_bp_ventry) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: No Need to add both br & bport have the entry\n",
			 __func__);
		return DP_SUCCESS;
	}

	vlist_entry = devm_kzalloc(&g_dp_dev->dev,
				   sizeof(struct vlist_entry), GFP_ATOMIC);
	if (!vlist_entry)
		return NULL;

	/* if bridge or bridge member port have the VLAN Entry
	 * Add to the corresponding list increase ref_cnt and return Entry
	 */
	if (flags & BRIDGE_FLAGS_SELF)
		vhead = &br_info->br_vlan_list;
	else
		vhead = get_vlan_entry_head_in_bp(br_info, subif->bport);

	if (!vhead) {
		devm_kfree(&g_dp_dev->dev, vlist_entry);
		return NULL;
	}

	/* Bridge Port or Bridge dont have the VLAN Entry allocate a new one */
	if (!br_ventry && !bp_ventry) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: Allocating a new entry\n",
			 __func__);
		ventry = devm_kzalloc(&g_dp_dev->dev,
				      sizeof(struct vlan_entry), GFP_ATOMIC);
		if (!ventry) {
			devm_kfree(&g_dp_dev->dev, vlist_entry);
			return NULL;
		}

		vlist_entry->vlan_entry = ventry;
		vlist_entry->vlan_entry->vlan_id = vlanid;
		INIT_LIST_HEAD(&vlist_entry->vlan_entry->pce_list);
		/* BR already have, now going to create BP */
	} else if (br_ventry && !bp_ventry) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: Allocating a BP entry\n",
			 __func__);
		vlist_entry->vlan_entry = br_ventry->vlan_entry;

		if (!vlist_entry->vlan_entry->fid)
			alloc_fid = true;

		/* Some BP already have, now we are going to create BR */
	} else if (!br_ventry && bp_ventry) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: Allocating a BR entry\n",
			 __func__);
		vlist_entry->vlan_entry = bp_ventry->vlan_entry;

		if (flags & BRIDGE_FLAGS_SELF && !vlist_entry->vlan_entry->fid)
			alloc_fid = true;

		/* BR already have, Some other BP in br have,
		 * but current BP don't have
		 */
	} else if (br_ventry && bp_ventry && !curr_bp_ventry) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: Allocating a bp entry for current bp\n",
			 __func__);
		vlist_entry->vlan_entry = br_ventry->vlan_entry;

		if (!vlist_entry->vlan_entry->fid)
			alloc_fid = true;
	} else {
		pr_err("%s: Impossible alloc both BP and BR have VLAN Entry\n",
		       __func__);
		devm_kfree(&g_dp_dev->dev, vlist_entry);
		return NULL;
	}

	/* FiD is created only when 1 Self + 1 Master */
	if (alloc_fid) {
		/* Allocate a unique FID for this VLAN ID */
		fid = allocate_br_vlan_fid(br_info, vlanid);
		if (fid < 0) {
			devm_kfree(&g_dp_dev->dev, vlist_entry);
			return NULL;
		}
		vlist_entry->vlan_entry->fid = fid;
	}
	vlist_entry->vlan_entry->ref_cnt++;
	list_add(&vlist_entry->list, vhead);

	/* Add new pce rule for this vid if not created this will be added
	 * only for a bridge member port
	 */
	if (dp_add_vlan_pce_rule(vlist_entry->vlan_entry, br_info, subif,
				 flags)) {
		vlist_entry->vlan_entry->ref_cnt--;
		list_del(&vlist_entry->list);
		devm_kfree(&g_dp_dev->dev, vlist_entry);
		pr_err("%s: dp_add_vlan_pce_rule: Failed\n", __func__);
		return NULL;
	}

	return vlist_entry->vlan_entry;
}

/* Delete a pce rule
 * For Master go through the exact bp and delete rule or disable rule
 * For Self go through all the pce entries and disable or delete rule
 * subif is NULL for Self
 */
static int dp_del_vlan_pce_rule(struct vlan_entry *ventry,
				struct br_info *br_info, dp_subif_t *subif,
				int flags)
{
	struct pce_entry *p;
	struct list_head *t, *q;

	if (flags & BRIDGE_FLAGS_MASTER) {
		p = get_pce_entry_from_bp(br_info, subif, ventry->vlan_id);
		if (!p)
			return DP_SUCCESS;

		/* Disable the PCE rule if it is enabled else delete */
		if (!p->disable) {
			if (dp_update_pce_rule(p, ventry, DP_PCE_RULE_DIS))
				return DP_FAILURE;
		} else {
			if (dp_del_rule(ventry, p))
				return DP_FAILURE;
		}
	} else if (flags & BRIDGE_FLAGS_SELF) {
		/* Delete all the pce rules which is not monitored by anyone */
		list_for_each_safe(q, t, &ventry->pce_list) {
			p = list_entry(q, struct pce_entry, list);
			if (!p->blk_info)
				continue;
			if (p->disable) {
				if (dp_del_rule(ventry, p))
					return DP_FAILURE;
			}
		}
		/* Disable all the PCE rules which are active */
		list_for_each_entry(p, &ventry->pce_list, list) {
			if (!p->blk_info)
				continue;
			if (!p->disable) {
				if (dp_update_pce_rule(p, ventry,
						       DP_PCE_RULE_DIS))
					return DP_FAILURE;
			}
		}
	}
	return DP_SUCCESS;
}

/* Deleting a bridge member VLAN Entry and FID
 * Bport=bp only in case of Master
 * Bport=0 only in case of Self
 */
static int dp_del_vlan_entry(struct br_info *br_info,
			     struct vlist_entry *vlist_entry, dp_subif_t *subif,
			     u16 vlanid, int flags)
{
	struct vlan_entry *ventry;

	/* if Entry or Ref Count is not present this VLAN ID doesn't exist */
	if (!vlist_entry || !vlist_entry->vlan_entry ||
	    !vlist_entry->vlan_entry->ref_cnt) {
		pr_err("%s: VLAN ID %d is not present\n", __func__, vlanid);
		return DP_SUCCESS;
	}

	ventry = vlist_entry->vlan_entry;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: %s entry in Br=%s vlanid=%d ref_cnt=%d\n",
		 __func__, (ventry->ref_cnt > 1) ? "Disabling" : "Deleting",
		 br_info->dev->name, ventry->vlan_id,
		 ventry->ref_cnt);

	/* Disable pce rule index for this vid */
	if (dp_del_vlan_pce_rule(ventry, br_info, subif, flags)) {
		pr_err("%s: %s: Br=%s Bp=%d vlanid=%d Failed\n",
		       __func__, "dp_del_vlan_pce_rule",
		       br_info->dev->name, subif ? subif->bport : 0,
		       ventry->vlan_id);
		return DP_FAILURE;
	}

	/* if usage > 1, Cannot delete now!! decrease ref count and delete the
	 * entry from list of either br or bridge member
	 */
	if (ventry->ref_cnt > 1) {
		ventry->ref_cnt--;
		list_del(&vlist_entry->list);
		devm_kfree(&g_dp_dev->dev, vlist_entry);
		return DP_SUCCESS;
	}

	/* Deallocate VLAN aware FiD, if FiD is > 0 */
	if (ventry->fid) {
		if (deallocate_br_vlan_fid(br_info, vlanid, ventry->fid)) {
			pr_err("%s: %s: Br=%s Bp=%d vlanid=%d Failed\n",
			       __func__, "deallocate_br_vlan_fid",
			       br_info->dev->name, subif ? subif->bport : 0,
			       ventry->vlan_id);
			return DP_FAILURE;
		}
	}

	/* Decrease ref count and delete the VLAN ID Entry */
	ventry->ref_cnt--;
	list_del(&vlist_entry->list);
	devm_kfree(&g_dp_dev->dev, ventry);
	devm_kfree(&g_dp_dev->dev, vlist_entry);

	br_info->num_vlan--;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: Vlan ID %d removed on bridge %s\n",
		 __func__, vlanid, br_info->dev->name);
	return DP_SUCCESS;
}

/* Self is called for bridge interface to assign VLAN ID */
static int dp_register_self(struct br_info *br_info, u16 vlanid,
			    int flags)
{
	int ret = DP_FAILURE;
	struct vlan_entry *ventry;
	struct vlist_entry *vlist_entry, *br_ventry, *bp_ventry;

	/* if not bridge return error */
	if (!netif_is_bridge_master(br_info->dev))
		return ret;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: dev=%s vlanid=%d flags=%08x\n", __func__,
		 br_info->dev->name, vlanid, flags);

	if (flags & BR_VLAN_DEREGISTER) {
		vlist_entry = get_vlist_entry_in_br(br_info, vlanid, 0);
		ret = dp_del_vlan_entry(br_info, vlist_entry, 0, vlanid,
					BRIDGE_FLAGS_SELF);
		goto EXIT;
	}

	/* Dont allow multiple commands to add multiple entries in list */
	br_ventry = get_vlist_entry_in_br(br_info, vlanid, 0);
	if (br_ventry) {
		pr_info("%s: Vlan ID %d is already present in Br %s\n",
			__func__, vlanid, br_info->dev->name);
		goto EXIT;
	}

	bp_ventry = get_vlist_entry_in_all_bp(br_info, vlanid);

	/* if not present will allocate a new one */
	ventry = dp_add_vlan_entry(br_info, br_ventry, bp_ventry, 0, vlanid,
				   BRIDGE_FLAGS_SELF);
	if (!ventry) {
		pr_err("%s: Failed to Add vland id %d to br %s\n",
		       __func__, vlanid, br_info->dev->name);
		goto EXIT;
	}
	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: VLAN Id %d added for bridge %s Fid %d ref_cnt %d\n",
		 __func__, ventry->vlan_id, br_info->dev->name, ventry->fid,
		 ventry->ref_cnt);

	br_info->num_vlan++;
	ret = DP_SUCCESS;
EXIT:
	return ret;
}

/* This API will be called for Bridge Member ports */
static int dp_register_master(struct br_info *br_info,
			      struct net_device *dev, u16 vlanid, int flags)
{
	dp_subif_t subif;
	struct vlan_entry *ventry;
	int ret = DP_FAILURE;
	struct vlist_entry *vlist_entry, *br_ventry, *bp_ventry;

	if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
		pr_err("%s: dp_get_netif_subifid failed\n", __func__);
		return DP_FAILURE;
	}

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: Br=%s bport=%d vlanid=%d flags=%08x\n", __func__,
		 br_info->dev->name, subif.bport, vlanid, flags);

	if (flags & BR_VLAN_DEREGISTER) {
		vlist_entry = get_vlist_entry_in_bp(br_info, subif.bport,
						    vlanid, 0);
		ret = dp_del_vlan_entry(br_info, vlist_entry, &subif,
					vlanid, BRIDGE_FLAGS_MASTER);
		goto EXIT;
	}

	/* Dont allow multiple commands to add multiple entries in list */
	bp_ventry = get_vlist_entry_in_bp(br_info, subif.bport, vlanid, 0);
	if (bp_ventry) {
		pr_info("%s: Vlan ID %d is already present in Bp %d of Br %s\n",
			__func__, vlanid, subif.bport, br_info->dev->name);
		goto EXIT;
	}

	bp_ventry = get_vlist_entry_in_all_bp(br_info, vlanid);
	br_ventry = get_vlist_entry_in_br(br_info, vlanid, 0);

	/* For every member port bport will be unique */
	ventry = dp_add_vlan_entry(br_info, br_ventry, bp_ventry,
				   &subif, vlanid, BRIDGE_FLAGS_MASTER);
	if (!ventry) {
		pr_err("%s: Failed to Add vland id %d to dev %s in br %s\n",
		       __func__, vlanid, dev->name, br_info->dev->name);
		goto EXIT;
	}

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: Vlan ID %d Enabled Iface=%s br=%s NewFiD=%d RefCnt=%d\n",
		 __func__, ventry->vlan_id, dev->name, br_info->dev->name,
		 ventry->fid, ventry->ref_cnt);

	ret = DP_SUCCESS;
EXIT:
	return ret;
}

/* Parse netlink message to get VLAN ID and Bridge Flags for Master and Self */
static int parse_netlink_msg(struct nlmsghdr *nlh, u16 *vid,
			     u16 *br_flags)
{
	struct nlattr *attr, *br_spec, *protinfo;
	int rem;
	struct bridge_vlan_info *vinfo = NULL;

	protinfo = nlmsg_find_attr(nlh, sizeof(struct ifinfomsg),
				   IFLA_PROTINFO);
	if (protinfo)
		return DP_SUCCESS;

	br_spec = nlmsg_find_attr(nlh, sizeof(struct ifinfomsg), IFLA_AF_SPEC);
	if (!br_spec)
		return -EINVAL;

	nla_for_each_nested(attr, br_spec, rem) {
		if ((nla_type(attr) != IFLA_BRIDGE_FLAGS) &&
		    (nla_type(attr) != IFLA_BRIDGE_VLAN_INFO))
			continue;

		if (nla_type(attr) == IFLA_BRIDGE_VLAN_INFO) {
			if (nla_len(attr) != sizeof(struct bridge_vlan_info))
				return -EINVAL;
			vinfo = nla_data(attr);
			if (vinfo)
				*vid = vinfo->vid;
		}

		if (nla_type(attr) == IFLA_BRIDGE_FLAGS) {
			if (nla_len(attr) < sizeof(u16))
				return -EINVAL;
			*br_flags = nla_get_u16(attr);
		}
	}

	return DP_SUCCESS;
}

/* This will be called for bridge add dev eth0 vid 100
 * By this time bridge interface is added to bridge
 */
int dp_ndo_bridge_setlink(struct net_device *dev, struct nlmsghdr *nlh,
			  u16 flags)
{
	u16 vid = 0;
	u16 br_flags = 0;
	struct net_device *br_dev;
	struct br_info *br_info;
	int ret = 0;
	struct dp_dev *dp_dev = NULL;

	if (!dev || !nlh)
		return -EINVAL;
	DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: dev=%s\n", __func__,
		 dev ? dev->name : "NULL");

	if (netif_is_bridge_master(dev))
		br_dev = dev;
	else if (netif_is_bridge_port(dev))
		br_dev = netdev_master_upper_dev_get(dev);
	else
		return -EOPNOTSUPP;

	if (parse_netlink_msg(nlh, &vid, &br_flags)) {
		pr_err("%s: Error in parsing netlink message\n", __func__);
		return -EINVAL;
	}

	DP_LIB_LOCK(&dp_lock);
	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev) {
		pr_err("%s: dp_dev NULL\n", __func__);
		ret = -EINVAL;
		goto EXIT;
	}

	/* if Vland ID and Bridge Flags are not set return the original ops */
	if (!vid && !br_flags) {
		if (dp_dev->old_dev_ops->ndo_bridge_setlink)
			ret = dp_dev->old_dev_ops->ndo_bridge_setlink(dev,
					nlh, flags);
		else
			ret = -EFAULT;
		goto EXIT;
	}

	br_info = dp_swdev_bridge_entry_lookup_rcu(br_dev);
	if (!br_info) {
		pr_err("%s: dev %s is not registered to DPM\n", __func__,
		       br_dev->name);
		ret = -EINVAL;
		goto EXIT;
	}

	if (!br_info->br_vlan_en) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: br=%s is not VLAN Aware\n",
			 __func__, br_dev->name);
		ret = -EINVAL;
		goto EXIT;
	}

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: dev=%s Flags=%s VlandID=%d\n",
		 __func__, br_dev->name,
		 (br_flags & BRIDGE_FLAGS_SELF) ? "Self" : "Master",
		 vid);

	/* if Br Flags is not passed considering it as Master */
	if (br_flags & BRIDGE_FLAGS_SELF)
		dp_register_self(br_info, vid, 0);
	else
		dp_register_master(br_info, dev, vid, 0);

	ret = 0;
EXIT:
	DP_LIB_UNLOCK(&dp_lock);
	return ret;
}

int dp_ndo_bridge_dellink(struct net_device *dev, struct nlmsghdr *nlh,
			  u16 flags)
{
	int ret = DP_SUCCESS;
	u16 vid = 0;
	u16 br_flags = 0;
	struct net_device *br_dev;
	struct br_info *br_info;
	struct dp_dev *dp_dev = NULL;

	if (!dev || !nlh)
		return -EINVAL;

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: dev=%s\n", __func__,
		 dev ? dev->name : "NULL");

	if (netif_is_bridge_master(dev))
		br_dev = dev;
	else if (netif_is_bridge_port(dev))
		br_dev = netdev_master_upper_dev_get(dev);
	else
		return -EOPNOTSUPP;

	if (parse_netlink_msg(nlh, &vid, &br_flags)) {
		pr_err("%s: Error in parsing netlink message\n", __func__);
		return -EINVAL;
	}

	DP_LIB_LOCK(&dp_lock);
	dp_dev = dp_dev_lookup(br_dev);
	if (!dp_dev) {
		pr_err("%s: dp_dev NULL\n", __func__);
		ret = -EINVAL;
		goto EXIT;
	}

	/* if Vland ID and Bridge Flags are not set return the original ops */
	if (!vid && !br_flags) {
		if (dp_dev->old_dev_ops->ndo_bridge_dellink)
			ret = dp_dev->old_dev_ops->ndo_bridge_dellink(br_dev,
					nlh, flags);
		else
			ret = -EFAULT;
		goto EXIT;
	}

	br_info = dp_swdev_bridge_entry_lookup_rcu(br_dev);
	if (!br_info) {
		pr_err("%s: dev %s is not registered to DPM\n", __func__,
		       br_dev->name);
		ret = -EINVAL;
		goto EXIT;
	}

	if (!br_info->br_vlan_en) {
		DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
			 "%s: br=%s is not VLAN Aware\n",
			 __func__, br_dev->name);
		ret = -EINVAL;
		goto EXIT;
	}

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN, "%s: dev=%s Flags=%s VlandID=%d\n",
		 __func__, dev->name,
		 br_flags & BRIDGE_FLAGS_SELF ? "Self" : "Master",
		 vid);

	if (br_flags & BRIDGE_FLAGS_SELF)
		dp_register_self(br_info, vid, BR_VLAN_DEREGISTER);
	else
		dp_register_master(br_info, dev, vid, BR_VLAN_DEREGISTER);

	ret = 0;
EXIT:
	DP_LIB_UNLOCK(&dp_lock);
	return ret;
}

/* FiD for VLAN can be only 6 bits, Here we need to know
 * maxvlanid - Max VLAN ID supported per Bridge
 * br_dev: bridge device
 */
int dp_set_br_vlan_limit(struct net_device *br_dev, u32 maxvlanid)
{
	int ret = DP_FAILURE;
	struct br_info *br_info;

	if (unlikely(!dp_init_ok)) {
		pr_err("%s failed: datapath not initialized yet\n", __func__);
		return DP_FAILURE;
	}

	if ((maxvlanid > MAX_FID) || (maxvlanid == 0)) {
		pr_err("VLAN Aware support max 64 FiD (VlanID + BriID)\n");
		return DP_FAILURE;
	}

	if (!netif_is_bridge_master(br_dev)) {
		pr_err("%s: br vlan limit set for wrong dev %s\n",
		       __func__, br_dev->name);
		return DP_FAILURE;
	}

	DP_LIB_LOCK(&dp_lock);
	br_info = dp_swdev_bridge_entry_lookup_rcu(br_dev);
	if (!br_info) {
		pr_err("%s: dev %s is not registered to DPM\n", __func__,
		       br_dev->name);
		goto EXIT;
	}

	br_info->max_vlan_limit = maxvlanid;
	br_info->max_brid_limit =
			1 << ((ilog2(MAX_FID) - ilog2(maxvlanid)) - 1);

	DP_DEBUG(DP_DBG_FLAG_BR_VLAN,
		 "%s: max_vlan_limit=%d max_brid_limit=%d\n",
		 __func__, br_info->max_vlan_limit, br_info->max_vlan_limit);

	ret = 0;
EXIT:
	DP_LIB_UNLOCK(&dp_lock);
	return ret;

}
EXPORT_SYMBOL(dp_set_br_vlan_limit);
