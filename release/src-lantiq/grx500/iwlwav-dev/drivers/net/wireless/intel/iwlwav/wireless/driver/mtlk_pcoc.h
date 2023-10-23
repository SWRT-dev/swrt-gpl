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
 *
 * Power management functionality implementation in compliance with
 * Code of Conduct on Energy Consumption of Broadband Equipment (a.k.a CoC)
 *
 * On-the fly CPU frequency switch on AR10
 *
 */


#ifndef __MTLK_PCOC_H__
#define __MTLK_PCOC_H__

#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT

/* outgoing requests for PMCU detectors / notifiers */
typedef enum {
  PMCU_REQ_LOW_PRECHANGE,
  PMCU_REQ_LOW_POSTCHANGE,
  PMCU_REQ_HIGH_PRECHANGE,
  PMCU_REQ_HIGH_POSTCHANGE
} ePMCU_request;

/* incoming requests for PMCU frequency control */
typedef enum {
  PMCU_SWITCH_TO_HIGH_INIT,
  PMCU_SWITCH_TO_HIGH,
  PMCU_SWITCH_TO_LOW,
  PMCU_SWITCH_TO_NONE
} ePMCU_switch_freq;

/* PMCU detectors/notifiers functions */
typedef int (*PMCU_detector_f)(void *ctx, ePMCU_request request);
typedef int (*PMCU_notifier_f)(void *ctx, ePMCU_request request);

/* PMCU client */
typedef struct {
  mtlk_dlist_entry_t list_entry;
  void               *ctx;
  PMCU_detector_f    detector;
  PMCU_notifier_f    notifier;
  uint64             traffic_high;
  uint64             traffic_low;
  uint64             traffic_delta;
} PMCU_client_t, *pPMCU_client_t;

/* PMCU parameters */
typedef struct
{
  uint32 interval_low2high;
  uint32 interval_high2low;
  uint32 limit_lower;
  uint32 limit_upper;
} PMCU_params_t, mtlk_pcoc_params_t;

typedef struct _mtlk_pcoc_cfg_t
{
  mtlk_pcoc_params_t default_params;
} mtlk_pcoc_cfg_t;

/* interface functions */
int            wv_PMCU_Create(void);
int            wv_PMCU_Cleanup(void);
int            wv_PMCU_Client_Register(pPMCU_client_t pClient);
int            wv_PMCU_Client_Unregister(void *ctx);
PMCU_params_t* wv_PMCU_Get_Params(void);
int            wv_PMCU_Set_Params (const PMCU_params_t *params);
int            wv_PMCU_Switch_Freq(ePMCU_switch_freq freq);
int            wv_PMCU_Traffic_Report(void *ctx, uint64 total_traffic);
BOOL           wv_PMCU_Is_High(void);
BOOL           wv_PMCU_is_enabled_adm(void);
int            wv_PMCU_set_enabled_adm(const BOOL is_enabled);
int            wv_PMCU_set_enabled_op(const BOOL is_enabled);
BOOL           wv_PMCU_is_active(void);
uint8          wv_PMCU_get_traffic_state(void);

#endif /*#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT*/
#endif /* __MTLK_PCOC_H__ */
