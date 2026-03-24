/*
 * DirectConnect provides a common interface for the network devices to achieve the full or partial 
   acceleration services from the underlying packet acceleration engine
 * Copyright (c) 2017, Intel Corporation.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms and conditions of the GNU General Public License,
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 */

#ifndef _DIRECTCONNECT_DP_SWPATH_H_
#define _DIRECTCONNECT_DP_SWPATH_H_

extern int32_t
dp_alloc_sw_port(struct module *owner, struct net_device *dev, int32_t dev_port, int32_t port_id, uint32_t flags);
extern int32_t
dp_dealloc_sw_port(struct module *owner, struct net_device *dev, int32_t dev_port, int32_t port_id, uint32_t flags);

#endif

