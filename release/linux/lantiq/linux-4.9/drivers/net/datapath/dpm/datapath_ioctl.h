// SPDX-License-Identifier: GPL-2.0
/*****************************************************************************
 * Copyright (c) 2020 - 2021, MaxLinear, Inc.
 * Copyright 2016 - 2020 Intel Corporation
 *
 *****************************************************************************/

#ifndef DATAPATH_IOCTL_H
#define DATAPATH_IOCTL_H

#include <net/datapath_api.h>
#include "datapath.h"
#include "datapath_instance.h"

int dp_register_ptp_ioctl(struct net_device *dev, int reset);
int dp_ops_reset(struct dp_dev *dp_dev,
		 struct net_device *dev);
int dp_ops_set(void **dev_ops, int ops_cb_offset,
	       size_t ops_size, void **dp_orig_ops_cb,
	       void *dp_new_ops, void *new_ops_cb);

#endif
/*DATAPATH_IOCTL_H*/

