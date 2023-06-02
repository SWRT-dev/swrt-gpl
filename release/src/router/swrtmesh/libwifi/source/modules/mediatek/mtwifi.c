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
void PartialScanNumOfCh(int band)
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
	if(wl_ioctl(get_wififname(band), RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);
		dbg("Site Survey fails\n");
		return;
	}
	file_unlock(lock);
}
#endif

int mtwifi_scan(const char *ifname, struct scan_param *p)
{
	int band;
	char data[8192];
	struct iwreq wrq;
	int lock;
	memset(data, 0, sizeof(data));
	if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3))
		band = 1;
	else if(!strncmp(ifname, "rae", 3))
		band = 2;
	else
		band = 0;
	if(p && p->ssid[0]){
		snprintf(data, sizeof(data), "SiteSurvey=%s", p->ssid);
	}else{
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
	PartialScanNumOfCh(band);
	strcpy(data, "PartialScan=1");//slower, ap still works
#else
	strcpy(data, "SiteSurvey=1");//faster, ap will stop working for 4s
#endif
	}
	wrq.u.data.length = strlen(data) + 1;
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	lock = file_lock("sitesurvey");
	if (wl_ioctl(ifname, RTPRIV_IOCTL_SET, &wrq) < 0)
	{
		file_unlock(lock);

		libwifi_dbg("Site Survey fails\n");
		return 0;
	}
	file_unlock(lock);

	libwifi_dbg("Please wait");
	sleep(1);
	libwifi_dbg(".");
	sleep(1);
	libwifi_dbg(".");
	sleep(1);
	libwifi_dbg(".");
	sleep(1);
	if(p == NULL || p->ssid[0] == 0){
#if defined(RALINK_DBDC_MODE) || defined(RTCONFIG_WLMODULE_MT7915D_AP) || defined(RTCONFIG_MT798X)
		libwifi_dbg(".");
		sleep(4);
		if(band){
			libwifi_dbg(".");
			sleep(1);
#if defined(RTCONFIG_WIFI6E)
			libwifi_dbg(".");
			sleep(3);
#endif
		}
#endif
	}
	libwifi_dbg(".\n\n");

	return 0;
}

static int mtwifi_get_scan_results(const char *ifname,
				struct wifi_bss *bsss,
				int *num)
{
	int hdrLen = 0;
	int i = 0, apCount = 0;
	char *data;
	int size = 16380;
	char header[128];
	struct iwreq wrq;
	SSA *ssap;
	*num = 0;
	data = malloc(size);
	if (data == NULL)
		return -1;
	memset(data, 0, size);
	strcpy(data, "");
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = data;
	wrq.u.data.flags = 0;

	if (wl_ioctl(ifname, RTPRIV_IOCTL_GSITESURVEY, &wrq) < 0)
	{
		libwifi_dbg("errors in getting site survey result\n");
		free(data);
		return -1;
	}
	memset(header, 0, sizeof(header));
	hdrLen = sprintf(header, "%-4s%-33s%-20s%-23s%-9s%-12s%-7s%-3s%-4s%-5s\n", "Ch", "SSID", "BSSID", "Security", "Signal(%)", "W-Mode", "ExtCH", "NT", "WPS", "DPID");
	libwifi_dbg("\n%s", header);
	if (wrq.u.data.length > 0 && strlen(wrq.u.data.pointer) > 0)
	{
		ssap=(SSA *)(wrq.u.data.pointer + hdrLen + 1);
		int len = strlen(wrq.u.data.pointer + hdrLen + 1);
		char *sp, *op;
 		op = sp = wrq.u.data.pointer + hdrLen + 1;

		while (*sp && ((len - (sp-op)) >= 0))
		{
			ssap->SiteSurvey[i].channel[3] = '\0';
			ssap->SiteSurvey[i].ssid[32] = '\0';
			ssap->SiteSurvey[i].bssid[19] = '\0';
			ssap->SiteSurvey[i].security[22] = '\0';
			ssap->SiteSurvey[i].signal[8] = '\0';
			ssap->SiteSurvey[i].wmode[11] = '\0';
			ssap->SiteSurvey[i].extch[6] = '\0';
			ssap->SiteSurvey[i].nt[2] = '\0';
			ssap->SiteSurvey[i].wps[3] = '\0';
			ssap->SiteSurvey[i].dpid[4] = '\0';
			sp+=hdrLen;
			apCount=++i;
		}
		for (i=0;i<apCount;i++)
		{
			libwifi_dbg("%-4s%-33s%-20s%-23s%-9s%-12s%-7s%-3s%-4s%-5s\n",
			ssap->SiteSurvey[i].channel,
			(char*)ssap->SiteSurvey[i].ssid,
			ssap->SiteSurvey[i].bssid,
			ssap->SiteSurvey[i].security,
			ssap->SiteSurvey[i].signal,
			ssap->SiteSurvey[i].wmode,
			ssap->SiteSurvey[i].extch,
			ssap->SiteSurvey[i].nt,
			ssap->SiteSurvey[i].wps,
			ssap->SiteSurvey[i].dpid
			);
		}
	}
	free(data);
	return 0;
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
	char data[12];
	struct iwreq wrq;

	memset(data, 0x00, sizeof(data));
	wrq.u.data.length = sizeof(data);
	wrq.u.data.pointer = (caddr_t) data;
	wrq.u.data.flags = OID_802_11_BSSID;

	if (wl_ioctl(ifname, RT_PRIV_IOCTL, &wrq) < 0) {
		dbg("errors in getting %s bssid\n", ifname);
		return -1;
	}
	memcpy(bssid, wrq.u.data.pointer, wrq.u.data.length);
	return 0;
}

static int mtwifi_get_ssid(const char *ifname, char *ssid)
{
	if(!strncmp(ifname, "rax", 3) || !strncmp(ifname, "rai", 3))
		strlcpy(ssid, nvram_safe_get("wl1_ssid"), 32);
	else if(!strncmp(ifname, "rae", 3))
		strlcpy(ssid, nvram_safe_get("wl2_ssid"), 32);
	else
		strlcpy(ssid, nvram_safe_get("wl0_ssid"), 32);
	return 0;
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

const struct wifi_driver mt_driver = {
	.name = "ra,rai,rax,rae,apcli,apclii,apclix,apclie",
	.scan = mtwifi_scan,
	.get_scan_results = mtwifi_get_scan_results,
	.get_bssid = mtwifi_get_bssid,
	.get_ssid = mtwifi_get_ssid,
	.get_maxrate = mtwifi_get_maxrate,
	.get_channel = mtwifi_get_channel,
	.radio.info = mtwifi_radio_info,
	.get_oper_stds = mtwifi_get_oper_stds,
	.set_channel = mtwifi_set_channel,
};
