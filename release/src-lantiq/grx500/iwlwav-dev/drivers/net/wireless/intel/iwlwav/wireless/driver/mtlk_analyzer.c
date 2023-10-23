/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#include "mtlkinc.h"
#include "mtlk_wss_debug.h"
#include "mtlk_coreui.h"
#include "mtlk_analyzer.h"

#define LOG_LOCAL_GID   GID_ANALYZER
#define LOG_LOCAL_FID   1

#define _MTLK_PEER_LONG_TERM_RSSI_MS                    (30 * MTLK_OSAL_MSEC_IN_SEC)
#define _MTLK_PEER_SHORT_THROUGHPUT_AVERAGE_SEC         (1)
#define _MTLK_PEER_LONG_THROUGHPUT_AVERAGE_SEC          (30)
#define _MTLK_PEER_RETRANSMITTED_NUMBER_SHORT_PACKETS   (100)

static __INLINE void
_mtlk_time_averager_process_sample(_mtlk_time_averager_t* averager, int32 sample_value)
{
  averager->summator += sample_value;
  averager->samples_counter++;
  if(mtlk_osal_time_passed(averager->counting_start_timestamp) > averager->averaging_time)
  {
    /* NOTE: without (int) type cast compiler performs unsignded division */
    averager->last_value = (int)averager->summator /
                           (int)averager->samples_counter;

    averager->summator = 0;
    averager->samples_counter = 0;
    averager->counting_start_timestamp = mtlk_osal_timestamp();
  }
}

static __INLINE int32
_mtlk_time_averager_get_value(const _mtlk_time_averager_t* averager)
{
  return averager->last_value;
}

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_LONG_RSSI_ALLOWED
static void
_mtlk_time_averager_init(_mtlk_time_averager_t* averager, uint32 averaging_time_ms)
{
  averager->averaging_time = mtlk_osal_ms_to_timestamp(averaging_time_ms);
  averager->counting_start_timestamp = mtlk_osal_timestamp();
  averager->last_value = 0;
  averager->samples_counter = 0;
  averager->summator = 0;
}
#endif

static __INLINE void
_mtlk_per_second_averager_process_sample(_mtlk_per_second_averager_t* averager, int32 sample_value)
{
  averager->summator += sample_value;
  if(mtlk_osal_time_passed(averager->counting_start_timestamp) > averager->averaging_time)
  {
    averager->last_sum = averager->summator;

    averager->summator = 0;
    averager->counting_start_timestamp = mtlk_osal_timestamp();
  }
}

static __INLINE int32
_mtlk_per_second_averager_get_value(const _mtlk_per_second_averager_t* averager)
{
  return averager->last_sum / averager->averaging_time_sec;
}

#if (MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED || MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED)
static void
_mtlk_per_second_averager_init(_mtlk_per_second_averager_t* averager, uint32 averaging_time_sec)
{
  averager->averaging_time = mtlk_osal_ms_to_timestamp(averaging_time_sec * MTLK_OSAL_MSEC_IN_SEC);
  averager->averaging_time_sec = averaging_time_sec;
  averager->counting_start_timestamp = mtlk_osal_timestamp();
  averager->last_sum = 0;
  averager->summator = 0;
}
#endif

static wave_moving_win_avg_t *
_wave_moving_win_averager_create (uint32 win_size, size_t value_size)
{
  uint32 limited_win_size = win_size;
  uint32 alloc_size;
  wave_moving_win_avg_t *avg = NULL;

  if (0 == win_size) {
    limited_win_size = 1; /* min */
  } else if (win_size > WAVE_MOVING_WIN_MAX_SIZE) {
    limited_win_size = WAVE_MOVING_WIN_MAX_SIZE; /* max */
  }
  if (limited_win_size != win_size) {
    ILOG0_DD("Requested window size (%u) is not supported, so it is changed to %u", win_size, limited_win_size);
  }

  alloc_size = sizeof(wave_moving_win_avg_t) + (limited_win_size * value_size);
  avg = mtlk_osal_mem_alloc(alloc_size, LQLA_MEM_TAG_TA);
  if (!avg) {
    ELOG_D("Can't allocate memory of %u bytes size", alloc_size);
    return NULL;
  }
  memset(avg, 0, alloc_size);
  avg->win_size = limited_win_size;

  return avg;
}

wave_moving_win_avg_t *
wave_moving_win_averager_u32_create (uint32 win_size)
{
  return _wave_moving_win_averager_create(win_size, sizeof(uint32));
}

wave_moving_win_avg_t *
wave_moving_win_averager_u8_create (uint32 win_size)
{
  return _wave_moving_win_averager_create(win_size, sizeof(uint8));
}

/*************************************************************************
 * Support of the logarithmic values processing
 *************************************************************************/
/* API input/output logarithmic values [dBm] in int8 (S8.0) format.
 * It is related to some statistics: Noise, RSSI etc.
 * Logarithmic averager will operate with data in S8.8 format.
 * Such a fixed-point representation is sufficient to achieve
 * an expected accuracy of 0.5 dBm.
 */
#define LOGVAL_FIXP_NBI       (8) /* number of bits of integer part for both */
#define LOGVAL_AVG_FIXP_NBF   (8) /* number of bits of fractional part [AVG] */
#define LOGVAL_DBM_FIXP_NBF   (0) /* number of bits of fractional part [dBm] */

#define LOGVAL_DBM_TO_AVG(__v) wave_fixp_scale(__v, LOGVAL_FIXP_NBI, LOGVAL_DBM_FIXP_NBF, LOGVAL_AVG_FIXP_NBF)
#define LOGVAL_AVG_TO_DBM(__v) wave_fixp_scale(__v, LOGVAL_FIXP_NBI, LOGVAL_AVG_FIXP_NBF, LOGVAL_DBM_FIXP_NBF)

/* Table of func f(x) = 10*log10(1 + 10^(-x/10))
 * Table access index corresponds to (x) in dBm
 * All the table values in fixed point S8.8 format
 */
static int _algo_misc_dbm_fx_table[] = {
    770,  649,  543,  451,  372,  305,  249,  202,
    163,  131,  105,   84,   68,   54,   43,   34,
     27,   21,   17,   13,   11,    8,    6,    5,
      4,    3,    2,    2,    1,    1,    1
};

static __INLINE int
_wave_algo_calc_misc_dbm_fx_val (unsigned x)
{
  if (x < ARRAY_SIZE(_algo_misc_dbm_fx_table))
    return _algo_misc_dbm_fx_table[x];
  else
    return 0;
}

/* Calculate the sum of two values in dBm */
int __MTLK_IFUNC
wave_algo_sum_dbm_values (int val1, int val2)
{
  int vmax, delta, d_dbm, fx;
  int res;

  if (val1 > val2) {
    vmax  = val1;
    delta = val1 - val2;
  } else {
    vmax  = val2;
    delta = val2 - val1;
  }

  d_dbm = LOGVAL_AVG_TO_DBM(delta);

  fx = _wave_algo_calc_misc_dbm_fx_val(d_dbm);
  res = vmax + fx;
  return res;
}

/* Moving average process for values in dBm.
 * Alfa filter:
 *   y(n) = (1 - alfa) * y(n-1) - alfa * x(n)
 *      alfa   - coefficient
 *      y(n-1) - previous value of y
 *
 * Alfa filter for values in dBm:
 *   y(n) = sum_dbm(y(n-1) + B, x(n) + A)
 *      A = 10*log10(alfa)
 *      B = 10*log10(1 - alfa)
 */
int __MTLK_IFUNC
wave_algo_moving_avg_process_sample (int last, int sample, int a, int b)
{
  int next;
  next = wave_algo_sum_dbm_values(last + b, sample + a);
  ILOG3_DDD("last %5d, sample %5d -> next %5d", last, sample, next);
  return next;
}

/*************************************************************************/

#if MTLK_WWSS_WLAN_STAT_ANALYZER_LONG_NOISE_ALLOWED

#define NOISE_AVG_INIT_VAL    LOGVAL_DBM_TO_AVG(MIN_NOISE)

static __INLINE void
_wave_peer_analyzer_long_noise_averager_init (wave_moving_avg_t *avg, int init_val)
{
  avg->avg_value   = init_val;
  avg->last_sample = init_val;
}

int8 __MTLK_IFUNC
wave_peer_analyzer_get_long_noise (const mtlk_peer_analyzer_t *peer_analyzer, unsigned idx)
{
  const wave_moving_avg_t *avg = &peer_analyzer->long_noise_average[idx];

  MTLK_ASSERT(peer_analyzer);
  MTLK_ASSERT(idx < ARRAY_SIZE(peer_analyzer->long_noise_average));
  if (__UNLIKELY(idx >= ARRAY_SIZE(peer_analyzer->long_noise_average))) {
    return NOISE_INVALID;
  }

  return LOGVAL_AVG_TO_DBM(avg->avg_value);
}

/* Coefficients for the average in AVG format (S8.8)
 * They are calculated for alfa = 0.02, to be tuned
 */
#define NOISE_LONG_AVG_COEF_A   (-4349) /* -16.989700 */
#define NOISE_LONG_AVG_COEF_B   (-22)   /*  -0.087739 */

void __MTLK_IFUNC
wave_peer_analyzer_process_noise_samples (mtlk_peer_analyzer_t *peer_analyzer, const int8 samples[])
{
  wave_moving_avg_t *avg;
  int sample;
  unsigned i;

  MTLK_ASSERT(peer_analyzer);

  for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
    avg = &peer_analyzer->long_noise_average[i];
    sample = samples[i];
    if (__LIKELY(NOISE_INVALID != sample)) {
      sample = LOGVAL_DBM_TO_AVG(sample);
    } else { /* use last one on invalid (unavailable) */
      sample = avg->last_sample;
    }

    avg->avg_value = wave_algo_moving_avg_process_sample(avg->avg_value, sample,
                                   NOISE_LONG_AVG_COEF_A, NOISE_LONG_AVG_COEF_B);
    avg->last_sample = sample;
  }
}

#endif

int __MTLK_IFUNC
mtlk_peer_analyzer_init(mtlk_peer_analyzer_t *peer_analyzer, uint32 stat_poll_period_ms)
{
  int i;
  uint32 win_size;

  MTLK_ASSERT(peer_analyzer != NULL);
  MTLK_ASSERT(stat_poll_period_ms != 0);
  if (stat_poll_period_ms == 0) {
    stat_poll_period_ms = 1; /* avoid div by 0 */
  }

  win_size = WAVE_MOVING_WIN_TIME_MS / stat_poll_period_ms;
  ILOG3_DD("stat_poll_period_ms %u, win_size %u", stat_poll_period_ms, win_size);

  peer_analyzer->tx_rate_mov_win_avg = wave_moving_win_averager_u32_create(win_size);
  if (!peer_analyzer->tx_rate_mov_win_avg) {
    return MTLK_ERR_NO_MEM;
  }
  peer_analyzer->rx_rate_mov_win_avg = wave_moving_win_averager_u32_create(win_size);
  if (!peer_analyzer->rx_rate_mov_win_avg) {
    wave_moving_win_averager_u32_cleanup(&peer_analyzer->tx_rate_mov_win_avg);
    return MTLK_ERR_NO_MEM;
  }

  for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
#if MTLK_WWSS_WLAN_STAT_ANALYZER_LONG_NOISE_ALLOWED
  _wave_peer_analyzer_long_noise_averager_init(&peer_analyzer->long_noise_average[i], NOISE_AVG_INIT_VAL);
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_LONG_RSSI_ALLOWED
    _mtlk_time_averager_init(&peer_analyzer->long_rssi_average[i], _MTLK_PEER_LONG_TERM_RSSI_MS);
#endif
  }

#if MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED
  _mtlk_per_second_averager_init(&peer_analyzer->short_tx_throughput_average, _MTLK_PEER_SHORT_THROUGHPUT_AVERAGE_SEC);
  _mtlk_per_second_averager_init(&peer_analyzer->long_tx_throughput_average, _MTLK_PEER_LONG_THROUGHPUT_AVERAGE_SEC);
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED
  _mtlk_per_second_averager_init(&peer_analyzer->short_rx_throughput_average, _MTLK_PEER_SHORT_THROUGHPUT_AVERAGE_SEC);
  _mtlk_per_second_averager_init(&peer_analyzer->long_rx_throughput_average, _MTLK_PEER_LONG_THROUGHPUT_AVERAGE_SEC);
#endif

  return mtlk_osal_lock_init(&peer_analyzer->analyzer_lock);
}

void __MTLK_IFUNC
mtlk_peer_analyzer_cleanup(mtlk_peer_analyzer_t *peer_analyzer)
{
  mtlk_osal_lock_acquire(&peer_analyzer->analyzer_lock);
  wave_moving_win_averager_u32_cleanup(&peer_analyzer->tx_rate_mov_win_avg);
  wave_moving_win_averager_u32_cleanup(&peer_analyzer->rx_rate_mov_win_avg);
  mtlk_osal_lock_release(&peer_analyzer->analyzer_lock);
  mtlk_osal_lock_cleanup(&peer_analyzer->analyzer_lock);
}


#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_LONG_RSSI_ALLOWED
void __MTLK_IFUNC
mtlk_peer_analyzer_process_rssi_sample(mtlk_peer_analyzer_t *peer_analyzer, const int8 rssi_samples[])
{
  int i;
  mtlk_osal_lock_acquire(&peer_analyzer->analyzer_lock);

  for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
    _mtlk_time_averager_process_sample(&peer_analyzer->long_rssi_average[i], rssi_samples[i]);
  }

  mtlk_osal_lock_release(&peer_analyzer->analyzer_lock);
}
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED
int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_short_term_rx(const mtlk_peer_analyzer_t *peer_analyzer)
{
  int32 res;

  mtlk_osal_lock_acquire((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  res = _mtlk_per_second_averager_get_value(&peer_analyzer->short_rx_throughput_average);
  mtlk_osal_lock_release((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  return res;
}
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED
int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_short_term_tx(const mtlk_peer_analyzer_t *peer_analyzer)
{
  int32 res;

  mtlk_osal_lock_acquire((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  res = _mtlk_per_second_averager_get_value(&peer_analyzer->short_tx_throughput_average);
  mtlk_osal_lock_release((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  return res;
}
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED
int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_long_term_rx(const mtlk_peer_analyzer_t *peer_analyzer)
{
  int32 res;

  mtlk_osal_lock_acquire((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  res = _mtlk_per_second_averager_get_value(&peer_analyzer->long_rx_throughput_average);
  mtlk_osal_lock_release((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  return res;
}
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED
int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_long_term_tx(const mtlk_peer_analyzer_t *peer_analyzer)
{
  int32 res;

  mtlk_osal_lock_acquire((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  res = _mtlk_per_second_averager_get_value(&peer_analyzer->long_tx_throughput_average);
  mtlk_osal_lock_release((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  return res;
}
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED
void __MTLK_IFUNC
mtlk_peer_analyzer_process_rx_packet(mtlk_peer_analyzer_t *peer_analyzer, uint32 data_size)
{
  mtlk_osal_lock_acquire((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);

  _mtlk_per_second_averager_process_sample(&peer_analyzer->short_rx_throughput_average, data_size);
  _mtlk_per_second_averager_process_sample(&peer_analyzer->long_rx_throughput_average, data_size);

  mtlk_osal_lock_release((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
}
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED
void __MTLK_IFUNC
mtlk_peer_analyzer_process_tx_packet(mtlk_peer_analyzer_t *peer_analyzer, uint32 data_size)
{
  mtlk_osal_lock_acquire((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);

  _mtlk_per_second_averager_process_sample(&peer_analyzer->short_tx_throughput_average, data_size);
  _mtlk_per_second_averager_process_sample(&peer_analyzer->long_tx_throughput_average, data_size);

  mtlk_osal_lock_release((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
}
#endif
