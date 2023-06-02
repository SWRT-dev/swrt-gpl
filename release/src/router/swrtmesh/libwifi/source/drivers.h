/*
 * drivers.h - wifi backends header file
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
#ifndef _DRIVERS_H
#define _DRIVERS_H

#include <easy/easy.h>
#include "wifi.h"

#ifdef WIFI_BROADCOM
#ifdef CONFIG_BCM963138
extern const struct wifi_driver bcm_driver;
#else
extern const struct wifi_driver brcm_driver;
#endif
#endif

#ifdef WIFI_MEDIATEK
extern const struct wifi_driver mt_driver;
#endif

#ifdef WIFI_QUALCOMM
extern const struct wifi_driver qca_driver;
#endif

#ifdef WIFI_INTEL
extern const struct wifi_driver intel_driver;
#endif

#ifdef WIFI_TEST
extern const struct wifi_driver test_driver;
#endif

#ifdef WIFI_MAC80211
extern const struct wifi_driver mac80211_driver;
#endif

#endif
