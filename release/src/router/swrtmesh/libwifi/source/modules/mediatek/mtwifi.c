/*
 * mtwifi.c - implements for mtwifi drivers.
 *
 * Copyright (C) 2023 SWRTdev. All rights reserved.
 * Copyright (C) 2023 paldier <paldier@hotmail.com>.
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
#define TYPEDEF_BOOL
#include <easy/easy.h>
#include "wifi.h"
#include "wifiutils.h"
#include "nlwifi.h"
#include "wpactrl_util.h"
#include "debug.h"
#include "drivers.h"
#include <shared.h>
#include <ralink.h>
#undef TYPEDEF_BOOL

/* mt_wifi/embedded/include/oid.h */
#define	OID_GET_SET_FROM_UI			0x4000
#define	OID_802_11_NETWORK_TYPES_SUPPORTED			0x0103
#define	OID_802_11_NETWORK_TYPE_IN_USE				0x0104
#define	OID_802_11_RSSI_TRIGGER						0x0107
#define	RT_OID_802_11_RSSI							0x0108	/* rt2860 only */
#define	RT_OID_802_11_RSSI_1						0x0109	/* rt2860 only */
#define	RT_OID_802_11_RSSI_2						0x010A	/* rt2860 only */
#define	OID_802_11_NUMBER_OF_ANTENNAS				0x010B
#define	OID_802_11_RX_ANTENNA_SELECTED				0x010C
#define	OID_802_11_TX_ANTENNA_SELECTED				0x010D
#define	OID_802_11_SUPPORTED_RATES					0x010E
#define	OID_802_11_ADD_WEP							0x0112
#define	OID_802_11_REMOVE_WEP						0x0113
#define	OID_802_11_PRIVACY_FILTER					0x0118
#define	OID_802_11_ASSOCIATION_INFORMATION			0x011E
#define	OID_802_11_TEST								0x011F
#define	RT_OID_802_11_COUNTRY_REGION				0x0507
#define	RT_OID_802_11_PHY_MODE						0x050C
#define	RT_OID_802_11_STA_CONFIG					0x050D
#define	OID_802_11_DESIRED_RATES					0x050E
#define	RT_OID_802_11_PREAMBLE						0x050F
#define	OID_802_11_WEP_STATUS						0x0510
#define	OID_802_11_AUTHENTICATION_MODE				0x0511
#define	OID_802_11_INFRASTRUCTURE_MODE				0x0512
#define	RT_OID_802_11_RESET_COUNTERS				0x0513
#define	OID_802_11_RTS_THRESHOLD					0x0514
#define	OID_802_11_FRAGMENTATION_THRESHOLD			0x0515
#define	OID_802_11_POWER_MODE						0x0516
#define	OID_802_11_TX_POWER_LEVEL					0x0517
#define	RT_OID_802_11_ADD_WPA						0x0518
#define	OID_802_11_REMOVE_KEY						0x0519
#define	RT_OID_802_11_QUERY_PID						0x051A
#define	RT_OID_802_11_QUERY_VID						0x051B
#define	OID_802_11_ADD_KEY							0x0520
#define	OID_802_11_CONFIGURATION					0x0521
#define	OID_802_11_TX_PACKET_BURST					0x0522
#define	RT_OID_802_11_QUERY_NOISE_LEVEL				0x0523
#define	RT_OID_802_11_EXTRA_INFO					0x0524
#define	RT_OID_802_11_HARDWARE_REGISTER				0x0525
#define OID_802_11_ENCRYPTION_STATUS            OID_802_11_WEP_STATUS
#define OID_802_11_DEAUTHENTICATION                 0x0526
#define OID_802_11_DROP_UNENCRYPTED                 0x0527
#define OID_802_11_MIC_FAILURE_REPORT_FRAME         0x0528
#define OID_802_11_EAP_METHOD						0x0529
#define OID_802_11_ACL_LIST							0x052A
#define OID_802_11_ACL_ADD_ENTRY					0x052B
#define OID_802_11_ACL_DEL_ENTRY					0x052C
#define OID_802_11_ACL_SET_POLICY					0x052D
#define OID_802_11_HT_STBC							0x052E
#define OID_802_11_UAPSD							0x052F
#define OID_802_11_AMSDU							0x0531
#define OID_802_11_AMPDU							0x0532
#define OID_802_11_APCFG							0x0533
#define OID_802_11_ASSOLIST							0x0534
#define OID_802_11_CURRENT_CRED					0x0535
#define OID_802_11_PASSPHRASES					0x0536
#define OID_802_11_CHANNEL_WIDTH					0x0537
#define OID_802_11_BEACON_PERIOD					0x0538
#define OID_802_11_COEXISTENCE						0x0530
#define OID_802_DOT1X_CONFIGURATION					0x0540
#define OID_802_DOT1X_PMKID_CACHE					0x0541
#define OID_802_DOT1X_RADIUS_DATA					0x0542
#define OID_802_DOT1X_WPA_KEY						0x0543
#define OID_802_DOT1X_STATIC_WEP_COPY				0x0544
#define OID_802_DOT1X_IDLE_TIMEOUT					0x0545
#define OID_802_DOT1X_RADIUS_ACL_NEW_CACHE          0x0546
#define OID_802_DOT1X_RADIUS_ACL_DEL_CACHE          0x0547
#define OID_802_DOT1X_RADIUS_ACL_CLEAR_CACHE        0x0548
#define OID_802_DOT1X_QUERY_STA_AID                 0x0549
#define OID_802_DOT1X_QUERY_STA_DATA                 0x0550
#define OID_802_DOT1X_QUERY_STA_RSN                 0x0551
#define	RT_OID_DEVICE_NAME							0x0607
#define	RT_OID_VERSION_INFO							0x0608
#define	RT_OID_802_11_QUERY_LINK_STATUS				0x060C
#define	OID_802_11_RSSI								0x060D
#define	OID_802_11_STATISTICS						0x060E
#define	OID_GEN_RCV_OK								0x060F
#define	OID_GEN_RCV_NO_BUFFER						0x0610
#define	RT_OID_802_11_QUERY_EEPROM_VERSION			0x0611
#define	RT_OID_802_11_QUERY_FIRMWARE_VERSION		0x0612
#define	RT_OID_802_11_QUERY_LAST_RX_RATE			0x0613
#define	RT_OID_802_11_TX_POWER_LEVEL_1				0x0614
#define	RT_OID_802_11_QUERY_PIDVID					0x0615
#define OID_SET_COUNTERMEASURES                     0x0616
#define OID_802_11_SET_IEEE8021X                    0x0617
#define OID_802_11_SET_IEEE8021X_REQUIRE_KEY        0x0618
#define OID_802_11_PMKID                            0x0620
#define RT_OID_WPA_SUPPLICANT_SUPPORT               0x0621
#define RT_OID_WE_VERSION_COMPILED                  0x0622
#define RT_OID_NEW_DRIVER                           0x0623
#define	OID_AUTO_PROVISION_BSSID_LIST				0x0624
#define RT_OID_WPS_PROBE_REQ_IE						0x0625
#define	RT_OID_802_11_SNR_0							0x0630
#define	RT_OID_802_11_SNR_1							0x0631
#define	RT_OID_802_11_QUERY_LAST_TX_RATE			0x0632
#define	RT_OID_802_11_QUERY_HT_PHYMODE				0x0633
#define	RT_OID_802_11_SET_HT_PHYMODE				0x0634
#define	OID_802_11_RELOAD_DEFAULTS					0x0635
#define	RT_OID_802_11_QUERY_APSD_SETTING			0x0636
#define	RT_OID_802_11_SET_APSD_SETTING				0x0637
#define	RT_OID_802_11_QUERY_APSD_PSM				0x0638
#define	RT_OID_802_11_SET_APSD_PSM					0x0639
#define	RT_OID_802_11_QUERY_DLS						0x063A
#define	RT_OID_802_11_SET_DLS						0x063B
#define	RT_OID_802_11_QUERY_DLS_PARAM				0x063C
#define	RT_OID_802_11_SET_DLS_PARAM					0x063D
#define RT_OID_802_11_QUERY_WMM						0x063E
#define RT_OID_802_11_SET_WMM						0x063F
#define RT_OID_802_11_QUERY_IMME_BA_CAP				0x0640
#define RT_OID_802_11_SET_IMME_BA_CAP				0x0641
#define RT_OID_802_11_QUERY_BATABLE					0x0642
#define RT_OID_802_11_ADD_IMME_BA					0x0643
#define RT_OID_802_11_TEAR_IMME_BA					0x0644
#define RT_OID_DRIVER_DEVICE_NAME                   0x0645
#define RT_OID_802_11_QUERY_DAT_HT_PHYMODE          0x0646
#define OID_WAPP_EVENT                              0x0647
#define OID_WAPP_EVENT2								0x09B4
#define OID_802_11_SET_PSPXLINK_MODE				0x0648
#define OID_802_11_SET_PASSPHRASE					0x0649
#define RT_OID_802_11_QUERY_TX_PHYMODE                          0x0650
#define RT_OID_802_11_QUERY_MAP_REAL_TX_RATE                          0x0678
#define RT_OID_802_11_QUERY_MAP_REAL_RX_RATE                          0x0679
#define	RT_OID_802_11_SNR_2							0x067A
#define RT_OID_802_11_QUERY_TXBF_TABLE				0x067C
#define RT_OID_802_11_PER_BSS_STATISTICS			0x067D
#define OID_802_11_VENDOR_IE_ADD				0x067E
#define OID_802_11_VENDOR_IE_UPDATE				0x067F
#define OID_802_11_VENDOR_IE_REMOVE				0x0680
#define OID_802_11_VENDOR_IE_SHOW				0x0681
#define OID_VERI_PKT_HEAD_UPDATE                                0x0682
#define OID_VERI_PKT_CTNT_UPDATE                                0x0683
#define OID_VERI_PKT_CTRL_ASSIGN_UPDATE                         0x0684
#define OID_VERI_PKT_CTRL_EN_UPDATE                             0x0685
#define OID_VERI_PKT_SEND                                       0x0686
#define	OID_802_11_GET_SSID_BSSID								0x0689
#define OID_802_11_GET_CURRENT_CHANNEL_STATS                    0x0693
#define OID_802_11_QUERY_WirelessMode				0x0718
#define RT_OID_802_11_QUERY_TDLS_PARAM			0x0676
#define	RT_OID_802_11_QUERY_TDLS				0x0677
#define OID_MTK_CHIP_ID							0x068A
#define OID_MTK_DRVER_VERSION					0x068B
#define OID_MAX_NUM_OF_STA					0x068C
#define	RT_OID_802_11_BSSID					  (OID_GET_SET_TOGGLE |	OID_802_11_BSSID)
#define	RT_OID_802_11_SSID					  (OID_GET_SET_TOGGLE |	OID_802_11_SSID)
#define	RT_OID_802_11_INFRASTRUCTURE_MODE	  (OID_GET_SET_TOGGLE |	OID_802_11_INFRASTRUCTURE_MODE)
#define	RT_OID_802_11_ADD_WEP				  (OID_GET_SET_TOGGLE |	OID_802_11_ADD_WEP)
#define	RT_OID_802_11_ADD_KEY				  (OID_GET_SET_TOGGLE |	OID_802_11_ADD_KEY)
#define	RT_OID_802_11_REMOVE_WEP			  (OID_GET_SET_TOGGLE |	OID_802_11_REMOVE_WEP)
#define	RT_OID_802_11_REMOVE_KEY			  (OID_GET_SET_TOGGLE |	OID_802_11_REMOVE_KEY)
#define	RT_OID_802_11_DISASSOCIATE			  (OID_GET_SET_TOGGLE |	OID_802_11_DISASSOCIATE)
#define	RT_OID_802_11_AUTHENTICATION_MODE	  (OID_GET_SET_TOGGLE |	OID_802_11_AUTHENTICATION_MODE)
#define	RT_OID_802_11_PRIVACY_FILTER		  (OID_GET_SET_TOGGLE |	OID_802_11_PRIVACY_FILTER)
#define	RT_OID_802_11_BSSID_LIST_SCAN		  (OID_GET_SET_TOGGLE |	OID_802_11_BSSID_LIST_SCAN)
#define	RT_OID_802_11_WEP_STATUS			  (OID_GET_SET_TOGGLE |	OID_802_11_WEP_STATUS)
#define	RT_OID_802_11_RELOAD_DEFAULTS		  (OID_GET_SET_TOGGLE |	OID_802_11_RELOAD_DEFAULTS)
#define	RT_OID_802_11_NETWORK_TYPE_IN_USE	  (OID_GET_SET_TOGGLE |	OID_802_11_NETWORK_TYPE_IN_USE)
#define	RT_OID_802_11_TX_POWER_LEVEL		  (OID_GET_SET_TOGGLE |	OID_802_11_TX_POWER_LEVEL)
#define	RT_OID_802_11_RSSI_TRIGGER			  (OID_GET_SET_TOGGLE |	OID_802_11_RSSI_TRIGGER)
#define	RT_OID_802_11_FRAGMENTATION_THRESHOLD (OID_GET_SET_TOGGLE |	OID_802_11_FRAGMENTATION_THRESHOLD)
#define	RT_OID_802_11_RTS_THRESHOLD			  (OID_GET_SET_TOGGLE |	OID_802_11_RTS_THRESHOLD)
#define	RT_OID_802_11_RX_ANTENNA_SELECTED	  (OID_GET_SET_TOGGLE |	OID_802_11_RX_ANTENNA_SELECTED)
#define	RT_OID_802_11_TX_ANTENNA_SELECTED	  (OID_GET_SET_TOGGLE |	OID_802_11_TX_ANTENNA_SELECTED)
#define	RT_OID_802_11_SUPPORTED_RATES		  (OID_GET_SET_TOGGLE |	OID_802_11_SUPPORTED_RATES)
#define	RT_OID_802_11_DESIRED_RATES			  (OID_GET_SET_TOGGLE |	OID_802_11_DESIRED_RATES)
#define	RT_OID_802_11_CONFIGURATION			  (OID_GET_SET_TOGGLE |	OID_802_11_CONFIGURATION)
#define	RT_OID_802_11_POWER_MODE			  (OID_GET_SET_TOGGLE |	OID_802_11_POWER_MODE)
#define RT_OID_802_11_SET_PSPXLINK_MODE		  (OID_GET_SET_TOGGLE |	OID_802_11_SET_PSPXLINK_MODE)
#define RT_OID_802_11_EAP_METHOD			  (OID_GET_SET_TOGGLE | OID_802_11_EAP_METHOD)
#define RT_OID_802_11_SET_PASSPHRASE		  (OID_GET_SET_TOGGLE | OID_802_11_SET_PASSPHRASE)
#define RT_OID_802_DOT1X_PMKID_CACHE		(OID_GET_SET_TOGGLE | OID_802_DOT1X_PMKID_CACHE)
#define RT_OID_802_DOT1X_RADIUS_DATA		(OID_GET_SET_TOGGLE | OID_802_DOT1X_RADIUS_DATA)
#define RT_OID_802_DOT1X_WPA_KEY			(OID_GET_SET_TOGGLE | OID_802_DOT1X_WPA_KEY)
#define RT_OID_802_DOT1X_STATIC_WEP_COPY	(OID_GET_SET_TOGGLE | OID_802_DOT1X_STATIC_WEP_COPY)
#define RT_OID_802_DOT1X_IDLE_TIMEOUT		(OID_GET_SET_TOGGLE | OID_802_DOT1X_IDLE_TIMEOUT)
#define RT_OID_802_11_SET_TDLS_PARAM			(OID_GET_SET_TOGGLE | RT_OID_802_11_QUERY_TDLS_PARAM)
#define RT_OID_802_11_SET_TDLS				(OID_GET_SET_TOGGLE | RT_OID_802_11_QUERY_TDLS)
#define OID_802_11_ACL_BLK_REJCT_COUNT_STATICS			0x069b
#define RT_OID_802_11_MAC_ADDRESS				0x0713
#define OID_802_11_BUILD_CHANNEL_EX				0x0714
#define OID_802_11_GET_CH_LIST					0x0715
#define OID_802_11_GET_COUNTRY_CODE				0x0716
#define OID_802_11_GET_CHANNEL_GEOGRAPHY		0x0717
#define RT_OID_WAC_REQ								0x0736
#define	RT_OID_WSC_AUTO_PROVISION_WITH_BSSID		0x0737
#define	RT_OID_WSC_AUTO_PROVISION					0x0738
#define RT_OID_WSC_SET_PASSPHRASE                   0x0740
#define RT_OID_WSC_DRIVER_AUTO_CONNECT              0x0741
#define RT_OID_WSC_QUERY_DEFAULT_PROFILE            0x0742
#define RT_OID_WSC_SET_CONN_BY_PROFILE_INDEX        0x0743
#define RT_OID_WSC_SET_ACTION                       0x0744
#define RT_OID_WSC_SET_SSID                         0x0745
#define RT_OID_WSC_SET_PIN_CODE                     0x0746
#define RT_OID_WSC_SET_MODE                         0x0747
#define RT_OID_WSC_SET_CONF_MODE                    0x0748
#define RT_OID_WSC_SET_PROFILE                      0x0749
#define RT_OID_APCLI_WSC_PIN_CODE					0x074A
#define RT_OID_802_11_REPEATER_TXRX_STATISTIC				0x074B
#define	RT_OID_WSC_FRAGMENT_SIZE					0x074D
#define	RT_OID_WSC_V2_SUPPORT						0x074E
#define	RT_OID_WSC_CONFIG_STATUS					0x074F
#define RT_OID_802_11_WSC_QUERY_PROFILE				0x0750
#define RT_OID_WSC_UUID								0x0753
#define RT_OID_WSC_SET_SELECTED_REGISTRAR			0x0754
#define RT_OID_WSC_EAPMSG							0x0755
#define RT_OID_WSC_MANUFACTURER						0x0756
#define RT_OID_WSC_MODEL_NAME						0x0757
#define RT_OID_WSC_MODEL_NO							0x0758
#define RT_OID_WSC_SERIAL_NO						0x0759
#define RT_OID_WSC_READ_UFD_FILE					0x075A
#define RT_OID_WSC_WRITE_UFD_FILE					0x075B
#define RT_OID_WSC_QUERY_PEER_INFO_ON_RUNNING		0x075C
#define RT_OID_WSC_MAC_ADDRESS						0x0760
#define RT_OID_GET_LLTD_ASSO_TABLE                  0x762
#define RT_OID_GET_REPEATER_AP_LINEAGE				0x763
#define OID_802_11R_SUPPORT							0x0780
#define OID_802_11R_MDID							0x0781
#define OID_802_11R_R0KHID							0x0782
#define OID_802_11R_RIC								0x0783
#define OID_802_11R_OTD								0x0784
#define OID_802_11R_INFO							0x0785
#define	RT_OID_802_11R_SUPPORT						(OID_GET_SET_TOGGLE | OID_802_11R_SUPPORT)
#define RT_OID_802_11R_MDID							(OID_GET_SET_TOGGLE | OID_802_11R_MDID)
#define RT_OID_802_11R_R0KHID						(OID_GET_SET_TOGGLE | OID_802_11R_R0KHID)
#define	RT_OID_802_11R_RIC							(OID_GET_SET_TOGGLE | OID_802_11R_RIC)
#define RT_OID_802_11R_OTD							(OID_GET_SET_TOGGLE | OID_802_11R_OTD)
#define RT_OID_802_11R_INFO							(OID_GET_SET_TOGGLE | OID_802_11R_INFO)
#define OID_802_11_CURRENT_CHANNEL_INFO					0x0956
#define OID_GET_CPU_TEMPERATURE 0x09A1
#define OID_WSC_UUID				0x0990
#define OID_SET_SSID				0x0992
#define OID_SET_PSK					0x0993
#define OID_GET_WSC_PROFILES		0x0994
#define OID_GET_MISC_CAP			0x0995
#define OID_GET_HT_CAP				0x0996
#define OID_GET_VHT_CAP				0x0997
#define OID_GET_CHAN_LIST			0x0998
#define OID_GET_OP_CLASS			0x0999
#define OID_GET_BSS_INFO			0x099A
#define OID_GET_AP_METRICS			0x099B
#define OID_GET_NOP_CHANNEL_LIST	0x099C
#define OID_GET_HE_CAP				0x099D
#define OID_GET_WMODE									0x099E
#define OID_GET_ASSOC_REQ_FRAME								0x099F
#define OID_GET_CAC_CAP									0x09A0
#define OID_802_11_CAC_STOP								0x09A1
#define OID_SET_SP_RULE									0x09A2
#define OID_SET_SP_DSCP_TBL								0x09AB
#define OID_802_11_SET_PMK                      0x09A7
#define OID_802_11_GET_DPP_FRAME                0x09A8
#define OID_WF6_CAPABILITY			0x09AA
#define OID_MAP_R3_DPP_URI			0x09AC
#define OID_MAP_R3_1905_SEC_ENABLED		0x09AD
#define OID_MAP_R3_ONBOARDING_TYPE		0x09AE
#define OID_GET_TX_PWR				0x09AF
#define OID_SEND_QOS_ACTION_FRAME		0x09B0
#define OID_SEND_QOS_UP_TUPLE_EXPIRED_NOTIFY	0x09B1
#define OID_GET_SRG_INFO			0x09B2
#define OID_GET_PMK_BY_PEER_MAC		0x09B3
#define OID_GET_WIRELESS_BAND		0x09B4
#define OID_802_11_BCN_TX_CNT		0x09B5
#define OID_GET_DTIM_PERIOD         0x09B6

#ifndef GNU_PACKED
#define GNU_PACKED  __attribute__ ((packed))
#endif
typedef struct GNU_PACKED _channel_info {
	uint8_t	channel;
	uint8_t	channel_idx;
	int	NF;
	unsigned int  rx_time;
	unsigned int	tx_time;
	unsigned int	obss_time;
	unsigned int	channel_busy_time;
	uint8_t	dfs_req;
	unsigned char 	actual_measured_time;
	unsigned int	edcca;
} CHANNEL_INFO, *PCHANNEL_INFO;

struct channel_info {
	unsigned char channel;
	unsigned char bandwidth;
	unsigned char extrach;
};

static uint8_t wmode2std[] = {
	[0] = WIFI_B | WIFI_G,
	[1] = WIFI_B,
	[2] = WIFI_A,
	[3] = WIFI_A | WIFI_B | WIFI_G,
	[4] = WIFI_G,
	[5] = WIFI_B | WIFI_G | WIFI_N | WIFI_A,
	[6] = WIFI_G | WIFI_N,
	[7] = WIFI_G | WIFI_N,
	[8] = WIFI_A | WIFI_N,
	[9] = WIFI_B | WIFI_G | WIFI_N,
	[10] = WIFI_A | WIFI_G | WIFI_N,
	[11] = WIFI_N,
	[12] = WIFI_B | WIFI_G | WIFI_N | WIFI_A | WIFI_AC,
	[13] = WIFI_G | WIFI_N | WIFI_A | WIFI_AC,
	[14] = WIFI_N | WIFI_A | WIFI_AC,
	[15] = WIFI_N | WIFI_AC,
	[16] = WIFI_B | WIFI_G | WIFI_N | WIFI_AX,
	[17] = WIFI_A | WIFI_N | WIFI_AC | WIFI_AX,
	[18] = WIFI_AX,//wifi6e
	[19] = WIFI_G | WIFI_N | WIFI_AX,//wifi6e
	[20] = WIFI_A | WIFI_N | WIFI_AC | WIFI_AX,//wifi6e
	[21] = WIFI_AX,//wifi6e
};
#if 0
static int mtwifi_get_ie(uint8_t *ies, size_t len, uint8_t e, void *data)
{
	int offset = 0;
	int ielen = 0;
	uint8_t eid;

	while (offset <= (len - 2)) {
		eid = ies[offset];
		ielen = ies[offset + 1];
		if (eid != e) {
			offset += ielen + 2;
			continue;
		}

		switch (eid) {
		case IE_BSS_LOAD:
			if (ielen > sizeof(struct wifi_ap_load))
				return -1;
			break;
		case IE_EXT_CAP:
			if (ielen > sizeof(struct wifi_caps_ext))
				return -1;
			break;
		case IE_HT_CAP:
			if (ielen > sizeof(struct wifi_caps_ht))
				return -1;
			break;
		case IE_VHT_CAP:
			if (ielen > sizeof(struct wifi_caps_vht))
				return -1;
			break;
		case IE_RRM:
			if (ielen > sizeof(struct wifi_caps_rm))
				return -1;
			break;
		default:
			break;
		}

		memcpy((uint8_t *)data, &ies[offset + 2], ielen);
		return 0;
	}

	return -ENOENT;
}

static int mtwifi_get_bandwidth_from_ie(uint8_t *ies, size_t len,
						enum wifi_bw *bw)
{
	int offset = 0;
	int ielen = 0;
	uint8_t *p = NULL;
	int cntr = 0;

#ifndef bit
#define bit(n)	(1 << (n))
#endif

	while (offset <= (len - 2)) {
		ielen = ies[offset + 1];
		p = &ies[offset + 2];

		switch (ies[offset]) {
		case 61:   /* HT oper */
			{
				uint8_t *ht_op;
				uint8_t off = 0;

				if (ielen != 22)
					return -1;

				ht_op = p + 1;
				off = ht_op[0] & 0x3;

				if (off == 0)
					*bw = BW20;
				else if (off == 1 || off == 3)
					*bw = BW40;

				if ((ht_op[0] & bit(2)) == 0)
					*bw = BW20;
				cntr++;
			}
			break;
		case 192:  /* VHT oper */
			{
				uint8_t *vht_op;
				uint8_t w, cfs0, cfs1;

				if (ielen < 5)
					return -1;

				vht_op = p;
				w = vht_op[0];
				cfs0 = vht_op[1];
				cfs1 = vht_op[2];

				if (w == 0)
					break;

				if (cfs1 == 0) {
					*bw = BW80;
				} else {
					if (cfs1 - cfs0 == 8)
						*bw = BW160;
					else if (cfs1 - cfs0 == 16)
						*bw = BW8080;
				}
				cntr++;
			}
			break;
		default:
			break;
		}

		if (cntr == 2)
			break;

		offset += ies[offset + 1] + 2;
	}
	return 0;
}
#endif
static int is_valid_ap_iface(const char *ifname)
{
	int valid_iface = 0;
//	int ret;
//	enum wifi_mode mode;
	DIR *d;
	struct dirent *dir;

	d = opendir("/sys/class/net");
	if (WARN_ON(!d))
		return 0;

	while ((dir = readdir(d))) {
		if (strcmp(dir->d_name, ".") == 0 ||
		    strcmp(dir->d_name, "..") == 0)
			continue;
		if (strcmp(ifname, dir->d_name) == 0) {
			valid_iface = 1;
		}
	}
	closedir(d);

	if (!valid_iface)
		return 0;
#if 0
	ret = nlwifi_get_mode(ifname, &mode);
	if (WARN_ON(ret))
		return 0;

	if (mode == WIFI_MODE_AP)  {
		return 1;
	}
#else
	if(!strncmp(ifname, "ra", 2))// ra0 rai0 rax0 rae0
		return 1;
#endif

	return 0;
}

static int mtwifi_get_oper_stds(const char *name, uint8_t *std)
{
	const char *ifname;
	unsigned long phy_mode = 0;
	struct iwreq wrq;
	ifname = name;
	wrq.u.data.length = sizeof(unsigned long);
	wrq.u.data.pointer = (caddr_t) &phy_mode;
	wrq.u.data.flags = RT_OID_GET_PHY_MODE;

	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0)
		return -1;

	*std = wmode2std[phy_mode];
	return 0;
}

#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
void PartialScanNumOfCh(const char *ifname)
{
	int lock;
	char data[64];
	struct iwreq wrq;

	memset(data, 0, sizeof(data));
	strcpy(data, "PartialScanNumOfCh=3");
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	lock = file_lock("sitesurvey");
	if(wl_ioctl(ifname, RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);
		dbg("Site Survey fails\n");
		return;
	}
	file_unlock(lock);
}
#endif

int mtwifi_scan(const char *name, struct scan_param *p)
{
#if 0
	int lock;
	char data[256];
	struct iwreq wrq;

	memset(data, 0, sizeof(data));
	if(p && p->ssid[0]){
		snprintf(data, sizeof(data), "SiteSurvey=%s", p->ssid);
	}else{
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		PartialScanNumOfCh(ifname);
		strcpy(data, "PartialScan=1");//slower, ap still works
#else
		strcpy(data, "SiteSurvey=1");//faster, ap will stop working for 4s
#endif
	}
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	lock = file_lock("sitesurvey");
	if (wl_ioctl(ifname, RTPRIV_IOCTL_GSITESURVEY, &wrq) < 0)
	{
		file_unlock(lock);
		libwifi_dbg("Site Survey fails\n");
		return -1;
	}
	file_unlock(lock);

	libwifi_dbg("Please wait.");
	sleep(4);
	if(p == NULL || p->ssid[0] == 0){
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		libwifi_dbg(".");
		sleep(4);
		if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3) || !strncmp(ifname, "apclii", 6) || !strncmp(ifname, "apclix", 6)){
			libwifi_dbg(".");
			sleep(1);
		}
		if(!strncmp(ifname, "rae", 3) || !strncmp(ifname, "apclie", 6)){
			libwifi_dbg(".");
			sleep(1);
#if defined(RTCONFIG_WIFI6E)
			libwifi_dbg(".");
			sleep(2);
#endif
		}
#endif
	}
	libwifi_dbg(".\n\n");

	return 0;
#else
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_scan(name, p);
#endif
}
#if 0
static int getSiteSurveyVSIEcount(const char *ifname)
{
	char data[64];
	struct iwreq wrq;
	memset(data, 0, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = ASUS_SUBCMD_GETSITESURVEY_VSIE_COUNT;

	if(wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0){
		dbg("errors in getting site survey vie result\n");
	}else{
    	sleep(2);
    	return atoi(data);
	}
	return 0;
}
#endif
static int mtwifi_get_scan_results(const char *name, struct wifi_bss *bsss, int *num)
{
#if 0
	int length = 0, band, ret;
	char prefix[64];
	size_t ie_len;
	uint8_t *ie;
	struct iwreq wrq;
	struct _SITESURVEY_VSIE *result, *pt;
	struct scanresult {
		int num;
		int max;
		struct wifi_bss *bsslist;
	};
	struct scanresult sres = {.num = 0, .max = *num, .bsslist = bsss};
	struct scanresult *scanres = &sres;
	struct wifi_bss *e = scanres->bsslist + scanres->num;
	if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3) || !strncmp(ifname, "apclii", 6) || !strncmp(ifname, "apclix", 6))
		band = 1;
	else if(!strncmp(ifname, "rae", 3) || !strncmp(ifname, "apclie", 6))
		band = 2;
	else
		band = 0;
	snprintf(prefix, sizeof(prefix), "wl%d_", band);
	libwifi_dbg("sres: &bsss = %p (numbss limit = %d)\n", e, scanres->max);
	length = (getSiteSurveyVSIEcount(ifname) / 6 + 1) << 10;
	if(length == 0){
		libwifi_dbg("length = 0\n");
		return -1;
	}
	result = (struct _SITESURVEY_VSIE *)calloc(length, 1);
	if (result == NULL){
		libwifi_dbg("vsie_list calloc failed\n");
		return -1;
	}
	memset(result, 0, length);
	wrq.u.data.length = length;
	wrq.u.data.pointer = result;
	wrq.u.data.flags = ASUS_SUBCMD_GETSITESURVEY_VSIE;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0)
	{
		libwifi_dbg("errors in getting site survey vie result\n");
		free(result);
		return -1;
	}
	if (wrq.u.data.length <= 0 && strlen(wrq.u.data.pointer) <= 0)
	{
		libwifi_dbg("The output var result is NULL\n");
		free(result);
		return -1;
	}
	for(pt = result; pt->Channel; pt++){
		e = scanres->bsslist + scanres->num;
		memcpy(e->ssid, pt->Ssid, sizeof(e->ssid));
		memcpy(e->bssid, pt->Bssid, sizeof(e->bssid));
		e->channel = (uint8_t)pt->Channel;
		if(!strncmp(ifname, "rae", 3) || !strncmp(ifname, "apclie", 6))
#if defined(RTCONFIG_WIFI6E)
			if(e->channel >= 1 && e->channel <= 233)
				e->band = BAND_6;
#else
			e->band = BAND_5;
#endif
		else if(e->channel > 0 && e->channel <= 14)
			e->band = BAND_2;
		else if(e->channel >= 36 && e->channel <= 177)
			e->band = BAND_5;
		e->rssi = (int)pt->Rssi;
		e->beacon_int = nvram_pf_get_int(prefix, "bcn");
		//e->caps.basic.cap = 
		//e->caps.valid |= WIFI_CAP_BASIC_VALID;
		ie_len = pt->vendor_ie_len;
		ie = pt->vendor_ie;
		wifi_get_bss_security_from_ies(e, ie, ie_len);
		libwifi_dbg("       wpa_versions 0x%x pairwise = 0x%x, group = 0x%x akms 0x%x caps 0x%x\n",
				e->rsn.wpa_versions, e->rsn.pair_ciphers, e->rsn.group_cipher, e->rsn.akms, e->rsn.rsn_caps);

		mtwifi_get_ie(ie, ie_len, IE_BSS_LOAD, &e->load);

		ret = mtwifi_get_ie(ie, ie_len, IE_EXT_CAP, &e->caps.ext);
		if (!ret)
			e->caps.valid |= WIFI_CAP_EXT_VALID;

		ret = mtwifi_get_ie(ie, ie_len, IE_HT_CAP, &e->caps.ht);
		if (!ret)
			e->caps.valid |= WIFI_CAP_HT_VALID;

		ret = mtwifi_get_ie(ie, ie_len, IE_VHT_CAP, &e->caps.vht);
		if (!ret)
			e->caps.valid |= WIFI_CAP_VHT_VALID;

		ret = mtwifi_get_ie(ie, ie_len, IE_RRM, &e->caps.rrm);
		if (!ret)
			e->caps.valid |= WIFI_CAP_RM_VALID;

		mtwifi_get_bandwidth_from_ie(ie, ie_len, &e->curr_bw);

		//TODO: wifi supp standards
		//TODO: a-only, b-only, g-only etc.
		if (!!(e->caps.valid & WIFI_CAP_VHT_VALID))
			e->oper_std = WIFI_N | WIFI_AC;
		else if (!!(e->caps.valid & WIFI_CAP_HT_VALID))
			e->oper_std = e->channel > 14 ? (WIFI_A | WIFI_N) :
							(WIFI_B | WIFI_G | WIFI_N);
		else
			e->oper_std = e->channel > 14 ? WIFI_A : (WIFI_B | WIFI_G);


		scanres->num++;
	}
	if (scanres->num >= scanres->max) {
		libwifi_warn("Num scan results > %d !\n", scanres->max);
		free(result);
		return -1;
	}

	free(result);
	*num = scanres->num;
	return 0;
#else
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_scan_results(name, bsss, num);
#endif
}

#if 0
static int ieee80211_frequency_to_channel(int freq)
{
	/* see 802.11-2007 17.3.8.3.2 and Annex J */
	if (freq == 0)
		return 0;
	else if (freq == 2484)
		return 14;
	else if (freq < 2484)
		return (freq - 2407) / 5;
	else if (freq >= 4910 && freq <= 4980)
		return (freq - 4000) / 5;
	else if (freq >= 5000 && freq < 5900)
		return (freq - 5000) / 5;
	else if (freq > 5950 && freq <= 7115)
		return (freq - 5950) / 5;
	else if (freq >= 58320 && freq <= 64800)
		return (freq - 56160) / 2160;
	else
		return 0;
}
#endif

static int mtwifi_get_phy_info(const char *name, struct wifi_radio *radio)
{
	char prefix[64];
	if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3) || !strncmp(name, "apclii", 6) || !strncmp(name, "apclix", 6)){
		radio->supp_band |= BAND_5;
		radio->oper_band = BAND_5;
	}else if(!strncmp(name, "rae", 3) || !strncmp(name, "apclie", 6)){
#if defined(RTCONFIG_WIFI6E)
		radio->supp_band |= BAND_6;
		radio->oper_band = BAND_6;
#else
		radio->supp_band |= BAND_5;
		radio->oper_band = BAND_5;
#endif
	}else{
		radio->supp_band |= BAND_2;
		radio->oper_band = BAND_2;
	}
	if(radio->oper_band & BAND_2)
		radio->supp_std |= WIFI_G | WIFI_B | WIFI_N;
	if(radio->oper_band & BAND_5)
		radio->supp_std |= WIFI_A | WIFI_N | WIFI_AC;
#if defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	if(radio->oper_band & BAND_2)
		radio->supp_std |= WIFI_AX;
	if(radio->oper_band & BAND_5)
		radio->supp_std |= WIFI_AX;
	if(radio->oper_band & BAND_6)
		radio->supp_std |= WIFI_AX;
#endif
#if defined(RTCONFIG_SOC_MT7988)
	if(radio->oper_band & BAND_2)
		radio->supp_std |= WIFI_BE;
	if(radio->oper_band & BAND_5)
		radio->supp_std |= WIFI_BE;
	if(radio->oper_band & BAND_6)
		radio->supp_std |= WIFI_BE;
#endif

	radio->supp_bw |= BIT(BW20);
	radio->supp_bw |= BIT(BW40);
	if(radio->oper_band & BAND_5){
		radio->supp_bw |= BIT(BW80);
#if defined(RTCONFIG_WLMODULE_MT7615E_AP) || defined(RTCONFIG_MT798X)
		radio->supp_bw |= BIT(BW160);
		radio->supp_bw |= BIT(BW8080);
#endif
	}

	if (0 == radio->supp_bw)
		radio->supp_bw |= BIT(BW_UNKNOWN);
	libwifi_dbg(" - oper band %d\n", radio->oper_band);
	libwifi_dbg(" - supp band 0x%x\n", radio->supp_band);
	if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3) || !strncmp(name, "apclii", 6) || !strncmp(name, "apclix", 6))
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	else if(!strncmp(name, "rae", 3) || !strncmp(name, "apclie", 6))
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	else
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	radio->frag = nvram_pf_get_int(prefix, "frag");
	radio->rts = nvram_pf_get_int(prefix, "rts");
	radio->srl = nvram_pf_get_int(prefix, "srl");
	radio->lrl = nvram_pf_get_int(prefix, "lrl");
	radio->beacon_int = nvram_pf_get_int(prefix, "bcn");
	radio->dtim_period = nvram_pf_get_int(prefix, "dtim");
	radio->tx_streams = nvram_pf_get_int(prefix, "HT_TxStream");
	radio->rx_streams = nvram_pf_get_int(prefix, "HT_RxStream");
#if defined(RTCONFIG_WLMODULE_MT7603E_AP)
	if(!strcmp(name, "ra0") || !strcmp(name, "ra1")|| !strcmp(name, "ra2") || !strcmp(name, "apcli0"))
		radio->cac_methods = 0;
	else
#endif
	if(radio->tx_streams == 4 && radio->rx_streams == 4)
		radio->cac_methods |= BIT(WIFI_CAC_MIMO_REDUCED);
	else
		radio->cac_methods |= BIT(WIFI_CAC_CONTINUOUS);

	return 0;
}

static int mtwifi_radio_info2(const char *name, struct wifi_radio *radio)
{
	int idx = 0, i;
	char prefix[64], tmp[16];
	CHANNEL_INFO s = {0};
	const char *ifname;
	struct iwreq wrq;
	libwifi_dbg("[%s] %s called\n", name, __func__);

	mtwifi_get_phy_info(name, radio);

	ifname = name;
	wrq.u.data.length = sizeof(uint64_t);
	wrq.u.data.pointer = (caddr_t)&s;
	wrq.u.data.flags = OID_802_11_CURRENT_CHANNEL_INFO;
	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
		dbg("errors in getting %s CHANNEL_INFO\n", ifname);
		return -1;
	}

	radio->channel = s.channel;
	radio->diag.cca_time = s.actual_measured_time * 1000;
	radio->diag.channel_busy = s.channel_busy_time * 1000;
	radio->diag.tx_airtime = s.tx_time * 1000;
	radio->diag.rx_airtime = s.rx_time * 1000;
	radio->num_iface = WIFI_IFACE_MAX_NUM;
	if(!strcmp(name, "ra0") || !strcmp(name, "ra1")|| !strcmp(name, "ra2") || !strcmp(name, "apcli0")){
		strlcpy(radio->iface[idx].name, "ra0", 15);
		radio->iface[idx].mode = WIFI_MODE_AP;
		for(i = 0; i < MAX_NO_MSSID; i++){
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", 0, i);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				idx++;
				snprintf(tmp, sizeof(tmp), "ra%d", idx);
				strlcpy(radio->iface[idx].name, tmp, 15);
				radio->iface[idx].mode = WIFI_MODE_AP;
			}
		}
		idx++;
		strlcpy(radio->iface[idx].name, "apcli0", 15);
		radio->iface[idx].mode = WIFI_MODE_STA;
	}else if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3) || !strncmp(name, "apclii", 6) || !strncmp(name, "apclix", 6)){
		if(!strncmp(name, "rax", 3) || !strncmp(name, "apclix", 6))
			strlcpy(radio->iface[idx].name, "rax0", 15);
		else
			strlcpy(radio->iface[idx].name, "rai0", 15);
		radio->iface[idx].mode = WIFI_MODE_AP;
		for(i = 0; i < MAX_NO_MSSID; i++){
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", 1, i);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				idx++;
				if(!strncmp(name, "rax", 3) || !strncmp(name, "apclix", 6))
					snprintf(tmp, sizeof(tmp), "rax%d", idx);
				else
					snprintf(tmp, sizeof(tmp), "rai%d", idx);
				strlcpy(radio->iface[idx].name, tmp, 15);
				radio->iface[idx].mode = WIFI_MODE_AP;
			}
		}
		idx++;
		if(!strncmp(name, "rax", 3) || !strncmp(name, "apclix", 6))
			strlcpy(radio->iface[idx].name, "apclix0", 15);
		else
			strlcpy(radio->iface[idx].name, "apclii0", 15);
		radio->iface[idx].mode = WIFI_MODE_STA;
	}else if(!strncmp(name, "rae", 3) || !strncmp(name, "apclie", 6)){
		strlcpy(radio->iface[idx].name, "rae0", 15);
		radio->iface[idx].mode = WIFI_MODE_AP;
		for(i = 0; i < MAX_NO_MSSID; i++){
			snprintf(prefix, sizeof(prefix), "wl%d.%d_", 1, i);
			if(nvram_pf_match(prefix, "bss_enabled", "1")){
				idx++;
				snprintf(tmp, sizeof(tmp), "rae%d", idx);
				strlcpy(radio->iface[idx].name, tmp, 15);
				radio->iface[idx].mode = WIFI_MODE_AP;
			}
		}
		idx++;
		strlcpy(radio->iface[idx].name, "apclie0", 15);
		radio->iface[idx].mode = WIFI_MODE_STA;
	}
	radio->num_iface = idx;

	if (radio->oper_band != BAND_2)
		/* TODO get it from iface combinations */
		radio->cac_methods |= BIT(WIFI_CAC_CONTINUOUS);
	else
		/* Don't report CAC methods for 2.4GHz */
		radio->cac_methods = 0;

	return 0;
}

static int mtwifi_radio_info(const char *name, struct wifi_radio *radio)
{
	uint8_t std = 0;
	int ret;

	ret = mtwifi_radio_info2(name, radio);

	ret |= mtwifi_get_oper_stds(name, &std);
	if (!ret)
		radio->oper_std = std;

	if (radio->beacon_int == 0) {
#if defined(RTCONFIG_WLMODULE_MT7603E_AP)
		if(!strcmp(name, "ra0") || !strcmp(name, "ra1")|| !strcmp(name, "ra2") || !strcmp(name, "apcli0"))
			radio->beacon_int = 0;
		else
#endif
		{
			uint64_t s = 0;
			const char *ifname;
			struct iwreq wrq;
			ifname = name;
			wrq.u.data.length = sizeof(uint64_t);
			wrq.u.data.pointer = (uint64_t *)&s;
			wrq.u.data.flags = OID_802_11_BEACON_PERIOD;
			if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
				dbg("errors in getting %s dtim period\n", ifname);
				return -1;
			}
			radio->beacon_int = (int)s;
		}
	}

	if (radio->dtim_period == 0) {
#if defined(RTCONFIG_MT798X)
		const char *ifname;
		int s = 0;
		struct iwreq wrq;
		ifname = name;
		wrq.u.data.length = sizeof(uint8_t);
		wrq.u.data.pointer = (uint8_t *)&s;
		wrq.u.data.flags = OID_GET_DTIM_PERIOD;
		if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
			dbg("errors in getting %s dtim period\n", ifname);
			return -1;
		}
		radio->dtim_period = s;
#else
		radio->dtim_period = 1;
#endif
	}

	return ret;
}

static int mtwifi_set_channel(const char *name, uint32_t channel, enum wifi_bw bw)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_get_bssid(const char *ifname, uint8_t *bssid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_bssid(ifname, bssid);
}

static int mtwifi_get_ssid(const char *ifname, char *ssid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_ssid(ifname, ssid);
}

static int mtwifi_get_maxrate(const char *ifname, unsigned long *rate)
{
	int ant = 0, max_mcs, bw = 20;
	uint8_t std = 0;
	char prefix[64];
	if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3)){
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	}else if(!strncmp(ifname, "rae", 3)){
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	}else{
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	}
	if(nvram_pf_match(prefix, "bw", "0"))
		bw = 20;
	else if(nvram_pf_match(prefix, "bw", "2"))
		bw = 40;
	else if(nvram_pf_match(prefix, "bw", "3"))
		bw = 80;
	else if(nvram_pf_match(prefix, "bw", "5"))
		bw = 160;
	else if(nvram_pf_match(prefix, "bw", "1") && nvram_pf_match(prefix, "bw_160", "1"))
		bw = 160;
	else if(nvram_pf_match(prefix, "unit", "1"))
		bw = 80;
	else
		bw = 40;
	ant = nvram_pf_get_int(prefix, "HT_RxStream");
	mtwifi_get_oper_stds(ifname, &std);
	if (!!(std & WIFI_AX)) {
		max_mcs = 11;
		*rate = wifi_mcs2rate(max_mcs, bw, ant, WIFI_4xLTF_GI800);
	} else if (!!(std & WIFI_AC)) {
		max_mcs = 9;
		*rate = wifi_mcs2rate(max_mcs, bw, ant, WIFI_SGI);
	} else if (!!(std & WIFI_N)) {
		max_mcs = 8 * ant - 1;
		max_mcs %= 8;
		*rate = wifi_mcs2rate(max_mcs, bw, ant, WIFI_SGI);
	} else if (!!(std & WIFI_G)) {
		*rate = 54;
	} else if (std == WIFI_B) {
		*rate = 11;
	}
	return 0;
}

static int mtwifi_get_channel(const char *name, uint32_t *channel, enum wifi_bw *bw)
{
	struct iwreq wrq;
	struct channel_info info;
	*channel = 0;
	*bw = 0;
	memset(&info, 0, sizeof(struct channel_info));
	wrq.u.data.length = sizeof(struct channel_info);
	wrq.u.data.pointer = (caddr_t) &info;
	wrq.u.data.flags = ASUS_SUBCMD_GCHANNELINFO;

	if (wl_ioctl(name, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0) {
		dbg("wl_ioctl failed on %s (%d)\n", __FUNCTION__, __LINE__);
		return -1;
	}

	*channel = (int)info.channel;

	switch (info.bandwidth) {
		case 0:
			*bw = BW20;
			break;
		case 1:
			*bw = BW40;
			break;
		case 2:
			*bw = BW80;
			break;
		case 3:
			*bw = BW160;
			break;
		default:
			*bw = BW_AUTO;
			break;
	}
	return 0;
}

static int mtwifi_get_bandwidth(const char *ifname, enum wifi_bw *bw)
{
	struct iwreq wrq;
	struct channel_info info;
	*bw = 0;
	memset(&info, 0, sizeof(struct channel_info));
	wrq.u.data.length = sizeof(struct channel_info);
	wrq.u.data.pointer = (caddr_t) &info;
	wrq.u.data.flags = ASUS_SUBCMD_GCHANNELINFO;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_ASUSCMD, &wrq) < 0) {
		dbg("wl_ioctl failed on %s (%d)\n", __FUNCTION__, __LINE__);
		return -1;
	}

	switch (info.bandwidth) {
		case 0:
			*bw = BW20;
			break;
		case 1:
			*bw = BW40;
			break;
		case 2:
			*bw = BW80;
			break;
		case 3:
			*bw = BW160;
			break;
		default:
			*bw = BW_UNKNOWN;
			break;
	}
	return 0;
}

static int mtwifi_get_supp_channels(const char *name, uint32_t *chlist, int *num, const char *cc, enum wifi_band band, enum wifi_bw bw)
{
	int unit = 0;
	char chList[256], prefix[64], chan[6], *next;
	char *country_code;
	if(!strncmp(name, "rax", 3) || !strncmp(name, "rai", 3)){
		unit = 1;
		band = BAND_5;
	}else if(!strncmp(name, "rae", 3)){
		unit = 2;
#if defined(RTCONFIG_WIFI6E)
		band = BAND_6;
#else
		band = BAND_5;
#endif
	}else{
		unit = 0;
		band = BAND_2;
	}
	*num = 0;
	snprintf(prefix, sizeof(prefix), "wl%d_", unit);
	country_code = nvram_pf_safe_get(prefix, "country_code");
	if(get_channel_list_via_driver(unit, chList, sizeof(chList)) <= 0)
	{
		*num = 0;
		return -1;
	}
	else if(get_channel_list_via_country(unit, country_code, chList, sizeof(chList)) <= 0)
	{
		*num = 0;
		return -1;
	}
	foreach_44(chan, chList, next){
		chlist[*num] = (uint32_t)strtol(chan, NULL, 10);
		*num += 1;
	}
	libwifi_dbg("[%s] %s called (max %d band %d)\n", name, __func__, *num, band);
	return 0;
}

static int mtwifi_get_noise(const char *name, int *noise)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	if (nlwifi_get_noise(name, noise))
		/* TODO - for 7615 upgrade backports/mt76 */
		*noise = -90;

	return 0;
}

static int mtwifi_acs(const char *name, struct acs_param *p)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_start_cac(const char *name, int channel, enum wifi_bw bw, enum wifi_cac_method method)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_start_cac(name, channel, bw, method);
}

static int mtwifi_stop_cac(const char *name)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);

	return nlwifi_stop_cac(name);
}

static int mtwifi_vendor_cmd(const char *ifname, uint32_t vid, uint32_t subcmd,	uint8_t *in, int ilen, uint8_t *out, int *olen)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_vendor_cmd(ifname, vid, subcmd, in, ilen, out, olen);
}

static int mtwifi_get_ap_info(const char *ifname, struct wifi_ap *ap)
{
	/* TODO
	*	ap->isolate_enabled
	*/
	struct wifi_bss *bss;
	uint32_t ch = 0;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (!is_valid_ap_iface(ifname))
		return -1;

	bss = &ap->bss;
	memset(bss, 0, sizeof(*bss));
	hostapd_cli_iface_ap_info(ifname, ap);
	nlwifi_get_bssid(ifname, bss->bssid);
	nlwifi_get_ssid(ifname, (char *)bss->ssid);
	if (!strlen((char *)bss->ssid))
		ap->enabled = false;

	nlwifi_get_channel(ifname, &ch, &bss->curr_bw);
	bss->channel = (uint8_t)ch;
	nlwifi_get_bandwidth(ifname, &bss->curr_bw);
	nlwifi_get_supp_stds(ifname, &ap->bss.supp_std);
	hostapd_cli_get_security_cap(ifname, &ap->sec.supp_modes);

	return 0;
}

int mtwifi_driver_info(const char *name, struct wifi_metainfo *info)
{
	char path[256] = {};
	char *pos;
	int fd;

	libwifi_dbg("%s %s called\n", name, __func__);

	snprintf(path, sizeof(path), "/sys/class/pci_bus/0000:00/device/0000:00:00.0/vendor");
	fd = open(path, O_RDONLY);
	if (WARN_ON(fd < 0))
		return -1;

	if (WARN_ON(!read(fd, info->vendor_id, sizeof(info->vendor_id)))) {
		close(fd);
		return -1;
	}
	if ((pos = strchr(info->vendor_id, '\n')))
		*pos = '\0';
	close(fd);

	snprintf(path, sizeof(path), "/sys/class/pci_bus/0000:00/device/0000:00:00.0/device");
	fd = open(path, O_RDONLY);
	if (WARN_ON(fd < 0))
		return -1;

	if (WARN_ON(!read(fd, info->device_id, sizeof(info->device_id)))) {
		close(fd);
		return -1;
	}
	if ((pos = strchr(info->device_id, '\n')))
		*pos = '\0';
	close(fd);

	return 0;
}

static int mtwifi_get_supp_band(const char *name, uint32_t *bands)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_supp_band(name, bands);
}

static int mtwifi_get_oper_band(const char *name, enum wifi_band *band)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_oper_band(name, band);
}

static int mtwifi_get_ifstatus(const char *name, ifstatus_t *f)
{
	if(get_radio_status((char *)name))
		*f |= IFF_UP;
	return 0;
}

static int mtwifi_get_caps(const char *name, struct wifi_caps *caps)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_radio_get_caps(name, caps);
}

static int mtwifi_get_supp_stds(const char *name, uint8_t *std)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_supp_stds(name, std);
	return 0;
}

static int mtwifi_get_country(const char *name, char *alpha2)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_get_country(name, alpha2);
}

static int mtwifi_get_oper_channels(const char *name, uint32_t *chlist, int *num,
				   const char *alpha2, enum wifi_band f, enum wifi_bw b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_get_curr_opclass(const char *name, struct wifi_opclass *o)
{
	libwifi_dbg("[%s, %s] %s called\n", name, __func__);
	return wifi_get_opclass(name, o);
}

static int mtwifi_get_basic_rates(const char *name, int *num, uint32_t *rates_kbps)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_get_oper_rates(const char *name, int *num, uint32_t *rates_kbps)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_get_supp_rates(const char *name, int *num, uint32_t *rates)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_get_stats(const char *name, struct wifi_radio_stats *s)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_scan_ex(const char *name, struct scan_param_ex *sp)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return nlwifi_scan_ex(name, sp);
}

static int mtwifi_get_bss_scan_result(const char *name, uint8_t *bssid, struct wifi_bss_detail *b)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_get_opclass_preferences(const char *name, struct wifi_opclass *opclass, int *num)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return wifi_get_opclass_pref(name, num, opclass);
}

static int mtwifi_simulate_radar(const char *name, struct wifi_radar_args *radar)
{
	if (WARN_ON(!radar))
		return -1;

	libwifi_dbg("[%s] %s called ch:%d, bandwidth:%d, type:0x%x, subband:0x%x\n",
	            name, __func__, radar->channel, radar->bandwidth, radar->type,
	            radar->subband_mask);

	return -1;
}

static int mtwifi_get_param(const char *name, const char *param, int *len, void *val)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_set_param(const char *name, const char *param, int len, void *val)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_get_hwaddr(const char *name, uint8_t *hwaddr)
{
	int s = -1;
	struct ifreq ifr;

	if (name == NULL)
		return -1;

	if ((s = socket(AF_INET, SOCK_RAW, IPPROTO_RAW)) < 0)
		return -1;

	strcpy(ifr.ifr_name, name);
	if (ioctl(s, SIOCGIFHWADDR, &ifr))
		goto error;

	memcpy(hwaddr, ifr.ifr_hwaddr.sa_data, ETHER_ADDR_LEN);
error:
	close(s);
	return 0;
}

static int mtwifi_add_iface(const char *name, enum wifi_mode m, char *argv[])
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_del_iface(const char *name, const char *ifname)
{
	libwifi_dbg("[%s] %s called\n", name, __func__);
	return -1;
}

static int mtwifi_list_iface(const char *name, struct iface_entry *iface, int *num)
{
	char path[256];
	char *ifname;
	struct dirent *p;
	struct iface_entry *entry;
	int count = 0;
	DIR *d;
	if(!strncmp(name, "rax", 3))
		ifname = "rax";
	else if(!strncmp(name, "rai", 3))
		ifname = "rai";
	else if(!strncmp(name, "rae", 3))
		ifname = "rae";
	else if(!strncmp(name, "ra", 2))
		ifname = "ra";
	else if(!strncmp(name, "apclix", 6))
		ifname = "apclix";
	else if(!strncmp(name, "apclii", 6))
		ifname = "apclii";
	else if(!strncmp(name, "apclie", 6))
		ifname = "apclie";
	else if(!strncmp(name, "apcli", 5))
		ifname = "apcli";
	else
		return -1;
	snprintf(path, sizeof(path), "/sys/class/net");
	if (WARN_ON(!(d = opendir(path))))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (strncmp(p->d_name, ifname, strlen(ifname)) || strlen(p->d_name) != (strlen(ifname) + 1))
			continue;
		if (WARN_ON(count >= *num))
			break;

		libwifi_dbg("[%s] iface  %s\n", name, p->d_name);
		entry = &iface[count];
		memset(entry->name, 0, 16);
		memcpy(entry->name, p->d_name, 15);
		if(!strncmp(name, "apcli", 5))
			entry->mode = WIFI_MODE_STA;
		else
			entry->mode = WIFI_MODE_AP;
		count++;
	}

	closedir(d);
	*num = count;

	return 0;
}

static int mtwifi_channels_info(const char *name, struct chan_entry *channel, int *num)
{
	char cc[3] = {0};
	int ret;
	int i;

	ret = nlwifi_channels_info(name, channel, num);
	if (ret)
		return ret;

	WARN_ON(nlwifi_get_country(name, cc));
	if (!strcmp(cc, "US") || !strcmp(cc, "JP"))	/* FIXME-CR: other non-ETSI countries */
		return 0;

	/* Check weather channels - just in case of regulatory issue */
	for (i = 0; i < *num; i++) {
		switch (channel[i].channel) {
		case 120:
		case 124:
		case 128:
			channel[i].cac_time = 600;
			break;
		default:
			break;
		}
	}

	return ret;
}

static int mtwifi_iface_start_wps(const char *ifname, struct wps_param wps)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
//	return hostapd_cli_iface_start_wps(ifname, wps);
	doSystem("iwpriv %s set WscMode=%d", ifname, 2);		// PBC method
	doSystem("iwpriv %s set WscGetConf=%d",ifname, 1);	// Trigger WPS AP to do simple config with WPS Client
	return 0;
}

static int mtwifi_iface_stop_wps(const char *ifname)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
//	return hostapd_cli_iface_stop_wps(ifname);
	doSystem("iwpriv %s set WscStop=1", ifname);			// Stop WPS Process.
	return 0;
}

static int mtwifi_iface_get_wps_status(const char *ifname, enum wps_status *s)
{
	int data = 0;
	struct iwreq wrq;
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
//	return hostapd_cli_iface_get_wps_status(ifname, s);
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) &data;
	wrq.u.data.flags = RT_OID_WSC_QUERY_STATUS;

	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0)
		libwifi_dbg("[%s]errors in getting WSC status\n", __func__);
	return data;
}

static int mtwifi_iface_get_wps_pin(const char *ifname, unsigned long *pin)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_get_wps_ap_pin(ifname, pin);
}

static int mtwifi_iface_set_wps_pin(const char *ifname, unsigned long pin)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_set_wps_ap_pin(ifname, pin);
}

static int mtwifi_iface_get_wps_device_info(const char *ifname, struct wps_device *info)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_get_caps(const char *ifname, struct wifi_caps *caps)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_ap_get_caps(ifname, caps);
}

static int mtwifi_iface_get_mode(const char *ifname, enum wifi_mode *mode)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_mode(ifname, mode);
}

static int mtwifi_iface_get_security(const char *ifname, uint32_t *auth, uint32_t *enc)
{
	char prefix[64] = {0};
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (WARN_ON(!auth) || WARN_ON(!enc))
		return -1;

	*auth = 0;
	*enc = 0;
	if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3))
		snprintf(prefix, sizeof(prefix), "wl%d_", 1);
	else if(!strncmp(ifname, "rae", 3))
		snprintf(prefix, sizeof(prefix), "wl%d_", 2);
	else if(!strncmp(ifname, "ra", 2))
		snprintf(prefix, sizeof(prefix), "wl%d_", 0);
	else if(!strncmp(ifname, "apclix", 6) || !strncmp(ifname, "apclii", 6))
		snprintf(prefix, sizeof(prefix), "wlc%d_", 1);
	else if(!strncmp(ifname, "apclie", 6))
		snprintf(prefix, sizeof(prefix), "wlc%d_", 2);
	else if(!strncmp(ifname, "apcli", 5))
		snprintf(prefix, sizeof(prefix), "wlc%d_", 0);

	if (nvram_pf_match(prefix, "auth_mode_x", "open") && nvram_pf_match(prefix, "wep_x", "0")) {
		*auth = AUTH_OPEN;
		*enc = CIPHER_NONE;
		return 0;
	}
	else if ((nvram_pf_match(prefix, "auth_mode_x", "open") && !nvram_pf_match(prefix, "wep_x", "0"))
		|| nvram_pf_match(prefix, "auth_mode_x", "shared") || nvram_pf_match(prefix, "auth_mode_x", "radius")) {
		*enc = CIPHER_WEP;
		*auth = AUTH_OPEN;
		return 0;
	}
	else if (nvram_pf_match(prefix, "auth_mode_x", "psk"))
		*auth = AUTH_WPAPSK;
	else if (nvram_pf_match(prefix, "auth_mode_x", "psk2")) 
		*auth = AUTH_WPA2PSK;
	else if (nvram_pf_match(prefix, "auth_mode_x", "pskpsk2"))
		*auth = AUTH_WPAPSK | AUTH_WPA2PSK;
	else if (nvram_pf_match(prefix, "auth_mode_x", "wpa"))
		*auth = AUTH_WPA;
	else if (nvram_pf_match(prefix, "auth_mode_x", "wpa2"))
		*auth = AUTH_WPA2;
	else if (nvram_pf_match(prefix, "auth_mode_x", "wpawpa2"))
		*auth = AUTH_WPA | AUTH_WPA2;

	if (nvram_pf_match(prefix, "crypto", "aes"))
		*enc = CIPHER_AES;
	else if (nvram_pf_match(prefix, "crypto", "tkip"))
		*enc = CIPHER_TKIP;
	else if (nvram_pf_match(prefix, "crypto", "tkip+aes"))
		*enc = CIPHER_AES | CIPHER_TKIP;

	if (*enc ==0 && *auth == 0) {
		*enc = CIPHER_UNKNOWN;
		*auth = AUTH_UNKNOWN;
	}
	return 0;
}

static int mtwifi_iface_add_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_add_vendor_ie(ifname, req);
}

static int mtwifi_iface_del_vendor_ie(const char *ifname, struct vendor_iereq *req)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_del_vendor_ie(ifname, req);
}

static int mtwifi_iface_get_vendor_ies(const char *ifname, struct vendor_ie *ies, int *num_ies)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_radio_list(struct radio_entry *radio, int *num)
{
	struct dirent *p;
	struct radio_entry *entry;
	int count = 0;
	DIR *d;

	if (WARN_ON(!(d = opendir("/sys/class/net"))))
		return -1;

	while ((p = readdir(d))) {
		if (!strcmp(p->d_name, "."))
			continue;
		if (!strcmp(p->d_name, ".."))
			continue;
		if (WARN_ON(count >= *num))
			break;
		if(strncmp(p->d_name, "ra", 2) && strncmp(p->d_name, "apcli", 5))
			continue;

		libwifi_dbg("radio: %s\n", p->d_name);
		entry = &radio[count];
		memset(entry->name, 0, 16);
		memcpy(entry->name, p->d_name, 15);
		count++;
	}

	closedir(d);
	*num = count;

	return 0;
}

static int mtwifi_iface_get_param(const char *ifname, const char *param, int *len, void *val)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_set_param(const char *ifname, const char *param, int len, void *val)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_subscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	enum wifi_mode mode;
	int ret = -1;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = mtwifi_iface_get_mode(ifname, &mode);
	if (WARN_ON(ret))
		return ret;

	switch (mode) {
	case WIFI_MODE_AP:
		ret = hostapd_ubus_iface_subscribe_frame(ifname, type, stype);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static int mtwifi_iface_unsubscribe_frame(const char *ifname, uint8_t type, uint8_t stype)
{
	enum wifi_mode mode;
	int ret = -1;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = mtwifi_iface_get_mode(ifname, &mode);
	if (WARN_ON(ret))
		return ret;

	switch (mode) {
	case WIFI_MODE_AP:
		ret = hostapd_ubus_iface_unsubscribe_frame(ifname, type, stype);
		break;
	default:
		ret = -1;
		break;
	}

	return ret;
}

static int mtwifi_iface_set_4addr(const char *ifname, bool enable)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_get_4addr(const char *ifname, bool *enable)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_get_4addr(ifname, enable);
}

static int mtwifi_iface_get_4addr_parent(const char *ifname, char *parent)
{
	enum wifi_mode mode;
	int ret;
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = nlwifi_get_mode(ifname, &mode);
	if (ret)
		return ret;

	if (mode != WIFI_MODE_AP_VLAN)
		return -1;

	ret = hostapd_cli_get_4addr_parent((char *)ifname, parent);
	return ret;
}

static int mtwifi_iface_set_vlan(const char *ifname, struct vlan_param vlan)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_get_stats(const char *ifname, struct wifi_ap_stats *s)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	memset(s, 0, sizeof(*s));
	return 0;
}

static int mtwifi_iface_get_beacon_ies(const char *ifname, uint8_t *ies, int *len)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_get_beacon_ies(ifname, ies, (size_t *)len);
}

static int mtwifi_iface_get_assoclist(const char *ifname, uint8_t *stas, int *num_stas)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (is_valid_ap_iface(ifname)) {
		return hostapd_iface_get_assoclist(ifname, stas, num_stas);
	}
	return -1;
}

static int mtwifi_iface_get_sta_info(const char *ifname, uint8_t *addr, struct wifi_sta *info)
{
	const char *iface;
	char ifname_wds[256] = { 0 };
	int ret = 0;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	if (hostapd_cli_is_wds_sta(ifname, addr, ifname_wds, sizeof(ifname_wds))) {
		libwifi_dbg("[%s] has virtual AP iface[%s]\n", ifname, ifname_wds);
		iface = ifname_wds;
	} else {
		iface = ifname;
	}
	ret = nlwifi_get_sta_info(iface, addr, info);
	if (!ret) {
		ret = hostapd_cli_iface_get_sta_info(ifname, addr, info);
	}

	if (!ret) {
		ret = mtwifi_get_maxrate(ifname, (unsigned long *) &info->maxrate);
	}

	return ret;
}

static int mtwifi_iface_get_sta_stats(const char *ifname, uint8_t *addr, struct wifi_sta_stats *s)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_disconnect_sta(const char *ifname, uint8_t *sta, uint16_t reason)
{
	libwifi_dbg("[%s] %s called reason[%d]\n", ifname, __func__, reason);
	return hostapd_cli_disconnect_sta(ifname, sta, reason);
}

static int mtwifi_iface_restrict_sta(const char *ifname, uint8_t *sta, int enable)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta_config *cfg)
{
	libwifi_dbg("[%s] %s called " MACSTR " %s\n", ifname, __func__,
		    MAC2STR(sta), cfg->enable ? "enable" : "disable");

	/* Enable event driven probe req monitoring */
	return hostapd_ubus_iface_monitor_sta(ifname, sta, cfg);
}

static int mtwifi_iface_get_monitor_sta(const char *ifname, uint8_t *sta, struct wifi_monsta *mon)
{
	struct wifi_sta info = {};
	int ret;
	int i;

	libwifi_dbg("[%s] %s " MACSTR " called\n", ifname, __func__, MAC2STR(sta));

	memset(mon, 0, sizeof(*mon));

	/* Get RSSI from nlwifi (from data frames) for connected stations */
	ret = nlwifi_get_sta_info(ifname, sta, &info);
	if (WARN_ON(ret))
		return ret;

	/* Check if connected */
	if (memcmp(mon->macaddr, info.macaddr, 6)) {
		WARN_ON(sizeof(mon->rssi) != sizeof(info.rssi));
		for (i = 0; i < sizeof(mon->rssi); i++)
			mon->rssi[i] = info.rssi[i];

		mon->rssi_avg = info.rssi_avg;
		mon->last_seen = info.idle_time;
		memcpy(mon->macaddr, info.macaddr, 6);
		return ret;
	}

	/* Worst case for not connected station - get from hostapd (probes) */
	return hostapd_ubus_iface_get_monitor_sta(ifname, sta, mon);
}

static int mtwifi_iface_get_monitor_stas(const char *ifname, struct wifi_monsta *stas, int *num)
{
	struct wifi_monsta *mon;
	uint8_t sta[6];
	int ret;
	int i;

	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	ret = hostapd_ubus_iface_get_monitor_stas(ifname, stas, num);
	if (WARN_ON(ret))
		return ret;

	for (i = 0; i < *num; i++) {
		mon = &stas[i];
		memcpy(sta, mon->macaddr, sizeof(sta));
		WARN_ON(mtwifi_iface_get_monitor_sta(ifname, sta, mon));
	}

	return ret;
}

static int mtwifi_iface_add_neighbor(const char *ifname, struct nbr nbr)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_add_neighbor(ifname, &nbr, sizeof(nbr));
}

static int mtwifi_iface_del_neighbor(const char *ifname, unsigned char *bssid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_del_neighbor(ifname, bssid);
}

static int mtwifi_iface_get_neighbor_list(const char *ifname, struct nbr *nbr, int *nr)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_iface_get_neighbor_list(ifname, nbr, nr);
}

static int mtwifi_iface_req_beacon_report(const char *ifname, uint8_t *sta,
					struct wifi_beacon_req *param, size_t param_sz)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	if (param_sz > 0 && param != NULL)
		return hostapd_cli_iface_req_beacon(ifname, sta, param, param_sz);

	/* No params passed - use default (minimal) configuration */
	return hostapd_cli_iface_req_beacon_default(ifname, sta);
}

static int mtwifi_iface_get_beacon_report(const char *ifname, uint8_t *sta,
				   struct sta_nbr *snbr, int *nr)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

#define MAX_BTM_NBR_NUM 64
static int mtwifi_iface_req_bss_transition(const char *ifname, unsigned char *sta,
				    int bsss_nr, unsigned char *bsss, unsigned int tmo)
{
	struct bss_transition_params params = { 0 };
	struct nbr nbss[MAX_BTM_NBR_NUM] = {};
	unsigned char *bss;
	int i;

	libwifi_dbg("[%s] %s called " MACSTR " nr %d\n", ifname, __func__,
		    MAC2STR(sta), bsss_nr);

	if (bsss_nr > MAX_BTM_NBR_NUM)
		return -1;

	params.valid_int = (uint8_t)tmo;
	params.pref = bsss_nr ? 1 : 0;
	params.disassoc_imminent = 1;

	for (i = 0; i < bsss_nr; i++) {
		bss = bsss + i * 6;
		memcpy(&nbss[i].bssid, bss, 6);
	}

	return hostapd_cli_iface_req_bss_transition(ifname, sta, bsss_nr, nbss, &params);
}

static int mtwifi_iface_req_btm(const char *ifname, unsigned char *sta,
			 int bsss_nr, struct nbr *bsss, struct wifi_btmreq *b)
{
	struct bss_transition_params params = { 0 };

	libwifi_dbg("[%s] %s called " MACSTR " nr %d\n", ifname, __func__,
		    MAC2STR(sta), bsss_nr);

	params.valid_int = b->validity_int;
	params.disassoc_timer = b->disassoc_tmo;

	if (b->mode & WIFI_BTMREQ_PREF_INC || bsss_nr)
		params.pref = 1;
	if (b->mode & WIFI_BTMREQ_ABRIDGED)
		params.abridged = 1;
	if (b->mode & WIFI_BTMREQ_DISASSOC_IMM)
		params.disassoc_imminent = 1;
	if (b->mode & WIFI_BTMREQ_BSSTERM_INC)
		params.bss_term = b->bssterm_dur;

	return hostapd_cli_iface_req_bss_transition(ifname, sta, bsss_nr, bsss, &params);
}

static int mtwifi_iface_get_11rkeys(const char *ifname, unsigned char *sta, uint8_t *r1khid)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_set_11rkeys(const char *ifname, struct fbt_keys *fk)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int get_sec_chan_offset(uint32_t freq)
{
	int ht40plus[] = { 5180, 5220, 5260, 5300, 5500, 5540, 5580, 5620, 5660, 5745, 5785, 5920 };
	int i;

	for (i = 0; i < ARRAY_SIZE(ht40plus); i++) {
		if (freq == ht40plus[i])
			return 1;
	}

	return -1;
}

static uint32_t get_cf1(uint32_t freq, uint32_t bw)
{
	uint32_t cf1 = 0;
	int cf1_bw80[] = { 5210, 5290, 5530, 5610, 5690, 5775 };
	int cf1_bw160[] = { 5250, 5570 };
	int i;

	/* TODO check/fix 6GHz */
	switch (bw) {
	case 160:
		for (i = 0; i < ARRAY_SIZE(cf1_bw160); i++) {
			if (freq >= cf1_bw160[i] - 70 &&
			    freq <= cf1_bw160[i] + 70) {
				cf1 = cf1_bw160[i];
				break;
			}
		}
		break;
	case 80:
		for (i = 0; i < ARRAY_SIZE(cf1_bw80); i++) {
			if (freq >= cf1_bw80[i] - 30 &&
			    freq <= cf1_bw80[i] + 30) {
				cf1 = cf1_bw80[i];
				break;
			}
		}
		break;

	default:
		cf1 = freq;
	}

	return cf1;
}

static int mtwifi_iface_chan_switch(const char *ifname, struct chan_switch_param *param)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	switch (param->bandwidth) {
	case 160:
	case 80:
		if (!param->sec_chan_offset)
			param->sec_chan_offset = get_sec_chan_offset(param->freq);
		if (!param->cf1)
			param->cf1 = get_cf1(param->freq, param->bandwidth);
		break;
	case 8080:
		if (WARN_ON(!param->cf1))
			return -1;
		if (WARN_ON(!param->cf2))
			return -1;
		break;
	case 40:
		if (!WARN_ON(!param->sec_chan_offset))
			return -1;
		break;
	default:
		break;
	}

	return hostapd_cli_iface_chan_switch(ifname, param);
}

static int mtwifi_iface_mbo_disallow_assoc(const char *ifname, uint8_t reason)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return hostapd_cli_mbo_disallow_assoc(ifname, reason);
}

static int mtwifi_iface_ap_set_state(const char *ifname, bool up)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);

	return hostapd_cli_ap_set_state(ifname, up);
}

/* sta interface ops */
static int mtwifi_iface_sta_info(const char *ifname, struct wifi_sta *sta)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	int ret = 0;
	ret = nlwifi_sta_info(ifname, sta);
	if(ret) return -1;

	WARN_ON(if_gethwaddr(ifname, sta->macaddr));
	ret = supplicant_sta_info(ifname, sta);

	return ret;
}

static int mtwifi_iface_sta_get_stats(const char *ifname, struct wifi_sta_stats *s)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return nlwifi_sta_get_stats(ifname, s);
}

static int mtwifi_iface_sta_get_ap_info(const char *ifname, struct wifi_bss *info)
{
	libwifi_dbg("[%s] %s called\n", ifname, __func__);
	return -1;
}

static int mtwifi_iface_sta_disconnect_ap(const char *ifname, uint32_t reason)
{
	libwifi_dbg("[%s] %s called reason %d\n", ifname, __func__, reason);
	return hostapd_cli_sta_disconnect_ap(ifname, reason);
}

const struct wifi_driver mt_driver = {
	.name = "ra,rai,rax,rae,apcli,apclii,apclix,apclie",
	.info = mtwifi_driver_info,
	/* Radio/phy callbacks */
	.radio.info = mtwifi_radio_info,
	.get_supp_band = mtwifi_get_supp_band,
	.get_oper_band = mtwifi_get_oper_band,
	.radio.get_ifstatus = mtwifi_get_ifstatus,
	.radio.get_caps = mtwifi_get_caps,
	.radio.get_supp_stds = mtwifi_get_supp_stds,
	.get_oper_stds = mtwifi_get_oper_stds,

	.get_country = mtwifi_get_country,
	.get_channel = mtwifi_get_channel,
	.set_channel = mtwifi_set_channel,
	.get_supp_channels = mtwifi_get_supp_channels,
	.get_oper_channels = mtwifi_get_oper_channels,

	.get_curr_opclass = mtwifi_get_curr_opclass,

	.get_bandwidth = mtwifi_get_bandwidth,
	.get_supp_bandwidths = nlwifi_get_supp_bandwidths,
	.get_maxrate = mtwifi_get_maxrate,
	.radio.get_basic_rates = mtwifi_get_basic_rates,
	.radio.get_oper_rates = mtwifi_get_oper_rates,
	.radio.get_supp_rates = mtwifi_get_supp_rates,
	.radio.get_stats = mtwifi_get_stats,

	.scan = mtwifi_scan,
	.scan_ex = mtwifi_scan_ex,
	.get_scan_results = mtwifi_get_scan_results,
	.get_bss_scan_result = mtwifi_get_bss_scan_result,

	.get_noise = mtwifi_get_noise,

	.acs = mtwifi_acs,
	.start_cac = mtwifi_start_cac,
	.stop_cac = mtwifi_stop_cac,
	.get_opclass_preferences = mtwifi_get_opclass_preferences,
	.simulate_radar = mtwifi_simulate_radar,

	.radio.get_param = mtwifi_get_param,
	.radio.set_param = mtwifi_set_param,

	.radio.get_hwaddr = mtwifi_get_hwaddr,

	.add_iface = mtwifi_add_iface,
	.del_iface = mtwifi_del_iface,
	.list_iface = mtwifi_list_iface,
	.channels_info = mtwifi_channels_info,

	/* Interface/vif common callbacks */
	.iface.start_wps = mtwifi_iface_start_wps,
	.iface.stop_wps = mtwifi_iface_stop_wps,
	.iface.get_wps_status = mtwifi_iface_get_wps_status,
	.iface.get_wps_pin = mtwifi_iface_get_wps_pin,
	.iface.set_wps_pin = mtwifi_iface_set_wps_pin,
	.iface.get_wps_device_info = mtwifi_iface_get_wps_device_info,

	.iface.get_caps = mtwifi_iface_get_caps,
	.iface.get_mode = mtwifi_iface_get_mode,
	.iface.get_security = mtwifi_iface_get_security,

	.iface.add_vendor_ie = mtwifi_iface_add_vendor_ie,
	.iface.del_vendor_ie = mtwifi_iface_del_vendor_ie,
	.iface.get_vendor_ies = mtwifi_iface_get_vendor_ies,

	.iface.get_param = mtwifi_iface_get_param,
	.iface.set_param = mtwifi_iface_set_param,
	.vendor_cmd = mtwifi_vendor_cmd,
	.iface.subscribe_frame = mtwifi_iface_subscribe_frame,
	.iface.unsubscribe_frame = mtwifi_iface_unsubscribe_frame,
	.set_4addr = mtwifi_iface_set_4addr,
	.get_4addr = mtwifi_iface_get_4addr,
	.get_4addr_parent = mtwifi_iface_get_4addr_parent,
	.set_vlan = mtwifi_iface_set_vlan,

        /* Interface/vif ap callbacks */
	.iface.ap_info = mtwifi_get_ap_info,
	.get_bssid = mtwifi_get_bssid,
	.get_ssid = mtwifi_get_ssid,

	.iface.get_stats = mtwifi_iface_get_stats,
	.get_beacon_ies = mtwifi_iface_get_beacon_ies,

	.get_assoclist = mtwifi_iface_get_assoclist,
	.get_sta_info = mtwifi_iface_get_sta_info,
	.get_sta_stats = mtwifi_iface_get_sta_stats,
	.disconnect_sta = mtwifi_iface_disconnect_sta,
	.restrict_sta = mtwifi_iface_restrict_sta,
	.monitor_sta = mtwifi_iface_monitor_sta,
	.get_monitor_sta = mtwifi_iface_get_monitor_sta,
	.get_monitor_stas = mtwifi_iface_get_monitor_stas,
	.iface.add_neighbor = mtwifi_iface_add_neighbor,
	.iface.del_neighbor = mtwifi_iface_del_neighbor,
	.iface.get_neighbor_list = mtwifi_iface_get_neighbor_list,
	.iface.req_beacon_report = mtwifi_iface_req_beacon_report,
	.iface.get_beacon_report = mtwifi_iface_get_beacon_report,

	.iface.req_bss_transition = mtwifi_iface_req_bss_transition,
	.iface.req_btm = mtwifi_iface_req_btm,

	.iface.get_11rkeys = mtwifi_iface_get_11rkeys,
	.iface.set_11rkeys = mtwifi_iface_set_11rkeys,

	.chan_switch = mtwifi_iface_chan_switch,
	.mbo_disallow_assoc = mtwifi_iface_mbo_disallow_assoc,
	.ap_set_state = mtwifi_iface_ap_set_state,

         /* Interface/vif sta callbacks */
	.iface.sta_info = mtwifi_iface_sta_info,
	.iface.sta_get_stats = mtwifi_iface_sta_get_stats,
	.iface.sta_get_ap_info = mtwifi_iface_sta_get_ap_info,
	.iface.sta_disconnect_ap = mtwifi_iface_sta_disconnect_ap,

	.radio_list = mtwifi_radio_list,
	.register_event = nlwifi_register_event,
	.unregister_event = nlwifi_unregister_event,
	.recv_event = nlwifi_recv_event,
};
