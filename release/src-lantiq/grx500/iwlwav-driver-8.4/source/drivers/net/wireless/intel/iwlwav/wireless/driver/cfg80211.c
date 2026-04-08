/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#include "mtlkinc.h"
#include <net/iw_handler.h>
#include <net/cfg80211.h>
#ifndef CPTCFG_IWLWAV_X86_HOST_PC
#include <../net/wireless/core.h>
#include <../net/wireless/nl80211.h>
#include <../net/wireless/reg.h>
#endif

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
#include "hw_mmb.h"
#include "wds.h"
#include "mtlk_df_nbuf.h"
#include "mtlk_param_db.h"
#include "core_config.h"
#include "wave_radio.h"
#include "vendor_cmds.h"

#define LOG_LOCAL_GID   GID_CFG80211
#define LOG_LOCAL_FID   0

#define API_WRAPPER(api_function) api_function ## _recovery_retry_wrapper

extern const struct ieee80211_regdomain *get_cfg80211_regdom(void);
extern const struct ieee80211_regdomain *get_wiphy_regdom(struct wiphy *wiphy);

struct workqueue_struct *cfg80211_get_cfg80211_wq(void);
void cfg80211_set_scan_expire_time(struct wiphy *wiphy, uint32 time);
uint32 cfg80211_get_scan_expire_time(struct wiphy *wiphy);

struct _wv_cfg80211_t
{
  BOOL registered;
  /* mtlk_vap_manager_t *vap_manager;*/
  wave_radio_t *radio;
  struct wiphy *wiphy;
};

/* Send flush stations request to Hostapd. */
int __MTLK_IFUNC
wv_cfg80211_handle_flush_stations(
  mtlk_vap_handle_t vap_handle,
  void *data, int data_len)
{
  mtlk_df_t *df               = mtlk_vap_get_df(vap_handle);
  mtlk_df_user_t *df_user     = mtlk_df_get_user(df);
  struct wireless_dev *wdev   = mtlk_df_user_get_wdev(df_user);
  mtlk_nbuf_t *evt_nbuf;

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
                                            LTQ_NL80211_VENDOR_EVENT_FLUSH_STATIONS);
  if (!evt_nbuf)
  {
    ELOG_D("Malloc event fail. data_len = %d", data_len);
    return MTLK_ERR_NO_MEM;
  }

  if (data != NULL && data_len > 0) {
    uint8 *cp = mtlk_df_nbuf_put(evt_nbuf, data_len);
    wave_memcpy(cp, data_len, data, data_len);
  }
  wv_cfg80211_vendor_event(evt_nbuf);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
wv_cfg80211_handle_get_unconnected_sta (struct wireless_dev *wdev, void *data, int data_len)
{
  mtlk_nbuf_t *evt_nbuf;
  uint8 *cp;

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
                                            LTQ_NL80211_VENDOR_EVENT_UNCONNECTED_STA);
  if (!evt_nbuf)
  {
    ELOG_D("Malloc event fail. data_len = %d", data_len);
    return MTLK_ERR_NO_MEM;
  }

  cp = mtlk_df_nbuf_put(evt_nbuf, data_len);
  wave_memcpy(cp, data_len, data, data_len);
  wv_cfg80211_vendor_event(evt_nbuf);

  return MTLK_ERR_OK;
}

/**
  FIXCFG80211: description
*/


void __MTLK_IFUNC
wv_cfg80211_class3_error_notify (struct wireless_dev *wdev, const u8 *addr)
{
  cfg80211_rx_spurious_frame(wdev->netdev, addr, GFP_ATOMIC);
}

static int
wv_cfg80211_send_peer_connect (struct wireless_dev *wdev, void *data, int data_len)
{
  uint8 *cp;
  mtlk_nbuf_t *evt_nbuf;
#if IWLWAV_RTLOG_MAX_DLEVEL >= 3
  mtlk_df_user_t *df_user;
  mtlk_vap_handle_t vap_handle;
#endif

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
                                            LTQ_NL80211_VENDOR_EVENT_WDS_CONNECT);
  if (!evt_nbuf)
  {
    return MTLK_ERR_NO_MEM;
  }

  cp = mtlk_df_nbuf_put(evt_nbuf, data_len);
  wave_memcpy(cp, data_len, data, data_len);

#if IWLWAV_RTLOG_MAX_DLEVEL >= 3
  df_user = mtlk_df_user_from_wdev(wdev);
  if (df_user) {
    vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
    ILOG3_D("CID-%04x: WDS peer connect", mtlk_vap_get_oid(vap_handle));
  }
#endif

  mtlk_dump(3, evt_nbuf->data, evt_nbuf->len, "WDS peer connect vendor frame");
  wv_cfg80211_vendor_event(evt_nbuf);

  return MTLK_ERR_OK;
}

static int
wv_cfg80211_send_peer_disconnect (struct wireless_dev *wdev, void *data, int data_len)
{
  uint8 *cp;
  mtlk_nbuf_t *evt_nbuf;
#if IWLWAV_RTLOG_MAX_DLEVEL >= 3
  mtlk_df_user_t *df_user;
  mtlk_vap_handle_t vap_handle;
#endif

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
                                            LTQ_NL80211_VENDOR_EVENT_WDS_DISCONNECT);
  if (!evt_nbuf)
  {
    return MTLK_ERR_NO_MEM;
  }

  cp = mtlk_df_nbuf_put(evt_nbuf, data_len);
  wave_memcpy(cp, data_len, data, data_len);

#if IWLWAV_RTLOG_MAX_DLEVEL >= 3
  df_user = mtlk_df_user_from_wdev(wdev);
  if (df_user) {
    vap_handle = mtlk_df_get_vap_handle(mtlk_df_user_get_df(df_user));
    ILOG3_D("CID-%04x: WDS peer disconnect", mtlk_vap_get_oid(vap_handle));
  }
#endif

  mtlk_dump(3, evt_nbuf->data, evt_nbuf->len, "WDS peer disconnect vendor frame");
  wv_cfg80211_vendor_event(evt_nbuf);

  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
wv_cfg80211_peer_connect (struct net_device *ndev, const IEEE_ADDR *mac_addr, wds_beacon_data_t *beacon_data)
{
  struct intel_vendor_wds_sta_info *st_info;
  int st_info_size = sizeof(struct intel_vendor_wds_sta_info) + beacon_data->ie_data_len;
  st_info = (struct intel_vendor_wds_sta_info *)mtlk_osal_mem_alloc(st_info_size, MTLK_MEM_TAG_CFG80211);

  if (NULL == st_info) {
    ELOG_V("Cannot allocate memory for WDS peer connect");
    return;
  }

  memset(st_info, 0, st_info_size);

  st_info->assoc_req_ies_len = beacon_data->ie_data_len;
  st_info->dtim_period       = beacon_data->dtim_period;
  st_info->beacon_interval   = beacon_data->beacon_interval;
  st_info->protection        = beacon_data->protection;
  st_info->short_preamble    = beacon_data->short_preamble;
  st_info->short_slot_time   = beacon_data->short_slot_time;
  st_info->max_rssi          = DBM_TO_MBM(beacon_data->max_rssi);
  st_info->sta_flags_mask    = BIT(NL80211_STA_FLAG_AUTHORIZED)     |
                               BIT(NL80211_STA_FLAG_SHORT_PREAMBLE) |
                               BIT(NL80211_STA_FLAG_AUTHENTICATED)  |
                               BIT(NL80211_STA_FLAG_ASSOCIATED);
  st_info->sta_flags_set     = (1 << NL80211_STA_FLAG_AUTHORIZED)                                         |
                               ((beacon_data->short_preamble ? 1 : 0) << NL80211_STA_FLAG_SHORT_PREAMBLE) |
                               (1 << NL80211_STA_FLAG_AUTHENTICATED)                                      |
                               (1 << NL80211_STA_FLAG_ASSOCIATED);
  st_info->mac_addr = *mac_addr;
  wave_memcpy(&st_info->assoc_req_ies, beacon_data->ie_data_len, beacon_data->ie_data, beacon_data->ie_data_len);
  wave_strcopy(st_info->ifname, ndev->name, sizeof(st_info->ifname));

  wv_cfg80211_send_peer_connect(ndev->ieee80211_ptr, st_info, st_info_size);
  mtlk_osal_mem_free(st_info);
}

void __MTLK_IFUNC  wv_cfg80211_peer_disconnect (struct net_device *ndev, const IEEE_ADDR *mac_addr)
{
  struct intel_vendor_wds_sta_info st_info;
  memset(&st_info, 0, sizeof(st_info));

  ILOG1_Y("Peer AP %Y", mac_addr);

  st_info.mac_addr = *mac_addr;
  wave_strcopy(st_info.ifname, ndev->name, sizeof(st_info.ifname));

  wv_cfg80211_send_peer_disconnect(ndev->ieee80211_ptr, &st_info, sizeof(st_info));
}

#define WAVE_CHAN_IS_5G(freq) (((freq) / 5000u) != 0)
#define WAVE_REGULATORY_POWER_MAX_DBM 0xFF

/*
 * Get regulatory power limit with a support of
 * regulatory rules conjuncted with AUTO_BW flag
 */
unsigned __MTLK_IFUNC
wv_cfg80211_regulatory_limit_get (struct wireless_dev *wdev,
                                  unsigned lower_freq, unsigned upper_freq)
{
  unsigned max_power = WAVE_REGULATORY_POWER_MAX_DBM;
  unsigned freq, freq_step = 5; /* frequency step between used channels for 2.4 GHz */

  /* adjust frequency step for 5.2 GHz and 6 GHz */
  if (WAVE_CHAN_IS_5G(lower_freq) || WAVE_FREQ_IS_6G(lower_freq))
    freq_step = 20;

  ILOG2_DDD("lower_freq %u upper_freq %u freq_step %u", lower_freq, upper_freq, freq_step);

  for (freq = lower_freq; freq <= upper_freq; freq += freq_step) {
    struct ieee80211_channel *channel = ieee80211_get_channel(wdev->wiphy, freq);

    if (NULL == channel) {
      WLOG_D("No ieee80211_channel for freq %d", freq);
      continue;
    }

    if (channel->flags & IEEE80211_CHAN_DISABLED) {
      ILOG2_D("Disabled channel %d", freq);
      continue;
    }

    ILOG2_DD("freq %d max_reg_power %d", channel->center_freq, channel->max_reg_power);

    /* find minimal allowed by regulatory */
    if (channel->max_reg_power < max_power)
      max_power = channel->max_reg_power;
  }

  /* check if nothing was found */
  if (max_power == WAVE_REGULATORY_POWER_MAX_DBM)
    max_power = 0;

  ILOG1_D("max_power %d dBm", max_power);

  return DBM_TO_POWER(max_power);
}

#define HT_CAPAB_LEN 26

void wv_fill_ht_capab(struct mtlk_scan_request *request, mtlk_hw_band_e band, u8 *buf, size_t len)
{
  struct wiphy *wiphy = (struct wiphy *) request->wiphy;
  int nlband = mtlkband2nlband(band);
  int i = 0;
  struct ieee80211_sta_ht_cap *ht_cap;
  uint16 cap_info;

  MTLK_ASSERT(buf && len >= HT_CAPAB_LEN && wiphy->bands[nlband]);

  ht_cap = &wiphy->bands[nlband]->ht_cap;

  cap_info = HOST_TO_WLAN16(ht_cap->cap);
  wave_memcpy(buf + i, len - i, &cap_info, sizeof(cap_info));
  i += sizeof(cap_info);

  buf[i++] = ht_cap->ampdu_factor | (ht_cap->ampdu_density << IEEE80211_HT_AMPDU_PARM_DENSITY_SHIFT);

  wave_memcpy(buf + i, len - i, &ht_cap->mcs, sizeof(ht_cap->mcs));
  i += sizeof(ht_cap->mcs); /* 16 */

  memset(buf + i, 0, HT_CAPAB_LEN - i); /* 7: extended capabilities, transmit beam forming, antenna selection */
}

void wv_cfg80211_debug_change_kernel_dfs_state(struct wiphy *wiphy, enum nl80211_dfs_state state)
{
  struct ieee80211_channel *c;
  uint32 cur_freq, low_freq, high_freq;
  low_freq  = ieee80211_channel_to_frequency(52, NL80211_BAND_5GHZ);   /* 52nd channel's frequency */
  high_freq = ieee80211_channel_to_frequency(144, NL80211_BAND_5GHZ);  /* 144th channel's frequency */

  ILOG1_S("Setting %s state to all DFS required channels",
          state == NL80211_DFS_USABLE ? "NL80211_DFS_USABLE" :
          (state == NL80211_DFS_AVAILABLE ? "NL80211_DFS_AVAILABLE" : "NL80211_DFS_UNAVAILABLE"));

  for (cur_freq = low_freq; cur_freq <= high_freq; cur_freq += 20) {
    c = ieee80211_get_channel(wiphy, cur_freq);

    if (!c || !(c->flags & IEEE80211_CHAN_RADAR))
      continue;

    c->dfs_state = state;
  }
}

static bool wv_cfg80211_reg_dfs_domain_same(struct wiphy *wiphy1, struct wiphy *wiphy2)
{
 const struct ieee80211_regdomain *wiphy1_regd = NULL;
 const struct ieee80211_regdomain *wiphy2_regd = NULL;
 const struct ieee80211_regdomain *cfg80211_regd = NULL;

 cfg80211_regd = get_cfg80211_regdom();
 if (cfg80211_regd == NULL)
   return FALSE;
 wiphy1_regd = get_wiphy_regdom(wiphy1);
 if (!wiphy1_regd)
   wiphy1_regd = cfg80211_regd;

 wiphy2_regd = get_wiphy_regdom(wiphy2);
 if (!wiphy2_regd)
   wiphy2_regd = cfg80211_regd;

 return wiphy1_regd->dfs_region == wiphy2_regd->dfs_region;
}

bool wv_cfg80211_get_chans_dfs_required(struct wiphy *wiphy,
                                            u32 center_freq,
                                            u32 bandwidth)
{
  struct ieee80211_channel *c;
  u32 freq;

  for (freq = center_freq - bandwidth/2 + 10;
    freq <= center_freq + bandwidth/2 - 10;
    freq += 20) {
    c = ieee80211_get_channel(wiphy, freq);
    if (!c)
      continue;

    if (c->flags & IEEE80211_CHAN_RADAR)
      return TRUE;
  }
  return FALSE;
}

/* In case of RBM bits set for non-DFS channels return FALSE */
bool wv_cfg80211_get_chans_dfs_bitmap_valid(struct wiphy *wiphy, u32 center_freq, u32 bandwidth, u8 rbm)
{
  struct ieee80211_channel *c;
  u32 freq, mask;
  bool chan_found = FALSE;

  for (freq = center_freq - bandwidth / 2 + 10, mask = 1;
       freq <= center_freq + bandwidth / 2 - 10;
       freq += 20, mask <<= 1)
  {
    if (rbm & mask) {
      c = ieee80211_get_channel(wiphy, freq);
      if (!c) continue;

      if (!(c->flags & IEEE80211_CHAN_RADAR))
        return FALSE;

      chan_found = TRUE;
    }
  }

  return chan_found;
}

/* Check if a channel is included in the radar detected channel/bitmap */
bool wv_cfg80211_radar_overlaps_chandef(struct wiphy *wiphy, u32 center_freq1,
                                        u32 bandwidth, u8 rbm,
                                        struct mtlk_chan_def* cd)
{
  struct ieee80211_channel *radar_c, *c;
  u32 freq, radar_freq, mask;
  u32 low_freq =
        freq2lowfreq(cd->center_freq1, cd->width);
  u32 high_freq =
        freq2highfreq(cd->center_freq1, cd->width);

  if (!rbm) rbm = MTLK_MAX_RBM;

  for (radar_freq = center_freq1 - bandwidth / 2 + 10, mask = 1;
       radar_freq <= center_freq1 + bandwidth / 2 - 10;
       radar_freq += 20, mask <<= 1) {
    if (rbm & mask) {
      radar_c = ieee80211_get_channel(wiphy, radar_freq);

      if (!radar_c || !(radar_c->flags & IEEE80211_CHAN_RADAR))
        continue;

      for (freq = low_freq; freq <= high_freq; freq += 20) {
        c = ieee80211_get_channel(wiphy, freq);

        if (!c || !(c->flags & IEEE80211_CHAN_RADAR))
          continue;

        if (radar_freq == freq)
          return true;

      }
    }
  }
  return false;
}

/* In case of invalid chandef, return FALSE */
bool wv_cfg80211_chandef_dfs_required(struct wiphy *wiphy,
                                  const struct cfg80211_chan_def *chandef)
{
  int width;
  int r;

  if (!cfg80211_chandef_valid(chandef)) {
    ELOG_V("Invalid chandef");
    return FALSE;
  }

  width = _wave_radio_chandef_width_get(chandef);
  if (width < 0)
    return FALSE;

  r = wv_cfg80211_get_chans_dfs_required(wiphy, chandef->center_freq1,
    width);
  if (r)
    return r;

  if (!chandef->center_freq2)
    return FALSE;

  return wv_cfg80211_get_chans_dfs_required(wiphy, chandef->center_freq2,
    width);
}


/* Propagate DFS state of our interface to other interfaces. Debug features like
 * setting CAC time and Non Occupancy Period are not supported, because we don't
 * know weather we are propagating event to device handled by mtlk driver. */
void wv_cfg80211_regulatory_propagate_dfs_state(struct wireless_dev *wdev,
           struct cfg80211_chan_def *chandef,
           enum nl80211_dfs_state dfs_state,
           enum nl80211_radar_event event,
           uint8 rbm)
{
  struct cfg80211_registered_device *rdev;
  struct wireless_dev *wdev2;
  uint32 width;
  BOOL devices_exist = FALSE;
  BOOL update_dfs_wq;

  ASSERT_RTNL();

  if (!cfg80211_chandef_valid(chandef)) {
    ELOG_S("%s: Invalid chandef", wdev->netdev->name);
    return;
  }
  if (!wv_cfg80211_chandef_dfs_required(wdev->wiphy, chandef)) {
    ILOG1_S("%s: No DFS channels", wdev->netdev->name);
    return;
  }

  list_for_each_entry(rdev, &cfg80211_rdev_list, list) {
    update_dfs_wq = FALSE;
    if (wdev->wiphy == &rdev->wiphy)
      continue;

    if (!wv_cfg80211_reg_dfs_domain_same(wdev->wiphy, &rdev->wiphy))
      continue;

    if (!ieee80211_get_channel(&rdev->wiphy,
      chandef->chan->center_freq))
      continue;

    width = wave_radio_chandef_width_get(chandef);
    list_for_each_entry(wdev2, &rdev->wiphy.wdev_list, list) {
      struct ieee80211_channel *wdev2_chan = wdev2->chandef.chan;

      if (wdev2->iftype != NL80211_IFTYPE_AP || wdev2_chan == NULL)
        continue;

      devices_exist = update_dfs_wq = TRUE;

      /* If radar detected on our channel */
      if ((event == NL80211_RADAR_DETECTED) &&
          (chandef->center_freq1 - width / 2 < wdev2_chan->center_freq &&
           chandef->center_freq1 + width / 2 > wdev2_chan->center_freq))
        wdev2->cac_started = false;
    }

    if (update_dfs_wq) {
      cancel_delayed_work(&rdev->dfs_update_channels_wk);
      queue_delayed_work(cfg80211_get_cfg80211_wq(), &rdev->dfs_update_channels_wk, 0);
    }

    /* no state change on CAC aborted */
    if (update_dfs_wq && (event != NL80211_RADAR_CAC_ABORTED)) {
      if (rbm && rbm != 0xff)
        cfg80211_set_dfs_state_bit_map(&rdev->wiphy, chandef, rbm, dfs_state);
      else
        cfg80211_set_dfs_state(&rdev->wiphy, chandef, dfs_state);
    }
  }
  /* Only CAC finished event needs to be propagated, the other events are
   * already global. */
  if (devices_exist && event == NL80211_RADAR_CAC_FINISHED)
    nl80211_radar_notify(wiphy_to_rdev(wdev->wiphy), chandef, event, NULL, 0, GFP_KERNEL);
}

#ifdef CONFIG_WAVE_DEBUG
#ifndef CPTCFG_IWLWAV_X86_HOST_PC
void wv_cfg80211_debug_nop(struct wiphy *wiphy, mtlk_vap_handle_t vap_handle,
                           struct cfg80211_chan_def *chandef)
{
  struct ieee80211_channel *c;
  uint32 low_freq, high_freq, cur_freq;
  uint32 non_ocup_prd_ms;
  struct cfg80211_registered_device *rdev;
  mtlk_core_t *core;
  mtlk_scan_support_t *scan_support;

  core = mtlk_vap_get_core(vap_handle);
  scan_support = mtlk_core_get_scan_support(core);

  MTLK_ASSERT(scan_support);
  MTLK_ASSERT(scan_support->dfs_debug_params.nop);

  non_ocup_prd_ms = scan_support->dfs_debug_params.nop * MTLK_OSAL_MSEC_IN_MIN;
  low_freq = freq2lowfreq(chandef->center_freq1, nlcw2mtlkcw(chandef->width));
  high_freq = freq2highfreq(chandef->center_freq1, nlcw2mtlkcw(chandef->width));
  rdev = wiphy_to_rdev(wiphy);

  ILOG1_D("Setting debug non occupancy period for %d seconds",
          scan_support->dfs_debug_params.nop * MTLK_OSAL_SEC_IN_MIN);

  for (cur_freq = low_freq; cur_freq <= high_freq; cur_freq += 20) {
    c = ieee80211_get_channel(wiphy, cur_freq);

    if (!c || !(c->flags & IEEE80211_CHAN_RADAR) || (c->dfs_state != NL80211_DFS_UNAVAILABLE))
      continue;

    if (non_ocup_prd_ms < IEEE80211_DFS_MIN_NOP_TIME_MS)
      c->dfs_state_entered -= msecs_to_jiffies(IEEE80211_DFS_MIN_NOP_TIME_MS - non_ocup_prd_ms);
    else
      c->dfs_state_entered += msecs_to_jiffies(non_ocup_prd_ms - IEEE80211_DFS_MIN_NOP_TIME_MS);
  }

  cancel_delayed_work(&rdev->dfs_update_channels_wk);
  queue_delayed_work(cfg80211_get_cfg80211_wq(), &rdev->dfs_update_channels_wk, 0);
}
#endif
#endif

static int
wv_cfg80211_handle_radar_event_if_sta_exist(struct wireless_dev *wdev, void *data,
  int data_len)
{
  mtlk_nbuf_t *evt_nbuf;
  uint8 *cp;

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
    LTQ_NL80211_VENDOR_EVENT_RADAR_DETECTED);
  if (!evt_nbuf)
  {
    ELOG_D("Malloc event fail. data_len = %d", data_len);
    return MTLK_ERR_NO_MEM;
  }

  cp = mtlk_df_nbuf_put(evt_nbuf, data_len);
  wave_memcpy(cp, data_len, data, data_len);
  wv_cfg80211_vendor_event(evt_nbuf);

  return MTLK_ERR_OK;
}

int wv_cfg80211_radar_event_if_sta_connected(struct wireless_dev *wdev,
  struct mtlk_chan_def *mcd)
{
  struct cfg80211_chan_def chandef;
  struct cfg80211_registered_device *rdev = wiphy_to_rdev(wdev->wiphy);
  unsigned long timeout;
  struct intel_vendor_radar radar;

  chandef.chan = ieee80211_get_channel(wdev->wiphy, mcd->chan.center_freq);
  chandef.width = mtlkcw2nlcw(mcd->width, mcd->is_noht);
  chandef.center_freq1 = mcd->center_freq1;
  chandef.center_freq2 = mcd->center_freq2;

  /* mark channel as UNAVAILABLE */
  cfg80211_set_dfs_state(wdev->wiphy, &chandef, NL80211_DFS_UNAVAILABLE);
  /* schedule workqueue */
  timeout = msecs_to_jiffies(IEEE80211_DFS_MIN_NOP_TIME_MS);
  queue_delayed_work(cfg80211_get_cfg80211_wq(), &rdev->dfs_update_channels_wk,
    timeout);

  /* notify hostapd with vendor event */
  memset(&radar, 0, sizeof(radar));
  radar.center_freq = mcd->chan.center_freq;
  radar.width = chandef.width;
  radar.center_freq1 = mcd->center_freq1;
  radar.center_freq2 = mcd->center_freq2;
  radar.radar_bit_map = 0xFF;
  return wv_cfg80211_handle_radar_event_if_sta_exist(wdev, &radar,
    sizeof(radar));
}

void wv_cfg80211_ch_switch_notify(struct net_device *ndev, const struct mtlk_chan_def *mcd, bool sta_exist)
{
  struct cfg80211_chan_def chandef;
  struct wireless_dev *wdev = ndev->ieee80211_ptr;
  mtlk_df_user_t *df_user = mtlk_df_user_from_ndev(ndev);

  /* df_user shouldn't be NULL in a normal situation */
  MTLK_CHECK_DF_USER_NORES(df_user);

  chandef.chan = ieee80211_get_channel(wdev->wiphy, mcd->chan.center_freq);
  chandef.width = mtlkcw2nlcw(mcd->width, mcd->is_noht);
  chandef.center_freq1 = mcd->center_freq1;
  chandef.center_freq2 = mcd->center_freq2;
  chandef.sb_dfs_bw = mcd->sb_dfs.sb_dfs_bw;

  if (sta_exist) {
    cfg80211_set_dfs_state(wdev->wiphy, &chandef, NL80211_DFS_AVAILABLE);
    mtlk_df_user_add_propagate_event(df_user, &chandef, NL80211_DFS_AVAILABLE, NL80211_RADAR_CAC_FINISHED, 0, 0);
    mtlk_df_user_schedule_prop_dfs_wq(df_user);
  }

  cfg80211_ch_switch_notify(ndev, &chandef);
}

#ifdef CONFIG_WAVE_DEBUG
void wv_cfg80211_set_scan_expire_time(struct wireless_dev *wdev, uint32 time)
{
  MTLK_ASSERT(wdev);
  MTLK_ASSERT(wdev->wiphy);

  cfg80211_set_scan_expire_time(wdev->wiphy, time * HZ);
}

uint32 wv_cfg80211_get_scan_expire_time(struct wireless_dev *wdev)
{
  MTLK_ASSERT(wdev);
  MTLK_ASSERT(wdev->wiphy);

  return cfg80211_get_scan_expire_time(wdev->wiphy) / HZ;
}
#endif

void wv_cfg80211_set_out_of_scan_caching(struct wireless_dev *wdev, int is_active)
{
  MTLK_ASSERT(wdev);
  MTLK_ASSERT(wdev->wiphy);

  wdev->wiphy->out_of_scan_caching = !!is_active;
}

int wv_cfg80211_get_out_of_scan_caching(struct wireless_dev *wdev)
{
  MTLK_ASSERT(wdev);
  MTLK_ASSERT(wdev->wiphy);

  return wdev->wiphy->out_of_scan_caching;
}

void wv_cfg80211_set_allow_scan_during_cac(struct wireless_dev *wdev, int allow)
{
  MTLK_ASSERT(wdev);
  MTLK_ASSERT(wdev->wiphy);

  wdev->wiphy->allow_scan_during_cac = !!allow;
}

int wv_cfg80211_get_allow_scan_during_cac(struct wireless_dev *wdev)
{
  MTLK_ASSERT(wdev);
  MTLK_ASSERT(wdev->wiphy);

  return wdev->wiphy->allow_scan_during_cac;
}

void __MTLK_IFUNC
wv_cfg80211_update_wiphy_ant_mask(struct wiphy *wiphy, uint8 available_ant_mask)
{
  wiphy->available_antennas_rx = available_ant_mask;
  wiphy->available_antennas_tx = available_ant_mask;
}
