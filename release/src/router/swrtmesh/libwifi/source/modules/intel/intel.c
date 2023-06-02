/*
 * intel.c - implements for Intel (mtlk) wifi drivers
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
#include <stdarg.h>
#include <stdint.h>
#include <errno.h>
#include <sys/types.h>
#include <fcntl.h>
#include <net/if.h>
#include <netlink/genl/genl.h>
#include <netlink/genl/ctrl.h>
#include <netpacket/packet.h>
#include <linux/nl80211.h>

/* header for Intel's vendor cmds */
#include <vendor_cmds.h>

#include "debug.h"
#include "util.h"
#include "wifi.h"
#include "wext.h"
#include "nlwifi.h"
#include "intel.h"

static uint8_t wmode2std[] = {
	[0] = WIFI_B,
	[1] = WIFI_G,
	[2] = WIFI_N,
	[3] = WIFI_B | WIFI_G,
	[4] = WIFI_G | WIFI_N,
	[5] = WIFI_B | WIFI_G | WIFI_N,
	[6] = WIFI_A,
	[7] = WIFI_N,
	[8] = WIFI_A | WIFI_N,
	[9] = WIFI_A | WIFI_B | WIFI_G,
	[10] = WIFI_A | WIFI_B | WIFI_G | WIFI_N,
	[11] = WIFI_A | WIFI_N | WIFI_AC,
};

int intel_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	char sbuf[512] = {0};
	mtlk_sta_info_t s;
	int slen = 0;
	int ret;

	if (!addr) {
		libwifi_err("Invalid args!\n");
		return -EINVAL;
	}

	nlwifi_get_sta_info(ifname, addr, info);

	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
			LTQ_NL80211_VENDOR_SUBCMD_GET_STA_MEASUREMENTS,
			addr, 6, (uint8_t *)&s, &slen);

	if (!ret && slen == sizeof(mtlk_sta_info_t)) {
		/* update with vendor fetched data */
		info->rssi_avg = s.peer_stats.SignalStrength;
		info->rssi[0] = s.peer_stats.ShortTermRSSIAverage[0];
		info->rssi[1] = s.peer_stats.ShortTermRSSIAverage[1];
		info->rssi[2] = s.peer_stats.ShortTermRSSIAverage[2];
		info->rssi[3] = s.peer_stats.ShortTermRSSIAverage[3];

		/* XXX: antenna[0] is NC for 2.4Ghz 3x3 as
		 * it returns value -128.
		 */
		if (info->rssi[0] == -128) {
			info->rssi[0] = info->rssi[1];
			info->rssi[1] = info->rssi[2];
			info->rssi[2] = info->rssi[3];
			info->rssi[3] = 0;
		}

		info->tx_rate.rate = s.peer_stats.LastDataUplinkRate;
		info->rx_rate.rate = s.peer_stats.LastDataDownlinkRate;

		info->stats.tx_rtx_pkts = s.peer_stats.Retransmissions;
		info->stats.tx_bytes = s.peer_stats.traffic_stats.BytesSent;
		info->stats.rx_bytes = s.peer_stats.traffic_stats.BytesReceived;
		info->stats.tx_pkts = s.peer_stats.traffic_stats.PacketsSent;
		info->stats.rx_pkts = s.peer_stats.traffic_stats.PacketsReceived;
	}

	/* Wait.. we don't have the sta capabilities yet!
	 * Parse the (re)assoc request frame last received by hostapd and try
	 * to get capabilities of this sta.
	 */
	chrCmd(sbuf, sizeof(sbuf),
		"hostapd_cli -i %s get_last_assoc_req " MACFMT " | cut -d'=' -f2",
		ifname, MAC2STR(addr));
	if (sbuf[0] != '\0') {
		int pos = 10;
		uint8_t buf[256] = {0};
		int reassoc = 0;

		/* libwifi_dbg("sta last assoc req: %s\n", sbuf); */
		strtob(sbuf, sizeof(sbuf), buf);
		reassoc = buf[0] == 0x20 ? 1 : 0;
		if (!memcmp(&buf[pos], addr, 6)) {
			char ssid[32] = {0};
			uint8_t cap[2] = {0};
			uint8_t *ie_start;
			uint8_t *vht_ie, *ht_ie;
			uint8_t *ext_ie, *rrm_ie, *ft_ie;
			int matched = 0;
			/* following for max phyrate calculations */
			//int phy_mode = 0;
			int bw = 20;
			int sgi = 0;
			int nss = 0;
			int max_mcs = -1;
			uint8_t *supp_mcs = NULL;
			int octet;
			int l;

#ifndef bit
#define bit(_n)	(1 << (_n))
#endif
			/* Addr2 matched; lookahead for ssid and jump to
			 * capability info.
			 * Then iterate through the ies for ht/vht_caps
			 * elements.
			 */
			pos += 14;
			cap[0] = buf[pos];
			cap[1] = buf[pos + 1];
			libwifi_dbg("capability = 0x%2x%2x\n", cap[0] & 0xff, cap[1] & 0xff);
			pos += reassoc ? 10 : 4;
			ie_start = (uint8_t *)&buf[pos];
			nlwifi_get_ssid(ifname, ssid);
			if (ssid[0] != '\0' && buf[pos] == 0x0 &&
				!memcmp(&buf[pos + 2], ssid, strlen(ssid))) {
				matched = 1;
			}

			if (!matched)
				return ret;

			wifi_cap_set_from_ie(info->cbitmap, cap, 2);

			ht_ie = wifi_find_ie(ie_start, 512, IE_HT_CAP);
			vht_ie = wifi_find_ie(ie_start, 512, IE_VHT_CAP);
			ext_ie = wifi_find_ie(ie_start, 512, IE_EXT_CAP);
			rrm_ie = wifi_find_ie(ie_start, 512, IE_RRM);
			ft_ie = wifi_find_ie(ie_start, 512, IE_MDE);

			if (ext_ie) {
				info->caps.valid |= WIFI_CAP_EXT_VALID;
				wifi_cap_set_from_ie(info->cbitmap, ext_ie, ext_ie[1] + 2);
				/////////////////////////////////
				{
					int _x;

					libwifi_dbg("Extended IE:  [");
					for (_x = 0; _x < ext_ie[1] + 2; _x++)
						libwifi_dbg("%02X ", ext_ie[_x]);
					libwifi_dbg("]\n");
				}
				////////////////////////////////

			}

			if (rrm_ie) {
				info->caps.valid |= WIFI_CAP_RM_VALID;
				wifi_cap_set_from_ie(info->cbitmap, rrm_ie, rrm_ie[1] + 2);

				/////////////////////////////////
				{
					int _x;

					libwifi_dbg("RRM IE:  [");
					for (_x = 0; _x < rrm_ie[1] + 2; _x++)
						libwifi_dbg("%02X ", rrm_ie[_x]);
					libwifi_dbg("]\n");
				}
				////////////////////////////////
			}

			if (ft_ie) {
				wifi_cap_set_from_ie(info->cbitmap, ft_ie, ft_ie[1] + 2);
			}

			if (ht_ie) {
				info->caps.valid |= WIFI_CAP_HT_VALID;
				//phy_mode = 1;	// ht
				bw = 40;

				wifi_cap_set_from_ie(info->cbitmap, ht_ie, ht_ie[1] + 2);

				if (wifi_cap_isset(info->cbitmap, WIFI_CAP_SGI20) ||
					wifi_cap_isset(info->cbitmap, WIFI_CAP_SGI40))
					sgi = 1;

				/////////////////////////////////
				{
					int _x;

					libwifi_dbg("HT IE:  [");
					for (_x = 0; _x < ht_ie[1] + 2; _x++)
						libwifi_dbg("%02X ", ht_ie[_x]);
					libwifi_dbg("]\n");
				}
				////////////////////////////////

				ht_ie += 2; /* ie + ielen */
				supp_mcs = ht_ie += 3; /* supported mcs */
				octet = 0;
				for (l = 0; l < 76; l++) {
					if (l && !(l % 8))
						octet++;

					if (!!(supp_mcs[octet] & bit(l % 8)))
						max_mcs++;
				}
				nss = (max_mcs / 8) + 1;
				max_mcs %= 8;
			}
			if (vht_ie) {
				//phy_mode = 2;	// vht
				bw = 80;
				info->caps.valid |= WIFI_CAP_VHT_VALID;

				wifi_cap_set_from_ie(info->cbitmap, vht_ie, vht_ie[1] + 2);

				if (wifi_cap_isset(info->cbitmap, WIFI_CAP_SGI80) ||
					wifi_cap_isset(info->cbitmap, WIFI_CAP_SGI160))
					sgi = 1;


				/////////////////////////////////
				{
					int _x;

					libwifi_dbg("VHT IE:  [");
					for (_x = 0; _x < vht_ie[1] + 2; _x++)
						libwifi_dbg("%02X ", vht_ie[_x]);
					libwifi_dbg("]\n");
				}
				////////////////////////////////

				vht_ie += 2; /* ie + ielen */
				supp_mcs = vht_ie += 4;  /* supported msc & nss */
				nss = 0;
				octet = 0;
				max_mcs = -1;
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
			}
			info->maxrate = (ht_ie || vht_ie) ?
					wifi_mcs2rate(max_mcs, bw, nss, sgi ? WIFI_SGI : WIFI_LGI)
					: 54;   /* TODO: 11b-only rate */
#undef bit
		}
	}

	/* .... and the airtime m|n,
	 * This could have been part of the mtlk_sta_info_t
	 * and no ugly stuff as below.
	 *
	 * NOTE: the same proc entry has a pretty good estimate
	 * of the instantenous date rate ('Efficiency of used
	 * air time [bytes/sec]').
	 */
	memset(sbuf, 0, sizeof(sbuf));
	chrCmd(sbuf, sizeof(sbuf),
		"cat /proc/net/mtlk/%s/PeerFlowStatus "
		"| grep -i -A10 " MACFMT " "
		"| grep 'Air Time Used by RX/TX to/from STA'"
		"| cut -d ':' -f1",
		ifname, MAC2STR(addr));

	if (sbuf[0])
		info->airtime = atoi(sbuf);

	return ret;
}

static int intel_get_noise(const char *ifname, int *noise)
{
	mtlk_radio_info_t r;
	int rlen = 0;
	int ret;

	*noise = 0;
	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
			LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_INFO,
			NULL, 0, (uint8_t *)&r, &rlen);

	if (!ret && rlen == sizeof(mtlk_radio_info_t))
		*noise = r.hw_stats.Noise;

	return ret;
}

static int intel_get_bandwidth(const char *ifname, enum wifi_bw *bw)
{
	mtlk_radio_info_t r;
	int rlen = 0;
	int ret;

	*bw = BW20;
	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
			LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_INFO,
			NULL, 0, (uint8_t *)&r, &rlen);

	if (!ret && rlen == sizeof(mtlk_radio_info_t)) {
		if (r.width == 160)
			*bw = BW160;
		else if (r.width == 80)
			*bw = BW80;
		else if (r.width == 40)
			*bw = BW40;
		else if (r.width == 20)
			*bw = BW20;
		else
			*bw = BW_UNKNOWN;
	}

	return ret;
}

static int intel_acs(const char *ifname, struct acs_param *p)
{
	char buf[256] = {0};

	chrCmd(buf, sizeof(buf),
		"hostapd_cli -i %s chan_switch 2 0", ifname);
	if (buf[0] != '\0' && strstr(buf, "OK"))
		return 0;

	return -1;
}

int intel_radio_get_stats(const char *ifname, struct wifi_radio_stats *s)
{
	struct mtlk_radio_info info;
	struct mtlk_wssa_drv_tr181_hw_stats *st;
	int ilen = 0;
	int ret;

	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
			LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_INFO,
			NULL, 0, (uint8_t *)&info, &ilen);

	if (!ret && ilen == sizeof(struct mtlk_radio_info)) {
		st = &info.hw_stats;

		s->tx_bytes = st->traffic_stats.BytesSent;
		s->rx_bytes = st->traffic_stats.BytesReceived;
		s->tx_pkts = st->traffic_stats.PacketsSent;
		s->rx_pkts = st->traffic_stats.PacketsReceived;
		s->tx_err_pkts = st->error_stats.ErrorsSent;
		s->rx_err_pkts = st->error_stats.ErrorsReceived;
		s->tx_dropped_pkts = st->error_stats.DiscardPacketsSent;
		s->rx_dropped_pkts = st->error_stats.DiscardPacketsReceived;
		//s->rx_plcp_err_pkts =
		s->rx_fcs_err_pkts = st->FCSErrorCount;
		//s->rx_mac_err_pkts =
		//s->rx_unknown_pkts =
		s->noise = st->Noise;
	}

	return ret;
}

int intel_ap_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	struct mtlk_vap_info info;
	struct mtlk_wssa_drv_tr181_vap_stats *st;
	int ilen = 0;
	int ret;

	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
			LTQ_NL80211_VENDOR_SUBCMD_GET_VAP_MEASUREMENTS,
			NULL, 0, (uint8_t *)&info, &ilen);

	if (!ret && ilen == sizeof(struct mtlk_vap_info)) {
		st = &info.vap_stats;

		s->tx_bytes = st->traffic_stats.BytesSent;
		s->rx_bytes = st->traffic_stats.BytesReceived;
		s->tx_pkts = st->traffic_stats.PacketsSent;
		s->rx_pkts = st->traffic_stats.PacketsReceived;
		s->tx_err_pkts = st->error_stats.ErrorsSent;
		s->tx_rtx_pkts = st->RetransCount;
		s->tx_rtx_fail_pkts = st->FailedRetransCount;
		s->tx_retry_pkts = st->RetryCount;
		s->tx_mretry_pkts = st->MultipleRetryCount;
		s->ack_fail_pkts = st->ACKFailureCount;
		s->aggr_pkts = st->AggregatedPacketCount;
		s->rx_err_pkts = st->error_stats.ErrorsReceived;
		s->tx_ucast_pkts = st->traffic_stats.UnicastPacketsSent;
		s->rx_ucast_pkts = st->traffic_stats.UnicastPacketsReceived;
		s->tx_dropped_pkts = st->error_stats.DiscardPacketsSent;
		s->rx_dropped_pkts = st->error_stats.DiscardPacketsReceived;
		s->tx_mcast_pkts = st->traffic_stats.MulticastPacketsSent;
		s->rx_mcast_pkts = st->traffic_stats.MulticastPacketsReceived;
		s->tx_bcast_pkts = st->traffic_stats.BroadcastPacketsSent;
		s->rx_bcast_pkts = st->traffic_stats.BroadcastPacketsReceived;
		s->rx_unknown_pkts = st->UnknownProtoPacketsReceived;
	}

	return ret;
}

static int intel_get_ap_info(const char *ifname, struct wifi_ap *ap)
{
	struct wifi_bss *bss;
	uint32_t ch = 0;
	int slen = 4;
	char sbuf[512] = {0};
	uint8_t stas[64 * 6] = {0};
	int num_stas = 64;
	int s = 0;
	int ret;

	bss = &ap->bss;
	memset(bss, 0, sizeof(*bss));
	nlwifi_get_bssid(ifname, bss->bssid);
	nlwifi_get_ssid(ifname, (char *)bss->ssid);
	nlwifi_get_channel(ifname, &ch, &bss->curr_bw);
	bss->channel = ch;
	nlwifi_get_bandwidth(ifname, &bss->curr_bw);

	ret = nlwifi_radio_get_caps(ifname, &ap->bss.caps);
	if (ret == 0) {
		if (!!(bss->caps.valid & WIFI_CAP_VHT_VALID)) {
			uint8_t tmp[6] = {0};

			tmp[0] = IE_VHT_CAP;
			tmp[1] = 4;

			memcpy(&tmp[2], &bss->caps.vht.byte[0], 4);
			wifi_cap_set_from_ie(bss->cbitmap, tmp, 6);
		}

		if (!!(bss->caps.valid & WIFI_CAP_HT_VALID)) {
			uint8_t tmp[4] = {0};

			tmp[0] = IE_HT_CAP;
			tmp[1] = 2;
			memcpy(&tmp[2], &bss->caps.ht.byte[0], 2);
			wifi_cap_set_from_ie(bss->cbitmap, tmp, 4);
		}
	}

	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
		LTQ_NL80211_VENDOR_SUBCMD_GET_AP_CAPABILITIES_MAX_STAs,
		NULL, 0, (uint8_t *)&s, &slen);
	if (!ret)
		ap->assoclist_max = s;

	s = 0;
	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
		LTQ_NL80211_VENDOR_SUBCMD_GET_HIDDEN_SSID,
		NULL, 0, (uint8_t *)&s, &slen);
	if (!ret)
		ap->ssid_advertised = (s == 0) ? true : false;

	s = 0;
	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
		LTQ_NL80211_VENDOR_SUBCMD_GET_AP_FORWARDING,
		NULL, 0, (uint8_t *)&s, &slen);
	if (!ret)
		ap->isolate_enabled = (s == 1) ? false : true;

	ret = nlwifi_get_assoclist(ifname, stas, &num_stas);
	if (!ret)
		ap->bss.load.sta_count = num_stas;

	/* load/utilization is available through /proc.. debug interface. */
	memset(sbuf, 0, sizeof(sbuf));
	chrCmd(sbuf, sizeof(sbuf),
		"cat /proc/net/mtlk/%s/Debug/General "
		"| grep -i \"Channel utilization\" "
		"| awk '{print $1}'",
		ifname);

	if (sbuf[0])
		ap->bss.load.utilization = atoi(sbuf);

	return 0;
}

static int intel_radio_get_param(const char *name, const char *param,
						int *len, void *val)
{
	int ret = 0;

	if (!param || param[0] == '\0')
		return -EINVAL;

	if (strncmp(param, "temperature", strlen(param)) == 0) {
		int s = 0;
		int slen = 4;

		ret = nlwifi_vendor_cmd(name, OUI_LTQ,
				LTQ_NL80211_VENDOR_SUBCMD_GET_TEMPERATURE_SENSOR,
				NULL, 0, (uint8_t *)&s, &slen);
		if (ret)
			return -1;
		*((int *)val) = s;
		*len = 1;
	}
	// TODO: other params likewise

	return 0;
}

static int intel_get_oper_stds(const char *name, uint8_t *std)
{
	int ret = 0;
	int s = 0;
	int slen = 4;

	ret = nlwifi_vendor_cmd(name, OUI_LTQ,
			LTQ_NL80211_VENDOR_SUBCMD_GET_NETWORK_MODE,
			NULL, 0, (uint8_t *)&s, &slen);

	if (ret || s > ARRAY_SIZE(wmode2std))
		return -1;

	*std = wmode2std[s];
	return 0;
}

static int intel_get_maxrate(const char *ifname, unsigned long *rate)
{
	uint8_t std = 0;
	int bw = 20;
	mtlk_radio_info_t r;
	int rlen = 0;
	uint8_t rx_ants;
	int max_mcs;
	int ret;

	*rate = 0;
	ret = nlwifi_vendor_cmd(ifname, OUI_LTQ,
			LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_INFO,
			NULL, 0, (uint8_t *)&r, &rlen);

	if (!ret && rlen == sizeof(mtlk_radio_info_t))
		bw = r.width;

	rx_ants = r.num_rx_antennas;
	intel_get_oper_stds(ifname, &std);

	/* libwifi_dbg("%s: bw = %d  rx_ants = %d\n",
					ifname, bw, rx_ants);*/

	/* very rough estimate of the max phyrate;
	 * ideally should read the beacon buffer for
	 * ht/vht ies and calculate max supported rate.
	 */
	if (!!(std & WIFI_AC)) {
		max_mcs = 9;
		*rate = wifi_mcs2rate(max_mcs, bw, rx_ants, WIFI_SGI);
	} else if (!!(std & WIFI_N)) {
		max_mcs = 8 * rx_ants - 1;
		max_mcs %= 8;
		*rate = wifi_mcs2rate(max_mcs, bw, rx_ants, WIFI_SGI);
	} else if (!!(std & WIFI_G)) {
		*rate = 54;
	} else if (std == WIFI_B) {
		*rate = 11;
	}

	return 0;
}

static int intel_radio_info(const char *name, struct wifi_radio *radio)
{
	uint8_t std = 0;
	int ret;

	ret = nlwifi_radio_info(name, radio);

	ret |= intel_get_oper_stds(name, &std);
	if (!ret)
		radio->oper_std = std;

	/* update attrs not returned through nlwifi */
	if (radio->beacon_int == 0) {
		int s = 0;
		int slen = 4;

		ret = nlwifi_vendor_cmd(name, OUI_LTQ,
				LTQ_NL80211_VENDOR_SUBCMD_GET_BEACON_PERIOD,
				NULL, 0, (uint8_t *)&s, &slen);
		if (!ret)
			radio->beacon_int = s;
	}

	if (radio->dtim_period == 0) {
		int s = 0;
		int slen = 4;

		ret = nlwifi_vendor_cmd(name, OUI_LTQ,
				LTQ_NL80211_VENDOR_SUBCMD_GET_DTIM_PERIOD,
				NULL, 0, (uint8_t *)&s, &slen);
		if (!ret)
			radio->dtim_period = s;
	}

	return ret;
}

static int intel_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	char buf[512] = {0};
	int wpaver = 0;
	int ret = 0;

	*auth = 0;
	*enc = 0;

	chrCmd(buf, sizeof(buf),
		"hostapd_cli -i %s get_config %s | grep wpa | cut -d'=' -f2",
		ifname, ifname);
	if (buf[0] != '\0') {
		wpaver = atoi(buf);
		/* libwifi_dbg("WPA version bitmap: %d\n", wpaver); */
	}

	memset(buf, 0, sizeof(buf));
	chrCmd(buf, sizeof(buf),
		"hostapd_cli -i %s get_config %s | grep key_mgmt | cut -d'=' -f2",
		ifname, ifname);
	if (buf[0] != '\0') {
		if (strstr(buf, "WPA-PSK")) {
			if (wpaver == 2)
				*auth = AUTH_WPA2PSK;
			if (wpaver == 1)
				*auth = AUTH_WPAPSK;
			if (wpaver == 3)
				*auth = AUTH_WPA2PSK | AUTH_WPAPSK;
		}
		if (strstr(buf, "WPA-EAP")) {
			if (wpaver == 2)
				*auth = AUTH_WPA2;
			if (wpaver == 1)
				*auth = AUTH_WPA;
			if (wpaver == 3)
				*auth = AUTH_WPA2 | AUTH_WPA;
		}

		memset(buf, 0, sizeof(buf));
		chrCmd(buf, sizeof(buf),
			"hostapd_cli -i %s get_config %s | grep pairwise_cipher | cut -d'=' -f2",
			ifname, ifname);
		if (buf[0] != '\0') {
			if (strstr(buf, "CCMP"))
				*enc |= CIPHER_AES;

			if (strstr(buf, "TKIP"))
				*enc |= CIPHER_TKIP;
		}
	} else {
		/* check for wep and null security */
		char phyname[32] = {0};

		if (nlwifi_get_phyname(ifname, phyname) != 0)
			goto out_wext;

		memset(buf, 0, sizeof(buf));
		chrCmd(buf, sizeof(buf),
			"grep wep_default_key= /var/run/hostapd-%s.conf | cut -d '=' -f2",
			phyname);
		if (buf[0] != '\0') {
			*auth = AUTH_OPEN;
			*enc = CIPHER_WEP;
		} else {
			*auth = AUTH_OPEN;
			*enc = CIPHER_NONE;
		}
	}

out_wext:
	if (*auth == 0 || *enc == 0) {
		/* static wep or null-security case */
		ret = wext_get_security(ifname, auth, enc);
	}

	return ret;
}

static int intel_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason)
{
	char buf[256] = {0};

	snprintf(buf, 255, "hostapd_cli -i %16s deauthenticate %16s " MACFMT,	/* Flawfinder: ignore */
				ifname, ifname, MAC2STR(sta));
	if (reason > 0)
		snprintf(buf + strlen(buf), 255, " reason=%d", reason);

	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0' && strstr(buf, "OK"))
		return 0;

	return -1;
}

int intel_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	char buf[512] = {0};
	char sta_macstr[18] = {0};

	if (!sta)
		return -1;

	snprintf(sta_macstr, sizeof(sta_macstr), MACFMT, MAC2STR(sta));	/* Flawfinder: ignore */
	/* Setup sta monitoring. Monitored data is
	 * reported through vendor events.
	 */
	if (cfg->enable) {
		/* add for monitor */
		enum wifi_bw bw;
		uint32_t ch;
		uint32_t cfreq1 = 0;

		if (nlwifi_get_channel(ifname, &ch, &bw) == 0) {
			if (ch >= 36 && ch < 200)
				cfreq1 = 5000 + ch * 5;
			else if (ch >= 1 && ch < 14)
				cfreq1 = 2407 + ch * 5;
			else if (ch == 14)
				cfreq1 = 2484;
		}

		snprintf(buf, sizeof(buf),
			"hostapd_cli -i %s unconnected_sta %s ",
			ifname, sta_macstr);
		snprintf(buf + strlen(buf), sizeof(buf),
			"%u center_freq1=%u bandwidth=%d", cfreq1, cfreq1, 20);

		libwifi_dbg("%s\n", buf);

		chrCmd(buf, sizeof(buf), buf);
		if (buf[0] != '\0' && strstr(buf, "OK"))
			return 0;
		else
			return -1;
	}

	return 0;
}

int intel_start_wps(const char *ifname, struct wps_param wps)
{
	char buf[256] = {0};

	if (wps.role == WPS_ENROLLEE) {
		//TODO: Enrollee mode
		/* if (wps.method == WPS_METHOD_PBC)
		else if (wps.method == WPS_METHOD_PIN)
		else */
			return -1;
	} else {
		if (wps.method == WPS_METHOD_PBC) {
			chrCmd(buf, sizeof(buf),
				"hostapd_cli -i %s wps_pbc %s", ifname, ifname);

			if (buf[0] != '\0' && strstr(buf, "OK"))
				return 0;

		} else if (wps.method == WPS_METHOD_PIN) {
#if 0  /* uuid "any" used */
			char uuid_buf[37] = {0};    /* 36 including '-'s */

			chrCmd(uuid_buf, sizeof(uuid_buf), "uuidgen");
			if (uuid_buf[0] != '\0' && strlen(uuid_buf) != 36)
				return -1;
#endif
			chrCmd(buf, sizeof(buf),
				"hostapd_cli -i %s wps_pin %s %s %08lu",
					ifname, ifname, "any", wps.pin);

			if (buf[0] != '\0' && strstr(buf, "OK"))
				return 0;

		}
	}

	return -1;
}

int intel_stop_wps(const char *ifname)
{
	char buf[256] = {0};

	chrCmd(buf, sizeof(buf),
		"hostapd_cli -i %s wps_cancel %s", ifname, ifname);

	if (buf[0] != '\0' && strstr(buf, "OK"))
		return 0;

	return -1;
}

static int intel_get_wps_status(const char *ifname, enum wps_status *s)
{
	char buf[256] = {0};

	chrCmd(buf, sizeof(buf),
		"hostapd_cli -i %s wps_get_status %s", ifname, ifname);

	if (buf[0] != '\0') {
		const char *f1 = "PBC Status: ";
		const char *f2 = "Last WPS result: ";
		char *pos1, *pos2;

		pos1 = strstr(buf, f1);
		pos2 = strstr(buf, f2);
		if (!pos1 || !pos2)
			return -1;

		pos1 += strlen(f1);
		if (strstr(pos1, "Disabled"))
			*s = WPS_STATUS_INIT;
		else if (strstr(pos1, "Active"))
			*s = WPS_STATUS_PROCESSING;
		else if (strstr(pos1, "Timed-out"))
			*s = WPS_STATUS_TIMEOUT;
		else if (strstr(pos1, "Overlap"))
			*s = WPS_STATUS_FAIL;
		else
			*s = WPS_STATUS_UNKNOWN;

		if (strstr(pos2, "Success"))
			*s = WPS_STATUS_SUCCESS;

		return 0;
	}

	return -1;
}

static int intel_get_wps_ap_pin(const char *ifname, unsigned long *pin)
{
	char buf[256] = {0};

	chrCmd(buf, sizeof(buf),
		"hostapd_cli -i %s wps_ap_pin %s get", ifname, ifname);

	if (buf[0] != '\0' && !strstr(buf, "FAIL")) {
		if ((strlen(buf) == 4) || (strlen(buf) == 8)) {
			*pin = strtoul(buf, NULL, 10);
			return 0;
		}
	}

	return -1;
}

static int intel_set_wps_ap_pin(const char *ifname, unsigned long pin)
{
	char buf[256] = {0};
	char pinbuf[9] = {0};

	snprintf(pinbuf, 9, "%08lu", pin);
	chrCmd(buf, sizeof(buf),
		"hostapd_cli -i %s wps_ap_pin %s set %08lu",
		ifname, ifname, pin);

	if (buf[0] != '\0' && strstr(buf, pinbuf))
		return 0;

	return -1;
}

int intel_handle_vendor_event(struct event_struct *ev)
{
	struct nlwifi_event_vendor_resp *r =
			(struct nlwifi_event_vendor_resp *)ev->resp.data;

	if (r->oui != OUI_LTQ)
		return 0;	/* discard as not ours */

	switch (r->subcmd) {
	case LTQ_NL80211_VENDOR_EVENT_CHAN_DATA:
		libwifi_dbg("%s: CHANNEL DATA\n", __func__);
		break;
	case LTQ_NL80211_VENDOR_EVENT_RADAR_DETECTED:
		libwifi_dbg("%s: RADAR detected\n", __func__);
		break;
	case LTQ_NL80211_VENDOR_EVENT_UNCONNECTED_STA:
		{
			//TODO: move to separate function
			struct ltq_unconnected_sta {
			  uint64_t rx_bytes;
			  uint32_t rx_packets;
			  int8_t rssi[4];
			  uint8_t addr[6];
			  int8_t noise[4];
			  uint16_t rate;
			} *monsta = (struct ltq_unconnected_sta *)r->data;

			libwifi_dbg("%s: Unconnected STA " MACFMT
				" rssi = %d %d %d %d   noise = %d %d %d %d  rate = %d\n",
				__func__, MAC2STR(monsta->addr),
				monsta->rssi[0],
				monsta->rssi[1],
				monsta->rssi[2],
				monsta->rssi[3],
				monsta->noise[0],
				monsta->noise[1],
				monsta->noise[2],
				monsta->noise[3],
				monsta->rate);
		}
		break;
	default:
		break;
	}

	if (ev->cb) {
		// XXX: suppressing this now
		//return ev->cb(ev);
	}

	return 0;
}

int intel_register_event(const char *ifname, struct event_struct *req,
						void **handle)
{
	/* handle vendor events of our interest */
	if (!strncmp(req->family, "nl80211", 7) &&
		!(strncmp(req->group, "vendor", 6))) {

		req->override_cb = intel_handle_vendor_event;
	}

	return nlwifi_register_event(ifname, req, handle);
}

static int intel_get_neighbor_list(const char *ifname, struct nbr *nbrs,
					int *nr)
{
	char buf[1024] = {0};
	unsigned char out[512] = {0};

	*nr = 0;
	snprintf(buf, sizeof(buf),
		"hostapd_cli -i %s list_neighbor_per_vap %s", ifname, ifname);

	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0') {
		strtob(buf, strlen(buf), out);
		*nr = (strlen(buf) >> 1 ) / sizeof(struct nbr);
		memcpy(nbrs, out, *nr * sizeof(struct nbr));
		return 0;
	}

	return 0;
}

static int intel_add_neighbor(const char *ifname, struct nbr nbr)
{
	unsigned char *nbr_bytes = (unsigned char *)&nbr;
	int len = sizeof(struct nbr);
	char buf[1024] = {0};
	char nbr_bssidstr[18] = {0};
	char ownssid[33] = {0};
	int i, ret;

	ret = nlwifi_get_ssid(ifname, ownssid);
	if (ret)
		return ret;

	snprintf(nbr_bssidstr, sizeof(nbr_bssidstr), MACFMT, MAC2STR(nbr.bssid));	/* Flawfinder: ignore */
	snprintf(buf, sizeof(buf),
		"hostapd_cli -i %s set_neighbor_per_vap %s %s ",
		ifname, ifname, nbr_bssidstr);

	snprintf(buf + strlen(buf), sizeof(buf), "ssid=\\\"%s\\\" nr=", ownssid);
	for (i = 0; i < len; i++)
		sprintf(buf + strlen(buf), "%02x", nbr_bytes[i] & 0xff);

	/* libwifi_dbg("Cmd =\n");
	for (i = 0; i < sizeof(buf) && buf[i]; i++)
		libwifi_dbg("%c", buf[i]);
	libwifi_dbg("\n"); */

	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0' && strstr(buf, "OK"))
		return 0;

	return -1;
}

static int intel_del_neighbor(const char *ifname, unsigned char *bssid)
{
	char buf[512] = {0};
	char nbr_bssidstr[18] = {0};
	char ownssid[33] = {0};
	int ret;

	ret = nlwifi_get_ssid(ifname, ownssid);
	if (ret)
		return ret;

	snprintf(nbr_bssidstr, sizeof(nbr_bssidstr), MACFMT, MAC2STR(bssid));	/* Flawfinder: ignore */
	snprintf(buf, sizeof(buf),
		"hostapd_cli -i %s remove_neighbor_per_vap %s %s ",
		ifname, ifname, nbr_bssidstr);

	snprintf(buf + strlen(buf), sizeof(buf), "ssid=\\\"%s\\\"", ownssid);

	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0' && strstr(buf, "OK")) {
		return 0;
	}

	return -1;
}

static int intel_req_beacon_report(const char *ifname, uint8_t *sta,
			struct wifi_beacon_req *param, size_t param_sz)
{
	char buf[1024] = {0};
	char da_macstr[18] = {0};
	int num_rept = 0;
	int meas_reqmode = 0;
	int reg = 0;
	int channel;
	int random_int;
	int dur;
	const char *mode = "passive";
	unsigned char for_bssid[6] = "\xff\xff\xff\xff\xff\xff";
	char ownssid[33] = {0};
	int ret;

	ret = nlwifi_get_ssid(ifname, ownssid);
	if (ret)
		return ret;

	snprintf(da_macstr, sizeof(da_macstr), MACFMT, MAC2STR(sta));	/* Flawfinder: ignore */
	channel = 255;
	random_int = 1000;
	dur = 50;

	snprintf(buf, sizeof(buf),
		"hostapd_cli -i %s req_beacon %s %d %d %d %d %d %d ",
		ifname, da_macstr,
		num_rept, meas_reqmode, reg, channel, random_int, dur);
	snprintf(buf + strlen(buf), sizeof(buf), "%s " MACFMT " ",
			mode, MAC2STR(for_bssid));
	snprintf(buf + strlen(buf), sizeof(buf), "ssid=\\\"%s\\\"", ownssid);

	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0' && strstr(buf, "OK")) {
		libwifi_dbg("%s: return OK\n", __func__);
		return 0;
	}

	return -1;
}

static int intel_get_beacon_report(const char *ifname, uint8_t *sta,
			struct sta_nbr *snbrs, int *nr)
{
	//TODO

	*nr = 0;
	return 0;
}

int intel_req_bss_transition(const char *ifname, unsigned char *sta,
			int bsss_nr, unsigned char *bsss, unsigned int tmo)
{
	char buf[1024] = {0};
	char macstr[18] = {0};
	struct nbr *pnbr;
	int pn = 0;
	int i, j;

	pnbr = calloc(32, sizeof(struct nbr));
	if (!pnbr)
		return -1;

	snprintf(macstr, sizeof(macstr), MACFMT, MAC2STR(sta));	/* Flawfinder: ignore */
	snprintf(buf, sizeof(buf),
		"hostapd_cli -i %s bss_tm_req %s ",
		ifname, macstr);

	snprintf(buf + strlen(buf), sizeof(buf), "%s", "pref=1 ");
	snprintf(buf + strlen(buf), sizeof(buf), "%s", "disassoc_imminent=1 ");
	if (tmo)
		snprintf(buf + strlen(buf), sizeof(buf), "valid_int=%u ", tmo);

	if (bsss_nr > 5)
		bsss_nr = 5;

	intel_get_neighbor_list(ifname, pnbr, &pn);
	/* append valid neighbors */
	for (i = 0; i < bsss_nr; i++) {
		for (j = 0; j < pn; j++) {
			struct nbr *t;

			t = pnbr + j;
			if (!memcmp(t->bssid, bsss + i*6, 6)) {
				snprintf(buf + strlen(buf), sizeof(buf),
					"neighbor="MACFMT",%u,%d,%d,%d ",
					MAC2STR(t->bssid), t->bssid_info,
					t->reg, t->channel, t->phy);
				break;
			}
		}
	}

	free(pnbr);

	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0' && strstr(buf, "dialog_token=")) {
		/* libwifi_dbg("OK. ret dialog_token = %d\n",
				atoi(&buf[strlen("dialog_token=")])); */
		return 0;
	}

	return -1;
}

int intel_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char buf[512] = {0};
	int i;
	int datalen;

	if (!req || req->ie.ie_hdr.len < 3) {
		libwifi_err("%s(): invalid args!\n", __func__);
		return -1;
	}

	datalen = req->ie.ie_hdr.len - 3;
	snprintf(buf, sizeof(buf),
		"hostapd_cli -i %s vendor_elements %s ", ifname, ifname);

	snprintf(buf + strlen(buf), sizeof(buf), "%02x%02x",
			req->ie.ie_hdr.eid, req->ie.ie_hdr.len);
	snprintf(buf + strlen(buf), sizeof(buf), "%02x%02x%02x",
			req->ie.oui[0], req->ie.oui[1], req->ie.oui[2]);

	for (i = 0; i < datalen; i++)
		sprintf(buf + strlen(buf), "%02x", req->ie.data[i] & 0xff);

	/* libwifi_dbg("%s\n", buf); */
	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0' && strstr(buf, "OK"))
		return 0;

	return -1;
}

int intel_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char buf[256] = {0};

	if (!req || req->ie.ie_hdr.len < 3) {
		libwifi_err("%s(): invalid args!\n", __func__);
		return -1;
	}

	/* TODO: get current vsie list, and delete matching ones */
	snprintf(buf, sizeof(buf),
		"hostapd_cli -i %s vendor_elements %s ", ifname, ifname);

	chrCmd(buf, sizeof(buf), buf);
	if (buf[0] != '\0' && strstr(buf, "OK"))
		return 0;

	return -1;
}

const struct wifi_driver intel_driver = {
	.name = "wlan",
	.scan = nlwifi_scan,
	.get_scan_results = nlwifi_get_scan_results,
	.get_bssid = nlwifi_get_bssid,
	.get_ssid = nlwifi_get_ssid,
	.get_maxrate = intel_get_maxrate,
	.get_channel = nlwifi_get_channel,
	.get_bandwidth = intel_get_bandwidth,
	.get_oper_stds = intel_get_oper_stds,
	.get_supp_channels = nlwifi_get_supp_channels,
	.get_noise = intel_get_noise,
	.acs = intel_acs,
	.vendor_cmd = nlwifi_vendor_cmd,
	.radio.get_stats = intel_radio_get_stats,
	.iface.ap_info = intel_get_ap_info,
	.iface.get_stats = intel_ap_get_stats,
	.iface.get_caps = nlwifi_ap_get_caps,
	.get_assoclist = nlwifi_get_assoclist,
	.iface.get_security = intel_get_security,
	.get_sta_info = intel_get_sta_info,
	.radio.get_caps = nlwifi_radio_get_caps,
	.get_country = nlwifi_get_country,
	.radio.info = intel_radio_info,
	.radio.get_param = intel_radio_get_param,
	.disconnect_sta = intel_disconnect_sta,
	.monitor_sta = intel_monitor_sta,
	.iface.start_wps = intel_start_wps,
	.iface.stop_wps = intel_stop_wps,
	.iface.get_wps_status = intel_get_wps_status,
	.iface.get_wps_pin = intel_get_wps_ap_pin,
	.iface.set_wps_pin = intel_set_wps_ap_pin,
	.register_event = intel_register_event,
	.recv_event = nlwifi_recv_event,
	.iface.get_neighbor_list = intel_get_neighbor_list,
	.iface.add_neighbor = intel_add_neighbor,
	.iface.del_neighbor = intel_del_neighbor,
	.iface.req_beacon_report = intel_req_beacon_report,
	.iface.get_beacon_report = intel_get_beacon_report,
	.iface.req_bss_transition = intel_req_bss_transition,
	.iface.add_vendor_ie = intel_add_vendor_ie,
	.iface.del_vendor_ie = intel_del_vendor_ie,
};
