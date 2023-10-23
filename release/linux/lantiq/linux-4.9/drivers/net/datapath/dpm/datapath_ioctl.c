// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#include "datapath_ioctl.h"

static int dp_ndo_ptp_ioctl(struct net_device *dev,
			    struct ifreq *ifr,
			    int cmd);

static int get_tsinfo(struct net_device *dev,
		      struct ethtool_ts_info *ts_info)
{
	struct mac_ops *ops;
	dp_subif_t subif = {0};
	int inst = 0;
	int err = 0;
	struct pmac_port_info *port_info;

	if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
		pr_err("%s dp_get_netif_subifid failed for %s\n",
		       __func__, dev->name);
		return -EFAULT;
	}

	port_info = get_dp_port_info(inst, subif.port_id);

	/* PTP is applicable to only physical & non-physical
	 * active Ethernet ports,
	 * For Prx300 portid should be 2,3,4
	 * For LGM portid should be 2 ... 10
	 */
	if (port_info->alloc_flags & (DP_F_FAST_ETH_LAN |
				      DP_F_FAST_ETH_WAN)) {
		ops = dp_port_prop[inst].mac_ops[subif.port_id];
		if (!ops)
			return -EFAULT;
		err = ops->mac_get_ts_info(ops, ts_info);
		if (err < 0)
			return -EFAULT;
		DP_DEBUG(DP_DBG_FLAG_INST, "%s done:%s\n", __func__, dev->name);
	}

	/* NOTE: Timestamp should not be reported for all other ports and
	 * subif, report back 0, otherwise tcpdump for non-physical ports will
	 * fail
	 */
	return 0;
}

int dp_ops_set(void **dev_ops, int ops_cb_offset,
	       size_t ops_size, void **dp_orig_ops_cb,
	       void *dp_new_ops, void *new_ops_cb)
{
	void **dev_ops_cb = NULL;

	if (!dev_ops) {
		pr_err("%s: dev_ops NULL\n", __func__);
		return DP_FAILURE;
	}
	if (!dp_new_ops) {
		pr_err("%s: dp_new_ops NULL\n", __func__);
		return DP_FAILURE;
	}
	if (*dev_ops != dp_new_ops) {
		if (*dev_ops) {
			*dp_orig_ops_cb = *dev_ops; /* save * old * ops * */
			memcpy(dp_new_ops, *dev_ops, ops_size);
		} else {
			*dp_orig_ops_cb = NULL;
		}
		*dev_ops = dp_new_ops;
	}
	/* callback for ops  */
	dev_ops_cb = (void **)((char *)dp_new_ops + ops_cb_offset);
	*dev_ops_cb = new_ops_cb;

	return	DP_SUCCESS;
}

static int dp_ndo_ptp_ioctl(struct net_device *dev,
			    struct ifreq *ifr, int cmd)
{
	int err = 0;
	struct mac_ops *ops;
	int inst = 0;
	struct pmac_port_info *port;
	struct dp_dev *dp_dev = NULL;
	dp_subif_t subif;

	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev) {
		pr_err("%s: dp_dev NULL\n", __func__);
		return -EFAULT;
	}

	/* DP handles only SIOCSHWTSTAMP and SIOCGHWTSTAMP */
	if (cmd != SIOCSHWTSTAMP && cmd != SIOCGHWTSTAMP) {
		if (dp_dev->old_dev_ops->ndo_do_ioctl)
			err = dp_dev->old_dev_ops->ndo_do_ioctl(dev, ifr, cmd);
		else
			return -EFAULT;
		return err;
	}

	if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
		pr_err("%s DP get subifid fail\n", __func__);
		return -EFAULT;
	}

	port = get_dp_port_info(inst, subif.port_id);

	ops = dp_port_prop[inst].mac_ops[subif.port_id];
	if (!ops)
		return -EFAULT;

	switch (cmd) {
	case SIOCSHWTSTAMP:
		err = ops->set_hwts(ops, ifr);
		if (err < 0) {
			port->f_ptp = 0;
			break;
		}
		port->f_ptp = 1;
		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "PTP in SIOCGHWTSTAMP done\n");
		break;
	case SIOCGHWTSTAMP:
		ops->get_hwts(ops, ifr);
		DP_DEBUG(DP_DBG_FLAG_DBG,
			 "PTP in SIOCGHWTSTAMP done\n");
		break;
	default:
		break;
	}

	return err;
}

int dp_register_ptp_ioctl(struct net_device *dev, int reset)
{
	struct dp_dev *dp_dev;
	struct dp_cap cap;
	void *cb[] = {&dp_ndo_ptp_ioctl, &get_tsinfo};
	int offset[] = {offsetof(const struct net_device_ops, ndo_do_ioctl),
			offsetof(const struct ethtool_ops, get_ts_info)};
	u32 flag[] = {DP_OPS_NETDEV, DP_OPS_ETHTOOL};
	int i;

	DP_DEBUG(DP_DBG_FLAG_OPS, "ptp_ops %s for %s\n",
		 reset ? "reset" : "update", dev->name);
	dp_dev = dp_dev_lookup(dev);
	if (!dp_dev)
		return DP_FAILURE;

	cap.inst = dp_dev->inst;
	dp_get_cap(&cap, 0);
	if (!cap.hw_ptp)
		return DP_FAILURE;

	for (i = 0; i < ARRAY_SIZE(offset); i++) {
		if (reset)
			flag[i] |= DP_OPS_RESET;
		if (dp_set_net_dev_ops_priv(dp_dev->dev, cb[i], offset[i],
					    flag[i])) {
			pr_err("%s failed to register ops %d\n", __func__, i);
			return DP_FAILURE;
		}
	}

	return DP_SUCCESS;
}

int dp_ops_reset(struct dp_dev *dp_dev,
		 struct net_device *dev)
{
	if (dev->netdev_ops == &dp_dev->new_dev_ops) {
		dev->netdev_ops = dp_dev->old_dev_ops;
		dp_dev->old_dev_ops = NULL;
	}
	if (dev->ethtool_ops == &dp_dev->new_ethtool_ops) {
		dev->ethtool_ops = dp_dev->old_ethtool_ops;
		dp_dev->old_ethtool_ops = NULL;
	}
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	if (dev->switchdev_ops == &dp_dev->new_swdev_ops) {
		dev->switchdev_ops = dp_dev->old_swdev_ops;
		dp_dev->old_swdev_ops = NULL;
	}
#endif
#if IS_ENABLED(CONFIG_INTEL_VPN)
	if (dev->xfrmdev_ops == &dp_dev->new_xfrm_ops) {
		dev->xfrmdev_ops = dp_dev->old_xfrm_ops;
		dp_dev->old_xfrm_ops = NULL;
	}
#endif
	return DP_SUCCESS;
}

