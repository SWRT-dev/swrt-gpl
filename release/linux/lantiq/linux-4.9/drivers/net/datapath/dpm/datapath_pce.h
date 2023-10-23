// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 *
 *****************************************************************************/

#ifndef DATAPATH_PCE_H
#define DATAPATH_PCE_H

/* Max Block Size for CTP by default, if not given through alloc API */
#define PCE_MAX_BLK_SIZE_CTP	32

/* Max Sub-Block Size for CTP VLAN aware feature */
#define VLAN_PCE_MAX_SUBBLK_SIZE 8

/* CTP flow entry index used during reset in GSWIP CTP_PortCfgSet */
#define CTP_FLOW_ENTRY_IDX_RESET 0xFFFFFFFF

/* Max default Sub-Block Size */
#define PCE_SUBBLK_SIZE_MAX	64

/* Main PCE rule entry struct */
struct dp_pce_rule {
	u32 hw_rules_max;
	u32 glbl_firstindex;		/* Global Rule First Index */
	u32 glbl_rules_max;		/* Global rules Max allowed */
	struct list_head blk_list;	/* List of block's in PCE */
};

/* Blocks which will be assigned Global have 1 block, Each CTP have 1 block */
struct dp_blk_entry {
	struct list_head list;
	struct dp_pce_blk_info info;  /* Block info */
	char blk_name[32];	      /* Block Name */
	u32 blk_firstindex;	      /* First PCE index for this Block */
	u32 blk_rules_max;	      /* num of rules allowed for this Block */
	u32 blk_rules_used;	      /* num of rules used for this Block */
	struct list_head subblk_list; /* Rules divided as Sub-Blks */
};

/* Sub-Blocks which will be assigned for Global as well as CTP's */
struct dp_subblk_entry {
	struct list_head list;
	char subblk_name[32];	/* Sub-Block app name like vlan_aware */
	int subblk_id;		/* Sub-Block ID */
	u32 subblk_firstindex;	/* Sub-Block Starting Pce Index */
	u32 subblk_rules_max;	/* num of rules in this Sub-Block */
	u32 subblk_rules_used;	/* num of rules used for this Sub-Block */
	DECLARE_BITMAP(_used, PCE_SUBBLK_SIZE_MAX);	/* Used rules */
	bool subblk_protected;  /* Indicates this sub-blk is protected or not */
};

/* Global PCE Rule struct */
extern struct dp_pce_rule pce_rule;
int dp_pce_rule_add_priv(struct dp_pce_blk_info *pce_blk_info,
			 GSW_PCE_rule_t *pce);
int dp_pce_rule_del_priv(struct dp_pce_blk_info *pce_blk_info,
			 GSW_PCE_ruleDelete_t *pce);
int dp_pce_rule_get_priv(struct dp_pce_blk_info *pce_blk_info,
			 GSW_PCE_rule_t *pce);
int dp_pce_rule_update_priv(struct dp_pce_blk_info *pce_blk_info,
			    GSW_PCE_rule_t *pce, enum DP_PCE_RULE_UPDATE update);
int dp_ndo_bridge_setlink(struct net_device *dev, struct nlmsghdr *nlh,
			  u16 flags);
int dp_ndo_bridge_dellink(struct net_device *dev, struct nlmsghdr *nlh,
			  u16 flags);
int dp_gsw_copy_ctp_cfg(int inst, int bp, struct dp_subblk_info *blk_info,
			int portid);
#endif /* DATAPATH_PCE_H */
