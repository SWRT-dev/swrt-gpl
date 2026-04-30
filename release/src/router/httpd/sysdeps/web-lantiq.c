/*
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of
 * the License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston,
 * MA 02111-1307 USA
 */
/*
 * ASUS Home Gateway Reference Design
 * Web Page Configuration Support Routines
 *
 * Copyright 2004, ASUSTeK Inc.
 * Copyright 2022-2026, SWRTdev.
 * All Rights Reserved.
 * 
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 */

#ifdef WEBS
#include <webs.h>
#include <uemf.h>
#include <ej.h>
#else /* !WEBS */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <unistd.h>
#include <limits.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <assert.h>
#include <httpd.h>
#endif /* WEBS */
#include <typedefs.h>
#include <bcmnvram.h>
#include <bcmutils.h>
#include <shutils.h>
#include <lantiq.h>
#include <iwlib.h>
//#include <stapriv.h>
#include <shared.h>
#include <sys/mman.h>
#ifndef O_BINARY
#define O_BINARY 	0
#endif
#ifndef MAP_FAILED
#define MAP_FAILED (-1)
#endif

#define wan_prefix(unit, prefix)	snprintf(prefix, sizeof(prefix), "wan%d_", unit)
//static char * rfctime(const time_t *timep);
//static char * reltime(unsigned int seconds);
void reltime(unsigned int seconds, char *buf);
static int wl_status(int eid, webs_t wp, int argc, char_t **argv, int unit);

#include <fcntl.h>
#include <signal.h>
#include <time.h>
#include <sys/klog.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/sockios.h>
#include <net/if_arp.h>
#include <dirent.h>

typedef struct _WPS_CONFIGURED_VALUE {
	unsigned short 	Configured;	// 1:un-configured/2:configured
	char		BSSID[18];
	char 		SSID[32 + 1];
	char		AuthMode[16];	// Open System/Shared Key/WPA-Personal/WPA2-Personal/WPA-Enterprise/WPA2-Enterprise
	char 		Encryp[8];	// None/WEP/TKIP/AES
	char 		DefaultKeyIdx;
	char 		WPAKey[64 + 1];
} WPS_CONFIGURED_VALUE;

/* shared/sysdeps/api-qca.c */
extern u_int ieee80211_mhz2ieee(u_int freq);
extern int get_channel_list_via_driver(int unit, char *buffer, int len);
extern int shared_get_channel(const char *ifname);

#define WL_A		(1U << 0)
#define WL_B		(1U << 1)
#define WL_G		(1U << 2)
#define WL_N		(1U << 3)
#define WL_AC		(1U << 4)
#define WL_AX		(1U << 5)

static const struct mode_s {
	unsigned int mask;
	char *mode;
} mode_tbl[] = {
	{ WL_A,	"a" },
	{ WL_B,	"b" },
	{ WL_G, "g" },
	{ WL_N, "n" },
	{ WL_AC, "ac" },
	{ WL_AX, "ax" },
	{ 0, NULL },
};

static void getWPSConfig(int unit, WPS_CONFIGURED_VALUE *result)
{
	char buf[128];
	FILE *fp;

	memset(result, 0, sizeof(*result));

	snprintf(buf, sizeof(buf), "hostapd_cli -i%s get_config", get_wififname(unit));
	fp = popen(buf, "r");
	if (fp) {
		while (fgets(buf, sizeof(buf), fp) != NULL) {
			char *pt1, *pt2;

			chomp(buf);
			//BSSID
			if ((pt1 = strstr(buf, "bssid="))) {
				pt2 = pt1 + safe_strlen("bssid=");
				strlcpy(result->BSSID, pt2, sizeof(result->BSSID));
			}
			//SSID
			if ((pt1 = strstr(buf, "ssid="))) {
				pt2 = pt1 + safe_strlen("ssid=");
				strlcpy(result->SSID, pt2, sizeof(result->SSID));
			}
			//Configured
			else if ((pt1 = strstr(buf, "wps_state="))) {
				pt2 = pt1 + safe_strlen("wps_state=");
				if (!strcmp(pt2, "configured") ||
				    (!strcmp(pt2, "disabled") && nvram_get_int("w_Setting"))
				   )
					result->Configured = 2;
				else
					result->Configured = 1;
			}
			//WPAKey
			else if ((pt1 = strstr(buf, "passphrase="))) {
				pt2 = pt1 + safe_strlen("passphrase=");
				strlcpy(result->WPAKey, pt2, sizeof(result->WPAKey));
			}
			//AuthMode
			else if ((pt1 = strstr(buf, "key_mgmt="))) {
				pt2 = pt1 + safe_strlen("key_mgmt=");
				strlcpy(result->AuthMode, pt2, sizeof(result->AuthMode));/* FIXME: NEED TRANSFORM CONTENT */
			}
			//Encryp
			else if ((pt1 = strstr(buf, "rsn_pairwise_cipher="))) {
				pt2 = pt1 + safe_strlen("rsn_pairwise_cipher=");
				if (!strcmp(pt2, "NONE"))
					strlcpy(result->Encryp, "None", sizeof(result->Encryp));
				else if (!strncmp(pt2, "WEP", 3))
					strlcpy(result->Encryp, "WEP", sizeof(result->Encryp));
				else if (!strcmp(pt2, "TKIP"))
					strlcpy(result->Encryp, "TKIP", sizeof(result->Encryp));
				else if (!strncmp(pt2, "CCMP", 4))
					strlcpy(result->Encryp, "AES", sizeof(result->Encryp));
				else if (unit == WL_60G_BAND && !strncmp(pt2, "GCMP", 4))
					strlcpy(result->Encryp, "AES", sizeof(result->Encryp));
			}
		}
		pclose(fp);
	}
	//dbg("%s: SSID[%s], Configured[%d], WPAKey[%s], AuthMode[%s], Encryp[%s]\n", __FUNCTION__, result->SSID, result->Configured, result->WPAKey, result->AuthMode, result->Encryp);
}

/**
 * Convert WL_XXX bit masks to string via mode_tbl[]
 * @mask:
 * @return:
 */
static char *mode_mask_to_str(unsigned int mask)
{
	static char result[sizeof("11b/g/nXXXXXXXXXXXX")] = "";
	const struct mode_s *q;
	char *p, *sep;
	size_t len, l;
	int noax = 0;

	if (!find_word(nvram_safe_get("rc_support"), "11AX"))
		noax = 1;

	p = result;
	len = sizeof(result);
	*p = '\0';
	sep = "11";
	for (q = &mode_tbl[0]; len > 0 && mask > 0 && q->mask; ++q) {
		if (!(mask & q->mask))
			continue;

		if (q->mask == WL_AX && noax)
			continue;

		mask &= ~q->mask;
		strlcat(p, sep, len);
		l = safe_strlen(sep);
		p += l;
		len -= l;
		strlcat(p, q->mode, len);
		l = safe_strlen(q->mode);
		p += l;
		len -= l;
		sep = "/";
	}

	return result;
}

/**
 * Get phy mode via nl80211 driver.
 * @ifname:
 * @return:
 */
char *getAPPhyModebyIfaceIW(const char *ifname)
{
	static char result[sizeof("11a/b/g/n/ac/adXXXXXX")] = "";
	unsigned int m = 0;
	char cmd[sizeof("iwinfo wlan0 info") + IFNAMSIZ];

	if (!ifname || *ifname == '\0') {
		dbg("%s: got invalid ifname %p\n", __func__, ifname);
		return 0;
	}

	/* Example:
	 * wlan0     ESSID: "OpenWrt_11ax"
	 *           Access Point: 04:CE:14:0A:21:17
	 *           Mode: Master  Channel: 3 (62.640 GH)
	 *           Tx-Power: unknown  Link Quality: 0/100
	 *           Signal: unknown  Noise: unknown
	 *           Bit Rate: unknown
	 *           Encryption: none
	 *           Type: nl80211  HW Mode(s): 802.11ac/ax/n
	 *           Hardware: 1AE9:0310 1AE9:0000 [Generic MAC80211]
	 *           TX power offset: unknown
	 *           Frequency offset: unknown
	 *           Supports VAPs: no
	 *           Beacon Interval: 100
	 */
	snprintf(cmd, sizeof(cmd), "iwinfo %s info", ifname);
	if (exec_and_parse(cmd, "HW Mode", "%*[^:]:%*[^:]: %s", 1, result))
		*result = '\0';
	if(strstr(result, "ax"))
		m |= WL_AX;
	if(strstr(result, "ac"))
		m |= WL_AC;
	if(strstr(result, "n"))
		m |= WL_N;
	if(strstr(result, "g"))
		m |= WL_G;
	if(strstr(result, "b"))
		m |= WL_B;
	if(strstr(result, "a"))
		m |= WL_A;

	return mode_mask_to_str(m);
}

char *getAPPhyMode(int unit)
{
	char *r = "";

	if (unit < 0 || unit >= MAX_NR_WL_IF)
		return "";

	switch (unit) {
	case WL_2G_BAND:	/* fall-through */
	case WL_5G_BAND:	/* fall-through */
	case WL_5G_2_BAND:
		r = getAPPhyModebyIfaceIW(get_wififname(unit));
		break;
	default:
		dbg("%s: unknown wl%d band!\n", __func__, unit);
	}

	return r;
}

int is_if_up(char *ifname)
{
    int s;
    struct ifreq ifr;

    /* Open a raw socket to the kernel */
    if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0) return -1;

    /* Set interface name */
    strlcpy(ifr.ifr_name, ifname, IFNAMSIZ);

    /* Get interface flags */
    if (ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
        fprintf(stderr, "SIOCGIFFLAGS error\n");
    } else {
        if (ifr.ifr_flags & IFF_UP) {
            fprintf(stderr, "%s is up\n", ifname);
            close(s);
            return 1;
        }
    }
    close(s);
    return 0;
}
unsigned int getAPChannel(int unit)
{
	int r = 0;

	if (__absent_band(unit))
		return 0;

	switch (unit) {
	case WL_2G_BAND:	/* fall-through */
	case WL_5G_BAND:	/* fall-through */
	case WL_5G_2_BAND:
		r = shared_get_channel(get_wififname(unit));
		break;
	default:
		dbg("%s: Unknown wl%d band!\n", __func__, unit);
	}

	return r;
}

static void getVAPBandwidth(int unit, char *buf, size_t buf_len)
{
	FILE *fp;
	int r;
	char rate[4], cmd[64], line[64];

	if (!buf || !buf_len)
		return;

	strlcpy(buf, rate, buf_len);
	snprintf(cmd, sizeof(cmd), "cat /proc/net/mtlk/%s/channel", get_vphyifname(unit));
	fp = popen(cmd, "r");
	if (!fp)
		return;
	/* Example:
	 * width:           	  80
	 * center_freq1:    	5210
	 * chan.center_freq:	5180
	 * channum_from:    	  36
	 * channum_to:      	  48
	 * primary_channel: 	  36
	 * primary_chan_idx:	   0
	 */
	while (fgets(line, sizeof(line), fp)) {
		if (!strstr(line, "width"))
			continue;
		if ((r = sscanf(line, "%*[^:]:%s", rate)) != 1) {
			continue;
		}
		break;
	}
	pclose(fp);

	if (rate[0] == '\0') {
		strlcpy(buf, "20", buf_len);
	} else {
		snprintf(buf, buf_len, "%s", rate);
	}
}

static int __getAPBitRate(const char *ifname, char *buf, size_t buf_len)
{
	int r, band, isax = 0;
	FILE *fp;
	char cmd[sizeof("iwconfig athXYYYYYY")], line[256], rate[16] = {0}, unit[16] = {0};

	if (!ifname || *ifname == '\0' || !buf || !buf_len) {
		dbg("%s: got invalid ifname,buf,buf_len %p,%p,%d\n",
			__func__, ifname, buf, buf_len);
		return 0;
	}

	snprintf(cmd, sizeof(cmd), "iwconfig %s", ifname);
	fp = popen(cmd, "r");
	if (!fp)
		return 0;

	/* Example:
	 * / # iwconfig ath0
	 * ath0      IEEE 802.11axa  ESSID:"ASUS_00_5G"
	 *           Mode:Master  Frequency:5.745 GHz  Access Point: 00:03:7F:12:B3:B3
	 *           Bit Rate:4.8039 Gb/s   Tx-Power:40 dBm
	 *           RTS thr:off   Fragment thr:off
	 *           Encryption key:BEA0-1288-6B95-B8A6-BDB4-4AF3-3248-2244   Security mode:restricted
	 *           Power Management:off
	 *           Link Quality=94/94  Signal level=-107 dBm  Noise level=-105 dBm
	 *           Rx invalid nwid:3287  Rx invalid crypt:0  Rx invalid frag:0
	 *           Tx excessive retries:0  Invalid misc:0   Missed beacon:0
	 */
	while (fgets(line, sizeof(line), fp)) {
		if(strstr(line, "acax") || strstr(line, "nax"))
			isax = 1;
		if (!strstr(line, "Bit Rate"))
			continue;
		if ((r = sscanf(line, "%*[^:]:%15s %15s", rate, unit)) != 2) {
			_dprintf("%s: Unknown bit rate of ifname [%s]: [%s]\n",
				__func__, ifname, line);
			continue;
		}
		break;
	}
	pclose(fp);

	if (rate[0] == '\0' || unit[0] == '\0') {
		band = get_wifname_band(ifname);
		getVAPBandwidth(band, line, sizeof(line));
		if(isax){
			if(strstr(line, "160"))
				r = 2400;
			else if(strstr(line, "80"))
				r = 1200;
			else if(strstr(line, "40"))
				r = 600;
			else
				r = 300;
		}else{
			if(strstr(line, "160"))
				r = 1733;
			else if(strstr(line, "80"))
				r = 866;
			else if(strstr(line, "40"))
				r = 433;
			else
				r = 266;
		}
		snprintf(buf, buf_len, "%d %s", r, "Mb/s");
	} else {
		snprintf(buf, buf_len, "%s %s", rate, unit);
	}

	return 0;
}

static void getVAPBitRate(int unit, char *ifname, char *buf, size_t buf_len)
{
	if (!buf || !buf_len)
		return;

	switch (unit) {
	case WL_2G_BAND:	/* fall-through */
	case WL_5G_BAND:	/* fall-through */
	case WL_5G_2_BAND:
		__getAPBitRate(ifname, buf, buf_len);
		break;
	default:
		dbg("%s: Unknown wl%d band!\n", __func__, unit);
	}
}

static int getVAPCAC(int unit)
{
	FILE *fp;
	int r = 0;
	char cmd[64], line[64];
	snprintf(cmd, sizeof(cmd), "hostapd_cli status");
	fp = popen(cmd, "r");
	if (!fp)
		return r;
	while (fgets(line, sizeof(line), fp)) {
		if (strstr(line, "state=")){
			if (strstr(line, "state=DFS"))
				r = 1;
			break;
		}
	}
	pclose(fp);
	return r;
}

int
ej_wl_control_channel(int eid, webs_t wp, int argc, char_t **argv)
{
	int ret = 0;
	int channel_24 = 0, channel_50 = 0, channel_5G2 = 0;

	channel_24 = getAPChannel(0);
	channel_50 = getAPChannel(1);
	channel_5G2 = getAPChannel(2);
	ret = websWrite(wp, "[\"%d\", \"%d\", \"%d\"]",
		channel_24, channel_50, channel_5G2);

	return ret;
}

void
convert_mac_string(char *mac)
{
	int i;
	char mac_str[18], mac_str2[18];
	memset(mac_str,0,sizeof(mac_str));
	memset(mac_str2,0,sizeof(mac_str2));

	for(i=0;i<strlen(mac);i++)
        {
                if(*(mac+i)>0x60 && *(mac+i)<0x67) {
			snprintf(mac_str2, sizeof(mac_str2), "%s%c",mac_str,*(mac+i)-0x20);
			strlcpy(mac_str, mac_str2, sizeof(mac_str2));
                } else {
			snprintf(mac_str2, sizeof(mac_str2), "%s%c",mac_str,*(mac+i));
			strlcpy(mac_str, mac_str2, sizeof(mac_str2));
		}
        }
	strlcpy(mac, mac_str, safe_strlen(mac_str) + 1);
}

static int getSTAInfo(int unit, WIFI_STA_TABLE *sta_info)
{
	int ret = 0, subunit;
	char *unit_name;
	char *p, *ifname;
	char *wl_ifnames;

	memset(sta_info, 0, sizeof(WIFI_STA_TABLE));
	unit_name = strdup(get_wififname(unit));
	if (!unit_name)
		return ret;
	wl_ifnames = strdup(nvram_safe_get("lan_ifnames"));
	if (!wl_ifnames) {
		free(unit_name);
		return ret;
	}
	p = wl_ifnames;
	while ((ifname = strsep(&p, " ")) != NULL) {
		while (*ifname == ' ') ++ifname;
		if (*ifname == 0) break;
		if(strncmp(ifname,unit_name,safe_strlen(unit_name)))
			continue;

		subunit = get_wlsubnet(unit, ifname);
		if (subunit < 0)
			subunit = 0;
		ret = get_lantiq_sta_info_by_ifname(ifname, subunit, sta_info);

	}
	free(wl_ifnames);
	free(unit_name);
	return ret;
}

char* GetBW(int BW)
{
	switch(BW)
	{
		case BW_10:
			return "10M";

		case BW_20:
			return "20M";

		case BW_40:
			return "40M";

		case BW_80:
			return "80M";
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
		case BW_160:
			return "160M";
#endif
		default:
			return "N/A";
	}
}

int
ej_wl_status(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
	int retval = 0;
	int ii = 0;
	char word[256], *next;

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		retval += wl_status(eid, wp, argc, argv, ii);
		retval += websWrite(wp, "\n");

		ii++;
	}

	return retval;
}

int
ej_wl_status_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	return ej_wl_status(eid, wp, argc, argv, 0);
}

static int
show_wliface_info(webs_t wp, int unit, char *ifname, char *op_mode)
{
	int i, ret = 0, cac = 0, radar_cnt = 0, radar_list[32];
	uint64_t m = 0;
	FILE *fp;
	unsigned char mac_addr[ETHER_ADDR_LEN];
	char tmpstr[1024], cmd[sizeof("iwconfig staXYYYYYY")], prefix[sizeof("wlX_XXX")];
	char *p, ap_bssid[sizeof("00:00:00:00:00:00XXX")], vphy[IFNAMSIZ];
#if defined(RTCONFIG_MULTILAN_MWL)
	char swap_name[64];
#endif

	if (unit < 0 || !ifname || !op_mode)
		return 0;

#if defined(RTCONFIG_MULTILAN_MWL)
	if (!strcmp(get_wififname(unit), ifname)) {
		/* do not show hidden BACKHAUL infromation */
		if (get_sdntype_iface("MAINFH", unit, swap_name, sizeof(swap_name)))
			ifname = swap_name;
	}
#endif
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	memset(&mac_addr, 0, sizeof(mac_addr));
	get_iface_hwaddr(ifname, mac_addr);
	ret += websWrite(wp, "=======================================================================================\n"); // separator
	ret += websWrite(wp, "OP Mode		: %s\n", op_mode);
	ret += websWrite(wp, "SSID		: %s\n", nvram_pf_safe_get(prefix, "ssid"));

		/* Example: iw wlan0 info
		 * Interface wlan0
		 *         ifindex 24
		 *         wdev 0x1
		 *         addr 04:ce:14:0b:46:12
		 *         type AP
		 *         wiphy 0
		 */
		snprintf(cmd, sizeof(cmd), "iw %s info", ifname);
		if (exec_and_parse(cmd, "addr", "%*s %[^\n]", 1, ap_bssid))
			*ap_bssid = '\0';
		convert_mac_string(ap_bssid);
	ret += websWrite(wp, "BSSID		: %s\n", ap_bssid);
	ret += websWrite(wp, "MAC address	: %02X:%02X:%02X:%02X:%02X:%02X\n",
		mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
	*tmpstr = '\0';
	strlcpy(tmpstr, getAPPhyMode(unit), sizeof(tmpstr));
	ret += websWrite(wp, "Phy Mode	: %s\n", tmpstr);
	getVAPBitRate(unit, ifname, tmpstr, sizeof(tmpstr));
	if (unit == WL_5G_BAND || unit == WL_5G_2_BAND) {
		cac = getVAPCAC(unit);
		strlcpy(vphy, get_vphyifname(unit), sizeof(vphy));
		radar_cnt = get_radar_channel_list(vphy, radar_list, ARRAY_SIZE(radar_list));
		for (i = 0; i < radar_cnt; ++i) {
			m |= ch5g2bitmask(radar_list[i]);
		}
	}

	ret += websWrite(wp, "Bit Rate	: %s%s", tmpstr, cac? " (CAC scan)" : "");
	getVAPBandwidth(unit, tmpstr, sizeof(tmpstr));
	if (*tmpstr != '\0')
		ret += websWrite(wp, ", %sMHz", tmpstr);
	ret += websWrite(wp, "\n");
	ret += websWrite(wp, "Channel		: %u", getAPChannel(unit));
	if (m) {
		ret += websWrite(wp, " (Radar: %s)", bitmask2chlist5g(m, ","));
	}
	ret += websWrite(wp, "\n");

	return ret;
}

static int
wl_status(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
	int ret = 0, wl_mode_x, i;
	WIFI_STA_TABLE *sta_info;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_", *ifname, *op_mode;
	char subunit_str[8];

#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_PROXYSTA)
	if (mediabridge_mode()) {
		/* Media bridge mode */
		snprintf(prefix, sizeof(prefix), "wl%d.1_", unit);
		ifname = nvram_pf_safe_get(prefix, "ifname");
		if (unit != nvram_get_int("wlc_band")) {
			snprintf(prefix, sizeof(prefix), "wl%d_", unit);
			ret += websWrite(wp, "%s radio is disabled\n",
				wl_nband_name(nvram_pf_get(prefix, "nband")));
			return ret;
		}
		ret += show_wliface_info(wp, unit, ifname, "Media Bridge");
	} else {
#endif
		/* Router mode, Repeater and AP mode */
#if defined(RTCONFIG_WIRELESSREPEATER)
		if (!unit && repeater_mode()) {
			/* Show P-AP information first, if we are about to show 2.4G information in repeater mode. */
#if defined(RTCONFIG_REPEATER_STAALLBAND)
			unit = nvram_get_int("wlc_triBand");
			snprintf(prefix, sizeof(prefix), "sta%d", unit);
			ret += show_wliface_info(wp, unit, prefix, "Repeater");
			ret += websWrite(wp, "\n");
			unit = 0;
#else
			snprintf(prefix, sizeof(prefix), "wl%d.1_", nvram_get_int("wlc_band"));
			ifname = nvram_pf_safe_get(prefix, "ifname");
			ret += show_wliface_info(wp, nvram_get_int("wlc_band"), ifname, "Repeater");
			ret += websWrite(wp, "\n");
#endif
		}
#endif

		snprintf(prefix, sizeof(prefix), "wl%d_", unit);
		ifname = nvram_pf_safe_get(prefix, "ifname");
		if (!get_radio_status(ifname)) {
#if defined(BAND_2G_ONLY)
			ret += websWrite(wp, "2.4 GHz radio is disabled\n");
#else
			ret += websWrite(wp, "%s radio is disabled\n",
				wl_nband_name(nvram_pf_get(prefix, "nband")));
#endif
			return ret;
		}
		if (nvram_get_int("wave_action") != 0) {
			ret += websWrite(wp, "%s radio is not ready\n",
				wl_nband_name(nvram_pf_get(prefix, "nband")));
			return ret;
		}
		if (nvram_pf_match(prefix, "nband", "1")) {
			if ( is_if_up(WIF_5G) != 1 ){
				ret += websWrite(wp, "5 GHz radio is not ready\n");
				return ret;
			}
		} else {
			if ( is_if_up(WIF_2G) != 1 ){
				ret += websWrite(wp, "2.4 GHz radio is not ready\n");
				return ret;
			}
		}

		wl_mode_x = nvram_pf_get_int(prefix, "mode_x");
		op_mode = "AP";
		if (wl_mode_x == 1)
			op_mode = "WDS Only";
		else if (wl_mode_x == 2)
			op_mode = "Hybrid";
		ret += show_wliface_info(wp, unit, ifname, op_mode);
		ret += websWrite(wp, "\nStations List\n");
		ret += websWrite(wp, "----------------------------------------------------------------\n");
		ret += websWrite(wp, "%-3s %-17s %-8s %-4s %-6s %-6s %-12s\n",
			"idx", "MAC", "PhyMode", "RSSI", "TXRATE", "RXRATE", "Connect Time");

		if ((sta_info = malloc(sizeof(*sta_info))) != NULL) {
			getSTAInfo(unit, sta_info);
			for(i = 0; i < sta_info->Num; i++) {
				*subunit_str = '\0';
				snprintf(subunit_str, sizeof(subunit_str), "%d", i);
				ret += websWrite(wp, "%-3s %-17s %-8s %4d %-6s %-6s %-12s\n",
					subunit_str,
					sta_info->Entry[i].addr,
					sta_info->Entry[i].mode,
					sta_info->Entry[i].rssi,
					sta_info->Entry[i].txrate,
					sta_info->Entry[i].rxrate,
					sta_info->Entry[i].conn_time
					);
			}
			free(sta_info);
		}
#if defined(RTCONFIG_WIRELESSREPEATER) && defined(RTCONFIG_PROXYSTA)
	}
#endif

	return ret;
}

static int ej_wl_sta_list(int unit, webs_t wp)
{
	WIFI_STA_TABLE *sta_info;
	char *value;
	int firstRow = 1;
	int i;
	int from_app = 0;

	from_app = check_user_agent(user_agent);

	if(hook_get_json == 1)
		websWrite(wp, "{");

	if ((sta_info = malloc(sizeof(*sta_info))) != NULL)
	{
		getSTAInfo(unit, sta_info);
		for(i = 0; i < sta_info->Num; i++)
		{
			if (firstRow == 1)
				firstRow = 0;
			else
				websWrite(wp, ", ");

			if (from_app == 0 && hook_get_json == 0)
				websWrite(wp, "[");

			websWrite(wp, "\"%s\"", sta_info->Entry[i].addr);

			if (from_app != 0 || hook_get_json == 1) {
				websWrite(wp, ":{");
				websWrite(wp, "\"isWL\":");
			}

			value = "Yes";
			if (from_app == 0 && hook_get_json == 0)
				websWrite(wp, ", \"%s\"", value);
			else
				websWrite(wp, "\"%s\"", value);

			value = "";

			if (from_app == 0 && hook_get_json == 0)
				websWrite(wp, ", \"%s\"", value);
	
			if (from_app != 0 || hook_get_json == 1) {
				websWrite(wp, ",\"rssi\":");
			}

			if (from_app == 0 && hook_get_json == 0)
				websWrite(wp, ", \"%d\"", sta_info->Entry[i].rssi);
			else
				websWrite(wp, "\"%d\"", sta_info->Entry[i].rssi);

			if (from_app == 0 && hook_get_json == 0)
				websWrite(wp, "]");
			else
				websWrite(wp, "}");
		}
		free(sta_info);
	}
	if(hook_get_json == 1)
		websWrite(wp, "}");
	return 0;
}

int ej_wl_sta_list_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	ej_wl_sta_list(0, wp);
	return 0;
}

int ej_wl_sta_list_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	ej_wl_sta_list(1, wp);
	return 0;
}

int ej_wl_sta_list_5g_2(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(RTCONFIG_HAS_5G_2)
	/* FIXME: I think it's not good to report 2-nd 5G station list in 1-st 5G station list. */
	ej_wl_sta_list(2, wp);
#endif
	if(hook_get_json == 1)
		websWrite(wp, "{}");
	return 0;
}

#if defined(RTCONFIG_STAINFO)
/**
 * Format:
 * 	[ MAC, TX_RATE, RX_RATE, CONNECT_TIME, IDX ]
 * IDX:	main/GN1/GN2/GN3
 */
static int wl_stainfo_list(int unit, webs_t wp)
{
	WIFI_STA_TABLE *sta_info;
	WLANCONFIG_LIST *r;
	char idx_str[8];
	int i, s, firstRow = 1;

	if ((sta_info = malloc(sizeof(*sta_info))) == NULL){
		if(hook_get_json == 1)
			websWrite(wp, "[]");
		return 0 ;
	}

	if(hook_get_json == 1)
		websWrite(wp, "[");

	getSTAInfo(unit, sta_info);
	for(i = 0, r = &sta_info->Entry[0]; i < sta_info->Num; i++, r++) {
		if (firstRow == 1)
			firstRow = 0;
		else
			websWrite(wp, ", ");

		websWrite(wp, "[");
		websWrite(wp, "\"%s\"", r->addr);
		websWrite(wp, ", \"%s\"", r->txrate);
		websWrite(wp, ", \"%s\"", r->rxrate);
		websWrite(wp, ", \"%s\"", r->conn_time);
		s = r->subunit_id;
		if (s < 0 || s > 7)
			s = 0;
		if (!s)
			strcpy(idx_str, "main");
		else
			snprintf(idx_str, sizeof(idx_str), "GN%d", s);
		websWrite(wp, ", \"%s\"", idx_str);
		websWrite(wp, "]");
	}
	if(hook_get_json == 1)
		websWrite(wp, "]");
	free(sta_info);
	return 0;
}

int
ej_wl_stainfo_list_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_stainfo_list(0, wp);
}

int
ej_wl_stainfo_list_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_stainfo_list(1, wp);
}

int
ej_wl_stainfo_list_5g_2(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_stainfo_list(2, wp);
}
#endif	/* RTCONFIG_STAINFO */

int ej_get_wlstainfo_list(int eid, webs_t wp, int argc, char_t **argv)
{
	char word[64], *next;
	int unit = 0;
	int haveInfo = 0;

	websWrite(wp, "{");

	foreach (word, nvram_safe_get("wl_ifnames"), next) {
		WIFI_STA_TABLE *sta_info;
		WLANCONFIG_LIST *r;
		int i, j, s;
		char alias[16];

		SKIP_ABSENT_BAND_AND_INC_UNIT(unit);
		if ((sta_info = malloc(sizeof(*sta_info))) == NULL)
			return 0;

		getSTAInfo(unit, sta_info);
		for (i = 0; i < MAX_NO_MSSID; i++) {
			int firstRow = 1;

			memset(alias, 0, sizeof(alias));
			switch (unit) {
				case WL_2G_BAND:
					if (i == 0)
						strlcpy(alias, "2G", sizeof(alias));
					else
						snprintf(alias, sizeof(alias), "%s_%d", "2G", i);
					break;
				case WL_5G_BAND:	/* fall-through */
#if defined(RTCONFIG_HAS_5G_2)
				case WL_5G_2_BAND:
#endif
					if (i == 0)
						snprintf(alias, sizeof(alias), "%s", unit == 2 ? "5G1" : "5G");
					else
						snprintf(alias, sizeof(alias), "%s_%d", unit == 2 ? "5G1" : "5G", i);
					break;
				default:
					dbg("%s():%d: Unknown unit %d (MAX_NR_WL_IF %d)\n", __func__, __LINE__, unit, MAX_NR_WL_IF);
			}

			for(j = 0, r = &sta_info->Entry[0]; j < sta_info->Num; j++, r++) {
				if (firstRow == 1) {
					if (haveInfo)
						websWrite(wp, ",");
					websWrite(wp, "\"%s\":[", alias);
					firstRow = 0;
					haveInfo = 1;
				}
				else
					websWrite(wp, ",");
				websWrite(wp, "{\"mac\":\"%s\",\"rssi\":%d}", r->addr, r->rssi);
			}

			if (!firstRow)
				websWrite(wp, "]");
		}
		free(sta_info);
		unit++;
	}
	websWrite(wp, "}");

	return 0;
}

char *getWscStatus(int unit)
{
	char buf[512];
	FILE *fp;
	int len;
	char *pt1,*pt2;

	snprintf(buf, sizeof(buf), "hostapd_cli -i%s wps_get_status", get_wififname(unit));
	fp = popen(buf, "r");
	if (fp) {
		memset(buf, 0, sizeof(buf));
		len = fread(buf, 1, sizeof(buf), fp);
		pclose(fp);
		if (len > 1) {
			buf[len-1] = '\0';
			pt1 = strstr(buf, "Last WPS result: ");
			if (pt1) {
				pt2 = pt1 + safe_strlen("Last WPS result: ");
				pt1 = strstr(pt2, "Peer Address: ");
				if (pt1) {
					*pt1 = '\0';
					chomp(pt2);
				}
				return pt2;
			}
		}
	}
	return "";
}

char *getAPPIN(int unit)
{
	static char buffer[128];
#if 0
	char cmd[64];
	FILE *fp;
	int len;

	buffer[0] = '\0';
	snprintf(cmd, sizeof(cmd), "hostapd_cli -i%s wps_ap_pin get", get_wififname(unit));
	fp = popen(cmd, "r");
	if (fp) {
		len = fread(buffer, 1, sizeof(buffer), fp);
		pclose(fp);
		if (len > 1) {
			buffer[len] = '\0';
			//dbg("%s: AP PIN[%s]\n", __FUNCTION__, buffer);
			if(!strncmp(buffer,"FAIL",4))
			   strlcpy(buffer,nvram_get("secret_code"), sizeof(buffer));
			return buffer;
		}
	}
	return "";
#else
	snprintf(buffer, sizeof(buffer), "%s", nvram_safe_get("secret_code"));
	return buffer;
#endif
}

int wl_wps_info(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
	int j = -1, u = unit;
	char tmpstr[128];
	WPS_CONFIGURED_VALUE result;
	int retval=0;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
	char *wps_sta_pin;
	char tag1[] = "<wps_infoXXXXXX>", tag2[] = "</wps_infoXXXXXX>";

#if defined(RTCONFIG_WPSMULTIBAND)
	for (j = -1; j < MAX_NR_WL_IF; ++j) {
#endif
		switch (j) {
		case WL_2G_BAND:	/* fall through */
		case WL_5G_BAND:	/* fall through */
		case WL_5G_2_BAND:	/* fall through */
			u = j;
			snprintf(tag1, sizeof(tag1), "<wps_info%d>", j);
			snprintf(tag2, sizeof(tag2), "</wps_info%d>", j);
			break;
		case -1: /* fall through */
		default:
			u = unit;
			strlcpy(tag1, "<wps_info>", sizeof(tag1));
			strlcpy(tag2, "</wps_info>", sizeof(tag2));
		}

		snprintf(prefix, sizeof(prefix), "wl%d_", u);

#if defined(RTCONFIG_WPSMULTIBAND)
		SKIP_ABSENT_BAND(u);
		if (!nvram_pf_get(prefix, "ifname"))
			continue;
#endif

		memset(&result, 0, sizeof(result));
		getWPSConfig(u, &result);

		if (j == -1)
			retval += websWrite(wp, "<wps>\n");

		//0. WSC Status
		memset(tmpstr, 0, sizeof(tmpstr));
		strlcpy(tmpstr, getWscStatus(u), sizeof(tmpstr));
		retval += websWrite(wp, "%s%s%s\n", tag1, tmpstr, tag2);

		//1. WPS Configured
		if (result.Configured==2)
			retval += websWrite(wp, "%s%s%s\n", tag1, "Yes", tag2);
		else
			retval += websWrite(wp, "%s%s%s\n", tag1, "No", tag2);

		//2. WPS SSID
		memset(tmpstr, 0, sizeof(tmpstr));
		char_to_ascii(tmpstr, result.SSID);
		retval += websWrite(wp, "%s%s%s\n", tag1, tmpstr, tag2);

		//3. WPS AuthMode
		retval += websWrite(wp, "%s%s%s\n", tag1, result.AuthMode, tag2);

		//4. WPS Encryp
		retval += websWrite(wp, "%s%s%s\n", tag1, result.Encryp, tag2);

		//5. WPS DefaultKeyIdx
		memset(tmpstr, 0, sizeof(tmpstr));
		snprintf(tmpstr, sizeof(tmpstr), "%d", result.DefaultKeyIdx);/* FIXME: TBD */
		retval += websWrite(wp, "%s%s%s\n", tag1, tmpstr, tag2);

		//6. WPS WPAKey
#if 0	//hide for security
		if (!safe_strlen(result.WPAKey))
			retval += websWrite(wp, "%sNone%s\n", tag1, tag2);
		else
		{
			memset(tmpstr, 0, sizeof(tmpstr));
			char_to_ascii(tmpstr, result.WPAKey);
			retval += websWrite(wp, "%s%s%s\n", tag1, tmpstr, tag2);
		}
#else
		retval += websWrite(wp, "%s%s\n", tag1, tag2);
#endif
		//7. AP PIN Code
		memset(tmpstr, 0, sizeof(tmpstr));
		strlcpy(tmpstr, getAPPIN(u), sizeof(tmpstr));
		retval += websWrite(wp, "%s%s%s\n", tag1, tmpstr, tag2);

		//8. Saved WPAKey
#if 0	//hide for security
		if (!safe_strlen(nvram_pf_safe_get(prefix, "wpa_psk")))
			retval += websWrite(wp, "%s%s%s\n", tag1, "None", tag2);
		else
		{
			char_to_ascii(tmpstr, nvram_pf_safe_get(prefix, "wpa_psk"));
			retval += websWrite(wp, "%s%s%s\n", tag1, tmpstr, tag2);
		}
#else
		retval += websWrite(wp, "%s%s\n", tag1, tag2);
#endif
		//9. WPS enable?
		if (nvram_pf_match(prefix, "wps_mode", "enabled"))
			retval += websWrite(wp, "%s%s%s\n", tag1, "None", tag2);
		else
			retval += websWrite(wp, "%s%s%s\n", tag1, nvram_safe_get("wps_enable"), tag2);

		//A. WPS mode
		wps_sta_pin = nvram_safe_get("wps_sta_pin");
		if (safe_strlen(wps_sta_pin) && strcmp(wps_sta_pin, "00000000"))
			retval += websWrite(wp, "%s%s%s\n", tag1, "1", tag2);
		else
			retval += websWrite(wp, "%s%s%s\n", tag1, "2", tag2);

		//B. current auth mode
		if (!safe_strlen(nvram_pf_safe_get(prefix, "auth_mode_x")))
			retval += websWrite(wp, "%s%s%s\n", tag1, "None", tag2);
		else
			retval += websWrite(wp, "%s%s%s\n", tag1, nvram_pf_safe_get(prefix, "auth_mode_x"), tag2);

		//C. WPS band
		retval += websWrite(wp, "%s%d%s\n", tag1, u, tag2);
#if defined(RTCONFIG_WPSMULTIBAND)
	}
#endif

	retval += websWrite(wp, "</wps>");

	return retval;
}

int
ej_wps_info(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_wps_info(eid, wp, argc, argv, WL_5G_BAND);
}

int
ej_wps_info_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_wps_info(eid, wp, argc, argv, WL_2G_BAND);
}

int
ej_wps_info_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_wps_info(eid, wp, argc, argv, WL_5G_BAND);
}

int
ej_wps_info_5g_2(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_wps_info(eid, wp, argc, argv, -1);
}

int
ej_wps_info_6g(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(RTCONFIG_HAS_6G)
	return wl_wps_info(eid, wp, argc, argv, WL_6G_BAND);
#else
	return 0;
#endif
}

int
ej_wps_info_6g_2(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(RTCONFIG_HAS_6G_2)
	return wl_wps_info(eid, wp, argc, argv, 3 /* FIXME: WL_6G_2_BAND */);
#else
	return 0;
#endif
}


// Wireless Client List		 /* Start --Alicia, 08.09.23 */

int ej_wl_auth_list(int eid, webs_t wp, int argc, char_t **argv)
{
	WLANCONFIG_LIST *result;
	#define AUTH_INFO_PATH "/tmp/auth_athX_list"
	FILE *fp;
	int ret = 0;
	char *p, *ifname;
	char *wl_ifnames;
	char line_buf[300]; // max 14x
	char *value;
	int firstRow;
	result = (WLANCONFIG_LIST *)malloc(sizeof(WLANCONFIG_LIST));
	memset(result, 0, sizeof(WLANCONFIG_LIST));

	wl_ifnames = strdup(nvram_safe_get("wl_ifnames"));
	if (!wl_ifnames) 
		return ret;
	
	p = wl_ifnames;

	firstRow = 1;
	while ((ifname = strsep(&p, " ")) != NULL) {
		while (*ifname == ' ') ++ifname;
		if (*ifname == 0) break;
		doSystem("iw dev %s station dump > %s", ifname, AUTH_INFO_PATH);
		fp = fopen(AUTH_INFO_PATH, "r");
		if (fp) {
			while ( fgets(line_buf, sizeof(line_buf), fp) ) {
				if(strstr(line_buf, "Station")) {
					sscanf(line_buf, "%*s%s", result->addr);
					convert_mac_string(result->addr);

					if (firstRow == 1)
						firstRow = 0;
					else
						websWrite(wp, ", ");
					websWrite(wp, "[");

					websWrite(wp, "\"%s\"", result->addr);
					value = "YES";
					websWrite(wp, ", \"%s\"", value);
					value = "";
					websWrite(wp, ", \"%s\"", value);
					websWrite(wp, "]");
				}
			}

			fclose(fp);
			unlink(AUTH_INFO_PATH);
		}
	}
	free(result);
	free(wl_ifnames);
	return ret;
}

#if 0
static void convertToUpper(char *str)
{
	if(str == NULL)
		return;
	while(*str)
	{
		if(*str >= 'a' && *str <= 'z')
		{
			*str &= (unsigned char)~0x20;
		}
		str++;
	}
}
#endif

#if 1
#define target 7
char str[target][40]={"Address:","Frequency:","Quality=","Encryption key:","ESSID:","IE:","Authentication Suites"};
static int wl_scan(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
   	int apCount=0,retval=0;
	char header[128];
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
	char cmd[300];
	FILE *fp;
	char buf[target][200];
	int i,fp_len;
	char *pt1,*pt2;
	char a1[10],a2[10];
	char ssid_str[256];
	char ch[4],ssid[33],address[18],enc[9],auth[16],sig[9],wmode[8];
	int  lock;

	dbg("Please wait...");
	lock = file_lock("nvramcommit");
	system("rm -f /tmp/wlist");
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	snprintf(cmd, sizeof(cmd),"iwlist %s scanning >> /tmp/wlist",nvram_safe_get(strcat_r(prefix, "ifname", tmp)));
	system(cmd);
	file_unlock(lock);
	
	if((fp= fopen("/tmp/wlist", "r"))==NULL) 
	   return -1;
	
	memset(header, 0, sizeof(header));
	snprintf(header, sizeof(header), "%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode");

	dbg("\n%s", header);

	retval += websWrite(wp, "[");
	while(1)
	{
		memset(buf,0,sizeof(buf));
		fp_len=0;
		for(i=0;i<target;i++)
		{
		   	while(fgets(buf[i], sizeof(buf[i]), fp))
			{
				fp_len += safe_strlen(buf[i]);  	
				if(i!=0 && strstr(buf[i],"Cell") && strstr(buf[i],"Address"))
				{
					fseek(fp,-fp_len, SEEK_CUR);
					fp_len=0;
					break;
				}
				else
			  	{ 	   
					if(strstr(buf[i],str[i]))
					{
					 	fp_len =0;
						break;
					}	
					else
						memset(buf[i],0,sizeof(buf[i]));
				}	

			}
		        	
	      		//dbg("buf[%d]=%s\n",i,buf[i]);
		}

  		if(feof(fp)) 
		   break;

		apCount++;

		dbg("\napCount=%d\n",apCount);
		//ch
	        pt1 = strstr(buf[1], "Channel ");
		if(pt1)
		{
			pt2 = strstr(pt1,")");
		   	memset(ch,0,sizeof(ch));
			strncpy(ch,pt1+safe_strlen("Channel "),pt2-pt1-safe_strlen("Channel "));
		}   

		//ssid
	        pt1 = strstr(buf[4], "ESSID:");	
		if(pt1)
		{
		   	memset(ssid,0,sizeof(ssid));
			strncpy(ssid,pt1+safe_strlen("ESSID:")+1,safe_strlen(buf[4])-2-(pt1+safe_strlen("ESSID:")+1-buf[4]));
		}   


		//bssid
	        pt1 = strstr(buf[0], "Address: ");	
		if(pt1)
		{
		   	memset(address,0,sizeof(address));
			strncpy(address,pt1+safe_strlen("Address: "),safe_strlen(buf[0])-(pt1+safe_strlen("Address: ")-buf[0])-1);
		}   
	

		//enc
		pt1=strstr(buf[3],"Encryption key:");
		if(pt1)
		{   
			if(strstr(pt1+safe_strlen("Encryption key:"),"on"))
			{  	
				strlcpy(enc, "ENC", sizeof(enc));
		
			} 
			else
				strlcpy(enc, "NONE", sizeof(enc));
		}

		//auth
		memset(auth,0,sizeof(auth));
		strlcpy(auth, "N/A", sizeof(auth));

		//sig
	        pt1 = strstr(buf[2], "Quality=");	
		pt2 = strstr(pt1,"/");
		if(pt1 && pt2)
		{
			memset(sig,0,sizeof(sig));
			memset(a1,0,sizeof(a1));
			memset(a2,0,sizeof(a2));
			strncpy(a1,pt1+safe_strlen("Quality="),pt2-pt1-safe_strlen("Quality="));
			strncpy(a2,pt2+1,strstr(pt2," ")-(pt2+1));
			snprintf(sig, sizeof(sig), "%d", safe_atoi(a1)/safe_atoi(a2));
		}   

		//wmode
		memset(wmode,0,sizeof(wmode));
		strlcpy(wmode, "11b/g/n", sizeof(wmode));


#if 1
		dbg("%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n",ch,ssid,address,enc,auth,sig,wmode);
#endif	


		memset(ssid_str, 0, sizeof(ssid_str));
		char_to_ascii(ssid_str, trim_r(ssid));
		if (apCount==1)
			retval += websWrite(wp, "[\"%s\", \"%s\"]", ssid_str, address);
		else
			retval += websWrite(wp, ", [\"%s\", \"%s\"]", ssid_str, address);

	}

	retval += websWrite(wp, "]");
	fclose(fp);
	return 0;
}   
#else
static int wl_scan(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
	int retval = 0, i = 0, apCount = 0;
	char data[8192];
	char ssid_str[256];
	char header[128];
	struct iwreq wrq;
	SSA *ssap;
	char tmp[128], prefix[] = "wlXXXXXXXXXX_";
	int lock;

	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	memset(data, 0x00, 255);
	strcpy(data, "SiteSurvey=1"); 
	wrq.u.data.length = strlen(data)+1; 
	wrq.u.data.pointer = data; 
	wrq.u.data.flags = 0; 

	lock = file_lock("nvramcommit");
	if (wl_ioctl(nvram_safe_get(strcat_r(prefix, "ifname", tmp)), RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);
		dbg("Site Survey fails\n");
		return 0;
	}
	file_unlock(lock);
	dbg("Please wait");
	sleep(1);
	dbg(".");
	sleep(1);
	dbg(".");
	sleep(1);
	dbg(".");
	sleep(1);
	dbg(".\n\n");
	memset(data, 0, 8192);
	strcpy(data, "");
	wrq.u.data.length = 8192;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;
	if (wl_ioctl(nvram_safe_get(strcat_r(prefix, "ifname", tmp)), RTPRIV_IOCTL_GSITESURVEY, &wrq) < 0)
	{
		dbg("errors in getting site survey result\n");
		return 0;
	}
	memset(header, 0, sizeof(header));
	//snprintf(header, sizeof(header), "%-3s%-33s%-18s%-8s%-15s%-9s%-8s%-2s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode", "NT");
#if 0// defined(RTN14U)
	snprintf(header, sizeof(header), "%-4s%-33s%-18s%-9s%-16s%-9s%-8s%-4s%-5s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode"," WPS", " DPID");
#else
	snprintf(header, sizeof(header), "%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n", "Ch", "SSID", "BSSID", "Enc", "Auth", "Siganl(%)", "W-Mode");
#endif
	dbg("\n%s", header);
	if (wrq.u.data.length > 0)
	{
#if defined(RTN65U)
		if (unit == 0 && get_model() == MODEL_RTN65U)
		{
			char *encryption;
			SITE_SURVEY_RT3352_iNIC *pSsap, *ssAP;

			pSsap = ssAP = (SITE_SURVEY_RT3352_iNIC *) (1 /* '\n' */ + wrq.u.data.pointer +  sizeof(SITE_SURVEY_RT3352_iNIC) /* header */);
			while(((unsigned int)wrq.u.data.pointer + wrq.u.data.length) > (unsigned int) ssAP)
			{
				ssAP->channel   [sizeof(ssAP->channel)    -1] = '\0';
				ssAP->ssid      [32                         ] = '\0';
				ssAP->bssid     [17                         ] = '\0';
				ssAP->encryption[sizeof(ssAP->encryption) -1] = '\0';
				if((encryption = strchr(ssAP->authmode, '/')) != NULL)
				{
					memmove(ssAP->encryption, encryption +1, sizeof(ssAP->encryption) -1);
					memset(encryption, ' ', sizeof(ssAP->authmode) - (encryption - ssAP->authmode));
					*encryption = '\0';
				}
				ssAP->authmode  [sizeof(ssAP->authmode)   -1] = '\0';
				ssAP->signal    [sizeof(ssAP->signal)     -1] = '\0';
				ssAP->wmode     [sizeof(ssAP->wmode)      -1] = '\0';
				ssAP->extch     [sizeof(ssAP->extch)      -1] = '\0';
				ssAP->nt        [sizeof(ssAP->nt)         -1] = '\0';
				ssAP->wps       [sizeof(ssAP->wps)        -1] = '\0';
				ssAP->dpid      [sizeof(ssAP->dpid)       -1] = '\0';

				convertToUpper(ssAP->bssid);
				ssAP++;
				apCount++;
			}

			if (apCount)
			{
				retval += websWrite(wp, "[");
				for (i = 0; i < apCount; i++)
				{
					dbg("%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n",
						pSsap[i].channel,
						pSsap[i].ssid,
						pSsap[i].bssid,
						pSsap[i].encryption,
						pSsap[i].authmode,
						pSsap[i].signal,
						pSsap[i].wmode
					);

					memset(ssid_str, 0, sizeof(ssid_str));
					char_to_ascii(ssid_str, trim_r(pSsap[i].ssid));

					if (!i)
						retval += websWrite(wp, "[\"%s\", \"%s\"]", ssid_str, pSsap[i].bssid);
					else
						retval += websWrite(wp, ", [\"%s\", \"%s\"]", ssid_str, pSsap[i].bssid);
				}
				retval += websWrite(wp, "]");
				dbg("\n");
			}
			else
				retval += websWrite(wp, "[]");
			return retval;
		}
#endif
		ssap=(SSA *)(wrq.u.data.pointer+strlen(header)+1);
		int len = strlen(wrq.u.data.pointer+strlen(header))-1;
		char *sp, *op;
 		op = sp = wrq.u.data.pointer+strlen(header)+1;
		while (*sp && ((len - (sp-op)) >= 0))
		{
			ssap->SiteSurvey[i].channel[3] = '\0';
			ssap->SiteSurvey[i].ssid[32] = '\0';
			ssap->SiteSurvey[i].bssid[17] = '\0';
			ssap->SiteSurvey[i].encryption[8] = '\0';
			ssap->SiteSurvey[i].authmode[15] = '\0';
			ssap->SiteSurvey[i].signal[8] = '\0';
			ssap->SiteSurvey[i].wmode[7] = '\0';
#if 0//defined(RTN14U)
			ssap->SiteSurvey[i].wps[3] = '\0';
			ssap->SiteSurvey[i].dpid[4] = '\0';
#endif
			sp+=strlen(header);
			apCount=++i;
		}
		if (apCount)
		{
			retval += websWrite(wp, "[");
			for (i = 0; i < apCount; i++)
			{
			   	dbg("\napCount=%d\n",i);
				dbg(
#if 0//defined(RTN14U)
				"%-4s%-33s%-18s%-9s%-16s%-9s%-8s%-4s%-5s\n",
#else
				"%-4s%-33s%-18s%-9s%-16s%-9s%-8s\n",
#endif
					ssap->SiteSurvey[i].channel,
					(char*)ssap->SiteSurvey[i].ssid,
					ssap->SiteSurvey[i].bssid,
					ssap->SiteSurvey[i].encryption,
					ssap->SiteSurvey[i].authmode,
					ssap->SiteSurvey[i].signal,
					ssap->SiteSurvey[i].wmode
#if 0//defined(RTN14U)
					, ssap->SiteSurvey[i].wps
					, ssap->SiteSurvey[i].dpid
#endif
				);

				memset(ssid_str, 0, sizeof(ssid_str));
				char_to_ascii(ssid_str, trim_r(ssap->SiteSurvey[i].ssid));

				if (!i)
//					retval += websWrite(wp, "\"%s\"", ssap->SiteSurvey[i].bssid);
					retval += websWrite(wp, "[\"%s\", \"%s\"]", ssid_str, ssap->SiteSurvey[i].bssid);
				else
//					retval += websWrite(wp, ", \"%s\"", ssap->SiteSurvey[i].bssid);
					retval += websWrite(wp, ", [\"%s\", \"%s\"]", ssid_str, ssap->SiteSurvey[i].bssid);
			}
			retval += websWrite(wp, "]");
			dbg("\n");
		}
		else
			retval += websWrite(wp, "[]");
	}
	return retval;
}

#endif
int
ej_wl_scan(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_scan(eid, wp, argc, argv, WL_2G_BAND);
}

int
ej_wl_scan_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_scan(eid, wp, argc, argv, WL_2G_BAND);
}

int
ej_wl_scan_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_scan(eid, wp, argc, argv, WL_5G_BAND);
}

int
ej_wl_scan_5g_2(int eid, webs_t wp, int argc, char_t **argv)
{
	return wl_scan(eid, wp, argc, argv, -1);
}

int display_channel(int bw, char *channel)
{
	char country_code[3];

	snprintf(country_code, sizeof(country_code), "%s",
		nvram_safe_get("wl_country_code"));

	if(bw == 80){
		if(strcmp(country_code, "GB") == 0){
			if(!strcmp(channel, "116") || !strcmp(channel, "132") ||
				 !strcmp(channel, "136") || !strcmp(channel, "140")){
				return 0;
			}
		}else{
			if(!strcmp(channel, "165") || !strcmp(channel, "140")){
				return 0;
			}
		}
	}else if(bw == 40){
		if(strcmp(country_code, "GB") == 0){
			if(!strcmp(channel, "116") || !strcmp(channel, "140")){
				return 0;
			}
		}
	}
	return 1;
}

static int ej_wl_channel_list(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
	int retval = 0;
	int k = 0;
	char chList[256];
	char buffer[128];	/* Temporary buffer */
	char* chan;

	get_channel_list_via_driver(unit, buffer, (int)sizeof(buffer));
	memset(chList, 0, sizeof(chList));
	if(unit){
		chan = strtok(buffer, ",");
		while(chan) {
			if(display_channel(80, chan) == 1){
				if(k != 0){
					strlcat(chList, ",", sizeof(chList));
				}
				strlcat(chList, chan, sizeof(chList));
				k++;
			}
			chan = strtok(NULL, ",");
		}
	}else
		strlcat(chList, buffer, sizeof(chList));

	return websWrite(wp, "[%s]", chList);
}

int
ej_wl_channel_list_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	return ej_wl_channel_list(eid, wp, argc, argv, WL_2G_BAND);
}

int
ej_wl_channel_list_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	/* show 80MHz channel list */
	return ej_wl_channel_list(eid, wp, argc, argv, WL_5G_BAND);
}

int
ej_wl_channel_list_5g_2(int eid, webs_t wp, int argc, char_t **argv)
{
	return ej_wl_channel_list(eid, wp, argc, argv, -1);
}

int
ej_wl_channel_list_6g(int eid, webs_t wp, int argc, char_t **argv)
{
	return ej_wl_channel_list(eid, wp, argc, argv, -1);
}

int get_wl_channel_list_by_bw_core(int unit, char *ch_list, size_t len, int bw)
{
	int k = 0;
	char buffer[128];	/* Temporary buffer */
	char* chan;

	get_channel_list_via_driver(unit, buffer, (int)sizeof(buffer));

	strlcat(ch_list, "[", len);
	chan = strtok(buffer, ",");
	while(chan) {
		if(display_channel(bw, chan) == 1){
			if(k != 0){
				strlcat(ch_list, ",", len);
			}
			strlcat(ch_list, chan, len);
			k++;
		}
		chan = strtok(NULL, ",");
	}
	strcat(ch_list, "]");
	return 0;
}

int ej_wl_channel_list_5g_20m(int eid, webs_t wp, int argc, char_t **argv)
{
	int retval = 0;
	static char ch_list[256] = {0};
	int check_bit;

	if( (nvram_get_int("wl_country_changed") & 0x1) == 0x1){
		check_bit = nvram_get_int("wl_country_changed") ^ 0x1 ;
		nvram_set_int("wl_country_changed", check_bit);
		memset(ch_list, 0, sizeof(ch_list));
	}

	if(strlen(ch_list) == 0) retval = get_wl_channel_list_by_bw_core(1, ch_list, sizeof(ch_list), 20);
	retval += websWrite(wp, ch_list);
	return retval;
}

int ej_wl_channel_list_5g_40m(int eid, webs_t wp, int argc, char_t **argv)
{
	int retval = 0;
	static char ch_list[256] = {0};
	int check_bit;

	if( (nvram_get_int("wl_country_changed") & 0x2) == 0x2){
		check_bit = nvram_get_int("wl_country_changed") ^ 0x2 ;
		nvram_set_int("wl_country_changed", check_bit);
		memset(ch_list, 0, sizeof(ch_list));
	}

	if(strlen(ch_list) == 0) retval = get_wl_channel_list_by_bw_core(1, ch_list, sizeof(ch_list), 40);
	retval += websWrite(wp, ch_list);
	return retval;
}

int ej_wl_channel_list_5g_80m(int eid, webs_t wp, int argc, char_t **argv)
{
	int retval = 0;
	static char ch_list[256] = {0};
	int check_bit;

	if( (nvram_get_int("wl_country_changed") & 0x4) == 0x4){
		check_bit = nvram_get_int("wl_country_changed") ^ 0x4;
		nvram_set_int("wl_country_changed", check_bit);
		memset(ch_list, 0, sizeof(ch_list));
	}

	if(strlen(ch_list) == 0) retval = get_wl_channel_list_by_bw_core(1, ch_list, sizeof(ch_list), 80);
	retval += websWrite(wp, ch_list);
	return retval;
}

static const struct g_bw40chanspec_s {
	uint64_t ch_mask;
	uint64_t bw_mask;
	char *tag;
} g_5gbw40chanspec_tbl[] = {
	{  CH40_M,  CH36_M |  CH40_M, "u" },
	{  CH48_M,  CH44_M |  CH48_M, "u" },
	{  CH56_M,  CH52_M |  CH56_M, "u" },
	{  CH64_M,  CH60_M |  CH64_M, "u" },
	{ CH104_M, CH100_M | CH104_M, "u" },
	{ CH112_M, CH108_M | CH112_M, "u" },
	{ CH120_M, CH116_M | CH120_M, "u" },
	{ CH128_M, CH124_M | CH128_M, "u" },
	{ CH136_M, CH132_M | CH136_M, "u" },
	{ CH144_M, CH140_M | CH144_M, "u" },
	{ CH153_M, CH149_M | CH153_M, "u" },
	{ CH161_M, CH157_M | CH161_M, "u" },
	{ CH169_M, CH165_M | CH169_M, "u" },
	{ CH177_M, CH173_M | CH177_M, "u" },

	{  CH36_M,  CH36_M |  CH40_M, "l" },
	{  CH44_M,  CH44_M |  CH48_M, "l" },
	{  CH52_M,  CH52_M |  CH56_M, "l" },
	{  CH60_M,  CH60_M |  CH64_M, "l" },
	{ CH100_M, CH100_M | CH104_M, "l" },
	{ CH108_M, CH108_M | CH112_M, "l" },
	{ CH116_M, CH116_M | CH120_M, "l" },
	{ CH124_M, CH124_M | CH128_M, "l" },
	{ CH132_M, CH132_M | CH136_M, "l" },
	{ CH140_M, CH140_M | CH144_M, "l" },
	{ CH149_M, CH149_M | CH153_M, "l" },
	{ CH157_M, CH157_M | CH161_M, "l" },
	{ CH165_M, CH165_M | CH169_M, "l" },
	{ CH173_M, CH173_M | CH177_M, "l" },

	{ 0, 0, NULL }
};

static const struct g_bw80pchanspec_s {
	uint64_t bw_mask;
	char *tag;
} g_5gbw80pchanspec_tbl[] = {
	{  CH36_M |  CH40_M |  CH44_M |  CH48_M, "/80" },
	{  CH52_M |  CH56_M |  CH60_M |  CH64_M, "/80" },
	{ CH100_M | CH104_M | CH108_M | CH112_M, "/80" },
	{ CH116_M | CH120_M | CH124_M | CH128_M, "/80" },
	{ CH132_M | CH136_M | CH140_M | CH144_M, "/80" },
	{ CH149_M | CH153_M | CH157_M | CH161_M, "/80" },
	{ CH165_M | CH169_M | CH173_M | CH177_M, "/80" },
#if defined(RTCONFIG_BW160M)
	{  CH36_M |  CH40_M |  CH44_M |  CH48_M |  CH52_M |  CH56_M |  CH60_M |  CH64_M, "/160" },
	{ CH100_M | CH104_M | CH108_M | CH112_M | CH116_M | CH120_M | CH124_M | CH128_M, "/160" },
	{ CH149_M | CH153_M | CH157_M | CH161_M | CH165_M | CH169_M | CH173_M | CH177_M, "/160" },
#endif
#if defined(RTCONFIG_BW240M)
	{ CH100_M | CH104_M | CH108_M | CH112_M | CH116_M | CH120_M | CH124_M | CH128_M | CH132_M | CH136_M | CH140_M | CH144_M, "/240" },
#endif

	{ 0, NULL },
#if defined(RTCONFIG_HAS_6G)
}, g_6gbw40pchanspec_tbl[] = {
	{   B6GCH1_M |   B6GCH5_M, "/40" },
	{   B6GCH9_M |  B6GCH13_M, "/40" },
	{  B6GCH17_M |  B6GCH21_M, "/40" },
	{  B6GCH25_M |  B6GCH29_M, "/40" },
	{  B6GCH33_M |  B6GCH37_M, "/40" },
	{  B6GCH41_M |  B6GCH45_M, "/40" },
	{  B6GCH49_M |  B6GCH53_M, "/40" },
	{  B6GCH57_M |  B6GCH61_M, "/40" },
	{  B6GCH65_M |  B6GCH69_M, "/40" },
	{  B6GCH73_M |  B6GCH77_M, "/40" },
	{  B6GCH81_M |  B6GCH85_M, "/40" },
	{  B6GCH89_M |  B6GCH93_M, "/40" },
	{  B6GCH97_M | B6GCH101_M, "/40" },
	{ B6GCH105_M | B6GCH109_M, "/40" },
	{ B6GCH113_M | B6GCH117_M, "/40" },
	{ B6GCH121_M | B6GCH125_M, "/40" },
	{ B6GCH129_M | B6GCH133_M, "/40" },
	{ B6GCH137_M | B6GCH141_M, "/40" },
	{ B6GCH145_M | B6GCH149_M, "/40" },
	{ B6GCH153_M | B6GCH157_M, "/40" },
	{ B6GCH161_M | B6GCH165_M, "/40" },
	{ B6GCH169_M | B6GCH173_M, "/40" },
	{ B6GCH177_M | B6GCH181_M, "/40" },
	{ B6GCH185_M | B6GCH189_M, "/40" },
	{ B6GCH193_M | B6GCH197_M, "/40" },
	{ B6GCH201_M | B6GCH205_M, "/40" },
	{ B6GCH209_M | B6GCH213_M, "/40" },
	{ B6GCH217_M | B6GCH221_M, "/40" },
	{ B6GCH225_M | B6GCH229_M, "/40" },

	{   B6GCH1_M |   B6GCH5_M |   B6GCH9_M |  B6GCH13_M, "/80" },
	{  B6GCH17_M |  B6GCH21_M |  B6GCH25_M |  B6GCH29_M, "/80" },
	{  B6GCH33_M |  B6GCH37_M |  B6GCH41_M |  B6GCH45_M, "/80" },
	{  B6GCH49_M |  B6GCH53_M |  B6GCH57_M |  B6GCH61_M, "/80" },
	{  B6GCH65_M |  B6GCH69_M |  B6GCH73_M |  B6GCH77_M, "/80" },
	{  B6GCH81_M |  B6GCH85_M |  B6GCH89_M |  B6GCH93_M, "/80" },
	{  B6GCH97_M | B6GCH101_M | B6GCH105_M | B6GCH109_M, "/80" },
	{ B6GCH113_M | B6GCH117_M | B6GCH121_M | B6GCH125_M, "/80" },
	{ B6GCH129_M | B6GCH133_M | B6GCH137_M | B6GCH141_M, "/80" },
	{ B6GCH145_M | B6GCH149_M | B6GCH153_M | B6GCH157_M, "/80" },
	{ B6GCH161_M | B6GCH165_M | B6GCH169_M | B6GCH173_M, "/80" },
	{ B6GCH177_M | B6GCH181_M | B6GCH185_M | B6GCH189_M, "/80" },
	{ B6GCH193_M | B6GCH197_M | B6GCH201_M | B6GCH205_M, "/80" },
	{ B6GCH209_M | B6GCH213_M | B6GCH217_M | B6GCH221_M, "/80" },

#if defined(RTCONFIG_BW160M)
	{   B6GCH1_M |   B6GCH5_M |   B6GCH9_M |  B6GCH13_M |  B6GCH17_M |  B6GCH21_M |  B6GCH25_M |  B6GCH29_M, "/160" },
	{  B6GCH33_M |  B6GCH37_M |  B6GCH41_M |  B6GCH45_M |  B6GCH49_M |  B6GCH53_M |  B6GCH57_M |  B6GCH61_M, "/160" },
	{  B6GCH65_M |  B6GCH69_M |  B6GCH73_M |  B6GCH77_M |  B6GCH81_M |  B6GCH85_M |  B6GCH89_M |  B6GCH93_M, "/160" },
	{  B6GCH97_M | B6GCH101_M | B6GCH105_M | B6GCH109_M | B6GCH113_M | B6GCH117_M | B6GCH121_M | B6GCH125_M, "/160" },
	{ B6GCH129_M | B6GCH133_M | B6GCH137_M | B6GCH141_M | B6GCH145_M | B6GCH149_M | B6GCH153_M | B6GCH157_M, "/160" },
	{ B6GCH161_M | B6GCH165_M | B6GCH169_M | B6GCH173_M | B6GCH177_M | B6GCH181_M | B6GCH185_M | B6GCH189_M, "/160" },
	{ B6GCH193_M | B6GCH197_M | B6GCH201_M | B6GCH205_M | B6GCH209_M | B6GCH213_M | B6GCH217_M | B6GCH221_M, "/160" },
#endif

	{   B6GCH1_M |   B6GCH5_M |   B6GCH9_M |  B6GCH13_M |  B6GCH17_M |  B6GCH21_M |  B6GCH25_M |  B6GCH29_M |  B6GCH33_M |  B6GCH37_M |  B6GCH41_M |  B6GCH45_M |  B6GCH49_M |  B6GCH53_M |  B6GCH57_M |  B6GCH61_M, "/320-1" },
	{  B6GCH33_M |  B6GCH37_M |  B6GCH41_M |  B6GCH45_M |  B6GCH49_M |  B6GCH53_M |  B6GCH57_M |  B6GCH61_M |  B6GCH65_M |  B6GCH69_M |  B6GCH73_M |  B6GCH77_M |  B6GCH81_M |  B6GCH85_M |  B6GCH89_M |  B6GCH93_M, "/320-2" },
	{  B6GCH65_M |  B6GCH69_M |  B6GCH73_M |  B6GCH77_M |  B6GCH81_M |  B6GCH85_M |  B6GCH89_M |  B6GCH93_M |  B6GCH97_M | B6GCH101_M | B6GCH105_M | B6GCH109_M | B6GCH113_M | B6GCH117_M | B6GCH121_M | B6GCH125_M, "/320-1" },
	{  B6GCH97_M | B6GCH101_M | B6GCH105_M | B6GCH109_M | B6GCH113_M | B6GCH117_M | B6GCH121_M | B6GCH125_M | B6GCH129_M | B6GCH133_M | B6GCH137_M | B6GCH141_M | B6GCH145_M | B6GCH149_M | B6GCH153_M | B6GCH157_M, "/320-2" },
	{ B6GCH129_M | B6GCH133_M | B6GCH137_M | B6GCH141_M | B6GCH145_M | B6GCH149_M | B6GCH153_M | B6GCH157_M | B6GCH161_M | B6GCH165_M | B6GCH169_M | B6GCH173_M | B6GCH177_M | B6GCH181_M | B6GCH185_M | B6GCH189_M, "/320-1" },
	{ B6GCH161_M | B6GCH165_M | B6GCH169_M | B6GCH173_M | B6GCH177_M | B6GCH181_M | B6GCH185_M | B6GCH189_M | B6GCH193_M | B6GCH197_M | B6GCH201_M | B6GCH205_M | B6GCH209_M | B6GCH213_M | B6GCH217_M | B6GCH221_M, "/320-2" },

	{ 0, NULL },
#endif
};

int ej_wl_chanspecs(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
	int retval = 0;
	char *ch_prefix = "";
	char ch_list[2 * 4 + 3 * 22 + 4 * 34 + 8] = "";
	char tmp_ch_list[sizeof("6gXXX\", \"") * 59] = "";
	uint64_t m, c_m, bw_m, chlist_mask = 0;
	const struct g_bw40chanspec_s *p = NULL;
	const struct g_bw80pchanspec_s *q = NULL;

	if (unit < 0 || unit >= WL_NR_BANDS)
		goto exit_ej_wl_chanspecs;

	if (get_channel_list_via_driver(unit, ch_list, sizeof(ch_list)) <= 0)
		goto exit_ej_wl_chanspecs;

	if (!(chlist_mask = chlist2bitmask(unit, ch_list, ",")))
		goto exit_ej_wl_chanspecs;

	if (unit == WL_6G_BAND) {
		ch_prefix = "6g";
	}

	/* Generate 20MHz channelspecslist. */
#if defined(RTCONFIG_HAS_6G)
	if (unit == WL_6G_BAND)
		__bitmask2chlist(unit, chlist_mask, "\", \"6g", tmp_ch_list, sizeof(tmp_ch_list));
	else
#endif
		__bitmask2chlist(unit, chlist_mask, "\", \"", tmp_ch_list, sizeof(tmp_ch_list));
	retval += websWrite(wp, "[ \"%s%s", ch_prefix, tmp_ch_list);

	/* Generate 40MHz,u 40MHz,l 80MHz, 160MHz channelspecs. */
	if (unit == WL_2G_BAND) {
		/* If ch-4 exist, print ch and append "u".
		 * If ch+4 exist, print ch and append "l".
		 */
		for (m = (1 << 4), c_m = chlist_mask; c_m && m; m <<= 1) {
			if (!(chlist_mask & m) || !(chlist_mask & (m >> 4)))
				continue;

			retval += websWrite(wp, "\", \"%su", bitmask2chlist(unit, m, ""));
			c_m &= ~m;
		}
		for (m = 1, c_m = chlist_mask; c_m && m; m <<= 1) {
			if (!(chlist_mask & m) || !(chlist_mask & (m << 4)))
				continue;

			retval += websWrite(wp, "\", \"%sl", bitmask2chlist(unit, m, ""));
			c_m &= ~m;
		}
	} else if (unit == WL_5G_BAND
#if defined(RTCONFIG_HAS_5G_2)
		|| unit == WL_5G_2_BAND
#endif
	) {
		p = g_5gbw40chanspec_tbl;
#if defined(RTCONFIG_HAS_6G)
	} else if (unit == WL_6G_BAND) {
		p = NULL;	/* u,l format is replaced by /40 */
#endif
	} else {
		_dprintf("%s: 40MHz chanspec of unit %d hasn't been defined!\n", __func__, unit);
	}

	for (; p && p->ch_mask && p->bw_mask && p->tag; ++p) {
		if (!(p->ch_mask & chlist_mask))
			continue;
		if ((p->bw_mask & chlist_mask) != p->bw_mask)
			continue;

		retval += websWrite(wp, "\", \"%s%s%s", ch_prefix, bitmask2chlist(unit, p->ch_mask, ""), p->tag);
	}

	/* Generate 80MHz, 160MHz, 240MHz, 320MHz chanspecs. */
	if (unit == WL_2G_BAND)
		q = NULL;
	else if (unit == WL_5G_BAND
#if defined(RTCONFIG_HAS_5G_2)
	      || unit == WL_5G_2_BAND
#endif
	) {
		q = g_5gbw80pchanspec_tbl;
#if defined(RTCONFIG_HAS_6G)
	} else if (unit == WL_6G_BAND) {
		q = g_6gbw40pchanspec_tbl;
#endif
	} else {
		_dprintf("%s: 80+MHz chanspec of unit %d hasn't been defined!\n", __func__, unit);
	}

	for (; q && q->bw_mask && q->tag; ++q) {
		if ((chlist_mask & q->bw_mask) != q->bw_mask)
			continue;

		for (m = 1, bw_m = q->bw_mask; bw_m && m; m <<= 1) {
			if (!(bw_m & m))
				continue;
			retval += websWrite(wp, "\", \"%s%s%s", ch_prefix, bitmask2chlist(unit, m, ""), q->tag);
			bw_m &= ~m;
		}
	}

exit_ej_wl_chanspecs:
	if (!retval)
		retval += websWrite(wp, "[ \"0\" ]");
	else
		retval += websWrite(wp, "\" ]");

	return retval;
}

int ej_wl_chanspecs_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	return ej_wl_chanspecs(eid, wp, argc, argv, WL_2G_BAND);
}

int ej_wl_chanspecs_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	return ej_wl_chanspecs(eid, wp, argc, argv, WL_5G_BAND);
}

int ej_wl_chanspecs_5g_2(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(RTCONFIG_HAS_5G_2)
	return ej_wl_chanspecs(eid, wp, argc, argv, WL_5G_2_BAND);
#else
	return ej_wl_chanspecs(eid, wp, argc, argv, -1);
#endif
}

int ej_wl_chanspecs_6g(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(RTCONFIG_HAS_6G)
	return ej_wl_chanspecs(eid, wp, argc, argv, WL_6G_BAND);
#else
	return ej_wl_chanspecs(eid, wp, argc, argv, -1);
#endif
}

int ej_wl_chanspecs_6g_2(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(RTCONFIG_HAS_6G_2)
	return ej_wl_chanspecs(eid, wp, argc, argv, WL_6G_2_BAND);
#else
	return ej_wl_chanspecs(eid, wp, argc, argv, -1);
#endif
}

static int ej_wl_rate(int eid, webs_t wp, int argc, char_t **argv, int unit)
{
#define ASUS_IOCTL_GET_STA_DATARATE (SIOCDEVPRIVATE+15) /* from qca-wifi/os/linux/include/ieee80211_ioctl.h */
        struct iwreq wrq;
	int retval = 0;
	char tmp[256], prefix[] = "wlXXXXXXXXXX_";
	char *name;
	char word[256], *next;
	int unit_max = 0;
	unsigned int rate[2];
	char rate_buf[32] = "0 Mbps";
	int sw_mode = sw_mode();
	int wlc_band = nvram_get_int("wlc_band");
	int from_app = 0;

	if (sw_mode != SW_MODE_REPEATER && sw_mode != SW_MODE_HOTSPOT && !wisp_mode())
		goto ERROR;
	if (absent_band(unit))
		goto ERROR;

	from_app = check_user_agent(user_agent);

	if (wlc_band < 0 || !nvram_match("wlc_state", "2"))
		goto ERROR;


	if (wlc_band >= WL_2G_BAND && wlc_band != unit)
		goto ERROR;

	snprintf(prefix, sizeof(prefix), "wl%d.1_", unit);
	name = nvram_pf_safe_get(prefix, "ifname");
#if defined(RTCONFIG_WISP)
	if(wisp_mode())
		name = get_staifname(unit);
#endif

	wrq.u.data.pointer = rate;
	wrq.u.data.length = sizeof(rate);

	if (wl_ioctl(name, ASUS_IOCTL_GET_STA_DATARATE, &wrq) < 0)
	{
		dbg("%s: errors in getting %s ASUS_IOCTL_GET_STA_DATARATE result\n", __func__, name);
		goto ERROR;
	}

	if (rate[0] > rate[1])
		snprintf(rate_buf, sizeof(rate_buf), "%d Mbps", rate[0]);
	else
		snprintf(rate_buf, sizeof(rate_buf), "%d Mbps", rate[1]);

ERROR:
	if(from_app == 0 && hook_get_json == 0)
		retval += websWrite(wp, "%s", rate_buf);
	else
		retval += websWrite(wp, "\"%s\"", rate_buf);
	return retval;
}


int
ej_wl_rate_2g(int eid, webs_t wp, int argc, char_t **argv)
{
	if(sw_mode() == SW_MODE_REPEATER || wisp_mode())
		return ej_wl_rate(eid, wp, argc, argv, WL_2G_BAND);
	else if(check_user_agent(user_agent) != FROM_BROWSER)
		return websWrite(wp, "\"\"");
	else
		return websWrite(wp, "");
}

int
ej_wl_rate_5g(int eid, webs_t wp, int argc, char_t **argv)
{
	if(sw_mode() == SW_MODE_REPEATER || wisp_mode())
		return ej_wl_rate(eid, wp, argc, argv, WL_5G_BAND);
	else if(check_user_agent(user_agent) != FROM_BROWSER)
		return websWrite(wp, "\"\"");
	else
		return websWrite(wp, "");
}

int
ej_wl_rate_5g_2(int eid, webs_t wp, int argc, char_t **argv)
{
	if(sw_mode() == SW_MODE_REPEATER || wisp_mode())
		return ej_wl_rate(eid, wp, argc, argv, -1);
	else if(check_user_agent(user_agent) != FROM_BROWSER)
		return websWrite(wp, "\"\"");
	else
		return websWrite(wp, "");
}


int
ej_wl_rate_6g(int eid, webs_t wp, int argc, char_t **argv)
{
#if defined(RTCONFIG_HAS_6G)
	if(sw_mode() == SW_MODE_REPEATER || wisp_mode())
		return ej_wl_rate(eid, wp, argc, argv, WL_6G_BAND);
	else 
#endif
	if(check_user_agent(user_agent) != FROM_BROWSER)
		return websWrite(wp, "\"\"");
	else
		return websWrite(wp, "");
}

int
ej_nat_accel_status(int eid, webs_t wp, int argc, char_t **argv)
{
	return websWrite(wp, "%d", 1);
}
#if defined(RTCONFIG_WLMODULE_WAV6XX_AP)
/* Hook validate_apply().
 * Sync wl[0~2]_yyy with wlx_yyy if yyy in global_params[].
 */
static const char *global_params[] = { "twt", NULL };
void __validate_apply_set_wl_var(char *nv, char *val)
{
	const char **p;
	int band;
	char prefix[sizeof("wlxxx_")];

	if (!nv || (strncmp(nv, "wl0_", 4) && strncmp(nv, "wl1_", 4)))
		return;

	for (p = &global_params[0]; *p != NULL; ++p) {
		if (strcmp(nv + 4, *p))
			continue;

		for (band = WL_2G_BAND; band < MAX_NR_WL_IF; ++band) {
			snprintf(prefix, sizeof(prefix), "wl%d_", band);
			if (!strncmp(nv, prefix, safe_strlen(prefix)))
				continue;
			nvram_pf_set(prefix, *p, val);
			_dprintf("%s: set %s%s=%s\n", __func__, prefix, *p, val? : "NULL");
		}
		break;
	}
}
#endif

#ifdef RTCONFIG_PROXYSTA
int
ej_wl_auth_psta(int eid, webs_t wp, int argc, char_t **argv)
{
	int retval = 0;
	int psta = 0, psta_auth = 0;

	if(nvram_match("wlc_state", "2")){	//connected
		psta = 1;
		psta_auth = 0;
	//else if(?)				//authorization failed
	//	retval += websWrite(wp, "wlc_state=2;wlc_state_auth=1;");
	}else{					//disconnected
		psta = 0;
		psta_auth = 0;
	}

	if(json_support){
		retval += websWrite(wp, "{");
		retval += websWrite(wp, "\"wlc_state\":\"%d\"", psta);
		retval += websWrite(wp, ",\"wlc_state_auth\":\"%d\"", psta_auth);
		retval += websWrite(wp, "}");
	}else{
		retval += websWrite(wp, "wlc_state=%d;", psta);
		retval += websWrite(wp, "wlc_state_auth=%d;", psta_auth);
	}

	return retval;
}
#endif

