/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

/*
 * $Id$
 *
 *
 *
 * Radio module definitions
 *
 */
#ifndef __WAVE_RADIO_H__
#define __WAVE_RADIO_H__

/**
*\file wave_radio.h
*/
#include "mtlkhal.h"
#include "cfg80211.h"
#include "stadb.h"

#define MAX_NUM_TX_ANTENNAS_654 (2)
typedef struct _wave_radio_t wave_radio_t;

typedef struct {
  wave_radio_t  *radio;
  unsigned       num_radios;
} wave_radio_descr_t;

typedef struct {
  unsigned    max_vaps;
  unsigned    max_stas;
  unsigned    master_vap_id;
} wave_radio_limits_t;

/* These are values for parameter "is_recovery" of
   the function "wave_radio_calibrate" */
enum {
  WAVE_RADIO_FAST_RCVRY,
  WAVE_RADIO_FULL_RCVRY,
  WAVE_RADIO_NO_RCVRY = WAVE_RADIO_FULL_RCVRY
};

/* Multi BSS VAP operation modes */
enum {
  WAVE_RADIO_NON_MULTIBSS_VAP = 0,
  WAVE_RADIO_OPERATION_MODE_MBSSID_TRANSMIT_VAP = 1,
  WAVE_RADIO_OPERATION_MODE_MBSSID_NON_TRANSMIT_VAP = 2,
  WAVE_RADIO_OPERATION_MODE_MBSSID_LAST = WAVE_RADIO_OPERATION_MODE_MBSSID_NON_TRANSMIT_VAP
};

/* PhyStat averager with moving window */
#define WAVE_PHY_STAT_AVG_TIME_MS     (3 * 1000)
#define WAVE_PHY_STAT_AVG_WINSIZE     (WAVE_PHY_STAT_AVG_TIME_MS / _DF_STAT_POLL_PERIOD)

typedef struct {
  uint32  summator;
  uint32  window_size;
  uint32  idx;
  uint8   values[WAVE_PHY_STAT_AVG_WINSIZE]; /* can be made dynamic later */
} wave_phy_stat_averager_t;

static __INLINE void
wave_phy_stat_averager_init (wave_phy_stat_averager_t *avg)
{
  avg->window_size = WAVE_PHY_STAT_AVG_WINSIZE;
  avg->summator = 0;
  avg->idx = 0;
  memset(avg->values, 0, WAVE_PHY_STAT_AVG_WINSIZE);
}

static __INLINE void
wave_phy_stat_averager_proc (wave_phy_stat_averager_t *avg, uint8 val)
{
  avg->summator -= avg->values[avg->idx];
  avg->summator += val;
  avg->values[avg->idx] = val;
  INC_WRAP_IDX(avg->idx, avg->window_size);
}

static __INLINE uint8
wave_phy_stat_averager_get (wave_phy_stat_averager_t *avg)
{
  uint32 summ;
  summ = avg->summator;
  summ += avg->window_size >> 1;  /* round up */
  return summ / avg->window_size;
}

/* Radio Phy Status according to devicePhyRxStatusDb */
typedef struct {
  /* Phy Rx Status data */
  int8                  noise;   /* noise                        dBm */
  uint8                 ch_load; /* channel_load,            0..100% */
  uint8                 ch_util; /* totalChannelUtilization, 0..100% */
  /* Calculated */
  uint8                 airtime;            /*   0..100% */
  uint32                airtime_efficiency; /* bytes/sec */
  /* Averagers */
  wave_phy_stat_averager_t  ch_load_averager;
  wave_phy_stat_averager_t  ch_util_averager;
} wave_radio_phy_stat_t;

typedef struct _probe_req_info {
  uint32 size;
  struct intel_vendor_probe_req_info *data;
} probe_req_info;

/**************************************************************
    WSS counters for HW Radio statistics
 */
typedef enum
{
  WAVE_RADIO_CNT_BYTES_SENT,
  WAVE_RADIO_CNT_BYTES_RECEIVED,
  WAVE_RADIO_CNT_PACKETS_SENT,
  WAVE_RADIO_CNT_PACKETS_RECEIVED,

  WAVE_RADIO_CNT_UNICAST_PACKETS_SENT,
  WAVE_RADIO_CNT_MULTICAST_PACKETS_SENT,
  WAVE_RADIO_CNT_BROADCAST_PACKETS_SENT,
  WAVE_RADIO_CNT_UNICAST_BYTES_SENT,
  WAVE_RADIO_CNT_MULTICAST_BYTES_SENT,
  WAVE_RADIO_CNT_BROADCAST_BYTES_SENT,

  WAVE_RADIO_CNT_UNICAST_PACKETS_RECEIVED,
  WAVE_RADIO_CNT_MULTICAST_PACKETS_RECEIVED,
  WAVE_RADIO_CNT_BROADCAST_PACKETS_RECEIVED,
  WAVE_RADIO_CNT_UNICAST_BYTES_RECEIVED,
  WAVE_RADIO_CNT_MULTICAST_BYTES_RECEIVED,
  WAVE_RADIO_CNT_BROADCAST_BYTES_RECEIVED,

  WAVE_RADIO_CNT_ERROR_PACKETS_SENT,
  WAVE_RADIO_CNT_ERROR_PACKETS_RECEIVED,
  WAVE_RADIO_CNT_DISCARD_PACKETS_RECEIVED,

  WAVE_RADIO_CNT_RX_PACKETS_DISCARDED_DRV_DUPLICATE,

  WAVE_RADIO_CNT_802_1X_PACKETS_RECEIVED,
  WAVE_RADIO_CNT_802_1X_PACKETS_SENT,
  WAVE_RADIO_CNT_802_1X_PACKETS_DISCARDED,
  WAVE_RADIO_CNT_PAIRWISE_MIC_FAILURE_PACKETS,
  WAVE_RADIO_CNT_GROUP_MIC_FAILURE_PACKETS,
  WAVE_RADIO_CNT_UNICAST_REPLAYED_PACKETS,
  WAVE_RADIO_CNT_MULTICAST_REPLAYED_PACKETS,
  WAVE_RADIO_CNT_MANAGEMENT_REPLAYED_PACKETS,
  WAVE_RADIO_CNT_FWD_RX_PACKETS,
  WAVE_RADIO_CNT_FWD_RX_BYTES,
  WAVE_RADIO_CNT_DAT_FRAMES_RECEIVED,
  WAVE_RADIO_CNT_CTL_FRAMES_RECEIVED,
  WAVE_RADIO_CNT_MAN_FRAMES_RES_QUEUE,
  WAVE_RADIO_CNT_MAN_FRAMES_SENT,
  WAVE_RADIO_CNT_MAN_FRAMES_CONFIRMED,
  WAVE_RADIO_CNT_MAN_FRAMES_RECEIVED,
  WAVE_RADIO_CNT_RX_MAN_FRAMES_RETRY_DROPPED,
  WAVE_RADIO_CNT_MAN_FRAMES_FAILED,
  WAVE_RADIO_CNT_TX_PROBE_RESP_SENT,
  WAVE_RADIO_CNT_TX_PROBE_RESP_DROPPED,
  WAVE_RADIO_CNT_BSS_MGMT_TX_QUE_FULL,

  WAVE_RADIO_CNT_LAST
} wave_radio_cnt_id_e;

uint32 __MTLK_IFUNC
wave_radio_wss_cntr_get (wave_radio_t *radio, wave_radio_cnt_id_e id);

/**************************************************************/

#define WAVE_RADIO_ID_FIRST     (0)

static __INLINE BOOL
wave_radio_id_is_first (unsigned radio_id)
{
  return (WAVE_RADIO_ID_FIRST == radio_id);
}

wave_radio_descr_t* wave_radio_create(unsigned radio_num, mtlk_hw_api_t *hw_api, mtlk_work_mode_e work_mode);
int wave_radio_init(wave_radio_descr_t *radio_descr, struct device *dev);
int wave_radio_init_finalize(wave_radio_descr_t *radio_descr);
void wave_radio_destroy(wave_radio_descr_t *radio_descr);
void wave_radio_deinit(wave_radio_descr_t *radio_descr);
void wave_radio_deinit_finalize(wave_radio_descr_t *radio_descr);
void wave_radio_ieee80211_unregister(wave_radio_descr_t *radio_descr);
void wave_radio_prepare_stop(wave_radio_descr_t *radio_descr);

mtlk_hw_api_t       *wave_radio_descr_hw_api_get(wave_radio_descr_t *radio_descr, unsigned radio_idx);
mtlk_vap_manager_t  *wave_radio_descr_vap_manager_get(wave_radio_descr_t *radio_descr, unsigned radio_idx);
int   wave_radio_descr_master_vap_handle_get (wave_radio_descr_t *radio_descr, unsigned radio_idx, mtlk_vap_handle_t  *vap_handle);

unsigned             wave_radio_id_get (wave_radio_t *radio);
mtlk_wss_t          *wave_radio_wss_get(wave_radio_t *radio);
mtlk_vap_manager_t  *wave_radio_vap_manager_get(wave_radio_t *radio);

mtlk_df_t           *wave_radio_df_get(wave_radio_t *radio);
mtlk_core_t         *wave_radio_master_core_get(wave_radio_t *radio);
int   wave_radio_vap_handle_get (wave_radio_t *radio, mtlk_vap_handle_t  *vap_handle);
void *wave_radio_beacon_man_private_get(wave_radio_t *radio);

int wave_radio_beacon_change(wave_radio_t *radio, struct net_device *ndev, struct cfg80211_beacon_data *beacon);
int wave_radio_abort_and_prevent_scan (wave_radio_t *radio);
int wave_radio_allow_or_resume_scan (wave_radio_t *radio);
int wave_radio_ap_start(struct wiphy *wiphy, wave_radio_t *radio, struct net_device *ndev, struct ieee80211_bss_conf *bss_conf, struct cfg80211_beacon_data *beacon);
int wave_radio_ap_stop(wave_radio_t *radio, struct net_device *ndev);
int wave_radio_sta_change(wave_radio_t *radio, struct net_device *ndev, const uint8 *mac, struct station_parameters *params, struct ieee80211_sta * sta);
int wave_radio_scan(wave_radio_t *radio, struct net_device *ndev, struct cfg80211_scan_request *request);
int wave_radio_cancel_hw_scan (wave_radio_t *radio);
int wave_radio_qos_map_set(wave_radio_t *radio, struct net_device *ndev, struct cfg80211_qos_map *qos_map);
int wave_radio_aid_get(struct net_device *ndev, const uint8 *mac_addr, u16 *aid);
int wave_radio_aid_free(struct net_device *ndev, u16 aid);
int wave_radio_sync_done(struct net_device *ndev);
int wave_radio_initial_data_receive(struct net_device *ndev, struct intel_vendor_initial_data_cfg *initial_data);
int wave_radio_deny_mac_set(struct net_device *ndev, struct intel_vendor_blacklist_cfg *blacklist_cfg);
int wave_radio_sta_steer(struct net_device *ndev, struct intel_vendor_steer_cfg *steer_cfg);
int wave_radio_sta_measurements_get(struct wiphy *wiphy, struct net_device *ndev, uint8 *addr);
int wave_radio_vap_measurements_get(struct wiphy *wiphy, struct net_device *ndev);
int wave_radio_set_atf_quotas(struct wiphy *wiphy, struct net_device *ndev, const void *data, int total_len);
int wave_radio_set_wds_wpa_sta(struct net_device *ndev, struct intel_vendor_mac_addr_list_cfg *addrlist_cfg);

int wave_radio_unconnected_sta_get(struct wireless_dev *wdev, struct intel_vendor_unconnected_sta_req_cfg *sta_req_data);

int wave_radio_radio_info_get(struct wiphy *wiphy, struct wireless_dev *wdev);

int wave_radio_antenna_set(wave_radio_t *radio, u32 tx_ant, u32 rx_ant);
int wave_radio_antenna_get(wave_radio_t *radio, u32 *tx_ant, u32 *rx_ant);

int wave_radio_rts_threshold_set(wave_radio_t *radio, u32 rts_threshold);

/* WIFI HAL* API's */
int wave_radio_get_associated_dev_stats(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_channel_stats(struct wiphy *wiphy, struct net_device *ndev, const void *data, size_t data_len);
int wave_radio_get_phy_channel_status(struct wiphy *wiphy, struct net_device *ndev);
int wave_radio_get_associated_dev_tid_stats(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_associated_dev_rate_info_rx_stats(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_associated_dev_rate_info_tx_stats(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);

int wave_radio_get_twt_params(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);

int wave_radio_get_peer_list(struct wiphy *wiphy, struct net_device *ndev);
int wave_radio_get_peer_flow_status(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_peer_capabilities(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_peer_rate_info(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_recovery_statistics(struct wiphy *wiphy, struct net_device *ndev);
int wave_radio_get_hw_flow_status(struct wiphy *wiphy, struct net_device *ndev);
int wave_radio_get_tr181_wlan_statistics(struct wiphy *wiphy, struct net_device *ndev);
int wave_radio_get_tr181_hw_statistics(struct wiphy *wiphy, struct net_device *ndev);
int wave_radio_get_tr181_peer_statistics(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_dev_diag_result2(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_dev_diag_result3(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr);
int wave_radio_get_associated_stations_stats(struct wiphy *wiphy, struct net_device *ndev, const uint8 *addr_list, size_t data_len);
int wave_radio_get_priority_gpio_stats(struct wiphy *wiphy, struct net_device *ndev);

/* CCA API */
int __MTLK_IFUNC wave_radio_cca_stats_get (struct wiphy *wiphy, struct net_device *ndev);
int __MTLK_IFUNC wave_radio_cca_msr_start(struct wiphy *wiphy, struct net_device *ndev, struct intel_vendor_cca_msr_cfg *cca_msr_cfg);
int __MTLK_IFUNC wave_radio_cca_msr_get(struct wiphy *wiphy, struct net_device *ndev, uint32 *user_chan);
void __MTLK_IFUNC wave_radio_cca_msr_done (wave_radio_t *radio, mtlk_core_t *core);
void __MTLK_IFUNC wave_radio_cca_stat_update (wave_radio_t *radio, wave_cca_stats_t *new_cca_stats, int channel);

/* General Radio stats */
int __MTLK_IFUNC wave_radio_usage_stats_get (struct wiphy *wiphy, struct net_device *ndev);

#ifdef CONFIG_WAVE_DEBUG
int wave_radio_get_peer_host_if_qos(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_peer_host_if(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_peer_rx_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_peer_ul_bsrc_tid_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_peer_baa_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_link_adaption_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_plan_manager_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_peer_twt_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_per_client_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_peer_phy_rx_status(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_peer_info(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_wlan_host_if_qos(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_wlan_host_if(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_wlan_rx_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_wlan_baa_stats(wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_radio_rx_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_radio_baa_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_tsman_init_tid_gl (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_tsman_init_sta_gl (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_tsman_rcpt_tid_gl (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_tsman_rcpt_sta_gl (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_radio_link_adapt_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_multicast_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_training_man_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_group_man_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_general_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_cur_channel_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_radio_phy_rx_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_dynamic_bw_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
int wave_radio_get_link_adapt_mu_stats (wave_radio_t *radio, struct net_device *ndev, const void *data, int total_len);
#endif /* CONFIG_WAVE_DEBUG */

int wave_radio_he_operation_get(wave_radio_t *radio, struct wiphy *wiphy, struct wireless_dev *wdev);
int wave_radio_he_non_advertised_get(wave_radio_t *radio, struct wiphy *wiphy, struct wireless_dev *wdev);

void wave_radio_ab_register(wave_radio_t *radio);
void wave_radio_ab_unregister(wave_radio_t *radio);

uint8 wave_radio_last_pm_freq_get(wave_radio_t *radio);
void wave_radio_last_pm_freq_set(wave_radio_t *radio, uint8 last_pm_freq);
mtlk_pdb_t *wave_radio_param_db_get(wave_radio_t *radio);

void __MTLK_IFUNC _wave_radio_set_11n_acax_compat (wave_radio_t *radio, int value);
int __MTLK_IFUNC wave_radio_get_11n_acax_compat (wave_radio_t *radio);

#define WAVE_RADIO_PDB_GET_INT(radio, id) \
  wave_pdb_get_int(wave_radio_param_db_get((radio)), (id))

#define WAVE_RADIO_PDB_GET_UINT(radio, id) \
  (unsigned)wave_pdb_get_int(wave_radio_param_db_get((radio)), (id))

#define WAVE_RADIO_PDB_SET_INT(radio, id, value) \
  wave_pdb_set_int(wave_radio_param_db_get((radio)), (id), (unsigned)(value))

#define WAVE_RADIO_PDB_SET_UINT(radio, id, value) \
  wave_pdb_set_int(wave_radio_param_db_get((radio)), (id), (value))

#define WAVE_RADIO_PDB_GET_BINARY(radio, id, buf, size) \
  wave_pdb_get_binary(wave_radio_param_db_get((radio)), (id), (buf), (size))

#define WAVE_RADIO_PDB_SET_BINARY(radio, id, value, size) \
  wave_pdb_set_binary(wave_radio_param_db_get((radio)), (id), (value), (size))

#define WAVE_RADIO_PDB_GET_MAC(radio, id, mac) \
  wave_pdb_get_mac(wave_radio_param_db_get((radio)), (id), (mac))

#define WAVE_RADIO_PDB_SET_MAC(radio, id, mac) \
  wave_pdb_set_mac(wave_radio_param_db_get((radio)), (id), (mac))

unsigned wave_radio_scan_is_running(wave_radio_t *radio);
unsigned wave_radio_scan_is_ignorant(wave_radio_t *radio);
uint32 wave_radio_scan_flags_get(wave_radio_t *radio);
mtlk_scan_support_t* wave_radio_scan_support_get(wave_radio_t *radio);
struct mtlk_chan_def* wave_radio_chandef_get(wave_radio_t *radio);
int wave_radio_chan_switch_type_get(wave_radio_t *radio);
void wave_radio_chan_switch_type_set(wave_radio_t *radio, int value);
void wave_radio_chandef_copy(struct mtlk_chan_def *mcd, struct cfg80211_chan_def *chandef);
int wave_radio_set_first_non_dfs_chandef (wave_radio_t * radio);

UMI_HDK_CONFIG *wave_radio_hdkconfig_get(wave_radio_t *radio);
mtlk_coc_t *wave_radio_coc_mgmt_get(wave_radio_t *radio);
mtlk_erp_t *wave_radio_erp_mgmt_get(wave_radio_t *radio);
void __MTLK_IFUNC wave_radio_interfdet_set(wave_radio_t *radio, BOOL enable_flag, int8 threshold);
BOOL wave_radio_interfdet_get(wave_radio_t *radio);
BOOL wave_radio_is_rate_80211b(uint8 bitrate);
BOOL wave_radio_is_rate_80211ag(uint8 bitrate);
uint32 wave_radio_mode_get(wave_radio_t *radio);
void wave_radio_mode_set(wave_radio_t *radio, const uint32 radio_mode);
void wave_radio_limits_set(wave_radio_descr_t *radio_descr, wave_radio_limits_t *radio_limits);
unsigned wave_radio_max_stas_get(wave_radio_t *radio);
unsigned wave_radio_max_vaps_get(wave_radio_t *radio);
unsigned wave_radio_master_vap_id_get(wave_radio_t *radio);

uint32 wave_radio_chandef_width_get(const struct cfg80211_chan_def *c);

BOOL    __MTLK_IFUNC wave_radio_progmodel_loaded_get (struct _wave_radio_t *radio);
void    __MTLK_IFUNC wave_radio_progmodel_loaded_set (struct _wave_radio_t *radio, BOOL is_loaded);

BOOL    __MTLK_IFUNC wave_radio_get_sta_vifs_exist (wave_radio_t *radio);
void    __MTLK_IFUNC wave_radio_recover_sta_vifs (wave_radio_t *radio);

void wave_radio_sta_cnt_inc(wave_radio_t *radio);
void wave_radio_sta_cnt_dec(wave_radio_t *radio);
int  wave_radio_sta_cnt_get(wave_radio_t *radio);

struct  ieee80211_hw    * __MTLK_IFUNC wave_radio_ieee80211_hw_get (wave_radio_t *radio);
struct  _wv_mac80211_t  * __MTLK_IFUNC wave_radio_mac80211_get (wave_radio_t *radio);
struct  _wave_radio_t   * __MTLK_IFUNC wave_radio_descr_wave_radio_get (wave_radio_descr_t *radio_descr, unsigned idx);

int  __MTLK_IFUNC wave_radio_cca_threshold_get (wave_radio_t *radio, iwpriv_cca_th_t *cca_th);
int  __MTLK_IFUNC wave_radio_cca_threshold_set (wave_radio_t *radio, iwpriv_cca_th_t *cca_th);

BOOL __MTLK_IFUNC wave_radio_is_phy_dummy(wave_radio_t *radio);
BOOL __MTLK_IFUNC wave_radio_is_first(wave_radio_t *radio);

int wave_radio_channel_table_build_2ghz(wave_radio_t *radio, struct ieee80211_channel *channels, int n_channels);
int wave_radio_channel_table_build_5ghz(wave_radio_t *radio, struct ieee80211_channel *channels, int n_channels);
int wave_radio_channel_table_build_6ghz(wave_radio_t *radio, struct ieee80211_channel *channels, int n_channels);
void wave_radio_channel_table_print(wave_radio_t *radio);
int wave_radio_calibrate(wave_radio_descr_t *radio_descr, BOOL is_recovery);
void wave_radio_band_set(wave_radio_t *radio, nl80211_band_e ieee_band);
mtlk_hw_band_e wave_radio_band_get(wave_radio_t *radio);
uint32 _wave_radio_chandef_width_get (const struct cfg80211_chan_def *c);
void __MTLK_IFUNC wave_radio_ch_switch_event (struct wiphy *wiphy, mtlk_core_t *core, struct mtlk_chan_def *mcd);
void wave_radio_calibration_status_get(wave_radio_t *radio, uint8 idx, uint8 *calib_done_mask, uint8 *calib_failed_mask);

int __MTLK_IFUNC wave_radio_send_hdk_config(wave_radio_t *radio, uint32 offline_mask, uint32 online_mask);
int __MTLK_IFUNC wave_radio_read_hdk_config(wave_radio_t *radio, uint32 *offline_mask, uint32 *online_mask);

uint8 __MTLK_IFUNC  wave_radio_max_tx_antennas_get(wave_radio_t *radio);
uint8 __MTLK_IFUNC  wave_radio_max_rx_antennas_get(wave_radio_t *radio);
uint8 __MTLK_IFUNC  wave_radio_tx_antenna_mask_get(wave_radio_t *radio);
uint8 __MTLK_IFUNC  wave_radio_rx_antenna_mask_get(wave_radio_t *radio);

void  __MTLK_IFUNC  wave_radio_antenna_cfg_update(wave_radio_t *radio, uint8 mask);

void  __MTLK_IFUNC  wave_radio_current_antenna_mask_reset(wave_radio_t *radio);
void  __MTLK_IFUNC  wave_radio_current_antenna_mask_set(wave_radio_t *radio, uint8 mask);
uint8 __MTLK_IFUNC  wave_radio_current_antenna_mask_get(wave_radio_t *radio);

void __MTLK_IFUNC
wave_radio_ant_masks_num_sts_get (wave_radio_t *radio, u32 *tx_ant_mask, u32 *rx_ant_mask, u32 *num_sts);

wv_cfg80211_t * __MTLK_IFUNC wave_radio_cfg80211_get(wave_radio_t *radio);
int __MTLK_IFUNC wave_radio_country_code_set(wave_radio_t *radio, const mtlk_country_code_t *country_code);
int __MTLK_IFUNC wave_radio_country_code_set_by_str(wave_radio_t *radio, char *str, size_t len);

BOOL __MTLK_IFUNC wave_radio_is_init_done(wave_radio_t *radio);

#endif

void __MTLK_IFUNC
wave_radio_total_traffic_delta_set (wave_radio_t *radio, uint32 total_traffic_delta);

uint32 __MTLK_IFUNC
wave_radio_airtime_efficiency_get (wave_radio_t *radio);

void __MTLK_IFUNC
wave_radio_phy_status_update (wave_radio_t *radio, wave_radio_phy_stat_t *params);

void __MTLK_IFUNC
wave_radio_phy_status_get (wave_radio_t *radio, wave_radio_phy_stat_t *params);

uint8 __MTLK_IFUNC
wave_radio_channel_load_get (wave_radio_t *radio);

void __MTLK_IFUNC
wave_radio_get_tr181_hw_stats (wave_radio_t *radio, mtlk_wssa_drv_tr181_hw_stats_t *stats);

void __MTLK_IFUNC
wave_radio_get_hw_stats (wave_radio_t *radio, mtlk_wssa_drv_hw_stats_t *stats);

void __MTLK_IFUNC wave_radio_sync_hostapd_done_set(wave_radio_t *radio, BOOL value);
BOOL __MTLK_IFUNC wave_radio_is_sync_hostapd_done(wave_radio_t *radio);
int __MTLK_IFUNC wave_radio_get_num_of_channels (mtlk_hw_band_e hw_band, int* num_of_channels);
wave_ant_params_t *wave_radio_get_ant_params (wave_radio_t *radio);
int __MTLK_IFUNC
wave_radio_cdb_antenna_cfg_update (wave_radio_t *radio, uint8 ant_mask);
int __MTLK_IFUNC
wave_radio_get_zwdfs_ant_enabled (wave_radio_t *radio);
void __MTLK_IFUNC
wave_radio_set_zwdfs_ant_enabled (wave_radio_t *radio, const int enable);
BOOL __MTLK_IFUNC wave_radio_get_is_zwdfs_radio (wave_radio_t *radio);
BOOL __MTLK_IFUNC wave_radio_get_is_zwdfs_operational_radio (wave_radio_t *radio);
wave_radio_t * __MTLK_IFUNC wave_radio_descr_get_zwdfs_radio (wave_radio_descr_t *radio_descr);

int __MTLK_IFUNC
wave_radio_set_cts_to_self_to (wave_radio_t *radio, const int cts_to_self_to);
int __MTLK_IFUNC wave_radio_get_cts_to_self_to (wave_radio_t *radio);

int __MTLK_IFUNC
wave_radio_set_tx_ampdu_density (wave_radio_t *radio, const int ampdu_density);
uint32 __MTLK_IFUNC wave_radio_get_tx_ampdu_density (wave_radio_t *radio);
uint32 __MTLK_IFUNC  wave_radio_max_tx_ampdu_density_get(wave_radio_t *radio, uint32 sta_ampdu_density);
void __MTLK_IFUNC wave_radio_on_rcvry_isol(wave_radio_t *radio);
void __MTLK_IFUNC wave_radio_on_rcvry_restore(wave_radio_t *radio);
void __MTLK_IFUNC wave_radio_bss_tx_timer_deinit(wave_radio_descr_t *radio_descr);
int __MTLK_IFUNC wave_radio_bss_tx_timer_init(wave_radio_descr_t *radio_descr);

void __MTLK_IFUNC wave_radio_set_cac_pending(wave_radio_t *radio, BOOL state);
BOOL __MTLK_IFUNC wave_radio_get_cac_pending(wave_radio_t *radio);

void __MTLK_IFUNC wave_radio_set_unsolicited_frame_tx(wave_radio_t *radio, struct net_device *ndev, const void *data, int data_len);

void __MTLK_IFUNC wave_radio_roc_in_progress_set(wave_radio_t *radio, unsigned val);
unsigned __MTLK_IFUNC wave_radio_roc_in_progress_get(wave_radio_t *radio);

void __MTLK_IFUNC wave_radio_clean_probe_req_list(wave_radio_t *radio);
void __MTLK_IFUNC wave_probe_req_list_add(wave_radio_t *radio, const IEEE_ADDR *addr);

int __MTLK_IFUNC wave_radio_get_probe_req_list (struct wiphy *wiphy, struct net_device *ndev);
int __MTLK_IFUNC
wave_radio_copy_probe_req_list(wave_radio_t *radio, probe_req_info *info);

int __MTLK_IFUNC wave_radio_preamble_punct_cca_ov_cfg_get (wave_radio_t *radio, UMI_PREAMBLE_PUNCT_CCA_OVERRIDE *req);

BOOL __MTLK_IFUNC wave_radio_was_scan_done_since_prev_sta_poll (wave_radio_t *radio);
void __MTLK_IFUNC wave_radio_set_scan_done_since_prev_sta_poll (wave_radio_t *radio, BOOL value);
