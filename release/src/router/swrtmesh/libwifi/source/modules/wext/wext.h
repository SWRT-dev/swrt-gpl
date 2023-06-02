/*
 * wext.h - WE related apis to be used _only_ internally by this lib
 *
 * Copyright (C) 2018 iopsys Software Solutions AB. All rights reserved.
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

#ifndef WEXT_H
#define WEXT_H

#include <stdint.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

struct scan_param;

#if __GNUC__ >= 4
# define LIBWIFI_INTERNAL	__attribute__((visibility("hidden")))
#else
# define LIBWIFI_INTERNAL
#endif

LIBWIFI_INTERNAL int wext_scan(const char *ifname, struct scan_param *p);
LIBWIFI_INTERNAL int wext_get_scan_results(const char *ifname,
					struct wifi_bss **bsss, int *num);
LIBWIFI_INTERNAL int wext_get_ssid(const char *ifname, char *ssid);
LIBWIFI_INTERNAL int wext_get_bssid(const char *ifname, char *bssid);
LIBWIFI_INTERNAL int wext_get_channel_list(const char *ifname, uint32_t *chlist,
				int *num, const char *cc, enum wifi_band band,
				enum wifi_bw bw);
LIBWIFI_INTERNAL int wext_get_channel(const char *ifname, uint32_t *channel,
							enum wifi_bw *bw);

LIBWIFI_INTERNAL int wext_get_security(const char *ifname, uint32_t *auth,
					uint32_t *enc);

#ifdef __cplusplus
}
#endif
#endif /* WEXT_H */
