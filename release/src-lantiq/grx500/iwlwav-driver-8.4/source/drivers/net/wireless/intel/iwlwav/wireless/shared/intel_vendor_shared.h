/*
 * This module contains definitions of structures passed between driver and
 * hostapd. These structures should exactly match the ones defined in mtlk
 * driver file with the same name.
 * Copyright (c) 2018, Intel
 *
 */

#ifndef __INTEL_VENDOR_SHARED_H__
#define __INTEL_VENDOR_SHARED_H__

#include "ieee_addr.h"

/*******************************************************************************
*                           Command structures                                 *
*******************************************************************************/

#define COUNTRY_CODE_MAX_LEN 3

#define NUMBER_OF_RX_ANTENNAS (4)
#define INTEL_CSA_DEAUTH_TX_TIME_ARR_SIZE     2
#define INTEL_CSA_DEAUTH_TX_TIME_UC_IDX       0
#define INTEL_CSA_DEAUTH_TX_TIME_MC_IDX       1
#define INTEL_MAX_PROTECTED_DEAUTH_FRAME_LEN  44
#define INTEL_NON_PROTECTED_DEAUTH_FRAME_LEN  26
#ifndef GEN6_MAX_VAP
#define INTEL_MAX_VAP                         32
#else
#define INTEL_MAX_VAP                         GEN6_MAX_VAP
#endif

enum {
  CSA_DEAUTH_MODE_DISABLED = 0,
  CSA_DEAUTH_MODE_UNICAST,
  CSA_DEAUTH_MODE_BROADCAST
};

struct intel_vendor_initial_data_cfg
{
  char  alpha2[COUNTRY_CODE_MAX_LEN];
  u8  is_11b;
  u8  radar_detection;
  u8  ieee80211n_acax_compat;
} __attribute__ ((packed));

struct intel_vendor_blacklist_cfg {
  DEF_IEEE_ADDR(addr);
  u16 status;
  u8 remove;
  u8 snrProbeHWM;
  u8 snrProbeLWM;
} __attribute__ ((packed));

struct intel_vendor_steer_cfg {
  DEF_IEEE_ADDR(addr);
  DEF_IEEE_ADDR(bssid);
  u16 status;
} __attribute__ ((packed));

struct intel_vendor_unconnected_sta_req_cfg {
  u32 bandwidth;
  u32 freq;
  u32 center_freq1;
  u32 center_freq2;
  DEF_IEEE_ADDR(addr);
} __attribute__ ((packed));

struct intel_vendor_sta_info { /* corresponds to vendor_sta_info in Driver */
  u32 StationId;
  u32 NetModesSupported;
  u32 BytesSent;        /* Peer traffic */
  u32 BytesReceived;
  u32 PacketsSent;
  u32 PacketsReceived;
  u32 RetransCount;
  u32 FailedRetransCount;
  u32 RetryCount;
  u32 LastDataUplinkRate;
  u32 LastDataDownlinkRate;
  u32 RateInfoFlag; /* Is rate info valid */
  u32 RatePhyMode;  /* Network (Phy) Mode */
  u32 RateCbwMHz;   /* BW [MHz] */
  u32 RateMcs;      /* MCS index */
  u32 RateNss;      /* NSS */
  s8  snr[NUMBER_OF_RX_ANTENNAS];
  s8  ShortTermRSSIAverage[NUMBER_OF_RX_ANTENNAS];
  s32 SignalStrength;
  u32 TxMgmtPwr;
  u32 TxStbcMode;
} __attribute__ ((packed));

struct intel_vendor_traffic_stats {
  u64 BytesSent;
  u64 BytesReceived;
  u64 PacketsSent;
  u64 PacketsReceived;
  u32 UnicastPacketsSent;
  u32 UnicastPacketsReceived;
  u32 MulticastPacketsSent;
  u32 MulticastPacketsReceived;
  u32 BroadcastPacketsSent;
  u32 BroadcastPacketsReceived;
} __attribute__ ((packed));

struct intel_vendor_error_stats {
  u32 ErrorsSent;
  u32 ErrorsReceived;
  u32 DiscardPacketsSent;
  u32 DiscardPacketsReceived;
} __attribute__ ((packed));

struct intel_vendor_vap_info {
  struct intel_vendor_traffic_stats traffic_stats;
  struct intel_vendor_error_stats   error_stats;
  u32 RetransCount;
  u32 FailedRetransCount;
  u32 RetryCount;
  u32 MultipleRetryCount;
  u32 ACKFailureCount;
  u32 AggregatedPacketCount;
  u32 UnknownProtoPacketsReceived;
  u64 TransmittedOctetsInAMSDUCount;
  u64 ReceivedOctetsInAMSDUCount;
  u64 TransmittedOctetsInAMPDUCount;
  u64 ReceivedOctetsInAMPDUCount;
  u32 RTSSuccessCount;
  u32 RTSFailureCount;
  u32 TransmittedAMSDUCount;
  u32 FailedAMSDUCount;
  u32 AMSDUAckFailureCount;
  u32 ReceivedAMSDUCount;
  u32 TransmittedAMPDUCount;
  u32 TransmittedMPDUsInAMPDUCount;
  u32 AMPDUReceivedCount;
  u32 MPDUInReceivedAMPDUCount;
  u32 ImplicitBARFailureCount;
  u32 ExplicitBARFailureCount;
  u32 TwentyMHzFrameTransmittedCount;
  u32 FortyMHzFrameTransmittedCount;
  u32 SwitchChannel20To40;
  u32 SwitchChannel40To20;
  u32 FrameDuplicateCount;
} __attribute__ ((packed));

struct intel_vendor_radio_info {
  struct intel_vendor_traffic_stats traffic_stats;
  struct intel_vendor_error_stats error_stats;
  u8 Enable;
  u8 Channel;
  u32 FCSErrorCount;
  s32 Noise;
  u64 tsf_start_time;
  u8 load;
  u32 tx_pwr_cfg;
  u8 num_tx_antennas;
  u8 num_rx_antennas;
  u32 primary_center_freq; /* center frequency in MHz */
  u32 center_freq1;
  u32 center_freq2;
  u32 width; /* 20,40,80,... */
} __attribute__ ((packed));

/* ATF: Air Time Fairness (a variable length message) */
struct intel_vendor_sta_grant {
  u16 sid;
  u16 grant;
} __attribute__ ((packed));

struct intel_vendor_atf_quotas {
  u8 debug;
  u8 distr_type;    /* disabled / dynamic / static */
  u8 algo_type;     /* global or weighted */
  u8 weighted_type; /* PER_STATION / PER_STATION_PER_AC / PER_VAP / ... */
  u32 interval;     /* in microseconds */
  u32 free_time;    /* in microseconds */
  u16 nof_bss;      /* number of BSSes - keep for debug */
  u16 nof_sta;      /* cfg param: ap_max_num_sta */
  u16 nof_grants;   /* number of elements in sta_grant[] array */
  u16 data_len;     /* sizeof sta_grant[] array in bytes */
  struct intel_vendor_sta_grant sta_grant[0];  /* SID and STA grant[nof_sta]; */
} __attribute__ ((packed));

struct intel_vendor_mac_addr_list_cfg {
  DEF_IEEE_ADDR(addr);
  u8 remove;
} __attribute__ ((packed));

/* Data for channel switch vendor implementation */
struct intel_vendor_channel_switch_cfg {
  u8 csaDeauthMode; /* 0-disabled, 1-unicast, 2-multicast */
  u8 csaMcDeauthFrameLength; /* Length of deauth frame per VAP */
  u16 csaDeauthTxTime[INTEL_CSA_DEAUTH_TX_TIME_ARR_SIZE]; /* Unicast and multicast deauth frame transmit time */
  u8 csaDeauthFrames[INTEL_MAX_PROTECTED_DEAUTH_FRAME_LEN]; /* Deauth frames (optionally encrypted) per VAP */
} __attribute__ ((packed));

/*******************************************************************************
*                           Event structures                                   *
*******************************************************************************/

/* Data for EVENT_LTQ_CHAN_DATA events */
struct intel_vendor_channel_data
{
  u32 channel;
  int BW;
  u32 primary;
  u32 secondary;
  u32 freq;
  u32 load;
  int noise_floor;
  u32 busy_time;
  u32 total_time;
  u32 calibration; /* success mask */
  u32 num_bss;
  u32 dynBW20;
  u32 dynBW40;
  u32 dynBW80;
  u32 dynBW160;
  u32 tx_power;
  int rssi;
  u32 snr;
  int cwi_noise;
  u32 not_80211_rx_evt; /* Used only by legacy statistcis */
  u32 ext_sta_rx;

  u32 filled_mask;
} __attribute__ ((packed));

/* Data for EVENT_LTQ_UNCONNECTED_STA_RX events */
struct intel_vendor_unconnected_sta {
  u64 rx_bytes;
  u32 rx_packets;
  s8 rssi[NUMBER_OF_RX_ANTENNAS];
  DEF_IEEE_ADDR(addr);
  s8 noise[NUMBER_OF_RX_ANTENNAS];
  u16 rate;
} __attribute__ ((packed));

/* Data for EVENT_LTQ_WDS_CONNECT and EVENT_LTQ_WDS_DISCONNECT */
struct intel_vendor_wds_sta_info {
  char ifname[IFNAMSIZ + 1];
  u32 max_rssi;
  u32 sta_flags_mask;
  u32 sta_flags_set;
  size_t assoc_req_ies_len;
  u16 beacon_interval;
  DEF_IEEE_ADDR(mac_addr);
  u8 dtim_period;
  u8 protection;
  u8 short_preamble;
  u8 short_slot_time;
  u8 assoc_req_ies[0];
} __attribute__ ((packed));

/* Data for EVENT_LTQ_CSA_RECEIVED */
struct intel_vendor_csa_received {
  u32 bandwidth;
  u32 freq;
  u32 center_freq1;
  u32 center_freq2;
  u32 count; /* the number of TBTT's until the channel switch event */
} __attribute__ ((packed));

/* Data for EVENT_LTQ_RADAR events */
struct intel_vendor_radar {
  s32 width; /* enum nl80211_chan_width (drv) -> enum chan_width (hostap) */
  u32 center_freq;
  u32 center_freq1;
  u32 center_freq2;
  u8 radar_bit_map;
} __attribute__ ((packed));

/* Data for EVENT_LTQ_SOFTBLOCK events */
struct intel_vendor_event_msg_drop {
  DEF_IEEE_ADDR(addr);          /**< Client MAC Address         */
  u8 vap_id;                    /**< VAP ID                     */
  u8 msgtype;                   /**  Msgtype                    */
  u8 rx_snr;                    /**< SNR in DB                  */
  u8 blocked;                   /**< True if response blocked   */
  u8 rejected;                  /**< True if Auth Resp with rejection sent to STA       */
  u8 broadcast;                 /**< True if broadcast probe    */
  u8 reason;                    /**< Ignore for Probe Req       */
} __attribute__ ((packed));

#endif /* __INTEL_VENDOR_SHARED_H__ */
