/*
 * Copyright (C) Intel Corporation
 * Author: Shao Guohua <guohua.shao@intel.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */
#include<linux/init.h>
#include<linux/module.h>
#include <linux/kernel.h>
#include <linux/types.h>
#include <linux/version.h>
#include <linux/if_ether.h>
#include <linux/ethtool.h>
#include <linux/proc_fs.h>
#include <linux/delay.h>
#include <linux/init.h>
#include <linux/clk.h>
#include <linux/if_ether.h>
#include <linux/clk.h>
#include <linux/ip.h>
#include <net/ip.h>
#include <net/datapath_api.h>
#include <linux/if_vlan.h>
#include "datapath.h"
#include "datapath_instance.h"

struct net_device *get_base_dev(struct net_device *dev, int level);

/* return 0 -- succeed and supported by HW
 * return -1  -- Not VLAN interface or not supported case
 */
int get_vlan_via_dev(struct net_device *dev, struct vlan_prop *vlan_prop)
{
#if IS_ENABLED(CONFIG_VLAN_8021Q)
	struct vlan_dev_priv *vlan;
	struct net_device *base1, *base2;

	if (!vlan_prop)
		return -1;
	vlan_prop->num = 0;
	vlan_prop->base = NULL;
	if (!is_vlan_dev(dev))
		return 0;
	base1 = get_base_dev(dev, 1);
	vlan = dp_vlan_dev_priv(dev);
	if (!base1) { /*single vlan */
		pr_err("Not 1st VLAN interface no base\n");
		return -1;
	}
	if (is_vlan_dev(base1)) { /*double or more vlan*/
		base2 = get_base_dev(base1, 1);
		if (!base2) {
			pr_err("Not 2nd VLAN interface no base\n");
			return -1;
		}
		if (is_vlan_dev(base2)) {
			pr_err("Too many VLAN tag, not supoprt\n");
			return -1;
		}
		/*double vlan */
		vlan_prop->num = 2;
		vlan_prop->in_proto = vlan->vlan_proto;
		vlan_prop->in_vid = vlan->vlan_id;
		vlan = dp_vlan_dev_priv(base1);
		vlan_prop->out_proto = vlan->vlan_proto;
		vlan_prop->out_vid = vlan->vlan_id;
		vlan_prop->base = base2;
		return 0;
	}
	/*single vlan */
	vlan_prop->num = 1;
	vlan_prop->out_proto = vlan->vlan_proto;
	vlan_prop->out_vid = vlan->vlan_id;
	vlan_prop->base = base1;
#endif
	return 0;
}

struct logic_dev *logic_list_lookup(struct list_head *head,
				    struct net_device *dev)
{
	struct logic_dev *pos;

	list_for_each_entry(pos, head, list) {
		if (pos->dev == dev)
			return pos;
	}
	return NULL;
}

/*level >=1: only get the specified level if possible
 *otherwise: -1 maximum
 */
struct net_device *get_base_dev(struct net_device *dev, int level)
{
	struct net_device *lower_dev, *tmp;
	struct list_head *iter;

	tmp = dev;
	lower_dev = NULL;
	do {
		netdev_for_each_lower_dev(tmp, lower_dev, iter)
			break;
		if (lower_dev) {
			tmp = lower_dev;
			lower_dev = NULL;
			level--;
			if (level == 0)
				break;
		} else {
			break;
		}
	} while (1);
	if (tmp == dev)
		return NULL;
	return tmp;
}

/* add logic device into its base dev's logic dev list */
int add_logic_dev(int inst, int port_id, struct net_device *dev,
		  dp_subif_t *subif_id, u32 flags)
{
	struct logic_dev *logic_dev_tmp;
	struct net_device *base_dev;
	dp_subif_t subif;
	int masked_subif;
	struct pmac_port_info *port_info;
	struct dp_subif_info *sif;

	if (!dev) {
		pr_err("dev NULL\n");
		return -1;
	}
	base_dev = get_base_dev(dev, -1);
	if (!base_dev) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC,
			 "Not found base dev of %s\n", dev->name);
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_LOGIC,
		 "base_dev=%s for logic dev %s\n", base_dev->name, dev->name);
	if (dp_get_port_subitf_via_dev_private(base_dev, &subif)) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC,
			 "Not registered base dev %s in DP\n", dev->name);
		return -1;
	}
	port_info = get_dp_port_info(inst, port_id);
	masked_subif = GET_VAP(subif.subif,
			       port_info->vap_offset,
			       port_info->vap_mask);
	DP_DEBUG(DP_DBG_FLAG_LOGIC, "masked_subif=%x\n", masked_subif);
	sif = get_dp_port_subif(port_info, masked_subif);
	logic_dev_tmp = logic_list_lookup(&sif->logic_dev, dev);
	if (logic_dev_tmp) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC, "Device already exist: %s\n",
			 dev->name);
		return -1;
	}
	logic_dev_tmp = kmalloc(sizeof(*logic_dev_tmp), GFP_KERNEL);
	if (!logic_dev_tmp) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC, "kmalloc fail for %zd bytes\n",
			 sizeof(*logic_dev_tmp));
		return -1;
	}
	logic_dev_tmp->dev = dev;
	logic_dev_tmp->ep = port_id;
	logic_dev_tmp->ctp = subif.subif;
	if (dp_port_prop[inst].info.subif_platform_set_unexplicit(inst,
								  port_id,
								  logic_dev_tmp,
								  0)) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC, "dp_set_unexplicit fail\n");
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_LOGIC, "add logic dev list\n");
	list_add(&logic_dev_tmp->list, &sif->logic_dev);

	subif_id->bport = logic_dev_tmp->bp;
	subif_id->subif = subif.subif;
	dp_inst_add_dev(dev, NULL,
			inst, port_id,
			logic_dev_tmp->bp,
			subif.subif, flags);
	return 0;
}

int del_logic_dev(int inst, struct list_head *head, struct net_device *dev,
		  u32 flags)
{
	struct logic_dev *logic_dev;

	logic_dev = logic_list_lookup(head, dev);
	if (!logic_dev) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC, "Not find %s in logic dev list\n",
			 dev->name);
		return -1;
	}
	dp_port_prop[inst].info.subif_platform_set_unexplicit(inst,
		logic_dev->ep,
		logic_dev, flags);
	dp_inst_del_dev(dev, NULL, inst, logic_dev->ep, logic_dev->ctp, 0);
	list_del(&logic_dev->list);
	kfree(logic_dev);

	return 0;
}

