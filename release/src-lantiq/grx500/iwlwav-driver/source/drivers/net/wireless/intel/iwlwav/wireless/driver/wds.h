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
  * WDS peer manager header file
 *
 */
#ifndef __WDS_H__
#define __WDS_H__

#define HT_CAP_IE_LEN                  26
#define VHT_CAP_IE_LEN                 12

typedef enum {
  WDS_STATE_DISCONNECTED,
  WDS_STATE_CONNECTING,
  WDS_STATE_CONNECTED,
  WDS_STATE_DISCONNECTING,
  WDS_STATE_ERROR
} wds_peer_state_t;

typedef struct _wds_t {
  mtlk_osal_spinlock_t  peers_list_lock;  /* Protect peers list only */
  mtlk_dlist_t          peers_list;       /* Shared access Tasklet(RO)/Serializer(RW) */
  mtlk_vap_handle_t     vap_handle;
  /* "on_beacon_..." fields only for master VAP */
  mtlk_osal_spinlock_t  on_beacon_lock;   /* Protect on_beacon list only */
  mtlk_dlist_t          on_beacon_list;   /* Shared access Tasklet(RW)/Serializer(RW) */

  MTLK_DECLARE_INIT_STATUS;
} wds_t;

typedef struct {
  uint8     *ie_data;
  unsigned  ie_data_len;
  uint32    basic_rate_set;
  uint32    op_rate_set;
  uint16    beacon_interval;
  uint8     channel;
  uint8     second_chan_offset;
  uint8     dtim_period;
  uint8     protection;
  uint8     short_preamble;
  uint8     short_slot_time;
  uint8     is_ht;
  uint8     is_vht;
  uint8     is_he;
  uint8     vap_idx;
  IEEE_ADDR addr;
  uint8     ht_cap_info[HT_CAP_IE_LEN];
  uint8     vht_cap_info[VHT_CAP_IE_LEN];
  struct ieee80211_sta_he_cap he_cap;
  struct ieee80211_he_6ghz_capa he_6ghz_cap;
  unsigned  ht_cap_info_len;
  unsigned  vht_cap_info_len;
  int       max_rssi;
} wds_beacon_data_t;

typedef struct _wds_peer_t {
  mtlk_dlist_entry_t    list_entry;
  mtlk_osal_mutex_t     lock;
  wds_beacon_data_t     beacon_data;
  mtlk_osal_timer_t     reconnect_timer;
  IEEE_ADDR             addr;
  wds_peer_state_t      state;
  wds_t                 *wds;
  mtlk_atomic_t         can_connect;
  mtlk_osal_timestamp_t age_ts;
  mtlk_osal_timestamp_t beacon_age_ts;
  unsigned              numof_reconnect;
  unsigned              numof_reconnect_beacon;
  unsigned              numof_beacons_wrong_chan;
  unsigned              numof_beacons_wrong_bond;
  uint8                 vap_idx;
  uint8                 is_ht;
  uint8                 is_vht;
  uint8                 is_he;
  MTLK_DECLARE_INIT_STATUS;
} wds_peer_t;

typedef struct {
  mtlk_dlist_entry_t    list_entry;
  IEEE_ADDR             addr;
  uint8                 *ie_data;
  unsigned              ie_data_len;
  uint8                 vap_id;
} wds_on_beacon_t;

int __MTLK_IFUNC
wds_init(wds_t *wds, mtlk_vap_handle_t vap_handle);

void __MTLK_IFUNC
wds_cleanup(wds_t *wds);

void __MTLK_IFUNC
wds_on_if_down(wds_t *wds);

void __MTLK_IFUNC
wds_on_if_up(wds_t *wds);

int __MTLK_IFUNC
wds_usr_del_peer_ap(wds_t *wds, const IEEE_ADDR *addr);

int __MTLK_IFUNC
wds_usr_add_peer_ap(wds_t *wds, const IEEE_ADDR *addr);

wds_peer_t *__MTLK_IFUNC
_wds_peer_find (wds_t *wds, const IEEE_ADDR *addr);

void __MTLK_IFUNC
wds_peer_disconnect(wds_t *wds, const IEEE_ADDR *addr);

void __MTLK_IFUNC
wds_peer_connect(wds_t *wds, sta_entry *sta,  const IEEE_ADDR *addr, uint8 is_ht, uint8 is_vht, uint8 is_he);

void __MTLK_IFUNC
wds_enable_abilities(wds_t *wds);

void __MTLK_IFUNC
wds_disable_abilities(wds_t *wds);

int __MTLK_IFUNC
mtlk_wds_get_peer_vect(wds_t *wds, mtlk_clpb_t **ppclpb);

int __MTLK_IFUNC
wds_beacon_process (mtlk_handle_t master_wds_handle, const void *data,  uint32 data_size);

int __MTLK_IFUNC
wds_peer_fsm_timer(mtlk_handle_t wds_handle, const void *peer_addr,  uint32 ieee_addr_size);

BOOL __MTLK_IFUNC
wds_sta_is_peer_ap(wds_t *wds, const IEEE_ADDR *addr);

void __MTLK_IFUNC
wds_switch_off(wds_t *wds);

wds_on_beacon_t * __MTLK_IFUNC
wds_on_beacon_find_remove(wds_t *master_wds, const IEEE_ADDR *addr, BOOL do_remove);

#endif /* __WDS_H__ */
