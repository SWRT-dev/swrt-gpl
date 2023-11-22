/*
 * util.h - header for helpers used internally by this library
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
#ifndef WIFIUTILS_H
#define WIFIUTILS_H

#include "wifidefs.h"

#define libwifi_err(...)	pr_error("libwifi: " __VA_ARGS__)
#define libwifi_warn(...)	pr_warn("libwifi: " __VA_ARGS__)
#define libwifi_info(...)	pr_info("libwifi: " __VA_ARGS__)
#define libwifi_dbg(...)	pr_debug("libwifi: " __VA_ARGS__)

uint8_t *wifi_find_ie(uint8_t *ies, size_t len, uint8_t eid);
uint8_t *wifi_find_ie_ext(uint8_t *ies, size_t len, uint8_t ext_id);

uint8_t *wifi_find_vsie(uint8_t *ies, size_t len, uint8_t *oui, uint8_t type,
						uint8_t stype);

void wifi_cap_set_from_capability_information(uint8_t *bitmap, const uint8_t *ie, size_t ie_len);
void wifi_cap_set_from_ht_capabilities_info(uint8_t *bitmap, const uint8_t *ie, size_t ie_len);
void wifi_cap_set_from_vht_capabilities_info(uint8_t *bitmap, const uint8_t *ie, size_t ie_len);
void wifi_cap_set_from_extended_capabilities(uint8_t *bitmap, const uint8_t *ie, size_t ie_len);
void wifi_cap_set_from_rm_enabled_capabilities(uint8_t *bitmap, const uint8_t *ie, size_t ie_len);
void wifi_cap_set_from_fast_bss_transition(uint8_t *bitmap, const uint8_t *ie, size_t ie_len);
void wifi_cap_set_from_ie(uint8_t *caps_bitmap, uint8_t *ie, size_t len);
void wifi_wmm_set_from_ie(struct wifi_ap_wmm_ac *ac, uint8_t *ie, size_t len);

unsigned long wifi_mcs2rate(uint32_t mcs, uint32_t bw, uint32_t nss, enum wifi_guard gi);
uint32_t wifi_bw_enum2MHz(enum wifi_bw bw);


int wif_parse_wpa_ie(uint8_t *iebuf, size_t len, struct wifi_rsne *r);
void wifi_rsne_to_security(struct wifi_rsne *r, uint32_t *s);
int wifi_get_bss_security_from_ies(struct wifi_bss *e, uint8_t *ies, size_t len);
int wifi_oper_stds_set_from_ie(uint8_t *beacon_ies, size_t beacon_ies_len, uint8_t *std);
int wifi_ssid_advertised_set_from_ie(uint8_t *ies, size_t ies_len, bool *ssid_advertised);
int wifi_apload_set_from_ie(uint8_t *ies, size_t ies_len, struct wifi_ap_load *load);

const char * wifi_band_to_str(enum wifi_band band);
const char * wifi_bw_to_str(enum wifi_bw bw);
void correct_oper_std_by_band(enum wifi_band band, uint8_t *std);

#ifndef BIT
#define BIT(n)	(1U << (n))
#endif

bool __is_any_bit_set(unsigned int v, int sizeof_v, ...);
bool __is_all_bits_set(unsigned int v, int sizeof_v, ...);

#define is_any_bit_set(v, ...)	\
	__is_any_bit_set(v, 8 * sizeof(typeof(v)), __VA_ARGS__)

#define is_all_bits_set(v, ...)	\
	__is_all_bits_set(v, 8 * sizeof(typeof(v)), __VA_ARGS__)

#define is_any_bit_clear(v, ...)	\
	!__is_all_bits_set(v, 8 * sizeof(typeof(v)), __VA_ARGS__)

#define is_all_bits_clear(v, ...)	\
	!__is_any_bit_set(v, 8 * sizeof(typeof(v)), __VA_ARGS__)


#define IS_MFPR(rsn_caps)	!!(rsn_caps & BIT(6))
#define IS_MFPC(rsn_caps)	!!(rsn_caps & BIT(7))

#define IS_AKM_PSK(x)	\
		is_any_bit_set(x, WIFI_AKM_PSK, WIFI_AKM_FT_PSK, \
				WIFI_AKM_PSK_SHA256, WIFI_AKM_SAE, \
				WIFI_AKM_FT_SAE, -1)


#define IS_AKM_EAP(x)	\
		is_any_bit_set(x, WIFI_AKM_DOT1X, WIFI_AKM_FT_DOT1X, \
				WIFI_AKM_DOT1X_SHA256, WIFI_AKM_SUITE_B, \
				WIFI_AKM_SUITE_B_SHA384, \
				WIFI_AKM_FT_DOT1X_SHA384, -1)

#define IS_AKM_SAE(x)	\
		is_any_bit_set(x, WIFI_AKM_SAE, WIFI_AKM_FT_SAE, -1)


#define IS_AKM_WPA3PSK(x)	\
		is_any_bit_set(x, WIFI_AKM_SAE, -1) && \
		is_all_bits_clear(x, WIFI_AKM_PSK, WIFI_AKM_PSK_SHA256, -1)


#define IS_AKM_WPA3PSK_T(x)	\
		is_all_bits_set(x, WIFI_AKM_PSK, WIFI_AKM_SAE, -1)

#define IS_AKM_WPA2_FT_PSK(x) \
		is_all_bits_set(x, WIFI_AKM_PSK, WIFI_AKM_FT_PSK, -1)

#define IS_AKM_WPA3_FT_PSK(x)	\
		is_all_bits_set(x, WIFI_AKM_SAE, WIFI_AKM_FT_SAE, -1)

extern unsigned char microsoft_oui[];
#endif /* WIFIUTILS_H */
