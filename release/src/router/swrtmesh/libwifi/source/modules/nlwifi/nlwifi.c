/*
 * nlwifi.c - implements for cfg80211 wifi drivers
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
#include <unistd.h>
#include <stdarg.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <byteswap.h>
#include <fcntl.h>
#include <net/if.h>
#include <netlink/msg.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netpacket/packet.h>
#include <dirent.h>

#include <easy/easy.h>
#include "debug.h"
#include "wifi.h"
#include "wifiutils.h"
#include "wext.h"
#include "nlwifi.h"
#include "nl80211_copy.h"

#define C2S(x) case x: return #x;

struct radio_data {
	const char *phyname;
	enum wifi_band band;
	struct wifi_radio *radio;
};

struct caps_data {
	const char *phyname;
	enum wifi_band band;
	struct wifi_caps *caps;
};

struct channels_data {
	int num;
	int max;
	enum wifi_band band;
	uint32_t *chlist;
	struct chan_entry *chan_entry;
};

struct cmd_respdata {
	uint32_t attr;
	int len;
	void *data;
};

struct survey_entry {
	uint32_t freq;
	int8_t noise;

	uint64_t active_time;
	uint64_t busy_time;
	uint64_t busy_ext_time;
	uint64_t rx_time;
	uint64_t tx_time;
};

struct survey_data {
	int num;
	int max;
	struct survey_entry *survey_entry;
	enum wifi_band band;
};

struct mlo_links_data {
	const char *ifname;
	enum wifi_band band;
	int num;
	int max;
	struct wifi_mlo_link *link;
};

static int nlwifi_get_survey(const char *name, struct survey_data *survey_data);
static int nlwifi_get_phy(const char *name, char *phy, size_t phy_size);
static int nlwifi_get_supp_channels_cb(struct nl_msg *msg, void *data);
static int nlwifi_iface_mlo_link(const char *ifname, enum wifi_band band,
				 struct wifi_mlo_link *mlo);
static enum wifi_band ieee80211_frequency_to_band(int freq);

static enum wifi_band nlwifi_band_to_wifi_band(enum nl80211_band nlband)
{
	enum wifi_band band;

	switch (nlband) {
	case NL80211_BAND_2GHZ:
		band = BAND_2;
		break;
	case NL80211_BAND_5GHZ:
		band = BAND_5;
		break;
	case NL80211_BAND_6GHZ:
		band = BAND_6;
		break;
	default:
		band = BAND_UNKNOWN;
		break;
	}

	return band;
}

static enum wifi_bw nlwifi_width_to_wifi_bw(enum nl80211_chan_width nlwidth)
{
	switch (nlwidth) {
	case NL80211_CHAN_WIDTH_80:
		return BW80;
	case NL80211_CHAN_WIDTH_40:
		return BW40;
	case NL80211_CHAN_WIDTH_20:
	case NL80211_CHAN_WIDTH_20_NOHT:
		return BW20;
	case NL80211_CHAN_WIDTH_160:
		return BW160;
	case NL80211_CHAN_WIDTH_80P80:
		return BW8080;
	case NL80211_CHAN_WIDTH_320:
		return BW320;
	default:
		return BW_UNKNOWN;
	}
}

int nlwifi_get_phy_wifi_ifaces(const char *name, enum wifi_band band, struct wifi_iface* iface, uint8_t *num_iface)
{
	char path[256];
	char phy[16];
	char netdevphy[256] = {0};
	struct dirent *p;
	int ret = -1;
	DIR *d;
	size_t i = 0;
	uint8_t if_max = *num_iface;

	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);

	if (WARN_ON(nlwifi_get_phy(name, phy, sizeof(phy))))
		return -1;

	*num_iface = 0;
	if (if_max > WIFI_IFACE_MAX_NUM) 
		if_max = WIFI_IFACE_MAX_NUM;

	snprintf(path, sizeof(path), "/sys/class/ieee80211/%s/device/net", phy);
	if (WARN_ON(!(d = opendir(path))))
		return ret;

	while ((p = readdir(d)) && (i < if_max)) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;

		nlwifi_get_phyname(p->d_name, netdevphy);
		if (strcmp(phy, netdevphy))
			continue;

		iface[i].link_id = -1;

		nlwifi_iface_get_band(p->d_name, &iface[i].band);
		if (band != BAND_ANY && iface[i].band == BAND_UNKNOWN) {
			struct wifi_mlo_link link = {};

			if (WARN_ON(nlwifi_iface_mlo_link(p->d_name, band, &link)))
				continue;

			iface[i].link_id = link.id;
			iface[i].frequency = link.frequency;
			iface[i].band = ieee80211_frequency_to_band(link.frequency);

			libwifi_dbg("[%s, %s] %s fallback link id %d\n", name, wifi_band_to_str(link.band), __func__, link.id);
		}
		if (band && band != BAND_ANY && band != iface[i].band)
			continue;

		memset(iface[i].name, 0, 16);
		memcpy(iface[i].name, p->d_name, 15);
		nlwifi_get_mode(p->d_name, &iface[i].mode);
		ret = 0;
		i++;
		*num_iface=(uint8_t)i;
		continue;
	}
	closedir(d);
	return ret;
}

int nlwifi_phy_to_netdev(const char *phy, char *netdev, size_t size)
{
	return nlwifi_phy_to_netdev_with_type(phy, netdev, size, NLWIFI_MODE_ANY);
}

int nlwifi_phy_to_netdev_with_type_and_band(const char *phy, char *netdev, size_t size, uint32_t type, enum wifi_band band)
{
	char path[512];
	char rphy[256] = {};
	char netdevphy[256] = {0};
	struct dirent *p;
	bool found = false;
	int ret = -1;
	DIR *d;
	ifstatus_t ifstatus = 0;
	enum wifi_mode mode;
	enum wifi_band b;

	memset(netdev, 0, size);

	if (type == NLWIFI_MODE_ANY && band == BAND_ANY && !strstr(phy, "phy")) {
		/* Assume we get correct netdev */
		strncpy(netdev, phy, size);
		return 0;
	}

	/* Get real phy in case someone pass netdev */
        if (WARN_ON(nlwifi_get_phy(phy, rphy, sizeof(rphy))))
		return -1;

	ret = snprintf(path, sizeof(path), "/sys/class/ieee80211/%s/device/net", rphy);
	if (WARN_ON(ret >= (int)sizeof(path)))
		return -1;

	if (WARN_ON(!(d = opendir(path))))
		return ret;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (get_ifstatus(p->d_name, &ifstatus))
			continue;
		if(!(ifstatus & IFF_UP))
			continue;

		nlwifi_get_phyname(p->d_name, netdevphy);
		if (strcmp(rphy, netdevphy))
			continue;

		if (type == NLWIFI_MODE_ANY && band == BAND_ANY) {
			strncpy(netdev, p->d_name, size);
			found = true;
			break;
		}

		ret = nlwifi_get_mode(p->d_name, &mode);
		if (WARN_ON(ret))
			continue;

		ret = nlwifi_iface_get_band(p->d_name, &b);
		if (WARN_ON(ret))
			continue;
		if (band != BAND_ANY && b == BAND_UNKNOWN) {
			struct wifi_mlo_link link = {};

			if (WARN_ON(nlwifi_iface_mlo_link(p->d_name, band, &link)))
				continue;

			b = ieee80211_frequency_to_band(link.frequency);
			libwifi_dbg("[%s, %s] %s fallback link id %d\n", p->d_name, wifi_band_to_str(b), __func__, link.id);
		}

		if (band != BAND_ANY && b != band)
			continue;

		if (((type & NLWIFI_MODE_AP) && (mode == WIFI_MODE_AP)) ||
		    (((type & NLWIFI_MODE_STA) && (mode == WIFI_MODE_STA)))) {
			strncpy(netdev, p->d_name, size);
			found = true;
			break;
		}

		if (type == NLWIFI_MODE_ANY && b == band) {
			strncpy(netdev, p->d_name, size);
			found = true;
			break;
		}
	}
	closedir(d);

	ret = found ? 0 : -1;

	libwifi_dbg("[%s, %s] %s type %d band %s ret %d [%s]\n", phy, rphy, __func__, type, wifi_band_to_str(band), ret, netdev);
	return ret;
}

int nlwifi_phy_to_netdev_with_type(const char *phy, char *netdev, size_t size, uint32_t type)
{
        return nlwifi_phy_to_netdev_with_type_and_band(phy, netdev, size, type, BAND_ANY);
}

int nlwifi_phy_to_netdev_with_band(const char *phy, char *netdev, size_t size, enum wifi_band band)
{
        return nlwifi_phy_to_netdev_with_type_and_band(phy, netdev, size, NLWIFI_MODE_ANY, band);
}

static int nlwifi_get_phy(const char *name, char *phy, size_t phy_size)
{
	int fd;
	char path[256];
	char buf[512];
	int index;

	if (strstr(name, "phy")) {
		strncpy(phy, name, phy_size);
		return 0;
	}

	snprintf(path, sizeof(path), "/sys/class/net/%s/phy80211/index", name);
	fd = open(path, O_RDONLY);
	if (WARN_ON(fd < 0))
		return -1;

	if (!read(fd, buf, sizeof(buf))) {
		close(fd);
		return -1;
	}

	if (sscanf(buf, "%d", &index) != 1) {
		close(fd);
		return -1;
	}

	close(fd);
	snprintf(phy, phy_size, "phy%d", index);
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

static enum wifi_band ieee80211_frequency_to_band(int freq)
{
	enum wifi_band band;

	if (freq > 2407 && freq < 5000)
		band = BAND_2;
	else if (freq > 5000 && freq < 5950)
		band = BAND_5;
	else if (freq > 5950)
		band = BAND_6;
	else
		band = BAND_UNKNOWN;

	return band;
}

static enum wifi_mode ieee80211_type_to_wifi_mode(enum nl80211_iftype type)
{
	if (type == NL80211_IFTYPE_STATION)
		return WIFI_MODE_STA;
	else if (type == NL80211_IFTYPE_AP)
		return WIFI_MODE_AP;
	else if (type == NL80211_IFTYPE_AP_VLAN)
		return WIFI_MODE_AP_VLAN;
	else if (type == NL80211_IFTYPE_MONITOR)
		return WIFI_MODE_MONITOR;
	else
		return WIFI_MODE_UNKNOWN;
}

static char *dfs_state_name(enum nl80211_dfs_state state)
{
	switch (state) {
	C2S(NL80211_DFS_USABLE)
	C2S(NL80211_DFS_AVAILABLE)
	C2S(NL80211_DFS_UNAVAILABLE)
	default:
		return "unknown";
	}
}

static int phy_nametoindex(const char *name)
{
	char buf[200];
	int fd, pos;

	snprintf(buf, sizeof(buf), "/sys/class/ieee80211/%s/index", name);

	fd = open(buf, O_RDONLY);
	if (fd < 0)
		return -1;
	pos = read(fd, buf, sizeof(buf) - 1);
	if (pos < 0) {
		close(fd);
		return -1;
	}
	buf[pos] = '\0';
	close(fd);
	return atoi(buf);
}

struct nl_sock *nlwifi_socket(void)
{
	struct nl_sock *nl;

	nl = nl_socket_alloc();
	if (nl == NULL)
		return NULL;

	genl_connect(nl);

	return nl;
}

void *nlwifi_alloc_msg(struct nl_sock *sk, int cmd, int flags, size_t priv)
{
	struct nl_msg *msg;
	int id;

	msg = nlmsg_alloc();
	if (!msg)
		return NULL;

	id = genl_ctrl_resolve(sk, "nl80211");
	if (id < 0) {
		nlmsg_free(msg);
		return NULL;
	}

	genlmsg_put(msg, NL_AUTO_PORT, NL_AUTO_SEQ, id, priv, flags, (uint8_t)cmd, 0);

	return msg;
}

static int nlwifi_ack_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;

	*ret = 0;
	return NL_STOP;
}

static int nlwifi_finish_handler(struct nl_msg *msg, void *arg)
{
	int *ret = arg;

	*ret = 0;
	return NL_SKIP;
}

static int nlwifi_err_handler(struct sockaddr_nl *nla, struct nlmsgerr *err,
								void *arg)
{
	int *ret = arg;

	*ret = err->error;
	libwifi_err("%s: %s\n", __func__, strerror(-err->error));
	return NL_STOP;
}

static int nlwifi_no_seq_check(struct nl_msg *msg, void *arg)
{
	return NL_OK;
}

int nlwifi_send_msg(struct nl_sock *nl, struct nl_msg *msg,
			int (*cmd_resp_handler)(struct nl_msg *, void *),
			void *cmd_resp_data)
{
	struct nl_cb *cb;
	int err = 1;

	if (!msg)
		return -1;

	cb = nl_cb_alloc(NL_CB_DEFAULT);
	if (!cb)
		return -1;

	nl_socket_set_cb(nl, cb);

	nl_cb_err(cb, NL_CB_CUSTOM, nlwifi_err_handler, &err);
	nl_cb_set(cb, NL_CB_FINISH, NL_CB_CUSTOM, nlwifi_finish_handler, &err);
	nl_cb_set(cb, NL_CB_ACK, NL_CB_CUSTOM, nlwifi_ack_handler, &err);
	nl_cb_set(cb, NL_CB_SEQ_CHECK, NL_CB_CUSTOM, nlwifi_no_seq_check, NULL);

	if (cmd_resp_handler)
		nl_cb_set(cb, NL_CB_VALID, NL_CB_CUSTOM, cmd_resp_handler, cmd_resp_data);


	err = nl_send_auto(nl, msg);
	if (err < 0)
		goto out;

	while(err > 0) {
		if (nl_recvmsgs(nl, cb) < 0) {
			libwifi_dbg("nl_recvmsgs failed\n");
			break;
		}
	}
out:
	nl_cb_put(cb);
	return err;
}

struct nlwifi_ctx {
	enum nl80211_commands cmd;
	int flags;

	/* cmd callback and data */
	int (*cb)(struct nl_msg *msg, void *data);
	void *data;
};

static int nlwifi_cmd(const char *name, struct nlwifi_ctx *ctx)
{
	struct nl_sock *nl;
	struct nl_msg *msg;
	int devidx;
	int ret = -1;

	if (strstr(name, "phy")) {
		devidx = phy_nametoindex(name);
		if (devidx < 0)
			goto out;
	} else {
		devidx = if_nametoindex(name);
		if (devidx == 0) {
			ret = -errno;
			goto out;
		}
	}

	nl = nlwifi_socket();
	if (!nl)
		goto out;

	msg = nlwifi_alloc_msg(nl, ctx->cmd, ctx->flags, 0);
	if (!msg)
		goto out_msg_failure;

	if (ctx->flags == NLM_F_DUMP && ctx->cmd == NL80211_CMD_GET_WIPHY) {
		nla_put_flag(msg, NL80211_ATTR_SPLIT_WIPHY_DUMP);
		nlmsg_hdr(msg)->nlmsg_flags |= NLM_F_DUMP;
	}

	if (strstr(name, "phy"))
		NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, devidx);
	else
		NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);

	ret = nlwifi_send_msg(nl, msg, ctx->cb, ctx->data);

nla_put_failure:
	nlmsg_free(msg);
out_msg_failure:
	nl_socket_free(nl);
out:
	return ret;
}

static int nlwifi_get_phyname_cb(struct nl_msg *msg, void *data)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	char *phyname = data;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (tb[NL80211_ATTR_WIPHY_NAME]) {
		strncpy(phyname, nla_get_string(tb[NL80211_ATTR_WIPHY_NAME]), 16);
	}

	return NL_SKIP;
}

int nlwifi_get_phyname(const char *ifname, char *phyname)
{
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.cb = nlwifi_get_phyname_cb,
		.data = phyname,
	};

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	return nlwifi_cmd(ifname, &ctx);
}

int nlwifi_scan(const char *ifname, struct scan_param *p)
{
	struct nl_sock *nl;
	struct nl_msg *msg;
	int devidx;
	int ret = -1;

	devidx = if_nametoindex(ifname);
	if (!devidx) {
		ret = -errno;
		goto out;
	}

	nl = nlwifi_socket();
	if (!nl)
		goto out;

	msg = nlwifi_alloc_msg(nl, NL80211_CMD_TRIGGER_SCAN, 0, 0);
	if (!msg)
		goto out_msg_failure;

	/* prepare cmd */
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);
	if (p && p->ssid[0]) {
		struct nlattr *nest;

		nest = nla_nest_start(msg, NL80211_ATTR_SCAN_SSIDS);
		if (!nest)
			goto nla_put_failure;

		libwifi_dbg("SSID to scan: '%s'  (len = %u)\n",
					p->ssid, strlen(p->ssid));
		NLA_PUT(msg, 1, strlen(p->ssid), p->ssid);
		nla_nest_end(msg, nest);
	}

	if (p && p->opclass) {
		uint32_t channels[32] = {0};
		struct nlattr *nest;
		int num = 32;
		int i;

		ret = wifi_opclass_to_channels(p->opclass, &num, channels);
		if (ret) {
			libwifi_err("Invalid opclass\n");
			goto nla_put_failure;
		}


		nest = nla_nest_start(msg, NL80211_ATTR_SCAN_FREQUENCIES);
		if (!nest)
			goto nla_put_failure;

		for (i = 0; i < num; i++) {
			int freq;

			freq = wifi_channel_to_freq(channels[i]);
			if (freq > 0) {
				libwifi_dbg("Channel to scan: '%d' (center-freq = %d)\n",
					    channels[i], freq);
				NLA_PUT_U32(msg, i + 1, freq);
			}
		}
		nla_nest_end(msg, nest);

	} else if (p && p->channel > 0) {
		struct nlattr *nest;
		int freq;

		nest = nla_nest_start(msg, NL80211_ATTR_SCAN_FREQUENCIES);
		if (!nest)
			goto nla_put_failure;

		freq = wifi_channel_to_freq(p->channel);
		if (freq > 0) {
			libwifi_dbg("Channel to scan: '%d' (center-freq = %d)\n",
				    p->channel, freq);
			NLA_PUT_U32(msg, 1, freq);
		}
		nla_nest_end(msg, nest);
	}

	/* Force scan for AP iface */
	NLA_PUT_U32(msg, NL80211_ATTR_SCAN_FLAGS, NL80211_SCAN_FLAG_AP);

	/* send cmd */
	ret = nlwifi_send_msg(nl, msg, NULL, NULL);
	nlmsg_free(msg);
	nl_socket_free(nl);
	return ret;

nla_put_failure:
	nlmsg_free(msg);
out_msg_failure:
	nl_socket_free(nl);
out:
	return ret;
}

int nlwifi_scan_ex(const char *ifname, struct scan_param_ex *sp)
{
	struct nl_sock *nl;
	struct nl_msg *msg;
	int devidx;
	uint32_t scan_flags = 0;
	int ret = -1;

	devidx = if_nametoindex(ifname);
	if (!devidx) {
		ret = -errno;
		goto out;
	}

	nl = nlwifi_socket();
	if (!nl)
		goto out;

	msg = nlwifi_alloc_msg(nl, NL80211_CMD_TRIGGER_SCAN, 0, 0);
	if (!msg)
		goto out_msg_failure;

	/* prepare cmd */
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);

	/* SSID list */
	if (sp && sp->num_ssid && sp->flag & WIFI_SCAN_REQ_SSID) {
		struct nlattr *nest;
		int i;

		if (sp->num_ssid > WIFI_SCAN_MAX_SSIDS)
			goto nla_put_failure;

		nest = nla_nest_start(msg, NL80211_ATTR_SCAN_SSIDS);
		if (!nest)
			goto nla_put_failure;

		for (i = 0; i < sp->num_ssid; i++) {
			libwifi_dbg("SSID to scan: '%s'  (len = %u)\n",
						sp->ssid[i], strlen(sp->ssid[i]));
			NLA_PUT(msg, 1, strlen(sp->ssid[i]), sp->ssid[i]);
		}

		nla_nest_end(msg, nest);
	}

	/* Frequency list */
	if (sp && sp->num_freq) {
		struct nlattr *nest;
		int i;

		nest = nla_nest_start(msg, NL80211_ATTR_SCAN_FREQUENCIES);
		if (!nest)
			goto nla_put_failure;

		for (i = 0; i < sp->num_freq; i++) {

			if (sp->freq[i] > 0) {
				libwifi_dbg("Frequency to scan: '%d'\n",
					    sp->freq[i]);
				NLA_PUT_U32(msg, i + 1, sp->freq[i]);
			}
		}
		nla_nest_end(msg, nest);
	}

	/* Flags */
	scan_flags |= NL80211_SCAN_FLAG_AP;
	if (sp->flush)
		scan_flags |= NL80211_SCAN_FLAG_FLUSH;

	if (scan_flags)
		NLA_PUT_U32(msg, NL80211_ATTR_SCAN_FLAGS, scan_flags);

	/* send cmd */
	ret = nlwifi_send_msg(nl, msg, NULL, NULL);

nla_put_failure:
	nlmsg_free(msg);
out_msg_failure:
	nl_socket_free(nl);
out:
	return ret;
}

UNUSED_FUNC static void nlwifi_parse_wpa_ie(uint8_t *iebuf, size_t len, int *nxt,
			uint32_t *auth, uint32_t *p_cipher, uint32_t *g_cipher)
{
	int ielen = iebuf[1] + 2;
	int offset = 2;
	unsigned char wpa1_oui[3] = {0x00, 0x50, 0xf2};
	unsigned char wpa2_oui[3] = {0x00, 0x0f, 0xac};
	unsigned char *wpa_oui;
	uint16_t cnt = 0;
	int i;

#ifndef bit
#define bit(_x)		(1 << (_x))
#endif

	if (ielen > len)
		ielen = len;

	switch (iebuf[0]) {
	case 0x30:
		if (ielen < 4)
			return;
		wpa_oui = wpa2_oui;
		*auth |= WPA_VERSION2;
		break;
	case 0xdd:
		wpa_oui = wpa1_oui;
		*auth |= WPA_VERSION1;
		if ((ielen < 8)
			|| (memcmp(&iebuf[offset], wpa_oui, 3) != 0)
			|| (iebuf[offset + 3] != 0x01)) {
			return;
		}
		offset += 4;
		break;
	default:
		return;
	}

	/* increment for next suites */
	(*nxt)++;
	offset += 2;

	if (ielen < (offset + 4)) {
		*auth = bit(WIFI_AKM_PSK);

		/* Short ie - assume tkip/tkip */
		*p_cipher |= bit(WIFI_CIPHER_TKIP);
		*g_cipher |= bit(WIFI_CIPHER_TKIP);
		return;
	}

	/* group cipher */
	if (!memcmp(&iebuf[offset], wpa_oui, 3)) {
		switch (iebuf[offset + 3]) {
			case WIFI_CIPHER_WEP40:
			case WIFI_CIPHER_WEP104:
			case WIFI_CIPHER_TKIP:
			case WIFI_CIPHER_CCMP:
				*g_cipher = bit(iebuf[offset + 3]);
				break;
			default:
				break;
		}
	}
	offset += 4;
	if (ielen < (offset + 2)) {
		/* no pairwise cipher or auth method.. assume tkip */
		*p_cipher = bit(WIFI_CIPHER_TKIP);
		*auth = bit(WIFI_AKM_PSK);
		return;
	}

	/* pairwise ciphers */
	cnt = (uint16_t)(iebuf[offset] | (iebuf[offset + 1] << 8));
	offset += 2;

	if (ielen < (offset + 4 * cnt))
		return;

	for (i = 0; i < cnt; i++) {
		if(!memcmp(&iebuf[offset], wpa_oui, 3)) {
			if (iebuf[offset+3] == WIFI_CIPHER_WEP40)
				*p_cipher = WIFI_CIPHER_WEP40;
			else if(iebuf[offset+3] == WIFI_CIPHER_WEP104)
				*p_cipher = WIFI_CIPHER_WEP104;
			else if(iebuf[offset+3] == WIFI_CIPHER_TKIP)
				*p_cipher = WIFI_CIPHER_TKIP;
			else if(iebuf[offset+3] == WIFI_CIPHER_CCMP)
				*p_cipher = WIFI_CIPHER_CCMP;
		}
		offset += 4;
	}

	if (ielen < (offset + 2))
		return;

	/* authentication suites */
	cnt = (uint16_t)(iebuf[offset] | (iebuf[offset + 1] << 8));
	offset += 2;

	if (ielen < (offset + 4 * cnt))
		return;

	for (i = 0; i < cnt; i++) {
		if (!memcmp(&iebuf[offset], wpa_oui, 3)) {
			switch (iebuf[offset + 3]) {
			case WIFI_AKM_DOT1X:
			case WIFI_AKM_PSK:
			case WIFI_AKM_FT_DOT1X:
			case WIFI_AKM_FT_PSK:
				*auth |= bit(iebuf[offset + 3]);
				break;
			default:
				break;
			}
		}
		offset += 4;
	}

	if (ielen < (offset + 1))
		return;
}

static int nlwifi_get_ie(uint8_t *ies, size_t len, uint8_t e, void *data)
{
	int offset = 0;
	int ielen = 0;
	uint8_t eid;

	while (offset <= (len - 2)) {
		eid = ies[offset];
		ielen = ies[offset + 1];
		if (eid != e) {
			offset += ielen + 2;
			continue;
		}

		switch (eid) {
		case IE_BSS_LOAD:
			if (ielen > sizeof(struct wifi_ap_load))
				return -1;
			break;
		case IE_EXT_CAP:
			if (ielen > sizeof(struct wifi_caps_ext))
				return -1;
			break;
		case IE_HT_CAP:
			if (ielen > sizeof(struct wifi_caps_ht))
				return -1;
			break;
		case IE_VHT_CAP:
			if (ielen > sizeof(struct wifi_caps_vht))
				return -1;
			break;
		case IE_RRM:
			if (ielen > sizeof(struct wifi_caps_rm))
				return -1;
			break;
		default:
			break;
		}

		memcpy((uint8_t *)data, &ies[offset + 2], ielen);
		return 0;
	}

	return -ENOENT;
}

static int nlwifi_get_bandwidth_from_ie(uint8_t *ies, size_t len,
						enum wifi_bw *bw)
{
	int offset = 0;
	int ielen = 0;
	uint8_t *p = NULL;
	int cntr = 0;

#ifndef bit
#define bit(n)	(1 << (n))
#endif

	while (offset <= (len - 2)) {
		ielen = ies[offset + 1];
		p = &ies[offset + 2];

		switch (ies[offset]) {
		case 61:   /* HT oper */
			{
				uint8_t *ht_op;
				uint8_t off = 0;

				if (ielen != 22)
					return -1;

				ht_op = p + 1;
				off = ht_op[0] & 0x3;

				if (off == 0)
					*bw = BW20;
				else if (off == 1 || off == 3)
					*bw = BW40;

				if ((ht_op[0] & bit(2)) == 0)
					*bw = BW20;
				cntr++;
			}
			break;
		case 192:  /* VHT oper */
			{
				uint8_t *vht_op;
				uint8_t w, cfs0, cfs1;

				if (ielen < 5)
					return -1;

				vht_op = p;
				w = vht_op[0];
				cfs0 = vht_op[1];
				cfs1 = vht_op[2];

				if (w == 0)
					break;

				if (cfs1 == 0) {
					*bw = BW80;
				} else {
					if (cfs1 - cfs0 == 8)
						*bw = BW160;
					else if (cfs1 - cfs0 == 16)
						*bw = BW8080;
				}
				cntr++;
			}
			break;
		default:
			break;
		}

		if (cntr == 2)
			break;

		offset += ies[offset + 1] + 2;
	}
	return 0;
}

static int nlwifi_get_iface_cb(struct nl_msg *msg, void *data)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct cmd_respdata *res = data;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_IFNAME])
		return NL_SKIP;

	if (!tb[res->attr])
		return NL_SKIP;

	/* libwifi_dbg("res->attr = %u\n", res->attr); */
	switch (res->attr) {
	case NL80211_ATTR_CHANNEL_WIDTH:
		{
			uint32_t bw;

			bw = nla_get_u32(tb[NL80211_ATTR_CHANNEL_WIDTH]);
			*((uint32_t *)res->data) = bw;
		}
		break;
	case NL80211_ATTR_WIPHY_FREQ:
		{
			uint32_t freq;

			freq = nla_get_u32(tb[NL80211_ATTR_WIPHY_FREQ]);
			*((uint32_t *)res->data) = freq;
		}
		break;
	case NL80211_ATTR_IFTYPE:
		{
			uint32_t iftype;

			iftype = nla_get_u32(tb[NL80211_ATTR_IFTYPE]);
			*((uint32_t *)res->data) = iftype;
		}
		break;
	case NL80211_ATTR_CENTER_FREQ1:
		{
			uint32_t center1;

			center1 = nla_get_u32(tb[NL80211_ATTR_CENTER_FREQ1]);
			*((uint32_t *)res->data) = center1;
		}
		break;
	default:
		/* libwifi_dbg("res->attr = default\n"); */
		res->len = nla_len(tb[res->attr]);
		memcpy(res->data, nla_data(tb[res->attr]), res->len);
		break;
	}

	return NL_SKIP;
}

int nlwifi_get_ssid(const char *ifname, char *ssid)
{
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_SSID,
		.len = 0,
		.data = ssid,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_iface_cb,
		.data = &res,
	};

	return nlwifi_cmd(ifname, &ctx);
}

int nlwifi_get_bssid(const char *ifname, uint8_t *bssid)
{
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_MAC,
		.len = 0,
		.data = bssid,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_iface_cb,
		.data = &res,
	};

	return nlwifi_cmd(ifname, &ctx);
}

int nlwifi_get_freq_from_survey(const char *ifname, uint32_t *freq)
{
	struct survey_entry survey = {};
	struct survey_data survey_data = {
		.survey_entry = &survey,
		.num = 0,
		.max = 1,
	};
	char netdev[16] = {};

	nlwifi_phy_to_netdev(ifname, netdev, sizeof(netdev));
	if (WARN_ON(nlwifi_get_survey(netdev, &survey_data)))
		return -1;

	*freq = survey.freq;

	libwifi_dbg("[%s] freq from survey %u\n", ifname, *freq);
	return 0;
}

static int nlwifi_iface_mlo_link(const char *ifname, enum wifi_band band,
				 struct wifi_mlo_link *mlo)
{
	struct wifi_mlo_link mlo_link[8];
	int mlo_link_num = ARRAY_SIZE(mlo_link);
	int ret;

	ret = nlwifi_get_mlo_links(ifname, band, mlo_link, &mlo_link_num);
	if (WARN_ON(ret))
		return ret;

	if (WARN_ON(mlo_link_num == 0))
		return -1;

	*mlo = mlo_link[0];
	libwifi_dbg("[%s, %s] mlo link id %d freq %u\n", ifname, wifi_band_to_str(band), mlo->id, mlo->frequency);
	return 0;
}

int nlwifi_get_channel_freq(const char *ifname, uint32_t *control_freq)
{
	uint32_t freq = 0;
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_WIPHY_FREQ,
		.len = sizeof(freq),
		.data = &freq,
	};
	int ret;

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_iface_cb,
		.data = &res,
	};

	ret = nlwifi_cmd(ifname, &ctx);

	*control_freq = freq;

	libwifi_dbg("[%s] freq = %u\n", ifname, *control_freq);
	return ret;
}

int nlwifi_iface_get_band(const char *netdev, enum wifi_band *band)
{
	uint32_t freq;
	int ret;

	ret = nlwifi_get_channel_freq(netdev, &freq);
	if (WARN_ON(ret))
		return ret;

	*band = ieee80211_frequency_to_band(freq);
	return ret;
}

static int nlwifi_get_mlo_links_cb(struct nl_msg *msg, void *data)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct mlo_links_data *links = data;
	struct wifi_mlo_link *l;
	struct nlattr *link;
	int n;

	libwifi_err("[%s] %s called\n", links->ifname, __func__);
	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	links->num = 0;
	if (!tb[NL80211_ATTR_IFNAME])
		return NL_SKIP;

	if (!tb[NL80211_ATTR_MLO_LINKS])
		return NL_SKIP;

	nla_for_each_nested(link, tb[NL80211_ATTR_MLO_LINKS], n) {
		struct nlattr *tb_msg[NL80211_ATTR_MAX + 1];
		uint32_t freq;
		uint32_t width;

		nla_parse_nested(tb_msg, NL80211_ATTR_MAX, link, NULL);
		if (!tb_msg[NL80211_ATTR_MLO_LINK_ID] &&
		    !tb_msg[NL80211_ATTR_MAC] &&
		    !tb_msg[NL80211_ATTR_WIPHY_FREQ])
			continue;

		if (links->num >= links->max)
			break;

		l = &links->link[links->num];
		freq = nla_get_u32(tb_msg[NL80211_ATTR_WIPHY_FREQ]);

		if (tb_msg[NL80211_ATTR_CHANNEL_WIDTH])
			width = nla_get_u32(tb_msg[NL80211_ATTR_CHANNEL_WIDTH]);
		else
			width = 20;

		if (tb[NL80211_ATTR_SSID])
			memcpy(l->ssid, nla_data(tb[NL80211_ATTR_SSID]), nla_len(tb[NL80211_ATTR_SSID]));

		l->id =  nla_get_u32(tb_msg[NL80211_ATTR_MLO_LINK_ID]);
		l->frequency = freq;
		l->channel = wifi_freq_to_channel(freq);
		l->band = ieee80211_frequency_to_band(freq);
		l->bandwidth = nlwifi_width_to_wifi_bw(width);
		memcpy(l->macaddr, nla_data(tb_msg[NL80211_ATTR_MAC]), 6);

		/* Check if specyfic band requested */
		if (links->band != BAND_ANY && links->band != l->band) {
			libwifi_dbg("[%s] %s skip link id %u (%u vs %u)\n", links->ifname,
				    __func__, l->id, links->band, l->band);
			continue;
		}

		links->num++;
	}

	return NL_SKIP;
}

int nlwifi_get_mlo_links(const char *ifname, enum wifi_band band,
			 struct wifi_mlo_link *link, int *num)
{
	struct mlo_links_data data = {
		.ifname = ifname,
		.band = band,
		.max = *num,
		.link = link,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_mlo_links_cb,
		.data = &data,
	};
	int ret;

	ret = nlwifi_cmd(ifname, &ctx);
	if (WARN_ON(ret))
		return ret;

	*num = data.num;

	libwifi_dbg("[%s] %s mlo links = %u\n", ifname, __func__, *num);
	return ret;
}

int nlwifi_get_channel_center_freq1(const char *ifname, uint32_t *center_freq1)
{
	uint32_t center1 = 0;
	int ret;
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_CENTER_FREQ1,
		.len = 0,
		.data = &center1,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_iface_cb,
		.data = &res,
	};

	ret = nlwifi_cmd(ifname, &ctx);
	*center_freq1 = center1;

	libwifi_dbg("[%s] freq = %u\n", ifname, *center_freq1);

	return ret;
}

int nlwifi_get_netdev_freq(const char *netdev, uint32_t *control_freq)
{
	uint32_t freq = 0;

	/* First check netdev */
	if (!nlwifi_get_channel_freq(netdev, &freq) && freq) {
		*control_freq = freq;
		return 0;
	}

	/* Next check 'in use' survey */
	return nlwifi_get_freq_from_survey(netdev, control_freq);
}


int nlwifi_get_phy_freq(const char *phy, uint32_t *freq)
{
	struct chan_entry channel[64] = {};
	int num = ARRAY_SIZE(channel);
	struct channels_data data = {
		.chan_entry = channel,
		.chlist = NULL,
		.num = 0,
		.max = num,
	};
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_supp_channels_cb,
		.data = &data,
	};

	libwifi_dbg("[%s] %s called (max: %d)\n", phy, __func__, num);
	if (nlwifi_cmd(phy, &ctx))
		return -1;

	*freq = channel[0].freq;
	libwifi_dbg("[%s] %s freq = %u\n", phy, __func__, *freq);

	return 0;
}

int nlwifi_get_frequency(const char *name, uint32_t *freq)
{
	char netdev[16] = {};
	int ret = -1;

	/* Check any netdev */
	if (!nlwifi_phy_to_netdev(name, netdev, sizeof(netdev)))
		ret = nlwifi_get_netdev_freq(netdev, freq);

	if (ret || *freq == 0)
		ret = nlwifi_get_phy_freq(name, freq);

	return ret;
}


int nlwifi_get_chan(const char *ifname, uint32_t *channel)
{
	uint32_t freq;
	int ret;

	ret = nlwifi_get_frequency(ifname, &freq);
	if (ret)
		return ret;

	*channel = ieee80211_frequency_to_channel(freq);
	libwifi_dbg("[%s] get_chan freq = %u chan %u\n", ifname, freq, *channel);

	return 0;
}

static int nlwifi_get_supp_channels_cb(struct nl_msg *msg, void *data)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *tb_band[NL80211_BAND_ATTR_MAX + 1];
	struct nlattr *tb_freq[NL80211_FREQUENCY_ATTR_MAX + 1];
	struct nlattr *nl_band, *nl_freq;
	struct channels_data *channels_data = data;
	enum nl80211_band nlband;
	enum wifi_band band;
	int iter_band;
	int iter_freq;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_WIPHY_BANDS])
		return NL_SKIP;

	nla_for_each_nested(nl_band, tb[NL80211_ATTR_WIPHY_BANDS], iter_band) {
		nla_parse(tb_band,
			NL80211_BAND_ATTR_MAX,
			nla_data(nl_band),
			nla_len(nl_band),
			NULL);

		if (!tb_band[NL80211_BAND_ATTR_FREQS])
			continue;

		nlband = nl_band->nla_type;
		switch (nlband) {
		case NL80211_BAND_2GHZ:
			band = BAND_2;
			break;
		case NL80211_BAND_5GHZ:
			band = BAND_5;
			break;
		case NL80211_BAND_6GHZ:
			band = BAND_6;
			break;
		default:
			band = BAND_UNKNOWN;
			break;
		}

		if (channels_data->band && channels_data->band != BAND_ANY && channels_data->band != band)
			continue;

		nla_for_each_nested(nl_freq, tb_band[NL80211_BAND_ATTR_FREQS],
							iter_freq) {
			uint32_t freq;
			int ch;

			nla_parse(tb_freq,
				NL80211_FREQUENCY_ATTR_MAX,
				nla_data(nl_freq),
				nla_len(nl_freq),
				NULL);

			if (!tb_freq[NL80211_FREQUENCY_ATTR_FREQ])
				continue;

			if (tb_freq[NL80211_FREQUENCY_ATTR_DISABLED])
				continue;

			freq = nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_FREQ]);
			ch = wifi_freq_to_channel(freq);
			libwifi_dbg(" - freq %d MHz  (Ch = %d) band %d\n", freq, ch, band);

			if (channels_data->band == BAND_2 && freq > 2484)
				continue;

			if (channels_data->band == BAND_5 && freq <= 2484)
				continue;

			if (WARN_ON(channels_data->num >= channels_data->max))
				break;

			if (channels_data->chlist)
				channels_data->chlist[channels_data->num] = ch;

			if (channels_data->chan_entry) {
				struct chan_entry *entry;
				enum nl80211_dfs_state state = NL80211_DFS_UNAVAILABLE;

				entry = &channels_data->chan_entry[channels_data->num];

				entry->channel = ch;
				entry->freq = freq;
				entry->band = band;

				/* noise will be updated from survey */
				entry->noise = -90;

				if (tb_freq[NL80211_FREQUENCY_ATTR_RADAR]) {
					libwifi_dbg(" - radar detection required\n");
					entry->dfs = true;
				}

				if (tb_freq[NL80211_FREQUENCY_ATTR_DFS_STATE]) {
					state = nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_DFS_STATE]);
					libwifi_dbg(" - dfs state %s\n", dfs_state_name(state));
					switch (state) {
					case NL80211_DFS_USABLE:
						entry->dfs_state = WIFI_DFS_STATE_USABLE;
						break;
					case NL80211_DFS_AVAILABLE:
						entry->dfs_state = WIFI_DFS_STATE_AVAILABLE;
						break;
					case NL80211_DFS_UNAVAILABLE:
						entry->dfs_state = WIFI_DFS_STATE_UNAVAILABLE;
						break;
					default:
						break;
					}
				}

				if (tb_freq[NL80211_FREQUENCY_ATTR_DFS_CAC_TIME]) {
					entry->cac_time = nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_DFS_CAC_TIME]);
					entry->cac_time /= 1000;
				}

				if (tb_freq[NL80211_FREQUENCY_ATTR_DFS_TIME] && state == NL80211_DFS_UNAVAILABLE) {
					entry->nop_time = nla_get_u32(tb_freq[NL80211_FREQUENCY_ATTR_DFS_TIME]) / 1000;
					entry->nop_time = 30 * 60 - entry->nop_time;
				}
			}

			channels_data->num++;
		}
	}

	return NL_SKIP;
}

int nlwifi_get_supp_channels(const char *name, uint32_t *chlist, int *num,
					const char *cc,
					enum wifi_band band,
					enum wifi_bw bw)
{
	struct channels_data data = {
		.chlist = chlist,
		.num = 0,
		.max = *num,
		.band = band,
	};
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_supp_channels_cb,
		.data = &data,
	};

	int ret;

	libwifi_dbg("[%s] %s called (max %d band %s)\n", name, __func__, *num, wifi_band_to_str(band));

	ret = nlwifi_cmd(name, &ctx);
	if (ret)
		return ret;

	*num = data.num;

	return 0;
}

int nlwifi_get_bandwidth(const char *ifname, enum wifi_bw *bandwidth)
{
	int ret = -1;
	uint32_t bw = NL80211_CHAN_WIDTH_20_NOHT;
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_CHANNEL_WIDTH,
		.len = sizeof(bw),
		.data = &bw,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_iface_cb,
		.data = &res,
	};

	*bandwidth = BW_UNKNOWN;

	ret = nlwifi_cmd(ifname, &ctx);
	if (ret)
		return ret;

	*bandwidth = nlwifi_width_to_wifi_bw(bw);
	libwifi_dbg("nlwifi bandwidth = %s\n", wifi_bw_to_str(*bandwidth));

	return 0;
}

static uint32_t nlwifi_default_channel(enum wifi_band band)
{
	switch (band) {
	case BAND_2:
		return 1;
	case BAND_5:
		return 36;
	case BAND_6:
		return 1;
	default:
		return 0;
	}
}

int nlwifi_get_band_channel(const char *phyname, enum wifi_band band,
			    uint32_t *channel, enum wifi_bw *bw)
{
	uint32_t freq;
	char netdev[16];
	int ret;

	*channel = 0;
	*bw = BW20;

	if (nlwifi_phy_to_netdev_with_band(phyname, netdev, sizeof(netdev), band)) {
		/* No netdev for this band */
		*bw = BW20;
		*channel = nlwifi_default_channel(band);
		return 0;
	}

	ret = nlwifi_get_channel_freq(netdev, &freq);
	if (WARN_ON(ret))
		return ret;

	if (freq == 0) {
		struct wifi_mlo_link mlo = {};

		ret = nlwifi_iface_mlo_link(netdev, band, &mlo);
		if (WARN_ON(ret))
			return ret;

		freq = mlo.frequency;
		*bw = mlo.bandwidth;
	} else {
		nlwifi_get_bandwidth(netdev, bw);
	}

	*channel = wifi_freq_to_channel(freq);

	return ret;
}

int nlwifi_get_channel(const char *name, uint32_t *channel,
		       enum wifi_bw *bw)
{
	enum wifi_band req_band=BAND_DUAL;
	enum wifi_bw req_bw=BW20;
	uint32_t chlist[128] = {};
	int num = 128;
	char req_cc[]="DE";
	char netdev[16];
	int ret;

	*channel = 0;
	*bw = 0;

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	ret = nlwifi_get_chan(name, channel);
	if (ret || *channel == 0) {
		libwifi_err("nlwifi_get_chan(%s) failed %d fallback\n", netdev, ret);
		if (nlwifi_get_supp_channels(name, chlist, &num, req_cc, req_band, req_bw))
			return -1;
		*channel = chlist[0];
	}

	nlwifi_get_bandwidth(netdev, bw);
	return ret;
}

int nlwifi_get_mode(const char *ifname, enum wifi_mode *mode)
{
	uint32_t nl_iftype = 0;
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_IFTYPE,
		.len = sizeof(nl_iftype),
		.data = &nl_iftype,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_iface_cb,
		.data = &res,
	};

	int ret = -1;

	ret = nlwifi_cmd(ifname, &ctx);
	if (ret)
		return ret;

	*mode = ieee80211_type_to_wifi_mode((enum nl80211_iftype) nl_iftype);
	libwifi_dbg("nl_type=%d wifi_mode=%d\n", nl_iftype, *mode);

	return 0;
}

static int _nlwifi_vendor_cmdresp(struct nl_msg *msg, void *resp)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *nl_resp, *nl_iter;
	struct cmd_respdata {
		int *len;
		void *data;
	} *res = resp;
	int iter = 0;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	*res->len = 0;
	nl_resp = tb[NL80211_ATTR_VENDOR_DATA];
	if (!nl_resp) {
		return NL_SKIP;
	}

	nla_for_each_nested(nl_iter, nl_resp, iter) {
		memcpy((uint8_t *)res->data + *res->len, nla_data(nl_iter),
						nla_len(nl_iter));
		*res->len += nla_len(nl_iter);
	}

	return NL_SKIP;
}

int nlwifi_vendor_cmd(const char *ifname, uint32_t vid, uint32_t subcmd,
					uint8_t *in, int ilen,
					uint8_t *out, int *olen)
{
	struct nl_sock *nl;
	struct nl_msg *msg;
	int devidx;
	int ret = -1;
	struct cmd_respdata {
		int *len;
		void *data;
	} resp = {
		.len = olen,
		.data = out,
	};

	libwifi_dbg("[%s] %s called vid 0x%x subcmd %u ilen %d olen %d\n", ifname, __func__, vid, subcmd, ilen, *olen);

	devidx = if_nametoindex(ifname);
	if (!devidx) {
		ret = -errno;
		goto out;
	}

	nl = nlwifi_socket();
	if (!nl)
		goto out;

	msg = nlwifi_alloc_msg(nl, NL80211_CMD_VENDOR, 0, 0);
	if (!msg)
		goto out_msg_failure;

	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, vid);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD, subcmd);
	if (in)
		NLA_PUT(msg, NL80211_ATTR_VENDOR_DATA, ilen, in);

	ret = nlwifi_send_msg(nl, msg, _nlwifi_vendor_cmdresp, &resp);

	if (ret < 0)
		goto nla_put_failure;

	ret = 0;
nla_put_failure:
	nlmsg_free(msg);
out_msg_failure:
	nl_socket_free(nl);
out:
	libwifi_dbg("[%s] %s called ret %d olen %d\n", ifname, __func__, ret, *olen);
	return ret;
}

int nlwifi_ap_get_caps(const char *ifname, struct wifi_caps *caps)
{
	//TODO
	return 0;
}

enum ieee80211_op_mode {
    IEEE80211_MODE_INFRA = 0,
    IEEE80211_MODE_IBSS = 1,
    IEEE80211_MODE_AP = 2,
    IEEE80211_MODE_MESH = 5,

    IEEE80211_MODE_NUM,
};

static int nlwifi_update_he_eht_caps(struct wifi_caps *caps, struct nlattr **tb_iftype, int opmode)
{
	int curr_opmode, len, i;
	struct nlattr *tb_flags[NL80211_IFTYPE_MAX + 1];

	if (nla_parse_nested(tb_flags, NL80211_IFTYPE_MAX,
			tb_iftype[NL80211_BAND_IFTYPE_ATTR_IFTYPES], NULL))
		return -1;

	for (i = 0; i < NUM_NL80211_IFTYPES; i++) {
		if (!nla_get_flag(tb_flags[i]))
			continue;

		switch (i) {
		case NL80211_IFTYPE_STATION:
			curr_opmode = IEEE80211_MODE_INFRA;
			break;
		case NL80211_IFTYPE_ADHOC:
			curr_opmode = IEEE80211_MODE_IBSS;
			break;
		case NL80211_IFTYPE_AP:
			curr_opmode = IEEE80211_MODE_AP;
			break;
		case NL80211_IFTYPE_MESH_POINT:
			curr_opmode = IEEE80211_MODE_MESH;
			break;
		default:
			return -1;
		}

		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_PHY])
			caps->valid |= WIFI_CAP_EHT_VALID;

		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_HE_CAP_PHY])
			caps->valid |= WIFI_CAP_HE_VALID;

		if (curr_opmode != opmode)
			/* copy HE/EHT caps only for choosen mode */
			continue;

		/* HE */
		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_HE_CAP_PHY]) {
			len = nla_len(tb_iftype[NL80211_BAND_IFTYPE_ATTR_HE_CAP_PHY]);

			if (len > sizeof(caps->he.byte_phy))
				len = sizeof(caps->he.byte_phy);

			memcpy(caps->he.byte_phy,
					nla_data(tb_iftype[NL80211_BAND_IFTYPE_ATTR_HE_CAP_PHY]),
					len);
		}

		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_HE_CAP_MAC]) {
			len = nla_len(tb_iftype[NL80211_BAND_IFTYPE_ATTR_HE_CAP_MAC]);

			if (len > sizeof(caps->he.byte_mac))
				len = sizeof(caps->he.byte_mac);

			memcpy(caps->he.byte_mac,
					nla_data(tb_iftype[NL80211_BAND_IFTYPE_ATTR_HE_CAP_MAC]),
					len);
		}

		/* EHT */
		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_MAC]) {
			len = nla_len(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_MAC]);

			if (len > sizeof(caps->eht.byte_mac))
				len = sizeof(caps->eht.byte_mac);

			memcpy(caps->eht.byte_mac,
					nla_data(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_MAC]),
					len);
		}

		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_PHY]) {
			len = nla_len(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_PHY]);

			if (len > sizeof(caps->eht.byte_phy))
				len = sizeof(caps->eht.byte_phy);

			memcpy(caps->eht.byte_phy,
					nla_data(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_PHY]),
					len);
		}

		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_MCS_SET]) {
			len = nla_len(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_MCS_SET]);

			if (len > sizeof(caps->eht.supp_mcs))
				len = sizeof(caps->eht.supp_mcs);

			memcpy(caps->eht.supp_mcs,
					nla_data(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_MCS_SET]),
					len);
		}

		if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_PPE]) {
			len = nla_len(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_PPE]);

			if (len > sizeof(caps->eht.byte_ppe_th))
				len = sizeof(caps->eht.byte_ppe_th);

			memcpy(caps->eht.byte_ppe_th,
					nla_data(tb_iftype[NL80211_BAND_IFTYPE_ATTR_EHT_CAP_PPE]),
					len);
		}
	}
	return 0;
}

static int nlwifi_radio_caps_cb(struct nl_msg *msg, void *data)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *tb_band[NL80211_BAND_ATTR_MAX + 1];
	struct nlattr *nl_band;
	struct caps_data *caps_data = data;
	struct wifi_caps *caps = caps_data->caps;
	enum wifi_band band;
	int iter_band;

#define min(x, y)	(x) < (y) ? (x) : (y)

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (tb[NL80211_ATTR_EXT_CAPA]) {
		caps->valid |= WIFI_CAP_EXT_VALID;
		memcpy(caps->ext.byte,
				nla_data(tb[NL80211_ATTR_EXT_CAPA]),
				min(nla_len(tb[NL80211_ATTR_EXT_CAPA]), 16));
	}

	if (!tb[NL80211_ATTR_WIPHY_BANDS])
		return NL_SKIP;

	nla_for_each_nested(nl_band, tb[NL80211_ATTR_WIPHY_BANDS], iter_band) {

		band = nlwifi_band_to_wifi_band(nl_band->nla_type);
		if (caps_data->band && caps_data->band != BAND_ANY && caps_data->band != band)
			continue;

		nla_parse(tb_band,
			NL80211_BAND_ATTR_MAX,
			nla_data(nl_band),
			nla_len(nl_band),
			NULL);

		if (tb_band[NL80211_BAND_ATTR_HT_CAPA]) {
			caps->valid |= WIFI_CAP_HT_VALID;
			caps->ht.cap = nla_get_u16(tb_band[NL80211_BAND_ATTR_HT_CAPA]);
		}
		if (tb_band[NL80211_BAND_ATTR_VHT_CAPA]) {
			caps->valid |= WIFI_CAP_VHT_VALID;
			caps->vht.cap = nla_get_u32(tb_band[NL80211_BAND_ATTR_VHT_CAPA]);
		}

		if (tb_band[NL80211_BAND_ATTR_IFTYPE_DATA]) {
			struct nlattr *tb_iftype[NL80211_BAND_IFTYPE_ATTR_MAX + 1];
			struct nlattr *nl_iftype;
			int rem, opmode;

			/* TODO: caps->he has room only for one opmode - selected arbitrarily here */
			opmode = IEEE80211_MODE_INFRA;
			nla_for_each_nested(nl_iftype, tb_band[NL80211_BAND_ATTR_IFTYPE_DATA], rem) {
				nla_parse(tb_iftype, NL80211_BAND_IFTYPE_ATTR_MAX,
						  nla_data(nl_iftype), nla_len(nl_iftype), NULL);

				if (tb_iftype[NL80211_BAND_IFTYPE_ATTR_IFTYPES]) {
					nlwifi_update_he_eht_caps(caps, tb_iftype, opmode);
				}
			}
		}
	}

	//TODO:
	// NL80211_ATTR_FEATURE_FLAGS
	// NL80211_ATTR_EXT_FEATURES,
	// NL80211_ATTR_IFTYPE_EXT_CAPA (per-AP caps can override radio caps)

	return NL_SKIP;
}

int nlwifi_radio_get_band_caps(const char *phyname, enum wifi_band band, struct wifi_caps *caps)
{
	struct caps_data data = {
		.phyname = phyname,
		.band = band,
		.caps = caps,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_radio_caps_cb,
		.data = &data,
	};

	libwifi_dbg("[%s, %s] %s called\n", phyname, wifi_band_to_str(band), __func__);

	return nlwifi_cmd(phyname, &ctx);
}

int nlwifi_radio_get_caps(const char *name, struct wifi_caps *caps)
{
	return nlwifi_radio_get_band_caps(name, BAND_ANY, caps);
}

static void _nlwifi_set_supp_stds(uint32_t bands, const struct wifi_caps *caps, uint8_t *std)
{
	if (bands & BAND_2)
		*std |= WIFI_G | WIFI_B;

	if (bands & BAND_5)
		*std |= WIFI_A;

	if (caps->valid & WIFI_CAP_HT_VALID)
		*std |= WIFI_N;
	if (caps->valid & WIFI_CAP_VHT_VALID)
		*std |= WIFI_AC;
	if (caps->valid & WIFI_CAP_HE_VALID)
		*std |= WIFI_AX;
	if (caps->valid & WIFI_CAP_EHT_VALID)
		*std |= WIFI_BE;
}

#define WIFI_CAP_HT_CHW_40	0x0002
#define WIFI_CAP_VHT_CHW_160	0x00000004
#define WIFI_CAP_VHT_CHW_8080	0x00000008
#define WIFI_CAP_VHT_CHW_MASK	0x0000000C
static void _nlwifi_set_supp_bandwidth(const struct wifi_caps *caps, uint32_t *supp_bw)
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

static int ext_feature_isset(const uint8_t *ext_features, int ext_features_len,
			     enum nl80211_ext_feature_index ftidx)
{
	uint8_t ft_byte;

	if ((int) ftidx / 8 >= ext_features_len)
		return 0;

	ft_byte = ext_features[ftidx / 8];
	return (ft_byte & BIT(ftidx % 8)) != 0;
}

static int nlwifi_get_phy_info_cb(struct nl_msg *msg, void *data)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct radio_data *radio_data = data;
	struct wifi_radio *radio = radio_data->radio;

	struct wifi_caps caps = { 0 };
	struct caps_data caps_data = {
		.phyname = radio_data->phyname,
		.band = radio_data->band,
		.caps = &caps,
	};

	struct nlattr *nl_band;
	enum nl80211_band band;
	int iter_band;

#define min(x, y)	(x) < (y) ? (x) : (y)

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (tb[NL80211_ATTR_WIPHY_BANDS]) {
		nla_for_each_nested(nl_band, tb[NL80211_ATTR_WIPHY_BANDS], iter_band) {
			band = nl_band->nla_type;
			switch (band) {
			case NL80211_BAND_2GHZ:
				radio->supp_band |= BAND_2;
				radio->oper_band = BAND_2;
				break;
			case NL80211_BAND_5GHZ:
				radio->supp_band |= BAND_5;
				radio->oper_band = BAND_5;
				break;
			case NL80211_BAND_6GHZ:
				radio->supp_band |= BAND_6;
				radio->oper_band = BAND_6;
				break;
			default:
				break;
			}
		}
	}

	if (radio_data->band != BAND_ANY)
		radio->oper_band = radio_data->band;

	nlwifi_radio_caps_cb(msg, &caps_data);
	_nlwifi_set_supp_stds(radio->supp_band, &caps, &radio->supp_std);
	_nlwifi_set_supp_bandwidth(&caps, &radio->supp_bw);

	if (tb[NL80211_ATTR_WIPHY_FRAG_THRESHOLD])
		radio->frag = nla_get_u32(tb[NL80211_ATTR_WIPHY_FRAG_THRESHOLD]);

	if (tb[NL80211_ATTR_WIPHY_RTS_THRESHOLD])
		radio->rts = nla_get_u32(tb[NL80211_ATTR_WIPHY_RTS_THRESHOLD]);

	if (tb[NL80211_ATTR_WIPHY_RETRY_SHORT])
		radio->srl = nla_get_u8(tb[NL80211_ATTR_WIPHY_RETRY_SHORT]);

	if (tb[NL80211_ATTR_WIPHY_RETRY_LONG])
		radio->lrl = nla_get_u8(tb[NL80211_ATTR_WIPHY_RETRY_LONG]);

	if (tb[NL80211_ATTR_BEACON_INTERVAL])
		radio->beacon_int = nla_get_u32(tb[NL80211_ATTR_BEACON_INTERVAL]);

	if (tb[NL80211_ATTR_DTIM_PERIOD])
		radio->dtim_period = nla_get_u32(tb[NL80211_ATTR_DTIM_PERIOD]);

	if (tb[NL80211_ATTR_WIPHY_ANTENNA_TX]) {
		uint32_t tmp;
		int i;

		tmp = nla_get_u32(tb[NL80211_ATTR_WIPHY_ANTENNA_TX]);
		for (i = 0; i < 8; i++) {
			if (!!(tmp & (1 << i)))
				radio->tx_streams++;
		}
	}

	if (tb[NL80211_ATTR_WIPHY_ANTENNA_RX]) {
		uint32_t tmp;
		int i;

		tmp = nla_get_u32(tb[NL80211_ATTR_WIPHY_ANTENNA_RX]);
		for (i = 0; i < 8; i++) {
			if (!!(tmp & (1 << i)))
				radio->rx_streams++;
		}
	}

	if (tb[NL80211_ATTR_EXT_FEATURES]) {
		uint8_t *ext_features;
		int len;

		ext_features = nla_data(tb[NL80211_ATTR_EXT_FEATURES]);
		len = nla_len(tb[NL80211_ATTR_EXT_FEATURES]);

		if (ext_feature_isset(ext_features, len, NL80211_EXT_FEATURE_RADAR_BACKGROUND))
			radio->cac_methods |= BIT(WIFI_CAC_MIMO_REDUCED);
	}

	return NL_SKIP;
}

int nlwifi_get_phy_info(const char *phyname, enum wifi_band band, struct wifi_radio *radio)
{
	struct radio_data data = {
		.phyname = phyname,
		.band = band,
		.radio = radio,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_phy_info_cb,
		.data = &data,
	};

	memset(radio, 0, sizeof(*radio));
	return nlwifi_cmd(phyname, &ctx);
}

int nlwifi_is_dot11h_enabled(const char *phy, bool *dot11h)
{
	struct chan_entry ch[64] = {0};
	int num = ARRAY_SIZE(ch);
	struct channels_data data = {
		.chan_entry = ch,
		.band = BAND_5,
		.chlist = NULL,
		.num = 0,
		.max = num,
	};
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_supp_channels_cb,
		.data = &data,
	};
	int i;

	*dot11h = false;
	libwifi_dbg("[%s] %s called\n", phy, __func__);
	if (nlwifi_cmd(phy, &ctx))
		return -1;

	for (i = 0; i < data.num; i++) {
		if (data.chan_entry[i].dfs) {
			*dot11h = true;
			return 0;
		}
	}

	return 0;
}

int nlwifi_radio_info_band(const char *name, enum wifi_band band, struct wifi_radio *radio)
{
	struct survey_entry survey = {};
	struct survey_data survey_data = {
		.survey_entry = &survey,
		.num = 0,
		.max = 1,
		.band = band,
	};
	char netdev[16];
	int ret = -1;

	libwifi_dbg("[%s, %s] %s called\n", name, wifi_band_to_str(band), __func__);

	ret = nlwifi_get_phy_info(name, band, radio);

	if (WARN_ON(ret))
		return ret;

	nlwifi_phy_to_netdev_with_band(name, netdev, sizeof(netdev), band);
	if (WARN_ON(nlwifi_get_survey(netdev, &survey_data)))
		return 0;

	radio->channel = (uint8_t)ieee80211_frequency_to_channel(survey.freq);
	radio->diag.cca_time = survey.active_time * 1000;
	radio->diag.channel_busy = survey.busy_time * 1000;
	radio->diag.tx_airtime = survey.tx_time * 1000;
	radio->diag.rx_airtime = survey.rx_time * 1000;
	radio->num_iface = WIFI_IFACE_MAX_NUM;
	nlwifi_get_phy_wifi_ifaces(name, band, radio->iface, &radio->num_iface);

	libwifi_dbg("[%s] %s oper-band = %d\n", name, __func__, radio->oper_band);
	if (radio->oper_band == BAND_5) {
		/* TODO get it from iface combinations */
		radio->cac_methods |= BIT(WIFI_CAC_CONTINUOUS);

		radio->dot11h_capable = true;
		nlwifi_is_dot11h_enabled(name, &radio->dot11h_enabled);
	} else {
		/* Don't report CAC methods for non 5GHz */
		radio->cac_methods = 0;
	}

	return 0;
}

int nlwifi_radio_info(const char *name, struct wifi_radio *radio)
{
	return nlwifi_radio_info_band(name, BAND_ANY, radio);
}

int nlwifi_radio_is_multiband(const char *name, bool *res)
{
	struct wifi_radio radio = {};
	int bands = 0;
	int ret;

	ret = nlwifi_radio_info(name, &radio);
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

static int nlwifi_get_assoclist_cb(struct nl_msg *msg, void *assoclist)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct asslist {
		const char *vif;
		int i;
		int nr;
		uint8_t *macs;
	} *stalist = assoclist;
	uint8_t *e;
	char ifname[16] = {0};

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);


	if (stalist->i > stalist->nr) {
		libwifi_warn("Num stations > %d !\n", stalist->nr);
		return NL_SKIP;
	}


	if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), ifname);
	if (!tb[NL80211_ATTR_MAC])
		return NL_SKIP;

	if (!strncmp(stalist->vif, ifname, 15)) {
		const uint8_t *mac = nla_data(tb[NL80211_ATTR_MAC]);

		e = stalist->macs + stalist->i * 6;
		memcpy(e, mac, 6);
		stalist->i += 1;
	}

	return NL_SKIP;
}

int nlwifi_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	struct asslist {
		const char *vif;
		int i;
		int nr;
		uint8_t *macs;
	} stalist = {
		.vif = ifname,
		.i = 0,
		.nr = *num_stas,
		.macs = stas,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_STATION,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_assoclist_cb,
		.data = &stalist,
	};

	int ret;

	ret = nlwifi_cmd(ifname, &ctx);
	if (ret)
		return ret;

	*num_stas = stalist.i;

	return 0;
}

static int nlwifi_bitrate_attrs(struct nlattr *attr, struct wifi_rate *rate)
{
	struct nlattr *r[NL80211_RATE_INFO_MAX + 1];
	static struct nla_policy bitrate_policy[NL80211_RATE_INFO_MAX + 1] = {
		[NL80211_RATE_INFO_BITRATE] = { .type = NLA_U16 },
		[NL80211_RATE_INFO_MCS] = { .type = NLA_U8 },
		[NL80211_RATE_INFO_40_MHZ_WIDTH] = { .type = NLA_FLAG },
		[NL80211_RATE_INFO_SHORT_GI] = { .type = NLA_FLAG },
		[NL80211_RATE_INFO_BITRATE32] = { .type = NLA_U32 },
		[NL80211_RATE_INFO_VHT_MCS] = { .type = NLA_U8 },
		[NL80211_RATE_INFO_VHT_NSS] = { .type = NLA_U8 },
		[NL80211_RATE_INFO_80_MHZ_WIDTH] = { .type = NLA_FLAG },
		[NL80211_RATE_INFO_80P80_MHZ_WIDTH] = { .type = NLA_FLAG },
		[NL80211_RATE_INFO_160_MHZ_WIDTH] = { .type = NLA_FLAG },
	};
	uint32_t r100kbps = 0;
	int ret = 0;

	ret = nla_parse_nested(r, NL80211_RATE_INFO_MAX, attr, bitrate_policy);
	if (ret) {
		libwifi_err("Error parsing nested bitrate attr %s\n", nl_geterror(ret));
		return ret;
	}

	if (r[NL80211_RATE_INFO_BITRATE32])
		r100kbps= nla_get_u32(r[NL80211_RATE_INFO_BITRATE32]);
	else if (r[NL80211_RATE_INFO_BITRATE])
		r100kbps = nla_get_u16(r[NL80211_RATE_INFO_BITRATE]);

	rate->rate = r100kbps / 10;

	if (r[NL80211_RATE_INFO_SHORT_GI])
		rate->m.sgi = 1;

	if (r[NL80211_RATE_INFO_MCS]) {
		rate->m.mcs = nla_get_u8(r[NL80211_RATE_INFO_MCS]);
		rate->phy = PHY_HT;
	}

	if (r[NL80211_RATE_INFO_VHT_MCS]) {
		rate->m.mcs = nla_get_u8(r[NL80211_RATE_INFO_VHT_MCS]);
		rate->phy = PHY_VHT;
	}

	if (r[NL80211_RATE_INFO_VHT_NSS])
		rate->m.nss = nla_get_u8(r[NL80211_RATE_INFO_VHT_NSS]);

	if (r[NL80211_RATE_INFO_40_MHZ_WIDTH])
		rate->m.bw = 40;
	else if (r[NL80211_RATE_INFO_80_MHZ_WIDTH])
		rate->m.bw = 80;
	else if (r[NL80211_RATE_INFO_160_MHZ_WIDTH])
		rate->m.bw = 160;
	else if (r[NL80211_RATE_INFO_80P80_MHZ_WIDTH])
		rate->m.bw = 160;
	else
		rate->m.bw = 0;

	return 0;
}

static int nlwifi_bss_param_attr(struct nlattr *bss_param_attr, struct wifi_sta *sta)
{
	struct nlattr *bss_info[NL80211_STA_BSS_PARAM_MAX + 1];
	static struct nla_policy bss_poilcy[NL80211_STA_BSS_PARAM_MAX + 1] = {
		[NL80211_STA_BSS_PARAM_SHORT_PREAMBLE] = { .type = NLA_FLAG },
		[NL80211_STA_BSS_PARAM_SHORT_SLOT_TIME] = { .type = NLA_FLAG },
	};

	if (nla_parse_nested(bss_info, NL80211_STA_BSS_PARAM_MAX,
		bss_param_attr, bss_poilcy)) {
		return -1;
	}

	if (bss_info[NL80211_STA_BSS_PARAM_SHORT_SLOT_TIME]) {
		if (nla_get_u16(bss_info[NL80211_STA_BSS_PARAM_SHORT_SLOT_TIME])) {
			wifi_cap_set(sta->cbitmap, WIFI_CAP_SHORT_SLOT);
		}
	}

	return 0;
}


static int nlwifi_get_station_cb(struct nl_msg *msg, void *data)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct stainfo {
		const char *vif;
		const uint8_t *macaddr;
		struct wifi_sta *s;
	} *resp = data;
	struct wifi_sta *sta = resp->s;
	struct nlattr *s[NL80211_STA_INFO_MAX + 1];

	static struct nla_policy sta_policy[NL80211_STA_INFO_MAX + 1] = {
		[NL80211_STA_INFO_INACTIVE_TIME] = { .type = NLA_U32 },
		[NL80211_STA_INFO_RX_BYTES] = { .type = NLA_U32 },
		[NL80211_STA_INFO_TX_BYTES] = { .type = NLA_U32 },
		[NL80211_STA_INFO_SIGNAL] = { .type = NLA_U8 },
		[NL80211_STA_INFO_TX_BITRATE] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_RX_PACKETS] = { .type = NLA_U32 },
		[NL80211_STA_INFO_TX_PACKETS] = { .type = NLA_U32 },
		[NL80211_STA_INFO_TX_RETRIES] = { .type = NLA_U32 },
		[NL80211_STA_INFO_TX_FAILED] = { .type = NLA_U32 },
		[NL80211_STA_INFO_SIGNAL_AVG] = { .type = NLA_U8 },
		[NL80211_STA_INFO_RX_BITRATE] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_BSS_PARAM] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_CONNECTED_TIME] = { .type = NLA_U32},
		[NL80211_STA_INFO_RX_DURATION] = { .type = NLA_U64 },
		[NL80211_STA_INFO_TX_DURATION] = { .type = NLA_U64 },
		[NL80211_STA_INFO_STA_FLAGS] =
			{ .minlen = sizeof(struct nl80211_sta_flag_update) },
		[NL80211_STA_INFO_RX_BYTES64] = { .type = NLA_U64 },
		[NL80211_STA_INFO_TX_BYTES64] = { .type = NLA_U64 },
		[NL80211_STA_INFO_CHAIN_SIGNAL] = { .type = NLA_NESTED },
		[NL80211_STA_INFO_EXPECTED_THROUGHPUT] = { .type = NLA_U32 },
		[NL80211_STA_INFO_RX_DROP_MISC] = { .type = NLA_U64},
	};
	const uint8_t *mac;
	char ifname[16] = {0};
	struct nl80211_sta_flag_update *sta_flags;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if_indextoname(nla_get_u32(tb[NL80211_ATTR_IFINDEX]), ifname);
	if (!tb[NL80211_ATTR_MAC] || !tb[NL80211_ATTR_STA_INFO])
		return NL_SKIP;

	if (strncmp(resp->vif, ifname, 15))
		return NL_SKIP;    /* not ours */

	mac = nla_data(tb[NL80211_ATTR_MAC]);
	if (resp->macaddr && memcmp(mac, resp->macaddr, 6))
		return NL_SKIP;
	memcpy(sta->macaddr, mac, 6);

	if (nla_parse_nested(s, NL80211_STA_INFO_MAX, tb[NL80211_ATTR_STA_INFO],
				sta_policy)) {
		libwifi_warn("Error parsing NL80211_STA_INFO attrs!\n");
		return NL_SKIP;
	}

	memcpy(sta->bssid, nla_data(tb[NL80211_ATTR_MAC]), 6);

	if (s[NL80211_STA_INFO_INACTIVE_TIME])
		sta->idle_time =
			nla_get_u32(s[NL80211_STA_INFO_INACTIVE_TIME]) / 1000;

	if (s[NL80211_STA_INFO_CONNECTED_TIME])
		sta->conn_time =
			nla_get_u32(s[NL80211_STA_INFO_CONNECTED_TIME]);

	if (s[NL80211_STA_INFO_RX_BITRATE])
		nlwifi_bitrate_attrs(s[NL80211_STA_INFO_RX_BITRATE], &sta->rx_rate);

	if (s[NL80211_STA_INFO_RX_DURATION])
		sta->rx_airtime =
			nla_get_u32(s[NL80211_STA_INFO_RX_DURATION]);

	if (s[NL80211_STA_INFO_TX_BITRATE])
		nlwifi_bitrate_attrs(s[NL80211_STA_INFO_TX_BITRATE], &sta->tx_rate);

	if (s[NL80211_STA_INFO_TX_DURATION])
		sta->tx_airtime =
			nla_get_u32(s[NL80211_STA_INFO_TX_DURATION]);

	if (s[NL80211_STA_INFO_RX_BYTES64]) {
		sta->stats.rx_bytes =
			nla_get_u64(s[NL80211_STA_INFO_RX_BYTES64]);
	} else if (s[NL80211_STA_INFO_RX_BYTES]) {
		sta->stats.rx_bytes =
			nla_get_u32(s[NL80211_STA_INFO_RX_BYTES]);
	}

	if (s[NL80211_STA_INFO_TX_BYTES64]) {
		sta->stats.tx_bytes =
			nla_get_u64(s[NL80211_STA_INFO_TX_BYTES64]);
	} else if (s[NL80211_STA_INFO_TX_BYTES]) {
		sta->stats.tx_bytes =
			nla_get_u32(s[NL80211_STA_INFO_TX_BYTES]);
	}

	if (s[NL80211_STA_INFO_RX_PACKETS])
		sta->stats.rx_pkts =
			nla_get_u32(s[NL80211_STA_INFO_RX_PACKETS]);

	if (s[NL80211_STA_INFO_TX_PACKETS])
		sta->stats.tx_pkts =
			nla_get_u32(s[NL80211_STA_INFO_TX_PACKETS]);

	if (s[NL80211_STA_INFO_TX_RETRIES])
		sta->stats.tx_retry_pkts =
			nla_get_u32(s[NL80211_STA_INFO_TX_RETRIES]);

	if (s[NL80211_STA_INFO_TX_FAILED])
		sta->stats.tx_fail_pkts =
			nla_get_u32(s[NL80211_STA_INFO_TX_FAILED]);

	if (s[NL80211_STA_INFO_RX_DROP_MISC])
		sta->stats.rx_fail_pkts =
			nla_get_u64(s[NL80211_STA_INFO_RX_DROP_MISC]);


	if (s[NL80211_STA_INFO_SIGNAL]) {
		sta->rssi[0] = nla_get_u8(s[NL80211_STA_INFO_SIGNAL]);
	}

	if (s[NL80211_STA_INFO_CHAIN_SIGNAL]) {
		struct nlattr *itr_attr;
		struct nlattr *rssi_attrs = s[NL80211_STA_INFO_CHAIN_SIGNAL];
		int idx = 0;
		int rem;

		nla_for_each_nested(itr_attr, rssi_attrs, rem) {
			sta->rssi[idx++] = nla_get_u8(itr_attr);
			if (idx == WIFI_NUM_ANTENNA)
				break;
		}
	}

	if (s[NL80211_STA_INFO_SIGNAL_AVG])
		sta->rssi_avg = (int)nla_get_u8(s[NL80211_STA_INFO_SIGNAL_AVG]);

	if (s[NL80211_STA_INFO_STA_FLAGS]) {
		sta_flags = (struct nl80211_sta_flag_update *)
					nla_data(s[NL80211_STA_INFO_STA_FLAGS]);

		if (sta_flags->mask & BIT(NL80211_STA_FLAG_SHORT_PREAMBLE)) {
			if (sta_flags->set & BIT(NL80211_STA_FLAG_SHORT_PREAMBLE)) {
				wifi_cap_set(sta->cbitmap, WIFI_CAP_SHORT_PREAMBLE);
			}
		}
		if (sta_flags->mask & BIT(NL80211_STA_FLAG_WME)) {
			wifi_cap_set(sta->cbitmap, WIFI_CAP_WMM);
			if (sta_flags->set & BIT(NL80211_STA_FLAG_WME)) {
				wifi_status_set(sta->sbitmap, WIFI_STATUS_WMM);
			}
		}
	}

	if (s[NL80211_STA_INFO_BSS_PARAM]) {
		nlwifi_bss_param_attr(s[NL80211_STA_INFO_BSS_PARAM], sta);
	}

	if (s[NL80211_STA_INFO_EXPECTED_THROUGHPUT])
		sta->tx_thput =
			nla_get_u32(s[NL80211_STA_INFO_EXPECTED_THROUGHPUT]) / 1024;

	return NL_SKIP;
}

int nlwifi_sta_info(const char *ifname, struct wifi_sta *info)
{
	struct stainfo {
		const char *vif;
		const uint8_t *macaddr;
		struct wifi_sta *s;
	} sta = {
		.vif = ifname,
		.macaddr = NULL,
		.s = info,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_STATION,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_station_cb,
		.data = &sta,
	};

	return nlwifi_cmd(ifname, &ctx);
}

int nlwifi_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	struct stainfo {
		const char *vif;
		const uint8_t *macaddr;
		struct wifi_sta *s;
	} sta = {
		.vif = ifname,
		.macaddr = addr,
		.s = info,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_STATION,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_station_cb,
		.data = &sta,
	};

	return nlwifi_cmd(ifname, &ctx);
}

int nlwifi_sta_get_stats(const char *ifname, struct wifi_sta_stats *stats)
{
	int ret;
	struct stainfo {
		const char *vif;
		const uint8_t *macaddr;
		struct wifi_sta *s;
	};

	struct wifi_sta station = {0};
	struct stainfo sta = {0};

	sta.vif = ifname;
	sta.macaddr = NULL;
	sta.s = &station;

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_STATION,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_station_cb,
		.data = &sta,
	};


	ret = nlwifi_cmd(ifname, &ctx);
	if (ret)
		return ret;

	memcpy(stats, &(sta.s->stats), sizeof(struct wifi_sta_stats));

	return 0;
}

static int nlwifi_get_scan_results_cb(struct nl_msg *msg, void *sres)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *bss[NL80211_BSS_MAX + 1];
	static struct nla_policy bss_policy[NL80211_BSS_MAX + 1] = {
		[NL80211_BSS_BSSID] = { .type = NLA_UNSPEC },
		[NL80211_BSS_FREQUENCY] = { .type = NLA_U32 },
		[NL80211_BSS_TSF] = { .type = NLA_U64 },
		[NL80211_BSS_BEACON_INTERVAL] = { .type = NLA_U16 },
		[NL80211_BSS_CAPABILITY] = { .type = NLA_U16 },
		[NL80211_BSS_INFORMATION_ELEMENTS] = { .type = NLA_UNSPEC },
		[NL80211_BSS_SIGNAL_MBM] = { .type = NLA_U32 },
		[NL80211_BSS_SIGNAL_UNSPEC] = { .type = NLA_U8 },
		[NL80211_BSS_STATUS] = { .type = NLA_U32 },
		[NL80211_BSS_SEEN_MS_AGO] = { .type = NLA_U32 },
		[NL80211_BSS_BEACON_IES] = { .type = NLA_UNSPEC },
	};
	struct scanresult {
		int i;
		int num;
		struct wifi_bss *bsss;
	} *scanres = sres;
	struct wifi_bss *e = scanres->bsss + scanres->i;
	uint8_t *ssid_ie;
	size_t ie_len;
	uint8_t *ie;
	int rssi = 0;
	int ret;
	int i;

	libwifi_dbg("sres: &bsss = %p (numbss limit = %d)\n", e, scanres->num);
	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_BSS])
		return NL_SKIP;  /* no bssid */

	if (scanres->i >= scanres->num) {
		libwifi_warn("Num scan results > %d !\n", scanres->num);
		return NL_SKIP;
	}

	if (nla_parse_nested(bss, NL80211_BSS_MAX, tb[NL80211_ATTR_BSS],
				bss_policy))
		return NL_SKIP;

	if (bss[NL80211_BSS_INFORMATION_ELEMENTS]) {
		ie = nla_data(bss[NL80211_BSS_INFORMATION_ELEMENTS]);
		ie_len = nla_len(bss[NL80211_BSS_INFORMATION_ELEMENTS]);
	} else {
		ie = NULL;
		ie_len = 0;
	}

	if (bss[NL80211_BSS_BSSID]) {
		libwifi_dbg("BSSID: " MACFMT "  ",
			MAC2STR((uint8_t *)nla_data(bss[NL80211_BSS_BSSID])));
		e = scanres->bsss + scanres->i;
		memset(e, 0, sizeof(struct wifi_bss));
		memcpy(e->bssid, nla_data(bss[NL80211_BSS_BSSID]), 6);
	}

	if (bss[NL80211_BSS_FREQUENCY]) {
		uint32_t frequency;
		libwifi_dbg("Freq: %u Mhz      ",
				nla_get_u32(bss[NL80211_BSS_FREQUENCY]));
		frequency = nla_get_u32(bss[NL80211_BSS_FREQUENCY]);
		e->channel = (uint8_t)wifi_freq_to_channel(frequency);
		e->band = ieee80211_frequency_to_band(frequency);
	}

	if (bss[NL80211_BSS_BEACON_INTERVAL]) {
		e->beacon_int = nla_get_u16(bss[NL80211_BSS_BEACON_INTERVAL]);
		libwifi_dbg("    Bcn Int: %d  ",
				nla_get_u16(bss[NL80211_BSS_BEACON_INTERVAL]));
	}

	if (bss[NL80211_BSS_CAPABILITY]) {
		e->caps.basic.cap = nla_get_u16(bss[NL80211_BSS_CAPABILITY]);
		e->caps.valid |= WIFI_CAP_BASIC_VALID;
	}

	if (bss[NL80211_BSS_SIGNAL_MBM]) {
		rssi = nla_get_u32(bss[NL80211_BSS_SIGNAL_MBM]);
		rssi /= 100;	/* in dBm */
	} else if (bss[NL80211_BSS_SIGNAL_UNSPEC]) {
		rssi = nla_get_u8(bss[NL80211_BSS_SIGNAL_UNSPEC]);
	}
	e->rssi = rssi;
	libwifi_dbg("    Rssi: %d dBm", rssi);

	ssid_ie = wifi_find_ie(ie, ie_len, IE_SSID);
	if (ssid_ie) {
		libwifi_dbg("     Ssid: ");
		for (i = 0; i < ssid_ie[1]; i++)
			libwifi_dbg("%c", ssid_ie[2+i]);

		memcpy(e->ssid, &ssid_ie[2], ssid_ie[1]);
	}

	wifi_get_bss_security_from_ies(e, ie, ie_len);
	libwifi_dbg("       wpa_versions 0x%x pairwise = 0x%x, group = 0x%x akms 0x%x caps 0x%x\n",
		    e->rsn.wpa_versions, e->rsn.pair_ciphers, e->rsn.group_cipher,
		    e->rsn.akms, e->rsn.rsn_caps);

	nlwifi_get_ie(ie, ie_len, IE_BSS_LOAD, &e->load);

	ret = nlwifi_get_ie(ie, ie_len, IE_EXT_CAP, &e->caps.ext);
	if (!ret)
		e->caps.valid |= WIFI_CAP_EXT_VALID;

	ret = nlwifi_get_ie(ie, ie_len, IE_HT_CAP, &e->caps.ht);
	if (!ret)
		e->caps.valid |= WIFI_CAP_HT_VALID;

	ret = nlwifi_get_ie(ie, ie_len, IE_VHT_CAP, &e->caps.vht);
	if (!ret)
		e->caps.valid |= WIFI_CAP_VHT_VALID;

	ret = nlwifi_get_ie(ie, ie_len, IE_RRM, &e->caps.rrm);
	if (!ret)
		e->caps.valid |= WIFI_CAP_RM_VALID;

	nlwifi_get_bandwidth_from_ie(ie, ie_len, &e->curr_bw);

	//TODO: wifi supp standards
	//TODO: a-only, b-only, g-only etc.
	if (!!(e->caps.valid & WIFI_CAP_VHT_VALID))
		e->oper_std = WIFI_N | WIFI_AC;
	else if (!!(e->caps.valid & WIFI_CAP_HT_VALID))
		e->oper_std = e->channel > 14 ? (WIFI_A | WIFI_N) :
						(WIFI_B | WIFI_G | WIFI_N);
	else
		e->oper_std = e->channel > 14 ? WIFI_A : (WIFI_B | WIFI_G);

	scanres->i += 1;

	return NL_SKIP;
}

int nlwifi_get_scan_results(const char *ifname, struct wifi_bss *bsss,
						int *num)
{
	struct scanres {
		int i;
		int nr;
		struct wifi_bss *bsslist;
	} scanres = {
		.i = 0,
		.nr = *num,
		.bsslist = bsss,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_SCAN,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_scan_results_cb,
		.data = &scanres,
	};

	int ret;

	ret = nlwifi_cmd(ifname, &ctx);
	if (ret)
		return ret;

	*num = scanres.i;
	libwifi_dbg("Num scanned bsss = %d   bsss = %p\n", *num, bsss);

	return 0;
}


static int _nlwifi_get_country(struct nl_msg *msg, void *resp)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (tb[NL80211_ATTR_REG_ALPHA2])
		memcpy(resp, nla_data(tb[NL80211_ATTR_REG_ALPHA2]),
				nla_len(tb[NL80211_ATTR_REG_ALPHA2]));

	return NL_SKIP;
}

int nlwifi_get_country(const char *name, char *alpha2)
{
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_REG,
		.cb = _nlwifi_get_country,
		.data = alpha2,
	};

	return nlwifi_cmd(name, &ctx);
}

static int nlwifi_get_survey_cb(struct nl_msg *msg, void *data)
{
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *sinfo[NL80211_SURVEY_INFO_MAX + 1];
	struct survey_data *survey_data = data;
	struct survey_entry *survey;

	static struct nla_policy survey_policy[NL80211_SURVEY_INFO_MAX + 1] = {
		[NL80211_SURVEY_INFO_FREQUENCY] = { .type = NLA_U32 },
		[NL80211_SURVEY_INFO_NOISE] = { .type = NLA_U8 },
	};

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
		  genlmsg_attrlen(gnlh, 0), NULL);

	if (!tb[NL80211_ATTR_SURVEY_INFO]) {
		libwifi_dbg("survey data missing!\n");
		return NL_SKIP;
	}

	if (nla_parse_nested(sinfo, NL80211_SURVEY_INFO_MAX,
			     tb[NL80211_ATTR_SURVEY_INFO],
			     survey_policy)) {
		libwifi_dbg("failed to parse nested attributes!\n");
		return NL_SKIP;
	}

	/* Stop while we don't have more buffers */
	if (survey_data->num >= survey_data->max)
		return NL_STOP;

	/* If max == 1 fill data for current channel */
	if (survey_data->max == 1 && !sinfo[NL80211_SURVEY_INFO_IN_USE])
		return NL_SKIP;

        survey = &survey_data->survey_entry[survey_data->num];

	if (sinfo[NL80211_SURVEY_INFO_FREQUENCY])
		libwifi_dbg(" - frequency: %u MHz%s\n",
			    nla_get_u32(sinfo[NL80211_SURVEY_INFO_FREQUENCY]),
			    sinfo[NL80211_SURVEY_INFO_IN_USE] ? " [in use]" : "");
	survey->freq = nla_get_u32(sinfo[NL80211_SURVEY_INFO_FREQUENCY]);

	if (survey_data->band && survey_data->band != BAND_ANY &&
	    survey_data->band != ieee80211_frequency_to_band(survey->freq)) {
		libwifi_dbg("survey skip due to band\n");
		return NL_SKIP;
	}

        survey_data->num++;

	if (sinfo[NL80211_SURVEY_INFO_NOISE]) {
		survey->noise = (int8_t) nla_get_u8(sinfo[NL80211_SURVEY_INFO_NOISE]);
		libwifi_dbg(" - noise: %d dBm\n",
			    (int8_t)nla_get_u8(sinfo[NL80211_SURVEY_INFO_NOISE]));
	} else {
		libwifi_dbg(" - noise unknown\n");
	}

	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME])
		survey->active_time = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY])
		survey->busy_time = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_BUSY]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY])
		survey->busy_ext_time = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_EXT_BUSY]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX])
		survey->rx_time = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_RX]);
	if (sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX])
		survey->tx_time = nla_get_u64(sinfo[NL80211_SURVEY_INFO_CHANNEL_TIME_TX]);

	libwifi_dbg(" - active_time: %llu\n", survey->active_time);
	libwifi_dbg(" - busy_time: %llu (%u%%)\n", survey->busy_time,
		    survey->active_time ? (survey->busy_time * 100 / survey->active_time) : 0);
	libwifi_dbg(" - busy_ext_time: %llu (%u%%)\n", survey->busy_ext_time,
		    survey->active_time ? (survey->busy_ext_time * 100 / survey->active_time) : 0);
	libwifi_dbg(" - rx_time: %llu (%u%%)\n", survey->rx_time,
		    survey->active_time ? (survey->rx_time * 100 / survey->active_time) : 0);
	libwifi_dbg(" - tx_time: %llu (%u%%)\n", survey->tx_time,
		    survey->active_time ? (survey->tx_time * 100 / survey->active_time) : 0);

	return NL_SKIP;
}

static int nlwifi_get_survey(const char *name, struct survey_data *survey_data)
{
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_SURVEY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_survey_cb,
		.data = survey_data,
	};

	return nlwifi_cmd(name, &ctx);
}

int nlwifi_get_band_noise(const char *name, enum wifi_band band, int *noise)
{
	struct survey_entry survey = {};
	struct survey_data survey_data = {
		.survey_entry = &survey,
		.num = 0,
		.max = 1,
		.band = band,
	};
	int ret;

	ret = nlwifi_get_survey(name, &survey_data);
	if (ret)
		return ret;

	if (survey.noise == 0)
		return -1;

	*noise = survey.noise;
	return 0;
}

int nlwifi_get_noise(const char *name, int *noise)
{
	return nlwifi_get_band_noise(name, BAND_ANY, noise);
}

int nlwifi_get_band_supp_stds(const char *name, enum wifi_band band, uint8_t *std)
{
	struct wifi_caps caps = {};
	uint32_t bands = 0;

	*std = 0;

	if (WARN_ON(nlwifi_radio_get_band_caps(name, band, &caps)))
		return -1;

	if (band == BAND_ANY)
		nlwifi_get_supp_band(name, &bands);
	else
		bands = band;

	_nlwifi_set_supp_stds(bands, &caps, std);

	return 0;
}

int nlwifi_get_supp_stds(const char *name, uint8_t *std)
{
	return nlwifi_get_band_supp_stds(name, BAND_ANY, std);
}

int nlwifi_get_band_supp_bandwidths(const char *name, enum wifi_band band, uint32_t *supp_bw)
{
	struct wifi_caps caps;

	if (WARN_ON(nlwifi_radio_get_band_caps(name, band, &caps)))
		return -1;

	_nlwifi_set_supp_bandwidth(&caps, supp_bw);
	return 0;
}

int nlwifi_get_supp_bandwidths(const char *name, uint32_t *supp_bw)
{
	return nlwifi_get_band_supp_bandwidths(name, BAND_ANY, supp_bw);
}

int nlwifi_get_max_bandwidth(const char *name, enum wifi_bw *max_bw)
{
	uint32_t supp_bw = 0;

	if (WARN_ON(nlwifi_get_supp_bandwidths(name, &supp_bw)))
		return -1;

	if (supp_bw & BIT(BW320))
		*max_bw = BW320;
	if (supp_bw & BIT(BW160))
		*max_bw = BW160;
	else if (supp_bw & BIT(BW80))
		*max_bw = BW80;
	else if (supp_bw & BIT(BW40))
		*max_bw = BW40;
	else
		*max_bw = BW20;

	return 0;
}

int nlwifi_surveys_get(const char *name, enum wifi_band band, struct survey_entry *entry, int *num)
{
	char netdev[16] = {};
	struct survey_data survey_data = {
		.survey_entry = entry,
		.num = 0,
		.max = *num,
		.band = band,
	};

	int ret;

	nlwifi_phy_to_netdev(name, netdev, sizeof(netdev));
	libwifi_dbg("[%s %s %s] %s called max %d\n", name, netdev, wifi_band_to_str(band), __func__, *num);
        ret = nlwifi_get_survey(netdev, &survey_data);
	if (ret)
		return ret;

	*num = survey_data.num;
	libwifi_dbg("[%s] %s num %d\n", name, __func__, *num);

	return 0;
}

int nlwifi_channels_info_band(const char *name, enum wifi_band band, struct chan_entry *channel, int *num)
{
	struct survey_entry *entry;
	int entry_num;
	int i, j;

	struct channels_data data = {
		.chan_entry = channel,
		.chlist = NULL,
		.num = 0,
		.max = *num,
		.band = band,
	};
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_supp_channels_cb,
		.data = &data,
	};

	int ret;

	libwifi_dbg("[%s, %s] %s called (max: %d)\n", name, wifi_band_to_str(band), __func__, *num);

	memset(channel, 0, sizeof(*channel) * (*num));
	ret = nlwifi_cmd(name, &ctx);
	if (ret)
		return ret;

	*num = data.num;

	/* Sync channels with survey data */
	entry_num = *num;
	entry = calloc(entry_num, sizeof(*entry));
	WARN_ON(!entry);
	if (!entry)
		return -1;

	if (WARN_ON(nlwifi_surveys_get(name, band, entry, &entry_num))) {
		free(entry);
		return -1;
	}

	for (i = 0; i < *num; i++) {
		struct chan_entry *chan_entry;
		struct survey_entry *survey_entry;

		chan_entry = &channel[i];

		for (j = 0; j < entry_num; j++) {
			survey_entry = &entry[j];

			if (survey_entry->freq != chan_entry->freq)
				continue;

			if (survey_entry->noise)
				chan_entry->noise = survey_entry->noise;
			chan_entry->survey.cca_time = survey_entry->active_time * 1000;
			chan_entry->survey.channel_busy = survey_entry->busy_time * 1000;
			chan_entry->survey.tx_airtime = survey_entry->tx_time * 1000;
			chan_entry->survey.rx_airtime = survey_entry->rx_time * 1000;

			if (survey_entry->active_time) {
				chan_entry->busy = (uint8_t)(survey_entry->busy_time * 100 / survey_entry->active_time);
				chan_entry->score = (uint8_t)(100 - (survey_entry->busy_time * 100 / survey_entry->active_time));
			} else {
				chan_entry->score = 255;
				chan_entry->busy = 255;
			}

			break;
		}
	}

	free(entry);

	return ret;
}

int nlwifi_channels_info(const char *name, struct chan_entry *channel, int *num)
{
	return nlwifi_channels_info_band(name, BAND_ANY, channel, num);
}

static int nlwifi_get_supp_band_cb(struct nl_msg *msg, void *data)
{
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	uint32_t *bands = data;
	struct nlattr *nl_band;
	enum nl80211_band band;
	int iter_band;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	if (tb[NL80211_ATTR_WIPHY_BANDS]) {

		nla_for_each_nested(nl_band, tb[NL80211_ATTR_WIPHY_BANDS], iter_band) {
			band = nl_band->nla_type;
			switch (band) {
			case NL80211_BAND_2GHZ:
				*bands |= BAND_2;
				break;
			case NL80211_BAND_5GHZ:
				*bands |= BAND_5;
				break;
			case NL80211_BAND_6GHZ:
				*bands |= BAND_6;
				break;
			case NL80211_BAND_60GHZ:
				*bands |= BAND_60;
				break;
			default:
				break;
			}
		}
	}

	return NL_SKIP;
}

int nlwifi_get_supp_band(const char *name, uint32_t *bands)
{
	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_WIPHY,
		.flags = NLM_F_DUMP,
		.cb = nlwifi_get_supp_band_cb,
		.data = bands,
	};

	libwifi_dbg("[%s] %s called\n", name, __func__);

	*bands = 0;
	return nlwifi_cmd(name, &ctx);
}

int nlwifi_get_oper_band(const char *name, enum wifi_band *band)
{
	uint32_t freq = 0;

	if (WARN_ON(nlwifi_get_frequency(name, &freq)))
		return -1;

	*band = ieee80211_frequency_to_band(freq);

	libwifi_dbg("[%s] oper band %d from freq %u\n", name, *band, freq);
	return 0;
}

int nlwifi_get_4addr(const char *ifname, bool *enabled)
{
	uint8_t en = 0;
	int ret;
	struct cmd_respdata res = {
		.attr = NL80211_ATTR_4ADDR,
		.len = sizeof(en),
		.data = &en,
	};

	struct nlwifi_ctx ctx = {
		.cmd = NL80211_CMD_GET_INTERFACE,
		.cb = nlwifi_get_iface_cb,
		.data = &res,
	};

	ret = nlwifi_cmd(ifname, &ctx);
	if (!ret)
		*enabled = en == 1 ? true : false;

	return ret;
}

/* events handling */
struct nlwifi_event {
	struct nl_sock *sock;
	struct event_struct req;  /* as passed by caller */
};

int nlwifi_event_handler(struct nl_msg *msg, void *arg)
{
	struct nlwifi_event *wevt = (struct nlwifi_event *)arg;
	struct event_response *resp = &wevt->req.resp;
	struct nlmsghdr *nlh = nlmsg_hdr(msg);
	struct genlmsghdr *gnlh = nlmsg_data(nlh);
	struct nlattr *nlattrs[NUM_NL80211_ATTR];
	char ifname[16] = {0};
	int ret;

	if (!genlmsg_valid_hdr(nlh, 0)) {
		libwifi_err("received invalid message\n");
		return 0;
	}

	ret = nla_parse(nlattrs, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);
	if (ret)
		return ret;

	if (!nlattrs[NL80211_ATTR_IFINDEX])
		return 0;

	if_indextoname(nla_get_u32(nlattrs[NL80211_ATTR_IFINDEX]), ifname);

	libwifi_dbg("%s %s cmd %u\n", ifname, __func__, gnlh->cmd);

	if (strncmp(ifname, wevt->req.ifname, 16)) {
		/* libwifi_err("Event from '%s' is not ours.\n", ifname); */
		return 0;
	}

	switch (gnlh->cmd) {
	case NL80211_CMD_TRIGGER_SCAN:
		resp->type = WIFI_EVENT_SCAN_START;
		break;
	case NL80211_CMD_NEW_SCAN_RESULTS:
		resp->type = WIFI_EVENT_SCAN_END;
		break;
	case NL80211_CMD_SCAN_ABORTED:
		resp->type = WIFI_EVENT_SCAN_ABORT;
		break;
	case NL80211_CMD_CONNECT:
		resp->type = WIFI_EVENT_STA_ASSOC;
		break;
	case NL80211_CMD_DISCONNECT:
		resp->type = WIFI_EVENT_STA_DISASSOC;
		break;
	case NL80211_CMD_MICHAEL_MIC_FAILURE:
		break;
	case NL80211_CMD_VENDOR:
		{
			struct nlwifi_event_vendor_resp *vr =
				(struct nlwifi_event_vendor_resp *)resp->data;
			uint8_t *data;
			int len;

			resp->type = WIFI_EVENT_VENDOR;
			vr->oui = 0;
			vr->subcmd = 0;

			if (nlattrs[NL80211_ATTR_VENDOR_ID])
				vr->oui = nla_get_u32(nlattrs[NL80211_ATTR_VENDOR_ID]);

			if (nlattrs[NL80211_ATTR_VENDOR_SUBCMD])
				vr->subcmd = nla_get_u32(nlattrs[NL80211_ATTR_VENDOR_SUBCMD]);

			if (nlattrs[NL80211_ATTR_VENDOR_DATA]) {
				data = nla_data(nlattrs[NL80211_ATTR_VENDOR_DATA]);
				len = nla_len(nlattrs[NL80211_ATTR_VENDOR_DATA]);

				if (resp->len > len + 8) {
					memcpy(vr->data, data, len);
					resp->len = len;
				} else
					resp->len = 0;
			}
		}
		break;
	default:
		break;
	}

	if (resp->type) {
		if (wevt->req.override_cb)
			wevt->req.override_cb(&wevt->req);
		else if (wevt->req.cb)
			wevt->req.cb(&wevt->req);
	}

	return 0;
}

static struct nlwifi_event_struct {
	const char *family;
	const char *grp;
	int (*handler)(struct nl_msg *msg, void *arg);
} nlwifi_events[] = {
	{
		.family = "nl80211",
		.grp = "scan",
		.handler = nlwifi_event_handler,
	},
	{
		.family = "nl80211",
		.grp = "config",
		.handler = nlwifi_event_handler,
	},
	{
		.family = "nl80211",
		.grp = "mlme",
		.handler = nlwifi_event_handler,
	},
	{
		.family = "nl80211",
		.grp = "vendor",
		.handler = nlwifi_event_handler,
	},
};

#define nlwifi_events_num	(sizeof(nlwifi_events)/sizeof(nlwifi_events[0]))

int nlwifi_register_event(const char *ifname, struct event_struct *req, void **handle)
{
	struct nlwifi_event *wev;
	struct nl_sock *sock;
	struct nlwifi_event_struct *ev;
	int grp;
	int err;
	int i;

	libwifi_dbg("%s %s called family %s group %s\n", ifname, __func__, req->family, req->group);

	for (i = 0; i < nlwifi_events_num; i++) {
		ev = &nlwifi_events[i];
		if (!strncmp(ev->family, req->family, strlen(ev->family)))
			break;
	}

	if (i > nlwifi_events_num) {
		libwifi_err("Error! Unknown event family '%s'\n", req->family);
		return -1;
	}

	wev = calloc(1, sizeof(struct nlwifi_event));
	if (!wev) {
		libwifi_err("%s: malloc failed!\n", __func__);
		return -1;
	}

	memcpy(&wev->req, req, sizeof(struct event_struct));
	sock = nl_socket_alloc();
	if (!sock) {
		libwifi_err("%s: nl_socket_alloc\n", __func__);
		goto free_handle;
	}
	wev->sock = sock;

	nl_socket_disable_seq_check(sock);
	nl_socket_modify_cb(sock, NL_CB_VALID, NL_CB_CUSTOM,
			    ev->handler, wev);

	if ((err = genl_connect(sock)) < 0) {
		libwifi_err("%s: %s\n", __func__, nl_geterror(err));
		goto free_sock;
	}

	if ((grp = genl_ctrl_resolve_grp(sock, req->family, req->group)) < 0) {
		libwifi_err("%s: %s (family '%s', group '%s')\n",
					__func__,
					nl_geterror(grp),
					req->family,
					req->group);
		goto free_sock;
	}

	nl_socket_add_membership(sock, grp);
	req->fd_monitor = nl_socket_get_fd(sock);
	*handle = wev;

	return 0;

free_sock:
	nl_socket_free(sock);
free_handle:
	free(wev);
	*handle = NULL;
	return -1;
}

int nlwifi_unregister_event(const char *ifname, void *handle)
{
	struct nlwifi_event *wev = (struct nlwifi_event *)handle;
	struct event_struct *req;
	int ret = -1;
	int grp;

	if (!wev)
		return ret;

	req = &wev->req;
	grp = genl_ctrl_resolve_grp(wev->sock, req->family, req->group);
	if (grp < 0) {
		libwifi_err("%s: %s ('%s', '%s')\n",
					__func__,
					nl_geterror(grp),
					req->family,
					req->group);
		goto free_sock;
	}

	ret = 0;
	nl_socket_drop_membership(wev->sock, grp);

free_sock:
	nl_socket_free(wev->sock);
	free(wev);
	return ret;
}

int nlwifi_recv_event(const char *ifname, void *handle)
{
	struct nlwifi_event *wev = (struct nlwifi_event *)handle;
	int err;

	UNUSED(ifname);

	err = nl_recvmsgs_default(wev->sock);
	if (err < 0) {
		libwifi_err("%s: %s\n", __func__, nl_geterror(err));
		return err;
	}

	return 0;
}

int nlwifi_driver_info(const char *name, struct wifi_metainfo *info)
{
	char phy[16] = {};
	char path[256] = {};
	char *pos;
	int fd;

	libwifi_dbg("%s %s called\n", name, __func__);

	if (WARN_ON(nlwifi_get_phy(name, phy, sizeof(phy))))
		return -1;

	snprintf(path, sizeof(path), "/sys/class/ieee80211/%s/device/vendor", phy);
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

	snprintf(path, sizeof(path), "/sys/class/ieee80211/%s/device/device", phy);
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

int nlwifi_start_cac(const char *ifname, int channel, enum wifi_bw bw,
		     enum wifi_cac_method method)
{
	int ht40plus[] = { 36, 44, 52, 60, 100, 108, 116, 124, 132, 149, 157, 184, 192 };
	struct nl_sock *nl;
	struct nl_msg *msg;
	int devidx;
	enum nl80211_chan_width width;
	uint32_t freq;
	uint32_t cf1;
	int ret = -1;
	const int *chans;
	int i;

	/* Support only background CAC */
	if (method != WIFI_CAC_MIMO_REDUCED)
		return ret;

	/* no DFS/CAC for 2.4GHz */
	if (channel < 36)
		return ret;

	freq = wifi_channel_to_freq(channel);

	switch (bw) {
	case BW20:
		width = NL80211_CHAN_WIDTH_20;
		break;
	case BW40:
		width = NL80211_CHAN_WIDTH_40;
		break;
	case BW80:
		width = NL80211_CHAN_WIDTH_80;
		break;
	default:
		return -1;
	}

	devidx = if_nametoindex(ifname);
	if (!devidx) {
		ret = -errno;
		goto out;
	}

	nl = nlwifi_socket();
	if (!nl)
		goto out;

	msg = nlwifi_alloc_msg(nl, NL80211_CMD_RADAR_DETECT, 0, 0);
	if (!msg)
		goto out_msg_failure;

	/* prepare cmd */
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, devidx);


	NLA_PUT_U32(msg, NL80211_ATTR_WIPHY_FREQ, freq);
	NLA_PUT_U32(msg, NL80211_ATTR_CHANNEL_WIDTH, width);
	NLA_PUT_FLAG(msg, NL80211_ATTR_RADAR_BACKGROUND);

	switch (width) {
	case NL80211_CHAN_HT20:
		NLA_PUT_U32(msg, NL80211_ATTR_WIPHY_CHANNEL_TYPE, NL80211_CHAN_HT20);
		break;
	case NL80211_CHAN_WIDTH_40:
		for (i = 0; i < ARRAY_SIZE(ht40plus); i++) {
			if (channel == ht40plus[i])
				break;
		}

		if (i == ARRAY_SIZE(ht40plus))
			NLA_PUT_U32(msg, NL80211_ATTR_WIPHY_CHANNEL_TYPE, NL80211_CHAN_HT40MINUS);
		else
			NLA_PUT_U32(msg, NL80211_ATTR_WIPHY_CHANNEL_TYPE, NL80211_CHAN_HT40PLUS);
		break;
	case NL80211_CHAN_WIDTH_80:
		chans = chan2list(channel, 80);
		if (!chans)
			goto nla_put_failure;

		cf1 = wifi_channel_to_freq(*chans + 6);
		NLA_PUT_U32(msg, NL80211_ATTR_CENTER_FREQ1, cf1);
		break;
	case NL80211_CHAN_WIDTH_160:
		chans = chan2list(channel, 160);
		if (!chans)
			goto nla_put_failure;

		cf1 = wifi_channel_to_freq(*chans + 14);
		NLA_PUT_U32(msg, NL80211_ATTR_CENTER_FREQ1, cf1);
		break;
	default:
		break;
	}

	/* send cmd */
	ret = nlwifi_send_msg(nl, msg, NULL, NULL);
	nlmsg_free(msg);
	nl_socket_free(nl);
	return ret;

nla_put_failure:
	nlmsg_free(msg);
out_msg_failure:
	nl_socket_free(nl);
out:
	return ret;
}

int nlwifi_stop_cac(const char *name)
{
	/* Seems today not supported by nl80211/cfg80211 */
	return -1;
}

const struct wifi_driver nlwifi_driver = {
	.name = "wlan",
	.radio.is_multiband = nlwifi_radio_is_multiband,
        .info = nlwifi_driver_info,
	.scan = nlwifi_scan,
	.scan_ex = nlwifi_scan_ex,
	.get_scan_results = nlwifi_get_scan_results,
	.get_bssid = nlwifi_get_bssid,
	.get_ssid = nlwifi_get_ssid,
	.get_channel = nlwifi_get_channel,
	.radio.get_band_channel = nlwifi_get_band_channel,
	.get_supp_channels = nlwifi_get_supp_channels,
	.get_bandwidth = nlwifi_get_bandwidth,
	.vendor_cmd = nlwifi_vendor_cmd,
	.iface.get_caps = nlwifi_ap_get_caps,
	.get_assoclist = nlwifi_get_assoclist,
	.get_sta_info = nlwifi_get_sta_info,
	.radio.get_caps = nlwifi_radio_get_caps,
	.radio.get_band_caps = nlwifi_radio_get_band_caps,
	.radio.info = nlwifi_radio_info,
	.radio.info_band = nlwifi_radio_info_band,
	.get_country = nlwifi_get_country,
	.register_event = nlwifi_register_event,
	.unregister_event = nlwifi_unregister_event,
	.recv_event = nlwifi_recv_event,
	.get_noise = nlwifi_get_noise,
	.radio.get_band_noise = nlwifi_get_band_noise,
	.radio.get_supp_stds = nlwifi_get_supp_stds,
	.radio.get_band_supp_stds = nlwifi_get_band_supp_stds,
	.channels_info = nlwifi_channels_info,
	.radio.channels_info_band = nlwifi_channels_info_band,
	.get_4addr = nlwifi_get_4addr,
	.start_cac = nlwifi_start_cac,
	.stop_cac = nlwifi_stop_cac,
};
