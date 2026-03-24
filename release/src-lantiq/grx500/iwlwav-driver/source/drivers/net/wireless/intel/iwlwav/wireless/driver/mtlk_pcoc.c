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

#ifdef CPTCFG_IWLWAV_PMCU_SUPPORT

#include "mtlkinc.h"
#include <linux/cpufreq.h>
#include <cpufreq/ltq_cpufreq.h>
#include "mtlk_pcoc.h"

#define LOG_LOCAL_GID   GID_PCOC
#define LOG_LOCAL_FID   1

/* PMCU default parameters */
#define PCOC_PARAM_DEFAULT_INTERVAL_LOW2HIGH         (125u) /* msec */
#if (defined (CONFIG_SOC_GRX500) || defined (CONFIG_SOC_GRX500_A21))
#define PCOC_PARAM_DEFAULT_INTERVAL_HIGH2LOW        (1000u) /* msec */
#else
#define PCOC_PARAM_DEFAULT_INTERVAL_HIGH2LOW       (10000u) /* msec */
#endif
#define PCOC_PARAM_DEFAULT_LIM_LOW           (1000u << 10u) /* kbps */
#define PCOC_PARAM_DEFAULT_LIM_UP            (2000u << 10u) /* kbps */

/* PMCU parameters limits */
#define LIMIT_LOWER_MIN                 (1u << 10u) /* kbps */
#define LIMIT_LOWER_MAX           (1499999u << 10u) /* kbps */
#define LIMIT_UPPER_MIN                 (2u << 10u) /* kbps */
#define LIMIT_UPPER_MAX           (1500000u << 10u) /* kbps */
#define INTERVAL_MIN                         (125u) /* msec */
#define INTERVAL_MAX                      (200000u) /* msec */


#define PMCU_WLAN_ID                              0
#define PMCU_INIT_HIGH_TIMEOUT                 1000

#define CPUFREQ_RET(ret) ((ret) | (LTQ_CPUFREQ_MODULE_WLAN<<4))
#define PMCU_INIT_CHECK() \
  if (pPMCU == NULL) { \
    ELOG_V("PMCU should be initialized first"); \
    return MTLK_ERR_NO_MEM; \
  }

/* PMCU module private data types */
typedef struct {
    PMCU_params_t          params;
    mtlk_osal_spinlock_t   params_lock;
    mtlk_osal_spinlock_t   clients_list_lock;
    mtlk_dlist_t           clients_list;
    mtlk_osal_event_t      event_init_high;
    mtlk_osal_timestamp_t  ts_h;
    mtlk_osal_timestamp_t  ts_l;
    mtlk_osal_timestamp_t  ts_p;
    uint64                 high_lim;
    uint64                 low_lim;
    mtlk_atomic_t          freq_to_switch;
    enum ltq_cpufreq_state power_state;
    BOOL                   prechange_rejected;
    BOOL                   enabled;
} PMCU_t, *pPMCU_t;

typedef enum {
  PMCU_TRAFFIC_LOW,
  PMCU_TRAFFIC_HIGH,
  PMCU_TRAFFIC_UNDEF
} ePMCU_traffic_state_t;

/* Static variables */
static pPMCU_t pPMCU = NULL;
static int PMCU_Notifier(struct notifier_block *nb, unsigned long val, void * data);
static int PMCU_Power_State_Report(enum ltq_cpufreq_state *state);
static enum ltq_cpufreq_state PMCU_Power_State_Get(void);

static struct notifier_block PMCU_notifier_block = {
  .notifier_call = PMCU_Notifier
};

static struct ltq_cpufreq_module_info PMCU_module_info = {
.name                           = "WLAN driver frequency scaling support",
.pmcuModule                     = LTQ_CPUFREQ_MODULE_WLAN,
.pmcuModuleNr                   = PMCU_WLAN_ID,
.powerFeatureStat               = 1,
.ltq_cpufreq_state_get          = PMCU_Power_State_Report,
.ltq_cpufreq_pwr_feature_switch = NULL
};


/* Implementation */

/* Static functions */

static inline const char *switch_str(ePMCU_switch_freq switch_freq)
{
  const char *str[PMCU_SWITCH_TO_NONE+1] = {
    "high init",
    "high",
    "low",
    "none"
  };
  return ((switch_freq >= PMCU_SWITCH_TO_HIGH_INIT) && (switch_freq < PMCU_SWITCH_TO_NONE+1)) ?
         str[switch_freq] : "undefined";
}

static inline const char *power_state_str(enum ltq_cpufreq_state power_state)
{
  const char *str[LTQ_CPUFREQ_PS_BOOST+1] = {
    "PS_UNDEF",
    "PS_D0",
    "PS_D1",
    "PS_D2",
    "PS_D3",
    "PS_D0D3",
    "PS_BOOST"
  };
  return ((power_state >= LTQ_CPUFREQ_PS_UNDEF) && (power_state <= LTQ_CPUFREQ_PS_BOOST)) ?
         str[power_state] : "undefined";
}

static inline BOOL power_is_high(void)
{
  return (pPMCU->power_state == LTQ_CPUFREQ_PS_D0);
}

static inline BOOL power_is_low(void)
{
  return (pPMCU->power_state == LTQ_CPUFREQ_PS_D3);
}

static inline void ts_high_reset(void)
{
  pPMCU->ts_h = mtlk_osal_timestamp();
}

static inline void ts_low_reset(void)
{
  pPMCU->ts_l = mtlk_osal_timestamp();
}

static inline void ts_pre_reset(void)
{
  pPMCU->ts_p = mtlk_osal_timestamp();
}

static inline void timestamp_reset(void)
{
  ts_high_reset();
  ts_low_reset();
  ts_pre_reset();
}

static inline BOOL ts_high_passed(void)
{
  return (mtlk_osal_time_passed_ms(pPMCU->ts_h) > pPMCU->params.interval_low2high);
}

static inline BOOL ts_low_passed(void)
{
  return (mtlk_osal_time_passed_ms(pPMCU->ts_l) > pPMCU->params.interval_high2low);
}

static inline BOOL ts_pre_passed(void)
{
  return (mtlk_osal_time_passed_ms(pPMCU->ts_p) > pPMCU->params.interval_high2low);
}

static inline BOOL is_enabled(void)
{
  return pPMCU->enabled;
}

static int PMCU_Power_State_Report(enum ltq_cpufreq_state *state)
{
  if (pPMCU == NULL) {
    ELOG_V("PMCU should be initialized first");
    return LTQ_CPUFREQ_RETURN_DENIED;
  }
  *state = pPMCU->power_state;
  ILOG2_D("PS: %d", *state);

  return LTQ_CPUFREQ_RETURN_SUCCESS;
}

static int dummy_detector(void *ctx, ePMCU_request request)
{
  ILOG3_DD("Request: %d 0x%08x", request, ctx);
  return MTLK_ERR_OK;
}

static int dummy_notifier(void *ctx, ePMCU_request request)
{
  ILOG3_DD("Request: %d for hw: 0x%08x", request, ctx);
  return MTLK_ERR_OK;
}

static void PMCU_Params_Convert(void)
{
  PMCU_params_t *p_params = &pPMCU->params;

  if (p_params->interval_low2high < 1000u) {
    pPMCU->high_lim = p_params->limit_upper / (1000 / p_params->interval_low2high);
  } else {
    pPMCU->high_lim = (uint64)p_params->limit_upper * (uint64)(p_params->interval_low2high / 1000);
  }
  if (p_params->interval_high2low < 1000u) {
    pPMCU->low_lim = p_params->limit_lower / (1000 / p_params->interval_high2low);
  } else {
    pPMCU->low_lim = (uint64)p_params->limit_lower * (uint64)(p_params->interval_high2low / 1000);
  }
}

static enum ltq_cpufreq_state PMCU_Power_State_Get(void)
{
  struct ltq_cpufreq     *pcpufreq;
  enum ltq_cpufreq_state state;

  pcpufreq = ltq_cpufreq_get();
  MTLK_ASSERT(NULL!=pcpufreq);
  state = pcpufreq->cpufreq_cur_state;
  ILOG2_S("current CPUFREQ state: %s", power_state_str(state));
  return state;
}

static void *PMCU_Client_Find(void *ctx)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  pPMCU_client_t     client;
  BOOL               found = FALSE;

  if (NULL == ctx) {
    ELOG_V("context is NULL");
    return NULL;
  }

  client = NULL;
  mtlk_dlist_foreach(&pPMCU->clients_list, entry, head) {
    client = MTLK_LIST_GET_CONTAINING_RECORD(entry, PMCU_client_t, list_entry);
    if ((client) && (ctx == client->ctx)) {
      found = TRUE;
      break;
    }
  }

  return found ? client : NULL;
}

static int PMCU_Detectors(ePMCU_request request)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  pPMCU_client_t     client;
  int                ret = MTLK_ERR_OK;

  client = NULL;
  mtlk_dlist_foreach(&pPMCU->clients_list, entry, head) {
    client = MTLK_LIST_GET_CONTAINING_RECORD(entry, PMCU_client_t, list_entry);
    if ((client) && (client->detector)) {
      ret = client->detector(client->ctx, request);
      if (MTLK_ERR_OK != ret)
        break;
    }
  }

  return ret;
}

static int PMCU_Notifiers(ePMCU_request request)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  pPMCU_client_t     client;
  int                ret = MTLK_ERR_OK;

  client = NULL;
  mtlk_dlist_foreach(&pPMCU->clients_list, entry, head) {
    client = MTLK_LIST_GET_CONTAINING_RECORD(entry, PMCU_client_t, list_entry);
    if ((client) && (client->notifier)) {
      ret = client->notifier(client->ctx, request);
      if (MTLK_ERR_OK != ret)
        break;
    }
  }

  return ret;
}

static int PMCU_Pre_Notifier(enum ltq_cpufreq_state new_state)
{
  ePMCU_switch_freq switch_pending;
  int               ret = MTLK_ERR_OK;

  switch_pending = mtlk_osal_atomic_get(&pPMCU->freq_to_switch);

  ILOG2_SS("switch_pending: %s, power state: %s",
  switch_str(switch_pending), power_state_str(new_state));

  switch (new_state) {
    case LTQ_CPUFREQ_PS_D0: /*to high */
      if (PMCU_SWITCH_TO_HIGH_INIT != switch_pending) {
        ts_pre_reset();
        ret = PMCU_Notifiers(PMCU_REQ_HIGH_PRECHANGE);
      }
    break;
    case LTQ_CPUFREQ_PS_D1: /* to low */
    case LTQ_CPUFREQ_PS_D2:
    case LTQ_CPUFREQ_PS_D3:
      if (PMCU_SWITCH_TO_NONE == switch_pending) {
        if (!ts_pre_passed()) {
          ret = MTLK_ERR_PENDING;
          break;
        }
        ts_pre_reset();
      }
      ret = PMCU_Detectors(PMCU_REQ_LOW_PRECHANGE);
      if (MTLK_ERR_OK != ret)
        break;
      ret = PMCU_Notifiers(PMCU_REQ_LOW_PRECHANGE);
    break;
    default: /*no action */
    break;
  }

  pPMCU->prechange_rejected = (MTLK_ERR_OK != ret);

  return ret;
}

static int PMCU_Post_Notifier(enum ltq_cpufreq_state new_state)
{
  ePMCU_switch_freq switch_pending;
  int               ret = MTLK_ERR_OK;

  switch_pending = mtlk_osal_atomic_get(&pPMCU->freq_to_switch);

  ILOG2_SS("switch_pending: %s, power state: %s",
  switch_str(switch_pending), power_state_str(new_state));

  switch (new_state) {
    case LTQ_CPUFREQ_PS_D0: /*to high */
      if (PMCU_SWITCH_TO_HIGH_INIT != switch_pending) {
        ret = PMCU_Notifiers(PMCU_REQ_HIGH_POSTCHANGE);
      } else {
        mtlk_osal_event_set(&pPMCU->event_init_high);
      }
    break;
    case LTQ_CPUFREQ_PS_D1: /* to low */
    case LTQ_CPUFREQ_PS_D2:
    case LTQ_CPUFREQ_PS_D3:
      ret = PMCU_Notifiers(PMCU_REQ_LOW_POSTCHANGE);
    break;
    default: /*no action */
    break;
  }

  mtlk_osal_atomic_set(&pPMCU->freq_to_switch, PMCU_SWITCH_TO_NONE);
  pPMCU->power_state = new_state;

  return ret;
}

static void PMCU_Freq_Down_Force(unsigned val)
{
  if (CPUFREQ_PRECHANGE == val) {
    ILOG2_V("Prechange");
    PMCU_Notifiers(PMCU_REQ_LOW_PRECHANGE);
  }
  else if (CPUFREQ_POSTCHANGE == val) {
    ILOG2_V("Postchange");
    PMCU_Notifiers(PMCU_REQ_LOW_POSTCHANGE);
    pPMCU->power_state = LTQ_CPUFREQ_PS_D3;
  }
}

/* CPUFREQ callback notification function */
static int PMCU_Notifier(struct notifier_block *nb, unsigned long val,
                         void * data)
{
  enum ltq_cpufreq_state new_state, old_state;
  ePMCU_switch_freq      switch_pending;
  struct cpufreq_freqs   *freq = (struct cpufreq_freqs *)data;
  int                    ret = MTLK_ERR_OK;

  PMCU_INIT_CHECK();

  new_state = ltq_cpufreq_get_ps_from_khz(freq->new);
  old_state = ltq_cpufreq_get_ps_from_khz(freq->old);
  if ((new_state == LTQ_CPUFREQ_PS_UNDEF) || (old_state == LTQ_CPUFREQ_PS_UNDEF)) {
    ELOG_DD("unknown states notified, old_state=%d, new_state=%d", old_state, new_state);
    return CPUFREQ_RET(NOTIFY_STOP_MASK);
  }

  if ((!is_enabled()) && (new_state != LTQ_CPUFREQ_PS_D0)) {
    pPMCU->prechange_rejected = TRUE;
    ILOG1_S("PMCU is OFF, ignoring state %s", power_state_str(new_state));
    return CPUFREQ_RET(NOTIFY_STOP_MASK);
  }

  /* special case: freq reduction due to thermal problems */
  if ((freq->flags & CPUFREQ_PM_NO_DENY) && power_is_high()){
    ILOG1_V("CPUFREQ_PM_NO_DENY received, force switch to low freq");
    PMCU_Freq_Down_Force(val);
    return CPUFREQ_RET(NOTIFY_OK);
  }

  /* check allowed CPUfreq states changes */
  switch_pending = mtlk_osal_atomic_get(&pPMCU->freq_to_switch);
  switch (switch_pending) {
    case PMCU_SWITCH_TO_HIGH_INIT:
    case PMCU_SWITCH_TO_HIGH:
      if (LTQ_CPUFREQ_PS_D0 != new_state) {
        ELOG_V("can't switch to lower freq while request to high is pending");
        return CPUFREQ_RET(NOTIFY_STOP_MASK);
      }
    break;
    case PMCU_SWITCH_TO_LOW:
    case PMCU_SWITCH_TO_NONE:
    default:
      /* no pending requests, or low is pending - any change is OK */
    break;
  }

  switch(val) {
    case CPUFREQ_PRECHANGE:
      ret = PMCU_Pre_Notifier(new_state);
      if (MTLK_ERR_OK != ret)
        ILOG1_SD("prechange failed for state %s, ret=%d", power_state_str(new_state), ret);
      else
        ILOG2_S("prechange accepted, new_state=%s", power_state_str(new_state));
    break;
    case CPUFREQ_POSTCHANGE:
      if (pPMCU->prechange_rejected) {
        ILOG1_V("skip postchange, as prechange was rejected");
        ret = MTLK_ERR_PENDING;
        break;
      }
      ret = PMCU_Post_Notifier(new_state);
      if (MTLK_ERR_OK != ret)
        ELOG_D("postchange failed, ret=%d", ret);
    break;
    default:
      ELOG_D("unknown notifier action: val=%d", val);
    break;
  }

  return (MTLK_ERR_OK == ret) ? CPUFREQ_RET(NOTIFY_OK) : CPUFREQ_RET(NOTIFY_STOP_MASK);
}

static int PMCU_Switch_To_Low(void)
{
  int ret;

  ret = ltq_cpufreq_state_req(LTQ_CPUFREQ_MODULE_WLAN, PMCU_WLAN_ID, LTQ_CPUFREQ_PS_D0D3);
  if (ret != LTQ_CPUFREQ_RETURN_SUCCESS) {
    ILOG2_D("power state request D0D3 failed, ret=%d", ret);
    return MTLK_ERR_UNKNOWN;
  }
  ILOG2_V("low freq allowed");

  mtlk_osal_atomic_set(&pPMCU->freq_to_switch, PMCU_SWITCH_TO_LOW);
  return MTLK_ERR_OK;
}

static inline BOOL PMCU_Is_Busy(void)
{
  ePMCU_switch_freq switch_pending;
  switch_pending = mtlk_osal_atomic_get(&pPMCU->freq_to_switch);
  ILOG3_D("switch_pending: %d", switch_pending);
  return (PMCU_SWITCH_TO_NONE != switch_pending);
}

static int PMCU_Switch_To_High(ePMCU_switch_freq freq)
{
  int ret;

  ret = ltq_cpufreq_state_req(LTQ_CPUFREQ_MODULE_WLAN, PMCU_WLAN_ID, LTQ_CPUFREQ_PS_D0);
  if (ret != LTQ_CPUFREQ_RETURN_SUCCESS) {
    ILOG1_DDD("Power state request D0 failed, ret=%d, is_busy %d, cur state in drv %d",
              ret, PMCU_Is_Busy(), pPMCU->power_state);
    return MTLK_ERR_UNKNOWN;
  }
  ILOG2_V("high freq requested");

  mtlk_osal_atomic_set(&pPMCU->freq_to_switch, freq);

  if (PMCU_SWITCH_TO_HIGH_INIT == freq) {
    mtlk_osal_event_reset(&pPMCU->event_init_high);
    ret = mtlk_osal_event_wait(&pPMCU->event_init_high, PMCU_INIT_HIGH_TIMEOUT);
    if (MTLK_ERR_OK != ret) {
      ELOG_V("high freq request timeout");
      mtlk_osal_atomic_set(&pPMCU->freq_to_switch, PMCU_SWITCH_TO_NONE);
      return MTLK_ERR_PROHIB;
    }
  }

  return MTLK_ERR_OK;
}

static BOOL PMCU_High_Detected(void)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  pPMCU_client_t     client;
  uint64             res = 0;

  client = NULL;
  mtlk_dlist_foreach(&pPMCU->clients_list, entry, head) {
    client = MTLK_LIST_GET_CONTAINING_RECORD(entry, PMCU_client_t, list_entry);
    if (client) {
      res += client->traffic_high;
      client->traffic_high = 0;
    }
  }

  return (res > pPMCU->high_lim);
}

static BOOL PMCU_Low_Detected(void)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  pPMCU_client_t     client;
  uint64             res = 0;

  client = NULL;
  mtlk_dlist_foreach(&pPMCU->clients_list, entry, head) {
    client = MTLK_LIST_GET_CONTAINING_RECORD(entry, PMCU_client_t, list_entry);
    if (client) {
      res += client->traffic_low;
      client->traffic_low = 0;
    }
  }

  if (res > pPMCU->low_lim)
    return FALSE;

  return (MTLK_ERR_OK == PMCU_Detectors(PMCU_SWITCH_TO_LOW));
}

static void PMCU_Params_Print(void)
{
  ILOG2_D("low2high      : %d ms", pPMCU->params.interval_low2high);
  ILOG2_D("high2low      : %d ms", pPMCU->params.interval_high2low);
  ILOG2_H("high threshold: %llu bytes/sec", pPMCU->params.limit_upper);
  ILOG2_H("low threshold : %llu bytes/sec", pPMCU->params.limit_lower);
}

/* Interface functions */

/*
 * wv_PMCU_Create
 */
int wv_PMCU_Create(void)
{
  int ret;

  /* check if PMCU was already initialized */
  if (pPMCU != NULL) {
    ELOG_V("PMCU already initialized");
    return MTLK_ERR_OK;
  }

  /* alloc memory for PMCU module */
  pPMCU = mtlk_osal_mem_alloc(sizeof(PMCU_t), LQLA_MEM_TAG_PMCU);
  if (NULL == pPMCU) {
    ELOG_V("PMCU memory alloc failed");
    return MTLK_ERR_NO_MEM;
  }

  /* register PMCU module in CPUfreq */
  ret = cpufreq_register_notifier(&PMCU_notifier_block, CPUFREQ_TRANSITION_NOTIFIER);
  if (ret) {
    ELOG_D("fail to register PMCU in CPUFreq, ret=%d", ret);
    return MTLK_ERR_UNKNOWN;
  }
  ltq_cpufreq_mod_list(&PMCU_module_info.list, LTQ_CPUFREQ_LIST_ADD);


  /* init event */
  if (MTLK_ERR_OK != mtlk_osal_event_init(&pPMCU->event_init_high)) {
    ELOG_V("failed to init event_init_high");
    return MTLK_ERR_UNKNOWN;
  }

  /* init locks */
  if (MTLK_ERR_OK != mtlk_osal_lock_init(&pPMCU->clients_list_lock)) {
    ELOG_V("failed to init clients_list_lock");
    return MTLK_ERR_UNKNOWN;
  }

  if (MTLK_ERR_OK != mtlk_osal_lock_init(&pPMCU->params_lock)) {
    ELOG_V("failed to init params_lock");
    return MTLK_ERR_UNKNOWN;
  }

  /* init list */
  mtlk_dlist_init(&pPMCU->clients_list);

  /* init variables */
  mtlk_osal_atomic_set(&pPMCU->freq_to_switch, PMCU_SWITCH_TO_NONE);
  {
    enum ltq_cpufreq_state pstate = PMCU_Power_State_Get();
    if (pstate != LTQ_CPUFREQ_PS_D3)
      pstate = LTQ_CPUFREQ_PS_D0;
    pPMCU->power_state = pstate;
  }
  pPMCU->prechange_rejected = FALSE;

  /* init default parameters */
  pPMCU->enabled                  = TRUE;
  pPMCU->params.interval_low2high = PCOC_PARAM_DEFAULT_INTERVAL_LOW2HIGH;
  pPMCU->params.interval_high2low = PCOC_PARAM_DEFAULT_INTERVAL_HIGH2LOW;
  pPMCU->params.limit_lower       = PCOC_PARAM_DEFAULT_LIM_LOW;
  pPMCU->params.limit_upper       = PCOC_PARAM_DEFAULT_LIM_UP;
  PMCU_Params_Print();
  PMCU_Params_Convert();

  /* init timestamp */
  timestamp_reset();

  ILOG2_V("PMCU initialized");

  return MTLK_ERR_OK;
}

/*
 * wv_PMCU_Cleanup
 */
int wv_PMCU_Cleanup(void)
{
  mtlk_dlist_entry_t *entry;
  mtlk_dlist_entry_t *head;
  pPMCU_client_t     client;
  int                ret;

  PMCU_INIT_CHECK();

  ret = cpufreq_unregister_notifier(&PMCU_notifier_block, CPUFREQ_TRANSITION_NOTIFIER);
  if (ret) {
    ELOG_D("fail to unregister PMCU in CPUFreq, ret=%d", ret);
  }
  ltq_cpufreq_mod_list(&PMCU_module_info.list, LTQ_CPUFREQ_LIST_DEL);

  mtlk_osal_lock_acquire(&pPMCU->clients_list_lock);

  mtlk_dlist_foreach(&pPMCU->clients_list, entry, head) {
    client = MTLK_LIST_GET_CONTAINING_RECORD(entry, PMCU_client_t, list_entry);
    ILOG3_D("removing client from list: 0x%08x", client->ctx);
    entry = mtlk_dlist_remove(&pPMCU->clients_list, entry);
    mtlk_osal_mem_free(client);
  }

  mtlk_osal_lock_release(&pPMCU->clients_list_lock);

  mtlk_osal_event_cleanup(&pPMCU->event_init_high);
  mtlk_osal_lock_cleanup(&pPMCU->params_lock);
  mtlk_osal_lock_cleanup(&pPMCU->clients_list_lock);
  mtlk_dlist_cleanup(&pPMCU->clients_list);
  mtlk_osal_mem_free(pPMCU);
  pPMCU = NULL;
  ILOG2_V("PMCU uninitialized");
  return MTLK_ERR_OK;
}

/*
 * wv_PMCU_Switch_Freq - initiate the switch of freq request to CPUfreq
 */
int  wv_PMCU_Switch_Freq(ePMCU_switch_freq freq)
{
  int                    ret = MTLK_ERR_OK;

  PMCU_INIT_CHECK();

  ILOG3_S("requested: %s", switch_str(freq));

  if (PMCU_Is_Busy()) {
    ILOG2_S("Ignoring request: %s due to previous is pending", switch_str(freq));
    return MTLK_ERR_OK;
  }

  /* issue high freq request only if one is really needed */
  if (((freq == PMCU_SWITCH_TO_HIGH) || (freq == PMCU_SWITCH_TO_HIGH_INIT)) && power_is_high()) {
    ILOG3_V("frequency is high, ignoring request");
    return MTLK_ERR_OK;
  }

  /* issue low freq request only if one is really needed */
  if ((freq == PMCU_SWITCH_TO_LOW)  && power_is_low()) {
    ILOG3_V("frequency is low, ignoring request");
    return MTLK_ERR_OK;
  }

  switch (freq) {
    case PMCU_SWITCH_TO_HIGH_INIT:
    case PMCU_SWITCH_TO_HIGH:
      ret = PMCU_Switch_To_High(freq);
    break;
    case PMCU_SWITCH_TO_LOW:
      ret = PMCU_Switch_To_Low();
    break;
    default:
    case PMCU_SWITCH_TO_NONE:
    break;
  }

  return ret;
}

/*
 * wv_PMCU_Client_Register
 */
int wv_PMCU_Client_Register(pPMCU_client_t pClient)
{
  pPMCU_client_t client = NULL;

  PMCU_INIT_CHECK();

  ILOG2_DDD("ctx: 0x%08x, detector: 0x%08x, notifier: 0x%08x", pClient->ctx, pClient->detector, pClient->notifier);
  if ((NULL == pClient->ctx) || (NULL == pClient->detector) || (NULL == pClient->notifier)) {
    ELOG_V("Client's context not properly initialized");
    return MTLK_ERR_PARAMS;
  }

  client = mtlk_osal_mem_alloc(sizeof(PMCU_client_t), LQLA_MEM_TAG_PMCU);
  if (NULL == client) {
    ELOG_V("Can't alloc client");
    return MTLK_ERR_NO_MEM;
  }
  *client = *pClient;
  client->traffic_high  = 0;
  client->traffic_low   = 0;
  client->traffic_delta = 0;

  mtlk_osal_lock_acquire(&pPMCU->clients_list_lock);
  mtlk_dlist_push_front(&pPMCU->clients_list, &client->list_entry);
  mtlk_osal_lock_release(&pPMCU->clients_list_lock);

  timestamp_reset();

  return MTLK_ERR_OK;
}

/*
 * wv_PMCU_Client_Unregister
 */
int wv_PMCU_Client_Unregister(void *ctx)
{
  pPMCU_client_t client;

  PMCU_INIT_CHECK();

  ILOG2_D("ctx: 0x%08x", ctx);

  client = PMCU_Client_Find(ctx);
  if (NULL == client) {
    ELOG_V("client not found");
    return MTLK_ERR_PARAMS;
  }

  ILOG2_D("unregistering client: 0x%08x", ctx);
  mtlk_osal_lock_acquire(&pPMCU->clients_list_lock);
  /* do not delete list entry, just assign func ptrs to a dummy ones */
  client->detector = dummy_detector;
  client->notifier = dummy_notifier;
  mtlk_osal_lock_release(&pPMCU->clients_list_lock);

  return MTLK_ERR_OK;
}

/*
 * wv_PMCU_Traffic_Report
 */
int  wv_PMCU_Traffic_Report(void *ctx, uint64 total_traffic)
{
  pPMCU_client_t client;
  BOOL           high_detected = FALSE;
  BOOL           low_detected  = FALSE;
  int            res = MTLK_ERR_OK;

  PMCU_INIT_CHECK();

  /* find adressed client */
  client = PMCU_Client_Find(ctx);
  if (NULL == client) {
    ELOG_V("client not found");
    return MTLK_ERR_PARAMS;
  }

  if (!is_enabled()) {
    client->traffic_high = 0;
    client->traffic_low = 0;
    client->traffic_delta = total_traffic;
    timestamp_reset();
    return MTLK_ERR_OK;
  }

  client->traffic_delta = (total_traffic - client->traffic_delta);
  client->traffic_high += client->traffic_delta;
  client->traffic_low  += client->traffic_delta;
  client->traffic_delta = total_traffic;

  /* low2high */
  if (ts_high_passed()) {
   high_detected = PMCU_High_Detected();
   ts_high_reset();
  }

  /* high2low */
  if (ts_low_passed()) {
    low_detected = PMCU_Low_Detected();
    ts_low_reset();
  }

  if (high_detected) { /* low2high is high prio, check it first */
    ILOG3_V("switch to high freq");
    res = wv_PMCU_Switch_Freq(PMCU_SWITCH_TO_HIGH);
    if (MTLK_ERR_OK == res)
      ts_pre_reset();
  } else if (low_detected) { /* high2low is lower prio, check it last */
    ILOG3_V("switch to low freq");
    res = wv_PMCU_Switch_Freq(PMCU_SWITCH_TO_LOW);
    if (MTLK_ERR_OK == res)
      ts_pre_reset();
  }

  if (MTLK_ERR_OK != res) {
    ILOG2_D("switch freq request failed, res %d", res);
  }

  return res;
}

/*
 * wv_PMCU_get_params
 */
PMCU_params_t* wv_PMCU_Get_Params(void)
{
  MTLK_ASSERT(NULL != pPMCU);

  return &pPMCU->params;
}

/*
 * wv_PMCU_set_params
 */
int wv_PMCU_Set_Params (const PMCU_params_t *params)
{
  PMCU_INIT_CHECK();
  ILOG3_V("Setting PMCU parameters");

  if ((params->interval_low2high < INTERVAL_MIN) || (params->interval_low2high > INTERVAL_MAX)) {
    ELOG_DD("Wrong PMCU configuration. Interval low2high must be in range %d..%d",
           INTERVAL_MIN, INTERVAL_MAX);
    return MTLK_ERR_PARAMS;
  }
  else if ((params->interval_high2low < INTERVAL_MIN) || (params->interval_high2low > INTERVAL_MAX)) {
    ELOG_DD("Wrong PMCU configuration. Interval high2low must be in range %d..%d",
           INTERVAL_MIN, INTERVAL_MAX);
    return MTLK_ERR_PARAMS;
  }
  else if (params->interval_high2low <= params->interval_low2high) {
    ELOG_V("Wrong PMCU configuration. Interval high2low must be less than low2high");
    return MTLK_ERR_PARAMS;
  }
  else if ((params->limit_lower < LIMIT_LOWER_MIN) ||
           (params->limit_lower == LIMIT_LOWER_MAX)) {
    ELOG_DD("Wrong PCMCU configuration. Lower limit must be in a range %d..%d",
           LIMIT_LOWER_MIN, LIMIT_LOWER_MAX);
    return MTLK_ERR_PARAMS;
  }
  else if ((params->limit_upper < LIMIT_UPPER_MIN) ||
           (params->limit_upper == LIMIT_UPPER_MAX)) {
    ELOG_DD("Wrong PCMCU configuration. Upper limit must be in range %d..%d",
           LIMIT_UPPER_MIN, LIMIT_UPPER_MAX);
    return MTLK_ERR_PARAMS;
  }
  else if ((params->limit_lower >= params->limit_upper)) {
    ELOG_V("Wrong PMCU configuration. Lower threshold must be less than upper.");
    return MTLK_ERR_PARAMS;
  }

  mtlk_osal_lock_acquire(&pPMCU->params_lock);
  pPMCU->params = *params;
  PMCU_Params_Print();
  PMCU_Params_Convert();
  mtlk_osal_lock_release(&pPMCU->params_lock);

  return MTLK_ERR_OK;
}

/*
 * wv_PMCU_Is_High
 */
BOOL wv_PMCU_Is_High(void)
{
  if (pPMCU == NULL) {
    ELOG_V("PMCU should be initialized first");
    return FALSE;
  }

  return power_is_high();
}

/*
* wv_PMCU_is_enabled_adm
*/
BOOL wv_PMCU_is_active(void)
{
  return (pPMCU == NULL ? FALSE : TRUE);
}

/*
* wv_PMCU_set_enabled_adm
*/
int wv_PMCU_set_enabled_op(const BOOL is_enabled)
{
  /*MTLK_UNREFERENCED_PARAM(is_enabled);*/
  return (pPMCU == NULL ? MTLK_ERR_NOT_READY : MTLK_ERR_OK);
}

/*
* wv_PMCU_get_traffic_state
*/
uint8 wv_PMCU_get_traffic_state(void)
{
  if (pPMCU == NULL)
    return (uint8)PMCU_TRAFFIC_UNDEF;

  if (power_is_high())
    return (uint8)PMCU_TRAFFIC_HIGH;

  return (uint8)PMCU_TRAFFIC_LOW;
}

/*
* wv_PMCU_is_active
*/
BOOL wv_PMCU_is_enabled_adm(void)
{
  if (pPMCU == NULL)
    return FALSE;

  return pPMCU->enabled;
}

/*
* wv_PMCU_set_enabled_op
*/
int wv_PMCU_set_enabled_adm(const BOOL is_enabled)
{
  int res = MTLK_ERR_OK;

  if (pPMCU == NULL)
    return MTLK_ERR_NOT_READY;

  ILOG2_D("is_enabled: %d", is_enabled);
  mtlk_osal_lock_acquire(&pPMCU->params_lock);
  pPMCU->enabled = is_enabled;
  mtlk_osal_lock_release(&pPMCU->params_lock);

  /* if disabled, set high freq and stay on it*/
  if (!is_enabled) {
    ILOG2_V("switch to high freq");
    res = wv_PMCU_Switch_Freq(PMCU_SWITCH_TO_HIGH);
    if (MTLK_ERR_OK != res) {
      ILOG2_D("switch freq request failed, res %d", res);
    }
  }

  return res;
}

#endif /*CPTCFG_IWLWAV_PMCU_SUPPORT*/
