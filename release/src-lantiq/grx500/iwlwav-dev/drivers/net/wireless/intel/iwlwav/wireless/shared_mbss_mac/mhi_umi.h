/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/***************************************************************************
****************************************************************************
**
** COMPONENT:        ENET Upper MAC    SW
**
** MODULE:           $File: //bwp/enet/demo153_sw/develop/src/shared/mhi_umi.h $
**
** VERSION:          $Revision: #4 $
**
** DATED:            $Date: 2007/03/04 $
**
** AUTHOR:           S Sondergaard
**
** DESCRIPTION:      Upper MAC Public Header
**
****************************************************************************
*
* 
*
* 
*
****************************************************************************/

#ifndef __MHI_UMI_INCLUDED_H
#define __MHI_UMI_INCLUDED_H

#include "mhi_ieee_address.h"
#include "mtlkbfield.h"
#include "shram.h"
#include "mhi_rsn_caps.h"
#include "msgid.h"
#include "mhi_dut.h"
#include "umi_rsn.h"
#include "MT_mac_host_adapt.h"
#include "mtlkbfield.h"
#include "mhi_mac_event.h"
#include "mhi_global_definitions.h"
#define   MTLK_PACK_ON
#include "mtlkpack.h"
#include "mhi_descriptors.h"


#define  TU                             1024 /* 1TU (Time Unit) = 1024 microseconds - defined in 802.11 */

/***************************************************************************/
/***                       Types and Defines                             ***/
/***************************************************************************/
#define MAX_USER_PRIORITIES         4
#define DUTY_CYCLE_DEFAULT_ON_TIME 	0x478B //OnTime: 18315
#define DUTY_CYCLE_DEFAULT_OFF_TIME	0x0E4F //OffTime: 3663 
#define RX_THRESHOLD_DEFAULT_VALUE	(-128)

#define UMI_MAX_MSDU_LENGTH             (MSDU_MAX_LENGTH)

// logger from MT_NSSMemoryPool.h
#define FREE_NSSBUF  0x55555555   //This value is used to mark a free buffer
#define USED_NSSBUF  0xABABABAB   //This value is used to mark an allocated buffer
#define NUM_TID      8

#define LOGGER_NUM_OF_GROUPS_BIT_MAP  4
#define LOGGER_NUM_OF_HW_FIFOS        16


#define GEN6_PHY_METRIC_INVALID 	0X80
#define GEN5_PHY_METRIC_INVALID 	0x0


#define UDP_HEADER_WORD_SIZE		11

#define GEN5_MAX_SID                128
#define GEN6_MAX_SID                256

#define GEN6_NUM_OF_BANDS           2


#define GEN5_MAX_VAP                16
#define GEN6_MAX_VAP                32

#define GEN5_PHY_RSSI_TO_DBM        65  
#define GEN6_PHY_RSSI_TO_DBM        0 

#define GEN5_NOISE_OFFS             58 
#define GEN6_NOISE_OFFS             25

#define MAX_NUM_OF_GROUPS  			(32)

#define GEN5_ALL_STA_SID        0x0FF   /* 2*GEN5_MAX_SID - 1 */
#define GEN6_ALL_STA_SID        0x1FF   /* 2*GEN6_MAX_SID - 1 */
#define ALL_SID_MAX             MAX(GEN5_ALL_STA_SID, GEN6_ALL_STA_SID)

/*--------------------------------------------------------------------------
 * Description:     Bitmasks of required calibr
 *
 * Representation:  uint32 (bitmask)tion procedures
 *
 * Access:          r
 *--------------------------------------------------------------------------*/
#define 	TPC_CLOSED_LOOP				0 //use on TPC Config
#define 	TPC_OPEN_LOOP				1 //use on TPC Config

#define 	NO_FIXED_GAIN	(0xFF)

/*
* Metalink Vendor Specific Action Frame definition
*/
#define ACTION_FRAME_CATEGORY_VENDOR_SPECIFIC 127

//WMM Access Categories
typedef enum AccessCategory
{
	ACCESS_CATEGORY_BE = 0,
	ACCESS_CATEGORY_BK,
	ACCESS_CATEGORY_VI,
	ACCESS_CATEGORY_VO,
	ACCESS_CATEGORY_NUM,
	ACCESS_CATEGORY_INAVLID = 0xFF,
} AccessCategory_e;

/* Metalink OUI (3-byte value).
* See http://standards.ieee.org/regauth/oui/oui.txt 
* for more information.
*/
#define MTLK_OUI_0   0x00
#define MTLK_OUI_1   0x09
#define MTLK_OUI_2   0x86

typedef enum
{
    /* according to numbers defined in BB_UTILS_upper_lower_cpu */
    UMI_CPU_ID_LM,
    UMI_CPU_ID_UM,
    UMI_CPU_ID_ALL,
    UMI_CPU_ID_MAX
} UmiCpuId_e;

typedef enum
{
    /* The type of FW debug stop , requested by the host */
    UMI_DEBUG_ASSERT,
    UMI_DEBUG_EXCEPTION,
	UMI_DEBUG_LAST
} UmiDebugType_e;

//operation mode types
typedef enum {
	OPERATION_MODE_NORMAL = 0,
	OPERATION_MODE_DUT, // 1
	OPERATION_MODE_SNIFFER, // 2
// Req 2: 4.2.2.1 - added enum for vap mode
	OPERATION_MODE_VSTA,
	OPERATION_MODE_MBSSID_TRANSMIT_VAP,
	OPERATION_MODE_MBSSID_NON_TRANSMIT_VAP,
	OPERATION_MODE_INVALID,
	NUM_OF_OPERATION_MODES,
} UmiOperationMode_e; 

//WDS AES support
typedef enum { 
    PEER_AP = 0,                 /* legacy WDS connection is done by looking for other side beacons (no asso and auth) */
    FOUR_ADDRESSES_STATION = 1,   /* 4-way handshake in 3-addr mode; asso and auth exist */
    WPA_WDS = 2,                 /* 4-way handshake in 4-addr mode; No asso and auth exist */
} UmiWDS_AES_e;



typedef uint8 UMI_STATUS;
#define UMI_OK                          0
#define UMI_NOT_INITIALISED             1
#define UMI_BAD_PARAMETER               2
#define UMI_BAD_VALUE                   3
#define UMI_BAD_LENGTH                  4
#define UMI_MC_BUSY                     5
#define UMI_ALREADY_ENABLED             6
#define UMI_HW_FAILURE                  7
#define UMI_BSS_ALREADY_ACTIVE          8
#define UMI_BSS_HAS_NO_CFP              9
#define UMI_BSS_UNKNOWN                 10
#define UMI_STATION_UNKNOWN             11
#define UMI_NOT_ENABLED                 12
#define UMI_OUT_OF_MEMORY               13
#define UMI_TIMEOUT                     14
#define UMI_NOT_CONNECTED               15
#define UMI_UNKNOWN_OBJECT              16
#define UMI_READ_ONLY                   17
#define UMI_WRITE_ONLY                  18
#define UMI_RATE_MISMATCH               19
#define UMI_TRANSFER_ALREADY_ACTIVE     20
#define UMI_TRANSFER_FAILED             21
#define UMI_NOT_SUPPORTED               22
#define UMI_RSN_IE_FORMAT_ERROR         23
#define UMI_RSN_BAD_CAPABILITIES        24
#define UMI_INTERNAL_MAC_ERROR          25
#define UMI_UM_BUSY						26
#define UMI_PS_NOT_ENABLED				27
#define UMI_ADD_BSS_FAIL				28
#define UMI_REMOVE_VAP_FAIL				29
#define UMI_MAX_VAP_WAS_ADDED			30
#define UMI_VAP_DB_FAIL                 31
#define UMI_PEER_AP_ALREADY_EXIST       32
#define UMI_VAP_IS_NOT_AVAILABLE        33
#define UMI_REQUEST_REJECTED        	34

/* Status codes for memory allocation */
#define UMI_ALLOC_OK                    UMI_OK
#define UMI_ALLOC_FAILED                UMI_OUT_OF_MEMORY
#define UMI_ALLOC_FWD_POOL_OK           26
#define UMI_STATUS_TOTAL_NUMBER         35

typedef uint8 UMI_NOTIFICATION;
#define UMI_NOTIFICATION_OK             0
#define UMI_NOTIFICATION_MIC_FAILURE    1

#define UMI_MAX_CHANNELS_PER_SCAN_REQ   16
typedef uint16 UMI_BSS_TYPE;
#define UMI_BSS_INFRA                   0
#define UMI_BSS_INFRA_PCF               1
#define UMI_BSS_ADHOC                   2
#define UMI_BSS_ANY                     3

typedef uint16 UMI_NETWORK_STATUS;
#define UMI_BSS_CREATED                 0   // We have created a network (BSS)
#define UMI_BSS_CONNECTING              1   // STA is trying to connect to AP
#define UMI_BSS_CONNECTED               2   // STA has connected to network (auth and assoc) or AP/STA resume connection after channel switch
#define UMI_BSS_FAILED                  4   // STA is unable to connect with any network
#define UMI_BSS_RADAR_NORM              5   // Regular radar was detected.
#define UMI_BSS_RADAR_HOP               6   // Frequency Hopping radar was detected.
#define UMI_BSS_CHANNEL_SWITCH_NORMAL   7   // STA received a channel announcement with non-silent mode.
#define UMI_BSS_CHANNEL_SWITCH_SILENT   8   // STA received a channel announcement with silent mode.
#define UMI_BSS_CHANNEL_SWITCH_DONE     9   // AP/STA have just switched channel (but traffic may be started only after UMI_BSS_CONNECTED event)
#define UMI_BSS_CHANNEL_PRE_SWITCH_DONE 10  //
#define UMI_BSS_OVER_CHANNEL_LOAD_THRESHOLD 11  // Channel load threshold over.


//PHY characteristics parameters
#define UMI_PHY_TYPE_802_11A          	0x00    /* 802.11a  */
#define UMI_PHY_TYPE_802_11B          	0x01    /* 802.11b  */
#define UMI_PHY_TYPE_802_11G          	0x02    /* 802.11g  */
#define UMI_PHY_TYPE_802_11B_L      	0x81    /* 802.11b with long preamble*/
#define UMI_PHY_TYPE_802_11N_5_2_BAND 	0x04    /* 802.11n_5.2G  */
#define UMI_PHY_TYPE_802_11N_2_4_BAND 	0x05    /* 802.11n_2.4G  */

#define UMI_PHY_TYPE_BAND_5_2_GHZ       0                  
#define UMI_PHY_TYPE_BAND_2_4_GHZ       1
#define UMI_PHY_TYPE_BAND_6_GHZ        	2

#define UMI_PHY_TYPE_UPPER_CHANNEL      0       /* define UPPER secondary channel offset */
#define UMI_PHY_TYPE_LOWER_CHANNEL      1       /* define LOWER secondary channel offset */

#define UMI_PHY_11B_FIRST_RATE          8 // copied from LM_PHY_11B_RATE_2_SHORT
#define UMI_PHY_11N_FIRST_RATE          15 // copied from LM_PHY_11N_RATE_6_5
//Channel SwitchMode values
#define UMI_CHANNEL_SW_MODE_NORMAL		0x00
#define UMI_CHANNEL_SW_MODE_SILENT		0x01
#define UMI_CHANNEL_SW_MODE_MASK		0x0f
#define UMI_CHANNEL_SW_MODE_SCN			0x00 //SCN (NO SECONDARY)
#define UMI_CHANNEL_SW_MODE_SCA			0x10 //SCA (ABOVE)
#define UMI_CHANNEL_SW_MODE_SCB			0x30 //SCB (BELOW)
#define UMI_CHANNEL_SW_MODE_SC_MASK		0xf0
#define UMI_CHANNEL_SW_MODE_SC_SHIFT	(4)


/* Stop reasons */
#define BSS_STOP_REASON_JOINED	  		0	               
#define BSS_STOP_REASON_DISCONNECT		1
#define BSS_STOP_REASON_JOINED_FAILED	2
#define BSS_STOP_REASON_SCAN			3
#define BSS_STOP_REASON_MC_REQ			4
#define BSS_STOP_REASON_BGSCAN			5
#define BSS_STOP_REASON_UM_REQ			6
#define BSS_STOP_REASON_NONE	 		0xFF

typedef uint16 UMI_PCF_CAPABILITY;
#define UMI_NO_PCF                      0
#define UMI_HAS_PCF                     1

typedef uint16 UMI_ACCESS_PROTOCOL;
#define UMI_USE_DCF                     0
#define UMI_USE_PCF                     1



#define PS_REQ_MODE_ON					1
#define PS_REQ_MODE_OFF					0

/*Dual band definitions*/
#define CDB_NUM_OF_SEGMENTS 2
/*************************************
* For the use of the Generic message.
*/
#define MAC_VARIABLES_REQ       1
#define MAC_EEPROM_REQ          2
#define MAC_OCS_TIMER_START     3
#define MAC_OCS_TIMER_STOP      4
#define MAC_OCS_TIMER_TIMEOUT   5


#define NEW_CASE                6

/***************************************************************************/
/***                         memory array definition                     ***/
/***************************************************************************/

// Those defines are used by driver. Do not remove without consulting with driver.

#define ARRAY_NULL              MSG_TYPE_NULL
#define ARRAY_DAT_IND           MSG_TYPE_DAT_IND
#define ARRAY_DAT_REQ           MSG_TYPE_DAT_REQ
#define ARRAY_MAN_IND           MSG_TYPE_MAN_IND
#define ARRAY_MAN_REQ           MSG_TYPE_MAN_REQ
#define ARRAY_DBG_IND           MSG_TYPE_DBG_IND
#define ARRAY_DBG_REQ           MSG_TYPE_DBG_REQ
#define ARRAY_BSS_IND           MSG_TYPE_BSS_IND
#define ARRAY_BSS_REQ           MSG_TYPE_BSS_REQ
#define ARRAY_DAT_LOGGER_IND    MSG_TYPE_DAT_LOGGER_IND /* Logger buffer */


/**************************************************************************************/
/*																								*/
/* Descriptors are used in the host interface FIFOs in the host interface (au32FIFOReqResQ / au32FIFOCfmIndQ	*/
/* All descriptors have TYPE and INDEX. The rest of the bits can be different									*/
/*																								*/
/**************************************************************************************/

#define HIM_DESC_TYPE_NUM_OF_BITS		(5)
#define HIM_DESC_INDEX_NUM_OF_BITS		(10)
#define HIM_DESC_RESERVED1_NUM_OF_BITS	(1)
#define HIM_DESC_RADIO_NUM_OF_BITS		(2)
#define HIM_DESC_VAP_NUM_OF_BITS		(6)
#define HIM_DESC_RESERVED2_NUM_OF_BITS	(8)

// Common for all descriptors
#define HIM_DESC_TYPE				MTLK_BFIELD_INFO(0, HIM_DESC_TYPE_NUM_OF_BITS)			/* 5  bits starting BIT 0  of host interface descriptor */
#define HIM_DESC_INDEX				MTLK_BFIELD_INFO(5, HIM_DESC_INDEX_NUM_OF_BITS)			/* 10 bits starting BIT 5  of host interface descriptor */
#define HIM_DESC_RESERVED1			MTLK_BFIELD_INFO(15, HIM_DESC_RESERVED1_NUM_OF_BITS)	/* 1  bits starting BIT 15 of host interface descriptor */
#define HIM_DESC_RADIO				MTLK_BFIELD_INFO(16, HIM_DESC_RADIO_NUM_OF_BITS)		/* 2  bits starting BIT 16 of host interface descriptor */
#define HIM_DESC_VAP				MTLK_BFIELD_INFO(18, HIM_DESC_VAP_NUM_OF_BITS)			/* 6  bits starting BIT 18 of host interface descriptor */
#define HIM_DESC_RESERVED2			MTLK_BFIELD_INFO(24, HIM_DESC_RESERVED2_NUM_OF_BITS)	/* 8  bits starting BIT 24 of host interface descriptor */
 
#define GEN5_TX_RING_SIZE			(4096)
#define GEN35_TX_RING_SIZE			(1500)
#define GEN5_RX_RING_SIZE			(4096)
#define GEN35_RX_RING_SIZE			(1500)
#define GEN5_MANG_TX_RING_SIZE		(128)
#define GEN5_MANG_RX_RING_SIZE		(128)
#define GEN35_MANG_RX_RING_SIZE		(128)
#define GEN35_MANG_TX_RING_SIZE		(128)

#define HIM_DESC_RADIO_INVALID	(_MTLK_U_AUX_BITS(HIM_DESC_RADIO_NUM_OF_BITS))	/* max of 2 bits: 0x3 */
#define HIM_DESC_VAP_INVALID	(_MTLK_U_AUX_BITS(HIM_DESC_VAP_NUM_OF_BITS))	/* max of 6 bits: 0x3F */

#define INVALID_VAP				(HIM_DESC_VAP_INVALID)
#define VAP_ID_DO_NOT_CARE		(0xFC)

#define MAX_NUM_SUPPORTED_RATES (32)
#define MAX_NUM_OF_TRIGGER_FRAME_STATIONS	(36)

/***************************************************************************/
/***                       UASP VAP  definitions 				         ***/
/***************************************************************************/
#define UAPSD_ENABLE				MTLK_BFIELD_INFO(0, 1)    /*  1 bit  starting bit0 */
#define UAPSD_AP_MAX_SP_LENGTH		MTLK_BFIELD_INFO(1, 3)    /*  3 bit  starting bit0 */

/***************************************************************************/
/***                       UASP connection event   definitions 	         ***/
/***************************************************************************/

#define IND_UAPSD_DELIVERY_ENABLED_BITMAP	MTLK_BFIELD_INFO(0, 4) 

#define UAPSD_BITMAP_VO_AC					MTLK_BFIELD_INFO(0, 1)    /*  1 bit  starting bit0 */
#define UAPSD_BITMAP_VI_AC					MTLK_BFIELD_INFO(1, 1)    /*  1 bit  starting bit1*/
#define UAPSD_BITMAP_BK_AC					MTLK_BFIELD_INFO(2, 1)    /*  1 bit  starting bit2 */
#define UAPSD_BITMAP_BE_AC					MTLK_BFIELD_INFO(3, 1)    /*  1 bit  starting bit3 */

#define IND_UAPSD_MAX_SP_LENGTH				MTLK_BFIELD_INFO(4, 3)    /*  3 bit  starting bit4 */


/***************************************************************************/
/***                       SET_BSS flags definitions				     ***/
/***************************************************************************/
#define VAP_ADD_FLAGS_HT					MTLK_BFIELD_INFO(0, 1)    /*  1 bit  starting bit0 */
#define VAP_ADD_FLAGS_VHT					MTLK_BFIELD_INFO(1, 1)    /*  1 bit  starting bit1 */
#define VAP_ADD_FLAGS_HE					MTLK_BFIELD_INFO(2, 1)    /*  1 bit  starting bit2 */
#define VAP_ADD_FLAGS_SMPS					MTLK_BFIELD_INFO(3, 2)    /*  2 bits starting bit3 */
#define VAP_ADD_FLAGS_RELIABLE_MCAST		MTLK_BFIELD_INFO(5, 1)    /*  1 bits starting bit5 */
#define VAP_ADD_FLAGS_HS20_ENABLE			MTLK_BFIELD_INFO(6, 1)    /*  1 bits starting bit6 */

/***************************************************************************/
/***           u8VHT_OperatingModeNotification flags definitions  		 ***/
/***************************************************************************/

#define OPER_MODE_CHANNEL_WIDTH 			MTLK_BFIELD_INFO(0, 2)	  /*  2 bit  starting bit0 */
#define OPER_MODE_RESERVED					MTLK_BFIELD_INFO(2, 2)	  /*  2 bit  starting bit2 */
#define OPER_MODE_RX_NSS					MTLK_BFIELD_INFO(4, 3)	  /*  3 bit  starting bit4 */
#define OPER_MODE_RX_NSS_TYPE				MTLK_BFIELD_INFO(7, 1)	  /*  1 bit  starting bit7 */


/***************************************************************************/
/***                         Message IDs                                 ***/
/***************************************************************************/

typedef enum _UMI_Msgs
{
	UMI_MSGS_START,
	UMI_DOWNLOAD_PROG_MODEL = UMI_MSGS_START,
	UMI_MAN_CLASS3_ERROR, 					// UMI_FRAME_CLASS_ERROR
	UMI_MAN_SET_KEY, 						// UMI_SET_KEY
	UMI_MAN_SET_BCL_VALUE, 					// UMI_BCL_REQUEST
	UMI_MAN_QUERY_BCL_VALUE, 				// UMI_BCL_REQUEST
	UMI_MAN_SET_WMM_PARAMETERS, 			// UMI_SET_WMM_PARAMETERS
	UMI_MAN_GENERIC_MAC, 					// UMI_GENERIC_MAC_REQUEST
	UMI_MAN_SW_RESET_MAC, 					// None
	UMI_DATA_PATH_INIT, 					// UMI_DATA_PATH_INIT_PARAMS
	UMI_MAN_PVT_READ,						// UMI_PVT
	UMI_MAN_GET_GROUP_PN, 					// UMI_GROUP_PN
	UMI_SET_CHAN, 							// UM_SET_CHAN_PARAMS 
	UMI_MAN_SET_CHANNEL_LOAD_VAR, 			// UMI_GET_CHANNEL_LOAD_REQ 
	UMI_MAN_SET_ANTENNAS,		 			// UMI_SET_ANTENNAS 
	UMI_MAN_DOWNLOAD_PROG_MODEL_PERMISSION, // None
	UMI_MAN_HW_DEPENDENT_CONFIG, 			// UMI_HW_DEPENDENT_CONFIG 
	UMI_MAN_VAP_STOP_VAP_TRAFFIC, 			// UMI_STOP_VAP_TRAFFIC 
	UMI_MAN_ADD_VAP, 						// UMI_ADD_VAP 
	UMI_MAN_REMOVE_VAP, 					// UMI_STOP_VAP_TRAFFIC
	UMI_MAN_SET_BSS, 						// UMI_SET_BSS 
	UMI_MAN_SET_INTERFERER_DETECTION_PARAMS, // UMI_INTERFERER_DETECTION_PARAMS 
	UMI_MAN_SEND_11B_SET_ANT, 				// UMI_ANT_SELECTION_11B
	UMI_MAN_TRACE, 							// UMI_DBG_TRACE_INO
	UMI_MAN_CONTINUOUS_INTERFERER, 			// UMI_CONTINUOUS_INTERFERER 
	UMI_MAN_RADAR,        					// UMI_RADAR_DETECTION 
    UMI_MAN_HDK_USER_DEMAND,                // UMI_HDK_USER_DEMAND 
    UMI_MAN_QAMPLUS_ACTIVATE,               // UMI_QAMPLUS_ACTIVATE 
    UMI_MAN_GET_STATISTICS,           		// UMI_GET_STATISTICS_DATA 
	UMI_DBG_DUT_MSG, 						// UMI_DUT 
	UMI_DBG_MAC_WATCHDOG, 					// UMI_MAC_WATCHDOG
	UMI_DBG_LOGGER_FLUSH_BUF,				// UMI_LOGGER_MSG_FLUSH
	UMI_DBG_LOGGER_SET_MODE, 				// UMI_LOGGER_MSG_SET_STATE 
	UMI_DBG_LOGGER_SET_SEVERITY, 			// Not in use
	UMI_DBG_LOGGER_SET_FILTER, 				// UMI_LOGGER_MSG_SET_FILTER 
	UMI_DBG_CLI, 							// UMI_DBG_CLI_REQ
	UMI_MAN_MAC_EVENT, 						// MAC_EVENT 
	UMI_DBG_FW_DBG, 						// UMI_FW_DBG_REQ
	UMI_MAN_STA_ADD, 						// UMI_STA_ADD
	UMI_MAN_STA_REMOVE, 					// UMI_STA_REMOVE
	UMI_MAN_SET_BEACON_TEMPLATE, 			// UMI_BEACON_SET 
	UMI_MAN_BEACON_TEMPLATE_WAS_SET, 		// UMI_BEACON_SET 
	UMI_MAN_REQUEST_SID, 					// UMI_REQUEST_SID
	UMI_MAN_REMOVE_SID, 					// UMI_REMOVE_SID
	UMI_MAN_STOP_TRAFFIC, 					// UMI_STOP_TRAFFIC
	UMI_MAN_SET_DEFAULT_KEY_INDEX,			// UMI_DEFAULT_KEY_INDEX
	UMI_MAN_SET_802_1X_FILTER,				// UMI_802_1X_FILTER
	UMI_MAN_TKIP_MIC_FAILURE,               // UMI_TKIP_MIC_FAILURE 
	UMI_MAN_HW_LOGGER_ADD_STREAM,           // UMI_ADD_STREAM_REQ 
	UMI_MAN_HW_LOGGER_REMOVE_STREAM,		// UMI_REMOVE_STREAM_REQ
	UMI_MAN_HW_LOGGER_ADD_FILTER,			// UMI_ADD_FIFO_FILTER_REQ
	UMI_MAN_HW_LOGGER_REMOVE_FILTER,		// UMI_REMOVE_FIFO_FILTER_REQ
	UMI_MAN_HW_LOGGER_SET_TRIGGER,			// UMI_SET_TRIGGER_REQ
	UMI_MAN_HW_LOGGER_RESET_TRIGGER,		// UMI_RESET_TRIGGER_REQ
	UMI_MAN_SET_TPC_ANT_PARAMS,				// tpcAntParams_t
	UMI_MAN_SET_TPC_CONFIG,					// tpcConfig_t 
	UMI_MAN_HDK_CONFIG,						// UMI_HDK_CONFIG 
	UMI_MAN_POLL_CLIENT,					// UMI_POLL_CLIENT
	/*Termporary - untill integration with new Set channel  */
	UMI_MAN_MBSS_PRE_ACTIVATE,				// UMI_MBSS_PRE_ACTIVATE
	UMI_MAN_CALIBRATE,						// UMI_CALIBRATE_PARAMS 
	UMI_MAN_DOWNLOAD_PROG_MODEL_20_40_DIFFS, // UMI_PROG_MODEL_20_40_DIFFS_PARAMS 
	UMI_MAN_SET_MULTICAST_MODE,				// UMI_MULTICAST_MODE 
	UMI_MAN_MULTICAST_ACTION,				// UMI_MULTICAST_ACTION 
	UMI_MAN_PLATFORM_DATA_FIELDS, 			// PLATFORM_DATA_FIELDS 
	UMI_MAN_PLATFORM_TABLE,					// PLATFORM_TABLE 
	UMI_MAN_SET_AFE_CALIBRATION_DATA,		// AFE_CALIBRATION_DATA 
	UMI_MAN_SET_RFIC_CALIBRATION_DATA,		// RFIC_CALIBRATION_DATA 
	UMI_MAN_SET_RSSI_CAL_PARAMS,			// UMI_SET_RSSI_CAL_CONFIG
	UMI_MAN_ENABLE_RADIO,					// UMI_ENABLE_RADIO 
	UMI_MAN_ENABLE_RADAR_INDICATION,		// UMI_ENABLE_RADAR_INDICATION 
	UMI_MAN_SET_AGG_RATE_LIMIT,				// UMI_AGG_RATE_LIMIT 
	UMI_MAN_SET_POWER_LIMIT,				// UMI_TX_POWER_LIMIT 
	UMI_TEST_BUS_ENABLE,					// UMI_TEST_BUS_EN
	UMI_MAN_SET_RX_TH,						// UMI_RX_TH 
	UMI_MAN_SET_RX_DUTY_CYCLE,				// UMI_RX_DUTY_CYCLE 
	UMI_MAN_SET_ADMISSION_CAPACITY,			// UMI_UPDATE_ADMISSION_CAPACITY 
	UMI_MAN_CPU_LOAD,						// None
	UMI_MAN_TS_VAP_CONFIGURATION,			// UMI_TS_VAP_CONFIGURE
	UMI_MAN_MSDU_IN_AMSDU_CONFIG,			// UMI_MSDU_IN_AMSDU_CONFIG 
	UMI_MAN_MU_OPERATION_CONFIG,			// UMI_MU_OPERATION_CONFIG 
	UMI_MAN_CCA_TH,							// UMI_CCA_TH 
	UMI_MAN_RADAR_DETECTION_RSSI_TH_CONFIG,	// UMI_RADAR_DETECTION_RSSI_TH_CONFIG
	UMI_MAN_RTS_MODE_CONFIG,				// UMI_RTS_MODE_CONFIG 
	UMI_MAN_MAX_MPDU_LENGTH,				// UMI_MAX_MPDU 
	UMI_MAN_BF_MODE_CONFIG,					// UMI_BF_MODE_CONFIG
	UMI_MAN_IRE_CONTROL_B,					// UMI_CONTROL 
	UMI_MAN_FIXED_RATE_CONFIG,				// UMI_FIXED_RATE_CONFIG
	UMI_MAN_SSB_MODE,						// UMI_SSB_Mode 
	UMI_MAN_BEACON_BLOCKED,					// UMI_BEACON_BLOCK
	UMI_MAN_BEACON_BLOCKED_INTERVAL,		// UMI_BEACON_BLOCK_TIMER_INTERVAL 
	UMI_MAN_GET_CCA_TH,                 	// UMI_CCA_TH 
	UMI_MAN_FREQ_JUMP_MODE,					// UMI_ENABLE_FREQUENCY_JUMP 		
	UMI_MAN_SLOW_PROBING_MASK,     			// UMI_DISABLE_POWER_ADAPT
	UMI_MAN_SET_TXOP_CONFIG,				// UMI_SET_TXOP_CONFIG
	UM_MAN_SET_BEACON_INTERVAL, 		    // UM_BEACON_INTERVAL
	UMI_2_4GHZ_COEX,						// UMI_SET_2_4_G_COEX 
	UMI_MAN_ATF_QUOTAS,						// UMI_ATF_QUOTAS 
	UMI_MAN_STATIC_PLAN_CONFIG,				// UMI_STATIC_PLAN_CONFIG
	UMI_MAN_FIXED_LTF_AND_GI,				// UMI_FIXED_LTF_AND_GI_REQ
	UMI_MAN_FIXED_POWER,					// FIXED_POWER
	UMI_MAN_OPERATING_MODE,					// OPERATING_MODE
	UMI_MAN_HDK_SET_ANT_CONFIG,				// UMI_HDK_SET_ANTENNA_REQ
	UMI_MAN_HDK_PREPARE_ANT_CONFIG,			// ANTENNA_PARAMETERS
	UMI_MAN_HDK_DOWNLOAD_PROGMODEL_ANT_DEPENDENT_TX,	// Raw data
	UMI_MAN_HDK_DOWNLOAD_PROGMODEL_ANT_DEPENDENT_RX,	// Raw data
	UMI_MAN_SET_QOS_PD_THRESH,				// UMI_SET_PD_THRESH 
	UMI_MAN_SET_RESTRICTED_AC_MODE,			// UMI_SET_RESTRICTED_AC 
	UMI_MAN_FAST_DROP_CONFIG,				// UMI_FAST_DROP_CONFIG_REQ 	
	UMI_MAN_ERP_SET,				        // UMI_ERP_SET 
	UMI_MAN_DMR_CONFIG,						// UMI_DMR_CONFIG
	UMI_MAN_HE_MU_DBG_IND,					// UMI_DBG_HE_MU_GROUP_STATS
	UMI_MAN_SET_REG_DOMAIN_CONFIG,			// UMI_REG_DOMAIN_CONFIG
	UMI_MAN_RTS_RATE_SET,					// UMI_PROTECTION_RATE_CONFIG	
	UMI_MAN_NFRP_CONFIG,					// UMI_NFRP_CONFIG
	UMI_MAN_DUTY_CYCLE_SET,					// UMI_DC_SET
	UMI_MAN_SET_DYNAMIC_MU_TYPE,			// UMI_DYNAMIC_MU_TYPE	
	UMI_MAN_SET_HE_MU_FIXED_PARAMETERS,		// UMI_HE_MU_FIXED_PARAMTERS
	UMI_MAN_SET_HE_MU_DURATION,				// UMI_HE_MU_DURATION
	UMI_MAN_GET_TWT_PARAMS,					// UMI_GET_TWT_PARAMS
	UMI_MAN_GET_AX_DEFAULT_PARAMS,			// UMI_MAN_GET_AX_DEFAULT_PARAMS
	UMI_MAN_SET_ETSI_PPDU_DURATION_LIMITS,	// UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS	
	UMI_MAN_SET_RTS_THRESHOLD,				// UMI_SET_RTS_THRESHOLD
	UMI_MAN_SET_RETRY_LIMIT,				// UMI_RETRY_LIMIT
	UMI_MAN_PIE,							// UMI_PIE_CONTROL
	UMI_MAN_SET_AP_BEACON_INFO,				// UMI_SET_AP_BEACON_INFO
	UMI_MAN_SET_UNSOLICITED,				// UMI_UNSOLICITED_SET
	UMI_MAN_MU_OFDMA_BF_CONFIG_REQ,			// UMI_MAN_MU_OFDMA_BF_CONFIG_REQ
	UMI_MAN_EXT_BLOCKED_ACCESS,				// UMI_EXT_BLOCKED_ACCESS
	UMI_MAN_PREAMBLE_PUNCTURE_CCA_OVERRIDE, // UMI_PREAMBLE_PUNCT_CCA_OVERRIDE
	UMI_MAN_ADVERTISE_BC_TWT_SP, 			// UMI_ADVERTISE_BC_TWT_SP
	UMI_MAN_TERMINATE_BC_TWT_SP, 			// UMI_TERMINATE_BC_TWT_SP	
	UMI_MAN_SEND_TWT_TEARDOWN_FRAME, 		// UMI_SEND_TWT_TEARDOWN_FRAME	
	UMI_MAN_ENABLE_WHM,						// UMI_WHM_ENABLE
	UMI_MAN_ENABLE_HW_LOGGER,				// UMI_WHM_HW_LOGGER_ENABLE
	UMI_MAN_WARNING_DETECTED,				// UMI_WHM_WARNING_DETECTION
	UMI_MAN_STA_EXCESSIVE_RETRIES_DETECTED, // UMI_STA_EXCESSIVE_RETRIES_DETECTION
	UMI_MAN_SET_EXCESSIVE_RETRIES_LIMIT,	// UMI_EXCESSIVE_RETRIES_LIMIT
	UMI_TOTAL_NUM_MSGS
} UMI_Msgs;


/*   host IF messages (REQ-CFM / IND-RES)    */
#define UM_DOWNLOAD_PROG_MODEL_REQ      UMI_MSG_MAN_REQ(UMI_DOWNLOAD_PROG_MODEL) // 0x0400
#define MC_DOWNLOAD_PROG_MODEL_CFM      UMI_MSG_MAN_CFM(UMI_DOWNLOAD_PROG_MODEL) // 0x1400

#define MC_MAN_CLASS3_ERROR_IND         UMI_MSG_MAN_IND(UMI_MAN_CLASS3_ERROR) // 0x3301
#define UM_MAN_CLASS3_ERROR_RES         UMI_MSG_MAN_RES(UMI_MAN_CLASS3_ERROR) // 0x2301

#define UM_MAN_SET_KEY_REQ              UMI_MSG_MAN_REQ(UMI_MAN_SET_KEY) // 0x0402
#define MC_MAN_SET_KEY_CFM              UMI_MSG_MAN_CFM(UMI_MAN_SET_KEY) // 0x1402

#define UM_MAN_SET_BCL_VALUE            UMI_MSG_MAN_REQ(UMI_MAN_SET_BCL_VALUE) // 0x0403
#define MC_MAN_SET_BCL_CFM              UMI_MSG_MAN_CFM(UMI_MAN_SET_BCL_VALUE) // 0x1403

#define UM_MAN_QUERY_BCL_VALUE          UMI_MSG_MAN_REQ(UMI_MAN_QUERY_BCL_VALUE) // 0x0404
#define MC_MAN_QUERY_BCL_CFM            UMI_MSG_MAN_CFM(UMI_MAN_QUERY_BCL_VALUE) // 0x1404

#define UM_MAN_SET_WMM_PARAMETERS_REQ   UMI_MSG_MAN_REQ(UMI_MAN_SET_WMM_PARAMETERS) //0x0405
#define MC_MAN_SET_WMM_PARAMETERS_CFM   UMI_MSG_MAN_CFM(UMI_MAN_SET_WMM_PARAMETERS) //0x1405

#define UM_MAN_GENERIC_MAC_REQ          UMI_MSG_MAN_REQ(UMI_MAN_GENERIC_MAC) // 0x0406
#define MC_MAN_GENERIC_MAC_CFM          UMI_MSG_MAN_CFM(UMI_MAN_GENERIC_MAC) // 0x1406

#define UM_MAN_SW_RESET_MAC_REQ         UMI_MSG_MAN_REQ(UMI_MAN_SW_RESET_MAC) // 0x0407
#define MC_MAN_SW_RESET_MAC_CFM         UMI_MSG_MAN_CFM(UMI_MAN_SW_RESET_MAC) // 0x1407

#define UM_MAN_DATA_PATH_INIT_REQ       UMI_MSG_MAN_REQ(UMI_DATA_PATH_INIT) //0x0408 
#define MC_MAN_DATA_PATH_INIT_CFM       UMI_MSG_MAN_CFM(UMI_DATA_PATH_INIT) //0x1408 

#define UM_MAN_PVT_READ_REQ             UMI_MSG_MAN_REQ(UMI_MAN_PVT_READ) // 0x0409
#define MC_MAN_PVT_READ_CFM             UMI_MSG_MAN_CFM(UMI_MAN_PVT_READ) // 0x1409

#define UM_MAN_GET_GROUP_PN_REQ         UMI_MSG_MAN_REQ(UMI_MAN_GET_GROUP_PN) // 0x040A
#define MC_MAN_GET_GROUP_PN_CFM         UMI_MSG_MAN_CFM(UMI_MAN_GET_GROUP_PN) // 0x140A

#define UM_SET_CHAN_REQ                 UMI_MSG_MAN_REQ(UMI_SET_CHAN) // 0x040B
#define UM_SET_CHAN_CFM                 UMI_MSG_MAN_CFM(UMI_SET_CHAN) // 0x140B

#define UM_MAN_GET_CHANNEL_LOAD_REQ     UMI_MSG_MAN_REQ(UMI_MAN_SET_CHANNEL_LOAD_VAR) // 0x040C
#define MC_MAN_GET_CHANNEL_LOAD_CFM     UMI_MSG_MAN_CFM(UMI_MAN_SET_CHANNEL_LOAD_VAR) // 0x140C

#define MC_MAN_SET_ANTENNAS_REQ         UMI_MSG_MAN_REQ(UMI_MAN_SET_ANTENNAS) // 0x040D
#define UM_MAN_SET_ANTENNAS_CFM         UMI_MSG_MAN_CFM(UMI_MAN_SET_ANTENNAS) // 0x140D

#define UM_MAN_DOWNLOAD_PROG_MODEL_PERMISSION_REQ   UMI_MSG_MAN_REQ(UMI_MAN_DOWNLOAD_PROG_MODEL_PERMISSION) // 0x040E
#define MC_MAN_DOWNLOAD_PROG_MODEL_PERMISSION_CFM   UMI_MSG_MAN_CFM(UMI_MAN_DOWNLOAD_PROG_MODEL_PERMISSION) // 0x140E

#define UM_MAN_HW_DEPENDENT_CONFIG_REQ 	UMI_MSG_MAN_REQ(UMI_MAN_HW_DEPENDENT_CONFIG) // 0x040F
#define MC_MAN_HW_DEPENDENT_CONFIG_CFM 	UMI_MSG_MAN_CFM(UMI_MAN_HW_DEPENDENT_CONFIG) // 0x140F

#define UM_MAN_STOP_VAP_TRAFFIC_REQ 	UMI_MSG_MAN_REQ(UMI_MAN_VAP_STOP_VAP_TRAFFIC) // 0x0410
#define MC_MAN_STOP_VAP_TRAFFIC_CFM 	UMI_MSG_MAN_CFM(UMI_MAN_VAP_STOP_VAP_TRAFFIC) // 0x1410

#define UM_MAN_ADD_VAP_REQ              UMI_MSG_MAN_REQ(UMI_MAN_ADD_VAP) // 0x0411
#define MC_MAN_ADD_VAP_CFM              UMI_MSG_MAN_CFM(UMI_MAN_ADD_VAP) // 0x1411

#define UM_MAN_REMOVE_VAP_REQ           UMI_MSG_MAN_REQ(UMI_MAN_REMOVE_VAP) // 0x0412
#define MC_MAN_REMOVE_VAP_CFM           UMI_MSG_MAN_CFM(UMI_MAN_REMOVE_VAP) // 0x1412

#define UM_BSS_SET_BSS_REQ              UMI_MSG_MAN_REQ(UMI_MAN_SET_BSS) // 0x0413
#define MC_BSS_SET_BSS_CFM              UMI_MSG_MAN_CFM(UMI_MAN_SET_BSS) // 0x1413

#define UM_MAN_SET_INTERFERER_DETECTION_PARAMS_REQ 	UMI_MSG_MAN_REQ(UMI_MAN_SET_INTERFERER_DETECTION_PARAMS) // 0x0414
#define MC_MAN_SET_INTERFERER_DETECTION_PARAMS_CFM 	UMI_MSG_MAN_CFM(UMI_MAN_SET_INTERFERER_DETECTION_PARAMS) // 0x1414

#define UM_MAN_SEND_11B_SET_ANT_REQ         UMI_MSG_MAN_REQ(UMI_MAN_SEND_11B_SET_ANT) // 0x0415
#define MC_MAN_SEND_11B_SET_ANT_CFM         UMI_MSG_MAN_CFM(UMI_MAN_SEND_11B_SET_ANT) // 0x1415

#define MC_MAN_TRACE_IND                    UMI_MSG_MAN_IND(UMI_MAN_TRACE) // 0x3316
#define UM_MAN_TRACE_RES                    UMI_MSG_MAN_RES(UMI_MAN_TRACE) // 0x2316

#define MC_MAN_CONTINUOUS_INTERFERER_IND    UMI_MSG_MAN_IND(UMI_MAN_CONTINUOUS_INTERFERER) // 0x3317
#define UM_MAN_CONTINUOUS_INTERFERER_RES    UMI_MSG_MAN_RES(UMI_MAN_CONTINUOUS_INTERFERER) // 0x2317

#define MC_MAN_RADAR_IND                    UMI_MSG_MAN_IND(UMI_MAN_RADAR) // 0x3318
#define UM_MAN_RADAR_RES                    UMI_MSG_MAN_RES(UMI_MAN_RADAR) // 0x2318

#define UM_MAN_HDK_USER_DEMAND_REQ          UMI_MSG_MAN_REQ(UMI_MAN_HDK_USER_DEMAND) //0x0419
#define MC_MAN_HDK_USER_DEMAND_CFM          UMI_MSG_MAN_CFM(UMI_MAN_HDK_USER_DEMAND) //0x1419

#define UM_MAN_QAMPLUS_ACTIVATE_REQ         UMI_MSG_MAN_REQ(UMI_MAN_QAMPLUS_ACTIVATE) // 0x041A
#define MC_MAN_QAMPLUS_ACTIVATE_CFM         UMI_MSG_MAN_CFM(UMI_MAN_QAMPLUS_ACTIVATE) // 0x141A
 
#define UM_MAN_GET_STATISTICS_REQ           UMI_MSG_MAN_REQ(UMI_MAN_GET_STATISTICS) //0x041B
#define MC_MAN_GET_STATISTICS_CFM           UMI_MSG_MAN_CFM(UMI_MAN_GET_STATISTICS) //0x141B

// Until driver for Dut application will align with FW use this  defines 
#define UM_DBG_DUT_MSG_REQ                  UMI_MSG_DBG_REQ(UMI_DBG_DUT_MSG) // 0x061C
#define MC_DBG_DUT_MSG_CFM                  UMI_MSG_DBG_CFM(UMI_DBG_DUT_MSG) // 0x161C

#define UM_DBG_MAC_WATCHDOG_REQ             UMI_MSG_DBG_REQ(UMI_DBG_MAC_WATCHDOG) // 0x061D
#define MC_DBG_MAC_WATCHDOG_CFM             UMI_MSG_DBG_CFM(UMI_DBG_MAC_WATCHDOG) // 0x161D

/* Logger Messages */
#define UM_DBG_LOGGER_FLUSH_BUF_REQ         UMI_MSG_DBG_REQ(UMI_DBG_LOGGER_FLUSH_BUF) // 0x061E
#define MC_DBG_LOGGER_FLUSH_BUF_CFM         UMI_MSG_DBG_CFM(UMI_DBG_LOGGER_FLUSH_BUF) // 0x161E

#define UM_DBG_LOGGER_SET_MODE_REQ      	UMI_MSG_DBG_REQ(UMI_DBG_LOGGER_SET_MODE) // 0x061F
#define MC_DBG_LOGGER_SET_MODE_CFM      	UMI_MSG_DBG_CFM(UMI_DBG_LOGGER_SET_MODE) // 0x161F

#define UM_DBG_LOGGER_SET_SEVERITY_REQ  	UMI_MSG_DBG_REQ(UMI_DBG_LOGGER_SET_SEVERITY) // 0x0620
#define MC_DBG_LOGGER_SET_SEVERITY_CFM  	UMI_MSG_DBG_CFM(UMI_DBG_LOGGER_SET_SEVERITY) // 0x1620

#define UM_DBG_LOGGER_SET_FILTER_REQ        UMI_MSG_DBG_REQ(UMI_DBG_LOGGER_SET_FILTER) // 0x0621
#define MC_DBG_LOGGER_SET_FILTER_CFM        UMI_MSG_DBG_CFM(UMI_DBG_LOGGER_SET_FILTER) // 0x1621

#define UM_DBG_CLI_REQ                      UMI_MSG_DBG_REQ(UMI_DBG_CLI) // 0x0622
#define MC_DBG_CLI_CFM                      UMI_MSG_DBG_CFM(UMI_DBG_CLI) // 0x1622

#define MC_MAN_MAC_EVENT_IND                UMI_MSG_MAN_IND(UMI_MAN_MAC_EVENT) //0x3323
#define UM_MAN_MAC_EVENT_RES                UMI_MSG_MAN_RES(UMI_MAN_MAC_EVENT) //0x2323

#define UM_DBG_FW_DBG_REQ                   UMI_MSG_DBG_REQ(UMI_DBG_FW_DBG) // 0x0624
#define MC_DBG_FW_DBG_CFM                   UMI_MSG_DBG_CFM(UMI_DBG_FW_DBG) // 0x1624

#define UM_MAN_STA_ADD_REQ 					UMI_MSG_MAN_REQ(UMI_MAN_STA_ADD) // 0x0425
#define MC_MAN_STA_ADD_CFM 					UMI_MSG_MAN_CFM(UMI_MAN_STA_ADD) // 0x1425

#define UM_MAN_STA_REMOVE_REQ 				UMI_MSG_MAN_REQ(UMI_MAN_STA_REMOVE) // 0x0426
#define MC_MAN_STA_REMOVE_CFM 				UMI_MSG_MAN_CFM(UMI_MAN_STA_REMOVE) // 0x1426

#define UM_MAN_SET_BEACON_TEMPLATE_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_BEACON_TEMPLATE) // 0x0427
#define MC_MAN_SET_BEACON_TEMPLATE_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_BEACON_TEMPLATE) // 0x1427

#define MC_MAN_BEACON_TEMPLATE_WAS_SET_IND	UMI_MSG_MAN_IND(UMI_MAN_BEACON_TEMPLATE_WAS_SET) //0x3328
#define UM_MAN_BEACON_TEMPLATE_WAS_SET_RES	UMI_MSG_MAN_RES(UMI_MAN_BEACON_TEMPLATE_WAS_SET) //0x2328

#define UM_MAN_REQUEST_SID_REQ				UMI_MSG_MAN_REQ(UMI_MAN_REQUEST_SID) //0x0429
#define MC_MAN_REQUEST_SID_CFM				UMI_MSG_MAN_CFM(UMI_MAN_REQUEST_SID) //0x1429

#define UM_MAN_REMOVE_SID_REQ				UMI_MSG_MAN_REQ(UMI_MAN_REMOVE_SID) //0x042A
#define MC_MAN_REMOVE_SID_CFM				UMI_MSG_MAN_CFM(UMI_MAN_REMOVE_SID) //0x142A

#define UM_MAN_STOP_TRAFFIC_REQ				UMI_MSG_MAN_REQ(UMI_MAN_STOP_TRAFFIC) //0x042B
#define MC_MAN_STOP_TRAFFIC_CFM				UMI_MSG_MAN_CFM(UMI_MAN_STOP_TRAFFIC) //0x142B

#define UM_MAN_SET_DEFAULT_KEY_INDEX_REQ	UMI_MSG_MAN_REQ(UMI_MAN_SET_DEFAULT_KEY_INDEX) //0x042C
#define MC_MAN_SET_DEFAULT_KEY_INDEX_CFM	UMI_MSG_MAN_CFM(UMI_MAN_SET_DEFAULT_KEY_INDEX) //0x142C

#define UM_MAN_SET_802_1X_FILTER_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_802_1X_FILTER) //0x042D
#define MC_MAN_SET_802_1X_FILTER_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_802_1X_FILTER) //0x142D
	
#define MC_MAN_TKIP_MIC_FAILURE_IND 		UMI_MSG_MAN_IND(UMI_MAN_TKIP_MIC_FAILURE) // 0x332E
#define UM_MAN_TKIP_MIC_FAILURE_RES 		UMI_MSG_MAN_RES(UMI_MAN_TKIP_MIC_FAILURE) // 0x232E

#define UM_MAN_HW_LOGGER_ADD_STREAM_REQ		UMI_MSG_MAN_REQ(UMI_MAN_HW_LOGGER_ADD_STREAM) //0x042F
#define MC_MAN_HW_LOGGER_ADD_STREAM_CFM		UMI_MSG_MAN_CFM(UMI_MAN_HW_LOGGER_ADD_STREAM) //0x142F

#define UM_MAN_HW_LOGGER_REMOVE_STREAM_REQ	UMI_MSG_MAN_REQ(UMI_MAN_HW_LOGGER_REMOVE_STREAM) //0x0430
#define MC_MAN_HW_LOGGER_REMOVE_STREAM_CFM	UMI_MSG_MAN_CFM(UMI_MAN_HW_LOGGER_REMOVE_STREAM) //0x1430

#define UM_MAN_HW_LOGGER_ADD_FILTER_REQ		UMI_MSG_MAN_REQ(UMI_MAN_HW_LOGGER_ADD_FILTER) //0x0431
#define MC_MAN_HW_LOGGER_ADD_FILTER_CFM		UMI_MSG_MAN_CFM(UMI_MAN_HW_LOGGER_ADD_FILTER) //0x1431

#define UM_MAN_HW_LOGGER_REMOVE_FILTER_REQ	UMI_MSG_MAN_REQ(UMI_MAN_HW_LOGGER_REMOVE_FILTER) //0x0432
#define UM_MAN_HW_LOGGER_REMOVE_FILTER_CFM	UMI_MSG_MAN_CFM(UMI_MAN_HW_LOGGER_REMOVE_FILTER) //0x1432

#define UM_MAN_HW_LOGGER_SET_TRIGGER_REQ	UMI_MSG_MAN_REQ(UMI_MAN_HW_LOGGER_SET_TRIGGER) //0x0433
#define UM_MAN_HW_LOGGER_SET_TRIGGER_CFM	UMI_MSG_MAN_CFM(UMI_MAN_HW_LOGGER_SET_TRIGGER) //0x1433

#define UM_MAN_HW_LOGGER_RESET_TRIGGER_REQ	UMI_MSG_MAN_REQ(UMI_MAN_HW_LOGGER_RESET_TRIGGER) //0x0434
#define UM_MAN_HW_LOGGER_RESET_TRIGGER_CFM	UMI_MSG_MAN_CFM(UMI_MAN_HW_LOGGER_RESET_TRIGGER) //0x1434

#define UM_MAN_SET_TPC_ANT_PARAMS_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_TPC_ANT_PARAMS)	//0x0435
#define MC_MAN_SET_TPC_ANT_PARAMS_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_TPC_ANT_PARAMS)	//0x1435

#define UM_MAN_SET_TPC_CONFIG_REQ			UMI_MSG_MAN_REQ(UMI_MAN_SET_TPC_CONFIG)	//0x0436
#define MC_MAN_SET_TPC_CONFIG_CFM			UMI_MSG_MAN_CFM(UMI_MAN_SET_TPC_CONFIG)	//0x1436

#define UM_MAN_HDK_CONFIG_REQ				UMI_MSG_MAN_REQ(UMI_MAN_HDK_CONFIG)	//0x0437
#define MC_MAN_HDK_CONFIG_CFM				UMI_MSG_MAN_CFM(UMI_MAN_HDK_CONFIG) //0x1437

#define UM_MAN_POLL_CLIENT_REQ 				UMI_MSG_MAN_REQ(UMI_MAN_POLL_CLIENT) // 0x0438
#define MC_MAN_POLL_CLIENT_CFM 				UMI_MSG_MAN_CFM(UMI_MAN_POLL_CLIENT) // 0x1438

#define UM_MAN_MBSS_PRE_ACTIVATE_REQ    	UMI_MSG_MAN_REQ(UMI_MAN_MBSS_PRE_ACTIVATE) // 0x0439
#define MC_MAN_MBSS_PRE_ACTIVATE_CFM    	UMI_MSG_MAN_CFM(UMI_MAN_MBSS_PRE_ACTIVATE) // 0x1439

#define UM_MAN_CALIBRATE_REQ				UMI_MSG_MAN_REQ(UMI_MAN_CALIBRATE) //0x043A
#define MC_MAN_CALIBRATE_CFM				UMI_MSG_MAN_CFM(UMI_MAN_CALIBRATE) //0x143A

#define UM_MAN_DOWNLOAD_PROG_MODEL_20_40_DIFFS_REQ    	UMI_MSG_MAN_REQ(UMI_MAN_DOWNLOAD_PROG_MODEL_20_40_DIFFS) // 0x043B
#define MC_MAN_DOWNLOAD_PROG_MODEL_20_40_DIFFS_CFM    	UMI_MSG_MAN_CFM(UMI_MAN_DOWNLOAD_PROG_MODEL_20_40_DIFFS) // 0x143B

#define UM_MAN_SET_MULTICAST_MODE_REQ    		UMI_MSG_MAN_REQ(UMI_MAN_SET_MULTICAST_MODE) // 0x043C
#define MC_MAN_SET_MULTICAST_MODE_CFM    		UMI_MSG_MAN_CFM(UMI_MAN_SET_MULTICAST_MODE) // 0x143C

#define UM_MAN_MULTICAST_ACTION_REQ				UMI_MSG_MAN_REQ(UMI_MAN_MULTICAST_ACTION) // 0x043D
#define MC_MAN_MULTICAST_ACTION_CFM				UMI_MSG_MAN_CFM(UMI_MAN_MULTICAST_ACTION) // 0x143D

#define UM_MAN_PLATFORM_DATA_FIELDS_REQ    		UMI_MSG_MAN_REQ(UMI_MAN_PLATFORM_DATA_FIELDS) // 0x043E
#define MC_MAN_PLATFORM_DATA_FIELDS_CFM    		UMI_MSG_MAN_CFM(UMI_MAN_PLATFORM_DATA_FIELDS) // 0x143E

#define UM_MAN_PLATFORM_TABLE_REQ    			UMI_MSG_MAN_REQ(UMI_MAN_PLATFORM_TABLE) // 0x043F
#define MC_MAN_PLATFORM_TABLE_CFM    			UMI_MSG_MAN_CFM(UMI_MAN_PLATFORM_TABLE) // 0x143F

#define UM_MAN_SET_AFE_CALIBRATION_DATA_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_AFE_CALIBRATION_DATA) //0x0440
#define MC_MAN_SET_AFE_CALIBRATION_DATA_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_AFE_CALIBRATION_DATA) //0x1440

#define UM_MAN_SET_RFIC_CALIBRATION_DATA_REQ	UMI_MSG_MAN_REQ(UMI_MAN_SET_RFIC_CALIBRATION_DATA) //0x0441
#define MC_MAN_SET_RFIC_CALIBRATION_DATA_CFM	UMI_MSG_MAN_CFM(UMI_MAN_SET_RFIC_CALIBRATION_DATA) //0x1441

#define UM_MAN_SET_RSSI_CAL_PARAMS_REQ			UMI_MSG_MAN_REQ(UMI_MAN_SET_RSSI_CAL_PARAMS)	//0x0442
#define MC_MAN_SET_RSSI_CAL_PARAMS_CFM			UMI_MSG_MAN_CFM(UMI_MAN_SET_RSSI_CAL_PARAMS)	//0x1442

#define UM_MAN_ENABLE_RADIO_REQ					UMI_MSG_MAN_REQ(UMI_MAN_ENABLE_RADIO)	//0x0443
#define MC_MAN_ENABLE_RADIO_CFM					UMI_MSG_MAN_CFM(UMI_MAN_ENABLE_RADIO)	//0x1443

#define UM_MAN_ENABLE_RADAR_INDICATION_REQ		UMI_MSG_MAN_REQ(UMI_MAN_ENABLE_RADAR_INDICATION)	//0x0444
#define MC_MAN_ENABLE_RADAR_INDICATION_CFM		UMI_MSG_MAN_CFM(UMI_MAN_ENABLE_RADAR_INDICATION)	//0x1444

#define UM_MAN_SET_AGG_RATE_LIMIT_REQ			UMI_MSG_MAN_REQ(UMI_MAN_SET_AGG_RATE_LIMIT) //0x0445
#define MC_MAN_SET_AGG_RATE_LIMIT_CFM			UMI_MSG_MAN_CFM(UMI_MAN_SET_AGG_RATE_LIMIT) //0x1445

#define UM_MAN_SET_POWER_LIMIT_REQ				UMI_MSG_MAN_REQ(UMI_MAN_SET_POWER_LIMIT) //0x0446
#define MC_MAN_SET_POWER_LIMIT_CFM				UMI_MSG_MAN_CFM(UMI_MAN_SET_POWER_LIMIT) //0x1446

#define UM_MAN_TEST_BUS_EN_REQ					UMI_MSG_MAN_REQ(UMI_TEST_BUS_ENABLE) // 0x0447
#define MC_MAN_TEST_BUS_EN_CFM					UMI_MSG_MAN_CFM(UMI_TEST_BUS_ENABLE) // 0x1447

#define UM_MAN_SET_RX_TH_REQ			 		UMI_MSG_MAN_REQ(UMI_MAN_SET_RX_TH) //0x0448
#define MC_MAN_SET_RX_TH_CFM					UMI_MSG_MAN_CFM(UMI_MAN_SET_RX_TH) //0x1448

#define UM_MAN_SET_RX_DUTY_CYCLE_REQ	 		UMI_MSG_MAN_REQ(UMI_MAN_SET_RX_DUTY_CYCLE) //0x0449
#define MC_MAN_SET_RX_DUTY_CYCLE_CFM	 		UMI_MSG_MAN_CFM(UMI_MAN_SET_RX_DUTY_CYCLE) //0x1449

#define UM_MAN_SET_ADMISSION_CAPACITY_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_ADMISSION_CAPACITY) //0x044A
#define MC_MAN_SET_ADMISSION_CAPACITY_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_ADMISSION_CAPACITY) //0x144A

#define UM_MAN_CPU_LOAD_REQ     				UMI_MSG_MAN_REQ(UMI_MAN_CPU_LOAD) // 0x044B
#define MC_MAN_CPU_LOAD_CFM     				UMI_MSG_MAN_CFM(UMI_MAN_CPU_LOAD) // 0x144B

#define UM_MAN_TS_VAP_CONFIGURE_REQ     		UMI_MSG_MAN_REQ(UMI_MAN_TS_VAP_CONFIGURATION) // 0x044C
#define MC_MAN_TS_VAP_CONFIGURE_CFM     		UMI_MSG_MAN_CFM(UMI_MAN_TS_VAP_CONFIGURATION) // 0x144C

#define UM_MAN_MSDU_IN_AMSDU_CONFIG_REQ         UMI_MSG_MAN_REQ(UMI_MAN_MSDU_IN_AMSDU_CONFIG) // 0x044D
#define MC_MAN_MSDU_IN_AMSDU_CONFIG_CFM         UMI_MSG_MAN_CFM(UMI_MAN_MSDU_IN_AMSDU_CONFIG) // 0x144D

#define UM_MAN_MU_OPERATION_CONFIG_REQ          UMI_MSG_MAN_REQ(UMI_MAN_MU_OPERATION_CONFIG) // 0x044E
#define MC_MAN_MU_OPERATION_CONFIG_CFM          UMI_MSG_MAN_CFM(UMI_MAN_MU_OPERATION_CONFIG) // 0x144E

#define UM_MAN_CCA_TH_REQ						UMI_MSG_MAN_REQ(UMI_MAN_CCA_TH) // 0x044F
#define MC_MAN_CCA_TH_CFM						UMI_MSG_MAN_CFM(UMI_MAN_CCA_TH) // 0x144F

#define UM_MAN_RADAR_DETECTION_RSSI_TH_CONFIG_REQ	UMI_MSG_MAN_REQ(UMI_MAN_RADAR_DETECTION_RSSI_TH_CONFIG) // 0x0450
#define MC_MAN_RADAR_DETECTION_RSSI_TH_CONFIG_CFM 	UMI_MSG_MAN_CFM(UMI_MAN_RADAR_DETECTION_RSSI_TH_CONFIG) // 0x1450

#define UM_MAN_RTS_MODE_CONFIG_REQ              UMI_MSG_MAN_REQ(UMI_MAN_RTS_MODE_CONFIG) // 0x0451
#define UM_MAN_RTS_MODE_CONFIG_CFM              UMI_MSG_MAN_CFM(UMI_MAN_RTS_MODE_CONFIG) // 0x1451

#define UM_MAN_MAX_MPDU_LENGTH_REQ              UMI_MSG_MAN_REQ(UMI_MAN_MAX_MPDU_LENGTH) // 0x0452
#define MC_MAN_MAX_MPDU_LENGTH_CFM              UMI_MSG_MAN_CFM(UMI_MAN_MAX_MPDU_LENGTH) // 0x1452

#define UM_MAN_BF_MODE_CONFIG_REQ               UMI_MSG_MAN_REQ(UMI_MAN_BF_MODE_CONFIG) // 0x0453
#define MC_MAN_BF_MODE_CONFIG_CFM               UMI_MSG_MAN_CFM(UMI_MAN_BF_MODE_CONFIG) // 0x1453

#define UM_MAN_SET_IRE_SWITCH_B_REQ             UMI_MSG_MAN_REQ(UMI_MAN_IRE_CONTROL_B) // 0x0454
#define MC_MAN_SET_IRE_SWITCH_B_CFM             UMI_MSG_MAN_CFM(UMI_MAN_IRE_CONTROL_B) // 0x1454

#define UM_MAN_FIXED_RATE_CONFIG_REQ            UMI_MSG_MAN_REQ(UMI_MAN_FIXED_RATE_CONFIG) // 0x0455
#define MC_MAN_FIXED_RATE_CONFIG_CFM            UMI_MSG_MAN_CFM(UMI_MAN_FIXED_RATE_CONFIG) // 0x1455

#define UM_MAN_SSB_MODE_REQ                     UMI_MSG_MAN_REQ(UMI_MAN_SSB_MODE) //0x0456
#define MC_MAN_SSB_MODE_CFM                     UMI_MSG_MAN_CFM(UMI_MAN_SSB_MODE) //0x1456

#define MC_MAN_BEACON_BLOCKED_IND               UMI_MSG_MAN_IND(UMI_MAN_BEACON_BLOCKED) // 0x3357
#define UM_MAN_BEACON_BLOCKED_RES               UMI_MSG_MAN_RES(UMI_MAN_BEACON_BLOCKED) // 0x2357

#define UM_MAN_BEACON_BLOCKED_INTERVAL_REQ      UMI_MSG_MAN_REQ(UMI_MAN_BEACON_BLOCKED_INTERVAL) // 0x0458
#define MC_MAN_BEACON_BLOCKED_INTERVAL_CFM      UMI_MSG_MAN_CFM(UMI_MAN_BEACON_BLOCKED_INTERVAL) // 0x1458

#define UM_MAN_GET_CCA_TH_REQ                   UMI_MSG_MAN_REQ(UMI_MAN_GET_CCA_TH) // 0x0459
#define MC_MAN_GET_CCA_TH_CFM                   UMI_MSG_MAN_CFM(UMI_MAN_GET_CCA_TH) // 0x1459

#define UM_MAN_FREQ_JUMP_MODE_REQ               UMI_MSG_MAN_REQ(UMI_MAN_FREQ_JUMP_MODE)	//0x045A
#define MC_MAN_FREQ_JUMP_MODE_CFM               UMI_MSG_MAN_CFM(UMI_MAN_FREQ_JUMP_MODE)	//0x145A

#define UM_MAN_SLOW_PROBING_MASK_REQ            UMI_MSG_MAN_REQ(UMI_MAN_SLOW_PROBING_MASK) // 0x045B
#define MC_MAN_SLOW_PROBING_MASK_CFM            UMI_MSG_MAN_CFM(UMI_MAN_SLOW_PROBING_MASK) // 0x145B

#define UM_MAN_SET_TXOP_CONFIG_REQ              UMI_MSG_MAN_REQ(UMI_MAN_SET_TXOP_CONFIG) // 0x045C
#define MC_MAN_SET_TXOP_CONFIG_CFM              UMI_MSG_MAN_CFM(UMI_MAN_SET_TXOP_CONFIG) // 0x145C

#define UM_MAN_SET_BEACON_INTERVAL_REQ          UMI_MSG_MAN_REQ(UM_MAN_SET_BEACON_INTERVAL) // 0x045D
#define MC_MAN_SET_BEACON_INTERVAL_CFM          UMI_MSG_MAN_CFM(UM_MAN_SET_BEACON_INTERVAL) // 0x145D

#define UM_MAN_2_4GHZ_COEX_REQ					UMI_MSG_MAN_REQ(UMI_2_4GHZ_COEX) //0x045E
#define MC_MAN_2_4GHZ_COEX_CFM					UMI_MSG_MAN_CFM(UMI_2_4GHZ_COEX) //0x145E

#define UM_MAN_ATF_QUOTAS_REQ					UMI_MSG_MAN_REQ(UMI_MAN_ATF_QUOTAS) // 0x045F
#define UM_MAN_ATF_QUOTAS_CFM					UMI_MSG_MAN_CFM(UMI_MAN_ATF_QUOTAS) // 0x145F

#define UM_MAN_STATIC_PLAN_CONFIG_REQ 			UMI_MSG_MAN_REQ(UMI_MAN_STATIC_PLAN_CONFIG) // 0x0460
#define MC_MAN_STATIC_PLAN_CONFIG_CFM			UMI_MSG_MAN_CFM(UMI_MAN_STATIC_PLAN_CONFIG) // 0x1460

#define UM_MAN_FIXED_LTF_AND_GI_REQ				UMI_MSG_MAN_REQ(UMI_MAN_FIXED_LTF_AND_GI) // 0x0461
#define	MC_MAN_FIXED_LTF_AND_GI_CFM				UMI_MSG_MAN_CFM(UMI_MAN_FIXED_LTF_AND_GI) // 0x1461

#define UM_MAN_FIXED_POWER_REQ  				UMI_MSG_MAN_REQ(UMI_MAN_FIXED_POWER) // 0x0462
#define MC_MAN_FIXED_POWER_CFM  				UMI_MSG_MAN_CFM(UMI_MAN_FIXED_POWER) // 0x1462

#define UM_MAN_OPERATING_MODE_REQ  				UMI_MSG_MAN_REQ(UMI_MAN_OPERATING_MODE) // 0x0463
#define MC_MAN_OPERATING_MODE_CFM  				UMI_MSG_MAN_CFM(UMI_MAN_OPERATING_MODE) // 0x1463

#define UM_MAN_HDK_ANT_CONFIG_REQ 				UMI_MSG_MAN_REQ(UMI_MAN_HDK_SET_ANT_CONFIG) // 0x0464
#define MC_MAN_HDK_ANT_CONFIG_CFM				UMI_MSG_MAN_CFM(UMI_MAN_HDK_SET_ANT_CONFIG) // 0x1464

#define UM_MAN_HDK_PREPARE_ANT_CONFIG_CHANGE_REQ UMI_MSG_MAN_REQ(UMI_MAN_HDK_PREPARE_ANT_CONFIG) // 0x0465
#define MC_MAN_HDK_PREPARE_ANT_CONFIG_CHANGE_CFM UMI_MSG_MAN_CFM(UMI_MAN_HDK_PREPARE_ANT_CONFIG) // 0x1465

#define UM_MAN_DOWNLOAD_PROG_MODEL_ANT_DEPENDENT_TX_REQ    	UMI_MSG_MAN_REQ(UMI_MAN_HDK_DOWNLOAD_PROGMODEL_ANT_DEPENDENT_TX) // 0x0466
#define MC_MAN_DOWNLOAD_PROG_MODEL_ANT_DEPENDENT_TX_CFM    	UMI_MSG_MAN_CFM(UMI_MAN_HDK_DOWNLOAD_PROGMODEL_ANT_DEPENDENT_TX) // 0x1466

#define UM_MAN_DOWNLOAD_PROG_MODEL_ANT_DEPENDENT_RX_REQ    	UMI_MSG_MAN_REQ(UMI_MAN_HDK_DOWNLOAD_PROGMODEL_ANT_DEPENDENT_RX) // 0x0467
#define MC_MAN_DOWNLOAD_PROG_MODEL_ANT_DEPENDENT_RX_CFM    	UMI_MSG_MAN_CFM(UMI_MAN_HDK_DOWNLOAD_PROGMODEL_ANT_DEPENDENT_RX) // 0x1467

#define UM_MAN_QOS_PD_THRESH_REQ                    UMI_MSG_MAN_REQ(UMI_MAN_SET_QOS_PD_THRESH) // 0x0468
#define MC_MAN_QOS_PD_THRESH_CFM                    UMI_MSG_MAN_CFM(UMI_MAN_SET_QOS_PD_THRESH) // 0x1468

#define UM_MAN_RESTRICTED_AC_MODE_REQ               UMI_MSG_MAN_REQ(UMI_MAN_SET_RESTRICTED_AC_MODE) // 0x0469
#define MC_MAN_RESTRICTED_AC_MODE_CFM               UMI_MSG_MAN_CFM(UMI_MAN_SET_RESTRICTED_AC_MODE) // 0x1469

#define UM_MAN_FAST_DROP_CONFIG_REQ                 UMI_MSG_MAN_REQ(UMI_MAN_FAST_DROP_CONFIG) // 0x046A
#define MC_MAN_FAST_DROP_CONFIG_CFM                 UMI_MSG_MAN_CFM(UMI_MAN_FAST_DROP_CONFIG) // 0x146A

#define UM_MAN_ERP_SET_REQ							UMI_MSG_MAN_REQ(UMI_MAN_ERP_SET) // 0x046B
#define MC_MAN_ERP_SET_CFM							UMI_MSG_MAN_CFM(UMI_MAN_ERP_SET) // 0x146B

#define UM_MAN_DMR_CONFIG_REQ						UMI_MSG_MAN_REQ(UMI_MAN_DMR_CONFIG) // 0x046C
#define MC_MAN_DMR_CONFIG_CFM  						UMI_MSG_MAN_CFM(UMI_MAN_DMR_CONFIG) // 0x146C

#define MC_MAN_HE_MU_DBG_IND						UMI_MSG_MAN_IND(UMI_MAN_HE_MU_DBG_IND) // 0x336D
#define UM_MAN_HE_MU_DBG_RES						UMI_MSG_MAN_RES(UMI_MAN_HE_MU_DBG_IND) // 0x236D

#define UM_MAN_SET_REG_DOMAIN_CONFIG_REQ			UMI_MSG_MAN_REQ(UMI_MAN_SET_REG_DOMAIN_CONFIG) // 0x046E
#define MC_MAN_SET_REG_DOMAIN_CONFIG_CFM			UMI_MSG_MAN_CFM(UMI_MAN_SET_REG_DOMAIN_CONFIG) // 0x146E

#define UM_MAN_RTS_RATE_SET_REQ						UMI_MSG_MAN_REQ(UMI_MAN_RTS_RATE_SET) //0x046F
#define MC_MAN_RTS_RATE_SET_CFM						UMI_MSG_MAN_CFM(UMI_MAN_RTS_RATE_SET) //0x146F

#define UM_MAN_NFRP_CONFIG_REQ						UMI_MSG_MAN_REQ(UMI_MAN_NFRP_CONFIG) //0x0470
#define UM_MAN_NFRP_CONFIG_CFM						UMI_MSG_MAN_CFM(UMI_MAN_NFRP_CONFIG) //0x1470 

#define UM_MAN_DUTY_CYCLE_SET_REQ					UMI_MSG_MAN_REQ(UMI_MAN_DUTY_CYCLE_SET) //0x0471
#define MC_MAN_DUTY_CYCLE_SET_CFM					UMI_MSG_MAN_CFM(UMI_MAN_DUTY_CYCLE_SET)  //0x1471

#define UM_MAN_SET_DYNAMIC_MU_TYPE_REQ				UMI_MSG_MAN_REQ(UMI_MAN_SET_DYNAMIC_MU_TYPE) //0x0472
#define UM_MAN_SET_DYNAMIC_MU_TYPE_CFM				UMI_MSG_MAN_CFM(UMI_MAN_SET_DYNAMIC_MU_TYPE) //0x1472

#define UM_MAN_SET_HE_MU_FIXED_PARAMETERS_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_HE_MU_FIXED_PARAMETERS) //0x0473
#define UM_MAN_SET_HE_MU_FIXED_PARAMETERS_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_HE_MU_FIXED_PARAMETERS) //0x1473

#define UM_MAN_SET_HE_MU_DURATION_REQ				UMI_MSG_MAN_REQ(UMI_MAN_SET_HE_MU_DURATION) //0x0474
#define UM_MAN_SET_HE_MU_DURATION_CFM				UMI_MSG_MAN_CFM(UMI_MAN_SET_HE_MU_DURATION) //0x1474

#define UM_MAN_GET_TWT_PARAMS_REQ					UMI_MSG_MAN_REQ(UMI_MAN_GET_TWT_PARAMS) //0x0475
#define UM_MAN_GET_TWT_PARAMS_CFM					UMI_MSG_MAN_CFM(UMI_MAN_GET_TWT_PARAMS) //0x1475

#define UM_MAN_GET_AX_DEFAULT_PARAMS_REQ			UMI_MSG_MAN_REQ(UMI_MAN_GET_AX_DEFAULT_PARAMS) //0x0476
#define UM_MAN_GET_AX_DEFAULT_PARAMS_CFM			UMI_MSG_MAN_CFM(UMI_MAN_GET_AX_DEFAULT_PARAMS) //0x1476

#define UM_MAN_SET_ETSI_PPDU_DURATION_LIMITS_REQ	UMI_MSG_MAN_REQ(UMI_MAN_SET_ETSI_PPDU_DURATION_LIMITS) //Ox0477
#define UM_MAN_SET_ETSI_PPDU_DURATION_LIMITS_CFM	UMI_MSG_MAN_CFM(UMI_MAN_SET_ETSI_PPDU_DURATION_LIMITS) //Ox1477

#define UM_MAN_SET_RTS_THRESHOLD_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_RTS_THRESHOLD) //0x0478
#define MC_MAN_SET_RTS_THRESHOLD_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_RTS_THRESHOLD) //0x1478

#define UM_MAN_SET_RETRY_LIMIT_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_RETRY_LIMIT) //0x0479
#define MC_MAN_SET_RETRY_LIMIT_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_RETRY_LIMIT) //0x1479

#define UM_MAN_PIE_REQ						UMI_MSG_MAN_REQ(UMI_MAN_PIE) // 0x047A
#define MC_MAN_PIE_CFM						UMI_MSG_MAN_CFM(UMI_MAN_PIE) // 0x147A

#define UM_MAN_SET_AP_BEACON_INFO_REQ		UMI_MSG_MAN_REQ(UMI_MAN_SET_AP_BEACON_INFO) //0x047B
#define MC_MAN_SET_AP_BEACON_INFO_CFM		UMI_MSG_MAN_CFM(UMI_MAN_SET_AP_BEACON_INFO) //0x147B

#define UM_MAN_SET_UNSOLICITED_REQ			UMI_MSG_MAN_REQ(UMI_MAN_SET_UNSOLICITED)	//0x047C
#define MC_MAN_SET_UNSOLICITED_CFM			UMI_MSG_MAN_CFM(UMI_MAN_SET_UNSOLICITED)	//0x147C

#define UM_MAN_MU_OFDMA_BF_CONFIG_REQ 		UMI_MSG_MAN_REQ(UMI_MAN_MU_OFDMA_BF_CONFIG_REQ)	//0x047D
#define MC_MAN_MU_OFDMA_BF_CONFIG_CFM		UMI_MSG_MAN_CFM(UMI_MAN_MU_OFDMA_BF_CONFIG_REQ)	//0x147D

#define MC_MAN_EXT_BLOCKED_ACCESS_IND    	UMI_MSG_MAN_IND(UMI_MAN_EXT_BLOCKED_ACCESS) // 0x337E
#define UM_MAN_EXT_BLOCKED_ACCESS_RES    	UMI_MSG_MAN_RES(UMI_MAN_EXT_BLOCKED_ACCESS) // 0x237E

#define UM_MAN_PREAMBLE_PUNCTURE_CCA_OVERRIDE_REQ 	UMI_MSG_MAN_REQ(UMI_MAN_PREAMBLE_PUNCTURE_CCA_OVERRIDE) // 0x047F
#define MC_MAN_PREAMBLE_PUNCTURE_CCA_OVERRIDE_CFM 	UMI_MSG_MAN_CFM(UMI_MAN_PREAMBLE_PUNCTURE_CCA_OVERRIDE) // 0x147F

#define UM_MAN_ADVERTISE_BC_TWT_SP_REQ 		UMI_MSG_MAN_REQ(UMI_MAN_ADVERTISE_BC_TWT_SP) // 0x0480
#define MC_MAN_ADVERTISE_BC_TWT_SP_CFM 		UMI_MSG_MAN_CFM(UMI_MAN_ADVERTISE_BC_TWT_SP) // 0x1480

#define UM_MAN_TERMINATE_BC_TWT_SP_REQ 		UMI_MSG_MAN_REQ(UMI_MAN_TERMINATE_BC_TWT_SP) // 0x0481
#define MC_MAN_TERMINATE_BC_TWT_SP_CFM 		UMI_MSG_MAN_CFM(UMI_MAN_TERMINATE_BC_TWT_SP) // 0x1481

#define UM_MAN_SEND_TWT_TEARDOWN_FRAME_REQ 	UMI_MSG_MAN_REQ(UMI_MAN_SEND_TWT_TEARDOWN_FRAME) // 0x0482
#define MC_MAN_SEND_TWT_TEARDOWN_FRAME_CFM 	UMI_MSG_MAN_CFM(UMI_MAN_SEND_TWT_TEARDOWN_FRAME) // 0x1482

#define UM_MAN_ENABLE_WHM_REQ			 	UMI_MSG_MAN_REQ(UMI_MAN_ENABLE_WHM) // 0x0483
#define MC_MAN_ENABLE_WHM_CFM				UMI_MSG_MAN_CFM(UMI_MAN_ENABLE_WHM) // 0x1483

#define UM_MAN_ENABLE_HW_LOGGER_REQ			UMI_MSG_MAN_REQ(UMI_MAN_ENABLE_HW_LOGGER) // 0x0484
#define MC_MAN_ENABLE_HW_LOGGER_CFM			UMI_MSG_MAN_CFM(UMI_MAN_ENABLE_HW_LOGGER) // 0x1484

#define MC_MAN_WARNING_DETECTED_IND			UMI_MSG_MAN_IND(UMI_MAN_WARNING_DETECTED) // 0x3385
#define UM_MAN_WARNING_DETECTED_RES			UMI_MSG_MAN_RES(UMI_MAN_WARNING_DETECTED) // 0x3285

#define MC_MAN_STA_EXCESSIVE_RETRIES_IND	UMI_MSG_MAN_IND(UMI_MAN_STA_EXCESSIVE_RETRIES_DETECTED) // 0x3386
#define UM_MAN_STA_EXCESSIVE_RETRIES_RES	UMI_MSG_MAN_RES(UMI_MAN_STA_EXCESSIVE_RETRIES_DETECTED) // 0x3286

#define UM_MAN_SET_EXCESSIVE_RETRIES_LIMIT_REQ	UMI_MSG_MAN_REQ(UMI_MAN_SET_EXCESSIVE_RETRIES_LIMIT)
#define MC_MAN_SET_EXCESSIVE_RETRIES_LIMIT_CFM	UMI_MSG_MAN_CFM(UMI_MAN_SET_EXCESSIVE_RETRIES_LIMIT)

/***************************************************************************/
/***                          Management Messages                        ***/
/***************************************************************************/

#define API_SET_OPERATION	0
#define API_GET_OPERATION	1

/***************************************************************************
**
** NAME         UMI_SET_PD_THRESH
**
** PARAMETERS   	uint16 minPdDiff;
**					uint8  mode;
**					uint8  minPdAmount;
**
** DESCRIPTION: This message should be sent to request a change restrictedAcMode
**
****************************************************************************/

#ifdef CPU_ARC
#define  QOS_PD_THRESH_PD_DIFF_MIN				(300)
#define  QOS_PD_THRESH_PD_DIFF_MAX				(65535)

#define  QOS_PD_THRESH_MODE_MIN					(0)
#define  QOS_PD_THRESH_MODE_MAX					(2)

#define  QOS_PD_THRESH_MIN_PD_AMT_MIN			(0)
#define  QOS_PD_THRESH_MIN_PD_AMT_MAX			(255)

#define QOS_PD_THRESHOLD_MIN_AMOUNT_DEFAULT	(0)
#define QOS_PD_THRESHOLD_MIN_DIFF_DEFAULT	(300)
#endif //CPU_ARC

typedef enum
{
	QOS_PD_THRESHOLD_DISABLED,
	QOS_PD_THRESHOLD_DYNAMIC,
	QOS_PD_THRESHOLD_FORCED,
	QOS_PD_THRESHOLD_NUM_MODES
} QoSPdThresholdMode_e;


typedef struct _UMI_SET_PD_THRESH
{
	uint16				 			minPdDiff;		/*Minimum allowed Difference between Minimum and maximum PDs on queues with same AC in Dynamic Mode*/
	uint8  /*QoSPdThresholdMode_e*/	mode;
	uint8				 			minPdAmount;	/*Minimum Amount of PDs on a queues needed so a queue is taken into considerartion in Dynamic Mode*/
	uint8							getSetOperation;
	uint8							Status;
	uint8							reserved[2];
} __MTLK_PACKED UMI_SET_PD_THRESH;

/***************************************************************************
**
** NAME         UMI_SET_RESTRICTED_AC
**
** PARAMETERS   	uint8  restrictedAcModeEnable;
**					uint8  acRestrictedBitmap;
**					uint16 restrictedAcThreshEnter;
**					uint16 restrictedAcThreshExit;
**
** DESCRIPTION: This message should be sent to request a change restrictedAcMode
**
****************************************************************************/
#ifdef CPU_ARC
#define RES_AC_THR_ENTER_MIN 0
#define RES_AC_THR_ENTER_MAX 8196

#define RES_AC_THR_EXIT_MIN 0
#define RES_AC_THR_EXIT_MAX 8196

#define RES_AC_BITMAP_MIN 0
#define RES_AC_BITMAP_MAX 0xF
#endif //CPU_ARC


typedef struct _UMI_SET_RESTRICTED_AC
{
	uint16 restrictedAcThreshEnter; // uint16 value for amount of PDs that should be free in order to allow free allocation to TX queue, when below this threhold restrictedMode will be enabled
	uint16 restrictedAcThreshExit;	// uint16 value for amount of PDs that should be free in order to allow exit from restricted AC mode
	uint8  restrictedAcModeEnable; 	// options: {FALSE: disabled, TRUE: enabled}
	uint8  acRestrictedBitmap;		// bitmap per AC to be given priority when restricted mode is enabled: bit 0-BE; bit 1-BK; bit 2-VI; bit 3-VO
	uint8  getSetOperation;
	uint8  Status;
} __MTLK_PACKED UMI_SET_RESTRICTED_AC;

/***************************************************************************
**
** NAME         UMI_SET_ANTENNAS
**
** PARAMETERS   	uint8 TxAntsMask;
**				uint8 RxAntsMask;
**
** DESCRIPTION: This message should be sent to request a change in active antennas 
**
****************************************************************************/
#ifdef CPU_ARC
#define ANT_PARAMS_TX_MASK_MIN			(0)
#define ANT_PARAMS_TX_MASK_MAX			(0xFF)

#define ANT_PARAMS_RX_MASK_MIN			(0)
#define ANT_PARAMS_RX_MASK_MAX			(0xFF)
#endif //CPU_ARC


typedef struct
{
	uint8 TxAntsMask;
	uint8 RxAntsMask;
	uint8 status;
	uint8 getSetOperation;
} __MTLK_PACKED UMI_SET_ANTENNAS;

/***************************************************************************
**
** NAME         UM_MAN_ACTIVATE_REQ
**
** PARAMETERS   sBSSID              The ID which identifies the Network to
**                                  be created or connected to. If the node
**                                  is a Infrastructure Station and a null
**                                  MAC Address is specified then the
**                                  request is interpreted to mean join any
**                                  suitable network.
**              sSSID               The Service Set Identifier of the ESS
**              sRSNie              RSN Information Element
**
** DESCRIPTION  Activate Request. This request should be sent to the Upper
**              MAC to start or connect to a network.
**
*****************************************************************************/
#define UMI_SC_BAND_MAX_LEN 32

/* RSN Information Element */
typedef struct _UMI_RSN_IE
{
    uint8   au8RsnIe[MTLK_PAD4(UMI_RSN_IE_MAX_LEN)];
} __MTLK_PACKED UMI_RSN_IE;

typedef struct _UMI_SUPPORTED_CHANNELS_IE
{
    uint8 asSBand[MTLK_PAD4(UMI_SC_BAND_MAX_LEN*2)]; // even bytes = u8FirstChannelNumber (0,2,4,...)
                                                     // odd bytes  = u8NumberOfChannels   (1,3,5,...)
} __MTLK_PACKED UMI_SUPPORTED_CHANNELS_IE;


typedef struct _UMI_MBSS_PRE_ACTIVATE_HDR
{
	uint16     u16Status; /* FW returns operation result here */
	uint8       u8_CoexistenceEnabled;
	uint8       u8_40mhzIntolerant;
} __MTLK_PACKED UMI_MBSS_PRE_ACTIVATE_HDR;


#ifdef CPU_ARC

#define AC_WMM_PARAM_CWMAX_MIN				(0)
#define AC_WMM_PARAM_CWMAX_MAX				(32767)

#define AC_WMM_PARAM_CWMIN_MIN				(0)
#define AC_WMM_PARAM_CWMIN_MAX				(32767)

#define AC_WMM_PARAM_TXOPLIMIT_MIN			(0)
#define AC_WMM_PARAM_TXOPLIMIT_MAX			(32767)

#define AC_WMM_PARAM_AIFSN_MIN				(1)
#define AC_WMM_PARAM_AIFSN_MAX				(15)

#define AC_WMM_PARAM_ACM_FLAG_MIN			(0)
#define AC_WMM_PARAM_ACM_FLAG_MAX			(1)
#endif //CPU_ARC

typedef struct _UMI_AC_WMM_PARAMS
{
	uint16	u16CWmin;
	uint16	u16CWmax;
	uint16	u16TXOPlimit;
	uint8	u8Aifsn;
	uint8	acm_flag;
} __MTLK_PACKED UMI_AC_WMM_PARAMS;



/***************************************************************************
**
** NAME         UMI_ADD_VAP
**
** PARAMETERS   BSSID		: add VAP BSS ID
**				status		: confirmation status
**
** DESCRIPTION: add a VAP structure  from host
**              and confirms from MAC
**
****************************************************************************/
#ifdef CPU_ARC
#define VAP_ADD_OPER_MODE_MIN		(0)
#define VAP_ADD_OPER_MODE_MAX		(5)
#endif //CPU_ARC

typedef struct _UMI_ADD_VAP
{
  IEEE_ADDR     sBSSID;
  uint8			vapId;
  uint8			operationMode; //use values from UmiOperationMode_e enum

  /* RATE info */ 
  uint8 	u8Rates[MAX_NUM_SUPPORTED_RATES]; //supported rates
  uint8 	u8Rates_Length; //length of the rates array
  uint8 	u8TX_MCS_Bitmask[10]; //instead of u8RX_MCS_Bitmask, Corresponds to "Supported MCS set field" in IE
  uint8 	u8VHT_Mcs_Nss[8]; // Correspond to "8.4.2.160.3 Supported VHT-MCS and NSS Set 
  uint8 	u8HE_Mcs_Nss[8]; // Supported HE-MCS and NSS Set 
  uint8     Status;
} __MTLK_PACKED UMI_ADD_VAP;


/***************************************************************************
**
** NAME         UMI_STOP_VAP_TRAFFIC
**
** PARAMETERS    vapId		: The corespond Vap identification number  
**				status		: confirmation status
**
** DESCRIPTION: stop traffic of a given vap, preliminary action to remove vap 
**
****************************************************************************/

typedef struct _UMI_STOP_VAP_TRAFFIC
{
  uint16        u16Status; 
  uint8	     	vapId;
  uint8         Reserved;
} __MTLK_PACKED UMI_STOP_VAP_TRAFFIC;


/***************************************************************************
**
** NAME         UMI_REMOVE_VAP
**
** PARAMETERS   BSSID		: The corespond Vap identification number  
**				status		: confirmation status
**
** DESCRIPTION: remove VAP structure for remove VAP requests from host
**              and confirms from MAC
**
****************************************************************************/
typedef struct _UMI_REMOVE_VAP
{
  uint16        u16Status; /* FW returns operation result here */
  uint8			vapId;
  uint8         Reserved;
} __MTLK_PACKED UMI_REMOVE_VAP;


/***************************************************************************
**
** NAME         UMI_SET_BSS
**
** PARAMETERS   
**				
**				vapId		
**				isShortPreamble boolian 
**				protectionMode	boolian
**				isShortSlotTime	boolian
**
** DESCRIPTION: remove VAP structure for remove VAP requests from host
**              and confirms from MAC
**
****************************************************************************/
#ifdef CPU_ARC
#define BSS_SET_BEACON_INTVL_MIN			(0)
#define BSS_SET_BEACON_INTVL_MAX			(65535)


#define BSS_SET_DTIM_INTVL_MIN				(0)
#define BSS_SET_DTIM_INTVL_MAX				(65535)

#define BSS_SET_PROT_MOD_MIN				(0)
#define BSS_SET_PROT_MOD_MAX				(4)

#define BSS_SET_SLOT_TIME_MIN				(0)
#define BSS_SET_SLOT_TIME_MAX				(1)

#define BSS_SET_TWT_OP_MOD_MIN				(0)
#define BSS_SET_TWT_OP_MOD_MAX				(1)

#define BSS_SET_MBSS_NUM_VAPS_IN_GRP_MIN	(0)
#define BSS_SET_MBSS_NUM_VAPS_IN_GRP_MAX	(32)


#define BSS_SET_FIXED_MCS_VAPS_MGMT_MIN		(0)
#define BSS_SET_FIXED_MCS_VAPS_MGMT_MAX		(0xFF)
#endif //CPU_ARC


#define PROTECTION_MODE_NO_PROTECTION			(0x0)
#define PROTECTION_MODE_NO_FORCE_PROTECTION		(0x1)
#define PROTECTION_MODE_FORCE_RTS_CTS 			(0x2)
#define PROTECTION_MODE_FORCE_CTS_TO_SELF 		(0x3)
#define PROTECTION_MODE_CTS_TO_SELF_ENABLE 		(0x4)
#define FIXED_MCS_VAP_MANAGEMENT_IS_NOT_VALID 	(0xFF)

#define BEACON_PHY_MODE_DEFAULT					(0x0)
#define BEACON_PHY_MODE_VHT						(0x1)
#define BEACON_PHY_MODE_HE						(0x2)


typedef struct _UMI_SET_BSS
{
 	uint8	vapId; 
 	uint8	protectionMode;
 	uint8	slotTime;  
  	/* RATE info */ 
  	uint8 	u8Rates[MAX_NUM_SUPPORTED_RATES]; 	//supported rates
  	uint8 	u8Rates_Length; 					//length of the rates array
  	uint8 	u8TX_MCS_Bitmask[10]; 				//instead of u8RX_MCS_Bitmask, Corresponds to "Supported MCS set field" in IE
  	uint8 	u8VHT_Mcs_Nss[8]; 					// Correspond to "8.4.2.160.3 Supported VHT-MCS and NSS Set field
  	uint8 	u8HE_Mcs_Nss[8]; 					// Supported HE-MCS and NSS Set
  	uint8 	u8HE_Bss_Color; 					// HE BSS Color
  	uint8	flags;		 						//from SET_BSS flags definitions
  	/*If 11B station connect to VAP always transmit with long prembale and clear this bit*/
  	uint8	useShortPreamble;
	uint8	twtOperationMode; 					// indicates whether Individual TWT is supported for this VAP
	uint8	twtBroadcastOperationMode;			// indicates whether Broadcast TWT is supported for this VAP
	uint8	fixedMcsVapManagement; 				//the required MCS for management frames - 0xFF means its not valid.
	uint8 	beaconPhyMode;						//BEACON_PHY_MODE_DEFAULT to use default calculated by FW, else using one of the above defines for BEACON_PHY_MODE
	uint8   Status;
	uint8	Reserved[2];
} __MTLK_PACKED UMI_SET_BSS;



typedef struct _UMI_SET_AP_BEACON_INFO
{
        uint16  beaconInterval;
        uint16  dtimInterval;
        uint8   vapId;
		uint8	mbssIdNumOfVapsInGroup;
		uint8   Status;
		uint8   Reserved;
}__MTLK_PACKED UMI_SET_AP_BEACON_INFO;

#define PROBE_RES_FRAME 		0
#define FILS_DISCOVERY_FRAME 	1
#define UNSOLICITED_MAX_SUPPORTED_FRAMES 2 

typedef struct _UMI_UNSOLICITED_SET
{
	uint32 hostBufferAddress;
	uint16 hostBufferSize; 
	uint16 txIntervalInTU;
	uint8  filsTimestampOffset;
	uint8  isEnabled;
	uint8  frameType;
	uint8  Status;
}__MTLK_PACKED UMI_UNSOLICITED_SET;


#define PHY_RX_STATUS_PHY_RATE_VALUE        MTLK_BFIELD_INFO(4, 17) /* Bits 4...20 */

typedef struct _UMI_PHY_STATUS_REQ
{
	uint32 ddrBufferAddress; 
	uint32 ddrBufferSize; 
} __MTLK_PACKED UMI_PHY_STATUS_REQ;

typedef struct _UMI_GET_CHANNEL_LOAD_REQ
{
	uint32 channelLoad;
	//Output param.. no status
} __MTLK_PACKED UMI_GET_CHANNEL_LOAD_REQ;

/***************************************************************************
**
** NAME        UM_MAN_PLATFORM_DATA_FIELDS_REQ
**
** PARAMETERS   dataFields    
**
** DESCRIPTION  download PSD data fields
**
****************************************************************************/

#define NUM_PLATFORM_DATA_FIELDS  64 

typedef struct _PLATFORM_DATA_FIELDS
{
	uint32 dataFields[NUM_PLATFORM_DATA_FIELDS];
	uint8 Status;
	uint8 Reserved[3];
} __MTLK_PACKED PLATFORM_DATA_FIELDS;

/***************************************************************************
**
** NAME        UM_MAN_PLATFORM_TABLE_REQ
**
** PARAMETERS   tableID    
**
** DESCRIPTION  download PSD table
**
****************************************************************************/
//Note : This is considered after shift with PSD_TABLE_ID_SHIFT.

#ifdef CPU_ARC
#define TABLE_ID_MIN  0x1
#define TABLE_ID_MAX  0x4F
#endif //CPU_ARC

typedef struct _PLATFORM_TABLE
{
	uint32 tableID;
	uint8 Status;
	uint8 Reserved[3];
} __MTLK_PACKED PLATFORM_TABLE;

/***************************************************************************
**                     SECURITY MESSAGES BEGIN                            **
***************************************************************************/

/***************************************************************************
**
** NAME         UM_MAN_SET_KEY_REQ
**
** PARAMETERS   u16Status           UMI_OK
**                                  UMI_NOT_INITIALISED
**                                  UMI_STATION_UNKNOWN
**              u16KeyType          Pairwise or group key
**              sStationID          MAC address of station
**              u16StationRole      Authenticator or supplicant
**              u16CipherSuite      Cipher suite selector
**              u16DefaultKeyIndex  For legacy WEP modes
**              au8RxSeqNum         Initial RX sequence number (little endian)
**              au8TxSeqNum         Initial TX sequence number (little endian)
**              au8Tk1              Temporal key 1
**              au8Tk2              Temporal key 2
**
** DESCRIPTION  Sets the temporal encryption key for the specified station
**
****************************************************************************/
#ifdef CPU_ARC
#define KEY_TYPE_MIN  0x0
#define KEY_TYPE_MAX  0x5

#define KEY_CIPHER_SUITE_MIN  SECURITY_TYPE_SECD_TKIP
#ifdef ENET_INC_ARCH_WAVE600D2
#define KEY_CIPHER_SUITE_MAX  SECURITY_TYPE_SECD_BIP_GMAC256
#else
#define KEY_CIPHER_SUITE_MAX  SECURITY_TYPE_SECD_GCMP256
#endif
#endif //CPU_ARC

#define DO_NOT_CHANGE_PN_SIGNATURE_0_3		(0xFFFFFFFF)
#define DO_NOT_CHANGE_PN_SIGNATURE_4_5		(0xFFFF)

typedef struct _UMI_SET_KEY
{
	uint16      u16Status;
	uint16		u16Sid;
	uint16      u16KeyType;
	uint16      u16CipherSuite;
	uint16      u16KeyIndex;
	uint8		reserved[2];
	uint8       au8RxSeqNum[MTLK_PAD4(UMI_RSN_SEQ_NUM_LEN)];
	uint8       au8TxSeqNum[MTLK_PAD4(UMI_RSN_SEQ_NUM_LEN)];
	union 
	{
		uint8     au8Tk[MTLK_PAD4(UMI_RSN_TK1_LEN) + MTLK_PAD4(UMI_RSN_TK2_LEN)];
		struct 
		{
			uint8   au8Tk1[MTLK_PAD4(UMI_RSN_TK1_LEN)];
			uint8   au8Tk2[MTLK_PAD4(UMI_RSN_TK2_LEN)];
		};
	};
} __MTLK_PACKED UMI_SET_KEY;




/***************************************************************************
**
** NAME         UM_MAN_CLEAR_KEY_REQ
**
** PARAMETERS   u16Status           UMI_OK
**                                  UMI_NOT_INITIALISED
**                                  UMI_STATION_UNKNOWN
**              u16KeyType          Pairwise or group key
**              sStationID          MAC address of station
**
** DESCRIPTION  Clears the temporal encryption key for the specified station
**
****************************************************************************/
typedef struct _UMI_CLEAR_KEY
{
    uint16      u16Status;
    uint16      u16KeyType;
    IEEE_ADDR   sStationID;
    uint8       reserved[2];
} __MTLK_PACKED UMI_CLEAR_KEY;

/***************************************************************************
**
** NAME         UM_MAN_GET_GROUP_PN_REQ
**
** PARAMETERS   UMI_GROUP_PN: empty structure to be filled on CFM
**
** DESCRIPTION  Requests the group transmit security sequence number
**
****************************************************************************/
typedef struct _UMI_GROUP_PN
{
    uint16      u16Status;
    uint8       vapIndex;
    uint8       reserved;
    uint8       au8TxSeqNum[MTLK_PAD4(UMI_RSN_SEQ_NUM_LEN)];
} __MTLK_PACKED UMI_GROUP_PN;



/***************************************************************************
**                        SECURITY MESSAGES END                           **
***************************************************************************/


/***************************************************************************
**
** NAME         UM_MAN_SET_BCL_VALUE/UM_MAN_QUERY_BCL_VALUE
**
** PARAMETERS   Unit
**              Address
**              Size
**              Data
**
** DESCRIPTION  Sets/queries BCL data from MAC
**
****************************************************************************/
#define MAX_GENERIC_REQ_DATA                    64

typedef struct _UMI_BCL_REQUEST
{
    uint32         Unit;
    uint32         Address;
    uint32         Size;
    uint32         Data[MAX_GENERIC_REQ_DATA];
} __MTLK_PACKED UMI_BCL_REQUEST;


/**********************************************************************************

UM_MAN_DATA_PATH_INIT_REQ

Description:
------------
	Directives to set HostIfGenRisc & HostIf Acc

**********************************************************************************/
typedef struct _UMI_DATA_PATH_INIT_PARAMS
{
	uint32 	txInRingStartAddress;
	uint32 	txInRingSizeBytes; 	
	uint32 	txOutRingStartAddress;
	uint32 	txOutRingSizeBytes; 
	uint32 	rxInRingStartAddress;
	uint32 	rxInRingSizeBytes; 	
	uint32 	rxOutRingStartAddress;
	uint32 	rxOutRingSizeBytes; 
	uint32 	mangTxRingStartAddress;
	uint32 	mangTxRingSizeBytes; 
	uint32 	mangRxRingStartAddress;
	uint32 	mangRxRingSizeBytes;  
	uint32 	txOutReadyCounterAddress;
	uint32 	rxOutReadyCounterAddress;
	uint32 	txInFreedCounterAddress;
	uint32 	rxInFreedCounterAddress;
	uint32	rxOutDw3FixedValues; //used for LGM & FLM
	uint32	txOutDw1FixedValues; //used for LGM & FLM
	uint16	loggerRxSid;
	uint16	numOfAllocatedRxHds;	
	uint16	numOfAllocatedTxHds;
	uint8  	dataPathMode;
	uint8  	loggerMaxStreamNumber;
	uint8  	hostEndianessMode;
	uint8  	hdOwnBitValue;
	uint8  	FWinterface;
	uint8   cbmFragmentationWaEnable;
	uint8   dataPathPort;
	uint8   Status;
	uint8	reserved[2];
} __MTLK_PACKED UMI_DATA_PATH_INIT_PARAMS;

typedef enum
{
	DATA_PATH_MODE_DC_NONE 	= 0,							/* no-GSWIP */
	DATA_PATH_MODE_DC_MODE_0,								/* GSWIP */
	DATA_PATH_MODE_DC_MODE_1,								/* PUMA	*/
	NUM_OF_DATA_PATH_MODE_GEN5,
	DATA_PATH_MODE_DC_MODE_2 = NUM_OF_DATA_PATH_MODE_GEN5,	/* LGM */
	DATA_PATH_MODE_DC_MODE_3,								/* FLM*/
	NUM_OF_DATA_PATH_MODE,
	MAX_NUM_OF_DATA_PATH = 0xFF
} DataPathMode_e;

typedef enum
{
	HOST_ENDIANESS_MODE_LE	= 0,
	HOST_ENDIANESS_MODE_BE,
	NUM_OF_HOST_ENDIANESS_MODES,
	MAX_NUM_OF_HOST_ENDIANESS_MODES = 0xFF
} HostEndianessMode_e;

typedef enum
{
	HD_HOST_OWN_BIT_VALUE 	= 0,
	WLAN_HOST_OWN_BIT_VALUE ,
	NUM_OF_HD_OWN_BIT_VALUES,
	MAX_NUM_OF_OWN_BIT_VALUES = 0xFF
} HdOwnBitValue_e;


/***************************************************************************
**
** NAME         UMI_GENERIC_MAC_REQUEST
**
** PARAMETERS   none
**
** DESCRIPTION  TODO
**
****************************************************************************/
typedef struct _UMI_GENERIC_MAC_REQUEST
{
    uint32 opcode;
    uint32 size;
    uint32 action;
    uint32 res0;
    uint32 res1;
    uint32 res2;
    uint32 retStatus;
    uint32 data[MAX_GENERIC_REQ_DATA];
} __MTLK_PACKED UMI_GENERIC_MAC_REQUEST;

#define MT_REQUEST_GET                  0
#define MT_REQUEST_SET                  1

#define EEPROM_ILLEGAL_ADDRESS          0x3



/***************************************************************************
**
** NAME         UMI_CHANGE_TX_POWER_LIMIT
**
** PARAMETERS   PowerLimitOption - 
**
** DESCRIPTION: This message should be sent to request a change to the transmit
**              power limit table.
**
****************************************************************************/
typedef struct _UMI_TX_POWER_LIMIT
{
	uint8 getSetOperation;
	uint8 powerLimitOffset;
	uint8 Status;
	uint8 Reserved;
} __MTLK_PACKED UMI_TX_POWER_LIMIT;

/**************************************
**
** NAME         UMI_MAC_WATCHDOG
**
** PARAMETERS   none
**
** DESCRIPTION  MAC Soft Watchdog
**
****************************************************************************/

typedef struct _UMI_MAC_WATCHDOG
{
	uint8  u8Status;  /* WD Status */
	uint8  u8Reserved;
	uint16 u16Timeout; /* Timeout for waiting answer from LM in milliseconds*/
} __MTLK_PACKED UMI_MAC_WATCHDOG;

/***************************************************************************
**
** NAME         UMI_GET_STATISTICS_DATA
**
** PARAMETERS   none
**
** DESCRIPTION  Statistics Data DMA transfer to Host
**
****************************************************************************/
typedef struct _UMI_GET_STATISTICS_DATA
{
	uint32 ddrBufferAddress; 
	uint32 length; 
	uint32 status; 
} __MTLK_PACKED UMI_GET_STATISTICS_DATA;


typedef struct _CLBR_HNDLR_RESET_CONF
{
	uint32 offlineCalMask;
	uint32 onlineCalMask;
} __MTLK_PACKED CLBR_HNDLR_RESET_CONF;

typedef struct _ANTENNA_PARAMETERS
{
	uint8 txAntMask;
	uint8 rxAntMask;
	uint8 txAntSelectionMask;
	uint8 rxAntSelectionMask;
} __MTLK_PACKED ANTENNA_PARAMETERS;

/*--------------------------------------------------------------------------
 * Managed Object Type Definitions
 *
 * Description:     EEPROM Version ID, Number of points in TPC_FREQ structure
 *
 *
 *--------------------------------------------------------------------------*/

#ifdef CPU_ARC
#define BAND_MIN 0
#define BAND_MAX 2
#endif

typedef struct __EEPROM_VERSION_TYPE
{
 	uint16 u16EEPROMVersion;
 	uint8 u8NumberOfPoints5GHz;
 	uint8 u8NumberOfPoints2GHz;
} __MTLK_PACKED EEPROM_VERSION_TYPE;

typedef struct _HDK_CONFIGURATION
{
	uint8					numTxAnts;
	uint8					numRxAnts;
	EEPROM_VERSION_TYPE		eepromInfo;
	uint8					band;
	CLBR_HNDLR_RESET_CONF	calibrationMasks;
} __MTLK_PACKED HDK_CONFIGURATION;

typedef struct _UMI_HDK_CONFIG
{
	uint32			   calibrationBufferBaseAddress;
	uint8			   getSetOperation;
	uint8			   setChannelMode;
	uint8			   reserved[2];
	HDK_CONFIGURATION  hdkConf;
	uint8 	Status;
	uint8   Reserved[3];
} __MTLK_PACKED UMI_HDK_CONFIG;
typedef struct _UMI_HDK_SET_ANTENNA_REQ
{
	ANTENNA_PARAMETERS antParams[CDB_NUM_OF_SEGMENTS]; 
} __MTLK_PACKED UMI_HDK_SET_ANTENNA_REQ;

/***************************************************************************
**
** NAME         UM_DBG_INPUT_REQ
**
** PARAMETERS   u16Length           The number of bytes of input stream
**                                  contained in this message.
**              au8Data             An array of characters containing a
**                                  section of debug input stream.
**
** DESCRIPTION  Debug Input Request
**
****************************************************************************/
typedef struct _UMI_DEBUG
{
    uint16 u16Length;
    uint16 u16stream;
    uint8  au8Data[MTLK_PAD4(UMI_DEBUG_DATA_SIZE)];
} __MTLK_PACKED UMI_DEBUG;


/***************************************************************************
**
** NAME         UMI_INTERFERER_DETECTION_PARAMS
**
** PARAMETERS  threshold - detection threshold in dBm
**
** DESCRIPTION  
**
****************************************************************************/
#ifdef CPU_ARC
#define INTRFRER_THRSH_MIN		(-128)
#define INTRFRER_THRSH_MAX		(127)
#endif //CPU_ARC

typedef struct _UMI_INTERFERER_DETECTION_PARAMS
{
	uint8	getSetOperation;
	int8	threshold;
	uint8   Status;
	uint8	reserved;
} __MTLK_PACKED UMI_INTERFERER_DETECTION_PARAMS;

/***************************************************************************
**
** NAME         UMI_ENABLE_RADAR_INDICATION
**
** PARAMETERS  enableIndication - a flag that indicates if indications 
** should be passed to the driver
**
** DESCRIPTION  
**
****************************************************************************/

typedef struct _UMI_ENABLE_RADAR_INDICATION
{
	int8	enableIndication;
	uint8	Status;
	uint8   Reserved[2];
} __MTLK_PACKED UMI_ENABLE_RADAR_INDICATION;


/***************************************************************************
**
** NAME         UMI_CONTINUOUS_INTERFERER
**
** PARAMETERS   maximumValue - maximum value detected in dBm
**
** DESCRIPTION  Return message buffer.
**
****************************************************************************/

typedef struct _UMI_CONTINUOUS_INTERFERER
{
	int8	maximumValue;
	uint8	channel;
	uint8   Reserved[2];
} __MTLK_PACKED UMI_CONTINUOUS_INTERFERER;

/***************************************************************************
**
** NAME         UMI_RADAR_DETECTION
**
** PARAMETERS   radar type
**
** DESCRIPTION  Return message buffer.
**
****************************************************************************/

typedef struct _UMI_RADAR_DETECTION
{	
	uint16  subBandBitmap;
	uint8	channel;
	uint8	radarType;
} __MTLK_PACKED UMI_RADAR_DETECTION;

/***************************************************************************
**
** NAME         UMI_STA_EXCESSIVE_RETRIES_DETECTION
**
** PARAMETERS   station id
**
** DESCRIPTION  Return message buffer
**
****************************************************************************/

typedef struct _UMI_STA_EXCESSIVE_RETRIES_DETECTION
{
	uint16 staId;
	uint8 reserved[2];
} __MTLK_PACKED UMI_STA_EXCESSIVE_RETRIES_DETECTION;

/***************************************************************************
**
** NAME:        UMI_EXT_BLOCKED_ACCESS
** PARAMETERS   
**
** DESCRIPTION  send Blocked Access indication.
**
****************************************************************************/

typedef struct
{
	uint32 	blockedAccessAddr; 	// Address of the access that caused filter to set violation
	uint16 	interruptCounter;	// interrupt counter from last indication to driver
	uint8 	blockedAccessCause; 	// 0 = read access, 1 = write access
	uint8 	securityLevel; 		// Mac security level during the violation. 1 - stage1 , 2 - stage2 , 3 - non-secured mode		
} __MTLK_PACKED UMI_EXT_BLOCKED_ACCESS;


/***************************************************************************
**
** NAME         UMI_DBG
**
** DESCRIPTION  A union of all Debug Messages.
**
****************************************************************************/
typedef union _UMI_DBG
{
    UMI_DEBUG            sDebug;
	UMI_DUT				 sDut;
} __MTLK_PACKED UMI_DBG;


/***************************************************************************
**
**  BSS Manger related definitions
**
****************************************************************************/

typedef struct _RX_BSS_IND_MSG_DESC
{
    uint32 u32HostPayloadAddr;
} __MTLK_PACKED RX_BSS_IND_MSG_DESC;


/***************************************************************************/
/***                            Data Messages                            ***/
/***************************************************************************/

/***************************************************************************
**
** NAME         UM_DAT_TXDATA_REQ
**
** PARAMETERS   u32MSDUtag          Reference to the buffer containing the
**                                  payload of the MSDU in external memory.
**              u16MSDUlength       Length of the MSDU payload in the range
**                                  0..UMI_MAX_MSDU_LENGTH.
**              u16AccessProtocol   UMI_USE_DCF
**                                  UMI_USE_PCF
**              sSA                 Source MAC Address (AP only).
**              sDA                 Destination MAC Address.
**              sWDSA               Wireless Distribution System Address
**                                  (reserved).
**              u16Status           Not used.
**              pvMyMsdu            Reserved for use by the MAC.
**
** DESCRIPTION  Transmit Data Request
**
****************************************************************************/

/* DW0 */
#define TX_DATA_INFO_STAID			 MTLK_BFIELD_INFO(0, 8)  /*  8 bits starting bit0 */
#define TX_DATA_INFO_MCIDX			 MTLK_BFIELD_INFO(0, 7)  /*  7 bits starting bit0 */
#define TX_DATA_INFO_VAPID			 MTLK_BFIELD_INFO(8, 4)  /*  4 bits starting bit8 */
#define TX_DATA_INFO_POWERMANAGEMENT MTLK_BFIELD_INFO(12,1)  /*  1 bit starting bit12 */
#define TX_DATA_INFO_UNKNOWN_SID 	 MTLK_BFIELD_INFO(13,1)  /*  1 bit starting bit13 */
#define TX_DATA_INFO_MCF			 MTLK_BFIELD_INFO(14,1)  /*  1 bit  starting bit14 */
#define TX_DATA_INFO_FRAMETYPE		 MTLK_BFIELD_INFO(15,2)  /*  2 bits starting bit15 */
#define TX_BSS_EXTRA_ACTION_CODE	 MTLK_BFIELD_INFO(17,7)  /*  7 bits starting bit17 */


#define TX_BSS_EXTRA_SUBTYPE        MTLK_BFIELD_INFO(26, 4) /* Subtype of management frame. Needed to recognize which management frame we got from higher layer */
#define TX_BSS_EXTRA_TYPE           MTLK_BFIELD_INFO(24, 2) /* type of management frame */

#define TX_BSS_EXTRA_STATUS			MTLK_BFIELD_INFO(30, 1) /*  1 bits starting bit30 */

/* DW1 */
#define TX_DATA_INFO_CLASS     		MTLK_BFIELD_INFO(0, 4)  /*  3 bits starting bit0 */
#define TX_DATA_INFO_EP     		MTLK_BFIELD_INFO(8, 4)  /*  4 bits starting bit8 */


/* DW2 */

/* DW3 */
#define TX_DATA_INFO_LENGTH    		MTLK_BFIELD_INFO(0, 16) /* 16 bits starting bit0  */
#define TX_DATA_INFO_OFFSET   		MTLK_BFIELD_INFO(23, 3) /*  2 bits starting bit23 */
#define TX_DATA_INFO_SOP_EOP   		MTLK_BFIELD_INFO(28, 2) /*  2 bits starting bit28 */

#define TX_DATA_INFO_OWN_BIT   		MTLK_BFIELD_INFO(31, 1) /* own bit */
/*DW4*/
#define TX_DATA_BD_INDEX    		MTLK_BFIELD_INFO(0, 16) /* 16 bits starting bit0  */


// Ethernet types for HD - TX_DATA_INFO_FRAMETYPE
typedef enum {
	FRAME_TYPE_ETHERNET = 0,
	FRAME_TYPE_ILLEGAL, // 1
	FRAME_TYPE_IPX_LLC_SNAP, // 2
	FRAME_TYPE_EAPOL, // 3
	NUM_OF_FRAME_TYPES
} FrameType_e; 


/* Values for u8PacketType  */
#define ENCAP_TYPE_RFC1042           0
#define ENCAP_TYPE_STT               1
#define ENCAP_TYPE_8022              2
#define ENCAP_TYPE_ILLEGAL           3

/* This was the old Data Request Message Descriptor */
typedef struct _UMI_DATA_RX
{
    uint32    u32MSDUtag;
    uint16    u16MSDUlength;
	uint8     u8Notification;
	uint8     u8Offset;
    uint16    u16AccessProtocol;
    IEEE_ADDR sSA;
    IEEE_ADDR sDA;
    IEEE_ADDR sWDSA;
    mtlk_void_ptr psMyMsdu;
} __MTLK_PACKED UMI_DATA_RX;

#define TX_BSS_INFO_WDS    MTLK_BFIELD_INFO(0, 1)  /*  1 bit  starting bit0 */
#define TX_BSS_INFO_TID    MTLK_BFIELD_INFO(1, 3)  /*  3 bits starting bit1 */
#define TX_BSS_INFO_LENGTH MTLK_BFIELD_INFO(4, 12) /* 12 bits starting bit4 */


#define TX_BSS_EXTRA_ENCAP_TYPE		MTLK_BFIELD_INFO(0, 2)  /* 2 LS bits */
#define TX_BSS_EXTRA_MANAGEMENT		MTLK_BFIELD_INFO(6, 1)  /* Flag indicating that this is a management frame we got from higher layer */


typedef struct _UMI_BSS_TX
{	
    uint32    u32HostPayloadAddr;
    uint16    sid;
    uint16    u16FrameInfo; /* use FRAME_INFO_... macros for access */
    uint8     u8ExtraData; /* see TX_EXTRA_... for available values */
    uint8     vapId;
	uint8	  u8Status;
	uint8	  retransmissions; // Num of retransmissions for this packet. reported to driver.
} __MTLK_PACKED UMI_BSS_TX;







/***************************************************************************
**
** NAME         MC_DAT_TXDATA_CFM
**
** PARAMETERS   u32MSDUtag          Reference to the buffer containing the
**                                  payload of the MSDU that was transmitted.
**              u16MSDUlength       As request.
**              u16AccessProtocol   As request.
**              sSA                 As request.
**              sDA                 As request.
**              sWDSA               As request.
**              u16Status           UMI_OK
**                                  UMI_NOT_INITIALISED
**                                  UMI_BAD_LENGTH
**                                  UMI_TX_TIMEOUT
**                                  UMI_BSS_HAS_NO_PCF
**                                  UMI_NOT_CONNECTED
**                                  UMI_NOT_INITIALISED
**
** DESCRIPTION  Transmit Data Confirm
**
***************************************************************************/


/* Logger <-> HIM Messages */

typedef struct LOG_AGENT_LOGGER_GROUPS_BITMAP
{
    uint32 logAgentLoggerGroupsBitMap[LOGGER_NUM_OF_GROUPS_BIT_MAP];
} __MTLK_PACKED LOG_AGENT_LOGGER_GROUPS_BITMAP;

typedef enum
{
	LOGGER_STATE_READY,
	LOGGER_STATE_INACTIVE,
	LOGGER_STATE_ACTIVE,
	LOGGER_STATE_INIT_FAILED,
	LOGGER_STATE_CYCLIC_MODE,
	LOGGER_STATE_MAX = MAX_UINT8
} LogAgentState_e;

typedef enum
{
	LOGGER_STATE_ACTIVE_ONLINE,
	LOGGER_STATE_ACTIVE_BACKGROUND,
	LOGGER_STATE_ACTIVE_MAX = MAX_UINT8
} LogAgentStateActive_e;

#define SID_FOR_LOGGER_RX_DATA 	  (0x7f)
#define LOGGER_END_OF_BUFFER_MARK 0xDEADBEEF


/***************************************************************************
**
** NAME         MC_DAT_SEND_BUF_TO_HOST_IND
**
** PARAMETERS   length - Length (in bytes) of actual buffer payload
**              buffer - Pointer to the buffer being sent
**
** DESCRIPTION  This message is sent by the FW MAC, over the Host/IF module,
**              to deliver a logAgent buffer from the MAC to the Host
**              (and eventually out to the LogViewer through the LogServer).
**
****************************************************************************/
typedef struct _UMI_LOGGER_SEND_MSGS_SEND_BUFFER
{
    uint32  length;
    char*   buffer;
} __MTLK_PACKED UMI_LOGGER_SEND_MSGS_SEND_BUFFER;

/***************************************************************************
**
** NAME         UM_DAT_SEND_BUF_TO_HOST_RES
**
** PARAMETERS   None
**
** DESCRIPTION  This message is sent by the Host when the buffer sent to it 
**              was handled by the Host/IF and is given back to the sender.
**              Upon receiving this message the buffer will be considered free.
**
****************************************************************************/

/* UM_DAT_SEND_BUF_TO_HOST_RES */
/* UMI_LOGGER_SEND_MSGS_SEND_BUFFER from IND  */


/***************************************************************************
**
** NAME         UM_DBG_LOGGER_FLUSH_BUF_REQ
**
** PARAMETERS   targetCPU  - The CPU (LM, UM, single CPU, etc.) whose logAgent's
**                           buffer should be flushed
**
** DESCRIPTION  This message is sent from the Host to the MAC when a flush request
**              was made in the code. Upon receiving this message, the logAgent
**              will send its current buffer "outside" (unless buffer is empty).
**
****************************************************************************/
#ifdef CPU_ARC
#define LOG_MSG_FLSH_BUF_TARGET_CPU_MIN			(0)
#define LOG_MSG_FLSH_BUF_TARGET_CPU_MAX			(2)
#endif //CPU_ARC

typedef struct _UMI_LOGGER_MSG_FLUSH_BUFFER
{ 
    uint32 /*UmiCpuId_e*/ targetCPU;
	uint8 	Status;
	uint8   Reserved[3];
} __MTLK_PACKED UMI_LOGGER_MSG_FLUSH_BUFFER;



/***************************************************************************
**
** NAME         UM_DBG_LOGGER_SET_MODE_REQ
**
** PARAMETERS   modeReq   - either of these logAgent states (LogAgentState_e):
**							LOGGER_MODE_ACTIVE, LOGGER_MODE_INACTIVE, LOGGER_MODE_CYCLIC
**              targetCPU  - The target CPU (LM, UM, single CPU, etc.)
**                           whose logAgent's state should be changed
**
** DESCRIPTION  This message is sent from the Host to the MAC's logAgent to
**              switch between logAgent states. The user can only switch between
**              LOGGER_MODE_ACTIVE, LOGGER_STATE_INACTIVE and LOGGER_STATE_CYCLIC_MODE. If the
**              logAgent is already in the state being set in the message,
**              nothing happens.
**
****************************************************************************/
typedef struct _UMI_LOGGER_MSG_SET_STATE
{
    uint32 /*LogAgentState_e*/ modeReq;
    uint32 /*UmiCpuId_e*/      targetCPU;
	uint8 					   Status;
	uint8   				   Reserved[3];
} __MTLK_PACKED UMI_LOGGER_MSG_SET_STATE;



/***************************************************************************
**
** NAME         UM_DBG_LOGGER_SET_SEVERITY_REQ
**
** PARAMETERS   newLevel   - Value of the new severity level
**              targetCPU  - The target CPU (LM, UM, single CPU, etc.) whose
**                           logAgent's severity level is to be set
**
** DESCRIPTION  This message is sent from the Host to the MAC's logAgent to
**              set the value of severity level filter.
**
****************************************************************************/
typedef struct _UMI_LOGGER_MSGS_SET_SEVERITY
{
    uint32 /*LogAgentSeverityLevel*/	newLevel;
    uint32 /*UmiCpuId_e*/               targetCPU;
	uint8								getSetOperation;
	uint8 								Status;
	uint8								reserved[2];
} __MTLK_PACKED UMI_LOGGER_MSGS_SET_SEVERITY;


/***************************************************************************
**
** NAME         UM_DBG_LOGGER_SET_FILTER_REQ
**
** PARAMETERS   gidFilterMask   - Value of the new severity level
**              targetCPU       - The target CPU (LM, UM, single CPU, etc.)
**                                whose logAgent's filter should be set
**
** DESCRIPTION  This message is sent from the Host to the MAC's logAgent to set
**              the GID filters map / mask. The user can turn on (GID bit = 1),
**              or off (GID bit = 0) any GID, either from the web UI, or from a
**              command line interface.
**
****************************************************************************/
typedef struct _UMI_LOGGER_MSG_SET_FILTER
{
    LOG_AGENT_LOGGER_GROUPS_BITMAP 	gidFilterMask;
    uint32 /*UmiCpuId_e*/           targetCPU;
	uint8 							status;
	uint8   						Reserved[3];
} __MTLK_PACKED UMI_LOGGER_MSG_SET_FILTER;



/***************************************************************************
**
** NAME         MC_MAN_TRACE_IND
**
** PARAMETERS   None
**
** DESCRIPTION  This message is sent from the UMAC to the Host, 
**              it contains a fixed char array and a size of the valid num of chars
**
****************************************************************************/
#define MAX_DBG_TRACE_DATA  64

typedef struct _UMI_DBG_TRACE_IND
{
    uint32 length;
    uint32 val1;
    uint32 val2;
    uint32 val3;
    uint8  au8Data[MTLK_PAD4(MAX_DBG_TRACE_DATA)];
} __MTLK_PACKED UMI_DBG_TRACE_IND;

/***************************************************************************
**
** NAME         UM_DBG_CLI_REQ
**
** PARAMETERS   None
**
** DESCRIPTION  This message is sent from the Host to the UMAC, 
**
****************************************************************************/

typedef struct _UMI_DBG_CLI_REQ
{
    uint32  action;
    uint32  numOfArgumets;
    uint32  data1;
    uint32  data2;
    uint32  data3;
	uint8 	Status;
	uint8   Reserved[3];
} __MTLK_PACKED UMI_DBG_CLI_REQ;

/***************************************************************************
**
** NAME         
**
** PARAMETERS   xtal   - Xtal value to set for this specific board
**
** DESCRIPTION  This message is sent from the Host to the MAC in order to 
**				configure the MAC's HW specific parts. i.e., the configuration 
**				known only after the Host knows the specific HW.
**
****************************************************************************/
#define AFE_INVALID 0

#ifdef CPU_ARC
#define HW_DEP_CFG_XTAL_MIN			(0)
#define HW_DEP_CFG_XTAL_MAX			(255)
#endif //CPU_ARC

typedef struct _UMI_HW_DEPENDENT_CONFIG
{
	uint32	xtal;
	uint32  AFEvalue[2];
	uint8 	Status;
	uint8   Reserved[3];
} __MTLK_PACKED UMI_HW_DEPENDENT_CONFIG;



/****************************************************************************/
/***                      Public Function Prototypes                    								 ***/
/***************************************************************************/

/*
 * Message between the MC and UM have a header.  The MC only needs the position
 * of the type field within the message and the length of the header.  All other
 * elements of the header are unused in the LM
*/

typedef struct _UMI_MSG
{
    mtlk_umimsg_ptr psNext;       /* Used to link list structures */
    uint8  u8Pad1;
    uint8  u8Persistent;
    uint16 u16MsgId;
    uint32 u32Pad2;                 /* For MIPS 8 bytes alignment */
    uint32 u32MessageRef;           /* Address in Host for Message body copy by DMA */
    uint8  abData[1];
} __MTLK_PACKED UMI_MSG;

typedef struct _UMI_MSG_HEADER
{
    mtlk_umimsg_ptr psNext;       /* Used to link list structures */
    uint8  u8Pad1;
    uint8  u8Persistent;
    uint16 u16MsgId;
    uint32 u32Pad2;                 /* For MIPS 8 bytes alignment */
    uint32 u32MessageRef;           /* Address in Host for Message body copy by DMA */
} __MTLK_PACKED UMI_MSG_HEADER;

/* REVISIT - was in shram.h - maybe should be in a him .h file but here is better for now */
/* linked UMI_DATA, MSDU, Host memory */
typedef struct _UMI_DATA_RX_STORAGE_ELEMENT
{
    UMI_MSG_HEADER    sMsgHeader;
    UMI_DATA_RX       sDATA;
} __MTLK_PACKED UMI_DATA_RX_STORAGE_ELEMENT;


/***************************************************************************/
/* Data transfer messages between MAC and Host */
/***************************************************************************/
/*Bss Manager packets transfer messages between MAC and Host */
/***************************************************************************/

typedef struct _SHRAM_BSS_REQ_MSG
{
    UMI_MSG_HEADER 	sHdr;                 /* Kernel Message Header */
    UMI_BSS_TX    	sMsg;                 /* UMI BSS Message */
} __MTLK_PACKED SHRAM_BSS_REQ_MSG;


/***************************************************************************
**
** NAME         UMI_MAN_SET_11B_ANT_REQ
**
** PARAMETERS   
**
** DESCRIPTION  
**              
*****************************************************************************/
#ifdef CPU_ARC
#define ANT_SEL_11B_TX_MIN				(0)
#define ANT_SEL_11B_TX_MAX				(255)

#define ANT_SEL_11B_RX_MIN				(0)
#define ANT_SEL_11B_RX_MAX				(255)

#define ANT_SEL_11B_RATE_MIN			(0)
#define ANT_SEL_11B_RATE_MAX			(255)
#endif //CPU_ARC

typedef struct _UMI_ANT_SELECTION_11B
{
	uint8 getSetOperation;
	uint8 txAnt;
	uint8 rxAnt;
	uint8 rate; /*this filed is set only when txAnt and rxAnt set to Round Rubin mode, otherwise the filed value should be -1 */
	uint8 Status;
	uint8 Reserved[3];
} __MTLK_PACKED UMI_ANT_SELECTION_11B;



/***************************************************************************
**
** NAME         UMI_CALIBRATE_PARAMS
**
** PARAMETERS   
**
** DESCRIPTION  
**              
*****************************************************************************/

#define MAX_CALIB_CHANS 60

#define CHAN_NUMS_MIN   0
#define CHAN_NUMS_MAX   200

#define NUM_CHANS_MIN   1
#define NUM_CHANS_MAX   MAX_CALIB_CHANS

#define CHAN_WIDTH_MIN  0
#define CHAN_WIDTH_MAX  5

typedef struct _UMI_CALIBRATE_PARAMS
{
	uint32	stored_calib_data; 			// pointer to shared memory
	uint32	Status; 					// calibration result (success/fail) for each channel in the appropriate bit
	uint8	chan_nums[MAX_CALIB_CHANS]; // channel numbers to calibrate
	uint8	num_chans; 					// how many channels specified in the array above
	uint8	chan_width; 				// values from the chanWidth enum; in what width they are to be calibrated
	uint8	reserved[2];
} __MTLK_PACKED UMI_CALIBRATE_PARAMS;


/***************************************************************************
**
** NAME         UMI_SET_2_4_G_COEX
**
** PARAMETERS   
**
** DESCRIPTION  
**              
*****************************************************************************/

/* Input validation defines */
#define ACTIVE_WIFI_WINDOW_SIZE_MIN			2						/* Min value for WIFI active period											*/
#define ACTIVE_WIFI_WINDOW_SIZE_MAX			(MAX_UINT32/(1024*2))	/* Max value for HW timer (1024 - mili to micro. 2 - US_TO_PRESCALER_UNIT)	*/
#define INACTIVE_WIFI_WINDOW_SIZE_MIN		2						/* Min value for WIFI inactive period										*/
#define INACTIVE_WIFI_WINDOW_SIZE_MAX		32						/* Max value of CTS TO SELF 												*/
#define COEX_QOS_MIN						CO_EX_QOS_NONE			/* Min value at CO_EX_QOS													*/
#define COEX_QOS_MAX						CO_EX_QOS_6				/* Max value at CO_EX_QOS													*/

typedef enum _CO_EX_QOS
{
	CO_EX_QOS_NONE,						
	CO_EX_QOS_ACTIVE_AND_SILENT_WINDOWS, 	/* ZigBee is activated. Silent windows are created by wifi. Holdoff is set to 1.					*/
	CO_EX_QOS_ACTIVE,					 	/* ZigBee is activated. Silent windows are created by zigbee. Holdoff is set to 1.					*/
	CO_EX_QOS_RESERVED_1,					/* Reserved. 																						*/	
	CO_EX_QOS_RESERVED_2,					/* Reserved. 																						*/		
	CO_EX_QOS_5,							/* FOR TESTING: Same as CO_EX_QOS_ACTIVE_AND_SILENT_WINDOWS except for holdoff which is set to 21. 	*/
	CO_EX_QOS_6,							/* FOR TESTING: Same as CO_EX_QOS_ACTIVE except for holdoff which is set to 21. 					*/
	CO_EX_QOS_NUM_OF_TYPES,
} CO_EX_QOS;

typedef struct _UMI_SET_2_4_G_COEX
{	
	uint32		activeWifiWindowSize;	/* For silent windows this parameters determines the period of time [mSec] that WIFI is allowed to transmit. Range [2..MAX_UINT32/(1024*2)] (1024 - due to change from milli to micro. 2 - due to US_TO_PRESCALER_FACTOR )	*/
	
	uint8		inactiveWifiWindowSize;	/* For silent windows this parameters determines the period of time [mSec] that WIFI is not allowed to transmit. Range [2..32] (32 - due to CTS to self)																		*/
	uint8		getSetOperation;
	uint8		coExQos;				/* Values from CO_EX_QOS: What is the QoS the Driver requests for the radios operating in 2.4G band 																														*/
	uint8		bCoExActive;			/* TRUE: 2.4GHz co-existence is activated 																																													*/
	
	uint8		Status;
	uint8		bCts2selfActive;        /* TRUE: cts2self is activated */
	uint8		reserved[2];
} __MTLK_PACKED UMI_SET_2_4_G_COEX;


/***************************************************************************
**
**             
**
** DESCRIPTION 
**           
**
****************************************************************************/
typedef struct _UMI_FW_DBG_REQ
{
    uint8	debugType; /* UmiDebugType_e */
	uint8 	Status;
	uint8   Reserved[2];
} __MTLK_PACKED UMI_FW_DBG_REQ;


/**********************************************************
 * BSS 
 **********************************************************/

 
#define UM_MSG(dir, type, msg_id) \
  (K_MSG_TYPE) ( \
               (((dir)    << MSG_DIR_OFFSET)  & MSG_DIR_MASK)  | \
               (((type)   << MSG_TYPE_OFFSET) & MSG_TYPE_MASK) | \
               (((msg_id) << MSG_NUM_OFFSET)  & MSG_NUM_MASK)    \
                 )

#define UMI_MSG_MAN_REQ(msg_id) 		UM_MSG(MSG_REQ, MSG_TYPE_MAN_REQ, msg_id) // 0x04xx
#define UMI_MSG_MAN_CFM(msg_id) 		UM_MSG(MSG_CFM, MSG_TYPE_MAN_REQ, msg_id) // 0x14xx
#define UMI_MSG_MAN_IND(msg_id) 		UM_MSG(MSG_IND, MSG_TYPE_MAN_IND, msg_id)
#define UMI_MSG_MAN_RES(msg_id) 		UM_MSG(MSG_RES, MSG_TYPE_MAN_IND, msg_id)

#define UMI_MSG_DBG_REQ(msg_id) 		UM_MSG(MSG_REQ, MSG_TYPE_DBG_REQ, msg_id)
#define UMI_MSG_DBG_CFM(msg_id) 		UM_MSG(MSG_CFM, MSG_TYPE_DBG_REQ, msg_id)
#define UMI_MSG_DBG_IND(msg_id) 		UM_MSG(MSG_IND, MSG_TYPE_DBG_IND, msg_id)
#define UMI_MSG_DBG_RES(msg_id) 		UM_MSG(MSG_RES, MSG_TYPE_DBG_IND, msg_id)

#define UMI_MSG_DAT_REQ(msg_id) 		UM_MSG(MSG_REQ, MSG_TYPE_DAT_REQ, msg_id)
#define UMI_MSG_DAT_CFM(msg_id) 		UM_MSG(MSG_CFM, MSG_TYPE_DAT_REQ, msg_id)
#define UMI_MSG_DAT_IND(msg_id) 		UM_MSG(MSG_IND, MSG_TYPE_DAT_IND, msg_id)
#define UMI_MSG_DAT_RES(msg_id) 		UM_MSG(MSG_RES, MSG_TYPE_DAT_IND, msg_id)
#define UMI_MSG_DAT_IND_LOGGER(msg_id)	UM_MSG(MSG_IND, MSG_TYPE_DAT_LOGGER_IND, msg_id)
#define UMI_MSG_DAT_RES_LOGGER(msg_id)	UM_MSG(MSG_RES, MSG_TYPE_DAT_LOGGER_IND, msg_id)

#define UMI_MSG_BSS_REQ(msg_id) 		UM_MSG(MSG_REQ, MSG_TYPE_BSS_REQ, msg_id)
#define UMI_MSG_BSS_CFM(msg_id) 		UM_MSG(MSG_CFM, MSG_TYPE_BSS_REQ, msg_id)
#define UMI_MSG_BSS_IND(msg_id) 		UM_MSG(MSG_IND, MSG_TYPE_BSS_IND, msg_id)
#define UMI_MSG_BSS_RES(msg_id) 		UM_MSG(MSG_RES, MSG_TYPE_BSS_IND, msg_id)


/* u8Flags flag description: */
#define STA_ADD_FLAGS_WDS                   MTLK_BFIELD_INFO(0, 1)
#define STA_ADD_FLAGS_WME                   MTLK_BFIELD_INFO(1, 1)
#define STA_ADD_FLAGS_RESERVED  			MTLK_BFIELD_INFO(2, 1)
#define STA_ADD_FLAGS_MFP                   MTLK_BFIELD_INFO(3, 1)
#define STA_ADD_FLAGS_IS_HT                 MTLK_BFIELD_INFO(4, 1)
#define STA_ADD_FLAGS_IS_VHT				MTLK_BFIELD_INFO(5, 1)
#define STA_ADD_FLAGS_OMN	                MTLK_BFIELD_INFO(6, 1)
#define STA_ADD_FLAGS_OPER_MODE_NOTIF_VALID MTLK_BFIELD_INFO(7, 1)

/* u8FlagsExt flag description: */
#define STA_ADD_FLAGS_EXT_PBAC              MTLK_BFIELD_INFO(0, 1)
#define STA_ADD_FLAGS_EXT_IS_HE             MTLK_BFIELD_INFO(1, 1)
#define STA_ADD_FLAGS_EXT_IS_HE_6GHZ		MTLK_BFIELD_INFO(2, 1)
#define STA_ADD_FLAGS_EXT_RES_3             MTLK_BFIELD_INFO(3, 1)
#define STA_ADD_FLAGS_EXT_RES_4             MTLK_BFIELD_INFO(4, 1)
#define STA_ADD_FLAGS_EXT_RES_5				MTLK_BFIELD_INFO(5, 1)
#define STA_ADD_FLAGS_EXT_RES_6	            MTLK_BFIELD_INFO(6, 1)
#define STA_ADD_FLAGS_EXT_RES_7				MTLK_BFIELD_INFO(7, 1)


#define MAX_SIZE_OF_RSSI_TABLE 				(217)

typedef struct _UMI_SET_RSSI_CAL_CONFIG	
{
		uint8	size;
		uint8	data[MTLK_PAD4(MAX_SIZE_OF_RSSI_TABLE)];
} __MTLK_PACKED UMI_SET_RSSI_CAL_CONFIG;


#ifdef CPU_ARC
#define STA_ADD_LISTEN_INTRVL_MIN			(0)
#define STA_ADD_LISTEN_INTRVL_MAX			(255)

#define STA_ADD_UAPSD_QUEUES_MIN			(0)
#define STA_ADD_UAPSD_QUEUES_MAX			(255)

#define STA_ADD_MAX_SP_MIN					(0)
#define STA_ADD_MAX_SP_MAX					(255)

#define STA_ADD_WDS_CLIENT_TYPE_MIN			(0)
#define STA_ADD_WDS_CLIENT_TYPE_MAX			(2)
#endif //CPU_ARC

typedef struct _UMI_STA_ADD
{
	uint16    	u16SID;
	uint8     	u8VapIndex;
	uint8     	u8Status;
	
	uint8     	u8ListenInterval;
	uint8     	u8BSS_Coex_20_40;
	uint8     	u8UAPSD_Queues;
	uint8     	u8Max_SP;

	uint16    	u16AID;
	IEEE_ADDR 	sAddr;
	
	uint8     	u8Rates[MAX_NUM_SUPPORTED_RATES];

	uint16    	u16HT_Cap_Info;
	uint8     	u8AMPDU_Param;
	uint8     	u8RX_MCS_Bitmask[10];	// Corresponds to "Supported MCS set field" in IE 
	uint8		u8Rates_Length;
	uint8     	u8Flags;				// See "u8Flags flag description:" above
	uint8     	u8FlagsExt;				// See "u8FlagsExt flag description:" above

	uint32    	u32VHT_Cap_Info;
	uint32    	u32VHT_Mcs_Nss[2]; 		// Correspond to "8.4.2.160.3 Supported VHT-MCS and NSS Set field"
	uint8    	u8HE_Mac_Phy_Cap_Info[18];
	uint8    	u8HE_Mcs_Nss[8]; // Supported HE-MCS and NSS Set field"
    uint8       u8HE_Ppe_Th[13];	
	uint8 		heExtSingleUserDisable;
	
	uint32		transmitBfCapabilities;

	uint8		u8VHT_OperatingModeNotification;
	uint8		u8HE_OperatingModeNotification;
    uint8       u8WDS_client_type; //UmiWDS_AES_e
	int8		rssi; // max rssi
	
	uint8 		Status;	
	uint8     	u8SecondaryVapIndex; //used for mesh (extender) implementation for correct routing in gswip/driver
	uint16 		u16HE_6ghz_Cap_Info;
} __MTLK_PACKED UMI_STA_ADD;

 
typedef struct _UMI_STA_REMOVE
{
	uint16 		u16SID;
	uint8  		u8Status;
	uint8  		u8Reserved;
} __MTLK_PACKED UMI_STA_REMOVE;



typedef struct _UMI_REQUEST_SID
{
	IEEE_ADDR 	sAddr;
	uint16 		u16SID;
	uint8  		u8Status;
	uint8  		u8Reserved[3];
} __MTLK_PACKED UMI_REQUEST_SID;



typedef struct _UMI_REMOVE_SID
{
	uint16 		u16SID;
	uint8  		u8Status;
 	uint8  		u8Reserved;
} __MTLK_PACKED UMI_REMOVE_SID;

// QAMplus : activate from Driver
typedef struct _UMI_QAMPLUS_ACTIVATE
{
	uint8 getSetOperation;
	uint8 enableQAMplus; 
	uint8 Status;
	uint8 reserved;
} __MTLK_PACKED UMI_QAMPLUS_ACTIVATE;

#define WINDOW_SIZE_NO_CHANGE	(0)

#ifdef CPU_ARC
#define VAP_ID_MIN 0
#define VAP_ID_MAX 31
#endif //CPU_ARC

//Min & Max window size defined only for w600
#define WINDOW_SIZE_MIN  2
#define WINDOW_SIZE_MAX  256

typedef struct _UMI_TS_VAP_CONFIGURE
{	
	uint32		windowSize;
	uint8		getSetOperation;
	uint8		vapId;
	uint8		enableBa;
	uint8		amsduSupport;
	uint8       Status;
	uint8 		Reserved[3];
} __MTLK_PACKED UMI_TS_VAP_CONFIGURE;

#define HT_MSDU_IN_AMSDU_MIN  2
#define HT_MSDU_IN_AMSDU_MAX  31

#define VHT_MSDU_IN_AMSDU_MIN  2
#define VHT_MSDU_IN_AMSDU_MAX  31

#define HE_MSDU_IN_AMSDU_MIN  2
#define HE_MSDU_IN_AMSDU_MAX  31

typedef struct _UMI_MSDU_IN_AMSDU_CONFIG
{
	uint8		getSetOperation;
	uint8		htMsduInAmsdu;
	uint8		vhtMsduInAmsdu;
	uint8 		heMsduInAmsdu;
	uint8		Status;
	uint8 		Reserved[3];
} __MTLK_PACKED UMI_MSDU_IN_AMSDU_CONFIG;


typedef struct _UMI_MU_OPERATION_CONFIG
{
	uint8		getSetOperation;
	uint8		enableMuOperation;
	uint8		Status;
	uint8		reserved;
} __MTLK_PACKED UMI_MU_OPERATION_CONFIG;

typedef struct _UMI_HE_MU_OPERATION_CONFIG
{
	uint8		enableHeMuOperation;
	uint8		getSetOperation;
	uint8		Status;
	uint8		reserved;
} __MTLK_PACKED UMI_HE_MU_OPERATION_CONFIG;

typedef struct _UMI_RTS_MODE_CONFIG
{
	uint8		getSetOperation;
	uint8		dynamicBw;
	uint8		staticBw;
	uint8		Status;
} __MTLK_PACKED UMI_RTS_MODE_CONFIG;

#define UMI_DEF_MPDU_LENGTH 11454
#define UMI_DEF_MPDU_LENGTH_MIN 3839
#define UMI_DEF_MPDU_LENGTH_MAX 11454

typedef struct _UMI_MAX_MPDU
{
	uint32	maxMpduLength;
	uint8	getSetOperation;
	uint8   Status;
	uint8	reserved[2];
} __MTLK_PACKED UMI_MAX_MPDU;


typedef enum BeamformingMode
{
	BF_STATE_EXPLICIT,
	BF_STATE_IMPLICIT,
	BF_STATE_STBC1X2,
	BF_STATE_STBC2X4,
	BF_STATE_NON_BF,
	BF_NUMBER_OF_MODES,
	BF_FIRST_STATE = BF_STATE_EXPLICIT,
	BF_LAST_STATE = BF_STATE_NON_BF,
	BF_STATE_AUTO_MODE = 0xFF,
} BeamformingMode_e;



typedef enum LaPacketType
{
	LA_PACKET_TYPE_NONE,
	LA_PACKET_TYPE_DATA_MANAGEMENT,
	LA_PACKET_TYPE_DATA,
    LA_PACKET_TYPE_MANAGEMENT,
	LA_PACKET_TYPE_MU_DATA,
    LA_PACKET_TYPE_OPTIONS_MAX = 0xFF,
} LaPacketType_e;


typedef struct _UMI_BF_MODE_CONFIG
{
	uint8		bfMode; //values from BeamformingMode_e
	uint8       Status;
	uint8		getSetOperation; 
	uint8		reserved;
} __MTLK_PACKED UMI_BF_MODE_CONFIG;

typedef enum OfdmaBeamformingMode
{
	OFDMA_SOUNDING_OFF,
	OFDMA_SOUNDING_ON,
	OFDMA_SOUNDING_DYNAMIC,
	OFDMA_SOUNDING_OPTIONS_MAX = 0xFF
}OfdmaBeamformingMode_e;

typedef struct _UMI_MU_OFDMA_BF_CONFIG
{
	uint32	bfPeriod;
	uint8	getSetOperation; 
	uint8	muBfMode; // values from OfdmaBeamformingMode_e
	uint8 	Status;
	uint8	reserved[1];

} __MTLK_PACKED UMI_MU_OFDMA_BF_CONFIG;

#define OFDMA_BF_PERIOD_MIN 100000 // value in msecs
#define OFDMA_BF_PERIOD_MAX 1000000 // value in msecs

#define BW_MIN 0
#define BW_MAX 4

#define PHY_MODE_MIN 0
#define PHY_MODE_MAX 7

#define NSS_MIN 0
#define NSS_MAX 4

#define MCS_MIN 0
#define MCS_MAX 32

#define CP_MODE_MIN 0
#define CP_MODE_MAX 5

typedef struct _UMI_FIXED_RATE_CONFIG 
{ 
	uint16 stationIndex;
	uint8 isAutoRate;
	uint8 bw;
	uint8 phyMode; 
	uint8 nss; 
	uint8 mcs; 
	uint8 cpMode;
	uint8 dcm;
	uint8 heExtPartialBwData;
	uint8 heExtPartialBwMng;
	uint8 changeType; //values from LaPacketType_e
	uint8 Status;
	uint8 Reserved[3];
} __MTLK_PACKED UMI_FIXED_RATE_CONFIG; 


typedef struct _UMI_STOP_TRAFFIC
{
	uint16 		u16SID;
	uint8  		u8Status;
	uint8  		u8Reserved;
} __MTLK_PACKED UMI_STOP_TRAFFIC;


typedef struct _UMI_BEACON_SET
{
	uint32 u32hostAddress;
	uint16 u16part1Len;
	uint16 u16part2Len;
	uint16 u16part3Len;  
	uint16 u16part4Len;  
	uint16 u16part5Len;	
	uint16 u16part6Len;  
	uint8  u8vapIndex;
	uint8  u8Status;
	uint8  addBssLoadIe;
	uint8  u8Reserved;
} __MTLK_PACKED UMI_BEACON_SET;

/***************************************************************************
**
** 	NAME:         UMI_RADAR_DETECTION_RSSI_TH_CONFIG
**
****************************************************************************/

#define RADAR_DETECTION_RSSI_TH_DEFAULT    (-64)
#define RADAR_DETECTION_RSSI_TH_MIN        (-128)
#define RADAR_DETECTION_RSSI_TH_MAX        (127)

typedef struct _UMI_RADAR_DETECTION_RSSI_TH_CONFIG
{
    int32  radarDetectionRssiTh;
	uint8  getSetOperation;
	uint8  Status;
	uint8  reserved[2];
} __MTLK_PACKED UMI_RADAR_DETECTION_RSSI_TH_CONFIG;

/***************************************************************************
**
** 	NAME:         UMI_SET_MU_STATIC_PLAN_MANAGER_REQ
**
****************************************************************************/

typedef enum
{
    UMI_DL_MU_MIMO,
	UMI_DL_MU_OFDMA,
    UMI_UL_MU_OFDMA
} UmiMuPlanId_e;

typedef enum
{
    UMI_BC_BAR,
    UMI_MU_UC_BAR,
    UMI_UC_PRE_DATA_TF,
} UmiMuPlanTriggerMethod_e;

typedef enum
{
    UMI_MU_PROTECTION_PHASE,
	UMI_MU_ACTION_PHASE,
    UMI_MU_SOUNDING_PHASE,
    UMI_MU_DL_DATA_PHASE,
    UMI_MU_UL_DATA_PHASE,
    UMI_MU_NFRP_PHASE,
    UMI_MU_INVALID_PHASE
} UmiMuPlanPhase_e;



#define	HE_MU_MAX_NUM_OF_GROUPS 					(16)
#define	HE_MU_MAX_NUM_OF_USERS_PER_GROUP 			(16)

#define INVALID_SID_FOR_HE_GROUP					(0x1FF)
#define HE_MU_GROUP_SET 							(1)
#define HE_MU_GROUP_RESET 							(0)

#define STATIC_PLAN_MANAGER_MAX_NUM_OF_MU_USERS 	(8)


typedef struct _UMI_MU_PLAN_COMMON_CONFIGURATION
{	
/*	MU-Plan common */
	 /*TXOP parameters*/
	 uint16 	maxTxopDuration;
	 uint8		startBwLimit;
	 uint8		muSequenceType;//0 - MU-BAR , 1- VHT like  (applicable for DL only ) 
	 
/*MU-plan  per phase */
	 /*phase common parameters*/
	 uint16 	maximumPpduTransmissionTimeLimit;
	 uint8		phaseFormat; //(phase type)   UL/DL data  
	 uint8		muType; //	Mimo / ofdma 
	 
	 uint8		numberOfPhaseRepetitions;
	 uint8	 	numOfParticipatingStations;
	 uint8	 	rfPower;
	 uint8		dl_HeCp;
	 
	 uint8		dl_HeLtf;
	 uint8		ul_HeCp; 
	 uint8		ul_HeLtf; 
	 uint8		tf_heGI_and_Ltf;
/*Trigger frame */
	/*Trigger frame common parameters*/
	 uint16 	tf_Length; 
	 uint8		tf_psdu_rate;
/*Rcr*/
	 uint8		rcr_Stbc;

	 uint16		rcr_heSigASpatialReuse; 
/*  Operation mode */
	 uint8		planOnOff; 
	 uint8		multiplexingFlag;
/*NFRP params*/	 
 	 uint16 	startingAid;
	 uint8		feedbackType;
	 uint8		reserved;
} __MTLK_PACKED UMI_MU_PLAN_COMMON_CONFIGURATION;
  


typedef struct _UMI_MU_PLAN_PER_USER_CONFIGURATION
{	
	/*Per user phase  parameters */
	uint16		tid_alloc_bitmap;
	uint8		uspStationIndexes;
	uint8		dl_PsduRatePerUsp;
	
	uint8		ul_PsduRatePerUsp;	
	uint8		dl_bfType;
	uint8		dl_subBandPerUsp;
	uint8		dl_startRuPerUsp;

	uint8		dl_ruSizePerUsp;
	/*Trigger frame RCR per User*/ 
	uint8		tfStartingSS;
	uint8		tfMpduMuSpacingFactor;
	uint8		tfPadding; 
	
	uint8		targetRssi;
	uint8	 	ul_ldpc;
	uint8	 	ul_psduRate;
	uint8	 	ul_SubBand;
	
	uint8	 	ul_StartRU;
	uint8	 	ul_ruSize;
	uint8	 	SsAllocation;
	uint8	 	codingType_BCC_OR_LDPC;
 }__MTLK_PACKED UMI_MU_PLAN_PER_USER_CONFIGURATION;


 typedef struct _UMI_STATIC_PLAN_CONFIG
 {	 
	UMI_MU_PLAN_COMMON_CONFIGURATION		commonSection;
	UMI_MU_PLAN_PER_USER_CONFIGURATION 		perUserParameters[STATIC_PLAN_MANAGER_MAX_NUM_OF_MU_USERS];
	uint8 Status;
	uint8 Reserved[3];
 } __MTLK_PACKED UMI_STATIC_PLAN_CONFIG;

typedef struct __UMI_DBG_HE_MU_GROUP_STATS 
{
	uint8 	groupId;
	uint8 	planType;
	uint8 	vapId;
	uint8 	setReset;
	uint16 	stationId[HE_MU_MAX_NUM_OF_USERS_PER_GROUP];
	uint8   Status;
	uint8   Reserved[3];
} __MTLK_PACKED UMI_DBG_HE_MU_GROUP_STATS;

/*Dynamic MU configurations*/
typedef struct _UMI_DYNAMIC_MU_TYPE
{
	uint8 dlMuType;
	uint8 ulMuType;
	uint8 minStationsInGroup;
	uint8 maxStationsInGroup;
	uint8 cdbConfig;
	uint8 getSetOperation;
	uint8 Status;
	uint8 reserved;	
} __MTLK_PACKED UMI_DYNAMIC_MU_TYPE;

typedef struct _UMI_HE_MU_FIXED_PARAMTERS
{
	uint8 muSequence;
	uint8 ltf_GI;
	uint8 codingType;
	uint8 heRate;
	uint8 getSetOperation;
	uint8 reserved[3];
} __MTLK_PACKED UMI_HE_MU_FIXED_PARAMTERS;

typedef struct _UMI_HE_MU_DURATION
{
	uint16 	PpduDuration;
	uint16 	TxopDuration;
	uint16 	TfLength;
	uint8 	NumberOfRepetitions;
	uint8  	getSetOperation;
} __MTLK_PACKED UMI_HE_MU_DURATION;

typedef struct _UMI_GET_TWT_PARAMS
{
	uint64	individualWakeTime;
	uint32	individualWakeInterval;
	uint16	individualMinWakeDuration;
	uint16  staId;
	uint8 	isImplicit;
	uint8   isAnnounced;
	uint8   isTriggerEnabled;
	uint8 	agreementType;
	uint8	individualChannel;
	uint8   BroadcastTargetBeacon;
    uint8   BroadcastListenInterval;
	uint8   reserved;
} __MTLK_PACKED UMI_GET_TWT_PARAMS;

typedef struct _UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS
{
	uint8	setEtsiPpduDuration;	/* TRUE: set PPDU duration limits according to ETSI limitations. FALSE: set as default values   */
	uint8	reserved[3];
} __MTLK_PACKED UMI_SET_ETSI_PPDU_DURATION_LIMITS_REQ_PARAMS;

#define FORCE_BANDS_MIN	0
#define FORCE_BANDS_MAX	1
#define CCA_OVER_MIN	0
#define CCA_OVER_MAX	1
#define CCA_BITMAP_MIN	0
#define CCA_BITMAP_MAX	MAX_UINT8

typedef struct _UMI_PREAMBLE_PUNCT_CCA_OVERRIDE
{
	uint8	ccaOverride;  	//-- TRUE - Force HW to use ccaBitmap as CCA vector. FALSE - Release HW to calculate CCA dynamically.
	uint8  	ccaBitmap; 		//-- CCA 8 bits vector. 8X20MHz band, of 160MHz.  1 - set band as busy. 0 - set band as free
	uint8 	forceFreeBands; //-- TRUE - Force all bitmap of the bands, including the free bands. FALSE - Force only busy bands.
	uint8	Status;			//-- Message return status
} __MTLK_PACKED UMI_PREAMBLE_PUNCT_CCA_OVERRIDE;

/* Broadcast TWT */
#define MIN_NUM_OF_BC_TWT_SP_TO_ADD   (1)
#define MAX_NUM_OF_BC_TWT_SP_TO_ADD   (8)
#define MIN_WAKE_DURATION_UNIT_256US  (0)
#define MAX_WAKE_DURATION_UNIT_1024US (1)
#define MIN_WAKE_INTERVAL_MANTISSA	  (0)
#define MAX_WAKE_INTERVAL_MANTISSA	  (MAX_UINT16)
#define MIN_TRIGGER_UNTRIGGERED		  (0)
#define MAX_TRIGGER_TRIGGERED	  	  (1)
#define MIN_FLOW_TYPE_ANNOUNCED		  (0)
#define MAX_FLOW_TYPE_UNANNOUNCED	  (1)
#define MIN_BROADCAST_TWT_ID		  (0)
#define MAX_BROADCAST_TWT_ID		  (31)
#define MIN_WAKE_INTERVAL_EXP		  (0)
#define MAX_WAKE_INTERVAL_EXP		  (31)
#define MIN_WAKE_DURATION			  (0)
#define MAX_WAKE_DURATION			  (MAX_UINT8)

typedef struct _UMI_BC_TWT_SP_PARAMS
{
	uint16 twtWakeIntervalMantissa;
	uint8 trigger;
	uint8 flowType;
	uint8 broadcastTwtId;
	uint8 wakeIntervalExponent;
	uint8 twtWakeDuration;
	uint8 reserved;
} __MTLK_PACKED UMI_BC_TWT_SP_PARAMS;

typedef struct _UMI_ADVERTISE_BC_TWT_SP
{
	uint8 numberOfSpsToAdd;
	uint8 wakeDurationUnit; 
	uint8 status; 
	uint8 getSetOperation;
	UMI_BC_TWT_SP_PARAMS bcTwtSpParams[MAX_NUM_OF_BC_TWT_SP_TO_ADD];
} __MTLK_PACKED UMI_ADVERTISE_BC_TWT_SP;

#define MIN_TWT_PERSISTENCE	(1)
#define MAX_TWT_PERSISTENCE (MAX_UINT8)

typedef struct _UMI_TERMINATE_BC_TWT_SP
{
	uint8 broadcastTwtPersistence;
	uint8 status;
	uint8 reserved[2];
} __MTLK_PACKED UMI_TERMINATE_BC_TWT_SP;

#define MIN_ALL_TWT					  (0)
#define MAX_ALL_TWT 				  (1)
#define MIN_AGREEMENT_TYPE_INDIVIDUAL (1)
#define MAX_AGREEMENT_TYPE_BROADCAST  (2)
#define MIN_INDIVIDUAL_FLOW_ID		  (0)
#define MAX_INDIVIDUAL_FLOW_ID		  (7)

typedef struct _UMI_SEND_TWT_TEARDOWN_FRAME
{
	uint16 staId;
	uint8 alltwt;
	uint8 agreementType; 	   // 1 for Individual Teardown frame, 2 for Broadcast Teardown frame. N/A when allTwt == 1
	uint8 broadcastIdOrFlowId; // Flow ID for Individual Teardown frame,  Broadcast ID for broadcast Teardown frame. N/A when allTwt == 1
	uint8 status;
	uint8 reserved[2];
} __MTLK_PACKED UMI_SEND_TWT_TEARDOWN_FRAME;

/* Trigger frame*/

typedef struct _UMI_TRIGGER_FRAME_COMMON_SECTION
{
	uint16 duration;
	uint16 length;
	uint16 startAID;
	uint8 bw;
	uint8 cpAndLtf;
	uint8 muMimoLtfMode;
	uint8 nHeLtf;
	uint8 stbc;
	uint8 ldpcExtraSymbol;
	uint8 aFactor;
	uint8 peDisambiguty;
	uint8 multiplexingFlag;
	uint8 feedbackType;
} __MTLK_PACKED UMI_TRIGGER_FRAME_COMMON_SECTION;


typedef struct _UMI_TRIGGER_FRAME_PER_USER_SECTION
{
	IEEE_ADDR	 staMacAddr;
	uint8 isDummyStation;
	uint8 ruAllocation;
	uint8 codingType;
	uint8 mcs;
	uint8 ssAllocationLowestSs;
	uint8 ssAllocationNss;
	uint8 tidAggLimit;
	uint8 acPrefferedLevel;
	uint8 reserved[2];
} __MTLK_PACKED UMI_TRIGGER_FRAME_PER_USER_SECTION;
 
 
typedef struct _UMI_TRIGGER_FRAME_CONFIGURABLE_FIELDS
{ 	
	UMI_TRIGGER_FRAME_COMMON_SECTION 	commonSection;
	UMI_TRIGGER_FRAME_PER_USER_SECTION 	perUserSection[MAX_NUM_OF_TRIGGER_FRAME_STATIONS];
} __MTLK_PACKED UMI_TRIGGER_FRAME_CONFIGURABLE_FIELDS;
 
 
 
typedef struct _UMI_TRIGGER_FRAME_CONFIG 
{ 
	uint8 enableTriggerFrameTx;
	uint8 vapId;
	uint8 numOfParticipatingStations;
	uint8 isProtectionRequired;
	uint8 isMultiUserTx;
	uint8 reserved[3];
	uint32 silenceTimer;
	uint32 numOfTriggerFrameCycles;
	UMI_TRIGGER_FRAME_CONFIGURABLE_FIELDS triggerFrameConfigurableFields;
} __MTLK_PACKED UMI_TRIGGER_FRAME_CONFIG; 

 typedef struct _UMI_SET_RTS_THRESHOLD
 {
	 uint16 newRtsThreshold;
	 uint8	vapId;
	 uint8	isDisabled;
	 uint8	getSetOperation;
	 uint8	Status;
	 uint8	reserved[2];
 } __MTLK_PACKED UMI_SET_RTS_THRESHOLD;

 /***************************************************************************
 **
 ** NAME		 UM_MAN_WARNING_DETECTED_IND
 **
 ** PARAMETERS	 None
 **
 ** DESCRIPTION  A message from FW the driver, 
 ** 			 Describes the detected warning - which layer triggered and from what reason
 **
 ****************************************************************************/

typedef enum {
	WHM_HOSTAPD = 1,
	WHM_DRIVER,
	WHM_FW,
	WHM_IW,	
	WHM_PHY
}WhmEventLayer_e;

typedef enum {
	FW_WARNING_NOT_HANDLED = 0,
	FW_WARNING_HANDLED,
	DONT_CARE,	
}WhmFwWarningHandling_e;

typedef struct _UMI_WHM_ENABLE
{
	uint8	whmEnable;
	uint8	fwWarningsEnable;
	uint8	Status;
	uint8	phyWarningsEnable;	
} __MTLK_PACKED UMI_WHM_ENABLE;

typedef struct _UMI_WHM_WARNING_DETECTION
{
 	uint8 whmRaisedByLayer; //	WhmEventLayer_e
	uint8 whmWarningId;
	uint8 reserved[2];
} __MTLK_PACKED UMI_WHM_WARNING_DETECTION; 

typedef struct _UMI_WHM_HW_LOGGER_ENABLE
{
	uint8	fwWarningHandlingStatus; //	WhmFwWarningHandling_e
	uint8	enableHwLogger;
	uint8	Status;
	uint8	reserved[1];
} __MTLK_PACKED UMI_WHM_HW_LOGGER_ENABLE; 

 /***************************************************************************
**
** frame class error section
**              
*****************************************************************************/
#define MAX_NUMBER_FRAME_CLASS_ERROR_ENTRIES_IN_MESSAGE 8

typedef struct _UMI_FRAME_CLASS_ERROR_ENTRY
{
	uint8  u8vapIndex;
	uint8  u8Reserved;
	IEEE_ADDR	sAddr;
} __MTLK_PACKED UMI_FRAME_CLASS_ERROR_ENTRY;

typedef struct _UMI_FRAME_CLASS_ERROR
{
	UMI_FRAME_CLASS_ERROR_ENTRY	frameClassErrorEntries[MAX_NUMBER_FRAME_CLASS_ERROR_ENTRIES_IN_MESSAGE];
	uint8 u8numOfValidEntries;
	uint8  u8Reserved[3];
} __MTLK_PACKED UMI_FRAME_CLASS_ERROR;


/***************************************************************************
**
** TKIP MIC failure section
**              
*****************************************************************************/
#define MAX_NUMBER_TKIP_MIC_FAILURE_ENTRIES_IN_MESSAGE 8


typedef struct _UMI_TKIP_MIC_FAILURE_ENTRY
{
	uint16	stationId;
	uint8	isGroupKey;
	uint8	reserved;
} __MTLK_PACKED UMI_TKIP_MIC_FAILURE_ENTRY;



typedef struct _UMI_TKIP_MIC_FAILURE
{
  uint8 	u8numOfValidEntries;
  uint8  	u8Reserved[3];
  UMI_TKIP_MIC_FAILURE_ENTRY  micFailureEntry [MAX_NUMBER_TKIP_MIC_FAILURE_ENTRIES_IN_MESSAGE];
} __MTLK_PACKED UMI_TKIP_MIC_FAILURE;


/***************************************************************************
**
** NAME         UM_MAN_SET_DEFAULT_KEY_INDEX_REQ
**
** PARAMETERS   u16Status           UMI_OK
**                                  UMI_NOT_INITIALIZED
**                                  UMI_STATION_UNKNOWN
**              u16SID              Station ID
**              u16KeyType          Pairwise or group key
**              u16KeyIndex         Default Key Index
**
** DESCRIPTION  Set default key index
**
****************************************************************************/
#ifdef CPU_ARC
#define  KEY_INDEX_MIN  0
#if defined (ENET_INC_ARCH_WAVE600D2)
#define  KEY_INDEX_MAX  7
#else
#define  KEY_INDEX_MAX  3
#endif //ENET_INC_ARCH_WAVE600D2
#endif //CPU_ARC

typedef struct _UMI_DEFAULT_KEY_INDEX
{
	uint16	u16Status;
	uint16	u16SID;
	uint16	u16KeyType;
	uint16	u16KeyIndex;
} __MTLK_PACKED UMI_DEFAULT_KEY_INDEX;


/***************************************************************************
**
** NAME         UM_MAN_SET_802_1X_FILTER_REQ
**
** PARAMETERS   u16Status	UMI_OK
**                                 		UMI_NOT_INITIALIZED
**                                  		UMI_STATION_UNKNOWN
**              u16SID			Station ID
**              u8IsFilterOpen          	0- 802.1x filter is enabled 1 - disabled
**
** DESCRIPTION  Set or remove 802.1x filter.
**
****************************************************************************/
#ifdef CPU_ARC
#define STATION_ID_MIN 0
#define STATION_ID_MAX 255
#endif //CPU_ARC

typedef struct _UMI_802_1X_FILTER
{
	uint16	u16Status;
	uint16	u16SID;
	uint8	u8IsFilterOpen;
	uint8	u8Reserved[3];
} __MTLK_PACKED UMI_802_1X_FILTER;

/***************************************************************************
**
** 	NAME         _UMI_POLL_CLIENT
**
****************************************************************************/

typedef struct _UMI_POLL_CLIENT
{
	uint16	stationIndex;
	uint8  	isActive; // station activity status 
	uint8	Status;
} __MTLK_PACKED UMI_POLL_CLIENT;


/***************************************************************************
**
** 	NAME         UMI_ADD_STREAM_REQ
**
****************************************************************************/
#ifdef CPU_ARC
#define BUFFER_THRESHOLD_ID_MIN 0x0
#define BUFFER_THRESHOLD_ID_MAX 0xFFFF

#define ASSIGNED_FIFOID_MIN  0x0
#define ASSIGNED_FIFOID_MAX  0x0FFFFFFF

#define STREAM_ID_MIN  0
#define STREAM_ID_MAX  0
#endif //CPU_ARC

typedef struct _UMI_ADD_STREAM_REQ 
{
	uint32 swPreCalcChecksum; 
	uint32 udpHeader[UDP_HEADER_WORD_SIZE];  // 11 words UDP header
	uint32 assignedFifosId; /*28 logger fifos */ 
	uint16 u16Status;
	uint16 bufferThreshold;
	uint16 hwModuleFifo[LOGGER_NUM_OF_HW_FIFOS];
	uint8 streamId;
	uint8 wlanIf;
	uint8 logVersion; 
	uint8 be0;
	uint8 be1; 
	uint8 loggerActiveMode; // LogAgentStateActive_e
	uint8 reserved [2];
} __MTLK_PACKED UMI_ADD_STREAM_REQ;

/***************************************************************************
**
** 	NAME         UMI_REMOVE_STREAM_REQ
**
****************************************************************************/

typedef struct _UMI_REMOVE_STREAM_REQ 
{
	uint16 u16Status;
	uint16 streamId;
} __MTLK_PACKED UMI_REMOVE_STREAM_REQ;


/***************************************************************************
**
** 	NAME         UMI_ADD_FILTER_REQ
**
****************************************************************************/

#ifdef CPU_ARC
#define FIFO_BITMAP_MIN  0x0
#define FIFO_BITMAP_MAX  0x0FFFFFFF

#define LOG_LEVEL_MIN    0x0
#define LOG_LEVEL_MAX    0x7
#endif //CPU_ARC

typedef struct _UMI_ADD_FIFO_FILTER_REQ 
{	
	uint32 fifosBitmap;
	uint16 u16Status; 
	uint8 FilterType; // 0 - log level, 1- module filte
	uint8 logLevel;
} __MTLK_PACKED UMI_ADD_FIFO_FILTER_REQ;


/***************************************************************************
**
** 	NAME         UMI_REMOVE_FILTER_REQ
**
****************************************************************************/

typedef struct _UMI_REMOVE_FIFO_FILTER_REQ 
{	
	uint32 	fifosBitmap;
	uint16 	u16Status; 
	uint8 	FilterType; // 0 - log level, 1- module filte
	uint8 	logLevel;
} __MTLK_PACKED UMI_REMOVE_FIFO_FILTER_REQ;


typedef struct _UMI_SET_WMM_PARAMETERS
{	
	UMI_AC_WMM_PARAMS wmmParamsPerAc[MAX_USER_PRIORITIES]; 
	uint8	vapId;
	uint8  	u8Status;
	uint8  	reserved [2];
} __MTLK_PACKED UMI_SET_WMM_PARAMETERS;


/***************************************************************************
**
** 	NAME         UMI_SET_TRIGGER_REQ
**
****************************************************************************/

typedef struct UMI_SET_TRIGGER_REQ 
{
	uint32 messageHeaderFirst4Bytes; 
	uint16 messageHeaderLast2Bytes; //last 16 bits of the message header 
	uint16 u16Status; 
	uint32 messageHeaderMaskFirst4Bytes;
	uint16 messageHeaderMaskLast2Bytes; //last 16 bits of the message header
	uint8 u8TriggerType; // 0x0 - Start trigger, 0x1 - stop trigger
	uint8 reserved;  
} __MTLK_PACKED UMI_SET_TRIGGER_REQ; 

/***************************************************************************
**
** 	NAME         UMI_SET_TRIGGER_REQ
**
****************************************************************************/
typedef struct UMI_RESET_TRIGGER_REQ 
{
	uint16 u16Status; 
	uint8 u8TriggerType; // 0x0 - Start trigger, 0x1 - stop trigger
	uint8 reserved;  
} __MTLK_PACKED UMI_RESET_TRIGGER_REQ; 


/***************************************************************************
**
** 	NAME         UMI_SET_PHY_HDR_REQ
**
****************************************************************************/
typedef struct _UMI_SET_PHY_HDR_REQ
{
	uint32 phyFifoFirstHeaderWord;
	uint32 phyFifoMessageLength;
	uint16 phyFifoSecondHeaderWord; 
	uint16 u16Status;
} __MTLK_PACKED UMI_SET_PHY_HDR_REQ; 

/***************************************************************************
**
** 	NAME         AFE calibration data
**
****************************************************************************/
#define AFE_CALIBRATION_DATA_SIZE_GEN5	11
#define AFE_CALIBRATION_DATA_SIZE_GEN6	19
#define MAX_AFE_CALIBRATION_DATA_SIZE	AFE_CALIBRATION_DATA_SIZE_GEN6

typedef struct _AFE_CALIBRATION_DATA
{
	uint8	calibrationData[MAX_AFE_CALIBRATION_DATA_SIZE];
	uint8	Status;
} __MTLK_PACKED AFE_CALIBRATION_DATA;

/***************************************************************************
**
** 	NAME         AFE calibration data
**
****************************************************************************/
#define RFIC_CIS_MAX_SIZE	22

#ifdef CPU_ARC
#define RFIC_CIS_SIZE_MIN  1
#define RFIC_CIS_SIZE_MAX  22
#endif //CPU_ARC

typedef struct _RFIC_CALIBRATION_DATA
{
	uint8	rficCisSize;
	uint8	rficCis[RFIC_CIS_MAX_SIZE];
	uint8	Status;
} __MTLK_PACKED RFIC_CALIBRATION_DATA;

/***************************************************************************
**
** 	NAME         UMI_ENABLE_RADIO
**
****************************************************************************/

#define DISABLE_RADIO 0
#define ENABLE_RADIO  1
#define ERP_RADIO_STATE  2


typedef struct _UMI_ENABLE_RADIO
{
	uint32 u32RadioOn;
	uint8 Status;
	uint8 Reserved[3];
} __MTLK_PACKED UMI_ENABLE_RADIO;

/***************************************************************************
**
** NAME         UMI_AGG_RATE_LIMIT
**
** PARAMETERS :
**				mode     - enabled(1) or disabled (0)
**				maxRate - maximum rate for feature
**				status    - return status
**
** DESCRIPTION 
**              
*****************************************************************************/

typedef struct _UMI_AGG_RATE_LIMIT
{
	uint16 	maxRate;
	uint8 	mode;
	uint8 	getSetOperation;
	uint8	Status;
	uint8	reserved[3];
} __MTLK_PACKED UMI_AGG_RATE_LIMIT;

/***************************************************************************
**
** NAME         UMI_RX_TH
**
** PARAMETERS   
**
**	value - high reception threshold value.
**
** DESCRIPTION  
**              
*****************************************************************************/
#ifdef CPU_ARC
#define RX_TH_VALUE_MIN -128
#define RX_TH_VALUE_MAX  127
#endif //CPU_ARC

typedef struct _UMI_RX_TH
{
	uint8 getSetOperation;
	int8  rxThValue;
	uint8 Status;
	uint8 reserved;
} __MTLK_PACKED UMI_RX_TH;

/***************************************************************************
**
** NAME         UMI_RX_DUTY_CYCLE
**
** PARAMETERS   
**
**	onTime - time interval of high reception threshold
**	offTime - time interval of low reception threshold
**
** DESCRIPTION  
**              
*****************************************************************************/
#ifdef CPU_ARC
#define ON_TIME_MIN  (0)
#define ON_TIME_MAX  (0x7FFF)

#define OFF_TIME_MIN (0)
#define OFF_TIME_MAX (0x7FFF)
#endif //CPU_ARC

typedef struct _UMI_RX_DUTY_CYCLE
{
	uint32 onTime;
	uint32 offTime;
	uint8  getSetOperation;
	uint8  Status;
	uint8  reserved[2];
} __MTLK_PACKED UMI_RX_DUTY_CYCLE;

/***************************************************************************
**
** NAME
**
** DESCRIPTION     Empty Message, used when message has no abData
**
****************************************************************************/
typedef uint8 EmptyMsg;



/***************************************************************************
**
** 	NAME:         UM_MAN_SET_MULTICAST_MODE_REQ
**
****************************************************************************/

#define DISABLE_RELIABLE_MULTICAST	(0)
#define ENABLE_RELIABLE_MULTICAST	(1)

typedef struct _UMI_MULTICAST_MODE
{
	uint8 getSetOperation;
	uint8 u8Action;       // DISABLE_RELIABLE_MULTICAST / ENABLE_RELIABLE_MULTICAST
	uint8 u8VapID;		  // VAP Index
	uint8 Status;
} __MTLK_PACKED UMI_MULTICAST_MODE;


/***************************************************************************
**
** 	NAME:         UM_MAN_MULTICAST_ACTION_REQ
**
****************************************************************************/

#define ADD_STA_TO_MULTICAST_GROUP				(0)
#define REMOVE_STA_FROM_MULTICAST_GROUP			(1)
#define REMOVE_STA_FROM_ALL_MULTICAST_GROUPS	(2)

#ifdef CPU_ARC
#define GROUP_ID_MIN  1
#define GROUP_ID_MAX  31
#endif //CPU_ARC

typedef struct _UMI_MULTICAST_ACTION
{
	uint16 u16StaID;	// Station Index
	uint8  u8Action;	// Join / Leave / Leave all Groups
	uint8  u8GroupID;	// Multicast Group Index
	uint8  Status;
	uint8  reserved[3];
} __MTLK_PACKED UMI_MULTICAST_ACTION;


typedef struct _UMI_UPDATE_ADMISSION_CAPACITY
{
    uint32 	availableAdmissionCapacity;
	uint8	getSetOperation;
	uint8 	Status;
	uint8	reserved[2];
} __MTLK_PACKED UMI_UPDATE_ADMISSION_CAPACITY;

/***************************************************************************
**
** 	NAME:         UM_MAN_GET_TEMPERATURE
**
****************************************************************************/

#define NUM_OF_ANTS_FOR_TEMPERATURE 3

typedef struct _UMI_HDK_USER_DEMAND
{
	int32	temperature;
	uint32  calibrateMask;
} __MTLK_PACKED UMI_HDK_USER_DEMAND;

/***************************************************************************
**
** 	NAME:         UMI_MAN_CCA_TH
**
****************************************************************************/
#ifdef CPU_ARC
#define PRIMARY_CH_CCA_MIN -128
#define PRIMARY_CH_CCA_MAX  127

#define SECONDARY_CH_CCA_MIN -128
#define SECONDARY_CH_CCA_MAX  127

#define SEC_40_CCA_TH_MIN -128
#define SEC_40_CCA_TH_MAX  127

#define SEC_80_CCA_TH_MIN -128
#define SEC_80_CCA_TH_MAX  127
#endif //CPU_ARC

typedef struct _UMI_CCA_TH
{
	int8 primaryChCCA;
	int8 secondaryChCCA;
	int8 midPktPrimaryCCA; // Wave500 only, reserved on Wave600
union {
		int8 midPktSecondary20CCA; // Wave500
		int8 sec40CcaTh;           // Wave600
		};
union {
		int8 midPktSecondary40CCA; // Wave500
		int8 sec80CcaTh;           // Wave600
		};
	uint8 Status;
	int8  reserved[2];
} __MTLK_PACKED UMI_CCA_TH;

/***************************************************************************
**
** 	NAME:         UMI_MAN_SSB_MODE
**
****************************************************************************/

typedef struct _UMI_SSB_MODE
{
	uint8 getSetOperation;
	uint8 enableSSB;
	uint8 SSB20Mode;
	uint8 Status;
} __MTLK_PACKED UMI_SSB_MODE;


typedef struct _UMI_ERP_SET
{
	uint32 staConnectionTimeInSeconds;
	uint32 radioOffTimeInMsecs;
	uint32 radioOnTimeInMsecs;
	uint16 numberOfStationThreshold;
	uint8  isErpEnable;
	uint8  Status;
} __MTLK_PACKED UMI_ERP_SET;

typedef struct _UMI_DC_SET
{
	uint16 radioOffTimeInMsecs;
	uint16 radioOnTimeInMsecs;
	uint8  isDutyCycleEnable; // 0 false -1 true
	uint8  isDutyCycleForce;
	uint8  reserved[2];
} __MTLK_PACKED UMI_DC_SET;


/***************************************************************************
**
** 	NAME:         UMI_MAN_BEACON_BLOCKED
**
****************************************************************************/

typedef struct _UMI_BEACON_BLOCK
{
	uint8  beaconBlock;// 0 = not blocked, 1 = blocked
	uint8  reserved[3];
} __MTLK_PACKED UMI_BEACON_BLOCK;


/***************************************************************************
**
** 	NAME:         UMI_MAN_BEACON_BLOCKED_INTERVAL
**
****************************************************************************/

typedef struct _UMI_BEACON_BLOCK_TIMER_INTERVAL
{
    uint32 initial; /* blocked to unblocked */
    uint32 iterative; /* unblocked to blocked */
	uint8  getSetOperation;
	uint8  Status;
	uint8  reserved[2];
} __MTLK_PACKED UMI_BEACON_BLOCK_TIMER_INTERVAL;

/***************************************************************************
**
** 	NAME:         UMI_PVT
**
****************************************************************************/
typedef struct _UMI_PVT
{
	uint32 voltage;
	int32  temperature;
} __MTLK_PACKED UMI_PVT;

/***************************************************************************
**
** 	NAME:         UMI_PIE_CONTROL
**
****************************************************************************/
#define PIE_NUM_OF_CONVERSIONS			11						/* Floating value to Discrete value conversion: size of table of conversion								*/

typedef struct PieConvertP_s
{
	uint32	dropProbTresh;										/* Determines the value of the denominator	*/	
	uint32	denominator;										/* Divide P by this value to normalize it	*/

}PieConvertP_t;

typedef enum PieUserMode
{
	PIE_USER_MODE_OFF,		
	PIE_USER_MODE_ON,
	PIE_USER_MODE_ON_WITH_DEFAULT_PARAMS,
	PIE_USER_MODE_NUM_OF_MODES,
}PieUserMode_e;

/* PIE Control parameters. Default values are set at init. User may modify via UMI_DATA_PATH_INIT (AKA tunable parameters) */
typedef struct _UMI_PIE_CONTROL
{
	/* word 0 */
	uint32			dropThresh;									/* A constant value below which a packet is enqueued regardless of the queue's latency									*/
	uint32			maxBurst;									/* A constant value indicating the maximum period of time in which packets are not dropped regardless of queue latency. */
	uint32			minQueueSizeBytes;							/* A constant value below which the packet is enqueued. 																*/
	uint32			weight;										/* A constant value defined by the algorithm and is equal to DQ_THRESHOLD / 2^TBD.										*/
	PieConvertP_t	pieConvertP[PIE_NUM_OF_CONVERSIONS];		/* A constant value defined by the algorithm: used to normalize the drop probability									*/
	uint32			decayVal;									/* A constant control parameter of the algorithm defining the decay of drop probability.								*/
	uint32 			minDropProb;								/* The MIN value of drop probability ("0%") 																			*/
	uint32 			maxDropProb;								/* The MAX value of drop probability ("100%")																			*/		
	/* word 29 */
	uint16			alpha;										/* A constant control parameter of the algorithm expressed in Hz and depends on the value of T_UPDATE.					*/
	uint16			beta;										/* A constant control parameter of the algorithm expressed in Hz and depends on the value of T_UPDATE.					*/	
	/* word 30-31 */
	uint16			dqThreshold[ACCESS_CATEGORY_NUM];			/* A constant defining the number of de-queued PDs required to compute the queue's latency.								*/				
	/* word 32-33 */
	uint16			targetLatency[ACCESS_CATEGORY_NUM];			/* A constant value defined by the algorithm: Target latency of the AC queueus											*/
	/* word 34 */
	uint16			tUpdate;									/* A constant value in 1[uSec] resolution which indicates the periodicity of the Drop Probability Calculation.			*/
	uint8			userMode;									/* Feature enable switch: OFF, ON or ON with default parameters (values from PieUserMode_e)								*/
	uint8			getSetOperation;
}UMI_PIE_CONTROL;

/***************************************************************************
**
** 	NAME:         UMI_TEST_BUS_EN
**
****************************************************************************/

typedef struct _UMI_TEST_BUS_EN
{
	uint32 enable;
} __MTLK_PACKED UMI_TEST_BUS_EN;


#define UMI_TXOP_MODE_DISABLED  (0)
#define UMI_TXOP_MODE_ENABLED   (1)

typedef struct _UMI_SET_TXOP_CONFIG 
{
	uint16 staId;
	uint16 txopDuration;
	uint16 maxNumOfStas;
	uint8  mode; 
	uint8  getSetOperation;
	uint8  Status;
	uint8  reserved[3];
} __MTLK_PACKED UMI_SET_TXOP_CONFIG;

typedef struct _UMI_ENABLE_FREQUENCY_JUMP
{
	uint8  u8FreqJumpOn;
	uint8  Status;
	uint8  Reserved[2];
} __MTLK_PACKED UMI_ENABLE_FREQUENCY_JUMP;

#ifdef CPU_ARC
#define BEACON_INERVAL_MIN 15
#define BEACON_INERVAL_MAX 65535
#endif //CPU_ARC

typedef struct _UMI_BEACON_INTERVAL
{
    uint32 beaconInterval;
    uint32 vapID;
	uint8  Status;
	uint8  Reserved[3];
} __MTLK_PACKED UMI_BEACON_INTERVAL;

/***************************************************************************
**
** 	NAME         UMI_NFRP_CONFIG
**
****************************************************************************/
typedef struct _UMI_NFRP_CONFIG	
{
		uint8	nfrpSupport;
		uint8	nfrpThreshold;
		uint8   Reserved[2];
} __MTLK_PACKED UMI_NFRP_CONFIG;

//Rank1/Rank2 rates mask
#define RANK_TWO_NUMBER_OF_RATES	9
#define RANK_TWO_SHIFT				23
#define RANK_TWO_RATES_MASK			MASK(RANK_TWO_NUMBER_OF_RATES, RANK_TWO_SHIFT, uint32)
#define RANK_ONE_RATES_MASK			~RANK_TWO_RATES_MASK

/***************************************************************************
**
** STA DB defines. Shared with driver.
**              
*****************************************************************************/
//#define DB_UNKNOWN_SID			0xFB

#ifdef CPU_ARC
#define SLOW_PROBING_MASK_MIN 0
#define SLOW_PROBING_MASK_MAX 0x7F
#endif //CPU_ARC

typedef struct _UMI_CONTROL
{
	int8  setAPHighLowFilterBank;
	uint8 Status;
	uint8 reserved[2];
} __MTLK_PACKED UMI_CONTROL;

typedef enum
{
	SET_AP_LOW_CHANNEL = 0,
	SET_AP_HIGH_CHANNEL,
} IREControl_e;

typedef struct _UMI_DISABLE_POWER_ADAPT
{
	uint8 getSetOperation;
	uint8 slowProbingMask;
	uint8 Status;
	uint8 reserved;
} __MTLK_PACKED UMI_DISABLE_POWER_ADAPT;

/***************************************************************************
**
** 	NAME:         _UMI_ATF_QUOTAS
**
****************************************************************************/
#define GRANT_SIZE uint16
#define ATF_MAX_INTERVAL_TIME_MS 10000 
#define ATF_MIN_INTERVAL_TIME_MS 300


typedef enum
{
    ATM_DISTRIBUTION_TYPE_DISABLED = 0,
    ATM_DISTRIBUTION_TYPE_DYNAMIC = 1,
    ATM_DISTRIBUTION_TYPE_STATIC = 2,
    ATM_DISTRIBUTION_TYPE_LAST    
} AtmDistributionType_e;

typedef enum
{
    ATM_ALGORITHM_TYPE_GLOBAL = 0,
    ATM_ALGORITHM_TYPE_WEIGHTED = 1,
    ATM_ALGORITHM_TYPE_LAST
} AtmAlgorithmType_e;

typedef enum
{
    ATM_WEIGHTED_TYPE_PER_STATION = 0,
    ATM_WEIGHTED_TYPE_PER_STATION_PER_AC = 1,
    ATM_WEIGHTED_TYPE_PER_VAP = 2,
    ATM_WEIGHTED_TYPE_PER_VAP_PER_AC = 3,
    ATM_WEIGHTED_TYPE_PER_VAP_PER_STATION = 4,
    ATM_WEIGHTED_TYPE_PER_VAP_PER_STATION_PER_AC = 5,
} AtmWeightedType_e;


typedef enum
{
    ATF_NO_ERRORS = 0,
    ATF_INPUT_IS_INVALID = 1,
    ATF_STATION_IN_GRANT_LIST_IS_NOT_CONNECTED = 2,
    ATF_VAP_IN_GRANT_LIST_IS_NOT_AVAILABLE = 3,
    ATF_MULTIPLE_DATA_IS_DISABLED = 4,
    ATF_NOT_ALL_CONNECTED_STATIONS_RECEIVED_GRANT = 5,
    ATF_NUM_ERROR_CODES,
} AtfErrorCode_e;


typedef struct _UMI_ATF_QUOTAS
{
    //word 0
    uint32 u32interval; // milisec
    //word 1
    uint32 u32freeTime; // milisec
    //word 2    
    uint8 u8AtmAlgorithemType; // AtmAlgorithmType_e
    uint8 u8AtmWeightedType; // AtmWeightedType_e
    uint8 u8nof_vaps;
    uint8 u8error_code; // AtfErrorCode_e
    //word 3  
    uint16 u16nof_sta;
    uint16 u16reserved1;

    //data    
    // for FW: data layout depends on HW_NUM_OF_VAPS and HW_NUM_OF_VAPS
    GRANT_SIZE stationGrant[GEN6_MAX_SID]; // Percentage(0-100)*100
    uint8 AtmDistributionType[GEN6_MAX_VAP]; // AtmDistributionType_e
    uint8 Status;
	uint8 Reserved[3];
}  __MTLK_PACKED UMI_ATF_QUOTAS;


typedef struct _FIXED_POWER
{
	uint8 vapId;
	uint8 stationId;
	uint8 powerVal;
	uint8 changeType;
	uint8 Status;
	uint8 Reserved[3];
} __MTLK_PACKED FIXED_POWER;

typedef struct _OPERATING_MODE 
{	
	uint16	stationId		;	
	uint8	channelWidth	;
	uint8	rxNss			;
} __MTLK_PACKED OPERATING_MODE;

#define DISABLE_FAST_DROP 0
#define ENABLE_FAST_DROP  1

typedef struct _UMI_FAST_DROP_CONFIG_REQ
{
	uint8 getSetOperation;
	uint8 enableFastDrop;
	uint8 Status;
	uint8 reserved;
} __MTLK_PACKED UMI_FAST_DROP_CONFIG_REQ;

typedef struct _UMI_CPU_LOADER_CFM
{
	uint32	upperMac;
	uint32	lowerMac;
	uint32	hostIf;
	uint32	txSender;
	uint32	rxHandler;	
} __MTLK_PACKED UMI_CPU_LOADER_CFM;

typedef enum
{
    UMI_DMR_DISABLED,					/*Dynamic Multicast Rate Disabled, lowest basic rate is used*/
	UMI_DMR_DYNAMIC,			        /*Dynamic Multicast Rate Enabled, All Supported Rates can be used*/
	UMI_DMR_STATIC,			        	/*Dynamic Multicast Rate Enabled, Only rate configured by user can be used*/
} UmiDmrMode_e;

typedef struct _UMI_DMR_CONFIG
{    
	uint8   dmrMode;					/*Dynamic Multicast Rate mode, use one of the UmiDmrMode_e values*/
    uint8   vapId;
    uint8   getSetOperation;    /*Set a new mode or return the current mode in dmrMode var*/
	uint8   status;             /* Pass / Fail */
	uint8   maxRate;            /* user configured max multicast rate, in static thsi will be the used rate, in dynamic it wil lbe the maximum */
    uint8   reserved[3];
} __MTLK_PACKED UMI_DMR_CONFIG;


typedef struct _UMI_FIXED_LTF_AND_GI_REQ
{
	uint8	isAuto;
	uint8	ltfAndGi;
	uint8	getSetOperation;
	uint8	Status;
} __MTLK_PACKED UMI_FIXED_LTF_AND_GI_REQ;

#define REG_DOMAIN_FCC		(0)
#define REG_DOMAIN_ETSI		(1)
#define REG_DOMAIN_JAPAN	(2)
#define REG_DOMAIN_KOREA	(3)
#define REG_DOMAIN_CHINA	(4)
#define REG_DOMAIN_DEFAULT	(0xFF)

typedef struct _UMI_REG_DOMAIN_CONFIG
{
	uint8	regDomainConfig;
	uint8	getSetOperation;
	uint8	Status;
	uint8	reserved;
} __MTLK_PACKED UMI_REG_DOMAIN_CONFIG; 

#ifdef CPU_ARC
#define CUT_OFF_POINT_MIN 0
#define CUT_OFF_POINT_MAX 2
#endif //CPU_ARC

typedef struct _UMI_PROTECTION_RATE_CONFIG
{
	uint8 getSetOperation; 
	uint8 cutoffPoint; 
	uint8 Status;
	uint8 padding;
} __MTLK_PACKED UMI_PROTECTION_RATE_CONFIG;

typedef struct _UMI_SET_RETRY_LIMIT
{
	uint8 getSetOperation; 
	uint8 txRetryLimitMgmt; 
	uint8 Status;
	uint8 txRetryLimitData;
} __MTLK_PACKED UMI_SET_RETRY_LIMIT;

typedef struct _UMI_SET_EXCESSIVE_RETRIES_LIMIT
{
	uint8 getSetOperation;
	uint8 Status;
	uint8 vapId;
	uint8 excessiveRetriesLimit;
} __MTLK_PACKED UMI_SET_EXCESSIVE_RETRIES_LIMIT;

#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* !__MHI_UMI_INCLUDED_H */

