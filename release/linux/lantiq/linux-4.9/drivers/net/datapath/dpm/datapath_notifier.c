/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#include <linux/module.h>
#include <linux/types.h>	/* size_t */
#include <linux/inetdevice.h>
#include <net/datapath_api.h>
#include <net/datapath_proc_api.h>
#include "datapath.h"
#include "datapath_instance.h"
#include "datapath_swdev_api.h"
#include "datapath_swdev.h"

static int dp_event(struct notifier_block *this,
		    unsigned long event, void *ptr);
static struct notifier_block dp_dev_notifier = {
	dp_event, /*handler*/
	NULL,
	0
};

int register_notifier(u32 flag)
{
	return register_netdevice_notifier(&dp_dev_notifier);
}

int unregister_notifier(u32 flag)
{
	return unregister_netdevice_notifier(&dp_dev_notifier);
}

int dp_event(struct notifier_block *this, unsigned long event, void *ptr)
{
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	struct net_device *dev;
	u8 *addr;
	int i;
	struct net_device *br_dev;
	struct dp_dev *dp_dev;
	struct br_info *br_info;
	int fid, inst, vap = 0;
	u32 idx;
	struct pmac_port_info *port;
	struct inst_property *prop;
	struct dp_subif_info *sif;

	dev = netdev_notifier_info_to_dev(ptr);
	if (!dev)
		return 0;
	addr = (u8 *)dev->dev_addr;
	if (!addr || dev->addr_len != ETH_ALEN) /*only support ethernet */
		return 0;
	if (is_zero_ether_addr(addr)) {
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "Skip zero mac address for %s\n", dev->name);
		return 0;
	}
	idx = dp_dev_hash(dev, NULL);
	dp_dev = dp_dev_lookup(&dp_dev_list[idx], dev, NULL, 0);
	if (!dp_dev) {
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "datapath dev(%s) not exists!!,No mac config\n",
			 dev ? dev->name : "NULL");
		return 0;
	}
	inst = dp_dev->inst;
	port = get_dp_port_info(inst, dp_dev->ep);
	prop = &dp_port_prop[inst];
	sif = get_dp_port_subif(port, vap);
	switch (event) {
	case NETDEV_GOING_DOWN:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "%s%d %s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
			 "Rem MAC with BP:",
			 sif->bp, "FID:", dp_dev->fid,
			 "dev:", dev ? dev->name : "NULL",
			 "MAC:", addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		for (i = 0; i < prop->info.cap.max_num_subif_per_port; i++) {
			if (get_dp_port_subif(port, i)->netif == dev) {
				vap = i;
				sif = get_dp_port_subif(port, vap);
				DP_DEBUG(DP_DBG_FLAG_NOTIFY, "vap:%d\n", vap);
				sif->fid = 0;
			}
		}
		prop->info.dp_mac_reset(0,
					dp_dev->fid,
					dp_dev->inst,
					addr);
	break;
	case NETDEV_CHANGEUPPER:
		dp_port_prop[inst].info.dp_mac_reset(0,
						     dp_dev->fid,
						     dp_dev->inst,
						     addr);
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "%s%d %s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
			 "Rem MAC with BP:",
			 sif->bp, "FID:", dp_dev->fid,
			 "dev:", dev ? dev->name : "NULL",
			 "MAC:", addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		if (!netif_is_bridge_port(dev)) {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY,
				 "chg upper:dev not a Bport\n");
		}
		br_dev = netdev_master_upper_dev_get(dev);
		if (!br_dev) {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY,
				 "chg upper:without br name\n");
				/* Set FID to Zero when br not attached to
				 * bport
				 */
			dp_dev->fid = 0;
			sif->fid = dp_dev->fid;
			goto dev_status;
		}
		/* Get respective FID when bport attached to bridge
		 */
		DP_DEBUG(DP_DBG_FLAG_NOTIFY, "Bridge name:%s\n",
			 br_dev ? br_dev->name : "NULL");
		br_info = dp_swdev_bridge_entry_lookup(br_dev->name);
		if (br_info) {
			dp_dev->fid = br_info->fid;
		} else {
			if (port->swdev_en == 1) {
				fid = dp_notif_br_alloc(br_dev);
				if (fid > 0) {
					dp_dev->fid = fid;
				} else {
					pr_err("FID alloc failed in %s\r\n",
					       __func__);
					return 0;
				}
			} else {
				dp_dev->fid = 0;
			}
		}
		for (i = 0; i < prop->info.cap.max_num_subif_per_port; i++) {
			if (get_dp_port_subif(port, i)->netif == dev) {
				vap = i;
				sif = get_dp_port_subif(port, vap);
				DP_DEBUG(DP_DBG_FLAG_NOTIFY, "vap:%d\n", vap);
				sif->fid = dp_dev->fid;
			}
		}
 dev_status:
		if (dev->flags & IFF_UP) {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY,
				 "%s%s%d%s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
				 "link UP,",
				 "ADD MAC with BP:",
				 sif->bp, " FID:", dp_dev->fid,
				 "dev:", dev ? dev->name : "NULL",
				 "MAC:", addr[0], addr[1], addr[2],
				 addr[3], addr[4], addr[5]);
			/* Note: For Linux network device's mac address,
			 *       its bridge port should be CPU port, not its
			 *       mapped bridge port in GSWIP
			 */
			prop->info.dp_mac_set(0,
					      dp_dev->fid,
					      dp_dev->inst,
					      addr);
		} else {
			DP_DEBUG(DP_DBG_FLAG_NOTIFY, "DEV:%s %s %s\n",
				 dev ? dev->name : "NULL",
				 "link down", "No MAC configuration");
		}
		break;
	case NETDEV_UP:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "%s%d %s%d %s%s %s%02x%02x%02x%02x%02x%02x\n",
			 "ADD MAC with BP:",
			 sif->bp, "FID:", dp_dev->fid,
			 "dev:", dev ? dev->name : "NULL",
			 "MAC:", addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		prop->info.dp_mac_set(0,
				      dp_dev->fid,
				      dp_dev->inst, addr);
	break;
	case NETDEV_UNREGISTER:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "DevUnreg %s:%02x:%02x:%02x:%02x:%02x:%02x\n",
			 dev->name,
			 addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		break;
	case NETDEV_CHANGEADDR:
		DP_DEBUG(DP_DBG_FLAG_NOTIFY,
			 "DevChg %s:%02x:%02x:%02x:%02x:%02x:%02x\n",
			 dev->name,
			 addr[0], addr[1], addr[2],
			 addr[3], addr[4], addr[5]);
		break;
	default:
		break;
	}
#endif
	return 0;
}
