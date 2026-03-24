/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
* $Id$
*
*
*
*/

#ifndef __MTLK_COREUI_H__
#define __MTLK_COREUI_H__

#include "mtlkerr.h"
#include "txmm.h"
#include "channels.h"
#include "mtlkqos.h"
#include "bitrate.h"
#include "dataex.h"
#include "mtlkdfdefs.h"
#include "mtlk_ab_manager.h"
#include "mtlk_coc.h"
#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
#include "mtlk_pcoc.h"
#endif /*CPTCFG_IWLWAV_PMCU_SUPPORT*/
#include "ta.h"
#include "fw_recovery.h"
/* todo: check if can be removed */
#include "mtlkwssa_drvinfo.h"
#include "vendor_cmds.h"
#include "mhi_umi_propr.h"

#include <uapi/linux/nl80211.h> /* for conversions from NL80211 to our values */
#include <net/cfg80211.h> /* for conversions from ieee80211 to our values */

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

#define LOG_LOCAL_GID   GID_DFUSER
#define LOG_LOCAL_FID   3
/* Renaming the constants common to 5G and 6G without changing the Kernel defines */
#define HE_5G_6G_PHY_CAP0_CHAN_WIDTH_SET_40MHZ_80MHZ   IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G
#define HE_5G_6G_PHY_CAP0_CHAN_WIDTH_SET_160MHZ        IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G
#define HE_5G_6G_PHY_CAP0_CHAN_WIDTH_SET_80PLUS80_MHZ  IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_80PLUS80_MHZ_IN_5G

/**
*\file mtlk_coreui.h

*\brief Core interface for DF UI module

*\defgroup COREUI Core interface for DF UI
*\{

*/

#define MTLK_CHANNEL_NOT_USED 0
#define MTLK_ESSID_MAX_SIZE   (IEEE80211_MAX_SSID_LEN + 1)
#define COUNTRY_CODE_DFLT     "US"

#define WAVE_RADIO_OFF 0
#define WAVE_RADIO_ON  1

#define MIN_RSSI        (-128)
#define MIN_NOISE       (-128)
#define RSSI_INVALID    MIN_RSSI
#define NOISE_INVALID   MIN_NOISE

#define MTLK_PHY_METRIC_IS_INVALID(__val) \
        ((GEN5_PHY_METRIC_INVALID == (__val)) || (GEN6_PHY_METRIC_INVALID == (uint8)(__val)))

#define MTLK_MIN_RBM 0x01
#define MTLK_MAX_RBM 0xFF

#define MTLK_MIN_BFPERIOD 100   /* Beamforming Period in secs */
#define MTLK_MAX_BFPERIOD 1000

#define MTLK_MAX_TX_AMPDU_DENSITY 7
#define MTLK_MIN_TX_AMPDU_DENSITY 0

#define MTLK_INTERFDET_DISABLED_THRESHOLD 1

#define MTLK_PARAM_DB_INVALID_UINT8  MAX_UINT8
#define MTLK_PARAM_DB_INVALID_UINT16 MAX_UINT16
#define MTLK_PARAM_DB_INVALID_UINT32 MAX_UINT32

/* Param DB supports values of size up to uint32. Thus INVALID_UINT32 will be used for uint64 too */
/* NOTE: This macro may produce warning:
   < comparison is always false due to limited range of data type [-Wtype-limits] >
   This is expected! */
#define MTLK_PARAM_DB_VALUE_IS_INVALID(param) ({ \
        int __res; \
        MTLK_PRAGMA(GCC diagnostic push); \
        MTLK_PRAGMA(GCC diagnostic ignored "-Wtype-limits"); \
        __res = ((sizeof(param) == sizeof(uint8)  ? (((uint8)  param) == MTLK_PARAM_DB_INVALID_UINT8)  : \
                 (sizeof(param) == sizeof(uint16) ? (((uint16) param) == MTLK_PARAM_DB_INVALID_UINT16) : \
               /* both sizes uint32 and uint64 */   (((uint32) param) == MTLK_PARAM_DB_INVALID_UINT32)))); \
        __res; \
        MTLK_PRAGMA(GCC diagnostic pop); \
        })

/* Special case for Param DB - Radar Detection Threshold */
#define MTLK_PARAM_DB_RADAR_DETECTION_RSSI_TH_IS_INVALID(param) \
        (WAVE_RADAR_DETECTION_RSSI_TH_INVALID == param)

#define MTLK_PARAM_DB_BF_MODE_IS_INVALID(param) (BF_NUMBER_OF_MODES == param)

/* adjust according to maximum SID supported by available platforms */
#define WAVE_MAX_SID                     GEN6_MAX_SID
#define WAVE_MAX_STA_ID                  (GEN6_MAX_SID-1)
#define WAVE_MIN_STA_ID                  0

/* Broadcast TWT definitions */
#define NUM_ADVERTISE_BTWT_COMMON_PARAMS 2
#define NUM_ADVERTISE_BTWT_SP_PARAMS     6
#define NUM_TERMINATE_BTWT_PARAMS        1
#define ADVERTISE_BTWT_SP_PARAMS_OFFSET  NUM_ADVERTISE_BTWT_COMMON_PARAMS
#define MIN_NUM_ADVERTISE_BTWT_PARAMS    (NUM_ADVERTISE_BTWT_COMMON_PARAMS + NUM_ADVERTISE_BTWT_SP_PARAMS)
#define MAX_NUM_ADVERTISE_BTWT_PARAMS    (NUM_ADVERTISE_BTWT_COMMON_PARAMS + (NUM_ADVERTISE_BTWT_SP_PARAMS*MAX_NUM_OF_BC_TWT_SP_TO_ADD))

/* TWT Teardown */
#define MIN_NUM_TX_TWT_TEARDOWN_PARAMS   2
#define MAX_NUM_TX_TWT_TEARDOWN_PARAMS   4
#define DEFAULT_AGREEMENT_TYPE           MIN_AGREEMENT_TYPE_INDIVIDUAL
#define DEFAULT_TWT_ID                   MIN_INDIVIDUAL_FLOW_ID

void __MTLK_IFUNC
mtlk_core_handle_tx_data(mtlk_vap_handle_t vap_handle, mtlk_nbuf_t *nbuf);

void __MTLK_IFUNC
mtlk_core_handle_tx_ctrl(mtlk_vap_handle_t    vap_handle,
                         mtlk_user_request_t *req,
                         uint32               id,
                         mtlk_clpb_t         *data);

typedef enum
{
  WAVE_CORE_REQ_ACTIVATE_OPEN = MTLK_CORE_ABILITIES_START, /*!< Activate the core for AP */
  WAVE_CORE_REQ_SET_BEACON,          /*!< Beacon data */
  WAVE_CORE_REQ_SET_BEACON_INTERVAL, /*!< Set Peer AP Beacon interval (used for sta interface) */
  WAVE_CORE_REQ_REQUEST_SID,         /*!< Request SID */
  WAVE_CORE_REQ_REMOVE_SID,          /*!< Remvoe SID */
  WAVE_CORE_REQ_SYNC_DONE,           /*!< Recovery sync done */
  WAVE_CORE_REQ_GET_STATION,         /*!< Get station */
  WAVE_CORE_REQ_SET_WMM_PARAM,       /*!< WMM parameters */
  WAVE_CORE_REQ_MGMT_TX,             /*!< Management packet on TX */
  WAVE_CORE_REQ_MGMT_RX,             /*!< Management packet on RX */
  WAVE_CORE_REQ_AP_CONNECT_STA,      /*!< Connect STA to AP */
  WAVE_CORE_REQ_AP_DISCONNECT_STA,   /*!< Disconnect STA for AP */
  WAVE_CORE_REQ_AP_DISCONNECT_ALL,   /*!< Disconnect all for AP */
  WAVE_CORE_REQ_AUTHORIZING_STA,     /*!< Authorizing STA */
  WAVE_CORE_REQ_DEACTIVATE,          /*!< Deactivate the core    */
  WAVE_CORE_REQ_SET_MAC_ADDR,        /*!< Assign MAC address     */
  WAVE_CORE_REQ_GET_MAC_ADDR,        /*!< Query MAC address      */
  WAVE_CORE_REQ_GET_STATUS,          /*!< Get core status & statistics    */
  WAVE_CORE_REQ_GET_TX_RX_WARN,      /*!< get tx/rx stats to monitor */
  WAVE_CORE_REQ_SET_WDS_CFG,         /*!< Process WDS Configuration */
  WAVE_CORE_REQ_GET_WDS_CFG,         /*!< Process WDS Configuration */
  WAVE_CORE_REQ_GET_WDS_PEERAP,      /*!< Process WDS Configuration */
  WAVE_CORE_REQ_GET_CORE_CFG,        /*!< Process Core */
  WAVE_CORE_REQ_SET_CORE_CFG,        /*!< Process Core */
  WAVE_CORE_REQ_GET_RANGE_INFO,        /*!< Get supported bitrates and channels info*/
  WAVE_CORE_REQ_GET_STADB_STATUS,      /*!< Get status&statistic data from STADB (list)*/
  WAVE_CORE_REQ_SET_WDS_WEP_ENC_CFG,            /*!< Configure WDS WEP encryption */
  WAVE_CORE_REQ_GET_ENCEXT_CFG,                 /*!< Get Extended encoding data structure */
  WAVE_CORE_REQ_SET_ENCEXT_CFG,                 /*!< Set Extended encoding parmeters */
  WAVE_CORE_REQ_SET_DEFAULT_KEY_CFG,            /*!< Set default key parmeters */
  WAVE_CORE_REQ_CHANGE_BSS,                     /*!< Change BSS */
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  WAVE_CORE_REQ_SET_RTLOG_CFG,                  /*!< Set RT-Logger cfg */
#endif
  MTLK_CORE_REQ_MCAST_HELPER_GROUP_ID_ACTION,   /*!< Multicast group id action */
  WAVE_CORE_REQ_GET_RX_TH,                      /*!< Get last High Reception Threshold */
  WAVE_CORE_REQ_SET_RX_TH,                      /*!< Set High Reception Threshold */
  WAVE_CORE_REQ_SET_QOS_MAP,                    /*!< Set QoS map */
  WAVE_CORE_REQ_SET_AGGR_CONFIG,                /*!< Set Aggregation config */
  WAVE_CORE_REQ_GET_AGGR_CONFIG,                /*!< Get Aggregation config */
  WAVE_CORE_REQ_SET_BLACKLIST_ENTRY,            /*!< Set blacklist entry */
  WAVE_CORE_REQ_GET_STATION_MEASUREMENTS,       /*!< Get station measurements */
  WAVE_CORE_REQ_GET_VAP_MEASUREMENTS,           /*!< Get VAP measurements */
  WAVE_CORE_REQ_GET_UNCONNECTED_STATION,        /*!< Get unconnected station (currently only RSSI) */
  WAVE_CORE_REQ_SET_FOUR_ADDR_CFG,              /*!< Set four address configuration */
  WAVE_CORE_REQ_GET_FOUR_ADDR_STA_LIST,         /*!< Get four address station list */
  WAVE_CORE_REQ_SET_ATF_QUOTAS,                 /*!< Set ATF quotas per station and per VAP */
  WAVE_CORE_REQ_SET_MCAST_RANGE,                /*!< Set mcast range (for both IPv4 and IPv6) */
  WAVE_CORE_REQ_GET_MCAST_RANGE_IPV4,           /*!< Get mcast range (IPv4) */
  WAVE_CORE_REQ_GET_MCAST_RANGE_IPV6,           /*!< Get mcast range (IPv6) */
  WAVE_CORE_REQ_SET_FWRD_UNKWN_MCAST_FLAG,      /*!< Set unknown mcast filter mode per VAP */
  WAVE_CORE_REQ_GET_FWRD_UNKWN_MCAST_FLAG,      /*!< Get unknown mcast filter mode per VAP */
  WAVE_CORE_REQ_SET_OPERATING_MODE,
  WAVE_CORE_REQ_SET_WDS_WPA_LIST_ENTRY,         /*!< Set WDS WPA list entry */
  WAVE_CORE_REQ_CHECK_4ADDR_MODE,               /*!< Check 4-addr STA mode */
  WAVE_CORE_REQ_GET_TX_POWER,                   /*!< Get Tx Power */
  WAVE_CORE_REQ_SET_HT_PROTECTION,              /*!< Set last HT protection mode */
  WAVE_CORE_REQ_GET_HT_PROTECTION,              /*!< Get HT protection mode */
  WAVE_CORE_REQ_GET_NETWORK_MODE,               /*!< Get Network Mode */
  WAVE_CORE_REQ_SET_SOFTBLOCKLIST_ENTRY,        /*!< Set SoftBlock Thresholds */
  /* WAVE_CORE_REQ_SET_AOCS_CHANNELS_TBL_DBG, */
  /* WAVE_CORE_REQ_SET_AOCS_CL, */
  WAVE_RADIO_REQ_GET_RADIO_INFO,                 /*!< Get radio info */
  WAVE_RADIO_REQ_SET_CHAN,                      /*!< Set channel */
  WAVE_RADIO_REQ_PREPARE_CAC_START,             /*!< Prepare radio for CAC */
  WAVE_RADIO_REQ_NOTIFY_CAC_FINISHED,           /*!< Notify CAC finished */
  WAVE_RADIO_REQ_DO_SCAN,                       /*!< Do scan */
  WAVE_RADIO_REQ_SCAN_TIMEOUT,                  /*!< Handle scan timeout */
  WAVE_RADIO_REQ_ALTER_SCAN,                    /*!< Alter current scan: abort/pause/resume */
  WAVE_RADIO_REQ_DUMP_SURVEY,                   /*!< Dump survey */
  WAVE_RADIO_REQ_FIN_PREV_FW_SC,                /*!< Finish and prevent FW set-channel process */
  WAVE_RADIO_REQ_GET_PHY_RX_STATUS,             /*!< Process Phy Rx status */
  WAVE_RADIO_REQ_GET_PHY_CHAN_STATUS,           /*!< Get phy channel status */
  WAVE_RADIO_REQ_GET_BCL_MAC_DATA,              /*!< Get BCL MAC data */
  WAVE_RADIO_REQ_SET_BCL_MAC_DATA,              /*!< Get BCL MAC data */
  WAVE_RADIO_REQ_SET_RECOVERY_CFG,              /*!< Set FW Recovery configuration */
  WAVE_RADIO_REQ_GET_RECOVERY_CFG,              /*!< Get FW Recovery configuration */
  WAVE_RADIO_REQ_GET_RECOVERY_STATS,            /*!< Get FW Recovery statistics */
  WAVE_RADIO_REQ_GET_TX_RATE_POWER,             /*!< Get Tx Rate Power */
  WAVE_RADIO_SET_STATIC_PLAN,                   /*!< Set Static Planner */
  WAVE_RADIO_REQ_GET_DOT11H_AP_CFG,             /*!< Process request to DOT11H on AP */
  WAVE_RADIO_REQ_SET_DOT11H_AP_CFG,             /*!< Process request to DOT11H on AP */
  WAVE_RADIO_REQ_GET_MASTER_CFG,                /*!< Process Core */
  WAVE_RADIO_REQ_SET_MASTER_CFG,                /*!< Process Core */
  WAVE_RADIO_REQ_GET_COC_CFG,                   /*!< Process COC */
  WAVE_RADIO_REQ_SET_COC_CFG,                   /*!< Process COC */
  WAVE_RADIO_REQ_GET_ERP_CFG,                   /*!< Process ERP */
  WAVE_RADIO_REQ_SET_ERP_CFG,                   /*!< Process ERP */
  WAVE_RADIO_REQ_GET_TPC_CFG,                   /*!< TPC config */
  WAVE_RADIO_REQ_SET_TPC_CFG,                   /*!< TPC config */
  WAVE_RADIO_REQ_MBSS_ADD_VAP_IDX,              /*!< MBSS AddVap issued by Master */
  WAVE_RADIO_REQ_MBSS_ADD_VAP_NAME,             /*!< MBSS AddVap issued by Master */
  WAVE_RADIO_REQ_MBSS_DEL_VAP_IDX,              /*!< MBSS DelVap issued by Master */
  WAVE_RADIO_REQ_MBSS_DEL_VAP_NAME,             /*!< MBSS DelVap issued by Master */
  WAVE_RADIO_REQ_MBSS_VAP_CREATE_SEC_VAP,       /*!< MBSS ReserveSecVaps issued by Master */
  WAVE_RADIO_REQ_SET_INTERFDET_CFG,             /*!< Set Interference Detection thresholds */
  WAVE_RADIO_REQ_GET_INTERFDET_CFG,             /*!< Get Interference Detection thresholds */
  WAVE_RADIO_REQ_SET_11B_CFG,                   /*!< Set 11b configuration */
  WAVE_RADIO_REQ_GET_11B_CFG,                   /*!< Get 11b configuration */
  WAVE_RADIO_REQ_SET_SCAN_AND_CALIB_CFG,        /*!< Set scan and calib config */
  WAVE_RADIO_REQ_GET_SCAN_AND_CALIB_CFG,        /*!< Get scan and calib config */
  WAVE_RADIO_REQ_SET_ALLOW_SCAN_DURING_CAC,     /*!< Is a scan req blocked when CAC is in progress */
  WAVE_RADIO_REQ_GET_ALLOW_SCAN_DURING_CAC,     /*!< Is a scan req blocked when CAC is in progress */
  WAVE_RADIO_REQ_GET_AGG_RATE_LIMIT,            /*!< Get Aggregation-rate limit */
  WAVE_RADIO_REQ_SET_AGG_RATE_LIMIT,            /*!< Set Aggregation-rate limit */
  WAVE_RADIO_REQ_GET_RX_DUTY_CYCLE,             /*!< Get last Reception Duty Cycle Setting */
  WAVE_RADIO_REQ_SET_RX_DUTY_CYCLE,             /*!< Set Reception Duty Cycle Setting */
  WAVE_RADIO_REQ_GET_TX_POWER_LIMIT_OFFSET,     /*!< Get last TX Power Limit */
  WAVE_RADIO_REQ_SET_TX_POWER_LIMIT_OFFSET,     /*!< Set TX Power Limit */
  WAVE_RADIO_REQ_SET_QAMPLUS_MODE,              /*!< Set QAMplus mode on demand */
  WAVE_RADIO_REQ_GET_QAMPLUS_MODE,              /*!< Get QAMplus mode */
  WAVE_RADIO_REQ_SET_RADIO_MODE,                /*!< Set Radio mode on demand */
  WAVE_RADIO_REQ_GET_RADIO_MODE,                /*!< Get Radio mode */
  WAVE_RADIO_REQ_SET_AMSDU_NUM,                 /*!< Set AMSDU num on demand */
  WAVE_RADIO_REQ_GET_AMSDU_NUM,                 /*!< Get AMSDU num */
  WAVE_RADIO_REQ_SET_CCA_THRESHOLD,             /*!< Set CCA threshold */
  WAVE_RADIO_REQ_GET_CCA_THRESHOLD,             /*!< Get CCA threshold */
  WAVE_RADIO_REQ_SET_CCA_ADAPTIVE,              /*!< Set CCA adaptive intervals */
  WAVE_RADIO_REQ_GET_CCA_ADAPTIVE,              /*!< Get CCA adaptive intervals */
  WAVE_RADIO_REQ_SET_RADAR_RSSI_TH,             /*!< Set Radar Detection RSSI threshold */
  WAVE_RADIO_REQ_GET_RADAR_RSSI_TH,             /*!< Get Radar Detection RSSI threshold */
  WAVE_RADIO_REQ_SET_MAX_MPDU_LENGTH,           /*!< Set MAX MPDU length */
  WAVE_RADIO_REQ_GET_MAX_MPDU_LENGTH,           /*!< Get MAX MPDU length */
  WAVE_RADIO_REQ_SET_SSB_MODE,                  /*!< Set SSB Mode */
  WAVE_RADIO_REQ_GET_SSB_MODE,                  /*!< Get SSB Mode */
  WAVE_RADIO_REQ_SET_COEX_CFG,                  /*!< Set 2.4 GHz BT, BLE, ZigBee Coex config */
  WAVE_RADIO_REQ_GET_COEX_CFG,                  /*!< Get 2.4 GHz BT, BLE, ZigBee Coex config */
  WAVE_RADIO_REQ_CHANGE_BSS,                    /*!< Change BSS information */
  WAVE_RADIO_REQ_SET_AP_BEACON_INFO,            /*!< Ap mode - Set beacon info, and start beaconing */
  WAVE_RADIO_REQ_SET_FREQ_JUMP_MODE,            /*!< Set Frequency Jump Mode */
  WAVE_RADIO_REQ_SET_RESTRICTED_AC_MODE,        /*!< Set Restricted AC Mode */
  WAVE_RADIO_REQ_GET_RESTRICTED_AC_MODE,        /*!< Get Restricted AC Mode */
  WAVE_RADIO_REQ_GET_CALIBRATION_MASK,          /*!< Get offline and online calibration mask */
  WAVE_RADIO_REQ_SET_CALIBRATION_MASK,          /*!< Set offline and online calibration mask */
  WAVE_RADIO_REQ_SET_NFRP_CFG,                  /*!< Set NFRP configuration */
  WAVE_RADIO_REQ_GET_RTS_RATE,                  /*!< Get RTS Protection Rate Configuration */
  WAVE_RADIO_REQ_SET_RTS_RATE,                  /*!< Set RTS Protection Rate Configuration */
  WAVE_RADIO_REQ_GET_PHY_INBAND_POWER,          /*!< Get InBand Power */
  WAVE_RADIO_REQ_SET_ETSI_PPDU_LIMITS,          /*!< Set ETSI PPDU Limits */
  WAVE_RADIO_REQ_GET_ETSI_PPDU_LIMITS,          /*!< Get ETSI PPDU Limits */
  WAVE_RADIO_REQ_SET_RTS_THRESHOLD,             /*!< Set RTS Threshold */
  WAVE_RADIO_REQ_GET_RTS_THRESHOLD,             /*!< Get RTS Threshold */
  WAVE_RADIO_REQ_SET_AP_RETRY_LIMIT,            /*!< Set Tx Retry Limit */
  WAVE_RADIO_REQ_GET_AP_RETRY_LIMIT,            /*!< Get Tx Retry Limit */

  WAVE_RADIO_REQ_SET_STATIONS_STATS,            /*!< Set stations statistics */
  WAVE_RADIO_REQ_GET_STATIONS_STATS,            /*!< Get stations statistics */
  WAVE_HW_REQ_GET_AP_CAPABILITIES,              /*!< Get AP capabilities */
  WAVE_HW_REQ_GET_EEPROM_CFG,                   /*!< Process EEPROM */
  WAVE_HW_REQ_SET_TEMPERATURE_SENSOR,           /*!< Set calibrate on demand */
  WAVE_HW_REQ_GET_TEMPERATURE_SENSOR,           /*!< Get temperature from sensor */
  WAVE_HW_REQ_GET_PVT_SENSOR,                   /*!< Get PVT sensor */
  WAVE_HW_REQ_SET_DYNAMIC_MU_CFG,               /*!< Set Dynamic MU Configuration */
  WAVE_HW_REQ_GET_DYNAMIC_MU_CFG,               /*!< Get Dynamic MU Configuration */
  WAVE_RCVRY_RESET,                             /*!< Reset recovery counters and fw dump */
  WAVE_CORE_REQ_GET_TWT_PARAMETERS,             /*!< Get the twt parameters from FW */
  WAVE_CORE_REQ_GET_AX_DEFAULT_PARAMS,          /*!< Get the ax default params */
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_STATS,       /*!< Get associated device statistics */
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_RATE_INFO_RX_STATS,    /*!< Get associated device rate info rx statistics */
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_RATE_INFO_TX_STATS,    /*!< Get associated device rate info tx statistics */
  WAVE_CORE_REQ_GET_CHANNEL_STATS,              /*!< Get radio channel statistics */
  WAVE_CORE_REQ_GET_ASSOCIATED_DEV_TID_STATS,
  WAVE_CORE_REQ_SET_STATS_POLL_PERIOD,          /*!< Set statistics poll period */
  WAVE_CORE_REQ_GET_STATS_POLL_PERIOD,          /*!< Get statistics poll period */
  WAVE_CORE_REQ_GET_PEER_LIST,
  WAVE_CORE_REQ_GET_PEER_FLOW_STATUS,
  WAVE_CORE_REQ_GET_PEER_CAPABILITIES,
  WAVE_CORE_REQ_GET_PEER_RATE_INFO,
  WAVE_CORE_REQ_GET_RECOVERY_STATS,
  WAVE_CORE_REQ_GET_HW_FLOW_STATUS,
  WAVE_CORE_REQ_GET_TR181_WLAN_STATS,
  WAVE_CORE_REQ_GET_TR181_HW_STATS,
  WAVE_CORE_REQ_GET_TR181_PEER_STATS,
  WAVE_HW_REQ_SET_ZWDFS_ANT_ENABLED,            /*!< Enable/Disable ZWDFS state - from zwdfs radio */
  WAVE_HW_REQ_GET_ZWDFS_ANT_ENABLED,            /*!< Get ZWDFS Antenna state - from zwdfs radio */
  WAVE_HW_REQ_SET_ZWDFS_CARD_ANT_CONFIG,        /*!< Change ZWDFS Card Antenna config - from main radio */
  WAVE_CORE_REQ_GET_DEV_DIAG_RESULT2,
  WAVE_CORE_REQ_GET_DEV_DIAG_RESULT3,
  WAVE_CORE_REQ_GET_ASSOC_STATIONS_STATS,
  WAVE_RADIO_REQ_GET_BSS_TX_STATUS,             /*!< Check BSS TX status */
  WAVE_CORE_REQ_SET_CHAN_SWITCH_DEAUTH_PARAMS,  /*!< Store CSA deauth parameters to be used by channel switch */

  WAVE_RADIO_REQ_SET_CTS_TO_SELF_TO,            /*!< Set CTS to self */
  WAVE_RADIO_REQ_GET_CTS_TO_SELF_TO,            /*!< Get CTS to self */
  WAVE_RADIO_REQ_SET_TX_AMPDU_DENSITY,          /*!< Set Tx AMPDU Denisty */
  WAVE_RADIO_REQ_GET_TX_AMPDU_DENSITY,          /*!< Get Tx AMPDU Denisty */
  WAVE_CORE_REQ_GET_PRIORITY_GPIO_STATS,        /*!< Get Priority GPIO */
  WAVE_CORE_REQ_SET_UNSOLICITED_FRAME_TX,       /*!< Set unsolicited frame transmission */
  WAVE_CORE_REQ_SET_DYNAMIC_MC_RATE,            /*!< Set Multicast Rate */
  WAVE_CORE_REQ_SET_FILS_DISCOVERY_FRAME,       /*!< Store FILS Discovery frame buffer */
  WAVE_RADIO_REQ_SET_INITAL_DATA,               /*!< Some required initial data send via Vendor cmd from hostapd */
  WAVE_RADIO_REQ_STEER_STA,                     /*!< Steer STA */
  WAVE_CORE_REQ_SET_HE_OPERATION,               /*!< Set HE Operation */
  WAVE_CORE_REQ_GET_HE_OPERATION,               /*!< Get HE Operation */
  WAVE_CORE_REQ_SET_HE_NON_ADVERTISED,          /*!< Set HE non advertised */
  WAVE_CORE_REQ_GET_HE_NON_ADVERTISED,          /*!< Get HE non advertised */
  WAVE_CORE_REQ_SET_HE_DEBUG,                   /*!< Set HE Debug */
  WAVE_RADIO_REQ_SET_MU_OFDMA_BF,               /*!< set mu ofdma */
  WAVE_RADIO_REQ_GET_MU_OFDMA_BF,               /*!< get mu ofdma */
  WAVE_CORE_REQ_GET_PREAMBLE_PUNCTURE_CCA_OVERRIDE, /*!< get preample puncture cca override */
  WAVE_CORE_REQ_CCA_STATS_GET,                  /*!< Get CCA statistics */
  WAVE_RADIO_REQ_GET_PROBE_REQ_LIST,            /*!< Get probe req info */
  WAVE_CORE_REQ_SET_PREAMBLE_PUNCTURE_CCA_OVERRIDE, /*!< Set preample puncture cca override */
  WAVE_RADIO_REQ_GET_USAGE_STATS,              /*!< Get general radio stats */
  WAVE_CORE_REQ_ADVERTISE_BTWT_SCHEDULE,         /*!< Advertise BTWT Schedule */
  WAVE_CORE_REQ_TERMINATE_BTWT_SCHEDULE,         /*!< Terminate BTWT Schedule */
  WAVE_CORE_REQ_GET_ADVERTISED_BTWT_SCHEDULE,    /*!< Get advertised BTWT Schedule */
  WAVE_CORE_REQ_TX_TWT_TEARDOWN,                 /*!< Transmit TWT Teardown Frame */
  WAVE_RADIO_REQ_GET_HW_LIMITS,                 /*!< Get HW Limits */
  WAVE_RADIO_REQ_SET_FIXED_LTF_AND_GI,           /*!< Set fixed LTF and GI */
  WAVE_RADIO_REQ_GET_FIXED_LTF_AND_GI,           /*!< Get fixed LTF and GI */
  WAVE_RADIO_REQ_SET_WHM_CONFIG,                 /*!< Set whm config */
  WAVE_RADIO_REQ_SET_WHM_RESET,                  /*!< Set whm reset */
  WAVE_CORE_REQ_SET_WHM_TRIGGER,                 /*!< Set Trigger WHM */
  WAVE_CORE_REQ_GET_CONNECTION_ALIVE,            /*!< Gen connection alive for AP */
  WAVE_CORE_REQ_SET_AP_EXCE_RETRY_LIMIT,         /*!< Set Tx Exce Retry Limit */
  WAVE_CORE_REQ_GET_AP_EXCE_RETRY_LIMIT,         /*!< Get Tx Exce Retry Limit */

/* DEBUG COMMANDS */
#ifdef CONFIG_WAVE_DEBUG
  MTLK_HW_REQ_GET_COUNTERS_SRC,                 /*!< Get WAVE counters source */
  MTLK_HW_REQ_SET_COUNTERS_SRC,                 /*!< Set WAVE counters source */
  WAVE_RADIO_REQ_SET_IRE_CTRL_B,                /*!< Set IRE Control B */
  WAVE_RADIO_REQ_GET_IRE_CTRL_B,                /*!< Get IRE Control B */
#ifdef CPTCFG_IWLWAV_SET_PM_QOS
  WAVE_RADIO_REQ_SET_CPU_DMA_LATENCY,           /*!< Set CPU DMA latency */
  WAVE_RADIO_REQ_GET_CPU_DMA_LATENCY,           /*!< Get CPU DMA latency */
#endif
  WAVE_HW_REQ_GET_BF_EXPLICIT_CAP,              /*!< Get BF Explicit Capable flag */
  WAVE_HW_REQ_GET_TASKLET_LIMITS,               /*!< Get tasklet limits */
  WAVE_HW_REQ_SET_TASKLET_LIMITS,               /*!< Set tasklet limits */
  WAVE_RADIO_REQ_GET_TA_CFG,                    /*!< Process TA */
  WAVE_RADIO_REQ_SET_TA_CFG,                    /*!< Process TA */
#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
  WAVE_RADIO_REQ_GET_PCOC_CFG,                  /*!< Process PCOC */
  WAVE_RADIO_REQ_SET_PCOC_CFG,                  /*!< Process PCOC */
#endif

  WAVE_CORE_REQ_GET_HSTDB_CFG,                  /*!< Process HSTDB */
  WAVE_CORE_REQ_SET_HSTDB_CFG,                  /*!< Process HSTDB */
  WAVE_RADIO_REQ_GET_MAC_WATCHDOG_CFG,          /*!< Process MAC watchdog */
  WAVE_RADIO_REQ_SET_MAC_WATCHDOG_CFG,          /*!< Process MAC watchdog */
  WAVE_HW_REQ_SET_TEST_BUS,                     /*!< Set Test Bus enable/disable */
  WAVE_RADIO_REQ_SET_FIXED_RATE,                /*!< Set Fixed Rate */
  WAVE_RADIO_REQ_SET_MAC_ASSERT,                /*!< Perform MAC assert */
  WAVE_RADIO_REQ_SET_FW_DEBUG,                  /*!< Send debug data to FW */
  WAVE_RADIO_REQ_SET_DBG_CLI,                   /*!< Send debug data to FW */
  WAVE_RADIO_REQ_SET_FW_LOG_SEVERITY,           /*!< Set FW Log Severity */
  WAVE_RADIO_REQ_SET_MTLK_DEBUG,                /*!< Set MTLK debug level */
  WAVE_CORE_REQ_GET_MC_IGMP_TBL,                /*!< Get IGMP MCAST table */
  WAVE_CORE_REQ_GET_MC_HW_TBL,                  /*!< Get MCAST HW table */
  WAVE_CORE_REQ_GET_WDS_WPA_LIST_ENTRIES,       /*!< Get WDS WPA list entries */
  WAVE_CORE_REQ_GET_BLACKLIST_ENTRIES,          /*!< Get blacklist entries */
  WAVE_CORE_REQ_GET_MULTI_AP_BL_ENTRIES,        /*!< Get Multi-AP blacklist entries */
  WAVE_CORE_REQ_GET_QOS_MAP_INFO,               /*!< Get status&statistic data related to Hotspot 2.0 */
  WAVE_CORE_REQ_GET_SERIALIZER_INFO,            /*!< Get Serializer Info */
  WAVE_RADIO_REQ_SET_DEBUG_CMD_FW,              /*!< Send any command to FW */
  WAVE_CORE_REQ_RESET_STATS,                    /*!< Reset core statistics  */
  WAVE_CORE_REQ_GET_PEER_HOST_IF_QOS,
  WAVE_CORE_REQ_GET_PEER_HOST_IF,
  WAVE_CORE_REQ_GET_PEER_RX_STATS,
  WAVE_CORE_REQ_GET_PEER_UL_BSRC_TID,
  WAVE_CORE_REQ_GET_PEER_BAA_STATS,
  WAVE_CORE_REQ_GET_LINK_ADAPTION_STATS,
  WAVE_CORE_REQ_GET_PLAN_MANAGER_STATS,
  WAVE_CORE_REQ_GET_TWT_STATS,
  WAVE_CORE_REQ_GET_PER_CLIENT_STATS,
  WAVE_CORE_REQ_GET_PEER_PHY_RX_STATUS,
  WAVE_CORE_REQ_GET_PEER_INFO,
  WAVE_CORE_REQ_GET_WLAN_HOST_IF_QOS,
  WAVE_CORE_REQ_GET_WLAN_HOST_IF,
  WAVE_CORE_REQ_GET_WLAN_RX_STATS,
  WAVE_CORE_REQ_GET_WLAN_BAA_STATS,
  WAVE_CORE_REQ_GET_RADIO_RX_STATS,
  WAVE_CORE_REQ_GET_RADIO_BAA_STATS,
  WAVE_CORE_REQ_GET_TSMAN_INIT_TID_GL,
  WAVE_CORE_REQ_GET_TSMAN_INIT_STA_GL,
  WAVE_CORE_REQ_GET_TSMAN_RCPT_TID_GL,
  WAVE_CORE_REQ_GET_TSMAN_RCPT_STA_GL,
  WAVE_CORE_REQ_GET_RADIO_LINK_ADAPT_STATS,
  WAVE_CORE_REQ_GET_MULTICAST_STATS,
  WAVE_CORE_REQ_GET_TR_MAN_STATS,
  WAVE_CORE_REQ_GET_GRP_MAN_STATS,
  WAVE_CORE_REQ_GET_GENERAL_STATS,
  WAVE_CORE_REQ_GET_CUR_CHANNEL_STATS,
  WAVE_CORE_REQ_GET_RADIO_PHY_RX_STATS,
  WAVE_CORE_REQ_GET_DYNAMIC_BW_STATS,
  WAVE_CORE_REQ_GET_LINK_ADAPT_MU_STATS,
#endif /* CONFIG_WAVE_DEBUG */
  WAVE_RADIO_REQ_EMULATE_INTERFERER,            /*!< Emulate Interferer */
} mtlk_core_tx_req_id_t;


typedef enum _mtlk_aocs_ac_e {
  AC_DISABLED,
  AC_ENABLED,
  AC_NOT_USED,
} mtlk_aocs_ac_e;

/* universal accessors for data, passed between Core and DF through clipboard */
#define MTLK_DECLARE_CFG_START(name)  typedef struct _##name {
#define MTLK_DECLARE_CFG_END(name)    } __MTLK_IDATA name;

#define MTLK_CFG_ITEM(type,name) type name; \
                                 uint8 name##_filled; \
                                 uint8 name##_requested;

#define MTLK_CFG_ITEM_ARRAY(type,name,num_of_elems) type name[num_of_elems]; \
                                                     uint8 name##_filled; \
                                                     uint8 name##_requested;

/* integer values */
#define MTLK_CFG_SET_ITEM(obj,name,src) \
  {\
    (obj)->name##_filled = 1; \
    (obj)->name = src; \
  }

#define MTLK_CFG_SET_ITEM_BY_FUNC(obj,name,func,func_params,func_res) \
  {\
    func_res = func func_params; \
    if (MTLK_ERR_OK == func_res) {\
      (obj)->name##_filled = 1;\
    }\
    else {\
      (obj)->name##_filled = 0;\
    }\
  }

#define MTLK_CFG_SET_ITEM_BY_FUNC_VOID(obj,name,func,func_params) \
  {\
    func func_params; \
    (obj)->name##_filled = 1; \
  }

#define MTLK_CFG_CHECK_AND_SET_ITEM(obj,name,src) \
  {                                               \
    if ((obj)->name##_requested) {                \
      (obj)->name##_filled = 1;                   \
      (obj)->name = src;                          \
    }                                             \
  }

#define MTLK_CFG_CHECK_AND_SET_ITEM_BY_FUNC(obj,name,func,func_params,func_res) \
  {                                                                             \
    if ((obj)->name##_requested) {                                              \
      func_res = func func_params;                                              \
      if (MTLK_ERR_OK == func_res) {                                            \
        (obj)->name##_filled = 1;                                               \
      }                                                                         \
      else {                                                                    \
        (obj)->name##_filled = 0;                                               \
      }                                                                         \
    }                                                                           \
  }

#define MTLK_CFG_CHECK_AND_SET_ITEM_BY_FUNC_VOID(obj,name,func,func_params) \
  {                                                                         \
    if ((obj)->name##_requested) {                                          \
      func func_params;                                                     \
      (obj)->name##_filled = 1;                                             \
    }                                                                       \
  }

#define MTLK_CFG_GET_ITEM(obj,name,dest) \
  if(1==(obj)->name##_filled){(dest)=(obj)->name;}

#define MTLK_CFG_GET_ITEM_BY_FUNC(obj,name,func,func_params,func_res) \
  if(1==(obj)->name##_filled){func_res=func func_params;}

#define MTLK_CFG_GET_ITEM_BY_FUNC_VOID(obj,name,func,func_params) \
  if(1==(obj)->name##_filled){func func_params;}

/* integer buffers -> used loop for provide different type sizes in assigning */
#define MTLK_CFG_SET_ARRAY_ITEM(obj,name,src,num_elems_received,result) \
  { \
    result=MTLK_ERR_PARAMS; \
    if (ARRAY_SIZE((obj)->name) == num_elems_received) {\
      uint32 counter; \
      for (counter=0;counter<num_elems_received;counter++) { \
        (obj)->name[counter]=(src)[counter]; \
      } \
      (obj)->name##_filled = 1; \
      result=MTLK_ERR_OK; \
    }\
  }

#define MTLK_CFG_GET_ARRAY_ITEM(obj,name,dst,num_elems) \
  { \
      if ((obj)->name##_filled == 1) {\
        uint32 counter; \
        for (counter=0;counter<num_elems;counter++) { \
          (dst)[counter]=(obj)->name[counter]; \
        }\
      }\
  }

/* FIXME: This is the same as MTLK_CFG_SET_ITEM_BY_FUNC, so redundant */
#define MTLK_CFG_SET_ARRAY_ITEM_BY_FUNC(obj,name,func,func_params,func_res) \
  {\
    func_res=func func_params; \
    if (MTLK_ERR_OK == func_res) {\
      (obj)->name##_filled = 1;\
    }\
    else {\
      (obj)->name##_filled = 0;\
    }\
  }

/* FIXME: This is the same as MTLK_CFG_SET_ITEM_BY_FUNC_VOID, so redundant */
#define MTLK_CFG_SET_ARRAY_ITEM_BY_FUNC_VOID(obj,name,func,func_params) \
  {\
    func func_params; \
    (obj)->name##_filled = 1; \
  }

/* Card Capabilities */
MTLK_DECLARE_CFG_START(mtlk_card_capabilities_t)
  MTLK_CFG_ITEM(uint32, max_vaps_supported)
  MTLK_CFG_ITEM(uint32, max_stas_supported)
MTLK_DECLARE_CFG_END(mtlk_card_capabilities_t)

/* DOT11H configuration structure for AP */
MTLK_DECLARE_CFG_START(mtlk_11h_ap_cfg_t)
  MTLK_CFG_ITEM(int, debug_chan)
  MTLK_CFG_ITEM(int, debugChannelAvailabilityCheckTime)
#ifdef CONFIG_WAVE_DEBUG
  MTLK_CFG_ITEM(int, debugChannelSwitchCount)
  MTLK_CFG_ITEM(uint32, debugNOP)
#endif
MTLK_DECLARE_CFG_END(mtlk_11h_ap_cfg_t)

/* radio calibration mask configuration structure */
MTLK_DECLARE_CFG_START(wave_radio_calibration_cfg_t)
  MTLK_CFG_ITEM(uint32, calibr_algo_mask)
  MTLK_CFG_ITEM(uint32, online_calibr_algo_mask)
MTLK_DECLARE_CFG_END(wave_radio_calibration_cfg_t)

typedef struct mtlk_country_code {
    char country[COUNTRY_CODE_MAX_LEN + 1]; /* NUL-terminated string */
} mtlk_country_code_t;

/* WDS configuration structure */
MTLK_DECLARE_CFG_START(mtlk_wds_cfg_t)
  MTLK_CFG_ITEM(IEEE_ADDR, peer_ap_addr_add)
  MTLK_CFG_ITEM(IEEE_ADDR, peer_ap_addr_del)
  MTLK_CFG_ITEM(mtlk_clpb_t *, peer_ap_vect)
  MTLK_CFG_ITEM(uint8, peer_ap_key_idx)
MTLK_DECLARE_CFG_END(mtlk_wds_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_wds_dbg_t)
  MTLK_CFG_ITEM(int, dummy)
MTLK_DECLARE_CFG_END(mtlk_wds_dbg_t)

/* 11b configuration */
MTLK_DECLARE_CFG_START(mtlk_11b_antsel_t)
  MTLK_CFG_ITEM(uint8, txAnt)
  MTLK_CFG_ITEM(uint8, rxAnt)
  MTLK_CFG_ITEM(uint8, rate)
MTLK_DECLARE_CFG_END(mtlk_11b_antsel_t)

MTLK_DECLARE_CFG_START(mtlk_11b_cfg_t)
  MTLK_CFG_ITEM(mtlk_11b_antsel_t, antsel)
MTLK_DECLARE_CFG_END(mtlk_11b_cfg_t)

/* WME configuration structure */
MTLK_DECLARE_CFG_START(mtlk_gen_core_country_name_t)
  MTLK_CFG_ITEM_ARRAY(char, name, 3)
MTLK_DECLARE_CFG_END(mtlk_gen_core_country_name_t)

typedef struct mtlk_core_channel_def
{
  uint16 channel;
  uint8 bonding;
  uint8 spectrum_mode;
}mtlk_core_channel_def_t;

typedef struct {
  int timer_acl_on;
  int timer_acl_probe_drop;
} wave_core_sb_timer_acl_t;

MTLK_DECLARE_CFG_START(mtlk_gen_core_cfg_t)
  MTLK_CFG_ITEM(int, bridge_mode)
  MTLK_CFG_ITEM(BOOL, dbg_sw_wd_enable)
  MTLK_CFG_ITEM(BOOL, reliable_multicast)
  MTLK_CFG_ITEM(BOOL, ap_forwarding)
  MTLK_CFG_ITEM(uint8, net_mode)
  MTLK_CFG_ITEM_ARRAY(char, nickname, MTLK_ESSID_MAX_SIZE)
  MTLK_CFG_ITEM_ARRAY(char, essid, MTLK_ESSID_MAX_SIZE)
  MTLK_CFG_ITEM(IEEE_ADDR, bssid)
  MTLK_CFG_ITEM(mtlk_core_channel_def_t, channel_def)
  MTLK_CFG_ITEM(uint8, frequency_band_cur)
  MTLK_CFG_ITEM(uint32, admission_capacity)
  MTLK_CFG_ITEM(uint8, bss_load)
  MTLK_CFG_ITEM(int, mesh_mode)
  MTLK_CFG_ITEM(uint32, softblock)
  MTLK_CFG_ITEM(uint8, mbssid_vap)
  MTLK_CFG_ITEM(uint8, vaps_in_grp)
  MTLK_CFG_ITEM(uint8, mgmt_rate)
  MTLK_CFG_ITEM(uint8, he_beacon)
  MTLK_CFG_ITEM(wave_core_sb_timer_acl_t, sb_timer_acl)
MTLK_DECLARE_CFG_END(mtlk_gen_core_cfg_t)

MTLK_DECLARE_CFG_START(wave_core_network_mode_cfg_t)
  MTLK_CFG_ITEM(uint8, net_mode)
MTLK_DECLARE_CFG_END(wave_core_network_mode_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_core_rts_mode_t)
  MTLK_CFG_ITEM(uint8, dynamic_bw)
  MTLK_CFG_ITEM(uint8, static_bw)
MTLK_DECLARE_CFG_END(mtlk_core_rts_mode_t)

/* TxOP Configuration */
#define WAVE_TXOP_CFG_SIZE                            4
#define WAVE_TXOP_CFG_STA_ID_MIN                      0
#define WAVE_TXOP_CFG_STA_ID_MAX                      (GEN6_MAX_SID - 1)
#define WAVE_TXOP_CFG_ALL_STA_ID                      GEN6_ALL_STA_SID
#define WAVE_TXOP_CFG_STA_ID_DEFAULT                  WAVE_TXOP_CFG_ALL_STA_ID
#define WAVE_TXOP_CFG_TXOP_DURATION_MIN               500 /* msec */
#define WAVE_TXOP_CFG_TXOP_DURATION_MAX               32767
#define WAVE_TXOP_CFG_TXOP_DURATION_DEFAULT_2GHZ      20000
#define WAVE_TXOP_CFG_TXOP_DURATION_DEFAULT_5GHZ_6GHZ 13300
#define WAVE_TXOP_CFG_MAX_STAS_NUM_MIN                0
#define WAVE_TXOP_CFG_MAX_STAS_NUM_MAX                GEN6_MAX_SID
#define WAVE_TXOP_CFG_MAX_STAS_NUM_DEFAULT            4
#define WAVE_TXOP_CFG_MODE_MIN                        UMI_TXOP_MODE_DISABLED
#define WAVE_TXOP_CFG_MODE_MAX                        UMI_TXOP_MODE_ENABLED
#define WAVE_TXOP_CFG_MODE_DEFAULT                    UMI_TXOP_MODE_ENABLED

typedef struct {
  uint32 drop_thresh;
  uint32 denominator;
} wave_pie_convertp_t;

typedef struct {
  uint32 drop_thresh;
  uint32 max_burst;
  uint16 reserved;
  uint32 minQueueSizeBytes;
  uint32 weigth;
  wave_pie_convertp_t pie_convert_p[PIE_NUM_OF_CONVERSIONS];
  uint32 decay_val;
  uint32 min_drop_prob;
  uint32 max_drop_prob;
  uint16 alpha;
  uint16 beta;
  uint16 dq_thresh[ACCESS_CATEGORY_NUM];
  uint16 target_latency[ACCESS_CATEGORY_NUM];
  uint16 t_update;
  uint8 user_mode;
} wave_pie_cfg_t;

MTLK_DECLARE_CFG_START(mtlk_master_core_cfg_t)
  MTLK_CFG_ITEM(uint32, acs_update_timeout)
  MTLK_CFG_ITEM(BOOL, mu_operation)
  MTLK_CFG_ITEM(BOOL, probe_req_list_enabled)
  MTLK_CFG_ITEM(mtlk_core_rts_mode_t, rts_mode_params)
  MTLK_CFG_ITEM(uint8, bf_mode)
  MTLK_CFG_ITEM(UMI_SET_TXOP_CONFIG, txop_params)
  MTLK_CFG_ITEM(uint32, active_ant_mask)
  MTLK_CFG_ITEM(uint32, slow_probing_mask)
#ifdef CONFIG_WAVE_DEBUG
  MTLK_CFG_ITEM(FIXED_POWER, fixed_pwr_params)
  MTLK_CFG_ITEM(uint32, unconnected_sta_scan_time)
#endif
  MTLK_CFG_ITEM(uint8, fast_drop)
  MTLK_CFG_ITEM(wave_pie_cfg_t, wave_pie_cfg)
  MTLK_CFG_ITEM(uint8, dfs_debug)
MTLK_DECLARE_CFG_END(mtlk_master_core_cfg_t)

/* HSTDB structure */
MTLK_DECLARE_CFG_START(mtlk_hstdb_cfg_t)
  MTLK_CFG_ITEM(uint32, wds_host_timeout)
  MTLK_CFG_ITEM(IEEE_ADDR, address)
MTLK_DECLARE_CFG_END(mtlk_hstdb_cfg_t)

/* COC structure */
MTLK_DECLARE_CFG_START(mtlk_coc_mode_cfg_t)
  MTLK_CFG_ITEM(mtlk_coc_power_cfg_t, power_params)
  MTLK_CFG_ITEM(mtlk_coc_auto_cfg_t,  auto_params)
  MTLK_CFG_ITEM(uint8, cur_ant_mask)
  MTLK_CFG_ITEM(uint8, hw_ant_mask)
MTLK_DECLARE_CFG_END(mtlk_coc_mode_cfg_t)

/* COC structure */
MTLK_DECLARE_CFG_START(mtlk_erp_mode_cfg_t)
  MTLK_CFG_ITEM(mtlk_coc_erp_cfg_t, erp_cfg)
MTLK_DECLARE_CFG_END(mtlk_erp_mode_cfg_t)

/* EEPROM structure */
MTLK_DECLARE_CFG_START(mtlk_eeprom_data_cfg_t)
  MTLK_CFG_ITEM(uint16, eeprom_version)
  MTLK_CFG_ITEM_ARRAY(uint8, mac_address, ETH_ALEN)
  MTLK_CFG_ITEM_ARRAY(uint8, eeprom_type_str, MTLK_EEPROM_TYPE_LEN)
  MTLK_CFG_ITEM(uint8, eeprom_type_id)
  MTLK_CFG_ITEM(uint8, type)
  MTLK_CFG_ITEM(uint8, revision)
  MTLK_CFG_ITEM(uint8, is_asic)
  MTLK_CFG_ITEM(uint8, is_emul)
  MTLK_CFG_ITEM(uint8, is_fpga)
  MTLK_CFG_ITEM(uint8, is_phy_dummy)
  MTLK_CFG_ITEM(uint16, vendor_id)
  MTLK_CFG_ITEM(uint16, device_id)
  MTLK_CFG_ITEM(uint16, sub_vendor_id)
  MTLK_CFG_ITEM(uint16, sub_device_id)
  MTLK_CFG_ITEM(int16, hdr_size)
  MTLK_CFG_ITEM_ARRAY(uint8, sn, MTLK_EEPROM_SN_LEN)
  MTLK_CFG_ITEM(uint8, production_week)
  MTLK_CFG_ITEM(uint8, production_year)
  MTLK_CFG_ITEM(uint8, cal_file_type)
MTLK_DECLARE_CFG_END(mtlk_eeprom_data_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_eeprom_cfg_t)
  MTLK_CFG_ITEM(mtlk_eeprom_data_cfg_t, eeprom_data)
  MTLK_CFG_ITEM(uint32, eeprom_total_size); /* real size of raw data */
  MTLK_CFG_ITEM_ARRAY(uint8, eeprom_raw_data, MTLK_MAX_EEPROM_SIZE)
MTLK_DECLARE_CFG_END(mtlk_eeprom_cfg_t)


/* TPC config */
MTLK_DECLARE_CFG_START(mtlk_tpc_cfg_t)
  MTLK_CFG_ITEM(uint32, loop_type)
MTLK_DECLARE_CFG_END(mtlk_tpc_cfg_t)

/* mbss add vap structure - special case: neither setter nor getter */
MTLK_DECLARE_CFG_START(mtlk_mbss_int_add_vap_cfg_t)
  MTLK_CFG_ITEM(uint32, added_vap_index)
MTLK_DECLARE_CFG_END(mtlk_mbss_int_add_vap_cfg_t)

#define MTLK_MBSS_VAP_LIMIT_DEFAULT ((uint32)-1)
#define VAP_LIMIT_SET_SIZE           (2)

MTLK_DECLARE_CFG_START(mtlk_mbss_cfg_t)
  MTLK_CFG_ITEM(uint32, deleted_vap_index)
  MTLK_CFG_ITEM(uint32, added_vap_index)
  MTLK_CFG_ITEM(uint8, role)
  /* staying here for future use, when we can initialize master vap after init */
  MTLK_CFG_ITEM(BOOL, is_master)
  MTLK_CFG_ITEM(const char *, added_vap_name)
  MTLK_CFG_ITEM(void *, wiphy)
  MTLK_CFG_ITEM(void *, vap_handle)
  MTLK_CFG_ITEM(void *, df_user)
  MTLK_CFG_ITEM(void *, ndev)
MTLK_DECLARE_CFG_END(mtlk_mbss_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_rcvry_cfg_t)
  MTLK_CFG_ITEM(wave_rcvry_cfg_t, recovery_cfg)
MTLK_DECLARE_CFG_END(mtlk_rcvry_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_rcvry_stats_t)
  MTLK_CFG_ITEM(mtlk_wssa_drv_recovery_stats_t, recovery_stats)
MTLK_DECLARE_CFG_END(mtlk_rcvry_stats_t)

/* NFRP Configuration */
MTLK_DECLARE_CFG_START(mtlk_nfrp_cfg_t)
  MTLK_CFG_ITEM(UMI_NFRP_CONFIG, nfrp_cfg_params)
MTLK_DECLARE_CFG_END(mtlk_nfrp_cfg_t)

typedef struct _mtlk_mhi_stats_t {
  uint32            mhi_stats_size;
  uint8            *mhi_stats_data;
} mtlk_mhi_stats_t;

typedef struct {
  uint32         size;
  uint8         *buff;
} wave_bin_data_t;

typedef struct _mtlk_beacon_interval_t {
    uint32 beacon_interval;
    uint32 vap_id;
} mtlk_beacon_interval_t;

typedef struct _mtlk_operating_mode_t {
  uint8 channel_width;
  uint8 rx_nss;
  uint16 station_id;
} mtlk_operating_mode_t;

typedef struct mtlk_vap_stats64_t{
  uint64 txFrames;
  uint64 rxFrames;
  uint64 txBytes;
  uint64 rxBytes;
}mtlk_vap_stats64;

typedef struct mtlk_vap_stats_t{
  uint32 txInUnicastHd;
  uint32 txInMulticastHd;
  uint32 txInBroadcastHd;
  uint32 txInUnicastNumOfBytes;
  uint32 txInMulticastNumOfBytes;
  uint32 txInBroadcastNumOfBytes;
  uint32 rxOutUnicastHd;
  uint32 rxOutMulticastHd;
  uint32 rxOutBroadcastHd;
  uint32 rxOutUnicastNumOfBytes;
  uint32 rxOutMulticastNumOfBytes;
  uint32 rxOutBroadcastNumOfBytes;
  uint32 agerCount;
   /* Rx Counters */
  uint32 dropMpdu;
  uint32 mpduUnicastOrMngmnt;
  uint32 mpduRetryCount;
  uint32 amsdu;
  uint32 mpduTypeNotSupported;
  uint32 replayData;
  uint32 replayMngmnt;
  uint32 tkipCount;
  uint32 securityFailure;
  uint32 amsduBytes;
  uint32 dropCount;
  uint32 ampdu;
  uint32 mpduInAmpdu;
  uint32 octetsInAmpdu;
  uint32 rxClassifierSecurityMismatch;
  uint32 bcMcCountVap;
  /* Baa Counters */
  uint32 rtsSuccessCount;
  uint32 qosTransmittedFrames;
  uint32 transmittedAmsdu;
  uint32 transmittedOctetsInAmsdu;
  uint32 transmittedAmpdu;
  uint32 transmittedMpduInAmpdu;
  uint32 transmittedOctetsInAmpdu;
  uint32 beamformingFrames;
  uint32 transmitStreamRprtMSDUFailed;
  uint32 rtsFailure;
  uint32 ackFailure;
  uint32 failedAmsdu;
  uint32 retryAmsdu;
  uint32 multipleRetryAmsdu;
  uint32 amsduAckFailure;
  uint32 implicitBarFailure;
  uint32 explicitBarFailure;
  uint32 transmitStreamRprtMultipleRetryCount;
  uint32 transmitBw20;
  uint32 transmitBw40;
  uint32 transmitBw80;
  uint32 transmitBw160;
  uint32 rxGroupFrame;
  uint32 txSenderError;
  uint32 qosTxVap[ACCESS_CATEGORY_NUM];
}mtlk_vap_stats;

typedef struct _mtlk_mhi_stats_vap {
    mtlk_vap_stats64 stats64;
    mtlk_vap_stats stats;
} mtlk_mhi_stats_vap_t;


typedef struct _mtlk_core_ui_gen_data_t{
  WE_GEN_DATAEX_REQUEST request;
} mtlk_core_ui_gen_data_t;

typedef struct _mtlk_core_ui_range_info_t {
  uint8   num_bitrates;
  int     bitrates[BITRATES_MAX];
} mtlk_core_ui_range_info_t;

#define MTLK_AUTHENTICATION_OPEN_SYSTEM     0
#define MTLK_AUTHENTICATION_SHARED_KEY      1
#define MTLK_AUTHENTICATION_AUTO            2 /* STA connects to AP according to AP's algorithm */

#define WAVE_WEP_KEY_MAX_LENGTH             (104/8) /* MAX length of WEP key (104 bit, 13 octets) */
#define WAVE_WEP_NUM_DEFAULT_KEYS           4       /* MAX number of WEP keys   */
#define WAVE_WEP_KEY_WEP1_LENGTH            5
#define WAVE_WEP_KEY_WEP2_LENGTH            WAVE_WEP_KEY_MAX_LENGTH
#define WAVE_IGTK_NUM_KEYS                  2       /* MAX number of IGTK keys  */
#define WAVE_BIGTK_NUM_KEYS                 2       /* MAX number of BIGTK keys */

/* MAX key length. Must be same as WLAN_MAX_KEY_LEN (32, defined in Linux) */
#define WAVE_MAX_KEY_LEN                    (UMI_RSN_TK1_LEN + UMI_RSN_TK2_LEN)

typedef enum wave_key_type
{
	WAVE_KEY_IDX0       = 0,
	WAVE_KEY_IDX1       = 1,
	WAVE_KEY_IDX2       = 2,
	WAVE_KEY_IDX3       = 3,
	WAVE_KEY_IDX4_IGTK  = 4,
	WAVE_KEY_IDX5_IGTK  = 5,
	WAVE_KEY_IDX6_BIGTK = 6,
	WAVE_KEY_IDX7_BIGTK = 7,
	WAVE_NUM_KEYS
} wave_key_type_e;

typedef struct wave_wep_key
{
  uint8 length;
  uint8 data[WAVE_WEP_KEY_MAX_LENGTH];
} wave_wep_key_t;

typedef struct _mtlk_core_ui_enc_cfg_t {
  uint8          wep_enabled;
  uint8          key_id;
  wave_wep_key_t wep_key;
} mtlk_core_ui_enc_cfg_t;

typedef enum {
  MTLK_IP_NONE,
  MTLK_IPv4,
  MTLK_IPv6,
} ip_type_t;

typedef struct mtlk_mc_addr
{
  ip_type_t type;
  union {
    struct in_addr  ip4_addr;
    struct in6_addr ip6_addr;
  } src_ip, grp_ip;
} mtlk_mc_addr_t;

typedef struct mtlk_ip_netmask
{
  ip_type_t type;
  union {
    struct in_addr ip4_addr;
    struct in6_addr ip6_addr;
  } addr, mask;
} mtlk_ip_netmask_t;

typedef enum {
  MTLK_MC_STA_ACTION_NONE,
  MTLK_MC_STA_JOIN_GROUP,
  MTLK_MC_STA_LEAVE_GROUP
} mc_action_t;

typedef enum {
  MTLK_GRID_ADD,
  MTLK_GRID_DEL
} mc_grid_action_t;

typedef enum {
  MTLK_MC_STADB_ADD,    /*!< Add new MAC's into Multicast STA DB */
  MTLK_MC_STADB_DEL,    /*!< Delete MAC's from Multicast STA DB */
  MTLK_MC_STADB_UPD     /*!< Update MAC's list in Multicast STA DB */
} mc_stadb_action_t;

typedef struct _mtlk_core_ui_mc_action_t {
  mc_action_t         action;
  uint8               grp_id;
  IEEE_ADDR           sta_mac_addr;
  mtlk_mc_addr_t      mc_addr;
} mtlk_core_ui_mc_action_t;

typedef struct _mtlk_core_ui_mc_grid_action_t {
  mc_grid_action_t    action;
  uint8               grp_id;
  mtlk_mc_addr_t      mc_addr;
} mtlk_core_ui_mc_grid_action_t;

typedef struct _mtlk_core_ui_mc_update_sta_db_t {
  mc_stadb_action_t   action;
  uint8               grp_id;
  unsigned            macs_num;
  IEEE_ADDR          *macs_list;
  mtlk_mc_addr_t      mc_addr;
  uint32              vap_id;
} mtlk_core_ui_mc_update_sta_db_t;

MTLK_DECLARE_CFG_START(mtlk_core_ui_authorizing_cfg_t)
MTLK_CFG_ITEM(IEEE_ADDR, sta_addr)
MTLK_CFG_ITEM(BOOL, authorizing)
MTLK_CFG_ITEM(struct ieee80211_sta *, sta)
MTLK_DECLARE_CFG_END(mtlk_core_ui_authorizing_cfg_t)

typedef struct _mtlk_core_ui_encext_cfg_t
{
  uint16    alg_type;
  IEEE_ADDR sta_addr;
  uint16    key_idx;
  uint16    key_len;
  uint8     key[WAVE_MAX_KEY_LEN];
  uint8     rx_seq[6];
  struct ieee80211_sta * sta;
} mtlk_core_ui_encext_cfg_t;

typedef struct _mtlk_core_ui_group_pn_t
{
  uint8     key[WAVE_MAX_KEY_LEN];
  uint8     seq[8];
  uint16    key_len;
  uint16    seq_len;
  uint16    cipher;
  uint16    key_idx;
} mtlk_core_ui_group_pn_t;

typedef struct _mtlk_core_ui_default_key_cfg_t
{
  BOOL      is_mgmt_key;
  uint16    key_idx;
  IEEE_ADDR sta_addr;
} mtlk_core_ui_default_key_cfg_t;

typedef struct _mtlk_core_ui_get_stadb_status_req_t {
  uint8      get_hostdb;
  uint8      use_cipher;
} mtlk_core_ui_get_stadb_status_req_t;

typedef struct _mtlk_core_qos_map_info_t {
  uint8  dscp_table[DSCP_NUM];
} mtlk_core_qos_map_info_t;

/* Interference Detection */
MTLK_DECLARE_CFG_START(mtlk_interfdet_cfg_t)
  MTLK_CFG_ITEM(int, threshold);
MTLK_DECLARE_CFG_END(mtlk_interfdet_cfg_t)


MTLK_DECLARE_CFG_START(mtlk_emulate_interferer_t)
  MTLK_CFG_ITEM(int, status);
MTLK_DECLARE_CFG_END(mtlk_emulate_interferer_t)

typedef struct
{
  uint16  maxRate;
  uint8   mode;
} __MTLK_IDATA mtlk_agg_rate_limit_req_cfg_t;

MTLK_DECLARE_CFG_START(mtlk_agg_rate_limit_cfg_t);
  MTLK_CFG_ITEM(mtlk_agg_rate_limit_req_cfg_t, agg_rate_limit);
MTLK_DECLARE_CFG_END(mtlk_agg_rate_limit_cfg_t)

typedef struct
{
  uint8  mode;
  uint8  maxRate;
} __MTLK_IDATA mtlk_multicast_rate_req_cfg_t;

MTLK_DECLARE_CFG_START(mtlk_multicast_rate_cfg_t);
  MTLK_CFG_ITEM(mtlk_multicast_rate_req_cfg_t, multicast_rate);
MTLK_DECLARE_CFG_END(mtlk_multicast_rate_cfg_t)

typedef struct
{
  uint8   mode;
  uint32  bfPeriod;
} __MTLK_IDATA mtlk_mu_ofdma_bf_req_cfg_t;

MTLK_DECLARE_CFG_START(mtlk_mu_ofdma_bf_cfg_t);
  MTLK_CFG_ITEM(mtlk_mu_ofdma_bf_req_cfg_t, mu_ofdma_bf);
MTLK_DECLARE_CFG_END(mtlk_mu_ofdma_bf_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_rx_th_cfg_t)
  MTLK_CFG_ITEM(int8, rx_threshold);
MTLK_DECLARE_CFG_END(mtlk_rx_th_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_rx_duty_cycle_cfg_t)
  MTLK_CFG_ITEM(UMI_RX_DUTY_CYCLE, duty_cycle);
MTLK_DECLARE_CFG_END(mtlk_rx_duty_cycle_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_tx_power_lim_cfg_t)
  MTLK_CFG_ITEM(uint8, powerLimitOffset);
MTLK_DECLARE_CFG_END(mtlk_tx_power_lim_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_qamplus_mode_cfg_t)
  MTLK_CFG_ITEM(uint8, qamplus_mode);
MTLK_DECLARE_CFG_END(mtlk_qamplus_mode_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_radio_mode_cfg_t)
  MTLK_CFG_ITEM(BOOL, radio_mode);
MTLK_DECLARE_CFG_END(mtlk_radio_mode_cfg_t)

#define MTLK_AGGR_CFG_WINDOW_SIZE_DFLT  WINDOW_SIZE_NO_CHANGE

typedef struct mtlk_core_aggr_cfg
{
  uint8 amsdu_mode;
  uint8 ba_mode;
  uint32 windowSize;
} mtlk_core_aggr_cfg_t;

MTLK_DECLARE_CFG_START(mtlk_aggr_cfg_t)
  MTLK_CFG_ITEM(mtlk_core_aggr_cfg_t, cfg);
MTLK_DECLARE_CFG_END(mtlk_aggr_cfg_t)

#define WAVE_UNSOLICITED_FRAME_CFG_INVALID               0xFF
/* Hostapd - Driver interface commands */
#define WAVE_UNSOLICITED_FRAME_CFG_MODE_DISABLE          0
#define WAVE_UNSOLICITED_FRAME_CFG_MODE_PROBE_RESP       1
#define WAVE_UNSOLICITED_FRAME_CFG_MODE_FILS_DISC        2

typedef struct wave_unsolicited_frame_cfg
{
  uint8 mode;
  uint8 duration;
} wave_unsolicited_frame_cfg_t;

MTLK_DECLARE_CFG_START(wave_unsolicited_cfg_t)
  MTLK_CFG_ITEM(wave_unsolicited_frame_cfg_t, cfg);
MTLK_DECLARE_CFG_END(wave_unsolicited_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_amsdu_num_cfg_t)
  MTLK_CFG_ITEM(uint32, amsdu_num);
  MTLK_CFG_ITEM(uint32, amsdu_vnum);
  MTLK_CFG_ITEM(uint32, amsdu_henum);
MTLK_DECLARE_CFG_END(mtlk_amsdu_num_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_ht_protection_cfg_t)
  MTLK_CFG_ITEM(uint8, use_cts_prot);
MTLK_DECLARE_CFG_END(mtlk_ht_protection_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_temperature_sensor_t)
  MTLK_CFG_ITEM(uint32, temperature)
  MTLK_CFG_ITEM(uint32, calibrate_mask)
MTLK_DECLARE_CFG_END(mtlk_temperature_sensor_t)

typedef struct {
  uint32 voltage;
  int32  temperature;
} iwpriv_pvt_t;

MTLK_DECLARE_CFG_START(wave_pvt_sensor_t)
  MTLK_CFG_ITEM(iwpriv_pvt_t, pvt_params)
MTLK_DECLARE_CFG_END(wave_pvt_sensor_t)

MTLK_DECLARE_CFG_START(wave_ui_mode_t)
  MTLK_CFG_ITEM(int, mode) /* BOOL */
MTLK_DECLARE_CFG_END(wave_ui_mode_t)

#define MTLK_TX_RETRY_LIMIT_MAX        15
typedef struct {
  uint8 retry_limit_mgmt;
  uint8 retry_limit_data;
} __MTLK_IDATA mtlk_retry_limit_req_cfg_t;

MTLK_DECLARE_CFG_START(wave_retry_limit_cfg_t)
  MTLK_CFG_ITEM(mtlk_retry_limit_req_cfg_t, retry_limit)
MTLK_DECLARE_CFG_END(wave_retry_limit_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_exce_retry_limit_cfg_t)
  MTLK_CFG_ITEM(uint8, exce_retry_limit)
MTLK_DECLARE_CFG_END(mtlk_exce_retry_limit_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_max_mpdu_len_cfg_t)
  MTLK_CFG_ITEM(uint32, max_mpdu_length);
MTLK_DECLARE_CFG_END(mtlk_max_mpdu_len_cfg_t)

typedef struct mtlk_create_secondary_vap_cfg {
  mtlk_vap_handle_t vap_handle;
  char ifname[IFNAMSIZ + 1];
} mtlk_create_secondary_vap_cfg_t;

MTLK_DECLARE_CFG_START(mtlk_del_secondary_vaps_cfg_t)
  MTLK_CFG_ITEM(void *, vap_handle)
MTLK_DECLARE_CFG_END(mtlk_del_secondary_vaps_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_four_addr_cfg_t)
  MTLK_CFG_ITEM(IEEE_ADDR, addr_add)
  MTLK_CFG_ITEM(IEEE_ADDR, addr_del)
  MTLK_CFG_ITEM(mtlk_clpb_t *, sta_vect)
MTLK_DECLARE_CFG_END(mtlk_four_addr_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_stats_poll_period_cfg_t)
  MTLK_CFG_ITEM(uint32, period);
MTLK_DECLARE_CFG_END(mtlk_stats_poll_period_cfg_t)

typedef struct {
  BOOL whm_enable;
  BOOL whm_drv_warn;
  BOOL whm_fw_warn;
  BOOL whm_phy_warn;
} __MTLK_IDATA mtlk_whm_cfg_t;

MTLK_DECLARE_CFG_START(wave_whm_cfg_t)
  MTLK_CFG_ITEM(mtlk_whm_cfg_t, whm_cfg)
MTLK_DECLARE_CFG_END(wave_whm_cfg_t)

typedef struct {
  WhmFwWarningHandling_e warn_handled;
  BOOL  whm_hw_logger_enable;
} __MTLK_IDATA mtlk_whm_hwlogger_cfg_t;

MTLK_DECLARE_CFG_START(wave_whm_hwlogger_cfg_t)
  MTLK_CFG_ITEM(mtlk_whm_hwlogger_cfg_t, whm_hwlogger_cfg)
MTLK_DECLARE_CFG_END(wave_whm_hwlogger_cfg_t)

MTLK_DECLARE_CFG_START(wave_whm_reset_t)
  MTLK_CFG_ITEM(uint32, whm_reset)
MTLK_DECLARE_CFG_END(wave_whm_reset_t)

typedef struct {
  uint8 warning_id;
  uint8 layer_info;
}__MTLK_IDATA mtlk_whm_nl_event_msg_cfg_t;

MTLK_DECLARE_CFG_START(wave_whm_nl_event_msg_cfg_t)
  MTLK_CFG_ITEM(mtlk_whm_nl_event_msg_cfg_t, whm_nl_event_cfg)
MTLK_DECLARE_CFG_END(wave_whm_nl_event_msg_cfg_t)

MTLK_DECLARE_CFG_START(wave_whm_trigger_t)
  MTLK_CFG_ITEM(uint32, whm_warning_id)
  MTLK_CFG_ITEM(uint32, whm_warning_layer)
MTLK_DECLARE_CFG_END(wave_whm_trigger_t)

/* Calculate antennas factor depending on TX antennas number */
/* Exception: (-1) in case of 0 antennas number */
static __INLINE int
mtlk_antennas_factor(unsigned num)
{
  int table_log_mul10[] = {-1, 0, 24, 38, 48}; /* 8 * 10 * LOG10(num), num = [0..4] */
  MTLK_ASSERT(num < ARRAY_SIZE(table_log_mul10));
  return table_log_mul10[num];
}

/* Power: 8 uints per dBm */
#define POWER_PER_DBM       8
#define DBM_TO_POWER(dbm)   ((dbm) << 3)
#define POWER_TO_DBM(power) (((power) + 4) >> 3)  /* With rounding */
#define POWER_TO_MBM(power) POWER_TO_DBM(DBM_TO_MBM(power))

/* Power statistics: 2 uints per dBm */
#define STAT_PW_PER_DBM     2
#define DBM_TO_STAT_PW(dbm) ((dbm) * STAT_PW_PER_DBM)
#define STAT_PW_TO_DBM(pw)  (((pw) + (STAT_PW_PER_DBM / 2)) / STAT_PW_PER_DBM)  /* With rounding */
#define STAT_PW_TO_MBM(pw)  STAT_PW_TO_DBM(DBM_TO_MBM(pw))
#define STAT_PW_TO_POWER(pw) ((pw) * POWER_PER_DBM / STAT_PW_PER_DBM)

#define MTLK_POWER_NOT_SET -1

MTLK_DECLARE_CFG_START(mtlk_tx_power_cfg_t)
  MTLK_CFG_ITEM(uint32, tx_power);
MTLK_DECLARE_CFG_END(mtlk_tx_power_cfg_t)

#define NUM_SUPPORTED_BANDS NUM_NL80211_BANDS /* Though only 2.4/5/6 GHz bands are supported, this value is set to *
                                               * the number of bands defined. It is due to the fact that the 6GHz  *
                                               * is introduced in the nl80211_band definition after 60GHz so as    *
                                               * not to break any ABI. The Kernel runs some of the loops upto      *
                                               * NUM_SUPPORTED_BANDS and hence we are forced to define it this way */
#define NUM_IWPRIV_SCAN_PARAMS 6
#define NUM_IWPRIV_SCAN_PARAMS_BG 10

typedef struct iwpriv_scan_params
{
  uint32 passive_scan_time;
  uint32 active_scan_time;
  uint32 num_probe_reqs;
  uint32 probe_req_interval;
  uint32 passive_scan_valid_time;
  uint32 active_scan_valid_time;
} iwpriv_scan_params_t;

typedef struct iwpriv_scan_params_bg
{
  uint32 passive_scan_time;
  uint32 active_scan_time;
  uint32 num_probe_reqs;
  uint32 probe_req_interval;
  uint32 num_chans_in_chunk;
  uint32 chan_chunk_interval;
  uint32 chan_chunk_interval_busy;
  uint32 window_slice;
  uint32 window_slice_overlap;
  uint32 cts_to_self_duration; /* 0 - don't send CTS-to-self */
} iwpriv_scan_params_bg_t;

MTLK_DECLARE_CFG_START(mtlk_scan_and_calib_cfg_t);
  MTLK_CFG_ITEM(uint8, rbm); /* radar Bit Map */
  MTLK_CFG_ITEM(uint32, radar_detect);
  MTLK_CFG_ITEM(uint32, scan_modifs);
  MTLK_CFG_ITEM(uint32, scan_pause_bg_cache);
#ifdef CONFIG_WAVE_DEBUG
  MTLK_CFG_ITEM(uint32, scan_expire_time);
#endif
  MTLK_CFG_ITEM(iwpriv_scan_params_t, scan_params);
  MTLK_CFG_ITEM(iwpriv_scan_params_bg_t, scan_params_bg);
  MTLK_CFG_ITEM(int, out_of_scan_cache);
MTLK_DECLARE_CFG_END(mtlk_scan_and_calib_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_scan_during_cac_cfg_t)
  MTLK_CFG_ITEM(int, allow);
MTLK_DECLARE_CFG_END(mtlk_scan_during_cac_cfg_t)

typedef void (*vfunptr)(void);

MTLK_DECLARE_CFG_START(mtlk_ta_crit_t);
  MTLK_CFG_ITEM(uint32, id);
  MTLK_CFG_ITEM(uint32, signature);
  MTLK_CFG_ITEM(vfunptr, fcn);
  MTLK_CFG_ITEM(vfunptr, clb);
  MTLK_CFG_ITEM(uint32, clb_ctx);
  MTLK_CFG_ITEM(uint32, tmr_cnt);
  MTLK_CFG_ITEM(uint32, tmr_period);
MTLK_DECLARE_CFG_END(mtlk_ta_crit_t)

MTLK_DECLARE_CFG_START(mtlk_ta_wss_counter_t);
  MTLK_CFG_ITEM(uint32, prev);
  MTLK_CFG_ITEM(uint32, delta);
MTLK_DECLARE_CFG_END(mtlk_ta_wss_counter_t)

MTLK_DECLARE_CFG_START(mtlk_ta_sta_wss_t);
  MTLK_CFG_ITEM(IEEE_ADDR, addr);
  MTLK_CFG_ITEM(BOOL,   coc_wss_valid);
  MTLK_CFG_ITEM(mtlk_ta_wss_counter_t, coc_rx_bytes);
  MTLK_CFG_ITEM(mtlk_ta_wss_counter_t, coc_tx_bytes);
MTLK_DECLARE_CFG_END(mtlk_ta_sta_wss_t)

MTLK_DECLARE_CFG_START(mtlk_dot11w_cfg_t)
  MTLK_CFG_ITEM(uint32, pmf_activated);
  MTLK_CFG_ITEM(uint32, pmf_required);
  MTLK_CFG_ITEM(uint32, saq_retr_tmout);
  MTLK_CFG_ITEM(uint32, saq_max_tmout);
MTLK_DECLARE_CFG_END(mtlk_dot11w_cfg_t)

typedef struct {
  struct ieee80211_sta * sta;
  const uint8 *mac;
  struct station_info *stinfo; /* FIXME this also needs to be duplicated */
} st_info_data_t;

struct vendor_unconnected_sta_req_data_internal
{
  u32 chan_width; /* from enum nl80211_chan_width */
  u32 center_freq; /* in MHz */
  u32 cf1; /* in MHz */
  u32 cf2; /* in MHz */
  IEEE_ADDR addr;
  struct wireless_dev *wdev; /* wdev that requested the data, filled by driver */
};

struct driver_sta_info {
  mtlk_wssa_drv_peer_rates_info_t  rates_info;
  mtlk_wssa_drv_peer_stats_t       peer_stats;
  u32 max_rate;
};

struct driver_vap_info {
  mtlk_wssa_drv_tr181_wlan_stats_t vap_stats;
  u64 TransmittedOctetsInAMSDUCount;
  u64 ReceivedOctetsInAMSDUCount;
  u64 TransmittedOctetsInAMPDUCount;
  u64 ReceivedOctetsInAMPDUCount;
  u32 RTSSuccessCount;
  u32 RTSFailureCount;
  u32 TransmittedAMSDUCount;
  u32 FailedAMSDUCount;
  u32 AMSDUAckFailureCount;
  u32 ReceivedAMSDUCount;
  u32 TransmittedAMPDUCount;
  u32 TransmittedMPDUsInAMPDUCount;
  u32 AMPDUReceivedCount;
  u32 MPDUInReceivedAMPDUCount;
  u32 ImplicitBARFailureCount;
  u32 ExplicitBARFailureCount;
  u32 TwentyMHzFrameTransmittedCount;
  u32 FortyMHzFrameTransmittedCount;
  u32 SwitchChannel20To40;
  u32 SwitchChannel40To20;
  u32 FrameDuplicateCount;
};

struct driver_radio_info {
  mtlk_wssa_drv_tr181_hw_t tr181_hw;
  mtlk_wssa_drv_tr181_hw_stats_t tr181_hw_stats;
  u64 tsf_start_time; /* measurement start time */
  u8 load;
  u32 tx_pwr_cfg;
  u8 num_tx_antennas;
  u8 num_rx_antennas;
  u32 primary_center_freq; /* center frequency in MHz */
  u32 center_freq1;
  u32 center_freq2;
  u32 width; /* 20,40,80,... */
};

enum mtlk_sta_4addr_mode_e {
  STA_4ADDR_MODE_DYNAMIC    = -1,
  STA_4ADDR_MODE_OFF        = 0,
  STA_4ADDR_MODE_ON         = 1
};

#define MTLK_PACK_ON
#include "mtlkpack.h"

struct __MTLK_PACKED vendor_check_4addr_mode {
  enum mtlk_sta_4addr_mode_e sta_4addr_mode;
};

#define MTLK_PACK_OFF
#include "mtlkpack.h"

typedef struct {
  int    idx;
  uint8 *mac;
  struct station_info *stinfo;
} st_info_by_idx_data_t;

struct mgmt_tx_params
{
  const uint8 *buf;
  size_t len;
  int channum;
  uint64 *cookie;
  BOOL no_cck;
  BOOL dont_wait_for_ack;
  uint32 extra_processing;
  mtlk_nbuf_t *skb;
};


struct tasklet_limits
{
  uint32 data_txout_lim;
  uint32 data_rx_lim;
  uint32 bss_rx_lim;
  uint32 bss_cfm_lim;
  uint32 legacy_lim;
};

MTLK_DECLARE_CFG_START(mtlk_cts_to_self_to_cfg_t)
  MTLK_CFG_ITEM(unsigned, cts_to)
MTLK_DECLARE_CFG_END(mtlk_cts_to_self_to_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_tx_ampdu_density_cfg_t)
  MTLK_CFG_ITEM(unsigned, ampdu_density)
MTLK_DECLARE_CFG_END(mtlk_tx_ampdu_density_cfg_t)

typedef struct mtlk_dfs_state_cfg {
  struct cfg80211_chan_def chandef;
  enum nl80211_dfs_state   dfs_state;
  enum nl80211_radar_event event;
  uint8                    rbm;
  BOOL                     cac_started;
  mtlk_dlist_entry_t       lentry;
} mtlk_dfs_state_cfg_t;

/* CCA threshold */
#define MTLK_CCA_TH_PARAMS_LEN  5   /* number of params */

typedef struct iwpriv_cca_th
{
  int8  values[MTLK_CCA_TH_PARAMS_LEN];
  int8  is_updated;
} iwpriv_cca_th_t;

MTLK_DECLARE_CFG_START(mtlk_cca_th_cfg_t)
  MTLK_CFG_ITEM(iwpriv_cca_th_t, cca_th_params);
MTLK_DECLARE_CFG_END(mtlk_cca_th_cfg_t)


/* adaptive CCA */

#define MTLK_CCA_ADAPT_PARAMS_LEN  7   /* number of params */

typedef struct iwpriv_cca_adapt
{
  u32  initial;
  u32  iterative;
  int  limit;
  u32  step_up;
  u32  step_down;
  u32  step_down_interval;
  u32  min_unblocked_time;
} iwpriv_cca_adapt_t;

struct mtlk_cca_adapt
{
  int8 cca_th_params[MTLK_CCA_TH_PARAMS_LEN];
  u32  step_down_coef;
  u32  last_unblocked_time;
  BOOL stepping_down;
  BOOL stepping_up;
  u32  interval;
  BOOL cwi_poll;
  BOOL cwi_drop_detected;
  u32  cwi_poll_ts;
};

MTLK_DECLARE_CFG_START(mtlk_cca_adapt_cfg_t)
  MTLK_CFG_ITEM(iwpriv_cca_adapt_t, cca_adapt_params);
MTLK_DECLARE_CFG_END(mtlk_cca_adapt_cfg_t)

/* Radar RSSI Threshold */
#define WAVE_RADAR_DETECTION_RSSI_TH_INVALID    (RADAR_DETECTION_RSSI_TH_MAX + 1)
MTLK_DECLARE_CFG_START(mtlk_radar_rssi_th_cfg_t)
  MTLK_CFG_ITEM(int, radar_rssi_th);
MTLK_DECLARE_CFG_END(mtlk_radar_rssi_th_cfg_t)

/* Multicast configuration structures */

typedef enum {
  MTLK_MCAST_ACTION_CLEANUP,
  MTLK_MCAST_ACTION_ADD,
  MTLK_MCAST_ACTION_DEL,
  MTLK_MCAST_ACTION_LAST
} mtlk_mcast_action_t;

typedef struct mtlk_mcast_range {
  mtlk_mcast_action_t action;
  mtlk_ip_netmask_t   netmask;
} mtlk_mcast_range_t;

MTLK_DECLARE_CFG_START(mtlk_mcast_range_cfg_t)
  MTLK_CFG_ITEM(struct mtlk_mcast_range, range)
MTLK_DECLARE_CFG_END(mtlk_mcast_range_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_forward_unknown_mcast_cfg_t)
  MTLK_CFG_ITEM(uint32, flag);
MTLK_DECLARE_CFG_END(mtlk_forward_unknown_mcast_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_mcast_range_vect_cfg_t)
  MTLK_CFG_ITEM(mtlk_clpb_t *, range_vect)
MTLK_DECLARE_CFG_END(mtlk_mcast_range_vect_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_wlan_rts_rate_cfg_t)
  MTLK_CFG_ITEM(unsigned, cutoff_point)
MTLK_DECLARE_CFG_END(mtlk_wlan_rts_rate_cfg_t)

/* PHY in band power */
typedef struct {
  int32  noise_estim[4]; /* per antenna */
  int32  system_gain[4]; /* per antenna */
  uint32 ant_mask;
} mtlk_phy_inband_power_data_t;

MTLK_DECLARE_CFG_START(mtlk_phy_inband_power_cfg_t)
  MTLK_CFG_ITEM(mtlk_phy_inband_power_data_t, power_data)
MTLK_DECLARE_CFG_END(mtlk_phy_inband_power_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_wlan_stations_stats_enabled_cfg_t)
  MTLK_CFG_ITEM(unsigned, enabled)
MTLK_DECLARE_CFG_END(mtlk_wlan_stations_stats_enabled_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_wlan_rts_threshold_cfg_t)
  MTLK_CFG_ITEM(unsigned, threshold)
MTLK_DECLARE_CFG_END(mtlk_wlan_rts_threshold_cfg_t)

/* The CW_ values must be 0, 1, 2, 3, X, where X >= 4. */
enum chanWidth
{
  CW_20 = 0,
  CW_40,
  CW_80,
  CW_160,
  CW_80_80,
  CW_DEFAULT     = CW_20,
  CW_MIN         = CW_20,
  CW_MAX_2G      = CW_40,
  CW_MAX_5G_GEN5 = CW_80,
  CW_MAX_5G_GEN6 = CW_160
};

static __INLINE BOOL
wave_chan_width_is_supported(enum chanWidth cw)
{
  return ((CW_MIN <= cw) && (cw <= CW_160));
}

/* mimic the linux struct ieee80211_channel with our own, skip the fields that we don't use yet */
struct mtlk_channel
{
  unsigned long dfs_state_entered;
  unsigned dfs_state; /* from enum mtlk_dfs_state */
  unsigned dfs_cac_ms;
  mtlk_hw_band_e band;
  u32 center_freq; /* center frequency in MHz */
  u32 flags; /* channel flags from enum mtlk_channel_flags */
  u32 orig_flags;
  int max_antenna_gain;
  int max_power;
  int max_reg_power;
};

enum mtlk_sb_dfs_bw {
  MTLK_SB_DFS_BW_NORMAL,
  MTLK_SB_DFS_BW_20,
  MTLK_SB_DFS_BW_40,
  MTLK_SB_DFS_BW_80,
};

typedef struct mtlk_sb_dfs_params
{
  u32 center_freq;
  u32 width; /* from enum chanWidth */
  u8 sb_dfs_bw; /* from enum mtlk_sb_dfs_bw */
} mtlk_sb_dfs_params_t;

/* mimic the linux struct cfg80211_channel with our own */
struct mtlk_chan_def
{
  /* mtlk_osal_spinlock_t chan_lock; */
  u32 center_freq1;
  u32 center_freq2;
  u32 width; /* from enum chanWidth */
  struct mtlk_channel chan; /* primary channel spec */
  BOOL is_noht;
  BOOL wait_for_beacon;
  mtlk_sb_dfs_params_t sb_dfs;
  int power_level; /* requested TX power (in dBm), minimum of all interfaces */
};

static __INLINE
BOOL __mtlk_is_sb_dfs_switch (enum mtlk_sb_dfs_bw bw)
{
  return MTLK_SB_DFS_BW_20 == bw ||
         MTLK_SB_DFS_BW_40 == bw ||
         MTLK_SB_DFS_BW_80 == bw;
}

/* Conversion between NL80211 band and MTLK band */
typedef enum nl80211_band  nl80211_band_e;
typedef enum mtlk_hw_band  mtlk_hw_band_e;

/* 6GHz band support */

/* First two elements in nl80211_band and mtlk_hw_band contain 5/2Ghz,
   but in different order:

   In nl80211_band:
     NL80211_BAND_2GHZ  - 0
     NL80211_BAND_5GHZ  - 1
     NL80211_BAND_60GHZ - 2  unsupported by HW
     NL80211_BAND_6GHZ  - 3

   In enum mtlk_hw_band:
     MTLK_HW_BAND_5_2_GHZ - 0
     MTLK_HW_BAND_2_4_GHZ - 1
     MTLK_HW_BAND_6_GHZ   - 2
*/

static __INLINE
mtlk_hw_band_e nlband2mtlkband(nl80211_band_e band)
{
  /* Tri band support */

  mtlk_hw_band_e mtlkband = MTLK_HW_BAND_MAX;
  MTLK_ASSERT(band == NL80211_BAND_2GHZ || band == NL80211_BAND_5GHZ || band == NL80211_BAND_6GHZ);
  switch (band) {
   case NL80211_BAND_2GHZ:
        mtlkband = MTLK_HW_BAND_2_4_GHZ;
        break;
   case NL80211_BAND_5GHZ:
        mtlkband = MTLK_HW_BAND_5_2_GHZ;
        break;
   case NL80211_BAND_6GHZ:
      mtlkband = MTLK_HW_BAND_6_GHZ;
      break;
   default:
      break;
  }
  return mtlkband;
}

static __INLINE
BOOL mtlk_nl_band_is_supported (nl80211_band_e band)
{
  if (NL80211_BAND_60GHZ == band)
    return false;
  return true;
}

static __INLINE
nl80211_band_e mtlkband2nlband(mtlk_hw_band_e band)
{
  /* Tri band support */
  nl80211_band_e nlband = NL80211_BAND_60GHZ;
  MTLK_ASSERT(band == MTLK_HW_BAND_2_4_GHZ || band == MTLK_HW_BAND_5_2_GHZ || band == MTLK_HW_BAND_6_GHZ);

  switch(band) {
    case MTLK_HW_BAND_2_4_GHZ:
       nlband = NL80211_BAND_2GHZ;
       break;
    case MTLK_HW_BAND_5_2_GHZ:
       nlband = NL80211_BAND_5GHZ;
       break;
    case MTLK_HW_BAND_6_GHZ:
       nlband = NL80211_BAND_6GHZ;
       break;

    default: break;
  }
  return nlband;  
}

/* Conversion from NL80211 chan-width constants to our chan-width constants */
static __INLINE
unsigned nlcw2mtlkcw(unsigned cw)
{
  switch (cw)
  {
  case NL80211_CHAN_WIDTH_20_NOHT:
  case NL80211_CHAN_WIDTH_20:
    return CW_20;
  case NL80211_CHAN_WIDTH_40:
    return CW_40;
  case NL80211_CHAN_WIDTH_80:
    return CW_80;
  case NL80211_CHAN_WIDTH_80P80:
    return CW_80_80;
  case NL80211_CHAN_WIDTH_160:
    return CW_160;
  default:
    MTLK_ASSERT(FALSE);
  }

  return CW_20;
};

/* Conversion from our chan-width constants to NL80211 chan-width constants */
static __INLINE
unsigned mtlkcw2nlcw(unsigned cw, BOOL is_noht)
{
  switch (cw)
  {
  case CW_20:
    return (is_noht ? NL80211_CHAN_WIDTH_20_NOHT : NL80211_CHAN_WIDTH_20);
  case CW_40:
    return NL80211_CHAN_WIDTH_40;
  case CW_80:
    return NL80211_CHAN_WIDTH_80;
  case CW_80_80:
    return NL80211_CHAN_WIDTH_80P80;
  case CW_160:
    return NL80211_CHAN_WIDTH_160;
  default:
    MTLK_ASSERT(0);
  }

  return NL80211_CHAN_WIDTH_20;
};

/* Conversion from our chan-width constants to real chan width */
static __INLINE
unsigned mtlkcw2cw(unsigned cw)
{
  return cw == CW_80_80 ? 80 : (1 << cw) * 20;
}

/* Conversion from our chan-width constants to strings */
static __INLINE
char* mtlkcw2str(unsigned cw)
{
  switch (cw) {
  case CW_20:
    return "20";
  case CW_40:
    return "40";
  case CW_80:
    return "80";
  case CW_160:
    return "160";
  case CW_80_80:
    return "80+80";
  default:
    return "Unknown";
  }
}


/* Conversion from center freq and given chanwidth encoding (0 for 20, 1 for 40, etc.) to freq of lowest 20 MHz chan */
static __INLINE
u32 freq2lowfreq(u32 center_freq, unsigned cw)
{
  return center_freq - mtlkcw2cw(cw) / 2 + 10;
}

/* Conversion from center freq and given chanwidth encoding (0 for 20, 1 for 40, etc.) to freq of highest 20 MHz chan */
static __INLINE
u32 freq2highfreq(u32 center_freq, unsigned cw)
{
  return center_freq + mtlkcw2cw(cw) / 2 - 10;
}

/* Conversion from center freq and given chanwidth encoding to the lowest 20 MHz channel number */
static __INLINE
int freq2lowchannum(u32 center_freq, unsigned cw)
{
  return ieee80211_frequency_to_channel(freq2lowfreq(center_freq, cw));
}

struct set_chan_param_data
{
  struct mtlk_chan_def chandef;
  u16 chan_switch_time; // number of tbtt-s before the CSA-type switch * beacon interval in ms
  u8 switch_type; // ST_NORMAL, ST_SCAN or ST_CSA
  u8 block_tx_pre; // block TX except beacons and csa action frames before the CSA-type switch
  u8 block_tx_post; // block all TX including beacons after the CSA-type switch
  u8 bg_scan; // set if a SCAN_type switch is part of a background scan
  u16 sid; //station index: used only when switch_type==ST_RSSI/ST_NORMAL_AFTER_RSSI
  /* rssi per antenna used only when switch_type==ST_NORMAL_AFTER_RSSI */
  s8 rssi[PHY_STATISTICS_MAX_RX_ANT];
  u8 vap_id;
  /* noise per antenna used only when switch_type==ST_NORMAL_AFTER_RSSI */
  u8 noise[PHY_STATISTICS_MAX_RX_ANT];
  /* Rate of last packet received switch_type==ST_NORMAL_AFTER_RSSI */
  u32 rate;
  u8 rf_gain[PHY_STATISTICS_MAX_RX_ANT];
};

enum mtlk_hidden_ssid
{
  MTLK_HIDDEN_SSID_NOT_IN_USE,
  MTLK_HIDDEN_SSID_ZERO_LEN,
  MTLK_HIDDEN_SSID_ZERO_CONTENTS
};

#define MTLK_MAX_NUM_BASIC_RATES 32

/* mimic cfg80211.h's bss_parameters */
struct mtlk_bss_parameters
{
  unsigned vap_id;
  int use_cts_prot;
  int use_short_preamble;
  int use_short_slot_time;
  int ap_isolate;
  int ht_opmode;
  u8 basic_rates[MTLK_MAX_NUM_BASIC_RATES];
  u8 basic_rates_len;
  s8 p2p_ctwindow, p2p_opp_ps;
};

struct mtlk_sta_bss_change_parameters
{
  const char *vif_name;
  struct ieee80211_supported_band **bands;
  mtlk_core_t *core;
  struct ieee80211_bss_conf *info;
  u32 changed;
  u8 vap_index;
};

struct mtlk_beacon_info_parameters
{
  uint8 vap_index;
  uint16 beacon_int;
  uint16 dtim_period;
};

#define MAX_ROC_DURATION_MS 5000
#define MAX_SCAN_SSIDS      16
#define MAX_MATCH_SETS      16
#define MAX_SCAN_IE_LEN     2048
#define NUM_2GHZ_CHANS      14
#define NUM_5GHZ_CHANS      33
#define NUM_TOTAL_CHANS     (NUM_2GHZ_CHANS + NUM_5GHZ_CHANS)

#define NUM_5GHZ_CENTRAL_FREQS_BW40     10  /* number of central freqs in 40 MHz mode */
#define NUM_5GHZ_CENTRAL_FREQS_BW80      5  /* number of central freqs in 80 MHz mode */
#define NUM_5GHZ_CENTRAL_FREQS_BW160     2  /* number of central freqs in 160 MHz mode */
#define NUM_TOTAL_CHAN_FREQS            (NUM_TOTAL_CHANS + NUM_5GHZ_CENTRAL_FREQS_BW40 +\
                                         NUM_5GHZ_CENTRAL_FREQS_BW80 + NUM_5GHZ_CENTRAL_FREQS_BW160)

/* For the channel statistics array dimensioning */
#define NUM_TOTAL_CHAN_STATS_SIZE       MAX(NUM_TOTAL_CHANS + 1, NUM_TOTAL_CHANS_6G + 1)

/* For the scan request channels */
#define NUM_TOTAL_CHAN_SCAN_SIZE       MAX(NUM_TOTAL_CHANS, NUM_TOTAL_CHANS_6G)

typedef enum {
  MTLK_SCAN_AP,
  MTLK_SCAN_STA,
  MTLK_SCAN_SCHED_AP,
  MTLK_SCAN_SCHED_STA,
  MTLK_SCAN_STA_ROC,
  MTLK_ROC_NORMAL,
  MTLK_SCAN_CCA,
} mtlk_scan_type_e;

struct mtlk_scan_request
{
  void *saved_request;
  void *wiphy;
  mtlk_scan_type_e type;
  u8 requester_vap_index;
  struct mtlk_channel channels[NUM_TOTAL_CHAN_SCAN_SIZE];
  u32 n_channels;
  u32 flags;
  u32 rates[NUM_SUPPORTED_BANDS]; /* [MTLK_HW_BAND_MAX] ??? */
  u32 interval;
  s32 min_rssi_thold;
  char ssids[MAX_SCAN_SSIDS][MTLK_ESSID_MAX_SIZE];
  int n_ssids;
  u8 ie[MAX_SCAN_IE_LEN];
  size_t ie_len;
  mtlk_country_code_t country_code;
  int duration;
};

enum UMI_AC {
  UMI_AC_BE,
  UMI_AC_BK,
  UMI_AC_VI,
  UMI_AC_VO,
  UMI_AC_NUM_ACS
};

/* Conversion from NL80211 AC queue numbers to ours */
static __INLINE
unsigned nlac2mtlkac(unsigned ac)
{
  switch (ac)
  {
  case NL80211_AC_VO:
    return UMI_AC_VO;
  case NL80211_AC_VI:
    return UMI_AC_VI;
  case NL80211_AC_BE:
    return UMI_AC_BE;
  case NL80211_AC_BK:
    return UMI_AC_BK;
  default:
    MTLK_ASSERT(0);
  }

  return 0;
};

/* mimic ieee80211_txq_params*/
struct mtlk_txq_params
{
  u16 txop;
  u16 cwmin;
  u16 cwmax;
  u8 aifs;
  u8 acm_flag;
  u8 ac;
};

#define MAX_PPE_THRESHOLD_SIZE 25
struct mtlk_he_debug_mode_data {
  struct ieee80211_he_cap_elem he_cap_elem;
  struct ieee80211_he_mcs_nss_supp he_mcs_nss_supp;
  u8 ppe_thresholds[MAX_PPE_THRESHOLD_SIZE];
  u8 he_debug_mode_enabled; /* Keep last */
};

struct mtlk_channel_status
{
  struct mtlk_channel *channel;
  u16 current_primary_chan_freq;
  u8 noise;
  u8 load;
};

typedef struct _mtlk_beacon_data_t
{
  void const *head;
  void const *tail;
  void const *probe_resp;
  void *data;
  uint32 dma_addr;
  uint16 head_len;
  uint16 tail_len;
  uint16 probe_resp_len;
  uint8 vap_idx;
  void *bmgr_priv;
} mtlk_beacon_data_t;

typedef struct _mtlk_radio_ap_tbl_item_t {
  const uint32            *abilities;
  uint32                   num_elems;
} mtlk_ab_tbl_item_t;

typedef struct _wave_connection_alive {
	uint32 tx_packets;
	uint32 rx_packets;
	BOOL probe_ap_after_scan;
} wave_connection_alive_t;

typedef enum
{
  MTLK_CORE_UI_ASSERT_TYPE_NONE,          /* Reset with */
  MTLK_CORE_UI_ASSERT_TYPE_FW_UMIPS,      /* 1 */
  MTLK_CORE_UI_ASSERT_TYPE_FW_LMIPS0,     /* 2 0 */
  MTLK_CORE_UI_ASSERT_TYPE_FW_LMIPS1,     /* 2 1 */
  MTLK_CORE_UI_ASSERT_TYPE_ALL_MACS,      /* 3 */
  MTLK_CORE_UI_ASSERT_TYPE_DRV_DIV0,      /* 4 */
  MTLK_CORE_UI_ASSERT_TYPE_DRV_BLOOP,     /* 5 */
  MTLK_CORE_UI_ASSERT_TYPE_LAST
} mtlk_core_ui_dbg_assert_type_e; /* MTLK_CORE_REQ_SET_MAC_ASSERT */

typedef enum
{
  MTLK_CORE_UI_RCVRY_TYPE_NONE = RCVRY_TYPE_NONE,
  MTLK_CORE_UI_RCVRY_TYPE_FAST = RCVRY_TYPE_FAST,
  MTLK_CORE_UI_RCVRY_TYPE_FULL = RCVRY_TYPE_FULL,
  MTLK_CORE_UI_RCVRY_TYPE_UNRECOVARABLE_ERROR = RCVRY_TYPE_UNRECOVARABLE_ERROR,
  MTLK_CORE_UI_RCVRY_TYPE_DUT = RCVRY_TYPE_DUT,
  MTLK_CORE_UI_RCVRY_TYPE_UNDEF = RCVRY_TYPE_UNDEF,
  MTLK_CORE_UI_RCVRY_TYPE_DBG = RCVRY_TYPE_DBG,
  MTLK_CORE_UI_RCVRY_TYPE_LAST
} mtlk_core_ui_wave_rcvry_type_e;


MTLK_DECLARE_CFG_START(mtlk_static_plan_cfg_t)
  MTLK_CFG_ITEM(UMI_STATIC_PLAN_CONFIG, config);
MTLK_DECLARE_CFG_END(mtlk_static_plan_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_advertise_btwt_sp_cfg_t)
  MTLK_CFG_ITEM(UMI_ADVERTISE_BC_TWT_SP, config);
MTLK_DECLARE_CFG_END(mtlk_advertise_btwt_sp_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_terminate_btwt_sp_cfg_t)
  MTLK_CFG_ITEM(UMI_TERMINATE_BC_TWT_SP, config);
MTLK_DECLARE_CFG_END(mtlk_terminate_btwt_sp_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_tx_twt_teardown_cfg_t)
  MTLK_CFG_ITEM(UMI_SEND_TWT_TEARDOWN_FRAME, config);
MTLK_DECLARE_CFG_END(mtlk_tx_twt_teardown_cfg_t);

/* SSB Mode Configuration */
typedef enum {
    MTLK_SSB_MODE_CFG_EN = 0,
    MTLK_SSB_MODE_CFG_20MODE,
    MTLK_SSB_MODE_CFG_SIZE
} mtlk_ssb_mode_cfg_e;

MTLK_DECLARE_CFG_START(mtlk_ssb_mode_cfg_t)
  MTLK_CFG_ITEM_ARRAY(uint8, params, MTLK_SSB_MODE_CFG_SIZE);
MTLK_DECLARE_CFG_END(mtlk_ssb_mode_cfg_t)

typedef struct _mtlk_coex_data_t
{
  uint8 coex_mode;
  uint32 activeWifiWindowSize;
  uint8 inactiveWifiWindowSize;
  uint8 cts2self_active;
} mtlk_coex_data_t;

MTLK_DECLARE_CFG_START(mtlk_coex_cfg_t)
  MTLK_CFG_ITEM(mtlk_coex_data_t, coex_data);
MTLK_DECLARE_CFG_END(mtlk_coex_cfg_t)

/* Frequency Jump Mode Configuration */
#define MTLK_FREQ_JUMP_MODE_DEFAULT  0
MTLK_DECLARE_CFG_START(mtlk_freq_jump_mode_cfg_t)
  MTLK_CFG_ITEM(uint32, freq_jump_mode)
MTLK_DECLARE_CFG_END(mtlk_freq_jump_mode_cfg_t)

/* Previous active channel configuration */
#define MTLK_CHANNEL_PREV_CFG_PRI_NUM_DEFAULT   0xFFFF
typedef struct _mtlk_channel_prev_data_t {
    uint16 pri_chan_num;
    uint16 sec_chan_freq;
} mtlk_channel_prev_data_t;

MTLK_DECLARE_CFG_START(mtlk_channel_prev_cfg_t)
  MTLK_CFG_ITEM(mtlk_channel_prev_data_t, prev_data);
MTLK_DECLARE_CFG_END(mtlk_channel_prev_cfg_t)


#define MTLK_BLOCK_TX_DEFAULT  0

/* Restricted AC Mode Configuration */
#define MTLK_PD_THRESH_CFG_SIZE                        3
#define MTLK_RESTRICTED_AC_MODE_CFG_SIZE               4
#define MTLK_RESTRICTED_AC_MODE_BITMAP_MASK            0xF
#define MTLK_RESTRICTED_AC_MODE_THRESH_EXIT_MAX        4096

MTLK_DECLARE_CFG_START(mtlk_restricted_ac_mode_cfg_t)
  MTLK_CFG_ITEM(UMI_SET_PD_THRESH, pd_thresh_params);
  MTLK_CFG_ITEM(UMI_SET_RESTRICTED_AC, ac_mode_params);
MTLK_DECLARE_CFG_END(mtlk_restricted_ac_mode_cfg_t)

typedef struct twt_params_int {
	uint32	individualWakeTimeLow;
	uint32	individualWakeTimeHigh;
	uint32	individualWakeInterval;
	uint32	individualMinWakeDuration;
	uint32 	isImplicit;
	uint32  isAnnounced;
	uint32  isTriggerEnabled;
	uint32 	agreementType;
	uint32	individualChannel;
	uint32  BroadcastTargetBeacon;
	uint32  BroadcastListenInterval;
}twt_params_int_t;

MTLK_DECLARE_CFG_START(ax_default_params_t)
  MTLK_CFG_ITEM(twt_params_int_t, twt_params);
MTLK_DECLARE_CFG_END(ax_default_params_t)

#define UMI_GET_TWT_PARAMS_SIZE 11
#define UMI_GET_AX_DEFAULT_SIZE (UMI_GET_TWT_PARAMS_SIZE)

/* Dynamic MU Type Configuration */
#define WAVE_DYNAMIC_MU_TYPE_CFG_SIZE 5
#define WAVE_DYNAMIC_MU_TYPE_DL_MU_TYPE_MIN 0
#define WAVE_DYNAMIC_MU_TYPE_DL_MU_TYPE_MAX 3
#define WAVE_DYNAMIC_MU_TYPE_DL_MU_TYPE_DEFAULT WAVE_DYNAMIC_MU_TYPE_DL_MU_TYPE_MIN /*OFDMA*/
#define WAVE_DYNAMIC_MU_TYPE_UL_MU_TYPE_MIN 0
#define WAVE_DYNAMIC_MU_TYPE_UL_MU_TYPE_MAX 3
#define WAVE_DYNAMIC_MU_TYPE_UL_MU_TYPE_DEFAULT WAVE_DYNAMIC_MU_TYPE_UL_MU_TYPE_MIN /*OFDMA*/
#define WAVE_DYNAMIC_MU_TYPE_MIN_STA_IN_GROUP_NUM_DEFAULT 8
#define WAVE_DYNAMIC_MU_TYPE_MAX_STA_IN_GROUP_NUM_DEFAULT 8
#define WAVE_DYNAMIC_MU_TYPE_CDB_CFG_DEFAULT 0 /* ??? */
/* HE MU Fixed Parameters Configuration */
#define WAVE_HE_MU_FIXED_PARAMTERS_CFG_SIZE 4
#define WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_MIN 0
#define WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_MAX 4
#define WAVE_HE_MU_FIXED_PARAMTERS_MU_SEQUENCE_DEFAULT 0xFF /* AUTO */
#define WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_MIN 0
#define WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_MAX 5
#define WAVE_HE_MU_FIXED_PARAMTERS_LTF_GI_DEFAULT 0xFF /* AUTO */
#define WAVE_HE_MU_FIXED_PARAMTERS_CODING_TYPE_MIN 0
#define WAVE_HE_MU_FIXED_PARAMTERS_CODING_TYPE_MAX 1
#define WAVE_HE_MU_FIXED_PARAMTERS_CODING_TYPE_DEFAULT 0xFF /* AUTO */
#define WAVE_HE_MU_FIXED_PARAMTERS_HE_RATE_DEFAULT 0 /* ??? */
/* HE MU Duration Configuration */
#define WAVE_HE_MU_DURATION_CFG_SIZE 4
#define WAVE_HE_MU_DURATION_PPDU_DURATION_DEFAULT 0 /* AUTO */
#define WAVE_HE_MU_DURATION_TXOP_DURATION_DEFAULT 0 /* AUTO */
#define WAVE_HE_MU_DURATION_TF_LENGTH_DEFAULT 0 /* AUTO */
#define WAVE_HE_MU_DURATION_NUM_REPETITIONS_DEFAULT 0xFF /* AUTO */

#define WAVE_PREAMBLE_PUNCTURE_CFG_SIZE 3

MTLK_DECLARE_CFG_START(wave_dynamic_mu_cfg_t)
  MTLK_CFG_ITEM(UMI_DYNAMIC_MU_TYPE, dynamic_mu_type_params);
  MTLK_CFG_ITEM(UMI_HE_MU_FIXED_PARAMTERS, he_mu_fixed_params);
  MTLK_CFG_ITEM(UMI_HE_MU_DURATION, he_mu_duration_params);
MTLK_DECLARE_CFG_END(wave_dynamic_mu_cfg_t)

#define WAVE_ETSI_PPDU_DURATION_LIMITS_MIN_SIZE 1
MTLK_DECLARE_CFG_START(wave_etsi_ppdu_duration_cfg_t)
  MTLK_CFG_ITEM(UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS,
                 umi_set_etsi_ppdu_duration_limits_req_params);
MTLK_DECLARE_CFG_END(wave_etsi_ppdu_duration_cfg_t)

/* Preamble Puncture CCA Override */
MTLK_DECLARE_CFG_START(mtlk_preamble_punct_cca_ov_cfg_t)
  MTLK_CFG_ITEM(UMI_PREAMBLE_PUNCT_CCA_OVERRIDE, preamble_punct_params);
MTLK_DECLARE_CFG_END(mtlk_preamble_punct_cca_ov_cfg_t)

/* Fixed LTF and GI */
MTLK_DECLARE_CFG_START(mtlk_fixed_ltf_and_gi_t)
  MTLK_CFG_ITEM(UMI_FIXED_LTF_AND_GI_REQ, fixed_ltf_and_gi_params)
MTLK_DECLARE_CFG_END(mtlk_fixed_ltf_and_gi_t)

/* DEBUG CONFIGURATION */
#ifdef CONFIG_WAVE_DEBUG
MTLK_DECLARE_CFG_START(mtlk_bf_explicit_cap_cfg_t)
  MTLK_CFG_ITEM(BOOL, bf_explicit_cap);
MTLK_DECLARE_CFG_END(mtlk_bf_explicit_cap_cfg_t)

#ifdef CPTCFG_IWLWAV_SET_PM_QOS
MTLK_DECLARE_CFG_START(mtlk_pm_qos_cfg_t)
  MTLK_CFG_ITEM(s32, cpu_dma_latency);
MTLK_DECLARE_CFG_END(mtlk_pm_qos_cfg_t)
#endif

MTLK_DECLARE_CFG_START(mtlk_wlan_counters_src_cfg_t)
  MTLK_CFG_ITEM(unsigned, src)
MTLK_DECLARE_CFG_END(mtlk_wlan_counters_src_cfg_t)

/* IRE Control */
MTLK_DECLARE_CFG_START(mtlk_ire_ctrl_cfg_t)
  MTLK_CFG_ITEM(int, ire_ctrl_value);
MTLK_DECLARE_CFG_END(mtlk_ire_ctrl_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_tasklet_limits_cfg_t)
  MTLK_CFG_ITEM(struct tasklet_limits, tl)
MTLK_DECLARE_CFG_END(mtlk_tasklet_limits_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_ta_debug_info_cfg_t)
  MTLK_CFG_ITEM(uint32, nof_crit);
  MTLK_CFG_ITEM(uint32, nof_sta_wss);
  MTLK_CFG_ITEM_ARRAY(mtlk_ta_crit_t, crit, TA_CRIT_NUM);
  MTLK_CFG_ITEM_ARRAY(mtlk_ta_sta_wss_t, sta_wss, WAVE_MAX_SID);
MTLK_DECLARE_CFG_END(mtlk_ta_debug_info_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_ta_cfg_t)
  MTLK_CFG_ITEM(uint32, timer_resolution);
  MTLK_CFG_ITEM(mtlk_ta_debug_info_cfg_t, debug_info);
MTLK_DECLARE_CFG_END(mtlk_ta_cfg_t)

#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT
/* PCOC structure */
MTLK_DECLARE_CFG_START(mtlk_pcoc_mode_cfg_t)
  MTLK_CFG_ITEM(BOOL,  is_enabled)
  MTLK_CFG_ITEM(BOOL,  is_active)
  MTLK_CFG_ITEM(uint8, traffic_state)
  MTLK_CFG_ITEM(mtlk_pcoc_params_t, params)
  MTLK_CFG_ITEM(uint32, pmcu_debug)
MTLK_DECLARE_CFG_END(mtlk_pcoc_mode_cfg_t)
#endif /*CPTCFG_IWLWAV_PMCU_SUPPORT*/

/* MAC watchdog configuration structure */
MTLK_DECLARE_CFG_START(mtlk_mac_wdog_cfg_t)
  MTLK_CFG_ITEM(uint16, mac_watchdog_timeout_ms)
  MTLK_CFG_ITEM(uint32, mac_watchdog_period_ms)
MTLK_DECLARE_CFG_END(mtlk_mac_wdog_cfg_t)

/* Fixed Rate */
typedef enum {
    MTLK_FIXED_RATE_CFG_SID = 0,
    MTLK_FIXED_RATE_CFG_AUTO,
    MTLK_FIXED_RATE_CFG_BW,
    MTLK_FIXED_RATE_CFG_PHYM,
    MTLK_FIXED_RATE_CFG_NSS,
    MTLK_FIXED_RATE_CFG_MCS,
    MTLK_FIXED_RATE_CFG_SCP,
    MTLK_FIXED_RATE_CFG_DCM,
    MTLK_FIXED_RATE_CFG_HE_EXTPARTIALBWDATA,
    MTLK_FIXED_RATE_CFG_HE_EXTPARTIALBWMNG,
    MTLK_FIXED_RATE_CFG_CHANGETYPE,
    MTLK_FIXED_RATE_CFG_SIZE
} mtlk_fixed_rate_cfg_e;

MTLK_DECLARE_CFG_START(mtlk_fixed_rate_cfg_t)
  MTLK_CFG_ITEM_ARRAY(uint32, params, MTLK_FIXED_RATE_CFG_SIZE);
MTLK_DECLARE_CFG_END(mtlk_fixed_rate_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_fw_debug_cfg_t);
  MTLK_CFG_ITEM(UMI_FW_DBG_REQ, FWDebugReq);
MTLK_DECLARE_CFG_END(mtlk_fw_debug_cfg_t)

MTLK_DECLARE_CFG_START(mtlk_dbg_cli_cfg_t);
  MTLK_CFG_ITEM(UMI_DBG_CLI_REQ,  DbgCliReq);
MTLK_DECLARE_CFG_END(mtlk_dbg_cli_cfg_t)

/* FW log severity configuration structure */
MTLK_DECLARE_CFG_START(mtlk_fw_log_severity_t)
  MTLK_CFG_ITEM(uint32, newLevel)
  MTLK_CFG_ITEM(uint32, targetCPU)
MTLK_DECLARE_CFG_END(mtlk_fw_log_severity_t)

MTLK_DECLARE_CFG_START(mtlk_debug_log_level_t)
  MTLK_CFG_ITEM(uint32, oid)
  MTLK_CFG_ITEM(int32, level)
  MTLK_CFG_ITEM(uint32, mode)
MTLK_DECLARE_CFG_END(mtlk_debug_log_level_t)

MTLK_DECLARE_CFG_START(mtlk_serializer_command_info_t)
  MTLK_CFG_ITEM(uint32, priority)
  MTLK_CFG_ITEM(BOOL, is_current)
  MTLK_CFG_ITEM(mtlk_slid_t, issuer_slid)
MTLK_DECLARE_CFG_END(mtlk_serializer_command_info_t)

#define MAX_DBG_FW_MSG_SIZE (sizeof(UMI_MAN))

typedef struct mtlk_debug_cmd_fw
{
  uint16 cmd_id;
  uint32 len;
  uint8 data[MAX_DBG_FW_MSG_SIZE];
} mtlk_debug_cmd_fw_t;

MTLK_DECLARE_CFG_START(mtlk_debug_cmd_fw_cfg_t)
  MTLK_CFG_ITEM(mtlk_debug_cmd_fw_t, debug_cmd);
MTLK_DECLARE_CFG_END(mtlk_debug_cmd_fw_cfg_t)

#endif /* CONFIG_WAVE_DEBUG */


#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"
void wave_radio_abilities_enable_vap_ops(mtlk_vap_handle_t vap_handle);
#ifdef UNUSED_CODE
void wave_radio_abilities_disable_vap_ops(mtlk_vap_handle_t vap_handle);
void wave_radio_abilities_disable_11b_abilities(mtlk_vap_handle_t vap_handle);
void wave_radio_abilities_enable_11b_abilities(mtlk_vap_handle_t vap_handle);
#endif /* UNUSED_CODE */
int  mtlk_core_dp_get_subif_cb (mtlk_vap_handle_t vap_handle, char *mac_addr, uint32_t *subif, uint8 vap_id, int32_t port_id);
BOOL mtlk_core_mcast_module_is_available(mtlk_core_t* nic);
void mtlk_core_mcast_group_id_action_serialized(mtlk_core_t* nic, mc_action_t action, int grp_id, uint8 *mac_addr, mtlk_mc_addr_t *mc_addr);
uint32 mtlk_core_mcast_handle_grid(mtlk_core_t* nic, mtlk_mc_addr_t *mc_addr, mc_grid_action_t action, int grp_id);
int mtlk_core_mcast_notify_fw(mtlk_vap_handle_t vap_handle, int action, int sta_id, int grp_id);

void mtlk_core_on_bss_drop_tx_que_full(mtlk_core_t *core);

/* unsolicited frame tx info structure per radio */
#define MAX_NUM_UNSOLICIT_TX_BUFS           2
#define GET_OTHER_UNSOLICIT_TX_BUF_INDEX(x) (((x) ^ 1) & 1)
typedef struct {
  uint8        buf_index;   /* index for driver's owned buffer */
  uint8        *templ[MAX_NUM_UNSOLICIT_TX_BUFS];   /* ping-pong buffers */
  uint32       dma_addr_hist[MAX_NUM_UNSOLICIT_TX_BUFS];
  uint32       len[MAX_NUM_UNSOLICIT_TX_BUFS];
} unsolicit_tx_frame_info_t;

#undef LOG_LOCAL_GID
#undef LOG_LOCAL_FID

#endif /* !__MTLK_COREUI_H__ */
