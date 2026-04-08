/******************************************************************************

                               Copyright (c) 2013
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/

/*
 * $Id$
 *
 *
 *
 * Core configuration implementation
 *
 */
#include "mtlkinc.h"
#include "core.h"
#include "core_config.h"
#include "mtlk_vap_manager.h"
#include "eeprom.h"
#include "hw_mmb.h"
#include "mtlkaux.h"
#include "mtlk_df_user_priv.h"
#include "mtlkdfdefs.h"
#include "mhi_umi.h"
#include "mhi_umi_propr.h"
#include "scan_support.h"
#include "mtlk_df_priv.h"
#include "mtlk_df.h"
#include "cfg80211.h"
#include "mac80211.h"
#include "mcast.h"
#include "wave_fapi_if.h"
#include "wave_hal_stats.h"
#include "bitrate.h"
#include <linux/jiffies.h>

#define LOG_LOCAL_GID   GID_CORE
#define LOG_LOCAL_FID   5

/* convert TID to AC   */
static const uint8 convert_tid_to_AC[HAL_NUM_OF_TIDS] = {
    WIFI_RADIO_QUEUE_TYPE_BE,
    WIFI_RADIO_QUEUE_TYPE_BK,
    WIFI_RADIO_QUEUE_TYPE_BK,
    WIFI_RADIO_QUEUE_TYPE_BE,
    WIFI_RADIO_QUEUE_TYPE_VI,
    WIFI_RADIO_QUEUE_TYPE_VI,
    WIFI_RADIO_QUEUE_TYPE_VO,
    WIFI_RADIO_QUEUE_TYPE_VO,
    WIFI_RADIO_QUEUE_TYPE_BE,
    WIFI_RADIO_QUEUE_TYPE_BK,
    WIFI_RADIO_QUEUE_TYPE_BK,
    WIFI_RADIO_QUEUE_TYPE_BE,
    WIFI_RADIO_QUEUE_TYPE_VI,
    WIFI_RADIO_QUEUE_TYPE_VI,
    WIFI_RADIO_QUEUE_TYPE_VO,
    WIFI_RADIO_QUEUE_TYPE_VO,
};

static inline unsigned int mtlk_nsecs_to_msecs(u64 nsecs)
{
  return jiffies_to_msecs(nsecs_to_jiffies(nsecs));
}

int __MTLK_IFUNC
mtlk_core_get_associated_dev_tid_stats (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  peerTidStats tid_stats;
  int tid_idx;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      memset(&tid_stats, 0, sizeof(peerTidStats));
      /* Plume needs the following per TID stats, however FW maintains per STA
         Thus we are filling the same statistic for all 16 TIDs                */
      for (tid_idx = 0; tid_idx < HAL_NUM_OF_TIDS; tid_idx++) {
        /* Number of msdus in given interval for per STA */
        tid_stats[tid_idx].num_msdus    = sta->sta_stats64_cntrs.numMsdus;
        /* Moving average value based on last couple of transmitted msdus for per station */
        tid_stats[tid_idx].ewma_time_ms = mtlk_nsecs_to_msecs((uint64)sta->sta_stats_cntrs.ewmaTimeNsec);
        /* Delta of cumulative msdus times over interval for per station  */
        tid_stats[tid_idx].sum_time_ms  = mtlk_nsecs_to_msecs(sta->sta_stats64_cntrs.sumTimeNsec);
        tid_stats[tid_idx].tid = tid_idx;
        tid_stats[tid_idx].ac  = convert_tid_to_AC[tid_idx];
      }
      mtlk_sta_decref(sta);
  }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &tid_stats, sizeof(peerTidStats));
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
mtlk_core_get_channel_stats (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_channel_stats *chan_stats = NULL;
  int res = MTLK_ERR_OK;
  unsigned int num_of_channels = 0, channels_list_size, chan_idx, *channels_list;
  size_t total_size = 0;
  mtlk_hw_t *hw = NULL;

  channels_list = mtlk_clpb_enum_get_next(clpb, &channels_list_size);
  num_of_channels = channels_list_size/(sizeof(*channels_list));
  MTLK_CLPB_TRY_EX(channels_list, channels_list_size, sizeof(*channels_list)*num_of_channels)

    hw = mtlk_vap_get_hw(core->vap_handle);

    total_size = sizeof(mtlk_channel_stats) * num_of_channels;
    chan_stats = mtlk_osal_mem_alloc(total_size, MTLK_MEM_TAG_EXTENSION);
    if (NULL == chan_stats) {
      ELOG_V("Can't allocate memory");
      MTLK_CLPB_EXIT(MTLK_ERR_NO_MEM);
    }
    memset(chan_stats, 0, total_size);

    for (chan_idx = 0; chan_idx < num_of_channels; chan_idx++) {
      mtlk_hw_copy_stats_per_channel(hw, &chan_stats[chan_idx], channels_list[chan_idx]);
    }
  MTLK_CLPB_FINALLY(res)
     res = mtlk_clpb_push_res_data_no_copy(clpb, res, chan_stats, total_size);
    if (MTLK_ERR_OK != res && chan_stats != NULL) {
      mtlk_osal_mem_free(chan_stats);
    }
    return res;
  MTLK_CLPB_END;
}

static const uint16 hal_cbw[] = {HAL_BW_20MHZ, HAL_BW_40MHZ, HAL_BW_80MHZ, HAL_BW_160MHZ};
void __MTLK_IFUNC
mtlk_core_get_associated_dev_rate_info_rx_stats (sta_entry * sta, peerRateInfoRxStats * peer_rx_rate_info)
{
  uint8 cbw, i, mode;
  cbw  = mtlk_bitrate_params_get_psdu_cbw(sta->info.stats.rx_psdu_data_rate_info);
  mode = mtlk_bitrate_params_get_psdu_mode(sta->info.stats.rx_psdu_data_rate_info);

  memset(peer_rx_rate_info, 0, sizeof(*peer_rx_rate_info));

  /* Handling mcs seperately incase of HT(11n) mode because WIFI HAL expects mcs value <0-7> for HT(11n)
    * but as per statndard HT mcs value is <0-32> */
  if (PHY_MODE_N == mode){
    mtlk_bitrate_params_get_11n_mcs_and_nss_for_plume(sta->info.stats.rx_psdu_data_rate_info, &peer_rx_rate_info->mcs, &peer_rx_rate_info->nss);
  } else {
    mtlk_bitrate_params_get_psdu_mcs_and_nss(sta->info.stats.rx_psdu_data_rate_info, &peer_rx_rate_info->mcs, &peer_rx_rate_info->nss);
  }

  peer_rx_rate_info->bw = hal_cbw[cbw];
  peer_rx_rate_info->bytes = sta->sta_stats64_cntrs.rxOutStaNumOfBytes;
  peer_rx_rate_info->flags |= BIT_ULL(HAS_BYTES);
  peer_rx_rate_info->msdus = sta->sta_stats64_cntrs.rdCount;
  peer_rx_rate_info->flags |= BIT_ULL(HAS_MSDUS);
  peer_rx_rate_info->mpdus = sta->sta_stats64_cntrs.mpduInAmpdu;
  peer_rx_rate_info->flags |= BIT_ULL(HAS_MPDUS);
  peer_rx_rate_info->ppdus = sta->sta_stats64_cntrs.ampdu;
  peer_rx_rate_info->flags |= BIT_ULL(HAS_PPDUS);
  peer_rx_rate_info->retries = sta->sta_stats64_cntrs.rxRetryCount;
  peer_rx_rate_info->rssi_combined = sta->info.stats.max_rssi;
  peer_rx_rate_info->flags |= BIT_ULL(HAS_RSSI_COMB);
  for (i = 0; i < MAX_NUM_RX_ANTENNAS; i++) {
    peer_rx_rate_info->rssi_array[i][0] = sta->info.stats.data_rssi[i];
  }
  peer_rx_rate_info->flags |= BIT_ULL(HAS_RSSI_ARRAY);
}

int __MTLK_IFUNC
mtlk_core_get_associated_dev_rate_info_rx_stats_clb (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  peerRateInfoRxStats peer_rx_rate_info;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      mtlk_core_get_associated_dev_rate_info_rx_stats(sta, &peer_rx_rate_info);
      mtlk_sta_decref(sta);
    }
  MTLK_CLPB_FINALLY(res)
  return mtlk_clpb_push_res_data(clpb, res, &peer_rx_rate_info, sizeof(peerRateInfoRxStats));
  MTLK_CLPB_END;
}

void __MTLK_IFUNC
mtlk_core_calc_associated_dev_rate_info_rx_stats (peerRateInfoRxStats * dest, peerRateInfoRxStats * new, peerRateInfoRxStats * old)
{
  dest->bytes += new->bytes - old->bytes;
  dest->msdus += new->msdus - old->msdus;
  dest->mpdus += new->mpdus - old->mpdus;
  dest->ppdus += new->ppdus - old->ppdus;
  dest->retries += new->retries - old->retries;
  dest->rssi_combined = new->rssi_combined;
  wave_memcpy(dest->rssi_array, sizeof(dest->rssi_array), new->rssi_array, sizeof(new->rssi_array));
}

void __MTLK_IFUNC
mtlk_core_get_associated_dev_rate_info_tx_stats (sta_entry * sta, peerRateInfoTxStats * peer_tx_rate_info)
{
  uint8 cbw, mode;
  mode = mtlk_bitrate_params_get_mode(sta->info.stats.tx_data_rate_params);

  memset(peer_tx_rate_info, 0, sizeof(*peer_tx_rate_info));

  if ((mode == PHY_MODE_AG) || (mode == PHY_MODE_B)) {
    peer_tx_rate_info->nss = HAL_LEGACY_NSS;
  } else {
    peer_tx_rate_info->nss = mtlk_bitrate_params_get_nss(sta->info.stats.tx_data_rate_params);
  }

  peer_tx_rate_info->mcs = mtlk_bitrate_params_get_mcs(sta->info.stats.tx_data_rate_params);
  cbw = mtlk_bitrate_params_get_cbw(sta->info.stats.tx_data_rate_params);
  peer_tx_rate_info->bw = hal_cbw[cbw];
  peer_tx_rate_info->bytes = sta->sta_stats64_cntrs.mpduByteTransmitted;
  peer_tx_rate_info->flags |= BIT_ULL(HAS_BYTES);
  peer_tx_rate_info->msdus = sta->sta_stats64_cntrs.successCount + sta->sta_stats64_cntrs.exhaustedCount;
  peer_tx_rate_info->flags |= BIT_ULL(HAS_MSDUS);
  peer_tx_rate_info->mpdus = sta->sta_stats64_cntrs.mpduTransmitted;
  peer_tx_rate_info->flags |= BIT_ULL(HAS_MPDUS);
  peer_tx_rate_info->ppdus = sta->sta_stats64_cntrs.transmittedAmpdu;
  peer_tx_rate_info->flags |= BIT_ULL(HAS_PPDUS);
  peer_tx_rate_info->retries = sta->sta_stats64_cntrs.txRetryCount;
  peer_tx_rate_info->attempts = sta->sta_stats64_cntrs.mpduFirstRetransmission;
}

int __MTLK_IFUNC
mtlk_core_get_associated_dev_rate_info_tx_stats_clb (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  peerRateInfoTxStats peer_tx_rate_info;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      mtlk_core_get_associated_dev_rate_info_tx_stats(sta, &peer_tx_rate_info);
      mtlk_sta_decref(sta);
    }

  MTLK_CLPB_FINALLY(res)
  return mtlk_clpb_push_res_data(clpb, res, &peer_tx_rate_info, sizeof(peerRateInfoTxStats));
  MTLK_CLPB_END;
}

void __MTLK_IFUNC
mtlk_core_calc_associated_dev_rate_info_tx_stats (peerRateInfoTxStats * dest, peerRateInfoTxStats * new, peerRateInfoTxStats * old)
{
  dest->bytes += new->bytes - old->bytes;
  dest->msdus += new->msdus - old->msdus;
  dest->mpdus += new->mpdus - old->mpdus;
  dest->retries += new->retries - old->retries;
  dest->attempts += new->attempts - old->attempts;
}

int __MTLK_IFUNC
mtlk_core_get_associated_dev_stats (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  peerFlowStats peer_stats;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      memset(&peer_stats , 0, sizeof(peerFlowStats));
	  mtlk_sta_get_associated_dev_stats(sta, &peer_stats);
      mtlk_sta_decref(sta);
    }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &peer_stats, sizeof(peerFlowStats));
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
mtlk_core_stats_poll_period_get (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_hw_t *hw = NULL;
  uint32 poll_period = 0;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  hw = mtlk_vap_get_hw(core->vap_handle);
  MTLK_ASSERT(hw);

  /* read config from internal db */
  poll_period = wave_hw_mmb_get_stats_poll_period(hw);

  /* push result and config to clipboard*/
  res = mtlk_clpb_push_res_data(clpb, MTLK_ERR_OK, &poll_period,
    sizeof(poll_period));

  return res;
}

int __MTLK_IFUNC
mtlk_core_stats_poll_period_set (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
  mtlk_df_user_t *df_user = mtlk_df_get_user(df);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  uint32 *poll_period = NULL, res_size;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  poll_period = mtlk_clpb_enum_get_next(clpb, &res_size);

  if (poll_period) {
    wave_hw_mmb_set_stats_poll_period(df, *poll_period);
    mtlk_df_user_stats_timer_set(df_user, *poll_period);
  } else {
    ILOG2_V("Poll period is NULL");
    res = MTLK_ERR_UNKNOWN;
  }

  return mtlk_clpb_push_res_data(clpb, res, &res, sizeof(res));
}

int __MTLK_IFUNC
mtlk_core_get_peer_list (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *nic = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  if (0 == (mtlk_core_get_net_state(nic) & (NET_STATE_HALTED | NET_STATE_CONNECTED))) {
    return MTLK_ERR_OK;
  }

  return mtlk_stadb_get_peer_list(&nic->slow_ctx->stadb, clpb);
}

int __MTLK_IFUNC
mtlk_core_get_peer_flow_status (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  mtlk_wssa_drv_peer_stats_t peer_flow_status;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      mtlk_sta_get_peer_stats(sta, &peer_flow_status);
      mtlk_sta_decref(sta);
    }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &peer_flow_status, sizeof(mtlk_wssa_drv_peer_stats_t));
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
mtlk_core_get_peer_capabilities (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  mtlk_wssa_drv_peer_capabilities_t peer_capabilities;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      mtlk_sta_get_peer_capabilities(sta, &peer_capabilities);
      mtlk_sta_decref(sta);
    }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &peer_capabilities, sizeof(mtlk_wssa_drv_peer_capabilities_t));
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
mtlk_core_get_peer_rate_info (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  mtlk_wssa_drv_peer_rates_info_t peer_rate_info;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      _mtlk_sta_get_peer_rates_info(sta, &peer_rate_info);
      mtlk_sta_decref(sta);
    }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &peer_rate_info, sizeof(mtlk_wssa_drv_peer_rates_info_t));
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
mtlk_core_get_recovery_stats (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_wssa_drv_recovery_stats_t recovery_stats;
  mtlk_hw_t *hw = NULL;

  hw = mtlk_vap_get_hw(core->vap_handle);
  MTLK_ASSERT(hw);

  wave_hw_get_recovery_stats(hw, &recovery_stats);

  return mtlk_clpb_push_res_data(clpb, MTLK_ERR_OK, &recovery_stats, sizeof(mtlk_wssa_drv_recovery_stats_t));
}

int __MTLK_IFUNC
mtlk_core_get_hw_flow_status (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  wave_radio_t  *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_wssa_drv_hw_stats_t hw_flow_status;

  wave_radio_get_hw_stats(radio, &hw_flow_status);

  return mtlk_clpb_push_res_data(clpb, MTLK_ERR_OK, &hw_flow_status, sizeof(mtlk_wssa_drv_hw_stats_t));
}

int __MTLK_IFUNC
mtlk_core_get_tr181_wlan_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_wssa_drv_tr181_wlan_stats_t tr181_stats;

  mtlk_core_get_tr181_wlan_stats(core, &tr181_stats);

  return mtlk_clpb_push_res_data(clpb, MTLK_ERR_OK, &tr181_stats, sizeof(mtlk_wssa_drv_tr181_wlan_stats_t));
}

int __MTLK_IFUNC
mtlk_core_get_tr181_hw_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  wave_radio_t  *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_wssa_drv_tr181_hw_stats_t tr181_stats;

  wave_radio_get_tr181_hw_stats(radio, &tr181_stats);

  return mtlk_clpb_push_res_data(clpb, MTLK_ERR_OK, &tr181_stats, sizeof(mtlk_wssa_drv_tr181_hw_stats_t));
}

int __MTLK_IFUNC
mtlk_core_get_tr181_peer_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  st_info_data_t *info_data;
  unsigned info_data_size;
  sta_entry *sta = NULL;
  mtlk_wssa_drv_tr181_peer_stats_t tr181_stats;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  info_data = mtlk_clpb_enum_get_next(clpb, &info_data_size);
  MTLK_CLPB_TRY(info_data, info_data_size)

  if (info_data->sta)
    sta = wv_ieee80211_get_sta(info_data->sta);
  else
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, info_data->mac);

  if (NULL == sta) {
    ILOG1_DY("CID-%04x: station %Y not found",
               mtlk_vap_get_oid(core->vap_handle), info_data->mac);
    MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
  } else {
      mtlk_sta_get_tr181_peer_stats(sta, &tr181_stats);
      mtlk_sta_decref(sta);
  }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &tr181_stats, sizeof(mtlk_wssa_drv_tr181_peer_stats_t));
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
wave_core_get_dev_diag_result2 (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  wifiAssociatedDevDiagnostic2_t dev_diag_res2_stats;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)

    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    } else {
      wave_sta_get_dev_diagnostic_res2(core, sta, &dev_diag_res2_stats);
      mtlk_sta_decref(sta);
    }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &dev_diag_res2_stats, sizeof(wifiAssociatedDevDiagnostic2_t));
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
wave_core_get_dev_diag_result3 (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  wifiAssociatedDevDiagnostic3_t *dev_diag_res3_stats = NULL;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)
    dev_diag_res3_stats = mtlk_osal_mem_alloc(sizeof(wifiAssociatedDevDiagnostic3_t), MTLK_MEM_TAG_EXTENSION);
    if (NULL == dev_diag_res3_stats) {
      ELOG_V("Can't allocate memory for dev_diag_res3_stats struct");
      MTLK_CLPB_EXIT(MTLK_ERR_NO_MEM);
    }
    memset(dev_diag_res3_stats, 0, sizeof(wifiAssociatedDevDiagnostic3_t));
  
      /* find station in stadb */
      sta = mtlk_vap_find_sta(core->vap_handle, addr->au8Addr);
      if (NULL == sta) {
        WLOG_DY("CID-%04x: station %Y not found",
          mtlk_vap_get_oid(core->vap_handle), addr);
        MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
      } else {
        wave_sta_get_dev_diagnostic_res3(core, sta, dev_diag_res3_stats);
        mtlk_sta_decref(sta);
      }
  
  MTLK_CLPB_FINALLY(res)
    res = mtlk_clpb_push_res_data_no_copy(clpb, res, dev_diag_res3_stats, sizeof(wifiAssociatedDevDiagnostic3_t));
    if (MTLK_ERR_OK != res && dev_diag_res3_stats != NULL) {
      mtlk_osal_mem_free(dev_diag_res3_stats);
    }
    return res;
  MTLK_CLPB_END;
}

int __MTLK_IFUNC
mtlk_core_get_priority_gpio_statistics (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_hw_t *hw = NULL;
  mtlk_priority_gpio_stats_t pta_stats;

  hw = mtlk_vap_get_hw(core->vap_handle);
  mtlk_hw_get_priority_gpio_stats(hw, &pta_stats);

  return mtlk_clpb_push_res_data(clpb, MTLK_ERR_OK, &pta_stats, sizeof(mtlk_priority_gpio_stats_t));
}
