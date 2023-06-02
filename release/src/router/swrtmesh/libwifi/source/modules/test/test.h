/*
 * test.h - 'test' wifi module header
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

#ifndef TEST_WIFI_H
#define TEST_WIFI_H

enum test_attr {
	TEST_ATTR_UNDEFINED,
	TEST_ATTR_BSSID,
	TEST_ATTR_SSID,
	TEST_ATTR_CHANNEL,
	TEST_ATTR_SUPP_CHANNELS,
	TEST_ATTR_MAXRATE,
	TEST_ATTR_BANDWIDTH,
	TEST_ATTR_OPER_STDS,
	TEST_ATTR_SUPP_STDS,
	TEST_ATTR_BASIC_RATES,
	TEST_ATTR_SUPP_RATES,
	TEST_ATTR_COUNTRY,
};

typedef uint8_t macaddr_t[6];

static inline int ifname_to_band(const char *ifname)
{
	return strstr(ifname, "5") ? 5 : 2;
}

static inline int ifname_to_band_enum(const char *ifname)
{
	switch (ifname_to_band(ifname)) {
	case 2:
		return BAND_2;
	case 5:
		return BAND_5;
	default:
		return BAND_UNKNOWN;
	}
}


#define TESTDATA(band, attr)	test ## band ## _ ## attr
#define TESTDATALEN(band, attr)	sizeof(TESTDATA(band, attr))
#define TESTDATASTRLEN(band, attr)	strlen(TESTDATA(band, attr))

#define TESTARRAY(o, olen, type, band, attr)				\
do {									\
	const type __t[] = TESTDATA(band, attr);			\
	size_t __tlen = sizeof(__t)/sizeof(type);			\
	memcpy(o, __t, sizeof(__t));					\
	olen = __tlen;							\
} while(0)


#define GET_TEST_ARRAY(o, olen, type, iface, attr)			\
({									\
	if (ifname_to_band(iface) == 5)					\
		TESTARRAY(o, olen, type, 5, attr);			\
	else								\
		TESTARRAY(o, olen, type, 2, attr);			\
})

#define GET_TEST_ARRAY_SIZE(type, iface, attr)				\
({									\
	(ifname_to_band(iface) == 5) ?					\
		TESTDATALEN(5, attr)/sizeof(type) :			\
		TESTDATALEN(2, attr)/sizeof(type);			\
})

#define TESTARRAY_ENTRY(o, type, band, attr, idx)			\
do {									\
	const type *__t = TESTDATA(band, attr);				\
	size_t __tlen = TESTDATALEN(band, attr)/sizeof(type);		\
	if (idx >= 0 && idx < __tlen)					\
		memcpy(o, __t + idx, sizeof(type));			\
} while(0)

#define GET_TEST_ARRAY_ENTRY(o, type, iface, attr, idx)			\
({									\
	if (ifname_to_band(iface) == 5)					\
		TESTARRAY_ENTRY(o, type, 5, attr, idx);			\
	else								\
		TESTARRAY_ENTRY(o, type, 2, attr, idx);			\
})

#define GET_TEST_INT(p, iface, attr)		\
({						\
	if (ifname_to_band(iface) == 5)		\
		p = TESTDATA(5, attr);		\
	else					\
		p = TESTDATA(2, attr);		\
})

#define GET_TEST_STRING(p, iface, attr)					\
({									\
	if (ifname_to_band(iface) == 5)					\
		memcpy(p, TESTDATA(5, attr), TESTDATASTRLEN(5, attr));	\
	else								\
		memcpy(p, TESTDATA(2, attr), TESTDATASTRLEN(2, attr));	\
})

#define GET_TEST_BUF(o, olen, iface, attr)				\
({									\
	if (ifname_to_band(iface) == 5) {				\
		memcpy(o, TESTDATA(5, attr), TESTDATALEN(5, attr));	\
		olen = TESTDATALEN(5, attr);				\
	} else {							\
		memcpy(o, TESTDATA(2, attr), TESTDATALEN(2, attr));	\
		olen = TESTDATALEN(2, attr);				\
	}								\
})

#define GET_TEST_BUF_TYPE(o, iface, attr, type)				\
({									\
	if (ifname_to_band(iface) == 5)					\
		memcpy(o, TESTDATA(5, attr), sizeof(type));		\
	else								\
		memcpy(o, TESTDATA(2, attr), sizeof(type));		\
})


/* test data for BAND_5 follows */
#define test5_bssid		"\xaa\xbb\xcc\xdd\xee\xff"
#define test5_ssid		"Test SSID 5Ghz"
#define test5_channel		100
#define test5_oper_band		2
#define test5_maxrate		1732
#define test5_supp_stds		WIFI_A | WIFI_N | WIFI_AC
#define test5_bandwidth		BW80
#define test5_extch		EXTCH_ABOVE
#define test5_supp_bandwidth	BW160 | BW80 | BW40 | BW20
#define test5_countrycode	"EU"
#define test5_supp_channels	{36, 40, 44, 48, 52, 56, 60, 64, 100, 104, 108, 112, 116}
#define test5_noise		-88
#define test5_maxassoc		32
#define test5_ssid_advertised	true
#define test5_isolate		false
#define test5_assoclist		{"\x50\x80\x70\x60\x50\x40", "\x50\x31\x32\x33\x34\x35"}
#define test5_load_utilization	7
#define test5_beacon_int	100
#define test5_dtim_period	1
#define test5_guard_int		8
#define test5_security_auth	AUTH_WPAPSK | AUTH_WPA2PSK
#define test5_security_enc	CIPHER_TKIP | CIPHER_AES

#define test5_aggr_enabled	1
#define test5_frag_threshold	2346
#define test5_rts_threshold	2347
#define test5_lrl		2
#define test5_srl		1
#define test5_txpower		100
#define test5_txpower_dbm	-10
#define test5_dot11h_capable	1
#define test5_dot11h_enabled	1
#define test5_acs_capable	1
#define test5_acs_enabled	1
#define test5_acs_interval	86400
#define test5_tx_streams	4
#define test5_rx_streams	4
#define test5_supp_rates	{6, 9, 12, 18, 24, 36, 48, 54}
#define test5_oper_rates	{6, 9, 12, 18, 24, 36, 48, 54}
#define test5_basic_rates	{6, 12, 24}
#define test5_temperature	66

const struct wifi_radio_stats test5_radio_stats_data = {
	.tx_bytes = 100000000,
	.rx_bytes = 120000000,
	.tx_pkts = 900000,
	.rx_pkts = 1000000,
	.tx_err_pkts = 10,
	.rx_err_pkts = 20,
	.tx_dropped_pkts = 1,
	.rx_dropped_pkts = 2,
	.rx_plcp_err_pkts = 3,
	.rx_fcs_err_pkts = 4,
	.rx_mac_err_pkts = 5,
	.rx_unknown_pkts = 6,
	.noise = test5_noise,
};
#define test5_radio_stats	&test5_radio_stats_data

const struct wifi_radio_diagnostic test5_radio_diagnostic_data = {
	.channel_busy = 300000,
	.tx_airtime = 100000,
	.rx_airtime = 150000,
	.obss_airtime = 40000,
	.cca_time = 500000,
	.false_cca_count = 500,
};
#define test5_radio_diagnostic	&test5_radio_diagnostic_data

const struct wifi_ap_stats test5_ap_stats_data = {
	.tx_bytes = 10000000,
	.rx_bytes = 12000000,
	.tx_pkts = 900000,
	.rx_pkts = 1000000,
	.tx_err_pkts = 100,
	.tx_rtx_pkts = 10,
	.tx_rtx_fail_pkts = 1,
	.tx_retry_pkts = 50,
	.tx_mretry_pkts = 40,
	.ack_fail_pkts = 70,
	.aggr_pkts = 4000000,
	.rx_err_pkts = 50,
	.tx_ucast_pkts = 800000,
	.rx_ucast_pkts = 990000,
	.tx_dropped_pkts = 7,
	.rx_dropped_pkts = 6,
	.tx_mcast_pkts = 100000,
	.rx_mcast_pkts = 10000,
	.tx_bcast_pkts = 100000,
	.rx_bcast_pkts = 10000,
	.rx_unknown_pkts = 500,
};
#define test5_ap_stats	&test5_ap_stats_data

const struct wifi_caps test5_ap_caps_data = {
	.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
		 WIFI_CAP_HT_VALID | WIFI_CAP_VHT_VALID | WIFI_CAP_RM_VALID,
	.basic.byte = "\x31\x11",
	.ext.byte = "\x00\x00\x08\x00\x00\x00\x00\x40",
	.ht.byte = "\xef\x01",
	.vht.byte = "\xb1\x79\xc3\x33",
	.rrm.byte = "\xf3\xc0\x01\x00\x00",
};
#define test5_ap_caps	&test5_ap_caps_data

const struct wifi_bss test5_scanres[] = {
	{		.ssid = "Scan5.1",
			.bssid = {"\x00\x11\x12\x13\x14\x15"},
			.mode = WIFI_INFRA,
			.channel = 36,
			.curr_bw = BW80,
			.band = BAND_5,
			.supp_std = WIFI_A | WIFI_N | WIFI_AC,
			.oper_std = WIFI_AC,
			.rssi = -50,
			.noise = -85,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_CCMP,
			.beacon_int = 100,
			.dtim_period = 1,
			.load.sta_count = 1,
	},
	{		.ssid = "Scan5.2",
			.bssid = {"\x00\x21\x22\x23\x24\x25"},
			.mode = WIFI_INFRA,
			.channel = 60,
			.curr_bw = BW80,
			.band = BAND_5,
			.supp_std = WIFI_A | WIFI_N | WIFI_AC,
			.oper_std = WIFI_AC,
			.rssi = -55,
			.noise = -86,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_CCMP,
			.beacon_int = 200,
			.dtim_period = 2,
			.load.sta_count = 2,
	},
	{		.ssid = "Scan5.3",
			.bssid = {"\x00\x31\x32\x33\x34\x35"},
			.mode = WIFI_INFRA,
			.channel = 100,
			.curr_bw = BW40,
			.band = BAND_5,
			.supp_std = WIFI_A | WIFI_N | WIFI_AC,
			.oper_std = WIFI_AC,
			.rssi = -66,
			.noise = -82,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_CCMP,
			.beacon_int = 300,
			.dtim_period = 3,
			.load.sta_count = 3,
	},
	{		.ssid = "Scan5.4",
			.bssid = {"\x00\x41\x42\x43\x44\x45"},
			.mode = WIFI_INFRA,
			.channel = 60,
			.curr_bw = BW80,
			.band = BAND_5,
			.supp_std = WIFI_A | WIFI_N | WIFI_AC,
			.oper_std = WIFI_AC,
			.rssi = -61,
			.noise = -84,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_CCMP,
			.beacon_int = 100,
			.dtim_period = 1,
			.load.sta_count = 10,
	},
	{		.ssid = "Scan5.a-only",
			.bssid = {"\x00\x51\x52\x53\x54\x55"},
			.mode = WIFI_INFRA,
			.channel = 36,
			.curr_bw = BW20,
			.band = BAND_5,
			.supp_std = WIFI_A,
			.oper_std = WIFI_A,
			.rssi = -55,
			.noise = -88,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_TKIP,
			.beacon_int = 100,
			.dtim_period = 1,
			.load.sta_count = 2,
	},
	{		.ssid = "Scan5.an-only",
			.bssid = {"\x00\x61\x62\x63\x64\x65"},
			.mode = WIFI_INFRA,
			.channel = 36,
			.curr_bw = BW40,
			.band = BAND_5,
			.supp_std = WIFI_A | WIFI_N,
			.oper_std = WIFI_N,
			.rssi = -51,
			.noise = -90,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_CCMP,
			.beacon_int = 100,
			.dtim_period = 2,
			.load.sta_count = 0,
	},
};

#define test5_scanresults	test5_scanres

const struct wifi_caps test5_sta_caps_data[] = {
	{
		.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
			WIFI_CAP_HT_VALID | WIFI_CAP_VHT_VALID,
		.basic.byte = "\xff\xff",
		.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.ht.byte = "\xff\xff",
		.vht.byte = "\xff\xff\xff\xff",
		.rrm.byte = "\xff\xff\xff\xff\xff",
	},
	{
		.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
			WIFI_CAP_HT_VALID,
		.basic.byte = "\xff\xff",
		.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.ht.byte = "\xff\xff",
	}
};
#define test5_sta_caps	test5_sta_caps_data

const struct wifi_sta test5_stalist[] = {
	{
		.macaddr = {"\x50\x80\x70\x60\x50\x40"},
		/*.cbitmap = {},
		.caps = */
		.oper_std = WIFI_A | WIFI_N | WIFI_AC,
		.maxrate = 1300,
		.rx_rate.rate = 1300,
		.rx_rate.m.mcs = 9,
		.rx_rate.m.bw = 80,
		.rx_rate.m.sgi = 1,
		.rx_rate.m.nss = 3,
		.rx_rate.phy = 9,
		.tx_rate.rate = 1300,
		.tx_rate.m.mcs = 9,
		.tx_rate.m.bw = 80,
		.tx_rate.m.sgi = 1,
		.tx_rate.m.nss = 3,
		.tx_rate.phy = 9,
		.rx_thput = 555,
		.tx_thput = 500,
		.rssi_avg = -40,
		.rssi = { -40, -42, -42, -127},
		.stats.tx_bytes = 197716844,
		.stats.rx_bytes = 198917288,
		.stats.tx_pkts = 251851,
		.stats.rx_pkts = 137122,
		.stats.tx_err_pkts = 0,
		.stats.tx_rtx_pkts = 0,
		.stats.tx_rtx_fail_pkts = 0,
		.stats.tx_retry_pkts = 0,
		.stats.tx_mretry_pkts = 0,
		.stats.tx_fail_pkts = 0,
		.stats.rx_fail_pkts = 0,
		.tx_airtime = 40,
		.rx_airtime = 58,
		.airtime = 98,
		.conn_time = 300,
		.idle_time = 0,
	},
	{
		.macaddr = {"\x50\x31\x32\x33\x34\x35"},
		/* .cbitmap = {},
		.caps = */
		.oper_std = WIFI_A | WIFI_N,
		.maxrate = 300,
		.rx_rate.rate = 270,
		.rx_rate.m.mcs = 15,
		.rx_rate.m.bw = 40,
		.rx_rate.m.sgi = 0,
		.rx_rate.m.nss = 0,
		.rx_rate.phy = 7,
		.tx_rate.rate = 300,
		.tx_rate.m.mcs = 15,
		.tx_rate.m.bw = 40,
		.tx_rate.m.sgi = 1,
		.tx_rate.m.nss = 0,
		.tx_rate.phy = 7,
		.rx_thput = 5,
		.tx_thput = 20,
		.rssi_avg = -55,
		.rssi = { -53, -52, -127, -127},
		.stats.tx_bytes = 10789790,
		.stats.rx_bytes = 22229360,
		.stats.tx_pkts = 11311,
		.stats.rx_pkts = 26147,
		.stats.tx_err_pkts = 0,
		.stats.tx_rtx_pkts = 0,
		.stats.tx_rtx_fail_pkts = 0,
		.stats.tx_retry_pkts = 0,
		.stats.tx_mretry_pkts = 0,
		.stats.tx_fail_pkts = 0,
		.stats.rx_fail_pkts = 0,
		.tx_airtime = 3,
		.rx_airtime = 8,
		.airtime = 11,
		.conn_time = 60,
		.idle_time = 1,
	},
};

#define test5_stations		test5_stalist

const struct nbr test5_nbrs[] = {
	{
		.bssid = "\x50\x10\x00\x11\x22\x33",
		.bssid_info = 0x11223344,
		.reg = 0,
		.channel = 36,
		.phy = 9,
	},
	{
		.bssid = "\x50\x20\x00\x11\x22\x33",
		.bssid_info = 0x55667788,
		.reg = 0,
		.channel = 100,
		.phy = 9,
	},
	{
		.bssid = "\x50\x20\x00\x10\x20\x30",
		.bssid_info = 0x09aabbcc,
		.reg = 0,
		.channel = 60,
		.phy = 9,
	},
};
#define test5_neighbor_list	test5_nbrs

const struct sta_nbr test5_snbrs[] = {
	{
		.bssid = "\x50\x80\x70\x60\x50\x40",
		.rssi = -88,
		.rsni = 135
	},
	{
		.bssid = "\x50\x31\x32\x33\x34\x35",
		.rssi = -46,
		.rsni = 97
	}
};
#define test5_sta_neighbors	test5_snbrs

/* test data for BAND_2 follows */
#define test2_bssid		"\x00\x11\x22\x33\x44\x55"
#define test2_ssid		"Test SSID 2Ghz"
#define test2_channel		1
#define test2_oper_band		1
#define test2_maxrate		300
#define test2_supp_stds		WIFI_B | WIFI_G | WIFI_N
#define test2_bandwidth		BW20
#define test2_extch		EXTCH_NONE
#define test2_supp_bandwidth	BW40 | BW20
#define test2_countrycode	"EU"
#define test2_supp_channels	{1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13}
#define test2_noise		-92
#define test2_maxassoc		32
#define test2_ssid_advertised	true
#define test2_isolate		false
#define test2_assoclist		{"\x20\x80\x70\x60\x50\x40"}
#define test2_load_utilization	55
#define test2_beacon_int	200
#define test2_dtim_period	2
#define test2_guard_int		0
#define test2_security_auth	AUTH_WPAPSK | AUTH_WPA2PSK
#define test2_security_enc	CIPHER_TKIP | CIPHER_AES

#define test2_aggr_enabled	1
#define test2_frag_threshold	2346
#define test2_rts_threshold	2347
#define test2_lrl		2
#define test2_srl		1
#define test2_txpower		100
#define test2_txpower_dbm	-10
#define test2_dot11h_capable	1
#define test2_dot11h_enabled	0
#define test2_acs_capable	1
#define test2_acs_enabled	1
#define test2_acs_interval	86400
#define test2_tx_streams	2
#define test2_rx_streams	2
#define test2_supp_rates	{1, 2, 5, 11, 24, 36, 48, 54}
#define test2_oper_rates	{1, 2, 5, 11, 24, 36, 48, 54}
#define test2_basic_rates	{1, 2, 5, 11}
#define test2_temperature	65

const struct wifi_radio_stats test2_radio_stats_data = {
	.tx_bytes = 100000000,
	.rx_bytes = 120000000,
	.tx_pkts = 900000,
	.rx_pkts = 1000000,
	.tx_err_pkts = 10,
	.rx_err_pkts = 20,
	.tx_dropped_pkts = 1,
	.rx_dropped_pkts = 2,
	.rx_plcp_err_pkts = 0,
	.rx_fcs_err_pkts = 0,
	.rx_mac_err_pkts = 0,
	.rx_unknown_pkts = 0,
	.noise = test2_noise,
};
#define test2_radio_stats	&test2_radio_stats_data

const struct wifi_radio_diagnostic test2_radio_diagnostic_data = {
	.channel_busy = 600000,
	.tx_airtime = 400000,
	.rx_airtime = 120000,
	.obss_airtime = 80000,
	.cca_time = 380000,
	.false_cca_count = 800,
};
#define test2_radio_diagnostic	&test2_radio_diagnostic_data

const struct wifi_ap_stats test2_ap_stats_data = {
	.tx_bytes = 20000000,
	.rx_bytes = 22000000,
	.tx_pkts = 1900000,
	.rx_pkts = 1200000,
	.tx_err_pkts = 200,
	.tx_rtx_pkts = 20,
	.tx_rtx_fail_pkts = 1,
	.tx_retry_pkts = 50,
	.tx_mretry_pkts = 40,
	.ack_fail_pkts = 70,
	.aggr_pkts = 4000000,
	.rx_err_pkts = 50,
	.tx_ucast_pkts = 850000,
	.rx_ucast_pkts = 950000,
	.tx_dropped_pkts = 27,
	.rx_dropped_pkts = 26,
	.tx_mcast_pkts = 200000,
	.rx_mcast_pkts = 20000,
	.tx_bcast_pkts = 200000,
	.rx_bcast_pkts = 20000,
	.rx_unknown_pkts = 600,
};
#define test2_ap_stats	&test2_ap_stats_data

const struct wifi_caps test2_ap_caps_data = {
	.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
		 WIFI_CAP_HT_VALID,
	.basic.byte = "\x31\x11",
	.ext.byte = "\x00\x00\x08\x00\x00\x00\x00\x40",
	.ht.byte = "\xef\x01",
};
#define test2_ap_caps	&test2_ap_caps_data


const struct wifi_bss test2_scanres[] = {
	{		.ssid = "Scan2.1",
			.bssid = {"\x20\x11\x12\x13\x14\x15"},
			.mode = WIFI_INFRA,
			.channel = 1,
			.curr_bw = BW20,
			.band = BAND_2,
			.supp_std = WIFI_B | WIFI_G | WIFI_N,
			.oper_std = WIFI_N,
			.rssi = -40,
			.noise = -89,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_TKIP,
			.beacon_int = 100,
			.dtim_period = 1,
			.load.sta_count = 3,
			.load.utilization = 33,
	},
	{		.ssid = "Scan2.2",
			.bssid = {"\x20\x21\x22\x23\x24\x25"},
			.mode = WIFI_INFRA,
			.channel = 6,
			.curr_bw = BW20,
			.band = BAND_2,
			.supp_std = WIFI_B | WIFI_G | WIFI_N,
			.oper_std = WIFI_N,
			.rssi = -45,
			.noise = -92,
			.rsn.akms = WIFI_AKM_FT_PSK,
			.rsn.group_cipher = WIFI_CIPHER_CCMP,
			.rsn.pair_ciphers = WIFI_CIPHER_CCMP,
			.security = WIFI_SECURITY_WPA2PSK,
			.enc = WIFI_CIPHER_CCMP,
			.beacon_int = 100,
			.dtim_period = 1,
			.load.sta_count = 1,
			.load.utilization = 5,
	},
};

#define test2_scanresults	test2_scanres

const struct wifi_caps test2_sta_caps_data[] = {
	{
		.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
			WIFI_CAP_HT_VALID,
		.basic.byte = "\xff\xff",
		.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.ht.byte = "\xff\xff",
	}
};
#define test2_sta_caps	test2_sta_caps_data

const struct wifi_sta test2_stalist[] = {
	{
		.macaddr = {"\x20\x80\x70\x60\x50\x40"},
		/* .cbitmap = {0},
		.caps = */
		.oper_std = WIFI_B | WIFI_G | WIFI_N,
		.maxrate = 300,
		.rx_rate.rate = 300,
		.rx_rate.m.mcs = 7,
		.rx_rate.m.bw = 20,
		.rx_rate.m.sgi = 1,
		.rx_rate.m.nss = 0,
		.rx_rate.phy = 7,
		.tx_rate.rate = 300,
		.tx_rate.m.mcs = 7,
		.tx_rate.m.bw = 20,
		.tx_rate.m.sgi = 1,
		.tx_rate.m.nss = 0,
		.tx_rate.phy = 7,
		.rx_thput = 0,
		.tx_thput = 0,
		.rssi_avg = -45,
		.rssi = { -45, -44, -127, -127},
		.stats.tx_bytes = 3912221,
		.stats.rx_bytes = 36176593,
		.stats.tx_pkts = 23241,
		.stats.rx_pkts = 40204,
		.stats.tx_err_pkts = 0,
		.stats.tx_rtx_pkts = 0,
		.stats.tx_rtx_fail_pkts = 0,
		.stats.tx_retry_pkts = 0,
		.stats.tx_mretry_pkts = 0,
		.stats.tx_fail_pkts = 0,
		.stats.rx_fail_pkts = 0,
		.tx_airtime = 0,
		.rx_airtime = 2,
		.airtime = 2,
		.conn_time = 600,
		.idle_time = 2,
	},
};

#define test2_stations		test2_stalist

const struct nbr test2_nbrs[] = {
	{
		.bssid = {"\x20\x10\x00\x11\x22\x33"},
		.bssid_info = 0x11223344,
		.reg = 0,
		.channel = 6,
		.phy = 7,
	},
	{
		.bssid = {"\x20\x20\x00\x11\x22\x33"},
		.bssid_info = 0x55667788,
		.reg = 0,
		.channel = 1,
		.phy = 5,
	},
};
#define test2_neighbor_list	test2_nbrs

const struct sta_nbr test2_snbrs[] = {
	{
		.bssid = {"\x20\x80\x70\x60\x50\x40"},
		.rssi = -45,
		.rsni = 154
	}
};
#define test2_sta_neighbors	test2_snbrs

const struct wifi_metainfo test5_drv_info_data = {
	.vendor_id = "0x55aa",
	.device_id = "0x0102",
	.drv_data = "802.11 driver version 1.0.0",
	.fw_data = "0.0.1000.1 built-01-Jan-2020",
};
#define test5_drv_info	&test5_drv_info_data

const struct wifi_metainfo test2_drv_info_data = {
	.vendor_id = "0x22aa",
	.device_id = "0x0503",
	.drv_data = "802.11 driver version 1.0.0",
	.fw_data = "0.0.1000.1 built-01-Jan-2020",
};
#define test2_drv_info	&test2_drv_info_data

#define test5_beacon_dump		"\x00\x07\x62\x69\x67\x69\x31\x32\x33" \
		"\x01\x08\x8c\x12\x98\x24\xb0\x48\x60\x6c" \
		"\x05\x04\x01\x02\x00\x00" \
		"\x30\x14\x01\x00\x00\x0f\xac\x04\x01\x00\x00\x0f\xac\x04\x01\x00\x00\x0f\xac\x02\x0c\x00" \
		"\x0b\x05\x02\x00\x16\x00\x00" \
		"\x2d\x1a\xef\x01\x17\xff\xff\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
		"\x3d\x16\x24\x05\x04\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
		"\x7f\x08\x04\x00\x08\x80\x00\x00\x00\x40" \
		"\xbf\x0c\xb2\x69\x8b\x0f\xaa\xff\x00\x00\xaa\xff\x00\x00" \
		"\xc0\x05\x01\x2a\x00\x00\x00" \
		"\xc3\x04\x02\x02\x02\x02" \
		"\xdd\x05\x00\x90\x4c\x04\x17" \
		"\xdd\x09\x00\x10\x18\x02\x02\x00\x1c\x00\x00" \
		"\xdd\x18\x00\x50\xf2\x02\x01\x01\x88\x00\x03\xa4\x00\x00\x27\xa4\x00\x00\x42\x43\x5e\x00\x62\x32\x2f\x00" \
		"\xdd\x07\x50\x6f\x9a\x16\x01\x01\x00"


#define test2_beacon_dump 		"\x00\x16\x69\x6f\x70\x73\x79\x73\x57\x72\x74\x2d\x30\x30\x32\x32\x30\x37\x37\x31\x31\x34\x38\x34" \
		"\x01\x08\x82\x84\x8b\x96\x24\x30\x48\x6c" \
		"\x03\x01\x01" \
		"\x05\x04\x01\x02\x00\x00" \
		"\x2a\x01\x00" \
		"\x32\x04\x0c\x12\x18\x60" \
		"\x30\x14\x01\x00\x00\x0f\xac\x04\x01\x00\x00\x0f\xac\x04\x01\x00\x00\x0f\xac\x02\x0c\x00" \
		"\x0b\x05\x00\x00\x0e\x00\x00" \
		"\x42\x01\x00" \
		"\x46\x05\x00\x00\x00\x00\x65" \
		"\x2d\x1a\xbc\x09\x1b\xff\xff\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
		"\x3d\x16\x01\x08\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00\x00" \
		"\x7f\x08\x04\x00\x08\x00\x00\x00\x00\x40" \
		"\xdd\x18\x00\x50\xf2\x04\x10\x4a\x00\x01\x10\x10\x44\x00\x01\x02\x10\x49\x00\x06\x00\x37\x2a\x00\x01\x20" \
		"\xdd\x09\x00\x10\x18\x02\x00\x00\x1c\x00\x00"

const struct wifi_monsta test5_monstas[] = {
	{
		.macaddr = {"\x15\x25\x35\x45\x55\x65"},
		.rssi = {-66, -67, -66, -64},
		.rssi_avg = -65,
		.last_seen = 2,
		.caps.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
				WIFI_CAP_HT_VALID | WIFI_CAP_VHT_VALID,
		.caps.basic.byte = "\xff\xff",
		.caps.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.caps.ht.byte = "\xff\xff",
		.caps.vht.byte = "\xff\xff\xff\xff",
		.caps.rrm.byte = "\xff\xff\xff\xff\xff"
	},
	{
		.macaddr = {"\x11\x22\x33\x44\x55\x66"},
		.rssi = {-58, -57, -58, -58},
		.rssi_avg = -58,
		.last_seen = 8,
		.caps.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
				WIFI_CAP_HT_VALID | WIFI_CAP_VHT_VALID,
		.caps.basic.byte = "\xff\xff",
		.caps.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.caps.ht.byte = "\xff\xff",
		.caps.vht.byte = "\xff\xff\xff\xff",
		.caps.rrm.byte = "\xff\xff\xff\xff\xff"
	},
	{
		.macaddr = {"\x55\x55\x55\x55\x55\x55"},
		.rssi = {-78, -78, -78, -79},
		.rssi_avg = -78,
		.last_seen = 6,
		.caps.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
				WIFI_CAP_HT_VALID,
		.caps.basic.byte = "\xff\xff",
		.caps.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.caps.ht.byte = "\xff\xff"
	}
};
#define test5_monitor_get	test5_monstas

const struct wifi_monsta test2_monstas[] = {
	{
		.macaddr = {"\x16\x26\x36\x46\x56\x66"},
		.rssi = {-60, -61, -61, -62},
		.rssi_avg = -61,
		.last_seen = 7,
		.caps.valid = WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
				WIFI_CAP_HT_VALID,
		.caps.basic.byte = "\xff\xff",
		.caps.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.caps.ht.byte = "\xff\xff"
	},
	{
		.macaddr = {"\x66\x55\x44\x33\x22\x11"},
		.rssi = {-66, -66, -65, -67},
		.rssi_avg = -66,
		.last_seen = 3,
		.caps.valid =WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
				WIFI_CAP_HT_VALID,
		.caps.basic.byte = "\xff\xff",
		.caps.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.caps.ht.byte = "\xff\xff"
	},
	{
		.macaddr = {"\x66\x66\x66\x66\x66\x66"},
		.rssi = {-55, -54, -54, -54},
		.rssi_avg = -54,
		.last_seen = 22,
		.caps.valid =WIFI_CAP_BASIC_VALID | WIFI_CAP_EXT_VALID | \
				WIFI_CAP_HT_VALID,
		.caps.basic.byte = "\xff\xff",
		.caps.ext.byte = "\xff\xff\xff\xff\xff\xff\xff\xff",
		.caps.ht.byte = "\xff\xff"
	}
};
#define test2_monitor_get	test2_monstas

#endif /* TEST_WIFI_H */
