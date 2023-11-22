/*
 * mt.c - for mac80211 based mt76 drivers.
 *
 * Copyright (C) 2020-2023 iopsys Software Solutions AB. All rights reserved.
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
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <errno.h>
#include <ctype.h>
#include <linux/limits.h>
#include <linux/nl80211.h>
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <netlink/genl/genl.h>

#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "debug.h"
#include "drivers.h"

#ifdef MT7915_VENDOR_EXT
#include "mt7915/vendor.h"
#endif

static int mt76_simulate_radar_debugfs(const char *phyname, struct wifi_radar_args *radar)
{
	char path[512] = {};
	int fd;
	int char_num;

	char_num = snprintf(path, sizeof(path), "/sys/kernel/debug/ieee80211/%s/mt76/radar_trigger", phyname);
	if (WARN_ON(char_num < 0))
		return -1;

	if (WARN_ON(char_num >= (int)sizeof(path)))
		return -1;

	fd = open(path, O_WRONLY);
	if (WARN_ON(fd < 0))
		return -1;

	/* Trigger radar
	 * echo 1 > /sys/kernel/debug/ieee80211/phy1/mt76/radar_trigger
	 */
	if (!write(fd, "1", 1)) {
		close(fd);
		return -1;
	}

	close(fd);
	return 0;
}

int radio_simulate_radar(const char *name, struct wifi_radar_args *radar)
{
	if (WARN_ON(!radar))
		return -1;

	libwifi_dbg("[%s] %s called ch:%d, bandwidth:%d, type:0x%x, subband:0x%x\n",
		    name, __func__, radar->channel, radar->bandwidth, radar->type,
		    radar->subband_mask);

	return mt76_simulate_radar_debugfs(name, radar);
}


#if defined(MT_USE_ETHTOOL_STATS) || !defined(MT7915_VENDOR_EXT)
/* Statistics available via SIOCETHTOOL */
enum {
	ET_TX_BYTES = 0,
	ET_RX_BYTES,
	ET_TX_PACKETS,
	ET_RX_PACKETS,
	ET_TX_RETRY_FAILED,
	ET_TX_RETRIES,
	ET_TX_AMPDU_CNT,
	ET_TX_RWP_FAIL_CNT,
	ET_RX_DROPPED,
	ET_RX_FIFO_FULL_CNT,
	ET_RX_DELIMETER_FAIL_CNT,
	ET_RX_VECTOR_MISMATCH_CNT,
	ET_RX_LEN_MISMATCH_CNT,
	ET_NOISE
};

/* Map enum members to strings */
static const char *et_e2s[] = {
	"tx_bytes", "rx_bytes", "tx_packets", "rx_packets",
	"tx_retry_failed", "tx_retries", "tx_ampdu_cnt", "tx_rwp_fail_cnt",
	"rx_dropped", "rx_fifo_full_cnt", "rx_delimiter_fail_cnt",
	"rx_vector_mismatch_cnt", "rx_len_mismatch_cnt", "noise"
};

/* enum/stats array index mapping per band, to be initialized dynamically */
static uint16_t *et_e2idx[4];

/* Number of elements in the stats array per band */
static size_t et_n_stats[4];

/* Get band index for the interface */
static inline int
et_bidx_get(const char *iface)
{
	uint32_t band;

	if (nlwifi_get_supp_band(iface, &band) < 0)
		return -1;

	return band == BAND_2 ? 0 : band == BAND_5 ? 1 :
			band == BAND_60 ? 2 : band == BAND_6 ? 3 : -1;
}

/* Socket for ioctl() calls */
static int ioctl_fd = -1;

static int et_map_init_done;

static void
et_map_init(void)
{
	int i, k, j;

	if ((ioctl_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
		libwifi_dbg("socket(): %s\n", strerror(errno));
		return;
	}

	for (i = 0; i < 10; i++)
	{
		char phy[5];
		struct ethtool_drvinfo drvinfo = {.cmd = ETHTOOL_GDRVINFO};
		struct ifreq req = {.ifr_data = (void *)&drvinfo};
		struct ethtool_gstrings *strings;
		int bidx;

		sprintf(phy, "phy%d", i);
		if (nlwifi_phy_to_netdev(phy, req.ifr_name, sizeof(req.ifr_name)) < 0 ||
		    *(req.ifr_name) == 0)
			return;

		if (ioctl(ioctl_fd, SIOCETHTOOL, &req) < 0) {
			libwifi_dbg("ioctl(%s, SIOCETHTOOL) failed: %s\n",
						req.ifr_name, strerror(errno));
			goto err;
		}

		if ((bidx = et_bidx_get(req.ifr_name)) < 0) {
			libwifi_dbg("Failed to detect band for %s", req.ifr_name);
			goto err;
		}

		strings = calloc(1, sizeof(*strings) + drvinfo.n_stats * ETH_GSTRING_LEN);
		if (strings == NULL)
			goto err;
		strings->cmd = ETHTOOL_GSTRINGS;
		strings->string_set = ETH_SS_STATS;
		strings->len = drvinfo.n_stats;
		req.ifr_data = (void *)strings;
		if (ioctl(ioctl_fd, SIOCETHTOOL, &req) < 0) {
			libwifi_dbg("ioctl(%s, SIOCETHTOOL) failed: %s\n",
						req.ifr_name, strerror(errno));
			continue;
		}

		et_e2idx[bidx] = calloc(1, drvinfo.n_stats);
		if (et_e2idx[bidx] == NULL) {
			free(strings);
			goto err;
		}
		for (k = 0; k < sizeof(et_e2s) / sizeof(et_e2s[0]); k++) {
			for (j = 0;
				j < drvinfo.n_stats &&
					strcmp((char *)(strings->data + j * ETH_GSTRING_LEN),
							et_e2s[k]) != 0;
				j++);

			if (j == drvinfo.n_stats) {
				libwifi_dbg("Failed to find %s counter for %s", et_e2s[k],
							req.ifr_name);
				free(strings);
				free(et_e2idx[bidx]);
				goto next;
			}
			*(et_e2idx[bidx] + k) = j;
		}
		et_n_stats[bidx] = (size_t)drvinfo.n_stats;
		next:
		continue;
	}

	return;

err:
	for (i = 0; i < sizeof(et_n_stats) / sizeof(et_n_stats[0]); i++)
		free(et_e2idx[i]);
	close(ioctl_fd);
	ioctl_fd = -1;
}

static uint64_t
read_int_from_file(const char *fmt, ...)
{
	char str[PATH_MAX], *tmp;
	va_list  ap;
	FILE    *f;
	uint64_t res;

	va_start(ap, fmt);
	vsnprintf(str, sizeof(str), fmt, ap); /* Flawfinder: ignore */
	va_end(ap);

	if ((f = fopen(str, "r")) == NULL)
		return 0;

	tmp = fgets(str, sizeof(str), f);
	fclose(f);
	if (tmp == NULL)
		return 0;

	res = strtoll(str, &tmp, 10);
	if (tmp == str || (*tmp != 0 && !isspace(*tmp)))
		return 0;

	return res;
}

static int iface_get_stats_internal(const char *ifname, const char *phy,
									struct wifi_ap_stats *s,
									int *p_bidx, struct ethtool_stats **p_stats)
{
	int bidx;
	struct ifreq req;
	struct ethtool_stats *stats;
	uint64_t *cnt;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	memset(s, 0, sizeof(*s));
	*p_stats = NULL;

	if (!et_map_init_done) {
		et_map_init();
		et_map_init_done = 1;
	}

	if (ioctl_fd < 0)
		return 0;

	if ((bidx = et_bidx_get(ifname)) < 0 || et_n_stats[bidx] == 0)
		return 0;

	stats = calloc(1, sizeof(*stats) +  et_n_stats[bidx] * sizeof(uint64_t));
	if (stats == NULL)
		return -1;

	*p_bidx = bidx;
	*p_stats = stats;

	strcpy(req.ifr_name, ifname); /* Flawfinder: ignore */
	stats->cmd = ETHTOOL_GSTATS;
	stats->n_stats = et_n_stats[bidx];
	req.ifr_data = (void *)stats;
	if (ioctl(ioctl_fd, SIOCETHTOOL, &req) < 0) {
		libwifi_dbg("ioctl(%s, SIOCETHTOOL) failed: %s\n",
					req.ifr_name, strerror(errno));
		return 0;
	}
	cnt = (uint64_t *)(((uint8_t *)stats) + sizeof(*stats));

	s->tx_bytes = cnt[et_e2idx[bidx][ET_TX_BYTES]];
	s->rx_bytes = cnt[et_e2idx[bidx][ET_RX_BYTES]];
	s->tx_pkts = cnt[et_e2idx[bidx][ET_TX_PACKETS]];
	s->rx_pkts = cnt[et_e2idx[bidx][ET_RX_PACKETS]];
	s->tx_err_pkts = cnt[et_e2idx[bidx][ET_TX_RETRY_FAILED]];
	s->tx_rtx_pkts = cnt[et_e2idx[bidx][ET_TX_RETRIES]];
	s->tx_rtx_fail_pkts = cnt[et_e2idx[bidx][ET_TX_RETRY_FAILED]];
	s->aggr_pkts = cnt[et_e2idx[bidx][ET_TX_AMPDU_CNT]];
	s->rx_dropped_pkts = cnt[et_e2idx[bidx][ET_RX_DROPPED]];

	s->rx_mcast_pkts =
		read_int_from_file("/sys/class/net/%s/statistics/multicast", ifname);

	s->tx_retry_pkts = read_int_from_file(
		"/sys/kernel/debug/ieee80211/%s/statistics/dot11RTSSuccessCount", phy);
	s->ack_fail_pkts = read_int_from_file(
		"/sys/kernel/debug/ieee80211/%s/statistics/dot11ACKFailureCount", phy);

	return 0;
}

static int get_phyname(const char *ifname, char *phy)
{
	uint64_t index = read_int_from_file("/sys/class/net/%s/phy80211/index", ifname);
	sprintf(phy, "phy%lu", index);
	return 0;
}

static int iface_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	int bidx;
	struct ethtool_stats *stats;
	char phy[8];
	int  ret;

	if (get_phyname(ifname, phy) != 0)
		return -1;

	memset(s, 0, sizeof(*s));

	ret = iface_get_stats_internal(ifname, phy, s, &bidx, &stats);

	if (stats != NULL)
		free(stats);

	return ret;
}

static int radio_get_stats(const char *name, struct wifi_radio_stats *s)
{
	struct wifi_iface ifaces[WIFI_IFACE_MAX_NUM];
	uint8_t num = WIFI_IFACE_MAX_NUM, i;

	libwifi_dbg("[%s] %s called\n", name, __func__);

	if (nlwifi_get_phy_wifi_ifaces(name, BAND_ANY, ifaces, &num) < 0) {
		libwifi_dbg("Failed to get list of interfaces of %s\n", name);
		return -1;
	}

	memset(s, 0, sizeof(*s));

	for (i = 0; i < num; i++) {
		int bidx;
		struct wifi_ap_stats si;
		struct ethtool_stats *stats;
		uint64_t *cnt;

		if (iface_get_stats_internal(ifaces[i].name, name,
									 &si, &bidx, &stats) < 0 || stats == NULL)
			continue;

		s->tx_bytes += si.tx_bytes;
		s->rx_bytes += si.rx_bytes;
		s->tx_pkts += si.tx_pkts;
		s->rx_pkts += si.rx_pkts;
		s->rx_dropped_pkts += si.rx_dropped_pkts;

		/* PHY stats may be got from any netdev */
		cnt = (uint64_t *)(((uint8_t *)stats) + sizeof(*stats));
		s->tx_err_pkts = cnt[et_e2idx[bidx][ET_TX_RWP_FAIL_CNT]];
		s->rx_err_pkts = cnt[et_e2idx[bidx][ET_RX_FIFO_FULL_CNT]] +
						cnt[et_e2idx[bidx][ET_RX_DELIMETER_FAIL_CNT]] +
						cnt[et_e2idx[bidx][ET_RX_VECTOR_MISMATCH_CNT]] +
						cnt[et_e2idx[bidx][ET_RX_LEN_MISMATCH_CNT]];
		s->noise = cnt[et_e2idx[bidx][ET_NOISE]];

		free(stats);
	}

	s->rx_fcs_err_pkts = read_int_from_file(
		"/sys/kernel/debug/ieee80211/%s/statistics/dot11FCSErrorCount", name);

	return 0;
}

#else /* !MT_USE_ETHTOOL_STATS */

struct wifi_iface_common_stats {
       WIFI_IF_COMMON_STATS
};

static int
read_int_from_file(const char *fmt, ...)
{
	char str[PATH_MAX], *tmp;
	va_list  ap;
	FILE    *f;
	int      res;

	va_start(ap, fmt);
	vsnprintf(str, sizeof(str), fmt, ap); /* Flawfinder: ignore */
	va_end(ap);

	if ((f = fopen(str, "r")) == NULL)
		return -1;

	tmp = fgets(str, sizeof(str), f);
	fclose(f);
	if (tmp == NULL)
		return -1;

	res = strtol(str, &tmp, 10);
	if (tmp == str || (*tmp != 0 && !isspace(*tmp)))
		return -1;

	return res;
}

static int iface_to_phy_index(const char *ifname)
{
	return read_int_from_file("/sys/class/net/%s/phy80211/index", ifname);
}

static int phy_name_to_index(const char *phyname)
{
	return read_int_from_file("/sys/class/ieee80211/%s/index", phyname);
}

static int if_stats_cb(struct nl_msg *msg, struct wifi_iface_common_stats *s,
						int sta)
{
	struct wifi_sta_ifstats *ifst = (struct wifi_sta_ifstats *)s;
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *nl_resp, *nl_iter;
	int iter = 0;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	nl_resp = tb[NL80211_ATTR_VENDOR_DATA];
	if (!nl_resp) {
		return NL_SKIP;
	}

	nla_for_each_nested(nl_iter, nl_resp, iter) {
		switch (nla_type(nl_iter))
		{
#define GET_ATTR(_attr, _field) \
		case MTK_VENDOR_ATTR_IFSTATS_DUMP_##_attr: \
			s->_field = *(uint64_t *)nla_data(nl_iter); break

			GET_ATTR(TX_BYTES, tx_bytes);
			GET_ATTR(RX_BYTES, rx_bytes);
			GET_ATTR(TX_PKTS, tx_pkts);
			GET_ATTR(RX_PKTS, rx_pkts);
			GET_ATTR(TX_ERR_PKTS, tx_err_pkts);
			GET_ATTR(TX_RTX_PKTS, tx_rtx_pkts);
			GET_ATTR(TX_RTX_FAIL_PKTS, tx_rtx_fail_pkts);
			GET_ATTR(TX_RETRY_PKTS, tx_retry_pkts);
			GET_ATTR(TX_MRETRY_PKTS, tx_mretry_pkts);
			GET_ATTR(ACK_FAIL_PKTS, ack_fail_pkts);
			GET_ATTR(AGGR_PKTS, aggr_pkts);
			GET_ATTR(RX_ERR_PKTS, rx_err_pkts);
			GET_ATTR(TX_UCAST_PKTS, tx_ucast_pkts);
			GET_ATTR(RX_UCAST_PKTS, rx_ucast_pkts);
			GET_ATTR(TX_DROPPED_PKTS, tx_dropped_pkts);
			GET_ATTR(RX_DROPPED_PKTS, rx_dropped_pkts);
			GET_ATTR(TX_MCAST_PKTS, tx_mcast_pkts);
			GET_ATTR(RX_MCAST_PKTS,rx_mcast_pkts);
			GET_ATTR(TX_BCAST_PKTS, tx_bcast_pkts);
			GET_ATTR(RX_BCAST_PKTS, rx_bcast_pkts);
			GET_ATTR(RX_UNKNOWN_PKTS, rx_unknown_pkts);
			GET_ATTR(TX_BUF_OVERFLOW, tx_buf_overflow);
			GET_ATTR(TX_STA_NOT_ASSOC, tx_sta_not_assoc);
			GET_ATTR(TX_FRAGS, tx_frags);
			GET_ATTR(TX_NO_ACK_PKTS, tx_no_ack_pkts);
			GET_ATTR(RX_DUP_PKTS, rx_dup_pkts);
			GET_ATTR(RX_TOO_LONG_PKTS, tx_too_long_pkts);
			GET_ATTR(TX_TOO_SHORT_PKTS, tx_too_short_pkts);
			GET_ATTR(UCAST_ACK, ucast_ack);
#undef GET_ATTR
#define GET_ATTR(_attr, _field, _type) \
		case MTK_VENDOR_ATTR_IFSTATS_DUMP_##_attr: \
			if (sta) { \
				ifst->_field = (_type)(*(uint64_t *)nla_data(nl_iter)); \
			} \
			break

			GET_ATTR(LAST_DL_RATE, last_dl_rate, uint32_t);
			GET_ATTR(LAST_UL_RATE, last_ul_rate, uint32_t);
			GET_ATTR(SIGNAL, signal, int8_t);
			GET_ATTR(RETRANS_100, retrans_100, uint8_t);
			default: break;
#undef GET_ATTR
		}
	}

	return NL_SKIP;
}


static int apif_stats_cb(struct nl_msg *msg, void *arg)
{
	return if_stats_cb(msg, arg, 0);
}

static int staif_stats_cb(struct nl_msg *msg, void *arg)
{
	return if_stats_cb(msg, arg, 1);
}

static int if_get_stats(const char *ifname, struct wifi_iface_common_stats *s,
							void *cb)
{
	int ifindex = if_nametoindex(ifname), pindex;
	struct nl_msg *msg = NULL;
	struct nl_sock *nl;

	memset(s, 0, sizeof(*s));

	if (ifindex == 0) {
		libwifi_dbg("Failed to find interface %s\n", ifname);
		return 0;
	}

	if ((pindex = iface_to_phy_index(ifname)) < 0) {
		libwifi_dbg("Failed to find phy for interface %s\n", ifname);
		return 0;
	}

	nl = nlwifi_socket();
	if (!nl)
		return 0;

	msg = nlwifi_alloc_msg(nl, NL80211_CMD_VENDOR, NLM_F_DUMP, 0);
	if (!msg)
		goto nla_put_failure;

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, pindex);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, MTK_NL80211_VENDOR_ID);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD,
				MTK_NL80211_VENDOR_SUBCMD_IFSTATS);
	NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, pindex);
	NLA_PUT_U32(msg, NL80211_ATTR_IFINDEX, ifindex);

	nlwifi_send_msg(nl, msg, cb, s);

nla_put_failure:
	if (msg != NULL)
		nlmsg_free(msg);
	nl_socket_free(nl);
	return 0;
}

static int apif_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	return if_get_stats(ifname, (struct wifi_iface_common_stats *)s,
						apif_stats_cb);
}

static int staif_get_stats(const char *ifname, struct wifi_sta_ifstats *s)
{
	return if_get_stats(ifname, (struct wifi_iface_common_stats *)s,
								staif_stats_cb);
}

static int radio_stats_cb(struct nl_msg *msg, void *arg)
{
	struct wifi_radio_stats *s = arg;
	struct genlmsghdr *gnlh = nlmsg_data(nlmsg_hdr(msg));
	struct nlattr *tb[NL80211_ATTR_MAX + 1];
	struct nlattr *nl_resp, *nl_iter;
	int iter = 0;

	nla_parse(tb, NL80211_ATTR_MAX, genlmsg_attrdata(gnlh, 0),
			genlmsg_attrlen(gnlh, 0), NULL);

	nl_resp = tb[NL80211_ATTR_VENDOR_DATA];
	if (!nl_resp) {
		return NL_SKIP;
	}

	nla_for_each_nested(nl_iter, nl_resp, iter) {
		switch (nla_type(nl_iter))
		{
#define GET_ATTR(_attr, _field, _type) \
		case MTK_VENDOR_ATTR_RSTATS_DUMP_##_attr: \
			s->_field = *(uint##_type##_t *)nla_data(nl_iter); break
			GET_ATTR(CTS_RCVD, cts_rcvd, 64);
			GET_ATTR(CTS_NOT_RCVD, cts_not_rcvd, 64);
			GET_ATTR(RX_FRAME_ERR_PKTS, rx_frame_err_pkts, 64);
			GET_ATTR(RX_GOOG_PLCP_PKTS, rx_good_plcp_pkts, 64);
			GET_ATTR(RX_OMAC_DATA_PKTS, omac_data_pkts, 64);
			GET_ATTR(RX_OMAC_MGMT_PKTS, omac_mgmt_pkts, 64);
			GET_ATTR(RX_OMAC_CTRL_PKTS, omac_ctrl_pkts, 64);
			GET_ATTR(RX_OMAC_CTS, omac_cts, 64);
			GET_ATTR(RX_OMAC_RTS, omac_rts, 64);
			GET_ATTR(TX_BYTES, tx_bytes, 64);
			GET_ATTR(RX_BYTES, rx_bytes, 64);
			GET_ATTR(TX_PKTS, tx_pkts, 64);
			GET_ATTR(RX_PKTS, rx_pkts, 64);
			GET_ATTR(TX_ERR_PKTS, tx_err_pkts, 32);
			GET_ATTR(RX_ERR_PKTS, rx_err_pkts, 32);
			GET_ATTR(TX_DROPPED_PKTS, tx_dropped_pkts, 32);
			GET_ATTR(RX_DROPPED_PKTS, rx_dropped_pkts, 32);
			GET_ATTR(RX_PLCP_ERR_PKTS, rx_plcp_err_pkts, 32);
			GET_ATTR(RX_FCS_ERR_PKTS, rx_fcs_err_pkts, 32);
			GET_ATTR(RX_MAC_ERR_PKTS, rx_mac_err_pkts, 32);
			GET_ATTR(RX_UNKNOWN_PKTS, rx_unknown_pkts, 32);
			GET_ATTR(NOISE, noise, 16);
			
			default: break;
		}
#undef GET_ATTR
	}

	return NL_SKIP;
}

static int radio_get_stats(const char *name, struct wifi_radio_stats *s)
{
	int pindex = phy_name_to_index(name);
	struct nl_msg *msg = NULL;
	struct nl_sock *nl;

	memset(s, 0, sizeof(*s));

	if (pindex < 0) {
		libwifi_dbg("Failed to find phy %s\n", name);
		return 0;
	}

	nl = nlwifi_socket();
	if (!nl)
		return 0;

	msg = nlwifi_alloc_msg(nl, NL80211_CMD_VENDOR, NLM_F_DUMP, 0);
	if (!msg)
		goto nla_put_failure;

	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, pindex);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_ID, MTK_NL80211_VENDOR_ID);
	NLA_PUT_U32(msg, NL80211_ATTR_VENDOR_SUBCMD,
				MTK_NL80211_VENDOR_SUBCMD_RSTATS);
	NLA_PUT_U32(msg, NL80211_ATTR_WIPHY, pindex);

	nlwifi_send_msg(nl, msg, radio_stats_cb, s);

nla_put_failure:
	if (msg != NULL)
		nlmsg_free(msg);
	nl_socket_free(nl);
	return 0;
}

#endif

const struct wifi_driver mt_driver = {
	.name = "wlan,phy",

	/* Radio/phy callbacks */
	.simulate_radar = radio_simulate_radar,

	.radio.get_stats = radio_get_stats,

	/* Interface/vif common callbacks */

#if !defined(MT_USE_ETHTOOL_STATS) && defined(MT7915_VENDOR_EXT)
	/* Interface/vif ap callbacks */
	.iface.get_stats = apif_get_stats,

	/* Interface/vif sta callbacks */
	.iface.sta_get_ifstats = staif_get_stats,
#endif

	/* use fallback driver ops */
	.fallback = &mac80211_driver,
};
