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
 * Written by: Grygorii Strashko
 *
 * Power management functionality implementation in compliance with
 * Code of Conduct on Energy Consumption of Broadband Equipment (a.k.a CoC)
 *
 */


#ifndef __MTLK_POWER_MANAGEMENT_H__
#define __MTLK_POWER_MANAGEMENT_H__

#define   MTLK_IDEFS_ON
#include "mtlkidefs.h"

/**********************************************************************
 * Public declaration
***********************************************************************/
typedef struct _mtlk_coc_temp_t mtlk_coc_temp_t;
typedef struct _mtlk_coc_t mtlk_coc_t;
typedef struct _mtlk_erp_t mtlk_erp_t;

#define MTLK_COC_AUTO_INTER_CFG_PARAMS 4  /* time intervals  */
#define MTLK_COC_AUTO_LIMIT_CFG_PARAMS 6 /* high/low limits */
#define MTLK_COC_AUTO_RSSI_CFG_PARAMS  3 /* RSSI low limits */
#define MTLK_COC_AUTO_START_CFG_PARAMS 1 /* starting time */
#define MTLK_COC_AUTO_TRANS_TIME_CFG_PARAMS 1 /* Transient state time */
#define MTLK_COC_AUTO_BW_SWITCH_CFG_PARAMS 1 /* COC BW switch config flag */
#define MTLK_COC_AUTO_RSSI_SWITCH_CFG_PARAMS 1 /* COC RSSI switch config flag */
#define MTLK_COC_AUTO_MIN_CFG_PARAMS   (MTLK_COC_AUTO_INTER_CFG_PARAMS + MTLK_COC_AUTO_LIMIT_CFG_PARAMS)
#define MTLK_COC_AUTO_OPT_CFG_PARAMS   (MTLK_COC_AUTO_RSSI_CFG_PARAMS  + MTLK_COC_AUTO_START_CFG_PARAMS + MTLK_COC_AUTO_TRANS_TIME_CFG_PARAMS + MTLK_COC_AUTO_BW_SWITCH_CFG_PARAMS + MTLK_COC_AUTO_RSSI_SWITCH_CFG_PARAMS)
#define MTLK_COC_AUTO_MAX_CFG_PARAMS   (MTLK_COC_AUTO_MIN_CFG_PARAMS   + MTLK_COC_AUTO_OPT_CFG_PARAMS)

#define MTLK_COC_EXTRA_PARAMS         2  /* CoC BW and OMN IE flags */
#define MTLK_COC_NUM_ANTENNAS         4
#define MTLK_COC_PW_ANT_CFG_PARAMS    3  /* CoC power mode, TxAnt, RxAnt */
#define MTLK_COC_ANT_MASK_CFG_PARAMS  (MTLK_COC_NUM_ANTENNAS + MTLK_COC_EXTRA_PARAMS) /* optional, mask per antenna */
#define MTLK_COC_PW_MAX_CFG_PARAMS    (MTLK_COC_PW_ANT_CFG_PARAMS + MTLK_COC_ANT_MASK_CFG_PARAMS)

#define MTLK_COC_START_TIME_RESET     (0)
#define MTLK_COC_START_TIME_DFLT      (60)
#define MTLK_COC_START_TIME_MAX       (60 * 5)
#define MTLK_COC_START_TIME_UNDEF     (MAX_UINT32)

#define MTLK_COC_ANT_MASK_VALUE_RESET (0)
#define MTLK_COC_ANT_MASK_VALUE_MAX   ((1 << MTLK_COC_NUM_ANTENNAS) - 1)
#define MTLK_COC_ANT_MASK_VALUE_UNDEF (MAX_UINT8)


typedef struct _mtlk_coc_auto_cfg_t
{
  int    params_count; /* filled */
  union {
    uint32 values[MTLK_COC_AUTO_MAX_CFG_PARAMS];
    /* number of fields and its order correspond to "sCoCAutoCfg" */
    struct {
      uint32 interval_1x1;
      uint32 interval_2x2;
      uint32 interval_3x3;
      uint32 interval_4x4;
      uint32 high_limit_1x1;
      uint32 low_limit_2x2;
      uint32 high_limit_2x2;
      uint32 low_limit_3x3;
      uint32 high_limit_3x3;
      uint32 low_limit_4x4;
      /* RSSI */
      int32  low_rssi_2x2;
      int32  low_rssi_3x3;
      int32  low_rssi_4x4;
      /* Minimal time to start */
      uint32 starting_time;
      uint32 transient_time;
      /*This is flag variable still kept as uint32 to fit structure allignment */
      uint32 bandwidth_flag;
      /* RSSI differnce in dbm, that should be considered while moving down
       * antenna stream */
      int32 rssi_delta_down;
     };
  };
} __MTLK_IDATA mtlk_coc_auto_cfg_t;

typedef struct _mtlk_coc_antenna_cfg_t
{
  uint8 num_tx_antennas;
  uint8 num_rx_antennas;
} __MTLK_IDATA mtlk_coc_antenna_cfg_t;

typedef struct
{
  union {
    uint32  word; /* for alignment */
    uint8   mask[MTLK_COC_NUM_ANTENNAS]; /* per antenna */
  };
} __MTLK_IDATA mtlk_coc_ant_mask_cfg_t;

typedef struct _mtlk_coc_bw_t
{
  uint8 coc_BW;
  uint8 coc_omn_IE;
} __MTLK_IDATA mtlk_coc_bw_t;

typedef struct
{
  mtlk_coc_ant_mask_cfg_t   ant_mask_params;
  mtlk_coc_antenna_cfg_t    antenna_params;
  mtlk_coc_bw_t             bw_params;
  uint8                     nof_ant_masks; /* num of filled */
  BOOL                      is_auto_mode;
} __MTLK_IDATA mtlk_coc_power_cfg_t;

typedef struct _mtlk_coc_cfg_t
{
  mtlk_vap_handle_t vap_handle;
  mtlk_txmm_t *txmm;
  mtlk_coc_auto_cfg_t     default_auto_cfg;
  mtlk_coc_antenna_cfg_t  hw_antenna_cfg;
  uint8                   hw_ant_mask; /* TX/RX */
} __MTLK_IDATA mtlk_coc_cfg_t;

typedef struct {
  uint32 max_tp;
  uint8  max_ants; /* antennas */
  int8   rssi;
} __MTLK_IDATA mtlk_coc_crit_t;

typedef struct _mtlk_coc_erp_cfg_t
{
  uint32 initial_wait_time; /* seconds */
  uint32 radio_off_time;    /* milliseconds */
  uint32 radio_on_time;     /* milliseconds */
  uint32 erp_enabled;       /* 0 - dsiabled, 1 - enabled */
  uint32 tx_20_max_tp;      /* max tp for 20 Mhz bw */
  uint32 tx_40_max_tp;      /* max tp for 40 Mhz bw */
  uint32 tx_80_max_tp;      /* max tp for 80 Mhz bw */
  uint32 tx_160_max_tp;     /* max tp for 160 Mhz bw */
  uint32 rx_tp_max;         /* max tp for rx */
  uint32 max_num_sta;       /* max connected sta */

  mtlk_vap_handle_t vap_handle;
  mtlk_txmm_t *txmm;
} __MTLK_IDATA mtlk_coc_erp_cfg_t;

/**********************************************************************
 * Public function declaration
***********************************************************************/
mtlk_coc_t* __MTLK_IFUNC
mtlk_coc_create(const mtlk_coc_cfg_t *cfg);

mtlk_erp_t* __MTLK_IFUNC
mtlk_erp_create(const mtlk_coc_erp_cfg_t *cfg);

void __MTLK_IFUNC
mtlk_coc_delete(mtlk_coc_t *coc_obj);

void __MTLK_IFUNC
mtlk_erp_delete(mtlk_erp_t *erp_obj);

int __MTLK_IFUNC
mtlk_coc_set_auto_params(mtlk_coc_t *coc_obj, const mtlk_coc_auto_cfg_t *auto_params);

mtlk_coc_auto_cfg_t * __MTLK_IFUNC
mtlk_coc_get_auto_params(mtlk_coc_t *coc_obj);

mtlk_error_t __MTLK_IFUNC
wave_coc_get_current_power_params (mtlk_coc_t *coc_obj, mtlk_coc_power_cfg_t *coc_power);

mtlk_coc_antenna_cfg_t * __MTLK_IFUNC
mtlk_coc_get_current_params(mtlk_coc_t *coc_obj);

unsigned __MTLK_IFUNC
mtlk_coc_get_current_tx_antennas (mtlk_coc_t *coc_obj);

void __MTLK_IFUNC
mtlk_coc_get_current_ant_mask (mtlk_coc_t *coc_obj, mtlk_coc_ant_mask_cfg_t *ant_mask);

mtlk_error_t __MTLK_IFUNC
wave_coc_set_ant_mask_params (mtlk_coc_t *coc_obj, const mtlk_coc_ant_mask_cfg_t *in_cfg);

mtlk_error_t __MTLK_IFUNC
wave_coc_set_power_params (mtlk_coc_t *coc_obj, const mtlk_coc_power_cfg_t *power_params);

int __MTLK_IFUNC
mtlk_coc_set_power_mode(mtlk_coc_t *coc_obj, const BOOL is_auto_mode);

int __MTLK_IFUNC
mtlk_coc_set_erp_mode (mtlk_erp_t *erp_obj, mtlk_coc_erp_cfg_t *erp_cfg);

int __MTLK_IFUNC
mtlk_coc_get_erp_mode (mtlk_erp_t *erp_obj, mtlk_coc_erp_cfg_t *erp_cfg);

int __MTLK_IFUNC
mtlk_coc_erp_deactivate(mtlk_erp_t *erp_obj);

int __MTLK_IFUNC
mtlk_coc_erp_activate(mtlk_erp_t *erp_obj);

int __MTLK_IFUNC
mtlk_coc_set_actual_power_mode(mtlk_coc_t *coc_obj);

int __MTLK_IFUNC
mtlk_coc_set_pause_power_mode(mtlk_coc_t *coc_obj);

BOOL __MTLK_IFUNC
mtlk_coc_is_auto_mode(mtlk_coc_t *coc_obj);

BOOL __MTLK_IFUNC
mtlk_coc_get_auto_mode_cfg(mtlk_coc_t *coc_obj);

void __MTLK_IFUNC
mtlk_coc_auto_mode_disable(mtlk_coc_t *coc_obj);

void __MTLK_IFUNC
mtlk_coc_reset_antenna_params(mtlk_coc_t *coc_obj);

int __MTLK_IFUNC
mtlk_coc_on_rcvry_configure(mtlk_coc_t *coc_obj);

int __MTLK_IFUNC
mtlk_erp_on_rcvry_configure(mtlk_erp_t *erp_obj);

void __MTLK_IFUNC
mtlk_coc_on_rcvry_isol(mtlk_coc_t *coc_obj);

void __MTLK_IFUNC
mtlk_erp_on_rcvry_isol(mtlk_erp_t *erp_obj);

int __MTLK_IFUNC
mtlk_coc_update_antenna_cfg (mtlk_coc_t *coc_obj, uint8 and_mask, BOOL update_coc_masks, BOOL *update_hw);

void __MTLK_IFUNC
mtlk_coc_dump_antenna_params (mtlk_coc_t *coc_obj);

int __MTLK_IFUNC
mtlk_coc_zwdfs_switch_opmode_notify (mtlk_coc_t *coc_obj, BOOL zwdfs_enabled);

#define   MTLK_IDEFS_OFF
#include "mtlkidefs.h"

#endif /* __MTLK_POWER_MANAGEMENT_H__ */
