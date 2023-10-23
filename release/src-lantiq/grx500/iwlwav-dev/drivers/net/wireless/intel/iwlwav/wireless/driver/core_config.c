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
 * Core configuration implementation
 *
 */
#include "mtlkinc.h"
#include "core.h"
#include "core_config.h"
#include "mtlk_df_nbuf.h"
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
#include "wave_hal_stats.h"
#include "wave_radio.h"
#include "wave_80211ax.h"
#include "ieee80211_crc32.h" /* for IEEE80211 32-bit CRC computation */

#define LOG_LOCAL_GID   GID_CORE_CONFIG
#define LOG_LOCAL_FID   0

#define RNR_6G_SHORT_SSID_LENGTH 4

int poll_client_req(mtlk_vap_handle_t vap_handle, sta_entry * sta, int *status)
{
  int                       res = MTLK_ERR_OK;
  mtlk_txmm_msg_t           man_msg;
  mtlk_txmm_data_t          *man_entry = NULL;
  UMI_POLL_CLIENT           *pPollClient = NULL;

  *status = 0;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_POLL_CLIENT_REQ;
  man_entry->payload_size = sizeof(UMI_POLL_CLIENT);

  pPollClient = (UMI_POLL_CLIENT *)(man_entry->payload);
  MTLK_STATIC_ASSERT(sizeof(pPollClient->stationIndex) == sizeof(uint16));
  if(NULL == sta) {
    WLOG_D("CID-%04x: station not found", mtlk_vap_get_oid(vap_handle));
    res = MTLK_ERR_UNKNOWN;
    goto end;
  }
  pPollClient->stationIndex = HOST_TO_MAC16((uint16)mtlk_sta_get_sid(sta));
  pPollClient->isActive = 0;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK  == pPollClient->Status) {
    *status = pPollClient->isActive;
    if (*status) {
      sta->activity_timestamp = mtlk_osal_timestamp();
    }
  }
  else {
    ELOG_DDD("CID-%04x: Can't get poll client status, res=%i status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, pPollClient->Status);
    if (UMI_OK != pPollClient->Status)
      res = MTLK_ERR_MAC;
  }

end:
  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

int __MTLK_IFUNC core_cfg_get_station (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  sta_entry *sta = NULL;
  st_info_data_t *info_data;
  unsigned info_data_size;
  int res = MTLK_ERR_OK;
  int status;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  info_data = mtlk_clpb_enum_get_next(clpb, &info_data_size);
  MTLK_CLPB_TRY(info_data, info_data_size)
    sta = wv_ieee80211_get_sta(info_data->sta);
    if (mtlk_vap_is_ap(core->vap_handle) && !wave_core_sync_hostapd_done_get(core)) {
      ILOG1_D("CID-%04x: HOSTAPD STA database is not synced", mtlk_vap_get_oid(core->vap_handle));
      MTLK_CLPB_EXIT(MTLK_ERR_NOT_READY);
    }
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not added", mtlk_vap_get_oid(core->vap_handle), info_data->mac);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    }

    res = poll_client_req(core->vap_handle, sta, &status);

    /* reset "filled"  bitstring (update it after each new variable is filled in the station info structure) */
    info_data->stinfo->filled = 0;

    /* connected time (in seconds) */
    info_data->stinfo->connected_time = mtlk_sta_get_age(sta);
    info_data->stinfo->filled |= BIT_ULL(NL80211_STA_INFO_CONNECTED_TIME);

    /* inactive time (in milliseconds) */
    info_data->stinfo->inactive_time = mtlk_sta_get_inactive_time(sta);
    info_data->stinfo->filled |= BIT_ULL(NL80211_STA_INFO_INACTIVE_TIME);

    /* the value is signed, therefore decided to use signal
       field of station_info structure, not max_rssi, which is unsigned */
    info_data->stinfo->signal = mtlk_sta_get_mgmt_max_rssi(sta);
    info_data->stinfo->filled |= BIT_ULL(NL80211_STA_INFO_SIGNAL);

    ILOG2_DD("STA age: %u sec, STA inactive: %u msec", info_data->stinfo->connected_time, info_data->stinfo->inactive_time);
  MTLK_CLPB_FINALLY(res)
    if (sta) mtlk_sta_decref(sta);
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

/* Send current WMM params to FW and set vap_in_fw_is_active to TRUE if sending succeeded */
static int
_mtlk_core_cfg_wmm_param_send(mtlk_core_t *core)
{
  int res;
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry;
  mtlk_pdb_t       *param_db_core;
  UMI_SET_WMM_PARAMETERS *req;
  unsigned  oid;
  int       i;

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG1_D("CID-%04x: Send WMM params", oid);

  /* prepare msg for the FW */
  if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &res)))
  {
    ELOG_DD("CID-%04x: UM_MAN_SET_WMM_PARAMETERS_REQ init failed, err=%i", oid, res);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id                  = UM_MAN_SET_WMM_PARAMETERS_REQ;
  man_entry->payload_size        = sizeof(*req);

  req = (UMI_SET_WMM_PARAMETERS *) man_entry->payload;
  memset(req, 0, sizeof(*req));

  if (mtlk_vap_is_ap(core->vap_handle))
    /* Read WMM settings from the Master VAP ParamDB and fill the message */
    param_db_core = mtlk_vap_get_param_db(mtlk_core_get_master(core)->vap_handle);
  else
    param_db_core = mtlk_vap_get_param_db(core->vap_handle);

  for (i = 0; i < UMI_AC_NUM_ACS; i++)
  {
    struct mtlk_txq_params mtp;
    mtlk_pdb_size_t mtp_len = sizeof(mtp);
    wave_pdb_get_binary(param_db_core, PARAM_DB_CORE_WMM_PARAMS_BE + i, &mtp, &mtp_len);
    req->wmmParamsPerAc[i].u16CWmin = HOST_TO_MAC16(mtp.cwmin);
    req->wmmParamsPerAc[i].u16CWmax = HOST_TO_MAC16(mtp.cwmax);
    req->wmmParamsPerAc[i].u16TXOPlimit = HOST_TO_MAC16(mtp.txop);
    req->wmmParamsPerAc[i].u8Aifsn = mtp.aifs;
    req->wmmParamsPerAc[i].acm_flag = mtp.acm_flag;
    ILOG2_DDDDDD("[%d]: txop %d, cwmin %d, cwmax %d, aifs %d, acm_flag %d",
                i, mtp.txop, mtp.cwmin, mtp.cwmax, mtp.aifs, mtp.acm_flag);
  }

  /* Set VAP ID and send the message */
  req->vapId = mtlk_vap_get_id(core->vap_handle);
  mtlk_dump(3, req, sizeof(*req), "dump of UMI_SET_WMM_PARAMETERS:");

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK  != req->u8Status) {
    ELOG_DDD("CID-%04x: UM_MAN_SET_WMM_PARAMETERS_REQ send failed, res=%i status=%hhu",
            oid, res, req->u8Status);
    if (UMI_OK  != req->u8Status)
      res = MTLK_ERR_MAC;
  } else {
    core->vap_in_fw_is_active = TRUE;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int
_mtlk_core_cfg_wmm_param_set_ap(mtlk_core_t *master_core)
{
    mtlk_vap_handle_t   vap_handle;
    mtlk_core_t        *vap_core;
    mtlk_vap_manager_t *vap_mgr;
    int                 i, vap_res, max_vaps_count;
    int                 res = MTLK_ERR_OK;

    MTLK_ASSERT(NULL != master_core);

    /* Send a msg with all WMM params to each VAP */
    vap_mgr = mtlk_vap_get_manager(master_core->vap_handle);
    max_vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);

    for (i = 0; i < max_vaps_count; i++)
    {
      if (MTLK_ERR_OK != mtlk_vap_manager_get_vap_handle(vap_mgr, i, &vap_handle))
      {
        ILOG5_D("VapID %u doesn't exist, goto next", i);
        continue;
      }

      ILOG1_DD("VapID %u, is_ap %d", i, (int)mtlk_vap_is_ap(vap_handle));

      if (!mtlk_vap_is_ap(vap_handle))
        continue;

      vap_core = mtlk_vap_get_core(vap_handle);

      /* Don't send if ADD_VAP has not yet been sent */
      if (vap_core->is_stopped)
        continue;

      /* Don't send if SET_BSS has not yet been sent */
      if (mtlk_core_get_net_state(vap_core) != NET_STATE_CONNECTED) {
        ILOG0_D("VapID %d is not ready, not setting wmm", i);
        continue;
      }

      ILOG1_D("CID-%04x: Sending UMI_SET_WMM_PARAMETERS", mtlk_vap_get_oid(vap_handle));
      vap_res = _mtlk_core_cfg_wmm_param_send(vap_core);
      if (vap_res != MTLK_ERR_OK) {
        res = vap_res;
      }
    }

    return res;
}

/* Should be called from master serializer context */
int __MTLK_IFUNC
core_cfg_wmm_param_set_by_params(mtlk_core_t *master_core, mtlk_core_t *core)
{
  unsigned                   oid;
  int                        res = MTLK_ERR_OK;

  MTLK_ASSERT(master_core == mtlk_core_get_master(master_core));

  oid = mtlk_vap_get_oid(core->vap_handle);

  /* Prevent setting WMM before ADD_VAP */
  if (core->is_stopped) {
    res = MTLK_ERR_NOT_READY;
    goto end;
  }

  /* Prevent setting WMM while waiting for beacons */
  if (mtlk_core_is_block_tx_mode(master_core)) {
    res = MTLK_ERR_NOT_READY;
    goto end;
  }

  /* Prevent setting WMM while in scan */
  if (mtlk_core_is_in_scan_mode(master_core)) {
    res = MTLK_ERR_RETRY;
    goto end;
  }

  ILOG1_D("CID-%04x", oid);

  if (mtlk_vap_is_ap(core->vap_handle) &&
      core == master_core) {
    res = _mtlk_core_cfg_wmm_param_set_ap(master_core);
  } else {
    res = _mtlk_core_cfg_wmm_param_send(core);
  }

end:
  return res;
}

/* Should be called from master serializer context */
int __MTLK_IFUNC core_cfg_wmm_param_set (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *master_core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_vap_handle_t vap_handle;
  uint8 *vap_id;
  unsigned clpb_size;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(sizeof(mtlk_clpb_t *) == data_size);

  vap_id = mtlk_clpb_enum_get_next(clpb, &clpb_size);
  MTLK_CLPB_TRY(vap_id, clpb_size)
    if (MTLK_ERR_OK != mtlk_vap_manager_get_vap_handle(mtlk_vap_get_manager(master_core->vap_handle),
                                                                            *vap_id, &vap_handle)) {
      res = MTLK_ERR_VALUE;
      MTLK_CLPB_EXIT(res);
    }

    res = core_cfg_wmm_param_set_by_params(master_core, mtlk_vap_get_core(vap_handle));
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int core_cfg_set_calibration_mask(mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;
  wave_radio_calibration_cfg_t *calibration_cfg;
  unsigned calibration_cfg_size;
  wave_radio_t  *radio;
  int            res = MTLK_ERR_OK;

  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));
  MTLK_ASSERT(sizeof(mtlk_clpb_t *) == data_size);

  radio = wave_vap_radio_get(core->vap_handle);

  calibration_cfg = mtlk_clpb_enum_get_next(clpb, &calibration_cfg_size);
  MTLK_CLPB_TRY(calibration_cfg, calibration_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(calibration_cfg, calibr_algo_mask, wave_radio_send_hdk_config,
                                   (radio, calibration_cfg->calibr_algo_mask,
                                    WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_ONLINE_ACM)), res);

      MTLK_CFG_CHECK_ITEM_AND_CALL(calibration_cfg, online_calibr_algo_mask, wave_radio_send_hdk_config,
                                   (radio, WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_CALIBRATION_ALGO_MASK),
                                   calibration_cfg->online_calibr_algo_mask), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int core_cfg_get_calibration_mask(mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;
  wave_radio_calibration_cfg_t calibration_cfg;
  wave_radio_t  *radio;
  int            res;

  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));
  MTLK_ASSERT(sizeof(mtlk_clpb_t *) == data_size);

  radio = wave_vap_radio_get(core->vap_handle);

  /* get from the FW */
  res = wave_radio_read_hdk_config(radio,
                                   &calibration_cfg.calibr_algo_mask,
                                   &calibration_cfg.online_calibr_algo_mask);

  MTLK_CFG_SET_ITEM(&calibration_cfg, calibr_algo_mask, calibration_cfg.calibr_algo_mask);
  MTLK_CFG_SET_ITEM(&calibration_cfg, online_calibr_algo_mask, calibration_cfg.online_calibr_algo_mask);

  /* push result and config into clipboard */
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &calibration_cfg, sizeof(calibration_cfg));
  }
  return res;
}

int core_cfg_send_set_chan_by_msg(mtlk_txmm_msg_t *man_msg)
{
  mtlk_txmm_data_t *man_entry = man_msg->data;
  UM_SET_CHAN_PARAMS *req = (UM_SET_CHAN_PARAMS *) man_entry->payload;
  int res;

  /* FW expects bg_scan as a flag 0 or 1, make sure it is the case */
  req->bg_scan = !!req->bg_scan;

  ILOG1_DDDDSDDD("Switching channel to %hhu--%u (width %u, primary at %u), type %s, sub band DFS %d, bg_scan %d, "
      "block_tx_pre %d",
              req->low_chan_num,
              req->low_chan_num + (CHANNUMS_PER_20MHZ << req->chan_width) - CHANNUMS_PER_20MHZ,
              (MHZS_PER_20MHZ << req->chan_width),
              req->low_chan_num + CHANNUMS_PER_20MHZ * req->primary_chan_idx,
              switchtype2string(req->switch_type),
              req->ChannelNotificationMode,
              req->bg_scan, req->block_tx_pre);
  mtlk_dump(2, req, sizeof(*req), "dump of UM_SET_CHAN_PARAMS:");

  res = mtlk_txmm_msg_send_blocked(man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (res != MTLK_ERR_OK)
  {
      ELOG_D("UM_SET_CHAN_REQ send failed, err=%i", res);
  }
  else if (req->result)
  {
    ELOG_D("UM_SET_CHAN_REQ execution failed, result=%hhu", req->result);
    res = MTLK_ERR_MAC;
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_update_antenna_mask (mtlk_core_t *core, uint8 ant_mask, BOOL update_coc_masks)
{
  int res = MTLK_ERR_OK;
  BOOL update_hw = FALSE;
  mtlk_coc_t *coc_obj = __wave_core_coc_mgmt_get(core);

  MTLK_ASSERT(coc_obj);

  res = mtlk_coc_update_antenna_cfg(coc_obj, ant_mask, update_coc_masks, &update_hw);

  if (MTLK_ERR_OK != res)
    return res;

  if (update_hw) { /* Update Antennas config and current mask */
    wave_radio_antenna_cfg_update(wave_vap_radio_get(core->vap_handle), ant_mask);
  }

  return res;
}

static int
_mtlk_core_activate_erp_if_needed (mtlk_core_t *core, uint8  switch_type)
{
  int res;
  mtlk_erp_t *erp_obj = __wave_core_erp_mgmt_get(core);

  if ((switch_type == ST_NORMAL) && (!mtlk_core_scan_is_running(core))) {
    res = mtlk_coc_erp_activate(erp_obj);
  } else {
    res = mtlk_coc_erp_deactivate(erp_obj);
  }
  return res;
}

static uint8 _wave_chandef_dfs_bitmap_get (wave_radio_t *radio, const struct mtlk_chan_def *cd)
{
  uint32 chanwidth, center_freq;
  struct wiphy *wiphy = wv_mac80211_wiphy_get(wave_radio_mac80211_get(radio));

  if (cd->chan.band == MTLK_HW_BAND_6_GHZ)
    return 0;

  if (cd->sb_dfs.sb_dfs_bw) {
    chanwidth = mtlkcw2cw(cd->sb_dfs.width);
    center_freq = cd->sb_dfs.center_freq;
  }
  else {
    chanwidth = mtlkcw2cw(cd->width);
    center_freq = cd->center_freq1;
  }

  return wv_cfg80211_get_chans_dfs_bitmap(wiphy, center_freq, chanwidth);
}

static uint32 _wave_get_cf1_or_cf2(const struct mtlk_chan_def *cd)
{
  if (!cd)
    return 0;
  if ((cd->chan.band == MTLK_HW_BAND_6_GHZ) && (cd->width == CW_160)) {
    MTLK_ASSERT(cd->center_freq2 != 0);
    return cd->center_freq2;
  }
  return cd->center_freq1;
}

/* cpd may be NULL, only needed for the CSA stuff */
int core_cfg_send_set_chan(mtlk_core_t *core,
                           const struct mtlk_chan_def *cd, struct set_chan_param_data *cpd)
{
  mtlk_txmm_msg_t       man_msg;
  mtlk_txmm_data_t      *man_entry;
  UM_SET_CHAN_PARAMS    *req;
  struct mtlk_chan_def  *ccd = __wave_core_chandef_get(core);
  wave_radio_t          *radio = wave_vap_radio_get(core->vap_handle);
  unsigned              primary_chan_num, old_freq1;
  int                   res = MTLK_ERR_OK;
  mtlk_hw_t             *hw = mtlk_vap_get_hw(core->vap_handle);
  BOOL skip_set_chan_zwdfs = FALSE;
  uint32                csa_deauth_mode_per_vap = 0, max_vaps_count = 0, vap_num = 0;
  uint32                uc_vap_bitmap = 0, mc_vap_bitmap = 0, mapped_addr = 0;
  uint16                beacon_int = 0, csa_uc_deauth_tx_time = 0, csa_mc_deauth_tx_time = 0;
  uint8                 vap_id = 0, *frame_buf[GEN6_MAX_VAP];
  BOOL                  enc_frame = FALSE;
  mtlk_pdb_size_t       data_size = 0;
  mtlk_vap_handle_t     vap_handle;
  mtlk_vap_manager_t    *vap_mgr = NULL;
  mtlk_core_t           *vap_core;
  struct intel_vendor_channel_switch_cfg vendor_cs_cfg;

  if (wave_radio_get_is_zwdfs_radio(radio) &&
      !wave_radio_get_zwdfs_ant_enabled(radio)) {
    skip_set_chan_zwdfs = TRUE;
    ILOG0_V("Ignoring channel switches for ZWDFS VAP: Antenna mask not active yet");
  }

  ILOG2_DDDDDD("band=%u, center_freq=%hu, center_freq1=%u, center_freq2=%u, flags=0x%08x, nl_width=%u",
                cd->chan.band, cd->chan.center_freq, cd->center_freq1, cd->center_freq2,
                cd->chan.flags, cd->width);

  /* prepare msg for the FW */
  if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &res)))
  {
    ELOG_DD("CID-%04x: UM_MAN_SET_CHAN_REQ init failed, err=%i",
            mtlk_vap_get_oid(core->vap_handle), res);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_SET_CHAN_REQ;
  man_entry->payload_size = sizeof(*req);
  req = (UM_SET_CHAN_PARAMS *) man_entry->payload;
  memset(req, 0, sizeof(*req));

  req->RegulationType = (uint8)core_cfg_get_regd_code(core);

  /* Make sure that our notion of channel width codes and FW's notion coincide */
  MTLK_STATIC_ASSERT(CW_20 == (unsigned) BANDWIDTH_TWENTY);
  MTLK_STATIC_ASSERT(CW_40 == (unsigned) BANDWIDTH_FOURTY);
  MTLK_STATIC_ASSERT(CW_80 == (unsigned) BANDWIDTH_EIGHTY);
  MTLK_STATIC_ASSERT(CW_160 == (unsigned) BANDWIDTH_ONE_HUNDRED_SIXTY);
  MTLK_STATIC_ASSERT(CW_80_80 == (unsigned) BANDWIDTH_EIGHTY_EIGHTY);

  if (MTLK_SB_DFS_BW_20 == cd->sb_dfs.sb_dfs_bw)
    req->ChannelNotificationMode = CHANNEL_NOTIFICATION_TWENTY;
  else if (MTLK_SB_DFS_BW_40 == cd->sb_dfs.sb_dfs_bw)
    req->ChannelNotificationMode = CHANNEL_NOTIFICATION_FOURTY;
  else if (MTLK_SB_DFS_BW_80 == cd->sb_dfs.sb_dfs_bw)
    req->ChannelNotificationMode = CHANNEL_NOTIFICATION_EIGHTY;
  else
    req->ChannelNotificationMode = CHANNEL_NOTIFICATION_NORMAL;

  if (req->ChannelNotificationMode) {
    req->chan_width = cd->sb_dfs.width;
    req->low_chan_num = freq2lowchannum(cd->sb_dfs.center_freq, cd->sb_dfs.width);
  }
  else {
    req->chan_width = cd->width;
    req->low_chan_num = freq2lowchannum(_wave_get_cf1_or_cf2(cd), cd->width);
  }

  primary_chan_num = ieee80211_frequency_to_channel(cd->chan.center_freq);
  req->primary_chan_idx = (primary_chan_num - req->low_chan_num) / CHANNUMS_PER_20MHZ;

  req->isRadarDetectionNeeded = _wave_chandef_dfs_bitmap_get(radio, cd);
  req->isContinuousInterfererDetectionNeeded = is_interf_det_needed(primary_chan_num);

  if (cd->width == CW_80_80) /* this assumes that center_freq2 > center_freq1 */
  {
    req->low_chan_num2 = freq2lowchannum(cd->center_freq2, cd->width);

    if (req->primary_chan_idx >= CHANS_IN_80MHZ) /* not in the first range, so must be in the second */
      req->primary_chan_idx = CHANS_IN_80MHZ + (primary_chan_num - req->low_chan_num2) / CHANNUMS_PER_20MHZ;
  }

  if (cpd)
  {
    /* 0 except for CSA, in which case multiplication will already have been done */
    req->chan_switch_time = HOST_TO_MAC16(cpd->chan_switch_time);
    req->switch_type = cpd->switch_type;
    req->block_tx_pre = cpd->block_tx_pre;
    req->block_tx_post = cpd->block_tx_post;
    req->bg_scan = cpd->bg_scan;
    req->u16SID = HOST_TO_MAC16(cpd->sid);
  }

  if (req->switch_type == ST_CSA) {
    /* Handle CSA deauth parameters here */
    memset(req->csaMcDeauthFramesLocation, 0, sizeof(req->csaMcDeauthFramesLocation));
    memset(req->csaMcDeauthFrameLength, 0, sizeof(req->csaMcDeauthFrameLength));
    memset(frame_buf, 0, sizeof(frame_buf));
    req->csaTxMcDeauthStartTime = req->csaTxUcDeauthStartTime = 0;
    req->csaMcDeAuthVapBitmap = req->csaUcDeAuthVapBitmap = req->csaIsMcDeauthFrameEncrypted = 0;
    /* Fill in CSA deauth params to UMI structure */
    data_size = sizeof(vendor_cs_cfg);
    vap_mgr = mtlk_vap_get_manager(core->vap_handle);
    max_vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
    for (vap_num = 0; vap_num < max_vaps_count; vap_num++) {
      if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_num, &vap_handle) != MTLK_ERR_OK) {
        ILOG3_D("Can't get VAP handle with num %d", vap_num);
        continue;
      }
      if (mtlk_vap_is_master(vap_handle))
        continue;
      if (!mtlk_vap_is_ap(vap_handle))
        continue;
      vap_core = mtlk_vap_get_core(vap_handle);
      MTLK_ASSERT(NULL != vap_core);
      if (vap_core->is_stopped)
        continue;
      memset(&vendor_cs_cfg, 0, data_size);
      /* Read vendor parameters for CSA deauth stored under this (master) VAP */
      res = MTLK_CORE_PDB_GET_BINARY(vap_core,
                          PARAM_DB_CORE_CSA_DEAUTH_PARAMS, &vendor_cs_cfg, &data_size);
      if (res != MTLK_ERR_OK || data_size < sizeof(vendor_cs_cfg)) {
        ELOG_D("CID-%04x: Can't read CSA deauth cfg from PDB", mtlk_vap_get_oid(vap_handle));
        res = MTLK_ERR_PARAMS;
        goto end;
      }
      csa_deauth_mode_per_vap = vendor_cs_cfg.csaDeauthMode;
      if (csa_deauth_mode_per_vap == CSA_DEAUTH_MODE_DISABLED) {
        continue;
      }
      /* Tx time is not supported per VAP level by FW, so send the last non-zero value of Tx time
       * from the VAP list
       */
      vap_id = mtlk_vap_get_id(vap_handle);
      if (csa_deauth_mode_per_vap == CSA_DEAUTH_MODE_UNICAST) {
        uc_vap_bitmap |= (1 << vap_id);
        if (vendor_cs_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX])
          csa_uc_deauth_tx_time = vendor_cs_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_UC_IDX];
      }
      else if (csa_deauth_mode_per_vap == CSA_DEAUTH_MODE_BROADCAST) {
        mc_vap_bitmap |= (1 << vap_id);
        /* Allocate memory from DMA region for Deauth frame length into frame_bus[vap_id].
         */
        frame_buf[vap_id] = mtlk_osal_mem_dma_alloc(vendor_cs_cfg.csaMcDeauthFrameLength * sizeof(vendor_cs_cfg.csaDeauthFrames[0]), MTLK_MEM_TAG_CSA_DEAUTH_DATA);
        if (!frame_buf[vap_id]) {
          ELOG_D("Malloc %u bytes for CSA deauth frame failed", vendor_cs_cfg.csaMcDeauthFrameLength * sizeof(vendor_cs_cfg.csaDeauthFrames[0]));
          res = MTLK_ERR_NO_RESOURCES;
          goto end;
        }
        wave_memcpy(frame_buf[vap_id], vendor_cs_cfg.csaMcDeauthFrameLength * sizeof(vendor_cs_cfg.csaDeauthFrames[0]), vendor_cs_cfg.csaDeauthFrames,
          vendor_cs_cfg.csaMcDeauthFrameLength * sizeof(vendor_cs_cfg.csaDeauthFrames[0]));
        /* Map the DMA address allocated above (frame_buf[vap_id]) to physical address and share the physical address with FW post endianess conversion.
         */
        mapped_addr = HOST_TO_MAC32(mtlk_osal_map_to_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(hw)),
                                                              frame_buf[vap_id], vendor_cs_cfg.csaMcDeauthFrameLength * sizeof(vendor_cs_cfg.csaDeauthFrames[0]),
                                                              MTLK_DATA_TO_DEVICE));
        if (!mapped_addr) {
          ILOG2_P("WARNING: failed mapping 0x%p to physical address", vendor_cs_cfg.csaDeauthFrames);
          res = MTLK_ERR_NO_RESOURCES;
          goto end;
        }
        req->csaMcDeauthFramesLocation[vap_id] = mapped_addr;
        req->csaMcDeauthFrameLength[vap_id] = vendor_cs_cfg.csaMcDeauthFrameLength * sizeof(vendor_cs_cfg.csaDeauthFrames[0]);
        if (req->csaMcDeauthFrameLength[vap_id] > INTEL_NON_PROTECTED_DEAUTH_FRAME_LEN)
          enc_frame = TRUE;
        if (vendor_cs_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX])
          csa_mc_deauth_tx_time = vendor_cs_cfg.csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_MC_IDX];
        /* reset mc frames values to zeroes */
        memset(&vendor_cs_cfg, 0, sizeof(vendor_cs_cfg));
        MTLK_CORE_PDB_SET_BINARY(vap_core,
          PARAM_DB_CORE_CSA_DEAUTH_PARAMS, &vendor_cs_cfg, data_size);
      }
    }
    /* If unicast and multicast tx time values are '0' then use default as,
     * 1 beacon interval for unicast and 2 beacon intervals for multicast.
     */
    if (uc_vap_bitmap != 0 || mc_vap_bitmap != 0) {
      beacon_int = (uint16) MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_BEACON_PERIOD);
      if (csa_uc_deauth_tx_time != 0) {
        req->csaTxUcDeauthStartTime = HOST_TO_MAC16(csa_uc_deauth_tx_time);
      }
      else {
        req->csaTxUcDeauthStartTime = HOST_TO_MAC16(beacon_int);
      }
      if (csa_mc_deauth_tx_time != 0) {
        req->csaTxMcDeauthStartTime = HOST_TO_MAC16(csa_mc_deauth_tx_time);
      }
      else {
        req->csaTxMcDeauthStartTime = HOST_TO_MAC16(beacon_int * 2);
      }
      req->csaUcDeAuthVapBitmap = HOST_TO_MAC32(uc_vap_bitmap);
      req->csaMcDeAuthVapBitmap = HOST_TO_MAC32(mc_vap_bitmap);
      if (enc_frame == TRUE)
        req->csaIsMcDeauthFrameEncrypted = req->csaMcDeAuthVapBitmap;
    }
  }

  /* leave result and reserved as 0 */

  old_freq1 = ccd->center_freq1;
  ccd->center_freq1 = 0; /* this marks that channel is "uncertain" (in this case changing) */

  wmb(); /* so that channel gets marked uncertain well before we start switching it */

  (void)mtlk_core_radio_enable_if_needed(core);

  if (skip_set_chan_zwdfs) {
    /* Skip sending set channel to FW when the antenna mask for ZWDFS
     * is not active yet. However, driver needs to update the current
     * channel def to valid channel, since CFG80211 events expect a
     * valid channel defined.
     *
     * Set channel is rejected at this point instead of at the entry level
     * in order to handle DFS channel setting when the ZWDFS antenna is
     * inactive. When DFS channel is set from Hostapd, the Hostapd enters
     * DFS mode and it waits on CAC finished from the driver. If we were to
     * reject the DFS channel setting in the entry level, the timer wouldn't
     * start and the Hostapd will wait infinitely for the CAC finished event.
     * Sending a error status also doesn't help as it will bring down the
     * Hostapd and take it to disabled state.
     */
    res = MTLK_ERR_OK;
  }
  else {
    res = core_cfg_send_set_chan_by_msg(&man_msg);
    mtlk_hw_reset_chan_statistics(hw, primary_chan_num, wave_radio_id_get(radio));
  }

  /* Send CCA threshold on every set channel */
  if (MTLK_ERR_OK == res && !skip_set_chan_zwdfs) {
    mtlk_core_cfg_send_actual_cca_threshold(core);
  }

  if (res != MTLK_ERR_OK)
  {
    ccd->center_freq1 = old_freq1;
    goto end;
  }

  if (cpd) {
    wave_memcpy(cpd->rssi, sizeof(cpd->rssi), req->rssi, sizeof(req->rssi));
    wave_memcpy(cpd->noise, sizeof(cpd->noise), req->noise, sizeof(req->noise));
    wave_memcpy(cpd->rf_gain, sizeof(cpd->rf_gain), req->rf_gain, sizeof(req->rf_gain));
    cpd->rate = MAC_TO_HOST32(req->rate);
  }
  {
    /* A lot of code gets the current chan from the param_db, so set it there, too.  But only for the radio! */
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_CHANNEL_CUR, freq2lowchannum(cd->chan.center_freq, CW_20));
    /* some code will check the bonding for HT, so this is useful; for VHT we'll need more fixes */
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_BONDING_MODE, req->primary_chan_idx);
    /* and let's set this too for old times' sake, but it will have to be called CHANWIDTH eventually */
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_SPECTRUM_MODE, req->chan_width);
    /* and one more superfluous thing because it's obvious from the channel number */
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_FREQ_BAND_CUR, cd->chan.band);

    if (req->chan_width >= CW_80) {
       if (wave_radio_band_get(radio) == MTLK_HW_BAND_6_GHZ) {
          mtlk_core_update_network_mode(core, MTLK_NETWORK_11AX_6G_ONLY);
       }  else {
          mtlk_core_update_network_mode(core, MTLK_NETWORK_11ANAC_MIXED);
       }
    }

    /* succeeded, so remember the current settings */
    __wave_core_chan_switch_type_set(core, req->switch_type);

    ccd->chan = cd->chan; /* struct assignment */
    ccd->width = cd->width;
    ccd->center_freq2 = cd->center_freq2;
    ccd->is_noht = cd->is_noht;
    ccd->sb_dfs.sb_dfs_bw = cd->sb_dfs.sb_dfs_bw;
    ccd->sb_dfs.width = cd->sb_dfs.width;
    ccd->sb_dfs.center_freq = cd->sb_dfs.center_freq;
    if (cpd)
      ccd->power_level = cpd->chandef.power_level;

    wmb(); /* so that the channel is marked "certain" only after we've switched it and recorded everything */

    ccd->center_freq1 = ccd != cd ? cd->center_freq1 : old_freq1;
  }

  if (!skip_set_chan_zwdfs) {
    mtlk_core_cfg_update_antenna_mask(core, req->antennaMask, FALSE);
  }

  /* Set these params after setting operating channel */
  if (req->switch_type == ST_NORMAL) { /* not for CSA */
    (void)mtlk_core_set_coc_actual_power_mode(core);
  } else if ((req->switch_type == ST_SCAN) || (req->switch_type == ST_CSA)) { /* SCAN or CSA */
    (void)mtlk_core_set_coc_pause_power_mode(core);
  }

  (void)_mtlk_core_activate_erp_if_needed(core, req->switch_type);

end:
  for (vap_num = 0; vap_num < max_vaps_count; vap_num++) {
    if (mtlk_vap_manager_get_vap_handle(vap_mgr, vap_num, &vap_handle) != MTLK_ERR_OK) {
      ILOG3_D("Can't get VAP handle with num %d", vap_num);
      continue;
    }
    if (!mtlk_vap_is_ap(vap_handle))
      continue;
    vap_id = mtlk_vap_get_id(vap_handle);
    if (req->csaMcDeauthFramesLocation[vap_id]) {
      /* Unmap the physical address mapped to the DMA address containing the Deauthentication frame.
       */
      mtlk_osal_unmap_phys_addr(mtlk_ccr_get_dev_ctx(mtlk_hw_mmb_get_ccr(hw)),
                                MAC_TO_HOST32(req->csaMcDeauthFramesLocation[vap_id]), req->csaMcDeauthFrameLength[vap_id],
                                MTLK_DATA_TO_DEVICE);
    }
    /* Free the memory alloc'ed for Deauthentication frame.
     */
    if (frame_buf[vap_id]) {
      ILOG2_P("freeing frame @ %p", frame_buf[vap_id]);
      mtlk_osal_mem_free(frame_buf[vap_id]);
    }
  }
  (void)mtlk_core_radio_disable_if_needed(core);

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int core_recovery_cfg_wmm_param_set(mtlk_core_t *core)
{
  return _mtlk_core_cfg_wmm_param_send(core);
}

static BOOL
_wave_dfs_must_switch (mtlk_core_t *master_core, struct mtlk_chan_def *chandef, mtlk_vap_handle_t vap_handle)
{
  return (master_core == mtlk_vap_get_core(vap_handle)) &&
         __mtlk_is_sb_dfs_switch(chandef->sb_dfs.sb_dfs_bw);
}

/* MAC80211: start(), config(), channel_switch(), pre_channel_switc all invoke
 * this function supposed to get done from the Master VAP's serializer
 */
int __MTLK_IFUNC core_cfg_set_chan_clpb (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  struct mtlk_chan_def *ccd;
  struct set_chan_param_data *cpd;
  unsigned cpd_size;
  BOOL paused_scan = FALSE;
  mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
  wave_radio_t  *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_scan_support_t   *ss = __wave_core_scan_support_get(core);
  mtlk_vap_handle_t     vap_handle;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t *) == data_size);

  ccd = __wave_core_chandef_get(core);

  cpd = mtlk_clpb_enum_get_next(clpb, &cpd_size);
  MTLK_CLPB_TRY(cpd, cpd_size)
    /* When in block_tx mode, prevent all channel switches except block_tx off */
    if (core->slow_ctx->is_block_tx == TRUE &&
        ccd->wait_for_beacon == TRUE)
    {
      ILOG0_V("Ignoring channel switches when in block_tx mode");
      MTLK_CLPB_EXIT(MTLK_ERR_BUSY);
    }

    if (MTLK_ERR_OK != mtlk_vap_manager_get_vap_handle(mtlk_df_get_vap_manager(df), cpd->vap_id, &vap_handle)) {
      MTLK_CLPB_EXIT(MTLK_ERR_OK);
    }

    if (cpd->chan_switch_time)
      cpd->switch_type = ST_CSA;
    else if (wave_radio_get_cac_pending(radio))
      cpd->switch_type = ST_SCAN;

    if (!_wave_dfs_must_switch(core, &cpd->chandef, vap_handle)       /* Always must set channel after radar detected */
        && ccd->chan.center_freq                                      /* we already have a channel set */
        && cpd->switch_type == __wave_core_chan_switch_type_get(core) /* with the same switch type as the new request */
        && cpd->chandef.power_level == ccd->power_level               /* power level as set by kernel (mac80211) */
        && mtlk_core_is_chandef_identical(&cpd->chandef, ccd))        /* and all the channel params match */
    {
      unsigned low_chan_num;
      unsigned primary_chan_num;
      unsigned primary_chan_idx;
      low_chan_num = freq2lowchannum(_wave_get_cf1_or_cf2((const struct mtlk_chan_def *)ccd),
                                     ccd->width);
      primary_chan_num = ieee80211_frequency_to_channel(ccd->chan.center_freq);
      primary_chan_idx = (primary_chan_num - low_chan_num) / CHANNUMS_PER_20MHZ;
      ccd->is_noht = cpd->chandef.is_noht;

      ILOG1_DDDD("Avoiding setting channel %u--%u (width %u, primary at %u) that is already set",
                 low_chan_num, low_chan_num + (CHANNUMS_PER_20MHZ << ccd->width) - CHANNUMS_PER_20MHZ,
                 (MHZS_PER_20MHZ << ccd->width), low_chan_num + CHANNUMS_PER_20MHZ * primary_chan_idx);

      MTLK_CLPB_EXIT(res);
    }

    if (mtlk_core_is_band_supported(core, cpd->chandef.chan.band) != MTLK_ERR_OK)
    {
      ELOG_D("HW does not support band %i", cpd->chandef.chan.band);
      MTLK_CLPB_EXIT(MTLK_ERR_PARAMS);
    }

    if (is_scan_running(ss))
    {
      if (!(ss->flags & SDF_BACKGROUND))
      {
        ELOG_V("Cannot change channels in the middle of a non-BG scan");
        MTLK_CLPB_EXIT(MTLK_ERR_BUSY);
      }
      else if (!(ss->flags & SDF_BG_BREAK)) /* background scan and not on a break at the moment */
      {
        ILOG0_V("Background scan encountered, so pausing it first");

        if ((res = pause_or_prevent_scan(core)) != MTLK_ERR_OK)
        {
          ELOG_V("Scan is already paused/prevented, canceling the channel switch");
          MTLK_CLPB_EXIT(res);
        }

        paused_scan = TRUE;
      }
      else
        ILOG0_V("Background scan during its break encountered, so changing the channel");
    }

    /* don't allow to set 2.4GHz channel's width to 40 MHz if Multi-Radio Co-Existence is enabled */
    core_cfg_change_chan_width_if_coex_en(core, &cpd->chandef);
    /* set channel */
    res = core_cfg_set_chan(core, &cpd->chandef, cpd);

    if (res != MTLK_ERR_OK) {
      ccd->wait_for_beacon = TRUE;
      MTLK_CLPB_EXIT(res);
    }

    if (ss->dfs_debug_params.debug_chan && (cpd->switch_type == ST_SCAN))
      ss->dfs_debug_params.switch_in_progress = FALSE;

    if (cpd->switch_type == ST_NORMAL &&
        core->slow_ctx->is_block_tx) {
      ILOG0_V("Exited block_tx mode successfully");
      core->slow_ctx->is_block_tx = FALSE;
    }

    if (is_scan_running(ss))
      /* save what we just set so the scan can return to it when it finishes */
      ss->orig_chandef = *ccd;

  MTLK_CLPB_FINALLY(res)
    if (paused_scan)
    {
      ILOG0_V("Resuming the paused scan");
      resume_or_allow_scan(core);
    }
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

/* PHY_RX_STATUS */
void hw_on_set_chan(mtlk_hw_t *hw, uint32 ch_width);

/* cpd may be NULL for normal and scan switches; only needed for CSA */
int __MTLK_IFUNC core_cfg_set_chan (mtlk_core_t *core, const struct mtlk_chan_def *cd, struct set_chan_param_data *cpd)
{
  wave_radio_t  *radio;
  uint32 param_algomask;
  uint32 param_oalgomask;
  int power_level;
  int res = MTLK_ERR_OK;
  mtlk_country_code_t country_code;

  MTLK_ASSERT(core == mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle)));

  radio = wave_vap_radio_get(core->vap_handle);
  param_algomask = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_CALIBRATION_ALGO_MASK);
  param_oalgomask = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_ONLINE_ACM);

  if (!is_channel_certain(cd)) {
    ELOG_V("Channel is not set");
    return MTLK_ERR_NOT_SUPPORTED;
  }

  if (cpd)
    /* use power level set by kernel */
    power_level = cpd->chandef.power_level;
  else if (mtlk_core_rcvry_is_running(core) && cd->power_level != 0)
    /* Recovery, use last power_level configuration */
    power_level = cd->power_level;
  else
    /* Driver decides which TX power to use (scan, AOCS)*/
    power_level = MTLK_POWER_NOT_SET;

  ILOG2_DDDDD("band=%u, center_freq=%hu, flags=0x%08x, nl_width=%u, power_level=%d",
             cd->chan.band, cd->chan.center_freq, cd->chan.flags, cd->width,
             power_level);

  /* may not do set_chan unless there has been a VAP activated */
  if (core->is_stopped &&
      (res = mtlk_mbss_send_vap_activate(core, wave_radio_band_get(radio))) != MTLK_ERR_OK)
  {
    ELOG_D("Could not activate the master VAP, err=%i", res);
    return res;
  }

  core_cfg_country_code_get(core, &country_code);

  /* set TX power limits */
  core_cfg_set_tx_power_limit(core, _wave_get_cf1_or_cf2(cd), cd->width,
                              cd->chan.center_freq, country_code, power_level);

  res = core_cfg_send_set_chan(core, cd, cpd);

  if (res == MTLK_ERR_OK && cpd) {
    /* Store channel parameters for Recovery purpose */
    wave_rcvry_chanparam_set(mtlk_vap_get_hw(core->vap_handle),
                             mtlk_vap_get_manager(core->vap_handle), (mtlk_handle_t*)cpd);
  }

  if (cpd && cpd->switch_type == ST_CSA)
  { /* restore default threshold, if adaptive */
    int i;
    iwpriv_cca_th_t    cca_th_params;
    iwpriv_cca_adapt_t cca_adapt_params;
    mtlk_pdb_size_t    cca_adapt_size = sizeof(cca_adapt_params);

    if (MTLK_ERR_OK == WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_CCA_ADAPT, &cca_adapt_params, &cca_adapt_size)) {
      if (cca_adapt_params.initial)
      {
        /* read user config */
        if (MTLK_ERR_OK != wave_radio_cca_threshold_get(radio, &cca_th_params)) {
          return MTLK_ERR_PARAMS;
        }

        for (i = 0; i < MTLK_CCA_TH_PARAMS_LEN; i++)
          core->slow_ctx->cca_adapt.cca_th_params[i] = cca_th_params.values[i];

        /* CCA threshold is sent just after set channel */
        /* mtlk_core_cfg_send_cca_threshold_req(core, &cca_th_params); */
      }
    }
  }

  return res;
}


/* If a "set-channel process" is still going on in FW
 * (started by any set-channel CMD and finished by the CFM of the closest set-channel that has
 * either type NORMAL or type CSA and block_tx_post=FALSE) then we have to finish it before starting any
 * other "process" (such as remove_vap or set_bss).  But we only ever use block_tx_post=FALSE so the only
 * way to leave the "set-channel process" open is by doing switch_type=SCAN.
 * This must be run on the MASTER CORE'S SERIALIZER.
 */
int finish_fw_set_chan_if_any(mtlk_core_t *core)
{
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(core == mtlk_core_get_master(core));

  if (mtlk_core_is_in_scan_mode(core) || mtlk_core_is_chan_switch_type_csa_with_block_tx(core))
  {
    struct mtlk_chan_def *ccd = __wave_core_chandef_get(core);

    /* we're in the master serializer and not ourselves changing the channel,
     * so it must be clear and certain, unless it was never set outside the scanning
     * (in which case it also gets restored uncertain after the scan).
     * In this latter weird case we must set it to just about anything valid.
     * But what might be valid?  The type is left as SCAN, so the other fields
     * except center_freq1 must be valid for 20MHz channels. So we copy chan's center_freq
     * to have a valid thing and then proceed as we would otherwise, i.e.,
     * run the official set_chan to have a NORMAL switch type performed and recorded
     */
    if (!is_channel_certain(ccd))
      ccd->center_freq1 = ccd->chan.center_freq;

    ILOG0_V("Finishing a FW set-chan process");
    /* call the almost fullest set-chan machinery to do all the work for us; it will use switch_type NORMAL */
    res = core_cfg_set_chan(core, ccd, NULL);

    if (res != MTLK_ERR_OK)
      return res;

    /* double check it worked */
    MTLK_ASSERT(is_channel_certain(ccd) && __wave_core_chan_switch_type_get(core) == ST_NORMAL);
  }

  return res;
}

/*
 * The last switch_type can be SCAN either because a scan is running or because it finished but no
 * real channel was set because there was none before the scan.  If scan is running we can't just go messing
 * with the currently-set channel.  Even if it is not running, it may start in master core at just about any time.
 * So what we do is this:
 * If we already are in the master serializer then a new scan cannot cut in on us,
 * so we just call the above function to do a NORMAL set-chan in case current type is SCAN.
 * If we are not in the master serializer, then we need to arrange that the master serializer pauses-or-prevents
 * any scan that is going on there or might be starting any time now. After this the FW set-channel process
 * can be closed if necessary (i.e., if last switch_type stayed as SCAN) but still from the master core.
 * So here we just have to enqueue on master-core's serializer that function that will do these two things.
 * core parameter must reflect THE CORE ON WHO'S SERIALIZER WE'RE RUNNING!!!
 */
int finish_and_prevent_fw_set_chan(mtlk_core_t *core, BOOL yes)
{
  mtlk_core_t *mcore = mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle));
  mtlk_clpb_t *clpb = NULL;
  int res;

  ILOG3_SDPPDD("Running on %s (%d): core=%p, mcore=%p, yes=%hhu, chan_switch_type=%hhu",
               current->comm, current->pid, core, mcore, yes, __wave_core_chan_switch_type_get(mcore));

  if (core == mcore)
  {
    /* we're on master core, things are simple, scan cannot start out of nowhere */
    if (yes && (mtlk_core_is_in_scan_mode(mcore) || mtlk_core_is_chan_switch_type_csa_with_block_tx(core)))
      return finish_fw_set_chan_if_any(mcore);

    return MTLK_ERR_OK;
  }

  /* else we need to call a function in master core that would pause-or-prevent a scan and then set NORMAL switch type */
  res = _mtlk_df_user_invoke_core(mtlk_vap_get_df(mcore->vap_handle), WAVE_RADIO_REQ_FIN_PREV_FW_SC, &clpb, &yes, sizeof(yes));
  res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_RADIO_REQ_FIN_PREV_FW_SC, TRUE);

  return res;
}

/* must be run on the master serializer */
/* see the comments of the previous function to learn why we have it */
int __MTLK_IFUNC finish_and_prevent_fw_set_chan_clpb (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  BOOL *yes;
  uint32 yes_size;
  int res;

  MTLK_ASSERT(!in_atomic());
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  yes = mtlk_clpb_enum_get_next(clpb, &yes_size);
  MTLK_CLPB_TRY(yes, yes_size)
    if (*yes)
    {
      res = pause_or_prevent_scan(core);

      if (res != MTLK_ERR_OK)
        MTLK_CLPB_EXIT(res);

      res = finish_fw_set_chan_if_any(core);
    }
    else
      res = resume_or_allow_scan(core);
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
core_cfg_internal_request_sid (mtlk_core_t *nic, IEEE_ADDR *addr, uint16 *p_sid)
{
  mtlk_vap_handle_t vap_handle;
  unsigned          oid;
  mtlk_error_t      res;
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_REQUEST_SID  *umi_req_sid;

  MTLK_ASSERT(NULL != nic);
  MTLK_ASSERT(NULL != p_sid);
  MTLK_ASSERT(NULL != addr);
  if(!nic || !addr || !p_sid)
    return MTLK_ERR_PARAMS;

  vap_handle = nic->vap_handle;
  oid = mtlk_vap_get_oid(vap_handle);

  /* prepare the msg to the FW */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), &res);
  if (!man_entry) {
    ELOG_DD("CID-%04x: UM_MAN_REQUEST_SID_REQ init failed, err=%i", oid, res);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_REQUEST_SID_REQ;
  man_entry->payload_size = sizeof(UMI_REQUEST_SID);
  umi_req_sid = (UMI_REQUEST_SID *) man_entry->payload;
  memset(umi_req_sid, 0, sizeof(UMI_REQUEST_SID));
  umi_req_sid->sAddr = *addr;

  mtlk_dump(2, umi_req_sid, sizeof(UMI_REQUEST_SID), "dump of UMI_REQUEST_SID before submitting to FW:");

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  mtlk_dump(2, umi_req_sid, sizeof(UMI_REQUEST_SID), "dump of UMI_REQUEST_SID after submitting to FW:");

  if (res != MTLK_ERR_OK) {
      ELOG_DD("CID-%04x: UM_MAN_REQUEST_SID_REQ send failed, err=%i", oid, res);
  } else if (umi_req_sid->u8Status) {
    ELOG_DD("CID-%04x: UM_MAN_REQUEST_SID_REQ execution failed, status=%hhu", oid, umi_req_sid->u8Status);
    res = MTLK_ERR_MAC;
  }

  if (res == MTLK_ERR_OK) {
    wave_sid_t sid = MAC_TO_HOST16(umi_req_sid->u16SID);
    ILOG1_DDY("CID-%04x: SID %u is requested for STA %Y", oid, sid, addr->au8Addr);
    res = wave_hw_set_sid_in_use(mtlk_vap_get_hw(vap_handle), sid, mtlk_vap_get_id_fw(vap_handle));
    if (res == MTLK_ERR_OK) {
      *p_sid = sid;
    } else {
      /* a possible error can be handled somehow later */
    }
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

static BOOL _core_cfg_check_sid_connected (mtlk_core_t *nic, IEEE_ADDR *mac_addr)
{
  mtlk_core_t *sta_core;

  if (mtlk_vap_manager_is_sta_connected(nic, &sta_core, (char *)mac_addr)) {
    ELOG_DYSD("CID-%04x: STA %Y already connected to %s (CID-%04x)!",
              mtlk_vap_get_oid(nic->vap_handle),
              mac_addr->au8Addr, mtlk_vap_is_ap(sta_core->vap_handle) ? "AP-VAP" : "STA-VAP",
              mtlk_vap_get_oid(sta_core->vap_handle));
    return TRUE;
  }
  return FALSE;
}

int __MTLK_IFUNC core_cfg_request_sid (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *nic = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  wave_radio_t *radio;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  uint16 sid, max_stas, sta_cnt;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);
  MTLK_CLPB_TRY(addr, addr_size)
    if (mtlk_vap_is_ap(nic->vap_handle) && !wave_core_sync_hostapd_done_get(nic)) {
      ILOG1_D("CID-%04x: HOSTAPD STA database is not synced", mtlk_vap_get_oid(nic->vap_handle));
      MTLK_CLPB_EXIT(MTLK_ERR_NOT_READY);
    }
    if (mtlk_core_rcvry_is_running(nic))
    {
      ILOG1_D("CID-%04x: Recovery is running", mtlk_vap_get_oid(nic->vap_handle));
      MTLK_CLPB_EXIT(MTLK_ERR_NOT_READY);
    }

    radio = wave_vap_radio_get(nic->vap_handle);
    max_stas = wave_radio_max_stas_get(radio);
    sta_cnt = wave_radio_sta_cnt_get(radio);
    if (sta_cnt >= max_stas) {
      ILOG0_DD("Cannot request SID as sta_cnt (%d) max_stas (%d)", sta_cnt, max_stas);
      MTLK_CLPB_EXIT(MTLK_ERR_PARAMS);
    }

    /* Check that station is not connected */
    if (_core_cfg_check_sid_connected(nic, addr)) {
      MTLK_CLPB_EXIT(MTLK_ERR_ALREADY_EXISTS);
    }

    res = core_cfg_internal_request_sid(nic, addr, &sid);
    if (res == MTLK_ERR_OK) {
      mtlk_hw_clean_sta_statistics_by_sid(mtlk_vap_get_hw(nic->vap_handle), sid);
    }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &sid, sizeof(sid));
  MTLK_CLPB_END
}

int __MTLK_IFUNC core_cfg_remove_sid (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res;

  uint16 *sid;
  uint32 sid_size;

  mtlk_core_t *nic = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  sid = mtlk_clpb_enum_get_next(clpb, &sid_size);
  MTLK_CLPB_TRY(sid, sid_size)
    if (mtlk_vap_is_ap(nic->vap_handle) && !wave_core_sync_hostapd_done_get(nic)) {
      ILOG1_D("CID-%04x: HOSTAPD STA database is not synced", mtlk_vap_get_oid(nic->vap_handle));
      MTLK_CLPB_EXIT(MTLK_ERR_NOT_READY);
    }
    /* Don't send any message to halted MAC */
    if (NET_STATE_HALTED == mtlk_core_get_net_state(nic)) {
      MTLK_CLPB_EXIT(MTLK_ERR_OK);
    }
    res = core_remove_sid(nic, *sid);

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

mtlk_error_t __MTLK_IFUNC
core_cfg_remove_all_sids_if_needed (mtlk_core_t *core)
{
  mtlk_hw_t        *hw;
  mtlk_vap_handle_t vap_handle;
  wave_vap_id_t     vap_id_fw;
  wave_sid_t        sid;
  BOOL              is_halted;
  mtlk_error_t      res = MTLK_ERR_OK;

  MTLK_ASSERT(core);
  if (!core)
    return MTLK_ERR_PARAMS;

  vap_handle = core->vap_handle;
  vap_id_fw = mtlk_vap_get_id_fw(vap_handle);
  hw = mtlk_vap_get_hw(vap_handle);

  /* Don't send any message to halted MAC */
  is_halted = (NET_STATE_HALTED == mtlk_core_get_net_state(core));
  if (is_halted) {
    for (sid = 0; sid < WAVE_MAX_SID; sid++) {
      (void)wave_hw_clear_sid_in_use(hw, sid, vap_id_fw, FALSE /* don't warn */);
    }
    return MTLK_ERR_OK;
  }

  for (sid = 0; sid < WAVE_MAX_SID; sid++) {
    if (wave_hw_is_sid_in_use(hw, sid, vap_id_fw)) {
      res = core_remove_sid(core, sid);
      if (res != MTLK_ERR_OK) {
        ELOG_DD("Remove SID request failed for %u, error code: %d", sid, res);
        break;
      }
    }
  }

  return res;
}

int core_cfg_ap_disconnect_all (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32               res = MTLK_ERR_OK;
  mtlk_core_t          *nic = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t          *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_ap(nic->vap_handle));
  MTLK_UNREFERENCED_PARAM(data);
  MTLK_UNREFERENCED_PARAM(data_size);

  if (!wave_core_sync_hostapd_done_get(nic)) {
    ILOG1_D("CID-%04x: HOSTAPD STA database is not synced", mtlk_vap_get_oid(nic->vap_handle));
    res = MTLK_ERR_NOT_READY;
    goto FINISH;
  }

  res = core_ap_disconnect_all(nic);

FINISH:
  return mtlk_clpb_push(clpb, &res, sizeof(res));
}

BOOL core_cfg_mbss_check_activation_params (struct nic *nic)
{
  BOOL res = FALSE;
  mtlk_pdb_size_t essid_len;
  char essid[MAX_SSID_LEN];

  MTLK_ASSERT(NULL != nic);
  essid_len = sizeof(essid);

  if (MTLK_ERR_OK != MTLK_CORE_PDB_GET_BINARY(nic, PARAM_DB_CORE_ESSID, essid, &essid_len)) {
    ELOG_D("CID-%04x: ESSID parameter has wrong length", mtlk_vap_get_oid(nic->vap_handle));
    goto FINISH;
  }

  if (0 == essid_len) {
    ELOG_D("CID-%04x: ESSID is not set", mtlk_vap_get_oid(nic->vap_handle));
    goto FINISH;
  }

  res = TRUE;

FINISH:
  return res;
}

int core_cfg_set_mac_addr (mtlk_core_t *nic, const char *mac)
{
  /* Validate MAC address */
  if (!mtlk_osal_is_valid_ether_addr(mac)) {
    ELOG_DY("CID-%04x: The MAC %Y is invalid", mtlk_vap_get_oid(nic->vap_handle), mac);
    return MTLK_ERR_PARAMS;
  }

  wave_pdb_set_mac(mtlk_vap_get_param_db(nic->vap_handle), PARAM_DB_CORE_MAC_ADDR, mac);
  ILOG1_DY("CID-%04x: New MAC: %Y", mtlk_vap_get_oid(nic->vap_handle), mac);

  return MTLK_ERR_OK;
}

/* Store of Country Code */
static int
_core_cfg_store_country_code (mtlk_core_t *core, const mtlk_country_code_t *country_code)
{
  int res;
  unsigned oid;
  wave_radio_t *radio;

  MTLK_ASSERT(NULL != core);
  MTLK_ASSERT(NULL != country_code);

  radio = wave_vap_radio_get(core->vap_handle);
  MTLK_ASSERT(NULL != radio);

  oid = mtlk_vap_get_oid(core->vap_handle);
  res = wave_radio_country_code_set(radio, country_code);
  if(MTLK_ERR_OK != res) {
    ELOG_D("CID-%04x: Can't update Country Code in PDB", oid);
  } else {
    ILOG0_DS("CID-%04x: Country Code set to: \"%s\"", oid, country_code->country);
  }

  return res;
}

/* Sending of regulatory domain information to firmware */
int
_core_send_reg_domain_config (mtlk_core_t *core)
{
    mtlk_txmm_msg_t            man_msg;
    mtlk_txmm_data_t           *man_entry;
    UMI_REG_DOMAIN_CONFIG      *mac_msg;
    int                        res;
    unsigned                   oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    /* Allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* Fill the message data */
    man_entry->id = UM_MAN_SET_REG_DOMAIN_CONFIG_REQ;
    man_entry->payload_size = sizeof(*mac_msg);
    mac_msg = (UMI_REG_DOMAIN_CONFIG *)man_entry->payload;

    mac_msg->regDomainConfig = (uint8)wave_core_cfg_get_regd_region(core);

    ILOG2_DD("CID-%04x: Regulatory domain in %u region", oid, mac_msg->regDomainConfig);

    /* Send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
    if (res != MTLK_ERR_OK || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set UM_MAN_SET_REG_DOMAIN_CONFIG_REQ failed, res=%d status=%hhu",
              oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* Cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}


/* Processing of Country Code */
int __MTLK_IFUNC
core_cfg_country_code_set (mtlk_core_t *core, const mtlk_country_code_t *country_code)
{
  int res;
  res = _core_cfg_store_country_code(core, country_code);

  if(MTLK_ERR_OK == res)
    res = _core_send_reg_domain_config(core);

  return res;
}

int __MTLK_IFUNC
core_cfg_country_code_set_by_str (mtlk_core_t *core, char *str, int len)
{
  mtlk_country_code_t   country_code;

  MTLK_ASSERT(NULL != core);
  MTLK_ASSERT(NULL != str);

  /* Input string may be NOT NUL-terminated, e.g. alpha[2] */
  /* Prepare a NUL-terminated string */
  wave_strncopy(country_code.country, str, sizeof(country_code.country), len);

  return core_cfg_country_code_set(core, &country_code);
}

/* Processing of required initial data (from hostapd) */
int __MTLK_IFUNC
core_cfg_set_hostapd_initial_data (mtlk_core_t *core, struct intel_vendor_initial_data_cfg* data)
{
  uint8 band, net_mode;
  MTLK_ASSERT(NULL != core);

  /* Required for sending tcp_config to FW before channel switch */
  if (data->is_11b) {
    net_mode = MTLK_NETWORK_11B_ONLY;
  }
  else { /* OFDM modulation*/
    band = core_cfg_get_freq_band_cfg(core);
    net_mode = get_net_mode(band, TRUE);
  }

  mtlk_core_update_network_mode(core, net_mode);

  return core_cfg_country_code_set_by_str(core, data->alpha2, sizeof(data->alpha2));
}

void __MTLK_IFUNC
core_cfg_country_code_get (mtlk_core_t *core, mtlk_country_code_t *country_code)
{
  wave_radio_t *radio;
  mtlk_pdb_size_t   size = sizeof(*country_code);

  MTLK_ASSERT(NULL != core);
  MTLK_ASSERT(NULL != country_code);

  radio = wave_vap_radio_get(core->vap_handle);
  MTLK_ASSERT(NULL != radio);

  WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_COUNTRY_CODE_ALPHA, country_code, &size);
  country_code->country[sizeof(country_code->country) - 1] = '\0';
}

uint32 __MTLK_IFUNC
core_cfg_get_regd_code (mtlk_core_t *core)
{
  mtlk_country_code_t   country_code;

  core_cfg_country_code_get(core, &country_code);

  return mtlk_psdb_country_to_regd_code(country_code.country);
}

uint32 __MTLK_IFUNC
wave_regd_code_to_regd_region (uint32 regd_code)
{
  uint32 res;
  switch (regd_code) {
  case REGD_CODE_FCC:
    res = REG_DOMAIN_FCC;   break;

  case REGD_CODE_ETSI:
  case REGD_CODE_SPAIN:
  case REGD_CODE_FRANCE:
  case REGD_CODE_UAE:
  case REGD_CODE_GERMANY:
    res = REG_DOMAIN_ETSI;  break;

  case REGD_CODE_MKK:
  case REGD_CODE_JAPAN:
    res = REG_DOMAIN_JAPAN;   break;

  case REGD_CODE_CHINA:
    res = REG_DOMAIN_CHINA;   break;

  case REGD_CODE_KOREA:
    res = REG_DOMAIN_KOREA;   break;

  default:
    res = REG_DOMAIN_DEFAULT; break;
  }

  ILOG3_DD("RegDomain code %u -> type %u", regd_code, res);

  return res;
}

uint32 __MTLK_IFUNC
wave_core_cfg_get_regd_region (mtlk_core_t *core)
{
  uint32 regd_code;

  MTLK_ASSERT(core);

  regd_code = core_cfg_get_regd_code(core);

  return wave_regd_code_to_regd_region(regd_code);
}

void __MTLK_IFUNC
core_cfg_country_code_set_default (mtlk_core_t *core)
{
  if (mtlk_vap_is_master(core->vap_handle)) {
    core_cfg_country_code_set_by_str(core, COUNTRY_CODE_DFLT, COUNTRY_CODE_MAX_LEN);
  }
}

uint8 core_cfg_get_freq_band_cur(mtlk_core_t *core)
{
  MTLK_ASSERT(NULL != core);

  return WAVE_RADIO_PDB_GET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_FREQ_BAND_CUR);
}

uint8 core_cfg_get_freq_band_cfg(mtlk_core_t *core)
{
  wave_radio_t *radio;
  MTLK_ASSERT(NULL != core);
  radio = wave_vap_radio_get(core->vap_handle);

  return WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_FREQ_BAND_CFG);
}

uint8 core_cfg_get_network_mode_cur(mtlk_core_t *core)
{
  MTLK_ASSERT(NULL != core);

  return MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_NET_MODE_CUR);
}

uint8 core_cfg_get_network_mode_cfg(mtlk_core_t *core)
{
  MTLK_ASSERT(NULL != core);

  return MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_NET_MODE_CFG);
}

BOOL core_cfg_net_state_is_connected(uint16 net_state)
{
  return ((NET_STATE_CONNECTED == net_state) ? TRUE:FALSE);
}

BOOL core_cfg_is_connected (mtlk_core_t *core)
{
  return core_cfg_net_state_is_connected(mtlk_core_get_net_state(core));
}

BOOL core_cfg_is_halted (mtlk_core_t *core)
{
  return (mtlk_core_get_net_state(core) & NET_STATE_HALTED);
}

uint8 core_cfg_get_network_mode(mtlk_core_t *core)
{
  int net_state;

  MTLK_ASSERT(NULL != core);
  net_state = mtlk_core_get_net_state(core);

  if (net_state == NET_STATE_CONNECTED)
    return core_cfg_get_network_mode_cur(core);
  else
    return core_cfg_get_network_mode_cfg(core);
}

/* In case network mode is 11b, return PSDB_PHY_CW_11B. Otherwise convert enum chanWidth to psdb_phy_cw_t. */
static psdb_phy_cw_t _get_phy_cw_mode(mtlk_core_t *core, enum chanWidth cw)
{
    /* Try to detect if network mode is 11B */
    if (MTLK_NETWORK_11B_ONLY == core_cfg_get_network_mode(core))
        return PSDB_PHY_CW_11B;

    switch (cw) {
        case CW_20:
            return PSDB_PHY_CW_OFDM_20;
        case CW_40:
            return PSDB_PHY_CW_OFDM_40;
        case CW_80:
        case CW_80_80:
            return PSDB_PHY_CW_OFDM_80;
        case CW_160:
            return PSDB_PHY_CW_OFDM_160;
        default:
            ELOG_D("chan_width [%d] currently isn't supported.", cw);
            return PSDB_PHY_CW_OFDM_80;
    }
}

static enum chanWidth
_wave_chan_width_validate(enum chanWidth cw)
{
  if (wave_chan_width_is_supported(cw)) {
    return cw;
  } else {
    ELOG_D("Unsupported channel width %d", cw);
    MTLK_ASSERT(FALSE);
    return CW_DEFAULT;
  }
}

void core_cfg_set_tx_power_limit (mtlk_core_t *core, unsigned center_freq,
                                  enum chanWidth cfg_width, unsigned cf_primary,
                                  mtlk_country_code_t req_country_code,
                                  int power_level)
{
  mtlk_df_t *df               = mtlk_vap_get_df(core->vap_handle); /* this checks vap_handle and df, cannot return NULL */
  mtlk_df_user_t *df_user     = mtlk_df_get_user(df);
  struct wireless_dev *wdev   = mtlk_df_user_get_wdev(df_user);   /* this checks df_user */
  wave_radio_t *radio         = wave_vap_radio_get(core->vap_handle);
  mtlk_hw_t *hw               = mtlk_vap_get_hw(core->vap_handle);
  uint16           chip_id    = hw_mmb_get_chip_id(hw);

  uint16                reg_pw_limits[WAVE_BANDWIDTHS_NUM] = { 0 }; /* array per BW */
  uint16                reg_pw_lim = 0;
  psdb_pw_limits_t      tmp_pwl, psd_pwl, cfg_pwl, cfg_oper_pwl;
  mtlk_country_code_t   country_code;
  mtlk_pdb_size_t       pw_limits_size = sizeof(psdb_pw_limits_t);
  psdb_phy_cw_t         phy_cw;
  wave_chan_bonding_t   chan_bonding;
  unsigned              sub_bw; /* subband's width */
  int                   res;
  uint16                power_level_p_unit = DBM_TO_POWER(power_level);
  uint16                freq_l, freq_u, freq_c;

  /* Check Country code */
  core_cfg_country_code_get(core, &country_code); /* read out country code */
  if ((req_country_code.country[0] != country_code.country[0]) ||
      (req_country_code.country[1] != country_code.country[1]) ) {
       ILOG1_SS("req_country_code(%s) is different from current country_code(%s)",
                req_country_code.country, country_code.country);
       return;
  }

  cfg_width = _wave_chan_width_validate(cfg_width);
  /* Static assertions - validate enum oder has not been corrupted */
  MTLK_STATIC_ASSERT((PSDB_PHY_CW_OFDM_20 + 1) == PSDB_PHY_CW_OFDM_40);
  MTLK_STATIC_ASSERT((PSDB_PHY_CW_OFDM_40 + 1) == PSDB_PHY_CW_OFDM_80);
  MTLK_STATIC_ASSERT((PSDB_PHY_CW_OFDM_80 + 1) == PSDB_PHY_CW_OFDM_160);
  MTLK_STATIC_ASSERT(WAVE_BANDWIDTHS_NUM > CW_160);

  phy_cw = _get_phy_cw_mode(core, cfg_width);
  if (!(_chipid_is_gen6_d2_or_gen7(chip_id))) {
    MTLK_ASSERT(phy_cw < ARRAY_SIZE(cfg_pwl.pw_limits));
  }
  ILOG2_DDDD("cw %d, center_freq %u, cf_primary %u -> phy_cw mode %d",
				cfg_width, center_freq, cf_primary, (int)phy_cw);

  if (!(_chipid_is_gen6_d2_or_gen7(chip_id))) {
    memset(&cfg_pwl, 0, sizeof(cfg_pwl));
  }
  memset(&cfg_oper_pwl, 0, sizeof(cfg_oper_pwl));
  MTLK_ASSERT(WAVE_BANDWIDTHS_NUM > cfg_width);
  wave_scan_support_fill_chan_bonding_by_freq(&chan_bonding, center_freq, cf_primary);

  /* Fill data for all subband widths in range [CW_20 ... cfg_width] */
  for (sub_bw = CW_20; sub_bw <= cfg_width; sub_bw++) {
    unsigned ofdm_idx;

    /* read out regulatory power limit for current subband */
    /* Tri band support */

    freq_l = channel_to_frequency(chan_bonding.lower_chan[sub_bw],
                                  wave_radio_band_get(radio));
    freq_u = channel_to_frequency(chan_bonding.upper_chan[sub_bw],
                                  wave_radio_band_get(radio));
    freq_c = channel_to_frequency(chan_bonding.center_chan[sub_bw],
                                  wave_radio_band_get(radio));
    reg_pw_lim = wv_cfg80211_regulatory_limit_get(wdev,
                                                  freq_l,
                                                  freq_u);

    reg_pw_limits[sub_bw] = reg_pw_lim;

    if (_chipid_is_gen6_d2_or_gen7(chip_id))
    {
      if (CW_20 == sub_bw) {
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_11B] = reg_pw_lim;
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_AG_20] = reg_pw_lim;
      }

      if (CW_20 == sub_bw || CW_40 == sub_bw) {
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_N_20 + sub_bw] = reg_pw_lim;
      }
      cfg_oper_pwl.pw_limits[PSDB_PHY_CW_BF_20 + sub_bw] = reg_pw_lim;
      cfg_oper_pwl.pw_limits[PSDB_PHY_CW_BF_AX_20 + sub_bw] = reg_pw_lim;
      cfg_oper_pwl.pw_limits[PSDB_PHY_CW_AX_20 + sub_bw] = reg_pw_lim;
      cfg_oper_pwl.pw_limits[PSDB_PHY_CW_MU_20 + sub_bw] = reg_pw_lim;
      cfg_oper_pwl.pw_limits[PSDB_PHY_CW_OFDM_20 + sub_bw] = reg_pw_lim;


      if ((power_level != MTLK_POWER_NOT_SET) && (power_level != 0))
      {
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_11B] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_11B], power_level_p_unit);
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_AG_20] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_AG_20], power_level_p_unit);
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_N_20] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_N_20], power_level_p_unit);
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_BF_20 + sub_bw] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_BF_20 + sub_bw], power_level_p_unit);
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_BF_AX_20 + sub_bw] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_BF_AX_20 + sub_bw], power_level_p_unit);
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_AX_20 + sub_bw] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_AX_20 + sub_bw], power_level_p_unit);
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_MU_20 + sub_bw] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_MU_20 + sub_bw], power_level_p_unit);
        cfg_oper_pwl.pw_limits[PSDB_PHY_CW_OFDM_20 + sub_bw] =
          hw_mmb_get_tx_power_target(cfg_oper_pwl.pw_limits[PSDB_PHY_CW_OFDM_20 + sub_bw], power_level_p_unit);
      }
    }

    /* get power limits from PSDB for current subband */
    core_get_psdb_hw_limits(core, country_code.country,
                            freq_c, &tmp_pwl);

    if (!(_chipid_is_gen6_d2_or_gen7(chip_id)))
    {

    /* apply regulatory limit to all power limits of current subband */
    /* store tmp_pwl in PSD, apply limit and store result in CFG */
#define _PW_LIMIT_APPLY_(tmp, psd, cfg, idx, reg_lim, kernel_power_lim) \
           psd.pw_limits[idx] = tmp.pw_limits[idx];      \
           cfg.pw_limits[idx] = hw_mmb_get_tx_power_target(tmp.pw_limits[idx], reg_lim); \
           if (power_level != MTLK_POWER_NOT_SET && power_level != 0) \
               cfg.pw_limits[idx] = hw_mmb_get_tx_power_target(cfg.pw_limits[idx], kernel_power_lim);

      /* use CW_20 limit for 11b */
      if (CW_20 == sub_bw) {
        _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, (PSDB_PHY_CW_11B), reg_pw_lim, power_level_p_unit);
        _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, (PSDB_PHY_CW_AG_20), reg_pw_lim, power_level_p_unit);
        ILOG1_DDDD("Updated PSD/CFG pw_limits 11B: %3u %3u 11AG: %3u %3u",
                 psd_pwl.pw_limits[PSDB_PHY_CW_11B], cfg_pwl.pw_limits[PSDB_PHY_CW_11B],
                 psd_pwl.pw_limits[PSDB_PHY_CW_AG_20], cfg_pwl.pw_limits[PSDB_PHY_CW_AG_20]);
      }

      /* use CW_20 and CW_40 limit for 11N */
      if (CW_20 == sub_bw || CW_40 == sub_bw) {
        _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, (PSDB_PHY_CW_N_20   + sub_bw), reg_pw_lim, power_level_p_unit);
        ILOG1_DDD("Updated PSD/CFG pw_limits 11N bw%u: %3u %3u",
                 sub_bw, psd_pwl.pw_limits[PSDB_PHY_CW_N_20 + sub_bw], cfg_pwl.pw_limits[PSDB_PHY_CW_11B]);
      }

      _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, (PSDB_PHY_CW_BF_20   + sub_bw), reg_pw_lim, power_level_p_unit);
      _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, (PSDB_PHY_CW_BF_AX_20   + sub_bw), reg_pw_lim, power_level_p_unit);
      _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, (PSDB_PHY_CW_MU_20   + sub_bw), reg_pw_lim, power_level_p_unit);
      _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, (PSDB_PHY_CW_AX_20   + sub_bw), reg_pw_lim, power_level_p_unit);

      ofdm_idx = PSDB_PHY_CW_OFDM_20 + sub_bw;
      _PW_LIMIT_APPLY_(tmp_pwl, psd_pwl, cfg_pwl, ofdm_idx, reg_pw_lim, power_level_p_unit);

#undef _PW_LIMIT_APPLY_

      ILOG1_DDD("Updated PSD/CFG pw_limits bw%u: %3u %3u",
                sub_bw, psd_pwl.pw_limits[ofdm_idx], cfg_pwl.pw_limits[ofdm_idx]);

      /* we can't send zero value as power limits to FW due to crash */
      /* it is occurred if both REG and PSD power limits are not set */
      if (!cfg_pwl.pw_limits[ofdm_idx]) {
        ELOG_DD("PSD/REG power limits are not set! center_freq %u bandwidth %u", center_freq, sub_bw);
        MTLK_ASSERT(FALSE);
        return;
      }
    }
  }

  if (_chipid_is_gen6_d2_or_gen7(chip_id))
  {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_TPC_RESERVED, 0);
    res = mtlk_reload_tpc_wrapper(core, ieee80211_frequency_to_channel(center_freq), &cfg_oper_pwl);
    if (MTLK_ERR_OK != res) {
      ELOG_V("Failed to reload TPC wrapper on operational mode");
    }
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_TX_POWER_CFG, cfg_oper_pwl.pw_limits[phy_cw]); /* power units */
  }
  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_TX_POWER_REG_LIM, reg_pw_lim);
  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_TX_POWER_PSD, psd_pwl.pw_limits[phy_cw]); /* power units */

  /* Store values of tx power PSD limits of 11b and non-11b (OFDM) phy modes.
   * They are not depended of current value of Tx Power PSD in parameters DB. */
  res = WAVE_RADIO_PDB_SET_BINARY(radio, PARAM_DB_RADIO_TPC_PW_LIMITS_PSD, &psd_pwl, pw_limits_size); /* power units */
  if (res != MTLK_ERR_OK)
  {
    ELOG_D("Can't update PSD power limits in PDB, res: %d", res);
    /* MTLK_ASSERT(FALSE); */
  }

  if (_chipid_is_gen6_d2_or_gen7(chip_id))
  {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_TPC_RESERVED, 1);
  }

  if (!(_chipid_is_gen6_d2_or_gen7(chip_id))) {
  /* call TPC wrapper */
    res = mtlk_reload_tpc_wrapper(core, ieee80211_frequency_to_channel(center_freq), &cfg_pwl);
    if (MTLK_ERR_OK != res) {
      ELOG_D("Failed to reload TPC wrapper, res: %d", res);
    } else {
      /* Store value of current tx power limit (it depends from network mode and channel width)
       * as a current value of Tx Power in parameters DB */
      WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_TX_POWER_CFG, cfg_pwl.pw_limits[phy_cw]); /* power units */
    }
  }
}

int __MTLK_IFUNC
mtlk_core_add_four_addr_entry (mtlk_core_t* nic, const IEEE_ADDR *addr)
{
  ieee_addr_entry_t *ieee_addr_entry = NULL;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  ILOG2_DY("CID-%04x: four address list entry add %Y",
    mtlk_vap_get_oid(nic->vap_handle), addr->au8Addr);

  if (mtlk_vap_is_ap(nic->vap_handle) &&
      wds_sta_is_peer_ap(&nic->slow_ctx->wds_mng, addr))
    return MTLK_ERR_PROHIB;

  ieee_addr_entry = mtlk_osal_mem_alloc(sizeof(*ieee_addr_entry),
    LQLA_MEM_TAG_FOUR_ADDR_LIST);
  if (!ieee_addr_entry) {
    WLOG_V("Can't alloc four address station list entry");
    return MTLK_ERR_NO_MEM;
  }
  memset(ieee_addr_entry, 0, sizeof(*ieee_addr_entry));

  mtlk_osal_lock_acquire(&nic->four_addr_sta_list.ieee_addr_lock);
  h = mtlk_hash_insert_ieee_addr(&nic->four_addr_sta_list.hash, addr,
    &ieee_addr_entry->hentry);
  mtlk_osal_lock_release(&nic->four_addr_sta_list.ieee_addr_lock);
  if (h) {
    mtlk_osal_mem_free(ieee_addr_entry);
    WLOG_DY("CID-%04x: %Y already in four address station list",
      mtlk_vap_get_oid(nic->vap_handle), addr->au8Addr);
    return MTLK_ERR_OK;
  }
  ILOG2_DY("CID-%04x: four address station list entry %Y added",
    mtlk_vap_get_oid(nic->vap_handle), addr->au8Addr);

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_core_del_four_addr_entry (mtlk_core_t *nic, const IEEE_ADDR *addr)
{
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  ILOG2_DY("CID-%04x: four address list entry delete %Y",
    mtlk_vap_get_oid(nic->vap_handle), addr->au8Addr);
  mtlk_osal_lock_acquire(&nic->four_addr_sta_list.ieee_addr_lock);

  /* find four address station list entry in four address station list */
  h = mtlk_hash_find_ieee_addr(&nic->four_addr_sta_list.hash, addr);
  if (h) {
    ieee_addr_entry_t *ieee_addr_entry = MTLK_CONTAINER_OF(h,
      ieee_addr_entry_t, hentry);
    mtlk_hash_remove_ieee_addr(&nic->four_addr_sta_list.hash,
      &ieee_addr_entry->hentry);
    mtlk_osal_mem_free(ieee_addr_entry);
  } else {
    WLOG_DY("CID-%04x: Can't remove four address station list entry %Y. "
      "It doesn't exist", mtlk_vap_get_oid(nic->vap_handle), addr->au8Addr);
  }
  mtlk_osal_lock_release(&nic->four_addr_sta_list.ieee_addr_lock);

  return MTLK_ERR_OK;
}

void core_cfg_flush_four_addr_list (mtlk_core_t *nic)
{
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&nic->four_addr_sta_list.ieee_addr_lock);
  h = mtlk_hash_enum_first_ieee_addr(&nic->four_addr_sta_list.hash, &e);
  while (h) {
    ieee_addr_entry_t *entry = MTLK_CONTAINER_OF(h, ieee_addr_entry_t,
      hentry);

    mtlk_hash_remove_ieee_addr(&nic->four_addr_sta_list.hash,
      &entry->hentry);
    mtlk_osal_mem_free(entry);

    h = mtlk_hash_enum_next_ieee_addr(&nic->four_addr_sta_list.hash, &e);
  }
  mtlk_osal_lock_release(&nic->four_addr_sta_list.ieee_addr_lock);
}

BOOL core_cfg_four_addr_entry_exists (mtlk_core_t *nic,
  const IEEE_ADDR *addr)
{
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  ILOG3_DY("CID-%04x: looking for four address station list entry %Y",
    mtlk_vap_get_oid(nic->vap_handle), addr->au8Addr);

  mtlk_osal_lock_acquire(&nic->four_addr_sta_list.ieee_addr_lock);
  h = mtlk_hash_find_ieee_addr(&nic->four_addr_sta_list.hash, addr);
  mtlk_osal_lock_release(&nic->four_addr_sta_list.ieee_addr_lock);

  if (!h) {
    ILOG3_DY("CID-%04x: four address station list entry NOT found %Y",
             mtlk_vap_get_oid(nic->vap_handle), addr->au8Addr);
  }

  return (h != NULL);
}

BOOL core_cfg_four_addr_list_is_empty (mtlk_core_t *nic)
{
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  mtlk_osal_lock_acquire(&nic->four_addr_sta_list.ieee_addr_lock);
  h = mtlk_hash_enum_first_ieee_addr(&nic->four_addr_sta_list.hash, &e);
  mtlk_osal_lock_release(&nic->four_addr_sta_list.ieee_addr_lock);

  return (h == NULL);
}

static int
_mtlk_core_get_four_addr_sta_vect (mtlk_core_t *nic, mtlk_clpb_t **ppclpb_sta_vect)
{
  int res = MTLK_ERR_OK;
  mtlk_hash_enum_t              e;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;

  *ppclpb_sta_vect = mtlk_clpb_create();
  if (NULL == *ppclpb_sta_vect) {
    ELOG_V("Cannot allocate clipboard object");
    res = MTLK_ERR_NO_MEM;
    goto end;
  }

  mtlk_osal_lock_acquire(&nic->four_addr_sta_list.ieee_addr_lock);
  h = mtlk_hash_enum_first_ieee_addr(&nic->four_addr_sta_list.hash, &e);
  while (h) {
    ILOG3_DY("CID-%04x: four address station list entry %Y",
      mtlk_vap_get_oid(nic->vap_handle), h->key.au8Addr);
    res = mtlk_clpb_push(*ppclpb_sta_vect, &h->key, sizeof(IEEE_ADDR));
    if (MTLK_ERR_OK != res) {
      ELOG_V("Cannot push data to the clipboard");
      goto err_push_data;
    }

    h = mtlk_hash_enum_next_ieee_addr(&nic->four_addr_sta_list.hash, &e);
  }
  goto no_err;

err_push_data:
  mtlk_clpb_delete(*ppclpb_sta_vect);
  *ppclpb_sta_vect = NULL;

no_err:
  mtlk_osal_lock_release(&nic->four_addr_sta_list.ieee_addr_lock);

end:
  return res;
}

int __MTLK_IFUNC
_mtlk_core_cfg_create_sec_vap(mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *master_core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_create_secondary_vap_cfg_t *create_sec_vap_cfg = NULL;
  mtlk_vap_manager_t *vap_manager;
  mtlk_df_user_t *df_user;
  uint32 create_sec_vap_cfg_size;
  mesh_mode_t mesh_mode;

  MTLK_ASSERT(master_core != NULL);
  MTLK_ASSERT(master_core == mtlk_core_get_master(master_core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  create_sec_vap_cfg = mtlk_clpb_enum_get_next(clpb, &create_sec_vap_cfg_size);
  MTLK_CLPB_TRY(create_sec_vap_cfg, create_sec_vap_cfg_size)

    vap_manager = mtlk_vap_get_manager(master_core->vap_handle);
    MTLK_ASSERT(vap_manager == mtlk_vap_get_manager(create_sec_vap_cfg->vap_handle));

    mesh_mode = MTLK_CORE_PDB_GET_INT(mtlk_vap_get_core(create_sec_vap_cfg->vap_handle), PARAM_DB_CORE_MESH_MODE);
    if (mesh_mode != MESH_MODE_BACKHAUL_AP && mesh_mode != MESH_MODE_EXT_HYBRID) {
      ELOG_D("Wrong mesh mode: %d", mesh_mode);
      MTLK_CLPB_EXIT(MTLK_ERR_PARAMS);
    }

    df_user = mtlk_df_get_user(mtlk_vap_get_df(create_sec_vap_cfg->vap_handle));

    {
      uint32 _vap_index;
      const unsigned char *_addr;
      const char *_name = create_sec_vap_cfg->ifname;

      res = mtlk_vap_manager_get_free_vap_index(vap_manager, &_vap_index);
      if (MTLK_ERR_OK != res) {
        ELOG_V("No free slot for secondary VAP");
        mtlk_vap_del_all_secondary_vaps(create_sec_vap_cfg->vap_handle);
        MTLK_CLPB_EXIT(res);
      }

      _addr = mtlk_df_user_get_addr(df_user);
      res = mtlk_vap_create_secondary_vap(create_sec_vap_cfg->vap_handle,
                                          _addr, _name, _vap_index);
      if (MTLK_ERR_OK != res) {
        ELOG_V("Creating secondary VAP failed");
        mtlk_vap_del_all_secondary_vaps(create_sec_vap_cfg->vap_handle);
        MTLK_CLPB_EXIT(res);
      }
    }

  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC core_cfg_set_four_addr_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_four_addr_cfg_t *four_addr_cfg = NULL;
    uint32 four_addr_cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get configuration */
    four_addr_cfg = mtlk_clpb_enum_get_next(clpb, &four_addr_cfg_size);
    MTLK_CLPB_TRY(four_addr_cfg, four_addr_cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
        MTLK_CFG_CHECK_ITEM_AND_CALL(four_addr_cfg, addr_add, mtlk_core_add_four_addr_entry,
                            (core, &four_addr_cfg->addr_add), res);
        MTLK_CFG_CHECK_ITEM_AND_CALL(four_addr_cfg, addr_del, mtlk_core_del_four_addr_entry,
                            (core, &four_addr_cfg->addr_del), res);
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      /* push result into clipboard */
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

/* Start of Static Planner processing: set config */

static int
_mtlk_core_send_static_plan_config (mtlk_core_t *core, const UMI_STATIC_PLAN_CONFIG *req)
{
    mtlk_txmm_msg_t         man_msg;
    mtlk_txmm_data_t       *man_entry;
    UMI_STATIC_PLAN_CONFIG *mac_msg;
    int                     res;
    unsigned                oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    ILOG0_D("CID-%04x: StaticPlanConfig", oid);

    /* Allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* Fill the message data */
    man_entry->id = UM_MAN_STATIC_PLAN_CONFIG_REQ;
    man_entry->payload_size = sizeof(*mac_msg);
    mac_msg = (UMI_STATIC_PLAN_CONFIG *)man_entry->payload;
    wave_memcpy(mac_msg, sizeof(*mac_msg), req, sizeof(*mac_msg));

    /* Send the message to FW */
    mtlk_dump(1, mac_msg, sizeof(*mac_msg), "dump of StaticPlanConfig");
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (res != MTLK_ERR_OK || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set send UM_MAN_STATIC_PLAN_CONFIG_REQ failed, res=%d status=%hhu",
              oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* Cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_static_plan (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_static_plan_cfg_t *static_plan_cfg = NULL;
    uint32 cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get configuration */
    static_plan_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
    MTLK_CLPB_TRY(static_plan_cfg, cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
         MTLK_CFG_CHECK_ITEM_AND_CALL(static_plan_cfg, config, _mtlk_core_send_static_plan_config,
                                      (core, &static_plan_cfg->config), res);
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      return mtlk_clpb_push_res(clpb, res); /* push result into clipboard */
    MTLK_CLPB_END
}

/* End of Static Planner processing */

int __MTLK_IFUNC
core_cfg_get_four_addr_sta_list (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_four_addr_cfg_t four_addr_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&four_addr_cfg, 0, sizeof(four_addr_cfg));

  MTLK_CFG_SET_ITEM_BY_FUNC(&four_addr_cfg, sta_vect,
    _mtlk_core_get_four_addr_sta_vect, (core, &four_addr_cfg.sta_vect), res);

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &four_addr_cfg, sizeof(four_addr_cfg));
  }

  return res;
}

/*------ CCA threshold ----------------------------------------------*/

int mtlk_core_cfg_send_cca_threshold_req(mtlk_core_t *core, iwpriv_cca_th_t *cca_th_params)
{
  int res;
  mtlk_txmm_msg_t           man_msg;
  mtlk_txmm_data_t          *man_entry = NULL;
  UMI_CCA_TH                *req = NULL;
  mtlk_vap_handle_t         vap_handle = core->vap_handle;

  MTLK_ASSERT(vap_handle);
  ILOG1_D("CID-%04x: CCA_TH_REQ", mtlk_vap_get_oid(vap_handle));

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_CCA_TH_REQ;
  man_entry->payload_size = sizeof(UMI_CCA_TH);

  req = (UMI_CCA_TH *)(man_entry->payload);
  req->primaryChCCA         = cca_th_params->values[0];
  req->secondaryChCCA       = cca_th_params->values[1];
  req->midPktPrimaryCCA     = cca_th_params->values[2];
  req->midPktSecondary20CCA = cca_th_params->values[3];
  req->midPktSecondary40CCA = cca_th_params->values[4];

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != req->Status) {
    ELOG_DDD("CID-%04x: Set CCA_TH_REQ failure, res=%i status=%hhu", mtlk_vap_get_oid(vap_handle), res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

/* Read CCA Threshold data from PDB */
int mtlk_core_cfg_read_cca_threshold(mtlk_core_t *core, iwpriv_cca_th_t *cca_th_params)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);

  if (MTLK_ERR_OK != wave_radio_cca_threshold_get(radio, cca_th_params)) {
    ELOG_D("CID-%04x: Can't read CCA_TH from PDB", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_PARAMS;
  }

  if (!cca_th_params->is_updated) {
    ILOG1_D("CID-%04x: CCA Threshold is not set", mtlk_vap_get_oid(core->vap_handle));
  }

  return MTLK_ERR_OK;
}

/* Store CCA Threshold in PDB */

static int
_mtlk_core_cfg_store_and_send_cca_threshold(mtlk_core_t *core, iwpriv_cca_th_t *cca_th_params)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  int i, res;

  cca_th_params->is_updated = 1;
  res = wave_radio_cca_threshold_set(radio, cca_th_params);
  if (MTLK_ERR_OK == res) {
    res = mtlk_core_cfg_send_cca_threshold_req(core, cca_th_params);

    for (i = 0; i < MTLK_CCA_TH_PARAMS_LEN; i++)
      core->slow_ctx->cca_adapt.cca_th_params[i] = cca_th_params->values[i];
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_cca_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                    res = MTLK_ERR_OK;
  uint32                 cfg_size;
  mtlk_core_t            *core = (mtlk_core_t*)hcore;
  mtlk_cca_th_cfg_t      *cca_th_cfg = NULL;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  /* get configuration */
  cca_th_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(cca_th_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(cca_th_cfg, cca_th_params,
          _mtlk_core_cfg_store_and_send_cca_threshold, (core, &cca_th_cfg->cca_th_params), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_cca_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_cca_th_cfg_t        cca_th_cfg;
  mtlk_core_t              *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* read config from internal db */
  MTLK_CFG_SET_ITEM_BY_FUNC(&cca_th_cfg, cca_th_params,
    mtlk_core_cfg_read_cca_threshold, (core, &cca_th_cfg.cca_th_params), res);

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &cca_th_cfg, sizeof(cca_th_cfg));
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_send_actual_cca_threshold (mtlk_core_t *core)
{
  iwpriv_cca_th_t   cca_th_params;
  int res;

  res = mtlk_core_cfg_read_cca_threshold(core, &cca_th_params);
  if( MTLK_ERR_OK == res) {
    /* TODO: check if the flag is_updated is needed */
    if (cca_th_params.is_updated) {
      res = mtlk_core_cfg_send_cca_threshold_req(core, &cca_th_params);
    }
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_init_cca_threshold (mtlk_core_t *core)
{
  return mtlk_core_cfg_send_actual_cca_threshold(core);
}

int __MTLK_IFUNC
mtlk_core_cfg_recovery_cca_threshold (mtlk_core_t *core)
{
  return mtlk_core_cfg_send_actual_cca_threshold(core);
}

/* CCA adaptive intervals */
static int _mtlk_core_send_cca_intervals_req (mtlk_core_t *core, iwpriv_cca_adapt_t *cca_adapt_params)
{
  int res;
  mtlk_txmm_msg_t           man_msg;
  mtlk_txmm_data_t          *man_entry = NULL;
  UMI_BEACON_BLOCK_TIMER_INTERVAL *req = NULL;
  mtlk_vap_handle_t         vap_handle = core->vap_handle;

  MTLK_ASSERT(vap_handle);
  ILOG1_D("CID-%04x: BEACON_BLOCKED_INTERVAL_REQ", mtlk_vap_get_oid(vap_handle));

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_BEACON_BLOCKED_INTERVAL_REQ;
  man_entry->payload_size = sizeof(UMI_BEACON_BLOCK_TIMER_INTERVAL);

  req = (UMI_BEACON_BLOCK_TIMER_INTERVAL *)(man_entry->payload);
  req->initial = HOST_TO_MAC32(cca_adapt_params->initial);
  req->iterative = HOST_TO_MAC32(cca_adapt_params->iterative);

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != req->Status) {
    ELOG_DDD("CID-%04x: Set BEACON_BLOCKED_INTERVAL_REQ failure, res=%i status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

/* Read CCA adaptive intervals data from PDB, return OK */
static int _mtlk_core_read_cca_intervals (mtlk_core_t *core, iwpriv_cca_adapt_t *cca_adapt_params)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_pdb_size_t cca_adapt_size = sizeof(*cca_adapt_params);

  if (MTLK_ERR_OK != WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_CCA_ADAPT, cca_adapt_params, &cca_adapt_size)) {
    ELOG_D("CID-%04x: Can't read CCA_ADAPT from PDB", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_PARAMS;
  }

  MTLK_ASSERT(sizeof(*cca_adapt_params) == cca_adapt_size);
  return MTLK_ERR_OK;
}

/* Store CCA adaptive intervals in PDB */
static int _mtlk_core_store_cca_intervals(mtlk_core_t *core, iwpriv_cca_adapt_t *cca_adapt_params)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  int res;

  res = WAVE_RADIO_PDB_SET_BINARY(radio, PARAM_DB_RADIO_CCA_ADAPT, cca_adapt_params, sizeof(*cca_adapt_params));
  if (res != MTLK_ERR_OK)
    ELOG_V("Can't update CCA_ADAPT in PDB");

  return res;
}

static int
_mtlk_core_receive_cca_intervals (mtlk_core_t *core, iwpriv_cca_adapt_t *cca_adapt_params)
{
  int res;
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry = NULL;
  UMI_BEACON_BLOCK_TIMER_INTERVAL *req = NULL;
  mtlk_vap_handle_t vap_handle = core->vap_handle;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_BEACON_BLOCKED_INTERVAL_REQ;
  man_entry->payload_size = sizeof(UMI_BEACON_BLOCK_TIMER_INTERVAL);

  req = (UMI_BEACON_BLOCK_TIMER_INTERVAL *)(man_entry->payload);
  req->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == req->Status) {
      cca_adapt_params->initial   = MAC_TO_HOST32(req->initial);
      cca_adapt_params->iterative = MAC_TO_HOST32(req->iterative);
  }
  else {
    ELOG_DDD("CID-%04x: Receive BEACON_BLOCKED_INTERVAL_REQ failed, res=%i status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

static int
_mtlk_core_get_cca_intervals (mtlk_core_t *core, iwpriv_cca_adapt_t *cca_adapt_params)
{
  int res;
  res = _mtlk_core_read_cca_intervals(core, cca_adapt_params);
  if (MTLK_ERR_OK != res)
    return res;

  /* Update FW related fields */
  return _mtlk_core_receive_cca_intervals(core, cca_adapt_params);
}

static int _mtlk_core_store_and_send_cca_intervals(mtlk_core_t *core, iwpriv_cca_adapt_t *cca_adapt_params)
{
  int res;

  res = _mtlk_core_store_cca_intervals(core, cca_adapt_params);
  if (MTLK_ERR_OK == res)
    res = _mtlk_core_send_cca_intervals_req(core, cca_adapt_params);

  return res;
}

int __MTLK_IFUNC mtlk_core_cfg_set_cca_intervals (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                    res = MTLK_ERR_OK;
  uint32                 cfg_size;
  mtlk_core_t            *core = (mtlk_core_t*)hcore;
  wave_radio_t           *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_cca_adapt_cfg_t   *cca_adapt_cfg = NULL;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  /* get configuration */
  cca_adapt_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(cca_adapt_cfg, cfg_size)
    if (!wave_radio_interfdet_get(radio)) {
      ELOG_V("Cannot enable adaptive CCA while interference detection is deactivated");
      MTLK_CLPB_EXIT(MTLK_ERR_NOT_SUPPORTED); /* do not process */
    }

    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(cca_adapt_cfg, cca_adapt_params,
        _mtlk_core_store_and_send_cca_intervals, (core, &cca_adapt_cfg->cca_adapt_params), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_cca_intervals (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_cca_adapt_cfg_t     cca_adapt_cfg;
  mtlk_core_t              *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* read config from internal db */
  MTLK_CFG_SET_ITEM_BY_FUNC(&cca_adapt_cfg, cca_adapt_params,
    _mtlk_core_read_cca_intervals, (core, &cca_adapt_cfg.cca_adapt_params), res);

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &cca_adapt_cfg, sizeof(cca_adapt_cfg));
  }

  return res;
}

int mtlk_core_cfg_init_cca_adapt (mtlk_core_t *core)
{
  int i, res;
  iwpriv_cca_th_t    cca_th_params;
  iwpriv_cca_adapt_t cca_adapt_params;

  /* read config copy it */
  if (MTLK_ERR_OK != mtlk_core_cfg_read_cca_threshold(core, &cca_th_params)) {
    return MTLK_ERR_UNKNOWN;
  }
  memset(&core->slow_ctx->cca_adapt, 0, sizeof(core->slow_ctx->cca_adapt));
  core->slow_ctx->cca_adapt.stepping_down = 0;
  core->slow_ctx->cca_adapt.step_down_coef = 1;
  core->slow_ctx->cca_adapt.last_unblocked_time = 0;

  for (i = 0; i < MTLK_CCA_TH_PARAMS_LEN; i++)
    core->slow_ctx->cca_adapt.cca_th_params[i] = cca_th_params.values[i];

  res = _mtlk_core_get_cca_intervals(core, &cca_adapt_params);
  if (MTLK_ERR_OK != res)
    return res;

  return _mtlk_core_store_cca_intervals(core, &cca_adapt_params);
}

int mtlk_core_cfg_recovery_cca_adapt(mtlk_core_t *core)
{
  iwpriv_cca_adapt_t   cca_adapt_params;

  /* read config and send if it was set */
  if (MTLK_ERR_OK == _mtlk_core_read_cca_intervals(core, &cca_adapt_params)) {
    return _mtlk_core_send_cca_intervals_req(core, &cca_adapt_params);
  }
  else {
    return MTLK_ERR_OK; /* was not set */
  }
}

int __MTLK_IFUNC
mtlk_core_cfg_send_cca_thresholds(mtlk_handle_t core_object,
				  const void *payload, uint32 size)
{
  struct nic *nic = HANDLE_T_PTR(struct nic, core_object);
  iwpriv_cca_th_t *cca_th_params = (iwpriv_cca_th_t *)payload;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(sizeof(*cca_th_params) == size);
  MTLK_UNREFERENCED_PARAM(size);
  MTLK_ASSERT(FALSE == mtlk_vap_is_slave_ap(nic->vap_handle));

  res = mtlk_core_cfg_send_cca_threshold_req(nic, cca_th_params);
  if (res != MTLK_ERR_OK)
    ELOG_DD("CID-%04x: Can't send CCA thresholds (err=%d)", mtlk_vap_get_oid(nic->vap_handle), res);

  mtlk_osal_timer_set(&nic->slow_ctx->cca_step_down_timer, nic->slow_ctx->cca_adapt.interval * 1000);
  return MTLK_ERR_OK;
}

/*------ Radar Detection RSSI Threshold -----------------------------*/

static BOOL
_mtlk_core_cfg_radar_rssi_is_supported(mtlk_core_t *core)
{
  /* Only 5 GHz */
  return (MTLK_HW_BAND_5_2_GHZ == core_cfg_get_freq_band_cur(core));
}

static int
_mtlk_core_receive_radar_rssi_th (mtlk_core_t *core, int *radar_rssi_th)
{
  int res;
  mtlk_txmm_msg_t                    man_msg;
  mtlk_txmm_data_t                   *man_entry = NULL;
  mtlk_vap_handle_t                  vap_handle = core->vap_handle;
  UMI_RADAR_DETECTION_RSSI_TH_CONFIG *req = NULL;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_RADAR_DETECTION_RSSI_TH_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_RADAR_DETECTION_RSSI_TH_CONFIG);
  req = (UMI_RADAR_DETECTION_RSSI_TH_CONFIG *)(man_entry->payload);
  req->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == req->Status) {
      *radar_rssi_th = MAC_TO_HOST32(req->radarDetectionRssiTh);
  }
  else {
    ELOG_DDD("CID-%04x: Receive RADAR_RSSI_TH_REQ failed, res=%i status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

static int
_mtlk_core_cfg_send_radar_rssi_th_req(mtlk_core_t *core, int radar_rssi_th)
{
  int res;
  mtlk_txmm_msg_t                    man_msg;
  mtlk_txmm_data_t                   *man_entry = NULL;
  mtlk_vap_handle_t                  vap_handle = core->vap_handle;
  UMI_RADAR_DETECTION_RSSI_TH_CONFIG *req = NULL;

  MTLK_ASSERT(vap_handle);
  ILOG1_DD("CID-%04x: Set RADAR_RSSI_TH to %d", mtlk_vap_get_oid(vap_handle), radar_rssi_th);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_RADAR_DETECTION_RSSI_TH_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_RADAR_DETECTION_RSSI_TH_CONFIG);

  req = (UMI_RADAR_DETECTION_RSSI_TH_CONFIG *)(man_entry->payload);
  req->radarDetectionRssiTh = HOST_TO_MAC32((int32)radar_rssi_th);

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != req->Status) {
    ELOG_DDD("CID-%04x: Set RADAR_RSSI_TH_REQ failure, res=%i status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

/* Read Radar Detection RSSI Threshold data from PDB */
static int
_mtlk_core_cfg_read_radar_rssi_threshold(mtlk_core_t *core)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  return WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_RADAR_RSSI_TH);
}

/* Store Radar Detection RSSI Threshold in PDB */
static void
_mtlk_core_cfg_store_radar_rssi_threshold(mtlk_core_t *core, int radar_rssi_th)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RADAR_RSSI_TH, radar_rssi_th);
}

static int
_mtlk_core_cfg_store_and_send_radar_rssi_threshold(mtlk_core_t *core, int radar_rssi_th)
{
  int res = MTLK_ERR_UNKNOWN;
  if(!_mtlk_core_cfg_radar_rssi_is_supported(core)) {
    ILOG0_V("Only 5 GHz band is supported");
    return MTLK_ERR_NOT_SUPPORTED;
  }

  if (RADAR_DETECTION_RSSI_TH_MIN > radar_rssi_th ||
      RADAR_DETECTION_RSSI_TH_MAX < radar_rssi_th) {
    ELOG_DDD("Vaule (%d) is not in range from %d to %d", radar_rssi_th,
             RADAR_DETECTION_RSSI_TH_MIN,
             RADAR_DETECTION_RSSI_TH_MAX
    );
    return MTLK_ERR_PARAMS;

  }

  res = _mtlk_core_cfg_send_radar_rssi_th_req(core, radar_rssi_th);
  if (MTLK_ERR_OK == res)
    _mtlk_core_cfg_store_radar_rssi_threshold(core, radar_rssi_th);

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_radar_rssi_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                       res = MTLK_ERR_OK;
  uint32                    cfg_size;
  mtlk_core_t               *core = (mtlk_core_t*)hcore;
  mtlk_radar_rssi_th_cfg_t  *radar_rssi_th_cfg = NULL;
  mtlk_clpb_t               *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  /* get configuration */
  radar_rssi_th_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(radar_rssi_th_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(radar_rssi_th_cfg, radar_rssi_th,
          _mtlk_core_cfg_store_and_send_radar_rssi_threshold, (core, radar_rssi_th_cfg->radar_rssi_th), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_radar_rssi_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_radar_rssi_th_cfg_t radar_rssi_th_cfg;
  mtlk_core_t              *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&radar_rssi_th_cfg, 0, sizeof(radar_rssi_th_cfg));

  if (_mtlk_core_cfg_radar_rssi_is_supported(core)) {
    MTLK_CFG_SET_ITEM_BY_FUNC(&radar_rssi_th_cfg, radar_rssi_th, _mtlk_core_receive_radar_rssi_th,
                              (core, &radar_rssi_th_cfg.radar_rssi_th), res);
  } else {
    res = MTLK_ERR_NOT_SUPPORTED;
  }

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &radar_rssi_th_cfg, sizeof(radar_rssi_th_cfg));
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_recovery_radar_rssi_threshold (mtlk_core_t *core)
{
  int radar_rssi_th;
  int res = MTLK_ERR_OK;

  /* read config and send if it is not default value */
  if(_mtlk_core_cfg_radar_rssi_is_supported(core)) {
    radar_rssi_th = _mtlk_core_cfg_read_radar_rssi_threshold(core);

    if (MTLK_PARAM_DB_RADAR_DETECTION_RSSI_TH_IS_INVALID(radar_rssi_th))
      return MTLK_ERR_OK;

    res = _mtlk_core_cfg_send_radar_rssi_th_req(core, radar_rssi_th);
  }

  return res;
}

/* SSB Mode Configuration */
static int
_mtlk_core_check_ssb_mode (mtlk_core_t *core, const mtlk_ssb_mode_cfg_t *ssb_mode_cfg)
{
    if (/* All params are within limits */
        (ssb_mode_cfg->params[MTLK_SSB_MODE_CFG_EN] > 1) ||
        (ssb_mode_cfg->params[MTLK_SSB_MODE_CFG_20MODE] > 1) ) {
        ELOG_V("one of params does not fit to the range [0...1]");
        return MTLK_ERR_PARAMS;
    }
    return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_core_send_ssb_mode (mtlk_core_t *core, const mtlk_ssb_mode_cfg_t *ssb_mode_cfg)
{
    mtlk_txmm_msg_t         man_msg;
    mtlk_txmm_data_t       *man_entry;
    UMI_SSB_MODE           *mac_msg;
    int                     res;
    unsigned                oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    ILOG2_D("CID-%04x: Send SSB MODE", oid);

    /* allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* fill the message data */
    man_entry->id = UM_MAN_SSB_MODE_REQ;
    man_entry->payload_size = sizeof(UMI_SSB_MODE);
    mac_msg = (UMI_SSB_MODE *)man_entry->payload;
    memset(mac_msg, 0, sizeof(*mac_msg));

    mac_msg->enableSSB  = ssb_mode_cfg->params[MTLK_SSB_MODE_CFG_EN];
    mac_msg->SSB20Mode  = ssb_mode_cfg->params[MTLK_SSB_MODE_CFG_20MODE];

    /* send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
    if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Send SSB MODE failed, res=%i status=%hhu", oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);
    return res;
}

static int
_mtlk_core_receive_ssb_mode (mtlk_core_t *core, mtlk_ssb_mode_cfg_t *ssb_mode_cfg)
{
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_SSB_MODE     *mac_msg;
  int               res;
  unsigned          oid;

  oid = mtlk_vap_get_oid(core->vap_handle);
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SSB_MODE_REQ;
  man_entry->payload_size = sizeof(UMI_SSB_MODE);
  mac_msg = (UMI_SSB_MODE *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == mac_msg->Status) {
    ssb_mode_cfg->params[MTLK_SSB_MODE_CFG_EN]     = mac_msg->enableSSB;
    ssb_mode_cfg->params[MTLK_SSB_MODE_CFG_20MODE] = mac_msg->SSB20Mode;
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_SSB_MODE_REQ failed, res=%i status=%hhu",
            oid, res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int
_mtlk_core_check_and_send_ssb_mode (mtlk_core_t *core, const mtlk_ssb_mode_cfg_t *ssb_mode_cfg)
{
    int res = _mtlk_core_check_ssb_mode(core, ssb_mode_cfg);
    if (MTLK_ERR_OK == res) {
      res = mtlk_core_send_ssb_mode(core, ssb_mode_cfg);
    }
    return res;
}

static void
_mtlk_core_store_ssb_mode (mtlk_core_t *core, const mtlk_ssb_mode_cfg_t *ssb_mode_cfg, uint32 data_size)
{
    MTLK_ASSERT(core != NULL);
    WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_SSB_MODE, ssb_mode_cfg, data_size);
}

void __MTLK_IFUNC
mtlk_core_read_ssb_mode (mtlk_core_t *core, mtlk_ssb_mode_cfg_t *ssb_mode_cfg, uint32 *data_size)
{
    MTLK_ASSERT(core != NULL);
    WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_SSB_MODE, ssb_mode_cfg, data_size);
}

int __MTLK_IFUNC
mtlk_core_set_ssb_mode (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_ssb_mode_cfg_t *ssb_mode_cfg = NULL;
    uint32 ssb_mode_cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get configuration */
    ssb_mode_cfg = mtlk_clpb_enum_get_next(clpb, &ssb_mode_cfg_size);
    MTLK_CLPB_TRY(ssb_mode_cfg, ssb_mode_cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
        /* send new config to FW */
        MTLK_CFG_CHECK_ITEM_AND_CALL(ssb_mode_cfg, params, _mtlk_core_check_and_send_ssb_mode,
                                     (core, ssb_mode_cfg), res);
        /* store new config in internal DB */
        MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(ssb_mode_cfg, params, _mtlk_core_store_ssb_mode,
                                          (core, ssb_mode_cfg, MTLK_SSB_MODE_CFG_SIZE));
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      /* push result into clipboard */
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_get_ssb_mode (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_ssb_mode_cfg_t ssb_mode_cfg;
    mtlk_core_t *core = (mtlk_core_t*)hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* read config from internal DB */
    MTLK_CFG_SET_ITEM_BY_FUNC(&ssb_mode_cfg, params, _mtlk_core_receive_ssb_mode,
                              (core, &ssb_mode_cfg), res);

    /* push result and config into clipboard */
    res = mtlk_clpb_push(clpb, &res, sizeof(res));
    if (MTLK_ERR_OK == res) {
        res = mtlk_clpb_push(clpb, &ssb_mode_cfg, sizeof(ssb_mode_cfg));
    }
    return res;
}

static int
_mtlk_core_cfg_send_atf_quotas (mtlk_core_t *core,
                                const struct intel_vendor_atf_quotas *mtlk_quotas,
                                uint16 max_sid,
                                uint8 *atf_op_res)
{
  mtlk_txmm_msg_t  man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_ATF_QUOTAS   *umi_quotas;
  int              res, i;
  uint16           oid;

  MTLK_ASSERT(core);

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG1_D("CID-%04x: Send ATF QUOTAS", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &res);
  if (!man_entry) {
    ELOG_DD("CID-%04x: UM_MAN_ATF_QUOTAS_REQ init failed, err=%i", oid, res);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_ATF_QUOTAS_REQ;
  umi_quotas = (UMI_ATF_QUOTAS *)(man_entry->payload);
  man_entry->payload_size = sizeof(UMI_ATF_QUOTAS);

  memset (umi_quotas, 0, man_entry->payload_size);

  /* transfer data from mtlk_quotas to umi_quotas */
  umi_quotas->u32interval         = HOST_TO_MAC32(mtlk_quotas->interval);
  umi_quotas->u32freeTime         = HOST_TO_MAC32(mtlk_quotas->free_time);
  umi_quotas->u8AtmAlgorithemType = mtlk_quotas->algo_type;
  umi_quotas->u8AtmWeightedType   = mtlk_quotas->weighted_type;
  umi_quotas->u8nof_vaps          = mtlk_quotas->nof_bss;
  umi_quotas->u16nof_sta          = HOST_TO_MAC16(mtlk_quotas->nof_grants);

  for (i = 0; i < mtlk_quotas->nof_bss; i++) {
    umi_quotas->AtmDistributionType[i] = mtlk_quotas->distr_type;
  }

  for (i = 0; i < mtlk_quotas->nof_grants; i++) {
    uint16 sid = mtlk_quotas->sta_grant[i].sid;
    umi_quotas->stationGrant[sid] = HOST_TO_MAC16(mtlk_quotas->sta_grant[i].grant);
  }

  ILOG1_DDDD("ATF: interval=%u, freeTime=%u, u8AtmAlgorithemType=%u, AtmDistributionType=%u",
             mtlk_quotas->interval,
             mtlk_quotas->free_time,
             mtlk_quotas->algo_type,
             mtlk_quotas->distr_type);
  ILOG1_DDDDD("ATF: u8AtmWeightedType=%d, nof_vaps=%u, nof_sta=%u, nof_grants=%u, max_sid=%u",
            mtlk_quotas->weighted_type,
            mtlk_quotas->nof_bss,
            mtlk_quotas->nof_sta,
            mtlk_quotas->nof_grants,
            max_sid);

  mtlk_dump(2, umi_quotas->stationGrant, sizeof(umi_quotas->stationGrant), "STA grant:");

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == umi_quotas->Status) {
    *atf_op_res = umi_quotas->u8error_code;
    ILOG1_DD("ATF: UM_MAN_ATF_QUOTAS_REQ sent, error_code=%d, res=%d", *atf_op_res, res);
  }
  else {
    ELOG_DDD("CID-%04x: Send ATF QUOTAS failed, res=%i status=%hhu", oid, res, umi_quotas->Status);
    if (UMI_OK != umi_quotas->Status)
      res = MTLK_ERR_MAC;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_atf_quotas (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res;
  uint8 atf_op_res;
  uint32 mtlk_quotas_size, expected_size;
  unsigned max_vaps, max_stas, max_sid;
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  struct intel_vendor_atf_quotas *mtlk_quotas = NULL;
  wave_radio_t *radio;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  mtlk_quotas = mtlk_clpb_enum_get_next(clpb, &mtlk_quotas_size);
  if (__UNLIKELY(NULL == mtlk_quotas)) {
    ELOG_V("failed to get data from clipboard");
    return MTLK_ERR_UNKNOWN;
  }

  expected_size = sizeof(*mtlk_quotas);
  if (__UNLIKELY(mtlk_quotas_size < expected_size)) {
    ELOG_DD("wrong data size in clipboard %u (expected: more than %u)", mtlk_quotas_size, expected_size);
    return MTLK_ERR_UNKNOWN;
  }

  expected_size += mtlk_quotas->data_len;
  if (__UNLIKELY(mtlk_quotas_size != expected_size)) {
    ELOG_DD("wrong data size in clipboard %u (expected: %u)", mtlk_quotas_size, expected_size);
    return MTLK_ERR_UNKNOWN;
  }

  /* Data in UMI_ATF_QUOTAS contain arrays whose dimensions are HW-dependent */
  radio = wave_vap_radio_get(core->vap_handle);
  max_vaps = wave_radio_max_vaps_get(radio);
  max_stas = GEN6_MAX_SID; /* wave_radio_max_stas_get(radio);*/
  

  if (mtlk_quotas->nof_bss > max_vaps) {
    ELOG_DD("nof_bss (%u) > max_vaps (%u)", mtlk_quotas->nof_bss, max_vaps);
    return MTLK_ERR_PARAMS;
  }

  if (mtlk_quotas->nof_sta > max_stas) {
    ELOG_DD("nof_sta (%u) > max_stas (%u)", mtlk_quotas->nof_sta, max_stas);
    return MTLK_ERR_PARAMS;
  }

  
  /* in Gen 6 we support only the dynamic ATF mode there is no support for static mode */
  if (mtlk_quotas->distr_type == ATM_DISTRIBUTION_TYPE_STATIC) {
    ELOG_D("we support only dynamic ATF mode -distr_type (%u)", mtlk_quotas->distr_type);
    return MTLK_ERR_PARAMS;
  }

  max_sid = GEN6_MAX_SID;

  {
    unsigned i;
    for (i = 0; i < mtlk_quotas->nof_grants; i++) {
      if (mtlk_quotas->sta_grant[i].sid > max_sid) {
        ELOG_DDD("SID (%u) number (%u) more than max SID (%u)",
                 mtlk_quotas->sta_grant[i].sid, i, max_sid);
        return MTLK_ERR_PARAMS;
      }
    }
  }

  /* send configuration to the FW */
  ILOG0_V("send ATF configuration to the FW ");
  res = _mtlk_core_cfg_send_atf_quotas(core, mtlk_quotas, max_sid, &atf_op_res);

  /* push result and response from FW into clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &atf_op_res, sizeof(atf_op_res));
}

int __MTLK_IFUNC
mtlk_core_cfg_send_active_ant_mask (mtlk_core_t *core, uint32 mask)
{
  int               res;
  uint32            hw_antenna_mask;
  mtlk_txmm_msg_t   umi_msg;
  mtlk_txmm_data_t  *man_entry = NULL;
  UMI_SET_ANTENNAS  *umi_set_antennas = NULL;
  mtlk_vap_handle_t vap_handle = core->vap_handle;
  wave_radio_t      *radio = wave_vap_radio_get(vap_handle);

  MTLK_ASSERT(radio);

  /* This iwpriv is supported for 5G band only -- the limitation imposed by request from FW team */
  if (mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_5_2_GHZ) != MTLK_ERR_OK) {
    ELOG_V("Active antenna mask is only supported for 5G band");
    return MTLK_ERR_OK;
  }

  /* The given antenna mask has to be supported by hardware per radio */
  hw_antenna_mask = wave_radio_tx_antenna_mask_get(radio); /* the same for TX/RX */
  if (mask & ~hw_antenna_mask) {
    ELOG_DD("The given active antenna mask (0x%X) isn't supported by hardware (0x%X)", mask, hw_antenna_mask);
    return MTLK_ERR_PARAMS;
  }

  /* Zero antenna mask means resetting it to maximum allowed by hardware */
  if (!mask) {
    mask = hw_antenna_mask;
  }

  /* Active antenna mask cannot be altered if Auto CoC is enabled */
  if (mtlk_coc_is_auto_mode(__wave_core_coc_mgmt_get(core))) {
    ELOG_V("Active antenna mask cannot be set if auto CoC is turned on");
    return MTLK_ERR_PARAMS;
  }

  ILOG2_D("Sending MC_MAN_SET_ANTENNAS_REQ, mask = %d", mask);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&umi_msg, mtlk_vap_get_txmm(vap_handle), NULL);

  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = MC_MAN_SET_ANTENNAS_REQ;
  man_entry->payload_size = sizeof(UMI_SET_ANTENNAS);

  umi_set_antennas = (UMI_SET_ANTENNAS *)(man_entry->payload);
  memset (umi_set_antennas, 0, sizeof(UMI_SET_ANTENNAS));
  umi_set_antennas->RxAntsMask = (uint8) mask;
  umi_set_antennas->TxAntsMask = (uint8) mask;

  res = mtlk_txmm_msg_send_blocked(&umi_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != umi_set_antennas->status) {
    ELOG_DDD("CID-%04x: Sending MC_MAN_SET_ANTENNAS_REQ failed, res=%d status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, umi_set_antennas->status);
    if (UMI_OK != umi_set_antennas->status)
      res = MTLK_ERR_MAC;
  }
  else {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_ACTIVE_ANT_MASK, mask);
    wave_radio_current_antenna_mask_set(radio, mask);
  }

  mtlk_txmm_msg_cleanup(&umi_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_get_active_ant_mask (mtlk_core_t *core, uint32 *mask)
{
  int               res;
  mtlk_txmm_msg_t   umi_msg;
  mtlk_txmm_data_t  *man_entry = NULL;
  UMI_SET_ANTENNAS  *umi_set_antennas = NULL;
  mtlk_vap_handle_t vap_handle = core->vap_handle;

  MTLK_ASSERT(mask);

  ILOG2_V("Sending MC_MAN_SET_ANTENNAS_REQ");

  man_entry = mtlk_txmm_msg_init_with_empty_data(&umi_msg, mtlk_vap_get_txmm(vap_handle), NULL);

  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = MC_MAN_SET_ANTENNAS_REQ;
  man_entry->payload_size = sizeof(UMI_SET_ANTENNAS);

  umi_set_antennas = (UMI_SET_ANTENNAS *)(man_entry->payload);
  memset (umi_set_antennas, 0, sizeof(UMI_SET_ANTENNAS));
  umi_set_antennas->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&umi_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != umi_set_antennas->status) {
    ELOG_DDD("CID-%04x: Sending MC_MAN_SET_ANTENNAS_REQ failed, res=%d status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, umi_set_antennas->status);
    if (UMI_OK != umi_set_antennas->status)
      res = MTLK_ERR_MAC;
  } else if (umi_set_antennas->TxAntsMask != umi_set_antennas->RxAntsMask) {
    ELOG_DDD("CID-%04x: TxAntsMask(0x%X) differs from RxAntsMask(0x%X)",
            mtlk_vap_get_oid(vap_handle), umi_set_antennas->TxAntsMask, umi_set_antennas->RxAntsMask);
    res = MTLK_ERR_UNKNOWN;
  } else {
    *mask = umi_set_antennas->TxAntsMask;
  }

  mtlk_txmm_msg_cleanup(&umi_msg);
  return res;
}

static int
_mtlk_core_cfg_store_channel_prev (mtlk_core_t *core, const mtlk_channel_prev_data_t *data)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  int res = WAVE_RADIO_PDB_SET_BINARY(radio, PARAM_DB_RADIO_CHANNEL_PREV, data, sizeof(*data));

  if (MTLK_ERR_OK != res) {
    ELOG_D("CID-%04x: Failed to store Previous Channel Configuration into Parameter DB.",
           mtlk_vap_get_oid(core->vap_handle));
  }
  return res;
}

static int
_mtlk_core_cfg_read_channel_prev (mtlk_core_t *core, mtlk_channel_prev_data_t *data)
{
  int res;
  mtlk_pdb_size_t data_size = sizeof(*data);
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);

  res = WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_CHANNEL_PREV, data, &data_size);
  MTLK_ASSERT(sizeof(*data) == data_size);

  if (MTLK_ERR_OK != res) {
    ELOG_D("CID-%04x: Failed to read Previous Channel Configuration from Parameter DB.",
           mtlk_vap_get_oid(core->vap_handle));
  }
  return res;
}

static void
_mtlk_core_cfg_reset_channel_prev (mtlk_core_t *core)
{
  mtlk_channel_prev_data_t mtlk_channel_prev_cfg;

  /* Reset configuration of previously set channel in parameter data base */
  mtlk_channel_prev_cfg.pri_chan_num = MTLK_CHANNEL_PREV_CFG_PRI_NUM_DEFAULT;
  mtlk_channel_prev_cfg.sec_chan_freq = 0;
  _mtlk_core_cfg_store_channel_prev(core, &mtlk_channel_prev_cfg);
}

static int
_mtlk_core_cfg_set_channel_prev_on_coex_en (mtlk_core_t *core, struct mtlk_chan_def *ccd)
{
  mtlk_channel_prev_data_t mtlk_channel_prev_cfg;

  MTLK_ASSERT(ccd->center_freq1 != ccd->chan.center_freq);

  /* Save configuration of currently set channel in parameter data base */
  mtlk_channel_prev_cfg.pri_chan_num = (uint16)ieee80211_frequency_to_channel(ccd->chan.center_freq);
  mtlk_channel_prev_cfg.sec_chan_freq = (uint16)ccd->center_freq1;
  return _mtlk_core_cfg_store_channel_prev(core, &mtlk_channel_prev_cfg);
}

static __INLINE void
__mtlk_core_cfg_prepare_csa_info_on_set_chan_for_coex (mtlk_core_t *core, struct set_chan_param_data *cpd)
{
#define MTLK_CSA_COUNT_DEFAULT  5
  memset(cpd, 0, sizeof(*cpd));
  /* Change channel's switch type to "Channel's Switch Announcement" */
  cpd->switch_type = ST_CSA;
  /* Set a time to wait until channel's switch */
  cpd->chan_switch_time = (uint16)(MTLK_CSA_COUNT_DEFAULT *
                                   MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_BEACON_PERIOD));
#undef MTLK_CSA_COUNT_DEFAULT
}

static void
_mtlk_core_cfg_restore_chan_width_on_coex_dis (mtlk_core_t *core)
{
  struct mtlk_chan_def *ccd = __wave_core_chandef_get(core);
  uint16 oid;

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG1_D("CID-%04x: 2.4GHz Multi-Radio Co-Existence is deactivated.", oid);

  /* Must be done for 2.4GHz channels of 20MHz width only */
  if ((MTLK_HW_BAND_2_4_GHZ == core_cfg_get_freq_band_cur(core)) &&
      (is_channel_certain(ccd)) && (CW_20 == ccd->width)) {
    uint16 pri_chan_num_curr;
    uint16 pri_chan_num_prev;
    mtlk_channel_prev_data_t mtlk_channel_prev_cfg;

    /* Must be done if network mode wasn't changed while Multi-Radio Co-Existence was active */
    if (!is_ht_net_mode(core_cfg_get_network_mode(core))) {
      WLOG_D("CID-%04x: Failed to restore 2.4GHz channel's width to 40MHz "
             "because network mode was changed to non-HT while Multi-Radio Co-Existence was active.", oid);
      _mtlk_core_cfg_reset_channel_prev(core);
      return;
    }

    /* Get configuration of previously set channel from parameter data base */
    if (MTLK_ERR_OK != _mtlk_core_cfg_read_channel_prev(core, &mtlk_channel_prev_cfg))
    {
      WLOG_D("CID-%04x: Failed to restore 2.4GHz channel's width to 40MHz "
             "because can't read parameters from DB.", oid);
      _mtlk_core_cfg_reset_channel_prev(core);
      return;
    }
    pri_chan_num_prev = mtlk_channel_prev_cfg.pri_chan_num;

    /* Must be done if channel's width was changed during activation of Multi-Radio Co-Existence */
    if (MTLK_CHANNEL_PREV_CFG_PRI_NUM_DEFAULT != pri_chan_num_prev) {
      /* Must be done if channel number wasn't changed while Multi-Radio Co-Existence was active */
      pri_chan_num_curr = (uint16)mtlk_core_get_channel(core);
      if (pri_chan_num_curr == pri_chan_num_prev) {
        struct mtlk_chan_def ccd_new;
        struct set_chan_param_data cpd_new;
        mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
        mtlk_df_user_t *df_user = mtlk_df_get_user(df);
        struct net_device *ndev = mtlk_df_user_get_ndev(df_user);

        /* Make a copy of channel's current definition */
        ccd_new = *ccd;

        /* Change channel's width to 40MHz */
        ccd_new.width = CW_40;
        /* Change center frequency #1 */
        ccd_new.center_freq1 = (uint32)mtlk_channel_prev_cfg.sec_chan_freq;

        /* Prepare information for channel's switch with "Channel's Switch Announcement" */
        __mtlk_core_cfg_prepare_csa_info_on_set_chan_for_coex(core, &cpd_new);

        if (MTLK_ERR_OK == core_cfg_send_set_chan(core, &ccd_new, &cpd_new)) {
          ILOG1_D("CID-%04x: 2.4GHz channel's width is restored to 40MHz.", oid);
          /* Notify the Hostapd that channel's width is changed */
          wv_cfg80211_ch_switch_notify(ndev, &ccd_new, FALSE);
        }
        else {
          ELOG_D("CID-%04x: Failed to restore 2.4GHz channel's width to 40MHz "
                 "because can't send message to Firmware.", oid);
        }
      }
      else {
        WLOG_DDD("CID-%04x: Failed to restore 2.4GHz channel's width to 40MHz "
                 "because channel was changed (%d -> %d) while Multi-Radio Co-Existence was active.",
                 oid, pri_chan_num_prev, pri_chan_num_curr);
      }
      _mtlk_core_cfg_reset_channel_prev(core);
    }
  }
}

static int
_mtlk_core_cfg_change_chan_width_on_coex_en (mtlk_core_t *core)
{
  struct mtlk_chan_def *ccd = __wave_core_chandef_get(core);
  int res = MTLK_ERR_OK;
  uint16 oid;

  oid = mtlk_vap_get_oid(core->vap_handle);

  ILOG1_D("CID-%04x: 2.4GHz Multi-Radio Co-Existence is activated.", oid);

  /* Must be done for 2.4GHz channels of 40MHz width only */
  if ((MTLK_HW_BAND_2_4_GHZ == core_cfg_get_freq_band_cur(core)) &&
      (is_channel_certain(ccd)) && (CW_40 == ccd->width)) {
    struct mtlk_chan_def ccd_new;
    struct set_chan_param_data cpd_new;
    mtlk_df_t *df = mtlk_vap_get_df(core->vap_handle);
    mtlk_df_user_t *df_user = mtlk_df_get_user(df);
    struct net_device *ndev = mtlk_df_user_get_ndev(df_user);

    /* Make a copy of channel's current definition */
    ccd_new = *ccd;

    res = _mtlk_core_cfg_set_channel_prev_on_coex_en(core, &ccd_new);
    if (MTLK_ERR_OK != res) {
      ELOG_D("CID-%04x: Failed to change 2.4GHz channel's width to 20MHz "
             "because can't store parameters into DB.", oid);
      return res;
    }

    /* Change channel's width to 20MHz */
    ccd_new.width = CW_20;
    /* Change center frequency #1 to center frequency of primary channel */
    ccd_new.center_freq1 = ccd_new.chan.center_freq;

    /* Prepare information for channel's switch with "Channel's Switch Announcement" */
    __mtlk_core_cfg_prepare_csa_info_on_set_chan_for_coex(core, &cpd_new);

    if (MTLK_ERR_OK == core_cfg_send_set_chan(core, &ccd_new, &cpd_new)) {
      ILOG1_D("CID-%04x: 2.4GHz channel's width is changed to 20MHz.", oid);
      /* Notify the Hostapd that channel's width is changed */
      wv_cfg80211_ch_switch_notify(ndev, &ccd_new, FALSE);
    }
    else {
      ELOG_D("CID-%04x: Failed to change 2.4GHz channel's width to 20MHz "
             "because can't send message to Firmware.", oid);
    }
  }
  return res;
}

static int
_mtlk_core_cfg_try_to_change_chan_width_on_coex_en (mtlk_core_t *core, uint32 active_time, uint8 inactive_time, uint8 cts2self_active)
{
  int res = _mtlk_core_cfg_change_chan_width_on_coex_en(core);

  if (MTLK_ERR_OK != res) {
    /* Disable Co-Existence due to error */
    res = mtlk_core_send_coex_config_req(core, CO_EX_QOS_NONE, active_time, inactive_time, cts2self_active);
    if (MTLK_ERR_OK == res) {
      ELOG_D("CID-%04x: Failed to enable Multi-Radio Co-Existence "
             "because 2.4GHz channel's width is 40MHz.", mtlk_vap_get_oid(core->vap_handle));
      WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_COEX_MODE, CO_EX_QOS_NONE);
      res = MTLK_ERR_BUSY;
    }
  }
  return res;
}

void __MTLK_IFUNC
core_cfg_change_chan_width_if_coex_en (mtlk_core_t *core, struct mtlk_chan_def *ccd)
{
  int mode = WAVE_RADIO_PDB_GET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_COEX_MODE);
  /* It's not allowed to set a channel's bandwidth to 40MHz in case if
     it's 2.4GHz channel and Multi-Radio Co-Existence is enabled */
  if ((MTLK_HW_BAND_2_4_GHZ == core_cfg_get_freq_band_cur(core)) && (CW_40 == ccd->width)) {
    if (mode > CO_EX_QOS_NONE) {
      ccd->width = CW_20;
      ccd->center_freq1 = ccd->chan.center_freq;
      WLOG_DD("CID-%04x: Can't set 2.4GHz channel's width to 40MHz due to active Multi-Radio Co-Existence, mode:%d. "
             "20MHz width will be used instead", mtlk_vap_get_oid(core->vap_handle), mode);
    }
  }
}

int __MTLK_IFUNC
mtlk_core_send_coex_config_req (mtlk_core_t *core, uint8 coex_mode, uint32 active_time, uint8 inactive_time, uint8 cts2self_active)
{
  mtlk_txmm_msg_t     man_msg;
  mtlk_txmm_data_t   *man_entry;
  UMI_SET_2_4_G_COEX *coex_cfg;
  int                 res;
  unsigned            oid;

  MTLK_ASSERT(core != NULL);
  oid = mtlk_vap_get_oid(core->vap_handle);

  if (MTLK_ERR_OK != mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_2_4_GHZ)) {
    ELOG_D("CID-%04x: MR Coex only supported in 2.4 GHz", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_NOT_SUPPORTED;
  }

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_2_4GHZ_COEX_REQ;
  man_entry->payload_size = sizeof(UMI_SET_2_4_G_COEX);
  coex_cfg = (UMI_SET_2_4_G_COEX *)man_entry->payload;
  memset(coex_cfg, 0, sizeof(*coex_cfg));

  if ((coex_mode > CO_EX_QOS_NONE) && (coex_mode < CO_EX_QOS_NUM_OF_TYPES)) {
    coex_cfg->coExQos     = coex_mode;
    coex_cfg->bCoExActive = 1; /* Enable Coex */
  } else if (coex_mode == CO_EX_QOS_NONE) {
    coex_cfg->coExQos     = CO_EX_QOS_NONE;
    coex_cfg->bCoExActive = 0; /* Disable Coex */
  } else {
    ELOG_DD("CID-%04x: Unsupported Coex mode:%d ", oid, coex_mode);
    res = MTLK_ERR_PARAMS;
    goto finish;
  }
  coex_cfg->activeWifiWindowSize = HOST_TO_MAC32(active_time);
  coex_cfg->inactiveWifiWindowSize = inactive_time;
  coex_cfg->bCts2selfActive = cts2self_active;

  ILOG1_DD("Send UMI_SET_2_4_G_COEX mode:%d active:%d", coex_cfg->coExQos, coex_cfg->bCoExActive);
  ILOG1_DDD("active_time:%d inactive_time:%d cts2self_active:%d", active_time, coex_cfg->inactiveWifiWindowSize, cts2self_active);
  mtlk_dump(1, coex_cfg, sizeof(*coex_cfg), "dump of UMI_SET_2_4_G_COEX:");

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != coex_cfg->Status) {
    ELOG_DDD("CID-%04x: Sending of 2.4 GHz Coex config failed, res=%i status=%hhu", oid, res, coex_cfg->Status);
    if (UMI_OK != coex_cfg->Status)
      res = MTLK_ERR_MAC;
  }

finish:
  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int
_mtlk_core_store_and_switch_coex (mtlk_core_t *core, mtlk_coex_data_t *new_coex)
{
  int res;
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  uint8 prev_mode     = (uint8)WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_COEX_MODE);
  uint32 prev_active  = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_COEX_ACTIVE_TIME);
  uint8 prev_inactive = (uint8)WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_COEX_INACTIVE_TIME);
  uint8 prev_cts2self_active = (uint8)WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_COEX_CTS2SELF_ACTIVE);
  uint32 new_active   = new_coex->activeWifiWindowSize;
  uint8 new_mode      = new_coex->coex_mode;
  uint8 new_inactive  = new_coex->inactiveWifiWindowSize;
  uint8 new_cts2self_active = new_coex->cts2self_active;

  if (MTLK_ERR_OK != mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_2_4_GHZ)) {
    ELOG_D("CID-%04x: MR Coex only supported in 2.4 GHz", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_NOT_SUPPORTED;
  }

  if (new_mode > COEX_QOS_MAX) {
    ELOG_D("Wrong 2.4 GHz coex mode:%u", new_mode);
    return MTLK_ERR_PARAMS;
  }

  if (new_active == MAX_UINT32) {
    new_active = prev_active;
  }

  if (new_active < ACTIVE_WIFI_WINDOW_SIZE_MIN || new_active > ACTIVE_WIFI_WINDOW_SIZE_MAX) {
    ELOG_D("Wrong 2.4 GHz coex active time:%u", new_active);
    return MTLK_ERR_PARAMS;
  }

  if (new_inactive == MAX_UINT8) {
    new_inactive = prev_inactive;
  }

  if (new_inactive < INACTIVE_WIFI_WINDOW_SIZE_MIN || new_inactive > INACTIVE_WIFI_WINDOW_SIZE_MAX) {
    ELOG_D("Wrong 2.4 GHz coex inactive time:%u", new_inactive);
    return MTLK_ERR_PARAMS;
  }

  if (new_cts2self_active == MAX_UINT8) {
    new_cts2self_active = prev_cts2self_active;
  }

  if ((new_mode == prev_mode) && (new_active == prev_active) && (new_inactive == prev_inactive) && (new_cts2self_active == prev_cts2self_active)) {
    ILOG1_DDDD("2.4 GHz coexistance is not changed: mode - %u, active time - %u, inactive time - %u, cts2self active - %u",
            new_mode, new_active, new_inactive, new_cts2self_active);
    return MTLK_ERR_OK;
  }

  res = mtlk_core_send_coex_config_req(core, new_mode, new_active, new_inactive, new_cts2self_active);

  if (MTLK_ERR_OK == res) {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_COEX_MODE, new_mode);
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_COEX_ACTIVE_TIME, new_active);
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_COEX_INACTIVE_TIME, new_inactive);
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_COEX_CTS2SELF_ACTIVE, new_cts2self_active);

    if ((new_mode > CO_EX_QOS_NONE) && (prev_mode == CO_EX_QOS_NONE)) {
      /* Must be done in case of Multi-Radio Co-Existence activation only */
      res = _mtlk_core_cfg_try_to_change_chan_width_on_coex_en(core, new_active, new_inactive, new_cts2self_active);
    }
    else if ((new_mode == CO_EX_QOS_NONE) && (prev_mode > CO_EX_QOS_NONE)) {
      /* Must be done in case of Multi-Radio Co-Existence deactivation only */
      _mtlk_core_cfg_restore_chan_width_on_coex_dis(core);
    }
  }
  return res;
}

int __MTLK_IFUNC
mtlk_core_set_coex_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_coex_cfg_t *coex_cfg = NULL;
  uint32 coex_cfg_size;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  coex_cfg = mtlk_clpb_enum_get_next(clpb, &coex_cfg_size);
  MTLK_CLPB_TRY(coex_cfg, coex_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(coex_cfg, coex_data, _mtlk_core_store_and_switch_coex,
                                   (core, &coex_cfg->coex_data), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_receive_coex_config (mtlk_core_t *core, mtlk_coex_data_t *coex_data_cfg)
{
  mtlk_txmm_msg_t     man_msg;
  mtlk_txmm_data_t   *man_entry;
  UMI_SET_2_4_G_COEX *coex_cfg;
  int                 res;
  unsigned            oid;
  uint8 mode_read = CO_EX_QOS_NONE;

  oid = mtlk_vap_get_oid(core->vap_handle);
  if (MTLK_ERR_OK != mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_2_4_GHZ)) {
    ELOG_D("CID-%04x: MR Coex only supported in 2.4 GHz", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_NOT_SUPPORTED;
  }

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_2_4GHZ_COEX_REQ;
  man_entry->payload_size = sizeof(UMI_SET_2_4_G_COEX);
  coex_cfg = (UMI_SET_2_4_G_COEX *)man_entry->payload;
  coex_cfg->getSetOperation = API_GET_OPERATION;
  /* dummy values */
  coex_cfg->bCoExActive = 0;
  coex_cfg->coExQos = COEX_QOS_MIN;
  coex_cfg->activeWifiWindowSize = HOST_TO_MAC32(ACTIVE_WIFI_WINDOW_SIZE_MIN);
  coex_cfg->inactiveWifiWindowSize = INACTIVE_WIFI_WINDOW_SIZE_MIN;
  coex_cfg->bCts2selfActive = 0;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == coex_cfg->Status) {
    if (coex_cfg->bCoExActive) {
      if (coex_cfg->coExQos < CO_EX_QOS_NUM_OF_TYPES) {
        mode_read = coex_cfg->coExQos;
      }
    }
    coex_data_cfg->activeWifiWindowSize = MAC_TO_HOST32(coex_cfg->activeWifiWindowSize);
    coex_data_cfg->inactiveWifiWindowSize = coex_cfg->inactiveWifiWindowSize;
    coex_data_cfg->cts2self_active = coex_cfg->bCts2selfActive;

    ILOG1_DD("CID-%04x: UM_MAN_2_4GHZ_COEX_REQ returned mode:%d", oid, mode_read);
    ILOG1_DDDD("CID-%04x: active_time:%d, inactive_time:%d, cts2self_active:%d", oid,
             coex_data_cfg->activeWifiWindowSize, coex_data_cfg->inactiveWifiWindowSize, coex_data_cfg->cts2self_active);
    mtlk_dump(1, coex_cfg, sizeof(*coex_cfg), "dump of UMI_SET_2_4_G_COEX:");
  }
  else {
    ELOG_DDD("CID-%04x: receiving UM_MAN_2_4GHZ_COEX_REQ failed, res=%d status=%hhu",
            oid, res, coex_cfg->Status);
    if (UMI_OK != coex_cfg->Status)
      res = MTLK_ERR_MAC;
  }

  coex_data_cfg->coex_mode = mode_read;
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_get_coex_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_coex_data_t coex_cfg_data;
  mtlk_coex_cfg_t coex_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  if (MTLK_ERR_OK != mtlk_core_is_band_supported(core, UMI_PHY_TYPE_BAND_2_4_GHZ)) {
    ELOG_D("CID-%04x: MR Coex only supported in 2.4 GHz", mtlk_vap_get_oid(core->vap_handle));
    res = MTLK_ERR_NOT_SUPPORTED;
  }
  else {
    res = mtlk_core_receive_coex_config(core, &coex_cfg_data);
    if (MTLK_ERR_OK == res) {
      MTLK_CFG_SET_ITEM(&coex_cfg, coex_data, coex_cfg_data);
    }
  }

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &coex_cfg, sizeof(coex_cfg));
  }

  return res;
}

/* ================ Multicast range processing ================== */
static int
_mtlk_core_cfg_setup_mcast_range (mtlk_core_t *core, mtlk_mcast_range_t *mcast_range)
{
  MTLK_ASSERT(mcast_range->action < MTLK_MCAST_ACTION_LAST);
  switch (mcast_range->action) {
    case  MTLK_MCAST_ACTION_CLEANUP:
      return mtlk_mc_ranges_cleanup(core, mcast_range->netmask.type);
    case  MTLK_MCAST_ACTION_ADD:
      return mtlk_mc_ranges_add(core, &mcast_range->netmask);
    case  MTLK_MCAST_ACTION_DEL:
      return mtlk_mc_ranges_del(core, &mcast_range->netmask);
    default:
      return  MTLK_ERR_PARAMS;
  }
}

int __MTLK_IFUNC
mtlk_core_cfg_set_mcast_range (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                        res = MTLK_ERR_OK;
  uint32                     cfg_size;
  mtlk_core_t                *core = (mtlk_core_t*)hcore;
  mtlk_mcast_range_cfg_t     *mcast_range_cfg;
  mtlk_clpb_t                *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  /* get configuration */
  mcast_range_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(mcast_range_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(mcast_range_cfg, range,
         _mtlk_core_cfg_setup_mcast_range, (core, &mcast_range_cfg->range), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

static int
_mtlk_core_cfg_get_mcast_range_list (mtlk_handle_t hcore, const void* data, uint32 data_size, ip_type_t ip_type)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_mcast_range_vect_cfg_t mcast_range_vect_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  memset(&mcast_range_vect_cfg, 0, sizeof(mcast_range_vect_cfg));

  MTLK_CFG_SET_ITEM_BY_FUNC(&mcast_range_vect_cfg, range_vect,
      mtlk_mc_ranges_get_vect, (core, ip_type, &mcast_range_vect_cfg), res);

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &mcast_range_vect_cfg, sizeof(mcast_range_vect_cfg));
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_get_mcast_range_list_ipv4 (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  return _mtlk_core_cfg_get_mcast_range_list(hcore, data, data_size, MTLK_IPv4);
}

int __MTLK_IFUNC
mtlk_core_cfg_get_mcast_range_list_ipv6 (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  return _mtlk_core_cfg_get_mcast_range_list(hcore, data, data_size, MTLK_IPv6);
}

int __MTLK_IFUNC
mtlk_core_cfg_get_forward_unknown_mcast_flag (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_forward_unknown_mcast_cfg_t forward_unknown_mcast_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&forward_unknown_mcast_cfg, 0, sizeof(forward_unknown_mcast_cfg));

  MTLK_CFG_SET_ITEM(&forward_unknown_mcast_cfg, flag,
                    MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_FWRD_UNKWN_MCAST_FLAG));

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res)
    res = mtlk_clpb_push(clpb, &forward_unknown_mcast_cfg, sizeof(forward_unknown_mcast_cfg));

  return res;
}

static int
_mtlk_core_cfg_set_forward_unknown_mcast_flag (mtlk_core_t *core, uint32 flag)
{
	MTLK_CORE_PDB_SET_INT(core, PARAM_DB_CORE_FWRD_UNKWN_MCAST_FLAG, !!flag);
	return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_forward_unknown_mcast_flag (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_forward_unknown_mcast_cfg_t *forward_unknown_mcast_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  uint32 forward_unknown_mcast_cfg_size;

  forward_unknown_mcast_cfg = mtlk_clpb_enum_get_next(clpb, &forward_unknown_mcast_cfg_size);
  MTLK_CLPB_TRY(forward_unknown_mcast_cfg, forward_unknown_mcast_cfg_size)
  {
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(forward_unknown_mcast_cfg, flag,
                                   _mtlk_core_cfg_set_forward_unknown_mcast_flag,
                                   (core, forward_unknown_mcast_cfg->flag), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  }
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END;
}

void mtlk_core_cfg_flush_wds_wpa_list (mtlk_core_t *nic)
{
  _mtlk_core_flush_ieee_addr_list(nic, &nic->wds_wpa_sta_list, "WDS WPA");
}

BOOL mtlk_core_cfg_wds_wpa_entry_exists (mtlk_core_t *nic, const IEEE_ADDR *addr)
{
  return _mtlk_core_ieee_addr_entry_exists(nic, addr, &nic->wds_wpa_sta_list, "WDS WPA");
}

int mtlk_core_cfg_add_wds_wpa_entry (mtlk_core_t* nic, const IEEE_ADDR *addr)
{
  return _mtlk_core_add_ieee_addr_entry(nic, addr, &nic->wds_wpa_sta_list, "WDS WPA");
}

int mtlk_core_cfg_del_wds_wpa_entry (mtlk_core_t* nic, const IEEE_ADDR *addr)
{
  return _mtlk_core_del_ieee_addr_entry(nic, addr, &nic->wds_wpa_sta_list, "WDS WPA",
    TRUE);
}

/************* Max MPDU length ******************/

uint32 mtlk_core_cfg_read_max_mpdu_length (mtlk_core_t *core)
{
    MTLK_ASSERT(core != NULL);
    return WAVE_RADIO_PDB_GET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_MAX_MPDU_LENGTH);
}

static void
_mtlk_core_store_max_mpdu_length (mtlk_core_t *core, const uint32 max_mpdu_length)
{
    MTLK_ASSERT(core != NULL);
    WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_MAX_MPDU_LENGTH, max_mpdu_length);
}

int __MTLK_IFUNC
mtlk_core_cfg_receive_max_mpdu_length (mtlk_core_t *core, uint32 *max_mpdu_length)
{
    mtlk_txmm_msg_t         man_msg;
    mtlk_txmm_data_t        *man_entry;
    UMI_MAX_MPDU            *mac_msg;
    int                     res;
    unsigned                oid;


    oid = mtlk_vap_get_oid(core->vap_handle);
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    man_entry->id = UM_MAN_MAX_MPDU_LENGTH_REQ;
    man_entry->payload_size = sizeof(UMI_MAX_MPDU);
    mac_msg = (UMI_MAX_MPDU *)man_entry->payload;
    mac_msg->getSetOperation = API_GET_OPERATION;

    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK == res && UMI_OK == mac_msg->Status) {
      *max_mpdu_length = MAC_TO_HOST32(mac_msg->maxMpduLength);
    }
    else {
      ELOG_DDD("CID-%04x: Receiving UM_MAN_MAX_MPDU_LENGTH_REQ failed, res=%d status=%hhu",
              oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    mtlk_txmm_msg_cleanup(&man_msg);
    return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_send_max_mpdu_length (mtlk_core_t *core, const uint32 max_mpdu_length)
{
    mtlk_txmm_msg_t         man_msg;
    mtlk_txmm_data_t        *man_entry;
    UMI_MAX_MPDU            *mac_msg;
    int                     res;
    unsigned                oid;
    mtlk_vap_manager_t *vap_mgr;
    wave_radio_t *radio;
    wv_mac80211_t *mac80211;
    BOOL has_peer_connections;
    BOOL has_stas_connected;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);
    vap_mgr = mtlk_vap_get_manager(core->vap_handle);
    radio = wave_vap_radio_get(core->vap_handle);
    mac80211 = wave_radio_mac80211_get(radio);
    has_peer_connections = mtlk_vap_manager_has_peer_connections(vap_mgr);
    has_stas_connected = wv_mac80211_has_sta_connections(mac80211);

    if (has_stas_connected || has_peer_connections) {
      /* command can be issued only if no stations are connected */
      WLOG_D("CID-%04x: can't set max mpdu length when stations are connected - request rejected", oid);
      return MTLK_ERR_NOT_READY;
    }

    if ((max_mpdu_length < UMI_DEF_MPDU_LENGTH_MIN) || (max_mpdu_length > UMI_DEF_MPDU_LENGTH_MAX)) {
      ELOG_DDD("Incorrect max mpdu length %u, ecpected %u..%u",
               max_mpdu_length, UMI_DEF_MPDU_LENGTH_MIN, UMI_DEF_MPDU_LENGTH_MAX);
      return MTLK_ERR_VALUE;
    }

    ILOG2_DD("CID-%04x: Set MAX MPDU LENGTH to %u", oid, max_mpdu_length);

    /* allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry)
    {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* fill the message data */
    man_entry->id = UM_MAN_MAX_MPDU_LENGTH_REQ;
    man_entry->payload_size = sizeof(UMI_MAX_MPDU);
    mac_msg = (UMI_MAX_MPDU *)man_entry->payload;
    memset(mac_msg, 0, sizeof(*mac_msg));
    mac_msg->maxMpduLength = HOST_TO_MAC32(max_mpdu_length);

    /* send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set UM_MAN_MAX_MPDU_LENGTH_REQ failed, res=%d status=%hhu",
               oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}

static void
_wave_core_store_ap_retry_limit (mtlk_core_t *core, wave_retry_limit_cfg_t *retry_limit_cfg)
{
    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(retry_limit_cfg!= NULL);

    WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_AP_RETRY_LIMIT, retry_limit_cfg->retry_limit.retry_limit_mgmt);
    WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_AP_RETRY_LIMIT_DATA, retry_limit_cfg->retry_limit.retry_limit_data);
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_receive_retry_limit (mtlk_core_t *core, wave_retry_limit_cfg_t *retry_limit_cfg)
{
    mtlk_txmm_msg_t         man_msg;
    mtlk_txmm_data_t        *man_entry;
    UMI_SET_RETRY_LIMIT     *mac_msg;
    mtlk_error_t            res;
    unsigned                oid;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(retry_limit_cfg != NULL);

    oid = mtlk_vap_get_oid(core->vap_handle);
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    man_entry->id = UM_MAN_SET_RETRY_LIMIT_REQ;
    man_entry->payload_size = sizeof(UMI_SET_RETRY_LIMIT);
    mac_msg = (UMI_SET_RETRY_LIMIT *)man_entry->payload;
    mac_msg->getSetOperation = API_GET_OPERATION;

    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK == res && UMI_OK == mac_msg->Status) {
      retry_limit_cfg->retry_limit.retry_limit_mgmt = mac_msg->txRetryLimitMgmt;
      retry_limit_cfg->retry_limit.retry_limit_data = mac_msg->txRetryLimitData;
    }
    else {
      ELOG_DDD("CID-%04x: Receiving UM_MAN_SET_RETRY_LIMIT_REQ failed, res=%d status=%hhu",
              oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    mtlk_txmm_msg_cleanup(&man_msg);
    return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_send_retry_limit (mtlk_core_t *core, wave_retry_limit_cfg_t *retry_limit_cfg)
{

    mtlk_txmm_msg_t         man_msg;
    mtlk_txmm_data_t        *man_entry;
    UMI_SET_RETRY_LIMIT     *mac_msg;
    mtlk_error_t            res;
    unsigned                oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    ILOG1_DDD("CID-%04x: Set Tx Retry Limit - mgmt : %hhu data : %hhu", oid, retry_limit_cfg->retry_limit.retry_limit_mgmt, retry_limit_cfg->retry_limit.retry_limit_data);

    /* allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry)
    {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* fill the message data */
    man_entry->id = UM_MAN_SET_RETRY_LIMIT_REQ;
    man_entry->payload_size = sizeof(UMI_SET_RETRY_LIMIT);
    mac_msg = (UMI_SET_RETRY_LIMIT *)man_entry->payload;
    memset(mac_msg, 0, sizeof(*mac_msg));
    mac_msg->txRetryLimitMgmt = retry_limit_cfg->retry_limit.retry_limit_mgmt;
    mac_msg->txRetryLimitData = retry_limit_cfg->retry_limit.retry_limit_data;

    /* send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set UM_MAN_SET_RETRY_LIMIT_REQ failed, res=%d status=%hhu",
               oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_send_exce_retry_limit (mtlk_core_t *core, uint8 exce_retry_limit)
{
    mtlk_txmm_msg_t         man_msg;
    mtlk_txmm_data_t        *man_entry;
    UMI_SET_EXCESSIVE_RETRIES_LIMIT     *mac_msg;
    mtlk_error_t            res;
    unsigned                oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    ILOG1_DD("CID-%04x: Set Excessive retry limit : %hhu", oid, exce_retry_limit);

    /* allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry)
    {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }
    /* fill the message data */
    man_entry->id = UM_MAN_SET_EXCESSIVE_RETRIES_LIMIT_REQ;
    man_entry->payload_size = sizeof(UMI_SET_EXCESSIVE_RETRIES_LIMIT);
    mac_msg = (UMI_SET_EXCESSIVE_RETRIES_LIMIT *)man_entry->payload;
    memset(mac_msg, 0, sizeof(*mac_msg));
    mac_msg->vapId = (uint8) mtlk_vap_get_id(core->vap_handle);
    mac_msg->excessiveRetriesLimit = exce_retry_limit;
    /* send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set UMI_SET_EXCESSIVE_RETRIES_LIMIT failed, res=%d status=%hhu",
               oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }
    /* cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);
    return res;
}

static int
_mtlk_core_cfg_send_and_store_exce_retry_cfg (mtlk_core_t *core, const uint8 exce_retry_limit)
{
  int res;

  if (exce_retry_limit > STA_EXCE_RETRY_THRESHOLD) {
    ELOG_D("Invalid excessive retry limit : %u", exce_retry_limit);
    return MTLK_ERR_PARAMS;
  }

  res = wave_core_cfg_send_exce_retry_limit(core, exce_retry_limit);
  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    MTLK_CORE_PDB_SET_INT(core, PARAM_DB_CORE_AP_RETRY_LIMIT_EXCE, exce_retry_limit);
  }
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_max_mpdu_length (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_max_mpdu_len_cfg_t *max_mpdu_len_cfg = NULL;
    uint32 max_mpdu_len_cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get configuration */
    max_mpdu_len_cfg = mtlk_clpb_enum_get_next(clpb, &max_mpdu_len_cfg_size);
    MTLK_CLPB_TRY(max_mpdu_len_cfg, max_mpdu_len_cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
        /* send new config to FW */
        MTLK_CFG_CHECK_ITEM_AND_CALL(max_mpdu_len_cfg, max_mpdu_length, mtlk_core_cfg_send_max_mpdu_length,
                                     (core, max_mpdu_len_cfg->max_mpdu_length), res);
        /* store new config in internal db*/
        MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(max_mpdu_len_cfg, max_mpdu_length, _mtlk_core_store_max_mpdu_length,
                                          (core, max_mpdu_len_cfg->max_mpdu_length));
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      /* push result into clipboard */
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_max_mpdu_length (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_max_mpdu_len_cfg_t max_mpdu_len_cfg;
    mtlk_core_t *core = (mtlk_core_t*)hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* read config from internal db */
    MTLK_CFG_SET_ITEM_BY_FUNC(&max_mpdu_len_cfg, max_mpdu_length, mtlk_core_cfg_receive_max_mpdu_length,
                              (core, &max_mpdu_len_cfg.max_mpdu_length), res);

    /* push result and config to clipboard*/
    res = mtlk_clpb_push(clpb, &res, sizeof(res));
    if (MTLK_ERR_OK == res) {
        res = mtlk_clpb_push(clpb, &max_mpdu_len_cfg, sizeof(max_mpdu_len_cfg));
    }

    return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_set_ap_retry_limit (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    mtlk_error_t res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    wave_retry_limit_cfg_t *retry_limit_cfg = NULL;
    uint32 retry_limit_cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get configuration */
    retry_limit_cfg = mtlk_clpb_enum_get_next(clpb, &retry_limit_cfg_size);
    MTLK_CLPB_TRY(retry_limit_cfg, retry_limit_cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
        /* send new config to FW */
        MTLK_CFG_CHECK_ITEM_AND_CALL(retry_limit_cfg, retry_limit, wave_core_cfg_send_retry_limit,
                                     (core, retry_limit_cfg), res);
        /* store new config in internal db*/
        MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(retry_limit_cfg, retry_limit, _wave_core_store_ap_retry_limit,
                                          (core, retry_limit_cfg));
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      /* push result into clipboard */
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_set_ap_exce_retry_limit (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    mtlk_error_t res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_exce_retry_limit_cfg_t *retry_limit_cfg = NULL;
    uint32 retry_limit_cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get configuration */
    retry_limit_cfg = mtlk_clpb_enum_get_next(clpb, &retry_limit_cfg_size);
    MTLK_CLPB_TRY(retry_limit_cfg, retry_limit_cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
        MTLK_CFG_CHECK_ITEM_AND_CALL(retry_limit_cfg, exce_retry_limit, _mtlk_core_cfg_send_and_store_exce_retry_cfg,
                                   (core, retry_limit_cfg->exce_retry_limit), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_get_ap_retry_limit (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    mtlk_error_t res = MTLK_ERR_OK;
    wave_retry_limit_cfg_t  retry_limit_cfg;
    mtlk_core_t *core = (mtlk_core_t*)hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* Get the config from FW */
    MTLK_CFG_SET_ITEM_BY_FUNC(&retry_limit_cfg, retry_limit, wave_core_cfg_receive_retry_limit,
                              (core, &retry_limit_cfg), res);

  /* push result and config to clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &retry_limit_cfg, sizeof(retry_limit_cfg));
}

static int
_mtlk_core_cfg_read_exce_retry_limit (mtlk_core_t *core)
{
   MTLK_ASSERT(core != NULL);
   return MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_AP_RETRY_LIMIT_EXCE);
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_get_ap_exce_retry_limit (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    mtlk_error_t res = MTLK_ERR_OK;
    mtlk_exce_retry_limit_cfg_t  retry_limit_cfg;
    mtlk_core_t *core = (mtlk_core_t*)hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    memset(&retry_limit_cfg, 0, sizeof(retry_limit_cfg));
    MTLK_CFG_SET_ITEM(&retry_limit_cfg, exce_retry_limit, _mtlk_core_cfg_read_exce_retry_limit(core));
    /* push result and config to clipboard */
    return mtlk_clpb_push_res_data(clpb, res, &retry_limit_cfg, sizeof(retry_limit_cfg));
}

/* Frequency Jump Mode Configuration */

int __MTLK_IFUNC
mtlk_core_cfg_send_freq_jump_mode (mtlk_core_t *core, uint32 freq_jump_mode)
{
    mtlk_txmm_msg_t             man_msg;
    mtlk_txmm_data_t            *man_entry;
    UMI_ENABLE_FREQUENCY_JUMP   *mac_msg;
    int                         res;
    uint16                      oid;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(core == mtlk_core_get_master(core));

    oid = mtlk_vap_get_oid(core->vap_handle);
    ILOG2_DD("CID-%04x: Set FREQUENCY JUMP MODE to %d", oid, freq_jump_mode);

    /* allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* fill the message data */
    man_entry->id = UM_MAN_FREQ_JUMP_MODE_REQ;
    man_entry->payload_size = sizeof(UMI_ENABLE_FREQUENCY_JUMP);
    mac_msg = (UMI_ENABLE_FREQUENCY_JUMP *)man_entry->payload;
    memset(mac_msg, 0, sizeof(*mac_msg));

    mac_msg->u8FreqJumpOn = freq_jump_mode;

    /* send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
    if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set FREQUENCY JUMP MODE failed, res=%i status=%hhu",
                oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);
    return res;
}

static int _mtlk_core_cfg_check_freq_jump_mode (mtlk_core_t *core, uint32 freq_jump_mode)
{
    uint16 oid;
    uint32 freq_jump_mode_old =
       WAVE_RADIO_PDB_GET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_FREQ_JUMP_MODE);

    oid = mtlk_vap_get_oid(core->vap_handle);
    if (freq_jump_mode_old == freq_jump_mode) {
      ELOG_D("CID-%04x: Set FREQUENCY JUMP MODE failed, this value set already", oid);
      return MTLK_ERR_PARAMS;
    }
    return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_freq_jump_mode (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_freq_jump_mode_cfg_t *freq_jump_mode_cfg = NULL;
    uint32 freq_jump_mode_cfg_size;
    uint16 oid;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(core == mtlk_core_get_master(core));
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
    oid = mtlk_vap_get_oid(core->vap_handle);

    /* get configuration */
    freq_jump_mode_cfg = mtlk_clpb_enum_get_next(clpb, &freq_jump_mode_cfg_size);
    MTLK_CLPB_TRY(freq_jump_mode_cfg, freq_jump_mode_cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
        /* check new config */
        MTLK_CFG_CHECK_ITEM_AND_CALL(freq_jump_mode_cfg, freq_jump_mode, _mtlk_core_cfg_check_freq_jump_mode,
                                     (core, freq_jump_mode_cfg->freq_jump_mode), res);
        /* send new config to FW */
        MTLK_CFG_CHECK_ITEM_AND_CALL(freq_jump_mode_cfg, freq_jump_mode, mtlk_core_cfg_send_freq_jump_mode,
                                     (core, freq_jump_mode_cfg->freq_jump_mode), res);
        /* store new config in internal DB */
        MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(freq_jump_mode_cfg, freq_jump_mode, WAVE_RADIO_PDB_SET_INT,
                                          (wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_FREQ_JUMP_MODE, freq_jump_mode_cfg->freq_jump_mode));
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      /* push result into clipboard */
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_set_wds_wep_enc_cfg (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *nic = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_core_ui_enc_cfg_t   *enc_cfg = NULL;
  uint32                    enc_cfg_size;
  wave_wep_key_t           *enc_cfg_wep_key;
  int                       res = MTLK_ERR_OK;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  enc_cfg = mtlk_clpb_enum_get_next(clpb, &enc_cfg_size);
  MTLK_CLPB_TRY(enc_cfg, enc_cfg_size)
    if ((mtlk_core_get_net_state(nic) & (NET_STATE_READY | NET_STATE_ACTIVATING | NET_STATE_CONNECTED)) == 0) {
      ILOG1_D("CID-%04x: Invalid card state - request rejected", mtlk_vap_get_oid(nic->vap_handle));
      MTLK_CLPB_EXIT(MTLK_ERR_NOT_READY);
    }

    enc_cfg_wep_key = &enc_cfg->wep_key;

    /* check the Cipher suite */
    if(WAVE_WEP_KEY_WEP2_LENGTH == enc_cfg_wep_key->length) { /* 104 bit */
      nic->slow_ctx->wds_keys[enc_cfg->key_id].cipher = SECURITY_TYPE_SECD_WEP104;
    }
    else if (WAVE_WEP_KEY_WEP1_LENGTH == enc_cfg_wep_key->length) { /* 40 bit */
      nic->slow_ctx->wds_keys[enc_cfg->key_id].cipher = SECURITY_TYPE_SECD_WEP40;
    }
    else {
     nic->slow_ctx->wds_keys[enc_cfg->key_id].cipher = SECURITY_TYPE_SECD_SKIP;
    }

    wave_memcpy(nic->slow_ctx->wds_keys[enc_cfg->key_id].key,
        sizeof(nic->slow_ctx->wds_keys[enc_cfg->key_id].key),
        enc_cfg_wep_key->data, sizeof(enc_cfg_wep_key->data));
    nic->slow_ctx->wds_keys[enc_cfg->key_id].key_len = enc_cfg_wep_key->length;

    ILOG1_DD("CID-%04x: Key index:%d", mtlk_vap_get_oid(nic->vap_handle), enc_cfg->key_id);
    mtlk_dump(1, enc_cfg_wep_key->data, sizeof(enc_cfg_wep_key->data), "KEY:");

  MTLK_CLPB_FINALLY(res)
    /* Don't need to push result to clipboard */
    return res;
  MTLK_CLPB_END;
}

BOOL core_cfg_channels_overlap(struct mtlk_chan_def *chandef1,
  struct mtlk_chan_def *chandef2)
{
#define CORE_CFG_OVERLAP_CHECK(cf1, hw1, cf2, hw2, ret)     \
  do {                                                      \
    if ((cf1 <= cf2) && (cf1 + hw1 > cf2 - hw2)) {          \
      ret = TRUE;                                           \
      break;                                                \
    }                                                       \
    if ((cf1 > cf2) && (cf1 - hw1 < cf2 + hw2)) {           \
      ret = TRUE;                                           \
      break;                                                \
    }                                                       \
    ret = FALSE;                                            \
  } while (0);

  uint32 half_width1, half_width2;
  BOOL ret = FALSE;

  MTLK_ASSERT(chandef1 != NULL);
  MTLK_ASSERT(chandef2 != NULL);

  if ((chandef1->chan.band == MTLK_HW_BAND_6_GHZ) ||
      (chandef2->chan.band == MTLK_HW_BAND_6_GHZ))
    return FALSE;

  half_width1 = mtlkcw2cw(chandef1->width) >> 1;
  half_width2 = mtlkcw2cw(chandef2->width) >> 1;
  CORE_CFG_OVERLAP_CHECK(chandef1->center_freq1, half_width1,
    chandef2->center_freq1, half_width2, ret);
  if (ret)
    goto out;

  if (chandef1->center_freq2 != 0) {
    CORE_CFG_OVERLAP_CHECK(chandef1->center_freq2, half_width1,
      chandef2->center_freq1, half_width2, ret);
    if (ret)
      goto out;
    if (chandef2->center_freq2 != 0) {
      CORE_CFG_OVERLAP_CHECK(chandef1->center_freq2, half_width1,
        chandef2->center_freq2, half_width2, ret);
      if (ret)
        goto out;
    }
  }
  if (chandef2->center_freq2 != 0) {
    CORE_CFG_OVERLAP_CHECK(chandef1->center_freq1, half_width1,
      chandef2->center_freq2, half_width2, ret);
  }

out:
  ILOG2_DDDDDDS("Channels %d--%d (width %u) and %d--%d (width %u) %s",
    freq2lowchannum(chandef1->center_freq1, chandef1->width),
    freq2lowchannum(chandef1->center_freq1, chandef1->width) + (CHANNUMS_PER_20MHZ << chandef1->width) - CHANNUMS_PER_20MHZ,
    mtlkcw2cw(chandef1->width),
    freq2lowchannum(chandef2->center_freq1, chandef2->width),
    freq2lowchannum(chandef2->center_freq1, chandef2->width) + (CHANNUMS_PER_20MHZ << chandef2->width) - CHANNUMS_PER_20MHZ,
    mtlkcw2cw(chandef2->width),
    ret ? "overlap" : "don't overlap");

  return ret;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_operating_mode (mtlk_handle_t hcore, const void *data, uint32 data_size)
{

  mtlk_txmm_msg_t        man_msg;
  mtlk_txmm_data_t       *man_entry = NULL;
  OPERATING_MODE         *pOperatingMode = NULL;
  mtlk_core_t            *core = (mtlk_core_t *) hcore;
  mtlk_vap_handle_t      vap_handle = core->vap_handle;
  mtlk_operating_mode_t  *operating_mode = NULL;
  uint32                  operating_mode_cfg_size;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **) data;
  int                    res = MTLK_ERR_OK;

  ILOG0_V("mtlk_core_cfg_set_operating_mode");

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  operating_mode = mtlk_clpb_enum_get_next(clpb, &operating_mode_cfg_size);
  MTLK_CLPB_TRY(operating_mode, operating_mode_cfg_size)
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
    if (NULL == man_entry) {
      ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
      MTLK_CLPB_EXIT(MTLK_ERR_NO_RESOURCES);
    }

    man_entry->id = UM_MAN_OPERATING_MODE_REQ;
    man_entry->payload_size = sizeof(OPERATING_MODE);

    pOperatingMode = (OPERATING_MODE *)(man_entry->payload);
    pOperatingMode->channelWidth = operating_mode->channel_width;
    pOperatingMode->rxNss = operating_mode->rx_nss;
    pOperatingMode->stationId = HOST_TO_MAC16(operating_mode->station_id);

    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
    if (MTLK_ERR_OK != res) {
      ELOG_DD("CID-%04x: operating mode set failure (%i)", mtlk_vap_get_oid(vap_handle), res);
    }

    mtlk_txmm_msg_cleanup(&man_msg);
  MTLK_CLPB_FINALLY(res)
    /* Don't need to push result to clipboard */
    return res;
  MTLK_CLPB_END
}


int __MTLK_IFUNC mtlk_core_cfg_set_wds_wpa_entry (mtlk_handle_t hcore, const void* data,
  uint32 data_size)
{
  mtlk_core_t *nic = HANDLE_T_PTR(mtlk_core_t, hcore);
  int res = MTLK_ERR_OK;
  struct intel_vendor_mac_addr_list_cfg *addrlist_cfg = NULL;
  uint32 addrlist_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  addrlist_cfg = mtlk_clpb_enum_get_next(clpb, &addrlist_cfg_size);
  MTLK_CLPB_TRY(addrlist_cfg, addrlist_cfg_size)
    if (addrlist_cfg->remove)
      res = mtlk_core_cfg_del_wds_wpa_entry(nic, &addrlist_cfg->addr);
    else
      res = mtlk_core_cfg_add_wds_wpa_entry(nic, &addrlist_cfg->addr);
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

/* Restricted AC Mode Configuration */

static int
_mtlk_core_receive_restricted_ac_mode (mtlk_core_t *core, UMI_SET_RESTRICTED_AC *restricted_ac_mode)
{
  mtlk_txmm_msg_t             man_msg;
  mtlk_txmm_data_t            *man_entry;
  UMI_SET_RESTRICTED_AC       *mac_msg;
  int                         res;
  uint16                      oid;

  oid = mtlk_vap_get_oid(core->vap_handle);
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_RESTRICTED_AC_MODE_REQ;
  man_entry->payload_size = sizeof(UMI_SET_RESTRICTED_AC);
  mac_msg = (UMI_SET_RESTRICTED_AC *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == mac_msg->Status) {
    restricted_ac_mode->restrictedAcThreshEnter = MAC_TO_HOST16(mac_msg->restrictedAcThreshEnter);
    restricted_ac_mode->restrictedAcThreshExit  = MAC_TO_HOST16(mac_msg->restrictedAcThreshExit);
    restricted_ac_mode->restrictedAcModeEnable  = mac_msg->restrictedAcModeEnable;
    restricted_ac_mode->acRestrictedBitmap      = mac_msg->acRestrictedBitmap;
  }
  else {
    ELOG_DDD("CID-%04x: Receiving UM_MAN_RESTRICTED_AC_MODE_REQ failed, res=%i status=%hhu",
            oid, res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_send_restricted_ac_mode (mtlk_core_t *core,
        const UMI_SET_RESTRICTED_AC *restricted_ac_mode)
{
  mtlk_txmm_msg_t             man_msg;
  mtlk_txmm_data_t            *man_entry;
  UMI_SET_RESTRICTED_AC       *mac_msg;
  int                         res;
  uint16                      oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Send RESTRICTED AC MODE", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_RESTRICTED_AC_MODE_REQ;
  man_entry->payload_size = sizeof(UMI_SET_RESTRICTED_AC);
  mac_msg = (UMI_SET_RESTRICTED_AC *)man_entry->payload;
  memset(mac_msg, 0, sizeof(*mac_msg));

  mac_msg->restrictedAcThreshEnter = HOST_TO_MAC16(restricted_ac_mode->restrictedAcThreshEnter);
  mac_msg->restrictedAcThreshExit = HOST_TO_MAC16(restricted_ac_mode->restrictedAcThreshExit);
  mac_msg->restrictedAcModeEnable = restricted_ac_mode->restrictedAcModeEnable;
  mac_msg->acRestrictedBitmap = restricted_ac_mode->acRestrictedBitmap;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
    ELOG_DDD("CID-%04x: Set RESTRICTED AC MODE failed, res=%i status=%hhu", oid, res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int
_mtlk_core_receive_pd_threshold (mtlk_core_t *core, UMI_SET_PD_THRESH *pd_thresh)
{
  mtlk_txmm_msg_t             man_msg;
  mtlk_txmm_data_t            *man_entry;
  UMI_SET_PD_THRESH           *mac_msg;
  int                         res;
  uint16                      oid;


  oid = mtlk_vap_get_oid(core->vap_handle);
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_QOS_PD_THRESH_REQ;
  man_entry->payload_size = sizeof(UMI_SET_PD_THRESH);
  mac_msg = (UMI_SET_PD_THRESH *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == mac_msg->Status) {
    pd_thresh->minPdDiff   = MAC_TO_HOST16(mac_msg->minPdDiff);
    pd_thresh->mode        = mac_msg->mode;
    pd_thresh->minPdAmount = mac_msg->minPdAmount;
  }
  else {
    ELOG_DDD("CID-%04x: Receiving UM_MAN_QOS_PD_THRESH_REQ failed, res=%i status=%hhu",
            oid, res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_send_pd_threshold (mtlk_core_t *core, const UMI_SET_PD_THRESH *pd_thresh)
{
  mtlk_txmm_msg_t             man_msg;
  mtlk_txmm_data_t            *man_entry;
  UMI_SET_PD_THRESH           *mac_msg;
  int                         res;
  uint16                      oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Send PD THRESHOLD", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_QOS_PD_THRESH_REQ;
  man_entry->payload_size = sizeof(UMI_SET_PD_THRESH);
  mac_msg = (UMI_SET_PD_THRESH *)man_entry->payload;
  memset(mac_msg, 0, sizeof(*mac_msg));

  mac_msg->minPdDiff = HOST_TO_MAC16(pd_thresh->minPdDiff);
  mac_msg->mode = pd_thresh->mode;
  mac_msg->minPdAmount = pd_thresh->minPdAmount;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
    ELOG_DDD("CID-%04x: Send PD THRESHOLD failed, res=%i status=%hhu", oid, res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

void __MTLK_IFUNC
mtlk_core_cfg_get_restricted_ac_mode (mtlk_core_t *core,
        UMI_SET_RESTRICTED_AC *restricted_ac_mode)
{
  mtlk_pdb_size_t data_size = sizeof(*restricted_ac_mode);
  WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                            PARAM_DB_RADIO_RESTRICTED_AC_MODE, restricted_ac_mode, &data_size);
}

void __MTLK_IFUNC
mtlk_core_cfg_get_pd_threshold (mtlk_core_t *core, UMI_SET_PD_THRESH *pd_thresh)
{
  mtlk_pdb_size_t data_size = sizeof(*pd_thresh);
  WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                            PARAM_DB_RADIO_PD_THRESHOLD, pd_thresh, &data_size);
}

static int
_mtlk_core_cfg_check_restricted_ac_mode (mtlk_core_t *core,
        const UMI_SET_RESTRICTED_AC *restricted_ac_mode)
{
  uint16 oid = mtlk_vap_get_oid(core->vap_handle);

  MTLK_UNREFERENCED_PARAM(oid);

  if (restricted_ac_mode->restrictedAcThreshEnter > MTLK_RESTRICTED_AC_MODE_THRESH_EXIT_MAX) {
    ELOG_DDD("CID-%04x: Invalid ThresholdEnter value (%d). Allowed values are 0...%d.",
            oid, restricted_ac_mode->restrictedAcThreshEnter, MTLK_RESTRICTED_AC_MODE_THRESH_EXIT_MAX);
    return MTLK_ERR_VALUE;
  }
  if (restricted_ac_mode->restrictedAcThreshExit > MTLK_RESTRICTED_AC_MODE_THRESH_EXIT_MAX) {
    ELOG_DDD("CID-%04x: Invalid ThresholdExit value (%d). Allowed values are 0...%d.",
            oid, restricted_ac_mode->restrictedAcThreshExit, MTLK_RESTRICTED_AC_MODE_THRESH_EXIT_MAX);
    return MTLK_ERR_VALUE;
  }
  return MTLK_ERR_OK;
}

static int
_mtlk_core_cfg_send_and_store_restricted_ac_mode (mtlk_core_t *core,
        const UMI_SET_RESTRICTED_AC *restricted_ac_mode)
{
  int res;
  uint16 oid;

  oid = mtlk_vap_get_oid(core->vap_handle);
  /* check parameters */
  res = _mtlk_core_cfg_check_restricted_ac_mode(core, restricted_ac_mode);

  if (MTLK_ERR_OK == res) {
    /* send new config to FW */
    res = mtlk_core_cfg_send_restricted_ac_mode(core, restricted_ac_mode);
    if (MTLK_ERR_OK == res) {
      /* store new config into internal DB */
      res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                      PARAM_DB_RADIO_RESTRICTED_AC_MODE,
                                      restricted_ac_mode, sizeof(*restricted_ac_mode));
      if (MTLK_ERR_OK != res) {
        ELOG_DD("CID-%04x: Store RESTRICTED AC MODE failed (%i)", oid, res);
      }
    }
  }
  return res;
}

static int
_mtlk_core_cfg_send_and_store_pd_threshold (mtlk_core_t *core,
        const UMI_SET_PD_THRESH *pd_thresh)
{
  /* send new config to FW */
  int res = mtlk_core_cfg_send_pd_threshold(core, pd_thresh);

  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                    PARAM_DB_RADIO_PD_THRESHOLD,
                                    pd_thresh, sizeof(*pd_thresh));
    if (MTLK_ERR_OK != res) {
      ELOG_DD("CID-%04x: Store PD THRESHOLD failed (%i)",
              mtlk_vap_get_oid(core->vap_handle), res);
    }
  }
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_restricted_ac_mode_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  uint32 restricted_ac_mode_cfg_size;
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_restricted_ac_mode_cfg_t *restricted_ac_mode_cfg = NULL;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  restricted_ac_mode_cfg = mtlk_clpb_enum_get_next(clpb, &restricted_ac_mode_cfg_size);
  MTLK_CLPB_TRY(restricted_ac_mode_cfg, restricted_ac_mode_cfg_size)
    /* send configuration */
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(restricted_ac_mode_cfg, pd_thresh_params, _mtlk_core_cfg_send_and_store_pd_threshold,
                                   (core, &restricted_ac_mode_cfg->pd_thresh_params), res);
      MTLK_CFG_CHECK_ITEM_AND_CALL(restricted_ac_mode_cfg, ac_mode_params, _mtlk_core_cfg_send_and_store_restricted_ac_mode,
                                   (core, &restricted_ac_mode_cfg->ac_mode_params), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_restricted_ac_mode_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_restricted_ac_mode_cfg_t restricted_ac_mode_cfg;
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&restricted_ac_mode_cfg, 0, sizeof(restricted_ac_mode_cfg));

  MTLK_CFG_SET_ITEM_BY_FUNC(&restricted_ac_mode_cfg, pd_thresh_params,
                            _mtlk_core_receive_pd_threshold, (core, &restricted_ac_mode_cfg.pd_thresh_params), res);
  MTLK_CFG_SET_ITEM_BY_FUNC(&restricted_ac_mode_cfg, ac_mode_params,
                            _mtlk_core_receive_restricted_ac_mode, (core, &restricted_ac_mode_cfg.ac_mode_params), res);

  /* push result into clipboard */
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &restricted_ac_mode_cfg, sizeof(restricted_ac_mode_cfg));
  }
  return res;
}

int __MTLK_IFUNC
mtlk_core_set_mu_operation (mtlk_core_t *core, uint8 mu_operation)
{
  mtlk_txmm_msg_t                 man_msg;
  mtlk_txmm_data_t               *man_entry = NULL;
  UMI_MU_OPERATION_CONFIG        *mu_operation_config = NULL;
  mtlk_vap_handle_t               vap_handle = core->vap_handle;
  int                             res;
  wave_radio_t                   *radio = wave_vap_radio_get(vap_handle);

  ILOG2_V("Sending UMI_MU_OPERATION_CONFIG");

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_MU_OPERATION_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_MU_OPERATION_CONFIG);

  mu_operation_config = (UMI_MU_OPERATION_CONFIG *)(man_entry->payload);
  mu_operation_config->enableMuOperation = mu_operation;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != mu_operation_config->Status) {
    ELOG_DDD("CID-%04x: Set UMI_MU_OPERATION_CONFIG failed, res=%d status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, mu_operation_config->Status);
    if (UMI_OK != mu_operation_config->Status)
      res = MTLK_ERR_MAC;
  }
  else {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_MU_OPERATION, mu_operation);
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_receive_mu_operation (mtlk_core_t *core, BOOL *mu_operation)
{
  mtlk_txmm_msg_t                 man_msg;
  mtlk_txmm_data_t               *man_entry = NULL;
  UMI_MU_OPERATION_CONFIG        *mu_operation_config = NULL;
  mtlk_vap_handle_t               vap_handle = core->vap_handle;
  int                             res;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_MU_OPERATION_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_MU_OPERATION_CONFIG);

  mu_operation_config = (UMI_MU_OPERATION_CONFIG *)(man_entry->payload);
  mu_operation_config->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == mu_operation_config->Status) {
    *mu_operation = mu_operation_config->enableMuOperation;
  }
  else {
    ELOG_DDD("CID-%04x: Receive UMI_MU_OPERATION_CONFIG failed, res=%i status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, mu_operation_config->Status);
    if (UMI_OK != mu_operation_config->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
wave_core_set_rts_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{

  int                    res = MTLK_ERR_OK;
  mtlk_core_t            *core = (mtlk_core_t*)hcore;
  mtlk_wlan_rts_threshold_cfg_t   *rts_threshold = NULL;
  uint32                 rts_threshold_size;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **)data;
  ILOG1_D("wave_core_set_rts_threshold = %i", data_size);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  /* get configuration */
  rts_threshold = mtlk_clpb_enum_get_next(clpb, &rts_threshold_size);
  MTLK_CLPB_TRY(rts_threshold, rts_threshold_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* send new config to FW */
      MTLK_CFG_CHECK_ITEM_AND_CALL(rts_threshold, threshold, _wave_core_cfg_set_rts_threshold,
               (core, rts_threshold->threshold), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END

}

int __MTLK_IFUNC
_wave_core_cfg_set_rts_threshold (mtlk_core_t *core, uint32 rts_threshold)
{
  mtlk_txmm_msg_t        man_msg;
  mtlk_txmm_data_t       *man_entry = NULL;
  UMI_SET_RTS_THRESHOLD  *rts_threshold_config = NULL;
  mtlk_vap_handle_t      vap_handle = core->vap_handle;
  int                    res;
  wave_radio_t           *radio = wave_vap_radio_get(vap_handle);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_RTS_THRESHOLD_REQ;
  man_entry->payload_size = sizeof(UMI_SET_RTS_THRESHOLD);

  rts_threshold_config = (UMI_SET_RTS_THRESHOLD *)(man_entry->payload);
  rts_threshold_config->newRtsThreshold = HOST_TO_MAC16((uint16)rts_threshold);
  rts_threshold_config->vapId = 0;
  rts_threshold_config->isDisabled = (rts_threshold == 0xFFFFFFFF) ? 1: 0;
  rts_threshold_config->getSetOperation = API_SET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != rts_threshold_config->Status) {
    ELOG_DDD("CID-%04x: Set rts_threshold_config failed, res=%d status=%hhu",
          mtlk_vap_get_oid(vap_handle), res, rts_threshold_config->Status);
    if (UMI_OK != rts_threshold_config->Status)
      res = MTLK_ERR_MAC;
  }
  else {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RTS_THRESH, rts_threshold);
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
wave_core_cfg_recover_rts_threshold (mtlk_core_t *core)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  uint32 rts_threshold  = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_RTS_THRESH);

  if (MTLK_PARAM_DB_VALUE_IS_INVALID(rts_threshold))
      return MTLK_ERR_OK;

  return _wave_core_cfg_set_rts_threshold(core, rts_threshold);
}

int __MTLK_IFUNC
wave_core_get_rts_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_wlan_rts_threshold_cfg_t rts_threshold;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* read config from internal db */
  MTLK_CFG_SET_ITEM_BY_FUNC(&rts_threshold, threshold, _wave_core_cfg_get_rts_threshold,
    (core, &rts_threshold.threshold),res);

  /* push result and config to clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &rts_threshold, sizeof(rts_threshold));
}

int __MTLK_IFUNC
_wave_core_cfg_get_rts_threshold (mtlk_core_t *core, uint32 *rts_threshold)
{
  int    res = MTLK_ERR_OK;
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);

  *rts_threshold = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_RTS_THRESH);

  if(MTLK_PARAM_DB_VALUE_IS_INVALID(*rts_threshold)) {
    res = _wave_core_cfg_receive_rts_threshold (core,rts_threshold);
    if (res == MTLK_ERR_OK) {
      WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RTS_THRESH, *rts_threshold);
    }
  }
  return res;
}

int __MTLK_IFUNC
_wave_core_cfg_receive_rts_threshold (mtlk_core_t *core, uint32 *rts_threshold)
{
  mtlk_txmm_msg_t 	   man_msg;
  mtlk_txmm_data_t	   *man_entry = NULL;
  UMI_SET_RTS_THRESHOLD  *rts_threshold_config = NULL;
  mtlk_vap_handle_t	   vap_handle = core->vap_handle;
  int res;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_RTS_THRESHOLD_REQ;
  man_entry->payload_size = sizeof(UMI_SET_RTS_THRESHOLD);

  rts_threshold_config = (UMI_SET_RTS_THRESHOLD *)(man_entry->payload);
  rts_threshold_config->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == rts_threshold_config->Status) {
    *rts_threshold = MAC_TO_HOST16(rts_threshold_config->newRtsThreshold);
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_SET_RTS_THRESHOLD_REQ failed, res=%d status=%hhu",
    mtlk_vap_get_oid(vap_handle), res, rts_threshold_config->Status);
    if (UMI_OK != rts_threshold_config->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_set_rts_mode (mtlk_core_t *core, uint8 dynamic_bw, uint8 static_bw)
{
  mtlk_txmm_msg_t      man_msg;
  mtlk_txmm_data_t    *man_entry = NULL;
  UMI_RTS_MODE_CONFIG *rts_mode_config = NULL;
  mtlk_vap_handle_t    vap_handle = core->vap_handle;
  int                  res;
  wave_radio_t         *radio = wave_vap_radio_get(vap_handle);

  ILOG2_V("Sending UMI_RTS_MODE_CONFIG");

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_RTS_MODE_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_RTS_MODE_CONFIG);

  rts_mode_config = (UMI_RTS_MODE_CONFIG *)(man_entry->payload);
  rts_mode_config->dynamicBw = dynamic_bw;
  rts_mode_config->staticBw = static_bw;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != rts_mode_config->Status) {
    ELOG_DDD("CID-%04x: Set UMI_RTS_MODE_CONFIG failed, res=%d status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, rts_mode_config->Status);
    if (UMI_OK != rts_mode_config->Status)
      res = MTLK_ERR_MAC;
  }
  else {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RTS_DYNAMIC_BW, dynamic_bw);
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RTS_STATIC_BW, static_bw);
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_receive_rts_mode (mtlk_core_t *core, mtlk_core_rts_mode_t *rts_params)
{
  mtlk_txmm_msg_t      man_msg;
  mtlk_txmm_data_t    *man_entry = NULL;
  UMI_RTS_MODE_CONFIG *rts_mode_config = NULL;
  mtlk_vap_handle_t    vap_handle = core->vap_handle;
  int                  res;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_RTS_MODE_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_RTS_MODE_CONFIG);

  rts_mode_config = (UMI_RTS_MODE_CONFIG *)(man_entry->payload);
  rts_mode_config->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == rts_mode_config->Status) {
    rts_params->dynamic_bw = rts_mode_config->dynamicBw;
    rts_params->static_bw  = rts_mode_config->staticBw;
  }
  else {
    ELOG_DDD("CID-%04x: Receive UMI_RTS_MODE_CONFIG failed, res=%d status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, rts_mode_config->Status);
    if (UMI_OK != rts_mode_config->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_set_reliable_multicast (mtlk_core_t *core, uint8 flag)
{
  int                 res = MTLK_ERR_OK;
  mtlk_txmm_msg_t     man_msg;
  mtlk_txmm_data_t   *man_entry = NULL;
  UMI_MULTICAST_MODE *pMcastMode = NULL;
  mtlk_vap_handle_t   vap_handle = core->vap_handle;

  MTLK_ASSERT(vap_handle != 0);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    res = MTLK_ERR_NO_RESOURCES;
    goto FINISH;
  }

  man_entry->id = UM_MAN_SET_MULTICAST_MODE_REQ;
  man_entry->payload_size = sizeof(UMI_MULTICAST_MODE);

  pMcastMode = (UMI_MULTICAST_MODE *)(man_entry->payload);
  pMcastMode->u8Action = flag;
  pMcastMode->u8VapID = mtlk_vap_get_id(vap_handle);
  pMcastMode->getSetOperation = API_SET_OPERATION;

  ILOG2_DSD("CID-%04x: Multicast FW request: Set %s mode for VapID %u",
            mtlk_vap_get_oid(vap_handle), flag ? "Reliable Multicast" : "Regular Multicast", pMcastMode->u8VapID);

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != pMcastMode->Status) {
    ELOG_DDD("CID-%04x: Set Reliable Multicast failure, res=%i status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, pMcastMode->Status);
    if (UMI_OK != pMcastMode->Status)
      res = MTLK_ERR_MAC;
    goto FINISH;
  } else {
    MTLK_CORE_PDB_SET_INT(core, PARAM_DB_CORE_RELIABLE_MCAST, flag);
  }

FINISH:
  if (man_entry) {
    mtlk_txmm_msg_cleanup(&man_msg);
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_get_reliable_multicast (mtlk_core_t *core, uint8 *flag)
{
  int   res = MTLK_ERR_OK;
  uint8 rel_mc_value;

  rel_mc_value = MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_RELIABLE_MCAST);

  if(MTLK_PARAM_DB_VALUE_IS_INVALID(rel_mc_value)) {
    res = mtlk_core_receive_reliable_multicast(core, flag);
    if(res == MTLK_ERR_OK) {
      MTLK_CORE_PDB_SET_INT(core, PARAM_DB_CORE_RELIABLE_MCAST, *flag);
    }
    return res;
  } else {
    *flag = rel_mc_value;
    res = MTLK_ERR_OK;
    return res;
  }
}

int __MTLK_IFUNC
mtlk_core_receive_reliable_multicast (mtlk_core_t *core, uint8 *flag)
{
  int                 res = MTLK_ERR_OK;
  mtlk_txmm_msg_t     man_msg;
  mtlk_txmm_data_t   *man_entry = NULL;
  UMI_MULTICAST_MODE *pMcastMode = NULL;
  mtlk_vap_handle_t   vap_handle = core->vap_handle;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_MULTICAST_MODE_REQ;
  man_entry->payload_size = sizeof(UMI_MULTICAST_MODE);

  pMcastMode = (UMI_MULTICAST_MODE *)(man_entry->payload);
  pMcastMode->getSetOperation = API_GET_OPERATION;
  pMcastMode->u8VapID = mtlk_vap_get_id(vap_handle);

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == pMcastMode->Status) {
      *flag = pMcastMode->u8Action;
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_SET_MULTICAST_MODE_REQ failed, res=%i status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, pMcastMode->Status);
    if (UMI_OK != pMcastMode->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

/********** Reception Duty Cycle setting *********************/
static void
_mtlk_core_store_rx_duty_cycle (mtlk_core_t *core, const mtlk_rx_duty_cycle_cfg_t *rx_duty_cycle_cfg)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(rx_duty_cycle_cfg != NULL);

  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RX_DUTY_CYCLE_ON_TIME,
                         rx_duty_cycle_cfg->duty_cycle.onTime);
  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RX_DUTY_CYCLE_OFF_TIME,
                         rx_duty_cycle_cfg->duty_cycle.offTime);
}

int __MTLK_IFUNC
mtlk_core_set_rx_duty_cycle (mtlk_core_t *core, const mtlk_rx_duty_cycle_cfg_t *rx_duty_cycle_cfg)
{
  mtlk_txmm_msg_t    man_msg;
  mtlk_txmm_data_t  *man_entry;
  UMI_RX_DUTY_CYCLE *mac_msg;
  int                res;

  MTLK_ASSERT((rx_duty_cycle_cfg->duty_cycle.onTime<=32767) && (rx_duty_cycle_cfg->duty_cycle.offTime<=32767));

  ILOG2_DDD("CID-%04x: Set RX Duty Cycle. onTime: %u, offTime: %u",
          mtlk_vap_get_oid(core->vap_handle), rx_duty_cycle_cfg->duty_cycle.onTime, rx_duty_cycle_cfg->duty_cycle.offTime);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", mtlk_vap_get_oid(core->vap_handle));
      return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_SET_RX_DUTY_CYCLE_REQ;
  man_entry->payload_size = sizeof(UMI_RX_DUTY_CYCLE);
  mac_msg = (UMI_RX_DUTY_CYCLE *)man_entry->payload;
  mac_msg->onTime  = HOST_TO_MAC32(rx_duty_cycle_cfg->duty_cycle.onTime);
  mac_msg->offTime = HOST_TO_MAC32(rx_duty_cycle_cfg->duty_cycle.offTime);

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (res != MTLK_ERR_OK || UMI_OK != mac_msg->Status) {
    ELOG_DDD("CID-%04x: Set UM_MAN_SET_RX_DUTY_CYCLE_REQ failed, res=%d status=%hhu",
            mtlk_vap_get_oid(core->vap_handle), res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

static int
_mtlk_core_receive_rx_duty_cycle (mtlk_core_t *core, UMI_RX_DUTY_CYCLE *rx_duty_cycle_cfg)
{
  mtlk_txmm_msg_t    man_msg;
  mtlk_txmm_data_t  *man_entry;
  UMI_RX_DUTY_CYCLE *mac_msg;
  int                res;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", mtlk_vap_get_oid(core->vap_handle));
      return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_RX_DUTY_CYCLE_REQ;
  man_entry->payload_size = sizeof(UMI_RX_DUTY_CYCLE);
  mac_msg = (UMI_RX_DUTY_CYCLE *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == mac_msg->Status) {
    rx_duty_cycle_cfg->onTime  = MAC_TO_HOST32(mac_msg->onTime);
    rx_duty_cycle_cfg->offTime = MAC_TO_HOST32(mac_msg->offTime);
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_SET_RX_DUTY_CYCLE_REQ failed, res=%d status=%hhu",
            mtlk_vap_get_oid(core->vap_handle), res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_rx_duty_cycle (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;

  mtlk_rx_duty_cycle_cfg_t *duty_cycle_cfg = NULL;
  uint32 duty_cycle_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  duty_cycle_cfg = mtlk_clpb_enum_get_next(clpb, &duty_cycle_cfg_size);
  MTLK_CLPB_TRY(duty_cycle_cfg, duty_cycle_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* send new config to FW */
      MTLK_CFG_CHECK_ITEM_AND_CALL(duty_cycle_cfg, duty_cycle, mtlk_core_set_rx_duty_cycle,
                                    (core, duty_cycle_cfg), res);
      /* store new config in internal db*/
      MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(duty_cycle_cfg, duty_cycle, _mtlk_core_store_rx_duty_cycle,
                                        (core, duty_cycle_cfg));
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_rx_duty_cycle (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_rx_duty_cycle_cfg_t rx_duty_cycle_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&rx_duty_cycle_cfg, 0, sizeof(rx_duty_cycle_cfg));
  MTLK_CFG_SET_ITEM_BY_FUNC(&rx_duty_cycle_cfg, duty_cycle, _mtlk_core_receive_rx_duty_cycle,
                           (core, &rx_duty_cycle_cfg.duty_cycle), res);

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
      res = mtlk_clpb_push(clpb, &rx_duty_cycle_cfg, sizeof(rx_duty_cycle_cfg));
  }

  return res;
}

/************** Set High Reception Threshold ****************/
static void
_mtlk_core_store_rx_threshold (mtlk_core_t *core, const mtlk_rx_th_cfg_t *rx_th_cfg)
{
  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(rx_th_cfg != NULL);

  MTLK_CORE_PDB_SET_INT(core, PARAM_DB_CORE_RX_THRESHOLD, rx_th_cfg->rx_threshold);
}

int __MTLK_IFUNC
mtlk_core_set_rx_threshold (mtlk_core_t *core, const mtlk_rx_th_cfg_t *rx_th_cfg)
{
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_RX_TH        *mac_msg;
  int               res;

  ILOG2_DD("CID-%04x: Set RX threshold: %d", mtlk_vap_get_oid(core->vap_handle), rx_th_cfg->rx_threshold);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry)
  {
      ELOG_D("CID-%04x: Can not get TXMM slot", mtlk_vap_get_oid(core->vap_handle));
      return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_SET_RX_TH_REQ;
  man_entry->payload_size = sizeof(UMI_RX_TH);
  mac_msg = (UMI_RX_TH *)man_entry->payload;
  mac_msg->rxThValue = rx_th_cfg->rx_threshold;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (res != MTLK_ERR_OK || UMI_OK != mac_msg->Status) {
    ELOG_DDD("CID-%04x: Set UM_MAN_SET_RX_TH_REQ failed, res=%d status=%hhu",
            mtlk_vap_get_oid(core->vap_handle), res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

static int
_mtlk_core_receive_rx_threshold (mtlk_core_t *core, int8 *rx_thresh)
{
  mtlk_txmm_msg_t   man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_RX_TH        *mac_msg;
  int               res;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", mtlk_vap_get_oid(core->vap_handle));
      return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_RX_TH_REQ;
  man_entry->payload_size = sizeof(UMI_RX_TH);
  mac_msg = (UMI_RX_TH *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == mac_msg->Status) {
    *rx_thresh = mac_msg->rxThValue;
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_SET_RX_TH_REQ failed, res=%i status=%hhu",
            mtlk_vap_get_oid(core->vap_handle), res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_rx_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;

  mtlk_rx_th_cfg_t *rxth_cfg = NULL;
  uint32 rxth_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_card_type_info_t card_type_info;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  mtlk_hw_get_prop(mtlk_vap_get_hwapi(core->vap_handle), MTLK_HW_PROP_CARD_TYPE_INFO,
                      &card_type_info, sizeof(&card_type_info));

  /* get configuration */
  rxth_cfg = mtlk_clpb_enum_get_next(clpb, &rxth_cfg_size);
  MTLK_CLPB_TRY(rxth_cfg, rxth_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* send new config to FW */
      if (_mtlk_card_is_asic(card_type_info)) { /* avoid for non ASIC, i.e. FPGA/Emul due to timing */
        MTLK_CFG_CHECK_ITEM_AND_CALL(rxth_cfg, rx_threshold, mtlk_core_set_rx_threshold,
                                      (core, rxth_cfg), res);
      }
      /* store new config in internal db*/
      MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(rxth_cfg, rx_threshold, _mtlk_core_store_rx_threshold,
                                        (core, rxth_cfg));
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_rx_threshold (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_rx_th_cfg_t rxth_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* read config from internal db */
  memset(&rxth_cfg, 0, sizeof(rxth_cfg));
  MTLK_CFG_SET_ITEM_BY_FUNC(&rxth_cfg, rx_threshold, _mtlk_core_receive_rx_threshold,
                            (core, &rxth_cfg.rx_threshold), res);

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
      res = mtlk_clpb_push(clpb, &rxth_cfg, sizeof(rxth_cfg));
  }

  return res;
}

/* TxOP Configuration */

mtlk_error_t __MTLK_IFUNC wave_core_cfg_receive_txop_params (mtlk_core_t *core, UMI_SET_TXOP_CONFIG *txop_params)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_SET_TXOP_CONFIG *mac_msg;
  mtlk_error_t res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Receive TxOP Parameters", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_SET_TXOP_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_SET_TXOP_CONFIG);
  mac_msg = (UMI_SET_TXOP_CONFIG *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if ((MTLK_ERR_OK == res) && (UMI_OK == mac_msg->Status)) {
    txop_params->staId = MAC_TO_HOST16(mac_msg->staId);
    txop_params->txopDuration = MAC_TO_HOST16(mac_msg->txopDuration);
    txop_params->maxNumOfStas = MAC_TO_HOST16(mac_msg->maxNumOfStas);
    txop_params->mode = mac_msg->mode;
  }
  else {
    ELOG_DDD("CID-%04x: Receive TxOP Parameters failed (res=%d, status=%hhu)", oid, res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

mtlk_error_t __MTLK_IFUNC wave_core_cfg_send_txop_params (mtlk_core_t *core, const UMI_SET_TXOP_CONFIG *txop_params)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_SET_TXOP_CONFIG *mac_msg;
  mtlk_error_t res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Send TxOP Parameters", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_SET_TXOP_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_SET_TXOP_CONFIG);
  mac_msg = (UMI_SET_TXOP_CONFIG *)man_entry->payload;
  mac_msg->getSetOperation = API_SET_OPERATION;

  mac_msg->staId = HOST_TO_MAC16(txop_params->staId);
  mac_msg->txopDuration = HOST_TO_MAC16(txop_params->txopDuration);
  mac_msg->maxNumOfStas = HOST_TO_MAC16(txop_params->maxNumOfStas);
  mac_msg->mode = txop_params->mode;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if ((MTLK_ERR_OK != res) || (UMI_OK != mac_msg->Status)) {
    ELOG_DDD("CID-%04x: Send TxOP Parameters failed (res=%d, status=%hhu)", oid, res, mac_msg->Status);
    if (UMI_OK != mac_msg->Status)
      res = MTLK_ERR_MAC;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

mtlk_error_t __MTLK_IFUNC wave_core_cfg_get_txop_params (mtlk_core_t *core, UMI_SET_TXOP_CONFIG *txop_params)
{
  mtlk_error_t res;
  mtlk_pdb_size_t data_size = sizeof(*txop_params);

  res = WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                                  PARAM_DB_RADIO_TXOP_CFG, txop_params, &data_size);
  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Read TxOP Parameters from DB failed (res=%d)",
            mtlk_vap_get_oid(core->vap_handle), res);
  }
  return res;
}

mtlk_error_t __MTLK_IFUNC wave_core_cfg_send_and_store_txop_params (mtlk_core_t *core,
                                                                    const UMI_SET_TXOP_CONFIG *txop_params)
{
  /* send new config to FW */
  mtlk_error_t res = wave_core_cfg_send_txop_params(core, txop_params);

  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                    PARAM_DB_RADIO_TXOP_CFG,
                                    txop_params, sizeof(*txop_params));
    if (MTLK_ERR_OK != res) {
      ELOG_DD("CID-%04x: Store TxOP Parameters in DB failed (res=%d)",
              mtlk_vap_get_oid(core->vap_handle), res);
    }
  }
  return res;
}

int __MTLK_IFUNC
mtlk_core_set_admission_capacity (mtlk_core_t *core, uint32 value)
{
  int                           res = MTLK_ERR_OK;
  mtlk_txmm_msg_t               man_msg;
  mtlk_txmm_data_t              *man_entry = NULL;
  UMI_UPDATE_ADMISSION_CAPACITY *pAvailAdmissionCap = NULL;
  mtlk_vap_handle_t             vap_handle = core->vap_handle;

  MTLK_ASSERT(vap_handle != 0);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    res = MTLK_ERR_NO_RESOURCES;
    goto FINISH;
  }

  man_entry->id = UM_MAN_SET_ADMISSION_CAPACITY_REQ;
  man_entry->payload_size = sizeof(UMI_UPDATE_ADMISSION_CAPACITY);

  pAvailAdmissionCap = (UMI_UPDATE_ADMISSION_CAPACITY *)(man_entry->payload);
  pAvailAdmissionCap->availableAdmissionCapacity = HOST_TO_MAC32(value);

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != pAvailAdmissionCap->Status) {
    ELOG_DDD("CID-%04x: Set Availbale Admission Capacity failure, res=%i status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, pAvailAdmissionCap->Status);
    if (UMI_OK != pAvailAdmissionCap->Status)
      res = MTLK_ERR_MAC;
  } else {
    MTLK_CORE_PDB_SET_INT(core, PARAM_DB_CORE_ADMISSION_CAPACITY, value);
  }

  mtlk_txmm_msg_cleanup(&man_msg);

FINISH:
  return res;
}

int __MTLK_IFUNC
mtlk_core_receive_admission_capacity (mtlk_core_t *core, uint32 *value)
{
  int                           res = MTLK_ERR_OK;
  mtlk_txmm_msg_t               man_msg;
  mtlk_txmm_data_t              *man_entry = NULL;
  UMI_UPDATE_ADMISSION_CAPACITY *pAvailAdmissionCap = NULL;
  mtlk_vap_handle_t             vap_handle = core->vap_handle;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_ADMISSION_CAPACITY_REQ;
  man_entry->payload_size = sizeof(UMI_UPDATE_ADMISSION_CAPACITY);
  pAvailAdmissionCap = (UMI_UPDATE_ADMISSION_CAPACITY *)(man_entry->payload);
  pAvailAdmissionCap->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == pAvailAdmissionCap->Status) {
      *value = MAC_TO_HOST32(pAvailAdmissionCap->availableAdmissionCapacity);
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_SET_ADMISSION_CAPACITY_REQ failed, res=%d status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, pAvailAdmissionCap->Status);
    if (UMI_OK != pAvailAdmissionCap->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_send_fast_drop (mtlk_core_t *core, uint8 fast_drop)
{
  int                         res;
  mtlk_txmm_msg_t             man_msg;
  mtlk_txmm_data_t           *man_entry = NULL;
  UMI_FAST_DROP_CONFIG_REQ   *fd_config = NULL;
  mtlk_vap_handle_t           vap_handle = core->vap_handle;

  ILOG2_V("Sending UM_MAN_FAST_DROP_CONFIG_REQ");

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);

  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_FAST_DROP_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_FAST_DROP_CONFIG_REQ);

  fd_config = (UMI_FAST_DROP_CONFIG_REQ *)(man_entry->payload);
  fd_config->enableFastDrop = fast_drop;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != fd_config->Status) {
    ELOG_DDD("CID-%04x: Sending UM_MAN_FAST_DROP_CONFIG_REQ failed, res=%d status=%hhu",
              mtlk_vap_get_oid(vap_handle), res, fd_config->Status);
    if (UMI_OK != fd_config->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_receive_fast_drop (mtlk_core_t *core, uint8 *fast_drop)
{
  int                         res;
  mtlk_txmm_msg_t             man_msg;
  mtlk_txmm_data_t           *man_entry = NULL;
  UMI_FAST_DROP_CONFIG_REQ   *fd_config = NULL;
  mtlk_vap_handle_t           vap_handle = core->vap_handle;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_FAST_DROP_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_FAST_DROP_CONFIG_REQ);
  fd_config = (UMI_FAST_DROP_CONFIG_REQ *)(man_entry->payload);
  fd_config->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == fd_config->Status) {
    *fast_drop = fd_config->enableFastDrop;
  }
  else {
    ELOG_DDD("CID-%04x: Receiving UM_MAN_FAST_DROP_CONFIG_REQ failed, res=%i status=%hhu",
             mtlk_vap_get_oid(vap_handle), res, fd_config->Status);
    if (UMI_OK != fd_config->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_fast_drop (mtlk_core_t *core, uint8 fast_drop)
{
  int res = MTLK_ERR_UNKNOWN;

  if ((DISABLE_FAST_DROP != fast_drop) && (ENABLE_FAST_DROP != fast_drop)) {
    ELOG_DDD("Invalid value %d passed, must be %d or %d",
             fast_drop, DISABLE_FAST_DROP, ENABLE_FAST_DROP);
    return MTLK_ERR_PARAMS;
  }

  if ((res = mtlk_core_cfg_send_fast_drop(core, fast_drop)) == MTLK_ERR_OK)
    WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_FAST_DROP, fast_drop);

  return res;
}

int __MTLK_IFUNC
mtlk_core_coc_set_erp_mode (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_erp_mode_cfg_t *erp_cfg = NULL;
  uint32 erp_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_erp_t *erp_mgmt = __wave_core_erp_mgmt_get(core);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  erp_cfg = mtlk_clpb_enum_get_next(clpb, &erp_cfg_size);
  MTLK_CLPB_TRY(erp_cfg, erp_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(erp_cfg, erp_cfg, mtlk_coc_set_erp_mode,
                                   (erp_mgmt, &erp_cfg->erp_cfg), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_coc_get_erp_mode (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_erp_mode_cfg_t erp_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  wave_radio_t *radio;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_erp_t *erp_mgmt = __wave_core_erp_mgmt_get(core);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(!mtlk_vap_is_slave_ap(core->vap_handle));

  radio = wave_vap_radio_get(core->vap_handle);
  MTLK_ASSERT(radio);

  memset(&erp_cfg, 0, sizeof(erp_cfg));

  MTLK_CFG_SET_ITEM_BY_FUNC(&erp_cfg, erp_cfg,
    mtlk_coc_get_erp_mode, (erp_mgmt, &erp_cfg.erp_cfg), res);

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &erp_cfg, sizeof(erp_cfg));
  }

  return res;
}

static int
_wave_core_send_nfrp_cfg (mtlk_core_t *core, const UMI_NFRP_CONFIG *req)
{
    mtlk_txmm_msg_t  man_msg;
    mtlk_txmm_data_t *man_entry;
    UMI_NFRP_CONFIG  *mac_msg;
    int              res;
    unsigned         oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    ILOG1_DDD("CID-%04x: NFRP config - nfrpSupport:(%d) nfrpThreshold:(%d)", oid, req->nfrpSupport, req->nfrpThreshold);

    /* Allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* Fill the message data */
    man_entry->id = UM_MAN_NFRP_CONFIG_REQ;
    man_entry->payload_size = sizeof(*mac_msg);
    mac_msg = (UMI_NFRP_CONFIG *)man_entry->payload;
    mac_msg->nfrpSupport = req->nfrpSupport;
    mac_msg->nfrpThreshold = req->nfrpThreshold;

    mtlk_dump(1, mac_msg, sizeof(*mac_msg), "dump of UMI_NFRP_CONFIG:");

    /* Send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    /* Cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}

int __MTLK_IFUNC
wave_core_set_nfrp_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t*)hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_nfrp_cfg_t *nfrp_cfg = NULL;
    uint32 cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
    MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

    /* get new configuration and send it to FW */
    nfrp_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
    MTLK_CLPB_TRY(nfrp_cfg, cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
         MTLK_CFG_CHECK_ITEM_AND_CALL(nfrp_cfg, nfrp_cfg_params, _wave_core_send_nfrp_cfg,
                                      (core, &nfrp_cfg->nfrp_cfg_params), res);
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      return mtlk_clpb_push_res(clpb, res); /* push result into clipboard */
    MTLK_CLPB_END
}

static int
_wave_core_read_pvt_sensor (mtlk_core_t *core, iwpriv_pvt_t *pvt_params)
{
  int res;
  mtlk_txmm_msg_t    man_msg;
  mtlk_txmm_data_t  *man_entry = NULL;
  UMI_PVT           *req = NULL;
  mtlk_vap_handle_t  vap_handle = core->vap_handle;

  MTLK_ASSERT(pvt_params);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_PVT_READ_REQ;
  man_entry->payload_size = sizeof(*req);
  req = (UMI_PVT *)(man_entry->payload);

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res) {
    pvt_params->voltage     = MAC_TO_HOST32(req->voltage);
    pvt_params->temperature = MAC_TO_HOST32(req->temperature);
  } else {
    ELOG_DD("CID-%04x: Read UM_MAN_PVT_READ_REQ failure (%d)", mtlk_vap_get_oid(vap_handle), res);
    memset(pvt_params, 0, sizeof(*pvt_params));
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

int __MTLK_IFUNC
wave_core_get_pvt_sensor (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  wave_pvt_sensor_t        pvt_sensor;
  mtlk_core_t              *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  MTLK_CFG_SET_ITEM_BY_FUNC(&pvt_sensor, pvt_params,
    _wave_core_read_pvt_sensor, (core, &pvt_sensor.pvt_params), res);

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &pvt_sensor, sizeof(pvt_sensor));
  }

  return res;
}

int __MTLK_IFUNC
wave_core_get_zwdfs_antenna_enabled (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t       *core = mtlk_core_get_master((mtlk_core_t*)hcore);
  wave_radio_t      *radio = wave_vap_radio_get(core->vap_handle);
  wave_ui_mode_t    cfg;
  mtlk_clpb_t       *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&cfg, 0, sizeof(cfg));
  MTLK_CFG_SET_ITEM(&cfg, mode, wave_radio_get_zwdfs_ant_enabled(radio));

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &cfg, sizeof(cfg));
  }
  return res;
}

static int
_wave_core_set_zwdfs_antenna_enabled (mtlk_core_t *mcore, BOOL enable)
{
  wave_radio_t *zwdfs_radio = wave_vap_radio_get(mcore->vap_handle);
  mtlk_hw_t *hw = mtlk_vap_get_hw(mcore->vap_handle);
  wave_radio_descr_t *radio_descr = wave_card_radio_descr_get(hw);
  wave_radio_t *operational_radio;
  mtlk_clpb_t *clpb = NULL;
  int res = MTLK_ERR_PARAMS;

  if (wave_radio_get_zwdfs_ant_enabled(zwdfs_radio) == enable) {
    WLOG_S("zwdfs antenna is already in %s state", enable ? "enabled" : "disabled");
    return res;
  }

  if (NET_STATE_CONNECTED != mtlk_core_get_net_state(mcore)) {
    ELOG_V("Zwdfs radio is not ready");
    return MTLK_ERR_NOT_READY;
  }

  if (mtlk_core_is_in_scan_mode(mcore)) {
    ELOG_V("can't change zwdfs antenna state while radio is in scan mode");
    return MTLK_ERR_BUSY;
  }

  if (radio_descr->num_radios != CDB_NUM_OF_SEGMENTS) {
    ELOG_D("Invalid number of radios %d", radio_descr->num_radios);
    return res;
  }

  /* finish turning zwdfs antenna on/off on the master serializer of the main radio */
  operational_radio = wave_radio_descr_wave_radio_get(radio_descr, !wave_radio_id_get(zwdfs_radio));
  res = _mtlk_df_user_invoke_core(wave_radio_df_get(operational_radio),
                                  WAVE_HW_REQ_SET_ZWDFS_CARD_ANT_CONFIG, &clpb, &enable, sizeof(enable));
  res = _mtlk_df_user_process_core_retval(res, clpb, WAVE_HW_REQ_SET_ZWDFS_CARD_ANT_CONFIG, TRUE);

  return res;
}

int __MTLK_IFUNC
wave_core_set_zwdfs_antenna_enabled (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t       *core = mtlk_core_get_master((mtlk_core_t*)hcore);
  wave_radio_t      *radio = wave_vap_radio_get(core->vap_handle);
  wave_ui_mode_t    *cfg;
  uint32             cfg_size;
  mtlk_clpb_t       *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* Invoke API to trigger ZWDFS antenna activation/deactivation */
      MTLK_CFG_CHECK_ITEM_AND_CALL(cfg, mode, _wave_core_set_zwdfs_antenna_enabled,
                                   (core, !!cfg->mode), res);
      if (res == MTLK_ERR_OK) {
        /* store new config in internal DB */
        MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(cfg, mode, wave_radio_set_zwdfs_ant_enabled,
                                          (radio, !!cfg->mode));
      }
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

static int
_wave_core_set_zwdfs_card_antennas_config (mtlk_core_t *mcore, BOOL enable)
{
  mtlk_scan_support_t *ss = __wave_core_scan_support_get(mcore);
  mtlk_hw_t *hw = mtlk_vap_get_hw(mcore->vap_handle);
  BOOL paused_scan = FALSE;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(mcore == mtlk_core_get_master(mcore));

  if (is_scan_running(ss)) {
    if (!(ss->flags & SDF_BACKGROUND)) {
      /* TODO-ZWDFS: handle FG scan as well */
      ELOG_V("Cannot reconf zwdfs card in the middle of a non-BG scan");
      return MTLK_ERR_RETRY;
    } else if (!(ss->flags & SDF_BG_BREAK)) { /* background scan and not on a break at the moment */
      ILOG0_V("Background scan encountered, so pausing it first");
      if ((res = pause_or_prevent_scan(mcore)) != MTLK_ERR_OK) {
        return res;
      } else {
        finish_fw_set_chan_if_any(mcore);
        paused_scan = TRUE;
      }
    }
  }

  res = wave_hw_zwdfs_antenna_cfg_reconf_req(hw, enable);

  if (paused_scan) {
    ILOG0_V("Resuming the paused scan");
    resume_or_allow_scan(mcore);
  }

  return res;
}

int __MTLK_IFUNC
wave_core_set_zwdfs_card_antennas_config (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  BOOL *enable;
  uint32 enable_size;
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  enable = mtlk_clpb_enum_get_next(clpb, &enable_size);
  MTLK_CLPB_TRY(enable, enable_size)
    res = _wave_core_set_zwdfs_card_antennas_config(core, *enable);
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
wave_core_get_cts_to_self_to (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res                    = MTLK_ERR_OK;
  mtlk_core_t       *core    = mtlk_core_get_master((mtlk_core_t*)hcore);
  wave_radio_t      *radio   = wave_vap_radio_get(core->vap_handle);
  mtlk_cts_to_self_to_cfg_t       cts_to_self_to;
  mtlk_clpb_t       *clpb    = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&cts_to_self_to, 0, sizeof(cts_to_self_to));
  MTLK_CFG_SET_ITEM(&cts_to_self_to, cts_to, wave_radio_get_cts_to_self_to(radio));

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &cts_to_self_to, sizeof(cts_to_self_to));
  }
  return res;
}

int __MTLK_IFUNC
wave_core_set_cts_to_self_to (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res                       = MTLK_ERR_OK;
  mtlk_core_t            *core  = (mtlk_core_t*)hcore;
  wave_radio_t           *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_cts_to_self_to_cfg_t *cts_to_self_to = NULL;
  uint32                 cts_size;
  mtlk_clpb_t            *clpb  = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  /* get value */
  cts_to_self_to = mtlk_clpb_enum_get_next(clpb, &cts_size);
  MTLK_CLPB_TRY(cts_to_self_to, cts_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* store new config in internal db*/
      MTLK_CFG_CHECK_ITEM_AND_CALL(cts_to_self_to, cts_to, wave_radio_set_cts_to_self_to,
                                   (radio, cts_to_self_to->cts_to), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC mtlk_core_cfg_set_ssid (mtlk_core_t *core, const u8 *ssid, u8 ssid_len)
{
  struct wireless_dev *wdev = mtlk_df_user_get_wdev(mtlk_df_get_user(mtlk_vap_get_df(core->vap_handle)));
  mtlk_gen_core_cfg_t core_cfg;
  int res;

  MTLK_ASSERT(NULL != wdev);

  if (ssid_len == 0)
    return MTLK_ERR_OK;
  if (ssid_len > IEEE80211_MAX_SSID_LEN || ssid_len >= MTLK_ESSID_MAX_SIZE)
    return MTLK_ERR_PARAMS;

  wave_strncopy(core_cfg.essid, ssid, sizeof(core_cfg.essid), ssid_len);

  res = mtlk_core_set_essid_by_cfg(core, &core_cfg);
  if (res != MTLK_ERR_OK)
    return res;

  /* wdev_lock not needed, lock taken by nl80211_set_beacon */
  wave_memcpy(wdev->ssid, sizeof(wdev->ssid), ssid, ssid_len);
  wdev->ssid_len = ssid_len;
  return MTLK_ERR_OK;
}

int __MTLK_IFUNC mtlk_core_get_tpc_cfg (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_tpc_cfg_t tpc_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&tpc_cfg, 0, sizeof(tpc_cfg));

  MTLK_CFG_SET_ITEM(&tpc_cfg, loop_type,
                    WAVE_RADIO_PDB_GET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_TPC_LOOP_TYPE));

  return mtlk_clpb_push_res_data(clpb, res, &tpc_cfg, sizeof(tpc_cfg));
}

static int
_wave_core_cfg_get_cutoff_point_req(mtlk_core_t *core, uint32 *cutoff_point)
{
  int                       res = MTLK_ERR_OK;
  mtlk_txmm_msg_t           man_msg;
  mtlk_txmm_data_t          *man_entry = NULL;
  UMI_PROTECTION_RATE_CONFIG *req = NULL;
  mtlk_vap_handle_t         vap_handle = core->vap_handle;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg,
    mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_RTS_RATE_SET_REQ;
  man_entry->payload_size = sizeof(UMI_PROTECTION_RATE_CONFIG);

  req = (UMI_PROTECTION_RATE_CONFIG *)(man_entry->payload);
  req->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == req->Status)
    *cutoff_point = req->cutoffPoint;
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_RTS_RATE_SET_REQ failed, res=%i status=%hhu",
      mtlk_vap_get_oid(vap_handle), res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

static void
tpc_config_default_g6(tpcConfig_t *tpcConfig)
{
  tpcConfig->calType = 1;   /* TPC_CAL_TYPE_LOGARITHMIC */
  tpcConfig->fixedGain = NO_FIXED_GAIN;
}

static int
_mtlk_set_tpc_config (struct nic *nic, const uint8 loop_type)
{
  mtlk_txmm_data_t          *man_entry = NULL;
  mtlk_txmm_msg_t           man_msg;
  int                       result = MTLK_ERR_OK;
  int                       reserved;
  tpcConfig_t               *tpcConfig = NULL;
  mtlk_card_type_t          card_type = MTLK_CARD_UNKNOWN;
  psdb_pw_limits_t          pwl;
  mtlk_pdb_size_t           pwl_size = sizeof(psdb_pw_limits_t);
  int                       i;
  wave_radio_t              *radio = wave_vap_radio_get(nic->vap_handle);
  mtlk_hw_t                 *hw    = mtlk_vap_get_hw(nic->vap_handle);
  uint16                    chip_id = hw_mmb_get_chip_id(hw);

  ILOG1_D("SET_TPC_CONFIG_REQ for band = %d", wave_radio_band_get(radio));
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(nic->vap_handle), &result);
  if (man_entry == NULL) {
    ELOG_D("CID-%04x: Can't send UM_MAN_SET_TPC_CONFIG_REQ to MAC due to the lack of MAN_MSG", mtlk_vap_get_oid(nic->vap_handle));
    goto FINISH;
  }

  man_entry->id           = UM_MAN_SET_TPC_CONFIG_REQ;
  man_entry->payload_size = sizeof(*tpcConfig);
  tpcConfig = man_entry->payload;
  memset(tpcConfig, 0, sizeof(*tpcConfig));

  mtlk_hw_get_prop(mtlk_vap_get_hwapi(nic->vap_handle), MTLK_HW_PROP_CARD_TYPE, &card_type, sizeof(&card_type));

  CARD_SELECTOR_START(card_type);
  IF_CARD_G6          ( tpc_config_default_g6(tpcConfig) );
  IF_CARD_G7          ( tpc_config_default_g6(tpcConfig) ); /* the same as on g6 */
  CARD_SELECTOR_END();

  tpcConfig->tpcLoopType = loop_type;

  if (_chipid_is_gen6_d2_or_gen7(chip_id))
  {
    reserved =  WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_TPC_RESERVED);
    if (reserved)
    {
      tpcConfig->Reserved[0] = 1;
    } else {
      tpcConfig->Reserved[0] = 0;
    }
  }
  result = WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_TPC_PW_LIMITS_CFG, &pwl, &pwl_size);
  if (result != MTLK_ERR_OK) {
    ELOG_D("CID-%04x: Can't send UM_MAN_SET_TPC_CONFIG_REQ request due to PDB read error", mtlk_vap_get_oid(nic->vap_handle));
    goto FINISH;
  }
  MTLK_ASSERT(sizeof(pwl)==pwl_size);

  tpcConfig->powerLimit11b = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_11B]);
  tpcConfig->regulationLimitAG = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_AG_20]);

  for (i = 0; i < MAXIMUM_BANDWIDTHS_GEN6; i++) {
    tpcConfig->regulationLimit[i] = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_OFDM_20 + i]);
    tpcConfig->regulationLimitMU[i] = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_MU_20 + i]);
    tpcConfig->regulationLimitBF[i] = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_BF_20 + i]);
    tpcConfig->regulationLimitBF_AX[i] = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_BF_AX_20 + i]);
    tpcConfig->regulationLimitAX[i] = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_AX_20 + i]);
    if (i < MAXIMUM_BANDWIDTHS_11N)
        tpcConfig->regulationLimitN[i] = HOST_TO_MAC16(pwl.pw_limits[PSDB_PHY_CW_N_20 + i]);
  }

  mtlk_dump(2, tpcConfig, sizeof(*tpcConfig), "dump of tpcConfig");
  SLOG1(0, 0, tpcConfig_t, tpcConfig);
  result = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (result != MTLK_ERR_OK || UMI_OK != tpcConfig->Status) {
    ELOG_DDD("CID-%04x: Set UM_MAN_SET_TPC_CONFIG_REQ failed, result=%d status=%hhu",
            mtlk_vap_get_oid(nic->vap_handle), result, tpcConfig->Status);
    if (UMI_OK != tpcConfig->Status)
      result = MTLK_ERR_MAC;
    goto FINISH;
  }

FINISH:
  if (man_entry) {
    mtlk_txmm_msg_cleanup(&man_msg);
  }

  return result;
}

static int
_mtlk_core_cfg_send_and_store_tpc_config (mtlk_core_t *core,
        const uint8 loop_type)
{
  /* send new config to FW */
  int res = _mtlk_set_tpc_config(core, loop_type);

  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle),
              PARAM_DB_RADIO_TPC_LOOP_TYPE, loop_type);
  }
  return res;
}

int __MTLK_IFUNC mtlk_core_set_tpc_cfg (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_tpc_cfg_t *tpc_cfg = NULL;
  uint32          tpc_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  tpc_cfg = mtlk_clpb_enum_get_next(clpb, &tpc_cfg_size);
  MTLK_CLPB_TRY(tpc_cfg, tpc_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(tpc_cfg, loop_type, _mtlk_core_cfg_send_and_store_tpc_config,
                                   (core, tpc_cfg->loop_type), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC wave_core_cfg_get_rts_rate (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_wlan_rts_rate_cfg_t rts_rate;
  mtlk_core_t              *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  MTLK_CFG_SET_ITEM_BY_FUNC(&rts_rate, cutoff_point, _wave_core_cfg_get_cutoff_point_req,
                           (core, &rts_rate.cutoff_point), res);

  /* push result and config to clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &rts_rate, sizeof(rts_rate));
}

static int
_wave_core_cfg_set_cutoff_point_req (mtlk_core_t *core, uint32 cutoff_point)
{
#define MIN_RTS_RATE_CUTOFF 0
#define MAX_RTS_RATE_CUTOFF 2
  int res;
  mtlk_txmm_msg_t           man_msg;
  mtlk_txmm_data_t          *man_entry = NULL;
  UMI_PROTECTION_RATE_CONFIG *req = NULL;
  mtlk_vap_handle_t         vap_handle = core->vap_handle;

  if (cutoff_point > MAX_RTS_RATE_CUTOFF) {
    ELOG_DDD("Invalid cutoff_point: %d. Allowed values: %d..%d", cutoff_point,
      MIN_RTS_RATE_CUTOFF, MAX_RTS_RATE_CUTOFF);
    return MTLK_ERR_PARAMS;
  }
  MTLK_ASSERT(vap_handle);

  ILOG1_DD("CID-%04x:RTS protection rate FW request: Set %d",
    mtlk_vap_get_oid(vap_handle), cutoff_point);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_RTS_RATE_SET_REQ;
  man_entry->payload_size = sizeof(UMI_PROTECTION_RATE_CONFIG);

  req = (UMI_PROTECTION_RATE_CONFIG *)(man_entry->payload);
  req->getSetOperation = API_SET_OPERATION;
  req->cutoffPoint = (uint8)cutoff_point;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != req->Status) {
    ELOG_DDD("CID-%04x: Set RTS protection rate failure, res=%i status=%hhu",
          mtlk_vap_get_oid(vap_handle), res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
#undef MIN_RTS_RATE_CUTOFF
#undef MAX_RTS_RATE_CUTOFF
}

inline static void __wave_core_cfg_store_cutoff_point (mtlk_core_t *core,
  const uint32 cutoff_point)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  MTLK_ASSERT(radio != NULL);

  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_RTS_CUTOFF_POINT, cutoff_point);
}

int __MTLK_IFUNC wave_core_cfg_set_rts_rate (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                    res = MTLK_ERR_OK;
  mtlk_core_t            *core = (mtlk_core_t*)hcore;
  mtlk_wlan_rts_rate_cfg_t   *rts_rate = NULL;
  uint32                 rts_rate_size;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **)data;
  wave_radio_t           *radio = wave_vap_radio_get(core->vap_handle);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  if (wave_radio_sta_cnt_get(radio) > 0) {
    WLOG_V("Setting RTS rate is disabled while connected STA or peer AP exists");
    return MTLK_ERR_NOT_SUPPORTED;
  }
  /* get configuration */
  rts_rate = mtlk_clpb_enum_get_next(clpb, &rts_rate_size);
  MTLK_CLPB_TRY(rts_rate, rts_rate_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* send new config to FW */
      MTLK_CFG_CHECK_ITEM_AND_CALL(rts_rate, cutoff_point, _wave_core_cfg_set_cutoff_point_req,
                                  (core, rts_rate->cutoff_point), res);
      /* store new config in internal db*/
      MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(rts_rate, cutoff_point, __wave_core_cfg_store_cutoff_point,
                                        (core, rts_rate->cutoff_point));
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

static int
_wave_core_cfg_get_stations_stats_enabled (uint32 *enabled, wave_radio_t *radio)
{
  *enabled = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_STATIONS_STATS_ENABLED);
  return MTLK_ERR_OK;
}

int __MTLK_IFUNC wave_core_cfg_get_stations_stats_enabled (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_wlan_stations_stats_enabled_cfg_t stations_stats_enabled;
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;
  mtlk_core_t              *core = (mtlk_core_t*)hcore;
  wave_radio_t             *radio = wave_vap_radio_get(core->vap_handle);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  MTLK_CFG_SET_ITEM_BY_FUNC(&stations_stats_enabled, enabled, _wave_core_cfg_get_stations_stats_enabled,
                           (&stations_stats_enabled.enabled, radio), res);

  /* push result and config to clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &stations_stats_enabled, sizeof(stations_stats_enabled));
}



static void
_wave_core_cfg_set_per_rate_stat_hash (mtlk_core_t *core, uint32 current_state, uint32 new_state)
{
  sta_entry                    *sta;
  mtlk_stadb_iterator_t         iter;
  sta_db *stadb;
  mtlk_vap_manager_t    *vap_mgr = mtlk_vap_get_manager(core->vap_handle);
  uint32                i, max_vaps_count;
  mtlk_vap_handle_t     vap_handle;
  int                   res;
  if (current_state == new_state)
    return;
  if (current_state) { /* Turning statistics off */
    max_vaps_count = mtlk_vap_manager_get_max_vaps_count(vap_mgr);
    for( i= 0; i < max_vaps_count; i++) {
      /* find vap, referenced by vap index */
      res = mtlk_vap_manager_get_vap_handle(vap_mgr, i, &vap_handle);
      if (MTLK_ERR_OK != res) {
        continue;
      }
      core = mtlk_vap_get_core(vap_handle);
      stadb = &core->slow_ctx->stadb;

      sta = (sta_entry*)mtlk_stadb_iterate_first(stadb, &iter);
      if (sta) {
        do {
          mtlk_osal_lock_acquire(&sta->lock);
          mtlk_sta_rate_stat_clean_hash(sta);
          mtlk_sta_rate_stat_hash_init(sta);
          mtlk_osal_lock_release(&sta->lock);
          sta = (sta_entry*)mtlk_stadb_iterate_next(&iter);
        } while (sta);
        mtlk_stadb_iterate_done(&iter);
      }
    } /* for */
  }
}

int __MTLK_IFUNC wave_core_cfg_set_stations_stats_enabled (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                    res = MTLK_ERR_OK;
  mtlk_core_t            *core = (mtlk_core_t*)hcore;
  mtlk_wlan_stations_stats_enabled_cfg_t   *stations_stats_enabled = NULL;
  uint32                 stations_stats_enabled_size;
  mtlk_clpb_t            *clpb = *(mtlk_clpb_t **)data;
  wave_radio_t           *radio = wave_vap_radio_get(core->vap_handle);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  /* get configuration */
  stations_stats_enabled = mtlk_clpb_enum_get_next(clpb, &stations_stats_enabled_size);
  MTLK_CLPB_TRY(stations_stats_enabled, stations_stats_enabled_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(stations_stats_enabled, enabled, _wave_core_cfg_set_per_rate_stat_hash,
        (core, WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_STATIONS_STATS_ENABLED), stations_stats_enabled->enabled));
      MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(stations_stats_enabled, enabled, WAVE_RADIO_PDB_SET_INT,
        (radio, PARAM_DB_RADIO_STATIONS_STATS_ENABLED, stations_stats_enabled->enabled));
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_reload_tpc_wrapper (struct nic *nic, uint32 channel, psdb_pw_limits_t *pwl)
{
  int res = MTLK_ERR_PARAMS;
  uint8 loop_type;
  mtlk_pdb_size_t pw_limits_size = sizeof(psdb_pw_limits_t);
  wave_radio_t *radio;
  MTLK_ASSERT(nic);
  MTLK_ASSERT(pwl);

  radio = wave_vap_radio_get(nic->vap_handle);

  res = WAVE_RADIO_PDB_SET_BINARY(radio, PARAM_DB_RADIO_TPC_PW_LIMITS_CFG, pwl->pw_limits, pw_limits_size);
  if(res != MTLK_ERR_OK)
  {
    ELOG_D("CID-%04x: Can't update power limits in PDB", mtlk_vap_get_oid(nic->vap_handle));
    goto END;
  }

  loop_type =  WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_TPC_LOOP_TYPE);
  res = _mtlk_set_tpc_config(nic, loop_type);
  if(res != MTLK_ERR_OK)
  {
    goto END;
  }

  res = mtlk_reload_tpc(
      (mtlk_hw_band_e)WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_FREQ_BAND_CUR),
      WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_PROG_MODEL_SPECTRUM_MODE),
      WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_BONDING_MODE),
      channel,
      mtlk_vap_get_txmm(nic->vap_handle),
      nic->txmm_async_eeprom_msgs,
      ARRAY_SIZE(nic->txmm_async_eeprom_msgs),
      mtlk_core_get_eeprom(nic),
      mtlk_eeprom_get_num_antennas(mtlk_core_get_eeprom(nic)));

  if(res != MTLK_ERR_OK)
  {
    goto END;
  }

  res = MTLK_ERR_OK;

END:

  return res;
}

int __MTLK_IFUNC wave_core_cfg_recover_cutoff_point (mtlk_core_t *core)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  uint32 cutoff_point  = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_RTS_CUTOFF_POINT);

  if (MTLK_PARAM_DB_VALUE_IS_INVALID(cutoff_point))
      return MTLK_ERR_OK;

  return _wave_core_cfg_set_cutoff_point_req(core, cutoff_point);
}

static __INLINE unsigned int
wave_band_to_nof_channels (mtlk_hw_band_e band)
{
  if ((MTLK_HW_BAND_2_4_GHZ == band)) {
    return NUM_2GHZ_CHANS;
  }
  else if ((MTLK_HW_BAND_5_2_GHZ == band)) {
    return NUM_5GHZ_CHANS;
  }
  else if ((MTLK_HW_BAND_6_GHZ == band)) {
    return NUM_TOTAL_CHANS_6G;
  }
  else {
    ELOG_V("Invalid band");
    return 0;
  }
}

unsigned int __MTLK_IFUNC
wave_core_cfg_get_nof_channels (mtlk_hw_band_e  hw_band)
{
  return wave_band_to_nof_channels(hw_band);
}

/* Dynamic MU Configuration */

static int _core_cfg_receive_dynamic_mu_type (mtlk_core_t *core, UMI_DYNAMIC_MU_TYPE *dynamic_mu_type)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_DYNAMIC_MU_TYPE *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Receive DYNAMIC MU TYPE", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_SET_DYNAMIC_MU_TYPE_REQ;
  man_entry->payload_size = sizeof(UMI_DYNAMIC_MU_TYPE);
  mac_msg = (UMI_DYNAMIC_MU_TYPE *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res) {
    dynamic_mu_type->dlMuType = mac_msg->dlMuType;
    dynamic_mu_type->ulMuType = mac_msg->ulMuType;
    dynamic_mu_type->minStationsInGroup = mac_msg->minStationsInGroup;
    dynamic_mu_type->maxStationsInGroup = mac_msg->maxStationsInGroup;
    dynamic_mu_type->cdbConfig = mac_msg->cdbConfig;
  }
  else {
    ELOG_DD("CID-%04x: Receive DYNAMIC MU TYPE failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC wave_core_cfg_send_dynamic_mu_type (mtlk_core_t *core, const UMI_DYNAMIC_MU_TYPE *dynamic_mu_type)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_DYNAMIC_MU_TYPE *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Send DYNAMIC MU TYPE", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_SET_DYNAMIC_MU_TYPE_REQ;
  man_entry->payload_size = sizeof(UMI_DYNAMIC_MU_TYPE);
  mac_msg = (UMI_DYNAMIC_MU_TYPE *)man_entry->payload;
  memset(mac_msg, 0, sizeof(*mac_msg));

  mac_msg->dlMuType = dynamic_mu_type->dlMuType;
  mac_msg->ulMuType = dynamic_mu_type->ulMuType;
  mac_msg->minStationsInGroup = dynamic_mu_type->minStationsInGroup;
  mac_msg->maxStationsInGroup = dynamic_mu_type->maxStationsInGroup;
  mac_msg->cdbConfig = dynamic_mu_type->cdbConfig;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Send DYNAMIC MU TYPE failed (%i)", oid, res);
  }
  else if (UMI_OK != mac_msg->Status){
    ELOG_DD("CID-%04x: %i. wrong input parameters or mu operation disabled", oid, mac_msg->Status);
    res = MTLK_ERR_PARAMS;
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int _core_cfg_receive_he_mu_fixed_parameters (mtlk_core_t *core,
                                                     UMI_HE_MU_FIXED_PARAMTERS *he_mu_fixed_parameters)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_HE_MU_FIXED_PARAMTERS *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Receive HE MU FIXED PARAMETERS", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UMI_MAN_SET_HE_MU_FIXED_PARAMETERS;
  man_entry->payload_size = sizeof(UMI_HE_MU_FIXED_PARAMTERS);
  mac_msg = (UMI_HE_MU_FIXED_PARAMTERS *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res) {
    he_mu_fixed_parameters->muSequence = mac_msg->muSequence;
    he_mu_fixed_parameters->ltf_GI = mac_msg->ltf_GI;
    he_mu_fixed_parameters->codingType = mac_msg->codingType;
    he_mu_fixed_parameters->heRate = mac_msg->heRate;
  }
  else {
    ELOG_DD("CID-%04x: Receive HE MU FIXED PARAMETERS failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC wave_core_cfg_send_he_mu_fixed_parameters (mtlk_core_t *core,
                                  const UMI_HE_MU_FIXED_PARAMTERS *he_mu_fixed_parameters)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_HE_MU_FIXED_PARAMTERS *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Send HE MU FIXED PARAMETERS", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UMI_MAN_SET_HE_MU_FIXED_PARAMETERS;
  man_entry->payload_size = sizeof(UMI_HE_MU_FIXED_PARAMTERS);
  mac_msg = (UMI_HE_MU_FIXED_PARAMTERS *)man_entry->payload;
  memset(mac_msg, 0, sizeof(*mac_msg));

  mac_msg->muSequence = he_mu_fixed_parameters->muSequence;
  mac_msg->ltf_GI = he_mu_fixed_parameters->ltf_GI;
  mac_msg->codingType = he_mu_fixed_parameters->codingType;
  mac_msg->heRate = he_mu_fixed_parameters->heRate;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Send HE MU FIXED PARAMETERS failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int _core_cfg_receive_he_mu_duration (mtlk_core_t *core, UMI_HE_MU_DURATION *he_mu_duration)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_HE_MU_DURATION *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Receive HE MU DURATION", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UMI_MAN_SET_HE_MU_DURATION;
  man_entry->payload_size = sizeof(UMI_HE_MU_DURATION);
  mac_msg = (UMI_HE_MU_DURATION *)man_entry->payload;
  mac_msg->getSetOperation = API_GET_OPERATION;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res) {
    he_mu_duration->PpduDuration = MAC_TO_HOST16(mac_msg->PpduDuration);
    he_mu_duration->TxopDuration = MAC_TO_HOST16(mac_msg->TxopDuration);
    he_mu_duration->TfLength = MAC_TO_HOST16(mac_msg->TfLength);
    he_mu_duration->NumberOfRepetitions = mac_msg->NumberOfRepetitions;
  }
  else {
    ELOG_DD("CID-%04x: Receive HE MU DURATION failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC wave_core_cfg_send_he_mu_duration (mtlk_core_t *core, const UMI_HE_MU_DURATION *he_mu_duration)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_HE_MU_DURATION *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Send HE MU DURATION", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UMI_MAN_SET_HE_MU_DURATION;
  man_entry->payload_size = sizeof(UMI_HE_MU_DURATION);
  mac_msg = (UMI_HE_MU_DURATION *)man_entry->payload;
  memset(mac_msg, 0, sizeof(*mac_msg));

  mac_msg->PpduDuration = HOST_TO_MAC16(he_mu_duration->PpduDuration);
  mac_msg->TxopDuration = HOST_TO_MAC16(he_mu_duration->TxopDuration);
  mac_msg->TfLength = HOST_TO_MAC16(he_mu_duration->TfLength);
  mac_msg->NumberOfRepetitions = he_mu_duration->NumberOfRepetitions;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Send HE MU DURATION failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int _wave_core_cfg_send_etsi_ppdu_duration_limits(mtlk_core_t *core,
             const UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS *etsi_ppdu_duration_limits)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Sending UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UMI_MAN_SET_ETSI_PPDU_DURATION_LIMITS;
  man_entry->payload_size = sizeof(UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS);
  mac_msg = (UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS *)man_entry->payload;

  mac_msg->setEtsiPpduDuration = etsi_ppdu_duration_limits->setEtsiPpduDuration;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Sending UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

void __MTLK_IFUNC wave_core_cfg_get_dynamic_mu_type (mtlk_core_t *core,
                                                     UMI_DYNAMIC_MU_TYPE *dynamic_mu_type)
{
  mtlk_pdb_size_t data_size = sizeof(*dynamic_mu_type);
  WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                            PARAM_DB_RADIO_DYNAMIC_MU_TYPE, dynamic_mu_type, &data_size);
}

void __MTLK_IFUNC wave_core_cfg_get_he_mu_fixed_parameters (mtlk_core_t *core,
                                                            UMI_HE_MU_FIXED_PARAMTERS *he_mu_fixed_parameters)
{
  mtlk_pdb_size_t data_size = sizeof(*he_mu_fixed_parameters);
  WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                            PARAM_DB_RADIO_HE_MU_FIXED_PARAMTERS, he_mu_fixed_parameters, &data_size);
}

void __MTLK_IFUNC wave_core_cfg_get_he_mu_duration (mtlk_core_t *core,
                                                    UMI_HE_MU_DURATION *he_mu_duration)
{
  mtlk_pdb_size_t data_size = sizeof(*he_mu_duration);
  WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                            PARAM_DB_RADIO_HE_MU_DURATION, he_mu_duration, &data_size);
}

static int _core_cfg_send_and_store_dynamic_mu_type (mtlk_core_t *core,
                                                     const UMI_DYNAMIC_MU_TYPE *dynamic_mu_type)
{
  /* send new config to FW */
  int res = wave_core_cfg_send_dynamic_mu_type(core, dynamic_mu_type);

  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                    PARAM_DB_RADIO_DYNAMIC_MU_TYPE,
                                    dynamic_mu_type, sizeof(*dynamic_mu_type));
    if (MTLK_ERR_OK != res) {
      ELOG_DD("CID-%04x: Store DYNAMIC MU TYPE failed (%i)",
              mtlk_vap_get_oid(core->vap_handle), res);
    }
  }
  return res;
}

static int _core_cfg_send_and_store_he_mu_fixed_parameters (mtlk_core_t *core,
                                  const UMI_HE_MU_FIXED_PARAMTERS *he_mu_fixed_parameters)
{
  /* send new config to FW */
  int res = wave_core_cfg_send_he_mu_fixed_parameters(core, he_mu_fixed_parameters);

  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                    PARAM_DB_RADIO_HE_MU_FIXED_PARAMTERS,
                                    he_mu_fixed_parameters, sizeof(*he_mu_fixed_parameters));
    if (MTLK_ERR_OK != res) {
      ELOG_DD("CID-%04x: Store HE MU FIXED PARAMETERS failed (%i)",
              mtlk_vap_get_oid(core->vap_handle), res);
    }
  }
  return res;
}

static int _core_cfg_send_and_store_he_mu_duration (mtlk_core_t *core,
                                                    const UMI_HE_MU_DURATION *he_mu_duration)
{
  /* send new config to FW */
  int res = wave_core_cfg_send_he_mu_duration(core, he_mu_duration);

  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                    PARAM_DB_RADIO_HE_MU_DURATION,
                                    he_mu_duration, sizeof(*he_mu_duration));
    if (MTLK_ERR_OK != res) {
      ELOG_DD("CID-%04x: Store HE MU DURATION failed (%i)",
              mtlk_vap_get_oid(core->vap_handle), res);
    }
  }
  return res;
}

static int _core_cfg_send_and_store_etsi_ppdu_duration_limits (mtlk_core_t *core,
                        const UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS  *etsi_ppdu_duration_limits)
{
  /* send new config to FW */
  int res = _wave_core_cfg_send_etsi_ppdu_duration_limits(core, etsi_ppdu_duration_limits);

  if (MTLK_ERR_OK == res) {
    /* store new config into internal DB */
    res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                    PARAM_DB_RADIO_ETSI_PPDU_DURATION_LIMITS,
                                    etsi_ppdu_duration_limits, sizeof(*etsi_ppdu_duration_limits));
    if (MTLK_ERR_OK != res) {
      ELOG_DD("CID-%04x: Storage of PARAM_DB_RADIO_ETSI_PPDU_DURATION_LIMITS failed (%i)",
              mtlk_vap_get_oid(core->vap_handle), res);
    }
  }
  return res;
}

int __MTLK_IFUNC wave_core_cfg_set_dynamic_mu_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  uint32 dynamic_mu_cfg_size;
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  wave_dynamic_mu_cfg_t *dinamic_mu_cfg = NULL;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  dinamic_mu_cfg = mtlk_clpb_enum_get_next(clpb, &dynamic_mu_cfg_size);
  MTLK_CLPB_TRY(dinamic_mu_cfg, dynamic_mu_cfg_size)
    /* send configuration */
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(dinamic_mu_cfg, dynamic_mu_type_params, _core_cfg_send_and_store_dynamic_mu_type,
                                   (core, &dinamic_mu_cfg->dynamic_mu_type_params), res);
      MTLK_CFG_CHECK_ITEM_AND_CALL(dinamic_mu_cfg, he_mu_fixed_params, _core_cfg_send_and_store_he_mu_fixed_parameters,
                                   (core, &dinamic_mu_cfg->he_mu_fixed_params), res);
      MTLK_CFG_CHECK_ITEM_AND_CALL(dinamic_mu_cfg, he_mu_duration_params, _core_cfg_send_and_store_he_mu_duration,
                                   (core, &dinamic_mu_cfg->he_mu_duration_params), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC wave_core_cfg_get_dynamic_mu_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  wave_dynamic_mu_cfg_t dinamic_mu_cfg;
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&dinamic_mu_cfg, 0, sizeof(dinamic_mu_cfg));

  MTLK_CFG_SET_ITEM_BY_FUNC(&dinamic_mu_cfg, dynamic_mu_type_params,
                            _core_cfg_receive_dynamic_mu_type, (core, &dinamic_mu_cfg.dynamic_mu_type_params), res);
  MTLK_CFG_SET_ITEM_BY_FUNC(&dinamic_mu_cfg, he_mu_fixed_params,
                            _core_cfg_receive_he_mu_fixed_parameters, (core, &dinamic_mu_cfg.he_mu_fixed_params), res);
  MTLK_CFG_SET_ITEM_BY_FUNC(&dinamic_mu_cfg, he_mu_duration_params,
                            _core_cfg_receive_he_mu_duration, (core, &dinamic_mu_cfg.he_mu_duration_params), res);

  /* push result and data into clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &dinamic_mu_cfg, sizeof(dinamic_mu_cfg));
}

int __MTLK_IFUNC wave_core_cfg_set_etsi_ppdu_duration_limits (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  uint32 etsi_ppdu_duration_size;
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  wave_etsi_ppdu_duration_cfg_t *etsi_ppdu_duration = NULL;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  etsi_ppdu_duration = mtlk_clpb_enum_get_next(clpb, &etsi_ppdu_duration_size);
  MTLK_CLPB_TRY(etsi_ppdu_duration, etsi_ppdu_duration_size)
    /* send configuration */
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
    MTLK_CFG_CHECK_ITEM_AND_CALL(etsi_ppdu_duration,
                                 umi_set_etsi_ppdu_duration_limits_req_params,
                                 _core_cfg_send_and_store_etsi_ppdu_duration_limits,
                                (core, &etsi_ppdu_duration->umi_set_etsi_ppdu_duration_limits_req_params), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

static void __MTLK_IFUNC
_core_cfg_get_etsi_ppdu_limits_pdb (mtlk_core_t *core, UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS *etsi_ppdu_limits)
{
  mtlk_pdb_size_t data_size = sizeof(*etsi_ppdu_limits);
  if(MTLK_ERR_OK != WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                            PARAM_DB_RADIO_ETSI_PPDU_DURATION_LIMITS, etsi_ppdu_limits, &data_size))
      ELOG_D("CID-%04x: Can't read ETSI_PPDU_LIMITS from PDB", mtlk_vap_get_oid(core->vap_handle));
}

int __MTLK_IFUNC
wave_core_cfg_get_etsi_ppdu_duration_limits (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  wave_etsi_ppdu_duration_cfg_t etsi_ppdu_duration;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&etsi_ppdu_duration, 0, sizeof(etsi_ppdu_duration));
  MTLK_CFG_SET_ITEM_BY_FUNC_VOID(&etsi_ppdu_duration, umi_set_etsi_ppdu_duration_limits_req_params, _core_cfg_get_etsi_ppdu_limits_pdb,
                                 (core, &etsi_ppdu_duration.umi_set_etsi_ppdu_duration_limits_req_params));

  /* push result and data into clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &etsi_ppdu_duration, sizeof(etsi_ppdu_duration));
}

static int _core_cfg_receive_twt_parameters (mtlk_core_t *core, twt_params_int_t *twt_params, const unsigned int staId)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_GET_TWT_PARAMS *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Receive TWT PARAMS", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_GET_TWT_PARAMS_REQ;
  man_entry->payload_size = sizeof(UMI_GET_TWT_PARAMS);
  mac_msg = (UMI_GET_TWT_PARAMS *)man_entry->payload;
  mac_msg->staId = HOST_TO_MAC32(staId);

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res) {
    twt_params->individualWakeTimeHigh = MAC_TO_HOST64(mac_msg->individualWakeTime) >> 32;
    twt_params->individualWakeTimeLow = MAC_TO_HOST64(mac_msg->individualWakeTime) & 0xFFFFFFFF;
    twt_params->individualWakeInterval = MAC_TO_HOST32(mac_msg->individualWakeInterval);
    twt_params->individualMinWakeDuration = MAC_TO_HOST16(mac_msg->individualMinWakeDuration);
    twt_params->isImplicit = mac_msg->isImplicit;
    twt_params->isAnnounced = mac_msg->isAnnounced;
    twt_params->isTriggerEnabled = mac_msg->isTriggerEnabled;
    twt_params->agreementType = mac_msg->agreementType;
    twt_params->individualChannel = mac_msg->individualChannel;
    twt_params->BroadcastTargetBeacon = mac_msg->BroadcastTargetBeacon;
    twt_params->BroadcastListenInterval = mac_msg->BroadcastListenInterval;
  }
  else {
    ELOG_DD("CID-%04x: Receive TWT PARAMS failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC wave_core_cfg_get_twt_params (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  int res = MTLK_ERR_OK;
  IEEE_ADDR *addr;
  uint32 addr_size;
  sta_entry *sta = NULL;
  twt_params_int_t twt_params = {0};

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  addr = mtlk_clpb_enum_get_next(clpb, &addr_size);

  MTLK_CLPB_TRY(addr, addr_size)
    /* find station in stadb */
    sta = mtlk_stadb_find_sta(&core->slow_ctx->stadb, addr->au8Addr);
    if (NULL == sta) {
      WLOG_DY("CID-%04x: station %Y not found",
        mtlk_vap_get_oid(core->vap_handle), addr);
      MTLK_CLPB_EXIT(MTLK_ERR_UNKNOWN);
    }

    _core_cfg_receive_twt_parameters(core, &twt_params, sta->info.sid);
    mtlk_sta_decref(sta);
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res_data(clpb, res, &twt_params, sizeof(twt_params));
  MTLK_CLPB_END;
}


static int _core_cfg_receive_ax_default_params (mtlk_core_t *core, ax_default_params_t *ax_params)
{
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_GET_TWT_PARAMS *mac_msg;
  int res;
  uint16 oid;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));

  oid = mtlk_vap_get_oid(core->vap_handle);
  ILOG2_D("CID-%04x: Receive ax default params", oid);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UMI_MAN_GET_AX_DEFAULT_PARAMS;
  man_entry->payload_size = sizeof(UMI_GET_TWT_PARAMS);
  mac_msg = (UMI_GET_TWT_PARAMS *)man_entry->payload;

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res) {
    ax_params->twt_params.individualWakeTimeHigh = MAC_TO_HOST64(mac_msg->individualWakeTime) >> 32;
    ax_params->twt_params.individualWakeTimeLow = MAC_TO_HOST64(mac_msg->individualWakeTime) & 0XFFFFFFFF;
    ax_params->twt_params.individualWakeInterval = MAC_TO_HOST32(mac_msg->individualWakeInterval);
    ax_params->twt_params.individualMinWakeDuration = MAC_TO_HOST16(mac_msg->individualMinWakeDuration);
    ax_params->twt_params.isImplicit = mac_msg->isImplicit;
    ax_params->twt_params.isAnnounced = mac_msg->isAnnounced;
    ax_params->twt_params.isTriggerEnabled = mac_msg->isTriggerEnabled;
    ax_params->twt_params.agreementType = mac_msg->agreementType;
    ax_params->twt_params.individualChannel = mac_msg->individualChannel;
    ax_params->twt_params.BroadcastTargetBeacon = mac_msg->BroadcastTargetBeacon;
    ax_params->twt_params.BroadcastListenInterval = mac_msg->BroadcastListenInterval;
  }
  else {
    ELOG_DD("CID-%04x: Receive ax default parameters failed (%i)", oid, res);
  }

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC wave_core_cfg_get_ax_defaults (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  ax_default_params_t ax_params;
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t *) hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(core == mtlk_core_get_master(core));
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  MTLK_CFG_SET_ITEM_BY_FUNC(&ax_params, twt_params,
                            _core_cfg_receive_ax_default_params, (core, &ax_params), res);

  /* push result and data into clipboard */
  return mtlk_clpb_push_res_data(clpb, res, &ax_params, sizeof(ax_params));
  
}

static int
_mtlk_core_receive_amsdu_num (mtlk_core_t *core, uint32 *htMsduInAmsdu,
  uint32 *vhtMsduInAmsdu, uint32 *heMsduInAmsdu)
{
  int res;
  mtlk_txmm_msg_t           man_msg;
  mtlk_txmm_data_t          *man_entry = NULL;
  UMI_MSDU_IN_AMSDU_CONFIG  *pAMSDUNum = NULL;
  mtlk_vap_handle_t         vap_handle = core->vap_handle;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_MSDU_IN_AMSDU_CONFIG_REQ;
  man_entry->payload_size = sizeof(UMI_MSDU_IN_AMSDU_CONFIG);

  pAMSDUNum = (UMI_MSDU_IN_AMSDU_CONFIG *)(man_entry->payload);
  pAMSDUNum->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK == res && UMI_OK == pAMSDUNum->Status) {
    *htMsduInAmsdu  = pAMSDUNum->htMsduInAmsdu;
    *vhtMsduInAmsdu = pAMSDUNum->vhtMsduInAmsdu;
    *heMsduInAmsdu = pAMSDUNum->heMsduInAmsdu;
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_MSDU_IN_AMSDU_CONFIG_REQ failed, res=%d status=%hhu",
          mtlk_vap_get_oid(vap_handle), res, pAMSDUNum->Status);
    if (UMI_OK != pAMSDUNum->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_get_amsdu_num (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_amsdu_num_cfg_t     amsdu_num_cfg;
  uint32                   amsdu_num = 0, amsdu_vnum = 0, amsdu_henum = 0;
  mtlk_core_t              *core = mtlk_core_get_master((mtlk_core_t*)hcore);
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  res = _mtlk_core_receive_amsdu_num(core, &amsdu_num, &amsdu_vnum, &amsdu_henum);
  if (MTLK_ERR_OK == res) {
    MTLK_CFG_SET_ITEM(&amsdu_num_cfg, amsdu_num, amsdu_num);
    MTLK_CFG_SET_ITEM(&amsdu_num_cfg, amsdu_vnum, amsdu_vnum);
    MTLK_CFG_SET_ITEM(&amsdu_num_cfg, amsdu_henum, amsdu_henum);
  }

  /* push result and config to clipboard*/
  return mtlk_clpb_push_res_data(clpb, res, &amsdu_num_cfg, sizeof(amsdu_num_cfg));
}

mtlk_error_t __MTLK_IFUNC
wave_core_set_tx_ampdu_density (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_error_t            res   = MTLK_ERR_OK;
  mtlk_core_t            *core  = (mtlk_core_t*)hcore;
  wave_radio_t           *radio = wave_vap_radio_get(core->vap_handle);
  mtlk_tx_ampdu_density_cfg_t *tx_ampdu_density = NULL;
  uint32                 tx_ampdu_density_size;
  mtlk_clpb_t            *clpb  = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  /* get value */
  tx_ampdu_density = mtlk_clpb_enum_get_next(clpb, &tx_ampdu_density_size);
  MTLK_CLPB_TRY(tx_ampdu_density, tx_ampdu_density_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* store new config in internal db*/
      MTLK_CFG_CHECK_ITEM_AND_CALL(tx_ampdu_density, ampdu_density, wave_radio_set_tx_ampdu_density,
                                   (radio, tx_ampdu_density->ampdu_density), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
wave_core_get_tx_ampdu_density (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res                    = MTLK_ERR_OK;
  mtlk_core_t       *core    = mtlk_core_get_master((mtlk_core_t*)hcore);
  wave_radio_t      *radio   = wave_vap_radio_get(core->vap_handle);
  mtlk_tx_ampdu_density_cfg_t  tx_ampdu_density;
  mtlk_clpb_t       *clpb    = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&tx_ampdu_density, 0, sizeof(tx_ampdu_density));
  MTLK_CFG_SET_ITEM(&tx_ampdu_density, ampdu_density, wave_radio_get_tx_ampdu_density(radio));

  return mtlk_clpb_push_res_data(clpb, res, &tx_ampdu_density, sizeof(tx_ampdu_density));
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_prepare_cac_start (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t  *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_erp_t   *erp_obj;
  wave_radio_t *radio;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  radio = wave_vap_radio_get(core->vap_handle);

  if (wave_radio_get_is_zwdfs_radio(radio) &&
      !wave_radio_get_zwdfs_ant_enabled(radio)) {
    ELOG_V("Can't do CAC on ZWDFS radio while antenna is disabled");
    return MTLK_ERR_BUSY;
  }

  erp_obj = __wave_core_erp_mgmt_get(core);
  MTLK_ASSERT(erp_obj);

  (void)mtlk_core_set_coc_pause_power_mode(core);
  (void)mtlk_coc_erp_deactivate(erp_obj);
  wave_radio_set_cac_pending(wave_vap_radio_get(core->vap_handle), TRUE);

  return MTLK_ERR_OK;
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_notify_cac_finished (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_core_t  *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_erp_t   *erp_obj;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  erp_obj = __wave_core_erp_mgmt_get(core);
  MTLK_ASSERT(erp_obj);

  (void)mtlk_core_set_coc_actual_power_mode(core);
  (void)mtlk_coc_erp_activate(erp_obj); /* activate if needed */
  wave_radio_set_cac_pending(wave_vap_radio_get(core->vap_handle), FALSE);

  return MTLK_ERR_OK;
}

static bool
mtlk_core_match_short_ssid(mtlk_hw_band_e band, mtlk_pdb_t *param_db_core,
                           u8 *sta_short_ssid)
{
  mtlk_error_t res;
  bool verdict = false;
  u8 *short_ssid_list;
  int short_ssid_count;
  mtlk_pdb_size_t list_size;
  u8 idx = 0xFF;

  /* Retrieve the number of the 6G short SSIDs from the PDB */
  short_ssid_count = wave_pdb_get_int(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_COUNT);
  ILOG3_DD("band = %d, PDB ssid_count = %d",band, short_ssid_count);

  if (short_ssid_count) {
    /* if short SSID list is present in the PDB then retrieve the list */
    list_size = short_ssid_count * RNR_6G_SHORT_SSID_LENGTH;
    short_ssid_list = mtlk_osal_mem_alloc(list_size , MTLK_MEM_TAG_CORE);
    if (!short_ssid_list) {
      ELOG_DD("Cannot allocate %u bytes for copying the short SSID list for band = %d",list_size, band);
      return verdict;
    }
    res = wave_pdb_get_binary(param_db_core, PARAM_DB_CORE_RNR_6G_SHORT_SSID_LIST, short_ssid_list, &list_size);
    if (res == MTLK_ERR_OK) {
      /* Compare the STA's short SSID with the short SSIDs of 6G colocated neighbors */  
      for (idx = 0; idx < short_ssid_count; idx++) {
          if (!memcmp(&short_ssid_list[idx * RNR_6G_SHORT_SSID_LENGTH], sta_short_ssid, RNR_6G_SHORT_SSID_LENGTH)) {
            verdict = true;
            ILOG2_DDD("Short SSID comparison results for band = %d: short_ssid_count = %d, verdict = PASS, idx = %d",
                       band, short_ssid_count, idx);
            break;
          }
        }
    }
    mtlk_osal_mem_free(short_ssid_list);
  }
  return verdict;
}

/*****************************************************************
    Same SSID(2.4/5)                  Different SSID(2.4/5)
******************************************************************
PR with wildcard SSID (OK)          PR with wildcard SSID (OK)
PR with 6G SSID (OK)                PR with 6G SSID (OK)
                                    (Driver should have the SSSID)
PR with Short SSID                  PR with Short SSID (OK)
(Driver to compute SSSID)           (Driver should have the SSSID)
*****************************************************************/

static bool
mtlk_core_out_of_band_6g_discovery(mtlk_hw_band_e band, mtlk_pdb_t *param_db_core,
                                   u8 *essid, u8 essid_len,
                                   u32 ap_short_ssid, ie_t *eshort_ssid_list)
{

#define FIRST_SHORT_SSID_OFFSET     3
#define LENGTH_FIELD_OFFSET         1
#define ELEM_ID_EXTN_SIZE           1

  u32 essid_to_short_ssid = 0;

  /* Compute the short SSID from the received SSID from the STA */
  if (essid && essid_len) {
    essid_to_short_ssid = cpu_to_le32(ieee80211_crc32(essid, essid_len));
    if (mtlk_core_match_short_ssid(band, param_db_core, (u8 *)&essid_to_short_ssid)) {
        ILOG2_D("SSID received on band %d matched with that of 6G AP", band);
        return true;
    }
  }

  /* Check for Short SSID List IE and parse */
  if (eshort_ssid_list) {
    u8 len;
    u8 *tmp = (u8 *)eshort_ssid_list;
    len = tmp[LENGTH_FIELD_OFFSET] - ELEM_ID_EXTN_SIZE; /* Length of the Short SSID list */
    tmp +=  FIRST_SHORT_SSID_OFFSET;
    ILOG2_DD("Short SSID list received with length %d on band %d", len, band);
    while (len) {
      if (!memcmp(tmp, (u8 *)&ap_short_ssid, RNR_6G_SHORT_SSID_LENGTH) || 
          mtlk_core_match_short_ssid(band, param_db_core, tmp)) {
        ILOG2_DD("Short SSID %x on band %d matched successfully", *((u32 *)tmp), band);
        return true;
      }
      len -= RNR_6G_SHORT_SSID_LENGTH;
      tmp += RNR_6G_SHORT_SSID_LENGTH;
    }
  }
  return false;

#undef FIRST_SHORT_SSID_OFFSET
#undef LENGTH_FIELD_OFFSET
#undef ELEM_ID_EXTN_SIZE
}

static inline ie_t *mtlk_core_find_eid(u8 *buf, unsigned len, u8 elem_id, u8 ext_elem_id)
{
#define ELEMENT_ID_EXTN_OFFSET 2
  u8 *end = buf + len;
  ie_t *eid = NULL;

  while (!eid && buf < end)
  {
    ie_t *ie = (ie_t *) buf;

    if ((ie->id == elem_id) &&
        ((!ext_elem_id) || (ext_elem_id == buf[ELEMENT_ID_EXTN_OFFSET])))
      eid = ie;
    buf += sizeof(ie_t) + ie->length;
  }

  if (buf > end) /* the information element was incomplete */
    eid = NULL;
  return eid;
#undef ELEMENT_ID_EXTN_OFFSET
}
/*************************************************************
 * Support of out of band and in band discovery of 6GHz AP   *
 *************************************************************
 *     Out of band 6G discovery on 2.4/5GHz interface        *
 * Compute the short SSID from the received SSID and compare *
 * with the short SSID(s) of colocated 6GHz neigh AP(s).     *
 * Parse the Short SSID List and compare with short SSID(s)  *
 * of the receiving AP and the colocated 6GHz neigh AP(s)    *
 *************************************************************
 *        In band discovery on 6GHz interface                *
 * Check for the presence of SSID List/Short SSID List IE in *
 * the Probe Req frame and compare with the SSID of the 6GHz *
 * AP that has received the frame.                           *
 *   IEEE P802.11ax/D6.0, November 2019 (Sec 26.17.2.3.1)    *
 * In 6 GHz, a STA shall not transmit a Probe Req to the     *
 * broadcast destn address that includes a Short SSID List   *
 * element with more than one Short SSID field               *
 *   IEEE P802.11ax/D6.0, November 2019 (Sec 26.17.2.3.3)    *
 * The non-AP STA shall not transmit a Probe Request frame to*
 * the broadcast destination address with the Address 3 field*
 * set to wildcard BSSID, and the SSID set to wildcard SSID. *
 *************************************************************/
bool
mtlk_core_broadcast_probe_req_enhanced_check(mtlk_hw_band_e band, mtlk_pdb_t *param_db_core, u8 *buf, u8 buf_size, IEEE_ADDR *bssid, BOOL *wildcard_ssid)
{
#define FIRST_SHORT_SSID_OFFSET     3
#define FIRST_SSID_LEN_OFFSET       3
#define FIRST_SSID_OFFSET           4

  u8 ssid[MAX_SSID_LEN];
  mtlk_pdb_size_t ssid_len = sizeof(ssid);
  ie_t *essid = NULL;
  ie_t *essid_list = NULL;
  ie_t *eshort_ssid_list = NULL;
  u8 *sta_ssid = NULL;
  u8 sta_ssid_len = 0;
  u8 sta_ssid_idx = FIRST_SSID_OFFSET;
  u8 sta_ssid_list_len = 0;
  u32 ap_short_ssid = 0;
  u8 *tmp = NULL;

  MTLK_ASSERT(NULL != wildcard_ssid);
  MTLK_ASSERT(NULL != param_db_core);
  MTLK_ASSERT(NULL != bssid);

  *wildcard_ssid = FALSE;

  if (!buf || !buf_size)
    return false;

  essid = mtlk_core_find_eid(buf, buf_size, IE_SSID, 0);

  /* SSID comparison */
  if (wave_pdb_get_binary(param_db_core, PARAM_DB_CORE_ESSID, ssid, &ssid_len) == MTLK_ERR_OK) {
    if (essid && (essid->length == ssid_len) && (!memcmp((char *) essid + sizeof(ie_t), ssid, ssid_len)))
      return true;
  } else {
    ILOG1_V("Failed to retrieve the own SSID from the Param Core DB!");
    return false;
  }

  /* Don't reply to broadcast Wildcard probe request or
   * probe request for different SSID if our SSID is hidden */
  if (wave_pdb_get_int(param_db_core, PARAM_DB_CORE_HIDDEN_SSID))
    return false;

  /* Wildcard SSID case */
  if (!essid || (essid->length == 0)) {
    *wildcard_ssid = TRUE;
    return true;
  }

  ap_short_ssid =  wave_pdb_get_int(param_db_core, PARAM_DB_CORE_SHORT_SSID);
  eshort_ssid_list = mtlk_core_find_eid(buf, buf_size, IE_EXTENSION, IE_EXT_SHORT_SSID_LIST);

  /* 6GHz inband discovery */
  if (band == MTLK_HW_BAND_6_GHZ) {
    /* Short SSID comparison */
    if (eshort_ssid_list) {
      tmp = (u8 *)eshort_ssid_list;
      ILOG2_DDD("Band = %d, AP short ssid = %x, STA short ssid = %x",
                band, ap_short_ssid, *((u32 *)&tmp[FIRST_SHORT_SSID_OFFSET]));
      if (ap_short_ssid == *((u32 *)&tmp[FIRST_SHORT_SSID_OFFSET]))
        return true;
    }

    if ((essid_list = mtlk_core_find_eid(buf, buf_size, IE_SSID_LIST, 0))) {
      /* SSID List IE processing and comparison */
      ILOG2_D("Band = %d, SSID List IE found", band);
      sta_ssid_list_len = essid_list->length;
      tmp = (u8 *)essid_list;
      sta_ssid_len = tmp[FIRST_SSID_LEN_OFFSET];

      while (sta_ssid_list_len && sta_ssid_len) {
        sta_ssid = &tmp[sta_ssid_idx];
        if (!memcmp(sta_ssid, ssid, ssid_len)) {
          ILOG2_D("SSID in band = %d matches one in the SSID List", band);
          return true;
        }
        sta_ssid_list_len -= (sta_ssid_len + sizeof(ie_t));
        sta_ssid_idx += sta_ssid_len;
        sta_ssid_len = tmp[sta_ssid_idx - 1];
      }
    }

    if (!is_broadcast_ether_addr(bssid->au8Addr))
      return true;

  /* Out of band discovery of 6GHz on 2.4/5GHz band */
  } else if (mtlk_core_out_of_band_6g_discovery(band, param_db_core, (u8 *)essid + sizeof(ie_t),
                                                essid->length, ap_short_ssid, eshort_ssid_list)) {
    return true;
  }
  return false;

#undef FIRST_SHORT_SSID_OFFSET
#undef FIRST_SSID_LEN_OFFSET
#undef FIRST_SSID_OFFSET
}

int __MTLK_IFUNC
wave_core_store_fils_discovery_frame (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  u8 *frame = mtlk_clpb_enum_get_next(clpb, &data_size);

  MTLK_ASSERT(NULL != core);

  MTLK_CLPB_TRY_EX(frame, data_size, sizeof(struct intel_vendor_fils_discovery_info) + sizeof(frame_action_head_t) + sizeof(frame_head_t))
  {
    u8 mbssid_vap = MTLK_CORE_PDB_GET_INT(core, PARAM_DB_CORE_MBSSID_VAP); 
    if (mbssid_vap < WAVE_RADIO_OPERATION_MODE_MBSSID_NON_TRANSMIT_VAP) {
      res = wave_pdb_set_binary(wave_radio_param_db_get(wave_vap_radio_get(core->vap_handle)),
                                PARAM_DB_RADIO_FILS_DISCOVERY_FRAME, frame, data_size);
      if (MTLK_ERR_OK == res)
        ILOG1_D("FILS Discovery template saved for mbssid_vap = %d", mbssid_vap);
      else
        ELOG_DDD("CID-%04x: Can't store fils_discovery frame for VAP mode %d (err=%d)", mtlk_vap_get_oid(core->vap_handle), mbssid_vap, res);
    } else {
      ILOG1_D("FILS Discovery template NOT saved for VAP mode %d", mbssid_vap);
    }
  }
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
wave_core_set_initial_data (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  struct intel_vendor_initial_data_cfg *initial_data = NULL;
  uint32 initial_data_size;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));


  initial_data = mtlk_clpb_enum_get_next(clpb, &initial_data_size);
  MTLK_CLPB_TRY(initial_data, initial_data_size)
    if ('\0' == initial_data->alpha2[0]) {
      ELOG_V("Country code is not set");
      MTLK_CLPB_EXIT(MTLK_ERR_PARAMS);
    }

    _wave_radio_set_11n_acax_compat(wave_vap_radio_get(core->vap_handle),
                                    initial_data->ieee80211n_acax_compat);

    res = core_cfg_set_hostapd_initial_data(core, initial_data);
    if (MTLK_ERR_OK != res) {
      ELOG_D("Can't set hostapd initial data, res: %d", res);
      MTLK_CLPB_EXIT(res);
    }
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
wave_core_set_he_operation (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  struct intel_vendor_he_oper *he_operation_in = NULL;
  struct intel_vendor_he_oper he_operation = {0};
  uint32 he_data_size;
  mtlk_pdb_size_t he_operation_len = sizeof(he_operation);

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  he_operation_in = mtlk_clpb_enum_get_next(clpb, &he_data_size);
  MTLK_CLPB_TRY(he_operation_in, he_data_size)
    res = MTLK_CORE_PDB_GET_BINARY(core, PARAM_DB_CORE_HE_OPERATION, &he_operation, &he_operation_len);
    if (MTLK_ERR_OK != res)
      MTLK_CLPB_EXIT(res);

    he_operation.he_bss_color_info = 0;

    if (he_operation_in->he_bss_color_info & HE_OPERATION_BSS_COLOR_DISABLED_FLAG)
      he_operation.he_bss_color_info |= (HE_OPERATION_BSS_COLOR_DISABLED_FLAG);
    else
      he_operation.he_bss_color_info &= (~HE_OPERATION_BSS_COLOR_DISABLED_FLAG);

    he_operation.he_bss_color_info |= (he_operation_in->he_bss_color_info & (HE_OPERATION_BSS_COLOR_MASK));

    res = MTLK_CORE_PDB_SET_BINARY(core, PARAM_DB_CORE_HE_OPERATION, &he_operation, sizeof(he_operation));
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
wave_core_get_he_operation (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  struct intel_vendor_he_oper he_operation = {0};
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_pdb_size_t he_operation_len = sizeof(he_operation);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  res = MTLK_CORE_PDB_GET_BINARY(core, PARAM_DB_CORE_HE_OPERATION, &he_operation, &he_operation_len);

  return mtlk_clpb_push_res_data(clpb, res, &he_operation, sizeof(he_operation));
}

int __MTLK_IFUNC
wave_core_set_he_non_advertised (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  struct intel_vendor_he_capa *he_non_advertised = NULL;
  uint32 he_data_size;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  he_non_advertised = mtlk_clpb_enum_get_next(clpb, &he_data_size);
  MTLK_CLPB_TRY(he_non_advertised, he_data_size)
    res = MTLK_CORE_PDB_SET_BINARY(core, PARAM_DB_CORE_HE_NON_ADVERTISED, he_non_advertised,
                                   sizeof(*he_non_advertised));
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
wave_core_get_he_non_advertised (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  struct intel_vendor_he_capa he_non_advertised = {0};
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_pdb_size_t he_len = sizeof(he_non_advertised);

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  res = MTLK_CORE_PDB_GET_BINARY(core, PARAM_DB_CORE_HE_NON_ADVERTISED, &he_non_advertised, &he_len);

  return mtlk_clpb_push_res_data(clpb, res, &he_non_advertised, sizeof(he_non_advertised));
}

int __MTLK_IFUNC
wave_core_set_he_debug_data (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  struct mtlk_he_debug_mode_data *he_debug = NULL;
  uint32 he_debug_size;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  he_debug = mtlk_clpb_enum_get_next(clpb, &he_debug_size);
  MTLK_CLPB_TRY(he_debug, he_debug_size)
    res = MTLK_CORE_PDB_SET_BINARY(core, PARAM_DB_CORE_HE_DEBUG_DATA, he_debug, he_debug_size);
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_send_slow_probing_mask(mtlk_core_t *core, uint32 mask)
{
  mtlk_txmm_msg_t                 man_msg;
  mtlk_txmm_data_t               *man_entry = NULL;
  UMI_DISABLE_POWER_ADAPT        *probing_mask = NULL;
  mtlk_vap_handle_t               vap_handle = core->vap_handle;
  wave_radio_t                    *radio = wave_vap_radio_get(vap_handle);
  int                             res;

  if (mask > MAX_UINT8) {
    ELOG_D("Wrong slow probing mask: %u, must be from 0 to 0xFF", mask);

    ELOG_V("SLOW_PROBING_POWER\n" \
           "SLOW_PROBING_BW\n" \
           "SLOW_PROBING_CP\n" \
           "SLOW_PROBING_BF\n" \
           "SLOW_PROBING_CDD\n" \
           "SLOW_PROBING_ANT_SEL\n" \
           "SLOW_PROBING_NONE\n" \
           "SLOW_PROBING_TASKS_NUM");

    return MTLK_ERR_PARAMS;
  }

  ILOG2_V("Sending UM_MAN_SLOW_PROBING_MASK_REQ");

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);

  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SLOW_PROBING_MASK_REQ;
  man_entry->payload_size = sizeof(UMI_DISABLE_POWER_ADAPT);

  probing_mask = (UMI_DISABLE_POWER_ADAPT *)(man_entry->payload);
  probing_mask->slowProbingMask = (uint8)mask;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != probing_mask->Status) {
    ELOG_DDD("CID-%04x: Set UM_MAN_SLOW_PROBING_MASK_REQ failed, res=%d status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, probing_mask->Status);
    if (UMI_OK != probing_mask->Status)
      res = MTLK_ERR_MAC;
  }
  else {
    WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_SLOW_PROBING_MASK, mask);
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC
mtlk_core_receive_slow_probing_mask (mtlk_core_t *core, uint32 *mask)
{
  mtlk_txmm_msg_t          man_msg;
  mtlk_txmm_data_t        *man_entry = NULL;
  UMI_DISABLE_POWER_ADAPT *probing_mask = NULL;
  mtlk_vap_handle_t        vap_handle = core->vap_handle;
  int                      res;

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SLOW_PROBING_MASK_REQ;
  man_entry->payload_size = sizeof(UMI_DISABLE_POWER_ADAPT);
  probing_mask = (UMI_DISABLE_POWER_ADAPT *)(man_entry->payload);
  probing_mask->getSetOperation = API_GET_OPERATION;

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK == res && UMI_OK == probing_mask->Status) {
      *mask = probing_mask->slowProbingMask;
  }
  else {
    ELOG_DDD("CID-%04x: Receive UM_MAN_SLOW_PROBING_MASK_REQ failed, res=%i status=%hhu",
            mtlk_vap_get_oid(vap_handle), res, probing_mask->Status);
    if (UMI_OK != probing_mask->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

static int
_mtlk_core_send_fixed_ltf_and_gi (mtlk_core_t *core, const UMI_FIXED_LTF_AND_GI_REQ *req)
{
    mtlk_txmm_msg_t            man_msg;
    mtlk_txmm_data_t           *man_entry;
    UMI_FIXED_LTF_AND_GI_REQ   *mac_msg;
    int                        res;
    unsigned                   oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    ILOG0_DDD("CID-%04x: Fixed LTF and GI - isAuto:(%d) ltfAndGi:(%d)", oid, req->isAuto, req->ltfAndGi);

    /* Allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry) {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* Fill the message data */
    man_entry->id = UM_MAN_FIXED_LTF_AND_GI_REQ;
    man_entry->payload_size = sizeof(*mac_msg);
    mac_msg = (UMI_FIXED_LTF_AND_GI_REQ *)man_entry->payload;
    mac_msg->isAuto = req->isAuto;
    mac_msg->ltfAndGi = req->ltfAndGi;

    /* Send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (res != MTLK_ERR_OK || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set UM_MAN_FIXED_LTF_AND_GI_REQ failed, res=%i status=%hhu",
              oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* Cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}

static int
_mtlk_core_cfg_store_fixed_ltf_and_gi (mtlk_core_t *core,
        const UMI_FIXED_LTF_AND_GI_REQ *fixed_ltf_and_gi)
{
  int res;
  /* store new config into internal DB */
  res = WAVE_RADIO_PDB_SET_BINARY(wave_vap_radio_get(core->vap_handle),
                                  PARAM_DB_RADIO_FIXED_LTF_AND_GI,
                                  fixed_ltf_and_gi, sizeof(*fixed_ltf_and_gi));
  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Store Fixed LTF and GI failed (%i)",
            mtlk_vap_get_oid(core->vap_handle), res);
  }
  return res;
}


void __MTLK_IFUNC
mtlk_core_cfg_get_fixed_ltf_and_gi (mtlk_core_t *core, UMI_FIXED_LTF_AND_GI_REQ *fixed_ltf_and_gi)
{
  mtlk_pdb_size_t data_size = sizeof(*fixed_ltf_and_gi);
  WAVE_RADIO_PDB_GET_BINARY(wave_vap_radio_get(core->vap_handle),
                            PARAM_DB_RADIO_FIXED_LTF_AND_GI, fixed_ltf_and_gi, &data_size);
}

int __MTLK_IFUNC
mtlk_core_set_fixed_ltf_and_gi (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t*)hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    mtlk_fixed_ltf_and_gi_t *fixed_ltf_and_gi = NULL;
    uint32 cfg_size;

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get new configuration and send it to FW */
    fixed_ltf_and_gi = mtlk_clpb_enum_get_next(clpb, &cfg_size);
    MTLK_CLPB_TRY(fixed_ltf_and_gi, cfg_size)
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
         MTLK_CFG_CHECK_ITEM_AND_CALL(fixed_ltf_and_gi, fixed_ltf_and_gi_params, _mtlk_core_send_fixed_ltf_and_gi,
                                      (core, &fixed_ltf_and_gi->fixed_ltf_and_gi_params), res);
         MTLK_CFG_CHECK_ITEM_AND_CALL(fixed_ltf_and_gi, fixed_ltf_and_gi_params, _mtlk_core_cfg_store_fixed_ltf_and_gi,
                                      (core, &fixed_ltf_and_gi->fixed_ltf_and_gi_params), res);
      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      return mtlk_clpb_push_res(clpb, res); /* push result into clipboard */
    MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_get_fixed_ltf_and_gi (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    int res = MTLK_ERR_OK;
    mtlk_fixed_ltf_and_gi_t fixed_ltf_and_gi;
    mtlk_core_t *core = (mtlk_core_t*)hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    memset(&fixed_ltf_and_gi, 0, sizeof(fixed_ltf_and_gi));
    MTLK_CFG_SET_ITEM_BY_FUNC_VOID(&fixed_ltf_and_gi, fixed_ltf_and_gi_params, mtlk_core_cfg_get_fixed_ltf_and_gi,
                                   (core, &fixed_ltf_and_gi.fixed_ltf_and_gi_params));


    /* push result and config into clipboard */
    res = mtlk_clpb_push(clpb, &res, sizeof(res));
    if (MTLK_ERR_OK == res) {
        res = mtlk_clpb_push(clpb, &fixed_ltf_and_gi, sizeof(fixed_ltf_and_gi));
    }
    return res;
}

/* DEBUG FUNCTIONS */
#ifdef CONFIG_WAVE_DEBUG


/************* WLAN counters source *******************/
static uint32 _mtlk_core_cfg_read_counters_src (mtlk_core_t *core)
{
    MTLK_ASSERT(core != NULL);
    return WAVE_RADIO_PDB_GET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_WLAN_COUNTERS_SRC);
}

static void _mtlk_core_cfg_store_counters_src (mtlk_core_t *core, const unsigned src)
{
    MTLK_ASSERT(core != NULL);
    WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_WLAN_COUNTERS_SRC, src);
}

int __MTLK_IFUNC mtlk_core_cfg_get_counters_src (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_wlan_counters_src_cfg_t counters_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  memset(&counters_cfg, 0, sizeof(counters_cfg));
  MTLK_CFG_SET_ITEM(&counters_cfg, src, _mtlk_core_cfg_read_counters_src(core));

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &counters_cfg, sizeof(counters_cfg));
  }
  return res;
}

int __MTLK_IFUNC mtlk_core_cfg_set_counters_src (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_wlan_counters_src_cfg_t *counters_cfg = NULL;
  uint32 counters_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  counters_cfg = mtlk_clpb_enum_get_next(clpb, &counters_cfg_size);
  MTLK_CLPB_TRY(counters_cfg, counters_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(counters_cfg, src, _mtlk_core_cfg_store_counters_src, (core, counters_cfg->src));
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

/*------ IRE Control B --------------------------------------------*/

#define MTLK_IRE_CTRL_DFLT  SET_AP_LOW_CHANNEL

static BOOL
_mtlk_core_cfg_ire_ctrl_is_supported (mtlk_core_t *core)
{
  return FALSE;
}

static int
_mtlk_core_cfg_send_ire_ctrl_req (mtlk_core_t *core, IREControl_e req_value)
{
  mtlk_vap_handle_t             vap_handle = core->vap_handle;
  mtlk_txmm_msg_t               man_msg;
  mtlk_txmm_data_t             *man_entry = NULL;
  UMI_CONTROL                  *req = NULL;
  unsigned                      oid;
  int                           res;

  MTLK_ASSERT(vap_handle);
  oid = mtlk_vap_get_oid(vap_handle);

  ILOG1_DD("CID-%04x: Set IRE_CONTROL_B to %d", oid, req_value);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_SET_IRE_SWITCH_B_REQ;
  man_entry->payload_size = sizeof(UMI_CONTROL);

  req = (UMI_CONTROL *)(man_entry->payload);
  req->setAPHighLowFilterBank = req_value; /* int8 */

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != req->Status) {
    ELOG_DDD("CID-%04x: Set IRE_CONTROL_B failure, res=%d status=%hhu", oid, res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

/* Read IRE Control from PDB */
static int
_mtlk_core_cfg_read_ire_ctrl (mtlk_core_t *core)
{
  int val;
   wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  val = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_IRE_CTRL_B);

  return val;
}

/* Store IRE Control in PDB */
static void
_mtlk_core_cfg_store_ire_ctrl (mtlk_core_t *core, int value)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_IRE_CTRL_B, value);
}

static int
_mtlk_core_cfg_store_and_send_ire_ctrl (mtlk_core_t *core, int value)
{
  int res = MTLK_ERR_UNKNOWN;
  if(!_mtlk_core_cfg_ire_ctrl_is_supported(core)) {
    ELOG_V("IRE Control is only supported on Gen5 with 5 GHz band");
    return MTLK_ERR_NOT_SUPPORTED;
  }

  if (SET_AP_LOW_CHANNEL != value &&
      SET_AP_HIGH_CHANNEL != value) {
    ELOG_DDD("Wrong vaule (%d), expected %d or %d", value,
             SET_AP_LOW_CHANNEL, SET_AP_HIGH_CHANNEL);

    return MTLK_ERR_PARAMS;
  }

  res = _mtlk_core_cfg_send_ire_ctrl_req(core, value);
  if (res == MTLK_ERR_OK)
    _mtlk_core_cfg_store_ire_ctrl(core, value);

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_set_ire_ctrl_b (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                       res = MTLK_ERR_OK;
  uint32                    cfg_size;
  mtlk_core_t               *core = (mtlk_core_t*)hcore;
  mtlk_ire_ctrl_cfg_t       *ire_ctrl_cfg = NULL;
  mtlk_clpb_t               *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  /* get configuration */
  ire_ctrl_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(ire_ctrl_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(ire_ctrl_cfg, ire_ctrl_value,
          _mtlk_core_cfg_store_and_send_ire_ctrl, (core, ire_ctrl_cfg->ire_ctrl_value), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
mtlk_core_cfg_get_ire_ctrl_b (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_ire_ctrl_cfg_t      ire_ctrl_cfg;
  mtlk_core_t              *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t              *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&ire_ctrl_cfg, 0, sizeof(ire_ctrl_cfg));

  if (_mtlk_core_cfg_ire_ctrl_is_supported(core)) {
    /* read config from internal db */
    MTLK_CFG_SET_ITEM(&ire_ctrl_cfg, ire_ctrl_value, _mtlk_core_cfg_read_ire_ctrl(core));
  } else {
    res = MTLK_ERR_NOT_SUPPORTED;
  }

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &ire_ctrl_cfg, sizeof(ire_ctrl_cfg));
  }

  return res;
}

int __MTLK_IFUNC
mtlk_core_cfg_recovery_ire_ctrl (mtlk_core_t *core)
{
  int   val;
  int   res = MTLK_ERR_OK;

  val = _mtlk_core_cfg_read_ire_ctrl(core);
  if (MTLK_IRE_CTRL_DFLT != val) {
    res = _mtlk_core_cfg_send_ire_ctrl_req(core, val);
  }

  return res;
}

/* Test Bus mode configuration */
static void
_wave_radio_store_test_bus_mode (wave_radio_t *radio, const int mode)
{
  MTLK_ASSERT(radio != NULL);
  WAVE_RADIO_PDB_SET_INT(radio, PARAM_DB_RADIO_TEST_BUS_MODE, mode);
}

int __MTLK_IFUNC
wave_core_send_test_bus_mode (mtlk_core_t *core, const uint32 mode)
{
  mtlk_txmm_msg_t         man_msg;
  mtlk_txmm_data_t       *man_entry;
  UMI_TEST_BUS_EN        *mac_msg;
  int                     res = MTLK_ERR_OK;

  ILOG1_DD("CID-%04x: TestBus mode %d", mtlk_vap_get_oid(core->vap_handle), mode);

  /* allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  /* fill the message data */
  man_entry->id = UM_MAN_TEST_BUS_EN_REQ;
  man_entry->payload_size = sizeof(*mac_msg);
  mac_msg = (UMI_TEST_BUS_EN *)man_entry->payload;
  memset(mac_msg, 0, sizeof(*mac_msg));

  MTLK_STATIC_ASSERT(sizeof(uint32) == sizeof(mac_msg->enable));
  mac_msg->enable = MAC_TO_HOST32(mode);

  /* send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  /* cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

int __MTLK_IFUNC
wave_core_set_test_bus_mode (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_core_t       *core = mtlk_core_get_master((mtlk_core_t*)hcore);
  wave_radio_t      *radio = wave_vap_radio_get(core->vap_handle);
  wave_ui_mode_t    *cfg;
  uint32             cfg_size;
  mtlk_clpb_t       *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      /* send new config to FW */
      MTLK_CFG_CHECK_ITEM_AND_CALL(cfg, mode, wave_core_send_test_bus_mode,
                                   (core, cfg->mode), res);
      /* store new config in internal DB */
      MTLK_CFG_CHECK_ITEM_AND_CALL_VOID(cfg, mode, _wave_radio_store_test_bus_mode,
                                        (radio, cfg->mode));
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int mtlk_core_cfg_get_wds_wpa_entry(mtlk_handle_t hcore, const void* data,
  uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);

  return _mtlk_core_dump_ieee_addr_list (core, &core->wds_wpa_sta_list,
    "WDS WPA", data, data_size);
}

int mtlk_core_cfg_get_multi_ap_blacklist_entries(mtlk_handle_t hcore,
  const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);

  return _mtlk_core_dump_ieee_addr_list (core, &core->multi_ap_blacklist,
    "Multi-AP black", data, data_size);
}

static int _wave_core_cfg_send_debug_command (mtlk_core_t *core, mtlk_debug_cmd_fw_t *debug_cmd)
{
  mtlk_txmm_msg_t    man_msg;
  mtlk_txmm_data_t  *man_entry = NULL;
  mtlk_vap_handle_t  vap_handle = core->vap_handle;
  int res;

  MTLK_ASSERT(vap_handle);

  ILOG0_DDD("CID-%04x: Debug FW request: Message ID 0x%04X, size %u", mtlk_vap_get_oid(vap_handle), debug_cmd->cmd_id, debug_cmd->len);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);
  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", mtlk_vap_get_oid(vap_handle));
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = debug_cmd->cmd_id;
  man_entry->payload_size = debug_cmd->len;

  wave_memcpy(man_entry->payload, debug_cmd->len, debug_cmd->data, debug_cmd->len);
  mtlk_dump(0, man_entry->payload, debug_cmd->len, "dump of Message before sending:");

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Debug FW request failure (%i)", mtlk_vap_get_oid(vap_handle), res);
  }

  mtlk_dump(0, man_entry->payload, debug_cmd->len, "dump of Message after CFM:");

  mtlk_txmm_msg_cleanup(&man_msg);
  return res;
}

int __MTLK_IFUNC wave_core_cfg_set_debug_cmd (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  int                      res = MTLK_ERR_OK;
  mtlk_core_t             *core = (mtlk_core_t*)hcore;
  mtlk_debug_cmd_fw_cfg_t *debug_cmd_cfg = NULL;
  uint32                   debug_cmd_cfg_size;
  mtlk_clpb_t             *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(mtlk_vap_is_master_ap(core->vap_handle));

  debug_cmd_cfg = mtlk_clpb_enum_get_next(clpb, &debug_cmd_cfg_size);

  MTLK_CLPB_TRY(debug_cmd_cfg, debug_cmd_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(debug_cmd_cfg, debug_cmd, _wave_core_cfg_send_debug_command,
                                   (core, &debug_cmd_cfg->debug_cmd), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

#endif /* CONFIG_WAVE_DEBUG */

int __MTLK_IFUNC
wave_core_cfg_send_cca_preamble_puncture_override(mtlk_core_t *core, const UMI_PREAMBLE_PUNCT_CCA_OVERRIDE *req)
{
  mtlk_txmm_msg_t  man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_PREAMBLE_PUNCT_CCA_OVERRIDE  *mac_msg;
  int              res = MTLK_ERR_UNKNOWN;
  unsigned         oid;

  MTLK_ASSERT(core != NULL);
  oid = mtlk_vap_get_oid(core->vap_handle);

  if(!((MTLK_HW_BAND_5_2_GHZ == core_cfg_get_freq_band_cur(core)) ||
       (MTLK_HW_BAND_6_GHZ == core_cfg_get_freq_band_cur(core)))) {
    ELOG_V("Only supported on 5GHz/6GHZ bands");
    return MTLK_ERR_NOT_SUPPORTED;
  }

  ILOG1_DDDD("CID-%04x: preamble_puncture_cca -ccaOverride (%d) ccaBitmap (%d) forceFreeBands (%d)", oid, req->ccaOverride,
                                                                            req->ccaBitmap, req->forceFreeBands);

  /* Allocate a new message */
  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
  if (!man_entry) {
    ELOG_D("CID-%04x: Can not get TXMM slot", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  /* Fill the message data */
  man_entry->id = UM_MAN_PREAMBLE_PUNCTURE_CCA_OVERRIDE_REQ;
  man_entry->payload_size = sizeof(*mac_msg);
  mac_msg = (UMI_PREAMBLE_PUNCT_CCA_OVERRIDE *)man_entry->payload;
  mac_msg->ccaOverride = req->ccaOverride;
  mac_msg->ccaBitmap = req->ccaBitmap;
  mac_msg->forceFreeBands = req->forceFreeBands;

  mtlk_dump(1, mac_msg, sizeof(*mac_msg), "dump of UMI_PREAMBLE_PUNCT_CCA_OVERRIDE");

  /* Send the message to FW */
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);
  if (MTLK_ERR_OK != res || UMI_OK != req->Status) {
    ELOG_DDD("CID-%04x: Set UMI_PREAMBLE_PUNCT_CCA_OVERRIDE failure, res=%d status=%hhu", oid, res, req->Status);
    if (UMI_OK != req->Status)
      res = MTLK_ERR_MAC;
  }

  /* Cleanup the message */
  mtlk_txmm_msg_cleanup(&man_msg);

  return res;
}

static int wave_core_cfg_store_and_send_cca_preamble_puncture_override(mtlk_core_t *core, const UMI_PREAMBLE_PUNCT_CCA_OVERRIDE *req)
{

  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  int res;
  mtlk_hw_t *hw                         = mtlk_vap_get_hw(core->vap_handle);
  uint32 chip_id                        = hw_mmb_get_chip_id(hw);

  if (_chipid_is_gen6_d2_or_gen7(chip_id)) {
    res = wave_core_cfg_send_cca_preamble_puncture_override(core, req);
    if (MTLK_ERR_OK == res) {
      /* store new config into internal DB */
      res = WAVE_RADIO_PDB_SET_BINARY(radio, PARAM_DB_RADIO_PREAMBLE_PUNCTURE_CFG, req, sizeof(req));
      if (MTLK_ERR_OK != res) {
        ELOG_DPD("CID-%04x: radio=%p,Store PREAMBLE_PUNCTURE failed (%i)", mtlk_vap_get_oid(core->vap_handle), radio, res);
      }
    }
  }
  else {
    return MTLK_ERR_OK;
  }
  return res;
}


/* Read preamble_punct_cca_ov_cfg data from PDB */
int __MTLK_IFUNC
wave_core_cfg_read_preamble_punct_cca_ov_cfg (mtlk_core_t *core, UMI_PREAMBLE_PUNCT_CCA_OVERRIDE *req)
{
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);

  if (MTLK_ERR_OK != wave_radio_preamble_punct_cca_ov_cfg_get(radio, req)) {
    ELOG_D("CID-%04x: Can't read CCA_TH from PDB", mtlk_vap_get_oid(core->vap_handle));
    return MTLK_ERR_PARAMS;
  }
  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
wave_core_cfg_set_cca_preamble_puncture_override (mtlk_handle_t hcore,
                                                  const void* data, uint32 data_size)
{
  int                       res = MTLK_ERR_OK;
  uint32                    cfg_size;
  mtlk_core_t               *core = (mtlk_core_t*)hcore;
  mtlk_preamble_punct_cca_ov_cfg_t  *preamble_punct_cca_ov_cfg = NULL;
  mtlk_clpb_t               *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  /* get configuration */
  preamble_punct_cca_ov_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(preamble_punct_cca_ov_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(preamble_punct_cca_ov_cfg, preamble_punct_params,
          wave_core_cfg_store_and_send_cca_preamble_puncture_override, (core, &preamble_punct_cca_ov_cfg->preamble_punct_params), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}


int __MTLK_IFUNC
wave_core_cfg_get_cca_preamble_puncture_override (mtlk_handle_t hcore,
                                                  const void* data, uint32 data_size)
{
  mtlk_preamble_punct_cca_ov_cfg_t  preamble_punct_cca_ov_cfg;
  int                       res = MTLK_ERR_OK;
  mtlk_core_t               *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t               *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);
  MTLK_ASSERT(core);

  memset(&preamble_punct_cca_ov_cfg, 0, sizeof(preamble_punct_cca_ov_cfg));

  /* read config from internal db */
  MTLK_CFG_SET_ITEM_BY_FUNC(&preamble_punct_cca_ov_cfg, preamble_punct_params, wave_core_cfg_read_preamble_punct_cca_ov_cfg,
                              (core, &preamble_punct_cca_ov_cfg.preamble_punct_params), res);

  /* push result and config to clipboard*/
  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &preamble_punct_cca_ov_cfg, sizeof(preamble_punct_cca_ov_cfg));
  }

  return res;
}

int __MTLK_IFUNC
_handle_fw_interference_ind (mtlk_handle_t core_object, const void *data, uint32 data_size)
{
  mtlk_core_t* core = HANDLE_T_PTR(mtlk_core_t, core_object);
  wave_radio_t *radio = wave_vap_radio_get(core->vap_handle);
  const UMI_CONTINUOUS_INTERFERER *interferer_ind;
  struct intel_vendor_channel_data ch_data;
  int res;
  BOOL emulating;
  iwpriv_cca_adapt_t cca_adapt_params;
  iwpriv_cca_th_t    cca_th_params;
  mtlk_pdb_size_t    cca_adapt_size = sizeof(cca_adapt_params);

  if (sizeof(UMI_CONTINUOUS_INTERFERER) == data_size) {
    interferer_ind = (UMI_CONTINUOUS_INTERFERER *)data;
    emulating = FALSE;
  }
  else if (sizeof(mtlk_core_interferer_emu_t) == data_size) { /* Emulating */
    mtlk_core_interferer_emu_t *interferer_emu = (mtlk_core_interferer_emu_t *)data;
    interferer_ind = &interferer_emu->interf_data;
    emulating = interferer_emu->emulating_interferer;
  }
  else {
    ELOG_D("Wrong radar event data size %d", data_size);
    return MTLK_ERR_UNKNOWN;
  }

  ILOG3_DD("UMI_CONTINUOUS_INTERFERER event: Channel: %u, maximumValue: %d",
           interferer_ind->channel, interferer_ind->maximumValue);

  if (mtlk_vap_is_master_ap(core->vap_handle)) {
    if (core->is_stopped) {
      ILOG2_V("UMI_CONTINUOUS_INTERFERER event while core is down");
      return MTLK_ERR_OK; /* do not process */
    }

    if (!wave_radio_interfdet_get(radio)) {
      ILOG3_V("UMI_CONTINUOUS_INTERFERER event while interference detection is deactivated");
      return MTLK_ERR_OK; /* do not process */
    }

    if (mtlk_core_scan_is_running(core)) {
      ILOG2_V("UMI_CONTINUOUS_INTERFERER event while scan is running");
      return MTLK_ERR_OK; /* do not process */
    }

    if (WAVE_RADIO_OFF == WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_MODE_CURRENT)) {
      ILOG2_V("UMI_CONTINUOUS_INTERFERER event while RF is off");
      return MTLK_ERR_OK; /* do not process */
    }

    if (MTLK_ERR_OK == WAVE_RADIO_PDB_GET_BINARY(radio, PARAM_DB_RADIO_CCA_ADAPT, &cca_adapt_params, &cca_adapt_size)) {
      if (cca_adapt_params.initial) {
        mtlk_osal_timer_cancel_sync(&core->slow_ctx->cca_step_down_timer);
        core->slow_ctx->cca_adapt.cwi_poll = TRUE;
        core->slow_ctx->cca_adapt.cwi_drop_detected = FALSE;
        core->slow_ctx->cca_adapt.stepping_down = FALSE;
        core->slow_ctx->cca_adapt.stepping_up = FALSE;
        core->slow_ctx->cca_adapt.step_down_coef = 1;

        /* read user config */
        if (MTLK_ERR_OK != mtlk_core_cfg_read_cca_threshold(core, &cca_th_params)) {
          return MTLK_ERR_UNKNOWN;
        }

        ILOG3_V("CCA adaptive: stop adaptation");

        if (_mtlk_core_cca_is_above_configured(core, &cca_th_params))
        {
          int i;

          ILOG2_V("CCA adaptive: restore original values");

          for (i = 0; i < MTLK_CCA_TH_PARAMS_LEN; i++) {
            core->slow_ctx->cca_adapt.cca_th_params[i] = cca_th_params.values[i];
          }
          res = mtlk_core_cfg_send_cca_threshold_req(core, &cca_th_params);
          if (res != MTLK_ERR_OK)
            ELOG_DD("CID-%04x: Can't send CCA thresholds (err=%d)", mtlk_vap_get_oid(core->vap_handle), res);
        }
      }

      if (is_interf_det_switch_needed(interferer_ind->channel)) {
        ILOG2_DDD("CID-%04x: Interference is detected on channel %d with Metric %d", mtlk_vap_get_oid(core->vap_handle),
                   interferer_ind->channel, interferer_ind->maximumValue);

        res = fill_channel_data(core, &ch_data);

        if (emulating) {
          //As part of false interferer emulation,noise is set -10.So that hostapd will switch channel
          ch_data.cwi_noise = -10;
        }

        if (res == MTLK_ERR_OK) mtlk_send_chan_data(core->vap_handle, &ch_data, sizeof(ch_data));
      }
    }
  }

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
_mtlk_core_emulate_interferer (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_error_t res;
  mtlk_core_interferer_emu_t interferer_emu;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  struct mtlk_chan_def *ccd = __wave_core_chandef_get(core);
  int cur_channel = ieee80211_frequency_to_channel(ccd->chan.center_freq);

  if (!is_channel_certain(ccd)) {
    ELOG_V("Can not emulate radar, channel is not certain");
    return MTLK_ERR_UNKNOWN;
  }

  if (!cur_channel) {
    ELOG_D("Could not find channel for frequency %d", ccd->chan.center_freq);
    return MTLK_ERR_UNKNOWN;
  }

  memset(&interferer_emu, 0, sizeof(interferer_emu));

  interferer_emu.interf_data.channel = cur_channel;
  interferer_emu.interf_data.maximumValue = 0;
  interferer_emu.emulating_interferer = TRUE;

  res = _handle_fw_interference_ind(hcore, &interferer_emu, sizeof(interferer_emu));

  return mtlk_clpb_push_res(clpb, res);
}

static mtlk_error_t
_wave_core_send_tx_twt_teardown (mtlk_core_t *core, UMI_SEND_TWT_TEARDOWN_FRAME *tx_twt_teardown)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_txmm_msg_t man_msg;
  mtlk_txmm_data_t *man_entry;
  UMI_SEND_TWT_TEARDOWN_FRAME *req;
  mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);
  wave_vap_id_t vap_id_fw = mtlk_vap_get_id_fw(core->vap_handle);
  uint16 sid = tx_twt_teardown->staId;

  if (!mtlk_hw_type_is_gen6_d2_or_gen7(hw)) {
    ILOG0_V("Hw type is neither WAV600D2 nor WAV700! Tx TWT Teardown UMI not sent to FW.");
    return MTLK_ERR_NOT_SUPPORTED;;
  }
  /*Validate SID */
  if (!wave_hw_is_sid_in_use(hw, sid, vap_id_fw)) {
    ILOG0_D("SID %d not in use currently. Message not sent to FW", sid);
    return MTLK_ERR_PARAMS;
  }
  /* prepare msg for the FW */
  if (!(man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), &res))) {
    ELOG_DD("CID-%04x: UM_MAN_SEND_TWT_TEARDOWN_FRAME_REQ init failed, err=%i",
            mtlk_vap_get_oid(core->vap_handle), res);
    res = MTLK_ERR_NO_RESOURCES;
    goto end;
  }
  man_entry->id = UM_MAN_SEND_TWT_TEARDOWN_FRAME_REQ;
  req = (UMI_SEND_TWT_TEARDOWN_FRAME *) man_entry->payload;
  *req = *tx_twt_teardown;
  req->staId = HOST_TO_MAC16(sid);
  mtlk_dump(2, req, sizeof(*req), "dump of UMI_SEND_TWT_TEARDOWN_FRAME:");
  /* send message to FW */
  ILOG1_V("Send Tx TWT Teardown UMI to FW");
  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (res != MTLK_ERR_OK || UMI_OK != req->status) {
    ELOG_DDD("CID-%04x: UM_MAN_SEND_TWT_TEARDWON_FRAME_REQ send failed, res=%i status=%hhu",
             mtlk_vap_get_oid(core->vap_handle), res, req->status);
    if (UMI_OK != req->status)
      res = MTLK_ERR_MAC;
  }
  if (NULL != man_entry)
    mtlk_txmm_msg_cleanup(&man_msg);
end:
  return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_tx_twt_teardown (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  mtlk_tx_twt_teardown_cfg_t *tx_twt_teardown_cfg = NULL;
  uint32 cfg_size;

  MTLK_ASSERT(core != NULL);
  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* Fetch the Tx TWT Teardown parameters and send it to FW */
  tx_twt_teardown_cfg = mtlk_clpb_enum_get_next(clpb, &cfg_size);
  MTLK_CLPB_TRY(tx_twt_teardown_cfg, cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
       MTLK_CFG_CHECK_ITEM_AND_CALL(tx_twt_teardown_cfg, config, _wave_core_send_tx_twt_teardown,
                                    (core, &tx_twt_teardown_cfg->config), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res); /* push result into clipboard */
  MTLK_CLPB_END
}

int
wv_ieee80211_vendor_event_coc (uint8 event_num, struct wireless_dev *wdev, const void * data, int data_len)
{
  mtlk_nbuf_t *evt_nbuf;
  MTLK_ASSERT(NULL != wdev);
  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len, event_num);
  if (!evt_nbuf)
  {
    ELOG_V("Malloc event fail");
    return MTLK_ERR_NO_MEM;
  }

  if (data != NULL && data_len > 0) {
    wave_memcpy(evt_nbuf->data + evt_nbuf->len, data_len, data, data_len);
    mtlk_df_nbuf_put(evt_nbuf, data_len);
  } else {
    ELOG_V("COC send  event fail");
    return MTLK_ERR_NO_RESOURCES;
  }

  wv_cfg80211_vendor_event(evt_nbuf);
  return MTLK_ERR_OK;
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_send_whm_hw_logger_cfg (mtlk_handle_t core_object, const void *data, uint32 data_size)
{
    mtlk_core_t* core = HANDLE_T_PTR(mtlk_core_t, core_object);
    mtlk_txmm_msg_t man_msg;
    mtlk_txmm_data_t *man_entry;
    UMI_WHM_HW_LOGGER_ENABLE *mac_msg;
    mtlk_error_t res = MTLK_ERR_OK;
    unsigned oid;
    wave_whm_hwlogger_cfg_t *hw_logger;

    MTLK_ASSERT(core);
    oid = mtlk_vap_get_oid(core->vap_handle);

    if (data_size == sizeof(wave_whm_hwlogger_cfg_t))
      hw_logger = (wave_whm_hwlogger_cfg_t*)data;
    else {
      ELOG_V("Error reading hw logger cfg params");
      return MTLK_ERR_PARAMS;
    }

    /* allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry)
    {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* fill the message data */
    man_entry->id = UM_MAN_ENABLE_HW_LOGGER_REQ;
    man_entry->payload_size = sizeof(UMI_WHM_HW_LOGGER_ENABLE);
    mac_msg = (UMI_WHM_HW_LOGGER_ENABLE *)man_entry->payload;
    memset(mac_msg, 0, sizeof(*mac_msg));
    mac_msg->fwWarningHandlingStatus = HOST_TO_MAC32(hw_logger->whm_hwlogger_cfg.warn_handled); 
    mac_msg->enableHwLogger = hw_logger->whm_hwlogger_cfg.whm_hw_logger_enable;

    /* send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set UMI_WHM_HW_LOGGER_ENABLE failed, res=%d status=%hhu",
               oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }

    /* cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_cfg_send_whm_cfg (mtlk_core_t *core, BOOL whm_enable, BOOL whm_fw_warn, BOOL whm_phy_warn)
{
    mtlk_txmm_msg_t   man_msg;
    mtlk_txmm_data_t  *man_entry;
    UMI_WHM_ENABLE    *mac_msg;
    mtlk_error_t      res = MTLK_ERR_OK;
    unsigned          oid;

    MTLK_ASSERT(core != NULL);
    oid = mtlk_vap_get_oid(core->vap_handle);

    /* allocate a new message */
    man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(core->vap_handle), NULL);
    if (!man_entry)
    {
      ELOG_D("CID-%04x: Can not get TXMM slot", oid);
      return MTLK_ERR_NO_RESOURCES;
    }

    /* fill the message data */
    man_entry->id = UM_MAN_ENABLE_WHM_REQ;
    man_entry->payload_size = sizeof(UMI_WHM_ENABLE);
    mac_msg = (UMI_WHM_ENABLE *)man_entry->payload;
    memset(mac_msg, 0, sizeof(*mac_msg));
    mac_msg->whmEnable = whm_enable;
    mac_msg->fwWarningsEnable = whm_fw_warn;
    mac_msg->phyWarningsEnable = whm_phy_warn;

    /* send the message to FW */
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK != res || UMI_OK != mac_msg->Status) {
      ELOG_DDD("CID-%04x: Set UMI_WHM_ENABLE failed, res=%d status=%hhu",
               oid, res, mac_msg->Status);
      if (UMI_OK != mac_msg->Status)
        res = MTLK_ERR_MAC;
    }
    else {
      /* Store in PDB */
      WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_WHM_ENABLE, whm_enable);
      WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_WHM_FW_WARN, whm_fw_warn);
      WAVE_RADIO_PDB_SET_INT(wave_vap_radio_get(core->vap_handle), PARAM_DB_RADIO_WHM_PHY_WARN, whm_phy_warn);
    }

    /* cleanup the message */
    mtlk_txmm_msg_cleanup(&man_msg);

    return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_set_whm_config (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    mtlk_error_t res = MTLK_ERR_OK;
    mtlk_core_t *core = (mtlk_core_t *) hcore;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    wave_whm_cfg_t *whm_cfg = NULL;
    uint32 whm_cfg_size;
    mtlk_hw_t *hw = mtlk_vap_get_hw(core->vap_handle);

    MTLK_ASSERT(core != NULL);
    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    whm_cfg = mtlk_clpb_enum_get_next(clpb, &whm_cfg_size);
    MTLK_CLPB_TRY(whm_cfg, whm_cfg_size)

      /* Set Driver WHM cfg and WHM state */
      wave_hw_set_whm_enable(hw, whm_cfg->whm_cfg.whm_enable);
      wave_hw_set_whm_driver_warn(hw, whm_cfg->whm_cfg.whm_drv_warn);
      if(whm_cfg->whm_cfg.whm_enable) {
        wave_hw_set_whm_state_machine(hw, WHM_STATE_READY);
      }
      MTLK_CFG_START_CHEK_ITEM_AND_CALL()
        /* send whm cfg to FW */
        MTLK_CFG_CHECK_ITEM_AND_CALL(whm_cfg, whm_cfg, wave_core_cfg_send_whm_cfg,
                                     (core, whm_cfg->whm_cfg.whm_enable, whm_cfg->whm_cfg.whm_fw_warn,
                                     whm_cfg->whm_cfg.whm_phy_warn), res);

      MTLK_CFG_END_CHEK_ITEM_AND_CALL()
    MTLK_CLPB_FINALLY(res)
      /* push result into clipboard */
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

static mtlk_error_t
_wave_core_set_whm_reset(void)
{
  mtlk_error_t res = MTLK_ERR_UNKNOWN;
  uint32 num_cards, i;
  mtlk_hw_t *hw;

  num_cards = wave_hw_mmb_get_current_card_idx();
  for (i = 0; i < num_cards; i++) {
    hw = wave_hw_mmb_get_current_card(i);
    if (hw != NULL) {
      wave_hw_set_whm_state_machine(hw, WHM_STATE_READY);
      wave_hw_reset_fw_log_buffer(hw);
      res = MTLK_ERR_OK;
    }
  }
  return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_set_whm_reset (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    mtlk_error_t res = MTLK_ERR_UNKNOWN;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    wave_whm_reset_t *whm_reset = NULL;
    uint32 whm_reset_size;

    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    /* get configuration */
    whm_reset = mtlk_clpb_enum_get_next(clpb, &whm_reset_size);
    MTLK_CLPB_TRY(whm_reset, whm_reset_size)
      if(whm_reset->whm_reset) {
        res = _wave_core_set_whm_reset();
      }
    MTLK_CLPB_FINALLY(res)
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

static mtlk_error_t
wave_whm_nl_send_msg(const uint8 event_id, struct wireless_dev *wdev,
                     const void *data, size_t data_len)
{
  mtlk_nbuf_t *evt_nbuf;
  uint8* cp;

  MTLK_ASSERT(NULL != wdev);
  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len, event_id);

  if (!evt_nbuf)
  {
    ELOG_D("Malloc event fail. data_len = %d", data_len);
    return MTLK_ERR_NO_MEM;
  }

  if (data != NULL && data_len > 0) {
    cp = mtlk_df_nbuf_put(evt_nbuf, data_len);
    wave_memcpy(cp, data_len, data, data_len);
  }

  wv_cfg80211_vendor_event(evt_nbuf);
  return MTLK_ERR_OK;
}

mtlk_error_t
_wave_whm_nl_send_msg(const uint8 event_id, struct wireless_dev *wdev,
                      uint8 warning_id, uint8 layer_info)
{
  struct intel_vendor_whm_event_cfg whm_nl_event_data;

  MTLK_ASSERT(NULL != wdev);

  whm_nl_event_data.warning_id    = warning_id;
  whm_nl_event_data.warning_layer = layer_info;
  whm_nl_event_data.num_cards     = wave_hw_mmb_get_current_card_idx();

  return wave_whm_nl_send_msg(event_id, wdev, &whm_nl_event_data, sizeof(whm_nl_event_data));
}

int __MTLK_IFUNC
wave_core_fw_warning_detection_ind_handle (mtlk_handle_t core_object, const void *data, uint32 data_size)
{
  const UMI_WHM_WARNING_DETECTION *whm_fw_detection_ind;
  struct nic *nic = HANDLE_T_PTR(struct nic, core_object);
  mtlk_hw_t *hw = mtlk_vap_get_hw(nic->vap_handle);
  mtlk_vap_handle_t vap_handle = nic->vap_handle;
  mtlk_df_t *df                = mtlk_vap_get_df(vap_handle);
  mtlk_df_user_t *df_user      = mtlk_df_get_user(df);
  struct wireless_dev *wdev    = mtlk_df_user_get_wdev(df_user);
  wave_whm_state_mac whm_state;
  wave_whm_hwlogger_cfg_t hw_logger;
  int res;

  if (sizeof(UMI_WHM_WARNING_DETECTION) == data_size) {
    whm_fw_detection_ind = (UMI_WHM_WARNING_DETECTION *)data;
  }
  else {
    ELOG_DD("Wrong WHM FW event detection data size %u : expected : %d", data_size, sizeof(UMI_WHM_WARNING_DETECTION));
    return MTLK_ERR_PARAMS;
  }

  ILOG0_SDD("UMI_WHM_WARNING_DETECTION event: %s :Detection_layer : %u, Warning_Id: %u",
            mtlk_df_get_name(mtlk_vap_get_df(vap_handle)),
            whm_fw_detection_ind->whmRaisedByLayer, whm_fw_detection_ind->whmWarningId);

  /* check whm state machine */
  wave_hw_whm_lock_acquire(hw);
  whm_state = wave_hw_get_whm_state_machine(hw);

  switch (whm_state)
  {
    case WHM_STATE_READY:
      wave_hw_set_whm_state_machine(hw, WHM_STATE_BUSY);
      wave_hw_whm_lock_release(hw);
      /* collect fw logs to local buffer */
      res = wave_hw_copy_fw_trace_buffers(hw);
      if (res != MTLK_ERR_OK) {
        ELOG_V("Error in copying WHM FW Logs");
        wave_hw_set_whm_state_machine(hw, WHM_STATE_READY);
        hw_logger.whm_hwlogger_cfg.warn_handled = FW_WARNING_NOT_HANDLED;
        hw_logger.whm_hwlogger_cfg.whm_hw_logger_enable = 1;
        wave_core_cfg_set_whm_hw_logger_cfg (nic, &hw_logger, sizeof(hw_logger));
        return res;
      }
      /* Enable WHM in FW after warning is handled */
      hw_logger.whm_hwlogger_cfg.warn_handled = FW_WARNING_HANDLED;
      hw_logger.whm_hwlogger_cfg.whm_hw_logger_enable = 1;
      wave_core_cfg_set_whm_hw_logger_cfg (nic, &hw_logger, sizeof(hw_logger));
	  break;
    case WHM_STATE_BUSY:
      ELOG_D("WHM state is BUSY-Can't handle FW warning-ID : %d",whm_fw_detection_ind->whmWarningId);
      wave_hw_whm_lock_release(hw);
      hw_logger.whm_hwlogger_cfg.warn_handled = FW_WARNING_NOT_HANDLED;
      hw_logger.whm_hwlogger_cfg.whm_hw_logger_enable = 1;
      wave_core_cfg_set_whm_hw_logger_cfg(nic, &hw_logger, sizeof(hw_logger));
      return MTLK_ERR_BUSY;
    case WHM_STATE_DISABLE:
      ELOG_D("WHM is disabled-Can't handle FW warning-ID: %d",whm_fw_detection_ind->whmWarningId);
      wave_hw_whm_lock_release(hw);
      return MTLK_ERR_BUSY;
    default:
      ELOG_D("Invalid WHM State-Can't handle FW warning-ID: %d",whm_fw_detection_ind->whmWarningId);
      wave_hw_whm_lock_release(hw);
      return MTLK_ERR_NOT_SUPPORTED;
  }
  /* Send NL event to WHM Daemon */
  res = _wave_whm_nl_send_msg(LTQ_NL80211_VENDOR_EVENT_WHM, wdev,
                              whm_fw_detection_ind->whmWarningId,
                              whm_fw_detection_ind->whmRaisedByLayer);

  if (res != MTLK_ERR_OK) {
    ELOG_D("Error in sending WHM event msg to whm daemon for warningID : %d",whm_fw_detection_ind->whmWarningId);
    return MTLK_ERR_BUSY;
  }
  return MTLK_ERR_OK;
}

mtlk_error_t wave_core_recover_whm (mtlk_core_t *core)
{
  wave_whm_cfg_t whm_rev_cfg;
  wave_radio_t  *radio;

  MTLK_ASSERT(core);
  radio = wave_vap_radio_get(core->vap_handle);

  whm_rev_cfg.whm_cfg.whm_enable = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_WHM_ENABLE);
  whm_rev_cfg.whm_cfg.whm_fw_warn = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_WHM_FW_WARN);
  whm_rev_cfg.whm_cfg.whm_phy_warn = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_WHM_PHY_WARN);

  if (MTLK_PARAM_DB_VALUE_IS_INVALID(whm_rev_cfg.whm_cfg.whm_enable) ||
      MTLK_PARAM_DB_VALUE_IS_INVALID(whm_rev_cfg.whm_cfg.whm_fw_warn) ||
      MTLK_PARAM_DB_VALUE_IS_INVALID(whm_rev_cfg.whm_cfg.whm_phy_warn)) {
      return MTLK_ERR_OK;
  }
  return wave_core_cfg_send_whm_cfg(core, whm_rev_cfg.whm_cfg.whm_enable,
                                    whm_rev_cfg.whm_cfg.whm_fw_warn, whm_rev_cfg.whm_cfg.whm_phy_warn);
}

mtlk_error_t __MTLK_IFUNC
wave_core_warning_detection_ind_handle (mtlk_core_t *core, whm_warning_id warning_id, whm_warning_layer warning_layer)
{
  struct nic *nic = mtlk_vap_manager_get_master_core(mtlk_vap_get_manager(core->vap_handle));
  mtlk_hw_t *hw = mtlk_vap_get_hw(nic->vap_handle);
  uint8 res, whm_state;
  wave_whm_hwlogger_cfg_t hw_logger;

  /* check WHM cfg */
  if (!(wave_hw_get_whm_enable(hw) && wave_hw_get_whm_driver_warn(hw))) {
    ELOG_D("WHM Driver Warning is disable - Can't raise a warning - %d", warning_id);
    return MTLK_ERR_NOT_SUPPORTED;
  }

  /* check whm state machine */
  wave_hw_whm_lock_acquire(hw);
  whm_state = wave_hw_get_whm_state_machine(hw);

  switch (whm_state)
  {
    case WHM_STATE_READY:
      /* Handle warning */
      wave_hw_set_whm_state_machine(hw, WHM_STATE_BUSY);
      wave_hw_whm_lock_release(hw);

      /* Disable HW logger */
      hw_logger.whm_hwlogger_cfg.warn_handled = DONT_CARE;
      hw_logger.whm_hwlogger_cfg.whm_hw_logger_enable = 0;
      wave_core_cfg_set_whm_hw_logger_cfg (nic, &hw_logger, sizeof(hw_logger));
      /* collect fw logs to local buffer */
      res = wave_hw_copy_fw_trace_buffers(hw);
      if (res != MTLK_ERR_OK) {
        ELOG_V("Error in copying WHM FW Logs");
        wave_hw_set_whm_state_machine(hw, WHM_STATE_READY);
        return res;
      }
      /* Enable WHM in FW after FW logs are collected */
      hw_logger.whm_hwlogger_cfg.warn_handled = DONT_CARE;
      hw_logger.whm_hwlogger_cfg.whm_hw_logger_enable = 1;
      wave_core_cfg_set_whm_hw_logger_cfg (nic, &hw_logger, sizeof(hw_logger));
      break;
    case WHM_STATE_BUSY:
      ELOG_DD("WHM is BUSY - Can't handle warning %d from %d", warning_id, warning_layer);
      wave_hw_whm_lock_release(hw);
      return MTLK_ERR_BUSY;
    case WHM_STATE_DISABLE:
      ELOG_DD("WHM is disabled - Can't handle warning %d from %d", warning_id, warning_layer);
      wave_hw_whm_lock_release(hw);
      return MTLK_ERR_BUSY;
    default:
      ELOG_V("Invalid WHM State");
      wave_hw_whm_lock_release(hw);
      return MTLK_ERR_NOT_SUPPORTED;
  }
  return MTLK_ERR_OK;
}

static mtlk_error_t
_wave_core_driver_warning_detection_ind_handle (whm_warning_id warning_id, whm_warning_layer warning_layer)
{
  mtlk_error_t res = MTLK_ERR_OK;
  mtlk_hw_t *hw;
  mtlk_df_t *master_df;
  mtlk_core_t *core;
  uint32 nof_cards, i;
  struct wireless_dev *wdev;

  ILOG0_DD("WHM : Warning Raised : ID : %d : Layer : %d", warning_id, warning_layer);

  nof_cards = wave_hw_mmb_get_current_card_idx();

  for (i = 0; i < nof_cards; i++) {
    hw = wave_hw_mmb_get_current_card(i);
    if (hw != NULL) {

      master_df = wave_hw_df_get(hw, 0);
      if (NULL == master_df) {
        ELOG_DD("Error Getting MasterVap warning ID : %d : card : %d not handled", warning_id, mtlk_hw_mmb_get_card_idx(hw));
        _wave_core_set_whm_reset();
        return MTLK_ERR_NOT_IN_USE;
      }

      core = mtlk_vap_get_core(mtlk_df_get_vap_handle(master_df));
      res = wave_core_warning_detection_ind_handle(core, warning_id, warning_layer);
      if (res != MTLK_ERR_OK) {
        ELOG_DDD("Error %d in handling warning ID : %d : card : %d", res, warning_id, mtlk_hw_mmb_get_card_idx(hw));
        _wave_core_set_whm_reset();
        return res;
      }
    }
    else {
      ELOG_D("Error in getting hw for card_idx : %d", i);
      _wave_core_set_whm_reset();
      return MTLK_ERR_PARAMS;
    }
  }

  /* Send NL event to WHM Daemon */
  wdev = mtlk_df_user_get_wdev(mtlk_df_get_user(master_df));
  res = _wave_whm_nl_send_msg(LTQ_NL80211_VENDOR_EVENT_WHM, wdev,
                              warning_id, warning_layer);
  if (res != MTLK_ERR_OK) {
    ELOG_DD("Error in sending WHM NL event to whm daemon for warning %d from %d", warning_id, warning_layer);
    return MTLK_ERR_BUSY;
  }
  return res;
}

mtlk_error_t __MTLK_IFUNC
wave_core_driver_warning_detection_ind_handle (whm_warning_id warning_id)
{
  return _wave_core_driver_warning_detection_ind_handle(warning_id, WHM_DRIVER_TRIGGER);
}

mtlk_error_t __MTLK_IFUNC
wave_core_set_whm_trigger (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
    mtlk_error_t res = MTLK_ERR_OK;
    mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
    wave_whm_trigger_t *whm_trigger_cfg = NULL;
    uint32 whm_trigger_size;

    MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

    whm_trigger_cfg = mtlk_clpb_enum_get_next(clpb, &whm_trigger_size);
    MTLK_CLPB_TRY(whm_trigger_cfg, whm_trigger_size)
          res = _wave_core_driver_warning_detection_ind_handle(whm_trigger_cfg->whm_warning_id, whm_trigger_cfg->whm_warning_layer);
    MTLK_CLPB_FINALLY(res)
      return mtlk_clpb_push_res(clpb, res);
    MTLK_CLPB_END
}

__INLINE int
wave_core_add_blacklist_addr_entry (mtlk_core_t *nic, struct intel_vendor_blacklist_cfg *blacklist_cfg,
                                   ieee_addr_list_t *list, char *name)
{
  ieee_addr_entry_t *ieee_addr_entry = NULL;

  ieee_addr_entry = __mtlk_core_add_ieee_addr_entry_internal(nic, &blacklist_cfg->addr, list,
                        name, sizeof(blacklist_snr_info_t));

  if (ieee_addr_entry) {
    /* STA in blacklist.Update the params if any */
    ieee_addr_entry->data[0] = blacklist_cfg->snrProbeHWM;
    ieee_addr_entry->data[1] = blacklist_cfg->snrProbeLWM;
    return MTLK_ERR_OK;
  } else {
    return MTLK_ERR_NO_MEM;
  }
}

int
wave_core_set_softblocklist_entry (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *nic = HANDLE_T_PTR(mtlk_core_t, hcore);
  int res = MTLK_ERR_OK;
  struct intel_vendor_blacklist_cfg *softblocklist_cfg = NULL;
  uint32 softblocklist_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  softblocklist_cfg = mtlk_clpb_enum_get_next(clpb, &softblocklist_cfg_size);
  MTLK_CLPB_TRY(softblocklist_cfg, softblocklist_cfg_size)
  if (MTLK_CORE_PDB_GET_INT(nic, PARAM_DB_CORE_SOFTBLOCK_DISABLE)) {
    ELOG_V("softblock is disabled");
    MTLK_CLPB_EXIT(MTLK_ERR_PARAMS);
  }

  if (softblocklist_cfg->remove) {
    _mtlk_core_del_ieee_addr_entry(nic, &softblocklist_cfg->addr,
      &nic->multi_ap_blacklist, "multi-AP black", FALSE);
  } else {
    res = wave_core_add_blacklist_addr_entry(nic, softblocklist_cfg,
            &nic->multi_ap_blacklist, "multi-AP black");
  }
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int wave_core_set_blacklist_entry (mtlk_core_t *core,
                                           struct intel_vendor_blacklist_cfg *blacklist_cfg)
{
  int res = MTLK_ERR_OK;

  if (blacklist_cfg->remove) {
    if (!is_zero_ether_addr(blacklist_cfg->addr.au8Addr)) {
      _mtlk_core_del_ieee_addr_entry(core, &blacklist_cfg->addr,
                                     &core->widan_blacklist, "widan black", FALSE);
      _mtlk_core_del_ieee_addr_entry(core, &blacklist_cfg->addr,
                                     &core->multi_ap_blacklist, "multi-AP black", FALSE);
    } else {
      _mtlk_core_flush_ieee_addr_list(core, &core->widan_blacklist, "widan black");
      _mtlk_core_flush_ieee_addr_list(core, &core->multi_ap_blacklist, "multi-AP black");
    }
  } else {
    if (blacklist_cfg->status == 0) {
      _mtlk_core_del_ieee_addr_entry(core, &blacklist_cfg->addr,
                                     &core->multi_ap_blacklist, "multi-AP black", FALSE);
      res = _mtlk_core_add_ieee_addr_entry(core, &blacklist_cfg->addr,
                                           &core->widan_blacklist, "widan black");
    }
    else {
      _mtlk_core_del_ieee_addr_entry(core, &blacklist_cfg->addr,
                                     &core->widan_blacklist, "widan black", FALSE);
      res = wave_core_add_blacklist_addr_entry(core, blacklist_cfg,
                                                 &core->multi_ap_blacklist, "multi-AP black");
    }
  }

  return res;
}

int wave_core_clpb_set_blacklist_entry (mtlk_handle_t hcore, const void* data,
                                                uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  int res = MTLK_ERR_OK;
  struct intel_vendor_blacklist_cfg *blacklist_cfg = NULL;
  uint32 blacklist_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  /* get configuration */
  blacklist_cfg = mtlk_clpb_enum_get_next(clpb, &blacklist_cfg_size);
  MTLK_CLPB_TRY(blacklist_cfg, blacklist_cfg_size)
    res = wave_core_set_blacklist_entry(core, blacklist_cfg);
  MTLK_CLPB_FINALLY(res)
    /* push result into clipboard */
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

BOOL
wave_core_blacklist_frame_drop (mtlk_core_t *nic,
  const IEEE_ADDR *addr, unsigned subtype, int8 rx_snr_db, BOOL isbroadcast)
{
  struct intel_vendor_event_msg_drop prb_req_drop;
  ieee_addr_entry_t *entry;
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  blacklist_snr_info_t *blacklist_snr_info_p = NULL;
  blacklist_snr_info_t blacklist_snr_info = { 0 };

  /* drop all frames to STA in list without reject code */
  if (_mtlk_core_ieee_addr_entry_exists(nic, addr, &nic->widan_blacklist,
    "widan black")) /* Widanblacklist=1, MultiAP blacklist=0 */
    return TRUE;
#ifdef CPTCFG_IWLWAV_FILTER_BLACKLISTED_BSS
  if((subtype == MAN_TYPE_PROBE_RES || subtype == MAN_TYPE_BEACON) &&
    (_mtlk_core_ieee_addr_entry_exists(nic, addr, &nic->multi_ap_blacklist,"multi-AP black")))
      return TRUE;
#endif

  /* Check if STA is in multi AP blacklist */
  mtlk_osal_lock_acquire(&nic->multi_ap_blacklist.ieee_addr_lock);
  h = mtlk_hash_find_ieee_addr(&nic->multi_ap_blacklist.hash, addr);
  if (h) {
    entry = MTLK_CONTAINER_OF(h, ieee_addr_entry_t, hentry);
    blacklist_snr_info_p = (blacklist_snr_info_t *)&entry->data[0];
    wave_memcpy(&blacklist_snr_info, sizeof(blacklist_snr_info), blacklist_snr_info_p, sizeof(*blacklist_snr_info_p));
  }
  mtlk_osal_lock_release(&nic->multi_ap_blacklist.ieee_addr_lock);

  if (h) {
    if ((blacklist_snr_info.snrProbeHWM == 0) && (blacklist_snr_info.snrProbeLWM == 0)) {
      /* case when the client is not added by softblock */
      if (subtype == MAN_TYPE_PROBE_REQ)
        return TRUE;
      return FALSE;  /* report all auth, assoc requests and other */
    }
  }
  else {
    return FALSE;
  }

  /* W=0, A=1, Prb req or beacon */
  if (subtype == MAN_TYPE_PROBE_REQ) {
    /* In the path of unicast/bcast probe req and beacon. Check for softblock */
    if ((wave_core_mngmnt_softblock_notify(nic, addr, &nic->multi_ap_blacklist,
        "multi-AP black", rx_snr_db, isbroadcast, &prb_req_drop)) == DRV_SOFTBLOCK_ALLOW) {
      /* This is the case when the STA is in multi ap blacklist due to SoftBlock
          and thresholds allow to be processed */
      return FALSE;
    } else {
      return TRUE;
    }
  }
  /* report all auth, assoc requests and other */

  /* A=1, other msgs */
  return FALSE;
}

int __MTLK_IFUNC
wave_core_send_softblock_msg_drop (mtlk_vap_handle_t vap_handle, void *data, int data_len)
{
  mtlk_df_t *df               = mtlk_vap_get_df(vap_handle);
  mtlk_df_user_t *df_user     = mtlk_df_get_user(df);
  struct wireless_dev *wdev   = mtlk_df_user_get_wdev(df_user);
  mtlk_nbuf_t *evt_nbuf;
  uint8* cp;

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
                                            LTQ_NL80211_VENDOR_EVENT_SOFTBLOCK_DROP);
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

int __MTLK_IFUNC
wave_core_mngmnt_softblock_notify (mtlk_core_t *nic, const IEEE_ADDR *addr,
      ieee_addr_list_t *list, char *name, int8 rx_snr_db, BOOL isbroadcast,
      struct intel_vendor_event_msg_drop *prb_req_drop)
{
  MTLK_HASH_ENTRY_T(ieee_addr) *h;
  ieee_addr_entry_t *entry;
  int ret = DRV_SOFTBLOCK_ACCEPT;
  blacklist_snr_info_t *blacklist_snr_info_p = NULL;
  blacklist_snr_info_t blacklist_snr_info = { 0 };

  if (MTLK_CORE_PDB_GET_INT(nic, PARAM_DB_CORE_SOFTBLOCK_DISABLE))
    return ret;

  MTLK_UNREFERENCED_PARAM(name);
  ILOG3_DSY("CID-%04x: looking for %s SoftBlock entry %Y",
      mtlk_vap_get_oid(nic->vap_handle), name, addr->au8Addr);

  mtlk_osal_lock_acquire(&list->ieee_addr_lock);
  h = mtlk_hash_find_ieee_addr(&list->hash, addr); /* finding the mac addr only from the list in address list */

  if (h) {
    sta_entry *sta = NULL;
    sta = mtlk_stadb_find_sta(&nic->slow_ctx->stadb, addr->au8Addr);
    if (sta) {
      mtlk_osal_lock_release(&list->ieee_addr_lock);
      mtlk_sta_decref(sta);
      ILOG1_D("CID-%04x: mgmt::Probe Req Softblock allowed - sta connected",
          mtlk_vap_get_oid(nic->vap_handle));
      return ret;
    }
    entry = MTLK_CONTAINER_OF(h, ieee_addr_entry_t, hentry);
    ret = DRV_MULTI_AP_BLACKLIST_FOUND;
    blacklist_snr_info_p = (blacklist_snr_info_t *)&entry->data[0];
    wave_memcpy(&blacklist_snr_info, sizeof(blacklist_snr_info), blacklist_snr_info_p, sizeof(*blacklist_snr_info_p));
  }
  mtlk_osal_lock_release(&list->ieee_addr_lock);

  if (h) {

    if ((blacklist_snr_info.snrProbeHWM != 0) && (blacklist_snr_info.snrProbeLWM != 0)) {
      /* The case when the notification to be sent to hostap since its configured */
      prb_req_drop->rx_snr = rx_snr_db;
      prb_req_drop->broadcast = isbroadcast;
      prb_req_drop->msgtype = MAN_TYPE_PROBE_REQ;
      prb_req_drop->reason = 0;
      prb_req_drop->rejected = 0;
      prb_req_drop->addr = *addr;
      MTLK_CORE_PDB_GET_MAC(nic, PARAM_DB_CORE_MAC_ADDR, &prb_req_drop->bssid);

      if ((blacklist_snr_info.snrProbeHWM < rx_snr_db) ||
             (blacklist_snr_info.snrProbeLWM > rx_snr_db)) {
        /* Silently Ignore the message */
        ILOG1_DD("CID-%04x: mgmt::Probe Req Softblock dropped SNR: %d",
              mtlk_vap_get_oid(nic->vap_handle), rx_snr_db);
        /* Notify hostap its dropped */
        ret = DRV_SOFTBLOCK_DROP;
        prb_req_drop->blocked = TRUE;
        /* Trigger notification */
        wave_core_send_softblock_msg_drop(nic->vap_handle, prb_req_drop, sizeof(*prb_req_drop));
      }
      else {
        ILOG1_DD("CID-%04x: mgmt::Probe Req Softblock allowed SNR: %d",
              mtlk_vap_get_oid(nic->vap_handle), rx_snr_db);
        /* Notify hostap its allowed */
        prb_req_drop->blocked = FALSE;
        ret = DRV_SOFTBLOCK_ALLOW; /* Notification will be done when its sent after further checks */
      }
    }
    else {
        return ret;
    }
  }
  return ret;
}

int __MTLK_IFUNC
wave_core_set_recovery_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_rcvry_cfg_t *_rcvry_cfg = NULL;
  uint32 _rcvry_cfg_size;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **)data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  _rcvry_cfg = mtlk_clpb_enum_get_next(clpb, &_rcvry_cfg_size);
  MTLK_CLPB_TRY(_rcvry_cfg, _rcvry_cfg_size)
    MTLK_CFG_START_CHEK_ITEM_AND_CALL()
      MTLK_CFG_CHECK_ITEM_AND_CALL(_rcvry_cfg, recovery_cfg, wave_rcvry_cfg_set,
                                   (mtlk_vap_get_hw(core->vap_handle),
                                    &_rcvry_cfg->recovery_cfg), res);
    MTLK_CFG_END_CHEK_ITEM_AND_CALL()
  MTLK_CLPB_FINALLY(res)
    return mtlk_clpb_push_res(clpb, res);
  MTLK_CLPB_END
}

int __MTLK_IFUNC
wave_core_get_recovery_cfg (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  uint32 res = MTLK_ERR_OK;
  mtlk_rcvry_cfg_t _rcvry_cfg;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&_rcvry_cfg, 0, sizeof(_rcvry_cfg));

  MTLK_CFG_START_CHEK_ITEM_AND_CALL()
    MTLK_CFG_SET_ITEM_BY_FUNC(&_rcvry_cfg, recovery_cfg, wave_rcvry_cfg_get,
                              (mtlk_vap_get_hw(core->vap_handle),
                               &_rcvry_cfg.recovery_cfg), res);
  MTLK_CFG_END_CHEK_ITEM_AND_CALL()

  res = mtlk_clpb_push(clpb, &res, sizeof(res));
  if (MTLK_ERR_OK == res) {
    res = mtlk_clpb_push(clpb, &_rcvry_cfg, sizeof(_rcvry_cfg));
  }
  return res;
}

int __MTLK_IFUNC
wave_core_get_recovery_stats (mtlk_handle_t hcore, const void *data, uint32 data_size)
{
  int res = MTLK_ERR_OK;
  mtlk_rcvry_stats_t _rcvry_stats;
  mtlk_core_t *core = (mtlk_core_t*)hcore;
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;

  MTLK_ASSERT(sizeof(mtlk_clpb_t*) == data_size);

  memset(&_rcvry_stats, 0, sizeof(_rcvry_stats));

  MTLK_CFG_START_CHEK_ITEM_AND_CALL()
    MTLK_CFG_SET_ITEM_BY_FUNC_VOID(&_rcvry_stats, recovery_stats, wave_hw_get_recovery_stats,
                              (mtlk_vap_get_hw(core->vap_handle), &_rcvry_stats.recovery_stats));
  MTLK_CFG_END_CHEK_ITEM_AND_CALL()

  return mtlk_clpb_push_res_data(clpb, res, &_rcvry_stats, sizeof(_rcvry_stats));
}

mtlk_error_t __MTLK_IFUNC
wave_core_get_connection_alive (mtlk_handle_t hcore, const void* data, uint32 data_size)
{
  mtlk_core_t *core = HANDLE_T_PTR(mtlk_core_t, hcore);
  mtlk_clpb_t *clpb = *(mtlk_clpb_t **) data;
  wave_connection_alive_t conn_alive = {0};
  mtlk_mhi_stats_vap_t *mhi_vap_stats;
  wave_radio_t *radio;

  MTLK_ASSERT(core);

  radio = wave_vap_radio_get(core->vap_handle);

  mhi_vap_stats = &core->mhi_vap_stat;
  conn_alive.tx_packets = mhi_vap_stats->stats.txInUnicastHd;
  conn_alive.rx_packets = mhi_vap_stats->stats.rxOutUnicastHd;

  if (wave_radio_was_scan_done_since_prev_sta_poll(radio)) {
    conn_alive.probe_ap_after_scan = TRUE;
    wave_radio_set_scan_done_since_prev_sta_poll(radio, FALSE);
  }

  return mtlk_clpb_push(clpb, &conn_alive, sizeof(conn_alive));
}
