/*
 * drivers.c - list of wifi backends
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: anjan.chanda@iopsys.eu
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2 as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA
 * 02110-1301 USA
 */
#include <stdio.h>
#include "drivers.h"

const struct wifi_driver *wifi_drivers[] = {
#ifdef WIFI_BROADCOM
#ifdef CONFIG_BCM963138
	&bcm_driver,
#else
	&brcm_driver,
#endif
#endif /* WIFI_BROADCOM */

#ifdef WIFI_MEDIATEK
	&mt_driver,
#endif

#ifdef WIFI_QUALCOMM
	&qca_driver,
#endif

#ifdef WIFI_INTEL
	&intel_driver,
#endif

#ifdef WIFI_TEST
	&test_driver,
#endif

#ifdef WIFI_MAC80211
	&mac80211_driver,
#endif
};

uint32_t num_wifi_drivers = sizeof(wifi_drivers)/sizeof(wifi_drivers[0]);
