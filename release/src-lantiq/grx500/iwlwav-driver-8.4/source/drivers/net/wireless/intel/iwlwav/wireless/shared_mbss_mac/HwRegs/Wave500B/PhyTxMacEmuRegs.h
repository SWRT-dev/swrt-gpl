
/***********************************************************************************
File:				PhyTxMacEmuRegs.h
Module:				phyTxMacEmu
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_TX_MAC_EMU_REGS_H_
#define _PHY_TX_MAC_EMU_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_TX_MAC_EMU_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG00         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38000)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG01         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38004)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG02         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38008)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR0             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x3800C)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38010)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR2             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38014)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38018)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG07         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x3801C)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG08         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38020)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG09         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38024)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0A         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38028)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0B         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x3802C)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0C         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38030)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0D         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38034)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0E         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38038)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4             (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x3803C)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG11         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38044)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG13         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x3804C)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG14         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38050)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38054)
#define	REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG16         (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38058)
#define	REG_PHY_TX_MAC_EMU_TONE_GEN                 (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x3805C)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1_DEL1        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38060)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1_DEL2        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38064)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1_DEL3        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38068)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3_DEL1        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38070)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3_DEL2        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38074)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3_DEL3        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38078)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4_DEL1        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38080)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4_DEL2        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38084)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4_DEL3        (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38088)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB_DEL1    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38090)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB_DEL2    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38094)
#define	REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB_DEL3    (PHY_TX_MAC_EMU_BASE_ADDRESS + 0x38098)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG00 0x38000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuRun:1;	// Emulator Run
		uint32 emuTxDone:1;	// Emulator Transmit Done
		uint32 reserved0:30;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg00_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG01 0x38004 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuNumOfPackets:24;	// Number of Packets
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg01_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG02 0x38008 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 endlessMode:1;	// Endless Mode
		uint32 emuCrcInMode:1;	// Insert CRC
		uint32 emuCrcInvert:1;	// Currupt CRC
		uint32 emuCrcReverse:1;	// Reverse CRC
		uint32 emuPrefixEnable:1;	// Transmit Prefix Data
		uint32 emuPrefixSet:1;	// Select Prefix set
		uint32 reserved0:2;
		uint32 emuPrefixLen:6;	// Prefix Length
		uint32 reserved1:18;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg02_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR0 0x3800C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPacketType:2;	// Packet type , 0-a/g , 1-11b , 2-ht , 3-vht
		uint32 emuCbw:2;	// Packet band width , 0-20Mhz , 1-40Mhz , 2-80Mhz , 
		uint32 emuTxPower:6;	// tx power
		uint32 emuBfType:2;	// bf_type , 0-custom1 , 1-bf  , 2-custom2 , 3-flat
		uint32 emuAntSelect:8;	// antenna selection
		uint32 emuGenRisc0:4;	// TX MAC Emulator - gen risc spare 0
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr0_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1 0x38010 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLdpcMode:1;	// Packet FEC Mode
		uint32 emuTxStbc:2;	// Enable TX in STBC
		uint32 emuPartialAid:9;	// partial aid
		uint32 emuTxopPsNotAllowed:1;	// TX MAC Emulator - txop_ps_not_allowed
		uint32 emuGroupId:6;	// group id
		uint32 emuMacReserved:2;	// mac resereved - not used by phy
		uint32 emuMuTrainingGrouping:1;	// mu training mode: , 0: packet form , 1: training form
		uint32 emuMuTraining:1;	// indication to activate mu training during CTS to self
		uint32 emuMuPhyNdp:1;	// mu ndp phy (don't do Fourier transform)
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr1_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR2 0x38014 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuCddOfst1:4;	// cdd for ant1
		uint32 emuCddOfst2:4;	// cdd offset for ant2
		uint32 emuCddOfst3:4;	// cdd offset for ant 3
		uint32 emuBwChange:1;	// bw change
		uint32 emuPowerBoost:8;	// power boost
		uint32 emuGenRisc1:3;	// TX MAC Emulator - gen risc spare 1
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr2_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3 0x38018 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuAggregate:1;
		uint32 emuNotSounding:1;	// not sounding
		uint32 emuSmoothing:1;	// smoothing
		uint32 emuDynamicBw:1;	// dynamic bw
		uint32 emuTcr3ReservedLow:4;	// tcr3 reserved
		uint32 emuMcs:6;	// mcs
		uint32 emuMacTcr3Reserved:2;	// reserved for mac
		uint32 emuShortCp:1;	// short cp
		uint32 emuTcr3ReservedHigh:7;
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr3_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG07 0x3801C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuSignal1:24;	// Packet Signal-1
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg07_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG08 0x38020 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuSignal2:24;	// Packet Signal-2
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg08_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG09 0x38024 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuSignal3:24;	// Packet Signal-3
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg09_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0A 0x38028 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPatternMode:2;	// Transmit pattern mode
		uint32 reserved0:2;
		uint32 emuInsPrbsErr:1;	// Insert PRBS Errors
		uint32 reserved1:3;
		uint32 emuInsErrNum:9;	// PRBS Error Rate
		uint32 reserved2:15;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0A_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0B 0x3802C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPrbsType:32;	// PRBS Type
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0B_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0C 0x38030 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPrbsInit:32;	// PRBS Init Data
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0C_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0D 0x38034 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuFixedData:32;	// Fixed Data Word
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0D_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG0E 0x38038 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuInit2RdyDly:16;	// Init to Ready Delay
		uint32 emuIpgDly:16;	// IPG delay
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg0E_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4 0x3803C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLength:20;	// packet length
		uint32 emuTcr4Reserved:4;	// tcr4 resereved
		uint32 reserved0:8;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr4_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG11 0x38044 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macPhyEnableSync:4;	// MAC TX Enable synchronized
		uint32 reserved0:28;
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg11_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG13 0x3804C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuBfHeader10:32;	// emu_bf_header10
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg13_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG14 0x38050 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuBfHeader32:32;	// emu_bf_header32
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg14_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB 0x38054 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuVhtSigb:29;	// vht_sigb
		uint32 reserved0:3;
	} bitFields;
} RegPhyTxMacEmuMacEmuVhtSigb_u;

/*REG_PHY_TX_MAC_EMU_TX_MAC_EMU_REG16 0x38058 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuVhtSigbCrc:8;	// vht_sigb_crc del0
		uint32 emuVhtSigbCrcDel1:8;	// vht_sigb_crc del1
		uint32 emuVhtSigbCrcDel2:8;	// vht_sigb_crc del2
		uint32 emuVhtSigbCrcDel3:8;	// vht_sigb_crc del3
	} bitFields;
} RegPhyTxMacEmuTxMacEmuReg16_u;

/*REG_PHY_TX_MAC_EMU_TONE_GEN 0x3805C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 toneGenMode:1;
		uint32 toneGenLstfMode:1;
		uint32 toneGenOn:1;
		uint32 toneGenMixerMode:2;
		uint32 toneGenCycles:27;
	} bitFields;
} RegPhyTxMacEmuToneGen_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1_DEL1 0x38060 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLdpcModeDel1:1;	// Packet FEC Mode for del1
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr1Del1_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1_DEL2 0x38064 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLdpcModeDel2:1;	// Packet FEC Mode for del2
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr1Del2_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR1_DEL3 0x38068 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLdpcModeDel3:1;	// Packet FEC Mode for del3
		uint32 reserved0:31;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr1Del3_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3_DEL1 0x38070 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 emuMcsDel1:6;	// mcs for del1
		uint32 reserved1:18;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr3Del1_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3_DEL2 0x38074 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 emuMcsDel2:6;	// mcs for del2
		uint32 reserved1:18;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr3Del2_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR3_DEL3 0x38078 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 emuMcsDel3:6;	// mcs for del3
		uint32 reserved1:18;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr3Del3_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4_DEL1 0x38080 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLengthDel1:20;	// packet length for del1
		uint32 reserved0:12;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr4Del1_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4_DEL2 0x38084 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLengthDel2:20;	// packet length for del2
		uint32 reserved0:12;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr4Del2_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_TCR4_DEL3 0x38088 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuLengthDel3:20;	// packet length for del3
		uint32 reserved0:12;
	} bitFields;
} RegPhyTxMacEmuMacEmuTcr4Del3_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB_DEL1 0x38090 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuVhtSigbDel1:29;	// vht_sigb_del1
		uint32 reserved0:3;
	} bitFields;
} RegPhyTxMacEmuMacEmuVhtSigbDel1_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB_DEL2 0x38094 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuVhtSigbDel2:29;	// vht_sigb_del2
		uint32 reserved0:3;
	} bitFields;
} RegPhyTxMacEmuMacEmuVhtSigbDel2_u;

/*REG_PHY_TX_MAC_EMU_MAC_EMU_VHT_SIGB_DEL3 0x38098 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuVhtSigbDel3:29;	// vht_sigb_del3
		uint32 reserved0:3;
	} bitFields;
} RegPhyTxMacEmuMacEmuVhtSigbDel3_u;



#endif // _PHY_TX_MAC_EMU_REGS_H_
