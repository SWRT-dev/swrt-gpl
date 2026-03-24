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
 * Power management functionality implementation in compliance with
 * Code of Conduct on Energy Consumption of Broadband Equipment (a.k.a CoC)
 */

#include "mtlkinc.h"
#include "mhi_umi.h"
#include "txmm.h"
#include "core.h"
#include "mtlk_coreui.h"
#include "mtlk_coc.h"
#include "hw_mmb.h"
#include "ta.h"
#include "vendor_cmds.h"
#include "core_priv.h"
#include "mtlk_df.h"
#include "mtlk_df_user_priv.h"
#include "mtlk_df_priv.h"
#include "core_config.h"

#define LOG_LOCAL_GID   GID_COC
#define LOG_LOCAL_FID   1

/**********************************************************************
 * Local definitions
***********************************************************************/
#define COC_TXNUM   MTLK_BFIELD_INFO(2, 2)
#define COC_RXNUM   MTLK_BFIELD_INFO(0, 2)

#define COC_MAKE_POWERMODE(txnum, rxnum) \
  (MTLK_BFIELD_VALUE(COC_TXNUM, txnum, uint8) | \
   MTLK_BFIELD_VALUE(COC_RXNUM, rxnum, uint8))

#define MTLK_COC_MSEC_IN_INTERVAL     (100)
typedef enum {
  COC_BW_SWITCH_DOWN = 1,
  COC_BW_SWITCH_UP
}_mtlk_coc_band_width_t;

typedef enum {
  COC_POWER_START_STATE = 0,
  COC_POWER_1x1_STATE,
  COC_POWER_2x2_STATE,
  COC_POWER_3x3_STATE,
  COC_POWER_4x4_STATE,
  COC_POWER_TRANSIENT_STATE,
  COC_POWER_STATE_LAST
} _mtlk_coc_power_state_t;

struct _mtlk_coc_t
{
  mtlk_vap_handle_t vap_handle;
  mtlk_handle_t ta_crit_handle;
  mtlk_txmm_t *txmm;
  uint8                   hw_max_antenna_mask; /* TX/RX */
  uint8                   hw_tx_antenna_mask;
  uint8                   hw_rx_antenna_mask;
  mtlk_coc_ant_mask_cfg_t default_ant_mask_cfg;
  mtlk_coc_ant_mask_cfg_t current_ant_mask_cfg;
  mtlk_coc_antenna_cfg_t  hw_antenna_cfg;
  mtlk_coc_antenna_cfg_t  current_antenna_cfg;
  mtlk_coc_antenna_cfg_t  manual_antenna_cfg;
  mtlk_coc_auto_cfg_t     auto_antenna_cfg;
  mtlk_coc_bw_t           bw_params;
  BOOL is_auto_mode;
  BOOL auto_mode_cfg;
  uint8 current_state;
  mtlk_osal_spinlock_t lock;
  BOOL trans_flag;
  uint8 pending_state;
  MTLK_DECLARE_INIT_STATUS;
};

struct _mtlk_erp_t
{
  mtlk_vap_handle_t      vap_handle;
  mtlk_handle_t          ta_crit_handle;
  mtlk_txmm_t           *txmm;
  uint8                  current_state;     /* TA crit state */
  uint32                 initial_wait_time; /* seconds */
  uint32                 radio_off_time;    /* milliseconds */
  uint32                 radio_on_time;     /* milliseconds */
  uint32                 cfg_erp_enabled;       /* 0 - dsiabled, 1 - enabled */
  uint32                 cur_erp_enabled;       /* 0 - dsiabled, 1 - enabled */
  uint32                 tx_20_max_tp;      /* max tp for 20 Mhz bw */
  uint32                 tx_40_max_tp;      /* max tp for 40 Mhz bw */
  uint32                 tx_80_max_tp;      /* max tp for 80 Mhz bw */
  uint32                 tx_160_max_tp;     /* max tp for 160 Mhz bw */
  uint32                 rx_tp_max;         /* max tp for rx */
  uint32                 max_num_sta;       /* max connected sta */
  mtlk_osal_spinlock_t   lock;
  MTLK_DECLARE_INIT_STATUS;
};

static const mtlk_ability_id_t _coc_abilities[] = {
  WAVE_RADIO_REQ_GET_COC_CFG,
  WAVE_RADIO_REQ_SET_COC_CFG
};

static const mtlk_ability_id_t _erp_abilities[] = {
  WAVE_RADIO_REQ_GET_ERP_CFG,
  WAVE_RADIO_REQ_SET_ERP_CFG
};

#define MAX_NUM_ANTENNAS        MAX_NUM_TX_ANTENNAS
#define DEFAULT_ANTENNA_MASK    ((1 << MAX_NUM_ANTENNAS) - 1)

static uint8 _coc_get_max_state_by_ant_num_table[MAX_NUM_ANTENNAS + 1] = {
  COC_POWER_START_STATE,
  COC_POWER_1x1_STATE,
  COC_POWER_2x2_STATE,
  COC_POWER_3x3_STATE,
  COC_POWER_4x4_STATE
};

static uint8
_wave_coc_get_ant_mask_cfg (mtlk_coc_t *coc_obj, unsigned nof_ants)
{
  unsigned idx = nof_ants - 1;
  if (idx < ARRAY_SIZE(coc_obj->current_ant_mask_cfg.mask)) {
    return coc_obj->current_ant_mask_cfg.mask[idx];
  } else {
    ELOG_D("Wrong number of antennas (%u)", nof_ants);
    return 0;
  }
}

static inline _mtlk_coc_power_state_t
_mtlk_coc_get_max_state_by_ant_num(unsigned ant_num)
{
    MTLK_ASSERT(ant_num < ARRAY_SIZE(_coc_get_max_state_by_ant_num_table));
    return _coc_get_max_state_by_ant_num_table[ant_num];
}


static int
_mtlk_coc_switch_mode (mtlk_coc_t *coc_obj, uint8 num_tx_antennas, uint8 num_rx_antennas)
{
  mtlk_txmm_msg_t         man_msg;
  mtlk_txmm_data_t*       man_entry = NULL;
  UMI_SET_ANTENNAS       *mac_msg;
  int                     res = MTLK_ERR_OK;
  uint32                  cur_rx_mask;
  mtlk_core_t            *core = mtlk_vap_get_core(coc_obj->vap_handle);
  wave_radio_t           *radio = wave_vap_radio_get(coc_obj->vap_handle);

  if (wave_radio_get_is_zwdfs_radio(radio) &&
      !wave_radio_get_zwdfs_ant_enabled(radio)) {
    ILOG1_V("Ignoring CoC switches for ZWDFS Radio: Antenna mask not active yet");
    return MTLK_ERR_OK;
  }

  res = mtlk_core_radio_enable_if_needed(core);
  if (MTLK_ERR_OK != res) {
    return res;
  }

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, coc_obj->txmm, NULL);

  if (NULL == man_entry)
  {
    res = MTLK_ERR_NO_RESOURCES;
  }
  else
  {
    man_entry->id           = MC_MAN_SET_ANTENNAS_REQ;
    man_entry->payload_size = sizeof(UMI_SET_ANTENNAS);
    mac_msg = (UMI_SET_ANTENNAS *)man_entry->payload;

    memset(mac_msg, 0, sizeof(*mac_msg));

    mac_msg->TxAntsMask = _wave_coc_get_ant_mask_cfg(coc_obj, num_tx_antennas);
    ILOG1_DDD("switch TX antennas: Full mask %02X, num antennas %d, resulting mask %02X",
              coc_obj->hw_tx_antenna_mask, num_tx_antennas, mac_msg->TxAntsMask);

    cur_rx_mask = _wave_coc_get_ant_mask_cfg(coc_obj, num_rx_antennas);
    mac_msg->RxAntsMask = cur_rx_mask;
    ILOG2_DDD("switch RX antennas: Full mask %02X, num antennas %d, resulting mask %02X",
              coc_obj->hw_rx_antenna_mask, num_rx_antennas, mac_msg->RxAntsMask);

    mtlk_dump(2, mac_msg, sizeof(*mac_msg), "dump of UMI_SET_ANTENNAS:");
    res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

    if (MTLK_ERR_OK == res && UMI_OK == mac_msg->status) {
      wave_radio_current_antenna_mask_set(wave_vap_radio_get(core->vap_handle), cur_rx_mask);
    } else {
      ELOG_DDDD("UMI_MAN_SET_ANTENNAS (TX%dxRX%d failed, res=%d status=%hhu",
               num_tx_antennas, num_rx_antennas, res, mac_msg->status);
      if (UMI_OK != mac_msg->status)
        res = MTLK_ERR_MAC;
    }

    mtlk_txmm_msg_cleanup(&man_msg);

  }

  (void)mtlk_core_radio_disable_if_needed(core);

  return res;
}

static _mtlk_coc_power_state_t
_mtlk_coc_get_current_state (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  return coc_obj->current_state;
}

static void
_mtlk_coc_set_state (mtlk_coc_t *coc_obj, const _mtlk_coc_power_state_t power_state)
{
  MTLK_ASSERT(NULL != coc_obj);

  coc_obj->current_state = power_state;
}

static uint32
_mtlk_coc_get_interval_by_state (mtlk_coc_t *coc_obj, const _mtlk_coc_power_state_t power_state)
{
  uint32 coc_interval = 0;

  switch (power_state) {
    case COC_POWER_START_STATE: coc_interval = coc_obj->auto_antenna_cfg.starting_time; break;
    case COC_POWER_1x1_STATE:   coc_interval = coc_obj->auto_antenna_cfg.interval_1x1; break;
    case COC_POWER_2x2_STATE:   coc_interval = coc_obj->auto_antenna_cfg.interval_2x2; break;
    case COC_POWER_3x3_STATE:   coc_interval = coc_obj->auto_antenna_cfg.interval_3x3; break;
    case COC_POWER_4x4_STATE:   coc_interval = coc_obj->auto_antenna_cfg.interval_4x4; break;
    case COC_POWER_TRANSIENT_STATE:   coc_interval = coc_obj->auto_antenna_cfg.transient_time; break;
    default: MTLK_ASSERT(FALSE);
  }

  if (coc_interval == 0)
    return 100; /* 10 seconds. otherwise we will enter a busy wait loop */

  return coc_interval;
}

static void
_mtlk_coc_get_antenna_by_state (mtlk_coc_t *coc_obj, const _mtlk_coc_power_state_t power_state, mtlk_coc_antenna_cfg_t *antenna_cfg)
{
  switch (power_state) {
    case COC_POWER_1x1_STATE:
      antenna_cfg->num_rx_antennas = 1;
      antenna_cfg->num_tx_antennas = 1;
      break;
    case COC_POWER_2x2_STATE:
      antenna_cfg->num_rx_antennas = 2;
      antenna_cfg->num_tx_antennas = 2;
      if ((antenna_cfg->num_rx_antennas > coc_obj->hw_antenna_cfg.num_rx_antennas) ||
          (antenna_cfg->num_tx_antennas > coc_obj->hw_antenna_cfg.num_tx_antennas)) {
        antenna_cfg->num_rx_antennas = 1;
        antenna_cfg->num_tx_antennas = 1;
      }
      break;
    case COC_POWER_3x3_STATE:
      antenna_cfg->num_rx_antennas = 3;
      antenna_cfg->num_tx_antennas = 3;
      if ((antenna_cfg->num_rx_antennas > coc_obj->hw_antenna_cfg.num_rx_antennas) ||
          (antenna_cfg->num_tx_antennas > coc_obj->hw_antenna_cfg.num_tx_antennas)) {
        *antenna_cfg = coc_obj->hw_antenna_cfg;
      }
      break;
    case COC_POWER_4x4_STATE:
      *antenna_cfg = coc_obj->hw_antenna_cfg;
      break;
    default: MTLK_ASSERT(FALSE);
  }
}

static void
_wave_coc_fill_ant_mask_cfg (mtlk_coc_ant_mask_cfg_t *cfg, uint8 full_mask, uint8 nof_ants);

int __MTLK_IFUNC
mtlk_coc_update_antenna_cfg (mtlk_coc_t *coc_obj, uint8 and_mask, BOOL update_coc_masks, BOOL *update_hw)
{
  uint8 cur_ant_count = count_bits_set(and_mask);

  if ((!cur_ant_count && !wave_radio_get_is_zwdfs_radio(wave_vap_radio_get(coc_obj->vap_handle)))
      || cur_ant_count >= COC_POWER_STATE_LAST) {
    ELOG_D("Unsupported antenna count %hhu", cur_ant_count);
    return MTLK_ERR_PARAMS;
  }

  mtlk_osal_lock_acquire(&coc_obj->lock);

  if ((coc_obj->current_antenna_cfg.num_tx_antennas != cur_ant_count) ||
      (coc_obj->current_antenna_cfg.num_rx_antennas != cur_ant_count)) {
    ILOG1_DDDDD("CID-%04x: CoC power mode changed from TX%dxRX%d to TX%dxRX%d",
                mtlk_vap_get_oid(coc_obj->vap_handle),
                coc_obj->current_antenna_cfg.num_tx_antennas,
                coc_obj->current_antenna_cfg.num_rx_antennas,
                cur_ant_count,
                cur_ant_count);
  }

  /* Nothing changed, no need to update, also HW */
  if (coc_obj->hw_rx_antenna_mask == and_mask) {
    mtlk_osal_lock_release(&coc_obj->lock);
    *update_hw = FALSE;
    return MTLK_ERR_OK;
  }

  ILOG1_DDD("CID-%04x: HW Antenna mask changed from 0x%02x to 0x%02x",
            mtlk_vap_get_oid(coc_obj->vap_handle), coc_obj->hw_rx_antenna_mask, and_mask);

  coc_obj->hw_tx_antenna_mask = and_mask;
  coc_obj->hw_rx_antenna_mask = and_mask;
  coc_obj->hw_antenna_cfg.num_tx_antennas = cur_ant_count;
  coc_obj->hw_antenna_cfg.num_rx_antennas = cur_ant_count;

  ILOG2_DD("Setting manual antenna config Tx:%d Rx%d\n", coc_obj->manual_antenna_cfg.num_tx_antennas,
           coc_obj->manual_antenna_cfg.num_rx_antennas);
  coc_obj->manual_antenna_cfg.num_tx_antennas = cur_ant_count;
  coc_obj->manual_antenna_cfg.num_rx_antennas = cur_ant_count;

  coc_obj->current_state = cur_ant_count;
  coc_obj->current_antenna_cfg.num_tx_antennas = cur_ant_count;
  coc_obj->current_antenna_cfg.num_rx_antennas = cur_ant_count;

  if (update_coc_masks) {
    /* TODO-ZWDFS: "update_coc_masks" might not be necessary here */
    _wave_coc_fill_ant_mask_cfg(&coc_obj->default_ant_mask_cfg, and_mask, cur_ant_count);
    coc_obj->current_ant_mask_cfg = coc_obj->default_ant_mask_cfg; /* the same */
    _mtlk_coc_set_state(coc_obj, COC_POWER_START_STATE);
  }

  mtlk_osal_lock_release(&coc_obj->lock);

  *update_hw = TRUE;
  return MTLK_ERR_OK;
}

static int
_mtlk_coc_set_antennas (mtlk_coc_t *coc_obj, const mtlk_coc_antenna_cfg_t *antenna_cfg)
{
  int res = MTLK_ERR_OK;
  
  MTLK_ASSERT(NULL != coc_obj);
  MTLK_ASSERT(0 != mtlk_vap_manager_get_active_vaps_number(mtlk_vap_get_manager(coc_obj->vap_handle)));

  /* Temporary always send antenna config to FW (called after each SET_CHAN),
   * because FW currently has issue that it discards current antenna config
   * after channel switch. TODO: Revert once FW provides a fix */
#if 0
  if ((antenna_cfg->num_rx_antennas == coc_obj->current_antenna_cfg.num_rx_antennas) &&
      (antenna_cfg->num_tx_antennas == coc_obj->current_antenna_cfg.num_tx_antennas)) {
    return MTLK_ERR_OK;
  }
#endif
  if (mtlk_core_is_block_tx_mode(mtlk_vap_get_core(coc_obj->vap_handle))) {
    return MTLK_ERR_OK;
  }

  if (!mtlk_core_is_in_oper_mode(mtlk_vap_get_core(coc_obj->vap_handle))) {
    return MTLK_ERR_OK;
  }

  if ((antenna_cfg->num_tx_antennas > coc_obj->hw_antenna_cfg.num_tx_antennas) ||
      (antenna_cfg->num_rx_antennas > coc_obj->hw_antenna_cfg.num_rx_antennas)) {
      ELOG_DDD("CID-%04x: Wrong CoC power mode TX%dxRX%d",
               mtlk_vap_get_oid(coc_obj->vap_handle),
               antenna_cfg->num_tx_antennas, antenna_cfg->num_rx_antennas);
      return MTLK_ERR_PARAMS;
  }

  /* lock should be set outside this function */
  mtlk_osal_lock_release(&coc_obj->lock);
  res = _mtlk_coc_switch_mode(coc_obj,
                              antenna_cfg->num_tx_antennas,
                              antenna_cfg->num_rx_antennas);
  /* lock should be unset outside this function */
  mtlk_osal_lock_acquire(&coc_obj->lock);

  if (MTLK_ERR_OK == res) {
    ILOG1_DDDDD("CID-%04x: CoC power mode changed from TX%dxRX%d to TX%dxRX%d",
                mtlk_vap_get_oid(coc_obj->vap_handle),
                coc_obj->current_antenna_cfg.num_tx_antennas,
                coc_obj->current_antenna_cfg.num_rx_antennas,
                antenna_cfg->num_tx_antennas,
                antenna_cfg->num_rx_antennas);

    /* update current antenna set */
    coc_obj->current_antenna_cfg = *antenna_cfg;
  }

  return res;
}

static int
_mtlk_coc_set_interval (mtlk_coc_t *coc_obj, const _mtlk_coc_power_state_t power_state)
{
  int res = MTLK_ERR_OK;
  ta_crit_coc_cfg_t ta_crit_cfg;

  MTLK_ASSERT(NULL != coc_obj);

  /* select interval according to state */
  ta_crit_cfg.interval = _mtlk_coc_get_interval_by_state(coc_obj, power_state);
  ta_crit_cfg.antennas = mtlk_coc_get_current_tx_antennas(coc_obj);
  res = mtlk_ta_coc_cfg(mtlk_vap_get_ta(coc_obj->vap_handle), &ta_crit_cfg);

  if (MTLK_ERR_OK != res) {
    ELOG_DD("CID-%04x: Wrong CoC auto configuration: interval %d doesn't fit for TA",
            mtlk_vap_get_oid(coc_obj->vap_handle), ta_crit_cfg.interval);
  }

  return res;
}

static int
_mtlk_coc_auto_mode_process (mtlk_coc_t *coc_obj, mtlk_coc_crit_t *coc_crit)
{
  int res = MTLK_ERR_OK;
  _mtlk_coc_power_state_t current_state, new_state;
  mtlk_coc_antenna_cfg_t antenna_cfg;
  unsigned max_antennas, crit_max_ants;
  unsigned nof_antennas;
  unsigned nof_ants_tp = 1;
  unsigned nof_ants_rssi = 0;
  uint32   max_tp;
  int      rssi;
  struct intel_vendor_event_coc  bw_params;
  struct wireless_dev *wdev;
  mtlk_core_t *core = mtlk_vap_get_core(coc_obj->vap_handle);

  MTLK_ASSERT(NULL != coc_obj);
  MTLK_ASSERT(NULL != coc_crit);
  MTLK_ASSERT(NULL != core);

  res = mtlk_core_radio_enable_if_needed(core);
  if (MTLK_ERR_OK != res) {
    return res;
  }

  /* 1. TP processing */
  max_antennas  = coc_obj->hw_antenna_cfg.num_tx_antennas;
  crit_max_ants = coc_crit->max_ants;
  max_tp        = coc_crit->max_tp;
  coc_obj->trans_flag = 0;
  mtlk_osal_lock_acquire(&coc_obj->lock);

  current_state = _mtlk_coc_get_current_state(coc_obj);
  new_state = current_state;

  /* Only next/previous states are available */
  switch (current_state) {
    case COC_POWER_START_STATE:
      nof_antennas = max_antennas;
      ILOG0_D("Start state -> %u Antennas", nof_antennas);
      goto set_state;
    case COC_POWER_1x1_STATE:
      nof_ants_tp = 1;
      if ((coc_obj->auto_antenna_cfg.interval_2x2) && (max_antennas >= 2) && (max_tp >= coc_obj->auto_antenna_cfg.low_limit_2x2)) {
        nof_ants_tp = 2;
      }
      else if((max_antennas == 4) && (max_tp >= coc_obj->auto_antenna_cfg.low_limit_4x4)){
        nof_ants_tp = 4;
      }
      else if((max_antennas == 3) && (max_tp >= coc_obj->auto_antenna_cfg.low_limit_3x3)){
        nof_ants_tp = 3;
      }
      break;
    case COC_POWER_2x2_STATE:
      if (coc_obj->auto_antenna_cfg.interval_2x2) {
        nof_ants_tp = 2;
        if (max_tp < coc_obj->auto_antenna_cfg.high_limit_1x1) {
          ILOG1_V(" COC_POWER_2x2_STATE 20Mhz \n");
          nof_ants_tp = 1;
          coc_obj->trans_flag = 1;
        }
        else if ((coc_obj->auto_antenna_cfg.interval_3x3) && (max_antennas >= 3) && (max_tp >= coc_obj->auto_antenna_cfg.low_limit_3x3)) {
          nof_ants_tp = 3;
        }
      }
      break;
    case COC_POWER_3x3_STATE:
        nof_ants_tp = 3;
        if ((max_antennas >= 4) && (max_tp >= coc_obj->auto_antenna_cfg.low_limit_4x4)) {
          nof_ants_tp = 4;
        }
        else if ((coc_obj->auto_antenna_cfg.interval_2x2) && (max_tp < coc_obj->auto_antenna_cfg.high_limit_2x2)) {
          nof_ants_tp = 2;
        }
        else if (max_tp < coc_obj->auto_antenna_cfg.low_limit_3x3) {
          ILOG1_V("COC_POWER_4x4_STATE 20Mhz \n");
          nof_ants_tp = 1;
          coc_obj->trans_flag = 1;
        }
      break;
    case COC_POWER_4x4_STATE:
      nof_ants_tp = 4;
      if ((coc_obj->auto_antenna_cfg.interval_3x3) && (max_tp < coc_obj->auto_antenna_cfg.high_limit_3x3)) {
        nof_ants_tp = 3;
      } else if (max_tp < coc_obj->auto_antenna_cfg.low_limit_4x4) {
        ILOG1_V("COC_POWER_4x4_STATE 20Mhz \n");
        nof_ants_tp = 1;
        coc_obj->trans_flag = 1;
      }
      break;
      /* Intermediate wait state between actual trasitions */
    case COC_POWER_TRANSIENT_STATE:
      coc_obj->trans_flag = 0;
      ILOG1_D("COC_POWER_TRANSIENT_STATE %d \n",coc_obj->pending_state);
      if (coc_obj->pending_state != max_antennas) {
        _mtlk_coc_get_antenna_by_state(coc_obj, coc_obj->pending_state, &antenna_cfg);
        res = _mtlk_coc_set_antennas(coc_obj, &antenna_cfg);
        if (MTLK_ERR_OK != res) {
          goto FINISH;
        }
      }
      res = _mtlk_coc_set_interval(coc_obj, coc_obj->pending_state);
      if (MTLK_ERR_OK != res) {
        goto FINISH;
      }

      _mtlk_coc_set_state(coc_obj, coc_obj->pending_state);
      goto FINISH;
      break;

    default:
      nof_ants_tp = 1;
      MTLK_ASSERT(FALSE);
  }

  /* 2. RSSI processing */
  rssi = coc_crit->rssi;
  switch (current_state) {
    case COC_POWER_1x1_STATE:
      nof_ants_rssi = 1;
      if ((coc_obj->auto_antenna_cfg.interval_2x2) && (max_antennas >= 2) && (rssi < coc_obj->auto_antenna_cfg.low_rssi_2x2)) {
        nof_ants_rssi = 2;
      }
      else if ((max_antennas == 4) && (rssi < coc_obj->auto_antenna_cfg.low_rssi_4x4)) {
        nof_ants_rssi = 4;
      }
      else if ((max_antennas == 3) && (rssi < coc_obj->auto_antenna_cfg.low_rssi_3x3)) {
        nof_ants_rssi = 3;
      }
      break;
    case COC_POWER_2x2_STATE:
      if (coc_obj->auto_antenna_cfg.interval_2x2) {
        nof_ants_rssi = 2;
        if (rssi > (coc_obj->auto_antenna_cfg.low_rssi_2x2 - coc_obj->auto_antenna_cfg.rssi_delta_down)) {
          nof_ants_rssi = 1;
        }
        else if ((coc_obj->auto_antenna_cfg.interval_3x3) && (max_antennas >= 3) && (rssi < coc_obj->auto_antenna_cfg.low_rssi_3x3)) {
          nof_ants_rssi = 3;
        }
      }
      break;
    case COC_POWER_3x3_STATE:
        nof_ants_rssi = 3;
        if ((max_antennas >= 4) && (rssi < coc_obj->auto_antenna_cfg.low_rssi_4x4)) {
          nof_ants_rssi = 4;
        }
        else if ((coc_obj->auto_antenna_cfg.interval_2x2) && (rssi > (coc_obj->auto_antenna_cfg.low_rssi_3x3 - coc_obj->auto_antenna_cfg.rssi_delta_down))) {
          nof_ants_rssi = 2;
        }
        else if (rssi > (coc_obj->auto_antenna_cfg.low_rssi_3x3 - coc_obj->auto_antenna_cfg.rssi_delta_down)) {
          nof_ants_rssi = 1;
        }
      break;
    case COC_POWER_4x4_STATE:
      nof_ants_rssi = 4;
      if ((coc_obj->auto_antenna_cfg.interval_3x3) && (rssi > (coc_obj->auto_antenna_cfg.low_rssi_4x4 - coc_obj->auto_antenna_cfg.rssi_delta_down))) {
        nof_ants_rssi = 3;
      } else if (rssi > (coc_obj->auto_antenna_cfg.low_rssi_4x4 - coc_obj->auto_antenna_cfg.rssi_delta_down)) {
        nof_ants_rssi = 1;
      }
      break;
     default:
       nof_ants_rssi = 1;
       MTLK_ASSERT(FALSE);
  }

  /* if RSSI is invalid skip the RSSI check */
  if (rssi > MIN_RSSI) {
    nof_antennas  = MAX(MIN(nof_ants_rssi, max_antennas), nof_ants_tp);
  } else {
    nof_antennas  = nof_ants_tp;
    ILOG1_DDDDDDDD(" Ignoring RSSI State %d: TP %u, RSSI %d -> Antennas ((TP %u, STA %u), (RSSI %u, HW %u)) -> %u",
      current_state, max_tp, rssi, nof_ants_tp, crit_max_ants, nof_ants_rssi, max_antennas, nof_antennas);
  }

  ILOG1_DDDDDDDD("State %d: TP %u, RSSI %d -> Antennas ((TP %u, STA %u), (RSSI %u, HW %u)) -> %u",
      current_state, max_tp, rssi, nof_ants_tp, crit_max_ants, nof_ants_rssi, max_antennas, nof_antennas);
set_state:
  switch (nof_antennas) {
    case 1: new_state = COC_POWER_1x1_STATE;  break;
    case 2: new_state = COC_POWER_2x2_STATE;  break;
    case 3: new_state = COC_POWER_3x3_STATE;  break;
    case 4: new_state = COC_POWER_4x4_STATE;  break;
    default: MTLK_ASSERT(FALSE);
  }

  if ((current_state != new_state) ) {
   if (current_state != COC_POWER_TRANSIENT_STATE) {

    wdev = mtlk_df_user_get_wdev(mtlk_df_get_user(mtlk_vap_get_df(core->vap_handle)));
    memset(&bw_params, 0, sizeof(bw_params));
    if ((coc_obj->trans_flag == 1) && (new_state == COC_POWER_1x1_STATE)) {
      /* indicate hostapd to move to 20 Mhz BW */
      if (coc_obj->auto_antenna_cfg.bandwidth_flag) {
        bw_params.coc_BW = COC_BW_SWITCH_UP;
      } else {
        bw_params.coc_BW = COC_BW_SWITCH_DOWN;
      }
      bw_params.coc_omn_IE = new_state;
      coc_obj->pending_state = new_state;
      ILOG1_V("COC nl event send  for 20Mhz \n");
      res = wv_ieee80211_vendor_event_coc(LTQ_NL80211_VENDOR_EVENT_COC_BEACON_UPDATE, wdev, &bw_params, sizeof(bw_params));
      if (MTLK_ERR_OK != res) {
        ELOG_V("COC nl event send failed");
      }
      ILOG1_D(" Moving to COC_POWER_TRANSIENT_STATE  from %d \n",coc_obj->current_state);
      _mtlk_coc_set_state(coc_obj, COC_POWER_TRANSIENT_STATE);
      goto FINISH;
    } else if ((new_state > COC_POWER_1x1_STATE) && (current_state != COC_POWER_START_STATE)) {
      /* Upto hostapd to decide which higher BW to set */
      bw_params.coc_BW = COC_BW_SWITCH_UP;
      bw_params.coc_omn_IE = new_state;
      if (new_state == max_antennas) {
        bw_params.coc_omn_IE = 0; /* While moving upwards,dont send OMN IE on the beacon. */
        /* while going up set antenna first, then update the BW */
        _mtlk_coc_get_antenna_by_state(coc_obj, new_state, &antenna_cfg);
        res = _mtlk_coc_set_antennas(coc_obj, &antenna_cfg);
        if (MTLK_ERR_OK != res) {
          goto FINISH;
        }
      }
      ILOG1_D("COC nl event send for Higher BandWidth with OMN IE = %d \n",bw_params.coc_omn_IE);
      res = wv_ieee80211_vendor_event_coc(LTQ_NL80211_VENDOR_EVENT_COC_BEACON_UPDATE, wdev, &bw_params, sizeof(bw_params));
      if (MTLK_ERR_OK != res) {
        ELOG_V("COC nl event send failed");
      }
      coc_obj->pending_state = new_state;
      ILOG0_D(" Moving to COC_POWER_TRANSIENT_STATE  from %d \n",coc_obj->current_state);
      _mtlk_coc_set_state(coc_obj, COC_POWER_TRANSIENT_STATE);
      goto FINISH;
    }
   }

    coc_obj->trans_flag = 0;
    coc_obj->pending_state = new_state;
    _mtlk_coc_get_antenna_by_state(coc_obj, new_state, &antenna_cfg);
    res = _mtlk_coc_set_antennas(coc_obj, &antenna_cfg);
    if (MTLK_ERR_OK != res) {
      goto FINISH;
    }

    res = _mtlk_coc_set_interval(coc_obj, new_state);
    if (MTLK_ERR_OK != res) {
      goto FINISH;
    }

    _mtlk_coc_set_state(coc_obj, new_state);
  }
FINISH:
  mtlk_osal_lock_release(&coc_obj->lock);
  return MTLK_ERR_OK;
}

static int
mtlk_core_send_erp_cfg (mtlk_erp_t *erp_obj)
{
  int                res;
  mtlk_core_t       *core = mtlk_vap_get_core(erp_obj->vap_handle);
  mtlk_txmm_msg_t    man_msg;
  mtlk_txmm_data_t  *man_entry = NULL;
  UMI_ERP_SET       *erp = NULL;
  mtlk_vap_handle_t  vap_handle = core->vap_handle;
  uint16             oid;
  int                net_state = mtlk_core_get_net_state(core);

  oid = mtlk_vap_get_oid(vap_handle);
  /* Allow only if the VAP has already been activated */
  if (!(net_state & (NET_STATE_ACTIVATING | NET_STATE_DEACTIVATING | NET_STATE_CONNECTED))) {
    ELOG_D("CID-%04x: VAP not activated yet", oid);
    return MTLK_ERR_NOT_READY;
  }

  res = mtlk_core_radio_enable_if_needed(core);
  if (MTLK_ERR_OK != res)
    return res;

  ILOG1_D("CID-%04x: Sending UM_MAN_ERP_SET_REQ", oid);
  ILOG1_D("                  isErpEnable:%d", erp_obj->cur_erp_enabled);
  ILOG1_D("          radioOffTimeInMsecs:%d", erp_obj->radio_off_time);
  ILOG1_D("          radioOnTimerInMsecs:%d", erp_obj->radio_on_time);
  ILOG1_D("     numberOfStationThreshold:%d", erp_obj->max_num_sta);
  ILOG1_D("     staConnectionTimeInSeconds:%d", erp_obj->initial_wait_time);

  man_entry = mtlk_txmm_msg_init_with_empty_data(&man_msg, mtlk_vap_get_txmm(vap_handle), NULL);

  if (NULL == man_entry) {
    ELOG_D("CID-%04x: No man entry available", oid);
    return MTLK_ERR_NO_RESOURCES;
  }

  man_entry->id = UM_MAN_ERP_SET_REQ;
  man_entry->payload_size = sizeof(UMI_ERP_SET);

  erp                          = (UMI_ERP_SET *)(man_entry->payload);
  erp->staConnectionTimeInSeconds = HOST_TO_MAC32(erp_obj->initial_wait_time);
  erp->radioOffTimeInMsecs     = HOST_TO_MAC32(erp_obj->radio_off_time);
  erp->radioOnTimeInMsecs      = HOST_TO_MAC32(erp_obj->radio_on_time);
  erp->isErpEnable             = erp_obj->cur_erp_enabled;
  erp->numberOfStationThreshold= HOST_TO_MAC16(erp_obj->max_num_sta);

  res = mtlk_txmm_msg_send_blocked(&man_msg, MTLK_MM_BLOCKED_SEND_TIMEOUT);

  if (MTLK_ERR_OK != res || UMI_OK != erp->Status) {
    ELOG_DDD("CID-%04x: Sending UM_MAN_ERP_SET_REQ failed, res=%d status=%hhu", oid, res, erp->Status);
    if (UMI_OK != erp->Status)
      res = MTLK_ERR_MAC;
  }

  mtlk_txmm_msg_cleanup(&man_msg);
  res = mtlk_core_radio_disable_if_needed(core);

  return res;
}

static int
_mtlk_erp_auto_mode_process (mtlk_erp_t *erp_obj, ta_crit_erp_result_t *erp_result)
{
  wave_radio_t   *radio = wave_vap_radio_get(erp_obj->vap_handle);
  uint32          bandwith;
  BOOL            erp_enable = TRUE;
  uint32          tx_threshold = 0;

  MTLK_ASSERT(NULL != erp_obj);
  MTLK_ASSERT(NULL != erp_result);

  mtlk_osal_lock_acquire(&erp_obj->lock);
  bandwith = WAVE_RADIO_PDB_GET_INT(radio, PARAM_DB_RADIO_SPECTRUM_MODE);
  ILOG3_HHDD("ERP process max_rx:%llu max_tx:%llu sta_num:%d bandwith:%d",
      erp_result->max_rx, erp_result->max_tx, erp_result->sta_num, bandwith);

  switch (bandwith) {
  case CW_20: tx_threshold = erp_obj->tx_20_max_tp; break;
  case CW_40: tx_threshold = erp_obj->tx_40_max_tp; break;
  case CW_80: tx_threshold = erp_obj->tx_80_max_tp; break;
  case CW_160:tx_threshold = erp_obj->tx_160_max_tp; break;
  case CW_80_80: tx_threshold = erp_obj->tx_160_max_tp; break;
  default: MTLK_ASSERT(0);
  }

  if (erp_result->max_tx >= tx_threshold)
    erp_enable = FALSE;

  if (erp_result->max_rx >= erp_obj->rx_tp_max )
    erp_enable = FALSE;

  if (erp_result->sta_num >= erp_obj->max_num_sta)
    erp_enable = FALSE;

  mtlk_osal_lock_release(&erp_obj->lock);

  ILOG3_DDD("ERP process max_rx_th:%d max_tx_th:%d sta_num_th:%d",
    erp_obj->rx_tp_max, tx_threshold, erp_obj->max_num_sta);

  ILOG1_DDD("ERP radio:%d current state:%d calculated:%d", wave_radio_id_get(radio), erp_obj->cur_erp_enabled, erp_enable);

  if (erp_enable != erp_obj->cur_erp_enabled) {
    erp_obj->cur_erp_enabled = erp_enable;
    mtlk_core_send_erp_cfg(erp_obj);
  }

  return MTLK_ERR_OK;
}


static void
_mtlk_coc_auto_power_callback (mtlk_handle_t clb_ctx, mtlk_handle_t clb_crit)
{
  mtlk_coc_t      *coc_obj;
  mtlk_coc_crit_t *coc_crit;

  coc_obj  = HANDLE_T_PTR(mtlk_coc_t, clb_ctx);
  coc_crit = HANDLE_T_PTR(mtlk_coc_crit_t, clb_crit);

  (void)_mtlk_coc_auto_mode_process(coc_obj, coc_crit);
}

static void
_mtlk_erp_auto_callback (mtlk_handle_t clb_ctx, mtlk_handle_t clb_crit)
{
  mtlk_erp_t *erp_obj;
  ta_crit_erp_result_t *erp_result;

  erp_obj = HANDLE_T_PTR(mtlk_erp_t, clb_ctx);

  MTLK_ASSERT(NULL != erp_obj);

  erp_result = (ta_crit_erp_result_t *)clb_crit;

  (void)_mtlk_erp_auto_mode_process(erp_obj, erp_result);
}

MTLK_INIT_STEPS_LIST_BEGIN(coc)
  MTLK_INIT_STEPS_LIST_ENTRY(coc, LOCK_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(coc, CONFIG_TA)
  MTLK_INIT_STEPS_LIST_ENTRY(coc, REG_CALLBACK)
  MTLK_INIT_STEPS_LIST_ENTRY(coc, REG_ABILITIES)
  MTLK_INIT_STEPS_LIST_ENTRY(coc, EN_ABILITIES)
MTLK_INIT_INNER_STEPS_BEGIN(coc)
MTLK_INIT_STEPS_LIST_END(coc);

MTLK_INIT_STEPS_LIST_BEGIN(erp)
  MTLK_INIT_STEPS_LIST_ENTRY(erp, LOCK_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(erp, CONFIG_TA)
  MTLK_INIT_STEPS_LIST_ENTRY(erp, REG_CALLBACK)
  MTLK_INIT_STEPS_LIST_ENTRY(erp, REG_ABILITIES)
  MTLK_INIT_STEPS_LIST_ENTRY(erp, EN_ABILITIES)
MTLK_INIT_INNER_STEPS_BEGIN(erp)
MTLK_INIT_STEPS_LIST_END(erp);

static void
_mtlk_coc_cleanup (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  MTLK_CLEANUP_BEGIN(coc, MTLK_OBJ_PTR(coc_obj))
    MTLK_CLEANUP_STEP(coc, EN_ABILITIES, MTLK_OBJ_PTR(coc_obj),
                      mtlk_abmgr_disable_ability_set,
                      (mtlk_vap_get_abmgr(coc_obj->vap_handle),
                      _coc_abilities, ARRAY_SIZE(_coc_abilities)));
    MTLK_CLEANUP_STEP(coc, REG_ABILITIES, MTLK_OBJ_PTR(coc_obj),
                      mtlk_abmgr_unregister_ability_set,
                      (mtlk_vap_get_abmgr(coc_obj->vap_handle),
                      _coc_abilities, ARRAY_SIZE(_coc_abilities)));
    MTLK_CLEANUP_STEP(coc, REG_CALLBACK, MTLK_OBJ_PTR(coc_obj),
                      mtlk_ta_crit_unregister, (coc_obj->ta_crit_handle));
    MTLK_CLEANUP_STEP(coc, CONFIG_TA, MTLK_OBJ_PTR(coc_obj),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(coc, LOCK_INIT, MTLK_OBJ_PTR(coc_obj),
                      mtlk_osal_lock_cleanup, (&coc_obj->lock));
  MTLK_CLEANUP_END(coc, MTLK_OBJ_PTR(coc_obj));
}

static void
_mtlk_erp_cleanup (mtlk_erp_t *erp_obj)
{
  MTLK_ASSERT(NULL != erp_obj);

  ILOG0_V("ERP CLEANUP");

  MTLK_CLEANUP_BEGIN(erp, MTLK_OBJ_PTR(erp_obj))
    MTLK_CLEANUP_STEP(erp, EN_ABILITIES, MTLK_OBJ_PTR(erp_obj),
                      mtlk_abmgr_disable_ability_set,
                      (mtlk_vap_get_abmgr(erp_obj->vap_handle),
                      _erp_abilities, ARRAY_SIZE(_erp_abilities)));
    MTLK_CLEANUP_STEP(erp, REG_ABILITIES, MTLK_OBJ_PTR(erp_obj),
                      mtlk_abmgr_unregister_ability_set,
                      (mtlk_vap_get_abmgr(erp_obj->vap_handle),
                      _erp_abilities, ARRAY_SIZE(_erp_abilities)));
    MTLK_CLEANUP_STEP(erp, REG_CALLBACK, MTLK_OBJ_PTR(erp_obj),
                      mtlk_ta_crit_unregister, (erp_obj->ta_crit_handle));
    MTLK_CLEANUP_STEP(erp, CONFIG_TA, MTLK_OBJ_PTR(erp_obj),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(erp, LOCK_INIT, MTLK_OBJ_PTR(erp_obj),
                      mtlk_osal_lock_cleanup, (&erp_obj->lock));
  MTLK_CLEANUP_END(erp, MTLK_OBJ_PTR(erp_obj));
}

static void
_wave_coc_fill_ant_mask_cfg (mtlk_coc_ant_mask_cfg_t *cfg, uint8 full_mask, uint8 nof_ants)
{
  size_t i;

  memset(cfg, 0, sizeof(*cfg));
  for (i = 0; i < MIN(nof_ants, ARRAY_SIZE(cfg->mask)); i++) {
    cfg->mask[i] = hw_get_antenna_mask(full_mask, i + 1 /* nof_antennas */);
  }
}

static int
_mtlk_coc_init (mtlk_coc_t *coc_obj, const mtlk_coc_cfg_t *cfg)
{
  ta_crit_coc_cfg_t ta_crit_cfg;
  uint8  num_ants, ant_mask;
  mtlk_error_t res;

  MTLK_ASSERT(NULL != coc_obj);
  MTLK_ASSERT(NULL != cfg->txmm);

  /* Initial state is maximal power mode allowed by hardware */
  coc_obj->vap_handle = cfg->vap_handle;
  coc_obj->txmm = cfg->txmm;

  num_ants = cfg->hw_antenna_cfg.num_tx_antennas;
  ant_mask = cfg->hw_ant_mask;
  coc_obj->hw_max_antenna_mask = ant_mask;
  _wave_coc_fill_ant_mask_cfg(&coc_obj->default_ant_mask_cfg, ant_mask, num_ants);
  coc_obj->current_ant_mask_cfg = coc_obj->default_ant_mask_cfg; /* the same */
  ILOG1_DDD("num_ants %u, full_mask 0x%X, masks 0x%08X",
            num_ants, ant_mask, coc_obj->default_ant_mask_cfg.word);

  coc_obj->hw_antenna_cfg = cfg->hw_antenna_cfg;
  coc_obj->manual_antenna_cfg = cfg->hw_antenna_cfg;
  coc_obj->auto_antenna_cfg = cfg->default_auto_cfg;

  coc_obj->current_antenna_cfg = cfg->hw_antenna_cfg;
  coc_obj->current_state = _mtlk_coc_get_max_state_by_ant_num(coc_obj->current_antenna_cfg.num_tx_antennas);
  ta_crit_cfg.interval = _mtlk_coc_get_interval_by_state(coc_obj, coc_obj->current_state);
  ILOG1_DDDD("TX_ant_num %d, mask 0x%X, initial_state %d, interval %d",
      coc_obj->current_antenna_cfg.num_tx_antennas, ant_mask, coc_obj->current_state, ta_crit_cfg.interval);

  MTLK_INIT_TRY(coc, MTLK_OBJ_PTR(coc_obj))
    MTLK_INIT_STEP(coc, LOCK_INIT, MTLK_OBJ_PTR(coc_obj),
                   mtlk_osal_lock_init, (&coc_obj->lock));
    MTLK_INIT_STEP_EX(coc, CONFIG_TA, MTLK_OBJ_PTR(coc_obj),
                      mtlk_ta_coc_cfg,
                      (mtlk_vap_get_ta(coc_obj->vap_handle), &ta_crit_cfg),
                      res,
                      res == MTLK_ERR_OK, MTLK_ERR_PARAMS);
    MTLK_INIT_STEP_EX(coc, REG_CALLBACK, MTLK_OBJ_PTR(coc_obj),
                      mtlk_ta_crit_register,
                      (mtlk_vap_get_ta(coc_obj->vap_handle),
                       TA_CRIT_ID_COC,
                       _mtlk_coc_auto_power_callback,
                       HANDLE_T(coc_obj)),
                      coc_obj->ta_crit_handle,
                      coc_obj->ta_crit_handle != MTLK_INVALID_HANDLE, MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(coc, REG_ABILITIES, MTLK_OBJ_PTR(coc_obj),
                   mtlk_abmgr_register_ability_set,
                   (mtlk_vap_get_abmgr(coc_obj->vap_handle),
                    _coc_abilities, ARRAY_SIZE(_coc_abilities)));
    MTLK_INIT_STEP_VOID(coc, EN_ABILITIES, MTLK_OBJ_PTR(coc_obj),
                        mtlk_abmgr_enable_ability_set,
                        (mtlk_vap_get_abmgr(coc_obj->vap_handle),
                         _coc_abilities, ARRAY_SIZE(_coc_abilities)));
  MTLK_INIT_FINALLY(coc, MTLK_OBJ_PTR(coc_obj))
  MTLK_INIT_RETURN(coc, MTLK_OBJ_PTR(coc_obj), _mtlk_coc_cleanup, (coc_obj));
}

static int
_mtlk_erp_init (mtlk_erp_t *erp_obj, const mtlk_coc_erp_cfg_t *cfg)
{
  ta_crit_erp_cfg_t ta_crit_cfg;
  uint32 res;

  MTLK_ASSERT(NULL != erp_obj);
  MTLK_ASSERT(NULL != cfg->txmm);

  ILOG0_V("ERP INIT");
  /* Initial state is maximal power mode allowed by hardware */
  erp_obj->vap_handle = cfg->vap_handle;
  erp_obj->txmm = cfg->txmm;

  ta_crit_cfg.interval = cfg->initial_wait_time * 10;
  erp_obj->initial_wait_time = cfg->initial_wait_time;
  erp_obj->max_num_sta = cfg->max_num_sta;
  erp_obj->radio_on_time = cfg->radio_on_time;
  erp_obj->radio_off_time = cfg->radio_off_time;
  erp_obj->rx_tp_max = cfg->rx_tp_max;
  erp_obj->tx_20_max_tp = cfg->tx_20_max_tp;
  erp_obj->tx_40_max_tp = cfg->tx_40_max_tp;
  erp_obj->tx_80_max_tp = cfg->tx_80_max_tp;
  erp_obj->tx_160_max_tp = cfg->tx_160_max_tp;
  erp_obj->cfg_erp_enabled = cfg->erp_enabled;
  erp_obj->cur_erp_enabled = 0;

  ILOG0_DD("initial_state %d, interval %d",
      erp_obj->current_state, ta_crit_cfg.interval);

  MTLK_INIT_TRY(erp, MTLK_OBJ_PTR(erp_obj))
    MTLK_INIT_STEP(erp, LOCK_INIT, MTLK_OBJ_PTR(erp_obj),
                   mtlk_osal_lock_init, (&erp_obj->lock));
    MTLK_INIT_STEP_EX(erp, CONFIG_TA, MTLK_OBJ_PTR(erp_obj),
                      mtlk_ta_erp_cfg,
                      (mtlk_vap_get_ta(erp_obj->vap_handle), &ta_crit_cfg, TRUE),
                      res,
                      res == MTLK_ERR_OK, MTLK_ERR_PARAMS);
    MTLK_INIT_STEP_EX(erp, REG_CALLBACK, MTLK_OBJ_PTR(erp_obj),
                      mtlk_ta_crit_register,
                      (mtlk_vap_get_ta(erp_obj->vap_handle),
                       TA_CRIT_ID_ERP,
                       _mtlk_erp_auto_callback,
                       HANDLE_T(erp_obj)),
                      erp_obj->ta_crit_handle,
                      erp_obj->ta_crit_handle != MTLK_INVALID_HANDLE, MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(erp, REG_ABILITIES, MTLK_OBJ_PTR(erp_obj),
                   mtlk_abmgr_register_ability_set,
                   (mtlk_vap_get_abmgr(erp_obj->vap_handle),
                    _erp_abilities, ARRAY_SIZE(_erp_abilities)));
    MTLK_INIT_STEP_VOID(erp, EN_ABILITIES, MTLK_OBJ_PTR(erp_obj),
                        mtlk_abmgr_enable_ability_set,
                        (mtlk_vap_get_abmgr(erp_obj->vap_handle),
                         _erp_abilities, ARRAY_SIZE(_erp_abilities)));
  MTLK_INIT_FINALLY(erp, MTLK_OBJ_PTR(erp_obj))
  MTLK_INIT_RETURN(erp, MTLK_OBJ_PTR(erp_obj), _mtlk_erp_cleanup, (erp_obj));
}

/*****************************************************************************
* Function implementation
******************************************************************************/
mtlk_coc_t* __MTLK_IFUNC
mtlk_coc_create (const mtlk_coc_cfg_t *cfg)
{
  mtlk_coc_t *coc_obj = mtlk_osal_mem_alloc(sizeof(mtlk_coc_t), MTLK_MEM_TAG_DFS);

  if (NULL != coc_obj)
  {
    memset(coc_obj, 0, sizeof(mtlk_coc_t));
    if (MTLK_ERR_OK != _mtlk_coc_init(coc_obj, cfg)) {
      mtlk_osal_mem_free(coc_obj);
      coc_obj = NULL;
    }
  }

  return coc_obj;
}

mtlk_erp_t* __MTLK_IFUNC
mtlk_erp_create (const mtlk_coc_erp_cfg_t *cfg)
{
  mtlk_erp_t *erp_obj = mtlk_osal_mem_alloc(sizeof(mtlk_erp_t), MTLK_MEM_TAG_DFS);

  ILOG0_V("ERP CREATE");
  if (NULL != erp_obj)
  {
    memset(erp_obj, 0, sizeof(mtlk_erp_t));
    if (MTLK_ERR_OK != _mtlk_erp_init(erp_obj, cfg)) {
      mtlk_osal_mem_free(erp_obj);
      erp_obj = NULL;
    }
  }

  return erp_obj;
}

void __MTLK_IFUNC
mtlk_coc_delete (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  _mtlk_coc_cleanup(coc_obj);

  mtlk_osal_mem_free(coc_obj);
}

void __MTLK_IFUNC
mtlk_erp_delete (mtlk_erp_t *erp_obj)
{
  MTLK_ASSERT(NULL != erp_obj);

  ILOG0_V("ERP DELETE");
  _mtlk_erp_cleanup(erp_obj);

  mtlk_osal_mem_free(erp_obj);
}

BOOL __MTLK_IFUNC
mtlk_coc_is_auto_mode (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  return coc_obj->is_auto_mode;
}

BOOL __MTLK_IFUNC
mtlk_coc_get_auto_mode_cfg (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  return coc_obj->auto_mode_cfg;
}

int __MTLK_IFUNC
mtlk_coc_set_power_mode (mtlk_coc_t *coc_obj, const BOOL is_auto_mode)
{
  _mtlk_coc_power_state_t state;
  int res = MTLK_ERR_OK;
  int max_antennas = mtlk_core_get_max_tx_antennas(mtlk_vap_get_core(coc_obj->vap_handle));
  
  MTLK_ASSERT(NULL != coc_obj);

  coc_obj->auto_mode_cfg = is_auto_mode;

  if (0 == mtlk_vap_manager_get_active_vaps_number(mtlk_vap_get_manager(coc_obj->vap_handle))) {
    return MTLK_ERR_OK;
  }

  mtlk_osal_lock_acquire(&coc_obj->lock);

  if (is_auto_mode) {
    /* if (!coc_obj->is_auto_mode) { */
    if (TRUE) { /* always update */
      /* set mode to high power mode */
      if (coc_obj->current_state == COC_POWER_TRANSIENT_STATE) {
        ILOG1_D("Next execution COC state is %d \n",coc_obj->pending_state);
        state = COC_POWER_TRANSIENT_STATE;

        if (coc_obj->pending_state != max_antennas) {
        /* set the transient timer to start here */
        res = _mtlk_coc_set_interval(coc_obj, state);
        if (MTLK_ERR_OK != res)
          goto FINISH;
        }
      } else {
        res = _mtlk_coc_set_antennas(coc_obj, &coc_obj->hw_antenna_cfg);
        if (MTLK_ERR_OK != res) {
          WLOG_DDD("CID-%04x: Can not select CoC power mode: TX%dxRX%d",
              mtlk_vap_get_oid(coc_obj->vap_handle),
              coc_obj->hw_antenna_cfg.num_tx_antennas,
              coc_obj->hw_antenna_cfg.num_rx_antennas);
          goto FINISH;
        }

        state = COC_POWER_START_STATE;

        res = _mtlk_coc_set_interval(coc_obj, state);
        if (MTLK_ERR_OK != res)
          goto FINISH;

        _mtlk_coc_set_state(coc_obj, state);

        mtlk_ta_crit_start(coc_obj->ta_crit_handle);
      }
    }
  }
  else {
    /* restore previous configuration */
    res = _mtlk_coc_set_antennas(coc_obj, &coc_obj->manual_antenna_cfg);
    if (MTLK_ERR_OK != res) {
      WLOG_DDD("CID-%04x: Can not select CoC power mode: TX%dxRX%d",
               mtlk_vap_get_oid(coc_obj->vap_handle),
               coc_obj->manual_antenna_cfg.num_tx_antennas,
               coc_obj->manual_antenna_cfg.num_rx_antennas);
      goto FINISH;
    }

    if (coc_obj->is_auto_mode) {
      mtlk_ta_crit_stop(coc_obj->ta_crit_handle);
    }
  }

  coc_obj->is_auto_mode = is_auto_mode;

FINISH:
  mtlk_osal_lock_release(&coc_obj->lock);
  return res;
}

int __MTLK_IFUNC
mtlk_coc_set_erp_mode (mtlk_erp_t *erp_obj, mtlk_coc_erp_cfg_t *erp_cfg)
{
  int res = MTLK_ERR_OK;
  ta_crit_erp_cfg_t ta_crit_cfg;

  MTLK_ASSERT(NULL != erp_obj);

  mtlk_osal_lock_acquire(&erp_obj->lock);

  erp_obj->cfg_erp_enabled   = erp_cfg->erp_enabled;
  erp_obj->initial_wait_time = erp_cfg->initial_wait_time;
  erp_obj->radio_on_time     = erp_cfg->radio_on_time;
  erp_obj->radio_off_time    = erp_cfg->radio_off_time;
  erp_obj->max_num_sta       = erp_cfg->max_num_sta;
  erp_obj->rx_tp_max         = erp_cfg->rx_tp_max;
  erp_obj->tx_20_max_tp      = erp_cfg->tx_20_max_tp;
  erp_obj->tx_40_max_tp      = erp_cfg->tx_40_max_tp;
  erp_obj->tx_80_max_tp      = erp_cfg->tx_80_max_tp;
  erp_obj->tx_160_max_tp     = erp_cfg->tx_160_max_tp;

  ta_crit_cfg.interval = erp_cfg->initial_wait_time * 10; /*in ticks */
  res = mtlk_ta_erp_cfg(mtlk_vap_get_ta(erp_obj->vap_handle), &ta_crit_cfg, TRUE);

  mtlk_osal_lock_release(&erp_obj->lock);

  /* Reconfigure ERP */
  mtlk_coc_erp_deactivate(erp_obj);
  mtlk_coc_erp_activate(erp_obj);
  return res;
}

int __MTLK_IFUNC
mtlk_coc_get_erp_mode (mtlk_erp_t *erp_obj, mtlk_coc_erp_cfg_t *erp_cfg)
{
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != erp_obj);

  mtlk_osal_lock_acquire(&erp_obj->lock);

  erp_cfg->erp_enabled       = erp_obj->cfg_erp_enabled;
  erp_cfg->initial_wait_time = erp_obj->initial_wait_time;
  erp_cfg->radio_on_time     = erp_obj->radio_on_time;
  erp_cfg->radio_off_time    = erp_obj->radio_off_time;
  erp_cfg->max_num_sta       = erp_obj->max_num_sta;
  erp_cfg->rx_tp_max         = erp_obj->rx_tp_max;
  erp_cfg->tx_20_max_tp      = erp_obj->tx_20_max_tp;
  erp_cfg->tx_40_max_tp      = erp_obj->tx_40_max_tp;
  erp_cfg->tx_80_max_tp      = erp_obj->tx_80_max_tp;
  erp_cfg->tx_160_max_tp     = erp_obj->tx_160_max_tp;

  mtlk_osal_lock_release(&erp_obj->lock);

  return res;
}

int __MTLK_IFUNC
mtlk_coc_erp_deactivate (mtlk_erp_t *erp_obj)
{
  int res = MTLK_ERR_OK;
  BOOL update = FALSE;

  MTLK_ASSERT(NULL != erp_obj);

  mtlk_osal_lock_acquire(&erp_obj->lock);

  if (erp_obj->current_state) {
    ILOG1_V("ERP Crit stop");
    mtlk_ta_crit_stop(erp_obj->ta_crit_handle);
    erp_obj->current_state = FALSE;
    if (erp_obj->cur_erp_enabled) {
      erp_obj->cur_erp_enabled = 0;
      update = TRUE;
    }
  } else {
    ILOG1_V("ERP already stopped");
  }

  mtlk_osal_lock_release(&erp_obj->lock);

  if (update)
     mtlk_core_send_erp_cfg(erp_obj);
  return res;
}

int __MTLK_IFUNC
mtlk_coc_erp_activate (mtlk_erp_t *erp_obj)
{
  int res = MTLK_ERR_OK;
  BOOL update = FALSE;

  MTLK_ASSERT(NULL != erp_obj);

  if (0 == mtlk_vap_manager_get_active_vaps_number(mtlk_vap_get_manager(erp_obj->vap_handle))) {
    return MTLK_ERR_OK;
  }

  mtlk_osal_lock_acquire(&erp_obj->lock);

  if (erp_obj->current_state != erp_obj->cfg_erp_enabled) {
    if (erp_obj->cfg_erp_enabled) {
      ILOG1_V("ERP Crit start");
      mtlk_ta_crit_start(erp_obj->ta_crit_handle);
    } else {
      ILOG1_V("ERP Crit stop");
      mtlk_ta_crit_stop(erp_obj->ta_crit_handle);
      if (erp_obj->cur_erp_enabled) {
        erp_obj->cur_erp_enabled = 0;
        update = TRUE;
      }
    }
    erp_obj->current_state = erp_obj->cfg_erp_enabled;
  }

  mtlk_osal_lock_release(&erp_obj->lock);

  /* update erp in fw */
  if (update)
    mtlk_core_send_erp_cfg(erp_obj);

  return res;
}

int  __MTLK_IFUNC
mtlk_coc_set_pause_power_mode (mtlk_coc_t *coc_obj)
{
  BOOL is_auto_mode;

  MTLK_ASSERT(NULL != coc_obj);

  /* Store auto_mode and set 1x1, but don;'t modify curent_state etc */
  mtlk_osal_lock_acquire(&coc_obj->lock);
  is_auto_mode = coc_obj->is_auto_mode;
  if(coc_obj->current_state == COC_POWER_TRANSIENT_STATE ){
    mtlk_osal_lock_release(&coc_obj->lock);
    return MTLK_ERR_OK;
  }else if (is_auto_mode) {
    mtlk_ta_crit_stop(coc_obj->ta_crit_handle);
  }
  mtlk_osal_lock_release(&coc_obj->lock);

  ILOG1_S("Paused CoC %s mode", is_auto_mode ? "auto" : "manual");

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_coc_set_actual_power_mode(mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);
  return mtlk_coc_set_power_mode(coc_obj, mtlk_coc_get_auto_mode_cfg(coc_obj));
}

mtlk_error_t __MTLK_IFUNC
wave_coc_get_current_power_params (mtlk_coc_t *coc_obj, mtlk_coc_power_cfg_t *coc_power)
{
  MTLK_ASSERT(NULL != coc_obj);
  MTLK_ASSERT(NULL != coc_power);
  if (!coc_obj || !coc_power) {
    return MTLK_ERR_UNKNOWN; /* TBD log */
  }

  mtlk_osal_lock_acquire(&coc_obj->lock);
  coc_power->ant_mask_params = coc_obj->current_ant_mask_cfg;
  coc_power->antenna_params  = coc_obj->current_antenna_cfg;
  coc_power->is_auto_mode    = coc_obj->is_auto_mode;
  mtlk_osal_lock_release(&coc_obj->lock);

  return MTLK_ERR_OK;
}

mtlk_coc_antenna_cfg_t * __MTLK_IFUNC
mtlk_coc_get_current_params (mtlk_coc_t *coc_obj)
{
  mtlk_coc_antenna_cfg_t *current_antenna_cfg;
  MTLK_ASSERT(NULL != coc_obj);

  mtlk_osal_lock_acquire(&coc_obj->lock);
  current_antenna_cfg = &coc_obj->current_antenna_cfg;
  mtlk_osal_lock_release(&coc_obj->lock);

  return current_antenna_cfg;
}

unsigned __MTLK_IFUNC
mtlk_coc_get_current_tx_antennas (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);
  return coc_obj->current_antenna_cfg.num_tx_antennas;
}

void __MTLK_IFUNC
mtlk_coc_get_current_ant_mask (mtlk_coc_t *coc_obj, mtlk_coc_ant_mask_cfg_t *ant_mask)
{
  MTLK_ASSERT(NULL != coc_obj);
  MTLK_ASSERT(NULL != ant_mask);

  mtlk_osal_lock_acquire(&coc_obj->lock);
  *ant_mask = coc_obj->current_ant_mask_cfg;
  mtlk_osal_lock_release(&coc_obj->lock);
}

mtlk_coc_auto_cfg_t * __MTLK_IFUNC
mtlk_coc_get_auto_params (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  return &coc_obj->auto_antenna_cfg;
}

void __MTLK_IFUNC
mtlk_coc_reset_antenna_params (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  mtlk_osal_lock_acquire(&coc_obj->lock);
  coc_obj->current_antenna_cfg = coc_obj->hw_antenna_cfg;
  mtlk_osal_lock_release(&coc_obj->lock);

  wave_radio_current_antenna_mask_reset(wave_vap_radio_get(coc_obj->vap_handle));
}

void __MTLK_IFUNC
mtlk_coc_dump_antenna_params (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  ILOG1_DD("hw_tx_antenna_mask = %d hw_rx_antenna_mask = %d\n", coc_obj->hw_tx_antenna_mask, coc_obj->hw_rx_antenna_mask);
  ILOG1_DD("hw_antenna_cfg : %d %d\n", coc_obj->hw_antenna_cfg.num_tx_antennas, coc_obj->hw_antenna_cfg.num_tx_antennas);
  ILOG1_DD("current_antenna_cfg : %d %d\n", coc_obj->current_antenna_cfg.num_tx_antennas, coc_obj->hw_antenna_cfg.num_tx_antennas);
  ILOG1_DD("manual_antenna_cfg : %d %d\n", coc_obj->manual_antenna_cfg.num_tx_antennas, coc_obj->hw_antenna_cfg.num_tx_antennas);
}

static mtlk_error_t
_wave_coc_check_antenna_params (mtlk_coc_t *coc_obj, const mtlk_coc_antenna_cfg_t *antenna_params)
{
  int res = MTLK_ERR_OK;

  MTLK_ASSERT(NULL != coc_obj);

  if ((antenna_params->num_rx_antennas > coc_obj->hw_antenna_cfg.num_rx_antennas) ||
      (antenna_params->num_tx_antennas > coc_obj->hw_antenna_cfg.num_tx_antennas) ||
      (antenna_params->num_rx_antennas != antenna_params->num_tx_antennas)) {
    ELOG_DDD("CID-%04x: Wrong CoC power mode TX%dxRX%d",
             mtlk_vap_get_oid(coc_obj->vap_handle),
             antenna_params->num_tx_antennas, antenna_params->num_rx_antennas);
    res = MTLK_ERR_PARAMS;
  }

  return res;
}

static mtlk_error_t
_wave_coc_check_ant_mask_params (const mtlk_coc_t *coc_obj,
                                 const mtlk_coc_power_cfg_t *power_params, mtlk_coc_ant_mask_cfg_t *out)
{
  mtlk_coc_ant_mask_cfg_t  input, curr, dflt, tmp;
  size_t        idx, len, start, stop;
  unsigned      ant_num, nof_masks;
  BOOL          is_auto;

  MTLK_ASSERT(NULL != coc_obj);
  MTLK_ASSERT(NULL != power_params);
  MTLK_ASSERT(NULL != out);

  is_auto   = power_params->is_auto_mode;
  input     = power_params->ant_mask_params;
  nof_masks = power_params->nof_ant_masks;

  dflt = coc_obj->default_ant_mask_cfg;
  curr = coc_obj->current_ant_mask_cfg;

  ILOG1_DDDDD("auto %d, dflt 0x%08X, curr 0x%08X, in 0x%08X, nof_ant_masks %u",
              is_auto, dflt.word, curr.word, input.word, nof_masks);

  tmp = curr;
  if (0 == nof_masks) /* nothing to change */
    goto out;

  len = ARRAY_SIZE(tmp.mask);
  if (is_auto) { /* for filled antennas */
    start = 0;
    stop  = MIN(power_params->nof_ant_masks, len);
  } else { /* manual -- for one antenna, the value is placed at mask[0] */
    ant_num = power_params->antenna_params.num_tx_antennas;
    if ((ant_num >= 1) && (ant_num <= len)) {
      idx = ant_num - 1;
      start = idx;
      stop  = start + 1;
      input.mask[idx] = input.mask[0]; /* specified antenna */
    } else {
      ELOG_D("Illegal antenna number %u", ant_num);
      goto error;
    }
  }

  /* Validate input and set new config */
  for (idx = start; idx < stop; idx++) {
    uint8 mask, hw_mask;

    hw_mask = coc_obj->hw_max_antenna_mask;
    ant_num = idx + 1;
    mask = input.mask[idx];
    if (mask == MTLK_COC_ANT_MASK_VALUE_RESET) {
      tmp.mask[idx] = dflt.mask[idx]; /* reset to default */
    } else if (mask == MTLK_COC_ANT_MASK_VALUE_UNDEF) {
      /* already done: use current */
    } else {
      unsigned nbits = count_bits_set(mask);
      if (nbits != ant_num) {
        ELOG_DDD("Antenna %u: wrong mask 0x%X, %u bits are set", ant_num, mask, nbits);
        goto error;
      } else if (~hw_mask & mask) {
        ELOG_DDD("Antenna %u: The given mask (0x%X) isn't supported by hardware (0x%X)", ant_num, mask, hw_mask);
        goto error;
      } else {
        tmp.mask[idx] = mask;
      }
    }
  }

out:
  ILOG0_D("Update Antenna masks to 0x%08X", tmp.word);
  *out = tmp;

  return MTLK_ERR_OK;

error:
  return MTLK_ERR_PARAMS;
}

mtlk_error_t __MTLK_IFUNC
wave_coc_set_power_params (mtlk_coc_t *coc_obj, const mtlk_coc_power_cfg_t *power_params)
{
  mtlk_coc_ant_mask_cfg_t tmp_mask;
  mtlk_error_t            res;

  mtlk_osal_lock_acquire(&coc_obj->lock);

  res = _wave_coc_check_antenna_params(coc_obj, &power_params->antenna_params);
  if (MTLK_ERR_OK != res)
    goto finish;

  res = _wave_coc_check_ant_mask_params(coc_obj, power_params, &tmp_mask);
  if (MTLK_ERR_OK != res)
    goto finish;

  /* Update all needs except auto_mode */
  coc_obj->manual_antenna_cfg   = power_params->antenna_params;
  coc_obj->current_ant_mask_cfg = tmp_mask;
  coc_obj->bw_params.coc_BW = power_params->bw_params.coc_BW;
  coc_obj->bw_params.coc_omn_IE = power_params->bw_params.coc_omn_IE;


finish:
  mtlk_osal_lock_release(&coc_obj->lock);

  return res;
}

int __MTLK_IFUNC
mtlk_coc_set_auto_params (mtlk_coc_t *coc_obj, const mtlk_coc_auto_cfg_t *in_auto_params)
{
  mtlk_coc_auto_cfg_t  new_auto_cfg, *auto_params = &new_auto_cfg;
  unsigned oid;
  int res = MTLK_ERR_OK;
  int max_antennas;
  int params_count;
  struct intel_vendor_event_coc  bw_params;
  struct wireless_dev *wdev;
  mtlk_core_t *core = mtlk_vap_get_core(coc_obj->vap_handle);

  MTLK_ASSERT(NULL != coc_obj);
  MTLK_ASSERT(NULL != auto_params);

  oid = mtlk_vap_get_oid(coc_obj->vap_handle);

  /* update filled params only */
  params_count = in_auto_params->params_count;
  if ((params_count < MTLK_COC_AUTO_MIN_CFG_PARAMS) ||
      (params_count > MTLK_COC_AUTO_MAX_CFG_PARAMS)) {
    ELOG_DDDD("CID-%04x: Wrong CoC auto configuration params count %d, allowed %d...%d",
              oid, params_count, MTLK_COC_AUTO_MIN_CFG_PARAMS, MTLK_COC_AUTO_MAX_CFG_PARAMS);
    return MTLK_ERR_PARAMS;
  }

  mtlk_osal_lock_acquire(&coc_obj->lock);
  new_auto_cfg = coc_obj->auto_antenna_cfg;
  mtlk_osal_lock_release(&coc_obj->lock);

  wave_wordcpy(new_auto_cfg.values, ARRAY_SIZE(new_auto_cfg.values), in_auto_params->values, params_count);

  max_antennas = mtlk_core_get_max_tx_antennas(mtlk_vap_get_core(coc_obj->vap_handle));

  if ((auto_params->starting_time == 0) ||
      (auto_params->interval_1x1 == 0) ||
      ((auto_params->interval_2x2 == 0) && (max_antennas == 2)) ||
      ((auto_params->interval_3x3 == 0) && (max_antennas == 3)) ||
      ((auto_params->interval_4x4 == 0) && (max_antennas == 4))) {
    ELOG_D("CID-%04x: Wrong CoC auto configuration: interval = 0", oid);
    return MTLK_ERR_PARAMS;
  }
  else if ((auto_params->high_limit_1x1 == 0) ||
           ((auto_params->low_limit_2x2 == 0) && (max_antennas >= 2)) ||
           ((auto_params->high_limit_2x2 == 0) && (max_antennas >= 2)) ||
           ((auto_params->low_limit_3x3 == 0) && (max_antennas >= 3)) ||
           ((auto_params->high_limit_3x3 == 0) && (max_antennas >= 4)) ||
           ((auto_params->low_limit_4x4 == 0) && (max_antennas >= 4))) {
    ELOG_D("CID-%04x: Wrong CoC auto configuration: threshold = 0", oid);
    return MTLK_ERR_PARAMS;
  }

  /* RSSI validation */
  else if ((auto_params->low_rssi_2x2 < MIN_RSSI) || (auto_params->low_rssi_2x2 > 0) ||
           (auto_params->low_rssi_3x3 < MIN_RSSI) || (auto_params->low_rssi_3x3 > 0) ||
           (auto_params->low_rssi_4x4 < MIN_RSSI) || (auto_params->low_rssi_4x4 > 0) ||
           (auto_params->rssi_delta_down > 0)) {
    ELOG_DD("CID-%04x: Wrong CoC auto configuration: RSSI value is not in range [%d...0]", oid, MIN_RSSI);
  } else {
    mtlk_osal_lock_acquire(&coc_obj->lock);
    coc_obj->auto_antenna_cfg = new_auto_cfg;
    if (coc_obj->is_auto_mode) {
      /* restart timer on-the-fly */
      if (auto_params->bandwidth_flag == 1) {
        wdev = mtlk_df_user_get_wdev(mtlk_df_get_user(mtlk_vap_get_df(core->vap_handle)));
        memset(&bw_params, 0, sizeof(bw_params));
        bw_params.coc_BW = COC_BW_SWITCH_UP;
        bw_params.coc_omn_IE = 0;
        _mtlk_coc_set_state(coc_obj, COC_POWER_START_STATE);
        ILOG1_D("COC nl event send for Higher BandWidth with OMN IE = %d \n",bw_params.coc_omn_IE);
        res = wv_ieee80211_vendor_event_coc(LTQ_NL80211_VENDOR_EVENT_COC_BEACON_UPDATE, wdev, &bw_params, sizeof(bw_params));
        if (MTLK_ERR_OK != res) {
          ELOG_V("COC nl event send failed");
        }
      }
      res = _mtlk_coc_set_interval(coc_obj, _mtlk_coc_get_current_state(coc_obj));
      if (MTLK_ERR_OK != res) {
        mtlk_osal_lock_release(&coc_obj->lock);
        return MTLK_ERR_PARAMS;
      }
    }
    mtlk_osal_lock_release(&coc_obj->lock);
  }

  return MTLK_ERR_OK;
}

int __MTLK_IFUNC
mtlk_coc_zwdfs_switch_opmode_notify (mtlk_coc_t *coc_obj, BOOL zwdfs_enabled)
{
  int res;
  mtlk_core_t *core;
  struct wireless_dev *wdev;
  struct intel_vendor_event_coc coc_params;

  MTLK_ASSERT(NULL != coc_obj);

  coc_params.coc_BW = COC_BW_SWITCH_UP;
  coc_params.coc_omn_IE = coc_obj->current_antenna_cfg.num_rx_antennas;
  if (zwdfs_enabled) {
    coc_params.coc_is_max_nss = FALSE;
  } else {
    coc_params.coc_is_max_nss = TRUE;
  }

  core = mtlk_vap_get_core(coc_obj->vap_handle);
  wdev = mtlk_df_user_get_wdev(mtlk_df_get_user(mtlk_vap_get_df(core->vap_handle)));

  res = wv_ieee80211_vendor_event_coc(LTQ_NL80211_VENDOR_EVENT_COC_BEACON_UPDATE,
                                      wdev, &coc_params, sizeof(coc_params));
  if (MTLK_ERR_OK != res) {
    ELOG_V("COC nl event send failed");
  }

  return res;
}

void __MTLK_IFUNC
mtlk_coc_auto_mode_disable (mtlk_coc_t *coc_obj)
{
  MTLK_ASSERT(NULL != coc_obj);

  mtlk_osal_lock_acquire(&coc_obj->lock);

  if (coc_obj->is_auto_mode) {
    mtlk_ta_crit_stop(coc_obj->ta_crit_handle);
    coc_obj->is_auto_mode = FALSE;
  }

  mtlk_osal_lock_release(&coc_obj->lock);
}

int __MTLK_IFUNC
mtlk_coc_on_rcvry_configure (mtlk_coc_t *coc_obj)
{
  int res;

  mtlk_coc_reset_antenna_params(coc_obj);
  res = mtlk_coc_set_power_mode(coc_obj, mtlk_coc_get_auto_mode_cfg(coc_obj));

  return res;
}

int __MTLK_IFUNC
mtlk_erp_on_rcvry_configure (mtlk_erp_t *erp_obj)
{
  return mtlk_coc_erp_activate(erp_obj);
}

void __MTLK_IFUNC
mtlk_coc_on_rcvry_isol (mtlk_coc_t *coc_obj)
{
  mtlk_coc_auto_mode_disable(coc_obj);
}

void __MTLK_IFUNC
mtlk_erp_on_rcvry_isol (mtlk_erp_t *erp_obj)
{
  MTLK_ASSERT(NULL != erp_obj);

  mtlk_osal_lock_acquire(&erp_obj->lock);

  if (erp_obj->current_state) {
    mtlk_ta_crit_stop(erp_obj->ta_crit_handle);
    erp_obj->current_state = FALSE;
    erp_obj->cur_erp_enabled = 0; /* only update current state and not send message */
  }

  mtlk_osal_lock_release(&erp_obj->lock);
}
