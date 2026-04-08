/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/****************************************************************************
 ****************************************************************************
 **
 ** COMPONENT:      Managed Object Definitions
 **
 ** MODULE:         $File: //bwp/enet/demo153_sw/develop/src/shared/mhi_mib_id.h $
 **
 ** VERSION:        $Revision: #8 $
 **
 ** DATED:          $Date: 2004/02/20 $
 **
 ** AUTHOR:         S Sondergaard
 **
 ** DESCRIPTION:    External interface for managed objects
 **
 ****************************************************************************
 *
 *  
 *
 *  
 *
 ****************************************************************************/

#ifndef  __MHI_MIB_ID_INCLUDED
#define  __MHI_MIB_ID_INCLUDED

#include "mhi_global_definitions.h"
#include "mhi_ieee_address.h"
#define   MTLK_PACK_ON
#include "mtlkpack.h"

//#include "System_GlobalDefinitions.h"
/***************************************************************************/
/***                   Include File Dependencies                         ***/
/***************************************************************************/

/***************************************************************************/
/***                  Global data                                        ***/
/***************************************************************************/

/*----Macro Definitions-----------------------------------------------------*/
/* Keep up to date with hi_motab.h */


/* MIB key length validations */
#define MIB_WEP_KEY_INVALID_LENGTH          0       /* WEP key is not set */
#define MIB_WEP_KEY_WEP1_LENGTH             5       /* WEP key type 1 is 40 bits (5 bytes) */
#define MIB_WEP_KEY_WEP2_LENGTH             13      /* WEP key type 2 is 104 bits (13 bytes) */
                                                    /* define other values for other keys here */

#define NO_RATE                             0xFFFF

#define FORCED_RATE_LEGACY_MASK             0x0001
#define FORCED_RATE_HT_MASK                 0x0002

/***************************************************************************/
/***                       RSN MIB Objects                               ***/
/***************************************************************************/

/* RSN Control */
#define RSNMIB_DEFAULT_REPLAY_WINDOW        (16)


/* Pairwise alert thresholds */
typedef struct _RSNMIB_PAIRWISE_ALERT_THRESHOLDS
{
    uint16  u16TkipIcvErrorThreshold;
    uint16  u16TkipMicFailureThreshold;
    uint16  u16TkipReplayThreshold;
    uint16  u16CcmpMicFailureThreshold;
    uint16  u16CcmpReplayThreshold;
    uint16  u16NumTxThreshold;
} __MTLK_PACKED RSNMIB_PAIRWISE_ALERT_THRESHOLDS;

/* Group alert thresholds */
typedef struct _RSNMIB_GROUP_ALERT_THRESHOLDS
{
    uint16  u16TkipIcvErrorThreshold;
    uint16  u16TkipMicFailureThreshold;
    uint16  u16CcmpMicFailureThreshold;
    uint16  u16NumTxThreshold;
} __MTLK_PACKED RSNMIB_GROUP_ALERT_THRESHOLDS;

/* RSN security parameters in RSN IE format */
#define MIB_RSN_PARAMETERS_LENGTH           (40)
/*--------------------------------------------------------------------------
 * Managed Object Type Definitions
 *
 * Description:     WME Parameters
 *--------------------------------------------------------------------------*/
/* Constant definitions for WME */

#define MAX_USER_PRIORITIES                 4


typedef struct _MIB_WME_TABLE
{
    uint8   au8CWmin[MTLK_PAD4(MAX_USER_PRIORITIES)];  /* Log CW min */
    uint16  au16Cwmax[MTLK_PAD4(MAX_USER_PRIORITIES)]; /* Log CW max */
    uint8   au8AIFS[MTLK_PAD4(MAX_USER_PRIORITIES)];
    uint16  au16TXOPlimit[MTLK_PAD4(MAX_USER_PRIORITIES)];
    uint16  auMSDULifeTime[MTLK_PAD4(MAX_USER_PRIORITIES)];
    uint8   auAdmissionControl[MTLK_PAD4(MAX_USER_PRIORITIES)];
} __MTLK_PACKED MIB_WME_TABLE;



typedef struct _FM_WME_OUI_AND_VER
{
    uint8  au8OUI[3];
    uint8  u8OUItype;
    uint8  u8OUIsubType;
    uint8  u8Version;
    uint8  u8QosInfoField;
    uint8  u8Reserved;
} __MTLK_PACKED FM_WME_OUI_AND_VER;

typedef struct _FM_WME_QOS_INFO_FIELD
{
	uint8  parameterSetCount	:4;
	uint8  reserved				:3;
    uint8  au8OUI				:1; 
} __MTLK_PACKED FM_WME_QOS_INFO_FIELD;

typedef struct _PRE_ACTIVATE_MIB_TYPE
{
    uint8  u8Reserved[3]                   ; // for 32 bit alignment
	uint8  u8NetworkMode                   ; // MIB_NETWORK_MODE uses NETWORK_MODES_E values
    uint8  u8UpperLowerChannel             ; // MIB_UPPER_LOWER_CHANNEL_BONDING
    uint8  u8SpectrumMode                  ; // MIB_SPECTRUM_MODE
    uint8  u8ShortSlotTimeOptionEnabled11g ; // MIB_SHORT_SLOT_TIME_OPTION_ENABLED_11G
    uint8  u8ShortPreambleOptionImplemented; // MIB_SHORT_PREAMBLE_OPTION_IMPLEMENTED
    uint32 u32OperationalRateSet           ; // MIB_OPERATIONAL_RATE_SET
    uint32 u32BSSbasicRateSet              ; // MIB_BSS_BASIC_RATE_SET
} __MTLK_PACKED PRE_ACTIVATE_MIB_TYPE;

typedef enum _NETWORK_MODES 
{
	NETWORK_11B_ONLY,
	NETWORK_11G_ONLY,
	NETWORK_11N_2_4_ONLY,
	NETWORK_11BG_MIXED,
	NETWORK_11GN_MIXED,
	NETWORK_11BGN_MIXED,
	NETWORK_11A_ONLY,
	NETWORK_11N_5_ONLY,
	NETWORK_11AN_MIXED,
	NUM_OF_NETWORK_MODES
} NETWORK_MODES_E;

/*--------------------------------------------------------------------------
 * Managed Object definitions
*
* Description:     The representation of unsigned 8-bits value
*
*--------------------------------------------------------------------------*/
typedef uint8  MIB_UINT8;

/*--------------------------------------------------------------------------
* Managed Object definitions
*
* Description:     The representation of unsigned 16-bits value
*
*--------------------------------------------------------------------------*/
typedef uint16 MIB_UINT16;

/*--------------------------------------------------------------------------
* Managed Object definitions
*
* Description:     The representation of unsigned 32-bits value
*
*--------------------------------------------------------------------------*/
typedef uint32 MIB_UINT32;

/*--------------------------------------------------------------------------
* Managed Object definitions
*
* Description:     The representation of unsigned 64-bits value
*
*--------------------------------------------------------------------------*/
#ifdef __ghs
typedef struct _MIB_UINT64
{
    uint32 ui32Low;
    uint32 ui32High;
} MIB_UINT64;
#else
typedef uint64 MIB_UINT64;
#endif

/*--------------------------------------------------------------------------
* Managed Object definitions
*
* Description:     TPC Object definitions
*
* Representation:
*
*--------------------------------------------------------------------------*/

#define MAXIMUM_BANDWIDTHS_GEN5		(3)
#define MAXIMUM_BANDWIDTHS_GEN6		(4)


#define TPC_FREQ_EXPECTED_SIZE	(24)

#define TPC_FREQ_XY_STR_NUM		(4)
#define TPC_FREQ_STR_NUM        (2 * MAXIMUM_BANDWIDTHS_GEN6)
#define TPC_FREQ_POINT_NUM      (5)

#define TPC_ANT_MIN 0
#define TPC_ANT_MAX 3

#define TPC_CH_ID_MIN 0
#define TPC_CH_ID_MAX 200

#define TPC_ULTI_EVM_GAIN_MIN 0
#define TPC_ULTI_EVM_GAIN_MAX 191

#define TPC_BW_MIN 0
#define TPC_BW_MAX 4

#define TPC_MAX_POWER_MIN 0
#define TPC_MAX_POWER_MAX 63

#define TPC_ULTI_EVM_INDEX_MIN 0
#define TPC_ULTI_EVM_INDEX_MAX 96


#ifdef PRAGMA_PACK
#pragma pack(push,1)
#endif

typedef struct _TPC_FREQ_XY_CAL
{
    uint8   chID;
    uint8   BackoffMultiplier;
    uint8   MaxTxPowerIndex;
    uint8   Backoff;
// These are occupy 4 bytes
    uint16  X_axis[ MTLK_PAD2( TPC_FREQ_POINT_NUM ) ];
// Array is assigned to 4-byte boundary
    uint8   Y_axis[ MTLK_PAD4( TPC_FREQ_POINT_NUM ) ];
} __attribute__((aligned(1), packed)) TPC_FREQ_XY_CAL;

#define TPC_FREQ_UEVM_BIT_SHIFT			0
#define TPC_FREQ_UEVM_MASK				(0x3F)
#define TPC_FREQ_BW_BIT_SHIFT			6
#define TPC_FREQ_BW_MASK				(0xC0)
#define TPC_FREQ_MAX_POWER_BIT_SHIFT	0
#define TPC_FREQ_MAX_POWER_MASK			(0x3F)
#define TPC_FREQ_ANT_NUM_BIT_SHIFT		6
#define TPC_FREQ_ANT_NUM_MASK			(0xC0)

typedef union
{
	uint8	val;
	struct
	{
#if defined (__BIG_ENDIAN_BITFIELD)
		uint8	BW			:2;
		uint8	ultimateEVM :6;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
		uint8	ultimateEVM :6;
		uint8	BW			:2;
#endif
	} uevm_bw_fields;
} __MTLK_PACKED uevm_bw;

typedef union
{
	uint8	val;
	struct
	{
#if defined (__BIG_ENDIAN_BITFIELD)
		uint8	antNum		:2;
		uint8	maxPower	:6;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
		uint8	maxPower	:6;
		uint8	antNum		:2;
#endif
	} maxp_ant_fields;
} __MTLK_PACKED maxp_ant;

typedef struct _TPC_FREQ_1S2D_NO_CALIB
{
    uint8   chID;
    maxp_ant maxp_ant;
    uevm_bw uevm_bw;
	uint8	ultimateEVMgain;
	int16	A1;
	int16	B1;
} __attribute__((aligned(1), packed)) TPC_FREQ_1S2D_NO_CALIB;

typedef struct _TPC_FREQ_2S2D_NO_CALIB
{
	uint8	chID;
    maxp_ant maxp_ant;
    uevm_bw uevm_bw;
	uint8	ultimateEVMgain;
	int16	A1;
	int16	B1;
	int16	A2;
	int16	B2;
} __attribute__((aligned(1), packed)) TPC_FREQ_2S2D_NO_CALIB;

typedef struct _TPC_FREQ_3S2D_NO_CALIB
{
	uint8	chID;
    maxp_ant maxp_ant;
    uevm_bw uevm_bw;
	uint8	ultimateEVMgain;
	int16	A1;
	int16	B1;
	int16	A2;
	int16	B2;
	int16	A3;
	int16	B3;
} __attribute__((aligned(1), packed)) TPC_FREQ_3S2D_NO_CALIB;

typedef struct _TPC_FREQ_1S2D
{
	uint8   chID;
    maxp_ant maxp_ant;
    uevm_bw uevm_bw;
	union
	{
		uint16 val;
		struct
		{
#if defined (__BIG_ENDIAN_BITFIELD)
			uint16	S2Do1			:5;
			uint16	S2Dg1			:4;
			uint16	ultimateEVMgain :7;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
			uint16	ultimateEVMgain :7;
			uint16	S2Dg1			:4;
			uint16	S2Do1			:5;
#endif
		} uevmg_s2d_fields;
	} uevmg_s2d;
	int16   A1;
	int16   B1;
} __attribute__((aligned(1), packed)) TPC_FREQ_1S2D;

typedef struct _TPC_FREQ_2S2D
{
	uint8   chID;
    maxp_ant maxp_ant;
    uevm_bw uevm_bw;
	uint8   ultimateEVMgain;
	union
	{
		uint8	val3;
		struct
		{
#if defined (__BIG_ENDIAN_BITFIELD)
			uint8	S2Dg2		:4;
			uint8	S2Dg1		:4;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
			uint8	S2Dg1		:4;
			uint8	S2Dg2		:4;
#endif
		} s2d_fields;
	} s2d;
	uint8   S2Do1;
	uint8   S2Do2;
	int16   A1;
	int16   B1;
	int16   A2;
	int16   B2;
} __attribute__((aligned(1), packed)) TPC_FREQ_2S2D;

typedef struct _TPC_FREQ_3S2D
{
	uint8   chID;
    maxp_ant maxp_ant;
    uevm_bw uevm_bw;
	uint8   ultimateEVMgain;
	union
	{
		uint32	val3;
		struct
		{
#if defined (__BIG_ENDIAN_BITFIELD)
			uint32	pad2		:3;
			uint32	S2Do3		:5;
			uint32	pad1		:2;
			uint32	S2Do2		:5;
			uint32	S2Dg3		:4;
			uint32	S2Do1		:5;
			uint32	S2Dg2		:4;
            uint32	S2Dg1		:4;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
            uint32	S2Dg1		:4;
			uint32	S2Dg2		:4;
			uint32	S2Do1		:5;
			uint32	S2Dg3		:4;
			uint32	S2Do2		:5;
			uint32	pad1		:2;
			uint32	S2Do3		:5;
			uint32	pad2		:3;
#endif
		} s2d_fields;
	} s2d;
	int16   A1;
	int16   B1;
	int16   A2;
	int16   B2;
	int16   A3;
	int16   B3;
} __attribute__((aligned(1), packed)) TPC_FREQ_3S2D;

typedef struct _TPC_FREQ_1_REGION
{
#if defined (__BIG_ENDIAN_BITFIELD)
	uint16	bw		:4;
	uint16	chID	:12;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
	uint16	chID	:12;
	uint16	bw		:4;
#endif
	uint8	maxp;
	uint8	uevm_index;
	uint8	uevm_gain;
	uint8	S2Dg1;
	uint8	S2Do1;
	int16   A1;
	int16   B1;
} __attribute__((aligned(1), packed)) TPC_FREQ_1_REGION;
typedef struct _TPC_FREQ_2_REGION
{
#if defined (__BIG_ENDIAN_BITFIELD)
	uint16	bw		:4;
	uint16	chID	:12;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
	uint16	chID	:12;
	uint16	bw		:4;
#endif
	uint8	maxp;
	uint8	uevm_index;
	uint8	uevm_gain;	
#if defined (__BIG_ENDIAN_BITFIELD)
	uint8	S2Dg2	:4;
	uint8	S2Dg1	:4;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
	uint8	S2Dg1	:4;
	uint8	S2Dg2	:4;
#endif
	uint8	S2Do1;
	uint8	S2Do2;
	int16   A1;
	int16   B1;
	int16   A2;
	int16   B2;
}__attribute__((aligned(1), packed)) TPC_FREQ_2_REGION;
typedef struct _TPC_FREQ_3_REGION
{
#if defined (__BIG_ENDIAN_BITFIELD)
	uint16	bw		:4;
	uint16	chID	:12;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
	uint16	chID	:12;
	uint16	bw		:4;
#endif
	uint8	maxp;
	uint8	uevm_index;
	uint8	uevm_gain;
#if defined (__BIG_ENDIAN_BITFIELD)
	uint8	S2Dg2	:4;
	uint8	S2Dg1	:4;
#else /* (__LITTLE_ENDIAN_BITFIELD) */
	uint8	S2Dg1	:4;
	uint8	S2Dg2	:4;
#endif
	uint8	S2Dg3;
	uint8	S2Do1;
	uint8	S2Do2;
	uint8	S2Do3;
	int16   A1;
	int16   B1;
	int16   A2;
	int16   B2;
	int16	A3;
	int16	B3;
}__attribute__((aligned(1), packed)) TPC_FREQ_3_REGION;
typedef union _TPC_FREQ
{
	TPC_FREQ_XY_CAL				tpcFreqXYcal;
	TPC_FREQ_1S2D_NO_CALIB		tpcFreq1s2dNoCal;
	TPC_FREQ_2S2D_NO_CALIB		tpcFreq2s2dNoCal;
	TPC_FREQ_3S2D_NO_CALIB		tpcFreq3s2dNoCal;
	TPC_FREQ_1S2D				tpcFreq1s2d;
	TPC_FREQ_2S2D				tpcFreq2s2d;
	TPC_FREQ_3S2D				tpcFreq3s2d;
	TPC_FREQ_1_REGION			tpcFreq1Region;
	TPC_FREQ_2_REGION			tpcFreq2Region;
	TPC_FREQ_3_REGION			tpcFreq3Region;
} TPC_FREQ;

typedef union _tpcFreqList
{
	TPC_FREQ	tpcFreqXY[TPC_FREQ_XY_STR_NUM];
	TPC_FREQ	tpcFreq[TPC_FREQ_STR_NUM];
} tpcFreqList_t;

typedef struct tpcAntParams
{
	uint8			ant;
	uint8			tpcFreqLen;
	uint16			padding;
	tpcFreqList_t	tpcParams;
	uint8 			Status;
	uint8   		Reserved[3];
} tpcAntParams_t;

#define REGULATION_LIMIT_MIN 0
#define REGULATION_LIMIT_MAX 255

#define REGULATION_LIMIT_MU_MIN 0
#define REGULATION_LIMIT_MU_MAX 255

#define REGULATION_LIMIT_BF_MIN 0
#define REGULATION_LIMIT_BF_MAX 255

#define POWER_LIMIT_11B_MIN 0
#define POWER_LIMIT_11B_MAX 255

#define TPC_LOOP_TYPE_MIN  0
#define TPC_LOOP_TYPE_MAX  1


typedef struct tpcParameters
{
	int16		regulationLimit[MAXIMUM_BANDWIDTHS_GEN6];
	int16       regulationLimitMU[MAXIMUM_BANDWIDTHS_GEN6];
	int16       regulationLimitBF[MAXIMUM_BANDWIDTHS_GEN6];
	int16		powerLimit11b;
	uint8		calType;
	uint8		tpcLoopType;
	uint8		fixedGain;
	uint8		getSetOperation;
	uint8 		Status;
	uint8   	Reserved[3];
} tpcConfig_t;




#ifdef PRAGMA_PACK
#pragma pack(pop)
#endif

#define NO_FIXED_GAIN	(0xFF)







/*--------------------------------------------------------------------------
 * Managed Object Type Definitions
 *
 * Description:     EEPROM Version ID, Number of points in TPC_FREQ structure
 *
 * Representation:  MIB_MANUFACTURER_ID
 *
 *--------------------------------------------------------------------------*/
typedef struct __EEPROM_VERSION_TYPE
{
 	uint16 u16EEPROMVersion;
 	uint8 u8NumberOfPoints5GHz;
 	uint8 u8NumberOfPoints2GHz;
} __MTLK_PACKED EEPROM_VERSION_TYPE;


/*--------------------------------------------------------------------------
 * Managed Object Type Definition
 *
 * Description:     ESS ID
 *
 * Representation:  MIB_ESSID
 *
 *--------------------------------------------------------------------------*/
/* Maximum length of ESSID */
#define     MIB_ESSID_LENGTH                32

typedef struct _MIB_ESS_ID
{
    /* ESSID is a counted string.
    * A zero length implies broadcast address for scanning */
    uint8 u8Length;
    uint8 reserved[3];
    char  acESSID[MTLK_PAD4(MIB_ESSID_LENGTH + 1)]; /* + 1 allows zero termination for debug output */
} __MTLK_PACKED MIB_ESS_ID;

/*--------------------------------------------------------------------------
 * Managed Object Type Definition
 *
 * Description:     Set of uint8 integers.
 *
 * Used to represent sets (e.g. authentication algorithms) of small  integers.
 *--------------------------------------------------------------------------*/
#define MIB_SET_LENGTH                      32

typedef struct _MIB_SET_OF_U8
{
    uint8 u8Nelements;         /* Number of elements in set */
    uint8 reserved[3];
    uint8 au8Elements[MTLK_PAD4(MIB_SET_LENGTH)];
} __MTLK_PACKED MIB_SET_OF_U8;


/*--------------------------------------------------------------------------
 * Managed Object Type Definition
 *
 * Description:     List of uint8 integers.
 *
 * Used to represent lists of small integers.
 *--------------------------------------------------------------------------*/
#define MIB_LIST_LENGTH                     32

typedef struct _MIB_LIST_OF_U8
{
    uint8  au8Elements[MTLK_PAD4(MIB_LIST_LENGTH)];
} __MTLK_PACKED MIB_LIST_OF_U8;

/*--------------------------------------------------------------------------
 * Managed Object Type Definition
 *
 * Description:     Default WEP Keys
 *
 * Contains the default WEP keys
 *--------------------------------------------------------------------------*/
/* The WEP secret key is either 40bits or 104bits */
/* In each case, a 24bit IV is appended to create 64 or 128bit RC4 key */
/* but that is stored in a separate structure */
#define MIB_WEP_KEY_MAX_LENGTH              13

typedef struct _MIB_WEP_KEY
{
    uint8 u8KeyLength;  /* Note: Length is used to infer type */
    uint8 reserved[3];
    uint8 au8KeyData[MTLK_PAD4(MIB_WEP_KEY_MAX_LENGTH)];
} __MTLK_PACKED MIB_WEP_KEY;


#define MIB_WEP_N_DEFAULT_KEYS              4

typedef struct _MIB_WEP_DEF_KEYS
{
    MIB_WEP_KEY  sKey[MTLK_PAD4(MIB_WEP_N_DEFAULT_KEYS)];
} __MTLK_PACKED MIB_WEP_DEF_KEYS;

#define BSS_BASIC_RATE_FLAG                 0x80;

/*
 * MAC related managed objects as specified in IEEE 802.11
 *   For detailed descriptions see IEEE 802.11,
 *   For other information (type etc) see hi_motab.h
 */

#define MAC_MO_BASE                         0x0000
#define MIB_KALISH_THE_INVALID_ID           0x0000




#define MIB_DTIM_PERIOD                     0x0016

#define MIB_LONG_RETRY_LIMIT                0x0020



#define MIB_BEACON_PERIOD                   0x0038
#define MIB_DEFAULT_BEACON_PERIOD	        100 /* TU */

#define MIB_IEEE_ADDRESS                    0x0040

#define MIB_PRE_ACTIVATE                    0x0109

/* PHY Managed Objects */
#define PHY_MO_BASE 0x200

#define     MIB_PHY_TYPE_FHSS     1     /* hopping radio */
#define     MIB_PHY_TYPE_DSS      2     /* direct sequence radio */
#define     MIB_PHY_TYPE_IR       3     /* Infrared */
#define     MIB_PHY_TYPE_OFDM     4     /* OFDM for 80211.b  */



/* 802.11h - new MIB */


/*----PRIVATE MANAGED OBJECTS-----------------------------------------------
 * The following managed objects are not part of the 802.11 specification,
 * but are required for the operation of the MAC software.
 *--------------------------------------------------------------------------*/
#define     MIB_PRIVATE_BASE                0x1000

/*--------------------------------------------------------------------------
 * Managed Object:      MIB_MAC_ENABLED
 *
 * Description:     Enable state of mac.   False (zero) prior to a
 *                  MIB_R_ENABLE_MAC request.   True (non-zero) after.
 *
 * Representation:  uint16 u16Nbytes
 *
 * Access:          r   at any time
 *--------------------------------------------------------------------------*/
#define     MIB_MAC_ENABLED                 0x1003


/*--------------------------------------------------------------------------
 * Managed Object:      MIB_EEPROM_VERSION
 *
 * Description:     EEPROM version number
 * Representation:  uint16
 *
 * Access:          w
  *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * Managed Object:  MIB_SHORT_PREAMBLE_OPTION_IMPLEMENTED
 *
 * Description:     802.11b/g. In APs (and STAs in a IBSS), indicates that
 *                  it has PHY supporting short preamble.
 *
 * Representation:  uint8 - boolean
 *
 * Access:          rw
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * Managed Object:  MIB_SHORT_SLOT_TIME_OPTION_ENABLED_11G
 *
 * Description:     When TRUE the STA Short Slot option has been implemented.
 *
 *
 * Representation:  uint8 - boolean
 *
 * Access:          rw
 *--------------------------------------------------------------------------*/


/*----WME MANAGED OBJECTS---------------------------------------------------
 * Managed objects for 802.11 WME extensions
 *--------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 * Managed Objects:  MIB_WME_XXX
 *
 * Description:     Configuration of WME parameters.
 *
 *
 * Representation:  various
 *
 * Access:          rw at an AP r at a STA
 *--------------------------------------------------------------------------*/
#define     MIB_WME_PARAMETERS              0x1050
#define     MIB_WME_OUI_AND_VERSION         0x1052

#define     MIB_WME_QAP_PARAMETERS          0x1056

/*----RSN MANAGED OBJECTS---------------------------------------------------
 * Managed objects for 802.11i RSN management
 *--------------------------------------------------------------------------*/
#define     MIB_RSN_BASE                    0x1100



/*--------------------------------------------------------------------------
 * Managed Object:  MIB_CALIBRATION_ALGO_MASK
 *
 * Description:     Bitmasks of required calibr
 *
 * Representation:  uint32 (bitmask)tion procedures
 *
 * Access:          r
 *--------------------------------------------------------------------------*/
#define 	TPC_CLOSED_LOOP				0 //use on TPC Config
#define 	TPC_OPEN_LOOP				1 //use on TPC Config
#define		TPC_BIT_VAL			MTLK_BFIELD_INFO(8, 1)  // The bit representing the TPC
#define		TPC_BIT_VAL_LOOP_OPEN       (0)
#define		TPC_BIT_VAL_LOOP_CLOSED     (1)

/*--------------------------------------------------------------------------
 * Managed Object:  MIB_MT_PHY_CFG
 *
 * Description:     Advanced PHY parameters
 *--------------------------------------------------------------------------*/
#define     MIB_MT_PHY_CFG                  0x1200

#define     MIB_SPECTRUM_MODE                   (MIB_MT_PHY_CFG + 0x02)

#define     MIB_SPECTRUM_20M    0
#define     MIB_SPECTRUM_40M    1
/*--------------------------------------------------------------------------
 * Managed Object:  MIB_MT_SW_CFG
 *
 * Description:     Advanced SW parameters
 *--------------------------------------------------------------------------*/
#define     MIB_MT_SW_CFG                   0x1300


#define     MIB_ACL_MODE                    (MIB_MT_SW_CFG + 0x21)
#define     MIB_ACL							(MIB_MT_SW_CFG + 0x22)
#define     MIB_ACL_MASKS					(MIB_MT_SW_CFG + 0x24)

#define     MIB_AP_MAX_PKTS_IN_SP			(MIB_MT_SW_CFG + 0x25)


/* ACL Definitions */
#define     MAX_ADDRESSES_IN_ACL 32


/*-- Type definitions -----------------------------------------------------*/

typedef uint16 MIB_ID;

typedef union _MIB_VALUE
{
    MIB_UINT8                   u8Uint8;                    /* Generic tags for test harness */
    MIB_UINT16                  u16Uint16;
    MIB_UINT32                  u32Uint32;
    MIB_UINT64                  u64Uint64;
    MIB_LIST_OF_U8              au8ListOfu8;
    MIB_SET_OF_U8               sSetOfu8;
    MIB_WEP_DEF_KEYS            sDefaultWEPKeys;
    MIB_WME_TABLE               sWMEParameters;
    FM_WME_OUI_AND_VER          sWMEOuiandVer;
    MIB_ESS_ID                  sDesiredESSID;
    PRE_ACTIVATE_MIB_TYPE       sPreActivateType;
    EEPROM_VERSION_TYPE         sEepromInfo;
} __MTLK_PACKED MIB_VALUE;

typedef struct _MIB_OBJECT
{
    MIB_ID                  u16ID;
    MIB_VALUE               uValue;
} __MTLK_PACKED MIB_OBJECT;

#define   MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* !__MHI_MIB_ID_INCLUDED */
