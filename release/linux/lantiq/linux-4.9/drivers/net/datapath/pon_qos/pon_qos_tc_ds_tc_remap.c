// SPDX-License-Identifier: GPL-2.0
/******************************************************************************
 *
 * Copyright (c) 2020 - 2021 MaxLinear, Inc.
 * Copyright (c) 2019-2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include <net/switch_api/gsw_types.h>
#include <net/switch_api/lantiq_gsw.h>
#include <net/switch_api/gsw_dev.h>
#include <net/datapath_api_vlan.h>
#include <net/datapath_api.h>
#include "pon_qos_tc_qos.h"

static int pon_qos_get_ctp_conf(struct net_device *dev,
				struct core_ops *ops,
				GSW_CTP_portConfig_t *ctp)
{
	dp_subif_t dp_subif = { 0 };
	struct ctp_ops *cops = &ops->gsw_ctp_ops;
	int ret = 0;

	if (!dev || !ops || !ctp)
		return -EINVAL;

	memset(ctp, 0, sizeof(*ctp));

	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, &dp_subif, 0);
	if (ret != DP_SUCCESS)
		return -ENODEV;

	ctp->nLogicalPortId = dp_subif.port_id;
	ctp->nSubIfIdGroup = dp_subif.subif_groupid;

	ctp->eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN;
	ctp->eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP;
	ret = cops->CTP_PortConfigGet(ops, ctp);
	if (ret != GSW_statusOk) {
		pr_err("%s: failed: %d\n", __func__, ret);
		return -EIO;
	}

	return 0;
}

#define PON_QOS_EVBLK_INV	-1

static int pon_qos_get_ev_blk(struct net_device *dev)
{
	struct core_ops *gswops;
	GSW_CTP_portConfig_t ctp = {0};
	int blk, blk_igmp, ret = 0;

	gswops = gsw_get_swcore_ops(0);
	if (!gswops)
		return -EINVAL;

	ret = pon_qos_get_ctp_conf(dev, gswops, &ctp);
	if (ret < 0)
		return ret;

	blk = ctp.nEgressExtendedVlanBlockId;
	blk_igmp = ctp.nEgressExtendedVlanBlockIdIgmp;
	if (ctp.bEgressExtendedVlanIgmpEnable &&
	    ctp.bEgressExtendedVlanEnable && blk == blk_igmp)
		return blk;

	return PON_QOS_EVBLK_INV;
}

static int pon_qos_ev_assign_blk(struct net_device *dev, int blk)
{
	struct core_ops *gswops;
	GSW_CTP_portConfig_t ctp = {0}, ctp2 = {0};
	int ret;

	gswops = gsw_get_swcore_ops(0);
	if (!gswops)
		return -EINVAL;

	ret = pon_qos_get_ctp_conf(dev, gswops, &ctp);
	if (ret < 0)
		return ret;

	ctp2.nLogicalPortId = ctp.nLogicalPortId;
	ctp2.nSubIfIdGroup = ctp.nSubIfIdGroup;

	ctp2.eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN;
	ctp2.eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP;
	if (blk >= 0) {
		ctp2.bEgressExtendedVlanEnable = 1;
		ctp2.bEgressExtendedVlanIgmpEnable = 1;
		ctp2.nEgressExtendedVlanBlockId = blk;
		ctp2.nEgressExtendedVlanBlockSize = 0;
		ctp2.nEgressExtendedVlanBlockIdIgmp = blk;
		ctp2.nEgressExtendedVlanBlockSizeIgmp = 0;
	} else {
		ctp2.bEgressExtendedVlanEnable = 0;
		ctp2.bEgressExtendedVlanIgmpEnable = 0;
		ctp2.nEgressExtendedVlanBlockId = 0;
		ctp2.nEgressExtendedVlanBlockSize = 0;
		ctp2.nEgressExtendedVlanBlockIdIgmp = 0;
		ctp2.nEgressExtendedVlanBlockSizeIgmp = 0;
	}
	ret = gswops->gsw_ctp_ops.CTP_PortConfigSet(gswops, &ctp2);
	if (ret != GSW_statusOk) {
		pr_err("%s: failed: %d\n", __func__, ret);
		return -EIO;
	}

	return 0;
}

static void pon_qos_ev_flt_init(struct dp_pattern_vlan *p)
{
	if (!p)
		return;

	p->vid = DP_VLAN_PATTERN_NOT_CARE;
	p->tpid = DP_VLAN_PATTERN_NOT_CARE;
	p->dei = DP_VLAN_PATTERN_NOT_CARE;
	p->proto = DP_VLAN_PATTERN_NOT_CARE;
	p->prio = DP_VLAN_PATTERN_NOT_CARE;
}

static void pon_qos_ev_free_stag_rules(struct list_head *l)
{
	struct dp_vlan1 *p, *n;

	list_for_each_entry_safe(p, n, l, list) {
		list_del(&p->list);
		kfree(p);
	}

	kfree(l);
}

static void pon_qos_ev_free_dtag_rules(struct list_head *l)
{
	struct dp_vlan2 *p, *n;

	list_for_each_entry_safe(p, n, l, list) {
		list_del(&p->list);
		kfree(p);
	}

	kfree(l);
}

/* This function adds default extVLAN rules to assign new traffic class based
 * on Pbit. Currently the switch hardware Pbit to TC table is consulted before
 * the extVLAN translation which can change the Pbit value. The rules make sure
 * that the right TC is assigned in this case.
 */
static int pon_qos_ev_assign_tc(struct net_device *dev,
				struct pon_qos_dot1p *dot1p,
				unsigned int dot1p_sz,
				int *nblk)
{
	struct dp_tc_vlan dp_vlan = {0};
	struct list_head *list1 = NULL;
	struct list_head *list2 = NULL;
	unsigned int idx = 0;
	int i, ret = 0;

	if (!dev || !dot1p || !nblk || dot1p_sz > PON_QOS_DOT1P_SZ)
		return -EINVAL;

	list1 = kzalloc(sizeof(*list1), GFP_KERNEL);
	if (!list1)
		return -ENOMEM;

	list2 = kzalloc(sizeof(*list2), GFP_KERNEL);
	if (!list2) {
		kfree(list1);
		return -ENOMEM;
	}

	INIT_LIST_HEAD(list1);
	INIT_LIST_HEAD(list2);
	dp_vlan.vlan1_head = list1;
	dp_vlan.vlan2_head = list2;

	/* HACK: remove the block first to avoid fragmentation so early */
	dp_vlan.dev = dev;
	dp_vlan.def_apply = DP_VLAN_APPLY_CTP;
	dp_vlan.dir = DP_DIR_EGRESS;
	dp_vlan.n_vlan0 = 0;
	dp_vlan.n_vlan1 = 0;
	dp_vlan.n_vlan2 = 0;

	ret = dp_vlan_set(&dp_vlan, 0);
	if (ret < 0)
		pr_err("Error configuring evlan TC assign\n");

	for (i = 0; i < dot1p_sz; i++) {
		struct dp_vlan1 *r1 = NULL;
		struct dp_vlan2 *r2 = NULL;

		if (dot1p[i].en) {
			r1 = kzalloc(sizeof(*r1), GFP_KERNEL);
			if (!r1) {
				ret = -ENOMEM;
				goto exit_free;
			}

			/* Add single tagged rule for TC assign based on Pbit */
			pon_qos_ev_flt_init(&r1->outer);
			r1->outer.prio = i;
			r1->act.ract.new_tc = i;
			r1->act.ract.act = DP_TC_REASSIGN;
			list_add_tail(&r1->list, list1);

			/* Add double tagged rule for TC assign based on Pbit */
			r2 = kzalloc(sizeof(*r2), GFP_KERNEL);
			if (!r2) {
				ret = -ENOMEM;
				goto exit_free;
			}

			pon_qos_ev_flt_init(&r2->outer);
			pon_qos_ev_flt_init(&r2->inner);
			r2->outer.prio = i;
			r2->act.ract.new_tc = i;
			r2->act.ract.act = DP_TC_REASSIGN;
			list_add_tail(&r2->list, list2);
			idx++;
		}
	}

	dp_vlan.dev = dev;
	dp_vlan.def_apply = DP_VLAN_APPLY_CTP;
	dp_vlan.dir = DP_DIR_EGRESS;
	dp_vlan.n_vlan0 = 0;
	dp_vlan.n_vlan1 = idx;
	dp_vlan.n_vlan2 = idx;

	ret = dp_vlan_set(&dp_vlan, 0);
	if (ret < 0)
		pr_err("%s: error configuring evlan TC assign\n", __func__);

exit_free:
	/* free the lists with tc reassign rules */
	pon_qos_ev_free_stag_rules(list1);
	pon_qos_ev_free_dtag_rules(list2);

	if (!ret) {
		*nblk = pon_qos_get_ev_blk(dev);
		pr_debug("%s: blk %d idx: %d\n", __func__, *nblk, idx);
		if (*nblk < 0 && idx) {
			pr_err("%s: evblk get failed\n", __func__);
			return -ENODEV;
		}
	}

	pr_debug("%s: TC reassign exit: %d\n", __func__, ret);
	return ret;
}

#define PON_QOS_DS_SUBIFS 4
#define PON_QOS_DS_STIDX 2
/**
 * The CTP network devices on the UNI ports should not have any EVLAN rules
 * configured on the egress CTP ports otherwise the queue assignment can be
 * broken. LCT interface is skipped as TC comes in the skb->priority there.
 * TODO: This code has to be removed when the DP APIs are extended to support
 * block set functions similar to the switch APIs. This should come together
 * with the block management and multicast ops. There is need for API to get
 * the CTP netdevs by using the based one.
 */
static int pon_qos_evblk_ctpdev_set(struct net_device *dev, int nblk)
{
	struct net_device *sdev;
	char intf[IFNAMSIZ];
	int i, rv = 0;

	for (i = PON_QOS_DS_STIDX; i < PON_QOS_DS_SUBIFS; i++) {
		/* TODO: only eth0_x_2/3, add better way to find them later */
		snprintf(intf, IFNAMSIZ, "%s_%d", dev->name, i);
		sdev = dev_get_by_name(&init_net, intf);
		if (!sdev)
			continue;

		rv = pon_qos_ev_assign_blk(sdev, nblk);
		if (rv < 0)
			pr_err("%s:%s evblk assign failed\n",
			       __func__, intf);

		dev_put(sdev);
	}

	return rv;
}

static int pon_qos_ev_assign_tc_upd(struct net_device *dev,
				    struct pon_qos_dot1p *dot1p,
				    unsigned int dot1p_sz)
{
	int rv = 0;
	int nblk = PON_QOS_EVBLK_INV;

	rv = pon_qos_evblk_ctpdev_set(dev, PON_QOS_EVBLK_INV);
	if (rv < 0) {
		pr_err("%s: evlan tc unassign failed\n", __func__);
		return rv;
	}

	rv = pon_qos_ev_assign_tc(dev, dot1p, dot1p_sz, &nblk);
	if (rv < 0) {
		pr_err("%s: evlan tc conf failed\n", __func__);
		return rv;
	}

	rv = pon_qos_evblk_ctpdev_set(dev, nblk);

	return rv;
}

int pon_qos_ev_tc_assign(struct pon_qos_qdisc *sch, u8 tc)
{
	struct net_device *dev = sch->dev;
	struct pon_qos_dot1p *dot1p = sch->dot1p;
	unsigned int dot1p_sz = ARRAY_SIZE(sch->dot1p);

	if (tc < dot1p_sz && !dot1p[tc].en) {
		dot1p[tc].en = 1;
		dot1p[tc].tcid = tc;
	}

	return pon_qos_ev_assign_tc_upd(dev, dot1p, dot1p_sz);
}

int pon_qos_ev_tc_unassign(struct pon_qos_qdisc *sch, u8 tc)
{
	struct net_device *dev = sch->dev;
	struct pon_qos_dot1p *dot1p = sch->dot1p;
	unsigned int dot1p_sz = ARRAY_SIZE(sch->dot1p);
	int i;

	for (i = 0; i < ARRAY_SIZE(sch->dot1p); i++) {
		if (dot1p[i].en && dot1p[i].tcid == tc) {
			dot1p[i].en = 0;
			break;
		}
	}

	return pon_qos_ev_assign_tc_upd(dev, dot1p, dot1p_sz);
}

int pon_qos_tc_mappings_init(void)
{
	struct core_ops *gsw_handle = NULL;
	GSW_return_t ret = 0;
	int i = 0;

	GSW_QoS_DSCP_ClassCfg_t dscp2tc = {
		.nTrafficClass = {
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
			0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15 },
	};
	GSW_QoS_SVLAN_PCP_ClassCfg_t spcp2tc = {
		.nTrafficClass = { 0, 1, 2, 3, 4, 5, 6, 7 },
	};
	GSW_QoS_portCfg_t qosPortCfg = {
		.eClassMode = GSW_QOS_CLASS_SELECT_SPCP_DSCP,
	};

	gsw_handle = gsw_get_swcore_ops(0);
	if (!gsw_handle) {
		pr_err("%s: Open SWAPI device FAILED!\n", __func__);
		return -EINVAL;
	}

	ret = gsw_handle->gsw_qos_ops.QoS_DSCP_ClassSet(gsw_handle, &dscp2tc);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_DSCP_ClassSet failed: %d\n", __func__, ret);
		return ret;
	}

	ret = gsw_handle->gsw_qos_ops.QoS_SVLAN_PCP_ClassSet(gsw_handle,
							     &spcp2tc);
	if (ret != GSW_statusOk) {
		pr_err("%s: QoS_SVLAN_PCP_ClassSet err: %d\n", __func__, ret);
		return ret;
	}

	/* FIXME: hardcoded port numbers for now */
	for (i = 2; i <= 4; i++) {
		qosPortCfg.nPortId = i;

		ret = gsw_handle->gsw_qos_ops.QoS_PortCfgSet(gsw_handle,
							     &qosPortCfg);
		if (ret != GSW_statusOk) {
			pr_err("%s: QoS_PortCfgSet failed for port %d: %d\n",
			       __func__, i, ret);
			return ret;
		}
	}

	return ret;
}
