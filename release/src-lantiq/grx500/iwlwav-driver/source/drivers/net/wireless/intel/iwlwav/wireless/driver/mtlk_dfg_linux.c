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
 * Driver framework implementation for Linux (Global DF)
 *
 */

#include "mtlkinc.h"
#include "mtlk_df_priv.h"
#include "mtlkirbd.h"
#include "mtlkirb_osdep.h"
#include "cpu_stat.h"

#include "hw_mmb.h"
#include "mtlk_vap_manager.h"
#include "mtlk_coreui.h"

#include "mtlk_gpl_helper.h"

#include "mtlkwlanirbdefs.h"

#ifdef CPTCFG_IWLWAV_BENCHMARK_TOOLS
#include "mtlk_dbg.h"
#endif

#include "core.h"
#include "mtlkhal.h"
#include "mtlk_df.h"
#include "mtlk_hs20.h"
#include "stadb.h"
#include "mtlknbufstats.h"

#define LOG_LOCAL_GID   GID_DFGLINUX
#define LOG_LOCAL_FID   1

#ifdef MTLK_DEBUG
int step_to_fail = 0;
#endif

/*****************************************************************
 * Private data types
 *****************************************************************/
typedef struct _mtlk_dfg_t
{
  mtlk_irbd_t  *drv_irbd;

  mtlk_df_proc_fs_node_t *proc_root_node;
  mtlk_df_proc_fs_node_t *proc_mtlk_node;

  /* list of child /proc entries (per card) */
  mtlk_dlist_t         df_list;
  mtlk_osal_spinlock_t  df_list_lock;

  mtlk_wss_t           *drv_wss;
  global_stadb_t        global_stadb;

  MTLK_DECLARE_INIT_STATUS;
} mtlk_dfg_t;

/*****************************************************************
 * Private variables
 *****************************************************************/
static mtlk_dfg_t mtlk_dfg = {0};
#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
static int mtlk_dfg_initialized = 0;
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */

/* TODO: DEV_DF made external for init in DFG will be fixed */
extern mtlk_hw_mmb_t mtlk_mmb_obj;

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
uint32 __MTLK_IFUNC
get_hw_time_stamp(void)
{
  mtlk_dlist_entry_t *head = NULL;
  mtlk_dlist_entry_t *df_entry = NULL;
  uint32 time_stamp = 0;

  if(!mtlk_dfg_initialized)
    return 0;

  mtlk_osal_lock_acquire(&mtlk_dfg.df_list_lock);

  mtlk_dlist_foreach(&mtlk_dfg.df_list, df_entry, head)
  {
    mtlk_vap_manager_t *vap_manager = mtlk_df_get_vap_manager(mtlk_df_get_df_by_dfg_entry(df_entry));
    mtlk_vap_handle_t   vap_handle;

    if (mtlk_vap_manager_get_master_vap(vap_manager, &vap_handle) == MTLK_ERR_OK) {
        time_stamp = mtlk_hw_get_timestamp(vap_handle);
    }

    break;
  }

  mtlk_osal_lock_release(&mtlk_dfg.df_list_lock);

  return time_stamp;
}
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */

/* here goes .seq_show wrappers. they are basically get the
 * pointer to call function which 
 * generates the output of the file.
 */
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
struct debug_mem_alloc_dump_ctx 
{ 
  mtlk_seq_entry_t *s;
  char             buf[512]; 
}; 

static int __MTLK_IFUNC 
_debug_mem_alloc_printf (mtlk_handle_t printf_ctx, 
                         const char   *format, 
                         ...) 
{ 
  int                              res; 
  va_list                          valst; 
  struct debug_mem_alloc_dump_ctx *ctx =  
    HANDLE_T_PTR(struct debug_mem_alloc_dump_ctx, printf_ctx); 

  va_start(valst, format);
  res = mtlk_vsnprintf(ctx->buf, sizeof(ctx->buf), format, valst);
  va_end(valst);

  mtlk_aux_seq_printf(ctx->s, "%s\n", ctx->buf);

  return res; 
} 

static int debug_mem_alloc_dump(mtlk_seq_entry_t *s, void *v)
{
  struct debug_mem_alloc_dump_ctx *ctx =  
    (struct debug_mem_alloc_dump_ctx *)vmalloc_tag(sizeof(*ctx), 
                                                   MTLK_MEM_TAG_DEBUG_DATA); 
  if (ctx) { 
    ctx->s = s; 

    mem_leak_dbg_print_allocators_info(_debug_mem_alloc_printf, 
                                       HANDLE_T(ctx)); 
    vfree_tag(ctx); 
    return 0; 
  } 

  return -ENOMEM;
}

static BOOL __MTLK_IFUNC
_debug_objpool_dump_clb (mtlk_objpool_t    *objpool,
                         mtlk_slid_t        creator_slid,
                         uint32             objects_number,
                         uint32             additional_allocations_size,
                         mtlk_handle_t      context)
{
  uint32 *cnt = HANDLE_T_PTR(uint32, context);

  MTLK_UNREFERENCED_PARAM(additional_allocations_size);

  (*cnt) += objects_number;
  return TRUE;
}

static int debug_objpool_dump (struct seq_file *s, void *v)
{
  int i;

  seq_printf(s,   "--------------------------------\n"); 
  seq_printf(s,   "|         Type         | Count |\n"); 
  seq_printf(s,   "--------------------------------\n"); 
  for (i = MTLK_OBJTYPES_START + 1; i < MTLK_OBJTYPES_END; i++) {
    uint32 cnt = 0;
    mtlk_objpool_enum_by_type(&g_objpool, 
                              i,
                              _debug_objpool_dump_clb, 
                              HANDLE_T(&cnt));
    seq_printf(s, "| %-20s | %5u |\n", 
               mtlk_objpool_get_type_name(i), cnt); 
  }
  seq_printf(s,   "--------------------------------\n"); 

  return 0; 
}
#endif /*CPTCFG_IWLWAV_ENABLE_OBJPOOL*/

static int debug_irb_topology_dump(mtlk_seq_entry_t *s, void *v)
{   
  mtlk_irbd_print_topology(s);
  return 0;
}

#ifdef CPTCFG_IWLWAV_CPU_STAT

static void cpu_stats_show(mtlk_seq_entry_t *s, void *v)
{
  int i;

  mtlk_aux_seq_printf(s,
             "\n"
             "CPU Utilization Statistics (measurement unit is '%s')\n"
             "\n"
             "-------------+-------------+-------------+-------------+-------------+\n"
             " Count       | Average     | Peek        | Peek SN     | Name\n"
             "-------------+-------------+-------------+-------------+-------------+\n",
             _CPU_STAT_GET_UNIT_NAME());

  _CPU_STAT_FOREACH_TRACK_IDX(i) {
    cpu_stat_node_t node;
    char name[32];
    uint64 avg_time;

    _CPU_STAT_GET_DATA(i, &node);

    if (!node.count)
      continue;

    _CPU_STAT_GET_NAME_EX(i, name, sizeof(name));

    /* NOTE: 64-bit division is not supported by default in Linux kernel space =>
     *       we should use the do_div() ASM macro here.
     */
    avg_time = node.total;
    do_div(avg_time, node.count); /* the result is stored in avg_delay */

    mtlk_aux_seq_printf(s,
               " %-12u| %-12u| %-12u| %-12u| %s %s\n",
               (uint32)node.count,
               (uint32)avg_time,
               (uint32)node.peak,
               (uint32)node.peak_sn,
               name,
               _CPU_STAT_IS_ENABLED(i)?"[*]":"");
  }

  mtlk_aux_seq_printf(s,
             "-------------+-------------+-------------+-------------+-------------+\n");
}


static int cpu_stats_seq_show(mtlk_seq_entry_t *s, void *v)
{
  cpu_stats_show(s, v);

  return 0;
}

static int cpu_stats_do_reset(struct file *file, const char *buffer,
                                         unsigned long count, void *data)
{
  mtlk_cpu_stat_reset();

  /* let it looks as all data has
   * been written successfully
   */
  return count;
}

static int cpu_stats_enable_read(char *page, off_t off, int count, void *data)
{
  int res = 0;
  char name[32];
  int i = 0;

  /* no support for offset */
  if (off){
    return 0;
  };

  /* scnprintf looks more preferable here,
   * but old kernels lack this function
   */

  res = mtlk_snprintf(page, count,
                      "%d\n",
                      _CPU_STAT_GET_ENABLED_ID());
  if (res < 0 || res >= count)
    return -ENOSPC;

  ILOG0_V("\n************************************************************\n"
        "* CPU Statistics Available Indexes:\n"
        "************************************************************\n");
  _CPU_STAT_FOREACH_TRACK_IDX(i) {

    _CPU_STAT_GET_NAME_EX(i, name, sizeof(name));
    
    ILOG0_DSDS("* %03d - %-30s (delay %d usec) %s",
         i, name, _CPU_STAT_GET_DELAY(i),
         _CPU_STAT_IS_ENABLED(i)?"[*]":"");
  }
  ILOG0_V("************************************************************");

  return res;
}

static int cpu_stats_enable_write(struct file *file, const char *buffer,
                                 unsigned long count, void *data)
{
  char buf[16];
  unsigned long len = min((unsigned long)sizeof(buf)-1, count);
  int val;

  /* if not all data copied return with error */
  if(copy_from_user(buf, buffer, len))
    return -EFAULT;

  /* put "\0" at the end of the read data */
  buf[len] = 0;

  /* sscanf allows to enter value in decimal, hex or octal */
  sscanf(buf, "%i", &val);
  mtlk_cpu_stat_enable(val);

  return wave_strlen(buf, len);
}

static int cpu_stats_delay_write(struct file *file, const char *buffer,
                                 unsigned long count, void *data)
{
  char buf[16];
  unsigned long len = min((unsigned long)sizeof(buf)-1, count);
  int track;
  uint32 delay;

  /* if not all data copied return with error */
  if(copy_from_user(buf, buffer, len))
    return -EFAULT;

  /* put "\0" at the end of the read data */
  buf[len] = 0;

  /* sscanf allows to enter value in decimal, hex or octal */
  sscanf(buf, "%i %u", &track, &delay);

  mtlk_cpu_stat_set_delay(track, delay);

  return wave_strlen(buf, len);
}

static int
_mtlk_dfg_cpu_stats_max_id_read(mtlk_seq_entry_t *s, void *v)
{
  mtlk_aux_seq_printf(s, "\n%d\n", CPU_STAT_ID_LAST - 1);

  return 0;
}
#endif /* CPTCFG_IWLWAV_CPU_STAT */

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
static int mtlk_df_ui_debug_pp_stats(mtlk_seq_entry_t *s, void *data)
{
  int                     j;
  uint32                  i;
  uint32                  size = 0;
  mtlk_nbuf_priv_stats_t *arr  = mtlk_nbuf_priv_stats_get_array(&size);

  if (!arr) {
    return (size == 0)?0:-ENOMEM;
  }

  for (j = 0; j < ARRAY_SIZE(arr[0].val); j++) {
    if (j) {
      mtlk_aux_seq_printf(s, ",");
    }
    mtlk_aux_seq_printf(s, "%s", mtlk_nbuf_priv_stats_get_name(j));
  }
  mtlk_aux_seq_printf(s, "\n");

  for (i = 0; i < size; i++) {
    for (j = 0; j < ARRAY_SIZE(arr[0].val); j++) {
      if (j) {
        mtlk_aux_seq_printf(s, ",");
      }
      mtlk_aux_seq_printf(s, "%u", arr[i].val[j]);
    }
    mtlk_aux_seq_printf(s, "\n");
  }

  mtlk_osal_mem_free(arr);

  return 0;
}

static int mtlk_df_ui_debug_pp_stats_lim_read (char *page, off_t off, int count, void *data)
{
  int res = 0;

  /* no support for offset */
  if (off){
    return 0;
  };

  /* scnprintf looks more preferable here,
   * but old kernels lack this function
   */
  res = mtlk_snprintf(page, count,
                      "%u\n",
                      mtlk_nbuf_priv_stats_get_max_entries());
  if (res < 0 || res >= count)
    return -ENOSPC;

  return res;
}

static int mtlk_df_ui_debug_pp_stats_lim_write(struct file *file, const char *buffer,
                                               unsigned long count, void *data)
{
  char buf[16];
  unsigned long len = min((unsigned long)sizeof(buf)-1, count);
  int val;

  /* if not all data copied return with error */
  if(copy_from_user(buf, buffer, len))
    return -EFAULT;

  /* put "\0" at the end of the read data */
  buf[len] = 0;

  /* sscanf allows to enter value in decimal, hex or octal */
  sscanf(buf, "%i", &val);
  mtlk_nbuf_priv_stats_set_max_entries(val);

  return wave_strlen(buf, len);
}

static int mtlk_df_ui_debug_pp_stats_reset(struct file *file, const char *buffer,
                                           unsigned long count, void *data)
{
  mtlk_nbuf_priv_stats_reset();

  /* let it looks as all data has
   * been written successfully
   */
  return count;
}
#endif

static int _mtlk_dfg_version_dump(mtlk_seq_entry_t *s, void *v)
{
  char *buffer;
  int res = 0;

  buffer = mtlk_osal_mem_alloc(MTLK_HW_VERSION_PRINTOUT_LEN, MTLK_MEM_TAG_VERSION_PRINTOUT);

  if (!buffer)
  {
    ELOG_V("Cannot allocate memory for version printout");
    res = -ENOMEM;
    goto finish;
  }

  mtlk_osal_lock_acquire(&mtlk_dfg.df_list_lock);
  mtlk_hw_mmb_global_version_printout(&mtlk_mmb_obj, buffer, MTLK_HW_VERSION_PRINTOUT_LEN);
  mtlk_osal_lock_release(&mtlk_dfg.df_list_lock);

  mtlk_aux_seq_printf(s, "%s", buffer);
  mtlk_osal_mem_free(buffer);

finish:
  return res;
}

static int mtlk_dfg_proc_init(mtlk_dfg_t *dfg)
{
  int res = MTLK_ERR_UNKNOWN;

  MTLK_ASSERT(NULL != dfg);
  if (NULL == dfg->proc_root_node) {
    dfg->proc_root_node = mtlk_df_proc_node_create("root", NULL);
    if (NULL == dfg->proc_root_node) {
      goto ERROR_PROC_RET;
    }

    /* /proc/net/mtlk contents are created upon first demand */
    dfg->proc_mtlk_node = mtlk_df_proc_node_create("mtlk", dfg->proc_root_node);
    if (NULL == dfg->proc_mtlk_node) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_seq_entry(
            "version", dfg->proc_mtlk_node, dfg, _mtlk_dfg_version_dump);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    res = mtlk_df_proc_node_add_seq_entry(
            "mem_alloc_dump", dfg->proc_mtlk_node, dfg, debug_mem_alloc_dump);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_seq_entry(
            "objpool_dump", dfg->proc_mtlk_node, dfg, debug_objpool_dump);
    if (MTLK_ERR_OK != res) {
        goto ERROR_PROC_RET;
    }
#endif /*CPTCFG_IWLWAV_ENABLE_OBJPOOL*/

    /* create IRB topology file */
    res = mtlk_df_proc_node_add_seq_entry(
            MTLK_IRB_INI_NAME, dfg->proc_mtlk_node, dfg, debug_irb_topology_dump);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

#ifdef CPTCFG_IWLWAV_CPU_STAT
    res = mtlk_df_proc_node_add_seq_entry(
            "cpu_stats", dfg->proc_mtlk_node, dfg, cpu_stats_seq_show);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_wo_entry(
            "cpu_stats_reset", dfg->proc_mtlk_node, dfg, cpu_stats_do_reset);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_rw_entry(
            "cpu_stats_enable", dfg->proc_mtlk_node, dfg, cpu_stats_enable_read, cpu_stats_enable_write);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_wo_entry(
            "cpu_stats_delay", dfg->proc_mtlk_node, dfg, cpu_stats_delay_write);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_seq_entry(
            "cpu_stats_max_id", dfg->proc_mtlk_node, dfg, _mtlk_dfg_cpu_stats_max_id_read);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }
#endif /* CPTCFG_IWLWAV_CPU_STAT */

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
    res = mtlk_df_proc_node_add_seq_entry(
          "ppkt_stats", dfg->proc_mtlk_node, dfg, mtlk_df_ui_debug_pp_stats);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_rw_entry(
      "ppkt_stats_limit", dfg->proc_mtlk_node, dfg, mtlk_df_ui_debug_pp_stats_lim_read, mtlk_df_ui_debug_pp_stats_lim_write);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }

    res = mtlk_df_proc_node_add_wo_entry(
      "ppkt_stats_reset", dfg->proc_mtlk_node, dfg, mtlk_df_ui_debug_pp_stats_reset);
    if (MTLK_ERR_OK != res) {
      goto ERROR_PROC_RET;
    }
#endif

  }

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  mtlk_dfg_initialized = 1;
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */

  return MTLK_ERR_OK;

ERROR_PROC_RET:
  return MTLK_ERR_UNKNOWN;
}

static void mtlk_dfg_proc_cleanup(mtlk_dfg_t *dfg)
{
  MTLK_ASSERT(NULL != dfg);

#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
    mtlk_df_proc_node_remove_entry("ppkt_stats", dfg->proc_mtlk_node);
    mtlk_df_proc_node_remove_entry("ppkt_stats_limit", dfg->proc_mtlk_node);
    mtlk_df_proc_node_remove_entry("ppkt_stats_reset", dfg->proc_mtlk_node);
#endif

#ifdef CPTCFG_IWLWAV_CPU_STAT
    mtlk_df_proc_node_remove_entry("cpu_stats_max_id", dfg->proc_mtlk_node);
    mtlk_df_proc_node_remove_entry("cpu_stats_delay", dfg->proc_mtlk_node);
    mtlk_df_proc_node_remove_entry("cpu_stats_enable", dfg->proc_mtlk_node);
    mtlk_df_proc_node_remove_entry("cpu_stats_reset", dfg->proc_mtlk_node);
    mtlk_df_proc_node_remove_entry("cpu_stats", dfg->proc_mtlk_node);
#endif /* CPTCFG_IWLWAV_CPU_STAT */

    mtlk_df_proc_node_remove_entry(MTLK_IRB_INI_NAME, dfg->proc_mtlk_node);
#ifdef CPTCFG_IWLWAV_ENABLE_OBJPOOL
    mtlk_df_proc_node_remove_entry("objpool_dump", dfg->proc_mtlk_node);
    mtlk_df_proc_node_remove_entry("mem_alloc_dump", dfg->proc_mtlk_node);
#endif /*CPTCFG_IWLWAV_ENABLE_OBJPOOL*/
    mtlk_df_proc_node_remove_entry("version", dfg->proc_mtlk_node);

    mtlk_df_proc_node_delete(dfg->proc_mtlk_node);
    dfg->proc_mtlk_node = NULL;

    mtlk_df_proc_node_delete(dfg->proc_root_node);
    dfg->proc_root_node = NULL;

#ifdef CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED
  mtlk_dfg_initialized = 0;
#endif /* CPTCFG_IWLWAV_TSF_TIMER_ACCESS_ENABLED */
  /* TODO DEV_DF is it needed here to enumerate and free child DF ? */
}

/* steps for init and cleanup */
MTLK_INIT_STEPS_LIST_BEGIN(mtlk_dfg)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_LOG_OSDEP_INIT)
#ifdef MTLK_DEBUG
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_STEP_TO_FAIL_SET)
#endif
#ifdef CPTCFG_IWLWAV_BENCHMARK_TOOLS
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_DBG_INIT)
#endif
#ifdef CPTCFG_IWLWAV_CPU_STAT
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_STAT_INIT)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_MMB_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_IRBD_DRV_ALLOC)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_IRBD_DRV_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_DF_LIST_INIT_LOCK)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_REGISTRY_INIT)
#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_PPKT_STATS_INIT)
#endif
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_PROC_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_WSS_MODULE_INIT)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_WSS_CREATE)
  MTLK_INIT_STEPS_LIST_ENTRY(mtlk_dfg, DFG_HS20_INIT)
MTLK_INIT_INNER_STEPS_BEGIN(mtlk_dfg)
MTLK_INIT_STEPS_LIST_END(mtlk_dfg);

void mtlk_dfg_cleanup(void)
{
  mtlk_dfg_t *pdfg = &mtlk_dfg;

  MTLK_CLEANUP_BEGIN(mtlk_dfg, MTLK_OBJ_PTR(pdfg))
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_HS20_INIT, MTLK_OBJ_PTR(pdfg),
                      mtlk_hs20_cleanup, ());
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_WSS_CREATE, MTLK_OBJ_PTR(pdfg),
                      mtlk_wss_delete, (pdfg->drv_wss));
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_WSS_MODULE_INIT, MTLK_OBJ_PTR(pdfg),
                      mtlk_wss_module_cleanup, ());
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_PROC_INIT, MTLK_OBJ_PTR(pdfg),
                      mtlk_dfg_proc_cleanup, (pdfg));
#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_PPKT_STATS_INIT, MTLK_OBJ_PTR(pdfg),
                      mtlk_nbuf_priv_stats_cleanup, ());
#endif
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_REGISTRY_INIT, MTLK_OBJ_PTR(pdfg),
                      mtlk_dlist_cleanup, (&pdfg->df_list));
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_DF_LIST_INIT_LOCK, MTLK_OBJ_PTR(pdfg),
                      mtlk_osal_lock_cleanup, (&pdfg->df_list_lock));
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_IRBD_DRV_INIT, MTLK_OBJ_PTR(pdfg), 
                      mtlk_irbd_cleanup, (pdfg->drv_irbd));
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_IRBD_DRV_ALLOC, MTLK_OBJ_PTR(pdfg), 
                      mtlk_irbd_free, (pdfg->drv_irbd));
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_MMB_INIT, MTLK_OBJ_PTR(pdfg), 
                      mtlk_hw_mmb_cleanup, (&mtlk_mmb_obj));
#ifdef CPTCFG_IWLWAV_CPU_STAT
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_STAT_INIT, MTLK_OBJ_PTR(pdfg),
                      mtlk_cpu_stat_cleanup, ())
#endif
#ifdef CPTCFG_IWLWAV_BENCHMARK_TOOLS
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_DBG_INIT, MTLK_OBJ_PTR(pdfg), 
                      mtlk_dbg_hres_cleanup, ())
#endif
#ifdef MTLK_DEBUG
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_STEP_TO_FAIL_SET, MTLK_OBJ_PTR(pdfg),
                      MTLK_NOACTION, ());
#endif
    MTLK_CLEANUP_STEP(mtlk_dfg, DFG_LOG_OSDEP_INIT, MTLK_OBJ_PTR(pdfg),
                      log_osdep_cleanup, ());
  MTLK_CLEANUP_END(mtlk_dfg, MTLK_OBJ_PTR(pdfg));
}

/* TODO: in multi-driver environment, driver number must be 
 *       requested from separate MTLK IRB driver.
 */
#define _mtlk_irbd_drv_name() MTLK_IRB_DRV_NAME "0"

int mtlk_dfg_init(void)
{
  mtlk_hw_mmb_cfg_t mmb_cfg;
  mtlk_dfg_t *pdfg = &mtlk_dfg;
  memset(pdfg, 0, sizeof(*pdfg));
  memset(&mmb_cfg, 0, sizeof(mmb_cfg));

  mmb_cfg.bist_check_permitted  = 1;
  mmb_cfg.no_pll_write_delay_us = 0;
  mmb_cfg.man_ind_msg_size      = mtlk_get_umi_man_ind_size();
  mmb_cfg.man_req_msg_size      = mtlk_get_umi_man_req_size();
  mmb_cfg.dbg_msg_size          = mtlk_get_umi_dbg_size();
  
  MTLK_INIT_TRY(mtlk_dfg, MTLK_OBJ_PTR(pdfg))
    MTLK_INIT_STEP_VOID(mtlk_dfg, DFG_LOG_OSDEP_INIT, MTLK_OBJ_PTR(pdfg),
                        log_osdep_init, ());
#ifdef MODULE
    ILOG2_V("Starting MTLK");
#endif
#ifdef MTLK_DEBUG
    MTLK_INIT_STEP_VOID(mtlk_dfg, DFG_STEP_TO_FAIL_SET, MTLK_OBJ_PTR(pdfg),
                        mtlk_startup_set_step_to_fail, (step_to_fail));
#endif

#ifdef CPTCFG_IWLWAV_BENCHMARK_TOOLS
    MTLK_INIT_STEP(mtlk_dfg, DFG_DBG_INIT, MTLK_OBJ_PTR(pdfg), 
                   mtlk_dbg_hres_init, ());
#endif
#ifdef CPTCFG_IWLWAV_CPU_STAT
    MTLK_INIT_STEP_VOID(mtlk_dfg, DFG_STAT_INIT, MTLK_OBJ_PTR(pdfg),
                        mtlk_cpu_stat_init, ())
#endif
    MTLK_INIT_STEP(mtlk_dfg, DFG_MMB_INIT, MTLK_OBJ_PTR(pdfg),
                   mtlk_hw_mmb_init, (&mtlk_mmb_obj, &mmb_cfg));
    MTLK_INIT_STEP_EX(mtlk_dfg, DFG_IRBD_DRV_ALLOC, MTLK_OBJ_PTR(pdfg), 
                      mtlk_irbd_alloc, (), 
                      pdfg->drv_irbd,
                      pdfg->drv_irbd != NULL,
                      MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(mtlk_dfg, DFG_IRBD_DRV_INIT, MTLK_OBJ_PTR(pdfg), 
                   mtlk_irbd_init, (pdfg->drv_irbd, mtlk_irbd_get_root(),
                   _mtlk_irbd_drv_name()));
    MTLK_INIT_STEP(mtlk_dfg, DFG_DF_LIST_INIT_LOCK, MTLK_OBJ_PTR(pdfg),
                       mtlk_osal_lock_init, (&pdfg->df_list_lock));
    MTLK_INIT_STEP_VOID(mtlk_dfg, DFG_REGISTRY_INIT, MTLK_OBJ_PTR(pdfg),
                       mtlk_dlist_init, (&pdfg->df_list));
#ifdef CPTCFG_IWLWAV_PER_PACKET_STATS
    MTLK_INIT_STEP(mtlk_dfg, DFG_PPKT_STATS_INIT, MTLK_OBJ_PTR(pdfg),
                   mtlk_nbuf_priv_stats_init, ());
#endif
    MTLK_INIT_STEP(mtlk_dfg, DFG_PROC_INIT, MTLK_OBJ_PTR(pdfg),
                   mtlk_dfg_proc_init, (pdfg));
    MTLK_INIT_STEP(mtlk_dfg, DFG_WSS_MODULE_INIT, MTLK_OBJ_PTR(pdfg),
                  mtlk_wss_module_init, ());
    MTLK_INIT_STEP_EX(mtlk_dfg, DFG_WSS_CREATE, MTLK_OBJ_PTR(pdfg),
                      mtlk_wss_create, (NULL, NULL, 0),
                      pdfg->drv_wss, pdfg->drv_wss != NULL, MTLK_ERR_NO_MEM);
    MTLK_INIT_STEP(mtlk_dfg, DFG_HS20_INIT, MTLK_OBJ_PTR(pdfg),
                   mtlk_hs20_init, ());
    MTLK_INIT_FINALLY(mtlk_dfg, MTLK_OBJ_PTR(pdfg))
  MTLK_INIT_RETURN(mtlk_dfg, MTLK_OBJ_PTR(pdfg), mtlk_dfg_cleanup, ())
}

mtlk_df_proc_fs_node_t* mtlk_dfg_get_drv_proc_node(void)
{
  return mtlk_dfg.proc_mtlk_node;
}

void _mtlk_dfg_register_df(mtlk_ldlist_entry_t *entry)
{
  mtlk_osal_lock_acquire(&mtlk_dfg.df_list_lock);
  mtlk_dlist_push_back(&mtlk_dfg.df_list, entry);
  mtlk_osal_lock_release(&mtlk_dfg.df_list_lock);
}

void _mtlk_dfg_unregister_df(mtlk_ldlist_entry_t *entry)
{
  mtlk_osal_lock_acquire(&mtlk_dfg.df_list_lock);
  mtlk_dlist_remove(&mtlk_dfg.df_list, entry);
  mtlk_osal_lock_release(&mtlk_dfg.df_list_lock);
}

mtlk_irbd_t * __MTLK_IFUNC
mtlk_dfg_get_driver_irbd(void)
{
  return mtlk_dfg.drv_irbd;
}

mtlk_wss_t * __MTLK_IFUNC
mtlk_dfg_get_driver_wss(void)
{
  return mtlk_dfg.drv_wss;
}

global_stadb_t * __MTLK_IFUNC
mtlk_dfg_get_driver_stadb(void)
{
  return &mtlk_dfg.global_stadb;
}
