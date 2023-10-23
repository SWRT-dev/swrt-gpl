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
 * FW RECOVERY header file
 *
 *
 */
#ifndef __FW_RECOVERY_H__
#define __FW_RECOVERY_H__


typedef enum {
  RCVRY_TYPE_NONE,
  RCVRY_TYPE_FAST,
  RCVRY_TYPE_FULL,
  RCVRY_TYPE_UNRECOVARABLE_ERROR,
  RCVRY_TYPE_DUT,
  RCVRY_TYPE_UNDEF,
  RCVRY_TYPE_IGNORE,
  RCVRY_TYPE_DBG,
  RCVRY_TYPE_LAST
} wave_rcvry_type_e;

typedef enum {
  RCVRY_NO_SCAN,
  RCVRY_FG_SCAN,
  RCVRY_BG_SCAN
} wave_rcvry_scan_type_e;

typedef enum {
  RCVRY_MODE_DISABLED,
  RCVRY_MODE_ENABLED_DONT_PANIC,
  RCVRY_MODE_ENABLED_WITH_PANIC,
  RCVRY_MODE_LAST
} wave_rcvry_mode_e;

/* Error types to use for payload of MAC Error event */
enum {
  WAVE_RCVRY_ERRORTYPE_RESERVED,              /* Reserved (not used) */
  WAVE_RCVRY_ERRORTYPE_MAC_EXTERNAL_ASSERT,   /* MAC External Assert */
  WAVE_RCVRY_ERRORTYPE_MAC_EXCEPTION,         /* MAC Exception Error */
  WAVE_RCVRY_ERRORTYPE_MAC_FATAL,             /* MAC Fatal Error */
  WAVE_RCVRY_ERRORTYPE_LAST                   /* Last (not used) */
};

#define WAVE_RCVRY_FW_ASSERT_IND_TIMEOUT  2000 /* Assert Indication waiting timeout (msec) */
#define WAVE_RCVRY_MODE_VAL_DEFAULT       RCVRY_MODE_ENABLED_DONT_PANIC    /* default value of Recovery mode settings - def (recovery ON, NO kernel panic) */
#define WAVE_RCVRY_KERNEL_PANIC_DEFAULT   0    /* default value of kernel panic on fw assert - def disabled */
#define WAVE_RCVRY_PRIMARY_VAP_RADIO_IDX  0    /* Master VAP of Radio 0 executes Master Recovery task */

#define WAVE_RCVRY_FW_DUMP_PROC_NAME      "fw_dump"
#define WAVE_RCVRY_STATS_PROC_NAME        "recovery_stats"

/* configuration structure */
typedef struct {
  wave_rcvry_mode_e rcvry_mode;      /* '0' rcvry disabled, '1' rcvry enabled no kernel panic, '2' rcvry enabled kernel panic enabled */
  uint8 rcvry_fast_cnt;         /* number of configured consecutive Fast Recovery */
  uint8 rcvry_full_cnt;         /* number of configured consecutive Full Recovery */
  uint8 fw_dump;                /* enable FW dumps evacuation after crash */
  uint32 rcvry_timer_value;     /* Fail timer value (sec or ms) used to clear current Fast and
                                   Full Recovery counters upon timer expiration and clear dump data */
} wave_rcvry_cfg_t;

typedef struct wave_rcvry_task_ctx wave_rcvry_task_ctx_t;

int wave_rcvry_card_add(void *mmb_base, void *hw_ctx);
void wave_rcvry_card_cleanup(const void *mmb_base);
int wave_rcvry_init(void);
void wave_rcvry_cleanup(void);
int wave_rcvry_init_finalize(mtlk_hw_t *hw_ctx);
void wave_rcvry_cleanup_finalize(mtlk_hw_t *hw_ctx);
#ifdef CONFIG_WAVE_DEBUG
void wave_rcvry_set_to_dbg_mode (mtlk_vap_handle_t vap_handle);
#endif

int wave_rcvry_cfg_set(mtlk_hw_t *hw_ctx, wave_rcvry_cfg_t *cfg);
int wave_rcvry_cfg_get(mtlk_hw_t *hw_ctx, wave_rcvry_cfg_t *cfg);

int wave_rcvry_reset(wave_rcvry_task_ctx_t **rcvry_handle);

wave_rcvry_type_e wave_rcvry_type_current_get(const mtlk_hw_t *hw_ctx);
void wave_rcvry_type_forced_set(const mtlk_hw_t *hw_ctx, wave_rcvry_type_e type);
void wave_rcvry_restart_scan_set(const mtlk_hw_t *hw_ctx,
                                 const mtlk_vap_manager_t *vap_manager,
                                 const BOOL value);
BOOL wave_rcvry_restart_scan_get(const mtlk_hw_t *hw_ctx,
                                 const mtlk_vap_manager_t *vap_manager);
wave_rcvry_scan_type_e wave_rcvry_scan_type_current_get(const mtlk_hw_t *hw_ctx,
                                                        const mtlk_vap_manager_t *vap_manager);
void wave_rcvry_chandef_current_get(const mtlk_hw_t *hw_ctx,
                                    const mtlk_vap_manager_t *vap_manager,
                                    mtlk_handle_t *chan_def);
void wave_rcvry_chanparam_set(const mtlk_hw_t *hw_ctx,
                              const mtlk_vap_manager_t *vap_manager,
                              const mtlk_handle_t *value);
mtlk_handle_t * wave_rcvry_chanparam_get(const mtlk_hw_t *hw_ctx,
                                         const mtlk_vap_manager_t *vap_manager);
int wave_rcvry_task_initiate(mtlk_hw_t *hw_ctx);

void wave_rcvry_no_dump_event (const void *mmb_base);
int wave_rcvry_process_msg_unrecovarable_error(const void *mmb_base);

BOOL wave_rcvry_is_configured(const mtlk_hw_t *hw_ctx);
BOOL wave_rcvry_fw_dump_in_progress_get(const mtlk_hw_t *hw_ctx);
#ifdef UNUSED_CODE
void wave_rcvry_mac_fatal_pending_set(const mtlk_hw_t *hw_ctx, const BOOL value);
#endif /* UNUSED_CODE */
BOOL wave_rcvry_mac_fatal_pending_get(const mtlk_hw_t *hw_ctx);
void wave_rcvry_pci_probe_error_set(const void *mmb_base, const BOOL value);
BOOL wave_rcvry_pci_probe_error_get(const void *mmb_base);
BOOL wave_rcvry_enabled_get(const void *mmb_base);
void wave_rcvry_mac_hang_evt_reset(mtlk_hw_t *hw_ctx, uint32 cpu_no);
void wave_rcvry_mac_hang_evt_reset_all(mtlk_hw_t *hw_ctx);
void wave_rcvry_mac_hang_evt_set(mtlk_hw_t *hw_ctx, uint32 cpu_no);
void wave_rcvry_mac_hang_evt_set_all(mtlk_hw_t *hw_ctx);
int wave_rcvry_mac_hang_evt_is_set(mtlk_hw_t *hw_ctx, uint32 cpu_no);
int wave_rcvry_mac_hang_evt_wait(mtlk_hw_t *hw_ctx, uint32 cpu_no);
void wave_rcvry_process_mac_fatal(mtlk_hw_t *hw_ctx, const APP_FATAL *data);
void wave_rcvry_process_mac_exception(mtlk_hw_t *hw_ctx, const APP_FATAL *data);
void wave_rcvry_fwdump_handle (const void *mmb_base);
BOOL wave_rcvry_krnl_panic_enabled (const void *mmb_base);

void wave_rcvry_fw_steady_set (const void *mmb_base, const BOOL value);
BOOL wave_rcvry_fw_steady_get (const void *mmb_base);
#endif /* __FW_RECOVERY_H__ */
