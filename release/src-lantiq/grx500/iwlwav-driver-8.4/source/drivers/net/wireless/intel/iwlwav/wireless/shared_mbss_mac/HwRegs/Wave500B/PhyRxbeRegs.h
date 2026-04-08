
/***********************************************************************************
File:				PhyRxbeRegs.h
Module:				phyRxbe
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RXBE_REGS_H_
#define _PHY_RXBE_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RXBE_BASE_ADDRESS                             MEMORY_MAP_UNIT_49_BASE_ADDRESS
#define	REG_PHY_RXBE_PHY_RXBE_REG04    (PHY_RXBE_BASE_ADDRESS + 0x10)
#define	REG_PHY_RXBE_PHY_RXBE_REG05    (PHY_RXBE_BASE_ADDRESS + 0x14)
#define	REG_PHY_RXBE_PHY_RXBE_REG07    (PHY_RXBE_BASE_ADDRESS + 0x1C)
#define	REG_PHY_RXBE_PHY_RXBE_REG08    (PHY_RXBE_BASE_ADDRESS + 0x20)
#define	REG_PHY_RXBE_PHY_RXBE_REG18    (PHY_RXBE_BASE_ADDRESS + 0x60)
#define	REG_PHY_RXBE_PHY_RXBE_REG19    (PHY_RXBE_BASE_ADDRESS + 0x64)
#define	REG_PHY_RXBE_PHY_RXBE_REG1E    (PHY_RXBE_BASE_ADDRESS + 0x78)
#define	REG_PHY_RXBE_PHY_RXBE_REG20    (PHY_RXBE_BASE_ADDRESS + 0x80)
#define	REG_PHY_RXBE_PHY_RXBE_REG21    (PHY_RXBE_BASE_ADDRESS + 0x84)
#define	REG_PHY_RXBE_PHY_RXBE_REG22    (PHY_RXBE_BASE_ADDRESS + 0x88)
#define	REG_PHY_RXBE_PHY_RXBE_REG23    (PHY_RXBE_BASE_ADDRESS + 0x8C)
#define	REG_PHY_RXBE_PHY_RXBE_REG24    (PHY_RXBE_BASE_ADDRESS + 0x90)
#define	REG_PHY_RXBE_PHY_RXBE_REG25    (PHY_RXBE_BASE_ADDRESS + 0x94)
#define	REG_PHY_RXBE_PHY_RXBE_REG26    (PHY_RXBE_BASE_ADDRESS + 0x98)
#define	REG_PHY_RXBE_PHY_RXBE_REG28    (PHY_RXBE_BASE_ADDRESS + 0xA0)
#define	REG_PHY_RXBE_PHY_RXBE_REG34    (PHY_RXBE_BASE_ADDRESS + 0xD0)
#define	REG_PHY_RXBE_PHY_RXBE_REG35    (PHY_RXBE_BASE_ADDRESS + 0xD4)
#define	REG_PHY_RXBE_PHY_RXBE_REG36    (PHY_RXBE_BASE_ADDRESS + 0xD8)
#define	REG_PHY_RXBE_PHY_RXBE_REG37    (PHY_RXBE_BASE_ADDRESS + 0xDC)
#define	REG_PHY_RXBE_PHY_RXBE_REG38    (PHY_RXBE_BASE_ADDRESS + 0xE0)
#define	REG_PHY_RXBE_PHY_RXBE_REG39    (PHY_RXBE_BASE_ADDRESS + 0xE4)
#define	REG_PHY_RXBE_PHY_RXBE_REG3F    (PHY_RXBE_BASE_ADDRESS + 0xFC)
#define	REG_PHY_RXBE_PHY_RXBE_REG41    (PHY_RXBE_BASE_ADDRESS + 0x104)
#define	REG_PHY_RXBE_PHY_RXBE_REG42    (PHY_RXBE_BASE_ADDRESS + 0x108)
#define	REG_PHY_RXBE_PHY_RXBE_REG4A    (PHY_RXBE_BASE_ADDRESS + 0x128)
#define	REG_PHY_RXBE_PHY_RXBE_REG4B    (PHY_RXBE_BASE_ADDRESS + 0x12C)
#define	REG_PHY_RXBE_PHY_RXBE_REG4C    (PHY_RXBE_BASE_ADDRESS + 0x130)
#define	REG_PHY_RXBE_PHY_RXBE_REG4D    (PHY_RXBE_BASE_ADDRESS + 0x134)
#define	REG_PHY_RXBE_PHY_RXBE_REG4E    (PHY_RXBE_BASE_ADDRESS + 0x138)
#define	REG_PHY_RXBE_PHY_RXBE_REG4F    (PHY_RXBE_BASE_ADDRESS + 0x13C)
#define	REG_PHY_RXBE_PHY_RXBE_REG50    (PHY_RXBE_BASE_ADDRESS + 0x140)
#define	REG_PHY_RXBE_PHY_RXBE_REG51    (PHY_RXBE_BASE_ADDRESS + 0x144)
#define	REG_PHY_RXBE_PHY_RXBE_REG52    (PHY_RXBE_BASE_ADDRESS + 0x148)
#define	REG_PHY_RXBE_PHY_RXBE_REG53    (PHY_RXBE_BASE_ADDRESS + 0x14C)
#define	REG_PHY_RXBE_PHY_RXBE_REG54    (PHY_RXBE_BASE_ADDRESS + 0x150)
#define	REG_PHY_RXBE_PHY_RXBE_REG55    (PHY_RXBE_BASE_ADDRESS + 0x154)
#define	REG_PHY_RXBE_PHY_RXBE_REG56    (PHY_RXBE_BASE_ADDRESS + 0x158)
#define	REG_PHY_RXBE_PHY_RXBE_REG57    (PHY_RXBE_BASE_ADDRESS + 0x15C)
#define	REG_PHY_RXBE_PHY_RXBE_REG58    (PHY_RXBE_BASE_ADDRESS + 0x160)
#define	REG_PHY_RXBE_PHY_RXBE_REG59    (PHY_RXBE_BASE_ADDRESS + 0x164)
#define	REG_PHY_RXBE_PHY_RXBE_REG5D    (PHY_RXBE_BASE_ADDRESS + 0x174)
#define	REG_PHY_RXBE_PHY_RXBE_REG5E    (PHY_RXBE_BASE_ADDRESS + 0x178)
#define	REG_PHY_RXBE_PHY_RXBE_REG63    (PHY_RXBE_BASE_ADDRESS + 0x18C)
#define	REG_PHY_RXBE_PHY_RXBE_REG64    (PHY_RXBE_BASE_ADDRESS + 0x190)
#define	REG_PHY_RXBE_PHY_RXBE_REG65    (PHY_RXBE_BASE_ADDRESS + 0x194)
#define	REG_PHY_RXBE_PHY_RXBE_REG66    (PHY_RXBE_BASE_ADDRESS + 0x198)
#define	REG_PHY_RXBE_PHY_RXBE_REG67    (PHY_RXBE_BASE_ADDRESS + 0x19C)
#define	REG_PHY_RXBE_PHY_RXBE_REG68    (PHY_RXBE_BASE_ADDRESS + 0x1A0)
#define	REG_PHY_RXBE_PHY_RXBE_REG69    (PHY_RXBE_BASE_ADDRESS + 0x1A4)
#define	REG_PHY_RXBE_PHY_RXBE_REG6A    (PHY_RXBE_BASE_ADDRESS + 0x1A8)
#define	REG_PHY_RXBE_PHY_RXBE_REG6B    (PHY_RXBE_BASE_ADDRESS + 0x1AC)
#define	REG_PHY_RXBE_PHY_RXBE_REG6C    (PHY_RXBE_BASE_ADDRESS + 0x1B0)
#define	REG_PHY_RXBE_PHY_RXBE_REG6D    (PHY_RXBE_BASE_ADDRESS + 0x1B4)
#define	REG_PHY_RXBE_PHY_RXBE_REG6E    (PHY_RXBE_BASE_ADDRESS + 0x1B8)
#define	REG_PHY_RXBE_PHY_RXBE_REG6F    (PHY_RXBE_BASE_ADDRESS + 0x1BC)
#define	REG_PHY_RXBE_PHY_RXBE_REG70    (PHY_RXBE_BASE_ADDRESS + 0x1C0)
#define	REG_PHY_RXBE_PHY_RXBE_REG71    (PHY_RXBE_BASE_ADDRESS + 0x1C4)
#define	REG_PHY_RXBE_PHY_RXBE_REG72    (PHY_RXBE_BASE_ADDRESS + 0x1C8)
#define	REG_PHY_RXBE_PHY_RXBE_REG73    (PHY_RXBE_BASE_ADDRESS + 0x1CC)
#define	REG_PHY_RXBE_PHY_RXBE_REG74    (PHY_RXBE_BASE_ADDRESS + 0x1D0)
#define	REG_PHY_RXBE_PHY_RXBE_REG75    (PHY_RXBE_BASE_ADDRESS + 0x1D4)
#define	REG_PHY_RXBE_PHY_RXBE_REG76    (PHY_RXBE_BASE_ADDRESS + 0x1D8)
#define	REG_PHY_RXBE_PHY_RXBE_REG77    (PHY_RXBE_BASE_ADDRESS + 0x1DC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RXBE_PHY_RXBE_REG04 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpPhyMacEventLen:1;	// RxBE Test bus control
		uint32 tpPhyMacDataLen:1;	// RxBE Test bus control
		uint32 tpPhyMacStationIdLen:1;	// RxBE Test bus control
		uint32 tpLdpcCwStatLen:1;	// RxBE Test bus control
		uint32 tpLlrStreamLen:1;	// RxBE Test bus control
		uint32 reserved0:26;
		uint32 tpDebugCntEn:1;	// Test Plug counter enable
	} bitFields;
} RegPhyRxbePhyRxbeReg04_u;

/*REG_PHY_RXBE_PHY_RXBE_REG05 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpLlrStreamSel:2;	// RxBE Test bus control
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxbePhyRxbeReg05_u;

/*REG_PHY_RXBE_PHY_RXBE_REG07 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpDebugClk2Strb:4;	// Test Bus debug clock per strobe
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxbePhyRxbeReg07_u;

/*REG_PHY_RXBE_PHY_RXBE_REG08 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpLowMuxCtrl:2;	// RxBE low test bus control
		uint32 tpMidMuxCtrl:2;	// RxBE mid test bus control
		uint32 tpHighMuxCtrl:2;	// RxBE high test bus control
		uint32 tpFourMuxCtrl:2;	// RxBE high test bus control
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxbePhyRxbeReg08_u;

/*REG_PHY_RXBE_PHY_RXBE_REG18 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 viterbiTbDepth:6;	// Viterbi length
		uint32 reserved0:26;
	} bitFields;
} RegPhyRxbePhyRxbeReg18_u;

/*REG_PHY_RXBE_PHY_RXBE_REG19 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reportFullMetrics:1;	// Report full metrics
		uint32 waitForGriscMetrics:1;	// Wait for gen risc metrics
		uint32 reserved0:2;
		uint32 waitForSnrMetrics:1;	// Wait for snr metrics
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxbePhyRxbeReg19_u;

/*REG_PHY_RXBE_PHY_RXBE_REG1E 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numBitsInPacket:25;	// Number of bits in the packet
		uint32 reserved0:7;
	} bitFields;
} RegPhyRxbePhyRxbeReg1E_u;

/*REG_PHY_RXBE_PHY_RXBE_REG20 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 macIfMinCycle:8;	// Cycle length
		uint32 reserved1:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg20_u;

/*REG_PHY_RXBE_PHY_RXBE_REG21 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetLimit:19;	// Errors per Packet limit
		uint32 reserved0:13;
	} bitFields;
} RegPhyRxbePhyRxbeReg21_u;

/*REG_PHY_RXBE_PHY_RXBE_REG22 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsLoadVal:32;	// PRBS LSFR init value
	} bitFields;
} RegPhyRxbePhyRxbeReg22_u;

/*REG_PHY_RXBE_PHY_RXBE_REG23 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsCrcEnable:1;	// Enable PRBS Comparison
		uint32 packetWithCrc:1;	// Enable CRC in packet
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxbePhyRxbeReg23_u;

/*REG_PHY_RXBE_PHY_RXBE_REG24 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearCounters:1;	// Clear PRBS Counters
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxbePhyRxbeReg24_u;

/*REG_PHY_RXBE_PHY_RXBE_REG25 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latchCounters:1;	// Latch PRBS Counters
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxbePhyRxbeReg25_u;

/*REG_PHY_RXBE_PHY_RXBE_REG26 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsType:32;	// PRBS Polynom type
	} bitFields;
} RegPhyRxbePhyRxbeReg26_u;

/*REG_PHY_RXBE_PHY_RXBE_REG28 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sigDecodeCrcLen:3;	// Signal CRC length
		uint32 reserved0:1;
		uint32 sigDecodeCrcType:8;	// Signal CRC type
		uint32 sigDecodeCrcCorrupt:1;	// Signal CRC corrupt
		uint32 reserved1:3;
		uint32 sigDecodeCrcInit:8;	// Signal CRC Init value
		uint32 reserved2:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg28_u;

/*REG_PHY_RXBE_PHY_RXBE_REG34 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrCntLatched:32;	// PRBS Error count - latched
	} bitFields;
} RegPhyRxbePhyRxbeReg34_u;

/*REG_PHY_RXBE_PHY_RXBE_REG35 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCntLatched310:32;	// PRBS Bit count [31:0] - latched
	} bitFields;
} RegPhyRxbePhyRxbeReg35_u;

/*REG_PHY_RXBE_PHY_RXBE_REG36 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCntLatched4732:16;	// PRBS Bit count [47:32] - latched
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxbePhyRxbeReg36_u;

/*REG_PHY_RXBE_PHY_RXBE_REG37 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcPacketCntLatched:32;	// Number of compared packets - latched
	} bitFields;
} RegPhyRxbePhyRxbeReg37_u;

/*REG_PHY_RXBE_PHY_RXBE_REG38 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcErrPacketCntLatched:32;	// Number of compared errored packets with CRC error - latched
	} bitFields;
} RegPhyRxbePhyRxbeReg38_u;

/*REG_PHY_RXBE_PHY_RXBE_REG39 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrPacketCntLatched:32;	// Number of compared errored packets with PRBS error - latched
	} bitFields;
} RegPhyRxbePhyRxbeReg39_u;

/*REG_PHY_RXBE_PHY_RXBE_REG3F 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBistStart:1;	// BIST start
		uint32 clearRamMode:1;	// Clear RAM mode in BIST
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxbePhyRxbeReg3F_u;

/*REG_PHY_RXBE_PHY_RXBE_REG41 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxbeMaskParityErr:1;	// Mask parity error in signal 1
		uint32 rxbeMaskRateErr:1;	// Mask invalid rate error in signal 1
		uint32 rxbeMaskPSizeErr:1;	// Mask packet size error in signal 1
		uint32 rxbeMaskReservedErr:1;	// Mask reserved bit error in signal 1
		uint32 rxbeMaskCrcErr:1;	// Mask bad CRC error in signal 3
		uint32 rxbeMaskSigbCrcErr:1;	// Mask bad CRC error in vht sigb
		uint32 rxbeMaskSig3ReservedErr:1;	// Mask reserved bit error in signal 2/3
		uint32 rxbeMaskSig3LengthErr:1;	// Mask min length check based on sig3 and sig1
		uint32 rxbeMaskUnsupportedMcs:9;	// Mask unsupported mcs
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxbePhyRxbeReg41_u;

/*REG_PHY_RXBE_PHY_RXBE_REG42 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimumPacketSize:12;	// Minimum packet size in bytes
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxbePhyRxbeReg42_u;

/*REG_PHY_RXBE_PHY_RXBE_REG4A 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrCnt:32;	// PRBS Error count - real time
	} bitFields;
} RegPhyRxbePhyRxbeReg4A_u;

/*REG_PHY_RXBE_PHY_RXBE_REG4B 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCnt310:32;	// PRBS Bit count [31:0] - real time
	} bitFields;
} RegPhyRxbePhyRxbeReg4B_u;

/*REG_PHY_RXBE_PHY_RXBE_REG4C 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCnt4732:16;	// PRBS Bit count [47:32] - real time
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxbePhyRxbeReg4C_u;

/*REG_PHY_RXBE_PHY_RXBE_REG4D 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcPacketCnt:32;	// Number of compared packets - real time
	} bitFields;
} RegPhyRxbePhyRxbeReg4D_u;

/*REG_PHY_RXBE_PHY_RXBE_REG4E 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcErrPacketCnt:32;	// Number of compared errored packets with CRC error  - real time
	} bitFields;
} RegPhyRxbePhyRxbeReg4E_u;

/*REG_PHY_RXBE_PHY_RXBE_REG4F 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrPacketCnt:32;	// Number of compared errored packets with PRBS error - real time
	} bitFields;
} RegPhyRxbePhyRxbeReg4F_u;

/*REG_PHY_RXBE_PHY_RXBE_REG50 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIfBitCounter:23;	// Phy MAC IF bit counter
		uint32 reserved0:9;
	} bitFields;
} RegPhyRxbePhyRxbeReg50_u;

/*REG_PHY_RXBE_PHY_RXBE_REG51 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpo:16;	// RxBE general purpose output
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxbePhyRxbeReg51_u;

/*REG_PHY_RXBE_PHY_RXBE_REG52 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpr:32;	// RxBE general purpose register
	} bitFields;
} RegPhyRxbePhyRxbeReg52_u;

/*REG_PHY_RXBE_PHY_RXBE_REG53 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpi:16;	// RxBE general purpose input
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxbePhyRxbeReg53_u;

/*REG_PHY_RXBE_PHY_RXBE_REG54 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpi2:16;	// RxBE general purpose input 2
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxbePhyRxbeReg54_u;

/*REG_PHY_RXBE_PHY_RXBE_REG55 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deintA0Rm:3;	// deint_a0_rm
		uint32 reserved0:1;
		uint32 deintA1Rm:3;	// deint_a1_rm
		uint32 reserved1:1;
		uint32 deintA2Rm:3;	// deint_a2_rm
		uint32 reserved2:1;
		uint32 deintA3Rm:3;	// deint_a3_rm
		uint32 reserved3:1;
		uint32 deintB0Rm:3;	// deint_b0_rm
		uint32 reserved4:1;
		uint32 deintB1Rm:3;	// deint_b1_rm
		uint32 reserved5:1;
		uint32 deintB2Rm:3;	// deint_b2_rm
		uint32 reserved6:1;
		uint32 deintB3Rm:3;	// deint_b3_rm
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxbePhyRxbeReg55_u;

/*REG_PHY_RXBE_PHY_RXBE_REG56 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mergedU96X128B0Rm:3;	// merged_u96x128b_0_rm
		uint32 reserved0:1;
		uint32 mergedU96X128B1Rm:3;	// merged_u96x128b_1_rm
		uint32 reserved1:1;
		uint32 mergedU96X128B2Rm:3;	// merged_u96x128b_2_rm
		uint32 reserved2:1;
		uint32 mergedU96X128B3Rm:3;	// merged_u96x128b_3_rm
		uint32 reserved3:1;
		uint32 mergedU8X128B0Rm:3;	// merged_u8x128b_0_rm
		uint32 reserved4:1;
		uint32 mergedU8X128B1Rm:3;	// merged_u8x128b_1_rm
		uint32 reserved5:1;
		uint32 mergedU8X128B2Rm:3;	// merged_u8x128b_2_rm
		uint32 reserved6:1;
		uint32 mergedU8X128B3Rm:3;	// merged_u8x128b_3_rm
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxbePhyRxbeReg56_u;

/*REG_PHY_RXBE_PHY_RXBE_REG57 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mergedU32X128B0Rm:3;	// merged_u32x128b_0_rm
		uint32 reserved0:1;
		uint32 mergedU32X128B1Rm:3;	// merged_u32x128b_1_rm
		uint32 reserved1:1;
		uint32 mergedU32X128B2Rm:3;	// merged_u32x128b_2_rm
		uint32 reserved2:1;
		uint32 mergedU32X128B3Rm:3;	// merged_u32x128b_3_rm
		uint32 reserved3:1;
		uint32 mergedU32X128B4Rm:3;	// merged_u32x128b_4_rm
		uint32 reserved4:1;
		uint32 mergedU32X128B5Rm:3;	// merged_u32x128b_5_rm
		uint32 reserved5:1;
		uint32 mergedU32X128B6Rm:3;	// merged_u32x128b_6_rm
		uint32 reserved6:1;
		uint32 mergedU32X128B7Rm:3;	// merged_u32x128b_7_rm
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxbePhyRxbeReg57_u;

/*REG_PHY_RXBE_PHY_RXBE_REG58 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mergedU32X128B8Rm:3;	// merged_u32x128b_8_rm
		uint32 reserved0:1;
		uint32 mergedU32X128B9Rm:3;	// merged_u32x128b_9_rm
		uint32 reserved1:1;
		uint32 mergedU32X128BARm:3;	// merged_u32x128b_a_rm
		uint32 reserved2:1;
		uint32 mergedU32X128BBRm:3;	// merged_u32x128b_b_rm
		uint32 reserved3:1;
		uint32 mergedU32X128BCRm:3;	// merged_u32x128b_c_rm
		uint32 reserved4:1;
		uint32 mergedU32X128BDRm:3;	// merged_u32x128b_d_rm
		uint32 reserved5:1;
		uint32 mergedU32X128BERm:3;	// merged_u32x128b_e_rm
		uint32 reserved6:1;
		uint32 mergedU32X128BFRm:3;	// merged_u32x128b_f_rm
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxbePhyRxbeReg58_u;

/*REG_PHY_RXBE_PHY_RXBE_REG59 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mergedU32X128B10Rm:3;	// merged_u32x128b_10_rm
		uint32 reserved0:1;
		uint32 mergedU32X128B11Rm:3;	// merged_u32x128b_11_rm
		uint32 reserved1:1;
		uint32 mergedU32X128B12Rm:3;	// merged_u32x128b_12_rm
		uint32 reserved2:1;
		uint32 mergedU32X128B13Rm:3;	// merged_u32x128b_13_rm
		uint32 reserved3:1;
		uint32 mergedU24X64B0Rm:3;	// merged_u24x64b_0_rm
		uint32 reserved4:1;
		uint32 mergedU24X64B1Rm:3;	// merged_u24x64b_1_rm
		uint32 reserved5:1;
		uint32 mergedU24X64B2Rm:3;	// merged_u24x64b_2_rm
		uint32 reserved6:1;
		uint32 muRamRm:3;	// mu_ram_rm
		uint32 reserved7:1;
	} bitFields;
} RegPhyRxbePhyRxbeReg59_u;

/*REG_PHY_RXBE_PHY_RXBE_REG5D 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkShutdownEn:14;	// gclk shutdown - when '1' clocks will be always deactive
		uint32 reserved0:18;
	} bitFields;
} RegPhyRxbePhyRxbeReg5D_u;

/*REG_PHY_RXBE_PHY_RXBE_REG5E 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bistScrBypass:1;	// bist_scr_bypass
		uint32 ramTestMode:1;	// ram_test_mode
		uint32 memGlobalRm:2;	// mem_global_rm
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxbePhyRxbeReg5E_u;

/*REG_PHY_RXBE_PHY_RXBE_REG63 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcOutWidth:4;	// ldpc_out_width
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxbePhyRxbeReg63_u;

/*REG_PHY_RXBE_PHY_RXBE_REG64 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 globalRdyLimit:16;	// global_rdy_limit
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxbePhyRxbeReg64_u;

/*REG_PHY_RXBE_PHY_RXBE_REG65 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableAirTimeBlock:1;	// enable_air_time_block
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxbePhyRxbeReg65_u;

/*REG_PHY_RXBE_PHY_RXBE_REG66 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deintC0Rm:3;	// deint_c0_rm
		uint32 reserved0:1;
		uint32 deintC1Rm:3;	// deint_c1_rm
		uint32 reserved1:1;
		uint32 deintD0Rm:3;	// deint_d0_rm
		uint32 reserved2:1;
		uint32 deintD1Rm:3;	// deint_d1_rm
		uint32 reserved3:17;
	} bitFields;
} RegPhyRxbePhyRxbeReg66_u;

/*REG_PHY_RXBE_PHY_RXBE_REG67 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtNonAggregate:1;	// vht_non_aggregate
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxbePhyRxbeReg67_u;

/*REG_PHY_RXBE_PHY_RXBE_REG68 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr0:32;	// spare_gpr_0
	} bitFields;
} RegPhyRxbePhyRxbeReg68_u;

/*REG_PHY_RXBE_PHY_RXBE_REG69 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr1:32;	// spare_gpr_1
	} bitFields;
} RegPhyRxbePhyRxbeReg69_u;

/*REG_PHY_RXBE_PHY_RXBE_REG6A 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr2:32;	// spare_gpr_2
	} bitFields;
} RegPhyRxbePhyRxbeReg6A_u;

/*REG_PHY_RXBE_PHY_RXBE_REG6B 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr3:32;	// spare_gpr_3
	} bitFields;
} RegPhyRxbePhyRxbeReg6B_u;

/*REG_PHY_RXBE_PHY_RXBE_REG6C 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txGlobalResetMask:12;	// reset rxbe units for tx
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxbePhyRxbeReg6C_u;

/*REG_PHY_RXBE_PHY_RXBE_REG6D 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscAirTimeResult:17;	// risc_air_time_result
		uint32 reserved0:15;
	} bitFields;
} RegPhyRxbePhyRxbeReg6D_u;

/*REG_PHY_RXBE_PHY_RXBE_REG6E 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscNumSym:15;	// risc_num_sym
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxbePhyRxbeReg6E_u;

/*REG_PHY_RXBE_PHY_RXBE_REG6F 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscPhyRate:15;	// risc_phy_rate
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxbePhyRxbeReg6F_u;

/*REG_PHY_RXBE_PHY_RXBE_REG70 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 evmResultSs0:8;	// evm_result_ss0
		uint32 evmResultSs1:8;	// evm_result_ss1
		uint32 evmResultSs2:8;	// evm_result_ss2
		uint32 evmResultSs3:8;	// evm_result_ss3
	} bitFields;
} RegPhyRxbePhyRxbeReg70_u;

/*REG_PHY_RXBE_PHY_RXBE_REG71 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigA1:24;	// vht_sig_a1
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg71_u;

/*REG_PHY_RXBE_PHY_RXBE_REG72 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigA2:24;	// vht_sig_a2
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg72_u;

/*REG_PHY_RXBE_PHY_RXBE_REG73 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigB:24;	// vht_sig_b
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg73_u;

/*REG_PHY_RXBE_PHY_RXBE_REG74 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htSig2:24;	// ht_sig2
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg74_u;

/*REG_PHY_RXBE_PHY_RXBE_REG75 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htSig3:24;	// ht_sig3
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg75_u;

/*REG_PHY_RXBE_PHY_RXBE_REG76 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lsig:24;	// lsig
		uint32 reserved0:8;
	} bitFields;
} RegPhyRxbePhyRxbeReg76_u;

/*REG_PHY_RXBE_PHY_RXBE_REG77 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfHeaderDelayLimit:8;	// bf_header_delay_limit
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxbePhyRxbeReg77_u;



#endif // _PHY_RXBE_REGS_H_
