/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MTLK_WSSA_DRVINFO_H__
#define __MTLK_WSSA_DRVINFO_H__

#include "mtlk_wss_debug.h"
#include "mhi_ieee_address.h"

typedef enum{
  MTLK_WSSA_11A_SUPPORTED,
  MTLK_WSSA_11B_SUPPORTED,
  MTLK_WSSA_11G_SUPPORTED,
  MTLK_WSSA_11N_SUPPORTED,
  MTLK_WSSA_11AC_SUPPORTED,
  MTLK_WSSA_11AX_SUPPORTED,
}mtlk_wssa_net_modes_supported_e;

typedef enum{
  VENDOR_UNKNOWN,
  VENDOR_LANTIQ,
  VENDOR_W101,
}mtlk_wssa_peer_vendor_t;

typedef enum{
  MTLK_PHY_MODE_AG,
  MTLK_PHY_MODE_B,
  MTLK_PHY_MODE_N,
  MTLK_PHY_MODE_AC,
  MTLK_PHY_MODE_AX,
}mtlk_wssa_phy_mode_e;

typedef struct mtlk_wssa_peer_traffic_stats{
  uint32 BytesSent;
  uint32 BytesReceived;
  uint32 PacketsSent;
  uint32 PacketsReceived;
}mtlk_wssa_peer_traffic_stats_t;

typedef struct mtlk_wssa_retrans_stats{
  uint32 Retransmissions;
  uint32 RetransCount;
  uint32 RetryCount;
  uint32 MultipleRetryCount;
  uint32 FailedRetransCount;
}mtlk_wssa_retrans_stats_t;

typedef struct mtlk_wssa_drv_tr181_peer_stats{
  uint32 StationId;
  uint32 NetModesSupported;
  mtlk_wssa_peer_traffic_stats_t traffic_stats;
  mtlk_wssa_retrans_stats_t retrans_stats;
  uint32 ErrorsSent;
  uint32 LastDataDownlinkRate;
  uint32 LastDataUplinkRate;
  int32 SignalStrength;
}mtlk_wssa_drv_tr181_peer_stats_t;

typedef struct mtlk_wssa_drv_peer_stats{
  mtlk_wssa_drv_tr181_peer_stats_t tr181_stats;
  int32 ShortTermRSSIAverage[4];
  int8 snr[4];
  uint32 AirtimeEfficiency;
  uint8 AirtimeUsage;
}mtlk_wssa_drv_peer_stats_t;

typedef struct mtlk_wssa_drv_peer_rate_info1{
  uint32 InfoFlag;
  uint32 PhyMode;
  int32 CbwIdx;
  int32 CbwMHz;
  int32 Scp;
  int32 Mcs;
  int32 Nss;
}mtlk_wssa_drv_peer_rate_info1_t;

typedef struct mtlk_wssa_drv_peer_rates_info{
  mtlk_wssa_drv_peer_rate_info1_t rx_mgmt_rate_info;
  uint32 RxMgmtRate;
  mtlk_wssa_drv_peer_rate_info1_t rx_data_rate_info;
  uint32 RxDataRate;
  mtlk_wssa_drv_peer_rate_info1_t tx_data_rate_info;
  uint32 TxDataRate;
  uint32 TxBfMode;
  uint32 TxStbcMode;
  uint32 TxPwrCur;
  uint32 TxMgmtPwr;
}mtlk_wssa_drv_peer_rates_info_t;

typedef struct mtlk_wssa_drv_traffic_stats{
  uint64 BytesSent;
  uint64 BytesReceived;
  uint64 PacketsSent;
  uint64 PacketsReceived;
  uint32 UnicastPacketsSent;
  uint32 UnicastPacketsReceived;
  uint32 MulticastPacketsSent;
  uint32 MulticastPacketsReceived;
  uint32 BroadcastPacketsSent;
  uint32 BroadcastPacketsReceived;
}mtlk_wssa_drv_traffic_stats_t;

typedef struct mtlk_wssa_drv_tr181_error_stats{
  uint32 ErrorsSent;
  uint32 ErrorsReceived;
  uint32 DiscardPacketsSent;
  uint32 DiscardPacketsReceived;
}mtlk_wssa_drv_tr181_error_stats_t;

typedef struct mtlk_wssa_drv_tr181_wlan_stats{
  mtlk_wssa_drv_traffic_stats_t traffic_stats;
  mtlk_wssa_drv_tr181_error_stats_t error_stats;
  mtlk_wssa_retrans_stats_t retrans_stats;
  uint32 ACKFailureCount;
  uint32 AggregatedPacketCount;
  uint32 UnknownProtoPacketsReceived;
}mtlk_wssa_drv_tr181_wlan_stats_t;

typedef struct mtlk_wssa_drv_tr181_hw{
  uint8 Enable;
  uint8 Channel;
}mtlk_wssa_drv_tr181_hw_t;

typedef struct mtlk_wssa_drv_tr181_hw_stats{
  mtlk_wssa_drv_traffic_stats_t traffic_stats;
  mtlk_wssa_drv_tr181_error_stats_t error_stats;
  uint32 FCSErrorCount;
  int32 Noise;
}mtlk_wssa_drv_tr181_hw_stats_t;

typedef struct mtlk_wssa_drv_recovery_stats{
  uint32 FastRcvryProcessed;
  uint32 FullRcvryProcessed;
  uint32 FastRcvryFailed;
  uint32 FullRcvryFailed;
}mtlk_wssa_drv_recovery_stats_t;

typedef struct mtlk_wssa_drv_peer_capabilities{
  uint32 NetModesSupported;
  uint32 WMMSupported;
  uint32 CBSupported;
  uint32 SGI20Supported;
  uint32 SGI40Supported;
  uint32 STBCSupported;
  uint32 LDPCSupported;
  uint32 BFSupported;
  uint32 Intolerant_40MHz;
  uint32 Vendor;
  uint32 MIMOConfigTX;
  uint32 MIMOConfigRX;
  uint32 AMPDUMaxLengthExp;
  uint32 AMPDUMinStartSpacing;
  uint32 AssociationTimestamp;
}mtlk_wssa_drv_peer_capabilities_t;

typedef struct mtlk_wssa_drv_hw_txm_stats{
  uint32 txmm_sent;
  uint32 txmm_cfmd;
  uint32 txmm_peak;
  uint32 txdm_sent;
  uint32 txdm_cfmd;
  uint32 txdm_peak;
}mtlk_wssa_drv_hw_txm_stats_t;

typedef struct mtlk_wssa_drv_mgmt_stats{
  uint32 MANFramesResQueue;
  uint32 MANFramesSent;
  uint32 MANFramesConfirmed;
  uint32 MANFramesReceived;
  uint32 MANFramesRetryDropped;
  uint32 BssMgmtTxQueFull;
  uint32 ProbeRespSent;
  uint32 ProbeRespDropped;
}mtlk_wssa_drv_mgmt_stats_t;

typedef struct mtlk_wssa_drv_hw_stats{
  mtlk_wssa_drv_recovery_stats_t rcvry_stats;
  mtlk_wssa_drv_hw_txm_stats_t   txm_stats;
  mtlk_wssa_drv_traffic_stats_t  traffic_stats;
  mtlk_wssa_drv_mgmt_stats_t     mgmt_stats;
  uint32 RadarsDetected;
  uint8  ChannelLoad;
  uint8  ChannelUtil;
  uint8  Airtime;
  uint32 AirtimeEfficiency;
}mtlk_wssa_drv_hw_stats_t;

typedef struct mtlk_wssa_drv_peer_list{
  IEEE_ADDR addr;
  uint32 is_sta_auth;
}mtlk_wssa_peer_list_t;
#endif /* __MTLK_WSSA_DRVINFO_H__ */
