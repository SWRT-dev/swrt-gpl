
/***********************************************************************************
File:				PacRxfRegs.h
Module:				pacRxf
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_RXF_REGS_H_
#define _PAC_RXF_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PAC_RXF_BASE_ADDRESS                             MEMORY_MAP_UNIT_7_BASE_ADDRESS
#define	REG_PAC_RXF_BYTE_FILTER_CONTROL_0                 (PAC_RXF_BASE_ADDRESS + 0x0)
#define	REG_PAC_RXF_BYTE_FILTER_CONTROL_1                 (PAC_RXF_BASE_ADDRESS + 0x4)
#define	REG_PAC_RXF_BYTE_FILTER_CONTROL_2                 (PAC_RXF_BASE_ADDRESS + 0x8)
#define	REG_PAC_RXF_SF_MATCH_VECTOR                       (PAC_RXF_BASE_ADDRESS + 0xC)
#define	REG_PAC_RXF_LONG_FILTERS_SEL_0TO31                (PAC_RXF_BASE_ADDRESS + 0x10)
#define	REG_PAC_RXF_LONG_FILTERS_SEL                      (PAC_RXF_BASE_ADDRESS + 0x14)
#define	REG_PAC_RXF_LONG_FILTER_0_CONTROL                 (PAC_RXF_BASE_ADDRESS + 0x18)
#define	REG_PAC_RXF_LONG_FILTER_1_CONTROL                 (PAC_RXF_BASE_ADDRESS + 0x1C)
#define	REG_PAC_RXF_LONG_FILTER_2_CONTROL                 (PAC_RXF_BASE_ADDRESS + 0x20)
#define	REG_PAC_RXF_LONG_FILTER_0_REF1                    (PAC_RXF_BASE_ADDRESS + 0x24)
#define	REG_PAC_RXF_LONG_FILTER_0_REF2                    (PAC_RXF_BASE_ADDRESS + 0x28)
#define	REG_PAC_RXF_LONG_FILTER_1_REF1                    (PAC_RXF_BASE_ADDRESS + 0x2C)
#define	REG_PAC_RXF_LONG_FILTER_1_REF2                    (PAC_RXF_BASE_ADDRESS + 0x30)
#define	REG_PAC_RXF_LONG_FILTER_2_REF1                    (PAC_RXF_BASE_ADDRESS + 0x34)
#define	REG_PAC_RXF_LONG_FILTER_2_REF2                    (PAC_RXF_BASE_ADDRESS + 0x38)
#define	REG_PAC_RXF_LONG_FILTER_3_REF1                    (PAC_RXF_BASE_ADDRESS + 0x3C)
#define	REG_PAC_RXF_LONG_FILTER_3_REF2                    (PAC_RXF_BASE_ADDRESS + 0x40)
#define	REG_PAC_RXF_LONG_FILTER_4_REF1                    (PAC_RXF_BASE_ADDRESS + 0x44)
#define	REG_PAC_RXF_LONG_FILTER_4_REF2                    (PAC_RXF_BASE_ADDRESS + 0x48)
#define	REG_PAC_RXF_LONG_FILTER_5_REF1                    (PAC_RXF_BASE_ADDRESS + 0x4C)
#define	REG_PAC_RXF_LONG_FILTER_5_REF2                    (PAC_RXF_BASE_ADDRESS + 0x50)
#define	REG_PAC_RXF_LONG_FILTER_6_REF1                    (PAC_RXF_BASE_ADDRESS + 0x54)
#define	REG_PAC_RXF_LONG_FILTER_6_REF2                    (PAC_RXF_BASE_ADDRESS + 0x58)
#define	REG_PAC_RXF_LONG_FILTER_7_REF1                    (PAC_RXF_BASE_ADDRESS + 0x5C)
#define	REG_PAC_RXF_LONG_FILTER_7_REF2                    (PAC_RXF_BASE_ADDRESS + 0x60)
#define	REG_PAC_RXF_LONG_FILTER_8_REF1                    (PAC_RXF_BASE_ADDRESS + 0x64)
#define	REG_PAC_RXF_LONG_FILTER_8_REF2                    (PAC_RXF_BASE_ADDRESS + 0x68)
#define	REG_PAC_RXF_LONG_FILTER_9_REF1                    (PAC_RXF_BASE_ADDRESS + 0x6C)
#define	REG_PAC_RXF_LONG_FILTER_9_REF2                    (PAC_RXF_BASE_ADDRESS + 0x70)
#define	REG_PAC_RXF_LONG_FILTER_10_REF1                   (PAC_RXF_BASE_ADDRESS + 0x74)
#define	REG_PAC_RXF_LONG_FILTER_10_REF2                   (PAC_RXF_BASE_ADDRESS + 0x78)
#define	REG_PAC_RXF_LONG_FILTER_11_REF1                   (PAC_RXF_BASE_ADDRESS + 0x7C)
#define	REG_PAC_RXF_LONG_FILTER_11_REF2                   (PAC_RXF_BASE_ADDRESS + 0x80)
#define	REG_PAC_RXF_LONG_FILTER_12_REF1                   (PAC_RXF_BASE_ADDRESS + 0x84)
#define	REG_PAC_RXF_LONG_FILTER_12_REF2                   (PAC_RXF_BASE_ADDRESS + 0x88)
#define	REG_PAC_RXF_LONG_FILTER_13_REF1                   (PAC_RXF_BASE_ADDRESS + 0x8C)
#define	REG_PAC_RXF_LONG_FILTER_13_REF2                   (PAC_RXF_BASE_ADDRESS + 0x90)
#define	REG_PAC_RXF_LONG_FILTER_14_REF1                   (PAC_RXF_BASE_ADDRESS + 0x94)
#define	REG_PAC_RXF_LONG_FILTER_14_REF2                   (PAC_RXF_BASE_ADDRESS + 0x98)
#define	REG_PAC_RXF_LONG_FILTER_15_REF1                   (PAC_RXF_BASE_ADDRESS + 0x9C)
#define	REG_PAC_RXF_LONG_FILTER_15_REF2                   (PAC_RXF_BASE_ADDRESS + 0xA0)
#define	REG_PAC_RXF_LONG_FILTER_16_REF1                   (PAC_RXF_BASE_ADDRESS + 0xA4)
#define	REG_PAC_RXF_LONG_FILTER_16_REF2                   (PAC_RXF_BASE_ADDRESS + 0xA8)
#define	REG_PAC_RXF_LONG_FILTER_17_REF1                   (PAC_RXF_BASE_ADDRESS + 0xAC)
#define	REG_PAC_RXF_LONG_FILTER_17_REF2                   (PAC_RXF_BASE_ADDRESS + 0xB0)
#define	REG_PAC_RXF_LF0_MATCH_VECTOR                      (PAC_RXF_BASE_ADDRESS + 0xB4)
#define	REG_PAC_RXF_LF1_MATCH_VECTOR                      (PAC_RXF_BASE_ADDRESS + 0xB8)
#define	REG_PAC_RXF_LF2_MATCH_VECTOR                      (PAC_RXF_BASE_ADDRESS + 0xBC)
#define	REG_PAC_RXF_WRAPPER_FRAME_ENABLE                  (PAC_RXF_BASE_ADDRESS + 0xC0)
#define	REG_PAC_RXF_MAX_MPDU_LENGTH_FILTER_EN             (PAC_RXF_BASE_ADDRESS + 0x100)
#define	REG_PAC_RXF_MAX_MPDU_LENGTH_11AG_B                (PAC_RXF_BASE_ADDRESS + 0x104)
#define	REG_PAC_RXF_MAX_MPDU_LENGTH_11N_AC                (PAC_RXF_BASE_ADDRESS + 0x108)
#define	REG_PAC_RXF_BSS_FROM_TO_DS_00_VAP_FILTER_EN       (PAC_RXF_BASE_ADDRESS + 0x10C)
#define	REG_PAC_RXF_BSS_FROM_TO_DS_01_VAP_FILTER_EN       (PAC_RXF_BASE_ADDRESS + 0x110)
#define	REG_PAC_RXF_BSS_FROM_TO_DS_10_VAP_FILTER_EN       (PAC_RXF_BASE_ADDRESS + 0x114)
#define	REG_PAC_RXF_BSS_FROM_TO_DS_11_VAP_FILTER_EN       (PAC_RXF_BASE_ADDRESS + 0x118)
#define	REG_PAC_RXF_A_MPDU_NO_BA_AGREEMENT_FILTER_EN      (PAC_RXF_BASE_ADDRESS + 0x11C)
#define	REG_PAC_RXF_RXF_FIFO_BASE_ADDR                    (PAC_RXF_BASE_ADDRESS + 0x120)
#define	REG_PAC_RXF_RXF_FIFO_DEPTH_MINUS_ONE              (PAC_RXF_BASE_ADDRESS + 0x124)
#define	REG_PAC_RXF_RXF_FIFO_CLEAR_STRB                   (PAC_RXF_BASE_ADDRESS + 0x128)
#define	REG_PAC_RXF_RXF_FIFO_RD_ENTRIES_NUM               (PAC_RXF_BASE_ADDRESS + 0x12C)
#define	REG_PAC_RXF_RXF_FIFO_NUM_ENTRIES_COUNT            (PAC_RXF_BASE_ADDRESS + 0x130)
#define	REG_PAC_RXF_RXF_FIFO_DEBUG                        (PAC_RXF_BASE_ADDRESS + 0x134)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP0_CTRL      (PAC_RXF_BASE_ADDRESS + 0x138)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP0_VALUE     (PAC_RXF_BASE_ADDRESS + 0x13C)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP0_MASK      (PAC_RXF_BASE_ADDRESS + 0x140)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP1_CTRL      (PAC_RXF_BASE_ADDRESS + 0x144)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP1_VALUE     (PAC_RXF_BASE_ADDRESS + 0x148)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP1_MASK      (PAC_RXF_BASE_ADDRESS + 0x14C)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP2_CTRL      (PAC_RXF_BASE_ADDRESS + 0x150)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP2_VALUE     (PAC_RXF_BASE_ADDRESS + 0x154)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP2_MASK      (PAC_RXF_BASE_ADDRESS + 0x158)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP3_CTRL      (PAC_RXF_BASE_ADDRESS + 0x15C)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP3_VALUE     (PAC_RXF_BASE_ADDRESS + 0x160)
#define	REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP3_MASK      (PAC_RXF_BASE_ADDRESS + 0x164)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP0_CTRL    (PAC_RXF_BASE_ADDRESS + 0x168)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP0_CTRL    (PAC_RXF_BASE_ADDRESS + 0x16C)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP0_CTRL    (PAC_RXF_BASE_ADDRESS + 0x170)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP0_CTRL    (PAC_RXF_BASE_ADDRESS + 0x174)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP1_CTRL    (PAC_RXF_BASE_ADDRESS + 0x178)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP1_CTRL    (PAC_RXF_BASE_ADDRESS + 0x17C)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP1_CTRL    (PAC_RXF_BASE_ADDRESS + 0x180)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP1_CTRL    (PAC_RXF_BASE_ADDRESS + 0x184)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP2_CTRL    (PAC_RXF_BASE_ADDRESS + 0x188)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP2_CTRL    (PAC_RXF_BASE_ADDRESS + 0x18C)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP2_CTRL    (PAC_RXF_BASE_ADDRESS + 0x190)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP2_CTRL    (PAC_RXF_BASE_ADDRESS + 0x194)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP3_CTRL    (PAC_RXF_BASE_ADDRESS + 0x198)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP3_CTRL    (PAC_RXF_BASE_ADDRESS + 0x19C)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP3_CTRL    (PAC_RXF_BASE_ADDRESS + 0x1A0)
#define	REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP3_CTRL    (PAC_RXF_BASE_ADDRESS + 0x1A4)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP0         (PAC_RXF_BASE_ADDRESS + 0x1A8)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP1         (PAC_RXF_BASE_ADDRESS + 0x1AC)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP2         (PAC_RXF_BASE_ADDRESS + 0x1B0)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP3         (PAC_RXF_BASE_ADDRESS + 0x1B4)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP0          (PAC_RXF_BASE_ADDRESS + 0x1B8)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP1          (PAC_RXF_BASE_ADDRESS + 0x1BC)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP2          (PAC_RXF_BASE_ADDRESS + 0x1C0)
#define	REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP3          (PAC_RXF_BASE_ADDRESS + 0x1C4)
#define	REG_PAC_RXF_MPDU_FIELD_FILTERS_RESULT             (PAC_RXF_BASE_ADDRESS + 0x1C8)
#define	REG_PAC_RXF_MPDU_FIELD_FILTERS_CTR_A              (PAC_RXF_BASE_ADDRESS + 0x1CC)
#define	REG_PAC_RXF_MPDU_FIELD_FILTERS_CTR_B              (PAC_RXF_BASE_ADDRESS + 0x1D0)
#define	REG_PAC_RXF_MPDU_FIELD_FILTERS_CLEAR_CTR          (PAC_RXF_BASE_ADDRESS + 0x1D4)
#define	REG_PAC_RXF_ADDR1_3_MULTICAST_BROADCAST_IND_EN    (PAC_RXF_BASE_ADDRESS + 0x1D8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_RXF_BYTE_FILTER_CONTROL_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 byteFilterControl0:8;	// RXF  byte filter control 0
		uint32 maskFilterControl0:8;	// RXF  mask
		uint32 offsetFilterControl0:7;	// RXF  offset
		uint32 reserved0:1;
		uint32 offsetWrapFilterControl0:7;	// RXF  offset wrapper
		uint32 reserved1:1;
	} bitFields;
} RegPacRxfByteFilterControl0_u;

/*REG_PAC_RXF_BYTE_FILTER_CONTROL_1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 byteFilterControl1:8;	// RXF  byte filter control 1
		uint32 maskfilterControl1:8;	// RXF  mask
		uint32 offsetFilterControl1:7;	// RXF  offset
		uint32 reserved0:1;
		uint32 offsetWrapFilterControl1:7;	// RXF  offset wrapper
		uint32 reserved1:1;
	} bitFields;
} RegPacRxfByteFilterControl1_u;

/*REG_PAC_RXF_BYTE_FILTER_CONTROL_2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 byteFilterControl2:8;	// RXF  byte filter control 2
		uint32 maskFilterControl2:8;	// RXF  mask
		uint32 offsetFilterControl2:7;	// RXF  offset
		uint32 reserved0:1;
		uint32 offsetWrapFilterControl2:7;	// RXF  offset wrapper
		uint32 reserved1:1;
	} bitFields;
} RegPacRxfByteFilterControl2_u;

/*REG_PAC_RXF_SF_MATCH_VECTOR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sfMatchVector:11;	// Short filters match vector
		uint32 reserved0:21;
	} bitFields;
} RegPacRxfSfMatchVector_u;

/*REG_PAC_RXF_LONG_FILTERS_SEL_0TO31 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selLf0:2;	// select Long Filter 0
		uint32 selLf1:2;	// select Long Filter 1
		uint32 selLf2:2;	// select Long Filter 2
		uint32 selLf3:2;	// select Long Filter 3
		uint32 selLf4:2;	// select Long Filter 4
		uint32 selLf5:2;	// select Long Filter 5
		uint32 selLf6:2;	// select Long Filter 6
		uint32 selLf7:2;	// select Long Filter 7
		uint32 selLf8:2;	// select Long Filter 8
		uint32 selLf9:2;	// select Long Filter 9
		uint32 selLf10:2;	// select Long Filter 10
		uint32 selLf11:2;	// select Long Filter 11
		uint32 selLf12:2;	// select Long Filter 12
		uint32 selLf13:2;	// select Long Filter 13
		uint32 selLf14:2;	// select Long Filter 14
		uint32 selLf15:2;	// select Long Filter 15
	} bitFields;
} RegPacRxfLongFiltersSel0To31_u;

/*REG_PAC_RXF_LONG_FILTERS_SEL 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 selLf16:2;	// select Long Filter 16
		uint32 selLf17:2;	// select Long Filter 17
		uint32 reserved0:28;
	} bitFields;
} RegPacRxfLongFiltersSel_u;

/*REG_PAC_RXF_LONG_FILTER_0_CONTROL 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter0Control:6;	// RXF  long filter 0 control
		uint32 reserved0:2;
		uint32 offsetLongFilter0Control:7;	// RXF  offset
		uint32 reserved1:17;
	} bitFields;
} RegPacRxfLongFilter0Control_u;

/*REG_PAC_RXF_LONG_FILTER_1_CONTROL 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter1Control:6;	// RXF  long filter 1 control
		uint32 reserved0:2;
		uint32 offsetLongFilter1Control:7;	// RXF  offset
		uint32 reserved1:17;
	} bitFields;
} RegPacRxfLongFilter1Control_u;

/*REG_PAC_RXF_LONG_FILTER_2_CONTROL 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter2Control:6;	// RXF  long filter 2 control
		uint32 reserved0:2;
		uint32 offsetLongFilter2Control:7;	// RXF  offset
		uint32 reserved1:17;
	} bitFields;
} RegPacRxfLongFilter2Control_u;

/*REG_PAC_RXF_LONG_FILTER_0_REF1 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter0Ref1:32;	// RXF  long filter 0 ref1
	} bitFields;
} RegPacRxfLongFilter0Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_0_REF2 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter0Ref2:16;	// RXF  long filter 0 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter0Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_1_REF1 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter1Ref1:32;	// RXF  long filter 1 ref1
	} bitFields;
} RegPacRxfLongFilter1Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_1_REF2 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter1Ref2:16;	// RXF  long filter 1 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter1Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_2_REF1 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter2Ref1:32;	// RXF  long filter 2 ref1
	} bitFields;
} RegPacRxfLongFilter2Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_2_REF2 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter2Ref2:16;	// RXF  long filter 2 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter2Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_3_REF1 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter3Ref1:32;	// RXF  long filter 3 ref1
	} bitFields;
} RegPacRxfLongFilter3Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_3_REF2 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter3Ref2:16;	// RXF  long filter 3 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter3Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_4_REF1 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter4Ref1:32;	// RXF  long filter 4 ref1
	} bitFields;
} RegPacRxfLongFilter4Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_4_REF2 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter4Ref2:16;	// RXF  long filter 4 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter4Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_5_REF1 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter5Ref1:32;	// RXF  long filter 5 ref1
	} bitFields;
} RegPacRxfLongFilter5Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_5_REF2 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter5Ref2:16;	// RXF  long filter 5 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter5Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_6_REF1 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter6Ref1:32;	// RXF  long filter 6 ref1
	} bitFields;
} RegPacRxfLongFilter6Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_6_REF2 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter6Ref2:16;	// RXF  long filter 6 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter6Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_7_REF1 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter7Ref1:32;	// RXF  long filter 7 ref1
	} bitFields;
} RegPacRxfLongFilter7Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_7_REF2 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter7Ref2:16;	// RXF  long filter 7 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter7Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_8_REF1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter8Ref1:32;	// RXF  long filter 8 ref1
	} bitFields;
} RegPacRxfLongFilter8Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_8_REF2 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter8Ref2:16;	// RXF  long filter 8 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter8Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_9_REF1 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter9Ref1:32;	// RXF  long filter 9 ref1
	} bitFields;
} RegPacRxfLongFilter9Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_9_REF2 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter9Ref2:16;	// RXF  long filter 9 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter9Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_10_REF1 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter10Ref1:32;	// RXF  long filter 10 ref1
	} bitFields;
} RegPacRxfLongFilter10Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_10_REF2 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter10Ref2:16;	// RXF  long filter 10 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter10Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_11_REF1 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter11Ref1:32;	// RXF  long filter 11 ref1
	} bitFields;
} RegPacRxfLongFilter11Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_11_REF2 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter11Ref2:16;	// RXF  long filter 11 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter11Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_12_REF1 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter12Ref1:32;	// RXF  long filter 12 ref1
	} bitFields;
} RegPacRxfLongFilter12Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_12_REF2 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter12Ref2:16;	// RXF  long filter 12 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter12Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_13_REF1 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter13Ref1:32;	// RXF  long filter 13 ref1
	} bitFields;
} RegPacRxfLongFilter13Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_13_REF2 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter13Ref2:16;	// RXF  long filter 13 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter13Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_14_REF1 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter14Ref1:32;	// RXF  long filter 14 ref1
	} bitFields;
} RegPacRxfLongFilter14Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_14_REF2 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter14Ref2:16;	// RXF  long filter 14 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter14Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_15_REF1 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter15Ref1:32;	// RXF  long filter 15 ref1
	} bitFields;
} RegPacRxfLongFilter15Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_15_REF2 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter15Ref2:16;	// RXF  long filter 15 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter15Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_16_REF1 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter16Ref1:32;	// RXF  long filter 16 ref1
	} bitFields;
} RegPacRxfLongFilter16Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_16_REF2 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter16Ref2:16;	// RXF  long filter 16 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter16Ref2_u;

/*REG_PAC_RXF_LONG_FILTER_17_REF1 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter17Ref1:32;	// RXF  long filter 17 ref1
	} bitFields;
} RegPacRxfLongFilter17Ref1_u;

/*REG_PAC_RXF_LONG_FILTER_17_REF2 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 longFilter17Ref2:16;	// RXF  long filter 17 ref2
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfLongFilter17Ref2_u;

/*REG_PAC_RXF_LF0_MATCH_VECTOR 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lf0MatchVector:18;	// Long filters 0 match vector
		uint32 reserved0:14;
	} bitFields;
} RegPacRxfLf0MatchVector_u;

/*REG_PAC_RXF_LF1_MATCH_VECTOR 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lf1MatchVector:18;	// Long filters 1 match vector
		uint32 reserved0:14;
	} bitFields;
} RegPacRxfLf1MatchVector_u;

/*REG_PAC_RXF_LF2_MATCH_VECTOR 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lf2MatchVector:18;	// Long filters 2 match vector
		uint32 reserved0:14;
	} bitFields;
} RegPacRxfLf2MatchVector_u;

/*REG_PAC_RXF_WRAPPER_FRAME_ENABLE 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wrapperFrameEnable:1;	// Wrapper frame mode enable
		uint32 reserved0:4;
		uint32 wrapperFrameLf1Enable:1;	// Wrapper frame long 1 enable
		uint32 wrapperFrameLf2Enable:1;	// Wrapper frame long 2 enable
		uint32 reserved1:1;
		uint32 wrapperFrameSf0Enable:1;	// Wrapper frame short 0 enable
		uint32 wrapperFrameSf1Enable:1;	// Wrapper frame short 1 enable
		uint32 wrapperFrameSf2Enable:1;	// Wrapper frame short 2 enable
		uint32 wrapperFrameSf3Enable:1;	// Wrapper frame short 3 enable
		uint32 wrapperFrameSf4Enable:1;	// Wrapper frame short 4 enable
		uint32 wrapperFrameSf5Enable:1;	// Wrapper frame short 5 enable
		uint32 wrapperFrameSf6Enable:1;	// Wrapper frame short 6 enable
		uint32 wrapperFrameSf7Enable:1;	// Wrapper frame short 7 enable
		uint32 wrapperFrameSf8Enable:1;	// Wrapper frame short 8 enable
		uint32 wrapperFrameSf9Enable:1;	// Wrapper frame short 9 enable
		uint32 wrapperFrameSf10Enable:1;	// Wrapper frame short 10 enable
		uint32 reserved2:13;
	} bitFields;
} RegPacRxfWrapperFrameEnable_u;

/*REG_PAC_RXF_MAX_MPDU_LENGTH_FILTER_EN 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLength11AgFilterEn:1;	// Enable Max MPDU size reject filter for 11a/g
		uint32 maxMpduLength11BFilterEn:1;	// Enable Max MPDU size reject filter for 11b
		uint32 maxMpduLength11NFilterEn:1;	// Enable Max MPDU size reject filter for 11n
		uint32 maxMpduLength11AcFilterEn:1;	// Enable Max MPDU size reject filter for 11ac
		uint32 reserved0:28;
	} bitFields;
} RegPacRxfMaxMpduLengthFilterEn_u;

/*REG_PAC_RXF_MAX_MPDU_LENGTH_11AG_B 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLength11Ag:16;	// Max MPDU size in case of PHY mode 11a/g
		uint32 maxMpduLength11B:16;	// Max MPDU size in case of PHY mode 11b
	} bitFields;
} RegPacRxfMaxMpduLength11AgB_u;

/*REG_PAC_RXF_MAX_MPDU_LENGTH_11N_AC 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxMpduLength11N:16;	// Max MPDU size in case of PHY mode 11n
		uint32 maxMpduLength11Ac:16;	// Max MPDU size in case of PHY mode 11ac
	} bitFields;
} RegPacRxfMaxMpduLength11NAc_u;

/*REG_PAC_RXF_BSS_FROM_TO_DS_00_VAP_FILTER_EN 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bssFromToDs00VapFilterEn:16;	// Enable BSS reject filter for {from_ds, to_ds} == 00. , Per VAP.
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfBssFromToDs00VapFilterEn_u;

/*REG_PAC_RXF_BSS_FROM_TO_DS_01_VAP_FILTER_EN 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bssFromToDs01VapFilterEn:16;	// Enable BSS reject filter for {from_ds, to_ds} == 01. , Per VAP.
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfBssFromToDs01VapFilterEn_u;

/*REG_PAC_RXF_BSS_FROM_TO_DS_10_VAP_FILTER_EN 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bssFromToDs10VapFilterEn:16;	// Enable BSS reject filter for {from_ds, to_ds} == 10. , Per VAP.
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfBssFromToDs10VapFilterEn_u;

/*REG_PAC_RXF_BSS_FROM_TO_DS_11_VAP_FILTER_EN 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bssFromToDs11VapFilterEn:16;	// Enable BSS reject filter for {from_ds, to_ds} == 11. , Per VAP.
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfBssFromToDs11VapFilterEn_u;

/*REG_PAC_RXF_A_MPDU_NO_BA_AGREEMENT_FILTER_EN 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aMpduNoBaAgreementFilterEn:1;	// Enable A-MPDU with no BA agreement reject filter
		uint32 reserved0:31;
	} bitFields;
} RegPacRxfAMpduNoBaAgreementFilterEn_u;

/*REG_PAC_RXF_RXF_FIFO_BASE_ADDR 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfFifoBaseAddr:24;	// RXF FIFO base address
		uint32 reserved0:8;
	} bitFields;
} RegPacRxfRxfFifoBaseAddr_u;

/*REG_PAC_RXF_RXF_FIFO_DEPTH_MINUS_ONE 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfFifoDepthMinusOne:8;	// RXF FIFO depth minus one
		uint32 reserved0:24;
	} bitFields;
} RegPacRxfRxfFifoDepthMinusOne_u;

/*REG_PAC_RXF_RXF_FIFO_CLEAR_STRB 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfFifoClearStrb:1;	// Clear RXF FIFO. , By write '1'.
		uint32 rxfFifoClearFullDropCtrStrb:1;	// Clear RXF FIFO full drop counter. , By write '1'.
		uint32 rxfFifoClearDecLessThanZeroStrb:1;	// Clear RXF FIFO decrement amount less than zero indication. , By write '1'.  , 
		uint32 reserved0:29;
	} bitFields;
} RegPacRxfRxfFifoClearStrb_u;

/*REG_PAC_RXF_RXF_FIFO_RD_ENTRIES_NUM 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfFifoRdEntriesNum:8;	// RXF FIFO number of entries to decrement.
		uint32 reserved0:24;
	} bitFields;
} RegPacRxfRxfFifoRdEntriesNum_u;

/*REG_PAC_RXF_RXF_FIFO_NUM_ENTRIES_COUNT 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfFifoNumEntriesCount:9;	// RXF FIFO number of entries count
		uint32 reserved0:23;
	} bitFields;
} RegPacRxfRxfFifoNumEntriesCount_u;

/*REG_PAC_RXF_RXF_FIFO_DEBUG 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxfFifoWrPtr:8;	// RXF FIFO write pointer index
		uint32 reserved0:8;
		uint32 rxfFifoNotEmpty:1;	// RXF FIFO not empty indication
		uint32 rxfFifoFull:1;	// RXF FIFO full indication
		uint32 rxfFifoDecrementLessThanZero:1;	// RXF setting FIFO amount has been decremented less than zero
		uint32 reserved1:5;
		uint32 rxfFifoFullDropCtr:8;	// RXF setting FIFO full drop counter
	} bitFields;
} RegPacRxfRxfFifoDebug_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP0_CTRL 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp0Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldLongFilterGrp0Equation:2;	// Select the equation: , 00: Equal (==). , 01: Not equal (!=). , 10: Greater than (>). , 11: below than (<).
		uint32 reserved1:2;
		uint32 mpduFieldLongFilterGrp0Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header).
		uint32 reserved2:18;
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp0Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP0_VALUE 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp0Value:32;	// MPDU field filter value to compare
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp0Value_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP0_MASK 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp0Mask:32;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp0Mask_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP1_CTRL 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp1Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldLongFilterGrp1Equation:2;	// Select the equation: , 00: Equal (==). , 01: Not equal (!=). , 10: Greater than (>). , 11: below than (<).
		uint32 reserved1:2;
		uint32 mpduFieldLongFilterGrp1Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header).
		uint32 reserved2:18;
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp1Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP1_VALUE 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp1Value:32;	// MPDU field filter value to compare
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp1Value_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP1_MASK 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp1Mask:32;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp1Mask_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP2_CTRL 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp2Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldLongFilterGrp2Equation:2;	// Select the equation: , 00: Equal (==). , 01: Not equal (!=). , 10: Greater than (>). , 11: below than (<).
		uint32 reserved1:2;
		uint32 mpduFieldLongFilterGrp2Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header).
		uint32 reserved2:18;
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp2Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP2_VALUE 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp2Value:32;	// MPDU field filter value to compare
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp2Value_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP2_MASK 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp2Mask:32;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp2Mask_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP3_CTRL 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp3Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldLongFilterGrp3Equation:2;	// Select the equation: , 00: Equal (==). , 01: Not equal (!=). , 10: Greater than (>). , 11: below than (<).
		uint32 reserved1:2;
		uint32 mpduFieldLongFilterGrp3Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header).
		uint32 reserved2:18;
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp3Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP3_VALUE 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp3Value:32;	// MPDU field filter value to compare
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp3Value_u;

/*REG_PAC_RXF_MPDU_FIELD_LONG_FILTER_GRP3_MASK 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldLongFilterGrp3Mask:32;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldLongFilterGrp3Mask_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP0_CTRL 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter0Grp0Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter0Grp0Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter0Grp0Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter0Grp0Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter0Grp0Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter0Grp0Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP0_CTRL 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter1Grp0Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter1Grp0Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter1Grp0Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter1Grp0Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter1Grp0Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter1Grp0Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP0_CTRL 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter2Grp0Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter2Grp0Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter2Grp0Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter2Grp0Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter2Grp0Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter2Grp0Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP0_CTRL 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter3Grp0Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter3Grp0Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter3Grp0Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter3Grp0Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter3Grp0Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter3Grp0Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP1_CTRL 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter0Grp1Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter0Grp1Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter0Grp1Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter0Grp1Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter0Grp1Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter0Grp1Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP1_CTRL 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter1Grp1Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter1Grp1Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter1Grp1Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter1Grp1Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter1Grp1Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter1Grp1Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP1_CTRL 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter2Grp1Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter2Grp1Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter2Grp1Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter2Grp1Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter2Grp1Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter2Grp1Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP1_CTRL 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter3Grp1Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter3Grp1Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter3Grp1Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter3Grp1Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter3Grp1Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter3Grp1Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP2_CTRL 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter0Grp2Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter0Grp2Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter0Grp2Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter0Grp2Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter0Grp2Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter0Grp2Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP2_CTRL 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter1Grp2Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter1Grp2Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter1Grp2Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter1Grp2Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter1Grp2Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter1Grp2Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP2_CTRL 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter2Grp2Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter2Grp2Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter2Grp2Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter2Grp2Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter2Grp2Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter2Grp2Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP2_CTRL 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter3Grp2Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter3Grp2Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter3Grp2Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter3Grp2Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter3Grp2Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter3Grp2Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER0_GRP3_CTRL 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter0Grp3Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter0Grp3Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter0Grp3Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter0Grp3Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter0Grp3Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter0Grp3Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER1_GRP3_CTRL 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter1Grp3Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter1Grp3Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter1Grp3Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter1Grp3Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter1Grp3Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter1Grp3Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER2_GRP3_CTRL 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter2Grp3Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter2Grp3Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter2Grp3Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter2Grp3Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter2Grp3Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter2Grp3Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_SHORT_FILTER3_GRP3_CTRL 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFilter3Grp3Source:1;	// 0: Filter on the original PHY status. , 1: Filter on the MPDU MAC header.
		uint32 reserved0:3;
		uint32 mpduFieldShortFilter3Grp3Equation:3;	// Select the equation: , 000: Equal (==). , 001: Not equal (!=). , 010: Greater than (>). , 011: below than (<). , 110: Greater than (>) – signed comparison. , 111: below than (<) – signed comparison.
		uint32 reserved1:1;
		uint32 mpduFieldShortFilter3Grp3Offset:6;	// Byte offset of the field (from the start of the PHY Status or MPDU MAC header)
		uint32 reserved2:2;
		uint32 mpduFieldShortFilter3Grp3Value:8;	// MPDU field filter value to compare
		uint32 mpduFieldShortFilter3Grp3Mask:8;	// MPDU field filter mask
	} bitFields;
} RegPacRxfMpduFieldShortFilter3Grp3Ctrl_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP0 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterVapEnGrp0:16;	// Enable group of MPDU field filters, bit per VAP
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfMpduFieldFilterVapEnGrp0_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP1 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterVapEnGrp1:16;	// Enable group of MPDU field filters, bit per VAP
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfMpduFieldFilterVapEnGrp1_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP2 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterVapEnGrp2:16;	// Enable group of MPDU field filters, bit per VAP
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfMpduFieldFilterVapEnGrp2_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_VAP_EN_GRP3 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterVapEnGrp3:16;	// Enable group of MPDU field filters, bit per VAP
		uint32 reserved0:16;
	} bitFields;
} RegPacRxfMpduFieldFilterVapEnGrp3_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP0 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterLogic0Grp0:1;	// The logic between ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic1Grp0:1;	// The logic between ESF1 and the result of ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic2Grp0:1;	// The logic between ESF2 and the result of ELF, ESF0 and ESF1. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic3Grp0:1;	// The logic between ESF3 and the result of ELF, ESF0, ESF1 and ESF2. , 0: AND. , 1: OR.
		uint32 reserved0:4;
		uint32 mpduFieldFilterElfUseGrp0:1;	// 0x0: No use of enhanced long filter result. , 0x1: use of enhanced long filter result.
		uint32 mpduFieldFilterEsfUseGrp0:3;	// 0x0: No use of enhanced short filter results. , 0x1: use only enhanced short filter 0 result. , 0x2: use only enhanced short filter 0-1 results. , 0x3: use only enhanced short filter 0-2 results. , 0x4: use only enhanced short filter 0-3 results.
		uint32 reserved1:20;
	} bitFields;
} RegPacRxfMpduFieldFilterLogicGrp0_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP1 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterLogic0Grp1:1;	// The logic between ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic1Grp1:1;	// The logic between ESF1 and the result of ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic2Grp1:1;	// The logic between ESF2 and the result of ELF, ESF0 and ESF1. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic3Grp1:1;	// The logic between ESF3 and the result of ELF, ESF0, ESF1 and ESF2. , 0: AND. , 1: OR.
		uint32 reserved0:4;
		uint32 mpduFieldFilterElfUseGrp1:1;	// 0x0: No use of enhanced long filter result. , 0x1: use of enhanced long filter result.
		uint32 mpduFieldFilterEsfUseGrp1:3;	// 0x0: No use of enhanced short filter results. , 0x1: use only enhanced short filter 0 result. , 0x2: use only enhanced short filter 0-1 results. , 0x3: use only enhanced short filter 0-2 results. , 0x4: use only enhanced short filter 0-3 results.
		uint32 reserved1:20;
	} bitFields;
} RegPacRxfMpduFieldFilterLogicGrp1_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP2 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterLogic0Grp2:1;	// The logic between ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic1Grp2:1;	// The logic between ESF1 and the result of ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic2Grp2:1;	// The logic between ESF2 and the result of ELF, ESF0 and ESF1. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic3Grp2:1;	// The logic between ESF3 and the result of ELF, ESF0, ESF1 and ESF2. , 0: AND. , 1: OR.
		uint32 reserved0:4;
		uint32 mpduFieldFilterElfUseGrp2:1;	// 0x0: No use of enhanced long filter result. , 0x1: use of enhanced long filter result.
		uint32 mpduFieldFilterEsfUseGrp2:3;	// 0x0: No use of enhanced short filter results. , 0x1: use only enhanced short filter 0 result. , 0x2: use only enhanced short filter 0-1 results. , 0x3: use only enhanced short filter 0-2 results. , 0x4: use only enhanced short filter 0-3 results.
		uint32 reserved1:20;
	} bitFields;
} RegPacRxfMpduFieldFilterLogicGrp2_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTER_LOGIC_GRP3 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFilterLogic0Grp3:1;	// The logic between ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic1Grp3:1;	// The logic between ESF1 and the result of ELF and ESF0. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic2Grp3:1;	// The logic between ESF2 and the result of ELF, ESF0 and ESF1. , 0: AND. , 1: OR.
		uint32 mpduFieldFilterLogic3Grp3:1;	// The logic between ESF3 and the result of ELF, ESF0, ESF1 and ESF2. , 0: AND. , 1: OR.
		uint32 reserved0:4;
		uint32 mpduFieldFilterElfUseGrp3:1;	// 0x0: No use of enhanced long filter result. , 0x1: use of enhanced long filter result.
		uint32 mpduFieldFilterEsfUseGrp3:3;	// 0x0: No use of enhanced short filter results. , 0x1: use only enhanced short filter 0 result. , 0x2: use only enhanced short filter 0-1 results. , 0x3: use only enhanced short filter 0-2 results. , 0x4: use only enhanced short filter 0-3 results.
		uint32 reserved1:20;
	} bitFields;
} RegPacRxfMpduFieldFilterLogicGrp3_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTERS_RESULT 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldShortFiltersResult:16;	// MPDU field short filters result
		uint32 mpduFieldLongFiltersResult:4;	// MPDU field long filters result
		uint32 mpduFieldGrpFiltersResult:4;	// MPDU field group filters result
		uint32 reserved0:8;
	} bitFields;
} RegPacRxfMpduFieldFiltersResult_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTERS_CTR_A 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFiltersCtrGrp0:16;	// Counter on each end of MPDU and the filter group is set
		uint32 mpduFieldFiltersCtrGrp1:16;	// Counter on each end of MPDU and the filter group is set
	} bitFields;
} RegPacRxfMpduFieldFiltersCtrA_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTERS_CTR_B 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFiltersCtrGrp2:16;	// Counter on each end of MPDU and the filter group is set
		uint32 mpduFieldFiltersCtrGrp3:16;	// Counter on each end of MPDU and the filter group is set
	} bitFields;
} RegPacRxfMpduFieldFiltersCtrB_u;

/*REG_PAC_RXF_MPDU_FIELD_FILTERS_CLEAR_CTR 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduFieldFiltersClearCtrGrp0:1;	// Clear MPDU field counter, clear by write '1'
		uint32 mpduFieldFiltersClearCtrGrp1:1;	// Clear MPDU field counter, clear by write '1'
		uint32 mpduFieldFiltersClearCtrGrp2:1;	// Clear MPDU field counter, clear by write '1'
		uint32 mpduFieldFiltersClearCtrGrp3:1;	// Clear MPDU field counter, clear by write '1'
		uint32 reserved0:28;
	} bitFields;
} RegPacRxfMpduFieldFiltersClearCtr_u;

/*REG_PAC_RXF_ADDR1_3_MULTICAST_BROADCAST_IND_EN 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 addr13MulticastBroadcastIndEn:1;	// Enable Multicast and Broadcast indications based addresses1/3 in the Rx Circular buffer
		uint32 reserved0:31;
	} bitFields;
} RegPacRxfAddr13MulticastBroadcastIndEn_u;



#endif // _PAC_RXF_REGS_H_
