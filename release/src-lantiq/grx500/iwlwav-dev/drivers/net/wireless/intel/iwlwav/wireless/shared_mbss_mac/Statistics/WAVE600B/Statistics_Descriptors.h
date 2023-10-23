/************************************************************************************
*
*	File:				Statistics_Descriptors.h
*	Description:	
*	COPYRIGHT:
*		(C) Intel Israel Ltd.
*		All rights are strictly reserved. Reproduction or divulgence in any
*		form whatsoever is not permitted without written authority from the
*		copyright owner. Issued by Intel Israel Ltd
*
**************************************************************************************/
#ifndef _STATISTICS_DESC_WAVE600B_H_
#define _STATISTICS_DESC_WAVE600B_H_


#ifdef CPU_ARC

#include "HwGlobalDefinitions.h"
#include "linkAdaptation_api.h"
#include "mhi_umi.h"
#include "GroupManager_API.h"
#include "PacketTrafficArbitrator_api.h"
#include "ieee80211.h"
#include "SharedDbTypes.h"
#include "TwtManager_API.h"
#include "Utils_Api.h"

#else

#define HW_NUM_OF_VAPS          			(32)
#define HW_NUM_OF_STATIONS      			(256)
#define NUM_OF_TIDS              			(8)
#define RATES_BIT_MASK_SIZE     			(2)
#define NUM_OF_BW               			(4)
#define TX_MU_GROUPS           				(16)
#define MU_NUM_OF_STATS       				(7)
#define PTA_STATE_NUM_OF_STATES 			(14)
#define PTA_EVENT_NUM_OF_EVENTS				(18)
#define PTA_RADIO_NUM_OF_TYPES  			(2)
#define ACCESS_CATEGORY_NUM					(4)
#define MAX_USP_IN_VHT_GROUP 				(4)
#define NUM_OF_MCS							(0xB)
#define NUM_OF_EXTENTION_RSSI				(4)
#define MAX_NUM_OF_ANTENNAS					(4)
#define TWT_MAX_INDIVIDUAL_AGREEMENTS_ALLOWED_PER_STA           (1)
#define MAX_NUM_OF_SP_BAND_0_B0 			(32) // number of SPs in Band_0 for other platforms than D2 - beware - also defined in twtManager.h
#define MAX_NUM_OF_SP_BAND_1_B0				(32) // number of SPs in Band_1 for other platforms than D2 - beware - also defined in twtManager.h

#if !defined GEN6_NUM_OF_BANDS
#define GEN6_NUM_OF_BANDS 					(2)
#endif
#endif //CPU_ARC

#if !defined MAX
#define MAX(x,y)      		  ((x) > (y) ? (x) : (y))
#endif

#ifdef CPU_ARC
#define HOST_INTERFACE_BYTE_COUNTER_RESET_VALUE				(0x1)

#ifndef WORKAROUND_FOR_HW_BUG_IN_OTFA_BYTE_COUNTER_CALCULATION
#error "Driver and FW byte counte usage is not match for Wave600B"
#endif //WORKAROUND_FOR_HW_BUG_IN_OTFA_BYTE_COUNTER_CALCULATION

#else
#define HOST_INTERFACE_BYTE_COUNTER_RESET_VALUE_WAVE600B	(0x1)
#endif


#define PHY_STATISTICS_MAX_RX_ANT      	 	(4)

typedef struct
{	
	uint32 txInUnicastHd;
	uint32 txInMulticastHd;
	uint32 txInBroadcastHd;
	uint32 txInUnicastNumOfBytes;
	uint32 txInMulticastNumOfBytes;
	uint32 txInBroadcastNumOfBytes;
	uint32 rxOutUnicatHd;
	uint32 rxOutMulticastHd;
	uint32 rxOutBroadcastHd;
	uint32 rxOutUnicastNumOfBytes;
	uint32 rxOutMulticastNumOfBytes;
	uint32 rxOutBroadcastNumOfBytes;
	uint32 agerCount;
#ifdef CPU_ARC
}  HostIfCountersPerVap_t;
#else
}  HostIfCountersPerVap_t_wave600b;
#endif


typedef struct 
{	
	uint32 rxOutStaNumOfBytes; 		
	uint32 agerPdNoTransmitCountSta;
#ifdef CPU_ARC
} HostIfCountersPerSta_t;
#else
} HostIfCountersPerSta_t_wave600b;
#endif


typedef struct 
{	
	uint32 qosByteCountSta[HW_NUM_OF_STATIONS][NUM_OF_TIDS];		
	uint32 qosTxSta[HW_NUM_OF_STATIONS][NUM_OF_TIDS];				
	uint32 qosTxVap[HW_NUM_OF_VAPS][ACCESS_CATEGORY_NUM];
#ifdef CPU_ARC
	HostIfCountersPerVap_t 			vapCounts[HW_NUM_OF_VAPS];
	HostIfCountersPerSta_t 			staCounts[HW_NUM_OF_STATIONS];
#else
	HostIfCountersPerVap_t_wave600b vapCounts[HW_NUM_OF_VAPS];
	HostIfCountersPerSta_t_wave600b staCounts[HW_NUM_OF_STATIONS];
#endif
#ifdef CPU_ARC
} HostIfCounters_t;
#else
} HostIfCounters_t_wave600b;
#endif


typedef struct 
{
	uint32 amsdu;
	uint32 amsduBytes;
	uint32 dropCount;
	uint32 mpduInAmpdu;
	uint32 octetsInAmpdu;
	uint32 rxCoordinatorSecurityMismatch;
	uint32 tkipCount;
	uint32 securityFailure;
#ifdef CPU_ARC
} RxCountersPerVap_t;
#else
} RxCountersPerVap_t_wave600b;
#endif

typedef struct 
{
	uint32 mpduUnicastOrMngmnt;
	uint32 mpduRetryCount;
	uint32 dropMpdu;
	uint32 ampdu;
	uint32 mpduTypeNotSupported;
	uint32 replayData;
	uint32 replayMngmnt;
	uint32 bcMcCountVap;
#ifdef CPU_ARC
} RxCountersRxppPerVap_t;
#else
} RxCountersRxppPerVap_t_wave600b;
#endif

typedef struct 
{
	uint32 rddelayed;				
	uint32 swUpdateDrop;			
	uint32 rdDuplicateDrop;			
	uint32 missingSn;				
#ifdef CPU_ARC
} RxCountersPerSta_t;
#else
} RxCountersPerSta_t_wave600b;
#endif

typedef struct 
{
	uint32 barMpduCount;
	uint32 crcErrorCount;
	uint32 delCrcError;			
#ifdef CPU_ARC
} RxCountersPerBand_t;
#else
} RxCountersPerBand_t_wave600b;
#endif


typedef struct 
{
	uint32 				rdCount[HW_NUM_OF_STATIONS];
#ifdef CPU_ARC
	RxCountersPerVap_t 			vapCounts[HW_NUM_OF_VAPS];
#else
	RxCountersPerVap_t_wave600b vapCounts[HW_NUM_OF_VAPS];
#endif
	uint16 				qosRxSta[HW_NUM_OF_STATIONS];
#ifdef CPU_ARC
	RxCountersPerSta_t 				staCounts[HW_NUM_OF_STATIONS];
	RxCountersRxppPerVap_t 			rxppVapCounts[HW_NUM_OF_VAPS];
	RxCountersPerBand_t 			bandCounts[GEN6_NUM_OF_BANDS];
#else
	RxCountersPerSta_t_wave600b 	staCounts[HW_NUM_OF_STATIONS];
	RxCountersRxppPerVap_t_wave600b rxppVapCounts[HW_NUM_OF_VAPS];
	RxCountersPerBand_t_wave600b 	bandCounts[GEN6_NUM_OF_BANDS];
#endif
#ifdef CPU_ARC
} RxCounters_t;
#else
} RxCounters_t_wave600b;
#endif

typedef struct 
{
	uint16 bufStsCnt0;
	uint16 bufStsCnt1;
	uint16 bufStsCnt2;
	uint16 bufStsCnt3;
	uint16 bufStsCnt4;
	uint16 bufStsCnt5;
	uint16 bufStsCnt6;
	uint16 bufStsCnt7;
	uint8 maskSelBitmap;
	uint8 reserved[3];
#ifdef CPU_ARC
} UplinkBsrcPerTidCnt_t;
#else
} UplinkBsrcPerTidCnt_t_wave600b;
#endif

typedef struct 
{
#ifdef CPU_ARC
	UplinkBsrcPerTidCnt_t BsrcPerTidCnt[HW_NUM_OF_STATIONS];
#else
	UplinkBsrcPerTidCnt_t_wave600b BsrcPerTidCnt[HW_NUM_OF_STATIONS];
#endif
#ifdef CPU_ARC
} UplinkBsrcTidCnt_t;
#else
} UplinkBsrcTidCnt_t_wave600b;
#endif

typedef struct 
{
	uint32 retryCount;
	uint32 multipleRetryCount;
#ifdef CPU_ARC
} BaaCountersRetryPerBand_t;
#else
} BaaCountersRetryPerBand_t_wave600b;
#endif

typedef struct
{
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
#ifdef CPU_ARC
} BaaCountersPerVap_t;
#else
} BaaCountersPerVap_t_wave600b;
#endif

typedef struct 
{
	uint32 mpduFirstRetransmission;		
	uint32 mpduTransmitted;
	uint32 mpduByteTransmitted;			
	uint32 mpduRetransmission;			
	uint32 channelTransmitTime;			
#ifdef CPU_ARC
} BaaCountersPerSta_t;
#else
} BaaCountersPerSta_t_wave600b;
#endif

typedef struct 
{
#ifdef CPU_ARC
	BaaCountersRetryPerBand_t 			retryCounts[GEN6_NUM_OF_BANDS];
	BaaCountersPerVap_t 				vapCounts[HW_NUM_OF_VAPS];
	BaaCountersPerSta_t 				staCounts[HW_NUM_OF_STATIONS];
#else
	BaaCountersRetryPerBand_t_wave600b 	retryCounts[GEN6_NUM_OF_BANDS];
	BaaCountersPerVap_t_wave600b	 	vapCounts[HW_NUM_OF_VAPS];
	BaaCountersPerSta_t_wave600b 		staCounts[HW_NUM_OF_STATIONS];
#endif
	uint32 fwMngmntframesSent[GEN6_NUM_OF_BANDS];
	uint32 fwMngmntFramesConfirmed[GEN6_NUM_OF_BANDS];
	uint32 fwMngmntframesRecieved[GEN6_NUM_OF_BANDS];
	uint32 fwctrlFramesSent[GEN6_NUM_OF_BANDS];
	uint32 fwctrlFramesRecieved[GEN6_NUM_OF_BANDS];
	uint32 fwMulticastReplayedPackets[GEN6_NUM_OF_BANDS];
	uint32 fwPairWiseMicFailurePackets[GEN6_NUM_OF_BANDS];
	uint32 groupMicFailurePackets[GEN6_NUM_OF_BANDS];
	uint32 beaconProbeResponsePhyType[GEN6_NUM_OF_BANDS];
	uint32 rxBasicReport[GEN6_NUM_OF_BANDS];
	uint32 txFrameErrorCount[GEN6_NUM_OF_BANDS];
	uint32 rxGroupFrame[HW_NUM_OF_VAPS];
	uint32 txSenderError[HW_NUM_OF_VAPS];
#ifdef CPU_ARC
} BaaCounters_t;
#else
} BaaCounters_t_wave600b;
#endif

typedef struct 
{
	uint32 InitiatorTidStatisticsEventAddbaRequestWasSent[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventAddbaRequestConfiremd[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventAddbaResponseReceivedWithStatusSuccessAndValid[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventAddbaResponseReceivedWithStatusSuccessAndInvalid[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventAddbaResponseReceivedWithStatusFailure[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventAddbaResponseTimeout[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventBaAgreementOpened[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventBarWasSent[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventBarWasDiscarded[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventBarWasAnsweredWithAck[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventBarWasAnsweredWithBa[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventInactivityTimeout[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventDelbaWasSent[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventDelbaReceived[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventCloseTid[GEN6_NUM_OF_BANDS];
	uint32 InitiatorTidStatisticsEventOpenTid[GEN6_NUM_OF_BANDS];
#ifdef CPU_ARC
} TsManagerInitiatorTidGlobalStatistics_t;
#else
} TsManagerInitiatorTidGlobalStatistics_t_wave600b;
#endif

typedef struct 
{
	uint16 InitiatorStationStatisticsEventRemove[GEN6_NUM_OF_BANDS];
	uint16 InitiatorStationStatisticsEventAdd[GEN6_NUM_OF_BANDS];
	uint16 InitiatorStationStatisticsEventAddbaResponseReceivedWithIllegalTid[GEN6_NUM_OF_BANDS];
	uint16 Reserve[GEN6_NUM_OF_BANDS];
#ifdef CPU_ARC
} TsManagerInitiatorStationGlobalStatistics_t;
#else
} TsManagerInitiatorStationGlobalStatistics_t_wave600b;
#endif

typedef struct 
{
	uint32 RecipientTidStatisticsEventAddbaRequestAccepted[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventAddbaRequestWasReceivedWithInvalidParameters[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventAddbaResponseWasSentWithStatusSuccess[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventAddbaResponseWasSentWithStatusFailure[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventBaAgreementOpened[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventBaAgreementClosed[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventBarWasReceivedWithBaAgreementOpened[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventInactivityTimeout[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventDelbaReceived[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventDelbaWasSent[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventIllegalAggregationWasReceived[GEN6_NUM_OF_BANDS];
	uint32 RecipientTidStatisticsEventCloseTid[GEN6_NUM_OF_BANDS];
#ifdef CPU_ARC
} TsManagerRecipientTidGlobalStatistics_t;
#else
} TsManagerRecipientTidGlobalStatistics_t_wave600b;
#endif

typedef struct 
{
	uint16 RecipientStationStatisticsEventRemove[GEN6_NUM_OF_BANDS];
	uint16 RecipientStationStatisticsEventAdd[GEN6_NUM_OF_BANDS];
	uint16 RecipientStationStatisticsEventAddbaRequestReceivedWithIllegalTid[GEN6_NUM_OF_BANDS];
	uint16 Reserve[GEN6_NUM_OF_BANDS];
#ifdef CPU_ARC
} TsManagerRecipientStationGlobalStatistics_t;
#else
} TsManagerRecipientStationGlobalStatistics_t_wave600b;
#endif

typedef struct 
{
	uint32 ratesMask[HW_NUM_OF_STATIONS][RATES_BIT_MASK_SIZE];			
	uint32 bfReportReceivedCounter[GEN6_NUM_OF_BANDS];
	uint32 protectionSentCounter[GEN6_NUM_OF_BANDS];
	uint32 protectionSucceededCounter[GEN6_NUM_OF_BANDS];
	uint8  DataPhyMode[HW_NUM_OF_STATIONS][NUM_OF_BW];			
	uint8  ManagementPhyMode[HW_NUM_OF_STATIONS][NUM_OF_BW];		
	uint8  powerData[HW_NUM_OF_STATIONS][NUM_OF_BW];				
	uint8  powerManagement[HW_NUM_OF_STATIONS][NUM_OF_BW];		
	uint8  antennaSelectionData[HW_NUM_OF_STATIONS][NUM_OF_BW];
	uint8  antennaSelectionManagement[HW_NUM_OF_STATIONS][NUM_OF_BW];
	uint8  scpData[HW_NUM_OF_STATIONS][NUM_OF_BW];
	uint8  stationCapabilities[HW_NUM_OF_STATIONS];
	uint8  dataBwLimit[HW_NUM_OF_STATIONS];
	uint8  bfModeData[HW_NUM_OF_STATIONS];
	uint8  stbcModeData[HW_NUM_OF_STATIONS];
	uint8  raState[HW_NUM_OF_STATIONS];
	uint8  raStability[HW_NUM_OF_STATIONS];
	uint8  nssData[HW_NUM_OF_STATIONS][NUM_OF_BW];
	uint8  nssManagement[HW_NUM_OF_STATIONS][NUM_OF_BW];
	uint8  mcsData[HW_NUM_OF_STATIONS][NUM_OF_BW];
	uint8  mcsManagement[HW_NUM_OF_STATIONS][NUM_OF_BW];
	uint8  minPower[GEN6_NUM_OF_BANDS][NUM_OF_BW];
	uint8  maxPower[GEN6_NUM_OF_BANDS][NUM_OF_BW];
	uint8  bw[HW_NUM_OF_STATIONS];							/* Band width used in last reception												*/ 
	uint32 transmittedAmpdu[HW_NUM_OF_STATIONS];			/* Number of PPDUs transmitted for given rate										*/
	uint32 totalTxTime[HW_NUM_OF_STATIONS];
	uint32 rxDc[MAX(NUM_OF_BW, MAX_NUM_OF_ANTENNAS)];
	uint32 txLo[MAX(NUM_OF_BW, MAX_NUM_OF_ANTENNAS)];
	uint32 rxIq[MAX(NUM_OF_BW, MAX_NUM_OF_ANTENNAS)];
	uint32 txIq[MAX(NUM_OF_BW, MAX_NUM_OF_ANTENNAS)];
	uint32 mpduInAmpdu[HW_NUM_OF_STATIONS];
	uint32 ewmaTimeNsec[HW_NUM_OF_STATIONS];				/* Moving average value based on last couple of transmitted msdus [Nano SECONDS]	*/
	uint32 sumTimeNsec[HW_NUM_OF_STATIONS];					/* Delta of cumulative msdus times over interval [Nano SECONDS] 					*/
	uint32 numMsdus[HW_NUM_OF_STATIONS];					/* Number of msdus in given interval												*/
	uint32 per[HW_NUM_OF_STATIONS];							/* PER																				*/
#ifdef CPU_ARC
} LinkAdaptationStatistics_t;
#else
} LinkAdaptationStatistics_t_wave600b;
#endif


typedef struct 
{
	uint32 notEnoughClonePds;
	uint32 allClonesFinishedTransmission;
	uint32 noStationsInGroup;
#ifdef CPU_ARC
} MulticastStatistics_t;
#else
} MulticastStatistics_t_wave600b;
#endif

typedef struct 
{
	uint32 trainingTimerExpiredCounter;
	uint32 trainingStartedCounter;
	uint32 trainingFinishedSuccessfullyCounter;
	uint32 trainingFinishedUnsuccessfullyCounter;
	uint32 trainingNotStartedCounter;
#ifdef CPU_ARC
} TrainingManagerStatistics_t;
#else
} TrainingManagerStatistics_t_wave600b;
#endif

typedef struct 
{
	uint8 groupInfoDb[TX_MU_GROUPS][MU_NUM_OF_STATS];
#ifdef CPU_ARC
} GroupManagerStatistics_t;
#else
} GroupManagerStatistics_t_wave600b;
#endif

typedef struct 
{
	uint8 downlinkMuType;
	uint8 dlRuSubChannels;
	uint8 dlRuType;
	uint8 reserved;
#ifdef CPU_ARC
} dl_mu_stats_t;
#else
} dl_mu_stats_t_wave600b;
#endif

typedef struct 
{
	uint8 uplinkMuType;
	uint8 ulRuSubChannels;
	uint8 ulRuType;
	uint8 reserved;
#ifdef CPU_ARC
} ul_mu_stats_t;
#else
} ul_mu_stats_t_wave600b;
#endif

typedef struct 
{
#ifdef CPU_ARC
	ul_mu_stats_t uplinkMuStats[HW_NUM_OF_STATIONS];
	dl_mu_stats_t downlinkMuStats[HW_NUM_OF_STATIONS];
#else
	ul_mu_stats_t_wave600b uplinkMuStats[HW_NUM_OF_STATIONS];
	dl_mu_stats_t_wave600b downlinkMuStats[HW_NUM_OF_STATIONS];
#endif
#ifdef CPU_ARC
} PlanManagerStatistics_t;
#else
} PlanManagerStatistics_t_wave600b;
#endif

typedef struct 
{
	uint32 ratesMask[TX_MU_GROUPS][MAX_USP_IN_VHT_GROUP];
	uint32 protectionSentCounter[TX_MU_GROUPS];
	uint32 protectionSucceededCounter[TX_MU_GROUPS];
	uint32 DataPhyMode[TX_MU_GROUPS];
	uint32 powerData[TX_MU_GROUPS][NUM_OF_BW];
	uint32 scpData[TX_MU_GROUPS][NUM_OF_BW];
	uint32 dataBwLimit[TX_MU_GROUPS];
	uint32 groupGoodput[TX_MU_GROUPS];
	uint32 raState[TX_MU_GROUPS];
	uint32 raStability[TX_MU_GROUPS][MAX_USP_IN_VHT_GROUP];
	uint32 nssData[TX_MU_GROUPS][MAX_USP_IN_VHT_GROUP];
	uint32 mcsData[TX_MU_GROUPS][MAX_USP_IN_VHT_GROUP];
#ifdef CPU_ARC
} LinkAdaptationMuStatistics_t;
#else
} LinkAdaptationMuStatistics_t_wave600b;
#endif

typedef struct 
{
	uint32 illegalEvent[PTA_EVENT_NUM_OF_EVENTS][PTA_STATE_NUM_OF_STATES][PTA_RADIO_NUM_OF_TYPES];
	uint32 ignoreEvent[PTA_EVENT_NUM_OF_EVENTS][PTA_STATE_NUM_OF_STATES][PTA_RADIO_NUM_OF_TYPES];	
	uint32 zigbeeStarvationTimerExpired;
	uint32 btStarvationTimerExpired;
	uint32 zigbeeDenyOverTime;
	uint32 btDenyOverTime;
	uint32 gpio0TxDemandLow;
	uint32 zigbeeGrnatNotUsed;
	uint32 btGrnatNotUsed;
	uint32 zigbeeGrnatUsed;
	uint32 btGrnatUsed;
	uint32 zigbeeTxOngoing;
	uint32 btTxOngoing;
	uint32 gpio;
	uint32 priorityIsHigh;
	uint32 priorityIsLow;
#ifdef CPU_ARC
} PtaStatistics_t;
#else
} PtaStatistics_t_wave600b;
#endif

typedef struct 
{
	uint8 implicit;
	uint8 announced;
	uint8 triggerEnabled;
	uint8 reserved;
#ifdef CPU_ARC
} twt_operation_t;
#else
} twt_operation_t_wave600b;
#endif

typedef struct 
{
	uint32 wakeTimeHigh;
	uint32 wakeTimeLow;
	uint32 wakeInterval;
	uint16 minWakeDuration;
	uint16 channel;
#ifdef CPU_ARC
} twt_individual_params_t;
#else
} twt_individual_params_t_wave600b;
#endif

typedef struct 
{
	uint32 tragetBeacon;
	uint32 listenInterval;
#ifdef CPU_ARC
} twt_broadcast_params_t;
#else
} twt_broadcast_params_t_wave600b;
#endif

typedef struct 
{
	uint16 state;
	uint16 agreementType;
#ifdef CPU_ARC
	twt_operation_t operation;
	union 
	{
		twt_individual_params_t individual;
		twt_broadcast_params_t broadcast;
	} params;
#else
	twt_operation_t_wave600b operation;
	union 
	{
		twt_individual_params_t_wave600b individual;
		twt_broadcast_params_t_wave600b broadcast;
	} params;
#endif
#ifdef CPU_ARC
} twt_agreement_t;
#else
} twt_agreement_t_wave600b;
#endif

typedef struct 
{
	uint32 numOfAgreementsForSta;
#ifdef CPU_ARC
	twt_agreement_t twtIndividualAgreement[TWT_MAX_INDIVIDUAL_AGREEMENTS_ALLOWED_PER_STA];
#else
	twt_agreement_t_wave600b twtIndividualAgreement[TWT_MAX_INDIVIDUAL_AGREEMENTS_ALLOWED_PER_STA];
#endif
#ifdef CPU_ARC
} twt_params_t;
#else
} twt_params_t_wave600b;
#endif

typedef struct 
{
#ifdef CPU_ARC
	uint32 numOfStaInSp[MAX_NUM_OF_SP_BAND_0 + MAX_NUM_OF_SP_BAND_1];
	twt_params_t twtStaParams[HW_NUM_OF_STATIONS];
#else
	uint32 numOfStaInSp[MAX_NUM_OF_SP_BAND_0_B0 + MAX_NUM_OF_SP_BAND_1_B0];
    twt_params_t_wave600b twtStaParams[HW_NUM_OF_STATIONS];
#endif
#ifdef CPU_ARC
} TwtStatistics_t;
#else
} TwtStatistics_t_wave600b;
#endif

typedef struct 
{
	uint8  rssi[4];      //Last 4 RSSI frames received
	uint8  time_s[4];    //Time of when last 4 RSSI were received
	uint16 count;        //Sequence numer of received managemant (bcn, ack) frames 
#ifdef CPU_ARC
} rssiSnapshot_t;
#else
} rssiSnapshot_t_wave600b;
#endif

typedef struct 
{
	int8 	maxRssi[HW_NUM_OF_STATIONS];
	int8	minRssi[HW_NUM_OF_STATIONS];	
#ifdef CPU_ARC
} AlphaFilterStatistics_t;
#else
} AlphaFilterStatistics_t_wave600b;
#endif


typedef struct 
{
	uint32 calibrationMask[GEN6_NUM_OF_BANDS];
	uint32 onlineCalibrationMask[GEN6_NUM_OF_BANDS];
	uint32 dropReasonClassifier[GEN6_NUM_OF_BANDS];
	uint32 dropReasonDisconnect[GEN6_NUM_OF_BANDS];
	uint32 dropReasonATF[GEN6_NUM_OF_BANDS];
	uint32 dropReasonTSFlush[GEN6_NUM_OF_BANDS];
	uint32 dropReasonReKey[GEN6_NUM_OF_BANDS];
	uint32 dropReasonSetKey[GEN6_NUM_OF_BANDS];
	uint32 dropReasonDiscard[GEN6_NUM_OF_BANDS];
	uint32 dropReasonDsabled[GEN6_NUM_OF_BANDS];
	uint32 dropReasonAggError[GEN6_NUM_OF_BANDS];
	uint32 defragStart[GEN6_NUM_OF_BANDS];
	uint32 defragEnd[GEN6_NUM_OF_BANDS];
	uint32 defragTimeout[GEN6_NUM_OF_BANDS];
	uint32 classViolationErrors[GEN6_NUM_OF_BANDS];
	uint32 rxhErrors[GEN6_NUM_OF_BANDS];
	uint32 rxSmps[GEN6_NUM_OF_BANDS];
	uint32 rxOmn[GEN6_NUM_OF_BANDS];
	uint32 txMngFromHost[GEN6_NUM_OF_BANDS];
	uint32 txDataFromHost[GEN6_NUM_OF_BANDS];
	uint32 cyclicBufferOverflow[GEN6_NUM_OF_BANDS];
	uint32 beaconsTransmitted[GEN6_NUM_OF_BANDS];
	uint32 debugStatistics[GEN6_NUM_OF_BANDS];
#ifdef CPU_ARC
} GeneralStatistics_t;
#else
} GeneralStatistics_t_wave600b;
#endif

typedef struct 
{
	/* TX */
	uint32 retryCount[HW_NUM_OF_STATIONS];		
	uint32 successCount[HW_NUM_OF_STATIONS];
	uint32 exhaustedCount[HW_NUM_OF_STATIONS];
	uint32 clonedCount[HW_NUM_OF_STATIONS];
	uint32 oneOrMoreRetryCount[HW_NUM_OF_STATIONS];
	uint32 packetRetransCount[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonClassifier[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonDisconnect[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonATF[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonTSFlush[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonReKey[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonSetKey[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonDiscard[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonDsabled[HW_NUM_OF_STATIONS];
	uint32 dropCntReasonAggError[HW_NUM_OF_STATIONS];
	/* RX */
	uint32 mpduRetryCount[HW_NUM_OF_STATIONS];	/* Number of rx retries			*/
	uint32 mpduInAmpdu[HW_NUM_OF_STATIONS];		/* Number of MPDUs received		*/
	uint32 ampdu[HW_NUM_OF_STATIONS];	 		/* Number of PPDUs received 	*/
	
#ifdef CPU_ARC
} PerClientStatistics_t;
#else
} PerClientStatistics_t_wave600b;
#endif

typedef struct 
{
	uint64	chUtilizationTotal;		/* Total time radio spent receiveing or transmitting on that channel (ch_utilization_active) 		*/	
	uint64	chUtilizationBusy; 		/* Time radio detected that channel was busy (Busy = Rx + Tx + Interference) 						*/		
	uint64	chUtilizationBusyTx; 	/* Time radio spent transmitting on channel 														*/ 	
	uint64	chUtilizationBusyRx; 	/* Time radio spent receiving on channel (Rx = Rx_obss + Rx_self + Rx_errr (self and obss errors)	*/ 	
	uint64	chUtilizationBusySelf;	/* Time radio spend receiving on channel from its own connected clients								*/
	uint64	chUtilizationBusyExt;	/* Time radio detected that extended channel was busy (40MHz extention channel busy)				*/
	uint64	chUtilization;			/* 9.4.2.28 BSS Load element. Channel Utilization paramter											*/
#ifdef CPU_ARC
} chUtilization_t;
#else
} chUtilization_t_wave600b;
#endif

typedef struct 
{
#ifdef CPU_ARC
	chUtilization_t 			channelUtilizationStats[GEN6_NUM_OF_BANDS];
#else
	chUtilization_t_wave600b 	channelUtilizationStats[GEN6_NUM_OF_BANDS];
#endif
	int8						channelMax80211Rssi[GEN6_NUM_OF_BANDS];
	uint8						channelNum[GEN6_NUM_OF_BANDS];
	uint8						reserved[4];

#ifdef CPU_ARC
} channelStatistics_t;
#else
} channelStatistics_t_wave600b;
#endif

typedef struct 
{
	//word 0
	uint8	noise[PHY_STATISTICS_MAX_RX_ANT];
	//word 1
	uint8	rf_gain[PHY_STATISTICS_MAX_RX_ANT];
	//word 2
	uint32	irad;
	//word 3
	uint32 	tsf;
	
	//word 4 (for SW use)
	uint8	channel_load;	
	uint8 	totalChannelUtilization;	/* Total Channel Utilization is an indication of how congested the medium is (all APs) */
	int8  	chNon80211Noise;
	int8	CWIvalue;
	
	//word 5 (for SW use)
	uint32	extStaRx;	

	//word 6 (for SW use)
	int16	txPower;
	uint8	channelNum;
	uint8	activeAntMask;

    //word 7-8 (for SW use)
    uint16  backgroundNoise[PHY_STATISTICS_MAX_RX_ANT]; /* Long term average Background Noise per antenna */
	
#ifdef CPU_ARC
} devicePhyRxStatusDb_t;
#else
} devicePhyRxStatusDb_t_wave600b;
#endif

typedef struct 
{
	//word 0
	int8 	rssi[PHY_STATISTICS_MAX_RX_ANT];
	//word 1
	uint32 	phyRate;	//17 bits [20:4]
	//word 2
	uint32 	irad;
	//word 3
	uint32 	lastTsf;
	//word 4 		for SW use
	uint32	perClientRxtimeUsage;
	//word 5		for SW use
	int8	noise[PHY_STATISTICS_MAX_RX_ANT];
	//word 6		for SW use
	int8  	gain[PHY_STATISTICS_MAX_RX_ANT];
	//word 7-9		for SW use
#ifdef CPU_ARC
	rssiSnapshot_t			rssiAckAvarage; //AP_MODE: RSSI from last 4 ack received
#else
	rssiSnapshot_t_wave600b	rssiAckAvarage; //AP_MODE: RSSI from last 4 ack received
#endif	
	uint16	psduRate;						/* PSDU RATE is copied from phy metrics. It is constructed from MCS & NSS [bits 0..7], PHY MODE [bits 8..10], BW [bits 11..12], SCP [bit 13], HE CP [bits 14..15] */

	//word 10-13
	uint8 	rssiArray[PHY_STATISTICS_MAX_RX_ANT][NUM_OF_EXTENTION_RSSI];   // 4=antennas, 4=20+20+40+80 extension rssi
	
	/* Per antenna RSSI (above noise floor) for all widths (primary,secondary) 
	  -----------------------------------------------
	  | chain_nun	|  20MHz [pri20]
	  |           	|  40MHZ [pri20,sec20] 
	  |          	 	|  80MHz [pri20,sec20,sec40]
	  |           	| 160MHz [pri20,sec20,sec40,sec80]
	  -----------------------------------------------
	  |  1        	|  rssi  [pri20,sec20,sec40,sec80 ]
	  |  ...      	|  ...
	  |  8        	|  rssi  [pri20,sec20,sec40,sec80 ]
	  ----------------------------------------------- */

	uint32 phyRateSynchedToPsduRate;		/* Phy rate extraced by RX HANDLER from PHY METRICS (the above phy rate is extracted by PAC Extrapolator) */

	//Last possible word num is 14 (total of 15 words that can be configured to pac Extrap for 4 bits parameter
#ifdef CPU_ARC
} stationPhyRxStatusDb_t;
#else
} stationPhyRxStatusDb_t_wave600b;
#endif

typedef struct 
{
#ifdef CPU_ARC
	devicePhyRxStatusDb_t 			devicePhyRxStatus[GEN6_NUM_OF_BANDS];
	stationPhyRxStatusDb_t 			staPhyRxStatus[HW_NUM_OF_STATIONS];
#else
	devicePhyRxStatusDb_t_wave600b 	devicePhyRxStatus[GEN6_NUM_OF_BANDS];
	stationPhyRxStatusDb_t_wave600b	staPhyRxStatus[HW_NUM_OF_STATIONS];
#endif
#ifdef CPU_ARC
} wholePhyRxStatusDb_t;
#else
} wholePhyRxStatusDb_t_wave600b;
#endif

typedef struct 
{
	uint32	dynamicBW20MHz[GEN6_NUM_OF_BANDS];
	uint32	dynamicBW40MHz[GEN6_NUM_OF_BANDS];
	uint32	dynamicBW80MHz[GEN6_NUM_OF_BANDS];
	uint32	dynamicBW160MHz[GEN6_NUM_OF_BANDS];
#ifdef CPU_ARC
} DynamicBwStatistics_t;
#else
} DynamicBwStatistics_t_wave600b;
#endif

/*
order should be aligned to StatisticType_e
*/
#ifdef CPU_ARC
typedef struct _StatisticsDb_t
{
    HostIfCounters_t										hostIfCounters;
    RxCounters_t											rxCounters;
    BaaCounters_t											baaCounters;
    UplinkBsrcTidCnt_t										uplinkBsrcTidCnt;
    TsManagerInitiatorTidGlobalStatistics_t					tsManagerInitiatorTidGlobalStats;
    TsManagerInitiatorStationGlobalStatistics_t				tsManagerInitiatorStationGlobalStats;
    TsManagerRecipientTidGlobalStatistics_t					tsManagerRecipientTidGlobalStats;
    TsManagerRecipientStationGlobalStatistics_t				tsManagerRecipientStationGlobalStats;
    LinkAdaptationStatistics_t								linkAdaptationStats;
    MulticastStatistics_t									multicastStats;
    TrainingManagerStatistics_t								trainingManagerStats;
    GroupManagerStatistics_t								groupManagerStats;
    LinkAdaptationMuStatistics_t							linkAdaptationMuStats;
    PtaStatistics_t											ptaStats;
    TwtStatistics_t											twtStats;
    PlanManagerStatistics_t									planManagerStats;
    AlphaFilterStatistics_t									alphaFilterStats;
    PerClientStatistics_t									perClientStats;
    GeneralStatistics_t										generalStats;
    DynamicBwStatistics_t									dynamicBwStats;
    wholePhyRxStatusDb_t									phyStatistics;
    channelStatistics_t										currentChannelStats;
} StatisticsDb_t;

#else
typedef struct _StatisticsDb_t_wave600b
{
	HostIfCounters_t_wave600b 								hostIfCounters;
	RxCounters_t_wave600b									rxCounters;
	BaaCounters_t_wave600b									baaCounters;
	UplinkBsrcTidCnt_t_wave600b								uplinkBsrcTidCnt;
	TsManagerInitiatorTidGlobalStatistics_t_wave600b		tsManagerInitiatorTidGlobalStats;
	TsManagerInitiatorStationGlobalStatistics_t_wave600b	tsManagerInitiatorStationGlobalStats;
	TsManagerRecipientTidGlobalStatistics_t_wave600b		tsManagerRecipientTidGlobalStats;
	TsManagerRecipientStationGlobalStatistics_t_wave600b	tsManagerRecipientStationGlobalStats;
	LinkAdaptationStatistics_t_wave600b						linkAdaptationStats;
	MulticastStatistics_t_wave600b							multicastStats;
	TrainingManagerStatistics_t_wave600b					trainingManagerStats;
	GroupManagerStatistics_t_wave600b						groupManagerStats;
	LinkAdaptationMuStatistics_t_wave600b					linkAdaptationMuStats;
	PtaStatistics_t_wave600b								ptaStats;
	TwtStatistics_t_wave600b								twtStats;
	PlanManagerStatistics_t_wave600b						planManagerStats;
	AlphaFilterStatistics_t_wave600b						alphaFilterStats;
	PerClientStatistics_t_wave600b							perClientStats;
	GeneralStatistics_t_wave600b							generalStats;
	DynamicBwStatistics_t_wave600b							dynamicBwStats;
	wholePhyRxStatusDb_t_wave600b							phyStatistics;
	channelStatistics_t_wave600b 							currentChannelStats;
} StatisticsDb_t_wave600b;
#endif
#endif //_STATISTICS_DESC_WAVE600B_H_
