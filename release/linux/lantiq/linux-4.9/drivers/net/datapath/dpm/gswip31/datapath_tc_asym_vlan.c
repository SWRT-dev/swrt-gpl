/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <net/switch_api/gsw_flow_ops.h>
#include <net/datapath_api.h>
#include <net/datapath_api_vlan.h>
#include "../datapath.h"
/***************************
 *	Code for TC VLAN
 ***************************/

static int update_bp(struct core_ops *ops,
		     u32 bpid,
		     int ingress,
		     GSW_VLANFILTER_alloc_t *pfilter,
		     GSW_EXTENDEDVLAN_alloc_t *pextvlan)
{
	int ret;
	GSW_BRIDGE_portConfig_t bpcfg1, bpcfg2;

	memset(&bpcfg1, 0, sizeof(bpcfg1));
	memset(&bpcfg2, 0, sizeof(bpcfg2));
	bpcfg1.nBridgePortId = bpid;
	bpcfg2.nBridgePortId = bpid;
	if (ingress) {
		bpcfg1.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN_FILTER
				| GSW_BRIDGE_PORT_CONFIG_MASK_INGRESS_VLAN;
		if (!pfilter) {
			bpcfg2.bIngressVlanFilterEnable = LTQ_FALSE;
		} else {
			bpcfg2.bIngressVlanFilterEnable = LTQ_TRUE;
			bpcfg2.nIngressVlanFilterBlockId =
				pfilter->nVlanFilterBlockId;
			bpcfg2.nIngressVlanFilterBlockSize = 0;
		}
		if (!pextvlan) {
			bpcfg2.bIngressExtendedVlanEnable = LTQ_FALSE;
		} else {
			bpcfg2.bIngressExtendedVlanEnable = LTQ_TRUE;
			bpcfg2.nIngressExtendedVlanBlockId =
				pextvlan->nExtendedVlanBlockId;
			bpcfg2.nIngressExtendedVlanBlockSize = 0;
		}
	} else {
		bpcfg1.eMask = GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN_FILTER1
				| GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN;
		if (!pfilter) {
			bpcfg2.bEgressVlanFilter1Enable = LTQ_FALSE;
		} else {
			bpcfg2.bEgressVlanFilter1Enable = LTQ_TRUE;
			bpcfg2.nEgressVlanFilter1BlockId =
				pfilter->nVlanFilterBlockId;
			bpcfg2.nEgressVlanFilter1BlockSize = 0;
		}
		if (!pextvlan) {
			bpcfg2.bEgressExtendedVlanEnable = LTQ_FALSE;
		} else {
			bpcfg2.bEgressExtendedVlanEnable = LTQ_TRUE;
			bpcfg2.nEgressExtendedVlanBlockId =
				pextvlan->nExtendedVlanBlockId;
			bpcfg2.nEgressExtendedVlanBlockSize = 0;
		}
	}
	bpcfg2.eMask = bpcfg1.eMask;

	ret = ops->gsw_brdgport_ops.BridgePort_ConfigGet(ops, &bpcfg1);
	if (ret != GSW_statusOk)
		return -EIO;

	ret = ops->gsw_brdgport_ops.BridgePort_ConfigSet(ops, &bpcfg2);
	if (ret != GSW_statusOk)
		return -EIO;

	if (ingress) {
		if (bpcfg1.bIngressVlanFilterEnable != LTQ_FALSE) {
			GSW_VLANFILTER_alloc_t alloc = {0};

			alloc.nVlanFilterBlockId =
				bpcfg1.nIngressVlanFilterBlockId;
			ops->gsw_vlanfilter_ops.VlanFilter_Free(ops, &alloc);
		}
		if (bpcfg1.bIngressExtendedVlanEnable != LTQ_FALSE) {
			GSW_EXTENDEDVLAN_alloc_t alloc = {0};

			alloc.nExtendedVlanBlockId =
				bpcfg1.nIngressExtendedVlanBlockId;
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		}
	} else {
		if (bpcfg1.bEgressVlanFilter1Enable != LTQ_FALSE) {
			GSW_VLANFILTER_alloc_t alloc = {0};

			alloc.nVlanFilterBlockId =
				bpcfg1.nEgressVlanFilter1BlockId;
			ops->gsw_vlanfilter_ops.VlanFilter_Free(ops, &alloc);
		}
		if (bpcfg1.bEgressExtendedVlanEnable != LTQ_FALSE) {
			GSW_EXTENDEDVLAN_alloc_t alloc = {0};

			alloc.nExtendedVlanBlockId =
				bpcfg1.nEgressExtendedVlanBlockId;
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		}
	}

	return 0;
}

static int update_ctp(struct core_ops *ops,
		      u32 lpid,
		      u32 subifidg,
		      int ingress,
		      int multicast,
		      GSW_EXTENDEDVLAN_alloc_t *pextvlan)
{
	int ret;
	GSW_CTP_portConfig_t ctpcfg1 = {0}, ctpcfg2 = {0};

	ctpcfg1.nLogicalPortId = lpid;
	ctpcfg2.nLogicalPortId = lpid;
	ctpcfg1.nSubIfIdGroup = subifidg;
	ctpcfg2.nSubIfIdGroup = subifidg;
	if (ingress) {
		if (~multicast & BIT(2)) {
			ctpcfg1.eMask |=
				GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN_IGMP;
			if (!pextvlan) {
				ctpcfg2.bIngressExtendedVlanIgmpEnable =
								LTQ_FALSE;
			} else {
				ctpcfg2.bIngressExtendedVlanIgmpEnable =
								LTQ_TRUE;
				ctpcfg2.nIngressExtendedVlanBlockIdIgmp =
					pextvlan->nExtendedVlanBlockId;
				ctpcfg2.nIngressExtendedVlanBlockSizeIgmp = 0;
			}
		}
		if (~multicast & BIT(1)) {
			ctpcfg1.eMask |= GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN;
			if (!pextvlan) {
				ctpcfg2.bIngressExtendedVlanEnable = LTQ_FALSE;
			} else {
				ctpcfg2.bIngressExtendedVlanEnable = LTQ_TRUE;
				ctpcfg2.nIngressExtendedVlanBlockId =
					pextvlan->nExtendedVlanBlockId;
				ctpcfg2.nIngressExtendedVlanBlockSize = 0;
			}
		}
	} else {
		if (~multicast & BIT(2)) {
			ctpcfg1.eMask |=
				GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP;
			if (!pextvlan) {
				ctpcfg2.bEgressExtendedVlanIgmpEnable =
								LTQ_FALSE;
			} else {
				ctpcfg2.bEgressExtendedVlanIgmpEnable =
								LTQ_TRUE;
				ctpcfg2.nEgressExtendedVlanBlockIdIgmp =
					pextvlan->nExtendedVlanBlockId;
				ctpcfg2.nEgressExtendedVlanBlockSizeIgmp = 0;
			}
		}
		if (~multicast & BIT(1)) {
			ctpcfg1.eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN;
			if (!pextvlan) {
				ctpcfg2.bEgressExtendedVlanEnable = LTQ_FALSE;
			} else {
				ctpcfg2.bEgressExtendedVlanEnable = LTQ_TRUE;
				ctpcfg2.nEgressExtendedVlanBlockId =
					pextvlan->nExtendedVlanBlockId;
				ctpcfg2.nEgressExtendedVlanBlockSize = 0;
			}
		}
	}
	ctpcfg2.eMask = ctpcfg1.eMask;

	ret = ops->gsw_ctp_ops.CTP_PortConfigGet(ops, &ctpcfg1);
	if (ret != GSW_statusOk)
		return -EIO;

	ret = ops->gsw_ctp_ops.CTP_PortConfigSet(ops, &ctpcfg2);
	if (ret != GSW_statusOk)
		return -EIO;

	if (ingress) {
		u32 block = ~0;

		if ((~multicast & BIT(2))
		    && ctpcfg1.bIngressExtendedVlanIgmpEnable != LTQ_FALSE) {
			GSW_EXTENDEDVLAN_alloc_t alloc = {0};

			block = ctpcfg1.nIngressExtendedVlanBlockIdIgmp;
			alloc.nExtendedVlanBlockId = block;
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		}
		if ((~multicast & BIT(1))
		    && ctpcfg1.bIngressExtendedVlanEnable != LTQ_FALSE
		    && block != ctpcfg1.nIngressExtendedVlanBlockId) {
			GSW_EXTENDEDVLAN_alloc_t alloc = {0};

			alloc.nExtendedVlanBlockId =
				ctpcfg1.nIngressExtendedVlanBlockId;
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		}
	} else {
		u32 block = ~0;

		if ((~multicast & BIT(2))
		    && ctpcfg1.bEgressExtendedVlanIgmpEnable != LTQ_FALSE) {
			GSW_EXTENDEDVLAN_alloc_t alloc = {0};

			block = ctpcfg1.nEgressExtendedVlanBlockIdIgmp;
			alloc.nExtendedVlanBlockId = block;
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		}
		if ((~multicast & BIT(1))
		    && ctpcfg1.bEgressExtendedVlanEnable != LTQ_FALSE
		    && block != ctpcfg1.nEgressExtendedVlanBlockId) {
			GSW_EXTENDEDVLAN_alloc_t alloc = {0};

			alloc.nExtendedVlanBlockId =
				ctpcfg1.nEgressExtendedVlanBlockId;
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		}
	}

	return 0;
}

static int vlan_filter_mode(struct dp_pattern_vlan *pout)
{
	int k;

	k = 0;
	if (pout->tpid != DP_VLAN_PATTERN_NOT_CARE)
		k |= 1 << 4;
	if (pout->proto != DP_VLAN_PATTERN_NOT_CARE)
		k |= 1 << 3;
	if (pout->dei != DP_VLAN_PATTERN_NOT_CARE)
		k |= 1 << 2;
	if (pout->prio != DP_VLAN_PATTERN_NOT_CARE)
		k |= 1 << 1;
	if (pout->vid != DP_VLAN_PATTERN_NOT_CARE)
		k |= 1;

	return k;
}

static int tc_vlan_filter(struct core_ops *ops,
			  struct dp_tc_vlan *vlan,
			  struct dp_tc_vlan_info *info)
{
	/* default return 1 to indicate Extended VLAN is required */
	int ret = 1;
	int total = vlan->n_vlan0 + vlan->n_vlan1 + vlan->n_vlan2;
	int untagged = -1;
	int tagged = -1;
	int mode = -1;
	GSW_VLANFILTER_alloc_t alloc;
	GSW_VLANFILTER_config_t *pcfg;
	int i, j, k;

	if (total <= 0) {
		/* Update bridge port */
		ret = update_bp(ops,
				(u32)info->bp,
				vlan->dir == DP_DIR_INGRESS,
				NULL,
				NULL);
		return ret;
	}

	/* bridge port */
	pcfg = kmalloc_array(total, sizeof(*pcfg), GFP_KERNEL);
	if (!pcfg)
		return -ENOMEM;
	memset(pcfg, 0, sizeof(*pcfg) * total);

	j = 0;

	/* untagged rule */
	for (i = 0; i < vlan->n_vlan0; i++) {
		if (!vlan->vlan0_list[i].def)
		/* VLAN filter for untagged packet have default rule only */
			goto EXIT;
		if ((vlan->vlan0_list[i].act.act & DP_VLAN_ACT_FWD)) {
		/* default value was set to drop */
			if (untagged > 0)
				goto EXIT;
		/* default value is forward */
			untagged = 0;
		} else if ((vlan->vlan0_list[i].act.act & DP_VLAN_ACT_DROP)) {
		/* default value was set to forward */
			if (untagged == 0)
				goto EXIT;
		/* default value is drop */
			untagged = 1;
		} else
		/* packet editing required */
			goto EXIT;
		continue;
	}

	/* 1-tag rule */
	for (i = 0; i < vlan->n_vlan1; i++) {
		if (vlan->vlan1_list[i].def) {
			if ((vlan->vlan1_list[i].act.act & DP_VLAN_ACT_FWD)) {
			/* default value was set to drop */
				if (tagged > 0)
					goto EXIT;
				tagged = 0;
			} else if ((vlan->vlan1_list[i].act.act &
				    DP_VLAN_ACT_DROP)) {
			/* default value was set to forward */
				if (tagged == 0)
					goto EXIT;
				tagged = 1;
			} else
			/* packet editing required */
				goto EXIT;
			continue;
		}
		/* Action other than FWD/DROP is not accepted */
		if ((vlan->vlan1_list[i].act.act & DP_VLAN_ACT_FWD))
			pcfg[j].bDiscardMatched = LTQ_FALSE;
		else if ((vlan->vlan1_list[i].act.act & DP_VLAN_ACT_DROP))
			pcfg[j].bDiscardMatched = LTQ_TRUE;
		else
			goto EXIT;
		/* VLAN filter only support VID, PCP, or TCI only */
		k = vlan_filter_mode(&vlan->vlan1_list[i].outer);
		switch (k) {
		case 1:
			pcfg[j].eVlanFilterMask = GSW_VLAN_FILTER_TCI_MASK_VID;
			pcfg[j].nVal = (u32)vlan->vlan1_list[i].outer.vid;
			break;
		case 2:
			pcfg[j].eVlanFilterMask = GSW_VLAN_FILTER_TCI_MASK_PCP;
			pcfg[j].nVal = (u32)vlan->vlan1_list[i].outer.prio;
			break;
		case 7:
			pcfg[j].eVlanFilterMask = GSW_VLAN_FILTER_TCI_MASK_TCI;
			pcfg[j].nVal =
				(u32)vlan->vlan1_list[i].outer.vid & 0xFFF;
			pcfg[j].nVal |=
				((u32)vlan->vlan1_list[i].outer.prio & 7) << 13;
			pcfg[j].nVal |=
				((u32)vlan->vlan1_list[i].outer.dei & 1) << 12;
			break;
		default:
			goto EXIT;
		}
		if (mode < 0)
			mode = k;
		else if (mode != k)
			goto EXIT;
		j++;
	}

	/* 2-tag rule */
	for (i = 0; i < vlan->n_vlan2; i++) {
		if (vlan->vlan2_list[i].def) {
			if ((vlan->vlan2_list[i].act.act & DP_VLAN_ACT_FWD)) {
				if (tagged > 0)
					goto EXIT;
				tagged = 0;
			} else if ((vlan->vlan2_list[i].act.act &
				    DP_VLAN_ACT_DROP)) {
				if (tagged == 0)
					goto EXIT;
				tagged = 1;
			} else {
				goto EXIT;
			}
			continue;
		}
		/* Action other than FWD/DROP is not accepted */
		if ((vlan->vlan2_list[i].act.act & DP_VLAN_ACT_FWD))
			pcfg[j].bDiscardMatched = LTQ_FALSE;
		else if ((vlan->vlan2_list[i].act.act & DP_VLAN_ACT_DROP))
			pcfg[j].bDiscardMatched = LTQ_TRUE;
		else
			goto EXIT;
		/* VLAN filter only support VID, PCP, or TCI only */
		k = vlan_filter_mode(&vlan->vlan2_list[i].outer);
		switch (k) {
		case 1:
			pcfg[j].eVlanFilterMask = GSW_VLAN_FILTER_TCI_MASK_VID;
			pcfg[j].nVal = (u32)vlan->vlan2_list[i].outer.vid;
			break;
		case 2:
			pcfg[j].eVlanFilterMask = GSW_VLAN_FILTER_TCI_MASK_PCP;
			pcfg[j].nVal = (u32)vlan->vlan2_list[i].outer.prio;
			break;
		case 7:
			pcfg[j].eVlanFilterMask = GSW_VLAN_FILTER_TCI_MASK_TCI;
			pcfg[j].nVal = (u32)vlan->vlan2_list[i].outer.vid &
				       0xFFF;
			pcfg[j].nVal |= ((u32)
					 vlan->vlan2_list[i].outer.prio & 7)
					 << 13;
			pcfg[j].nVal |= ((u32)
					 vlan->vlan2_list[i].outer.dei & 1)
					 << 12;
			break;
		default:
			goto EXIT;
		}
		if (mode < 0)
			mode = k;
		else if (mode != k)
			goto EXIT;
		/* Inner VLAN should be "don't care" for all fields */
		k = vlan_filter_mode(&vlan->vlan2_list[i].inner);
		if (k != 0)
			goto EXIT;
		j++;
	}

	/* Check if we have anything to configure */
	if (j <= 0) {
		/* Update bridge port */
		ret = update_bp(ops,
				(u32)info->bp,
				vlan->dir == DP_DIR_INGRESS,
				NULL,
				NULL);
		goto EXIT;
	}

	if (untagged < 0)
		untagged = 0;
	if (tagged < 0)
		tagged = 0;

	/* Allocate VLAN filter */
	memset(&alloc, 0, sizeof(alloc));
	alloc.nNumberOfEntries = (u32)j;
	alloc.bDiscardUntagged = untagged ? LTQ_TRUE : LTQ_FALSE;
	alloc.bDiscardUnmatchedTagged = tagged ? LTQ_TRUE : LTQ_FALSE;
	ret = ops->gsw_vlanfilter_ops.VlanFilter_Alloc(ops, &alloc);
	if (ret != GSW_statusOk) {
		ret = -EIO;
		goto EXIT;
	}
	DP_DEBUG(DP_DBG_FLAG_PAE, "VlanFilter_Alloc - %u[%u]\n",
		 alloc.nVlanFilterBlockId, alloc.nNumberOfEntries);

	/* Configure each VLAN filter */
	for (i = 0; i < j; i++) {
		pcfg[i].nVlanFilterBlockId = alloc.nVlanFilterBlockId;
		pcfg[i].nEntryIndex = (u32)i;
		ret = ops->gsw_vlanfilter_ops.VlanFilter_Set(ops, &pcfg[i]);
		if (ret != GSW_statusOk) {
			ops->gsw_vlanfilter_ops.VlanFilter_Free(ops, &alloc);
			ret = -EIO;
			goto EXIT;
		}
	}

	/* Update bridge port */
	ret = update_bp(ops,
			(u32)info->bp,
			vlan->dir == DP_DIR_INGRESS,
			&alloc,
			NULL);
	if (ret != 0)
		ops->gsw_vlanfilter_ops.VlanFilter_Free(ops, &alloc);

EXIT:
	kfree(pcfg);
	return ret;
}

static int write_vtetype(struct core_ops *ops, u16 val)
{
	int ret;
	GSW_register_t reg = {0};

	reg.nRegAddr = 0xA42;
	reg.nData = val;
	ret = ops->gsw_common_ops.RegisterSet(ops, &reg);
	if (ret == GSW_statusOk)
		return 0;
	else
		return -EIO;
}

static int ext_vlan_filter_cfg(struct core_ops *ops,
			       GSW_EXTENDEDVLAN_filterVLAN_t *pcfg,
			       struct dp_pattern_vlan *pattern)
{
	int ret = 0;

	if (pattern->prio == DP_VLAN_PATTERN_NOT_CARE) {
		pcfg->bPriorityEnable = LTQ_FALSE;
	} else {
		pcfg->bPriorityEnable = LTQ_TRUE;
		pcfg->nPriorityVal = pattern->prio;
	}
	if (pattern->vid == DP_VLAN_PATTERN_NOT_CARE) {
		pcfg->bVidEnable = LTQ_FALSE;
	} else {
		pcfg->bVidEnable = LTQ_TRUE;
		pcfg->nVidVal = pattern->vid;
	}
	if (pattern->tpid == DP_VLAN_PATTERN_NOT_CARE) {
		pcfg->eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_NO_FILTER;
	} else if (pattern->tpid == ETH_P_8021Q) {
		pcfg->eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_8021Q;
	} else {
		pcfg->eTpid = GSW_EXTENDEDVLAN_FILTER_TPID_VTETYPE;
		ret = write_vtetype(ops, (u16)pattern->tpid);
	}
	if (pattern->dei == DP_VLAN_PATTERN_NOT_CARE)
		pcfg->eDei = GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;
	else if (pattern->dei == 0)
		pcfg->eDei = GSW_EXTENDEDVLAN_FILTER_DEI_0;
	else
		pcfg->eDei = GSW_EXTENDEDVLAN_FILTER_DEI_1;

	return ret;
}

static int ext_vlan_insert(struct core_ops *ops,
			   GSW_EXTENDEDVLAN_treatmentVlan_t *pcfg,
			   struct dp_act_vlan *act,
			   unsigned int idx)
{
	int ret;

	DP_DEBUG(DP_DBG_FLAG_PAE, "act->tpid[%u]: 0x%04x\n",
		 idx, act->tpid[idx]);
	switch (act->tpid[idx]) {
	case CP_FROM_INNER:
		pcfg->eTpid = GSW_EXTENDEDVLAN_TREATMENT_INNER_TPID;
		break;
	case CP_FROM_OUTER:
		pcfg->eTpid = GSW_EXTENDEDVLAN_TREATMENT_OUTER_TPID;
		break;
	case ETH_P_8021Q:
		pcfg->eTpid = GSW_EXTENDEDVLAN_TREATMENT_8021Q;
		break;
	default:
		pcfg->eTpid = GSW_EXTENDEDVLAN_TREATMENT_VTETYPE;
		ret = write_vtetype(ops, (u16)act->tpid[idx]);
		if (ret)
			return ret;
	}

	switch (act->vid[idx]) {
	case CP_FROM_INNER:
		pcfg->eVidMode = GSW_EXTENDEDVLAN_TREATMENT_INNER_VID;
		break;
	case CP_FROM_OUTER:
		pcfg->eVidMode = GSW_EXTENDEDVLAN_TREATMENT_OUTER_VID;
		break;
	default:
		pcfg->eVidMode = GSW_EXTENDEDVLAN_TREATMENT_VID_VAL;
		pcfg->eVidVal = (u32)act->vid[idx];
	}

	switch (act->dei[idx]) {
	case CP_FROM_INNER:
		pcfg->eDei = GSW_EXTENDEDVLAN_TREATMENT_INNER_DEI;
		break;
	case CP_FROM_OUTER:
		pcfg->eDei = GSW_EXTENDEDVLAN_TREATMENT_OUTER_DEI;
		break;
	case 0:
		pcfg->eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_0;
		break;
	case 1:
		pcfg->eDei = GSW_EXTENDEDVLAN_TREATMENT_DEI_1;
		break;
	default:
		return -EINVAL;
	}

	switch (act->prio[idx]) {
	case CP_FROM_INNER:
		pcfg->ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_INNER_PRORITY;
		break;
	case CP_FROM_OUTER:
		pcfg->ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_OUTER_PRORITY;
		break;
	case DERIVE_FROM_DSCP:
		pcfg->ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_DSCP;
		break;
	default:
		pcfg->ePriorityMode = GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		pcfg->ePriorityVal = (u32)act->prio[idx];
		break;
	}
	return 0;
}

static int ext_vlan_action_cfg(struct core_ops *ops,
			       GSW_EXTENDEDVLAN_treatment_t *pcfg,
			       struct dp_act_vlan *act)
{
	int ret;
	dp_subif_t subif;

	DP_DEBUG(DP_DBG_FLAG_PAE, "act->act: 0x%02x\n", (unsigned int)act->act);

	/* Copy DSCP table */
	memcpy(pcfg->nDscp2PcpMap, act->dscp_pcp_map,
	       sizeof(pcfg->nDscp2PcpMap));

	DP_DEBUG(DP_DBG_FLAG_PAE, "act->ract.act: 0x%02x Bp_dev %p\n",
		 (unsigned int)act->ract.act, act->ract.bp_dev);

	/* Reassign Bridge Port */
	if ((act->ract.act & DP_BP_REASSIGN) && (act->ract.bp_dev)) {
		ret = dp_get_port_subitf_via_dev(act->ract.bp_dev, &subif);
		if (ret == DP_FAILURE)
			return 0;

		DP_DEBUG(DP_DBG_FLAG_PAE, "New Bridge Port %d\n", subif.bport);

		pcfg->bReassignBridgePort = 1;
		pcfg->nNewBridgePortId = subif.bport;
	}

	/* Reassign traffic class */
	if (act->ract.act & DP_TC_REASSIGN) {
		DP_DEBUG(DP_DBG_FLAG_PAE, "New TC %d\n", act->ract.new_tc);

		pcfg->bNewTrafficClassEnable = 1;
		pcfg->nNewTrafficClass = act->ract.new_tc;
	}

	/* forward without modification */
	if ((act->act & DP_VLAN_ACT_FWD))
		return 0;

	/* drop the packet */
	if ((act->act & DP_VLAN_ACT_DROP)) {
		pcfg->eRemoveTag = GSW_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;
		return 0;
	}

	/* remove tag */
	if ((act->act & DP_VLAN_ACT_POP)) {
		DP_DEBUG(DP_DBG_FLAG_PAE, "act->pop_n: %d\n", act->pop_n);
		switch (act->pop_n) {
		case 1:
			pcfg->eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
			break;
		case 2:
			pcfg->eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG;
			break;
		default:
			return -EINVAL;
		}
	}

	if (!(act->act & DP_VLAN_ACT_PUSH))
		return 0;

	switch (act->push_n) {
	case 2:
		pcfg->bAddInnerVlan = LTQ_TRUE;
		ret = ext_vlan_insert(ops, &pcfg->sInnerVlan, act, 1);
		if (ret)
			return ret;
	case 1:
		pcfg->bAddOuterVlan = LTQ_TRUE;
		return ext_vlan_insert(ops, &pcfg->sOuterVlan, act, 0);
	default:
		return -EINVAL;
	}
}

static int ext_vlan_cfg(struct core_ops *ops,
			GSW_EXTENDEDVLAN_config_t *pcfg,
			int def,
			int ethertype,
			struct dp_pattern_vlan *outer,
			struct dp_pattern_vlan *inner,
			struct dp_act_vlan *act)
{
	const int ethertype_map[] = {
		DP_VLAN_PATTERN_NOT_CARE,
		GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER,
		DP_PROTO_IP4,
		GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPOE,
		DP_PROTO_PPPOE,
		GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_PPPOE,
		DP_PROTO_PPPOE_SES,
		GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_PPPOE,
		DP_PROTO_ARP,
		GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_ARP,
		DP_PROTO_IP6,
		GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_IPV6IPOE,
		DP_PROTO_EAPOL,
		GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_EAPOL,
	};
	int ret;
	unsigned int i;

	for (i = 0; i < ARRAY_SIZE(ethertype_map); i += 2) {
		if (ethertype == ethertype_map[i])
			break;
	}
	if (i >= ARRAY_SIZE(ethertype_map))
		return -EINVAL;
	pcfg->sFilter.eEtherType = (u32)ethertype_map[i + 1];

	switch ((inner ? 2 : 0) | (outer ? 1 : 0)) {
	case 0:
		DP_DEBUG(DP_DBG_FLAG_PAE, "Untagged Rule\n");
		/* untagged rule */
		if (def)	/* default untagged rule is not supported */
			return -EINVAL;
		pcfg->sFilter.sInnerVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		pcfg->sFilter.sOuterVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		break;
	case 1:
		DP_DEBUG(DP_DBG_FLAG_PAE, "1-tag Rule\n");
		/* 1-tag rule */
		pcfg->sFilter.sInnerVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
		if (def) {
			/* default 1-tag rule */
			pcfg->sFilter.sOuterVlan.eType =
				GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
			break;
		}
		pcfg->sFilter.sOuterVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		ret = ext_vlan_filter_cfg(ops,
					  &pcfg->sFilter.sOuterVlan,
					  outer);
		if (ret)
			return ret;
		break;
	case 3:
		DP_DEBUG(DP_DBG_FLAG_PAE, "2-tag Rule\n");
		/* 2-tag rule */
		if (def) {
			/* default 2-tag rule */
			pcfg->sFilter.sOuterVlan.eType =
				GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;
			pcfg->sFilter.sInnerVlan.eType =
				GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
			break;
		}
		pcfg->sFilter.sOuterVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		pcfg->sFilter.sInnerVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		ret = ext_vlan_filter_cfg(ops,
					  &pcfg->sFilter.sOuterVlan,
					  outer);
		if (ret)
			return ret;
		ret = ext_vlan_filter_cfg(ops,
					  &pcfg->sFilter.sInnerVlan,
					  inner);
		if (ret)
			return ret;
		break;
	default:
		return -EINVAL;
	}

	ret = ext_vlan_action_cfg(ops, &pcfg->sTreatment, act);
	if (ret)
		return ret;

	return ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, pcfg);
}

static int tc_ext_vlan(struct core_ops *ops,
		       struct dp_tc_vlan *vlan,
		       struct dp_tc_vlan_info *info)
{
	int ret;
	int total = vlan->n_vlan0 + vlan->n_vlan1 + vlan->n_vlan2;
	GSW_EXTENDEDVLAN_alloc_t alloc = {0};
	GSW_EXTENDEDVLAN_config_t cfg;
	int i, j;

	if (total > 0) {
		alloc.nNumberOfEntries = (u32)total;
		ret = ops->gsw_extvlan_ops.ExtendedVlan_Alloc(ops, &alloc);
		if (ret != GSW_statusOk)
			return -EIO;
		DP_DEBUG(DP_DBG_FLAG_PAE, "ExtendedVlan_Alloc - %u[%u]\n",
			 alloc.nExtendedVlanBlockId,
			 alloc.nNumberOfEntries);

		j = 0;

		/* untagged rule */
		for (i = 0; i < vlan->n_vlan0; i++) {
			memset(&cfg, 0, sizeof(cfg));
			cfg.nExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
			cfg.nEntryIndex = j;
			j++;
			ret = ext_vlan_cfg(ops,
					   &cfg,
					   vlan->vlan0_list[i].def,
					   vlan->vlan0_list[i].outer.proto,
					   NULL,
					   NULL,
					   &vlan->vlan0_list[i].act);
			if (ret != 0)
				goto ERROR;
		}

		/* 1-tag rule */
		for (i = 0; i < vlan->n_vlan1; i++) {
			memset(&cfg, 0, sizeof(cfg));
			cfg.nExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
			cfg.nEntryIndex = j;
			j++;
			ret = ext_vlan_cfg(ops,
					   &cfg,
					   vlan->vlan1_list[i].def,
					   vlan->vlan1_list[i].outer.proto,
					   &vlan->vlan1_list[i].outer,
					   NULL,
					   &vlan->vlan1_list[i].act);
			if (ret != 0)
				goto ERROR;
		}

		/* 2-tag rule */
		for (i = 0; i < vlan->n_vlan2; i++) {
			memset(&cfg, 0, sizeof(cfg));
			cfg.nExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
			cfg.nEntryIndex = j;
			j++;
			ret = ext_vlan_cfg(ops,
					   &cfg,
					   vlan->vlan2_list[i].def,
					   vlan->vlan2_list[i].outer.proto,
					   &vlan->vlan2_list[i].outer,
					   &vlan->vlan2_list[i].inner,
					   &vlan->vlan2_list[i].act);
			if (ret != 0)
				goto ERROR;
		}
	}

	if ((info->dev_type & 0x01) == 0) {
		/* Configure CTP */
		ret = update_ctp(ops,
				 (u32)info->dp_port,
				 (u32)info->subix,
				 vlan->dir == DP_DIR_INGRESS,
				 info->dev_type & GENMASK(2, 1),
				 total > 0 ? &alloc : NULL);
	} else {
		/* Configure bridge port */
		ret = update_bp(ops,
				(u32)info->bp,
				vlan->dir == DP_DIR_INGRESS,
				NULL,
				total > 0 ? &alloc : NULL);
	}
	if (ret == 0)
		return 0;

ERROR:
	ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
	return ret;
}

/*tc_vlan_set_31:
 *param[in] vlan: directly from DP core API's parameter
 *param[in] info: traslated via vlan->dev.
 *       if it is BP/pmapper dev, info->dev_type=DP_DEV_TYPE_BP_PMAPPER {
 *                                info->bp is set
 *            if vlan->def_apply == DP_VLAN_APPLY_CTP,
 *                                info->subix is set also
 *       } elif it is CTP/GEM dev, info->dev_type=DP_DEV_TYPE_CTP_GEM {
 *                                info->subix is set
 *       }
 *flag: reserved for future
 */
int tc_vlan_set_31(struct core_ops *ops,
		   struct dp_tc_vlan *vlan,
		   struct dp_tc_vlan_info *info,
		   int flag)
{
	/* If it's bridge port, try to configure VLAN filter. */
	if ((info->dev_type & 0x01) != 0) {
		int ret;

		/* Multicast (IGMP Controlled) VLAN is not supported
		 * on Bridge Port
		 */
		if ((info->dev_type & 0x02) != 0)
			return -EINVAL;

		ret = tc_vlan_filter(ops, vlan, info);
		/* Either managed to configure VLAN filter
		 * or error happens in GSW API
		 */
		if (ret <= 0)
			return ret;
		/* VLAN filter can't cover the case and need extended VLAN */
	}

	/* Configure extended VLAN */
	return tc_ext_vlan(ops, vlan, info);
}
