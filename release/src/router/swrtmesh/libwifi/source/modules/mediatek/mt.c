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
#include <linux/ethtool.h>
#include <linux/sockios.h>
#include <errno.h>
#include <ctype.h>
#include <linux/limits.h>

#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "debug.h"
#include "drivers.h"

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
	vsnprintf(str, sizeof(str), fmt, ap);
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

	strcpy(req.ifr_name, ifname);
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

	if (nlwifi_get_phy_wifi_ifaces(name, ifaces, &num) < 0) {
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

const struct wifi_driver mt_driver = {
	.name = "wlan,phy",

	/* Radio/phy callbacks */
	.simulate_radar = radio_simulate_radar,

	.radio.get_stats = radio_get_stats,

	/* Interface/vif common callbacks */

	/* Interface/vif ap callbacks */

	.iface.get_stats = iface_get_stats,

	/* Interface/vif sta callbacks */

	/* use fallback driver ops */
	.fallback = &mac80211_driver,
};
