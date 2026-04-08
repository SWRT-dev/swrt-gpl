/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef  __MTLK_ANALYZER__
#define  __MTLK_ANALYZER__

#include "intel_vendor_shared.h"

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

typedef struct _mtlk_peer_analyzer_t
{
  mtlk_osal_spinlock_t analyzer_lock;

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
} mtlk_peer_analyzer_t;

int __MTLK_IFUNC
mtlk_peer_analyzer_init(mtlk_peer_analyzer_t *peer_analyzer);

void __MTLK_IFUNC
mtlk_peer_analyzer_cleanup(mtlk_peer_analyzer_t *peer_analyzer);

int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_long_term_rssi(const mtlk_peer_analyzer_t *peer_analyzer, int rssi_index);

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
