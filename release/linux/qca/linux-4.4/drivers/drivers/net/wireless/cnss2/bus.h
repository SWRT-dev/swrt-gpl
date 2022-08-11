/* Copyright (c) 2018-2019, The Linux Foundation. All rights reserved.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 and
 * only version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 */

#ifndef _CNSS_BUS_H
#define _CNSS_BUS_H

#include "main.h"

enum cnss_dev_bus_type {
	CNSS_BUS_NONE = -1,
	CNSS_BUS_PCI,
	CNSS_BUS_AHB,
};

void *cnss_bus_dev_to_bus_priv(struct device *dev);
struct cnss_plat_data *cnss_bus_dev_to_plat_priv(struct device *dev);
int cnss_bus_alloc_qdss_mem(struct cnss_plat_data *plat_priv);
void cnss_bus_free_qdss_mem(struct cnss_plat_data *plat_priv);
#endif
