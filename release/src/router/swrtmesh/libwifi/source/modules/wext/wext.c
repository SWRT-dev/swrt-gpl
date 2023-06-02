/*
 * wext.c - implements Wireless Extension SIOCIW*** ioctls
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

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <fcntl.h>
#include <unistd.h>
#include <net/if.h>
#include <linux/wireless.h>

#include "debug.h"
#include "util.h"
#include "wifi.h"

int wext_scan(const char *ifname, struct scan_param *p)
{
	int ret;
	struct iwreq wrq;
	struct iw_scan_req sreq;
	int s;

	memset(&wrq, 0 ,sizeof(struct iwreq));
	snprintf(wrq.ifr_name, 16, "%s", ifname);

	if (p && p->ssid[0]) {
		sreq.essid_len = strlen(p->ssid);
		memcpy(sreq.essid, p->ssid, strlen(p->ssid));
		wrq.u.data.pointer = (caddr_t) &sreq;
		wrq.u.data.length = sizeof(sreq);
		wrq.u.data.flags = IW_SCAN_THIS_ESSID;
	}

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		libwifi_err("%s(): socket error!\n", __func__);
		return -1;
	}

	ret = ioctl(s, SIOCSIWSCAN, &wrq);
	close(s);

	return ret;
}

int wext_get_scan_results(const char *ifname, struct wifi_bss *bsss,
							int *num)
{
	return -ENOTSUP;
}

int wext_get_ssid(const char *ifname, char *ssid)
{
	int ret;
	struct iwreq wrq;
	uint8_t ssidbuf[33] = {0};
	int s;

	memset(&wrq, 0 ,sizeof(struct iwreq));
	snprintf(wrq.ifr_name, 16, "%s", ifname);
	wrq.u.essid.pointer = (caddr_t) ssidbuf;
	wrq.u.essid.length = sizeof(ssidbuf);

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		libwifi_err("%s(): socket error!\n", __func__);
		return -1;
	}

	ret = ioctl(s, SIOCGIWESSID, &wrq);
	if (!ret)
		memcpy(ssid, ssidbuf, 32);

	close(s);

	return ret;
}

int wext_get_bssid(const char *ifname, uint8_t *bssid)
{
	int ret;
	struct iwreq wrq;
	uint8_t bssidbuf[6] = {0};
	int s;

	memset(&wrq, 0 ,sizeof(struct iwreq));
	snprintf(wrq.ifr_name, 16, "%s", ifname);
	wrq.u.data.pointer = (char *)&bssidbuf;
	wrq.u.data.length = 0;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		libwifi_err("%s(): socket error!\n", __func__);
		return -1;
	}

	ret = ioctl(s, SIOCGIWAP, &wrq);
	if (!ret)
		memcpy(bssid, wrq.u.ap_addr.sa_data, 6);

	close(s);

	return ret;
}

int wext_get_channel_list(const char *ifname, uint32_t *chlist, int *num,
					const char *cc, enum wifi_band band,
					enum wifi_bw bw)
{
	return -ENOTSUP;
}

int wext_get_channel(const char *ifname, uint32_t *channel, enum wifi_bw *bw)
{
	int ret;
	struct iw_freq freq;
	struct iwreq wrq;
	int s;

	memset(&wrq, 0 ,sizeof(struct iwreq));
	snprintf(wrq.ifr_name, 16, "%s", ifname);
	wrq.u.data.pointer = (char *)&freq;
	wrq.u.data.length = 0;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		libwifi_err("%s(): socket error!\n", __func__);
		return -1;
	}

	fcntl(s, F_SETFD, fcntl(s, F_GETFD) | FD_CLOEXEC);
	ret = ioctl(s, SIOCGIWFREQ, &wrq);
	*channel = !ret ? wrq.u.freq.m : 0;

	close(s);

	return ret;
}

int wext_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	int ret;
	struct iwreq wrq;
	struct iw_point encoding;
	int s;

	*enc = 0;
	*auth = 0;
	memset(&wrq, 0 ,sizeof(struct iwreq));
	memset(&encoding, 0, sizeof(struct iw_point));
	snprintf(wrq.ifr_name, 16, "%s", ifname);
	wrq.u.encoding.pointer = (char *)&encoding;
	wrq.u.encoding.length = 0;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (s < 0) {
		libwifi_err("%s(): socket error!\n", __func__);
		return -1;
	}

	fcntl(s, F_SETFD, fcntl(s, F_GETFD) | FD_CLOEXEC);
	ret = ioctl(s, SIOCGIWENCODE, &wrq);
	if (ret != 0) {
		close(s);
		ret = -errno;
		libwifi_err("%s: %d\n", __func__, __LINE__);
		return ret;
	}

	if (!!(wrq.u.encoding.flags & IW_ENCODE_ENABLED)) {
		*enc = CIPHER_WEP;
		if (!!(wrq.u.encoding.flags & IW_ENCODE_OPEN))
			*auth = AUTH_OPEN;
		else if (!!(wrq.u.encoding.flags & IW_ENCODE_RESTRICTED))
			*auth = AUTH_SHARED;  /* not really */
	} else if (!!(wrq.u.encoding.flags & IW_ENCODE_DISABLED)) {
		*enc = CIPHER_NONE;
		*auth = AUTH_OPEN;
	}

	close(s);

	return ret;
}

/* do not use wext_driver in newer implementations */
const struct wifi_driver wext_driver = {
	.name = "wlan",
	.scan = wext_scan,
	.get_scan_results = wext_get_scan_results,
	.get_bssid = wext_get_bssid,
	.get_ssid = wext_get_ssid,
	.get_channel = wext_get_channel,
	.get_supp_channels = wext_get_channel_list,
	.iface.get_security = wext_get_security,
};
