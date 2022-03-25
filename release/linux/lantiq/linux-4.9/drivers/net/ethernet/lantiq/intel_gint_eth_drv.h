/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef _INTEL_GINT_ETH_DRV_H_
#define _INTEL_GINT_ETH_DRV_H_

#define NUM_ETH_INF 3
#define NUM_GINT_INF 16

#define MAC3_GINT_CFG 0X130C
#define MAC4_GINT_CFG 0X140C

#define MAC3_GINT_HD0_CFG 0X1320
#define MAC3_GINT_HD1_CFG 0x1324
#define MAC3_GINT_HD2_CFG 0x1328
#define MAC3_GINT_HD3_CFG 0x132C
#define MAC3_GINT_HD4_CFG 0x1330
#define MAC3_GINT_HD5_CFG 0x1334
#define MAC3_GINT_HD6_CFG 0x1338


#define MAC4_GINT_HD0_CFG 0X1420
#define MAC4_GINT_HD1_CFG 0x1424
#define MAC4_GINT_HD2_CFG 0x1428
#define MAC4_GINT_HD3_CFG 0x142C
#define MAC4_GINT_HD4_CFG 0x1430
#define MAC4_GINT_HD5_CFG 0x1434
#define MAC4_GINT_HD6_CFG 0x1438

struct gint_hw {
	struct net_device *devs[NUM_ETH_INF];
	int num_devs;
	int port_map[NUM_ETH_INF];
	int wan;
};

struct gint_eth_priv {
	struct net_device_stats stats;
	struct module *owner;
	struct sk_buff *skb; /*!< skb buffer structure*/
	spinlock_t lock; /*!< spin lock */
	dp_subif_t dp_subif;
	u32 dp_port_id;
	u32 subif;
	int id;
	int xgmac_id;
	struct gint_hw	*hw;
	int subif_min;
	int subif_max;
};


#endif /* _INTEL_GINT_ETH_DRV_H_ */






