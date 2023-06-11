/*
 * brcm.c - for cfg80211 based broadcom driver
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
 *
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
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <net/if.h>
#include <time.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <dirent.h>
#include <wlioctl_defs.h>

#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "wpactrl_util.h"
#include "wlctrl.h"
#include "debug.h"

static int radio_list(struct radio_entry *radio, int *num)
{
	struct dirent *p;
	struct radio_entry *entry;
	int count = 0;
	DIR *d;

	if (WARN_ON(!(d = opendir("/sys/class/ieee80211"))))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (WARN_ON(count >= *num))
			break;

		libwifi_dbg("radio: %s\n", p->d_name);
		entry = &radio[count];
		strncpy(entry->name, p->d_name, sizeof(entry->name));
		count++;
	}

	closedir(d);
	*num = count;

	return 0;
}

static int is_valid_ap_iface(const char *ifname)
{
	int valid_iface = 0;
	int ret;
	enum wifi_mode mode;
	DIR *d;
	struct dirent *dir;

	d = opendir("/sys/class/net");
	if (WARN_ON(!d))
		return 0;

	while ((dir = readdir(d))) {
		if (strcmp(dir->d_name, ".") == 0 ||
		    strcmp(dir->d_name, "..") == 0)
			continue;
		if (strcmp(ifname, dir->d_name) == 0) {
			valid_iface = 1;
		}
	}
	closedir(d);

	if (!valid_iface)
		return 0;

	ret = nlwifi_get_mode(ifname, &mode);
	if (WARN_ON(ret))
		return 0;

	if (mode == WIFI_MODE_AP)  {
		return 1;
	}

	return 0;
}

/* Radio callbacks */
static int radio_info(const char *name, struct wifi_radio *radio)
{
	char netdev[16];
	int n = 32, m = 32;
	int ret;

	libwifi_dbg("[%s] %s called\n", name, __func__);
	ret = nlwifi_radio_info(name, radio);
	if (ret)
		return ret;
	ret = bcmwl_get_supp_stds(name, &radio->supp_std);
	if (ret)
		return ret;

	WARN_ON(bcmwl_radio_get_beacon_int(name, &radio->beacon_int));
	WARN_ON(bcmwl_radio_get_dtim(name, &radio->dtim_period));
	WARN_ON(bcmwl_radio_get_country(name, radio->regdomain));
	WARN_ON(wifi_get_sideband(name, &radio->extch));
	WARN_ON(bcmwl_radio_get_sideband(name, &radio->extch));
	WARN_ON(bcmwl_radio_get_txpower(name, &radio->txpower_dbm, &radio->txpower));
	WARN_ON(bcmwl_radio_get_supp_bw(name, &radio->supp_bw));
	WARN_ON(bcmwl_radio_get_stats(name, &radio->stats));
	WARN_ON(bcmwl_radio_get_hw_txchain(name, &radio->tx_streams));
	WARN_ON(bcmwl_radio_get_hw_rxchain(name, &radio->rx_streams));
	WARN_ON(bcmwl_radio_get_diag(name, &radio->diag));
	WARN_ON(bcmwl_radio_get_basic_rates(name, &n, radio->basic_rates));
	WARN_ON(bcmwl_radio_get_oper_rates(name, &m, radio->supp_rates));

	if (radio->oper_band != BAND_2)
		WARN_ON(bcmwl_radio_get_cac_methods(name, &radio->cac_methods));

	ret = nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP);
	if (ret == 0) {
		uint8_t he = 0;

		WARN_ON(bcmwl_ap_get_oper_stds(netdev, &radio->oper_std));
		he = !!(radio->oper_std & WIFI_AX);
		WARN_ON(bcmwl_radio_get_gi(name, he, &radio->gi));
		return 0;
	}

	ret = nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_STA);
	if (ret == 0) {
		return supplicant_cli_get_oper_std(netdev, &radio->oper_std);
	}

	radio->oper_std = radio->supp_std;
	return 0;
}

static int radio_get_supp_band(const char *name, uint32_t *bands)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_supp_band(name, bands);
}

static int radio_get_oper_band(const char *name, enum wifi_band *band)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return bcmwl_get_oper_band(name, band);
}

static int radio_get_caps(const char *name, struct wifi_caps *caps)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_radio_get_caps(name, caps);
}

static int radio_get_supp_stds(const char *name, uint8_t *std)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return bcmwl_get_supp_stds(name, std);
}

static int radio_get_oper_stds(const char *name, uint8_t *std)
{
	char netdev[16];
	int ret;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP);
	if (ret == 0) {
		ret = bcmwl_ap_get_oper_stds(netdev, std);
	} else {
		ret = bcmwl_get_supp_stds(name, std);
	}

	return ret;
}

static int radio_get_country(const char *name, char *alpha2)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return bcmwl_radio_get_country(name, alpha2);
}

static int radio_get_countrylist(const char *name, char *cc, int *num)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return bcmwl_radio_get_countrylist(name, cc, num);
}

static int radio_get_channel(const char *name, uint32_t *channel, enum wifi_bw *bw)
{
	char netdev[16];
	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);
	return bcmwl_radio_get_channel(netdev, channel, bw);
}

static int radio_set_channel(const char *name, uint32_t channel, enum wifi_bw bw)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_supp_channels(const char *name, uint32_t *chlist, int *num,
				   const char *alpha2, enum wifi_band f, enum wifi_bw b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_supp_channels(name, chlist, num, alpha2, f, b);
}

static int radio_get_oper_channels(const char *name, uint32_t *chlist, int *num,
				   const char *alpha2, enum wifi_band f, enum wifi_bw b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_curr_opclass(const char *name, struct wifi_opclass *o)
{
	char netdev[16];

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);
	return wifi_get_opclass(netdev, o);;
}

static int radio_get_bandwidth(const char *name, enum wifi_bw *bw)
{
	char netdev[16];
	uint32_t channel;

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);
	return bcmwl_radio_get_channel(netdev, &channel, bw);
}

static int radio_get_maxrate(const char *name, unsigned long *rate_Mbps)
{
	uint32_t chan;
	uint8_t txchains;
	uint32_t lrate;
	uint32_t he_mcs, vht_mcs, ht_mcs;
	uint32_t max_mcs = 0;
	enum wifi_bw bw;
	enum wifi_guard gi;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!rate_Mbps)
		return -EINVAL;

	WARN_ON(bcmwl_radio_get_channel(name, &chan, &bw));
	WARN_ON(bcmwl_radio_get_hw_txchain(name, &txchains));
	WARN_ON(bcmwl_radio_get_maxrates(name, &he_mcs, &vht_mcs, &ht_mcs, &lrate));

	if (he_mcs > 0) {
		bcmwl_radio_get_gi(name, 1, &gi);
		if (gi == WIFI_AUTOGI)
			gi = WIFI_4xLTF_GI800;
		max_mcs = he_mcs;

		*rate_Mbps = wifi_mcs2rate(max_mcs, wifi_bw_enum2MHz(bw), txchains, gi);

		return 0;
	}

	bcmwl_radio_get_gi(name, 0, &gi);
	if (gi == WIFI_AUTOGI)
			gi = WIFI_SGI;

	if (vht_mcs > 0) {
		max_mcs = vht_mcs;
	} else if (ht_mcs > 0) {
		max_mcs = ht_mcs % 8;
	}

	if (max_mcs > 0) {
		*rate_Mbps = wifi_mcs2rate(max_mcs, wifi_bw_enum2MHz(bw), txchains, gi);
		return 0;
	}

	*rate_Mbps = lrate;
	return 0;
}

static int radio_get_basic_rates(const char *name, int *num, uint32_t *rates)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);

	return bcmwl_radio_get_basic_rates(name, num, rates);
}

static int radio_get_oper_rates(const char *name, int *num, uint32_t *rates)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);

	return bcmwl_radio_get_oper_rates(name, num, rates);
}

static int radio_get_supp_rates(const char *name, int *num, uint32_t *rates)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}


static int radio_scan(const char *name, struct scan_param *p)
{
	char netdev[16];

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);

	WARN_ON(bcmwl_radio_reset_chanim_stats(name));
	return nlwifi_scan(netdev, p);
}

int radio_scan_ex(const char *ifname, struct scan_param_ex *sp)
{
	char netdev[16];

	nlwifi_phy_to_netdev(ifname, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", ifname, netdev, __func__);

	WARN_ON(bcmwl_radio_reset_chanim_stats(ifname));
	return nlwifi_scan_ex(netdev, sp);
}

static int radio_get_scan_results(const char *name, struct wifi_bss *bsss, int *num)
{
	char netdev[16];

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);
	return nlwifi_get_scan_results(netdev, bsss, num);
}

static int radio_get_bss_scan_result(const char *name, uint8_t *bssid,
				     struct wifi_bss_detail *b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_noise(const char *name, int *noise)
{
	char netdev[16];

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);
	if (bcmwl_radio_get_noise(netdev, noise))
		*noise = -90;

	return 0;
}

static int radio_acs(const char *name, struct acs_param *p)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return bcmwl_radio_acs(name, p);
}

int radio_start_cac(const char *name, int channel, enum wifi_bw bw,
		   enum wifi_cac_method method)
{
	char netdev[16] = {};
	libwifi_dbg("[%s] %s called\n", name, __func__);

	WARN_ON(nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP));
	return bcmwl_start_cac(netdev, channel, bw, method);
}

int radio_stop_cac(const char *name)
{
	char netdev[16] = {};
	libwifi_dbg("[%s] %s called\n", name, __func__);

	WARN_ON(nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP));
	return bcmwl_stop_cac(netdev);
}

static int radio_get_opclass_preferences(const char *name,
					 struct wifi_opclass *opclass,
					 int *num)
{
	int ret;
	libwifi_dbg("[%s] %s called\n", name, __func__);

	/* First get this using common code */
	ret = wifi_get_opclass_pref(name, num, opclass);

	/* If module specific channes required apply here */

	return ret;
}

int radio_simulate_radar(const char *name, struct wifi_radar_args *radar)
{
	char netdev[16] = {};

	libwifi_dbg("[%s] %s called ch:%d, bandwidth:%d, type:0x%x, subband:0x%x\n",
	            name, __func__, radar->channel, radar->bandwidth, radar->type,
	            radar->subband_mask);

	WARN_ON(nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP));

	return bcmwl_simulate_radar(netdev, radar);
}


static int radio_get_param(const char *name, const char *param, int *len, void *val)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_set_param(const char *name, const char *param, int len, void *val)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_add_iface(const char *name, enum wifi_mode m, char *argv[])
{
	char buf[1024];
	char *ifname = NULL;
	int i = 0;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	while (argv[i]) {
		if (!strcmp(argv[i], "ifname")) {
			ifname = argv[i+1];
			break;
		}

		i+=2;
	}

	if (WARN_ON(!ifname))
		return -1;

	switch (m) {
	case WIFI_MODE_STA:
		/* Today for sta we allow only radio netdev */
		if (WARN_ON(strcmp(name, ifname)))
			return -1;
		chrCmd(buf, sizeof(buf), "iw %s interface add %s type managed", name, ifname);
		break;
	case WIFI_MODE_AP:
		chrCmd(buf, sizeof(buf), "iw %s interface add %s type ap", name, ifname);
		break;
	case WIFI_MODE_MONITOR:
		chrCmd(buf, sizeof(buf), "iw %s interface add %s type monitor", name, ifname);
		break;
	default:
		return -1;
	}

	return 0;
}

static int radio_del_iface(const char *name, const char *ifname)
{
	char buf[1024];

	libwifi_dbg("[%s] %s called\n", name, __func__);

	chrCmd(buf, sizeof(buf), "iw %s del", ifname);

	return 0;
}

static int radio_list_iface(const char *name, struct iface_entry *iface, int *num)
{
	char path[256];
	struct dirent *p;
	struct iface_entry *entry;
	int count = 0;
	DIR *d;

	snprintf(path, sizeof(path), "/sys/class/ieee80211/%s/device/net", name);
	if (WARN_ON(!(d = opendir(path))))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (WARN_ON(count >= *num))
			break;

		libwifi_dbg("[%s] iface  %s\n", name, p->d_name);
		entry = &iface[count];
		strncpy(entry->name, p->d_name, sizeof(entry->name));
		nlwifi_get_mode(entry->name, &entry->mode);
		count++;
	}

	closedir(d);
	*num = count;

	return 0;
}

/* common iface callbacks */

static int iface_start_wps(const char *ifname, struct wps_param wps)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_start_wps(ifname, wps);
}

static int iface_stop_wps(const char *ifname)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_stop_wps(ifname);
}

static int iface_get_wps_status(const char *ifname, enum wps_status *s)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_get_wps_status(ifname, s);
}

static int iface_get_wps_pin(const char *ifname, unsigned long *pin)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_get_wps_ap_pin(ifname, pin);
}

static int iface_set_wps_pin(const char *ifname, unsigned long pin)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_set_wps_ap_pin(ifname, pin);
}

static int iface_get_wps_device_info(const char *ifname, struct wps_device *info)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_get_caps(const char *ifname, struct wifi_caps *caps)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_ap_get_caps(ifname, caps);
}

static int iface_get_mode(const char *ifname, enum wifi_mode *mode)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_mode(ifname, mode);
}

static int iface_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	uint32_t wpa_auth;
	uint32_t wl_sec;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (WARN_ON(!auth) || WARN_ON(!enc))
		return -EINVAL;

	*auth = 0;
	*enc = 0;

	WARN_ON(bcmwl_iface_get_wsec(ifname, &wl_sec));
	WARN_ON(bcmwl_iface_get_wpa_auth(ifname, &wpa_auth));

	if (wpa_auth == WPA_AUTH_DISABLED) {
		*auth = AUTH_OPEN;
		*enc = CIPHER_NONE;
	}

	if (WSEC_WEP_ENABLED(wl_sec)) {
		*enc = CIPHER_WEP;
		*auth = AUTH_OPEN;
		return 0;
	}

	if (WSEC_TKIP_ENABLED(wl_sec)) {
		*enc |= CIPHER_TKIP;
	}

	if (WSEC_AES_ENABLED(wl_sec)) {
		*enc |= CIPHER_AES;
	}

	if (*enc ==0 && *auth == 0) {
		*enc = CIPHER_UNKNOWN;
		*auth = AUTH_UNKNOWN;
	}

	return 0;
}

static int iface_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return bcmwl_iface_add_vendor_ie(ifname, req);
}

static int iface_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return bcmwl_iface_del_vendor_ie(ifname, req);
}

static int iface_get_vendor_ies(const char *ifname, struct vendor_ie *ies, int *num_ies)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_get_param(const char *ifname, const char *param, int *len, void *val)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_set_param(const char *ifname, const char *param, int len, void *val)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_vendor_cmd(const char *ifname, uint32_t vid, uint32_t subcmd,
			    uint8_t *in, int inlen, uint8_t *out, int *outlen)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_subscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	enum wifi_mode mode;
	int ret = -1;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = iface_get_mode(ifname, &mode);
	if (WARN_ON(ret))
		return ret;

	switch (mode) {
	case WIFI_MODE_AP:
		ret = hostapd_ubus_iface_subscribe_frame(ifname, type, stype);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static int iface_unsubscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	enum wifi_mode mode;
	int ret = -1;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = iface_get_mode(ifname, &mode);
	if (WARN_ON(ret))
		return ret;

	switch (mode) {
	case WIFI_MODE_AP:
		ret = hostapd_ubus_iface_unsubscribe_frame(ifname, type, stype);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static int iface_set_4addr(const char *ifname, bool enable)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_get_4addr(const char *ifname, bool *enabled)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	return nlwifi_get_4addr(ifname, enabled);
}

static int iface_get_4addr_parent(const char *ifname, char *parent)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	return bcmwl_iface_get_4addr_parent(ifname, parent);
}

static int iface_set_vlan(const char *ifname, struct vlan_param vlan)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

int iface_link_measure(const char *ifname, uint8_t *sta)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (!sta || hwaddr_is_zero(sta)) {
		libwifi_dbg("[%s] %s invalid arg\n", ifname, __func__);
		return -1;
	}

	return bcmwl_iface_link_measure(ifname, sta);
}

/* ap interface ops */
static int iface_ap_info(const char *ifname, struct wifi_ap *ap)
{
	/* TODO
	*	ap->isolate_enabled
	*/
	struct wifi_bss *bss;
	uint32_t ch = 0;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (!is_valid_ap_iface(ifname))
		return -1;

	bss = &ap->bss;
	memset(bss, 0, sizeof(*bss));
	bcmwl_iface_ap_info(ifname, ap);
	nlwifi_get_bssid(ifname, bss->bssid);
	nlwifi_get_ssid(ifname, (char *)bss->ssid);
	radio_get_channel(ifname, &ch, &bss->curr_bw);
	bss->channel = ch;
	bcmwl_get_supp_stds(ifname, &ap->bss.supp_std);
	if (hostapd_cli_get_security_cap(ifname, &ap->sec.supp_modes))
		bcmwl_get_supported_security_const(ifname, &ap->sec.supp_modes);

	return 0;
}

static int iface_get_bssid(const char *ifname, uint8_t *bssid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_bssid(ifname, bssid);
}

static int iface_get_ssid(const char *ifname, char *ssid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_ssid(ifname, ssid);
}

static int iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	return bcmwl_iface_get_assoclist(ifname, stas, num_stas);
}

static int iface_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	libwifi_dbg("[%s] %s called " MACSTR "\n", ifname, __func__, MAC2STR(addr));

	memset(info, 0, sizeof(*info));
	memcpy(info->macaddr, addr, 6);

	WARN_ON(bcmwl_iface_get_exp_tp(ifname, addr, info));

	return bcmwl_get_sta_info(ifname, addr, info);
}

static int iface_get_sta_stats(const char *ifname, uint8_t *addr, struct wifi_sta_stats *s)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason)
{
	libwifi_dbg("[%s] %s called reason[%d]\n", ifname, __func__, reason);
	return hostapd_cli_disconnect_sta(ifname, sta, reason);
}

static int iface_restrict_sta(const char *ifname, uint8_t *sta, int enable)
{
	libwifi_dbg("[%s] %s called " MACSTR " %s\n", ifname, __func__,
		    MAC2STR(sta), enable ? "enable" : "disable");

	return bcmwl_iface_restrict_sta(ifname, sta, enable);
}

static int iface_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	libwifi_dbg("[%s] %s called " MACSTR " %s\n", ifname, __func__,
		    MAC2STR(sta), cfg->enable ? "enable" : "disable");

	return bcmwl_iface_monitor_sta(ifname, sta, cfg);
}

static int iface_probe_sta(const char *ifname, uint8_t *sta)
{
	libwifi_dbg("[%s] %s called " MACSTR "\n", ifname, __func__, MAC2STR(sta));

	return hostapd_cli_probe_sta(ifname, sta);
}

static int iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon)
{
	libwifi_dbg("[%s] %s " MACSTR " called\n", ifname, __func__, MAC2STR(sta));

	return bcmwl_iface_get_monitor_sta(ifname, sta, mon);
}

static int iface_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_add_neighbor(const char *ifname, struct nbr nbr)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_add_neighbor(ifname, &nbr, sizeof(nbr));
}

static int iface_del_neighbor(const char *ifname, unsigned char *bssid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_del_neighbor(ifname, bssid);
}

static int iface_get_neighbor_list(const char *ifname, struct nbr *nbr, int *nr)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_get_neighbor_list(ifname, nbr, nr);
}

static int iface_req_beacon_report(const char *ifname, uint8_t *sta,
				    struct wifi_beacon_req *param, size_t param_sz)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (param_sz > 0 && param != NULL)
		return hostapd_cli_iface_req_beacon(ifname, sta, param, param_sz);

	/* No params passed - use default (minimal) configuration */
	return hostapd_cli_iface_req_beacon_default(ifname, sta);
}

static int iface_get_beacon_report(const char *ifname, uint8_t *sta,
				   struct sta_nbr *snbr, int *nr)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

#define MAX_BTM_NBR_NUM 64
static int iface_req_bss_transition(const char *ifname, unsigned char *sta,
				    int bsss_nr, unsigned char *bsss, unsigned int tmo)
{
	struct bss_transition_params params = { 0 };
	struct nbr nbss[MAX_BTM_NBR_NUM] = {};
	unsigned char *bss;
	int i;

	libwifi_dbg("[%s] %s called " MACSTR " nr %d\n", ifname, __func__,
		    MAC2STR(sta), bsss_nr);

	if (bsss_nr > MAX_BTM_NBR_NUM)
		return -1;

	params.valid_int = tmo;
	params.pref = bsss_nr ? 1 : 0;
	params.disassoc_imminent = 1;

	for (i = 0; i < bsss_nr; i++) {
		bss = bsss + i * 6;
		memcpy(&nbss[i].bssid, bss, 6);
	}

	return hostapd_cli_iface_req_bss_transition(ifname, sta, bsss_nr, nbss, &params);
}

static int iface_req_btm(const char *ifname, unsigned char *sta,
			 int bsss_nr, struct nbr *bsss, struct wifi_btmreq *b)
{
	struct bss_transition_params params = { 0 };

	libwifi_dbg("[%s] %s called " MACSTR " nr %d\n", ifname, __func__,
		    MAC2STR(sta), bsss_nr);

	params.valid_int = b->validity_int;
	params.disassoc_timer = b->disassoc_tmo;

	if (b->mode & WIFI_BTMREQ_PREF_INC || bsss_nr)
		params.pref = 1;
	if (b->mode & WIFI_BTMREQ_ABRIDGED)
		params.abridged = 1;
	if (b->mode & WIFI_BTMREQ_DISASSOC_IMM)
		params.disassoc_imminent = 1;
	if (b->mode & WIFI_BTMREQ_BSSTERM_INC)
		params.bss_term = b->bssterm_dur;

	memcpy(&params.mbo, &b->mbo, sizeof(params.mbo));

	return hostapd_cli_iface_req_bss_transition(ifname, sta, bsss_nr, bsss, &params);
}

static int iface_get_11rkeys(const char *ifname, unsigned char *sta, uint8_t *r1khid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_set_11rkeys(const char *ifname, struct fbt_keys *fk)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_chan_switch(const char *ifname, struct chan_switch_param *param)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return bcmwl_iface_chan_switch(ifname, param);
}

static int iface_mbo_disallow_assoc(const char *ifname, uint8_t reason)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return bcmwl_iface_mbo_disallow_assoc(ifname, reason);
}

static int iface_ap_set_state(const char *ifname, bool up)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	return bcmwl_iface_ap_set_state(ifname, up);
}

/* sta interface ops */
static int iface_sta_info(const char *ifname, struct wifi_sta *sta)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	memset(sta, 0, sizeof(*sta));
	return bcmwl_iface_sta_info(ifname, sta);
}

static int iface_sta_get_stats(const char *ifname, struct wifi_sta_stats *s)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_sta_get_stats(ifname, s);
}

static int iface_sta_get_ap_info(const char *ifname, struct wifi_bss *info)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_sta_disconnect_ap(const char *ifname, uint32_t reason)
{
	libwifi_dbg("[%s] %s called reason %d\n", ifname, __func__, reason);
	return hostapd_cli_sta_disconnect_ap(ifname, reason);
}

/* Event wrapper, allow us to handle multiple families */
struct event_ctx {
	void *handle;
	char family[32];
};

static int register_event(const char *ifname, struct event_struct *ev,
			  void **evhandle)
{
	struct event_ctx *ctx;
	void *handle = NULL;
	int ret;

	libwifi_dbg("%s %s called family %s group %s\n", ifname, __func__, ev->family, ev->group);

	ctx = calloc(1, sizeof(struct event_ctx));
	WARN_ON(!ctx);
	if (!ctx)
		return -1;

	if (!strcmp(ev->family, "bcmwl"))
		ret = bcmwl_register_event(ifname, ev, &handle);
	else if (!strcmp(ev->family, "nl80211"))
		ret = nlwifi_register_event(ifname, ev, &handle);
	else
		ret = -1;

	strncpy(ctx->family, ev->family, sizeof(ctx->family));
	ctx->handle = handle;

	if (ret == -1) {
		free(ctx);
		ctx = NULL;
	}

	*evhandle = ctx;
	return ret;
}

static int unregister_event(const char *ifname, void *evhandle)
{
	struct event_ctx *ctx = evhandle;
	int ret;

	if (WARN_ON(!ctx))
		return -1;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (!strcmp(ctx->family, "bcmwl"))
		ret = bcmwl_unregister_event(ifname, ctx->handle);
	else if (!strcmp(ctx->family, "nl80211"))
		ret = nlwifi_unregister_event(ifname, ctx->handle);
	else
		ret = -1;

	free(ctx);
	return ret;
}

static int recv_event(const char *ifname, void *evhandle)
{
	struct event_ctx *ctx = evhandle;
	int ret;

	//libwifi_dbg("[%s] %s called %s\n", ifname, __func__, ctx->family ?: "null");

	if (!strcmp(ctx->family, "bcmwl"))
		ret = bcmwl_recv_event(ifname, ctx->handle);
	else if (!strcmp(ctx->family, "nl80211"))
		ret = nlwifi_recv_event(ifname, ctx->handle);
	else
		ret = -1;

	return ret;
}

const struct wifi_driver brcm_driver = {
	.name = "wl,phy,wds",
	.info = bcmwl_driver_info,

	/* Radio/phy callbacks */
	.radio.info = radio_info,
	.get_supp_band = radio_get_supp_band,
	.get_oper_band = radio_get_oper_band,
	.radio.get_caps = radio_get_caps,
	.radio.get_supp_stds = radio_get_supp_stds,
	.get_oper_stds = radio_get_oper_stds,

	.get_country = radio_get_country,
	.get_countrylist = radio_get_countrylist,
	.get_channel = radio_get_channel,
	.set_channel = radio_set_channel,
	.get_supp_channels = radio_get_supp_channels,
	.get_oper_channels = radio_get_oper_channels,

	.get_curr_opclass = radio_get_curr_opclass,

	.get_bandwidth = radio_get_bandwidth,
	.get_supp_bandwidths = bcmwl_radio_get_supp_bw,
	.get_maxrate = radio_get_maxrate,
	.radio.get_basic_rates = radio_get_basic_rates,
	.radio.get_oper_rates = radio_get_oper_rates,
	.radio.get_supp_rates = radio_get_supp_rates,
	.radio.get_stats = bcmwl_radio_get_stats,

	.scan = radio_scan,
	.scan_ex = radio_scan_ex,
	.get_scan_results = radio_get_scan_results,
	.get_bss_scan_result = radio_get_bss_scan_result,

	.get_noise = radio_get_noise,

	.acs = radio_acs,
	.start_cac = radio_start_cac,
	.stop_cac = radio_stop_cac,
	.get_opclass_preferences = radio_get_opclass_preferences,
	.simulate_radar = radio_simulate_radar,

	.radio.get_param = radio_get_param,
	.radio.set_param = radio_set_param,

	.add_iface = radio_add_iface,
	.del_iface = radio_del_iface,
	.list_iface = radio_list_iface,
	.channels_info = bcmwl_radio_channels_info,

	/* Interface/vif common callbacks */
	.iface.start_wps = iface_start_wps,
	.iface.stop_wps = iface_stop_wps,
	.iface.get_wps_status = iface_get_wps_status,
	.iface.get_wps_pin = iface_get_wps_pin,
	.iface.set_wps_pin = iface_set_wps_pin,
	.iface.get_wps_device_info = iface_get_wps_device_info,

	.iface.get_caps = iface_get_caps,
	.iface.get_mode = iface_get_mode,
	.iface.get_security = iface_get_security,

	.iface.add_vendor_ie = iface_add_vendor_ie,
	.iface.del_vendor_ie = iface_del_vendor_ie,
	.iface.get_vendor_ies = iface_get_vendor_ies,

	.iface.get_param = iface_get_param,
	.iface.set_param = iface_set_param,
	.vendor_cmd = iface_vendor_cmd,
	.iface.subscribe_frame = iface_subscribe_frame,
	.iface.unsubscribe_frame = iface_unsubscribe_frame,
	.set_4addr = iface_set_4addr,
	.get_4addr = iface_get_4addr,
	.get_4addr_parent = iface_get_4addr_parent,
	.set_vlan = iface_set_vlan,
	.link_measure = iface_link_measure,

        /* Interface/vif ap callbacks */
	.iface.ap_info = iface_ap_info,
	.get_bssid = iface_get_bssid,
	.get_ssid = iface_get_ssid,
	.iface.get_stats = bcmwl_iface_get_stats,
	.get_beacon_ies = bcmwl_iface_get_beacon_ies,

	.get_assoclist = iface_get_assoclist,
	.get_sta_info = iface_get_sta_info,
	.get_sta_stats = iface_get_sta_stats,
	.disconnect_sta = iface_disconnect_sta,
	.restrict_sta = iface_restrict_sta,
	.monitor_sta = iface_monitor_sta,
	.iface.probe_sta = iface_probe_sta,
	.get_monitor_sta = iface_get_monitor_sta,
	.get_monitor_stas = iface_get_monitor_stas,
	.iface.add_neighbor = iface_add_neighbor,
	.iface.del_neighbor = iface_del_neighbor,
	.iface.get_neighbor_list = iface_get_neighbor_list,
	.iface.req_beacon_report = iface_req_beacon_report,
	.iface.get_beacon_report = iface_get_beacon_report,

	.iface.req_bss_transition = iface_req_bss_transition,
	.iface.req_btm = iface_req_btm,

	.iface.get_11rkeys = iface_get_11rkeys,
	.iface.set_11rkeys = iface_set_11rkeys,

	.chan_switch = iface_chan_switch,
	.mbo_disallow_assoc = iface_mbo_disallow_assoc,
	.ap_set_state = iface_ap_set_state,

         /* Interface/vif sta callbacks */
	.iface.sta_info = iface_sta_info,
	.iface.sta_get_stats = iface_sta_get_stats,
	.iface.sta_get_ap_info = iface_sta_get_ap_info,
	.iface.sta_disconnect_ap = iface_sta_disconnect_ap,

	.radio_list = radio_list,
	.register_event = register_event,
	.unregister_event = unregister_event,
	.recv_event = recv_event,
};
