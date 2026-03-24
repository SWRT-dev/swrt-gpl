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
 * $Id: ta.h 13212 2012-07-04 11:45:27Z sumilovs $
 *
 * Traffic Analyzer External Interface
 *
 */

#ifndef __MTLK_TA_H__
#define __MTLK_TA_H__

#define TA_CRIT_SIGN        0x4352BEAD
#define TA_SIGN             0x5441BEAD
#define TA_DEF_TMR_TICKS        2
#define TA_COC_TICK_MSEC        100

/* TA criterion ID type */
typedef enum _ta_crit_id_t {
  TA_CRIT_ID_COC,
  TA_CRIT_ID_ERP,
  TA_CRIT_ID_LAST
} ta_crit_id_t;

#define TA_CRIT_NUM TA_CRIT_ID_LAST

/* TA callback function pointer */
typedef void (*ta_crit_clb_t)(mtlk_handle_t clb_ctx, mtlk_handle_t result);

/* AOCS configuration structure */
typedef struct _ta_crit_erp_cfg_t {
  uint32 interval;
  uint32 coeff;
} ta_crit_erp_cfg_t;

/* COC configuration structure */
typedef struct _ta_crit_coc_cfg_t {
  uint32 interval;
  uint32 coeff;
  uint8  antennas; /* current antennas number */
} ta_crit_coc_cfg_t;

typedef struct _ta_crit_erp_result_t {
  uint64 max_rx;
  uint64 max_tx;
  uint32 sta_num;
} ta_crit_erp_result_t;

/** Interface functions */

mtlk_handle_t __MTLK_IFUNC mtlk_ta_create(mtlk_vap_manager_t *vap_manager);
void    __MTLK_IFUNC mtlk_ta_delete(mtlk_handle_t ta_handle);

int    __MTLK_IFUNC mtlk_ta_erp_cfg(mtlk_handle_t ta_handle, ta_crit_erp_cfg_t *cfg, BOOL restart_fn);
int     __MTLK_IFUNC mtlk_ta_coc_cfg(mtlk_handle_t ta_handle, ta_crit_coc_cfg_t *coc_cfg);

uint32  __MTLK_IFUNC mtlk_ta_get_timer_resolution_ms(mtlk_handle_t ta_handle);

void    __MTLK_IFUNC mtlk_ta_on_connect(mtlk_handle_t ta_handle, IEEE_ADDR *sta_addr, mtlk_vap_handle_t vap_handle);
void    __MTLK_IFUNC mtlk_ta_on_disconnect(mtlk_handle_t ta_handle, const IEEE_ADDR *sta_addr);

void    __MTLK_IFUNC mtlk_ta_crit_start(mtlk_handle_t crit_handle);
void    __MTLK_IFUNC mtlk_ta_crit_stop(mtlk_handle_t crit_handle);

mtlk_handle_t __MTLK_IFUNC mtlk_ta_crit_register(mtlk_handle_t ta_handle, ta_crit_id_t crit_id, ta_crit_clb_t clb, mtlk_handle_t clb_ctx);
void          __MTLK_IFUNC mtlk_ta_crit_unregister(mtlk_handle_t crit_handle);


mtlk_vap_handle_t __MTLK_IFUNC mtlk_ta_get_vap_handle(mtlk_handle_t ta_handle);

int ta_timer(mtlk_handle_t ta_handle, const void *data,  uint32 data_size);

#ifdef CONFIG_WAVE_DEBUG
struct _mtlk_ta_debug_info_cfg_t;
void __MTLK_IFUNC mtlk_ta_get_debug_info(mtlk_handle_t ta_handle, struct _mtlk_ta_debug_info_cfg_t *debug_info);
uint32  __MTLK_IFUNC mtlk_ta_get_timer_resolution_ticks(mtlk_handle_t ta_handle);
int     __MTLK_IFUNC mtlk_ta_set_timer_resolution_ticks(mtlk_handle_t ta_handle, uint32 timer_resolution);
#endif
void __MTLK_IFUNC mtlk_ta_on_rcvry_isol(mtlk_handle_t ta_handle);
void __MTLK_IFUNC mtlk_ta_on_rcvry_restore(mtlk_handle_t ta_handle);

#endif

