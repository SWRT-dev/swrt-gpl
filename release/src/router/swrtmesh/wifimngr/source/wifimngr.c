/*
 * wifimngr.c - provides "wifi" ubus object
 *
 * Copyright (C) 2019 iopsys Software Solutions AB. All rights reserved.
 *
 * Author: Anjan Chanda <anjan.chanda@iopsys.eu>
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
#include <dirent.h>
#include <limits.h>
#include <net/if.h>
#include <libubox/blobmsg.h>
#include <libubox/blobmsg_json.h>
#include <libubox/uloop.h>
#include <libubox/ustream.h>
#include <libubox/utils.h>
#include <libubus.h>
#include <uci.h>

#include <easy/easy.h>
#include <wifi.h>

#include "wifimngr.h"
#include "version.h"


#ifndef BIT
#define BIT(x)	(1 << (x))
#endif


struct wifimngr_device wifi_device[WIFI_DEV_MAX_NUM];
int wifi_device_num;

/* if IF_OPER_* not defined */
#ifndef IF_OPER_UNKNOWN
enum {
	IF_OPER_UNKNOWN,
	IF_OPER_NOTPRESENT,
	IF_OPER_DOWN,
	IF_OPER_LOWERLAYERDOWN,
	IF_OPER_TESTING,
	IF_OPER_DORMANT,
	IF_OPER_UP,
};
#endif /* IF_OPER_UNKNOWN */

const char *standard_str[] = {
	"b",
	"g",
	"a",
	"n",
	"ac",
	"ax",
	"be",
	"unknown"
};

const char *guard_interval_str[] = {
	"400ns",
	"800ns",
	"1xLTF_800ns",
	"1xLTF_1600ns",
	"2xLTF_800ns",
	"2xLTF_1600ns",
	"4xLTF_800ns",
	"4xLTF_3200ns",
	"Auto"
};

#if 0	// TODO: new
const char *auth_str[] = {
	"none",
	"wep",
	"psk",
	"psk2",
	"wpa",
	"wpa2",
	"unknown"
};

const char *cipher_str[] = {
	"cipher_grp",
	"wep40",
	"tkip",
	"",
	"ccmp",
	"wep104",
	"cmac",
	"cipher_nogrp",
	"gcmp",
	"gcmp256",
	"ccmp256",
	"gmac",
	"gmac256",
	"cmac256",
	"unknown"
};
#else
/* deprecate */
const char *auth_str[] = {"OPEN", "SHARED", "WEPAUTO", "WPANONE", "WPA1", \
			"WPAPSK", "WPA2", "WPA2PSK", "Unknown"};

const char *cipher_str[] = {"NONE", "WEP", "TKIP", "AES", "CCMP256", \
			"GCMP128", "GCMP256", "Unknown"};
#endif

const char *wifi_secstr[] = {
	"NONE",
	"WEP64",
	"WEP128",
	"WPAPSK",
	"WPA2PSK",
	"WPA2PSK+FT",
	"WPA3PSK",
	"WPA2PSK+WPA3PSK",
	"WPA3PSK+FT",
	"WPA",
	"WPA2",
	"WPA3",
};

const char *wifi_cipherstr[] = {
	"CIPHER_GRP",
	"WEP40",
	"TKIP",
	"",
	"CCMP",
	"WEP104",
	"CMAC",
	"CIPHER_NOGRP",
	"GCMP",
	"GCMP256",
	"CCMP256",
	"GMAC",
	"GMAC256",
	"CMAC256",
};


const char *akm_str[] = {
	"none",
	"wpa",
	"psk",
	"ft-wpa",
	"ft-psk",
	"wpa-sha256",
	"psk-sha256",
	"tdls",
	"sae",
	"ft-sae",
	"ap-peerkey",
	"suiteB",
	"suiteB-gmac",
	"suiteB-gmac256",
	"suiteB-cmac256",
	"unknown",
};


const char *operstate_str[] = {
	"up",
	"down",
	"unknown",
	"dormant",
	"notpresent",
	"lowerdown",
	"error",
};

const int32_t bw_int[] = {
	20,
	40,
	80,
	160,
	0,
	-1,
	-1,
	8080,
	-99,
};

static const char *ifstatus_str(ifstatus_t f)
{
	const char *s;

	if (f & IFF_UP) {
		s = (f & IFF_RUNNING) ? "running" : "up";
	} else {
		s = "down";
	}

	return s;
}

static char *wifi_security_str(uint32_t sec, char *out)
{
	char x[128] = {0};
	bool sep = false;
	int i, len;


	len = sizeof(wifi_secstr)/sizeof(wifi_secstr[0]);

	for (i = 0; i < len; i++) {
		if (!!(sec & (1 << i)) && strlen(wifi_secstr[i])) {
			snprintf(x + strlen(x), 127, "%s%s", sep ? "/" : "",
						wifi_secstr[i]);
			sep = true;
		}
	}

	strncpy(out, x, strlen(x));

	return out;
}

static char *etostr(uint32_t e, char *out, size_t sizeof_out, int elen, const char **arr)
{
	char *c = "";
	int i;

	for (i = 0; i < elen; i++) {
		if (e & (1 << i)) {
			snprintf(out + strlen(out), sizeof_out - 1, "%s%s", c, arr[i]);
			c = "/";
		}
	}

	return out;
}

static const char *wifi_radio_phyname(const char *radio)
{
	int i;

	if (!radio || radio[0] == '\0')
		return NULL;

	if (wifi_device_num == 0) {
		char radios[WIFI_DEV_MAX_NUM][16] = {0};
		int num_radios;

		num_radios = wifimngr_get_wifi_devices(radios);
		if (num_radios < 0)
			return NULL;
	}

	for (i = 0; i < WIFI_DEV_MAX_NUM; i++) {
		if (!strncmp(wifi_device[i].device, radio, 16))
			return wifi_device[i].phy;
	}

	return NULL;
}

static const char *ubus_objname_to_ifname(struct ubus_object *obj)
{
	if (strstr(obj->name, "wifi.radio."))
		return obj->name + strlen("wifi.radio.");

	if (strstr(obj->name, "wifi.ap."))
		return obj->name + strlen("wifi.ap.");

	if (strstr(obj->name, "wifi.backhaul."))
		return obj->name + strlen("wifi.backhaul.");

	return "\0";
}

#define ubus_radio_to_ifname(o)	wifi_radio_phyname(ubus_objname_to_ifname(o))
#define ubus_ap_to_ifname(o)	ubus_objname_to_ifname(o)
#define ubus_sta_to_ifname(o)	ubus_objname_to_ifname(o)

static int find_phy_from_device_path(char *path, char *phy)
{
	bool found = false;
	struct dirent *p;
	DIR *d;


	d = opendir("/sys/class/ieee80211");
	if (!d)
		return -1;

	while ((p = readdir(d))) {
		char buf[PATH_MAX] = {0};
		char *phypath;

		if (!strcmp(p->d_name, "."))
			continue;

		if (!strcmp(p->d_name, ".."))
			continue;

		snprintf(buf, PATH_MAX, "/sys/class/ieee80211/%s/device", p->d_name);
		phypath = realpath(buf, NULL);
		 if (phypath) {
			if (strstr(phypath, path)) {
				strncpy(phy, p->d_name, 15);
				found = true;
			}

			free(phypath);
		 }

		 if (found)
			 break;
	}

	closedir(d);
	return found ? 0 : -1;
}

static bool phy_dir_exist(char *phy)
{
	bool found = false;
	struct dirent *p;
	DIR *d;

	d = opendir("/sys/class/ieee80211");
	if (!d)
		return found;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;

		if (!strcmp(p->d_name, ".."))
			continue;

		if (!strcmp(phy, p->d_name)) {
			found = true;
			break;
		}
	}

	closedir(d);
	return found;
}

static int uci_get_wifi_devices(char devlist[][16])
{
	struct uci_context *ctx;
	struct uci_package *pkg;
	struct uci_element *e;
	int n = 0;

	ctx = uci_alloc_context();
	if (!ctx)
		return -1;

	if (uci_load(ctx, "wireless", &pkg)) {
		uci_free_context(ctx);
		return -1;
	}

	/* flush older wifi_device */
	memset(&wifi_device, 0, WIFI_DEV_MAX_NUM * sizeof(struct wifimngr_device));
	wifi_device_num = 0;

	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);
		if (!strcmp(s->type, "wifi-device")) {
			struct uci_element *x;
			struct uci_option *op;

			memset(devlist[n], 0, 16);
			strncpy(devlist[n], s->e.name, 15);

			strncpy(wifi_device[wifi_device_num].device, s->e.name, 15);
			strncpy(wifi_device[wifi_device_num].phy, s->e.name, 15);

			if (!phy_dir_exist(s->e.name)) {
				uci_foreach_element(&s->options, x) {
					op = uci_to_option(x);
					if (!strncmp(x->name, "path", 4) &&
					    op->type == UCI_TYPE_STRING) {
						char phy[16] = {0};
						int ret;

						ret = find_phy_from_device_path(op->v.string, phy);
						if (!ret) {
							memset(wifi_device[wifi_device_num].phy, 0, 15);
							strncpy(wifi_device[wifi_device_num].phy, phy, 15);
						}
					}
				}
			}

			wifi_device_num++;

			if (++n == WIFI_DEV_MAX_NUM)
				break;
		}
	}

	uci_free_context(ctx);
	return n;
}

int wifimngr_get_wifi_devices(char devs[][16])
{
	return uci_get_wifi_devices(devs);
}

static int uci_get_wifi_interfaces(struct wifimngr_iface ifs[])
{
	struct uci_context *ctx;
	struct uci_package *pkg;
	struct uci_element *e;
	int n = 0;

	ctx = uci_alloc_context();
	if (!ctx)
		return -1;

	if (uci_load(ctx, "wireless", &pkg)) {
		uci_free_context(ctx);
		return -1;
	}

	memset(ifs, 0, sizeof(struct wifimngr_iface) * WIFI_IF_MAX_NUM);

	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);

		if (!strcmp(s->type, "wifi-iface")) {
			struct uci_element *x;
			struct uci_option *op;

			uci_foreach_element(&s->options, x) {
				op = uci_to_option(x);
				if (!strncmp(x->name, "ifname", 6) &&
					op->type == UCI_TYPE_STRING) {

					strncpy(ifs[n].iface, op->v.string, 15);
				} else if (!strncmp(x->name, "device", 6) &&
					op->type == UCI_TYPE_STRING) {

					strncpy(ifs[n].device, op->v.string, 15);
				} else if (!strncmp(x->name, "disabled", 8) &&
					   op->type == UCI_TYPE_STRING) {

					ifs[n].disabled = !!atoi(op->v.string);
				} else if (!strncmp(x->name, "mode", 4) &&
					op->type == UCI_TYPE_STRING) {

					if (!strncmp(op->v.string, "ap", 2))
						ifs[n].mode = WIFI_AP;
					else if (!strncmp(op->v.string, "wet", 3))
						ifs[n].mode = WIFI_STA;
					else if (!strncmp(op->v.string, "sta", 3))
						ifs[n].mode = WIFI_STA;
				}
			}

			if (strlen(ifs[n].iface) == 0)
				snprintf(ifs[n].iface, 16, "idx%d", n);

			if (++n == WIFI_IF_MAX_NUM)
				break;
		}
	}

	uci_free_context(ctx);
	return n;
}

int wifimngr_get_wifi_interfaces(struct wifimngr_iface ifs[])
{
	return uci_get_wifi_interfaces(ifs);
}

static const char *uci_get_wifi_option(char *type, char *name, const char *option)
{
	struct uci_context *ctx;
	struct uci_package *pkg;
	struct uci_element *e;
	const char *val = NULL;

	ctx = uci_alloc_context();
	if (!ctx)
		return NULL;

	if (uci_load(ctx, "wireless", &pkg)) {
		uci_free_context(ctx);
		return NULL;
	}

	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);
		if (!strcmp(s->type, type)) {
			val = uci_lookup_option_string(ctx, s, option);
			break;
		}
	}

	uci_free_context(ctx);
	return val ? val : NULL;
}

static void wl_dump_capabilities(enum wifi_band band, struct blob_buf *bb,
				 struct wifi_caps *caps, uint8_t *bitmap,
				 int bitmap_len)
{
	char bitmap_str[128] = {0};
	void *f;
	int i;

	f = blobmsg_open_table(bb, "capabilities");

	if ((sizeof(bitmap_str) - 1) >= (2 * bitmap_len)) {
		btostr(bitmap, bitmap_len, bitmap_str);
		blobmsg_add_string(bb, "bitmap", bitmap_str);
	}

	blobmsg_add_u8(bb, "wmm", wifi_cap_isset(bitmap, WIFI_CAP_WMM) ? true : false);
	blobmsg_add_u8(bb, "apsd", wifi_cap_isset(bitmap, WIFI_CAP_APSD) ? true : false);
	blobmsg_add_u8(bb, "shortslot", wifi_cap_isset(bitmap, WIFI_CAP_SHORT_SLOT) ? true : false);
	blobmsg_add_u8(bb, "dot11h", wifi_cap_isset(bitmap, WIFI_CAP_SPECTRUM_MGMT) ? true : false);
	blobmsg_add_u8(bb, "mu_edca", wifi_cap_isset(bitmap, WIFI_CAP_MU_EDCA) ? true : false);

	if (!!(caps->valid & WIFI_CAP_EXT_VALID)) {
		blobmsg_add_u8(bb, "2040coex", wifi_cap_isset(bitmap, WIFI_CAP_2040_COEX) ? true : false);
		blobmsg_add_u8(bb, "psmp", wifi_cap_isset(bitmap, WIFI_CAP_PSMP) ? true : false);
		blobmsg_add_u8(bb, "proxy_arp", wifi_cap_isset(bitmap, WIFI_CAP_PROXY_ARP) ? true : false);
		blobmsg_add_u8(bb, "dot11v_btm", wifi_cap_isset(bitmap, WIFI_CAP_11V_BSS_TRANS) ? true : false);
		blobmsg_add_u8(bb, "multi_bssid", wifi_cap_isset(bitmap, WIFI_CAP_MULTI_BSSID) ? true : false);
		blobmsg_add_u8(bb, "ssidlist", wifi_cap_isset(bitmap, WIFI_CAP_SSID_LIST) ? true : false);
		blobmsg_add_u8(bb, "interworking", wifi_cap_isset(bitmap, WIFI_CAP_INTERWORKING) ? true : false);
		blobmsg_add_u8(bb, "qosmap", wifi_cap_isset(bitmap, WIFI_CAP_QOSMAP) ? true : false);
		blobmsg_add_u8(bb, "tdls", wifi_cap_isset(bitmap, WIFI_CAP_TDLS) ? true : false);
		blobmsg_add_u8(bb, "qos_scs", wifi_cap_isset(bitmap, WIFI_CAP_SCS) ? true : false);
		blobmsg_add_u8(bb, "qload_report", wifi_cap_isset(bitmap, WIFI_CAP_QLOAD_REPORT) ? true : false);
		blobmsg_add_u8(bb, "omi", wifi_cap_isset(bitmap, WIFI_CAP_OMI) ? true : false);
		blobmsg_add_u8(bb, "qos_mscs", wifi_cap_isset(bitmap, WIFI_CAP_MSCS) ? true : false);
		blobmsg_add_u8(bb, "twt_requester", wifi_cap_isset(bitmap, WIFI_CAP_TWT_REQ) ? true : false);
		blobmsg_add_u8(bb, "twt_responder", wifi_cap_isset(bitmap, WIFI_CAP_TWT_RSP) ? true : false);
	}

	if (!!(caps->valid & WIFI_CAP_HT_VALID)) {
		uint8_t *supp_mcs = caps->ht.supp_mcs;
		int max_mcs = -1;
		int octet = 0;
		void *n;
		int l;

		n = blobmsg_open_table(bb, "dot11n");
		blobmsg_add_u8(bb, "dot11n_ldpc", wifi_cap_isset(bitmap, WIFI_CAP_HT_LDPC) ? true : false);
		blobmsg_add_u8(bb, "dot11n_40", wifi_cap_isset(bitmap, WIFI_CAP_2040) ? true : false);
		blobmsg_add_u8(bb, "dot11n_ps", wifi_cap_isset(bitmap, WIFI_CAP_HT_SMPS) ? true : false);
		blobmsg_add_u8(bb, "dot11n_sgi20", wifi_cap_isset(bitmap, WIFI_CAP_SGI20) ? true : false);
		blobmsg_add_u8(bb, "dot11n_sgi40", wifi_cap_isset(bitmap, WIFI_CAP_SGI40) ? true : false);
		blobmsg_add_u8(bb, "dot11n_tx_stbc", wifi_cap_isset(bitmap, WIFI_CAP_HT_TX_STBC) ? true : false);
		blobmsg_add_u8(bb, "dot11n_rx_stbc", wifi_cap_isset(bitmap, WIFI_CAP_HT_RX_STBC) ? true : false);

		for (l = 0; l < 77; l++) {
			if (l && !(l % 8))
				octet++;

			if (!!(supp_mcs[octet] & (1 << (l % 8))))
				max_mcs++;
		}

		blobmsg_add_u32(bb, "dot11n_supp_max_mcs", max_mcs);
		blobmsg_close_table(bb, n);
	}/* else {
		blobmsg_add_u8(bb, "dot11n", false);
	}*/

	if (!!(caps->valid & WIFI_CAP_VHT_VALID)) {
		void *ac;

		ac = blobmsg_open_table(bb, "dot11ac");
		blobmsg_add_u8(bb, "dot11ac_160", wifi_cap_isset(bitmap, WIFI_CAP_160) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_8080", wifi_cap_isset(bitmap, WIFI_CAP_8080) ? true : false);
		if (wifi_cap_isset(bitmap, WIFI_CAP_VHT_MPDU_11454))
			blobmsg_add_u32(bb, "dot11ac_mpdu_max", 11454);
		else if (wifi_cap_isset(bitmap, WIFI_CAP_VHT_MPDU_7991))
			blobmsg_add_u32(bb, "dot11ac_mpdu_max", 7991);
		else if (wifi_cap_isset(bitmap, WIFI_CAP_VHT_MPDU_3895))
			blobmsg_add_u32(bb, "dot11ac_mpdu_max", 3895);
		blobmsg_add_u8(bb, "dot11ac_sgi80", wifi_cap_isset(bitmap, WIFI_CAP_SGI80) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_sgi160", wifi_cap_isset(bitmap, WIFI_CAP_SGI160) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_rx_ldpc", wifi_cap_isset(bitmap, WIFI_CAP_VHT_RX_LDPC) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_tx_stbc", wifi_cap_isset(bitmap, WIFI_CAP_VHT_TX_STBC) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_rx_stbc_1ss", wifi_cap_isset(bitmap, WIFI_CAP_VHT_RX_STBC_1SS) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_rx_stbc_2ss", wifi_cap_isset(bitmap, WIFI_CAP_VHT_RX_STBC_2SS) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_rx_stbc_3ss", wifi_cap_isset(bitmap, WIFI_CAP_VHT_RX_STBC_3SS) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_rx_stbc_4ss", wifi_cap_isset(bitmap, WIFI_CAP_VHT_RX_STBC_4SS) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_su_beamformer", wifi_cap_isset(bitmap, WIFI_CAP_VHT_SU_BFR) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_su_beamformee", wifi_cap_isset(bitmap, WIFI_CAP_VHT_SU_BFE) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_mu_beamformer", wifi_cap_isset(bitmap, WIFI_CAP_VHT_MU_BFR) ? true : false);
		blobmsg_add_u8(bb, "dot11ac_mu_beamformee", wifi_cap_isset(bitmap, WIFI_CAP_VHT_MU_BFE) ? true : false);

		for (i = 0; i < 2; i++) {
			uint8_t *mcs_map = &caps->vht.supp_mcs[i*4];
			int max_mcs = -1;
			int octet = 0;
			int nss = 0;
			int l;

			for (l = 0; l < 16; l += 2) {
				uint8_t supp_mcs_mask = 0;

				if (l && !(l % 8))
					octet++;

				supp_mcs_mask = mcs_map[octet] & (0x3 << (l % 8));
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

			if (i == 0) {
				blobmsg_add_u32(bb, "dot11ac_supp_max_rx_mcs", max_mcs);
				blobmsg_add_u32(bb, "dot11ac_supp_max_rx_nss", nss);
			} else {
				blobmsg_add_u32(bb, "dot11ac_supp_max_tx_mcs", max_mcs);
				blobmsg_add_u32(bb, "dot11ac_supp_max_tx_nss", nss);
			}
		}

		blobmsg_close_table(bb, ac);
	}/* else {
		blobmsg_add_u8(bb, "dot11ac", false);
	}*/

	if (!!(caps->valid & WIFI_CAP_HE_VALID)) {
		uint8_t supp_chwidth = (caps->he.byte_phy[0] & 0xf7) >> 1;
		bool b0 = !!(supp_chwidth & BIT(0));
		//bool b1 = !!(supp_chwidth & BIT(1));
		bool b2 = !!(supp_chwidth & BIT(2));
		bool b3 = !!(supp_chwidth & BIT(3));
		int mcs_maplen = 4;
		uint8_t *mcs_map;
		void *ax;

		ax = blobmsg_open_table(bb, "dot11ax");
		blobmsg_add_u8(bb, "dot11ax_twt_requester", wifi_cap_isset(bitmap, WIFI_CAP_HE_TWT_REQ) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_twt_responder", wifi_cap_isset(bitmap, WIFI_CAP_HE_TWT_RSP) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_all_ack", wifi_cap_isset(bitmap, WIFI_CAP_HE_ALL_ACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_trs", wifi_cap_isset(bitmap, WIFI_CAP_HE_TRS) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_bsr", wifi_cap_isset(bitmap, WIFI_CAP_HE_BSR) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_twt_bcast", wifi_cap_isset(bitmap, WIFI_CAP_HE_BCAST_TWT) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ba_32bit", wifi_cap_isset(bitmap, WIFI_CAP_HE_32BIT_BA_BMP) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_om_control", wifi_cap_isset(bitmap, WIFI_CAP_HE_OM_CONTROL) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ofdma_ra", wifi_cap_isset(bitmap, WIFI_CAP_HE_OFDMA_RA) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_amsdu_frag", wifi_cap_isset(bitmap, WIFI_CAP_HE_AMSDU_FRAG) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_twt_flexible", wifi_cap_isset(bitmap, WIFI_CAP_HE_FLEX_TWT) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_qtp", wifi_cap_isset(bitmap, WIFI_CAP_HE_QTP) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_bqr", wifi_cap_isset(bitmap, WIFI_CAP_HE_BQR) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_srp_responder", wifi_cap_isset(bitmap, WIFI_CAP_HE_SRP_RSP) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ops", wifi_cap_isset(bitmap, WIFI_CAP_HE_OPS) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_amsdu_in_ampdu", wifi_cap_isset(bitmap, WIFI_CAP_HE_AMSDU_IN_AMPDU) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_dynamic_smps", wifi_cap_isset(bitmap, WIFI_CAP_HE_DYN_SMPS) ? true : false);

		blobmsg_add_u8(bb, "dot11ax_2g_40", wifi_cap_isset(bitmap, WIFI_CAP_HE_40_BAND2) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_5g_4080", wifi_cap_isset(bitmap, WIFI_CAP_HE_4080_BAND5) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_5g_160", wifi_cap_isset(bitmap, WIFI_CAP_HE_160_8080_BAND5) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_5g_160_and_8080", wifi_cap_isset(bitmap, WIFI_CAP_HE_160_8080_BAND5) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_2g_242ru", wifi_cap_isset(bitmap, WIFI_CAP_HE_242RU_BAND2) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_5g_242ru", wifi_cap_isset(bitmap, WIFI_CAP_HE_242RU_BAND2) ? true : false);

		blobmsg_add_u8(bb, "dot11ax_ldpc_payload", wifi_cap_isset(bitmap, WIFI_CAP_HE_LDPC_PAYLOAD) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_tx_stbc_80", wifi_cap_isset(bitmap, WIFI_CAP_HE_STBC_TX_80) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_rx_stbc_80", wifi_cap_isset(bitmap, WIFI_CAP_HE_STBC_RX_80) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ul_mumimo_full", wifi_cap_isset(bitmap, WIFI_CAP_HE_FULL_BW_UL_MUMIMO) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ul_mumimo_partial", wifi_cap_isset(bitmap, WIFI_CAP_HE_PART_BW_UL_MUMIMO) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_su_beamformer", wifi_cap_isset(bitmap, WIFI_CAP_HE_SU_BFR) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_su_beamformee", wifi_cap_isset(bitmap, WIFI_CAP_HE_SU_BFE) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_mu_beamformer", wifi_cap_isset(bitmap, WIFI_CAP_HE_MU_BFR) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_bfe_sts_le_80", wifi_cap_isset(bitmap, WIFI_CAP_HE_BFE_STS_LE_80MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_bfe_sts_gt_80", wifi_cap_isset(bitmap, WIFI_CAP_HE_BFE_STS_GT_80MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ng16_su_feedback", wifi_cap_isset(bitmap, WIFI_CAP_HE_NG16_SU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ng16_mu_feedback", wifi_cap_isset(bitmap, WIFI_CAP_HE_NG16_MU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_codebook_su_feedback", wifi_cap_isset(bitmap, WIFI_CAP_HE_CODEBOOK_42_SU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_codebook_mu_feedback", wifi_cap_isset(bitmap, WIFI_CAP_HE_CODEBOOK_75_MU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_triggered_su_bf_feedback", wifi_cap_isset(bitmap, WIFI_CAP_HE_TRIGGERED_SU_BF_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_triggered_mu_bf_partial_bw_feedback", wifi_cap_isset(bitmap, WIFI_CAP_HE_TRIGGERED_MU_BF_PARTIAL_BW_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_triggered_cqi_feedback", wifi_cap_isset(bitmap, WIFI_CAP_HE_TRIGGERED_CQI_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_partial_bw_er", wifi_cap_isset(bitmap, WIFI_CAP_HE_PARTIAL_BW_ER) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_dl_mumimo_partial", wifi_cap_isset(bitmap, WIFI_CAP_HE_PARTIAL_BW_DL_MUMIMO) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_ppe_thresh_present", wifi_cap_isset(bitmap, WIFI_CAP_HE_PPE_THRESHOLDS_PRESENT) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_psr_sr", wifi_cap_isset(bitmap, WIFI_CAP_HE_PSR_SR) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_doppler_tx", wifi_cap_isset(bitmap, WIFI_CAP_HE_DOPPLER_TX) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_doppler_rx", wifi_cap_isset(bitmap, WIFI_CAP_HE_DOPPLER_RX) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_stbc_tx_gt_80", wifi_cap_isset(bitmap, WIFI_CAP_HE_STBC_TX_GT_80MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_stbc_rx_gt_80", wifi_cap_isset(bitmap, WIFI_CAP_HE_STBC_RX_GT_80MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_rts", wifi_cap_isset(bitmap, WIFI_CAP_HE_RTS_TXOP_BASED) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_mu_rts", wifi_cap_isset(bitmap, WIFI_CAP_MU_RTS) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_multi_bssid", wifi_cap_isset(bitmap, WIFI_CAP_MULTI_BSSID) ? true : false);
		blobmsg_add_u8(bb, "dot11ax_mu_edca", wifi_cap_isset(bitmap, WIFI_CAP_MU_EDCA) ? true : false);


		/* get max MCS and NSS */
		if (band == BAND_5 || band == BAND_6) {
			if (b2)
				mcs_maplen += 4;
			if (b3)
				mcs_maplen += 4;
		}

		for (i = 0; i < mcs_maplen; i += 2) {
			int max_mcs = -1;
			int octet = 0;
			int nss = 0;
			int l;


			mcs_map = &caps->he.byte_opt[i];

			for (l = 0; l < 16; l += 2) {
				uint8_t supp_mcs_mask = 0;

				if (l && !(l % 8))
					octet++;

				supp_mcs_mask = mcs_map[octet] & (0x3 << (l % 8));
				supp_mcs_mask >>= (l % 8);
				if (supp_mcs_mask == 3)
					break;

				nss++;
				if (supp_mcs_mask == 0)
					max_mcs = 7;
				else if (supp_mcs_mask == 1)
					max_mcs = 9;
				else if (supp_mcs_mask == 2)
					max_mcs = 11;
			}

			if (max_mcs < 0)
				continue;

			switch (i) {
			case 0:
				if (band == BAND_2) {
					if (b0) {
						blobmsg_add_u32(bb, "dot11ax_supp_max_rx_mcs_40", max_mcs);
						blobmsg_add_u32(bb, "dot11ax_supp_max_rx_nss_40", nss);
					} else {
						blobmsg_add_u32(bb, "dot11ax_supp_max_rx_mcs_20", max_mcs);
						blobmsg_add_u32(bb, "dot11ax_supp_max_rx_nss_20", nss);
					}
				} else {
					blobmsg_add_u32(bb, "dot11ax_supp_max_rx_mcs_80", max_mcs);
					blobmsg_add_u32(bb, "dot11ax_supp_max_rx_nss_80", nss);
				}
				break;
			case 2:
				if (band == BAND_2) {
					if (b0) {
						blobmsg_add_u32(bb, "dot11ax_supp_max_tx_mcs_40", max_mcs);
						blobmsg_add_u32(bb, "dot11ax_supp_max_tx_nss_40", nss);
					} else {
						blobmsg_add_u32(bb, "dot11ax_supp_max_tx_mcs_20", max_mcs);
						blobmsg_add_u32(bb, "dot11ax_supp_max_tx_nss_20", nss);
					}
				} else {
					blobmsg_add_u32(bb, "dot11ax_supp_max_tx_mcs_80", max_mcs);
					blobmsg_add_u32(bb, "dot11ax_supp_max_tx_nss_80", nss);
				}
				break;
			case 4:
				blobmsg_add_u32(bb, "dot11ax_supp_max_rx_mcs_160", max_mcs);
				blobmsg_add_u32(bb, "dot11ax_supp_max_rx_nss_160", nss);
				break;
			case 6:
				blobmsg_add_u32(bb, "dot11ax_supp_max_tx_mcs_160", max_mcs);
				blobmsg_add_u32(bb, "dot11ax_supp_max_tx_nss_160", nss);
				break;
			case 8:
				blobmsg_add_u32(bb, "dot11ax_supp_max_rx_mcs_8080", max_mcs);
				blobmsg_add_u32(bb, "dot11ax_supp_max_rx_nss_8080", nss);
				break;
			case 10:
				blobmsg_add_u32(bb, "dot11ax_supp_max_tx_mcs_8080", max_mcs);
				blobmsg_add_u32(bb, "dot11ax_supp_max_tx_nss_8080", nss);
				break;
			default:
				break;
			}
		}
		blobmsg_close_table(bb, ax);
	}

	if (!!(caps->valid & WIFI_CAP_EHT_VALID)) {
		void *be;

		be = blobmsg_open_table(bb, "dot11be");
		blobmsg_add_u8(bb, "dot11be_epcs", wifi_cap_isset(bitmap, WIFI_CAP_EHT_EPCS) ? true : false);
		blobmsg_add_u8(bb, "dot11be_om_control", wifi_cap_isset(bitmap, WIFI_CAP_EHT_OM_CONTROL) ? true : false);
		blobmsg_add_u8(bb, "dot11be_triggered_txop_mode1", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TRIGGERED_TXOP_MODE_1) ? true : false);
		blobmsg_add_u8(bb, "dot11be_triggered_txop_mode2", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TRIGGERED_TXOP_MODE_2) ? true : false);
		blobmsg_add_u8(bb, "dot11be_rtwt", wifi_cap_isset(bitmap, WIFI_CAP_EHT_RTWT) ? true : false);
		blobmsg_add_u8(bb, "dot11be_scs_tdesc", wifi_cap_isset(bitmap, WIFI_CAP_EHT_SCS_TDESC) ? true : false);
		if (wifi_cap_isset(bitmap, WIFI_CAP_EHT_MPDU_11454))
			blobmsg_add_u32(bb, "dot11be_mpdu_max", 11454);
		else if (wifi_cap_isset(bitmap, WIFI_CAP_EHT_MPDU_7991))
			blobmsg_add_u32(bb, "dot11be_mpdu_max", 7991);
		else if (wifi_cap_isset(bitmap, WIFI_CAP_EHT_MPDU_3895))
			blobmsg_add_u32(bb, "dot11be_mpdu_max", 3895);

		blobmsg_add_u8(bb, "dot11be_trs", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TRS) ? true : false);
		blobmsg_add_u8(bb, "dot11be_txop_return_mode2", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TXOP_RETURN_IN_MODE_2) ? true : false);
		blobmsg_add_u8(bb, "dot11be_two_bqr", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TWO_BQR) ? true : false);
		blobmsg_add_u8(bb, "dot11be_link_adapt", wifi_cap_isset(bitmap, WIFI_CAP_EHT_LINK_ADAPT) ? true : false);
		blobmsg_add_u8(bb, "dot11be_6g_320", wifi_cap_isset(bitmap, WIFI_CAP_EHT_320_BAND6) ? true : false);
		blobmsg_add_u8(bb, "dot11be_242ru_bw20_plus", wifi_cap_isset(bitmap, WIFI_CAP_EHT_242RU_IN_BW_GT_20MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ndp_4xltf_gi32", wifi_cap_isset(bitmap, WIFI_CAP_EHT_NDP_WITH_4xEHT_LTF_AND_3_2_GI) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ul_mumimo_partial", wifi_cap_isset(bitmap, WIFI_CAP_EHT_PARTIAL_BW_UL_MUMIMO) ? true : false);
		blobmsg_add_u8(bb, "dot11be_dl_mumimo_partial", wifi_cap_isset(bitmap, WIFI_CAP_EHT_PARTIAL_BW_DL_MUMIMO) ? true : false);
		blobmsg_add_u8(bb, "dot11be_su_beamformer", wifi_cap_isset(bitmap, WIFI_CAP_EHT_SU_BFR) ? true : false);
		blobmsg_add_u8(bb, "dot11be_su_beamformee", wifi_cap_isset(bitmap, WIFI_CAP_EHT_SU_BFE) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ng16_su_feedback", wifi_cap_isset(bitmap, WIFI_CAP_EHT_NG16_SU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ng16_mu_feedback", wifi_cap_isset(bitmap, WIFI_CAP_EHT_NG16_MU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11be_codebook_su_feedback", wifi_cap_isset(bitmap, WIFI_CAP_EHT_CODEBOOK_42_SU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11be_codebook_mu_feedback", wifi_cap_isset(bitmap, WIFI_CAP_EHT_CODEBOOK_75_MU_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11be_triggered_cqi_feedback", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TRIGGERED_CQI_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11be_psr_sr", wifi_cap_isset(bitmap, WIFI_CAP_EHT_PSR_SR) ? true : false);
		blobmsg_add_u8(bb, "dot11be_power_boost", wifi_cap_isset(bitmap, WIFI_CAP_EHT_POWER_BOOST_FACTOR) ? true : false);
		blobmsg_add_u8(bb, "dot11be_mu_ppdu_4xltf_gi08", wifi_cap_isset(bitmap, WIFI_CAP_EHT_MU_PPDU_WITH_4xEHT_LTF_AND_0_8_GI) ? true : false);
		blobmsg_add_u8(bb, "dot11be_non_triggered_cqi_feedback", wifi_cap_isset(bitmap, WIFI_CAP_EHT_NON_TRIGGERED_CQI_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11be_tx_1k_4k_qam_lt_242ru", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TX_1K_4K_QAM_WITH_LT_242RU) ? true : false);
		blobmsg_add_u8(bb, "dot11be_rx_1k_4k_qam_lt_242ru", wifi_cap_isset(bitmap, WIFI_CAP_EHT_RX_1K_4K_QAM_WITH_LT_242RU) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ppe_thresh_present", wifi_cap_isset(bitmap, WIFI_CAP_EHT_PPE_THRESHOLDS_PRESENT) ? true : false);
		blobmsg_add_u8(bb, "dot11be_dup_in_6g", wifi_cap_isset(bitmap, WIFI_CAP_EHT_DUP_IN_BAND6) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ul_non_ofdma_80", wifi_cap_isset(bitmap, WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_80MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ul_non_ofdma_160", wifi_cap_isset(bitmap, WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_160MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ul_non_ofdma_320", wifi_cap_isset(bitmap, WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_320MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11be_mu_beamformer_80", wifi_cap_isset(bitmap, WIFI_CAP_EHT_MU_BFR_80MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11be_mu_beamformer_160", wifi_cap_isset(bitmap, WIFI_CAP_EHT_MU_BFR_160MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11be_mu_beamformer_320", wifi_cap_isset(bitmap, WIFI_CAP_EHT_MU_BFR_320MHZ) ? true : false);
		blobmsg_add_u8(bb, "dot11be_ratelimit_tb_sounding", wifi_cap_isset(bitmap, WIFI_CAP_EHT_TB_SOUND_FEEDBACK_RATELIMIT) ? true : false);
		blobmsg_add_u8(bb, "dot11be_rx_1k_qam_dl_ofdma_wider_bw", wifi_cap_isset(bitmap, WIFI_CAP_EHT_RX_1K_QAM_DL_OFDMA_WIDER_BW) ? true : false);
		blobmsg_add_u8(bb, "dot11be_rx_4k_qam_dl_ofdma_wider_bw", wifi_cap_isset(bitmap, WIFI_CAP_EHT_RX_4K_QAM_DL_OFDMA_WIDER_BW) ? true : false);
		blobmsg_add_u8(bb, "dot11be_20_only", wifi_cap_isset(bitmap, WIFI_CAP_EHT_20MHZ_ONLY) ? true : false);
		blobmsg_add_u8(bb, "dot11be_20_only_triggered_mu_bf_full_bw_feedback", wifi_cap_isset(bitmap, WIFI_CAP_EHT_20MHZ_ONLY_TRIGGERED_MU_BF_FULL_BW_FEEDBACK) ? true : false);
		blobmsg_add_u8(bb, "dot11be_20_only_mru", wifi_cap_isset(bitmap, WIFI_CAP_EHT_20MHZ_ONLY_MRU) ? true : false);

		blobmsg_close_table(bb, be);
	}


	if (!!(caps->valid & WIFI_CAP_RM_VALID)) {
		void *k;

		k = blobmsg_open_table(bb, "dot11k");
		blobmsg_add_u8(bb, "dot11k_link_meas", wifi_cap_isset(bitmap, WIFI_CAP_RM_LINK) ? true : false);
		blobmsg_add_u8(bb, "dot11k_nbr_report", wifi_cap_isset(bitmap, WIFI_CAP_RM_NBR_REPORT) ? true : false);
		blobmsg_add_u8(bb, "dot11k_bcn_passive", wifi_cap_isset(bitmap, WIFI_CAP_RM_BCN_PASSIVE) ? true : false);
		blobmsg_add_u8(bb, "dot11k_bcn_active", wifi_cap_isset(bitmap, WIFI_CAP_RM_BCN_ACTIVE) ? true : false);
		blobmsg_add_u8(bb, "dot11k_bcn_table", wifi_cap_isset(bitmap, WIFI_CAP_RM_BCN_TABLE) ? true : false);
		blobmsg_add_u8(bb, "dot11k_rcpi", wifi_cap_isset(bitmap, WIFI_CAP_RM_RCPI) ? true : false);
		blobmsg_add_u8(bb, "dot11k_rsni", wifi_cap_isset(bitmap, WIFI_CAP_RM_RSNI) ? true : false);
		blobmsg_close_table(bb, k);
	} /* else
		blobmsg_add_u8(bb, "dot11k", false); */

	//blobmsg_add_u8(bb, "dot11r", wifi_cap_isset(bitmap, WIFI_CAP_FT_BSS) ? true : false);
	blobmsg_close_table(bb, f);
}

static void wl_print_sta_stats(struct blob_buf *bb, struct wifi_sta *sx) {
	void *t, *s;
	t = blobmsg_open_table(bb, "stats");
	blobmsg_add_u64(bb, "tx_total_pkts", sx->stats.tx_pkts);
	blobmsg_add_u64(bb, "tx_total_bytes", sx->stats.tx_bytes);
	blobmsg_add_u64(bb, "tx_failures", sx->stats.tx_fail_pkts);
	blobmsg_add_u64(bb, "tx_pkts_retries", sx->stats.tx_retry_pkts);
	blobmsg_add_u64(bb, "rx_data_pkts", sx->stats.rx_pkts);
	blobmsg_add_u64(bb, "rx_data_bytes", sx->stats.rx_bytes);
	blobmsg_add_u64(bb, "rx_failures", sx->stats.rx_fail_pkts);

	s = blobmsg_open_table(bb, "tx_rate_latest");
	blobmsg_add_u32(bb, "rate", sx->tx_rate.rate);
	blobmsg_add_u32(bb, "mcs", sx->tx_rate.m.mcs);
	blobmsg_add_u32(bb, "bandwidth", sx->tx_rate.m.bw);
	blobmsg_add_u32(bb, "sgi", sx->tx_rate.m.sgi);
	blobmsg_add_u32(bb, "nss", sx->tx_rate.m.nss);
	blobmsg_add_u32(bb, "phy", sx->tx_rate.phy);
	blobmsg_close_table(bb, s);

	s = blobmsg_open_table(bb, "rx_rate_latest");
	blobmsg_add_u32(bb, "rate", sx->rx_rate.rate);
	blobmsg_add_u32(bb, "mcs", sx->rx_rate.m.mcs);
	blobmsg_add_u32(bb, "bandwidth", sx->rx_rate.m.bw);
	blobmsg_add_u32(bb, "sgi", sx->rx_rate.m.sgi);
	blobmsg_add_u32(bb, "nss", sx->rx_rate.m.nss);
	blobmsg_add_u32(bb, "phy", sx->rx_rate.phy);
	blobmsg_close_table(bb, s);
	blobmsg_close_table(bb, t);
}

static void wifi_print_radio_stats(struct blob_buf *bb,
					struct wifi_radio_stats *s)
{
	blobmsg_add_u64(bb, "tx_bytes", s->tx_bytes);
	blobmsg_add_u64(bb, "tx_packets", s->tx_pkts);
	blobmsg_add_u64(bb, "tx_error_packets", s->tx_err_pkts);
	blobmsg_add_u64(bb, "tx_dropped_packets", s->tx_dropped_pkts);

	blobmsg_add_u64(bb, "rx_bytes", s->rx_bytes);
	blobmsg_add_u64(bb, "rx_packets", s->rx_pkts);
	blobmsg_add_u64(bb, "rx_error_packets", s->rx_err_pkts);
	blobmsg_add_u64(bb, "rx_dropped_packets", s->rx_dropped_pkts);
	blobmsg_add_u64(bb, "rx_plcp_error_packets", s->rx_plcp_err_pkts);
	blobmsg_add_u64(bb, "rx_fcs_error_packets", s->rx_fcs_err_pkts);
	blobmsg_add_u64(bb, "rx_mac_error_packets", s->rx_mac_err_pkts);
	blobmsg_add_u64(bb, "rx_unknown_packets", s->rx_unknown_pkts);
}

static void wifi_print_radio_diagnostics(struct blob_buf *bb,
					struct wifi_radio_diagnostic *d)
{
	blobmsg_add_u64(bb, "channel_busy", d->channel_busy);
	blobmsg_add_u64(bb, "tx_airtime", d->tx_airtime);
	blobmsg_add_u64(bb, "rx_airtime", d->rx_airtime);
	blobmsg_add_u64(bb, "obss_airtime", d->obss_airtime);
	blobmsg_add_u64(bb, "cca_time", d->cca_time);
	blobmsg_add_u64(bb, "false_cca_count", d->false_cca_count);
}

static void wifi_print_radio_supp_bands(struct blob_buf *bb,
					uint32_t supp_band)
{
	void *a;

	a = blobmsg_open_array(bb, "supp_bands");
	if (supp_band & BAND_2)
		blobmsg_add_string(bb, "", "2.4GHz");
	if (supp_band & BAND_5)
		blobmsg_add_string(bb, "", "5GHz");
	if (supp_band & BAND_6)
		blobmsg_add_string(bb, "", "6GHz");
	if (supp_band & BAND_60)
		blobmsg_add_string(bb, "", "60GHz");
	blobmsg_close_array(bb, a);
}

static void wifi_print_radio_supp_std(struct blob_buf *bb,
				      uint32_t supp_std)
{
	void *a;

	a = blobmsg_open_array(bb, "supp_std");
	if (supp_std & WIFI_B)
		blobmsg_add_string(bb, "", "11b");
	if (supp_std & WIFI_G)
		blobmsg_add_string(bb, "", "11g");
	if (supp_std & WIFI_A)
		blobmsg_add_string(bb, "", "11a");
	if (supp_std & WIFI_N)
		blobmsg_add_string(bb, "", "11n");
	if (supp_std & WIFI_AC)
		blobmsg_add_string(bb, "", "11ac");
	if (supp_std & WIFI_AX)
		blobmsg_add_string(bb, "", "11ax");
	if (supp_std & WIFI_BE)
		blobmsg_add_string(bb, "", "11be");
	blobmsg_close_array(bb, a);
}

static void wifi_print_radio_supp_bw(struct blob_buf *bb,
				     uint32_t supp_bw)
{
	void *a;

	a = blobmsg_open_array(bb, "supp_bw");
	if (supp_bw & BIT(BW20))
		blobmsg_add_string(bb, "", "20MHz");
	if (supp_bw & BIT(BW40))
		blobmsg_add_string(bb, "", "40MHz");
	if (supp_bw & BIT(BW80))
		blobmsg_add_string(bb, "", "80MHz");
	if (supp_bw & BIT(BW160))
		blobmsg_add_string(bb, "", "160MHz");
	if (supp_bw & BIT(BW8080))
		blobmsg_add_string(bb, "", "80+80MHz");
	blobmsg_close_array(bb, a);
}

static void wifi_print_radio_cac_methods(struct blob_buf *bb,
					 uint32_t cac_methods)
{
	void *a;

	a = blobmsg_open_array(bb, "cac_methods");
	if (cac_methods & BIT(WIFI_CAC_CONTINUOUS))
		blobmsg_add_string(bb, "", "continous");
	if (cac_methods & BIT(WIFI_CAC_DEDICATED))
		blobmsg_add_string(bb, "", "continous-dedicated");
	if (cac_methods & BIT(WIFI_CAC_MIMO_REDUCED))
		blobmsg_add_string(bb, "", "mimo-reduced");
	if (cac_methods & BIT(WIFI_CAC_TIME_SLICED))
		blobmsg_add_string(bb, "", "time-sliced");
	blobmsg_close_array(bb, a);
}

int wl_radio_status(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	char std_buf2[32] = "802.11";
	char std_buf[32] = {0};
	char hwaddr_str[18] = {0};
	uint8_t hwaddr[6] = {0};
	const char *wldev;
	const char *radioname;
	ifstatus_t ifs = 0;
	uint32_t channel;
	enum wifi_bw bw;
	enum wifi_band band = BAND_2;
	unsigned long maxrate;
	int noise;
	struct wifi_metainfo minfo = {0};
	struct wifi_radio radio;
	struct wifi_opclass opclass;
	struct wifi_opclass supp_opclass[64] = {0};
	int num_opclass;
	struct blob_buf bb;
	void *c, *d, *dd;
	int i, j;


	memset(&bb, 0, sizeof(bb));
	wldev = ubus_radio_to_ifname(obj);
	radioname = ubus_objname_to_ifname(obj);

	wifi_radio_get_ifstatus(wldev, &ifs);
	wifi_get_channel(wldev, &channel, &bw);
	wifi_get_maxrate(wldev, &maxrate);
	wifi_get_noise(wldev, &noise);

	memset(&radio, 0, sizeof(struct wifi_radio));
	wifi_radio_info(wldev, &radio);

	band = radio.oper_band;

	if (wifi_radio_get_hwaddr(wldev, hwaddr) != 0)
		if_gethwaddr(wldev, hwaddr);
	hwaddr_ntoa(hwaddr, hwaddr_str);

	memset(&opclass, 0, sizeof(struct wifi_opclass));
	wifi_get_curr_opclass(wldev, &opclass);

	num_opclass = ARRAY_SIZE(supp_opclass);
	wifi_get_supp_opclass(wldev, &num_opclass, supp_opclass);

	wifi_driver_info(wldev, &minfo);

	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "radio", radioname);
	blobmsg_add_string(&bb, "phyname", wldev);
	blobmsg_add_string(&bb, "macaddr", hwaddr_str);
	blobmsg_add_string(&bb, "firmware", minfo.fw_data);
	blobmsg_add_string(&bb, "vendor_id", minfo.vendor_id);
	blobmsg_add_string(&bb, "device_id", minfo.device_id);

	blobmsg_add_u8(&bb, "isup", (ifs & IFF_UP) ? true : false);
	blobmsg_add_string(&bb, "band",
			!!(band & BAND_5) ? "5GHz" :
			!!(band & BAND_6) ? "6GHz" :
			!!(band & BAND_2) ? "2.4GHz" : "Unknown");

	snprintf(std_buf2 + strlen(std_buf2), 31, "%s",
		etostr(radio.oper_std, std_buf, sizeof(std_buf), WIFI_NUM_STD, standard_str));
	blobmsg_add_string(&bb, "standard", std_buf2);

	blobmsg_add_u32(&bb, "num_iface", radio.num_iface);
	c = blobmsg_open_array(&bb, "iface");
	for (i = 0; i < radio.num_iface; i++) {
		d = blobmsg_open_table(&bb, "");
		blobmsg_add_string(&bb, "name", radio.iface[i].name);
		if ( radio.iface[i].mode == WIFI_MODE_AP )
			blobmsg_add_string(&bb, "mode", "ap");
		else if ( radio.iface[i].mode == WIFI_MODE_AP_VLAN )
			blobmsg_add_string(&bb, "mode", "ap-vlan");
		else if ( radio.iface[i].mode == WIFI_MODE_STA)
			blobmsg_add_string(&bb, "mode", "sta");
		else if ( radio.iface[i].mode == WIFI_MODE_MONITOR)
			blobmsg_add_string(&bb, "mode", "monitor");
		else
			blobmsg_add_string(&bb, "mode", "unknown");
		blobmsg_close_table(&bb, d);
	}
	blobmsg_close_array(&bb, c);

	blobmsg_add_u32(&bb, "opclass", opclass.g_opclass);
	blobmsg_add_u32(&bb, "channel", channel);
	blobmsg_add_u32(&bb, "bandwidth", bw_int[bw]);
	blobmsg_add_string(&bb, "channel_ext",
			radio.extch == EXTCH_NONE ? "none" :
			radio.extch == EXTCH_ABOVE ? "above" :
			radio.extch == EXTCH_BELOW ? "below" : "auto");
	blobmsg_add_u32(&bb, "tx_streams", radio.tx_streams);
	blobmsg_add_u32(&bb, "rx_streams", radio.rx_streams);
	blobmsg_add_u32(&bb, "noise", noise);
	blobmsg_add_string(&bb, "guard_int", guard_interval_str[radio.gi]);
	blobmsg_add_u64(&bb, "maxrate", maxrate);

	wifi_print_radio_supp_bands(&bb, radio.supp_band);
	wifi_print_radio_supp_std(&bb, radio.supp_std);
	wifi_print_radio_supp_bw(&bb, radio.supp_bw);
	wifi_print_radio_cac_methods(&bb, radio.cac_methods);

	c = blobmsg_open_array(&bb, "supp_rates");
	for (i = 0; i < 32 && radio.supp_rates[i] != 0; i++) {
		blobmsg_add_u32(&bb, "", radio.supp_rates[i]);
	}

	blobmsg_close_array(&bb, c);
	c = blobmsg_open_array(&bb, "basic_rates");
	for (i = 0; i < 32 && radio.basic_rates[i] != 0; i++) {
		blobmsg_add_u32(&bb, "", radio.basic_rates[i]);
	}
	blobmsg_close_array(&bb, c);

	c = blobmsg_open_array(&bb, "supp_channels");
#if 0
	for (i = 0; i < 32 && radio.supp_channels[i] != 0; i++) {
		blobmsg_add_u32(&bb, "", radio.supp_channels[i]);
	}
#else
	for (i = 0; i < num_opclass; i++) {
		d = blobmsg_open_table(&bb, "");
		blobmsg_add_u32(&bb, "opclass", supp_opclass[i].g_opclass);
		blobmsg_add_u32(&bb, "bandwidth", bw_int[supp_opclass[i].bw]);
		blobmsg_add_u32(&bb, "txpower", supp_opclass[i].opchannel.txpower);
		dd = blobmsg_open_array(&bb, "channels");
		for (j = 0; j < supp_opclass[i].opchannel.num; j++)
			blobmsg_add_u32(&bb, "", supp_opclass[i].opchannel.ch[j].channel);
		blobmsg_close_array(&bb, dd);
		blobmsg_close_table(&bb, d);
	}
#endif
	blobmsg_close_array(&bb, c);

	if (radio.acs_capable) {
		c = blobmsg_open_table(&bb, "autochannel");
		blobmsg_add_u8(&bb, "enabled", radio.acs_enabled ? true : false);
		blobmsg_add_u32(&bb, "interval", radio.acs_interval);
		blobmsg_close_table(&bb, c);
	}

	blobmsg_add_u32(&bb, "txpower_dbm", radio.txpower_dbm);
	blobmsg_add_u32(&bb, "txpower", radio.txpower);
	blobmsg_add_u8(&bb, "dot11h_capable", radio.dot11h_capable ? true : false);
	blobmsg_add_u8(&bb, "dot11h_enabled", radio.dot11h_enabled ? true : false);
	blobmsg_add_string(&bb, "regdomain", radio.regdomain);

	blobmsg_add_u32(&bb, "beacon_int", radio.beacon_int);
	blobmsg_add_u32(&bb, "dtim_period", radio.dtim_period);

	blobmsg_add_string(&bb, "preamble",
			radio.pr == SHORT_PREAMBLE ? "short" :
			radio.pr == LONG_PREAMBLE ? "long" : "auto");

	blobmsg_add_u32(&bb, "short_retry_limit", radio.srl);
	blobmsg_add_u32(&bb, "long_retry_limit", radio.lrl);
	blobmsg_add_u32(&bb, "frag_threshold", radio.frag);
	blobmsg_add_u32(&bb, "rts_threshold", radio.rts);
	blobmsg_add_u8(&bb, "aggr_enabled", radio.aggr_enable ? true : false);

	c = blobmsg_open_table(&bb, "stats");
	wifi_print_radio_stats(&bb, &radio.stats);
	blobmsg_close_table(&bb, c);

	c = blobmsg_open_table(&bb, "diagnostics");
	wifi_print_radio_diagnostics(&bb, &radio.diag);
	blobmsg_close_table(&bb, c);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static int wl_radio_stats(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	const char *ifname;
	struct wifi_radio_stats s;
	struct blob_buf bb;
	int ret;

	ifname = ubus_radio_to_ifname(obj);
	ret = wifi_radio_get_stats(ifname, &s);
	if (ret != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);
	wifi_print_radio_stats(&bb, &s);
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}


static int dump_beacon(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	char iestr[513] = {0};	/* 2 x 256 + 1 */
	struct blob_buf bb;
	const char *ifname;
	uint8_t *bcn;
	uint8_t *ie;
	int len = 2400;
	int ret;
	void *a;

	memset(&bb, 0, sizeof(bb));

	ifname = ubus_ap_to_ifname(obj);

	bcn = calloc(1, len * sizeof(uint8_t));
	if (!bcn)
		return UBUS_STATUS_UNKNOWN_ERROR;

	ret = wifi_get_beacon_ies(ifname, bcn, &len);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	blob_buf_init(&bb, 0);
	a = blobmsg_open_array(&bb, "beacon-ies");
	wifi_foreach_ie(ie, bcn, len) {
		btostr(ie, ie[1] + 2, iestr);
		blobmsg_add_string(&bb, "", iestr);
		memset(iestr, 0, sizeof(iestr));
	}
	blobmsg_close_array(&bb, a);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);
	free(bcn);

	return 0;
}

static void wl_ap_wmm_status(struct blob_buf *bb, struct wifi_ap_wmm_ac ac[])
{
	const char *ac_str[] = { "BE", "BK", "VI", "VO" };
	void *a, *t;
	int i;

	a = blobmsg_open_array(bb, "wmm_params");
	for (i = 0; i < WIFI_NUM_AC; i++) {
		t = blobmsg_open_table(bb, "");
		blobmsg_add_string(bb, "ac", ac_str[ac[i].ac]);
		blobmsg_add_u32(bb, "aifsn", ac[i].aifsn);
		blobmsg_add_u32(bb, "cwmin", ac[i].cwmin);
		blobmsg_add_u32(bb, "cwmax", ac[i].cwmax);
		blobmsg_add_u32(bb, "txop", ac[i].txop);
		blobmsg_close_table(bb, t);
	}
	blobmsg_close_array(bb, a);
}

static void wl_dump_supp_security(struct blob_buf *bb, uint32_t sec)
{
	void *a;


	a = blobmsg_open_array(bb, "supp_security");

	if (!!(sec & BIT(WIFI_SECURITY_NONE)))
		blobmsg_add_string(bb, "", "NONE");
	if (!!(sec & BIT(WIFI_SECURITY_WEP64)))
		blobmsg_add_string(bb, "", "WEP64");
	if (!!(sec & BIT(WIFI_SECURITY_WEP128)))
		blobmsg_add_string(bb, "", "WEP128");
	if (!!(sec & BIT(WIFI_SECURITY_WPAPSK)))
		blobmsg_add_string(bb, "", "WPAPSK");
	if (!!(sec & BIT(WIFI_SECURITY_WPA2PSK)))
		blobmsg_add_string(bb, "", "WPA2PSK");
	if (!!(sec & BIT(WIFI_SECURITY_WPA3PSK)))
		blobmsg_add_string(bb, "", "WPA3PSK");
	if (!!(sec & BIT(WIFI_SECURITY_WPAPSK)) && (sec & BIT(WIFI_SECURITY_WPA2PSK)))
		blobmsg_add_string(bb, "", "WPAPSK+WPA2PSK");
	if (!!(sec & BIT(WIFI_SECURITY_WPA2PSK)) && (sec & BIT(WIFI_SECURITY_WPA3PSK)))
		blobmsg_add_string(bb, "", "WPA2PSK+WPA3PSK");
	if (!!(sec & BIT(WIFI_SECURITY_WPA)))
		blobmsg_add_string(bb, "", "WPA");
	if (!!(sec & BIT(WIFI_SECURITY_WPA2)))
		blobmsg_add_string(bb, "", "WPA2");
	if (!!(sec & BIT(WIFI_SECURITY_WPA3)))
		blobmsg_add_string(bb, "", "WPA3");
	if (!!(sec & BIT(WIFI_SECURITY_WPA)) && (sec & BIT(WIFI_SECURITY_WPA2)))
		blobmsg_add_string(bb, "", "WPA+WPA2");
	if (!!(sec & BIT(WIFI_SECURITY_WPA2)) && (sec & BIT(WIFI_SECURITY_WPA3)))
		blobmsg_add_string(bb, "", "WPA2+WPA3");
	if (!!(sec & BIT(WIFI_SECURITY_FT_WPA3PSK)))
		blobmsg_add_string(bb, "", "WPA3PSK+FT");
	if (!!(sec & BIT(WIFI_SECURITY_FT_WPA2PSK)))
		blobmsg_add_string(bb, "", "WPA2PSK+FT");

	blobmsg_close_array(bb, a);
}

static int wl_ap_status(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	const char *ifname;
	ifstatus_t ifs = 0;
	ifopstatus_t opstatus;
	uint8_t bssid[6] = {0};
	char bssid_str[24] = {0};
	char ssid[64] = {0};
	uint32_t auth;
	uint32_t enc;
	//unsigned long rate;
	char enc_buf[32] = {0};
	struct wifi_ap ap;
	struct wifi_bss *bss = &ap.bss;
	struct blob_buf bb;
	uint32_t channel;
	enum wifi_bw bw;
	char std_buf[32] = {0};
	char std_buf2[32] = "802.11";
	char sec_str[128] = {0};
	enum wifi_band band = BAND_2;

	memset(&bb, 0, sizeof(bb));
	memset(&ap, 0, sizeof(struct wifi_ap));

	ifname = ubus_ap_to_ifname(obj);

	wifi_radio_get_ifstatus(ifname, &ifs);
	wifi_get_ifoperstatus(ifname, &opstatus);
	if (opstatus > IF_OPER_UP) {
		opstatus = IF_OPER_UNKNOWN;
	}

	wifi_get_channel(ifname, &channel, &bw);
	wifi_get_bandwidth(ifname, &bw);
	wifi_get_oper_band(ifname, &band);
	wifi_ap_info(ifname, &ap);

	wifi_get_bssid(ifname, bssid);
	hwaddr_ntoa(bssid, bssid_str);
	wifi_get_ssid(ifname, ssid);
	//wifi_get_security(ifname, &akm, &enc, &g_enc);	// TODO: new
	wifi_security_str(ap.bss.security, sec_str);


	wifi_get_security(ifname, &auth, &enc);

	//wifi_get_bitrate(ifname, &rate);
	//wifi_get_noise(ifname, &noise);

	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "ifname", ifname);
	blobmsg_add_u8(&bb, "enabled", ap.enabled ? true : false);
	blobmsg_add_string(&bb, "status", ifstatus_str(ifs));
	//blobmsg_add_string(&bb, "operstate", operstate_str[opstatus]);
	blobmsg_add_string(&bb, "ssid", ssid);
	blobmsg_add_string(&bb, "bssid", bssid_str);

	if (!strcmp(sec_str, "NONE") && (enc & CIPHER_WEP))
		sprintf(sec_str, "WEP");

	blobmsg_add_string(&bb, "security", sec_str);	// TODO: new
	if (enc == CIPHER_NONE)
		blobmsg_add_string(&bb, "encryption", "DISABLED");
	else if (!!(enc & CIPHER_WEP)) {
		blobmsg_add_string(&bb, "encryption", "WEP");
	} else {
		blobmsg_add_string(&bb, "encryption",
			etostr(ap.bss.rsn.pair_ciphers, enc_buf, sizeof(enc_buf), 14, wifi_cipherstr));
	}

	blobmsg_add_u32(&bb, "channel", channel);
	blobmsg_add_u32(&bb, "bandwidth", bw_int[bw]);
	//blobmsg_add_u64(&bb, "rate", rate);
	snprintf(std_buf2 + strlen(std_buf2), 31, "%s",
		etostr(ap.bss.oper_std, std_buf, sizeof(std_buf), WIFI_NUM_STD, standard_str));
	blobmsg_add_string(&bb, "standard", std_buf2);
	blobmsg_add_u32(&bb, "num_stations", ap.bss.load.sta_count);
	blobmsg_add_u32(&bb, "max_stations", ap.assoclist_max);
	blobmsg_add_u32(&bb, "utilization", ap.bss.load.utilization);
	blobmsg_add_u32(&bb, "adm_capacity", ap.bss.load.available);
	blobmsg_add_u8(&bb, "hidden", !ap.ssid_advertised ? true : false);

	wl_dump_supp_security(&bb, ap.sec.supp_modes);


	wl_dump_capabilities(band, &bb, &bss->caps, bss->cbitmap, sizeof(bss->cbitmap));

	wl_ap_wmm_status(&bb, ap.ac);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static void wl_ap_wmm_stats(struct blob_buf *bb, struct wifi_ap_wmm_ac ac[])
{
	const char *ac_str[] = { "BE", "BK", "VI", "VO" };
	void *a, *t;
	int i;

	a = blobmsg_open_array(bb, "wmm_stats");
	for (i = 0; i < WIFI_NUM_AC; i++) {
		t = blobmsg_open_table(bb, "");
		blobmsg_add_string(bb, "ac", ac_str[ac[i].ac]);
		blobmsg_add_u64(bb, "tx_bytes", ac[i].stats.tx_bytes);
		blobmsg_add_u32(bb, "tx_packets", ac[i].stats.tx_pkts);
		blobmsg_add_u32(bb, "tx_error_packets", ac[i].stats.tx_err_pkts);
		blobmsg_add_u32(bb, "tx_retrans_packets", ac[i].stats.tx_rtx_pkts);

		blobmsg_add_u64(bb, "rx_bytes", ac[i].stats.rx_bytes);
		blobmsg_add_u32(bb, "rx_packets", ac[i].stats.rx_pkts);
		blobmsg_add_u32(bb, "rx_error_packets", ac[i].stats.rx_err_pkts);

		blobmsg_close_table(bb, t);
	}
	blobmsg_close_array(bb, a);
}

static int wl_ap_stats(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct wifi_ap_stats s;
	struct wifi_ap ap;
	const char *ifname;
	struct blob_buf bb;
	int ret;

	ifname = ubus_ap_to_ifname(obj);

	ret = wifi_ap_get_stats(ifname, &s);
	if (ret != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(&ap, 0, sizeof(struct wifi_ap));
	ret = wifi_ap_info(ifname, &ap);

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);
	blobmsg_add_u64(&bb, "tx_bytes", s.tx_bytes);
	blobmsg_add_u64(&bb, "tx_packets", s.tx_pkts);
	blobmsg_add_u64(&bb, "tx_unicast_packets", s.tx_ucast_pkts);
	blobmsg_add_u64(&bb, "tx_multicast_packets", s.tx_mcast_pkts);
	blobmsg_add_u64(&bb, "tx_broadcast_packets", s.tx_bcast_pkts);
	blobmsg_add_u64(&bb, "tx_error_packets", s.tx_err_pkts);
	blobmsg_add_u64(&bb, "tx_retrans_packets", s.tx_rtx_pkts);
	blobmsg_add_u64(&bb, "tx_retrans_fail_packets", s.tx_rtx_fail_pkts);
	blobmsg_add_u64(&bb, "tx_retry_packets", s.tx_retry_pkts);
	blobmsg_add_u64(&bb, "tx_multi_retry_packets", s.tx_mretry_pkts);
	blobmsg_add_u64(&bb, "tx_dropped_packets", s.tx_dropped_pkts);
	blobmsg_add_u64(&bb, "ack_fail_packets", s.ack_fail_pkts);
	blobmsg_add_u64(&bb, "aggregate_packets", s.aggr_pkts);

	blobmsg_add_u64(&bb, "rx_bytes", s.rx_bytes);
	blobmsg_add_u64(&bb, "rx_packets", s.rx_pkts);
	blobmsg_add_u64(&bb, "rx_unicast_packets", s.rx_ucast_pkts);
	blobmsg_add_u64(&bb, "rx_multicast_packets", s.rx_mcast_pkts);
	blobmsg_add_u64(&bb, "rx_broadcast_packets", s.rx_bcast_pkts);
	blobmsg_add_u64(&bb, "rx_error_packets", s.rx_err_pkts);
	blobmsg_add_u64(&bb, "rx_dropped_packets", s.rx_dropped_pkts);
	blobmsg_add_u64(&bb, "rx_unknown_packets", s.rx_unknown_pkts);

	wl_ap_wmm_stats(&bb, ap.ac);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static int wl_assoclist(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	void *a, *t;
	int i;
	const char *ifname;
	unsigned char stas[768] = {0};	/* max 128 per interface */
	int nr = 128;
	struct blob_buf bb;

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);

	ifname = ubus_ap_to_ifname(obj);

	if (wifi_get_assoclist(ifname, stas, &nr) != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	a = blobmsg_open_array(&bb, "assoclist");
	for (i = 0; i < nr; i++) {
		unsigned char *sta = &stas[i*6];
		char sta_macstr[18] = {0};

		t = blobmsg_open_table(&bb, "");
		blobmsg_add_string(&bb, "wdev", ifname);
		snprintf(sta_macstr, 18, "%02x:%02x:%02x:%02x:%02x:%02x",
			sta[0], sta[1], sta[2], sta[3], sta[4], sta[5]);
		blobmsg_add_string(&bb, "macaddr", sta_macstr);
		blobmsg_close_table(&bb, t);
	}
	blobmsg_close_array(&bb, a);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

enum {
	CHAN_SWITCH_COUNT,
	CHAN_SWITCH_FREQ,
	CHAN_SWITCH_SEC_CHAN_OFFSET,
	CHAN_SWITCH_CF1,
	CHAN_SWITCH_CF2,
	CHAN_SWITCH_BW,
	CHAN_SWITCH_BLOCK_TX,
	CHAN_SWITCH_HT,
	CHAN_SWITCH_VHT,
	CHAN_SWITCH_HE,
	__CHAN_SWITCH_MAX,
};

static const struct blobmsg_policy chan_switch_policy[__CHAN_SWITCH_MAX] = {
	[CHAN_SWITCH_COUNT] = { .name = "count", .type = BLOBMSG_TYPE_INT32 },
	[CHAN_SWITCH_FREQ] = { .name = "freq", .type = BLOBMSG_TYPE_INT32 },
	[CHAN_SWITCH_SEC_CHAN_OFFSET] = { .name = "sec_chan_offset", .type = BLOBMSG_TYPE_INT32 },
	[CHAN_SWITCH_CF1] = { .name = "cf1", .type = BLOBMSG_TYPE_INT32 },
	[CHAN_SWITCH_CF2] = { .name = "cf2", .type = BLOBMSG_TYPE_INT32 },
	[CHAN_SWITCH_BW] = { .name = "bw", .type = BLOBMSG_TYPE_INT32 },
	[CHAN_SWITCH_BLOCK_TX] = { .name = "blocktx", .type = BLOBMSG_TYPE_BOOL },
	[CHAN_SWITCH_HT] = { .name = "ht", .type = BLOBMSG_TYPE_BOOL },
	[CHAN_SWITCH_VHT] = { .name = "vht", .type = BLOBMSG_TYPE_BOOL },
	[CHAN_SWITCH_HE] = { .name = "he", .type = BLOBMSG_TYPE_BOOL },
};

static int ap_chan_switch(struct ubus_context *ctx, struct ubus_object *obj,
			  struct ubus_request_data *req, const char *method,
			  struct blob_attr *msg)
{
	struct blob_attr *tb[__CHAN_SWITCH_MAX];
	struct chan_switch_param param = {};
	const char *ifname;
	int ret;

	blobmsg_parse(chan_switch_policy, __CHAN_SWITCH_MAX, tb, blob_data(msg),
		      blob_len(msg));

	ifname = ubus_ap_to_ifname(obj);

	if (!tb[CHAN_SWITCH_COUNT])
		return UBUS_STATUS_INVALID_ARGUMENT;
	if (!tb[CHAN_SWITCH_FREQ])
		return UBUS_STATUS_INVALID_ARGUMENT;

	param.count = blobmsg_get_u32(tb[CHAN_SWITCH_COUNT]);
	param.freq = blobmsg_get_u32(tb[CHAN_SWITCH_FREQ]);

	if (tb[CHAN_SWITCH_SEC_CHAN_OFFSET])
		param.sec_chan_offset = blobmsg_get_u32(tb[CHAN_SWITCH_SEC_CHAN_OFFSET]);
	if (tb[CHAN_SWITCH_CF1])
		param.cf1 = blobmsg_get_u32(tb[CHAN_SWITCH_CF1]);
	if (tb[CHAN_SWITCH_CF2])
		param.cf2 = blobmsg_get_u32(tb[CHAN_SWITCH_CF2]);
	if (tb[CHAN_SWITCH_BW])
		param.bandwidth = blobmsg_get_u32(tb[CHAN_SWITCH_BW]);
	if (tb[CHAN_SWITCH_BLOCK_TX])
		param.blocktx = blobmsg_get_bool(tb[CHAN_SWITCH_BLOCK_TX]);
	if (tb[CHAN_SWITCH_HT])
		param.ht = blobmsg_get_bool(tb[CHAN_SWITCH_HT]);
	if (tb[CHAN_SWITCH_VHT])
		param.vht = blobmsg_get_bool(tb[CHAN_SWITCH_VHT]);
	if (tb[CHAN_SWITCH_HE])
		param.he = blobmsg_get_bool(tb[CHAN_SWITCH_HE]);

	ret = wifi_chan_switch(ifname, &param);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return 0;
}

/* detailed stainfo policy */
enum {
	STAINFO_MACADDR,    /* sta macaddress */
	__STAINFO_MAX,
};

static const struct blobmsg_policy stainfo_policy[__STAINFO_MAX] = {
	[STAINFO_MACADDR] = { .name = "sta", .type = BLOBMSG_TYPE_STRING },
};

static int wl_dump_stations(struct blob_buf *bb, const char *ifname,
				unsigned char *macaddr)
{
	void *t, *r, *s;
	int i, j;
	struct wifi_sta sx;
	enum wifi_bw bw;
	int noise, rssi, snr;
	unsigned char stas[768] = {0};
	int nr = 64;

	if (wifi_get_assoclist(ifname, stas, &nr) != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	for (i = 0; i < nr; i++) {
		uint32_t channel = 0;
		unsigned char *sta = &stas[i*6];
		char sta_macstr[18] = {0};
		enum wifi_band band = BAND_5;

		if (macaddr && !hwaddr_is_zero(macaddr)
				&& memcmp(macaddr, sta, 6))
			continue;

		memset(&sx, 0, sizeof(sx));
		if (wifi_get_sta_info(ifname, sta, &sx))
			continue;

		wifi_get_channel(ifname, &channel, &bw);
		wifi_get_oper_band(ifname, &band);
		wifi_get_noise(ifname, &noise);
		wifi_get_bandwidth(ifname, &bw);
		rssi = sx.rssi[0];
		snr = rssi - noise;



		t = blobmsg_open_table(bb, "");
		hwaddr_ntoa(sx.macaddr, sta_macstr);
		blobmsg_add_string(bb, "macaddr", sta_macstr);
		blobmsg_add_string(bb, "wdev", ifname);
		blobmsg_add_u32(bb, "channel", channel);
		blobmsg_add_u32(bb, "frequency", wifi_channel_to_freq_ex(channel, band));
		blobmsg_add_u32(bb, "rssi", rssi);
		blobmsg_add_u32(bb, "noise", sx.noise[0] == 0 ? noise : sx.noise[0]);
		blobmsg_add_u32(bb, "snr", snr);
		blobmsg_add_u32(bb, "idle", sx.idle_time);
		blobmsg_add_u64(bb, "in_network", sx.conn_time);
		blobmsg_add_u32(bb, "tx_airtime", sx.tx_airtime);
		blobmsg_add_u32(bb, "rx_airtime", sx.rx_airtime);
		blobmsg_add_u32(bb, "airtime", sx.airtime);
		blobmsg_add_u32(bb, "maxrate", sx.maxrate);
		blobmsg_add_u32(bb, "nss", sx.rate.m.nss);
		blobmsg_add_u32(bb, "bandwidth", bw_int[sx.rate.m.bw]);
		blobmsg_add_u32(bb, "est_rx_thput", sx.est_rx_thput);
		blobmsg_add_u32(bb, "est_tx_thput", sx.est_tx_thput);

		s = blobmsg_open_table(bb, "status");
		blobmsg_add_u8(bb, "wmm",
			wifi_status_isset(sx.sbitmap, WIFI_STATUS_WMM) ? true : false);
		blobmsg_add_u8(bb, "ps",
			wifi_status_isset(sx.sbitmap, WIFI_STATUS_PS) ? true : false);
		blobmsg_close_table(bb, s);

		wl_dump_capabilities(band, bb, &sx.caps, sx.cbitmap, sizeof(sx.cbitmap));
		wl_print_sta_stats(bb, &sx);

		r = blobmsg_open_array(bb, "rssi_per_antenna");
		for (j = 0; j < WIFI_NUM_ANTENNA; j++)
			blobmsg_add_u32(bb, "", sx.rssi[j]);
		blobmsg_close_array(bb, r);
		blobmsg_close_table(bb, t);
	}

	return 0;
}

static int wl_stations(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__STAINFO_MAX];
	char sta_macstr[18] = {0};
	uint8_t sta[6] = {0};
	const char *ifname;
	struct blob_buf bb;
	void *a;

	memset(&bb, 0, sizeof(bb));
	blobmsg_parse(stainfo_policy, __STAINFO_MAX, tb, blob_data(msg), blob_len(msg));

	ifname = ubus_ap_to_ifname(obj);

	if ((tb[STAINFO_MACADDR])) {
		strncpy(sta_macstr, blobmsg_data(tb[STAINFO_MACADDR]),
					sizeof(sta_macstr)-1);
		if (hwaddr_aton(sta_macstr, sta) == NULL) {
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
	}

	blob_buf_init(&bb, 0);
	a = blobmsg_open_array(&bb, "stations");
	wl_dump_stations(&bb, ifname, sta);
	blobmsg_close_array(&bb, a);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

enum {
	SCAN_SSID,
	SCAN_BSSID,
	SCAN_CHANNEL,
	SCAN_OPCLASS,
	/* SCAN_TYPE,    // active, passsive, background */
	__SCAN_MAX,
};

static const struct blobmsg_policy wl_scan_policy[__SCAN_MAX] = {
	[SCAN_SSID] = { .name = "ssid", .type = BLOBMSG_TYPE_STRING },
	[SCAN_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_STRING },
	[SCAN_CHANNEL] = { .name = "channel", .type = BLOBMSG_TYPE_INT32 },
	[SCAN_OPCLASS] = { .name = "opclass", .type = BLOBMSG_TYPE_INT32 },
	/* [SCAN_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 }, */
};

enum {
	SCAN_EX_SSID,
	SCAN_EX_BSSID,
	SCAN_EX_OPCLASS,
	SCAN_EX_CHANNEL,
	// SCAN_EX_TYPE,    // active, passsive, cached */
	__SCAN_EX_MAX,
};

static const struct blobmsg_policy wl_scan_ex_policy[__SCAN_MAX] = {
	[SCAN_EX_SSID] = { .name = "ssid", .type = BLOBMSG_TYPE_ARRAY },
	[SCAN_EX_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_STRING },
	[SCAN_EX_OPCLASS] = { .name = "opclass", .type = BLOBMSG_TYPE_ARRAY },
	[SCAN_EX_CHANNEL] = { .name = "channel", .type = BLOBMSG_TYPE_ARRAY },
	// [SCAN_EX_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
};

int wl_scan(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	struct blob_attr *tb[__SCAN_MAX];
	struct scan_param p = {0};
	const char *ifname;
	int ret;

	blobmsg_parse(wl_scan_policy, __SCAN_MAX, tb, blob_data(msg),
			blob_len(msg));

	ifname = ubus_radio_to_ifname(obj);

	if (tb[SCAN_SSID])
		strncpy(p.ssid, blobmsg_data(tb[SCAN_SSID]), 32);

	if (tb[SCAN_CHANNEL] && tb[SCAN_OPCLASS]) {
		wifimngr_err("%s(): Either opclass or channel allowed\n",
			     __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (tb[SCAN_CHANNEL])
		p.channel = blobmsg_get_u32(tb[SCAN_CHANNEL]);

	if (tb[SCAN_OPCLASS])
		p.opclass = blobmsg_get_u32(tb[SCAN_OPCLASS]);

	ret = wifi_scan(ifname, &p);

	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

#define UBUS_STATUS_BUSY 16

int wl_scan_ex(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	struct blob_attr *tb[__SCAN_MAX];
	struct scan_param_ex sp = {0};
	const char *rname;
	uint32_t opclasses[128];
	uint32_t channels[128];
	int op_idx = 0, ch_idx = 0, ret;
	enum wifi_band oper_band = BAND_2;

	blobmsg_parse(wl_scan_ex_policy, __SCAN_EX_MAX, tb, blob_data(msg),
			blob_len(msg));

	rname = ubus_radio_to_ifname(obj);
	wifi_get_oper_band(rname, &oper_band);

	/* example calls:
	 * ubus call wifi.radio.wl0 scan_ex '{"ssid":["iopsysWrt-44D4376AF4C0"]}'
	 * ubus call wifi.radio.wl0 scan_ex '{"channel":[36]}'
	 * ubus call wifi.radio.wl0 scan_ex '{"opclass":[115]}'
	 * ubus call wifi.radio.wl0 scan_ex '{"opclass":[115, 118], "channel":[36, 40, 52]}'
	 */

	/* array of ssids */
	if (tb[SCAN_EX_SSID]) {
		struct blob_attr *attr;
		int rem, i = 0;

		sp.flag |= WIFI_SCAN_REQ_SSID;
		sp.num_ssid = blobmsg_check_array(
				tb[SCAN_EX_SSID], BLOBMSG_TYPE_STRING);

		wifimngr_dbg("scan_ex: num_ssid = %d\n", sp.num_ssid);

		blobmsg_for_each_attr(attr, tb[SCAN_EX_SSID], rem) {
			if (blobmsg_type(attr) != BLOBMSG_TYPE_STRING)
				return UBUS_STATUS_INVALID_ARGUMENT;

			strncpy(sp.ssid[i], blobmsg_data(attr),
								sizeof(sp.ssid[i]) - 1);
			i++;
		}
	}

	/* array of opclasses */
	if (tb[SCAN_EX_OPCLASS]) {
		struct blob_attr *attr;
		struct wifi_opclass supp_opclass[64] = {0};
		int num_sup_opclass;
		int num_opclass;
		uint32_t opclass;
		int rem, j;

		num_opclass = blobmsg_check_array(
				tb[SCAN_EX_OPCLASS], BLOBMSG_TYPE_INT32);

		num_sup_opclass = ARRAY_SIZE(supp_opclass);
		wifi_get_supp_opclass(rname, &num_sup_opclass, supp_opclass);

		blobmsg_for_each_attr(attr, tb[SCAN_EX_OPCLASS], rem) {

			if (blobmsg_type(attr) != BLOBMSG_TYPE_INT32)
				return UBUS_STATUS_INVALID_ARGUMENT;

			opclass = blobmsg_get_u32(attr);

			/* opclass is BW20 & supported by the radio in its current band */
			for (j = 0; j < num_sup_opclass; j++) {
				if (supp_opclass[j].opclass == opclass
						&& supp_opclass[j].bw == BW20
						&& supp_opclass[j].band == oper_band) {
					opclasses[op_idx++] = opclass;
					break; /* for */
				}
			}
		}

		if (op_idx != num_opclass) {
			/* opclass from the list unsupported or <> BW20 */
			wifimngr_dbg("scan_ex: opclass unsupported or not for 20MHz\n");
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
	}

	/* array of channels */
	if (tb[SCAN_EX_CHANNEL]) {
		struct blob_attr *attr;
		const char *country = NULL;
		char alpha2[3] = {0};
		uint32_t channel, supp_chan[128] = {0};
		int num_sup_chan;
		int num_channel;
		int rem, k;

		num_channel = blobmsg_check_array(
				tb[SCAN_EX_CHANNEL], BLOBMSG_TYPE_INT32);

		if (!wifi_get_country(rname, alpha2))
			country = alpha2;
		else
			country = uci_get_wifi_option("wifi-device", NULL,
								"country");
		num_sup_chan = ARRAY_SIZE(supp_chan);
		wifi_get_supp_channels(rname, supp_chan, &num_sup_chan,
				country == NULL ? "" : country,
				/* only allow scanning on current operating band */
				oper_band,
				/* only allow 20MHz control channels */
				BW20);

		blobmsg_for_each_attr(attr, tb[SCAN_EX_CHANNEL], rem) {

			if (blobmsg_type(attr) != BLOBMSG_TYPE_INT32)
				return UBUS_STATUS_INVALID_ARGUMENT;

			channel = blobmsg_get_u32(attr);

			/* check channel value supported by the radio */
			for (k = 0; k < num_sup_chan; k++) {
				if (supp_chan[k] == channel) {
					channels[ch_idx++] = channel;
					break; /* for */
				}
			}
		}

		if (ch_idx != num_channel) {
			/* channel from the list unsupported, out of band or not a control 20MHz */
			wifimngr_dbg("scan_ex: unsupported / out of band / not 20MHz control channel\n");
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
	}

	wifimngr_dbg("Num of opclasses=%d, num of channels=%d \n", op_idx, ch_idx);

	/* Translate opclasses & channels to freqs. */

	/* derive list of frequencies from opclass */
	if (op_idx > 0 && ch_idx == 0) {
		int l, m;
		int idx = 0;

		for (l = 0; l < op_idx; l++) {

			ch_idx = ARRAY_SIZE(channels);
			if (wifi_opclass_to_channels(opclasses[l], &ch_idx, channels)) {
				wifimngr_err("Invalid opclass\n");
				return UBUS_STATUS_INVALID_ARGUMENT;
			}

			for (m = 0; m < ch_idx; m++) {
				sp.freq[idx] = wifi_channel_to_freq_ex(channels[m], oper_band);
				if (sp.freq[idx] < 0) {
					wifimngr_err("No freq for channel %d\n", channels[m]);
					continue;
				}
				idx++;
			}
		}
		sp.num_freq = idx;
	}
	/* translate chanels to frequencies directly (use current band) */
	else if (op_idx == 0 && ch_idx > 0) {
		int m;
		int idx = 0;

		for (m = 0; m < ch_idx; m++) {
			sp.freq[idx] = wifi_channel_to_freq_ex(channels[m], oper_band);
			if (sp.freq[idx] < 0) {
				wifimngr_err("No freq for channel %d\n", channels[m]);
				continue;
			}
			idx++;
		}
		sp.num_freq = idx;
	}
	/* check channel & opclass match */
	else {
		uint32_t supp_chan[128] = {0};
		uint32_t *p = &supp_chan[0];
		int sum = 0, idx = 0;
		int num, i, j;

		for (i = 0 ; i < op_idx; i++) {
			num = ARRAY_SIZE(supp_chan) - sum;
			if (wifi_opclass_to_channels(opclasses[i], &num, p)) {
				wifimngr_err("Invalid opclass\n");
				return UBUS_STATUS_INVALID_ARGUMENT;
			}

			sum += num;
			if (sum > ARRAY_SIZE(supp_chan))
				return UBUS_STATUS_INVALID_ARGUMENT;

			p = &supp_chan[sum];
		}

		for (i = 0; i < sum; i++) {
			sp.freq[idx] = wifi_channel_to_freq_ex(supp_chan[i], oper_band);
			if (sp.freq[idx] < 0) {
				wifimngr_err("No freq for channel %d\n", supp_chan[i]);
				continue;
			}
			idx++;
		}

		for (i = 0; i < ch_idx; i++) { /* check each channel */
			for (j = 0; j < sum; j++) {
				if (channels[i] == supp_chan[j])
					break;
			}

			/* We already add this channel */
			if (j != sum)
				continue;

			sp.freq[idx] = wifi_channel_to_freq_ex(channels[i], oper_band);
			if (sp.freq[idx] < 0) {
				wifimngr_err("No freq for channel %d\n", channels[i]);
				continue;
			}
			idx++;
		}
		sp.num_freq = idx;
	}

	ret = wifi_scan_ex(rname, &sp);

	wifimngr_dbg("scan_ex: ret = %d\n", ret);

	if (ret == -EBUSY)
		/* case for an ongoing unfinished scan */
		return UBUS_STATUS_BUSY;
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

enum {
	SCANRES_BSSID,
	__SCANRES_MAX,
};

static const struct blobmsg_policy wl_scanres_policy[__SCANRES_MAX] = {
	[SCANRES_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_STRING },
};

static void wl_scanresult_print(struct blob_buf *bb, void *buf, bool detail)
{
	struct wifi_bss *b = (struct wifi_bss *)buf;
	//char enc_buf[32] = {0};
	//char cpr_buf[32] = {0};
	char std_buf[32] = {0};
	char std_buf2[32] = "802.11";
	char bssid_str[18] = {0};
	char sec_str[64] = {0};
	char cstr[64] = {0};

	blobmsg_add_string(bb, "ssid", (char *)b->ssid);
	hwaddr_ntoa(b->bssid, bssid_str);
	blobmsg_add_string(bb, "bssid", bssid_str);
	blobmsg_add_u32(bb, "channel", b->channel);
	blobmsg_add_u32(bb, "bandwidth", bw_int[b->curr_bw]);
	//wifi_security_str(b->sec, b->enc, b->g_enc, sec_str);
	wifi_security_str(b->security, sec_str);

	etostr(b->rsn.pair_ciphers, cstr, sizeof(cstr), 14, wifi_cipherstr);

	blobmsg_add_string(bb, "encryption", sec_str);
	blobmsg_add_string(bb, "ciphers", cstr);
	blobmsg_add_string(bb, "band",
			!!(b->band & BAND_5) ? "5GHz" :
			!!(b->band & BAND_6) ? "6GHz" :
			!!(b->band & BAND_2) ? "2.4GHz" : "Unknown");
	blobmsg_add_u32(bb, "rssi", b->rssi);
	//blobmsg_add_u32(bb, "snr", b->snr);
	snprintf(std_buf2 + strlen(std_buf2), 31, "%s",
		etostr(b->oper_std, std_buf, sizeof(std_buf), WIFI_NUM_STD, standard_str));
	blobmsg_add_string(bb, "standard", std_buf2);
	blobmsg_add_u32(bb, "load_stas", b->load.sta_count);
	blobmsg_add_u32(bb, "load_utilization", b->load.utilization);
	blobmsg_add_u32(bb, "load_available", b->load.available);

	if (detail) {
		struct wifi_bss_detail *bx =
					(struct wifi_bss_detail *)buf;
		char iebuf[1024] = {0};
		int i;

		for (i = 0; i < bx->ielen; i++) {
			sprintf(iebuf + strlen(iebuf), "%02x", bx->ie[i]);
		}
		blobmsg_add_string(bb, "ielist", iebuf);
	}
}

static int wl_scanresults(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	char *data;
	struct wifi_bss *bsss;
	struct blob_attr *tb[__SCANRES_MAX];
	const char *device;
	void *a;
	void *t;
	int num = 128;  /* initial estimate for max APs in scanresults */
	int i;
	char bssid_str[18] = {0};
	uint8_t bssid[6] = {0};
	bool detail = false;
	struct blob_buf bb;

	memset(&bb, 0, sizeof(bb));
	blobmsg_parse(wl_scanres_policy, __SCANRES_MAX, tb, blob_data(msg),
							blob_len(msg));

	device = ubus_radio_to_ifname(obj);

	if (tb[SCANRES_BSSID]) {
		struct wifi_bss_detail bx;

		strncpy(bssid_str, blobmsg_data(tb[SCANRES_BSSID]),
							sizeof(bssid_str)-1);
		if (hwaddr_aton(bssid_str, bssid) == NULL) {
			wifimngr_err("%s(): Invalid bssid format\n",
								__func__);
			return UBUS_STATUS_INVALID_ARGUMENT;
		}
		detail = true;
		memset(&bx, 0, sizeof(bx));
		if (wifi_get_bss_scan_result(device, bssid, &bx) != 0)
			return UBUS_STATUS_UNKNOWN_ERROR;

		blob_buf_init(&bb, 0);
		//t = blobmsg_open_table(&bb, "");
		wl_scanresult_print(&bb, &bx, detail);
		//blobmsg_close_table(&bb, t);
		ubus_send_reply(ctx, req, bb.head);
		blob_buf_free(&bb);

		return UBUS_STATUS_OK;
	}

	/* dump scanresults list */
	data = calloc(num, sizeof(struct wifi_bss));

	if (!data) {
		wifimngr_err("OOM scan results\n");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}
	bsss = (struct wifi_bss *)data;
	if (wifi_get_scan_results(device, bsss, &num) != 0) {
		free(data);
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	blob_buf_init(&bb, 0);
	a = blobmsg_open_array(&bb, "accesspoints");
	for (i = 0; i < num; i++) {
		t = blobmsg_open_table(&bb, "");
		wl_scanresult_print(&bb, &bsss[i], detail);
		blobmsg_close_table(&bb, t);
	}
	blobmsg_close_array(&bb, a);
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	for (i = 0; i < num; i++) {
		wifimngr_dbg("[%3d]   %18s    %33s     %3d\n",
			i, bsss[i].bssid, bsss[i].ssid, bsss[i].channel);
	}

	free(data);
	return UBUS_STATUS_OK;
}

enum {
	ACS_INTERVAL_SECS,
	ACS_ALGO,
	ACS_NUM_SCANS,
	__ACS_MAX,
};

static const struct blobmsg_policy wl_acs_policy[__ACS_MAX] = {
	[ACS_INTERVAL_SECS] = { .name = "interval", .type = BLOBMSG_TYPE_INT32 },
	[ACS_ALGO] = { .name = "algo", .type = BLOBMSG_TYPE_INT32 },
	[ACS_NUM_SCANS] = { .name = "scans", .type = BLOBMSG_TYPE_INT32 },
};

int wl_autochannel(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	const char *device;
	uint32_t ch;
	struct blob_attr *tb[__ACS_MAX];
	int ret;
	struct blob_buf bb;

	memset(&bb, 0, sizeof(bb));
	blobmsg_parse(wl_acs_policy, __ACS_MAX, tb, blob_data(msg), blob_len(msg));

	device = ubus_radio_to_ifname(obj);
	//TODO: acs params

	ret = wifi_acs(device, NULL);

	blob_buf_init(&bb, 0);
	blobmsg_add_u32(&bb, "code", ret);
	if (ret == 0) {
		enum wifi_bw bw;

		ret = wifi_get_channel(device, &ch, &bw);
		if (ret == 0)
			blobmsg_add_u32(&bb, "new_channel", ch);
		blobmsg_add_string(&bb, "status", "success");
	} else
		blobmsg_add_string(&bb, "status", "error");

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);
	return 	UBUS_STATUS_OK;
}

enum {
	CAC_CHANNEL,
	CAC_BANDWIDTH,
	CAC_METHOD,
	__CAC_MAX,
};

static const struct blobmsg_policy wl_cac_policy[__CAC_MAX] = {
	[CAC_CHANNEL] = { .name = "channel", .type = BLOBMSG_TYPE_INT32 },
	[CAC_BANDWIDTH] = { .name = "bandwidth", .type = BLOBMSG_TYPE_INT32 },
	[CAC_METHOD] = { .name = "method", .type = BLOBMSG_TYPE_INT32 },
};

int wl_start_cac(struct ubus_context *ctx, struct ubus_object *obj,
		 struct ubus_request_data *req, const char *method,
		 struct blob_attr *msg)
{
	enum wifi_cac_method m = WIFI_CAC_MIMO_REDUCED;
	struct blob_attr *tb[__CAC_MAX];
	enum wifi_bw bw = BW20;
	const char *ifname;
	uint32_t ch = 0;
	int ret;


	ifname = ubus_radio_to_ifname(obj);
	blobmsg_parse(wl_cac_policy, __CAC_MAX, tb, blob_data(msg), blob_len(msg));

	if (!tb[CAC_CHANNEL])
		return UBUS_STATUS_INVALID_ARGUMENT;

	ch = blobmsg_get_u32(tb[CAC_CHANNEL]);

	if (tb[CAC_BANDWIDTH]) {
		int bandwidth;

		bandwidth = blobmsg_get_u32(tb[CAC_BANDWIDTH]);
		if (bandwidth == 20)
			bw = BW20;
		else if (bandwidth == 40)
			bw = BW40;
		else if (bandwidth == 80)
			bw = BW80;
		else if (bandwidth == 160)
			bw = BW160;
		else
			return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (tb[CAC_METHOD]) {
		m = blobmsg_get_u32(tb[CAC_METHOD]);
		if (m < 0 || m > WIFI_CAC_TIME_SLICED)
			return UBUS_STATUS_INVALID_ARGUMENT;
	}

	ret = wifi_start_cac(ifname, ch, bw, m);

	return ret == 0 ? UBUS_STATUS_OK : UBUS_STATUS_UNKNOWN_ERROR;
}

int wl_stop_cac(struct ubus_context *ctx, struct ubus_object *obj,
		 struct ubus_request_data *req, const char *method,
		 struct blob_attr *msg)
{
	const char *ifname;
	int ret;


	ifname = ubus_radio_to_ifname(obj);

	ret = wifi_stop_cac(ifname);

	return ret == 0 ? UBUS_STATUS_OK : UBUS_STATUS_UNKNOWN_ERROR;
}

enum {
	RADAR_CHANNEL,
	RADAR_BANDWIDTH,
	RADAR_TYPE,
	RADAR_SUBBAND_MASK,
	__RADAR_MAX,
};

static const struct blobmsg_policy wl_radar_policy[__RADAR_MAX] = {
	[RADAR_CHANNEL] = { .name = "channel", .type = BLOBMSG_TYPE_INT32 },
	[RADAR_BANDWIDTH] = { .name = "bandwidth", .type = BLOBMSG_TYPE_INT32 },
	[RADAR_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
	[RADAR_SUBBAND_MASK] = { .name = "subband_mask", .type = BLOBMSG_TYPE_INT32 },
};

int wl_simulate_radar(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct wifi_radar_args radar = { 0 };
	struct blob_attr *tb[__RADAR_MAX];
	const char *ifname;
	int ret;

	ifname = ubus_radio_to_ifname(obj);
	blobmsg_parse(wl_radar_policy, __RADAR_MAX, tb, blob_data(msg), blob_len(msg));

	if (tb[RADAR_CHANNEL]) {
		radar.channel = blobmsg_get_u32(tb[RADAR_CHANNEL]);
	}

	if (tb[RADAR_BANDWIDTH]) {
		int bandwidth;

		bandwidth = blobmsg_get_u32(tb[RADAR_BANDWIDTH]);
		switch (bandwidth) {
		case 20:
			radar.bandwidth = BW20;
			break;
		case 40:
			radar.bandwidth = BW40;
			break;
		case 80:
			radar.bandwidth = BW80;
			break;
		case 160:
			radar.bandwidth = BW160;
			break;
		default:
			radar.bandwidth = BW_UNKNOWN;
		}
	}

	if (tb[RADAR_TYPE]) {
		radar.type = blobmsg_get_u32(tb[RADAR_TYPE]);
	}

	if (tb[RADAR_SUBBAND_MASK]) {
		/* subband mask can be from range 0 - 0x0FF
		 * here is the rule:
		 * bit:         3       2       1      0
		 * low freq | 20MHz | 20MHz | 20MHz| 20MHz | high freq
		 *          |              80MHz           |
		 */
		radar.subband_mask = blobmsg_get_u32(tb[RADAR_SUBBAND_MASK]);
	}

	ret = wifi_simulate_radar(ifname, &radar);

	return ret == 0 ? UBUS_STATUS_OK : UBUS_STATUS_UNKNOWN_ERROR;
}

static void uci_add_option(struct uci_context *ctx, struct uci_package *p,
				struct uci_section *s, const char *option,
				void *value, bool is_list)
{
	struct uci_ptr ptr = { 0 };

	ptr.p = p;
	ptr.s = s;
	ptr.package = p->e.name;
	ptr.section = s->e.name;
	ptr.option = option;
	ptr.target = UCI_TYPE_OPTION;
	ptr.flags |= UCI_LOOKUP_EXTENDED;
	ptr.value = (char *)value;

	if (is_list)
		uci_add_list(ctx, &ptr);
	else
		uci_set(ctx, &ptr);
}

static int uci_add_wifi_iface(char **argv)
{
	struct uci_context *ctx;
	struct uci_package *pkg;
	struct uci_section *sec;
	struct uci_element *e;
	struct uci_option *op;
	char *ifname = NULL;
	bool found = false;
	int i = 0;

	ctx = uci_alloc_context();
	if (!ctx)
		return -1;

	if (uci_load(ctx, "wireless", &pkg)) {
		uci_free_context(ctx);
		return -1;
	}

	while (argv[i]) {
		if (!strcmp(argv[i], "ifname")) {
			ifname = argv[i + 1];
			break;
		}
		i += 2;
	}

	if (ifname) {
		uci_foreach_element(&pkg->sections, e) {
			struct uci_element *x, *tmp;

			sec = uci_to_section(e);

			if (strcmp(sec->type, "wifi-iface"))
				continue;

			uci_foreach_element_safe(&sec->options, tmp, x) {
				op = uci_to_option(x);
				if (op->type == UCI_TYPE_STRING &&
				    !strncmp(x->name, "ifname", 6) &&
				    !strncmp(op->v.string, ifname, 15)) {

					found = true;
					break;
				}
			}

			if (found) {
				wifimngr_warn("%s %s already added\n", __func__, ifname);
				break;
			}
		}
	}

	if (!found)
		uci_add_section(ctx, pkg, "wifi-iface", &sec);

	i = 0;
	while (argv[i]) {
		uci_add_option(ctx, pkg, sec, argv[i], argv[i + 1], false);
		i += 2;
	}

	uci_commit(ctx, &pkg, false);
	uci_free_context(ctx);

	return 0;
}

static int uci_del_wifi_iface(char *ifname)
{
	struct uci_context *ctx;
	struct uci_package *pkg;
	struct uci_element *e;
	struct uci_option *op;
	bool found = false;

	ctx = uci_alloc_context();
	if (!ctx)
		return -1;

	if (uci_load(ctx, "wireless", &pkg)) {
		uci_free_context(ctx);
		return -1;
	}

	uci_foreach_element(&pkg->sections, e) {
		struct uci_section *s = uci_to_section(e);
		struct uci_element *x, *tmp;

		if (strcmp(s->type, "wifi-iface"))
			continue;

		uci_foreach_element_safe(&s->options, tmp, x) {
			op = uci_to_option(x);
			if (op->type == UCI_TYPE_STRING &&
				!strncmp(x->name, "ifname", 6) &&
				!strncmp(op->v.string, ifname, 15)) {

				found = true;
				break;
			}
		}

		if (found) {
			struct uci_ptr ptr = { 0 };

			ptr.p = pkg;
			ptr.s = s;
			ptr.package = pkg->e.name;
			ptr.section = s->e.name;
			ptr.option = NULL;
			ptr.target = UCI_TYPE_SECTION;
			ptr.flags |= UCI_LOOKUP_EXTENDED;
			ptr.value = "wifi-iface";

			uci_delete(ctx, &ptr);
			uci_commit(ctx, &pkg, false);
			break;
		}
	}

	uci_free_context(ctx);

	return found ? 0 : -1;
}

enum {
	ADD_IFACE_ARGS,
	ADD_IFACE_CONF,
	__ADD_IFACE_MAX,
};

static const struct blobmsg_policy add_iface_policy[__ADD_IFACE_MAX] = {
	[ADD_IFACE_ARGS] = { .name = "args", .type = BLOBMSG_TYPE_TABLE },
	[ADD_IFACE_CONF] = { .name = "config", .type = BLOBMSG_TYPE_BOOL },
};

int wl_add_iface(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
#define ADD_IFACE_ARGS_MAX	17

	enum wifi_mode mode = WIFI_MODE_UNKNOWN;
	struct blob_attr *tb[__ADD_IFACE_MAX];
	char argv[ADD_IFACE_ARGS_MAX * 2 + 1][128] = { 0 };
	char *argvp[ADD_IFACE_ARGS_MAX * 2 + 1] = { 0 };
	bool config = true;
	const char *device;
	int i = 0;
	int ret;
	int rem;

	blobmsg_parse(add_iface_policy, __ADD_IFACE_MAX, tb, blob_data(msg),
						blob_len(msg));

	device = ubus_radio_to_ifname(obj);

	/* add the 'device' option */
	strcpy(argv[i], "device");
	argvp[i] = argv[i];
	strncpy(argv[++i], device, 127);
	argvp[i] = argv[i];
	i++;


	if (tb[ADD_IFACE_CONF])
		config = blobmsg_get_bool(tb[ADD_IFACE_CONF]);


	if (tb[ADD_IFACE_ARGS]) {
		struct blob_attr *attr;

		blobmsg_for_each_attr(attr, tb[ADD_IFACE_ARGS], rem) {
			if (blobmsg_type(attr) != BLOBMSG_TYPE_STRING)
				return UBUS_STATUS_INVALID_ARGUMENT;


			wifimngr_dbg("name = %s\n", blobmsg_name(attr));
			strncpy(argv[i], blobmsg_name(attr), 127);
			argvp[i] = argv[i];
			memcpy(argv[i + 1], blobmsg_data(attr),
						blobmsg_data_len(attr));
			argvp[i + 1] = argv[i + 1];

			if (!strcmp(argvp[i], "mode")) {
				if (!strcmp(argvp[i + 1], "ap"))
					mode = WIFI_MODE_AP;
				else if (!strcmp(argvp[i + 1], "sta"))
					mode = WIFI_MODE_STA;
				else if (!strcmp(argvp[i + 1], "monitor"))
					mode = WIFI_MODE_MONITOR;
				else
					return UBUS_STATUS_INVALID_ARGUMENT;
			}

			i += 2;
			if (i > 31)
				break;
		}
	}

	/* if mode is not specified, assume "ap" */
	if (mode == WIFI_MODE_UNKNOWN) {
		mode = WIFI_MODE_AP;
		strcpy(argv[i], "mode");
		argvp[i] = argv[i];
		strcpy(argv[++i], "ap");
		argvp[i] = argv[i];
	}

	if (config) {
		ret = uci_add_wifi_iface(argvp);
		if (!ret)
			wifimngr_dbg("Successfully added wifi-iface\n");
	}

	ret = wifi_add_iface(device, mode, argvp);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

enum {
	DEL_IFACE_IFNAME,
	DEL_IFACE_CONF,
	__DEL_IFACE_MAX,
};

static const struct blobmsg_policy del_iface_policy[__DEL_IFACE_MAX] = {
	[DEL_IFACE_IFNAME] = { .name = "ifname", .type = BLOBMSG_TYPE_STRING },
	[DEL_IFACE_CONF] = { .name = "config", .type = BLOBMSG_TYPE_BOOL },
};

int wl_del_iface(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	struct blob_attr *tb[__DEL_IFACE_MAX];
	bool config = true;
	const char *device;
	char ifname[16] = {0};
	int ret;

	blobmsg_parse(del_iface_policy, __DEL_IFACE_MAX, tb, blob_data(msg),
						blob_len(msg));

	device = ubus_radio_to_ifname(obj);

	if (!tb[DEL_IFACE_IFNAME])
		return UBUS_STATUS_INVALID_ARGUMENT;

	strncpy(ifname, blobmsg_data(tb[DEL_IFACE_IFNAME]), 15);

	if (tb[DEL_IFACE_CONF])
		config = blobmsg_get_bool(tb[DEL_IFACE_CONF]);

	if (config) {
		ret = uci_del_wifi_iface(ifname);
		if (!ret)
			wifimngr_dbg("Successfully removed wifi-iface\n");
	}

	ret = wifi_del_iface(device, ifname);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static char *dfs_state_str(enum dfs_state state)
{
	switch (state) {
	case WIFI_DFS_STATE_USABLE:
		return "usable";
	case WIFI_DFS_STATE_CAC:
		return "cac";
	case WIFI_DFS_STATE_UNAVAILABLE:
		return "unavailable";
	case WIFI_DFS_STATE_AVAILABLE:
		return "available";
	default:
		break;
	}

	return "unknown";
}

int wl_channels_info(struct ubus_context *ctx, struct ubus_object *obj,
		     struct ubus_request_data *req, const char *method,
		     struct blob_attr *msg)
{
	struct chan_entry channels[32] = {};
	struct chan_entry *entry;
	struct blob_buf bb;
	const char *device;
	void *a;
	void *t;
	int num;
	int ret;
	int i;

	device = ubus_radio_to_ifname(obj);
	num = ARRAY_SIZE(channels);
	ret = wifi_channels_info(device, channels, &num);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);

	a = blobmsg_open_array(&bb, "channels");
	for (i = 0; i < num; i++) {
		entry = &channels[i];
		t = blobmsg_open_table(&bb, "");
		blobmsg_add_u32(&bb, "channel", entry->channel);
		blobmsg_add_u32(&bb, "freq", entry->freq);
		blobmsg_add_u32(&bb, "noise", entry->noise);
		if (entry->dfs) {
			blobmsg_add_u8(&bb, "radar", entry->dfs);
			blobmsg_add_string(&bb, "dfs_state", dfs_state_str(entry->dfs_state));
			blobmsg_add_u32(&bb, "cac_time", entry->cac_time);
			if (entry->dfs_state == WIFI_DFS_STATE_UNAVAILABLE)
				blobmsg_add_u32(&bb, "nop_time", entry->nop_time);
		}

		wifi_print_radio_diagnostics(&bb, &entry->survey);
		blobmsg_close_table(&bb, t);
	}
	blobmsg_close_array(&bb, a);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

int wl_opclass_preferences(struct ubus_context *ctx, struct ubus_object *obj,
			   struct ubus_request_data *req, const char *method,
			   struct blob_attr *msg)
{
	struct wifi_opclass supp_opclass[64] = {0};
	void *a, *t, *aa, *tt, *aaa;
	struct blob_buf bb;
	const char *radio;
	int num_opclass;
	int i, j, k;

	radio = ubus_radio_to_ifname(obj);

	num_opclass = ARRAY_SIZE(supp_opclass);
	if (wifi_get_opclass_preferences(radio, supp_opclass, &num_opclass))
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);

	a = blobmsg_open_array(&bb, "pref_opclass");
	for (i = 0; i < num_opclass; i++) {
		t = blobmsg_open_table(&bb, "");
		blobmsg_add_u32(&bb, "opclass", supp_opclass[i].g_opclass);
		blobmsg_add_u32(&bb, "bandwidth", bw_int[supp_opclass[i].bw]);
		blobmsg_add_u32(&bb, "txpower", supp_opclass[i].opchannel.txpower);
		aa = blobmsg_open_array(&bb, "channels");
		for (j = 0; j < supp_opclass[i].opchannel.num; j++) {
			tt = blobmsg_open_table(&bb, "");
			blobmsg_add_u32(&bb, "channel", supp_opclass[i].opchannel.ch[j].channel);
			blobmsg_add_u32(&bb, "score", supp_opclass[i].opchannel.ch[j].score);
			blobmsg_add_u32(&bb, "dfs", supp_opclass[i].opchannel.ch[j].dfs);
			if (supp_opclass[i].opchannel.ch[j].dfs) {
				blobmsg_add_string(&bb, "dfs_state", dfs_state_str(supp_opclass[i].opchannel.ch[j].dfs_state));
				blobmsg_add_u32(&bb, "cac_time", supp_opclass[i].opchannel.ch[j].cac_time);
				if (supp_opclass[i].opchannel.ch[j].dfs_state == WIFI_DFS_STATE_UNAVAILABLE)
					blobmsg_add_u32(&bb, "nop_time", supp_opclass[i].opchannel.ch[j].nop_time);
			}
			aaa = blobmsg_open_array(&bb, "ctrl_channels");
			for (k = 0; k < ARRAY_SIZE(supp_opclass[i].opchannel.ch[j].ctrl_channels); k++) {
				if (supp_opclass[i].opchannel.ch[j].ctrl_channels[k])
					blobmsg_add_u32(&bb, "", supp_opclass[i].opchannel.ch[j].ctrl_channels[k]);
			}
			blobmsg_close_array(&bb, aaa);
			blobmsg_close_table(&bb, tt);
		}
		blobmsg_close_array(&bb, aa);
		blobmsg_close_table(&bb, t);
	}
	blobmsg_close_array(&bb, a);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

/* list neighbors policy */
enum {
	NBR_LIST_SSID,    /* ssid filter */
	NBR_LIST_CLIENT,  /* from beacon report by client */
	__NBR_LIST_MAX,
};

static const struct blobmsg_policy nbr_list_policy[__NBR_LIST_MAX] = {
	[NBR_LIST_SSID] = { .name = "ssid", .type = BLOBMSG_TYPE_STRING },
	[NBR_LIST_CLIENT] = { .name = "client", .type = BLOBMSG_TYPE_STRING },
};

/* sta disconnect policy */
enum {
	STA_DISCONNECT_STA,   /* sta mac ':' separated */
	STA_DISCONNECT_REASON,
	__STA_DISCONNECT_MAX,
};

static const struct blobmsg_policy sta_disconnect_policy[__STA_DISCONNECT_MAX] = {
	[STA_DISCONNECT_STA] = { .name = "sta", .type = BLOBMSG_TYPE_STRING },
	[STA_DISCONNECT_REASON] = { .name = "reason", .type = BLOBMSG_TYPE_INT32 }
};

/* sta monitor policy */
enum {
	STA_MONITOR_ADDR,   /* sta mac ':' separated */
	__STA_MONITOR_MAX,
};

static const struct blobmsg_policy sta_monitor_policy[__STA_MONITOR_MAX] = {
	[STA_MONITOR_ADDR] = { .name = "sta", .type = BLOBMSG_TYPE_STRING },
};

/* add neighbor policy */
enum {
	NBR_ADD_BSSID,   /* bssid ':' separated */
	NBR_ADD_CHANNEL,
	NBR_ADD_BINFO,   /* bssid info; default = 0x3 (reachable) */
	NBR_ADD_REG,     /* regulatory; default = 0 */
	NBR_ADD_PHY,     /* phytype; default = 9 = PHY_VHT */
	__NBR_ADD_MAX,
};

static const struct blobmsg_policy nbr_add_policy[__NBR_ADD_MAX] = {
	[NBR_ADD_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_STRING },
	[NBR_ADD_CHANNEL] = { .name = "channel", .type = BLOBMSG_TYPE_INT32 },
	[NBR_ADD_BINFO] = { .name = "bssid_info", .type = BLOBMSG_TYPE_STRING },
	[NBR_ADD_REG] = { .name = "reg", .type = BLOBMSG_TYPE_INT32 },
	[NBR_ADD_PHY] = { .name = "phy", .type = BLOBMSG_TYPE_INT32 },
};

/* delete neighbor policy */
enum {
	NBR_DEL_BSSID,
	__NBR_DEL_MAX,
};

static const struct blobmsg_policy nbr_del_policy[__NBR_DEL_MAX] = {
	[NBR_DEL_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_STRING },
};

/* send neighbor request policy */
enum {
	NBR_REQ_CLIENT,
	NBR_REQ_OPCLASS,
	NBR_REQ_CHANNEL,
	NBR_REQ_DURATION,
	NBR_REQ_MODE,
	NBR_REQ_BSSID,
	NBR_REQ_REP_DETAIL,
	NBR_REQ_SSID,
	NBR_REQ_CHAN_REPORT,
	NBR_REQ_ELEMENT_IDS,
	__NBR_REQ_MAX,
};

static const struct blobmsg_policy nbr_req_policy[__NBR_REQ_MAX] = {
	[NBR_REQ_CLIENT] = { .name = "client", .type = BLOBMSG_TYPE_STRING },
	[NBR_REQ_OPCLASS] = { .name = "opclass", .type = BLOBMSG_TYPE_INT32 },
	[NBR_REQ_CHANNEL] = { .name = "channel", .type = BLOBMSG_TYPE_INT32 },
	[NBR_REQ_DURATION] = { .name = "duration", .type = BLOBMSG_TYPE_INT32 },
	[NBR_REQ_MODE] = { .name = "mode", .type = BLOBMSG_TYPE_STRING },
	[NBR_REQ_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_STRING },
	[NBR_REQ_REP_DETAIL] = { .name = "reporting_detail", .type = BLOBMSG_TYPE_INT32  },
	[NBR_REQ_SSID] = { .name = "ssid", .type = BLOBMSG_TYPE_STRING },
	[NBR_REQ_CHAN_REPORT] = { .name = "channel_report", .type = BLOBMSG_TYPE_ARRAY },
	[NBR_REQ_ELEMENT_IDS] = { .name = "request_element", .type = BLOBMSG_TYPE_ARRAY },
};

/* send btm request policy */
enum {
	BTM_REQ_CLIENT,
	BTM_REQ_BSSID,
	BTM_REQ_NEIGHBOR,
	BTM_REQ_MODE,
	BTM_DISASSOC_TMO,
	BTM_VALIDITY_INT,
	BTM_BSSTERM_DUR,
	BTM_MBO_REASON,        /* MBO r19, 4.2.6: Transition Reason Code Attribute (id=0x06) */
	BTM_MBO_CELL_PREF,     /* MBO r19, 4.2.5: Cellular Data Connection Preference (id=0x05) */
	BTM_MBO_REASSOC_DELAY, /* MBO r19, 4.2.8: Assoc retry delay (id=0x08), 0-65535 */
	__BTM_REQ_MAX,
};

static const struct blobmsg_policy btmreq_policy[__BTM_REQ_MAX] = {
	[BTM_REQ_CLIENT] = { .name = "client", .type = BLOBMSG_TYPE_STRING },
	[BTM_REQ_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_ARRAY },
	[BTM_REQ_NEIGHBOR] = { .name = "neighbor", .type = BLOBMSG_TYPE_ARRAY },
	[BTM_REQ_MODE] = { .name = "mode", .type = BLOBMSG_TYPE_INT32 },
	[BTM_DISASSOC_TMO] = { .name = "disassoc_tmo", .type = BLOBMSG_TYPE_INT32 },
	[BTM_VALIDITY_INT] = { .name = "validity_int", .type = BLOBMSG_TYPE_INT32 },
	[BTM_BSSTERM_DUR] = { .name = "bssterm_dur", .type = BLOBMSG_TYPE_INT32 },
	[BTM_MBO_REASON] = { .name = "mbo_reason", .type = BLOBMSG_TYPE_INT32 },
	[BTM_MBO_CELL_PREF] = { .name = "mbo_cell_pref", .type = BLOBMSG_TYPE_INT32 },
	[BTM_MBO_REASSOC_DELAY] = { .name = "mbo_reassoc_delay", .type = BLOBMSG_TYPE_INT32 },
};

/* deprecate */
/* send neighbor bss transition policy */
enum {
	NBR_TRANS_CLIENT,
	NBR_TRANS_BSSID,
	NBR_TRANS_TIMEOUT,
	__NBR_TRANS_MAX,
};

static const struct blobmsg_policy transition_policy[__NBR_TRANS_MAX] = {
	[NBR_TRANS_CLIENT] = { .name = "client", .type = BLOBMSG_TYPE_STRING },
	[NBR_TRANS_BSSID] = { .name = "bssid", .type = BLOBMSG_TYPE_ARRAY },
	[NBR_TRANS_TIMEOUT] = { .name = "timeout", .type = BLOBMSG_TYPE_INT32 },
};

/* send association controll policy */
enum {
	ASSOC_CNTRL_CLIENT,
	ASSOC_CNTRL_ENABLE,
	__ASSOC_CNTRL_MAX,
};

static const struct blobmsg_policy assoc_cntrl_policy[__ASSOC_CNTRL_MAX] = {
	[ASSOC_CNTRL_CLIENT] = { .name = "client", .type = BLOBMSG_TYPE_ARRAY },
	[ASSOC_CNTRL_ENABLE] = { .name = "enable", .type = BLOBMSG_TYPE_INT32  },
};

int sta_disconnect(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__STA_DISCONNECT_MAX];
	uint8_t sta[6] = {0};
	char sta_str[18] = {0};
	const char *ifname;
	uint16_t reason = 0;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(sta_disconnect_policy, __STA_DISCONNECT_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[STA_DISCONNECT_STA])) {
		wifimngr_err("%s(): sta mac not specified!\n", __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	strncpy(sta_str, blobmsg_data(tb[STA_DISCONNECT_STA]), sizeof(sta_str)-1);
	if (hwaddr_aton(sta_str, sta) == NULL) {
		wifimngr_err("%s(): Invalid address format. Use 00:10:22:..\n", __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (tb[STA_DISCONNECT_REASON])
		reason = blobmsg_get_u32(tb[STA_DISCONNECT_REASON]);


	if (wifi_disconnect_sta(ifname, sta, reason) != 0) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	return UBUS_STATUS_OK;
}

int sta_probe(struct ubus_context *ctx, struct ubus_object *obj,
	      struct ubus_request_data *req, const char *method,
	      struct blob_attr *msg)
{
	struct blob_attr *tb[__STA_MONITOR_MAX];
	uint8_t sta[6] = {0};
	char sta_str[18] = {0};
	const char *ifname;


	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(sta_monitor_policy, __STA_MONITOR_MAX, tb, blob_data(msg),
		      blob_len(msg));

	if (!(tb[STA_MONITOR_ADDR])) {
		wifimngr_err("%s(): sta mac not specified!\n", __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	strncpy(sta_str, blobmsg_data(tb[STA_MONITOR_ADDR]), sizeof(sta_str)-1);
	if (hwaddr_aton(sta_str, sta) == NULL) {
		wifimngr_err("%s(): Invalid address format. Use 00:10:22:..\n", __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (wifi_probe_sta(ifname, sta) != 0) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	return UBUS_STATUS_OK;
}

int sta_monitor_add_del(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg, bool enable)
{
	struct blob_attr *tb[__STA_MONITOR_MAX];
	struct wifi_monsta_config cfg = {};
	const char *ifname;
	uint8_t sta[6] = {0};

	cfg.enable = enable;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(sta_monitor_policy, __STA_MONITOR_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[STA_MONITOR_ADDR])) {
		wifimngr_err("%s(): sta mac not specified!\n", __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (hwaddr_aton(blobmsg_data(tb[STA_MONITOR_ADDR]), sta) == NULL) {
		wifimngr_err("%s(): Invalid address\n", __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (wifi_monitor_sta(ifname, sta, &cfg) != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

int sta_monitor_add(struct ubus_context *ctx, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	return sta_monitor_add_del(ctx, obj, req, method, msg, true);
}

int sta_monitor_del(struct ubus_context *ctx, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	return sta_monitor_add_del(ctx, obj, req, method, msg, false);
}

static void
sta_monitor_add_entry(struct blob_buf *bb, struct wifi_monsta *mon)
{
	char sta_str[20] = {};
	void *t;
	void *a;
	int i;

	snprintf(sta_str, 19, "%02x:%02x:%02x:%02x:%02x:%02x", MAC2STR(mon->macaddr));

	t = blobmsg_open_table(bb, "sta");
	blobmsg_add_string(bb, "macaddr", sta_str);
	blobmsg_add_u32(bb, "seen", mon->last_seen);
	blobmsg_add_u32(bb, "rssi_avg", mon->rssi_avg);

	a = blobmsg_open_array(bb, "rssi");
	for (i = 0; i < ARRAY_SIZE(mon->rssi); i++)
		blobmsg_add_u32(bb, "", mon->rssi[i]);
	blobmsg_close_array(bb, a);

	blobmsg_close_table(bb, t);
}

int sta_monitor_get(struct ubus_context *ctx, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	struct blob_attr *tb[__STA_MONITOR_MAX];
	struct blob_buf bb;
	const char *ifname;
	struct wifi_monsta stamon = {};
	struct wifi_monsta stas[128] = {};
	int num = ARRAY_SIZE(stas);
	uint8_t sta[6] = {0};
	void *array;
	int i;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(sta_monitor_policy, __STA_MONITOR_MAX, tb,
					blob_data(msg), blob_len(msg));

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);

	if ((tb[STA_MONITOR_ADDR])) {
		if (hwaddr_aton(blobmsg_data(tb[STA_MONITOR_ADDR]), sta) == NULL) {
			wifimngr_err("%s(): Invalid address\n", __func__);
			return UBUS_STATUS_INVALID_ARGUMENT;
		}

		if (wifi_get_monitor_sta(ifname, sta, &stamon) != 0)
			return UBUS_STATUS_UNKNOWN_ERROR;

		sta_monitor_add_entry(&bb, &stamon);
	} else {
		if (wifi_get_monitor_stas(ifname, stas, &num))
			return UBUS_STATUS_UNKNOWN_ERROR;

		array = blobmsg_open_array(&bb, "stations");
		for (i = 0; i < num; i++)
			sta_monitor_add_entry(&bb, &stas[i]);
		blobmsg_close_array(&bb, array);
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

enum {
	SUBSCRIBE_FRAME_TYPE,
	SUBSCRIBE_FRAME_SUBTYPE,
	__SUBSCRIBE_FRAME_MAX,
};

static const struct blobmsg_policy subscribe_frame_policy[__SUBSCRIBE_FRAME_MAX] = {
	[SUBSCRIBE_FRAME_TYPE] = { .name = "type", .type = BLOBMSG_TYPE_INT32 },
	[SUBSCRIBE_FRAME_SUBTYPE] = { .name = "stype", .type = BLOBMSG_TYPE_INT32 },
};

int subscribe_unsubscribe_frame(struct ubus_context *ctx, struct ubus_object *obj,
				struct ubus_request_data *req, const char *method,
				struct blob_attr *msg, bool subscribe)
{
	struct blob_attr *tb[__SUBSCRIBE_FRAME_MAX];
	const char *ifname;
	unsigned int stype;
	unsigned int type;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(subscribe_frame_policy, __SUBSCRIBE_FRAME_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!tb[SUBSCRIBE_FRAME_TYPE])
		return UBUS_STATUS_INVALID_ARGUMENT;
	if (!tb[SUBSCRIBE_FRAME_SUBTYPE])
		return UBUS_STATUS_INVALID_ARGUMENT;

	type = blobmsg_get_u32(tb[SUBSCRIBE_FRAME_TYPE]);
	stype = blobmsg_get_u32(tb[SUBSCRIBE_FRAME_SUBTYPE]);

	if (subscribe) {
		if (wifi_subscribe_frame(ifname, type, stype))
			return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		if (wifi_unsubscribe_frame(ifname, type, stype))
			return UBUS_STATUS_UNKNOWN_ERROR;
	}

	return UBUS_STATUS_OK;
}

int subscribe_frame(struct ubus_context *ctx, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	return subscribe_unsubscribe_frame(ctx, obj, req, method, msg, true);
}

int unsubscribe_frame(struct ubus_context *ctx, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	return subscribe_unsubscribe_frame(ctx, obj, req, method, msg, false);
}


/* link measure request policy */
enum {
	LINKMEAS_STA,
	__LINKMEAS_MAX,
};

static const struct blobmsg_policy linkmeas_policy[__LINKMEAS_MAX] = {
	[LINKMEAS_STA] = { .name = "sta", .type = BLOBMSG_TYPE_STRING },
};

int ap_measure_link(struct ubus_context *ctx, struct ubus_object *obj,
		    struct ubus_request_data *req, const char *method,
		    struct blob_attr *msg)
{
	struct blob_attr *tb[__LINKMEAS_MAX];
	char macstr[18] = {0};
	uint8_t sta[6] = {0};
	const char *ifname;
	int ret;


	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(linkmeas_policy, __LINKMEAS_MAX, tb, blob_data(msg),
		      blob_len(msg));

	if (!(tb[LINKMEAS_STA]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	strncpy(macstr, blobmsg_data(tb[LINKMEAS_STA]), sizeof(macstr)-1);
	if (hwaddr_aton(macstr, sta) == NULL)
		return UBUS_STATUS_INVALID_ARGUMENT;

	ret = wifi_link_measure(ifname, sta);
	if (ret != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

enum {
	MBO_DISALLOW_ASSOC_REASON,
	__MBO_DISALLOW_ASSOC_MAX,
};

static const struct blobmsg_policy mbo_disallow_assoc_policy[__MBO_DISALLOW_ASSOC_MAX] = {
	[MBO_DISALLOW_ASSOC_REASON] = { .name = "reason", .type = BLOBMSG_TYPE_INT32 },
};

int ap_mbo_disallow_assoc(struct ubus_context *ctx, struct ubus_object *obj,
			  struct ubus_request_data *req, const char *method,
			  struct blob_attr *msg)
{
	struct blob_attr *tb[__MBO_DISALLOW_ASSOC_MAX];
	const char *ifname;
	int ret;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(mbo_disallow_assoc_policy, __MBO_DISALLOW_ASSOC_MAX, tb, blob_data(msg),
		      blob_len(msg));

	if (!(tb[MBO_DISALLOW_ASSOC_REASON]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	ret = wifi_mbo_disallow_assoc(ifname, blobmsg_get_u32(tb[MBO_DISALLOW_ASSOC_REASON]));
	if (ret != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

int ap_bss_up(struct ubus_context *ctx, struct ubus_object *obj,
			  struct ubus_request_data *req, const char *method,
			  struct blob_attr *msg)
{
	const char *ifname;
	int ret;

	ifname = ubus_ap_to_ifname(obj);

	ret = wifi_ap_set_state(ifname, true);
	if (ret != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

int ap_bss_down(struct ubus_context *ctx, struct ubus_object *obj,
			  struct ubus_request_data *req, const char *method,
			  struct blob_attr *msg)
{
	const char *ifname;
	int ret;

	ifname = ubus_ap_to_ifname(obj);

	ret = wifi_ap_set_state(ifname, false);
	if (ret != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

int nbr_add(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__NBR_ADD_MAX];
	const char *ifname;
	char bssid_str[18] = {0};
	uint8_t bssid[6] = {0};
	char binfo_str[12] = {0};
	unsigned int bssid_info = 0;
	unsigned int ch = 0;
	unsigned int phy = 9;
	unsigned int reg = 0;
	struct nbr nbr;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(nbr_add_policy, __NBR_ADD_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[NBR_ADD_BSSID])) {
		wifimngr_err("%s(): Neighbor Bssid not specified!\n", __func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	strncpy(bssid_str, blobmsg_data(tb[NBR_ADD_BSSID]), sizeof(bssid_str)-1);
	if (hwaddr_aton(bssid_str, bssid) == NULL)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[NBR_ADD_BINFO]) {
		strncpy(binfo_str, blobmsg_data(tb[NBR_ADD_BINFO]), sizeof(binfo_str)-1);
		bssid_info = atoi(binfo_str);
	}

	if (tb[NBR_ADD_CHANNEL])
		ch = blobmsg_get_u32(tb[NBR_ADD_CHANNEL]);

	if (tb[NBR_ADD_REG])
		reg = blobmsg_get_u32(tb[NBR_ADD_REG]);

	if (tb[NBR_ADD_PHY])
		phy = blobmsg_get_u32(tb[NBR_ADD_PHY]);

	/* wifimngr_dbg("Add: %02x:%02x:%02x:%02x:%02x:%02x Ch = %d Phy = %d\n",
			bssid[0], bssid[1], bssid[2],
			bssid[3], bssid[4], bssid[5],
			ch, phy); */

	memset(&nbr, 0, sizeof(struct nbr));
	memcpy(nbr.bssid, bssid, 6);
	nbr.bssid_info = bssid_info;
	nbr.channel = ch;
	nbr.phy = phy;
	nbr.reg = reg;

	if (wifi_add_neighbor(ifname, nbr) != 0) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	return UBUS_STATUS_OK;
}

int nbr_del(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__NBR_DEL_MAX];
	const char *ifname;
	char bssid_str[18] = {0};
	uint8_t bssid[6] = {0};

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(nbr_del_policy, __NBR_DEL_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[NBR_DEL_BSSID]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	strncpy(bssid_str, blobmsg_data(tb[NBR_DEL_BSSID]), sizeof(bssid_str)-1);
	if (hwaddr_aton(bssid_str, bssid) == NULL)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (wifi_del_neighbor(ifname, bssid) != 0) {
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	return UBUS_STATUS_OK;
}

static int _nbr_list(struct blob_buf *bb, const char *ifname, bool wildcard)
{
	char bssid_str[18] = {0};
	struct nbr *nbr;
	int nr = 32;
	void *a, *t;
	int ret;
	int i;

	nbr = calloc(nr, sizeof(struct nbr));
	if (!nbr) {
		wifimngr_err("OOM get_neighbor_list()\n");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	ret = wifi_get_neighbor_list(ifname, nbr, &nr);
	if (ret)
		goto out_exit;

	if (!wildcard)
		a = blobmsg_open_array(bb, "neighbors");

	for (i = 0; i < nr; i++) {
		struct nbr *e;

		e = nbr + i;
		t = blobmsg_open_table(bb, "");
		sprintf(bssid_str, "%02X:%02X:%02X:%02X:%02X:%02X",
				e->bssid[0], e->bssid[1], e->bssid[2],
				e->bssid[3], e->bssid[4], e->bssid[5]);
		blobmsg_add_string(bb, "bssid", bssid_str);
		blobmsg_add_u32(bb, "bss_info", e->bssid_info);
		blobmsg_add_u32(bb, "regulatory", e->reg);
		blobmsg_add_u32(bb, "channel", e->channel);
		blobmsg_add_u32(bb, "phy", e->phy);
		blobmsg_close_table(bb, t);
	}

	if (!wildcard)
		blobmsg_close_array(bb, a);

out_exit:
	free(nbr);
	return ret;
}

static int _sta_nbr_list(struct blob_buf *bb, const char *ifname,
						unsigned char *sta)
{
	struct sta_nbr *snbr;
	int nr = 32;
	void *a, *t;
	char bssid_str[18] = {0};
	int ret;
	int i;

	snbr = calloc(nr, sizeof(struct sta_nbr));
	if (!snbr) {
		wifimngr_err("OOM get_neighbor_list()\n");
		return UBUS_STATUS_UNKNOWN_ERROR;
	}

	ret = wifi_get_beacon_report(ifname, sta, snbr, &nr);
	if (ret)
		goto out_exit;

	a = blobmsg_open_array(bb, "neighbors");
	for (i = 0; i < nr; i++) {
		struct sta_nbr *e;

		e = snbr + i;
		t = blobmsg_open_table(bb, "");
		sprintf(bssid_str, "%02X:%02X:%02X:%02X:%02X:%02X",
				e->bssid[0], e->bssid[1], e->bssid[2],
				e->bssid[3], e->bssid[4], e->bssid[5]);
		blobmsg_add_string(bb, "bssid", bssid_str);
		blobmsg_add_u32(bb, "rssi", e->rssi);
		blobmsg_add_u32(bb, "rsni", e->rsni);
		blobmsg_close_table(bb, t);
	}
	blobmsg_close_array(bb, a);

out_exit:
	free(snbr);
	return ret;
}

static int nbr_list(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__NBR_LIST_MAX];
	char sta_macstr[18] = {0};
	uint8_t sta[6] = {0};
	struct blob_buf bb;
	const char *ifname;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(nbr_list_policy, __NBR_LIST_MAX, tb,
					blob_data(msg), blob_len(msg));

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);
	if ((tb[NBR_LIST_CLIENT])) {
		strncpy(sta_macstr, blobmsg_data(tb[NBR_LIST_CLIENT]),
						sizeof(sta_macstr)-1);
		if (hwaddr_aton(sta_macstr, sta) == NULL)
			return UBUS_STATUS_INVALID_ARGUMENT;

		if (_sta_nbr_list(&bb, ifname, sta) != 0)
			return UBUS_STATUS_UNKNOWN_ERROR;
	} else {
		if (_nbr_list(&bb, ifname, false) != 0)
			return UBUS_STATUS_UNKNOWN_ERROR;
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

int nbr_request(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__NBR_REQ_MAX];
	const char *ifname;
	char macstr[18] = {0};
	uint8_t sta_macaddr[6] = {0};
	char ssid[64];
	char mode[32];
	struct wifi_beacon_req *params;
	uint8_t ssid_len = 0;
	uint8_t *bcn_req;
	uint8_t reporting_detail = 0;
	uint8_t num_element = 0;
	int pos = 0;
	size_t req_size = sizeof(struct wifi_beacon_req);
	int ret = UBUS_STATUS_OK;

	blobmsg_parse(nbr_req_policy, __NBR_REQ_MAX, tb,
					blob_data(msg), blob_len(msg));

	/* Table 9-88 - Optional subelement IDs for Beacon request */
	if (tb[NBR_REQ_SSID]) {
		strncpy(ssid, blobmsg_data(tb[NBR_REQ_SSID]), sizeof(ssid)-1);
		ssid_len = strlen(ssid);
		if (ssid_len > 32)
			return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (ssid_len && tb[NBR_REQ_SSID]) {
		req_size += 2; /* tlv */
		req_size += ssid_len;
	}

	if (tb[NBR_REQ_REP_DETAIL]) {
		reporting_detail = (uint8_t)blobmsg_get_u32(
				tb[NBR_REQ_REP_DETAIL]);
		req_size += 3; /* 1 octet value */
	}

	if (tb[NBR_REQ_ELEMENT_IDS]) {
		if (reporting_detail != 1)
			return UBUS_STATUS_INVALID_ARGUMENT;

		num_element = blobmsg_check_array(
				tb[NBR_REQ_ELEMENT_IDS], BLOBMSG_TYPE_INT32);
		/* 9.4.2.10 - Request element */
		req_size += 2 + num_element;
	}

	/* Allocate space for the request */
	bcn_req = calloc(1, req_size);
	if (!bcn_req)
		return -ENOMEM;

	/* Fill in request parameters */
	params = (struct wifi_beacon_req *) bcn_req;

	if (!(tb[NBR_REQ_CLIENT])) {
		ret = UBUS_STATUS_INVALID_ARGUMENT;
		goto out;
	}

	strncpy(macstr, blobmsg_data(tb[NBR_REQ_CLIENT]), sizeof(macstr)-1);
	if (hwaddr_aton(macstr, sta_macaddr) == NULL) {
		ret = UBUS_STATUS_INVALID_ARGUMENT;
		goto out;
	}

	if (tb[NBR_REQ_OPCLASS])
		params->oper_class = blobmsg_get_u32(tb[NBR_REQ_OPCLASS]);

	if (tb[NBR_REQ_CHANNEL])
		params->channel = blobmsg_get_u32(tb[NBR_REQ_CHANNEL]);

	if (tb[NBR_REQ_DURATION])
		params->duration = blobmsg_get_u32(tb[NBR_REQ_DURATION]);

	params->mode = WIFI_BCNREQ_MODE_UNSET;

	if (tb[NBR_REQ_MODE]) {
		strncpy(mode, blobmsg_data(tb[NBR_REQ_MODE]), sizeof(mode)-1);
		if (strlen(mode) > 8) {
			ret = UBUS_STATUS_INVALID_ARGUMENT;
			goto out;
		}

		if (!strcmp(mode, "active"))
			params->mode = WIFI_BCNREQ_MODE_ACTIVE;
		else if (!strcmp(mode, "passive"))
			params->mode = WIFI_BCNREQ_MODE_PASSIVE;
		else if (!strcmp(mode, "table"))
			params->mode = WIFI_BCNREQ_MODE_TABLE;
	}

	if (tb[NBR_REQ_BSSID]) {
		strncpy(macstr, blobmsg_data(tb[NBR_REQ_BSSID]), sizeof(macstr)-1);
		if (hwaddr_aton(macstr, params->bssid) == NULL) {
			ret = UBUS_STATUS_INVALID_ARGUMENT;
			goto out;
		}
	}

	if (ssid_len && tb[NBR_REQ_SSID]) {
		params->variable[pos] = WIFI_BCNREQ_SSID;
		params->variable[pos+1] = ssid_len;
		strncpy((char *) &params->variable[pos+2],
				ssid, ssid_len);
		pos += 2 + ssid_len;
	}

	if (tb[NBR_REQ_REP_DETAIL]) {
		params->variable[pos] = WIFI_BCNREQ_REP_DETAIL;
		params->variable[pos+1] = 1;
		params->variable[pos+2] = reporting_detail;
		pos += 3;
	}

	/* example: ubus call wifi.ap.wlan1 request_neighbor '{"client":"44:d4:37:4d:
	 * 84:83", "bssid":"44:d4:37:42:47:bf", "ssid":"iopsysWrt", "mode":"active",
	 * "channel_report":[{"opclass":81,"channels": [1, 6, 13]}, {"opclass":82,
	 * "channels": [1, 6, 13]}], "reporting_detail":1, "request_element":[7, 33]}'
	 */

	if (tb[NBR_REQ_CHAN_REPORT]) {
		int num_report = blobmsg_check_array(
				tb[NBR_REQ_CHAN_REPORT], BLOBMSG_TYPE_TABLE);
		struct blob_attr *cur;
		int rem;

		wifimngr_dbg("nbr_request: num_report = %d\n", num_report);

		/* 9.4.2.36 AP Channel Report element */
		blobmsg_for_each_attr(cur, tb[NBR_REQ_CHAN_REPORT], rem) {
			struct blob_attr *data[2], *attr;
			int num_channels, j = 0;
			int remm;
			uint8_t *bcn_req_ext;
			static const struct blobmsg_policy supp_attrs[2] = {
				[0] = { .name = "opclass", .type = BLOBMSG_TYPE_INT32 },
				[1] = { .name = "channels", .type = BLOBMSG_TYPE_ARRAY },
			};

			//if (blobmsg_type(cur) != BLOBMSG_TYPE_TABLE)
			//	continue;

			blobmsg_parse(supp_attrs, 2, data, blobmsg_data(cur),
					blobmsg_data_len(cur));

			if (!data[0] || !data[1])
				continue;

			num_channels = blobmsg_check_array(data[1], BLOBMSG_TYPE_INT32);

			/* Allocate additional space for Channel Report elements:
			 * - 3 octets: Element ID + Length + Operating Class.
			 * - Channel List consists of num_channels octets.
			 * - num_channels can be equal to 0.
			 */
			req_size = req_size + 3 + num_channels;
			bcn_req_ext = realloc(bcn_req, req_size);
			if (!bcn_req_ext) {
				ret = -ENOMEM;
				goto out;
			}
			bcn_req = bcn_req_ext;

			// Iterate through all channels of the opclass
			blobmsg_for_each_attr(attr, data[1], remm) {
				if (blobmsg_type(attr) != BLOBMSG_TYPE_INT32)
					continue;
				/* Channel List */
				params->variable[pos + (3 + j)] = (uint8_t) blobmsg_get_u32(attr);
				j++;
			}

			if (num_channels != j) {
				ret = UBUS_STATUS_INVALID_ARGUMENT;
				goto out;
			}

			/* Element ID */
			params->variable[pos] = WIFI_BCNREQ_AP_CHAN_REPORT;
			/* Length = 1 (for opclass) + number_of_channels [octets] */
			params->variable[pos+1] = 1 + j;
			/* Operating Class */
			params->variable[pos+2] = (uint8_t) blobmsg_get_u32(data[0]);

			pos += (3 + j); /* El ID, len, opclass + channel list */
		}
	}

	/* If the Reporting Detail equals 1, a Request element is optionally
	 * present in the optional subelements field. If included, the Request
	 * element lists the Element IDs of the elements requested to be
	 * reported in the Reported Frame Body of the Beacon Report. */
	if (num_element && reporting_detail == 1) {
		/* 9.4.2.10 - Request element */
		struct blob_attr *attr_id;
		int rem_id;
		int k = 0;

		wifimngr_dbg("nbr_request: num_element = %d\n", num_element);

		params->variable[pos] = WIFI_BCNREQ_REQUEST;
		params->variable[pos+1] = num_element;
		blobmsg_for_each_attr(attr_id, tb[NBR_REQ_ELEMENT_IDS], rem_id) {
				/* Table 9-77 - Element IDs */
				if (blobmsg_type(attr_id) != BLOBMSG_TYPE_INT32)
					continue;
				params->variable[pos + (2 + k)] = (uint8_t) blobmsg_get_u32(attr_id);
				k++;
		}

		if (num_element != k) {
			ret = UBUS_STATUS_INVALID_ARGUMENT;
			goto out;
		}

		pos += (2 + k);
	}

	ifname = ubus_ap_to_ifname(obj);
	if (wifi_req_beacon_report(ifname, sta_macaddr, params, req_size) != 0)
		ret = UBUS_STATUS_UNKNOWN_ERROR;

out:
	free(bcn_req);
	return ret;
}

#define MAX_BTM_BSS_NUM 12
int btm_request(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__BTM_REQ_MAX];
	const char *ifname;
	char sta_macstr[18] = {0};
	uint8_t sta[6] = {0};
	uint8_t t_bss[6] = {0};
	char bss_str[18] = {0};
	struct blob_attr *attr;
	int rem;
	int i = 0;
	struct wifi_btmreq param = {};
	struct nbr bsss[MAX_BTM_BSS_NUM] = {0};

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(btmreq_policy, __BTM_REQ_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[BTM_REQ_CLIENT]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[BTM_REQ_BSSID] && tb[BTM_REQ_NEIGHBOR]) {
		wifimngr_err("%s(): Either bssid or nbr allowed\n",
					__func__);
		return UBUS_STATUS_INVALID_ARGUMENT;
	}

	if (tb[BTM_REQ_BSSID]) {
		blobmsg_for_each_attr(attr, tb[BTM_REQ_BSSID], rem) {
			if (blobmsg_type(attr) != BLOBMSG_TYPE_STRING)
				continue;

			strncpy(bss_str, blobmsg_data(attr), sizeof(bss_str)-1);
			if (hwaddr_aton(bss_str, t_bss) == NULL)
				return UBUS_STATUS_INVALID_ARGUMENT;

			memcpy(&bsss[i].bssid, t_bss, 6);

			if (++i >= MAX_BTM_BSS_NUM)
				break;
		}

		param.flags &= ~BTMREQ_F_NBR_PARAM_SET;
	}

	if (tb[BTM_REQ_NEIGHBOR]) {
		int num_neighbor = blobmsg_check_array(
				tb[BTM_REQ_NEIGHBOR], BLOBMSG_TYPE_TABLE);
		struct blob_attr *cur;

		wifimngr_dbg("btm_request: num_neighbor = %d\n", num_neighbor);

		param.flags |= BTMREQ_F_NBR_PARAM_SET;

		blobmsg_for_each_attr(cur, tb[BTM_REQ_NEIGHBOR], rem) {
			struct blob_attr *data[5];
			static const struct blobmsg_policy supp_attrs[5] = {
				[0] = { .name = "bssid", .type = BLOBMSG_TYPE_STRING },
				[1] = { .name = "bssid_info", .type = BLOBMSG_TYPE_INT32 },
				[2] = { .name = "reg", .type = BLOBMSG_TYPE_INT32 },
				[3] = { .name = "channel", .type = BLOBMSG_TYPE_INT32 },
				[4] = { .name = "phy", .type = BLOBMSG_TYPE_INT32 },
			};

			blobmsg_parse(supp_attrs, 5, data, blobmsg_data(cur),
					blobmsg_data_len(cur));

			/* bssid */
			if (!data[0])
				continue;

			if (blobmsg_type(data[0]) != BLOBMSG_TYPE_STRING)
				return UBUS_STATUS_INVALID_ARGUMENT;

			strncpy(bss_str, blobmsg_data(data[0]), sizeof(bss_str)-1);
			if (hwaddr_aton(bss_str, t_bss) == NULL)
				return UBUS_STATUS_INVALID_ARGUMENT;
			memcpy(&bsss[i].bssid, t_bss, 6);

			/* rest of the params */
			if (data[1] && data[2] && data[3] && data[4]) {
				bsss[i].bssid_info = blobmsg_get_u32(data[1]);
				bsss[i].reg = (uint8_t) blobmsg_get_u32(data[2]);
				bsss[i].channel = (uint8_t) blobmsg_get_u32(data[3]);
				bsss[i].phy = (uint8_t) blobmsg_get_u32(data[4]);
			} else {
				/* all nbrs must have all params set or use bssids only */
				param.flags &= ~BTMREQ_F_NBR_PARAM_SET;
			}

			if (++i >= MAX_BTM_BSS_NUM)
				break;
		}
	}

	strncpy(sta_macstr, blobmsg_data(tb[BTM_REQ_CLIENT]), sizeof(sta_macstr)-1);
	if (hwaddr_aton(sta_macstr, sta) == NULL)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[BTM_REQ_MODE])
		param.mode = blobmsg_get_u32(tb[BTM_REQ_MODE]);
	if (tb[BTM_DISASSOC_TMO])
		param.disassoc_tmo = blobmsg_get_u32(tb[BTM_DISASSOC_TMO]);
	if (tb[BTM_VALIDITY_INT])
		param.validity_int = blobmsg_get_u32(tb[BTM_VALIDITY_INT]);
	if (tb[BTM_BSSTERM_DUR])
		param.validity_int = blobmsg_get_u32(tb[BTM_BSSTERM_DUR]);

	/* MBO parameters */
	if (tb[BTM_MBO_REASON])
		param.mbo.reason = blobmsg_get_u32(tb[BTM_MBO_REASON]);
	if (tb[BTM_MBO_CELL_PREF])
		param.mbo.cell_pref = blobmsg_get_u32(tb[BTM_MBO_CELL_PREF]);
	if (tb[BTM_MBO_REASSOC_DELAY])
		param.mbo.reassoc_delay = blobmsg_get_u32(tb[BTM_MBO_REASSOC_DELAY]);

	param.mbo.valid = false;
	if (param.mbo.reason <= 255 || param.mbo.cell_pref <= 255
			|| param.mbo.reassoc_delay <= 65535)
		param.mbo.valid = true;

	if (wifi_req_btm(ifname, sta, i, bsss, &param) != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

/* deprecate */
int nbr_transition(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__NBR_TRANS_MAX];
	const char *ifname;
	char sta_macstr[18] = {0};
	uint8_t sta[6] = {0};
	uint8_t t_bss[6] = {0};
	char bss_str[18] = {0};
	unsigned int tmo = 0;
	struct blob_attr *attr;
	int rem;
	int i = 0;
	unsigned char bss[72] = {0};

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(transition_policy, __NBR_TRANS_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[NBR_TRANS_CLIENT]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (tb[NBR_TRANS_BSSID]) {
		blobmsg_for_each_attr(attr, tb[NBR_TRANS_BSSID], rem) {
			if (blobmsg_type(attr) != BLOBMSG_TYPE_STRING)
				continue;

			strncpy(bss_str, blobmsg_data(attr), sizeof(bss_str)-1);
			if (hwaddr_aton(bss_str, t_bss) == NULL)
				return UBUS_STATUS_INVALID_ARGUMENT;

			memcpy(&bss[i*6], t_bss, 6);

			// blobmsg_name(attr),
			// blobmsg_data(attr),
			// blobmsg_data_len(attr));
			if (++i > 9)
				break;
		}
	}

	if (tb[NBR_TRANS_TIMEOUT])
		tmo = blobmsg_get_u32(tb[NBR_TRANS_TIMEOUT]);

	strncpy(sta_macstr, blobmsg_data(tb[NBR_TRANS_CLIENT]), sizeof(sta_macstr)-1);
	if (hwaddr_aton(sta_macstr, sta) == NULL)
		return UBUS_STATUS_INVALID_ARGUMENT;

	if (wifi_req_bss_transition(ifname, sta, i, bss, tmo) != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

int assoc_control(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *req, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__ASSOC_CNTRL_MAX];
	const char *ifname;
	char sta_macstr[18] = {0};
	uint8_t sta[6] = {0};
	struct blob_attr *attr;
	int enable, rem;

	wifimngr_dbg("%s: entering\n", __func__);

	ifname = ubus_ap_to_ifname(obj);

	blobmsg_parse(assoc_cntrl_policy, __ASSOC_CNTRL_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[ASSOC_CNTRL_CLIENT]) || !(tb[ASSOC_CNTRL_ENABLE]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	enable = blobmsg_get_u32(tb[ASSOC_CNTRL_ENABLE]);

	blobmsg_for_each_attr(attr, tb[ASSOC_CNTRL_CLIENT], rem) {
		if (blobmsg_type(attr) != BLOBMSG_TYPE_STRING)
				continue;

		strncpy(sta_macstr, blobmsg_data(attr), sizeof(sta_macstr)-1);
		if (hwaddr_aton(sta_macstr, sta) == NULL)
			return UBUS_STATUS_INVALID_ARGUMENT;

		if (wifi_restrict_sta(ifname, sta, enable) != 0)
			return UBUS_STATUS_UNKNOWN_ERROR;
	}

	return UBUS_STATUS_OK;
}

/* vendor_ie add/del policy */
enum {
	VSIE_MGMT_STYPE,       /* bitmask of (1 << mgmt frame subtype) */
	VSIE_OUI,
	VSIE_DATA,
	__VSIE_MAX,
};

static const struct blobmsg_policy vsie_policy[__VSIE_MAX] = {
	[VSIE_MGMT_STYPE] = { .name = "mgmt", .type = BLOBMSG_TYPE_INT32 },
	[VSIE_OUI] = { .name = "oui", .type = BLOBMSG_TYPE_STRING},
	[VSIE_DATA] = { .name = "data", .type = BLOBMSG_TYPE_STRING},
};

int vsie_add(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *ureq, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__VSIE_MAX];
	const char *ifname;
	uint8_t vsie_buf[256] = {0};
	struct vendor_iereq *req = (struct vendor_iereq *)vsie_buf;
	uint8_t *data = req->ie.data;
	char data_str[499] = {0};	/* (256 - 7) * 2 + 1 */
	char oui_str[7] = {0};          /* 3 * 2 + 1 */
	uint8_t oui[3] = {0};
	int data_len = 0;
	int data_strlen;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(vsie_policy, __VSIE_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[VSIE_OUI]) || !(tb[VSIE_DATA]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	req->mgmt_subtype = 0;
	req->ie.ie_hdr.eid = 0xdd;
	req->ie.ie_hdr.len = 0;

	if (tb[VSIE_MGMT_STYPE])
		req->mgmt_subtype = blobmsg_get_u32(tb[VSIE_MGMT_STYPE]);

	if (blobmsg_data_len(tb[VSIE_OUI]) != sizeof(oui_str))
		return UBUS_STATUS_INVALID_ARGUMENT;

	strncpy(oui_str, blobmsg_data(tb[VSIE_OUI]), 6);
	strtob(oui_str, 7, oui);
	memcpy(req->ie.oui, oui, 3);
	req->ie.ie_hdr.len += 3;

	data_strlen = blobmsg_data_len(tb[VSIE_DATA]);
	if (data_strlen > sizeof(data_str) - 1)
		return UBUS_STATUS_INVALID_ARGUMENT;

	strncpy(data_str, blobmsg_data(tb[VSIE_DATA]), data_strlen);
	strtob(data_str, data_strlen, &data[0]);

	data_len = (data_strlen - 1) / 2;
	req->ie.ie_hdr.len += data_len;

	if (wifi_add_vendor_ie(ifname, req) != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

int vsie_del(struct ubus_context *ctx, struct ubus_object *obj,
		      struct ubus_request_data *ureq, const char *method,
		      struct blob_attr *msg)
{
	struct blob_attr *tb[__VSIE_MAX];
	const char *ifname;
	uint8_t vsie_buf[256] = {0};
	struct vendor_iereq *req = (struct vendor_iereq *)vsie_buf;
	uint8_t *data = req->ie.data;
	char data_str[499] = {0};	/* (256 - 7) * 2 + 1 */
	char oui_str[7] = {0};          /* 3 * 2 + 1 */
	uint8_t oui[3] = {0};
	int data_strlen;
	int data_len = 0;
	//uint32_t mgmt_frames = 0;

	ifname = ubus_ap_to_ifname(obj);
	blobmsg_parse(vsie_policy, __VSIE_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (!(tb[VSIE_OUI]))
		return UBUS_STATUS_INVALID_ARGUMENT;

	req->mgmt_subtype = 0;
	req->ie.ie_hdr.eid = 0xdd;
	req->ie.ie_hdr.len = 0;

	if (tb[VSIE_MGMT_STYPE])
		req->mgmt_subtype = blobmsg_get_u32(tb[VSIE_MGMT_STYPE]);


	if (blobmsg_data_len(tb[VSIE_OUI]) != sizeof(oui_str))
		return UBUS_STATUS_INVALID_ARGUMENT;

	strncpy(oui_str, blobmsg_data(tb[VSIE_OUI]), 6);
	strtob(oui_str, 7, oui);
	memcpy(req->ie.oui, oui, 3);
	req->ie.ie_hdr.len += 3;

	if (tb[VSIE_DATA]) {
		/* match data starting with pattern */
		data_strlen = blobmsg_data_len(tb[VSIE_DATA]);
		if (data_strlen > sizeof(data_str) - 1)
			return UBUS_STATUS_INVALID_ARGUMENT;

		strncpy(data_str, blobmsg_data(tb[VSIE_DATA]), data_strlen);
		strtob(data_str, data_strlen, &data[0]);

		data_len = (data_strlen - 1) / 2;
		req->ie.ie_hdr.len += data_len;
	}

	if (wifi_del_vendor_ie(ifname, req) != 0)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

static int wl_sta_status(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	uint8_t bssid[6] = {0};
	char ssid[64] = {0};
	const char *ifname;
	ifstatus_t ifs = 0;
	struct wifi_sta sta;
	struct blob_buf bb;
	int j, ret;
	void *t;

	char std_buf2[32] = "802.11";
	char std_buf[32] = {0};
	char par_macstr[18] = {0};
	char sta_macstr[18] = {0};
	char sec_str[128] = {0};
	char enc_buf[32] = {0};

	uint32_t channel = 0;
	enum wifi_bw bw;
	int noise, rssi, snr;
	enum wifi_band band = BAND_2;
	bool addr4mode = false;

	ifname = ubus_sta_to_ifname(obj);
	wifi_radio_get_ifstatus(ifname, &ifs);

	memset(&sta, 0, sizeof(struct wifi_sta));
	ret = wifi_sta_info(ifname, &sta);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	wifi_get_ssid(ifname, ssid);
	wifi_get_4addr(ifname, &addr4mode);

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);
	blobmsg_add_string(&bb, "ifname", ifname);
	blobmsg_add_string(&bb, "status", ifstatus_str(ifs));
	blobmsg_add_u8(&bb, "4addr", addr4mode);
	hwaddr_ntoa(sta.macaddr, sta_macstr);
	blobmsg_add_string(&bb, "macaddr", sta_macstr);
	snprintf(std_buf2 + strlen(std_buf2), 31, "%s",
		etostr(sta.oper_std, std_buf, sizeof(std_buf), WIFI_NUM_STD, standard_str));
	blobmsg_add_string(&bb, "standard", std_buf2);
	wifi_security_str(sta.sec.curr_mode, sec_str);
	blobmsg_add_string(&bb, "security", sec_str);
	blobmsg_add_string(&bb, "encryption",
			etostr(sta.sec.pair_ciphers, enc_buf, sizeof(enc_buf), 14, cipher_str));	//FIXME-CR
	wl_dump_supp_security(&bb, sta.sec.supp_modes);

	if (memcmp(bssid, sta.bssid, sizeof(bssid))) {
		wifi_get_channel(ifname, &channel, &bw);
		wifi_get_oper_band(ifname, &band);
		wifi_get_noise(ifname, &noise);
		wifi_get_bandwidth(ifname, &bw);
		rssi = sta.rssi[0];
		snr = rssi - noise;
		hwaddr_ntoa(sta.bssid, par_macstr);

		blobmsg_add_string(&bb, "bssid", par_macstr);
		blobmsg_add_string(&bb, "ssid", ssid);
		blobmsg_add_u32(&bb, "channel", channel);
		blobmsg_add_u32(&bb, "bandwidth", bw_int[bw]);
		blobmsg_add_u32(&bb, "frequency", wifi_channel_to_freq_ex(channel, band));
		blobmsg_add_u32(&bb, "rssi", rssi);
		blobmsg_add_u32(&bb, "noise", sta.noise[0] == 0 ? noise : sta.noise[0]);
		blobmsg_add_u32(&bb, "snr", snr);

		blobmsg_add_u32(&bb, "idle", sta.idle_time);
		blobmsg_add_u64(&bb, "in_network", sta.conn_time);
		blobmsg_add_u32(&bb, "tx_airtime", sta.tx_airtime);
		blobmsg_add_u32(&bb, "rx_airtime", sta.rx_airtime);
		blobmsg_add_u32(&bb, "airtime", sta.airtime);
		blobmsg_add_u32(&bb, "maxrate", sta.maxrate);
		blobmsg_add_u32(&bb, "tx_throughput", sta.tx_thput);
		blobmsg_add_u32(&bb, "est_rx_thput", sta.est_rx_thput);
		blobmsg_add_u32(&bb, "est_tx_thput", sta.est_tx_thput);

		t = blobmsg_open_table(&bb, "status");
		blobmsg_add_u8(&bb, "wmm",
			wifi_status_isset(sta.sbitmap, WIFI_STATUS_WMM) ? true : false);
		blobmsg_add_u8(&bb, "ps",
			wifi_status_isset(sta.sbitmap, WIFI_STATUS_PS) ? true : false);
		blobmsg_close_table(&bb, t);

		wl_dump_capabilities(band, &bb, &sta.caps, sta.cbitmap, sizeof(sta.cbitmap));
		wl_print_sta_stats(&bb, &sta);

		t = blobmsg_open_array(&bb, "rssi_per_antenna");
		for (j = 0; j < WIFI_NUM_ANTENNA; j++)
			blobmsg_add_u32(&bb, "", sta.rssi[j]);
		blobmsg_close_array(&bb, t);
	}
	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

static int wl_sta_stats(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	const char *ifname;
	struct wifi_sta_stats s;
	struct blob_buf bb;
	int ret;

	ifname = ubus_sta_to_ifname(obj);

	ret = wifi_sta_get_stats(ifname, &s);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);

	blobmsg_add_string(&bb, "ifname", ifname);
	void *t;
	t = blobmsg_open_table(&bb, "stats");
	blobmsg_add_u64(&bb, "tx_total_pkts", s.tx_pkts);
	blobmsg_add_u64(&bb, "tx_total_bytes", s.tx_bytes);
	blobmsg_add_u64(&bb, "tx_failures", s.tx_fail_pkts);
	blobmsg_add_u64(&bb, "tx_pkts_retries", s.tx_retry_pkts);
	blobmsg_add_u64(&bb, "rx_data_pkts", s.rx_pkts);
	blobmsg_add_u64(&bb, "rx_data_bytes", s.rx_bytes);
	blobmsg_add_u64(&bb, "rx_failures", s.rx_fail_pkts);
	blobmsg_close_table(&bb, t);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

/* disconnect from ap policy */
enum {
	AP_DISCONNECT_REASON,   /* deauth/disassoc reason code */
	__AP_DISCONNECT_MAX,
};

static const struct blobmsg_policy ap_disconnect_policy[__AP_DISCONNECT_MAX] = {
	[AP_DISCONNECT_REASON] = { .name = "reason", .type = BLOBMSG_TYPE_INT32}
};

static int wl_sta_disconnect_ap(struct ubus_context *ctx,
				struct ubus_object *obj,
				struct ubus_request_data *req,
				const char *method,
				struct blob_attr *msg)
{
	struct blob_attr *tb[__AP_DISCONNECT_MAX];
	unsigned int reason_code = 0;
	const char *ifname;
	int ret;

	ifname = ubus_sta_to_ifname(obj);
	blobmsg_parse(ap_disconnect_policy, __AP_DISCONNECT_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (tb[AP_DISCONNECT_REASON])
		reason_code = blobmsg_get_u32(tb[AP_DISCONNECT_REASON]);

	ret = wifi_sta_disconnect_ap(ifname, reason_code);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	return UBUS_STATUS_OK;
}

enum {
	STA_4ADDR_ENABLE,
	__STA_4ADDR_MAX,
};

static const struct blobmsg_policy sta_4addr_policy[__STA_4ADDR_MAX] = {
	[STA_4ADDR_ENABLE] = { .name = "enable", .type = BLOBMSG_TYPE_INT32}
};

static int wl_sta_4addr(struct ubus_context *ctx,
			struct ubus_object *obj,
			struct ubus_request_data *req,
			const char *method,
			struct blob_attr *msg)
{
	struct blob_attr *tb[__STA_4ADDR_MAX];
	struct blob_buf bb;
	bool enable;
	const char *ifname;
	int ret;

	ifname = ubus_sta_to_ifname(obj);
	blobmsg_parse(sta_4addr_policy, __STA_4ADDR_MAX, tb,
					blob_data(msg), blob_len(msg));

	if (tb[STA_4ADDR_ENABLE]) {
		enable = blobmsg_get_u32(tb[STA_4ADDR_ENABLE]);
		ret = wifi_set_4addr(ifname, enable);
		if (ret)
			return UBUS_STATUS_UNKNOWN_ERROR;

		return UBUS_STATUS_OK;
	}

	ret = wifi_get_4addr(ifname, &enable);
	if (ret)
		return UBUS_STATUS_UNKNOWN_ERROR;

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);

	blobmsg_add_u8(&bb, "enable", enable);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return UBUS_STATUS_OK;
}

static int wl_status(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	char radios[WIFI_DEV_MAX_NUM][16] = {0};
	struct wifimngr_iface ifs[WIFI_IF_MAX_NUM];
	struct blob_buf bb;
	int num_radios = 0;
	int num_ifs = 0;
	void *t, *tt, *ttt, *tttt, *t5;
	int i;

	num_radios = wifimngr_get_wifi_devices(radios);
	if (num_radios < 0)
		return 0;

	num_ifs = wifimngr_get_wifi_interfaces(ifs);
	if (num_ifs < 0)
		return 0;

	/* wifimngr_dbg("radios = %d    ifs = %d\n", num_radios, num_ifs); */

	memset(&bb, 0, sizeof(bb));
	blob_buf_init(&bb, 0);

	t = blobmsg_open_array(&bb, "radios");
	for (i = 0; i < num_radios; i++) {
		uint32_t channel;
		uint32_t channels[64] = {0};
		char *phyname;
		ifstatus_t ifstat = 0;
		enum wifi_band band = BAND_2;
		enum wifi_band chband = BAND_2;
		uint32_t supp_band = 0;
		uint8_t supp_std = 0;
		unsigned long maxrate;
		int noise;
		enum wifi_bw bw;
		//int bwi = 20;
		uint8_t s = 0;
		//char s_str[32] = {0};
		char std_buf[32] = {0};
		char std_buf2[32] = "802.11";
		struct wifi_caps radiocaps;
		char alpha2[3] = {0};
		const char *country;
		int nr = 0;
		int j;

		phyname = (char *) wifi_radio_phyname(radios[i]);
		if (!phyname)
			continue;

		wifi_radio_get_ifstatus(phyname, &ifstat);
		wifi_get_channel(phyname, &channel, &bw);
		wifi_get_oper_band(phyname, &band);
		wifi_get_bandwidth(phyname, &bw);

		if (!wifi_get_country(phyname, alpha2))
			country = alpha2;
		else
			country = uci_get_wifi_option("wifi-device", phyname,
								"country");

		wifi_radio_get_caps(phyname, &radiocaps);
		wifi_get_supp_stds(phyname, &supp_std);

		wifi_get_supp_band(phyname, &supp_band);
		if ((supp_band & BAND_2) && (supp_band & BAND_5))
			chband = BAND_DUAL;
		else
			chband = band;

		/* TODO pass band mask instead of single band */
		nr = ARRAY_SIZE(channels);
		wifi_get_supp_channels(phyname, channels, &nr,
					country == NULL ? "" : country,
					chband, bw);

		wifi_get_maxrate(phyname, &maxrate);
		wifi_get_noise(phyname, &noise);
		wifi_get_oper_stds(phyname, &s);

		ttt = blobmsg_open_table(&bb, "");

		blobmsg_add_string(&bb, "name", radios[i]);
		blobmsg_add_string(&bb, "phyname", phyname);
		blobmsg_add_u8(&bb, "isup", (ifstat & IFF_UP) ? true : false);
		//blobmsg_add_string(&bb, "standard", s_str);

		snprintf(std_buf2 + strlen(std_buf2), 31, "%s",
			etostr(s, std_buf, sizeof(std_buf), WIFI_NUM_STD, standard_str));
		blobmsg_add_string(&bb, "standard", std_buf2);
		blobmsg_add_string(&bb, "country", country);
		blobmsg_add_string(&bb, "band",
			!!(band & BAND_5) ? "5GHz" :
			!!(band & BAND_6) ? "6GHz" :
			!!(band & BAND_2) ? "2.4GHz" : "Unknown");

		blobmsg_add_u32(&bb, "channel", channel);
		blobmsg_add_u32(&bb, "frequency", wifi_channel_to_freq_ex(channel, band));
		blobmsg_add_u32(&bb, "bandwidth", bw_int[bw]);
		blobmsg_add_u32(&bb, "noise", noise);
		blobmsg_add_u64(&bb, "maxrate", maxrate);
		wifi_print_radio_supp_bands(&bb, supp_band);
		wifi_print_radio_supp_std(&bb, supp_std);

		t5 = blobmsg_open_array(&bb, "channels");
		for (j = 0; j < nr && channels[j] != 0; j++) {
			blobmsg_add_u32(&bb, "", channels[j]);
		}
		blobmsg_close_array(&bb, t5);


		tt = blobmsg_open_array(&bb, "accesspoints");
		for (j = 0; j < num_ifs; j++) {
			char *ifname = ifs[j].iface;
			uint8_t bssid[6] = {0};
			char bssidstr[18] = {0};
			char ssid[64] = {0};
			//ifopstatus_t opstatus;
			uint32_t auth;
			uint32_t enc;
			struct wifi_caps apcaps;


			if (ifs[j].device[0] &&
				!strncmp(ifs[j].device, radios[i], 16) &&
				ifs[j].mode == WIFI_AP) {
				/* wifimngr_dbg("Matched: rad = %s  if = %s\n",
						ifs[j].device, ifname); */
				//memset(ifs[j].device, 0, 16);
				;
			} else {
				continue;
			}


			wifi_radio_get_ifstatus(ifname, &ifstat);
			//wifi_get_ifoperstatus(ifname, &opstatus);
			wifi_get_bssid(ifname, bssid);
			hwaddr_ntoa(bssid, bssidstr);
			wifi_get_ssid(ifname, ssid);
			wifi_get_security(ifname, &auth, &enc);
			wifi_get_caps(ifname, &apcaps);

			tttt = blobmsg_open_table(&bb, "");
			blobmsg_add_string(&bb, "ifname", ifname);
			blobmsg_add_string(&bb, "status", ifstatus_str(ifstat));
			//blobmsg_add_string(&bb, "operstate", operstate_str[opstatus]);
			blobmsg_add_string(&bb, "ssid", ssid);
			blobmsg_add_string(&bb, "bssid", bssidstr);

			blobmsg_close_table(&bb, tttt);
		}
		blobmsg_close_array(&bb, tt);

		tt = blobmsg_open_array(&bb, "backhauls");
		for (j = 0; j < num_ifs; j++) {
			char *ifname = ifs[j].iface;
			//ifopstatus_t opstatus;

			if (ifs[j].device[0] &&
				!strncmp(ifs[j].device, radios[i], 16) &&
				ifs[j].mode == WIFI_STA) {
				;
			} else {
				continue;
			}

			wifi_radio_get_ifstatus(ifname, &ifstat);
			//wifi_get_ifoperstatus(ifname, &opstatus);

			tttt = blobmsg_open_table(&bb, "");
			blobmsg_add_string(&bb, "ifname", ifname);
			blobmsg_add_string(&bb, "status", ifstatus_str(ifstat));
			//blobmsg_add_string(&bb, "operstate", operstate_str[opstatus]);
			blobmsg_close_table(&bb, tttt);
		}
		blobmsg_close_array(&bb, tt);

		blobmsg_close_table(&bb, ttt);
	}
	blobmsg_close_array(&bb, t);

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);

	return 0;
}

enum {
	DEBUG_LEVEL,
	__DEBUG_LEVEL_MAX,
};

static const struct blobmsg_policy wifi_set_debug_policy[__DEBUG_LEVEL_MAX] = {
	[DEBUG_LEVEL] = { .name = "level", .type = BLOBMSG_TYPE_INT32 },
};

static int wifi_set_debug(struct ubus_context *ctx, struct ubus_object *obj,
			struct ubus_request_data *req, const char *method,
			struct blob_attr *msg)
{
	struct blob_buf bb;
	struct blob_attr *tb[__DEBUG_LEVEL_MAX];
	char *envval = getenv("LIBWIFI_DEBUG_LEVEL");
	char new_envval[8] = { 0 };
	uint16_t debug_level;

	debug_level = envval ? atoi(envval) : 0;
	memset(&bb, 0, sizeof(bb));

	blobmsg_parse(wifi_set_debug_policy, __DEBUG_LEVEL_MAX, tb,
			blob_data(msg), blob_len(msg));

	if (!(tb[DEBUG_LEVEL])) {
		blob_buf_init(&bb, 0);
		blobmsg_add_u32(&bb, "LIBWIFI_DEBUG_LEVEL: ", debug_level);
		ubus_send_reply(ctx, req, bb.head);
		blob_buf_free(&bb);
	} else {
		debug_level = blobmsg_get_u32(tb[DEBUG_LEVEL]);
		snprintf(new_envval, 7, "%d", debug_level);
		setenv("LIBWIFI_DEBUG_LEVEL", new_envval, 1);
	}

	return 0;
}

enum {
	GET_PNAME,
	__GET_MAX,
};

static const struct blobmsg_policy radio_get_param_policy[__GET_MAX] = {
	[GET_PNAME] = { .name = "param", .type = BLOBMSG_TYPE_STRING },
};

int wl_radio_get_param(struct ubus_context *ctx, struct ubus_object *obj,
		  struct ubus_request_data *req, const char *method,
		  struct blob_attr *msg)
{
	struct blob_attr *tb[__GET_MAX];
	const char *device;
	struct blob_buf bb;
	int data = 0;	// FIXME: data type based on 'param' name
	int len = 0;	//        also len
	int ret;

	memset(&bb, 0, sizeof(bb));
	blobmsg_parse(radio_get_param_policy, __GET_MAX, tb,
						blob_data(msg), blob_len(msg));

	device = ubus_radio_to_ifname(obj);

	ret = wifi_radio_get_param(device, "temperature", &len, &data);
	blob_buf_init(&bb, 0);
	blobmsg_add_u32(&bb, "status", ret);
	if (ret == 0) {
		blobmsg_add_u32(&bb, "value_int", data);
	}

	ubus_send_reply(ctx, req, bb.head);
	blob_buf_free(&bb);
	return 	UBUS_STATUS_OK;
}

#define MAX_WIFI_METHODS	8
int add_wifi_methods(struct ubus_object *wifi_obj)
{
	int n_methods = 0;
	struct ubus_method *wifi_methods;

	wifi_methods = calloc(MAX_WIFI_METHODS, sizeof(struct ubus_method));
	if (!wifi_methods)
		return -ENOMEM;

	UBUS_METHOD_ADD(wifi_methods, n_methods,
		UBUS_METHOD_NOARG("status", wl_status));

	UBUS_METHOD_ADD(wifi_methods, n_methods,
		UBUS_METHOD("debug", wifi_set_debug, wifi_set_debug_policy));

	wifi_obj->methods = wifi_methods;
	wifi_obj->n_methods = n_methods;

	return 0;
}

int wifimngr_add_object(struct wifimngr *w,
				struct ubus_context *ctx,
				const char *objname,
				int (*add_methods)(struct ubus_object *o))
{
	struct ubus_object *wobj;
	struct ubus_object_type *wobj_type;
	int ret;

	wobj = calloc(1, sizeof(struct ubus_object));
	if (!wobj)
		return -ENOMEM;

	wobj_type = calloc(1, sizeof(struct ubus_object_type));
	if (!wobj_type) {
		free(wobj);
		return -ENOMEM;
	}

	wobj->name = objname;
	if (add_methods)
		add_methods(wobj);

	wobj_type->name = wobj->name;
	wobj_type->n_methods = wobj->n_methods;
	wobj_type->methods = wobj->methods;
	wobj->type = wobj_type;

	ret = ubus_add_object(ctx, wobj);
	if (!ret) {
		/* wifimngr_dbg("Added '%s' radio obj\n", wobj->obj.name); */
		w->wifi = wobj;
	}

	return ret;
}

#define MAX_RADIO_METHODS	16

static int add_radio_methods(struct ubus_object *radio_obj,
				const char *radioname)
{
	struct ubus_method *radio_methods;
	int n_methods = 0;
	enum wifi_band band;
	char *phyname;

	radio_methods = calloc(MAX_RADIO_METHODS, sizeof(struct ubus_method));
	if (!radio_methods)
		return -ENOMEM;

	phyname = (char *) wifi_radio_phyname(radioname);
	if (!phyname) {
		free(radio_methods);
		return -1;
	}

	wifi_get_oper_band(phyname, &band);

	UBUS_METHOD_ADD(radio_methods, n_methods,
		UBUS_METHOD_NOARG("status", wl_radio_status));

	UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD_NOARG("stats", wl_radio_stats));

	UBUS_METHOD_ADD(radio_methods, n_methods,
		UBUS_METHOD("get", wl_radio_get_param, radio_get_param_policy));

	//UBUS_METHOD_ADD(radio_methods, n_methods,
	//	UBUS_METHOD_NOARG("temperature", wl_temperature));

	UBUS_METHOD_ADD(radio_methods, n_methods,
		UBUS_METHOD("scan", wl_scan, wl_scan_policy));

	if (libwifi_supports(phyname, "wifi_scan_ex")) {
		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD("scan_ex", wl_scan_ex, wl_scan_ex_policy));
	}

	UBUS_METHOD_ADD(radio_methods, n_methods,
		UBUS_METHOD("scanresults", wl_scanresults, wl_scanres_policy));

	UBUS_METHOD_ADD(radio_methods, n_methods,
		UBUS_METHOD("autochannel", wl_autochannel, wl_acs_policy));

	if (libwifi_supports(phyname, "wifi_start_cac")) {
		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD("start_cac", wl_start_cac, wl_cac_policy));

		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD_NOARG("stop_cac", wl_stop_cac));
	}

	if (libwifi_supports(phyname, "wifi_add_iface")) {
		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD("add_iface", wl_add_iface, add_iface_policy));
	}

	if (libwifi_supports(phyname, "wifi_del_iface")) {
		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD("del_iface", wl_del_iface, del_iface_policy));
	}

	if (libwifi_supports(phyname, "wifi_channels_info")) {
		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD_NOARG("channels_info", wl_channels_info));
	}

	if (libwifi_supports(phyname, "wifi_get_opclass_preferences")) {
		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD_NOARG("opclass_preferences", wl_opclass_preferences));
	}

	if (libwifi_supports(phyname, "wifi_simulate_radar") &&
	                    (band == BAND_5 || band == BAND_DUAL || band == BAND_UNKNOWN)) {
		UBUS_METHOD_ADD(radio_methods, n_methods,
			UBUS_METHOD("simulate_radar", wl_simulate_radar, wl_radar_policy));
	}

	radio_obj->methods = radio_methods;
	radio_obj->n_methods = n_methods;

	return 0;
}

int wifimngr_add_radio_object(struct wifimngr *w,
				struct ubus_context *ctx,
				const char *radioname)
{
	struct wifi_ubus_object *wobj;
	int ret;
	char objname[32] = {0};

	wobj = calloc(1, sizeof(struct wifi_ubus_object));
	if (!wobj)
		return -ENOMEM;

	snprintf(objname, 28, "wifi.radio.%s", radioname);
	wobj->obj.name = strdup(objname);
	add_radio_methods(&wobj->obj, radioname);

	wobj->obj_type.name = wobj->obj.name;
	wobj->obj_type.n_methods = wobj->obj.n_methods;
	wobj->obj_type.methods = wobj->obj.methods;
	wobj->obj.type = &wobj->obj_type;

	ret = ubus_add_object(ctx, &wobj->obj);
	if (!ret) {
		/* wifimngr_dbg("Added '%s' radio obj\n", wobj->obj.name); */
		list_add_tail(&wobj->list, &w->radiolist);
	}

	return ret;
}

#define MAX_AP_METHODS	32
static int add_ap_methods(struct ubus_object *interface_obj,
				const char *ifname)
{
	int n_methods = 0;
	struct ubus_method *ap_methods;

	ap_methods = calloc(MAX_AP_METHODS, sizeof(struct ubus_method));
	if (!ap_methods)
		return -ENOMEM;

	UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD_NOARG("status", wl_ap_status));

	UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD_NOARG("stats", wl_ap_stats));

	UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD_NOARG("assoclist", wl_assoclist));

	UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("stations", wl_stations, stainfo_policy));

	if (libwifi_supports(ifname, "wifi_disconnect_sta")) {
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("disconnect", sta_disconnect, sta_disconnect_policy));
	}

	if (libwifi_supports(ifname, "wifi_probe_sta")) {
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("probe_sta", sta_probe, sta_monitor_policy));
	}

	if (libwifi_supports(ifname, "wifi_monitor_sta")) {
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("monitor_add", sta_monitor_add, sta_monitor_policy));
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("monitor_del", sta_monitor_del, sta_monitor_policy));
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("monitor_get", sta_monitor_get, sta_monitor_policy));
	}

	if (libwifi_supports(ifname, "wifi_subscribe_frame"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("subscribe_frame", subscribe_frame, subscribe_frame_policy));
	if (libwifi_supports(ifname, "wifi_unsubscribe_frame"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("unsubscribe_frame", unsubscribe_frame, subscribe_frame_policy));

	if (libwifi_supports(ifname, "wifi_add_neighbor"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("add_neighbor", nbr_add, nbr_add_policy));

	if (libwifi_supports(ifname, "wifi_del_neighbor"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("del_neighbor", nbr_del, nbr_del_policy));

	if (libwifi_supports(ifname, "wifi_get_neighbor_list"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("list_neighbor", nbr_list, nbr_list_policy));

	if (libwifi_supports(ifname, "wifi_req_beacon_report"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("request_neighbor", nbr_request, nbr_req_policy));

	if (libwifi_supports(ifname, "wifi_req_btm"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("request_btm", btm_request, btmreq_policy));

	/* deprecate */
	if (libwifi_supports(ifname, "wifi_req_bss_transition"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("request_transition", nbr_transition, transition_policy));

	if (libwifi_supports(ifname, "wifi_restrict_sta"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("assoc_control", assoc_control, assoc_cntrl_policy));

	if (libwifi_supports(ifname, "wifi_add_vendor_ie"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("add_vendor_ie", vsie_add, vsie_policy));

	if (libwifi_supports(ifname, "wifi_del_vendor_ie"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("del_vendor_ie", vsie_del, vsie_policy));

	if (libwifi_supports(ifname, "wifi_get_beacon_ies"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD_NOARG("dump_beacon", dump_beacon));

	if (libwifi_supports(ifname, "wifi_chan_switch"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("chan_switch", ap_chan_switch, chan_switch_policy));

	if (libwifi_supports(ifname, "wifi_link_measure"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("measure_link", ap_measure_link, linkmeas_policy));

	if (libwifi_supports(ifname, "wifi_mbo_disallow_assoc"))
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD("mbo_disallow_assoc", ap_mbo_disallow_assoc, mbo_disallow_assoc_policy));

	if (libwifi_supports(ifname, "wifi_ap_set_state")) {
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD_NOARG("up", ap_bss_up));
		UBUS_METHOD_ADD(ap_methods, n_methods,
			UBUS_METHOD_NOARG("down", ap_bss_down));
	}

	interface_obj->methods = ap_methods;
	interface_obj->n_methods = n_methods;

	return 0;
}

#define MAX_STA_METHODS	8
static int add_sta_methods(struct ubus_object *interface_obj,
				const char *ifname)
{
	int n_methods = 0;
	struct ubus_method *sta_methods;

	sta_methods = calloc(MAX_STA_METHODS, sizeof(struct ubus_method));
	if (!sta_methods)
		return -ENOMEM;

	UBUS_METHOD_ADD(sta_methods, n_methods,
			UBUS_METHOD_NOARG("status", wl_sta_status));

	UBUS_METHOD_ADD(sta_methods, n_methods,
			UBUS_METHOD_NOARG("stats", wl_sta_stats));

	if (libwifi_supports(ifname, "wifi_sta_disconnect_ap")) {
		UBUS_METHOD_ADD(sta_methods, n_methods,
			UBUS_METHOD("disconnect", wl_sta_disconnect_ap,
						ap_disconnect_policy));
	}

	if (libwifi_supports(ifname, "wifi_set_4addr")) {
		UBUS_METHOD_ADD(sta_methods, n_methods,
			UBUS_METHOD("4addr", wl_sta_4addr,
					     sta_4addr_policy));
	}

	interface_obj->methods = sta_methods;
	interface_obj->n_methods = n_methods;

	return 0;
}

#ifdef WIFIMNGR_GRANULAR_OBJECTS
static int wifi_iface_link(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	const char *ifname;

	ifname = ubus_ap_to_ifname(obj);
	wifimngr_dbg("%s: ifname: %s\n", __func__, ifname);

	//TODO: placeholder now

	return 0;
}

static int wifi_iface_qos(struct ubus_context *ctx, struct ubus_object *obj,
		struct ubus_request_data *req, const char *method,
		struct blob_attr *msg)
{
	const char *ifname;

	ifname = ubus_ap_to_ifname(obj);
	wifimngr_dbg("%s: ifname: %s\n", __func__, ifname);

	//TODO: placeholder now

	return 0;
}

static int wifi_iface_diag_counters(struct ubus_context *ctx,
					struct ubus_object *obj,
					struct ubus_request_data *req,
					const char *method,
					struct blob_attr *msg)
{
	const char *ifname;

	ifname = ubus_ap_to_ifname(obj);
	wifimngr_dbg("%s: ifname: %s\n", __func__, ifname);

	//TODO: placeholder now

	return 0;
}

#define MAX_INTERFACE_MGMT_METHODS	8
static int add_interface_mgmt_methods(struct ubus_object *iface_mgmt_obj,
				const char *ifname)
{
	int n_methods = 0;
	struct ubus_method *iface_mgmt_methods;

	iface_mgmt_methods = calloc(MAX_INTERFACE_MGMT_METHODS, sizeof(struct ubus_method));
	if (!iface_mgmt_methods)
		return -ENOMEM;

	UBUS_METHOD_ADD(iface_mgmt_methods, n_methods,
			UBUS_METHOD_NOARG("link", wifi_iface_link));

	UBUS_METHOD_ADD(iface_mgmt_methods, n_methods,
			UBUS_METHOD_NOARG("qos", wifi_iface_qos));

	iface_mgmt_obj->methods = iface_mgmt_methods;
	iface_mgmt_obj->n_methods = n_methods;

	return 0;
}

#define MAX_INTERFACE_DIAG_METHODS	8
static int add_interface_diag_methods(struct ubus_object *iface_diag_obj,
				const char *ifname)
{
	int n_methods = 0;
	struct ubus_method *iface_diag_methods;

	iface_diag_methods = calloc(MAX_INTERFACE_DIAG_METHODS, sizeof(struct ubus_method));
	if (!iface_diag_methods)
		return -ENOMEM;

	UBUS_METHOD_ADD(iface_diag_methods, n_methods,
			UBUS_METHOD_NOARG("counters", wifi_iface_diag_counters));

	iface_diag_obj->methods = iface_diag_methods;
	iface_diag_obj->n_methods = n_methods;

	return 0;
}

int wifimngr_add_interface_subobject(
			struct wifi_ubus_object *wobj,
			struct ubus_context *ctx,
			const char *ifname,
			const char *subobject_name,
			int (*add_object_methods)(struct ubus_object *o,
							const char *ifname))
{
	struct wifi_ubus_object *sobj;
	int ret;
	char objname[64] = {0};

	sobj = calloc(1, sizeof(struct wifi_ubus_object));
	if (!sobj)
		return -ENOMEM;

	snprintf(objname, 63, "wifi.ap.%s.%s", ifname, subobject_name);
	sobj->obj.name = strdup(objname);
	if (add_object_methods)
		add_object_methods(&sobj->obj, ifname);

	sobj->obj_type.name = sobj->obj.name;
	sobj->obj_type.n_methods = sobj->obj.n_methods;
	sobj->obj_type.methods = sobj->obj.methods;
	sobj->obj.type = &sobj->obj_type;

	ret = ubus_add_object(ctx, &sobj->obj);
	if (!ret)
		list_add_tail(&sobj->list, &wobj->sobjlist);

	return ret;
}
#endif /* WIFIMNGR_GRANULAR_OBJECTS */

int wifimngr_add_interface_object(struct wifimngr *w,
				struct ubus_context *ctx,
				struct wifimngr_iface *iface)
{
	struct wifi_ubus_object *wobj;
	int ret;
	char objname[32] = {0};

	wobj = calloc(1, sizeof(struct wifi_ubus_object));
	if (!wobj)
		return -ENOMEM;

	if (iface->mode == WIFI_AP) {
		snprintf(objname, 25, "wifi.ap.%s", iface->iface);
		add_ap_methods(&wobj->obj, iface->iface);
	} else if (iface->mode == WIFI_STA) {
		snprintf(objname, 31, "wifi.backhaul.%s", iface->iface);
		add_sta_methods(&wobj->obj, iface->iface);
	} else {
		/* unhandled wifi mode */
		free(wobj);
		return -EINVAL;
	}

	wobj->obj.name = strdup(objname);
	wobj->obj_type.name = wobj->obj.name;
	wobj->obj_type.n_methods = wobj->obj.n_methods;
	wobj->obj_type.methods = wobj->obj.methods;
	wobj->obj.type = &wobj->obj_type;

	ret = ubus_add_object(ctx, &wobj->obj);
	if (!ret)
		list_add_tail(&wobj->list, &w->iflist);


#ifdef WIFIMNGR_GRANULAR_OBJECTS
	INIT_LIST_HEAD(&wobj->sobjlist);

	/* create sub objects per-interface */
	wifimngr_add_interface_subobject(wobj, ctx,
					ifname,
					"mgmt",
					add_interface_mgmt_methods);

	wifimngr_add_interface_subobject(wobj, ctx,
					ifname,
					"diagnostic",
					add_interface_diag_methods);
#endif

	return ret;
}

int wifimngr_init(struct wifimngr **w)
{
	struct wifimngr *wm;
	char radios[WIFI_DEV_MAX_NUM][16] = {0};

	wm = calloc(1, sizeof(struct wifimngr));
	if (!wm)
		return -ENOMEM;

	INIT_LIST_HEAD(&wm->radiolist);
	INIT_LIST_HEAD(&wm->iflist);
	*w = wm;

	wifimngr_get_wifi_devices(radios);
	UNUSED(radios);

	return 0;
}

int wifimngr_exit(struct wifimngr *w)
{
	struct wifi_ubus_object *p, *tmp;

	if (!w)
		return -EINVAL;

	list_for_each_entry_safe(p, tmp, &w->iflist, list) {
		list_del(&p->list);
		if (p->obj.methods)
			free((void *)p->obj.methods);
		free(p);
	}

	list_for_each_entry_safe(p, tmp, &w->radiolist, list) {
		list_del(&p->list);
		if (p->obj.methods)
			free((void *)p->obj.methods);
		free(p);
	}

	list_del_init(&w->iflist);
	list_del_init(&w->radiolist);

	if (w->wifi) {
		free(w->wifi->type);
		free(w->wifi);
	}

	free(w);

	return 0;
}

void wifimngr_version(void)
{
	printf("version        : %s.%s\n", wifimngr_base_version,
					wifimngr_xtra_version);
	printf("libwifi version: %s\n",
					libwifi_get_version());
}
