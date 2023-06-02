 /*
 * bcm.c - implements for Broadcom wifi
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
#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <net/if.h>
#include <stdbool.h>
#include <errno.h>

#include "debug.h"
#include "util.h"
#include "wifi.h"
#include "wpactrl_util.h"

#include "broadcom.h"

#ifndef BIT
#define BIT(n)	(1U << (n))
#endif

#define WORKAROUND_BCM_DFS_SCAN		1

int
wl_ether_atoe(const char *a, struct wl_ether_addr *n)
{
	char *c = NULL;
	int i = 0;

	memset(n, 0, ETHER_ADDR_LEN);
	for (;;) {
		n->octet[i++] = (uint8)strtoul(a, &c, 16);
		if (!*c++ || i == ETHER_ADDR_LEN)
			break;
		a = c;
	}
	return (i == ETHER_ADDR_LEN);
}

char *
wl_ether_etoa(const struct wl_ether_addr *n)
{
	static char etoa_buf[ETHER_ADDR_LEN * 3];
	char *c = etoa_buf;
	int i;

	for (i = 0; i < ETHER_ADDR_LEN; i++) {
		if (i)
			*c++ = ':';
		c += sprintf(c, "%02X", n->octet[i] & 0xff);
	}
	return etoa_buf;
}


static int wl_ioctl(const char *ifname, int cmd, void *buf, int len)
{
	struct ifreq ifr;
	wl_ioctl_t wioc = {0};
	int s;

	wioc.cmd = cmd;
	wioc.buf = buf;
	wioc.len = len;

	strncpy(ifr.ifr_name, ifname, IFNAMSIZ);
	ifr.ifr_data = (caddr_t) &wioc;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0)
		return -1;

	fcntl(s, F_SETFD, fcntl(s, F_GETFD) | FD_CLOEXEC);

	if (ioctl(s, SIOCDEVPRIVATE, &ifr) < 0) {
		close(s);
		return -1;
	}

	close(s);
	return 0;
}

static int wl_iovar(const char *ifname, int set, char *iovar,
				void *param, int paramlen,
				void *bufptr, int buflen)
{
	int len;
	int total_len;

	len = strlen(iovar) + 1;
	total_len = len + paramlen;

	if (buflen < total_len)
		return -1;

	snprintf(bufptr, buflen, "%s", iovar);
	memcpy((uint8_t *)bufptr + len, param, paramlen);

	if (set)
		return wl_ioctl(ifname, WLC_SET_VAR, bufptr, total_len);

	return wl_ioctl(ifname, WLC_GET_VAR, bufptr, buflen);
}

int wl_iovar_set(const char *ifname, char *iovar, void *param, int paramlen,
			void *buf, int buflen)
{
	return wl_iovar(ifname, 1, iovar, param, paramlen, buf, buflen);
}

int wl_iovar_get_noparam(const char *ifname, char *iovar, void *bufptr, int buflen)
{
	return wl_iovar(ifname, 0, iovar, NULL, 0, bufptr, buflen);
}

int wl_iovar_get(const char *ifname, char *iovar, void *param, int paramlen,
			void *buf, int buflen)
{
	return wl_iovar(ifname, 0, iovar, param, paramlen, buf, buflen);
}

static int wl_swap(const char *ifname)
{
	int val = 0;

	if (wl_ioctl(ifname, WLC_GET_MAGIC, &val, sizeof(int)) < 0)
		return 0; /* don't swap on error */

	/* is endian swap needed */
	if (val == (int)BCMSWAP32(WLC_IOCTL_MAGIC))
		return 1;

	return 0;
}

#if 0	// TODO: remove
static int bcm_get_ifstatus(const char *ifname, uint32_t *buf)
{
	unsigned int isup;
	int swap = 0;

	swap = wl_swap(ifname);

	if (wl_ioctl(ifname, WLC_GET_UP, &isup, sizeof(isup)) < 0)
		isup = 0;

	*buf = swap ? BCMSWAP32(isup) : isup;

	return 0;
}
#endif

int bcm_get_isap(char *ifname, int *buf)
{
	unsigned int isap;
	int swap = 0;

	swap = wl_swap(ifname);

	if (wl_ioctl(ifname, WLC_GET_AP, &isap, sizeof(isap)) < 0)
		return -1;

	*buf = swap ? BCMSWAP32(isap) : isap;

	return 0;
}

static int bcm_get_oper_band(const char *ifname, enum wifi_band *band)
{
	unsigned int b = 0;
	int swap = 0;

	swap = wl_swap(ifname);

	if (wl_ioctl(ifname, WLC_GET_BAND, &b, sizeof(b)) < 0) {
		*band = BAND_UNKNOWN;
		return -1;
	}

	b = swap ? BCMSWAP32(b) : b ;

	switch (b) {
	case 1:
		*band = BAND_5;
		break;
	case 2:
		*band = BAND_2;
		break;
	case 0:
		*band = BAND_DUAL;
		break;
	default:
		*band = BAND_UNKNOWN;
		break;
	}

	return 0;
}

static int bcm_get_country(const char *ifname, char *alpha2)
{
	wl_country_t cc;

	if (!alpha2)
		return -1;

	memset(&cc, 0, sizeof(cc));
	if (wl_iovar_get_noparam(ifname, "country", &cc, sizeof(cc)) < 0)
		return -1;

	snprintf(alpha2, 4, "%s", cc.country_abbrev);
	return 0;
}

int wl_format_ssid(char *ssid_buf, uint8_t *ssid, int ssid_len)
{
	int i, c;
	char *p = ssid_buf;

	if (ssid_len > 32)
		ssid_len = 32;

	for (i = 0; i < ssid_len; i++) {
		c = (int)ssid[i];
		if (c == '\\') {
			*p++ = '\\';
			*p++ = '\\';
		} else if (isprint((uchar)c)) {
			*p++ = (char)c;
		} else {
			p += sprintf(p, "\\x%02X", c);
		}
	}
	*p = '\0';

	return p - ssid_buf;
}

int bcm_get_bitrate(char *ifname, unsigned long *buf)
{
	int rate = 0;
	int swap = 0;

	swap = wl_swap(ifname);
	if (wl_ioctl(ifname, WLC_GET_RATE, &rate, sizeof(rate)) < 0)
		rate = 0;

	*buf = swap ? BCMSWAP32(rate) : rate;

	*buf /= 2;

	return 0;
}

static int bcm_get_maxrate(const char *ifname, unsigned long *out)
{
	int ioctl_req_version = 0x2000;
	wl_bss_info_t *bi;
	chanspec_t chspec;
	uint16_t vht_rxmap;
	int swap = 0;
	uint8_t supp_mcs[2] = {0};
	int bandwidth = 20;
	float maxrate = 54.0;
	int nss = 0;
	int octet = 0;
	int max_mcs = -1;
	int sgi = 1;
	char *tmp;
	int l;

	swap = wl_swap(ifname);
	tmp = malloc(WLC_IOCTL_MAXLEN * sizeof(char));
	if (tmp == NULL)
		return -1;

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));
	if (wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN) < 0) {
		free(tmp);
		return -1;
	}

	bi = (wl_bss_info_t *)(tmp + 4);
	chspec = bi->chanspec;
	if (CHSPEC_IS160(chspec) || CHSPEC_IS8080(chspec))
		bandwidth = 160;
	else if (CHSPEC_IS80(chspec))
		bandwidth = 80;
	else if (CHSPEC_IS40(chspec))
		bandwidth = 40;

	vht_rxmap = swap ? BCMSWAP16(bi->vht_rxmcsmap) : bi->vht_rxmcsmap;
	if (bi->vht_cap) {
		*((uint16_t *)supp_mcs) = vht_rxmap;

		for (l = 0; l < 16; l += 2) {
			uint8_t supp_mcs_mask = 0;

			if (l && !(l % 8))
				octet++;

			supp_mcs_mask = supp_mcs[octet] & (0x3 << (l % 8));
			supp_mcs_mask >>= (l % 8);
			if (supp_mcs_mask == 3)
				break;

			nss++;
			if (supp_mcs_mask == 0)
				max_mcs = 7;
			else if (supp_mcs_mask == 1)
				max_mcs = 8;
			else if (supp_mcs_mask == 2)
				max_mcs = 9;
		}
		maxrate = wifi_mcs2rate(max_mcs, bandwidth, nss, sgi ? WIFI_SGI : WIFI_LGI);
	} else if (bi->n_cap) {
		int i;
		int bit_i = 0;
		int more = 1;

		for (i = 0; i < MCSSET_LEN && more; i++) {
			for (bit_i = 0; bit_i < 8; bit_i++) {
				if (!(bi->basic_mcs[i] & (1 << bit_i))) {
					more = 0;
					break;
				}
				max_mcs++;
			}
		}

		nss = (max_mcs / 8) + 1;
		max_mcs %= 8;
		maxrate = wifi_mcs2rate(max_mcs, bandwidth, nss, sgi ? WIFI_SGI : WIFI_LGI);
	}

	*out = (unsigned long)maxrate;     /* in Mbps */
	free(tmp);
	return 0;
}

static int bcm_get_noise(const char *ifname, int *buf)
{
	unsigned int noise;
	int swap = 0;

	swap = wl_swap(ifname);

	if (wl_ioctl(ifname, WLC_GET_PHY_NOISE, &noise, sizeof(noise)) < 0)
		noise = 0;

	*buf = swap ? BCMSWAP32(noise) : noise;

	return 0;
}

static int bcm_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	unsigned int wpa_auth = 0;
	unsigned int wsec = 0;
	int swap = 0;

	*auth = 0;
	*enc = 0;
	swap = wl_swap(ifname);

	if (wl_ioctl(ifname, WLC_GET_WPA_AUTH, &wpa_auth,
					sizeof(wpa_auth)) < 0) {
		*auth = AUTH_UNKNOWN;
		*enc = CIPHER_UNKNOWN;
		return -1;
	}

	if (wl_ioctl(ifname, WLC_GET_WSEC, &wsec, sizeof(wsec)) < 0) {
		*enc = CIPHER_UNKNOWN;
		return -1;
	}

	wpa_auth = swap ? BCMSWAP32(wpa_auth) : wpa_auth;
	wsec = swap ? BCMSWAP32(wsec) : wsec;

	/* libwifi_dbg("wpa_auth = 0x%x wsec = 0x%x\n", wpa_auth, wsec); */
	if (!!(wsec & 0x1)) {
		*enc = CIPHER_WEP;
		*auth = AUTH_OPEN;
		return 0;
	}

	if (wpa_auth == WPA_AUTH_DISABLED) {
		*auth = AUTH_OPEN;
		*enc = CIPHER_NONE;
		return 0;
	}

	/* in wpa_auth mode ... */
	if (!!(wpa_auth & WPA_AUTH_PSK) && !!(wpa_auth & WPA2_AUTH_PSK)) {
		*auth = AUTH_WPAPSK | AUTH_WPA2PSK;
	} else if (!!(wpa_auth & WPA2_AUTH_PSK)) {
		*auth = AUTH_WPA2PSK;
	} else if (!!(wpa_auth & WPA_AUTH_PSK)) {
		*auth = AUTH_WPAPSK;
	} else if (!!(wpa_auth & WPA_AUTH_UNSPECIFIED) &&
			!!(wpa_auth & WPA2_AUTH_UNSPECIFIED)) {
		*auth = AUTH_WPA | AUTH_WPA2;
	} else if (!!(wpa_auth & WPA2_AUTH_UNSPECIFIED)) {
		*auth = AUTH_WPA2;
	}
	else if (!!(wpa_auth & WPA_AUTH_UNSPECIFIED)) {
		*auth = AUTH_WPA;
	}
	/* else if (wpa_auth & WPA2_AUTH_1X_SHA256)
		strcpy(wpa, "1X-SHA256");
	else if (wpa_auth & WPA2_AUTH_FT)
		strcpy(wpa, "FT");	// TODO
	else if (wpa_auth & WPA2_AUTH_PSK_SHA256)
		strcpy(wpa, "PSK-SHA256"); */
	else {
		*auth = AUTH_UNKNOWN;
	}

	if (!!(wsec & 0x2))
		*enc |= CIPHER_TKIP;
	if (!!(wsec & 0x4))
		*enc |= CIPHER_AES;

	return 0;
}

int bcm_get_bssinfo(const char *ifname, enum wifi_bw *bandwidth,
					uint32_t *channel,
					int *noise)
{
	wl_bss_info_t *bi;
	int ioctl_req_version = 0x2000;
	char *tmp;
	int swap = 0;

	swap = wl_swap(ifname);

	tmp = malloc(WLC_IOCTL_MAXLEN * sizeof(char));
	if (tmp == NULL)
		return -1;

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	if (wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN) < 0) {
		libwifi_err("ioctl BSS_INFO error!!!\n");
		free(tmp);
		return 0;
	}

	bi = (wl_bss_info_t *)(tmp + 4);

	if (channel != NULL) {
		if (bi->ctl_ch) {
			*channel = bi->ctl_ch;
		} else {
			chanspec_t chs;

			chs = swap ? BCMSWAP16(bi->chanspec) : bi->chanspec;
			*channel = CHSPEC_CHANNEL(chs);
		}
	}

	if (noise != NULL)
		*noise = bi->phy_noise;

	if (bandwidth != NULL) {
		uint32_t bw = 0;

		if (swap)
			bw = (CHSPEC_IS160(BCMSWAP16(bi->chanspec)) ?
			160 : (CHSPEC_IS8080(BCMSWAP16(bi->chanspec)) ?
			8080 : (CHSPEC_IS80(BCMSWAP16(bi->chanspec)) ?
			80 : (CHSPEC_IS40(BCMSWAP16(bi->chanspec)) ?
			40 : (CHSPEC_IS20(BCMSWAP16(bi->chanspec)) ?
			20 : 10)))));
		else
			bw = (CHSPEC_IS160(bi->chanspec) ?
			160 : (CHSPEC_IS8080(bi->chanspec) ?
			8080 : (CHSPEC_IS80(bi->chanspec)) ?
			80 : (CHSPEC_IS40(bi->chanspec) ?
			40 : (CHSPEC_IS20(bi->chanspec) ?
			20 : 10))));

		if (bw == 80)
			*bandwidth = BW80;
		else if (bw == 40)
			*bandwidth = BW40;
		else if (bw == 20)
			*bandwidth = BW20;
		else if (bw == 160)
			*bandwidth = BW160;
		else if (bw == 8080)
			*bandwidth = BW8080;
		else
			*bandwidth = BW_UNKNOWN;
	}

	free(tmp);
	return 0;
}

static int bcm_get_bssload_from_beacon(const char *ifname, struct wifi_ap_load *load)
{
	unsigned char *ie, *ie_start;
	int bcnlen, len;
	int swap = 0;
	unsigned char *buf;
	unsigned char bssid[6] = {0};
	int offset;

	if (wl_ioctl(ifname, WLC_GET_BSSID, bssid, 6))
		return -1;

	swap = wl_swap(ifname);
	buf = malloc(WLC_IOCTL_MAXLEN);
	if (buf == NULL)
		return -1;

	memset(buf, 0, WLC_IOCTL_MAXLEN);
	if (wl_iovar_get_noparam(ifname, "beacon_info", &buf[0], WLC_IOCTL_MAXLEN) < 0) {
		free(buf);
		return -1;
	}

	bcnlen = *((int *)buf);
	bcnlen = swap ? BCMSWAP32(bcnlen) : bcnlen;
	if (bcnlen <= 0) {
		free(buf);
		return -1;
	}

	/* Don't trust BCM's API -
	 * Frame starts after the first 4 bytes, which holds sizeof
	 * returned buffer.
	 * This however is not always true across all BCM implementations.
	 * So, look for beacon frame start pattern in the returned buffer.
	 */
	offset = 4;
	while (offset < bcnlen) {
		if (buf[offset] == 0x80) {
			if (!memcmp(&buf[offset + 4], "\xff\xff\xff\xff\xff\xff", 6) &&
				!memcmp(&buf[offset + 10], bssid, 6) &&
				!memcmp(&buf[offset + 16], bssid, 6))
				/* found start of bcn frame */
				break;
		}
		offset++;
	}

	if (offset >= bcnlen) {
		free(buf);
		return -1;
	}

	/* ie starts after buf_len + mac header + tsf + bcnintv + capinfo */
	ie_start = buf + offset + 24 + 8 + 2 + 2;
	bcnlen -= (offset + 24 + 8 + 2 + 2);
	for (ie = ie_start; ie < ie_start + bcnlen; ie += len + 2) {
		len = ie[1];
		libwifi_dbg("Elm: 0x%02x  len = 0x%02x\n", ie[0], len);
		if (ie[0] == IE_BSS_LOAD && len == 5) {
			/* QoS Load Elm */
			load->sta_count = *((uint16_t *)&ie[2]);
			load->utilization = (int)ie[4];
			load->utilization *= (float)100 / (float)255;
			if (load->utilization > 100)
				load->utilization = 100;
			load->available = *((uint16_t *)&ie[5]);
			libwifi_dbg("load = %d   sta_count = %d\n",
					load->utilization, load->sta_count);
			break;
		}
	}

	free(buf);
	return 0;
}

/* This function is almost duplicate of bcm_get_bssload_from_beacon().
 * TODO - merge both the functions and cleanup.
 */
static int bcm_get_beacon_ies(const char *ifname, uint8_t *ies, int *ies_len)
{
	uint8_t *ie_start;
	int bcnlen;
	int swap = 0;
	unsigned char *buf;
	unsigned char bssid[6] = {0};
	int offset;

	if (wl_ioctl(ifname, WLC_GET_BSSID, bssid, 6))
		return -1;

	swap = wl_swap(ifname);
	buf = malloc(WLC_IOCTL_MAXLEN);
	if (buf == NULL)
		return -1;

	memset(buf, 0, WLC_IOCTL_MAXLEN);
	if (wl_iovar_get_noparam(ifname, "beacon_info", &buf[0], WLC_IOCTL_MAXLEN) < 0) {
		free(buf);
		return -1;
	}

	bcnlen = *((int *)buf);
	bcnlen = swap ? BCMSWAP32(bcnlen) : bcnlen;
	if (bcnlen <= 0) {
		free(buf);
		return -1;
	}

	offset = 4;
	/* Look for beacon frame start in the returned buffer */
	while (offset < bcnlen) {
		if (buf[offset] == 0x80) {
			if (!memcmp(&buf[offset + 4], "\xff\xff\xff\xff\xff\xff", 6) &&
				!memcmp(&buf[offset + 10], bssid, 6) &&
				!memcmp(&buf[offset + 16], bssid, 6))
				/* found start of bcn frame */
				break;
		}
		offset++;
	}

	if (offset >= bcnlen) {
		free(buf);
		return -1;
	}

	/* ie starts after buf_len + mac header + tsf + bcnintv + capinfo */
	ie_start = buf + offset + 24 + 8 + 2 + 2;
	bcnlen -= (offset + 24 + 8 + 2 + 2);

	if (*ies_len >= bcnlen) {
		memcpy(ies, ie_start, bcnlen);
		*ies_len = bcnlen;
	} else
		*ies_len = 0;

	free(buf);
	return 0;
}

int bcm_get_oper_stds(const char *ifname, uint8_t *mode)
{
#define INVALID_MODE	0xffffffff
	uint32_t gmode = 0;
	uint32_t nmode = 0;
	uint32_t vhtmode = 0;
	unsigned char _iovar_buf[256] = {0};
	int swap;
	enum wifi_band band = 0;
	bool band_is_2g = false;
	bool band_is_5g = false;

	*mode = 0;
	swap = wl_swap(ifname);

	bcm_get_oper_band(ifname, &band);
	switch (band) {
	case BAND_5:
		band_is_5g = true;
		break;
	case BAND_2:
		band_is_2g = true;
		break;
	case BAND_DUAL:
		band_is_2g = true;
		band_is_5g = true;
		break;
	default:
		break;
	}

	if (band_is_2g &&
		!wl_ioctl(ifname, WLC_GET_GMODE, &gmode, sizeof(gmode))) {
		gmode = swap ? BCMSWAP32(gmode) : gmode;

#define GMODE_LEGACY_B		0
#define GMODE_AUTO		1
#define GMODE_ONLY		2

		if (gmode == GMODE_LEGACY_B)
			*mode |= WIFI_B;
		else if (gmode == GMODE_AUTO)
			*mode |= (WIFI_B | WIFI_G);
		else if (gmode == GMODE_ONLY)
			*mode |= WIFI_G;
	} else
		gmode = INVALID_MODE;

	if (!wl_iovar_get_noparam(ifname, "nmode", &_iovar_buf[0], 256)) {
		nmode = *((uint32_t *)_iovar_buf);
		nmode = swap ? BCMSWAP32(nmode) : nmode;

		if (nmode)
			*mode |= WIFI_N;
	} else
		nmode = INVALID_MODE;

	memset(_iovar_buf, 0, sizeof(_iovar_buf));
	if (band_is_5g &&
		!wl_iovar_get_noparam(ifname, "vhtmode", &_iovar_buf[0], 256)) {
		vhtmode = *((uint32_t *)_iovar_buf);
		vhtmode = swap ? BCMSWAP32(vhtmode) : vhtmode;

		if (vhtmode)
			*mode |= WIFI_AC;
		else if (!(*mode & WIFI_N))
			*mode |= WIFI_A;
	} else
		vhtmode = INVALID_MODE;

	/* libwifi_dbg("g = 0x%x    n = 0x%x     vht = 0x%x\n",
						gmode, nmode, vhtmode); */

	return 0;
}

static int bcm_wifi_bss_info(const char *ifname, struct wifi_bss *bss)
{
	int ioctl_req_version = 0x2000;
	struct wl_maclist *macs;
	char bufptr[1536] = {0};
	//char ssid[33] = {0};
	//char bssid[18] = {0};
	cca_req_t req, *res;
	wl_bss_info_t *bi;
	chanspec_t chspec;
	int macs_len;
	int swap = 0;
	char *tmp;
	cca_t *c;
	uint32_t bw = 0;
	uint8_t *ext_ie, *rrm_ie, *ft_ie;
	uint8_t *vht_ie, *ht_ie;
	uint8_t *wmm_ie;
	uint8_t *he_ie;
	uint16_t capability = 0;
	uint8_t *ie_start;
	uint16_t ie_offset = 0;
	uint32_t ie_len = 0;
	uint8_t *ies = (uint8_t *)bufptr;
	int ies_length = sizeof(bufptr);

	swap = wl_swap(ifname);

	tmp = malloc(WLC_IOCTL_MAXLEN * sizeof(char));
	if (tmp == NULL)
		return -ENOMEM;

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	if (wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN) < 0) {
		free(tmp);
		return 0;
	}

	bi = (wl_bss_info_t *)(tmp + 4);
	chspec = bi->chanspec;

	if (bi->ctl_ch) {
		bss->channel = bi->ctl_ch;
	} else {
		chanspec_t chs;

		chs = swap ? BCMSWAP16(bi->chanspec) : bi->chanspec;
		bss->channel = CHSPEC_CHANNEL(chs);
	}

	bss->rssi = swap ? BCMSWAP16(bi->RSSI) : bi->RSSI;
	bss->noise = bi->phy_noise;
	libwifi_dbg("last_rssi = %d   phy_noise = %d\n", bss->rssi, bss->noise);

	if (swap)
		bw = (CHSPEC_IS160(BCMSWAP16(bi->chanspec)) ?
		160 : (CHSPEC_IS8080(BCMSWAP16(bi->chanspec)) ?
		8080 : (CHSPEC_IS80(BCMSWAP16(bi->chanspec)) ?
		80 : (CHSPEC_IS40(BCMSWAP16(bi->chanspec)) ?
		40 : (CHSPEC_IS20(BCMSWAP16(bi->chanspec)) ?
		20 : 10)))));
	else
		bw = (CHSPEC_IS160(bi->chanspec) ?
		160 : (CHSPEC_IS8080(bi->chanspec) ?
		8080 : (CHSPEC_IS80(bi->chanspec)) ?
		80 : (CHSPEC_IS40(bi->chanspec) ?
		40 : (CHSPEC_IS20(bi->chanspec) ?
		20 : 10))));

	if (bw == 80)
		bss->curr_bw = BW80;
	else if (bw == 40)
		bss->curr_bw = BW40;
	else if (bw == 20)
		bss->curr_bw = BW20;
	else if (bw == 160)
		bss->curr_bw = BW160;
	else if (bw == 8080)
		bss->curr_bw = BW8080;
	else
		bss->curr_bw = BW_UNKNOWN;

	snprintf((char *)bss->ssid, bi->SSID_len + 1, "%s", bi->SSID);
	memcpy(bss->bssid, bi->BSSID.octet, 6);

	/* get capabilities */
	capability = swap ? BCMSWAP16(bi->capability) : bi->capability;
	wifi_cap_set_from_ie(bss->cbitmap, (uint8_t *)&capability, 2);

	ie_len = swap ? BCMSWAP32(bi->ie_length) : bi->ie_length;
	ie_offset = swap ? BCMSWAP16(bi->ie_offset) : bi->ie_offset;
	ie_start = (uint8_t *)((uint8_t *)bi + ie_offset);
	if (ie_len == 0) {
		if (bcm_get_beacon_ies(ifname, ies, &ies_length) == 0) {
			ie_start = ies;
			ie_len = ies_length;
		}
	}

	/* {
		int _x;

		libwifi_dbg("IEs:  [ ");
		for (_x = 0; _x < ie_len; _x++)
			libwifi_dbg("%02X ", ie_start[_x]);
		libwifi_dbg("]\n");
	} */

	ht_ie = wifi_find_ie(ie_start, ie_len, IE_HT_CAP);
	vht_ie = wifi_find_ie(ie_start, ie_len, IE_VHT_CAP);
	ext_ie = wifi_find_ie(ie_start, ie_len, IE_EXT_CAP);
	rrm_ie = wifi_find_ie(ie_start, ie_len, IE_RRM);
	ft_ie = wifi_find_ie(ie_start, ie_len, IE_MDE);

	he_ie = wifi_find_ie_ext(ie_start, ie_len, IE_EXT_HE_CAP);

	wmm_ie = wifi_find_vsie(ie_start, ie_len, (uint8_t *)"\x00\x50\xf2", 2, 0xff);
	if (wmm_ie) {
		wifi_cap_set(bss->cbitmap, WIFI_CAP_WMM);
		wmm_ie += 2 + 3 + 2 + 1;  /* skip upto 'version' */
		if (!!(wmm_ie[0] & 0x80))
			wifi_cap_set(bss->cbitmap, WIFI_CAP_APSD);
	}

	if (ext_ie) {
		bss->caps.valid |= WIFI_CAP_EXT_VALID;
		memcpy(bss->caps.ext.byte, &ext_ie[2], min(ext_ie[1], 16));
		wifi_cap_set_from_ie(bss->cbitmap, ext_ie, ext_ie[1] + 2);
	}

	if (rrm_ie) {
		bss->caps.valid |= WIFI_CAP_RM_VALID;
		memcpy(&bss->caps.rrm, &rrm_ie[2], rrm_ie[1]);
		wifi_cap_set_from_ie(bss->cbitmap, rrm_ie, rrm_ie[1] + 2);
	}

	if (ft_ie) {
		wifi_cap_set_from_ie(bss->cbitmap, ft_ie, ft_ie[1] + 2);
	}

	if (ht_ie) {
		bss->caps.valid |= WIFI_CAP_HT_VALID;
		memcpy(&bss->caps.ht, &ht_ie[2], ht_ie[1]);
		wifi_cap_set_from_ie(bss->cbitmap, ht_ie, ht_ie[1] + 2);
	}

	if (vht_ie) {
		bss->caps.valid |= WIFI_CAP_VHT_VALID;
		memcpy(&bss->caps.vht, &vht_ie[2], vht_ie[1]);
		wifi_cap_set_from_ie(bss->cbitmap, vht_ie, vht_ie[1] + 2);
	}

	if (he_ie) {
		bss->caps.valid |= WIFI_CAP_HE_VALID;
		memcpy(&bss->caps.he, &he_ie[3], min(he_ie[1], sizeof(struct wifi_caps_he)));
		wifi_cap_set_from_ie(bss->cbitmap, he_ie, he_ie[1] + 2);
	}

	free(tmp);
	bcm_get_security(ifname, &bss->auth, &bss->enc); // TODO: deprecate
	wifi_get_bss_security_from_ies(bss, ie_start, ie_len);

	/* get bss ch utilization */
	memset(bufptr, 0, sizeof(bufptr));
	req.chanspec = chspec;
	req.num_secs = 1;
	if (wl_iovar_get(ifname, "cca_get_stats", &req, sizeof(req), bufptr, 1536) < 0)
		return bcm_get_bssload_from_beacon(ifname, &bss->load);

	res = (cca_req_t *)bufptr;
	if (res->chanspec == 0 || res->num_secs == 0)
		return -1;

	c = &res->secs[0];
	if (swap) {
		res->num_secs = BCMSWAP16(res->num_secs);
		c->duration = BCMSWAP32(c->duration);
		c->congest_ibss = BCMSWAP32(c->congest_ibss);
		c->congest_obss = BCMSWAP32(c->congest_obss);
		c->interference = BCMSWAP32(c->interference);
		c->timestamp = BCMSWAP32(c->timestamp);
	}

	if (c->duration) {
		int ibss_pcnt, obss_pcnt, inter_pcnt;

		ibss_pcnt = c->congest_ibss * 100 /c->duration;
		obss_pcnt = c->congest_obss * 100 /c->duration;
		inter_pcnt = c->interference * 100 /c->duration;

		bss->load.utilization = ibss_pcnt + obss_pcnt + inter_pcnt;
		if (bss->load.utilization > 100)
			bss->load.utilization = 100;
	}

	/* get connected stas num */
	macs_len = 4 + WL_MAX_STA_COUNT * 6;
	macs = (struct wl_maclist *)malloc(macs_len);
	if (macs == NULL)
		return -ENOMEM;

	memset(macs, 0, macs_len);
	macs->count = WL_MAX_STA_COUNT;
	if (wl_ioctl(ifname, WLC_GET_ASSOCLIST, macs, macs_len)) {
		free(macs);
		return -1;
	}

	macs->count = swap ? BCMSWAP32(macs->count) : macs->count;
	bss->load.sta_count = macs->count;
	free(macs);

	return 0;
}

static int bcm_get_ap_wmm_params(const char *ifname,
						struct wifi_ap_wmm_ac a[])
{
	struct edcf_acparam p[WIFI_NUM_AC];
	int swap = 0;
	int ret;

	swap = wl_swap(ifname);

	ret = wl_iovar_get_noparam(ifname, "wme_ac_ap", &p, sizeof(p));
	if (ret)
		return -1;

	a[0].ac = BE;
	a[0].aifsn = p[0].ACI & EDCF_AIFSN_MASK;
	a[0].cwmin = p[0].ECW & EDCF_ECWMIN_MASK;
	a[0].cwmax = (p[0].ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
	a[0].txop = swap ? BCMSWAP16(p[0].TXOP) : p[0].TXOP;

	a[1].ac = BK;
	a[1].aifsn = p[1].ACI & EDCF_AIFSN_MASK;
	a[1].cwmin = p[1].ECW & EDCF_ECWMIN_MASK;
	a[1].cwmax = (p[1].ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
	a[1].txop = swap ? BCMSWAP16(p[1].TXOP) : p[1].TXOP;

	a[2].ac = VI;
	a[2].aifsn = p[2].ACI & EDCF_AIFSN_MASK;
	a[2].cwmin = p[2].ECW & EDCF_ECWMIN_MASK;
	a[2].cwmax = (p[2].ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
	a[2].txop = swap ? BCMSWAP16(p[2].TXOP) : p[2].TXOP;

	a[3].ac = VO;
	a[3].aifsn = p[3].ACI & EDCF_AIFSN_MASK;
	a[3].cwmin = p[3].ECW & EDCF_ECWMIN_MASK;
	a[3].cwmax = (p[3].ECW & EDCF_ECWMAX_MASK) >> EDCF_ECWMAX_SHIFT;
	a[3].txop = swap ? BCMSWAP16(p[3].TXOP) : p[3].TXOP;

	return 0;
}

static int bcm_get_ap_wmm_stats(const char *ifname,
						struct wifi_ap_wmm_ac a[])
{
	uint8_t buf[256] = {0};
	wl_wme_cnt_t *cnt = (wl_wme_cnt_t *)buf;
	int swap = 0;
	int len;
	int ret;
	int i;

	swap = wl_swap(ifname);

	ret = wl_iovar_get_noparam(ifname, "wme_counters", buf, sizeof(buf));
	if (ret) {
		libwifi_err("error iovar 'wmm counters'\n");
		return -1;
	}

	len = swap ? BCMSWAP16(cnt->length) : cnt->length;
	if (len < sizeof(*cnt)) {
		libwifi_err("error iovar 'wmm counters' (len = %d < expected %d)\n",
				len, sizeof(wl_wme_cnt_t));
		return -1;
	}

	for (i = 0; i < WIFI_NUM_AC; i++) {
		a[i].stats.tx_bytes = swap ?
					BCMSWAP32(cnt->tx[i].bytes) :
					cnt->tx[i].bytes;

		a[i].stats.rx_bytes = swap ?
					BCMSWAP32(cnt->rx[i].bytes) :
					cnt->rx[i].bytes;

		a[i].stats.tx_pkts = swap ?
					BCMSWAP32(cnt->tx[i].packets) :
					cnt->tx[i].packets;

		a[i].stats.rx_pkts = swap ?
					BCMSWAP32(cnt->rx[i].packets) :
					cnt->rx[i].packets;

		a[i].stats.tx_err_pkts = swap ?
					BCMSWAP32(cnt->tx_failed[i].packets) :
					cnt->tx_failed[i].packets;

		a[i].stats.rx_err_pkts = swap ?
					BCMSWAP32(cnt->rx_failed[i].packets) :
					cnt->rx_failed[i].packets;
	}


	return 0;
}

static int bcm_get_ap_wmm_info(const char *ifname, struct wifi_ap_wmm_ac a[])
{
	int ret;

	ret = bcm_get_ap_wmm_params(ifname, a);

	ret |= bcm_get_ap_wmm_stats(ifname, a);

	return ret;
}

int bcm_ap_get_caps(const char *ifname, struct wifi_caps *caps)
{
	// TODO
	return 0;
}

static int bcm_get_bss_supported_security(const char *ifname, uint32_t *sec)
{
	*sec = BIT(WIFI_SECURITY_NONE) |
#ifdef WIFI_ALLOW_WEP_SUPPORT
		BIT(WIFI_SECURITY_WEP64) |
		BIT(WIFI_SECURITY_WEP128)
#endif
#ifdef WIFI_ALLOW_EAP_SUPPORT
		BIT(WIFI_SECURITY_WPA) |
		BIT(WIFI_SECURITY_WPA2) |
		BIT(WIFI_SECURITY_WPA3) |
#endif
		BIT(WIFI_SECURITY_WPAPSK) |
		BIT(WIFI_SECURITY_WPA2PSK) |
		BIT(WIFI_SECURITY_WPA3PSK);

	return 0;
}

static int bcm_get_ap_info(const char *ifname, struct wifi_ap *ap)
{
	unsigned char iov[256] = {0};
	struct wifi_bss *bss;
	int swap = 0;
	int bss_maxassoc = 0;
	int hidden = 0;
	int ap_isolate = 0;
	enum wifi_band oper_band = 0;
	uint8_t oper_std, supp_std;
	int ret;

	bss = &ap->bss;
	ret = bcm_wifi_bss_info(ifname, bss);
	if (ret != 0)
		return -1;

	swap = wl_swap(ifname);
	ret = wl_iovar_get_noparam(ifname, "bss_maxassoc", &iov[0], 256);
	if (!ret) {
		bss_maxassoc = *((uint32_t *)iov);
		bss_maxassoc = swap ? BCMSWAP32(bss_maxassoc) : bss_maxassoc;
		ap->assoclist_max = bss_maxassoc;
	}

	ret = wl_ioctl(ifname, WLC_GET_CLOSED, &hidden, sizeof(hidden));
	if (!ret) {
		hidden = swap ? BCMSWAP32(hidden) : hidden;
		ap->ssid_advertised = !hidden ? true : false;
	}

	memset(iov, 0, sizeof(iov));
	ret = wl_iovar_get_noparam(ifname, "ap_isolate", &iov[0], 256);
	if (!ret) {
		ap_isolate = *((uint32_t *)iov);
		ap_isolate = swap ? BCMSWAP32(ap_isolate) : ap_isolate;
		ap->isolate_enabled = ap_isolate == 0 ? false : true;
	}

	if (bcm_get_oper_stds(ifname, &oper_std) == 0)
		ap->bss.oper_std = oper_std;

	if (bcm_get_oper_band(ifname, &oper_band) == 0) {
		if (oper_band == BAND_5)
			supp_std = WIFI_A | WIFI_N | WIFI_AC;
		else if (oper_band == BAND_2)
			supp_std = WIFI_B | WIFI_G | WIFI_N;
		else
			supp_std = WIFI_UNKNOWN;

		ap->bss.supp_std = supp_std;
	}

	ret |= bcm_get_ap_wmm_info(ifname, ap->ac);

	bcm_get_bss_supported_security(ifname, &ap->sec.supp_modes);

	return ret;
}

static const uint8_t wf_chspec_bw_mhz[] = {5, 10, 20, 40, 80, 160, 160};
#define WF_NUM_BW (sizeof(wf_chspec_bw_mhz)/sizeof(uint8_t))

static const uint8 wf_5g_80m_chans[] = {42, 58, 106, 122, 138, 155};
static const uint8 wf_5g_40m_chans[] = {38, 46, 54, 62, 102, 110,
					118, 126, 134, 142, 151, 159};
static const uint8 wf_5g_160m_chans[] = {50, 114};

static int channel_to_sb(uint center_ch, uint ctl_ch, uint bw)
{
	uint low_ch;
	uint sb;

	low_ch = center_ch - ((bw - 20) / 2) / 5;
	if ((ctl_ch - low_ch) % 4)
		return -1;

	sb = ((ctl_ch - low_ch) / 4);
	if (sb >= (bw / 20))
		return -1;

	return sb;
}

static uint8_t chanspec_to_ctrlchannel(chanspec_t c)
{
	uint8_t bw;
	uint8_t center_ch;
	uint8_t sb;

	if (CHSPEC_BW_LE20(c))
		return CHSPEC_CHANNEL(c);

	sb = CHSPEC_CTL_SB(c) >> WL_CHANSPEC_CTL_SB_SHIFT;
	if (!CHSPEC_IS8080(c)) {
		bw = (c & WL_CHANSPEC_BW_MASK) >>
				WL_CHANSPEC_BW_SHIFT;
		bw = bw >= WF_NUM_BW ? 0 :
			wf_chspec_bw_mhz[bw];
		center_ch = CHSPEC_CHANNEL(c) >>
				WL_CHANSPEC_CHAN_SHIFT;
	} else {
		uint8_t chan_id = CHSPEC_CHANNEL(c);

		bw = 80;
		center_ch = wf_5g_80m_chans[chan_id];
	}
	return ((center_ch - (((bw - 20) / 2) / 5)) + sb * 4);
}

static uint16_t ctrlchannel_to_chanspec(uint32_t ctl_ch, uint32_t bw)
{
	uint16_t chspec;
	const uint8_t *center_ch = NULL;
	int num_ch = 0;
	int sb = -1;
	int i = 0;

	chspec = ((ctl_ch <= CH_MAX_2G_CHANNEL) ?
			WL_CHANSPEC_BAND_2G : WL_CHANSPEC_BAND_5G);
	chspec |= bw;
	if (bw == WL_CHANSPEC_BW_40) {
		center_ch = wf_5g_40m_chans;
		num_ch = sizeof(wf_5g_40m_chans)/sizeof(uint8);
		bw = 40;
	} else if (bw == WL_CHANSPEC_BW_80) {
		center_ch = wf_5g_80m_chans;
		num_ch = sizeof(wf_5g_80m_chans)/sizeof(uint8);;
		bw = 80;
	} else if (bw == WL_CHANSPEC_BW_160) {
		center_ch = wf_5g_160m_chans;
		num_ch = sizeof(wf_5g_160m_chans)/sizeof(uint8);;
		bw = 160;
		// TODO: 8080
	} else if (BW_LE20(bw)) {
		chspec |= ctl_ch;
		return chspec;
	} else {
		return 0;
	}

	for (i = 0; i < num_ch; i ++) {
		sb = channel_to_sb(center_ch[i], ctl_ch, bw);
		if (sb >= 0) {
			chspec |= center_ch[i];
			chspec |= (sb << WL_CHANSPEC_CTL_SB_SHIFT);
			break;
		}
	}
	if (sb < 0)
		return 0;

	return chspec;
}

static int bcm_get_supp_channels(const char *ifname, uint32_t *chlist, int *num,
					const char *cc,
					enum wifi_band band,
					enum wifi_bw bw)
{
	chanspec_t c = 0, *cspecs;
	wl_uint32_list_t *list;
	char *buf;
	int buflen;
	int swap;
	int i;
	char abbrev[4] = "";  /* default = current locale */
	char *cc_abbrev;
	int num_cspecs = 0;

	*num = 0;
	swap = wl_swap(ifname);

#define NUM_CHANSPECS_LIST_SIZE 110
	if (band == BAND_5)
		c |= WL_CHANSPEC_BAND_5G;
	else if (band == BAND_2)
		c |= WL_CHANSPEC_BAND_2G;
	else
		return -EINVAL;

	if (bw == BW20)
		c |= WL_CHANSPEC_BW_20;
	else if (bw == BW40)
		c |= WL_CHANSPEC_BW_40;
	else if (bw == BW80)
		c |= WL_CHANSPEC_BW_80;
	else if (bw == BW160)
		c |= WL_CHANSPEC_BW_160;
	else if (bw == BW8080)
		c |= WL_CHANSPEC_BW_8080;

	if (cc) {
		strncpy(abbrev, cc, 2);
		abbrev[2] = '\0';
	}

	c = swap ? BCMSWAP16(c) : c;

	buf = malloc(8192);
	if (buf == NULL)
		return -1;

	memset(buf, 0, 8192);
	buflen = strlen("chanspecs") + 1;

	cspecs = (chanspec_t *)(buf + buflen);
	*cspecs = c;
	buflen += sizeof(chanspec_t);
	cc_abbrev = buf + buflen;
	strncpy(cc_abbrev, abbrev, 4);
	buflen += 4;
	num_cspecs += NUM_CHANSPECS_LIST_SIZE;

	list = (wl_uint32_list_t *)(buf + buflen);
	list->count = swap ? BCMSWAP32(num_cspecs) : num_cspecs;
	buflen += sizeof(uint32_t) * (num_cspecs + 1);


	if (wl_iovar_get_noparam(ifname, "chanspecs", &buf[0], buflen) < 0) {
		free(buf);
		return -1;
	}

	list = (wl_uint32_list_t *)buf;
	list->count = swap ? BCMSWAP32(list->count) : list->count;
	for (i = 0; i < list->count; i++) {
		uint8_t ctrlch;

		c = swap ? (chanspec_t)BCMSWAP32(list->element[i]) :
				(chanspec_t)list->element[i];

		ctrlch = chanspec_to_ctrlchannel(c);
		/* libwifi_err("%d [0x%04x]\n", ctrlch, c); */
		chlist[i] = ctrlch;
	}

	*num = list->count;
	free(buf);

	return 0;
}



int bcm_radio_get_caps(const char *ifname, struct wifi_caps *caps)
{
	wl_bss_info_t *bi;
	int ioctl_req_version = 0x2000;
	char *tmp;
	int swap = 0;
	chanspec_t c = 0, *cspecs;
	wl_uint32_list_t *list;
	char *buf;
	int buflen;
	int i;
	char abbrev[4] = "";  /* default = current locale */
	char *cc_abbrev;
	int num_cspecs = 0;

	swap = wl_swap(ifname);

	tmp = malloc(WLC_IOCTL_MAXLEN * sizeof(char));
	if (tmp == NULL)
		return -1;

	memset(tmp, 0, WLC_IOCTL_MAXLEN);
	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	if (wl_ioctl(ifname, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN) < 0) {
		free(tmp);
		return 0;
	}

	bi = (wl_bss_info_t *)(tmp + 4);
	caps->valid = 0;

	if (bi->n_cap)
		caps->valid |= WIFI_CAP_HT_VALID;

	if (bi->vht_cap)
		caps->valid |= WIFI_CAP_VHT_VALID;

	free(tmp);

	c |= WL_CHANSPEC_BAND_5G;
	c |= WL_CHANSPEC_BW_160;
	c |= WL_CHANSPEC_BW_8080;

	abbrev[3] = '\0';

	c = swap ? BCMSWAP16(c) : c;

	buf = malloc(8192);
	if (buf == NULL)
		return -1;

	memset(buf, 0, 8192);
	buflen = strlen("chanspecs") + 1;

	cspecs = (chanspec_t *)(buf + buflen);
	*cspecs = c;
	buflen += sizeof(chanspec_t);
	cc_abbrev = buf + buflen;
	strncpy(cc_abbrev, abbrev, 4);
	buflen += 4;
#define NUM_CHANSPECS_LIST_SIZE 110
	num_cspecs += NUM_CHANSPECS_LIST_SIZE;

	list = (wl_uint32_list_t *)(buf + buflen);
	list->count = swap ? BCMSWAP32(num_cspecs) : num_cspecs;
	buflen += sizeof(uint32_t) * (num_cspecs + 1);

	if (wl_iovar_get_noparam(ifname, "chanspecs", &buf[0], buflen) < 0) {
		free(buf);
		return -1;
	}

	list = (wl_uint32_list_t *)buf;
	list->count = swap ? BCMSWAP32(list->count) : list->count;
	for (i = 0; i < list->count; i++) {
		c = swap ? (chanspec_t)BCMSWAP32(list->element[i]) :
				(chanspec_t)list->element[i];

		if (CHSPEC_BW_LE20(c)) {
			continue;
		} else {
			if (!CHSPEC_IS8080(c)) {
				uint8_t bw;

				bw = (c & WL_CHANSPEC_BW_MASK) >>
						WL_CHANSPEC_BW_SHIFT;
				bw = bw >= WF_NUM_BW ? 0 :
					wf_chspec_bw_mhz[bw];
				/* if (bw == 160)
					caps->flags |= CAP_160_SUPPORTED; */ //TODO
			} else {
				/* caps->flags |= CAP_8080_SUPPORTED; */  //TODO
			}
		}
	}

	free(buf);
	return 0;
}

int bcm_radio_get_stats(const char *ifname, struct wifi_radio_stats *s)
{
	char buf[1536] = {0};
	wl_cnt_wlc_t *cnt;
	wl_cnt_info_t *c;
	int swap;
	int ret;

	if (s == NULL)
		return -1;

	memset(s, 0, sizeof(struct wifi_radio_stats));
	swap = wl_swap(ifname);
	ret = wl_iovar_get(ifname, "counters", NULL, 0, buf, sizeof(buf));
	if (ret) {
		libwifi_err("wl_iovar_get error!!\n");
		return -1;
	}

	c = (wl_cnt_info_t *)buf;
	c->version = swap ? BCMSWAP16(c->version) : c->version;
	if (c->version == 11 || c->version == 10)
		cnt = (wl_cnt_wlc_t *)c->data;
	else if (c->version == 30)
		cnt = (wl_cnt_wlc_t *)(c->data + 4);  /* xtlv formatted */
	else {
		libwifi_err("'counters' version not supported!\n");
		return 0;
	}

	if (swap) {
		s->tx_pkts = (unsigned long)BCMSWAP32(cnt->txframe);
		s->tx_bytes = (unsigned long)BCMSWAP32(cnt->txbyte);
		s->tx_err_pkts = (uint32_t)BCMSWAP32(cnt->txerror);
		s->tx_dropped_pkts = BCMSWAP32(cnt->txnobuf);

		s->rx_pkts = (unsigned long)BCMSWAP32(cnt->rxframe);
		s->rx_bytes = (unsigned long)BCMSWAP32(cnt->rxbyte);
		s->rx_err_pkts = BCMSWAP32(cnt->rxerror);
		s->rx_dropped_pkts = BCMSWAP32(cnt->rxnobuf);

		s->rx_fcs_err_pkts = BCMSWAP32(cnt->rxcrc);
		s->rx_unknown_pkts = BCMSWAP32(cnt->rxbadproto);
	} else {

		s->tx_pkts = (unsigned long)(cnt->txframe);
		s->tx_bytes = (unsigned long)(cnt->txbyte);
		s->tx_err_pkts = cnt->txerror;
		s->tx_dropped_pkts = cnt->txnobuf;

		s->rx_pkts = (unsigned long)(cnt->rxframe);
		s->rx_bytes = (unsigned long)(cnt->rxbyte);
		s->rx_err_pkts = cnt->rxerror;
		s->rx_dropped_pkts = cnt->rxnobuf;

		s->rx_fcs_err_pkts = cnt->rxcrc;
		s->rx_unknown_pkts = cnt->rxbadproto;
	}

	return 0;
}

int bcm_get_channel(const char *ifname, uint32_t *ch, enum wifi_bw *bw)
{
	return bcm_get_bssinfo(ifname, bw, ch, NULL);
}

static int bcm_get_bandwidth(const char *ifname, enum wifi_bw *bw)
{
	return bcm_get_bssinfo(ifname, bw, NULL, NULL);
}

int bcm_get_ssid(const char *ifname, char *buf)
{
	wlc_ssid_t ssid;

	if (wl_ioctl(ifname, WLC_GET_SSID, &ssid, sizeof(ssid)))
		return -1;

	memcpy(buf, ssid.ssid, 32);

	return 0;
}

int bcm_get_bssid(const char *ifname, uint8_t *bssid)
{
	if (wl_ioctl(ifname, WLC_GET_BSSID, bssid, 6))
		return -1;

	/* sprintf(buf, "%02X:%02X:%02X:%02X:%02X:%02X",
		(uint8_t)bssid[0], (uint8_t)bssid[1], (uint8_t)bssid[2],
		(uint8_t)bssid[3], (uint8_t)bssid[4], (uint8_t)bssid[5]); */

	return 0;
}

#if 0	// deprecate
static int bcm_get_device_id(const char *ifname, int *buf)
{
	wlc_rev_info_t revinfo;

	*buf = 0;
	memset(&revinfo, 0, sizeof(wlc_rev_info_t));

	if (wl_ioctl(ifname, WLC_GET_REVINFO, &revinfo, sizeof(revinfo)))
		return -1;

	*buf = wl_swap(ifname) ? BCMSWAP32(revinfo.deviceid) : revinfo.deviceid;

	return 0;
}
#endif

static int bcm_acs(const char *ifname, struct acs_param *p)
{
	char buf[128] = {0};

	UNUSED(p);
	chrCmd(buf, 128, "acs_cli -i %s autochannel", ifname);
	return strncmp(buf, "Request finished", 16);
}

static int bcm_get_radio_info(const char *name, struct wifi_radio *radio)
{
	int ioctl_req_version = 0x2000;
	uint8_t basic_idx = 0, supp_idx = 0;
	enum wifi_band oper_band = 0;
	uint8_t oper_std, supp_std;
	uint8_t intbuf[256] = {0};
	char alpha2[4] = {0};
	wl_bss_info_t *bi;
	int beaconint = 0;
	int dtimperiod = 0;
	int pwr_pcnt = 0;
	int srl, lrl;
	uint32_t dfs;
	int swap = 0;
	int nss = 0;
	char *tmp;
	int ret;
	int i;

	swap = wl_swap(name);

	tmp = calloc(1, WLC_IOCTL_MAXLEN * sizeof(char));
	if (tmp == NULL)
		return -ENOMEM;

	memcpy(tmp, &ioctl_req_version, sizeof(ioctl_req_version));

	if (wl_ioctl(name, WLC_GET_BSS_INFO, tmp, WLC_IOCTL_MAXLEN) < 0) {
		free(tmp);
		return -1;
	}

	bi = (wl_bss_info_t *)(tmp + 4);
	bi->chanspec = swap ? BCMSWAP16(bi->chanspec) : bi->chanspec;

	if (bi->ctl_ch)
		radio->channel = bi->ctl_ch;

	radio->acs_capable = 1;
	radio->acs_enabled = 1;		// FIXME

	/* oper and supp standards */
	if (bcm_get_oper_stds(name, &oper_std) == 0)
		radio->oper_std = oper_std;

	if (bcm_get_oper_band(name, &oper_band) == 0) {
		if (oper_band == BAND_5) {
			supp_std = WIFI_A | WIFI_N | WIFI_AC;
			radio->dot11h_capable = 1;
		} else if (oper_band == BAND_2) {
			supp_std = WIFI_B | WIFI_G | WIFI_N;
		} else
			supp_std = WIFI_UNKNOWN;

		radio->supp_std = supp_std;
	}

	/* basic and supp rates */
	bi->rateset.count = swap ? BCMSWAP32(bi->rateset.count) :
				bi->rateset.count;

	for (i = 0; i < bi->rateset.count; i++) {
		if (bi->rateset.rates[i]) {
			radio->supp_rates[supp_idx++] =
				(bi->rateset.rates[i] & 0x7f) >> 1;

			if (!!(bi->rateset.rates[i] & 0x80)) {
				radio->basic_rates[basic_idx++] =
					(bi->rateset.rates[i] & 0x7f) >> 1;
			}
		}
	}

	/* mcs, tx and rx streams */
	if (bi->vht_cap) {
		uint8_t supp_mcs[2] = {0};
		uint16_t vht_rxmap;
		int octet = 0;

		vht_rxmap = swap ?
			BCMSWAP16(bi->vht_rxmcsmap) : bi->vht_rxmcsmap;

		*((uint16_t *)supp_mcs) = vht_rxmap;

		for (i = 0; i < 16; i += 2) {
			uint8_t supp_mcs_mask = 0;

			if (i && !(i % 8))
				octet++;

			supp_mcs_mask = supp_mcs[octet] & (0x3 << (i % 8));
			supp_mcs_mask >>= (i % 8);
			if (supp_mcs_mask == 3)
				break;

			nss++;
		}

		radio->tx_streams = nss;
		radio->rx_streams = nss;
		radio->aggr_enable = 1;

	} else if (bi->n_cap) {
		radio->aggr_enable = 1;
		for (i = 0; i < MCSSET_LEN; i++) {
			if (bi->basic_mcs[i]) {
				radio->tx_streams++;
				radio->rx_streams++;
			}
		}
	}

	/* beaconint, dtimperiod, rts, frag, txpower */
	ret = wl_ioctl(name, WLC_GET_BCNPRD, &beaconint, sizeof(int));
	if (!ret)
		radio->beacon_int = beaconint;

	ret = wl_ioctl(name, WLC_GET_DTIMPRD, &dtimperiod, sizeof(int));
	if (!ret)
		radio->dtim_period = dtimperiod;

	ret = wl_iovar_get_noparam(name, "rtsthresh", intbuf, sizeof(intbuf));
	if (!ret) {
		radio->rts = *((uint16_t *)intbuf);
		radio->rts = swap ? BCMSWAP16(radio->rts) : radio->rts;
	}

	memset(&intbuf, 0, sizeof(intbuf));
	ret = wl_iovar_get_noparam(name, "fragthresh", intbuf, sizeof(intbuf));
	if (!ret) {
		radio->frag = *((uint16_t *)intbuf);
		radio->frag = swap ? BCMSWAP16(radio->frag) : radio->frag;
	}

	ret = wl_ioctl(name, WLC_GET_PWROUT_PERCENTAGE, &pwr_pcnt, sizeof(int));
	if (!ret)
		radio->txpower = pwr_pcnt;


	ret = wl_ioctl(name, WLC_GET_SRL, &srl, sizeof(int));
	if (!ret)
		radio->srl = srl;

	ret = wl_ioctl(name, WLC_GET_LRL, &lrl, sizeof(int));
	if (!ret)
		radio->lrl = lrl;

	/* 11h, country ... */
	if (oper_band == BAND_5)
		radio->dot11h_capable = 1;

	ret = wl_ioctl(name, WLC_GET_SPECT_MANAGMENT, &dfs, sizeof(uint32_t));
	if (!ret)
		radio->dot11h_enabled = !!(dfs != 0);

	ret = bcm_get_country(name, alpha2);
	if (!ret)
		snprintf(radio->regdomain, 4, "%s", alpha2);

	free(tmp);

	ret = bcm_radio_get_stats(name, &radio->stats);

	return ret;
}

static int bcm_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason)
{
	char cmd[256] = { 0 };

	snprintf(cmd, 255, "wlctl -i %16s deauthenticate " MACFMT,	/* Flawfinder: ignore */
						ifname, MAC2STR(sta));

	if (reason > 0)
		snprintf(cmd + strlen(cmd), 255, " %d", reason);


	runCmd(cmd);

	return 0;
}

int bcm_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	runCmd("wlctl -i %s sta_monitor %s " MACSTR,
	       ifname, cfg->enable ? "add" : "del", MAC2STR(sta));

	if (cfg->enable)
		runCmd("wlctl -i %s sta_monitor enable", ifname);

	return 0;
}

int bcm_probe_sta(const char *ifname, uint8_t *sta)
{
	char buf[512] = {0};
	wl_scb_probe_t p;
	int swap;

	swap = wl_swap(ifname);

	/* probe sta if connected every 20 secs */
	p.scb_timeout = swap ? BCMSWAP32(20) : 20;
	p.scb_activity_time = swap ? BCMSWAP32(5) : 5;
	p.scb_max_probe = swap ? BCMSWAP32(1) : 1;

	if (wl_iovar_set(ifname, "scb_probe", &p, sizeof(p), buf, 512) < 0) {
		libwifi_err("wl_iovar_set probe sta error!!\n");
		return -1;
	}

	return 0;
}

int bcm_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *monsta)
{
	char buf[256] = {0};
	char *pos = NULL;

	memset(monsta, 0, sizeof(*monsta));
	memcpy(monsta->macaddr, sta, 6);

	/* get monitored data */
	chrCmd(buf, sizeof(buf),
		"wlctl -i %s sta_monitor stats " MACSTR,
		ifname, MAC2STR(sta));
	if (buf[0]) {
		pos = strstr(buf, "RSSI :");
		if (pos) {
			pos += 6;
			monsta->rssi[0] = strtol(pos, NULL, 10);
			monsta->rssi[1] = 0;
			monsta->rssi[2] = 0;
			monsta->rssi[3] = 0;
		}
		monsta->last_seen = 0;
	}

	return 0;
}

int bcm_get_assoclist(const char *ifname, uint8_t *stas, int *num)
{
	return hostapd_iface_get_assoclist(ifname, stas, num);
}

static int bcm_get_sta_info_airtime(const char *ifname, uint8_t *addr,
							struct wifi_sta *wsta)
{
	char bufptr[1536] = {0};
	iov_bs_data_struct_t *data = (iov_bs_data_struct_t *)bufptr;
	int32_t flag = 0;
	int num_stas = 0;
	int swap;
	int i;

	if (!wsta)
		return -1;

	swap = wl_swap(ifname);

	if (wl_iovar_get(ifname, "bs_data", &flag, sizeof(flag), bufptr, 1536) < 0) {
		libwifi_err("wl_iovar_get error!!\n");
		return -1;
	}

	num_stas = swap ? BCMSWAP16(data->structure_count) : data->structure_count;
	for (i = 0; i < num_stas; i++) {
		iov_bs_data_record_t *rec = &data->structure_record[i];
		iov_bs_data_counters_t *ctr = &rec->station_counters;

		if (!memcmp(rec->station_address.octet, addr, 6)) {
			int airtm = 0;

			ctr->time_delta = swap ? BCMSWAP32(ctr->time_delta) :
					ctr->time_delta;

			ctr->airtime = swap ? BCMSWAP32(ctr->airtime) :
					ctr->airtime;

			airtm = ctr->time_delta ?
				ctr->airtime * 100 / ctr->time_delta : 0;

			wsta->airtime = airtm;
			if (wsta->airtime > 100) {
				/* Rare but seen; can happen when more than
				 * 1's worth of sampled data is considered.
				 */
				wsta->airtime = 100;
			}

			return 0;
		}
	}

	return 0;
}

static int bcm_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *wsta)
{
	struct wl_sta_info *sta;
	struct wl_sta_info_ext7 *ext7 = NULL;
	char bufptr[1536] = {0};
	int bw;
	int sgi = 0;
	int nss = 0;
	int max_mcs = -1;
	int swap;
	int i;


	swap = wl_swap(ifname);

	if (wsta == NULL)
		return -1;

	if (wl_iovar_get(ifname, "sta_info", addr, 6, bufptr, 1536) < 0) {
		libwifi_err("wl_iovar_get error!!\n");
		return -1;
	}

	sta = (struct wl_sta_info *)bufptr;
	if (swap)
		sta->ver = BCMSWAP16(sta->ver);

	if (sta->ver >= 7)
		ext7 = (struct wl_sta_info_ext7 *)(sta + 1);

	if (swap) {
		sta->len = BCMSWAP16(sta->len);
		sta->cap = BCMSWAP16(sta->cap);
		sta->aid = BCMSWAP16(sta->aid);
		sta->flags = BCMSWAP32(sta->flags);
		sta->idle = BCMSWAP32(sta->idle);
		sta->rateset.count = BCMSWAP32(sta->rateset.count);
		sta->in = BCMSWAP32(sta->in);
		sta->listen_interval_inms =
				BCMSWAP32(sta->listen_interval_inms);
		sta->ht_capabilities = BCMSWAP16(sta->ht_capabilities);
		sta->vht_flags = BCMSWAP16(sta->vht_flags);

		if (sta->flags & WL_STA_SCBSTATS) {
			sta->tx_tot_pkts = BCMSWAP32(sta->tx_tot_pkts);
			sta->tx_tot_bytes = BCMSWAP64(sta->tx_tot_bytes);
			sta->tx_pkts = BCMSWAP32(sta->tx_pkts);
			sta->tx_ucast_bytes = BCMSWAP64(sta->tx_ucast_bytes);
			sta->tx_mcast_pkts = BCMSWAP32(sta->tx_mcast_pkts);
			sta->tx_mcast_bytes = BCMSWAP64(sta->tx_mcast_bytes);
			sta->tx_failures = BCMSWAP32(sta->tx_failures);
			sta->rx_tot_pkts = BCMSWAP32(sta->rx_tot_pkts);
			sta->rx_tot_bytes = BCMSWAP64(sta->rx_tot_bytes);
			sta->rx_ucast_pkts = BCMSWAP32(sta->rx_ucast_pkts);
			sta->rx_ucast_bytes = BCMSWAP64(sta->rx_ucast_bytes);
			sta->rx_mcast_pkts = BCMSWAP32(sta->rx_mcast_pkts);
			sta->rx_mcast_bytes = BCMSWAP64(sta->rx_mcast_bytes);
			sta->tx_rate = BCMSWAP32(sta->tx_rate);
			sta->tx_rate_fallback =
				BCMSWAP32(sta->tx_rate_fallback);
			sta->rx_rate = BCMSWAP32(sta->rx_rate);
			sta->rx_decrypt_succeeds =
				BCMSWAP32(sta->rx_decrypt_succeeds);
			sta->rx_decrypt_failures =
				BCMSWAP32(sta->rx_decrypt_failures);
			sta->tx_pkts_retried = BCMSWAP32(sta->tx_pkts_retried);
			sta->tx_pkts_total = BCMSWAP32(sta->tx_pkts_total);
			sta->tx_pkts_retries = BCMSWAP32(sta->tx_pkts_retries);
			sta->tx_pkts_retry_exhausted =
				BCMSWAP32(sta->tx_pkts_retry_exhausted);
			sta->tx_pkts_fw_total =
				BCMSWAP32(sta->tx_pkts_fw_total);
			sta->tx_pkts_fw_retries =
				BCMSWAP32(sta->tx_pkts_fw_retries);
			sta->tx_pkts_fw_retry_exhausted =
				BCMSWAP32(sta->tx_pkts_fw_retry_exhausted);
			sta->rx_pkts_retried = BCMSWAP32(sta->rx_pkts_retried);
		}
	}

	/* fill wifi_sta */
	memset(wsta, 0, sizeof(struct wifi_sta));
	memcpy(wsta->macaddr, sta->ea.octet, 6);

	/* libwifi_dbg("Capability: 0x%04x\n", sta->cap); */
	wifi_cap_set_from_ie(wsta->cbitmap, (uint8_t *)&sta->cap, 2); /* set caps bits */

	if (!!(sta->flags & WL_STA_N_CAP)) {
		wsta->caps.valid |= WIFI_CAP_HT_VALID;

		if (!!(sta->ht_capabilities & WL_STA_CAP_LDPC_CODING))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HT_LDPC);

		if (!!(sta->ht_capabilities & WL_STA_CAP_40MHZ))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_2040);

		if (!!(sta->ht_capabilities & WL_STA_CAP_GF))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HT_GREENFIELD);

		if (!!(sta->ht_capabilities & WL_STA_CAP_SHORT_GI_20)) {
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_SGI20);
			sgi = 1;
		}

		if (!!(sta->ht_capabilities & WL_STA_CAP_SHORT_GI_40)) {
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_SGI40);
			sgi = 1;
		}

		if (!!(sta->ht_capabilities & WL_STA_CAP_TX_STBC))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HT_TX_STBC);

		if (!!(sta->ht_capabilities & WL_STA_CAP_RX_STBC_MASK))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HT_RX_STBC);

		if (!!(sta->ht_capabilities & WL_STA_CAP_MIMO_PS_MASK))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HT_SMPS);

		/* set wmm cap true for ht sta */
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_WMM);

		if (ext7) {
			uint8_t *supp_mcs = ext7->rateset_adv.mcs;
			int octet = 0;
			int l;

			for (l = 0; l < 76; l++) {
				if (l && !(l % 8))
					octet++;

				if (!!(supp_mcs[octet] & bit(l % 8)))
					max_mcs++;
			}
			nss = (max_mcs / 8) + 1;
			max_mcs %= 8;

			bw = 20;
			if (ext7->link_bw > 0 && ext7->link_bw <= 4)
				bw *= (1 << (ext7->link_bw - 1));

			if (max_mcs > 0)
				wsta->maxrate = wifi_mcs2rate(max_mcs, bw, nss, sgi ? WIFI_SGI : WIFI_LGI);
		}
	}

	if (!!(sta->flags & WL_STA_VHT_CAP)) {
		wsta->caps.valid |= WIFI_CAP_VHT_VALID;

		if (!!(sta->vht_flags & WL_STA_VHT_LDPCCAP))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_VHT_RX_LDPC);

		if (!!(sta->vht_flags & WL_STA_SGI80)) {
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_SGI80);
			sgi = 1;
		}

		if (!!(sta->vht_flags & WL_STA_SGI160)) {
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_SGI160);
			sgi = 1;
		}

		if (!!(sta->vht_flags & WL_STA_VHT_TX_STBCCAP))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_VHT_TX_STBC);

		if (!!(sta->vht_flags & WL_STA_SU_BEAMFORMEE))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_VHT_SU_BFE);

		if (!!(sta->vht_flags & WL_STA_SU_BEAMFORMER))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_VHT_SU_BFR);

		if (!!(sta->vht_flags & WL_STA_MU_BEAMFORMEE))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_VHT_MU_BFE);

		if (!!(sta->vht_flags & WL_STA_MU_BEAMFORMER))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_VHT_MU_BFR);

		/* set wmm cap true for vht */
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_WMM);

		if (ext7) {
			nss = 0;
			max_mcs = 0;
			for (i = 0; i < WL_VHT_CAP_MCS_MAP_NSS_MAX; i++) {
				uint16_t vht_mcs;

				vht_mcs = swap ?
					BCMSWAP16(ext7->rateset_adv.vht_mcs[i])
					: ext7->rateset_adv.vht_mcs[i];

				if (!vht_mcs)
					break;

				if ((vht_mcs & 0x3ff) == 0x3ff)
					max_mcs = 9;
				else if ((vht_mcs & 0x3ff) == 0x1ff)
					max_mcs = 8;
				else
					max_mcs = 7;

				nss++;
			}

			bw = 20;
			if (ext7->link_bw > 0 && ext7->link_bw <= 4)
				bw *= (1 << (ext7->link_bw - 1));

			if (max_mcs)
				wsta->maxrate = wifi_mcs2rate(max_mcs, bw, nss, sgi ? WIFI_SGI : WIFI_LGI);
		}
	}

	if (ext7) {
		if (swap)
			ext7->wnm_cap = BCMSWAP32(ext7->wnm_cap);

		if (!!(ext7->wnm_cap & 0x1))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_11V_BSS_TRANS);
	}

	/* status flags */

	/* libwifi_dbg("flags: 0x%08x\n", sta->flags); */
	if (!!(sta->flags & WL_STA_WME))
		wifi_status_set(wsta->sbitmap, WIFI_STATUS_WMM);

	if (!!(sta->flags & WL_STA_PS))
		wifi_status_set(wsta->sbitmap, WIFI_STATUS_PS);

	wsta->conn_time = sta->in;
	wsta->idle_time = sta->idle;
	wsta->tx_rate.rate = sta->tx_rate / 1000;  /* in Mbps */
	wsta->rx_rate.rate = sta->rx_rate / 1000;  /* in Mbps */

	if (ext7 && wsta->maxrate == 0) {
		if (ext7->rateset_adv.count < WL_MAXRATES_IN_SET) {
			wsta->maxrate =
				(ext7->rateset_adv.rates[ext7->rateset_adv.count] & 0x7f) * 2;
		}
	}

	for (i = 0; i < 4; i++) {
		wsta->rssi[i] = sta->rssi[i];
		wsta->noise[i] = sta->nf[i];
	}

	if (sta->flags & WL_STA_SCBSTATS) {
		wsta->stats.tx_pkts = sta->tx_tot_pkts;
		wsta->stats.rx_pkts = sta->rx_tot_pkts;
		wsta->stats.tx_bytes = sta->tx_tot_bytes;
		wsta->stats.rx_bytes = sta->rx_tot_bytes;
		wsta->stats.tx_fail_pkts = sta->tx_failures;
		wsta->stats.rx_fail_pkts = sta->rx_decrypt_failures; /* FIXME */
		wsta->stats.tx_retry_pkts = sta->tx_pkts_retried;
		//wsta->stats.tx_drop_pkts = sta->tx_failures;
		//wsta->stats.rx_drop_pkts = sta->rx_decrypt_failures;
	}


	/* to get airtime, instant thput etc. */
	bcm_get_sta_info_airtime(ifname, addr, wsta);

	return 0;
}

static int bcm_csa(const char *ifname, uint32_t cs)
{
	wl_chan_switch_t csa;
	char buf[512] = {0};

	csa.mode = 1;
	csa.count = 2;
	csa.reg = 0;
	csa.chspec = cs;
	csa.frame_type = 0;  /* broadcast */

	if (wl_iovar_set(ifname, "csa", &csa, sizeof(csa), buf, 512) < 0) {
		libwifi_err("wl_iovar_set csa error!!\n");
		return -1;
	}

	return 0;
}

int bcm_set_channel(const char *ifname, uint32_t channel, enum wifi_bw bw)
{
	//uint8_t bufptr[256] = {0};
	int csint = 0;
	uint16_t cs;
	int ret;
	uint32_t bwint = 0;

	if (channel == 0)
		return bcm_acs(ifname, NULL);

	if (bw == BW80)
		bwint = WL_CHANSPEC_BW_80;
	else if (bw == BW160)
		bwint = WL_CHANSPEC_BW_160;
	else if (bw == BW8080)
		bwint = WL_CHANSPEC_BW_8080;
	else if (bw == BW40)
		bwint = WL_CHANSPEC_BW_40;
	else if (bw == BW20)
		bwint = WL_CHANSPEC_BW_20;
	else
		return -1;

	cs = ctrlchannel_to_chanspec(channel, bwint);
	libwifi_dbg("set chanspec = 0x%x\n", cs);
	if (cs == 0 || cs == INVCHANSPEC)
		return -1;

	csint = cs;
	/*
	if (wl_iovar_set(ifname, "chanspec", &csint, sizeof(int),
						bufptr, 256) < 0) {
		libwifi_err("wl_iovar_set error!!\n");
		return -1;
	}
	*/
	ret = bcm_csa(ifname, csint);

	return ret;
}

static int bcm_scan_trigger(const char *ifname, struct scan_param *p)
{
	struct wl_scan_params params;
	int swap;

	swap = wl_swap(ifname);
	memset(&params, 0, sizeof(params));
	if (p) {
		strncpy((char *)params.ssid.ssid, p->ssid, 32);
		params.ssid.ssid_len = strlen(p->ssid);
	}

	params.bss_type = DOT11_BSSTYPE_ANY;
	memset(&params.bssid, 0xFF, ETHER_ADDR_LEN);
	params.scan_type = (-1);
	params.nprobes = swap ? BCMSWAP32(-1) : -1;
	params.active_time = swap ? BCMSWAP32(-1) : -1;
	params.passive_time = swap ? BCMSWAP32(-1) : -1;
	params.home_time = swap ? BCMSWAP32(-1) : -1;
	params.channel_num = 0;

	return wl_ioctl(ifname, WLC_SCAN, &params, sizeof(params));
}

static int bcm_scan(const char *ifname, struct scan_param *p)
{
	chanspec_t orig_cs;
	char buf[256] = {0};
	int swap;
	int ret = 0;
	int chg = 0;

	ret = bcm_scan_trigger(ifname, p);
	if (!ret)
		return 0;

	/* else workaroud for pre 4366 chips that reject scanning
	 * when in dfs channels
	 */
	swap = wl_swap(ifname);
#ifdef WORKAROUND_BCM_DFS_SCAN
{
	unsigned int *c;
	int buflen = 0;
	uint32_t scan_cs = 0xe02a;
	int ctrlch;

	buflen = strlen("chanspec") + 1;
	buflen += sizeof(int);
	if (wl_iovar_get_noparam(ifname, "chanspec", buf, buflen) < 0) {
		libwifi_err("error get chanspec\n");
		return -1;
	}
	c = (unsigned int *)&buf[0];
	orig_cs = swap ? BCMSWAP16(*c) : *c;
	ctrlch = chanspec_to_ctrlchannel(orig_cs);
	if (ctrlch >= 52) {
		ret = bcm_csa(ifname, scan_cs);
		libwifi_dbg("csa ret = %d\n", ret);
		chg = 1;
	}
	sleep(1);
}
#endif
	ret = bcm_scan_trigger(ifname, p);
	libwifi_dbg("scan ret = %d\n", ret);

#ifdef WORKAROUND_BCM_DFS_SCAN
	if (chg) {
		usleep(100*1000);
		ret = bcm_csa(ifname, orig_cs);
		libwifi_dbg("csa ret = %d\n", ret);
	}
#endif
	return ret;
}

static int collect_bssload_info(wl_bss_info_t *bi, struct wifi_ap_load *l, int swap)
{
	uint8_t *ie_start, *ie, *data;
	uint16_t ie_offset = 0;
	uint32_t ie_len = 0;
	uint16_t tag, len;

	ie_len = swap ? BCMSWAP32(bi->ie_length) : bi->ie_length;
	ie_offset = swap ? BCMSWAP16(bi->ie_offset) : bi->ie_offset;
	ie_start = (uint8_t *)((uint8_t *)bi + ie_offset);

	/* libwifi_dbg("%s(): %d swap = %d ie_start = %p ie_len = %d
			ie_offset = %d\n",
			__func__, __LINE__,
			swap, ie_start, ie_len, ie_offset); */

	for (ie = ie_start, len = ie[1];
			ie < ie_start + ie_len; ie += len + 2) {
		tag = *ie & 0x00FF;
		len = *(ie + 1);
		data = ie + 2;
		if (tag == 11) { /* BSS Load */
			l->sta_count = data[0] | (data[1] << 8);
			l->utilization = data[2];
			l->utilization *= (float)100 / (float)255;
			if (l->utilization > 100)
				l->utilization = 100;
			l->available = data[3] | (data[4] << 8);
			return 0;
		}
	}

	return -1;
}

void wl_get_bss_info_summary(wl_bss_info_t *bi, struct wifi_bss *e,
							int noise, int swap)
{
	//char bssid[18] = {0};
	char buf[256];
	//int16_t rssi;
	int32_t version = 0;
	chanspec_t chspec = 0;
	int8_t rssi;
	uint8_t *ie_start;
	uint16_t ie_offset = 0;
	uint32_t ie_len = 0;
	uint16_t capability;


	/* sprintf(bssid, "%02X:%02X:%02X:%02X:%02X:%02X",
		bi->BSSID.octet[0], bi->BSSID.octet[1], bi->BSSID.octet[2],
		bi->BSSID.octet[3], bi->BSSID.octet[4], bi->BSSID.octet[5]);

	strncpy(e->bssid, bssid, 18); */
	memcpy(e->bssid, bi->BSSID.octet, 6);

	capability = swap ? BCMSWAP16(bi->capability) : bi->capability;
	libwifi_dbg("capability: 0x%x\n", capability);
	if (!!(capability & 0x1))
		e->mode = WIFI_INFRA;
	else if (!!(capability & 0x2))
		e->mode = WIFI_ADHOC;

	rssi = swap ? BCMSWAP16(bi->RSSI) : bi->RSSI;
	e->rssi = rssi;
	e->noise = bi->phy_noise;
	chspec = swap ? BCMSWAP16(bi->chanspec) : bi->chanspec;
	e->band = CHSPEC_IS2G(chspec) ? BAND_2 :
		CHSPEC_IS5G(chspec) ? BAND_5 :
		BAND_UNKNOWN;

	e->channel = (bi->ctl_ch) ? bi->ctl_ch : CHSPEC_CHANNEL(chspec);
	e->curr_bw = (CHSPEC_IS160(bi->chanspec) ?
			BW160 : (CHSPEC_IS8080(chspec) ?
			BW8080 : (CHSPEC_IS80(chspec)) ?
			BW80 : (CHSPEC_IS40(chspec) ?
			BW40 : (CHSPEC_IS20(chspec) ?
			BW20 : BW_UNKNOWN))));

	version = swap ? BCMSWAP32(bi->version) : bi->version;
	if (version != LEGACY_WL_BSS_INFO_VERSION && bi->n_cap) {
		if (bi->vht_cap) {
			e->oper_std = WIFI_N | WIFI_AC;
		} else {
			if (!!(e->band & BAND_5))
				e->oper_std = WIFI_A | WIFI_N;
			else if (!!(e->band & BAND_2))
				e->oper_std = WIFI_B | WIFI_G | WIFI_N;
		}
	} else {
		e->oper_std = WIFI_B | WIFI_G;
	}

	wl_format_ssid(buf, bi->SSID, bi->SSID_len);
	strncpy((char *)e->ssid, buf, 32);
	e->ssid[32] = '\0';

	collect_bssload_info(bi, &e->load, swap);
	//collect_security_info(bi, &auth, &cipher, swap);	/* FIXME */
	//e->sec = auth;
	//e->enc = cipher;

	ie_len = swap ? BCMSWAP32(bi->ie_length) : bi->ie_length;
	ie_offset = swap ? BCMSWAP16(bi->ie_offset) : bi->ie_offset;
	ie_start = (uint8_t *)((uint8_t *)bi + ie_offset);


	wifi_get_bss_security_from_ies(e, ie_start, ie_len);
	//e->sec --> e->security;
	e->enc = e->rsn.pair_ciphers;	// deprecated; remove soon
}

static void parse_scanresults_list(const char *ifname, char *buf,
				struct wifi_bss *bsss,
				int *num, int swap)
{
	wl_scan_results_t *list = (wl_scan_results_t *)buf;
	int count = 0;
	int version = 0;
	int noise;
	wl_bss_info_t *bi;
	int i;
	struct wifi_bss *e;
	int bsslen = 0;

	count = swap ? BCMSWAP32(list->count) : list->count;
	version = swap ? BCMSWAP32(list->version) : list->version;
	*num = 0;

	if (count == 0) {
		return;
	} else if (version != WL_BSS_INFO_VERSION &&
			version != LEGACY2_WL_BSS_INFO_VERSION &&
			version != LEGACY_WL_BSS_INFO_VERSION) {
		libwifi_err("%s(): version mismatch!\n", __func__);
		return;
	}

	bcm_get_bssinfo(ifname, NULL, NULL, &noise);

	bi = list->bss_info;
	for (i = 0; i < count; i++) {
		e = bsss + *num;
		memset(e, 0, sizeof(struct wifi_bss));
		wl_get_bss_info_summary(bi, e, noise, swap);
		bsslen = swap ? BCMSWAP32(bi->length) : bi->length;
		bi = (wl_bss_info_t *)(((int8 *)bi) + bsslen);
		*num += 1;
	}
}

static int bcm_get_scan_results(const char *ifname,
				struct wifi_bss *bsss,
				int *num)
{
	char *data;
	int size = 16380;
	wl_scan_results_t *list;
	int swap;

	*num = 0;
	data = malloc(size);
	if (data == NULL)
		return -1;

	memset(data, 0, size);
	list = (wl_scan_results_t *)data;

	swap = wl_swap(ifname);
	list->buflen = swap ? BCMSWAP32(size) : size;
	size = swap ? BCMSWAP32(size) : size;

	if (wl_ioctl(ifname, WLC_SCAN_RESULTS, data, size)) {
		free(data);
		return -1;
	}

	parse_scanresults_list(ifname, data, bsss, num, swap);

	free(data);
	return 0;
}

int bcm_get_bss_scanresult(const char *ifname, uint8_t *bssid,
						struct wifi_bss_detail *b)
{
	char *data;
	int size = 16380;
	wl_scan_results_t *list;
	wl_bss_info_t *bi;
	int version = 0;
	int bsslen = 0;
	int count = 0;
	int noise;
	int swap;
	int i;

	data = malloc(size);
	if (data == NULL)
		return -ENOMEM;

	memset(data, 0, size);
	list = (wl_scan_results_t *)data;

	swap = wl_swap(ifname);
	list->buflen = swap ? BCMSWAP32(size) : size;
	size = swap ? BCMSWAP32(size) : size;

	if (wl_ioctl(ifname, WLC_SCAN_RESULTS, data, size)) {
		free(data);
		return -1;
	}

	count = swap ? BCMSWAP32(list->count) : list->count;
	version = swap ? BCMSWAP32(list->version) : list->version;

	if (!count)
		return -1;

	if (version != WL_BSS_INFO_VERSION &&
			version != LEGACY2_WL_BSS_INFO_VERSION &&
			version != LEGACY_WL_BSS_INFO_VERSION) {
		libwifi_err("%s(): version mismatch!\n", __func__);
		return -1;
	}

	bcm_get_bssinfo(ifname, NULL, NULL, &noise);
	bi = list->bss_info;
	for (i = 0; i < count; i++) {
		if (!memcmp(bi->BSSID.octet, bssid, 6)) {
			uint16_t ie_offset = 0;
			uint32_t ie_len = 0;
			uint8_t *ie_start;

			memset(b, 0, sizeof(struct wifi_bss_detail));
			wl_get_bss_info_summary(bi, &b->basic, noise, swap);

			ie_len = swap ? BCMSWAP32(bi->ie_length) : bi->ie_length;
			ie_offset = swap ? BCMSWAP16(bi->ie_offset) : bi->ie_offset;
			ie_start = (uint8_t *)((uint8_t *)bi + ie_offset);

			if (ie_len > 1024)
				ie_len = 1024;

			b->ielen = ie_len;
			memcpy(b->ie, ie_start, ie_len);
			free(data);
			return 0;
		}

		bsslen = swap ? BCMSWAP32(bi->length) : bi->length;
		bi = (wl_bss_info_t *)(((int8 *)bi) + bsslen);
	}

	free(data);
	return -1;
}

int bcm_ap_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	char buf[1536] = {0};
	wl_if_stats_t *cnt;
	int swap;
	int ret;

	if (s == NULL)
		return -1;

	memset(s, 0, sizeof(struct wifi_ap_stats));
	swap = wl_swap(ifname);
	ret = wl_iovar_get(ifname, "if_counters", NULL, 0, buf, sizeof(buf));
	if (ret) {
		libwifi_err("wl_iovar_get error!!\n");
		return -1;
	}

	cnt = (wl_if_stats_t *)buf;
	if (swap) {
		s->tx_bytes = (unsigned long)BCMSWAP64(cnt->txbyte);
		s->rx_bytes = (unsigned long)BCMSWAP64(cnt->rxbyte);
		s->tx_pkts = (unsigned long)BCMSWAP64(cnt->txframe);
		s->rx_pkts = (unsigned long)BCMSWAP64(cnt->rxframe);
		s->tx_err_pkts = (uint32_t)BCMSWAP64(cnt->txerror);
		//s->tx_rtx_pkts =
		s->tx_rtx_fail_pkts = (uint32_t)BCMSWAP64(cnt->txfail);
		s->tx_retry_pkts = (uint32_t)BCMSWAP64(cnt->txretry);
		s->tx_mretry_pkts = (uint32_t)BCMSWAP64(cnt->txretrie);
		//s->ack_fail_pkts =
		//s->aggr_pkts =
		s->rx_err_pkts = (uint32_t)BCMSWAP64(cnt->rxerror);
		//s->tx_ucast_pkts =
		//s->rx_ucast_pkts =
		s->tx_dropped_pkts = (uint32_t)BCMSWAP64(cnt->txnobuf);
		s->rx_dropped_pkts = (uint32_t)BCMSWAP64(cnt->rxnobuf);
		s->tx_mcast_pkts = (unsigned long)BCMSWAP64(cnt->txmulti);
		s->rx_mcast_pkts = (unsigned long)BCMSWAP64(cnt->rxmulti);
		//s->tx_bcast_pkts =
		//s->rx_bcast_pkts =
		//s->rx_unknown_pkts =
	} else {
		s->tx_bytes = (unsigned long)cnt->txbyte;
		s->rx_bytes = (unsigned long)cnt->rxbyte;
		s->tx_pkts = (unsigned long)cnt->txframe;
		s->rx_pkts = (unsigned long)cnt->rxframe;
		s->tx_err_pkts = (uint32_t)cnt->txerror;
		//s->tx_rtx_pkts =
		s->tx_rtx_fail_pkts = (uint32_t)cnt->txfail;
		s->tx_retry_pkts = (uint32_t)cnt->txretry;
		s->tx_mretry_pkts = (uint32_t)cnt->txretrie;
		//s->ack_fail_pkts =
		//s->aggr_pkts =
		s->rx_err_pkts = (uint32_t)cnt->rxerror;
		//s->tx_ucast_pkts =
		//s->rx_ucast_pkts =
		s->tx_dropped_pkts = (uint32_t)cnt->txnobuf;
		s->rx_dropped_pkts = (uint32_t)cnt->rxnobuf;
		s->tx_mcast_pkts = (unsigned long)cnt->txmulti;
		s->rx_mcast_pkts = (unsigned long)cnt->rxmulti;
		//s->tx_bcast_pkts =
		//s->rx_bcast_pkts =
		//s->rx_unknown_pkts =
	}

	return ret;
}

int bcm_start_wps(const char *ifname, struct wps_param wps)
{

	if (wps.role == WPS_ENROLLEE) {
		if (wps.method == WPS_METHOD_PBC)
			runCmd("wps_cmd enroll %s pbc", ifname);
		/* else if (wps.method == WPS_METHOD_PIN) */
			/* TODO: enrollee in pin mode */
		else
			return -1;
	} else {
		if (wps.method == WPS_METHOD_PBC)
			runCmd("wps_cmd addenrollee %s pbc", ifname);
		else if (wps.method == WPS_METHOD_PIN)
			runCmd("wps_cmd addenrollee %s sta_pin=%08lu",
					ifname, wps.pin);
		else
			return -1;
	}

	return 0;
}

int bcm_stop_wps(const char *ifname)
{
	UNUSED(ifname);

	runCmd("wps_cmd stop");
	return 0;
}

static int bcm_get_wps_status(const char *ifname, enum wps_status *s)
{
	char out[32] = {0};
	int code;

	UNUSED(ifname);

	code = atoi(chrCmd(out, 32, "nvram get wps_proc_status 2>/dev/null"));
	*s = (enum wps_status)code;

	return 0;
}

static int bcm_get_wps_pin(const char *ifname, unsigned long *pin)
{
	char out[64] = {0};

	UNUSED(ifname);

	chrCmd(out, 64, "nvram get wps_device_pin 2>/dev/null");
	*pin = strtoul(out, NULL, 10);

	return 0;
}

static int bcm_set_wps_pin(const char *ifname, unsigned long pin)
{
	UNUSED(ifname);

	runCmd("wps_cmd setpin %08lu &", pin);

	return 0;
}

static int bcm_get_neighbor_list_static(const char *ifname, struct nbr *nbrs,
								int *nr)
{
	char buf[2048] = {};
	struct nbr *neigh;
	char *p, *q, *line;
	int max = *nr;

	chrCmd(buf, sizeof(buf), "wl -i %s rrm_nbr_list", ifname);

	p = buf;
	neigh = &nbrs[0];
	*nr = 0;

	/* bssid 44:D4:37:4D:7B:30 bssid_info 0x00000001 reg 0x0b channel 0x0d phytype 0x00 */
	while ((line = strsep(&p, "\n"))) {
		if (WARN_ON(*nr >= max))
			break;

		q = strstr(line, "bssid");
		if (!q)
			continue;

		if (WARN_ON(sscanf(q, "bssid %hhx:%hhx:%hhx:%hhx:%hhx:%hhx bssid_info %x reg %hhx channel %hhx phytype %hhx",
			    &neigh->bssid[0], &neigh->bssid[1], &neigh->bssid[2],
			    &neigh->bssid[3], &neigh->bssid[4], &neigh->bssid[5],
			    &neigh->bssid_info, &neigh->reg, &neigh->channel, &neigh->phy) != 10))
			continue;
		(*nr)++;
		neigh++;
	}

	return 0;
}

static int bcm_get_neighbor_list(const char *ifname, struct nbr *nbrs, int *nr)
{
	return bcm_get_neighbor_list_static(ifname, nbrs, nr);
}

static int bcm_add_neighbor(const char *ifname, struct nbr nbr)
{
	char buf[2048] = {};

	/* bssid, bssid_info, regulatory, channel, phytype */
	chrCmd(buf, sizeof(buf), "wl -i %s rrm_nbr_add_nbr " MACSTR " %u %u %u %u",
	       ifname, MAC2STR(nbr.bssid), nbr.bssid_info, nbr.reg, nbr.channel,
	       nbr.phy);

	return 0;
}

static int bcm_del_neighbor(const char *ifname, unsigned char *bssid)
{
	char buf[2048] = {};

	chrCmd(buf, sizeof(buf), "wl -i %s rrm_nbr_del_nbr " MACSTR, ifname, MAC2STR(bssid));

	return 0;
}

static int bcm_req_beacon_report(const char *ifname, uint8_t *sta,
		struct wifi_beacon_req *param, size_t param_sz)
{
	char buf[512] = {0};
	bcnreq_t bcnreq;
	int swap;

	if (sta == NULL) {
		libwifi_err("%s(): invalid args!\n", __func__);
		return -1;
	}

	swap = wl_swap(ifname);

	memset(&bcnreq, 0, sizeof(bcnreq_t));
	memcpy(bcnreq.da.octet, sta, 6);
	bcnreq.dur = swap ? BCMSWAP32(50) : 50;
	/* TODO: check driver sets other params as we desire */

	if (wl_iovar_set(ifname, "rrm_bcn_req", &bcnreq, sizeof(bcnreq),
						buf, 512) < 0) {
		libwifi_err("wl_iovar_set error!!\n");
		return -1;
	}

	return 0;
}

static int bcm_get_beacon_report(const char *ifname, uint8_t *sta,
		struct sta_nbr *snbr, int *nr)
{
	*nr = 0;
	return 0;
}

int bcm_req_bss_transition(const char *ifname, unsigned char *sta,
			int bsss_nr, unsigned char *bsss, unsigned int tmo)
{
	struct bss_transition_params params = { 0 };

	libwifi_dbg("[%s] %s called " MACSTR " nr %d\n", ifname, __func__,
		    MAC2STR(sta), bsss_nr);

	params.valid_int = tmo;
	params.pref = bsss_nr ? 1 : 0;
	params.disassoc_imminent = 1;

	return hostapd_cli_iface_req_bss_transition(ifname, sta, bsss_nr, bsss, &params);
}


int bcm_sta_get_ap_info(const char *ifname, struct wifi_bss *info)
{
	//TODO
	return 0;
}


int bcm_sta_info(const char *ifname, struct wifi_sta *sta)
{
	//TODO
	return 0;
}

int bcm_sta_disconnect_ap(const char *ifname, uint32_t reason)
{
	//TODO
	return 0;
}

int bcm_driver_info(const char *name, struct wifi_metainfo *info)
{
	uint8_t buf[256] = {0};
	wlc_rev_info_t rev;
	int swap = 0;
	int ret;

	swap = wl_swap(name);

	ret = wl_iovar_get_noparam(name, "ver", buf, sizeof(buf));
	if (!ret) {
		memcpy(info->fw_data, buf, sizeof(info->fw_data) - 1);
	}

	memset(&rev, 0, sizeof(rev));
	ret |= wl_ioctl(name, WLC_GET_REVINFO, &rev, sizeof(rev));
	if (!ret) {
		uint32_t venid, devid;

		venid = swap ? BCMSWAP32(rev.vendorid) : rev.vendorid;
		devid = swap ? BCMSWAP32(rev.deviceid) : rev.deviceid;

		snprintf(info->vendor_id, 7, "0x%4x", (uint16_t)venid);
		snprintf(info->device_id, 7, "0x%4x", (uint16_t)devid);
	}

	return ret;
}

int bcm_add_iface(const char *name, enum wifi_mode m, char *argv[])
{
	int i = 0;

	while (argv[i]) {
		libwifi_dbg("argv[%d] = %s\n", i, argv[i]);
		i++;
	}

	return 0;
}

int bcm_del_iface(const char *name, const char *ifname)
{
	//TODO

	return 0;
}

int bcm_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char iebuf[512] = { 0 };
	size_t ielen;

	ielen = req->ie.ie_hdr.len;
	if (ielen > 254)
		return -1;

	btostr(req->ie.data, ielen - 3, iebuf);
	runCmd("wlctl -i %s add_ie %d %d %02x:%02x:%02x %s",
	       ifname, 15, ielen,
	       req->ie.oui[0], req->ie.oui[1], req->ie.oui[2],
	       iebuf);

	return 0;
}

int bcm_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char iebuf[512] = { 0 };
	size_t ielen;

	ielen = req->ie.ie_hdr.len;
	if (ielen > 254)
		return -1;

	btostr(req->ie.data, ielen - 3, iebuf);
	runCmd("wlctl -i %s del_ie %d %d %02x:%02x:%02x %s",
	       ifname, 15, ielen,
	       req->ie.oui[0], req->ie.oui[1], req->ie.oui[2],
	       iebuf);

	return 0;
}

const struct wifi_driver bcm_driver = {
	.name = "wl",
	.info = bcm_driver_info,
	/* .get_device_id = bcm_get_device_id, */
	.scan = bcm_scan,
	.get_scan_results = bcm_get_scan_results,
	.get_bssid = bcm_get_bssid,
	.get_ssid = bcm_get_ssid,
	.get_maxrate = bcm_get_maxrate,
	.get_channel = bcm_get_channel,
	.get_bandwidth = bcm_get_bandwidth,
	.get_oper_stds = bcm_get_oper_stds,
	.get_supp_channels = bcm_get_supp_channels,
	.get_noise = bcm_get_noise,
	.acs = bcm_acs,
	.radio.info = bcm_get_radio_info,
	.radio.get_stats = bcm_radio_get_stats,
	.iface.get_stats = bcm_ap_get_stats,
	.iface.get_security = bcm_get_security,
	.get_beacon_ies = bcm_get_beacon_ies,
	.add_iface = bcm_add_iface,
	.del_iface = bcm_del_iface,
	.iface.add_vendor_ie = bcm_add_vendor_ie,
	.iface.del_vendor_ie = bcm_del_vendor_ie,

	/* .get_ifstatus = bcm_get_ifstatus, */
	.radio.get_caps = bcm_radio_get_caps,
	.iface.get_caps = bcm_ap_get_caps,
	.set_channel = bcm_set_channel,
	.get_oper_band = bcm_get_oper_band,
	.get_country = bcm_get_country,
	.iface.ap_info = bcm_get_ap_info,
	.get_bss_scan_result = bcm_get_bss_scanresult,
	.disconnect_sta = bcm_disconnect_sta,
	.monitor_sta = bcm_monitor_sta,
	.get_monitor_sta = bcm_get_monitor_sta,
	.get_assoclist = bcm_get_assoclist,
	.get_sta_info = bcm_get_sta_info,
	.iface.probe_sta = bcm_probe_sta,
	/* .get_sta_stats = bcm_get_sta_stats, */
	.iface.start_wps = bcm_start_wps,
	.iface.stop_wps = bcm_stop_wps,
	.iface.get_wps_status = bcm_get_wps_status,
	.iface.get_wps_pin = bcm_get_wps_pin,
	.iface.set_wps_pin = bcm_set_wps_pin,
	.iface.get_neighbor_list = bcm_get_neighbor_list,
	.iface.add_neighbor = bcm_add_neighbor,
	.iface.del_neighbor = bcm_del_neighbor,
	.iface.req_beacon_report = bcm_req_beacon_report,
	.iface.get_beacon_report = bcm_get_beacon_report,
	.iface.req_bss_transition = bcm_req_bss_transition,
	.iface.sta_disconnect_ap = bcm_sta_disconnect_ap,
	.iface.sta_get_ap_info = bcm_sta_get_ap_info,
	.iface.sta_info = bcm_sta_info,
};
