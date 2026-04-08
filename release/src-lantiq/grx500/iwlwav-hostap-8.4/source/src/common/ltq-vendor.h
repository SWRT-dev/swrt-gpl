/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

#ifndef _VENDOR_CMDS_H_
#define _VENDOR_CMDS_H_
/*
 * This file is a registry of identifier assignments from the Lantiq
 * OUI AC:9A:96 for purposes other than MAC address assignment. New identifiers
 * can be assigned through normal review process for changes to the upstream
 * hostap.git repository.
 */

#define OUI_LTQ 0xAC9A96

/**
 * enum ltq_nl80211_vendor_subcmds - LTQ nl80211 vendor command identifiers
 *
 * @LTQ_NL80211_VENDOR_SUBCMD_UNSPEC: Reserved value 0
 *
 * @LTQ_NL80211_VENDOR_SUBCMD_GET_AID: Get AID request.
 *
 * @LTQ_NL80211_VENDOR_SUBCMD_FREE_AID: Free AID request.
 *
 * @LTQ_NL80211_VENDOR_SUBCMD_TX_EAPOL: Send EAPoL request.
 *
 * @LTQ_NL80211_VENDOR_SUBCMD_SYNC_DONE: Sync the operations between hostapd and
 * driver
 */
enum ltq_nl80211_vendor_subcmds {
  LTQ_NL80211_VENDOR_SUBCMD_UNSPEC = 0,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AID = 1,
  LTQ_NL80211_VENDOR_SUBCMD_FREE_AID = 2,
  LTQ_NL80211_VENDOR_SUBCMD_TX_EAPOL = 3,
  LTQ_NL80211_VENDOR_SUBCMD_INITIAL_DATA = 4,
  LTQ_NL80211_VENDOR_SUBCMD_SYNC_DONE = 5,
  LTQ_NL80211_VENDOR_SUBCMD_DFS_DEBUG = 6,
  LTQ_NL80211_VENDOR_SUBCMD_CAC_COMPLETE = 7,
  LTQ_NL80211_VENDOR_SUBCMD_SET_DENY_MAC = 8,
  LTQ_NL80211_VENDOR_SUBCMD_STA_STEER = 9,
  LTQ_NL80211_VENDOR_SUBCMD_GET_STA_MEASUREMENTS = 10,
  LTQ_NL80211_VENDOR_SUBCMD_GET_UNCONNECTED_STA = 11,
  LTQ_NL80211_VENDOR_SUBCMD_GET_VAP_MEASUREMENTS = 12,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_INFO = 13,
  LTQ_NL80211_VENDOR_SUBCMD_SET_ATF_QUOTAS = 14,
  LTQ_NL80211_VENDOR_SUBCMD_SET_WDS_WPA_STA = 15,
  LTQ_NL80211_VENDOR_SUBCMD_BLOCK_TX = 16,
  LTQ_NL80211_VENDOR_SUBCMD_SET_DGAF_DISABLED = 17,
  LTQ_NL80211_VENDOR_SUBCMD_SET_BSS_LOAD = 18,
  LTQ_NL80211_VENDOR_SUBCMD_SET_MESH_MODE = 19,
  LTQ_NL80211_VENDOR_SUBCMD_CHECK_4ADDR_MODE = 20,
  LTQ_NL80211_VENDOR_SUBCMD_SET_DISABLE_MASTER_VAP = 21, /* obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_SET_SOFTBLOCK_THRESHOLDS = 22,
  LTQ_NL80211_VENDOR_SUBCMD_SET_SOFTBLOCK_DISABLE = 23,

  /* add new hostap sub commands here */

  LTQ_NL80211_VENDOR_SUBCMD_SET_LONG_RETRY_LIMIT = 100,
  LTQ_NL80211_VENDOR_SUBCMD_GET_LONG_RETRY_LIMIT = 101,
  LTQ_NL80211_VENDOR_SUBCMD_SET_DTIM_PERIOD = 102,
  LTQ_NL80211_VENDOR_SUBCMD_GET_DTIM_PERIOD = 103,
  LTQ_NL80211_VENDOR_SUBCMD_SET_BEACON_PERIOD = 104,
  LTQ_NL80211_VENDOR_SUBCMD_GET_BEACON_PERIOD = 105,

  LTQ_NL80211_VENDOR_SUBCMD_SET_11H_RADAR_DETECT = 106,
  LTQ_NL80211_VENDOR_SUBCMD_GET_11H_RADAR_DETECT = 107,
  LTQ_NL80211_VENDOR_SUBCMD_SET_11H_BEACON_COUNT = 108,
  LTQ_NL80211_VENDOR_SUBCMD_GET_11H_BEACON_COUNT = 109,
  LTQ_NL80211_VENDOR_SUBCMD_SET_11H_CH_CHECK_TIME = 110,
  LTQ_NL80211_VENDOR_SUBCMD_GET_11H_CH_CHECK_TIME = 111,
  LTQ_NL80211_VENDOR_SUBCMD_SET_NON_OCCUPATED_PRD = 112,
  LTQ_NL80211_VENDOR_SUBCMD_GET_NON_OCCUPATED_PRD = 113,
  LTQ_NL80211_VENDOR_SUBCMD_SET_11H_EMULAT_RADAR = 114,

  LTQ_NL80211_VENDOR_SUBCMD_SET_L2NAT_AGING_TIMEOUT = 115, /* Obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_GET_L2NAT_AGING_TIMEOUT = 116, /* Obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_SET_L2NAT_DEFAULT_HOST  = 117, /* Obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_GET_L2NAT_DEFAULT_HOST  = 118, /* Obsolete */

  LTQ_NL80211_VENDOR_SUBCMD_SET_ADD_PEERAP = 119,
  LTQ_NL80211_VENDOR_SUBCMD_SET_DEL_PEERAP = 120,
  LTQ_NL80211_VENDOR_SUBCMD_SET_PEERAP_KEY_IDX = 121,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PEERAP_KEY_IDX = 122,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PEERAP_LIST = 123,

  LTQ_NL80211_VENDOR_SUBCMD_SET_11AD_ACTIVE = 124,
  LTQ_NL80211_VENDOR_SUBCMD_GET_11AD_ACTIVE = 125,

  LTQ_NL80211_VENDOR_SUBCMD_SET_MAC_WATCHDOG_TIMEOUT_MS = 126,
  LTQ_NL80211_VENDOR_SUBCMD_GET_MAC_WATCHDOG_TIMEOUT_MS = 127,
  LTQ_NL80211_VENDOR_SUBCMD_SET_MAC_WATCHDOG_PERIOD_MS = 128,
  LTQ_NL80211_VENDOR_SUBCMD_GET_MAC_WATCHDOG_PERIOD_MS = 129,

  LTQ_NL80211_VENDOR_SUBCMD_SET_STA_KEEPALIVE_INTERVAL = 130,
  LTQ_NL80211_VENDOR_SUBCMD_GET_STA_KEEPALIVE_INTERVAL = 131,

  LTQ_NL80211_VENDOR_SUBCMD_SET_BRIDGE_MODE = 132,
  LTQ_NL80211_VENDOR_SUBCMD_GET_BRIDGE_MODE = 133,
  LTQ_NL80211_VENDOR_SUBCMD_SET_RELIABLE_MULTICAST = 134,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RELIABLE_MULTICAST = 135,
  LTQ_NL80211_VENDOR_SUBCMD_SET_AP_FORWARDING = 136,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AP_FORWARDING = 137,
  LTQ_NL80211_VENDOR_SUBCMD_GET_NETWORK_MODE = 138,
  LTQ_NL80211_VENDOR_SUBCMD_SET_BSS_BASIC_RATE_SET = 139,
  LTQ_NL80211_VENDOR_SUBCMD_GET_BSS_BASIC_RATE_SET = 140,
  LTQ_NL80211_VENDOR_SUBCMD_SET_HIDDEN_SSID = 141,
  LTQ_NL80211_VENDOR_SUBCMD_GET_HIDDEN_SSID = 142,
  LTQ_NL80211_VENDOR_SUBCMD_SET_UP_RESCAN_EXEMPTION_TIME = 143,
  LTQ_NL80211_VENDOR_SUBCMD_GET_UP_RESCAN_EXEMPTION_TIME = 144,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FW_LOG_SEVERITY = 145,

  LTQ_NL80211_VENDOR_SUBCMD_SET_WDS_HOST_TIMEOUT = 146,
  LTQ_NL80211_VENDOR_SUBCMD_GET_WDS_HOST_TIMEOUT = 147,

  LTQ_NL80211_VENDOR_SUBCMD_SET_HSTDB_LOCAL_MAC = 148,
  LTQ_NL80211_VENDOR_SUBCMD_GET_HSTDB_LOCAL_MAC = 149,

  LTQ_NL80211_VENDOR_SUBCMD_GET_EEPROM = 150,

  LTQ_NL80211_VENDOR_SUBCMD_SET_IRB_PINGER_ENABLED = 151, /* obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_GET_IRB_PINGER_ENABLED = 152, /* obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_SET_IRB_PINGER_STATS = 153, /* obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_GET_IRB_PINGER_STATS = 154, /* obsolete */

  LTQ_NL80211_VENDOR_SUBCMD_SET_DCDP_API_LITEPATH = 155,
  LTQ_NL80211_VENDOR_SUBCMD_GET_DCDP_API_LITEPATH = 156,
  LTQ_NL80211_VENDOR_SUBCMD_SET_DCDP_API_LITEPATH_COMP = 157,
  LTQ_NL80211_VENDOR_SUBCMD_GET_DCDP_API_LITEPATH_COMP = 158,

  LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_API = 159,
  LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_API = 160,
  LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_API_EXT =161,
  LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_API_EXT = 162,
  LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_VLAN_ID = 163,
  LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_VLAN_ID = 164,
  LTQ_NL80211_VENDOR_SUBCMD_SET_UDMA_VLAN_ID_EXT = 165,
  LTQ_NL80211_VENDOR_SUBCMD_GET_UDMA_VLAN_ID_EXT = 166,

  LTQ_NL80211_VENDOR_SUBCMD_SET_COC_POWER_MODE = 167,
  LTQ_NL80211_VENDOR_SUBCMD_GET_COC_POWER_MODE = 168,
  LTQ_NL80211_VENDOR_SUBCMD_SET_COC_AUTO_PARAMS = 169,
  LTQ_NL80211_VENDOR_SUBCMD_GET_COC_AUTO_PARAMS = 170,

  LTQ_NL80211_VENDOR_SUBCMD_SET_PCOC_POWER_MODE = 171,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PCOC_POWER_MODE = 172,
  LTQ_NL80211_VENDOR_SUBCMD_SET_PCOC_AUTO_PARAMS = 173,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PCOC_AUTO_PARAMS = 174,
  LTQ_NL80211_VENDOR_SUBCMD_SET_PCOC_PMCU_DEBUG = 175,

  LTQ_NL80211_VENDOR_SUBCMD_SET_PRM_ID_TPC_LOOP_TYPE = 176,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PRM_ID_TPC_LOOP_TYPE = 177,

  LTQ_NL80211_VENDOR_SUBCMD_SET_INTERFER_TIMEOUTS = 178,
  LTQ_NL80211_VENDOR_SUBCMD_SET_INTERFER_THRESH = 179,
  LTQ_NL80211_VENDOR_SUBCMD_SET_INTERFER_SCANTIMES = 180,
  LTQ_NL80211_VENDOR_SUBCMD_GET_INTERFER_MODE = 181,

  LTQ_NL80211_VENDOR_SUBCMD_GET_AP_CAPABILITIES_MAX_STAs = 182,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AP_CAPABILITIES_MAX_VAPs = 183,

  LTQ_NL80211_VENDOR_SUBCMD_SET_11B_ANTENNA_SELECTION = 184,
  LTQ_NL80211_VENDOR_SUBCMD_GET_11B_ANTENNA_SELECTION = 185,

  LTQ_NL80211_VENDOR_SUBCMD_SET_DBG_CLI = 186,
  LTQ_NL80211_VENDOR_SUBCMD_SET_FW_DEBUG = 187,

  LTQ_NL80211_VENDOR_SUBCMD_SET_TA_TIMER_RESOLUTION = 188,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TA_TIMER_RESOLUTION = 189,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TA_DBG = 190,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FW_RECOVERY = 191,
  LTQ_NL80211_VENDOR_SUBCMD_GET_FW_RECOVERY = 192,

  LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_PARAMS = 193,
  LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS = 194,

  LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_PARAMS_BG = 195,
  LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_PARAMS_BG = 196,

  LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_MODIFS = 197,
  LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_MODIFS = 198,

  LTQ_NL80211_VENDOR_SUBCMD_SET_CALIB_CW_MASKS = 199,
  LTQ_NL80211_VENDOR_SUBCMD_GET_CALIB_CW_MASKS = 200,

  LTQ_NL80211_VENDOR_SUBCMD_SET_SCAN_EXP_TIME = 201,
  LTQ_NL80211_VENDOR_SUBCMD_GET_SCAN_EXP_TIME = 202,

  LTQ_NL80211_VENDOR_SUBCMD_GET_GENL_FAMILY_ID = 203,

  LTQ_NL80211_VENDOR_SUBCMD_SET_TASKLET_LIMITS = 204,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TASKLET_LIMITS = 205,

  LTQ_NL80211_VENDOR_SUBCMD_SET_RADIO_MODE = 206,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RADIO_MODE = 207,

  LTQ_NL80211_VENDOR_SUBCMD_SET_AGGR_CONFIG = 208,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AGGR_CONFIG = 209,

  LTQ_NL80211_VENDOR_SUBCMD_SET_AMSDU_NUM = 210,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AMSDU_NUM = 211,

  LTQ_NL80211_VENDOR_SUBCMD_SET_AGG_RATE_LIMIT = 212,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AGG_RATE_LIMIT = 213,

  LTQ_NL80211_VENDOR_SUBCMD_SET_ADMISSION_CAPACITY = 216,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ADMISSION_CAPACITY = 217,

  LTQ_NL80211_VENDOR_SUBCMD_SET_RX_THRESHOLD = 218,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RX_THRESHOLD = 219,

  LTQ_NL80211_VENDOR_SUBCMD_SET_RX_DUTY_CYCLE = 220,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RX_DUTY_CYCLE = 221,

  LTQ_NL80211_VENDOR_SUBCMD_SET_TX_POWER_LIMIT_OFFSET = 222,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TX_POWER_LIMIT_OFFSET = 223,

  LTQ_NL80211_VENDOR_SUBCMD_SET_PROTECTION_METHOD = 224,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PROTECTION_METHOD = 225,

  LTQ_NL80211_VENDOR_SUBCMD_GET_BEAMFORM_EXPLICIT = 226,

  LTQ_NL80211_VENDOR_SUBCMD_SET_TEMPERATURE_SENSOR = 227,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TEMPERATURE_SENSOR = 228,

  LTQ_NL80211_VENDOR_SUBCMD_SET_QAMPLUS_MODE = 229,
  LTQ_NL80211_VENDOR_SUBCMD_GET_QAMPLUS_MODE = 230,

  LTQ_NL80211_VENDOR_SUBCMD_SET_ACS_UPDATE_TO = 231,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ACS_UPDATE_TO = 232,

  LTQ_NL80211_VENDOR_SUBCMD_SET_MU_OPERATION = 233,
  LTQ_NL80211_VENDOR_SUBCMD_GET_MU_OPERATION = 234,

  LTQ_NL80211_VENDOR_SUBCMD_SET_CCA_THRESHOLD = 235,
  LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_THRESHOLD = 236,

  LTQ_NL80211_VENDOR_SUBCMD_SET_CCA_ADAPT = 237,
  LTQ_NL80211_VENDOR_SUBCMD_GET_CCA_ADAPT = 238,

  LTQ_NL80211_VENDOR_SUBCMD_SET_RADAR_RSSI_TH = 239,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RADAR_RSSI_TH = 240,

  LTQ_NL80211_VENDOR_SUBCMD_SET_CPU_DMA_LATENCY = 241,
  LTQ_NL80211_VENDOR_SUBCMD_GET_CPU_DMA_LATENCY = 242,

  LTQ_NL80211_VENDOR_SUBCMD_SET_RTS_MODE = 243,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RTS_MODE = 244,

  LTQ_NL80211_VENDOR_SUBCMD_SET_MAX_MPDU_LENGTH = 245,
  LTQ_NL80211_VENDOR_SUBCMD_GET_MAX_MPDU_LENGTH = 246,

  LTQ_NL80211_VENDOR_SUBCMD_SET_BF_MODE = 247,
  LTQ_NL80211_VENDOR_SUBCMD_GET_BF_MODE = 248,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_RATE = 249,

  LTQ_NL80211_VENDOR_SUBCMD_SET_ACTIVE_ANT_MASK = 250,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ACTIVE_ANT_MASK = 251,

  LTQ_NL80211_VENDOR_SUBCMD_SET_IRE_CTRL_B = 252,
  LTQ_NL80211_VENDOR_SUBCMD_GET_IRE_CTRL_B = 253,

  LTQ_NL80211_VENDOR_SUBCMD_SET_4ADDR_MODE = 254, /* obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_GET_4ADDR_MODE = 255, /* obsolete */
  LTQ_NL80211_VENDOR_SUBCMD_SET_4ADDR_STA_ADD = 256,
  LTQ_NL80211_VENDOR_SUBCMD_SET_4ADDR_STA_DEL = 257,
  LTQ_NL80211_VENDOR_SUBCMD_GET_4ADDR_STA_LIST = 258,

  LTQ_NL80211_VENDOR_SUBCMD_SET_TXOP_CONFIG = 259,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TXOP_CONFIG = 260,

  LTQ_NL80211_VENDOR_SUBCMD_SET_SSB_MODE = 261,
  LTQ_NL80211_VENDOR_SUBCMD_GET_SSB_MODE = 262,

  LTQ_NL80211_VENDOR_SUBCMD_SET_COEX_CFG = 265,
  LTQ_NL80211_VENDOR_SUBCMD_GET_COEX_CFG = 266,

  LTQ_NL80211_VENDOR_SUBCMD_SET_PROBING_MASK = 267,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PROBING_MASK = 268,

  LTQ_NL80211_VENDOR_SUBCMD_SET_MCAST_RANGE_SETUP = 269,
  LTQ_NL80211_VENDOR_SUBCMD_GET_MCAST_RANGE_SETUP = 270,
  LTQ_NL80211_VENDOR_SUBCMD_SET_MCAST_RANGE_SETUP_IPV6 = 271,
  LTQ_NL80211_VENDOR_SUBCMD_GET_MCAST_RANGE_SETUP_IPV6 = 272,

  LTQ_NL80211_VENDOR_SUBCMD_SET_ONLINE_CALIBRATION_ALGO_MASK = 273,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ONLINE_CALIBRATION_ALGO_MASK = 274,
  LTQ_NL80211_VENDOR_SUBCMD_SET_CALIBRATION_ALGO_MASK = 275,
  LTQ_NL80211_VENDOR_SUBCMD_GET_CALIBRATION_ALGO_MASK = 276,
  LTQ_NL80211_VENDOR_SUBCMD_SET_USE_SHORT_CYCLIC_PREFIX = 277,
  LTQ_NL80211_VENDOR_SUBCMD_GET_USE_SHORT_CYCLIC_PREFIX = 278,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FREQ_JUMP_MODE = 279,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_POWER = 280,
  LTQ_NL80211_VENDOR_SUBCMD_GET_FIXED_POWER = 281,

  LTQ_NL80211_VENDOR_SUBCMD_SET_UNCONNECTED_STA_SCAN_TIME = 282,
  LTQ_NL80211_VENDOR_SUBCMD_GET_UNCONNECTED_STA_SCAN_TIME = 283,

  LTQ_NL80211_VENDOR_SUBCMD_SET_RESTRICTED_AC_MODE = 284,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RESTRICTED_AC_MODE = 285,

  LTQ_NL80211_VENDOR_SUBCMD_SET_PD_THRESHOLD = 286,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PD_THRESHOLD = 287,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FAST_DROP = 288,
  LTQ_NL80211_VENDOR_SUBCMD_GET_FAST_DROP = 289,

  LTQ_NL80211_VENDOR_SUBCMD_SET_ERP = 290,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FAST_DYNAMIC_MC_RATE = 291,
  LTQ_NL80211_VENDOR_SUBCMD_GET_FAST_DYNAMIC_MC_RATE = 292,

  LTQ_NL80211_VENDOR_SUBCMD_SET_COUNTERS_SRC = 293,
  LTQ_NL80211_VENDOR_SUBCMD_GET_COUNTERS_SRC = 294,

  LTQ_NL80211_VENDOR_SUBCMD_SET_FIXED_LTF_AND_GI = 295,
  LTQ_NL80211_VENDOR_SUBCMD_GET_FIXED_LTF_AND_GI = 296,

  LTQ_NL80211_VENDOR_SUBCMD_SET_STATIC_PLAN_CONFIG = 297,

  LTQ_NL80211_VENDOR_SUBCMD_SET_WDS_WEP_ENC_CFG = 298,

  LTQ_NL80211_VENDOR_SUBCMD_GET_PVT_SENSOR = 299,

  LTQ_NL80211_VENDOR_SUBCMD_SET_ENABLE_TEST_BUS = 300,

  LTQ_NL80211_VENDOR_SUBCMD_SET_HE_MU_OPERATION = 301,
  LTQ_NL80211_VENDOR_SUBCMD_GET_HE_MU_OPERATION = 302,

  LTQ_NL80211_VENDOR_SUBCMD_SET_RTS_RATE = 303,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RTS_RATE = 304,

  LTQ_NL80211_VENDOR_SUBCMD_SET_DBG_ASSERT = 305,

  LTQ_NL80211_VENDOR_SUBCMD_SET_NON_TRANSMIT_VAP = 306,
  LTQ_NL80211_VENDOR_SUBCMD_SET_MBSSID_VAP = 307,
  LTQ_NL80211_VENDOR_SUBCMD_SET_MBSSID_NUM_VAPS_IN_GROUP = 308,

  LTQ_NL80211_VENDOR_SUBCMD_GET_RCVRY_STATS = 309,

  LTQ_NL80211_VENDOR_SUBCMD_GET_ASSOCIATED_DEV_STATS = 310,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ASSOCIATED_DEV_RATE_INFO_RX_STATS = 311,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ASSOCIATED_DEV_RATE_INFO_TX_STATS = 312,
  LTQ_NL80211_VENDOR_SUBCMD_GET_CHANNEL_STATS = 313,

  LTQ_NL80211_VENDOR_SUBCMD_SET_STATS_POLL_PERIOD = 314,
  LTQ_NL80211_VENDOR_SUBCMD_GET_STATS_POLL_PERIOD = 315,

  LTQ_NL80211_VENDOR_SUBCMD_GET_ASSOCIATED_DEV_TID_STATS = 316,

  /* MTLK_LEGACY_STATISTICS start: Used by legacy statistics only */
  LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_LIST = 317,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_FLOW_STATUS = 318,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_CAPABILITIES = 319,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PEER_RATE_INFO = 320,
  LTQ_NL80211_VENDOR_SUBCMD_GET_RECOVERY_STATS = 321,
  LTQ_NL80211_VENDOR_SUBCMD_GET_HW_FLOW_STATUS = 322,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TR181_WLAN_STATS = 323,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TR181_HW_STATS = 324,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TR181_PEER_STATS = 325,
  /* MTLK_LEGACY_STATISTICS end */

  LTQ_NL80211_VENDOR_SUBCMD_GET_CDB_CFG = 326,
  LTQ_NL80211_VENDOR_SUBCMD_SET_NFRP_CFG = 327,

  LTQ_NL80211_VENDOR_SUBCMD_SET_MGMT_FRAMES_RATE = 329,

  LTQ_NL80211_VENDOR_SUBCMD_SET_DYNAMIC_MU_TYPE = 330,
  LTQ_NL80211_VENDOR_SUBCMD_GET_DYNAMIC_MU_TYPE = 331,
  LTQ_NL80211_VENDOR_SUBCMD_SET_HE_MU_FIXED_PARAMETERS = 332,
  LTQ_NL80211_VENDOR_SUBCMD_GET_HE_MU_FIXED_PARAMETERS = 333,
  LTQ_NL80211_VENDOR_SUBCMD_SET_HE_MU_DURATION = 334,
  LTQ_NL80211_VENDOR_SUBCMD_GET_HE_MU_DURATION = 335,

  LTQ_NL80211_VENDOR_SUBCMD_GET_PHY_INBAND_POWER = 336,
  LTQ_NL80211_VENDOR_SUBCMD_GET_HE_OPERATION = 337,

  LTQ_NL80211_VENDOR_SUBCMD_GET_HE_NON_ADVERTISED = 338,
  LTQ_NL80211_VENDOR_SUBCMD_SET_HE_OPERATION = 339,
  LTQ_NL80211_VENDOR_SUBCMD_SET_ZWDFS_ANT = 340,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ZWDFS_ANT = 341,

  LTQ_NL80211_VENDOR_SUBCMD_GET_TWT_PARAMETERS = 342,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AX_DEFAULT_PARAMS = 343,

  LTQ_NL80211_VENDOR_SUBCMD_SET_STATIONS_STATISTICS = 346,
  LTQ_NL80211_VENDOR_SUBCMD_GET_STATIONS_STATISTICS = 347,

  LTQ_NL80211_VENDOR_SUBCMD_SET_ETSI_PPDU_LIMITS = 348,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ETSI_PPDU_LIMITS = 349,

  LTQ_NL80211_VENDOR_SUBCMD_GET_DEV_DIAG_RESULT2 = 350,
  LTQ_NL80211_VENDOR_SUBCMD_GET_DEV_DIAG_RESULT3 = 351,

  LTQ_NL80211_VENDOR_SUBCMD_GET_DCDP_DATAPATH_MODE = 352,

  LTQ_NL80211_VENDOR_SUBCMD_GET_RTS_THRESHOLD = 353,

  LTQ_NL80211_VENDOR_SUBCMD_SET_PIE_CFG = 354,
  LTQ_NL80211_VENDOR_SUBCMD_GET_PIE_CFG = 355,

  LTQ_NL80211_VENDOR_SUBCMD_SET_HE_NON_ADVERTISED = 356,

  LTQ_NL80211_VENDOR_SUBCMD_SET_AP_RETRY_LIMIT = 357,
  LTQ_NL80211_VENDOR_SUBCMD_GET_AP_RETRY_LIMIT = 358,

  LTQ_NL80211_VENDOR_SUBCMD_CHANNEL_SWITCH_DEAUTH_CFG = 359,

  LTQ_NL80211_VENDOR_SUBCMD_SET_CTS_TO_SELF_TO = 360,
  LTQ_NL80211_VENDOR_SUBCMD_GET_CTS_TO_SELF_TO = 361,

  LTQ_NL80211_VENDOR_SUBCMD_GET_ERP_CFG = 362,

  LTQ_NL80211_VENDOR_SUBCMD_SET_TX_AMPDU_DENSITY = 363,
  LTQ_NL80211_VENDOR_SUBCMD_GET_TX_AMPDU_DENSITY = 364,

  LTQ_NL80211_VENDOR_SUBCMD_ADD_BACKHAUL_VRT_IFACE = 365,

  LTQ_NL80211_VENDOR_SUBCMD_SET_OUT_OF_SCAN_CACHING = 366,
  LTQ_NL80211_VENDOR_SUBCMD_GET_OUT_OF_SCAN_CACHING = 367,

  LTQ_NL80211_VENDOR_SUBCMD_GET_PRIORITY_GPIO = 368,

  LTQ_NL80211_VENDOR_SUBCMD_SET_HE_DEBUG_DATA = 369,
  LTQ_NL80211_VENDOR_SUBCMD_SET_ALLOW_SCAN_DURING_CAC = 370,
  LTQ_NL80211_VENDOR_SUBCMD_GET_ALLOW_SCAN_DURING_CAC = 371,

  LTQ_NL80211_VENDOR_SUBCMD_GET_PHY_CHAN_STATUS = 372,
  LTQ_NL80211_VENDOR_SUBCMD_SET_UNSOLICITED_FRAME_TX = 373,

  LTQ_NL80211_VENDOR_SUBCMD_SET_MU_OFDMA_BF = 374,
  LTQ_NL80211_VENDOR_SUBCMD_GET_MU_OFDMA_BF = 375,
  /* add new iw sub commands here */

  /* used to define LTQ_NL80211_VENDOR_SUBCMD_MAX below */
  __LTQ_NL80211_VENDOR_SUBCMD_AFTER_LAST,
  LTQ_NL80211_VENDOR_SUBCMD_MAX = __LTQ_NL80211_VENDOR_SUBCMD_AFTER_LAST - 1

};

enum ltq_nl80211_vendor_events {
  LTQ_NL80211_VENDOR_EVENT_RX_EAPOL = 0,
  LTQ_NL80211_VENDOR_EVENT_FLUSH_STATIONS = 1,
  LTQ_NL80211_VENDOR_EVENT_CHAN_DATA = 2,
  LTQ_NL80211_VENDOR_EVENT_UNCONNECTED_STA = 3,
  LTQ_NL80211_VENDOR_EVENT_WDS_CONNECT = 4,
  LTQ_NL80211_VENDOR_EVENT_WDS_DISCONNECT = 5,
  LTQ_NL80211_VENDOR_EVENT_CSA_RECEIVED = 6,
  LTQ_NL80211_VENDOR_EVENT_RADAR_DETECTED = 7,
  LTQ_NL80211_VENDOR_EVENT_ASSERT_DUMP_READY = 8,
  LTQ_NL80211_VENDOR_EVENT_NO_DUMP = 9,
  LTQ_NL80211_VENDOR_EVENT_UNRECOVARABLE_ERROR = 10,
  LTQ_NL80211_VENDOR_EVENT_MAC_ERROR = 11,
  LTQ_NL80211_VENDOR_EVENT_SOFTBLOCK_DROP = 12
};

#define HE_ADVERTISED_CAP_LEN 42

struct ltq_he_debug_mode_data {
	u8 enable_debug_mode;
	u8 he_debug_capab[HE_ADVERTISED_CAP_LEN];
	/* Add new debug info here */
};

static inline u8 ltq_find_bit_offset(u8 val)
{
        u8 res = 0;

        for (; val; val >>= 1) {
                if (val & 1)
                        break;
                res++;
        }

        return res;
}

static inline u8 ltq_set_he_cap(int val, u8 mask)
{
        return (u8) (mask & (val << ltq_find_bit_offset(mask)));
}

static inline void ltq_clr_set_he_cap(u8 *field, int val, u8 mask)
{
	*field &= ~mask;
	*field |= (u8) (mask & val);
}

/* IEEE 802.11AX DRAFT VER 4.3
 * HE MAC Capabilities Information field defines
 */
#define HE_MACCAP_CAP0_IDX 0
#define HE_MAC_CAP0_HTC_HE_SUPPORT ((u8) BIT(0))
#define HE_MAC_CAP0_TWT_REQUESTER_SUPPORT ((u8) BIT(1))
#define HE_MAC_CAP0_TWT_RESPONDER_SUPPORT ((u8) BIT(2))
#define HE_MAC_CAP0_FRAGMENTATION_SUPPORT ((u8) (BIT(3) | BIT(4)))
#define HE_MAC_CAP0_MAX_NUM_OF_FRAG_MSDU ((u8) (BIT(5) | BIT(6) | BIT(7)))

#define HE_MACCAP_CAP1_IDX 1
#define HE_MAC_CAP1_MINIMUM_FRAGMENT_SIZE ((u8) (BIT(0) | BIT(1)))
#define HE_MAC_CAP1_TRIGGER_FRAME_MAC_PAD_DUR ((u8) (BIT(2) | BIT(3)))
#define HE_MAC_CAP1_MULTI_TID_AGGR_RX_SUPPORT ((u8) (BIT(4) | BIT(5) | BIT(6)))
/* HE_MACCAP_HE_LINK_ADAPTION_SUPPORT  B15, B16 */
#define HE_MAC_CAP1_HE_LINK_ADAPTION_SUPPORT ((u8) BIT(7))

#define HE_MACCAP_CAP2_IDX 2
#define HE_MAC_CAP2_HE_LINK_ADAPTION_SUPPORT ((u8) BIT(0))
#define HE_MAC_CAP2_ALL_ACK_SUPPORT ((u8) BIT(1))
#define HE_MAC_CAP2_TRS_SUPPORT ((u8) BIT(2))
#define HE_MAC_CAP2_BSR_SUPPORT ((u8) BIT(3))
#define HE_MAC_CAP2_BROADCAST_TWT_SUPPORT ((u8) BIT(4))
#define HE_MAC_CAP2_32BIT_BA_BITMAP_SUPPORT ((u8) BIT(5))
#define HE_MAC_CAP2_MU_CASCADING_SUPPORT ((u8) BIT(6))
#define HE_MAC_CAP2_ACK_ENABLED_AGGREGATION_SUPPORT ((u8) BIT(7))

#define HE_MACCAP_CAP3_IDX 3
#define HE_MAC_CAP3_GROUP_ADD_MULTI_STA_BA_IN_DL_MU_SUP ((u8) BIT(0))
#define HE_MAC_CAP3_OM_CONTROL_SUPPORT ((u8) BIT(1))
#define HE_MAC_CAP3_OFDMA_RA_SUPPORT ((u8) BIT(2))
#define HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT ((u8) (BIT(3) | BIT(4)))
#define HE_MAC_CAP3_AMSDU_FRGMENTATION_SUPPORT ((u8) BIT(5))
#define HE_MAC_CAP3_FLEXIBLE_TWT_SCHEDULE_SUPPORT ((u8) BIT(6))
#define HE_MAC_CAP3_RX_CONTROL_FRAME_TO_MULTIBSS ((u8) BIT(7))

#define HE_MACCAP_CAP4_IDX 4
#define HE_MAC_CAP4_BSRP_BQRP_AMPDU_AGGREGATION ((u8) BIT(0))
#define HE_MAC_CAP4_QTP_SUPPORT ((u8) BIT(1))
#define HE_MAC_CAP4_BQR_SUPPORT ((u8) BIT(2))
#define HE_MAC_CAP4_SRP_RESPONDER ((u8) BIT(3))
#define HE_MAC_CAP4_NDP_FEEDBACK_REPORT_SUPPORT ((u8) BIT(4))
#define HE_MAC_CAP4_OPS_SUPPORT ((u8) BIT(5))
#define HE_MAC_CAP4_AMSDU_IN_AMPDU_SUPPORT ((u8) BIT(6))
/* HE_MACCAP_MULTI_TID_AGGREGATION_TX_SUPPORT B39, B40, B41 */
#define HE_MAC_CAP4_MULTI_TID_AGGR_TX_SUPPORT ((u8) (BIT(7)))

#define HE_MACCAP_CAP5_IDX 5
#define HE_MAC_CAP5_MULTI_TID_AGGR_TX_SUPPORT ((u8) (BIT(0) | BIT(1)))
#define HE_MAC_CAP5_HE_SUBCHANNEL_SELE_TRANS_SUP ((u8) BIT(2))
#define HE_MAC_CAP5_UL_2X996TONE_RU_SUPPORT ((u8) BIT(3))
#define HE_MAC_CAP5_OM_CONTROL_UL_MU_DATA_DIS_RX_SUP ((u8) BIT(4))
#define HE_MAC_CAP5_HE_DYNAMIC_SM_POWER_SAVE ((u8) BIT(5))
#define HE_MAC_CAP5_PUNCTURED_SOUNDING_SUPPORT ((u8) BIT(6))
#define HE_MAC_CAP5_HT_VHT_TRIGGER_FRAME_RX_SUPPORT ((u8) BIT(7))

#define HE_PHYCAP_CAP1_IDX 1
#define HE_PHY_CAP1_PUN_PREAM_RX ((u8) (BIT(0) | BIT(1) | BIT(2) | BIT(3)))
#define HE_PHY_CAP1_DEVICE_CLASS ((u8) BIT(4))
#define HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD ((u8) BIT(5))
#define HE_PHY_CAP1_SU_PPDU_1XHE_LTF_0_8US_GI ((u8) BIT(6))
/* HE_PHYCAP_MIDAMBLE_TXRX_MAX__NSTS B15 B16 */
#define HE_PHY_CAP1_MIDAMBLE_TXRX_MAX_NSTS ((u8) BIT(7))

#define HE_PHYCAP_CAP2_IDX 2
#define HE_PHY_CAP2_MIDAMBLE_TXRX_MAX_NSTS ((u8) BIT(0))
#define HE_PHY_CAP2_NDP_4X_HE_LTF_AND_3_2MS_GI ((u8) BIT(1))
#define HE_PHY_CAP2_STBC_TX_LESS_OR_EQUAL_80MHz ((u8) BIT(2))
#define HE_PHY_CAP2_STBC_RX_LESS_OR_EQUAL_80MHz ((u8) BIT(3))
#define HE_PHY_CAP2_DOPPLER_TX ((u8) BIT(4))
#define HE_PHY_CAP2_DOPPLER_RX ((u8) BIT(5))
#define HE_PHY_CAP2_FULL_BANDWIDTH_UL_MU_MIMO ((u8) BIT(6))
#define HE_PHY_CAP2_PARTIAL_BANDWIDTH_UL_MU_MIMO ((u8) BIT(7))

#define HE_PHYCAP_CAP3_IDX 3
#define HE_PHY_CAP3_DCM_MAX_CONSTELLATION_TX ((u8) (BIT(0) | BIT(1)))
#define HE_PHY_CAP3_DCM_MAX_NSS_TX ((u8) BIT(2))
#define HE_PHY_CAP3_DCM_MAX_CONSTELLATION_RX ((u8) (BIT(3) | BIT(4)))
#define HE_PHY_CAP3_DCM_MAX_NSS_RX ((u8) BIT(5))
#define HE_PHY_CAP3_RX_HE_MUPPDU_FROM_NON_AP_STA ((u8) BIT(6))
#define HE_PHY_CAP3_SU_BEAMFORMER ((u8) BIT(7))

#define HE_PHYCAP_CAP4_IDX 4
#define HE_PHY_CAP4_SU_BEAMFORMEE ((u8) BIT(0))
#define HE_PHY_CAP4_MU_BEAMFORMER ((u8) BIT(1))
#define HE_PHY_CAP4_BF_STS_LESS_OR_EQ_80MHz ((u8) (BIT(2) | BIT(3) | BIT(4)))
#define HE_PHY_CAP4_BF_STS_GREATER_THAN_80MHz ((u8) (BIT(5) | BIT(6) | BIT(7)))

#define HE_PHYCAP_CAP5_IDX 5
#define HE_PHY_CAP5_NUM_SOUND_DIM_LESS_80MHz ((u8) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PHY_CAP5_NUM_SOUND_DIM_GREAT_80MHz ((u8) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PHY_CAP5_NG_16_FOR_SU_FB_SUPPORT ((u8) BIT(6))
#define HE_PHY_CAP5_NG_16_FOR_MU_FB_SUPPORT ((u8) BIT(7))

#define HE_PHYCAP_CAP6_IDX 6
#define HE_PHY_CAP6_CODEBOOK_SIZE42_FOR_SU_SUPPORT ((u8) BIT(0))
#define HE_PHY_CAP6_CODEBOOK_SIZE75_FOR_MU_SUPPORT ((u8) BIT(1))
#define HE_PHY_CAP6_TRIGGERED_SU_BEAMFORMING_FEEDBACK ((u8) BIT(2))
#define HE_PHY_CAP6_TRIGGERED_MU_BEAMFORMING_PARTIAL_BW_FEEDBACK ((u8) BIT(3))
#define HE_PHY_CAP6_TRIGGERED_CQI_FEEDBACK ((u8) BIT(4))
#define HE_PHY_CAP6_PARTIAL_BANDWIDTH_EXTENDED_RANGE ((u8) BIT(5))
#define HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MU_MIMO ((u8) BIT(6))
#define HE_PHY_CAP6_PPE_THRESHOLD_PRESENT ((u8) BIT(7))

#define HE_PHYCAP_CAP7_IDX 7
#define HE_PHY_CAP7_SRP_BASED_SR_SUPPORT ((u8) BIT(0))
#define HE_PHY_CAP7_POWER_BOOST_FACTOR_SUPPORT ((u8) BIT(1))
#define HE_PHY_CAP7_SU_PPDU_AND_HE_MU_WITH_4X_HE_LTF_0_8US_GI ((u8) BIT(2))
#define HE_PHY_CAP7_MAX_NC ((u8) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PHY_CAP7_STBC_TX_GREATER_THAN_80MHz ((u8) BIT(6))
#define HE_PHY_CAP7_STBC_RX_GREATER_THAN_80MHz ((u8) BIT(7))

#define HE_PHYCAP_CAP8_IDX 8
#define HE_PHY_CAP8_HE_ER_SU_PPDU_4X_HE_LTF_0_8_US_GI ((u8) BIT(0))
#define HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_2_4_GHZ_BAND ((u8) BIT(1))
#define HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU ((u8) BIT(2))
#define HE_PHY_CAP8_80MHZ_IN_160MHZ_HE_PPDU ((u8) BIT(3))
#define HE_PHY_CAP8_HE_ER_SU_PPDU_1X_HE_LTF_0_8_US_GI ((u8) BIT(4))
#define HE_PHY_CAP8_MIDAMBLE_TX_RX_2X_AND_1X_HE_LTF ((u8) BIT(5))
#define HE_PHY_CAP8_DCM_MAX_BW ((u8) (BIT(6) | BIT(7)))

#define HE_PHYCAP_CAP9_IDX 9
#define HE_PHY_CAP9_LONGER_THAN_16_HE_SIGB_OFDM_SYMBOLS_SUPPORT ((u8) BIT(0))
#define HE_PHY_CAP9_NON_TRIGGERED_CQI_FEEDBACK ((u8) BIT(1))
#define HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU_SUPPORT ((u8) BIT(2))
#define HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU_SUPPORT ((u8) BIT(3))
#define HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_PPDU_NON_COMP_SIGB ((u8) BIT(4))
#define HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_PPDU_COMP_SIGB ((u8) BIT(5))
#define HE_PHY_CAP9_NOMINAL_PACKET_PADDING ((u8) (BIT(6) | BIT(7)))
/* B80..B87 - Reserved */

/* HE PPE Thresholds field */
#define HE_PPE_CAP0_IDX 0
#define HE_PPE_CAP0_NSS_M1 ((u8) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PPE_CAP0_RU_INDEX_BITMASK ((u8) (BIT(3) | BIT(4) | BIT(5) | BIT(6)))
#define HE_PPE_CAP0_PPET16_FOR_NSS1_FOR_RU0 ((u8) BIT(7))

#define HE_PPE_CAP1_IDX 1
#define HE_PPE_CAP1_PPET16_FOR_NSS1_FOR_RU0 ((u8) (BIT(0) | BIT(1)))
#define HE_PPE_CAP1_PPET8_FOR_NSS1_FOR_RU0 ((u8) (BIT(2) | BIT(3) | BIT(4)))
#define HE_PPE_CAP1_PPET16_FOR_NSS1_FOR_RU1 ((u8) (BIT(5) | BIT(6) | BIT(7)))

#define HE_PPE_CAP2_IDX 2
#define HE_PPE_CAP2_PPET8_FOR_NSS1_FOR_RU1 ((u8) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PPE_CAP2_PPET16_FOR_NSS1_FOR_RU2 ((u8) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PPE_CAP2_PPET8_FOR_NSS1_FOR_RU2 ((u8) (BIT(6) | BIT(7)))

#define HE_PPE_CAP3_IDX 3
#define HE_PPE_CAP3_PPET8_FOR_NSS1_FOR_RU2 (u8) BIT(0)
#define HE_PPE_CAP3_PPET16_FOR_NSS1_FOR_RU3 ((u8) (BIT(1) | BIT(2) | BIT(3)))
#define HE_PPE_CAP3_PPET8_FOR_NSS1_FOR_RU3 ((u8) (BIT(4) | BIT(5) | BIT(6)))
#define HE_PPE_CAP3_PPET16_FOR_NSS2_FOR_RU0 ((u8) BIT(7))

#define HE_PPE_CAP4_IDX 4
#define HE_PPE_CAP4_PPET16_FOR_NSS2_FOR_RU0 ((u8) (BIT(0) | BIT(1)))
#define HE_PPE_CAP4_PPET8_FOR_NSS2_FOR_RU0 ((u8) (BIT(2) | BIT(3) | BIT(4)))
#define HE_PPE_CAP4_PPET16_FOR_NSS2_FOR_RU1 ((u8) (BIT(5) | BIT(6) | BIT(7)))

#define HE_PPE_CAP5_IDX 5
#define HE_PPE_CAP5_PPET8_FOR_NSS2_FOR_RU1 ((u8) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PPE_CAP5_PPET16_FOR_NSS2_FOR_RU2 ((u8) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PPE_CAP5_PPET8_FOR_NSS2_FOR_RU2 ((u8) (BIT(6) | BIT(7)))

#define HE_PPE_CAP6_IDX 6
#define HE_PPE_CAP6_PPET8_FOR_NSS2_FOR_RU2 ((u8) BIT(0))
#define HE_PPE_CAP6_PPET16_FOR_NSS2_FOR_RU3 ((u8) (BIT(1) | BIT(2) | BIT(3)))
#define HE_PPE_CAP6_PPET8_FOR_NSS2_FOR_RU3 ((u8) (BIT(4) | BIT(5) | BIT(6)))
#define HE_PPE_CAP6_PPET16_FOR_NSS3_FOR_RU0 ((u8) BIT(7))

#define HE_PPE_CAP7_IDX 7
#define HE_PPE_CAP7_PPET16_FOR_NSS3_FOR_RU0 ((u8) (BIT(0) | BIT(1)))
#define HE_PPE_CAP7_PPET8_FOR_NSS3_FOR_RU0 ((u8) (BIT(2) | BIT(3) | BIT(4)))
#define HE_PPE_CAP7_PPET16_FOR_NSS3_FOR_RU1 ((u8) (BIT(5) | BIT(6) | BIT(7)))

#define HE_PPE_CAP8_IDX 8
#define HE_PPE_CAP8_PPET8_FOR_NSS3_FOR_RU1 ((u8) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PPE_CAP8_PPET16_FOR_NSS3_FOR_RU2 ((u8) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PPE_CAP8_PPET8_FOR_NSS3_FOR_RU2 ((u8) (BIT(6) | BIT(7)))

#define HE_PPE_CAP9_IDX 9
#define HE_PPE_CAP9_PPET8_FOR_NSS3_FOR_RU2 ((u8) BIT(0))
#define HE_PPE_CAP9_PPET16_FOR_NSS3_FOR_RU3 ((u8) (BIT(1) | BIT(2) | BIT(3)))
#define HE_PPE_CAP9_PPET8_FOR_NSS3_FOR_RU3 ((u8) (BIT(4) | BIT(5) | BIT(6)))
#define HE_PPE_CAP9_PPET16_FOR_NSS4_FOR_RU0 ((u8) BIT(7))

#define HE_PPE_CAP10_IDX 10
#define HE_PPE_CAP10_PPET16_FOR_NSS4_FOR_RU0 ((u8) (BIT(0) | BIT(1)))
#define HE_PPE_CAP10_PPET8_FOR_NSS4_FOR_RU0 ((u8) (BIT(2) | BIT(3) | BIT(4)))
#define HE_PPE_CAP10_PPET16_FOR_NSS4_FOR_RU1 ((u8) (BIT(5) | BIT(6) | BIT(7)))

#define HE_PPE_CAP11_IDX 11
#define HE_PPE_CAP11_PPET8_FOR_NSS4_FOR_RU1 ((u8) (BIT(0) | BIT(1) | BIT(2)))
#define HE_PPE_CAP11_PPET16_FOR_NSS4_FOR_RU2 ((u8) (BIT(3) | BIT(4) | BIT(5)))
#define HE_PPE_CAP11_PPET8_FOR_NSS4_FOR_RU2 ((u8) (BIT(6) | BIT(7)))

#define HE_PPE_CAP12_IDX 12
#define HE_PPE_CAP12_PPET8_FOR_NSS4_FOR_RU2 ((u8) BIT(0))
#define HE_PPE_CAP12_PPET16_FOR_NSS4_FOR_RU3 ((u8) (BIT(1) | BIT(2) | BIT(3)))
#define HE_PPE_CAP12_PPET8_FOR_NSS4_FOR_RU3 ((u8) (BIT(4) | BIT(5) | BIT(6)))

struct he_override_hw_capab {
	int  he_mac_fragmentation;
	int  he_mac_plus_htc_he_support;
	int  he_mac_twt_requester_support;
	int  he_mac_twt_responder_support;
	int  he_mac_all_ack_support;
	int  he_mac_bsr_support;
	int  he_mac_broadcast_twt_support;
	int  he_mac_32bit_ba_bitmap_support;
	int  he_mac_mu_cascading_support;
	int  he_mac_ack_enabled_aggregation_support;
	int  he_mac_group_addressed_multi_sta_blockack_in_dl_mu_support;
	int  he_mac_om_control_support;
	int  he_mac_ofdma_ra_support;
	int  he_mac_a_msdu_fragmentation_support;
	int  he_mac_flexible_twt_schedule_support;
	int  he_mac_rx_control_frame_to_multibss;
	int  he_mac_bsrp_bqrp_a_mpdu_aggregation;
	int  he_mac_qtp_support;
	int  he_mac_bqr_support;
	int  he_mac_a_msdu_in_ack_enabled_a_mpdu_support;
	int  he_mac_maximum_number_of_fragmented_msdus_amsdus;
	int  he_mac_minimum_fragment_size;
	int  he_mac_trigger_frame_mac_padding_duration;
	int  he_mac_multi_tid_aggregation_rx_support;
	int  he_mac_he_link_adaptation;
	int  he_mac_maximum_a_mpdu_length_exponent;
	int  he_mac_multi_tid_aggregation_tx_support;
	int  he_mac_ndp_feedback_report_support;
	int  he_mac_om_control_ul_mu_data_disable_rx_support;
	int  he_phy_preamble_puncturing_rx;
	int  he_phy_device_class;
	int  he_phy_ldpc_coding_in_payload;
	int  he_phy_su_beamformer_capable;
	int  he_phy_su_beamformee_capable;
	int  he_phy_mu_beamformer_capable;
	int  he_phy_ng_16_su_feedback;
	int  he_phy_ng_16_mu_feedback;
	int  he_phy_codebook_size42_for_su_support;
	int  he_phy_codebook_size75_for_mu_support;
	int  he_phy_ppe_thresholds_present;
	int  he_phy_srp_based_sr_support;
	int  he_phy_power_boost_factor_alpha_support;
	int  he_phy_su_ppdu_and_he_mu_with_4x_he_ltf_and_08us_gi;
	int  he_phy_triggered_su_beamforming_feedback;
	int  he_phy_triggered_mu_beamforming_partial_bw_feedback;
	int  he_phy_triggered_cqi_feedback;
	int  he_phy_partial_bandwidth_extended_range;
	int  he_phy_su_ppdu_with_1x_he_ltf_and_08_us_gi;
	int  he_phy_ndp_with_4x_he_ltf_and_32_us_gi;
	int  he_phy_stbc_tx_less_than_or_equal_80mhz;
	int  he_phy_stbc_rx_less_than_or_equal_80mhz;
	int  he_phy_doppler_rx;
	int  he_phy_doppler_tx;
	int  he_phy_full_bandwidth_ul_mu_mimo;
	int  he_phy_partial_bandwidth_ul_mu_mimo;
	int  he_phy_dcm_max_constellation_tx;
	int  he_phy_dcm_max_constellation_rx;
	int  he_phy_dcm_max_nss_tx;
	int  he_phy_dcm_max_nss_rx;
	int  he_phy_beamformee_sts_for_less_than_or_equal_80mhz;
	int  he_phy_beamformee_sts_for_greater_than_80mhz;
	int  he_phy_number_of_sounding_dimensions_for_less_than_or_equal_80mhz;
	int  he_phy_number_of_sounding_dimensions_for_greater_than_80mhz;
	int  he_phy_max_nc;
	int  he_phy_nominal_packet_padding;
	int  he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz;
	int  he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz;
	int  he_mcs_nss_rx_he_mcs_map_160_mhz;
	int  he_mcs_nss_tx_he_mcs_map_160_mhz;
	int  he_mcs_nss_rx_he_mcs_map_8080_mhz;
	int  he_mcs_nss_tx_he_mcs_map_8080_mhz;
	int  he_ppe_thresholds_nsts;
	int  he_ppe_thresholds_ru_index_bitmask;
	int  he_ppe_thresholds_ppet16_for_nsts1_for_ru0;
	int  he_ppe_thresholds_ppet16_for_nsts1_for_ru1;
	int  he_ppe_thresholds_ppet16_for_nsts1_for_ru2;
	int  he_ppe_thresholds_ppet16_for_nsts1_for_ru3;
	int  he_ppe_thresholds_ppet16_for_nsts2_for_ru0;
	int  he_ppe_thresholds_ppet16_for_nsts2_for_ru1;
	int  he_ppe_thresholds_ppet16_for_nsts2_for_ru2;
	int  he_ppe_thresholds_ppet16_for_nsts2_for_ru3;
	int  he_ppe_thresholds_ppet16_for_nsts3_for_ru0;
	int  he_ppe_thresholds_ppet16_for_nsts3_for_ru1;
	int  he_ppe_thresholds_ppet16_for_nsts3_for_ru2;
	int  he_ppe_thresholds_ppet16_for_nsts3_for_ru3;
	int  he_ppe_thresholds_ppet16_for_nsts4_for_ru0;
	int  he_ppe_thresholds_ppet16_for_nsts4_for_ru1;
	int  he_ppe_thresholds_ppet16_for_nsts4_for_ru2;
	int  he_ppe_thresholds_ppet16_for_nsts4_for_ru3;
	int  he_ppe_thresholds_ppet8_for_nsts1_for_ru0;
	int  he_ppe_thresholds_ppet8_for_nsts1_for_ru1;
	int  he_ppe_thresholds_ppet8_for_nsts1_for_ru2;
	int  he_ppe_thresholds_ppet8_for_nsts1_for_ru3;
	int  he_ppe_thresholds_ppet8_for_nsts2_for_ru0;
	int  he_ppe_thresholds_ppet8_for_nsts2_for_ru1;
	int  he_ppe_thresholds_ppet8_for_nsts2_for_ru2;
	int  he_ppe_thresholds_ppet8_for_nsts2_for_ru3;
	int  he_ppe_thresholds_ppet8_for_nsts3_for_ru0;
	int  he_ppe_thresholds_ppet8_for_nsts3_for_ru1;
	int  he_ppe_thresholds_ppet8_for_nsts3_for_ru2;
	int  he_ppe_thresholds_ppet8_for_nsts3_for_ru3;
	int  he_ppe_thresholds_ppet8_for_nsts4_for_ru0;
	int  he_ppe_thresholds_ppet8_for_nsts4_for_ru1;
	int  he_ppe_thresholds_ppet8_for_nsts4_for_ru2;
	int  he_ppe_thresholds_ppet8_for_nsts4_for_ru3;
};

static inline int
hostapd_conf_get_he_mac_capab_info(u8 *he_mac_capab_info, const char *buf,
				   char *pos,
				   struct he_override_hw_capab *cap_idx)
{
	if (os_strcmp(buf, "he_mac_fragmentation") == 0) {
		cap_idx->he_mac_fragmentation = 1;
		he_mac_capab_info[HE_MACCAP_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP0_FRAGMENTATION_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_plus_htc_he_support") == 0) {
		cap_idx->he_mac_plus_htc_he_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP0_HTC_HE_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_twt_requester_support") == 0) {
		cap_idx->he_mac_twt_requester_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP0_TWT_REQUESTER_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_twt_responder_support") == 0) {
		cap_idx->he_mac_twt_responder_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP0_TWT_RESPONDER_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_all_ack_support") == 0) {
		cap_idx->he_mac_all_ack_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP2_ALL_ACK_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_bsr_support") == 0) {
		cap_idx->he_mac_bsr_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP2_BSR_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_broadcast_twt_support") == 0) {
		cap_idx->he_mac_broadcast_twt_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP2_BROADCAST_TWT_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_32bit_ba_bitmap_support") == 0) {
		cap_idx->he_mac_32bit_ba_bitmap_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP2_32BIT_BA_BITMAP_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_mu_cascading_support") == 0) {
		cap_idx->he_mac_mu_cascading_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP2_MU_CASCADING_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_ack_enabled_aggregation_support") == 0) {
		cap_idx->he_mac_ack_enabled_aggregation_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP2_ACK_ENABLED_AGGREGATION_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_group_addressed_multi_sta_blockack_in_dl_mu_support") == 0) {
		cap_idx->he_mac_group_addressed_multi_sta_blockack_in_dl_mu_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP3_GROUP_ADD_MULTI_STA_BA_IN_DL_MU_SUP);
		return 1;
	} else if (os_strcmp(buf, "he_mac_om_control_support") == 0) {
		cap_idx->he_mac_om_control_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP3_OM_CONTROL_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_ofdma_ra_support") == 0) {
		cap_idx->he_mac_ofdma_ra_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP3_OFDMA_RA_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_a_msdu_fragmentation_support") == 0) {
		cap_idx->he_mac_a_msdu_fragmentation_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP3_AMSDU_FRGMENTATION_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_flexible_twt_schedule_support") == 0) {
		cap_idx->he_mac_flexible_twt_schedule_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP3_FLEXIBLE_TWT_SCHEDULE_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_rx_control_frame_to_multibss") == 0) {
		cap_idx->he_mac_rx_control_frame_to_multibss = 1;
		he_mac_capab_info[HE_MACCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP3_RX_CONTROL_FRAME_TO_MULTIBSS);
		return 1;
	} else if (os_strcmp(buf, "he_mac_bsrp_bqrp_a_mpdu_aggregation") == 0) {
		cap_idx->he_mac_bsrp_bqrp_a_mpdu_aggregation = 1;
		he_mac_capab_info[HE_MACCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP4_BSRP_BQRP_AMPDU_AGGREGATION);
		return 1;
	} else if (os_strcmp(buf, "he_mac_qtp_support") == 0) {
		cap_idx->he_mac_qtp_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP4_QTP_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_bqr_support") == 0) {
		cap_idx->he_mac_bqr_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP4_BQR_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_a_msdu_in_ack_enabled_a_mpdu_support") == 0) {
		cap_idx->he_mac_a_msdu_in_ack_enabled_a_mpdu_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP4_AMSDU_IN_AMPDU_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_maximum_number_of_fragmented_msdus_amsdus") == 0) {
		cap_idx->he_mac_maximum_number_of_fragmented_msdus_amsdus = 1;
		he_mac_capab_info[HE_MACCAP_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP0_MAX_NUM_OF_FRAG_MSDU);
		return 1;
	} else if (os_strcmp(buf, "he_mac_minimum_fragment_size") == 0) {
		cap_idx->he_mac_minimum_fragment_size = 1;
		he_mac_capab_info[HE_MACCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP1_MINIMUM_FRAGMENT_SIZE);
		return 1;
	} else if (os_strcmp(buf, "he_mac_trigger_frame_mac_padding_duration") == 0) {
		cap_idx->he_mac_trigger_frame_mac_padding_duration = 1;
		he_mac_capab_info[HE_MACCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP1_TRIGGER_FRAME_MAC_PAD_DUR);
		return 1;
	} else if (os_strcmp(buf, "he_mac_multi_tid_aggregation_rx_support") == 0) {
		cap_idx->he_mac_multi_tid_aggregation_rx_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP1_MULTI_TID_AGGR_RX_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_he_link_adaptation") == 0) {
		cap_idx->he_mac_he_link_adaptation = 1;
		he_mac_capab_info[HE_MACCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP1_HE_LINK_ADAPTION_SUPPORT);
		he_mac_capab_info[HE_MACCAP_CAP2_IDX] |= ltq_set_he_cap((atoi(pos) >> 1),
						HE_MAC_CAP2_HE_LINK_ADAPTION_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_maximum_a_mpdu_length_exponent") == 0) {
		cap_idx->he_mac_maximum_a_mpdu_length_exponent = 1;
		he_mac_capab_info[HE_MACCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_multi_tid_aggregation_tx_support") == 0) {
		cap_idx->he_mac_multi_tid_aggregation_tx_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP4_MULTI_TID_AGGR_TX_SUPPORT);
		he_mac_capab_info[HE_MACCAP_CAP5_IDX] |= ltq_set_he_cap((atoi(pos) >> 1),
						HE_MAC_CAP5_MULTI_TID_AGGR_TX_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_ndp_feedback_report_support") == 0) {
		cap_idx->he_mac_ndp_feedback_report_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP4_NDP_FEEDBACK_REPORT_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_mac_om_control_ul_mu_data_disable_rx_support") == 0) {
		cap_idx->he_mac_om_control_ul_mu_data_disable_rx_support = 1;
		he_mac_capab_info[HE_MACCAP_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_MAC_CAP5_OM_CONTROL_UL_MU_DATA_DIS_RX_SUP);
		return 1;
	}

	return 0;
}

static inline int
hostapd_conf_get_he_phy_capab_info(u8 *he_phy_capab_info, const char *buf,
				   char *pos,
				   struct he_override_hw_capab *cap_idx)
{
	if (os_strcmp(buf, "he_phy_preamble_puncturing_rx") == 0) {
		cap_idx->he_phy_preamble_puncturing_rx = 1;
		he_phy_capab_info[HE_PHYCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP1_PUN_PREAM_RX);
		return 1;
	} else if (os_strcmp(buf, "he_phy_device_class") == 0) {
		cap_idx->he_phy_device_class = 1;
		he_phy_capab_info[HE_PHYCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP1_DEVICE_CLASS);
		return 1;
	} else if (os_strcmp(buf, "he_phy_ldpc_coding_in_payload") == 0) {
		cap_idx->he_phy_ldpc_coding_in_payload = 1;
		he_phy_capab_info[HE_PHYCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD);
		return 1;
	} else if (os_strcmp(buf, "he_phy_su_beamformer_capable") == 0) {
		cap_idx->he_phy_su_beamformer_capable = 1;
		he_phy_capab_info[HE_PHYCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP3_SU_BEAMFORMER);
		return 1;
	} else if (os_strcmp(buf, "he_phy_su_beamformee_capable") == 0) {
		cap_idx->he_phy_su_beamformee_capable = 1;
		he_phy_capab_info[HE_PHYCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP4_SU_BEAMFORMEE);
		return 1;
	} else if (os_strcmp(buf, "he_phy_mu_beamformer_capable") == 0) {
		cap_idx->he_phy_mu_beamformer_capable = 1;
		he_phy_capab_info[HE_PHYCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP4_MU_BEAMFORMER);
		return 1;
	} else if (os_strcmp(buf, "he_phy_ng_16_su_feedback") == 0) {
		cap_idx->he_phy_ng_16_su_feedback = 1;
		he_phy_capab_info[HE_PHYCAP_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP5_NG_16_FOR_SU_FB_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_phy_ng_16_mu_feedback") == 0) {
		cap_idx->he_phy_ng_16_mu_feedback = 1;
		he_phy_capab_info[HE_PHYCAP_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP5_NG_16_FOR_MU_FB_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_phy_codebook_size42_for_su_support") == 0) {
		cap_idx->he_phy_codebook_size42_for_su_support = 1;
		he_phy_capab_info[HE_PHYCAP_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP6_CODEBOOK_SIZE42_FOR_SU_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_phy_codebook_size75_for_mu_support") == 0) {
		cap_idx->he_phy_codebook_size75_for_mu_support = 1;
		he_phy_capab_info[HE_PHYCAP_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP6_CODEBOOK_SIZE75_FOR_MU_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_phy_ppe_thresholds_present") == 0) {
		cap_idx->he_phy_ppe_thresholds_present = 1;
		he_phy_capab_info[HE_PHYCAP_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP6_PPE_THRESHOLD_PRESENT);
		return 1;
	} else if (os_strcmp(buf, "he_phy_srp_based_sr_support") == 0) {
		cap_idx->he_phy_srp_based_sr_support = 1;
		he_phy_capab_info[HE_PHYCAP_CAP7_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP7_SRP_BASED_SR_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_phy_power_boost_factor_alpha_support") == 0) {
		cap_idx->he_phy_power_boost_factor_alpha_support = 1;
		he_phy_capab_info[HE_PHYCAP_CAP7_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP7_POWER_BOOST_FACTOR_SUPPORT);
		return 1;
	} else if (os_strcmp(buf, "he_phy_su_ppdu_and_he_mu_with_4x_he_ltf_and_08us_gi") == 0) {
		cap_idx->he_phy_su_ppdu_and_he_mu_with_4x_he_ltf_and_08us_gi = 1;
		he_phy_capab_info[HE_PHYCAP_CAP7_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP7_SU_PPDU_AND_HE_MU_WITH_4X_HE_LTF_0_8US_GI);
		return 1;
	} else if (os_strcmp(buf, "he_phy_triggered_su_beamforming_feedback") == 0) {
		cap_idx->he_phy_triggered_su_beamforming_feedback = 1;
		he_phy_capab_info[HE_PHYCAP_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP6_TRIGGERED_SU_BEAMFORMING_FEEDBACK);
		return 1;
	} else if (os_strcmp(buf, "he_phy_triggered_mu_beamforming_partial_bw_feedback") == 0) {
		cap_idx->he_phy_triggered_mu_beamforming_partial_bw_feedback = 1;
		he_phy_capab_info[HE_PHYCAP_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP6_TRIGGERED_MU_BEAMFORMING_PARTIAL_BW_FEEDBACK);
		return 1;
	} else if (os_strcmp(buf, "he_phy_triggered_cqi_feedback") == 0) {
		cap_idx->he_phy_triggered_cqi_feedback = 1;
		he_phy_capab_info[HE_PHYCAP_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP6_TRIGGERED_CQI_FEEDBACK);
		return 1;
	} else if (os_strcmp(buf, "he_phy_partial_bandwidth_extended_range") == 0) {
		cap_idx->he_phy_partial_bandwidth_extended_range = 1;
		he_phy_capab_info[HE_PHYCAP_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP6_PARTIAL_BANDWIDTH_EXTENDED_RANGE);
		return 1;
	} else if (os_strcmp(buf, "he_phy_su_ppdu_with_1x_he_ltf_and_08_us_gi") == 0) {
		cap_idx->he_phy_su_ppdu_with_1x_he_ltf_and_08_us_gi = 1;
		he_phy_capab_info[HE_PHYCAP_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP1_SU_PPDU_1XHE_LTF_0_8US_GI);
		return 1;
	} else if (os_strcmp(buf, "he_phy_ndp_with_4x_he_ltf_and_32_us_gi") == 0) {
		cap_idx->he_phy_ndp_with_4x_he_ltf_and_32_us_gi = 1;
		he_phy_capab_info[HE_PHYCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP2_NDP_4X_HE_LTF_AND_3_2MS_GI);
		return 1;
	} else if (os_strcmp(buf, "he_phy_stbc_tx_less_than_or_equal_80mhz") == 0) {
		cap_idx->he_phy_stbc_tx_less_than_or_equal_80mhz = 1;
		he_phy_capab_info[HE_PHYCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP2_STBC_TX_LESS_OR_EQUAL_80MHz);
		return 1;
	} else if (os_strcmp(buf, "he_phy_stbc_rx_less_than_or_equal_80mhz") == 0) {
		cap_idx->he_phy_stbc_rx_less_than_or_equal_80mhz = 1;
		he_phy_capab_info[HE_PHYCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP2_STBC_RX_LESS_OR_EQUAL_80MHz);
		return 1;
	} else if (os_strcmp(buf, "he_phy_doppler_rx") == 0) {
		cap_idx->he_phy_doppler_rx = 1;
		he_phy_capab_info[HE_PHYCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP2_DOPPLER_RX);
		return 1;
	} else if (os_strcmp(buf, "he_phy_doppler_tx") == 0) {
		cap_idx->he_phy_doppler_tx = 1;
		he_phy_capab_info[HE_PHYCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP2_DOPPLER_TX);
		return 1;
	} else if (os_strcmp(buf, "he_phy_full_bandwidth_ul_mu_mimo") == 0) {
		cap_idx->he_phy_full_bandwidth_ul_mu_mimo = 1;
		he_phy_capab_info[HE_PHYCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP2_FULL_BANDWIDTH_UL_MU_MIMO);
		return 1;
	} else if (os_strcmp(buf, "he_phy_partial_bandwidth_ul_mu_mimo") == 0) {
		cap_idx->he_phy_partial_bandwidth_ul_mu_mimo = 1;
		he_phy_capab_info[HE_PHYCAP_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP2_PARTIAL_BANDWIDTH_UL_MU_MIMO);
		return 1;
	} else if (os_strcmp(buf, "he_phy_dcm_max_constellation_tx") == 0) {
		cap_idx->he_phy_dcm_max_constellation_tx = 1;
		he_phy_capab_info[HE_PHYCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP3_DCM_MAX_CONSTELLATION_TX);
		return 1;
	} else if (os_strcmp(buf, "he_phy_dcm_max_constellation_rx") == 0) {
		cap_idx->he_phy_dcm_max_constellation_rx = 1;
		he_phy_capab_info[HE_PHYCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP3_DCM_MAX_CONSTELLATION_RX);
		return 1;
	} else if (os_strcmp(buf, "he_phy_dcm_max_nss_tx") == 0) {
		cap_idx->he_phy_dcm_max_nss_tx = 1;
		he_phy_capab_info[HE_PHYCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP3_DCM_MAX_NSS_TX);
		return 1;
	} else if (os_strcmp(buf, "he_phy_dcm_max_nss_rx") == 0) {
		cap_idx->he_phy_dcm_max_nss_rx = 1;
		he_phy_capab_info[HE_PHYCAP_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP3_DCM_MAX_NSS_RX);
		return 1;
	} else if (os_strcmp(buf, "he_phy_beamformee_sts_for_less_than_or_equal_80mhz") == 0) {
		cap_idx->he_phy_beamformee_sts_for_less_than_or_equal_80mhz = 1;
		he_phy_capab_info[HE_PHYCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP4_BF_STS_LESS_OR_EQ_80MHz);
		return 1;
	} else if (os_strcmp(buf, "he_phy_beamformee_sts_for_greater_than_80mhz") == 0) {
		cap_idx->he_phy_beamformee_sts_for_greater_than_80mhz = 1;
		he_phy_capab_info[HE_PHYCAP_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP4_BF_STS_GREATER_THAN_80MHz);
		return 1;
	} else if (os_strcmp(buf, "he_phy_number_of_sounding_dimensions_for_less_than_or_equal_80mhz") == 0) {
		cap_idx->he_phy_number_of_sounding_dimensions_for_less_than_or_equal_80mhz = 1;
		he_phy_capab_info[HE_PHYCAP_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP5_NUM_SOUND_DIM_LESS_80MHz);
		return 1;
	} else if (os_strcmp(buf, "he_phy_number_of_sounding_dimensions_for_greater_than_80mhz") == 0) {
		cap_idx->he_phy_number_of_sounding_dimensions_for_greater_than_80mhz = 1;
		he_phy_capab_info[HE_PHYCAP_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP5_NUM_SOUND_DIM_GREAT_80MHz);
		return 1;
	} else if (os_strcmp(buf, "he_phy_max_nc") == 0) {
		cap_idx->he_phy_max_nc = 1;
		he_phy_capab_info[HE_PHYCAP_CAP7_IDX] |= ltq_set_he_cap(atoi(pos), HE_PHY_CAP7_MAX_NC);
		return 1;
	} else if (os_strcmp(buf, "he_phy_nominal_packet_padding") == 0) {
		cap_idx->he_phy_nominal_packet_padding = 1;
		he_phy_capab_info[HE_PHYCAP_CAP9_IDX] |= ltq_set_he_cap(atoi(pos),
					HE_PHY_CAP9_NOMINAL_PACKET_PADDING);
		return 1;
	}
	return 0;
}

static inline int
hostapd_conf_get_he_txrx_mcs_support(u8 *he_txrx_mcs_support, const char *buf,
				     char *pos,
				     struct he_override_hw_capab *cap_idx)
{
	if (os_strcmp(buf, "he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz") == 0) {
		cap_idx->he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz = 1;
		he_txrx_mcs_support[0] = atoi(pos) & 0xff;
		he_txrx_mcs_support[1] = (atoi(pos) >> 8) & 0xff;
		return 1;
	} else if (os_strcmp(buf, "he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz") == 0) {
		cap_idx->he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz = 1;
		he_txrx_mcs_support[2] = atoi(pos) & 0xff;
		he_txrx_mcs_support[3] = (atoi(pos) >> 8) & 0xff;
		return 1;
	} else if (os_strcmp(buf, "he_mcs_nss_rx_he_mcs_map_160_mhz") == 0) {
		cap_idx->he_mcs_nss_rx_he_mcs_map_160_mhz = 1;
		he_txrx_mcs_support[4] = atoi(pos) & 0xff;
		he_txrx_mcs_support[5] = (atoi(pos) >> 8) & 0xff;
		return 1;
	} else if (os_strcmp(buf, "he_mcs_nss_tx_he_mcs_map_160_mhz") == 0) {
		cap_idx->he_mcs_nss_tx_he_mcs_map_160_mhz = 1;
		he_txrx_mcs_support[6] = atoi(pos) & 0xff;
		he_txrx_mcs_support[7] = (atoi(pos) >> 8) & 0xff;
		return 1;
	} else if (os_strcmp(buf, "he_mcs_nss_rx_he_mcs_map_8080_mhz") == 0) {
		cap_idx->he_mcs_nss_rx_he_mcs_map_8080_mhz = 1;
		he_txrx_mcs_support[8] = atoi(pos) & 0xff;
		he_txrx_mcs_support[9] = (atoi(pos) >> 8) & 0xff;
		return 1;
	} else if (os_strcmp(buf, "he_mcs_nss_tx_he_mcs_map_8080_mhz") == 0) {
		cap_idx->he_mcs_nss_tx_he_mcs_map_8080_mhz = 1;
		he_txrx_mcs_support[10] = atoi(pos) & 0xff;
		he_txrx_mcs_support[11] = (atoi(pos) >> 8) & 0xff;
		return 1;
	}

	return 0;
}

static inline int
hostapd_conf_get_he_ppe_thresholds(u8 *he_ppe_thresholds, const char *buf,
				   char *pos,
				   struct he_override_hw_capab *cap_idx)
{
	if (os_strcmp(buf, "he_ppe_thresholds_nsts") == 0) {
		cap_idx->he_ppe_thresholds_nsts = 1;
		he_ppe_thresholds[HE_PPE_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP0_NSS_M1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ru_index_bitmask") == 0) {
		cap_idx->he_ppe_thresholds_ru_index_bitmask = 1;
		he_ppe_thresholds[HE_PPE_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP0_RU_INDEX_BITMASK);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts1_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP0_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP0_PPET16_FOR_NSS1_FOR_RU0);
		he_ppe_thresholds[HE_PPE_CAP1_IDX] |= ltq_set_he_cap(atoi(pos) >> 1,
						HE_PPE_CAP1_PPET16_FOR_NSS1_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts1_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP1_PPET16_FOR_NSS1_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts1_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP2_PPET16_FOR_NSS1_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts1_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP3_PPET16_FOR_NSS1_FOR_RU3);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts2_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP3_PPET16_FOR_NSS2_FOR_RU0);
		he_ppe_thresholds[HE_PPE_CAP4_IDX] |= ltq_set_he_cap(atoi(pos) >> 1,
						HE_PPE_CAP4_PPET16_FOR_NSS2_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts2_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP4_PPET16_FOR_NSS2_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts2_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP5_PPET16_FOR_NSS2_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts2_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP6_PPET16_FOR_NSS2_FOR_RU3);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts3_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP6_PPET16_FOR_NSS3_FOR_RU0);
		he_ppe_thresholds[HE_PPE_CAP7_IDX] |= ltq_set_he_cap((atoi(pos) >> 1),
						HE_PPE_CAP7_PPET16_FOR_NSS3_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts3_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP7_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP7_PPET16_FOR_NSS3_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts3_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP8_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP8_PPET16_FOR_NSS3_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts3_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP9_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP9_PPET16_FOR_NSS3_FOR_RU3);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts4_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP9_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP9_PPET16_FOR_NSS4_FOR_RU0);
		he_ppe_thresholds[HE_PPE_CAP10_IDX] |= ltq_set_he_cap(atoi(pos) >> 1,
						HE_PPE_CAP10_PPET16_FOR_NSS4_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts4_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP10_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP10_PPET16_FOR_NSS4_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts4_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP11_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP11_PPET16_FOR_NSS4_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet16_for_nsts4_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP12_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP12_PPET16_FOR_NSS4_FOR_RU3);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts1_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP1_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP1_PPET8_FOR_NSS1_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts1_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP2_PPET8_FOR_NSS1_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts1_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP2_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP2_PPET8_FOR_NSS1_FOR_RU2 );
		he_ppe_thresholds[HE_PPE_CAP3_IDX] |= ltq_set_he_cap((atoi(pos) >> 2),
						HE_PPE_CAP3_PPET8_FOR_NSS1_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts1_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP3_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP3_PPET8_FOR_NSS1_FOR_RU3);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts2_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP4_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP4_PPET8_FOR_NSS2_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts2_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP5_PPET8_FOR_NSS2_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts2_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP5_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP5_PPET8_FOR_NSS2_FOR_RU2);
		he_ppe_thresholds[HE_PPE_CAP6_IDX] |= ltq_set_he_cap(atoi(pos) >> 2,
						HE_PPE_CAP6_PPET8_FOR_NSS2_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts2_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP6_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP6_PPET8_FOR_NSS2_FOR_RU3);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts3_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP7_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP7_PPET8_FOR_NSS3_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts3_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP8_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP8_PPET8_FOR_NSS3_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts3_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP8_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP8_PPET8_FOR_NSS3_FOR_RU2);
		he_ppe_thresholds[HE_PPE_CAP9_IDX] |= ltq_set_he_cap(atoi(pos) >> 2,
						HE_PPE_CAP9_PPET8_FOR_NSS3_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts3_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP9_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP9_PPET8_FOR_NSS3_FOR_RU3);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts4_for_ru0") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru0 = 1;
		he_ppe_thresholds[HE_PPE_CAP10_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP10_PPET8_FOR_NSS4_FOR_RU0);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts4_for_ru1") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru1 = 1;
		he_ppe_thresholds[HE_PPE_CAP11_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP11_PPET8_FOR_NSS4_FOR_RU1);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts4_for_ru2") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru2 = 1;
		he_ppe_thresholds[HE_PPE_CAP11_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP11_PPET8_FOR_NSS4_FOR_RU2);
		he_ppe_thresholds[HE_PPE_CAP12_IDX] |= ltq_set_he_cap((atoi(pos) >> 2),
						HE_PPE_CAP12_PPET8_FOR_NSS4_FOR_RU2);
		return 1;
	} else if (os_strcmp(buf, "he_ppe_thresholds_ppet8_for_nsts4_for_ru3") == 0) {
		cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru3 = 1;
		he_ppe_thresholds[HE_PPE_CAP12_IDX] |= ltq_set_he_cap(atoi(pos),
						HE_PPE_CAP12_PPET8_FOR_NSS4_FOR_RU3);
		return 1;
	}


	return 0;
}

static inline void
hostapd_set_debug_he_mac_capab_info(u8 *debug_he_mac, u8 *conf_he_mac,
			struct he_override_hw_capab *cap_idx)
{
	if (cap_idx->he_mac_fragmentation)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP0_IDX],
			conf_he_mac[HE_MACCAP_CAP0_IDX], HE_MAC_CAP0_FRAGMENTATION_SUPPORT);

	if (cap_idx->he_mac_plus_htc_he_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP0_IDX],
			conf_he_mac[HE_MACCAP_CAP0_IDX], HE_MAC_CAP0_HTC_HE_SUPPORT);

	if (cap_idx->he_mac_twt_requester_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP0_IDX],
			conf_he_mac[HE_MACCAP_CAP0_IDX], HE_MAC_CAP0_TWT_REQUESTER_SUPPORT);

	if (cap_idx->he_mac_twt_responder_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP0_IDX],
			conf_he_mac[HE_MACCAP_CAP0_IDX], HE_MAC_CAP0_TWT_RESPONDER_SUPPORT);

	if (cap_idx->he_mac_all_ack_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP2_IDX],
			conf_he_mac[HE_MACCAP_CAP2_IDX], HE_MAC_CAP2_ALL_ACK_SUPPORT);

	if (cap_idx->he_mac_bsr_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP2_IDX],
			conf_he_mac[HE_MACCAP_CAP2_IDX], HE_MAC_CAP2_BSR_SUPPORT);

	if (cap_idx->he_mac_broadcast_twt_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP2_IDX],
			conf_he_mac[HE_MACCAP_CAP2_IDX], HE_MAC_CAP2_BROADCAST_TWT_SUPPORT);

	if (cap_idx->he_mac_32bit_ba_bitmap_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP2_IDX],
			conf_he_mac[HE_MACCAP_CAP2_IDX], HE_MAC_CAP2_32BIT_BA_BITMAP_SUPPORT);

	if (cap_idx->he_mac_mu_cascading_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP2_IDX],
			conf_he_mac[HE_MACCAP_CAP2_IDX], HE_MAC_CAP2_MU_CASCADING_SUPPORT);

	if (cap_idx->he_mac_ack_enabled_aggregation_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP2_IDX],
			conf_he_mac[HE_MACCAP_CAP2_IDX], HE_MAC_CAP2_ACK_ENABLED_AGGREGATION_SUPPORT);

	if (cap_idx->he_mac_group_addressed_multi_sta_blockack_in_dl_mu_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP3_IDX],
			conf_he_mac[HE_MACCAP_CAP3_IDX], HE_MAC_CAP3_GROUP_ADD_MULTI_STA_BA_IN_DL_MU_SUP);

	if (cap_idx->he_mac_om_control_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP3_IDX],
			conf_he_mac[HE_MACCAP_CAP3_IDX], HE_MAC_CAP3_OM_CONTROL_SUPPORT);

	if (cap_idx->he_mac_ofdma_ra_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP3_IDX],
			conf_he_mac[HE_MACCAP_CAP3_IDX], HE_MAC_CAP3_OFDMA_RA_SUPPORT);

	if (cap_idx->he_mac_a_msdu_fragmentation_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP3_IDX],
			conf_he_mac[HE_MACCAP_CAP3_IDX], HE_MAC_CAP3_AMSDU_FRGMENTATION_SUPPORT);

	if (cap_idx->he_mac_flexible_twt_schedule_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP3_IDX],
			conf_he_mac[HE_MACCAP_CAP3_IDX], HE_MAC_CAP3_FLEXIBLE_TWT_SCHEDULE_SUPPORT);

	if (cap_idx->he_mac_rx_control_frame_to_multibss)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP3_IDX],
			conf_he_mac[HE_MACCAP_CAP3_IDX], HE_MAC_CAP3_RX_CONTROL_FRAME_TO_MULTIBSS);

	if (cap_idx->he_mac_bsrp_bqrp_a_mpdu_aggregation)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP4_IDX],
			conf_he_mac[HE_MACCAP_CAP4_IDX], HE_MAC_CAP4_BSRP_BQRP_AMPDU_AGGREGATION);

	if (cap_idx->he_mac_qtp_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP4_IDX],
			conf_he_mac[HE_MACCAP_CAP4_IDX], HE_MAC_CAP4_QTP_SUPPORT);

	if (cap_idx->he_mac_bqr_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP4_IDX],
			conf_he_mac[HE_MACCAP_CAP4_IDX], HE_MAC_CAP4_BQR_SUPPORT);

	if (cap_idx->he_mac_a_msdu_in_ack_enabled_a_mpdu_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP4_IDX],
			conf_he_mac[HE_MACCAP_CAP4_IDX], HE_MAC_CAP4_AMSDU_IN_AMPDU_SUPPORT);

	if (cap_idx->he_mac_maximum_number_of_fragmented_msdus_amsdus)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP0_IDX],
			conf_he_mac[HE_MACCAP_CAP0_IDX], HE_MAC_CAP0_MAX_NUM_OF_FRAG_MSDU);

	if (cap_idx->he_mac_minimum_fragment_size)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP1_IDX],
			conf_he_mac[HE_MACCAP_CAP1_IDX], HE_MAC_CAP1_MINIMUM_FRAGMENT_SIZE);

	if (cap_idx->he_mac_trigger_frame_mac_padding_duration)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP1_IDX],
			conf_he_mac[HE_MACCAP_CAP1_IDX], HE_MAC_CAP1_TRIGGER_FRAME_MAC_PAD_DUR);

	if (cap_idx->he_mac_multi_tid_aggregation_rx_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP1_IDX],
			conf_he_mac[HE_MACCAP_CAP1_IDX], HE_MAC_CAP1_MULTI_TID_AGGR_RX_SUPPORT);

	if (cap_idx->he_mac_he_link_adaptation) {
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP1_IDX],
			conf_he_mac[HE_MACCAP_CAP1_IDX], HE_MAC_CAP1_HE_LINK_ADAPTION_SUPPORT);
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP2_IDX],
			conf_he_mac[HE_MACCAP_CAP2_IDX], HE_MAC_CAP2_HE_LINK_ADAPTION_SUPPORT);
	}

	if (cap_idx->he_mac_maximum_a_mpdu_length_exponent)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP3_IDX],
			conf_he_mac[HE_MACCAP_CAP3_IDX], HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_EXT);

	if (cap_idx->he_mac_multi_tid_aggregation_tx_support) {
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP4_IDX],
			conf_he_mac[HE_MACCAP_CAP4_IDX], HE_MAC_CAP4_MULTI_TID_AGGR_TX_SUPPORT);
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP5_IDX],
			conf_he_mac[HE_MACCAP_CAP5_IDX], HE_MAC_CAP5_MULTI_TID_AGGR_TX_SUPPORT);
	}

	if (cap_idx->he_mac_ndp_feedback_report_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP4_IDX],
			conf_he_mac[HE_MACCAP_CAP4_IDX], HE_MAC_CAP4_NDP_FEEDBACK_REPORT_SUPPORT);

	if (cap_idx->he_mac_om_control_ul_mu_data_disable_rx_support)
		ltq_clr_set_he_cap(&debug_he_mac[HE_MACCAP_CAP5_IDX],
			conf_he_mac[HE_MACCAP_CAP5_IDX], HE_MAC_CAP5_OM_CONTROL_UL_MU_DATA_DIS_RX_SUP);
}

static inline void
hostapd_set_debug_he_phy_capab_info(u8 *debug_he_phy, u8 *conf_he_phy,
			struct he_override_hw_capab *cap_idx)
{
	if (cap_idx->he_phy_preamble_puncturing_rx)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP1_IDX],
			conf_he_phy[HE_PHYCAP_CAP1_IDX], HE_PHY_CAP1_PUN_PREAM_RX);

	if (cap_idx->he_phy_device_class)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP1_IDX],
			conf_he_phy[HE_PHYCAP_CAP1_IDX], HE_PHY_CAP1_DEVICE_CLASS);

	if (cap_idx->he_phy_ldpc_coding_in_payload)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP1_IDX],
			conf_he_phy[HE_PHYCAP_CAP1_IDX], HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD);

	if (cap_idx->he_phy_su_beamformer_capable)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP3_IDX],
			conf_he_phy[HE_PHYCAP_CAP3_IDX], HE_PHY_CAP3_SU_BEAMFORMER);

	if (cap_idx->he_phy_su_beamformee_capable)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP4_IDX],
			conf_he_phy[HE_PHYCAP_CAP4_IDX], HE_PHY_CAP4_SU_BEAMFORMEE);

	if (cap_idx->he_phy_mu_beamformer_capable)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP4_IDX],
			conf_he_phy[HE_PHYCAP_CAP4_IDX], HE_PHY_CAP4_MU_BEAMFORMER);

	if (cap_idx->he_phy_ng_16_su_feedback)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP5_IDX],
			conf_he_phy[HE_PHYCAP_CAP5_IDX], HE_PHY_CAP5_NG_16_FOR_SU_FB_SUPPORT);

	if (cap_idx->he_phy_ng_16_mu_feedback)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP5_IDX],
			conf_he_phy[HE_PHYCAP_CAP5_IDX], HE_PHY_CAP5_NG_16_FOR_MU_FB_SUPPORT);

	if (cap_idx->he_phy_codebook_size42_for_su_support)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP6_IDX],
			conf_he_phy[HE_PHYCAP_CAP6_IDX], HE_PHY_CAP6_CODEBOOK_SIZE42_FOR_SU_SUPPORT);

	if (cap_idx->he_phy_codebook_size75_for_mu_support)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP6_IDX],
			conf_he_phy[HE_PHYCAP_CAP6_IDX], HE_PHY_CAP6_CODEBOOK_SIZE75_FOR_MU_SUPPORT);

	if (cap_idx->he_phy_ppe_thresholds_present)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP6_IDX],
			conf_he_phy[HE_PHYCAP_CAP6_IDX], HE_PHY_CAP6_PPE_THRESHOLD_PRESENT);

	if (cap_idx->he_phy_srp_based_sr_support)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP7_IDX],
			conf_he_phy[HE_PHYCAP_CAP7_IDX], HE_PHY_CAP7_SRP_BASED_SR_SUPPORT);

	if (cap_idx->he_phy_power_boost_factor_alpha_support)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP7_IDX],
			conf_he_phy[HE_PHYCAP_CAP7_IDX], HE_PHY_CAP7_POWER_BOOST_FACTOR_SUPPORT);

	if (cap_idx->he_phy_su_ppdu_and_he_mu_with_4x_he_ltf_and_08us_gi) {
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP7_IDX],
			conf_he_phy[HE_PHYCAP_CAP7_IDX],
			HE_PHY_CAP7_SU_PPDU_AND_HE_MU_WITH_4X_HE_LTF_0_8US_GI);
	}

	if (cap_idx->he_phy_triggered_su_beamforming_feedback) {
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP6_IDX],
			conf_he_phy[HE_PHYCAP_CAP6_IDX],
			HE_PHY_CAP6_TRIGGERED_SU_BEAMFORMING_FEEDBACK);
	}

	if (cap_idx->he_phy_triggered_mu_beamforming_partial_bw_feedback) {
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP6_IDX],
			conf_he_phy[HE_PHYCAP_CAP6_IDX],
			HE_PHY_CAP6_TRIGGERED_MU_BEAMFORMING_PARTIAL_BW_FEEDBACK);
	}

	if (cap_idx->he_phy_triggered_cqi_feedback)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP6_IDX],
			conf_he_phy[HE_PHYCAP_CAP6_IDX], HE_PHY_CAP6_TRIGGERED_CQI_FEEDBACK);

	if (cap_idx->he_phy_partial_bandwidth_extended_range)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP6_IDX],
			conf_he_phy[HE_PHYCAP_CAP6_IDX], HE_PHY_CAP6_PARTIAL_BANDWIDTH_EXTENDED_RANGE);

	if (cap_idx->he_phy_su_ppdu_with_1x_he_ltf_and_08_us_gi)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP1_IDX],
			conf_he_phy[HE_PHYCAP_CAP1_IDX], HE_PHY_CAP1_SU_PPDU_1XHE_LTF_0_8US_GI);

	if (cap_idx->he_phy_ndp_with_4x_he_ltf_and_32_us_gi)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP2_IDX],
			conf_he_phy[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_NDP_4X_HE_LTF_AND_3_2MS_GI);

	if (cap_idx->he_phy_stbc_tx_less_than_or_equal_80mhz)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP2_IDX],
			conf_he_phy[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_STBC_TX_LESS_OR_EQUAL_80MHz);

	if (cap_idx->he_phy_stbc_rx_less_than_or_equal_80mhz)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP2_IDX],
			conf_he_phy[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_STBC_RX_LESS_OR_EQUAL_80MHz);

	if (cap_idx->he_phy_doppler_rx)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP2_IDX],
			conf_he_phy[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_DOPPLER_RX);

	if (cap_idx->he_phy_doppler_tx)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP2_IDX],
			conf_he_phy[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_DOPPLER_TX);

	if (cap_idx->he_phy_full_bandwidth_ul_mu_mimo)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP2_IDX],
			conf_he_phy[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_FULL_BANDWIDTH_UL_MU_MIMO);

	if (cap_idx->he_phy_partial_bandwidth_ul_mu_mimo)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP2_IDX],
			conf_he_phy[HE_PHYCAP_CAP2_IDX], HE_PHY_CAP2_PARTIAL_BANDWIDTH_UL_MU_MIMO);

	if (cap_idx->he_phy_dcm_max_constellation_tx)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP3_IDX],
			conf_he_phy[HE_PHYCAP_CAP3_IDX], HE_PHY_CAP3_DCM_MAX_CONSTELLATION_TX);

	if (cap_idx->he_phy_dcm_max_constellation_rx)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP3_IDX],
			conf_he_phy[HE_PHYCAP_CAP3_IDX], HE_PHY_CAP3_DCM_MAX_CONSTELLATION_RX);

	if (cap_idx->he_phy_dcm_max_nss_tx)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP3_IDX],
			conf_he_phy[HE_PHYCAP_CAP3_IDX], HE_PHY_CAP3_DCM_MAX_NSS_TX);

	if (cap_idx->he_phy_dcm_max_nss_rx)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP3_IDX],
			conf_he_phy[HE_PHYCAP_CAP3_IDX], HE_PHY_CAP3_DCM_MAX_NSS_RX);

	if (cap_idx->he_phy_beamformee_sts_for_less_than_or_equal_80mhz)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP4_IDX],
			conf_he_phy[HE_PHYCAP_CAP4_IDX], HE_PHY_CAP4_BF_STS_LESS_OR_EQ_80MHz);

	if (cap_idx->he_phy_beamformee_sts_for_greater_than_80mhz)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP4_IDX],
			conf_he_phy[HE_PHYCAP_CAP4_IDX], HE_PHY_CAP4_BF_STS_GREATER_THAN_80MHz);

	if (cap_idx->he_phy_number_of_sounding_dimensions_for_less_than_or_equal_80mhz)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP5_IDX],
			conf_he_phy[HE_PHYCAP_CAP5_IDX], HE_PHY_CAP5_NUM_SOUND_DIM_LESS_80MHz);

	if (cap_idx->he_phy_number_of_sounding_dimensions_for_greater_than_80mhz)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP5_IDX],
			conf_he_phy[HE_PHYCAP_CAP5_IDX], HE_PHY_CAP5_NUM_SOUND_DIM_GREAT_80MHz);

	if (cap_idx->he_phy_max_nc)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP7_IDX],
			conf_he_phy[HE_PHYCAP_CAP7_IDX], HE_PHY_CAP7_MAX_NC);

	if (cap_idx->he_phy_nominal_packet_padding)
		ltq_clr_set_he_cap(&debug_he_phy[HE_PHYCAP_CAP9_IDX],
			conf_he_phy[HE_PHYCAP_CAP9_IDX], HE_PHY_CAP9_NOMINAL_PACKET_PADDING);
}

static inline void
hostapd_set_debug_he_txrx_mcs_support(u8 *debug_he_txrx_mcs, u8 *conf_he_txrx_mcs,
				      struct he_override_hw_capab *cap_idx)
{
	if (cap_idx->he_mcs_nss_rx_he_mcs_map_less_than_or_equal_80_mhz) {
		debug_he_txrx_mcs[0] = conf_he_txrx_mcs[0];
		debug_he_txrx_mcs[1] = conf_he_txrx_mcs[1];
	}

	if (cap_idx->he_mcs_nss_tx_he_mcs_map_less_than_or_equal_80_mhz) {
		debug_he_txrx_mcs[2] = conf_he_txrx_mcs[2];
		debug_he_txrx_mcs[3] = conf_he_txrx_mcs[3];
	}

	if (cap_idx->he_mcs_nss_rx_he_mcs_map_160_mhz) {
		debug_he_txrx_mcs[4] = conf_he_txrx_mcs[4];
		debug_he_txrx_mcs[5] = conf_he_txrx_mcs[5];
	}

	if (cap_idx->he_mcs_nss_tx_he_mcs_map_160_mhz) {
		debug_he_txrx_mcs[6] = conf_he_txrx_mcs[6];
		debug_he_txrx_mcs[7] = conf_he_txrx_mcs[7];
	}

	if (cap_idx->he_mcs_nss_rx_he_mcs_map_8080_mhz) {
		debug_he_txrx_mcs[8] = conf_he_txrx_mcs[8];
		debug_he_txrx_mcs[9] = conf_he_txrx_mcs[9];
	}

	if (cap_idx->he_mcs_nss_tx_he_mcs_map_8080_mhz) {
		debug_he_txrx_mcs[10] = conf_he_txrx_mcs[10];
		debug_he_txrx_mcs[11] = conf_he_txrx_mcs[11];
	}
}
static inline void
hostapd_set_debug_he_ppe_thresholds(u8 *debug_he_ppet, u8 *conf_he_ppet,
				    struct he_override_hw_capab *cap_idx)
{
	if (cap_idx->he_ppe_thresholds_nsts)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP0_IDX],
			conf_he_ppet[HE_PPE_CAP0_IDX], HE_PPE_CAP0_NSS_M1);

	if (cap_idx->he_ppe_thresholds_ru_index_bitmask)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP0_IDX],
			conf_he_ppet[HE_PPE_CAP0_IDX], HE_PPE_CAP0_RU_INDEX_BITMASK);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru0) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP0_IDX],
			conf_he_ppet[HE_PPE_CAP0_IDX], HE_PPE_CAP0_PPET16_FOR_NSS1_FOR_RU0);
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP1_IDX],
			conf_he_ppet[HE_PPE_CAP1_IDX], HE_PPE_CAP1_PPET16_FOR_NSS1_FOR_RU0);
	}

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP1_IDX],
			conf_he_ppet[HE_PPE_CAP1_IDX], HE_PPE_CAP1_PPET16_FOR_NSS1_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru2)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP2_IDX],
			conf_he_ppet[HE_PPE_CAP2_IDX], HE_PPE_CAP2_PPET16_FOR_NSS1_FOR_RU2);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts1_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP3_IDX],
			conf_he_ppet[HE_PPE_CAP3_IDX], HE_PPE_CAP3_PPET16_FOR_NSS1_FOR_RU3);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru0) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP3_IDX],
			conf_he_ppet[HE_PPE_CAP3_IDX], HE_PPE_CAP3_PPET16_FOR_NSS2_FOR_RU0);
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP4_IDX],
			conf_he_ppet[HE_PPE_CAP4_IDX], HE_PPE_CAP4_PPET16_FOR_NSS2_FOR_RU0);
	}

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP4_IDX],
			conf_he_ppet[HE_PPE_CAP4_IDX], HE_PPE_CAP4_PPET16_FOR_NSS2_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru2)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP5_IDX],
			conf_he_ppet[HE_PPE_CAP5_IDX], HE_PPE_CAP5_PPET16_FOR_NSS2_FOR_RU2);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts2_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP6_IDX],
			conf_he_ppet[HE_PPE_CAP6_IDX], HE_PPE_CAP6_PPET16_FOR_NSS2_FOR_RU3);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru0) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP6_IDX],
			conf_he_ppet[HE_PPE_CAP6_IDX], HE_PPE_CAP6_PPET16_FOR_NSS3_FOR_RU0);
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP7_IDX],
			conf_he_ppet[HE_PPE_CAP7_IDX], HE_PPE_CAP7_PPET16_FOR_NSS3_FOR_RU0);
	}

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP7_IDX],
			conf_he_ppet[HE_PPE_CAP7_IDX], HE_PPE_CAP7_PPET16_FOR_NSS3_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru2)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP8_IDX],
			conf_he_ppet[HE_PPE_CAP8_IDX], HE_PPE_CAP8_PPET16_FOR_NSS3_FOR_RU2);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts3_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP9_IDX],
			conf_he_ppet[HE_PPE_CAP9_IDX], HE_PPE_CAP9_PPET16_FOR_NSS3_FOR_RU3);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru0) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP9_IDX],
			conf_he_ppet[HE_PPE_CAP9_IDX], HE_PPE_CAP9_PPET16_FOR_NSS4_FOR_RU0);
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP10_IDX],
			conf_he_ppet[HE_PPE_CAP10_IDX], HE_PPE_CAP10_PPET16_FOR_NSS4_FOR_RU0);
	}

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP10_IDX],
			conf_he_ppet[HE_PPE_CAP10_IDX], HE_PPE_CAP10_PPET16_FOR_NSS4_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru2)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP11_IDX],
			conf_he_ppet[HE_PPE_CAP11_IDX], HE_PPE_CAP11_PPET16_FOR_NSS4_FOR_RU2);

	if (cap_idx->he_ppe_thresholds_ppet16_for_nsts4_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP12_IDX],
			conf_he_ppet[HE_PPE_CAP12_IDX], HE_PPE_CAP12_PPET16_FOR_NSS4_FOR_RU3);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru0)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP1_IDX],
			conf_he_ppet[HE_PPE_CAP1_IDX], HE_PPE_CAP1_PPET8_FOR_NSS1_FOR_RU0);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP2_IDX],
			conf_he_ppet[HE_PPE_CAP2_IDX], HE_PPE_CAP2_PPET8_FOR_NSS1_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru2) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP2_IDX],
			conf_he_ppet[HE_PPE_CAP2_IDX], HE_PPE_CAP2_PPET8_FOR_NSS1_FOR_RU2 );
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP3_IDX],
			conf_he_ppet[HE_PPE_CAP3_IDX], HE_PPE_CAP3_PPET8_FOR_NSS1_FOR_RU2);
	}

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts1_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP3_IDX],
			conf_he_ppet[HE_PPE_CAP3_IDX], HE_PPE_CAP3_PPET8_FOR_NSS1_FOR_RU3);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru0)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP4_IDX],
			conf_he_ppet[HE_PPE_CAP4_IDX], HE_PPE_CAP4_PPET8_FOR_NSS2_FOR_RU0);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP5_IDX],
			conf_he_ppet[HE_PPE_CAP5_IDX], HE_PPE_CAP5_PPET8_FOR_NSS2_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru2) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP5_IDX],
			conf_he_ppet[HE_PPE_CAP5_IDX], HE_PPE_CAP5_PPET8_FOR_NSS2_FOR_RU2);
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP6_IDX],
			conf_he_ppet[HE_PPE_CAP6_IDX], HE_PPE_CAP6_PPET8_FOR_NSS2_FOR_RU2);
	}

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts2_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP6_IDX],
			conf_he_ppet[HE_PPE_CAP6_IDX], HE_PPE_CAP6_PPET8_FOR_NSS2_FOR_RU3);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru0)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP7_IDX],
			conf_he_ppet[HE_PPE_CAP7_IDX], HE_PPE_CAP7_PPET8_FOR_NSS3_FOR_RU0);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP8_IDX],
			conf_he_ppet[HE_PPE_CAP8_IDX], HE_PPE_CAP8_PPET8_FOR_NSS3_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru2) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP8_IDX],
			conf_he_ppet[HE_PPE_CAP8_IDX], HE_PPE_CAP8_PPET8_FOR_NSS3_FOR_RU2);
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP9_IDX],
			conf_he_ppet[HE_PPE_CAP9_IDX], HE_PPE_CAP9_PPET8_FOR_NSS3_FOR_RU2);
	}

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts3_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP9_IDX],
			conf_he_ppet[HE_PPE_CAP9_IDX], HE_PPE_CAP9_PPET8_FOR_NSS3_FOR_RU3);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru0)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP10_IDX],
			conf_he_ppet[HE_PPE_CAP10_IDX], HE_PPE_CAP10_PPET8_FOR_NSS4_FOR_RU0);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru1)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP11_IDX],
			conf_he_ppet[HE_PPE_CAP11_IDX], HE_PPE_CAP11_PPET8_FOR_NSS4_FOR_RU1);

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru2) {
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP11_IDX],
			conf_he_ppet[HE_PPE_CAP11_IDX], HE_PPE_CAP11_PPET8_FOR_NSS4_FOR_RU2);
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP12_IDX],
			conf_he_ppet[HE_PPE_CAP12_IDX], HE_PPE_CAP12_PPET8_FOR_NSS4_FOR_RU2);
	}

	if (cap_idx->he_ppe_thresholds_ppet8_for_nsts4_for_ru3)
		ltq_clr_set_he_cap(&debug_he_ppet[HE_PPE_CAP12_IDX],
			conf_he_ppet[HE_PPE_CAP12_IDX], HE_PPE_CAP12_PPET8_FOR_NSS4_FOR_RU3);
}
#endif /* _VENDOR_CMDS_H_ */
