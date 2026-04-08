/************************************************************************************
*
*  File:        wave_hal_stats.h
*  Description:
*  COPYRIGHT:
*    (C) Intel India Ltd.
*    All rights are strictly reserved. Reproduction or divulgence in any
*    form whatsoever is not permitted without written authority from the
*    copyright owner. Issued by Intel India Ltd
*
**************************************************************************************/
#ifndef _HAL_STATISTICS_H_
#define _HAL_STATISTICS_H_

#define HAL_NUM_OF_ANTS             (8)
#define HAL_NUM_OF_BANDWIDTHS       (4)
#define HAL_NUM_OF_TIDS             (16)

/* Defined as per Wifi HAL values
 * 1 for 20Mhz, 2 for 40 MHz, 4 for 80 MHz, 8 for 160 MHz, 10 for 80+80Mhz */
#define HAL_BW_20MHZ                (1)
#define HAL_BW_40MHZ                (2)
#define HAL_BW_80MHZ                (4)
#define HAL_BW_160MHZ               (8)

/* below Enum is defined similar to wifi_hal.h, for validating the Tx/Rx stats */
enum Tx_Rx_flag {
  HAS_BYTES,
  HAS_MSDUS,
  HAS_MPDUS,
  HAS_PPDUS,
  HAS_BW_80P80,
  HAS_RSSI_COMB,
  HAS_RSSI_ARRAY,
};

/* Structures defined in this file are similar to wifi_hal.h structures which are used by DWPAL.
 * DWPAL fetch these structures and forward it to wifi hal as part of PLUME API's */

typedef struct _PeerFlowStats_t {
  uint64 cli_rx_bytes;
  uint64 cli_tx_bytes;
  uint64 cli_rx_frames;
  uint64 cli_tx_frames;
  uint64 cli_rx_retries;
  uint64 cli_tx_retries;
  uint64 cli_rx_errors;
  uint64 cli_tx_errors;
  uint64 cli_rx_rate;
  uint64 cli_tx_rate;
} peerFlowStats;

typedef struct _PeerRateInfoRxStats_t {
  uint8 nss;
  uint8 mcs;
  uint16 bw;
  uint64 flags;
  uint64 bytes;
  uint64 msdus;
  uint64 mpdus;
  uint64 ppdus;
  uint64 retries;
  uint8 rssi_combined;
  uint8 rssi_array[HAL_NUM_OF_ANTS][HAL_NUM_OF_BANDWIDTHS];
} peerRateInfoRxStats;

typedef struct _PeerRateInfoTxStats_t {
  uint8 nss;
  uint8 mcs;
  uint16 bw;
  uint64 flags;
  uint64 bytes;
  uint64 msdus;
  uint64 mpdus;
  uint64 ppdus;
  uint64 retries;
  uint64 attempts;
} peerRateInfoTxStats;

typedef struct _ChannelStatistics_t {
  int32 ch_number;
  uint8 ch_in_pool;
  int32 ch_noise;
  uint8 ch_radar_noise;
  int32 ch_max_80211_rssi;
  int32 ch_non_80211_noise;
  int32 ch_utilization;
  uint64 ch_utilization_total;
  uint64 ch_utilization_busy;
  uint64 ch_utilization_busy_tx;
  uint64 ch_utilization_busy_rx;
  uint64 ch_utilization_busy_self;
  uint64 ch_utilization_busy_ext;
} mtlk_channel_stats;

typedef struct _PeerTidStats_t {
  uint8 ac;
  uint8 tid;
  uint64 ewma_time_ms;
  uint64 sum_time_ms;
  uint64 num_msdus;
} peerTidStats[HAL_NUM_OF_TIDS];


#endif /* _HAL_STATISTICS_H_ */
