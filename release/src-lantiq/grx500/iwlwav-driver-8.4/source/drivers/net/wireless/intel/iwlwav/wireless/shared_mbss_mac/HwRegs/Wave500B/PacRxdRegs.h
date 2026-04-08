
/***********************************************************************************
File:				PacRxdRegs.h
Module:				pacRxd
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_RXD_REGS_H_
#define _PAC_RXD_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PAC_RXD_BASE_ADDRESS                             MEMORY_MAP_UNIT_16_BASE_ADDRESS
#define	REG_PAC_RXD_EVENT_ENABLE_A        (PAC_RXD_BASE_ADDRESS + 0x0)
#define	REG_PAC_RXD_EVENT_ENABLE_B        (PAC_RXD_BASE_ADDRESS + 0x4)
#define	REG_PAC_RXD_RX_ENABLE_A           (PAC_RXD_BASE_ADDRESS + 0x8)
#define	REG_PAC_RXD_RX_ENABLE_B           (PAC_RXD_BASE_ADDRESS + 0xC)
#define	REG_PAC_RXD_CONDITION_0           (PAC_RXD_BASE_ADDRESS + 0x10)
#define	REG_PAC_RXD_CONDITION_1           (PAC_RXD_BASE_ADDRESS + 0x14)
#define	REG_PAC_RXD_CONDITION_2           (PAC_RXD_BASE_ADDRESS + 0x18)
#define	REG_PAC_RXD_CONDITION_3           (PAC_RXD_BASE_ADDRESS + 0x1C)
#define	REG_PAC_RXD_CONDITION_4           (PAC_RXD_BASE_ADDRESS + 0x20)
#define	REG_PAC_RXD_CONDITION_5           (PAC_RXD_BASE_ADDRESS + 0x24)
#define	REG_PAC_RXD_CONDITION_6           (PAC_RXD_BASE_ADDRESS + 0x28)
#define	REG_PAC_RXD_CONDITION_7           (PAC_RXD_BASE_ADDRESS + 0x2C)
#define	REG_PAC_RXD_CONDITION_8           (PAC_RXD_BASE_ADDRESS + 0x30)
#define	REG_PAC_RXD_CONDITION_9           (PAC_RXD_BASE_ADDRESS + 0x34)
#define	REG_PAC_RXD_CONDITION_10          (PAC_RXD_BASE_ADDRESS + 0x38)
#define	REG_PAC_RXD_CONDITION_11          (PAC_RXD_BASE_ADDRESS + 0x3C)
#define	REG_PAC_RXD_CONDITION_12          (PAC_RXD_BASE_ADDRESS + 0x40)
#define	REG_PAC_RXD_CONDITION_13          (PAC_RXD_BASE_ADDRESS + 0x44)
#define	REG_PAC_RXD_CONDITION_14          (PAC_RXD_BASE_ADDRESS + 0x48)
#define	REG_PAC_RXD_CONDITION_15          (PAC_RXD_BASE_ADDRESS + 0x4C)
#define	REG_PAC_RXD_CONDITION_16          (PAC_RXD_BASE_ADDRESS + 0x50)
#define	REG_PAC_RXD_CONDITION_17          (PAC_RXD_BASE_ADDRESS + 0x54)
#define	REG_PAC_RXD_CONDITION_18          (PAC_RXD_BASE_ADDRESS + 0x58)
#define	REG_PAC_RXD_CONDITION_19          (PAC_RXD_BASE_ADDRESS + 0x5C)
#define	REG_PAC_RXD_CONDITION_20          (PAC_RXD_BASE_ADDRESS + 0x60)
#define	REG_PAC_RXD_CONDITION_21          (PAC_RXD_BASE_ADDRESS + 0x64)
#define	REG_PAC_RXD_CONDITION_22          (PAC_RXD_BASE_ADDRESS + 0x68)
#define	REG_PAC_RXD_CONDITION_23          (PAC_RXD_BASE_ADDRESS + 0x6C)
#define	REG_PAC_RXD_CONDITION_24          (PAC_RXD_BASE_ADDRESS + 0x70)
#define	REG_PAC_RXD_CONDITION_25          (PAC_RXD_BASE_ADDRESS + 0x74)
#define	REG_PAC_RXD_CONDITION_26          (PAC_RXD_BASE_ADDRESS + 0x78)
#define	REG_PAC_RXD_CONDITION_27          (PAC_RXD_BASE_ADDRESS + 0x7C)
#define	REG_PAC_RXD_CONDITION_28          (PAC_RXD_BASE_ADDRESS + 0x80)
#define	REG_PAC_RXD_CONDITION_29          (PAC_RXD_BASE_ADDRESS + 0x84)
#define	REG_PAC_RXD_CONDITION_30          (PAC_RXD_BASE_ADDRESS + 0x88)
#define	REG_PAC_RXD_CONDITION_31          (PAC_RXD_BASE_ADDRESS + 0x8C)
#define	REG_PAC_RXD_CONDITION_32          (PAC_RXD_BASE_ADDRESS + 0x90)
#define	REG_PAC_RXD_CONDITION_33          (PAC_RXD_BASE_ADDRESS + 0x94)
#define	REG_PAC_RXD_CONTROL               (PAC_RXD_BASE_ADDRESS + 0xA4)
#define	REG_PAC_RXD_FRAME_ABORT           (PAC_RXD_BASE_ADDRESS + 0xA8)
#define	REG_PAC_RXD_ALL_EVENTS_A          (PAC_RXD_BASE_ADDRESS + 0xAC)
#define	REG_PAC_RXD_ALL_EVENTS_B          (PAC_RXD_BASE_ADDRESS + 0xB0)
#define	REG_PAC_RXD_CONDITION_EXPAND0     (PAC_RXD_BASE_ADDRESS + 0x100)
#define	REG_PAC_RXD_CONDITION_EXPAND1     (PAC_RXD_BASE_ADDRESS + 0x104)
#define	REG_PAC_RXD_CONDITION_EXPAND2     (PAC_RXD_BASE_ADDRESS + 0x108)
#define	REG_PAC_RXD_CONDITION_EXPAND3     (PAC_RXD_BASE_ADDRESS + 0x10C)
#define	REG_PAC_RXD_CONDITION_EXPAND4     (PAC_RXD_BASE_ADDRESS + 0x110)
#define	REG_PAC_RXD_CONDITION_EXPAND5     (PAC_RXD_BASE_ADDRESS + 0x114)
#define	REG_PAC_RXD_CONDITION_EXPAND6     (PAC_RXD_BASE_ADDRESS + 0x118)
#define	REG_PAC_RXD_CONDITION_EXPAND7     (PAC_RXD_BASE_ADDRESS + 0x11C)
#define	REG_PAC_RXD_CONDITION_EXPAND8     (PAC_RXD_BASE_ADDRESS + 0x120)
#define	REG_PAC_RXD_CONDITION_EXPAND9     (PAC_RXD_BASE_ADDRESS + 0x124)
#define	REG_PAC_RXD_CONDITION_EXPAND10    (PAC_RXD_BASE_ADDRESS + 0x128)
#define	REG_PAC_RXD_CONDITION_EXPAND11    (PAC_RXD_BASE_ADDRESS + 0x12C)
#define	REG_PAC_RXD_CONDITION_EXPAND12    (PAC_RXD_BASE_ADDRESS + 0x130)
#define	REG_PAC_RXD_CONDITION_EXPAND13    (PAC_RXD_BASE_ADDRESS + 0x134)
#define	REG_PAC_RXD_CONDITION_EXPAND14    (PAC_RXD_BASE_ADDRESS + 0x138)
#define	REG_PAC_RXD_CONDITION_EXPAND15    (PAC_RXD_BASE_ADDRESS + 0x13C)
#define	REG_PAC_RXD_CONDITION_EXPAND16    (PAC_RXD_BASE_ADDRESS + 0x140)
#define	REG_PAC_RXD_CONDITION_EXPAND17    (PAC_RXD_BASE_ADDRESS + 0x144)
#define	REG_PAC_RXD_CONDITION_EXPAND18    (PAC_RXD_BASE_ADDRESS + 0x148)
#define	REG_PAC_RXD_CONDITION_EXPAND19    (PAC_RXD_BASE_ADDRESS + 0x14C)
#define	REG_PAC_RXD_CONDITION_EXPAND20    (PAC_RXD_BASE_ADDRESS + 0x150)
#define	REG_PAC_RXD_CONDITION_EXPAND21    (PAC_RXD_BASE_ADDRESS + 0x154)
#define	REG_PAC_RXD_CONDITION_EXPAND22    (PAC_RXD_BASE_ADDRESS + 0x158)
#define	REG_PAC_RXD_CONDITION_EXPAND23    (PAC_RXD_BASE_ADDRESS + 0x15C)
#define	REG_PAC_RXD_CONDITION_EXPAND24    (PAC_RXD_BASE_ADDRESS + 0x160)
#define	REG_PAC_RXD_CONDITION_EXPAND25    (PAC_RXD_BASE_ADDRESS + 0x164)
#define	REG_PAC_RXD_CONDITION_EXPAND26    (PAC_RXD_BASE_ADDRESS + 0x168)
#define	REG_PAC_RXD_CONDITION_EXPAND27    (PAC_RXD_BASE_ADDRESS + 0x16C)
#define	REG_PAC_RXD_CONDITION_EXPAND28    (PAC_RXD_BASE_ADDRESS + 0x170)
#define	REG_PAC_RXD_CONDITION_EXPAND29    (PAC_RXD_BASE_ADDRESS + 0x174)
#define	REG_PAC_RXD_CONDITION_EXPAND30    (PAC_RXD_BASE_ADDRESS + 0x178)
#define	REG_PAC_RXD_CONDITION_EXPAND31    (PAC_RXD_BASE_ADDRESS + 0x17C)
#define	REG_PAC_RXD_CONDITION_EXPAND32    (PAC_RXD_BASE_ADDRESS + 0x180)
#define	REG_PAC_RXD_CONDITION_EXPAND33    (PAC_RXD_BASE_ADDRESS + 0x184)
#define	REG_PAC_RXD_RXD_LOGGER_CONTROL    (PAC_RXD_BASE_ADDRESS + 0x194)
#define	REG_PAC_RXD_RXD_LOGGER_ACTIVE     (PAC_RXD_BASE_ADDRESS + 0x198)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_RXD_EVENT_ENABLE_A 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eventEnableA:32;	// RXD  event enable lines 0to31
	} bitFields;
} RegPacRxdEventEnableA_u;

/*REG_PAC_RXD_EVENT_ENABLE_B 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eventEnableB:2;	// RXD  event enable lines 32to33
		uint32 reserved0:30;
	} bitFields;
} RegPacRxdEventEnableB_u;

/*REG_PAC_RXD_RX_ENABLE_A 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEnableA:32;	// RXD  rx enable lines 0to31
	} bitFields;
} RegPacRxdRxEnableA_u;

/*REG_PAC_RXD_RX_ENABLE_B 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEnableB:2;	// RXD  rx enable lines 32to33
		uint32 reserved0:29;
		uint32 rxEnableNoMatch:1;	// RXD rx enable when no match of any RXD line
	} bitFields;
} RegPacRxdRxEnableB_u;

/*REG_PAC_RXD_CONDITION_0 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition0:32;	// RXD  condition 0
	} bitFields;
} RegPacRxdCondition0_u;

/*REG_PAC_RXD_CONDITION_1 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition1:32;	// RXD  condition 1
	} bitFields;
} RegPacRxdCondition1_u;

/*REG_PAC_RXD_CONDITION_2 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition2:32;	// RXD  condition 2
	} bitFields;
} RegPacRxdCondition2_u;

/*REG_PAC_RXD_CONDITION_3 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition3:32;	// RXD  condition 3
	} bitFields;
} RegPacRxdCondition3_u;

/*REG_PAC_RXD_CONDITION_4 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition4:32;	// RXD  condition 4
	} bitFields;
} RegPacRxdCondition4_u;

/*REG_PAC_RXD_CONDITION_5 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition5:32;	// RXD  condition 5
	} bitFields;
} RegPacRxdCondition5_u;

/*REG_PAC_RXD_CONDITION_6 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition6:32;	// RXD  condition 6
	} bitFields;
} RegPacRxdCondition6_u;

/*REG_PAC_RXD_CONDITION_7 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition7:32;	// RXD  condition 7
	} bitFields;
} RegPacRxdCondition7_u;

/*REG_PAC_RXD_CONDITION_8 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition8:32;	// RXD  condition 8
	} bitFields;
} RegPacRxdCondition8_u;

/*REG_PAC_RXD_CONDITION_9 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition9:32;	// RXD  condition 9
	} bitFields;
} RegPacRxdCondition9_u;

/*REG_PAC_RXD_CONDITION_10 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition10:32;	// RXD  condition 10
	} bitFields;
} RegPacRxdCondition10_u;

/*REG_PAC_RXD_CONDITION_11 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition11:32;	// RXD  condition 11
	} bitFields;
} RegPacRxdCondition11_u;

/*REG_PAC_RXD_CONDITION_12 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition12:32;	// RXD  condition 12
	} bitFields;
} RegPacRxdCondition12_u;

/*REG_PAC_RXD_CONDITION_13 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition13:32;	// RXD  condition 13
	} bitFields;
} RegPacRxdCondition13_u;

/*REG_PAC_RXD_CONDITION_14 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition14:32;	// RXD  condition 14
	} bitFields;
} RegPacRxdCondition14_u;

/*REG_PAC_RXD_CONDITION_15 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition15:32;	// RXD  condition 15
	} bitFields;
} RegPacRxdCondition15_u;

/*REG_PAC_RXD_CONDITION_16 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition16:32;	// RXD  condition 16
	} bitFields;
} RegPacRxdCondition16_u;

/*REG_PAC_RXD_CONDITION_17 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition17:32;	// RXD  condition 17
	} bitFields;
} RegPacRxdCondition17_u;

/*REG_PAC_RXD_CONDITION_18 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition18:32;	// RXD  condition 18
	} bitFields;
} RegPacRxdCondition18_u;

/*REG_PAC_RXD_CONDITION_19 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition19:32;	// RXD  condition 19
	} bitFields;
} RegPacRxdCondition19_u;

/*REG_PAC_RXD_CONDITION_20 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition20:32;	// RXD  condition 20
	} bitFields;
} RegPacRxdCondition20_u;

/*REG_PAC_RXD_CONDITION_21 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition21:32;	// RXD  condition 21
	} bitFields;
} RegPacRxdCondition21_u;

/*REG_PAC_RXD_CONDITION_22 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition22:32;	// RXD  condition 22
	} bitFields;
} RegPacRxdCondition22_u;

/*REG_PAC_RXD_CONDITION_23 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition23:32;	// RXD  condition 23
	} bitFields;
} RegPacRxdCondition23_u;

/*REG_PAC_RXD_CONDITION_24 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition24:32;	// RXD  condition 24
	} bitFields;
} RegPacRxdCondition24_u;

/*REG_PAC_RXD_CONDITION_25 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition25:32;	// RXD  condition 25
	} bitFields;
} RegPacRxdCondition25_u;

/*REG_PAC_RXD_CONDITION_26 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition26:32;	// RXD  condition 26
	} bitFields;
} RegPacRxdCondition26_u;

/*REG_PAC_RXD_CONDITION_27 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition27:32;	// RXD  condition 27
	} bitFields;
} RegPacRxdCondition27_u;

/*REG_PAC_RXD_CONDITION_28 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition28:32;	// RXD  condition 28
	} bitFields;
} RegPacRxdCondition28_u;

/*REG_PAC_RXD_CONDITION_29 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition29:32;	// RXD  condition 29
	} bitFields;
} RegPacRxdCondition29_u;

/*REG_PAC_RXD_CONDITION_30 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition30:32;	// RXD  condition 30
	} bitFields;
} RegPacRxdCondition30_u;

/*REG_PAC_RXD_CONDITION_31 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition31:32;	// RXD  condition 31
	} bitFields;
} RegPacRxdCondition31_u;

/*REG_PAC_RXD_CONDITION_32 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition32:32;	// RXD  condition 32
	} bitFields;
} RegPacRxdCondition32_u;

/*REG_PAC_RXD_CONDITION_33 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 condition33:32;	// RXD  condition 33
	} bitFields;
} RegPacRxdCondition33_u;

/*REG_PAC_RXD_CONTROL 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 controlNdpaNdpTimer:1;	// RXD control ndpa ndp timer
		uint32 reserved1:29;
	} bitFields;
} RegPacRxdControl_u;

/*REG_PAC_RXD_FRAME_ABORT 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 overflowAbort:1;	// RXD  overflow
		uint32 phyError:1;	// RXD  phy error
		uint32 crcError:1;	// RXD  crc error
		uint32 reserved0:29;
	} bitFields;
} RegPacRxdFrameAbort_u;

/*REG_PAC_RXD_ALL_EVENTS_A 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allEventsA:32;	// RXD  all events lines 0to31
	} bitFields;
} RegPacRxdAllEventsA_u;

/*REG_PAC_RXD_ALL_EVENTS_B 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allEventsB:2;	// RXD  all events expand lines 32to33
		uint32 reserved0:30;
	} bitFields;
} RegPacRxdAllEventsB_u;

/*REG_PAC_RXD_CONDITION_EXPAND0 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand0:11;	// Condition Expand 0
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand0_u;

/*REG_PAC_RXD_CONDITION_EXPAND1 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand1:11;	// Condition Expand 1
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand1_u;

/*REG_PAC_RXD_CONDITION_EXPAND2 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand2:11;	// Condition Expand 2
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand2_u;

/*REG_PAC_RXD_CONDITION_EXPAND3 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand3:11;	// Condition Expand 3
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand3_u;

/*REG_PAC_RXD_CONDITION_EXPAND4 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand4:11;	// Condition Expand 4
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand4_u;

/*REG_PAC_RXD_CONDITION_EXPAND5 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand5:11;	// Condition Expand 5
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand5_u;

/*REG_PAC_RXD_CONDITION_EXPAND6 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand6:11;	// Condition Expand 6
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand6_u;

/*REG_PAC_RXD_CONDITION_EXPAND7 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand7:11;	// Condition Expand 7
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand7_u;

/*REG_PAC_RXD_CONDITION_EXPAND8 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand8:11;	// Condition Expand 8
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand8_u;

/*REG_PAC_RXD_CONDITION_EXPAND9 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand9:11;	// Condition Expand 9
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand9_u;

/*REG_PAC_RXD_CONDITION_EXPAND10 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand10:11;	// Condition Expand 10
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand10_u;

/*REG_PAC_RXD_CONDITION_EXPAND11 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand11:11;	// Condition Expand 11
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand11_u;

/*REG_PAC_RXD_CONDITION_EXPAND12 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand12:11;	// Condition Expand 12
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand12_u;

/*REG_PAC_RXD_CONDITION_EXPAND13 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand13:11;	// Condition Expand 13
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand13_u;

/*REG_PAC_RXD_CONDITION_EXPAND14 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand14:11;	// Condition Expand 14
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand14_u;

/*REG_PAC_RXD_CONDITION_EXPAND15 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand15:11;	// Condition Expand 15
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand15_u;

/*REG_PAC_RXD_CONDITION_EXPAND16 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand16:11;	// Condition Expand 16
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand16_u;

/*REG_PAC_RXD_CONDITION_EXPAND17 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand17:11;	// Condition Expand 17
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand17_u;

/*REG_PAC_RXD_CONDITION_EXPAND18 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand18:11;	// Condition Expand 18
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand18_u;

/*REG_PAC_RXD_CONDITION_EXPAND19 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand19:11;	// Condition Expand 19
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand19_u;

/*REG_PAC_RXD_CONDITION_EXPAND20 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand20:11;	// Condition Expand 20
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand20_u;

/*REG_PAC_RXD_CONDITION_EXPAND21 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand21:11;	// Condition Expand 21
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand21_u;

/*REG_PAC_RXD_CONDITION_EXPAND22 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand22:11;	// Condition Expand 22
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand22_u;

/*REG_PAC_RXD_CONDITION_EXPAND23 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand23:11;	// Condition Expand 23
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand23_u;

/*REG_PAC_RXD_CONDITION_EXPAND24 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand24:11;	// Condition Expand 24
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand24_u;

/*REG_PAC_RXD_CONDITION_EXPAND25 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand25:11;	// Condition Expand 25
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand25_u;

/*REG_PAC_RXD_CONDITION_EXPAND26 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand26:11;	// Condition Expand 26
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand26_u;

/*REG_PAC_RXD_CONDITION_EXPAND27 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand27:11;	// Condition Expand 27
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand27_u;

/*REG_PAC_RXD_CONDITION_EXPAND28 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand28:11;	// Condition Expand 28
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand28_u;

/*REG_PAC_RXD_CONDITION_EXPAND29 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand29:11;	// Condition Expand 29
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand29_u;

/*REG_PAC_RXD_CONDITION_EXPAND30 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand30:11;	// Condition Expand 30
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand30_u;

/*REG_PAC_RXD_CONDITION_EXPAND31 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand31:11;	// Condition Expand 31
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand31_u;

/*REG_PAC_RXD_CONDITION_EXPAND32 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand32:11;	// Condition Expand 32
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand32_u;

/*REG_PAC_RXD_CONDITION_EXPAND33 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 conditionExpand33:11;	// Condition Expand 33
		uint32 reserved0:21;
	} bitFields;
} RegPacRxdConditionExpand33_u;

/*REG_PAC_RXD_RXD_LOGGER_CONTROL 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxdLoggerEn:1;	// RXD logger enable
		uint32 reserved0:7;
		uint32 rxdLoggerPriority:2;	// RXD logger priority
		uint32 reserved1:22;
	} bitFields;
} RegPacRxdRxdLoggerControl_u;

/*REG_PAC_RXD_RXD_LOGGER_ACTIVE 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxdLoggerActive:1;	// RXD logger active
		uint32 reserved0:31;
	} bitFields;
} RegPacRxdRxdLoggerActive_u;



#endif // _PAC_RXD_REGS_H_
