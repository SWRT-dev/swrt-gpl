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
#ifndef _STATISTICS_DESC_WAVE500B_H_
#define _STATISTICS_DESC_WAVE500B_H_


#ifdef CPU_MIPS

#include "HwGlobalDefinitions.h"
#include "linkAdaptation_api.h"
#include "mhi_umi.h"
#include "GroupManager_API.h"
#include "PacketTrafficArbitrator_api.h"
#include "ieee80211.h"
#include "SharedDbTypes.h"
#include "Utils_Api.h"

#else
#define HW_NUM_OF_VAPS          			(16)
#define HW_NUM_OF_STATIONS      			(128)
#define NUM_OF_TIDS              			(8)
#define RATES_BIT_MASK_SIZE     			(2)
#define NUM_OF_BW               			(3)
#define PHY_STATISTICS_MAX_RX_ANT      	 	(4)
#define TX_MU_GROUPS           				(8)
#define MU_NUM_OF_STATS       				(7)
#define PTA_STATE_NUM_OF_STATES 			(13)
#define PTA_EVENT_NUM_OF_EVENTS				(13)
#define PTA_RADIO_NUM_OF_TYPES  			(2)
#define ACCESS_CATEGORY_NUM					(4)
#define MAX_USP_IN_VHT_GROUP 				(4)
#define NUM_OF_MCS							(0xB)

#endif //CPU_MIPS



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
#ifdef CPU_MIPS
} HostIfCountersPerVap_t;
#else
} HostIfCountersPerVap_t_wave500b;
#endif

typedef struct
{	
	uint32 rxOutStaNumOfBytes; 		
#ifdef CPU_MIPS
} HostIfCountersPerSta_t;
#else
} HostIfCountersPerSta_t_wave500b;
#endif

typedef struct
{
	uint16 qosTxSta[HW_NUM_OF_STATIONS][NUM_OF_TIDS];				
	uint16 qosTxVap[HW_NUM_OF_VAPS][ACCESS_CATEGORY_NUM];
#ifdef CPU_MIPS
	HostIfCountersPerVap_t 			vapCounts[HW_NUM_OF_VAPS];
	HostIfCountersPerSta_t 			staCounts[HW_NUM_OF_STATIONS];
#else
	HostIfCountersPerVap_t_wave500b vapCounts[HW_NUM_OF_VAPS];
	HostIfCountersPerSta_t_wave500b staCounts[HW_NUM_OF_STATIONS];
#endif
#ifdef CPU_MIPS
} HostIfCounters_t;
#else
} HostIfCounters_t_wave500b;
#endif


typedef struct
{
	uint32 mpduUnicastOrMngmnt;
	uint32 mpduRetryCount;
	uint32 amsdu;	
	uint32 dropMpdu;
	uint32 mpduTypeNotSupported;
	uint32 replayData;
	uint32 replayMngmnt;
	uint32 tkipCount;
	uint32 securityFailure;	
	uint32 amsduBytes;
	uint32 ampdu;
	uint32 mpduInAmpdu;
	uint32 octetsInAmpdu;
	uint32 rxClassifierSecurityMismatch;
	uint32 bcMcCountVap;
#ifdef CPU_MIPS
} RxCountersRxppPerVap_t;
#else
} RxCountersRxppPerVap_t_wave500b;
#endif

typedef struct
{
	uint32 rdCount;
	uint32 rddelayed;				
	uint32 swUpdateDrop;			
	uint32 rdDuplicateDrop;			
	uint32 missingSn;				
#ifdef CPU_MIPS
} RxCountersPerSta_t;
#else
} RxCountersPerSta_t_wave500b;
#endif


typedef struct
{
	uint16 qosRxSta[HW_NUM_OF_STATIONS];				
#ifdef CPU_MIPS
	RxCountersPerSta_t 				staCounts[HW_NUM_OF_STATIONS];
	RxCountersRxppPerVap_t 			rxppVapCounts[HW_NUM_OF_VAPS];
#else
	RxCountersPerSta_t_wave500b 	staCounts[HW_NUM_OF_STATIONS];
	RxCountersRxppPerVap_t_wave500b rxppVapCounts[HW_NUM_OF_VAPS];
#endif

	uint32 barMpduCount;
	uint32 crcErrorCount;
	uint32 delCrcError;	

#ifdef CPU_MIPS
} RxCounters_t;
#else
} RxCounters_t_wave500b;
#endif


typedef struct
{
	uint32 rtsSuccessCount;
	uint32 qosTransmittedFrames;
	uint32 transmittedAmsdu;
	uint32 transmittedOctetsInAmsdu;
	uint32 transmittedAmpdu;
	uint32 transmittedMpduInAmpdu;
	uint32 transmittedOctetsInAmpdu;
	uint32 beamformingFrames;
	uint32 transmitStreamRprtMSDUFailed;	
	uint32 rtsFailure;
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
#ifdef CPU_MIPS
} BaaCountersPerVap_t;
#else
} BaaCountersPerVap_t_wave500b;
#endif

typedef struct
{
	uint32 mpduFirstRetransmission;		
	uint32 mpduTransmitted;
	uint32 mpduByteTransmitted;			
#ifdef CPU_MIPS
} BaaCountersPerSta_t;
#else
} BaaCountersPerSta_t_wave500b;
#endif

typedef struct
{
	uint32 retryCount;
	uint32 multipleRetryCount;
#ifdef CPU_MIPS
	BaaCountersPerVap_t 			vapCounts[HW_NUM_OF_VAPS];
	BaaCountersPerSta_t 			staCounts[HW_NUM_OF_STATIONS];
#else
	BaaCountersPerVap_t_wave500b	vapCounts[HW_NUM_OF_VAPS];
	BaaCountersPerSta_t_wave500b	staCounts[HW_NUM_OF_STATIONS];
#endif
	uint32 rxGroupFrame[HW_NUM_OF_VAPS];
	uint32 txSenderError[HW_NUM_OF_VAPS];
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
#ifdef CPU_MIPS
} BaaCounters_t;
#else
} BaaCounters_t_wave500b;
#endif

typedef struct
{
	uint32 InitiatorTidStatisticsEventAddbaRequestWasSent;
	uint32 InitiatorTidStatisticsEventAddbaRequestConfiremd;
	uint32 InitiatorTidStatisticsEventAddbaResponseReceivedWithStatusSuccessAndValid;
	uint32 InitiatorTidStatisticsEventAddbaResponseReceivedWithStatusSuccessAndInvalid;
	uint32 InitiatorTidStatisticsEventAddbaResponseReceivedWithStatusFailure;
	uint32 InitiatorTidStatisticsEventAddbaResponseTimeout;
	uint32 InitiatorTidStatisticsEventBaAgreementOpened;
	uint32 InitiatorTidStatisticsEventBarWasSent;
	uint32 InitiatorTidStatisticsEventBarWasDiscarded;
	uint32 InitiatorTidStatisticsEventBarWasAnsweredWithAck;
	uint32 InitiatorTidStatisticsEventBarWasAnsweredWithBa;
	uint32 InitiatorTidStatisticsEventInactivityTimeout;
	uint32 InitiatorTidStatisticsEventDelbaWasSent;
	uint32 InitiatorTidStatisticsEventDelbaReceived;
	uint32 InitiatorTidStatisticsEventCloseTid;
	uint32 InitiatorTidStatisticsEventOpenTid;
#ifdef CPU_MIPS
} TsManagerInitiatorTidGlobalStatistics_t;
#else
} TsManagerInitiatorTidGlobalStatistics_t_wave500b;
#endif

typedef struct
{
	uint16 InitiatorStationStatisticsEventRemove;
	uint16 InitiatorStationStatisticsEventAdd;
	uint16 InitiatorStationStatisticsEventAddbaResponseReceivedWithIllegalTid;
	uint16 Reserve;
#ifdef CPU_MIPS
} TsManagerInitiatorStationGlobalStatistics_t;
#else
} TsManagerInitiatorStationGlobalStatistics_t_wave500b;
#endif

typedef struct
{
	uint32 RecipientTidStatisticsEventAddbaRequestAccepted;
	uint32 RecipientTidStatisticsEventAddbaRequestWasReceivedWithInvalidParameters;
	uint32 RecipientTidStatisticsEventAddbaResponseWasSentWithStatusSuccess;
	uint32 RecipientTidStatisticsEventAddbaResponseWasSentWithStatusFailure;
	uint32 RecipientTidStatisticsEventBaAgreementOpened;
	uint32 RecipientTidStatisticsEventBaAgreementClosed;
	uint32 RecipientTidStatisticsEventBarWasReceivedWithBaAgreementOpened;
	uint32 RecipientTidStatisticsEventInactivityTimeout;
	uint32 RecipientTidStatisticsEventDelbaReceived;
	uint32 RecipientTidStatisticsEventDelbaWasSent;
	uint32 RecipientTidStatisticsEventIllegalAggregationWasReceived;
	uint32 RecipientTidStatisticsEventCloseTid;
#ifdef CPU_MIPS
} TsManagerRecipientTidGlobalStatistics_t;
#else
} TsManagerRecipientTidGlobalStatistics_t_wave500b;
#endif

typedef struct
{
	uint16 RecipientStationStatisticsEventRemove;
	uint16 RecipientStationStatisticsEventAdd;
	uint16 RecipientStationStatisticsEventAddbaRequestReceivedWithIllegalTid;
	uint16 Reserve;
#ifdef CPU_MIPS
} TsManagerRecipientStationGlobalStatistics_t;
#else
} TsManagerRecipientStationGlobalStatistics_t_wave500b;
#endif

typedef struct
{
	uint32 ratesMask[HW_NUM_OF_STATIONS][RATES_BIT_MASK_SIZE];			
	uint32 bfReportReceivedCounter;
	uint32 protectionSentCounter;
	uint32 protectionSucceededCounter;
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
	uint8  minPower[NUM_OF_BW];
	uint8  maxPower[NUM_OF_BW];
	uint16 reserved;
	uint32 totalTxTime[HW_NUM_OF_STATIONS];
	uint32 rxDc[PHY_STATISTICS_MAX_RX_ANT];
	uint32 txLo[PHY_STATISTICS_MAX_RX_ANT];
	uint32 rxIq[PHY_STATISTICS_MAX_RX_ANT];
	uint32 txIq[PHY_STATISTICS_MAX_RX_ANT];
	uint32 bf[MAX_NUMBER_OF_BF_CALS];
#ifdef CPU_MIPS
} LinkAdaptationStatistics_t;
#else
} LinkAdaptationStatistics_t_wave500b;
#endif


typedef struct
{
	uint32 notEnoughClonePds;
	uint32 allClonesFinishedTransmission;
	uint32 noStationsInGroup;
#ifdef CPU_MIPS
} MulticastStatistics_t;
#else
} MulticastStatistics_t_wave500b;
#endif

typedef struct
{
	uint32 trainingTimerExpiredCounter;
	uint32 trainingStartedCounter;
	uint32 trainingFinishedSuccessfullyCounter;
	uint32 trainingFinishedUnsuccessfullyCounter;
	uint32 trainingNotStartedCounter;
#ifdef CPU_MIPS
} TrainingManagerStatistics_t;
#else
} TrainingManagerStatistics_t_wave500b;
#endif

typedef struct
{
	uint8 groupInfoDb[TX_MU_GROUPS][MU_NUM_OF_STATS];
#ifdef CPU_MIPS
} GroupManagerStatistics_t;
#else
} GroupManagerStatistics_t_wave500b;
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
#ifdef CPU_MIPS
} LinkAdaptationMuStatistics_t;
#else
} LinkAdaptationMuStatistics_t_wave500b;
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
#ifdef CPU_MIPS
} PtaStatistics_t;
#else
} PtaStatistics_t_wave500b;
#endif



typedef struct
{
	uint8  rssi[4];      //Last 4 RSSI frames received
	uint8  time_s[4];    //Time of when last 4 RSSI were received
	uint16 count;        //Sequence numer of received managemant (bcn, ack) frames 
#ifdef CPU_MIPS
} rssiSnapshot_t;
#else
} rssiSnapshot_t_wave500b;
#endif

typedef struct
{
	uint8 	maxRssi[HW_NUM_OF_STATIONS];
#ifdef CPU_MIPS
} AlphaFilterStatistics_t;
#else
} AlphaFilterStatistics_t_wave500b;
#endif

typedef struct
{
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
#ifdef CPU_MIPS
} GeneralStatistics_t;
#else
} GeneralStatistics_t_wave500b;
#endif

typedef struct
{
	uint32 			successCount[HW_NUM_OF_STATIONS];
	uint32 			exhaustedCount[HW_NUM_OF_STATIONS];
	uint32 			clonedCount[HW_NUM_OF_STATIONS];
#ifdef CPU_MIPS
} PerClientTransmitStatistics_t;
#else
} PerClientTransmitStatistics_t_wave500b;
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
	uint8	reserved;
	
#ifdef CPU_MIPS
} devicePhyRxStatusDb_t;
#else
} devicePhyRxStatusDb_t_wave500b;
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
	//word 4 	for SW use
	uint32	perClientRxtimeUsage;
	//word 5		for SW use
	uint8	noise[PHY_STATISTICS_MAX_RX_ANT];
	//word 6		for SW use
	uint8  	gain[PHY_STATISTICS_MAX_RX_ANT];
	//word 7		for SW use
	int8	maxRssi;	
	uint8	reserved[3];
#ifdef CPU_MIPS
} stationPhyRxStatusDb_t;
#else
} stationPhyRxStatusDb_t_wave500b;
#endif

typedef struct
{
#ifdef CPU_MIPS
	devicePhyRxStatusDb_t 			devicePhyRxStatus;
	stationPhyRxStatusDb_t 			staPhyRxStatus[HW_NUM_OF_STATIONS];
#else
	devicePhyRxStatusDb_t_wave500b 	devicePhyRxStatus;
	stationPhyRxStatusDb_t_wave500b	staPhyRxStatus[HW_NUM_OF_STATIONS];
#endif
#ifdef CPU_MIPS
} wholePhyRxStatusDb_t;
#else
} wholePhyRxStatusDb_t_wave500b;
#endif

typedef struct
{
	uint32 	dynamicBW20MHz;
	uint32 	dynamicBW40MHz;
	uint32 	dynamicBW80MHz;
	uint32	reserved;
#ifdef CPU_MIPS
} DynamicBwStatistics_t;
#else
} DynamicBwStatistics_t_wave500b;
#endif

/*
order should be aligned to StatisticType_e
*/
#ifdef CPU_MIPS
typedef struct _StatisticsDb_t
{
    HostIfCounters_t								hostIfCounters;
    RxCounters_t									rxCounters;
    BaaCounters_t									baaCounters;
    TsManagerInitiatorTidGlobalStatistics_t			tsManagerInitiatorTidGlobalStats;
    TsManagerInitiatorStationGlobalStatistics_t		tsManagerInitiatorStationGlobalStats;
    TsManagerRecipientTidGlobalStatistics_t			tsManagerRecipientTidGlobalStats;
    TsManagerRecipientStationGlobalStatistics_t		tsManagerRecipientStationGlobalStats;
    LinkAdaptationStatistics_t						linkAdaptationStats;
    MulticastStatistics_t							multicastStats;
    TrainingManagerStatistics_t						trainingManagerStats;
    GroupManagerStatistics_t						groupManagerStats;
    LinkAdaptationMuStatistics_t					linkAdaptationMuStats;
    PtaStatistics_t									ptaStats;
    PerClientTransmitStatistics_t					perClientTransmitStats;
    GeneralStatistics_t								generalStats;
    DynamicBwStatistics_t							dynamicBwStats;
    wholePhyRxStatusDb_t							phyStatistics;
} StatisticsDb_t;

#else
typedef struct _StatisticsDb_t_wave500b
{
	HostIfCounters_t_wave500b 								hostIfCounters;
	RxCounters_t_wave500b									rxCounters;
	BaaCounters_t_wave500b									baaCounters;
	TsManagerInitiatorTidGlobalStatistics_t_wave500b		tsManagerInitiatorTidGlobalStats;
	TsManagerInitiatorStationGlobalStatistics_t_wave500b	tsManagerInitiatorStationGlobalStats;
	TsManagerRecipientTidGlobalStatistics_t_wave500b		tsManagerRecipientTidGlobalStats;
	TsManagerRecipientStationGlobalStatistics_t_wave500b	tsManagerRecipientStationGlobalStats;
	LinkAdaptationStatistics_t_wave500b						linkAdaptationStats;
	MulticastStatistics_t_wave500b							multicastStats;
	TrainingManagerStatistics_t_wave500b					trainingManagerStats;
	GroupManagerStatistics_t_wave500b						groupManagerStats;
	LinkAdaptationMuStatistics_t_wave500b					linkAdaptationMuStats;
	PtaStatistics_t_wave500b								ptaStats;
	PerClientTransmitStatistics_t_wave500b					perClientTransmitStats;
	GeneralStatistics_t_wave500b							generalStats;
	DynamicBwStatistics_t_wave500b							dynamicBwStats;
	wholePhyRxStatusDb_t_wave500b							phyStatistics;
} StatisticsDb_t_wave500b;
#endif

#endif //_STATISTICS_DESC_WAVE500B_H_
