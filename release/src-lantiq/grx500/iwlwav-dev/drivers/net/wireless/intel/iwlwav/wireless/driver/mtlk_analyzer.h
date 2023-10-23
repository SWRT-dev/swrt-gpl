/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef  __MTLK_ANALYZER__
#define  __MTLK_ANALYZER__

#include "vendor_cmds.h"

typedef struct __mtlk_time_averager_t
{
  uint32 counting_start_timestamp;
  uint32 averaging_time;
  uint32 samples_counter;
  int32  summator;
  int32  last_value;
} _mtlk_time_averager_t;

typedef struct __mtlk_count_aggregator_t
{
  uint32 samples_to_aggregate;
  uint32 samples_counter;
  int32  summator;
  int32  last_sum;
} _mtlk_count_aggregator_t;

typedef struct __mtlk_count_averager_t
{
  _mtlk_count_aggregator_t aggregator;
  uint32 samples_to_average;
} _mtlk_count_averager_t;

typedef struct __mtlk_per_second_averager_t
{
  uint32 counting_start_timestamp;
  uint32 averaging_time;
  uint32 averaging_time_sec;
  int32  summator;
  int32  last_sum;
} _mtlk_per_second_averager_t;

#define WAVE_MOVING_WIN_MAX_SIZE  (64) /* max window size */
#define WAVE_MOVING_WIN_TIME_MS   (3 * 1000)

/* Moving window averager. Window size is set on creation  */
typedef struct {
  uint32  summator;
  uint32  win_size;
  uint32  idx;
  /* will be allocated on creation */
  union {
    uint32  values32[0];
    uint8   values8[0];
  };
} wave_moving_win_avg_t;

/* Moving algo averager by one sample */
typedef struct {
  int   last_sample;
  int   avg_value;
} wave_moving_avg_t;

typedef struct _mtlk_peer_analyzer_t
{
  mtlk_osal_spinlock_t analyzer_lock;

#if MTLK_WWSS_WLAN_STAT_ANALYZER_LONG_NOISE_ALLOWED
  wave_moving_avg_t   long_noise_average[NUMBER_OF_RX_ANTENNAS];
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_LONG_RSSI_ALLOWED
  _mtlk_time_averager_t       long_rssi_average[NUMBER_OF_RX_ANTENNAS];
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED
  _mtlk_per_second_averager_t short_tx_throughput_average;
  _mtlk_per_second_averager_t long_tx_throughput_average;
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED
  _mtlk_per_second_averager_t short_rx_throughput_average;
  _mtlk_per_second_averager_t long_rx_throughput_average;
#endif

  wave_moving_win_avg_t   *tx_rate_mov_win_avg;
  wave_moving_win_avg_t   *rx_rate_mov_win_avg;
} mtlk_peer_analyzer_t;

wave_moving_win_avg_t *
wave_moving_win_averager_u32_create (uint32 win_size);

wave_moving_win_avg_t *
wave_moving_win_averager_u8_create (uint32 win_size);

#define wave_moving_win_averager_u32_cleanup(a)  wave_moving_win_averager_cleanup(a)
#define wave_moving_win_averager_u8_cleanup(a)   wave_moving_win_averager_cleanup(a)

static __INLINE void
wave_moving_win_averager_cleanup (wave_moving_win_avg_t **avg_ptr)
{
  mtlk_osal_mem_free(*avg_ptr);
  *avg_ptr = NULL;
}

#define wave_moving_win_averager_u32_get(a)  (uint32)wave_moving_win_averager_get(a)
#define wave_moving_win_averager_u8_get(a)   (uint8)wave_moving_win_averager_get(a)

static __INLINE uint32
wave_moving_win_averager_get (const wave_moving_win_avg_t *avg)
{
  uint32 summ;

  if (__UNLIKELY(!avg)) { /* freed already */
    return 0;
  }

  summ = avg->summator;
  summ += avg->win_size >> 1;  /* round up */
  return summ / avg->win_size;
}

static __INLINE void
wave_moving_win_averager_u32_proc (wave_moving_win_avg_t *avg, uint32 val)
{
  avg->summator -= avg->values32[avg->idx];
  avg->summator += val;
  avg->values32[avg->idx] = val;
  INC_WRAP_IDX(avg->idx, avg->win_size);
}

static __INLINE void
wave_moving_win_averager_u8_proc (wave_moving_win_avg_t *avg, uint8 val)
{
  avg->summator -= avg->values8[avg->idx];
  avg->summator += val;
  avg->values8[avg->idx] = val;
  INC_WRAP_IDX(avg->idx, avg->win_size);
}

static __INLINE void
wave_peer_analyzer_proc_txrx_rates(mtlk_peer_analyzer_t *peer_analyzer, uint32 tx_rate, uint32 rx_rate)
{
  mtlk_osal_lock_acquire(&peer_analyzer->analyzer_lock);
  wave_moving_win_averager_u32_proc(peer_analyzer->tx_rate_mov_win_avg, tx_rate);
  wave_moving_win_averager_u32_proc(peer_analyzer->rx_rate_mov_win_avg, rx_rate);
  mtlk_osal_lock_release(&peer_analyzer->analyzer_lock);
}

static __INLINE void
wave_peer_analyzer_get_txrx_rates (const mtlk_peer_analyzer_t *peer_analyzer, uint32 *tx_rate, uint32 *rx_rate)
{
  *tx_rate = wave_moving_win_averager_u32_get(peer_analyzer->tx_rate_mov_win_avg);
  *rx_rate = wave_moving_win_averager_u32_get(peer_analyzer->rx_rate_mov_win_avg);
}

static __INLINE uint32
wave_peer_analyzer_get_tx_rate (const mtlk_peer_analyzer_t *peer_analyzer)
{
  return wave_moving_win_averager_u32_get(peer_analyzer->tx_rate_mov_win_avg);
}

static __INLINE uint32
wave_peer_analyzer_get_rx_rate (const mtlk_peer_analyzer_t *peer_analyzer)
{
  return wave_moving_win_averager_u32_get(peer_analyzer->rx_rate_mov_win_avg);
}

int __MTLK_IFUNC
mtlk_peer_analyzer_init(mtlk_peer_analyzer_t *peer_analyzer, uint32 stat_poll_period_ms);

void __MTLK_IFUNC
mtlk_peer_analyzer_cleanup(mtlk_peer_analyzer_t *peer_analyzer);

#if MTLK_WWSS_WLAN_STAT_ANALYZER_LONG_NOISE_ALLOWED
void __MTLK_IFUNC
wave_peer_analyzer_process_noise_samples(mtlk_peer_analyzer_t *peer_analyzer, const int8 samples[]);

int8 __MTLK_IFUNC
wave_peer_analyzer_get_long_noise(const mtlk_peer_analyzer_t *peer_analyzer, unsigned idx);
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_LONG_RSSI_ALLOWED
void __MTLK_IFUNC
mtlk_peer_analyzer_process_rssi_sample(mtlk_peer_analyzer_t *peer_analyzer, const int8 rssi_samples[]);
#endif

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED
int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_short_term_rx(const mtlk_peer_analyzer_t *peer_analyzer);

int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_long_term_rx(const mtlk_peer_analyzer_t *peer_analyzer);

void __MTLK_IFUNC
mtlk_peer_analyzer_process_rx_packet(mtlk_peer_analyzer_t *peer_analyzer, uint32 data_size);

#endif /* MTLK_WWSS_WLAN_STAT_ANALYZER_RX_RATE_ALLOWED */

#if MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED
int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_short_term_tx(const mtlk_peer_analyzer_t *peer_analyzer);

int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_long_term_tx(const mtlk_peer_analyzer_t *peer_analyzer);

void __MTLK_IFUNC
mtlk_peer_analyzer_process_tx_packet(mtlk_peer_analyzer_t *peer_analyzer, uint32 data_size);

#endif /* MTLK_WWSS_WLAN_STAT_ANALYZER_TX_RATE_ALLOWED */

#endif /* __MTLK_ANALYZER__ */
