/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/****************************************************************************
 ****************************************************************************
 **
 ** COMPONENT:      Frame definitions
 **
 ** MODULE:         $File: //bwp/enet/demo153_sw/develop/src/common/mhi_frame.h $
 **
 ** VERSION:        $Revision: #3 $
 **
 ** DATED:          $Date: 2004/02/24 $
 **
 ** AUTHOR:         S Sondergaard
 **
 ** DESCRIPTION:    802.11 Frame types
 **
 ** LAST MODIFIED BY:   $Author: jh3 $
 **
 **
 ****************************************************************************
 *
 *  
 *
 *  
 *
 *  
 *
 ****************************************************************************/


#ifndef __MHI_FRAME_INCLUDED
#define __MHI_FRAME_INCLUDED

#include "mtlkbfield.h"
#include "mhi_mib_id.h"
#include "shram.h"
#define   MTLK_PACK_ON
#include "mtlkpack.h"

/***************************************************************************/
/***                  Type definitions                                   ***/
/***************************************************************************/

typedef struct _MHI_FM_CF_PARAMETER_SET
{
    uint8  u8CFPcount;         /* how many DTIMs (inc current frame) before next CFP start */
    uint8  u8CFPperiod;        /* DTIMs between start of CFPs */
    uint16 u16CFPmaxDuration;  /* Max duration of CFP in Kus            */
    uint16 u16CFPDurationRemaining;   /* Remaining time in CFP in Kus  */
    uint8  reserved[2];
} __MTLK_PACKED MHI_FM_CF_PARAMETER_SET;

typedef struct _MHI_FM_DS_PARAMETER_SET
{
    uint8  u8CurrentChannel;
    uint8  reserved[3];
} __MTLK_PACKED MHI_FM_DS_PARAMETER_SET;


typedef struct _MHI_FM_IBSS_PARAMETER_SET
{
    uint16 u16ATIMwindowSize;  /* The ATIM window size in kmicroseconds */
    uint8  reserved[2];
} __MTLK_PACKED MHI_FM_IBSS_PARAMETER_SET;


/*
* WME AC parameter field
*/
typedef struct _MHI_FM_WME_PARAMETERS
{
    uint8   u8AciAifsn;
    uint8   u8LogCWminmax;
    uint16  u16Txop;
} __MTLK_PACKED MHI_FM_WME_PARAMETERS;

typedef struct _MHI_FM_WME_PARAMETER_SET
{
    FM_WME_OUI_AND_VER  sWMEhdr;
    MHI_FM_WME_PARAMETERS   sWMEparams[MAX_USER_PRIORITIES];
} __MTLK_PACKED MHI_FM_WME_PARAMETER_SET;


typedef struct _HT_INFORMATION_ELEMENT
{
    uint16  PrimaryChannel                  :8;
    uint16  SecondaryChOffset               :2;
    uint16  STAChanWidth                    :1;
    uint16  RIFS_Mode                       :1;
    uint16  PSMP_Access                     :1;
    uint16  ServiceIntervalGran             :3;

    uint16  OperatingMode                   :2;
    uint16  NonGfStaPresent                 :1;
    uint16  TransmitBurstLimit				:1; // refer to "9.13.3.4 Transmit burst limit" in 11n standard
    uint16  OBSS_Non_HT_STAsPresent			:1; // Indicates if the use of protection for non-HT STAs by overlapping BSSs is determined to be desirable. 
    uint16  Reserved11bit                   :11;     

    uint16  Reserved6bit                    :6;     
    uint16  DualBeacon						:1;	// refer to "secondary beacon" in "11.1.2.1 Beacon generation in infrastructure networks"
    uint16  DualCTS_Protection              :1;

    uint16  SecondaryBeacon                 :1;
    uint16  L_SIG_TXOP_ProtecFullSupp       :1;
    uint16  PCO_Active                      :1;
    uint16  PCO_Phase                       :1;
    uint16  Reserved4bit                    :4;

    uint16  BasicMSC_Set[8];
} __MTLK_PACKED HT_INFORMATION_ELEMENT;

/*
* Metalink Vendor Specific Action Frame definition
*/

#define ACTION_FRAME_CATEGORY_VENDOR_SPECIFIC 127
/* Metalink OUI (3-byte value).
* See http://standards.ieee.org/regauth/oui/oui.txt 
* for more information.
*/
#define MTLK_OUI_0   0x00
#define MTLK_OUI_1   0x09
#define MTLK_OUI_2   0x86

/* To use with the mtlkbfield.h:
* get from uint32 var: MTLK_U_BITS_GET(var, OUI_VALUE);
* set to   uint32 var: MTLK_U_BITS_SET(var, OUI_VALUE, MTLK_VSAF_OUI);
*/
#define OUI_VALUE          MTLK_BFIELD_INFO(0, 24)

/*
 * MTLK Vendor Specific Action Frame format:
 *
 * |============================|
 * | MTLK VSAF Format Version   |
 * |----------------------------|
 * |       Payload Size         | MTLK VSAF Header
 * |----------------------------|
 * | Number of Items in Payload |
 * |============================|
 * |     Item1 Data Size        |
 * |----------------------------|
 * |         Item1 ID           |
 * |----------------------------|  MTLK VSAF Item1
 * |                            |
 * |        Item1 Data          |
 * |                            |
 * |============================|
 * | ...                        |
 * |============================|
 * |     ItemN Data Size        |
 * |----------------------------|
 * |         ItemN ID           |
 * |----------------------------|  MTLK VSAF ItemN
 * |                            |
 * |        ItemN Data          |
 * |                            |
 * |============================|
 */
typedef struct _MTLK_VS_ACTION_FRAME_PAYLOAD_HEADER
{
	uint32 u32Version;
	uint32 u32DataSize; /* data size (not including this header) */
	uint32 u32nofItems;
} __MTLK_PACKED MTLK_VS_ACTION_FRAME_PAYLOAD_HEADER;


#define CURRENT_VSAF_FMT_VERSION 1

typedef struct _MTLK_VS_ACTION_FRAME_ITEM_HEADER
{
	uint32 u32DataSize;
	uint32 u32ID;
} __MTLK_PACKED MTLK_VS_ACTION_FRAME_ITEM_HEADER;

#define MTLK_VSAF_ITEM_ID_SP  0x00000001 /* Sounding packet          */
#define MTLK_VSAF_ITEM_ID_SPR 0x00000002 /* Sounding packet response */

/*
* VSAF SPR Item Structure:
*
* |============================|
* |       VSAF SPR Header      |
* |============================|-- u32SPDataOffset
* |                            |
* |  Original SP Data Payload  | - Data of Header.u32SPDataSize bytes long
* |                            |
* |============================|-- u32MetricsOffset
* |                            |
* |           Metrics          | - Data of Header.u32MetricsSize bytes long
* |                            |
* |============================|
* 
*/
#define SPR_DATA_VERSION_1 1  //Full R_Metric and Scaling Factor data
#define SPR_DATA_VERSION_2 2  //Effective SNR data

/*******************************************************************************************
* MTLK RF Management common
*******************************************************************************************/
#define LBF_RFM_FIELD_MAT_SHIFT		 4

#define RA_STATS_NULL_VALUE			 0xff

#define MTLK_RF_MGMT_TYPE_OFF           0x00
#define MTLK_RF_MGMT_TYPE_MTLK_ASEL     0x01
#define MTLK_RF_MGMT_TYPE_MTLK_LBF      0x02
#define MTLK_RF_MGMT_TYPE_LBF           0x03
#define MTLK_RF_MGMT_TYPE_AIRGAIN_ASEL  0x04

#define MTLK_HW_TYPE_G2                0x00
#define MTLK_HW_TYPE_G3                0x01
#define MTLK_HW_TYPE_G35               0x02

/*******************************************************************************************
* MTLK ASel Gen2
*******************************************************************************************/
/* The antenna set currently consists of the bit pairs:
*   BITS 0/1 - 1st TX/RX antenna 
*   BITS 2/3 - RX only antenna
*   BITS 4/5 - 2nd TX/RX antenna 
*  where for each antenna we can select only 2 options 01 or 10 (00 and 11 are
*  invalid).
*
*  RX only antenna is irrelevnt here, since MAC uses the specific 
*  Antenna Set we choose for the frame transmissions only. However, we
*  set it to a valid value (10) constantly.
*/
#define MTLK_ASEL_G2_AVAILABLE_ASETS { 0x19 /* 011001 */,  0x1a /* 011010 */,  0x2a /* 101010 */,  0x29  /* 101001 */ }

/*******************************************************************************************
* MTLK LBF Gen3
*******************************************************************************************/
/* For rank1 we can specify the following values (antenna1, antenna2):
*  0,0
*  1,1
*  0,3
*  0,4
* Since we have only 4 bits, we compress these values in the following manner:
*   - antenna1 value (values: 0,1) is passed thru the BIT3
*   - antenna2 value (values: 0,3,4) is passed thru the BIT0-BIT2
* Thus, the table above could be translated to the following values:
*  0x0 - (0,0) - 0000
*  0x9 - (1,1) - 1001
*  0x3 - (0,3) - 0011
*  0x4 - (0,4) - 0100
*
* Also, there is a special case: default CDD value (0xF).
*
* For rank2 we use just index [0...3].
*/

#define MTLK_LBF_G3_RANK1_AVAILABLE_VALUES { 0x0, 0x1, 0x2, 0x3, 0x4 }
#define MTLK_LBF_G3_RANK2_AVAILABLE_VALUES { 0x0, 0x1, 0x2, 0x3 }

#define LBFRank1Val MTLK_BFIELD_INFO(0, 4) /* 4 bits starting BIT0 in u8RFMgmtData field */
#define LBFRank2Val MTLK_BFIELD_INFO(4, 4) /* 4 bits starting BIT4 in u8RFMgmtData field */

typedef struct _MTLK_VSAF_SPR_ITEM_HDR
{
	uint16 u16Version;       // Structure version - SPR_DATA_VERSION
	uint8  u8SPIsCB;         // IsCB of received SP packet
	uint8  u8SPMcs;          // MCS of received SP packet
	uint8  u8Reserved[4];    // Reserved
	uint16 u16SPDataOffset;  // Original SP Data offset calculated from the end of header
	uint16 u16SPDataSize;    // Original SP Data size
	uint16 u16MetricsOffset; // Metrics Data offset calculated from the end of header
	uint16 u16MetricsSize;   // Metrics Data size
} __MTLK_PACKED MTLK_VSAF_SPR_ITEM_HDR;



#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* !__MHI_FRAME_INCLUDED */
