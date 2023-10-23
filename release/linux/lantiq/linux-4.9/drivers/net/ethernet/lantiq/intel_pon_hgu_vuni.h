/*
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License version 2 as published
 *  by the Free Software Foundation.
 *
 *  Copyright (C) 2019 Intel Corporation.
 */

#ifndef _INTEL_PON_HGU_VUNI_H_
#define _INTEL_PON_HGU_VUNI_H_

#define NUM_IF 6

struct vuni_hw {
	struct net_device *devs[NUM_IF];
	int num_devs;
};

/**
 * This structure is used internal purpose
 */
struct intel_vuni_priv {
	/*!< network device interface Statistics */
	struct rtnl_link_stats64 stats;
	struct sk_buff *skb; /*!< skb buffer structure*/
	spinlock_t lock; /*!< spin lock */
	int current_speed; /*!< interface current speed*/
	int full_duplex; /*!< duplex mode*/
	int current_duplex; /*!< current interface duplex mode*/
	unsigned int                flags;  /*!< flags */
	struct module *owner;
	dp_subif_t dp_subif;
	s32 dev_port; /*dev  instance */
	s32 f_dp;   /* status for register to datapath*/
	u32 dp_port_id;
	int num_port;
	struct vuni_hw *hw;
	int id;
	int vani;
	int extra_subif;
	u32 extra_subif_domain;
	/*! min netdevices for extra subif/lct */
	int start;
	/*! max netdevices for extra subif/lct */
	int end;
	struct ethtool_cmd bkup_cmd;
	bool needs_recovery;
};

/**
 * This structure is used internal purpose
 */

struct intel_net_soc_data {
	bool need_defer;
	bool hw_checksum;
	unsigned int queue_num;
	u32 mtu_limit;
};

#endif
