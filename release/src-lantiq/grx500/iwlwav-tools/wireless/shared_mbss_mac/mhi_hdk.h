/******************************************************************************

                               Copyright (c) 2012
                            Lantiq Deutschland GmbH

  For licensing information, see the file 'LICENSE' in the root folder of
  this software module.

******************************************************************************/
/***************************************************************
 File:	mhi_hdk.h
 Module:	
 Purpose: 	
 Description: 
***************************************************************/
#ifndef __MHI_HDK_INCLUDED_H
#define __MHI_HDK_INCLUDED_H

//---------------------------------------------------------------------------------
//						Includes									
//---------------------------------------------------------------------------------
//#define   MTLK_PACK_ON
//#include "mtlkpack.h"

//---------------------------------------------------------------------------------
//						Defines									
//---------------------------------------------------------------------------------


//--------------- Data Structures ---------------------------------------------------

typedef struct CorrResults_tag
{
	int32	II;
	int32	QQ;
	int32	IQ;
}CorrRes_t;

typedef struct S2dParams_tag
{
	uint32 antMask;
	int32 gain;
	int32 iOffset;
	int8 regionIndex; //0 or 1
	int8 reserved[3];
} S2dParams_t;

typedef struct DutRssiOffsetGain_tag
{
	uint32 antMask;
    uint16 rssiResult[4]; //Out
    int Method;
    uint16 NOS;	
} DutRssiOffsetGain_t;

typedef struct RssiCwPower_tag
{
	CorrRes_t corrResults[4];
	uint32 antMask;	
	int method;//not used
	int FreqOffset;// not used
	int NOS;// not used
} RssiCwPower_t;

typedef struct RssiGainBlock_tag
{
	uint8 antMask;
	int8 lnaIndex;
	int8 pgc1;
	int8 pgc2;
	int8 pgc3;
} RssiGainBlock_t;




//#define MTLK_PACK_OFF
//#include "mtlkpack.h"

//---------------------------------------------------------------------------------
//						Includes									
//---------------------------------------------------------------------------------
#define   MTLK_PACK_ON
#include "mtlkpack.h"

//---------------------------------------------------------------------------------
//						Defines									
//---------------------------------------------------------------------------------
//#define MAXIMUM_BANDWIDTHS_GEN5		(3)
#define MAXIMUM_BANDWIDTHS_GEN6		(4)

/*--------------------------------------------------------------------------
* Managed Object definitions
*
* Description:     TPC Object definitions
*
* Representation:
*
*--------------------------------------------------------------------------*/
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
} __attribute__((aligned(1), packed)) TPC_FREQ_2_REGION;
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
} __attribute__((aligned(1), packed)) TPC_FREQ_3_REGION;
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
} __MTLK_PACKED TPC_FREQ;

typedef union _tpcFreqList_t
{
	TPC_FREQ	tpcFreqXY[TPC_FREQ_XY_STR_NUM];
	TPC_FREQ	tpcFreq[TPC_FREQ_STR_NUM];
} __MTLK_PACKED tpcFreqList_t;

typedef struct tpcAntParams
{
	uint8			ant;
	uint8			tpcFreqLen;
	uint16			padding;
	tpcFreqList_t	tpcParams;
	uint8 			Status;
	uint8   		Reserved[3];
} __MTLK_PACKED tpcAntParams_t;

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

#define MAXIMUM_BANDWIDTHS_11N 2
typedef struct tpcParameters
{
	int16		regulationLimit[MAXIMUM_BANDWIDTHS_GEN6];	/* 11AC */
	int16		regulationLimitMU[MAXIMUM_BANDWIDTHS_GEN6];
	int16		regulationLimitBF[MAXIMUM_BANDWIDTHS_GEN6];
	int16		regulationLimitBF_AX[MAXIMUM_BANDWIDTHS_GEN6];
	int16		regulationLimitAX[MAXIMUM_BANDWIDTHS_GEN6];
	int16		regulationLimitN[MAXIMUM_BANDWIDTHS_11N];
	int16		regulationLimitAG;
	int16		powerLimit11b;
	uint8		calType;
	uint8		tpcLoopType;
	uint8		fixedGain;
 	uint8		getSetOperation;
	uint8		Status;
	uint8		Reserved[3];
} __MTLK_PACKED tpcConfig_t;



#ifdef PRAGMA_PACK
#pragma pack(pop)
#endif


#define MTLK_PACK_OFF
#include "mtlkpack.h"

#endif /* !__MHI_HDK_INCLUDED_H */

