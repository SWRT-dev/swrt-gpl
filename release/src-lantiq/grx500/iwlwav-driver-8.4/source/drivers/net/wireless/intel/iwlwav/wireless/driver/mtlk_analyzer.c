/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
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

int __MTLK_IFUNC
mtlk_peer_analyzer_init(mtlk_peer_analyzer_t *peer_analyzer)
{
  int i;
  for (i = 0; i < NUMBER_OF_RX_ANTENNAS; ++i) {
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

int32 __MTLK_IFUNC
mtlk_peer_analyzer_get_long_term_rssi(const mtlk_peer_analyzer_t *peer_analyzer, int rssi_index)
{
  int32 res;

#if MTLK_WWSS_WLAN_STAT_ANALYZER_RX_LONG_RSSI_ALLOWED
  mtlk_osal_lock_acquire((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
  res = _mtlk_time_averager_get_value(&peer_analyzer->long_rssi_average[rssi_index]);
  mtlk_osal_lock_release((mtlk_osal_spinlock_t*) &peer_analyzer->analyzer_lock);
#else
  res = 0;  /* is not processed */
#endif

  return res;
}

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
