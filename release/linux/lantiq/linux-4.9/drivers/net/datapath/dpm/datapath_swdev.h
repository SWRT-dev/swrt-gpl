// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef DATAPATH_SWDEV_H
#define DATAPATH_SWDEV_H

#include <linux/if.h>
#include <linux/etherdevice.h>
#include <net/switchdev.h>
#include <net/switch_api/lantiq_gsw_api.h> /*Switch related structures */
#include <net/switch_api/lantiq_gsw.h>

#define BRIDGE_ID_ENTRY_HASH_LENGTH 6
#define BR_ID_ENTRY_HASH_TABLE_SIZE BIT(BRIDGE_ID_ENTRY_HASH_LENGTH)

#define ADD_BRENTRY		BIT(0)
#define DEL_BRENTRY		BIT(1)
#define BRIDGE_NO_ACTION	BIT(2)
#define LOGIC_DEV_REGISTER	BIT(3)
#define CPU_PORT_DISABLE	BIT(4)

#define DP_SWDEV_BRIDGE_ITEM 1
#define DP_DEF_MAX_VLAN_LIMIT 8

struct bridge_id_entry_item {
	struct net_device *dev;
	u16 fid;
	u32 flags;
	u32 bportid; /*Bridge port*/
	u32 dp_port; /*DP port*/
	u32 inst;
	u32 priv_flag; /* to store bp dev priv flags (like IFF_NO_QUEUE) */
	struct switchdev_trans_item tritem;
};

struct br_info {
	struct hlist_node br_hlist;
	struct net_device *dev; /* bridge dev */
	u16 fid;
	u32 flag;
	u32 inst;
	u32 dp_port;
	struct list_head bp_list;
	bool br_vlan_en; 		/* VLAN Enabled for this bridge */
	u32 max_vlan_limit;		/* Max Vlan allowed for this bridge */
	u32 max_brid_limit;		/* Max Brid allowed for this bridge */
	u32 num_vlan;			/* Num of vlans supported in br */
	u32 num_fid;			/* Num of vlan aware fid in br */
	struct list_head br_vlan_list;  /* List pointing to the vlan entry */
	struct rcu_head rcu_head;
};

struct bridge_member_port {
	struct list_head list;
	u32 bportid; /* bridge port */
	struct net_device *dev; /* bridge port dev for debug */
	u32 dev_reg_flag;
	u16 bport[8];
	u32 dev_priv_flag; /* to store bp dev priv flags (like IFF_NO_QUEUE) */
	u32 dp_port;
	bool isolate;
	bool hairpin;
	struct list_head bport_vlan_list; /* List pointing to the vlan entry */
};

struct vlist_entry {
	struct list_head list;
	struct vlan_entry *vlan_entry;  /* Pointer to the VLAN Aware Entry */
};

struct vlan_entry {
	u16 fid;	/* New FiD used for this bridge port */
	u16 vlan_id;	/* Vlan ID supported in this bridge port */
	u32 ref_cnt;	/* Reference count for this VLAN */
	struct list_head pce_list; /* List pointing to PCE */
};

struct pce_entry {
	struct list_head list;
	struct dp_pce_blk_info *blk_info;
	u16 idx; /* pce index */
	bool disable; /* PCE rule index enabled or disabled */
};

struct fdb_tbl {
	struct list_head fdb_list;
	struct net_device *port_dev;
	u8 addr[ETH_ALEN];
	__be16 vid;
};

extern struct list_head fdb_tbl_list;
int dp_swdev_bridge_id_entry_init(void);
struct br_info *dp_swdev_bridge_entry_lookup_rcu(struct net_device *dev);
int dp_swdev_chk_bport_in_br(struct net_device *bp_dev, int bport,
			     int inst);
int dp_swdev_get_dp_port_from_bp(struct br_info *br_item, int bport,
				 u32 *priv_flag);
int dp_register_bport_vlan(struct br_info *br_info,
			   struct net_device *dev, int bport, int flags);
int dp_register_br_vlan(struct br_info *br_info, int flags);
struct pce_entry *get_pce_entry_from_ventry(struct vlan_entry *ventry,
		int portid, int subifid);
void dp_switchdev_exit(void);

#endif /*DATAPATH_SWDEV_H*/
