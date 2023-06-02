/*
 * nlwifi.h - nl80211 apis to be used _only_ internally by this lib
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

#ifndef NLWIFI_H
#define NLWIFI_H

#include <stdint.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

//struct scan_param;

#if __GNUC__ >= 4
# define LIBWIFI_INTERNAL	__attribute__((visibility("hidden")))
#else
# define LIBWIFI_INTERNAL
#endif

#define NLWIFI_MODE_AP          1 << 0
#define NLWIFI_MODE_STA         1 << 1
#define NLWIFI_MODE_AP_STA      NLWIFI_MODE_AP | NLWIFI_MODE_STA
#define NLWIFI_MODE_ANY         0xff

LIBWIFI_INTERNAL int nlwifi_scan(const char *ifname, struct scan_param *p);
LIBWIFI_INTERNAL int nlwifi_scan_ex(const char *ifname, struct scan_param_ex *sp);
LIBWIFI_INTERNAL int nlwifi_get_scan_results(const char *ifname,
					struct wifi_bss *bsss, int *num);
LIBWIFI_INTERNAL int nlwifi_get_ssid(const char *ifname, char *ssid);
LIBWIFI_INTERNAL int nlwifi_get_bssid(const char *ifname, uint8_t *bssid);
LIBWIFI_INTERNAL int nlwifi_get_channel_freq(const char *ifname, uint32_t *control_freq);
LIBWIFI_INTERNAL int nlwifi_get_channel_center_freq1(const char *ifname, uint32_t *center_freq1);
LIBWIFI_INTERNAL int nlwifi_get_channel_list(const char *ifname,
				uint32_t *chlist, int *num, const char *cc,
				enum wifi_band band, enum wifi_bw bw);
LIBWIFI_INTERNAL int nlwifi_get_channel(const char *ifname, uint32_t *channel,
							enum wifi_bw *bw);
LIBWIFI_INTERNAL int nlwifi_get_mode(const char *ifname, enum wifi_mode *mode);

LIBWIFI_INTERNAL int nlwifi_get_supp_channels(const char *ifname,
					uint32_t *chlist, int *num,
					const char *cc,
					enum wifi_band band,
					enum wifi_bw bw);

LIBWIFI_INTERNAL int nlwifi_get_supp_stds(const char *name, uint8_t *std);
LIBWIFI_INTERNAL int nlwifi_get_supp_bandwidths(const char *name, uint32_t *supp_bw);
LIBWIFI_INTERNAL int nlwifi_get_bandwidth(const char *ifname, enum wifi_bw *bandwidth);
LIBWIFI_INTERNAL int nlwifi_vendor_cmd(const char *ifname, uint32_t vid,
					uint32_t subcmd, uint8_t *in, int ilen,
					uint8_t *out, int *olen);

LIBWIFI_INTERNAL int nlwifi_ap_get_caps(const char *ifname, struct wifi_caps *caps);
LIBWIFI_INTERNAL int nlwifi_get_assoclist(const char *ifname, uint8_t *stas,
					int *num_stas);

LIBWIFI_INTERNAL int nlwifi_sta_info(const char *ifname, struct wifi_sta *info);
LIBWIFI_INTERNAL int nlwifi_sta_get_stats(const char *ifname,struct wifi_sta_stats *s);

LIBWIFI_INTERNAL int nlwifi_get_sta_info(const char *ifname, uint8_t *addr,
					struct wifi_sta *info);

LIBWIFI_INTERNAL int nlwifi_radio_info(const char *name, struct wifi_radio *radio);
LIBWIFI_INTERNAL int nlwifi_get_phy_info(const char *name, struct wifi_radio *radio);
LIBWIFI_INTERNAL int nlwifi_radio_get_caps(const char *name, struct wifi_caps *caps);
LIBWIFI_INTERNAL int nlwifi_get_country(const char *name, char *alpha2);

LIBWIFI_INTERNAL int nlwifi_get_phyname(const char *ifname, char *phyname);
LIBWIFI_INTERNAL int nlwifi_get_noise(const char *name, int *noise);


LIBWIFI_INTERNAL int nlwifi_register_event(const char *ifname,
				struct event_struct *ev, void **evhandle);
LIBWIFI_INTERNAL int nlwifi_unregister_event(const char *ifname, void *handle);
LIBWIFI_INTERNAL int nlwifi_recv_event(const char *ifname, void *evhandle);
LIBWIFI_INTERNAL int nlwifi_phy_to_netdev(const char *phy, char *netdev, size_t size);
LIBWIFI_INTERNAL int nlwifi_phy_to_netdev_with_type(const char *phy, char *netdev, size_t size, uint32_t type);
LIBWIFI_INTERNAL int nlwifi_driver_info(const char *name, struct wifi_metainfo *info);
LIBWIFI_INTERNAL int nlwifi_channels_info(const char *name, struct chan_entry *channel, int *num);

LIBWIFI_INTERNAL int nlwifi_get_supp_band(const char *name, uint32_t *bands);
LIBWIFI_INTERNAL int nlwifi_get_oper_band(const char *name, enum wifi_band *band);

LIBWIFI_INTERNAL int nlwifi_get_max_bandwidth(const char *name, enum wifi_bw *max_bw);

LIBWIFI_INTERNAL int nlwifi_get_4addr(const char *ifname, bool *enabled);

LIBWIFI_INTERNAL int nlwifi_start_cac(const char *ifname, int channel, enum wifi_bw bw,
		     						  enum wifi_cac_method method);
LIBWIFI_INTERNAL int nlwifi_stop_cac(const char *name);

struct nlwifi_event_vendor_resp {
	uint32_t oui;
	uint32_t subcmd;
	uint8_t data[];
};

#ifdef __cplusplus
}
#endif
#endif /* NLWIFI_H */
