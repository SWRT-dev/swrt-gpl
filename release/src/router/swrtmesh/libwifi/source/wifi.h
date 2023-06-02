/**
 * @file wifi.h
 * @brief WiFi defintions.
 *
 * Copyright (C) 2020-2023 iopsys Software Solutions AB. All rights reserved.
 *
 * @author anjan.chanda@iopsys.eu
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

#ifndef WIFI_H
#define WIFI_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/types.h>

#include "wifidefs.h"
#include "wifiops.h"

#ifdef __cplusplus
extern "C" {
#endif

/** API versioning stuff */
#define str(s)	_str(s)
#define _str(s)	#s
#define libwifi_version		\
	str(LIBWIFI_MAJOR)"."str(LIBWIFI_MINOR)"-"str(LIBWIFI_REV)


#ifdef __cplusplus
}
#endif

#endif /* WIFI_H */
