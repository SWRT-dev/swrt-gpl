/*
 * wifi.c - API glue
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
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
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <sys/ioctl.h>
#include <sys/random.h>
#include <net/if.h>
#include <time.h>

//#define LIBEASY_TRACE_TIME

#include <easy/easy.h>
#include "debug.h"
#include "wifi.h"

extern const struct wifi_driver *wifi_drivers[];
extern uint32_t num_wifi_drivers;

#define drv_op(driver, op, ...)						\
({									\
	int _ret = -ENOTSUP;						\
	if (driver) {							\
		if (driver->op)						\
			_ret = driver->op(__VA_ARGS__);			\
		else if (driver->fallback && driver->fallback->op)	\
			_ret = driver->fallback->op(__VA_ARGS__);	\
	}								\
	_ret;								\
})

const struct wifi_driver *get_wifi_driver(const char *ifname)
{
	int i;
	int s;

	if (!ifname || ifname[0] == '\0')
		return NULL;

	for (i = 0; i < num_wifi_drivers; i++) {
		char *token = NULL, *ifpx, *tmp = NULL;
		char drv[16] = {0};

		strncpy(drv, wifi_drivers[i]->name, 15);
		for (ifpx = drv; ; ifpx = NULL) {
			token = strtok_r(ifpx, ",", &tmp);
			if (token == NULL)
				break;

			if (strstr(ifname, token) && (s = strspn(ifname, token)) &&
			    (ifname[s] == '\0' || (ifname[s] >= '0' && ifname[s] <= '9'))) {
				return wifi_drivers[i];
			}
		}


		if (strstr(ifname, drv) && (s = strspn(ifname, drv)) &&
		    (ifname[s] == '\0' || (ifname[s] >= '0' && ifname[s] <= '9'))) {
			return wifi_drivers[i];
		}
	}

	return NULL;
}

int wifi_driver_info(const char *name, struct wifi_metainfo *info)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, info, name, info);

	EXIT(ret);
	return ret;
}

int wifi_scan(const char *ifname, struct scan_param *p)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, scan, ifname, p);

	EXIT(ret);
	return ret;
}

int wifi_scan_ex(const char *ifname, struct scan_param_ex *sp)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, scan_ex, ifname, sp);

	EXIT(ret);
	return ret;
}

int wifi_get_scan_results(const char *ifname, struct wifi_bss *bsss, int *nr)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_scan_results, ifname, bsss, nr);

	EXIT(ret);
	return ret;
}

int wifi_get_bss_scan_result(const char *ifname, uint8_t *bssid,
						struct wifi_bss_detail *b)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_bss_scan_result, ifname, bssid, b);

	EXIT(ret);
	return ret;
}

#if 0
int wifi_get_device_id(char *ifname, int *id)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);

	if (drv && drv->get_device_id)
		return drv->get_device_id(ifname, id);

	return -ENOTSUP;
}
#endif

int wifi_get_ifstatus(const char *ifname, ifstatus_t *f)
{
	int ret;

	ENTER();
	ret = get_ifstatus(ifname, f);

	EXIT(ret);
	return ret;
}

int wifi_radio_get_ifstatus(const char *ifname, ifstatus_t *f)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret;

	ENTER();
	ret = drv_op(drv, radio.get_ifstatus, ifname, f);
	if (ret == -ENOTSUP)
		ret = wifi_get_ifstatus(ifname, f);

	EXIT(ret);
	return ret;
}

int wifi_get_ifoperstatus(const char *ifname, ifopstatus_t *opstatus)
{
	int ret;

	ENTER();
	ret = if_getoperstate(ifname, opstatus);
	EXIT(ret);
	return ret;
}

int wifi_get_caps(const char *ifname, struct wifi_caps *caps)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_caps, ifname, caps);

	EXIT(ret);
	return ret;
}

int wifi_radio_get_caps(const char *name, struct wifi_caps *caps)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, radio.get_caps, name, caps);

	EXIT(ret);
	return ret;
}

int wifi_get_supp_stds(const char *name, uint8_t *std)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, radio.get_supp_stds, name, std);

	EXIT(ret);
	return ret;
}

int wifi_get_bssid(const char *ifname, uint8_t *bssid)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_bssid, ifname, bssid);

	EXIT(ret);
	return ret;
}

int wifi_get_ssid(const char *ifname, char *ssid)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_ssid, ifname, ssid);

	EXIT(ret);
	return ret;
}

int wifi_get_channel(const char *ifname, uint32_t *channel, enum wifi_bw *bw)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_channel, ifname, channel, bw);

	EXIT(ret);
	return ret;
}

int wifi_vendor_cmd(const char *ifname, uint32_t vid, uint32_t subcmd,
			uint8_t *in, int ilen, uint8_t *out, int *olen)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, vendor_cmd, ifname, vid, subcmd, in, ilen, out, olen);

	EXIT(ret);
	return ret;
}


int wifi_subscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.subscribe_frame, ifname, type, stype);

	EXIT(ret);
	return ret;
}

int wifi_unsubscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.unsubscribe_frame, ifname, type, stype);

	EXIT(ret);
	return ret;
}

int wifi_set_4addr(const char *ifname, bool enable)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, set_4addr, ifname, enable);

	EXIT(ret);
	return ret;
}

int wifi_get_4addr(const char *ifname, bool *enabled)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_4addr, ifname, enabled);

	EXIT(ret);
	return ret;
}

int wifi_get_4addr_parent(const char *ifname, char *parent)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_4addr_parent, ifname, parent);

	EXIT(ret);
	return ret;
}

int wifi_set_vlan(const char *ifname, struct vlan_param vlan)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, set_vlan, ifname, vlan);

	EXIT(ret);
	return ret;
}

int wifi_link_measure(const char *ifname, uint8_t *sta)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, link_measure, ifname, sta);

	EXIT(ret);
	return ret;
}

int wifi_set_channel(const char *ifname, uint32_t channel, enum wifi_bw bw)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, set_channel, ifname, channel, bw);

	EXIT(ret);
	return ret;
}

int wifi_chan_switch(const char *ifname, struct chan_switch_param *param)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, chan_switch, ifname, param);

	EXIT(ret);
	return ret;
}

int wifi_mbo_disallow_assoc(const char *ifname, uint8_t reason)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, mbo_disallow_assoc, ifname, reason);

	EXIT(ret);
	return ret;
}

int wifi_ap_set_state(const char *ifname, bool up)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, ap_set_state, ifname, up);

	EXIT(ret);
	return ret;
}

int wifi_get_supp_channels(const char *ifname, uint32_t *chlist, int *num,
					const char *cc, enum wifi_band band,
					enum wifi_bw bw)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_supp_channels, ifname, chlist, num, cc, band, bw);

	if (ret == -ENOTSUP)
		ret = wifi_get_valid_channels(ifname, band, bw, cc, chlist, num);

	EXIT(ret);
	return ret;
}

int wifi_get_oper_stds(const char *name, uint8_t *std)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_oper_stds, name, std);

	EXIT(ret);
	return ret;
}

int wifi_get_country(const char *name, char *alpha2)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_country, name, alpha2);

	EXIT(ret);
	return ret;
}

int wifi_get_countrylist(const char *name, char *cc, int *num)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_countrylist, name, cc, num);

	EXIT(ret);
	return ret;
}

int wifi_get_oper_band(const char *name, enum wifi_band *band)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_oper_band, name, band);

	EXIT(ret);
	return ret;
}

int wifi_get_supp_band(const char *name, uint32_t *bands)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_supp_band, name, bands);

	EXIT(ret);
	return ret;
}

int wifi_get_mode(const char *ifname, enum wifi_mode *mode)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_mode, ifname, mode);

	EXIT(ret);
	return ret;
}

int wifi_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_security, ifname, auth, enc);

	EXIT(ret);
	return ret;
}

int wifi_get_maxrate(const char *ifname, unsigned long *rate)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_maxrate, ifname, rate);

	EXIT(ret);
	return ret;
}

int wifi_get_supp_opclass(const char *name, int *num_opclass,
						struct wifi_opclass *o)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_supp_opclass, name, num_opclass, o);
	if (ret == -ENOTSUP)
		ret = wifi_get_supported_opclass(name, num_opclass, o);

	EXIT(ret);
	return ret;
}

int wifi_get_curr_opclass(const char *name, struct wifi_opclass *o)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_curr_opclass, name, o);
	if (ret == -ENOTSUP)
		ret = wifi_get_opclass(name, o);

	EXIT(ret);
	return ret;
}

int wifi_get_bandwidth(const char *name, enum wifi_bw *bw)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_bandwidth, name, bw);

	EXIT(ret);
	return ret;
}

int wifi_get_supp_bandwidths(const char *name, uint32_t *bws)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_supp_bandwidths, name, bws);

	EXIT(ret);
	return ret;
}

int wifi_get_noise(const char *ifname, int *noise)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_noise, ifname, noise);

	EXIT(ret);
	return ret;
}

int wifi_radio_get_stats(const char *ifname, struct wifi_radio_stats *s)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, radio.get_stats, ifname, s);

	EXIT(ret);
	return ret;
}

int wifi_radio_list(struct radio_entry *radio, int *num)
{
	const struct wifi_driver *drv;
	struct radio_entry *r;
	int cur_num;
	int n;
	int i;

	cur_num = 0;
	n = *num;
	r = radio;

        /*
         * Return complete list or fail. In case of fail upper
         * layer will fallback to UCI.
         */
	for (i = 0; i < num_wifi_drivers; i++) {
		drv = wifi_drivers[i];
		if (!drv)
			return -1;
		if (!drv->radio_list)
			return -1;

		if (drv->radio_list(r, &n))
			return -1;

		cur_num += n;
		if (cur_num >= *num)
			break;

		/* Calc left size */
		n = *num - cur_num;
		r = &radio[cur_num];
	}

	*num = cur_num;

	return 0;
}

int wifi_radio_info(const char *ifname, struct wifi_radio *info)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, radio.info, ifname, info);

	EXIT(ret);
	return ret;
}

int wifi_radio_get_hwaddr(const char *ifname, uint8_t *hwaddr)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, radio.get_hwaddr, ifname, hwaddr);

	EXIT(ret);
	return ret;
}

int wifi_ap_info(const char *ifname, struct wifi_ap *info)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.ap_info, ifname, info);

	EXIT(ret);
	return ret;
}

int wifi_ap_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_stats, ifname, s);

	EXIT(ret);
	return ret;
}

int wifi_get_beacon_ies(const char *ifname, uint8_t *ies, int *len)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_beacon_ies, ifname, ies, len);

	EXIT(ret);
	return ret;
}

int wifi_acs(const char *ifname, struct acs_param *p)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, acs, ifname, p);

	EXIT(ret);
	return ret;
}

int wifi_get_assoclist(const char *ifname, uint8_t *stas, int *num)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_assoclist, ifname, stas, num);

	EXIT(ret);
	return ret;
}

int wifi_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_sta_info, ifname, addr, info);

	EXIT(ret);
	return ret;
}

int wifi_radio_get_param(const char *name, const char *param, int *len, void *val)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, radio.get_param, name, param, len, val);

	EXIT(ret);
	return ret;
}

int wifi_add_iface(const char *name, enum wifi_mode m, char *argv[])
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, add_iface, name, m, argv);

	EXIT(ret);
	return ret;
}

int wifi_del_iface(const char *name, const char *ifname)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, del_iface, name, ifname);

	EXIT(ret);
	return ret;
}

int wifi_list_iface(const char *name, struct iface_entry *iface, int *num)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, list_iface, name, iface, num);

	EXIT(ret);
	return ret;
}

int wifi_channels_info(const char *name, struct chan_entry *channel, int *num)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, channels_info, name, channel, num);

	EXIT(ret);
	return ret;
}

int wifi_start_cac(const char *name, int channel, enum wifi_bw bw,
		   enum wifi_cac_method method)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, start_cac, name, channel, bw, method);

	EXIT(ret);
	return ret;
}

int wifi_stop_cac(const char *name)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, stop_cac, name);

	EXIT(ret);
	return ret;
}

int wifi_get_opclass_preferences(const char *name, struct wifi_opclass *opclass,
				 int *num)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_opclass_preferences, name, opclass, num);

	EXIT(ret);
	return ret;
}

int wifi_simulate_radar(const char *name, struct wifi_radar_args *radar)
{
	const struct wifi_driver *drv = get_wifi_driver(name);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, simulate_radar, name, radar);

	EXIT(ret);
	return ret;
}

#if 0
int wifi_get_sta_stats(const char *ifname, uint8_t *addr, struct wifi_sta_stats *sts)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);

	if (drv && drv->get_sta_stats)
		return drv->get_sta_stats(ifname, addr, sts);

	return -1;
}
#endif

int wifi_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, disconnect_sta, ifname, sta, reason);

	EXIT(ret);
	return ret;
}

int wifi_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, monitor_sta, ifname, sta, cfg);

	EXIT(ret);
	return ret;
}

int wifi_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_monitor_sta, ifname, sta, mon);

	EXIT(ret);
	return ret;
}

int wifi_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num_stas)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, get_monitor_stas, ifname, stas, num_stas);

	EXIT(ret);
	return ret;
}

int wifi_probe_sta(const char *ifname, uint8_t *sta)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.probe_sta, ifname, sta);

	EXIT(ret);
	return ret;
}

int wifi_restrict_sta(const char *ifname, uint8_t *sta, int enable)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, restrict_sta, ifname, sta, enable);

	EXIT(ret);
	return ret;
}

int wifi_get_neighbor_list(const char *ifname, struct nbr *nbrs, int *nr)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_neighbor_list, ifname, nbrs, nr);

	EXIT(ret);
	return ret;
}

int wifi_add_neighbor(const char *ifname, struct nbr nbr)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.add_neighbor, ifname, nbr);

	EXIT(ret);
	return ret;
}

int wifi_del_neighbor(const char *ifname, unsigned char *bssid)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.del_neighbor, ifname, bssid);

	EXIT(ret);
	return ret;
}

int wifi_req_beacon_report(const char *ifname, uint8_t *sta,
				struct wifi_beacon_req *param, size_t param_sz)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.req_beacon_report, ifname, sta, param, param_sz);

	EXIT(ret);
	return ret;
}

int wifi_get_beacon_report(const char *ifname, uint8_t *sta,
				struct sta_nbr *snbrs, int *nr)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_beacon_report, ifname, sta, snbrs, nr);

	EXIT(ret);
	return ret;
}

int wifi_req_bss_transition(const char *ifname, unsigned char *sta,
		int bsss_nr, unsigned char *bsss, unsigned int tmo)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.req_bss_transition, ifname, sta, bsss_nr, bsss, tmo);

	EXIT(ret);
	return ret;
}

int wifi_req_btm(const char *ifname, uint8_t *sta, int bsss_nr,
				struct nbr *bsss, struct wifi_btmreq *b)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.req_btm, ifname, sta, bsss_nr, bsss, b);

	EXIT(ret);
	return ret;
}

int wifi_get_11rkeys(const char *ifname, unsigned char *sta,
			uint8_t *r1khid)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_11rkeys, ifname, sta, r1khid);

	EXIT(ret);
	return ret;
}

int wifi_set_11rkeys(const char *ifname, struct fbt_keys *fk)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.set_11rkeys, ifname, fk);

	EXIT(ret);
	return ret;
}

int wifi_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.add_vendor_ie, ifname, req);

	EXIT(ret);
	return ret;
}

int wifi_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.del_vendor_ie, ifname, req);

	EXIT(ret);
	return ret;
}

int wifi_get_vendor_ies(const char *ifname, struct vendor_ie *ies, int *num_ies)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_vendor_ies, ifname, ies, num_ies);

	EXIT(ret);
	return ret;
}

int wifi_sta_info(const char *ifname, struct wifi_sta *sta)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.sta_info, ifname, sta);

	EXIT(ret);
	return ret;
}

int wifi_sta_get_stats(const char *ifname, struct wifi_sta_stats *s)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.sta_get_stats, ifname, s);

	EXIT(ret);
	return ret;
}

int wifi_sta_get_ap_info(const char *ifname, struct wifi_bss *info)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.sta_get_ap_info, ifname, info);

	EXIT(ret);
	return ret;
}

int wifi_sta_disconnect_ap(const char *ifname, uint32_t reason)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.sta_disconnect_ap, ifname, reason);

	EXIT(ret);
	return ret;
}

int wifi_register_event(const char *ifname, struct event_struct *ev, void **handle)
{
	const struct wifi_driver *drv;
	int ret = -ENOTSUP;

	if (!ifname)
		return easy_register_event(ev, handle);

	drv = get_wifi_driver(ifname);
	ret = drv_op(drv, register_event, ifname, ev, handle);
	if (ret == -ENOTSUP)
		ret = easy_register_event(ev, handle);

	return ret;
}

int wifi_unregister_event(const char *ifname, void *handle)
{
	const struct wifi_driver *drv;
	int ret = -ENOTSUP;

	if (!ifname || ifname[0] == '\0')
		return easy_unregister_event(handle);

	drv = get_wifi_driver(ifname);
	ret = drv_op(drv, unregister_event, ifname, handle);
	if (ret == -ENOTSUP)
		ret = easy_unregister_event(handle);

	return ret;
}

int wifi_recv_event(const char *ifname, void *handle)
{
	const struct wifi_driver *drv;
	int ret = -1;

	if (!ifname || ifname[0] == '\0')
		return easy_recv_event(handle);


	drv = get_wifi_driver(ifname);
	ret = drv_op(drv, recv_event, ifname, handle);
	if (ret == -ENOTSUP)
		ret = easy_recv_event(handle);

	return ret;
}

int wifi_start_wps(const char *ifname, struct wps_param wps)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.start_wps, ifname, wps);

	EXIT(ret);
	return ret;
}

int wifi_stop_wps(const char *ifname)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.stop_wps, ifname);

	EXIT(ret);
	return ret;
}

int wifi_get_wps_status(const char *ifname, enum wps_status *s)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_wps_status, ifname, s);

	EXIT(ret);
	return ret;
}

int wifi_get_wps_pin(const char *ifname, unsigned long *pin)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_wps_pin, ifname, pin);

	EXIT(ret);
	return ret;
}

int wifi_set_wps_pin(const char *ifname, unsigned long pin)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.set_wps_pin, ifname, pin);

	EXIT(ret);
	return ret;
}

int wifi_get_wps_device_info(const char *ifname, struct wps_device *info)
{
	const struct wifi_driver *drv = get_wifi_driver(ifname);
	int ret = -ENOTSUP;

	ENTER();
	ret = drv_op(drv, iface.get_wps_device_info, ifname, info);

	EXIT(ret);
	return ret;
}

int wifi_generate_wps_pin(unsigned long *pin)
{
	unsigned long PIN;
	unsigned long int accum = 0;
	int digit;
	char local_devPwd[32];
	union {
		unsigned char rand_bytes[8];
		uint32_t rand_uint32[2];
	} rand_data;
	ssize_t ret;

	ENTER();
	ret = getrandom(rand_data.rand_bytes, 8, GRND_NONBLOCK);
	if (ret != 8) {
		EXIT(-1);
		return -1;
	}

	sprintf(local_devPwd, "%08u", *(uint32_t *)rand_data.rand_uint32);
	local_devPwd[7] = '\0';
	PIN = strtoul(local_devPwd, NULL, 10);

	PIN *= 10;
	accum += 3 * ((PIN / 10000000) % 10);
	accum += 1 * ((PIN / 1000000) % 10);
	accum += 3 * ((PIN / 100000) % 10);
	accum += 1 * ((PIN / 10000) % 10);
	accum += 3 * ((PIN / 1000) % 10);
	accum += 1 * ((PIN / 100) % 10);
	accum += 3 * ((PIN / 10) % 10);

	digit = (accum % 10);
	accum = (10 - digit) % 10;

	PIN += accum;
	*pin = PIN;

	EXIT(0);
	return 0;
}

int wifi_is_wps_pin_valid(unsigned long pin)
{
	unsigned long int accum = 0;
	unsigned int len;
	int valid = 0;
	char pinstr[16] = {0};

	ENTER();
	sprintf(pinstr, "%lu", pin);
	len = strlen(pinstr);
	if (len == 8) {
		accum += 3 * ((pin / 10000000) % 10);
		accum += 1 * ((pin / 1000000) % 10);
		accum += 3 * ((pin / 100000) % 10);
		accum += 1 * ((pin / 10000) % 10);
		accum += 3 * ((pin / 1000) % 10);
		accum += 1 * ((pin / 100) % 10);
		accum += 3 * ((pin / 10) % 10);
		accum += 1 * ((pin / 1) % 10);

		if ((accum % 10) == 0)
			valid = 1;
	} else if (len == 4) {
		valid = 1;
	}

	EXITV();
	return valid;
}

/*
 * This is actually an ABI list. Do NOT change the order of the apis
 * in the list unless similar adjustment is made within the wifi_driver
 * operations structure.
 * It serves as a convenient list to see at a glance the APIs that
 * libwifi currently supports.
 */
const char *libwifi_apis[] = {
	"libwifi_get_apis",    /* always should be first */

	"wifi_driver_info",
	"wifi_radio_list",

	/*
	 * wifi radio apis
	 */
	"wifi_radio_info",
	"wifi_get_supp_band",
	"wifi_get_oper_band",
	"wifi_radio_get_ifstatus",
	"wifi_radio_get_caps",
	"wifi_get_supp_stds",
	"wifi_get_oper_stds",
	"wifi_get_country",
	"wifi_get_countrylist",
	"wifi_get_channel",
	"wifi_set_channel",  /* deprecate */
	"wifi_get_supp_channels",
	"wifi_get_oper_channels",
	"wifi_get_supp_opclass",
	"wifi_get_curr_opclass",
	"wifi_get_bandwidth",
	"wifi_get_supp_bandwidths",
	"wifi_get_maxrate",
	"wifi_get_basic_rates",
	"wifi_get_oper_rates",
	"wifi_get_supp_rates",
	"wifi_radio_get_stats",
	"wifi_scan",
	"wifi_scan_ex",
	"wifi_get_scan_results",
	"wifi_get_bss_scan_result",
	"wifi_get_noise",
	"wifi_acs",
	"wifi_radio_get_param",
	"wifi_radio_set_param",
	"wifi_radio_get_hwaddr",
	"wifi_add_iface",
	"wifi_del_iface",
	"wifi_list_iface",
	"wifi_channels_info",
	"wifi_start_cac",
	"wifi_stop_cac",
	"wifi_get_opclass_preferences",
	"wifi_simulate_radar",


	/*
	 * wifi interface apis
	 */
	"wifi_start_wps",
	"wifi_stop_wps",
	"wifi_get_wps_status",
	"wifi_get_wps_pin",
	"wifi_set_wps_pin",
	"wifi_get_wps_device_info",
	"wifi_get_caps",
	"wifi_get_mode",
	"wifi_get_security",
	"wifi_add_vendor_ie",
	"wifi_del_vendor_ie",
	"wifi_get_vendor_ies",
	"wifi_get_param",
	"wifi_set_param",
	"wifi_vendor_cmd",
	"wifi_subscribe_frame",
	"wifi_unsubscribe_frame",
	"wifi_set_4addr",
	"wifi_get_4addr",
	"wifi_get_4addr_parent",
	"wifi_set_vlan",
	"wifi_link_measure",

	"wifi_ap_info",
	"wifi_get_bssid",
	"wifi_get_ssid",
	"wifi_ap_get_stats",
	"wifi_get_beacon_ies",
	"wifi_get_assoclist",
	"wifi_get_sta_info",
	"wifi_get_sta_stats",
	"wifi_disconnect_sta",
	"wifi_restrict_sta",
	"wifi_monitor_sta",
	"wifi_get_monitor_sta",
	"wifi_get_monitor_stas",
	"wifi_probe_sta",
	"wifi_add_neighbor",
	"wifi_del_neighbor",
	"wifi_get_neighbor_list",
	"wifi_req_beacon_report",
	"wifi_get_beacon_report",
	"wifi_req_bss_transition",
	"wifi_req_btm",
	"wifi_get_11rkeys",
	"wifi_set_11rkeys",
	"wifi_chan_switch",
	"wifi_mbo_disallow_assoc",
	"wifi_ap_set_state",

	"wifi_sta_info",
	"wifi_sta_get_stats",
	"wifi_sta_get_ap_info",
	"wifi_sta_disconnect_ap",

	"wifi_register_event",
	"wifi_recv_event",
	"libwifi_get_version",   /* always the last api */
};

#define libwifi_apis_num  sizeof(libwifi_apis) / sizeof(libwifi_apis[0])

const char **libwifi_get_apis(char *name)
{
	return libwifi_apis;
}

const char *libwifi_get_version(void)
{
	return libwifi_version;
}

typedef int (*libwifi_api_t)(char *name, ...);

int libwifi_supports(const char *ifname, char *apiname)
{
	const struct wifi_driver *drv;
	libwifi_api_t find;
	int num_radio_apis;
	uint8_t *base;
	uint8_t *basefb;
	int i, idx;

	if (!ifname || !apiname) {
		errno = -EINVAL;
		return 0;
	}

	drv = get_wifi_driver(ifname);
	if (!drv) {
		errno = -EINVAL;
		return 0;
	}

	num_radio_apis = sizeof(drv->radio)/sizeof(libwifi_api_t);
	base = (uint8_t *)drv;
	basefb = (uint8_t *)drv->fallback;

	for (i = 0; i < libwifi_apis_num; i++) {
		if (strncmp(libwifi_apis[i], apiname, strlen(libwifi_apis[i])))
			continue;

		if (i < 3)
			return 1;

		idx = i - 3;
		base += idx <= num_radio_apis ?
			offsetof(struct wifi_driver, radio) :
			offsetof(struct wifi_driver, iface);

		if (basefb) {
			basefb += idx <= num_radio_apis ?
					offsetof(struct wifi_driver, radio) :
					offsetof(struct wifi_driver, iface);
		}

		if (idx > num_radio_apis)
			idx -= num_radio_apis;

		find = *((libwifi_api_t *)(base +
				idx * sizeof(libwifi_api_t)));

		if (!find && basefb) {
			find = *((libwifi_api_t *)(basefb +
					idx * sizeof(libwifi_api_t)));
		}

		return (!find) ? 0 : 1;
	}

	errno = -ENOTSUP;
	return 0;
}
