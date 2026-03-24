/******************************************************************************

         Copyright (c) 2020, MaxLinear, Inc.
         Copyright 2016 - 2020 Intel Corporation
         Copyright 2015 - 2016 Lantiq Beteiligungs-GmbH & Co. KG
         Copyright 2009 - 2014 Lantiq Deutschland GmbH
         Copyright 2007 - 2008 Infineon Technologies AG

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

*******************************************************************************/

#ifndef __MTLK_WSSA_DRVINFO_H__
#define __MTLK_WSSA_DRVINFO_H__

#include "mtlk_wss_debug.h"
#include "mhi_ieee_address.h"
#include "Statistics/WAVE600/Statistics_Descriptors.h"
#include "Statistics/WAVE600B/Statistics_Descriptors.h"
#include "Statistics/WAVE600D2/Statistics_Descriptors.h"

#ifdef CONFIG_WAVE_DEBUG
#define MAX_USP_IN_VHT_GROUP (4)
#define NUM_OF_BW            (4)
#define ACCESS_CATEGORY_NUM  (4)
#endif /* CONFIG_WAVE_DEBUG */

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
  int8  LongTermNoiseAverage[4];
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
  uint32 AirtimeEfficiency;
  uint8  ChannelLoad;
  uint8  ChannelUtil;
  uint8  Airtime;
}mtlk_wssa_drv_hw_stats_t;

typedef struct mtlk_wssa_drv_peer_list{
  IEEE_ADDR addr;
  uint32 is_sta_auth;
}mtlk_wssa_peer_list_t;


typedef struct {
  uint32 ccaTotal;    /* Total count for CCA sampling */
  uint32 has_ccaTotal;
  uint32 ccaIdle;     /* CCA idle count */
  uint32 has_ccaIdle;
  uint32 ccaIntf;     /* CCA interference count */
  uint32 has_ccaIntf;
  uint32 ccaTx;       /* CCA transmitting count */
  uint32 has_ccaTx;
  uint32 ccaRx;       /* CCA receiving count */
  uint32 has_ccaRx;
} wave_cca_stats_user_t;

typedef struct {
  uint64 ccaIdle;     /* CCA idle count */
  uint64 ccaIntf;     /* CCA interference count */
  uint64 ccaTx;       /* CCA transmitting count */
  uint64 ccaRx;       /* CCA receiving count */
  mtlk_osal_hr_timestamp_t time_on_channel;
} wave_cca_stats_t;

typedef struct {
  uint64 busy_self;              /* rx time in radio microsec */
  uint64 busy_tx;                /* tx time microsec */
  uint64 busy_rx;                /* rx time microsec */
  uint64 busy;                   /* busy time microsec */
  mtlk_osal_hr_timestamp_t time_on_channel; /* timestamp to calc time spent */
} wave_radio_usage_stats_internal_t;

typedef struct {
  uint32 time_on_channel;        /* time spent on the channel msec */
  uint32 busy;                   /* busy in msec */
  uint32 busy_self;              /* rx time in radio msec */
  uint32 busy_tx;                /* tx time msec */
  uint32 busy_rx;                /* rx time msec */
} wave_radio_usage_stats_t;

#ifdef CONFIG_WAVE_DEBUG
typedef struct mtlk_wssa_drv_wlan_host_if_qos{
  uint32 qosTxVap[ACCESS_CATEGORY_NUM];
}mtlk_wssa_drv_wlan_host_if_qos_t;

typedef struct mtlk_wssa_drv_wlan_host_if{
  uint32 txInUnicastHd;
  uint32 txInMulticastHd;
  uint32 txInBroadcastHd;
  uint32 txInUnicastNumOfBytes;
  uint32 txInMulticastNumOfBytes;
  uint32 txInBroadcastNumOfBytes;
  uint32 rxOutUnicastHd;
  uint32 rxOutMulticastHd;
  uint32 rxOutBroadcastHd;
  uint32 rxOutUnicastNumOfBytes;
  uint32 rxOutMulticastNumOfBytes;
  uint32 rxOutBroadcastNumOfBytes;
  uint32 agerCount;
}mtlk_wssa_drv_wlan_host_if_t;

typedef struct mtlk_wssa_drv_wlan_rx_stats{
  uint32 amsdu;
  uint32 amsduBytes;
  uint32 dropCount;
  uint32 mpduInAmpdu;
  uint32 octetsInAmpdu;
  uint32 rxCoorSecurityMismatch;
  uint32 tkipCount;
  uint32 securityFailure;
  uint32 mpduUnicastOrMngmnt;
  uint32 mpduRetryCount;
  uint32 dropMpdu;
  uint32 ampdu;
  uint32 mpduTypeNotSupported;
  uint32 replayData;
  uint32 replayMngmnt;
  uint32 bcMcCountVap;
}mtlk_wssa_drv_wlan_rx_stats_t;

typedef struct mtlk_wssa_drv_wlan_baa_stats{
  uint32 rtsSuccessCount;
  uint32 rtsFailure;
  uint32 transmitStreamRprtMSDUFailed;
  uint32 qosTransmittedFrames;
  uint32 transmittedAmsdu;
  uint32 transmittedOctetsInAmsdu;
  uint32 transmittedAmpdu;
  uint32 transmittedMpduInAmpdu;
  uint32 transmittedOctetsInAmpdu;
  uint32 beamformingFrames;
  uint32 ackFailure;
  uint32 failedAmsdu;
  uint32 retryAmsdu;
  uint32 multipleRetryAmsdu;
  uint32 amsduAckFailure;
  uint32 implicitBarFailure;
  uint32 explicitBarFailure;
  uint32 transmitStreamRprtMultipleRetryCount;
  uint32 transmitBw20;
  uint32 transmitBw40;
  uint32 transmitBw80;
  uint32 transmitBw160;
  uint32 rxGroupFrame;
  uint32 txSenderError;
}mtlk_wssa_drv_wlan_baa_stats_t;

typedef struct mtlk_wssa_drv_radio_rx_stats{
  uint32 barMpduCount;
  uint32 crcErrorCount;
  uint32 delCrcError;
}mtlk_wssa_drv_radio_rx_stats_t;

typedef struct mtlk_wssa_drv_radio_baa_stats{
  uint32 retryCount;
  uint32 multipleRetryCount;
  uint32 fwMngmntframesSent;
  uint32 fwMngmntFramesConfirmed;
  uint32 fwMngmntframesRecieved;
  uint32 fwctrlFramesSent;
  uint32 fwctrlFramesRecieved;
  uint32 fwMulticastReplayedPackets;
  uint32 fwPairWiseMicFailurePackets;
  uint32 groupMicFailurePackets;
  uint32 beaconProbeResponsePhyType;
  uint32 rxBasicReport;
  uint32 txFrameErrorCount;
}mtlk_wssa_drv_radio_baa_stats_t;

typedef struct mtlk_wssa_drv_tsman_init_tid_gl{

  uint32 InitTidStatsEventAddbaRequestWasSent;
  uint32 InitTidStatsEventAddbaRequestConfiremd;
  uint32 InitTidStatsEventAddbaResponseReceivedWithStatusSuccessAndValid;
  uint32 InitTidStatsEventAddbaResponseReceivedWithStatusSuccessAndInvalid;
  uint32 InitTidStatsEventAddbaResponseReceivedWithStatusFailure;
  uint32 InitTidStatsEventAddbaResponseTimeout;
  uint32 InitTidStatsEventBaAgreementOpened;
  uint32 InitTidStatsEventBarWasSent;
  uint32 InitTidStatsEventBarWasDiscarded;
  uint32 InitTidStatsEventBarWasAnsweredWithAck;
  uint32 InitTidStatsEventBarWasAnsweredWithBa;
  uint32 InitTidStatsEventInactivityTimeout;
  uint32 InitTidStatsEventDelbaWasSent;
  uint32 InitTidStatsEventDelbaReceived;
  uint32 InitTidStatsEventCloseTid;
  uint32 InitTidStatsEventOpenTid;
}mtlk_wssa_drv_tsman_init_tid_gl_t;

typedef struct mtlk_wssa_drv_tsman_init_sta_gl{
  uint32 InitStaStatsEventRemove;
  uint32 InitStaStatsEventAdd;
  uint32 InitStaStatsEventAddbaResponseReceivedWithIllegalTid;
}mtlk_wssa_drv_tsman_init_sta_gl_t;

typedef struct mtlk_wssa_drv_tsman_rcpt_tid_gl{
  uint32 RcpTidStatsEventAddbaRequestAccepted;
  uint32 RcpTidStatsEventAddbaRequestWasReceivedWithInvalidParameters;
  uint32 RcpTidStatsEventAddbaResponseWasSentWithStatusSuccess;
  uint32 RcpTidStatsEventAddbaResponseWasSentWithStatusFailure;
  uint32 RcpTidStatsEventBaAgreementOpened;
  uint32 RcpTidStatsEventBaAgreementClosed;
  uint32 RcpTidStatsEventBarWasReceivedWithBaAgreementOpened;
  uint32 RcpTidStatsEventInactivityTimeout;
  uint32 RcpTidStatsEventDelbaReceived;
  uint32 RcpTidStatsEventDelbaWasSent;
  uint32 RcpTidStatsEventIllegalAggregationWasReceived;
  uint32 RcpTidStatsEventCloseTid;
}mtlk_wssa_drv_tsman_rcpt_tid_gl_t;

typedef struct mtlk_wssa_drv_tsman_rcpt_sta_gl{
  uint32 RcptStaStatsEventRemove;
  uint32 RcptStaStatsEventAdd;
  uint32 RcptStaStatsEventAddbaRequestReceivedWithIllegalTid;
}mtlk_wssa_drv_tsman_rcpt_sta_gl_t;

typedef struct mtlk_wssa_drv_radio_link_adapt_stats{
  uint32 minPower[NUM_OF_BW];
  uint32 maxPower[NUM_OF_BW];
  uint32 bfReportReceivedCounter;
  uint32 protectionSentCounter;
  uint32 protectionSucceededCounter;
  uint32 rxDc[NUM_OF_BW];
  uint32 txLo[NUM_OF_BW];
  uint32 rxIq[NUM_OF_BW];
  uint32 txIq[NUM_OF_BW];
}mtlk_wssa_drv_radio_link_adapt_stats_t;

typedef struct mtlk_wssa_drv_multicast_stats{
  uint32 notEnoughClonePds;
  uint32 allClonesFinishedTransmission;
  uint32 noStationsInGroup;
}mtlk_wssa_drv_multicast_stats_t;

typedef struct mtlk_wssa_drv_training_man_stats{
  uint32 trainingTimerExpiredCounter;
  uint32 trainingStartedCounter;
  uint32 trainingFinishedSuccessfullyCounter;
  uint32 trainingFinishedUnsuccessfullyCounter;
  uint32 trainingNotStartedCounter;
}mtlk_wssa_drv_training_man_stats_t;

typedef struct mtlk_wssa_drv_group_man_stats{
  uint32 groupInfoDb;
}mtlk_wssa_drv_group_man_stats_t;

typedef struct mtlk_wssa_drv_general_stats{
  uint32 calibrationMask;
  uint32 onlineCalibrationMask;
  uint32 dropReasonClassifier;
  uint32 dropReasonDisconnect;
  uint32 dropReasonATF;
  uint32 dropReasonTSFlush;
  uint32 dropReasonReKey;
  uint32 dropReasonSetKey;
  uint32 dropReasonDiscard;
  uint32 dropReasonDsabled;
  uint32 dropReasonAggError;
  uint32 defragStart;
  uint32 defragEnd;
  uint32 defragTimeout;
  uint32 classViolationErrors;
  uint32 rxhErrors;
  uint32 rxSmps;
  uint32 rxOmn;
  uint32 txMngFromHost;
  uint32 txDataFromHost;
  uint32 cyclicBufferOverflow;
  uint32 beaconsTransmitted;
  uint32 debugStatistics;
}mtlk_wssa_drv_general_stats_t;

typedef struct mtlk_wssa_drv_cur_channel_stats{
  uint64 chUtilizationTotal;
  uint64 chUtilizationBusy;
  uint64 chUtilizationBusyTx;
  uint64 chUtilizationBusyRx;
  uint64 chUtilizationBusySelf;
  uint64 chUtilizationBusyExt;
  int32  channelMax80211Rssi;
  uint32 channelNum;
}mtlk_wssa_drv_cur_channel_stats_t;

typedef struct mtlk_wssa_drv_radio_phy_rx_stats{
  uint32 channel_load;
  int32  chNon80211Noise;
  int32  CWIvalue;
  uint32 channelNum;
  int32  txPower;
  uint32 irad;
  uint32 tsf;
  uint32 extStaRx;
  int8   noise[PHY_STATISTICS_MAX_RX_ANT];
  uint8  rf_gain[PHY_STATISTICS_MAX_RX_ANT];
  int8   backgroundNoise[PHY_STATISTICS_MAX_RX_ANT];
  uint8  activeAntMask;
}mtlk_wssa_drv_radio_phy_rx_stats_t;

typedef struct mtlk_wssa_drv_dynamic_bw_stats{
  uint32  dynamicBW20MHz;
  uint32  dynamicBW40MHz;
  uint32  dynamicBW80MHz;
  uint32  dynamicBW160MHz;
}mtlk_wssa_drv_dynamic_bw_stats_t;

typedef struct mtlk_wssa_drv_radio_link_adapt_mu_stats{
  uint32 protectionSentCounter;
  uint32 protectionSucceededCounter;
  uint32 DataPhyMode;
  uint32 powerData[NUM_OF_BW];
  uint32 scpData[NUM_OF_BW];
  uint32 dataBwLimit;
  uint32 groupGoodput;
  uint32 raState;
  uint32 raStability[MAX_USP_IN_VHT_GROUP];
  uint32 nssData[MAX_USP_IN_VHT_GROUP];
  uint32 mcsData[MAX_USP_IN_VHT_GROUP];
}mtlk_wssa_drv_radio_link_adapt_mu_stats_t;
#endif /* CONFIG_WAVE_DEBUG */
#endif /* __MTLK_WSSA_DRVINFO_H__ */
