/*
 * mtwifi.c - implements for mtwifi drivers.
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
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#define TYPEDEF_BOOL
#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "wpactrl_util.h"
#include "debug.h"
#include "drivers.h"
#include <shared.h>
#include <ralink.h>
#undef TYPEDEF_BOOL
#include "mtwifi.h"

static int radio_list(struct radio_entry *radio, int *num)
{
	struct dirent *p;
	struct radio_entry *entry;
	int count = 0;
	DIR *d;

	if (WARN_ON(!(d = opendir("/sys/class/net"))))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (WARN_ON(count >= *num))
			break;
		if(strncmp(p->d_name, "ra", 2) && strncmp(p->d_name, "apcli", 5))
			continue;

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
//	int ret;
//	enum wifi_mode mode;
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
#if 0
	ret = nlwifi_get_mode(ifname, &mode);
	if (WARN_ON(ret))
		return 0;

	if (mode == WIFI_MODE_AP)  {
		return 1;
	}
#else
	if(!strncmp(ifname, "ra", 2))// ra0 rai0 rax0 rae0
		return 1;
#endif

	return 0;
}


#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
void PartialScanNumOfCh(const char *ifname)
{
	int lock;
	char data[64];
	struct iwreq wrq;

	memset(data, 0, sizeof(data));
	strcpy(data, "PartialScanNumOfCh=3");
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	lock = file_lock("sitesurvey");
	if(wl_ioctl(ifname, RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);
		dbg("Site Survey fails\n");
		return;
	}
	file_unlock(lock);
}
#endif

#if 0
static int getSiteSurveyVSIEcount(const char *ifname)
{
	char data[64];
	struct iwreq wrq;
	memset(data, 0, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = ASUS_SUBCMD_GETSITESURVEY_VSIE_COUNT;

	if(wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0){
		dbg("errors in getting site survey vie result\n");
	}else{
    	sleep(2);
    	return atoi(data);
	}
	return 0;
}

static int ieee80211_frequency_to_channel(int freq)
{
	/* see 802.11-2007 17.3.8.3.2 and Annex J */
	if (freq == 0)
		return 0;
	else if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq >= 5000 && freq < 5900)
		return (freq - 5000) / 5;
	else if (freq > 5950 && freq <= 7115)
		return (freq - 5950) / 5;
	else if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;
	else
		return 0;
}

static int radio_get_phy_info(const char *name, struct wifi_radio *radio)
{
	char prefix[64];
	if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3) || !strncmp(name, "apclii", 6) || !strncmp(name, "apclix", 6)){
		radio->supp_band |= BAND_5;
		radio->oper_band = BAND_5;
	}else if(!strncmp(name, "rae", 3) || !strncmp(name, "apclie", 6)){
#if defined(RTCONFIG_WIFI6E)
		radio->supp_band |= BAND_6;
		radio->oper_band = BAND_6;
#else
		radio->supp_band |= BAND_5;
		radio->oper_band = BAND_5;
#endif
	}else{
		radio->supp_band |= BAND_2;
		radio->oper_band = BAND_2;
	}
	if(radio->oper_band & BAND_2)
		radio->supp_std |= WIFI_G | WIFI_B | WIFI_N;
	if(radio->oper_band & BAND_5)
		radio->supp_std |= WIFI_A | WIFI_N | WIFI_AC;
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if(radio->oper_band & BAND_2)
		radio->supp_std |= WIFI_AX;
	if(radio->oper_band & BAND_5)
		radio->supp_std |= WIFI_AX;
	if(radio->oper_band & BAND_6)
		radio->supp_std |= WIFI_AX;
#endif
#if defined(RTCONFIG_SOC_MT7988)
	if(radio->oper_band & BAND_2)
		radio->supp_std |= WIFI_BE;
	if(radio->oper_band & BAND_5)
		radio->supp_std |= WIFI_BE;
	if(radio->oper_band & BAND_6)
		radio->supp_std |= WIFI_BE;
#endif

	radio->supp_bw |= BIT(BW20);
	radio->supp_bw |= BIT(BW40);
	if(radio->oper_band & BAND_5){
		radio->supp_bw |= BIT(BW80);
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_MT798X)
		radio->supp_bw |= BIT(BW160);
		radio->supp_bw |= BIT(BW8080);
#endif
	}

	if (0 == radio->supp_bw)
		radio->supp_bw |= BIT(BW_UNKNOWN);
	libwifi_dbg(" - oper band %d\n", radio->oper_band);
	libwifi_dbg(" - supp band 0x%x\n", radio->supp_band);
	if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3) || !strncmp(name, "apclii", 6) || !strncmp(name, "apclix", 6))
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	else if(!strncmp(name, "rae", 3) || !strncmp(name, "apclie", 6))
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	radio->frag = nvram_pf_get_int(prefix, "frag");
	radio->rts = nvram_pf_get_int(prefix, "rts");
	radio->srl = nvram_pf_get_int(prefix, "srl");
	radio->lrl = nvram_pf_get_int(prefix, "lrl");
	radio->beacon_int = nvram_pf_get_int(prefix, "bcn");
	radio->dtim_period = nvram_pf_get_int(prefix, "dtim");
	radio->tx_streams = nvram_pf_get_int(prefix, "HT_TxStream");
	radio->rx_streams = nvram_pf_get_int(prefix, "HT_RxStream");
#if defined(RTCONFIG_WLMODULE_MT7603E_AP)
	if(!strcmp(name, "ra0") || !strcmp(name, "ra1")|| !strcmp(name, "ra2") || !strcmp(name, "apcli0"))
		radio->cac_methods = 0;
	else
#endif
	if(radio->tx_streams == 4 && radio->rx_streams == 4)
		radio->cac_methods |= BIT(WIFI_CAC_MIMO_REDUCED);
	else
		radio->cac_methods |= BIT(WIFI_CAC_CONTINUOUS);

	return 0;
}

static int radio_radio_info2(const char *name, struct wifi_radio *radio)
{
	int idx = 0, i;
	char prefix[64], tmp[16];
	CHANNEL_INFO s = {0};
	const char *ifname;
	struct iwreq wrq;
	libwifi_dbg("[%s] %s called\n", name, __func__);

	radio_get_phy_info(name, radio);

	ifname = name;
	wrq.u.data.length = sizeof(uint64_t);
	wrq.u.data.pointer = (caddr_t)&s;
	wrq.u.data.flags = OID_802_11_CURRENT_CHANNEL_INFO;
	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
		dbg("errors in getting %s CHANNEL_INFO\n", ifname);
		return -1;
	}

	radio->channel = s.channel;
	radio->diag.cca_time = s.actual_measured_time * 1000;
	radio->diag.channel_busy = s.channel_busy_time * 1000;
	radio->diag.tx_airtime = s.tx_time * 1000;
	radio->diag.rx_airtime = s.rx_time * 1000;
	radio->num_iface = WIFI_IFACE_MAX_NUM;
	if(!strcmp(name, "ra0") || !strcmp(name, "ra1")|| !strcmp(name, "ra2") || !strcmp(name, "apcli0")){
		strlcpy(radio->iface[idx].name, "ra0", 15);
		radio->iface[idx].mode = WIFI_MODE_AP;
		for(i = 0; i < MAX_NO_MSSID; i++){
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", 0, i);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				idx++;
				snprintf(tmp, sizeof(tmp), "ra%d", idx);
				strlcpy(radio->iface[idx].name, tmp, 15);
				radio->iface[idx].mode = WIFI_MODE_AP;
			}
		}
		idx++;
		strlcpy(radio->iface[idx].name, "apcli0", 15);
		radio->iface[idx].mode = WIFI_MODE_STA;
	}else if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3) || !strncmp(name, "apclii", 6) || !strncmp(name, "apclix", 6)){
		if(!strncmp(name, "rax", 3) || !strncmp(name, "apclix", 6))
			strlcpy(radio->iface[idx].name, "rax0", 15);
		else
			strlcpy(radio->iface[idx].name, "rai0", 15);
		radio->iface[idx].mode = WIFI_MODE_AP;
		for(i = 0; i < MAX_NO_MSSID; i++){
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", 1, i);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				idx++;
				if(!strncmp(name, "rax", 3) || !strncmp(name, "apclix", 6))
					snprintf(tmp, sizeof(tmp), "rax%d", idx);
				else
					snprintf(tmp, sizeof(tmp), "rai%d", idx);
				strlcpy(radio->iface[idx].name, tmp, 15);
				radio->iface[idx].mode = WIFI_MODE_AP;
			}
		}
		idx++;
		if(!strncmp(name, "rax", 3) || !strncmp(name, "apclix", 6))
			strlcpy(radio->iface[idx].name, "apclix0", 15);
		else
			strlcpy(radio->iface[idx].name, "apclii0", 15);
		radio->iface[idx].mode = WIFI_MODE_STA;
	}else if(!strncmp(name, "rae", 3) || !strncmp(name, "apclie", 6)){
		strlcpy(radio->iface[idx].name, "rae0", 15);
		radio->iface[idx].mode = WIFI_MODE_AP;
		for(i = 0; i < MAX_NO_MSSID; i++){
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", 1, i);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				idx++;
				snprintf(tmp, sizeof(tmp), "rae%d", idx);
				strlcpy(radio->iface[idx].name, tmp, 15);
				radio->iface[idx].mode = WIFI_MODE_AP;
			}
		}
		idx++;
		strlcpy(radio->iface[idx].name, "apclie0", 15);
		radio->iface[idx].mode = WIFI_MODE_STA;
	}
	radio->num_iface = idx;

	if (radio->oper_band != BAND_2)
		/* TODO get it from iface combinations */
		radio->cac_methods |= BIT(WIFI_CAC_CONTINUOUS);
	else
		/* Don't report CAC methods for 2.4GHz */
		radio->cac_methods = 0;

	return 0;
}

static int radio_radio_info(const char *name, struct wifi_radio *radio)
{
	uint8_t std = 0;
	int ret;

	ret = radio_radio_info2(name, radio);

	ret |= radio_get_oper_stds(name, &std);
	if (!ret)
		radio->oper_std = std;

	if (radio->beacon_int == 0) {
#if defined(RTCONFIG_WLMODULE_MT7603E_AP)
		if(!strcmp(name, "ra0") || !strcmp(name, "ra1")|| !strcmp(name, "ra2") || !strcmp(name, "apcli0"))
			radio->beacon_int = 0;
		else
#endif
		{
			uint64_t s = 0;
			const char *ifname;
			struct iwreq wrq;
			ifname = name;
			wrq.u.data.length = sizeof(uint64_t);
			wrq.u.data.pointer = (uint64_t *)&s;
			wrq.u.data.flags = OID_802_11_BEACON_PERIOD;
			if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
				dbg("errors in getting %s dtim period\n", ifname);
				return -1;
			}
			radio->beacon_int = (int)s;
		}
	}

	if (radio->dtim_period == 0) {
#if defined(RTCONFIG_MT798X)
		const char *ifname;
		int s = 0;
		struct iwreq wrq;
		ifname = name;
		wrq.u.data.length = sizeof(uint8_t);
		wrq.u.data.pointer = (uint8_t *)&s;
		wrq.u.data.flags = OID_GET_DTIM_PERIOD;
		if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
			dbg("errors in getting %s dtim period\n", ifname);
			return -1;
		}
		radio->dtim_period = s;
#else
		radio->dtim_period = 1;
#endif
	}

	return ret;
}
#endif
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

static void correct_oper_std_by_band(enum wifi_band band, uint8_t *std)
{
	switch (band) {
	case BAND_2:
		*std &= ~(WIFI_A | WIFI_AC);
		break;
	case BAND_5:
		if (*std & WIFI_G)
			*std |= WIFI_A;
		*std &= ~(WIFI_G | WIFI_B);
		break;
	case BAND_6:
		*std &= ~(WIFI_B | WIFI_G | WIFI_A | WIFI_AC);
		break;
	default:
		break;
	}
}

/* Radio callbacks */
static int radio_info_band(const char *name, enum wifi_band band, struct wifi_radio *radio)
{
	char netdev[16];
	int ret;

	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);
	ret = nlwifi_radio_info_band(name, band, radio);
	if (ret)
		return ret;

	ret = nlwifi_get_band_supp_stds(name, band, &radio->supp_std);
	if (ret)
		return ret;

	ret = nlwifi_get_country(name, radio->regdomain);
	if (ret)
		return ret;

//	ret = nlwifi_phy_to_netdev_with_type_and_band(name, netdev, sizeof(netdev), NLWIFI_MODE_AP, band);
	ret = 0;
	strlcpy(netdev, name, sizeof(netdev));
	if (ret == 0) {
		ret = radio_get_iface_extch(netdev, radio);
		ret |= hostapd_cli_get_oper_stds(netdev, &radio->oper_std);
		correct_oper_std_by_band(band, &radio->oper_std);

		return ret;
	}

	ret = nlwifi_phy_to_netdev_with_type_and_band(name, netdev, sizeof(netdev), NLWIFI_MODE_STA, band);
	if (ret == 0) {
		ret = radio_get_iface_extch(netdev, radio);
		ret |= supplicant_cli_get_oper_std(netdev, &radio->oper_std);
		correct_oper_std_by_band(band, &radio->oper_std);

		return ret;
	}

	radio->oper_std = radio->supp_std;
	return 0;
}

int mtwifi_driver_info(const char *name, struct wifi_metainfo *info)
{
	char path[256] = {};
	char *pos;
	int fd;

	libwifi_dbg("%s %s called\n", name, __func__);

	snprintf(path, sizeof(path), "/sys/class/pci_bus/0000:00/device/0000:00:00.0/vendor");
	fd = open(path, O_RDONLY);
	if (WARN_ON(fd < 0))
		return -1;

	if (WARN_ON(!read(fd, info->vendor_id, sizeof(info->vendor_id)))) {
		close(fd);
		return -1;
	}
	if ((pos = strchr(info->vendor_id, '\n')))
		*pos = '\0';
	close(fd);

	snprintf(path, sizeof(path), "/sys/class/pci_bus/0000:00/device/0000:00:00.0/device");
	fd = open(path, O_RDONLY);
	if (WARN_ON(fd < 0))
		return -1;

	if (WARN_ON(!read(fd, info->device_id, sizeof(info->device_id)))) {
		close(fd);
		return -1;
	}
	if ((pos = strchr(info->device_id, '\n')))
		*pos = '\0';
	close(fd);

	return 0;
}

static int radio_info(const char *name, struct wifi_radio *radio)
{
	return radio_info_band(name, BAND_ANY, radio);
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

static int radio_get_band_oper_stds(const char *name, enum wifi_band band, uint8_t *std)
{
#if 0
	char netdev[16];
	int ret;

	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);

	ret = nlwifi_phy_to_netdev_with_type_and_band(name, netdev, sizeof(netdev), NLWIFI_MODE_AP, band);
	if (ret == 0) {
		ret = hostapd_cli_get_oper_stds(netdev, std);
		correct_oper_std_by_band(band, std);
	} else {
		ret = nlwifi_get_supp_stds(name, std);
		correct_oper_std_by_band(band, std);
	}

	return ret;
#else
	const char *ifname;
	unsigned long phy_mode = 0;
	struct iwreq wrq;
	ifname = name;
	wrq.u.data.length = sizeof(unsigned long);
	wrq.u.data.pointer = (caddr_t) &phy_mode;
	wrq.u.data.flags = RT_OID_GET_PHY_MODE;

	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0)
		return -1;

	*std = wmode2std[phy_mode];
	return 0;
#endif
}

static int radio_get_oper_stds(const char *name, uint8_t *std)
{
	return radio_get_band_oper_stds(name, BAND_ANY, std);
}

static int radio_get_country(const char *name, char *alpha2)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_country(name, alpha2);
}

static int radio_get_channel(const char *name, uint32_t *channel, enum wifi_bw *bw)
{
	struct iwreq wrq;
	struct channel_info info;
	*channel = 0;
	*bw = 0;
	memset(&info, 0, sizeof(struct channel_info));
	wrq.u.data.length = sizeof(struct channel_info);
	wrq.u.data.pointer = (caddr_t) &info;
	wrq.u.data.flags = ASUS_SUBCMD_GCHANNELINFO;

	if (wl_ioctl(name, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0) {
		dbg("wl_ioctl failed on %s (%d)\n", __FUNCTION__, __LINE__);
		return -1;
	}

	*channel = (int)info.channel;

	switch (info.bandwidth) {
		case 0:
			*bw = BW20;
			break;
		case 1:
			*bw = BW40;
			break;
		case 2:
			*bw = BW80;
			break;
		case 3:
			*bw = BW160;
			break;
		default:
			*bw = BW_AUTO;
			break;
	}
	return 0;
}

static int radio_set_channel(const char *name, uint32_t channel, enum wifi_bw bw)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_supp_channels(const char *name, uint32_t *chlist, int *num,
				   const char *alpha2, enum wifi_band f, enum wifi_bw b)
{
	int unit = 0, band;
	char chList[256], prefix[64], chan[6], *next;
	char *country_code;
	if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3)){
		unit = 1;
		band = BAND_5;
	}else if(!strncmp(name, "rae", 3)){
		unit = 2;
#if defined(RTCONFIG_WIFI6E)
		band = BAND_6;
#else
		band = BAND_5;
#endif
	}else{
		unit = 0;
		band = BAND_2;
	}
	*num = 0;
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	country_code = nvram_pf_safe_get(prefix, "country_code");
	if(get_channel_list_via_driver(unit, chList, sizeof(chList)) <= 0)
	{
		*num = 0;
		return -1;
	}
	else if(get_channel_list_via_country(unit, country_code, chList, sizeof(chList)) <= 0)
	{
		*num = 0;
		return -1;
	}
	foreach_44(chan, chList, next){
		chlist[*num] = (uint32_t)strtol(chan, NULL, 10);
		*num += 1;
	}
	libwifi_dbg("[%s] %s called (max %d band %d)\n", name, __func__, *num, band);
	return 0;
}

static int radio_get_oper_channels(const char *name, uint32_t *chlist, int *num,
				   const char *alpha2, enum wifi_band f, enum wifi_bw b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_band_curr_opclass(const char *name, enum wifi_band band, struct wifi_opclass *o)
{
	char netdev[16];

//	nlwifi_phy_to_netdev_with_band(name, netdev, sizeof(netdev), band);
	strlcpy(netdev, name, sizeof(netdev));
	libwifi_dbg("[%s, %s, %s] %s called\n", name, netdev, wifi_band_to_str(band), __func__);
	return wifi_get_band_opclass(netdev, band, o);;
}

static int radio_get_curr_opclass(const char *name, struct wifi_opclass *o)
{
	libwifi_dbg("[%s, %s] %s called\n", name, __func__);
	return wifi_get_opclass(name, o);
}

static int radio_get_bandwidth(const char *ifname, enum wifi_bw *bw)
{
	struct iwreq wrq;
	struct channel_info info;
	*bw = 0;
	memset(&info, 0, sizeof(struct channel_info));
	wrq.u.data.length = sizeof(struct channel_info);
	wrq.u.data.pointer = (caddr_t) &info;
	wrq.u.data.flags = ASUS_SUBCMD_GCHANNELINFO;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0) {
		dbg("wl_ioctl failed on %s (%d)\n", __FUNCTION__, __LINE__);
		return -1;
	}

	switch (info.bandwidth) {
		case 0:
			*bw = BW20;
			break;
		case 1:
			*bw = BW40;
			break;
		case 2:
			*bw = BW80;
			break;
		case 3:
			*bw = BW160;
			break;
		default:
			*bw = BW_UNKNOWN;
			break;
	}
	return 0;
}

static int radio_get_band_maxrate(const char *name, enum wifi_band band, unsigned long *rate_Mbps)
{
#if 0
	struct wifi_radio radio = { 0 };
	char netdev[16];
	int ret = 0;
	int max_mcs = 0;
	enum wifi_guard sgi = WIFI_SGI;

	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);

	ret = nlwifi_get_phy_info(name, band, &radio);
	if (WARN_ON(ret))
		return ret;

	if (nlwifi_phy_to_netdev_with_type_and_band(name, netdev, sizeof(netdev), NLWIFI_MODE_AP, band) == 0) {
		ret = hostapd_cli_get_oper_stds(netdev, &radio.oper_std);
		correct_oper_std_by_band(band, &radio.oper_std);
	} else if(nlwifi_phy_to_netdev_with_type_and_band(name, netdev, sizeof(netdev), NLWIFI_MODE_STA, band) == 0) {
		ret = supplicant_cli_get_oper_std(netdev, &radio.oper_std);
		correct_oper_std_by_band(band, &radio.oper_std);
	}

	if (ret) {
		ret = nlwifi_get_band_supp_stds(name, band, &radio.oper_std);
		if (WARN_ON(ret))
			return ret;
	}

	if (radio.oper_band & BAND_2) max_mcs = 7;
	if (radio.oper_band & BAND_5) max_mcs = 9;
	if (radio.oper_std & WIFI_AX) {
		sgi = WIFI_4xLTF_GI800;
		max_mcs = 11;
	}

	ret = nlwifi_get_bandwidth(netdev, &radio.curr_bw);
	libwifi_dbg("[%s, %s] %s called curr_bw:%s ret %d\n",
		    netdev, wifi_band_to_str(band),__func__, wifi_bw_to_str(radio.curr_bw), ret);

	if (ret) {
		ret = nlwifi_get_max_bandwidth(name, &radio.curr_bw);
		libwifi_dbg("[%s] %s called max_bw: %s\n", name, __func__, wifi_bw_to_str(radio.curr_bw));
	}

	libwifi_dbg("%s called max_mcs=%d, bw=%d, nss=%d sgi=%d \n", __func__, max_mcs, wifi_bw_enum2MHz(radio.curr_bw), radio.rx_streams, sgi);

	*rate_Mbps = wifi_mcs2rate(max_mcs, wifi_bw_enum2MHz(radio.curr_bw), radio.rx_streams, sgi);

	return ret;
#else
	int ant = 0, max_mcs, bw = 20;
	uint8_t std = 0;
	char prefix[64];
	if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3)){
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	}else if(!strncmp(name, "rae", 3)){
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	}else{
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	}
	if(nvram_pf_match(prefix, "bw", "0"))
		bw = 20;
	else if(nvram_pf_match(prefix, "bw", "2"))
		bw = 40;
	else if(nvram_pf_match(prefix, "bw", "3"))
		bw = 80;
	else if(nvram_pf_match(prefix, "bw", "5"))
		bw = 160;
	else if(nvram_pf_match(prefix, "bw", "1") && nvram_pf_match(prefix, "bw_160", "1"))
		bw = 160;
	else if(nvram_pf_match(prefix, "unit", "1"))
		bw = 80;
	else
		bw = 40;
	ant = nvram_pf_get_int(prefix, "HT_RxStream");
	radio_get_oper_stds(name, &std);
	if (!!(std & WIFI_AX)) {
		max_mcs = 11;
		*rate_Mbps = wifi_mcs2rate(max_mcs, bw, ant, WIFI_4xLTF_GI800);
	} else if (!!(std & WIFI_AC)) {
		max_mcs = 9;
		*rate_Mbps = wifi_mcs2rate(max_mcs, bw, ant, WIFI_SGI);
	} else if (!!(std & WIFI_N)) {
		max_mcs = 8 * ant - 1;
		max_mcs %= 8;
		*rate_Mbps = wifi_mcs2rate(max_mcs, bw, ant, WIFI_SGI);
	} else if (!!(std & WIFI_G)) {
		*rate_Mbps = 54;
	} else if (std == WIFI_B) {
		*rate_Mbps = 11;
	}
	return 0;
#endif
}

static int radio_get_maxrate(const char *name, unsigned long *rate_Mbps)
{
	return radio_get_band_maxrate(name, BAND_ANY, rate_Mbps);
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
#if 0
	int lock;
	char data[256];
	struct iwreq wrq;

	memset(data, 0, sizeof(data));
	if(p && p->ssid[0]){
		snprintf(data, sizeof(data), "SiteSurvey=%s", p->ssid);
	}else{
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		PartialScanNumOfCh(ifname);
		strcpy(data, "PartialScan=1");//slower, ap still works
#else
		strcpy(data, "SiteSurvey=1");//faster, ap will stop working for 4s
#endif
	}
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	lock = file_lock("sitesurvey");
	if (wl_ioctl(ifname, RTPRIV_IOCTL_GSITESURVEY, &wrq) < 0)
	{
		file_unlock(lock);
		libwifi_dbg("Site Survey fails\n");
		return -1;
	}
	file_unlock(lock);

	libwifi_dbg("Please wait.");
	sleep(4);
	if(p == NULL || p->ssid[0] == 0){
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		libwifi_dbg(".");
		sleep(4);
		if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3) || !strncmp(ifname, "apclii", 6) || !strncmp(ifname, "apclix", 6)){
			libwifi_dbg(".");
			sleep(1);
		}
		if(!strncmp(ifname, "rae", 3) || !strncmp(ifname, "apclie", 6)){
			libwifi_dbg(".");
			sleep(1);
#if defined(RTCONFIG_WIFI6E)
			libwifi_dbg(".");
			sleep(2);
#endif
		}
#endif
	}
	libwifi_dbg(".\n\n");

	return 0;
#else
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_scan(name, p);
#endif
}

static int radio_scan_ex(const char *name, struct scan_param_ex *sp)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_scan_ex(name, sp);
}

static int radio_get_scan_results(const char *name, struct wifi_bss *bsss, int *num)
{
#if 0
	int length = 0, band, ret;
	char prefix[64];
	size_t ie_len;
	uint8_t *ie;
	struct iwreq wrq;
	struct _SITESURVEY_VSIE *result, *pt;
	struct scanresult {
		int num;
		int max;
		struct wifi_bss *bsslist;
	};
	struct scanresult sres = {.num = 0, .max = *num, .bsslist = bsss};
	struct scanresult *scanres = &sres;
	struct wifi_bss *e = scanres->bsslist + scanres->num;
	if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3) || !strncmp(ifname, "apclii", 6) || !strncmp(ifname, "apclix", 6))
		band = 1;
	else if(!strncmp(ifname, "rae", 3) || !strncmp(ifname, "apclie", 6))
		band = 2;
	else
		band = 0;
	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	libwifi_dbg("sres: &bsss = %p (numbss limit = %d)\n", e, scanres->max);
	length = (getSiteSurveyVSIEcount(ifname) / 6 + 1) << 10;
	if(length == 0){
		libwifi_dbg("length = 0\n");
		return -1;
	}
	result = (struct _SITESURVEY_VSIE *)calloc(length, 1);
	if (result == NULL){
		libwifi_dbg("vsie_list calloc failed\n");
		return -1;
	}
	memset(result, 0, length);
	wrq.u.data.length = length;
	wrq.u.data.pointer = result;
	wrq.u.data.flags = ASUS_SUBCMD_GETSITESURVEY_VSIE;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		libwifi_dbg("errors in getting site survey vie result\n");
		free(result);
		return -1;
	}
	if (wrq.u.data.length <= 0 && strlen(wrq.u.data.pointer) <= 0)
	{
		libwifi_dbg("The output var result is NULL\n");
		free(result);
		return -1;
	}
	for(pt = result; pt->Channel; pt++){
		e = scanres->bsslist + scanres->num;
		memcpy(e->ssid, pt->Ssid, sizeof(e->ssid));
		memcpy(e->bssid, pt->Bssid, sizeof(e->bssid));
		e->channel = (uint8_t)pt->Channel;
		if(!strncmp(ifname, "rae", 3) || !strncmp(ifname, "apclie", 6))
#if defined(RTCONFIG_WIFI6E)
			if(e->channel >= 1 && e->channel <= 233)
				e->band = BAND_6;
#else
			e->band = BAND_5;
#endif
		else if(e->channel > 0 && e->channel <= 14)
			e->band = BAND_2;
		else if(e->channel >= 36 && e->channel <= 177)
			e->band = BAND_5;
		e->rssi = (int)pt->Rssi;
		e->beacon_int = nvram_pf_get_int(prefix, "bcn");
		//e->caps.basic.cap = 
		//e->caps.valid |= WIFI_CAP_BASIC_VALID;
		ie_len = pt->vendor_ie_len;
		ie = pt->vendor_ie;
		wifi_get_bss_security_from_ies(e, ie, ie_len);
		libwifi_dbg("       wpa_versions 0x%x pairwise = 0x%x, group = 0x%x akms 0x%x caps 0x%x\n",
				e->rsn.wpa_versions, e->rsn.pair_ciphers, e->rsn.group_cipher, e->rsn.akms, e->rsn.rsn_caps);

		radio_get_ie(ie, ie_len, IE_BSS_LOAD, &e->load);

		ret = radio_get_ie(ie, ie_len, IE_EXT_CAP, &e->caps.ext);
		if (!ret)
			e->caps.valid |= WIFI_CAP_EXT_VALID;

		ret = radio_get_ie(ie, ie_len, IE_HT_CAP, &e->caps.ht);
		if (!ret)
			e->caps.valid |= WIFI_CAP_HT_VALID;

		ret = radio_get_ie(ie, ie_len, IE_VHT_CAP, &e->caps.vht);
		if (!ret)
			e->caps.valid |= WIFI_CAP_VHT_VALID;

		ret = radio_get_ie(ie, ie_len, IE_RRM, &e->caps.rrm);
		if (!ret)
			e->caps.valid |= WIFI_CAP_RM_VALID;

		radio_get_bandwidth_from_ie(ie, ie_len, &e->curr_bw);

		//TODO: wifi supp standards
		//TODO: a-only, b-only, g-only etc.
		if (!!(e->caps.valid & WIFI_CAP_VHT_VALID))
			e->oper_std = WIFI_N | WIFI_AC;
		else if (!!(e->caps.valid & WIFI_CAP_HT_VALID))
			e->oper_std = e->channel > 14 ? (WIFI_A | WIFI_N) :
							(WIFI_B | WIFI_G | WIFI_N);
		else
			e->oper_std = e->channel > 14 ? WIFI_A : (WIFI_B | WIFI_G);


		scanres->num++;
	}
	if (scanres->num >= scanres->max) {
		libwifi_warn("Num scan results > %d !\n", scanres->max);
		free(result);
		return -1;
	}

	free(result);
	*num = scanres->num;
	return 0;
#else
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_scan_results(name, bsss, num);
#endif
}

static int radio_get_bss_scan_result(const char *name, uint8_t *bssid,
				     struct wifi_bss_detail *b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_band_noise(const char *name, enum wifi_band band, int *noise)
{
	char netdev[16];

//	nlwifi_phy_to_netdev_with_band(name, netdev, sizeof(netdev), band);
	strlcpy(netdev, name, sizeof(netdev));
	libwifi_dbg("[%s, %s, %s] %s called\n", name, netdev, wifi_band_to_str(band),  __func__);
	if (nlwifi_get_band_noise(netdev, band, noise))
		/* TODO - for 7615 upgrade backports/mt76 */
		*noise = -90;

	return 0;
}

static int radio_get_noise(const char *name, int *noise)
{
	return radio_get_band_noise(name, BAND_ANY, noise);
}

static int radio_acs(const char *name, struct acs_param *p)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_start_cac(const char *name, int channel, enum wifi_bw bw, enum wifi_cac_method method)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_start_cac(name, channel, bw, method);
}

static int radio_stop_cac(const char *name)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);

	return nlwifi_stop_cac(name);
}

static int radio_get_opclass_preferences(const char *name,
					 struct wifi_opclass *opclass,
					 int *num)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return wifi_get_opclass_pref(name, num, opclass);
}

static int radio_get_band_opclass_preferences(const char *name,
				enum wifi_band band,
				struct wifi_opclass *opclass,
				int *num)
{
	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);
	return wifi_get_band_opclass_pref(name, band, num, opclass);
}

static int radio_simulate_radar(const char *name, struct wifi_radar_args *radar)
{
	if (WARN_ON(!radar))
		return -1;

	libwifi_dbg("[%s] %s called ch:%d, bandwidth:%d, type:0x%x, subband:0x%x\n",
	            name, __func__, radar->channel, radar->bandwidth, radar->type,
	            radar->subband_mask);

	return -1;
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

static int radio_get_hwaddr(const char *name, uint8_t *hwaddr)
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

static int radio_add_iface(const char *name, enum wifi_mode m, char *argv[])
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_del_iface(const char *name, const char *ifname)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_list_iface(const char *name, struct iface_entry *iface, int *num)
{
	char path[256];
	char *ifname;
	struct dirent *p;
	struct iface_entry *entry;
	int count = 0;
	DIR *d;
	if(!strncmp(name, "rax", 3))
		ifname = "rax";
	else if(!strncmp(name, "rai", 3))
		ifname = "rai";
	else if(!strncmp(name, "rae", 3))
		ifname = "rae";
	else if(!strncmp(name, "ra", 2))
		ifname = "ra";
	else if(!strncmp(name, "apclix", 6))
		ifname = "apclix";
	else if(!strncmp(name, "apclii", 6))
		ifname = "apclii";
	else if(!strncmp(name, "apclie", 6))
		ifname = "apclie";
	else if(!strncmp(name, "apcli", 5))
		ifname = "apcli";
	else
		return -1;
	snprintf(path, sizeof(path), "/sys/class/net");
	if (WARN_ON(!(d = opendir(path))))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (strncmp(p->d_name, ifname, strlen(ifname)) || strlen(p->d_name) != (strlen(ifname) + 1))
			continue;
		if (WARN_ON(count >= *num))
			break;

		libwifi_dbg("[%s] iface  %s\n", name, p->d_name);
		entry = &iface[count];
		memset(entry->name, 0, 16);
		memcpy(entry->name, p->d_name, 15);
		if(!strncmp(name, "apcli", 5))
			entry->mode = WIFI_MODE_STA;
		else
			entry->mode = WIFI_MODE_AP;
		count++;
	}

	closedir(d);
	*num = count;

	return 0;
}

static int radio_channels_info_band(const char *name, enum wifi_band band, struct chan_entry *channel, int *num)
{
	char cc[3] = {0};
	int ret;
	int i;

	ret = nlwifi_channels_info_band(name, band, channel, num);
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

static int radio_channels_info(const char *name, struct chan_entry *channel, int *num)
{
	return radio_channels_info_band(name, BAND_ANY, channel, num);
}

static int radio_get_ifstatus(const char *name, ifstatus_t *f)
{
	if(get_radio_status((char *)name))
		*f |= IFF_UP;
	return 0;
}

/* common iface callbacks */

static int iface_start_wps(const char *ifname, struct wps_param wps)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
//	return hostapd_cli_iface_start_wps(ifname, wps);
	doSystem("iwpriv %s set WscMode=%d", ifname, 2);		// PBC method
	doSystem("iwpriv %s set WscGetConf=%d",ifname, 1);	// Trigger WPS AP to do simple config with WPS Client
	return 0;
}

static int iface_stop_wps(const char *ifname)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
//	return hostapd_cli_iface_stop_wps(ifname);
	doSystem("iwpriv %s set WscStop=1", ifname);			// Stop WPS Process.
	return 0;
}

static int iface_get_wps_status(const char *ifname, enum wps_status *s)
{
	int data = 0;
	struct iwreq wrq;
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
//	return hostapd_cli_iface_get_wps_status(ifname, s);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;

	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0)
		libwifi_dbg("[%s]errors in getting WSC status\n", __func__);
	return data;
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
	char prefix[64] = {0};
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (WARN_ON(!auth) || WARN_ON(!enc))
		return -1;

	*auth = 0;
	*enc = 0;
	if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3))
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	else if(!strncmp(ifname, "rae", 3))
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	else if(!strncmp(ifname, "ra", 2))
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	else if(!strncmp(ifname, "apclix", 6) || !strncmp(ifname, "apclii", 6))
		snprintf(prefix, sizeof(prefix), "wlc%d_", 1);
	else if(!strncmp(ifname, "apclie", 6))
		snprintf(prefix, sizeof(prefix), "wlc%d_", 2);
	else if(!strncmp(ifname, "apcli", 5))
		snprintf(prefix, sizeof(prefix), "wlc%d_", 0);

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
	nlwifi_iface_get_band(ifname, &bss->band);
	correct_oper_std_by_band(bss->band, &bss->oper_std);
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

static int iface_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	memset(s, 0, sizeof(*s));
	return 0;
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

const struct wifi_driver mt_driver = {
	.name = "ra,rai,rax,rae,apcli,apclii,apclix,apclie",
	.info = mtwifi_driver_info,
	/* Radio/phy callbacks */
	.radio.is_multiband = nlwifi_radio_is_multiband,
	.radio.info = radio_info,
	.radio.info_band = radio_info_band,
	.get_supp_band = radio_get_supp_band,
	.get_oper_band = radio_get_oper_band,
	.radio.get_ifstatus = radio_get_ifstatus,
	.radio.get_caps = radio_get_caps,
	.radio.get_band_caps = nlwifi_radio_get_band_caps,
	.radio.get_supp_stds = radio_get_supp_stds,
	.radio.get_band_supp_stds = nlwifi_get_band_supp_stds,
	.get_oper_stds = radio_get_oper_stds,
	.radio.get_band_oper_stds = radio_get_band_oper_stds,

	.get_country = radio_get_country,
	.get_channel = radio_get_channel,
	.radio.get_band_channel = nlwifi_get_band_channel,
	.set_channel = radio_set_channel,
	.get_supp_channels = radio_get_supp_channels,
	.get_oper_channels = radio_get_oper_channels,

	.get_curr_opclass = radio_get_curr_opclass,
	.radio.get_band_curr_opclass = radio_get_band_curr_opclass,

	.get_bandwidth = radio_get_bandwidth,
	.get_supp_bandwidths = nlwifi_get_supp_bandwidths,
	.radio.get_band_supp_bandwidths = nlwifi_get_band_supp_bandwidths,
	.get_maxrate = radio_get_maxrate,
	.radio.get_band_maxrate = radio_get_band_maxrate,
	.radio.get_basic_rates = radio_get_basic_rates,
	.radio.get_oper_rates = radio_get_oper_rates,
	.radio.get_supp_rates = radio_get_supp_rates,
	.radio.get_stats = radio_get_stats,

	.scan = radio_scan,
	.scan_ex = radio_scan_ex,
	.get_scan_results = radio_get_scan_results,
	.get_bss_scan_result = radio_get_bss_scan_result,

	.get_noise = radio_get_noise,
	.radio.get_band_noise = radio_get_band_noise,

	.acs = radio_acs,
	.start_cac = radio_start_cac,
	.stop_cac = radio_stop_cac,
	.get_opclass_preferences = radio_get_opclass_preferences,
	.radio.get_band_opclass_preferences = radio_get_band_opclass_preferences,
	.simulate_radar = radio_simulate_radar,

	.radio.get_param = radio_get_param,
	.radio.set_param = radio_set_param,

	.radio.get_hwaddr = radio_get_hwaddr,

	.add_iface = radio_add_iface,
	.del_iface = radio_del_iface,
	.list_iface = radio_list_iface,
	.channels_info = radio_channels_info,
	.radio.channels_info_band = radio_channels_info_band,

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

	/* Interface/vif ap callbacks */
	.iface.ap_info = iface_ap_info,
	.get_bssid = iface_get_bssid,
	.get_ssid = iface_get_ssid,

	.iface.get_stats = iface_get_stats,
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
