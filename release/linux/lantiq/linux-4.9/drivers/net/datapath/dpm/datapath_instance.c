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
#include "datapath.h"
#include "datapath_instance.h"
#include "datapath_swdev_api.h"
#include "datapath_ioctl.h"
int dp_cap_num;
struct dp_hw_cap hw_cap_list[DP_MAX_HW_CAP];

/*Subif DEV hash list
 *Note: in current implementation, its memory will not be really removed,
 *Instead, it is just move to a free hash list dp_dev_list_free
 *Reason: if we really free the memory, net_device's opt callback sometimes
 *will be set to NULL?? Need further check
 */
struct hlist_head dp_dev_list[DP_DEV_HASH_SIZE];
struct hlist_head dp_dev_list_free[DP_DEV_HASH_SIZE];

/*Module hash list */
struct hlist_head dp_mod_list[DP_MOD_HASH_SIZE];

struct hlist_head *get_dp_dev_list(void)
{
	return dp_dev_list;
}
EXPORT_SYMBOL(get_dp_dev_list);

int register_dp_hw_cap(struct dp_hw_cap *info, u32 flag)
{
	int i;

	if (!info) {
		pr_err("register_dp_hw_cap: NULL info\n");
		return -1;
	}
	for (i = 0; i < DP_MAX_HW_CAP; i++) {
		if (hw_cap_list[i].valid)
			continue;
		hw_cap_list[i].valid = 1;
		hw_cap_list[i].info = info->info;
		dp_cap_num++;
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_EXTRA_DEBUG)
		pr_err("Succeed to %s HAL[%d]: type=%d ver=%d dp_cap_num=%d\n",
		       "Register",
		       i,
		       info->info.type,
		       info->info.ver,
		       dp_cap_num);
#endif
		return 0;
	}
	pr_err("Failed to %s HAL: type=%d ver=%d\n",
	       "Register",
	       info->info.type,
	       info->info.ver);
	return -1;
}

/*return value:
 *succeed: return 0 with info->inst updated
 *fail: -1
 */
int dp_request_inst(struct dp_inst_info *info, u32 flag)
{
	int i, k, j;

	if (!info)
		return -1;

	if (flag & DP_F_DEREGISTER) {
		/*do de-register */

		return 0;
	}
	/*register a dp instance */

	/*to check whether any such matched HW cap */
	for (k = 0; k < DP_MAX_HW_CAP; k++) {
		if (!hw_cap_list[k].valid)
			continue;
		if ((hw_cap_list[k].info.type == info->type) &&
		    (hw_cap_list[k].info.ver == info->ver)) {
			break;
		}
	}
	if (k == DP_MAX_HW_CAP) {
		pr_err("dp_request_inst fail to math cap type=%d/ver=%d\n",
		       info->type, info->ver);
		return -1;
	}

	/* to find a free instance */
	for (i = 0; i < DP_MAX_INST; i++) {
		if (!dp_port_prop[i].valid)
			break;
	}
	if (i == DP_MAX_INST) {
		pr_err("dp_request_inst fail for dp inst full arealdy\n");
		return -1;
	}
	if (alloc_dma_chan_tbl(i)) {
		pr_err("FAIL to alloc dma chan tbl\n");
		return -1;
	}
	dp_port_prop[i].ops[0] = info->ops[0];
	dp_port_prop[i].ops[1] = info->ops[1];

	for (j = 0; j < DP_MAX_MAC_HANDLE; j++) {
		if (info->mac_ops[j])
			dp_port_prop[i].mac_ops[j] = info->mac_ops[j];
	}

	dp_port_prop[i].info = hw_cap_list[k].info;
	dp_port_prop[i].cbm_inst = info->cbm_inst;
	dp_port_prop[i].qos_inst = info->qos_inst;
	dp_port_prop[i].valid = 1;
#ifdef CONFIG_LTQ_DATAPATH_CPUFREQ
	dp_cpufreq_notify_init(i);
	DP_DEBUG(DP_DBG_FLAG_COC, "DP registered CPUFREQ notifier\n");
#endif
	if (alloc_dp_port_subif_info(i)) {
		pr_err("alloc_dp_port_subif_info fail..\n");
		return DP_FAILURE;
	}
	if (dp_port_prop[i].info.dp_platform_set(i, DP_PLATFORM_INIT) < 0) {
		dp_port_prop[i].valid = 0;
		pr_err("dp_platform_init failed for inst=%d\n", i);
		return -1;
	}
	info->inst = i;
	dp_inst_num++;
	DP_DEBUG(DP_DBG_FLAG_INST,
		 "dp_request_inst ok: inst=%d, dp_inst_num=%d\n",
		 i, dp_inst_num);
	return 0;
}
EXPORT_SYMBOL(dp_request_inst);

struct dp_hw_cap *match_hw_cap(struct dp_inst_info *info, u32 flag)
{
	int k;

	for (k = 0; k < DP_MAX_HW_CAP; k++) {
		if (!hw_cap_list[k].valid)
			continue;
		if ((hw_cap_list[k].info.type == info->type) &&
		    (hw_cap_list[k].info.ver == info->ver)) {
			return &hw_cap_list[k];
		}
	}
	return NULL;
}

/*Note: like pon one device can have multiple ctp,
 *ie, it may register multiple times
 */
u32 dp_dev_hash(struct net_device *dev, char *subif_name)
{
	unsigned long index;

	if (!dev)
		index = (unsigned long)subif_name;
	else
		index = (unsigned long)dev;
	/*Note: it is 4K alignment. Need tune later */
	return (u32)((index >> DP_DEV_HASH_SHIFT) % DP_DEV_HASH_SIZE);
}

struct dp_dev *dp_dev_lookup(struct hlist_head *head,
			     struct net_device *dev, char *subif_name, u32 flag)
{
	struct dp_dev *item;

	if (!dev) {
		hlist_for_each_entry(item, head, hlist) {
			if (strcmp(item->subif_name, subif_name) == 0)
				return item;
		}
	} else {
		hlist_for_each_entry(item, head, hlist) {
			if (item->dev == dev)
				return item;
		}
	}
	return NULL;
}

#if IS_ENABLED(CONFIG_PPA)
#if LINUX_VERSION_CODE < KERNEL_VERSION(4, 13, 0)
static int dp_ndo_setup_tc(struct net_device *dev, u32 handle,
			   __be16 protocol, struct tc_to_netdev *tc)
{
#if IS_ENABLED(CONFIG_PPA)
	if (qos_mgr_hook_setup_tc)
		return qos_mgr_hook_setup_tc(dev, handle, protocol, tc);
#endif
	if (dev->netdev_ops->ndo_setup_tc)
		return dev->netdev_ops->ndo_setup_tc(dev, handle, protocol, tc);
	pr_err("Cannot support ndo_setup_tc\n");
	return -1;
}
#else
static int dp_ndo_setup_tc(struct net_device *dev,
			   enum tc_setup_type type,
			   void *type_data)
{
	return -1;
}
#endif /* LINUX_VERSION_CODE */
#endif /* CONFIG_PPA */

/*Note:
 *dev and subif_name: only one will be used for the hash index calculation.
 *subif_name is only used for ATM IPOA/PPPOA case since its dev is NULL.
 *otherwise it should use its dev.
 *it will not work if just dev->name as subif_name
 */
int dp_inst_add_dev(struct net_device *dev, char *subif_name, int inst,
		    int ep, int bp, int ctp, u32 flag)
{
	struct dp_dev *dp_dev;
	u8 new_f = 0;
	u32 idx;
	struct subif_basic *subif;
#if IS_ENABLED(CONFIG_PPA)
	int err = DP_SUCCESS;
#endif
	if (!dev && !subif_name) {
		pr_err("Why dev/subif_name both NULL?\n");
		return -1;
	}
	idx = dp_dev_hash(dev, subif_name);
	subif = kmalloc(sizeof(*subif), GFP_KERNEL);
	if (!subif) {
		pr_err("failed to alloc %zd bytes\n", sizeof(*subif));
		return -1;
	}

	subif->subif = ctp;
	dp_dev = dp_dev_lookup(&dp_dev_list[idx], dev, subif_name, flag);

	if (!dp_dev) { /*search free list */
		dp_dev = dp_dev_lookup(&dp_dev_list_free[idx],
				       dev, subif_name, flag);
		if (dp_dev) {
			hlist_del(&dp_dev->hlist); /*remove from free list */
			dp_dev->count = 0;
			new_f = 1;
		}
	}
	if (!dp_dev) { /*alloc new */
		dp_dev = kzalloc(sizeof(*dp_dev), GFP_KERNEL);
		if (dp_dev) {
			dp_dev->count = 0;
			dp_dev->subif_name[0] = 0;
			dp_dev->fid = 0;
			INIT_LIST_HEAD(&dp_dev->ctp_list);
			new_f = 1;
		}
	}
	if (!dp_dev) {
		pr_err("Failed to kmalloc %zd bytes\n", sizeof(*dp_dev));
		kfree(subif);
		return -1;
	}
	if (new_f) {
		dp_dev->inst = inst;
		dp_dev->dev = dev;
		dp_dev->ep = ep;
		dp_dev->bp = bp;
		dp_dev->ctp = ctp;
		if (subif_name) {
			strncpy(dp_dev->subif_name, subif_name,
				sizeof(dp_dev->subif_name) - 1);
		}
		hlist_add_head(&dp_dev->hlist, &dp_dev_list[idx]);
#if IS_ENABLED(CONFIG_PPA)
#if (!IS_ENABLED(CONFIG_SOC_GRX500))
		/*backup ops*/
		if (dev) {
			dev->features |= NETIF_F_HW_TC;
			err = dp_ops_set((void **)&dev->netdev_ops,
					 offsetof(const struct net_device_ops,
						  ndo_setup_tc),
					 sizeof(*dev->netdev_ops),
					 (void **)&dp_dev->old_dev_ops,
					 &dp_dev->new_dev_ops,
					 &dp_ndo_setup_tc);
			if (err)
				return DP_FAILURE;
		}
#endif /* CONFIG_SOC_GRX500 */
#endif /* CONFIG_PPA */
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_SWITCHDEV)
	if (!(flag & DP_F_SUBIF_LOGICAL))
		dp_port_register_switchdev(dp_dev, dev);
#endif
#if IS_ENABLED(CONFIG_INTEL_DATAPATH_PTP1588)
	dp_register_ptp_ioctl(dp_dev, dev, inst);
#endif
	}
	dp_dev->count++;
	list_add(&subif->list, &dp_dev->ctp_list);
	DP_DEBUG(DP_DBG_FLAG_DBG, "dp_inst_add_dev:add new ctp=%d\n", ctp);
	return 0;
}

int dp_inst_del_dev(struct net_device *dev, char *subif_name, int inst, int ep,
		    u16 ctp, u32 flag)
{
	struct dp_dev *dp_dev;
	u32 idx;
	struct subif_basic *tmp;

	idx = dp_dev_hash(dev, subif_name);
	dp_dev = dp_dev_lookup(&dp_dev_list[idx], dev, subif_name, flag);
	if (!dp_dev) {
		pr_err("Failed to dp_dev_lookup: %s_%s\n",
		       dev ? dev->name : "NULL", subif_name);
		return -1;
	}
	if (dp_dev->count <= 0) {
		pr_err("Count(%d) should > 0(%s_%s)\n", dp_dev->count,
		       dev ? dev->name : "NULL", subif_name);
		return -1;
	}
	if (dp_dev->inst != inst) {
		pr_err("Why inst not same:%d_%d(%s_%s)\n", dp_dev->inst, inst,
		       dev ? dev->name : "NULL", subif_name);
		return -1;
	}
	if (dp_dev->ep != ep) {
		pr_err("Why ep not same:%d_%d(%s_%s)\n", dp_dev->ep, ep,
		       dev ? dev->name : "NULL", subif_name);
		return -1;
	}
	list_for_each_entry(tmp, &dp_dev->ctp_list, list) {
		/* Check added to remove pmapper device from dp_dev list
		 * if pmapper device is unregistered with last gem
		 */
		if (dp_dev->dev == dev) {
			list_del(&tmp->list);
			dp_dev->count--;

			if (!dp_dev->count) { /*move to free list */
				hlist_del(&dp_dev->hlist);
				if (dev)
					dp_ops_reset(dp_dev, dev);
				/*do't really free now
				 *in case network stack is holding the callback
				 */
				hlist_add_head(&dp_dev->hlist,
					       &dp_dev_list_free[idx]);
			}
			return 0;
		}
	}
	DP_DEBUG(DP_DBG_FLAG_INST, "dp_del_dev fail(%s_%s): not found ctp=%d\n",
		 dev ? dev->name : "NULL",
		 subif_name, ctp);
	return -1;
}

u32 dp_mod_hash(struct module *owner, u16 ep)
{
	unsigned long index;

	index = (unsigned long)owner;
	return (u32)(((index >> DP_MOD_HASH_SHIFT) % DP_MOD_HASH_SIZE) | ep);
}

struct dp_mod *dp_mod_lookup(struct hlist_head *head, struct module *owner,
			     u16 ep, u32 flag)
{
	struct dp_mod *item;

	hlist_for_each_entry(item, head, hlist) {
		if ((item->mod == owner) &&
		    (item->ep == ep))
		return item;
	}
	return NULL;
}

/* tuple: owner + ep
 * act: inst
 */
int dp_inst_insert_mod(struct module *owner, u16 ep, u32 inst, u32 flag)
{
	struct dp_mod *dp_mod;
	u8 new_f = 0;
	u32 idx;

	if (!owner) {
		pr_err("owner NULL?\n");
		return -1;
	}
	idx = dp_mod_hash(owner, ep);
	DP_DEBUG(DP_DBG_FLAG_INST, "dp_inst_insert_mod:idx=%u\n", idx);
	dp_mod = dp_mod_lookup(&dp_mod_list[idx], owner, ep, flag);
	if (!dp_mod) { /*alloc new */
		dp_mod = kmalloc(sizeof(*dp_mod), GFP_KERNEL);
		if (dp_mod) {
			dp_mod->mod = owner;
			dp_mod->ep = ep;
			dp_mod->inst = inst;
			new_f = 1;
		}
	}
	if (!dp_mod) {
		pr_err("Failed to kmalloc %zd bytes\n", sizeof(*dp_mod));
		return -1;
	}
	if (new_f)
		hlist_add_head(&dp_mod->hlist, &dp_mod_list[idx]);
	DP_DEBUG(DP_DBG_FLAG_INST, "dp_inst_insert_mod:%s\n", owner->name);
	return 0;
}

int dp_inst_del_mod(struct module *owner, u16 ep, u32 flag)
{
	struct dp_mod *dp_mod;
	u32 idx;

	if (!owner) {
		pr_err("owner NULL?\n");
		return -1;
	}
	idx = dp_mod_hash(owner, ep);
	dp_mod = dp_mod_lookup(&dp_mod_list[idx], owner, ep, flag);
	if (!dp_mod) {
		pr_err("Failed to dp_mod_lookup: %s\n",
		       owner->name);
		return -1;
	}
	hlist_del(&dp_mod->hlist);
	kfree(dp_mod);

	DP_DEBUG(DP_DBG_FLAG_INST, "dp_inst_del_mod ok: %s:\n", owner->name);
	return 0;
}

int dp_get_inst_via_module(struct module *owner,  u16 ep, u32 flag)
{
	struct dp_mod *dp_mod;
	u32 idx;

	if (!owner) {
		pr_err("owner NULL?\n");
		return -1;
	}
	idx = dp_mod_hash(owner, ep);
	dp_mod = dp_mod_lookup(&dp_mod_list[idx], owner, ep, flag);
	if (!dp_mod) {
		pr_err("Failed to dp_mod_lookup: %s\n",
		       owner->name);
		return -1;
	}

	return dp_mod->inst;
}

/* if dev NULL, use subif_name, otherwise use dev to search */
int dp_get_inst_via_dev(struct net_device *dev, char *subif_name, u32 flag)
{
	struct dp_dev *dp_dev;

	if (!dev && !subif_name) /*for ATM IPOA/PPPOA */
		return 0; /*workaround:otherwise caller need to check value */

	dp_dev = dp_dev_lookup(dp_dev_list, dev, subif_name, flag);
	if (!dp_dev)
		return 0; /*workaround:otherwise caller need to check value */

	return dp_dev->inst;
}

static u32 dev_hash_index;
static struct dp_dev *dp_dev_proc;
int proc_inst_dev_dump(struct seq_file *s, int pos)
{
	struct subif_basic *tmp;

	if (!capable(CAP_SYS_PACCT))
		return -1;
	while (!dp_dev_proc) {
		dev_hash_index++;
		pos = 0;
		if (dev_hash_index == DP_DEV_HASH_SIZE)
			return -1;

		dp_dev_proc = hlist_entry_safe((
			&dp_dev_list[dev_hash_index])->first,
			struct dp_dev, hlist);
	}
	seq_printf(s, "Hash=%u pos=%d dev=%s(@%px) inst=%d ep=%d bp=%d ctp=%d count=%d @%px\n",
		   dev_hash_index,
		   pos,
		   dp_dev_proc->dev ? dp_dev_proc->dev->name :
		   dp_dev_proc->subif_name,
		   dp_dev_proc->dev,
		   dp_dev_proc->inst,
		   dp_dev_proc->ep,
		   dp_dev_proc->bp,
		   dp_dev_proc->ctp,
		   dp_dev_proc->count,
		   dp_dev_proc);
	seq_puts(s, "  ctp=");
	list_for_each_entry(tmp, &dp_dev_proc->ctp_list, list) {
		seq_printf(s, "%u ", tmp->subif);
	}
	seq_puts(s, "\n");
	dp_dev_proc = hlist_entry_safe((dp_dev_proc)->hlist.next,
				       struct dp_dev, hlist);

	pos++;
	return pos;
}

int proc_inst_dev_start(void)
{
	dev_hash_index = 0;
	dp_dev_proc = hlist_entry_safe(
		(&dp_dev_list[dev_hash_index])->first,
		struct dp_dev, hlist);
	return 0;
}

static void dump_cap(struct seq_file *s, struct dp_cap *cap)
{
	if (!s)
		return;
	seq_printf(s, "	HW TX checksum offloading: %s\n",
		   cap->tx_hw_chksum ? "Yes" : "No");
	seq_printf(s, "	HW RX checksum verification: %s\n",
		   cap->rx_hw_chksum ? "Yes" : "No");
	seq_printf(s, "	HW TSO: %s\n",
		   cap->hw_tso ? "Yes" : "No");
	seq_printf(s, "	HW GSO: %s\n",
		   cap->hw_gso ? "Yes" : "No");
	seq_printf(s, "	QOS Engine: %s\n",
		   cap->qos_eng_name);
	seq_printf(s, "	Pkt Engine: %s\n",
		   cap->pkt_eng_name);
	seq_printf(s, "	max_num_queues: %d\n",
		   cap->max_num_queues);
	seq_printf(s, "	max_num_scheds: %d\n",
		   cap->max_num_scheds);
	seq_printf(s, "	max_num_deq_ports: %d\n",
		   cap->max_num_deq_ports);
	seq_printf(s, "	max_num_qos_ports: %d\n",
		   cap->max_num_qos_ports);
	seq_printf(s, "	max_num_dp_ports: %d\n",
		   cap->max_num_dp_ports);
	seq_printf(s, "	max_num_subif_per_port: %d\n",
		   cap->max_num_subif_per_port);
	seq_printf(s, "	max_num_subif: %d\n",
		   cap->max_num_subif);
	seq_printf(s, "	max_num_bridge_port: %d\n",
		   cap->max_num_bridge_port);
}

static u32 mod_hash_index;
static struct dp_mod *dp_mod_proc;
int proc_inst_mod_dump(struct seq_file *s, int pos)
{
	if (!capable(CAP_SYS_PACCT))
		return -1;
	while (!dp_mod_proc) {
		mod_hash_index++;
		pos = 0;
		if (mod_hash_index == DP_MOD_HASH_SIZE)
			return -1;

		dp_mod_proc = hlist_entry_safe((
			&dp_mod_list[mod_hash_index])->first,
			struct dp_mod, hlist);
	}
	seq_printf(s, "Hash=%u pos=%d owner=%s(@%px) ep=%d inst=%d\n",
		   mod_hash_index,
		   pos,
		   dp_mod_proc->mod->name,
		   dp_mod_proc->mod,
		   dp_mod_proc->ep,
		   dp_mod_proc->inst);

	dp_mod_proc = hlist_entry_safe((dp_mod_proc)->hlist.next,
				       struct dp_mod, hlist);
	pos++;
	return pos;
}

int proc_inst_dump(struct seq_file *s, int pos)
{
	struct dp_cap cap;

	if (!capable(CAP_SYS_PACCT))
		return -1;
	if (!dp_port_prop[pos].valid)
		goto NEXT;
	seq_printf(s, "Inst[%d] Type=%u ver=%d\n",
		   pos,
		   dp_port_prop[pos].info.type,
		   dp_port_prop[pos].info.ver);
	/*dump_cap(s, &dp_port_prop[pos].info.cap);*/
	cap.inst = pos;
	dp_get_cap(&cap, 0);
	dump_cap(s, &cap);

NEXT:
	pos++;
	if (pos == DP_MAX_INST)
		return -1;
	return pos;
}

int proc_inst_hal_dump(struct seq_file *s, int pos)
{
	if (!capable(CAP_SYS_PACCT))
		return -1;
	if (!hw_cap_list[pos].valid)
		goto NEXT;

	seq_printf(s, "HAL[%d] Type=%u ver=%d dp_cap_num=%d\n",
		   pos,
		   hw_cap_list[pos].info.type,
		   hw_cap_list[pos].info.ver,
		   dp_cap_num);
	dump_cap(s, &hw_cap_list[pos].info.cap);

NEXT:
	pos++;
	if (pos == DP_MAX_HW_CAP)
		return -1;
	return pos;
}

int proc_inst_mod_start(void)
{
	mod_hash_index = 0;
	dp_mod_proc = hlist_entry_safe(
		(&dp_mod_list[mod_hash_index])->first,
		struct dp_mod, hlist);
	return 0;
}

int dp_inst_init(u32 flag)
{
	int i;

	dp_cap_num = 0;
	memset(hw_cap_list, 0, sizeof(hw_cap_list));
	for (i = 0; i < DP_DEV_HASH_SIZE; i++) {
		INIT_HLIST_HEAD(&dp_dev_list[i]);
		INIT_HLIST_HEAD(&dp_dev_list_free[i]);
	}

	return 0;
}

