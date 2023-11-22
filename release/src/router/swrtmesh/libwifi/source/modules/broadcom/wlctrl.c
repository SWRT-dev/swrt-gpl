/*
 * wlctrl.c - broadcom related utilities
 *
 * Copyright (C) 2021 iopsys Software Solutions AB. All rights reserved.
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

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <net/if.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <net/if.h>
#include <time.h>
#include <syslog.h>
#include <unistd.h>
#include <stdlib.h>
#include <endian.h>
#include <byteswap.h>
#include <sys/ioctl.h>
#include <wlc_types.h>
#include <linux/filter.h>
#include <linux/socket.h>

#include <easy/easy.h>
#include "debug.h"
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "wlctrl.h"

#include "bcmwifi_rspec.h"
#include "brcm_nl80211.h"
#include "wpactrl_util.h"

#define BCMSWAP16 bswap_16
#define BCMSWAP32 bswap_32
#define BCMSWAP64 bswap_64

struct wl_arg {
	const char *ifname;

	int cmd;		/* eg WLC_GET_VAR */
	const char *iovar;	/* optional */

	const void *param;	/* optional */
	int paramlen;

	void *buf;		/* output buffer */
	int buflen;

	bool set;
	int len;		/* return len */
};

static int f2c(int freq)
{
	int chan;

	if (freq > 5000)
		chan = (freq - 5000) / 5;
	else if (freq == 2484)
		chan = 14;
	else
		chan = (freq - 2407) / 5;

	return chan;
}

static int c2f(int chan)
{
	int freq = 0;

	if (chan >= 1 && chan <= 13)
		freq = 2407 + chan * 5;
	if (chan == 14)
		freq = 2484;
	if (chan >= 36)
		freq = 5000 + chan * 5;

	return freq;
}

static uint32_t cac_time(const char *cc, int chan)
{
	uint32_t cac_time = 60;

	if (!strcmp(cc, "US") || !strcmp(cc, "JP"))
		return cac_time;

	if (chan == 120 || chan == 124 || chan == 128)
		cac_time = 600;

	return cac_time;
}

static int wl_ioctl(struct wl_arg *arg)
{
	char buf[WLC_IOCTL_MAXLEN];
	struct ifreq ifr;
	wl_ioctl_t wioc;
	int len = 0;
	int ret = -1;
	int s;

	if (WARN_ON(arg->buflen > sizeof(buf)))
		goto exit;

	memset(buf, 0, sizeof(buf));
	memset(&wioc, 0, sizeof(wioc));
	memset(&ifr, 0, sizeof(ifr));

	wioc.cmd = arg->cmd;
	wioc.buf = buf;
	wioc.len = sizeof(buf);
	wioc.set = arg->set;

	if (arg->iovar) {
		len = strlen(arg->iovar) + 1;
		if (WARN_ON(len > sizeof(buf)))
			goto exit;

		snprintf(buf, WLC_IOCTL_MAXLEN, "%s", arg->iovar);
		if (arg->cmd == WLC_SET_VAR)
			wioc.len = len + arg->paramlen;
	}

	if (arg->param) {
		if (WARN_ON((len + arg->paramlen) > sizeof(buf)))
			goto exit;
		memcpy(&buf[len], arg->param, arg->paramlen);
	}

	strncpy(ifr.ifr_name, arg->ifname, IFNAMSIZ);
	ifr.ifr_data = (caddr_t) &wioc;

	s = socket(AF_INET, SOCK_DGRAM, 0);
	if (WARN_ON(s < 0))
		goto exit;

	fcntl(s, F_SETFD, fcntl(s, F_GETFD) | FD_CLOEXEC);

	ret = ioctl(s, SIOCDEVPRIVATE, &ifr);
	close(s);

	if (ret)
		goto exit;

	arg->len = wioc.len;

	if (arg->buf)
		memcpy(arg->buf, buf, arg->buflen);

exit:
	libwifi_dbg("[%s] %s cmd %d %s ret %d\n", arg->ifname, __func__, wioc.cmd, arg->iovar ?: "", ret);
	return ret;
}

static int wl_iovar_set(const char *ifname, char *iovar, void *param, int paramlen,
			void *buf, int buflen)
{
	struct wl_arg arg = {
		.ifname = ifname,
		.cmd = WLC_SET_VAR,
		.iovar = iovar,
		.param = param,
		.paramlen = paramlen,
		.buf = buf,
		.buflen = buflen
	};

	return wl_ioctl(&arg);
}

static int wl_iovar_get_noparam(const char *ifname, char *iovar, void *buf, int buflen)
{
	struct wl_arg arg = {
		.ifname = ifname,
		.cmd = WLC_GET_VAR,
		.iovar = iovar,
		.buf = buf,
		.buflen = buflen
	};

	return wl_ioctl(&arg);
}

static int wl_iovar_get(const char *ifname, char *iovar, void *param, int paramlen,
			void *buf, int buflen)
{
	struct wl_arg arg = {
		.ifname = ifname,
		.cmd = WLC_GET_VAR,
		.iovar = iovar,
		.param = param,
		.paramlen = paramlen,
		.buf = buf,
		.buflen = buflen
	};

	return wl_ioctl(&arg);
}

static int wl_swap(const char *ifname)
{
	unsigned int magic = 0;
	struct wl_arg arg = {
		.ifname = ifname,
		.cmd = WLC_GET_MAGIC,
		.buf = &magic,
		.buflen = sizeof(magic)
	};

	if (WARN_ON(wl_ioctl(&arg) < 0))
		return 0; /* don't swap on error */

	/* is endian swap needed */
	if (magic == WLC_IOCTL_MAGIC)
		return 0;
	else if (bswap_32(magic) == WLC_IOCTL_MAGIC)
		return 1;
	else
		libwifi_err("[%s] wrong magic, driver 0x%x vs header 0x%x\n", ifname, magic, WLC_IOCTL_MAGIC);

	return 0;
}

static uint16_t wl_swap_16(const char *ifname, uint16_t val)
{
	return wl_swap(ifname) ? bswap_16(val) : val;
}

static uint32_t wl_swap_32(const char *ifname, uint32_t val)
{
	return wl_swap(ifname) ? bswap_32(val) : val;
}

static uint64_t wl_swap_64(const char *ifname, uint64_t val)
{
	return wl_swap(ifname) ? bswap_64(val) : val;
}

static int wl_vendor(struct wl_arg *arg)
{
	struct bcm_nlmsg_hdr *req;
	uint8_t out[WLC_IOCTL_MAXLEN];
	uint8_t in[2048];
	int outlen = 0;
	int inlen = 0;
	int len = 0;
	int ret;

	memset(out, 0, sizeof(out));
	memset(in, 0, sizeof(in));

	req = (struct bcm_nlmsg_hdr *) in;
	inlen = sizeof(*req);

	req->cmd = arg->cmd;
	req->len = arg->buflen;

	req->offset = sizeof(*req);
	req->set = arg->set;

	if (arg->iovar) {
		len = strlen(arg->iovar) + 1;
		if (WARN_ON((len + sizeof(*req)) > sizeof(in)))
			return -1;

		snprintf((char *)in + req->offset, sizeof(in) - req->offset, "%s", arg->iovar);
		inlen += len;
	}

	if (arg->param) {
		if (WARN_ON((len + arg->paramlen + sizeof(*req)) > sizeof(in)))
			return -1;
		memcpy(&in[len + req->offset], arg->param, arg->paramlen);
		inlen += arg->paramlen;
	}

	ret = nlwifi_vendor_cmd(arg->ifname, OUI_BRCM, BRCM_VENDOR_SCMD_PRIV_STR,
				in, inlen, out, &outlen);

	libwifi_dbg("[%s] %s called cmd %d ret %d and len %d\n", arg->ifname, __func__, arg->cmd, ret, outlen);

	arg->len = outlen;

	if (arg->buf)
		memcpy(arg->buf, out, arg->buflen);

	return ret;
}

static int wl_ctl(struct wl_arg *arg)
{
	int ret;

	/* Today a lot of vendor commands fail - prefer ioctl */
	return wl_ioctl(arg);

	ret = wl_vendor(arg);
	if (ret)
		ret = wl_ioctl(arg);

	return ret;
}

static const uint8_t wf_chspec_bw_mhz[] = {5, 10, 20, 40, 80, 160, 160};
static const uint8 wf_5g_80m_chans[] = {42, 58, 106, 122, 138, 155};
#define WF_NUM_BW (sizeof(wf_chspec_bw_mhz)/sizeof(uint8_t))

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

static chanspec_t ctrlchannel_to_chanspec(const char *ifname, uint32_t ch,
					  enum wifi_bw bw)
{
	char buf[2048] = {0};
	uint32_t ctrlch_lower[] = {1, 2, 3, 4, 5, 6, 7, 8, 9,
				 36, 44, 52, 60,
				 100, 108, 116, 124, 132, 140,
				 149, 157};
	enum wifi_band band;
	char bwstr[16] = {0};
	bool bw_sep = true;
	unsigned int cs = 0;


	if (bcmwl_get_oper_band(ifname, &band))
		return 0;

	if (!(band == BAND_2 && ch > 0 && ch <= 14) &&
		!(band == BAND_5 && ch >= 36 && ch < 200))
		return 0;

	if (bw == BW20)
		bw_sep = false;

	if (bw == BW40) {
		int i;

		bw_sep = false;
		strcpy(bwstr, "u");

		for (i = 0; i < ARRAY_SIZE(ctrlch_lower); i++) {
			if (ch == ctrlch_lower[i]) {
				strcpy(bwstr, "l");
				break;
			}
		}
	}

	if (bw == BW80 || bw == BW160)
		snprintf(bwstr, 15, "%d", 20 << bw);

	chrCmd(buf, sizeof(buf),
	       "wl -i %s chanspecs | grep %d%s%s | cut -d ' ' -f2",
	       ifname, ch, bw_sep ? "/" : "", bwstr);

	if (buf[0])
		sscanf(buf, "(%x)", &cs);

	return (chanspec_t)cs;
}

static int wl_ioctl_iface_get_beacon(const char *name, uint8_t *bcn, size_t *len)
{
	char buf[WLC_IOCTL_MAXLEN] = { 0 };
	int ret;
	uint32_t bcn_len;

	if (WARN_ON(!bcn) || WARN_ON(!len) || WARN_ON(*len == 0))
		return -EINVAL;

	memset(bcn, 0, *len);

	ret = wl_iovar_get_noparam(name, "beacon_info", (void*)buf, WLC_IOCTL_MAXLEN);

	if (WARN_ON(ret))
		return ret;

	bcn_len = wl_swap_32(name, *(uint32_t*)buf);

	if (WARN_ON(bcn_len == 0 || *len < bcn_len )) {
		*len = 0;
		return -1;
	}

	memcpy(bcn, buf + 4, bcn_len);
	*len = bcn_len;

	return 0;
}

static int wl_ioctl_get_version(const char *name, char *version, size_t *len)
{
	char buf[256] = { 0 };
	const char *seps = "\t\n\v\f\r";
	int ret;
	int i;

	memset(version, 0, *len);

	ret = wl_iovar_get_noparam(name, "ver", (void*)buf, 256);
	if (WARN_ON(ret))
		return ret;

	i = strlen(buf) - 1;
	while (i > 0 && strchr(seps, buf[i]) != NULL) {
		buf[i] = '\0';
		i--;
	}
	if (*len > strlen(buf)) {
		*len = strlen(buf);
		strncpy(version, buf, *len);
	} else {
		*len -= 1;
		strncpy(version, buf, *len);
		version[*len] = '\0';
	}

	return 0;
}

static int wl_ioctl_iface_get_assoclist_max(const char *name, uint32_t *max)
{
	uint32_t assoclist_max;
	int ret;

	ret = wl_iovar_get_noparam(name, "bss_maxassoc", &assoclist_max, sizeof(assoclist_max));
	if (WARN_ON(ret))
		return ret;

	*max = wl_swap_32(name, assoclist_max);
	return ret;
}

int wl_ioctl_iface_get_assoc_info(const char *name, struct wifi_sta *sta)
{
	unsigned char buf[WLC_IOCTL_MAXLEN] = { 0 };
	wl_assoc_info_t assoc_info = {0};
	uint32_t req_ies_len = 0, resp_ies_len = 0;
	int swap = wl_swap(name);
	uint8_t *ie_ptr = NULL;
	int ret;

	libwifi_dbg("[%s] %s caleld\n", name, __func__);

	ret = wl_iovar_get_noparam(name, "assoc_info", buf, WLC_IOCTL_MAXLEN);
	if (WARN_ON(ret))
		return ret;

	memcpy(&assoc_info, buf, sizeof(assoc_info));
	if (swap) {
		assoc_info.req_len = BCMSWAP32(assoc_info.req_len);
		assoc_info.resp_len = BCMSWAP32(assoc_info.resp_len);
		assoc_info.flags = BCMSWAP32(assoc_info.flags);
	}

	libwifi_dbg("[%s] %s flags 0x%x req_len %d resp_len %d\n", name, __func__, assoc_info.flags, assoc_info.req_len, assoc_info.resp_len);

	if (assoc_info.req_len) {
		assoc_info.req.capability = le16toh(assoc_info.req.capability);
		assoc_info.req.listen = le16toh(assoc_info.req.listen);
		req_ies_len = assoc_info.req_len - sizeof(struct dot11_assoc_req);

		libwifi_dbg("[%s] capability 0x%x\n", name, assoc_info.req.capability);
		ret = wl_iovar_get_noparam(name, "assoc_req_ies", buf, WLC_IOCTL_MAXLEN);
		if (WARN_ON(ret))
			return ret;

		/* capabilities information */
		ie_ptr = (uint8_t *)&assoc_info.req.capability;
		sta->caps.valid |= WIFI_CAP_BASIC_VALID;
		memcpy(&sta->caps.basic, ie_ptr, sizeof(assoc_info.req.capability));
		wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, sizeof(assoc_info.req.capability));

		/* ht capabilities */
		ie_ptr = wifi_find_ie(buf, req_ies_len, IE_HT_CAP);
		if (ie_ptr) {
			sta->caps.valid |= WIFI_CAP_HT_VALID;
			memcpy(&sta->caps.ht, &ie_ptr[2], ie_ptr[1]);
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}

		/* vht capabilities */
		ie_ptr = wifi_find_ie(buf, req_ies_len, IE_VHT_CAP);
		if (ie_ptr) {
			sta->caps.valid |= WIFI_CAP_VHT_VALID;
			memcpy(&sta->caps.vht, &ie_ptr[2], ie_ptr[1]);
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}

		/* extended capabilities */
		ie_ptr = wifi_find_ie(buf, req_ies_len, IE_EXT_CAP);
		if (ie_ptr) {
			sta->caps.valid |= WIFI_CAP_EXT_VALID;
			memcpy(&sta->caps.ext, &ie_ptr[2], ie_ptr[1]);
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}

		/* RM enabled capabilities */
		ie_ptr = wifi_find_ie(buf, req_ies_len, IE_RRM);
		if (ie_ptr) {
			sta->caps.valid |= WIFI_CAP_RM_VALID;
			memcpy(&sta->caps.ext, &ie_ptr[2], ie_ptr[1]);
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}

		/* mobility domain element */
		ie_ptr = wifi_find_ie(buf, req_ies_len, IE_MDE);
		if (ie_ptr) {
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}

		/* HE capabilities */
		ie_ptr = wifi_find_ie_ext(buf, req_ies_len, IE_EXT_HE_CAP);
		if (ie_ptr) {
			sta->caps.valid |= WIFI_CAP_HE_VALID;
			memcpy(&sta->caps.he, &ie_ptr[3], min(ie_ptr[1], sizeof(struct wifi_caps_he)));
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}

		/* WMM */
		ie_ptr = wifi_find_vsie(buf, req_ies_len, microsoft_oui, 2, 1);
		if (ie_ptr) {
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}
	}

	if (assoc_info.resp_len) {
		assoc_info.resp.capability = le16toh(assoc_info.resp.capability);
		assoc_info.resp.status = le16toh(assoc_info.resp.status);
		assoc_info.resp.aid = le16toh(assoc_info.resp.aid);
		resp_ies_len = assoc_info.resp_len - sizeof(struct dot11_assoc_req);

		libwifi_dbg("[%s] capability 0x%x\n", name, assoc_info.resp.capability);
		ret = wl_iovar_get_noparam(name, "assoc_resp_ies", buf, WLC_IOCTL_MAXLEN);
		WARN_ON(ret);

		/* WMM */
		ie_ptr = wifi_find_vsie(buf, resp_ies_len, microsoft_oui, 2, 1);
		if (ie_ptr) {
			wifi_cap_set_from_ie(sta->cbitmap, ie_ptr, ie_ptr[1] + 2);
		}
	}

	return 0;
}

#ifndef LIBWIFI_USE_WL_IOCTL
static int wl_cli_get_noise(const char *name, int *noise)
{
	char buf[64] = {0};
	int val = 0;
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s noise", name);

	if (WARN_ON(status))
		return status;

	if (WARN_ON(sscanf(buf, "%d", &val) != 1))
		return -1;

	*noise = val;
	return 0;
}

int bcmwl_radio_get_noise(const char *name, int *noise)
{
	uint32_t val = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_PHY_NOISE,
		.buf = &val,
		.buflen = sizeof(val)
	};

	ret = wl_ioctl(&arg);
	if (!ret) {
		*noise = wl_swap_32(name, val);
		return ret;
	}

	libwifi_dbg("[%s] %s fallback to wl\n", name, __func__);
	ret = wl_cli_get_noise(name, noise);

	return ret;
}

static int bcmwl_get_country(const char *name, char *alpha2)
{
	char buf[256] = {0};
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s country", name);

	if (WARN_ON(status))
		return status;

	if (WARN_ON(snprintf(alpha2, 3, "%s", buf) < 2))
		return -1;

	return 0;
}

int bcmwl_radio_get_countrylist(const char *name, char *cc, int *num)
{
	char buf[WLC_IOCTL_MAXLEN] = { 0 };
	int ret;
	int i = 0, count = 0;
	const char *abbrev;
	wl_country_list_t *cll = NULL;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	wl_country_list_t cl = { 0 };

	cl.buflen = WLC_IOCTL_MAXLEN;
	cl.count = 0;
	cl.band_set = FALSE;

	cl.buflen = BCMSWAP32(cl.buflen);
	cl.band_set = BCMSWAP32(cl.band_set);
	cl.band = BCMSWAP32(cl.band);
	cl.count = BCMSWAP32(cl.count);

	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_COUNTRY_LIST,
		.param = &cl,
		.paramlen = sizeof(wl_country_list_t),
		.buf = buf,
		.buflen = (sizeof(buf) - sizeof(wl_country_list_t))
	};

	ret = wl_ctl(&arg);

	if (WARN_ON(ret))
		return ret;

	cll = (wl_country_list_t *)buf;

	for (i = 0; i < cll->count; i++) {
		abbrev = &cll->country_abbrev[i*WLC_CNTRY_BUF_SZ];
		strncpy(&cc[i*WLC_CNTRY_BUF_SZ], abbrev, WLC_CNTRY_BUF_SZ);
		count++;
	}

	*num = (count * WLC_CNTRY_BUF_SZ);
	return ret;
}


int bcmwl_radio_get_country(const char *name, char *alpha2)
{
       char val[3];
       int ret;
       struct wl_arg arg = {
               .ifname = name,
               .cmd = WLC_GET_COUNTRY,
               .buf = &val,
               .buflen = sizeof(val)
       };

       libwifi_dbg("[%s] %s called\n", name, __func__);

       ret = wl_ctl(&arg);
       if (!ret) {
               snprintf(alpha2, 3, "%s", val);
               return ret;
       }

       libwifi_dbg("[%s] %s fallback to wl\n", name, __func__);
       ret = bcmwl_get_country(name, alpha2);

       return ret;
}


static int bcmwl_get_rssi(const char *name, int8_t *rssi)
{
	char buf[64] = {0};
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s rssi", name);
	if (WARN_ON(status))
		return status;

	if (WARN_ON(sscanf(buf, "%hhi", rssi) != 1))
		return -1;

	return 0;
}

static int bcmwl_get_macaddr(const char *name, uint8_t *macaddr)
{
	char buf[512] = {0};
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s cur_etheraddr", name);
	if (WARN_ON(status))
		return status;

	status = sscanf(buf, "cur_etheraddr: %2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
			&macaddr[0], &macaddr[1], &macaddr[2],
			&macaddr[3], &macaddr[4], &macaddr[5]);

	if (WARN_ON(status != 6))
		return -1;

	return 0;
}

static int bcmwl_get_bssid(const char *name, uint8_t *bssid)
{
	char buf[512] = {0};
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s bssid", name);
	if (WARN_ON(status))
		return status;

	status = sscanf(buf, "%2hhx:%2hhx:%2hhx:%2hhx:%2hhx:%2hhx",
			&bssid[0], &bssid[1], &bssid[2],
			&bssid[3], &bssid[4], &bssid[5]);

	if (WARN_ON(status != 6))
		return -1;

	return 0;
}

static const int * bcmwl_get_chans_from_line(const char *line)
{
	const int *chans = NULL;
	char *chanspec;
	char *block;
	char *ptr;
	int chan;
	int bw;

	ptr = strdupa(line);
	if (!ptr)
		return chans;

	while ((block = strsep(&ptr, ","))) {
		chanspec = strstr(block, "chanspec:");
		if (!chanspec)
			continue;
		if (strstr(block, "chanspec: none"))
			break;

		chanspec = chanspec + strlen("chanspec: ");

		if (strstr(chanspec, "u") || strstr(chanspec, "l")) {
			chan = atoi(chanspec);
			bw = 40;
		} else if (strstr(chanspec, "/")) {
			chan = atoi(chanspec);
			bw = atoi(strstr(chanspec, "/") + 1);
		} else	{
			chan = atoi(chanspec);
			bw = 20;
		}

		chans = chan2list(chan, bw);
		break;
	}

	return chans;
}

static void bcmwl_radio_channels_update_preism(const char *ifname, struct chan_entry *channel, int num)
{
	char dfs_status[4096] = {0};
	const int *chans;
	char *line;
	char *ptr;
	int i;

	if (WARN_ON(Cmd(dfs_status, sizeof(dfs_status), "wl -i %s dfs_ap_move", ifname)))
		return;

	/*
	 * Check two lines of dfs_ap_move status:
	 * version=1, move status=3
	 * AP Target Chanspec 36/160 (0xe832)
	 * Radar Scan Aborted
	 * version: 1, num_sub_status: 2
	 * @0: state: PRE-ISM Channel Availability Check(CAC), time elapsed: 19050ms, chanspec: 100/80 (0xE06A), chanspec last cleared: none, sub type: 0x00
	 * @1: state: IDLE, time elapsed: 0ms, chanspec: none, chanspec last cleared: none, sub type: 0x01
	 */

	ptr = dfs_status;

	while ((line = strsep(&ptr, "\r\n"))) {
		if (!strstr(line, "@0") && !strstr(line, "@1"))
			continue;

		if (!strstr(line, "PRE-ISM Channel Availability Check"))
			continue;

		libwifi_dbg("line: %s\n", line);
		chans = bcmwl_get_chans_from_line(line);
		if (!chans)
			continue;
		while (*chans) {
			libwifi_dbg("[%s] preism chan %d\n", ifname, *chans);
			for (i = 0; i < num; i++) {
				if (!channel[i].dfs)
					continue;
				if (channel[i].channel == *chans)
					channel[i].dfs_state = WIFI_DFS_STATE_CAC;
			}
			chans++;
		}
	}
}

int bcmwl_iface_chan_switch(const char *ifname, struct chan_switch_param *param)
{
	char buf[1024] = {0};
	char chanspec[128] = {0};
	int bandwidth;
	int channel;
	int count;
	int status;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	count = param->count;
	channel = f2c(param->freq);
	bandwidth = param->bandwidth;

	snprintf(chanspec, sizeof(chanspec), "%d/%d", channel, bandwidth);

	if (bandwidth == 40 && channel <= 14) {
		switch (param->sec_chan_offset) {
		case -1:
			snprintf(chanspec, sizeof(chanspec), "%du", channel);
			break;
		case 1:
			snprintf(chanspec, sizeof(chanspec), "%dl", channel);
			break;
		default:
			break;
		}
	}

	if (wifi_is_dfs_channel(ifname, channel, bandwidth)) {
		/* First try Zero Wait DFS */
		status = Cmd(buf, sizeof(buf), "wl -i %s dfs_ap_move %s", ifname, chanspec);
		WARN_ON(status);
		if (!status)
			return status;
	}

	/* If nonDFS or ZWDFS failed call CSA */
	status = Cmd(buf, sizeof(buf), "wl -i %s csa 0 %d %s", ifname, count, chanspec);
	WARN_ON(status);

	return status;
}

static int bcmwl_get_macmode(const char *name, int *macmode)
{
	char buf[64] = {0};
	int val = 0;
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s macmode", name);

	if (WARN_ON(status))
		return status;

	if (WARN_ON(sscanf(buf, "%d", &val) != 1))
		return -1;

	*macmode = val;
	return 0;
}

static int bcmwl_set_macmode(const char *name, int macmode)
{
	int ret;
	char buf[64] = {0};

	ret = Cmd(buf, sizeof(buf), "wl -i %s macmode %d",
	          name, macmode);

	return ret;
}

static int bcmwl_get_probresp_mac_filter(const char *name, bool *prmf)
{
	char buf[64] = {0};
	int ret, val = 0;

	ret = Cmd(buf, sizeof(buf), "wl -i %s probresp_mac_filter", name);

	if (WARN_ON(ret))
		return -1;

	if (WARN_ON(sscanf(buf, "%d", &val) != 1))
		return -1;

	*prmf = (val == 1) ? true : false;

	return ret;
}

static int bcmwl_set_probresp_mac_filter(const char *name, bool prmf)
{
	int ret;
	char buf[64] = {0};

	ret = Cmd(buf, sizeof(buf), "wl -i %s probresp_mac_filter %d",
	          name, prmf ? 1 : 0);

	return ret;
}

static int bcmwl_iface_is_maclist_empty(const char *name, bool *is_empty)
{
	int ret;
	char buf[WLC_IOCTL_MAXLEN] = {0};

	ret = Cmd(buf, sizeof(buf), "wl -i %s mac", name);
	*is_empty = strlen(buf) ? false : true;

	return ret;
}

static int bcmwl_iface_update_maclist(const char *name, uint8_t *sta, bool remove)
{
	int ret = 0;
	char buf[64] = {0};

	ret = Cmd(buf, sizeof(buf), "wl -i %s mac %s " MACSTR, name,
					remove ? "del" : "", MAC2STR(sta));

	return ret;
}

int bcmwl_radio_reset_chanim_stats(const char *name)
{
	int ret = 0;
	char buf[64] = {0};

	ret = Cmd(buf, sizeof(buf), "wl -i %s chanim_stats us reset", name);

	return ret;
}

static int bcmwl_radio_channels_update_survey(const char *name, struct chan_entry *channel, int num)
{
	uint32_t chanspec, tx, inbss, obss, nocat, nopkt, doze, txop, goodtx, badtx, total_tm, busy_tm;
	char *line, *ptr, *p;
	uint32_t busy;
	int ret = -1;
	char *buf;
	int noise;
	int n;
	int i;

	/* Setup some default values, in case of fail */
	for (i = 0; i < num; i++) {
		channel[i].score = 255;
		channel[i].noise = -90;
	}

	buf = malloc(16*1024);
	if (WARN_ON(!buf))
		return ret;
	memset(buf, 0, 16*1024);

	ret = Cmd(buf, 16*1024, "wl -i %s chanim_stats us all", name);
	if (WARN_ON(ret)) {
		free(buf);
		return ret;
	}

	ptr = buf;

	while ((line = strsep(&ptr, "\r\n"))) {
		n = sscanf(line, "%u %u %u %u %u %u %u %u %u %u %u %u",
			   &chanspec, &tx, &inbss, &obss, &nocat, &nopkt, &doze,
			   &txop, &goodtx, &badtx, &total_tm, &busy_tm);
		if (n != 12)
			continue;

		if (total_tm == 0)
			continue;

		if (busy_tm && (busy_tm < total_tm))
			busy = (100 * busy_tm) / total_tm;
		else
			busy = (100 * (total_tm - txop)) / total_tm;

		libwifi_dbg("chan %u busy %03u%% \t(txop %u total_tm %u busy_tm %u)\n", (chanspec & 0xff) , busy, txop, total_tm, busy_tm);

		for (i = 0; i < num; i++) {
			if (channel[i].channel != (chanspec & 0xff))
				continue;

			/* Fill survey data */
			channel[i].survey.channel_busy = busy_tm;
			channel[i].survey.cca_time = total_tm;
			channel[i].survey.tx_airtime = tx;
			channel[i].survey.rx_airtime = inbss;
			channel[i].survey.obss_airtime = obss;

			/* Fill busy - now is for 20MHz and come from survey data */
			channel[i].busy = busy;

			/* Fill score */
			if (busy <= 100)
				channel[i].score = 100 - busy;
			else
				channel[i].score = 255;

			break;
		}
	}

	/* Get noise */
	memset(buf, 0, 16*1024);
	ret = Cmd(buf, 16*1024, "wl -i %s chanim_stats all", name);
	if (WARN_ON(ret)) {
		free(buf);
		return ret;
	}

	ptr = buf;
	while ((line = strsep(&ptr, "\r\n"))) {
		p = strstr(line, "chanspec:");
		if (!p)
			continue;
		if (sscanf(p, "chanspec: %x", &chanspec) != 1)
			continue;
		p = strstr(line, "noise: ");
		if (!p)
			continue;
		if (sscanf(p, "noise: %d", &noise) != 1)
			continue;

		libwifi_dbg("[%s] chanim_stats chan %d noise %d\n", name, chanspec & 0xff, noise);
		for (i = 0; i < num; i++) {
			if (channel[i].channel != (chanspec & 0xff))
				continue;

			channel[i].noise = noise;
			break;
		}
	}

	free(buf);
	return ret;
}

#else
int bcmwl_radio_get_noise(const char *name, int *noise)
{
	uint32_t val = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_PHY_NOISE,
		.buf = &val,
		.buflen = sizeof(val)
	};

	ret = wl_ioctl(&arg);
	if (!ret)
		*noise = wl_swap_32(name, val);

	return ret;
}

int bcmwl_radio_get_country(const char *name, char *alpha2)
{
	char val[3];
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_COUNTRY,
		.buf = &val,
		.buflen = sizeof(val)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = wl_ctl(&arg);
	if (!ret)
		snprintf(alpha2, 3, "%s", val);

	return ret;
}

static int bcmwl_get_rssi(const char *name, int8_t *rssi)
{
	int ret;
	uint32_t val = 0;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_RSSI,
		.buf = &val,
		.buflen = sizeof(val)
	};

	ret = wl_ioctl(&arg);
	if (!ret)
		*rssi = wl_swap_32(name, val);

	return ret;
}

static int bcmwl_get_macaddr(const char *name, uint8_t *macaddr)
{
	int ret;

	ret = wl_iovar_get_noparam(name, "cur_etheraddr",
			macaddr, ETHER_ADDR_LEN);

	return ret;
}

static int bcmwl_get_bssid(const char *name, uint8_t *bssid)
{
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_BSSID,
		.buf = bssid,
		.buflen = ETHER_ADDR_LEN
	};

	ret = WARN_ON(wl_ioctl(&arg));

	return ret;
}

static int chanspec_to_bandwidth(chanspec_t c)
{
	int bandwidth = 0;

	if (CHSPEC_IS20(c)) bandwidth = 20;
	if (CHSPEC_IS40(c)) bandwidth = 40;
	if (CHSPEC_IS80(c)) bandwidth = 80;
	if (CHSPEC_IS160(c)) bandwidth = 160;
	if (CHSPEC_IS8080(c)) bandwidth = 8080;

	return bandwidth;
}

static void bcmwl_radio_channels_update_preism(const char *ifname, struct chan_entry *channel, int num)
{
	int ret;
	char buf[256] = { 0 };
	struct wl_dfs_ap_move_status_v2 *status_v2;
	wl_dfs_sub_status_t *sub;
	const int *chans = NULL;
	int bw, ctrl_ch;
	int swap;
	int i;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	swap = wl_swap(ifname);

	ret = wl_iovar_get_noparam(ifname, "dfs_ap_move", &buf[0], sizeof(buf));
	if (!ret) {
		status_v2 = (struct wl_dfs_ap_move_status_v2 *)buf;

		if (swap)
			status_v2->scan_status.num_sub_status =
				BCMSWAP16(status_v2->scan_status.num_sub_status);

		/* TODO: check WL_DFS_AP_MOVE_VERSION
		 * & WL_DFS_STATUS_ALL_VERSION
		 */

		if (status_v2->move_status != (int8) DFS_SCAN_S_IDLE) {
			for (i = 0; i < status_v2->scan_status.num_sub_status; i++) {
				sub = &status_v2->scan_status.dfs_sub_status[i];
				if (swap)
					sub->state = BCMSWAP32(sub->state);

				if (sub->state == WL_DFS_CACSTATE_PREISM_CAC) {
					if (swap)
						sub->chanspec = (chanspec_t)BCMSWAP32(sub->chanspec);

					ctrl_ch = chanspec_to_ctrlchannel(sub->chanspec);
					bw = chanspec_to_bandwidth(sub->chanspec);
					chans = chan2list(ctrl_ch, bw);
				}
			}
		}

		while (chans && *chans) {
			libwifi_dbg("[%s] %s preism chan %d\n", ifname, __func__, *chans);
			for (i = 0; i < num; i++) {
				if (!channel[i].dfs)
					continue;
				if (channel[i].channel == *chans)
					channel[i].dfs_state = WIFI_DFS_STATE_CAC;
			}
			chans++;
		}
	}
}

static int bcmwl_set_dfs_ap_move(const char *ifname, chanspec_t cs)
{
	int swap;
	char buf[512] = {0};

	swap = wl_swap(ifname);

	if (swap)
		cs = BCMSWAP16(cs);

	if (wl_iovar_set(ifname, "dfs_ap_move", &cs, sizeof(int), buf, 512) < 0) {
		libwifi_err("wl_iovar_set dfs_ap_move error!!\n");
		return -1;
	}

	return 0;
}

static int bcmwl_set_csa(const char *ifname, uint8_t mode,
			  uint8_t count, chanspec_t cs)
{
	wl_chan_switch_t csa;
	char buf[512] = {0};

	csa.mode = mode;
	csa.count = count;
	csa.reg = 0;
	csa.frame_type = 0;  /* broadcast */
	csa.chspec = cs;

	return wl_iovar_set(ifname, "csa", &csa, sizeof(csa), buf, 512);
}

static chanspec_t bcmwl_channel_to_sb(chanspec_t center_ch, chanspec_t ctl_ch, int bandwidth)
{
	chanspec_t sb;
	chanspec_t lowest;

	/* calculate lowest channel for given center channel and bandwidth */
	lowest = center_ch - (chanspec_t)(((bandwidth - 20) / 2) / 5);

	if ((ctl_ch - lowest) % 4) {
		/* delta must be a multiple of four */
		return -1;
	}

	sb = ((ctl_ch - lowest) / 4);
	if (sb >= (bandwidth / 20)) {
		return -1;
	}

	/* convert here & return 16bit value directly */
	sb = sb << WL_CHANSPEC_CTL_SB_SHIFT;
	return sb;
}

static int bcmwl_get_chsb(int bandwidth, chanspec_t ch_ctl, int sec_ch_offset,
		chanspec_t ch_cen1, chanspec_t ch_cen2,
		chanspec_t *ch, chanspec_t *sb)
{
	*sb = -1;

	switch (bandwidth) {
	case 20:
		*ch = ch_ctl;
		*sb = WL_CHANSPEC_CTL_SB_NONE;
		return 0;
	case 40:
		/* handle 40MHz / g */
		if (ch_ctl <= 14) {
			switch (sec_ch_offset) {
			case -1:
				*ch = LOWER_20_SB(ch_ctl);
				*sb = WL_CHANSPEC_CTL_SB_LLU;
				return 0;
			case 1:
				*ch = UPPER_20_SB(ch_ctl);
				*sb = WL_CHANSPEC_CTL_SB_LLL;
				return 0;
			default:
				*ch = ch_ctl;
				*sb = WL_CHANSPEC_CTL_SB_NONE;
				return 0;
			}
		}
		/* no break */
	case 80:
	case 160:
		*ch = ch_cen1;
		/* sec_ch_offset ignored for 40/80/160, calculate instead */
		*sb = bcmwl_channel_to_sb(ch_cen1, ch_ctl, bandwidth);
		break;
	case 8080:
		*ch = (((chanspec_t)ch_cen1 << WL_CHANSPEC_CHAN0_SHIFT) |
		       ((chanspec_t)ch_cen2 << WL_CHANSPEC_CHAN1_SHIFT));
		/* sec_ch_offset ignored for 8080, calculate instead */
		*sb = bcmwl_channel_to_sb(ch_cen1, ch_ctl, 80);
		break;
	default:
		return -1;
	}

	/* no matching sideband */
	if (*sb < 0)
		return -1;

	return 0;
}

int bcmwl_iface_chan_switch(const char *ifname, struct chan_switch_param *param)
{
	int status;
	chanspec_t cs = {0};
	chanspec_t chspec_ch, chspec_band, chspec_bw, chspec_sb;
	int bandwidth;
	int ch_ctl, ch_cen1, ch_cen2;
	int sec_chan_offset;
	int count;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	count = param->count;
	ch_ctl = f2c(param->freq);
	ch_cen1 = f2c(param->cf1);
	ch_cen2 = f2c(param->cf2);
	bandwidth = param->bandwidth;
	sec_chan_offset = param->sec_chan_offset;

	/* figure out chspec band */
	chspec_band = ((ch_ctl  <= CH_MAX_2G_CHANNEL) ?
	                WL_CHANSPEC_BAND_2G : WL_CHANSPEC_BAND_5G);
	chspec_ch = ch_ctl;

	/* figure out chspec bw */
	switch (bandwidth) {
		case 20:
			chspec_bw = WL_CHANSPEC_BW_20;
			break;
		case 40:
			chspec_bw = WL_CHANSPEC_BW_40;
			break;
		case 80:
			chspec_bw = WL_CHANSPEC_BW_80;
			break;
		case 160:
			chspec_bw = WL_CHANSPEC_BW_160;
			break;
		case 8080:
			chspec_bw = WL_CHANSPEC_BW_8080;
			break;
		default:
			return -1;
	}

	/* figure out chspec sideband and channel */
	bcmwl_get_chsb(bandwidth, ch_ctl, sec_chan_offset, ch_cen1, ch_cen2,
	               &chspec_ch, &chspec_sb);

	/* Assemble chanspec for ioctl */
	cs = (chspec_ch | chspec_band | chspec_bw | chspec_sb);

	if (wifi_is_dfs_channel(ifname, ch_ctl, bandwidth)) {
		/* First try Zero Wait DFS */
		status = bcmwl_set_dfs_ap_move(ifname, cs);

		if (!status)
			return 0;

		WARN_ON(status);
	}

	/* If nonDFS or ZWDFS failed call CSA */
	status = bcmwl_set_csa(ifname, 0, count, cs);

	WARN_ON(status);

	return status;
}

static int bcmwl_get_macmode(const char *name, int *macmode)
{
	int ret;
	uint32_t val = 0;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_MACMODE,
		.buf = &val,
		.buflen = sizeof(val)
	};

	ret = wl_ioctl(&arg);

	if (!ret)
		*macmode = wl_swap_32(name, val);

	return ret;
}

static int bcmwl_set_macmode(const char *name, int macmode)
{
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_SET_MACMODE,
		.param = &macmode,
		.paramlen = sizeof(int),
		.set = 1
	};

	ret = wl_ioctl(&arg);

	return ret;
}

static int bcmwl_get_probresp_mac_filter(const char *name, bool *prmf)
{
	int ret;
	bool buf;

	ret = wl_iovar_get_noparam(name, "probresp_mac_filter",
	                           &buf, sizeof(buf));

	if (!ret)
		*prmf = buf;

	return ret;
}

static int bcmwl_set_probresp_mac_filter(const char *name, bool prmf)
{
	int ret;
	int presp_mac_filter_on = prmf ? 1 : 0;
	char buf[64] = {0};

	ret = wl_iovar_set(name, "probresp_mac_filter", &presp_mac_filter_on,
	                   sizeof(int), buf, sizeof(buf));

	return ret;
}

static int bcmwl_iface_is_maclist_empty(const char *name, bool *is_empty)
{
	int ret;
	char buf[WLC_IOCTL_MAXLEN] = {0};
	struct maclist *maclist;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_MACLIST,
		.buf = &buf,
		.buflen = sizeof(buf)
	};
	int swap;
	int max;

	max = (WLC_IOCTL_MAXLEN - sizeof(int)) / ETHER_ADDR_LEN;
	maclist = (struct maclist *) buf;
	swap = wl_swap(name);

	maclist->count = swap ? BCMSWAP32(max) : max;
	ret = wl_ioctl(&arg);

	if (!ret) {
		maclist->count = swap ? BCMSWAP32(maclist->count) : maclist->count;
		*is_empty = maclist->count ? false : true;
	}

	return ret;
}

static int bcmwl_iface_update_maclist(const char *name, uint8_t *sta, bool remove)
{
	char sbuf[WLC_IOCTL_MAXLEN] = {0};
	char dbuf[WLC_IOCTL_MAXLEN] = {0};
	struct maclist *src_maclist;
	struct maclist *dst_maclist;
	int swap;
	int max;
	bool is_present = false, changed = false;
	uint16_t i, count = 0;
	int ret;

	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_MACLIST,
		.buf = &sbuf,
		.buflen = sizeof(sbuf)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	swap = wl_swap(name);
	max = (WLC_IOCTL_MAXLEN - sizeof(int)) / ETHER_ADDR_LEN;
	src_maclist = (struct maclist *) sbuf;
	dst_maclist = (struct maclist *) dbuf;

	src_maclist->count = swap ? BCMSWAP32(max) : max;
	dst_maclist->count = swap ? BCMSWAP32(max) : max;

	/* Get MAC list */
	ret = wl_ioctl(&arg);

	if (WARN_ON(ret))
		return ret;

	src_maclist->count = swap ? BCMSWAP32(src_maclist->count) : src_maclist->count;

	for (i = 0; i < src_maclist->count; i++) {
		if (!memcmp(&src_maclist->ea[i].octet, sta, 6)) {
			is_present = true;
			if (remove) {
				/* Remove */
				changed = true;
				continue;
			} else {
				/* Add */
				libwifi_dbg("[%s] %s : " MACSTR " already on the list\n",
					        name, __func__, MAC2STR(sta));
				break;
			}
		}
		memcpy(dst_maclist->ea[count].octet, src_maclist->ea[i].octet, ETHER_ADDR_LEN);
		count++;
	}

	if (!remove && !is_present) {
		/* Adding new STA to the list */
		libwifi_dbg("[%s] %s : adding new STA to deny list\n", name, __func__);
		memcpy(dst_maclist->ea[count].octet, sta, ETHER_ADDR_LEN);
		changed = true;
		count++;
	}

	if (changed) {
		dst_maclist->count = swap ? BCMSWAP32(count) : count;

		struct wl_arg arg2 = {
			.ifname = name,
			.cmd = WLC_SET_MACLIST,
			.param = &dbuf,
			.paramlen = sizeof(struct maclist) + count * sizeof(struct ether_addr),
			.buf = NULL,
			.buflen = 0,
			.set = true
		};

		/* Set MAC list */
		ret = wl_ioctl(&arg2);
	} else {
		libwifi_dbg("[%s] %s : no change on MAC deny list\n", name, __func__);
	}

	return WARN_ON(ret);
}
#endif

static int wl_cli_get_chanspec(const char *name, chanspec_t *val)
{
	char buf[64] = {0};
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s chanspec", name);

	if (WARN_ON(status))
		return status;

	if (WARN_ON(sscanf(strstr(buf,"(0x"), "(0x%x", (uint32_t *) val) != 1))
		return -1;

	return 0;
}

static int bcmwl_radio_get_chanspec(const char *name, chanspec_t *val)
{
	int ret;
	int swap = wl_swap(name);
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_VAR,
		.iovar = "chanspec",
		.buf = val,
		.buflen = sizeof(*val)
	};

	ret = wl_ctl(&arg);
	if (ret == 0)
		goto swap;

	libwifi_dbg("[%s] %s fallback to wl ret=%d\n", name, __func__, ret);
	ret = wl_cli_get_chanspec(name, val);

	if (WARN_ON(ret))
		return ret;

	goto end;

swap:
	*val = swap ? BCMSWAP16(*val) : *val;
end:
	return ret;
}

int bcmwl_radio_get_channel(const char *name, uint32_t *channel, enum wifi_bw *bw)
{
	chanspec_t val;
	int ret;

	*bw = BW_UNKNOWN;

	ret = bcmwl_radio_get_chanspec(name, &val);
	if (WARN_ON(ret))
		return ret;

	if (CHSPEC_IS20(val)) *bw = BW20;
	if (CHSPEC_IS40(val)) *bw = BW40;
	if (CHSPEC_IS80(val)) *bw = BW80;
	if (CHSPEC_IS160(val)) *bw = BW160;
	if (CHSPEC_IS8080(val)) *bw = BW8080;
	*channel = chanspec_to_ctrlchannel(val);

	return 0;
}

int bcmwl_radio_get_sideband(const char *name, enum wifi_chan_ext *sideband)
{
	chanspec_t val;
	int ret;

	ret = bcmwl_radio_get_chanspec(name, &val);
	if (WARN_ON(ret))
		return ret;

	if(CHSPEC_SB_UPPER(val)) *sideband = EXTCH_BELOW;
	if(CHSPEC_SB_LOWER(val)) *sideband = EXTCH_ABOVE;

	return 0;
}

/* Best if we could include this directly from SDK */
#define ALIGN_SIZE(size, boundary) (((size) + (boundary) - 1) \
                                                 & ~((boundary) - 1))
typedef struct bcm_xtlv {
        uint16  id;
        uint16  len;
        uint8   data[1];
} bcm_xtlv_t;


uint8_t *bcmwl_get_tlv(int swap, uint8_t *buf, size_t len, uint16_t id)
{
	bcm_xtlv_t *xtlv;
	uint8_t *ptr;
	uint8_t *res;

	ptr = buf;
	res = NULL;

	while ((ptr - buf) < len) {
		xtlv = (bcm_xtlv_t *) ptr;
		if (swap) {
			xtlv->id = BCMSWAP16(xtlv->id);
			xtlv->len = BCMSWAP16(xtlv->len);
		}

		libwifi_dbg("tlv id 0x%x len %u\n", xtlv->id, xtlv->len);
		if (xtlv->id == id) {
			res = xtlv->data;
			break;
		}
		ptr += ALIGN_SIZE(sizeof(xtlv->id) + sizeof(xtlv->len) + xtlv->len, 4);
	}

	return res;
}

int bcmwl_radio_get_stats(const char *name, struct wifi_radio_stats *s)
{
	char buf[WLC_IOCTL_MAXLEN] = {};
	wl_cnt_info_t *c = (wl_cnt_info_t *) buf;
	wl_cnt_wlc_t *cnt;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_VAR,
		.iovar = "counters",
		.buf = buf,
		.buflen = sizeof(buf),
	};
	int swap;
	int ret;

	libwifi_dbg("[%s] %s called\n", name, __func__);
	memset(s, 0, sizeof(*s));

	swap = wl_swap(name);
	ret = wl_ctl(&arg);
	if (ret) {
		libwifi_err("[%s] %s error %d\n", name, __func__, ret);
		return -1;
	}

	/* TODO check headers */
	c = (wl_cnt_info_t *) buf;
	if (swap) {
		c->version = BCMSWAP16(c->version);
		c->datalen = BCMSWAP16(c->datalen);
	}

	if (c->version == 11 || c->version == 10)
		cnt = (wl_cnt_wlc_t *)c->data;
	else if (c->version == 30)
		cnt = (wl_cnt_wlc_t *) bcmwl_get_tlv(swap, c->data, c->datalen, WL_CNT_XTLV_WLC);  /* xtlv formatted */
	else {
		libwifi_err("'counters' version not supported!\n");
		return 0;
	}

	if (WARN_ON(!cnt))
		return -1;

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

int bcmwl_iface_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	char buf[1024] = {};
	wl_if_stats_t *cnt = (wl_if_stats_t *) buf;
	wl_if_stats_v2_t *cntv2;
	struct wl_arg arg = {
		.ifname = ifname,
		.cmd = WLC_GET_VAR,
		.iovar = "if_counters",
		.buf = buf,
		.buflen = sizeof(buf),
	};
	int swap;
	int ret;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	memset(s, 0, sizeof(*s));

	swap = wl_swap(ifname);
	ret = wl_ctl(&arg);
	if (WARN_ON(ret))
		return ret;

	if (swap) {
		s->tx_bytes = BCMSWAP64(cnt->txbyte);
		s->rx_bytes = BCMSWAP64(cnt->rxbyte);
		s->tx_pkts = BCMSWAP64(cnt->txframe);
		s->rx_pkts = BCMSWAP64(cnt->rxframe);

		s->tx_err_pkts = BCMSWAP64(cnt->txerror);
		s->tx_rtx_pkts = BCMSWAP64(cnt->txretry);
		s->tx_rtx_fail_pkts = BCMSWAP64(cnt->txfail);
		s->tx_retry_pkts = BCMSWAP64(cnt->txretry);
		s->tx_mretry_pkts = BCMSWAP64(cnt->txretrie);
		s->rx_err_pkts = BCMSWAP64(cnt->rxerror);
		s->tx_dropped_pkts = BCMSWAP64(cnt->txnobuf);
		s->rx_dropped_pkts = BCMSWAP64(cnt->rxnobuf);
		s->tx_mcast_pkts = BCMSWAP64(cnt->txmulti);
		s->rx_mcast_pkts = BCMSWAP64(cnt->rxmulti);

		if (BCMSWAP16(cnt->version) == 2) {
			cntv2 = (wl_if_stats_v2_t *) buf;
			s->tx_ucast_pkts = BCMSWAP64(cntv2->txucastpkts);
			s->rx_ucast_pkts = BCMSWAP64(cntv2->rxucastpkts);
			s->tx_bcast_pkts = BCMSWAP64(cntv2->txbcastpkts);
			s->rx_bcast_pkts = BCMSWAP64(cntv2->rxbcastpkts);
			s->ack_fail_pkts = BCMSWAP64(cntv2->txnoack);
			s->aggr_pkts = BCMSWAP64(cntv2->txaggrpktcnt);
			s->tx_rtx_fail_pkts = BCMSWAP64(cntv2->txretransfail);
			s->rx_unknown_pkts = BCMSWAP64(cntv2->rxbadprotopkts);
		}

	} else {
		s->tx_bytes = cnt->txbyte;
		s->rx_bytes = cnt->rxbyte;
		s->tx_pkts = cnt->txframe;
		s->rx_pkts = cnt->rxframe;

		s->tx_err_pkts = cnt->txerror;
		s->tx_rtx_pkts = cnt->txretry;
		s->tx_rtx_fail_pkts = cnt->txfail;
		s->tx_retry_pkts = cnt->txretry;
		s->tx_mretry_pkts = cnt->txretrie;
		s->rx_err_pkts = cnt->rxerror;
		s->tx_dropped_pkts = cnt->txnobuf;
		s->rx_dropped_pkts = cnt->rxnobuf;
		s->tx_mcast_pkts = cnt->txmulti;
		s->rx_mcast_pkts = cnt->rxmulti;

		if (cnt->version == 2) {
			cntv2 = (wl_if_stats_v2_t *) buf;
			s->tx_ucast_pkts = cntv2->txucastpkts;
			s->rx_ucast_pkts = cntv2->rxucastpkts;
			s->tx_bcast_pkts = cntv2->txbcastpkts;
			s->rx_bcast_pkts = cntv2->rxbcastpkts;
			s->ack_fail_pkts = cntv2->txnoack;
			s->aggr_pkts = cntv2->txaggrpktcnt;
			s->tx_rtx_fail_pkts = cntv2->txretransfail;
			s->rx_unknown_pkts = cntv2->rxbadprotopkts;
		}
	}

	return ret;
}

static int bcmwl_get_ap_wmm_stats(const char *ifname,
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

int bcmwl_get_supported_security_const(const char *name, uint32_t *sec)
{
	*sec = BIT(WIFI_SECURITY_NONE) |
	       BIT(WIFI_SECURITY_WEP64) |
	       BIT(WIFI_SECURITY_WEP128) |
	       BIT(WIFI_SECURITY_WPA) |
	       BIT(WIFI_SECURITY_WPA2) |
	       BIT(WIFI_SECURITY_WPA3) |
	       BIT(WIFI_SECURITY_WPAPSK) |
	       BIT(WIFI_SECURITY_WPA2PSK) |
	       BIT(WIFI_SECURITY_WPA3PSK);
	return 0;
}

int bcmwl_get_sta_supported_security(const char *name, uint32_t *sec)
{
	/* TODO get this from wpa_supplicant */
	*sec = BIT(WIFI_SECURITY_NONE) |
	       BIT(WIFI_SECURITY_WEP64) |
	       BIT(WIFI_SECURITY_WEP128) |
	       BIT(WIFI_SECURITY_WPA) |
	       BIT(WIFI_SECURITY_WPA2) |
	       BIT(WIFI_SECURITY_WPA3) |
	       BIT(WIFI_SECURITY_WPAPSK) |
	       BIT(WIFI_SECURITY_WPA2PSK) |
	       BIT(WIFI_SECURITY_WPA3PSK);
	return 0;
}

int bcmwl_iface_get_beacon_ies(const char *name, uint8_t *ies, int *len)
{
	uint8_t bcn[WLC_IOCTL_MAXLEN] = { 0 };
	uint8_t *bcn_ies;
	size_t bcn_len = sizeof(bcn);
	size_t ies_len;
	int ret = 0;
	struct beacon_frame *bf;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (WARN_ON(!ies) || WARN_ON(!len) || WARN_ON(*len == 0))
		return -EINVAL;

	memset(ies, 0, *len);

	ret = wl_ioctl_iface_get_beacon(name, bcn, &bcn_len);

	if (WARN_ON(ret))
		return ret;

	bf = (struct beacon_frame *)bcn;
	ies_len = bcn_len - ((uint8_t *)&bf->cap_info - bcn + 2);
	bcn_ies = bf->var;

	if (WARN_ON(ies_len == 0 || *len < ies_len)) {
		*len = 0;
		return -1;
	}

	memcpy(ies, bcn_ies, ies_len);
	*len = (int)ies_len;

	return 0;
}

static int _brcm_get_ap_wmm_params(const char *ifname, const uint8_t *p,
				   struct wifi_ap_wmm_ac a[])
{
	a[0].ac = (p[0] & 0x60) >> 5;
	a[0].aifsn = p[0] & 0x0f;
	a[0].cwmin = p[1] & 0x0f;
	a[0].cwmax = (p[1] & 0xf0) >> 4;
	a[0].txop = (p[3] << 8) | p[2];		/* LSB first */

	a[1].ac = (p[4] & 0x60) >> 5;
	a[1].aifsn = p[4] & 0x0f;
	a[1].cwmin = p[5] & 0x0f;
	a[1].cwmax = (p[5] & 0xf0) >> 4;
	a[1].txop = (p[7] << 8) | p[6];

	a[2].ac = (p[8] & 0x60) >> 5;
	a[2].aifsn = p[8] & 0x0f;
	a[2].cwmin = p[9] & 0x0f;
	a[2].cwmax = (p[9] & 0xf0) >> 4;
	a[2].txop = (p[11] << 8) | p[10];

	a[3].ac = (p[12] & 0x60) >> 5;
	a[3].aifsn = p[12] & 0x0f;
	a[3].cwmin = p[13] & 0x0f;
	a[3].cwmax = (p[13] & 0xf0) >> 4;
	a[3].txop = (p[15] << 8) | p[14];

	return 0;
}

int bcmwl_iface_ap_info(const char *name, struct wifi_ap *ap)
{
	int ret = 0;
	uint8_t beacon[WLC_IOCTL_MAXLEN] = { 0 };
	uint8_t *beacon_ies;
	uint8_t *ie_ptr;
	size_t beacon_len = sizeof(beacon);
	size_t ies_len;
	struct beacon_frame *bf;

	WARN_ON(wl_ioctl_iface_get_assoclist_max(name, &ap->assoclist_max));

	/* get beacon */
	ret = wl_ioctl_iface_get_beacon(name, beacon, &beacon_len);

	if (WARN_ON(ret)) {
		ap->enabled = false;
		return ret;
	}

	ap->enabled = true;
	bf = (struct beacon_frame *)beacon;
	ies_len = beacon_len - ((uint8_t *)&bf->cap_info - beacon + 2);
	beacon_ies = bf->var;

	ret = wifi_oper_stds_set_from_ie(beacon_ies, ies_len, &ap->bss.oper_std);
	if (ret)
		WARN_ON(bcmwl_get_oper_stds(name, &ap->bss.oper_std));
	WARN_ON(wifi_ssid_advertised_set_from_ie(beacon_ies, ies_len, &ap->ssid_advertised));
	WARN_ON(wifi_get_bss_security_from_ies(&ap->bss, beacon_ies, ies_len));
	WARN_ON(wifi_apload_set_from_ie(beacon_ies, ies_len, &ap->bss.load));
	ap->bss.beacon_int = bf->beacon_int;
	ap->sec.curr_mode = ap->bss.security;

	/* capabilities information */
	ie_ptr = (uint8_t *)&bf->cap_info;
	ap->bss.caps.valid |= WIFI_CAP_BASIC_VALID;
	memcpy(&ap->bss.caps.basic, ie_ptr, sizeof(bf->cap_info));
	wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, sizeof(bf->cap_info));

	/* wmm params */
	ie_ptr = wifi_find_vsie(beacon_ies, ies_len, (uint8_t *)"\x00\x50\xf2", 2, 0xff);
	if (ie_ptr) {
		wifi_cap_set(ap->bss.cbitmap, WIFI_CAP_WMM);
		ie_ptr += 2 + 3 + 2 + 1;  /* skip upto 'version' */
		if (!!(ie_ptr[0] & 0x80))
			wifi_cap_set(ap->bss.cbitmap, WIFI_CAP_APSD);

		ie_ptr += 2; /* points to start of AC parameter records */
		_brcm_get_ap_wmm_params(name, ie_ptr, ap->ac);
	}

	/* ht capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_HT_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_HT_VALID;
		memcpy(&ap->bss.caps.ht, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* vht capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_VHT_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_VHT_VALID;
		memcpy(&ap->bss.caps.vht, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* extended capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_EXT_CAP);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_EXT_VALID;
		memcpy(&ap->bss.caps.ext, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* RM enabled capabilities */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_RRM);
	if (ie_ptr) {
		ap->bss.caps.valid |= WIFI_CAP_RM_VALID;
		memcpy(&ap->bss.caps.ext, &ie_ptr[2], ie_ptr[1]);
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* mobility domain element */
	ie_ptr = wifi_find_ie(beacon_ies, ies_len, IE_MDE);
	if (ie_ptr) {
		wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
	}

	/* HE capabilities */
	ie_ptr = wifi_find_ie_ext(beacon_ies, ies_len, IE_EXT_HE_CAP);
	if (ie_ptr) {
		size_t len = ie_ptr[1] & 0xff;

		if (len >= 17) {
			ap->bss.caps.valid |= WIFI_CAP_HE_VALID;
			memcpy(&ap->bss.caps.he.byte_mac, &ie_ptr[3], 6);
			memcpy(&ap->bss.caps.he.byte_phy, &ie_ptr[9], 11);

			memcpy(&ap->bss.caps.he.byte_opt, &ie_ptr[20], len - 17);
			wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
		}
	}

	/* EHT capabilities */
	ie_ptr = wifi_find_ie_ext(beacon_ies, ies_len, IE_EXT_EHT_CAP);
	if (ie_ptr) {
		size_t len = ie_ptr[1] & 0xff;

		if (len >= 11) {
			ap->bss.caps.valid |= WIFI_CAP_EHT_VALID;
			memcpy(&ap->bss.caps.eht.byte_mac, &ie_ptr[3], 2);
			memcpy(&ap->bss.caps.eht.byte_phy, &ie_ptr[5], 9);

			memcpy(&ap->bss.caps.eht.supp_mcs, &ie_ptr[14], len - 11);
			wifi_cap_set_from_ie(ap->bss.cbitmap, ie_ptr, ie_ptr[1] + 2);
		}
	}

	/* WMM stats */
	WARN_ON(bcmwl_get_ap_wmm_stats(name, ap->ac));

	return ret;
}

int bcmwl_ap_get_oper_stds(const char *name, uint8_t *std)
{
	uint8_t beacon_ies[WLC_IOCTL_MAXLEN] = { 0 };
	size_t ies_len = sizeof(beacon_ies);
	int ret;

	if (WARN_ON(!std))
		return -EINVAL;

	ret = bcmwl_iface_get_beacon_ies(name, beacon_ies, (int*)&ies_len);

	if (WARN_ON(ret)) {
		return ret;
	}

	ret = wifi_oper_stds_set_from_ie(beacon_ies, ies_len, std);

	if (ret == 0) {
		return 0;
	}

	ret = bcmwl_get_oper_stds(name, std);

	if (WARN_ON(ret)) {
		return ret;
	}

	return 0;
}

int bcmwl_get_oper_band(const char *name, enum wifi_band *band)
{
	uint32_t val = 0, b;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_BAND,
		.buf = &val,
		.buflen = sizeof(val)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = wl_ctl(&arg);
	if (ret) {
		*band = BAND_UNKNOWN;
		return ret;
	}

	b = wl_swap_32(name, val);

	switch (b) {
	case WLC_BAND_5G:
		*band = BAND_5;
		break;
	case WLC_BAND_2G:
		*band = BAND_2;
		break;
	case WLC_BAND_AUTO:
	case WLC_BAND_ALL:
		*band = BAND_DUAL;
		break;
	case WLC_BAND_6G:
		*band = BAND_6;
		break;
	default:
		*band = BAND_UNKNOWN;
		break;
	}

	return ret;
}

int bcmwl_get_oper_stds(const char *name, uint8_t *std)
{
	enum wifi_band band;
	uint32_t nmode = 0;
	uint32_t vhtmode = 0;
	uint32_t he = 0;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!std)
		return -EINVAL;

	*std = 0;

	WARN_ON(wl_iovar_get(name, "nmode", NULL, 0, &nmode, sizeof(nmode)));
	WARN_ON(wl_iovar_get(name, "vhtmode", NULL, 0, &vhtmode, sizeof(vhtmode)));
	WARN_ON(wl_iovar_get(name, "he", NULL, 0, &he, sizeof(he)));

	if (he) {
		*std |= WIFI_AX;
	}

	if (vhtmode) {
		*std |= WIFI_AC;
	}

	if (nmode) {
		*std |= WIFI_N;
	}

	if (WARN_ON(bcmwl_get_oper_band(name, &band)))
		return 0;

	if (band == BAND_5) {
		*std |= WIFI_A;
	}

	if (band == BAND_2) {
		int n = 32;
		uint32_t rates[32] = {0};

		if (!bcmwl_radio_get_oper_rates(name, &n, rates)) {
			int i;

			for (i = 0; i < n; i++) {
				if (rates[i] == 5 || rates[i] == 11)
					*std |= WIFI_B;
				else if (rates[i] == 6 || rates[i] == 9 || rates[i] > 11)
					*std |= WIFI_G;
			}
		}
	}

	return 0;
}

int bcmwl_get_supp_stds(const char *name, uint8_t *std)
{
	uint32_t band = 0;
	uint32_t var = 0;

	*std = 0;

	if (!wl_iovar_get(name, "he", NULL, 0, &var, sizeof(var)))
		*std |= WIFI_AX;
	if (!wl_iovar_get(name, "nmode", NULL, 0, &var, sizeof(var)))
		*std |= WIFI_N;

	WARN_ON(nlwifi_get_supp_band(name, &band));

	if (band & BAND_5 || band & BAND_6) {
		if (!wl_iovar_get(name, "vhtmode", NULL, 0, &var, sizeof(var)))
			*std |= WIFI_AC;
		*std |= WIFI_A;
	}
	if (band & BAND_2)
		*std |= WIFI_G | WIFI_B;

	return 0;
}

static int bcmwl_iface_get_recent_rate(const char *ifname, uint8_t *macaddr, struct wifi_rate *rx_rate, struct wifi_rate *tx_rate)
{
	const uint8 legacy_mbps[] = {1, 2, 5, 6, 9, 11, 12, 18, 24, 36, 48, 54};
	const uint8 legacy_mbps_len = sizeof(legacy_mbps) / sizeof(legacy_mbps[0]);
	char buf[WLC_IOCTL_MAXLEN] = {0};
	wl_rate_histo_report_t *rpt;
	wl_rate_histo_maps2_t *maps2;
	int rpt_len;
	uint16 seconds;
	uint8 recent_index;
	uint8 recent_type;
	uint8 idx;
	int status;

	rpt_len = sizeof(wl_rate_histo_report_t) + sizeof(wl_rate_histo_maps1_t);
	if (WARN_ON(rpt_len >= sizeof(buf)))
		return -1;

	rpt = (wl_rate_histo_report_t *) buf;
	memset(rpt, 0, sizeof(*rpt));

	/* TODO VER_1 */
	rpt->ver = WL_HISTO_VER_2;
	rpt->type = WL_HISTO_TYPE_RATE_MAP2;
	rpt->length = rpt_len;
	rpt->fixed_len = WL_HISTO_VER_1_FIXED_LEN;
	memcpy(&rpt->ea, macaddr, 6);

	status = wl_iovar_get(ifname, "rate_histo_report", rpt, sizeof(*rpt), rpt, WLC_IOCTL_MAXLEN);
	WARN_ON(status);

	if (WARN_ON(rpt->ver != WL_HISTO_VER_2))
		return -1;

	if (WARN_ON(rpt->type != WL_HISTO_TYPE_RATE_MAP2))
		return -1;

	if (WARN_ON(rpt->length < sizeof(*rpt) + sizeof(*maps2)))
		return -1;

	maps2 = (wl_rate_histo_maps2_t *)WL_HISTO_DATA(rpt);
	if (WARN_ON(!maps2))
		return -1;

	/* TODO check how to get SGI */

	/* Get most used rx rate and fill sta_info */
	seconds = maps2->rx.seconds;
	recent_index = maps2->rx.recent_index;
	recent_type = maps2->rx.recent_type;

	libwifi_dbg("seconds: %d, type: %d, bw: %d, rx recent_index: %u\n", seconds, recent_type, WL_HISTO_INDEX2BW(recent_index), recent_index);

	switch (recent_type) {
	case WL_HISTO_RATE_NUM_TYPES:
	case WL_HISTO_RATE_TYPE_VHT:
	case WL_HISTO_RATE_TYPE_HT:
		idx = (recent_index - WL_NUM_LEGACY_RATES) % WL_NUM_VHT_RATES;
		libwifi_dbg("rx recent ht/vht/he %dx%d\n", WL_HISTO_RATE_DECODE_VHT_MCS(idx), WL_HISTO_RATE_DECODE_VHT_NSS(idx));
		rx_rate->m.mcs = WL_HISTO_RATE_DECODE_VHT_MCS(idx);
		rx_rate->m.nss = WL_HISTO_RATE_DECODE_VHT_NSS(idx);
		rx_rate->m.bw = WL_HISTO_INDEX2BW(recent_index);
		rx_rate->phy = PHY_VHT;
		rx_rate->rate = wifi_mcs2rate(rx_rate->m.mcs, rx_rate->m.bw, rx_rate->m.nss, WIFI_LGI);
		break;
	case WL_HISTO_RATE_TYPE_LEGACY:
		libwifi_dbg("rx recent legacy\n");
		if (WARN_ON(recent_index >= legacy_mbps_len))
			break;
		rx_rate->rate = legacy_mbps[recent_index];
		rx_rate->phy = PHY_OFDM;
		break;
	default:
		break;
	}

	if (recent_type == WL_HISTO_RATE_TYPE_HT)
		rx_rate->phy = PHY_HT;

	/* Get most used tx rate and fill sta_info */
	seconds = maps2->tx.seconds;
	recent_index = maps2->tx.recent_index;
	recent_type = maps2->tx.recent_type;

	libwifi_dbg("seconds: %d, type %d, bw: %d, tx recent_index: %u\n", seconds, recent_type, WL_HISTO_INDEX2BW(recent_index), recent_index);

	switch (recent_type) {
	case WL_HISTO_RATE_NUM_TYPES:
	case WL_HISTO_RATE_TYPE_HT:
	case WL_HISTO_RATE_TYPE_VHT:
		idx = (recent_index - WL_NUM_LEGACY_RATES) % WL_NUM_VHT_RATES;
		libwifi_dbg("tx recent ht/vht/he %dx%d\n", WL_HISTO_RATE_DECODE_VHT_MCS(idx), WL_HISTO_RATE_DECODE_VHT_NSS(idx));
		tx_rate->m.mcs = WL_HISTO_RATE_DECODE_VHT_MCS(idx);
		tx_rate->m.nss = WL_HISTO_RATE_DECODE_VHT_NSS(idx);
		tx_rate->m.bw = WL_HISTO_INDEX2BW(recent_index);
		tx_rate->phy = PHY_VHT;
		tx_rate->rate = wifi_mcs2rate(tx_rate->m.mcs, tx_rate->m.bw, tx_rate->m.nss, WIFI_LGI);
		break;
	case WL_HISTO_RATE_TYPE_LEGACY:
		libwifi_dbg("tx recent legacy\n");
		if (WARN_ON(recent_index >= legacy_mbps_len))
			break;
		tx_rate->rate = legacy_mbps[recent_index];
		tx_rate->phy = PHY_OFDM;
		break;
	default:
		break;
	}

	if (recent_type == WL_HISTO_RATE_TYPE_HT)
		tx_rate->phy = PHY_HT;

	return status;
}

int bcmwl_iface_get_exp_tp(const char *ifname, uint8_t *macaddr, struct wifi_sta *sta)
{
	struct wifi_rate recent_tx_rate = {};
	struct wifi_rate recent_rx_rate = {};

	if (bcmwl_iface_get_recent_rate(ifname, macaddr, &recent_rx_rate, &recent_tx_rate))
		return -1;

	/*
	 * Simplest implementation base on recent rate we get
	 * from histograms.
	 *
	 * TODO: check all rates and calc smth like:
	 *  - rate_usage_percent = rate_pkt_count * 100 / all_pck_count
	 *  - rate_exp_tp = rate_phy_tp * rate_usage_percent
	 *  - exp_tp = SUM(rate_exp_tps)
	 *
	 * TODO: check chanim_stats - channel idle time - our left airtime
	 *
	 * TODO: check if idle time cover beacons and probes also
	 *       If not, check number of APs and beacons airtime
	 *
	 * Today assume:
	 *  - we can fill maximum 85% of airtime - best case
	 *  - Reserve 20% for TCP ACK.
	 */
	sta->est_rx_thput = (recent_rx_rate.rate * 85 * 80) / 10000;
	sta->est_tx_thput = (recent_tx_rate.rate * 85 * 80) / 10000;

	libwifi_dbg("[%s] " MACSTR " exp_max_tx %d Mbps exp_max_rx %d Mbps\n",
		    ifname, MAC2STR(macaddr), sta->est_tx_thput, sta->est_rx_thput);

	return 0;
}

int bcmwl_iface_sta_info(const char *ifname, struct wifi_sta *sta)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	wl_ioctl_iface_get_assoc_info(ifname, sta);
	WARN_ON(bcmwl_get_rssi(ifname, &sta->rssi[0]));
	WARN_ON(bcmwl_get_macaddr(ifname, sta->macaddr));
	WARN_ON(bcmwl_get_bssid(ifname, sta->bssid));
	WARN_ON(bcmwl_get_oper_stds(ifname, &sta->oper_std));

	if (WARN_ON(supplicant_cli_get_sta_supported_security(ifname, &sta->sec.supp_modes)))
		bcmwl_get_sta_supported_security(ifname, &sta->sec.supp_modes);

	WARN_ON(supplicant_cli_get_sta_security(ifname, &sta->sec));
	bcmwl_get_sta_info(ifname, sta->bssid, sta);
	bcmwl_iface_get_exp_tp(ifname, sta->bssid, sta);

	return 0;
}

int bcmwl_radio_acs(const char *ifname, struct acs_param *p)
{
	char buf[128] = {0};
	int res;

	/* TODO should we start acsd2 if not started or relly on UCI ACS? */
	UNUSED(p);
	res = Cmd(buf, 128, "acs_cli2 -i %s autochannel", ifname);
	WARN_ON(res);

	return res;
}

/* TODO: return all 20MHz channels with cac ongoing */
int bcmwl_is_cac_ongoing(const char *ifname, uint32_t *channel)
{
	int ret;
	struct chan_entry chlist[32] = {0};
	int num = 32;
	int i;

	ret = bcmwl_radio_channels_info(ifname, chlist, &num);
	if (ret)
		return -1;

	for (i = 0; i < num; i++) {
		if (chlist[i].dfs_state == WIFI_DFS_STATE_CAC) {
			*channel = chlist[i].channel;
			return 1;
		}
	}

	return 0;
}

// TODO: move to header
extern bool wifi_is_dfs_usable(const char *name, int chan, enum wifi_bw bw);

int bcmwl_start_cac(const char *ifname, int channel, enum wifi_bw bw,
		    enum wifi_cac_method method)
{
	char buf[512] = {0};
	int res;


	if (!channel) {
		libwifi_dbg("[%s] %s invalid channel\n", ifname, __func__);
		return -1;
	}

	if (!wifi_is_dfs_usable(ifname, channel, bw))
		return -1;

	switch (method) {
	case WIFI_CAC_CONTINUOUS:
		Cmd(buf, sizeof(buf), "wl -i %s dfs_ap_move -1", ifname);
		res = Cmd(buf, sizeof(buf),
			  "wl -i %s csa 0 5 %d/%d", ifname, channel, wifi_bw_enum2MHz(bw));
		if (WARN_ON(res))
			return -1;
		break;
	case WIFI_CAC_MIMO_REDUCED:
		res = Cmd(buf, sizeof(buf),
			  "wl -i %s dfs_ap_move %d/%d", ifname, channel, wifi_bw_enum2MHz(bw));
		if (WARN_ON(res))
			return -1;

		/* TODO add post CAC action here, stunt/switch */
		res = Cmd(buf, sizeof(buf),
			  "wl -i %s dfs_ap_move -2", ifname);

		if (WARN_ON(res))
			return -1;
		break;
	default:
		libwifi_dbg("[%s] %s unsupported cac method %d\n", ifname, __func__, method);
		return -1;
	}

	return 0;
}

int bcmwl_stop_cac(const char *ifname)
{
	char buf[512] = {0};
	int res;

	res = Cmd(buf, sizeof(buf),
		  "wl -i %s dfs_ap_move -1", ifname);
	if (WARN_ON(res))
		return -1;

	return 0;
}

int bcmwl_simulate_radar(const char *ifname, struct wifi_radar_args *radar)
{
	char buf[512] = {0};
	int res;

	/* TODO: add handling for radar args */
	if (!radar->channel && !radar->bandwidth && !radar->type && !radar->subband_mask) {
		libwifi_dbg("[%s] %s none args specified, just trigger radar for oper channel\n", ifname, __func__);
	}
	res = Cmd(buf, sizeof(buf),
	          "wl -i %s radar 2", ifname);
	if (WARN_ON(res))
		return -1;

	return 0;
}

int bcmwl_radio_channels_info(const char *ifname, struct chan_entry *channel, int *num)
{
	char buf[4096] = {0};
	char *line, *ptr, *nop;
	char cc[3] = {0};
	int nop_minutes;
	int max;
	int chan;
	int res;
	int i;

	max = *num;
	*num = 0;

	memset(channel, 0x0, sizeof(*channel) * max);

	res = Cmd(buf, sizeof(buf), "wl -i %s chan_info", ifname);
	if (WARN_ON(res))
		return -1;

	ptr = buf;
	i = 0;

	bcmwl_radio_get_country(ifname, cc);

	while ((line = strsep(&ptr, "\r\n"))) {
		if (sscanf(line, "Channel %d\n", &chan) != 1)
			continue;

		if (WARN_ON(i >= max))
			return -1;

		if (strstr(line, "2G Band"))
			channel[i].band = BAND_2;
		else if (strstr(line, "5G Band"))
			channel[i].band = BAND_5;
		else if (strstr(line, "6G Band"))
			channel[i].band = BAND_6;
		else
			channel[i].band = BAND_UNKNOWN;

		channel[i].channel = chan;
		channel[i].freq = wifi_channel_to_freq_ex(chan, channel[i].band);

		do {
			if (!strstr(line, "RADAR Sensitive")) {
				channel[i].dfs = false;
				break;
			}

			channel[i].dfs = true;
			channel[i].dfs_state = WIFI_DFS_STATE_USABLE;
			channel[i].cac_time = cac_time(cc, chan);

			/* Check NOP */
			if ((nop = strstr(line, "Temporarily Out of Service for")) != NULL) {
				if (sscanf(nop, "Temporarily Out of Service for %d minutes", &nop_minutes) == 1)
					channel[i].nop_time = nop_minutes * 60;
				channel[i].dfs_state = WIFI_DFS_STATE_UNAVAILABLE;
				break;
			}

			/* Check ISM */
			if (!strstr(line, "Passive")) {
				/* TODO check if real ISM using dfs_ap_move */
				channel[i].dfs_state = WIFI_DFS_STATE_AVAILABLE;
				break;
			}
		} while (0);

		i++;
	}

	*num = i;

	/* Finally Update PRE-ISM */
	bcmwl_radio_channels_update_preism(ifname, channel, *num);
	bcmwl_radio_channels_update_survey(ifname, channel, *num);

	return 0;
}

int bcmwl_radio_get_beacon_int(const char *name, uint32_t *beacon_int)
{
	int bint = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_BCNPRD,
		.buf = &bint,
		.buflen = sizeof(bint)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);
	*beacon_int = 0;

	ret = wl_ctl(&arg);
	if (!ret) {
		*beacon_int = (uint32_t)wl_swap_32(name, bint);
	}

	return ret;
}

int bcmwl_radio_get_dtim(const char *name, uint32_t *dtim_period)
{
	int dtim = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_DTIMPRD,
		.buf = &dtim,
		.buflen = sizeof(dtim)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);
	*dtim_period = 0;

	ret = wl_ctl(&arg);
	if (!ret) {
		*dtim_period = (uint32_t)wl_swap_32(name, dtim);
	}

	return ret;
}

int bcmwl_radio_get_txpower(const char *name, int8_t *txpower_dbm, int8_t *txpower_percent)
{
	int val;
	int8_t tmp;
	const float dbm2mw[] = {
		0.001, 0.001, 0.001, 0.001, 0.002, 0.002, 0.003, 0.003,
		0.004, 0.005, 0.006, 0.008, 0.010, 0.013, 0.016, 0.020,
		0.025, 0.032, 0.040, 0.050, 0.063, 0.079, 0.100, 0.126,
		0.158, 0.200, 0.251, 0.316, 0.398, 0.501, 0.631, 0.794,
		1.000, 1.259, 1.585, 1.995, 2.512, 3.162, 3.981, 5.012,
		6.310, 7.943, 10.000, 12.589, 15.849, 19.953, 25.119, 31.623,
		39.811, 50.119, 63.096, 79.433, 100.000, 125.893, 158.489, 199.526,
		251.189, 316.228, 398.107, 501.187, 630.957, 794.328, 1000.000, 1258.925 };

	float max = 1496.236;	/* = 31.75 dBm (WLC_TXPWR_MAX) */
	float mw;


	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!txpower_dbm || !txpower_percent)
		return -EINVAL;

	*txpower_dbm = 0;
	*txpower_percent = 100;

	WARN_ON(wl_iovar_get(name, "qtxpower", NULL, 0, &val, sizeof(val)));
	val &= ~WL_TXPWR_OVERRIDE;
	tmp = (int8_t)(val & 0xff);
	if ((float)tmp / 4.0 > 31.5)
		mw = max;
	else
		mw = dbm2mw[32 + tmp / 4];

	*txpower_dbm = tmp/4;
	*txpower_percent = (mw / max) * 100.0;

	return 0;
}

int bcmwl_radio_get_supp_bw(const char *name, uint32_t *supp_bw)
{
	char caps[WLC_IOCTL_MEDLEN] = {0};
	char *tokptr = NULL, *saveptr = NULL;
	uint8_t std = 0;
	int ret;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = bcmwl_get_supp_stds(name, &std);
	if (ret)
		return ret;

	*supp_bw = BIT(BW20);

	if (std & WIFI_N)
		*supp_bw |= BIT(BW40);

	if (std & WIFI_AC)
		*supp_bw |= BIT(BW80);

	ret = wl_iovar_get_noparam(name, "cap", (void *)caps, sizeof(caps));
	if (WARN_ON(ret))
		return ret;

	tokptr = strtok_r(caps, " ", &saveptr);
	while (tokptr != NULL) {
		if (!strcmp(tokptr, "80+80")) {
			*supp_bw |= BIT(BW8080);
		} else if (!strcmp(tokptr, "160")) {
			*supp_bw |= BIT(BW160);
		}
		tokptr = strtok_r(NULL, " ", &saveptr);
	}

	return 0;
}

static int bcmwl_radio_get_rateset_v2(const char *name, wl_rateset_args_v2_t *rs)
{

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!rs)
		return -EINVAL;

	WARN_ON(wl_iovar_get(name, "cur_rateset", NULL , 0, rs, sizeof(wl_rateset_args_v2_t)));

	if (rs->version == 2)
		return 0;

	return -1;
}

static int bcmwl_radio_get_mcs_index_per_nss(const uint16_t *mcsbuf, uint32_t len, uint32_t *mcs_idx)
{
	uint8_t mcs_tmp;
	uint8_t mcs_max = 0;
	int i, j;

	if (!mcs_idx)
		return -EINVAL;

	*mcs_idx = 0;

	for (i = 0; i < len; i++) {
		mcs_tmp = 0;
		for (j = 0; j <= 11; j++) {
			if (mcsbuf[i] & BIT(j))
				mcs_tmp = j;
		}
		if (mcs_tmp > mcs_max)
			mcs_max = mcs_tmp;
	}
	*mcs_idx = mcs_max;

	return 0;
}

static int bcmwl_radio_get_mcs_index(const uint8_t *mcsbuf, uint32_t len, uint32_t *mcs_idx)
{
	uint8_t mcs_tmp = 0;
	int i, j;

	if (!mcs_idx)
		return -EINVAL;

	*mcs_idx = 0;

	for (i = 0; i < len; i++) {
		/*
		 * HT allow up to 4 NSS and also optional mcs like 32,33 ...
		 * Skip this optional MCS, while we don't need them for
		 * maxrate calculation.
		 */
		if (i > 3)
			break;

		for (j = 0; j < 8; j++) {
			if (mcsbuf[i] & BIT(j))
				mcs_tmp++;
		}
	}
	if (mcs_tmp > 0)
		*mcs_idx = mcs_tmp - 1;

	return 0;
}

int bcmwl_radio_get_gi(const char *name, int he, enum wifi_guard *gi)
{
	int sgi_tx = 0;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!gi)
		return -EINVAL;

	WARN_ON(wl_iovar_get(name, "sgi_tx", NULL, 0, &sgi_tx, sizeof(sgi_tx)));
	sgi_tx = wl_swap_32(name, sgi_tx);

	switch(sgi_tx) {
		case -1:
			*gi = WIFI_AUTOGI;
			break;
		case 0:
			*gi = (he == 1) ? WIFI_2xLTF_GI1600 : WIFI_LGI;
			break;
		case 1:
			*gi = (he == 1) ? WIFI_2xLTF_GI800 : WIFI_SGI;
			break;
		case 2:
			*gi = (he == 1) ? WIFI_1xLTF_GI800 : WIFI_SGI;
			break;
		case 3:
			*gi = (he == 1) ? WIFI_2xLTF_GI800 : WIFI_SGI;
			break;
		case 4:
			*gi = (he == 1) ? WIFI_2xLTF_GI1600 : WIFI_SGI;
			break;
		case 5:
			*gi = (he == 1) ? WIFI_4xLTF_GI3200 : WIFI_SGI;
			break;
		default:
			*gi = WIFI_GI_UNKNOWN;
			break;
	}

	return 0;
}

static int bcmwl_radio_get_chains(const char *name, char *type, uint8_t *chains)
{
	uint32_t chainmask;
	int i;

	libwifi_dbg("[%s] %s called type %s\n", name, __func__, type);

	if (!chains)
		return -EINVAL;

	*chains = 0;
	chainmask = 0;
	WARN_ON(wl_iovar_get(name, type, NULL , 0, &chainmask, sizeof(chainmask)));

	chainmask = wl_swap_32(name, chainmask);

	/* This is bit mask of used/available chains */
	for (i = 0; i < sizeof(chainmask) * 8; i++) {
		if (chainmask & BIT(i))
			(*chains)++;
	}

	if (*chains == 0) {
		libwifi_warn("[%s] %s: Unhandled value of chains %d, type %s\n", name, __func__, chains, type);
		return -1;
	}

	return 0;
}

int bcmwl_radio_get_hw_txchain(const char *name, uint8_t *txchains)
{
	return bcmwl_radio_get_chains(name, "hw_txchain", txchains);
}

int bcmwl_radio_get_hw_rxchain(const char *name, uint8_t *rxchains)
{
	return bcmwl_radio_get_chains(name, "hw_rxchain", rxchains);
}

int bcmwl_radio_get_maxrates(const char *name, uint32_t *he_mcs, uint32_t *vht_mcs,
			     uint32_t *ht_mcs, uint32_t *legacy_rate)
{
	int i;
	uint32_t lrate;
	wl_rateset_args_v2_t rs = {};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!he_mcs || !vht_mcs || !ht_mcs || !legacy_rate)
		return -EINVAL;

	*legacy_rate = 0;

	WARN_ON(bcmwl_radio_get_rateset_v2(name, &rs));

	bcmwl_radio_get_mcs_index_per_nss(rs.he_mcs, WL_HE_CAP_MCS_MAP_NSS_MAX, he_mcs);
	bcmwl_radio_get_mcs_index_per_nss(rs.vht_mcs, WL_VHT_CAP_MCS_MAP_NSS_MAX, vht_mcs);
	bcmwl_radio_get_mcs_index(rs.mcs, MCSSET_LEN, ht_mcs);

	for (i = 0; i < rs.count; i++) {
		lrate = (rs.rates[i] & 0x7f) / 2;
		if (lrate > *legacy_rate)
			*legacy_rate = lrate;
	}

	return 0;
}

int bcmwl_radio_get_basic_rates(const char *name, int *num, uint32_t *rates)
{
	wl_rateset_args_v2_t rs = {};
	int i;


	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!rates || !num)
		return -EINVAL;

	*num = 0;

	WARN_ON(bcmwl_radio_get_rateset_v2(name, &rs));

	for (i = 0; i < rs.count; i++) {
		if (rs.rates[i] && !!(rs.rates[i] & 0x80)) {
			rates[*num] = (rs.rates[i] & 0x7f) >> 1;
			*num += 1;
		}
	}

	return 0;
}

int bcmwl_radio_get_oper_rates(const char *name, int *num, uint32_t *rates)
{
	wl_rateset_args_v2_t rs = {};
	int i;


	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!rates || !num)
		return -EINVAL;

	*num = 0;

	WARN_ON(bcmwl_radio_get_rateset_v2(name, &rs));

	for (i = 0; i < rs.count; i++) {
		if (rs.rates[i]) {
			rates[*num] = (rs.rates[i] & 0x7f) >> 1;
			*num += 1;
		}
	}

	return 0;
}

int bcmwl_radio_get_diag(const char *name, struct wifi_radio_diagnostic *diag)
{
	wl_chanim_stats_t param;
	wl_chanim_stats_t *chanim;
	chanim_stats_t *stats;
	char buf[WL_CHANIM_BUF_LEN] = { 0 };
	int swap;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (!diag)
		return -EINVAL;

	swap = wl_swap(name);

	memset(diag, 0, sizeof(struct wifi_radio_diagnostic));

	/* chanim stats */
	param.buflen = swap ? BCMSWAP32(WL_CHANIM_BUF_LEN) : WL_CHANIM_BUF_LEN;
	param.count = swap ? BCMSWAP32(WL_CHANIM_COUNT_ONE) : WL_CHANIM_COUNT_ONE;

	WARN_ON(wl_iovar_get(name, "chanim_stats", &param, sizeof(wl_chanim_stats_t), buf, WL_CHANIM_BUF_LEN));
	chanim = (wl_chanim_stats_t *)buf;

	chanim->count = swap ? BCMSWAP32(chanim->count) : chanim->count;

	if (chanim->count == 1) {
		stats = chanim->stats;
		stats->glitchcnt = swap ? BCMSWAP32(stats->glitchcnt) : stats->glitchcnt;
		diag->channel_busy = (uint64_t)((100 - stats->ccastats[CCASTATS_TXOP]) * 10000);
		diag->tx_airtime = (uint64_t)(stats->ccastats[CCASTATS_TXDUR] * 10000);
		diag->rx_airtime = (uint64_t)(stats->ccastats[CCASTATS_INBSS] * 10000);
		diag->obss_airtime = (uint64_t)(stats->ccastats[CCASTATS_OBSS] * 10000);
		diag->cca_time = (uint64_t)1000000;
		diag->false_cca_count = (uint64_t)stats->glitchcnt;
	} else
		libwifi_warn("Invalid chanim count %d\n", chanim->count);

	return 0;
}

int bcmwl_radio_get_channels(const char *name, enum wifi_bw bw, enum wifi_chan_ext sb, uint32_t *channels, int *num)
{
	char buf[WLC_IOCTL_MAXLEN] = {0};
	wl_uint32_list_t *list;
	uint32_t ctrl_channel;
	chanspec_t c;
	int count;
	int ret;
	int i;
	int swap = wl_swap(name);

	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_VAR,
		.iovar = "chanspecs",
		.buf = &buf[0],
		.buflen = sizeof(buf)
	};

	libwifi_dbg("[%s] %s called bw %d num %d\n", name, __func__, bw, *num);

	ret = wl_ctl(&arg);
	if (WARN_ON(ret))
		return ret;

	memset(channels, 0, *num * sizeof(uint32_t));

	list = (wl_uint32_list_t *)buf;
	list->count = swap ? BCMSWAP32(list->count) : list->count;

	count = 0;
	for (i = 0; i < list->count; i++) {
		if (WARN_ON(count >= *num))
			break;

		c = swap ? (chanspec_t)BCMSWAP32(list->element[i]) :
				(chanspec_t)list->element[i];
		ctrl_channel = chanspec_to_ctrlchannel(c);

		switch (bw) {
		case BW20:
			if (CHSPEC_IS20(c))
				channels[count++] = ctrl_channel;
			break;
		case BW40:
			if (CHSPEC_IS40(c)) {
				switch (sb) {
				case EXTCH_ABOVE:
					if (CHSPEC_SB_LOWER(c))
						channels[count++] = ctrl_channel;
					break;
				case EXTCH_BELOW:
					if (CHSPEC_SB_UPPER(c))
						channels[count++] = ctrl_channel;
					break;
				default:
					channels[count++] = ctrl_channel;
					break;
				}
			}
			break;
		case BW80:
			if (CHSPEC_IS80(c))
				channels[count++] = ctrl_channel;
			break;
		case BW160:
			if (CHSPEC_IS160(c))
				channels[count++] = ctrl_channel;
			break;
		case BW8080:
			if (CHSPEC_IS8080(c))
				channels[count++] = ctrl_channel;
			break;
		default:
			break;
		}
	}

	*num = count;
	return 0;
}

int bcmwl_iface_mbo_disallow_assoc(const char *ifname, uint8_t reason)
{
	char buf[256] = {0};
	int status;

	status = Cmd(buf, sizeof(buf), "wl -i %s mbo ap_assoc_disallowed %u", ifname, reason);
	WARN_ON(status);

	return status;
}

int bcmwl_iface_ap_set_state(const char *ifname, bool up)
{
	char buf[256] = {0};
	char state[256] = {0};
	int status;

	/* First check if hostapd already configured */
	if (hostapd_cli_ap_get_state(ifname, state, sizeof(state)))
		return -1;

	if (!strcmp(state, "DISABLED"))
		return -1;

	/* enable/disable beaconing */
	status = Cmd(buf, sizeof(buf), "wl -i %s bss %s", ifname, up ? "up" : "down");
	WARN_ON(status);

	return status;
}

/* Events handling */
static const char *event2str(int e)
{
#define C2S(x) case x: return #x
	switch (e) {
	C2S(WLC_E_DISASSOC_IND);
	C2S(WLC_E_DEAUTH_IND);
	C2S(WLC_E_DEAUTH);
	C2S(WLC_E_DISASSOC);
	C2S(WLC_E_ASSOC);
	C2S(WLC_E_ASSOC_IND);
	C2S(WLC_E_REASSOC);
	C2S(WLC_E_REASSOC_IND);
	C2S(WLC_E_AUTH);
	C2S(WLC_E_AUTH_IND);
	C2S(WLC_E_ACTION_FRAME);
	C2S(WLC_E_ACTION_FRAME_RX);
	C2S(WLC_E_CSA_COMPLETE_IND);
	C2S(WLC_E_AP_CHAN_CHANGE);
	C2S(WLC_E_IF);
	C2S(WLC_E_RADAR_DETECTED);
	C2S(WLC_E_PROBREQ_MSG);
	C2S(WLC_E_RADIO);
	C2S(WLC_E_LINK);
	C2S(WLC_E_PROBREQ_MSG_RX);
	C2S(WLC_E_CAC_STATE_CHANGE);
	C2S(WLC_E_ESCAN_RESULT);
	C2S(WLC_E_PROBE_STA_IND);
	}
#undef C2S
	return "";
}

static void bcmwl_bpf_filter_probe(const char *ifname, int fd)
{
	struct sock_filter code[] = {
		/* load and accumulates event_type */
		BPF_STMT(BPF_LD | BPF_W | BPF_ABS, offsetof(bcm_event_t, event.event_type)),

		/*
		 * check accumulated event_type is equal to WLC_E_PROBREQ_MSG
		 *  - true jump with offset 2 - return 0, ignore packet
		 *  - false jump with offset 0 - check WLC_E_PROBREQ_MSG_RX
		 */
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, WLC_E_PROBREQ_MSG, 2, 0),

		/*
		 * check accumulated event_type is equal to WLC_E_PROBREQ_MSG_RX
		 *  - true jump with offset 1 - return 0, ignore packet
		 *  - false jump with offset 1 - check WLC_E_PROBE_STA_IND
		 */
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, WLC_E_PROBREQ_MSG_RX, 1, 0),

		/*
		 * check accumulated event_type is equal to WLC_E_PROBE_STA_IND
		 *  - true jump with offset 0 - return 0, ignore packet
		 *  - false jump with offset 1 - accept packet
		 */
		BPF_JUMP(BPF_JMP | BPF_JEQ | BPF_K, WLC_E_PROBE_STA_IND, 0, 1),
		BPF_STMT(BPF_RET | BPF_K, 0),
		BPF_STMT(BPF_RET | BPF_K, 0xffff),
	};
	struct sock_fprog bpf = {
		.len = ARRAY_SIZE(code),
		.filter = code
	};

	if (setsockopt(fd, SOL_SOCKET, SO_ATTACH_FILTER, &bpf, sizeof(bpf)) < 0)
		libwifi_err("[%s] attach bpf filter failed errno %d (%s)", ifname, errno, strerror(errno));
}

static int bcmwl_event_socket_open(const char *ifname)
{
	struct sockaddr_ll sll;
	int ifindex;
	int fd;

	ifindex = if_nametoindex(ifname);
	if (WARN_ON(ifindex == 0))
		return -1;

	if ((fd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE_BRCM))) < 0) {
		libwifi_err("[%s] open socket error: %s\n", ifname, strerror(errno));
		return -1;
	}

	memset(&sll, 0, sizeof(sll));
	sll.sll_family   = AF_PACKET;
	sll.sll_protocol = htons(ETHER_TYPE_BRCM);
	sll.sll_ifindex  = ifindex;
	if (bind(fd, (struct sockaddr *) &sll, sizeof(sll)) < 0) {
		libwifi_err("[%s] bind error %s\n", ifname, strerror(errno));
		close(fd);
		return -1;
	}

	/* Install bpf filter and skip probe frames, while we don't need them in ESL */
	bcmwl_bpf_filter_probe(ifname, fd);

	return fd;
}

static int bcmwl_event_socket_close(int fd)
{
    return close(fd);
}

static ssize_t bcmwl_event_socket_read(int fd, void *msg, size_t msglen)
{
	return recv(fd, msg, msglen, MSG_DONTWAIT);
}

/* Internal bcmwl event context */
struct bcmwl_event_ctx {
	struct event_struct ev;
	int fd;
	int resp_max_len;
};

int bcmwl_register_event(const char *ifname, struct event_struct *ev,
			  void **evhandle)
{
	struct bcmwl_event_ctx *ctx;
	char buf[1024] = {0};

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	*evhandle = NULL;

	ctx = calloc(1, sizeof(*ctx));
	WARN_ON(!ctx);
	if (!ctx)
		return -1;

	ev->fd_monitor = bcmwl_event_socket_open(ifname);
	if (ev->fd_monitor == -1) {
		libwifi_err("[%s] %s open socket failed %d\n", ifname, __func__, errno);
		free(ctx);
		return -1;
	}

	memcpy(&ctx->ev, ev, sizeof(*ev));
	ctx->fd = ev->fd_monitor;
	ctx->resp_max_len = ev->resp.len;

	*evhandle = ctx;

	/* Register events we would like report to upper layer */
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_DISASSOC_IND));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_DEAUTH_IND));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_DEAUTH));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_DISASSOC));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_ASSOC_IND));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_ASSOC));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_REASSOC_IND));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_REASSOC));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_AUTH_IND));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_AUTH));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_ACTION_FRAME));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_ACTION_FRAME_RX));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_CSA_COMPLETE_IND));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_AP_CHAN_CHANGE));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_RADAR_DETECTED));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_CAC_STATE_CHANGE));
	WARN_ON(bcmwl_enable_event_bit(ifname, WLC_E_ESCAN_RESULT));

	/* Make sure we will get this events */
	Cmd(buf, sizeof(buf), "ebtables -t broute -D BROUTING -p 0x886c -j DROP");
	Cmd(buf, sizeof(buf), "ebtables -t broute -I BROUTING 1 -p 0x886c -j DROP");

	/* Don't pass EAPOLs to bridge */
	Cmd(buf, sizeof(buf), "ebtables -t broute -D BROUTING -p 0x888e -j DROP");
	Cmd(buf, sizeof(buf), "ebtables -t broute -I BROUTING 1 -p 0x888e -j DROP");

	return 0;
}

int bcmwl_unregister_event(const char *ifname, void *evhandle)
{
	struct bcmwl_event_ctx *ctx = evhandle;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	bcmwl_event_socket_close(ctx->fd);
	free(ctx);

	/*
	 * Don't disable event bits, while we could have some
	 * other user for that. Found that disable all events
	 * introduce issue(s) for DHD implementation, next
	 * hostapd won't get cfg80211 events.
	 */

	return 0;
}

static int bcmwl_event_handle(const char *ifname, bcm_event_t *event, int size, char *buf, size_t buf_size)
{
	char macaddr[18] = {0};
	char raw[1024] = {0};
	char *raw_ptr = &raw[0];
	char event_name[128] = {0};
	uint32_t event_type;
	uint32_t datalen = 0;
	uint32_t reason = -1;

	datalen = ntohl(event->event.datalen);
	event_type = ntohl(event->event.event_type);

	libwifi_dbg("[%s] %s called event %u (%s) iface %s size %d datalen %u\n",
		    ifname, __func__, event_type, event_name, event->event.ifname,
		    size, datalen);

	/* Check frame length */
	if (WARN_ON(sizeof(*event) + datalen >= size))
		return 0;

	/* Check if we have place for raw data */
	if (datalen * 2 < sizeof(raw))
		btostr((uint8_t *) (event + 1), datalen, raw);

	/* Fill macaddr */
	snprintf(macaddr, sizeof(macaddr), MACSTR, MAC2STR(event->event.addr.octet)); /* Flawfinder: ignore */


	switch (event_type) {
		case WLC_E_DISASSOC_IND:
		case WLC_E_DISASSOC:
			snprintf(event_name, sizeof(event_name), "disassoc");
			reason = ntohl(event->event.reason);
			break;
		case WLC_E_DEAUTH_IND:
		case WLC_E_DEAUTH:
			snprintf(event_name, sizeof(event_name), "deauth");
			reason = ntohl(event->event.reason);
			break;
		case WLC_E_ASSOC:
		case WLC_E_ASSOC_IND:
			snprintf(event_name, sizeof(event_name), "assoc");
			break;
		case WLC_E_REASSOC:
		case WLC_E_REASSOC_IND:
			snprintf(event_name, sizeof(event_name), "reassoc");
			break;
		case WLC_E_AUTH:
		case WLC_E_AUTH_IND:
			snprintf(event_name, sizeof(event_name), "auth");
			break;
		case WLC_E_ACTION_FRAME:
			snprintf(event_name, sizeof(event_name), "action");
			break;
		case WLC_E_ACTION_FRAME_RX:
			snprintf(event_name, sizeof(event_name), "action");
			/* Skip wl_event_rx_frame_data_t header */
			raw_ptr += 2 * sizeof(wl_event_rx_frame_data_t);
			break;
		/* we already have this from hostapd/supplicant */
		case WLC_E_AUTHORIZED:
		default:
			return -1;
	}

	snprintf(buf, buf_size,
		 "wifi.sta '{\"ifname\":\"%s\", \"event\":\"%s\", \"data\":{\"macaddr\":\"%s\"",
		 event->event.ifname, event_name, macaddr);

	if (reason != -1)
		snprintf(buf + strlen(buf), buf_size, ", \"reason\":%u", reason);


	if (datalen > 0)
		snprintf(buf + strlen(buf), buf_size, ", \"raw\":\"%s\"", raw_ptr);

	snprintf(buf + strlen(buf), buf_size, "%s", "}}'");

	return 0;
}

static void bcmwl_get_chanspec(chanspec_t chanspec, char *chan, size_t chan_size, char *bw, size_t bw_size)
{
	snprintf(chan, chan_size, "%u", chanspec_to_ctrlchannel(chanspec));
	if (CHSPEC_IS20(chanspec))
		snprintf(bw, bw_size, "20");
	if (CHSPEC_IS40(chanspec))
		snprintf(bw, bw_size, "40");
	if (CHSPEC_IS80(chanspec))
		snprintf(bw, bw_size, "80");
	if (CHSPEC_IS160(chanspec))
		snprintf(bw, bw_size, "160");
	if (CHSPEC_IS8080(chanspec))
		snprintf(bw, bw_size, "8080");
}

static int bcmwl_event_handle_csa(const char *ifname, bcm_event_t *event, int size, char *buf, size_t buf_size)
{
	chanspec_t chanspec;
	char chan[32], bw[32];

	/* Get current chanspec */
	if (!bcmwl_radio_get_chanspec(ifname, &chanspec)) {
		bcmwl_get_chanspec(chanspec, chan, sizeof(chan), bw, sizeof(bw));
	} else {
		snprintf(chan, sizeof(chan), "unknown");
		snprintf(bw, sizeof(bw), "unknown");
	}

	snprintf(buf, buf_size,
		 "wifi.radio '{\"ifname\":\"%s\", \"event\":\"%s\", \"data\": {\"status\": \"%d\", \"channel\": \"%s\", \"bandwidth\":\"%s\"}}'",
		 event->event.ifname, "csa-finished", ntohl(event->event.reason), chan, bw);

	return 0;
}

static int bcmwl_event_handle_radar(const char *ifname, bcm_event_t *event, int size, char *buf, size_t buf_size)
{
	wl_event_radar_detect_data_t *ev;
	char chan[32], tchan[32];
	char bw[32], tbw[32];
	int swap;

	ev = (wl_event_radar_detect_data_t *) (event + 1);

	swap = wl_swap(ifname);

	if (swap) {
		ev->current_chanspec = BCMSWAP16(ev->current_chanspec);
		ev->target_chanspec = BCMSWAP16(ev->target_chanspec);
	}

	bcmwl_get_chanspec(ev->current_chanspec, chan, sizeof(chan), bw, sizeof(bw));
	bcmwl_get_chanspec(ev->target_chanspec, tchan, sizeof(tchan), tbw, sizeof(tbw));

	snprintf(buf, buf_size,
		 "wifi.radio '{\"ifname\":\"%s\", \"event\":\"%s\", \"data\": {\"channel\":\"%s\", \"bandwidth\":\"%s\"}, \"target_channel\":\"%s\", \"target_bandwidt\":\"%s\"}'",
		 event->event.ifname, "radar", chan, bw, tchan, tbw);

	return 0;
}

static int bcmwl_event_handle_chan_change(const char *ifname, bcm_event_t *event, int size, char *buf, size_t buf_size)
{
	wl_event_change_chan_t *ev;
	char event_name[64] = {0};
	char reason[64] = {0};
	unsigned int length;
	char chan[32];
	char bw[32] = "20";
	int swap;

	ev = (wl_event_change_chan_t *)(event + 1);
	length = ntohl(event->event.datalen);

	if (WARN_ON(length < sizeof(*ev)))
		return -1;

	/* Header - bcm_event_t using network endianness */
	swap = wl_swap(ifname);

	snprintf(event_name, sizeof(event_name), "ap-chan-change");

	/* Data after bcm_event_t using HW endianness */
	if (swap) {
		ev->reason = BCMSWAP32(ev->reason);
		ev->target_chanspec = BCMSWAP16(ev->target_chanspec);
	}

	switch (ev->reason) {
	case WL_CHAN_REASON_CSA:
		snprintf(reason, sizeof(reason), "csa");
		break;
	case WL_CHAN_REASON_DFS_AP_MOVE_START:
		snprintf(reason, sizeof(reason), "move-start");
		break;
	case WL_CHAN_REASON_DFS_AP_MOVE_RADAR_FOUND:
		snprintf(reason, sizeof(reason), "radar");
		break;
	case WL_CHAN_REASON_DFS_AP_MOVE_ABORTED:
		snprintf(reason, sizeof(reason), "move-aborted");
		break;
	case WL_CHAN_REASON_DFS_AP_MOVE_SUCCESS:
		snprintf(reason, sizeof(reason), "move-success");
		break;
	case WL_CHAN_REASON_DFS_AP_MOVE_STUNT:
		snprintf(reason, sizeof(reason), "move-stunt");
		break;
	case WL_CHAN_REASON_CSA_TO_DFS_CHAN_FOR_CAC_ONLY:
		snprintf(reason, sizeof(reason), "cac-only");
		break;
	case WL_CHAN_REASON_ANY:
		snprintf(reason, sizeof(reason), "any");
		break;
	default:
		libwifi_err("[%s] %s unhandled reason %d\n", ifname, __func__, ev->reason);
		return -1;
	}

	bcmwl_get_chanspec(ev->target_chanspec, chan, sizeof(chan), bw, sizeof(bw));

	snprintf(buf, buf_size,
		 "wifi.radio '{\"ifname\":\"%s\", \"event\":\"%s\", \"data\": {\"reason\":\"%s\", \"target-channel\":\"%s\", \"target-width\":\"%s\"}}'",
		 event->event.ifname, event_name, reason, chan, bw);

	return 0;
}

static int bcmwl_event_handle_if(const char *ifname, bcm_event_t *event, int size, char *buf, size_t buf_size)
{
	struct wl_event_data_if *ev;
	char event_name[64] = {0};
	char macaddr[18] = {0};
	char role[64] = {0};
	unsigned int length;
	char name[16] = {0};

	ev = (struct wl_event_data_if *)(event + 1);
	length = ntohl(event->event.datalen);

	if (WARN_ON(length < sizeof(*ev)))
		return -1;

	switch (ev->opcode) {
	case WLC_E_IF_ADD:
		snprintf(event_name, sizeof(event_name), "iface-add");
		break;
	case WLC_E_IF_DEL:
		snprintf(event_name, sizeof(event_name), "iface-del");
		break;
	case WLC_E_IF_CHANGE:
		snprintf(event_name, sizeof(event_name), "iface-change");
		break;
	case WLC_E_IF_BSSCFG_UP:
		snprintf(event_name, sizeof(event_name), "bss-up");
		break;
	case WLC_E_IF_BSSCFG_DOWN:
		snprintf(event_name, sizeof(event_name), "bss-down");
		break;
	default:
		return -1;
	}

	switch (ev->role) {
	case WLC_E_IF_ROLE_STA:
		snprintf(role, sizeof(role), "sta");
		break;
	case WLC_E_IF_ROLE_AP:
		snprintf(role, sizeof(role), "ap");
		break;
	case WLC_E_IF_ROLE_WDS:
		snprintf(role, sizeof(role), "apvlan");
		break;
	default:
		break;
	}

	snprintf(macaddr, sizeof(macaddr), MACSTR, MAC2STR(ev->peer_addr.octet));	/* Flawfinder: ignore */

	/* TODO check if this is correct */
	snprintf(name, sizeof(name), "wds%u.0.%u", ev->bssidx, ev->ifidx);

	libwifi_dbg("[%s] %s opcode %s role %s ifidx %u bssidx %u macaddr %s [%s]\n",
		    ifname, __func__, event_name, role, ev->ifidx, ev->bssidx, macaddr, name);

	snprintf(buf, buf_size,
		 "wifi.iface '{\"ifname\":\"%s\", \"event\":\"%s\", \"data\": {\"role\":\"%s\", \"macaddr\":\"%s\"}}'",
		 event->event.ifname, event_name, role, macaddr);

	/* Send to upper layer - seems today only DHD report this event
	 * TODO fix it
	 */
	switch (ev->role) {
	case WLC_E_IF_ROLE_WDS:
		switch (ev->opcode) {
		case WLC_E_IF_ADD:
			snprintf(buf, buf_size,
				 "wifi.iface '{\"ifname\":\"%s\", \"event\": \"wds-station-added\", \"data\" : {\"ifname\":\"%s\", \"macaddr\":\"%s\"}}'",
				 ifname, name, macaddr);
			break;
		case WLC_E_IF_DEL:
			snprintf(buf, buf_size,
				 "wifi.iface '{\"ifname\":\"%s\", \"event\": \"wds-station-removed\", \"data\" : {\"ifname\":\"%s\", \"macaddr\":\"%s\"}}'",
				 ifname, name, macaddr);
			break;
		default:
			break;
		}
	default:
		break;
	}

        return 0;
}


static const char *dfs_state2str(int e)
{
#define C2S(x) case x: return #x
	switch (e) {
	C2S(WL_DFS_CACSTATE_IDLE);
	C2S(WL_DFS_CACSTATE_PREISM_CAC);
	C2S(WL_DFS_CACSTATE_ISM);
	C2S(WL_DFS_CACSTATE_CSA);
	C2S(WL_DFS_CACSTATE_POSTISM_CAC);
	C2S(WL_DFS_CACSTATE_PREISM_OOC);
	C2S(WL_DFS_CACSTATE_POSTISM_OOC);
	}
#undef C2S
	return "unknown";
}

static int bcmwl_event_handle_cac_state_change(const char *ifname, bcm_event_t *event, int size, struct bcmwl_event_ctx *ctx)
{
	wlc_cac_event_t *ev;
	wl_dfs_status_all_t *scan_status;
	wl_dfs_sub_status_t *status;
	char out[2048] = {0};
	//char event_name[64] = {0};
	unsigned int length;
	struct event_response *resp;
	char chan[64] = {0};
	char bw[64] = {0};
	int resp_max;
	int out_len;
	int swap;
	int i;

	ev = (wlc_cac_event_t *)(event + 1);
	length = ntohl(event->event.datalen);

	if (WARN_ON(length < sizeof(*ev)))
		return -1;

	resp = &ctx->ev.resp;
	resp_max = ctx->resp_max_len;

	swap = wl_swap(ifname);

	scan_status = &ev->scan_status;
	if (swap) {
		ev->version = BCMSWAP16(ev->version);
		ev->length = BCMSWAP16(ev->length);
		ev->type = BCMSWAP16(ev->type);

		/* TODO check other versions */
		if (WARN_ON(ev->version != WLC_E_CAC_STATE_TYPE_DFS_STATUS_ALL))
			return -1;

		scan_status->version = BCMSWAP16(scan_status->version);
		scan_status->num_sub_status = BCMSWAP16(scan_status->num_sub_status);

		for (i = 0; i < scan_status->num_sub_status; i++) {
			status = &scan_status->dfs_sub_status[i];
			status->state = BCMSWAP32(status->state);
			status->duration = BCMSWAP32(status->duration);
			status->chanspec = BCMSWAP16(status->chanspec);
			status->chanspec_last_cleared = BCMSWAP16(status->chanspec_last_cleared);
			status->sub_type = BCMSWAP16(status->sub_type);
		}
	}

	for (i = 0; i < scan_status->num_sub_status; i++) {
		status = &scan_status->dfs_sub_status[i];
		bcmwl_get_chanspec(status->chanspec, chan, sizeof(chan), bw, sizeof(bw));
		libwifi_dbg("[%s] [%d] chan %s bw %s state %u %s\n", ifname, i, chan, bw, status->state, dfs_state2str(status->state));

		if (status->chanspec == INVCHANSPEC)
			continue;

		if (status->state == WL_DFS_CACSTATE_IDLE || status->state == WL_DFS_CACSTATE_ISM)
			continue;

		if (i != 0 && status->state != WL_DFS_CACSTATE_PREISM_CAC)
			continue;

		snprintf(out, sizeof(out),
			 "wifi.radio '{\"ifname\":\"%s\", \"event\": \"%s\", \"data\" : {\"channel\":\"%s\", \"bw\":\"%s\"}}'",
			 event->event.ifname, dfs_state2str(status->state), chan, bw);

		out_len = strlen(out);
		if (WARN_ON(out_len >= resp_max))
			return 0;

		resp->type = WIFI_EVENT_VENDOR;
		memcpy(resp->data, out, out_len);
		resp->len = out_len;

		if (ctx->ev.cb)
			ctx->ev.cb(&ctx->ev);
	}

	return 0;
}

static int bcmwl_event_handle_escan_result(const char *ifname, bcm_event_t *event,
					   int size, char *out, size_t olen)
{
	wl_escan_result_t *ev;
	uint32_t status;
	//int swap;
	struct wifi_radio_diagnostic diag;
	int ret;


	ev = (wl_escan_result_t *)(event + 1);
	//swap = wl_swap(ifname);
	status = ntohl(event->event.status);
	libwifi_dbg("escan status = 0x%x\n", status);

	if (status == WLC_E_STATUS_PARTIAL) {
		wl_bss_info_t *bi = &ev->bss_info[0];
		int noise = 0;

		ret = bcmwl_radio_get_diag(ifname, &diag);
		      bcmwl_radio_get_noise(ifname, &noise);
		if (!ret) {
			uint8_t chutil = (uint8_t)((diag.channel_busy * 255) / 100);

			libwifi_dbg("channel utilization: %llu (normalized: %u)\n",
				diag.channel_busy,
				(uint8_t)((diag.channel_busy * 255) / 100));

			snprintf(out, olen,
				"wifi.radio '{\"ifname\":\"%s\", \"event\": \"%s\", \"data\" : {\"channel\":%u, \"noise\":%d, \"utilization\":%u}}'",
				event->event.ifname, "survey", bi->ctl_ch, noise, chutil);
		}
	}

	return 0;
}

int bcmwl_recv_event(const char *ifname, void *evhandle)
{
	struct bcmwl_event_ctx *ctx = evhandle;
	char buf[2048] = {0};
	uint32_t event_type;
	bcm_event_t *event;
	int frame_size;

	struct event_response *resp;
	char out[2048] = {0};
	int resp_max;
	int out_len;
	int res;

	if (WARN_ON(!ctx))
		return -1;

	frame_size = bcmwl_event_socket_read(ctx->fd, buf, sizeof(buf));
	if (frame_size < 0) {
		if (errno != EAGAIN)
			libwifi_dbg("[%s] %s read error %s (%d)\n", ifname, __func__, strerror(errno), errno);
		return -errno;
	}

	if (WARN_ON(frame_size < sizeof(bcm_event_t)))
		return -1;

	/* This is what we get from lower layer */
	event = (bcm_event_t *) buf;
	event_type = ntohl(event->event.event_type);

	/* To upper layer */
	resp = &ctx->ev.resp;
	resp_max = ctx->resp_max_len;

	libwifi_dbg("[%s] event %d (%s) size %d\n", ifname, event_type, event2str(event_type), frame_size);

	switch (event_type) {
	case WLC_E_DISASSOC_IND:
	case WLC_E_DEAUTH_IND:
	case WLC_E_DEAUTH:
	case WLC_E_DISASSOC:
	case WLC_E_ASSOC:
	case WLC_E_ASSOC_IND:
	case WLC_E_REASSOC:
	case WLC_E_REASSOC_IND:
	case WLC_E_AUTH:
	case WLC_E_AUTH_IND:
	//case WLC_E_ACTION_FRAME:
	case WLC_E_ACTION_FRAME_RX:
		res = bcmwl_event_handle(ifname, event, frame_size, out, sizeof(out));
		break;
	case WLC_E_CSA_COMPLETE_IND:
		res = bcmwl_event_handle_csa(ifname, event, frame_size, out, sizeof(out));
		break;
	case WLC_E_AP_CHAN_CHANGE:
		res = bcmwl_event_handle_chan_change(ifname, event, frame_size, out, sizeof(out));
		break;
	case WLC_E_IF:
		res = bcmwl_event_handle_if(ifname, event, frame_size, out, sizeof(out));
		break;
	case WLC_E_RADAR_DETECTED:
		res = bcmwl_event_handle_radar(ifname, event, frame_size, out, sizeof(out));
		break;
	case WLC_E_CAC_STATE_CHANGE:
		/* Handle this internally, while we will send two events */
		return bcmwl_event_handle_cac_state_change(ifname, event, frame_size, ctx);
	case WLC_E_ESCAN_RESULT:
		res = bcmwl_event_handle_escan_result(ifname, event, frame_size, out, sizeof(out));
		break;
	case WLC_E_PROBREQ_MSG:
	case WLC_E_RADIO:
	case WLC_E_LINK:
	case WLC_E_PROBREQ_MSG_RX:
	default:
		return 0;
	}

	UNUSED(res);

	out_len = strlen(out);
	if (WARN_ON(out_len >= resp_max))
		return 0;

	/* Pass to upper layer */
	resp->type = WIFI_EVENT_VENDOR;
	memcpy(resp->data, out, out_len);
	resp->len = out_len;

	if (ctx->ev.cb)
		ctx->ev.cb(&ctx->ev);

	return 0;
}

#ifndef WL_EVENTING_MASK_MAX_LEN
#define WL_EVENTING_MASK_MAX_LEN 64
#endif

static int bcmwl_event_bit(const char *ifname, unsigned int bit, int set)
{
	int mask_len = WL_EVENTING_MASK_MAX_LEN;
	eventmsgs_ext_t *ext;
	int ret = -1;

	ext = calloc(1, sizeof(*ext) + mask_len);
	WARN_ON(!ext);
	if (!ext)
		return -1;

	if (WARN_ON((bit / 8) >= mask_len)) {
		free(ext);
		return -1;
	}

	ext->ver = EVENTMSGS_VER;
	ext->len = mask_len;

	if (set) {
		ext->mask[bit/8] = (1 << (bit % 8));
		ext->command = EVENTMSGS_SET_BIT;
	} else {
		memset(ext->mask, 0xff, mask_len);
		ext->mask[bit/8] = ~(1 << (bit % 8));
		ext->command = EVENTMSGS_RESET_BIT;
	}

	ret = wl_iovar_set(ifname, "event_msgs_ext", ext, sizeof(*ext) + mask_len, NULL, 0);

	free(ext);
	return ret;
}

int bcmwl_enable_event_bit(const char *ifname, unsigned int bit)
{
	return bcmwl_event_bit(ifname, bit, 1);
}

int bcmwl_disable_event_bit(const char *ifname, unsigned int bit)
{
	return bcmwl_event_bit(ifname, bit, 0);
}

int bcmwl_driver_info(const char *name, struct wifi_metainfo *info)
{
	size_t len = (sizeof(((struct wifi_metainfo*)0)->fw_data));

	libwifi_dbg("[%s] %s called\n", name, __func__);

	nlwifi_driver_info(name, info);
	wl_ioctl_get_version(name, info->fw_data, &len);

	return 0;
}

int bcmwl_iface_get_wpa_auth(const char *name, uint32_t *wpa_auth)
{
	uint32_t val = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_WPA_AUTH,
		.buf = &val,
		.buflen = sizeof(val)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = wl_ctl(&arg);
	if (!ret) {
		*wpa_auth = wl_swap_32(name, val);
		return ret;
	}

	return ret;
}

int bcmwl_iface_get_auth(const char *name, uint32_t *auth)
{
	uint32_t val = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_AUTH,
		.buf = &val,
		.buflen = sizeof(val)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = wl_ctl(&arg);
	if (!ret) {
		*auth = wl_swap_32(name, val);
		return ret;
	}

	return ret;
}

int bcmwl_iface_get_wsec(const char *name, uint32_t *enc)
{
	uint32_t val = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_WSEC,
		.buf = &val,
		.buflen = sizeof(val)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = wl_ctl(&arg);
	if (!ret) {
		*enc = wl_swap_32(name, val);
		return ret;
	}

	return ret;
}

int bcmwl_iface_get_doth11h(const char *name, bool *doth11h)
{
	uint32_t val = 0;
	int ret;
	struct wl_arg arg = {
		.ifname = name,
		.cmd = WLC_GET_SPECT_MANAGMENT,
		.buf = &val,
		.buflen = sizeof(val)
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	ret = wl_ctl(&arg);
	if (!ret) {
		*doth11h = !!(wl_swap_32(name, val) != 0);
		return ret;
	}

	return ret;
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

int bcmwl_iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	char buf[WLC_IOCTL_MAXLEN] = {0};
	struct maclist *maclist;
	struct wl_arg arg = {
		.ifname = ifname,
		.cmd = WLC_GET_ASSOCLIST,
		.buf = &buf,
		.buflen = sizeof(buf)
	};
	int swap;
	int max;
	int i;

	max = (WLC_IOCTL_MAXLEN - sizeof(int)) / ETHER_ADDR_LEN;
	maclist = (struct maclist *) buf;
	swap = wl_swap(ifname);

	maclist->count = swap ? BCMSWAP32(max) : max;
	if (WARN_ON(wl_ioctl(&arg)))
		return -1;

	maclist->count = swap ? BCMSWAP32(maclist->count) : maclist->count;
	for (i = 0; i < maclist->count; i++) {
		if (WARN_ON(i >= *num_stas))
			return -1;

		memcpy(&stas[i * 6], maclist->ea[i].octet, 6);
	}

	*num_stas = i;
	return 0;
}

static char * he_get_table(int index)
{
	switch (index) {
	case 0:
		return "bw80_tx";
	case 1:
		return "bw80_rx";
	case 2:
		return "bw160_tx";
	case 3:
		return "bw160_rx";
	case 4:
		return "bw80p80_tx";
	case 5:
		return "bw80p80_rx";
	default:
		return "unknown";
	}
}

static char * he_get_mcs(uint8_t bits)
{
	switch (bits) {
	case 0:
		return "mcs0_7";
	case 1:
		return "mcs0_9";
	case 2:
		return "mcs0_11";
	default:
		return "unknown";
	}
}

/* TODO remove this when will be in public header */
#ifndef WL_RSPEC_HE_GI_MASK
#define WL_RSPEC_HE_GI_MASK     0x00000C00      /* HE GI indices */
#endif

#ifndef WL_RSPEC_HE_GI_SHIFT
#define WL_RSPEC_HE_GI_SHIFT    10
#endif

#ifndef RSPEC_HE_LTF_GI
#define RSPEC_HE_LTF_GI(rspec)  (((rspec) & WL_RSPEC_HE_GI_MASK) >> WL_RSPEC_HE_GI_SHIFT)
#endif

#ifndef WL_RSPEC_HE_1x_LTF_GI_0_8us
#define WL_RSPEC_HE_1x_LTF_GI_0_8us     (0x0)
#endif

#ifndef WL_RSPEC_HE_2x_LTF_GI_0_8us
#define WL_RSPEC_HE_2x_LTF_GI_0_8us     (0x1)
#endif

#ifndef WL_RSPEC_HE_2x_LTF_GI_1_6us
#define WL_RSPEC_HE_2x_LTF_GI_1_6us     (0x2)
#endif

#ifndef WL_RSPEC_HE_4x_LTF_GI_3_2us
#define WL_RSPEC_HE_4x_LTF_GI_3_2us     (0x3)
#endif

static int bcmwl_get_sta_info_rspec(struct wifi_rate *rate, uint32_t rspec)
{
	enum wifi_guard gi = WIFI_LGI;
	int bw = 20;
	int nss = 0;
	int mcs = 0;
	int phy;
	int stbc;
	int ldpc;

	rate->rate = (rspec & WL_RSPEC_RATE_MASK);
	stbc = ((rspec & WL_RSPEC_STBC) != 0) ? 1 : 0;
	ldpc = ((rspec & WL_RSPEC_LDPC) != 0) ? 1 : 0;

	switch (rspec & WL_RSPEC_BW_MASK) {
	case WL_RSPEC_BW_20MHZ:
		bw = 20;
		break;
	case WL_RSPEC_BW_40MHZ:
		bw = 40;
		break;
	case WL_RSPEC_BW_80MHZ:
		bw = 80;
		break;
	case WL_RSPEC_BW_160MHZ:
		bw = 160;
		break;
	default:
		bw = 20;
		break;
	}

	switch (rspec & WL_RSPEC_ENCODING_MASK) {
	case WL_RSPEC_ENCODE_RATE:
		phy = PHY_OFDM;
		bw = 20;
		nss = 1;
		break;
	case WL_RSPEC_ENCODE_HT:
		gi = ((rspec & WL_RSPEC_SGI)  != 0) ? WIFI_SGI : WIFI_LGI;
		nss = 1 + rate->rate / 8;
		mcs = rate->rate % 8;
		phy = PHY_HT;
		break;
	case WL_RSPEC_ENCODE_VHT:
		gi = ((rspec & WL_RSPEC_SGI)  != 0) ? WIFI_SGI : WIFI_LGI;
		nss = (rspec & WL_RSPEC_VHT_NSS_MASK) >> WL_RSPEC_VHT_NSS_SHIFT;
		mcs = (rspec & WL_RSPEC_VHT_MCS_MASK);
		phy = PHY_VHT;
		break;
	case WL_RSPEC_ENCODE_HE:
		nss = (rspec & WL_RSPEC_HE_NSS_MASK) >> WL_RSPEC_HE_NSS_SHIFT;
		mcs = (rspec & WL_RSPEC_HE_MCS_MASK);
		phy = PHY_HE;

		switch (RSPEC_HE_LTF_GI(rspec)) {
		case WL_RSPEC_HE_1x_LTF_GI_0_8us:
			gi = WIFI_1xLTF_GI800;
			break;
		case WL_RSPEC_HE_2x_LTF_GI_0_8us:
			gi = WIFI_2xLTF_GI800;
			break;
		case WL_RSPEC_HE_2x_LTF_GI_1_6us:
			gi = WIFI_2xLTF_GI1600;
			break;
		case WL_RSPEC_HE_4x_LTF_GI_3_2us:
			gi = WIFI_4xLTF_GI3200;
			break;
		default:
			break;
		}

		break;
	default:
		return -1;
	}

	libwifi_dbg("phy %d rate %d bw %d nss %d mcs %d gi %d stbc %d ldpc %d\n",
		    phy, rate->rate, bw, nss, mcs, gi, stbc, ldpc);

	switch (phy) {
	case PHY_OFDM:
		rate->rate /= 2;
		rate->m.nss = nss;
		rate->m.bw = bw;
		break;
	default:
		rate->m.bw = bw;
		rate->m.nss = nss;
		rate->m.mcs = mcs;
		rate->m.sgi = gi;
		rate->rate = wifi_mcs2rate(mcs, bw, nss, gi);
		break;
	}

	rate->phy = phy;

	return 0;
}

int bcmwl_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *wsta)
{
	sta_info_v8_t *sta;
	char buf[WLC_IOCTL_MAXLEN] = {0};
	int bw = 20;
	int sgi = 0;
	int nss = 0;
	int max_mcs = -1;
	int swap;
	int i;
	int j;


	if (wsta == NULL)
		return -1;

	swap = wl_swap(ifname);

	if (wl_iovar_get(ifname, "sta_info", addr, 6, buf, sizeof(buf)) < 0) {
		libwifi_err("wl_iovar_get error!!\n");
		return -1;
	}

	sta = (sta_info_v8_t *) buf;
	if (swap)
		sta->ver = BCMSWAP16(sta->ver);

	if (WARN_ON(sta->ver != WL_STA_VER_8))
		return -1;

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
		sta->he_flags = BCMSWAP32(sta->he_flags);
		sta->omi = BCMSWAP16(sta->omi);

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
			sta->tx_rspec = BCMSWAP32(sta->tx_rspec);
			sta->rx_rspec = BCMSWAP32(sta->rx_rspec);
		}
	}

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

		{
			uint8_t *supp_mcs = sta->rateset_adv.mcs;
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
			memcpy(wsta->caps.ht.supp_mcs, supp_mcs, 16);
			if (sta->link_bw > 0 && sta->link_bw <= 4)
				bw *= (1 << (sta->link_bw - 1));

			if (max_mcs > 0) {
				wsta->rate.rate = wifi_mcs2rate(max_mcs, bw, nss, sgi ? WIFI_SGI : WIFI_LGI);
				wsta->rate.m.mcs = max_mcs;
				wsta->maxrate = wsta->rate.rate;
			}
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

		{
			int octel = 0;
			int shift = 0;
			uint8_t supp_mcs[2] = {0xff, 0xff};
			const uint8_t mask[4] = {0xfc, 0xf3, 0xcf, 0x3f};

			nss = 0;
			max_mcs = 0;
			for (i = 0; i < WL_VHT_CAP_MCS_MAP_NSS_MAX; i++) {
				uint16_t vht_mcs;

				vht_mcs = swap ?
					BCMSWAP16(sta->rateset_adv.vht_mcs[i])
					: sta->rateset_adv.vht_mcs[i];


				if (!vht_mcs)
					break;

				octel = (2 * i) / 8;
				shift = (2 * i) % 8;

				if ((vht_mcs & 0x3ff) == 0x3ff) {
					max_mcs = 9;
					supp_mcs[octel] &= (mask[i%4] | (0x02 << shift));
				} else if ((vht_mcs & 0x3ff) == 0x1ff) {
					max_mcs = 8;
					supp_mcs[octel] &= (mask[i%4] | (0x01 << shift));
				} else {
					max_mcs = 7;
					supp_mcs[octel] &= (mask[i%4] | (0x00 << shift));
				}

				nss++;
			}

			bw = 20;
			memcpy(&wsta->caps.vht.supp_mcs[0], supp_mcs, 2);
			memcpy(&wsta->caps.vht.supp_mcs[4], supp_mcs, 2);
			if (sta->link_bw > 0 && sta->link_bw <= 4)
				bw *= (1 << (sta->link_bw - 1));

			libwifi_dbg("max_mcs %d bw %d nss %d sgi %d\n", max_mcs, bw, nss, sgi);
			if (max_mcs) {
				wsta->rate.rate = wifi_mcs2rate(max_mcs, bw, nss, sgi ? WIFI_SGI : WIFI_LGI);
				wsta->rate.m.mcs = max_mcs;
				wsta->maxrate = wsta->rate.rate;
			}
		}
	}

	if (!!(sta->flags & WL_STA_HE_CAP)) {
		wsta->caps.valid |= WIFI_CAP_HE_VALID;

		libwifi_dbg("he_flags: 0x%x\n", sta->he_flags);

		/* TODO check more flags here */
		if (!!(sta->he_flags & WL_STA_HE_LDPCCAP))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HE_LDPC_PAYLOAD);

		if (!!(sta->he_flags & WL_STA_HE_TX_STBCCAP))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HE_STBC_TX_80);

		if (!!(sta->he_flags & WL_STA_HE_RX_STBCCAP))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HE_STBC_RX_80);

		//if (!!(sta->he_flags & WL_STA_HE_HTC_CAP))
		//	wifi_cap_set(wsta->cbitmap, );

		if (!!(sta->he_flags & WL_STA_HE_SU_MU_BEAMFORMEE)) {
		//	wifi_cap_set(wsta->cbitmap, WIFI_CAP_HE_MU_BFE);
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HE_SU_BFE);
		}

		if (!!(sta->he_flags & WL_STA_HE_MU_BEAMFORMER))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HE_MU_BFR);

		if (!!(sta->he_flags & WL_STA_HE_SU_BEAMFORMER))
			wifi_cap_set(wsta->cbitmap, WIFI_CAP_HE_SU_BFR);

		for (i = 0; i < 6; i++) {
			uint16_t he_mcs;
			uint8_t bits;

			/*
			 * Index in table:
			 *  [0] = bw80_tx
			 *  [1] = bw80 rx
			 *  [2] = bw160 tx
			 *  [3] = bw160 rx
			 *  [4] = bw80p80 tx
			 *  [5] = bw80p80 rx
			 *
                         * he_mcs format:
			 *  Two bits per stream: d[1:0] is for stream #0, d[3:2] for stream #1, etc.
			 *  'b00: mandatory minimum support of MCS 0 through 7
			 *  'b01: support of MCS 0 through 8
			 *  'b10: support of MCS 0 through 9
			 *  'b11: no HE MCS'es supported
			 */

			he_mcs = swap ?
				BCMSWAP16(sta->rateset_adv.he_mcs[i])
				: sta->rateset_adv.he_mcs[i];

			if (he_mcs == 0xFFFF)
				continue;

			nss = 0;
			max_mcs = 0;
			for (j = 0; j < sizeof(he_mcs) * 8; j+=2) {
				bits = (he_mcs >> j) & 0x3;
				if (bits == 0x3)
					continue;

				libwifi_dbg("%s nss%d %s\n", he_get_table(i), j/2, he_get_mcs(bits));
				switch (bits) {
				case 0:
					max_mcs = 7;
					break;
				case 1:
					max_mcs = 9;
					break;
				case 2:
					max_mcs = 11;
					break;
				default:
					max_mcs = 0;
					break;
				}

				switch (i) {
				case 0:
				case 1:
					bw = 80;
					break;
				case 2:
				case 3:
					bw = 160;
					break;
				/* TODO handle 80p80 */
				case 4:
				case 5:
					bw = 160;
					break;
				default:
					bw = 0;
				}

				nss++;
			}

			libwifi_dbg("bw %d nss %d max_mcs %d\n", bw, nss, max_mcs);
			if (max_mcs) {
				wsta->rate.rate = wifi_mcs2rate(max_mcs, bw, nss, sgi ? WIFI_SGI : WIFI_LGI);
				wsta->rate.m.mcs = max_mcs;
				wsta->maxrate = wsta->rate.rate;
			}
		}
	}

	/* Set current nss/bw */
	wsta->rate.m.nss = nss;
	wsta->rate.m.sgi = sgi;

	switch (sta->link_bw) {
	case 1:
		wsta->rate.m.bw = BW20;
		break;
	case 2:
		wsta->rate.m.bw = BW40;
		break;
	case 3:
		wsta->rate.m.bw = BW80;
		break;
	case 4:
		wsta->rate.m.bw = BW160;
		break;
	default:
		wsta->rate.m.bw = BW_AUTO;
		break;
	}

	if (swap)
		sta->wnm_cap = BCMSWAP32(sta->wnm_cap);
	if (!!(sta->wnm_cap & 0x1))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_11V_BSS_TRANS);

	wsta->caps.valid |= WIFI_CAP_RM_VALID;
	if (sta->rrm_capabilities[0] & (1 << DOT11_RRM_CAP_LINK))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_RM_LINK);
	if (sta->rrm_capabilities[0] & (1 << DOT11_RRM_CAP_NEIGHBOR_REPORT))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_RM_NBR_REPORT);
	if (sta->rrm_capabilities[0] & (1 << DOT11_RRM_CAP_BCN_PASSIVE))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_RM_BCN_PASSIVE);
	if (sta->rrm_capabilities[0] & (1 << DOT11_RRM_CAP_BCN_ACTIVE))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_RM_BCN_ACTIVE);
	if (sta->rrm_capabilities[0] & (1 << DOT11_RRM_CAP_BCN_TABLE))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_RM_BCN_TABLE);
	if (sta->rrm_capabilities[3] & (1 << (DOT11_RRM_CAP_RCPI % 8)))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_RM_RCPI);
	if (sta->rrm_capabilities[3] & (1 << (DOT11_RRM_CAP_RSNI % 8)))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_RM_RSNI);
	if (sta->rrm_capabilities[3] & (1 << (DOT11_RRM_CAP_BSSAAD % 8)))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_BSS_AVG_ACCESS_DELAY);
	if (sta->rrm_capabilities[4] & (1 << (DOT11_RRM_CAP_BSSAAC % 8)))
		wifi_cap_set(wsta->cbitmap, WIFI_CAP_BSS_AVAIL_ADMISSION);

	/* status flags */

	/* libwifi_dbg("flags: 0x%08x\n", sta->flags); */
	if (!!(sta->flags & WL_STA_WME))
		wifi_status_set(wsta->sbitmap, WIFI_STATUS_WMM);

	if (!!(sta->flags & WL_STA_PS))
		wifi_status_set(wsta->sbitmap, WIFI_STATUS_PS);

	wsta->conn_time = sta->in;
	wsta->idle_time = sta->idle;

	/* Check tx/rx rspec */
	if (bcmwl_get_sta_info_rspec(&wsta->rx_rate, sta->rx_rspec))
		wsta->rx_rate.rate = sta->rx_rate / 1000;  /* in Mbps */

	if (bcmwl_get_sta_info_rspec(&wsta->tx_rate, sta->tx_rspec))
		wsta->tx_rate.rate = sta->tx_rate / 1000;  /* in Mbps */

	if (wsta->maxrate == 0) {
		if (sta->rateset_adv.count < WL_MAXRATES_IN_SET) {
			wsta->maxrate =
				(sta->rateset_adv.rates[sta->rateset_adv.count] & 0x7f) * 2;
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

int bcmwl_iface_link_measure(const char *ifname, uint8_t *sta)
{
	char buf[1024] = {0};
	int ret;


	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	memcpy(buf, sta, 6);
	ret = wl_iovar_set(ifname, "rrm_lm_req", sta, 6, buf, sizeof(buf));
	if (ret)
		libwifi_dbg("[%s] %s Failed\n", ifname, __func__);

	return ret;
}

int bcmwl_iface_restrict_sta(const char *ifname, uint8_t *sta, int enable)
{
	bool presp_mac_filter, is_list_empty;
	int macmode;
	int status;

	/* Add/remove STA to/from list */
	status = bcmwl_iface_update_maclist(ifname, sta, enable ? true : false);

	if (WARN_ON(status))
		return status;

	if (!enable) {
		/* Reject request to associate sent by stations on the list. */
		status = bcmwl_get_macmode(ifname, &macmode);
		if (WARN_ON(status))
				return status;

		if (macmode != WLC_MACMODE_DENY) {
			status = bcmwl_set_macmode(ifname, WLC_MACMODE_DENY);
			if (WARN_ON(status))
				return status;
		}

		/* Don't respond to Probe Requests sent by specified STA(s). */
		status = bcmwl_get_probresp_mac_filter(ifname, &presp_mac_filter);
		if (WARN_ON(status))
				return status;

		if (!presp_mac_filter) {
			status = bcmwl_set_probresp_mac_filter(ifname, true);
			if (WARN_ON(status))
				return status;
		}
	}

	/* Disable MAC address matching once the restrict list is empty */
	status = bcmwl_iface_is_maclist_empty(ifname, &is_list_empty);

	if (WARN_ON(status))
		return status;

	if (is_list_empty) {
		status = bcmwl_set_macmode(ifname, WLC_MACMODE_DISABLED);
		if (WARN_ON(status))
			return status;

		status = bcmwl_set_probresp_mac_filter(ifname, false);
		if (WARN_ON(status))
			return status;
	}

	return 0;
}

int bcmwl_iface_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	runCmd("wlctl -i %s sta_monitor %s " MACSTR,
	       ifname, cfg->enable ? "add" : "del", MAC2STR(sta));

	if (cfg->enable)
		runCmd("wlctl -i %s sta_monitor enable", ifname);

	return 0;
}

int bcmwl_iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *monsta)
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

int bcmwl_iface_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
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

int bcmwl_iface_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
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

int bcmwl_iface_get_4addr_parent(const char *ifname, char *parent)
{
	int ret;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	ret = wl_iovar_get_noparam(ifname, "wds_ap_ifname", parent, 16);
	if (WARN_ON(ret))
		return ret;

	return 0;
}

int bcmwl_radio_get_cac_methods(const char *name, uint32_t *methods)
{
	char dfs_status[4096] = {};

	*methods = BIT(WIFI_CAC_CONTINUOUS);

	if (!Cmd(dfs_status, sizeof(dfs_status), "wl -i %s dfs_ap_move", name))
		*methods |= BIT(WIFI_CAC_MIMO_REDUCED);

	return 0;
}

static int bcmwl_get_neighbor_list_static(const char *ifname,
					  struct nbr *nbrs,
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

int bcmwl_get_neighbor_list(const char *ifname, struct nbr *nbrs, int *nr)
{
	return bcmwl_get_neighbor_list_static(ifname, nbrs, nr);
}

int bcmwl_add_neighbor(const char *ifname, struct nbr *nbr, size_t len)
{
	char buf[2048] = {};
	char ssid[128];

	if (hostapd_cli_get_ssid(ifname, ssid, sizeof(ssid)))
		return -1;

	libwifi_dbg("[%s] %s called " MACSTR " chan %d opclass %d\n", ifname, __func__, MAC2STR(nbr->bssid), nbr->channel, nbr->reg);
	/* wl rrm_nbr_add_nbr <[bssid] [bssid info] [regulatory] [channel] [phytype]> [ssid] [chanspec] [prefence] */
	chrCmd(buf, sizeof(buf), "wl -i %s rrm_nbr_add_nbr " MACSTR " %u %u %u %u %s",
	       ifname, MAC2STR(nbr->bssid), nbr->bssid_info, nbr->reg, nbr->channel,
	       nbr->phy, ssid);

	return 0;
}

int bcmwl_del_neighbor(const char *ifname, unsigned char *bssid)
{
	char buf[2048] = {};

	libwifi_dbg("[%s] %s called " MACSTR "\n", ifname, __func__, MAC2STR(bssid));
	chrCmd(buf, sizeof(buf), "wl -i %s rrm_nbr_del_nbr " MACSTR, ifname, MAC2STR(bssid));

	return 0;
}
