/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
#ifndef __MHI_UMI_PROPR_INCLUDED_H
#define __MHI_UMI_PROPR_INCLUDED_H

#include "mhi_umi.h"

#define   MTLK_PACK_ON
#include "mtlkpack.h"



#define CALIBR_BITMAP_SIZE				3	/*The MAX number of channels stored in Host buffer are 70*/



/***************************************************************************
**
** NAME         UM_MAN_MBSS_PRE_ACTIVATE_REQ
**
** PARAMETERS   
**
** DESCRIPTION  Pre-Activate Request. This request should be sent to the Upper
**              MAC to set internal operational parameters.
**
*****************************************************************************/
typedef struct _UMI_MBSS_PRE_ACTIVATE
{
  UMI_MBSS_PRE_ACTIVATE_HDR sHdr;
  uint32 					storedCalibrationChannelBitMap[CALIBR_BITMAP_SIZE];
} __MTLK_PACKED UMI_MBSS_PRE_ACTIVATE;

//Supported Switch Types
#define ST_NORMAL					(0x0)
#define ST_SCAN						(0x1)
#define ST_CSA						(0x2)
#define ST_RSSI						(0x3)
#define ST_NORMAL_AFTER_RSSI        (0x4)
#define ST_CALIBRATE				(0x5)
#define ST_DPP_ACTION				(0x6)
#define ST_LAST						(0x7)


//Supported Channel Widths
typedef enum _Bandwidth_e
{
	BANDWIDTH_TWENTY				= 0x0,
	BANDWIDTH_FOURTY				= 0x1,
	BANDWIDTH_EIGHTY				= 0x2,
	BANDWIDTH_ONE_HUNDRED_SIXTY		= 0x3,
	BANDWIDTH_EIGHTY_EIGHTY			= 0x4,
	BANDWIDTH_ALL					= 0x5,
	BANDWIDTH_MAX					= 0x7,
} Bandwidth_e;

typedef enum _ChannelNotificationMode_e
{
	CHANNEL_NOTIFICATION_NORMAL	  = 0x0,
	CHANNEL_NOTIFICATION_TWENTY	  = 0x1,
	CHANNEL_NOTIFICATION_FOURTY	  = 0x2,
	CHANNEL_NOTIFICATION_EIGHTY	  = 0x3,
} ChannelNotificationMode_e;

#define REGD_CODE_UNKNOWN    0x00
#define REGD_CODE_FCC        0x10
#define REGD_CODE_DOC        0x20

#define REGD_CODE_ETSI       0x30
#define REGD_CODE_SPAIN      0x31
#define REGD_CODE_FRANCE     0x32
#define REGD_CODE_UAE        0x33
#define REGD_CODE_GERMANY    0x34

#define REGD_CODE_MKK        0x40
#define REGD_CODE_ISRAEL     0x41
#define REGD_CODE_SINGAPORE  0x42
#define REGD_CODE_AUSTRALIA  0x44
#define REGD_CODE_BRAZIL     0x45
#define REGD_CODE_CHINA      0x46
#define REGD_CODE_KOREA      0x47

#define REGD_CODE_APAC       0x50
#define REGD_CODE_JAPAN      0x51




//Set Channel Possible Results
#define SET_CHANNEL_RESULT_PASS			(0x0)
#define SET_CHANNEL_RESULT_FAIL			(0x1)

/***************************************************************************
**
** NAME         UM_SET_CHAN_PARAMS
**
** DESCRIPTION: structure used to pass parameters for all channel switch 
**				operations mandated by driver
**
****************************************************************************/
#ifdef CPU_ARC

#define CHAN_PARAMS_CHAN_NUM_MIN				(1)	  //first channl is 1 in 2.4 and 6 giga.  
#define CHAN_PARAMS_CHAN_NUM_MAX				(233) //max channel in 6 giga

#define CHAN_PARAMS_CHAN_INDEX_MIN				(0)	  
#define CHAN_PARAMS_CHAN_INDEX_MAX				(7) 

#define CHAN_PARAMS_CHAN_BANDWIDTH_MIN			(0)
#define CHAN_PARAMS_CHAN_BANDWIDTH_MAX			(4)

#define CHAN_PARAMS_CHAN_SWITCH_TIME_MIN		(0)
#define CHAN_PARAMS_CHAN_SWITCH_TIME_MAX		(3000)	/* Customer typically uses 2500 */

#define CHAN_PARAMS_CHAN_SWITCH_TYPE_MIN		(0)
#define CHAN_PARAMS_CHAN_SWITCH_TYPE_MAX		(6)

#define CHAN_PARAMS_BLOCK_TX_PRE_MIN			(0)
#define CHAN_PARAMS_BLOCK_TX_PRE_MAX			(1)

#define CHAN_PARAMS_BLOCK_TX_POST_MIN			(0)
#define CHAN_PARAMS_BLOCK_TX_POST_MAX			(1)

#define CHAN_PARAMS_BG_SCAN_MIN					(0)
#define CHAN_PARAMS_BG_SCAN_MAX					(1)

#define CHAN_PARAMS_CONT_INTERFER_MIN			(0)
#define CHAN_PARAMS_CONT_INTERFER_MAX			(1)

#define CHAN_PARAMS_RADAR_DET_MIN				(0)
#define CHAN_PARAMS_RADAR_DET_MAX				(1)


#define CHAN_PARAMS_ANT_MASK_MIN				(0)
#define CHAN_PARAMS_ANT_MASK_MAX				(0x3f)

#define CHAN_PARAMS_CHAN_NOTIFICATION_MIN		(0)
#define CHAN_PARAMS_CHAN_NOTIFICATION_MAX		(3)

#endif //CPU_ARC

typedef struct _UM_SET_CHAN_PARAMS
{
	//word #1
	uint8 	low_chan_num; 
	uint8 	low_chan_num2; 						// only for use with 80+80 mode
	uint8 	chan_width; 						// values from the chanWidth enum
	uint8 	primary_chan_idx; 					// within the 1 to 8 channels specified by chan_width, i.e., from 0 to 7

	//word #2
	uint16	chan_switch_time; 					// counter value = number of tbtt-s before switch * beacon interval [ms]; calculated by driver since both params are VAP specific yet apply to all VAPs in this case (valid only in switch_type==csa)
	uint8 	switch_type; 						// values from the switchType enum e.g. ST_CSA
	uint8 	block_tx_pre; 						// block TX except beacons and csa action frames before the switch (valid only in switch_type==csa)

	//word #3
	uint8 	block_tx_post; 						// block All Tx including beacons after the switch (valid only in switch_type==csa)
	uint8 	bg_scan; 							// valid only in 1st request with switch_type==scan : signal if scan process is considered a "background" process or  when switch_type = rssi : signal if CTS2SELF should be enabled 0-DISABLED;1-ENABLED)
	uint8 	result; 							// values from setChanReport enumeration
	uint8 	isRadarDetectionNeeded;

	//word #4
	uint16 	u16SID;								//station index: used only when switch_type==ST_RSSI/ST_NORMAL_AFTER_RSSI
	uint8 	isContinuousInterfererDetectionNeeded;
	uint8	reserved1;
	
	//word #5 - currently not in use for Gen6
	int8 	rssi[4];	//rssi per antenna used only when switch_type==ST_NORMAL_AFTER_RSSI
	
	//word #6 - currently not in use for Gen6
	uint8 	noise[4];	//noise per antenna used only when switch_type==ST_NORMAL_AFTER_RSSI

	//word #7 - currently not in use for Gen6
	uint8 	rf_gain[4];	//gain per antenna used only when switch_type==ST_NORMAL_AFTER_RSSI

	//word #8
	uint32	rate;								//Rate of last packet received switch_type==ST_NORMAL_AFTER_RSSI

	//word #9
	uint8	antennaMask;
	uint8   ChannelNotificationMode;
	uint8	RegulationType;
	uint8	Status;

	// words #10 - #53 are valid only when switch_type==ST_CSA

	//word #10
	uint32	csaUcDeAuthVapBitmap; // Indicates which VAPs are to be sent UC DEAUTH frames

	//word #11
	uint32	csaMcDeAuthVapBitmap; // Indicates which VAPs are to be sent MC DEAUTH frames

	//word #12 - word #43
	uint32	csaMcDeauthFramesLocation[GEN6_MAX_VAP]; // Indicates the MC DEAUTH frames location

	//word #44
	uint16	csaTxUcDeauthStartTime; // Timer value which indicates when the AP shall transmit UC DEAUTH frames (e.g. chan_switch_time - 3 beacon intervals).
	uint16	csaTxMcDeauthStartTime; // Timer value which indicates when the AP shall transmit MC DEAUTH frames (e.g. chan_switch_time - 2 beacon interval). 

	//word #45 - word #52
	uint8	csaMcDeauthFrameLength[GEN6_MAX_VAP]; // Indicates the MC DEAUTH frame length per VAP.
	
	//word #53
	uint32 csaIsMcDeauthFrameEncrypted;	// Indicates if the MC DEAUTH frame is encrypted or not. Each bit represents a vap (1 - encrypted. 0 - not encrypted).

	//word #54
	uint8 cts_to_self_duration; // Relevant in switch type = ST_DPP_ACTION and/or ST_SCAN with bs_scan set to TRUE. This value indicates the CTS TO SELF duration in mSec.
	uint8 reserved[3];

} __MTLK_PACKED UM_SET_CHAN_PARAMS;

/***************************************************************************
**
** NAME         UMI_MAN
**
** DESCRIPTION  A union of all Management Messages.
**
****************************************************************************/
typedef union _UMI_MAN
{
    UMI_SET_KEY                     sSetKey;
	UMI_PHY_STATUS_REQ				sGetPhyStatus;
    UMI_CLEAR_KEY                   sClearKey;
    UMI_BCL_REQUEST                 sBclRequest;
    UMI_GENERIC_MAC_REQUEST         sGenericMacRequest;
	UMI_MAC_WATCHDOG				sMacWatchdog;
	UMI_SET_ANTENNAS				sSetAntennas;
    UMI_TX_POWER_LIMIT		        sChangeTxPowerLimit;
    UMI_ADD_VAP						sAddVap;
    UMI_REMOVE_VAP					sRemoveVap;
	UMI_HW_DEPENDENT_CONFIG			sHwDepConfig;
    UMI_DBG_TRACE_IND               sUmiDbgTraceInd;
	UMI_INTERFERER_DETECTION_PARAMS	sUmiInterfererDetectionParams;
	UMI_CONTINUOUS_INTERFERER		sUmiInterferer;
	UMI_RADAR_DETECTION             sUmiRadar;
	UMI_ENABLE_RADAR_INDICATION     sUmiEnableRadarIndication;
	UMI_ANT_SELECTION_11B			sUmi11bAntSel;
	UMI_STA_ADD						sUmiStaAdd;
	UMI_STA_REMOVE					sUmiStaRemove;
	UMI_REQUEST_SID					sUmiRequestSid;
	UMI_REMOVE_SID					sUmiRemoveSid;
	UMI_STOP_TRAFFIC				sUmiStoptraffic;
	UMI_BEACON_SET					sUmiBeaconSet;
	UMI_SET_WMM_PARAMETERS			sUmiSetWmmParameters;
	UM_SET_CHAN_PARAMS				sUmSetChanParams;
	PLATFORM_DATA_FIELDS			sUmiPlatformDataField;
	UMI_ATF_QUOTAS					sUmiAtfQuotas;	
	UMI_TRIGGER_FRAME_CONFIG		sTriggerFrameConfig;
	UMI_PIE_CONTROL					sUmiPieControl;
	UMI_WHM_ENABLE					sUmiWhmEnable;
	UMI_WHM_HW_LOGGER_ENABLE		sUmiWhmHwLoggerEnable;
} __MTLK_PACKED UMI_MAN;

typedef union _UMI_IND
{
    UMI_SET_KEY                     sSetKey;
	UMI_PHY_STATUS_REQ				sGetPhyStatus;
    UMI_CLEAR_KEY                   sClearKey;
    UMI_BCL_REQUEST                 sBclRequest;
    UMI_GENERIC_MAC_REQUEST         sGenericMacRequest;
	UMI_MAC_WATCHDOG				sMacWatchdog;
	UMI_SET_ANTENNAS				sSetAntennas;
    UMI_TX_POWER_LIMIT		        sChangeTxPowerLimit;
    UMI_ADD_VAP						sAddVap;
    UMI_REMOVE_VAP					sRemoveVap;
	UMI_HW_DEPENDENT_CONFIG			sHwDepConfig;
    UMI_DBG_TRACE_IND               sUmiDbgTraceInd;
	UMI_INTERFERER_DETECTION_PARAMS	sUmiInterfererDetectionParams;
	UMI_CONTINUOUS_INTERFERER		sUmiInterferer;
	UMI_RADAR_DETECTION             sUmiRadar;
	UMI_ENABLE_RADAR_INDICATION     sUmiEnableRadarIndication;
	UMI_ANT_SELECTION_11B			sUmi11bAntSel;
	UMI_STA_ADD						sUmiStaAdd;
	UMI_STA_REMOVE					sUmiStaRemove;
	UMI_REQUEST_SID					sUmiRequestSid;
	UMI_REMOVE_SID					sUmiRemoveSid;
	UMI_STOP_TRAFFIC				sUmiStoptraffic;
	UMI_BEACON_SET					sUmiBeaconSet;
	UMI_SET_WMM_PARAMETERS			sUmiSetWmmParameters;
	UM_SET_CHAN_PARAMS				sUmSetChanParams;
	PLATFORM_DATA_FIELDS			sUmiPlatformDataField;
	UMI_TRIGGER_FRAME_CONFIG		sTriggerFrameConfig;
	UMI_DBG_HE_MU_GROUP_STATS		sDbgHeMuGroupStats;
	UMI_WHM_WARNING_DETECTION		sUmiWhmWarningDetection;
	UMI_STA_EXCESSIVE_RETRIES_DETECTION	sUmiStaExcessRetriesDetection;
} __MTLK_PACKED UMI_IND;


/***************************************************************************/
/***                      Top Level UMI Message                          ***/
/***************************************************************************/

/* Management messages between MAC and host */
typedef struct _SHRAM_MAN_MSG
{
    UMI_MSG_HEADER sHdr;                 /* Kernel Message Header */
    UMI_MAN        sMsg;                 /* UMI Management Message */
} __MTLK_PACKED SHRAM_MAN_MSG;
typedef struct _SHRAM_IND_MSG
{
    UMI_MSG_HEADER sHdr;                 /* Kernel Message Header */
    UMI_IND        sMsg;                 /* UMI Indication Message */
} __MTLK_PACKED SHRAM_IND_MSG;

/* Debug messages between MAC and host */
typedef struct _SHRAM_DBG_MSG
{
    UMI_MSG_HEADER sHdr;                 /* Kernel Message Header */
    UMI_DBG        sMsg;                 /* UMI Debug Message */
} __MTLK_PACKED SHRAM_DBG_MSG;


#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* __MHI_UMI_PROPR_INCLUDED_H */
