#include "datapath_ioctl.h"

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
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
		DP_DEBUG(DP_DBG_FLAG_INST,
			 "get_tsinfo done:%s\n",
			 dev->name);
	}

	/* NOTE: Timestamp should not be reported for all other ports and
	 * subif, report back 0, otherwise tcpdump for non-physical ports will
	 * fail
	 */
	return 0;
}
#endif

int dp_ops_set(void **dev_ops, int ops_cb_offset,
	       size_t ops_size, void **dp_orig_ops_cb,
	       void *dp_new_ops, void *new_ops_cb)
{
	void **dev_ops_cb = NULL;
	int i;

	if (!dev_ops) {
		pr_err("dev_ops NULL\n");
		return DP_FAILURE;
	}
	if (!dp_new_ops) {
		pr_err("dp_new_ops NULL\n");
		return DP_FAILURE;
	}
	if (*dev_ops != dp_new_ops) {
		if (*dev_ops) {
			*dp_orig_ops_cb = *dev_ops; /* save * old * ops * */
			for (i = 0; i < ops_size / sizeof(unsigned long *); i++)
				*((unsigned long *)(dp_new_ops) + i) =
					*((unsigned long *)(*dev_ops) + i);
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

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
static int dp_ndo_ptp_ioctl(struct net_device *dev,
			    struct ifreq *ifr, int cmd)
{
	int err = 0;
	struct mac_ops *ops;
	int inst = 0;
	struct pmac_port_info *port;
	u32 idx = 0;
	struct dp_dev *dp_dev = NULL;

	idx = dp_dev_hash(dev, NULL);
	dp_dev = dp_dev_lookup(&dp_dev_list[idx], dev, NULL, 0);
	if (!dp_dev) {
		pr_err("\n dp_dev NULL\n");
		return -EFAULT;
	}

	/* DP handles only SIOCSHWTSTAMP and SIOCGHWTSTAMP */
	if ((cmd != SIOCSHWTSTAMP) && (cmd != SIOCGHWTSTAMP)) {
		if (dp_dev->old_dev_ops->ndo_do_ioctl)
			err = dp_dev->old_dev_ops->ndo_do_ioctl(dev, ifr, cmd);
		else
			return -EFAULT;
		return err;
	}

	port = get_port_info_via_dev(dev);
	if (!port)
		return -EFAULT;

	ops = dp_port_prop[inst].mac_ops[port->port_id];
	if (!ops)
		return -EFAULT;

	switch (cmd) {
		case SIOCSHWTSTAMP: {
			err = ops->set_hwts(ops, ifr);
			if (err < 0) {
				port->f_ptp = 0;
				break;
			}
			port->f_ptp = 1;
			DP_DEBUG(DP_DBG_FLAG_DBG,
				 "PTP in SIOCGHWTSTAMP done\n");
			}
			break;
		case SIOCGHWTSTAMP: {
			ops->get_hwts(ops, ifr);
			DP_DEBUG(DP_DBG_FLAG_DBG,
				 "PTP in SIOCGHWTSTAMP done\n");
			break;
		}
	}

	return err;
}

int dp_register_ptp_ioctl(struct dp_dev *dp_dev,
			  struct net_device *dev, int inst)
{
	struct dp_cap cap;
	int err = DP_SUCCESS;

	if (!dev)
		return DP_FAILURE;
	if (!dp_dev)
		return DP_FAILURE;
	cap.inst = inst;
	dp_get_cap(&cap, 0);
	if (!cap.hw_ptp)
		return DP_FAILURE;

	/* netdev ops register */
	err = dp_ops_set((void **)&dev->netdev_ops,
			 offsetof(const struct net_device_ops, ndo_do_ioctl),
			 sizeof(*dev->netdev_ops),
			 (void **)&dp_dev->old_dev_ops,
			 &dp_dev->new_dev_ops,
			 &dp_ndo_ptp_ioctl);
	if (err)
		return DP_FAILURE;

	/* ethtool ops register */
	err = dp_ops_set((void **)&dev->ethtool_ops,
			 offsetof(const struct ethtool_ops, get_ts_info),
			 sizeof(*dev->ethtool_ops),
			 (void **)&dp_dev->old_ethtool_ops,
			 &dp_dev->new_ethtool_ops,
			 &get_tsinfo);
	if (err)
		return DP_FAILURE;

	DP_DEBUG(DP_DBG_FLAG_INST,
		 "dp_register_ptp_ioctl done:%s\n",
		 dev->name);
	return DP_SUCCESS;
}
#endif
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
	return DP_SUCCESS;
}

