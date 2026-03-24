/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLK_MAC80211_H_
#define _MTLK_MAC80211_H_

#include <net/cfg80211.h>
#include <net/mac80211.h>
#include <linux/netdevice.h>
#include "mtlk_df_user_priv.h"
#include "mtlk_coreui.h"

typedef struct _wv_mac80211_t wv_mac80211_t;
typedef struct _mtlk_df_user_t mtlk_df_user_t;
typedef struct _wave_radio_t wave_radio_t;

#define MBSSID_AID_OFFSET0 0
#define MBSSID_TX_VAP_AID_OFFSET64 64
#define MBSSID_NON_TX_VAP_AID_OFFSET64 64

wv_mac80211_t *       __MTLK_IFUNC wv_ieee80211_init (struct device *dev, void *radio_ctx, mtlk_handle_t hw_handle);
mtlk_error_t          __MTLK_IFUNC wv_ieee80211_setup_register(struct device *dev, wave_radio_t *radio, wv_mac80211_t *mac80211, mtlk_handle_t hw_handle);
void                  __MTLK_IFUNC wv_ieee80211_unregister_if_needed (wv_mac80211_t *mac80211);
void                  __MTLK_IFUNC wv_ieee80211_cleanup(wv_mac80211_t *mac80211);
int                   __MTLK_IFUNC wv_ieee80211_mngmn_frame_rx(mtlk_core_t *nic, const u8 *data, uint32 size, unsigned freq, int sig_dbm, int snr_db, int noise_dbm,
                                                               unsigned subtype, uint16 rate_idx, u8 phy_mode, uint8 pmf_flags, u8 nss);
void                  __MTLK_IFUNC wv_ieee80211_tx_status(struct ieee80211_hw *hw, struct sk_buff *skb, uint8 status);
struct ieee80211_hw * __MTLK_IFUNC wv_ieee80211_hw_get(wv_mac80211_t *mac80211);

void *                __MTLK_IFUNC wv_mac80211_wiphy_get(wv_mac80211_t *mac80211);
void                  __MTLK_IFUNC wv_ieee80211_report_low_ack(struct ieee80211_sta *sta, uint32 num_of_packets);

wave_radio_t *        __MTLK_IFUNC wv_ieee80211_hw_radio_get (struct ieee80211_hw *hw);

BOOL                  __MTLK_IFUNC wv_mac80211_has_sta_connections(wv_mac80211_t *mac80211);
BOOL                  __MTLK_IFUNC wv_mac80211_get_sta_vifs_exist(wv_mac80211_t *mac80211);
mtlk_error_t          __MTLK_IFUNC wv_mac80211_NDP_send_to_all_APs(wv_mac80211_t *mac80211, mtlk_nbuf_t *nbuf_ndp, BOOL power_mgmt_on, BOOL wait_for_ack);

void                  __MTLK_IFUNC wv_ieee80211_scan_completed(struct ieee80211_hw *hw, BOOL aborted);
void                  __MTLK_IFUNC wv_ieee80211_scan_roc_completed(struct ieee80211_hw *hw);
void                  __MTLK_IFUNC wv_ieee80211_scan_roc_ready_on_chan(struct ieee80211_hw *hw);
mtlk_df_user_t*       __MTLK_IFUNC wv_ieee80211_ndev_to_dfuser(struct net_device *ndev);
mtlk_df_user_t*       __MTLK_IFUNC wv_ieee80211_vif_to_dfuser (struct ieee80211_vif *vif);
struct nic*           __MTLK_IFUNC wv_ieee80211_vif_to_core (struct ieee80211_vif *vif);

void                  __MTLK_IFUNC wv_mac80211_recover_sta_vifs(wv_mac80211_t *mac80211);

struct ieee80211_hw *  __MTLK_IFUNC wave_vap_ieee80211_hw_get (mtlk_vap_handle_t vap_handle);
void                   __MTLK_IFUNC wv_ieee80211_free_txskb(mtlk_vap_handle_t vap_handle, struct sk_buff *skb);
struct ieee80211_sta * __MTLK_IFUNC wave_vap_get_sta (mtlk_vap_handle_t vap_handle, u8 * mac);
struct ieee80211_sta * __MTLK_IFUNC wave_vap_get_sta_by_aid (mtlk_vap_handle_t vap_handle, u16 aid);
void                   __MTLK_IFUNC wave_vap_increment_ndp_counter(mtlk_vap_handle_t vap_handle);

void                  __MTLK_IFUNC wv_ieee80211_radar_detected(wv_mac80211_t *mac80211, struct mtlk_chan_def *mcd, uint8 rbm);

void                  __MTLK_IFUNC wv_mac80211_iface_set_initialized (wv_mac80211_t *mac80211, u8 vap_index);
BOOL                  __MTLK_IFUNC wv_mac80211_iface_get_is_initialized (wv_mac80211_t *mac80211, u8 vap_index);

struct ieee80211_vif * __MTLK_IFUNC wave_vap_get_vif (mtlk_vap_handle_t vap_handle);
struct ieee80211_vif * __MTLK_IFUNC wv_mac80211_get_vif (wv_mac80211_t *mac80211, u8 vap_index);
mtlk_error_t __MTLK_IFUNC wv_ieee80211_eapol_rx(wv_mac80211_t * mac80211,
                                                struct ieee80211_vif *vif,
                                                void *data, int data_len,
                                                nl80211_band_e band);
void _wave_mac80211_register_vendor_cmds(struct wiphy *wiphy);
void _wave_mac80211_register_vendor_evts(struct wiphy *wiphy);

static __INLINE mtlk_nbuf_t *
wv_ieee80211_vendor_event_alloc(struct wiphy *wiphy, int approxlen, int event_idx)
{
  return cfg80211_vendor_event_alloc(wiphy, NULL, approxlen, event_idx, GFP_ATOMIC);
}
#endif /* _MTLK_MAC80211_H_ */
