/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/***************************************************************************
****************************************************************************
**
** COMPONENT:        Shared RAM Allocation
**
** MODULE:           $File: //bwp/enet/demo153_sw/develop/src/common/shram.h $
**
** VERSION:          $Revision: #7 $
**
** DATED:            $Date: 2004/03/10 $
**
** AUTHOR:           LDE
**
** DESCRIPTION:      Shared RAM Allocation Header
**
** CHANGE HISTORY:
**
**   $Log: shram.h $
**   Revision 1.3  2003/02/21 15:50:30Z  prh
**   Combine host and test_mc code to avoid duplication.
**   Revision 1.2  2003/02/18 15:01:23Z  wrh
**   Removed unused definition
**   Revision 1.1  2003/02/11 16:29:12Z  wrh
**   Initial revision
**   Revision 1.34  2002/07/31 11:41:14Z  jcm
**   CF poll list
**   Revision 1.33.1.1  2002/08/16 10:46:58Z  ifo
**   Reduced FSDU usage in FPGA to fit in new FPGA build
**   Revision 1.33  2002/04/29 13:54:22Z  ifo
**   Moved typedefs to umi.h to allow usage of this file in lower MAC
**   builds without requiring UMAC files.
**   Revision 1.32  2002/04/04 12:16:34Z  prh
**   Increase RX buffers size (except for COSIM).
**   Revision 1.31  2002/04/04 11:09:21Z  mrg
**   Modified number of TX FSDUs for cosim.
**   Revision 1.30  2002/03/08 16:52:28Z  lde
**   Suprising that HIM/Cardbus tests worked.
**   Revision 1.29  2002/03/04 15:07:58Z  ifo
**   Renames test loopback mac to use different macro.
**   Revision 1.28  2002/02/05 08:44:45Z  ifo
**   Made Rx buffer size same as LM (LM needs now to use this value).
**   Revision 1.27.1.1  2002/02/01 14:29:40Z  jag1
**   Revision 1.27  2002/01/08 08:07:43Z  ifo
**   Defined RX circular buffer size in powers of two so
**   the macro may be used by the Lower MAC.
**   Revision 1.26  2001/12/04 15:53:24Z  prh
**   Change for C100.
**   Revision 1.25  2001/12/04 15:18:31Z  prh
**   Compile errors.
**   Revision 1.24  2001/12/04 14:16:46Z  prh
**   Optimize memory usage.
**   Revision 1.23  2001/10/24 14:12:42Z  ifo
**   Chnaged macro to include defines for test umac.
**   Revision 1.22  2001/10/05 10:13:13Z  lde
**   Restored HIM message buffer allocation.
**   Revision 1.21  2001/09/04 07:31:23Z  ifo
**   Removed unused or revised parameters.
**   Revision 1.20  2001/08/24 17:22:10Z  ifo
**   Removed obsolete SRAM struct.
**   Revision 1.19  2001/08/24 16:19:16Z  ifo
**   Added some HIM definitions.
**   Removed SRAM defn as obsolete.
**   Revision 1.18  2001/08/10 11:33:10Z  prh
**   Allow module test UMI buffer allocation to be smaller.
**   Revision 1.17  2001/07/25 07:27:09Z  ifo
**   Added comment.
**   Revision 1.16  2001/07/18 07:44:24Z  ifo
**   Removed unused macros and eleements.
**   Still needs more tidy.
**   Revision 1.15  2001/07/09 12:58:02Z  prh
**   Changes for BSS module testing.
**   Revision 1.14  2001/07/06 14:12:30Z  prh
**   Sort out FSDU/SHRAM allocation.
**   Revision 1.13  2001/07/06 08:49:10Z  prh
**   Temp change for PROBE_FSDU.
**   Revision 1.12  2001/06/27 14:21:10Z  prh
**   Add variable payload refernce to some management packet types.
**   Revision 1.11  2001/06/22 14:30:03Z  ifo
**   bug fix
**   Revision 1.10  2001/06/22 10:07:46Z  prh
**   Add resource for Auth TX packet.
**   Revision 1.9  2001/06/18 07:21:21Z  ifo
**   Added definition of Tx FSDU/MSDU storage.
**   Revision 1.8  2001/06/14 16:51:12Z  ses
**   Add FSDUbeacon.
**   Revision 1.7  2001/06/12 13:26:26Z  ifo
**   Chnaged macro name.
**   Revision 1.6  2001/06/07 09:22:50Z  ses
**   Add probe allocation.
**   Revision 1.5  2001/06/04 15:44:12Z  ses
**   Move shared RAM structures.
**   Revision 1.4  2001/04/30 08:55:08Z  lde
**   In  PC land, the shared RAM is now just a block of memory.
**   Revision 1.3  2001/04/20 16:19:08Z  lde
**   Now includes um_sdu.h and database.h for constants.
**   Revision 1.2  2001/04/04 15:22:02Z  lde
**   Now compiles
**   Revision 1.1  2001/04/04 14:02:25Z  lde
**   Initial revision
**
** LAST MODIFIED BY:   $Author: prh $
**                     $Modtime:$
**
****************************************************************************
*
* 
*
* 
*
****************************************************************************/

#ifndef SHRAM_INCLUDED_H
#define SHRAM_INCLUDED_H

#define  MTLK_PACK_ON
#include "mtlkpack.h"

/***************************************************************************/
/***        Build Specific Defines that override the default             ***/
/***************************************************************************/

/***************************************************************************/
/***       Default memory sizes (overridable on a build by build basis   ***/
/***************************************************************************/

#define FSDU_SECTION_SIZE      (2304*7*4)
#define MHI_MAN_STAT_SIZE_MAX    (20 * 1024)


#if !defined(NO_TX_FSDUS)       // there are 7 if MSDU_MAX_LENGTH=2304 and 10 if MSDU_MAX_LENGTH=1600
    #define NO_TX_FSDUS         2//9    /* Used for allocating FSDU memory pool in shram_fsdu.c */
	#define NO_TX_FSDUS_AC_BK   1
	
	#define NO_TX_FSDUS_AC_BE   4//20//<y.s> 10
	#define NO_TX_FSDUS_AC_VI   4
	#define NO_TX_FSDUS_AC_VO   2// <y.s> 10
    #define TOTAL_TX_FSDUS      (NO_TX_FSDUS_AC_BK + NO_TX_FSDUS_AC_BE +NO_TX_FSDUS_AC_VI +NO_TX_FSDUS_AC_VO)

	#define NO_MAX_PACKETS_AGG_SUPPORTED_BK   1
	#define NO_MAX_PACKETS_AGG_SUPPORTED_BE	  2
	#define NO_MAX_PACKETS_AGG_SUPPORTED_VI	  2
	#define NO_MAX_PACKETS_AGG_SUPPORTED_VO	  2
#endif


#if !defined(NO_CF_POLLABLE) && defined (ENET_INC_PCF)
    #define NO_CF_POLLABLE      4
#endif

#if !defined(BSS_IND_COUNT)
    #define BSS_IND_COUNT       3
#endif

#if !defined(STA_IND_COUNT)
    #define STA_IND_COUNT       3
#endif

#if !defined(RSN_IND_COUNT)
    #define RSN_IND_COUNT       1
#endif

#if !defined(UMI_IND_COUNT)
    #define UMI_IND_COUNT       1
#endif

#if !defined (DEBUG_BUFFER_SIZE)
#if defined(ENET_CFG_RSN_MODULE_TEST)
    #define DEBUG_BUFFER_SIZE   (MAX_DEBUG_OUTPUT*80)     /* Stop build overflowing  */
#else
    #define DEBUG_BUFFER_SIZE   (MAX_DEBUG_OUTPUT*100)    /* Large buffers prevent debug overflow */
#endif
#endif


/***************************************************************************/
/***       HIM buffers                                                   ***/
/***************************************************************************/
/*
 * These define the number of Management, Data and Debug message buffers
 * allocated by the HIM. Note that there are no Memory message buffers used
 * between the Host and the MAC.
 */

#if !defined(NUM_MAN_REQS)
#define NUM_MAN_REQS            2
#endif

#if !defined(NUM_DAT_REQS)
#if defined( ENET_CFG_TLM_SUBSYS )
#define NUM_DAT_REQS            16  /* formerly TX_MSDU_COUNT*/
#else
#define NUM_DAT_REQS            200 /* formerly TX_MSDU_COUNT*/
#endif
#endif

// Due to a bug in the driver - MAC created a workaround and reduced dbg req & ind to 1
#define NUM_DBG_REQ_OR_IND_WORKAROUND 1


#if !defined(NUM_DBG_REQS)
#define NUM_DBG_REQS            NUM_DBG_REQ_OR_IND_WORKAROUND   /* Buffers for HOST to send DBG messages to MAC with.  */
#endif

#if !defined(NUM_BSS_REQS)
#define NUM_BSS_REQS            (1)   /* Buffers for HOST to send BSS manager messages to MAC  */
#endif


#if !defined(NUM_DBG_INDS)
#define NUM_DBG_INDS            NUM_DBG_REQ_OR_IND_WORKAROUND   /* 1 for each HIM 'serial' port */
#endif

#if !defined(NUM_MAN_INDS)
#define NUM_MAN_INDS            (BSS_IND_COUNT + STA_IND_COUNT + RSN_IND_COUNT + UMI_IND_COUNT)
#endif

#if !defined(NUM_BSS_INDS)
#define NUM_BSS_INDS            (1)   /* Buffers for HOST to send BSS manager messages to MAC  */
#endif



/* If we are using the forwarding pool it must be have at lease one buffer */
#if !defined (ENET_AP_FORWARD_POOL_SIZE) && defined (ENET_CFG_ALLOC_SEPARATE_FORWARD_POOL)
    #define ENET_AP_FORWARD_POOL_SIZE 3
#endif

 /*
 * The size of the Circular Buffer in the Lower MAC used for received MPDUs.
 * It must be a power of two bytes.
 * Note that the receive buffer size is in powers of 2 above 4K.
 * Thus a value of 12 => 4K, 13 => 8K, 14 => 16K, etc.
 */


#if !defined(ENET_CFG_SIZE_RX_CIRC_BUF_LOG_2)

#define ENET_CFG_SIZE_RX_CIRC_BUF_LOG_2     15

// #define ENET_CFG_SIZE_RX_CIRC_BUF_LOG_2     12
#endif

#define ENET_CFG_SIZE_RX_CIRC_BUF  (1 << ENET_CFG_SIZE_RX_CIRC_BUF_LOG_2)
#define ENET_RX_CIRC_BUF_SHADOW    ENET_CFG_SIZE_RX_CIRC_BUF*2


#define ENET_CFG_SIZE_RX_BUF_WAVE500 (1 << 14)   //A patch needed for wave500 till this buffer is removed
#define ENET_CFG_SIZE_RX_BUF_WAVE500_IN_WORDS (ENET_CFG_SIZE_RX_BUF_WAVE500 >> 2)

/* Number of message that can be placed on message queues between host and HIM */
#define HIM_CHI_N               (NUM_MAN_REQS + NUM_DBG_REQS + NUM_BSS_REQS +\
                                 NUM_MAN_INDS + NUM_DBG_INDS + NUM_BSS_INDS)

typedef struct
{
    uint32 u32IndStartOffset;
    uint32 u32IndNumOfElements;
    uint32 u32ReqStartOffset;
    uint32 u32ReqNumOfElements;
} __MTLK_PACKED CHI_MEM_AREA_DESC;


typedef struct
{
   uint32 u32InCounterAddress;
   uint32 u32InStatusAddress; // used only for Gen5
   uint32 u32OutCounterAddress;
   uint32 u32OutStatusAddress; // used only for Gen5
   uint32 u32InCounterAddressBigEnd;		// PUMA DIRECT CONNECT uses Wave's BIG Endianness address
   uint32 u32OutFreedCounterAddressBigEnd;	// PUMA DIRECT CONNECT XX OUT FREED counter address (XX = TX/RX). Counter is in BIG Endianness.
} __MTLK_PACKED CHI_MEM_RING;

typedef struct
{
   uint32 u32RdPoolBaseAddress;
   uint32 u32RdSizeInBytes; 
   uint32 u32PayloadAddressOffsetInRd;
   uint32 u32BdIndexOffsetInRd; 
   uint32 u32NumRdsInPool;
   uint32 u32NumMgmtRdsInPool;
   uint32 u32FirstMgmtRdIndex;
} __MTLK_PACKED CHI_MEM_RD_POOL;

typedef struct
{
   uint32 u32PdPoolBaseAddress;
   uint32 u32DataPdPoolOffsetFromBaseAddress;
   uint32 u32PdSizeInBytes; 
   uint32 u32PayloadAddressOffsetInPd;
   uint32 u32NextPdPointerOffsetInPd;
   uint32 u32NextPdPointerFieldMask;
   uint32 u32PdSourceWordOffsetInPd; 	// Offset in words of "pdSource" field in TxPd_t
   uint32 u32PdSourceFieldMask;	 		// "pdSource" field mask within the given word in TxPd_t 
   uint32 u32PdSourceFieldShift;		// "pdSource" field bit location within the given word in TxPd_t. This should be used for "shift right" before applying the mask.
   uint32 u32PdSourceValueToUse;		// The pdSource value of an original (parent) PD. This PD should be used by driver to return the payload address. Other values shouldn't.
   uint32 u32MaxNumOfAvailablePds;	
} __MTLK_PACKED CHI_MEM_PD_POOL;


typedef struct
{
    CHI_MEM_AREA_DESC sFifoQ;
    CHI_MEM_AREA_DESC sMAN;
    CHI_MEM_AREA_DESC sDBG;
    CHI_MEM_AREA_DESC sBSS;		
	CHI_MEM_RING sTxRING;
	CHI_MEM_RING sRxRING;
	CHI_MEM_RING sMangTxRING;
	CHI_MEM_RING sMangRxRING;
    CHI_MEM_RD_POOL	sRdPOOL;	
    CHI_MEM_PD_POOL	sPdPOOL;
    uint32 u32Magic; /* must be written last!!! */
} __MTLK_PACKED VECTOR_AREA_BASIC;


#define HOST_EXTENSION_MAGIC         		        0xBEADFEED

#define VECTOR_AREA_CALIBR_EXTENSION_ID								(1)
#define VECTOR_AREA_MIPS_CONTROL_EXTENSION_ID						(2)
#define VECTOR_AREA_LOGGER_EXTENSION_ID								(3)
#define VECTOR_AREA_FW_CAPABILITIES_NUM_OF_STATIONS_EXTENSION_ID	(4)
#define VECTOR_AREA_FW_CAPABILITIES_NUM_OF_VAPS_EXTENSION_ID		(5)
#define VECTOR_AREA_LOGGER_BUF_INFO_EXTENSION_ID					(6)
#define VECTOR_AREA_FRAGMENTATION_RING_EXTENSION_ID 				(7)
#define VECTOR_AREA_VECTOR_AREA_DEBUG_EXTENSION_ID					(8)  
#define VECTOR_AREA_CARD_CONFIGURATION_EXTENSION_ID					(9)
#define VECTOR_AREA_VECTOR_AREA_WHM_BUFFERS_INFO_EXTENSION_ID		(10)

typedef struct _VECTOR_AREA_EXTENSION_HEADER
{ 
    uint32 u32ExtensionMagic;
    uint32 u32ExtensionID;
    uint32 u32ExtensionDataSize;
} __MTLK_PACKED VECTOR_AREA_EXTENSION_HEADER;


typedef struct _VECTOR_AREA_CALIBR_EXTENSION_DATA
{ 
    uint32 u32DescriptorLocation;  /* PAS offset to set Calibration Cache Descriptor */
    uint32 u32BufferRequestedSize; /* Requested Calibration Cache Buffer Size        */
	uint32 pPsdProgmodelAddress;
}__MTLK_PACKED VECTOR_AREA_CALIBR_EXTENSION_DATA;


typedef struct
{ 
    VECTOR_AREA_EXTENSION_HEADER      sHeader;
    VECTOR_AREA_CALIBR_EXTENSION_DATA sData;
}__MTLK_PACKED VECTOR_AREA_CALIBR_EXTENSION;

typedef struct _VECTOR_AREA_MIPS_CONTROL_EXTENSION_DATA
{
    uint32 u32DescriptorLocation;  /* PAS offset to set mips control Descriptor (MIPS_CONTROL_DESCRIPTOR) */
}__MTLK_PACKED VECTOR_AREA_MIPS_CONTROL_EXTENSION_DATA;


typedef struct
{ 
    VECTOR_AREA_EXTENSION_HEADER            sHeader;
    VECTOR_AREA_MIPS_CONTROL_EXTENSION_DATA sData;
}__MTLK_PACKED VECTOR_AREA_MIPS_CONTROL;

#define FW_CORE0			(0) // wave600 and wave500: Lower mac 0
#define FW_CORE1			(1) // wave600: Lower mac 1; wave500: Upper mac
#define FW_CORE2			(2) // wave600: Upper mac; wave500: N/A
#define MAX_NUM_OF_FW_CORES	(3)

typedef struct
{ 
    uint32 u32FwCoreCtrl[MAX_NUM_OF_FW_CORES];
} __MTLK_PACKED FW_CORE_CONTROL_DESCRIPTOR;

#define FW_CORE_CTRL_DO_ASSERT MTLK_BFIELD_INFO(0, 1) /* 1 bits starting bit 0 */

typedef struct 
{
    uint32 u32BufferDescriptorsLocation; /* PAS offset to set Logger buffers Descriptor */
    uint32 u32NumOfBufferDescriptors;    /* number of BD in Logger BD table */
} __MTLK_PACKED VECTOR_AREA_LOGGER_EXTENSION_DATA;

typedef struct
{
    VECTOR_AREA_EXTENSION_HEADER      sHeader;
    VECTOR_AREA_LOGGER_EXTENSION_DATA sData;
} __MTLK_PACKED VECTOR_AREA_LOGGER_EXTENSION;

typedef struct 
{
uint32 u32NumOfStations;	/* number of stas supported */
} __MTLK_PACKED VECTOR_AREA_FW_CAPABILITIES_NUM_OF_STATIONS_EXTENSION_DATA;

typedef struct
{
	VECTOR_AREA_EXTENSION_HEADER								sHeader;
	VECTOR_AREA_FW_CAPABILITIES_NUM_OF_STATIONS_EXTENSION_DATA	sData;
} __MTLK_PACKED VECTOR_AREA_FW_CAPABILITIES_NUM_OF_STATIONS_EXTENSION;

typedef struct 
{
	uint32 u32NumOfVaps;		/* number of vaps supported */
} __MTLK_PACKED VECTOR_AREA_FW_CAPABILITIES_NUM_OF_VAPS_EXTENSION_DATA;

typedef struct
{
	VECTOR_AREA_EXTENSION_HEADER							sHeader;
	VECTOR_AREA_FW_CAPABILITIES_NUM_OF_VAPS_EXTENSION_DATA	sData;
} __MTLK_PACKED VECTOR_AREA_FW_CAPABILITIES_NUM_OF_VAPS_EXTENSION;

typedef struct 
{
    uint32 u32Size;
    uint32 u32NumberOfBuffers;
    uint32 u32BufferPoolAddress;
} __MTLK_PACKED VECTOR_AREA_LOGGER_BUF_INFO;


typedef struct 
{
    VECTOR_AREA_LOGGER_BUF_INFO lmBufferInfo;
    VECTOR_AREA_LOGGER_BUF_INFO umBufferInfo;
} __MTLK_PACKED VECTOR_AREA_LOGGER_BUF_INFO_EXTENSION_DATA;

typedef struct
{
    VECTOR_AREA_EXTENSION_HEADER sHeader;
    VECTOR_AREA_LOGGER_BUF_INFO_EXTENSION_DATA sData;
} __MTLK_PACKED VECTOR_AREA_LOGGER_BUF_INFO_EXTENSION;

typedef struct 
{
    uint32 u32FragmentationRingAddress;			/* Address of Fragmentation Ring for CBM Fragmentation issue W/A, each entry is 4 bytes and used for storing HDs addresses*/
	uint32 u32FragmentationRingNumOfEntries;	/* Num Of entries in Ring*/
} __MTLK_PACKED VECTOR_AREA_FRAGMENTATION_RING_EXTENSION_DATA;

typedef struct
{
    VECTOR_AREA_EXTENSION_HEADER sHeader;
    VECTOR_AREA_FRAGMENTATION_RING_EXTENSION_DATA sData;
    
} __MTLK_PACKED VECTOR_AREA_FRAGMENTATION_HD_RING_EXTENSION;


typedef struct 
{
	uint32			u32Core0_info; /* lower mac 0 */
	uint32			u32Core1_info; /* lower mac 1 */
	uint32			u32Core2_info; /* upper mac */
} __MTLK_PACKED VECTOR_AREA_DEBUG_EXTENSION_DATA;


typedef struct
{
    VECTOR_AREA_EXTENSION_HEADER sHeader;
    VECTOR_AREA_DEBUG_EXTENSION_DATA sData;
} __MTLK_PACKED VECTOR_AREA_DEBUG_EXTENSION;


// Band configaration parameters

#define BAND_CONFIGURATION_MODE_SINGLE_BAND				(0)
#define BAND_CONFIGURATION_MODE_DUAL_BAND				(1)
#define BAND_CONFIGURATION_MODE_SINGLE_BAND_WITH_SCAN	(2)

#define CARD_CONFIGURATION_PCI_MODE_SINGLE_PCI 			(0)
#define CARD_CONFIGURATION_PCI_MODE_DUAL_PCI 			(1)

#define TEST_PLATFORM_TYPE_FPGA							(0)
#define TEST_PLATFORM_TYPE_PDXP							(1)

#define PCIE_NO_SNOOPING_MODE_DISABLE					(0)
#define PCIE_NO_SNOOPING_MODE_ENABLE					(1)


typedef struct 
{
	uint32			u32BandConfiguration; /* 0x0 - single band / 0x1 - dual band / 0x2 - single band + scan on other band */
	uint32			u32PCIeCardConfiguration; /* 0x0 - single PCIe / 0x1 - dual PCIe */
	uint32			u32TestPlatformType; /* 0x0 - FPGA, 0x1 - PDXP (Palladium). this field is ignored when running on real chip */
	uint32			masterVapIDBand0; /* this will be used by the FW for dual band and for single band */
	uint32			masterVapIDBand1; /* this will be used by the FW for dual band only */
	uint32			u32PcieNoSnooping; /* 0x0 (FALSE) - No snooping feature is not needed. 0x1 (TRUE) - No snooping feature should be configured. (D2 only) */
} __MTLK_PACKED VECTOR_AREA_CARD_CONFIGURATION_EXTENSION_DATA;

typedef struct
{
    VECTOR_AREA_EXTENSION_HEADER sHeader;
    VECTOR_AREA_CARD_CONFIGURATION_EXTENSION_DATA sData;
} __MTLK_PACKED VECTOR_AREA_CARD_CONFIGURATION_EXTENSION;

#define WHM_ADDRESSES_AND_SIZES (8)
#define WHM_HW_LOGGER_BUFFERS (0)
typedef struct
{
	int32 u32WhmBuffersAddress[WHM_ADDRESSES_AND_SIZES];
	uint32 u32WhmBuffersSize[WHM_ADDRESSES_AND_SIZES];
} __MTLK_PACKED VECTOR_AREA_WHM_BUFFERS_INFO_EXTENSIONS_DATA;

typedef struct
{	
    VECTOR_AREA_EXTENSION_HEADER sHeader;
	VECTOR_AREA_WHM_BUFFERS_INFO_EXTENSIONS_DATA sData;
} __MTLK_PACKED VECTOR_AREA_WHM_INFO_EXTENSION;

typedef struct 
{
	// Imprtant note !!!!!!!!!!!!!!!!!!
	// There should be no change in the order / size / structure of all fields starting from sBasic and until sDebugExt.
	// The reason for it is that ROM in D2 is also using this structure and it can't be changed.

	// Dont change - START
    VECTOR_AREA_BASIC   									sBasic;
    VECTOR_AREA_CALIBR_EXTENSION							sCalibr;
    VECTOR_AREA_MIPS_CONTROL								sMipsControl;
    VECTOR_AREA_LOGGER_EXTENSION							sLoggerExt; // Not used anymore
	VECTOR_AREA_FW_CAPABILITIES_NUM_OF_STATIONS_EXTENSION	sFwCapabilitiesNumOfStationsExt;
	VECTOR_AREA_FW_CAPABILITIES_NUM_OF_VAPS_EXTENSION		sFwCapabilitiesNumOfVapsExt;
    VECTOR_AREA_LOGGER_BUF_INFO_EXTENSION                   sLoggerBufInfoExt; //  Not used anymore
	VECTOR_AREA_FRAGMENTATION_HD_RING_EXTENSION		        sHdFragmentationRingExt;  
	VECTOR_AREA_DEBUG_EXTENSION		                        sDebugExt;
	// Dont change - END
    VECTOR_AREA_CARD_CONFIGURATION_EXTENSION                sCardConfigExt;
	VECTOR_AREA_WHM_INFO_EXTENSION							sWhmInfoExt;
}__MTLK_PACKED VECTOR_AREA;


/***************************************************************************
**
** NAME         RXMETRICS_HEADER
**
** PARAMETERS       
**              
**              
**
** DESCRIPTION  Sounding packet metric header
**
****************************************************************************/
typedef struct _MTLK_RXMETRICS_HEADER
{
	uint32 rank; /* The number of spatial streams of received sounding packet */
	uint32 isCB;        
	/* put here all the metrics-related stuff you need - padded to 32bit */
} __MTLK_PACKED MTLK_RXMETRICS_HEADER;

/*********************************************************************************************
* G2 Metrics
*********************************************************************************************/

/***************************************************************************
**
** NAME         ASL_SHRAM_METRIC_T

** DESCRIPTION  Sounding packet metric struct
**
****************************************************************************/
#define MAX_SCALE_FACTOR_LENGTH			27
#define MAX_MEASURED_SPATIAL_STREAMS	2
#define MAX_NUM_OF_MEASURED_BINS		108


typedef struct ASL_SHRAM_METRIC_T 
{

	MTLK_RXMETRICS_HEADER     sMtlkHeader;
	
	uint32			 au32metric[MAX_MEASURED_SPATIAL_STREAMS][MAX_NUM_OF_MEASURED_BINS];
	uint32			 au32scaleFactor[MAX_SCALE_FACTOR_LENGTH];
	uint32			 u32NoiseFunction; //represents different values for different Antenna Selection methods

}__MTLK_PACKED ASL_SHRAM_METRIC_T;
/*********************************************************************************************/

/*********************************************************************************************
* G3 Metrics
*********************************************************************************************/
#define PHY_ESNR_VAL MTLK_BFIELD_INFO(0, 13) /* 13 bits starting BIT0  in effectiveSNR field */
#define PHY_MCSF_VAL MTLK_BFIELD_INFO(13, 4) /*  4 bits starting BIT13 in effectiveSNR field */

typedef struct RFM_SHRAM_METRIC_T
{
	uint32			 effectiveSNR;
}__MTLK_PACKED RFM_SHRAM_METRIC_T;
/*********************************************************************************************/

#define DRIVER_FRAME_PREFIX_RESERVED_LENGTH_IN_WORDS 0xd

typedef struct _RX_PHY_METRICS {
  uint32   tsf;
  uint32   reservedArea[DRIVER_FRAME_PREFIX_RESERVED_LENGTH_IN_WORDS];
  uint64   genriscSpare;
  uint64   phy_status_word0;
  /* Arrays of size 4 (NUMBER_OF_RX_ANTENNAS) actually must be 32bits total */
  union {
    uint32  word_noise_estimation;
    uint8   noise_estimation_arr[4];
  };
  union {
    uint32  word_rf_gain;
    uint8   rf_gain_arr[4];
  };
  union {
    uint32  word_rssi;
    int8    rssi_arr[4];
  };
  uint32   phy_status_word2_2;
} __MTLK_PACKED RX_PHY_METRICS;


#define PHY_STAT0_RATE_INFO MTLK_BFIELD_INFO(46, 11) /* 11 bits starting bit46: PHYMODE + MCS + CBW + SCP */

#define PHY_STAT2_2_RATE    MTLK_BFIELD_INFO(16, 15) /* 15 bits starting bit16 [10*MBPS]*/

/* FIXME: moved from mhi_umi.h */
#define PHY_STAT_RATE_PHYMODE   MTLK_BFIELD_INFO(0, 2)  /*  2 bits starting bit0  [ 00 - 11a/g     01 - 11b    10 - 11n (HT mode)   11 - 11ac (VHT mode) ]*/
#define PHY_STAT_RATE_MCS       MTLK_BFIELD_INFO(2, 6)  /*  6 bits starting bit2  [MCS index ]*/
#define PHY_STAT_RATE_CBW       MTLK_BFIELD_INFO(8, 2)  /*  2 bits starting bit8  [ 00 - 20MHz     01 - 40MHz    10 - 80MHz    11 - 160MHz ]*/
#define PHY_STAT_RATE_SCP       MTLK_BFIELD_INFO(10, 1) /*  1 bits starting bit10 [Short CP]*/

#define PHY_RATE_INVALID    (0x7FFF)
#define PHY_NOISE_INVALID   (0x80)
#define PHY_RFGAIN_INVALID  (0x80)

/*********************************************************************************************/

/*********************************************************************************************
* G6 Metrics
*********************************************************************************************/

#define G6_MAX_ANTENNAS (8)
#define G6_MAX_NSS      (8)

typedef struct _G6_RXMETRICS_COMMON_PRE_PPDU
{
	uint32  Status0Word0;
	uint32  Status0Word1;
	union { /* Status0Word2 and Status0Word3 */
		struct { uint32  TdRssiWord0, TdRssiWord1; };
		struct { int8    TdRssi[G6_MAX_ANTENNAS]; };
	};
	union { /* Status1Word0 and Status0Word1 */
		struct { uint32  NoiseWord0, NoiseWord1; };
		struct { uint8   Noise[G6_MAX_ANTENNAS]; };
	};
	union { /* Status1Word2 and Status0Word3 */
		struct { uint32  RfGainWord0, RfGainWord1; };
		struct { uint8   RfGain[G6_MAX_ANTENNAS]; };
	};
	uint32  Status2Word0;
	uint32  Status2Word1;
	uint32  Status2Word2;
	uint32  Status2Word3;
} __MTLK_PACKED G6_RXMETRICS_COMMON_PRE_PPDU;

typedef struct _G6_RXMETRICS_COMMON_POST_PPDU
{
	uint32  Status0Word0;
	uint32  Status0Word1;
	uint32  Status0Word2;
	uint32  Status0Word3;
} __MTLK_PACKED G6_RXMETRICS_COMMON_POST_PPDU;

typedef struct _G6_RXMETRICS_USER_SPEC_PRE_PPDU
{
	uint32  Status2Word0;
	uint32  Status2Word1;
	union { /* Status2Word2 and Status2Word3 */
		struct { uint32  FdRssiWord0, FdRssiWord1; };
		struct { int8    FdRssi[G6_MAX_ANTENNAS]; };
	};
} __MTLK_PACKED G6_RXMETRICS_USER_SPEC_PRE_PPDU;

typedef struct _G6_RXMETRICS_USER_SPEC_POST_PPDU
{
	union { /* Status0Word0 and Status0Word1 */
		struct { uint32  EvmWord0, EvmWord1; };
		struct { uint8   Evm[G6_MAX_NSS]; };
	};
	union {
		uint32  Status2Word2;
		uint8   EffectiveSnr;
	};
	uint32  Status2Word3;
} __MTLK_PACKED G6_RXMETRICS_USER_SPEC_POST_PPDU;

typedef struct _G6_RX_PHY_METRICS {
	G6_RXMETRICS_COMMON_PRE_PPDU       commonPrePpduMetrics;
	G6_RXMETRICS_COMMON_POST_PPDU      commonPostPpduMetrics;
	G6_RXMETRICS_USER_SPEC_PRE_PPDU    userSpecPrePpduMetrics;
	G6_RXMETRICS_USER_SPEC_POST_PPDU   userSpecPostPpduMetrics;
} __MTLK_PACKED G6_RX_PHY_METRICS;


#define G6_COMMON_POST_PPDU_PHY_STAT0_0_POST_METRICS_VALID MTLK_BFIELD_INFO(0, 1)  /* 1 bit  starting bit0 */
#define G6_COMMON_POST_PPDU_PHY_STAT0_0_PRE_METRICS_VALID  MTLK_BFIELD_INFO(1, 1)  /* 1 bit  starting bit1 */

#define G6_COMMON_PRE_PPDU_PHY_STAT0_0_PHY_MODE       MTLK_BFIELD_INFO(17, 3) /*  3 bits starting bit17 */

#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_PSDU_LENGTH MTLK_BFIELD_INFO(0, 22) /* 22 bits starting bit0  */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_PSDU_RATE   MTLK_BFIELD_INFO(24, 8) /*  8 bits starting bit24 */
/* PSDU Rate subfields */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_11B_MCS     MTLK_BFIELD_INFO(24, 2) /* 11b MCS */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_11B_LP      MTLK_BFIELD_INFO(26, 1) /* Long Preamble */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_LEG_MCS     MTLK_BFIELD_INFO(24, 3) /* Legacy MCS */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_HT_MCS      MTLK_BFIELD_INFO(24, 6) /* HT MCS */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_VHT_MCS     MTLK_BFIELD_INFO(24, 4) /* VHT MCS */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_VHT_NSS     MTLK_BFIELD_INFO(28, 3) /* VHT NSS */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_HE_MCS      MTLK_BFIELD_INFO(24, 4) /* HE MCS */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_HE_NSS      MTLK_BFIELD_INFO(28, 3) /* HE NSS */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_0_HE_DCM      MTLK_BFIELD_INFO(31, 1) /* HE DCM */

#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_1_LDPC        MTLK_BFIELD_INFO(0, 1)  /*  1 bit  starting bit0 */
#define G6_USER_SPEC_PRE_PPDU_PHY_STAT2_1_PHY_RATE    MTLK_BFIELD_INFO(4, 17) /* 17 bits starting bit4 */

/*********************************************************************************************
 *  PSDU RATE INFO : 16 bits
 *    ------------------------------------------------------------
 *    |  PSDU RATE                 | PHY_MODE |  CBW  | RESERVED |
 *    ------------------------------------------------------------
 *    0                            8          11     13          16
 *
 * PSDU RATE : 8 bits
 * Description:
 *    --------------------------------- 
 *    | Legacy  | MCS - [2:0] 3 bits  |
 *    ---------------------------------
 *    | 11b     | MCS - 0             |
 *    ---------------------------------
 *    | HT      | MCS - [5:0] 6 bits  |
 *    ---------------------------------
 *    | VHT     | MCS - [3:0] 4 bits  |
 *    |         | NSS - [6:4] 3 bits  |
 *    ---------------------------------
 *    | HE      | MCS - [3:0] 4 bits  |
 *    |         | NSS - [6:4] 3 bits  |
 *    ---------------------------------
 *
 * PHYMODE - 3 bits [ 0 - 11a/g, 1 -11b, 2 -11n (HT mode), 3 -11ac (VHT mode), 4 - HE (SU),
 * 5 - HE ( EX SU) , 6 - HE (MU- trigger based) (not supported by the Mac), 7- HE (MU- DL)]
 *
 * BW - 2 bits [0-20MHz, 1-40MHz, 2-80Mhz, 3-160Mhz ]
 ********************************************************************************************/
#define MTLK_BITRATE_INFO_PSDU_RATE MTLK_BFIELD_INFO(0, 16)
#define MTLK_BITRATE_INFO_BY_PSDU_RATE(psdu_rate) \
        MTLK_BFIELD_VALUE(MTLK_BITRATE_INFO_PSDU_RATE, (psdu_rate), mtlk_bitrate_info16_t)
#define MTLK_BITRATE_PSDU_LEGACY_MCS          MTLK_BFIELD_INFO(0, 3)
#define MTLK_BITRATE_PSDU_HT_MCS              MTLK_BFIELD_INFO(0, 6)
#define MTLK_BITRATE_PSDU_VHT_MCS             MTLK_BFIELD_INFO(0, 4)
#define MTLK_BITRATE_PSDU_VHT_NSS             MTLK_BFIELD_INFO(4, 3)
#define MTLK_BITRATE_PSDU_HE_MCS              MTLK_BFIELD_INFO(0, 4)
#define MTLK_BITRATE_PSDU_HE_NSS              MTLK_BFIELD_INFO(4, 3)
#define MTLK_BITRATE_STAT_PSDU_PHYMODE        MTLK_BFIELD_INFO(8, 3)
#define MTLK_BITRATE_PSDU_BW                  MTLK_BFIELD_INFO(11, 2)
#define MTLK_BITRATE_PSDU_SCP                 MTLK_BFIELD_INFO(13, 1)
#define MTLK_BITRATE_PSDU_SCP_HE              MTLK_BFIELD_INFO(14, 2)

/*********************************************************************************************
* PMCU
*********************************************************************************************/
typedef struct _PMCU_DDR_ACCESS {
  uint32   value;
} __MTLK_PACKED PMCU_DDR_ACCESS ;

#define PMCU_DDR_ACCESS_FW_LOCK         MTLK_BFIELD_INFO(0, 1)  /*  1 bits starting bit0 [1 - Disable FW access to DDR, 0 - Enable FW access to DDR]*/
#define PMCU_DDR_ACCESS_SIGNALING       MTLK_BFIELD_INFO(1, 1)  /*  1 bits starting bit1 [1 - Signaled, 0 - Reseted]*/

/*********************************************************************************************/


#define  MTLK_PACK_OFF
#include "mtlkpack.h"
#endif /* !SHRAM_INCLUDED_H */

