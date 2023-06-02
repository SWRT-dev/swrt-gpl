/*
 * Custom OID/ioctl definitions for
 * Broadcom 802.11abg Networking Device Driver
 *
 * Definitions subject to change without notice.
 *
 * Copyright 2006, Broadcom Corporation
 * All Rights Reserved.
 *
 * THIS SOFTWARE IS OFFERED "AS IS", AND BROADCOM GRANTS NO WARRANTIES OF ANY
 * KIND, EXPRESS OR IMPLIED, BY STATUTE, COMMUNICATION OR OTHERWISE. BROADCOM
 * SPECIFICALLY DISCLAIMS ANY IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS
 * FOR A SPECIFIC PURPOSE OR NONINFRINGEMENT CONCERNING THIS SOFTWARE.
 *
 */

#ifndef _IOPSYS_QUESTD_BROADCOM_H
#define _IOPSYS_QUESTD_BROADCOM_H

/* Reverse the bytes in a 16-bit value */
#define BCMSWAP16(val) \
	((uint16)((((uint16)(val) & (uint16)0x00ffU) << 8) | \
		  (((uint16)(val) & (uint16)0xff00U) >> 8)))

/* Reverse the bytes in a 32-bit value */
#define BCMSWAP32(val) \
	((uint32)((((uint32)(val) & (uint32)0x000000ffU) << 24) | \
		  (((uint32)(val) & (uint32)0x0000ff00U) <<  8) | \
		  (((uint32)(val) & (uint32)0x00ff0000U) >>  8) | \
		  (((uint32)(val) & (uint32)0xff000000U) >> 24)))

/* Reverse the bytes in a 64-bit value */
#define BCMSWAP64(val) \
	((uint64)((((uint64)(val) & 0x00000000000000ffULL) << 56) | \
	          (((uint64)(val) & 0x000000000000ff00ULL) << 40) | \
	          (((uint64)(val) & 0x0000000000ff0000ULL) << 24) | \
	          (((uint64)(val) & 0x00000000ff000000ULL) <<  8) | \
	          (((uint64)(val) & 0x000000ff00000000ULL) >>  8) | \
	          (((uint64)(val) & 0x0000ff0000000000ULL) >> 24) | \
	          (((uint64)(val) & 0x00ff000000000000ULL) >> 40) | \
	          (((uint64)(val) & 0xff00000000000000ULL) >> 56)))


#define SSID_FMT_BUF_LEN (4*32+1)	/* Length for SSID format string */
#define	LEGACY_WL_BSS_INFO_VERSION	107
#define	LEGACY2_WL_BSS_INFO_VERSION	108
#define	WL_BSS_INFO_VERSION	109	/* current version of wl_bss_info struct */
#define MCSSET_LEN	16	/* 16-bits per 8-bit set to give 128-bits bitmap of MCS Index */
#define WL_STA_ANT_MAX		4	/**< max possible rx antennas */
#define WL_STA_VER		4
#define WL_NUMCHANNELS		64
#ifndef ETHER_ADDR_LEN
#define	ETHER_ADDR_LEN		6
#endif
#define WLC_IOCTL_MEDLEN	1536    /* "med" length ioctl buffer required */

#define WL_STA_AID(a)		((a) &~ 0xc000)

/* WPA authentication mode bitvec */
#define WPA_AUTH_DISABLED	0x0000	/* Legacy (i.e., non-WPA) */
#define WPA_AUTH_NONE		0x0001	/* none (IBSS) */
#define WPA_AUTH_UNSPECIFIED	0x0002	/* over 802.1x */
#define WPA_AUTH_PSK		0x0004	/* Pre-shared key */
/* #define WPA_AUTH_8021X 0x0020 */	/* 802.1x, reserved */
#define WPA2_AUTH_UNSPECIFIED	0x0040	/* over 802.1x */
#define WPA2_AUTH_PSK		0x0080	/* Pre-shared key */
#define BRCM_AUTH_PSK           0x0100  /* BRCM specific PSK */
#define BRCM_AUTH_DPT		0x0200	/* DPT PSK without group keys */
#define WPA2_AUTH_1X_SHA256	0x1000  /* 1X with SHA256 key derivation */
#define WPA2_AUTH_TPK		0x2000	/* TDLS Peer Key */
#define WPA2_AUTH_FT		0x4000	/* Fast Transition. */
#define WPA2_AUTH_PSK_SHA256	0x8000	/* PSK with SHA256 key derivation */

/* Flags for sta_info_t indicating properties of STA */
#define WL_STA_BRCM		0x00000001	/* Running a Broadcom driver */
#define WL_STA_WME		0x00000002	/* WMM association */
#define WL_STA_NONERP		0x00000004	/* No ERP */
#define WL_STA_AUTHE		0x00000008	/* Authenticated */
#define WL_STA_ASSOC		0x00000010	/* Associated */
#define WL_STA_AUTHO		0x00000020	/* Authorized */
#define WL_STA_WDS		0x00000040	/* Wireless Distribution System */
#define WL_STA_WDS_LINKUP	0x00000080	/* WDS traffic/probes flowing properly */
#define WL_STA_PS		0x00000100	/* STA is in power save mode from AP's viewpoint */
#define WL_STA_APSD_BE		0x00000200	/* APSD delv/trigger for AC_BE is default enabled */
#define WL_STA_APSD_BK		0x00000400	/* APSD delv/trigger for AC_BK is default enabled */
#define WL_STA_APSD_VI		0x00000800	/* APSD delv/trigger for AC_VI is default enabled */
#define WL_STA_APSD_VO		0x00001000	/* APSD delv/trigger for AC_VO is default enabled */
#define WL_STA_N_CAP		0x00002000	/* STA 802.11n capable */
#define WL_STA_SCBSTATS		0x00004000	/* Per STA debug stats */
#define WL_STA_AMPDU_CAP	0x00008000	/* STA AMPDU capable */
#define WL_STA_AMSDU_CAP	0x00010000	/* STA AMSDU capable */
#define WL_STA_MIMO_PS		0x00020000	/* mimo ps mode is enabled */
#define WL_STA_MIMO_RTS		0x00040000	/* send rts in mimo ps mode */
#define WL_STA_RIFS_CAP		0x00080000	/* rifs enabled */
#define WL_STA_VHT_CAP		0x00100000	/* STA VHT(11ac) capable */
#define WL_STA_WPS		0x00200000	/* WPS state */
#define WL_STA_DWDS_CAP		0x01000000	/* DWDS CAP */
#define WL_STA_DWDS		0x02000000	/* DWDS active */

#define WL_WDS_LINKUP		WL_STA_WDS_LINKUP	/* deprecated */

/* STA HT cap fields */
#define WL_STA_CAP_LDPC_CODING		0x0001	/* Support for rx of LDPC coded pkts */
#define WL_STA_CAP_40MHZ		0x0002  /* FALSE:20Mhz, TRUE:20/40MHZ supported */
#define WL_STA_CAP_MIMO_PS_MASK		0x000C  /* Mimo PS mask */
#define WL_STA_CAP_MIMO_PS_SHIFT	0x0002	/* Mimo PS shift */
#define WL_STA_CAP_MIMO_PS_OFF		0x0003	/* Mimo PS, no restriction */
#define WL_STA_CAP_MIMO_PS_RTS		0x0001	/* Mimo PS, send RTS/CTS around MIMO frames */
#define WL_STA_CAP_MIMO_PS_ON		0x0000	/* Mimo PS, MIMO disallowed */
#define WL_STA_CAP_GF			0x0010	/* Greenfield preamble support */
#define WL_STA_CAP_SHORT_GI_20		0x0020	/* 20MHZ short guard interval support */
#define WL_STA_CAP_SHORT_GI_40		0x0040	/* 40Mhz short guard interval support */
#define WL_STA_CAP_TX_STBC		0x0080	/* Tx STBC support */
#define WL_STA_CAP_RX_STBC_MASK		0x0300	/* Rx STBC mask */
#define WL_STA_CAP_RX_STBC_SHIFT	8	/* Rx STBC shift */
#define WL_STA_CAP_DELAYED_BA		0x0400	/* delayed BA support */
#define WL_STA_CAP_MAX_AMSDU		0x0800	/* Max AMSDU size in bytes , 0=3839, 1=7935 */
#define WL_STA_CAP_DSSS_CCK		0x1000	/* DSSS/CCK supported by the BSS */
#define WL_STA_CAP_PSMP			0x2000	/* Power Save Multi Poll support */
#define WL_STA_CAP_40MHZ_INTOLERANT	0x4000	/* 40MHz Intolerant */
#define WL_STA_CAP_LSIG_TXOP		0x8000	/* L-SIG TXOP protection support */

#define WL_STA_CAP_RX_STBC_NO		0x0	/* no rx STBC support */
#define WL_STA_CAP_RX_STBC_ONE_STREAM	0x1	/* rx STBC support of 1 spatial stream */
#define WL_STA_CAP_RX_STBC_TWO_STREAM	0x2	/* rx STBC support of 1-2 spatial streams */
#define WL_STA_CAP_RX_STBC_THREE_STREAM	0x3	/* rx STBC support of 1-3 spatial streams */

/* scb vht flags */
#define WL_STA_VHT_LDPCCAP	0x0001
#define WL_STA_SGI80		0x0002
#define WL_STA_SGI160		0x0004
#define WL_STA_VHT_TX_STBCCAP	0x0008
#define WL_STA_VHT_RX_STBCCAP	0x0010
#define WL_STA_SU_BEAMFORMER	0x0020
#define WL_STA_SU_BEAMFORMEE	0x0040
#define WL_STA_MU_BEAMFORMER	0x0080
#define WL_STA_MU_BEAMFORMEE	0x0100
#define WL_STA_VHT_TXOP_PS	0x0200
#define WL_STA_HTC_VHT_CAP	0x0400

/* include "typedefs.h" */
typedef unsigned char	uchar;
typedef unsigned short	ushort;
typedef unsigned int	uint;
typedef unsigned long	ulong;
typedef unsigned char	uint8;
typedef unsigned short	uint16;
typedef unsigned int	uint32;
typedef unsigned long long uint64;
typedef unsigned int	uintptr;
typedef signed char	int8;
typedef signed short	int16;
typedef signed int	int32;
typedef signed long long int64;
typedef float		float32;
typedef double		float64;

#include "bcmwifi_channels.h"
//#include <libubox/blobmsg.h>

#define WL_MCSSET_LEN				16
#define WL_MAX_STA_COUNT			32

#define WLC_GET_PASSIVE				48

#define WL_BSS_RSSI_OFFSET			82
#define WL_BSS_NOISE_OFFSET			84

#define WLC_IOCTL_MAGIC				0x14e46c77
#define	WLC_IOCTL_MAXLEN			8192

#define WLC_CNTRY_BUF_SZ        		4

/* common ioctl definitions */
#define WLC_GET_MAGIC				0
#define WLC_GET_VERSION				1
#define WLC_UP					2
#define WLC_DOWN				3
#define WLC_GET_LOOP				4
#define WLC_SET_LOOP				5
#define WLC_DUMP				6
#define WLC_GET_MSGLEVEL			7
#define WLC_SET_MSGLEVEL			8
#define WLC_GET_PROMISC				9
#define WLC_SET_PROMISC				10
/* #define WLC_DUMP_RSSI			11 */ /* no longer supported */
#define WLC_GET_RATE				12
/* #define WLC_SET_RATE				13 */ /* no longer supported */
#define WLC_GET_INSTANCE			14
/* #define WLC_GET_FRAG				15 */ /* no longer supported */
/* #define WLC_SET_FRAG				16 */ /* no longer supported */
/* #define WLC_GET_RTS				17 */ /* no longer supported */
/* #define WLC_SET_RTS				18 */ /* no longer supported */
#define WLC_GET_INFRA				19
#define WLC_SET_INFRA				20
#define WLC_GET_AUTH				21
#define WLC_SET_AUTH				22
#define WLC_GET_BSSID				23
#define WLC_SET_BSSID				24
#define WLC_GET_SSID				25
#define WLC_SET_SSID				26
#define WLC_RESTART				27
/* #define WLC_DUMP_SCB				28 */ /* no longer supported */
#define WLC_GET_CHANNEL				29
#define WLC_SET_CHANNEL				30
#define WLC_GET_SRL				31
#define WLC_SET_SRL				32
#define WLC_GET_LRL				33
#define WLC_SET_LRL				34
#define WLC_GET_PLCPHDR				35
#define WLC_SET_PLCPHDR				36
#define WLC_GET_RADIO				37
#define WLC_SET_RADIO				38
#define WLC_GET_PHYTYPE				39
#define WLC_DUMP_RATE				40
#define WLC_SET_RATE_PARAMS			41
#define WLC_GET_FIXRATE				42
#define WLC_SET_FIXRATE				43
/* #define WLC_GET_WEP				42 */ /* no longer supported */
/* #define WLC_SET_WEP				43 */ /* no longer supported */
#define WLC_GET_KEY				44
#define WLC_SET_KEY				45
#define WLC_GET_REGULATORY			46
#define WLC_SET_REGULATORY			47
#define WLC_GET_PASSIVE_SCAN			48
#define WLC_SET_PASSIVE_SCAN			49
#define WLC_SCAN				50
#define WLC_SCAN_RESULTS			51
#define WLC_DISASSOC				52
#define WLC_REASSOC				53
#define WLC_GET_ROAM_TRIGGER			54
#define WLC_SET_ROAM_TRIGGER			55
#define WLC_GET_ROAM_DELTA			56
#define WLC_SET_ROAM_DELTA			57
#define WLC_GET_ROAM_SCAN_PERIOD		58
#define WLC_SET_ROAM_SCAN_PERIOD		59
#define WLC_EVM					60	/* diag */
#define WLC_GET_TXANT				61
#define WLC_SET_TXANT				62
#define WLC_GET_ANTDIV				63
#define WLC_SET_ANTDIV				64
/* #define WLC_GET_TXPWR			65 */ /* no longer supported */
/* #define WLC_SET_TXPWR			66 */ /* no longer supported */
#define WLC_GET_CLOSED				67
#define WLC_SET_CLOSED				68
#define WLC_GET_MACLIST				69
#define WLC_SET_MACLIST				70
#define WLC_GET_RATESET				71
#define WLC_SET_RATESET				72
/* #define WLC_GET_LOCALE			73 */ /* no longer supported */
#define WLC_LONGTRAIN				74
#define WLC_GET_BCNPRD				75
#define WLC_SET_BCNPRD				76
#define WLC_GET_DTIMPRD				77
#define WLC_SET_DTIMPRD				78
#define WLC_GET_SROM				79
#define WLC_SET_SROM				80
#define WLC_GET_WEP_RESTRICT			81
#define WLC_SET_WEP_RESTRICT			82
#define WLC_GET_COUNTRY				83
#define WLC_SET_COUNTRY				84
#define WLC_GET_PM				85
#define WLC_SET_PM				86
#define WLC_GET_WAKE				87
#define WLC_SET_WAKE				88
/* #define WLC_GET_D11CNTS			89 */ /* -> "counters" iovar */
#define WLC_GET_FORCELINK			90	/* ndis only */
#define WLC_SET_FORCELINK			91	/* ndis only */
#define WLC_FREQ_ACCURACY			92	/* diag */
#define WLC_CARRIER_SUPPRESS			93	/* diag */
#define WLC_GET_PHYREG				94
#define WLC_SET_PHYREG				95
#define WLC_GET_RADIOREG			96
#define WLC_SET_RADIOREG			97
#define WLC_GET_REVINFO				98
#define WLC_GET_UCANTDIV			99
#define WLC_SET_UCANTDIV			100
#define WLC_R_REG				101
#define WLC_W_REG				102
/* #define WLC_DIAG_LOOPBACK			103	old tray diag */
/* #define WLC_RESET_D11CNTS			104 */ /* -> "reset_d11cnts" iovar */
#define WLC_GET_MACMODE				105
#define WLC_SET_MACMODE				106
#define WLC_GET_MONITOR				107
#define WLC_SET_MONITOR				108
#define WLC_GET_GMODE				109
#define WLC_SET_GMODE				110
#define WLC_GET_LEGACY_ERP			111
#define WLC_SET_LEGACY_ERP			112
#define WLC_GET_RX_ANT				113
#define WLC_GET_CURR_RATESET			114	/* current rateset */
#define WLC_GET_SCANSUPPRESS			115
#define WLC_SET_SCANSUPPRESS			116
#define WLC_GET_AP				117
#define WLC_SET_AP				118
#define WLC_GET_EAP_RESTRICT			119
#define WLC_SET_EAP_RESTRICT			120
#define WLC_SCB_AUTHORIZE			121
#define WLC_SCB_DEAUTHORIZE			122
#define WLC_GET_WDSLIST				123
#define WLC_SET_WDSLIST				124
#define WLC_GET_ATIM				125
#define WLC_SET_ATIM				126
#define WLC_GET_RSSI				127
#define WLC_GET_PHYANTDIV			128
#define WLC_SET_PHYANTDIV			129
#define WLC_AP_RX_ONLY				130
#define WLC_GET_TX_PATH_PWR			131
#define WLC_SET_TX_PATH_PWR			132
#define WLC_GET_WSEC				133
#define WLC_SET_WSEC				134
#define WLC_GET_PHY_NOISE			135
#define WLC_GET_BSS_INFO			136
#define WLC_GET_PKTCNTS				137
#define WLC_GET_LAZYWDS				138
#define WLC_SET_LAZYWDS				139
#define WLC_GET_BANDLIST			140
#define WLC_GET_BAND				141
#define WLC_SET_BAND				142
#define WLC_SCB_DEAUTHENTICATE			143
#define WLC_GET_SHORTSLOT			144
#define WLC_GET_SHORTSLOT_OVERRIDE		145
#define WLC_SET_SHORTSLOT_OVERRIDE		146
#define WLC_GET_SHORTSLOT_RESTRICT		147
#define WLC_SET_SHORTSLOT_RESTRICT		148
#define WLC_GET_GMODE_PROTECTION		149
#define WLC_GET_GMODE_PROTECTION_OVERRIDE	150
#define WLC_SET_GMODE_PROTECTION_OVERRIDE	151
#define WLC_UPGRADE				152
/* #define WLC_GET_MRATE			153 */ /* no longer supported */
/* #define WLC_SET_MRATE			154 */ /* no longer supported */
#define WLC_GET_IGNORE_BCNS			155
#define WLC_SET_IGNORE_BCNS			156
#define WLC_GET_SCB_TIMEOUT			157
#define WLC_SET_SCB_TIMEOUT			158
#define WLC_GET_ASSOCLIST			159
#define WLC_GET_CLK				160
#define WLC_SET_CLK				161
#define WLC_GET_UP				162
#define WLC_OUT					163
#define WLC_GET_WPA_AUTH			164
#define WLC_SET_WPA_AUTH			165
#define WLC_GET_UCFLAGS				166
#define WLC_SET_UCFLAGS				167
#define WLC_GET_PWRIDX				168
#define WLC_SET_PWRIDX				169
#define WLC_GET_TSSI				170
#define WLC_GET_SUP_RATESET_OVERRIDE		171
#define WLC_SET_SUP_RATESET_OVERRIDE		172
/* #define WLC_SET_FAST_TIMER			173 */ /* no longer supported */
/* #define WLC_GET_FAST_TIMER			174 */ /* no longer supported */
/* #define WLC_SET_SLOW_TIMER			175 */ /* no longer supported */
/* #define WLC_GET_SLOW_TIMER			176 */ /* no longer supported */
/* #define WLC_DUMP_PHYREGS			177 */ /* no longer supported */
#define WLC_GET_PROTECTION_CONTROL		178
#define WLC_SET_PROTECTION_CONTROL		179
#define WLC_GET_PHYLIST				180
#define WLC_ENCRYPT_STRENGTH			181	/* ndis only */
#define WLC_DECRYPT_STATUS			182	/* ndis only */
#define WLC_GET_KEY_SEQ				183
#define WLC_GET_SCAN_CHANNEL_TIME		184
#define WLC_SET_SCAN_CHANNEL_TIME		185
#define WLC_GET_SCAN_UNASSOC_TIME		186
#define WLC_SET_SCAN_UNASSOC_TIME		187
#define WLC_GET_SCAN_HOME_TIME			188
#define WLC_SET_SCAN_HOME_TIME			189
#define WLC_GET_SCAN_NPROBES			190
#define WLC_SET_SCAN_NPROBES			191
#define WLC_GET_PRB_RESP_TIMEOUT		192
#define WLC_SET_PRB_RESP_TIMEOUT		193
#define WLC_GET_ATTEN				194
#define WLC_SET_ATTEN				195
#define WLC_GET_SHMEM				196	/* diag */
#define WLC_SET_SHMEM				197	/* diag */
/* #define WLC_GET_GMODE_PROTECTION_CTS		198 */ /* no longer supported */
/* #define WLC_SET_GMODE_PROTECTION_CTS		199 */ /* no longer supported */
#define WLC_SET_WSEC_TEST			200
#define WLC_SCB_DEAUTHENTICATE_FOR_REASON	201
#define WLC_TKIP_COUNTERMEASURES		202
#define WLC_GET_PIOMODE				203
#define WLC_SET_PIOMODE				204
#define WLC_SET_ASSOC_PREFER			205
#define WLC_GET_ASSOC_PREFER			206
#define WLC_SET_ROAM_PREFER			207
#define WLC_GET_ROAM_PREFER			208
#define WLC_SET_LED				209
#define WLC_GET_LED				210
#define WLC_GET_INTERFERENCE_MODE		211
#define WLC_SET_INTERFERENCE_MODE		212
#define WLC_GET_CHANNEL_QA			213
#define WLC_START_CHANNEL_QA			214
#define WLC_GET_CHANNEL_SEL			215
#define WLC_START_CHANNEL_SEL			216
#define WLC_GET_VALID_CHANNELS			217
#define WLC_GET_FAKEFRAG			218
#define WLC_SET_FAKEFRAG			219
#define WLC_GET_PWROUT_PERCENTAGE		220
#define WLC_SET_PWROUT_PERCENTAGE		221
#define WLC_SET_BAD_FRAME_PREEMPT		222
#define WLC_GET_BAD_FRAME_PREEMPT		223
#define WLC_SET_LEAP_LIST			224
#define WLC_GET_LEAP_LIST			225
#define WLC_GET_CWMIN				226
#define WLC_SET_CWMIN				227
#define WLC_GET_CWMAX				228
#define WLC_SET_CWMAX				229
#define WLC_GET_WET				230
#define WLC_SET_WET				231
#define WLC_GET_PUB				232
/* #define WLC_SET_GLACIAL_TIMER		233 */ /* no longer supported */
/* #define WLC_GET_GLACIAL_TIMER		234 */ /* no longer supported */
#define WLC_GET_KEY_PRIMARY			235
#define WLC_SET_KEY_PRIMARY			236
/* #define WLC_DUMP_RADIOREGS			237 */ /* no longer supported */
#define WLC_GET_ACI_ARGS			238
#define WLC_SET_ACI_ARGS			239
#define WLC_UNSET_CALLBACK			240
#define WLC_SET_CALLBACK			241
#define WLC_GET_RADAR				242
#define WLC_SET_RADAR				243
#define WLC_SET_SPECT_MANAGMENT			244
#define WLC_GET_SPECT_MANAGMENT			245
#define WLC_WDS_GET_REMOTE_HWADDR		246	/* handled in wl_linux.c/wl_vx.c */
#define WLC_WDS_GET_WPA_SUP			247
#define WLC_SET_CS_SCAN_TIMER			248
#define WLC_GET_CS_SCAN_TIMER			249
#define WLC_MEASURE_REQUEST			250
#define WLC_INIT				251
#define WLC_SEND_QUIET				252
#define WLC_KEEPALIVE			253
#define WLC_SEND_PWR_CONSTRAINT			254
#define WLC_UPGRADE_STATUS			255
#define WLC_CURRENT_PWR				256
#define WLC_GET_SCAN_PASSIVE_TIME		257
#define WLC_SET_SCAN_PASSIVE_TIME		258
#define WLC_LEGACY_LINK_BEHAVIOR		259
#define WLC_GET_CHANNELS_IN_COUNTRY		260
#define WLC_GET_COUNTRY_LIST			261
#define WLC_GET_VAR				262	/* get value of named variable */
#define WLC_SET_VAR				263	/* set named variable to value */
#define WLC_NVRAM_GET				264	/* deprecated */
#define WLC_NVRAM_SET				265
#define WLC_NVRAM_DUMP				266
#define WLC_REBOOT				267
#define WLC_SET_WSEC_PMK			268
#define WLC_GET_AUTH_MODE			269
#define WLC_SET_AUTH_MODE			270
#define WLC_GET_WAKEENTRY			271
#define WLC_SET_WAKEENTRY			272
#define WLC_NDCONFIG_ITEM			273	/* currently handled in wl_oid.c */
#define WLC_NVOTPW				274
#define WLC_OTPW				275
#define WLC_IOV_BLOCK_GET			276
#define WLC_IOV_MODULES_GET			277
#define WLC_SOFT_RESET				278
#define WLC_GET_ALLOW_MODE			279
#define WLC_SET_ALLOW_MODE			280
#define WLC_GET_DESIRED_BSSID			281
#define WLC_SET_DESIRED_BSSID			282
#define	WLC_DISASSOC_MYAP			283
#define WLC_GET_NBANDS				284	/* for Dongle EXT_STA support */
#define WLC_GET_BANDSTATES			285	/* for Dongle EXT_STA support */
#define WLC_GET_WLC_BSS_INFO			286	/* for Dongle EXT_STA support */
#define WLC_GET_ASSOC_INFO			287	/* for Dongle EXT_STA support */
#define WLC_GET_OID_PHY				288	/* for Dongle EXT_STA support */
#define WLC_SET_OID_PHY				289	/* for Dongle EXT_STA support */
#define WLC_SET_ASSOC_TIME			290	/* for Dongle EXT_STA support */
#define WLC_GET_DESIRED_SSID			291	/* for Dongle EXT_STA support */
#define WLC_GET_CHANSPEC			292	/* for Dongle EXT_STA support */
#define WLC_GET_ASSOC_STATE			293	/* for Dongle EXT_STA support */
#define WLC_SET_PHY_STATE			294	/* for Dongle EXT_STA support */
#define WLC_GET_SCAN_PENDING			295	/* for Dongle EXT_STA support */
#define WLC_GET_SCANREQ_PENDING			296	/* for Dongle EXT_STA support */
#define WLC_GET_PREV_ROAM_REASON		297	/* for Dongle EXT_STA support */
#define WLC_SET_PREV_ROAM_REASON		298	/* for Dongle EXT_STA support */
#define WLC_GET_BANDSTATES_PI			299	/* for Dongle EXT_STA support */
#define WLC_GET_PHY_STATE			300	/* for Dongle EXT_STA support */
#define WLC_GET_BSS_WPA_RSN			301	/* for Dongle EXT_STA support */
#define WLC_GET_BSS_WPA2_RSN			302	/* for Dongle EXT_STA support */
#define WLC_GET_BSS_BCN_TS			303	/* for Dongle EXT_STA support */
#define WLC_GET_INT_DISASSOC			304	/* for Dongle EXT_STA support */
#define WLC_SET_NUM_PEERS			305     /* for Dongle EXT_STA support */
#define WLC_GET_NUM_BSS				306	/* for Dongle EXT_STA support */
#define WLC_NPHY_SAMPLE_COLLECT			307	/* Nphy sample collect mode */
#define WLC_UM_PRIV				308	/* for usermode driver private ioctl */
#define WLC_GET_CMD				309
/* #define WLC_LAST				310 */	/* Never used - can be reused */
#define WLC_SET_INTERFERENCE_OVERRIDE_MODE	311	/* set inter mode override */
#define WLC_GET_INTERFERENCE_OVERRIDE_MODE	312	/* get inter mode override */
#define WLC_GET_WAI_RESTRICT			313	/* for WAPI */
#define WLC_SET_WAI_RESTRICT			314	/* for WAPI */
#define WLC_SET_WAI_REKEY			315	/* for WAPI */
#define WLC_LAST

#define DOT11_BSSTYPE_INFRASTRUCTURE		0	/* d11 infrastructure */
#define DOT11_BSSTYPE_INDEPENDENT		1	/* d11 independent */
#define DOT11_BSSTYPE_ANY			2	/* d11 any BSS type */
#define DOT11_SCANTYPE_ACTIVE			0	/* d11 scan active */
#define DOT11_SCANTYPE_PASSIVE			1	/* d11 scan passive */

/* uint32 list */
typedef struct wl_uint32_list {
	/* in - # of elements, out - # of entries */
	uint32 count;
	/* variable length uint32 list */
	uint32 element[1];
} wl_uint32_list_t;

struct wl_ether_addr {
	uint8_t					octet[6];
} __attribute__ ((packed));

struct wl_maclist {
	uint					count;
	struct wl_ether_addr 	ea[1];
};

typedef struct wl_sta_rssi {
	int						rssi;
	char					mac[6];
	uint16_t				foo;
} wl_sta_rssi_t;

//#define WL_NUMRATES     255 /* max # of rates in a rateset */
#define WL_NUMRATES     16 /* max # of rates in a rateset */
typedef struct wl_rateset {
    uint32_t  				count;          /* # rates in this set */
    uint8_t   				rates[WL_NUMRATES]; /* rates in 500kbps units w/hi bit set if basic */
} wl_rateset_t;

typedef struct wl_bss_info {
	uint32		version;		/**< version field */
	uint32		length;			/**< byte length of data in this record,
						 * starting at version and including IEs
						 */
	struct wl_ether_addr BSSID;
	uint16		beacon_period;		/**< units are Kusec */
	uint16		capability;		/**< Capability information */
	uint8		SSID_len;
	uint8		SSID[32];
	struct {
		uint	count;			/**< # rates in this set */
		uint8	rates[16];		/**< rates in 500kbps units w/hi bit set if basic */
	} rateset;				/**< supported rates */
	chanspec_t	chanspec;		/**< chanspec for bss */
	uint16		atim_window;		/**< units are Kusec */
	uint8		dtim_period;		/**< DTIM period */
	int16		RSSI;			/**< receive signal strength (in dBm) */
	int8		phy_noise;		/**< noise (in dBm) */

	uint8		n_cap;			/**< BSS is 802.11N Capable */
	uint32		nbss_cap;		/**< 802.11N+AC BSS Capabilities */
	uint8		ctl_ch;			/**< 802.11N BSS control channel number */
	uint8		padding1[3];		/**< explicit struct alignment padding */
	uint16		vht_rxmcsmap;	/**< VHT rx mcs map (802.11ac IE, VHT_CAP_MCS_MAP_*) */
	uint16		vht_txmcsmap;	/**< VHT tx mcs map (802.11ac IE, VHT_CAP_MCS_MAP_*) */
	uint8		flags;			/**< flags */
	uint8		vht_cap;		/**< BSS is vht capable */
	uint8		reserved[2];		/**< Reserved for expansion of BSS properties */
	uint8		basic_mcs[MCSSET_LEN];	/**< 802.11N BSS required MCS set */

	uint16		ie_offset;		/**< offset at which IEs start, from beginning */
	uint32		ie_length;		/**< byte length of Information Elements */
	int16		SNR;			/**< average SNR of during frame reception */
	uint16		vht_mcsmap;		/**< STA's Associated vhtmcsmap */
	uint16		vht_mcsmap_prop;	/**< STA's Associated prop vhtmcsmap */
	uint16		vht_txmcsmap_prop;	/**< prop VHT tx mcs prop */
	/* Add new fields here */
	/* variable length Information Elements */
} wl_bss_info_t;

#define	WL_IF_STATS_T_VERSION	1	/* current version of wl_if_stats_t struct */

/* per interface counters */
typedef struct wl_if_stats {
	uint16	version;		/* version of the structure */
	uint16	length;			/* length of the entire structure */
	uint32	PAD;			/* padding */

	/* transmit stat counters */
	uint64	txframe;		/* tx data frames */
	uint64	txbyte;			/* tx data bytes */
	uint64	txerror;		/* tx data errors (derived: sum of others) */
	uint64  txnobuf;		/* tx out of buffer errors */
	uint64  txrunt;			/* tx runt frames */
	uint64  txfail;			/* tx failed frames */
	uint64	txretry;		/* tx retry frames */
	uint64	txretrie;		/* tx multiple retry frames */
	uint64	txfrmsnt;		/* tx sent frames */
	uint64	txmulti;		/* tx mulitcast sent frames */
	uint64	txfrag;			/* tx fragments sent */

	/* receive stat counters */
	uint64	rxframe;		/* rx data frames */
	uint64	rxbyte;			/* rx data bytes */
	uint64	rxerror;		/* rx data errors (derived: sum of others) */
	uint64	rxnobuf;		/* rx out of buffer errors */
	uint64  rxrunt;			/* rx runt frames */
	uint64  rxfragerr;		/* rx fragment errors */
	uint64	rxmulti;		/* rx multicast frames */
} wl_if_stats_t;

#define	WL_CNT_T_VERSION	30	/**< current version of wl_cnt_t struct */
/* Top structure of counters IOVar buffer */
typedef struct {
	uint16	version;	/**< see definition of WL_CNT_T_VERSION */
	uint16	datalen;	/**< length of data including all paddings. */
	uint8   data [1];	/**< variable length payload:
				 * 1 or more bcm_xtlv_t type of tuples.
				 * each tuple is padded to multiple of 4 bytes.
				 * 'datalen' field of this structure includes all paddings.
				 */
} wl_cnt_info_t;

#define	NFIFO			6	/**< # tx/rx fifopairs */
#define NREINITREASONCOUNT	8

/* wlc layer counters */
typedef struct {
	/* transmit stat counters */
	uint32	txframe;	/**< tx data frames */
	uint32	txbyte;		/**< tx data bytes */
	uint32	txretrans;	/**< tx mac retransmits */
	uint32	txerror;	/**< tx data errors (derived: sum of others) */
	uint32	txctl;		/**< tx management frames */
	uint32	txprshort;	/**< tx short preamble frames */
	uint32	txserr;		/**< tx status errors */
	uint32	txnobuf;	/**< tx out of buffers errors */
	uint32	txnoassoc;	/**< tx discard because we're not associated */
	uint32	txrunt;		/**< tx runt frames */
	uint32	txchit;		/**< tx header cache hit (fastpath) */
	uint32	txcmiss;	/**< tx header cache miss (slowpath) */

	/* transmit chip error counters */
	uint32	txuflo;		/**< tx fifo underflows */
	uint32	txphyerr;	/**< tx phy errors (indicated in tx status) */
	uint32	txphycrs;

	/* receive stat counters */
	uint32	rxframe;	/**< rx data frames */
	uint32	rxbyte;		/**< rx data bytes */
	uint32	rxerror;	/**< rx data errors (derived: sum of others) */
	uint32	rxctl;		/**< rx management frames */
	uint32	rxnobuf;	/**< rx out of buffers errors */
	uint32	rxnondata;	/**< rx non data frames in the data channel errors */
	uint32	rxbadds;	/**< rx bad DS errors */
	uint32	rxbadcm;	/**< rx bad control or management frames */
	uint32	rxfragerr;	/**< rx fragmentation errors */
	uint32	rxrunt;		/**< rx runt frames */
	uint32	rxgiant;	/**< rx giant frames */
	uint32	rxnoscb;	/**< rx no scb error */
	uint32	rxbadproto;	/**< rx invalid frames */
	uint32	rxbadsrcmac;	/**< rx frames with Invalid Src Mac */
	uint32	rxbadda;	/**< rx frames tossed for invalid da */
	uint32	rxfilter;	/**< rx frames filtered out */

	/* receive chip error counters */
	uint32	rxoflo;		/**< rx fifo overflow errors */
	uint32	rxuflo[NFIFO];	/**< rx dma descriptor underflow errors */

	uint32	d11cnt_txrts_off;	/**< d11cnt txrts value when reset d11cnt */
	uint32	d11cnt_rxcrc_off;	/**< d11cnt rxcrc value when reset d11cnt */
	uint32	d11cnt_txnocts_off;	/**< d11cnt txnocts value when reset d11cnt */

	/* misc counters */
	uint32	dmade;		/**< tx/rx dma descriptor errors */
	uint32	dmada;		/**< tx/rx dma data errors */
	uint32	dmape;		/**< tx/rx dma descriptor protocol errors */
	uint32	reset;		/**< reset count */
	uint32	tbtt;		/**< cnts the TBTT int's */
	uint32	txdmawar;
	uint32	pkt_callback_reg_fail;	/**< callbacks register failure */

	/* 802.11 MIB counters, pp. 614 of 802.11 reaff doc. */
	uint32	txfrag;		/**< dot11TransmittedFragmentCount */
	uint32	txmulti;	/**< dot11MulticastTransmittedFrameCount */
	uint32	txfail;		/**< dot11FailedCount */
	uint32	txretry;	/**< dot11RetryCount */
	uint32	txretrie;	/**< dot11MultipleRetryCount */
	uint32	rxdup;		/**< dot11FrameduplicateCount */
	uint32	txrts;		/**< dot11RTSSuccessCount */
	uint32	txnocts;	/**< dot11RTSFailureCount */
	uint32	txnoack;	/**< dot11ACKFailureCount */
	uint32	rxfrag;		/**< dot11ReceivedFragmentCount */
	uint32	rxmulti;	/**< dot11MulticastReceivedFrameCount */
	uint32	rxcrc;		/**< dot11FCSErrorCount */
	uint32	txfrmsnt;	/**< dot11TransmittedFrameCount (bogus MIB?) */
	uint32	rxundec;	/**< dot11WEPUndecryptableCount */

	/* WPA2 counters (see rxundec for DecryptFailureCount) */
	uint32	tkipmicfaill;	/**< TKIPLocalMICFailures */
	uint32	tkipcntrmsr;	/**< TKIPCounterMeasuresInvoked */
	uint32	tkipreplay;	/**< TKIPReplays */
	uint32	ccmpfmterr;	/**< CCMPFormatErrors */
	uint32	ccmpreplay;	/**< CCMPReplays */
	uint32	ccmpundec;	/**< CCMPDecryptErrors */
	uint32	fourwayfail;	/**< FourWayHandshakeFailures */
	uint32	wepundec;	/**< dot11WEPUndecryptableCount */
	uint32	wepicverr;	/**< dot11WEPICVErrorCount */
	uint32	decsuccess;	/**< DecryptSuccessCount */
	uint32	tkipicverr;	/**< TKIPICVErrorCount */
	uint32	wepexcluded;	/**< dot11WEPExcludedCount */

	uint32	txchanrej;	/**< Tx frames suppressed due to channel rejection */
	uint32	psmwds;		/**< Count PSM watchdogs */
	uint32	phywatchdog;	/**< Count Phy watchdogs (triggered by ucode) */

	/* MBSS counters, AP only */
	uint32	prq_entries_handled;	/**< PRQ entries read in */
	uint32	prq_undirected_entries;	/**<    which were bcast bss & ssid */
	uint32	prq_bad_entries;	/**<    which could not be translated to info */
	uint32	atim_suppress_count;	/**< TX suppressions on ATIM fifo */
	uint32	bcn_template_not_ready;	/**< Template marked in use on send bcn ... */
	uint32	bcn_template_not_ready_done; /* ...but "DMA done" interrupt rcvd */
	uint32	late_tbtt_dpc;	/**< TBTT DPC did not happen in time */

	/* per-rate receive stat counters */
	uint32  rx1mbps;	/* packets rx at 1Mbps */
	uint32  rx2mbps;	/* packets rx at 2Mbps */
	uint32  rx5mbps5;	/* packets rx at 5.5Mbps */
	uint32  rx6mbps;	/* packets rx at 6Mbps */
	uint32  rx9mbps;	/* packets rx at 9Mbps */
	uint32  rx11mbps;	/* packets rx at 11Mbps */
	uint32  rx12mbps;	/* packets rx at 12Mbps */
	uint32  rx18mbps;	/* packets rx at 18Mbps */
	uint32  rx24mbps;	/* packets rx at 24Mbps */
	uint32  rx36mbps;	/* packets rx at 36Mbps */
	uint32  rx48mbps;	/* packets rx at 48Mbps */
	uint32  rx54mbps;	/* packets rx at 54Mbps */
	uint32  rx108mbps;	/* packets rx at 108mbps */
	uint32  rx162mbps;	/* packets rx at 162mbps */
	uint32  rx216mbps;	/* packets rx at 216 mbps */
	uint32  rx270mbps;	/* packets rx at 270 mbps */
	uint32  rx324mbps;	/* packets rx at 324 mbps */
	uint32  rx378mbps;	/* packets rx at 378 mbps */
	uint32  rx432mbps;	/* packets rx at 432 mbps */
	uint32  rx486mbps;	/* packets rx at 486 mbps */
	uint32  rx540mbps;	/* packets rx at 540 mbps */

	uint32	rfdisable;	/**< count of radio disables */

	uint32	txexptime;	/**< Tx frames suppressed due to timer expiration */

	uint32	txmpdu_sgi;	/**< count for sgi transmit */
	uint32	rxmpdu_sgi;	/**< count for sgi received */
	uint32	txmpdu_stbc;	/**< count for stbc transmit */
	uint32	rxmpdu_stbc;	/**< count for stbc received */

	uint32	rxundec_mcst;	/**< dot11WEPUndecryptableCount */

	/* WPA2 counters (see rxundec for DecryptFailureCount) */
	uint32	tkipmicfaill_mcst;	/**< TKIPLocalMICFailures */
	uint32	tkipcntrmsr_mcst;	/**< TKIPCounterMeasuresInvoked */
	uint32	tkipreplay_mcst;	/**< TKIPReplays */
	uint32	ccmpfmterr_mcst;	/**< CCMPFormatErrors */
	uint32	ccmpreplay_mcst;	/**< CCMPReplays */
	uint32	ccmpundec_mcst;	/**< CCMPDecryptErrors */
	uint32	fourwayfail_mcst;	/**< FourWayHandshakeFailures */
	uint32	wepundec_mcst;	/**< dot11WEPUndecryptableCount */
	uint32	wepicverr_mcst;	/**< dot11WEPICVErrorCount */
	uint32	decsuccess_mcst;	/**< DecryptSuccessCount */
	uint32	tkipicverr_mcst;	/**< TKIPICVErrorCount */
	uint32	wepexcluded_mcst;	/**< dot11WEPExcludedCount */

	uint32	dma_hang;	/**< count for dma hang */
	uint32	reinit;		/**< count for reinit */

	uint32  pstatxucast;	/**< count of ucast frames xmitted on all psta assoc */
	uint32  pstatxnoassoc;	/**< count of txnoassoc frames xmitted on all psta assoc */
	uint32  pstarxucast;	/**< count of ucast frames received on all psta assoc */
	uint32  pstarxbcmc;	/**< count of bcmc frames received on all psta */
	uint32  pstatxbcmc;	/**< count of bcmc frames transmitted on all psta */

	uint32  cso_passthrough; /* hw cso required but passthrough */
	uint32	cso_normal;	/**< hw cso hdr for normal process */
	uint32	chained;	/**< number of frames chained */
	uint32	chainedsz1;	/**< number of chain size 1 frames */
	uint32	unchained;	/**< number of frames not chained */
	uint32	maxchainsz;	/**< max chain size so far */
	uint32	currchainsz;	/**< current chain size */
	uint32	pciereset;	/**< Secondary Bus Reset issued by driver */
	uint32	cfgrestore;	/**< configspace restore by driver */
	uint32	reinitreason[NREINITREASONCOUNT]; /* reinitreason counters; 0: Unknown reason */
	uint32	rxrtry;

	uint32  rxmpdu_mu;      /* Number of MU MPDUs received */

	/* detailed control/management frames */
	uint32  txbar;          /**< Number of TX BAR */
	uint32  rxbar;          /**< Number of RX BAR */
	uint32  txpspoll;       /**< Number of TX PS-poll */
	uint32  rxpspoll;       /**< Number of RX PS-poll */
	uint32  txnull;         /**< Number of TX NULL_DATA */
	uint32  rxnull;         /**< Number of RX NULL_DATA */
	uint32  txqosnull;      /**< Number of TX NULL_QoSDATA */
	uint32  rxqosnull;      /**< Number of RX NULL_QoSDATA */
	uint32  txassocreq;     /**< Number of TX ASSOC request */
	uint32  rxassocreq;     /**< Number of RX ASSOC request */
	uint32  txreassocreq;   /**< Number of TX REASSOC request */
	uint32  rxreassocreq;   /**< Number of RX REASSOC request */
	uint32  txdisassoc;     /**< Number of TX DISASSOC */
	uint32  rxdisassoc;     /**< Number of RX DISASSOC */
	uint32  txassocrsp;     /**< Number of TX ASSOC response */
	uint32  rxassocrsp;     /**< Number of RX ASSOC response */
	uint32  txreassocrsp;   /**< Number of TX REASSOC response */
	uint32  rxreassocrsp;   /**< Number of RX REASSOC response */
	uint32  txauth;         /**< Number of TX AUTH */
	uint32  rxauth;         /**< Number of RX AUTH */
	uint32  txdeauth;       /**< Number of TX DEAUTH */
	uint32  rxdeauth;       /**< Number of RX DEAUTH */
	uint32  txprobereq;     /**< Number of TX probe request */
	uint32  rxprobereq;     /**< Number of RX probe request */
	uint32  txprobersp;     /**< Number of TX probe response */
	uint32  rxprobersp;     /**< Number of RX probe response */
	uint32  txaction;       /**< Number of TX action frame */
	uint32  rxaction;       /**< Number of RX action frame */
	uint32  ampdu_wds;      /**< Number of AMPDU watchdogs */
	uint32  txlost;         /**< Number of lost packets reported in txs */
	uint32	txdatamcast;	/**< Number of TX multicast data packets */
	uint32	txdatabcast;	/**< Number of TX broadcast data packets */
	uint32	psmxwds;	/**< Number of PSMx watchdogs */
	uint32	txchain_shutdown; /* Number of Txchain shutdowns due to over temperature */
#ifdef DSLCPE
	uint32	txnodup;	/**< tx PKTDUP errors */
#endif
} wl_cnt_wlc_t;

struct bs_data {
	char macaddr[24];
	char phy_mbps[8];
	char data_mbps[8];
	char air_use[8];
	char data_use[8];
	char retries[8];
};

struct bsd_sta_info {
	char sta_mac[32];
	char iface[8];
	int timestamp;
	int tx_rate;
	int rssi;
	bool bounce;
	bool picky;
	bool psta;
	bool dualband;
};

struct bsd_record {
	int timestamp;
	char sta_mac[32];
	int from_ch;
	int to_ch;
	char reason[16];
	char desc[64];
};

typedef struct wl_scan_results {
	uint32 buflen;
	uint32 version;
	uint32 count;
	wl_bss_info_t bss_info[1];
} wl_scan_results_t;

/* Used to get specific STA parameters */
typedef struct wl_scb_val {
	uint32	val;
	struct wl_ether_addr ea;
} wl_scb_val_t;

/* Used to get specific link/ac parameters */
typedef struct wl_link_val {
	int ac;
	uint8 val;
	struct wl_ether_addr ea;
} wl_link_val_t;

/* Used by iovar versions of some ioctls, i.e. WLC_SCB_AUTHORIZE et al */
typedef struct wl_authops {
	uint32 code;
	wl_scb_val_t ioctl_args;
} wl_authops_t;

typedef struct wl_sta_info {
	uint16			ver;		/**< version of this struct */
	uint16			len;		/**< length in bytes of this structure */
	uint16			cap;		/**< sta's advertised capabilities */
	uint16                  PAD;
	uint32			flags;		/**< flags defined below */
	uint32			idle;		/**< time since data pkt rx'd from sta */
	struct wl_ether_addr	ea;		/**< Station address */
	uint16                  PAD2;
	wl_rateset_t		rateset;	/**< rateset in use */
	uint32			in;		/**< seconds elapsed since associated */
	uint32			listen_interval_inms; /* Min Listen interval in ms for this STA */
	uint32			tx_pkts;	/**< # of user packets transmitted (unicast) */
	uint32			tx_failures;	/**< # of user packets failed */
	uint32			rx_ucast_pkts;	/**< # of unicast packets received */
	uint32			rx_mcast_pkts;	/**< # of multicast packets received */
	uint32			tx_rate;	/**< Rate used by last tx frame */
	uint32			rx_rate;	/**< Rate of last successful rx frame */
	uint32			rx_decrypt_succeeds;	/**< # of packet decrypted successfully */
	uint32			rx_decrypt_failures;	/**< # of packet decrypted unsuccessfully */
	uint32			tx_tot_pkts;	/**< # of user tx pkts (ucast + mcast) */
	uint32			rx_tot_pkts;	/**< # of data packets recvd (uni + mcast) */
	uint32			tx_mcast_pkts;	/**< # of mcast pkts txed */
	uint64			tx_tot_bytes;	/**< data bytes txed (ucast + mcast) */
	uint64			rx_tot_bytes;	/**< data bytes recvd (ucast + mcast) */
	uint64			tx_ucast_bytes;	/**< data bytes txed (ucast) */
	uint64			tx_mcast_bytes;	/**< # data bytes txed (mcast) */
	uint64			rx_ucast_bytes;	/**< data bytes recvd (ucast) */
	uint64			rx_mcast_bytes;	/**< data bytes recvd (mcast) */
	int8			rssi[WL_STA_ANT_MAX]; /* average rssi per antenna
										   * of data frames
										   */
	int8			nf[WL_STA_ANT_MAX];	/**< per antenna noise floor */
	uint16			aid;		/**< association ID */
	uint16			ht_capabilities;	/**< advertised ht caps */
	uint16			vht_flags;		/**< converted vht flags */
	uint16                  PAD3;
	uint32			tx_pkts_retried;	/**< # of frames where a retry was
							 * necessary
							 */
	uint32			tx_pkts_retry_exhausted; /* # of user frames where a retry
							  * was exhausted
							  */
	int8			rx_lastpkt_rssi[WL_STA_ANT_MAX]; /* Per antenna RSSI of last
								  * received data frame.
								  */
	/* TX WLAN retry/failure statistics:
	 * Separated for host requested frames and WLAN locally generated frames.
	 * Include unicast frame only where the retries/failures can be counted.
	 */
	uint32			tx_pkts_total;		/**< # user frames sent successfully */
	uint32			tx_pkts_retries;	/**< # user frames retries */
	uint32			tx_pkts_fw_total;	/**< # FW generated sent successfully */
	uint32			tx_pkts_fw_retries;	/**< # retries for FW generated frames */
	uint32			tx_pkts_fw_retry_exhausted;	/**< # FW generated where a retry
								 * was exhausted
								 */
	uint32			rx_pkts_retried;	/**< # rx with retry bit set */
	uint32			tx_rate_fallback;	/**< lowest fallback TX rate */
} wl_sta_info_t;

#define WL_HE_CAP_MCS_MAP_NSS_MAX       8
#define WL_VHT_CAP_MCS_MAP_NSS_MAX      8
#define WL_MAXRATES_IN_SET		16	/**< max # of rates in a rateset */
#define MCSSET_LEN	16	/* 16-bits per 8-bit set to give 128-bits bitmap of MCS Index */

#define WLC_MAX_ASSOC_OUI_NUM 6
typedef struct {
	uint8 count;
	uint8 oui[WLC_MAX_ASSOC_OUI_NUM][3];
} sta_vendor_oui_t;

typedef struct wl_rateset_args_v2 {
	uint16 version;         /**< version. */
	uint16 len;             /**< length */
	uint32  count;          /**< # rates in this set */
	uint8   rates[WL_MAXRATES_IN_SET];      /**< rates in 500kbps units w/hi bit set if basic */
	uint8   mcs[MCSSET_LEN];                /**< supported mcs index bit map */
	uint16 vht_mcs[WL_VHT_CAP_MCS_MAP_NSS_MAX]; /**< supported mcs index bit map per nss */
	uint16 he_mcs[WL_HE_CAP_MCS_MAP_NSS_MAX]; /**< supported he mcs index bit map per nss */
} wl_rateset_args_v2_t;


/* sta_info_t ver-7 extension */
typedef struct wl_sta_info_ext7 {
	uint32                  rx_dur_total;   /* total user RX duration (estimated) */

	chanspec_t              chanspec;       /** chanspec this sta is on */
	uint16                  PAD;
	wl_rateset_args_v2_t    rateset_adv;    /* rateset along with mcs index bitmap */
	uint16                  wpauth;                 /* authentication type */
	uint8                   algo;                   /* crypto algorithm */
	uint8			PAD2;
	uint32                  tx_rspec;       /* Rate of last successful tx frame */
	uint32                  rx_rspec;       /* Rate of last successful rx frame */
	uint32                  wnm_cap;        /* wnm capabilities */

	uint16                  he_flags;	/* converted he flags */
	uint16                  PAD3;
	sta_vendor_oui_t        sta_vendor_oui;
	uint8			link_bw;
} wl_sta_info_ext7_t;

typedef struct wlc_ssid {
	uint32_t				ssid_len;
	unsigned char			ssid[32];
} wlc_ssid_t;

/* Linux network driver ioctl encoding */
typedef struct wl_ioctl {
	uint32_t				cmd;	/* common ioctl definition */
	void					*buf;	/* pointer to user buffer */
	uint32_t				len;	/* length of user buffer */
	uint8_t					set;	/* get or set request (optional) */
	uint32_t				used;	/* bytes read or written (optional) */
	uint32_t				needed;	/* bytes needed (optional) */
} wl_ioctl_t;

/*
 * Structure for passing hardware and software
 * revision info up from the driver.
 */
typedef struct wlc_rev_info {
	uint		vendorid;	/**< PCI vendor id */
	uint		deviceid;	/**< device id of chip */
	uint		radiorev;	/**< radio revision */
	uint		chiprev;	/**< chip revision */
	uint		corerev;	/**< core revision */
	uint		boardid;	/**< board identifier (usu. PCI sub-device id) */
	uint		boardvendor;	/**< board vendor (usu. PCI sub-vendor id) */
	uint		boardrev;	/**< board revision */
	uint		driverrev;	/**< driver version */
	uint		ucoderev;	/**< microcode version */
	uint		bus;		/**< bus type */
	uint		chipnum;	/**< chip number */
	uint		phytype;	/**< phy type */
	uint		phyrev;		/**< phy revision */
	uint		anarev;		/**< anacore rev */
	uint		chippkg;	/**< chip package info */
	uint		nvramrev;	/**< nvram revision number */
} wlc_rev_info_t;

#define WLC_CNTRY_BUF_SZ	4
typedef struct wl_country {
	char country_abbrev[WLC_CNTRY_BUF_SZ];	/**< nul-terminated country code used in
						 * the Country IE
						 */
	int32 rev;				/**< revision specifier for ccode
						 * on set, -1 indicates unspecified.
						 * on get, rev >= 0
						 */
	char ccode[WLC_CNTRY_BUF_SZ];		/**< nul-terminated built-in country code.
						 * variable length, but fixed size in
						 * struct allows simple allocation for
						 * expected country strings <= 3 chars.
						 */
} wl_country_t;

typedef struct wl_country_list {
	uint32_t buflen;
	uint32_t band_set;
	uint32_t band;
	uint32_t count;
	char country_abbrev[1];
} wl_country_list_t;

struct ether_addr {
	uint8 octet[ETHER_ADDR_LEN];
};

typedef struct wl_scan_params {
	wlc_ssid_t ssid;		/* default: {0, ""} */
	struct ether_addr bssid;	/* default: bcast */
	int8 bss_type;			/* default: any,
					 * DOT11_BSSTYPE_ANY/INFRASTRUCTURE/INDEPENDENT
					 */
	int8 scan_type;			/* -1 use default, DOT11_SCANTYPE_ACTIVE/PASSIVE */
	int32 nprobes;			/* -1 use default, number of probes per channel */
	int32 active_time;		/* -1 use default, dwell time per channel for
					 * active scanning
					 */
	int32 passive_time;		/* -1 use default, dwell time per channel
					 * for passive scanning
					 */
	int32 home_time;		/* -1 use default, dwell time for the home channel
					 * between channel scans
					 */
	int32 channel_num;		/* count of channels and ssids that follow
					 *
					 * low half is count of channels in channel_list, 0
					 * means default (use all available channels)
					 *
					 * high half is entries in wlc_ssid_t array that
					 * follows channel_list, aligned for int32 (4 bytes)
					 * meaning an odd channel count implies a 2-byte pad
					 * between end of channel_list and first ssid
					 *
					 * if ssid count is zero, single ssid in the fixed
					 * parameter portion is assumed, otherwise ssid in
					 * the fixed portion is ignored
					 */
	uint16 channel_list[1];		/* list of chanspecs */
} wl_scan_params_t;

/* 11k Neighbor Report element */
typedef struct nbr_element {
	uint8_t id;
	uint8_t len;
	struct wl_ether_addr bssid;
	uint32_t bssid_info;
	uint8_t reg;
	uint8_t channel;
	uint8_t phytype;
	uint8_t addtype;
	uint8_t bss_trans_preference;
	uint8_t pad[3];
} __attribute__((packed)) nbr_element_t;
#define NBR_ADD_STATIC 0
#define NBR_ADD_DYNAMIC 1

typedef struct bcnreq {
	uint8_t bcn_mode;
	int dur;
	int channel;
	struct ether_addr da;
	uint16_t random_int;
	wlc_ssid_t ssid;
	uint16_t reps;
} bcnreq_t;

#ifdef BCM_imp26
typedef struct wl_bsstrans_req {
	uint16 tbtt;	/* time of BSS to end of life, in unit of TBTT */
	uint16 dur;	/* time of BSS to keep off, in unit of minute */
	uint8 reqmode;	/* request mode of BSS transition request */
	uint8 unicast;	/* request by unicast or by broadcast */
} __attribute__((packed)) wl_bsstrans_req_t;
#else	//imp32
typedef struct wl_bsstrans_req {
	uint16 tbtt;			/* time of BSS to end of life, in unit of TBTT */
	uint16 dur;			/* time of BSS to keep off, in unit of minute */
	uint8 reqmode;			/* request mode of BSS transition request */
	uint8 unicast;			/* request by unicast or by broadcast */
	// we don't use following anyway
	struct wl_ether_addr sta_mac;	/* sta mac */
	uint8 retry_delay;		/* delay in second to wait before trying association */
	uint8 reason;
	uint8 token;			/* Dialog token to be used */
	uint8 pad;
} __attribute__((packed)) wl_bsstrans_req_t;
#endif

/* cca metrics to calculate channel utilization */
typedef struct {
	uint32 duration;
	uint32 congest_ibss;
	uint32 congest_obss;
	uint32 interference;
	uint32 timestamp;
} cca_t;

typedef struct {
	chanspec_t chanspec;
	uint16 num_secs;
	cca_t secs[1];
} cca_req_t;

typedef struct wl_chan_switch {
	uint8 mode;		/* value 0 or 1 */
	uint8 count;		/* count # of beacons before switching */
	chanspec_t chspec;	/* chanspec */
	uint8 reg;		/* regulatory class */
	uint8 frame_type;	/* csa frame type, unicast or broadcast */
} wl_chan_switch_t;

/* The actual counters maintained for each station */
typedef struct {
	/* The following counters are a subset of what pktq_stats provides per precedence. */
	uint32 retry;          /* packets re-sent because they were not received */
	uint32 retry_drop;     /* packets finally dropped after retry limit */
	uint32 rtsfail;        /* count of rts attempts that failed to receive cts */
	uint32 acked;          /* count of packets sent (acked) successfully */
	uint32 txrate_succ;    /* running total of phy rate of packets sent successfully */
	uint32 txrate_main;    /* running total of phy 'main' rate */
	uint32 throughput;     /* actual data transferred successfully */
	uint32 time_delta;     /* time difference since last pktq_stats */
	uint32 airtime;        /* cumulative total medium access delay in useconds */
} __attribute__ ((packed))iov_bs_data_counters_t;

/* The structure for individual station information. */
typedef struct {
	struct ether_addr	station_address;	/* The station MAC address */
	uint16			station_flags;		/* Bit mask of flags, for future use. */
	iov_bs_data_counters_t	station_counters;	/* The actual counter values */
} __attribute__ ((packed)) iov_bs_data_record_t;

typedef struct {
	uint16	structure_version;	/* Structure version number (for wl/wlu matching) */
	uint16	structure_count;	/* Number of iov_bs_data_record_t records following */
	iov_bs_data_record_t	structure_record[1];	/* 0 - structure_count records */
} __attribute__ ((packed)) iov_bs_data_struct_t;

/* Bitmask of options that can be passed in to the iovar. */
enum {
	SCB_BS_DATA_FLAG_NO_RESET = (1<<0)	/* Do not clear the counters after reading */
};


#define EDCF_AIFSN_MASK              0x0f        /* AIFSN mask */
#define EDCF_ACM_MASK                0x10        /* ACM mask */
#define EDCF_ACI_MASK                0x60        /* ACI mask */

#define EDCF_ECWMIN_MASK             0x0f        /* cwmin exponent form mask */
#define EDCF_ECWMAX_MASK             0xf0        /* cwmax exponent form mask */
#define EDCF_ECWMAX_SHIFT            4           /* cwmax exponent form shift */

struct edcf_acparam {
	uint8	ACI;
	uint8	ECW;
	uint16  TXOP;		/* stored in network order (ls octet first) */
} __attribute__ ((packed));

typedef struct {
	uint32 packets;
	uint32 bytes;
} wl_traffic_stats_t;

#define AC_COUNT		4	/* number of ACs */

typedef struct {
	uint16	version;	/**< see definition of WL_WME_CNT_VERSION */
	uint16	length;		/**< length of entire structure */
	wl_traffic_stats_t tx[AC_COUNT];	/**< Packets transmitted */
	wl_traffic_stats_t tx_failed[AC_COUNT];	/**< Packets dropped or failed to transmit */
	wl_traffic_stats_t rx[AC_COUNT];	/**< Packets received */
	wl_traffic_stats_t rx_failed[AC_COUNT];	/**< Packets failed to receive */
	wl_traffic_stats_t forward[AC_COUNT];	/**< Packets forwarded by AP */
	wl_traffic_stats_t tx_expired[AC_COUNT]; /**< packets dropped due to lifetime expiry */
} wl_wme_cnt_t;

typedef struct {
	uint32 scb_timeout;
	uint32 scb_activity_time;
	uint32 scb_max_probe;
} wl_scb_probe_t;

#endif /*_IOPSYS_QUESTD_BROADCOM_H*/
