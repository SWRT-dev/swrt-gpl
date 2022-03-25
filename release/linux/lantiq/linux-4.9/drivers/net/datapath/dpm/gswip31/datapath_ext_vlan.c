/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifdef NON_LINUX
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "lantiq_gsw.h"
#include "lantiq_gsw_api.h"
#include "gsw_flow_ops.h"
/* Adaption */
#define kfree(x)	free(x)
#define kmalloc(x, y)	malloc(x)
#else
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/etherdevice.h>
#include <net/datapath_api.h>
#include "../datapath.h"
#endif

/* Structure only used by function set_gswip_ext_vlan */
struct priv_ext_vlan {
	/* number of bridge ports has VLAN */
	u32 num_bp;
	/* bridge port list (vlan1_list + vlan2_list) */
	u32 bp[1];
};

/* Supporting Functions */
static int update_vlan0(struct core_ops *ops, u32 bp,
			GSW_EXTENDEDVLAN_config_t *vcfg)
{
	GSW_return_t ret;
	u32 block = vcfg->nExtendedVlanBlockId;

	memset(vcfg, 0, sizeof(*vcfg));
	vcfg->nExtendedVlanBlockId = block;
	vcfg->nEntryIndex = 0;
	vcfg->sFilter.sOuterVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vcfg->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER;
	vcfg->sTreatment.bReassignBridgePort = LTQ_TRUE;
	vcfg->sTreatment.nNewBridgePortId = bp;
	ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

	if (ret != GSW_statusOk)
		return -EIO;

	memset(vcfg, 0, sizeof(*vcfg));
	vcfg->nExtendedVlanBlockId = block;
	vcfg->nEntryIndex = 1;
	vcfg->sFilter.sOuterVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
	vcfg->sFilter.sInnerVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;
	vcfg->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER;
	vcfg->sTreatment.bReassignBridgePort = LTQ_TRUE;
	vcfg->sTreatment.nNewBridgePortId = bp;
	ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

	if (ret != GSW_statusOk)
		return -EIO;

	memset(vcfg, 0, sizeof(*vcfg));
	vcfg->nExtendedVlanBlockId = block;
	vcfg->nEntryIndex = 2;
	vcfg->sFilter.sInnerVlan.eType = GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT;
	vcfg->sFilter.eEtherType = GSW_EXTENDEDVLAN_FILTER_ETHERTYPE_NO_FILTER;
	vcfg->sTreatment.bReassignBridgePort = LTQ_TRUE;
	vcfg->sTreatment.nNewBridgePortId = bp;
	ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

	if (ret != GSW_statusOk)
		return -EIO;

	return 0;
}

static int update_vlan1(struct core_ops *ops, int base, int num,
			struct vlan1 *vlan_list, int drop,
			GSW_EXTENDEDVLAN_config_t *vcfg)
{
	GSW_return_t ret;
	u32 block = vcfg->nExtendedVlanBlockId;
	int i;

	for (i = 0; i < num; i++, base += 2) {
		memset(vcfg, 0, sizeof(*vcfg));
		vcfg->nExtendedVlanBlockId = block;
		vcfg->nEntryIndex = base;

		vcfg->sFilter.sOuterVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		vcfg->sFilter.sOuterVlan.bPriorityEnable = LTQ_FALSE;
		vcfg->sFilter.sOuterVlan.bVidEnable = LTQ_TRUE;
		vcfg->sFilter.sOuterVlan.nVidVal = vlan_list[i].outer_vlan.vid;
		vcfg->sFilter.sOuterVlan.eTpid = vlan_list[i].outer_vlan.tpid;
		vcfg->sFilter.sOuterVlan.eDei =
			GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;

		vcfg->sFilter.sInnerVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG;

		vcfg->sFilter.eEtherType = vlan_list[i].ether_type;

		if (drop) {
			vcfg->sTreatment.eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;
		} else {
			vcfg->sTreatment.eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
			vcfg->sTreatment.bReassignBridgePort = LTQ_TRUE;
			vcfg->sTreatment.nNewBridgePortId = vlan_list[i].bp;
		}

		ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

		if (ret != GSW_statusOk)
			return -EIO;

		memset(vcfg, 0, sizeof(*vcfg));
		vcfg->nExtendedVlanBlockId = block;
		vcfg->nEntryIndex = base + 1;

		vcfg->sFilter.sOuterVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		vcfg->sFilter.sOuterVlan.bPriorityEnable = LTQ_FALSE;
		vcfg->sFilter.sOuterVlan.bVidEnable = LTQ_TRUE;
		vcfg->sFilter.sOuterVlan.nVidVal = vlan_list[i].outer_vlan.vid;
		vcfg->sFilter.sOuterVlan.eTpid = vlan_list[i].outer_vlan.tpid;
		vcfg->sFilter.sOuterVlan.eDei =
			GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;

		vcfg->sFilter.sInnerVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER;

		vcfg->sFilter.eEtherType = vlan_list[i].ether_type;

		if (drop) {
			vcfg->sTreatment.eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;
		} else {
			vcfg->sTreatment.eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_REMOVE_1_TAG;
			vcfg->sTreatment.bReassignBridgePort = LTQ_TRUE;
			vcfg->sTreatment.nNewBridgePortId = vlan_list[i].bp;
		}

		ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

		if (ret != GSW_statusOk)
			return -EIO;
	}

	return 0;
}

static int update_vlan2(struct core_ops *ops, int base, int num,
			struct vlan2 *vlan_list, int drop,
			GSW_EXTENDEDVLAN_config_t *vcfg)
{
	GSW_return_t ret;
	u32 block = vcfg->nExtendedVlanBlockId;
	int i;

	for (i = 0; i < num; i++, base++) {
		memset(vcfg, 0, sizeof(*vcfg));
		vcfg->nExtendedVlanBlockId = block;
		vcfg->nEntryIndex = base;

		vcfg->sFilter.sOuterVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		vcfg->sFilter.sOuterVlan.bPriorityEnable = LTQ_FALSE;
		vcfg->sFilter.sOuterVlan.bVidEnable = LTQ_TRUE;
		vcfg->sFilter.sOuterVlan.nVidVal = vlan_list[i].outer_vlan.vid;
		vcfg->sFilter.sOuterVlan.eTpid = vlan_list[i].outer_vlan.tpid;
		vcfg->sFilter.sOuterVlan.eDei =
			GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;

		vcfg->sFilter.sInnerVlan.eType =
			GSW_EXTENDEDVLAN_FILTER_TYPE_NORMAL;
		vcfg->sFilter.sInnerVlan.bPriorityEnable = LTQ_FALSE;
		vcfg->sFilter.sInnerVlan.bVidEnable = LTQ_TRUE;
		vcfg->sFilter.sInnerVlan.nVidVal = vlan_list[i].inner_vlan.vid;
		vcfg->sFilter.sInnerVlan.eTpid = vlan_list[i].inner_vlan.tpid;
		vcfg->sFilter.sInnerVlan.eDei =
			GSW_EXTENDEDVLAN_FILTER_DEI_NO_FILTER;

		vcfg->sFilter.eEtherType = vlan_list[i].ether_type;

		if (drop) {
			vcfg->sTreatment.eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_DISCARD_UPSTREAM;
		} else {
			vcfg->sTreatment.eRemoveTag =
				GSW_EXTENDEDVLAN_TREATMENT_REMOVE_2_TAG;
			vcfg->sTreatment.bReassignBridgePort = LTQ_TRUE;
			vcfg->sTreatment.nNewBridgePortId = vlan_list[i].bp;
		}

		ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

		if (ret != GSW_statusOk)
			return -EIO;
	}

	return 0;
}

static int update_ctp(struct core_ops *ops, struct ext_vlan_info *vlan)
{
	static GSW_CTP_portConfig_t ctp;
	static GSW_EXTENDEDVLAN_config_t vcfg;
	GSW_return_t ret;
	ltq_bool_t enable;
	u32 block;
	GSW_EXTENDEDVLAN_alloc_t alloc;
	int i;

	DP_DEBUG(DP_DBG_FLAG_SWDEV, "bp=%d v1=%d v1_d=%d v2=%d v2_d=%d\n",
		 vlan->bp, vlan->n_vlan1, vlan->n_vlan1_drop,
		vlan->n_vlan2, vlan->n_vlan2_drop);
	memset(&ctp, 0, sizeof(ctp));
	memset(&alloc, 0, sizeof(GSW_EXTENDEDVLAN_alloc_t));

	ctp.nLogicalPortId = vlan->logic_port;
	ctp.nSubIfIdGroup = vlan->subif_grp;
	ctp.eMask = GSW_CTP_PORT_CONFIG_MASK_INGRESS_VLAN;
	ret = ops->gsw_ctp_ops.CTP_PortConfigGet(ops, &ctp);

	if (ret != GSW_statusOk)
		return -EIO;

	enable = ctp.bIngressExtendedVlanEnable;
	block = ctp.nIngressExtendedVlanBlockId;

	/* disable previous vlan block operation,if enabled and
	 * free the previous allocated vlan blocks
	 * so that new vlan block can be allocated and configured to ctp
	 */
	if (enable) {
		ctp.bIngressExtendedVlanEnable = LTQ_FALSE;
		ret = ops->gsw_ctp_ops.CTP_PortConfigSet(ops, &ctp);
		if (ret != GSW_statusOk) {
			pr_err("Fail:Ingress VLAN operate disable in ctp\n");
			return -EIO;
		}
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "ingress VLAN operation disabled in ctp\n");
		alloc.nExtendedVlanBlockId = block;
		ret = ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		if (ret != GSW_statusOk)
			return -EIO;
	}
	memset(&alloc, 0, sizeof(GSW_EXTENDEDVLAN_alloc_t));

	alloc.nNumberOfEntries += vlan->n_vlan1 * 2;
	alloc.nNumberOfEntries += vlan->n_vlan2;
	alloc.nNumberOfEntries += vlan->n_vlan1_drop * 2;
	alloc.nNumberOfEntries += vlan->n_vlan2_drop;
	if (alloc.nNumberOfEntries == 0) {
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "nNumberOfEntries == 0\n");
		return 0;
	}

	alloc.nNumberOfEntries += 3;
	ret = ops->gsw_extvlan_ops.ExtendedVlan_Alloc(ops, &alloc);

	if (ret != GSW_statusOk)
		return -EIO;

	vcfg.nExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
	ret = update_vlan0(ops, vlan->bp, &vcfg);

	if (ret < 0)
		goto UPDATE_ERROR;

	i = 3;
	ret = update_vlan2(ops, i, vlan->n_vlan2, vlan->vlan2_list, 0, &vcfg);

	if (ret < 0)
		goto UPDATE_ERROR;

	i += vlan->n_vlan2;
	ret = update_vlan2(ops, i, vlan->n_vlan2_drop,
			   vlan->vlan2_drop_list, 1, &vcfg);

	if (ret < 0)
		goto UPDATE_ERROR;

	i += vlan->n_vlan2_drop;
	ret = update_vlan1(ops, i, vlan->n_vlan1, vlan->vlan1_list, 0, &vcfg);

	if (ret < 0)
		goto UPDATE_ERROR;

	i += vlan->n_vlan1;
	ret = update_vlan1(ops, i, vlan->n_vlan1_drop,
			   vlan->vlan1_drop_list, 1, &vcfg);

	if (ret < 0)
		goto UPDATE_ERROR;

	ctp.bIngressExtendedVlanEnable = LTQ_TRUE;
	ctp.nIngressExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
	ctp.nIngressExtendedVlanBlockSize = 0;
	ret = ops->gsw_ctp_ops.CTP_PortConfigSet(ops, &ctp);

	if (ret != GSW_statusOk)
		return -EIO;

	return 0;
UPDATE_ERROR:
	ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
	return ret;
}

static int bp_add_vlan1(struct core_ops *ops, struct vlan1 *vlan,
			GSW_BRIDGE_portConfig_t *bpcfg,
			GSW_EXTENDEDVLAN_config_t *vcfg)
{
	int ret;
	GSW_EXTENDEDVLAN_alloc_t alloc = {0};
	GSW_ExtendedVlanFilterType_t types[6] = {
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG,
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER,
		GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT,
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG,
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER,
		GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT
	};
	u32 i;

	memset(bpcfg, 0, sizeof(*bpcfg));
	bpcfg->nBridgePortId = vlan->bp;
	bpcfg->eMask = GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN;
	ret = ops->gsw_brdgport_ops.BridgePort_ConfigGet(ops, bpcfg);

	if (ret != GSW_statusOk)
		return -EIO;

	/* This bridge port should have no egress VLAN configured yet */
	if (bpcfg->bEgressExtendedVlanEnable != LTQ_FALSE)
		return -EINVAL;

	alloc.nNumberOfEntries = ARRAY_SIZE(types) / 2;
	ret = ops->gsw_extvlan_ops.ExtendedVlan_Alloc(ops, &alloc);

	if (ret != GSW_statusOk)
		return -EIO;

	for (i = 0; i < alloc.nNumberOfEntries; i++) {
		memset(vcfg, 0, sizeof(*vcfg));
		vcfg->nExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
		vcfg->nEntryIndex = i;
		vcfg->sFilter.sOuterVlan.eType = types[i * 2];
		vcfg->sFilter.sInnerVlan.eType = types[i * 2 + 1];
		/* filter ether_type as ingress */
		vcfg->sFilter.eEtherType = vlan->ether_type;

		vcfg->sTreatment.bAddOuterVlan = LTQ_TRUE;
		vcfg->sTreatment.sOuterVlan.ePriorityMode =
			GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vcfg->sTreatment.sOuterVlan.ePriorityVal = 0;
		vcfg->sTreatment.sOuterVlan.eVidMode =
			GSW_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vcfg->sTreatment.sOuterVlan.eVidVal = vlan->outer_vlan.vid;
		vcfg->sTreatment.sOuterVlan.eTpid = vlan->outer_vlan.tpid;
		vcfg->sTreatment.sOuterVlan.eDei =
			GSW_EXTENDEDVLAN_TREATMENT_DEI_0;

		vcfg->sTreatment.bAddInnerVlan = LTQ_FALSE;

		ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

		if (ret != GSW_statusOk) {
			pr_err("Fail updating Extended VLAN entry (%u, %u).\n",
			       alloc.nExtendedVlanBlockId, i);
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
			return -EIO;
		}
	}

	bpcfg->bEgressExtendedVlanEnable = LTQ_TRUE;
	bpcfg->nEgressExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
	bpcfg->nEgressExtendedVlanBlockSize = 0;
	ret = ops->gsw_brdgport_ops.BridgePort_ConfigSet(ops, bpcfg);

	if (ret != GSW_statusOk) {
		pr_err("Failed in attaching Extended VLAN to Bridge Port.\n");
		ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		return -EIO;
	} else {
		return 0;
	}
}

static int bp_add_vlan2(struct core_ops *ops, struct vlan2 *vlan,
			GSW_BRIDGE_portConfig_t *bpcfg,
			GSW_EXTENDEDVLAN_config_t *vcfg)
{
	int ret;
	GSW_EXTENDEDVLAN_alloc_t alloc = {0};
	GSW_ExtendedVlanFilterType_t types[6] = {
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG,
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER,
		GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT,
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_TAG,
		GSW_EXTENDEDVLAN_FILTER_TYPE_NO_FILTER,
		GSW_EXTENDEDVLAN_FILTER_TYPE_DEFAULT
	};
	u32 i;

	memset(bpcfg, 0, sizeof(*bpcfg));
	bpcfg->nBridgePortId = vlan->bp;
	bpcfg->eMask = GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN;
	ret = ops->gsw_brdgport_ops.BridgePort_ConfigGet(ops, bpcfg);

	if (ret != GSW_statusOk)
		return -EIO;

	/* This bridge port should have no egress VLAN configured yet */
	if (bpcfg->bEgressExtendedVlanEnable != LTQ_FALSE)
		return -EINVAL;

	alloc.nNumberOfEntries = ARRAY_SIZE(types) / 2;
	ret = ops->gsw_extvlan_ops.ExtendedVlan_Alloc(ops, &alloc);

	if (ret != GSW_statusOk)
		return -EIO;

	for (i = 0; i < alloc.nNumberOfEntries; i++) {
		memset(vcfg, 0, sizeof(*vcfg));
		vcfg->nExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
		vcfg->nEntryIndex = i;
		vcfg->sFilter.sOuterVlan.eType = types[i * 2];
		vcfg->sFilter.sInnerVlan.eType = types[i * 2 + 1];
		/* filter ether_type as ingress */
		vcfg->sFilter.eEtherType = vlan->ether_type;

		vcfg->sTreatment.bAddOuterVlan = LTQ_TRUE;
		vcfg->sTreatment.sOuterVlan.ePriorityMode =
			GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vcfg->sTreatment.sOuterVlan.ePriorityVal = 0;
		vcfg->sTreatment.sOuterVlan.eVidMode =
			GSW_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vcfg->sTreatment.sOuterVlan.eVidVal = vlan->outer_vlan.vid;
		vcfg->sTreatment.sOuterVlan.eTpid = vlan->outer_vlan.tpid;
		vcfg->sTreatment.sOuterVlan.eDei =
			GSW_EXTENDEDVLAN_TREATMENT_DEI_0;

		vcfg->sTreatment.bAddInnerVlan = LTQ_TRUE;
		vcfg->sTreatment.sInnerVlan.ePriorityMode =
			GSW_EXTENDEDVLAN_TREATMENT_PRIORITY_VAL;
		vcfg->sTreatment.sInnerVlan.ePriorityVal = 0;
		vcfg->sTreatment.sInnerVlan.eVidMode =
			GSW_EXTENDEDVLAN_TREATMENT_VID_VAL;
		vcfg->sTreatment.sInnerVlan.eVidVal = vlan->inner_vlan.vid;
		vcfg->sTreatment.sInnerVlan.eTpid = vlan->inner_vlan.tpid;
		vcfg->sTreatment.sInnerVlan.eDei =
			GSW_EXTENDEDVLAN_TREATMENT_DEI_0;

		ret = ops->gsw_extvlan_ops.ExtendedVlan_Set(ops, vcfg);

		if (ret != GSW_statusOk) {
			pr_err("Fail updating Extended VLAN entry (%u, %u).\n",
			       alloc.nExtendedVlanBlockId, i);
			ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
			return -EIO;
		}
	}

	bpcfg->bEgressExtendedVlanEnable = LTQ_TRUE;
	bpcfg->nEgressExtendedVlanBlockId = alloc.nExtendedVlanBlockId;
	bpcfg->nEgressExtendedVlanBlockSize = 0;
	ret = ops->gsw_brdgport_ops.BridgePort_ConfigSet(ops, bpcfg);

	if (ret != GSW_statusOk) {
		pr_err("Failed in attaching Extended VLAN to Bridge Port.\n");
		ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);
		return -EIO;
	} else {
		return 0;
	}
}

static int bp_add_vlan(struct core_ops *ops, struct ext_vlan_info *vlan,
		       int idx, GSW_BRIDGE_portConfig_t *bpcfg)
{
	static GSW_EXTENDEDVLAN_config_t vcfg;

	if (idx < vlan->n_vlan1)
		return bp_add_vlan1(ops, vlan->vlan1_list + idx, bpcfg, &vcfg);
	else
		return bp_add_vlan2(ops,
				    vlan->vlan2_list + (idx - vlan->n_vlan1),
				    bpcfg, &vcfg);
}

static int bp_rm_vlan(struct core_ops *ops, u32 bp,
		      GSW_BRIDGE_portConfig_t *bpcfg)
{
	int ret;
	GSW_EXTENDEDVLAN_alloc_t alloc = {0};

	DP_DEBUG(DP_DBG_FLAG_SWDEV, "bp=%d\n", bp);
	memset(bpcfg, 0, sizeof(*bpcfg));
	bpcfg->nBridgePortId = bp;
	bpcfg->eMask = GSW_BRIDGE_PORT_CONFIG_MASK_EGRESS_VLAN;
	ret = ops->gsw_brdgport_ops.BridgePort_ConfigGet(ops, bpcfg);

	if (ret != GSW_statusOk)
		return -EIO;

	if (bpcfg->bEgressExtendedVlanEnable == LTQ_FALSE)
		return 0;

	alloc.nExtendedVlanBlockId = bpcfg->nEgressExtendedVlanBlockId;
	bpcfg->bEgressExtendedVlanEnable = LTQ_FALSE;
	ret = ops->gsw_brdgport_ops.BridgePort_ConfigSet(ops, bpcfg);

	if (ret != GSW_statusOk)
		return -EIO;

	ret = ops->gsw_extvlan_ops.ExtendedVlan_Free(ops, &alloc);

	if (ret != GSW_statusOk)
		return -EIO;
	else
		return 0;
}

static int bp_diff(u32 *bp0, u32 num_bp0, u32 *bp1)
{
	u32 i, j;

	for (i = 0; i < num_bp0; i++) {
		for (j = 0; j < num_bp0 && bp0[j] != bp1[i]; j++)
			;
		if (j >= num_bp0)
			break;
	}
	return i;
}

/* Function for VLAN configure */
int set_gswip_ext_vlan(struct core_ops *ops, struct ext_vlan_info *vlan,
		       int flag)
{
	static GSW_BRIDGE_portConfig_t bpcfg;

	int ret;
	struct priv_ext_vlan *old_priv, *new_priv;
	size_t new_priv_size;
	int i, j;

	/* Assumptions:
	 * 1. Every time this function is called, one and only one "vlan" is
	 *    added or removed. No replacement happens.
	 * 2. The content of 2 vlan_list (not vlan_drop_list) keeps sequence.
	 *    Only one new item is inserted or removed. No re-ordering happens.
	 * 3. Bridge ports are not freed before this function is called. This
	 *    is not compulsary. Just in case any resource free function
	 *    such as free VLAN block, disable VLAN from bridge port, fails.
	 * 4. In egress, assume packet at bridge port has no VLAN before
	 *    Extended VLAN processing to save 2 Extended VLAN entries. This
	 *    can be changed later if required.
	 */
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "ext vlan info bp=%d logic=%d subif=%d\n",
		 vlan->bp, vlan->logic_port, vlan->subif_grp);
	ret = update_ctp(ops, vlan);

	if (ret < 0)
		return ret;

	/* prepare new private data */
	new_priv_size = sizeof(*new_priv);
	new_priv_size += sizeof(new_priv->bp[0])
			 * (vlan->n_vlan1 + vlan->n_vlan2);
	new_priv = kmalloc(new_priv_size, GFP_KERNEL);

	if (!new_priv)
		return -ENOMEM;

	new_priv->num_bp = (u32)(vlan->n_vlan1 + vlan->n_vlan2);

	for (i = j = 0; i < vlan->n_vlan1; i++, j++)
		new_priv->bp[j] = vlan->vlan1_list[i].bp;

	for (i = 0; i < vlan->n_vlan2; i++, j++)
		new_priv->bp[j] = vlan->vlan2_list[i].bp;

	/* remember pointer to old private data */
	old_priv = vlan->priv;

	if (!old_priv) {
		/* no old private data, vlan is added */
		ret = bp_add_vlan(ops, vlan, 0, &bpcfg);
	} else if (old_priv->num_bp < new_priv->num_bp) {
		/* vlan added */
		i = bp_diff(old_priv->bp, old_priv->num_bp, new_priv->bp);

		ret = bp_add_vlan(ops, vlan, i, &bpcfg);
	} else if (old_priv->num_bp > new_priv->num_bp) {
		/* vlan removed */
		i = bp_diff(new_priv->bp, new_priv->num_bp, old_priv->bp);

		bp_rm_vlan(ops, old_priv->bp[i], &bpcfg);
		ret = 0;
	} else {
		ret = 0;
	}

	/* return new private data in vlan structure */
	vlan->priv = new_priv;

	/* free old private data if any */
	kfree(old_priv);
	return ret;
}
