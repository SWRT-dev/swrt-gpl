/*
 * Copyright (C) Intel Corporation
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License version 2 as published
 * by the Free Software Foundation.
 */

#ifndef DATAPATH_SWIHDEV_API_H
#define DATAPATH_SWIHDEV_API_H

int dp_switchdev_init(void);
int dp_port_register_switchdev(struct dp_dev *dp_dev,
			       struct net_device *dp_port);
int dp_del_br_if(struct net_device *dev, struct net_device *br_dev,
		 int inst, int bport);
int dp_notif_br_alloc(struct net_device *br_dev);

#endif
/*DATAPATH_SWIHDEV_API_H*/
