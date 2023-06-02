/*
 * test.c - implements 'test' wifi driver
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

#include <easy/easy.h>
#include "wifiutils.h"
#include "wifi.h"
#include "debug.h"
#include "test.h"

#define MACFMT "%02x:%02x:%02x:%02x:%02x:%02x"

void log_test(const char *fmt, ...)
{
	FILE *fp = NULL;
	va_list args;

	fp = fopen("/tmp/test.log", "a");
	if (!fp)
		return;

	time_t now = time(NULL);
	struct tm *tm_now = localtime(&now);
	const char *tm_fmt = "[%4d-%02d-%02d %02d:%02d:%02d] ";

	va_start(args, fmt);
	fprintf(fp, tm_fmt,		/* Flawfinder: ignore */
			tm_now->tm_year + 1900,
			tm_now->tm_mon + 1,
			tm_now->tm_mday,
			tm_now->tm_hour,
			tm_now->tm_min,
			tm_now->tm_sec);
	vfprintf(fp, fmt, args);	/* Flawfinder: ignore */
	va_end(args);

	fflush(fp);
	fclose(fp);
}

int test_ap_get_caps(const char *ifname, struct wifi_caps *caps)
{
	GET_TEST_BUF_TYPE(caps, ifname, ap_caps, struct wifi_caps);

	return 0;
}

int test_radio_get_caps(const char *ifname, struct wifi_caps *caps)
{
	GET_TEST_BUF_TYPE(caps, ifname, ap_caps, struct wifi_caps);

	return 0;
}

int test_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	int num_stations = 0;
	//int ret = -1;
	int i;

	if (!addr) {
		libwifi_err("Invalid args!\n");
		return -EINVAL;
	}

	num_stations = GET_TEST_ARRAY_SIZE(struct wifi_sta, ifname, stalist);

	for (i = 0; i < num_stations; i++) {
		struct wifi_sta tmp;

		memset(&tmp, 0, sizeof(struct wifi_sta));
		GET_TEST_ARRAY_ENTRY(&tmp, struct wifi_sta, ifname, stalist, i);

		if (!memcmp(tmp.macaddr, addr, 6)) {
			memcpy(info, &tmp, sizeof(struct wifi_sta));
			//ret = 0;
			break;
		}
	}

	GET_TEST_ARRAY_ENTRY(&info->caps, struct wifi_caps, ifname, sta_caps, i);

	if (!!(info->caps.valid & WIFI_CAP_BASIC_VALID))
		wifi_cap_set_from_ie(info->cbitmap, info->caps.basic.byte, 2);

	if (!!(info->caps.valid & WIFI_CAP_VHT_VALID)) {
		uint8_t tmp[6] = {0};

		wifi_cap_set(info->cbitmap, WIFI_CAP_VHT_MPDU_3895);

		tmp[0] = 0xff;
		tmp[1] = 0xff;

		memcpy(&tmp[2], &info->caps.vht.byte[0], 4);
		wifi_cap_set_from_ie(info->cbitmap, tmp, 6);
	}

	if (!!(info->caps.valid & WIFI_CAP_HT_VALID)) {
		uint8_t tmp[4] = {0};

		tmp[0] = 0xff;
		tmp[1] = 0xff;
		memcpy(&tmp[2], &info->caps.ht.byte[0], 2);
		wifi_cap_set_from_ie(info->cbitmap, tmp, 4);
	}

	if (!!(info->caps.valid & WIFI_CAP_EXT_VALID)) {
		uint8_t tmp[10] = {0};

		tmp[0] = 0xff;
		tmp[1] = 0xff;
		memcpy(&tmp[2], &info->caps.ext.byte[0], 8);
		wifi_cap_set_from_ie(info->cbitmap, tmp, 10);
	}

	if (!!(info->caps.valid & WIFI_CAP_RM_VALID)) {
		uint8_t tmp[7] = {0};

		tmp[0] = 0xff;
		tmp[1] = 0xff;
		memcpy(&tmp[2], &info->caps.rrm.byte[0], 5);
		wifi_cap_set_from_ie(info->cbitmap, tmp, 7);
	}

	return 0;
}

int test_radio_get_stats(const char *ifname, struct wifi_radio_stats *s)
{
	GET_TEST_BUF_TYPE(s, ifname, radio_stats, struct wifi_radio_stats);

	return 0;
}

int test_ap_get_stats(const char *ifname, struct wifi_ap_stats *s)
{

	GET_TEST_BUF_TYPE(s, ifname, ap_stats, struct wifi_ap_stats);

	return 0;
}

static int test_radio_get_param(const char *name, const char *param,
						int *len, void *val)
{
	//int ret = 0;

	if (!param || param[0] == '\0')
		return -EINVAL;

	if (strncmp(param, "temperature", strlen(param)) == 0)
		GET_TEST_INT(*(int *)val, name, temperature);

	return 0;
}

int test_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	GET_TEST_ARRAY(stas, *num_stas, macaddr_t, ifname, assoclist);

	return 0;
}

static int test_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason)
{
	uint8_t stas[384] = {0};
	int num_stas = 64;
	int ret;
	int i;

	ret = test_get_assoclist(ifname, stas, &num_stas);
	if (ret)
		return -1;

	for (i = 0; i < num_stas; i++) {
		if (!memcmp(&stas[i*6], sta, 6)) {
			log_test("disconnect: { \"ifname\": \"%s\", \"sta\": \"" MACFMT \
					"\", \"reason\":%hu }\n", ifname, sta[0], sta[1], sta[2],
					sta[3], sta[4], sta[5], reason);
			return 0;
		}
	}

	return -1;
}

int test_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	uint8_t stas[384] = {0};
	int num_stas = 64;
	int ret;
	int i;

	log_test("monitor_sta: { \"ifname\": \"%s\", \"sta\": \"" MACFMT \
			"\", \"enable\": %d }\n", ifname, sta[0], sta[1], sta[2],
			sta[3], sta[4], sta[5], cfg->enable);

	ret = test_get_assoclist(ifname, stas, &num_stas);
	if (ret)
		return -1;

	for (i = 0; i < num_stas; i++) {
		int offset = i * 6;
		if (!memcmp(&stas[offset], sta, 6))
			return -1;
	}

	return 0;
}

static int test_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon)
{
	int num, i;
	int ret = -1;

	num = GET_TEST_ARRAY_SIZE(struct wifi_monsta, ifname, monitor_get);

	for (i = 0; i < num; i++) {
		struct wifi_monsta entry;

		memset(&entry, 0, sizeof(entry));
		GET_TEST_ARRAY_ENTRY(&entry, struct wifi_monsta, ifname, monitor_get, i);

		if (!memcmp(entry.macaddr, sta, 6)) {
			memcpy(mon, &entry, sizeof(struct wifi_monsta));
			ret = 0;
			break;
		}
	}

	return ret;
}

static int test_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num)
{
	int buflen = 0;

	GET_TEST_BUF(stas, buflen, ifname, monitor_get);
	if (buflen)
		*num = buflen / sizeof(struct wifi_monsta);

	return 0;
}

int test_stop_wps(const char *ifname)
{
	log_test("stop_wps: { \"ifname\": \"%s\" }\n", ifname);

	return 0;
}

int test_start_wps(const char *ifname, struct wps_param wps)
{

	if (wps.method == WPS_METHOD_PIN)
		log_test("start_wps: { \"ifname\": \"%s\", \"pin\": \"%d\", \"role\": %d"\
				", \"mode\": %d }\n",
				ifname, wps.pin, wps.role, wps.method);
	else
		log_test("start_wps: { \"ifname\": \"%s\", \"role\": %d"\
				", \"mode\": %d }\n",
				ifname, wps.role, wps.method);
	return 0;
}

static int test_get_wps_status(const char *ifname, enum wps_status *s)
{
	*s = WPS_STATUS_SUCCESS;
	return 0;
}

static int test_get_wps_ap_pin(const char *ifname, unsigned long *pin)
{
	*pin = 12345678;
	return 0;
}

static int test_set_wps_ap_pin(const char *ifname, unsigned long pin)
{
	log_test("set_wps_ap_pin: { \"ifname\": \"%s\", \"pin\": %lu }\n",
			ifname, pin);
	return 0;
}

#if 0
int test_handle_vendor_event(struct event_struct *ev)
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
#endif

static int test_get_neighbor_list(const char *ifname, struct nbr *nbr,
					int *nr)
{
	int buflen = 0;

	GET_TEST_BUF(nbr, buflen, ifname, neighbor_list);
	if (buflen)
		*nr = buflen / sizeof(struct nbr);

	return 0;
}

static int test_add_neighbor(const char *ifname, struct nbr nbr)
{
	log_test("add_neighbor: { \"ifname\": \"%s\", \"bssid_info\": %d, \"reg\""\
			": %d, \"channel\": %d, \"phy\": %d, \"bssid\": \"" MACFMT "\" }\n",
			ifname,	nbr.bssid_info, nbr.reg, nbr.channel, nbr.phy,
			nbr.bssid[0], nbr.bssid[1], nbr.bssid[2],
			nbr.bssid[3], nbr.bssid[4], nbr.bssid[5]);
	return 0;
}

static int test_del_neighbor(const char *ifname, unsigned char *bssid)
{
	char buf[1024] = {0};
	struct nbr *tmp, *pnbr = (struct nbr *)buf;
	int pn = 0;
	int ret;
	int i;

	ret = test_get_neighbor_list(ifname, pnbr, &pn);
	if (ret)
		return -1;

	for (i = 0; i < pn; i++) {
		tmp = &pnbr[i];
		if (!memcmp(tmp->bssid, bssid, 6)) {
			log_test("del_neighbor: { \"ifname\": \"%s\", \"bssid\": \"" \
					MACFMT "\" }\n", ifname,
					bssid[0], bssid[1], bssid[2], bssid[3], bssid[4], bssid[5]);
			return 0;
		}
	}

	return -1;
}

static int test_req_beacon_report(const char *ifname, uint8_t *sta,
		struct wifi_beacon_req *param, size_t param_sz)
{
	log_test("req_beacon_report: { \"ifname\": \"%s\", \"client\": \"" MACFMT \
			"\" }\n", ifname, sta[0], sta[1], sta[2],
			sta[3], sta[4], sta[5]);
	return 0;
}

static int test_get_beacon_report(const char *ifname, uint8_t *sta,
		struct sta_nbr *snbr, int *nr)
{
	int buflen = 0;
	int ret;
	uint8_t stas[384] = {0};
	int num_stas = 64;
	int i;

	ret = test_get_assoclist(ifname, stas, &num_stas);
	if (ret)
		return -1;

	for (i = 0; i < num_stas; i++) {
		if (!memcmp(&stas[i*6], sta, 6)) {
			GET_TEST_BUF(snbr, buflen, ifname, snbrs);
			if (buflen)
				*nr = buflen / sizeof(struct sta_nbr);

			return 0;
		}
	}

	return -1;
}

int test_req_bss_transition(const char *ifname, unsigned char *sta,
			int bsss_nr, unsigned char *bsss, unsigned int tmo)
{
	char bsss_str[512] = {0};
	int i;

	strncat(bsss_str, "[", 1);

	for (i = 0; i < bsss_nr; i++) {
		char macstr[32] = {0};
		int offset = i*6;

		snprintf(macstr, sizeof(macstr), "\"" MACFMT "\"",
				bsss[offset + 0], bsss[offset + 1], bsss[offset + 2],
				bsss[offset + 3], bsss[offset + 4], bsss[offset + 5]);
		snprintf(bsss_str + strlen(bsss_str), sizeof(bsss_str), "%s", macstr);
		strncat(bsss_str, ",", 1);
	}

	strncat(bsss_str, "]", 1);

	log_test("req_bss_transition: { \"ifname\": \"%s\", \"client\": \"" MACFMT\
			"\", \"bssid\": %s }\n", ifname,
			sta[0], sta[1], sta[2],
			sta[3], sta[4], sta[5],
			bsss_str);

	return 0;
}

int test_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char buf[512] = {0};
	int i;
	int datalen;

	if (!req || req->ie.ie_hdr.len < 3) {
		libwifi_err("%s(): invalid args!\n", __func__);
		return -1;
	}

	datalen = req->ie.ie_hdr.len - 3;

	for (i = 0; i < datalen; i++)
		sprintf(buf + strlen(buf), "%02x", req->ie.data[i] & 0xff);

	log_test("add_vendor_ie: { \"ifname\": \"%s\", \"mgmt\": %d, \"oui\": "\
			"\"%02x%02x%02x\", \"data\": \"%s\" }\n",
			ifname, req->mgmt_subtype,
			req->ie.oui[0], req->ie.oui[1], req->ie.oui[2],
			buf
	);

	return 0;
}

int test_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	char buf[512] = {0};
	int i;
	int datalen;

	if (!req || req->ie.ie_hdr.len < 3) {
		libwifi_err("%s(): invalid args!\n", __func__);
		return -1;
	}

	datalen = req->ie.ie_hdr.len - 3;

	for (i = 0; i < datalen; i++)
		sprintf(buf + strlen(buf), "%02x", req->ie.data[i] & 0xff);

	log_test("del_vendor_ie: { \"ifname\": \"%s\", \"mgmt\": %d, \"oui\": "\
			"\"%02x%02x%02x\", \"data\": \"%s\" }\n",
			ifname, req->mgmt_subtype,
			req->ie.oui[0], req->ie.oui[1], req->ie.oui[2],
			buf
	);

	return 0;
}

static int test_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	GET_TEST_INT(*auth, ifname, security_auth);
	GET_TEST_INT(*enc, ifname, security_enc);

	return 0;
}

int test_get_country(const char *name, char *alpha2)
{
	GET_TEST_STRING(alpha2, name, countrycode);

	return 0;
}

static int test_get_noise(const char *ifname, int *noise)
{
	GET_TEST_INT(*noise, ifname, noise);

	return 0;
}

int test_get_supp_channels(const char *ifname, uint32_t *chlist, int *num,
					const char *cc,
					enum wifi_band band,
					enum wifi_bw bw)
{
	GET_TEST_ARRAY(chlist, *num, uint32_t, ifname, supp_channels);

	return 0;
}

static int test_get_bandwidth(const char *ifname, enum wifi_bw *bw)
{
	GET_TEST_INT(*bw, ifname, bandwidth);

	return 0;
}


static int test_get_oper_stds(const char *name, uint8_t *std)
{
	GET_TEST_INT(*std, name, supp_stds);

	return 0;
}

int test_get_channel(const char *ifname, uint32_t *channel,
						enum wifi_bw *bw)
{
	GET_TEST_INT(*channel, ifname, channel);
	GET_TEST_INT(*bw, ifname, bandwidth);
	return 0;
}

int test_get_ssid(const char *ifname, char *ssid)
{
	GET_TEST_STRING(ssid, ifname, ssid);

	return 0;
}

int test_get_bssid(const char *ifname, uint8_t *bssid)
{
	int l;

	GET_TEST_BUF(bssid, l, ifname, bssid);
	UNUSED(l);

	return 0;
}

static int test_get_maxrate(const char *ifname, unsigned long *rate)
{
	GET_TEST_INT(*rate, ifname, maxrate);

	return 0;
}

static int test_get_ap_info(const char *ifname, struct wifi_ap *ap)
{
	struct wifi_bss *bss;
	uint32_t ch = 0;
	//int slen = 4;
	//char sbuf[512] = {0};
	uint8_t stas[64 * 6] = {0};
	int num_stas = 64;
	//int s = 0;
	int ret;

	bss = &ap->bss;
	memset(bss, 0, sizeof(*bss));
	test_get_bssid(ifname, bss->bssid);
	test_get_ssid(ifname, (char *)bss->ssid);
	test_get_channel(ifname, &ch, &bss->curr_bw);
	bss->channel = ch;
	test_get_bandwidth(ifname, &bss->curr_bw);
	test_get_oper_stds(ifname, &bss->oper_std);
	test_ap_get_caps(ifname, &bss->caps);

	//GET_TEST_BUF_TYPE(&bss->caps, ifname, ap_caps, struct wifi_caps);

	if (!!(bss->caps.valid & WIFI_CAP_BASIC_VALID))
		wifi_cap_set_from_ie(bss->cbitmap, bss->caps.basic.byte, 2);

	if (!!(bss->caps.valid & WIFI_CAP_VHT_VALID)) {
		uint8_t tmp[6] = {0};

		tmp[0] = 0xbf;
		tmp[1] = 0x04;

		memcpy(&tmp[2], &bss->caps.vht.byte[0], 4);
		wifi_cap_set_from_ie(bss->cbitmap, tmp, 6);
	}

	if (!!(bss->caps.valid & WIFI_CAP_HT_VALID)) {
		uint8_t tmp[4] = {0};

		tmp[0] = 0x2d;
		tmp[1] = 0x02;
		memcpy(&tmp[2], &bss->caps.ht.byte[0], 2);
		wifi_cap_set_from_ie(bss->cbitmap, tmp, 4);
	}

	if (!!(bss->caps.valid & WIFI_CAP_EXT_VALID)) {
		uint8_t tmp[10] = {0};

		tmp[0] = 0x7f;
		tmp[1] = 0x08;
		memcpy(&tmp[2], &bss->caps.ext.byte[0], 8);
		wifi_cap_set_from_ie(bss->cbitmap, tmp, 10);
	}

	if (!!(bss->caps.valid & WIFI_CAP_RM_VALID)) {
		uint8_t tmp[7] = {0};

		tmp[0] = 0x46;
		tmp[1] = 0x05;
		memcpy(&tmp[2], &bss->caps.rrm.byte[0], 5);
		wifi_cap_set_from_ie(bss->cbitmap, tmp, 7);
	}

	GET_TEST_INT(ap->assoclist_max, ifname, maxassoc);
	GET_TEST_INT(ap->ssid_advertised, ifname, ssid_advertised);
	GET_TEST_INT(ap->isolate_enabled, ifname, isolate);
	ret = test_get_assoclist(ifname, stas, &num_stas);
	if (!ret)
		ap->bss.load.sta_count = num_stas;

	GET_TEST_INT(ap->bss.load.utilization, ifname, load_utilization);

	return 0;
}

static int test_radio_info(const char *name, struct wifi_radio *radio)
{
	//uint8_t std = 0;
	uint32_t supp_chs[64] = {0};
	int num_chs = 64;
	uint32_t curr_ch = 0;
	int num_supp_rates = 0;
	int num_basic_rates = 0;
	int ret = 0;
	int i;


	ret = test_get_channel(name, &curr_ch, &radio->curr_bw);
	radio->channel = curr_ch & 0xff;	// FIXME: data size

	GET_TEST_INT(radio->oper_band, name, oper_band);
	ret |= test_get_bandwidth(name, &radio->curr_bw);
	ret |= test_get_oper_stds(name, &radio->oper_std);
	ret |= test_get_country(name, radio->regdomain);
	ret |= test_get_supp_channels(name, supp_chs, &num_chs,
					radio->regdomain,
					ifname_to_band_enum(name),
					radio->curr_bw);
	for (i = 0; i < num_chs; i++)
		radio->supp_channels[i] = supp_chs[i] & 0xff;

	GET_TEST_INT(radio->extch, name, extch);
	GET_TEST_INT(radio->beacon_int, name, beacon_int);
	GET_TEST_INT(radio->dtim_period, name, dtim_period);
	GET_TEST_INT(radio->gi, name, guard_int);

	GET_TEST_INT(radio->tx_streams, name, tx_streams);
	GET_TEST_INT(radio->rx_streams, name, rx_streams);
	GET_TEST_INT(radio->acs_capable, name, acs_capable);
	GET_TEST_INT(radio->acs_enabled, name, acs_enabled);
	GET_TEST_INT(radio->acs_interval, name, acs_interval);
	GET_TEST_INT(radio->dot11h_capable, name, dot11h_capable);
	GET_TEST_INT(radio->dot11h_enabled, name, dot11h_enabled);
	GET_TEST_INT(radio->txpower, name, txpower);
	GET_TEST_INT(radio->txpower_dbm, name, txpower_dbm);
	GET_TEST_INT(radio->srl, name, srl);
	GET_TEST_INT(radio->lrl, name, lrl);
	GET_TEST_INT(radio->frag, name, frag_threshold);
	GET_TEST_INT(radio->rts, name, rts_threshold);
	GET_TEST_INT(radio->aggr_enable, name, aggr_enabled);

	GET_TEST_ARRAY(radio->supp_rates, num_supp_rates, uint32_t,
							name, supp_rates);
	GET_TEST_ARRAY(radio->basic_rates, num_basic_rates, uint32_t,
							name, basic_rates);
	UNUSED(num_supp_rates);
	UNUSED(num_basic_rates);

	ret |= test_radio_get_stats(name, &radio->stats);

	//TODO: create new api for radio diagnostic data
	GET_TEST_BUF_TYPE(&radio->diag, name, radio_diagnostic,
						struct wifi_radio_diagnostic);

	return ret;
}

static int test_acs(const char *ifname, struct acs_param *p)
{
	log_test("acs: { \"ifname\": \"%s\" }\n", ifname);
	return 0;
}

int test_scan(const char *ifname, struct scan_param *p)
{
	if (p)
		log_test("scan: { \"ifname\": \"%s\", \"ssid\": \"%s\", "\
				"\"bssid\": \"" MACFMT "\", \"channel\": %d, \"type\": %d }\n", ifname,
				p->ssid, p->bssid[0], p->bssid[1], p->bssid[2], p->bssid[3],
				p->bssid[4], p->bssid[5], p->channel, p->type);
	else
		log_test("scan: { \"ifname\": \"%s\" }\n", ifname);

	return 0;
}

int test_get_scan_results(const char *ifname, struct wifi_bss *bsss,
						int *num)
{
	int buflen = 0;

	GET_TEST_BUF(bsss, buflen, ifname, scanresults);
	if (buflen)
		*num = buflen / sizeof(struct wifi_bss);

	return 0;
}

int test_get_bss_scanresult(const char *ifname, uint8_t *bssid,
						struct wifi_bss_detail *b)
{
	int num = 0;
	int ret = -1;
	int i;

	if (!bssid) {
		libwifi_err("Invalid args!\n");
		return -EINVAL;
	}

	num = GET_TEST_ARRAY_SIZE(struct wifi_bss, ifname, scanresults);

	for (i = 0; i < num; i++) {
		struct wifi_bss tmp;

		memset(&tmp, 0, sizeof(struct wifi_bss));
		GET_TEST_ARRAY_ENTRY(&tmp, struct wifi_bss, ifname, scanresults, i);

		if (!memcmp(tmp.bssid, bssid, 6)) {
			memcpy(&b->basic, &tmp, sizeof(struct wifi_bss));
			ret = 0;
			break;
		}
	}

	return ret;
}

int test_register_event(const char *ifname, struct event_struct *req,
		void **handle)
{
	log_test("register_event: { \"ifname\": \"%s\", \"family\": \"%s\", \"group\": \"%s\" }\n",
			req->ifname, req->family, req->group);
	return 0;
}

int test_recv_event(const char *ifname, void *handle)
{
	log_test("recv_event: { \"ifname\": \"%s\" }\n", ifname);
	return -1;
}

int test_driver_info(const char *name, struct wifi_metainfo *info)
{
	GET_TEST_BUF_TYPE(info, name, drv_info, struct wifi_metainfo);

	return 0;
}

static int test_iface_subscribe_frame(const char *ifname, uint8_t type,
		uint8_t stype)
{
	log_test("iface_subscribe_frame: { \"ifname\": \"%s\", \"type\": %d, " \
		"\"stype\": %d }\n", ifname, type, stype);
	return 0;
}

static int test_iface_unsubscribe_frame(const char *ifname, uint8_t type,
		uint8_t stype)
{
	log_test("iface_unsubscribe_frame: { \"ifname\": \"%s\", \"type\": %d, " \
		"\"stype\": %d }\n", ifname, type, stype);
	return 0;
}

static int test_get_beacon_ies(const char *ifname, uint8_t *ies, int *ies_len)
{
	GET_TEST_BUF(ies, *ies_len, ifname, beacon_dump);
	return 0;
}

int test_add_iface(const char *name, enum wifi_mode m, char *argv[])
{
        int i;
        char args[512] = {0};

        args[0] = '[';

        for (i = 0; argv[i]; i++) {
				int len = strlen(args);
				args[len] = '"';

                len = strlen(args);
				strncat(args+len, argv[i], sizeof(args) - len);

				len = strlen(args);
				strncat(args+len, "\", ", sizeof(args) - len);
        }

        args[strlen(args) - 2] = ']';

        log_test("add_iface: { \"ifname\": \"%s\", \"wifi_mode\": "\
                "\"%d\", \"argv\": %s }\n", name, m, args);

        return 0;
}

int test_del_iface(const char *name, const char *ifname)
{
	log_test("del_iface: { \"ifname\": \"%s\", \"iface\": \"%s\" }\n",
			name, ifname);
	return 0;
}

const struct wifi_driver test_driver = {
	.name = "test",
	.info = test_driver_info,
	.scan = test_scan,
	.get_scan_results = test_get_scan_results,
	.get_bss_scan_result = test_get_bss_scanresult,
	.get_bssid = test_get_bssid,
	.get_ssid = test_get_ssid,
	.get_maxrate = test_get_maxrate,
	.get_channel = test_get_channel,
	.get_bandwidth = test_get_bandwidth,
	.get_oper_stds = test_get_oper_stds,
	.get_supp_channels = test_get_supp_channels,
	.get_noise = test_get_noise,
	.get_country = test_get_country,
	.get_assoclist = test_get_assoclist,
	.iface.ap_info = test_get_ap_info,
	.radio.info = test_radio_info,
	.acs = test_acs,
	.iface.get_security = test_get_security,
	.get_beacon_ies = test_get_beacon_ies,
	.add_iface = test_add_iface,
	.del_iface = test_del_iface,
	.get_sta_info = test_get_sta_info,
	.radio.get_stats = test_radio_get_stats,
	.iface.get_stats = test_ap_get_stats,
	.disconnect_sta = test_disconnect_sta,
	.monitor_sta = test_monitor_sta,
	.get_monitor_sta = test_get_monitor_sta,
	.get_monitor_stas = test_get_monitor_stas,
	.iface.stop_wps = test_stop_wps,
	.iface.get_wps_status = test_get_wps_status,
	.iface.get_wps_pin = test_get_wps_ap_pin,
	.iface.set_wps_pin = test_set_wps_ap_pin,
	.iface.start_wps = test_start_wps,
	.radio.get_param = test_radio_get_param,
	.recv_event = test_recv_event,
	.register_event = test_register_event,
	.iface.get_caps = test_ap_get_caps,
	.radio.get_caps = test_radio_get_caps,
	/* .vendor_cmd = test_vendor_cmd, */
	.iface.get_neighbor_list = test_get_neighbor_list,
	.iface.add_neighbor = test_add_neighbor,
	.iface.del_neighbor = test_del_neighbor,
	.iface.req_beacon_report = test_req_beacon_report,
	.iface.get_beacon_report = test_get_beacon_report,
	.iface.req_bss_transition = test_req_bss_transition,
	.iface.add_vendor_ie = test_add_vendor_ie,
	.iface.del_vendor_ie = test_del_vendor_ie,
	.iface.subscribe_frame = test_iface_subscribe_frame,
	.iface.unsubscribe_frame = test_iface_unsubscribe_frame,
};
