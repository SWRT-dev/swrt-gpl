/**
 * @file wifidefs.h
 * @brief WiFi defintions.
 *
 * Copyright (C) 2020 iopsys Software Solutions AB. All rights reserved.
 *
 * @author anjan.chanda@iopsys.eu
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

#ifndef WIFIDEFS_H
#define WIFIDEFS_H

#include <stdint.h>
#include <stdbool.h>
#include <linux/types.h>

#ifdef __cplusplus
extern "C" {
#endif

#define WIFI_NUM_AC		4
#define WIFI_NUM_ANTENNA	4
#define WIFI_NUM_WEPKEYS	4
#define WIFI_NUM_RADIUS		2

/** wifi status and reason code as per IEEE802.11 Std. */
typedef uint16_t wifi_status_t, wifi_reason_t;


/*
 * enum wifi_std - wifi standards as per IEEE802.11 Std.
 */
enum wifi_std {
	WIFI_B  = 1 << 0,        /**< 802.11b */
	WIFI_G  = 1 << 1,        /**< 802.11g */
	WIFI_A  = 1 << 2,        /**< 802.11a */
	WIFI_N  = 1 << 3,        /**< 802.11n */
	WIFI_AC = 1 << 4,        /**< 802.11ac */
	WIFI_AX = 1 << 5,        /**< 802.11ax */
	WIFI_BE = 1 << 6,        /**< 802.11be */

	WIFI_NUM_STD = 7,
	WIFI_UNKNOWN = 1 << WIFI_NUM_STD,
};

/** enum wifi_band - frequency bands */
enum wifi_band {
	BAND_2       = 1 << 0,         /**< 0x1 for 2.4 Ghz band. */
	BAND_5       = 1 << 1,         /**< 0x2 for 5 Ghz band. */
	BAND_DUAL    = 3,              /**< 0x3 for both 2.4 and 5 Ghz bands */
	BAND_60      = 1 << 2,         /**< 0x4 for 60 Ghz */
	BAND_6       = 1 << 3,         /**< 0x8 for 6 Ghz */
	BAND_UNII_1  = 1 << 4,         /**< 0x10 (5.15 to 5.25) GHz */
	BAND_UNII_2  = 1 << 5,         /**< 0x20 (5.25 to 5.725) GHz */
	BAND_UNII_3  = 1 << 6,         /**< 0x40 (5.725 to 5.85) GHz */
	BAND_UNII_4  = 1 << 7,         /**< 0x80 (5.85 to 5.925) GHz */
	BAND_UNII_5  = 1 << 8,         /**< 0x100 (5.925 to 6.425) GHz */
	BAND_UNII_6  = 1 << 9,         /**< 0x200 (6.425 to 6.525) GHz */
	BAND_UNII_7  = 1 << 10,        /**< 0x400 (6.525 to 6.875) GHz */
	BAND_UNII_8  = 1 << 11,        /**< 0x800 (6.875 to 7.125) GHz */

	BAND_ANY     = 1 << 12,        /**< 0x1000 any band */
	BAND_UNKNOWN = 1 << 13,        /**< 0x2000 (and above) unsupported band */
};

/** enum wifi_bw - bandwidth */
enum wifi_bw {
	BW20,             /**< bandwidth = 20 Mhz */
	BW40,             /**< bandwidth = 40 Mhz */
	BW80,             /**< bandwidth = 80 Mhz */
	BW160,            /**< bandwidth = 160 Mhz */
	BW_AUTO,          /**< bandwidth = ? */
	BW8080 = 7,       /**< bandwidth = 80+80 Mhz */
	BW320,		  /**< bandwidth = 320 MHz */
	BW_UNKNOWN,       /**< Unknown or Undefined */
};

/** enum wifi_chan_ext - extension channel */
enum wifi_chan_ext {
	EXTCH_NONE,     /**< no extension channel */
	EXTCH_ABOVE,    /**< extension channel above primary */
	EXTCH_BELOW,    /**< extension channel below primary */
	EXTCH_AUTO,     /**< extension channel auto */
};

/** enum wifi_guard - guard interval */
enum wifi_guard {
	WIFI_GI400,
	WIFI_SGI = WIFI_GI400,         /**< 0.4us guard interval */
	WIFI_GI800,
	WIFI_LGI = WIFI_GI800,         /**< 0.8us guard interval for not HE device */
	WIFI_1xLTF_GI800,              /**< 0.8us guard interval for HE device */
	WIFI_1xLTF_GI1600,             /**< 1.6us guard interval for HE device */
	WIFI_2xLTF_GI800,              /**< 0.8us guard interval for HE device */
	WIFI_2xLTF_GI1600,             /**< 1.6us guard interval for HE device */
	WIFI_4xLTF_GI800,              /**< 0.8us guard interval for HE device */
	WIFI_4xLTF_GI3200,             /**< 3.2us guard interval for HE device */
	WIFI_GI_UNKNOWN,
	WIFI_AUTOGI = WIFI_GI_UNKNOWN, /**< guard interval auto or Unknown */
};

/** enum wifi_preamble - preamble */
enum wifi_preamble {
	SHORT_PREAMBLE,    /**< short preamble */
	LONG_PREAMBLE,     /**< long preamble */
	AUTO_PREAMBLE      /**< auto preamble */
};

/** enum wifi_bss_mode - wifi bss operational mode */
enum wifi_bss_mode {
	WIFI_ADHOC,           /**< Adhoc (or IBSS) mode of operation */
	WIFI_INFRA,           /**< Infrastructure (or AP) mode */
};

/** enum wifi_mode - wifi mode */
enum wifi_mode {
	WIFI_MODE_UNKNOWN,
	WIFI_MODE_AP,         /**< AP (or Master) mode */
	WIFI_MODE_AP_VLAN,    /**< WDS Station */
	WIFI_MODE_STA,        /**< Station (or Managed) mode */
	WIFI_MODE_MONITOR,    /**< Monitor mode */
};

/** enum wifi_security - wifi security types (user friendly names) */
enum wifi_security {
	WIFI_SECURITY_NONE,
	WIFI_SECURITY_WEP64,
	WIFI_SECURITY_WEP128,
	WIFI_SECURITY_WPAPSK,
	WIFI_SECURITY_WPA2PSK,
	WIFI_SECURITY_FT_WPA2PSK,
	WIFI_SECURITY_WPA3PSK,
	WIFI_SECURITY_WPA3PSK_T,
	WIFI_SECURITY_FT_WPA3PSK,
	WIFI_SECURITY_WPA,
	WIFI_SECURITY_WPA2,
	WIFI_SECURITY_WPA3,
};

/** @enum wifi_wpaversion - wpa versions */
enum wifi_wpaversion {
	WPA_VERSION1 = 0x100,
	WPA_VERSION2 = 0x200,
};

/** enum wifi_akm - auth and key management types */
enum wifi_akm {
	WIFI_AKM_NONE,
	WIFI_AKM_DOT1X,
	WIFI_AKM_PSK,
	WIFI_AKM_FT_DOT1X,
	WIFI_AKM_FT_PSK,           /* 4 */
	WIFI_AKM_DOT1X_SHA256,
	WIFI_AKM_PSK_SHA256,
	WIFI_AKM_TDLS,
	WIFI_AKM_SAE,              /* 8 */
	WIFI_AKM_FT_SAE,
	WIFI_AKM_AP_PEER_KEY,
	WIFI_AKM_SUITE_B,
	WIFI_AKM_SUITE_B_SHA384,   /* 12 */
	WIFI_AKM_FT_DOT1X_SHA384,
};

/** enum wifi_cipher - cipher types */
enum wifi_cipher {
	WIFI_CIPHER_GRP,
	WIFI_CIPHER_WEP40,
	WIFI_CIPHER_TKIP,
	WIFI_CIPHER_CCMP128 = 4,
	WIFI_CIPHER_CCMP = WIFI_CIPHER_CCMP128,
	WIFI_CIPHER_AES = WIFI_CIPHER_CCMP,
	WIFI_CIPHER_WEP104 = 5,
	WIFI_CIPHER_BIP_CMAC128,
	WIFI_CIPHER_NOGRP,
	WIFI_CIPHER_GCMP128 = 8,
	WIFI_CIPHER_GCMP = WIFI_CIPHER_GCMP128,
	WIFI_CIPHER_GCMP256,
	WIFI_CIPHER_CCMP256,
	WIFI_CIPHER_BIP_GMAC128,
	WIFI_CIPHER_BIP_GMAC256,
	WIFI_CIPHER_BIP_CMAC256,
};

/* authentication types bitflags */
enum {
	AUTH_OPEN    = 1<<0,
	AUTH_SHARED  = 1<<1,
	AUTH_WEPAUTO = 1<<2,
	AUTH_WPANONE = 1<<3,
	AUTH_WPA     = 1<<4,
	AUTH_WPAPSK  = 1<<5,
	AUTH_WPA2    = 1<<6,
	AUTH_WPA2PSK = 1<<7,
	AUTH_MAX = 8,
	AUTH_UNKNOWN = 1 << AUTH_MAX,
};

/* encryption types bitflags */
enum {
	CIPHER_NONE    = 1<<0,
	CIPHER_WEP     = 1<<1,
	CIPHER_TKIP    = 1<<2,
	CIPHER_AES     = 1<<3,
	CIPHER_CCMP256 = 1<<4,
	CIPHER_GCMP128 = 1<<5,
	CIPHER_GCMP256 = 1<<6,
	CIPHER_MAX = 7,
	CIPHER_UNKNOWN = 1 << CIPHER_MAX,
};

/** enum wifi_phytype - wifi phys */
enum wifi_phytype {
	PHY_DSSS   = 2,
	PHY_OFDM   = 4,
	PHY_HRDSSS = 5,
	PHY_ERP    = 6,
	PHY_HT     = 7,
	PHY_DMG    = 8,
	PHY_VHT    = 9,
	PHY_TVHT   = 10,
	PHY_S1G    = 11,
	PHY_CDMG   = 12,
	PHY_CMMG   = 13,
	PHY_HE     = 14,
};

/** enum wifi_cac_method - wifi CAC methods */
enum wifi_cac_method {
	WIFI_CAC_CONTINUOUS,
	WIFI_CAC_DEDICATED,
	WIFI_CAC_MIMO_REDUCED,
	WIFI_CAC_TIME_SLICED,
};

/** @struct mimo_rate - used for phyrate calculation */
struct mimo_rate {
	uint8_t mcs;        /**< MCS value */
	uint8_t bw;         /**< Bandwidth in Mhz */
	uint8_t sgi;        /**< = 1 if SGI enabled; else 0 */
	uint8_t nss;        /**< Number of SS */
} __attribute__((packed));

/** struct wifi_rate - holds rate information */
struct wifi_rate {
	uint32_t rate;           /**< rate in Mbps */
	struct mimo_rate m;      /**< of type struct #mimo_rate */
	enum wifi_phytype phy;   /**< of type struct #wifi_phytype */
};

/*
 * The following struct wifi_caps_* defines capability bits as
 * defined in IEEE-802.11-2016 Std.
 */

/* struct wifi_caps_basic - capability field */
struct wifi_caps_basic {
	union {
		uint8_t byte[2];
		uint16_t cap;
	};
};

/* struct wifi_caps_ext - extended capabilities */
struct wifi_caps_ext {
	uint8_t byte[16];  /** upto 8 octets used now */
};

/* struct wifi_caps_ht - ht capabilities */
struct wifi_caps_ht {
	union {
		uint8_t byte[2];
		uint16_t cap;
	};
	uint8_t ampdu_params;
	uint8_t supp_mcs[16];
	union {
		uint8_t byte_ext[2];
		uint16_t cap_ext;
	};
	uint8_t txbf[4];
	uint8_t asel;
} __attribute__ ((packed));

/* struct wifi_caps_vht - vht capabilities */
struct wifi_caps_vht {
	union {
		uint8_t byte[4];
		uint32_t cap;
	};
	uint8_t supp_mcs[8];
};

/* struct wifi_caps_rm - radio measurement capability */
struct wifi_caps_rm {
	uint8_t byte[5];
};

/* struct wifi_caps_he - he capabilities */
struct wifi_caps_he {
	uint8_t byte_mac[6];
	uint8_t byte_phy[11];
	uint8_t byte_opt[46];
};

/* struct wifi_caps_eht - eht capabilities */
struct wifi_caps_eht {
	uint8_t byte_mac[2];
	uint8_t byte_phy[9];

	/* optional */
	uint8_t supp_mcs[13];
	uint8_t byte_ppe_th[62];
};

/* struct wifi_radio_args - wifi simulate radar arguments */
struct wifi_radar_args {
	uint32_t channel;
	uint32_t bandwidth;
	uint32_t type;
	uint32_t subband_mask;
};

/** enum wifi_capflags - wifi capability bit-flags */
enum wifi_capflags {
	WIFI_CAP_ESS,
	WIFI_CAP_IBSS,
	WIFI_CAP_SHORT_PREAMBLE,
	WIFI_CAP_SPECTRUM_MGMT,
	WIFI_CAP_WMM,
	WIFI_CAP_SHORT_SLOT,
	WIFI_CAP_APSD,
	WIFI_CAP_RADIO_MEAS,
	WIFI_CAP_DELAYED_BA,
	WIFI_CAP_IMMEDIATE_BA,

	/* Extended capability flags */
	WIFI_CAP_2040_COEX,
	WIFI_CAP_EXT_CHSWITCH,
	WIFI_CAP_PSMP,
	WIFI_CAP_PROXY_ARP,
	WIFI_CAP_11V_BSS_TRANS,
	WIFI_CAP_MULTI_BSSID,
	WIFI_CAP_SSID_LIST,
	WIFI_CAP_INTERWORKING,
	WIFI_CAP_QOSMAP,
	WIFI_CAP_TDLS,
	WIFI_CAP_SCS,
	WIFI_CAP_QLOAD_REPORT,
	WIFI_CAP_OMI,
	WIFI_CAP_MSCS,
	WIFI_CAP_TWT_REQ,
	WIFI_CAP_TWT_RSP,

	/* HT flags */
	WIFI_CAP_HT_LDPC,
	WIFI_CAP_2040,
	WIFI_CAP_HT_SMPS,
	WIFI_CAP_HT_GREENFIELD,
	WIFI_CAP_SGI20,
	WIFI_CAP_SGI40,
	WIFI_CAP_HT_TX_STBC,
	WIFI_CAP_HT_RX_STBC,
	WIFI_CAP_HT_MAX_AMSDU_3839,
	WIFI_CAP_HT_MAX_AMSDU_7935,
	WIFI_CAP_HT_CCK40,

	/* VHT related flags */
	WIFI_CAP_VHT_MPDU_3895,
	WIFI_CAP_VHT_MPDU_7991,
	WIFI_CAP_VHT_MPDU_11454,
	WIFI_CAP_160,
	WIFI_CAP_8080,
	WIFI_CAP_VHT_RX_LDPC,
	WIFI_CAP_SGI80,
	WIFI_CAP_SGI160,
	WIFI_CAP_VHT_TX_STBC,
	WIFI_CAP_VHT_RX_STBC_1SS,
	WIFI_CAP_VHT_RX_STBC_2SS,
	WIFI_CAP_VHT_RX_STBC_3SS,
	WIFI_CAP_VHT_RX_STBC_4SS,
	WIFI_CAP_VHT_SU_BFR,
	WIFI_CAP_VHT_SU_BFE,
	WIFI_CAP_VHT_MU_BFR,
	WIFI_CAP_VHT_MU_BFE,

	/* 11r FT flags */
	WIFI_CAP_FT_BSS,
	WIFI_CAP_FT_RRP,

	/* 11k RRM flags */
	WIFI_CAP_RM_LINK,
	WIFI_CAP_RM_NBR_REPORT,
	WIFI_CAP_RM_BCN_PASSIVE,
	WIFI_CAP_RM_BCN_ACTIVE,
	WIFI_CAP_RM_BCN_TABLE,
	WIFI_CAP_RM_RCPI,
	WIFI_CAP_RM_RSNI,
	WIFI_CAP_BSS_AVG_ACCESS_DELAY,
	WIFI_CAP_BSS_AVAIL_ADMISSION,

	/* HE flags */
	WIFI_CAP_HE_TWT_REQ,
	WIFI_CAP_HE_TWT_RSP,
	WIFI_CAP_HE_ALL_ACK,
	WIFI_CAP_HE_TRS,
	WIFI_CAP_HE_BSR,
	WIFI_CAP_HE_BCAST_TWT,
	WIFI_CAP_HE_32BIT_BA_BMP,
	WIFI_CAP_HE_OM_CONTROL,
	WIFI_CAP_HE_OFDMA_RA,
	WIFI_CAP_HE_AMSDU_FRAG,
	WIFI_CAP_HE_FLEX_TWT,
	WIFI_CAP_HE_QTP,
	WIFI_CAP_HE_BQR,
	WIFI_CAP_HE_SRP_RSP,
	WIFI_CAP_HE_OPS,
	WIFI_CAP_HE_AMSDU_IN_AMPDU,
	WIFI_CAP_HE_DYN_SMPS,
	WIFI_CAP_HE_40_BAND2,
	WIFI_CAP_HE_4080_BAND5,
	WIFI_CAP_HE_160_BAND5,
	WIFI_CAP_HE_160_8080_BAND5,
	WIFI_CAP_HE_242RU_BAND2,
	WIFI_CAP_HE_242RU_BAND5,
	WIFI_CAP_HE_PREAMBLE_PUNC_RX,
	WIFI_CAP_HE_LDPC_PAYLOAD,
	WIFI_CAP_HE_STBC_TX_80,
	WIFI_CAP_HE_STBC_RX_80,
	WIFI_CAP_HE_DOPPLER_TX,
	WIFI_CAP_HE_DOPPLER_RX,
	WIFI_CAP_HE_FULL_BW_UL_MUMIMO,
	WIFI_CAP_HE_PART_BW_UL_MUMIMO,
	WIFI_CAP_HE_SU_BFR,
	WIFI_CAP_HE_SU_BFE,
	WIFI_CAP_HE_MU_BFR,
	WIFI_CAP_HE_BFE_STS_LE_80MHZ,
	WIFI_CAP_HE_BFE_STS_GT_80MHZ,
	WIFI_CAP_HE_NG16_SU_FEEDBACK,
	WIFI_CAP_HE_NG16_MU_FEEDBACK,
	WIFI_CAP_HE_CODEBOOK_42_SU_FEEDBACK,
	WIFI_CAP_HE_CODEBOOK_75_MU_FEEDBACK,
	WIFI_CAP_HE_TRIGGERED_SU_BF_FEEDBACK,
	WIFI_CAP_HE_TRIGGERED_MU_BF_PARTIAL_BW_FEEDBACK,
	WIFI_CAP_HE_TRIGGERED_CQI_FEEDBACK,
	WIFI_CAP_HE_PARTIAL_BW_ER,
	WIFI_CAP_HE_PARTIAL_BW_DL_MUMIMO,
	WIFI_CAP_HE_PPE_THRESHOLDS_PRESENT,
	WIFI_CAP_HE_PSR_SR,
	WIFI_CAP_HE_POWER_BOOST_FACTOR,
	WIFI_CAP_HE_STBC_TX_GT_80MHZ,
	WIFI_CAP_HE_STBC_RX_GT_80MHZ,

	/* EHT flags */
	WIFI_CAP_EHT_EPCS,
	WIFI_CAP_EHT_OM_CONTROL,
	WIFI_CAP_EHT_TRIGGERED_TXOP_MODE_1,
	WIFI_CAP_EHT_TRIGGERED_TXOP_MODE_2,
	WIFI_CAP_EHT_RTWT,
	WIFI_CAP_EHT_MPDU_3895,
	WIFI_CAP_EHT_MPDU_7991,
	WIFI_CAP_EHT_MPDU_11454,
	WIFI_CAP_EHT_SCS_TDESC,
	WIFI_CAP_EHT_TRS,
	WIFI_CAP_EHT_TXOP_RETURN_IN_MODE_2,
	WIFI_CAP_EHT_TWO_BQR,
	WIFI_CAP_EHT_LINK_ADAPT,
	WIFI_CAP_EHT_320_BAND6,
	WIFI_CAP_EHT_242RU_IN_BW_GT_20MHZ,
	WIFI_CAP_EHT_NDP_WITH_4xEHT_LTF_AND_3_2_GI,
	WIFI_CAP_EHT_PARTIAL_BW_UL_MUMIMO,
	WIFI_CAP_EHT_SU_BFR,
	WIFI_CAP_EHT_SU_BFE,
	WIFI_CAP_EHT_NG16_SU_FEEDBACK,
	WIFI_CAP_EHT_NG16_MU_FEEDBACK,
	WIFI_CAP_EHT_CODEBOOK_42_SU_FEEDBACK,
	WIFI_CAP_EHT_CODEBOOK_75_MU_FEEDBACK,
	WIFI_CAP_EHT_TRIGGERED_SU_BF_FEEDBACK,
	WIFI_CAP_EHT_TRIGGERED_MU_BF_PARTIAL_BW_FEEDBACK,
	WIFI_CAP_EHT_TRIGGERED_CQI_FEEDBACK,
	WIFI_CAP_EHT_PARTIAL_BW_DL_MUMIMO,
	WIFI_CAP_EHT_PSR_SR,
	WIFI_CAP_EHT_POWER_BOOST_FACTOR,
	WIFI_CAP_EHT_MU_PPDU_WITH_4xEHT_LTF_AND_0_8_GI,
	WIFI_CAP_EHT_NON_TRIGGERED_CQI_FEEDBACK,
	WIFI_CAP_EHT_TX_1K_4K_QAM_WITH_LT_242RU,
	WIFI_CAP_EHT_RX_1K_4K_QAM_WITH_LT_242RU,
	WIFI_CAP_EHT_PPE_THRESHOLDS_PRESENT,
	WIFI_CAP_EHT_MCS15_IN_MRU,
	WIFI_CAP_EHT_MCS15_IN_MRU_80MHZ,
	WIFI_CAP_EHT_MCS15_IN_MRU_160MHZ,
	WIFI_CAP_EHT_MCS15_IN_MRU_320MHZ,
	WIFI_CAP_EHT_DUP_IN_BAND6,
	WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_80MHZ,
	WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_160MHZ,
	WIFI_CAP_EHT_NON_OFDMA_UL_MUMIMO_320MHZ,
	WIFI_CAP_EHT_MU_BFR_80MHZ,
	WIFI_CAP_EHT_MU_BFR_160MHZ,
	WIFI_CAP_EHT_MU_BFR_320MHZ,
	WIFI_CAP_EHT_TB_SOUND_FEEDBACK_RATELIMIT,
	WIFI_CAP_EHT_RX_1K_QAM_DL_OFDMA_WIDER_BW,
	WIFI_CAP_EHT_RX_4K_QAM_DL_OFDMA_WIDER_BW,
	WIFI_CAP_EHT_20MHZ_ONLY,
	WIFI_CAP_EHT_20MHZ_ONLY_TRIGGERED_MU_BF_FULL_BW_FEEDBACK,
	WIFI_CAP_EHT_20MHZ_ONLY_MRU,

	/* Custom capabilities */
	WIFI_CAP_UNASSOC_STA_MEAS,
	WIFI_CAP_UNASSOC_STA_MEAS_OFFCHANNEL,
	WIFI_CAP_STEER_LOWRSSI_STA,
	WIFI_CAP_MU_EDCA,
	WIFI_CAP_MU_RTS,
	WIFI_CAP_HE_RTS_TXOP_BASED,


	WIFI_CAP_AFTER_LAST,
	WIFI_CAP_MAX = WIFI_CAP_AFTER_LAST - 1,
};

/* Bitflags in enum wifi_capflags should be accessed through the following
 * three macros only. New bitflags may be added and/or existing ones may be
 * removed.
 *
 * Example usage -
 * a) Set WIFI_CAP_HT_TX_STBC in 'struct wifi_bss *bss' -
 *	wifi_cap_set(bss->cbitmap, WIFI_CAP_HT_TX_STBC);
 *
 * b) Print bitflags that are set in 'struct wifi_bss *bss' -
 *      wifi_for_each_cap(bss->cbitmap, flag) {
 *		if (wifi_cap_isset(bss->cbitmap, flag))
 *			printf("Capability %d is set\n", flag);
 *      }
 */

#define wifi_cap_set(b, f)	(b[(f) / 8 + 1] |= (1 << ((f) % 8)))
#define wifi_cap_isset(b, f)	(!!(b[(f) / 8 + 1] & (1 << ((f) % 8))))

#define wifi_for_each_cap(b, _iter)				\
	for (_iter = 0; _iter < WIFI_CAP_MAX; _iter++)


/** struct wifi_caps - wifi device/interface capabilities */
struct wifi_caps {
#define WIFI_CAP_BASIC_VALID	0x1
#define WIFI_CAP_EXT_VALID	0x2
#define WIFI_CAP_HT_VALID	0x4
#define WIFI_CAP_VHT_VALID	0x8
#define WIFI_CAP_RM_VALID	0x10
#define WIFI_CAP_HE_VALID	0x20
#define WIFI_CAP_EHT_VALID	0x40
	uint32_t valid;         /** bitmap of caps available and valid */
	struct wifi_caps_basic basic;
	struct wifi_caps_ext ext;
	struct wifi_caps_ht ht;
	struct wifi_caps_vht vht;
	struct wifi_caps_rm rrm;
	struct wifi_caps_he he;
	struct wifi_caps_eht eht;
};

/** struct wifi_oper_ht - HT operation element */
struct wifi_oper_ht {
	uint8_t channel;
	uint8_t info[5];
	uint8_t basic_mcs[16];
};

/** struct wifi_oper_vht - VHT operation element */
struct wifi_oper_vht {
	uint8_t channel_width;
	uint8_t freq_mid_seg0;
	uint8_t freq_mid_seg1;
	uint8_t basic_mcs[2];
};

/** struct wifi_oper_he - HE operational element */
struct wifi_oper_he {
	uint8_t param[3];
	uint8_t color;
	uint8_t basic_mcs[2];

	/* optional */
	uint8_t vht[3];
	uint8_t co_bss[1];
	uint8_t oper_6ghz[5];
};

/** struct wifi_oper_eht - EHT operational element */
struct wifi_oper_eht {
	uint8_t param;
	uint8_t basic_mcs[4];
	uint8_t info[5];
};


/** enum wifi_statusflags - wifi status bit-flags */
enum wifi_statusflags {
	WIFI_STATUS_WMM,                 /**< WMM enabled */
	WIFI_STATUS_PS,                  /**< in Power Save mode */
	WIFI_STATUS_LOCAL_OPCLASS,       /**< in local regulatory opclass */
	WIFI_STATUS_COLOR,               /**< BSS color enabled */

	WIFI_STATUS_AFTER_LAST,
	WIFI_STATUS_MAX = WIFI_STATUS_AFTER_LAST - 1,
};

#define wifi_status_set(b, f)	(b[(f) / 8 + 1] |= (1 << ((f) % 8)))
#define wifi_status_isset(b, f)	(!!(b[(f) / 8 + 1] & (1 << ((f) % 8))))

#define wifi_for_each_status(b, _iter)				\
	for (_iter = 0; _iter < WIFI_STATUS_MAX; _iter++)


/** struct wifi_iface - interface per wifi radio */
struct wifi_iface {
	char name[16];
	enum wifi_mode mode;
	enum wifi_band band;
};

/* limit max interfaces per radio */
#define WIFI_IFACE_MAX_NUM	16


/*
 * struct wifi_radio_stats - per radio statistics
 */
struct wifi_radio_stats {
	unsigned long tx_bytes; 	/**< TX bytes including framing characters */
	unsigned long rx_bytes;		/**< RX bytes including framing characters */ 
	unsigned long tx_pkts;		/**< TX packets */
	unsigned long rx_pkts;		/**< RX packets */
	uint32_t tx_err_pkts;		/**< Packets not transmitted due errors */
	uint32_t rx_err_pkts;		/**< RX not delivered to higher proto due errors */
	uint32_t tx_dropped_pkts;	/**< Packets not sent not due errors */
	uint32_t rx_dropped_pkts;	/**< RX not delivered to higher proto not due errors */
	uint32_t rx_plcp_err_pkts;	/**< RX with PLCP header error */
	uint32_t rx_fcs_err_pkts;	/**< RX with FCS error */
	uint32_t rx_mac_err_pkts;	/**< RX with bad MAC header */
	uint32_t rx_unknown_pkts;	/**< RX destined for a MAC address that is not associated with the iface */
	int noise;					/**< Noise in dBm */
	uint64_t cts_rcvd;			/**< RX CTS answers on RTS */
	uint64_t cts_not_rcvd;		/**< Not answered RTS */
	uint64_t rx_frame_err_pkts; /**< RX with good preamble but bad header */
	uint64_t rx_good_plcp_pkts;	/**< RX with good PLCP header */
	uint64_t omac_data_pkts;	/**< RX data with good FCS but addressed to a different MAC */
	uint64_t omac_mgmt_pkts;	/**< RX mgmt with good FCS but addressed to a different MAC */
	uint64_t omac_ctrl_pkts;	/**< RX ctrl with good FCS but addressed to a different MAC */
	uint64_t omac_cts;			/**< RX CTS addressed to a different MAC */
	uint64_t omac_rts;			/**< RX RTS addressed to a different MAC */
};

/** struct wifi_radio_diagnostic - radio diagnostic data */
struct wifi_radio_diagnostic {
	uint64_t channel_busy;     /** in usecs */
	uint64_t tx_airtime;       /** in usecs */
	uint64_t rx_airtime;       /** in usecs */
	uint64_t obss_airtime;     /** in usecs */
	uint64_t cca_time;         /** in usecs */
	uint64_t false_cca_count;  /** number of false detections (glitches) */
};

/*
 * struct wifi_radio - wifi radio struct
 */
struct wifi_radio {
	bool enabled;
	uint8_t tx_streams;
	uint8_t rx_streams;
	uint32_t max_bitrate;
	enum wifi_band oper_band;	/**< exactly one band from supp_band */
	uint8_t supp_band;		/**< bitmap of wifi frequency bands */
	uint8_t supp_std;		/**< bitmap of wifi_std */
	uint8_t oper_std;		/**< bitmap of wifi_std from supp_std */
	uint8_t channel;		/**< current primary (ctrl) channel */
	uint8_t supp_channels[64];
	uint8_t oper_channels[64];	/**< in use channels */
	bool acs_capable;
	bool acs_enabled;
	uint32_t acs_interval;		/**< in secs */
	uint32_t supp_bw;               /**< bitmap of wifi_bw */
	uint32_t cac_methods;		/**< bitmap of wifi_cac_method */
	enum wifi_bw curr_bw;
	enum wifi_chan_ext extch;       /**< current extension channel */
	enum wifi_guard gi;
	int8_t txpower;                 /**< -1 for auto; else in %-age */
	int8_t txpower_dbm;             /**< in dBm */
	bool dot11h_capable;
	bool dot11h_enabled;
	char regdomain[4];              /**< countrycode + "O" | "I" + NUL */
	uint8_t srl;                    /**< short retry limit */
	uint8_t lrl;                    /**< long retry limit */
	uint32_t frag;
	uint32_t rts;
	uint32_t beacon_int;            /**< in msecs */
	uint32_t dtim_period;
	bool aggr_enable;
	enum wifi_preamble pr;
	uint32_t basic_rates[32];
	uint32_t oper_rates[32];
	uint32_t supp_rates[32];
	struct wifi_radio_stats stats;
	struct wifi_radio_diagnostic diag;
	uint8_t max_iface_ap;
	uint8_t max_iface_sta;
	uint8_t num_iface;
	struct wifi_iface iface[WIFI_IFACE_MAX_NUM];
};

/* Statistics for interface common for AP and STA modes */
#define WIFI_IF_COMMON_STATS \
	uint64_t tx_bytes; 			/** TX bytes including framing characters */ \
	uint64_t rx_bytes; 			/** RX bytes including framing characters */ \
	uint64_t tx_pkts; 			/** TX packets */ \
	uint64_t rx_pkts;  			/** RX packets */ \
	uint64_t tx_err_pkts; 		/** Packets not transmitted due errors */ \
	uint64_t tx_rtx_pkts; 		/** Sum of all retransmissions of all packets  */ \
	uint64_t tx_rtx_fail_pkts; 	/** TX fail after too many retransmissions */ \
	uint64_t tx_retry_pkts; 	/** TX OK after on or more retransmissions */ \
	uint64_t tx_mretry_pkts; 	/** TX OK after more than one retransmissions */ \
	uint64_t ack_fail_pkts; 	/** Expected and not received ACK */ \
	uint64_t aggr_pkts; 		/** TX aggregated */ \
	uint64_t rx_err_pkts; 		/** RX not delivered to higher proto due errors */ \
	uint64_t tx_ucast_pkts; 	/** TX unicast */ \
	uint64_t rx_ucast_pkts; 	/** RX unicast */ \
	uint64_t tx_dropped_pkts; 	/** Packets not sent not due errors */ \
	uint64_t rx_dropped_pkts; 	/** RX not delivered to higher proto not due errors */ \
	uint64_t tx_mcast_pkts; 	/** TX multicast */ \
	uint64_t rx_mcast_pkts; 	/** RX multicast */ \
	uint64_t tx_bcast_pkts; 	/** TX broadcast*/ \
	uint64_t rx_bcast_pkts; 	/** RX broadcast */ \
	uint64_t rx_unknown_pkts; 	/** RX with unknown/unsupported proto */ \
	uint64_t tx_buf_overflow; 	/** TX dropped due buffer overflow */ \
	uint64_t tx_sta_not_assoc; 	/** TX dropped because dst STA is not associated */ \
	uint64_t tx_frags; 			/** TX fragments */ \
	uint64_t tx_no_ack_pkts; 	/** TX data packets with no ACK received */ \
	uint64_t rx_dup_pkts; 		/** RX duplicate according to Sequence Control */ \
	uint64_t tx_too_long_pkts; 	/** RX too long */ \
	uint64_t tx_too_short_pkts; /** RX too short (can't detect packet type) */ \
	uint64_t ucast_ack; 		/** Unicast ACKs with good FCS received */ 

/*
 * struct wifi_ap_stats - per-BSS statistics
 */
struct wifi_ap_stats {
	WIFI_IF_COMMON_STATS
};

/*
 * enum wifi_ap_confstatus - AP's configuration status
 */
enum wifi_ap_confstatus {
	WIFI_AP_DISABLED,
	WIFI_AP_ENABLED,
	WIFI_AP_ERR_MISCONF,
	WIFI_AP_ERROR,
};

/*
 * enum wifi_mfp_config - management frame protection config
 */
enum wifi_mfp_config {
	WIFI_MFP_DISABLED,
	WIFI_MFP_OPTIONAL,
	WIFI_MFP_REQUIRED,
};

struct wifi_rsne {
	uint16_t wpa_versions;                /**< bitmap of WPA_VERSION* */
	uint32_t group_cipher;                /**< one of WIFI_CIPHER_* */
	uint32_t pair_ciphers;                /**< bitmap of WIFI_CIPHER_* */
	uint32_t akms;                        /**< bitmap of WIFI_AKM_* */
	uint16_t rsn_caps;
};

struct wifi_ap_security {
	uint32_t supp_modes;    /** bitmap of supported WIFI_SECURITY_* */
	uint32_t curr_mode;     /** from wifi_rsnie in beacon/probe-resp */
	uint8_t wepidx;
	uint8_t wep104[WIFI_NUM_WEPKEYS][13];
	uint8_t wep40[WIFI_NUM_WEPKEYS][5];
	uint8_t psk[32];
	char passphrase[64];
	uint32_t rekey_int;
	struct ip_address radius_server[WIFI_NUM_RADIUS];
	uint32_t radius_port[WIFI_NUM_RADIUS];
	char radius_secret[WIFI_NUM_RADIUS][128];
	enum wifi_mfp_config mfp;
};

struct wifi_sta_security {
	uint32_t supp_modes;    /** bitmap of supported WIFI_SECURITY_* */
	uint32_t curr_mode;     /** from wifi_rsnie in beacon/probe-resp */
	uint32_t group_cipher;  /**< one of WIFI_CIPHER_* */
	uint32_t pair_ciphers;  /**< bitmap of WIFI_CIPHER_* */
	enum wifi_mfp_config mfp;
};

/* wps configuration state */
enum wps_state {
	WPS_DISABLED,
	WPS_UNCONFIGURED,
	WPS_CONFIGURED,
	WPS_SETUPLOCKED,
};

/*
 * enum wps_method - WPS configuration methods
 */
enum wps_method {
	WPS_METHOD_PBC,
	WPS_METHOD_PIN,
	WPS_METHOD_UNKNOWN,
};

#if 1	// redefine
/* wpa status */
enum wps_status {
	WPS_STATUS_IDLE,
	WPS_STATUS_INIT = WPS_STATUS_IDLE,
	WPS_STATUS_PROCESSING,
	WPS_STATUS_SUCCESS,
	WPS_STATUS_FAIL,
	WPS_STATUS_TIMEOUT,
	WPS_STATUS_UNKNOWN,
};
#endif

struct wifi_ap_wps {
	bool enable;
	uint32_t supp_methods;     /** bitmap of enum wps_method */
	enum wps_method en_method;
	enum wps_status status;
	uint32_t version;
	char pin[8];    /** 4 or 8 digit number only; not NUL-terminated */
};

/*
 * struct wps_device - wps device info
 */
struct wps_device {
	char name[32];
	char manufacturer[64];
	char modelname[32];
	char modelnum[32];
	char serialnum[32];
};

/*
 * struct wifi_sta_stats - per STA statistics
 */
struct wifi_sta_stats {
	uint64_t tx_bytes;
	uint64_t rx_bytes;
	uint32_t tx_pkts;
	uint32_t rx_pkts;
	uint32_t tx_err_pkts;
	uint32_t tx_rtx_pkts;
	uint32_t tx_rtx_fail_pkts;
	uint32_t tx_retry_pkts;
	uint32_t tx_mretry_pkts;
	uint32_t tx_fail_pkts;
	uint64_t rx_fail_pkts;
};

/*
 * struct wifi_sta - info of a wifi sta from AP's assoclist
 */
struct wifi_sta {
	uint8_t macaddr[6];         	/**< sta macaddress */
	uint8_t bssid[6];           	/**< bssid of connected ap */
	uint8_t sbitmap[4];         	/**< bitmap of enum wifi_statusflags */
	uint8_t cbitmap[32];        	/**< bitmap for enum wifi_capflags */
	struct wifi_caps caps;      	/**< capabilities */
	uint8_t oper_std;           	/**< bitmap of wifi_std from supp_std */
	uint32_t maxrate;           	/**< max phy operational rate in Mbps */
	struct wifi_rate rx_rate;   	/**< latest rate: from AP -> this STA */
	struct wifi_rate tx_rate;   	/**< latest rate: this STA -> AP */
	uint32_t rx_thput;          	/**< AP -> this STA instant throughput in Mbps */
	uint32_t tx_thput;          	/**< this STA -> AP instant throughput in Mbps */
	int8_t rssi_avg;                /**< average rssi */
	int8_t rssi[WIFI_NUM_ANTENNA];  /**< latest rssi in dBm per-chain */
	int8_t noise_avg;               /**< average phy noise in dBm */
	int8_t noise[WIFI_NUM_ANTENNA]; /**< latest noise in dBm */
	struct wifi_sta_stats stats;
	uint64_t tx_airtime;        	/**< Tx airtime(msecs) in the last second */
	uint64_t rx_airtime;        	/**< Rx airtime(msecs) in the last second */
	int8_t airtime;             	/**< airtime in %-age in the last second */
	uint32_t conn_time;         	/**< time in secs since connected */
	uint32_t idle_time;         	/**< inactive time in secs */
	struct wifi_sta_security sec;	/**< security */
	struct wifi_rate rate;          /**< max link rate */
	uint32_t est_rx_thput;          /**< AP -> this STA expected/estimated throughput in Mbps */
	uint32_t est_tx_thput;          /**< this STA -> AP expected/estimated throughput in Mbps */
};

/*
 * struct wifi_monsta_config - monitored sta config
 */
struct wifi_monsta_config {
	bool enable;			/**< enable/disable STA monitor */
};

/*
 * struct wifi_monsta - monitored sta info
 */
struct wifi_monsta {
	uint8_t macaddr[6];
	int8_t rssi[WIFI_NUM_ANTENNA];   /**< latest rssi in dBm */
	int8_t rssi_avg;		 /** < average rssi */
	int last_seen;			 /** < last seen in seconds */
	struct wifi_caps caps;		 /**< capabilities */
};

/*
 * struct wifi_ap_wmm_ac_stats - AP's per WMM_AC statistics
 */
struct wifi_ap_wmm_ac_stats {
	uint64_t tx_bytes;
	uint64_t rx_bytes;
	uint32_t tx_pkts;
	uint32_t rx_pkts;
	uint32_t tx_err_pkts;
	uint32_t rx_err_pkts;
	uint32_t tx_rtx_pkts;
};

/** enum wmm_ac_type - wmm access categories */
enum wmm_ac_type {
	BE,
	BK,
	VI,
	VO,
};

/*
 * struct wifi_ap_wmm_ac - AP's per WMM_AC configuration
 */
struct wifi_ap_wmm_ac {
	enum wmm_ac_type ac;
	uint8_t aifsn;
	uint8_t cwmin;
	uint8_t cwmax;
	uint8_t txop;
	bool ack_policy;
	struct wifi_ap_wmm_ac_stats stats;
};

/*
 * struct wifi_ap_accounting - accounting server info
 */
struct wifi_ap_accounting {
	bool enable;
	struct ip_address server[WIFI_NUM_RADIUS];
	uint32_t server_port[WIFI_NUM_RADIUS];
	char secret[WIFI_NUM_RADIUS][128];
	uint32_t intm_interval;
};

enum acl_policy {
	ALLOW,
	DENY,
};

struct wifi_ap_acl {
	bool acl_enabled;
	enum acl_policy policy;
	void *allowlist;           /** points to array of STA macaddress */
	void *denylist;            /** points to array of STA macaddress */
};

/** struct wifi_ap_load - Bss load */
struct wifi_ap_load {
	uint16_t sta_count;         /**< number of STAs connected */
	uint8_t utilization;        /**< channel utilization [0..255] */
	uint16_t available;         /**< available admission capacity */
} __attribute__((packed));

/*
 * struct wifi_bss - BSS information.
 * This struct is also used to store info of a scanned AP.
 */
struct wifi_bss {
	uint8_t ssid[33];
	uint8_t bssid[6];
	enum wifi_bss_mode mode;
	uint8_t channel;
	enum wifi_bw curr_bw;
	enum wifi_band band;
	uint8_t supp_std;
	uint8_t oper_std;
	int rssi;
	int noise;
	struct wifi_rsne rsn;
	uint32_t auth;		/* replacing 'sec'; will be deprecated */
	uint32_t enc;		/* deprecated; 'g_enc' is now 'rsn.group_cipher' */
	uint32_t security;      /**< bitmap of enum wifi_security */
	uint32_t beacon_int;
	uint32_t dtim_period;
	struct wifi_ap_load load;
	struct wifi_caps caps;
	uint8_t cbitmap[32];        /**< bitmap for enum wifi_capflags */
};

/*
 * struct wifi_bss_detail - details of scanned neighbor BSS
 */
struct wifi_bss_detail {
	struct wifi_bss basic;
	uint32_t ielen;
	uint8_t ie[1024];
};

/*
 * struct wifi_ap - AP structure corresponding to a BSS
 */
struct wifi_ap {
	bool enabled;
	struct wifi_bss bss;
	enum wifi_ap_confstatus confstatus;
	ifopstatus_t opstatus;
	bool ssid_advertised;
	bool wmm_cap;
	bool uapsd_cap;
	bool wmm_enabled;
	bool uapsd_enabled;
	uint32_t assoclist_max;
	bool isolate_enabled;
	struct wifi_ap_acl acl;
	struct wifi_ap_security sec;
	struct wifi_ap_wps wps;
	struct wifi_ap_accounting acct;
	struct wifi_ap_wmm_ac ac[WIFI_NUM_AC];
	struct wifi_ap_stats stats;
	uint32_t assoclist_num;
	void *assoclist;  /** list of struct wifi_sta or simply macaddresses */
};

/*
 * struct wifi_sta_ifstats - statistics for interface in STA mode
 */
struct wifi_sta_ifstats {
	WIFI_IF_COMMON_STATS
	uint32_t last_dl_rate;	/**< Recent AP->STA rate in kbps */
	uint32_t last_ul_rate;	/**< Recent STA->AP rate in kbps */
	int8_t signal;			/**< Signal average of the last 100 packets received, in dBm */
	uint8_t retrans_100;	/**< Sum of all retransmissions of the last 100 packets */
};

enum wifi_scan_state {
	SCAN_NONE,
	SCAN_REQUESTED,
	SCAN_SCANNING = SCAN_REQUESTED,
	SCAN_CANCELED,
	SCAN_COMPLETE,
	SCAN_DONE = SCAN_COMPLETE,
	SCAN_ERROR,
};

/*
 * struct wifi_neighbor - struct holding scan results of neighbor APs
 */
struct wifi_neighbor {
	char radio[16];               /** scanning wifi radio device name */
	uint32_t num_result;          /** num of scanned APs */
	struct wifi_bss *scanreslist; /** points to array of struct wifi_bss */
};

/*
 * struct wifi - structure for a 'wifi' object
 */
struct wifi {
	uint32_t num_radio;
	uint32_t num_ap;
	struct wifi_radio *radiolist; /** points to struct wifi_radio array */
	struct wifi_ap *aplist;       /** points to struct wifi_ap array */
};

/** struct scan_param - scan request parameters */
struct scan_param {
	char ssid[33];     /**< ssid specific scan */
	uint8_t bssid[6];  /**< scan bssid */
	uint32_t channel;  /**< channel to scan */
	uint32_t opclass;  /**< opclass to scan */
	uint8_t type;      /**< auto ( = 0), active (= 1), passive (=2) */
};

/* enum of wifi scan types */
enum scan_type {
	WIFI_SCAN_AUTO,
	WIFI_SCAN_ACTIVE,
	WIFI_SCAN_PASSIVE,
};

/** struct scan_param_ex - extended scan request parameters */
struct scan_param_ex {
#define WIFI_SCAN_REQ_SSID		0x1
#define WIFI_SCAN_REQ_BSSID		0x2
	uint32_t flag;
	uint8_t bssid[6];    /**< scan bssid */
#define WIFI_SCAN_MAX_SSIDS		16
	uint8_t num_ssid;    /**< number of ssids to scan */
	char ssid[WIFI_SCAN_MAX_SSIDS][33];   /**< array of ssids */
#define WIFI_SCAN_MAX_FREQ		128
	uint8_t num_freq;    /**< number of frequencies to scan */
	uint32_t freq[WIFI_SCAN_MAX_FREQ];    /**< array of frequencies */
	enum scan_type type; /**< scan type */
	bool flush;          /**< clean cfg80211 cache */
};

/** struct acs_param - auto channel sel arguments */
struct acs_param {
	// TODO
};

/** struct chan_switch_param - channel switch parameters */
struct chan_switch_param {
	int count;		/**< number of beacons before switch */
	int freq;		/**< control channel frequency */
	int bandwidth;		/**< bandwidth in MHz */
	int sec_chan_offset;	/**< for HT40+/HT40- */
	int cf1;		/**< central frequency1 */
	int cf2;		/**< central frequency2 */
	bool blocktx;		/**< blocktx during channel switch */
	bool ht;		/**< use HT */
	bool vht;		/**< use VHT */
	bool he;		/**< use HE */
};

#define OUI_LEN		3

/** struct vendor_ie - vendor ie struct */
struct vendor_ie {
	struct {
		__u8 eid;
		__u8 len;
	} ie_hdr;
	__u8 oui[OUI_LEN];
	__u8 data[];
} __attribute__ ((packed));

/** struct vendor_iereq - vendor specific ie request struct */
struct vendor_iereq {
	__u32 mgmt_subtype;  /**< bitmap of management frame subtypes */
	struct vendor_ie ie; /**< vendor ie structure */
};

/* bitmap of wps roles */
enum wps_role {
	WPS_ENROLLEE      = 1<<0,
	WPS_PROXY         = 1<<1,
	WPS_REGISTRAR     = 1<<2,
	WPS_ENROLLEE_BSTA = 1<<3,
	WPS_ROLE_MAX      = 1<<4,
};

/**
 * struct wps_param - WPS parameter to be used during registration
 * @role: enrollee, registrar or proxy. Bitmap of %WPS_ENROLLEE,
 *	%WPS_REGISTRAR, %WPS_PROXY etc.
 * @method: WPS configuration method, i.e. one of enum wps_method
 * @pin: pin value if wps_method 'PIN' is used for registration
 */
struct wps_param {
	enum wps_role role;		/**< bitmap of wps_role */
	enum wps_method method;		/**< bitmap of wps_method */
	union {
		unsigned long pin;	/**< pin value for PIN method */
	};
};

/* struct vlan_param */
struct vlan_param {
	uint8_t dir;	/* 0 = ingress, 1 = egress */
	uint8_t pcp;	/* priority */
	uint16_t vid;	/* vlan id */
};

/* 11r related structs and defines */

#define FT_R0KH_ID_MAX_LEN	48
#define FT_R1KH_ID_LEN		6
#define WPA_PMK_NAME_LEN	16
#define PMK_LEN			32

struct fbt_keys {
	uint8_t ap_address[6];                /** bssid */
	uint8_t r1kh_id[FT_R1KH_ID_LEN];
	uint8_t s1kh_id[6];                   /** mac address of sta */
	uint8_t pmk_r0_name[WPA_PMK_NAME_LEN];
	uint8_t pmk_r1[PMK_LEN];
	uint8_t pmk_r1_name[WPA_PMK_NAME_LEN];
	uint8_t r0kh_id[FT_R0KH_ID_MAX_LEN];
	uint8_t r0kh_id_len;
	uint16_t pairwise;
} __attribute__((packed));


/* 11k neighbor BSS related structs and defines */

/** struct nbr_header - meta data for 'struct nbr' */
struct nbr_header {
#define NBR_F_EXCLUDE	0x1  /** exclude this neighbor */
#define NBR_F_USER	0x2  /** user added entry */
	uint32_t flags;
} __attribute__((packed));

/** enum nbr_bssinfo - 802.11k neighbor bssid info bitflags */
enum nbr_bssinfo {
	NBR_BSSINFO_REACHABLE = 1<<0,
	NBR_BSSINFO_SECURITY  = 1<<1,
	NBR_BSSINFO_QOS       = 1<<2,
	NBR_BSSINFO_RRM       = 1<<3,
	NBR_BSSINFO_HT        = 1<<4,
	NBR_BSSINFO_VHT       = 1<<5,
};

/*
 * struct nbr - 802.11k neighbor BSS
 */
struct nbr {
	uint8_t bssid[6];    /**< Bssid */
	uint32_t bssid_info; /**< as in IEEE 802.11-2016 9.4.2.37 */
	uint8_t reg;         /**< regulatory region */
	uint8_t channel;     /**< channel */
	uint8_t phy;         /**< of enum wifi_phytype */
	/* ... optional subelms */
} __attribute__((packed));

/*
 * struct sta_nbr - 802.11k neighbor BSS based on beacon report from STA
 */
struct sta_nbr {
	uint8_t bssid[6];
	int8_t rssi;
	int8_t rsni;
} __attribute__((packed));

/** Optional subelement IDS for Beacon request */
enum {
	WIFI_BCNREQ_SSID = 0,
	WIFI_BCNREQ_BCN_REPORTING = 1,
	WIFI_BCNREQ_REP_DETAIL = 2,
	WIFI_BCNREQ_REQUEST = 10,
	WIFI_BCNREQ_EXT_REQUEST	= 11,
	WIFI_BCNREQ_AP_CHAN_REPORT = 51,
	WIFI_BCNREQ_WB_CHAN_SWITCH = 163,
	WIFI_BCNREQ_VENDOR_SPECIFIC = 221,
};

/*
 * Measurement Mode definitions for Beacon request
 */
enum wifi_bcnreq_mode {
	WIFI_BCNREQ_MODE_PASSIVE = 0,
	WIFI_BCNREQ_MODE_ACTIVE = 1,
	WIFI_BCNREQ_MODE_TABLE = 2,
	WIFI_BCNREQ_MODE_UNSET = 255,
};

/*
 * struct wifi_beacon_req - 11k beacon request
 */
struct wifi_beacon_req {
	uint8_t oper_class;		/**< Operating Class */
	uint8_t channel;		/**< Channel Number */
	uint16_t rand_interval;		/**< Randomization Interval (in TUs) */
	uint16_t duration;		/**< Measurement Duration (in TUs) */
	uint8_t mode;			/**< Measurement Mode */
	uint8_t bssid[6];		/**< BSSID */
	uint8_t variable[0];		/**< Optional Subelements */
} __attribute__((packed));

/** BTM Request mode */
enum {
	WIFI_BTMREQ_PREF_INC     = 1 << 0,
	WIFI_BTMREQ_ABRIDGED     = 1 << 1,
	WIFI_BTMREQ_DISASSOC_IMM = 1 << 2,
	WIFI_BTMREQ_BSSTERM_INC  = 1 << 3,
	WIFI_BTMREQ_ESSTERM_IMM  = 1 << 4,
};

/*
 * struct wifi_btmreq_mbo
 */
struct wifi_btmreq_mbo {
	bool valid;			/**< mbo params valid */
	unsigned int reason;		/**< reason */
	unsigned int cell_pref;		/**< cell prefered - valid 0, 1, 255 */
	unsigned int reassoc_delay;	/**< reassoc delay - only valid with disassoc imminent */
};

/*
 * struct wifi_btmreq - 11v BTM Request
 */
struct wifi_btmreq {
	uint8_t mode;			/**< bitmap of WIFI_BTMREQ_* */
	uint16_t disassoc_tmo;		/**< in tbtts when DISASSOC_IMM is set */
	uint8_t validity_int;		/**< in tbtts until candidate list is valid */
	uint16_t bssterm_dur;		/**< bss termination duration in minutes */
	struct wifi_btmreq_mbo mbo;	/**< mbo parameters */
#define BTMREQ_F_NBR_PARAM_SET 0x1 /** TODO: remove dependencies and remove */
	uint32_t flags;			/**< bit flags */
};

enum wifi_mbo_disallow_assoc_reason {
	WIFI_MBO_ALLOW_ASSOC = 0,
        WIFI_MBO_DISALLOW_ASSOC_REASON_UNSPECIFIED = 1,
        WIFI_MBO_DISALLOW_ASSOC_REASON_MAX_STA = 2,
        WIFI_MBO_DISALLOW_ASSOC_REASON_AIR_INTERFERENCE = 3,
        WIFI_MBO_DISALLOW_ASSOC_REASON_AUTH_SERVER_OVERLOAD = 4,
        WIFI_MBO_DISALLOW_ASSOC_REASON_LOW_RSSI = 5,
};

#define WIFI_FRAME_MGMT               0
#define WIFI_FRAME_CTRL               1
#define WIFI_FRAME_DATA               2

/* management */
#define WIFI_FRAME_ASSOC_REQ         0
#define WIFI_FRAME_ASSOC_RESP        1
#define WIFI_FRAME_REASSOC_REQ       2
#define WIFI_FRAME_REASSOC_RESP      3
#define WIFI_FRAME_PROBE_REQ         4
#define WIFI_FRAME_PROBE_RESP        5
#define WIFI_FRAME_BEACON            8
#define WIFI_FRAME_DISASSOC          10
#define WIFI_FRAME_AUTH              11
#define WIFI_FRAME_DEAUTH            12
#define WIFI_FRAME_ACTION            13

/* information elements id's */
#define IE_SSID                      0
#define IE_SUPP_RATES                1
#define IE_DS_PARAM                  3
#define IE_BSS_LOAD                  11
#define IE_HT_CAP                    45
#define IE_RSN                       48
#define IE_MDE                       54
#define IE_HT_OPER                   61
#define IE_RRM                       70
#define IE_EXT_CAP                   127
#define IE_VHT_CAP                   191
#define IE_VEND_SPEC                 221
#define IE_EXT                       255

/* elements id extension */
#define IE_EXT_HE_CAP                35
#define IE_EXT_HE_OPER               36
#define IE_EXT_MU_EDCA               38
#define IE_EXT_MULTI_BSSID           55
#define IE_EXT_6G_CAPS               59
#define IE_EXT_EHT_OPER              106
#define IE_EXT_ML                    107
#define IE_EXT_EHT_CAP               108

struct radio_entry {
	char name[16];
};

struct iface_entry {
	char name[16];
	enum wifi_mode mode;
};

/** enum dfs_state - DFS state */
enum dfs_state {
	WIFI_DFS_STATE_NONE,           /**< CAC not required */
	WIFI_DFS_STATE_USABLE,         /**< CAC required */
	WIFI_DFS_STATE_CAC,            /**< CAC ongoing */
	WIFI_DFS_STATE_UNAVAILABLE,    /**< NOP - radar detected */
	WIFI_DFS_STATE_AVAILABLE,      /**< CAC completed */
};

/*
 * struct chan_entry - describe 20MHz channel
 */
struct chan_entry {
	uint32_t channel;			/**< channel number */
	uint32_t ctrl_channels[32];		/**< control channels */
	enum wifi_band band;			/**< band */
	uint32_t freq;				/**< frequency */
	int noise;				/**< noise floor in dBm */

	/* DFS params */
	bool dfs;				/**< is radar detection required */
	enum dfs_state dfs_state;		/**< current state of DFS channel */
	uint32_t cac_time;			/**< required CAC time in seconds */
	uint32_t nop_time;			/**< left NOP time in seconds */

	uint8_t score;				/**< score 0-100, 0 - least preferred, 255 - invalid value */
	uint8_t busy;				/**< busy 0-100%, 255 - invalid value, for opclass cover also bandwidth */
	uint8_t bss_num;			/**< number of other BSSes, for opclass cover bandwidth and adjacent channels */

	/* Survey params - for bandwidth = 20MHz */
	struct wifi_radio_diagnostic survey;	/**< servey data */
};

/* max number of opclass per regulatory domain */
#define WIFI_NUM_OPCLASS_IN_REGDOMAIN	16

/* max number of channels per operating class */
#define WIFI_NUM_CHANNEL_IN_OPCLASS	64

/** struct wifi_opchannel - channel definition in operating class */
struct wifi_opchannel {
	int8_t txpower;               /**< max txpower in dBm */
	uint8_t num;
	struct chan_entry ch[WIFI_NUM_CHANNEL_IN_OPCLASS];
};

/** struct wifi_opclass - operating class */
struct wifi_opclass {
	uint32_t opclass;
	uint32_t g_opclass;
	enum wifi_band band;
	enum wifi_bw bw;
	enum wifi_chan_ext ext;
	struct wifi_opchannel opchannel;
};

/** struct wifi_mlo link - MLO link */
struct wifi_mlo_link {
	uint32_t id;			/**< MLO link id */
	uint8_t macaddr[6];         	/**< MLO link macaddress */
	enum wifi_band band;		/**< MLO link band */
	uint32_t channel;		/**< MLO link channel */
	enum wifi_bw bandwidth;		/**< MLO link bandwidth */
};

/** Get valid channels for given country, band and bandwidth */
int wifi_get_valid_channels(const char *ifname, enum wifi_band b,
					enum wifi_bw bw, const char *cc,
					uint32_t *chlist, int *n);


/** Get list of channels for a global opclass */
int wifi_opclass_to_channels(uint32_t opclass, int *num, uint32_t *channels);

/** Get list of operating classes supported by the wifi radio */
int wifi_get_supported_opclass(const char *name, int *num_opclass,
						struct wifi_opclass *o);

int wifi_get_band_supported_opclass(const char *name, enum wifi_band band,
				    int *num_opclass, struct wifi_opclass *o);

/** Check if given channel is DFS */
bool wifi_is_dfs_channel(const char *name, int channel, int bandwidth);

/** Generate a random wps pin */
int wifi_generate_wps_pin(unsigned long *pin);

/** Check if a wps pin is valid. Return 1 if valid; else 0 */
int wifi_is_wps_pin_valid(unsigned long pin);

/** Helper functions to convert frequency to/from channel number */
int wifi_channel_to_freq(int chan);
int wifi_channel_to_freq_ex(int chan, enum wifi_band band);
int wifi_freq_to_channel(int freq);

const int *chan2list(int chan, int bw);

/** Iterator for information elements */
#define wifi_foreach_ie(e, _iebuf, _len)				\
	for ((e) = (_iebuf);						\
		(_iebuf) + (_len) - (e) - 2 - (e)[1] >= 0;		\
		(e) += 2 + (e)[1])



#ifdef __cplusplus
}
#endif

#endif /* WIFIDEFS_H */
