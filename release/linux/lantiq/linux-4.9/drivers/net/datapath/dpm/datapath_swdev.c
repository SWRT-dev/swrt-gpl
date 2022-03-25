/*
 * Copyright (C) Intel Corporation
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
#include <linux/if_bridge.h>
#include <linux/clk.h>
#include <linux/ip.h>
#include <linux/if_vlan.h>
#include <net/ip.h>
#include <linux/ethtool.h>
#include <net/rtnetlink.h>
#include <generated/utsrelease.h>
#include <net/datapath_api.h>
#include "datapath_swdev.h"
#include "datapath.h"
#include "datapath_instance.h"
#include "datapath_ioctl.h"

static void dp_swdev_insert_bridge_id_entry(struct br_info *);
static void dp_swdev_remove_bridge_id_entry(struct br_info *);
static int dp_swdev_add_bport_to_list(struct br_info *br_item,
				      int bport);
static int dp_swdev_del_bport_from_list(struct br_info *br_item,
					int bport);

struct hlist_head
	g_bridge_id_entry_hash_table[DP_MAX_INST][BR_ID_ENTRY_HASH_TABLE_SIZE];

DP_DEFINE_LOCK(dp_swdev_lock);
static inline void swdev_lock(void)
{
	DP_LIB_LOCK(&dp_swdev_lock);
}

static inline void swdev_unlock(void)
{
	DP_LIB_UNLOCK(&dp_swdev_lock);
}

u16 dp_swdev_cal_hash(unsigned char *name)
{
	size_t hash = 5381;

	while (*name)
		hash = 33 * hash ^ (unsigned char)*name++;
	return (u16)(hash & 0x3F);
}

struct hlist_head *get_dp_g_bridge_id_entry_hash_table_info(int instance,
							    int index)
{
	return &g_bridge_id_entry_hash_table[instance][index];
}
EXPORT_SYMBOL(get_dp_g_bridge_id_entry_hash_table_info);

int dp_get_fid_by_brname(struct net_device *dev, int *inst)
{
	struct br_info *br_info;

	br_info = dp_swdev_bridge_entry_lookup(dev->name);
	if (!br_info)
		return -1;

	*inst = br_info->inst;
	return br_info->fid;
}
EXPORT_SYMBOL(dp_get_fid_by_brname);

int dp_swdev_chk_bport_in_br(struct net_device *bp_dev, int bport, int inst)
{
	struct net_device *br_dev;
	struct bridge_member_port *temp_list = NULL;
	struct br_info *br_info;
	int found = 0;

	br_dev = netdev_master_upper_dev_get(bp_dev);
	if (!br_dev)
		return -1;
	br_info = dp_swdev_bridge_entry_lookup(br_dev->name);
	if (!br_info)
		return -1;
	list_for_each_entry(temp_list, &br_info->bp_list, list) {
		if (temp_list->portid == bport) {
			found = 1;
			DP_DEBUG(DP_DBG_FLAG_SWDEV, "bport(%s) in bridge\n",
				 bp_dev->name ? bp_dev->name : "NULL");
			return 0;
		}
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "bport(%s) not in bridge\n",
		 bp_dev->name ? bp_dev->name : "NULL");
	return -1;
}

struct br_info *dp_swdev_bridge_entry_lookup(char *br_name)
{
	u16 idx;
	struct br_info *br_item = NULL;
	struct hlist_head *tmp;
	int i = 0;

	idx = dp_swdev_cal_hash(br_name);
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "hash index:%d\n", idx);
	for (i = 0; i < DP_MAX_INST; i++) {
		tmp = (&g_bridge_id_entry_hash_table[i][idx]);
		hlist_for_each_entry(br_item, tmp, br_hlist) {
			if (br_item) {
				if (strcmp(br_name,
					   br_item->br_device_name) == 0) {
					DP_DEBUG(DP_DBG_FLAG_SWDEV,
						 "hash entry found(%s)\n",
						 br_name);
					return br_item;
				}
			} else {
				break;
			}
		}
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "No hash entry found(%s)\n",
		 br_name);
	return NULL;
}

static void dp_swdev_remove_bridge_id_entry(struct br_info *br_item)
{
	/*reset switch bridge configurations*/
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "hash del\n");
	hlist_del(&br_item->br_hlist);
	kfree(br_item);
}

static void dp_swdev_insert_bridge_id_entry(struct br_info
					    *br_item)
{
	u16 idx;
	struct hlist_head *tmp;

	idx = dp_swdev_cal_hash(br_item->br_device_name);
	tmp = (&g_bridge_id_entry_hash_table[br_item->inst][idx]);
	hlist_add_head(&br_item->br_hlist, tmp);
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "hash added idx:%d bridge(%s)\n",
		 idx, br_item->br_device_name);
}

int dp_swdev_bridge_id_entry_free(int instance)
{
	u32 index;
	struct hlist_node *tmp;
	struct br_info *br_item;

	for (index = 0; index < BR_ID_ENTRY_HASH_TABLE_SIZE;
		index++) {
		hlist_for_each_entry_safe
			(br_item, tmp,
			 &g_bridge_id_entry_hash_table[instance][index],
			 br_hlist) {
			dp_swdev_remove_bridge_id_entry(br_item);
		}
	}
	return 0;
}

int dp_swdev_bridge_id_entry_init(void)
{
	int i, j;

	for (i = 0; i < DP_MAX_INST; i++)
		for (j = 0; j < BR_ID_ENTRY_HASH_TABLE_SIZE; j++)
			INIT_HLIST_HEAD(&g_bridge_id_entry_hash_table[i][j]);

	return 0;/*<success>*/
}

static int dp_swdev_del_bport_from_list(struct br_info *br_item,
					int bport)
{
	int found = 0;
	struct bridge_member_port *temp_list = NULL;

	list_for_each_entry(temp_list, &br_item->bp_list, list) {
		if (temp_list->portid == bport) {
			found = 1;
			break;
		}
	}
	if (found) {
		list_del(&temp_list->list);
		kfree(temp_list);
		return 1;
	}
	return 0;
}

static int dp_swdev_add_bport_to_list(struct br_info *br_item,
				      int bport)
{
	int found = 0;
	struct bridge_member_port *bport_list = NULL;
	struct bridge_member_port *temp_list = NULL;

	list_for_each_entry(temp_list, &br_item->bp_list, list) {
		if (temp_list->portid == bport) {
			found = 1;
			break;
		}
	}
	if (found == 0) {
		bport_list = (struct bridge_member_port *)
			kmalloc(sizeof(struct bridge_member_port),
				GFP_KERNEL);
		if (!bport_list) {
			pr_err("\n Node creation failed\n");
			return -1;
		}
		bport_list->dev_reg_flag = br_item->flag;
		bport_list->portid = bport;
		bport_list->alloc_flag = br_item->alloc_flag;
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "bport:%d reg_flag:%d\n",
			 bport_list->portid, bport_list->dev_reg_flag);
		list_add(&bport_list->list, &br_item->bp_list);
	}
	return 0;
}

static int dp_swdev_clr_gswip_cfg(struct bridge_id_entry_item *br_item,
				  u8 *addr)
{
	struct br_info *br_info;
	struct pmac_port_info *p_info;
	struct inst_info *i_info;
	int ret;

	if (br_item->flags == BRIDGE_NO_ACTION) {
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "bport not added so no action required\n");
		return 0;
	}
	br_info = dp_swdev_bridge_entry_lookup(br_item->br_device_name);
	if (!br_info)
		return 0;
	if (!dp_swdev_del_bport_from_list(br_info, br_item->portid))
		return 0;
	p_info = get_dp_port_info(br_item->inst, br_item->dp_port);
	if (p_info->swdev_en != 1)
		return 0;
	i_info = &dp_port_prop[br_item->inst].info;
	ret = i_info->swdev_bridge_port_cfg_reset(br_info, br_item->inst,
						  br_item->portid);
	if (ret == DEL_BRENTRY) {
		i_info->swdev_free_brcfg(br_item->inst, br_item->fid);
		dp_swdev_remove_bridge_id_entry(br_info);
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "rem bport(%d),bridge(%s)\n",
			 br_item->portid, br_item->br_device_name);
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "rem bport(%d)\n", br_item->portid);
	return 0;
}

static int dp_swdev_cfg_vlan(struct bridge_id_entry_item *br_item,
			     struct net_device *dev)
{
	struct dp_dev *dp_dev;
	u32 idx, inst;
	int vap;

	if (br_item->flags & LOGIC_DEV_REGISTER) {
		idx = dp_dev_hash(dev, NULL);
		dp_dev = dp_dev_lookup(&dp_dev_list[idx], dev, NULL, 0);
		if (!dp_dev) {
			pr_err("\n dp_dev NULL\n");
			/* Cannot return -1 from here as this fn is
			 * called by swdev commit phase
			 */
			return 0;
		}
		inst = br_item->inst;
		vap = GET_VAP(dp_dev->ctp,
			      get_dp_port_info(inst, dp_dev->ep)->vap_offset,
			      get_dp_port_info(inst, dp_dev->ep)->vap_mask);
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "vap=%d ep=%d bp=%d\n",
			 vap, dp_dev->ep, dp_dev->bp);
		dp_port_prop[br_item->inst].info.dp_cfg_vlan(br_item->inst,
							     vap, dp_dev->ep);
	}
	return 0;
}

static int dp_swdev_filter_vlan(struct net_device *dev,
				const struct switchdev_obj *obj,
				struct switchdev_trans *trans,
				struct net_device *br_dev)
{
	struct br_info *br_info;
	struct bridge_id_entry_item *br_item;
	dp_subif_t subif = {0};

	if (switchdev_trans_ph_prepare(trans)) {
		/*Get current BPORT ID,instance from DP*/
		if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
			pr_err("%s dp_get_netif_subifid failed for %s\n",
			       __func__, dev->name);
			return -EOPNOTSUPP;
		}
		br_item = kmalloc(sizeof(*br_item), GFP_KERNEL);
		if (!br_item)
			//TODO need to check dequeue if no memory
			return -ENOMEM;
		br_item->inst = subif.inst;
		/* current bridge member port*/
		br_item->portid = subif.bport;
		swdev_lock();
		br_info = dp_swdev_bridge_entry_lookup(br_dev->name);
		if (br_info) {
			strcpy(br_item->br_device_name,
			       br_info->br_device_name);
			br_item->fid = br_info->fid;
		} else {
			return -EOPNOTSUPP;
		}
		switchdev_trans_item_enqueue(trans, br_item,
					     kfree, &br_item->tritem);
		swdev_unlock();
		return 0;
	}
	br_item = switchdev_trans_item_dequeue(trans);
	if (br_item)
		dp_swdev_cfg_vlan(br_item, dev);
	return 0;
}

static int dp_swdev_cfg_gswip(struct bridge_id_entry_item *br_item,
			      struct net_device *dev)
{
	struct br_info *br_info;
	struct pmac_port_info *p_info;
	struct inst_info *i_info;
	int ret;

	p_info = get_dp_port_info(br_item->inst, br_item->dp_port);
	i_info = &dp_port_prop[br_item->inst].info;
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "britem flags:%x\n", br_item->flags);

	if (br_item->flags & ADD_BRENTRY) {
		if (p_info->swdev_en == 0) {
			DP_DEBUG(DP_DBG_FLAG_SWDEV, "swdev disable for bp %d\n",
				 br_item->portid);
			return 0;
		}
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "Add br entry %s\n",
			 br_item->br_device_name);

		ret = i_info->swdev_bridge_cfg_set(br_item->inst, br_item->fid);
		if (ret == 0) {
			br_info = kmalloc(sizeof(*br_info), GFP_KERNEL);
			if (!br_info) {
				pr_err("Switch cfg Fail as kmalloc %zd bytes fail\n",
				       sizeof(*br_info));
				/*TODO need to check return value
				 *for switchdev commit
				 */
				return 0;
			}

			br_info->fid = br_item->fid;
			br_info->inst = br_item->inst;
			br_info->cpu_port = ENABLE;
			br_info->alloc_flag = br_item->alloc_flag;

			/* Logic dev flag added to verify if SWDEV registered
			 * the logical i.e. VLAN device.Helpful during
			 * br/bport delete
			 */
			if (br_item->flags & LOGIC_DEV_REGISTER)
				br_info->flag = LOGIC_DEV_REGISTER;

			strcpy(br_info->br_device_name,
			       br_item->br_device_name);
			INIT_LIST_HEAD(&br_info->bp_list);
			dp_swdev_insert_bridge_id_entry(br_info);
			dp_swdev_add_bport_to_list(br_info, br_item->portid);


			i_info->swdev_bridge_port_cfg_set(br_info,
							  br_item->inst,
							  br_item->portid,
							  dev->priv_flags);
			br_item->flags &= ~ADD_BRENTRY;
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "added bport(%d),bridge(%s)\n",
				 br_item->portid,
				 br_info->br_device_name);
			return 0;
		}
	} else {
		br_info = dp_swdev_bridge_entry_lookup(br_item->br_device_name);
		if (!br_info)
			return 0;
		br_info->flag = 0;

		br_info->alloc_flag = br_item->alloc_flag;

		if (br_item->flags & LOGIC_DEV_REGISTER)
			br_info->flag = LOGIC_DEV_REGISTER;
		dp_swdev_add_bport_to_list(br_info, br_item->portid);
		if (p_info->swdev_en == 1) {
			i_info->swdev_bridge_port_cfg_set(br_info,
							  br_item->inst,
							  br_item->portid,
							  dev->priv_flags);
		}
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "added bport(%d)\n",
			 br_item->portid);
		return 0;
	}
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "No configuration,Pls check!!\n");
	return 0;
}

static int dp_swdev_add_if(struct net_device *dev,
			   const struct switchdev_attr *attr,
			   struct switchdev_trans *trans,
			   struct net_device *br_dev)
{
	struct br_info *br_info;
	struct pmac_port_info *p_info;
	struct inst_info *i_info;
	struct bridge_id_entry_item *br_item;
	int br_id = 0;
	struct net_device *base;
	dp_subif_t subif = {0};
	u32 flag = 0;
	int port, inst;
	int32_t res;

	/* SWITCHDEV_TRANS_PREPARE phase */
	if (switchdev_trans_ph_prepare(trans)) {
		/*Get current BPORT ID,instance from DP*/
		res = dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0);
		if (res) {
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "get subifid failed for %s, chk if vlan dev\n",
				 dev->name);
			/*Check bridge port exists otherwise register
			 *device with datapath  i.e. only in case of new
			 *VLAN interface
			 */
			/*Check if dev is a VLAN device */
			if (!is_vlan_dev(dev))
				return -EOPNOTSUPP;

			base = get_base_dev(dev, -1);
			DP_DEBUG(DP_DBG_FLAG_SWDEV, "base dev name:%s\n",
				 base ? base->name : "NULL");
			if (!base)
				base = dev;
			res = dp_get_netif_subifid(base, NULL, NULL, NULL,
						   &subif, 0);
			if (res) {
				pr_err("dp_get_netif_subifid fail:%s\n",
				       base->name);
				return -EOPNOTSUPP;
			}
			port = subif.port_id;
			inst = subif.inst;
			subif.subif = -1;
			if (dp_register_subif(
				get_dp_port_info(inst, port)->owner,
				dev, dev->name, &subif,
				DP_F_SUBIF_LOGICAL)) {
				pr_err("dp_register_subif fail: %s\n",
				       dev->name);
				return -EOPNOTSUPP;
			}
			flag = LOGIC_DEV_REGISTER;
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "registered subif,bp=%d port=%d\n",
				 subif.bport, subif.port_id);
		}
		br_item = kmalloc(sizeof(*br_item), GFP_KERNEL);
		if (!br_item)
			/*TODO need to check dequeue if no memory*/
			return -ENOMEM;
		br_item->inst = subif.inst;
		/* current bridge member port*/
		br_item->portid = subif.bport;
		br_item->dp_port = subif.port_id;

		/* Alloc Flag is needed by HAL layer to see the interface is
		 * LAN or GPON or EPON
		 */
		br_item->alloc_flag = subif.alloc_flag;

		swdev_lock();
		br_info = dp_swdev_bridge_entry_lookup(br_dev->name);
		p_info = get_dp_port_info(br_item->inst, br_item->dp_port);
		i_info = &dp_port_prop[br_item->inst].info;
		if (br_info) {
			strcpy(br_item->br_device_name,
			       br_info->br_device_name);
			br_item->fid = br_info->fid;
			br_item->flags = flag;
		} else {
			br_item->flags = ADD_BRENTRY | flag;
			if (p_info->swdev_en == 1) {
				br_id = i_info->swdev_alloc_bridge_id(
							br_item->inst);
				if (br_id) {
					/* Store bridge information
					 * to add in the table.
					 * This info is used during
					 * switchdev commit phase
					 */
					strcpy(br_item->br_device_name,
					       br_dev->name);
					br_item->fid = br_id;
				} else {
					pr_err("Switch config failed\r\n");
					kfree(br_item);
					swdev_unlock();
					return -EOPNOTSUPP;
				}
			} else {
				strcpy(br_item->br_device_name, br_dev->name);
				br_item->fid = br_id;
			}
		}
		switchdev_trans_item_enqueue(trans, br_item,
					     kfree, &br_item->tritem);
		swdev_unlock();
		return 0;
	}
	/*configure switch in commit phase and it cannot return failure*/
	swdev_lock();
	br_item = switchdev_trans_item_dequeue(trans);
	if (br_item) {
		dp_swdev_cfg_gswip(br_item, dev);
		if (br_item->flags & LOGIC_DEV_REGISTER)
			/*do only for vlan flag*/
			dp_swdev_cfg_vlan(br_item, dev);
	}
	swdev_unlock();
	return 0;
}

static int dp_swdev_del_if(struct net_device *dev,
			   const struct switchdev_attr *attr,
			   struct switchdev_trans *trans,
			   struct net_device *br_dev)
{
	struct br_info *br_info;
	struct bridge_id_entry_item *br_item;
	struct net_device *base;
	struct bridge_member_port *temp_list = NULL;
	dp_subif_t subif = {0};
	int port, inst;
	u8 *addr = (u8 *)dev->dev_addr;

	/* SWITCHDEV_TRANS_PREPARE phase */
	if (switchdev_trans_ph_prepare(trans)) {
		/*Get current BR_PORT ID from DP*/
		if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "dp_get_netif_subifid failed for %s\n",
				 dev->name);
		/*	if (!is_vlan_dev(dev))*/
				return -EINVAL;
		}
		br_item = kmalloc(sizeof(*br_item), GFP_KERNEL);
		if (!br_item)
			/*TODO need to check dequeue if no memory*/
			return -ENOMEM;
		swdev_lock();
		br_info = dp_swdev_bridge_entry_lookup(br_dev->name);
		if (br_info) {
			br_item->fid = br_info->fid;
			br_item->inst = subif.inst;
			/* current bridge member port*/
			br_item->portid = subif.bport;
			br_item->dp_port = subif.port_id;
			br_item->flags = BRIDGE_NO_ACTION;
			strcpy(br_item->br_device_name,
			       br_info->br_device_name);

			list_for_each_entry(temp_list, &br_info->bp_list,
					    list) {
				if (temp_list->portid == br_item->portid) {
					br_item->flags =
						temp_list->dev_reg_flag;
					break;
				}
			}
		} else {
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "No bridge entry or bport to delete\r\n");
			br_item->flags = BRIDGE_NO_ACTION;
		}
		/*TODO check return value & enqueue*/
		switchdev_trans_item_enqueue(trans, br_item, kfree,
					     &br_item->tritem);
		swdev_unlock();
		return 0;
	}
	/*Check bridge port exists otherwise register device with datapath
	 *i.e. only in case of new VLAN interface
	 */
	/*configure switch in commit phase and it cannot return failure*/
	swdev_lock();
	br_item = switchdev_trans_item_dequeue(trans);
	if (br_item) {
		dp_swdev_clr_gswip_cfg(br_item, addr);
		/* De-Register Logical Dev i.e. VLAN DEV
		 * if it is registered
		 */
		if (br_item->flags & LOGIC_DEV_REGISTER) {
			base = get_base_dev(dev, -1);
			DP_DEBUG(DP_DBG_FLAG_SWDEV,
				 "base dev name:%s\n",
				 base ? base->name : "NULL");
			if (!base)
				base = dev;
			/*the previous sequence was running into a deadlock in
			 * taking the swdev_lock
			 */
			dp_swdev_cfg_vlan(br_item, dev);
			swdev_unlock();
			if (dp_get_netif_subifid(base, NULL, NULL,
						 NULL, &subif, 0)) {
				pr_err("dp_get_netif_subifid fail:%s\n",
				       base->name);
				/*Cannot Return -EOPNOTSUPP
				 * in swdev commit stage
				 */
				return 0;
			}
			port = subif.port_id;
			inst = subif.inst;
			if (dp_register_subif(
					get_dp_port_info(inst, port)->owner,
					dev, dev->name, &subif,
					DP_F_DEREGISTER)) {
				pr_err("dp_register_subif fail: %s\n",
				       dev->name);
				/*Cannot Return -EOPNOTSUPP
				 * in swdev commit stage
				 */
				return 0;
			}
			swdev_lock();
		}
	}
	swdev_unlock();
	return 0;
}

int dp_del_br_if(struct net_device *dev, struct net_device *br_dev,
		 int inst, int bport)
{
	struct br_info *br_info;
	struct bridge_id_entry_item *br_item;
	struct bridge_member_port *temp_list = NULL;
	u8 *addr = (u8 *)dev->dev_addr;

	br_item = kmalloc(sizeof(*br_item), GFP_KERNEL);
	if (!br_item)
		return -1;
	swdev_lock();
	br_info = dp_swdev_bridge_entry_lookup(br_dev->name);
	if (br_info) {
		br_item->fid = br_info->fid;
		br_item->inst = inst;
		br_item->portid = bport;
		strcpy(br_item->br_device_name, br_info->br_device_name);

		list_for_each_entry(temp_list, &br_info->bp_list, list) {
			if (temp_list->portid == br_item->portid) {
				br_item->flags =
					temp_list->dev_reg_flag;
				break;
			}
		}
	} else {
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "No bridge entry or bport to delete\r\n");
		br_item->flags = BRIDGE_NO_ACTION;
	}

	if (br_item)
		dp_swdev_clr_gswip_cfg(br_item, addr);
	swdev_unlock();
	kfree(br_item);
	return 0;
}

static int dp_swdev_port_attr_set(struct net_device *dev,
				  const struct switchdev_attr *attr,
				  struct switchdev_trans *trans)
{
	int err = -EOPNOTSUPP;
	struct net_device *br_dev;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWDEV_TEST)
	{
		struct net_device *br_dev =
			netdev_master_upper_dev_get(attr->orig_dev);
		char buf[100];
		struct net_device *lower_dev;
		struct list_head *iter;

		buf[0] = 0;
		if (br_dev) {
			netdev_for_each_lower_dev(br_dev, lower_dev, iter) {
				sprintf(buf + strlen(buf), "%s ",
					lower_dev->name);
			}
		}
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "flag=%d attr=%d stat=%d dev=%s ori/up_dev=%s/%s:%s\n",
			 attr->flags, attr->id, attr->u.stp_state,
			 dev->name,
			 attr->orig_dev ? attr->orig_dev->name : "NULL"
			 br_dev ? br_dev->name : "Null", buf);
		return 0;
	}
#endif
	/* switchdev attr orig dev -> bridge port dev pointer
	 *then get the bridge dev from switchdev attr's orig dev
	 */
	br_dev = netdev_master_upper_dev_get(attr->orig_dev);
	if (!br_dev)
		return -EOPNOTSUPP;
	switch (attr->id) {
	case SWITCHDEV_ATTR_ID_PORT_STP_STATE:
		/*STP STATE forwading or ifconfig UP - add bridge*/
		if (attr->u.stp_state == BR_STATE_FORWARDING) {
			err = dp_swdev_add_if(attr->orig_dev,
					      attr, trans, br_dev);
		}
		/*STP STATE disabled or ifconfig DOWN - del bridge*/
		else if (attr->u.stp_state == BR_STATE_DISABLED) {
			err = dp_swdev_del_if(attr->orig_dev,
					      attr, trans, br_dev);
			if (err != 0)
				err = -EOPNOTSUPP;
		} else {
			return -EOPNOTSUPP;
		}
		break;
	case SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS:
		/* err = dp_swdev_port_attr_bridge_flags_set(dp_swdev_port,
		 *attr->u.brport_flags,
		 *trans);
		 */
		break;
	case SWITCHDEV_ATTR_ID_BRIDGE_AGEING_TIME:
		/*err = dp_swdev_port_attr_bridge_ageing_time_set(dp_swdev_port,
		 *attr->u.ageing_time,
		 *trans);
		 */
		break;
	case SWITCHDEV_ATTR_ID_BRIDGE_VLAN_FILTERING:
		/*err = dp_swdev_port_attr_bridge_br_vlan_set(dev,
		 *attr->orig_dev,trans);
		 */
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return err;
}

static int dp_swdev_port_attr_get(struct net_device *dev,
				  struct switchdev_attr *attr)
{
	struct net_device *br_dev;
	struct br_info *br_info;
	dp_subif_t subif = {0};
	/*For this api default err return value "-EOPNOTSUPP"
	 * cannot be set as this blocks bridgeport offload_fwd_mark
	 * setting at linux bridge level("nbp_switchdev_mark_set")
	 */
	br_dev = netdev_master_upper_dev_get(attr->orig_dev);
	if (!br_dev)
		return 0;

	if (dp_get_netif_subifid(dev, NULL, NULL, NULL, &subif, 0)) {
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "%s dp_get_netif_subifid failed for %s\n",
			 __func__, dev->name);
		return 0;
	}

	switch (attr->id) {
	case SWITCHDEV_ATTR_ID_PORT_PARENT_ID:
		br_info = dp_swdev_bridge_entry_lookup(br_dev->name);
		if (!br_info)
			return 0;
		if (br_info->fid < 0)
			return -EOPNOTSUPP;
		attr->u.ppid.id_len = sizeof(br_info->fid);
		memcpy(&attr->u.ppid.id, &br_info->fid, attr->u.ppid.id_len);
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "SWITCHDEV_ATTR_ID_PORT_PARENT_ID:%s fid=%d\n",
			 attr->orig_dev ? attr->orig_dev->name : "NULL",
			 br_info->fid);
		//err = 0;
		break;
	case SWITCHDEV_ATTR_ID_PORT_BRIDGE_FLAGS:
		break;
	default:
		return 0;
	}
	return 0;
}

static int dp_swdev_port_obj_add(struct net_device *dev,
				 const struct switchdev_obj *obj,
				 struct switchdev_trans *trans)
{
	int err = -EOPNOTSUPP;
	struct net_device *br_dev;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWDEV_TEST)
	{
		struct net_device *br_dev = netdev_master_upper_dev_get(dev);

		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "obj_add: obj-id=%d flag=%d dev=%s origdev=%s %s=%s\n",
			 obj->id, obj->flags, dev->name,
			 obj->orig_dev ? obj->orig_dev->name : "NULL", "up-dev",
			 br_dev ? br_dev->name : "Null");
		return 0;
	}
	return err; //TODO

	DP_DEBUG(DP_DBG_FLAG_SWDEV, "%s id:%d flags:%d dev name:%s\r\n",
		 __func__, obj->id,
		obj->flags, dev->name);
	if (netif_is_bridge_port(dev))
		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "dp_swdev_port_obj_add attr bridge port\r\n");
	if (trans->ph_prepare == 1)
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "%s ph->prepare:%d\r\n",
			 __func__, trans->ph_prepare);
#endif
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "%s id:%d flags:%d dev name:%s\r\n",
		 __func__, obj->id,
		 obj->flags, dev->name);
	br_dev = netdev_master_upper_dev_get(obj->orig_dev);
	if (!br_dev)
		return err;
	switch (obj->id) {
	case SWITCHDEV_OBJ_ID_PORT_VLAN:
		err = dp_swdev_filter_vlan(obj->orig_dev, obj, trans, br_dev);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return err;
}

static int dp_swdev_port_obj_del(struct net_device *dev,
				 const struct switchdev_obj *obj)
{
	int err = -EOPNOTSUPP;
	return err;//TODO
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWDEV_TEST)
	{
		struct net_device *br_dev = netdev_master_upper_dev_get(dev);

		DP_DEBUG(DP_DBG_FLAG_SWDEV,
			 "obj_del: obj-id=%d flag=%d dev=%s origdev=%s %s=%s\n",
			 obj->id, obj->flags, dev->name,
			 obj->orig_dev ? obj->orig_dev->name : "NULL", "up-dev",
			 br_dev ? br_dev->name : "Null");
		return 0;
	}
#endif

	DP_DEBUG(DP_DBG_FLAG_SWDEV, "dp_swdev_port_obj_del\r\n");
	switch (obj->id) {
	case SWITCHDEV_OBJ_ID_PORT_VLAN:
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return err;
}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWDEV_TEST)
static int dp_swdev_port_fdb_dump(struct net_device *dev,
				  struct switchdev_obj_port_fdb *fdb_obj,
				  switchdev_obj_dump_cb_t *cb)
{
#if 0
	int err = 0;
	struct fdb_tbl *fdb_d = NULL;

	list_for_each_entry(fdb_d, &fdb_tbl_list_31, fdb_list) {
		if (fdb_d) {
			if (fdb_d->port_dev != dev) {
				continue;
			} else {
				ether_addr_copy(fdb_obj->addr, fdb_d->addr);
				fdb_obj->ndm_state = NUD_PERMANENT;
				//fdb->vid = 10;
				err = cb(&fdb_obj->obj);
				if (err)
					return 0;
			}
		} else {
			break;
		}
	}
#endif
	return 0;
}

static int dp_swdev_port_obj_dump(struct net_device *dev,
				  struct switchdev_obj *obj,
				  switchdev_obj_dump_cb_t *cb)
{
	int err = -EOPNOTSUPP;

	DP_DEBUG(DP_DBG_FLAG_SWDEV, "dp_swdev_port_obj_dump\r\n");
	switch (obj->id) {
	case SWITCHDEV_OBJ_ID_PORT_VLAN:
		err = dp_swdev_port_vlan_dump(mlxsw_sp_port,
					      SWITCHDEV_OBJ_PORT_VLAN(obj),
					      cb);
		break;
	case SWITCHDEV_OBJ_ID_PORT_FDB:
		err = dp_swdev_port_fdb_dump(dev,
					     SWITCHDEV_OBJ_PORT_FDB(obj),
					     cb);
		break;
	default:
		err = -EOPNOTSUPP;
		break;
	}
	return err;
}

static int dp_ndo_bridge_setlink(struct net_device *dev,
				 struct nlmsghdr *nlh,
				 u16 flags)
{
	int status = 0;

	DP_DEBUG(DP_DBG_FLAG_SWDEV, "ndo_bridge_setlink: dev=%s\n",
		 dev ? dev->name : "NULL");
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWDEV_TEST)
	struct nlattr *attr, *br_spec;
	int rem;
	u16 mode = 0;

	br_spec = nlmsg_find_attr(nlh, sizeof(struct ifinfomsg), IFLA_AF_SPEC);
	if (!br_spec)
		return -EINVAL;
	nla_for_each_nested(attr, br_spec, rem) {
		DP_DEBUG(DP_DBG_FLAG_SWDEV, "nla_type(attr)=%d\n",
			 nla_type(attr));
		if (nla_type(attr) != IFLA_BRIDGE_MODE)
			continue;
		if (nla_len(attr) < sizeof(mode))
			return -EINVAL;
		mode = nla_get_u16(attr); /*like BRIDGE_MODE_VEPA */
	}
#endif
	//return switchdev_port_bridge_setlink(dev, nlh, flags);
	return status;
}

static inline int dp_ndo_bridge_getlink(struct sk_buff *skb, u32 pid,
					u32 seq, struct net_device *dev,
					u32 filter_mask, int nlflags)
{
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "port_bridge_getlink: dev=%s\n",
		 dev ? dev->name : "NULL");
	if (!dev)
		return -EINVAL;

	return switchdev_port_bridge_getlink(skb, pid, seq, dev,
					    filter_mask, nlflags);
}

int dp_ndo_bridge_dellink(struct net_device *dev, struct nlmsghdr *nlh,
			  u16 flags)
{
	DP_DEBUG(DP_DBG_FLAG_SWDEV, "port_bridge_dellink: dev=%s\n",
		 dev ? dev->name : "NULL");
	if (!dev)
		return -EINVAL;
	return switchdev_port_bridge_dellink(dev, nlh, flags);
}
#endif

int dp_notif_br_alloc(struct net_device *br_dev)
{
	int br_id = 0;
	struct br_info *br_info;

	br_id = dp_port_prop[0].info.swdev_alloc_bridge_id(0);
	if (br_id) {
		if ((dp_port_prop[0].info.swdev_bridge_cfg_set(0, br_id)
			== 0)) {
			br_info = kmalloc(sizeof(*br_info), GFP_KERNEL);
			if (!br_info) {
				pr_err("Switch cfg Fail as kmalloc %zd bytes fail\n",
				       sizeof(*br_info));
				return -1;
			}
			br_info->fid = br_id;
			br_info->inst = 0;
			br_info->cpu_port = ENABLE;
			br_info->flag = 0;
			strcpy(br_info->br_device_name, br_dev->name);
			INIT_LIST_HEAD(&br_info->bp_list);
			dp_swdev_insert_bridge_id_entry(br_info);
		} else {
			pr_err("Switch configuration failed\r\n");
			return -1;
		}
	} else {
		pr_err("Switch bridge alloc failed\r\n");
		return -1;
	}
	return br_id;
}

#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWDEV_TEST)
/*Register netdev_ops for switchdev*/
static int dp_set_netdev_ops(struct dp_dev *dp_dev,
			     struct net_device *dp_port)
{
	int err = DP_SUCCESS;

	if (!dp_dev)
		return -1;
	err = dp_ops_set((void **)&dp_port->netdev_ops,
			 offsetof(const struct net_device_ops,
				  ndo_bridge_setlink),
			 sizeof(*dp_port->netdev_ops),
			 (void **)&dp_dev->old_dev_ops,
			 &dp_dev->new_dev_ops,
			 &dp_ndo_bridge_setlink);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->netdev_ops,
			 offsetof(const struct net_device_ops,
				  ndo_bridge_getlink),
			 sizeof(*dp_port->netdev_ops),
			 (void **)&dp_dev->old_dev_ops,
			 &dp_dev->new_dev_ops,
			 &dp_ndo_bridge_getlink);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->netdev_ops,
			 offsetof(const struct net_device_ops,
				  ndo_bridge_dellink),
			 sizeof(*dp_port->netdev_ops),
			 (void **)&dp_dev->old_dev_ops,
			 &dp_dev->new_dev_ops,
			 &dp_ndo_bridge_dellink);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->netdev_ops,
			 offsetof(const struct net_device_ops,
				  ndo_fdb_add),
			 sizeof(*dp_port->netdev_ops),
			 (void **)&dp_dev->old_dev_ops,
			 &dp_dev->new_dev_ops,
			 &switchdev_port_fdb_add);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->netdev_ops,
			 offsetof(const struct net_device_ops,
				  ndo_fdb_del),
			 sizeof(*dp_port->netdev_ops),
			 (void **)&dp_dev->old_dev_ops,
			 &dp_dev->new_dev_ops,
			 &switchdev_port_fdb_del);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->netdev_ops,
			 offsetof(const struct net_device_ops,
				  ndo_fdb_dump),
			 sizeof(*dp_port->netdev_ops),
			 (void **)&dp_dev->old_dev_ops,
			 &dp_dev->new_dev_ops,
			 &switchdev_port_fdb_dump);
	if (err)
		return DP_FAILURE;
	return 0;
}
#endif

/* This function registers the created port in datapath to switchdev */
int dp_port_register_switchdev(struct dp_dev  *dp_dev,
			       struct net_device *dp_port)
{
	int err = DP_SUCCESS;

	if (!dp_port) {
		pr_err("cannot support switchdev if dev is NULL\n");
		return -1;
	}
	if (dp_port_prop[dp_dev->inst].info.swdev_flag == 1) {
		if (!dp_port->netdev_ops) {
			pr_err("netdev_ops not defined\n");
			return -1;
		}
	/* switchdev ops register */
	err = dp_ops_set((void **)&dp_port->switchdev_ops,
			 offsetof(const struct switchdev_ops,
				  switchdev_port_attr_get),
			 sizeof(*dp_port->switchdev_ops),
			 (void **)&dp_dev->old_swdev_ops,
			 &dp_dev->new_swdev_ops,
			 &dp_swdev_port_attr_get);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->switchdev_ops,
			 offsetof(const struct switchdev_ops,
				  switchdev_port_attr_set),
			 sizeof(*dp_port->switchdev_ops),
			 (void **)&dp_dev->old_swdev_ops,
			 &dp_dev->new_swdev_ops,
			 &dp_swdev_port_attr_set);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->switchdev_ops,
			 offsetof(const struct switchdev_ops,
				  switchdev_port_obj_add),
			 sizeof(*dp_port->switchdev_ops),
			 (void **)&dp_dev->old_swdev_ops,
			 &dp_dev->new_swdev_ops,
			 &dp_swdev_port_obj_add);
	if (err)
		return DP_FAILURE;
	err = dp_ops_set((void **)&dp_port->switchdev_ops,
			 offsetof(const struct switchdev_ops,
				  switchdev_port_obj_del),
			 sizeof(*dp_port->switchdev_ops),
			 (void **)&dp_dev->old_swdev_ops,
			 &dp_dev->new_swdev_ops,
			 &dp_swdev_port_obj_del);
	if (err)
		return DP_FAILURE;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWDEV_TEST)
	err = dp_ops_set((void **)&dp_port->switchdev_ops,
			 offsetof(const struct switchdev_ops,
				  switchdev_port_obj_dump),
			 sizeof(*dp_port->switchdev_ops),
			 (void **)&dp_dev->old_swdev_ops,
			 &dp_dev->new_swdev_ops,
			 &dp_swdev_port_obj_dump);
	if (err)
		return DP_FAILURE;
	dp_set_netdev_ops(dp_dev, dp_port);
#endif
	}
	return 0;
}

void dp_switchdev_exit(void)
{
	int i;

	for (i = 0; i < DP_MAX_INST; i++)
		dp_swdev_bridge_id_entry_free(i);
}

int dp_switchdev_init(void)
{
	dp_swdev_bridge_id_entry_init();
	return 0;
}
