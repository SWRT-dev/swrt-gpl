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
 * FW RECOVERY
 *
 *
 */
#include "mtlkinc.h"
#include "mtlk_vap_manager.h"
#include "mtlk_df.h"
#include "mtlk_coreui.h"
#include "core.h"
#include "ta.h"
#include "hw_mmb.h"
#include "mtlkhal.h"
#include "fw_recovery.h"
#include "nlmsgs.h"
#include "mac80211.h"
#include "cfg80211.h"
#include "mtlk_dfg.h"
#include "mtlk_df_priv.h"
#include "vendor_cmds.h"
#include "mtlk_df_nbuf.h"

#define LOG_LOCAL_GID   GID_RECOVERY
#define LOG_LOCAL_FID   0

#define RCVRY_SIGN  0x5259BEAD /* RY */

/* Recovery WSS statistics */
typedef enum {
  RCVRY_NOF_FAST_RCVRY_PROCESSED,   /* number of Fast Recovery executed successfully */
  RCVRY_NOF_FULL_RCVRY_PROCESSED,   /* number of Full Recovery executed successfully */
  RCVRY_NOF_FAST_RCVRY_FAILED,      /* number of Fast Recovery failed */
  RCVRY_NOF_FULL_RCVRY_FAILED,      /* number of Full Recovery failed */
  RCVRY_CNT_LAST
} rcvry_cnt_id_e;

/* Map of Recovery WSS statistics */
static const uint32 wave_rcvry_wss_cnt_id_map[] =
{
  MTLK_WWSS_WLAN_STAT_ID_NOF_FAST_RCVRY_PROCESSED,  /* RCVRY_NOF_FAST_RCVRY_PROCESSED */
  MTLK_WWSS_WLAN_STAT_ID_NOF_FULL_RCVRY_PROCESSED,  /* RCVRY_NOF_FULL_RCVRY_PROCESSED */
  MTLK_WWSS_WLAN_STAT_ID_NOF_FAST_RCVRY_FAILED,     /* RCVRY_NOF_FAST_RCVRY_FAILED */
  MTLK_WWSS_WLAN_STAT_ID_NOF_FULL_RCVRY_FAILED,     /* RCVRY_NOF_FULL_RCVRY_FAILED */
};

/* VAP info (per radio instance) */
typedef struct {
  uint8             vap_number;             /* VAP global number */
  int               was_connected;          /* VAP state */
  mtlk_vap_handle_t vap_handle;             /* pointer to handle */
  mtlk_osal_mutex_t *slave_sync_mutex;      /* mutex to wait until serializer of Slave VAP is blocked */
  mtlk_osal_mutex_t *slave_release_mutex;   /* mutex to release serializer of Slave VAP after Recovery is done */
} wave_rcvry_vap_info_t;

/* Scan info (per radio instance) */
typedef struct {
  struct mtlk_chan_def   chan_def_current;  /* current channel configuration */
  wave_rcvry_scan_type_e scan_type_current; /* current Scan type */
} wave_rcvry_scan_info_t;

/* CAC info (per radio instance) */
typedef struct {
  struct set_chan_param_data cpd_current;   /* current channel parameters */
} wave_rcvry_cac_info_t;

typedef struct {
  mtlk_vap_manager_t     *vap_manager;      /* pointer to context of VAP manager */
  mtlk_vap_handle_t      master_vap_handle; /* pointer to context of Master VAP handler */
  uint32                 max_vaps_number;   /* maximal number of VAPs */
  wave_rcvry_vap_info_t  **vap_info;        /* NULL-terminated array of pointers to VAPs info */
  wave_rcvry_vap_info_t  *vap_info_array;   /* array of VAPs info */
  wave_rcvry_scan_info_t *scan_info;        /* Scan info */
  wave_rcvry_cac_info_t  *cac_info;         /* CAC info */

  atomic_t               is_restart_scan;   /* restart the scan in case of Fast or Full Recovery */
} wave_rcvry_radio_ctx_t;

/* Recovery task context (per card instance) */
struct wave_rcvry_task_ctx {
  mtlk_osal_spinlock_t    lock;                                     /* Recovery might be initiated from serializer and ISR contexts,
                                                                       thus Recovery structure must be protected from simultaneous access */
  uint32                  signature;
  uint32                  max_radios_number;                        /* maximal number of radios */
  wave_rcvry_radio_ctx_t  radio_ctx[WAVE_CARD_RADIO_NUM_MAX];       /* context for Recovery radio */

  mtlk_osal_mutex_t       fw_dump_mutex;                            /* mutex for sharing fw_dump_data */
  mtlk_osal_mutex_t       slave_sync_mutex;                         /* mutex to wait until serializers of Slave VAPs are blocked */
  mtlk_osal_mutex_t       slave_release_mutex[2];                   /* mutex to release serializers of Slave VAPs after Recovery is done */
  uint8                   slave_release_mutex_idx;                  /* mutex index */

  wave_rcvry_cfg_t        cfg;                                      /* Recovery configuration */

  wave_rcvry_type_e       rcvry_type_current;                       /* current Recovery type */
  wave_rcvry_type_e       rcvry_type_forced;                        /* forced Recovery type */
  uint8                   rcvry_none_cnt;                           /* number of executed None Recovery */
  uint8                   rcvry_fast_cnt;                           /* number of executed consecutive Fast Recovery */
  uint8                   rcvry_full_cnt;                           /* number of executed consecutive Full Recovery */

  mtlk_osal_timer_t       rcvry_timer;                               /* timer that upon expiration resets Recovery counters and fw dump */

  mtlk_wss_t              *wss;                                     /* Recovery WSS statistics */
  mtlk_wss_cntr_handle_t  *wss_cnt[RCVRY_CNT_LAST];

  uint8                   mac_error_type;                           /* MAC Error type (used only if Recovery is Off) */

  atomic_t                is_fw_dump_in_progress;                   /* FW dumps evacuation is/isn't in progress */
  atomic_t                is_mac_fatal_pending;                     /* event MAC Fatal or Exception received, but not processed yet */
  BOOL                    is_pci_probe_error;                       /* Recovery is executing in _pci_probe */
  BOOL                    is_rcvry_failed;                          /* Execution of Fast or Full Recovery failed */
  BOOL                    is_fw_steady_state;
  /* TODO: reduce number (to 1) of objects for MAC-hang event waiting */
  mtlk_osal_event_t   mac_hang_evt[MAX_NUM_OF_FW_CORES];            /* object for MAC-hang event waiting */
  void                    *fw_dump_data;                            /* FW dump data */
  MTLK_DECLARE_INIT_STATUS;                                         /* container for initialization steps */
};

/* per card instance */
typedef struct {
  void                    *mmb_base;
  mtlk_hw_t               *hw_ctx;                       /* pointer to HW-context */
  mtlk_df_proc_fs_node_t  *proc_node_card;
  mtlk_df_proc_fs_node_t  *proc_node_fw;
  char                    card_dir_name[6];              /* as "cardX\0" */
  wave_fw_dump_info_t     *info;
  int                     nof_files;
  int                     nof_files_done;
  uint8                   card_idx;                      /* internal index of PCI-card */
  wave_rcvry_task_ctx_t   rcvry_ctx;                     /* context for Recovery task */
} wave_rcvry_card_cfg_t;

int wave_rcvry_reset (wave_rcvry_task_ctx_t ** rcvry_handle)
{

  wave_rcvry_task_ctx_t *rcvry_ctx = *rcvry_handle;

  MTLK_ASSERT(NULL != rcvry_ctx);
  MTLK_ASSERT(RCVRY_SIGN == rcvry_ctx->signature);

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  rcvry_ctx->rcvry_none_cnt = 0;
  rcvry_ctx->rcvry_fast_cnt = 0;
  rcvry_ctx->rcvry_full_cnt = 0;
  mtlk_osal_lock_release(&rcvry_ctx->lock);

  mtlk_osal_mutex_acquire(&rcvry_ctx->fw_dump_mutex);
  if (rcvry_ctx->fw_dump_data != NULL){
    mtlk_osal_vmem_free(rcvry_ctx->fw_dump_data);
    rcvry_ctx->fw_dump_data = NULL;
  }
  mtlk_osal_mutex_release(&rcvry_ctx->fw_dump_mutex);

  ILOG1_V("Recovery timer finished");
  return 0;
}

static uint32 _rcvry_timer_handler (mtlk_osal_timer_t *timer, mtlk_handle_t data)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = (wave_rcvry_task_ctx_t *)data;
  mtlk_vap_handle_t master_vap_handle;

  MTLK_ASSERT(NULL != rcvry_ctx);
  MTLK_ASSERT(RCVRY_SIGN == rcvry_ctx->signature);

  master_vap_handle = rcvry_ctx->radio_ctx[WAVE_RCVRY_PRIMARY_VAP_RADIO_IDX].master_vap_handle;

  _mtlk_df_user_invoke_core_async(mtlk_vap_get_df(master_vap_handle), WAVE_RCVRY_RESET, &rcvry_ctx, sizeof(rcvry_ctx), NULL, 0);
  return 0;
}

#if (IWLWAV_RTLOG_MAX_DLEVEL >= IWLWAV_RTLOG_WARNING_DLEVEL)
static inline char *__rcvry_type_to_string (wave_rcvry_type_e rcvry_type)
{
  switch (rcvry_type) {
    case RCVRY_TYPE_NONE:
      return "NONE";
    case RCVRY_TYPE_FAST:
      return "FAST";
    case RCVRY_TYPE_FULL:
      return "FULL";
    case RCVRY_TYPE_UNRECOVARABLE_ERROR:
      return "UNRECOVARABLE_ERROR";
    case RCVRY_TYPE_DUT:
      return "DUT";
    case RCVRY_TYPE_UNDEF:
      return "UNDEF";
    case RCVRY_TYPE_DBG:
      return "DBG";
    default:
      ELOG_D("Unknown Recovery type 0x%04X", (uint32)rcvry_type);
  }
  return "Unknown Recovery type";
}
#endif

/* framework for FW dump evacuation */

/* per driver instance */
typedef struct {
  mtlk_osal_spinlock_t    lock;
  wave_rcvry_card_cfg_t   *p_card[MTLK_MAX_HW_ADAPTERS_SUPPORTED];
  mtlk_df_proc_fs_node_t  *proc_node_drv;
} wave_rcvry_proc_descr_t;

/* Recovery Configuration structure */
typedef struct {
  wave_rcvry_proc_descr_t proc_descr;
  atomic_t rcvry_type_global;           /* global recovery type */
  MTLK_DECLARE_INIT_STATUS;             /* container for initialization steps */
} wave_rcvry_t;

static wave_rcvry_t wave_rcvry;

static inline void __rcvry_type_global_set (wave_rcvry_type_e type)
{
  atomic_set(&wave_rcvry.rcvry_type_global, (uint32)type);
}

static inline wave_rcvry_type_e __rcvry_type_global_get (void)
{
  return (wave_rcvry_type_e)atomic_read(&wave_rcvry.rcvry_type_global);
}

static inline void __rcvry_mac_error_type_set (wave_rcvry_task_ctx_t *rcvry_ctx, uint8 mac_error_type)
{
  rcvry_ctx->mac_error_type = mac_error_type;
}

static inline uint8 __rcvry_mac_error_type_get (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  return rcvry_ctx->mac_error_type;
}

static wave_rcvry_card_cfg_t *_rcvry_card_ctx_get (const void *mmb_base)
{
  int i;
  wave_rcvry_proc_descr_t *proc_descr = &wave_rcvry.proc_descr;

  for (i = 0; i < MTLK_MAX_HW_ADAPTERS_SUPPORTED; i++) {
    wave_rcvry_card_cfg_t *pcard = proc_descr->p_card[i];
    if (pcard && (mmb_base == pcard->mmb_base))
      return pcard;
  }
  return NULL;
}

static wave_rcvry_card_cfg_t *_rcvry_card_hw_ctx_get (const mtlk_hw_t *hw_ctx)
{
  int i;
  wave_rcvry_proc_descr_t *proc_descr = &wave_rcvry.proc_descr;

  for (i = 0; i < MTLK_MAX_HW_ADAPTERS_SUPPORTED; i++) {
    wave_rcvry_card_cfg_t *pcard = proc_descr->p_card[i];
    if (pcard && (hw_ctx == pcard->hw_ctx))
      return pcard;
  }
  return NULL;
}

static wave_rcvry_task_ctx_t *_rcvry_task_ctx_get (const mtlk_hw_t *hw_ctx)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard)
    return &pcard->rcvry_ctx;
  else
    ELOG_V("Recovery context not found");

  return NULL;
}

static inline void __rcvry_type_current_set (wave_rcvry_task_ctx_t *rcvry_ctx, wave_rcvry_type_e type);

#ifdef CONFIG_WAVE_DEBUG
void wave_rcvry_set_to_dbg_mode (mtlk_vap_handle_t vap_handle){
  mtlk_hw_t * hw = mtlk_vap_get_hw(vap_handle);
  wave_rcvry_task_ctx_t * rcvry_ctx = _rcvry_task_ctx_get(hw);
  if (rcvry_ctx) {
    __rcvry_type_current_set(rcvry_ctx, RCVRY_TYPE_DBG);
  }
}
#endif

static wave_rcvry_radio_ctx_t *_rcvry_radio_ctx_get (wave_rcvry_task_ctx_t *rcvry_ctx,
                                                     const mtlk_vap_manager_t *vap_manager)
{
  unsigned i;

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    wave_rcvry_radio_ctx_t *radio_ctx = &rcvry_ctx->radio_ctx[i];
    if (vap_manager == radio_ctx->vap_manager)
      return radio_ctx;
  }
  return NULL;
}

static mtlk_error_t __unified_rcvry_proc_entry_read_fill_header (char *buff, size_t size, wave_rcvry_card_cfg_t *pcard) {
  int i, sprintf_written = 0;
  char* buff_tmp = buff;

  if (buff == NULL)
    return MTLK_ERR_PARAMS;

  sprintf_written = mtlk_snprintf(buff_tmp, (size -= sprintf_written), "INTL\n");
  if (sprintf_written <= 0 || (size_t)sprintf_written >= size)
    return MTLK_ERR_UNKNOWN;
  buff_tmp += sprintf_written;

  for (i = 0; i < pcard->nof_files; ++i) {
    if (0 == pcard->info[i].io_size) {
      continue; /* skip */
    }
    sprintf_written = mtlk_snprintf(buff_tmp, (size -= sprintf_written), "%s\n%X\n", pcard->info[i].name, pcard->info[i].io_size);
    if (sprintf_written <= 0 || (size_t)sprintf_written >= size)
      return MTLK_ERR_UNKNOWN;
    buff_tmp += sprintf_written;
  }
  sprintf_written = mtlk_snprintf(buff_tmp, (size -= sprintf_written), "<<<<");
  if (sprintf_written <= 0 || (size_t)sprintf_written >= size)
    return MTLK_ERR_UNKNOWN;
  buff_tmp += sprintf_written;

  *buff_tmp = '\n'; /* replace the '\0' from the last sprinf with '\n\ */

  return MTLK_ERR_OK;
}

static int _unified_rcvry_proc_entry_read (char *page, off_t off, int count, void *data)
{
  uint32 header_size, total_size, u_offset, u_count;
  wave_rcvry_card_cfg_t *pcard = mtlk_df_proc_entry_get_df(data);
  void **dump_file_p = &pcard->rcvry_ctx.fw_dump_data;
  mtlk_osal_mutex_t *fw_dump_mutex = &pcard->rcvry_ctx.fw_dump_mutex;
  int res = 0;

  if (!capable(CAP_SYS_PACCT)) {
    ELOG_V("fw_recovery:unified_rcvry_proc_entry_read :Permission denied");
    res = -EPERM;
    goto end;
  }

  mtlk_osal_mutex_acquire(fw_dump_mutex);
  if (*dump_file_p == NULL) {
    goto mutex_release;
  }
  mtlk_osal_mutex_release(fw_dump_mutex);

  mtlk_df_proc_entry_get_io_range(data, &header_size, &total_size);
  if (header_size == 0 || total_size == 0 || count <= 0 || off < 0 || (uint32)off >= total_size)
    goto end;

  u_offset = (uint32)off;
  u_count = (uint32)count;

  if (u_offset < header_size) {
    /* create and print the header */
    if (u_offset == 0 && u_count >= header_size) {
      mtlk_error_t ret = __unified_rcvry_proc_entry_read_fill_header(page, u_count, pcard);
      if (ret != MTLK_ERR_OK) {
        ELOG_V("Failed to fill the fw dump header\n");
        goto end;
      }
      res = header_size;
    } else {
      int to_write;
      mtlk_error_t ret;
      char* header_buff = mtlk_osal_mem_alloc(header_size + 1, LQLA_MEM_TAG_FW_RECOVERY);
      if (!header_buff) {
        ELOG_V("Failed to allocate memory for dump file header\n");
        goto end;
      }
      ret = __unified_rcvry_proc_entry_read_fill_header(header_buff, header_size + 1, pcard);
      if (ret != MTLK_ERR_OK) {
        mtlk_osal_mem_free(header_buff);
        ELOG_V("Failed to fill the fw dump header\n");
        goto end;
      }

      to_write = MIN(header_size - u_offset, u_count);
      wave_memcpy(page, u_count, header_buff + u_offset, to_write);
      mtlk_osal_mem_free(header_buff);
      res = to_write;
    }
    goto end;
  } else { /* the offset is after the header */
    uint32 dump_pos = u_offset - header_size;
    uint32 to_write = 0;
    char *dump_file;

    total_size -= header_size;

    if (dump_pos >= total_size)
      goto end; /* EOF */

    mtlk_osal_mutex_acquire(fw_dump_mutex);
    if (*dump_file_p != NULL) {
      to_write = MIN(u_count, total_size - dump_pos);
      dump_file = (char*)(*dump_file_p);
      dump_file += dump_pos;
      wave_memcpy(page, u_count, dump_file, to_write);

      if (dump_pos + to_write >= total_size) {
        /* user reached end of dump_file, clean the stored dump_file */
        mtlk_osal_vmem_free(*dump_file_p);
        *dump_file_p = NULL;
      }

      res = to_write;
    }
    goto mutex_release;
  }

mutex_release:
  mtlk_osal_mutex_release(fw_dump_mutex);
end:
  return res;
}

static int __rcvry_stats_proc_entry_read_fill_buff (char *buff, size_t size, mtlk_wssa_drv_recovery_stats_t* stats)
{
  char *buff_tmp = buff;
  int written = 0;

  MTLK_ASSERT(stats != NULL);

#define __MAX_RCVRY_STAT_NUMBER 9999
  if (stats->FastRcvryProcessed > __MAX_RCVRY_STAT_NUMBER)
    stats->FastRcvryProcessed = __MAX_RCVRY_STAT_NUMBER;
  if (stats->FullRcvryProcessed > __MAX_RCVRY_STAT_NUMBER)
    stats->FullRcvryProcessed = __MAX_RCVRY_STAT_NUMBER;
  if (stats->FastRcvryFailed > __MAX_RCVRY_STAT_NUMBER)
    stats->FastRcvryFailed = __MAX_RCVRY_STAT_NUMBER;
  if (stats->FullRcvryFailed > __MAX_RCVRY_STAT_NUMBER)
    stats->FullRcvryFailed = __MAX_RCVRY_STAT_NUMBER;

#define __RCVRY_STATS_HEADER "Recovery statistics:\n"
#define __RCVRY_STATS_NUM_OF " : Number of "
#define __RCVRY_STATS_SUCCESS " recovery processed successfully"
#define __RCVRY_STATS_FAILED " recovery failed"

  written = mtlk_snprintf(buff_tmp, (size -= written), __RCVRY_STATS_HEADER);
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;
  written = mtlk_snprintf(buff_tmp += written, (size -= written), "        %u", stats->FastRcvryProcessed);
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;
  written = mtlk_snprintf(buff_tmp += written, (size -= written), __RCVRY_STATS_NUM_OF "FAST" __RCVRY_STATS_SUCCESS "\n");
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;

  written = mtlk_snprintf(buff_tmp += written, (size -= written), "        %u", stats->FullRcvryProcessed);
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;
  written = mtlk_snprintf(buff_tmp += written, (size -= written), __RCVRY_STATS_NUM_OF "FULL" __RCVRY_STATS_SUCCESS "\n");
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;

  written = mtlk_snprintf(buff_tmp += written, (size -= written), "        %u", stats->FastRcvryFailed);
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;
  written = mtlk_snprintf(buff_tmp += written, (size -= written), __RCVRY_STATS_NUM_OF "FAST" __RCVRY_STATS_FAILED "\n");
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;

  written = mtlk_snprintf(buff_tmp += written, (size -= written), "        %u", stats->FullRcvryFailed);
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;
  written = mtlk_snprintf(buff_tmp += written, (size -= written), __RCVRY_STATS_NUM_OF "FULL" __RCVRY_STATS_FAILED);
  if (written < 0 || (size_t)written >= size)
    return MTLK_ERR_UNKNOWN;
  *(buff_tmp += written) = '\n';

  return (buff_tmp - buff) + 1;
}

static int _rcvry_stats_proc_entry_read (char *page, off_t off, int count, void *data)
{
  wave_rcvry_card_cfg_t *pcard = mtlk_df_proc_entry_get_df(data);
  mtlk_hw_t *hw = pcard->hw_ctx;
  int res = 0;
  int total_size = 0;
  mtlk_wssa_drv_recovery_stats_t stats;

  if (count <= 0)
    goto end;

  total_size += sizeof(__RCVRY_STATS_HEADER) - 1;
  total_size += 4 * (sizeof(__RCVRY_STATS_NUM_OF) - 1);
  total_size += 2 * (sizeof(__RCVRY_STATS_SUCCESS) - 1);
  total_size += 2 * (sizeof(__RCVRY_STATS_FAILED) - 1);
  total_size += 4 * 4; /* FAST/FULL * 4 */
  total_size += 8 * 4; /* 8 spaces tabs * 4 lines */
  total_size += 4; /* 4 New Lines */
  total_size += 4 * 4; /* 4 (max 9999) recovery stats numbers */

  if (off >= total_size)
    goto end;

  wave_hw_get_recovery_stats(hw, &stats);

  if (off == 0 && count >= total_size) {
    res = __rcvry_stats_proc_entry_read_fill_buff(page, count, &stats);
    if (res < 0) {
      ELOG_V("Failed to fill buff with recovery statistics data\n");
      res = 0;
      goto end;
    }
  } else {
    char* stats_output_buff = mtlk_osal_mem_alloc(total_size, LQLA_MEM_TAG_FW_RECOVERY);
    if (!stats_output_buff) {
      ELOG_V("Failed to allocate memory for dump file header\n");
      goto end;
    }

    total_size = __rcvry_stats_proc_entry_read_fill_buff(stats_output_buff, total_size, &stats);
    if (total_size < 0) {
      ELOG_V("Failed to fill buff with recovery statistics data\n");
      mtlk_osal_mem_free(stats_output_buff);
      res = 0;
      goto end;
    }
    res = MIN(total_size - off, count);
    wave_memcpy(page, count, stats_output_buff + off, res);
    mtlk_osal_mem_free(stats_output_buff);
  }

end:
  return res;
}

static int _rcvry_stats_proc_entry_init (wave_rcvry_card_cfg_t *pcard, const char* name)
{
  ILOG1_S("Creating rcvry stats proc file %s", name);

  return mtlk_df_proc_node_add_io_entry(
         name,
         pcard->proc_node_fw,
         pcard,
         _rcvry_stats_proc_entry_read, /* no BCL read is supported */
         0, 0);
}

static int _unified_rcvry_proc_entry_init (wave_rcvry_card_cfg_t *pcard, const char* name, uint32 size, uint32 h_size)
{
  ILOG1_SDDD("Creating unified fw dump file %s, header size %d, total size %d (0x%X)",
            name, h_size, size, size);

  return mtlk_df_proc_node_add_io_entry(
         name,
         pcard->proc_node_fw,
         pcard,
         _unified_rcvry_proc_entry_read, /* no BCL read is supported */
         h_size, size);
}

#define WAVE_RCVRY_CARD_DIR_NAME "card"
#define WAVE_RCVRY_FW_DIR_NAME   "FW"

static void _rcvry_card_delete (wave_rcvry_card_cfg_t *pcard)
{
  int i;
  wave_rcvry_proc_descr_t *proc_descr = &wave_rcvry.proc_descr;

  MTLK_ASSERT(NULL != pcard);

  ILOG0_S("Deleting proc for card %s", pcard->card_dir_name);

  if (pcard->proc_node_fw) {
    ILOG1_S("deleting %s", WAVE_RCVRY_FW_DUMP_PROC_NAME);
    mtlk_df_proc_node_remove_entry(WAVE_RCVRY_FW_DUMP_PROC_NAME, pcard->proc_node_fw);

    ILOG1_S("deleting %s", WAVE_RCVRY_STATS_PROC_NAME);
    mtlk_df_proc_node_remove_entry(WAVE_RCVRY_STATS_PROC_NAME, pcard->proc_node_fw);

    ILOG0_V("deleting FW dir");
    mtlk_df_proc_node_delete(pcard->proc_node_fw);
  }

  if (pcard->proc_node_card) {
    ILOG0_V("deleting card dir");
    mtlk_df_proc_node_delete(pcard->proc_node_card);
  }

  mtlk_osal_lock_acquire(&proc_descr->lock);
  proc_descr->p_card[pcard->card_idx] = NULL;
  mtlk_osal_lock_release(&proc_descr->lock);

  for (i = 0; i < MAX_NUM_OF_FW_CORES; i++)
    mtlk_osal_event_cleanup(&pcard->rcvry_ctx.mac_hang_evt[i]);

  if (pcard->info != NULL)
    mtlk_osal_mem_free(pcard->info);

  mtlk_osal_mem_free(pcard);
}

static BOOL _wv_rcvry_is_enabled(wave_rcvry_task_ctx_t *rcvry_ctx){
  return (rcvry_ctx->cfg.rcvry_mode != RCVRY_MODE_DISABLED);
}

/* TODO: rework this function using MTLK_STEPS */
int wave_rcvry_card_add (void *mmb_base, void *hw_ctx)
{
  wave_rcvry_task_ctx_t *rcvry_ctx;
  int i, res = MTLK_ERR_OK;
  wave_rcvry_card_cfg_t *pcard = NULL;
  wave_fw_dump_info_t *info = NULL;
  wave_rcvry_proc_descr_t *proc_descr = &wave_rcvry.proc_descr;

  uint32 header_size = 0;
  uint32 dump_total_size = 0;
  char io_size_hex_buff[32];

  MTLK_ASSERT(NULL != mmb_base);
  MTLK_ASSERT(NULL != hw_ctx);

  mtlk_osal_lock_acquire(&proc_descr->lock);
  for (i = 0; i < MTLK_MAX_HW_ADAPTERS_SUPPORTED; i++) {
    if (proc_descr->p_card[i] == NULL) {
      pcard = mtlk_osal_mem_alloc(sizeof(wave_rcvry_card_cfg_t), LQLA_MEM_TAG_FW_RECOVERY);

      if (NULL == pcard) {
        ELOG_D("Failed to alloc memory for card %d", i);
        mtlk_osal_lock_release(&proc_descr->lock);
        return MTLK_ERR_NO_MEM;
      }
      memset(pcard, 0, sizeof(wave_rcvry_card_cfg_t));

      /* take the card slot */
      proc_descr->p_card[i] = pcard;
      pcard->card_idx = (uint8)i;

      break;
    }
  }
  mtlk_osal_lock_release(&proc_descr->lock);

  if (pcard == NULL) {
    ELOG_D("Max num of cards is reached %d", MTLK_MAX_HW_ADAPTERS_SUPPORTED);
    return MTLK_ERR_NO_MEM;
  }

  pcard->proc_node_card = NULL;
  pcard->proc_node_fw = NULL;

  pcard->mmb_base = mmb_base;
  pcard->hw_ctx = hw_ctx;
  mtlk_snprintf(pcard->card_dir_name, sizeof(pcard->card_dir_name), "%s%u", WAVE_RCVRY_CARD_DIR_NAME, pcard->card_idx);

  rcvry_ctx = &pcard->rcvry_ctx;

  /* Set default configuration */
  rcvry_ctx->cfg.rcvry_mode = (panic_on_fw_error[mtlk_hw_mmb_get_card_idx(hw_ctx)]) ?
                                  RCVRY_MODE_ENABLED_WITH_PANIC :
                                  RCVRY_MODE_ENABLED_DONT_PANIC ;
  rcvry_ctx->cfg.rcvry_fast_cnt = 10;
  rcvry_ctx->cfg.rcvry_full_cnt = 6;
  rcvry_ctx->cfg.rcvry_timer_value = 30 * 60 * MTLK_OSAL_MSEC_IN_SEC;
  rcvry_ctx->cfg.fw_dump = 1;

  mtlk_osal_mutex_init(&rcvry_ctx->fw_dump_mutex);

  ILOG1_DS("CID-%02x: Recovery is %s", mtlk_hw_mmb_get_card_idx(hw_ctx),
                                       _wv_rcvry_is_enabled(rcvry_ctx) ? "On" : "Off");

  __rcvry_mac_error_type_set(rcvry_ctx, WAVE_RCVRY_ERRORTYPE_MAC_EXTERNAL_ASSERT);

  for (i = 0; i < MAX_NUM_OF_FW_CORES; i++)
    mtlk_osal_event_init(&rcvry_ctx->mac_hang_evt[i]);

  ILOG1_S("Creating dir %s", pcard->card_dir_name);
  pcard->proc_node_card = mtlk_df_proc_node_create(pcard->card_dir_name, proc_descr->proc_node_drv);
  if (NULL == pcard->proc_node_card) {
    res = MTLK_ERR_NO_MEM;
    goto WAVE_RCVRY_CARD_ADD_ERR;
  }

  ILOG0_S("Creating dir %s", WAVE_RCVRY_FW_DIR_NAME);
  pcard->proc_node_fw = mtlk_df_proc_node_create(WAVE_RCVRY_FW_DIR_NAME, pcard->proc_node_card);
  if (NULL == pcard->proc_node_fw) {
    res = MTLK_ERR_UNKNOWN;
    goto WAVE_RCVRY_CARD_ADD_ERR;
  }

  pcard->nof_files = mtlk_hw_get_fw_dump_info(hw_ctx, &pcard->info);
  if (pcard->nof_files < 0) {
    res = MTLK_ERR_UNKNOWN;
    goto WAVE_RCVRY_CARD_ADD_ERR;
  }

  ILOG0_D("Number of files to create %d", pcard->nof_files);
  info = pcard->info;

  header_size += 6; /* magic + 2*LF */
  for (i = 0; i < pcard->nof_files; i++, info++, pcard->nof_files_done++) {
    /* Don't create files of zero size */
    if (0 == info->io_size) {
      continue; /* skip */
    }

    dump_total_size += info->io_size;
    mtlk_snprintf(io_size_hex_buff, sizeof(io_size_hex_buff), "%X", info->io_size);
    /* info->name is initialized in static table, hence it's NUL-terminated and cannot be NULL */
    header_size += wave_strlen(info->name, FW_NAME_MAX_LEN);
    header_size += wave_strlen(io_size_hex_buff, sizeof(io_size_hex_buff));
    header_size += 2; /* 2*LF */
  }

  header_size += 4; /* <<<< */

  dump_total_size += header_size;

  res = _unified_rcvry_proc_entry_init(pcard, WAVE_RCVRY_FW_DUMP_PROC_NAME, dump_total_size, header_size);
    if (MTLK_ERR_OK != res)
      goto WAVE_RCVRY_CARD_ADD_ERR;

  res = _rcvry_stats_proc_entry_init(pcard, WAVE_RCVRY_STATS_PROC_NAME);
  if (MTLK_ERR_OK != res)
    goto WAVE_RCVRY_CARD_ADD_ERR;

  return res;

WAVE_RCVRY_CARD_ADD_ERR:
  _rcvry_card_delete(pcard);
  return res;
}

void wave_rcvry_card_cleanup(const void *mmb_base)
{
  wave_rcvry_card_cfg_t *pcard;

  MTLK_ASSERT(NULL != mmb_base);

  pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard) {
    wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;

    if (rcvry_ctx->fw_dump_data != NULL){
      mtlk_osal_vmem_free(rcvry_ctx->fw_dump_data);
      rcvry_ctx->fw_dump_data = NULL;
    }
    mtlk_osal_mutex_cleanup(&rcvry_ctx->fw_dump_mutex);

    _rcvry_card_delete(pcard);
  }
}

static void _rcvry_proc_file_init (wave_rcvry_proc_descr_t *proc_descr)
{
  memset(proc_descr, 0, sizeof(*proc_descr));
  proc_descr->proc_node_drv = mtlk_dfg_get_drv_proc_node();
  mtlk_osal_lock_init(&proc_descr->lock);
}

static void _rcvry_proc_file_cleanup (wave_rcvry_proc_descr_t *proc_descr)
{
  mtlk_osal_lock_cleanup(&proc_descr->lock);
}

static BOOL _rcvry_is_initialized (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  uint32 sig;

  MTLK_ASSERT(NULL != rcvry_ctx);

  /*mtlk_osal_lock_acquire(&rcvry_ctx->lock);*/
  sig = rcvry_ctx->signature;
  /*mtlk_osal_lock_release(&rcvry_ctx->lock);*/

  if (RCVRY_SIGN != sig) {
    /*WLOG_D("Invalid Recovery signature %08x", sig);*/
    return FALSE;
  }
  return TRUE;
}

static struct wireless_dev *
_wv_pcard_get_wdev(wave_rcvry_card_cfg_t *pcard){
  struct wireless_dev *wdev = NULL;
  if (pcard) {
    wave_rcvry_task_ctx_t *rcvry_ctx;
    mtlk_vap_handle_t master_vap_handle;
    MTLK_ASSERT(pcard);
    rcvry_ctx = &pcard->rcvry_ctx;
    MTLK_ASSERT(rcvry_ctx->radio_ctx);
    master_vap_handle = rcvry_ctx->radio_ctx[WAVE_RCVRY_PRIMARY_VAP_RADIO_IDX].master_vap_handle;
    MTLK_ASSERT(master_vap_handle);
    wdev = mtlk_df_user_get_wdev(mtlk_df_get_user(mtlk_vap_get_df(master_vap_handle)));
  }
  return wdev;
}

static int
wv_ieee80211_vendor_event_rcvry(uint8 event_num, struct wireless_dev *wdev, const void * data, int data_len){
  mtlk_nbuf_t *evt_nbuf;

  MTLK_ASSERT(NULL != wdev);

  evt_nbuf = wv_cfg80211_vendor_event_alloc(wdev, data_len,
                                            event_num);
  if (!evt_nbuf)
  {
    ELOG_V("Malloc event fail");
    return MTLK_ERR_NO_MEM;
  }

  if (data != NULL && data_len > 0) {
    wave_memcpy(evt_nbuf->data + evt_nbuf->len, data_len, data, data_len);
    mtlk_df_nbuf_put(evt_nbuf, data_len);
  }

  wv_cfg80211_vendor_event(evt_nbuf);

  return MTLK_ERR_OK;
}

static int _rcvry_nl_send_msg (const uint8 event_id, wave_rcvry_card_cfg_t *pcard,
                               const void *data, size_t data_size,
                               const char *pcmd_str)
{
  int res = MTLK_ERR_OK;
  struct wireless_dev *wdev;

  if (_rcvry_is_initialized(&pcard->rcvry_ctx)){
    wdev = _wv_pcard_get_wdev(pcard);
    if (wdev) {
      res = wv_ieee80211_vendor_event_rcvry(event_id, wdev, data, data_size);
    } else {
      ILOG0_DS("CID-%02x: Unable to send %s using nl80211 - wdev is NULL", pcard->card_idx, pcmd_str);
      return res;
    }
  }
  else {
    ILOG0_DS("CID-%02x: Unable to send %s using nl80211 - Recovery not initialized", pcard->card_idx, pcmd_str);
    return res;
  }

  if (MTLK_ERR_OK == res)
    ILOG1_DS("CID-%02x: %s sent using nl80211", pcard->card_idx, pcmd_str);
  else
    WLOG_DSD("CID-%02x: Unable to send %s using nl80211 (res=%d)", pcard->card_idx, pcmd_str, res);

  return res;
}

/* steps for Recovery initialization and cleanup */
MTLK_INIT_STEPS_LIST_BEGIN(wave_rcvry)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry, PROCFILE_CFG)
MTLK_INIT_INNER_STEPS_BEGIN(wave_rcvry)
MTLK_INIT_STEPS_LIST_END(wave_rcvry);

void wave_rcvry_cleanup (void)
{
  /* Reset global Recovery type */
  __rcvry_type_global_set(RCVRY_TYPE_UNDEF);

  MTLK_CLEANUP_BEGIN(wave_rcvry, MTLK_OBJ_PTR(&wave_rcvry))
    MTLK_CLEANUP_STEP(wave_rcvry, PROCFILE_CFG, MTLK_OBJ_PTR(&wave_rcvry),
                      _rcvry_proc_file_cleanup, (&wave_rcvry.proc_descr));
  MTLK_CLEANUP_END(wave_rcvry, MTLK_OBJ_PTR(&wave_rcvry))
}

int wave_rcvry_init (void)
{
  /* Reset global Recovery type */
  __rcvry_type_global_set(RCVRY_TYPE_UNDEF);

  MTLK_INIT_TRY(wave_rcvry, MTLK_OBJ_PTR(&wave_rcvry))
    MTLK_INIT_STEP_VOID(wave_rcvry, PROCFILE_CFG, MTLK_OBJ_PTR(&wave_rcvry),
                        _rcvry_proc_file_init, (&wave_rcvry.proc_descr));
  MTLK_INIT_FINALLY(wave_rcvry, MTLK_OBJ_PTR(&wave_rcvry))
  MTLK_INIT_RETURN(wave_rcvry, MTLK_OBJ_PTR(&wave_rcvry), wave_rcvry_cleanup, ())
}

int wave_rcvry_cfg_set (mtlk_hw_t *hw_ctx, wave_rcvry_cfg_t *cfg)
{
  uint8 card_idx;
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (!rcvry_ctx)
    return MTLK_ERR_NOT_READY;

  card_idx = mtlk_hw_mmb_get_card_idx(hw_ctx);

  if (cfg->rcvry_timer_value > 86400) {
    ELOG_D("CID-%02x: Invalid Recovery timer value. "
           "Maximum allowed value is 86400 (1 day)", card_idx);
    return MTLK_ERR_PARAMS;
  }

  if (cfg->rcvry_mode >= RCVRY_MODE_LAST) {
    ELOG_D("CID-%02x: Invalid Recovery mode - '0' disabled, '1' enabled no kernel panic, '2' enabled with kernel panic", card_idx);
    return MTLK_ERR_PARAMS;
  }

  if (cfg->fw_dump > 1) {
    ELOG_D("CID-%02x: Invalid fw-dump configuration '0' disabled, '1' enabled", card_idx);
    return MTLK_ERR_PARAMS;
  }
  ILOG1_D("CID-%02x: Set Recovery counter configuration", card_idx);

  rcvry_ctx->cfg.rcvry_mode = cfg->rcvry_mode;
  rcvry_ctx->cfg.rcvry_fast_cnt = cfg->rcvry_fast_cnt;
  rcvry_ctx->cfg.rcvry_full_cnt = cfg->rcvry_full_cnt;
  rcvry_ctx->cfg.rcvry_timer_value = cfg->rcvry_timer_value * MTLK_OSAL_MSEC_IN_SEC;
  rcvry_ctx->cfg.fw_dump = cfg->fw_dump;

  return MTLK_ERR_OK;
}

int wave_rcvry_cfg_get (mtlk_hw_t *hw_ctx, wave_rcvry_cfg_t *cfg)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (!rcvry_ctx)
    return MTLK_ERR_NOT_READY;

  cfg->rcvry_mode = rcvry_ctx->cfg.rcvry_mode;
  cfg->rcvry_fast_cnt = rcvry_ctx->cfg.rcvry_fast_cnt;
  cfg->rcvry_full_cnt = rcvry_ctx->cfg.rcvry_full_cnt;
  cfg->rcvry_timer_value = rcvry_ctx->cfg.rcvry_timer_value / MTLK_OSAL_MSEC_IN_SEC;
  cfg->fw_dump = rcvry_ctx->cfg.fw_dump;

  return MTLK_ERR_OK;
}

static void _rcvry_task_vap_info_cleanup (wave_rcvry_radio_ctx_t *radio_ctx)
{
  if (radio_ctx->vap_info) {
    mtlk_osal_mem_free(radio_ctx->vap_info);
    radio_ctx->vap_info = NULL;
  }
  if (radio_ctx->vap_info_array) {
    mtlk_osal_mem_free(radio_ctx->vap_info_array);
    radio_ctx->vap_info_array = NULL;
  }
}

#define RCVRY_VAP_INFO_SIZE(n)      (sizeof(wave_rcvry_vap_info_t) * (size_t)(n))
#define RCVRY_VAP_INFO_PTR_SIZE(n)  (sizeof(wave_rcvry_vap_info_t *) * (size_t)(n + 1))

static int _rcvry_task_vap_info_init (wave_rcvry_radio_ctx_t *radio_ctx, uint32 radio_idx)
{
  uint16 oid = mtlk_vap_get_oid(radio_ctx->master_vap_handle);
  size_t vap_info_ptr_size = RCVRY_VAP_INFO_PTR_SIZE(radio_ctx->max_vaps_number);
  size_t vap_info_size = RCVRY_VAP_INFO_SIZE(radio_ctx->max_vaps_number);

  MTLK_UNREFERENCED_PARAM(oid);

  radio_ctx->vap_info = mtlk_osal_mem_alloc(vap_info_ptr_size, LQLA_MEM_TAG_FW_RECOVERY);
  if (!radio_ctx->vap_info) {
    ELOG_DD("CID-%04x: Failed to allocate memory for VAP-info pointers of Radio %d", oid, radio_idx);
    return MTLK_ERR_NO_MEM;
  }

  radio_ctx->vap_info_array = mtlk_osal_mem_alloc(vap_info_size, LQLA_MEM_TAG_FW_RECOVERY);
  if (!radio_ctx->vap_info_array) {
    ELOG_DD("CID-%04x: Failed to allocate memory for VAP-info of Radio %d", oid, radio_idx);
    return MTLK_ERR_NO_MEM;
  }

  memset(radio_ctx->vap_info, 0, vap_info_ptr_size);
  memset(radio_ctx->vap_info_array, 0, vap_info_size);
  return MTLK_ERR_OK;
}

static inline void __rcvry_task_vap_info_save (wave_rcvry_vap_info_t *vap_info, mtlk_vap_handle_t vap_handle)
{
  vap_info->vap_handle = vap_handle;
  vap_info->vap_number = mtlk_vap_get_id(vap_handle);
  vap_info->was_connected = mtlk_core_get_net_state(mtlk_vap_get_core(vap_handle));
  vap_info->slave_sync_mutex = NULL;
  vap_info->slave_release_mutex = NULL;

  ILOG1_DPDP("CID-%04x: VAP-info saved at address %p: VAP-number %d, vap_handle %p",
             mtlk_vap_get_oid(vap_info->vap_handle), vap_info, vap_info->vap_number, vap_info->vap_handle);
}

static inline void __rcvry_task_vap_info_prepare (wave_rcvry_radio_ctx_t *radio_ctx)
{
  uint32 i, j;
  wave_rcvry_vap_info_t *vap_info_temp;
  mtlk_vap_handle_t vap_handle;

  memset(radio_ctx->vap_info, 0, RCVRY_VAP_INFO_PTR_SIZE(radio_ctx->max_vaps_number));
  memset(radio_ctx->vap_info_array, 0, RCVRY_VAP_INFO_SIZE(radio_ctx->max_vaps_number));

  j = 0;
  vap_info_temp = radio_ctx->vap_info_array;

  if (MTLK_ERR_OK == mtlk_vap_manager_get_master_vap(radio_ctx->vap_manager, &vap_handle)) {
    __rcvry_task_vap_info_save(vap_info_temp, vap_handle);
    radio_ctx->vap_info[j++] = vap_info_temp++;
  }

  for (i = 0; i < radio_ctx->max_vaps_number - 1; i++)
  {
    if (MTLK_ERR_OK != mtlk_vap_manager_get_vap_handle(radio_ctx->vap_manager, i, &vap_handle))
      continue;

    __rcvry_task_vap_info_save(vap_info_temp, vap_handle);
    radio_ctx->vap_info[j++] = vap_info_temp++;
  }
  radio_ctx->vap_info[j] = NULL;
}

static void _rcvry_task_vap_info_prepare (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  uint32 i;

  for (i = 0; i < rcvry_ctx->max_radios_number; i++)
    __rcvry_task_vap_info_prepare(&rcvry_ctx->radio_ctx[i]);
}

static void _rcvry_task_scan_info_cleanup (wave_rcvry_radio_ctx_t *radio_ctx)
{
  if (radio_ctx->scan_info) {
    mtlk_osal_mem_free(radio_ctx->scan_info);
    radio_ctx->scan_info = NULL;
  }
}

static int _rcvry_task_scan_info_init (wave_rcvry_radio_ctx_t *radio_ctx, uint32 radio_idx)
{
  radio_ctx->scan_info = mtlk_osal_mem_alloc(sizeof(wave_rcvry_scan_info_t), LQLA_MEM_TAG_FW_RECOVERY);
  if (!radio_ctx->scan_info) {
    ELOG_DD("CID-%04x: Failed to allocate memory for Scan info of Radio %d",
            mtlk_vap_get_oid(radio_ctx->master_vap_handle), radio_idx);
    return MTLK_ERR_NO_MEM;
  }

  memset(radio_ctx->scan_info, 0, sizeof(wave_rcvry_scan_info_t));
  return MTLK_ERR_OK;
}

static void _rcvry_task_cac_info_cleanup (wave_rcvry_radio_ctx_t *radio_ctx)
{
  if (radio_ctx->cac_info) {
    mtlk_osal_mem_free(radio_ctx->cac_info);
    radio_ctx->cac_info = NULL;
  }
}

static int _rcvry_task_cac_info_init (wave_rcvry_radio_ctx_t *radio_ctx, uint32 radio_idx)
{
  radio_ctx->cac_info = mtlk_osal_mem_alloc(sizeof(wave_rcvry_cac_info_t), LQLA_MEM_TAG_FW_RECOVERY);
  if (!radio_ctx->cac_info) {
    ELOG_DD("CID-%04x: Failed to allocate memory for CAC info of Radio %d",
            mtlk_vap_get_oid(radio_ctx->master_vap_handle), radio_idx);
    return MTLK_ERR_NO_MEM;
  }

  memset(radio_ctx->cac_info, 0, sizeof(wave_rcvry_cac_info_t));
  return MTLK_ERR_OK;
}

static inline int __rcvry_task_scan_info_prepare (wave_rcvry_radio_ctx_t *radio_ctx)
{
  struct scan_support *ss;
  mtlk_core_t *mcore;

  memset(radio_ctx->scan_info, 0, sizeof(wave_rcvry_scan_info_t));

  mcore = mtlk_vap_get_core(radio_ctx->master_vap_handle);
  ss = __wave_core_scan_support_get(mcore);

  radio_ctx->scan_info->scan_type_current =
    mtlk_core_scan_is_running(mcore) ? ((ss->flags & SDF_BACKGROUND) ? RCVRY_BG_SCAN : RCVRY_FG_SCAN) : RCVRY_NO_SCAN;

  return mtlk_scan_recovery_get_channel(mcore, &radio_ctx->scan_info->chan_def_current);
}

static int _rcvry_task_scan_info_prepare (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  uint32 i;
  int res = MTLK_ERR_OK;

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    res = __rcvry_task_scan_info_prepare(&rcvry_ctx->radio_ctx[i]);
    if (MTLK_ERR_OK != res)
      break;
  }
  return res;
}

static int _rcvry_task_ctx_init (mtlk_hw_t *hw_ctx, wave_rcvry_task_ctx_t *rcvry_ctx)
{
  /* Only non-zero variables must be initialized by this function */
  uint32 i;
  int res = MTLK_ERR_OK;
  uint8 card_idx = mtlk_hw_mmb_get_card_idx(hw_ctx);
  wave_radio_descr_t *radio_descr = (wave_radio_descr_t*)wave_card_radio_descr_get(hw_ctx);

  MTLK_UNREFERENCED_PARAM(card_idx);

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);

  /* Get maximal number of radios */
  rcvry_ctx->max_radios_number = (uint32)radio_descr->num_radios;
  ILOG1_DD("CID-%02x: Maximal number of radios (%d) configured",
           card_idx, rcvry_ctx->max_radios_number);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++)
  {
    wave_rcvry_radio_ctx_t *radio_ctx = &rcvry_ctx->radio_ctx[i];

    /* Get pointers to contexts of VAP manager and Master VAP handler */
    radio_ctx->vap_manager = wave_radio_descr_vap_manager_get(radio_descr, i);
    if (radio_ctx->vap_manager)
      mtlk_vap_manager_get_master_vap(radio_ctx->vap_manager, &radio_ctx->master_vap_handle);

    if (!radio_ctx->vap_manager || !radio_ctx->master_vap_handle) {
      radio_ctx->vap_manager = NULL;
      radio_ctx->master_vap_handle = NULL;
      ELOG_DD("CID-%02x: Cancel Initiating Recovery: Master VAP not created for Radio %d", card_idx, i);
      res = MTLK_ERR_UNKNOWN;
      goto _err;
    }

    /* Get maximal number of VAPs */
    radio_ctx->max_vaps_number = mtlk_vap_manager_get_max_vaps_count(radio_ctx->vap_manager);
    ILOG1_DDD("CID-%04x: Maximal number of VAPs (%d) configured for Radio %d",
              mtlk_vap_get_oid(radio_ctx->master_vap_handle), radio_ctx->max_vaps_number, i);

    /* Create VAP-info */
    res = _rcvry_task_vap_info_init(radio_ctx, i);
    if (MTLK_ERR_OK != res)
      goto _err;

    /* Create Scan info */
    res = _rcvry_task_scan_info_init(radio_ctx, i);
    if (MTLK_ERR_OK != res)
      goto _err;

    /* Create CAC info */
    res = _rcvry_task_cac_info_init(radio_ctx, i);
    if (MTLK_ERR_OK != res)
      goto _err;
  }

  rcvry_ctx->rcvry_type_current = RCVRY_TYPE_UNDEF;
  rcvry_ctx->rcvry_type_forced = RCVRY_TYPE_UNDEF;

  mtlk_osal_lock_release(&rcvry_ctx->lock);

  rcvry_ctx->signature = RCVRY_SIGN;
  return res;

_err:
  mtlk_osal_lock_release(&rcvry_ctx->lock);
  return res;
}

static void _rcvry_task_ctx_cleanup (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  uint32 i;

  rcvry_ctx->signature = 0;

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);

  rcvry_ctx->rcvry_type_forced = RCVRY_TYPE_UNDEF;
  rcvry_ctx->rcvry_type_current = RCVRY_TYPE_UNDEF;

  for (i = 0; i < rcvry_ctx->max_radios_number; i++)
  {
    wave_rcvry_radio_ctx_t *radio_ctx = &rcvry_ctx->radio_ctx[i];

    /* Cleanup CAC info */
    _rcvry_task_cac_info_cleanup(radio_ctx);
    /* Cleanup Scan info */
    _rcvry_task_scan_info_cleanup(radio_ctx);
    /* Cleanup VAP-info */
    _rcvry_task_vap_info_cleanup(radio_ctx);
    /* Cleanup maximal number of VAPs */
    radio_ctx->max_vaps_number = 0;
    /* Cleanup pointers to contexts of VAP manager and Master VAP handler */
    radio_ctx->master_vap_handle = NULL;
    radio_ctx->vap_manager = NULL;
  }
  /* Cleanup maximal number of radios */
  rcvry_ctx->max_radios_number = 0;

  mtlk_osal_lock_release(&rcvry_ctx->lock);
}

/* steps for Recovery task context initialization and cleanup */
MTLK_INIT_STEPS_LIST_BEGIN(wave_rcvry_task_ctx)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_LOCK_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_SYNC_MUTEX_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RELEASE_MUTEX0_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RELEASE_MUTEX1_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_PARENT_GET)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_CNT_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_VARS_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RCVRY_TIMER_INIT)
MTLK_INIT_INNER_STEPS_BEGIN(wave_rcvry_task_ctx)
MTLK_INIT_STEPS_LIST_END(wave_rcvry_task_ctx)

void wave_rcvry_cleanup_finalize (mtlk_hw_t *hw_ctx)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (!rcvry_ctx)
    return;

  MTLK_CLEANUP_BEGIN(wave_rcvry_task_ctx, MTLK_OBJ_PTR(rcvry_ctx))
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RCVRY_TIMER_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_osal_timer_cleanup, (&rcvry_ctx->rcvry_timer));
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_VARS_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      _rcvry_task_ctx_cleanup, (rcvry_ctx));
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_CNT_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_wss_cntrs_close, (rcvry_ctx->wss, rcvry_ctx->wss_cnt, ARRAY_SIZE(rcvry_ctx->wss_cnt)));
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_wss_delete, (rcvry_ctx->wss));
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_PARENT_GET, MTLK_OBJ_PTR(rcvry_ctx),
                      MTLK_NOACTION, ());
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RELEASE_MUTEX1_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_osal_mutex_cleanup, (&rcvry_ctx->slave_release_mutex[1]));
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RELEASE_MUTEX0_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_osal_mutex_cleanup, (&rcvry_ctx->slave_release_mutex[0]));
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_SYNC_MUTEX_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_osal_mutex_cleanup, (&rcvry_ctx->slave_sync_mutex));
    MTLK_CLEANUP_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_LOCK_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_osal_lock_cleanup, (&rcvry_ctx->lock));
  MTLK_CLEANUP_END(wave_rcvry_task_ctx, MTLK_OBJ_PTR(rcvry_ctx))
}

int wave_rcvry_init_finalize (mtlk_hw_t *hw_ctx)
{
  mtlk_wss_t *hw_wss;
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (!rcvry_ctx)
    return MTLK_ERR_WRONG_CONTEXT;

  MTLK_INIT_TRY(wave_rcvry_task_ctx, MTLK_OBJ_PTR(rcvry_ctx))
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_LOCK_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                   mtlk_osal_lock_init, (&rcvry_ctx->lock));
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_SYNC_MUTEX_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                   mtlk_osal_mutex_init, (&rcvry_ctx->slave_sync_mutex));
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RELEASE_MUTEX0_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                   mtlk_osal_mutex_init, (&rcvry_ctx->slave_release_mutex[0]));
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RELEASE_MUTEX1_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                   mtlk_osal_mutex_init, (&rcvry_ctx->slave_release_mutex[1]));
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_PARENT_GET, MTLK_OBJ_PTR(rcvry_ctx),
                   wave_hw_mmb_get_prop, (hw_ctx, MTLK_HW_WSS, &hw_wss, sizeof(&hw_wss)));
    MTLK_INIT_STEP_EX(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                      mtlk_wss_create, (hw_wss, wave_rcvry_wss_cnt_id_map, ARRAY_SIZE(wave_rcvry_wss_cnt_id_map)),
                      rcvry_ctx->wss, rcvry_ctx->wss != NULL, MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_WSS_CNT_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                   mtlk_wss_cntrs_open, (rcvry_ctx->wss, wave_rcvry_wss_cnt_id_map, rcvry_ctx->wss_cnt, RCVRY_CNT_LAST));
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_VARS_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                   _rcvry_task_ctx_init, (hw_ctx, rcvry_ctx));
    MTLK_INIT_STEP(wave_rcvry_task_ctx, RCVRY_TASK_CTX_RCVRY_TIMER_INIT, MTLK_OBJ_PTR(rcvry_ctx),
                   mtlk_osal_timer_init, (&rcvry_ctx->rcvry_timer, _rcvry_timer_handler, HANDLE_T(rcvry_ctx)));
  MTLK_INIT_FINALLY(wave_rcvry_task_ctx, MTLK_OBJ_PTR(rcvry_ctx))
  MTLK_INIT_RETURN(wave_rcvry_task_ctx, MTLK_OBJ_PTR(rcvry_ctx), wave_rcvry_cleanup_finalize, (hw_ctx))
}

static inline void __rcvry_type_current_set (wave_rcvry_task_ctx_t *rcvry_ctx, wave_rcvry_type_e type)
{
  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  rcvry_ctx->rcvry_type_current = type;
  mtlk_osal_lock_release(&rcvry_ctx->lock);
}

static inline wave_rcvry_type_e __rcvry_type_current_get (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  wave_rcvry_type_e res;

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  res = rcvry_ctx->rcvry_type_current;
  mtlk_osal_lock_release(&rcvry_ctx->lock);
  return res;
}

static inline void __rcvry_type_forced_set (wave_rcvry_task_ctx_t *rcvry_ctx, wave_rcvry_type_e type)
{
  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  rcvry_ctx->rcvry_type_forced = type;
  mtlk_osal_lock_release(&rcvry_ctx->lock);
}

static BOOL _rcvry_is_running (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  wave_rcvry_type_e type;

  MTLK_ASSERT(NULL != rcvry_ctx);

  if (RCVRY_TYPE_UNRECOVARABLE_ERROR == __rcvry_type_global_get()) {
    WLOG_S("%s Recovery faild", __rcvry_type_to_string(RCVRY_TYPE_UNRECOVARABLE_ERROR));
    return TRUE;
  }

  type = __rcvry_type_current_get(rcvry_ctx);

  if (RCVRY_TYPE_UNDEF == type)
    return FALSE;

  WLOG_S("%s Recovery is running", __rcvry_type_to_string(type));
  return TRUE;
}

wave_rcvry_type_e wave_rcvry_type_current_get (const mtlk_hw_t *hw_ctx)
{
  wave_rcvry_type_e res = RCVRY_TYPE_UNDEF;
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx))
    res = __rcvry_type_current_get(rcvry_ctx);

  return res;
}

void wave_rcvry_type_forced_set (const mtlk_hw_t *hw_ctx, wave_rcvry_type_e type)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx))
    __rcvry_type_forced_set(rcvry_ctx, type);
}

void wave_rcvry_restart_scan_set (const mtlk_hw_t *hw_ctx,
                                  const mtlk_vap_manager_t *vap_manager,
                                  const BOOL value)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx)) {
    wave_rcvry_radio_ctx_t *radio_ctx = _rcvry_radio_ctx_get(rcvry_ctx, vap_manager);
    if (radio_ctx)
      atomic_set(&radio_ctx->is_restart_scan, (uint32)value);
  }
}

BOOL wave_rcvry_restart_scan_get (const mtlk_hw_t *hw_ctx,
                                  const mtlk_vap_manager_t *vap_manager)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx)) {
    wave_rcvry_radio_ctx_t *radio_ctx = _rcvry_radio_ctx_get(rcvry_ctx, vap_manager);
    if (radio_ctx)
      return (BOOL)atomic_read(&radio_ctx->is_restart_scan);
  }
  return FALSE;
}

wave_rcvry_scan_type_e wave_rcvry_scan_type_current_get (const mtlk_hw_t *hw_ctx,
                                                         const mtlk_vap_manager_t *vap_manager)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx) &&
      (RCVRY_TYPE_UNDEF != __rcvry_type_current_get(rcvry_ctx))) {
    wave_rcvry_radio_ctx_t *radio_ctx = _rcvry_radio_ctx_get(rcvry_ctx, vap_manager);
    if (radio_ctx)
      return radio_ctx->scan_info->scan_type_current;
  }
  return RCVRY_NO_SCAN;
}

void wave_rcvry_chandef_current_get (const mtlk_hw_t *hw_ctx,
                                     const mtlk_vap_manager_t *vap_manager,
                                     mtlk_handle_t *chan_def)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx) &&
      (RCVRY_TYPE_UNDEF != __rcvry_type_current_get(rcvry_ctx))) {
    wave_rcvry_radio_ctx_t *radio_ctx = _rcvry_radio_ctx_get(rcvry_ctx, vap_manager);
    if (radio_ctx) {
      *chan_def = HANDLE_T(&radio_ctx->scan_info->chan_def_current);
      return;
    }
  }
  *chan_def = HANDLE_T(NULL);
}

void wave_rcvry_chanparam_set (const mtlk_hw_t *hw_ctx,
                               const mtlk_vap_manager_t *vap_manager,
                               const mtlk_handle_t *value)
{
  wave_rcvry_task_ctx_t *rcvry_ctx;

  MTLK_ASSERT(NULL != value);
  rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx)) {
    wave_rcvry_radio_ctx_t *radio_ctx = _rcvry_radio_ctx_get(rcvry_ctx, vap_manager);
    if (radio_ctx)
      radio_ctx->cac_info->cpd_current = *((struct set_chan_param_data*)value);
  }
}

mtlk_handle_t * wave_rcvry_chanparam_get (const mtlk_hw_t *hw_ctx,
                                          const mtlk_vap_manager_t *vap_manager)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = _rcvry_task_ctx_get(hw_ctx);

  if (rcvry_ctx && _rcvry_is_initialized(rcvry_ctx)) {
    wave_rcvry_radio_ctx_t *radio_ctx = _rcvry_radio_ctx_get(rcvry_ctx, vap_manager);
    if (radio_ctx)
      return (mtlk_handle_t*)(&radio_ctx->cac_info->cpd_current);
  }
  return NULL;
}

static int _wave_rcvry_nl_process_msg_dump_send (wave_rcvry_card_cfg_t *pcard);

static int _rcvry_step_fw_dump (wave_rcvry_card_cfg_t *pcard, uint16 oid)
{
  int res;

  ILOG1_D("CID-%04x: Master Recovery FW Dump Send Step started", oid);

  res = _wave_rcvry_nl_process_msg_dump_send(pcard);

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%04x: Master Recovery FW Dump Send Step finished successfully", oid);
  else
    ELOG_DD("CID-%04x: Master Recovery FW Dump Send Step finished with error: res=%d", oid, res);

  return res;
}

static inline void __rcvry_step_cancel_commands (wave_rcvry_radio_ctx_t *radio_ctx)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;

  while (vap_info_temp) {
    mtlk_core_t *core = mtlk_vap_get_core(vap_info_temp->vap_handle);
    ILOG1_D("CID-%04x: Cancel all tasks in serializer", mtlk_vap_get_oid(vap_info_temp->vap_handle));
    mtlk_serializer_cancel_commands(&core->slow_ctx->serializer);
    vap_info_temp = *vap_info_ptr_temp++;
  }
}

static void _rcvry_step_cancel_commands (wave_rcvry_card_cfg_t *pcard, uint16 oid)
{
  uint32 i;
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;

  MTLK_ASSERT(RCVRY_TYPE_NONE == __rcvry_type_current_get(rcvry_ctx));

  ILOG1_D("CID-%04x: Master Recovery Cancel Commands Step started", oid);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++)
    __rcvry_step_cancel_commands(&rcvry_ctx->radio_ctx[i]);

  ILOG1_D("CID-%04x: Master Recovery Cancel Commands Step finished successfully", oid);
}

static inline int __rcvry_step_flush_stations (wave_rcvry_radio_ctx_t *radio_ctx)
{
  wave_radio_sync_hostapd_done_set(wave_vap_radio_get(radio_ctx->master_vap_handle), FALSE);
  return wv_cfg80211_handle_flush_stations(radio_ctx->master_vap_handle, NULL, 0);
}

static int _rcvry_step_flush_stations (wave_rcvry_card_cfg_t *pcard, uint16 oid)
{
  uint32 i;
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;
  int res = MTLK_ERR_OK;

  ILOG1_D("CID-%04x: Master Recovery Flush Stations Step started", oid);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    wave_rcvry_radio_ctx_t *radio_ctx = &rcvry_ctx->radio_ctx[i];
    if (mtlk_vap_manager_did_ap_started(radio_ctx->vap_manager)){
      if (wave_radio_is_sync_hostapd_done(wave_vap_radio_get(radio_ctx->master_vap_handle))) {
        int res_temp = __rcvry_step_flush_stations(radio_ctx);
        if (MTLK_ERR_OK != res_temp) {
          ELOG_DDD("CID-%04x: Master Recovery Flush Stations Step for Radio %d "
                  "finished with error: res=%d", oid, i, res_temp);
          /* NOTE: continue for next radio if any */
          res = res_temp;
        }
      }
    }
  }

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%04x: Master Recovery Flush Stations Step finished successfully", oid);
  else
    ILOG1_D("CID-%04x: Master Recovery Flush Stations Step finished", oid);

  return MTLK_ERR_OK; /* NOTE: return success always */
}

static inline int __rcvry_step_vap_deactivation (wave_rcvry_radio_ctx_t *radio_ctx)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;
  mtlk_core_t *mcore = mtlk_vap_get_core(radio_ctx->master_vap_handle);
  int res = MTLK_ERR_OK;

  while (vap_info_temp) {
    res = core_recovery_deactivate(mcore, mtlk_vap_get_core(vap_info_temp->vap_handle));
    vap_info_temp = *vap_info_ptr_temp++;
  }
  return res;
}

static int _rcvry_step_vap_deactivation (wave_rcvry_task_ctx_t *rcvry_ctx, uint16 oid)
{
  uint32 i;
  int res = MTLK_ERR_OK;

  ILOG1_D("CID-%04x: Master Recovery VAP Deactivation Step started", oid);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    int res_temp = __rcvry_step_vap_deactivation(&rcvry_ctx->radio_ctx[i]);
    if (MTLK_ERR_OK != res_temp) {
      ELOG_DDD("CID-%04x: Master Recovery VAP Deactivation Step for Radio %d "
               "finished with error: res=%d", oid, i, res_temp);
      /* NOTE: continue for next radio if any */
      res = res_temp;
    }
  }

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%04x: Master Recovery VAP Deactivation Step finished successfully", oid);
  else
    ILOG1_D("CID-%04x: Master Recovery VAP Deactivation Step finished", oid);

  return MTLK_ERR_OK; /* NOTE: return success always */
}

static inline int __rcvry_step_fw_isolation (wave_rcvry_radio_ctx_t *radio_ctx, wave_rcvry_type_e rcvry_type)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;
  int res = MTLK_ERR_OK;

  if (RCVRY_TYPE_NONE == rcvry_type) {
    mtlk_core_t *mcore = mtlk_vap_get_core(radio_ctx->master_vap_handle);
    abort_scan_sync(mcore);
  }

  while (vap_info_temp) {
    /* FIXME: may be we don't need 2nd argument for this function */
    res = core_on_rcvry_isol(mtlk_vap_get_core(vap_info_temp->vap_handle), rcvry_type);
    if (MTLK_ERR_OK != res)
      return res;
    mtlk_df_on_rcvry_isol(mtlk_vap_get_df(vap_info_temp->vap_handle));
    vap_info_temp = *vap_info_ptr_temp++;
  }

  mtlk_vap_manager_set_active_vaps_number(radio_ctx->vap_manager, 0);

  return res;
}

static int _rcvry_step_fw_isolation (wave_rcvry_card_cfg_t *pcard, uint16 oid)
{
  uint32 i;
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;
  wave_rcvry_type_e rcvry_type = __rcvry_type_current_get(rcvry_ctx);
  int res = MTLK_ERR_OK;

  ILOG1_D("CID-%04x: Master Recovery FW Isolation Step started", oid);

  mtlk_hw_mmb_isolate(pcard->hw_ctx);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    res = __rcvry_step_fw_isolation(&rcvry_ctx->radio_ctx[i], rcvry_type);
    if (MTLK_ERR_OK != res)
      break;
  }

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%04x: Master Recovery FW Isolation Step finished successfully", oid);
  else
    ELOG_DD("CID-%04x: Master Recovery FW Isolation Step finished with error: res=%d", oid, res);

  return res;
}

static inline int __rcvry_step_fw_restoration (wave_rcvry_radio_ctx_t *radio_ctx, wave_rcvry_type_e rcvry_type)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;
  int res = MTLK_ERR_OK;

  while (vap_info_temp) {
    mtlk_df_on_rcvry_restore(mtlk_vap_get_df(vap_info_temp->vap_handle));
    res = core_on_rcvry_restore(mtlk_vap_get_core(vap_info_temp->vap_handle), rcvry_type);
    if (MTLK_ERR_OK != res)
      return res;
    vap_info_temp = *vap_info_ptr_temp++;
  }
  return res;
}

static int _rcvry_step_fw_restoration (wave_rcvry_card_cfg_t *pcard, uint16 oid)
{
  int res;
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;
  wave_rcvry_type_e rcvry_type = __rcvry_type_current_get(rcvry_ctx);

  MTLK_ASSERT((RCVRY_TYPE_FAST == rcvry_type) ||
              (RCVRY_TYPE_FULL == rcvry_type));

  ILOG1_D("CID-%04x: Master Recovery FW Restoration Step started", oid);

  res = mtlk_hw_mmb_restore(pcard->hw_ctx);

  if (MTLK_ERR_OK == res) {
    uint32 i;

    for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
      res = __rcvry_step_fw_restoration(&rcvry_ctx->radio_ctx[i], rcvry_type);
      if (MTLK_ERR_OK != res)
        break;
    }
  }

  if (MTLK_ERR_OK == res) {
    res = wave_radio_calibrate((wave_radio_descr_t *)wave_card_radio_descr_get(pcard->hw_ctx),
                               (RCVRY_TYPE_FULL == rcvry_type));
  }

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%04x: Master Recovery FW Restoration Step finished successfully", oid);
  else
    ELOG_DD("CID-%04x: Master Recovery FW Restoration Step finished with error: res=%d", oid, res);

  return res;
}

static inline int __rcvry_step_fw_reconfiguration (wave_rcvry_radio_ctx_t *radio_ctx, wave_rcvry_type_e rcvry_type)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;
  int res = MTLK_ERR_OK;

  /* Configure VAPs */
  while (vap_info_temp) {
    res = core_on_rcvry_configure(mtlk_vap_get_core(vap_info_temp->vap_handle),
                                  (uint32)vap_info_temp->was_connected);
    if (MTLK_ERR_OK != res)
      return res;
    vap_info_temp = *vap_info_ptr_temp++;
  }

  /* Continue Background Scan */
  if (RCVRY_BG_SCAN == radio_ctx->scan_info->scan_type_current) {
    res = mtlk_scan_recovery_continue_scan(mtlk_vap_get_core(radio_ctx->master_vap_handle),
                                           MTLK_SCAN_SUPPORT_BG_SCAN);
    if (MTLK_ERR_OK != res)
      return res;
  }

  mtlk_ta_on_rcvry_restore(mtlk_vap_get_ta(radio_ctx->master_vap_handle));

  return res;
}

static int _rcvry_step_fw_reconfiguration (wave_rcvry_card_cfg_t *pcard, uint16 oid)
{
  uint32 i;
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;
  wave_rcvry_type_e rcvry_type = __rcvry_type_current_get(rcvry_ctx);
  int res = MTLK_ERR_OK;

  MTLK_ASSERT((RCVRY_TYPE_FAST == rcvry_type) ||
              (RCVRY_TYPE_FULL == rcvry_type));

  ILOG1_D("CID-%04x: Master Recovery FW Reconfiguration Step started", oid);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    res = __rcvry_step_fw_reconfiguration(&rcvry_ctx->radio_ctx[i], rcvry_type);
    if (MTLK_ERR_OK != res)
      break;
  }

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%04x: Master Recovery FW Reconfiguration Step finished successfully", oid);
  else
    ELOG_DD("CID-%04x: Master Recovery FW Reconfiguration Step finished with error: res=%d", oid, res);

  return res;
}

static inline void __rcvry_step_finalization (wave_rcvry_radio_ctx_t *radio_ctx)
{
  wave_radio_t *radio = wave_vap_radio_get(radio_ctx->master_vap_handle);

  if (wave_radio_get_sta_vifs_exist(radio))
    wave_radio_recover_sta_vifs(radio);
}

static void _rcvry_step_finalization (wave_rcvry_task_ctx_t *rcvry_ctx, uint16 oid)
{
  uint32 i;
  wave_rcvry_type_e rcvry_type = __rcvry_type_current_get(rcvry_ctx);

  MTLK_ASSERT((RCVRY_TYPE_FAST == rcvry_type) ||
              (RCVRY_TYPE_FULL == rcvry_type));

  ILOG1_D("CID-%04x: Master Recovery Finalization Step started", oid);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++)
    __rcvry_step_finalization(&rcvry_ctx->radio_ctx[i]);

  /* Update Recovery WSS statistics */
  switch (rcvry_type)
  {
    case RCVRY_TYPE_FAST:
      mtlk_wss_cntr_inc(rcvry_ctx->wss_cnt[RCVRY_NOF_FAST_RCVRY_PROCESSED]);
      break;
    case RCVRY_TYPE_FULL:
      mtlk_wss_cntr_inc(rcvry_ctx->wss_cnt[RCVRY_NOF_FULL_RCVRY_PROCESSED]);
      break;
    default: break;
  }

  ILOG1_D("CID-%04x: Master Recovery Finalization Step finished successfully", oid);
}

static inline void __rcvry_step_on_error (wave_rcvry_radio_ctx_t *radio_ctx)
{
  core_on_rcvry_error(mtlk_vap_get_core(radio_ctx->master_vap_handle));
}

static void _rcvry_step_on_error (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  uint32 i;
  wave_rcvry_type_e rcvry_type = __rcvry_type_current_get(rcvry_ctx);

  MTLK_ASSERT((RCVRY_TYPE_FAST == rcvry_type) ||
              (RCVRY_TYPE_FULL == rcvry_type));

  for (i = 0; i < rcvry_ctx->max_radios_number; i++)
    __rcvry_step_on_error(&rcvry_ctx->radio_ctx[i]);

  /* Update Recovery WSS statistics */
  switch (rcvry_type)
  {
    case RCVRY_TYPE_FAST:
      mtlk_wss_cntr_inc(rcvry_ctx->wss_cnt[RCVRY_NOF_FAST_RCVRY_FAILED]);
      break;
    case RCVRY_TYPE_FULL:
      mtlk_wss_cntr_inc(rcvry_ctx->wss_cnt[RCVRY_NOF_FULL_RCVRY_FAILED]);
      break;
    default: break;
  }
}

static int _rcvry_task_slave (mtlk_handle_t pcard_handle, const void *payload, uint32 size)
{
  uint16 oid;
  wave_rcvry_vap_info_t *vap_info;
  mtlk_osal_mutex_t *slave_sync_mutex;
  mtlk_osal_mutex_t *slave_release_mutex;

  MTLK_ASSERT(sizeof(vap_info) == size);
  vap_info = *(wave_rcvry_vap_info_t **)payload;
  MTLK_ASSERT(NULL != vap_info);

  oid = mtlk_vap_get_oid(vap_info->vap_handle);
  ILOG1_D("CID-%04x: Slave Recovery task started", oid);

  slave_sync_mutex = vap_info->slave_sync_mutex;
  slave_release_mutex = vap_info->slave_release_mutex;

  /* Some specific actions of Slave Recovery task might be here */
  /* Att: Don't use vap_info below this point due to racing with Master Recovery task */

  ILOG1_D("CID-%04x: Unblock Master Recovery task", oid);
  mtlk_osal_mutex_release(slave_sync_mutex);
  ILOG1_D("CID-%04x: Wait until Slave Recovery task is unblocked by Master Recovery task", oid);
  mtlk_osal_mutex_acquire(slave_release_mutex);
  ILOG1_D("CID-%04x: Slave Recovery task finished successfully", oid);
  return MTLK_ERR_OK;
}

static inline void __rcvry_step_slave_vaps_blocked_wait (wave_rcvry_radio_ctx_t *radio_ctx,
                                                         mtlk_osal_mutex_t *slave_sync_mutex)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;

  while (vap_info_temp) {
    mtlk_osal_mutex_acquire(slave_sync_mutex);
    vap_info_temp = *vap_info_ptr_temp++;
  }
  mtlk_osal_mutex_release(slave_sync_mutex);
}

static void _rcvry_step_slave_vaps_blocked_wait (wave_rcvry_task_ctx_t *rcvry_ctx, uint16 oid)
{
  uint32 i;

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    ILOG1_DD("CID-%04x: Wait until all Slave Recovery tasks of Radio %d are blocked", oid, i);
    __rcvry_step_slave_vaps_blocked_wait(&rcvry_ctx->radio_ctx[i], &rcvry_ctx->slave_sync_mutex);
    ILOG1_DD("CID-%04x: All Slave Recovery tasks of Radio %d are blocked", oid, i);
  }
}

static inline void __rcvry_step_slave_vaps_unblock (wave_rcvry_radio_ctx_t *radio_ctx,
                                                    mtlk_osal_mutex_t *slave_release_mutex)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;

  while (vap_info_temp) {
    mtlk_osal_mutex_release(slave_release_mutex);
    vap_info_temp = *vap_info_ptr_temp++;
  }
}

static void _rcvry_step_slave_vaps_unblock (wave_rcvry_task_ctx_t *rcvry_ctx, uint16 oid)
{
  uint32 i;

  for (i = 0; i < rcvry_ctx->max_radios_number; i++) {
    ILOG1_DD("CID-%04x: Unblock all Slave Recovery tasks of Radio %d", oid, i);
    __rcvry_step_slave_vaps_unblock(&rcvry_ctx->radio_ctx[i],
      &rcvry_ctx->slave_release_mutex[rcvry_ctx->slave_release_mutex_idx]);
    ILOG1_DD("CID-%04x: All Slave Recovery tasks of Radio %d are unblocked", oid, i);
  }
}

static inline int __rcvry_step_common_flow (wave_rcvry_card_cfg_t *pcard, uint16 oid)
{
  int res;

  /* Execute VAP Deactivation step */
  res = _rcvry_step_vap_deactivation(&pcard->rcvry_ctx, oid);
  if (MTLK_ERR_OK != res)
    return res;
  /* Execute Isolation step */
  res = _rcvry_step_fw_isolation(pcard, oid);
  return res;
}

static int _rcvry_step_no_dump (wave_rcvry_card_cfg_t *pcard)
{
  return _rcvry_nl_send_msg(LTQ_NL80211_VENDOR_EVENT_NO_DUMP, pcard,
                            NULL, 0, "NO dump event");
}

static int _rcvry_task_initiate(wave_rcvry_card_cfg_t *pcard);
static BOOL _wave_rcvry_krnl_panic_enabled(wave_rcvry_card_cfg_t *pcard);
static int _wave_rcvry_fwdump_handle (mtlk_handle_t pcard_handle, const void *payload, uint32 size);
static int wave_rcvry_nl_send_msg_mac_error_evt (wave_rcvry_card_cfg_t *pcard, const uint8 error_type);
static int wave_rcvry_nl_send_msg_unrecovarable_error (wave_rcvry_card_cfg_t *pcard);

static int _rcvry_task_master (mtlk_handle_t pcard_handle, const void *payload, uint32 size)
{
  wave_rcvry_card_cfg_t *pcard = (wave_rcvry_card_cfg_t *)pcard_handle;
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;
  wave_rcvry_type_e rcvry_type = __rcvry_type_current_get(rcvry_ctx);
  uint16 oid = pcard->card_idx;
  int res = MTLK_ERR_UNKNOWN;

  ILOG1_D("CID-%04x: Master Recovery task started", oid);

  rcvry_ctx->is_rcvry_failed = FALSE;

  /* Wait until Slave VAPs are blocked */
  _rcvry_step_slave_vaps_blocked_wait(rcvry_ctx, oid);

  /* Start rcvry timer if not running */
  if (mtlk_osal_timer_is_stopped(&rcvry_ctx->rcvry_timer)){
    int rcvry_timer = rcvry_ctx->cfg.rcvry_timer_value;
    if (rcvry_timer == 0){
      ELOG_D("CID-%04x: fail timer value not set, setting in to default", oid);
      rcvry_timer = 30 * 60 * MTLK_OSAL_MSEC_IN_SEC;
    }
    ILOG1_DD("CID-%04x: Recovery timer started (%d ms)", oid, rcvry_timer);
    mtlk_osal_timer_set(&rcvry_ctx->rcvry_timer, rcvry_timer);
  }

  /* Wait for FW Assert Indication from UMAC */
  wave_hw_mmb_wait_umac_assert_evt(pcard->hw_ctx);

  /* Notify about MAC Error type if Recovery is Off */
  if (RCVRY_TYPE_NONE == rcvry_type) {
    wave_rcvry_nl_send_msg_mac_error_evt(pcard, __rcvry_mac_error_type_get(rcvry_ctx));
  }

  /* Execute FW Dump step */
  _wave_rcvry_fwdump_handle((mtlk_handle_t)pcard, NULL, 0);

  switch (rcvry_type)
  {
    case RCVRY_TYPE_NONE:
    {
      int res_temp;
      /* Execute all steps (don't break if some step is failed) and save 1st failure result only */
      /* Execute Flush Stations step */
      res = _rcvry_step_flush_stations(pcard, oid);
      /* Execute common steps (regardless of previous result) */
      res_temp = __rcvry_step_common_flow(pcard, oid);
      if (MTLK_ERR_OK == res)
        res = res_temp;
      /* Execute Cancel Commands step (regardless of previous result) */
      _rcvry_step_cancel_commands(pcard, oid);
      /* Set Recovery type */
      __rcvry_type_current_set(rcvry_ctx, RCVRY_TYPE_DUT);

      break;
    }
    case RCVRY_TYPE_FAST:
    case RCVRY_TYPE_FULL:
    {
      /* Execute common steps */
      res = __rcvry_step_common_flow(pcard, oid);
      if (MTLK_ERR_OK != res)
        break;
      /* Execute FW Restoration step */
      res = _rcvry_step_fw_restoration(pcard, oid);
      if (MTLK_ERR_OK != res)
        break;
      /* Execute FW Reconfiguration step */
      res = _rcvry_step_fw_reconfiguration(pcard, oid);
      if (MTLK_ERR_OK != res)
        break;
      /* Execute Finalization step */
      _rcvry_step_finalization(rcvry_ctx, oid);

      if (MTLK_ERR_OK == res) {
        /* Reset Recovery type */
        __rcvry_type_current_set(rcvry_ctx, RCVRY_TYPE_UNDEF);
        /* Reset MAC Error type */
        __rcvry_mac_error_type_set(rcvry_ctx, WAVE_RCVRY_ERRORTYPE_MAC_EXTERNAL_ASSERT);
      }
      else {
        /* Execute On Error step */
        _rcvry_step_on_error(rcvry_ctx);
      }
      break;
    }
    case RCVRY_TYPE_UNRECOVARABLE_ERROR:
    {
      /* Notify about Unrecovarable Error */
      res = wave_rcvry_nl_send_msg_unrecovarable_error(pcard);
      /* If panic is enabled, panic */
      if (_wave_rcvry_krnl_panic_enabled(pcard)){
        char msg[128];
        mtlk_snprintf(msg, sizeof(msg), "Unrecovarable Error on card '%u'",pcard->card_idx);
        mtlk_osal_kernel_panic(msg);
      }
      else {
        int res_temp;
        /* Execute all steps (don't break if some step is failed) and save 1st failure result only */
        /* Execute Flush Stations step */
        res = _rcvry_step_flush_stations(pcard, oid);
        /* Execute common steps (regardless of previous result) */
        res_temp = __rcvry_step_common_flow(pcard, oid);
        if (MTLK_ERR_OK == res)
          res = res_temp;
        /* Execute Cancel Commands step (regardless of previous result) */
        _rcvry_step_cancel_commands(pcard, oid);
      }
      break;
    }
    default: break;
  }

  /* Release Slave VAPs */
  _rcvry_step_slave_vaps_unblock(rcvry_ctx, oid);

  if (MTLK_ERR_OK == res)
    ILOG1_D("CID-%04x: Master Recovery task finished successfully", oid);
  else {
    rcvry_ctx->is_rcvry_failed = TRUE;
    ELOG_DD("CID-%04x: Master Recovery task finished with error %d", oid, res);

    /* Try to initiate Recovery task */
    if ((RCVRY_TYPE_UNRECOVARABLE_ERROR != rcvry_type) &&
        (RCVRY_TYPE_DUT != rcvry_type))
      _rcvry_task_initiate(pcard);
  }
  return res;
}

static inline void __rcvry_task_schedule (wave_rcvry_card_cfg_t *pcard,
                                          wave_rcvry_radio_ctx_t *radio_ctx,
                                          mtlk_osal_mutex_t *slave_sync_mutex,
                                          mtlk_osal_mutex_t *slave_release_mutex,
                                          uint32 radio_idx)
{
  wave_rcvry_vap_info_t **vap_info_ptr_temp = radio_ctx->vap_info;
  wave_rcvry_vap_info_t *vap_info_temp = *vap_info_ptr_temp++;

  /* Schedule Master and Slave Recovery tasks */
  while (vap_info_temp) {
    vap_info_temp->slave_sync_mutex = slave_sync_mutex;
    vap_info_temp->slave_release_mutex = slave_release_mutex;

    if (mtlk_vap_is_master(vap_info_temp->vap_handle) && (WAVE_RCVRY_PRIMARY_VAP_RADIO_IDX == radio_idx)) {
      core_schedule_recovery_task(vap_info_temp->vap_handle, _rcvry_task_master, (mtlk_handle_t)pcard,
                                  NULL, 0);
      ILOG1_D("CID-%04x: Master Recovery task scheduled", mtlk_vap_get_oid(vap_info_temp->vap_handle));
    }
    else {
      core_schedule_recovery_task(vap_info_temp->vap_handle, _rcvry_task_slave, (mtlk_handle_t)pcard,
                                  &vap_info_temp, sizeof(vap_info_temp));
      ILOG1_DD("CID-%04x: Slave Recovery task scheduled for Radio %d",
               mtlk_vap_get_oid(vap_info_temp->vap_handle), radio_idx);
    }
    vap_info_temp = *vap_info_ptr_temp++;
  }
}

static void _rcvry_task_schedule (wave_rcvry_card_cfg_t *pcard, wave_rcvry_task_ctx_t *rcvry_ctx)
{
  uint32 i;
  mtlk_osal_mutex_t *mutex;

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  rcvry_ctx->slave_release_mutex_idx ^= 1;
  mtlk_osal_lock_release(&rcvry_ctx->lock);

  mutex = &rcvry_ctx->slave_release_mutex[rcvry_ctx->slave_release_mutex_idx];
  mtlk_osal_mutex_acquire(mutex);

  for (i = 0; i < rcvry_ctx->max_radios_number; i++)
    __rcvry_task_schedule(pcard, &rcvry_ctx->radio_ctx[i], &rcvry_ctx->slave_sync_mutex, mutex, i);
}

static inline void __rcvry_task_initiate_print (wave_rcvry_task_ctx_t *rcvry_ctx, uint16 oid)
{
  wave_rcvry_type_e rcvry_type_current;
  uint8 rcvry_fast_cnt, rcvry_fast_cnt_cfg, rcvry_full_cnt, rcvry_full_cnt_cfg;

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  rcvry_type_current = rcvry_ctx->rcvry_type_current;
  rcvry_fast_cnt = rcvry_ctx->rcvry_fast_cnt;
  rcvry_full_cnt = rcvry_ctx->rcvry_full_cnt;
  rcvry_fast_cnt_cfg = rcvry_ctx->cfg.rcvry_fast_cnt;
  rcvry_full_cnt_cfg = rcvry_ctx->cfg.rcvry_full_cnt;
  mtlk_osal_lock_release(&rcvry_ctx->lock);

  RLOG_DS("CID-%04x: %s Recovery initiated", oid,
          __rcvry_type_to_string(rcvry_type_current));
  RLOG_DDDDD("CID-%04x: Fast: %d (%d), Full: %d (%d)", oid,
             rcvry_fast_cnt, rcvry_fast_cnt_cfg, rcvry_full_cnt, rcvry_full_cnt_cfg);
}

static inline BOOL __rcvry_cnt_current_cmp (wave_rcvry_task_ctx_t *rcvry_ctx, wave_rcvry_type_e type)
{
  BOOL res = FALSE;

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  switch (type)
  {
    case RCVRY_TYPE_FAST:
    {
      res = (rcvry_ctx->rcvry_fast_cnt < rcvry_ctx->cfg.rcvry_fast_cnt);
      break;
    }
    case RCVRY_TYPE_FULL:
    {
      res = (rcvry_ctx->rcvry_full_cnt < rcvry_ctx->cfg.rcvry_full_cnt);
      break;
    }
    default: break;
  }
  mtlk_osal_lock_release(&rcvry_ctx->lock);
  return res;
}

static inline void __rcvry_cnt_current_inc (wave_rcvry_task_ctx_t *rcvry_ctx, wave_rcvry_type_e type)
{
  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  switch (type)
  {
    case RCVRY_TYPE_NONE:
    {
      rcvry_ctx->rcvry_none_cnt++;
      break;
    }
    case RCVRY_TYPE_FAST:
    {
      rcvry_ctx->rcvry_fast_cnt++;
      break;
    }
    case RCVRY_TYPE_FULL:
    {
      rcvry_ctx->rcvry_full_cnt++;
      break;
    }
    default: break;
  }
  mtlk_osal_lock_release(&rcvry_ctx->lock);
}

static inline uint8 __rcvry_cnt_current_get (wave_rcvry_task_ctx_t *rcvry_ctx, wave_rcvry_type_e type)
{
  uint8 res = 0;

  mtlk_osal_lock_acquire(&rcvry_ctx->lock);
  switch (type)
  {
    case RCVRY_TYPE_NONE:
    {
      res = rcvry_ctx->rcvry_none_cnt;
      break;
    }
    case RCVRY_TYPE_FAST:
    {
      res = rcvry_ctx->rcvry_fast_cnt;
      break;
    }
    case RCVRY_TYPE_FULL:
    {
      res = rcvry_ctx->rcvry_full_cnt;
      break;
    }
    default: break;
  }
  mtlk_osal_lock_release(&rcvry_ctx->lock);
  return res;
}

static wave_rcvry_type_e _rcvry_type_current_determine (wave_rcvry_task_ctx_t *rcvry_ctx)
{
  wave_rcvry_type_e type;

  /* Determine Recovery type */

  /* Note: None Recovery can be executed only once */
  if (__rcvry_cnt_current_get(rcvry_ctx, RCVRY_TYPE_NONE) > 0)
    type = RCVRY_TYPE_IGNORE;
  else {
    if (_wv_rcvry_is_enabled(rcvry_ctx)) {
      if (__rcvry_cnt_current_cmp(rcvry_ctx, RCVRY_TYPE_FAST))
        type = RCVRY_TYPE_FAST;
      else if (__rcvry_cnt_current_cmp(rcvry_ctx, RCVRY_TYPE_FULL))
        type = RCVRY_TYPE_FULL;
      else {
        type = RCVRY_TYPE_UNRECOVARABLE_ERROR;
        __rcvry_type_global_set(type);
      }
    }
    else
      type = RCVRY_TYPE_NONE;
  }

  /* Increment counter of current Recovery type */
  __rcvry_cnt_current_inc(rcvry_ctx, type);
  /* Set current Recovery type */
  __rcvry_type_current_set(rcvry_ctx, type);
  return type;
}

static int _wave_rcvry_fwdump_handle (mtlk_handle_t pcard_handle, const void *payload, uint32 size)
{
  wave_rcvry_card_cfg_t *pcard = (wave_rcvry_card_cfg_t *)pcard_handle;
  if (!(pcard->rcvry_ctx.cfg.fw_dump) || pcard->rcvry_ctx.fw_dump_data != NULL){
    _rcvry_step_no_dump(pcard);
  }
  else {
    _rcvry_step_fw_dump(pcard, pcard->card_idx);
  }

  return MTLK_ERR_OK;
}

static void _rcvry_dump_schedule(wave_rcvry_card_cfg_t *pcard){
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;
  mtlk_vap_handle_t master_vap_handle = rcvry_ctx->radio_ctx[WAVE_RCVRY_PRIMARY_VAP_RADIO_IDX].master_vap_handle;

  core_schedule_recovery_task(master_vap_handle, _wave_rcvry_fwdump_handle, (mtlk_handle_t)pcard,
                                NULL, 0);
  ILOG1_D("CID-%04x: FW handle scheduled", mtlk_vap_get_oid(master_vap_handle));
}

static void _rcvry_umac_assert_evt_simulate(wave_rcvry_card_cfg_t *pcard);

static int _rcvry_task_initiate (wave_rcvry_card_cfg_t *pcard)
{
  int res;
  uint8 card_idx;
  wave_rcvry_type_e rcvry_type;
  mtlk_vap_handle_t vap_handle;
  wave_rcvry_task_ctx_t *rcvry_ctx;

  MTLK_ASSERT(NULL != pcard);
  rcvry_ctx = &pcard->rcvry_ctx;
  MTLK_ASSERT(RCVRY_SIGN == rcvry_ctx->signature);
  card_idx = mtlk_hw_mmb_get_card_idx(pcard->hw_ctx);
  vap_handle = rcvry_ctx->radio_ctx[WAVE_RCVRY_PRIMARY_VAP_RADIO_IDX].master_vap_handle;

  /* Check Recovery initialization */
  if (!vap_handle) {
    ELOG_D("CID-%02x: Cancel Initiating Recovery: Master VAP not created for Radio 0", card_idx);
    return MTLK_ERR_UNKNOWN;
  }

  /* Determine Recovery type */
  rcvry_type = _rcvry_type_current_determine(rcvry_ctx);
  if (RCVRY_TYPE_IGNORE == rcvry_type) {
    ELOG_D("CID-%02x: Cancel Initiating Recovery: ignore consecutive None Recovery", card_idx);
    return MTLK_ERR_OK;
  }

  /* Disable WLAN interrupts */
  mtlk_hw_mmb_dis_interrupts(pcard->hw_ctx);

  /* Print information about Recovery to be executed */
  __rcvry_task_initiate_print(rcvry_ctx, mtlk_vap_get_oid(vap_handle));

  if (!rcvry_ctx->is_rcvry_failed) {
    /* Prepare VAP-info */
    _rcvry_task_vap_info_prepare(rcvry_ctx);
    /* Prepare Scan info */
    res = _rcvry_task_scan_info_prepare(rcvry_ctx);
    if (MTLK_ERR_OK != res) {
      ELOG_D("CID-%02x: Cancel Initiating Recovery: Can't get current channel configuration", card_idx);
      return res;
    }
  }
  else
    _rcvry_umac_assert_evt_simulate(pcard);

  /* Schedule Master and Slave Recovery tasks */
  _rcvry_task_schedule(pcard, rcvry_ctx);

  return MTLK_ERR_OK;
}

int wave_rcvry_task_initiate (mtlk_hw_t *hw_ctx)
{
  int res = MTLK_ERR_NOT_READY;
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);
  uint8 card_idx = mtlk_hw_mmb_get_card_idx(hw_ctx);

  MTLK_UNREFERENCED_PARAM(card_idx);

  ILOG1_D("CID-%02x: Initiating Recovery", card_idx);

  if (pcard && _rcvry_is_initialized(&pcard->rcvry_ctx)) {
    if (!_rcvry_is_running(&pcard->rcvry_ctx))
      res = _rcvry_task_initiate(pcard);
    else
      ELOG_D("CID-%02x: Cancel Initiating Recovery: running already", card_idx);
  }
  else
    ELOG_D("CID-%02x: Cancel Initiating Recovery: not initialized yet", card_idx);

  /* If Recovery not initialized yet, then error is occurred in _pci_probe,
     thus Recovery is handled by implementation in _pci_probe / _pci_remove */
  return res;
}

BOOL wave_rcvry_is_configured (const mtlk_hw_t *hw_ctx)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard) {
    wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;

    return (__rcvry_cnt_current_cmp(rcvry_ctx, RCVRY_TYPE_FAST) ||
            __rcvry_cnt_current_cmp(rcvry_ctx, RCVRY_TYPE_FULL));
  }
  return FALSE;
}

static void _rcvry_fw_dump_in_progress_set (const uint8 card_idx, const BOOL value)
{
  wave_rcvry_card_cfg_t *pcard = wave_rcvry.proc_descr.p_card[card_idx];

  if (pcard)
    atomic_set(&pcard->rcvry_ctx.is_fw_dump_in_progress, (uint32)value);
}

BOOL wave_rcvry_fw_dump_in_progress_get (const mtlk_hw_t *hw_ctx)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard)
    return (BOOL)atomic_read(&pcard->rcvry_ctx.is_fw_dump_in_progress);

  return FALSE;
}

static int _rcvry_nl_send_msg_dump_ready (wave_rcvry_card_cfg_t *pcard)
{
  int res = MTLK_ERR_OK;
  struct intel_vendor_whm_event_cfg whm_nl_event_data;
  struct intel_vendor_fw_dump_ready_info dump_ready_nl_event_data = { 0 };

  memset(&whm_nl_event_data, 0, sizeof(whm_nl_event_data));
  whm_nl_event_data.warning_id    = WHM_DRV_MAC_FATAL;
  whm_nl_event_data.warning_layer = WHM_DRIVER_TRIGGER;

  dump_ready_nl_event_data.card_idx = pcard->card_idx;

  res = _rcvry_nl_send_msg(LTQ_NL80211_VENDOR_EVENT_ASSERT_DUMP_READY, pcard,
                           &dump_ready_nl_event_data, sizeof(dump_ready_nl_event_data), "Dump Ready event");
  if (res != MTLK_ERR_OK) {
    ELOG_V("Error in sending FW-Dump-Ready event to Dump handler");
    return res;
  }

  res = _rcvry_nl_send_msg(LTQ_NL80211_VENDOR_EVENT_WHM, pcard, &whm_nl_event_data, sizeof(whm_nl_event_data), "WHM Dump Ready event");;
  if (res != MTLK_ERR_OK) {
    ELOG_V("Error in sending WHM-Dump-Ready event to WHM Daemon");
    return res;
  }

  return res;
}

static int _wave_rcvry_nl_process_msg_dump_send (wave_rcvry_card_cfg_t *pcard)
{
  wave_rcvry_task_ctx_t *rcvry_ctx = &pcard->rcvry_ctx;
  int dump_total_size = 0;
  int i;
  uint8 card_idx = pcard->card_idx;
  int current_size = 0;
  wave_fw_dump_info_t *info = NULL;

  info = pcard->info;

  MTLK_ASSERT(info);

  for (i = 0; i < pcard->nof_files; i++, info++) {
    if (info == NULL) return MTLK_ERR_NO_ENTRY;
    if (0 == info->io_size) {
      continue; /* skip */
    }
    dump_total_size += info->io_size;
  }

  _rcvry_fw_dump_in_progress_set(card_idx, TRUE);

  mtlk_osal_mutex_acquire(&rcvry_ctx->fw_dump_mutex);
  rcvry_ctx->fw_dump_data = mtlk_osal_vmem_alloc(dump_total_size, LQLA_MEM_TAG_FW_RECOVERY);
  if(NULL == rcvry_ctx->fw_dump_data)
  {
    _rcvry_fw_dump_in_progress_set(card_idx, FALSE);
    ELOG_D("CID-%02x: alloc failure for fw-dump", card_idx);
    /*no memory - no dump*/
    _rcvry_step_no_dump(pcard);
    mtlk_osal_mutex_release(&rcvry_ctx->fw_dump_mutex);
    return MTLK_ERR_NO_MEM;
  }

  info = pcard->info;
  for (i = 0; i < pcard->nof_files; i++, info++) {
    if (info == NULL){
      _rcvry_fw_dump_in_progress_set(card_idx, FALSE);
      mtlk_osal_mutex_release(&rcvry_ctx->fw_dump_mutex);
      return MTLK_ERR_NO_ENTRY;
    }
    if (0 == info->io_size) {
      continue; /* skip */
    }
    wave_hw_mmb_memcpy_fromio(rcvry_ctx->fw_dump_data + current_size, pcard->mmb_base, info->io_base, info->io_size);
    current_size+=info->io_size;
  }
  mtlk_osal_mutex_release(&rcvry_ctx->fw_dump_mutex);

  _rcvry_fw_dump_in_progress_set(card_idx, FALSE);

  /* Send message and set FW dump in progress flag */
  return _rcvry_nl_send_msg_dump_ready(pcard);
}

static int wave_rcvry_nl_send_msg_unrecovarable_error (wave_rcvry_card_cfg_t *pcard)
{
  /* Send message */
  return _rcvry_nl_send_msg(LTQ_NL80211_VENDOR_EVENT_UNRECOVARABLE_ERROR, pcard, NULL, 0, "UNRECOVARABLE ERROR event");
}

int wave_rcvry_process_msg_unrecovarable_error (const void *mmb_base)
{
  int res = MTLK_ERR_NOT_READY;
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard)
    res = wave_rcvry_nl_send_msg_unrecovarable_error(pcard);

  return res;
}

static int wave_rcvry_nl_send_msg_mac_error_evt (wave_rcvry_card_cfg_t *pcard, const uint8 error_type)
{
  MTLK_ASSERT((WAVE_RCVRY_ERRORTYPE_RESERVED != error_type) &&
              (WAVE_RCVRY_ERRORTYPE_LAST != error_type));

  ILOG1_DD("CID-%02x: MAC Error type to send (error_type=%d) ", pcard->card_idx, error_type);

  /* Send message */
  return _rcvry_nl_send_msg(LTQ_NL80211_VENDOR_EVENT_MAC_ERROR, pcard,
                            &error_type, sizeof(error_type), "MAC Error event");
}

static void _rcvry_mac_fatal_pending_set (wave_rcvry_card_cfg_t *pcard, const BOOL value)
{
  atomic_set(&pcard->rcvry_ctx.is_mac_fatal_pending, (uint32)value);
}
#ifdef UNUSED_CODE
void wave_rcvry_mac_fatal_pending_set (const mtlk_hw_t *hw_ctx, const BOOL value)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard)
    _rcvry_mac_fatal_pending_set(pcard, value);
}
#endif /* UNUSED_CODE */
BOOL wave_rcvry_mac_fatal_pending_get (const mtlk_hw_t *hw_ctx)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard)
    return (BOOL)atomic_read(&pcard->rcvry_ctx.is_mac_fatal_pending);

  return 0;
}

void wave_rcvry_fw_steady_set (const void *mmb_base, const BOOL value)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard)
    pcard->rcvry_ctx.is_fw_steady_state = value;
}

BOOL wave_rcvry_fw_steady_get (const void *mmb_base)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard)
    return pcard->rcvry_ctx.is_fw_steady_state;

  return FALSE;
}

void wave_rcvry_pci_probe_error_set (const void *mmb_base, const BOOL value)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard)
    pcard->rcvry_ctx.is_pci_probe_error = value;
}

BOOL wave_rcvry_pci_probe_error_get (const void *mmb_base)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard)
    return pcard->rcvry_ctx.is_pci_probe_error;

  return FALSE;
}

BOOL wave_rcvry_enabled_get (const void *mmb_base)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard)
    return _wv_rcvry_is_enabled(&pcard->rcvry_ctx);

  return (BOOL)WAVE_RCVRY_MODE_VAL_DEFAULT;
}

static BOOL _wave_rcvry_krnl_panic_enabled(wave_rcvry_card_cfg_t *pcard){
  return (pcard->rcvry_ctx.cfg.rcvry_mode == RCVRY_MODE_ENABLED_WITH_PANIC);
}

BOOL wave_rcvry_krnl_panic_enabled (const void *mmb_base)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);

  if (pcard)
    return _wave_rcvry_krnl_panic_enabled(pcard);

  return FALSE;
}

void wave_rcvry_fwdump_handle (const void *mmb_base)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);
  if (pcard) _wave_rcvry_fwdump_handle((mtlk_handle_t)pcard, NULL, 0);
}

void wave_rcvry_no_dump_event (const void *mmb_base)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_ctx_get(mmb_base);
  if (pcard) {
    _rcvry_step_no_dump(pcard);
  }
}

void wave_rcvry_mac_hang_evt_reset (mtlk_hw_t *hw_ctx, uint32 cpu_no)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard) {
    MTLK_ASSERT(cpu_no < MAX_NUM_OF_FW_CORES);
    mtlk_osal_event_reset(&pcard->rcvry_ctx.mac_hang_evt[cpu_no]);
  }
}

void wave_rcvry_mac_hang_evt_reset_all (mtlk_hw_t *hw_ctx)
{
  uint32 core_no;
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard) {
    for (core_no = 0; core_no < MAX_NUM_OF_FW_CORES; core_no++)
      mtlk_osal_event_reset(&pcard->rcvry_ctx.mac_hang_evt[core_no]);
  }
}

static void _rcvry_mac_hang_evt_set (wave_rcvry_card_cfg_t *pcard, uint32 cpu_no)
{
  MTLK_ASSERT(cpu_no < MAX_NUM_OF_FW_CORES);
  mtlk_osal_event_set(&pcard->rcvry_ctx.mac_hang_evt[cpu_no]);
}

void wave_rcvry_mac_hang_evt_set (mtlk_hw_t *hw_ctx, uint32 cpu_no)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard)
    _rcvry_mac_hang_evt_set(pcard, cpu_no);
}

void wave_rcvry_mac_hang_evt_set_all (mtlk_hw_t *hw_ctx)
{
  uint32 core_no;
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard) {
    for (core_no = 0; core_no < MAX_NUM_OF_FW_CORES; core_no++)
      mtlk_osal_event_set(&pcard->rcvry_ctx.mac_hang_evt[core_no]);
  }
}

static int _rcvry_mac_hang_evt_wait (wave_rcvry_card_cfg_t *pcard, uint32 cpu_no, uint32 wait_time)
{
  MTLK_ASSERT(cpu_no < MAX_NUM_OF_FW_CORES);
  return mtlk_osal_event_wait(&pcard->rcvry_ctx.mac_hang_evt[cpu_no], wait_time);
}

int wave_rcvry_mac_hang_evt_is_set (mtlk_hw_t *hw_ctx, uint32 cpu_no)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard)
    return _rcvry_mac_hang_evt_wait(pcard, cpu_no, 0);

  return MTLK_ERR_OK;
}

int wave_rcvry_mac_hang_evt_wait (mtlk_hw_t *hw_ctx, uint32 cpu_no)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (pcard)
    return _rcvry_mac_hang_evt_wait(pcard, cpu_no, WAVE_RCVRY_FW_ASSERT_IND_TIMEOUT);

  return MTLK_ERR_OK;
}

static void _rcvry_umac_assert_evt_simulate (wave_rcvry_card_cfg_t *pcard)
{
  mtlk_hw_state_e hw_state;

  wave_hw_mmb_get_prop(pcard->hw_ctx, MTLK_HW_PROP_STATE, &hw_state, sizeof(hw_state));

  /* FW Assert can be done only if FW is in Steady State */
  if (wave_hw_is_init_state(hw_state)) {
    uint32 cpu_no = hw_assert_type_to_core_nr(pcard->hw_ctx, MTLK_CORE_UI_ASSERT_TYPE_FW_UMIPS);
    ILOG1_DD("CID-%02x: Simulating FW MAC#%d assert", pcard->card_idx, cpu_no);
    _rcvry_mac_hang_evt_set(pcard, cpu_no);
  }
}



#ifdef CONFIG_WAVE_RTLOG_REMOTE
static void _rcvry_process_mac_fatal_log (mtlk_hw_t *hw, const APP_FATAL *data)
{
#ifdef CONFIG_WAVE_DEBUG
  int res;
#endif
  mtlk_log_event_t log_event;

  /*LOG_MAKE_INFO(log_evt, ver, dsize, wif, dest, prior, exp, oid, gid, fid, lid)*/
  LOG_MAKE_INFO(log_event,
                0,                                      /* version */
                0,                                      /* data size */
                MAC_TO_HOST32(data->FWinterface),       /* firmware wlanif */
                0,                                      /* dest */
                0,                                      /* priority */
                0,                                      /* exp */
                MAC_TO_HOST32(data->OriginId),          /* firmware OID */
                MAC_TO_HOST32(data->GroupId),           /* firmware GID */
                MAC_TO_HOST32(data->FileId),            /* firmware FID */
                MAC_TO_HOST32(data->uCauseRegOrLineNum) /* firmware LID */
               );

  log_event.timestamp = MAC_TO_HOST32(data->uTimeStamp);

#if (IWLWAV_RTLOG_FLAGS & RTLF_REMOTE_ENABLED)
  wave_hw_mmb_set_prop(hw, MTLK_HW_LOG, &log_event, sizeof(log_event) + 0 /* data size */);
#else
  MTLK_UNREFERENCED_PARAM(hw);
#endif
#ifdef CONFIG_WAVE_DEBUG
  res = mtlk_nl_send_brd_msg(&log_event, sizeof(log_event), GFP_ATOMIC,
                             NETLINK_LOGSERVER_GROUP, NL_DRV_IRBM_NOTIFY);
  if (MTLK_ERR_OK != res)
    ELOG_D("Unable to notify LogServer. (No App joined this Group?) (%d)", res);
#endif
}
#endif

static void _rcvry_process_mac_hang (wave_rcvry_card_cfg_t *pcard, mtlk_hw_state_e hw_state, uint32 cpu_no)
{
  mtlk_hw_state_e ohw_state;

  wave_hw_mmb_get_prop(pcard->hw_ctx, MTLK_HW_PROP_STATE, &ohw_state, sizeof(ohw_state));

  /* Execute FW Assert if it's not done yet */
  if (ohw_state != MTLK_HW_STATE_MAC_ASSERTED && __rcvry_type_current_get(&pcard->rcvry_ctx) != RCVRY_TYPE_DBG){
    wave_hw_mmb_set_prop(pcard->hw_ctx, MTLK_HW_RESET, NULL, 0);
  }

  _rcvry_mac_fatal_pending_set(pcard, FALSE);

  /* it's an implementation of mtlk_set_hw_state() */
#if (IWLWAV_RTLOG_MAX_DLEVEL >= 1)
  ILOG1_DD("%i -> %i", ohw_state, hw_state);
#endif
  wave_hw_mmb_set_prop(pcard->hw_ctx, MTLK_HW_PROP_STATE, &hw_state, sizeof(hw_state));

  /* TODO: investigate - do we need this variable?
  nic->slow_ctx->mac_stuck_detected_by_sw = 0; */

  WLOG_DD("CID-%02x: MAC Hang detected, event=%d", pcard->card_idx, hw_state);

  /* it's an implementation of mtlk_df_ui_notify_notify_fw_hang() */
  WLOG_DD("CID-%02x: FW CPU#%d hang detected", pcard->card_idx, cpu_no);

  /* In case of fw debug send dump and exit */
  if (__rcvry_type_current_get(&pcard->rcvry_ctx) == RCVRY_TYPE_DBG){
    _rcvry_dump_schedule(pcard);
    return;
  }

  _rcvry_mac_hang_evt_set(pcard, cpu_no);
}

static void _rcvry_process_mac_fatal (wave_rcvry_card_cfg_t *pcard, const APP_FATAL *data)
{
  const char *const names[] = { /* APP_FATAL fields */
        "uCoreNum", "uTimeStamp", "uCauseRegOrLineNum", "uStatusReg",
        "uEpcReg", "FileId", "GroupId", "OriginId", "FWinterface", "uFirstFreePdOffset" };
  uint32 i, val, *arr = (uint32*)data;

  MTLK_UNUSED_VAR(names);       /* to avoid warning when debug messages disabled */

  /* Verify size of names[] array: */
  MTLK_STATIC_ASSERT((sizeof(APP_FATAL)/sizeof(uint32) == sizeof(names)/sizeof(names[0])));

  WLOG_DDDDDD("CID-%02x: MAC event: From FW_CORE%d: MAC fatal error: [GroupID: %u, FileID: %u, Line: %u], TS 0x%x",
               pcard->card_idx,
               MAC_TO_HOST32(data->uCoreNum),
               MAC_TO_HOST32(data->GroupId),
               MAC_TO_HOST32(data->FileId),
               MAC_TO_HOST32(data->uCauseRegOrLineNum),
               MAC_TO_HOST32(data->uTimeStamp));

  mtlk_txmm_terminate_waiting_msg(mtlk_hw_mmb_get_txmm_base(pcard->hw_ctx));
  mtlk_txmm_terminate_waiting_msg(mtlk_hw_mmb_get_txdm_base(pcard->hw_ctx));



  ELOG_D("CID-%02x: APP_FATAL", pcard->card_idx);
  for (i = 0; i < (sizeof(names) / sizeof(names[0])); i++) {
    val = MAC_TO_HOST32(arr[i]);
    ELOG_SDD("\t%-20s = 0x%08X (%u)", names[i], val, val);
  }

  __rcvry_mac_error_type_set(&pcard->rcvry_ctx, WAVE_RCVRY_ERRORTYPE_MAC_FATAL);

#if MTLK_USE_DIRECTCONNECT_DP_API
  #if DC_DP_API_VERSION_CODE >= DC_DP_API_VERSION(5,0,0)
  if (mtlk_mmb_dcdp_path_available(pcard->hw_ctx)) {
    /* Recovery of PDPoolBuffers */
    wave_hw_pdpool_return_list_t return_list;
    mtlk_error_t res = wave_mmb_build_pdpool_return_list(pcard->hw_ctx, MAC_TO_HOST32(data->uFirstFreePdOffset), &return_list);

    if (MTLK_ERR_OK == res) {
      if (DC_DP_SUCCESS != dc_dp_return_bufs(
          mtlk_hw_mmb_get_dp_port_id(pcard->hw_ctx, 0),
          return_list.ret_dma_list,
          return_list.len, 0)) {
        ELOG_V("dc_dp_return_bufs() error: cannot to recover buffers");
      }
      wave_mmb_cleanup_pdpool_return_list(&return_list);
    } else {
      ELOG_V("Can't build PD return list: no memory");
    }
  }
  #endif
#endif
#ifdef CONFIG_WAVE_RTLOG_REMOTE
  _rcvry_process_mac_fatal_log(pcard->hw_ctx, data);
#endif
  _rcvry_process_mac_hang(pcard, MTLK_HW_STATE_APPFATAL, MAC_TO_HOST32(data->uCoreNum));
}

void wave_rcvry_process_mac_fatal (mtlk_hw_t *hw_ctx, const APP_FATAL *data)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (!pcard)
    return;

  _rcvry_mac_fatal_pending_set(pcard, TRUE);
  _rcvry_process_mac_fatal(pcard, data);
}

static void _rcvry_process_mac_exception (wave_rcvry_card_cfg_t *pcard, const APP_FATAL *data)
{
  WLOG_DDDDDD("CID-%02x: MAC event: From FW_CORE%d: MAC exception: Cause 0x%x, EPC 0x%x, Status 0x%x, TS 0x%x",
               pcard->card_idx,
               MAC_TO_HOST32(data->uCoreNum),
               MAC_TO_HOST32(data->uCauseRegOrLineNum),
               MAC_TO_HOST32(data->uEpcReg),
               MAC_TO_HOST32(data->uStatusReg),
               MAC_TO_HOST32(data->uTimeStamp));

  mtlk_txmm_terminate_waiting_msg(mtlk_hw_mmb_get_txmm_base(pcard->hw_ctx));
  mtlk_txmm_terminate_waiting_msg(mtlk_hw_mmb_get_txdm_base(pcard->hw_ctx));



  __rcvry_mac_error_type_set(&pcard->rcvry_ctx, WAVE_RCVRY_ERRORTYPE_MAC_EXCEPTION);

  _rcvry_process_mac_hang(pcard, MTLK_HW_STATE_EXCEPTION, MAC_TO_HOST32(data->uCoreNum));
}

void wave_rcvry_process_mac_exception (mtlk_hw_t *hw_ctx, const APP_FATAL *data)
{
  wave_rcvry_card_cfg_t *pcard = _rcvry_card_hw_ctx_get(hw_ctx);

  if (!pcard)
    return;

  _rcvry_mac_fatal_pending_set(pcard, TRUE);
  _rcvry_process_mac_exception(pcard, data);
}
