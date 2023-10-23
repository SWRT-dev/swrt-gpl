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
 * Core's parameters DB definitions
 *
 * Written by: Grygorii Strashko
 *
 */

#ifndef __CORE_PDB_DEF_H_
#define __CORE_PDB_DEF_H_

#include "mtlk_param_db.h"
#include "channels.h"
#include "mtlkaux.h"
#include "core_priv.h"
#include "mtlk_coreui.h"
#include "mtlk_psdb.h"
#include "scan_support.h"
#include "wave_80211ax.h"

#define MTLK_IDEFS_ON
#include "mtlkidefs.h"

static const uint8   mtlk_core_initial_zero_uint8 = 0;
static const uint32  mtlk_core_initial_zero_int = 0;
static const uint32  mtlk_core_initial_arp_proxy = 0;
static const uint32  mtlk_core_initial_bridge_mode = 0;
static const uint32  mtlk_core_initial_ap_forwarding = 1;

static const uint8   mtlk_core_initial_mac_addr[ETH_ALEN] = {0};
static const uint8   mtlk_core_initial_bssid[ETH_ALEN] = {0};

static const uint32  mtlk_core_initial_sta_force_spectrum_mode = CW_40;
static const char    mtlk_core_initial_nick_name[] = "";
static const char    mtlk_core_initial_essid[] = "";
static const uint32  mtlk_core_initial_net_mode_cfg = MTLK_NETWORK_11A_ONLY;
static const uint32  mtlk_core_initial_net_mode_cur = MTLK_NETWORK_11A_ONLY;

static const uint32  mtlk_core_initial_tx_power_psd = 0;
static const uint32  mtlk_core_initial_tx_power_cfg = 0;

static const uint32  mtlk_core_initial_hidden_ssid = 0; // NL80211_HIDDEN_SSID_NOT_IN_USE
static const uint32  mtlk_core_initial_is_bss_load_enable = 0;

static const uint32  mtlk_core_initial_four_addr_mode = 0;
static const uint32  mtlk_core_initial_invalid_uint8  = MTLK_PARAM_DB_INVALID_UINT8;
static const uint32  mtlk_core_initial_invalid_uint32 = MTLK_PARAM_DB_INVALID_UINT32;
static const uint32  wave_core_initial_HT_prot = PROTECTION_MODE_NO_FORCE_PROTECTION;
static const uint32  wave_core_initial_DTIM_period = 5;
static const uint32  wave_core_initial_mbssid_vap = 0;
static const uint32  wave_core_initial_mbssid_num_vaps_in_group = 0;
static const uint32  mtlk_core_initial_softblock_disable = 0; /* enable softblock by default */
static const uint32  mtlk_core_initial_beacon_period = 100;
static const char    wave_core_initial_csa_deauth_params[] = "";
static const char    wave_core_initial_rnr_6g_short_ssid_list[] = "";
static const char    wave_core_initial_bcast_twt_config[] = "";
static const uint32  wave_core_initial_exce_retry_limit = 50;

#define B_RATE 0x80
#define MBIT_RATE_ENCODING_MUL 2
#define HUNDRED_KBIT_RATE_ENCODING_DIV 5

static const uint8   mtlk_core_initial_basic_rates_2[] =
{
  (1 * MBIT_RATE_ENCODING_MUL),
  (2 * MBIT_RATE_ENCODING_MUL),
  (55 / HUNDRED_KBIT_RATE_ENCODING_DIV),
  (11 * MBIT_RATE_ENCODING_MUL)
};

static const uint8   mtlk_core_initial_basic_rates_5[] =
{
  (6 * MBIT_RATE_ENCODING_MUL),
  (12 * MBIT_RATE_ENCODING_MUL),
  (24 * MBIT_RATE_ENCODING_MUL)
};

static const uint8 mtlk_core_initial_set_bss_flags = 0;
static const uint8 mtlk_core_initial_rx_mcs_bitmask[10] = { 0 };
static const uint8 mtlk_core_initial_vht_mcs_nss[8] = { 0 };

/* For swaps that need to be done at compile time */
#define COMPILE_TIME_SWAP16(x) (((x & 0xff) << 8) | (x & 0xff00))

#ifdef __LITTLE_ENDIAN
#  define PDB_HOST_TO_MAC16(x) (x)
#endif
#ifdef __BIG_ENDIAN
#  define PDB_HOST_TO_MAC16(x) COMPILE_TIME_SWAP16(x)
#endif

#define PDB_HOST_TO_MAC16_4(a, b, c, d)        \
  PDB_HOST_TO_MAC16(a), PDB_HOST_TO_MAC16(b), PDB_HOST_TO_MAC16(c), PDB_HOST_TO_MAC16(d)

                                                                   /* txop cwmin cwmax    aifs          ac */
static const struct mtlk_txq_params mtlk_core_initial_wmm_params_be = {  6,   3,   4,        0,  0, UMI_AC_BE };
static const struct mtlk_txq_params mtlk_core_initial_wmm_params_bk = { 10,   7,   4,        0,  0, UMI_AC_BK };
static const struct mtlk_txq_params mtlk_core_initial_wmm_params_vi = {  4,   1,   3,  3008/32,  0, UMI_AC_VI };
static const struct mtlk_txq_params mtlk_core_initial_wmm_params_vo = {  3,   1,   2,  1504/32,  0, UMI_AC_VO };

static const uint32  mtlk_core_initial_power_limit = 4;

static const uint32  mtlk_core_initial_disconnect_on_nacks_weight = 0;
static const uint32  mtlk_core_initial_mib_use_long_preamble_for_multicast = 0;
static const uint32 mtlk_core_initial_enable_radio = TRUE;

/* 802.11AX support */
static const uint8 mtlk_core_initial_he_mcs_nss[8] = { 0 };
static const struct intel_vendor_he_oper mtlk_core_initial_he_operation = {.he_oper_params[0] = HE_OPERATION_CAP0,
                                                                           .he_oper_params[1] = HE_OPERATION_CAP1,
                                                                           .he_oper_params[2] = HE_OPERATION_CAP2,
                                                                           .he_bss_color_info = HE_OPERATION_CAP3,
                                                                           .he_mcs_nss_set[0] = HE_OPERATION_CAP4,
                                                                           .he_mcs_nss_set[1] = HE_OPERATION_CAP5,
                                                                           .co_hosted_bss     = HE_OPERATION_CAP6};
static const struct intel_vendor_he_capa mtlk_core_initial_he_non_advertised =
                                         {.he_mac_capab_info[HE_MAC_CAP0_IDX] = HE_NON_ADVERT_MAC_CAP0,
                                          .he_mac_capab_info[HE_MAC_CAP1_IDX] = HE_NON_ADVERT_MAC_CAP1,
                                          .he_mac_capab_info[HE_MAC_CAP2_IDX] = HE_NON_ADVERT_MAC_CAP2,
                                          .he_mac_capab_info[HE_MAC_CAP3_IDX] = HE_NON_ADVERT_MAC_CAP3,
                                          .he_mac_capab_info[HE_MAC_CAP4_IDX] = HE_NON_ADVERT_MAC_CAP4,
                                          .he_mac_capab_info[HE_MAC_CAP5_IDX] = HE_NON_ADVERT_MAC_CAP5,

                                          .he_phy_capab_info[HE_PHY_CAP0_IDX] = HE_NON_ADVERT_PHY_CAP0,
                                          .he_phy_capab_info[HE_PHY_CAP1_IDX] = HE_NON_ADVERT_PHY_CAP1,
                                          .he_phy_capab_info[HE_PHY_CAP2_IDX] = HE_NON_ADVERT_PHY_CAP2,
                                          .he_phy_capab_info[HE_PHY_CAP3_IDX] = HE_NON_ADVERT_PHY_CAP3,
                                          .he_phy_capab_info[HE_PHY_CAP4_IDX] = HE_NON_ADVERT_PHY_CAP4,
                                          .he_phy_capab_info[HE_PHY_CAP5_IDX] = HE_NON_ADVERT_PHY_CAP5,
                                          .he_phy_capab_info[HE_PHY_CAP6_IDX] = HE_NON_ADVERT_PHY_CAP6,
                                          .he_phy_capab_info[HE_PHY_CAP7_IDX] = HE_NON_ADVERT_PHY_CAP7,
                                          .he_phy_capab_info[HE_PHY_CAP8_IDX] = HE_NON_ADVERT_PHY_CAP8,};
static const struct mtlk_he_debug_mode_data mtlk_core_initial_he_debug_mode_data = { 0 };

static const mtlk_pdb_initial_value mtlk_core_parameters[] =
{
  /* ID,                          TYPE,                 FLAGS,                        SIZE,                            POINTER TO CONST */
  /**/
  {PARAM_DB_CORE_MAC_ADDR,        PARAM_DB_TYPE_MAC,    PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_mac_addr),        mtlk_core_initial_mac_addr},
  {PARAM_DB_CORE_ARP_PROXY,       PARAM_DB_TYPE_INT,    PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_arp_proxy),       &mtlk_core_initial_arp_proxy},
  {PARAM_DB_CORE_BRIDGE_MODE,     PARAM_DB_TYPE_INT,    PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_bridge_mode),     &mtlk_core_initial_bridge_mode},
  {PARAM_DB_CORE_AP_FORWARDING,   PARAM_DB_TYPE_INT,    PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_ap_forwarding),   &mtlk_core_initial_ap_forwarding},
  {PARAM_DB_CORE_RELIABLE_MCAST,  PARAM_DB_TYPE_INT,    PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_invalid_uint32),  &mtlk_core_initial_invalid_uint32},
  {PARAM_DB_CORE_BSSID,           PARAM_DB_TYPE_MAC,    PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_bssid),        mtlk_core_initial_bssid},
  {PARAM_DB_CORE_STA_FORCE_SPECTRUM_MODE,  PARAM_DB_TYPE_INT,    PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_sta_force_spectrum_mode),   &mtlk_core_initial_sta_force_spectrum_mode},
  {PARAM_DB_CORE_NICK_NAME,       PARAM_DB_TYPE_STRING, PARAM_DB_VALUE_FLAG_NO_FLAG,  MTLK_ESSID_MAX_SIZE,   mtlk_core_initial_nick_name},
  {PARAM_DB_CORE_ESSID,           PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG,  MTLK_ESSID_MAX_SIZE,   mtlk_core_initial_essid},
  {PARAM_DB_CORE_NET_MODE_CFG,    PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_net_mode_cfg),   &mtlk_core_initial_net_mode_cfg},
  {PARAM_DB_CORE_NET_MODE_CUR,    PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_net_mode_cur),   &mtlk_core_initial_net_mode_cur},
  {PARAM_DB_CORE_HIDDEN_SSID, PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_hidden_ssid),  &mtlk_core_initial_hidden_ssid},
  {PARAM_DB_CORE_BASIC_RATES_2, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_basic_rates_2), &mtlk_core_initial_basic_rates_2},
  {PARAM_DB_CORE_BASIC_RATES_5, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_basic_rates_5), &mtlk_core_initial_basic_rates_5},
  {PARAM_DB_CORE_RX_MCS_BITMASK, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_rx_mcs_bitmask), &mtlk_core_initial_rx_mcs_bitmask},
  {PARAM_DB_CORE_VHT_MCS_NSS, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_vht_mcs_nss), &mtlk_core_initial_vht_mcs_nss},
  {PARAM_DB_CORE_HE_MCS_NSS, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_he_mcs_nss), &mtlk_core_initial_he_mcs_nss},
  {PARAM_DB_CORE_HE_OPERATION, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_he_operation), &mtlk_core_initial_he_operation},
  {PARAM_DB_CORE_HE_NON_ADVERTISED, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_he_non_advertised), &mtlk_core_initial_he_non_advertised},
  {PARAM_DB_CORE_HE_DEBUG_DATA, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_he_debug_mode_data), &mtlk_core_initial_he_debug_mode_data},
  {PARAM_DB_CORE_SET_BSS_FLAGS, PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG,  sizeof(mtlk_core_initial_set_bss_flags), &mtlk_core_initial_set_bss_flags},
  {PARAM_DB_CORE_WMM_PARAMS_BE, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_wmm_params_be), &mtlk_core_initial_wmm_params_be},
  {PARAM_DB_CORE_WMM_PARAMS_BK, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_wmm_params_bk), &mtlk_core_initial_wmm_params_bk},
  {PARAM_DB_CORE_WMM_PARAMS_VI, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_wmm_params_vi), &mtlk_core_initial_wmm_params_vi},
  {PARAM_DB_CORE_WMM_PARAMS_VO, PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_wmm_params_vo), &mtlk_core_initial_wmm_params_vo},
  {PARAM_DB_CORE_IS_BSS_LOAD_ENABLE,       PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_is_bss_load_enable),       &mtlk_core_initial_is_bss_load_enable},
  {PARAM_DB_CORE_ADMISSION_CAPACITY,       PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_invalid_uint32),           &mtlk_core_initial_invalid_uint32},
  {PARAM_DB_CORE_RX_THRESHOLD,             PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_invalid_uint32),           &mtlk_core_initial_invalid_uint32},
  {PARAM_DB_CORE_AMSDU_MODE,               PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_invalid_uint8),            &mtlk_core_initial_invalid_uint8},
  {PARAM_DB_CORE_BA_MODE,                  PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_invalid_uint8),            &mtlk_core_initial_invalid_uint8},
  {PARAM_DB_CORE_WINDOW_SIZE,              PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_invalid_uint32),           &mtlk_core_initial_invalid_uint32},
  {PARAM_DB_CORE_IWPRIV_FORCED,            PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int),                 &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_HT_PROTECTION,            PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(wave_core_initial_HT_prot),                  &wave_core_initial_HT_prot},
  {PARAM_DB_CORE_TWT_OPERATION_MODE,       PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_uint8),               &mtlk_core_initial_zero_uint8 },
  {PARAM_DB_CORE_MESH_MODE,                PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int),                 &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_DTIM_PERIOD,              PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(wave_core_initial_DTIM_period),              &wave_core_initial_DTIM_period},
  {PARAM_DB_CORE_MBSSID_VAP,               PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(wave_core_initial_mbssid_vap),               &wave_core_initial_mbssid_vap},
  {PARAM_DB_CORE_MBSSID_NUM_VAPS_IN_GROUP, PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(wave_core_initial_mbssid_num_vaps_in_group), &wave_core_initial_mbssid_num_vaps_in_group},
  {PARAM_DB_CORE_SOFTBLOCK_DISABLE,        PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_softblock_disable),        &mtlk_core_initial_softblock_disable},
  {PARAM_DB_CORE_MGMT_FRAMES_RATE,         PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_invalid_uint8),            &mtlk_core_initial_invalid_uint8},
  {PARAM_DB_CORE_BEACON_PERIOD,            PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_beacon_period),            &mtlk_core_initial_beacon_period},
  {PARAM_DB_CORE_FWRD_UNKWN_MCAST_FLAG,    PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int),                 &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_CSA_DEAUTH_PARAMS,        PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(struct intel_vendor_channel_switch_cfg),   wave_core_initial_csa_deauth_params},
  {PARAM_DB_CORE_RNR_6G_SHORT_SSID_LIST,   PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(wave_core_initial_rnr_6g_short_ssid_list), wave_core_initial_rnr_6g_short_ssid_list},
  {PARAM_DB_CORE_RNR_6G_SHORT_SSID_COUNT,  PARAM_DB_TYPE_INT,    PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int),              &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_HE_BEACON, PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int), &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_SHORT_SSID, PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int), &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_BCAST_TWT_OPERATION_MODE, PARAM_DB_TYPE_INT, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_uint8),               &mtlk_core_initial_zero_uint8 },
  {PARAM_DB_CORE_BCAST_TWT_CONFIG,        PARAM_DB_TYPE_BINARY, PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(wave_core_initial_bcast_twt_config), wave_core_initial_bcast_twt_config},
  {PARAM_DB_CORE_SB_TIMER_ACL_ON,         PARAM_DB_TYPE_INT,  PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int),                 &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_SB_TIMER_ACL_PROBE_DROP, PARAM_DB_TYPE_INT,  PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(mtlk_core_initial_zero_int),                 &mtlk_core_initial_zero_int},
  {PARAM_DB_CORE_AP_RETRY_LIMIT_EXCE,     PARAM_DB_TYPE_INT,  PARAM_DB_VALUE_FLAG_NO_FLAG, sizeof(wave_core_initial_exce_retry_limit),         &wave_core_initial_exce_retry_limit},
  {PARAM_DB_CORE_LAST_VALUE_ID,       0,                    0,                            0,                               NULL},
};

#define MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __CORE_PDB_DEF_H_ */
