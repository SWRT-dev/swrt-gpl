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
 * WDS peer manager
 *
 * TODO: Collect all calls of _wds_send_fw_cfg_peer_ap() and put them inside FSM
 *       This should increase readability and simplify further code maintenance
 *
 */
#include "mtlkinc.h"
#include "mtlk_vap_manager.h"
#include "mtlk_coreui.h"
#include "core.h"
#include "core_config.h"
#include "mhi_umi.h"
#include "wds.h"
#include "cfg80211.h"
#include "mtlk_df_user_priv.h"
#include "mtlk_df_priv.h"

#define LOG_LOCAL_GID   GID_WDS
#define LOG_LOCAL_FID   0

#define WDS_RECONNECT_TIMEOUT          1000u

typedef enum {
  WDS_BEACON_IDLE,
  WDS_BEACON_CONNECT,
  WDS_BEACON_DISCONNECT,
} wds_beacon_action_e;

static const mtlk_ability_id_t _wds_abilities[] = {
  WAVE_CORE_REQ_SET_WDS_CFG,
  WAVE_CORE_REQ_GET_WDS_CFG,
  WAVE_CORE_REQ_GET_WDS_PEERAP
};

MTLK_INIT_STEPS_LIST_BEGIN(wds)
  MTLK_INIT_STEPS_LIST_ENTRY(wds, ON_BEACON_LIST)
  MTLK_INIT_STEPS_LIST_ENTRY(wds, ON_BEACON_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(wds, PEERS_LIST)
  MTLK_INIT_STEPS_LIST_ENTRY(wds, PEERS_LIST_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(wds, REG_ABILITIES)
  MTLK_INIT_STEPS_LIST_ENTRY(wds, EN_ABILITIES)
MTLK_INIT_INNER_STEPS_BEGIN(wds)
MTLK_INIT_STEPS_LIST_END(wds);

MTLK_INIT_STEPS_LIST_BEGIN(wds_peer)
  MTLK_INIT_STEPS_LIST_ENTRY(wds_peer, LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(wds_peer, RECONNECT_TMR)
MTLK_INIT_INNER_STEPS_BEGIN(wds_peer)
MTLK_INIT_STEPS_LIST_END(wds_peer);

static uint32 _wds_reconnect_timer_handler (mtlk_osal_timer_t *timer, mtlk_handle_t wds_peer_handle)
{
  wds_peer_t  *peer = (wds_peer_t *)wds_peer_handle;
  if (peer)
    mtlk_osal_atomic_set(&peer->can_connect, 1);
  return 0;
}

static wds_t* _wds_get_master (wds_t *wds)
{
  mtlk_vap_handle_t  master_vap;

  if (MTLK_ERR_OK == mtlk_vap_manager_get_master_vap(mtlk_vap_get_manager(wds->vap_handle), &master_vap)) {
    return &(mtlk_vap_get_core(master_vap)->slow_ctx->wds_mng);
  }
  return NULL;
}

static void _wds_on_beacon_init (wds_t *wds, wds_on_beacon_t *on_beacon, const IEEE_ADDR *addr)
{
  on_beacon->addr = *addr;
  on_beacon->ie_data = NULL;
  on_beacon->ie_data_len = 0;
  on_beacon->vap_id = mtlk_vap_get_id(wds->vap_handle);
}

static int _wds_on_beacon_create_add (wds_t *wds, const IEEE_ADDR *addr)
{
  wds_on_beacon_t *on_beacon = NULL;
  wds_t           *master_wds = _wds_get_master(wds);

  if (__UNLIKELY(NULL == master_wds)) {
    ELOG_D("CID-%04x: WDS Manager not found", mtlk_vap_get_oid(wds->vap_handle));
    return MTLK_ERR_NOT_IN_USE;
  }

  /* prevent adding duplicate entry */
  mtlk_osal_lock_acquire(&master_wds->on_beacon_lock);
  on_beacon = wds_on_beacon_find_remove(master_wds, addr, FALSE);
  mtlk_osal_lock_release(&master_wds->on_beacon_lock);
  if (on_beacon)
    return MTLK_ERR_OK;

  on_beacon = mtlk_osal_mem_alloc(sizeof(wds_on_beacon_t), LQLA_MEM_TAG_WDS);
  if (!on_beacon) {
    WLOG_V("Can't alloc on beacon entry");
    return MTLK_ERR_NO_MEM;
  }

  ILOG3_D("CID-%04x: WDS create on_beacon entry ", mtlk_vap_get_oid(wds->vap_handle));
  memset(on_beacon, 0, sizeof(wds_on_beacon_t));

  _wds_on_beacon_init(wds, on_beacon, addr);

  mtlk_osal_lock_acquire(&master_wds->on_beacon_lock);
  mtlk_dlist_push_front(&master_wds->on_beacon_list, &on_beacon->list_entry);
  mtlk_osal_lock_release(&master_wds->on_beacon_lock);

  return MTLK_ERR_OK;
}

static void _wds_peer_cleanup (wds_peer_t *peer)
{
  ILOG2_DY("CID-%04x: WDS peer cleanup %Y", mtlk_vap_get_oid(peer->wds->vap_handle), peer->addr.au8Addr);

  MTLK_CLEANUP_BEGIN(wds_peer, MTLK_OBJ_PTR(peer));
    MTLK_CLEANUP_STEP(wds_peer, RECONNECT_TMR, MTLK_OBJ_PTR(peer),
                      mtlk_osal_timer_cleanup, (&peer->reconnect_timer));
    MTLK_CLEANUP_STEP(wds_peer, LOCK, MTLK_OBJ_PTR(peer),
                      mtlk_osal_mutex_cleanup, (&peer->lock));
  MTLK_CLEANUP_END(wds_peer, MTLK_OBJ_PTR(peer));
}

static int _wds_peer_init (wds_t *wds, wds_peer_t *peer, const IEEE_ADDR *addr)
{
  peer->addr = *addr;
  peer->wds = wds;
  peer->state = WDS_STATE_DISCONNECTED;
  peer->vap_idx = mtlk_vap_get_id(wds->vap_handle);

  ILOG2_DY("CID-%04x: WDS peer init %Y", mtlk_vap_get_oid(peer->wds->vap_handle), peer->addr.au8Addr);
  MTLK_INIT_TRY(wds_peer, MTLK_OBJ_PTR(peer))
    MTLK_INIT_STEP(wds_peer, LOCK, MTLK_OBJ_PTR(peer),
                   mtlk_osal_mutex_init, (&peer->lock));
    MTLK_INIT_STEP(wds_peer, RECONNECT_TMR, MTLK_OBJ_PTR(peer),
                   mtlk_osal_timer_init,
                   (&peer->reconnect_timer, _wds_reconnect_timer_handler, (mtlk_handle_t)peer));
  MTLK_INIT_FINALLY(wds_peer, MTLK_OBJ_PTR(peer));
  MTLK_INIT_RETURN(wds_peer, MTLK_OBJ_PTR(peer), _wds_peer_cleanup, (peer));

}

static void _wds_peer_remove (wds_peer_t *peer)
{
  wds_t           *wds = peer->wds;
  wds_t           *master_wds;
  wds_on_beacon_t *on_beacon;

  master_wds = _wds_get_master(wds);

  if (__UNLIKELY(NULL == master_wds)) {
    ELOG_D("CID-%04x: WDS Manager not found", mtlk_vap_get_oid(wds->vap_handle));
    return;
  }

  mtlk_osal_lock_acquire(&master_wds->on_beacon_lock);
  on_beacon = wds_on_beacon_find_remove(master_wds, &peer->addr, TRUE);
  mtlk_osal_lock_release(&master_wds->on_beacon_lock);
  if (on_beacon){
    if (NULL != on_beacon->ie_data) {
      mtlk_osal_mem_free(on_beacon->ie_data);
    }
    mtlk_osal_mem_free(on_beacon);
  }

  ILOG2_DY("CID-%04x: WDS peer remove from list %Y", mtlk_vap_get_oid(wds->vap_handle), peer->addr.au8Addr);

  /* Remove peer from WDS DB */
  mtlk_dlist_remove(&wds->peers_list, &peer->list_entry);

  /* cancel timer */
  _mtlk_osal_timer_cancel_sync(&peer->reconnect_timer);
}

static void _wds_peer_delete (wds_peer_t *peer)
{
  /* Delete peer context */
  _wds_peer_cleanup(peer);

  ILOG2_DY("CID-%04x: WDS peer delete ctx %Y", mtlk_vap_get_oid(peer->wds->vap_handle), peer->addr.au8Addr);
  mtlk_osal_mem_free(peer);
}

static int _wds_peer_create_add (wds_t *wds, const IEEE_ADDR *addr)
{
  int         res = MTLK_ERR_UNKNOWN;
  wds_peer_t  *peer = NULL;

  ILOG2_DY("CID-%04x: WDS peer create %Y", mtlk_vap_get_oid(wds->vap_handle), addr->au8Addr);
  peer = mtlk_osal_mem_alloc(sizeof(wds_peer_t), LQLA_MEM_TAG_WDS);

  if (!peer) {
    WLOG_V("Can't alloc peer AP");
    return MTLK_ERR_NO_MEM;
  }

  memset(peer, 0, sizeof(wds_peer_t));

  res = _wds_peer_init(wds, peer, addr);
  if (res != MTLK_ERR_OK) {
    ELOG_D("Can't init peer AP (err#%d)", res);
    goto err_init;
  }

  mtlk_dlist_push_front(&wds->peers_list, &peer->list_entry);

  /* Wait for a beacon to connect if core is in connected state */
  if (NET_STATE_CONNECTED == mtlk_core_get_net_state(mtlk_vap_get_core(wds->vap_handle))) {
    res = _wds_on_beacon_create_add(wds, addr);
    if (MTLK_ERR_OK != res) {
      mtlk_dlist_remove(&wds->peers_list, &peer->list_entry);
      _wds_peer_cleanup(peer);
      goto err_init;
    }
  }

  return MTLK_ERR_OK;

err_init:
  mtlk_osal_mem_free(peer);
  return res;
}

wds_peer_t *__MTLK_IFUNC _wds_peer_find (wds_t *wds, const IEEE_ADDR *addr)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  wds_peer_t *peer;

  mtlk_dlist_foreach(&wds->peers_list, entry, head) {
    peer = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_peer_t, list_entry);
    if (0 == ieee_addr_compare(addr, &peer->addr)) {
      ILOG3_DY("CID-%04x: WDS peer found %Y", mtlk_vap_get_oid(wds->vap_handle), peer->addr.au8Addr);
      return peer;
    }
  }

  ILOG3_DY("CID-%04x: WDS peer NOT found %Y", mtlk_vap_get_oid(wds->vap_handle), addr->au8Addr);
  return NULL;
}

wds_on_beacon_t * __MTLK_IFUNC
wds_on_beacon_find_remove (wds_t *master_wds, const IEEE_ADDR *addr, BOOL do_remove)
{
  mtlk_dlist_entry_t  *entry;
  mtlk_dlist_entry_t  *head;
  wds_on_beacon_t     *on_beacon_excl;

  /* Note: Executed within Tasklet and/or Thread contexts */
  /* The function finds addr in on_beacon list and exclude (depending on the flag) its
     entry if was found. */

  MTLK_ASSERT(mtlk_vap_is_master_ap(master_wds->vap_handle));

  on_beacon_excl = NULL;
  mtlk_dlist_foreach(&master_wds->on_beacon_list, entry, head)
  {
    wds_on_beacon_t *on_beacon;
    on_beacon = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_on_beacon_t, list_entry);
    if (0 == ieee_addr_compare(addr, &on_beacon->addr))
    {
      ILOG3_DYD("CID-%04x: WDS on beacon excluded %Y, VapID %u",
        mtlk_vap_get_oid(master_wds->vap_handle),
        on_beacon->addr.au8Addr,
        on_beacon->vap_id);

      on_beacon_excl = on_beacon;
      if (do_remove)
        mtlk_dlist_remove(&master_wds->on_beacon_list, entry);
      break;
    }
  }
  return on_beacon_excl;
}

int __MTLK_IFUNC
wds_beacon_process (mtlk_handle_t master_wds_handle, const void *data,  uint32 data_size)
{
  wds_beacon_data_t   *beacon_data;
  mtlk_vap_manager_t  *vap_mng;
  mtlk_vap_handle_t   vap_handle;
  mtlk_core_t         *core;
  mtlk_core_t         *nic;
  wds_peer_t          *peer;
  wds_on_beacon_t     *on_beacon, on_beacon_copy;
  wds_t               *wds = NULL;
  wds_t               *master_wds = NULL;
  wds_beacon_action_e action = WDS_BEACON_IDLE;
  int bonding_core, channel_core, bonding_beacon, spectrum_core;
  BOOL is_ht_cap_changed = FALSE, is_vht_cap_changed = FALSE, is_he_cap_changed = FALSE;
  wave_radio_t        *radio;

  MTLK_ASSERT(data_size == sizeof(wds_beacon_data_t));

  beacon_data = (wds_beacon_data_t *)data;
  vap_mng = mtlk_vap_get_manager(((wds_t*)master_wds_handle)->vap_handle);
  nic = mtlk_vap_get_core(((wds_t*)master_wds_handle)->vap_handle);
  master_wds = &nic->slow_ctx->wds_mng;
  radio = wave_vap_radio_get(nic->vap_handle);

  /* find on_beacon entry */
  mtlk_osal_lock_acquire(&master_wds->on_beacon_lock);
  on_beacon = wds_on_beacon_find_remove(master_wds, &beacon_data->addr, FALSE);
  if (on_beacon) {
    on_beacon_copy = *on_beacon;
    if (on_beacon->ie_data) {
      on_beacon_copy.ie_data = mtlk_osal_mem_alloc(on_beacon->ie_data_len, LQLA_MEM_TAG_WDS);
      if (!on_beacon_copy.ie_data) {
        mtlk_osal_lock_release(&master_wds->on_beacon_lock);
        ELOG_V("Cannot allocate mem !");
        goto end;
      }
      wave_memcpy(on_beacon_copy.ie_data, on_beacon->ie_data_len, on_beacon->ie_data, on_beacon->ie_data_len);
      mtlk_osal_mem_free(on_beacon->ie_data);
      on_beacon->ie_data = NULL;
    }
    on_beacon = &on_beacon_copy;
  }
  mtlk_osal_lock_release(&master_wds->on_beacon_lock);

  if (NULL == on_beacon) /* peer might be already deleted */
    goto end;
  if ((NULL == on_beacon->ie_data) || (0 == on_beacon->ie_data_len)) { /*should not happen */
    ELOG_V("Beacon IEs are missed!");
    MTLK_ASSERT(0);
    goto end;
  }

  /* update pointer to ie_data and ie_data_len within beacon_data */
  beacon_data->ie_data     = on_beacon->ie_data;
  beacon_data->ie_data_len = on_beacon->ie_data_len;

  if (MTLK_ERR_OK != mtlk_vap_manager_get_vap_handle(vap_mng, beacon_data->vap_idx, &vap_handle))
    goto end;   /* VAP does not exist */

  core = mtlk_vap_get_core(vap_handle);
  if (NET_STATE_CONNECTED != mtlk_core_get_net_state(core)){
    ILOG2_D("CID-%04x: VAP not active, ignore beacon", beacon_data->vap_idx);
    goto end;
  }

  if (!core->slow_ctx->ap_started) {
    ILOG5_D("CID-%04x: AP not started, ignore beacon", beacon_data->vap_idx);
    goto end;
  }

  if (BR_MODE_WDS != MTLK_CORE_HOT_PATH_PDB_GET_INT(core, CORE_DB_CORE_BRIDGE_MODE))
    goto end;   /* VAP not in WDS mode */

  /* Find beacon bssid in WDS peer list of particular VAP */
  wds = &core->slow_ctx->wds_mng;
  mtlk_osal_lock_acquire(&wds->peers_list_lock);

  peer = _wds_peer_find(wds, &beacon_data->addr);
  if (!peer)
    goto unlock;

  peer->beacon_age_ts = mtlk_osal_timestamp();

  if ((peer->state == WDS_STATE_DISCONNECTED) && mtlk_osal_atomic_get(&peer->can_connect)) {
    /* get bonding, channel, spectrum from master VAP ctx */
    bonding_core = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_BONDING_MODE);
    channel_core = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_CHANNEL_CUR);
    spectrum_core = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_SPECTRUM_MODE);
    bonding_beacon = (beacon_data->second_chan_offset == UMI_CHANNEL_SW_MODE_SCB) ? ALTERNATE_LOWER :
                     (beacon_data->second_chan_offset == UMI_CHANNEL_SW_MODE_SCA) ? ALTERNATE_UPPER : ALTERNATE_NONE;

     /* exclude bonding from check in case of 20 Mhz spectrum */
    if (CW_20 == spectrum_core)
      bonding_core = bonding_beacon;
    /* exclude bonding from check in case of non-HT beacon */
    if ((CW_40 == spectrum_core) && (!beacon_data->is_ht))
      bonding_core = bonding_beacon;
    /* exclude bonding from check in case of VHT beacon */
    if (beacon_data->is_vht)
      bonding_core = bonding_beacon;
    /* exclude bonding from check in case of non HT/VHT beacon or no secondary
     * channel */
    if ((!beacon_data->is_vht && !beacon_data->is_ht) || bonding_beacon == ALTERNATE_NONE)
      bonding_core = bonding_beacon;

    /* exclude bonding from check in case of HE beacon */
    if (beacon_data->is_he)
      bonding_core = bonding_beacon;
    /* exclude bonding from check in case of non HT/VHT beacon or no secondary
     * channel */
    if ((!beacon_data->is_vht && !beacon_data->is_ht && !beacon_data->is_he) || bonding_beacon == ALTERNATE_NONE)
      bonding_core = bonding_beacon;

    if (bonding_core ^ bonding_beacon) {
      ILOG3_DD("Beacon on wrong bonding!  Our bonding %d, Beacon's bonding %d", bonding_core, bonding_beacon);
      peer->numof_beacons_wrong_bond++;
    }
    if (channel_core ^ beacon_data->channel) {
      ILOG3_DD("Beacon on wrong channel! Our channel %d, Beacon's channel %d", channel_core, beacon_data->channel);
      peer->numof_beacons_wrong_chan++;
    }
    if ((bonding_core ^ bonding_beacon) | (channel_core ^ beacon_data->channel))
      goto unlock;

    ILOG0_DY("CID-%04x: beacon for peer %Y. Connecting peer AP.", mtlk_vap_get_oid(vap_handle), &peer->addr);
    peer->beacon_data = *beacon_data;
    peer->state = WDS_STATE_CONNECTING;
    action = WDS_BEACON_CONNECT;
  } else if (peer->state == WDS_STATE_CONNECTED) {
      if (beacon_data->is_ht)
        is_ht_cap_changed = memcmp(peer->beacon_data.ht_cap_info, beacon_data->ht_cap_info, beacon_data->ht_cap_info_len);
      if (beacon_data->is_vht)
        is_vht_cap_changed = memcmp(peer->beacon_data.vht_cap_info, beacon_data->vht_cap_info, beacon_data->vht_cap_info_len);
      if (beacon_data->is_he) {
        is_he_cap_changed = memcmp(&peer->beacon_data.he_cap, &beacon_data->he_cap, sizeof(beacon_data->he_cap));
        if (is_he_cap_changed) {
            mtlk_dump(2, &peer->beacon_data.he_cap, sizeof(peer->beacon_data.he_cap), "peer:");
            mtlk_dump(2, &beacon_data->he_cap, sizeof(beacon_data->he_cap), "beacon:");
        }
      }
      if ((peer->beacon_data.basic_rate_set     ^ beacon_data->basic_rate_set)     |
          (peer->beacon_data.op_rate_set        ^ beacon_data->op_rate_set)        |
          (peer->beacon_data.channel            ^ beacon_data->channel)            |
          (peer->beacon_data.second_chan_offset ^ beacon_data->second_chan_offset) |
          (peer->beacon_data.protection         ^ beacon_data->protection)         |
          (peer->beacon_data.short_preamble     ^ beacon_data->short_preamble)     |
          (peer->beacon_data.short_slot_time    ^ beacon_data->short_slot_time)    |
          (peer->beacon_data.is_ht              ^ beacon_data->is_ht)              |
          (peer->beacon_data.is_vht             ^ beacon_data->is_vht)             |
          (peer->beacon_data.is_he              ^ beacon_data->is_he)              |
          (is_ht_cap_changed)                                                      |
          (is_he_cap_changed)                                                      |
          (is_vht_cap_changed)) {

        ILOG0_Y("WDS: Beacon change detectded for peer %Y. Reconnecting peer AP.", &peer->addr);
        ILOG1_DD("WDS: Basic rate set       : 0x%08X -> 0x%08X", peer->beacon_data.basic_rate_set, beacon_data->basic_rate_set);
        ILOG1_DD("WDS: Operational rate set : 0x%08X -> 0x%08X", peer->beacon_data.op_rate_set, beacon_data->op_rate_set);
        ILOG1_DD("WDS: Channel              : %d -> %d", peer->beacon_data.channel, beacon_data->channel);
        ILOG1_DD("WDS: Secondary chan offset: %d -> %d", peer->beacon_data.second_chan_offset, beacon_data->second_chan_offset);
        ILOG1_DD("WDS: Protection           : %d -> %d", peer->beacon_data.protection, beacon_data->protection);
        ILOG1_DD("WDS: Short preamble       : %d -> %d", peer->beacon_data.short_preamble, beacon_data->short_preamble);
        ILOG1_DD("WDS: Short slot time      : %d -> %d", peer->beacon_data.short_slot_time, beacon_data->short_slot_time);
        ILOG1_DD("WDS: HT                   : %d -> %d", peer->beacon_data.is_ht, beacon_data->is_ht);
        ILOG1_DD("WDS: VHT                  : %d -> %d", peer->beacon_data.is_vht, beacon_data->is_vht);
        ILOG1_DD("WDS: HE                   : %d -> %d", peer->beacon_data.is_he, beacon_data->is_he);
        ILOG1_S( "WDS: HT Cap Info          : %s", is_ht_cap_changed ? "changed" : "not changed");
        ILOG1_S( "WDS: VHT Cap Info         : %s", is_vht_cap_changed ? "changed" : "not changed");
        ILOG1_S( "WDS: HE Cap Info          : %s", is_he_cap_changed ? "changed" : "not changed");

        peer->state = WDS_STATE_DISCONNECTING;
        peer->numof_reconnect_beacon++;
        action = WDS_BEACON_DISCONNECT;
        peer->beacon_data = *beacon_data;
      }
  }

unlock:
  mtlk_osal_lock_release(&wds->peers_list_lock);

  switch (action) {
    case WDS_BEACON_CONNECT:
      wv_cfg80211_peer_connect(mtlk_df_user_get_ndev(mtlk_df_get_user(mtlk_vap_get_df(vap_handle))), &beacon_data->addr, beacon_data);
    break;
    case WDS_BEACON_DISCONNECT:
      wv_cfg80211_peer_disconnect(mtlk_df_user_get_ndev(mtlk_df_get_user(mtlk_vap_get_df(vap_handle))), &beacon_data->addr);
    break;
    case WDS_BEACON_IDLE:
    default:
    break;
  }

end:
  if (on_beacon) {
    if (NULL != on_beacon->ie_data) {
      mtlk_osal_mem_free(on_beacon->ie_data);
      on_beacon->ie_data = NULL;
    }
  }
  return MTLK_ERR_OK;
}

void __MTLK_IFUNC
wds_cleanup (wds_t *wds)
{
  mtlk_dlist_t local_peers_list;
  mtlk_dlist_entry_t *entry;
  wds_peer_t *peer;

  mtlk_dlist_init(&local_peers_list);

  /* Move entries to local list */
  mtlk_osal_lock_acquire(&wds->peers_list_lock);
  mtlk_dlist_move(&local_peers_list, &wds->peers_list);
  mtlk_osal_lock_release(&wds->peers_list_lock);

  /* delete all peers */
  while( NULL != (entry = mtlk_dlist_pop_front(&local_peers_list)) )
  {
    peer = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_peer_t, list_entry);
    _wds_peer_cleanup(peer);
    ILOG2_DY("CID-%04x: WDS peer delete context %Y", mtlk_vap_get_oid(peer->wds->vap_handle), peer->addr.au8Addr);
    mtlk_osal_mem_free(peer);
  }
  mtlk_dlist_cleanup(&local_peers_list);

  ILOG2_DP("CID-%04x: WDS Cleanup (%p)", mtlk_vap_get_oid(wds->vap_handle), wds);

  MTLK_CLEANUP_BEGIN(wds, MTLK_OBJ_PTR(wds))
    MTLK_CLEANUP_STEP(wds, EN_ABILITIES, MTLK_OBJ_PTR(wds),
                      wds_disable_abilities, (wds));
    MTLK_CLEANUP_STEP(wds, REG_ABILITIES, MTLK_OBJ_PTR(wds),
                      mtlk_abmgr_unregister_ability_set,
                      (mtlk_vap_get_abmgr(wds->vap_handle),
                       _wds_abilities, ARRAY_SIZE(_wds_abilities)));
    MTLK_CLEANUP_STEP(wds, PEERS_LIST_LOCK, MTLK_OBJ_PTR(wds),
                      mtlk_osal_lock_cleanup, (&wds->peers_list_lock));
    MTLK_CLEANUP_STEP(wds, PEERS_LIST, MTLK_OBJ_PTR(wds),
                      mtlk_dlist_cleanup, (&wds->peers_list));
    MTLK_CLEANUP_STEP(wds, ON_BEACON_LOCK, MTLK_OBJ_PTR(wds),
                      mtlk_osal_lock_cleanup, (&wds->on_beacon_lock));
    MTLK_CLEANUP_STEP(wds, ON_BEACON_LIST, MTLK_OBJ_PTR(wds),
                      mtlk_dlist_cleanup, (&wds->on_beacon_list));
  MTLK_CLEANUP_END(wds, MTLK_OBJ_PTR(wds));
}

int __MTLK_IFUNC
wds_init (wds_t *wds, mtlk_vap_handle_t vap_handle)
{
  wds->vap_handle = vap_handle;

  ILOG2_DP("CID-%04x: WDS Init (%p)", mtlk_vap_get_oid(wds->vap_handle), wds);

  MTLK_INIT_TRY(wds, MTLK_OBJ_PTR(wds));
    MTLK_INIT_STEP_VOID(wds, ON_BEACON_LIST, MTLK_OBJ_PTR(wds),
                        mtlk_dlist_init, (&wds->on_beacon_list));
    MTLK_INIT_STEP(wds, ON_BEACON_LOCK, MTLK_OBJ_PTR(wds),
                   mtlk_osal_lock_init, (&wds->on_beacon_lock));
    MTLK_INIT_STEP_VOID(wds, PEERS_LIST, MTLK_OBJ_PTR(wds),
                        mtlk_dlist_init, (&wds->peers_list));
    MTLK_INIT_STEP(wds, PEERS_LIST_LOCK, MTLK_OBJ_PTR(wds),
                   mtlk_osal_lock_init, (&wds->peers_list_lock));
    MTLK_INIT_STEP(wds, REG_ABILITIES, MTLK_OBJ_PTR(wds),
                   mtlk_abmgr_register_ability_set,
                   (mtlk_vap_get_abmgr(wds->vap_handle),
                    _wds_abilities, ARRAY_SIZE(_wds_abilities)));
    /* Note: WDS abilities are enabled on Bridge mode change to WDS */
    MTLK_INIT_STEP_VOID(wds, EN_ABILITIES, MTLK_OBJ_PTR(wds),
                        MTLK_NOACTION, ());
  MTLK_INIT_FINALLY(wds, MTLK_OBJ_PTR(wds))
  MTLK_INIT_RETURN(wds, MTLK_OBJ_PTR(wds), wds_cleanup, (wds))
}

int __MTLK_IFUNC
wds_usr_add_peer_ap (wds_t *wds, const IEEE_ADDR *addr)
{
  wave_radio_t *radio;
  int res;
  uint32 max_sta;

  ILOG2_DY("CID-%04x: WDS adding new peer ap %Y", mtlk_vap_get_oid(wds->vap_handle), addr->au8Addr);
  mtlk_osal_lock_acquire(&wds->peers_list_lock);

  /* Check is peer already in wds db */
  if (NULL != _wds_peer_find(wds, addr)) {
    ELOG_V("Peer already exist");
    res = MTLK_ERR_OK;
    goto end;
  }

  /* Check is maximum allowed number of peers reached */
  radio = wave_vap_radio_get(wds->vap_handle);
  max_sta = wave_radio_max_stas_get(radio);
  if (max_sta == mtlk_dlist_size(&wds->peers_list)) {
    ELOG_D("MAX number of Peer APs reached (%d)", max_sta);
    res = MTLK_ERR_NO_MEM;
    goto end;
  }

  res = _wds_peer_create_add(wds, addr);
  if (MTLK_ERR_OK == res) {
    wds_peer_t *peer = _wds_peer_find(wds, addr);
    if (peer)
      mtlk_osal_atomic_set(&peer->can_connect, 1);
  }
end:
  mtlk_osal_lock_release(&wds->peers_list_lock);
  return res;
}

int __MTLK_IFUNC
wds_usr_del_peer_ap (wds_t *wds, const IEEE_ADDR *addr)
{
  int res = MTLK_ERR_OK;
  wds_peer_t  *peer;
  wds_peer_state_t peer_state;

  ILOG2_DY("CID-%04x: WDS delete peer ap %Y", mtlk_vap_get_oid(wds->vap_handle), addr->au8Addr);
  mtlk_osal_lock_acquire(&wds->peers_list_lock);

  /* find peer in wds DB*/
  peer = _wds_peer_find(wds, addr);
  if (NULL == peer) {
    WLOG_Y("Can't remove peerAP %Y. Peer AP doesn't exist", addr->au8Addr);
    res = MTLK_ERR_NO_ENTRY;
    mtlk_osal_lock_release(&wds->peers_list_lock);
    return res;
  }
  peer_state = peer->state;
  _wds_peer_remove(peer);
  mtlk_osal_lock_release(&wds->peers_list_lock);
  _wds_peer_delete(peer);

  if ((peer_state == WDS_STATE_CONNECTED) || (peer_state == WDS_STATE_CONNECTING)) {
    ILOG0_DY("CID-%04x: disconnecting peer AP %Y", mtlk_vap_get_oid(wds->vap_handle), addr->au8Addr);
    wv_cfg80211_peer_disconnect(mtlk_df_user_get_ndev(mtlk_df_get_user(mtlk_vap_get_df(wds->vap_handle))), addr);
  }
  return res;

}

void __MTLK_IFUNC wds_peer_connect (wds_t *wds, sta_entry *sta, const IEEE_ADDR *addr, uint8 is_ht, uint8 is_vht, uint8 is_he)
{
  wds_peer_t  *peer;

  ILOG2_DY("CID-%04x: WDS peer connect %Y", mtlk_vap_get_oid(wds->vap_handle), addr->au8Addr);
  /* find peer in wds DB, change it's state */
  mtlk_osal_lock_acquire(&wds->peers_list_lock);
  peer = _wds_peer_find(wds, addr);
  if (peer) {
    sta->peer_ap = 1;
    peer->state = WDS_STATE_CONNECTED;
    peer->age_ts = mtlk_osal_timestamp();
    peer->numof_reconnect++;
    peer->is_ht = is_ht;
    peer->is_vht = is_vht;
    peer->is_he = is_he;
  }
  mtlk_osal_lock_release(&wds->peers_list_lock);
  ILOG1_S("WDS peer %s", peer ? "connected" : "not in WDS DB");
}

void __MTLK_IFUNC wds_peer_disconnect (wds_t *wds, const IEEE_ADDR *addr)
{
  wds_peer_t  *peer;

  ILOG2_DY("CID-%04x: WDS peer disconnect %Y", mtlk_vap_get_oid(wds->vap_handle), addr);
  mtlk_osal_lock_acquire(&wds->peers_list_lock);
  peer = _wds_peer_find(wds, addr);
  if (peer) {
    peer->state = WDS_STATE_DISCONNECTED;
    mtlk_osal_atomic_set(&peer->can_connect, 0);
    peer->is_ht = peer->is_vht = peer->is_he = 0;
    /* kick reconnect timer */
    _mtlk_osal_timer_set(&peer->reconnect_timer, WDS_RECONNECT_TIMEOUT);
  }
  mtlk_osal_lock_release(&wds->peers_list_lock);
  ILOG1_S("WDS peer %s", peer ? "disconnected" : "not in WDS DB");
}

BOOL __MTLK_IFUNC wds_sta_is_peer_ap(wds_t *wds, const IEEE_ADDR *addr)
{
  wds_peer_t  *peer = NULL;
  mtlk_osal_lock_acquire(&wds->peers_list_lock);
  peer = _wds_peer_find(wds, addr);
  mtlk_osal_lock_release(&wds->peers_list_lock);
  return peer ? TRUE : FALSE;
}

void __MTLK_IFUNC wds_switch_off(wds_t *wds)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  wds_peer_t *peer;

  ILOG1_D("CID-%04x: Shutting down WDS", mtlk_vap_get_oid(wds->vap_handle));

  mtlk_osal_lock_acquire(&wds->peers_list_lock);

  do {
    peer = NULL;

    mtlk_dlist_foreach(&wds->peers_list, entry, head) {
      peer = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_peer_t, list_entry);
      if (peer)
        break;
    }

    if (peer) {
      if (peer->state == WDS_STATE_CONNECTED || peer->state == WDS_STATE_CONNECTING) {
        peer->state = WDS_STATE_DISCONNECTING;
        wv_cfg80211_peer_disconnect(mtlk_df_user_get_ndev(mtlk_df_get_user(mtlk_vap_get_df(wds->vap_handle))), &peer->addr);
      }
      ILOG2_DY("CID-%04x: Removing peer AP %Y", mtlk_vap_get_oid(wds->vap_handle), peer->addr.au8Addr);
      _wds_peer_remove(peer);
      mtlk_osal_lock_release(&wds->peers_list_lock);
      _wds_peer_delete(peer);
      mtlk_osal_lock_acquire(&wds->peers_list_lock);
    }
  } while (peer);

  mtlk_osal_lock_release(&wds->peers_list_lock);
  return;
}

void __MTLK_IFUNC
wds_enable_abilities (wds_t *wds)
{
  if (wds && mtlk_vap_is_ap(wds->vap_handle))
  {
    ILOG2_D("CID-%04x: Enable WDS abilities", mtlk_vap_get_oid(wds->vap_handle));
    mtlk_abmgr_enable_ability_set(mtlk_vap_get_abmgr(wds->vap_handle),
                                  _wds_abilities, ARRAY_SIZE(_wds_abilities));
  }

  return;
}

void __MTLK_IFUNC
wds_disable_abilities (wds_t *wds)
{

  if (wds && mtlk_vap_is_ap(wds->vap_handle))
  {
    ILOG2_D("CID-%04x: Disable WDS abilities", mtlk_vap_get_oid(wds->vap_handle));
    mtlk_abmgr_disable_ability_set(mtlk_vap_get_abmgr(wds->vap_handle),
                                    _wds_abilities, ARRAY_SIZE(_wds_abilities));
  }
  return;
}

void __MTLK_IFUNC
wds_on_if_down (wds_t *wds)
{

  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  wds_peer_t *peer;
  wds_on_beacon_t *on_beacon;
  wds_t *master_wds;
  uint8 vap_id;

  ILOG2_D("CID-%04x: WDS on IF DOWN", mtlk_vap_get_oid(wds->vap_handle));
  vap_id = mtlk_vap_get_id(wds->vap_handle);

  /* Remove all STA with this->vap_handle from master's on_beacon_DB */
  master_wds = _wds_get_master(wds);
  if (__UNLIKELY(NULL == master_wds)) {
    ELOG_D("CID-%04x: WDS Manager not found", mtlk_vap_get_oid(wds->vap_handle));
    return;
  }

  mtlk_osal_lock_acquire(&master_wds->on_beacon_lock);
  mtlk_dlist_foreach(&master_wds->on_beacon_list, entry, head)
  {
    on_beacon = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_on_beacon_t, list_entry);
    if (vap_id == on_beacon->vap_id)
    {
      ILOG3_DY("CID-%04x: WDS On beacon record removed %Y", mtlk_vap_get_oid(wds->vap_handle), on_beacon->addr.au8Addr);
      entry = mtlk_dlist_remove(&master_wds->on_beacon_list, entry);

      if (NULL != on_beacon->ie_data) {
        mtlk_osal_mem_free(on_beacon->ie_data);
      }
      mtlk_osal_mem_free(on_beacon);
    }
  }
  mtlk_osal_lock_release(&master_wds->on_beacon_lock);

  /* Remove Peers from WDS DB */
  mtlk_osal_lock_acquire(&wds->peers_list_lock);

  mtlk_dlist_foreach(&wds->peers_list, entry, head)
  {
    peer = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_peer_t, list_entry);
    ILOG3_DY("CID-%04x: WDS peer force disconnect %Y", mtlk_vap_get_oid(wds->vap_handle), peer->addr.au8Addr);

    peer->state = WDS_STATE_DISCONNECTED;
  }

  mtlk_osal_lock_release(&wds->peers_list_lock);
  return;
}

void __MTLK_IFUNC
wds_on_if_up (wds_t *wds)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  wds_peer_t *peer;

  ILOG2_D("CID-%04x: WDS on IF UP", mtlk_vap_get_oid(wds->vap_handle));
  mtlk_osal_lock_acquire(&wds->peers_list_lock);

  mtlk_dlist_foreach(&wds->peers_list, entry, head) {
    peer = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_peer_t, list_entry);
    _wds_on_beacon_create_add(wds, &peer->addr);
  }

  mtlk_osal_lock_release(&wds->peers_list_lock);
  return;
}

int __MTLK_IFUNC
mtlk_wds_get_peer_vect (wds_t *wds, mtlk_clpb_t **ppclpb_peer_vect)
{
  int res = MTLK_ERR_OK;
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  wds_peer_t *peer;

  *ppclpb_peer_vect = mtlk_clpb_create();
  if (NULL == *ppclpb_peer_vect) {
    ELOG_V("Cannot allocate clipboard object");
    res = MTLK_ERR_NO_MEM;
    goto end;
  }

  /* Add peers to output data buff */
  mtlk_osal_lock_acquire(&wds->peers_list_lock);
  mtlk_dlist_foreach(&wds->peers_list, entry, head)
  {
    peer = MTLK_LIST_GET_CONTAINING_RECORD(entry, wds_peer_t, list_entry);
    res = mtlk_clpb_push(*ppclpb_peer_vect, &peer->addr, sizeof(IEEE_ADDR));
    if(MTLK_ERR_OK != res) {
      ELOG_V("Cannot push data to the clipboard");
      goto err_push_data;
    }
  }

  goto no_err;

err_push_data:
  mtlk_clpb_delete(*ppclpb_peer_vect);
  *ppclpb_peer_vect = NULL;

no_err:
  mtlk_osal_lock_release(&wds->peers_list_lock);

end:
  return res;
}
