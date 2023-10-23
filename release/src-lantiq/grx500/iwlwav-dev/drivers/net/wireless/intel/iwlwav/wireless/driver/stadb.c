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
* Written by: Dmitry Fleytman
*
*/
#include "mtlkinc.h"

#include "core.h"
#include "core_config.h"
#include "stadb.h"
#include "mtlk_osal.h"
#include "core.h"
#include "mtlk_coreui.h"
#include "mtlk_param_db.h"
#include "mtlkwlanirbdefs.h"
#include "mtlk_snprintf.h"
#include "mtlk_df_nbuf.h"
#include "mtlk_df.h"
#include "mtlk_dfg.h"
#include "mtlk_df_priv.h"

#define LOG_LOCAL_GID   GID_STADB
#define LOG_LOCAL_FID   1

#define STADB_FLAGS_STOPPED      0x20000000
#define DEFAULT_HOSTDB_TIMEOUT 86400 /* 24 hours */

/******************************************************************************************
 * GLOBAL STA DB API
 ******************************************************************************************/

static __INLINE mtlk_atomic_t *
__mtlk_global_stadb_get_cntr(void)
{
  global_stadb_t *db = mtlk_dfg_get_driver_stadb();
  return &db->sta_cnt;
}

BOOL __MTLK_IFUNC
mtlk_global_stadb_is_empty (void)
{
  return (mtlk_osal_atomic_get(__mtlk_global_stadb_get_cntr()) == 0);
}

#ifdef MTLK_PER_RATE_STAT
MTLK_HASH_DEFINE_EXTERN(rate, mtlk_rate_key,
                        _mtlk_hash_rate_hashval,
                        _mtlk_hash_rate_keycmp);

MTLK_HASH_DEFINE_EXTERN(per, mtlk_per_key,
                        _mtlk_hash_per_hashval,
                        _mtlk_hash_per_keycmp);

static int mtlk_num_of_stations_supports_per_rate_stats = 0;
#define MTLK_RATES_STAT_HASH_NOF_BUCKETS 7
#endif

/******************************************************************************************
 * STA API
 ******************************************************************************************/

static const uint32 _mtlk_sta_wss_id_map[] =
{
  MTLK_WWSS_WLAN_STAT_ID_DISCARD_PACKETS_RECEIVED,                      /* MTLK_STAI_CNT_DISCARD_PACKETS_RECEIVED */

#if MTLK_MTIDL_PEER_STAT_FULL
  MTLK_WWSS_WLAN_STAT_ID_RX_PACKETS_DISCARDED_DRV_DUPLICATE,           /* MTLK_STAI_CNT_RX_PACKETS_DISCARDED_DRV_DUPLICATE */
  MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_NO_RESOURCES,        /* MTLK_STAI_CNT_TX_PACKETS_DISCARDED_DRV_NO_RESOURCES */
  MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_SQ_OVERFLOW,         /* MTLK_STAI_CNT_TX_PACKETS_DISCARDED_SQ_OVERFLOW */
  MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_DROP_ALL_FILTER,     /* MTLK_STAI_CNT_TX_PACKETS_DISCARDED_DROP_ALL_FILTER */
  MTLK_WWSS_WLAN_STAT_ID_TX_PACKETS_DISCARDED_DRV_TX_QUEUE_OVERFLOW,   /* MTLK_STAI_CNT_TX_PACKETS_DISCARDED_TX_QUEUE_OVERFLOW */

  MTLK_WWSS_WLAN_STAT_ID_802_1X_PACKETS_RECEIVED,                      /* MTLK_STAI_CNT_802_1X_PACKETS_RECEIVED                                         */
  MTLK_WWSS_WLAN_STAT_ID_802_1X_PACKETS_SENT,                          /* MTLK_STAI_CNT_802_1X_PACKETS_SENT                                             */
  MTLK_WWSS_WLAN_STAT_ID_802_1X_PACKETS_DISCARDED,                     /* MTLK_STAI_CNT_802_1X_PACKETS_DISCARDED                                        */

  MTLK_WWSS_WLAN_STAT_ID_FWD_RX_PACKETS,                               /* MTLK_STAI_CNT_FWD_RX_PACKETS */
  MTLK_WWSS_WLAN_STAT_ID_FWD_RX_BYTES,                                 /* MTLK_STAI_CNT_FWD_RX_BYTES */
  MTLK_WWSS_WLAN_STAT_ID_PS_MODE_ENTRANCES,                            /* MTLK_STAI_CNT_PS_MODE_ENTRANCES */
#endif /* MTLK_MTIDL_PEER_STAT_FULL */
};

/* counters will be modified with checking ALLOWED option */
#define _mtlk_sta_inc_cnt(sta, id)         { if (id##_ALLOWED) __mtlk_sta_inc_cnt(sta, id); }
#define _mtlk_sta_add_cnt(sta, id, val)    { if (id##_ALLOWED) __mtlk_sta_add_cnt(sta, id, val); }


static __INLINE uint32
__mtlk_sta_get_phy_rate_synched_to_psdu_rate (const sta_entry *sta)
{
  return sta->info.stats.last_phy_rate_synched_to_psdu_rate;
}

static __INLINE uint32
__mtlk_sta_get_phy_rate_synched_to_psdu_rate_kbps (const sta_entry *sta)
{
  return MTLK_BITRATE_TO_KBPS(__mtlk_sta_get_phy_rate_synched_to_psdu_rate(sta));
}

static __INLINE uint32
__mtlk_sta_get_max_phy_rate_synched_to_psdu_rate(const sta_entry *sta)
{
  MTLK_ASSERT(sta);
  return sta->info.stats.max_phy_rate_synched_to_psdu_rate;
}

static __INLINE uint32
__mtlk_sta_get_max_phy_rate_synched_to_psdu_rate_kbps(const sta_entry *sta)
{
  return MTLK_BITRATE_TO_KBPS(__mtlk_sta_get_max_phy_rate_synched_to_psdu_rate(sta));
}

static __INLINE void
__mtlk_sta_inc_cnt (sta_entry        *sta,
                   sta_info_cnt_id_e cnt_id)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_STAI_CNT_LAST);

  mtlk_wss_cntr_inc(sta->wss_hcntrs[cnt_id]);
}

static __INLINE void
__mtlk_sta_add_cnt (sta_entry        *sta,
                   sta_info_cnt_id_e cnt_id,
                   uint32 val)
{
  MTLK_ASSERT(cnt_id >= 0 && cnt_id < MTLK_STAI_CNT_LAST);

  mtlk_wss_cntr_add(sta->wss_hcntrs[cnt_id], val);
}

static __INLINE int8
__mtlk_sta_get_snr (const sta_entry *sta, int antenna_idx)
{
  return sta->info.stats.snr[antenna_idx];
}

static __INLINE uint32
__mtlk_sta_rate_info_to_rate (const sta_entry *sta, const char *path, mtlk_bitrate_info_t rate_info)
{
  uint32    rx_rate;

  rx_rate = mtlk_bitrate_info_to_rate(rate_info);
  if (MTLK_BITRATE_INVALID == rx_rate) {
      ILOG2_YSD("STA:%Y %s rx_rate is not found for bitrate_info 0x%08X",
                mtlk_sta_get_addr(sta), path, (uint32)rate_info);
  }

  return rx_rate;
}

static uint32
_mtlk_sta_get_rx_mgmt_rate (const sta_entry *sta)
{
  MTLK_ASSERT(sta);

  return __mtlk_sta_rate_info_to_rate(sta, "Management", sta->info.stats.rx_mgmt_rate_info);
}

static uint32
_mtlk_sta_get_tx_data_rate (const sta_entry *sta)
{
  return mtlk_bitrate_params_to_rate(sta->info.stats.tx_data_rate_params);
}

static uint32
_mtlk_sta_get_max_tx_data_rate (const sta_entry *sta)
{
  return sta->info.stats.max_tx_data_rate;
}

static uint8
_mtlk_sta_airtime_usage_get (const sta_entry *sta)
{
  return sta->info.stats.airtime_stats.airtime_usage;
}

static uint32
_mtlk_sta_airtime_efficiency_get (const sta_entry *sta)
{
  return sta->info.stats.airtime_stats.airtime_efficiency;
}

static BOOL
_mtlk_sta_get_is_sta_auth (const sta_entry *sta)
{
  return((MTLK_PCKT_FLTR_ALLOW_ALL == sta->info.filter) ? TRUE : FALSE);
}

static __INLINE uint32
__mtlk_sta_get_tx_data_rate_kbps (const sta_entry *sta)
{
  return MTLK_BITRATE_TO_KBPS(_mtlk_sta_get_tx_data_rate(sta));
}

static __INLINE uint32
__mtlk_sta_get_max_tx_data_rate_kbps (const sta_entry *sta)
{
  return MTLK_BITRATE_TO_KBPS(_mtlk_sta_get_max_tx_data_rate(sta));
}

void __MTLK_IFUNC
mtlk_sta_update_rx_rate_rssi_on_man_frame (sta_entry *sta, const mtlk_phy_info_t *phy_info)
{
    unsigned i;

    MTLK_ASSERT(sta);
    MTLK_ASSERT(phy_info);

    sta->info.stats.rx_mgmt_rate_info = phy_info->bitrate_info;

    sta->info.stats.mgmt_max_rssi = phy_info->max_rssi;
    for (i = 0; i < MTLK_ARRAY_SIZE(sta->info.stats.mgmt_rssi); i++) {
        sta->info.stats.mgmt_rssi[i] = phy_info->rssi[i];
    }
}

static uint32 __MTLK_IFUNC
_mtlk_sta_keepalive_tmr (mtlk_osal_timer_t *timer,
                         mtlk_handle_t      clb_usr_data)
{
  sta_entry *sta = HANDLE_T_PTR(sta_entry, clb_usr_data);
  mtlk_osal_msec_t timeout = sta->paramdb->keepalive_interval;

  sta->paramdb->cfg.api.on_sta_keepalive(sta->paramdb->cfg.api.usr_data, *mtlk_sta_get_addr(sta));
  return timeout; /* restart with same timeout */
}

static void
_mtlk_sta_reset_cnts (sta_entry *sta)
{
  int i = 0;

  for (; i < MTLK_STAI_CNT_LAST; i++) {
    mtlk_wss_reset_stat(sta->wss, i);
  }
}

static void
_mtlk_sta_set_packets_filter_default (sta_entry *sta)
{
  sta->info.filter = MTLK_PCKT_FLTR_DISCARD_ALL;
}

/******************************************************************************************/

void __MTLK_IFUNC
mtlk_sta_update_mhi_peers_stats (sta_entry *sta)
{
  uint32 value;
  mtlk_mhi_stats_sta_cntr_t *stat_cntrs;

  MTLK_ASSERT(sta);
  stat_cntrs = mtlk_sta_get_mhi_stat_array(sta);
  /* Currently only one wss counter should be updated */
  value = mtlk_sta_get_cnt(sta, MTLK_STAI_CNT_DISCARD_PACKETS_RECEIVED);  /* last */
  value = stat_cntrs->swUpdateDrop - value;            /* delta = new - last */
  _mtlk_sta_add_cnt(sta, MTLK_STAI_CNT_DISCARD_PACKETS_RECEIVED, value);  /* update with delta */
}


static __INLINE void
_mtlk_sta_get_peer_traffic_stats (const sta_entry* sta, mtlk_wssa_peer_traffic_stats_t* stats)
{
  mtlk_mhi_stats_sta_cntr_t *stat_cntrs;
  MTLK_ASSERT((sta) || (stats));

  stat_cntrs = mtlk_sta_get_mhi_stat_array(sta);
  stats->PacketsSent      = stat_cntrs->successCount + stat_cntrs->exhaustedCount;
  stats->PacketsReceived  = stat_cntrs->rdCount;
  stats->BytesSent        = stat_cntrs->mpduByteTransmitted;
  stats->BytesReceived    = stat_cntrs->rxOutStaNumOfBytes;
}

static __INLINE void
_mtlk_sta_get_peer_retrans_stats (const sta_entry* sta, mtlk_wssa_retrans_stats_t* retrans)
{
  mtlk_mhi_stats_sta_cntr_t *stat_cntrs;
  MTLK_ASSERT((sta) || (retrans));

  stat_cntrs = mtlk_sta_get_mhi_stat_array(sta);

  retrans->Retransmissions    = 0; /* Not available from FW */
  retrans->RetransCount       = stat_cntrs->packetRetransCount;
  retrans->RetryCount         = stat_cntrs->oneOrMoreRetryCount;
  retrans->FailedRetransCount = stat_cntrs->exhaustedCount;
  retrans->MultipleRetryCount = stat_cntrs->txRetryCount;
}

void __MTLK_IFUNC
mtlk_sta_get_associated_dev_stats (const sta_entry* sta, peerFlowStats* peer_stats)
{
  MTLK_ASSERT(sta);
  MTLK_ASSERT(peer_stats);

  peer_stats->cli_rx_bytes   = sta->sta_stats64_cntrs.rxOutStaNumOfBytes;
  peer_stats->cli_tx_bytes   = sta->sta_stats64_cntrs.mpduByteTransmitted;
  peer_stats->cli_rx_frames  = sta->sta_stats64_cntrs.rdCount;
  peer_stats->cli_tx_frames  = sta->sta_stats64_cntrs.mpduTransmitted;
  peer_stats->cli_rx_retries = sta->sta_stats64_cntrs.rxRetryCount;
  peer_stats->cli_tx_retries = sta->sta_stats64_cntrs.txRetryCount;
  peer_stats->cli_rx_errors  = sta->sta_stats64_cntrs.swUpdateDrop + sta->sta_stats64_cntrs.rdDuplicateDrop + sta->sta_stats64_cntrs.missingSn;
  peer_stats->cli_tx_errors  = sta->sta_stats64_cntrs.tx_errors;

  peer_stats->cli_tx_rate    = MTLK_BITRATE_TO_KBPS(wave_peer_analyzer_get_tx_rate(&sta->info.sta_analyzer));
  peer_stats->cli_rx_rate    = MTLK_BITRATE_TO_KBPS(wave_peer_analyzer_get_rx_rate(&sta->info.sta_analyzer));

  mtlk_hw_get_peer_rssi_snapshot_ack(sta, &peer_stats->cli_rssi_ack);
}

static void
_mtlk_sta_get_peer_stats (const sta_entry* sta, mtlk_wssa_drv_peer_stats_t* stats)
{
  int i;

  MTLK_ASSERT(sta);

  mtlk_sta_get_tr181_peer_stats(sta, &stats->tr181_stats);

  for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
    stats->ShortTermRSSIAverage[i] = mtlk_sta_get_short_term_rssi(sta, i);
    stats->snr[i]                  = __mtlk_sta_get_snr(sta, i);
    stats->LongTermNoiseAverage[i] = mtlk_sta_get_long_term_noise(sta, i);
  }

  stats->AirtimeUsage      = _mtlk_sta_airtime_usage_get(sta);
  stats->AirtimeEfficiency = _mtlk_sta_airtime_efficiency_get(sta);
}

static __INLINE uint64
__wave_sta_get_errors_sent_stats (const sta_entry* sta) {
  return sta->sta_stats64_cntrs.exhaustedCount +
         sta->sta_stats64_cntrs.dropCntReasonClassifier +
         sta->sta_stats64_cntrs.dropCntReasonDisconnect +
         sta->sta_stats64_cntrs.dropCntReasonATF +
         sta->sta_stats64_cntrs.dropCntReasonTSFlush +
         sta->sta_stats64_cntrs.dropCntReasonReKey +
         sta->sta_stats64_cntrs.dropCntReasonSetKey +
         sta->sta_stats64_cntrs.dropCntReasonDiscard +
         sta->sta_stats64_cntrs.dropCntReasonDsabled +
         sta->sta_stats64_cntrs.dropCntReasonAggError;
}

void
mtlk_sta_get_tr181_peer_stats (const sta_entry* sta, mtlk_wssa_drv_tr181_peer_stats_t *stats)
{
  MTLK_ASSERT(sta);

  stats->StationId          = mtlk_sta_get_sid(sta);
  stats->NetModesSupported  = sta->info.sta_net_modes;

  _mtlk_sta_get_peer_traffic_stats(sta, &stats->traffic_stats);
  _mtlk_sta_get_peer_retrans_stats(sta, &stats->retrans_stats);
  stats->ErrorsSent = __wave_sta_get_errors_sent_stats(sta);

  stats->LastDataDownlinkRate = __mtlk_sta_get_tx_data_rate_kbps(sta);
  stats->LastDataUplinkRate   = __mtlk_sta_get_phy_rate_synched_to_psdu_rate_kbps(sta);

  stats->SignalStrength = sta->info.stats.max_rssi;
}

void
mtlk_core_get_driver_sta_info (const sta_entry* sta, struct driver_sta_info *stats)
{
  MTLK_ASSERT(sta);

  _mtlk_sta_get_peer_stats(sta, &stats->peer_stats);
  _mtlk_sta_get_peer_rates_info(sta, &stats->rates_info);
}

static void
_mtlk_sta_get_peer_capabilities(const sta_entry* sta, mtlk_wssa_drv_peer_capabilities_t* capabilities)
{
  uint16 ht_capabilities_info;
  uint8 zero_rx_mask[WV_HT_MCS_MASK_LEN];
  struct ieee80211_sta * mac80211_sta;
  memset(capabilities, 0, sizeof(*capabilities));
  memset(&ht_capabilities_info, 0, sizeof(ht_capabilities_info));
  memset(zero_rx_mask, 0, sizeof(zero_rx_mask));

  capabilities->NetModesSupported     = sta->info.sta_net_modes;
  capabilities->WMMSupported          = MTLK_BFIELD_GET(sta->info.flags, STA_FLAGS_WMM);
  capabilities->Vendor                = sta->info.vendor;

  /* Parse u16HTCapabilityInfo according to
   * IEEE Std 802.11-2012 -- 8.4.2.58.2 HT Capabilities Info field */
  rcu_read_lock();
  mac80211_sta = wv_sta_entry_get_mac80211_sta(sta);
  if (mac80211_sta->ht_cap.ht_supported && memcmp(zero_rx_mask, mac80211_sta->ht_cap.mcs.rx_mask,
     sizeof(zero_rx_mask)) != 0){
	  ht_capabilities_info   = MAC_TO_HOST16(mac80211_sta->ht_cap.cap);
  }
  rcu_read_unlock();
  capabilities->LDPCSupported         = MTLK_BFIELD_GET(ht_capabilities_info, MTLK_STA_HTCAP_LDPC_SUPPORTED);
  capabilities->CBSupported           = MTLK_BFIELD_GET(ht_capabilities_info, MTLK_STA_HTCAP_CB_SUPPORTED);
  capabilities->SGI20Supported        = MTLK_BFIELD_GET(ht_capabilities_info, MTLK_STA_HTCAP_SGI20_SUPPORTED);
  capabilities->SGI40Supported        = MTLK_BFIELD_GET(ht_capabilities_info, MTLK_STA_HTCAP_SGI40_SUPPORTED);
  capabilities->Intolerant_40MHz      = MTLK_BFIELD_GET(ht_capabilities_info, MTLK_STA_HTCAP_40MHZ_INTOLERANT);
  capabilities->MIMOConfigTX          = MTLK_BFIELD_GET(ht_capabilities_info, MTLK_STA_HTCAP_MIMO_CONFIG_TX);
  capabilities->MIMOConfigRX          = MTLK_BFIELD_GET(ht_capabilities_info, MTLK_STA_HTCAP_MIMO_CONFIG_RX);
  capabilities->STBCSupported         = (capabilities->MIMOConfigTX || capabilities->MIMOConfigRX);

  /* Parse AMPDU_Parameters according to
   * IEEE Std 802.11-2012 -- 8.4.2.58.3 A-MPDU Parameters field */
  capabilities->AMPDUMaxLengthExp     = (uint8)MTLK_BFIELD_GET(sta->info.ampdu_param, MTLK_STA_AMPDU_PARAMS_MAX_LENGTH_EXP);
  capabilities->AMPDUMinStartSpacing  = (uint8)MTLK_BFIELD_GET(sta->info.ampdu_param, MTLK_STA_AMPDU_PARAMS_MIN_START_SPACING);

  /* Parse tx_bf_capabilities according to
   * IEEE Std 802.11-2012 -- 8.4.2.58.6 Transmit Beamforming Capabilities */
  /* We assume that explicit beamforming is supported in case at least one BF capability is supported */
  capabilities->BFSupported           = !!(MAC_TO_HOST32(sta->info.tx_bf_cap_info));

  capabilities->AssociationTimestamp  = mtlk_osal_time_passed_ms(sta->connection_timestamp);
}

static void
_mtlk_sta_fill_rate_info_by_invalid (mtlk_wssa_drv_peer_rate_info1_t *info)
{
  info->InfoFlag = FALSE;
  info->PhyMode = -1;
  info->Scp     = -1;
  info->Mcs     = -1;
  info->Nss     =  0;
  info->CbwIdx  =  0;
  info->CbwMHz  =  0;
}

static uint32
_mtlk_sta_rate_cbw_to_cbw_in_mhz (uint32 cbw)
{
  uint32 cbw_in_mhz[] = { 20, 40, 80, 160 };

  if (cbw >= ARRAY_SIZE(cbw_in_mhz)) {
    WLOG_DD("Wrong CBW value %u (max %d expected)", cbw, ARRAY_SIZE(cbw_in_mhz));
    cbw = 0;
  }
  return cbw_in_mhz[cbw];
}

static void
_mtlk_sta_fill_rate_info_by_params (mtlk_wssa_drv_peer_rate_info1_t *info, mtlk_bitrate_params_t params)
{
  info->InfoFlag = TRUE;
  mtlk_bitrate_params_get_hw_params(params,
                                    &info->PhyMode, &info->CbwIdx, &info->Scp, &info->Mcs, &info->Nss);
  info->CbwMHz  = _mtlk_sta_rate_cbw_to_cbw_in_mhz(info->CbwIdx);
}

/* BOOL flag: TRUE - rate parameters, FALSE - stat rate_info */
static void
_mtlk_sta_fill_rate_info_by_info (mtlk_wssa_drv_peer_rate_info1_t *info, mtlk_bitrate_info_t rate_info)
{
  info->InfoFlag = mtlk_bitrate_info_get_flag(rate_info); /* TRUE for bitfields */
  if(info->InfoFlag) {
    info->PhyMode =  mtlk_bitrate_info_get_mode(rate_info);
    info->Scp     =  mtlk_bitrate_info_get_scp(rate_info);
    info->Mcs     =  mtlk_bitrate_info_get_mcs(rate_info);
    info->Nss     =  mtlk_bitrate_info_get_nss(rate_info);
    info->CbwIdx  =  mtlk_bitrate_info_get_cbw(rate_info);
    info->CbwMHz  = _mtlk_sta_rate_cbw_to_cbw_in_mhz(info->CbwIdx);
  } else { /* unknown */
    _mtlk_sta_fill_rate_info_by_invalid(info);
  }
}

static void
_mtlk_sta_fill_rate_info_by_psdu_info (mtlk_wssa_drv_peer_rate_info1_t *info, mtlk_bitrate_info16_t psdu_rate_info)
{
  uint8 mcs, nss;

  info->InfoFlag = TRUE;
  info->PhyMode  = mtlk_bitrate_params_get_psdu_mode(psdu_rate_info);
  info->CbwIdx   = mtlk_bitrate_params_get_psdu_cbw(psdu_rate_info);
  info->CbwMHz   = _mtlk_sta_rate_cbw_to_cbw_in_mhz(info->CbwIdx);
  info->Scp      = mtlk_bitrate_params_get_psdu_scp(psdu_rate_info);

  mtlk_bitrate_params_get_psdu_mcs_and_nss(psdu_rate_info, &mcs, &nss);
  info->Mcs = mcs;
  info->Nss = nss;
}

void __MTLK_IFUNC mtlk_core_get_tx_power_data(mtlk_core_t *core, mtlk_tx_power_data_t *tx_power_data);

void
_mtlk_sta_get_peer_rates_info(const sta_entry *sta, mtlk_wssa_drv_peer_rates_info_t *rates_info)
{
  mtlk_tx_power_data_t  tx_pw_data;
  mtlk_core_t *mcore;
  wave_radio_t *radio;
  unsigned      cbw;
  unsigned      radio_idx;

  MTLK_ASSERT(NULL != sta);
  MTLK_ASSERT(NULL != rates_info);

  memset(rates_info, 0, sizeof(*rates_info));
  mcore = mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(sta->vap_handle));
  MTLK_ASSERT(NULL != mcore);

  radio = wave_vap_radio_get(mcore->vap_handle);
  radio_idx = wave_radio_id_get(radio);

  mtlk_core_get_tx_power_data(mcore, &tx_pw_data);

  /* TX: downlink info */
  rates_info->TxDataRate  = _mtlk_sta_get_tx_data_rate(sta);
  if (MTLK_BITRATE_INVALID != rates_info->TxDataRate) {
    _mtlk_sta_fill_rate_info_by_params(&rates_info->tx_data_rate_info, sta->info.stats.tx_data_rate_params);
  } else {
    _mtlk_sta_fill_rate_info_by_invalid(&rates_info->tx_data_rate_info);
  }

  cbw = rates_info->tx_data_rate_info.CbwIdx;

  /* Current power is for current antennas number */
  rates_info->TxPwrCur    = POWER_TO_MBM(tx_pw_data.cur_ant_gain +
                                mtlk_hw_sta_stat_to_power(mtlk_vap_get_hw(sta->vap_handle), radio_idx,
                                    mtlk_sta_get_mhi_tx_stat_power_data(sta),
                                    cbw));

  /* Don't apply ant_gain in 11B mode */
  rates_info->TxMgmtPwr   = POWER_TO_MBM(
                            ((PHY_MODE_B == mtlk_sta_get_mhi_tx_stat_phymode_mgmt(sta)) ?
                                0 : tx_pw_data.cur_ant_gain) +
                            mtlk_hw_sta_stat_to_power(mtlk_vap_get_hw(sta->vap_handle), radio_idx,
                            mtlk_sta_get_mhi_tx_stat_power_mgmt(sta), 0));

  rates_info->TxBfMode    = mtlk_sta_get_mhi_tx_stat_bf_mode(sta);
  rates_info->TxStbcMode  = mtlk_sta_get_mhi_tx_stat_stbc_mode(sta);

  /* RX: uplink info */
  rates_info->RxMgmtRate = _mtlk_sta_get_rx_mgmt_rate(sta);
  _mtlk_sta_fill_rate_info_by_info(&rates_info->rx_mgmt_rate_info, sta->info.stats.rx_mgmt_rate_info);

  rates_info->RxDataRate = __mtlk_sta_get_phy_rate_synched_to_psdu_rate(sta);
  _mtlk_sta_fill_rate_info_by_psdu_info(&rates_info->rx_data_rate_info, sta->info.stats.rx_psdu_data_rate_info);
}

MTLK_INIT_STEPS_LIST_BEGIN(sta_entry)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_entry, STA_LOCK)
#ifdef MTLK_PER_RATE_STAT
  MTLK_INIT_STEPS_LIST_ENTRY(sta_entry, PER_RATE_STAT)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(sta_entry, KALV_TMR)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_entry, WSS_NODE)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_entry, WSS_HCNTRs)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_entry, PEER_ANALYZER)
MTLK_INIT_INNER_STEPS_BEGIN(sta_entry)
MTLK_INIT_STEPS_LIST_END(sta_entry);

MTLK_START_STEPS_LIST_BEGIN(sta_entry)
  MTLK_START_STEPS_LIST_ENTRY(sta_entry, KALV_TMR)
  MTLK_START_STEPS_LIST_ENTRY(sta_entry, ENABLE_FILTER)
MTLK_START_INNER_STEPS_BEGIN(sta_entry)
MTLK_START_STEPS_LIST_END(sta_entry);

static void _mtlk_sta_timer_cleanup(mtlk_osal_timer_t *timer){
	mtlk_osal_timer_cleanup(timer);
	kfree_tag(timer);
}

#ifdef MTLK_PER_RATE_STAT

#define MAC_ADDR_AS_STRING_SIZE (IEEE_ADDR_LEN*3)

void __MTLK_IFUNC mtlk_sta_rate_stat_clean_hash (sta_entry *sta)
{
  mtlk_hash_t *hash = &sta->per_rate_stat_hash_tx;
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(rate) *h;
  MTLK_HASH_ENTRY_T(per)  *p;
  h = mtlk_hash_enum_first_rate(hash, &e);
  while (h)
  {
    mtlk_rate_stat_tx *stat = MTLK_CONTAINER_OF(h, mtlk_rate_stat_tx, hentry);
    mtlk_hash_remove_rate(hash, &stat->hentry);
    mtlk_osal_mem_free(stat);

    h = mtlk_hash_enum_next_rate(hash, &e);
  }

  hash = &sta->per_rate_stat_hash_rx;
  h = mtlk_hash_enum_first_rate(hash, &e);
  while (h)
  {
    mtlk_rate_stat_rx *stat = MTLK_CONTAINER_OF(h, mtlk_rate_stat_rx, hentry);
    mtlk_hash_remove_rate(hash, &stat->hentry);
    mtlk_osal_mem_free(stat);

    h = mtlk_hash_enum_next_rate(hash, &e);
  }

  hash = &sta->per_rate_stat_hash_packet_error_rate;
  p = mtlk_hash_enum_first_per(hash, &e);
  while (p)
  {
    mtlk_rate_stat_per *stat = MTLK_CONTAINER_OF(p, mtlk_rate_stat_per, hentry);
    mtlk_hash_remove_per(hash, &stat->hentry);
    mtlk_osal_mem_free(stat);

    p = mtlk_hash_enum_next_per(hash, &e);
  }

  mtlk_hash_cleanup_rate(&sta->per_rate_stat_hash_tx);
  mtlk_hash_cleanup_rate(&sta->per_rate_stat_hash_rx);
  mtlk_hash_cleanup_per(&sta->per_rate_stat_hash_packet_error_rate);
}

static void _mtlk_sta_rate_stat_cleanup (sta_entry *sta)
{
  char name[MAC_ADDR_AS_STRING_SIZE];
  int res;
  mtlk_df_t *df = mtlk_vap_get_df(sta->vap_handle);
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);

  if (sta->rate_stat_possible)
    mtlk_num_of_stations_supports_per_rate_stats--;

  res = mtlk_mac_to_str(mtlk_sta_get_addr(sta), name, MAC_ADDR_AS_STRING_SIZE);
  if (res != MTLK_ERR_OK) {
    ELOG_Y("procs of station %Y could not be removed", mtlk_sta_get_addr(sta));
    goto end;
  }

  mtlk_df_proc_node_remove_entry(name, mtlk_df_user_get_per_stat_tx_proc_node(df_user));
  mtlk_df_proc_node_remove_entry(name, mtlk_df_user_get_per_stat_rx_proc_node(df_user));
  mtlk_df_proc_node_remove_entry(name, mtlk_df_user_get_per_stat_packet_error_rate_proc_node(df_user));

end:
  mtlk_sta_rate_stat_clean_hash(sta);
}
#endif

static void
_mtlk_sta_cleanup (sta_entry *sta)
{
  MTLK_CLEANUP_BEGIN(sta_entry, MTLK_OBJ_PTR(sta))
    MTLK_CLEANUP_STEP(sta_entry, PEER_ANALYZER, MTLK_OBJ_PTR(sta),
                      mtlk_peer_analyzer_cleanup, (&sta->info.sta_analyzer));
    MTLK_CLEANUP_STEP(sta_entry, WSS_HCNTRs, MTLK_OBJ_PTR(sta),
                      mtlk_wss_cntrs_close, (sta->wss, sta->wss_hcntrs, ARRAY_SIZE(sta->wss_hcntrs)));
    MTLK_CLEANUP_STEP(sta_entry, WSS_NODE, MTLK_OBJ_PTR(sta),
                      mtlk_wss_delete, (sta->wss));
    MTLK_CLEANUP_STEP(sta_entry, KALV_TMR, MTLK_OBJ_PTR(sta),
    		          _mtlk_sta_timer_cleanup, (sta->keepalive_timer));
#ifdef MTLK_PER_RATE_STAT
    MTLK_CLEANUP_STEP(sta_entry, PER_RATE_STAT, MTLK_OBJ_PTR(sta),
                      _mtlk_sta_rate_stat_cleanup, (sta));
#endif
    MTLK_CLEANUP_STEP(sta_entry, STA_LOCK, MTLK_OBJ_PTR(sta),
                      mtlk_osal_lock_cleanup, (&sta->lock));
  MTLK_CLEANUP_END(sta_entry, MTLK_OBJ_PTR(sta))
}

#ifdef MTLK_PER_RATE_STAT
static int _mtlk_sta_rate_stat_packet_error_rate_proc_read (char *page, off_t off, int count, void *data)
{ 
  sta_entry *sta = mtlk_df_proc_entry_get_df(data);
  mtlk_hash_t *hash = &sta->per_rate_stat_hash_packet_error_rate;
  mtlk_hash_enum_t e;
  int res = 0;
  int skip;
  MTLK_HASH_ENTRY_T(per) *h; 
    
  skip = off/sizeof(perRatePERStats);
    
  h = mtlk_hash_enum_first_per(hash, &e);
  mtlk_osal_lock_acquire(&sta->lock);
  while (h)
  {
    mtlk_rate_stat_per *stat = MTLK_CONTAINER_OF(h, mtlk_rate_stat_per, hentry);
    if (count < (ssize_t)sizeof(stat->per)) {
      goto end;
    }
    if (skip) {
      skip--;
      h = mtlk_hash_enum_next_per(hash, &e);
      continue;
    }
    wave_memcpy(page, count, &stat->per, sizeof(stat->per));
    page += sizeof(stat->per);
    count -= sizeof(stat->per);
    res += sizeof(stat->per);
    h = mtlk_hash_enum_next_per(hash, &e);
  }
end:
  mtlk_osal_lock_release(&sta->lock);
  return res;
}

static int _mtlk_sta_rate_stat_rx_proc_read (char *page, off_t off, int count, void *data)
{
  sta_entry *sta = mtlk_df_proc_entry_get_df(data);
  mtlk_hash_t *hash = &sta->per_rate_stat_hash_rx;
  mtlk_hash_enum_t e;
  int res = 0;
  int skip;
  MTLK_HASH_ENTRY_T(rate) *h;

  skip = off/sizeof(peerRateInfoRxStats);

  h = mtlk_hash_enum_first_rate(hash, &e);
  mtlk_osal_lock_acquire(&sta->lock);
  while (h)
  {
    mtlk_rate_stat_rx *stat = MTLK_CONTAINER_OF(h, mtlk_rate_stat_rx, hentry);
    if (count < (ssize_t)sizeof(stat->rx)) {
      goto end;
    }
    if (skip) {
      skip--;
      h = mtlk_hash_enum_next_rate(hash, &e);
      continue;
    }
    wave_memcpy(page, count, &stat->rx, sizeof(stat->rx));
    page += sizeof(stat->rx);
    count -= sizeof(stat->rx);
    res += sizeof(stat->rx);
    h = mtlk_hash_enum_next_rate(hash, &e);
  }
end:
  mtlk_osal_lock_release(&sta->lock);
  return res;
}

static int _mtlk_sta_rate_stat_tx_proc_read (char *page, off_t off, int count, void *data)
{
  sta_entry *sta = mtlk_df_proc_entry_get_df(data);
  mtlk_hash_t *hash = &sta->per_rate_stat_hash_tx;
  mtlk_hash_enum_t e;
  int res = 0;
  int skip;
  MTLK_HASH_ENTRY_T(rate) *h;

  skip = off/sizeof(peerRateInfoTxStats);

  h = mtlk_hash_enum_first_rate(hash, &e);
  mtlk_osal_lock_acquire(&sta->lock);
  while (h)
  {
    mtlk_rate_stat_tx *stat = MTLK_CONTAINER_OF(h, mtlk_rate_stat_tx, hentry);
    if (count < (ssize_t)sizeof(stat->tx)) {
      goto end;
    }
    if (skip) {
      skip--;
      h = mtlk_hash_enum_next_rate(hash, &e);
      continue;
    }
    wave_memcpy(page, count, &stat->tx, sizeof(stat->tx));
    page += sizeof(stat->tx);
    count -= sizeof(stat->tx);
    res += sizeof(stat->tx);
    h = mtlk_hash_enum_next_rate(hash, &e);
  }
end:
  mtlk_osal_lock_release(&sta->lock);
  return res;
}

int __MTLK_IFUNC
mtlk_sta_rate_stat_hash_init (sta_entry * sta)
{
  int res = mtlk_hash_init_rate(&sta->per_rate_stat_hash_tx, MTLK_RATES_STAT_HASH_NOF_BUCKETS);
  if (MTLK_ERR_OK != res) {
    return res;
  }
  res = mtlk_hash_init_rate(&sta->per_rate_stat_hash_rx, MTLK_RATES_STAT_HASH_NOF_BUCKETS);
  if (MTLK_ERR_OK != res) {
    mtlk_hash_cleanup_rate(&sta->per_rate_stat_hash_tx);
    return res;
  }
  res = mtlk_hash_init_per(&sta->per_rate_stat_hash_packet_error_rate, MTLK_RATES_STAT_HASH_NOF_BUCKETS);
  if (MTLK_ERR_OK != res) {
    mtlk_hash_cleanup_rate(&sta->per_rate_stat_hash_rx);
    mtlk_hash_cleanup_rate(&sta->per_rate_stat_hash_tx);
    return res;
  }
  return res;
}

static int
_mtlk_sta_rate_stat_init (sta_entry * sta)
{
  char name[MAC_ADDR_AS_STRING_SIZE];
  int res = MTLK_ERR_OK;
  mtlk_df_t *df = mtlk_vap_get_df(sta->vap_handle);
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  res = mtlk_sta_rate_stat_hash_init(sta);
  if (MTLK_ERR_OK != res) {
    goto end;
  }

  res = mtlk_mac_to_str(mtlk_sta_get_addr(sta), name, MAC_ADDR_AS_STRING_SIZE);
  if (MTLK_ERR_OK != res) {
    goto err;
  }

  res = mtlk_df_proc_node_add_ro_entry(
          name, mtlk_df_user_get_per_stat_tx_proc_node(df_user), sta, _mtlk_sta_rate_stat_tx_proc_read);
  if (MTLK_ERR_OK != res) {
    goto err;
  }

  res = mtlk_df_proc_node_add_ro_entry(
          name, mtlk_df_user_get_per_stat_rx_proc_node(df_user), sta, _mtlk_sta_rate_stat_rx_proc_read);
  if (MTLK_ERR_OK != res) {
    goto err_rx;
  }

  res = mtlk_df_proc_node_add_ro_entry(
          name, mtlk_df_user_get_per_stat_packet_error_rate_proc_node(df_user), sta,
          _mtlk_sta_rate_stat_packet_error_rate_proc_read);
  if (MTLK_ERR_OK != res) {
    goto err_per;
  }

  return res;

err_per:
  mtlk_df_proc_node_remove_entry(name, mtlk_df_user_get_per_stat_rx_proc_node(df_user));
err_rx:
  mtlk_df_proc_node_remove_entry(name, mtlk_df_user_get_per_stat_tx_proc_node(df_user));
err:
  mtlk_sta_rate_stat_clean_hash(sta);
end:
  return res;
}
#endif

static int
_mtlk_sta_init (sta_entry *sta,
                sta_info* info_cfg,
                mtlk_vap_handle_t vap_handle,
                sta_db *paramdb)
{
  MTLK_STATIC_ASSERT(ARRAY_SIZE(_mtlk_sta_wss_id_map)  == MTLK_STAI_CNT_LAST);
  MTLK_STATIC_ASSERT(ARRAY_SIZE(sta->wss_hcntrs) == MTLK_STAI_CNT_LAST);

  sta->keepalive_timer = kmalloc_tag(sizeof(mtlk_osal_timer_t), GFP_KERNEL, MTLK_MEM_TAG_TIMER);
  if (!sta->keepalive_timer) {
    ELOG_V("Can't allocate station keepalive timer");
    return MTLK_ERR_NO_MEM;
  }

  if (&sta->info != info_cfg){
    sta->info = *info_cfg;
  }

  sta->paramdb = paramdb;
  sta->vap_handle = vap_handle;

  /* Invalidate all rate parameters */
  sta->info.stats.tx_data_rate_params = MTLK_BITRATE_PARAMS_INVALID;
  sta->info.stats.rx_data_rate_info   = MTLK_BITRATE_INFO_INVALID;
  sta->info.stats.rx_mgmt_rate_info   = MTLK_BITRATE_INFO_INVALID;

#ifdef MTLK_PER_RATE_STAT
  if (MTLK_PER_RATE_STAT_MAX_STA_NUM > mtlk_num_of_stations_supports_per_rate_stats)
  {
    sta->rate_stat_possible = TRUE;
    mtlk_num_of_stations_supports_per_rate_stats++;
  }
#endif

  MTLK_INIT_TRY(sta_entry, MTLK_OBJ_PTR(sta))
    MTLK_INIT_STEP(sta_entry, STA_LOCK, MTLK_OBJ_PTR(sta),
                   mtlk_osal_lock_init, (&sta->lock));
#ifdef MTLK_PER_RATE_STAT
    MTLK_INIT_STEP(sta_entry, PER_RATE_STAT, MTLK_OBJ_PTR(sta),
                   _mtlk_sta_rate_stat_init, (sta));
#endif
    MTLK_INIT_STEP(sta_entry, KALV_TMR, MTLK_OBJ_PTR(sta),
                   mtlk_osal_timer_init, (sta->keepalive_timer,
                                          _mtlk_sta_keepalive_tmr,
                                          HANDLE_T(sta)));
    MTLK_INIT_STEP_EX(sta_entry, WSS_NODE, MTLK_OBJ_PTR(sta),
                      mtlk_wss_create, (paramdb->wss, _mtlk_sta_wss_id_map, ARRAY_SIZE(_mtlk_sta_wss_id_map)),
                      sta->wss, sta->wss != NULL, MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(sta_entry, WSS_HCNTRs, MTLK_OBJ_PTR(sta),
                   mtlk_wss_cntrs_open, (sta->wss, _mtlk_sta_wss_id_map, sta->wss_hcntrs, MTLK_STAI_CNT_LAST));
    MTLK_INIT_STEP(sta_entry, PEER_ANALYZER, MTLK_OBJ_PTR(sta),
                   mtlk_peer_analyzer_init, (&sta->info.sta_analyzer,
                   wave_hw_mmb_get_stats_poll_period(mtlk_vap_get_hw(vap_handle))));
  MTLK_INIT_FINALLY(sta_entry, MTLK_OBJ_PTR(sta))
  MTLK_INIT_RETURN(sta_entry, MTLK_OBJ_PTR(sta), _mtlk_sta_cleanup, (sta))
}

/* WARNING: the function _mtlk_sta_stop() cannot be called from a spinlock context
 * due to mtlk_irbd_cleanup() calls wait functions !!! */
static __INLINE void
_mtlk_sta_stop (sta_entry *sta)
{
  MTLK_STOP_BEGIN(sta_entry, MTLK_OBJ_PTR(sta))
    MTLK_STOP_STEP(sta_entry, ENABLE_FILTER, MTLK_OBJ_PTR(sta),
                   MTLK_NOACTION, ());
    MTLK_STOP_STEP(sta_entry, KALV_TMR, MTLK_OBJ_PTR(sta),
                   mtlk_osal_timer_cancel_sync, (sta->keepalive_timer));
  MTLK_STOP_END(sta_entry, MTLK_OBJ_PTR(sta))
}

static __INLINE int
_mtlk_sta_start (sta_entry *sta, const IEEE_ADDR *addr, BOOL dot11n_mode)
{

  MTLK_START_TRY(sta_entry, MTLK_OBJ_PTR(sta))

    sta->connection_timestamp = mtlk_osal_timestamp();
    sta->activity_timestamp   = sta->connection_timestamp;

    sta->info.cipher       = IW_ENCODE_ALG_NONE;

    MTLK_START_STEP(sta_entry, KALV_TMR, MTLK_OBJ_PTR(sta),
                    mtlk_osal_timer_set, (sta->keepalive_timer,
                                           sta->paramdb->keepalive_interval ?
                                           sta->paramdb->keepalive_interval :
                                           DEFAULT_KEEPALIVE_TIMEOUT));
    MTLK_START_STEP_VOID(sta_entry, ENABLE_FILTER, MTLK_OBJ_PTR(sta),
                         _mtlk_sta_set_packets_filter_default, (sta));
  MTLK_START_FINALLY(sta_entry, MTLK_OBJ_PTR(sta))
  MTLK_START_RETURN(sta_entry, MTLK_OBJ_PTR(sta), _mtlk_sta_stop, (sta))
}

void __MTLK_IFUNC
mtlk_sta_on_packet_sent (sta_entry *sta, uint32 nbuf_len, uint32 nbuf_flags)
{
  ASSERT(sta != NULL);

  if (0 != nbuf_len) { /*skip null packets */
#if MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED
    mtlk_peer_analyzer_process_tx_packet(&sta->info.sta_analyzer, nbuf_len);
#endif
  }
}

/* with DEBUG statistic */
#if MTLK_MTIDL_PEER_STAT_FULL

void __MTLK_IFUNC
mtlk_sta_on_packet_dropped(sta_entry *sta, mtlk_tx_drop_reasons_e reason)
{
  __mtlk_sta_inc_cnt(sta, reason);
}

#endif /* MTLK_MTIDL_PEER_STAT_FULL */

void __MTLK_IFUNC
mtlk_sta_on_packet_indicated(sta_entry *sta, mtlk_nbuf_t *nbuf, uint32 nbuf_flags)
{
#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED
  uint32 data_length;
  ASSERT(sta != NULL);
  data_length = mtlk_df_nbuf_get_data_length(nbuf);

  mtlk_peer_analyzer_process_rx_packet(&sta->info.sta_analyzer, data_length);
#endif
}

void __MTLK_IFUNC
mtlk_sta_on_rx_packet_802_1x(sta_entry *sta)
{
  ASSERT(sta != NULL);
  _mtlk_sta_inc_cnt(sta, MTLK_STAI_CNT_802_1X_PACKETS_RECEIVED);
}

void __MTLK_IFUNC
mtlk_sta_on_tx_packet_802_1x(sta_entry *sta)
{
  ASSERT(sta != NULL);
  _mtlk_sta_inc_cnt(sta, MTLK_STAI_CNT_802_1X_PACKETS_SENT);
}

void __MTLK_IFUNC
mtlk_sta_on_tx_packet_discarded_802_1x(sta_entry *sta)
{
  ASSERT(sta != NULL);
  _mtlk_sta_inc_cnt(sta, MTLK_STAI_CNT_802_1X_PACKETS_DISCARDED);
}

void __MTLK_IFUNC
mtlk_sta_on_rx_packet_forwarded(sta_entry *sta, mtlk_nbuf_t *nbuf)
{
  ASSERT(sta != NULL);
  _mtlk_sta_inc_cnt(sta, MTLK_STAI_CNT_FWD_RX_PACKETS);
  _mtlk_sta_add_cnt(sta, MTLK_STAI_CNT_FWD_RX_BYTES, mtlk_df_nbuf_get_data_length(nbuf));
}

void __MTLK_IFUNC
mtlk_sta_update_phy_info (sta_entry *sta, mtlk_hw_t *hw, stationPhyRxStatusDb_t *sta_status)
{
    mtlk_mhi_stats_sta_tx_rate_t  *mhi_tx_stats;
    uint32 word;
    int8   noise;
    unsigned i;

    MTLK_STATIC_ASSERT(sizeof(sta->info.stats.data_rssi) >= sizeof(sta_status->rssi));
    MTLK_STATIC_ASSERT(PHY_STATISTICS_MAX_RX_ANT <= sizeof(sta_status->rssi));
    MTLK_STATIC_ASSERT(PHY_STATISTICS_MAX_RX_ANT <= sizeof(sta_status->noise));
    MTLK_STATIC_ASSERT(PHY_STATISTICS_MAX_RX_ANT <= sizeof(sta->info.stats.data_rssi));
    MTLK_STATIC_ASSERT(PHY_STATISTICS_MAX_RX_ANT <= sizeof(sta->info.stats.snr));

    mtlk_osal_lock_acquire(&sta->lock);

    /* Calculate RSSI [dBm] per antenna and MAX RSSI by PHY RSSI of STA status */
    sta->info.stats.max_rssi = mtlk_hw_get_rssi_max_by_rx_phy_rssi(hw,
                                sta_status->rssi, sta->info.stats.data_rssi);

    ILOG3_DDDDDD("SID %02X Updated rssi: %d, %d, %d, %d, max_rssi %d",
                mtlk_sta_get_sid(sta),
                sta->info.stats.data_rssi[0], sta->info.stats.data_rssi[1],
                sta->info.stats.data_rssi[2], sta->info.stats.data_rssi[3],
                sta->info.stats.max_rssi);

    for (i = 0; i < MTLK_ARRAY_SIZE(sta_status->noise); i++) {
      /* Don't process invalid RSSI PHY metric */
      if (RSSI_INVALID == sta->info.stats.data_rssi[i])
        continue;

      /* Convert PHY Noise to Noise in dBm */
      noise = mtlk_hw_noise_phy_to_noise_dbm(hw, sta_status->noise[i], sta_status->gain[i]);

      /* Don't update STA statistics on invalid data */
      if (NOISE_INVALID != noise) {
        /* Calculate SNR [dB] using RSSI [dBm] and Noise [dBm] */
        sta->info.stats.snr[i] = mtlk_calculate_snr(sta->info.stats.data_rssi[i], noise);

        ILOG3_DDDDDD("[%u] RSSI %d, gain %u, noise %u -> %d, SNR %d",
                     i, sta->info.stats.data_rssi[i], sta_status->gain[i],
                     sta_status->noise[i], noise, sta->info.stats.snr[i]);

        sta_status->noise[i] = noise;
      }
    }

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_LONG_RSSI_ALLOWED
    mtlk_peer_analyzer_process_rssi_sample(&sta->info.sta_analyzer, sta->info.stats.data_rssi);
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_LONG_NOISE_ALLOWED
    wave_peer_analyzer_process_noise_samples(&sta->info.sta_analyzer, sta_status->noise);
#endif

    mhi_tx_stats = mtlk_sta_get_mhi_tx_stats(sta);

    ILOG3_DDDDD("mode %d, cbw %d, scp %d, mcs %2d, nss %d",
                                 mhi_tx_stats->DataPhyMode,
                                 mhi_tx_stats->dataBwLimit,
                                 mhi_tx_stats->scpData,
                                 mhi_tx_stats->mcsData,
                                 mhi_tx_stats->nssData);

    sta->info.stats.tx_data_rate_params = MTLK_BITRATE_PARAMS_BY_RATE_PARAMS(
                                                   mhi_tx_stats->DataPhyMode,
                                                   mhi_tx_stats->dataBwLimit,
                                                   mhi_tx_stats->scpData,
                                                   mhi_tx_stats->mcsData,
                                                   mhi_tx_stats->nssData);

    word = MAC_TO_HOST32(sta_status->phyRate);
    sta->info.stats.rx_data_rate_info =
        MTLK_BITRATE_INFO_BY_PHY_RATE(MTLK_BFIELD_GET(word, PHY_RX_STATUS_PHY_RATE_VALUE));

    sta->info.stats.rx_psdu_data_rate_info =
        MTLK_BITRATE_INFO_BY_PSDU_RATE(sta_status->psduRate);

    sta->info.stats.perClientRxtimeUsage = sta_status->perClientRxtimeUsage;
    sta->info.stats.lastTsf = sta_status->lastTsf;
    sta->info.stats.irad = sta_status->irad;

    sta->info.stats.last_tx_data_rate = _mtlk_sta_get_tx_data_rate(sta);
    if(sta->info.stats.max_tx_data_rate < sta->info.stats.last_tx_data_rate) {
      sta->info.stats.max_tx_data_rate = sta->info.stats.last_tx_data_rate;
    }
    sta->info.stats.last_phy_rate_synched_to_psdu_rate = sta_status->phyRateSynchedToPsduRate;
    if(sta->info.stats.max_phy_rate_synched_to_psdu_rate < sta->info.stats.last_phy_rate_synched_to_psdu_rate) {
      sta->info.stats.max_phy_rate_synched_to_psdu_rate = sta->info.stats.last_phy_rate_synched_to_psdu_rate;
    }

    wave_peer_analyzer_proc_txrx_rates(&sta->info.sta_analyzer,
                                       sta->info.stats.last_tx_data_rate,
                                       sta->info.stats.last_phy_rate_synched_to_psdu_rate);

    mtlk_osal_lock_release(&sta->lock);
}

void __MTLK_IFUNC
mtlk_sta_set_packets_filter (sta_entry         *sta,
                             mtlk_pckt_filter_e filter_type)
{
  if (sta->info.filter != filter_type) {
    ILOG1_YDD("STA (%Y) filter: %d => %d", mtlk_sta_get_addr(sta), sta->info.filter, filter_type);
    sta->info.filter = filter_type;
  }
}

void __MTLK_IFUNC
mtlk_sta_get_peer_stats (const sta_entry* sta, mtlk_wssa_drv_peer_stats_t* stats)
{
  MTLK_ASSERT(sta != NULL);
  MTLK_ASSERT(stats != NULL);

  _mtlk_sta_get_peer_stats(sta, stats);
}

void __MTLK_IFUNC
mtlk_sta_get_peer_capabilities (const sta_entry* sta, mtlk_wssa_drv_peer_capabilities_t* capabilities)
{
  MTLK_ASSERT(sta != NULL);
  MTLK_ASSERT(capabilities != NULL);

  _mtlk_sta_get_peer_capabilities(sta, capabilities);
}

void __MTLK_IFUNC
wave_sta_get_dev_diagnostic_res2(mtlk_core_t *core, const sta_entry* sta, wifiAssociatedDevDiagnostic2_t *dev_diagnostic_stats)
{
  uint32 band_width;
  int i;
  static const char *operating_standard[]={"802.11a","802.11b","802.11g","802.11n","802.11ac","802.11ax"};
  struct mtlk_chan_def *current_chandef = __wave_core_chandef_get(core);
  MTLK_ASSERT(sta != NULL);
  MTLK_ASSERT(dev_diagnostic_stats != NULL);
  band_width = _mtlk_sta_rate_cbw_to_cbw_in_mhz(current_chandef->width);
  memset(dev_diagnostic_stats, 0, sizeof(wifiAssociatedDevDiagnostic2_t));
  dev_diagnostic_stats->MACAddress                         = *mtlk_sta_get_addr(sta);
  dev_diagnostic_stats->AuthenticationState                = true;
  dev_diagnostic_stats->LastDataDownlinkRate               = __mtlk_sta_get_tx_data_rate_kbps(sta);
  dev_diagnostic_stats->LastDataUplinkRate                 = __mtlk_sta_get_phy_rate_synched_to_psdu_rate_kbps(sta);
  dev_diagnostic_stats->SignalStrength                     = sta->info.stats.max_rssi;
  dev_diagnostic_stats->Retransmissions                    = 0; /* Not available from FW */
  dev_diagnostic_stats->Active                             = true;
  dev_diagnostic_stats->DataFramesSentAck                  = sta->sta_stats64_cntrs.successCount;
  dev_diagnostic_stats->DataFramesSentNoAck                = sta->sta_stats64_cntrs.exhaustedCount;
  dev_diagnostic_stats->BytesSent                          = sta->sta_stats64_cntrs.mpduByteTransmitted;
  dev_diagnostic_stats->BytesReceived                      = sta->sta_stats64_cntrs.rxOutStaNumOfBytes;
  dev_diagnostic_stats->MinRSSI                            = sta->sta_stats_cntrs.minRssi;
  dev_diagnostic_stats->MaxRSSI                            = sta->sta_stats_cntrs.maxRssi;
  mtlk_snprintf(dev_diagnostic_stats->OperatingChannelBandwidth, sizeof(dev_diagnostic_stats->OperatingChannelBandwidth), "%d", band_width);
  for(i = ARRAY_SIZE(operating_standard)-1; i >= 0; i--) {
    if (MTLK_BIT_GET(sta->info.sta_net_modes, i)) {
      /* sta_net_modes - bit 0 - 802.11a  */
      /* sta_net_modes - bit 1 - 802.11b  */
      /* sta_net_modes - bit 2 - 802.11g  */
      /* sta_net_modes - bit 3 - 802.11n  */
      /* sta_net_modes - bit 4 - 802.11ac */
      /* sta_net_modes - bit 5 - 802.11ax */
      wave_strcopy(dev_diagnostic_stats->OperatingStandard, operating_standard[i], sizeof(dev_diagnostic_stats->OperatingStandard));
      break;
    }
  }


  for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
    dev_diagnostic_stats->RSSI[i] = mtlk_sta_get_short_term_rssi(sta, i);
    dev_diagnostic_stats->SNR[i]  = __mtlk_sta_get_snr(sta, i);
  }

}

/******************************************************************************************/

/******************************************************************************************
 * STA DB API
 ******************************************************************************************/
#define MTLK_STADB_ITER_ADDBA_PERIOD     1000 /* msec */

#define MTLK_STADB_HASH_NOF_BUCKETS      16
#define MTLK_SID_HASH_NOF_BUCKETS        128

static uint32 __MTLK_IFUNC
mtlk_whm_tx_rx_stall_timer_clb (mtlk_osal_timer_t *timer, mtlk_handle_t data)
{
  sta_db* stadb = HANDLE_T_PTR(sta_db, data);
  if (wave_rcvry_mac_fatal_pending_get(mtlk_vap_get_hw(stadb->vap_handle)))
    return MTLK_ERR_OK;

  _mtlk_df_user_invoke_core_async(mtlk_vap_get_df(stadb->vap_handle), WAVE_CORE_REQ_GET_TX_RX_WARN,
                                  NULL, 0, NULL,0);
  return (WHM_TXRX_STALL_TIMEOUT * MTLK_OSAL_MSEC_IN_SEC);
}

static uint32 __MTLK_IFUNC
mtlk_whm_discnct_timer_clb (mtlk_osal_timer_t *timer, mtlk_handle_t data)
{
  sta_db* stadb = HANDLE_T_PTR(sta_db, data);
  MTLK_HASH_ENTRY_T(ieee_addr)  *h;
  mtlk_hash_enum_t              e;
  sta_whm_entry                 *whm_entry;

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&stadb->whm_discnct_hash, &e);
  while (h) {
    whm_entry = MTLK_CONTAINER_OF(h, sta_whm_entry, hentry);
    if ((whm_entry->is_connected == FALSE) && (whm_entry->discnct_cnt_in_cycle == 0)) {
      mtlk_hash_remove_ieee_addr(&stadb->whm_discnct_hash, &whm_entry->hentry);
      mtlk_osal_mem_free(whm_entry);
    }
    else if ((whm_entry->is_connected == TRUE) && (whm_entry->discnct_cnt_in_cycle == 0)) {
      whm_entry->nof_discncts = 0;
      memset(whm_entry->discnct_timestamp, 0, sizeof(whm_entry->discnct_timestamp));
    }
    else {
      whm_entry->discnct_cnt_in_cycle = 0;
    }
    h = mtlk_hash_enum_next_ieee_addr(&stadb->whm_discnct_hash, &e);
  }
  mtlk_osal_lock_release(&stadb->lock);
  return (WHM_DISCNCT_TIMEOUT * MTLK_OSAL_MSEC_IN_SEC);
}

void __MTLK_IFUNC
__mtlk_sta_on_unref_private (sta_entry *sta)
{
  MTLK_ASSERT(sta != NULL);
  mtlk_osal_event_set(&sta->ref_cnt_event);
}

void __MTLK_IFUNC
mtlk_sta_wait_delete (struct ieee80211_sta *mac80211_sta){
  sta_entry * sta = wv_ieee80211_get_sta(mac80211_sta);
  mtlk_sta_decref(sta); /* For search */
  mtlk_sta_decref(sta); /* To get to zero */
  /* At that point is_added_to_driver is zero, so incref won't happen */
  if (mtlk_osal_atomic_get(&sta->ref_cnt)){
    int res = mtlk_osal_event_wait(&sta->ref_cnt_event, 4000);
    if (res != MTLK_ERR_OK){
      ELOG_YD("sta %Y cleanup error, refcnt= %d", mac80211_sta->addr,
              mtlk_osal_atomic_get(&sta->ref_cnt));
    }
  }

  mtlk_osal_event_cleanup(&sta->ref_cnt_event);
}

int __MTLK_IFUNC
mtlk_stadb_add_sta (sta_db *stadb, sta_entry * sta, sta_info *info_cfg)
{
  int                           res = MTLK_ERR_OK;
  sta_whm_entry                 *whm_entry;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  BOOL dot11n_mode;
  if (!sta) {
    res = MTLK_ERR_NO_MEM;
    WLOG_V("Can't find STA");
    goto err;
  }

  res = _mtlk_sta_init(sta, info_cfg, stadb->vap_handle, stadb);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init STA (err#%d)", res);
    goto err;
  }

  dot11n_mode = MTLK_BFIELD_GET(info_cfg->flags, STA_FLAGS_11n);
  res = _mtlk_sta_start(sta, mtlk_sta_get_addr(sta), dot11n_mode);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't start STA (err#%d)", res);
    goto err_start;
  }

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_insert_ieee_addr(&stadb->hash, mtlk_sta_get_addr(sta), &sta->hentry);
  if (h == NULL) {
    ++stadb->hash_cnt;

    mtlk_hash_insert_sid(&stadb->sid_hash, &info_cfg->sid, &sta->hentry_sid);
  }
  mtlk_sta_incref(sta);
  mtlk_osal_lock_release(&stadb->lock);

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_find_ieee_addr(&stadb->whm_discnct_hash, mtlk_sta_get_addr(sta));
  if (h) {
    whm_entry = MTLK_CONTAINER_OF(h, sta_whm_entry, hentry);
    whm_entry->is_connected = TRUE;
  }
  mtlk_osal_lock_release(&stadb->lock);

  if (mtlk_sta_info_is_4addr(info_cfg))
    mtlk_osal_atomic_inc(&stadb->four_addr_sta_cnt);

  ILOG3_YP("PEER %Y added (%p)", mtlk_sta_get_addr(sta)->au8Addr, sta);
  mtlk_osal_atomic_inc(__mtlk_global_stadb_get_cntr());
  wave_radio_sta_cnt_inc(wave_vap_radio_get(stadb->vap_handle));
  sta->is_added_to_driver = TRUE;

  return res;

err_start:
  _mtlk_sta_cleanup(sta);
err:
  return res;
}

static void
_mtlk_stadb_remove_sta_from_stadb_internal (sta_db* stadb, sta_entry *sta)
{
  const IEEE_ADDR *addr = mtlk_sta_get_addr(sta);

  mtlk_mc_drop_sta(mtlk_vap_get_core(stadb->vap_handle), addr, sta->secondary_vap_id);

  if (mtlk_sta_is_4addr(sta))
    mtlk_osal_atomic_dec(&stadb->four_addr_sta_cnt);

  ILOG3_Y("PEER %Y removed", addr->au8Addr);
}

void __MTLK_IFUNC
mtlk_stadb_remove_sta (sta_db* stadb, sta_entry *sta)
{
  sta_whm_entry                 *whm_entry;
  MTLK_HASH_ENTRY_T(ieee_addr)  *h;
  uint32                        index;
  mtlk_osal_timestamp_diff_t    diff;

  /* Decrease global counter */
  mtlk_osal_atomic_dec(__mtlk_global_stadb_get_cntr());

  wave_radio_sta_cnt_dec(wave_vap_radio_get(stadb->vap_handle));

  /* Stop STA (_mtlk_sta_stop() cannot be called under spinlock) */
  _mtlk_sta_stop(sta);
  _mtlk_sta_cleanup(sta);

  /* Remove STA from STADB */
  mtlk_osal_lock_acquire(&stadb->lock);

  --stadb->hash_cnt;
  mtlk_hash_remove_ieee_addr(&stadb->hash, &sta->hentry);

  mtlk_hash_remove_sid(&stadb->sid_hash, &sta->hentry_sid);

  _mtlk_stadb_remove_sta_from_stadb_internal(stadb, sta);
  mtlk_osal_lock_release(&stadb->lock);
  mtlk_sta_decref(sta); /* De-reference by STA DB hash */

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_find_ieee_addr(&stadb->whm_discnct_hash, mtlk_sta_get_addr(sta));
  if (!h) {
    /* STA disconnected first time */
    whm_entry = mtlk_osal_mem_alloc(sizeof(sta_whm_entry) , MTLK_MEM_TAG_STADB_ITER);
    if (!whm_entry){
      ELOG_D("CID-%04x: Can't allocate memory for whm", mtlk_vap_get_oid(stadb->vap_handle));
    } else {
      memset(whm_entry, 0, sizeof(sta_whm_entry));
      whm_entry->discnct_timestamp[0] = mtlk_osal_timestamp();
      whm_entry->nof_discncts++;
      whm_entry->discnct_cnt_in_cycle++;
      whm_entry->is_connected = FALSE;
      mtlk_hash_insert_ieee_addr(&stadb->whm_discnct_hash, mtlk_sta_get_addr(sta), &whm_entry->hentry);
    }
    mtlk_osal_lock_release(&stadb->lock);
  } else {
    /* STA Entry already exist */
    whm_entry = MTLK_CONTAINER_OF(h, sta_whm_entry, hentry);
    whm_entry->discnct_cnt_in_cycle++;
    whm_entry->is_connected = FALSE;
    index = whm_entry->nof_discncts % MAX_WHM_DISCONNECTIONS_PER_CYCLE;
    whm_entry->discnct_timestamp[index] = mtlk_osal_timestamp();
    diff = ((whm_entry->discnct_timestamp[index]) - (whm_entry->discnct_timestamp[index + 1]));
    mtlk_osal_lock_release(&stadb->lock);
    if (whm_entry->discnct_cnt_in_cycle >= MAX_WHM_DISCONNECTIONS_PER_CYCLE) {
      whm_entry->discnct_cnt_in_cycle = 0;
      ILOG0_YDDD("WHM : Sta (%Y) got disconnected %u times in a timespan of %u mins - Raising a warning ID : %d", mtlk_sta_get_addr(sta), MAX_WHM_DISCONNECTIONS_PER_CYCLE, (WHM_DISCNCT_TIMEOUT/60), WHM_DRV_STA_DISCONNECT);
      for (index = 0 ; index < MAX_WHM_DISCONNECTIONS_PER_CYCLE; index++) {
        ILOG0_DD("WHM : Sta Disconnect - count %u : timestamp : %u", index+1, mtlk_osal_timestamp_to_ms(whm_entry->discnct_timestamp[index]));
      }
      /* Trigger warning to WHM */
      wave_core_driver_warning_detection_ind_handle(WHM_DRV_STA_DISCONNECT);
    } else if (whm_entry->nof_discncts > MAX_WHM_DISCONNECTIONS_PER_CYCLE &&
               (mtlk_osal_timestamp_to_ms(diff) > (WHM_DISCNCT_TIMEOUT * MTLK_OSAL_MSEC_IN_SEC))) {
      ILOG0_YDDD("WHM : Sta (%Y) got disconnected %u times in a timespan of %u mins - Raising a warning ID : %d", mtlk_sta_get_addr(sta), MAX_WHM_DISCONNECTIONS_PER_CYCLE, (WHM_DISCNCT_TIMEOUT/60), WHM_DRV_STA_DISCONNECT);
      for (index = 0 ; index < MAX_WHM_DISCONNECTIONS_PER_CYCLE; index++) {
        ILOG0_DD("WHM : Sta Disconnect - count %u : timestamp : %u", index+1, mtlk_osal_timestamp_to_ms(whm_entry->discnct_timestamp[index]));
      }
      wave_core_driver_warning_detection_ind_handle(WHM_DRV_STA_DISCONNECT);
    } else {
      whm_entry->nof_discncts++;
    }
  }
  sta->is_added_to_driver = FALSE;
}

MTLK_INIT_STEPS_LIST_BEGIN(sta_db)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, WHM_TX_RX_STALL_TIMER)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, STADB_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, SID_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, STADB_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, REG_ABILITIES)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, EN_ABILITIES)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, WHM_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(sta_db, WHM_DISCNCT_TIMER)
MTLK_INIT_INNER_STEPS_BEGIN(sta_db)
MTLK_INIT_STEPS_LIST_END(sta_db);

MTLK_START_STEPS_LIST_BEGIN(sta_db)
  MTLK_START_STEPS_LIST_ENTRY(sta_db, WHM_TX_RX_STALL_TIMER)
  MTLK_START_STEPS_LIST_ENTRY(sta_db, WHM_DISCNCT_TIMER)
  MTLK_START_STEPS_LIST_ENTRY(sta_db, STADB_WSS_CREATE)
MTLK_START_INNER_STEPS_BEGIN(sta_db)
MTLK_START_STEPS_LIST_END(sta_db);

static const mtlk_ability_id_t _stadb_abilities[] = {
  WAVE_CORE_REQ_GET_STADB_STATUS
};

int __MTLK_IFUNC
mtlk_stadb_init (sta_db *stadb, mtlk_vap_handle_t vap_handle)
{
  MTLK_ASSERT(stadb != NULL);
  stadb->vap_handle = vap_handle;
  // Set default configuration
  stadb->keepalive_interval    = DEFAULT_KEEPALIVE_TIMEOUT;
  stadb->flags                |= STADB_FLAGS_STOPPED;
  stadb->hash_cnt              = 0;

  MTLK_INIT_TRY(sta_db, MTLK_OBJ_PTR(stadb))
    MTLK_INIT_STEP(sta_db, WHM_TX_RX_STALL_TIMER, MTLK_OBJ_PTR(stadb), 
                   mtlk_osal_timer_init, (&stadb->whm_tx_rx_stall_timer, mtlk_whm_tx_rx_stall_timer_clb, HANDLE_T(stadb)));
    MTLK_INIT_STEP(sta_db, STADB_HASH, MTLK_OBJ_PTR(stadb),
                   mtlk_hash_init_ieee_addr, (&stadb->hash, MTLK_STADB_HASH_NOF_BUCKETS))
    MTLK_INIT_STEP(sta_db, SID_HASH, MTLK_OBJ_PTR(stadb),
                   mtlk_hash_init_sid, (&stadb->sid_hash, MTLK_SID_HASH_NOF_BUCKETS))
    MTLK_INIT_STEP(sta_db, STADB_LOCK, MTLK_OBJ_PTR(stadb),
                   mtlk_osal_lock_init, (&stadb->lock));
    MTLK_INIT_STEP(sta_db, REG_ABILITIES, MTLK_OBJ_PTR(stadb),
                    mtlk_abmgr_register_ability_set,
                    (mtlk_vap_get_abmgr(stadb->vap_handle), _stadb_abilities, ARRAY_SIZE(_stadb_abilities)));
    MTLK_INIT_STEP_VOID(sta_db, EN_ABILITIES, MTLK_OBJ_PTR(stadb),
                        mtlk_abmgr_enable_ability_set,
                        (mtlk_vap_get_abmgr(stadb->vap_handle), _stadb_abilities, ARRAY_SIZE(_stadb_abilities)));
    MTLK_INIT_STEP(sta_db, WHM_HASH, MTLK_OBJ_PTR(stadb),
                   mtlk_hash_init_ieee_addr, (&stadb->whm_discnct_hash, MTLK_STADB_HASH_NOF_BUCKETS))
    MTLK_INIT_STEP(sta_db, WHM_DISCNCT_TIMER, MTLK_OBJ_PTR(stadb), 
                   mtlk_osal_timer_init, (&stadb->whm_discnct_timer, mtlk_whm_discnct_timer_clb, HANDLE_T(stadb)));

  MTLK_INIT_FINALLY(sta_db, MTLK_OBJ_PTR(stadb))
  MTLK_INIT_RETURN(sta_db, MTLK_OBJ_PTR(stadb), mtlk_stadb_cleanup, (stadb))
}

void __MTLK_IFUNC
mtlk_stadb_configure (sta_db *stadb, const sta_db_cfg_t *cfg)
{
  MTLK_ASSERT(stadb != NULL);
  MTLK_ASSERT(cfg != NULL);
  MTLK_ASSERT(cfg->max_nof_stas != 0);
  MTLK_ASSERT(cfg->parent_wss != NULL);
  MTLK_ASSERT(cfg->api.on_sta_keepalive != NULL);

  stadb->cfg = *cfg;
}

const sta_entry * __MTLK_IFUNC
mtlk_stadb_iterate_first (sta_db *stadb, mtlk_stadb_iterator_t *iter)
{
  int                           err = MTLK_ERR_UNKNOWN;
  const sta_entry *             res = NULL;
  uint32                        idx = 0;
  mtlk_hash_enum_t              e;
  wave_radio_t                 *radio;
  uint32                        max_nof_sta;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  MTLK_ASSERT(stadb != NULL);
  MTLK_ASSERT(iter != NULL);

  radio = wave_vap_radio_get(stadb->vap_handle);
  MTLK_ASSERT(radio != NULL);
  max_nof_sta = wave_radio_max_stas_get(radio);

  mtlk_osal_lock_acquire(&stadb->lock);

  iter->size = stadb->hash_cnt;

  if (!iter->size) {
    err = MTLK_ERR_NOT_IN_USE;
    goto end;
  }

  if (iter->size > max_nof_sta) {
    ELOG_DD("Number of supported STAs over: (%d > %d)", iter->size, max_nof_sta);
    err = MTLK_ERR_NOT_IN_USE;
    goto end;
  }

  iter->arr =
    (sta_entry**)mtlk_osal_mem_alloc(iter->size * sizeof(sta_entry *),
                                     MTLK_MEM_TAG_STADB_ITER);
  if (!iter->arr) {
    ELOG_D("Can't allocate iteration array of %d entries", iter->size);
    err = MTLK_ERR_NO_MEM;
    goto end;
  }

  memset(iter->arr, 0, iter->size * sizeof(sta_entry *));

  h = mtlk_hash_enum_first_ieee_addr(&stadb->hash, &e);
  while (idx < iter->size && h) {
    sta_entry *sta = MTLK_CONTAINER_OF(h, sta_entry, hentry);
    iter->arr[idx] = sta;
    mtlk_sta_incref(sta); /* Reference by iterator */
    ++idx;
    h = mtlk_hash_enum_next_ieee_addr(&stadb->hash, &e);
  }

  err = MTLK_ERR_OK;

end:
  mtlk_osal_lock_release(&stadb->lock);

  if (err == MTLK_ERR_OK) {
    iter->idx   = 0;

    res = mtlk_stadb_iterate_next(iter);
    if (!res) {
      mtlk_stadb_iterate_done(iter);
    }
  }

  return res;
}

const sta_entry * __MTLK_IFUNC
mtlk_stadb_iterate_next (mtlk_stadb_iterator_t *iter)
{
  const sta_entry *sta = NULL;

  MTLK_ASSERT(iter != NULL);
  MTLK_ASSERT(iter->arr != NULL);

  if (iter->idx < iter->size) {
    sta = iter->arr[iter->idx];
    ++iter->idx;
  }

  return sta;
}

void __MTLK_IFUNC
mtlk_stadb_iterate_done (mtlk_stadb_iterator_t *iter)
{
  uint32 idx = 0;

  MTLK_ASSERT(iter != NULL);
  MTLK_ASSERT(iter->arr != NULL);

  for (idx = 0; idx < iter->size; idx++) {
    mtlk_sta_decref(iter->arr[idx]); /* De-reference by iterator */
  }
  mtlk_osal_mem_free(iter->arr);
  memset(iter, 0, sizeof(*iter));
}

void __MTLK_IFUNC
mtlk_stadb_iterate_done_none_decref (mtlk_stadb_iterator_t *iter)
{
  MTLK_ASSERT(iter != NULL);
  MTLK_ASSERT(iter->arr != NULL);

  mtlk_osal_mem_free(iter->arr);
  memset(iter, 0, sizeof(*iter));
}

void __MTLK_IFUNC
wave_sta_get_dev_diagnostic_res3(mtlk_core_t *core, sta_entry* sta, wifiAssociatedDevDiagnostic3_t *dev_diagnostic_stats)
{
   uint8 agreement = 0, ru_allocation_count = 0, latest_ru = 0;
   MTLK_ASSERT(sta != NULL);
   MTLK_ASSERT(dev_diagnostic_stats != NULL);

   memset(dev_diagnostic_stats, 0, sizeof(wifiAssociatedDevDiagnostic3_t));
   wave_sta_get_dev_diagnostic_res2(core, sta, &dev_diagnostic_stats->wifiAssociatedDevDiagnostic2);

   dev_diagnostic_stats->PacketsSent        = sta->sta_stats64_cntrs.successCount + sta->sta_stats64_cntrs.exhaustedCount;
   dev_diagnostic_stats->PacketsReceived    = sta->sta_stats64_cntrs.rdCount;
   dev_diagnostic_stats->ErrorsSent         = __wave_sta_get_errors_sent_stats(sta);
   dev_diagnostic_stats->RetransCount       = sta->sta_stats64_cntrs.packetRetransCount;
   dev_diagnostic_stats->FailedRetransCount = sta->sta_stats64_cntrs.exhaustedCount;
   dev_diagnostic_stats->RetryCount         = sta->sta_stats64_cntrs.oneOrMoreRetryCount;
   dev_diagnostic_stats->MultipleRetryCount = sta->sta_stats64_cntrs.txRetryCount;
   dev_diagnostic_stats->MaxDownlinkRate    = __mtlk_sta_get_max_tx_data_rate_kbps(sta);
   dev_diagnostic_stats->MaxUplinkRate      = __mtlk_sta_get_max_phy_rate_synched_to_psdu_rate_kbps(sta);

   dev_diagnostic_stats->TwtParams.agreementType = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].agreementType;
   dev_diagnostic_stats->TwtParams.operation.implicit = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].operation.implicit;
   dev_diagnostic_stats->TwtParams.operation.announced = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].operation.announced;;
   dev_diagnostic_stats->TwtParams.operation.triggerEnabled = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].operation.triggerEnabled;
   dev_diagnostic_stats->TwtParams.patams.individual.wakeTime = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.wakeTime;
   dev_diagnostic_stats->TwtParams.patams.individual.wakeInterval = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.wakeInterval;
   dev_diagnostic_stats->TwtParams.patams.individual.minWakeDuration = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.minWakeDuration;
   dev_diagnostic_stats->TwtParams.patams.individual.channel = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.channel;

    dev_diagnostic_stats->UplinkMuStats.UpinkMuType = sta->sta_stats_cntrs.uplinkMuStats.uplinkMuType;
    latest_ru = sta->sta_stats_cntrs.uplinkMuStats.allocatedUplinkRuNum;
    if(sta->sta_stats_cntrs.uplinkMuStats.uplinkRuNumFlag){
      dev_diagnostic_stats->UplinkMuStats.AllocatedUplinkRuNum = HAL_MAX_RU_ALLOCATIONS_DRV;
    } else{
      dev_diagnostic_stats->UplinkMuStats.AllocatedUplinkRuNum = sta->sta_stats_cntrs.uplinkMuStats.allocatedUplinkRuNum;
    }
    for(ru_allocation_count = 0; ru_allocation_count < dev_diagnostic_stats->UplinkMuStats.AllocatedUplinkRuNum; ru_allocation_count++){
      dev_diagnostic_stats->UplinkMuStats.UplinkRuAllocations[ru_allocation_count].subchannels = sta->sta_stats_cntrs.uplinkMuStats.uplinkRuAllocations[latest_ru].subChannels;
      dev_diagnostic_stats->UplinkMuStats.UplinkRuAllocations[ru_allocation_count].type = sta->sta_stats_cntrs.uplinkMuStats.uplinkRuAllocations[latest_ru].type;
      if(latest_ru == 0){
        latest_ru = HAL_MAX_RU_ALLOCATIONS_DRV - 1;
      } else {
        latest_ru--;
      }
    }
    memset(&sta->sta_stats_cntrs.uplinkMuStats.uplinkRuAllocations, 0, sizeof(sta->sta_stats_cntrs.uplinkMuStats.uplinkRuAllocations));
    sta->sta_stats_cntrs.uplinkMuStats.allocatedUplinkRuNum = 0;
    for(ru_allocation_count = 0; ru_allocation_count < HAL_MAX_BSR_DRV; ru_allocation_count++) {
      switch(ru_allocation_count) {
        case 0: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBestEffort; break;
        case 1: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBestEffort; break;
        case 2: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBackground; break;
        case 3: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBackground; break;
        case 4: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVideo; break;
        case 5: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVideo; break;
        case 6: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVoice; break;
        case 7: dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVoice; break;
      }
        dev_diagnostic_stats->UplinkMuStats.BufferStatus[ru_allocation_count].queueSize = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[ru_allocation_count].queueSize;
    }

    dev_diagnostic_stats->DownlinkMuStats.DownlinkMuType = sta->sta_stats_cntrs.downlinkMuStats.downlinkMuType;
    latest_ru = sta->sta_stats_cntrs.downlinkMuStats.allocatedDownlinkRuNum;
    if(sta->sta_stats_cntrs.downlinkMuStats.downlinkRuNumFlag){
      dev_diagnostic_stats->DownlinkMuStats.AllocatedDownlinkRuNum = HAL_MAX_RU_ALLOCATIONS_DRV;
    } else{
      dev_diagnostic_stats->DownlinkMuStats.AllocatedDownlinkRuNum = sta->sta_stats_cntrs.downlinkMuStats.allocatedDownlinkRuNum;
    }
    for(ru_allocation_count = 0; ru_allocation_count < dev_diagnostic_stats->DownlinkMuStats.AllocatedDownlinkRuNum; ru_allocation_count++){
      dev_diagnostic_stats->DownlinkMuStats.DownlinkRuAllocations[ru_allocation_count].subchannels = sta->sta_stats_cntrs.downlinkMuStats.downlinkRuAllocations[ru_allocation_count].subChannels;
      dev_diagnostic_stats->DownlinkMuStats.DownlinkRuAllocations[ru_allocation_count].type = sta->sta_stats_cntrs.downlinkMuStats.downlinkRuAllocations[ru_allocation_count].type;
      if(latest_ru == 0){
        latest_ru = HAL_MAX_RU_ALLOCATIONS_DRV - 1;
      } else {
        latest_ru--;
      }
    }
    memset(&sta->sta_stats_cntrs.downlinkMuStats.downlinkRuAllocations, 0, sizeof(sta->sta_stats_cntrs.downlinkMuStats.downlinkRuAllocations));
    sta->sta_stats_cntrs.downlinkMuStats.allocatedDownlinkRuNum = 0;
    for(ru_allocation_count = 0; ru_allocation_count < HAL_MAX_BSR_DRV; ru_allocation_count++) {
      switch(ru_allocation_count) {
        case 0: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBestEffort; break;
        case 1: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBestEffort; break;
        case 2: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBackground; break;
        case 3: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryBackground; break;
        case 4: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVideo; break;
        case 5: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVideo; break;
        case 6: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVoice; break;
        case 7: dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].accessCategory = wlanAccessCategoryVoice; break;
      }
      dev_diagnostic_stats->DownlinkMuStats.BufferStatus[ru_allocation_count].queueSize = sta->sta_stats_cntrs.downlinkMuStats.dlBufferStatus[ru_allocation_count].queueSize;
    }

}

int __MTLK_IFUNC
mtlk_stadb_start (sta_db *stadb)
{
  MTLK_ASSERT(stadb != NULL);

  if (!(stadb->flags & STADB_FLAGS_STOPPED))
    return MTLK_ERR_OK;

  if (!(stadb->flags & STADB_FLAGS_STOPPED)) {
    return MTLK_ERR_OK;
  }

  MTLK_START_TRY(sta_db, MTLK_OBJ_PTR(stadb))
    MTLK_START_STEP(sta_db, WHM_TX_RX_STALL_TIMER, MTLK_OBJ_PTR(stadb),
                    mtlk_osal_timer_set, (&stadb->whm_tx_rx_stall_timer, WHM_TXRX_STALL_TIMEOUT * MTLK_OSAL_MSEC_IN_SEC));
    MTLK_START_STEP(sta_db, WHM_DISCNCT_TIMER, MTLK_OBJ_PTR(stadb),
                    mtlk_osal_timer_set, (&stadb->whm_discnct_timer, WHM_DISCNCT_TIMEOUT * MTLK_OSAL_MSEC_IN_SEC));
    MTLK_START_STEP_EX(sta_db, STADB_WSS_CREATE, MTLK_OBJ_PTR(stadb),
                       mtlk_wss_create, (stadb->cfg.parent_wss, NULL, 0),
                       stadb->wss, stadb->wss != NULL, MTLK_ERR_OK);
    stadb->flags &= ~STADB_FLAGS_STOPPED;
  MTLK_START_FINALLY(sta_db, MTLK_OBJ_PTR(stadb))
  MTLK_START_RETURN(sta_db, MTLK_OBJ_PTR(stadb), mtlk_stadb_stop, (stadb))
}

void __MTLK_IFUNC
mtlk_stadb_stop (sta_db *stadb)
{
  if (stadb->flags & STADB_FLAGS_STOPPED) {
    return;
  }

  MTLK_STOP_BEGIN(sta_db, MTLK_OBJ_PTR(stadb))
    MTLK_STOP_STEP(sta_db, STADB_WSS_CREATE, MTLK_OBJ_PTR(stadb),
                   mtlk_wss_delete, (stadb->wss));
    MTLK_STOP_STEP(sta_db, WHM_DISCNCT_TIMER, MTLK_OBJ_PTR(stadb),
                   mtlk_osal_timer_cancel_sync, (&stadb->whm_discnct_timer));
    MTLK_STOP_STEP(sta_db, WHM_TX_RX_STALL_TIMER, MTLK_OBJ_PTR(stadb),
                   mtlk_osal_timer_cancel_sync, (&stadb->whm_tx_rx_stall_timer));
    stadb->flags |= STADB_FLAGS_STOPPED;
  MTLK_STOP_END(sta_db, MTLK_OBJ_PTR(stadb))
}

void _wave_stadb_cleanup_whm_sta_list (sta_db *stadb)
{
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&stadb->whm_discnct_hash, &e);
  while (h) {
    sta_whm_entry *whm_entry = MTLK_CONTAINER_OF(h, sta_whm_entry, hentry);
    mtlk_hash_remove_ieee_addr(&stadb->whm_discnct_hash, &whm_entry->hentry);
    mtlk_osal_mem_free(whm_entry);
    h = mtlk_hash_enum_next_ieee_addr(&stadb->whm_discnct_hash, &e);
  }
  mtlk_osal_lock_release(&stadb->lock);

  mtlk_hash_cleanup_ieee_addr(&stadb->whm_discnct_hash);
}

static void mtlk_tx_rx_warn_timer_deinit(sta_db *stadb)
{
  if (!mtlk_osal_timer_is_cancelled(&stadb->whm_tx_rx_stall_timer)) {
    mtlk_osal_timer_cancel_sync(&stadb->whm_tx_rx_stall_timer);
  }
  mtlk_osal_timer_cleanup(&stadb->whm_tx_rx_stall_timer);
}

void __MTLK_IFUNC
mtlk_stadb_cleanup (sta_db *stadb)
{
  MTLK_CLEANUP_BEGIN(sta_db, MTLK_OBJ_PTR(stadb))
    MTLK_CLEANUP_STEP(sta_db, WHM_DISCNCT_TIMER, MTLK_OBJ_PTR(stadb),
                      mtlk_osal_timer_cleanup, (&stadb->whm_discnct_timer));
    MTLK_CLEANUP_STEP(sta_db, WHM_HASH, MTLK_OBJ_PTR(stadb),
                      _wave_stadb_cleanup_whm_sta_list, (stadb));
    MTLK_CLEANUP_STEP(sta_db, EN_ABILITIES, MTLK_OBJ_PTR(stadb),
                      mtlk_abmgr_disable_ability_set,
                      (mtlk_vap_get_abmgr(stadb->vap_handle), _stadb_abilities, ARRAY_SIZE(_stadb_abilities)));
    MTLK_CLEANUP_STEP(sta_db, REG_ABILITIES, MTLK_OBJ_PTR(stadb),
                      mtlk_abmgr_unregister_ability_set,
                      (mtlk_vap_get_abmgr(stadb->vap_handle), _stadb_abilities, ARRAY_SIZE(_stadb_abilities)));
    MTLK_CLEANUP_STEP(sta_db, STADB_LOCK, MTLK_OBJ_PTR(stadb),
                      mtlk_osal_lock_cleanup, (&stadb->lock));
    MTLK_CLEANUP_STEP(sta_db, SID_HASH, MTLK_OBJ_PTR(stadb),
                      mtlk_hash_cleanup_sid, (&stadb->sid_hash));
    MTLK_CLEANUP_STEP(sta_db, STADB_HASH, MTLK_OBJ_PTR(stadb),
                      mtlk_hash_cleanup_ieee_addr, (&stadb->hash));
    MTLK_CLEANUP_STEP(sta_db, WHM_TX_RX_STALL_TIMER, MTLK_OBJ_PTR(stadb),
                      mtlk_tx_rx_warn_timer_deinit, (stadb));
  MTLK_CLEANUP_END(sta_db, MTLK_OBJ_PTR(stadb))
}

void __MTLK_IFUNC
mtlk_stadb_reset_cnts (sta_db *stadb)
{
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  MTLK_ASSERT(stadb != NULL);

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&stadb->hash, &e);
  while (h) {
    sta_entry *sta = MTLK_CONTAINER_OF(h, sta_entry, hentry);
    _mtlk_sta_reset_cnts(sta);
    h = mtlk_hash_enum_next_ieee_addr(&stadb->hash, &e);
  }
  mtlk_osal_lock_release(&stadb->lock);
}

void __MTLK_IFUNC
mtlk_stadb_disconnect_all (sta_db *stadb,
    mtlk_stadb_disconnect_sta_clb_f clb,
    mtlk_handle_t usr_ctx,
    BOOL wait_all_packets_confirmed)
{
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  sta_entry                    *sta;
  mtlk_stadb_iterator_t         iter;
  mtlk_df_user_t               *df_user;
  struct wireless_dev          *wdev;

  MTLK_ASSERT(stadb != NULL);
  MTLK_ASSERT(clb != NULL);

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&stadb->hash, &e);
  while (h) {
    sta = MTLK_CONTAINER_OF(h, sta_entry, hentry);
    mtlk_sta_set_packets_filter(sta, MTLK_PCKT_FLTR_DISCARD_ALL);
    h = mtlk_hash_enum_next_ieee_addr(&stadb->hash, &e);
  }
  mtlk_osal_lock_release(&stadb->lock);

  MTLK_UNREFERENCED_PARAM(wait_all_packets_confirmed);

  df_user = mtlk_df_get_user(mtlk_vap_get_df(stadb->vap_handle));
  wdev = mtlk_df_user_get_wdev(df_user);

  sta = (sta_entry*)mtlk_stadb_iterate_first(stadb, &iter);
  if (sta) {
    do {
      clb(usr_ctx, sta);
      mtlk_stadb_remove_sta(stadb, sta);

      if (!mtlk_vap_is_sta(stadb->vap_handle)) {
        struct station_del_parameters station_del_parameters;
        IEEE_ADDR sta_addr = *mtlk_sta_get_addr(sta);

        station_del_parameters.mac = sta_addr.au8Addr;
        station_del_parameters.subtype = 12; /*Deauthentication*/
        station_del_parameters.reason_code = WLAN_REASON_UNSPECIFIED;

        mtlk_sta_decref(sta); /*sta must not be used after this step */
        ieee80211_del_station(wdev->wiphy, mtlk_df_user_get_ndev(df_user), &station_del_parameters);
      } else {
        mtlk_sta_decref(sta); /* decref iteration */
        ieee80211_connection_loss(wave_vap_get_vif(stadb->vap_handle));
      }

      sta = (sta_entry*)mtlk_stadb_iterate_next(&iter);
    } while (sta);
    mtlk_stadb_iterate_done_none_decref(&iter);
  }

  if (!mtlk_stadb_is_empty(stadb)) {
    ELOG_V("STA DB must be empty but actually not");
    MTLK_ASSERT(0);
  }
}

int __MTLK_IFUNC
mtlk_stadb_get_peer_list (sta_db *stadb, mtlk_clpb_t *clpb)
{
  int                   res = MTLK_ERR_OK;
  const sta_entry       *sta;
  mtlk_stadb_iterator_t iter;
  mtlk_wssa_peer_list_t peer_list;
  int                   sta_cnt =  mtlk_stadb_get_sta_cnt(stadb);

  res = mtlk_clpb_push(clpb, &sta_cnt, sizeof(int));
  if (MTLK_ERR_OK != res) {
    return res;
  }

  sta = mtlk_stadb_iterate_first(stadb, &iter);
  if (sta) {
    do {
      peer_list.addr = *(mtlk_sta_get_addr(sta));
      peer_list.is_sta_auth = _mtlk_sta_get_is_sta_auth(sta);

      res = mtlk_clpb_push(clpb, &peer_list, sizeof(mtlk_wssa_peer_list_t));
      if (MTLK_ERR_OK != res) {
        goto err_push_sta;
      }

      sta = mtlk_stadb_iterate_next(&iter);
    } while (sta);
    mtlk_stadb_iterate_done(&iter);
  }

  return res;

err_push_sta:
  mtlk_stadb_iterate_done(&iter);
  mtlk_clpb_purge(clpb);
  return res;
}

int __MTLK_IFUNC
mtlk_stadb_get_stat(sta_db *stadb, hst_db *hstdb, mtlk_clpb_t *clpb, uint8 group_cipher)
{
  int                   res = MTLK_ERR_OK;
  const sta_entry       *sta;
  mtlk_stadb_iterator_t iter;
  mtlk_stadb_stat_t     stadb_stat;
  mtlk_hstdb_iterator_t hiter;
  int bridge_mode;

  bridge_mode = wave_pdb_get_int(mtlk_vap_get_param_db(stadb->vap_handle), PARAM_DB_CORE_BRIDGE_MODE);

  sta = mtlk_stadb_iterate_first(stadb, &iter);
  if (sta) {
    do {

      /* Omit Open-mode STAs if security enabled */
      if ((!group_cipher) ||
          (group_cipher == IW_ENCODE_ALG_WEP) || (mtlk_sta_get_cipher(sta)) ||
          MTLK_BFIELD_GET(sta->info.flags, STA_FLAGS_WDS)) {

        /* Get general STA statistic */
        memset(&stadb_stat, 0, sizeof(stadb_stat));

        stadb_stat.type = STAT_ID_STADB;
        stadb_stat.u.general_stat.addr = *(mtlk_sta_get_addr(sta));
        stadb_stat.u.general_stat.sta_sid = mtlk_sta_get_sid(sta);
        stadb_stat.u.general_stat.aid = mtlk_sta_get_sid(sta) + 1;
        stadb_stat.u.general_stat.sta_rx_dropped = 0;
        /* stadb_stat.u.general_stat.network_mode = mtlk_sta_get_net_mode(sta); */
        stadb_stat.u.general_stat.tx_rate = _mtlk_sta_get_tx_data_rate(sta);
        stadb_stat.u.general_stat.is_sta_auth = _mtlk_sta_get_is_sta_auth(sta);
        stadb_stat.u.general_stat.is_four_addr = mtlk_sta_is_4addr(sta);
        stadb_stat.u.general_stat.nss = mtlk_bitrate_info_get_nss(sta->info.stats.tx_data_rate_params);

        mtlk_sta_get_peer_stats(sta, &stadb_stat.u.general_stat.peer_stats);

        /* push stadb statistic to clipboard */
        res = mtlk_clpb_push(clpb, &stadb_stat, sizeof(stadb_stat));
        if (MTLK_ERR_OK != res) {
          goto err_push_sta;
        }

        /* Get HostDB statistic related to current STA */
        if ((NULL != hstdb) && ((BR_MODE_WDS == bridge_mode) ||
          mtlk_stadb_get_four_addr_sta_cnt(stadb))) {

          const IEEE_ADDR *host_addr;
          IEEE_ADDR addr;

          ieee_addr_set(&addr, mtlk_sta_get_addr(sta)->au8Addr);
          host_addr = mtlk_hstdb_iterate_first(hstdb, &addr, &hiter);

          if (host_addr) {
            do {
              memset(&stadb_stat, 0, sizeof(stadb_stat));
              stadb_stat.type = STAT_ID_HSTDB;
              stadb_stat.u.hstdb_stat.addr = *host_addr;

              /* push to clipboard*/
              res = mtlk_clpb_push(clpb, &stadb_stat, sizeof(stadb_stat));
              if (MTLK_ERR_OK != res) {
                goto err_push_host;
              }

              host_addr = mtlk_hstdb_iterate_next(&hiter);
            } while (host_addr);
            mtlk_hstdb_iterate_done(&hiter);
          }
        }
      }

      sta = mtlk_stadb_iterate_next(&iter);
    } while (sta);
    mtlk_stadb_iterate_done(&iter);
  }

  return res;

err_push_host:
  mtlk_hstdb_iterate_done(&hiter);
err_push_sta:
  mtlk_stadb_iterate_done(&iter);
  mtlk_clpb_purge(clpb);
  return res;
}

sta_entry * __MTLK_IFUNC
mtlk_vap_find_sta (mtlk_vap_handle_t vap_handle, const unsigned char *mac)
{
  struct ieee80211_sta * mac80211_sta;
  sta_entry * sta = NULL;

  rcu_read_lock();
  mac80211_sta = wave_vap_get_sta(vap_handle, (u8 *) mac);
  if (mac80211_sta) {
    sta = wv_ieee80211_get_sta(mac80211_sta);
    if (!(sta->is_added_to_driver)) { /*driver sta was not yet added*/
      rcu_read_unlock();
      mtlk_sta_decref(sta);
      return NULL;
    }
  }
  rcu_read_unlock();

  return sta;
}

struct ieee80211_sta * __MTLK_IFUNC
wv_sta_entry_get_mac80211_sta (const sta_entry * sta)
{
  return container_of((void *)sta, struct ieee80211_sta, drv_priv);
}

uint32 __MTLK_IFUNC
mtlk_stadb_stas_num(sta_db *stadb)
{
  return mtlk_stadb_get_sta_cnt(stadb);
}

sta_entry * __MTLK_IFUNC
mtlk_stadb_get_last_sta (sta_db *stadb)
{
  mtlk_core_t *core = mtlk_vap_get_core(stadb->vap_handle);
  return mtlk_stadb_find_sta(stadb, core->slow_ctx->last_sta_addr.au8Addr);
}
#ifdef UNUSED_CODE
sta_entry * __MTLK_IFUNC
mtlk_stadb_get_first_sta (sta_db *stadb)
{
  sta_entry *sta = NULL;
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  MTLK_ASSERT(stadb != NULL);

  mtlk_osal_lock_acquire(&stadb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&stadb->hash, &e);
  if (h) {
    sta = MTLK_CONTAINER_OF(h, sta_entry, hentry);
    mtlk_sta_incref(sta); /* Reference by caller */
  }
  mtlk_osal_lock_release(&stadb->lock);

  return sta;
}
#endif /* UNUSED_CODE */
/******************************************************************************************
 * HST API
 ******************************************************************************************/

static __INLINE BOOL
_mtlk_is_hst_behind_sta (const host_entry *host, const IEEE_ADDR *sta_addr)
{
  return !(ieee_addr_compare(&(host->sta_addr), sta_addr));
}

MTLK_INIT_STEPS_LIST_BEGIN(host_entry)
MTLK_INIT_INNER_STEPS_BEGIN(host_entry)
MTLK_INIT_STEPS_LIST_END(host_entry);
MTLK_START_STEPS_LIST_BEGIN(host_entry)
  MTLK_START_STEPS_LIST_ENTRY(host_entry, STA_LINK)
MTLK_START_INNER_STEPS_BEGIN(host_entry)
MTLK_START_STEPS_LIST_END(host_entry);

static void
_mtlk_hst_cleanup (host_entry *host)
{
  MTLK_CLEANUP_BEGIN(host_entry, MTLK_OBJ_PTR(host))
  MTLK_CLEANUP_END(host_entry, MTLK_OBJ_PTR(host))
}

static int
_mtlk_hst_init (host_entry *host, hst_db *paramdb)
{
  memset(host, 0, sizeof(host_entry));

  host->paramdb = paramdb;

  MTLK_INIT_TRY(host_entry, MTLK_OBJ_PTR(host))
  MTLK_INIT_FINALLY(host_entry, MTLK_OBJ_PTR(host))
  MTLK_INIT_RETURN(host_entry, MTLK_OBJ_PTR(host), _mtlk_hst_cleanup, (host))
}

static __INLINE void
_mtlk_hst_stop (host_entry *host)
{
  MTLK_ASSERT(!is_zero_ether_addr(host->sta_addr.au8Addr));

  MTLK_STOP_BEGIN(host_entry, MTLK_OBJ_PTR(host))
    MTLK_STOP_STEP(host_entry, STA_LINK, MTLK_OBJ_PTR(host),
                   ieee_addr_zero, (&host->sta_addr));
  MTLK_STOP_END(host_entry, MTLK_OBJ_PTR(host))
}

static __INLINE int
_mtlk_hst_start (host_entry *host, IEEE_ADDR *sta_addr)
{
  host->timestamp = mtlk_osal_timestamp();

  MTLK_START_TRY(host_entry, MTLK_OBJ_PTR(host))
    MTLK_START_STEP_VOID(host_entry, STA_LINK, MTLK_OBJ_PTR(host),
                         ieee_addr_copy, (&host->sta_addr, sta_addr));
  MTLK_START_FINALLY(host_entry, MTLK_OBJ_PTR(host))
  MTLK_START_RETURN(host_entry, MTLK_OBJ_PTR(host), _mtlk_hst_stop, (host))
}

static __INLINE void
_mtlk_hst_update (host_entry *host, IEEE_ADDR *sta_addr)
{
  if(host == NULL || sta_addr == NULL) {
    ILOG2_V("Bad params at _mtlk_hst_update");
    return;
  }
  ILOG3_YD("Update HOST %Y - %lu", _mtlk_hst_get_addr(host), host->timestamp);

  if (!_mtlk_is_hst_behind_sta(host, sta_addr)){

    if (!is_zero_ether_addr(host->sta_addr.au8Addr)){
      /* Host was associated previously with another STA */
      ILOG2_YY("HOST %Y is stopped with STA %Y on UPDATE", _mtlk_hst_get_addr(host)->au8Addr, host->sta_addr.au8Addr);
      _mtlk_hst_stop(host);
    }

    ILOG2_YY("HOST %Y is started with STA %Y on UPDATE", _mtlk_hst_get_addr(host)->au8Addr, sta_addr->au8Addr);
    _mtlk_hst_start(host, sta_addr);
  }
  else{
    host->timestamp = mtlk_osal_timestamp();
  }

}
/******************************************************************************************/

/******************************************************************************************
 * HST DB API
 ******************************************************************************************/
#define MTLK_HSTDB_HASH_NOF_BUCKETS 16

static void
_mtlk_hstdb_recalc_default_host_unsafe (hst_db *hstdb)
{
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  host_entry                   *newest_hst = NULL;
  uint32                        smallest_diff = (uint32)-1;

  h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);
  while (h)
  {
    host_entry *hst  = MTLK_CONTAINER_OF(h, host_entry, hentry);
    uint32      diff = mtlk_osal_timestamp_diff(mtlk_osal_timestamp(), hst->timestamp);

    if (!mtlk_osal_is_zero_address(hst->sta_addr.au8Addr)){
      if (diff < smallest_diff) {
        smallest_diff = diff;
        newest_hst    = hst;
      }
    }

    h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);
  }

  if (newest_hst == NULL) {
    ieee_addr_zero(&hstdb->default_host);
  }
  else {
    hstdb->default_host = *_mtlk_hst_get_addr(newest_hst);
  }

  ILOG3_Y("default_host %Y", hstdb->default_host.au8Addr);
}

static void
_mtlk_hstdb_add_host (hst_db* hstdb, const unsigned char *mac,
                      IEEE_ADDR * sta_addr)
{
  int         res = MTLK_ERR_UNKNOWN;
  host_entry *hst = NULL;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  hst = mtlk_osal_mem_alloc(sizeof(*hst), MTLK_MEM_TAG_STADB_ITER);
  if (!hst) {
    ELOG_V("Can't allocate host!");
    res = MTLK_ERR_NO_MEM;
    goto err_alloc;
  }

  res = _mtlk_hst_init(hst, hstdb);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't start Host (err#%d)", res);
    goto err_init;
  }

  ILOG2_YY("HOST %Y started with STA %Y on CREATE", ((IEEE_ADDR *)mac)->au8Addr, sta_addr->au8Addr);
  res = _mtlk_hst_start(hst, sta_addr);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't start Host (err#%d)", res);
    goto err_start;
  }

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_insert_ieee_addr(&hstdb->hash, (IEEE_ADDR *)mac, &hst->hentry);
  if (h == NULL) {
    ++hstdb->hash_cnt;
  }
  mtlk_osal_lock_release(&hstdb->lock);

  if (h != NULL) {
    res = MTLK_ERR_PROHIB;
    WLOG_V("Already registered");
    goto err_insert;
  }

  mtlk_hstdb_update_default_host(hstdb, mac);

  ILOG2_YPY("HOST %Y added (%p), belongs to STA %Y", mac, hst, hst->sta_addr.au8Addr);

  return;

err_insert:
  _mtlk_hst_stop(hst);
err_start:
  _mtlk_hst_cleanup(hst);
err_init:
  mtlk_osal_mem_free(hst);
err_alloc:
  return;
}

void __MTLK_IFUNC
mtlk_hstdb_update_default_host (hst_db* hstdb, const unsigned char *mac)
{
  if (!ieee_addr_is_valid(&hstdb->default_host) ||
       0 == ieee_addr_compare(&hstdb->default_host, &hstdb->local_mac)) {
    ieee_addr_set(&hstdb->default_host, mac);
    ILOG3_Y("default_host %Y", hstdb->default_host.au8Addr);
  }
}

void __MTLK_IFUNC
mtlk_hstdb_update_host (hst_db* hstdb, const unsigned char *mac, IEEE_ADDR * sta_addr)
{
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_find_ieee_addr(&hstdb->hash, (IEEE_ADDR *)mac);

  if (h) {
    host_entry *host = MTLK_CONTAINER_OF(h, host_entry, hentry);
    /* Station already associated with HOST - just update timestamp */
    _mtlk_hst_update(host, sta_addr);
    mtlk_osal_lock_release(&hstdb->lock);
  } else {
    mtlk_osal_lock_release(&hstdb->lock);

    ILOG2_V("Can't find peer (HOST), adding...");
    _mtlk_hstdb_add_host(hstdb, mac, sta_addr);
  }
}

static void
_mtlk_hstdb_remove_host_unsafe (hst_db *hstdb, host_entry *host, const IEEE_ADDR *mac)
{
  MTLK_ASSERT(NULL != hstdb);
  MTLK_ASSERT(NULL != host);
  MTLK_ASSERT(NULL != mac);

  if (!is_zero_ether_addr(host->sta_addr.au8Addr)){
    ILOG2_YY("HOST %Y stopped on STA %Y on REMOVE", _mtlk_hst_get_addr(host)->au8Addr, host->sta_addr.au8Addr);
    _mtlk_hst_stop(host);
  }

  _mtlk_hst_cleanup(host);
  mtlk_osal_mem_free(host);

  ILOG2_Y("HOST %Y removed", mac->au8Addr);
}

/* Function should be used with HSTDB lock acquired by caller */
static void
_mtlk_hstdb_hash_remove_ieee_addr_internal(hst_db *hstdb, host_entry *host)
{
  MTLK_ASSERT(NULL != hstdb);
  MTLK_ASSERT(NULL != host);

  --hstdb->hash_cnt;
  mtlk_hash_remove_ieee_addr(&hstdb->hash, &host->hentry);
}

int __MTLK_IFUNC
mtlk_hstdb_remove_all_by_sta (hst_db *hstdb, const IEEE_ADDR *sta_addr)
{
  /* For L2NAT only! not applicable for WDS */

  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  BOOL                          recalc_def_host = FALSE;

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);
  while (h)
  {
    host_entry *hst  = MTLK_CONTAINER_OF(h, host_entry, hentry);
    if (_mtlk_is_hst_behind_sta(hst, sta_addr)) {
      /* WARNING: We store the MAC address here since it may
      *           become corrupted after mtlk_hash_remove_hstdb()
      */
      const IEEE_ADDR mac = *_mtlk_hst_get_addr(hst);

      /* Call "internal" functions to remove host, they don't use locks */
      _mtlk_hstdb_hash_remove_ieee_addr_internal(hstdb, hst);
      _mtlk_hstdb_remove_host_unsafe(hstdb, hst, &mac);

      /* are we deleting default host? */
      if (FALSE == recalc_def_host && 0 == ieee_addr_compare(&hstdb->default_host, &mac)) {
        recalc_def_host = TRUE;
      }

    }
    h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);
  }

  if (recalc_def_host) {
    _mtlk_hstdb_recalc_default_host_unsafe(hstdb);
  }
  mtlk_osal_lock_release(&hstdb->lock);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_hstdb_remove_host_by_addr (hst_db *hstdb, const IEEE_ADDR *mac)
{
  int res;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_find_ieee_addr(&hstdb->hash, mac);
  if (h) {
    host_entry *host = MTLK_CONTAINER_OF(h, host_entry, hentry);
    _mtlk_hstdb_hash_remove_ieee_addr_internal(hstdb, host);
    mtlk_osal_lock_release(&hstdb->lock);

    /* The host isn't in the DB anymore, so nobody can access it =>
     * we can remove it with no locking
     */
    _mtlk_hstdb_remove_host_unsafe(hstdb, host, mac);
    res = MTLK_ERR_OK;
  }
  else {
    mtlk_osal_lock_release(&hstdb->lock);
    res = MTLK_ERR_NOT_IN_USE;
  }

  return res;
}

void __MTLK_IFUNC
wv_hostdb_cleanup(hst_db *hstdb)
{
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  mtlk_hash_enum_t              e;
  uint32                        diff , timeout = hstdb->wds_host_timeout * MTLK_OSAL_MSEC_IN_SEC;
  BOOL                          recalc_def_host = FALSE;

  if (mtlk_osal_time_passed_ms(hstdb->last_cleanup) < timeout)
    return;

  hstdb->last_cleanup = mtlk_osal_timestamp();

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);

  while (h) {
    host_entry *host = MTLK_CONTAINER_OF(h, host_entry, hentry);
    diff = mtlk_osal_time_passed_ms(host->timestamp);
    if (diff > timeout) {
      _mtlk_hstdb_hash_remove_ieee_addr_internal(hstdb, host);
      _mtlk_hstdb_remove_host_unsafe(hstdb, host, _mtlk_hst_get_addr(host));
    }

    /* are we deleting default host? */
    if ((FALSE == recalc_def_host) && (0 == ieee_addr_compare(&hstdb->default_host, _mtlk_hst_get_addr(host)))) {
      recalc_def_host = TRUE;
    }
    h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);
  }

  if (recalc_def_host) {
    _mtlk_hstdb_recalc_default_host_unsafe(hstdb);
  }

  mtlk_osal_lock_release(&hstdb->lock);
}

static int
_mtlk_hstdb_cleanup_all_hst_unsafe (hst_db *hstdb)
{
  /* Remove all existing entries from DB. Called on slow context cleanup only.
     All entries must be already stopped at this point */

  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);
  while (h)
  {
    host_entry *hst  = MTLK_CONTAINER_OF(h, host_entry, hentry);
    ILOG2_Y("WDS HOST Clean up on DESTROY %Y ", _mtlk_hst_get_addr(hst)->au8Addr);
    _mtlk_hstdb_hash_remove_ieee_addr_internal(hstdb, hst);
    _mtlk_hst_cleanup(hst);
    mtlk_osal_mem_free(hst);

    h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);
  }

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_hstdb_stop_all_by_sta (hst_db *hstdb, const IEEE_ADDR *sta_addr)
{
  /* For WDS only! not applicable for L2NAT */

  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);
  while (h)
  {
    host_entry *hst  = MTLK_CONTAINER_OF(h, host_entry, hentry);
    if (_mtlk_is_hst_behind_sta(hst, sta_addr)){
      ILOG2_YY("HOST %Y on STA %Y is stopped", _mtlk_hst_get_addr(hst)->au8Addr, sta_addr->au8Addr);
      _mtlk_hst_stop(hst);
    }

    h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);

  }

  mtlk_osal_lock_release(&hstdb->lock);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_hstdb_start_all_by_sta (hst_db *hstdb, IEEE_ADDR *sta_addr)
{
  /* For WDS only! not applicable for L2NAT.
     Associate existing HOST entries with just connected STA
     if previous STA address is the same */

  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);
  while (h)
  {
    host_entry *hst  = MTLK_CONTAINER_OF(h, host_entry, hentry);

    if (0 == ieee_addr_compare(&hst->sta_addr, sta_addr)){
      ILOG2_YY("HOST Entry reassociated %Y to STA %Y on CONNECT", _mtlk_hst_get_addr(hst)->au8Addr, hst->sta_addr.au8Addr);
      _mtlk_hst_start(hst, sta_addr);
    }

    h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);

  }
  mtlk_osal_lock_release(&hstdb->lock);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_hstdb_dcdp_remove_all_by_sta(hst_db *hstdb, const sta_entry *sta)
{
#if MTLK_USE_DIRECTCONNECT_DP_API
  mtlk_hstdb_iterator_t  hiter;
  const IEEE_ADDR       *host_addr = NULL;
  const IEEE_ADDR       *sta_addr = mtlk_sta_get_addr(sta);

  ILOG3_Y("Remove all HOST addresses from switch table on STA %Y", sta_addr->au8Addr);

  memset(&hiter, 0, sizeof(hiter));
  host_addr = mtlk_hstdb_iterate_first(hstdb, sta_addr, &hiter);
  if (host_addr) {
    do {
      /* Remove host MAC addr from switch MAC table */
      mtlk_df_user_dcdp_remove_mac_addr(mtlk_vap_get_secondary_df(hstdb->vap_handle,
                                                                  sta->secondary_vap_id),
                                        host_addr->au8Addr);
      host_addr = mtlk_hstdb_iterate_next(&hiter);
    } while (host_addr);
    mtlk_hstdb_iterate_done(&hiter);
  }
#endif
  return MTLK_ERR_OK;
}
#ifdef UNUSED_CODE
mtlk_vap_handle_t __MTLK_IFUNC
mtlk_host_get_vap_handle (mtlk_handle_t host_handle)
{
  host_entry  *host;

  MTLK_ASSERT(host_handle);

  host = HANDLE_T_PTR(host_entry, host_handle);
  return (host->paramdb->vap_handle);
}
#endif /* UNUSED_CODE */

MTLK_INIT_STEPS_LIST_BEGIN(hst_db)
  MTLK_INIT_STEPS_LIST_ENTRY(hst_db, HSTDB_HASH)
  MTLK_INIT_STEPS_LIST_ENTRY(hst_db, HSTDB_LOCK)
#ifdef CONFIG_WAVE_DEBUG
  MTLK_INIT_STEPS_LIST_ENTRY(hst_db, REG_ABILITIES)
  MTLK_INIT_STEPS_LIST_ENTRY(hst_db, EN_ABILITIES)
#endif
MTLK_INIT_INNER_STEPS_BEGIN(hst_db)
MTLK_INIT_STEPS_LIST_END(hst_db);

#ifdef CONFIG_WAVE_DEBUG
static const mtlk_ability_id_t _hstdb_abilities[] = {
  WAVE_CORE_REQ_GET_HSTDB_CFG,
  WAVE_CORE_REQ_SET_HSTDB_CFG
};
#endif

int __MTLK_IFUNC
mtlk_hstdb_init (hst_db *hstdb, mtlk_vap_handle_t vap_handle)
{
  hstdb->wds_host_timeout = DEFAULT_HOSTDB_TIMEOUT;

  hstdb->vap_handle = vap_handle;

  hstdb->last_cleanup = mtlk_osal_timestamp();

  MTLK_INIT_TRY(hst_db, MTLK_OBJ_PTR(hstdb))
    MTLK_INIT_STEP(hst_db, HSTDB_HASH, MTLK_OBJ_PTR(hstdb),
                   mtlk_hash_init_ieee_addr, (&hstdb->hash, MTLK_HSTDB_HASH_NOF_BUCKETS));
    MTLK_INIT_STEP(hst_db, HSTDB_LOCK, MTLK_OBJ_PTR(hstdb),
                   mtlk_osal_lock_init, (&hstdb->lock));
#ifdef CONFIG_WAVE_DEBUG
    MTLK_INIT_STEP(hst_db, REG_ABILITIES, MTLK_OBJ_PTR(hstdb),
                   mtlk_abmgr_register_ability_set,
                   (mtlk_vap_get_abmgr(hstdb->vap_handle), _hstdb_abilities, ARRAY_SIZE(_hstdb_abilities)));
    MTLK_INIT_STEP_VOID(hst_db, EN_ABILITIES, MTLK_OBJ_PTR(hstdb),
                        mtlk_abmgr_enable_ability_set,
                        (mtlk_vap_get_abmgr(hstdb->vap_handle), _hstdb_abilities, ARRAY_SIZE(_hstdb_abilities)));
#endif
  MTLK_INIT_FINALLY(hst_db, MTLK_OBJ_PTR(hstdb))
  MTLK_INIT_RETURN(hst_db, MTLK_OBJ_PTR(hstdb), mtlk_hstdb_cleanup, (hstdb))
}

void __MTLK_IFUNC
mtlk_hstdb_cleanup (hst_db *hstdb)
{

  _mtlk_hstdb_cleanup_all_hst_unsafe(hstdb);

  MTLK_CLEANUP_BEGIN(hst_db, MTLK_OBJ_PTR(hstdb))
#ifdef CONFIG_WAVE_DEBUG
    MTLK_CLEANUP_STEP(hst_db, EN_ABILITIES, MTLK_OBJ_PTR(hstdb),
                       mtlk_abmgr_disable_ability_set,
                       (mtlk_vap_get_abmgr(hstdb->vap_handle), _hstdb_abilities, ARRAY_SIZE(_hstdb_abilities)));
    MTLK_CLEANUP_STEP(hst_db, REG_ABILITIES, MTLK_OBJ_PTR(hstdb),
                       mtlk_abmgr_unregister_ability_set,
                       (mtlk_vap_get_abmgr(hstdb->vap_handle), _hstdb_abilities, ARRAY_SIZE(_hstdb_abilities)));
#endif
    MTLK_CLEANUP_STEP(hst_db, HSTDB_LOCK, MTLK_OBJ_PTR(hstdb),
                      mtlk_osal_lock_cleanup, (&hstdb->lock));
    MTLK_CLEANUP_STEP(hst_db, HSTDB_HASH, MTLK_OBJ_PTR(hstdb),
                      mtlk_hash_cleanup_ieee_addr, (&hstdb->hash));
  MTLK_CLEANUP_END(hst_db, MTLK_OBJ_PTR(hstdb))
}

sta_entry * __MTLK_IFUNC
mtlk_hstdb_find_sta (hst_db* hstdb, const unsigned char *mac)
{
  sta_entry                    *sta = NULL;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&hstdb->lock);
  h = mtlk_hash_find_ieee_addr(&hstdb->hash, (IEEE_ADDR *)mac);
  if (h) {
    host_entry *host = MTLK_CONTAINER_OF(h, host_entry, hentry);
    sta = mtlk_stadb_find_sta(mtlk_core_get_stadb(mtlk_vap_get_core(hstdb->vap_handle)),
                              host->sta_addr.au8Addr);
    if (sta){
      ILOG3_YPP("Found host %Y, STA %p (%p)",
        _mtlk_hst_get_addr(host), sta, host);
    }
  }
  mtlk_osal_lock_release(&hstdb->lock);

  return sta;
}

const IEEE_ADDR * __MTLK_IFUNC
mtlk_hstdb_iterate_first (hst_db *hstdb, const IEEE_ADDR *sta_addr, mtlk_hstdb_iterator_t *iter)
{
  int                           err  = MTLK_ERR_OK;
  const IEEE_ADDR              *addr = NULL;
  uint32                        idx  = 0;
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  MTLK_ASSERT(hstdb != NULL);
  MTLK_ASSERT(iter != NULL);

  mtlk_osal_lock_acquire(&hstdb->lock);

  iter->size = hstdb->hash_cnt;

  if (!iter->size) {
    err = MTLK_ERR_NOT_IN_USE;
    goto end;
  }

  iter->addr = (IEEE_ADDR *)mtlk_osal_mem_alloc(iter->size * sizeof(IEEE_ADDR), MTLK_MEM_TAG_STADB_ITER);
  if (!iter->addr) {
    ELOG_D("Can't allocate iteration array of %d entries", iter->size);
    err = MTLK_ERR_NO_MEM;
    goto end;
  }

  memset(iter->addr, 0, iter->size * sizeof(IEEE_ADDR));

  h = mtlk_hash_enum_first_ieee_addr(&hstdb->hash, &e);
  while (idx < iter->size && h) {
    const host_entry *host = MTLK_CONTAINER_OF(h, host_entry, hentry);
    if (_mtlk_is_hst_behind_sta(host, sta_addr)) {
      iter->addr[idx] = *_mtlk_hst_get_addr(host);
      ++idx;
    }
    h = mtlk_hash_enum_next_ieee_addr(&hstdb->hash, &e);
  }

  err = MTLK_ERR_OK;

end:
  mtlk_osal_lock_release(&hstdb->lock);

  if (err == MTLK_ERR_OK) {
    iter->idx   = 0;

    addr = mtlk_hstdb_iterate_next(iter);
    if (!addr) {
      mtlk_osal_mem_free(iter->addr);
    }
  }

  return addr;
}

const IEEE_ADDR * __MTLK_IFUNC
mtlk_hstdb_iterate_next (mtlk_hstdb_iterator_t *iter)
{
  const IEEE_ADDR *addr = NULL;

  MTLK_ASSERT(iter != NULL);
  MTLK_ASSERT(iter->addr != NULL);

  if (iter->idx < iter->size &&
      !ieee_addr_is_zero(&iter->addr[iter->idx])) {
    addr = &iter->addr[iter->idx];
    ++iter->idx;
  }

  return addr;
}

void __MTLK_IFUNC
mtlk_hstdb_iterate_done (mtlk_hstdb_iterator_t *iter)
{
  MTLK_ASSERT(iter != NULL);
  MTLK_ASSERT(iter->addr != NULL);

  mtlk_osal_mem_free(iter->addr);
  memset(iter, 0, sizeof(*iter));
}
/******************************************************************************************/

void __MTLK_IFUNC
mtlk_sta_get_packet_error_rate_stats(sta_entry *sta, perRatePERStats *stats_per)
{
  uint32 cbw;

  cbw = mtlk_bitrate_params_get_cbw(sta->info.stats.tx_data_rate_params);
  stats_per->bw = _mtlk_sta_rate_cbw_to_cbw_in_mhz(cbw);
  stats_per->mcs = mtlk_bitrate_params_get_mcs(sta->info.stats.tx_data_rate_params);
  stats_per->nss = mtlk_bitrate_params_get_nss(sta->info.stats.tx_data_rate_params);
  stats_per->gi = mtlk_bitrate_params_get_scp(sta->info.stats.tx_data_rate_params);
  stats_per->packet_error_rate =  sta->sta_stats_cntrs.per;

  return;
}

#ifdef CONFIG_WAVE_DEBUG
void __MTLK_IFUNC
mtlk_sta_get_host_if_qos_stats (const sta_entry* sta, peerHostIfQos_t* peer_host_if_qos)
{
  uint8 tid = 0;
  for(tid=0 ; tid < NUM_OF_TIDS; tid++) {
    peer_host_if_qos->qosTxSta[tid] = sta->sta_stats_cntrs.qosTxSta[tid];
    peer_host_if_qos->qosByteCountSta[tid] = sta->sta_stats_cntrs.downlinkMuStats.dlBufferStatus[tid].queueSize;
  }
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_host_if_stats (const sta_entry* sta, peerHostIf_t* peer_host_if)
{
  peer_host_if->rxOutStaNumOfBytes = sta->sta_stats64_cntrs.rxOutStaNumOfBytes;
  peer_host_if->agerPdNoTransmitCountSta = sta->sta_stats64_cntrs.agerPdNoTransmitCountSta;
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_peer_rx_stats (const sta_entry* sta, peerRxStats_t *peer_rx_stats)
{
  peer_rx_stats->rddelayed = sta->sta_stats64_cntrs.rddelayed;
  peer_rx_stats->swUpdateDrop = sta->sta_stats64_cntrs.swUpdateDrop;
  peer_rx_stats->rdDuplicateDrop = sta->sta_stats64_cntrs.rdDuplicateDrop;
  peer_rx_stats->missingSn = sta->sta_stats64_cntrs.missingSn;
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_peer_ul_bsrc_tid_stats (const sta_entry* sta, peerUlBsrcTidStats_t *peer_ul_bsrc_tid)
{
  uint count = 0;
  peer_ul_bsrc_tid->bufStsCnt0 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  peer_ul_bsrc_tid->bufStsCnt1 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  peer_ul_bsrc_tid->bufStsCnt2 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  peer_ul_bsrc_tid->bufStsCnt3 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  peer_ul_bsrc_tid->bufStsCnt4 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  peer_ul_bsrc_tid->bufStsCnt5 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  peer_ul_bsrc_tid->bufStsCnt6 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  peer_ul_bsrc_tid->bufStsCnt7 = sta->sta_stats_cntrs.uplinkMuStats.ulBufferStatus[count++].queueSize;
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_peer_baa_stats (const sta_entry* sta, peerBaaStats_t *peer_baa_stats)
{
  peer_baa_stats->mpduFirstRetransmission = sta->sta_stats64_cntrs.mpduFirstRetransmission;
  peer_baa_stats->mpduTransmitted = sta->sta_stats64_cntrs.mpduTransmitted;
  peer_baa_stats->mpduByteTransmitted = sta->sta_stats64_cntrs.mpduByteTransmitted;
  peer_baa_stats->mpduRetransmission = sta->sta_stats64_cntrs.txRetryCount;
  peer_baa_stats->channelTransmitTime = sta->sta_stats64_cntrs.channelTransmitTime;
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_link_adaption_stats(const sta_entry* sta, linkAdaptionStats_t *link_stats)
{
  uint8 rate = 0;

  link_stats->DataPhyMode = sta->info.stats.mhi_tx_stats.DataPhyMode;
  link_stats->ManagementPhyMode = sta->info.stats.mhi_tx_stats.ManagementPhyMode;
  link_stats->powerData = sta->info.stats.mhi_tx_stats.powerData;
  link_stats->powerManagement = sta->info.stats.mhi_tx_stats.powerManagement;
  link_stats->antennaSelectionData = sta->info.stats.mhi_tx_stats.antennaSelectionData;
  link_stats->antennaSelectionManagement = sta->info.stats.mhi_tx_stats.antennaSelectionManagement;
  link_stats->scpData = sta->info.stats.mhi_tx_stats.scpData;
  link_stats->stationCapabilities = sta->info.stats.mhi_tx_stats.stationCapabilities;
  link_stats->dataBwLimit = sta->info.stats.mhi_tx_stats.dataBwLimit;
  link_stats->bfModeData = sta->info.stats.mhi_tx_stats.bfModeData;
  link_stats->stbcModeData = sta->info.stats.mhi_tx_stats.stbcModeData;
  link_stats->raState = sta->info.stats.mhi_tx_stats.raState;
  link_stats->raStability = sta->info.stats.mhi_tx_stats.raStability;
  link_stats->nssData = sta->info.stats.mhi_tx_stats.nssData;
  link_stats->nssManagement = sta->info.stats.mhi_tx_stats.nssManagement;
  link_stats->mcsData = sta->info.stats.mhi_tx_stats.mcsData;
  link_stats->mcsManagement = sta->info.stats.mhi_tx_stats.mcsManagement;
  link_stats->bw = sta->info.stats.mhi_tx_stats.bw;
  link_stats->transmittedAmpdu = sta->info.stats.mhi_tx_stats.transmittedAmpdu;
  link_stats->totalTxTime = sta->info.stats.mhi_tx_stats.totalTxTime;
  link_stats->mpduInAmpdu = sta->info.stats.mhi_tx_stats.mpduInAmpdu;
  link_stats->ewmaTimeNsec = sta->info.stats.mhi_tx_stats.ewmaTimeNsec;
  link_stats->sumTimeNsec = sta->info.stats.mhi_tx_stats.sumTimeNsec;
  link_stats->numMsdus = sta->info.stats.mhi_tx_stats.numMsdus;
  link_stats->per = sta->info.stats.mhi_tx_stats.per;
  for(rate = 0; rate < RATES_BIT_MASK_SIZE; rate++) {
    link_stats->ratesMask[rate] = sta->info.stats.mhi_tx_stats.ratesMask[rate];
  }
}

void __MTLK_IFUNC
mtlk_sta_get_plan_manager_stats (const sta_entry* sta, planManagerStats_t *plan_manager_stats)
{
  plan_manager_stats->uplinkMuType = sta->sta_stats_cntrs.uplinkMuStats.uplinkMuType;
  plan_manager_stats->ulRuSubChannels = sta->sta_stats_cntrs.ulRuSubChannels;
  plan_manager_stats->ulRuType = sta->sta_stats_cntrs.ulRuType;
  plan_manager_stats->downlinkMuType = sta->sta_stats_cntrs.downlinkMuStats.downlinkMuType;
  plan_manager_stats->dlRuSubChannels = sta->sta_stats_cntrs.dlRuSubChannels;
  plan_manager_stats->dlRuType = sta->sta_stats_cntrs.dlRuType;
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_twt_stats (const sta_entry* sta, twtStats_t *twt_stats)
{
  uint agreement = 0;
  twt_stats->numOfAgreementsForSta = sta->sta_stats_cntrs.twtStaParams.numOfAgreementsForSta;
  twt_stats->state = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].state;
  twt_stats->agreementType = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].agreementType;
  twt_stats->implicit = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].operation.implicit;
  twt_stats->announced = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].operation.announced;
  twt_stats->triggerEnabled = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].operation.triggerEnabled;
  twt_stats->wakeTime = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.wakeTime;
  twt_stats->wakeInterval = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.wakeInterval;
  twt_stats->minWakeDuration = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.minWakeDuration;
  twt_stats->channel = sta->sta_stats_cntrs.twtStaParams.twtIndividualAgreement[agreement].params.individual.channel;
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_per_client_stats (const sta_entry* sta, perClientStats_t *per_client_stats)
{
  per_client_stats->retryCount = sta->sta_stats64_cntrs.retryCount;
  per_client_stats->successCount = sta->sta_stats64_cntrs.successCount;
  per_client_stats->exhaustedCount = sta->sta_stats64_cntrs.exhaustedCount;
  per_client_stats->clonedCount = sta->sta_stats64_cntrs.clonedCount;
  per_client_stats->oneOrMoreRetryCount = sta->sta_stats64_cntrs.oneOrMoreRetryCount;
  per_client_stats->packetRetransCount = sta->sta_stats64_cntrs.packetRetransCount;
  per_client_stats->dropCntReasonClassifier = sta->sta_stats64_cntrs.dropCntReasonClassifier;
  per_client_stats->dropCntReasonDisconnect = sta->sta_stats64_cntrs.dropCntReasonDisconnect;
  per_client_stats->dropCntReasonATF = sta->sta_stats64_cntrs.dropCntReasonATF;
  per_client_stats->dropCntReasonTSFlush = sta->sta_stats64_cntrs.dropCntReasonTSFlush;
  per_client_stats->dropCntReasonReKey = sta->sta_stats64_cntrs.dropCntReasonReKey;
  per_client_stats->dropCntReasonSetKey = sta->sta_stats64_cntrs.dropCntReasonSetKey;
  per_client_stats->dropCntReasonDiscard = sta->sta_stats64_cntrs.dropCntReasonDiscard;
  per_client_stats->dropCntReasonDsabled = sta->sta_stats64_cntrs.dropCntReasonDsabled;
  per_client_stats->dropCntReasonAggError = sta->sta_stats64_cntrs.dropCntReasonAggError;
  per_client_stats->mpduRetryCount = sta->sta_stats64_cntrs.rxRetryCount;
  per_client_stats->mpduInAmpdu = sta->sta_stats64_cntrs.perclientmpduInAmpdu;
  per_client_stats->ampdu = sta->sta_stats64_cntrs.ampdu;
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_phy_rx_status (const sta_entry* sta, peerPhyRxStatus_t *phy_rx_status)
{
  uint8 antenna;
  phy_rx_status->rssi = sta->info.stats.max_rssi; /* Max Rssi */
  phy_rx_status->phyRate = sta->info.stats.rx_data_rate_info;
  phy_rx_status->irad = sta->info.stats.irad;
  phy_rx_status->lastTsf = sta->info.stats.lastTsf;
  phy_rx_status->perClientRxtimeUsage = sta->info.stats.perClientRxtimeUsage;
  phy_rx_status->psduRate = sta->info.stats.rx_psdu_data_rate_info;
  phy_rx_status->phyRateSynchedToPsduRate = sta->info.stats.last_phy_rate_synched_to_psdu_rate;
  for(antenna=0 ; antenna < PHY_STATISTICS_MAX_RX_ANT; antenna++) {
    phy_rx_status->noise[antenna] = sta->info.stats.noise[antenna];
    phy_rx_status->gain[antenna]  = sta->info.stats.gain[antenna];
  }
  return;
}

void __MTLK_IFUNC
mtlk_sta_get_peer_info (const sta_entry* sta, peerInfo_t *peer_info)
{
  mtlk_sta_get_host_if_qos_stats(sta, &peer_info->peer_host_if_qos);
  mtlk_sta_get_host_if_stats(sta, &peer_info->peer_host_if);
  mtlk_sta_get_peer_rx_stats(sta, &peer_info->peer_rx_stats);
  mtlk_sta_get_peer_ul_bsrc_tid_stats (sta, &peer_info->peer_ul_bsrc_tid);
  mtlk_sta_get_peer_baa_stats (sta, &peer_info->peer_baa_stats);
  mtlk_sta_get_plan_manager_stats (sta, &peer_info->plan_manager_stats);
  mtlk_sta_get_twt_stats (sta, &peer_info->twt_stats);
  mtlk_sta_get_per_client_stats (sta, &peer_info->per_client_stats);
  mtlk_sta_get_phy_rx_status (sta, &peer_info->phy_rx_status);
  return;
}
#endif /* CONFIG_WAVE_DEBUG */
