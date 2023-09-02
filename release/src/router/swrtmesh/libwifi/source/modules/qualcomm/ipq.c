/*
 * ipq.c -implements for Qualcomm drivers.
 *
 * Copyright (C) 2023 SWRTdev. All rights reserved.
 * Copyright (C) 2023 paldier <paldier@hotmail.com>.
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
#define TYPEDEF_BOOL
#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "wpactrl_util.h"
#include "debug.h"
#include "drivers.h"
#include <shared.h>
#include <qca.h>
#undef TYPEDEF_BOOL

#if __WORDSIZE == 64
#define a2uint64 atol
#else
#define a2uint64 atoll
#endif

/* Radio callbacks */
static int qca_get_bandwidth(const char *name, enum wifi_bw *bw)
{
	int bandwidth = 0, nctrlsb = 0;
	libwifi_dbg("[%s] %s called\n", name, __func__);
	get_bw_nctrlsb(name, &bandwidth, &nctrlsb);
	switch (bandwidth) {
		case 20:
			*bw = BW20;
			break;
		case 40:
			*bw = BW40;
			break;
		case 80:
			*bw = BW80;
			break;
		case 160:
			*bw = BW160;
			break;
		default:
			*bw = BW_AUTO;
			break;
	}
	return 0;
}

static int qca_get_hwaddr(const char *name, uint8_t *hwaddr)
{
	int s = -1;
	struct ifreq ifr;

	if (name == NULL)
		return -1;

	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return -1;

	strcpy(ifr.ifr_name, name);
	if (ioctl(s, SIOCGIFHWADDR, &ifr))
		goto error;

	memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
error:
	close(s);
	return 0;
}

/* common iface callbacks */
static int qca_iface_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	int unit = 0, subunit = 0;
	char prefix[64] = {0};
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (WARN_ON(!auth) || WARN_ON(!enc))
		return -1;

	*auth = 0;
	*enc = 0;
	get_wlif_unit(ifname, &unit, &subunit);
	if(!strncmp(ifname, "sta", 3))
		snprintf(prefix, sizeof(prefix), "wlc%d_", unit);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_", unit);

	if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_match(prefix, "wep_x", "0")) {
		*auth = AUTH_OPEN;
		*enc = CIPHER_NONE;
		return 0;
	}
	else if ((nvram_pf_match(prefix, "auth_mode_x", "open") && !nvram_pf_match(prefix, "wep_x", "0"))
		|| nvram_pf_match(prefix, "auth_mode_x", "shared") || nvram_pf_match(prefix, "auth_mode_x", "radius")) {
		*enc = CIPHER_WEP;
		*auth = AUTH_OPEN;
		return 0;
	}
	else if (nvram_pf_match(prefix, "auth_mode_x", "psk"))
		*auth = AUTH_WPAPSK;
	else if (nvram_pf_match(prefix, "auth_mode_x", "psk2")) 
		*auth = AUTH_WPA2PSK;
	else if (nvram_pf_match(prefix, "auth_mode_x", "pskpsk2"))
		*auth = AUTH_WPAPSK | AUTH_WPA2PSK;
	else if (nvram_pf_match(prefix, "auth_mode_x", "wpa"))
		*auth = AUTH_WPA;
	else if (nvram_pf_match(prefix, "auth_mode_x", "wpa2"))
		*auth = AUTH_WPA2;
	else if (nvram_pf_match(prefix, "auth_mode_x", "wpawpa2"))
		*auth = AUTH_WPA | AUTH_WPA2;

	if (nvram_pf_match(prefix, "crypto", "aes"))
		*enc = CIPHER_AES;
	else if (nvram_pf_match(prefix, "crypto", "tkip"))
		*enc = CIPHER_TKIP;
	else if (nvram_pf_match(prefix, "crypto", "tkip+aes"))
		*enc = CIPHER_AES | CIPHER_TKIP;

	if (*enc ==0 && *auth == 0) {
		*enc = CIPHER_UNKNOWN;
		*auth = AUTH_UNKNOWN;
	}
	return 0;
}

static int qca_iface_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	char path[256] = {0}, buf[128] = {0};
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	memset(s, 0, sizeof(*s));
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "tx_bytes");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->tx_bytes = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "rx_bytes");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->rx_bytes = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "tx_packets");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->tx_pkts = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "rx_packets");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->rx_pkts = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "tx_errors");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->tx_err_pkts = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "rx_errors");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->rx_err_pkts = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "tx_dropped");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->tx_dropped_pkts = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "rx_dropped");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->rx_dropped_pkts = a2uint64(buf);
	snprintf(path, sizeof(path), "/sys/class/net/%s/statistics/%s", ifname, "multicast");
	if(check_if_file_exist(path) && f_read_string(path, buf, sizeof(buf)) > 0)
		s->rx_mcast_pkts = a2uint64(buf);
#if 0
		s->tx_mcast_pkts = cnt->txmulti;
		s->tx_rtx_pkts = cnt->txretry;
		s->tx_rtx_fail_pkts = cnt->txfail;
		s->tx_retry_pkts = cnt->txretry;
		s->tx_mretry_pkts = cnt->txretrie;
		s->tx_ucast_pkts = cntv2->txucastpkts;
		s->rx_ucast_pkts = cntv2->rxucastpkts;
		s->tx_bcast_pkts = cntv2->txbcastpkts;
		s->rx_bcast_pkts = cntv2->rxbcastpkts;
		s->ack_fail_pkts = cntv2->txnoack;
		s->aggr_pkts = cntv2->txaggrpktcnt;
		s->tx_rtx_fail_pkts = cntv2->txretransfail;
		s->rx_unknown_pkts = cntv2->rxbadprotopkts;
#endif
	return 0;
}

int qca_simulate_radar(const char *name, struct wifi_radar_args *radar)
{
	if (WARN_ON(!radar))
		return -1;

	libwifi_dbg("[%s] %s called ch:%d, bandwidth:%d, type:0x%x, subband:0x%x\n",
		    name, __func__, radar->channel, radar->bandwidth, radar->type,
		    radar->subband_mask);
	
	return doSystem("radartool -i %s bangradar", name);;
}


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
		memset(entry->name, 0, 16);
		memcpy(entry->name, p->d_name, 15);
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

static int radio_get_iface_extch(const char *netdev, struct wifi_radio *radio)
{
	uint32_t center1;
	uint32_t freq;
	int ret;

	radio->extch = EXTCH_NONE;
	ret = nlwifi_get_bandwidth(netdev, &radio->curr_bw);
	if (ret)
		return ret;

	if (radio->curr_bw == BW20) {
		radio->extch = EXTCH_NONE;
		return 0;
	}

	if (radio->curr_bw != BW40) {
		radio->extch = EXTCH_AUTO;
		return 0;
	}

	ret = nlwifi_get_channel_center_freq1(netdev, &center1);
	if (ret)
		return ret;

	ret = nlwifi_get_channel_freq(netdev, &freq);
	if (ret)
		return ret;

	if (freq < center1) {
		radio->extch = EXTCH_ABOVE;
	} else if (freq > center1) {
		radio->extch = EXTCH_BELOW;
	}

	return 0;
}

/* Radio callbacks */
static int radio_info(const char *name, struct wifi_radio *radio)
{
	char netdev[16];
	int ret;

	libwifi_dbg("[%s] %s called\n", name, __func__);
	ret = nlwifi_radio_info(name, radio);
	if (ret)
		return ret;

	ret = nlwifi_get_supp_stds(name, &radio->supp_std);
	if (ret)
		return ret;

	ret = nlwifi_get_country(name, radio->regdomain);
	if (ret)
		return ret;

	ret = nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP);
	if (ret == 0) {
		ret = radio_get_iface_extch(netdev, radio);
		ret |= hostapd_cli_get_oper_stds(netdev, &radio->oper_std);
		return ret;
	}

	ret = nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_STA);
	if (ret == 0) {
		ret = radio_get_iface_extch(netdev, radio);
		ret |= supplicant_cli_get_oper_std(netdev, &radio->oper_std);
		return ret;
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
	return nlwifi_get_oper_band(name, band);
}

static int radio_get_caps(const char *name, struct wifi_caps *caps)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_radio_get_caps(name, caps);
}

static int radio_get_supp_stds(const char *name, uint8_t *std)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_supp_stds(name, std);
}

static int radio_get_oper_stds(const char *name, uint8_t *std)
{
	char netdev[16];
	int ret;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP);
	if (ret == 0) {
		ret = hostapd_cli_get_oper_stds(netdev, std);
	} else {
		ret = nlwifi_get_supp_stds(name, std);
	}

	return ret;
}

static int radio_get_country(const char *name, char *alpha2)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_country(name, alpha2);
}

static int radio_get_channel(const char *name, uint32_t *channel, enum wifi_bw *bw)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_channel(name, channel, bw);
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


static int radio_get_maxrate(const char *name, unsigned long *rate_Mbps)
{
	struct wifi_radio radio = { 0 };
	char netdev[16];
	int ret = 0;
	int max_mcs = 0;
	enum wifi_guard sgi = WIFI_SGI;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = nlwifi_get_phy_info(name, &radio);
	if (WARN_ON(ret))
		return ret;

	if (nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP) == 0) {
		ret = hostapd_cli_get_oper_stds(netdev, &radio.oper_std);
	} else if(nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_STA) == 0) {
		ret = supplicant_cli_get_oper_std(netdev, &radio.oper_std);
	}

	if (ret) {
		ret = nlwifi_get_supp_stds(name, &radio.oper_std);
		if (WARN_ON(ret))
			return ret;
	}

	if (radio.oper_band & BAND_2) max_mcs = 7;
	if (radio.oper_band & BAND_5) max_mcs = 9;
	if (radio.oper_std & WIFI_AX) {
		sgi = WIFI_4xLTF_GI800;
		max_mcs = 11;
	}

	ret = nlwifi_get_bandwidth(name, &radio.curr_bw);
	libwifi_dbg("[%s] %s called curr_bw:0x%x\n", name, __func__, radio.curr_bw);

	if (ret) {
		ret = nlwifi_get_max_bandwidth(name, &radio.curr_bw);
		libwifi_dbg("[%s] %s called max_bw:0x%x\n", name, __func__, radio.curr_bw);
	}

	libwifi_dbg("%s called max_mcs=%d, bw=%d, nss=%d sgi=%d \n", __func__, max_mcs, wifi_bw_enum2MHz(radio.curr_bw), radio.rx_streams, sgi);

	*rate_Mbps = wifi_mcs2rate(max_mcs, wifi_bw_enum2MHz(radio.curr_bw), radio.rx_streams, sgi);

	return ret;
}

static int radio_get_basic_rates(const char *name, int *num, uint32_t *rates_kbps)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_oper_rates(const char *name, int *num, uint32_t *rates_kbps)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_supp_rates(const char *name, int *num, uint32_t *rates)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_stats(const char *name, struct wifi_radio_stats *s)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_scan(const char *name, struct scan_param *p)
{
	char netdev[16];

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);
	return nlwifi_scan(netdev, p);
}

int radio_scan_ex(const char *name, struct scan_param_ex *sp)
{
	char netdev[16];

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s, %s] %s called\n", name, netdev, __func__);

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
	if (nlwifi_get_noise(netdev, noise))
		/* TODO - for 7615 upgrade backports/mt76 */
		*noise = -90;

	return 0;
}

static int radio_acs(const char *name, struct acs_param *p)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_start_cac(const char *name, int channel, enum wifi_bw bw,
			   enum wifi_cac_method method)
{
	char netdev[16] = {};
	libwifi_dbg("[%s] %s called\n", name, __func__);

	WARN_ON(nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP));
	return nlwifi_start_cac(netdev, channel, bw, method);
}

static int radio_stop_cac(const char *name)
{
	char netdev[16] = {};
	libwifi_dbg("[%s] %s called\n", name, __func__);

	WARN_ON(nlwifi_phy_to_netdev_with_type(name, netdev, sizeof(netdev), NLWIFI_MODE_AP));
	return nlwifi_stop_cac(netdev);
}

static int radio_get_opclass_preferences(const char *name,
					 struct wifi_opclass *opclass,
					 int *num)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return wifi_get_opclass_pref(name, num, opclass);
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
		memset(entry->name, 0, 16);
		memcpy(entry->name, p->d_name, 15);
		nlwifi_get_mode(entry->name, &entry->mode);
		count++;
	}

	closedir(d);
	*num = count;

	return 0;
}

static int radio_channels_info(const char *name, struct chan_entry *channel, int *num)
{
	char cc[3] = {0};
	int ret;
	int i;

	ret = nlwifi_channels_info(name, channel, num);
	if (ret)
		return ret;

	WARN_ON(nlwifi_get_country(name, cc));
	if (!strcmp(cc, "US") || !strcmp(cc, "JP"))	/* FIXME-CR: other non-ETSI countries */
		return 0;

	/* Check weather channels - just in case of regulatory issue */
	for (i = 0; i < *num; i++) {
		switch (channel[i].channel) {
		case 120:
		case 124:
		case 128:
			channel[i].cac_time = 600;
			break;
		default:
			break;
		}
	}

	return ret;
}

static int radio_get_ifstatus(const char *name, ifstatus_t *f)
{
	char path[256];
	struct dirent *p;
	DIR *d;
	ifstatus_t ifstatus;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (get_ifstatus(name, f)) {
		snprintf(path, sizeof(path), "/sys/class/ieee80211/%s/device/net", name);

		if (WARN_ON(!(d = opendir(path))))
			return -1;

		while ((p = readdir(d))) {
			if (!strcmp(p->d_name, "."))
				continue;
			if (!strcmp(p->d_name, ".."))
				continue;

			if (!get_ifstatus(p->d_name, &ifstatus)) {
				if (ifstatus & IFF_UP) {
					*f |= IFF_UP;
					break;
				}
			}
		}

		closedir(d);
	}

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

static int iface_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_add_vendor_ie(ifname, req);
}

static int iface_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_del_vendor_ie(ifname, req);
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
	return nlwifi_vendor_cmd(ifname, vid, subcmd, in, inlen, out, outlen);
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

static int iface_get_4addr(const char *ifname, bool *enable)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_4addr(ifname, enable);
}

static int iface_get_4addr_parent(const char *ifname, char *parent)
{
	enum wifi_mode mode;
	int ret;
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = nlwifi_get_mode(ifname, &mode);
	if (ret)
		return ret;

	if (mode != WIFI_MODE_AP_VLAN)
		return -1;

	ret = hostapd_cli_get_4addr_parent((char *)ifname, parent);
	return ret;
}

static int iface_set_vlan(const char *ifname, struct vlan_param vlan)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
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
	hostapd_cli_iface_ap_info(ifname, ap);
	nlwifi_get_bssid(ifname, bss->bssid);
	nlwifi_get_ssid(ifname, (char *)bss->ssid);
	if (!strlen((char *)bss->ssid))
		ap->enabled = false;

	nlwifi_get_channel(ifname, &ch, &bss->curr_bw);
	bss->channel = (uint8_t)ch;
	nlwifi_get_bandwidth(ifname, &bss->curr_bw);
	nlwifi_get_supp_stds(ifname, &ap->bss.supp_std);
	hostapd_cli_get_security_cap(ifname, &ap->sec.supp_modes);

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

static int iface_get_beacon_ies(const char *ifname, uint8_t *ies, int *len)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_get_beacon_ies(ifname, ies, (size_t *)len);
}

static int iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (is_valid_ap_iface(ifname)) {
		return hostapd_iface_get_assoclist(ifname, stas, num_stas);
	}
	return -1;
}

static int iface_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	const char *iface;
	char ifname_wds[256] = { 0 };
	int ret = 0;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	if (hostapd_cli_is_wds_sta(ifname, addr, ifname_wds, sizeof(ifname_wds))) {
		libwifi_dbg("[%s] has virtual AP iface[%s]\n", ifname, ifname_wds);
		iface = ifname_wds;
	} else {
		iface = ifname;
	}
	ret = nlwifi_get_sta_info(iface, addr, info);
	if (!ret) {
		ret = hostapd_cli_iface_get_sta_info(ifname, addr, info);
	}

	if (!ret) {
		ret = radio_get_maxrate(ifname, (unsigned long *) &info->maxrate);
	}

	return ret;
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
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int iface_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	libwifi_dbg("[%s] %s called " MACSTR " %s\n", ifname, __func__,
		    MAC2STR(sta), cfg->enable ? "enable" : "disable");

	/* Enable event driven probe req monitoring */
	return hostapd_ubus_iface_monitor_sta(ifname, sta, cfg);
}

static int iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon)
{
	struct wifi_sta info = {};
	int ret;
	int i;

	libwifi_dbg("[%s] %s " MACSTR " called\n", ifname, __func__, MAC2STR(sta));

	memset(mon, 0, sizeof(*mon));

	/* Get RSSI from nlwifi (from data frames) for connected stations */
	ret = nlwifi_get_sta_info(ifname, sta, &info);
	if (WARN_ON(ret))
		return ret;

	/* Check if connected */
	if (memcmp(mon->macaddr, info.macaddr, 6)) {
		WARN_ON(sizeof(mon->rssi) != sizeof(info.rssi));
		for (i = 0; i < sizeof(mon->rssi); i++)
			mon->rssi[i] = info.rssi[i];

		mon->rssi_avg = info.rssi_avg;
		mon->last_seen = info.idle_time;
		memcpy(mon->macaddr, info.macaddr, 6);
		return ret;
	}

	/* Worst case for not connected station - get from hostapd (probes) */
	return hostapd_ubus_iface_get_monitor_sta(ifname, sta, mon);
}

static int iface_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num)
{
	struct wifi_monsta *mon;
	uint8_t sta[6];
	int ret;
	int i;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = hostapd_ubus_iface_get_monitor_stas(ifname, stas, num);
	if (WARN_ON(ret))
		return ret;

	for (i = 0; i < *num; i++) {
		mon = &stas[i];
		memcpy(sta, mon->macaddr, sizeof(sta));
		WARN_ON(iface_get_monitor_sta(ifname, sta, mon));
	}

	return ret;
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

	params.valid_int = (uint8_t)tmo;
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


static int get_sec_chan_offset(uint32_t freq)
{
	int ht40plus[] = { 5180, 5220, 5260, 5300, 5500, 5540, 5580, 5620, 5660, 5745, 5785, 5920 };
	int i;

	for (i = 0; i < ARRAY_SIZE(ht40plus); i++) {
		if (freq == ht40plus[i])
			return 1;
	}

	return -1;
}

static uint32_t get_cf1(uint32_t freq, uint32_t bw)
{
	uint32_t cf1 = 0;
	int cf1_bw80[] = { 5210, 5290, 5530, 5610, 5690, 5775 };
	int cf1_bw160[] = { 5250, 5570 };
	int i;

	/* TODO check/fix 6GHz */
	switch (bw) {
	case 160:
		for (i = 0; i < ARRAY_SIZE(cf1_bw160); i++) {
			if (freq >= cf1_bw160[i] - 70 &&
			    freq <= cf1_bw160[i] + 70) {
				cf1 = cf1_bw160[i];
				break;
			}
		}
		break;
	case 80:
		for (i = 0; i < ARRAY_SIZE(cf1_bw80); i++) {
			if (freq >= cf1_bw80[i] - 30 &&
			    freq <= cf1_bw80[i] + 30) {
				cf1 = cf1_bw80[i];
				break;
			}
		}
		break;

	default:
		cf1 = freq;
	}

	return cf1;
}

static int iface_chan_switch(const char *ifname, struct chan_switch_param *param)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	switch (param->bandwidth) {
	case 160:
	case 80:
		if (!param->sec_chan_offset)
			param->sec_chan_offset = get_sec_chan_offset(param->freq);
		if (!param->cf1)
			param->cf1 = get_cf1(param->freq, param->bandwidth);
		break;
	case 8080:
		if (WARN_ON(!param->cf1))
			return -1;
		if (WARN_ON(!param->cf2))
			return -1;
		break;
	case 40:
		if (!WARN_ON(!param->sec_chan_offset))
			return -1;
		break;
	default:
		break;
	}

	return hostapd_cli_iface_chan_switch(ifname, param);
}

static int iface_mbo_disallow_assoc(const char *ifname, uint8_t reason)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_mbo_disallow_assoc(ifname, reason);
}

static int iface_ap_set_state(const char *ifname, bool up)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	return hostapd_cli_ap_set_state(ifname, up);
}

/* sta interface ops */
static int iface_sta_info(const char *ifname, struct wifi_sta *sta)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	int ret = 0;
	ret = nlwifi_sta_info(ifname, sta);
	if(ret) return -1;

	WARN_ON(if_gethwaddr(ifname, sta->macaddr));
	ret = supplicant_sta_info(ifname, sta);

	return ret;
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




const struct wifi_driver qca_driver = {
	.name = "ath,wifi,sta,phy",
	.info = nlwifi_driver_info,

	/* Radio/phy callbacks */
	.radio.info = radio_info,
	.get_supp_band = radio_get_supp_band,
	.get_oper_band = radio_get_oper_band,
	.radio.get_ifstatus = radio_get_ifstatus,
	.radio.get_caps = radio_get_caps,
	.radio.get_supp_stds = radio_get_supp_stds,
	.get_oper_stds = radio_get_oper_stds,

	.get_country = radio_get_country,
	.get_channel = radio_get_channel,
	.set_channel = radio_set_channel,
	.get_supp_channels = radio_get_supp_channels,
	.get_oper_channels = radio_get_oper_channels,

	.get_curr_opclass = radio_get_curr_opclass,

	.get_bandwidth = qca_get_bandwidth,
	.get_supp_bandwidths = nlwifi_get_supp_bandwidths,
	.get_maxrate = radio_get_maxrate,
	.radio.get_basic_rates = radio_get_basic_rates,
	.radio.get_oper_rates = radio_get_oper_rates,
	.radio.get_supp_rates = radio_get_supp_rates,
	.radio.get_stats = radio_get_stats,

	.scan = radio_scan,
	.scan_ex = radio_scan_ex,
	.get_scan_results = radio_get_scan_results,
	.get_bss_scan_result = radio_get_bss_scan_result,

	.get_noise = radio_get_noise,

	.acs = radio_acs,
	.start_cac = radio_start_cac,
	.stop_cac = radio_stop_cac,
	.get_opclass_preferences = radio_get_opclass_preferences,
	.simulate_radar = qca_simulate_radar,

	.radio.get_param = radio_get_param,
	.radio.set_param = radio_set_param,

	.radio.get_hwaddr = qca_get_hwaddr,

	.add_iface = radio_add_iface,
	.del_iface = radio_del_iface,
	.list_iface = radio_list_iface,
	.channels_info = radio_channels_info,

	/* Interface/vif common callbacks */
	.iface.start_wps = iface_start_wps,
	.iface.stop_wps = iface_stop_wps,
	.iface.get_wps_status = iface_get_wps_status,
	.iface.get_wps_pin = iface_get_wps_pin,
	.iface.set_wps_pin = iface_set_wps_pin,
	.iface.get_wps_device_info = iface_get_wps_device_info,

	.iface.get_caps = iface_get_caps,
	.iface.get_mode = iface_get_mode,
	.iface.get_security = qca_iface_get_security,

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

        /* Interface/vif ap callbacks */
	.iface.ap_info = iface_ap_info,
	.get_bssid = iface_get_bssid,
	.get_ssid = iface_get_ssid,
	.iface.get_stats = qca_iface_get_stats,
	.get_beacon_ies = iface_get_beacon_ies,

	.get_assoclist = iface_get_assoclist,
	.get_sta_info = iface_get_sta_info,
	.get_sta_stats = iface_get_sta_stats,
	.disconnect_sta = iface_disconnect_sta,
	.restrict_sta = iface_restrict_sta,
	.monitor_sta = iface_monitor_sta,
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
	.register_event = nlwifi_register_event,
	.unregister_event = nlwifi_unregister_event,
	.recv_event = nlwifi_recv_event,
};
