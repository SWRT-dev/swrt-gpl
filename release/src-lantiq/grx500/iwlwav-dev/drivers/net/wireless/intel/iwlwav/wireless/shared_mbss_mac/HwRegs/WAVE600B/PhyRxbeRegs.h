
/***********************************************************************************
File:				PhyRxbeRegs.h
Module:				PhyRxBe
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_BE_REGS_H_
#define _PHY_RX_BE_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_BE_BASE_ADDRESS                             MEMORY_MAP_UNIT_49_BASE_ADDRESS
#define	REG_PHY_RX_BE_PHY_RXBE_REG04                (PHY_RX_BE_BASE_ADDRESS + 0x10)
#define	REG_PHY_RX_BE_PHY_RXBE_REG05                (PHY_RX_BE_BASE_ADDRESS + 0x14)
#define	REG_PHY_RX_BE_PHY_RXBE_REG07                (PHY_RX_BE_BASE_ADDRESS + 0x1C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG08                (PHY_RX_BE_BASE_ADDRESS + 0x20)
#define	REG_PHY_RX_BE_PHY_RXBE_REG18                (PHY_RX_BE_BASE_ADDRESS + 0x60)
#define	REG_PHY_RX_BE_PHY_RXBE_REG19                (PHY_RX_BE_BASE_ADDRESS + 0x64)
#define	REG_PHY_RX_BE_PHY_RXBE_REG1E                (PHY_RX_BE_BASE_ADDRESS + 0x78)
#define	REG_PHY_RX_BE_PHY_RXBE_REG20                (PHY_RX_BE_BASE_ADDRESS + 0x80)
#define	REG_PHY_RX_BE_PHY_RXBE_REG21                (PHY_RX_BE_BASE_ADDRESS + 0x84)
#define	REG_PHY_RX_BE_PHY_RXBE_REG22                (PHY_RX_BE_BASE_ADDRESS + 0x88)
#define	REG_PHY_RX_BE_PHY_RXBE_REG23                (PHY_RX_BE_BASE_ADDRESS + 0x8C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG24                (PHY_RX_BE_BASE_ADDRESS + 0x90)
#define	REG_PHY_RX_BE_PHY_RXBE_REG25                (PHY_RX_BE_BASE_ADDRESS + 0x94)
#define	REG_PHY_RX_BE_PHY_RXBE_REG26                (PHY_RX_BE_BASE_ADDRESS + 0x98)
#define	REG_PHY_RX_BE_PHY_RXBE_REG28                (PHY_RX_BE_BASE_ADDRESS + 0xA0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG34                (PHY_RX_BE_BASE_ADDRESS + 0xD0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG35                (PHY_RX_BE_BASE_ADDRESS + 0xD4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG36                (PHY_RX_BE_BASE_ADDRESS + 0xD8)
#define	REG_PHY_RX_BE_PHY_RXBE_REG37                (PHY_RX_BE_BASE_ADDRESS + 0xDC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG38                (PHY_RX_BE_BASE_ADDRESS + 0xE0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG39                (PHY_RX_BE_BASE_ADDRESS + 0xE4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG3F                (PHY_RX_BE_BASE_ADDRESS + 0xFC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG41                (PHY_RX_BE_BASE_ADDRESS + 0x104)
#define	REG_PHY_RX_BE_PHY_RXBE_REG42                (PHY_RX_BE_BASE_ADDRESS + 0x108)
#define	REG_PHY_RX_BE_PHY_RXBE_REG4A                (PHY_RX_BE_BASE_ADDRESS + 0x128)
#define	REG_PHY_RX_BE_PHY_RXBE_REG4B                (PHY_RX_BE_BASE_ADDRESS + 0x12C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG4C                (PHY_RX_BE_BASE_ADDRESS + 0x130)
#define	REG_PHY_RX_BE_PHY_RXBE_REG4D                (PHY_RX_BE_BASE_ADDRESS + 0x134)
#define	REG_PHY_RX_BE_PHY_RXBE_REG4E                (PHY_RX_BE_BASE_ADDRESS + 0x138)
#define	REG_PHY_RX_BE_PHY_RXBE_REG4F                (PHY_RX_BE_BASE_ADDRESS + 0x13C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG50                (PHY_RX_BE_BASE_ADDRESS + 0x140)
#define	REG_PHY_RX_BE_PHY_RXBE_REG51                (PHY_RX_BE_BASE_ADDRESS + 0x144)
#define	REG_PHY_RX_BE_PHY_RXBE_REG52                (PHY_RX_BE_BASE_ADDRESS + 0x148)
#define	REG_PHY_RX_BE_PHY_RXBE_REG53                (PHY_RX_BE_BASE_ADDRESS + 0x14C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG54                (PHY_RX_BE_BASE_ADDRESS + 0x150)
#define	REG_PHY_RX_BE_PHY_RXBE_REG55                (PHY_RX_BE_BASE_ADDRESS + 0x154)
#define	REG_PHY_RX_BE_PHY_RXBE_REG56                (PHY_RX_BE_BASE_ADDRESS + 0x158)
#define	REG_PHY_RX_BE_PHY_RXBE_REG57                (PHY_RX_BE_BASE_ADDRESS + 0x15C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG58                (PHY_RX_BE_BASE_ADDRESS + 0x160)
#define	REG_PHY_RX_BE_PHY_RXBE_REG59                (PHY_RX_BE_BASE_ADDRESS + 0x164)
#define	REG_PHY_RX_BE_PHY_RXBE_REG5A                (PHY_RX_BE_BASE_ADDRESS + 0x168)
#define	REG_PHY_RX_BE_PHY_RXBE_REG5D                (PHY_RX_BE_BASE_ADDRESS + 0x174)
#define	REG_PHY_RX_BE_PHY_RXBE_REG5E                (PHY_RX_BE_BASE_ADDRESS + 0x178)
#define	REG_PHY_RX_BE_PHY_RXBE_REG64                (PHY_RX_BE_BASE_ADDRESS + 0x190)
#define	REG_PHY_RX_BE_PHY_RXBE_REG65                (PHY_RX_BE_BASE_ADDRESS + 0x194)
#define	REG_PHY_RX_BE_PHY_RXBE_REG67                (PHY_RX_BE_BASE_ADDRESS + 0x19C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG68                (PHY_RX_BE_BASE_ADDRESS + 0x1A0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG69                (PHY_RX_BE_BASE_ADDRESS + 0x1A4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG6A                (PHY_RX_BE_BASE_ADDRESS + 0x1A8)
#define	REG_PHY_RX_BE_PHY_RXBE_REG6B                (PHY_RX_BE_BASE_ADDRESS + 0x1AC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG6C                (PHY_RX_BE_BASE_ADDRESS + 0x1B0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG6D                (PHY_RX_BE_BASE_ADDRESS + 0x1B4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG6E                (PHY_RX_BE_BASE_ADDRESS + 0x1B8)
#define	REG_PHY_RX_BE_PHY_RXBE_REG6F                (PHY_RX_BE_BASE_ADDRESS + 0x1BC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG70                (PHY_RX_BE_BASE_ADDRESS + 0x1C0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG71                (PHY_RX_BE_BASE_ADDRESS + 0x1C4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG72                (PHY_RX_BE_BASE_ADDRESS + 0x1C8)
#define	REG_PHY_RX_BE_PHY_RXBE_REG73                (PHY_RX_BE_BASE_ADDRESS + 0x1CC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG74                (PHY_RX_BE_BASE_ADDRESS + 0x1D0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG75                (PHY_RX_BE_BASE_ADDRESS + 0x1D4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG76                (PHY_RX_BE_BASE_ADDRESS + 0x1D8)
#define	REG_PHY_RX_BE_PHY_RXBE_REG77                (PHY_RX_BE_BASE_ADDRESS + 0x1DC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG78                (PHY_RX_BE_BASE_ADDRESS + 0x1E0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG79                (PHY_RX_BE_BASE_ADDRESS + 0x1E4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG80                (PHY_RX_BE_BASE_ADDRESS + 0x1E8)
#define	REG_PHY_RX_BE_PHY_RXBE_REG81                (PHY_RX_BE_BASE_ADDRESS + 0x1EC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG82                (PHY_RX_BE_BASE_ADDRESS + 0x1F0)
#define	REG_PHY_RX_BE_PHY_RXBE_REG83                (PHY_RX_BE_BASE_ADDRESS + 0x1F4)
#define	REG_PHY_RX_BE_PHY_RXBE_REG84                (PHY_RX_BE_BASE_ADDRESS + 0x1F8)
#define	REG_PHY_RX_BE_PHY_RXBE_REG85                (PHY_RX_BE_BASE_ADDRESS + 0x1FC)
#define	REG_PHY_RX_BE_PHY_RXBE_REG86                (PHY_RX_BE_BASE_ADDRESS + 0x200)
#define	REG_PHY_RX_BE_PHY_RXBE_REG87                (PHY_RX_BE_BASE_ADDRESS + 0x204)
#define	REG_PHY_RX_BE_PHY_RXBE_REG88                (PHY_RX_BE_BASE_ADDRESS + 0x208)
#define	REG_PHY_RX_BE_PHY_RXBE_REG89                (PHY_RX_BE_BASE_ADDRESS + 0x20C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG90                (PHY_RX_BE_BASE_ADDRESS + 0x210)
#define	REG_PHY_RX_BE_PHY_RXBE_REG91                (PHY_RX_BE_BASE_ADDRESS + 0x214)
#define	REG_PHY_RX_BE_PHY_RXBE_REG92                (PHY_RX_BE_BASE_ADDRESS + 0x218)
#define	REG_PHY_RX_BE_PHY_RXBE_REG93                (PHY_RX_BE_BASE_ADDRESS + 0x21C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG94                (PHY_RX_BE_BASE_ADDRESS + 0x220)
#define	REG_PHY_RX_BE_PHY_RXBE_REG95                (PHY_RX_BE_BASE_ADDRESS + 0x224)
#define	REG_PHY_RX_BE_PHY_RXBE_REG96                (PHY_RX_BE_BASE_ADDRESS + 0x228)
#define	REG_PHY_RX_BE_PHY_RXBE_REG97                (PHY_RX_BE_BASE_ADDRESS + 0x22C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG98                (PHY_RX_BE_BASE_ADDRESS + 0x230)
#define	REG_PHY_RX_BE_PHY_RXBE_REG99                (PHY_RX_BE_BASE_ADDRESS + 0x234)
#define	REG_PHY_RX_BE_PHY_RXBE_REG9A                (PHY_RX_BE_BASE_ADDRESS + 0x238)
#define	REG_PHY_RX_BE_PHY_RXBE_REG9B                (PHY_RX_BE_BASE_ADDRESS + 0x23C)
#define	REG_PHY_RX_BE_PHY_RXBE_REG9C                (PHY_RX_BE_BASE_ADDRESS + 0x240)
#define	REG_PHY_RX_BE_PHY_RXBE_REGA1                (PHY_RX_BE_BASE_ADDRESS + 0x254)
#define	REG_PHY_RX_BE_PHY_RXBE_REGA2                (PHY_RX_BE_BASE_ADDRESS + 0x258)
#define	REG_PHY_RX_BE_LDPC_CALC_VALID               (PHY_RX_BE_BASE_ADDRESS + 0x25C)
#define	REG_PHY_RX_BE_LDPC_VITERBI_USER_ACTIVE      (PHY_RX_BE_BASE_ADDRESS + 0x260)
#define	REG_PHY_RX_BE_LDPC_USER_ACTIVE              (PHY_RX_BE_BASE_ADDRESS + 0x264)
#define	REG_PHY_RX_BE_LDPC_MAX_N_CW                 (PHY_RX_BE_BASE_ADDRESS + 0x268)
#define	REG_PHY_RX_BE_LDPC_CYCLES_PER_CW            (PHY_RX_BE_BASE_ADDRESS + 0x26C)
#define	REG_PHY_RX_BE_LDPC_MAX_CHECK_CONV           (PHY_RX_BE_BASE_ADDRESS + 0x270)
#define	REG_PHY_RX_BE_BUS_PAC_PHY_RX_CRC_CHECK_0    (PHY_RX_BE_BASE_ADDRESS + 0x274)
#define	REG_PHY_RX_BE_BUS_PAC_PHY_RX_CRC_CHECK_1    (PHY_RX_BE_BASE_ADDRESS + 0x278)
#define	REG_PHY_RX_BE_RCR_ACTIVE_USERS_0            (PHY_RX_BE_BASE_ADDRESS + 0x27C)
#define	REG_PHY_RX_BE_RCR_ACTIVE_USERS_1            (PHY_RX_BE_BASE_ADDRESS + 0x280)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_BE_PHY_RXBE_REG04 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpPhyMacEventLen : 1; //RxBE Test bus control, reset value: 0x0, access type: RW
		uint32 tpPhyMacDataLen : 1; //RxBE Test bus control, reset value: 0x0, access type: RW
		uint32 tpPhyMacStationIdLen : 1; //RxBE Test bus control, reset value: 0x0, access type: RW
		uint32 tpLdpcCwStatLen : 1; //RxBE Test bus control, reset value: 0x0, access type: RW
		uint32 tpLlrStreamLen : 1; //RxBE Test bus control, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
		uint32 tpDebugCntEn : 1; //Test Plug counter enable, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxBePhyRxbeReg04_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG05 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpLlrStreamSel : 2; //RxBE Test bus control, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxBePhyRxbeReg05_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG07 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpDebugClk2Strb : 4; //Test Bus debug clock per strobe, reset value: 0x6, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxBePhyRxbeReg07_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG08 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tpLowMuxCtrl : 2; //RxBE low test bus control, reset value: 0x0, access type: RW
		uint32 tpMidMuxCtrl : 2; //RxBE mid test bus control, reset value: 0x1, access type: RW
		uint32 tpHighMuxCtrl : 2; //RxBE high test bus control, reset value: 0x2, access type: RW
		uint32 tpFourMuxCtrl : 2; //RxBE high test bus control, reset value: 0x3, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxBePhyRxbeReg08_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG18 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 viterbiTbDepth : 6; //Viterbi length, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyRxBePhyRxbeReg18_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG19 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reportFullMetrics : 1; //Report full metrics, reset value: 0x0, access type: RW
		uint32 waitForGriscMetrics : 1; //Wait for gen risc metrics, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 waitForSnrMetrics : 1; //Wait for snr metrics, reset value: 0x1, access type: RW
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyRxBePhyRxbeReg19_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG1E 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numBitsInPacket : 25; //Number of bits in the packet, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegPhyRxBePhyRxbeReg1E_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG20 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 16;
		uint32 macIfMinCycle : 8; //Cycle length, reset value: 0x4, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg20_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG21 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packetLimit : 19; //Errors per Packet limit, reset value: 0x7ffff, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegPhyRxBePhyRxbeReg21_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG22 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsLoadVal : 32; //PRBS LSFR init value, reset value: 0x162, access type: CONST
	} bitFields;
} RegPhyRxBePhyRxbeReg22_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG23 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsCrcEnable : 1; //Enable PRBS Comparison, reset value: 0x0, access type: RW
		uint32 packetWithCrc : 1; //Enable CRC in packet, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxBePhyRxbeReg23_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG24 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearCounters : 1; //Clear PRBS Counters, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxBePhyRxbeReg24_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG25 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latchCounters : 1; //Latch PRBS Counters, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxBePhyRxbeReg25_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG26 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsType : 32; //PRBS Polynom type, reset value: 0x80000003, access type: CONST
	} bitFields;
} RegPhyRxBePhyRxbeReg26_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG28 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sigDecodeCrcLen : 3; //Signal CRC length, reset value: 0x7, access type: CONST
		uint32 reserved0 : 1;
		uint32 sigDecodeCrcType : 8; //Signal CRC type, reset value: 0x60, access type: CONST
		uint32 sigDecodeCrcCorrupt : 1; //Signal CRC corrupt, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 sigDecodeCrcInit : 8; //Signal CRC Init value, reset value: 0xff, access type: CONST
		uint32 reserved2 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg28_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG34 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrCntLatched : 32; //PRBS Error count - latched, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg34_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG35 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCntLatched310 : 32; //PRBS Bit count [31:0] - latched, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg35_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG36 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCntLatched4732 : 16; //PRBS Bit count [47:32] - latched, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxBePhyRxbeReg36_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG37 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcPacketCntLatched : 32; //Number of compared packets - latched, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg37_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG38 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcErrPacketCntLatched : 32; //Number of compared errored packets with CRC error - latched, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg38_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG39 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrPacketCntLatched : 32; //Number of compared errored packets with PRBS error - latched, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg39_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG3F 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBistStart : 1; //BIST start, reset value: 0x0, access type: RW
		uint32 clearRamMode : 1; //Clear RAM mode in BIST, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxBePhyRxbeReg3F_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG41 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxbeMaskParityErr : 1; //Mask parity error in signal 1, reset value: 0x0, access type: RW
		uint32 rxbeMaskRateErr : 1; //Mask invalid rate error in signal 1, reset value: 0x0, access type: RW
		uint32 rxbeMaskPSizeErr : 1; //Mask packet size error in signal 1, reset value: 0x0, access type: RW
		uint32 rxbeMaskReservedErr : 1; //Mask reserved bit error in signal 1, reset value: 0x0, access type: RW
		uint32 rxbeMaskCrcErr : 1; //Mask bad CRC error in signal 3, reset value: 0x0, access type: RW
		uint32 rxbeMaskSigbCrcErr : 1; //Mask bad CRC error in vht sigb, reset value: 0x0, access type: RW
		uint32 rxbeMaskSig3ReservedErr : 1; //Mask reserved bit error in signal 2/3, reset value: 0x0, access type: RW
		uint32 rxbeMaskSig3LengthErr : 1; //Mask min length check based on sig3 and sig1, reset value: 0x0, access type: RW
		uint32 rxbeMaskNdpErr : 1; //Mask ndp error, reset value: 0x0, access type: RW
		uint32 rxbeMaskNdpErrAll : 1; //rxbe_mask_ndp_err_all, reset value: 0x0, access type: RW
		uint32 rxbeMaskUnsupportedMcs : 18; //Mask unsupported mcs, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegPhyRxBePhyRxbeReg41_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG42 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 minimumPacketSize : 12; //Minimum packet size in bytes, reset value: 0xc, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxBePhyRxbeReg42_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG4A 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrCnt : 32; //PRBS Error count - real time, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg4A_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG4B 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCnt310 : 32; //PRBS Bit count [31:0] - real time, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg4B_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG4C 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsBitCnt4732 : 16; //PRBS Bit count [47:32] - real time, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxBePhyRxbeReg4C_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG4D 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcPacketCnt : 32; //Number of compared packets - real time, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg4D_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG4E 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcErrPacketCnt : 32; //Number of compared errored packets with CRC error  - real time, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg4E_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG4F 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrPacketCnt : 32; //Number of compared errored packets with PRBS error - real time, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg4F_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG50 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIfBitCounter : 23; //Phy MAC IF bit counter, reset value: 0x0, access type: RO
		uint32 reserved0 : 9;
	} bitFields;
} RegPhyRxBePhyRxbeReg50_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG51 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpo : 16; //RxBE general purpose output, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxBePhyRxbeReg51_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG52 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpr : 32; //RxBE general purpose register, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxBePhyRxbeReg52_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG53 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpi : 16; //RxBE general purpose input, reset value: 0xa080, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxBePhyRxbeReg53_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG54 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeGpi2 : 16; //RxBE general purpose input 2, reset value: 0xa080, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxBePhyRxbeReg54_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG55 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deintRamRm : 3; //deint_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxBePhyRxbeReg55_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG56 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rcrRamRm : 3; //rcr_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxBePhyRxbeReg56_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG57 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 viterbiTbBlockRamRm : 3; //viterbi_tb_block_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxBePhyRxbeReg57_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG58 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userRamRm : 3; //user_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxBePhyRxbeReg58_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG59 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muRamRm : 3; //mu_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegPhyRxBePhyRxbeReg59_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG5A 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rRam2Rm : 3; //r_ram2_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 obufRamRm : 3; //obuf_ram_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 25;
	} bitFields;
} RegPhyRxBePhyRxbeReg5A_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG5D 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkShutdownEn : 14; //gclk shutdown - when '1' clocks will be always deactive, reset value: 0x0, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyRxBePhyRxbeReg5D_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG5E 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bistScrBypass : 1; //bist_scr_bypass, reset value: 0x0, access type: RW
		uint32 ramTestMode : 1; //ram_test_mode, reset value: 0x0, access type: RW
		uint32 memGlobalRm : 2; //mem_global_rm, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxBePhyRxbeReg5E_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG64 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 globalRdyLimit : 16; //global_rdy_limit, reset value: 0x1CC0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyRxBePhyRxbeReg64_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG65 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableAirTimeBlock : 1; //enable_air_time_block, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxBePhyRxbeReg65_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG67 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtNonAggregate : 1; //vht_non_aggregate, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxBePhyRxbeReg67_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG68 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr0 : 32; //spare_gpr_0, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxBePhyRxbeReg68_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG69 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr1 : 32; //spare_gpr_1, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxBePhyRxbeReg69_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG6A 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr2 : 32; //spare_gpr_2, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxBePhyRxbeReg6A_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG6B 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareGpr3 : 32; //spare_gpr_3, reset value: 0x0, access type: RW
	} bitFields;
} RegPhyRxBePhyRxbeReg6B_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG6C 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txGlobalResetMask : 12; //reset rxbe units for tx, reset value: 0xfdf, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxBePhyRxbeReg6C_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG6D 0x1B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscAirTimeResult : 17; //risc_air_time_result, reset value: 0x0, access type: RW
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyRxBePhyRxbeReg6D_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG6E 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscNumSym : 15; //risc_num_sym, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxBePhyRxbeReg6E_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG6F 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscPhyRate : 15; //risc_phy_rate, reset value: 0x0, access type: RW
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxBePhyRxbeReg6F_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG70 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 evmResultSs0 : 8; //evm_result_ss0, reset value: 0x0, access type: RO
		uint32 evmResultSs1 : 8; //evm_result_ss1, reset value: 0x0, access type: RO
		uint32 evmResultSs2 : 8; //evm_result_ss2, reset value: 0x0, access type: RO
		uint32 evmResultSs3 : 8; //evm_result_ss3, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBePhyRxbeReg70_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG71 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigA1 : 24; //vht_sig_a1, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg71_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG72 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigA2 : 24; //vht_sig_a2, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg72_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG73 0x1CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigB : 24; //vht_sig_b, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg73_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG74 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htSig2 : 24; //ht_sig2, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg74_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG75 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 htSig3 : 24; //ht_sig3, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg75_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG76 0x1D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lsig : 24; //lsig, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPhyRxBePhyRxbeReg76_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG77 0x1DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfHeaderDelayLimit : 8; //bf_header_delay_limit, reset value: 0xf0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxBePhyRxbeReg77_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG78 0x1E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour0 : 6; //pm_colour_0, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour1 : 6; //pm_colour_1, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour2 : 6; //pm_colour_2, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour3 : 6; //pm_colour_3, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg78_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG79 0x1E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour4 : 6; //pm_colour_4, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour5 : 6; //pm_colour_5, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour6 : 6; //pm_colour_6, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour7 : 6; //pm_colour_7, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg79_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG80 0x1E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour8 : 6; //pm_colour_8, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour9 : 6; //pm_colour_9, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour10 : 6; //pm_colour_10, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour11 : 6; //pm_colour_11, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg80_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG81 0x1EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour12 : 6; //pm_colour_12, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour13 : 6; //pm_colour_13, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour14 : 6; //pm_colour_14, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour15 : 6; //pm_colour_15, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg81_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG82 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour16 : 6; //pm_colour_16, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour17 : 6; //pm_colour_17, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour18 : 6; //pm_colour_18, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour19 : 6; //pm_colour_19, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg82_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG83 0x1F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour20 : 6; //pm_colour_20, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour21 : 6; //pm_colour_21, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour22 : 6; //pm_colour_22, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour23 : 6; //pm_colour_23, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg83_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG84 0x1F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour24 : 6; //pm_colour_24, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour25 : 6; //pm_colour_25, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour26 : 6; //pm_colour_26, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour27 : 6; //pm_colour_27, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg84_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG85 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour28 : 6; //pm_colour_28, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour29 : 6; //pm_colour_29, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour30 : 6; //pm_colour_30, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour31 : 6; //pm_colour_31, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg85_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG86 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour32 : 6; //pm_colour_32, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour33 : 6; //pm_colour_33, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour34 : 6; //pm_colour_34, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour35 : 6; //pm_colour_35, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg86_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG87 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour36 : 6; //pm_colour_36, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour37 : 6; //pm_colour_37, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour38 : 6; //pm_colour_38, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour39 : 6; //pm_colour_39, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg87_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG88 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour40 : 6; //pm_colour_40, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour41 : 6; //pm_colour_41, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour42 : 6; //pm_colour_42, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour43 : 6; //pm_colour_43, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg88_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG89 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour44 : 6; //pm_colour_44, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour45 : 6; //pm_colour_45, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour46 : 6; //pm_colour_46, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour47 : 6; //pm_colour_47, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg89_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG90 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour48 : 6; //pm_colour_48, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour49 : 6; //pm_colour_49, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour50 : 6; //pm_colour_50, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour51 : 6; //pm_colour_51, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg90_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG91 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour52 : 6; //pm_colour_52, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour53 : 6; //pm_colour_53, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour54 : 6; //pm_colour_54, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour55 : 6; //pm_colour_55, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg91_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG92 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour56 : 6; //pm_colour_56, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour57 : 6; //pm_colour_57, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour58 : 6; //pm_colour_58, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour59 : 6; //pm_colour_59, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg92_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG93 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmColour60 : 6; //pm_colour_60, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 pmColour61 : 6; //pm_colour_61, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 pmColour62 : 6; //pm_colour_62, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 pmColour63 : 6; //pm_colour_63, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegPhyRxBePhyRxbeReg93_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG94 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmAirTimeInterval : 10; //pm_air_time_interval, reset value: 0x140, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyRxBePhyRxbeReg94_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG95 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pmCurrentUser : 6; //pm_current_user, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyRxBePhyRxbeReg95_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG96 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcTotalLastSym : 8; //ldpc_total_last_sym, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyRxBePhyRxbeReg96_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG97 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcTotalCw : 15; //ldpc_total_cw, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxBePhyRxbeReg97_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG98 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSigA1 : 26; //he_sig_a1, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
	} bitFields;
} RegPhyRxBePhyRxbeReg98_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG99 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSigA2 : 26; //he_sig_a2, reset value: 0x0, access type: RO
		uint32 reserved0 : 6;
	} bitFields;
} RegPhyRxBePhyRxbeReg99_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG9A 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 psduLen : 22; //psdu_len, reset value: 0x0, access type: RO
		uint32 reserved0 : 10;
	} bitFields;
} RegPhyRxBePhyRxbeReg9A_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG9B 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 userConfig : 12; //user_config, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxBePhyRxbeReg9B_u;

/*REG_PHY_RX_BE_PHY_RXBE_REG9C 0x240 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxbeSchPs : 5; //rxbe_sch_ps, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPhyRxBePhyRxbeReg9C_u;

/*REG_PHY_RX_BE_PHY_RXBE_REGA1 0x254 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcAlphaIndex : 2; //ldpc_alpha_index, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyRxBePhyRxbeRega1_u;

/*REG_PHY_RX_BE_PHY_RXBE_REGA2 0x258 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcQuantizeCycles : 1; //ldpc_quantize_cycles, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxBePhyRxbeRega2_u;

/*REG_PHY_RX_BE_LDPC_CALC_VALID 0x25C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcCalcValid : 1; //ldpc cycles calculator done, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyRxBeLdpcCalcValid_u;

/*REG_PHY_RX_BE_LDPC_VITERBI_USER_ACTIVE 0x260 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcViterbiUserActive : 6; //number of active users viterbi+ldpc, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyRxBeLdpcViterbiUserActive_u;

/*REG_PHY_RX_BE_LDPC_USER_ACTIVE 0x264 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcUserActive : 6; //number of active users ldpc only, reset value: 0x0, access type: RO
		uint32 reserved0 : 26;
	} bitFields;
} RegPhyRxBeLdpcUserActive_u;

/*REG_PHY_RX_BE_LDPC_MAX_N_CW 0x268 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcMaxNCw : 15; //maximal value of ldpc_n_cw from all ldpc users, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegPhyRxBeLdpcMaxNCw_u;

/*REG_PHY_RX_BE_LDPC_CYCLES_PER_CW 0x26C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcCyclesPerCw : 12; //calculated cycles per cw, reset value: 0x480, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyRxBeLdpcCyclesPerCw_u;

/*REG_PHY_RX_BE_LDPC_MAX_CHECK_CONV 0x270 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcMaxChecks : 16; //ldpc max checks, reset value: 0x0, access type: RO
		uint32 ldpcConvergence : 1; //ldpc max checks, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPhyRxBeLdpcMaxCheckConv_u;

/*REG_PHY_RX_BE_BUS_PAC_PHY_RX_CRC_CHECK_0 0x274 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 busPacPhyRxCrcCheck0 : 32; //bus_pac_phy_rx_crc_check_0, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBeBusPacPhyRxCrcCheck0_u;

/*REG_PHY_RX_BE_BUS_PAC_PHY_RX_CRC_CHECK_1 0x278 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 busPacPhyRxCrcCheck1 : 4; //bus_pac_phy_rx_crc_check_1, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxBeBusPacPhyRxCrcCheck1_u;

/*REG_PHY_RX_BE_RCR_ACTIVE_USERS_0 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rcrActiveUsers0 : 32; //rcr_active_users_0, reset value: 0x0, access type: RO
	} bitFields;
} RegPhyRxBeRcrActiveUsers0_u;

/*REG_PHY_RX_BE_RCR_ACTIVE_USERS_1 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rcrActiveUsers1 : 4; //rcr_active_users_1, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegPhyRxBeRcrActiveUsers1_u;



#endif // _PHY_RX_BE_REGS_H_
