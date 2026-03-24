/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include <net/iw_handler.h>
#include <net/cfg80211.h>
#include <net/mac80211.h>
#ifndef CPTCFG_IWLWAV_X86_HOST_PC
#include <../net/wireless/core.h>
#endif
#include <linux/netdevice.h>

#include "mtlk_clipboard.h"
#include "mtlk_vap_manager.h"
#include "mtlk_coreui.h"
#include "mtlkdfdefs.h"
#include "mtlk_df_user_priv.h"
#include "mtlk_df_priv.h"
#include "mtlk_df.h"
#include "cfg80211.h"
#include "mac80211.h"
#include "mtlkaux.h"
#include "frame.h"
#include "mtlk_packets.h"
#include "wds.h"
#include "mtlk_df_nbuf.h"
#include "mtlk_param_db.h"
#include "core_config.h"
#include "vendor_cmds.h"
#include "wave_radio.h"
#include "wave_80211ax.h"

#define LOG_LOCAL_GID             GID_MAC80211
#define LOG_LOCAL_FID             0

#define MAC80211_HW_TX_QUEUES     4

/* Let FW send CSAs to STAs associated to AP mode VAPs */
#define DEFAULT_CSA_COUNT         5

#define LINUX_ERR_OK              0

#define TX_STATUS_SUCCESS         1

#define API_WRAPPER(api_function) api_function ## _recovery_retry_wrapper

#define MAC80211_WIDAN_NDP_TID    7
#define MAC80211_WIDAN_NDP_TIMEOUT  10
#define MAC80211_CONNECTION_IDLE_TIME  (30 * HZ)

#define RCVRY_RETRY_TIMEOUT_MS    10000

struct _wv_mac80211_t
{
  BOOL registered;
  /* mtlk_vap_manager_t *vap_manager; */
  wave_radio_t        *radio;
  struct ieee80211_hw *mac80211_hw;
  struct ieee80211_vif **vif_array;
  int vif_array_size;
  mtlk_osal_timer_t csa_timer;
};

enum ww_csa_status {
  WV_CSA_NOT_DONE,
  WV_CSA_SUCCESS,
  WV_CSA_FAILED,
};

/* Per interface driver private date allocated by mac80211 upon interface creation*/
struct wv_vif_priv {
  struct ieee80211_vif *vif;
  mtlk_df_user_t *df_user;
  u8 vap_index;
  mtlk_vap_handle_t vap_handle;
  u16 vif_sid;
  enum ieee80211_sta_state current_state;
  BOOL is_initialized;
  unsigned long latest_rx_and_tx_packets;
  int ndp_counter;
  enum ww_csa_status csa_status;
};

/* For async channel switch serializer actions */
struct _wv_chswitch_ctx
{
  struct ieee80211_vif *vif;
  struct ieee80211_channel_switch ch_switch;
  struct ieee80211_hw *hw;
};

struct _wv_vifs_counters {
  u8 num_vifs;
  u8 num_sta_vifs;
  u8 num_connected_sta_vifs;
};

/* Beacon configuration */
struct _wv_beacon_cfg {
  struct sk_buff *beacon_tmpl;  /* Beacon template */
  struct sk_buff *probe_resp;   /* Probe response */
  struct cfg80211_beacon_data beacon_data;
};

static const uint32 _cipher_suites[] = {
  WLAN_CIPHER_SUITE_USE_GROUP,
  WLAN_CIPHER_SUITE_WEP40,
  WLAN_CIPHER_SUITE_TKIP,
  WLAN_CIPHER_SUITE_CCMP,
  WLAN_CIPHER_SUITE_WEP104,
  WLAN_CIPHER_SUITE_AES_CMAC,
  WLAN_CIPHER_SUITE_SMS4
};

static const uint32 _cipher_suites_gcmp[] = {
  WLAN_CIPHER_SUITE_USE_GROUP,
  WLAN_CIPHER_SUITE_WEP40,
  WLAN_CIPHER_SUITE_TKIP,
  WLAN_CIPHER_SUITE_CCMP,
  WLAN_CIPHER_SUITE_WEP104,
  WLAN_CIPHER_SUITE_AES_CMAC,
  WLAN_CIPHER_SUITE_SMS4,
  WLAN_CIPHER_SUITE_GCMP,
  WLAN_CIPHER_SUITE_GCMP_256,
  WLAN_CIPHER_SUITE_BIP_GMAC_128,
  WLAN_CIPHER_SUITE_BIP_GMAC_256
};

#define WV_STYPE_ALL              0xFFFF
#define WV_STYPE_STA_RX           (BIT(IEEE80211_STYPE_ACTION >> 4) |       \
                                   BIT(IEEE80211_STYPE_ASSOC_RESP >> 4) |  \
                                   BIT(IEEE80211_STYPE_REASSOC_RESP >> 4) |  \
                                   BIT(IEEE80211_STYPE_PROBE_RESP >> 4) |  \
                                   BIT(IEEE80211_STYPE_BEACON >> 4) |  \
                                   BIT(IEEE80211_STYPE_AUTH >> 4) |  \
                                   BIT(IEEE80211_STYPE_DISASSOC >> 4))

#define WV_STYPE_AP_RX            (BIT(IEEE80211_STYPE_ASSOC_REQ >> 4)   | \
                                   BIT(IEEE80211_STYPE_REASSOC_REQ >> 4) | \
                                   BIT(IEEE80211_STYPE_PROBE_REQ >> 4)   | \
                                   BIT(IEEE80211_STYPE_DISASSOC >> 4)    | \
                                   BIT(IEEE80211_STYPE_AUTH >> 4)        | \
                                   BIT(IEEE80211_STYPE_DEAUTH >> 4)      | \
                                   BIT(IEEE80211_STYPE_ACTION >> 4))

static const struct ieee80211_txrx_stypes
_wv_mgmt_stypes[NUM_NL80211_IFTYPES] = {
  [NL80211_IFTYPE_STATION] = {
    .tx = WV_STYPE_ALL,
    .rx = WV_STYPE_STA_RX
  },
  [NL80211_IFTYPE_AP] = {
    .tx = WV_STYPE_ALL,
    .rx = WV_STYPE_AP_RX
  },
  [NL80211_IFTYPE_AP_VLAN] = {
    /* Same as AP */
    .tx = WV_STYPE_ALL,
    .rx = WV_STYPE_AP_RX
  }
};

#define RATE(_rate, _rateid, _flags) {  \
  .bitrate        = (_rate),    \
  .hw_value       = (_rateid),   \
  .flags          = (_flags)    \
}

#define CHAN2G(_channel, _freq) {       \
  .band           = NL80211_BAND_2GHZ,  \
  .hw_value       = (_channel),         \
  .center_freq    = (_freq)             \
}

#define CHAN5G(_channel, _freq) {       \
  .band           = NL80211_BAND_5GHZ,  \
  .hw_value       = (_channel),         \
  .center_freq    = (_freq),            \
  .off_channel_dfs_cac_ms = 360000      \
}

#define CHAN5G_WEATHER(_channel, _freq) {       \
  .band           = NL80211_BAND_5GHZ,  \
  .hw_value       = (_channel),         \
  .center_freq    = (_freq),            \
  .off_channel_dfs_cac_ms = 5400000     \
}

#define CHAN6G(_channel, _freq) {       \
  .band           = NL80211_BAND_6GHZ,  \
  .hw_value       = (_channel),         \
  .center_freq    = (_freq)             \
}

static const IEEE_ADDR _bcast_addr =
  { {0xff, 0xff, 0xff, 0xff, 0xff, 0xff} };

/*****************************************************************************/

#define WAVE_WV_CHECK_PTR_VOID(__ptr) \
do { \
  if (!(__ptr)) { \
    mtlk_assert_log_ss(MTLK_SLID, "Station mode interface:", #__ptr); \
      return; \
  } \
} while (0)

#define WAVE_WV_CHECK_PTR_RES(__ptr) \
do { \
  if (!(__ptr)) { \
    mtlk_assert_log_ss(MTLK_SLID, "Station mode interface:", #__ptr); \
      return -EINVAL; \
  } \
} while (0)

#define WAVE_WV_CHECK_PTR_GOTO(__ptr, __label) \
do { \
  if (!(__ptr)) { \
    mtlk_assert_log_ss(MTLK_SLID, "Station mode interface:", #__ptr); \
      goto __label; \
  } \
} while (0)

/*****************************************************************************/
#define IEEE80211_HT_MCS_LIMIT    4
#define IEEE80211_VHT_MCS_LIMIT   8

#define WV_SUPP_RX_STBC 1

#define WV_SUPP_AMPDU_MAXLEN_EXP 20

#define WV_ENDIAN16_SHIFT_CHANGE(x) ((1 - (x) / 8) * 16 - 8 + (x))

#ifdef __LITTLE_ENDIAN
  #define WV_MCS_MAP_SS_SHIFT(x) (((x) - 1) * 2)
#endif
#ifdef __BIG_ENDIAN
  #define WV_MCS_MAP_SS_SHIFT(x) WV_ENDIAN16_SHIFT_CHANGE(((x) - 1) * 2)
#endif

#define VHT_CAP_FLAGS \
  (IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_3895 /* this is a 0 */                                              \
/* | IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_7991 */ /* this is a 1; value 3 in the first 2 bits is illegal */ \
   | IEEE80211_VHT_CAP_MAX_MPDU_LENGTH_11454 /* this is a 2 */                                           \
/* | IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ */                                                         \
/* | IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160_80PLUS80MHZ */                                                \
/* | IEEE80211_VHT_CAP_RXLDPC */                                                                         \
   | IEEE80211_VHT_CAP_SHORT_GI_80                                                                       \
/* | IEEE80211_VHT_CAP_SHORT_GI_160 */                                                                   \
   | IEEE80211_VHT_CAP_TXSTBC         /* Static configuration for all chips. Always ON. */               \
/* | IEEE80211_VHT_CAP_RXSTBC_1 set runtime depending on the chip */                                     \
/* | IEEE80211_VHT_CAP_RXSTBC_2 */                                                                       \
/* | IEEE80211_VHT_CAP_RXSTBC_3 */                                                                       \
/* | IEEE80211_VHT_CAP_RXSTBC_4 */                                                                       \
   | IEEE80211_VHT_CAP_SU_BEAMFORMER_CAPABLE                                                             \
   | IEEE80211_VHT_CAP_SU_BEAMFORMEE_CAPABLE                                                             \
/* avoid setting things dependent on STS-es now, leave them for later */                                 \
/* | ((WV_SUPP_NUM_STS - 1) << IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT) */                                \
/* | ((WV_SUPP_NUM_STS - 1) << IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_SHIFT) */                           \
   | IEEE80211_VHT_CAP_MU_BEAMFORMER_CAPABLE                                                             \
   | IEEE80211_VHT_CAP_VHT_TXOP_PS                                                                       \
/* | IEEE80211_VHT_CAP_HTC_VHT */                                                                        \
/* for VHT A-MPDU factor is 7<<23, which means MAX */                                                    \
   | ((WV_SUPP_AMPDU_MAXLEN_EXP - IEEE80211_HT_MAX_AMPDU_FACTOR)                                         \
      << IEEE80211_VHT_CAP_MAX_A_MPDU_LENGTH_EXPONENT_SHIFT)                                             \
/* | IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_UNSOL_MFB */                                              \
/* | IEEE80211_VHT_CAP_VHT_LINK_ADAPTATION_VHT_MRQ_MFB */                                                \
   | IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN                                                                \
/* | IEEE80211_VHT_CAP_TX_ANTENNA_PATTERN */ )

/* FIXCFG80211: set rates according to hardware */
static struct ieee80211_rate _2ghz_rates[] = {
  RATE(10, 11, 0),
  RATE(20, 12, IEEE80211_RATE_SHORT_PREAMBLE),
  RATE(55, 13, IEEE80211_RATE_SHORT_PREAMBLE),
  RATE(110, 10, IEEE80211_RATE_SHORT_PREAMBLE),
  RATE(60, 0, 0),
  RATE(90, 1, 0),
  RATE(120, 2, 0),
  RATE(180, 3, 0),
  RATE(240, 4, 0),
  RATE(360, 5, 0),
  RATE(480, 6, 0),
  RATE(540, 7, 0)
};

/* FIXCFG80211: set rates according to hardware */
static struct ieee80211_rate _5ghz_rates[] = {
  RATE(60, 0, 0),
  RATE(90, 1, 0),
  RATE(120, 2, 0),
  RATE(180, 3, 0),
  RATE(240, 4, 0),
  RATE(360, 5, 0),
  RATE(480, 6, 0),
  RATE(540, 7, 0)
};

/* template for 2.4 GHz channels, we'll take a copy before wiphy_register */
static struct ieee80211_channel _2ghz_channels[] = {
  CHAN2G(1, 2412),
  CHAN2G(2, 2417),
  CHAN2G(3, 2422),
  CHAN2G(4, 2427),
  CHAN2G(5, 2432),
  CHAN2G(6, 2437),
  CHAN2G(7, 2442),
  CHAN2G(8, 2447),
  CHAN2G(9, 2452),
  CHAN2G(10, 2457),
  CHAN2G(11, 2462),
  CHAN2G(12, 2467),
  CHAN2G(13, 2472),
  CHAN2G(14, 2484)
};

/* template for 5 GHz channels, we'll take a copy before wiphy_register */
static struct ieee80211_channel _5ghz_channels[] = {
  CHAN5G(36, 5180),
  CHAN5G(40, 5200),
  CHAN5G(44, 5220),
  CHAN5G(48, 5240),
  CHAN5G(52, 5260),
  CHAN5G(56, 5280),
  CHAN5G(60, 5300),
  CHAN5G(64, 5320),
  CHAN5G(100, 5500),
  CHAN5G(104, 5520),
  CHAN5G(108, 5540),
  CHAN5G(112, 5560),
  CHAN5G(116, 5580),
  CHAN5G_WEATHER(120, 5600),
  CHAN5G_WEATHER(124, 5620),
  CHAN5G_WEATHER(128, 5640),
  CHAN5G(132, 5660),
  CHAN5G(136, 5680),
  CHAN5G(140, 5700),
  CHAN5G(144, 5720),
  CHAN5G(149, 5745),
  CHAN5G(153, 5765),
  CHAN5G(157, 5785),
  CHAN5G(161, 5805),
  CHAN5G(165, 5825)
};

/* template for 6 GHz channels */
static struct ieee80211_channel _6ghz_channels[] = {
CHAN6G(1,5955),
CHAN6G(5,5975),
CHAN6G(9,5995),
CHAN6G(13,6015),
CHAN6G(17,6035),
CHAN6G(21,6055),
CHAN6G(25,6075),
CHAN6G(29,6095),
CHAN6G(33,6115),
CHAN6G(37,6135),
CHAN6G(41,6155),
CHAN6G(45,6175),
CHAN6G(49,6195),
CHAN6G(53,6215),
CHAN6G(57,6235),
CHAN6G(61,6255),
CHAN6G(65,6275),
CHAN6G(69,6295),
CHAN6G(73,6315),
CHAN6G(77,6335),
CHAN6G(81,6355),
CHAN6G(85,6375),
CHAN6G(89,6395),
CHAN6G(93,6415),/* U-NII-5 band end */
CHAN6G(97,6435),
CHAN6G(101,6455),
CHAN6G(105,6475),
CHAN6G(109,6495),
CHAN6G(113,6515),/* U-NII-6 band end */
CHAN6G(117,6535),
CHAN6G(121,6555),
CHAN6G(125,6575),
CHAN6G(129,6595),
CHAN6G(133,6615),
CHAN6G(137,6635),
CHAN6G(141,6655),
CHAN6G(145,6675),
CHAN6G(149,6695),
CHAN6G(153,6715),
CHAN6G(157,6735),
CHAN6G(161,6755),
CHAN6G(165,6775),
CHAN6G(169,6795),
CHAN6G(173,6815),
CHAN6G(177,6835),
CHAN6G(181,6855),
CHAN6G(185,6875),/* U-NII-7 band end */
CHAN6G(189,6895),
CHAN6G(193,6915),
CHAN6G(197,6935),
CHAN6G(201,6955),
CHAN6G(205,6975),
CHAN6G(209,6995),
CHAN6G(213,7015),
CHAN6G(217,7035),
CHAN6G(221,7055),
CHAN6G(225,7075),
CHAN6G(229,7095),
CHAN6G(233,7115) /* U-NII-8 band end */
};

#define WAVE600_B0_D2_HT_TX_BF_CAPA   (BIT(0) | BIT(3) | BIT(4) | BIT(10) | \
                                      BIT(16) | BIT(17) | BIT(18) | BIT(23) | \
                                      BIT(24) | BIT(27))
#define WAVE600_B0_D2_4X4_HT_TX_BF_CAPA (WAVE600_B0_D2_HT_TX_BF_CAPA | BIT(28))
#define WAVE600_A0_HT_TX_BF_CAPA     (BIT(0) | BIT(4) | BIT(10))

/* 2.4 GHz supported band template, we'll take a fresh copy for each wiphy we register */
static struct ieee80211_supported_band _supported_band_2ghz = {
  .n_channels = 0,  /* to be filled later */
  .channels = NULL, /* to be filled later */
  .n_bitrates = ARRAY_SIZE(_2ghz_rates),
  .bitrates = _2ghz_rates,
  /* FIXCFG80211: setup according to hardware */
  .ht_cap.cap = (/* IEEE80211_HT_CAP_LDPC_CODING     | Set runtime depending on the chip */
                 IEEE80211_HT_CAP_SUP_WIDTH_20_40 | /* Static configuration for all chips. Always ON. */
                 IEEE80211_HT_CAP_SGI_20          |
                 IEEE80211_HT_CAP_SGI_40          |
                 IEEE80211_HT_CAP_DSSSCCK40       |
                 IEEE80211_HT_CAP_MAX_AMSDU       |
                 IEEE80211_HT_CAP_SGI_20          |
                 IEEE80211_HT_CAP_SGI_40          |
                 IEEE80211_HT_CAP_TX_STBC         /* | Set runtime depending on the chip */
                 /* (WV_SUPP_RX_STBC << IEEE80211_HT_CAP_RX_STBC_SHIFT)*/),
  /* thexe are minimal rates for just 1 STS, to be adjusted below once we know the number of STS-es */
  .ht_cap.mcs.rx_mask[0] = 0xff,
  .ht_cap.mcs.rx_mask[4] = 0x01, /* "Bit 32 always on */
  /* .ht_cap.ampdu_factor   = IEEE80211_HT_MAX_AMPDU_64K, - Set runtime depending on the chip */
  /* .ht_cap.ampdu_density  = IEEE80211_HT_MPDU_DENSITY_16, */
  .ht_cap.mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED,
  .ht_cap.ht_supported = TRUE,
  .vht_cap.vht_supported = TRUE,
  .vht_cap.cap = VHT_CAP_FLAGS,
  /* MCS 0--9 supported for 1 spatial stream, no support for more; adjusted below when we know more */
  .vht_cap.vht_mcs.rx_mcs_map = ((IEEE80211_VHT_MCS_SUPPORT_0_9 << WV_MCS_MAP_SS_SHIFT(1))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(2))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(3))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(4))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(5))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(6))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(7))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(8))),
  /* MCS 0--9 supported for 1 spatial stream, no support for more; adjusted below when we know more */
  .vht_cap.vht_mcs.tx_mcs_map = ((IEEE80211_VHT_MCS_SUPPORT_0_9 << WV_MCS_MAP_SS_SHIFT(1))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(2))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(3))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(4))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(5))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(6))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(7))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(8)))
};

/* 5 GHz supported band template, we'll take a fresh copy for each wiphy we register */
static struct ieee80211_supported_band _supported_band_5ghz = {
  .n_channels = 0,  /* to be filled later */
  .channels = NULL, /* to be filled later */
  .n_bitrates = ARRAY_SIZE(_5ghz_rates),
  .bitrates = _5ghz_rates,
  /* FIXCFG80211: setup according to hardware */
  .ht_cap.cap = (/* IEEE80211_HT_CAP_LDPC_CODING     | - set runtime depending on the chip */
                 IEEE80211_HT_CAP_SUP_WIDTH_20_40 | /* Static configuration for all chips. Always ON. */
                 IEEE80211_HT_CAP_SGI_20          |
                 IEEE80211_HT_CAP_SGI_40          |
                 IEEE80211_HT_CAP_DSSSCCK40       |
                 IEEE80211_HT_CAP_MAX_AMSDU       |
                 IEEE80211_HT_CAP_SGI_20          |
                 IEEE80211_HT_CAP_SGI_40          |
                 IEEE80211_HT_CAP_TX_STBC         /* | Set runtime depending on the chip
                 (WV_SUPP_RX_STBC << IEEE80211_HT_CAP_RX_STBC_SHIFT)*/),
  /* thexe are minimal rates for just 1 STS, to be adjusted below once we know the number of STS-es */
  .ht_cap.mcs.rx_mask[0] = 0xff,
  .ht_cap.mcs.rx_mask[4] = 0x01, /* "Bit 32 always on */
  /* .ht_cap.ampdu_factor   = IEEE80211_HT_MAX_AMPDU_64K, - Set runtime depending on the chip */
  /* .ht_cap.ampdu_density  = IEEE80211_HT_MPDU_DENSITY_16, */
  .ht_cap.mcs.tx_params = IEEE80211_HT_MCS_TX_DEFINED,
  .ht_cap.ht_supported = TRUE,
  .vht_cap.vht_supported = TRUE,
  .vht_cap.cap = VHT_CAP_FLAGS,
  /* MCS 0--9 supported for 1 spatial stream, no support for more; adjusted below when we know more */
  .vht_cap.vht_mcs.rx_mcs_map = ((IEEE80211_VHT_MCS_SUPPORT_0_9 << WV_MCS_MAP_SS_SHIFT(1))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(2))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(3))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(4))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(5))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(6))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(7))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(8))),
  /* MCS 0--9 supported for 1 spatial stream, no support for more; adjusted below when we know more */
  .vht_cap.vht_mcs.tx_mcs_map = ((IEEE80211_VHT_MCS_SUPPORT_0_9 << WV_MCS_MAP_SS_SHIFT(1))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(2))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(3))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(4))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(5))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(6))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(7))
                                 | (IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(8)))
};

/* 6 GHz supported band template, we'll take a fresh copy for each wiphy we register */
static struct ieee80211_supported_band _supported_band_6ghz = {
  .n_channels = 0,  /* to be filled later */
  .channels = NULL, /* to be filled later */
  .n_bitrates = ARRAY_SIZE(_5ghz_rates),
  .bitrates = _5ghz_rates,
  /* FIXCFG80211: setup according to hardware */
  .ht_cap.cap = 0,
  /* thexe are minimal rates for just 1 STS, to be adjusted below once we know the number of STS-es */
  .ht_cap.mcs.rx_mask[0] = 0,
  .ht_cap.mcs.rx_mask[4] = 0, /* "Bit 32 always on */
  .ht_cap.mcs.tx_params = 0,
  .ht_cap.ht_supported = FALSE,
  .vht_cap.vht_supported = FALSE,
  .vht_cap.cap = 0,
  .vht_cap.vht_mcs.rx_mcs_map = 0,

  .vht_cap.vht_mcs.tx_mcs_map = 0
};

/* Common HE MAC PHY Capabilities for all bands and STS */
#define HE_MAC_CAP0       (IEEE80211_HE_MAC_CAP0_HTC_HE)
#define HE_MAC_CAP1       0
#define HE_MAC_CAP2       (IEEE80211_HE_MAC_CAP2_ACK_EN)
#define HE_MAC_CAP3       (IEEE80211_HE_MAC_CAP3_OMI_CONTROL | IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_VHT_2)
#define HE_MAC_CAP4       (IEEE80211_HE_MAC_CAP4_AMDSU_IN_AMPDU)
#define HE_MAC_CAP5       (IEEE80211_HE_MAC_CAP5_OM_CTRL_UL_MU_DATA_DIS_RX)
#define HE_5G_6G_MAC_CAP5 (HE_MAC_CAP5 | IEEE80211_HE_MAC_CAP5_UL_2x996_TONE_RU)

#define HE_PHY_CAP1       (IEEE80211_HE_PHY_CAP1_DEVICE_CLASS_A | IEEE80211_HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD)
#define HE_PHY_CAP1_5G_6G_D2 (HE_PHY_CAP1 | IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_MASK)
#define HE_PHY_CAP2       (IEEE80211_HE_PHY_CAP2_NDP_4x_LTF_AND_3_2US)
#define HE_PHY_CAP3       (IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER)
#define HE_PHY_CAP4       (IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_4 | \
                           IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_4 | \
                           IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE | IEEE80211_HE_PHY_CAP4_MU_BEAMFORMER)
#define HE_PHY_CAP6       (IEEE80211_HE_PHY_CAP6_TRIG_SU_BEAMFORMER_FB | \
                           IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT)
#define HE_PHY_CAP7       (IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI)
#define HE_PPE_THRES1     0x1c /*PPET16_NSTSn_RUx|PPET8_NSTSn_RUx..*/
#define HE_PPE_THRES2     0xc7
#define HE_PPE_THRES3     0x71
#define HE_PPE_THRES4     0xf1 /*..PPET16_NSTSn_RUx|PPET8_NSTSn_RUx */
#define HE_PPE_THRES_NOT_SUPPORTED 0xff

/* definataions for 6GHZ band capab */
/* B3,B4,B5 represents A-MPDU Length Exponent of VHT capab */
#define HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_1        ((u16) BIT(3))
#define HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_2        ((u16) BIT(4))
#define HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_3        ((u16) BIT(3) | BIT(4))
#define HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_4        ((u16) BIT(5))
#define HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_5        ((u16) BIT(3) | BIT(5))
#define HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_6        ((u16) BIT(4) | BIT(5))
#define HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MAX      ((u16) BIT(3) | \
							   BIT(4) | BIT(5))

/* B6,B7 represnts Maximum MPDU Length subfield of VHT capab */
#define HE_6G_CAP_MAX_MPDU_LENGTH_7991                ((u16) BIT(6))
#define HE_6G_CAP_MAX_MPDU_LENGTH_11454               ((u16) BIT(7))
#define HE_6G_CAP_MAX_MPDU_LENGTH_MASK                ((u16) BIT(6) | BIT(7))

/* B0,B1,B2 represents A-MPDU Parameters of HT capab */
#define HE_6G_BAND_CAPB_MINIMUM_MPDU_SPACING 0
/* B8 reserved */
/* B9,B10 represents SMPS subfield of HT capab */
#define HE_6G_BAND_CAPAB_SMPS ((u16) BIT(9) | BIT(10))
/* B11 RD Responder subfield of HT capab (currently not supported) */
/* B12 represents Rx Antenna Pattern Consistency subfield of VHT capab */
#define HE_6G_BAND_CAPAB_RX_ANTENNA_PATTERN ((u16) BIT(12))
/* B13 represnts Tx Antenna Pattern Consistency subfield of VHT capab */
#define HE_6G_BAND_CAPAB_TX_ANTENNA_PATTERN ((u16) BIT(13))

#define HE_6G_BAND_CAPAB_INFO (HE_6G_BAND_CAPB_MINIMUM_MPDU_SPACING | HE_6G_CAP_MAX_A_MPDU_LENGTH_EXPONENT_MAX | \
				HE_6G_CAP_MAX_MPDU_LENGTH_11454 | HE_6G_BAND_CAPAB_SMPS | HE_6G_BAND_CAPAB_RX_ANTENNA_PATTERN)

#define HE_6G_BAND_CAPAB_INVALID 0


/* Common for 2GHz band */
#define HE_2G_PHY_CAP0    (IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G | \
                          IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_RU_MAPPING_IN_2G)

/* Common for 5GHz and 6GHz bands */
#define HE_5G_6G_PHY_CAP0 (IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_80MHZ_IN_5G | \
                           IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_160MHZ_IN_5G)
#define HE_MCS_NSS_NOT_SUPPORTED cpu_to_le16(0xffff)

/* Common for 2 STS */
#define HE_2STS_PHY_CAP7  (HE_PHY_CAP7 | IEEE80211_HE_PHY_CAP7_MAX_NC_1)
#define HE_MCS_2SS_80     cpu_to_le16(0xfffa)
#define HE_MCS_2SS_160    HE_MCS_2SS_80
#define HE_PPE_THRES_2NSS  (IEEE80211_PPE_THRES0_NSTS_1 | IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK)

/* Common for 4 STS */
#define HE_4STS_PHY_CAP7  (HE_PHY_CAP7 | IEEE80211_HE_PHY_CAP7_MAX_NC_3)

/* Common HE MAC PHY Capabilities for all bands and STS. _D2 suffix is D2 specific */
#define HE_MAC_CAP1_D2    (0)
#define HE_MAC_CAP2_D2    (IEEE80211_HE_MAC_CAP2_ACK_EN | \
                           IEEE80211_HE_MAC_CAP2_BCAST_TWT) 

#define HE_PHY_CAP2_D2    (IEEE80211_HE_PHY_CAP2_STBC_TX_UNDER_80MHZ | \
                           IEEE80211_HE_PHY_CAP2_NDP_4x_LTF_AND_3_2US | \
                           IEEE80211_HE_PHY_CAP2_UL_MU_FULL_MU_MIMO)

#define HE_PHY_CAP3_D2    (IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER | \
                           IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_BPSK)


#define HE_PHY_CAP4_D2    (IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE | \
                           IEEE80211_HE_PHY_CAP4_MU_BEAMFORMER | \
                           IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_7 | \
                           IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_7)

#define HE_PHY_CAP6_D2    (IEEE80211_HE_PHY_CAP6_TRIG_SU_BEAMFORMER_FB | \
                           IEEE80211_HE_PHY_CAP6_PARTIAL_BW_EXT_RANGE | \
                           IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT)

#define HE_PHY_CAP9_D2    (IEEE80211_HE_PHY_CAP9_LONGER_THAN_16_SIGB_OFDM_SYM | \
                           IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU)

#define HE_PHY_CAP8_2GHZ_D2      (IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI | \
                                  IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_484)

#define HE_PHY_CAP8_5GHZ_6GHZ_D2 (IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI | \
                                  IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_2x996)

/* Common for 2 STS on A0 and B0 */
#define HE_2STS_PHY_CAP5    (IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_2 | \
                             IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_2 | \
                             IEEE80211_HE_PHY_CAP5_NG16_SU_FEEDBACK)
#define HE_2STS_PHY_CAP7    (HE_PHY_CAP7 | IEEE80211_HE_PHY_CAP7_MAX_NC_1)

/* Common for 2 STS on D2 */
#define HE_2STS_PHY_CAP5_D2 (IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_2 | \
                             IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_2 | \
                             IEEE80211_HE_PHY_CAP5_NG16_SU_FEEDBACK)
#define HE_2STS_PHY_CAP7_D2 (HE_PHY_CAP7 | IEEE80211_HE_PHY_CAP7_MAX_NC_1 | \
                             IEEE80211_HE_PHY_CAP7_STBC_TX_ABOVE_80MHZ)
#define HE_MCS_2SS_80        cpu_to_le16(0xfffa)
#define HE_MCS_2SS_160       HE_MCS_2SS_80
#define HE_PPE_THRES_2NSS   (IEEE80211_PPE_THRES0_NSTS_1 | IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK)
/* Common for 4 STS on A0 and B0 */
#define HE_4STS_PHY_CAP5    (IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_4 | \
                             IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_4 | \
                             IEEE80211_HE_PHY_CAP5_NG16_SU_FEEDBACK)
#define HE_4STS_PHY_CAP7    (HE_PHY_CAP7 | IEEE80211_HE_PHY_CAP7_MAX_NC_3)

/* Common for 4 STS on D2 */
#define HE_4STS_PHY_CAP5_D2 (IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_4 | \
                             IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_4 | \
                             IEEE80211_HE_PHY_CAP5_NG16_SU_FEEDBACK)
#define HE_4STS_PHY_CAP7_D2 (HE_PHY_CAP7 | IEEE80211_HE_PHY_CAP7_MAX_NC_3 | \
                             IEEE80211_HE_PHY_CAP7_STBC_TX_ABOVE_80MHZ)

#define HE_MCS_4SS_80     cpu_to_le16(0xffaa)
#define HE_MCS_4SS_160    HE_MCS_4SS_80
#define HE_PPE_THRES_4NSS (IEEE80211_PPE_THRES0_NSTS_3 | IEEE80211_PPE_THRES_RU_INDEX_BITMASK_MASK)

static const struct ieee80211_sband_iftype_data _mac80211_he_capa_2ghz_AB[] =
{
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1,
        .mac_cap_info[2] = HE_MAC_CAP2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_MAC_CAP5,

        .phy_cap_info[0] = HE_2G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1,
        .phy_cap_info[2] = HE_PHY_CAP2,
        .phy_cap_info[3] = HE_PHY_CAP3,
        .phy_cap_info[4] = HE_PHY_CAP4,
        .phy_cap_info[5] = HE_2STS_PHY_CAP5,
        .phy_cap_info[6] = HE_PHY_CAP6,
        .phy_cap_info[7] = HE_2STS_PHY_CAP7
        },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_2SS_80,
        .tx_mcs_80 =    HE_MCS_2SS_80,
        .rx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        },
        .ppe_thres[0]  = HE_PPE_THRES_2NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES4,
        .ppe_thres[7]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[8]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[9]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[10] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[11] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[12] = HE_PPE_THRES_NOT_SUPPORTED
      }
  },
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1,
        .mac_cap_info[2] = HE_MAC_CAP2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_MAC_CAP5,

        .phy_cap_info[0] = HE_2G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1,
        .phy_cap_info[2] = HE_PHY_CAP2,
        .phy_cap_info[3] = HE_PHY_CAP3,
        .phy_cap_info[4] = HE_PHY_CAP4,
        .phy_cap_info[5] = HE_4STS_PHY_CAP5,
        .phy_cap_info[6] = HE_PHY_CAP6,
        .phy_cap_info[7] = HE_4STS_PHY_CAP7
      },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_4SS_80,
        .tx_mcs_80 =    HE_MCS_4SS_80,
        .rx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        },
        .ppe_thres[0]  = HE_PPE_THRES_4NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES3,
        .ppe_thres[7]  = HE_PPE_THRES1,
        .ppe_thres[8]  = HE_PPE_THRES2,
        .ppe_thres[9]  = HE_PPE_THRES3,
        .ppe_thres[10] = HE_PPE_THRES1,
        .ppe_thres[11] = HE_PPE_THRES2,
        .ppe_thres[12] = HE_PPE_THRES3
      }
  }
};

static const struct ieee80211_sband_iftype_data _mac80211_he_capa_5ghz_AB[] =
{
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1,
        .mac_cap_info[2] = HE_MAC_CAP2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

        .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1,
        .phy_cap_info[2] = HE_PHY_CAP2,
        .phy_cap_info[3] = HE_PHY_CAP3,
        .phy_cap_info[4] = HE_PHY_CAP4,
        .phy_cap_info[5] = HE_2STS_PHY_CAP5,
        .phy_cap_info[6] = HE_PHY_CAP6,
        .phy_cap_info[7] = HE_2STS_PHY_CAP7
      },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_2SS_80,
        .tx_mcs_80 =    HE_MCS_2SS_80,
        .rx_mcs_160 =   HE_MCS_2SS_160,
        .tx_mcs_160 =   HE_MCS_2SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
       },
        .ppe_thres[0]  = HE_PPE_THRES_2NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES4,
        .ppe_thres[7]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[8]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[9]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[10] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[11] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[12] = HE_PPE_THRES_NOT_SUPPORTED
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INVALID
      }
  },
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
    .has_he = true,
    .he_cap_elem = {
      .mac_cap_info[0] = HE_MAC_CAP0,
      .mac_cap_info[1] = HE_MAC_CAP1,
      .mac_cap_info[2] = HE_MAC_CAP2,
      .mac_cap_info[3] = HE_MAC_CAP3,
      .mac_cap_info[4] = HE_MAC_CAP4,
      .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

      .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
      .phy_cap_info[1] = HE_PHY_CAP1,
      .phy_cap_info[2] = HE_PHY_CAP2,
      .phy_cap_info[3] = HE_PHY_CAP3,
      .phy_cap_info[4] = HE_PHY_CAP4,
      .phy_cap_info[5] = HE_4STS_PHY_CAP5,
      .phy_cap_info[6] = HE_PHY_CAP6,
      .phy_cap_info[7] = HE_4STS_PHY_CAP7
    },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_4SS_80,
        .tx_mcs_80 =    HE_MCS_4SS_80,
        .rx_mcs_160 =   HE_MCS_4SS_160,
        .tx_mcs_160 =   HE_MCS_4SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
      },
        .ppe_thres[0]  = HE_PPE_THRES_4NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES3,
        .ppe_thres[7]  = HE_PPE_THRES1,
        .ppe_thres[8]  = HE_PPE_THRES2,
        .ppe_thres[9]  = HE_PPE_THRES3,
        .ppe_thres[10] = HE_PPE_THRES1,
        .ppe_thres[11] = HE_PPE_THRES2,
        .ppe_thres[12] = HE_PPE_THRES3
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INVALID
      }
  }
};

static const struct ieee80211_sband_iftype_data _mac80211_he_capa_2ghz_D2[] =
 {
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1_D2,
        .mac_cap_info[2] = HE_MAC_CAP2_D2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_MAC_CAP5,

        .phy_cap_info[0] = HE_2G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1,
        .phy_cap_info[2] = HE_PHY_CAP2_D2,
        .phy_cap_info[3] = HE_PHY_CAP3_D2,
        .phy_cap_info[4] = HE_PHY_CAP4_D2,
        .phy_cap_info[5] = HE_2STS_PHY_CAP5_D2,
        .phy_cap_info[6] = HE_PHY_CAP6_D2,
        .phy_cap_info[7] = HE_2STS_PHY_CAP7_D2,
        .phy_cap_info[8] = HE_PHY_CAP8_2GHZ_D2,
        .phy_cap_info[9] = HE_PHY_CAP9_D2,
        },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_2SS_80,
        .tx_mcs_80 =    HE_MCS_2SS_80,
        .rx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        },
        .ppe_thres[0]  = HE_PPE_THRES_2NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES4,
        .ppe_thres[7]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[8]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[9]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[10] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[11] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[12] = HE_PPE_THRES_NOT_SUPPORTED
      }
  },
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1_D2,
        .mac_cap_info[2] = HE_MAC_CAP2_D2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_MAC_CAP5,

        .phy_cap_info[0] = HE_2G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1,
        .phy_cap_info[2] = HE_PHY_CAP2_D2,
        .phy_cap_info[3] = HE_PHY_CAP3_D2,
        .phy_cap_info[4] = HE_PHY_CAP4_D2,
        .phy_cap_info[5] = HE_4STS_PHY_CAP5_D2,
        .phy_cap_info[6] = HE_PHY_CAP6_D2,
        .phy_cap_info[7] = HE_4STS_PHY_CAP7_D2,
        .phy_cap_info[8] = HE_PHY_CAP8_2GHZ_D2,
        .phy_cap_info[9] = HE_PHY_CAP9_D2,
      },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_4SS_80,
        .tx_mcs_80 =    HE_MCS_4SS_80,
        .rx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_160 =   HE_MCS_NSS_NOT_SUPPORTED,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        },
        .ppe_thres[0]  = HE_PPE_THRES_4NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES3,
        .ppe_thres[7]  = HE_PPE_THRES1,
        .ppe_thres[8]  = HE_PPE_THRES2,
        .ppe_thres[9]  = HE_PPE_THRES3,
        .ppe_thres[10] = HE_PPE_THRES1,
        .ppe_thres[11] = HE_PPE_THRES2,
        .ppe_thres[12] = HE_PPE_THRES3
      }
  }
};

static const struct ieee80211_sband_iftype_data _mac80211_he_capa_5ghz_D2[] =
 {
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1_D2,
        .mac_cap_info[2] = HE_MAC_CAP2_D2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

        .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1_5G_6G_D2,
        .phy_cap_info[2] = HE_PHY_CAP2_D2,
        .phy_cap_info[3] = HE_PHY_CAP3_D2,
        .phy_cap_info[4] = HE_PHY_CAP4_D2,
        .phy_cap_info[5] = HE_2STS_PHY_CAP5_D2,
        .phy_cap_info[6] = HE_PHY_CAP6_D2,
        .phy_cap_info[7] = HE_2STS_PHY_CAP7_D2,
        .phy_cap_info[8] = HE_PHY_CAP8_5GHZ_6GHZ_D2,
        .phy_cap_info[9] = HE_PHY_CAP9_D2,
      },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_2SS_80,
        .tx_mcs_80 =    HE_MCS_2SS_80,
        .rx_mcs_160 =   HE_MCS_2SS_160,
        .tx_mcs_160 =   HE_MCS_2SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
       },
        .ppe_thres[0]  = HE_PPE_THRES_2NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES4,
        .ppe_thres[7]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[8]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[9]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[10] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[11] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[12] = HE_PPE_THRES_NOT_SUPPORTED
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INVALID
      }
  },
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
    .has_he = true,
    .he_cap_elem = {
      .mac_cap_info[0] = HE_MAC_CAP0,
      .mac_cap_info[1] = HE_MAC_CAP1_D2,
      .mac_cap_info[2] = HE_MAC_CAP2_D2,
      .mac_cap_info[3] = HE_MAC_CAP3,
      .mac_cap_info[4] = HE_MAC_CAP4,
      .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

      .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
      .phy_cap_info[1] = HE_PHY_CAP1_5G_6G_D2,
      .phy_cap_info[2] = HE_PHY_CAP2_D2,
      .phy_cap_info[3] = HE_PHY_CAP3_D2,
      .phy_cap_info[4] = HE_PHY_CAP4_D2,
      .phy_cap_info[5] = HE_4STS_PHY_CAP5_D2,
      .phy_cap_info[6] = HE_PHY_CAP6_D2,
      .phy_cap_info[7] = HE_4STS_PHY_CAP7_D2,
      .phy_cap_info[8] = HE_PHY_CAP8_5GHZ_6GHZ_D2,
      .phy_cap_info[9] = HE_PHY_CAP9_D2,
    },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_4SS_80,
        .tx_mcs_80 =    HE_MCS_4SS_80,
        .rx_mcs_160 =   HE_MCS_4SS_160,
        .tx_mcs_160 =   HE_MCS_4SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
      },
        .ppe_thres[0]  = HE_PPE_THRES_4NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES3,
        .ppe_thres[7]  = HE_PPE_THRES1,
        .ppe_thres[8]  = HE_PPE_THRES2,
        .ppe_thres[9]  = HE_PPE_THRES3,
        .ppe_thres[10] = HE_PPE_THRES1,
        .ppe_thres[11] = HE_PPE_THRES2,
        .ppe_thres[12] = HE_PPE_THRES3
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INVALID
      }
  }
};


static const struct ieee80211_sband_iftype_data _mac80211_he_capa_6ghz_AB[] =
{
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1,
        .mac_cap_info[2] = HE_MAC_CAP2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

        .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1,
        .phy_cap_info[2] = HE_PHY_CAP2,
        .phy_cap_info[3] = HE_PHY_CAP3,
        .phy_cap_info[4] = HE_PHY_CAP4,
        .phy_cap_info[5] = HE_2STS_PHY_CAP5,
        .phy_cap_info[6] = HE_PHY_CAP6,
        .phy_cap_info[7] = HE_2STS_PHY_CAP7
      },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_2SS_80,
        .tx_mcs_80 =    HE_MCS_2SS_80,
        .rx_mcs_160 =   HE_MCS_2SS_160,
        .tx_mcs_160 =   HE_MCS_2SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
       },
        .ppe_thres[0]  = HE_PPE_THRES_2NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES4,
        .ppe_thres[7]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[8]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[9]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[10] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[11] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[12] = HE_PPE_THRES_NOT_SUPPORTED
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INFO
      }
  },
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
    .has_he = true,
    .he_cap_elem = {
      .mac_cap_info[0] = HE_MAC_CAP0,
      .mac_cap_info[1] = HE_MAC_CAP1,
      .mac_cap_info[2] = HE_MAC_CAP2,
      .mac_cap_info[3] = HE_MAC_CAP3,
      .mac_cap_info[4] = HE_MAC_CAP4,
      .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

      .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
      .phy_cap_info[1] = HE_PHY_CAP1,
      .phy_cap_info[2] = HE_PHY_CAP2,
      .phy_cap_info[3] = HE_PHY_CAP3,
      .phy_cap_info[4] = HE_PHY_CAP4,
      .phy_cap_info[5] = HE_4STS_PHY_CAP5,
      .phy_cap_info[6] = HE_PHY_CAP6,
      .phy_cap_info[7] = HE_4STS_PHY_CAP7
    },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_4SS_80,
        .tx_mcs_80 =    HE_MCS_4SS_80,
        .rx_mcs_160 =   HE_MCS_4SS_160,
        .tx_mcs_160 =   HE_MCS_4SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
      },
        .ppe_thres[0]  = HE_PPE_THRES_4NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES3,
        .ppe_thres[7]  = HE_PPE_THRES1,
        .ppe_thres[8]  = HE_PPE_THRES2,
        .ppe_thres[9]  = HE_PPE_THRES3,
        .ppe_thres[10] = HE_PPE_THRES1,
        .ppe_thres[11] = HE_PPE_THRES2,
        .ppe_thres[12] = HE_PPE_THRES3
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INFO
      }
  }
};


static const struct ieee80211_sband_iftype_data _mac80211_he_capa_6ghz_D2[] =
 {
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
      .has_he = true,
      .he_cap_elem = {
        .mac_cap_info[0] = HE_MAC_CAP0,
        .mac_cap_info[1] = HE_MAC_CAP1_D2,
        .mac_cap_info[2] = HE_MAC_CAP2_D2,
        .mac_cap_info[3] = HE_MAC_CAP3,
        .mac_cap_info[4] = HE_MAC_CAP4,
        .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

        .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
        .phy_cap_info[1] = HE_PHY_CAP1_5G_6G_D2,
        .phy_cap_info[2] = HE_PHY_CAP2_D2,
        .phy_cap_info[3] = HE_PHY_CAP3_D2,
        .phy_cap_info[4] = HE_PHY_CAP4_D2,
        .phy_cap_info[5] = HE_2STS_PHY_CAP5_D2,
        .phy_cap_info[6] = HE_PHY_CAP6_D2,
        .phy_cap_info[7] = HE_2STS_PHY_CAP7_D2,
        .phy_cap_info[8] = HE_PHY_CAP8_5GHZ_6GHZ_D2,
        .phy_cap_info[9] = HE_PHY_CAP9_D2,
      },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_2SS_80,
        .tx_mcs_80 =    HE_MCS_2SS_80,
        .rx_mcs_160 =   HE_MCS_2SS_160,
        .tx_mcs_160 =   HE_MCS_2SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
       },
        .ppe_thres[0]  = HE_PPE_THRES_2NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES4,
        .ppe_thres[7]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[8]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[9]  = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[10] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[11] = HE_PPE_THRES_NOT_SUPPORTED,
        .ppe_thres[12] = HE_PPE_THRES_NOT_SUPPORTED
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INFO
      }
  },
  {
    .types_mask = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP),
    .he_cap = {
    .has_he = true,
    .he_cap_elem = {
      .mac_cap_info[0] = HE_MAC_CAP0,
      .mac_cap_info[1] = HE_MAC_CAP1_D2,
      .mac_cap_info[2] = HE_MAC_CAP2_D2,
      .mac_cap_info[3] = HE_MAC_CAP3,
      .mac_cap_info[4] = HE_MAC_CAP4,
      .mac_cap_info[5] = HE_5G_6G_MAC_CAP5,

      .phy_cap_info[0] = HE_5G_6G_PHY_CAP0,
      .phy_cap_info[1] = HE_PHY_CAP1_5G_6G_D2,
      .phy_cap_info[2] = HE_PHY_CAP2_D2,
      .phy_cap_info[3] = HE_PHY_CAP3_D2,
      .phy_cap_info[4] = HE_PHY_CAP4_D2,
      .phy_cap_info[5] = HE_4STS_PHY_CAP5_D2,
      .phy_cap_info[6] = HE_PHY_CAP6_D2,
      .phy_cap_info[7] = HE_4STS_PHY_CAP7_D2,
      .phy_cap_info[8] = HE_PHY_CAP8_5GHZ_6GHZ_D2,
      .phy_cap_info[9] = HE_PHY_CAP9_D2,
    },
      .he_mcs_nss_supp = {
        .rx_mcs_80 =    HE_MCS_4SS_80,
        .tx_mcs_80 =    HE_MCS_4SS_80,
        .rx_mcs_160 =   HE_MCS_4SS_160,
        .tx_mcs_160 =   HE_MCS_4SS_160,
        .rx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
        .tx_mcs_80p80 = HE_MCS_NSS_NOT_SUPPORTED,
      },
        .ppe_thres[0]  = HE_PPE_THRES_4NSS,
        .ppe_thres[1]  = HE_PPE_THRES1,
        .ppe_thres[2]  = HE_PPE_THRES2,
        .ppe_thres[3]  = HE_PPE_THRES3,
        .ppe_thres[4]  = HE_PPE_THRES1,
        .ppe_thres[5]  = HE_PPE_THRES2,
        .ppe_thres[6]  = HE_PPE_THRES3,
        .ppe_thres[7]  = HE_PPE_THRES1,
        .ppe_thres[8]  = HE_PPE_THRES2,
        .ppe_thres[9]  = HE_PPE_THRES3,
        .ppe_thres[10] = HE_PPE_THRES1,
        .ppe_thres[11] = HE_PPE_THRES2,
        .ppe_thres[12] = HE_PPE_THRES3
      },
      .he_6ghz_capa = {
         .capa = HE_6G_BAND_CAPAB_INFO
      }
  }
};

static void
_wv_ieee80211_free_beacon_cfg_data (struct _wv_beacon_cfg *wv_beacon_cfg);

static mtlk_error_t
_wv_ieee80211_get_beacon_cfg_data (struct ieee80211_hw *hw,
                                   struct ieee80211_vif *vif,
                                   struct _wv_beacon_cfg *wv_beacon_cfg);

static __INLINE struct _wv_mac80211_t *
__wv_ieee80211_hw_get_mac80211 (struct ieee80211_hw *mac80211_hw)
{
  return (wv_mac80211_t *)mac80211_hw->priv;
}

void * __MTLK_IFUNC
wv_mac80211_wiphy_get (wv_mac80211_t *mac80211)
{
  MTLK_ASSERT(mac80211 != NULL);

  return (void *)mac80211->mac80211_hw->wiphy;
}

static __INLINE struct scan_support *
__wv_mac80211_scan_support_get (wv_mac80211_t *mac80211)
{
  struct scan_support *ss = NULL;

  MTLK_ASSERT(NULL != mac80211);
  MTLK_ASSERT(NULL != mac80211->radio);
  if ((NULL != mac80211) && (NULL != mac80211->radio)) {
    ss = wave_radio_scan_support_get(mac80211->radio);
  }

  return ss;
}

static __INLINE struct mtlk_chan_def *
__wv_mac80211_chandef_get (wv_mac80211_t *mac80211)
{
  struct mtlk_chan_def *ccd = NULL;

  MTLK_ASSERT(NULL != mac80211);
  MTLK_ASSERT(NULL != mac80211->radio);
  if ((NULL != mac80211) && (NULL != mac80211->radio)) {
    ccd = wave_radio_chandef_get(mac80211->radio);
  }

  return ccd;
}

static __INLINE unsigned
__wv_mac80211_max_vaps_count_get (wv_mac80211_t *mac80211)
{
  MTLK_ASSERT(NULL != mac80211);
  MTLK_ASSERT(NULL != mac80211->radio);

  return mtlk_vap_manager_get_max_vaps_count(wave_radio_vap_manager_get(mac80211->radio));
}

static struct ieee80211_vif *
_wv_mac80211_get_vif (wv_mac80211_t *mac80211, u8 vap_index)
{
  struct ieee80211_vif *ieee80211_vif = NULL;

  MTLK_ASSERT(NULL != mac80211);
  if (NULL != mac80211->vif_array) {
    ieee80211_vif = mac80211->vif_array[vap_index];
  }
  return ieee80211_vif;
}

struct ieee80211_vif * __MTLK_IFUNC
wv_mac80211_get_vif (wv_mac80211_t *mac80211, u8 vap_index)
{
  return _wv_mac80211_get_vif (mac80211, vap_index);
}

static struct wv_vif_priv *
_wv_mac80211_get_vif_priv (wv_mac80211_t *mac80211, u8 vap_index)
{
  struct ieee80211_vif *ieee80211_vif;

  ieee80211_vif = _wv_mac80211_get_vif (mac80211, vap_index);
  if (NULL == ieee80211_vif)
    return NULL;

  return (struct wv_vif_priv *)ieee80211_vif->drv_priv;
}

static __INLINE BOOL
__wv_mac80211_get_sta_vifs_exist (wv_mac80211_t *mac80211)
{
  MTLK_ASSERT(NULL != mac80211);
  MTLK_ASSERT(NULL != mac80211->radio);

  return wave_radio_get_sta_vifs_exist(mac80211->radio);
}

static mtlk_df_t *
_wv_mac80211_master_df_get (wv_mac80211_t *mac80211)
{
  mtlk_df_t * master_df = NULL;
  if (mac80211 == NULL) {
    ELOG_V("mac80211 == NULL");
    return NULL;
  }

  if (mac80211->radio == NULL) {
    ELOG_V("mac80211->radio == NULL");
    return NULL;
  }

  master_df = wave_radio_df_get(mac80211->radio);
  MTLK_ASSERT(master_df != NULL);
  if (master_df == NULL) {
    ELOG_V("master_df == NULL");
  }

  return master_df;
}

static __INLINE wave_radio_t *
__wv_mac80211_radio_get (wv_mac80211_t *mac80211)
{
  MTLK_ASSERT(NULL != mac80211);
  return mac80211->radio;
}

wave_radio_t * __MTLK_IFUNC
wv_ieee80211_hw_radio_get (struct ieee80211_hw *hw)
{
  return __wv_mac80211_radio_get(__wv_ieee80211_hw_get_mac80211(hw));
}

static __INLINE mtlk_df_t *
__wv_ieee80211_hw_master_df_get (struct ieee80211_hw *hw)
{
  return _wv_mac80211_master_df_get(__wv_ieee80211_hw_get_mac80211(hw));
}

static BOOL
_wv_rcvry_will_not_end (wave_rcvry_type_e rcvry_type)
{
  return (rcvry_type == RCVRY_TYPE_DBG) ||
         (rcvry_type == RCVRY_TYPE_IGNORE) ||
         (rcvry_type == RCVRY_TYPE_UNRECOVARABLE_ERROR) ||
         (rcvry_type == RCVRY_TYPE_DUT);
}

static mtlk_error_t
_wait_rcvry_completion_if_needed (mtlk_core_t *core)
{
  int wait_iter_in_ms = 10;
  int no_of_retries = RCVRY_RETRY_TIMEOUT_MS / wait_iter_in_ms;
  int retry_counter = no_of_retries;

  while (retry_counter && (mtlk_core_rcvry_is_running(core) ||
         wave_rcvry_mac_fatal_pending_get(mtlk_vap_get_hw(core->vap_handle)))) {
    if (_wv_rcvry_will_not_end(wave_rcvry_type_current_get(mtlk_vap_get_hw(core->vap_handle))))
      return MTLK_ERR_NOT_READY;
    if (retry_counter == no_of_retries)
      ILOG0_V("Recovery is running, waiting for process to complete...");
    mtlk_osal_msleep(wait_iter_in_ms);
    retry_counter--;
  }

  if (retry_counter != no_of_retries)
    ILOG0_D("Waited %d ms for recovery procedure to complete", (no_of_retries - retry_counter) * wait_iter_in_ms);

  if (retry_counter)
    return MTLK_ERR_OK;
  else
    return MTLK_ERR_NOT_READY;
}

static BOOL _wv_ieee80211_is_radar_chan (struct ieee80211_channel *chan)
{
  return !!(chan->flags & IEEE80211_CHAN_RADAR);
}

static struct ieee80211_hw *
_wave_vap_ieee80211_hw_get (mtlk_vap_handle_t vap_handle)
{
  wave_radio_t *radio = wave_vap_radio_get(vap_handle);
  return wave_radio_ieee80211_hw_get(radio);
}

struct ieee80211_hw * __MTLK_IFUNC
wave_vap_ieee80211_hw_get (mtlk_vap_handle_t vap_handle)
{
  return _wave_vap_ieee80211_hw_get (vap_handle);
}

void __MTLK_IFUNC
wv_ieee80211_free_txskb (mtlk_vap_handle_t vap_handle, struct sk_buff *skb)
{
  struct ieee80211_hw *mac80211_hw;

  MTLK_ASSERT(NULL != vap_handle);
  MTLK_ASSERT(NULL != skb);

  if (NULL == skb) {
    ELOG_V("skb is NULL");
    return;
  }
  if (NULL == vap_handle) {
    ELOG_V("vap_handle is NULL");
    return;
  }

  mac80211_hw = wave_vap_ieee80211_hw_get(vap_handle);
  MTLK_ASSERT(NULL != mac80211_hw);
  if (NULL == mac80211_hw) {
    ELOG_V("mac80211_hw is NULL");
    return;
  }

  ieee80211_free_txskb(mac80211_hw, skb);
}

static struct _wv_mac80211_t *
_wave_vap_get_mac80211 (mtlk_vap_handle_t vap_handle)
{
  struct ieee80211_hw   *mac80211_hw = wave_vap_ieee80211_hw_get(vap_handle);
  struct _wv_mac80211_t *wv_mac80211 = __wv_ieee80211_hw_get_mac80211(mac80211_hw);

  return wv_mac80211;
}

static struct wv_vif_priv *
_wave_vap_get_vif_priv (mtlk_vap_handle_t vap_handle)
{
  struct _wv_mac80211_t *wv_mac80211  = _wave_vap_get_mac80211(vap_handle);
  return _wv_mac80211_get_vif_priv(wv_mac80211, mtlk_vap_get_id(vap_handle));
}

struct ieee80211_vif * __MTLK_IFUNC
wave_vap_get_vif (mtlk_vap_handle_t vap_handle)
{
  struct _wv_mac80211_t *wv_mac80211  = _wave_vap_get_mac80211(vap_handle);
  return _wv_mac80211_get_vif(wv_mac80211, mtlk_vap_get_id(vap_handle));
}

struct ieee80211_sta * __MTLK_IFUNC
wave_vap_get_sta (mtlk_vap_handle_t vap_handle, u8 * mac)
{
  return ieee80211_find_sta(wave_vap_get_vif(vap_handle), (u8 *) mac);
}

static BOOL _wv_ieee80211_supported_cipher_suite (struct wiphy *wiphy, u32 cipher)
{
  int i;
  for (i = 0; i < wiphy->n_cipher_suites; i++)
    if (cipher == wiphy->cipher_suites[i])
      return TRUE;

  return FALSE;
}

void __MTLK_IFUNC
wv_mac80211_iface_set_initialized (wv_mac80211_t *mac80211, u8 vap_index)
{
  struct wv_vif_priv *wv_iface_inf = _wv_mac80211_get_vif_priv(mac80211, vap_index);

  if (__UNLIKELY(NULL == wv_iface_inf)) {
    return;
  }
  wv_iface_inf->is_initialized = TRUE;
}

BOOL __MTLK_IFUNC
wv_mac80211_iface_get_is_initialized (wv_mac80211_t *mac80211, u8 vap_index)
{
  struct wv_vif_priv *wv_iface_inf = _wv_mac80211_get_vif_priv(mac80211, vap_index);

  if (__UNLIKELY(NULL == wv_iface_inf)) {
    return FALSE;
  }
  return wv_iface_inf->is_initialized;
}

static void _wv_ieee80211_fill_vif_counters (void *data, u8 *mac, struct ieee80211_vif *vif)
{
  struct _wv_vifs_counters *counters = (struct _wv_vifs_counters *)data;

  struct wv_vif_priv *curr_vif_priv = (struct wv_vif_priv *)vif->drv_priv;
  mtlk_vap_handle_t curr_vap_handle;
  mtlk_core_t *curr_core;

  if (curr_vif_priv == NULL)
    return;

  curr_vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(curr_vif_priv->df_user));
  curr_core = mtlk_vap_get_core(curr_vap_handle);

  if (NET_STATE_CONNECTED != mtlk_core_get_net_state(curr_core))
    return; /* Core is not ready */

  counters->num_vifs++;

  if (vif->type == NL80211_IFTYPE_STATION) {
    counters->num_sta_vifs++;

    if (IEEE80211_STA_AUTHORIZED == curr_vif_priv->current_state)
      counters->num_connected_sta_vifs++;
  }
}

BOOL __MTLK_IFUNC
wv_mac80211_has_sta_connections (wv_mac80211_t *mac80211)
{
  struct _wv_vifs_counters counters;

  MTLK_ASSERT(NULL != mac80211);

  memset(&counters, 0, sizeof(counters));
  ieee80211_iterate_interfaces(mac80211->mac80211_hw, IEEE80211_IFACE_ITER_ACTIVE,
                            _wv_ieee80211_fill_vif_counters, &counters);

  return counters.num_connected_sta_vifs > 0;
}

BOOL __MTLK_IFUNC
wv_mac80211_get_sta_vifs_exist (wv_mac80211_t *mac80211)
{
  struct _wv_vifs_counters counters;

  MTLK_ASSERT(NULL != mac80211);

  memset(&counters, 0, sizeof(counters));
  ieee80211_iterate_interfaces(mac80211->mac80211_hw, IEEE80211_IFACE_ITER_ACTIVE,
                            _wv_ieee80211_fill_vif_counters, &counters);

  return counters.num_sta_vifs > 0;
}

static void _wv_ieee80211_op_tx (struct ieee80211_hw *hw,
  struct ieee80211_tx_control *control, struct sk_buff *skb)
{
  struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);
  struct ieee80211_vif *vif = info->control.vif;
  struct ieee80211_hdr *hdr = (void *)skb->data;
  struct ieee80211_conf *conf = &hw->conf;
  struct cfg80211_chan_def *chandef = &conf->chandef;
  struct ieee80211_channel *channel = chandef->chan;
  struct wv_vif_priv *wv_iface_inf;
  mtlk_df_user_t *df_user;
  struct mgmt_tx_params mtp;
  u64 cookie;
  int in_chan;
  BOOL is_sta = FALSE;

  if (!netif_running(skb->dev)) {
    ELOG_V("You should bring interface up first");
    goto drop;
  }

  if (!vif) {
    ELOG_V("DROP skb with no vif");
    goto drop;
  }

  wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  df_user = wv_iface_inf->df_user;
  if (df_user == NULL) {
    ELOG_V("df_user is NULL");
    goto drop;
  }

  /* prevent station from sending assoc\reassoc\probe\beacon responses*/
  is_sta = mtlk_vap_is_sta(mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user)));
  if (is_sta) {
    if (unlikely(ieee80211_is_assoc_resp(hdr->frame_control) ||
        ieee80211_is_reassoc_resp(hdr->frame_control) ||
        ieee80211_is_probe_resp(hdr->frame_control) ||
        ieee80211_is_beacon(hdr->frame_control))) {
      WLOG_V("STA can't send assoc resp, probe resp or beacon");
      goto drop;
    }
  }

  in_chan = ieee80211_frequency_to_channel(channel->center_freq);
  memset(&mtp, 0, sizeof(mtp));

  if (ieee80211_is_nullfunc(hdr->frame_control) || ieee80211_is_qos_nullfunc(hdr->frame_control))
    mtp.extra_processing = PROCESS_NULL_DATA_PACKET;
  else
    mtp.extra_processing = PROCESS_MANAGEMENT;

  /* EAPOL frames */
  if (info->control.flags & IEEE80211_TX_CTRL_PORT_CTRL_PROTO) {
    enum nl80211_iftype type = NL80211_IFTYPE_UNSPECIFIED;
    /* ieee80211_data_to_8023 is designed to be used in RX direction,
     * but here we use in in TX path, as FW requires ETH header.
     * To not re-create this function from scratch, just provide
     * opsosite interface type which does the trick */
    if (NL80211_IFTYPE_AP == vif->type)
      type = NL80211_IFTYPE_STATION;
    else if (NL80211_IFTYPE_STATION == vif->type)
      type = NL80211_IFTYPE_AP;

    if (ieee80211_data_to_8023(skb, vif->addr, type)) {
      ELOG_V("Header conversion for EAPOL TX frame failed");
      goto drop;
    }

    ILOG2_V("TX EAPOL frame");

    mtp.extra_processing = PROCESS_EAPOL;
  }

  mtp.buf = skb->data;
  mtp.len = skb->len;
  mtp.channum = in_chan;
  mtp.cookie = &cookie;
  mtp.no_cck = !!(info->flags & IEEE80211_TX_CTL_NO_CCK_RATE);
  mtp.dont_wait_for_ack = !!(info->flags & IEEE80211_TX_CTL_NO_ACK);
  mtp.skb = skb;

  _mtlk_df_user_invoke_core_async(mtlk_df_user_get_df(df_user),
       WAVE_CORE_REQ_MGMT_TX, &mtp, sizeof(mtp), NULL, 0);
  return;

drop:
  ieee80211_free_txskb(hw, skb);
}

static void _wv_ieee80211_op_data_tx (struct ieee80211_hw *hw,
  struct ieee80211_vif *vif, struct sk_buff *skb)
{
  mtlk_df_user_data_tx(((struct wv_vif_priv *)vif->drv_priv)->df_user, skb);
}

static void _wv_ieee80211_op_stop (struct ieee80211_hw *hw)
{
  wv_mac80211_t *mac80211;

  ILOG0_S("%s: Last interface was stopped", wiphy_name(hw->wiphy));
  mac80211 = (wv_mac80211_t *)hw->priv;
  MTLK_ASSERT(mac80211 != NULL);
  if (mac80211 == NULL) {
    ELOG_V("Interface stop, invalid mac80211 == NULL");
    return;
  }

  if (!mtlk_osal_timer_is_stopped(&mac80211->csa_timer))
    mtlk_osal_timer_cancel_sync(&mac80211->csa_timer);

  if (mac80211->vif_array){
    mtlk_osal_mem_free(mac80211->vif_array);
    mac80211->vif_array = NULL;
    mac80211->vif_array_size = 0;
  }
}

static enum mtlk_sb_dfs_bw
_mtlk_nl_to_mtlk_sbdfs_bw (enum nl80211_sb_dfs_bw bw)
{
  switch (bw) {
  case NL80211_SB_DFS_BW_20:
    return MTLK_SB_DFS_BW_20;
  case NL80211_SB_DFS_BW_40:
    return MTLK_SB_DFS_BW_40;
  case NL80211_SB_DFS_BW_80:
    return MTLK_SB_DFS_BW_80;
  default:
    return MTLK_SB_DFS_BW_NORMAL;
  }
}

static mtlk_error_t
_wv_activate_vif (mtlk_df_user_t *df_user)
{
  mtlk_clpb_t *clpb = NULL;
  mtlk_error_t res = MTLK_ERR_OK;

  /* Set security is currently not done, using open mode for now */

  ILOG2_S("%s: Switching to serializer to activate the Vif", mtlk_df_user_get_name(df_user));
  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
      WAVE_CORE_REQ_ACTIVATE_OPEN, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval(res, clpb,
      WAVE_CORE_REQ_ACTIVATE_OPEN, TRUE);

  return res;
}

#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_RELEASE_DLEVEL)
static const char *nl_band_to_str[] = { "2GHz", "5GHz", "60GHz", "6GHz", NULL};
#endif

static int
_wv_ieee80211_op_config (struct ieee80211_hw *hw, u32 changed)
{
  struct ieee80211_conf     *conf = &hw->conf;
  struct cfg80211_chan_def  *chandef = &conf->chandef;
   mtlk_clpb_t *clpb = NULL;
  struct set_chan_param_data cpd;
  wv_mac80211_t     *mac80211 = __wv_ieee80211_hw_get_mac80211(hw);
  mtlk_vap_handle_t  master_vap_handle;
  mtlk_df_t         *master_df;
  mtlk_core_t       *master_core;
  mtlk_scan_support_t *scan_support;
  mtlk_sb_dfs_params_t *sb_dfs;
  mtlk_error_t res = MTLK_ERR_OK;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_INFO_DLEVEL)
  struct ieee80211_channel  *channel = chandef->chan;
  const char *band = nl_band_to_str[channel->band];
#endif
  WAVE_WV_CHECK_PTR_RES(mac80211);

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_RES(master_df);

  ILOG1_SD("%s: mac80211 config callback. changed=0x%x",
           wiphy_name(hw->wiphy), changed);

  master_vap_handle = mtlk_df_get_vap_handle(master_df);
  master_core = mtlk_vap_get_core(master_vap_handle);
  scan_support =
      mtlk_core_get_scan_support(mtlk_vap_get_core(master_vap_handle));

  if (_wait_rcvry_completion_if_needed(master_core) != MTLK_ERR_OK) {
    ELOG_S("%s: Recovery is running, can't change configuration",
           wiphy_name(hw->wiphy));
    return -EBUSY;
  }

  if (changed & IEEE80211_CONF_CHANGE_IDLE) {
    ILOG1_SD("%s: Interface idle state has changed to %d",
             wiphy_name(hw->wiphy), !!(conf->flags & IEEE80211_CONF_IDLE));
  }

  if (changed & IEEE80211_CONF_CHANGE_POWER)
    ILOG0_SDDD("%s: Power level change, power_level=%d, max_power=%d, max_reg_power=%d",
             wiphy_name(hw->wiphy), conf->power_level, channel->max_power,
             channel->max_reg_power);

  if (changed & IEEE80211_CONF_CHANGE_CHANNEL) {
    ILOG0_SSDDDDD("%s: Channel config change. band=%s, freq=%d, "
                  "center_freq1=%d, center_freq2=%d, is_scan_running=%i, width=%d",
                  wiphy_name(hw->wiphy),
                  band,
                  channel->center_freq,
                  chandef->center_freq1,
                  chandef->center_freq2,
                  is_scan_running(scan_support),
                  mtlkcw2cw(nlcw2mtlkcw(chandef->width)));
  }

  if ((changed & IEEE80211_CONF_CHANGE_CHANNEL) || (changed & IEEE80211_CONF_CHANGE_POWER)){

    memset(&cpd, 0, sizeof(cpd));
    sb_dfs = &cpd.chandef.sb_dfs;
    wave_radio_chandef_copy(&cpd.chandef, chandef);
    cpd.vap_id = mtlk_vap_get_id(master_vap_handle);

    /* In case AP master VAP is active allow time for AP mode VAP(s)
     * to send CSAs to associated stations */
    if (is_mac_scan_running(scan_support)) {
      if (mtlk_core_is_chandef_identical(&cpd.chandef, &scan_support->orig_chandef)) {
        cpd.switch_type = ST_NORMAL;
        ILOG1_S("%s: during mac scan - back to original channel",
                wiphy_name(hw->wiphy));
      } else {
        cpd.switch_type = ST_SCAN;
        ILOG1_S("%s: during mac scan - setting SCAN switch type",
                wiphy_name(hw->wiphy));
      }
    } else {
      cpd.switch_type = ST_NORMAL;
      ILOG1_V("Not using CSA in channel switch");
    }

    cpd.chandef.power_level = conf->power_level;
    cpd.block_tx_pre = TRUE;
    cpd.block_tx_post = FALSE; /* TRUE means waiting for radars */

    sb_dfs->sb_dfs_bw = _mtlk_nl_to_mtlk_sbdfs_bw(chandef->sb_dfs.sub_width);
    if (__mtlk_is_sb_dfs_switch(sb_dfs->sb_dfs_bw)) {
      ILOG1_SD("%s: Sub band DFS channel switch, new bandwidth %d",
               wiphy_name(hw->wiphy), sb_dfs->sb_dfs_bw);
      sb_dfs->center_freq = chandef->sb_dfs.center_freq;
      sb_dfs->width = nlcw2mtlkcw(chandef->sb_dfs.width);
    }

    res = _mtlk_df_user_invoke_core(master_df, WAVE_RADIO_REQ_SET_CHAN, &clpb, &cpd, sizeof(cpd));
    res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_RADIO_REQ_SET_CHAN, TRUE);
    if (res != MTLK_ERR_OK)
      ELOG_S("%s: wv_ieee80211_config - error setting channel",
             wiphy_name(hw->wiphy));
  }

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

#define WV_CSA_TIMEOUT 500 /* ms */

static BOOL
_wv_all_vifs_have_completed_csa (wv_mac80211_t *mac80211)
{
  int i, active_vif_count = 0;
  int csa_finish_report_pending_cnt = 0;
  struct wv_vif_priv *wv_iface_inf;

  for (i = 0; i < mac80211->vif_array_size; i++) {
    if (!mac80211->vif_array[i])
      continue;

    if ((NL80211_IFTYPE_AP != mac80211->vif_array[i]->type) &&
        (NL80211_IFTYPE_STATION != mac80211->vif_array[i]->type))
      continue;

    wv_iface_inf = (struct wv_vif_priv*)mac80211->vif_array[i]->drv_priv;
    if (!wv_iface_inf || !wv_iface_inf->is_initialized)
      continue;

    active_vif_count++;
    if (wv_iface_inf->csa_status)
      csa_finish_report_pending_cnt++;
  }

  if (0 == active_vif_count)
    return FALSE;

  if (active_vif_count != csa_finish_report_pending_cnt) {
    /* Set timer as protection: if not all VIFs did CSA in timer
     * interval, do csa_finish for VIFs who did */
    if (mtlk_osal_timer_is_stopped(&mac80211->csa_timer)) {
      ILOG2_D("Setting CSA timer for %d msec", WV_CSA_TIMEOUT);
      mtlk_osal_timer_set(&mac80211->csa_timer, WV_CSA_TIMEOUT);
    }

    ILOG2_DD("Not all VAPs (%d), has completed (%d) CSA yet",
             active_vif_count, csa_finish_report_pending_cnt);
    return FALSE;
  }

  mtlk_osal_timer_cancel_sync(&mac80211->csa_timer);
  return TRUE;
}

static void
_wv_all_vifs_csa_finish (wv_mac80211_t *mac80211)
{
  int i;
  struct wv_vif_priv *wv_iface_inf;

  for (i = 0; i < mac80211->vif_array_size; i++) {
    if (!mac80211->vif_array[i])
      continue;

    wv_iface_inf = (struct wv_vif_priv*)mac80211->vif_array[i]->drv_priv;
    if (!wv_iface_inf->is_initialized)
      continue;

    if (!mac80211->vif_array[i]->csa_active) {
      WLOG_S("%s: CSA not active!", ieee80211_vif_to_name(mac80211->vif_array[i]));
      wv_iface_inf->csa_status = WV_CSA_NOT_DONE;
      continue;
    }

    ILOG2_S("%s: Calling CSA/Chswitch finish", ieee80211_vif_to_name(mac80211->vif_array[i]));

    if (NL80211_IFTYPE_AP == mac80211->vif_array[i]->type)
      ieee80211_csa_finish(mac80211->vif_array[i]);
    else if (NL80211_IFTYPE_STATION == mac80211->vif_array[i]->type)
      ieee80211_chswitch_done(mac80211->vif_array[i],
                              wv_iface_inf->csa_status == WV_CSA_SUCCESS);

    wv_iface_inf->csa_status = WV_CSA_NOT_DONE;
  }

  ILOG0_V("CSA Completed");
}

/* AP interface initiated channel switch has been completed */
static void _wv_csa_completed (mtlk_handle_t user_context,
                               int           processing_result,
                               mtlk_clpb_t  *pclpb)
{
  struct _wv_chswitch_ctx *chswitch_ctx =
      (struct _wv_chswitch_ctx *)user_context;
  mtlk_error_t res = MTLK_ERR_OK;
  struct ieee80211_hw *hw = chswitch_ctx->hw;
  wv_mac80211_t *mac80211 = (wv_mac80211_t *)hw->priv;
  struct ieee80211_vif *vif = chswitch_ctx->vif;
  struct mtlk_chan_def *ccd;
  struct wv_vif_priv *wv_iface_inf;

  ccd = __wv_mac80211_chandef_get(mac80211);
  if (ccd == NULL) {
    ELOG_V("Invalid parameters");
    goto end;
  }

  res = _mtlk_df_user_process_core_retval_void(processing_result, pclpb,
                                          WAVE_RADIO_REQ_SET_CHAN, FALSE);

  if (res == MTLK_ERR_OK) {
    uint32 size;
    int *data = mtlk_clpb_enum_get_next(pclpb, &size);
    MTLK_CLPB_TRY(data, size)
    {
      res = *data;
    }
    MTLK_CLPB_FINALLY(res) {
      ILOG1_SD("%s: Channel switch completed, result=%d",
               ieee80211_vif_to_name(vif), res);

      if (res != MTLK_ERR_OK)
        ELOG_S("%s: Error setting channel", ieee80211_vif_to_name(vif));
    }
    MTLK_CLPB_END;
  }

  wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
  wv_iface_inf->csa_status = WV_CSA_SUCCESS;

  if (_wv_all_vifs_have_completed_csa(mac80211))
    _wv_all_vifs_csa_finish(mac80211);

end:
  mtlk_osal_mem_free(chswitch_ctx);
}

/* Called just before channel switch for AP and STA interfaces */
static int
_wv_ieee80211_op_pre_channel_switch (struct ieee80211_hw *hw,
                                     struct ieee80211_vif *vif,
                                     struct ieee80211_channel_switch *ch_switch)
{
  struct ieee80211_conf     *conf = &hw->conf;
  struct cfg80211_chan_def *chandef = &ch_switch->chandef;
  struct set_chan_param_data cpd;
  mtlk_df_t *master_df;
  mtlk_vap_handle_t master_vap_handle;
  mtlk_scan_support_t *scan_support;
  int beacon_period;
  mtlk_core_t *master_core;
  struct _wv_chswitch_ctx *chswitch_ctx;
  mtlk_sb_dfs_params_t *sb_dfs;
  mtlk_error_t res = MTLK_ERR_OK;
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= 1)
  struct ieee80211_channel  *channel = chandef->chan;
  const char *band = nl_band_to_str[channel->band];
#endif
  WAVE_WV_CHECK_PTR_RES(wv_iface_inf);

  if (vif->type ==  NL80211_IFTYPE_STATION) {
    /* change state of the channel to DFS AVAILIABLE so that AP vifs will be
     * able to follow the station if the far AP made a CSA to a DFS channel.
     * otherwise AP vifs need to do CAC on the channel
     */
    cfg80211_set_dfs_state(hw->wiphy, chandef, NL80211_DFS_AVAILABLE);

    ILOG1_S("%s: STA CSA will be handled in ieee80211_channel_switch.",
            ieee80211_vif_to_name(vif));
    return LINUX_ERR_OK;
  }

  ILOG1_SSD("%s: Invoked from %s (%i)", ieee80211_vif_to_name(vif),
            current->comm, current->pid);

  MTLK_ASSERT(hw->wiphy != NULL);

  master_df = __wv_ieee80211_hw_master_df_get(hw);
  if (master_df == NULL) {
    res = MTLK_ERR_PARAMS;
    goto end;
  }

  ILOG1_SSDDDD("%s: Ch switch settings: band=%s, freq=%d, "
               "center_freq1=%d, center_freq2=%d, TBTT=%d",
               ieee80211_vif_to_name(vif),
               band,
               channel->center_freq,
               chandef->center_freq1,
               chandef->center_freq2,
               ch_switch->count);

  master_vap_handle = mtlk_df_get_vap_handle(master_df);
  master_core = mtlk_vap_get_core(master_vap_handle);

  memset(&cpd, 0, sizeof(cpd));
  sb_dfs = &cpd.chandef.sb_dfs;
  wave_radio_chandef_copy(&cpd.chandef, chandef);
  cpd.chandef.power_level = conf->power_level;
  cpd.vap_id = wv_iface_inf->vap_index;

  beacon_period = vif->bss_conf.beacon_int;

  scan_support =
      mtlk_core_get_scan_support(mtlk_vap_get_core(master_vap_handle));

  /* FW does not accept CSA type with 0 TBTT count */
  if (ch_switch->count)
    cpd.switch_type = ST_CSA;
  else
    cpd.switch_type = ST_NORMAL;

#ifdef CONFIG_WAVE_DEBUG
  if (scan_support->dfs_debug_params.beacon_count > 0) {
    cpd.chan_switch_time =
        scan_support->dfs_debug_params.beacon_count * beacon_period;
    ILOG1_D("Using debug CSA Beacon count: %d",
        scan_support->dfs_debug_params.beacon_count);
  } else {
    if (ch_switch->count != 0){
      cpd.chan_switch_time = ch_switch->count * beacon_period;
    }
  }
#else
  if (ch_switch->count != 0){
    cpd.chan_switch_time = ch_switch->count * beacon_period;
  }
#endif

  cpd.block_tx_pre = ch_switch->block_tx; /* hostapd always sets this to 1, BTW */
  /* this is in alignment with wave_radio_channel_switch from cfg80211 AP channel_switch driver's operation.
     it had the following comment next to it :
     TRUE is meant for waiting for radars, but hostapd has ensured it's switching to a safe range */
  cpd.block_tx_post = FALSE;

  sb_dfs->sb_dfs_bw = _mtlk_nl_to_mtlk_sbdfs_bw(chandef->sb_dfs.sub_width);
  if (__mtlk_is_sb_dfs_switch(sb_dfs->sb_dfs_bw)) {
    ILOG1_SD("%s: Sub band DFS channel switch, new bandwidth %d",
             ieee80211_vif_to_name(vif), sb_dfs->sb_dfs_bw);
    sb_dfs->center_freq = chandef->sb_dfs.center_freq;
    sb_dfs->width = nlcw2mtlkcw(chandef->sb_dfs.width);
  }

  /* If using debug radar simulation, we can switch to non safe (radar) range, so block_tx_post should be set */
  if (scan_support->dfs_debug_params.debug_chan &&
      scan_support->dfs_debug_params.switch_in_progress) {
    uint32 cur_freq;
    struct ieee80211_channel *c;
    uint32 low_freq =
        freq2lowfreq(chandef->center_freq1, nlcw2mtlkcw(chandef->width));
    uint32 high_freq =
        freq2highfreq(chandef->center_freq1, nlcw2mtlkcw(chandef->width));

    for (cur_freq = low_freq; cur_freq <= high_freq; cur_freq += 20) {
      c = ieee80211_get_channel(hw->wiphy, cur_freq);

      if (!c || !(c->flags & IEEE80211_CHAN_RADAR))
        continue;

      /* We did set it as AVAILABLE on radar event */
      if (c->dfs_state == NL80211_DFS_AVAILABLE) {
        cpd.block_tx_post = TRUE;
        break;
      }
    }
  }

  chswitch_ctx = (struct _wv_chswitch_ctx *)mtlk_osal_mem_alloc(sizeof
      (struct _wv_chswitch_ctx), MTLK_MEM_TAG_MAC80211);
  if (NULL == chswitch_ctx)
    return -ENOMEM;
  chswitch_ctx->vif = vif;
  chswitch_ctx->hw = hw;
  chswitch_ctx->ch_switch = *ch_switch;

  _mtlk_df_user_invoke_core_async(master_df, WAVE_RADIO_REQ_SET_CHAN, &cpd,
                                  sizeof(cpd), _wv_csa_completed,
                                  (mtlk_handle_t) chswitch_ctx);

end:

 /* ieee80211_csa_finish will be called from _wv_csa_completed callback */

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _wv_ieee80211_op_post_channel_switch(struct ieee80211_hw *hw,
                                                struct ieee80211_vif *vif)
{
  wv_mac80211_t *mac80211;
  mtlk_df_t *master_df;
  mtlk_core_t *master_core;
  struct ieee80211_conf *conf = &hw->conf;
  struct cfg80211_chan_def *chandef = &conf->chandef;
  mtlk_sb_dfs_params_t *sb_dfs;
  mtlk_clpb_t *clpb = NULL;
  struct set_chan_param_data cpd;
  mtlk_error_t res = MTLK_ERR_OK;

  mac80211 = __wv_ieee80211_hw_get_mac80211(hw);
  WAVE_WV_CHECK_PTR_RES(mac80211);

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_RES(master_df);

  master_core = mtlk_vap_get_core(mtlk_df_get_vap_handle(master_df));

  ILOG1_S("%s: Post channel switch", ieee80211_vif_to_name(vif));

  if (master_core->slow_ctx->is_block_tx == TRUE) {
    /* (Station mode feature) We will exit this state later once we'll see a beacon. */
    goto end;
  }

  /*
   * If we Performed CSA to same channel and BW, mac80211 will not call us with op_config().
   * To exit ST_CSA state in FW & driver, finish the operation by calling WAVE_RADIO_REQ_SET_CHAN
   * again with ST_NORMAL params.
   *
   * If the CSA was to a different channel or BW, WAVE_RADIO_REQ_SET_CHAN will exit
   * immediately due to cpd being identical to ccd.
   */

  memset(&cpd, 0, sizeof(cpd));
  sb_dfs = &cpd.chandef.sb_dfs;
  wave_radio_chandef_copy(&cpd.chandef, chandef);
  cpd.chandef.power_level = conf->power_level;
  sb_dfs->sb_dfs_bw = _mtlk_nl_to_mtlk_sbdfs_bw(chandef->sb_dfs.sub_width);
  if (__mtlk_is_sb_dfs_switch(sb_dfs->sb_dfs_bw)) {
    ILOG1_SD("%s: Sub band DFS channel switch, new bandwidth %d",
              wiphy_name(hw->wiphy), sb_dfs->sb_dfs_bw);
    sb_dfs->center_freq = chandef->sb_dfs.center_freq;
    sb_dfs->width = nlcw2mtlkcw(chandef->sb_dfs.width);
  }

  res = _mtlk_df_user_invoke_core(master_df, WAVE_RADIO_REQ_SET_CHAN, &clpb, &cpd, sizeof(cpd));
  res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_RADIO_REQ_SET_CHAN, TRUE);
  if (res != MTLK_ERR_OK)
    ELOG_S("%s: wv_ieee80211_config - error setting channel", wiphy_name(hw->wiphy));

end:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

/* STA channel switch following peer AP CSA has been completed */
static void _wv_sta_ch_switch_completed (mtlk_handle_t user_context,
                                         int           processing_result,
                                         mtlk_clpb_t  *pclpb)
{
  struct _wv_chswitch_ctx *chswitch_ctx =
      (struct _wv_chswitch_ctx *)user_context;
  mtlk_error_t res = MTLK_ERR_OK;
  struct ieee80211_hw *hw = chswitch_ctx->hw;
  wv_mac80211_t *mac80211 = (wv_mac80211_t *)hw->priv;
  struct ieee80211_vif *vif = chswitch_ctx->vif;
  struct cfg80211_chan_def *chandef = &chswitch_ctx->ch_switch.chandef;
  struct ieee80211_channel *channel = chandef->chan;
  struct mtlk_chan_def *ccd;
  mtlk_df_t *master_df;
  mtlk_vap_handle_t master_vap_handle;
  mtlk_core_t *master_core;
  struct wv_vif_priv *wv_iface_inf;

  ccd = __wv_mac80211_chandef_get(mac80211);
  master_df = __wv_ieee80211_hw_master_df_get(hw);
  if (ccd == NULL || master_df == NULL) {
    ELOG_V("Invalid parameters");
    goto end;
  }

  master_vap_handle = mtlk_df_get_vap_handle(master_df);
  master_core = mtlk_vap_get_core(master_vap_handle);

  res = _mtlk_df_user_process_core_retval_void(processing_result, pclpb,
                                          WAVE_RADIO_REQ_SET_CHAN, FALSE);

  if (res == MTLK_ERR_OK) {
    uint32 size;
    int *data = mtlk_clpb_enum_get_next(pclpb, &size);

    MTLK_CLPB_TRY(data, size)
    {
      res = *data;
    }
    MTLK_CLPB_FINALLY(res) {
      ILOG1_SD("%s: Channel switch completed, result=%d",
               ieee80211_vif_to_name(vif), res);

      if (res != MTLK_ERR_OK){
        if (ccd->wait_for_beacon == FALSE &&
            master_core->slow_ctx->is_block_tx == TRUE) {
          master_core->slow_ctx->is_block_tx = FALSE;
        }
        ELOG_S("%s: Error setting channel", ieee80211_vif_to_name(vif));
      } else {
        if (_wv_ieee80211_is_radar_chan(channel)) {
          ILOG0_S("%s: Channel switched to DFS channel, blocking tx until a beacon is found",
                  ieee80211_vif_to_name(vif));
          master_core->slow_ctx->is_block_tx = TRUE;
          ccd->wait_for_beacon = TRUE;
        }
      }
    }
    MTLK_CLPB_END;
  }

  wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
  if (MTLK_ERR_OK == res)
    wv_iface_inf->csa_status = WV_CSA_SUCCESS;
  else
    wv_iface_inf->csa_status = WV_CSA_FAILED;

  if (_wv_all_vifs_have_completed_csa(mac80211))
    _wv_all_vifs_csa_finish(mac80211);

end:
  mtlk_osal_mem_free(chswitch_ctx);
}

/* Called for STA interface getting a CSA from peer AP */
static void _wv_ieee80211_op_channel_switch (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif,
    struct ieee80211_channel_switch *ch_switch)
{
  wv_mac80211_t            *mac80211 = __wv_ieee80211_hw_get_mac80211(hw);
  struct ieee80211_conf    *conf = &hw->conf;
  struct cfg80211_chan_def *chandef = &ch_switch->chandef;
  struct ieee80211_channel *channel = chandef->chan;
  struct mtlk_chan_def     *ccd;
  struct set_chan_param_data cpd;
  mtlk_df_t *master_df;
  mtlk_vap_handle_t master_vap_handle;
  mtlk_scan_support_t *scan_support;
  int beacon_period;
  mtlk_core_t *master_core;
  struct _wv_chswitch_ctx *chswitch_ctx;
#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_INFO_DLEVEL)
  const char *band = nl_band_to_str[channel->band];
#endif
  ILOG0_SSDDDD("%s: Sta vif got CSA from AP. CSA settings: band=%s, freq=%d, "
               "center_freq1=%d, center_freq2=%d, TBTTs %u",
               ieee80211_vif_to_name(vif),
               band,
               channel->center_freq,
               chandef->center_freq1,
               chandef->center_freq2,
               ch_switch->count);

  MTLK_ASSERT(hw->wiphy != NULL);

  /* Can't to continue if mac80211 is NULL */
  WAVE_WV_CHECK_PTR_VOID(mac80211);

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_VOID(master_df);

  ccd = __wv_mac80211_chandef_get(mac80211);
  WAVE_WV_CHECK_PTR_VOID(ccd);

  master_vap_handle = mtlk_df_get_vap_handle(master_df);
  master_core = mtlk_vap_get_core(master_vap_handle);
  WAVE_WV_CHECK_PTR_VOID(master_core);

  memset(&cpd, 0, sizeof(cpd));
  wave_radio_chandef_copy(&cpd.chandef, chandef);
  cpd.chandef.power_level = conf->power_level;

  cpd.vap_id = mtlk_vap_get_id(master_vap_handle);

  beacon_period = vif->bss_conf.beacon_int;

  scan_support =
      mtlk_core_get_scan_support(mtlk_vap_get_core(master_vap_handle));

#ifdef CONFIG_WAVE_DEBUG
  if (scan_support->dfs_debug_params.beacon_count > 0) {
    cpd.switch_type = ST_CSA;
    cpd.chan_switch_time =
        scan_support->dfs_debug_params.beacon_count * beacon_period;
    ILOG1_D("Using debug CSA Beacon count: %d",
        scan_support->dfs_debug_params.beacon_count);
  } else {
    if (ch_switch->count != 0){
      cpd.switch_type = ST_CSA;
      cpd.chan_switch_time = ch_switch->count * beacon_period;
    }
  }
#else
  if (ch_switch->count != 0){
    cpd.switch_type = ST_CSA;
    cpd.chan_switch_time = ch_switch->count * beacon_period;
  }
#endif

  cpd.block_tx_pre = ch_switch->block_tx;

  /* check if we are moving to DFS channel, if we are we are stoping all TX after changing channel untill we will see a beacon */
  if (_wv_ieee80211_is_radar_chan(channel)) {
    cpd.switch_type = ST_CSA;
    cpd.block_tx_post = TRUE;
    ILOG0_S("%s: Received CSA from far AP to a DFS channel, blocking TX",
            ieee80211_vif_to_name(vif));
    /* CSA will probably take a while. wait_for_beacon is false, since we don't
     * want the beacons just before the actual channel switch to un-block tx. */
    ccd->wait_for_beacon = FALSE;
    master_core->slow_ctx->is_block_tx = TRUE;
  } else {
    cpd.block_tx_post = FALSE; /* TRUE means waiting for radars */
  }

  chswitch_ctx = (struct _wv_chswitch_ctx *)mtlk_osal_mem_alloc(sizeof
      (struct _wv_chswitch_ctx), MTLK_MEM_TAG_MAC80211);
  if (NULL == chswitch_ctx) {
    ELOG_V("Memory allocation failed!");
    return;
  }
  chswitch_ctx->vif = vif;
  chswitch_ctx->hw = hw;
  chswitch_ctx->ch_switch = *ch_switch;

  _mtlk_df_user_invoke_core_async(master_df, WAVE_RADIO_REQ_SET_CHAN, &cpd,
                                  sizeof(cpd), _wv_sta_ch_switch_completed,
                                  (mtlk_handle_t) chswitch_ctx);
}

static int _wv_ieee80211_op_get_txpower (struct ieee80211_hw *hw,
                                         struct ieee80211_vif *vif,
                                         int *dbm)
{
  mtlk_df_t *master_df;
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;
  wv_mac80211_t * mac80211 = (wv_mac80211_t *)hw->priv;
  if (0 == mtlk_vap_manager_get_active_vaps_number(wave_radio_vap_manager_get(mac80211->radio))){
    ILOG2_S("%s: Can't get tx power when no vaps are activated", ieee80211_vif_to_name(vif));
    return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_NOT_READY);
  }
  master_df = __wv_ieee80211_hw_master_df_get(hw);

  if (master_df == NULL) {
    ELOG_V("Error getting master_df");
    return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_PARAMS);
  }
  res = _mtlk_df_user_invoke_core(master_df,
        WAVE_CORE_REQ_GET_TX_POWER, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval(res, clpb,
        WAVE_CORE_REQ_GET_TX_POWER, FALSE);

  if (MTLK_ERR_OK == res) {
    mtlk_tx_power_cfg_t *tx_power_cfg;
    uint32 power_cfg_size;

    tx_power_cfg = mtlk_clpb_enum_get_next(clpb, &power_cfg_size);
    MTLK_CLPB_TRY(tx_power_cfg, power_cfg_size)
    *dbm = tx_power_cfg->tx_power;
    MTLK_CLPB_FINALLY(res)
      mtlk_clpb_delete(clpb); /* already deleted in error cases */
    MTLK_CLPB_END
  }

  return _mtlk_df_mtlk_to_linux_error_code(res);

}

static int _wv_ieee80211_op_start (struct ieee80211_hw *hw)
{
  wv_mac80211_t *mac80211;
  mtlk_df_t *master_df;
  int max_vaps_count = 0;

  ILOG0_S("%s: First interface is starting", wiphy_name(hw->wiphy));
  mac80211 = (wv_mac80211_t *)hw->priv;
  MTLK_ASSERT(mac80211 != NULL);
  if (mac80211 == NULL) {
    ELOG_S("%s: Interface start, invalid mac80211 == NULL",
           wiphy_name(hw->wiphy));
    return -EINVAL;
  }
  if (!mac80211->registered) {
     ELOG_S("%s: Interface start, invalid mac80211 - not registered",
           wiphy_name(hw->wiphy));
    return -EINVAL;
  }

  master_df = _wv_mac80211_master_df_get(mac80211);
  MTLK_ASSERT(master_df != NULL);
  if (master_df == NULL) {
    ILOG0_S("%s: Interface start, no master VAP is set yet, do not continue "
            "with ch configuration", wiphy_name(hw->wiphy));
    return LINUX_ERR_OK;
  }

  max_vaps_count = __wv_mac80211_max_vaps_count_get(mac80211);
  mac80211->vif_array = (struct ieee80211_vif **)mtlk_osal_mem_alloc((sizeof(struct ieee80211_vif *) * max_vaps_count), MTLK_MEM_TAG_MAC80211);
  if (NULL == mac80211->vif_array) {
    return -ENOMEM;
  }
  memset(mac80211->vif_array, 0, sizeof(struct ieee80211_vif *) * max_vaps_count);
  mac80211->vif_array_size = max_vaps_count;

  return LINUX_ERR_OK;
}


static mtlk_error_t
_wv_request_sid (mtlk_df_user_t *df_user, u16 *vif_sid, const uint8 *mac_addr)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
      WAVE_CORE_REQ_REQUEST_SID, &clpb, mac_addr, IEEE_ADDR_LEN);
  res = _mtlk_df_user_process_core_retval(res, clpb,
      WAVE_CORE_REQ_REQUEST_SID, FALSE);

  if (MTLK_ERR_OK == res) {
    u16 *sid;
    uint32 sid_size;

    sid = mtlk_clpb_enum_get_next(clpb, &sid_size);
    MTLK_CLPB_TRY(sid, sid_size)
      *vif_sid = *sid;
    MTLK_CLPB_FINALLY(res)
      mtlk_clpb_delete(clpb); /* already deleted in error cases */
    MTLK_CLPB_END
  }

  return res;
}

static mtlk_error_t
_wv_release_sid (mtlk_df_user_t *df_user, u16 vif_sid)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
      WAVE_CORE_REQ_REMOVE_SID, &clpb, &vif_sid, sizeof(vif_sid));
  res = _mtlk_df_user_process_core_retval(res, clpb,
      WAVE_CORE_REQ_REMOVE_SID, TRUE);

  return res;
}

/* Saves the mac addr from vif into pdb
 * Must be called after vap init & before vap activate
 */
static mtlk_error_t
_wv_set_mac_addr_pdb (mtlk_df_user_t *df_user, const char *mac_adr)
{
  mtlk_clpb_t *clpb = NULL;
  mtlk_error_t res = MTLK_ERR_BUSY;

  MTLK_ASSERT(NULL != df_user);
  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
      WAVE_CORE_REQ_SET_MAC_ADDR, &clpb, mac_adr, ETH_ALEN);
  res = _mtlk_df_user_process_core_retval_void(res, clpb,
      WAVE_CORE_REQ_SET_MAC_ADDR, TRUE);

  return res;
}

mtlk_df_user_t * __MTLK_IFUNC
wv_ieee80211_ndev_to_dfuser (struct net_device *ndev)
{
  struct ieee80211_vif *vif;
  struct wv_vif_priv *wv_iface_inf;

  MTLK_ASSERT(NULL != ndev);

  vif = net_device_to_ieee80211_vif(ndev);
  if (__UNLIKELY(vif == NULL)) {
    ELOG_V("Error getting mac80211 vif");
    return NULL;
  }
  wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
  MTLK_ASSERT(wv_iface_inf != NULL);
  if (__UNLIKELY(wv_iface_inf == NULL)) {
    ELOG_V("Error getting df_user");
    return NULL;
  }
  return wv_iface_inf->df_user;
}

mtlk_df_user_t *__MTLK_IFUNC
wv_ieee80211_vif_to_dfuser (struct ieee80211_vif *vif)
{
  struct wv_vif_priv *wv_iface_inf;

  if (__UNLIKELY(!vif))
    return NULL;

  wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
  MTLK_ASSERT(wv_iface_inf != NULL);
  if (__UNLIKELY(wv_iface_inf == NULL)) {
    ELOG_S("%s: Error getting df_user", ieee80211_vif_to_name(vif));
    return NULL;
  }
  return wv_iface_inf->df_user;
}

struct nic *wv_ieee80211_vif_to_core (struct ieee80211_vif *vif)
{
  mtlk_df_user_t *df_user;
  mtlk_vap_handle_t vap_handle;

  df_user = wv_ieee80211_vif_to_dfuser(vif);
  if (!df_user)
    return NULL;

  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  return mtlk_vap_get_core(vap_handle);
}

static int _wv_ieee80211_op_add_interface (struct ieee80211_hw *hw, struct ieee80211_vif *vif)
{
  struct wv_vif_priv* wv_iface_inf;
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;
  mtlk_mbss_cfg_t mbss_cfg;
  mtlk_df_user_t **clpb_data;
  uint32 clpb_data_size;
  wv_mac80211_t *mac80211;
  mtlk_df_t      *master_df;
  mtlk_core_t *master_core;
  struct net_device *netdev_p = NULL;
  wave_radio_t *radio;

  ILOG1_SSD("%s: Invoked from %s (%i)", ieee80211_vif_to_name(vif),
            current->comm, current->pid);

  RLOG_SY("Adding interface %s, mac addr: %Y. ",
          ieee80211_vif_to_name(vif), vif->addr);

  /* should not happen */
  if (vif->type !=  NL80211_IFTYPE_STATION &&
      vif->type !=  NL80211_IFTYPE_AP) {
    ELOG_V("Error, interface type must be Managed (sta) or AP.");
    return -ENOTSUPP;
  }

  wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
  WAVE_WV_CHECK_PTR_RES(wv_iface_inf);
  MTLK_ASSERT(wv_iface_inf != NULL);
  if (wv_iface_inf == NULL) {
    ELOG_V("Interface creation error, wv_iface_inf == NULL");
    return -EINVAL;
  }

  mac80211 = (wv_mac80211_t *)hw->priv;
  MTLK_ASSERT(mac80211 != NULL);
  if (mac80211 == NULL) {
    ELOG_V("Interface creation error, mac80211 == NULL");
    return -EINVAL;
  }

  if (!mac80211->registered) {
     ELOG_S("%s: Interface add, invalid mac80211 - not registered",
           wiphy_name(hw->wiphy));
    return -EINVAL;
  }

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_RES(master_df);

  master_core = mtlk_vap_get_core(mtlk_df_get_vap_handle(master_df));
  if (_wait_rcvry_completion_if_needed(master_core) != MTLK_ERR_OK) {
    ELOG_V("Recovery is running, can't change configuration");
    return -EBUSY;
  }

  mbss_cfg.added_vap_name = ieee80211_vif_to_name(vif);
  if (!mbss_cfg.added_vap_name) {
    ELOG_V("Net device name not found");
    return -EINVAL;
  }

  mbss_cfg.wiphy = hw->wiphy;
  mbss_cfg.role = (vif->type ==  NL80211_IFTYPE_STATION) ? MTLK_ROLE_STA : MTLK_ROLE_AP;
  mbss_cfg.is_master = FALSE; /* AP mode VAP is master */

  /* Basically mac80211 handles net device. wlan driver that works with mac80211 framework
   * should not interact with wdev or netdev directly. However, we do need netdev some
   * proprietary features */
  netdev_p = dev_get_by_name(&init_net, mbss_cfg.added_vap_name);
  MTLK_ASSERT(netdev_p != NULL);
  if (!netdev_p) {
    ELOG_S("Net device for interface %s was not found",
        mbss_cfg.added_vap_name);
    return -EINVAL;
  }
  mbss_cfg.ndev = netdev_p;

  radio = wave_vap_radio_get(mtlk_df_get_vap_handle(master_df));

  if (mtlk_df_user_get_ndev(mtlk_df_get_user(master_df)) == netdev_p){
    /*trying to add master vap*/
    wv_iface_inf->df_user = mtlk_df_get_user(master_df);
  } else {
    /* Keep trying in case scan is in progress for the current radio */
    {
      int retry_counter = 0;
      do {
        res = _mtlk_df_user_invoke_core(master_df,
                  WAVE_RADIO_REQ_MBSS_ADD_VAP_NAME, &clpb, &mbss_cfg, sizeof(mbss_cfg));
        res = _mtlk_df_user_process_core_retval_void(res, clpb,
                  WAVE_RADIO_REQ_MBSS_ADD_VAP_NAME, FALSE);

        if (MTLK_ERR_RETRY != res) break;
        mtlk_osal_msleep(50);
        retry_counter++;
      } while ((MTLK_ERR_RETRY == res) && (retry_counter < MAX_VAP_WAIT_RETRIES));

      if (retry_counter > 0)
        ILOG0_SD("%s: Scan waited, number of retries %d",
                mtlk_df_user_get_name(mtlk_df_get_user(master_df)), retry_counter);
    }

    if (MTLK_ERR_OK != res)
      goto end;

    clpb_data = mtlk_clpb_enum_get_next(clpb, &clpb_data_size);
    MTLK_CLPB_TRY(clpb_data, clpb_data_size)
      wv_iface_inf->df_user = *clpb_data;
    MTLK_CLPB_FINALLY(res)
      mtlk_clpb_delete(clpb);
    MTLK_CLPB_END;
  }

  wv_iface_inf->vap_index = mtlk_vap_get_id(mtlk_df_get_vap_handle(mtlk_df_user_get_df(wv_iface_inf->df_user)));
  wv_iface_inf->latest_rx_and_tx_packets = 0;
  wv_iface_inf->ndp_counter = 0;

  /* set device MAC address */
  res = _wv_set_mac_addr_pdb(wv_iface_inf->df_user, vif->addr);
  if (MTLK_ERR_OK != res)
    goto end;

  ASSERT(mac80211->vif_array != NULL);
  wv_iface_inf->vif = vif;
  mac80211->vif_array[wv_iface_inf->vap_index] = vif;

  if (vif->type == NL80211_IFTYPE_STATION) {
    /* By default bridging in client mode is not supported */
    netdev_p->priv_flags &= ~IFF_DONT_BRIDGE;

    /* from this point on, and until ieee80211_stop AP VAPs should not control
    * channel settings*/
  }

  if (vif->type != NL80211_IFTYPE_STATION) {
    /* AP vaps will do ADD_VAP on start_ap() callback for slaves,
     * and at op_config() or scan_do_scan() for master vap */
    goto end;
  }

  res = wave_radio_abort_and_prevent_scan(radio);
  if (MTLK_ERR_OK != res)
    goto end;
  res = _wv_activate_vif(wv_iface_inf->df_user);
  if (MTLK_ERR_OK != res){
    wave_radio_allow_or_resume_scan(radio);
    goto end;
  }
  res = wave_radio_allow_or_resume_scan(radio);

end:
  /* Decrement ref-counted netdev */
  dev_put(netdev_p);
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static void _wv_ieee80211_op_remove_interface (struct ieee80211_hw *hw, struct ieee80211_vif *vif)
{
  struct wv_vif_priv* wv_iface_inf;
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = NULL;
  mtlk_mbss_cfg_t mbss_cfg;
  wv_mac80211_t *mac80211;
  mtlk_df_t *master_df;
  mtlk_vap_handle_t _vap_to_be_removed;
  mtlk_core_t *core;

  RLOG_SY("Removing interface %s, mac addr: %Y. ",
          ieee80211_vif_to_name(vif), vif->addr);

  mac80211 = (wv_mac80211_t *)hw->priv;
  MTLK_ASSERT(mac80211 != NULL);
  if (mac80211 == NULL) {
    ELOG_V("Interface removal error, mac80211 == NULL");
    return;
  }

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_VOID(master_df);

  wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
  MTLK_ASSERT(wv_iface_inf != NULL);
  if (wv_iface_inf == NULL) {
    ELOG_V("Interface removal error, wv_iface_inf == NULL");
    return;
  }

  MTLK_ASSERT(wv_iface_inf->df_user != NULL);
  if (wv_iface_inf->df_user == NULL) {
    ELOG_V("Interface removal error, wv_iface_inf->df_user == NULL");
    return;
  }

  _vap_to_be_removed =
      mtlk_df_get_vap_handle(mtlk_df_user_get_df(wv_iface_inf->df_user));
  MTLK_ASSERT(_vap_to_be_removed != NULL);
  if (_vap_to_be_removed == NULL) {
    ELOG_V("Interface removal error, _vap_handle == NULL");
    return;
  }
  core = mtlk_vap_get_core(_vap_to_be_removed);
  if (_wait_rcvry_completion_if_needed(core) != MTLK_ERR_OK) {
    ELOG_V("Recovery is running, however remove interface must be done");
  }

  wv_iface_inf->is_initialized = FALSE;
  wv_iface_inf->latest_rx_and_tx_packets = 0;
  wv_iface_inf->ndp_counter = 0;
  wv_iface_inf->csa_status = WV_CSA_NOT_DONE;

  if (mtlk_df_user_get_hs20_status(mtlk_df_user_get_df(wv_iface_inf->df_user)))
  {
    mtlk_df_user_set_hs20_status(mtlk_df_user_get_df(wv_iface_inf->df_user), FALSE);
  }

  /* Remove VAP */
  mbss_cfg.vap_handle = _vap_to_be_removed;

  if (!mtlk_vap_is_master(_vap_to_be_removed)) {
    /* Keep trying in case scan is in progress for the current radio */
    {
      int retry_counter = 0;
      wv_iface_inf->df_user = NULL;
      do {
        res = _mtlk_df_user_invoke_core(master_df,
                  WAVE_RADIO_REQ_MBSS_DEL_VAP_NAME, &clpb, &mbss_cfg, sizeof(mbss_cfg));
        res = _mtlk_df_user_process_core_retval(res, clpb,
                  WAVE_RADIO_REQ_MBSS_DEL_VAP_NAME, TRUE);

        if (MTLK_ERR_RETRY != res) break;
        mtlk_osal_msleep(50);
        retry_counter++;
      } while ((MTLK_ERR_RETRY == res) && (retry_counter < MAX_VAP_WAIT_RETRIES));

      if (retry_counter > 0)
        ILOG0_SD("%s: Scan waited, number of retries %d",
                mtlk_df_user_get_name(mtlk_df_get_user(master_df)), retry_counter);
    }

    if (res != MTLK_ERR_OK)
      ELOG_SD("Interface %s removal error %d", ieee80211_vif_to_name(vif), res);
  } else {
    mtlk_clpb_t *clpb = NULL;
    do {
      res = _mtlk_df_user_invoke_core(master_df, WAVE_CORE_REQ_DEACTIVATE, &clpb, NULL, 0);
      res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_CORE_REQ_DEACTIVATE, TRUE);
      mtlk_osal_msleep(100);
    } while ((MTLK_ERR_OK != res) && (MTLK_ERR_FW != res));
  }

  ASSERT (mac80211->vif_array != NULL);
  mac80211->vif_array[wv_iface_inf->vap_index] = NULL;

  return;
}

static void _wv_ieee80211_op_configure_filter (struct ieee80211_hw *hw,
    unsigned int changed_flags,
    unsigned int *total_flags,
    u64 multicast)
{
  ILOG3_V("Interface filter configuration, RX filter is not supported.");
  /*
  * Receiving all multicast frames is always enabled since we currently
  * do not support programming multicast filters into the device.
  * FIF_CONTROL should not be set, since our HW does not send control
  * frames up the stack
  */
  *total_flags &= FIF_OTHER_BSS | FIF_ALLMULTI;
}

static void wv_negotiate_sta_he_mcs_nss(struct ieee80211_he_cap_elem *sta_caps,
    struct ieee80211_he_mcs_nss_supp *sta_he_mcs_nss,
    const struct ieee80211_he_mcs_nss_supp *advertised_our_caps,
    struct ieee80211_he_mcs_nss_supp *output_caps)
{
  get_he_mcs_nss(advertised_our_caps->rx_mcs_80, sta_he_mcs_nss->tx_mcs_80,
                 &output_caps->tx_mcs_80);
  get_he_mcs_nss(advertised_our_caps->tx_mcs_80, sta_he_mcs_nss->rx_mcs_80,
                 &output_caps->rx_mcs_80);

  if (sta_caps->phy_cap_info[0] &
      HE_5G_6G_PHY_CAP0_CHAN_WIDTH_SET_160MHZ) {
    get_he_mcs_nss(advertised_our_caps->rx_mcs_160, sta_he_mcs_nss->tx_mcs_160,
                   &output_caps->tx_mcs_160);
    get_he_mcs_nss(advertised_our_caps->tx_mcs_160, sta_he_mcs_nss->rx_mcs_160,
                   &output_caps->rx_mcs_160);
  }

  if (sta_caps->phy_cap_info[0] &
      HE_5G_6G_PHY_CAP0_CHAN_WIDTH_SET_80PLUS80_MHZ) {
    get_he_mcs_nss(advertised_our_caps->rx_mcs_80p80, sta_he_mcs_nss->tx_mcs_80p80,
                   &output_caps->tx_mcs_80p80);
    get_he_mcs_nss(advertised_our_caps->tx_mcs_80p80, sta_he_mcs_nss->rx_mcs_80p80,
                   &output_caps->rx_mcs_80p80);
  }
}

static void wv_negotiate_sta_he_mac_phy_capab(struct ieee80211_he_cap_elem *sta_caps,
    const struct ieee80211_he_cap_elem *advertised_our_caps,
    const struct ieee80211_he_cap_elem *non_advertised_our_caps,
    struct ieee80211_he_cap_elem *output_caps)
{
  u8 cap = 0, min = 0;

  output_caps->mac_cap_info[0] |= min_he_cap(sta_caps->mac_cap_info[0],
          non_advertised_our_caps->mac_cap_info[0],
          IEEE80211_HE_MAC_CAP0_HTC_HE);
  output_caps->mac_cap_info[0] |= min_he_cap(sta_caps->mac_cap_info[0],
          advertised_our_caps->mac_cap_info[0],
          IEEE80211_HE_MAC_CAP0_TWT_REQ);
  output_caps->mac_cap_info[0] |= min_he_cap(sta_caps->mac_cap_info[0],
          advertised_our_caps->mac_cap_info[0],
          IEEE80211_HE_MAC_CAP0_TWT_RES);
  output_caps->mac_cap_info[0] |= min_he_cap(sta_caps->mac_cap_info[0],
          non_advertised_our_caps->mac_cap_info[0],
          IEEE80211_HE_MAC_CAP0_DYNAMIC_FRAG_MASK);
  output_caps->mac_cap_info[0] |= min_he_cap(sta_caps->mac_cap_info[0],
          non_advertised_our_caps->mac_cap_info[0],
          IEEE80211_HE_MAC_CAP0_MAX_NUM_FRAG_MSDU_MASK);

  output_caps->mac_cap_info[1] |= min_he_cap(sta_caps->mac_cap_info[1],
          non_advertised_our_caps->mac_cap_info[1],
          IEEE80211_HE_MAC_CAP1_MIN_FRAG_SIZE_MASK);
  output_caps->mac_cap_info[1] |= min_he_cap(sta_caps->mac_cap_info[1],
          non_advertised_our_caps->mac_cap_info[1],
          IEEE80211_HE_MAC_CAP1_TF_MAC_PAD_DUR_MASK);
  cap |= get_he_cap(advertised_our_caps->mac_cap_info[4],
                    IEEE80211_HE_MAC_CAP4_MULTI_TID_AGG_TX_QOS_B39);
  cap |= (get_he_cap(advertised_our_caps->mac_cap_info[5],
                     (IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B40 |
                      IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B41)) << 1);
  min = MIN(cap, get_he_cap(sta_caps->mac_cap_info[1],
                            IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_MASK));
  output_caps->mac_cap_info[1] |= set_he_cap(min,
          IEEE80211_HE_MAC_CAP1_MULTI_TID_AGG_RX_QOS_MASK);

  output_caps->mac_cap_info[4] |= set_he_cap(cap,
          IEEE80211_HE_MAC_CAP4_MULTI_TID_AGG_TX_QOS_B39);
  output_caps->mac_cap_info[5] |= set_he_cap((cap >> 1),
          (IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B40 |
           IEEE80211_HE_MAC_CAP5_MULTI_TID_AGG_TX_QOS_B41));

  if (output_caps->mac_cap_info[0] & IEEE80211_HE_MAC_CAP0_HTC_HE) {
    output_caps->mac_cap_info[1] |= min_he_cap(sta_caps->mac_cap_info[1],
            advertised_our_caps->mac_cap_info[1],
            IEEE80211_HE_MAC_CAP1_LINK_ADAPTATION);
    output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
            advertised_our_caps->mac_cap_info[2],
            IEEE80211_HE_MAC_CAP2_LINK_ADAPTATION);
    output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
            advertised_our_caps->mac_cap_info[2], IEEE80211_HE_MAC_CAP2_BSR);
    output_caps->mac_cap_info[3] |= min_he_cap(sta_caps->mac_cap_info[3],
            non_advertised_our_caps->mac_cap_info[3],
            IEEE80211_HE_MAC_CAP3_OMI_CONTROL);
    output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
            non_advertised_our_caps->mac_cap_info[2], IEEE80211_HE_MAC_CAP2_TRS);
  }
  output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
          non_advertised_our_caps->mac_cap_info[2],
          IEEE80211_HE_MAC_CAP2_ALL_ACK);
  output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
          advertised_our_caps->mac_cap_info[2],
          IEEE80211_HE_MAC_CAP2_BCAST_TWT);
  output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
          non_advertised_our_caps->mac_cap_info[2],
          IEEE80211_HE_MAC_CAP2_32BIT_BA_BITMAP);
  output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
          advertised_our_caps->mac_cap_info[2],
          IEEE80211_HE_MAC_CAP2_MU_CASCADING);
  output_caps->mac_cap_info[2] |= min_he_cap(sta_caps->mac_cap_info[2],
          non_advertised_our_caps->mac_cap_info[2],
          IEEE80211_HE_MAC_CAP2_ACK_EN);

  output_caps->mac_cap_info[3] |= min_he_cap(sta_caps->mac_cap_info[3],
          advertised_our_caps->mac_cap_info[3],
          IEEE80211_HE_MAC_CAP3_OFDMA_RA);
  output_caps->mac_cap_info[3] |= min_he_cap(sta_caps->mac_cap_info[3],
          non_advertised_our_caps->mac_cap_info[3],
          IEEE80211_HE_MAC_CAP3_MAX_AMPDU_LEN_EXP_MASK);
  output_caps->mac_cap_info[3] |= min_he_cap(sta_caps->mac_cap_info[3],
          non_advertised_our_caps->mac_cap_info[3],
          IEEE80211_HE_MAC_CAP3_AMSDU_FRAG);
  output_caps->mac_cap_info[3] |= min_he_cap(sta_caps->mac_cap_info[3],
          non_advertised_our_caps->mac_cap_info[3],
          IEEE80211_HE_MAC_CAP3_FLEX_TWT_SCHED);
  output_caps->mac_cap_info[3] |= min_he_cap(sta_caps->mac_cap_info[3],
          non_advertised_our_caps->mac_cap_info[3],
          IEEE80211_HE_MAC_CAP3_RX_CTRL_FRAME_TO_MULTIBSS);

  output_caps->mac_cap_info[4] |= min_he_cap(sta_caps->mac_cap_info[4],
          non_advertised_our_caps->mac_cap_info[4],
          IEEE80211_HE_MAC_CAP4_BSRP_BQRP_A_MPDU_AGG);
  output_caps->mac_cap_info[4] |= min_he_cap(sta_caps->mac_cap_info[4],
          advertised_our_caps->mac_cap_info[4], IEEE80211_HE_MAC_CAP4_QTP);
  output_caps->mac_cap_info[4] |= min_he_cap(sta_caps->mac_cap_info[4],
          advertised_our_caps->mac_cap_info[4], IEEE80211_HE_MAC_CAP4_SRP_RESP);
  output_caps->mac_cap_info[4] |= min_he_cap(sta_caps->mac_cap_info[4],
          advertised_our_caps->mac_cap_info[4], IEEE80211_HE_MAC_CAP4_NDP_FB_REP);
  output_caps->mac_cap_info[4] |= min_he_cap(sta_caps->mac_cap_info[4],
          advertised_our_caps->mac_cap_info[4], IEEE80211_HE_MAC_CAP4_OPS);
  output_caps->mac_cap_info[4] |= min_he_cap(sta_caps->mac_cap_info[4],
          non_advertised_our_caps->mac_cap_info[4],
          IEEE80211_HE_MAC_CAP4_AMDSU_IN_AMPDU);

  output_caps->mac_cap_info[5] |= min_he_cap(sta_caps->mac_cap_info[5],
          advertised_our_caps->mac_cap_info[5],
          IEEE80211_HE_MAC_CAP5_SUBCHAN_SELECVITE_TRANSMISSION);
  output_caps->mac_cap_info[5] |= min_he_cap(sta_caps->mac_cap_info[5],
          non_advertised_our_caps->mac_cap_info[5],
          IEEE80211_HE_MAC_CAP5_UL_2x996_TONE_RU);
  output_caps->mac_cap_info[5] |= min_he_cap(sta_caps->mac_cap_info[5],
          advertised_our_caps->mac_cap_info[5],
          IEEE80211_HE_MAC_CAP5_OM_CTRL_UL_MU_DATA_DIS_RX);
  if (sta_caps->mac_cap_info[5] & IEEE80211_HE_MAC_CAP5_HE_DYNAMIC_SM_PS)
    output_caps->mac_cap_info[5] |= IEEE80211_HE_MAC_CAP5_HE_DYNAMIC_SM_PS;

  output_caps->phy_cap_info[0] = sta_caps->phy_cap_info[0];
  output_caps->phy_cap_info[1] |= min_he_cap(sta_caps->phy_cap_info[1],
	  non_advertised_our_caps->phy_cap_info[1],
          IEEE80211_HE_PHY_CAP1_PREAMBLE_PUNC_RX_MASK);
  output_caps->phy_cap_info[1] |= min_he_cap(sta_caps->phy_cap_info[1],
          non_advertised_our_caps->phy_cap_info[1],
          IEEE80211_HE_PHY_CAP1_DEVICE_CLASS_A);
  output_caps->phy_cap_info[1] |= min_he_cap(sta_caps->phy_cap_info[1],
          advertised_our_caps->phy_cap_info[1],
          IEEE80211_HE_PHY_CAP1_LDPC_CODING_IN_PAYLOAD);
  output_caps->phy_cap_info[1] |= min_he_cap(sta_caps->phy_cap_info[1],
          non_advertised_our_caps->phy_cap_info[1],
          IEEE80211_HE_PHY_CAP1_HE_LTF_AND_GI_FOR_HE_PPDUS_0_8US);

  if (sta_caps->phy_cap_info[4] & IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE) {
    output_caps->phy_cap_info[7] |= min_he_cap(sta_caps->phy_cap_info[7],
            advertised_our_caps->phy_cap_info[7], IEEE80211_HE_PHY_CAP7_MAX_NC_MASK);
    output_caps->phy_cap_info[2] |= min_he_cap(sta_caps->phy_cap_info[2],
            non_advertised_our_caps->phy_cap_info[2],
            IEEE80211_HE_PHY_CAP2_NDP_4x_LTF_AND_3_2US);
    min = MIN(get_he_cap(sta_caps->phy_cap_info[4],
                         IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_MASK),
              get_he_cap(advertised_our_caps->phy_cap_info[5],
                         IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_MASK));
    output_caps->phy_cap_info[4] |= set_he_cap(min,
            IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_MASK);
    min = MIN(get_he_cap(sta_caps->phy_cap_info[4],
                         IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_MASK),
              get_he_cap(advertised_our_caps->phy_cap_info[5],
                         IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_MASK));
    output_caps->phy_cap_info[4] |= set_he_cap(min,
            IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_MASK);
  }

  if ((sta_caps->phy_cap_info[2] & IEEE80211_HE_PHY_CAP2_DOPPLER_TX) ||
      (sta_caps->phy_cap_info[2] & IEEE80211_HE_PHY_CAP2_DOPPLER_RX)) {
    output_caps->phy_cap_info[1] |= min_he_cap(sta_caps->phy_cap_info[1],
            advertised_our_caps->phy_cap_info[1],
            IEEE80211_HE_PHY_CAP1_MIDAMBLE_RX_TX_MAX_NSTS);
    output_caps->phy_cap_info[2] |= min_he_cap(sta_caps->phy_cap_info[2],
            advertised_our_caps->phy_cap_info[2],
            IEEE80211_HE_PHY_CAP2_MIDAMBLE_RX_TX_MAX_NSTS);
    output_caps->phy_cap_info[8] |= min_he_cap(sta_caps->phy_cap_info[8],
            advertised_our_caps->phy_cap_info[8],
            IEEE80211_HE_PHY_CAP8_MIDAMBLE_RX_TX_2X_AND_1XLTF);
  }

  output_caps->phy_cap_info[2] |= min_he_cap(sta_caps->phy_cap_info[2],
          advertised_our_caps->phy_cap_info[2],
          IEEE80211_HE_PHY_CAP2_STBC_TX_UNDER_80MHZ);
  min = MIN(get_he_cap(sta_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_STBC_TX_UNDER_80MHZ),
            get_he_cap(advertised_our_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_STBC_RX_UNDER_80MHZ));
  output_caps->phy_cap_info[2] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP2_STBC_TX_UNDER_80MHZ);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_STBC_RX_UNDER_80MHZ),
            get_he_cap(advertised_our_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_STBC_TX_UNDER_80MHZ));
  output_caps->phy_cap_info[2] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP2_STBC_RX_UNDER_80MHZ);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_DOPPLER_TX),
            get_he_cap(advertised_our_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_DOPPLER_RX));
  output_caps->phy_cap_info[2] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP2_DOPPLER_TX);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_DOPPLER_RX),
            get_he_cap(advertised_our_caps->phy_cap_info[2],
                       IEEE80211_HE_PHY_CAP2_DOPPLER_TX));
  output_caps->phy_cap_info[2] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP2_DOPPLER_RX);
  output_caps->phy_cap_info[2] |= min_he_cap(sta_caps->phy_cap_info[2],
          advertised_our_caps->phy_cap_info[2],
          IEEE80211_HE_PHY_CAP2_UL_MU_FULL_MU_MIMO);
  output_caps->phy_cap_info[2] |= min_he_cap(sta_caps->phy_cap_info[2],
          advertised_our_caps->phy_cap_info[2],
          IEEE80211_HE_PHY_CAP2_UL_MU_PARTIAL_MU_MIMO);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK),
            get_he_cap(non_advertised_our_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK));
  output_caps->phy_cap_info[3] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK),
            get_he_cap(non_advertised_our_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK));
  output_caps->phy_cap_info[3] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK),
            get_he_cap(non_advertised_our_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK));
  output_caps->phy_cap_info[3] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK),
            get_he_cap(non_advertised_our_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK));
  output_caps->phy_cap_info[3] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK);
  output_caps->phy_cap_info[3] |= min_he_cap(sta_caps->phy_cap_info[3],
          advertised_our_caps->phy_cap_info[3],
          IEEE80211_HE_PHY_CAP3_RX_HE_MU_PPDU_FROM_NON_AP_STA);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER),
            get_he_cap(advertised_our_caps->phy_cap_info[4],
                       IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE));
  output_caps->phy_cap_info[3] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[4],
                       IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE),
            get_he_cap(advertised_our_caps->phy_cap_info[3],
                       IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER));
  output_caps->phy_cap_info[4] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP4_SU_BEAMFORMEE);
  output_caps->phy_cap_info[4] |= min_he_cap(sta_caps->phy_cap_info[4],
          advertised_our_caps->phy_cap_info[4],
          IEEE80211_HE_PHY_CAP4_MU_BEAMFORMER);

  if (sta_caps->phy_cap_info[4] & IEEE80211_HE_PHY_CAP3_SU_BEAMFORMER) {
    min = MIN(get_he_cap(sta_caps->phy_cap_info[5],
                         IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_MASK),
              get_he_cap(advertised_our_caps->phy_cap_info[4],
                         IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_UNDER_80MHZ_MASK));
    output_caps->phy_cap_info[5] |= set_he_cap(min,
            IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_UNDER_80MHZ_MASK);
    min = MIN(get_he_cap(sta_caps->phy_cap_info[5],
                         IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_MASK),
              get_he_cap(advertised_our_caps->phy_cap_info[4],
                         IEEE80211_HE_PHY_CAP4_BEAMFORMEE_MAX_STS_ABOVE_80MHZ_MASK));
    output_caps->phy_cap_info[5] |= set_he_cap(min,
            IEEE80211_HE_PHY_CAP5_BEAMFORMEE_NUM_SND_DIM_ABOVE_80MHZ_MASK);
  }
  output_caps->phy_cap_info[5] |= min_he_cap(sta_caps->phy_cap_info[5],
          non_advertised_our_caps->phy_cap_info[5],
          IEEE80211_HE_PHY_CAP5_NG16_SU_FEEDBACK);
  output_caps->phy_cap_info[5] |= min_he_cap(sta_caps->phy_cap_info[5],
          non_advertised_our_caps->phy_cap_info[5],
          IEEE80211_HE_PHY_CAP5_NG16_MU_FEEDBACK);

  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          non_advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_CODEBOOK_SIZE_42_SU);
  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          non_advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_CODEBOOK_SIZE_75_MU);
  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_TRIG_SU_BEAMFORMER_FB);
  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_TRIG_MU_BEAMFORMER_FB);
  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_TRIG_CQI_FB);
  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_PARTIAL_BW_EXT_RANGE);
  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          non_advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_PARTIAL_BANDWIDTH_DL_MUMIMO);
  output_caps->phy_cap_info[6] |= min_he_cap(sta_caps->phy_cap_info[6],
          non_advertised_our_caps->phy_cap_info[6],
          IEEE80211_HE_PHY_CAP6_PPE_THRESHOLD_PRESENT);

  output_caps->phy_cap_info[7] |= min_he_cap(sta_caps->phy_cap_info[7],
          advertised_our_caps->phy_cap_info[7],
          IEEE80211_HE_PHY_CAP7_SRP_BASED_SR);
  output_caps->phy_cap_info[7] |= min_he_cap(sta_caps->phy_cap_info[7],
          advertised_our_caps->phy_cap_info[7],
          IEEE80211_HE_PHY_CAP7_POWER_BOOST_FACTOR_AR);
  output_caps->phy_cap_info[7] |= min_he_cap(sta_caps->phy_cap_info[7],
          non_advertised_our_caps->phy_cap_info[7],
          IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[7],
                       IEEE80211_HE_PHY_CAP7_STBC_TX_ABOVE_80MHZ),
            get_he_cap(advertised_our_caps->phy_cap_info[7],
                       IEEE80211_HE_PHY_CAP7_STBC_RX_ABOVE_80MHZ));
  output_caps->phy_cap_info[7] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP7_STBC_TX_ABOVE_80MHZ);

  min = MIN(get_he_cap(sta_caps->phy_cap_info[7],
                       IEEE80211_HE_PHY_CAP7_STBC_RX_ABOVE_80MHZ),
            get_he_cap(advertised_our_caps->phy_cap_info[7],
                       IEEE80211_HE_PHY_CAP7_STBC_TX_ABOVE_80MHZ));
  output_caps->phy_cap_info[7] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP7_STBC_RX_ABOVE_80MHZ);

  if (output_caps->phy_cap_info[7] &
      IEEE80211_HE_PHY_CAP7_HE_SU_MU_PPDU_4XLTF_AND_08_US_GI) {
    output_caps->phy_cap_info[8] |= min_he_cap(sta_caps->phy_cap_info[8],
            non_advertised_our_caps->phy_cap_info[8],
            IEEE80211_HE_PHY_CAP8_HE_ER_SU_PPDU_4XLTF_AND_08_US_GI);
  }

  if (output_caps->phy_cap_info[0] &
      IEEE80211_HE_PHY_CAP0_CHANNEL_WIDTH_SET_40MHZ_IN_2G) {
    output_caps->phy_cap_info[8] |= min_he_cap(sta_caps->phy_cap_info[8],
            non_advertised_our_caps->phy_cap_info[8],
            IEEE80211_HE_PHY_CAP8_20MHZ_IN_40MHZ_HE_PPDU_IN_2G);
  }

  if (output_caps->phy_cap_info[0] &
      HE_5G_6G_PHY_CAP0_CHAN_WIDTH_SET_40MHZ_80MHZ) {
    output_caps->phy_cap_info[8] |= min_he_cap(sta_caps->phy_cap_info[8],
            non_advertised_our_caps->phy_cap_info[8],
            IEEE80211_HE_PHY_CAP8_20MHZ_IN_160MHZ_HE_PPDU);
  }

  if (output_caps->phy_cap_info[0] &
      HE_5G_6G_PHY_CAP0_CHAN_WIDTH_SET_160MHZ) {
    output_caps->phy_cap_info[8] |= min_he_cap(sta_caps->phy_cap_info[8],
            non_advertised_our_caps->phy_cap_info[8],
            IEEE80211_HE_PHY_CAP8_80MHZ_IN_160MHZ_HE_PPDU);
  }
  output_caps->phy_cap_info[8] |= min_he_cap(sta_caps->phy_cap_info[8],
          non_advertised_our_caps->phy_cap_info[8],
          IEEE80211_HE_PHY_CAP8_HE_ER_SU_1XLTF_AND_08_US_GI);

  if ((output_caps->phy_cap_info[3] & IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_TX_MASK) ||
      (output_caps->phy_cap_info[3] & IEEE80211_HE_PHY_CAP3_DCM_MAX_CONST_RX_MASK)) {
    output_caps->phy_cap_info[8] |= min_he_cap(sta_caps->phy_cap_info[8],
            advertised_our_caps->phy_cap_info[8],
            IEEE80211_HE_PHY_CAP8_DCM_MAX_RU_MASK);
  }

  min = MIN(get_he_cap(sta_caps->phy_cap_info[9],
                       IEEE80211_HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU),
            get_he_cap(advertised_our_caps->phy_cap_info[9],
                       IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU));
  output_caps->phy_cap_info[9] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU);
  min = MIN(get_he_cap(sta_caps->phy_cap_info[9],
                       IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU),
            get_he_cap(advertised_our_caps->phy_cap_info[9],
                       IEEE80211_HE_PHY_CAP9_TX_1024_QAM_LESS_THAN_242_TONE_RU));
  output_caps->phy_cap_info[9] |= set_he_cap(min,
          IEEE80211_HE_PHY_CAP9_RX_1024_QAM_LESS_THAN_242_TONE_RU);
  output_caps->phy_cap_info[9] |= min_he_cap(sta_caps->phy_cap_info[9],
          non_advertised_our_caps->phy_cap_info[9],
          IEEE80211_HE_PHY_CAP9_LONGER_THAN_16_SIGB_OFDM_SYM);
  output_caps->phy_cap_info[9] |= min_he_cap(sta_caps->phy_cap_info[9],
          non_advertised_our_caps->phy_cap_info[9],
          IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_COMP_SIGB);
  output_caps->phy_cap_info[9] |= min_he_cap(sta_caps->phy_cap_info[9],
          non_advertised_our_caps->phy_cap_info[9],
          IEEE80211_HE_PHY_CAP9_RX_FULL_BW_SU_USING_MU_WITH_NON_COMP_SIGB);
  output_caps->phy_cap_info[9] |= min_he_cap(sta_caps->phy_cap_info[9],
          advertised_our_caps->phy_cap_info[9],
          IEEE80211_HE_PHY_CAP9_NON_TRIGGERED_CQI_FEEDBACK);
}

static int wv_negotiate_sta_he_cap(mtlk_df_user_t *df_user,
    struct ieee80211_hw *hw,
    struct ieee80211_vif *vif,
    struct ieee80211_sta_he_cap *sta_he_cap)
{
  mtlk_core_t *core;
  mtlk_pdb_t *param_db_core;
  struct ieee80211_supported_band *sband;
  const struct ieee80211_sta_he_cap *hw_he_cap;
  mtlk_pdb_size_t pdb_len = sizeof(struct intel_vendor_he_capa);
  struct ieee80211_channel *chan;
  struct ieee80211_sta_he_cap neg_sta_he_cap;
  struct intel_vendor_he_capa hw_non_adv_he_cap = {0};
  size_t max_sta_he_cap_len = sizeof(struct ieee80211_sta_he_cap);
  struct mtlk_he_debug_mode_data adv_he_capab;
  mtlk_pdb_size_t adv_he_capab_len = sizeof(struct mtlk_he_debug_mode_data);

  core = mtlk_vap_get_core(mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user)));
  MTLK_ASSERT(NULL != core);

  rcu_read_lock();
  chan = rcu_dereference(vif->chanctx_conf)->def.chan;
  MTLK_ASSERT(NULL != chan);
  rcu_read_unlock();

  sband = hw->wiphy->bands[chan->band];
  if (!sband || !sband->iftype_data || !sband->iftype_data->he_cap.has_he)
    return -EINVAL;

  hw_he_cap = &sband->iftype_data->he_cap;
  param_db_core = mtlk_vap_get_param_db(core->vap_handle);

  /* Make sure we can cast intel_vendor_he_capa data to ieee80211_he_cap_elem */
  MTLK_STATIC_ASSERT(sizeof(struct intel_vendor_he_capa) >= sizeof(struct ieee80211_he_cap_elem));
  MTLK_STATIC_ASSERT(MTLK_OFFSET_OF(struct intel_vendor_he_capa,  he_mac_capab_info) ==
                     MTLK_OFFSET_OF(struct ieee80211_he_cap_elem, mac_cap_info));
  MTLK_STATIC_ASSERT(MTLK_OFFSET_OF(struct intel_vendor_he_capa,  he_phy_capab_info) ==
                     MTLK_OFFSET_OF(struct ieee80211_he_cap_elem, phy_cap_info));
  MTLK_STATIC_ASSERT(MTLK_OFFSET_OF(struct intel_vendor_he_capa,  he_txrx_mcs_support) ==
                     MTLK_OFFSET_OF(struct ieee80211_he_cap_elem, optional));
  wave_pdb_get_binary(param_db_core, PARAM_DB_CORE_HE_NON_ADVERTISED,
                      &hw_non_adv_he_cap, &pdb_len);

  wave_pdb_get_binary(param_db_core, PARAM_DB_CORE_HE_DEBUG_DATA,
                      &adv_he_capab, &adv_he_capab_len);

  mtlk_dump(1, sta_he_cap, max_sta_he_cap_len, "Received: STA HE Cap");

  memset(&neg_sta_he_cap, 0, max_sta_he_cap_len);
  if (adv_he_capab.he_debug_mode_enabled) {
    ILOG0_V("Add STA HE Debug Mode Enabled\n");
    wv_negotiate_sta_he_mac_phy_capab(&sta_he_cap->he_cap_elem, &adv_he_capab.he_cap_elem,
                                      (struct ieee80211_he_cap_elem *)&hw_non_adv_he_cap,
                                      &neg_sta_he_cap.he_cap_elem);
    wv_negotiate_sta_he_mcs_nss(&neg_sta_he_cap.he_cap_elem, &sta_he_cap->he_mcs_nss_supp,
                                &adv_he_capab.he_mcs_nss_supp, &neg_sta_he_cap.he_mcs_nss_supp);
  } else {
    wv_negotiate_sta_he_mac_phy_capab(&sta_he_cap->he_cap_elem, &hw_he_cap->he_cap_elem,
                                      (struct ieee80211_he_cap_elem *)&hw_non_adv_he_cap,
                                      &neg_sta_he_cap.he_cap_elem);
    wv_negotiate_sta_he_mcs_nss(&neg_sta_he_cap.he_cap_elem, &sta_he_cap->he_mcs_nss_supp,
                                &hw_he_cap->he_mcs_nss_supp, &neg_sta_he_cap.he_mcs_nss_supp);
  }
  neg_sta_he_cap.has_he =true;
  /* Note: There is no negotiation on PPE currently, so PPE offset is being passed as all zero from here.
   * To-do: received STA PPE data required to be negotiated if STA supports 8 spatial stream (max AP supports 4).
   */
  wave_memcpy(sta_he_cap, max_sta_he_cap_len, &neg_sta_he_cap, max_sta_he_cap_len);

  mtlk_dump(1, sta_he_cap, max_sta_he_cap_len, "Negotiated: STA HE Cap");

  return 0;
}

static mtlk_error_t _wv_request_sta_connect_ap(mtlk_df_user_t *df_user,
    struct ieee80211_hw *hw, struct ieee80211_vif *vif,
    struct ieee80211_sta *sta)
{
  const u8 mbssid_aid_offset[] = {MBSSID_AID_OFFSET0, MBSSID_TX_VAP_AID_OFFSET64, MBSSID_NON_TX_VAP_AID_OFFSET64};
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  sta_entry * sta_p = wv_ieee80211_get_sta(sta);
  sta_info * sta_info_p;
  mtlk_clpb_t *clpb = NULL;
  int i = 0, band = 0;
  uint8 zero_rx_mask[WV_HT_MCS_MASK_LEN];
  uint16 zero_vht_info[WV_VHT_MCS_INFO_NUM_FIELDS];
  uint32 mbssid_aid_offset_idx;
  mtlk_vap_handle_t vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  uint32 mesh_mode = wave_pdb_get_int(mtlk_vap_get_param_db(vap_handle), PARAM_DB_CORE_MESH_MODE);
  mtlk_error_t res = MTLK_ERR_OK;

  ILOG2_SY("%s: request to connect sta:%Y", ieee80211_vif_to_name(vif), sta->addr);

  memset(zero_rx_mask, 0, sizeof(zero_rx_mask));
  memset(zero_vht_info, 0, sizeof(zero_vht_info));

  sta_info_p = &sta_p->info;

  for (band = 0; band < NUM_SUPPORTED_BANDS; band++) {
    unsigned long basic_rates = sta->supp_rates[band];
    for_each_set_bit(i, &basic_rates, BITS_PER_LONG) {
      int bitrate = hw->wiphy->bands[band]->bitrates[i].bitrate / 5;
      sta_info_p->rates[sta_info_p->supported_rates_len++] = bitrate;
      if (band == NL80211_BAND_5GHZ && wave_radio_is_rate_80211ag(bitrate)) {
        MTLK_BIT_SET(sta_info_p->sta_net_modes, MTLK_WSSA_11A_SUPPORTED, 1);
      } else if (band == NL80211_BAND_6GHZ) {
        MTLK_BIT_SET(sta_info_p->sta_net_modes, MTLK_WSSA_11AX_SUPPORTED, 1);
      } else if (band == NL80211_BAND_2GHZ) {
        if (wave_radio_is_rate_80211ag(bitrate))
          MTLK_BIT_SET(sta_info_p->sta_net_modes, MTLK_WSSA_11G_SUPPORTED, 1);
        if (wave_radio_is_rate_80211b(bitrate)) {
          MTLK_BIT_SET(sta_info_p->sta_net_modes, MTLK_WSSA_11B_SUPPORTED, 1);
        }
      }
    }
  }

  wv_iface_inf->latest_rx_and_tx_packets = 0;
  wv_iface_inf->ndp_counter = 0;

  /* If code below is not working, check if "last association request" patch is in kernel and hostapd */
  if (sta->resp) {
    wv_ie80211_elems elems;
    mtlk_error_t res = MTLK_ERR_OK;
    struct ieee80211_mgmt *p_frame= (struct ieee80211_mgmt *)sta->resp;
    /* Association request by default */
    uint8 *ie_ptr = p_frame->u.assoc_req.variable;
    memset(&elems, 0, sizeof(elems));
    /* Or maybe we have Reassociation request? */
    if (MAN_TYPE_REASSOC_REQ == (WLAN_FC_GET_STYPE(mtlk_wlan_pkt_get_frame_ctl(sta->resp)) >> FRAME_SUBTYPE_SHIFT))
      ie_ptr = p_frame->u.reassoc_req.variable;

    /* Adjust frame length before parsing IEs */
    res = mtlk_mgmt_parse_elems(ie_ptr, (sta->resp_len-((unsigned long)ie_ptr-(unsigned long)p_frame)), &elems);
    if (MTLK_ERR_OK != res) {
      ELOG_S("%s: Error parsing IE elements", ieee80211_vif_to_name(vif));
      mtlk_sta_decref(sta_p);
      return MTLK_ERR_PARAMS;
    }

    if (elems.bss_coex_20_40) {
      sta_info_p->bss_coex_20_40 = *elems.bss_coex_20_40;
    }

    if (elems.vendor_lantiq) {
      sta_info_p->vendor = 1;
    }
    else if (elems.vendor_metalink) {
      sta_info_p->vendor = 1;
    }
    else if (elems.vendor_w101) {
      sta_info_p->vendor = 2;
    }

    if (mesh_mode == MESH_MODE_HYBRID || mesh_mode == MESH_MODE_EXT_HYBRID) {
      if (elems.vendor_intel) {
        uint8 type = elems.vendor_intel[WLAN_EID_VENDOR_SPECIFIC_OUI_TYPE_OFFSET];
        const uint8 *attributes = elems.vendor_intel +
          WLAN_EID_INTEL_VENDOR_ATTRIBUTES_OFFSET;

        if ((type == WLAN_EID_INTEL_VENDOR_TYPE_4ADDR_MODE) &&
            (*attributes == WLAN_EID_INTEL_VENDOR_4ADDR_MODE_STA)) {
          /* Default value of WDS_client_type is set by memset to value 0 == PEER_AP */
          sta_info_p->WDS_client_type = FOUR_ADDRESSES_STATION;
        }
      }
      if (elems.vendor_brcm) {
        uint8 type = elems.vendor_brcm[WLAN_EID_VENDOR_SPECIFIC_OUI_TYPE_OFFSET];

        if (WLAN_EID_BRCM_VENDOR_VERSION_2 == type) {
          uint8 flags1 = elems.vendor_brcm[WLAN_EID_BRCM_VENDOR_FLAGS1_OFFSET];
          if (flags1 & WLAN_EID_BRCM_VENDOR_DWDS_CAPABLE) {
            sta_info_p->WDS_client_type = FOUR_ADDRESSES_STATION;
          }
        }
      }
      if (elems.vendor_wfa) {
        uint8 type = elems.vendor_wfa[WLAN_EID_VENDOR_SPECIFIC_OUI_TYPE_OFFSET];

        if (WLAN_OUI_TYPE_WFA_MULTI_AP == type) {
          /* Default value of WDS_client_type is set by memset to value 0 == PEER_AP */
          sta_info_p->WDS_client_type = FOUR_ADDRESSES_STATION;
        }
      }
    }
    if (elems.opmode_notif) {
      sta_info_p->opmode_notif = *elems.opmode_notif;
      MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_OPMODE_NOTIF, 1);
    }
  }

  mbssid_aid_offset_idx = wave_pdb_get_int(mtlk_vap_get_param_db(vap_handle), PARAM_DB_CORE_MBSSID_VAP);

  ASSERT(ARRAY_SIZE(mbssid_aid_offset) > mbssid_aid_offset_idx);

  /* For AP interface hostapd requests aid using a vendor command and later
   * uses this aid (which is sid + 1) when adding a station.
   * A station interface can only be connected to a single AP. SID is requested
   * during connection, stored in driver private struct and used here */
  if (vif->type == NL80211_IFTYPE_AP)
    sta_info_p->sid = sta->aid - mbssid_aid_offset[mbssid_aid_offset_idx] - 1;
  else
    sta_info_p->sid = wv_iface_inf->vif_sid;

  sta_info_p->listen_interval = sta->listen_interval;

  sta_info_p->rssi_dbm = (int32)sta->rssi;
  mtlk_stadb_stats_set_mgmt_rssi(sta_info_p, sta_info_p->rssi_dbm);
  ILOG2_D("Get RSSI from HOSTAPD: info->rssi_dbm=%d", sta_info_p->rssi_dbm);

  MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_MFP, sta->mfp ? 1 : 0);
  MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_WMM, sta->wme ? 1 : 0);

  if (sta->ht_cap.ht_supported) {
    if(memcmp(zero_rx_mask, sta->ht_cap.mcs.rx_mask,
        sizeof(zero_rx_mask)) != 0) {
      sta_info_p->ampdu_param =
          sta->ht_cap.ampdu_density << 2 | sta->ht_cap.ampdu_factor;
      sta_info_p->tx_bf_cap_info = sta->ht_cap.tx_BF_cap_info;
      MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_11n, 1);
      MTLK_BIT_SET(sta_info_p->sta_net_modes, MTLK_WSSA_11N_SUPPORTED, 1);
    }
  }

  if (sta->vht_cap.vht_supported) {
    if(memcmp(&zero_vht_info, &sta->vht_cap.vht_mcs,
        sizeof(zero_vht_info)) != 0) {
      MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_11ac, 1);
      MTLK_BIT_SET(sta_info_p->sta_net_modes, MTLK_WSSA_11AC_SUPPORTED, 1);
    }
  }

  if (sta->he_cap.has_he) {
    if (wv_negotiate_sta_he_cap(df_user, hw, vif, &sta->he_cap))
      return -EINVAL;

    MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_11ax, 1);
    MTLK_BIT_SET(sta_info_p->sta_net_modes, MTLK_WSSA_11AX_SUPPORTED, 1);
   /* Set the flag corresponding to HE 6GHZ Band Capabilities.    *
    * Any negotiation with HW capabilities is done in the Hostapd */
    if (!sta->vht_cap.vht_supported)
      MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_11ax_6ghz, 1);
  }

  MTLK_BFIELD_SET(sta_info_p->flags, STA_FLAGS_OMN_SUPPORTED, !!sta->opmode_notif);

  if (sta->vendor_wds)
    sta_info_p->WDS_client_type = FOUR_ADDRESSES_STATION;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
      WAVE_CORE_REQ_AP_CONNECT_STA, &clpb, &sta, sizeof(sta));
  res = _mtlk_df_user_process_core_retval(res, clpb,
      WAVE_CORE_REQ_AP_CONNECT_STA, TRUE);

  /* For AP wep there is only one key so the SET_KEY was called only once,
   * however it needs to be set for broadcast (which was done in set_key) and
   * for unicast, which we will do here.
   * we'll get the key params from the core and set it in unicast mode
   * in case of wep cipher */
  if ((mtlk_vap_is_ap(vap_handle)) && (res == MTLK_ERR_OK)) {
    mtlk_core_t * core = mtlk_vap_get_core(vap_handle);

    if (core->slow_ctx->wep_enabled) {
      int key_index = 0;
      mtlk_core_ui_encext_cfg_t encext_cfg;

      memset(&encext_cfg, 0, sizeof(encext_cfg));
      encext_cfg.key_len = core->slow_ctx->keys[key_index].key_len;
      encext_cfg.key_idx = key_index;
      wave_memcpy(encext_cfg.key, sizeof(encext_cfg.key), core->slow_ctx->keys[key_index].key, encext_cfg.key_len);
      encext_cfg.alg_type = IW_ENCODE_ALG_WEP;
      encext_cfg.sta = sta;
      ieee_addr_set(&encext_cfg.sta_addr, sta->addr);

      res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
          WAVE_CORE_REQ_SET_ENCEXT_CFG, &clpb, (char*)&encext_cfg, sizeof(encext_cfg));
      res = _mtlk_df_user_process_core_retval_void(res, clpb,
          WAVE_CORE_REQ_SET_ENCEXT_CFG, TRUE);
    }
  }

  mtlk_sta_decref(sta_p);
  return res;
}

static mtlk_error_t _wv_request_sta_disconnect_ap (mtlk_df_user_t *df_user,
    struct ieee80211_sta *sta)
{
  mtlk_clpb_t *clpb = NULL;
  mtlk_error_t res = MTLK_ERR_OK;

  ILOG2_SY("%s: request disconnect sta:%Y", mtlk_df_user_get_name(df_user), sta->addr);

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
      WAVE_CORE_REQ_AP_DISCONNECT_STA, &clpb, &sta, sizeof(sta));
  res = _mtlk_df_user_process_core_retval(res, clpb,
      WAVE_CORE_REQ_AP_DISCONNECT_STA, TRUE);

  return res;
}

static int _wv_request_change_station (mtlk_df_user_t *df_user,
    struct ieee80211_sta *sta, BOOL is_authorizing)
{
  mtlk_df_t* df =mtlk_df_user_get_df(df_user);
  uint8 * mac = sta->addr;
  mtlk_vap_handle_t vap_handle = mtlk_df_get_vap_handle(df);
  wave_radio_t *radio = wave_vap_radio_get(vap_handle);
  struct net_device *ndev = mtlk_df_user_get_ndev(df_user);
  struct station_parameters params;
  u32 sta_flags_mask = BIT(NL80211_STA_FLAG_AUTHORIZED);
  u32 sta_flags_set = 0;
  if (is_authorizing) {
    sta_flags_set = BIT(NL80211_STA_FLAG_AUTHORIZED);
  }
  params.sta_flags_set = sta_flags_set;
  params.sta_flags_mask = sta_flags_mask;
  return wave_radio_sta_change(radio, ndev, mac, &params, sta);
}

static int _wv_ieee80211_op_sta_state (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif,
    struct ieee80211_sta *sta,
    enum ieee80211_sta_state old_state,
    enum ieee80211_sta_state new_state)
{
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  mtlk_df_user_t *df_user = wv_ieee80211_vif_to_dfuser(vif);
  mtlk_vap_handle_t vap_handle;
  mtlk_core_t *core;
  mtlk_error_t ret = MTLK_ERR_OK;

  if (!df_user)
    return -EPERM;

  ILOG1_SYDD("%s: station %Y state change %d->%d", ieee80211_vif_to_name(vif),
             sta->addr, old_state, new_state);

  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  core = mtlk_vap_get_core(vap_handle);

  /* In case sta state has changed as a result of connection loss caused by FW reset
   * AP the sta is connected to is no longer in sta DB and there is no need to update
   * FW about station status changes */
  if (mtlk_core_rcvry_is_running(core) ||
      wave_rcvry_mac_fatal_pending_get(mtlk_vap_get_hw(core->vap_handle)) ||
      wv_iface_inf->is_initialized == FALSE) {

    if (mtlk_vap_is_sta(vap_handle))
      wv_iface_inf->current_state = new_state;

    if (new_state < old_state) {
      if (old_state == IEEE80211_STA_NONE && new_state == IEEE80211_STA_NOTEXIST) {
        /* this is needed for recovery */
        mtlk_sta_wait_delete(sta);
      }

      ILOG0_SY("%s: Station %Y state change while in recovery, no need to update FW",
               ieee80211_vif_to_name(vif), sta->addr);
      return LINUX_ERR_OK;
    } else {
      ELOG_SY("%s: Station %Y state change while in recovery",
              ieee80211_vif_to_name(vif), sta->addr);
      return -EBUSY;
    }
  }

  if (old_state == IEEE80211_STA_NOTEXIST && new_state == IEEE80211_STA_NONE) {
    sta_entry * sta_p;
    /* for stations connecting to the AP - sid is requested via
     * wave_radio.c - wave_radio_aid_get (through HOSTAPD)
     */
    if (mtlk_vap_is_sta(vap_handle)) {
      ret = _wv_request_sid(df_user, &wv_iface_inf->vif_sid, sta->addr);
      if (MTLK_ERR_OK != ret) {
        ELOG_SY("%s: requesting sid for station %Y failed",
                ieee80211_vif_to_name(vif), sta->addr);
        goto out;
      }
      ILOG1_SD("%s: received sid '%d' for station vap",
               ieee80211_vif_to_name(vif), wv_iface_inf->vif_sid);
    }
    sta_p = wv_ieee80211_get_sta(sta);
    mtlk_osal_event_init(&sta_p->ref_cnt_event);
    /*we dont use sta_decref here so the refcnt will start at 1 */

  } else if (old_state == IEEE80211_STA_NONE && new_state == IEEE80211_STA_AUTH) {
  } else if (old_state == IEEE80211_STA_AUTH && new_state == IEEE80211_STA_ASSOC) {
    ret = _wv_request_sta_connect_ap(df_user, hw, vif, sta);
    if (MTLK_ERR_OK != ret) {
      ELOG_SY("%s: request to connect station %Y failed",
              ieee80211_vif_to_name(vif), sta->addr);
      goto out;
    }
  } else if (old_state == IEEE80211_STA_ASSOC && new_state == IEEE80211_STA_AUTHORIZED) {
    if (mtlk_vap_is_sta(vap_handle)) {
      /* change state of the channel to DFS AVAILIABLE so that AP vifs will be
       * able to be ENABLED into the DFS channel without the need of CAC. */
      cfg80211_set_dfs_state(hw->wiphy, &vif->bss_conf.chandef, NL80211_DFS_AVAILABLE);
    }
    ret = _wv_request_change_station(df_user, sta, TRUE);
    if (ret) { /* Linux error code */
      ELOG_SY("%s: request to change state of station %Y failed",
              ieee80211_vif_to_name(vif), sta->addr);
      return ret;
    }
  } else if (old_state == IEEE80211_STA_AUTHORIZED && new_state == IEEE80211_STA_ASSOC) {
    ret = _wv_request_change_station(df_user, sta, FALSE);
    if (ret) { /* Linux error code */
      ELOG_SY("%s: request to change state of station %Y failed",
              ieee80211_vif_to_name(vif), sta->addr);
      return ret;
    }
  } else if (old_state == IEEE80211_STA_ASSOC && new_state == IEEE80211_STA_AUTH) {
    ret = _wv_request_sta_disconnect_ap(df_user, sta);
    if (MTLK_ERR_OK != ret) {
      ELOG_SY("%s: request to disconnect station %Y failed",
              ieee80211_vif_to_name(vif), sta->addr);
      goto out;
    }
  } else if (old_state == IEEE80211_STA_AUTH && new_state == IEEE80211_STA_NONE) {
  } else if (old_state == IEEE80211_STA_NONE && new_state == IEEE80211_STA_NOTEXIST) {
    if (mtlk_vap_is_sta(vap_handle)) {
      ILOG1_SD("%s: releasing sid '%d'",
               ieee80211_vif_to_name(vif), wv_iface_inf->vif_sid);
      ret = _wv_release_sid(df_user, wv_iface_inf->vif_sid);
      if (MTLK_ERR_OK != ret) {
        ELOG_S("%s: wv_release_sid failed", ieee80211_vif_to_name(vif));
      }
    }
    mtlk_sta_wait_delete(sta);
  } else {
    return -EIO;
  }

  if (mtlk_vap_is_sta(vap_handle))
    wv_iface_inf->current_state = new_state;

out:
  return _mtlk_df_mtlk_to_linux_error_code(ret);
}

static int _wv_ieee80211_op_set_key (struct ieee80211_hw *hw,
    enum set_key_cmd cmd,
    struct ieee80211_vif *vif,
    struct ieee80211_sta *sta,
    struct ieee80211_key_conf *key)
{
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  mtlk_df_user_t *df_user = wv_iface_inf->df_user;
  BOOL pairwise = (key->flags & IEEE80211_KEY_FLAG_PAIRWISE) != 0;
  mtlk_clpb_t *clpb = NULL;
  mtlk_core_ui_encext_cfg_t encext_cfg;
  mtlk_error_t ret = MTLK_ERR_OK;
  mtlk_vap_handle_t vap_handle;
  mtlk_core_t *core;
  struct ieee80211_key_seq seq;
  u32 iv32;
  u16 iv16;
  BOOL isBIGTK;

  if (df_user == NULL) {
    ELOG_V("df_user is NULL");
    return -EPERM;
  }

  if (key->keyidx < 0 || key->keyidx >= WAVE_NUM_KEYS) {
    ELOG_D("Invalid key index (%d)", key->keyidx);
    return -EINVAL;
  }

  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  core = mtlk_vap_get_core(vap_handle);
  if (mtlk_core_rcvry_is_running(core) ||
      wave_rcvry_mac_fatal_pending_get(mtlk_vap_get_hw(core->vap_handle))) {
    ELOG_V("Recovery is running, key should not be set in FW");
    return LINUX_ERR_OK;
  }
  ieee80211_get_key_rx_seq(key, 0, &seq);

  isBIGTK = (key->keyidx == WAVE_KEY_IDX6_BIGTK || key->keyidx == WAVE_KEY_IDX7_BIGTK);

  switch (cmd) {
  case SET_KEY:
    ILOG2_V("SET_KEY");
    if ((key->keyidx >= WAVE_KEY_IDX4_IGTK && !isBIGTK)) {
      ILOG2_D("Invalid key index %d ignored", key->keyidx);
      ret = MTLK_ERR_OK;
      goto out;
    }

    memset(&encext_cfg, 0, sizeof(encext_cfg));
    key->hw_key_idx = key->keyidx;

    ILOG2_DDDDDDD("cipher = %d, icv_len = %d, iv_len = %d, hw_key_idx = %d, flags = %d, keyidx = %d, keylen = %d",
        key->cipher, key->icv_len, key->iv_len, key->hw_key_idx,
        key->flags, key->keyidx, key->keylen);

    /* Validate pairwise flag */
    if (!sta) {
      if (pairwise) {
        WLOG_V("MAC_ADDR is not defined and pairwise is set");
        ret = MTLK_ERR_PARAMS;
        goto out;
      }

      ILOG2_Y("mac_addr = %Y", _bcast_addr.au8Addr);
      encext_cfg.sta_addr = _bcast_addr;
    } else {
      ILOG2_Y("mac_addr = %Y", sta->addr);
      mtlk_osal_copy_eth_addresses(encext_cfg.sta_addr.au8Addr, sta->addr);
    }

    if (!_wv_ieee80211_supported_cipher_suite(hw->wiphy, key->cipher)) {
      ELOG_D("Cipher 0x%08X is not supported on this platform", key->cipher);
      ret = MTLK_ERR_NOT_SUPPORTED;
      goto out;
    }

    switch (key->cipher) {
    case WLAN_CIPHER_SUITE_WEP40:
    case WLAN_CIPHER_SUITE_WEP104:
      /* Validate WEP key */
      if (mtlk_df_ui_validate_wep_key(key->key, key->keylen)
          != MTLK_ERR_OK) {
        ELOG_V("Invalid WEP key");
        ret = MTLK_ERR_PARAMS;
        goto out;
      }

      /* Validate key index */
      if (key->keyidx >= WAVE_WEP_NUM_DEFAULT_KEYS) {
        ELOG_D("Invalid WEP key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }

      encext_cfg.alg_type = IW_ENCODE_ALG_WEP;
      break;

    case WLAN_CIPHER_SUITE_TKIP:
      /* Validate key index */
      if ((key->keyidx > 2) || (pairwise && (key->keyidx != 0))) {
        ELOG_D("Invalid TKIP key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }
      iv16 = seq.tkip.iv16;
      iv32 = seq.tkip.iv32;
      encext_cfg.rx_seq[0] = iv16 & 0xff;
      encext_cfg.rx_seq[1] = (iv16 >> 8) & 0xff;
      encext_cfg.rx_seq[2] = iv32 & 0xff;
      encext_cfg.rx_seq[3] = (iv32 >> 8) & 0xff;
      encext_cfg.rx_seq[4] = (iv32 >> 16) & 0xff;
      encext_cfg.rx_seq[5] = (iv32 >> 24) & 0xff;

      encext_cfg.alg_type = IW_ENCODE_ALG_TKIP;
      break;

    case WLAN_CIPHER_SUITE_CCMP:
      /* Validate key index */
      if ((key->keyidx > 2) || (pairwise && (key->keyidx != 0))) {
        ELOG_D("Invalid CCMP key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }
      encext_cfg.rx_seq[0] = seq.ccmp.pn[5];
      encext_cfg.rx_seq[1] = seq.ccmp.pn[4];
      encext_cfg.rx_seq[2] = seq.ccmp.pn[3];
      encext_cfg.rx_seq[3] = seq.ccmp.pn[2];
      encext_cfg.rx_seq[4] = seq.ccmp.pn[1];
      encext_cfg.rx_seq[5] = seq.ccmp.pn[0];

      encext_cfg.alg_type = IW_ENCODE_ALG_CCMP;
      break;

    case WLAN_CIPHER_SUITE_AES_CMAC:
      /* Validate key index */
      if (((key->keyidx > 2) && !isBIGTK) || (pairwise && (key->keyidx != 0))) {
        ELOG_D("Invalid AES_CMAC key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }
      encext_cfg.rx_seq[0] = seq.aes_cmac.pn[5];
      encext_cfg.rx_seq[1] = seq.aes_cmac.pn[4];
      encext_cfg.rx_seq[2] = seq.aes_cmac.pn[3];
      encext_cfg.rx_seq[3] = seq.aes_cmac.pn[2];
      encext_cfg.rx_seq[4] = seq.aes_cmac.pn[1];
      encext_cfg.rx_seq[5] = seq.aes_cmac.pn[0];

      encext_cfg.alg_type = IW_ENCODE_ALG_AES_CMAC;
      break;

    case WLAN_CIPHER_SUITE_GCMP:
      /* Validate key index */
      if ((key->keyidx > 2) || (pairwise && (key->keyidx !=0))) {
        ELOG_D("Invalid GCMP key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }
      encext_cfg.rx_seq[0] = seq.gcmp.pn[5];
      encext_cfg.rx_seq[1] = seq.gcmp.pn[4];
      encext_cfg.rx_seq[2] = seq.gcmp.pn[3];
      encext_cfg.rx_seq[3] = seq.gcmp.pn[2];
      encext_cfg.rx_seq[4] = seq.gcmp.pn[1];
      encext_cfg.rx_seq[5] = seq.gcmp.pn[0];

      encext_cfg.alg_type = IW_ENCODE_ALG_GCMP;
      break;

    case WLAN_CIPHER_SUITE_GCMP_256:
      /* Validate key index */
      if ((key->keyidx > 2) || (pairwise && (key->keyidx !=0))) {
        ELOG_D("Invalid GCMP key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }
      encext_cfg.rx_seq[0] = seq.gcmp.pn[5];
      encext_cfg.rx_seq[1] = seq.gcmp.pn[4];
      encext_cfg.rx_seq[2] = seq.gcmp.pn[3];
      encext_cfg.rx_seq[3] = seq.gcmp.pn[2];
      encext_cfg.rx_seq[4] = seq.gcmp.pn[1];
      encext_cfg.rx_seq[5] = seq.gcmp.pn[0];

      encext_cfg.alg_type = IW_ENCODE_ALG_GCMP_256;
      break;

    case WLAN_CIPHER_SUITE_BIP_GMAC_128:
      /* Validate key index */
      if (((key->keyidx > 2) && !isBIGTK) || (pairwise && (key->keyidx != 0))) {
        ELOG_D("Invalid GCMP key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }
      encext_cfg.rx_seq[0] = seq.aes_gmac.pn[5];
      encext_cfg.rx_seq[1] = seq.aes_gmac.pn[4];
      encext_cfg.rx_seq[2] = seq.aes_gmac.pn[3];
      encext_cfg.rx_seq[3] = seq.aes_gmac.pn[2];
      encext_cfg.rx_seq[4] = seq.aes_gmac.pn[1];
      encext_cfg.rx_seq[5] = seq.aes_gmac.pn[0];

      encext_cfg.alg_type = IW_ENCODE_ALG_BIP_GMAC_128;
      break;

    case WLAN_CIPHER_SUITE_BIP_GMAC_256:
      /* Validate key index */
      if (((key->keyidx > 2) && !isBIGTK) || (pairwise && (key->keyidx != 0))) {
        ELOG_D("Invalid GCMP key index %d", key->keyidx);
        ret = MTLK_ERR_PARAMS;
        goto out;
      }
      encext_cfg.rx_seq[0] = seq.aes_gmac.pn[5];
      encext_cfg.rx_seq[1] = seq.aes_gmac.pn[4];
      encext_cfg.rx_seq[2] = seq.aes_gmac.pn[3];
      encext_cfg.rx_seq[3] = seq.aes_gmac.pn[2];
      encext_cfg.rx_seq[4] = seq.aes_gmac.pn[1];
      encext_cfg.rx_seq[5] = seq.aes_gmac.pn[0];

      encext_cfg.alg_type = IW_ENCODE_ALG_BIP_GMAC_256;
      break;

    default:
      ret = MTLK_ERR_PARAMS;
      goto out;
    }

    /* Validate and set the key length */
    if ((WAVE_MAX_KEY_LEN < key->keylen) || (key->keylen == 0)) {
      WLOG_D("Invalid key length %u", key->keylen);
      ret = MTLK_ERR_PARAMS;
      goto out;
    }

    encext_cfg.key_len = key->keylen;
    encext_cfg.key_idx = key->keyidx;
    wave_memcpy(encext_cfg.key, sizeof(encext_cfg.key), key->key, key->keylen);
    encext_cfg.sta = sta;

    if (isBIGTK) {
      if(MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_MBSSID_VAP) > WAVE_RADIO_OPERATION_MODE_MBSSID_TRANSMIT_VAP) {
        ILOG1_D("CID-%04x: Not pushing OPERATION_MODE_MBSSID_NON_TRANSMIT_VAP", mtlk_vap_get_oid(core->vap_handle));
        ret = MTLK_ERR_OK;
        goto out;
      } else {
        ILOG1_D("CID-%04x: Pushing OPERATION_MODE_MBSSID_TRANSMIT_VAP", mtlk_vap_get_oid(core->vap_handle));
      }
    }

    ret = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
        WAVE_CORE_REQ_SET_ENCEXT_CFG, &clpb, (char*) &encext_cfg,
        sizeof(encext_cfg));
    ret = _mtlk_df_user_process_core_retval_void(ret, clpb,
        WAVE_CORE_REQ_SET_ENCEXT_CFG, TRUE);

    /* when using wep this callback is called only once
     * because it's the same key for unicast and broadcast.
     * but we need to notify it twice to the FW, with broadcast mac address
     * and the peer ap mac address.
     */
    if ((ret == MTLK_ERR_OK) && (encext_cfg.alg_type == IW_ENCODE_ALG_WEP) && (!(mtlk_vap_is_ap(vap_handle)))) {
      if (sta) {
        ieee_addr_set(&encext_cfg.sta_addr, sta->addr);
      } else {
        sta_entry *peer_ap = mtlk_stadb_get_ap(mtlk_core_get_stadb(core));
        if (!peer_ap) {
          ELOG_D("CID-%04x: ap sta_entry not found", mtlk_vap_get_oid(core->vap_handle));
          ret = MTLK_ERR_PARAMS;
          goto out;
        }
        ieee_addr_set(&encext_cfg.sta_addr, mtlk_sta_get_addr(peer_ap)->au8Addr);
        mtlk_sta_decref(peer_ap);
      }
      encext_cfg.sta = NULL;
      ret = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
          WAVE_CORE_REQ_SET_ENCEXT_CFG, &clpb, (char*) &encext_cfg,
          sizeof(encext_cfg));
      ret = _mtlk_df_user_process_core_retval_void(ret, clpb,
          WAVE_CORE_REQ_SET_ENCEXT_CFG, TRUE);
    }

    break;

  case DISABLE_KEY:
    /* FW doesn't support delete key */
    ILOG2_V("DISABLE_KEY");
    break;

  default:
    ELOG_D("Unsupported key cmd %d", cmd);
    ret = MTLK_ERR_PARAMS;
    goto out;
  }

out:
  return _mtlk_df_mtlk_to_linux_error_code(ret);
}

static void _wv_ieee80211_set_default_key (struct ieee80211_hw *hw, struct ieee80211_vif *vif, int idx)
{
  mtlk_error_t res = MTLK_ERR_PARAMS;
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  mtlk_df_user_t *df_user = wv_iface_inf->df_user;
  mtlk_clpb_t *clpb = NULL;
  mtlk_core_ui_default_key_cfg_t default_key_cfg;
  mtlk_vap_handle_t vap_handle;
  mtlk_core_t *core;

  ILOG1_SD("%s: key index:%d", ieee80211_vif_to_name(vif), idx);
  if (df_user == NULL) {
    ELOG_V("df_user is NULL");
    return;
  }

  /* Unsetting default key using idx = -1 isn't supported */
  if (idx == -1)
    return;

  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  core = mtlk_vap_get_core(vap_handle);

  if (_wait_rcvry_completion_if_needed(core) != MTLK_ERR_OK) {
    ELOG_V("Recovery is running, can't set default unicast key");
    return;
  }

  /*check default key index */
  if ((idx >= WAVE_WEP_NUM_DEFAULT_KEYS) && (idx != 6 && idx != 7)) {
    ILOG2_D("Invalid WEP key index %d", idx);
    return;
  }

  memset(&default_key_cfg, 0, sizeof(default_key_cfg));
  default_key_cfg.sta_addr = _bcast_addr;
  default_key_cfg.key_idx = idx;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
      WAVE_CORE_REQ_SET_DEFAULT_KEY_CFG, &clpb,
      (char*)&default_key_cfg, sizeof(default_key_cfg));
  res = _mtlk_df_user_process_core_retval_void(res, clpb,
      WAVE_CORE_REQ_SET_DEFAULT_KEY_CFG, TRUE);
}

static void _wv_ieee80211_op_set_default_unicast_key (struct ieee80211_hw *hw, struct ieee80211_vif *vif, int idx)
{
  _wv_ieee80211_set_default_key(hw, vif, idx);
}

static void _wv_ieee80211_op_set_default_multicast_key (struct ieee80211_hw *hw, struct ieee80211_vif *vif, int idx)
{
  _wv_ieee80211_set_default_key(hw, vif, idx);
}

static void _wv_ieee80211_op_get_key_seq_by_vif (struct ieee80211_vif *vif,
    struct ieee80211_key_conf *key,
    struct ieee80211_key_seq *seq)
{
  struct nic * core = wv_ieee80211_vif_to_core(vif);
  uint8 * core_seq = NULL;

  if (!core)
    return;

  core_seq = core->slow_ctx->seq;

  mtlk_dump(2, core_seq, UMI_RSN_SEQ_NUM_LEN, "GROUP RSC");

  switch (key->cipher) {
  case WLAN_CIPHER_SUITE_TKIP:
    seq->tkip.iv16 = (core_seq[1] << 8) | (core_seq[0]);
    seq->tkip.iv32 = (core_seq[5] << 24) | (core_seq[4] << 16)
        | (core_seq[3] << 8) | (core_seq[2]);
    break;
  case WLAN_CIPHER_SUITE_CCMP:
  case WLAN_CIPHER_SUITE_CCMP_256:
  case WLAN_CIPHER_SUITE_AES_CMAC:
  case WLAN_CIPHER_SUITE_BIP_CMAC_256:
  case WLAN_CIPHER_SUITE_BIP_GMAC_128:
  case WLAN_CIPHER_SUITE_BIP_GMAC_256:
  case WLAN_CIPHER_SUITE_GCMP:
  case WLAN_CIPHER_SUITE_GCMP_256:
    /*might need to be inserted backwards, like in set key
     * for now we will assume there is no need for that */
    wave_memcpy(seq->ccmp.pn, sizeof(seq->ccmp.pn), core_seq, UMI_RSN_SEQ_NUM_LEN);
    break;
  default:
    wave_memcpy(seq->hw.seq, sizeof(seq->hw.seq), core_seq, UMI_RSN_SEQ_NUM_LEN);
    seq->hw.seq_len = UMI_RSN_SEQ_NUM_LEN;
  }
}

static void _wv_ieee80211_op_pre_get_key_seq_by_vif (struct ieee80211_vif *vif)
{
  struct nic * core = wv_ieee80211_vif_to_core(vif);
  mtlk_df_user_t *df_user;
  mtlk_clpb_t *clpb = NULL;
  mtlk_error_t res = MTLK_ERR_OK;

  if (!core)
   return;

  df_user = wv_ieee80211_vif_to_dfuser(vif);
  if (!df_user)
    return;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),WAVE_CORE_REQ_GET_ENCEXT_CFG, &clpb, NULL, 0);
  _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_CORE_REQ_GET_ENCEXT_CFG, TRUE);
}

static int _wv_ieee80211_op_set_frag_threshold (struct ieee80211_hw *hw, u32 value)
{
  ILOG0_V("TODO: mac80211 callback _wv_ieee80211_op_set_frag_threshold");
  return LINUX_ERR_OK;
}

static int _wv_ieee80211_op_conf_tx (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif, u16 ac,
    const struct ieee80211_tx_queue_params *params)
{
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_df_user_t *df_user = wv_iface_inf->df_user;
  mtlk_df_t *master_df;
  mtlk_clpb_t *clpb = NULL;
  mtlk_vap_handle_t vap_handle;
  mtlk_pdb_t *param_db_core;
  struct mtlk_txq_params tp;
  uint8 vap_id;

  if (df_user == NULL) {
    ELOG_V("df_user is NULL");
    return -EPERM;
  }
  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  param_db_core = mtlk_vap_get_param_db(vap_handle);
  tp.txop = params->txop;
  tp.cwmin = params->cw_min;
  tp.cwmax = params->cw_max;
  tp.aifs = params->aifs;
  tp.acm_flag = params->acm;
  tp.ac = nlac2mtlkac(ac);

  ILOG2_DDD("CID-%04x: Saving WMM params for VapID %u queue %u",
            mtlk_vap_get_oid(vap_handle), wv_iface_inf->vap_index, tp.ac);
  res = wave_pdb_set_binary(param_db_core, PARAM_DB_CORE_WMM_PARAMS_BE + tp.ac, &tp, sizeof(tp));

  if (res != MTLK_ERR_OK)
    goto end;

  if (tp.ac != UMI_AC_BK)
    goto end;

  master_df = mtlk_df_user_get_master_df(df_user);
  vap_id = mtlk_vap_get_id(vap_handle);

  if (!wv_iface_inf->is_initialized) {
    ILOG1_D("vap %d is not initialized, not setting wmm", vap_id);
    goto end;
  }

  res = _mtlk_df_user_invoke_core(master_df, WAVE_CORE_REQ_SET_WMM_PARAM, &clpb, &vap_id, sizeof(vap_id));
  res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_CORE_REQ_SET_WMM_PARAM, TRUE);

end:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _wv_ieee80211_op_get_connection_alive (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif)
{
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  mtlk_df_user_t *df_user = wv_iface_inf->df_user;
  uint32 size;
  mtlk_clpb_t *clpb = NULL;
  wave_connection_alive_t *conn_alive = NULL;
  int ret = 0;
  mtlk_error_t mtlk_res;
  unsigned long total_rx_and_tx_uni_packets = 0;

  if (df_user == NULL) {
    ELOG_V("df_user is NULL");
    return 0;
  }

  mtlk_res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user), WAVE_CORE_REQ_GET_CONNECTION_ALIVE, &clpb, NULL, 0);
  mtlk_res = _mtlk_df_user_process_core_retval_void(mtlk_res, clpb, WAVE_CORE_REQ_GET_CONNECTION_ALIVE, FALSE);

  if (MTLK_ERR_OK != mtlk_res) {
    ELOG_D("Core invocation to get connection alive failed, res %d", mtlk_res);
    return 0;
  }

  conn_alive = mtlk_clpb_enum_get_next(clpb, &size);
  MTLK_CLPB_TRY(conn_alive, size)
    total_rx_and_tx_uni_packets += conn_alive->tx_packets;
    total_rx_and_tx_uni_packets += conn_alive->rx_packets;

    if (total_rx_and_tx_uni_packets - wv_iface_inf->ndp_counter > wv_iface_inf->latest_rx_and_tx_packets)
      ret = 1;

    /* If 0 is returned, mac80211 will send probe req to the AP.
     * In case scan just ended, let mac80211 probe the AP,
     * as mac80211 does not detect if AP is still on our
     * channel after the scan, and also our FW Unicast TX/RX
     * counters do not seem to be doing the job: they get
     * incremented although AP already has left the channel */
    if (conn_alive->probe_ap_after_scan)
      ret = 0;

    ILOG1_DDDDD("VapID=%u, total_rx_and_tx_unicast_packets=%u, latest_rx_and_tx_packets=%u, ndp_counter=%d, res=%d",
                wv_iface_inf->vap_index, total_rx_and_tx_uni_packets, wv_iface_inf->latest_rx_and_tx_packets, wv_iface_inf->ndp_counter, ret);

    wv_iface_inf->latest_rx_and_tx_packets = total_rx_and_tx_uni_packets;
    wv_iface_inf->ndp_counter = 0;

  MTLK_CLPB_FINALLY(mtlk_res)
    mtlk_clpb_delete(clpb);
  MTLK_CLPB_END;

  if (MTLK_ERR_OK != mtlk_res) {
    ELOG_D("Failed to get connection alive, res %d", mtlk_res);
    return 0;
  }

  return ret;
}

static void _wv_ieee80211_op_bss_info_changed (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif, struct ieee80211_bss_conf *info, u32 changed)
{
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);
  mtlk_df_user_t *df_user = wv_iface_inf->df_user;
  struct net_device *ndev;
  mtlk_error_t res;
  mtlk_vap_handle_t vap_handle;
  mtlk_core_t *core;
  mtlk_df_t *master_df;
  struct mtlk_sta_bss_change_parameters bss_change_params;
  mtlk_clpb_t *clpb = NULL;
  wv_mac80211_t * mac80211;

  ILOG1_SD("Setting BSS info for %s. changed = 0x%x",
           ieee80211_vif_to_name(vif), changed);

  mac80211 = __wv_ieee80211_hw_get_mac80211(hw);
  WAVE_WV_CHECK_PTR_VOID(mac80211);

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_VOID(master_df);

  /* Ignore QOS changes duo to mac80211 notifies about them per queue,
   * and our FW receives WMM for all queues at once.
   * Send WMM to FW in op_conf_tx when the last queue wmm arrives. */
  if (changed & BSS_CHANGED_QOS) {
    changed &= ~BSS_CHANGED_QOS;
    if (!changed)
      return;
  }

  /* Ignore beacon template changes during CSA, since our FW updates
   * channel and CSA IE. mac80211 will update beacon again once CSA
   * is done
   */
  if (vif->csa_active && ((changed & BSS_CHANGED_BEACON) ||
      (changed & BSS_CHANGED_AP_PROBE_RESP))) {
    ILOG2_S("%s: CSA in progress. FW manages CSA in beacon.",
            ieee80211_vif_to_name(vif));
    changed &= ~BSS_CHANGED_BEACON;
    changed &= ~BSS_CHANGED_AP_PROBE_RESP;
    if (!changed)
      return;
  }

  if (df_user == NULL) {
    ELOG_V("df_user is NULL");
    return;
  }
  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
  core = mtlk_vap_get_core(vap_handle);

  if (_wait_rcvry_completion_if_needed(core) != MTLK_ERR_OK) {
    ELOG_V("Recovery is running, can't change bss info");
    return;
  }

  ndev = mtlk_df_user_get_ndev(df_user);

  if ((changed & BSS_CHANGED_BEACON) ||
      (changed & BSS_CHANGED_AP_PROBE_RESP)) {
    struct _wv_beacon_cfg wv_beacon_cfg;

    res = _wv_ieee80211_get_beacon_cfg_data(hw, vif, &wv_beacon_cfg);
    if (res != MTLK_ERR_OK) {
      ELOG_SD("%s: failed to retrive beacon template. res =%d",
              ieee80211_vif_to_name(vif), res);
      return;
    }

    res = wave_radio_beacon_change(radio, ndev, &wv_beacon_cfg.beacon_data);
    _wv_ieee80211_free_beacon_cfg_data(&wv_beacon_cfg);
    if (res != MTLK_ERR_OK) {
      ELOG_SD("%s: failed to change beacon. res =%d",
              ieee80211_vif_to_name(vif), res);
      return;
    }
  }

  if (!(mtlk_vap_is_sta(vap_handle) && (changed & BSS_CHANGED_BEACON_INT)) &&
      !(changed & (BSS_CHANGED_ERP_CTS_PROT | BSS_CHANGED_ERP_PREAMBLE |
                   BSS_CHANGED_ERP_SLOT | BSS_CHANGED_BASIC_RATES |
                   BSS_CHANGED_AP_ISOLATE))) {
    return;
  }

  memset(&bss_change_params, 0, sizeof(bss_change_params));
  bss_change_params.vif_name = ieee80211_vif_to_name(vif);
  bss_change_params.bands = hw->wiphy->bands;
  bss_change_params.core = core;
  bss_change_params.info = info;
  bss_change_params.changed = changed;
  bss_change_params.vap_index = wv_iface_inf->vap_index;

  /* Keep trying in case scan is in progress for the current radio */
  {
    int retry_counter = 0;

    do {
      res = _mtlk_df_user_invoke_core(master_df,
                WAVE_RADIO_REQ_CHANGE_BSS, &clpb, &bss_change_params, sizeof(bss_change_params));
      res = _mtlk_df_user_process_core_retval(res, clpb,
                WAVE_RADIO_REQ_CHANGE_BSS, TRUE);

      if (MTLK_ERR_RETRY != res) break;
      mtlk_osal_msleep(50);
      retry_counter++;
    } while ((MTLK_ERR_RETRY == res) && (retry_counter < MAX_SCAN_WAIT_RETRIES));

    if (retry_counter > 0)
      ILOG0_SD("%s: Scan waited, number of retries %d", ieee80211_vif_to_name(vif), retry_counter);
  }
}

static int _wv_ieee80211_op_hw_scan (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif,
    struct ieee80211_scan_request *scan_req)
{
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);
  struct wireless_dev *wdev = ieee80211_vif_to_wdev(vif);

  if (!wdev) {
    ELOG_S("%s: failed to retrieve wdev", ieee80211_vif_to_name(vif));
    return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_UNKNOWN);
  }

  return wave_radio_scan(radio, wdev->netdev, &scan_req->req);
}

static void _wv_ieee80211_op_cancel_hw_scan (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif)
{
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);
  wave_radio_cancel_hw_scan(radio);
}

static int _wv_ieee80211_op_set_qos_map (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif, struct cfg80211_qos_map *qos_map)
{
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);
  struct wireless_dev *wdev = ieee80211_vif_to_wdev(vif);

  if (!wdev) {
    ELOG_S("%s: failed to retrieve wdev", ieee80211_vif_to_name(vif));
    return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_UNKNOWN);
  }

  return wave_radio_qos_map_set(radio, wdev->netdev, qos_map);
}

/* Setting up passive scan flag on radar channels */
static void _wv_ieee80211_mark_ch_for_passive_scan (struct wiphy *wiphy)
{
  struct ieee80211_supported_band *sband;
  struct ieee80211_channel *ch;
  int i;

  if (!wiphy->bands[NL80211_BAND_5GHZ])
    return;

  sband = wiphy->bands[NL80211_BAND_5GHZ];

  for (i = 0; i < sband->n_channels; ++i) {
    ch = &sband->channels[i];
    if (!_wv_ieee80211_is_radar_chan(ch) ||
        (ch->flags & IEEE80211_CHAN_DISABLED))
      continue;

    ch->flags |= IEEE80211_CHAN_NO_IR;
  }
}


static void _wv_ieee80211_reg_notifier (struct wiphy *wiphy, struct regulatory_request *request)
{
  struct ieee80211_hw *hw = wiphy_to_ieee80211_hw(wiphy);
  wave_radio_t        *radio = wv_ieee80211_hw_radio_get(hw);

  /* set new country code by chars array (NOT NUL-terminated) */
  wave_radio_country_code_set_by_str(radio, request->alpha2, sizeof(request->alpha2));
  /* Result already logged */

  /* Setting up passive scan on radar channels */
  _wv_ieee80211_mark_ch_for_passive_scan(wiphy);
}

void __MTLK_IFUNC
wv_ieee80211_unregister_if_needed (wv_mac80211_t *mac80211)
{
  MTLK_ASSERT(mac80211 != NULL);
  if (mac80211 == NULL)
    return;

  if (mac80211->registered) {
    mac80211->registered = FALSE;
    ILOG0_V("Unregistering mac80211 HW");
    ieee80211_unregister_hw(mac80211->mac80211_hw);
  }

}

static void _cfg80211_if_limit_cleanup (struct ieee80211_iface_limit *if_limit)
{
  if (NULL != if_limit)
  {
    mtlk_osal_mem_free(if_limit);
  }
}

static void _cfg80211_if_comb_cleanup (struct ieee80211_iface_combination *if_comb)
{
  if (NULL != if_comb)
  {
    struct ieee80211_iface_limit *if_limit;
    if_limit = (struct ieee80211_iface_limit *) if_comb->limits;

    _cfg80211_if_limit_cleanup(if_limit);
    mtlk_osal_mem_free(if_comb);
  }
}

static struct ieee80211_iface_limit *
_create_cfg80211_if_limits (wave_radio_t *radio)
{
  struct ieee80211_iface_limit *if_limits;
  size_t size = sizeof(struct ieee80211_iface_limit);

  if_limits = mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_MAC80211);
  if (NULL == if_limits)
  {
    ELOG_V("create_cfg80211_if_limits: can't allocate memory");
    return NULL;
  }

  memset(if_limits, 0, size);

  if_limits->max = wave_radio_max_vaps_get(radio);
  if_limits->types = BIT(NL80211_IFTYPE_STATION) | BIT(NL80211_IFTYPE_AP);

  return if_limits;
}

static struct ieee80211_iface_combination *
_create_cfg80211_if_comb (mtlk_handle_t hw_handle, wave_radio_t *radio)
{
  struct ieee80211_iface_combination *if_comb;
  struct ieee80211_iface_limit *if_limits;

  if_limits = _create_cfg80211_if_limits(radio);
  if (NULL == if_limits)
  {
    return NULL;
  }

  if_comb = mtlk_osal_mem_alloc(sizeof(struct ieee80211_iface_combination), MTLK_MEM_TAG_MAC80211);
  if (NULL == if_comb)
  {
    ELOG_V("create_cfg80211_if_comb: can't allocate memory");
    _cfg80211_if_limit_cleanup(if_limits);
    return NULL;
  }
  memset(if_comb, 0, sizeof(struct ieee80211_iface_combination));

  if_comb->limits = if_limits;
  if_comb->n_limits = 1;
  if_comb->max_interfaces = wave_radio_max_vaps_get(radio);

  if_comb->num_different_channels = 1;

  if_comb->beacon_int_infra_match = 0;
  if_comb->beacon_int_min_gcd = 1;
  if_comb->radar_detect_widths =   (1 << NL80211_CHAN_WIDTH_20_NOHT)
                                 | (1 << NL80211_CHAN_WIDTH_20)
                                 | (1 << NL80211_CHAN_WIDTH_40)
                                 | (1 << NL80211_CHAN_WIDTH_80)
                                 | (1 << NL80211_CHAN_WIDTH_160);
  return if_comb;
}

static void
_wv_mac80211_disable_hw_unsupported_channels (mtlk_hw_t *hw, struct ieee80211_supported_band *supported_band)
{
  int i;
  struct ieee80211_channel *chan;
  MTLK_ASSERT(supported_band);

  for (i = 0; i < supported_band->n_channels; i++) {
    chan = &supported_band->channels[i];
    if (!wave_hw_is_channel_supported(hw, chan->hw_value)) {
      ILOG1_D("Unsupported chan %u", chan->hw_value);
      chan->flags |= IEEE80211_CHAN_DISABLED;
    }
  }
}

static void
_set_tx_bf_cap (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, int num_sts)
{
  mtlk_hw_t *hw = HANDLE_T_PTR(mtlk_hw_t, hw_handle);

  if (mtlk_hw_type_is_gen6_a0(hw)) {
    cfg80211_band->ht_cap.tx_BF_cap_info = WAVE600_A0_HT_TX_BF_CAPA;
  } else { /* gen6_b0, gen6_d2, gen7 */
      if (num_sts == 2)
        cfg80211_band->ht_cap.tx_BF_cap_info = WAVE600_B0_D2_HT_TX_BF_CAPA;
      else if (num_sts == 4)
        cfg80211_band->ht_cap.tx_BF_cap_info = WAVE600_B0_D2_4X4_HT_TX_BF_CAPA;
  }
}

static void
_set_rx_antenna_pattern (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL rx_antenna_pattern = hw_get_rx_antenna_pattern_support(hw_handle);
  ILOG1_DS("RX_ANTENNA_PATTERN support is %d for %s band", (int)rx_antenna_pattern, nl_band_to_str[band]);
  if (rx_antenna_pattern)
    cfg80211_band->vht_cap.cap |= IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN;
  else
    cfg80211_band->vht_cap.cap &= ~IEEE80211_VHT_CAP_RX_ANTENNA_PATTERN;
}

static void
_set_ldpc_cap (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL ldpc_support = hw_get_ldpc_support(hw_handle);
  ILOG1_DS("LDPC support is %d for %s band", (int)ldpc_support, nl_band_to_str[band]);
  if (ldpc_support) {
    cfg80211_band->ht_cap.cap |= IEEE80211_HT_CAP_LDPC_CODING;
    cfg80211_band->vht_cap.cap |= IEEE80211_VHT_CAP_RXLDPC;
  } else {
    cfg80211_band->ht_cap.cap &= ~IEEE80211_HT_CAP_LDPC_CODING;
    cfg80211_band->vht_cap.cap &= ~IEEE80211_VHT_CAP_RXLDPC;
  }
}

static void
_set_ampdu_factor (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL ampdu_64k = hw_get_ampdu_64k_support(hw_handle);
  ILOG1_SS("AMPDU factor %s is supported for %s band", ampdu_64k ? "64k" : "32k", nl_band_to_str[band]);
  if (ampdu_64k)
    cfg80211_band->ht_cap.ampdu_factor = IEEE80211_HT_MAX_AMPDU_64K;
  else
    cfg80211_band->ht_cap.ampdu_factor = IEEE80211_HT_MAX_AMPDU_32K;
}

static void
_set_ampdu_density (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL ampdu_density = hw_get_ampdu_density_restriction(hw_handle);
  ILOG1_SS("AMPDU density has %s restriction for %s band", ampdu_density ? "16 usec" : "no", nl_band_to_str[band]);
  cfg80211_band->ht_cap.ampdu_density = ampdu_density ? IEEE80211_HT_MPDU_DENSITY_16 : IEEE80211_HT_MPDU_DENSITY_NONE;
}

static void
_set_rx_stbc (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL rx_stbc = hw_get_rx_stbc_support(hw_handle);
  ILOG1_SS("RX STBC %s supported for %s band", rx_stbc ? "is" : "not", nl_band_to_str[band]);
  cfg80211_band->ht_cap.cap &= ~IEEE80211_HT_CAP_RX_STBC;
  cfg80211_band->vht_cap.cap &= ~IEEE80211_VHT_CAP_RXSTBC_MASK;
  if (rx_stbc) {
    cfg80211_band->ht_cap.cap |= (WV_SUPP_RX_STBC << IEEE80211_HT_CAP_RX_STBC_SHIFT);
    cfg80211_band->vht_cap.cap |= IEEE80211_VHT_CAP_RXSTBC_1;
  }
}

static void
_set_vht_support (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL is_supported = hw_get_vht_support(hw_handle);

  ILOG1_SS("vht %s supported for %s band", is_supported ? "is" : "not", nl_band_to_str[band]);

  cfg80211_band->vht_cap.vht_supported = is_supported;
}

static void
_set_vht_80mhz_short_gi (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL vht_80mhz_short_gi_support = hw_get_80mhz_short_gi_support(hw_handle, band);

  ILOG1_SS("80Mhz Short GI %s supported for %s band", vht_80mhz_short_gi_support ? "is" : "not", nl_band_to_str[band]);
  if (vht_80mhz_short_gi_support) {
    cfg80211_band->vht_cap.cap |= IEEE80211_VHT_CAP_SHORT_GI_80;
  } else {
    cfg80211_band->vht_cap.cap &= ~IEEE80211_VHT_CAP_SHORT_GI_80;
  }
}

static void
_set_vht_160mhz_short_gi (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL vht_160mhz_short_gi_support = hw_get_160mhz_short_gi_support(hw_handle, band);

  ILOG1_SS("160Mhz Short GI %s supported for %s band", vht_160mhz_short_gi_support ? "is" : "not", nl_band_to_str[band]);
  if (vht_160mhz_short_gi_support) {
    cfg80211_band->vht_cap.cap |= IEEE80211_VHT_CAP_SHORT_GI_160;
  } else {
    cfg80211_band->vht_cap.cap &= ~IEEE80211_VHT_CAP_SHORT_GI_160;
  }
}

static void
_set_vht_cap_160mhz (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL vht_160_mhz = hw_get_vht_160mhz_support(hw_handle, band);
  ILOG1_SS("160Mhz %s supported for %s band", vht_160_mhz ? "is" : "not", nl_band_to_str[band]);
  if (vht_160_mhz) {
    cfg80211_band->vht_cap.cap |= IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ;
  } else {
    cfg80211_band->vht_cap.cap &= ~IEEE80211_VHT_CAP_SUPP_CHAN_WIDTH_160MHZ;
  }
}

static void
_set_he_support (mtlk_handle_t hw_handle, struct ieee80211_supported_band *cfg80211_band, enum nl80211_band band)
{
  BOOL is_supported = hw_get_he_support(hw_handle);

  ILOG1_SS("he %s supported for %s band", is_supported ? "is" : "not", nl_band_to_str[band]);

  cfg80211_band->he_cap.has_he = is_supported;
}

static void
_wv_mac80211_update_he_capa (mtlk_handle_t hw_handle,
                             struct ieee80211_supported_band **sband,
                             enum nl80211_band band, u32 num_sts)
{
  u32 idx;
  mtlk_hw_t *hw = HANDLE_T_PTR(mtlk_hw_t, hw_handle);
  BOOL gen6_d2_or_gen7 = mtlk_hw_type_is_gen6_d2_or_gen7(hw);

  if(num_sts > 4) {
    ELOG_D("Unsupported STS:%d", num_sts);
    num_sts = 2;
  }

  ILOG2_D("Using number of STS:%d", num_sts);

  /* idx 0 for STS 1 and 2, idx 1 for STS 3 and 4 */
  idx = (num_sts - 1) >> 1;

  if (band == NL80211_BAND_2GHZ) {
    if(gen6_d2_or_gen7) {
      MTLK_ASSERT(idx < ARRAY_SIZE(_mac80211_he_capa_2ghz_D2));
      (*sband)->iftype_data = &_mac80211_he_capa_2ghz_D2[idx];
    } else {
      MTLK_ASSERT(idx < ARRAY_SIZE(_mac80211_he_capa_2ghz_AB));
      (*sband)->iftype_data = &_mac80211_he_capa_2ghz_AB[idx];
    }
  } else if ((band == NL80211_BAND_5GHZ)) {
    if(gen6_d2_or_gen7) {
      MTLK_ASSERT(idx < ARRAY_SIZE(_mac80211_he_capa_5ghz_D2));
      (*sband)->iftype_data = &_mac80211_he_capa_5ghz_D2[idx];
    } else {
      MTLK_ASSERT(idx < ARRAY_SIZE(_mac80211_he_capa_5ghz_AB));
      (*sband)->iftype_data = &_mac80211_he_capa_5ghz_AB[idx];
    }
  }  else if ((band == NL80211_BAND_6GHZ)) {
    if(gen6_d2_or_gen7) {
      MTLK_ASSERT(idx < ARRAY_SIZE(_mac80211_he_capa_6ghz_D2));
      (*sband)->iftype_data = &_mac80211_he_capa_6ghz_D2[idx];
    } else {
      MTLK_ASSERT(idx < ARRAY_SIZE(_mac80211_he_capa_6ghz_AB));
      (*sband)->iftype_data = &_mac80211_he_capa_6ghz_AB[idx];
    }
  } else
    return;

  (*sband)->n_iftype_data = 1;
}


/**
 * Helper for wv_mac80211_init: initializes a band (2.4, 2.4_VHT or 5 GHz)
 */
static mtlk_error_t _wv_mac80211_init_band (
  mtlk_handle_t hw_handle,
  struct        ieee80211_supported_band** supported_band,
  struct        ieee80211_supported_band* supported_band_template,
  struct        ieee80211_channel* channel_template,
  int           n_channels, /* Number of channels in channel template */
  u32           num_sts,    /* Number of spatial streams */
  enum nl80211_band band)   /* Init Band */
{
  unsigned i;
  size_t size;
  mtlk_hw_t *hw = HANDLE_T_PTR(mtlk_hw_t, hw_handle);

  MTLK_ASSERT(hw);

  /* Allocate memory for struct ieee80211_supported_band and copy initial data from a template */
  size = sizeof(struct ieee80211_supported_band);
  *supported_band = mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_MAC80211);
  if (*supported_band == NULL)
    return MTLK_ERR_NO_MEM;

  wave_memcpy(*supported_band, size, supported_band_template, size);

  _set_tx_bf_cap           (hw_handle, *supported_band, num_sts);
  _set_ldpc_cap            (hw_handle, *supported_band, band);
  _set_ampdu_factor        (hw_handle, *supported_band, band);
  _set_ampdu_density       (hw_handle, *supported_band, band);
  _set_rx_stbc             (hw_handle, *supported_band, band);
  _set_rx_antenna_pattern  (hw_handle, *supported_band, band);
  _set_vht_support         (hw_handle, *supported_band, band);
  _set_vht_cap_160mhz      (hw_handle, *supported_band, band);
  _set_vht_80mhz_short_gi  (hw_handle, *supported_band, band);
  _set_vht_160mhz_short_gi (hw_handle, *supported_band, band);
  _set_he_support          (hw_handle, *supported_band, band);

  /* Initialize the list of supported channels */
  size = sizeof(struct ieee80211_channel) * n_channels;
  (*supported_band)->channels = mtlk_osal_mem_alloc(size, MTLK_MEM_TAG_MAC80211);
  if ((*supported_band)->channels == NULL)
    return MTLK_ERR_NO_MEM;

  wave_memcpy((*supported_band)->channels, size, channel_template, size);
  (*supported_band)->n_channels = n_channels;

  /* Disable HW unsupported channels */
  _wv_mac80211_disable_hw_unsupported_channels(hw, *supported_band);

#ifdef MTLK_DEBUG
  for (i = 0; i < (unsigned)(*supported_band)->n_channels; i++) {
    ILOG2_DD("Ant mask per channel: Channel %d enabled %d", (*supported_band)->channels[i].hw_value,
             !((*supported_band)->channels[i].flags & IEEE80211_CHAN_DISABLED));
  }
#endif

  /* Set the rates achievable with more than 1 STS (1 was already set above, statically) */
  for (i = 2; i <= MIN(num_sts, IEEE80211_HT_MCS_LIMIT); i++) {
    (*supported_band)->ht_cap.mcs.rx_mask[i-1] = 0xff;
  }

  for (i = 2; i <= MIN(num_sts, IEEE80211_VHT_MCS_LIMIT); i++) {
    (*supported_band)->vht_cap.vht_mcs.rx_mcs_map &= ~(IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(i));
    (*supported_band)->vht_cap.vht_mcs.rx_mcs_map |=  (IEEE80211_VHT_MCS_SUPPORT_0_9   << WV_MCS_MAP_SS_SHIFT(i));
    (*supported_band)->vht_cap.vht_mcs.tx_mcs_map &= ~(IEEE80211_VHT_MCS_NOT_SUPPORTED << WV_MCS_MAP_SS_SHIFT(i));
    (*supported_band)->vht_cap.vht_mcs.tx_mcs_map |=  (IEEE80211_VHT_MCS_SUPPORT_0_9   << WV_MCS_MAP_SS_SHIFT(i));
  }

  (*supported_band)->vht_cap.cap |= ((num_sts - 1) << IEEE80211_VHT_CAP_BEAMFORMEE_STS_SHIFT);
  (*supported_band)->vht_cap.cap |= ((num_sts - 1) << IEEE80211_VHT_CAP_SOUNDING_DIMENSIONS_SHIFT);

  ILOG0_SD("%s band supported, up to %i spatial streams", nl_band_to_str[band], num_sts);
  return MTLK_ERR_OK;
}

static mtlk_error_t _wv_mac80211_update_supported_bands (mtlk_handle_t hw_handle,
    unsigned radio_id, struct wiphy *wiphy, u32 num_sts)
{
  mtlk_error_t ret_code = MTLK_ERR_OK;

  if (mtlk_is_band_supported(hw_handle, radio_id, nlband2mtlkband(NL80211_BAND_2GHZ))) {
    ret_code = _wv_mac80211_init_band(hw_handle, &wiphy->bands[NL80211_BAND_2GHZ], &_supported_band_2ghz,
                   _2ghz_channels, ARRAY_SIZE(_2ghz_channels), num_sts, NL80211_BAND_2GHZ);

    if (ret_code != MTLK_ERR_OK)
      return ret_code;

    _wv_mac80211_update_he_capa(hw_handle, &wiphy->bands[NL80211_BAND_2GHZ], NL80211_BAND_2GHZ, num_sts);
  }

  if (mtlk_is_band_supported(hw_handle, radio_id, nlband2mtlkband(NL80211_BAND_5GHZ))) {
    ret_code = _wv_mac80211_init_band(hw_handle, &wiphy->bands[NL80211_BAND_5GHZ], &_supported_band_5ghz,
                   _5ghz_channels, ARRAY_SIZE(_5ghz_channels), num_sts, NL80211_BAND_5GHZ);
    if (ret_code != MTLK_ERR_OK)
      return ret_code;

    _wv_mac80211_update_he_capa(hw_handle, &wiphy->bands[NL80211_BAND_5GHZ], NL80211_BAND_5GHZ, num_sts);
  }

   /* 6GHz band support updates */
  MTLK_STATIC_ASSERT(NUM_TOTAL_CHANS_6G == ARRAY_SIZE(_6ghz_channels));
  if (mtlk_is_band_supported(hw_handle, radio_id, nlband2mtlkband(NL80211_BAND_6GHZ))) {
    mtlk_hw_t *hw = HANDLE_T_PTR(mtlk_hw_t, hw_handle);
    int size = (mtlk_hw_type_is_gen6_d2_or_gen7(hw) ? NUM_TOTAL_CHANS_6G : NUM_6GHZ_CHANS_UNII5);

    int ret_code = _wv_mac80211_init_band(hw_handle, &wiphy->bands[NL80211_BAND_6GHZ], &_supported_band_6ghz,
                       _6ghz_channels, size, num_sts, NL80211_BAND_6GHZ);
    if (ret_code != MTLK_ERR_OK)
      return ret_code;
   /* Reset the HT and VHT capabilities for 6GHz. This avoids any change in  wv_mac80211_init_band() */
    wiphy->bands[NL80211_BAND_6GHZ]->ht_cap.ht_supported = FALSE;
    wiphy->bands[NL80211_BAND_6GHZ]->vht_cap.vht_supported = FALSE;

    _wv_mac80211_update_he_capa(hw_handle, &wiphy->bands[NL80211_BAND_6GHZ], NL80211_BAND_6GHZ, num_sts);
  }

  if (!wiphy->bands[NL80211_BAND_2GHZ] && !wiphy->bands[NL80211_BAND_5GHZ] && !wiphy->bands[NL80211_BAND_6GHZ])
  {
    ELOG_V("No bands supported, please check your EEPROM/calibration file");
    return MTLK_ERR_PARAMS;
  }

  return MTLK_ERR_OK;
}

mtlk_error_t __MTLK_IFUNC
wv_ieee80211_setup_register (struct device *dev, wave_radio_t *radio, wv_mac80211_t *mac80211, mtlk_handle_t hw_handle)
{
  int err_code;
  mtlk_error_t ret;
  struct ieee80211_hw *hw = mac80211->mac80211_hw;
  u32 num_sts = WV_SUPP_NUM_STS; /* should get overwritten */

  if (mac80211->registered)
    return MTLK_ERR_OK;

  ILOG0_V("Registering mac80211 HW");

  /* Tell mac80211 our characteristics */
  ieee80211_hw_set(hw, SIGNAL_DBM);
  ieee80211_hw_set(hw, SPECTRUM_MGMT);
  ieee80211_hw_set(hw, REPORTS_TX_ACK_STATUS);
  ieee80211_hw_set(hw, MFP_CAPABLE);
  ieee80211_hw_set(hw, AMPDU_AGGREGATION);
  ieee80211_hw_set(hw, HAS_RATE_CONTROL);
  ieee80211_hw_set(hw, NO_AUTO_VIF);
  ieee80211_hw_set(hw, DATA_OFFLOAD);
  ieee80211_hw_set(hw, SUPPORTS_VENDOR_VHT);
  ieee80211_hw_set(hw, SUPPORTS_OP_MODE_NOTIF);

  /* Driver's private station information */
  hw->sta_data_size = sizeof (sta_entry);

  /* driver's private per-interface information */
  hw->vif_data_size = sizeof (struct wv_vif_priv);

  hw->chanctx_data_size = 0; /* relevant for multichannel HW */

  hw->wiphy->interface_modes = BIT(NL80211_IFTYPE_STATION) |
                               BIT(NL80211_IFTYPE_AP);

  hw->wiphy->mgmt_stypes = _wv_mgmt_stypes;

  /* set WIPHY flags */
  hw->wiphy->flags |= (WIPHY_FLAG_HAS_CHANNEL_SWITCH    |
                       WIPHY_FLAG_REPORTS_OBSS          |
                       WIPHY_FLAG_AP_PROBE_RESP_OFFLOAD |
                       WIPHY_FLAG_HAS_REMAIN_ON_CHANNEL |
                       WIPHY_FLAG_OFFCHAN_TX           |
                       WIPHY_FLAG_AP_UAPSD);

  if (mtlk_mmb_drv_get_disable_11d_hint_param()) {
    hw->wiphy->flags |= WIPHY_FLAG_DISABLE_11D_HINT;
    ILOG0_V("11d hints are disabled");
  }

  /* set WIPHY features */
  hw->wiphy->features = (NL80211_FEATURE_SK_TX_STATUS               |
                         NL80211_FEATURE_SCAN_FLUSH                 |
                         NL80211_FEATURE_SAE                        |
                         NL80211_FEATURE_AP_MODE_CHAN_WIDTH_CHANGE  |
                         NL80211_FEATURE_LOW_PRIORITY_SCAN          |
                         NL80211_FEATURE_AP_SCAN                    |
                         NL80211_FEATURE_STATIC_SMPS);

  hw->wiphy->regulatory_flags = REGULATORY_WIPHY_DISABLE_INTERSECTION;

  hw->wiphy->max_scan_ssids = MAX_SCAN_SSIDS;
  hw->wiphy->max_sched_scan_ssids = MAX_SCAN_SSIDS;
  hw->wiphy->max_sched_scan_reqs = 0;
  hw->wiphy->max_match_sets = MAX_MATCH_SETS;
  hw->wiphy->max_scan_ie_len = MAX_SCAN_IE_LEN;
  hw->wiphy->max_sched_scan_ie_len = MAX_SCAN_IE_LEN;
  hw->wiphy->out_of_scan_caching = 0; /* disabled */
  hw->wiphy->allow_scan_during_cac = 0; /* disabled */
  hw->wiphy->scan_cache_pause = 1; /* in pause */

  hw->queues = MAC80211_HW_TX_QUEUES; /*number of available hardware transmit queues for data packets.*/

  hw->wiphy->iface_combinations = _create_cfg80211_if_comb(hw_handle, mac80211->radio);
  hw->wiphy->n_iface_combinations = 1;

  hw_get_fw_version(hw_handle, hw->wiphy->fw_version,
      sizeof(hw->wiphy->fw_version));

  hw_get_hw_version(hw_handle, &hw->wiphy->hw_version);

  /* TX/RX available antennas: bitmap of antennas which are available to be configured as TX/RX antenna */
  wave_radio_ant_masks_num_sts_get(mac80211->radio,
                                   &hw->wiphy->available_antennas_tx, &hw->wiphy->available_antennas_rx, &num_sts);

  hw->wiphy->bands[NL80211_BAND_2GHZ] = NULL;
  hw->wiphy->bands[NL80211_BAND_5GHZ] = NULL;
  hw->wiphy->bands[NL80211_BAND_6GHZ] = NULL;

  ret = _wv_mac80211_update_supported_bands(hw_handle, wave_radio_id_get(mac80211->radio),
                                            hw->wiphy, num_sts);
  if (ret != MTLK_ERR_OK)
    return ret;

  if (hw->wiphy->bands[NL80211_BAND_2GHZ]) {
    wave_radio_band_set(mac80211->radio, NL80211_BAND_2GHZ);
  }
  if (hw->wiphy->bands[NL80211_BAND_5GHZ]) {
    wave_radio_band_set(mac80211->radio, NL80211_BAND_5GHZ);
  }
  if (hw->wiphy->bands[NL80211_BAND_6GHZ]) {
    wave_radio_band_set(mac80211->radio, NL80211_BAND_6GHZ);
  }

  if (wave_radio_get_is_zwdfs_radio(radio)) {
    hw->wiphy->flags |= WIPHY_FLAG_USE_OFFCHANNEL_CAC;
    ILOG0_D("off channel CAC is used on radio %d", wave_radio_id_get(radio));
  }

  /* Setting up passive scan on radar channels */
  _wv_ieee80211_mark_ch_for_passive_scan(hw->wiphy);

  hw->wiphy->signal_type = CFG80211_SIGNAL_TYPE_MBM;

  if(hw_get_gcmp_support(hw_handle)) {
    hw->wiphy->cipher_suites = _cipher_suites_gcmp;
    hw->wiphy->n_cipher_suites = ARRAY_SIZE(_cipher_suites_gcmp);
  } else {
    hw->wiphy->cipher_suites = _cipher_suites;
    hw->wiphy->n_cipher_suites = ARRAY_SIZE(_cipher_suites);
  }
  hw->wiphy->reg_notifier = _wv_ieee80211_reg_notifier;

  /* Uses mac80211's default value (5) */
  hw->max_listen_interval = 0;
  hw->wiphy->max_remain_on_channel_duration = MAX_ROC_DURATION_MS;

  _wave_mac80211_register_vendor_cmds(hw->wiphy);
  _wave_mac80211_register_vendor_evts(hw->wiphy);

  /* Register hw with mac80211 kernel framework and add one default STA interface */
  err_code = ieee80211_register_hw(hw);
  if (err_code) {
    ELOG_D("ieee80211 HW registration failed. Error code: %d", err_code);
    return MTLK_ERR_PARAMS;
  }
 

  if (hw->wiphy->bands[NL80211_BAND_2GHZ]) {
    struct ieee80211_supported_band *band = hw->wiphy->bands[NL80211_BAND_2GHZ];
    if(MTLK_ERR_OK != wave_radio_channel_table_build_2ghz(mac80211->radio, band->channels, band->n_channels))
      return MTLK_ERR_PARAMS;
  }
  if (hw->wiphy->bands[NL80211_BAND_5GHZ]) {
    struct ieee80211_supported_band *band = hw->wiphy->bands[NL80211_BAND_5GHZ];
    if(MTLK_ERR_OK != wave_radio_channel_table_build_5ghz(mac80211->radio, band->channels, band->n_channels))
      return MTLK_ERR_PARAMS;
  }
  if (hw->wiphy->bands[NL80211_BAND_6GHZ]) {
    struct ieee80211_supported_band *band = hw->wiphy->bands[NL80211_BAND_6GHZ];
    if(MTLK_ERR_OK != wave_radio_channel_table_build_6ghz(mac80211->radio, band->channels, band->n_channels))
      return MTLK_ERR_PARAMS;
  }

  wave_radio_channel_table_print(mac80211->radio);

  mac80211->registered = TRUE;

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC wv_ieee80211_cleanup (wv_mac80211_t *mac80211)
{
  struct ieee80211_supported_band **bands;
  struct ieee80211_iface_combination *if_comb;

  MTLK_ASSERT(mac80211 != NULL);
  if (mac80211 == NULL) {
    ELOG_V("mac80211 is NULL");
    return;
  }

  mtlk_osal_timer_cleanup(&mac80211->csa_timer);

  if_comb = (struct ieee80211_iface_combination*) mac80211->mac80211_hw->wiphy->iface_combinations;
  _cfg80211_if_comb_cleanup(if_comb);

  bands = mac80211->mac80211_hw->wiphy->bands;

  if (bands[NL80211_BAND_5GHZ]) {
    if (bands[NL80211_BAND_5GHZ]->channels)
      mtlk_osal_mem_free(bands[NL80211_BAND_5GHZ]->channels);

    mtlk_osal_mem_free(bands[NL80211_BAND_5GHZ]);
  }

  if (bands[NL80211_BAND_2GHZ]) {
    if (bands[NL80211_BAND_2GHZ]->channels)
      mtlk_osal_mem_free(bands[NL80211_BAND_2GHZ]->channels);

    mtlk_osal_mem_free(bands[NL80211_BAND_2GHZ]);
  }

  if (bands[NL80211_BAND_6GHZ]) {
    if (bands[NL80211_BAND_6GHZ]->channels)
      mtlk_osal_mem_free(bands[NL80211_BAND_6GHZ]->channels);

    mtlk_osal_mem_free(bands[NL80211_BAND_6GHZ]);
  }


  if (!mac80211->registered) {
    ILOG0_V("Freeing mac80211 HW");
    ieee80211_free_hw(mac80211->mac80211_hw);
  }
}

static void
_wv_ieee80211_hw_rx_status (struct ieee80211_hw *mac80211_hw, struct ieee80211_rx_status *rx_status,
    unsigned freq, int sig_dbm, int snr_db, int noise_dbm, uint16 rate_idx, u8 phy_mode, uint8 mtlk_pmf_flags, u8 nss)
{
  struct _wv_mac80211_t *mac80211 = __wv_ieee80211_hw_get_mac80211(mac80211_hw);
  struct mtlk_chan_def  *ccd;

  WAVE_WV_CHECK_PTR_VOID(mac80211);

  ccd = __wv_mac80211_chandef_get(mac80211);
  WAVE_WV_CHECK_PTR_VOID(ccd);

/* Tri band support */
  if (freq <= 2484)
    rx_status->band = NL80211_BAND_2GHZ;
  else if (freq >= 4910 && freq <= 5835)
    rx_status->band = NL80211_BAND_5GHZ;
  else if (freq >= 5935 && freq <= 7115)
    rx_status->band = NL80211_BAND_6GHZ;
  else
    rx_status->band = NL80211_BAND_60GHZ;

  if (mtlk_pmf_flags & MTLK_MGMT_FRAME_DECRYPTED)
    rx_status->flag |= RX_FLAG_DECRYPTED;

  if (mtlk_pmf_flags & MTLK_MGMT_FRAME_IV_STRIPPED)
    rx_status->flag |= RX_FLAG_IV_STRIPPED;

  if (mtlk_pmf_flags & MTLK_MGMT_FRAME_MIC_STRIPPED)
    rx_status->flag |= RX_FLAG_MIC_STRIPPED;

  rx_status->signal = sig_dbm;
  rx_status->freq = freq;
  rx_status->rate_idx = rate_idx;
  rx_status->nss = nss;
  rx_status->snr_db = snr_db;
  rx_status->noise = noise_dbm;

  switch (ccd->width) {
  case CW_20:
    break;
  case CW_40:
    rx_status->bw |= RATE_INFO_BW_40;
    break;
  case CW_80:
    rx_status->bw |= RATE_INFO_BW_80;
    break;
  case CW_160:
  case CW_80_80: /* 80+80 MHz rate removed from Linux starting from */
                 /* kernel 4.0, it is  treated the same as 160 MHz */
    rx_status->bw |= RATE_INFO_BW_160;
    break;
  }

  switch(phy_mode) {
  case PHY_MODE_N:
    rx_status->encoding = RX_ENC_HT;
    break;

  case PHY_MODE_AC:
    rx_status->encoding = RX_ENC_VHT;
    break;

  case PHY_MODE_AX:
    rx_status->encoding = RX_ENC_HE;
    break;

  default:
    break;
  };
}

int __MTLK_IFUNC
wv_ieee80211_mngmn_frame_rx (mtlk_core_t *nic, const u8 *data, uint32 size, unsigned freq, int sig_dbm, int snr_db, int noise_dbm,
                             unsigned subtype, uint16 rate_idx, u8 phy_mode, uint8 pmf_flags, u8 nss)
{
  struct ieee80211_hw *mac80211_hw = wave_vap_ieee80211_hw_get(nic->vap_handle);
  struct ieee80211_rx_status *rx_status;
  struct sk_buff *skb;
  u8 *buf;
  wv_mac80211_t *mac80211 = (wv_mac80211_t *)mac80211_hw->priv;

  if (!mac80211 || !mac80211->vif_array){
    ILOG2_V("Can't send rx frame to nowhere");
    return -EFAULT;
  }

  skb = dev_alloc_skb(size);
  if (!skb) {
    ELOG_V("Couldn't allocate RX skb frame");
    return -EFAULT;
  }

  buf = skb_put(skb, size);
  wave_memcpy(buf, size, data, size);
  rx_status = IEEE80211_SKB_RXCB(skb);
  memset(rx_status, 0, sizeof(struct ieee80211_rx_status));
  _wv_ieee80211_hw_rx_status(mac80211_hw, rx_status, freq, sig_dbm, snr_db, noise_dbm, rate_idx, phy_mode, pmf_flags, nss);

  ieee80211_rx_ni(mac80211_hw, skb);

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
wv_ieee80211_tx_status (struct ieee80211_hw *hw, struct sk_buff *skb, uint8 status)
{
  struct ieee80211_tx_info *info = IEEE80211_SKB_CB(skb);

  if ((status == TX_STATUS_SUCCESS) && !(info->flags & IEEE80211_TX_CTL_NO_ACK))
    info->flags |= IEEE80211_TX_STAT_ACK;

  ieee80211_tx_status(hw, skb);
}

void __MTLK_IFUNC
wv_ieee80211_report_low_ack (struct ieee80211_sta *sta, uint32 num_of_retry_packets)
{
  ieee80211_report_low_ack(sta, num_of_retry_packets);
}

void __MTLK_IFUNC
wave_vap_increment_ndp_counter (mtlk_vap_handle_t vap_handle)
{
  struct wv_vif_priv    *wv_iface_inf = _wave_vap_get_vif_priv(vap_handle);

  if (NULL != wv_iface_inf)
    wv_iface_inf->ndp_counter++;
}

struct ieee80211_hw * __MTLK_IFUNC
wv_ieee80211_hw_get (wv_mac80211_t *mac80211)
{
  MTLK_ASSERT(mac80211 != NULL);

  return mac80211->mac80211_hw;
}

static void _wv_ieee80211_op_sta_rc_update (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif, struct ieee80211_sta *sta, u32 changed)
{
  struct wv_vif_priv *wv_iface_inf = (struct wv_vif_priv *)vif->drv_priv;
  mtlk_df_user_t *df_user = wv_iface_inf->df_user;
  mtlk_operating_mode_t operating_mode;

  ILOG0_SD("wv_ieee80211_sta_rc_update for %s. changed = 0x%x",
      ieee80211_vif_to_name(vif), changed);
  if (df_user == NULL) {
    ELOG_V("df_user is NULL");
    return;
  }

  if (changed & IEEE80211_RC_BW_CHANGED)
    ILOG0_D("IEEE80211_RC_BW_CHANGED, new BW = %d", sta->bandwidth);

  if (changed & IEEE80211_RC_NSS_CHANGED)
    ILOG0_D("IEEE80211_RC_NSS_CHANGED, new RX NSS value = %d", sta->rx_nss);

  operating_mode.station_id = wv_iface_inf->vif_sid;
  operating_mode.channel_width = sta->bandwidth;
  operating_mode.rx_nss = sta->rx_nss;

  _mtlk_df_user_invoke_core_async(mtlk_df_user_get_df(df_user),
        WAVE_CORE_REQ_SET_OPERATING_MODE, (char*) &operating_mode,
        sizeof(operating_mode), NULL, 0);

  return;

}

void __MTLK_IFUNC
wv_ieee80211_radar_detected (wv_mac80211_t *mac80211, struct mtlk_chan_def *mcd, uint8 rbm)
{
  struct cfg80211_chan_def chandef = {0};
  struct ieee80211_hw *hw = wv_ieee80211_hw_get(mac80211);

  chandef.chan = ieee80211_get_channel(hw->wiphy, mcd->chan.center_freq);
  chandef.width = mtlkcw2nlcw(mcd->width, mcd->is_noht);
  chandef.center_freq1 = mcd->center_freq1;
  chandef.center_freq2 = mcd->center_freq2;

  ieee80211_radar_detected_rbm(hw, &chandef, rbm);
}

void __MTLK_IFUNC
wv_ieee80211_scan_completed (struct ieee80211_hw *hw, BOOL aborted)
{
  mtlk_df_t *master_df;
  struct cfg80211_scan_info info = {
    .aborted = aborted,
  };

  MTLK_ASSERT(hw != NULL);

  master_df = __wv_ieee80211_hw_master_df_get(hw);
  if (master_df)
    RLOG_S("%s: Scan done", mtlk_df_get_name(master_df));

  ieee80211_scan_completed(hw, &info);
}

void __MTLK_IFUNC
wv_ieee80211_scan_roc_completed (struct ieee80211_hw *hw)
{
  MTLK_ASSERT(hw != NULL);

  ILOG1_V("ROC is expired");
  ieee80211_remain_on_channel_expired(hw);
}

void __MTLK_IFUNC
wv_ieee80211_scan_roc_ready_on_chan(struct ieee80211_hw *hw)
{
  MTLK_ASSERT(hw != NULL);

  ILOG1_V("ROC channel is set");
  ieee80211_ready_on_channel(hw);
}

/* FW recovery - reset vif values to reflect updated FW status */
void __MTLK_IFUNC
wv_mac80211_recover_sta_vifs (wv_mac80211_t *mac80211)
{
  MTLK_ASSERT(NULL != mac80211);
  MTLK_ASSERT(NULL != mac80211->radio);

  if (__wv_mac80211_get_sta_vifs_exist(mac80211)) {
    unsigned vap_index, max_vaps_count;

    max_vaps_count = __wv_mac80211_max_vaps_count_get(mac80211);

    for (vap_index = 0; vap_index < max_vaps_count; vap_index++) {
      struct wv_vif_priv *curr_vif_priv = _wv_mac80211_get_vif_priv(mac80211, vap_index);

      if (curr_vif_priv != NULL){
        curr_vif_priv->is_initialized = FALSE;
      }
    }
  }

  return;
}

static int _wv_ieee80211_op_set_rts_threshold (struct ieee80211_hw *hw, u32 value)
{
  wave_radio_t *radio;

  MTLK_ASSERT(hw != NULL);

  radio  = wv_ieee80211_hw_radio_get(hw);
  MTLK_ASSERT(NULL != radio);

  return wave_radio_rts_threshold_set(radio, value);
}

static void _wv_request_get_sta_stats (mtlk_df_user_t *df_user,
          st_info_data_t *info_data)
{
  mtlk_clpb_t *clpb = NULL;
  mtlk_error_t res = MTLK_ERR_OK;

  if (mtlk_core_rcvry_is_running(mtlk_vap_get_core(mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user)))) ||
      wave_rcvry_mac_fatal_pending_get(mtlk_vap_get_hw(mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user)))))
    return;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
    WAVE_CORE_REQ_GET_STATION, &clpb, info_data, sizeof(st_info_data_t));
  res = _mtlk_df_user_process_core_retval(res, clpb,
    WAVE_CORE_REQ_GET_STATION, TRUE);

  if (res != MTLK_ERR_OK)
    WLOG_V("Getting time sta statistics failed.");
}

static void _wv_ieee80211_op_sta_statistics (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif, struct ieee80211_sta *sta,
    struct station_info *sinfo)
{
  mtlk_wssa_drv_tr181_peer_stats_t *sta_stats;
  uint32 stats_size;
  mtlk_clpb_t *clpb = NULL;
  mtlk_error_t res = MTLK_ERR_OK;
  st_info_data_t info_data;
  mtlk_vap_handle_t vap_handle;
  mtlk_df_user_t *df_user = wv_ieee80211_vif_to_dfuser(vif);

  if ((vif->type == NL80211_IFTYPE_STATION) && !vif->bss_conf.assoc) {
    ILOG2_Y("STA Mode: Not connected any AP: %Y", sta->addr);
    return;
  }

  if (NULL == df_user) {
    WLOG_V("Getting df_user failed.");
    return;
  }
  vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));

  if (mtlk_core_rcvry_is_running(mtlk_vap_get_core(vap_handle)) ||
      wave_rcvry_mac_fatal_pending_get(mtlk_vap_get_hw(vap_handle))){
    return;
  }

  MTLK_ASSERT(sta != NULL);

  info_data.sta = sta;
  info_data.mac = sta->addr;
  info_data.stinfo = sinfo;
  _wv_request_get_sta_stats(df_user, &info_data);

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
    WAVE_CORE_REQ_GET_TR181_PEER_STATS, &clpb, &info_data, sizeof(st_info_data_t));
  res = _mtlk_df_user_process_core_retval(res, clpb,
    WAVE_CORE_REQ_GET_TR181_PEER_STATS, FALSE);

  if (res != MTLK_ERR_OK) {
    WLOG_V("Getting sta statistics failed.");
    return;
  }

  sta_stats = mtlk_clpb_enum_get_next(clpb, &stats_size);
  MTLK_CLPB_TRY(sta_stats, stats_size)

    sinfo->rx_packets = sta_stats->traffic_stats.PacketsReceived;
    sinfo->filled |= BIT(NL80211_STA_INFO_RX_PACKETS);
    sinfo->rx_bytes = sta_stats->traffic_stats.BytesReceived;
    sinfo->filled |= BIT(NL80211_STA_INFO_RX_BYTES64);
    sinfo->tx_packets = sta_stats->traffic_stats.PacketsSent;
    sinfo->filled |= BIT(NL80211_STA_INFO_TX_PACKETS);
    sinfo->tx_bytes = sta_stats->traffic_stats.BytesSent;
    sinfo->filled |= BIT(NL80211_STA_INFO_TX_BYTES64);

    sinfo->tx_retries = sta_stats->retrans_stats.RetransCount;
    sinfo->filled |= BIT(NL80211_STA_INFO_TX_RETRIES);
    sinfo->tx_failed = sta_stats->ErrorsSent;
    sinfo->filled |= BIT(NL80211_STA_INFO_TX_FAILED);

  MTLK_CLPB_FINALLY(res)
    mtlk_clpb_delete(clpb);
  MTLK_CLPB_END;
}

static int _wv_ieee80211_op_get_survey (struct ieee80211_hw *hw, int idx,
                     struct survey_info *survey)
{
  wave_radio_t *radio;
  mtlk_vap_handle_t vap_handle;
  mtlk_vap_manager_t *vap_manager;
  mtlk_df_user_t *df_user;
  mtlk_clpb_t *clpb = NULL;
  mtlk_wssa_drv_tr181_hw_stats_t *tr181_stats;
  uint32 stats_size;
  mtlk_error_t res = MTLK_ERR_OK;
  int chan_num = ARRAY_SIZE(_2ghz_channels) + ARRAY_SIZE(_5ghz_channels);

  if (idx < 0 || idx >= chan_num)
    return -ENOENT;

  survey->channel = hw->conf.chandef.chan;
  radio = wv_ieee80211_hw_radio_get(hw);
  MTLK_ASSERT(NULL != radio);

  vap_manager = wave_radio_vap_manager_get(radio);

  if (MTLK_ERR_OK != mtlk_vap_manager_get_master_vap(vap_manager, &vap_handle))
    return -ENXIO;

  df_user = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
  MTLK_CHECK_DF_USER(df_user);

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
    WAVE_CORE_REQ_GET_TR181_HW_STATS, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval(res, clpb,
    WAVE_CORE_REQ_GET_TR181_HW_STATS, FALSE);

  if (res != MTLK_ERR_OK) {
    WLOG_V("Getting hw statistics failed.");
    return _mtlk_df_mtlk_to_linux_error_code(res);
  }

  tr181_stats = mtlk_clpb_enum_get_next(clpb, &stats_size);
  MTLK_CLPB_TRY(tr181_stats, stats_size)

    survey->noise = tr181_stats->Noise;
    survey->filled = SURVEY_INFO_NOISE_DBM;

  MTLK_CLPB_FINALLY(res)
    mtlk_clpb_delete(clpb);
  MTLK_CLPB_END;

  return LINUX_ERR_OK;
}

static const char _wv_gstrings_stats[][ETH_GSTRING_LEN] = {
  "iwlwav_i_rx_ucast",
  "iwlwav_i_tx_ucast",
  "iwlwav_i_rx_mcast",
  "iwlwav_i_tx_mcast",
  "iwlwav_i_rx_bcast",
  "iwlwav_i_tx_bcast",
  "iwlwav_i_dropped",
  "iwlwav_i_err_received",
  "iwlwav_i_retransmitions",
  "iwlwav_i_single_retry",
  "iwlwav_i_multy_retry",
  "iwlwav_i_failed_retry",
  "iwlwav_i_ack_failed",
  "iwlwav_i_aggr_packets",
  "iwlwav_i_unknown_proto_packets",
  "iwlwav_p_fcs_error_count",
};

#define MTLK_SSTATS_LEN   ARRAY_SIZE(_wv_gstrings_stats)

static int _wv_ieee80211_op_get_et_sset_count (struct ieee80211_hw *hw,
                                          struct ieee80211_vif *vif, int sset)
{
  if (sset == ETH_SS_STATS)
    return MTLK_SSTATS_LEN;
  return 0;
}

static void _wv_request_get_phy_stats (mtlk_df_user_t *df_user,
                            int *curr_idx, u64 *data)
{
  mtlk_wssa_drv_tr181_hw_stats_t *phy_stats;
  mtlk_clpb_t *clpb = NULL;
  uint32 stats_size;
  mtlk_error_t res = MTLK_ERR_OK;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
    WAVE_CORE_REQ_GET_TR181_HW_STATS, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval(res, clpb,
    WAVE_CORE_REQ_GET_TR181_HW_STATS, FALSE);

  if (res != MTLK_ERR_OK) {
    WLOG_V("Getting radio statistics failed.");
    return;
  }

  phy_stats = mtlk_clpb_enum_get_next(clpb, &stats_size);
  MTLK_CLPB_TRY(phy_stats, stats_size)

    data[(*curr_idx)++] = phy_stats->FCSErrorCount;

  MTLK_CLPB_FINALLY(res)
    mtlk_clpb_delete(clpb);
  MTLK_CLPB_END;
}

static void _wv_ieee80211_op_get_et_stats (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif,
    struct ethtool_stats *stats, u64 *data)
{
  int i = 0;
  uint32 stats_size;
  mtlk_wssa_drv_tr181_wlan_stats_t *iface_stats;
  mtlk_clpb_t *clpb = NULL;
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_df_user_t *df_user = wv_ieee80211_vif_to_dfuser(vif);
  if (NULL == df_user) {
    WLOG_V("Getting df_user failed.");
    return;
  }

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
    WAVE_CORE_REQ_GET_TR181_WLAN_STATS, &clpb, NULL, 0);
  res = _mtlk_df_user_process_core_retval(res, clpb,
    WAVE_CORE_REQ_GET_TR181_WLAN_STATS, FALSE);

  if (res != MTLK_ERR_OK) {
    WLOG_V("Getting vap statistics failed.");
    return;
  }

  /* obtain iface proprietary parameters */
  iface_stats = mtlk_clpb_enum_get_next(clpb, &stats_size);
  MTLK_CLPB_TRY(iface_stats, stats_size)

    data[i++] = iface_stats->traffic_stats.UnicastPacketsReceived;
    data[i++] = iface_stats->traffic_stats.UnicastPacketsSent;
    data[i++] = iface_stats->traffic_stats.MulticastPacketsReceived;
    data[i++] = iface_stats->traffic_stats.MulticastPacketsSent;
    data[i++] = iface_stats->traffic_stats.BroadcastPacketsReceived;
    data[i++] = iface_stats->traffic_stats.BroadcastPacketsSent;
    data[i++] = iface_stats->error_stats.DiscardPacketsReceived;
    data[i++] = iface_stats->error_stats.ErrorsReceived;
    data[i++] = iface_stats->retrans_stats.Retransmissions;
    data[i++] = iface_stats->retrans_stats.RetryCount;
    data[i++] = iface_stats->retrans_stats.MultipleRetryCount;
    data[i++] = iface_stats->retrans_stats.FailedRetransCount;
    data[i++] = iface_stats->ACKFailureCount;
    data[i++] = iface_stats->AggregatedPacketCount;
    data[i++] = iface_stats->UnknownProtoPacketsReceived;

  MTLK_CLPB_FINALLY(res)
    mtlk_clpb_delete(clpb);
  MTLK_CLPB_END;

  /* obtain phy proprietary parameters */
  _wv_request_get_phy_stats(df_user, &i, data);

  WARN_ON(i != MTLK_SSTATS_LEN);
}

static void _wv_ieee80211_op_get_et_strings (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif,
    u32 sset, u8 *data)
{
  if (sset == ETH_SS_STATS)
    wave_memcpy(data, sizeof(_wv_gstrings_stats),
        _wv_gstrings_stats, sizeof(_wv_gstrings_stats));
}

static void
_wv_ieee80211_free_beacon_cfg_data (struct _wv_beacon_cfg *wv_beacon_cfg)
{
  dev_kfree_skb(wv_beacon_cfg->beacon_tmpl);
  dev_kfree_skb(wv_beacon_cfg->probe_resp);
}

static mtlk_error_t
_wv_ieee80211_get_beacon_cfg_data (struct ieee80211_hw *hw,
                                   struct ieee80211_vif *vif,
                                   struct _wv_beacon_cfg *wv_beacon_cfg)
{
  mtlk_error_t res = MTLK_ERR_PARAMS;
  struct ieee80211_mutable_offsets offs;

  memset(wv_beacon_cfg, 0, sizeof(*wv_beacon_cfg));

  wv_beacon_cfg->beacon_tmpl = ieee80211_beacon_get_template(hw, vif, &offs);
  if (!wv_beacon_cfg->beacon_tmpl) {
    res = MTLK_ERR_PARAMS;
    goto out;
  }

  wv_beacon_cfg->probe_resp = ieee80211_proberesp_get(hw, vif);
  if (!wv_beacon_cfg->probe_resp) {
    res = MTLK_ERR_PARAMS;
    dev_kfree_skb(wv_beacon_cfg->beacon_tmpl);
    goto out;
  }

  res = wave_beacon_template_get_beacon_data(wv_beacon_cfg->beacon_tmpl,
                                             wv_beacon_cfg->probe_resp,
                                             &wv_beacon_cfg->beacon_data, &offs);

out:
  return res;
}

static int _wv_ieee80211_op_start_ap (struct ieee80211_hw *hw,
                                      struct ieee80211_vif *vif)
{
  struct wireless_dev *wdev = ieee80211_vif_to_wdev(vif);
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);
  mtlk_df_user_t *df_user;
  int res;
  struct _wv_beacon_cfg wv_beacon_cfg;


  if (!wdev) {
    ELOG_S("%s: failed to retrieve wdev", ieee80211_vif_to_name(vif));
    res = MTLK_ERR_UNKNOWN;
    goto out;
  }

  ILOG0_S("%s: start_ap", ieee80211_vif_to_name(vif));
  MTLK_ASSERT(NULL != radio);

  df_user = wv_ieee80211_vif_to_dfuser(vif);
  if (df_user == NULL) {
    res = MTLK_ERR_UNKNOWN;
    goto out;
  }

  res = wave_radio_abort_and_prevent_scan(radio);
  if (MTLK_ERR_OK != res)
    goto out;
  res = _wv_activate_vif(df_user);
  if (MTLK_ERR_OK != res){
    wave_radio_allow_or_resume_scan(radio);
    goto out;
  }
  res = wave_radio_allow_or_resume_scan(radio);

  res = _wv_ieee80211_get_beacon_cfg_data(hw, vif, &wv_beacon_cfg);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Failed to get beacon template data, res=%d", res);
    goto out;
  }

  res = wave_radio_ap_start(hw->wiphy, radio, wdev->netdev, &vif->bss_conf, &wv_beacon_cfg.beacon_data);
  if (res != MTLK_ERR_OK)
    ELOG_SD("%s: failed to start ap. res =%d", ieee80211_vif_to_name(vif), res);

  _wv_ieee80211_free_beacon_cfg_data(&wv_beacon_cfg);

out:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int wave_core_trigger_whm(wave_radio_t *radio, whm_warning_id warning_id)
{
  wave_whm_trigger_t whm_trigger_cfg;
  mtlk_clpb_t *clpb = NULL;
  mtlk_df_user_t *df_user;
  mtlk_df_t *master_df;
  int res = MTLK_ERR_UNKNOWN;

  MTLK_ASSERT(NULL != radio);

  master_df = wave_radio_df_get(radio);
  if (!master_df)
    return res;

  df_user = mtlk_df_get_user(master_df);
  MTLK_CHECK_DF_USER(df_user);

  memset(&whm_trigger_cfg, 0, sizeof(whm_trigger_cfg));

  whm_trigger_cfg.whm_warning_id = warning_id;
  whm_trigger_cfg.whm_warning_layer = WHM_DRIVER_TRIGGER;

  res = _mtlk_df_user_invoke_core(mtlk_df_user_get_df(df_user),
    WAVE_CORE_REQ_SET_WHM_TRIGGER, &clpb, &whm_trigger_cfg, sizeof(whm_trigger_cfg));
  res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_CORE_REQ_SET_WHM_TRIGGER, TRUE);

  return res;
}

static void _wv_ieee80211_op_stop_ap (struct ieee80211_hw *hw,
    struct ieee80211_vif *vif)
{
  struct wireless_dev *wdev = ieee80211_vif_to_wdev(vif);
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);

  if (!wdev) {
    ELOG_S("%s: failed to retrieve wdev", ieee80211_vif_to_name(vif));
    return;
  }

  ILOG0_SSD("%s: Invoked from %s (%i)", ieee80211_vif_to_name(vif),
            current->comm, current->pid);

  MTLK_ASSERT(NULL != radio);

  if (strncmp(current->comm, "hostapd", sizeof("hostapd"))) {
    /* In normal stop ap case, hostapd triggers it. Trigger WHM if not hostapd. */
    WLOG_DS("WHM: Id %d, Trigger warning due to unexpected stop ap from %s",
           WHM_HOSTAP_IFDOWN, current->comm);
    wave_core_trigger_whm(radio, WHM_HOSTAP_IFDOWN);
  }

  wave_radio_ap_stop(radio, wdev->netdev);
}

static int _wv_ieee80211_op_set_antenna (struct ieee80211_hw *hw,
    u32 tx_ant, u32 rx_ant)
{
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);

  MTLK_ASSERT(NULL != radio);

  return wave_radio_antenna_set(radio, tx_ant, rx_ant);
}

static int _wv_ieee80211_op_get_antenna (struct ieee80211_hw *hw,
    u32 *tx_ant, u32 *rx_ant)
{
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);

  MTLK_ASSERT(NULL != radio);

  return wave_radio_antenna_get(radio, tx_ant, rx_ant);
}

static void _wv_ieee80211_op_handle_radar_debug (struct ieee80211_hw *hw,
                                                 struct cfg80211_chan_def *chandef,
                                                 uint8 radar_bitmap)
{
#ifdef CONFIG_WAVE_DEBUG
#ifndef CPTCFG_IWLWAV_X86_HOST_PC
  wave_radio_t        *radio = wv_ieee80211_hw_radio_get(hw);
  mtlk_df_t           *master_df;
  mtlk_vap_handle_t    master_vap_handle;
  mtlk_scan_support_t *scan_support;

  MTLK_ASSERT(NULL != radio);

  scan_support = wave_radio_scan_support_get(radio);
  master_df = __wv_ieee80211_hw_master_df_get(hw);

  if (master_df == NULL) {
    ELOG_S("%s: Error retrieving master df", wiphy_name(hw->wiphy));
    return;
  }

  master_vap_handle = mtlk_df_get_vap_handle(master_df);
  /* Use debug non occupancy period */
  if (scan_support->dfs_debug_params.nop)
    wv_cfg80211_debug_nop(hw->wiphy, master_vap_handle, chandef, radar_bitmap);
#endif
#endif
  return;
}

/** Q: Why _wv_ieee80211_op_driver_alloc_netdev and _wv_ieee80211_op_driver_free_netdev_allowed exists?
 *
 * A: DC_DP requires the netdev registration (for the master vap)
 * to be done along with the datapath init.
 * The datapath init is done on insmod so we are allocating the netdev
 * for the master vaps on insmod.
 * When the mac80211 is supposed to alloc them we instead call
 * driver_alloc_netdev and returns the one we allocated on insmod.
 * If the net_dev is not the first one (so it is not the master) we returns NULL
 * and mac80211 will allocate it himself.
 * If someone tried to free the master netdev (other than on rmmod) we
 * will block it so the datapath will remains valid.
 *
 * In CDB we have 2 radios on one card, so the FW demands the datapath for
 * both radios together, therefore we must allocate the netdev ourself so we can
 * get DC_DP paths for both radios and init datapath in the FW.
 * This is done also so we can use the card (with data) even when only
 * one interface (or multiple, but on the same radio) was added,
 * without having to wait for the user to add an interface for the other radio.
 */
static struct net_device *
_wv_ieee80211_op_driver_alloc_netdev (struct ieee80211_hw * hw)
{
  wave_radio_t *radio = wv_ieee80211_hw_radio_get(hw);
  mtlk_vap_handle_t vap_handle;
  mtlk_df_user_t *df_user;
  mtlk_vap_manager_t * vap_manager = wave_radio_vap_manager_get(radio);

  if (mtlk_vap_manager_get_master_ndev_taken(vap_manager))
    return NULL;

  if (mtlk_vap_manager_get_master_vap(vap_manager, &vap_handle) != MTLK_ERR_OK)
    return NULL;

  df_user     = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
  mtlk_vap_manager_set_master_ndev_taken(vap_manager, TRUE);
  ILOG2_V("master_ndev_take");
  return mtlk_df_user_get_ndev(df_user);
}

static int empty_func(struct net_device *dev)
{
	return -EOPNOTSUPP;
}

static struct net_device_ops minimal_ops;

static int
_wv_ieee80211_op_driver_free_netdev_allowed (struct ieee80211_hw * hw, struct net_device * ndev)
{
  struct _wv_mac80211_t *mac80211 = __wv_ieee80211_hw_get_mac80211(hw);
  mtlk_vap_manager_t * vap_manager = wave_radio_vap_manager_get(wv_ieee80211_hw_radio_get(hw));
  mtlk_vap_handle_t vap_handle;
  mtlk_df_user_t *df_user;

  if (mtlk_vap_manager_get_master_vap(vap_manager, &vap_handle) != MTLK_ERR_OK)
    return -EFAULT;

  df_user     = mtlk_df_get_user(mtlk_vap_get_df(vap_handle));
  if (mtlk_df_user_get_ndev(df_user) == ndev){
    if (!mtlk_vap_manager_get_master_ndev_taken(vap_manager)){
      /*Should never get here, only if mac80211 cleanup flow changed*/
      ILOG0_V("Removing master vap netdev");
      return LINUX_ERR_OK;
    }
    if (!mac80211->registered) {
      minimal_ops = *(ndev->netdev_ops);
      minimal_ops.ndo_open = empty_func;
      minimal_ops.ndo_stop = empty_func;
      ndev->netdev_ops = &minimal_ops;
      ILOG0_V("Clearing master vap netdev ops");
    }
    ILOG0_V("Can't remove master vap");
    return -EOPNOTSUPP;
  }
  return LINUX_ERR_OK;
}

static int
_wv_ieee80211_op_prepare_cac_start (struct ieee80211_hw *hw,
                                    struct ieee80211_vif *vif,
                                    u32 cac_time_ms)
{
  mtlk_error_t res = MTLK_ERR_UNKNOWN;
  mtlk_df_t *master_df;
  mtlk_clpb_t *clpb = NULL;

  RLOG_SD("%s: CAC started, cac time %i ms",
          ieee80211_vif_to_name(vif), cac_time_ms);

  master_df = __wv_ieee80211_hw_master_df_get(hw);
  if(!master_df)
    goto out;

  res = _mtlk_df_user_invoke_core(master_df, WAVE_RADIO_REQ_PREPARE_CAC_START, &clpb, NULL, 0);
  _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_RADIO_REQ_PREPARE_CAC_START, TRUE);

out:
  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static void
_wv_ieee80211_op_notify_cac_finished (struct ieee80211_hw *hw,
                                      struct ieee80211_vif *vif,
                                      bool cac_canceled)
{
  mtlk_error_t res;
  mtlk_df_t *master_df;
  mtlk_clpb_t *clpb = NULL;

  RLOG_SS("%s: CAC %s", ieee80211_vif_to_name(vif),
          cac_canceled ? "canceled" : "finished");

  master_df = __wv_ieee80211_hw_master_df_get(hw);
  if(!master_df)
    return;

  res = _mtlk_df_user_invoke_core(master_df, WAVE_RADIO_REQ_NOTIFY_CAC_FINISHED, &clpb, NULL, 0);
  _mtlk_df_user_process_core_retval_void(res, clpb, WAVE_RADIO_REQ_NOTIFY_CAC_FINISHED, TRUE);
}

static void
_wv_ieee80211_op_get_vap_stats(struct net_device *ndev,
                               struct rtnl_link_stats64 *stats)
{
  mtlk_df_user_t *df_user;

  df_user = mtlk_df_user_from_ndev(ndev);
  mtlk_df_user_get_stats(df_user, stats);
}

#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_RELEASE_DLEVEL)
static const char *roc_type_to_str[] = { "ROC_TYPE_NORMAL", "ROC_TYPE_MGMT_TX" };
#endif

static const mtlk_scan_type_e ieee_roc_type_to_mtlk[]  = { MTLK_ROC_NORMAL, MTLK_SCAN_STA_ROC };

static int _wv_ieee80211_remain_on_channel(struct ieee80211_hw *hw,
    struct ieee80211_vif *vif,
    struct ieee80211_channel *chan,
    int duration,
    enum ieee80211_roc_type type)
{
  mtlk_clpb_t *clpb = NULL;
  wv_mac80211_t *mac80211;
  struct wv_vif_priv *wv_iface_inf;
  mtlk_vap_handle_t  master_vap_handle;
  mtlk_core_t       *master_core;
  mtlk_df_t         *master_df;
  wave_radio_t      *radio;
  struct mtlk_scan_request *sr;
  struct mtlk_channel *mc;
  struct mtlk_chan_def *current_chandef = NULL;
  enum ieee80211_roc_type target_roc_type;
  int res = MTLK_ERR_OK;



  ILOG1_SSD("%s: Invoked from %s (%i):", ieee80211_vif_to_name(vif), current->comm, current->pid);

  /* we support 2GHz, 5GHz and 6GHz bands */
  if (((enum nl80211_band)chan->band == NL80211_BAND_60GHZ) || ((enum nl80211_band)chan->band >= NUM_NL80211_BANDS)) {
   ELOG_D("nl band %d not supported", chan->band);
   return -EINVAL;
  }

  /* we support ROC: NORMAL and MGMT TX */
  if ((type != IEEE80211_ROC_TYPE_MGMT_TX) && (type != IEEE80211_ROC_TYPE_NORMAL)) {
    ELOG_D("ROC type %d not supported", type);
    return -EINVAL;
  }

  RLOG_SSDD("roc_type=%s, band=%s, freq=%d, duration=%i",
    roc_type_to_str[type], nl_band_to_str[chan->band], chan->center_freq, duration);

  wv_iface_inf = (struct wv_vif_priv*)vif->drv_priv;
  WAVE_WV_CHECK_PTR_RES(wv_iface_inf);
  mac80211 = __wv_ieee80211_hw_get_mac80211(hw);
  WAVE_WV_CHECK_PTR_RES(mac80211);

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_RES(master_df);

  master_vap_handle = mtlk_df_get_vap_handle(master_df);
  master_core = mtlk_vap_get_core(master_vap_handle);
  radio = wv_ieee80211_hw_radio_get(hw);

  if (_wait_rcvry_completion_if_needed(master_core) != MTLK_ERR_OK){
    ELOG_V("Recovery is running, can't perform HW scan");
    return -EBUSY;
  }


  /* scan request is too big to put on the stack */
  if (!(sr = mtlk_osal_mem_alloc(sizeof(*sr), MTLK_MEM_TAG_CFG80211))) {
    ELOG_V("_wv_ieee80211_op_hw_scan: can't allocate memory");
    return _mtlk_df_mtlk_to_linux_error_code(MTLK_ERR_NO_MEM);
  }

  /*
   * ROC_TYPE_MGMT_TX does CTS-to-Self on the current channel before switch to the new one.
   * If ROC channel equal to current channel, client will not reply to MGMT frame during
   * CTS-To_self duration and some sessions like DPP will be timed out.
   * Solution: do ROC_TYPE_NORMAL instead.
   */
  target_roc_type = type;
  current_chandef = __wave_core_chandef_get(master_core);
  if (!is_channel_certain(current_chandef)) {
    WLOG_V("channel uncertain");
  } else if ((type == IEEE80211_ROC_TYPE_MGMT_TX) && (current_chandef->chan.center_freq == chan->center_freq)) {
    ILOG0_V("force ROC_TYPE_NORMAL");
    target_roc_type = IEEE80211_ROC_TYPE_NORMAL;
  }

  memset(sr, 0, sizeof(*sr));
  sr->type = ieee_roc_type_to_mtlk[target_roc_type];
  sr->requester_vap_index = wv_iface_inf->vap_index;
  sr->wiphy = hw->wiphy;
  sr->n_channels = 1; /* only one chnannel */
  sr->duration = (target_roc_type == IEEE80211_ROC_TYPE_MGMT_TX)
    ? WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_CTS_TO_SELF_TO)
    : duration;

  /* Only one channel */
  mc = &sr->channels[0];
  mc->dfs_state_entered = chan->dfs_state_entered;
  mc->dfs_state = chan->dfs_state;
  mc->dfs_cac_ms = chan->dfs_cac_ms;
  mc->band = nlband2mtlkband(chan->band);
  mc->center_freq = chan->center_freq;
  mc->flags = chan->flags;
  mc->orig_flags = chan->orig_flags;
  mc->max_antenna_gain = chan->max_antenna_gain;
  mc->max_power = chan->max_power;
  mc->max_reg_power = chan->max_reg_power;

  /* Keep trying in case scan is in progress for the current radio */
  {
    int retry_counter = 0;

    do {
      /* any checks for preexisting scans done later to minimize locking */
      res = _mtlk_df_user_invoke_core(master_df,
          WAVE_RADIO_REQ_DO_SCAN, &clpb, sr, sizeof(*sr));
      res = _mtlk_df_user_process_core_retval(res, clpb,
          WAVE_RADIO_REQ_DO_SCAN, TRUE);

      if (MTLK_ERR_RETRY != res) break;
      mtlk_osal_msleep(50);
      retry_counter++;
    } while ((MTLK_ERR_RETRY == res) && (retry_counter < MAX_SCAN_WAIT_RETRIES));

    if (retry_counter > 0)
      ILOG0_SD("%s: Scan waited, number of retries %d", ieee80211_vif_to_name(vif), retry_counter);

  }

  /* no waiting for the end of the roc */
  mtlk_osal_mem_free(sr);

  ILOG2_SD("%s: return with %d", ieee80211_vif_to_name(vif), _mtlk_df_mtlk_to_linux_error_code(res));

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

static int _wv_ieee80211_cancel_remain_on_channel(struct ieee80211_hw *hw, struct ieee80211_vif *vif)
{
  int res;
  mtlk_clpb_t *clpb = NULL;
  mtlk_alter_scan_t alter_scan;
  wv_mac80211_t     *mac80211 = __wv_ieee80211_hw_get_mac80211(hw);
  mtlk_df_t         *master_df;
  wave_radio_t      *radio;

  ILOG1_V("Cancel remain on channel");
  WAVE_WV_CHECK_PTR_RES(mac80211);

  master_df = _wv_mac80211_master_df_get(mac80211);
  WAVE_WV_CHECK_PTR_RES(master_df);

  radio = wv_ieee80211_hw_radio_get(hw);
  WAVE_WV_CHECK_PTR_RES(radio);

  wave_radio_roc_in_progress_set(radio, 0);


  /* Abort in case a scan_roc is running (initiated by the sta interface) */
  memset(&alter_scan, 0, sizeof(alter_scan));
  alter_scan.abort_scan = TRUE;
  alter_scan.pause_or_prevent = FALSE;

  res = _mtlk_df_user_invoke_core(master_df, WAVE_RADIO_REQ_ALTER_SCAN, &clpb, &alter_scan, sizeof(alter_scan));
  res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_RADIO_REQ_ALTER_SCAN, TRUE);

  if (MTLK_ERR_OK != res)
    ELOG_V("Failed to abort or prevent scan");

  return _mtlk_df_mtlk_to_linux_error_code(res);
}

const struct ieee80211_ops wv_ieee80211_ops = {
  .start_ap                 = _wv_ieee80211_op_start_ap,
  .stop_ap                  = _wv_ieee80211_op_stop_ap,
  .tx                       = _wv_ieee80211_op_tx,
  .data_tx                  = _wv_ieee80211_op_data_tx,
  .start                    = _wv_ieee80211_op_start,
  .stop                     = _wv_ieee80211_op_stop,
  .config                   = _wv_ieee80211_op_config,
  .add_interface            = _wv_ieee80211_op_add_interface,
  .remove_interface         = _wv_ieee80211_op_remove_interface,
  .configure_filter         = _wv_ieee80211_op_configure_filter,
  .sta_state                = _wv_ieee80211_op_sta_state,
  .set_key                  = _wv_ieee80211_op_set_key,
  .set_default_unicast_key  = _wv_ieee80211_op_set_default_unicast_key,
  .set_default_multicast_key= _wv_ieee80211_op_set_default_multicast_key,
  .get_key_seq_by_vif       = _wv_ieee80211_op_get_key_seq_by_vif,
  .pre_get_key_seq_by_vif   = _wv_ieee80211_op_pre_get_key_seq_by_vif,
  .set_frag_threshold       = _wv_ieee80211_op_set_frag_threshold,
  .bss_info_changed         = _wv_ieee80211_op_bss_info_changed,
  .sta_rc_update            = _wv_ieee80211_op_sta_rc_update,
  .conf_tx                  = _wv_ieee80211_op_conf_tx,
  .set_qos_map              = _wv_ieee80211_op_set_qos_map,
  .channel_switch           = _wv_ieee80211_op_channel_switch,
  .channel_switch_beacon    = NULL, /* AP CSA beacon is managed by firmware */
  .pre_channel_switch       = _wv_ieee80211_op_pre_channel_switch,
  .post_channel_switch      = _wv_ieee80211_op_post_channel_switch,
  .hw_scan                  = _wv_ieee80211_op_hw_scan,
  .cancel_hw_scan           = _wv_ieee80211_op_cancel_hw_scan,
  .get_connection_alive     = _wv_ieee80211_op_get_connection_alive,
  .set_rts_threshold        = _wv_ieee80211_op_set_rts_threshold,
  .get_txpower              = _wv_ieee80211_op_get_txpower,
  .sta_statistics           = _wv_ieee80211_op_sta_statistics,
  .get_survey               = _wv_ieee80211_op_get_survey,
  .get_et_sset_count        = _wv_ieee80211_op_get_et_sset_count,
  .get_et_stats             = _wv_ieee80211_op_get_et_stats,
  .get_et_strings           = _wv_ieee80211_op_get_et_strings,
  .get_antenna              = _wv_ieee80211_op_get_antenna,
  .set_antenna              = _wv_ieee80211_op_set_antenna,
  .handle_radar_debug       = _wv_ieee80211_op_handle_radar_debug,
  .driver_alloc_netdev      = _wv_ieee80211_op_driver_alloc_netdev,
  .driver_free_netdev_allowed = _wv_ieee80211_op_driver_free_netdev_allowed,
  .prepare_cac_start        = _wv_ieee80211_op_prepare_cac_start,
  .notify_cac_finished      = _wv_ieee80211_op_notify_cac_finished,
  .get_vap_stats            = _wv_ieee80211_op_get_vap_stats,
  .remain_on_channel        = _wv_ieee80211_remain_on_channel,
  .cancel_remain_on_channel = _wv_ieee80211_cancel_remain_on_channel,
};

/* Not all VIFs did CSA in time, report which did, this
 * should result in mac80211 bringing interfaces down */
static uint32
_wv_csa_timer_expired (mtlk_osal_timer_t *timer, mtlk_handle_t data)
{
  wv_mac80211_t *mac80211 = (wv_mac80211_t *)data;

  WLOG_V("Not all VAPs did CSA in time");
  _wv_all_vifs_csa_finish(mac80211);

  return 0;
}

wv_mac80211_t * __MTLK_IFUNC
wv_ieee80211_init (struct device *dev, void *radio_ctx, mtlk_handle_t hw_handle)
{
  struct ieee80211_hw *hw;
  wv_mac80211_t *mac80211 = NULL;
  wave_radio_t *radio = (wave_radio_t *)radio_ctx;

  /* Actual MAC address will be set in mtlk_mbss_send_vap_activate according
   * to the value in param_db for the specific VAP
   * Currently we do not know the VAP index.
   * This will only happen on wv_ieee80211_start
   */
  u8 addr[ETH_ALEN] = { 0 };

  MTLK_ASSERT(NULL != radio);
  if (radio == NULL) {
    return NULL;
  }

  hw = ieee80211_alloc_hw(sizeof(wv_mac80211_t), &wv_ieee80211_ops);
  if (!hw){
    ELOG_V("ieee80211_alloc_hw failed");
    return NULL;
  }

  /* retrieve mac80211 handle */
  mac80211 = hw->priv;

  MTLK_ASSERT(mac80211 != NULL);
  if (mac80211 == NULL) {
    ELOG_V("mac80211 phy initialization failed, error retrieving mac80211 from hw->priv");
    return NULL;
  }

  /* link mac80211 with radio module */
  mac80211->radio = radio;
  mac80211->mac80211_hw = hw;
  if (mtlk_osal_timer_init(&mac80211->csa_timer, _wv_csa_timer_expired, HANDLE_T(mac80211))) {
    return NULL;
  }

  SET_IEEE80211_DEV(hw, dev);
  SET_IEEE80211_PERM_ADDR (hw, addr);

  return mac80211;
}

mtlk_error_t __MTLK_IFUNC
wv_mac80211_NDP_send_to_all_APs (wv_mac80211_t *mac80211,
  mtlk_nbuf_t *nbuf_ndp, BOOL power_mgmt_on, BOOL wait_for_ack)
{
  mtlk_error_t res = MTLK_ERR_OK;
  unsigned max_vaps_count = 0;
  unsigned vap_index;
  frame_head_t *wifi_header;
  uint64 cookie;

  MTLK_ASSERT(NULL != mac80211);

  if (!__wv_mac80211_get_sta_vifs_exist(mac80211))
    return res;

  wifi_header = (frame_head_t *) nbuf_ndp->data;
  max_vaps_count = __wv_mac80211_max_vaps_count_get(mac80211);
  for (vap_index = 0; vap_index < max_vaps_count; vap_index++) {
    struct wv_vif_priv *curr_vif_priv = _wv_mac80211_get_vif_priv(mac80211, vap_index);
    mtlk_vap_handle_t curr_vap_handle;
    mtlk_core_t *curr_core;
    sta_entry *sta;
    const IEEE_ADDR *addr;

    if (curr_vif_priv == NULL) {
      continue;
    }

    curr_vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(curr_vif_priv->df_user));
    curr_core = mtlk_vap_get_core(curr_vap_handle);

    if (NET_STATE_CONNECTED != mtlk_core_get_net_state(curr_core)) {
      /* Core is not ready */
      continue;
    }

    if (!mtlk_vap_is_sta(curr_vap_handle)) {
      /* vap is not sta */
      continue;
    }

    if (IEEE80211_STA_AUTHORIZED != curr_vif_priv->current_state) {
      /* VSTA not connected to AP */
      continue;
    }

    sta = mtlk_stadb_get_ap(mtlk_core_get_stadb(curr_core));
    if (!sta) {
      ELOG_D("CID-%04x: failed to find peer ap in station db", mtlk_vap_get_oid(curr_vap_handle));
      continue;
    }

    addr = mtlk_sta_get_addr(sta);

    ieee_addr_set(&wifi_header->dst_addr, addr->au8Addr);
    ieee_addr_set(&wifi_header->src_addr, curr_vif_priv->vif->addr);
    ieee_addr_set(&wifi_header->bssid, addr->au8Addr);
    mtlk_sta_decref(sta);

    if (wait_for_ack) {
      mtlk_osal_event_reset(&curr_core->ndp_acked);
      curr_core->waiting_for_ndp_ack = TRUE;
    }

    ILOG2_DY("CID-%04x: Sending NDP to %Y", mtlk_vap_get_oid(curr_vap_handle),
      wifi_header->dst_addr.au8Addr);
    res = mtlk_mmb_bss_mgmt_tx(curr_vap_handle, nbuf_ndp->data, sizeof(frame_head_t),
      0, FALSE, !wait_for_ack, FALSE /* unicast */ ,
      &cookie, PROCESS_NULL_DATA_PACKET, NULL,
      power_mgmt_on, MAC80211_WIDAN_NDP_TID);
    if (res != MTLK_ERR_OK) {
      ILOG1_DDS("CID-%04x: Send null data frame error: res=%d (%s)",
               mtlk_vap_get_oid(curr_vap_handle),
               res, mtlk_get_error_text(res));
      curr_core->waiting_for_ndp_ack = FALSE;
      return res;
    }
    if (wait_for_ack) {
      res = mtlk_osal_event_wait(&curr_core->ndp_acked,
        MAC80211_WIDAN_NDP_TIMEOUT);
      curr_core->waiting_for_ndp_ack = FALSE;
      if (res != MTLK_ERR_OK)
        break;
    }
  }
  return res;
}

mtlk_error_t __MTLK_IFUNC
wv_ieee80211_eapol_rx (wv_mac80211_t * mac80211,
                       struct ieee80211_vif *vif,
                       void *data, int data_len,
                       nl80211_band_e band)
{
  struct sk_buff *skb;
  u8 *skb_data, *ptr_data;
  uint16 proto = cpu_to_be16(ETH_P_PAE);
  size_t skb_size, ieee80211_hdr_size, size_left;
  struct ieee80211_rx_status *rx_status;
  struct ieee80211_hdr ieee80211_hdr = {0};
  struct ethhdr *ether_header = (struct ethhdr *)data;

  ieee80211_hdr_size = sizeof(ieee80211_hdr) - sizeof(ieee80211_hdr.addr4); /* 3 address mode */
  skb_size = data_len
             + ieee80211_hdr_size                     /* mac80211 expects ieee80211 header */
             + sizeof(rfc1042_header) + sizeof(proto) /* put LLC/SNAP header */
             - sizeof(*ether_header);                 /* remove existing ETH header */
  skb = dev_alloc_skb(skb_size);
  if (!skb) {
    ELOG_V("Couldn't allocate RX skb frame");
    return MTLK_ERR_NO_MEM;
  }

  skb_data = skb_put(skb, skb_size);
  ptr_data = skb_data;
  size_left = skb_size;

  ieee80211_hdr.frame_control = cpu_to_le16(IEEE80211_FTYPE_DATA | IEEE80211_STYPE_DATA);
  if (vif->type == NL80211_IFTYPE_AP) {
    ieee80211_hdr.frame_control |= cpu_to_le16(IEEE80211_FCTL_TODS);
    mtlk_osal_copy_eth_addresses(ieee80211_hdr.addr3, ether_header->h_dest);
  }
  else if (vif->type == NL80211_IFTYPE_STATION) {
    ieee80211_hdr.frame_control |= cpu_to_le16(IEEE80211_FCTL_FROMDS);
    mtlk_osal_copy_eth_addresses(ieee80211_hdr.addr3, ether_header->h_source);
  }

  mtlk_osal_copy_eth_addresses(ieee80211_hdr.addr1, ether_header->h_dest);
  mtlk_osal_copy_eth_addresses(ieee80211_hdr.addr2, ether_header->h_source);
  wave_memcpy(skb_data, size_left, &ieee80211_hdr, ieee80211_hdr_size);
  ptr_data += ieee80211_hdr_size;
  size_left -= ieee80211_hdr_size;

  wave_memcpy(ptr_data, size_left, rfc1042_header, sizeof(rfc1042_header));
  ptr_data += sizeof(rfc1042_header);
  size_left -= sizeof(rfc1042_header);

  /* ETH_P_PAE in SNAP required for mac80211 to not drop data frame for yet
   * unauthorized STA and to send frame on control port via NL80211 */
  *(uint16 *)ptr_data = proto;
  ptr_data += sizeof(proto);
  size_left -= sizeof(proto);

  wave_memcpy(ptr_data, size_left, data + sizeof(*ether_header), data_len - sizeof(*ether_header));

  /* Fill minimum required RX status */
  rx_status = IEEE80211_SKB_RXCB(skb);
  memset(rx_status, 0, sizeof(*rx_status));
  rx_status->band = band;
  rx_status->flag |= RX_FLAG_DECRYPTED;
  rx_status->flag |= RX_FLAG_SKIP_MONITOR;
  rx_status->flag |= (RX_FLAG_MMIC_STRIPPED | RX_FLAG_IV_STRIPPED);

  ILOG2_V("RX EAPOL frame");
  ieee80211_rx_ni(mac80211->mac80211_hw, skb);
  return MTLK_ERR_OK;
}
