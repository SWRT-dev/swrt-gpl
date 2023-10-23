// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef DATAPATH_SWIHDEV_API_H
#define DATAPATH_SWIHDEV_API_H

int dp_switchdev_init(void);
int dp_register_switchdev_ops(struct net_device *dev, int reset);
int dp_del_br_if(struct net_device *dev, struct net_device *br_dev,
		 int inst, int bport);
int dp_notif_br_alloc(struct net_device *br_dev);
int dp_notif_br_free(struct net_device *br_dev);

#endif
/*DATAPATH_SWIHDEV_API_H*/
