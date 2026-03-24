/******************************************************************************
 *
 * Copyright (c) 2019 Intel Corporation
 *
 * For licensing information, see the file 'LICENSE' in the root folder of
 * this software module.
 *
 *****************************************************************************/

#include <linux/netdevice.h>
#include "qos_mgr_stack_al.h"
#include <uapi/net/qos_mgr_common.h>
#include "qos_mgr_tc.h"

static int32_t qos_mgr_get_ctp_conf(struct net_device *dev,
				    struct core_ops *ops,
				    GSW_CTP_portConfig_t *ctp)
{
	dp_subif_t *dp_subif = NULL;
	struct ctp_ops *cops = &ops->gsw_ctp_ops;
	int ret = 0;

	if (!dev || !ops || !ctp)
		return QOS_MGR_FAILURE;

	memset(ctp, 0, sizeof(*ctp));

	dp_subif = kzalloc(sizeof(dp_subif_t), GFP_ATOMIC);
	if (!dp_subif) {
		pr_err("%s: DP subif allocation failed\n", __func__);
		return QOS_MGR_FAILURE;
	}

	ret = dp_get_netif_subifid(dev, NULL, NULL, NULL, dp_subif, 0);
	if (ret != DP_SUCCESS) {
		kfree(dp_subif);
		return QOS_MGR_FAILURE;
	}
	ctp->nLogicalPortId = dp_subif->port_id;
	ctp->nSubIfIdGroup = dp_subif->subif >> 8;

	kfree(dp_subif);

	ctp->eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN;
	ctp->eMask |= GSW_CTP_PORT_CONFIG_MASK_EGRESS_VLAN_IGMP;
	ret = cops->CTP_PortConfigGet(ops, ctp);
	if (ret != GSW_statusOk) {
		pr_err("%s: gsw ctp config get failed: %d\n", __func__, ret);
		return QOS_MGR_FAILURE;
	}

	return QOS_MGR_SUCCESS;
}

#define QOS_MGR_EVBLK_INV	-1

static int32_t qos_mgr_get_ev_blk(struct net_device *dev)
{
	struct core_ops *gswops;
	GSW_CTP_portConfig_t ctp = {0};
	int32_t blk, blk_igmp, ret = 0;

	gswops = gsw_get_swcore_ops(0);
	if (!gswops)
		return QOS_MGR_FAILURE;

	ret = qos_mgr_get_ctp_conf(dev, gswops, &ctp);
	if (ret < 0)
		return QOS_MGR_FAILURE;

	blk = ctp.nEgressExtendedVlanBlockId;
	blk_igmp = ctp.nEgressExtendedVlanBlockIdIgmp;
	if (ctp.bEgressExtendedVlanIgmpEnable &&
	    ctp.bEgressExtendedVlanEnable && blk == blk_igmp)
		return blk;

	return QOS_MGR_EVBLK_INV;
}

static int32_t qos_mgr_ev_assign_blk(struct net_device *dev, int32_t blk)
{
	struct core_ops *gswops;
	GSW_CTP_portConfig_t ctp = {0}, ctp2 = {0};
	int32_t ret;

	gswops = gsw_get_swcore_ops(0);
	if (!gswops)
		return QOS_MGR_FAILURE;

	ret = qos_mgr_get_ctp_conf(dev, gswops, &ctp);
	if (ret < 0)
		return QOS_MGR_FAILURE;

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
		pr_err("%s: gsw ctp config set failed: %d\n", __func__, ret);
		return QOS_MGR_FAILURE;
	}

	return 0;
}

/*
 * This function adds default extVLAN rules to assign new traffic class based
 * on Pbit. Currently the switch hardware Pbit to TC table is consulted before
 * the extVLAN translation which can change the Pbit value. The rules make sure
 * that the right TC is assigned in this case.
 */
#define MAX_DS_QUEUE_NUM	8

static int32_t qos_mgr_ev_assign_tc(struct net_device *dev,
				    struct qos_mgr_dot1p *dot1p,
				    unsigned int dot1p_sz,
				    int *nblk)
{
	struct dp_vlan1 *rule1 = NULL;
	struct dp_vlan2 *rule2 = NULL;
	struct dp_tc_vlan dp_vlan = {0};
	unsigned int idx = 0;
	int i, ret = 0;


	if (!dev || !dot1p || !nblk || dot1p_sz > QOS_MGR_DOT1P_SZ)
		return -EINVAL;

	rule1 = kzalloc(sizeof(struct dp_vlan1) * MAX_DS_QUEUE_NUM, GFP_KERNEL);
	if (!rule1)
		return -ENOMEM;
	rule2 = kzalloc(sizeof(struct dp_vlan2) * MAX_DS_QUEUE_NUM, GFP_KERNEL);
	if (!rule2) {
		kfree(rule1);
		return -ENOMEM;
	}

	/* HACK: remove the block first to avoid fragmentation so early */
	dp_vlan.dev = dev;
	dp_vlan.def_apply = DP_VLAN_APPLY_CTP;
	dp_vlan.dir = DP_DIR_EGRESS;
	dp_vlan.vlan0_list = NULL;
	dp_vlan.vlan1_list = rule1;
	dp_vlan.vlan2_list = rule2;
	dp_vlan.n_vlan0 = 0;
	dp_vlan.n_vlan1 = dp_vlan.n_vlan2 = 0;

	ret = dp_vlan_set(&dp_vlan, 0);
	if (ret < 0)
		pr_err("Error configuring evlan TC assign\n");

	for (i = 0; i < dot1p_sz; i++) {
		struct dp_vlan1 *r1 = &rule1[idx];
		struct dp_vlan2 *r2 = &rule2[idx];

		if (dot1p[i].en) {
			/* Add single tagged rule for TC assign based on Pbit */
			r1->outer.prio = i;
			r1->outer.vid = DP_VLAN_PATTERN_NOT_CARE;
			r1->outer.tpid = DP_VLAN_PATTERN_NOT_CARE;
			r1->outer.dei = DP_VLAN_PATTERN_NOT_CARE;
			r1->outer.proto = DP_VLAN_PATTERN_NOT_CARE;
			r1->act.ract.new_tc = i;
			r1->act.ract.act = DP_TC_REASSIGN;

			/* Add double tagged rule for TC assign based on Pbit */
			r2->outer.prio = i;
			r2->outer.vid = DP_VLAN_PATTERN_NOT_CARE;
			r2->outer.tpid = DP_VLAN_PATTERN_NOT_CARE;
			r2->outer.dei = DP_VLAN_PATTERN_NOT_CARE;
			r2->outer.proto = DP_VLAN_PATTERN_NOT_CARE;
			r2->inner.prio = DP_VLAN_PATTERN_NOT_CARE;
			r2->inner.vid = DP_VLAN_PATTERN_NOT_CARE;
			r2->inner.tpid = DP_VLAN_PATTERN_NOT_CARE;
			r2->inner.dei = DP_VLAN_PATTERN_NOT_CARE;
			r2->inner.proto = DP_VLAN_PATTERN_NOT_CARE;
			r2->act.ract.new_tc = i;
			r2->act.ract.act = DP_TC_REASSIGN;
			idx++;
		}
	}

	dp_vlan.dev = dev;
	dp_vlan.def_apply = DP_VLAN_APPLY_CTP;
	dp_vlan.dir = DP_DIR_EGRESS;
	dp_vlan.vlan0_list = NULL;
	dp_vlan.vlan1_list = rule1;
	dp_vlan.vlan2_list = rule2;
	dp_vlan.n_vlan0 = 0;
	dp_vlan.n_vlan1 = dp_vlan.n_vlan2 = idx;

	ret = dp_vlan_set(&dp_vlan, 0);
	if (ret < 0)
		pr_err("%s: error configuring evlan TC assign\n", __func__);

	kfree(rule1);
	kfree(rule2);

	if (!ret) {
		*nblk = qos_mgr_get_ev_blk(dev);
		pr_debug("%s: blk %d idx: %d\n", __func__, *nblk, idx);
		if (*nblk < 0 && idx) {
			pr_err("%s: evblk get failed\n", __func__);
			return QOS_MGR_FAILURE;
		}
	}

	pr_debug("%s: evlan TC assign rules configured: %d\n", __func__, ret);
	return ret;
}

#define QOS_MGR_DS_SUBIFS 4
#define QOS_MGR_DS_STIDX 2
/**
 * The CTP network devices on the UNI ports should not have any EVLAN rules
 * configured on the egress CTP ports otherwise the queue assignment can be
 * broken. LCT interface is skipped as TC comes in the skb->priority there.
 * TODO: This code has to be removed when the DP APIs are extended to support
 * block set functions similar to the switch APIs. This should come together
 * with the block management and multicast ops. There is need for API to get
 * the CTP netdevs by using the based one.
 */
static int32_t qos_mgr_evblk_ctpdev_set(struct net_device *dev, int nblk)
{
	struct net_device *sdev;
	char intf[IFNAMSIZ];
	int i, rv = 0;

	for (i = QOS_MGR_DS_STIDX; i < QOS_MGR_DS_SUBIFS; i++) {
		/* TODO: only eth0_x_2/3, add better way to find them later */
		snprintf(intf, IFNAMSIZ, "%s_%d", dev->name, i);
		sdev = dev_get_by_name(&init_net, intf);
		if (!sdev)
			continue;

		rv = qos_mgr_ev_assign_blk(sdev, nblk);
		if (rv < 0)
			pr_err("%s:%s evblk assign failed\n",
				 __func__, intf);

		dev_put(sdev);
	}

	return rv;
}

static int32_t qos_mgr_ev_assign_tc_upd(struct net_device *dev,
					struct qos_mgr_dot1p *dot1p,
					unsigned int dot1p_sz)
{
	int rv = 0;
	int nblk = QOS_MGR_EVBLK_INV;

	rv = qos_mgr_evblk_ctpdev_set(dev, QOS_MGR_EVBLK_INV);
	if (rv < 0) {
		pr_err("%s: evlan tc unassign failed\n", __func__);
		return -1;
	}

	rv = qos_mgr_ev_assign_tc(dev, dot1p, dot1p_sz, &nblk);
	if (rv < 0) {
		pr_err("%s: evlan tc conf failed\n", __func__);
		return -1;
	}

	rv = qos_mgr_evblk_ctpdev_set(dev, nblk);

	return rv;
}

int32_t qos_mgr_ev_qassign(struct _tc_qos_mgr_db *dbi,
			   unsigned int tcid, uint8_t pbit)
{
	struct net_device *dev = dbi->dev;
	struct qos_mgr_dot1p *dot1p = dbi->dot1p;
	unsigned int dot1p_sz = ARRAY_SIZE(dbi->dot1p);

	if (pbit < dot1p_sz && !dot1p[pbit].en) {
		dot1p[pbit].en = 1;
		dot1p[pbit].tcid = tcid;
	}

	return qos_mgr_ev_assign_tc_upd(dev, dot1p, dot1p_sz);
}

int32_t qos_mgr_ev_qunassign(struct _tc_qos_mgr_db *dbi, unsigned int tcid)
{
	struct net_device *dev = dbi->dev;
	struct qos_mgr_dot1p *dot1p = dbi->dot1p;
	unsigned int dot1p_sz = ARRAY_SIZE(dbi->dot1p);
	int i;

	for (i = 0; i < ARRAY_SIZE(dbi->dot1p); i++) {
		if (dot1p[i].en && dot1p[i].tcid == tcid) {
			dot1p[i].en = 0;
			break;
		}
	}

	return qos_mgr_ev_assign_tc_upd(dev, dot1p, dot1p_sz);
}
