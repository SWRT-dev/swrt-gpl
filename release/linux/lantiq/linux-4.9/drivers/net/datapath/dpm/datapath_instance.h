// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef DATAPATH_INSTANCE_H
#define DATAPATH_INSTANCE_H

extern int dp_cap_num;
extern struct dp_hw_cap hw_cap_list[DP_MAX_HW_CAP];

#define DP_DEV_HASH_BIT_LENGTH 10
#define DP_DEV_HASH_SIZE (1 << DP_DEV_HASH_BIT_LENGTH)

#define DP_MOD_HASH_BIT_LENGTH 10
#define DP_MOD_HASH_SIZE (1 << DP_MOD_HASH_BIT_LENGTH)

#define NO_NEED_PMAC(flags)  ((flags & \
		(DP_F_FAST_WLAN | DP_F_FAST_DSL)) && \
		!((flags) & (DP_TX_CAL_CHKSUM | DP_TX_DSL_FCS)))

#define DP_OPS_MASK			GENMASK(15, 1)
#define DP_OPS_OWNER_DPM	BIT(16) /* DP as ops owner */
#define DP_OPS_OWNER_EXT	BIT(17) /* external user as ops owner */

extern struct hlist_head dp_dev_list[DP_DEV_HASH_SIZE];
u32 dp_dev_hash(void *dev);
struct dp_dev *dp_dev_lookup(struct net_device *dev);

struct dp_mod {
	struct hlist_node hlist;
	struct module *mod;
	u16 ep;
	int inst;
};

struct dp_dev {
	struct hlist_node hlist;
	struct net_device *dev;
	int inst;
	int ep;
	int bp;
	int ctp;
	int fid;
	u32 count;
	int cb_cnt;
	u32 owner;
	const struct net_device_ops *old_dev_ops;
	struct net_device_ops new_dev_ops;
	const struct ethtool_ops *old_ethtool_ops;
	struct ethtool_ops new_ethtool_ops;
#if IS_ENABLED(CONFIG_NET_SWITCHDEV)
	struct switchdev_ops *old_swdev_ops;
	struct switchdev_ops new_swdev_ops;
#endif
#if IS_ENABLED(CONFIG_INTEL_VPN)
	struct xfrmdev_ops *old_xfrm_ops;
	struct xfrmdev_ops new_xfrm_ops;
#endif
};

/*dp_inst_p: dp instance basic property */
int dp_get_inst_via_dev(struct net_device *dev,
			char *subif_name, u32 flag);
int dp_get_inst_via_module(struct module *owner, u16 ep, u32 flag);
struct dp_hw_cap *match_hw_cap(struct dp_inst_info *info, u32 flag);
int dp_inst_add_dev(struct net_device *dev, int inst,
		    int ep, int bp, int ctp, u32 flag);
int dp_inst_del_dev(struct net_device *dev, int inst,
		    int ep, u16 ctp, u32 flag);
int dp_inst_insert_mod(struct module *owner, u16 ep, u32 inst, u32 flag);
int dp_inst_del_mod(struct module *owner, u16 ep, u32 flag);
int dp_set_net_dev_ops_priv(struct net_device *dev, void *ops_cb,
			    int ops_offset, u32 flag);

int proc_inst_dev_dump(struct seq_file *s, int pos);
int proc_inst_dev_start(void);
int proc_inst_mod_dump(struct seq_file *s, int pos);
int proc_inst_hal_dump(struct seq_file *s, int pos);
int proc_inst_dump(struct seq_file *s, int pos);

int proc_inst_mod_start(void);

#endif
