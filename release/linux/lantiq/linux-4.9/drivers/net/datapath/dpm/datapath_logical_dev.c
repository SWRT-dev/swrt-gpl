// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#include <linux/types.h>
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

/* level > 0: search until specified level is reached
 * level < 0: search for maximum depth
 */
struct net_device *get_base_dev(struct net_device *dev, int level)
{
	struct net_device *cdev = dev;
	struct net_device *ldev;
	struct list_head *iter;

	/* klocwork workaround
	 * code logic comes from netdev_for_each_lower_dev
	 */
	while (cdev && level) {
		iter = cdev->adj_list.lower.next,
		ldev = netdev_lower_get_next(cdev, &iter);
		if (!ldev)
			break;
		cdev = ldev;
		level--;
	}
	return cdev == dev ? NULL : cdev;
}

/* add logic device into its base dev's logic dev list */
int add_logic_dev(int inst, int port_id, struct net_device *dev,
		  dp_subif_t *subif_id, struct dp_subif_data *data, u32 flags)
{
	struct logic_dev *logic_dev_tmp;
	struct net_device *base_dev;
	dp_subif_t subif;
	int masked_subif;
	struct pmac_port_info *port_info;
	struct dp_subif_info *sif;
	struct inst_info *dp_info = get_dp_prop_info(inst);

	if (!dev) {
		pr_err("dev NULL\n");
		return -1;
	}
	base_dev = get_base_dev(dev, -1);
	if (!base_dev) {
		pr_err("Not found base dev of %s\n", dev->name);
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_LOGIC,
		 "base_dev=%s for logic dev %s\n", base_dev->name, dev->name);
	if (dp_get_netif_subifid(base_dev, NULL, NULL, NULL, &subif, 0)) {
		pr_err("Not registered base dev %s in DP\n", dev->name);
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
		pr_err("Device already exist: %s\n", dev->name);
		return -1;
	}
	logic_dev_tmp = devm_kzalloc(&g_dp_dev->dev,
				     sizeof(*logic_dev_tmp), GFP_ATOMIC);
	if (!logic_dev_tmp) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC, "kmalloc fail for %zd bytes\n",
			 sizeof(*logic_dev_tmp));
		return -1;
	}
	logic_dev_tmp->dev = dev;
	logic_dev_tmp->ep = port_id;
	logic_dev_tmp->ctp = subif.subif;
	if (dp_info->subif_platform_set_unexplicit(inst, port_id,
						   logic_dev_tmp,
						   data->flag_ops)) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC, "dp_set_unexplicit fail\n");
		return -1;
	}
	DP_DEBUG(DP_DBG_FLAG_LOGIC, "add logic dev list\n");
	list_add(&logic_dev_tmp->list, &sif->logic_dev);

	subif_id->bport = logic_dev_tmp->bp;
	subif_id->subif = subif.subif;
	dp_inst_add_dev(dev, inst, port_id, logic_dev_tmp->bp,
			subif.subif, flags);
	return 0;
}

int del_logic_dev(int inst, struct list_head *head, struct net_device *dev,
		  u32 flags)
{
	struct logic_dev *logic_dev;
	struct inst_info *dp_info = get_dp_prop_info(inst);

	logic_dev = logic_list_lookup(head, dev);
	if (!logic_dev) {
		DP_DEBUG(DP_DBG_FLAG_LOGIC, "Not find %s in logic dev list\n",
			 dev->name);
		return -1;
	}
	dp_info->subif_platform_set_unexplicit(inst, logic_dev->ep, logic_dev,
					       flags);
	dp_inst_del_dev(dev, inst, logic_dev->ep, logic_dev->ctp, flags);
	list_del(&logic_dev->list);
	devm_kfree(&g_dp_dev->dev, logic_dev);

	return 0;
}

