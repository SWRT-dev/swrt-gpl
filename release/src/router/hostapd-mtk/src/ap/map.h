/*
 * hostapd / MAP definition
 * Copyright (c) 2015, Jouni Malinen <j@w1.fi>
 *
 * This software may be distributed under the terms of the BSD license.
 * See README for more details.
 */

#ifndef MAP_H
#define MAP_H

#define BAND_2G							0
#define BAND_5GL						1
#define BAND_5GH						2
#define BAND_NUM						3

/* spec v171027 */
enum MAPRole {
	MAP_ROLE_TEARDOWN = 4,
	MAP_ROLE_FRONTHAUL_BSS = 5,
	MAP_ROLE_BACKHAUL_BSS = 6,
	MAP_ROLE_BACKHAUL_STA = 7,
};

struct map_bh_profile {
	u8 bh_macaddr[ETH_ALEN];
	u8 bh_ssid[SSID_MAX_LEN];
	u8 bh_ssid_len; 
	u8 bh_wpa;
	int bh_wpa_key_mgmt;
	u8 bh_wpa_passphrase[32];
	int bh_rsn_pairwise;
	u8 bh_map_vendor_extension;
	u8 bh_hidden_ssid;
//	u8 bh_profile_valid;
};

#endif /* MAP_H */
