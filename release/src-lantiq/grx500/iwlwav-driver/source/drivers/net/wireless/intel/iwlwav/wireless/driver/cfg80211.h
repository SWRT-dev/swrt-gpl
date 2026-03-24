/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef _MTLK_CFG80211_H_
#define _MTLK_CFG80211_H_

#include <net/cfg80211.h>
#include "mtlk_coreui.h"
#include "wds.h"

#if LINUX_VERSION_CODE < KERNEL_VERSION(3,10,0)
#error "minimum Linux kernel version supported by MTLK WLAN driver 3.10.0"
#endif

#define WV_WOW_MAX_PATTERNS       20
#define WV_WOW_MIN_PATTERN_LEN    1
#define WV_WOW_MAX_PATTERN_LEN    128

#define WV_SUPP_NUM_STS 3 /* a possible default to fall back on */

typedef struct _wv_cfg80211_t wv_cfg80211_t;
typedef struct _wave_radio_t wave_radio_t;

int             __MTLK_IFUNC wv_cfg80211_handle_flush_stations(mtlk_vap_handle_t vap_handle, void *data, int data_len);
int             __MTLK_IFUNC wv_cfg80211_handle_get_unconnected_sta(struct wireless_dev *wdev, void *data,int data_len);
void            __MTLK_IFUNC wv_cfg80211_class3_error_notify (struct wireless_dev *wdev, const u8 *addr);

unsigned        __MTLK_IFUNC wv_cfg80211_regulatory_limit_get (struct wireless_dev *wdev,
                                                               unsigned lower_freq, unsigned upper_freq);

void            __MTLK_IFUNC wv_cfg80211_peer_connect (struct net_device *ndev, const IEEE_ADDR *mac_addr, wds_beacon_data_t *beacon_data);
void            __MTLK_IFUNC wv_cfg80211_peer_disconnect (struct net_device *ndev, const IEEE_ADDR *mac_addr);

bool wv_cfg80211_get_chans_dfs_required(struct wiphy *wiphy, u32 center_freq, u32 bandwidth);
uint8 __MTLK_IFUNC wv_cfg80211_get_chans_dfs_bitmap (struct wiphy *wiphy, u32 center_freq, u32 bandwidth);
void wv_cfg80211_debug_change_kernel_dfs_state(struct wiphy *wiphy, enum nl80211_dfs_state state);
int wv_cfg80211_radar_event_if_sta_connected(struct wireless_dev *wdev, struct mtlk_chan_def *mcd);
void wv_cfg80211_debug_nop(struct wiphy *wiphy, mtlk_vap_handle_t vap_handle, struct cfg80211_chan_def *chandef, uint8 radar_bitmap);
void wv_cfg80211_ch_switch_notify(struct net_device *ndev, const struct mtlk_chan_def *mcd,  bool sta_exist);
void wv_fill_ht_capab(struct mtlk_scan_request *request, mtlk_hw_band_e band, u8 *buf, size_t len);

void wv_cfg80211_set_chan_dfs_state(struct wiphy *wiphy, uint32 center_freq, uint32 bandwidth, enum nl80211_dfs_state dfs_state);

void wv_cfg80211_set_scan_expire_time(struct wireless_dev *wdev, uint32 time);
uint32 wv_cfg80211_get_scan_expire_time(struct wireless_dev *wdev);

void wv_cfg80211_set_out_of_scan_caching(struct wireless_dev *wdev, int is_active);
int wv_cfg80211_get_out_of_scan_caching(struct wireless_dev *wdev);

void wv_cfg80211_set_allow_scan_during_cac(struct wireless_dev *wdev, int allow);
int wv_cfg80211_get_allow_scan_during_cac(struct wireless_dev *wdev);
void wv_cfg80211_set_scan_pause(struct wireless_dev *wdev, BOOL is_pause);

BOOL _mtlk_is_rate_80211b(uint32 bitrate);
BOOL _mtlk_is_rate_80211ag(uint32 bitrate);
void __MTLK_IFUNC
wv_cfg80211_update_wiphy_ant_mask(struct wiphy *wiphy, uint8 available_ant_mask);

int __MTLK_IFUNC
wv_cfg80211_vendor_cmd_alloc_and_reply (struct wiphy *wiphy, const void *data,
                                        int data_len);

static __INLINE mtlk_nbuf_t *
wv_cfg80211_vendor_cmd_alloc_reply_skb(struct wiphy *wiphy, int approxlen)
{
  return cfg80211_vendor_cmd_alloc_reply_skb(wiphy, approxlen);
}

static __INLINE int
wv_cfg80211_vendor_cmd_reply(mtlk_nbuf_t *nbuf)
{
  return cfg80211_vendor_cmd_reply(nbuf);
}

static __INLINE mtlk_nbuf_t *
wv_cfg80211_vendor_event_alloc(struct wireless_dev *wdev, int approxlen, int event_idx)
{
  return cfg80211_vendor_event_alloc(wdev->wiphy, wdev, approxlen, event_idx, GFP_ATOMIC);
}

static __INLINE void
wv_cfg80211_vendor_event(mtlk_nbuf_t *nbuf)
{
  cfg80211_vendor_event(nbuf, GFP_ATOMIC);
}

static __INLINE
void wv_cfg80211_scan_done(struct mtlk_scan_request *request, bool aborted)
{
  struct cfg80211_scan_info info = {
                        .aborted = aborted,
  };
  struct cfg80211_scan_request *sr =
    (struct cfg80211_scan_request *)request->saved_request;

  /* request->saved_request is freed by the kernel, need to set driver pointer
     to NULL as well */
  request->saved_request = NULL;
  cfg80211_scan_done(sr, &info);
}

#ifdef CPTCFG_IWLWAV_DBG_DRV_UTEST

void __MTLK_IFUNC wv_cfg80211_utest(struct net_device *ndev);

#endif /* CPTCFG_IWLWAV_DBG_DRV_UTEST */

BOOL __MTLK_IFUNC wv_cfg80211_dfs_validate_radar_bitmap(struct wiphy *wiphy, u32 center_freq,
                                                        u32 bandwidth, u8 *rbm);
bool wv_cfg80211_radar_overlaps_chandef(struct wiphy *wiphy, u32 center_freq,
                                        u32 bandwidth, u8 rbm,
                                        struct mtlk_chan_def *cd);
bool wv_cfg80211_chandef_dfs_required(struct wiphy *wiphy,
                                  const struct cfg80211_chan_def *chandef);

#endif /* _MTLK_CFG80211_H_ */
