/*
 * ipq40xx.c -implements for Qualcomm drivers.
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
#include <linux/wireless.h>
#define TYPEDEF_BOOL
#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
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

#define	IEEE80211_IOCTL_GETPARAM	(SIOCIWFIRSTPRIV+1)
#define	IEEE80211_IOCTL_GETCHANINFO	(SIOCIWFIRSTPRIV+7)
#define	IEEE80211_PARAM_SWRT_GET_CAPS		562
#define	IEEE80211_PARAM_SWRT_GET_CAPSEXT	563
#define	IEEE80211_PARAM_SWRT_GET_HTCAPS		564
#define	IEEE80211_PARAM_SWRT_GET_HTCAPSEXT	565
#define	IEEE80211_PARAM_SWRT_GET_VHTCAPS	566

#define IEEE80211_CHAN_MAX      255
typedef unsigned int	u_int;
struct ieee80211_ath_channel {
    uint16_t       ic_freq;        /* setting in Mhz */
    uint32_t       ic_flags;       /* see below */
    uint16_t       ic_flagext;     /* see below */
    uint8_t        ic_ieee;        /* IEEE channel number */
    int8_t          ic_maxregpower; /* maximum regulatory tx power in dBm */
    int8_t          ic_maxpower;    /* maximum tx power in dBm */
    int8_t          ic_minpower;    /* minimum tx power in dBm */
    uint8_t        ic_regClassId;  /* regClassId of this channel */ 
    uint8_t        ic_antennamax;  /* antenna gain max from regulatory */
    uint8_t        ic_vhtop_ch_freq_seg1;         /* Channel Center frequency */
    uint8_t        ic_vhtop_ch_freq_seg2;         /* Channel Center frequency applicable
                                                  * for 80+80MHz mode of operation */ 
};

struct ieee80211req_chaninfo {
	u_int	ic_nchans;
	struct ieee80211_ath_channel ic_chans[IEEE80211_CHAN_MAX];
};

struct event_ctx {
	void *handle;
	char family[32];
};

static int getsocket(void)
{
	static int s = -1;

	if (s < 0) {
		s = socket(AF_INET, SOCK_DGRAM, 0);
		if (s < 0)
			libwifi_dbg("socket(SOCK_DRAGM)");
	}
	return s;
}

static int get80211param(const char *ifname, int param, void *data, size_t len)
{
	struct iwreq iwr;

	memset(&iwr, 0, sizeof(iwr));
	if (strlcpy(iwr.ifr_name, ifname, sizeof(iwr.ifr_name)) >= sizeof(iwr.ifr_name)) {
		libwifi_dbg("ifname too long: %s\n", ifname);
		return -1;
	}
	iwr.u.mode = param;

	if (ioctl(getsocket(), IEEE80211_IOCTL_GETPARAM, &iwr) < 0) {
		libwifi_dbg("ioctl[IEEE80211_IOCTL_GETPARAM]");
		return -1;
	}
	if (len < IFNAMSIZ) {
		/*
		 * Argument data fits inline; put it there.
		 */
		memcpy(data, iwr.u.name, len);
	}
	return iwr.u.data.length;
}

static uint32_t ieee80211_channel_to_frequency(uint32_t chan)
{
	if (chan <= 0)
		return 0;
	else if (chan == 14)
		return 2484;
	else if (chan < 14)
		return (chan * 5) + 2407;
	else if (chan >= 183 && chan <= 196)
		return (chan * 5) + 4000;
	else if (chan >= 34 && chan <= 177)
		return (chan * 5) + 5000;
	else
		return 0;
}

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
		if (strncmp(p->d_name, "ath", 3))
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

	if (!strncmp(ifname, "ath", 3) || !strncmp(ifname, "wifi", 4))
		return 1;

	return 0;
}

static void _radio_set_supp_stds(uint32_t bands, const struct wifi_caps *caps, uint8_t *std)
{
	if (bands & BAND_2)
		*std |= WIFI_G | WIFI_B;

	if (bands & BAND_5)
		*std |= WIFI_A;

	if (caps->valid & WIFI_CAP_HT_VALID)
		*std |= WIFI_N;
	if (caps->valid & WIFI_CAP_VHT_VALID)
		*std |= WIFI_AC;
}


static int radio_get_supp_band(const char *name, uint32_t *bands)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	if(!strcmp(name, WIF_2G) || !strcmp(name, VPHY_2G) || !strcmp(name, STA_2G)){
		*bands |= BAND_2;
	}else if(!strcmp(name, WIF_5G) || !strcmp(name, VPHY_5G) || !strcmp(name, STA_5G)){
		*bands |= BAND_5;
	}
#if defined(RTCONFIG_HAS_5G_2)
	else if(!strcmp(name, WIF_5G2) || !strcmp(name, VPHY_5G2) || !strcmp(name, STA_5G2)){
		*bands |= BAND_5;
	}
#endif
	return 0;
}

int radio_get_txpower(const char *name, int8_t *txpower_dbm, int8_t *txpower_percent)
{
	struct ieee80211req_chaninfo chans = {0};
	struct iwreq wrq;
	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!txpower_dbm || !txpower_percent)
		return -EINVAL;
	*txpower_dbm = 0;
	*txpower_percent = 100;
	memset(&wrq, 0, sizeof(wrq));
	wrq.u.data.pointer = (void *)&chans;
	wrq.u.data.length = sizeof(chans);
	if (wl_ioctl(name, IEEE80211_IOCTL_GETCHANINFO, &wrq) < 0)
		return -1;

	*txpower_dbm = chans.ic_chans[0].ic_maxregpower;

	return 0;
}

/* Radio callbacks */
static int radio_info_band(const char *name, enum wifi_band band, struct wifi_radio *radio)
{
	char path[128] = {0}, buf[128] = {0}, prefix[16] = {0};

	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);
	if(!strcmp(name, WIF_2G) || !strcmp(name, VPHY_2G) || !strcmp(name, STA_2G)){
		radio->supp_band |= BAND_2;
		radio->oper_band = BAND_2;
		snprintf(path, sizeof(path), "/sys/class/net/%s/hwmodes", VPHY_2G);
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	}else if(!strcmp(name, WIF_5G) || !strcmp(name, VPHY_5G) || !strcmp(name, STA_5G)){
		radio->supp_band |= BAND_5;
		radio->oper_band = BAND_5;
		snprintf(path, sizeof(path), "/sys/class/net/%s/hwmodes", VPHY_5G);
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	}
#if defined(RTCONFIG_HAS_5G_2)
	else if(!strcmp(name, WIF_5G2) || !strcmp(name, VPHY_5G2) || !strcmp(name, STA_5G2)){
		radio->supp_band |= BAND_5;
		radio->oper_band = BAND_5;
		snprintf(path, sizeof(path), "/sys/class/net/%s/hwmodes", VPHY_5G2);
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	}
#endif
	if(radio->supp_band & BAND_2)
		radio->supp_std |= WIFI_G | WIFI_B;
	if(radio->supp_band & BAND_5)
		radio->supp_std |= WIFI_A;

	f_read_string(path, buf, sizeof(buf));
	if(strstr(buf, "11NG_HT"))//11NG_HT20 11NG_HT40PLUS 11NG_HT40MINUS 11NG_HT40
		radio->supp_std |= WIFI_N;
	if(strstr(buf, "11AC_VHT"))//11AC_VHT20 11AC_VHT40PLUS 11AC_VHT40MINUS 11AC_VHT40 11AC_VHT80 11AC_VHT160 11AC_VHT80_80
		radio->supp_std |= WIFI_AC;

	if(strstr(buf, "11NG_HT")){
		radio->supp_bw |= BIT(BW20);
		if(strstr(buf, "11NG_HT40"))
			radio->supp_bw |= BIT(BW40);
	}
	if(strstr(buf, "11AC_VHT")) {
		radio->supp_bw |= BIT(BW20);
		radio->supp_bw |= BIT(BW40);
		radio->supp_bw |= BIT(BW80);
		if(strstr(buf, "11AC_VHT160"))
			radio->supp_bw |= BIT(BW160);
		if(strstr(buf, "11AC_VHT80_80")){
			radio->supp_bw |= BIT(BW8080);
			radio->supp_bw |= BIT(BW160);
		}
	}

	if (0 == radio->supp_bw)
		radio->supp_bw |= BIT(BW_UNKNOWN);
//an easy way to get these, see gen_ath_config
	radio->frag = nvram_pf_get_int(prefix, "frag");
	radio->rts = nvram_pf_get_int(prefix, "rts");
	radio->srl = nvram_pf_match(prefix, "plcphdr", "short");
	radio->lrl = nvram_pf_match(prefix, "plcphdr", "long");
	radio->beacon_int = nvram_pf_get_int(prefix, "bcn");
	radio->dtim_period = nvram_pf_get_int(prefix, "dtim");
	radio->tx_streams = nvram_pf_get_int(prefix, "HT_TxStream");
	radio->rx_streams = nvram_pf_get_int(prefix, "HT_RxStream");
	//radio->cac_methods = 0;

	radio->channel = (uint8_t)shared_get_channel(name);
//	radio->diag.cca_time = survey.active_time * 1000;
//	radio->diag.channel_busy = survey.busy_time * 1000;
//	radio->diag.tx_airtime = survey.tx_time * 1000;
//	radio->diag.rx_airtime = survey.rx_time * 1000;
//==>fix guest wifi!!!!
	radio->num_iface = 1;
	radio->iface[0].band = radio->oper_band;
	memcpy(radio->iface[0].name, name, 15);
	if(!strcmp(name, STA_2G) || !strcmp(name, STA_5G)
#if defined(RTCONFIG_HAS_5G_2)
		|| !strcmp(name, STA_5G2)
#endif
	)
		radio->iface[0].mode = WIFI_MODE_STA;
	else
		radio->iface[0].mode = WIFI_MODE_AP;
//<==
	if (radio->oper_band == BAND_5) {
		/* TODO get it from iface combinations */
		radio->cac_methods |= BIT(WIFI_CAC_CONTINUOUS);

		radio->dot11h_capable = true;
		if(has_dfs_channel())
			radio->dot11h_enabled = true;
		else
			radio->dot11h_enabled = false;
	} else {
		/* Don't report CAC methods for non 5GHz */
		radio->cac_methods = 0;
	}
	radio_get_txpower(name, &radio->txpower_dbm, &radio->txpower);
	snprintf(radio->regdomain, sizeof(radio->regdomain), "%s", nvram_pf_safe_get(prefix, "country_code"));

	if(!strcmp(name, STA_2G) || !strcmp(name, STA_5G)
#if defined(RTCONFIG_HAS_5G_2)
		|| !strcmp(name, STA_5G2)
#endif
	)
		supplicant_cli_get_oper_std(name, &radio->oper_std);
	else
		hostapd_cli_get_oper_stds(name, &radio->oper_std);

	correct_oper_std_by_band(band, &radio->oper_std);
	radio->oper_std = radio->supp_std;
	return 0;
}

static int radio_info(const char *name, struct wifi_radio *radio)
{
	return radio_info_band(name, BAND_ANY, radio);
}


static int radio_get_oper_band(const char *name, enum wifi_band *band)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	if(!strcmp(name, WIF_2G) || !strcmp(name, VPHY_2G) || !strcmp(name, STA_2G)){
		*band = BAND_2;
	}else if(!strcmp(name, WIF_5G) || !strcmp(name, VPHY_5G) || !strcmp(name, STA_5G)){
		*band = BAND_5;
	}
#if defined(RTCONFIG_HAS_5G_2)
	else if(!strcmp(name, WIF_5G2) || !strcmp(name, VPHY_5G2) || !strcmp(name, STA_5G2)){
		*band = BAND_5;
	}
#endif
	return 0;
}
static int radio_get_band_caps(const char *name, enum wifi_band band, struct wifi_caps *caps)
{
	uint32_t capsext;
	char ifname[16];
	libwifi_dbg("[%s] %s called\n", name, __func__);
	if(!strcmp(name, STA_2G) || !strcmp(name, VPHY_2G))
		snprintf(ifname, sizeof(ifname), "%s", WIF_2G);
	else if(!strcmp(name, STA_5G) || !strcmp(name, VPHY_5G))
		snprintf(ifname, sizeof(ifname), "%s", WIF_5G);
#if defined(RTCONFIG_HAS_5G_2)
	else if(!strcmp(name, STA_5G2) || !strcmp(name, VPHY_5G2))
		snprintf(ifname, sizeof(ifname), "%s", WIF_5G2);
#endif
	else
		snprintf(ifname, sizeof(ifname), "%s", name);
	get80211param(ifname, IEEE80211_PARAM_SWRT_GET_CAPSEXT, &capsext, sizeof(capsext));

//	get80211param(ifname, IEEE80211_PARAM_SWRT_GET_CAPS, &caps->basic.cap, sizeof(caps->basic.cap));
	if(capsext != 0){
		caps->valid |= WIFI_CAP_EXT_VALID;
		memcpy(caps->ext.byte, &capsext, sizeof(uint32_t));
	}

	get80211param(ifname, IEEE80211_PARAM_SWRT_GET_HTCAPS, &caps->ht.cap, sizeof(caps->ht.cap));
	if(caps->ht.cap != 0){
		caps->valid |= WIFI_CAP_HT_VALID;
		get80211param(ifname, IEEE80211_PARAM_SWRT_GET_HTCAPSEXT, &caps->ht.cap_ext, sizeof(caps->ht.cap_ext));
	}
	get80211param(ifname, IEEE80211_PARAM_SWRT_GET_VHTCAPS, &caps->vht.cap, sizeof(caps->vht.cap));
	if(caps->vht.cap != 0){
		caps->valid |= WIFI_CAP_VHT_VALID;
	}

	return 0;
}

static int radio_get_caps(const char *name, struct wifi_caps *caps)
{
	return radio_get_band_caps(name, BAND_ANY, caps);
}

int radio_get_band_supp_stds(const char *name, enum wifi_band band, uint8_t *std)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	struct wifi_caps caps = {};
	uint32_t bands = 0;

	*std = 0;

	if (WARN_ON(radio_get_band_caps(name, band, &caps)))
		return -1;

	if (band == BAND_ANY)
		radio_get_supp_band(name, &bands);
	else
		bands = band;

	_radio_set_supp_stds(bands, &caps, std);
	return 0;
}

static int radio_get_supp_stds(const char *name, uint8_t *std)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return radio_get_band_supp_stds(name, BAND_ANY, std);
}


static int radio_get_band_oper_stds(const char *name, enum wifi_band band, uint8_t *std)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	struct wifi_caps caps = {};
	uint32_t bands = 0;

	*std = 0;

	if (WARN_ON(radio_get_band_caps(name, band, &caps)))
		return -1;

	if (band == BAND_ANY)
		radio_get_supp_band(name, &bands);
	else
		bands = band;

	_radio_set_supp_stds(bands, &caps, std);

	return 0;
}

static int radio_get_oper_stds(const char *name, uint8_t *std)
{
	return radio_get_band_oper_stds(name, BAND_ANY, std);
}

static int radio_get_country(const char *name, char *alpha2)
{
	char prefix[16] = {0};
	libwifi_dbg("[%s] %s called\n", name, __func__);

	if(!strcmp(name, WIF_2G) || !strcmp(name, VPHY_2G) || !strcmp(name, STA_2G)){
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	}else if(!strcmp(name, WIF_5G) || !strcmp(name, VPHY_5G) || !strcmp(name, STA_5G)){
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	}
#if defined(RTCONFIG_HAS_5G_2)
	else if(!strcmp(name, WIF_5G2) || !strcmp(name, VPHY_5G2) || !strcmp(name, STA_5G2)){
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	}
#endif
	snprintf(alpha2, 2, "%s", nvram_pf_safe_get(prefix, "country_code"));
	return 0;
}

static int radio_get_band_channel(const char *name, enum wifi_band band, uint32_t *channel, enum wifi_bw *bw)
{
	int bandwidth = 0, nctrlsb = 0;
	libwifi_dbg("[%s] %s called\n", name, __func__);
	*channel = (uint32_t)shared_get_channel(name);
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

static int radio_get_channel(const char *name, uint32_t *channel, enum wifi_bw *bw)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return radio_get_band_channel(name, BAND_ANY, channel, bw);
}

static int radio_set_channel(const char *name, uint32_t channel, enum wifi_bw bw)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_supp_channels(const char *name, uint32_t *chlist, int *num,
				   const char *alpha2, enum wifi_band f, enum wifi_bw b)
{
	struct iwreq wrq;
	struct iw_range *range;
	unsigned char buffer[sizeof(iwrange) * 2];	/* Large enough */
	int i, max = *num;
	libwifi_dbg("[%s] %s called\n", name, __func__);

	if(wl_ioctl(name, SIOCGIWNAME, &wrq))		/* check wireless extensions. */
		return -1;
	*num = 0;
	memset(buffer, 0, sizeof(buffer));
	wrq.u.data.pointer = (caddr_t) buffer;
	wrq.u.data.length = sizeof(buffer);
	wrq.u.data.flags = 0;

	if(wl_ioctl(name, SIOCGIWRANGE, &wrq)) {
		cprintf("NOT SUPPORT SIOCGIWRANGE in %s\n", name);
		return -1;
	}

	range = (struct iw_range *) buffer;
	if(wrq.u.data.length < 300 || range->we_version_compiled <= 15) {
		cprintf("Wireless Extensions is TOO OLD length(%d) ver(%d)\n", wrq.u.data.length, range->we_version_compiled);
		return -2;
	}

	if (range->num_frequency < max)
		max = range->num_frequency;

	for(i = 0; i < max; i++) {
		chlist[i] = (uint32_t)range->freq[i].i;
	}
	*num = i;
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
	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);
	return wifi_get_band_opclass(name, band, o);;
}

static int radio_get_curr_opclass(const char *name, struct wifi_opclass *o)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return wifi_get_opclass(name, o);
}

#define WIFI_CAP_HT_CHW_40	0x0002
#define WIFI_CAP_VHT_CHW_160	0x00000004
#define WIFI_CAP_VHT_CHW_8080	0x00000008
#define WIFI_CAP_VHT_CHW_MASK	0x0000000C
static void _qca_set_supp_bandwidth(const struct wifi_caps *caps, uint32_t *supp_bw)
{
	if (caps->valid & WIFI_CAP_HT_VALID) {
		*supp_bw |= BIT(BW20);
		if (caps->ht.cap & WIFI_CAP_HT_CHW_40)
			*supp_bw |= BIT(BW40);
	}
	if (caps->valid & WIFI_CAP_VHT_VALID) {
		*supp_bw |= BIT(BW20);
		*supp_bw |= BIT(BW40);
		*supp_bw |= BIT(BW80);
		if ((caps->vht.cap & WIFI_CAP_VHT_CHW_MASK) == WIFI_CAP_VHT_CHW_160)
			*supp_bw |= BIT(BW160);
		else if ((caps->vht.cap & WIFI_CAP_VHT_CHW_MASK) == WIFI_CAP_VHT_CHW_8080) {
			*supp_bw |= BIT(BW160);
			*supp_bw |= BIT(BW8080);
		}
	}

	if (caps->valid & WIFI_CAP_HE_VALID) {
		/* TODO parse MCS for 160/320MHz */
		*supp_bw |= BIT(BW20);
		*supp_bw |= BIT(BW40);
		*supp_bw |= BIT(BW80);
	}

	if (0 == *supp_bw)
		*supp_bw |= BIT(BW_UNKNOWN);
}

int radio_get_band_supp_bandwidths(const char *name, enum wifi_band band, uint32_t *supp_bw)
{
	struct wifi_caps caps;

	if (WARN_ON(radio_get_band_caps(name, band, &caps)))
		return -1;

	_qca_set_supp_bandwidth(&caps, supp_bw);
	return 0;
}

int radio_get_supp_bandwidths(const char *name, uint32_t *supp_bw)
{
	return radio_get_band_supp_bandwidths(name, BAND_ANY, supp_bw);
}

static int radio_get_bandwidth(const char *name, enum wifi_bw *bw)
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

static int radio_get_band_maxrate(const char *name, enum wifi_band band, unsigned long *rate_Mbps)
{
	struct wifi_radio radio = { 0 };
	char prefix[16];
	int ret = 0;
	int max_mcs = 0;
	enum wifi_guard sgi = WIFI_SGI;

	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);
	radio_get_oper_band(name, &radio.oper_band);
	if(!strcmp(name, STA_2G) || !strcmp(name, STA_5G)
#if defined(RTCONFIG_HAS_5G_2)
		|| !strcmp(name, STA_5G2)
#endif
	)
		radio_get_supp_stds(name, &radio.oper_std);
	else
		hostapd_cli_get_oper_stds(name, &radio.oper_std);
	correct_oper_std_by_band(band, &radio.oper_std);

	if (radio.oper_band & BAND_2){
		max_mcs = 7;
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	}
	if (radio.oper_band & BAND_5){
		max_mcs = 9;
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	}
	radio.rx_streams = nvram_pf_get_int(prefix, "HT_RxStream");
	ret = radio_get_bandwidth(name, &radio.curr_bw);
	libwifi_dbg("[%s, %s] %s called curr_bw:%s ret %d\n",
		    name, wifi_band_to_str(band),__func__, wifi_bw_to_str(radio.curr_bw), ret);

	libwifi_dbg("%s called max_mcs=%d, bw=%d, nss=%d sgi=%d \n", __func__, max_mcs, wifi_bw_enum2MHz(radio.curr_bw), radio.rx_streams, sgi);

	*rate_Mbps = wifi_mcs2rate(max_mcs, wifi_bw_enum2MHz(radio.curr_bw), radio.rx_streams, sgi);

	return ret;
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

#define APSCAN_WLIST	"/tmp/apscan_wlist"
/* Run iwlist command to do site-survey.
 * @ssv_if:
 * @return:
 *     -1:	invalid parameter
 * 	0:	site-survey fail
 * 	1:	site-survey success
 * NOTE:	sitesurvey filelock must be hold by caller!
 */
static int do_sitesurvey(char *ssv_if, char *essid)
{
	int retry, ssv_ok = 0;
	char *result, *p;
	char *iwlist_argv[] = { "iwlist", ssv_if, "scanning", NULL, NULL, NULL };

	if (!ssv_if || *ssv_if == '\0')
		return -1;
	if(essid){
		iwlist_argv[3] = "essid";
		iwlist_argv[4] = essid;
		libwifi_dbg("SSID to scan: '%s'  (len = %u)\n", essid, strlen(essid));
	}
	for (retry = 0, ssv_ok = 0; !ssv_ok && retry < 1; ++retry) {
		_eval(iwlist_argv, ">/tmp/apscan_wlist", 0, NULL);

		if (!f_exists(APSCAN_WLIST) || !(result = file2str(APSCAN_WLIST)))
			continue;
		if (!(p = strstr(result, "Scan completed"))) {
			if ((p = strchr(result, '\n')))
				*p = '\0';
			if ((p = strchr(result, '\r')))
				*p = '\0';
			_dprintf("%s: iwlist %s scanning fail!! (%s)!\n", __func__, ssv_if, result);
			free(result);
			continue;
		}

		free(result);
		ssv_ok = 1;
	}

	return ssv_ok;
}

#define target 9
char str[target][40] = {"Address:","ESSID:","Frequency:","Quality=","Encryption key:","IE:","Authentication Suites","Pairwise Ciphers","phy_mode="};
static int radio_scan(const char *name, struct scan_param *p)
{
	int lock, ssv_ok = 0, band = -1, radio, is_sta = 0, bitrate, wlc_band = -1;
	char ssv_if[10], prefix[16];

	libwifi_dbg("[%s] %s called\n", name, __func__);
#if defined(RTCONFIG_WIRELESSREPEATER)
	if(nvram_get("wlc_band") && (repeater_mode() || mediabridge_mode()))
		wlc_band = nvram_get_int("wlc_band");
#endif
	/* Router/access-point/repeater mode
	 * 1. If VAP i/f is UP, radio on, use VAP i/f to site-survey.
	 * 2. If VAP i/f is DOWN, radio off, create new STA i/f to site-survey.
	*/
	if(!strcmp(name, WIF_2G) || !strcmp(name, VPHY_2G) || !strcmp(name, STA_2G)){
		band = 0;
	}else if(!strcmp(name, WIF_5G) || !strcmp(name, VPHY_5G) || !strcmp(name, STA_5G)){
		band = 1;
	}
#if defined(RTCONFIG_HAS_5G_2)
	else if(!strcmp(name, WIF_5G2) || !strcmp(name, VPHY_5G2) || !strcmp(name, STA_5G2)){
		band = 2;
	}
#endif
	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	__get_wlifname(band, 0, ssv_if);
	radio = get_radio_status(ssv_if);
	bitrate = get_bitrate(ssv_if);
	if(!radio || bitrate || (mediabridge_mode() && band != wlc_band))
		strlcpy(ssv_if, get_staifname(band), sizeof(ssv_if));
	if(!strncmp(ssv_if, "sta", 3))
		is_sta = 1;
	lock = file_lock("sitesurvey");
	if(band != wlc_band && is_sta){
#if defined(RTCONFIG_SOC_IPQ40XX)
		doSystem("echo 1 > /sys/module/qca_ol/parameters/qwrap_enable");
		eval("wlanconfig", ssv_if, "create", "wlandev", get_vphyifname(band), "wlanmode", "wrap", "nosbeacon");
#else
		eval("wlanconfig", ssv_if, "create", "wlandev", get_vphyifname(band), "wlanmode", "sta", "nosbeacon");
#endif
		eval("ifconfig", ssv_if, "up");
	}
	if(p && p->ssid[0])
		ssv_ok = do_sitesurvey(ssv_if, p->ssid);
	else
		ssv_ok = do_sitesurvey(ssv_if, NULL);
	if(band != wlc_band && is_sta){
		eval("ifconfig", ssv_if, "down");
		eval("wlanconfig", ssv_if, "destroy");
#if defined(RTCONFIG_SOC_IPQ40XX)
		doSystem("echo 0 > /sys/module/qca_ol/parameters/qwrap_enable");
#endif
	}
	if(ssv_ok <= 0 && !radio && is_sta){
		__get_wlifname(band, 0, ssv_if);
		eval("ifconfig", ssv_if, "up");
		if(p && p->ssid[0])
			ssv_ok = do_sitesurvey(ssv_if, p->ssid);
		else
			ssv_ok = do_sitesurvey(ssv_if, NULL);
		eval("ifconfig", ssv_if, "down");
	}
	file_unlock(lock);

	return 0;
}

int radio_scan_ex(const char *name, struct scan_param_ex *sp)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_scan_results(const char *name, struct wifi_bss *bsss, int *num)
{
   	int apCount = 0, is_ready, i;
	char header[128];
	char buf[target][200], set_flag[target];
	char ch[4] = "", ssid[33] = "", address[18] = "", enc[9] = "";
	char auth[16] = "", sig[9] = "", wmode[8] = "";
	char temp1[200];
	char a1[10], a2[10], a3[10];
	char siglv[11] = "";
	char prefix_header[] = "Cell xx - Address:";
	char *pt1, *pt2, *pt3, *pt4;
	FILE *fp;
	struct wifi_bss *pbss;
	libwifi_dbg("[%s] %s called\n", name, __func__);
	if(!(fp= fopen(APSCAN_WLIST, "r")))
		return 0;
	
	snprintf(header, sizeof(header), "%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode");

	for(apCount = 1; apCount < *num; apCount++){
	   	is_ready=0;
		memset(set_flag,0,sizeof(set_flag));
		memset(buf,0,sizeof(buf));
		memset(temp1,0,sizeof(temp1));
		snprintf(prefix_header, sizeof(prefix_header), "Cell %02d - Address:", apCount);
  		if(feof(fp)) 
		   break;
		pbss = bsss + (apCount - 1);
		memset(pbss, 0, sizeof(struct wifi_bss));
		while(fgets(temp1, sizeof(temp1), fp)){
			if(strstr(temp1, prefix_header) != NULL){
				if(is_ready){   
					fseek(fp, -sizeof(temp1), SEEK_CUR);   
					break;
				}else{	   
					is_ready = 1;
					snprintf(prefix_header, sizeof(prefix_header), "Cell %02d - Address:", apCount + 1);
				}	
			}
			if(is_ready){		   
				for(i = 0; i < target; i++){
					if(strstr(temp1, str[i]) != NULL && set_flag[i] == 0){   
						set_flag[i] = 1;
					    memcpy(buf[i], temp1, sizeof(temp1));
						break;
					}		
				}
			}	

		}
		pt1 = strstr(buf[2], "Channel ");	
		if(pt1){
			pt2 = strstr(pt1, ")");
		   	memset(ch, 0, sizeof(ch));
			strncpy(ch, pt1 + strlen("Channel "), pt2 - pt1 - strlen("Channel "));
			pbss->channel = (uint8_t)atoi(ch);
			if(pbss->channel > 0 && pbss->channel <= 14)
				pbss->band = BAND_2;
			else if(pbss->channel > 14 && pbss->channel <= 177)
				pbss->band = BAND_5;
			else
				pbss->band = BAND_UNKNOWN;
		}
		pt1 = strstr(buf[1], "ESSID:");	
		if(pt1){
		   	memset(ssid, 0, sizeof(ssid));
			strncpy(ssid, pt1 + strlen("ESSID:") + 1, strlen(buf[1]) - 2 - (pt1 + strlen("ESSID:") + 1 - buf[1]));
			memcpy(pbss->ssid, ssid, strlen(ssid));
		}
		pt1 = strstr(buf[0], "Address: ");	
		if(pt1){
		   	memset(address, 0, sizeof(address));
			strncpy(address, pt1 + strlen("Address: "), strlen(buf[0]) - (pt1 + strlen("Address: ") - buf[0]) - 1);
			ether_atoe(address, pbss->bssid);
		}
		pt1 = strstr(buf[4], "Encryption key:");
		if(pt1){
			if(strstr(pt1 + strlen("Encryption key:"), "on")){
				pt2 = strstr(buf[7], "Pairwise Ciphers");
				if(pt2){
					if(strstr(pt2, "CCMP TKIP") || strstr(pt2, "TKIP CCMP")){
						strlcpy(enc, "TKIP+AES", sizeof(enc));
						pbss->rsn.pair_ciphers |= BIT(WIFI_CIPHER_TKIP | WIFI_CIPHER_AES);
						pbss->rsn.group_cipher = WIFI_CIPHER_TKIP | WIFI_CIPHER_AES;
					}else if(strstr(pt2, "CCMP")){
						strlcpy(enc, "AES", sizeof(enc));
						pbss->rsn.pair_ciphers |= BIT(WIFI_CIPHER_AES);
						pbss->rsn.group_cipher = WIFI_CIPHER_AES;
					}else{
						strlcpy(enc, "TKIP", sizeof(enc));
						pbss->rsn.pair_ciphers |= BIT(WIFI_CIPHER_TKIP);
						pbss->rsn.group_cipher = WIFI_CIPHER_TKIP;
					}
				}else{
					strlcpy(enc, "WEP", sizeof(enc));
					pbss->rsn.pair_ciphers |= BIT(WIFI_CIPHER_WEP40);
					pbss->rsn.group_cipher = WIFI_CIPHER_WEP40;
				}
			}else{
				strlcpy(enc, "NONE", sizeof(enc));
			}
		}
		memset(auth, 0, sizeof(auth));
		pt1 = strstr(buf[5], "IE:");
		if(pt1 && strstr(buf[5], "Unknown") == NULL){
			if(strstr(pt1 + strlen("IE:"), "WPA2") != NULL){
				strlcpy(auth, "WPA2-", sizeof(auth));
				pbss->rsn.wpa_versions |= WPA_VERSION2;
			}else if(strstr(pt1 + strlen("IE:"), "WPA") != NULL){
				strlcpy(auth, "WPA-", sizeof(auth));
				pbss->rsn.wpa_versions |= WPA_VERSION1;
			}
		
			pt2 = strstr(buf[6], "Authentication Suites");
			if(pt2){
				if(strstr(pt2 + strlen("Authentication Suites"), "PSK") != NULL){
			   		strcat(auth, "Personal");
					pbss->rsn.akms |= BIT(WIFI_AKM_PSK);
					if(!!(pbss->rsn.wpa_versions & WPA_VERSION2))
						pbss->security |= BIT(WIFI_SECURITY_WPA2PSK);
					if(!!(pbss->rsn.wpa_versions & WPA_VERSION1))
						pbss->security |= BIT(WIFI_SECURITY_WPAPSK);
				}else if(strstr(pt2 + strlen("Authentication Suites"), "802.1x") != NULL){
				   	strcat(auth, "Enterprise");
					if(!!(pbss->rsn.wpa_versions & WPA_VERSION2))
						pbss->security |= BIT(WIFI_SECURITY_WPA2);
					if(!!(pbss->rsn.wpa_versions & WPA_VERSION1))
						pbss->security |= BIT(WIFI_SECURITY_WPAPSK);
				}else
					libwifi_warn("%s:%d ERROR!! NO AKM TYPE Set!!\n", __func__, __LINE__);
			}
		}else{
			if(strcmp(enc, "WEP") == 0){
				strlcpy(auth, "Unknown", sizeof(auth));
				pbss->security |= BIT(WIFI_SECURITY_NONE);
			}else{
				strlcpy(auth, "Open System", sizeof(auth));
				pbss->security |= BIT(WIFI_SECURITY_NONE);
			}
		}
		pbss->rsn.rsn_caps = 0;//wpa3
		libwifi_dbg("       wpa_versions 0x%x pairwise = 0x%x, group = 0x%x akms 0x%x caps 0x%x\n",
		    pbss->rsn.wpa_versions, pbss->rsn.pair_ciphers, pbss->rsn.group_cipher, pbss->rsn.akms, pbss->rsn.rsn_caps);
		//memcpy(pbss->load, ?m, sizeof(struct wifi_ap_load));//fixme
		pt1 = strstr(buf[3], "Quality=");	
		pt2 = NULL;
		if (pt1 != NULL)
			pt2 = strstr(pt1, "/");
		if(pt1 && pt2){
			memset(sig, 0, sizeof(sig));
			memset(a1, 0, sizeof(a1));
			memset(a2, 0, sizeof(a2));
			strncpy(a1, pt1 + strlen("Quality="), pt2 - pt1 - strlen("Quality="));
			strncpy(a2, pt2 + 1, strstr(pt2, " ") - ( pt2 + 1));
			sprintf(sig, "%d", 100 * (atoi(a1) + 6) / (atoi(a2) + 6));
			if(sw_mode() != SW_MODE_REPEATER) {
				memset(siglv, 0, sizeof(siglv));
				memset(a3, 0, sizeof(a3));
				pt3 = strstr(pt2, "Signal level=-");
				if(pt3){
					pt4 = strstr(pt3," dBm");
					if(pt3 && pt4)
					{
						strncpy(a3, pt3 + strlen("Signal level=-"), pt4 - pt3 - strlen("Signal level=-"));
						snprintf(siglv, sizeof(siglv), "%s", a3);
						pbss->rssi = 0 - atoi(siglv);
						libwifi_dbg("    Rssi: %d dBm", pbss->rssi);
					}	
				}
			}
		}
		memset(wmode, 0, sizeof(wmode));
		pt1 = strstr(buf[8], "phy_mode=");
		if(pt1){
			//pbss->caps.valid |= WIFI_CAP_EXT_VALID;
			pbss->caps.valid |= WIFI_CAP_HT_VALID;
			if((pt2 = strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_11AC_VHT")) != NULL){
				strlcpy(wmode, "ac", sizeof(wmode));
				pbss->caps.valid |= WIFI_CAP_VHT_VALID;
			}else if((pt2 = strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_11A")) != NULL || (pt2 = strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_TURBO_A")) != NULL)
				strlcpy(wmode, "a", sizeof(wmode));
			else if((pt2 = strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_11B")) != NULL)
				strlcpy(wmode, "b", sizeof(wmode));
			else if((pt2 = strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_11G")) != NULL || (pt2 = strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_TURBO_G")) != NULL)
				strlcpy(wmode, "bg", sizeof(wmode));
			else if((pt2 = strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_11NA")) != NULL)
				strlcpy(wmode, "an", sizeof(wmode));
			else if(strstr(pt1 + strlen("phy_mode="), "IEEE80211_MODE_11NG"))
				strlcpy(wmode, "bgn", sizeof(wmode));
		}
		else
			strlcpy(wmode, "unknown", sizeof(wmode));
		//pbss->beacon_int
	}
	if(apCount >= *num)
		libwifi_warn("Num scan results > %d !\n", *num);
	*num = apCount - 1;
	fclose(fp);
	return 0;
}

static int radio_get_bss_scan_result(const char *name, uint8_t *bssid,
				     struct wifi_bss_detail *b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_get_band_noise(const char *name, enum wifi_band band, int *noise)
{
	int status, quality, signal;
	unsigned int update;
	extern int get_wl_status(const char *ifname, int *status, int *quality, int *signal, int *noise, unsigned int *update);
	get_wl_status(name, &status, &quality, &signal, noise, &update);
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

static int radio_start_cac(const char *name, int channel, enum wifi_bw bw,
			   enum wifi_cac_method method)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int radio_stop_cac(const char *name)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
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
	
	doSystem("radartool -i %s bangradar", name);
	return 0;
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
	struct dirent *p;
	struct iface_entry *entry;
	int count = 0;
	DIR *d;

	if (WARN_ON(!(d = opendir("/sys/class/net"))))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (strncmp(p->d_name, "ath", 3) && strncmp(p->d_name, "sta", 3))
			continue;
		if (WARN_ON(count >= *num))
			break;

		libwifi_dbg("[%s] iface  %s\n", name, p->d_name);
		entry = &iface[count];
		memset(entry->name, 0, 16);
		memcpy(entry->name, p->d_name, 15);
		if(!strncmp(p->d_name, "ath", 3))
			entry->mode = WIFI_MODE_AP;
		else if(!strncmp(p->d_name, "sta", 3))
			entry->mode = WIFI_MODE_STA;
		count++;
	}

	closedir(d);
	*num = count;

	return 0;
}

static int radio_channels_info_band(const char *name, enum wifi_band band, struct chan_entry *channel, int *num)
{
	int i = 0, max = *num;
	int cac = 0;
	char cc[3] = {0};
	char vap[IFNAMSIZ];
	struct chan_entry *entry;
	struct iwreq wrq;
	struct iw_range *range;
	unsigned char buffer[sizeof(iwrange) * 2];	/* Large enough */

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if(wl_ioctl(name, SIOCGIWNAME, &wrq))		/* check wireless extensions. */
		return -1;
	*num = 0;
	memset(buffer, 0, sizeof(buffer));
	wrq.u.data.pointer = (caddr_t) buffer;
	wrq.u.data.length = sizeof(buffer);
	wrq.u.data.flags = 0;

	if(wl_ioctl(name, SIOCGIWRANGE, &wrq)) {
		cprintf("NOT SUPPORT SIOCGIWRANGE in %s\n", name);
		return -1;
	}

	range = (struct iw_range *) buffer;
	if(wrq.u.data.length < 300 || range->we_version_compiled <= 15) {
		cprintf("Wireless Extensions is TOO OLD length(%d) ver(%d)\n", wrq.u.data.length, range->we_version_compiled);
		return -2;
	}

	if (range->num_frequency < max)
		max = range->num_frequency;

	for(i = 0; i < max && range->freq[i].i; i++) {
		entry = channel + i;
		memset(entry, 0, sizeof(struct chan_entry));
		entry->channel = (uint32_t)range->freq[i].i;
		if(entry->channel > 0 && entry->channel <= 14)
			entry->band = BAND_2;
		else if(entry->channel > 14 && entry->channel <= 177)
			entry->band = BAND_5;
		entry->freq = ieee80211_channel_to_frequency(entry->channel);

		entry->noise = -90;
	if(band == BAND_5 && has_dfs_channel()){
		entry->dfs = true;
		if(!strcmp(name, WIF_2G) || !strcmp(name, VPHY_2G) || !strcmp(name, STA_2G)){
			snprintf(vap, sizeof(vap), "%s", WIF_2G);
		}else if(!strcmp(name, WIF_5G) || !strcmp(name, VPHY_5G) || !strcmp(name, STA_5G)){
			snprintf(vap, sizeof(vap), "%s", WIF_5G);
		}
#if defined(RTCONFIG_HAS_5G_2)
		else if(!strcmp(name, WIF_5G2) || !strcmp(name, VPHY_5G2) || !strcmp(name, STA_5G2)){
			snprintf(vap, sizeof(vap), "%s", WIF_5G2);
		}
#endif
		iwpriv_get_int(vap, "get_cac_state", &cac);
		if(cac == 1)
			entry->dfs_state = WIFI_DFS_STATE_CAC;
		else
			entry->dfs_state = WIFI_DFS_STATE_AVAILABLE;
		entry->cac_time = 60;
		entry->nop_time = 1800;
	}else
		entry->dfs_state = WIFI_DFS_STATE_NONE;

	}
	*num = i;

	WARN_ON(radio_get_country(name, cc));
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

	return 0;
}

static int radio_channels_info(const char *name, struct chan_entry *channel, int *num)
{
	return radio_channels_info_band(name, BAND_ANY, channel, num);
}

static int radio_get_ifstatus(const char *name, ifstatus_t *f)
{
	char vap[16];
	ifstatus_t ifstatus;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (get_ifstatus(name, f)) {
		if(!strcmp(name, VPHY_2G)){
			snprintf(vap, sizeof(vap), "%s", WIF_2G);
		}else if(!strcmp(name, VPHY_5G)){
			snprintf(vap, sizeof(vap), "%s", WIF_5G);
		}
#if defined(RTCONFIG_HAS_5G_2)
		else if(!strcmp(name, VPHY_5G2)){
			snprintf(vap, sizeof(vap), "%s", WIF_5G2);
		}
#endif
		if (!get_ifstatus(vap, &ifstatus)) {
			if (ifstatus & IFF_UP) {
				*f |= IFF_UP;
			}
		}
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
	return 0;
}

static int iface_get_mode(const char *ifname, enum wifi_mode *mode)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	if(!strcmp(ifname, STA_2G) || !strcmp(ifname, STA_5G)
#if defined(RTCONFIG_HAS_5G_2)
		|| !strcmp(ifname, STA_5G2)
#endif
	)
		*mode = WIFI_MODE_STA;
	else
		*mode = WIFI_MODE_AP;
	return 0;
}

static int iface_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
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

static int iface_get_4addr(const char *ifname, bool *enable)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	*enable = false;
	return 0;
}

static int iface_get_4addr_parent(const char *ifname, char *parent)
{
	enum wifi_mode mode;
	int ret;
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = iface_get_mode(ifname, &mode);
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

static int iface_link_measure(const char *ifname, uint8_t *sta)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (!sta || hwaddr_is_zero(sta)) {
		libwifi_dbg("[%s] %s invalid arg\n", ifname, __func__);
		return -1;
	}

	return hostapd_cli_rrm_lm_req(ifname, sta);
}

/* ap interface ops */

static int iface_get_bssid(const char *ifname, uint8_t *bssid)
{
	char path[128] = {0}, buf[20] = {0};
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	snprintf(path, sizeof(path), "/sys/class/net/%s/address", ifname);
	f_read_string(path, buf, sizeof(buf));
	ether_atoe(buf, bssid);
	return 0;
}

static int iface_get_ssid(const char *ifname, char *ssid)
{
	int unit = 0, subunit = 0;
	char prefix[64] = {0};
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	get_wlif_unit(ifname, &unit, &subunit);
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	snprintf(ssid, 32, "%s", nvram_pf_get(prefix, "ssid"));
	return 0;
}

static int iface_ap_info(const char *ifname, struct wifi_ap *ap)
{
	/* TODO
	*	ap->isolate_enabled
	*/
	struct wifi_bss *bss;
	uint32_t ch = 0;
	int unit = 0, subunit = 0;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (!is_valid_ap_iface(ifname))
		return -1;
	get_wlif_unit(ifname, &unit, &subunit);
	bss = &ap->bss;
	memset(bss, 0, sizeof(*bss));
	hostapd_cli_iface_ap_info(ifname, ap);
	iface_get_bssid(ifname, bss->bssid);
	iface_get_ssid(ifname, (char *)bss->ssid);
	if (!strlen((char *)bss->ssid))
		ap->enabled = false;

	radio_get_channel(ifname, &ch, &bss->curr_bw);
	bss->channel = (uint8_t)ch;
	if(unit == 0)
		bss->band = BAND_2;
	else
		bss->band = BAND_5;

	correct_oper_std_by_band(bss->band, &bss->oper_std);
	radio_get_supp_stds(ifname, &ap->bss.supp_std);
	hostapd_cli_get_security_cap(ifname, &ap->sec.supp_modes);

	return 0;
}



static int iface_get_stats(const char *ifname, struct wifi_ap_stats *s)
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
	ret = hostapd_cli_iface_get_sta_info(iface, addr, info);

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

static int iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *monsta)
{
	int i;
	WIFI_STA_TABLE *sta_info;

	libwifi_dbg("[%s] %s " MACSTR " called\n", ifname, __func__, MAC2STR(sta));

	memset(monsta, 0, sizeof(*monsta));
	memcpy(monsta->macaddr, sta, 6);
	if ((sta_info = malloc(sizeof(*sta_info))) != NULL){
		get_qca_sta_info_by_ifname(ifname, '0', sta_info);
		for(i = 0; i < sta_info->Num; i++) {
			if(memcmp(sta, sta_info->Entry[i].addr, 6))
				continue;
			monsta->rssi[0] = sta_info->Entry[i].rssi;
			monsta->rssi[1] = 0;
			monsta->rssi[2] = 0;
			monsta->rssi[3] = 0;
			monsta->last_seen = 0;
			free(sta_info);
			return 0;
		}
		free(sta_info);
	}

	/* Worst case for not connected station - get from hostapd (probes) */
	return hostapd_ubus_iface_get_monitor_sta(ifname, sta, monsta);
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
	uint32_t sec, min, hour, rate;
	int i;
	uint8_t bssid[6];
	const char *iface;
	WIFI_STA_TABLE *sta_info;
	if(!strcmp(ifname, VPHY_2G))
		iface = WIF_2G;
	else if(!strcmp(ifname, VPHY_5G))
		iface = WIF_5G;
	else
		iface = ifname;
	if ((sta_info = malloc(sizeof(*sta_info))) != NULL){
		get_qca_sta_info_by_ifname(iface, '0', sta_info);
		iface_get_bssid(iface, bssid);
		for(i = 0; i < sta_info->Num; i++) {
			memcpy(sta->macaddr, sta_info->Entry[i].addr, 6);
			memcpy(sta->bssid, bssid, 6);
			sscanf(sta_info->Entry[i].conn_time, "%d:%d:%d", &hour, &min, &sec);
			sta->conn_time = hour * 3600 + min * 60 + sec;
			if(sta->conn_time)
				sta->idle_time = 0;
			else
				sta->idle_time = 300;
			sscanf(sta_info->Entry[i].rxrate, "%dM", &rate);
			sta->rx_rate.rate = rate;
			sscanf(sta_info->Entry[i].txrate, "%dM", &rate);
			sta->tx_rate.rate = rate;
			if(strstr(sta_info->Entry[i].vhtcaps, "g") || strstr(sta_info->Entry[i].vhtcaps, "G") || strstr(sta_info->Entry[i].htcaps, "S")){
				sta->rx_rate.m.sgi = 1;
				sta->tx_rate.m.sgi = 1;
			}
			if(!strcmp(sta_info->Entry[i].mode, "11AC_VHT80")){
				sta->rx_rate.phy = PHY_VHT;
				sta->rx_rate.m.bw = 80;
				sta->tx_rate.phy = PHY_VHT;
				sta->tx_rate.m.bw = 80;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strcmp(sta_info->Entry[i].mode, "11AC_VHT160")){
				sta->rx_rate.phy = PHY_VHT;
				sta->rx_rate.m.bw = 160;
				sta->tx_rate.phy = PHY_VHT;
				sta->tx_rate.m.bw = 160;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strcmp(sta_info->Entry[i].mode, "11AC_VHT80_80")){
				sta->rx_rate.phy = PHY_VHT;
				sta->rx_rate.m.bw = 160;
				sta->tx_rate.phy = PHY_VHT;
				sta->tx_rate.m.bw = 160;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strncmp(sta_info->Entry[i].mode, "11AC_VHT40", 10)){
				sta->rx_rate.phy = PHY_VHT;
				sta->rx_rate.m.bw = 40;
				sta->tx_rate.phy = PHY_VHT;
				sta->tx_rate.m.bw = 40;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strcmp(sta_info->Entry[i].mode, "11AC_VHT20")){
				sta->rx_rate.phy = PHY_VHT;
				sta->rx_rate.m.bw = 20;
				sta->tx_rate.phy = PHY_VHT;
				sta->tx_rate.m.bw = 20;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strncmp(sta_info->Entry[i].mode, "11NA_HT40", 10)){
				sta->rx_rate.phy = PHY_HT;
				sta->rx_rate.m.bw = 40;
				sta->tx_rate.phy = PHY_HT;
				sta->tx_rate.m.bw = 40;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strcmp(sta_info->Entry[i].mode, "11NA_HT20")){
				sta->rx_rate.phy = PHY_HT;
				sta->rx_rate.m.bw = 20;
				sta->tx_rate.phy = PHY_HT;
				sta->tx_rate.m.bw = 20;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strcmp(sta_info->Entry[i].mode, "TURBO_A") || !strcmp(sta_info->Entry[i].mode, "11A")){
				sta->rx_rate.phy = PHY_OFDM;
				sta->rx_rate.m.bw = 20;
				sta->tx_rate.phy = PHY_OFDM;
				sta->tx_rate.m.bw = 20;
				sta->rx_rate.m.mcs = 9;
				sta->tx_rate.m.mcs = 9;
			}else if(!strncmp(sta_info->Entry[i].mode, "11NG_HT40", 10)){
				sta->rx_rate.phy = PHY_HT;
				sta->rx_rate.m.bw = 40;
				sta->tx_rate.phy = PHY_HT;
				sta->tx_rate.m.bw = 40;
				sta->rx_rate.m.mcs = 7;
				sta->tx_rate.m.mcs = 7;
			}else if(!strcmp(sta_info->Entry[i].mode, "11NG_HT20")){
				sta->rx_rate.phy = PHY_HT;
				sta->rx_rate.m.bw = 20;
				sta->tx_rate.phy = PHY_HT;
				sta->tx_rate.m.bw = 20;
				sta->rx_rate.m.mcs = 7;
				sta->tx_rate.m.mcs = 7;
			}else if(!strcmp(sta_info->Entry[i].mode, "TURBO_G") || !strcmp(sta_info->Entry[i].mode, "11G")){
				sta->rx_rate.phy = PHY_OFDM;
				sta->rx_rate.m.bw = 20;
				sta->tx_rate.phy = PHY_OFDM;
				sta->tx_rate.m.bw = 20;
				sta->rx_rate.m.mcs = 7;
				sta->tx_rate.m.mcs = 7;
			}else if(!strcmp(sta_info->Entry[i].mode, "11B")){
				sta->rx_rate.phy = PHY_DSSS;
				sta->rx_rate.m.bw = 20;
				sta->tx_rate.phy = PHY_DSSS;
				sta->tx_rate.m.bw = 20;
				sta->rx_rate.m.mcs = 7;
				sta->tx_rate.m.mcs = 7;
			}
			sta->rx_rate.m.nss = (uint8_t)sta_info->Entry[i].rxnss;
			sta->tx_rate.m.nss = (uint8_t)sta_info->Entry[i].txnss;
		}
		free(sta_info);
	}

	WARN_ON(if_gethwaddr(iface, sta->macaddr));
	ret = supplicant_sta_info(iface, sta);

	return ret;
}

static int iface_sta_get_stats(const char *ifname, struct wifi_sta_stats *s)
{
//driver don't support statistics
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
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

static int radio_is_multiband(const char *name, bool *res)
{
	struct wifi_radio radio = {};
	int bands = 0;
	int ret;

	ret = radio_info(name, &radio);
	if (ret)
		return ret;

	if (radio.supp_band & BAND_2)
		bands++;
	if (radio.supp_band & BAND_5)
		bands++;
	if (radio.supp_band & BAND_6)
		bands++;

	*res = bands > 1 ? true : false;
	return 0;
}

int qca_driver_info(const char *name, struct wifi_metainfo *info)
{
	char path[256] = {};
	char *pos;
	int fd;

	libwifi_dbg("%s %s called\n", name, __func__);
//ipq4019+qca9984 or ipq4019+ipq4019+qca9984
	if(!strcmp(name, "ath0")
#if defined(RTCONFIG_HAS_5G_2) //|| !defined(RTCONFIG_PCIE_QCA9984)
		|| !strcmp(name, "ath1")
#endif
	){
		snprintf(info->vendor_id, sizeof(info->vendor_id), "%s", "0x168c");
		snprintf(info->device_id, sizeof(info->device_id), "%s", "0x4019");
		return 0;
	}
	snprintf(path, sizeof(path), "/sys/devices/platform/soc/40000000.pci/pci0000:00/0000:00:00.0/%s", "vendor");
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

	snprintf(path, sizeof(path), "/sys/devices/platform/soc/40000000.pci/pci0000:00/0000:00:00.0/%s", "device");
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

static int qca_register_event(const char *ifname, struct event_struct *ev, void **evhandle)
{
	return 0;
}

static int qca_unregister_event(const char *ifname, void *evhandle)
{
	return 0;
}

static int qca_recv_event(const char *ifname, void *evhandle)
{
	return 0;
}

static int register_event(const char *ifname, struct event_struct *ev, void **evhandle)
{
	struct event_ctx *ctx;
	void *handle = NULL;
	int ret;

	libwifi_dbg("%s %s called family %s group %s\n", ifname, __func__, ev->family, ev->group);

	ctx = calloc(1, sizeof(struct event_ctx));
	WARN_ON(!ctx);
	if (!ctx)
		return -1;

	if (!strcmp(ev->family, "wext"))
		ret = qca_register_event(ifname, ev, &handle);
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

	if (!strcmp(ctx->family, "wext"))
		ret = qca_unregister_event(ifname, ctx->handle);
	else
		ret = -1;

	free(ctx);
	return ret;
}

static int recv_event(const char *ifname, void *evhandle)
{
	struct event_ctx *ctx = evhandle;
	int ret;

	libwifi_dbg("[%s] %s called %s\n", ifname, __func__, ctx->family ?: "null");

	if (!strcmp(ctx->family, "wext"))
		ret = qca_recv_event(ifname, ctx->handle);
	else
		ret = -1;

	return ret;
}

const struct wifi_driver qca_driver = {
	.name = "ath,sta,phy",
	.info = qca_driver_info,

	/* Radio/phy callbacks */
	.radio.is_multiband = radio_is_multiband,
	.radio.info = radio_info,
	.radio.info_band = radio_info_band,
	.get_supp_band = radio_get_supp_band,
	.get_oper_band = radio_get_oper_band,
	.radio.get_ifstatus = radio_get_ifstatus,
	.radio.get_caps = radio_get_caps,
	.radio.get_band_caps = radio_get_band_caps,
	.radio.get_supp_stds = radio_get_supp_stds,
	.radio.get_band_supp_stds = radio_get_band_supp_stds,
	.get_oper_stds = radio_get_oper_stds,
	.radio.get_band_oper_stds = radio_get_band_oper_stds,

	.get_country = radio_get_country,
	.get_channel = radio_get_channel,
	.radio.get_band_channel = radio_get_band_channel,
	.set_channel = radio_set_channel,
	.get_supp_channels = radio_get_supp_channels,
	.get_oper_channels = radio_get_oper_channels,

	.get_curr_opclass = radio_get_curr_opclass,
	.radio.get_band_curr_opclass = radio_get_band_curr_opclass,

	.get_bandwidth = radio_get_bandwidth,
	.get_supp_bandwidths = radio_get_supp_bandwidths,
	.radio.get_band_supp_bandwidths = radio_get_band_supp_bandwidths,
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
	.link_measure = iface_link_measure,

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
	.register_event = register_event,
	.unregister_event = unregister_event,
	.recv_event = recv_event,
};
