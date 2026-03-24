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
 * $Id: ta_priv.h 13206 2012-07-04 08:07:40Z sumilovs $
 *
 * Traffic Analyzer Private Header
 *
 */

#ifndef __MTLK_TA_PRIV_H__
#define __MTLK_TA_PRIV_H__

#define TA_TIMER_MS_PER_TICK 50
#define TA_TIMER_MAX_VALUE_IN_TICKS (5000 / TA_TIMER_MS_PER_TICK)

/**
 *  Formula for CoC result calculation:
 *  
 *    N (Mb/s) = bytes [for interval] * K * 8 [bytes->bits] / (1024*1024) [bits->Mbits],
 *
 *    , where K = ticks_per_second / interval_in_ticks
 *
 */

/* #define TA_COC_TICKS_PER_SECOND  (1000 / TA_COC_TICK_MSEC) /\* 10 *\/ */
/* #define TA_SHIFT_BYTES_TO_KBYTES 7 /\* +10 : bytes -> Kbytes, -3 : bytes -> bits *\/ */
#define TA_Q_SHIFT               18
/* #define TA_BYTES_SHIFT           16 */

/* #define TA_COC_TICKS_PER_SECOND_SHIFTED (TA_COC_TICKS_PER_SECOND << TA_Q_SHIFT)               /\* 10*2^12 *\/ */
/* #define TA_COC_FINAL_SHIFT (TA_SHIFT_BYTES_TO_KBYTES - TA_BYTES_SHIFT + TA_Q_SHIFT)           /\* 7-16+12=3 *\/ */
/* #define TA_COC_ROUND_ADD   (1<<(TA_COC_FINAL_SHIFT-1))                                                                 */

typedef struct __ta_wss_cntr_t{
  uint64 prev;
  uint32 delta;
} _ta_wss_cntr_t;

typedef struct __sta_wss_entry_t {
  mtlk_dlist_entry_t    list_entry;

  IEEE_ADDR sta_addr;
  mtlk_vap_handle_t vap_handle;

  _ta_wss_cntr_t  coc_rx_bytes;
  _ta_wss_cntr_t  coc_tx_bytes;
  _ta_wss_cntr_t  erp_rx_bytes;
  _ta_wss_cntr_t  erp_tx_bytes;
  BOOL            coc_wss_valid;
  BOOL            erp_wss_valid;
} _sta_wss_entry_t;

typedef struct __ta_crit_entry_t {
  struct __traf_analyzer_t *ta;
  ta_crit_id_t    id;
  uint32          signature;
  
  void            (*fcn)(struct __ta_crit_entry_t *crit); /* NULL means not started */
  
  ta_crit_clb_t   clb;          
  mtlk_handle_t   clb_ctx;

  uint32          tmr_cnt;      /* Counter of ticks for criteria */
  uint32          tmr_period;   /* Counter period for crit. fcn will be called on (cnt==period) */
} _ta_crit_entry_t;

/* TA structure */
typedef struct __traf_analyzer_t {
  uint32              signature;        /* TA object signature */
  mtlk_vap_manager_t  *vap_manager;     /* VAP manager */

  mtlk_osal_timer_t   timer;            /* Timer for criteria processing */
  mtlk_atomic_t       timer_res_ms;     /* Timer resolution in ms */

  ta_crit_coc_cfg_t   coc_cfg;          /* COC configuration */
  ta_crit_erp_cfg_t   erp_cfg;          /* erp configuration */

  _ta_crit_entry_t crit_tbl[TA_CRIT_NUM];
  uint32              active_crit_num;
  mtlk_osal_mutex_t   sta_wss_list_lock;  /* Protect content of sta_wss list entries */
  mtlk_dlist_t        sta_wss_list;       /* List of connected stations */

  MTLK_DECLARE_INIT_STATUS;

} _traf_analyzer_t;

#endif
