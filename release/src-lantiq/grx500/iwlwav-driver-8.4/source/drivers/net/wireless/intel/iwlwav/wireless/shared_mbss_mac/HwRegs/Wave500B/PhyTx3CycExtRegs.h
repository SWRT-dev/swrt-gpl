
/***********************************************************************************
File:				PhyTx3CycExtRegs.h
Module:				phyTx3CycExt
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_TX3_CYC_EXT_REGS_H_
#define _PHY_TX3_CYC_EXT_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_TX3_CYC_EXT_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG21    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x33084)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG2E    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330B8)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG30    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330C0)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG31    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330C4)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG32    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330C8)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG33    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330CC)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG34    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330D0)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG35    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330D4)
#define	REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG36    (PHY_TX3_CYC_EXT_BASE_ADDRESS + 0x330D8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG21 0x33084 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rampDownEndAddr:7;	// Tx3 Cyc Ramp Down End Address
		uint32 reserved0:1;
		uint32 rampUpEndAddr:7;	// Tx3 Cyc Ramp Up End Address
		uint32 reserved1:17;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg21_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG2E 0x330B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gclkBypass:2;	// Tx3 Gclk bypass
		uint32 reserved0:30;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg2E_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG30 0x330C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cycOutAmpGain0:10;	// Tx3 Amplitude gain factor 0
		uint32 reserved0:6;
		uint32 cycOutAmpGain1:10;	// Tx3 Amplitude gain factor 1
		uint32 reserved1:6;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg30_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG31 0x330C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cycOutAmpGain2:10;	// Tx3 Amplitude gain factor 2
		uint32 reserved0:6;
		uint32 cycOutAmpGain3:10;	// Tx3 Amplitude gain factor 3
		uint32 reserved1:6;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg31_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG32 0x330C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cycOutAmpGain4:10;	// Tx3 Amplitude gain factor 4
		uint32 reserved0:6;
		uint32 cycOutAmpGain5:10;	// Tx3 Amplitude gain factor 5
		uint32 reserved1:6;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg32_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG33 0x330CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cycOutAmpGain6:10;	// Tx3 Amplitude gain factor 6
		uint32 reserved0:6;
		uint32 cycOutAmpGain7:10;	// Tx3 Amplitude gain factor 7
		uint32 reserved1:6;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg33_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG34 0x330D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cycOutAmpGain8:10;	// Tx3 Amplitude gain factor 8
		uint32 reserved0:6;
		uint32 cycOutAmpGain9:10;	// Tx3 Amplitude gain factor 9
		uint32 reserved1:6;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg34_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG35 0x330D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cycOutAmpGainA:10;	// Tx3 Amplitude gain factor a
		uint32 reserved0:6;
		uint32 cycOutAmpGainB:10;	// Tx3 Amplitude gain factor b
		uint32 reserved1:6;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg35_u;

/*REG_PHY_TX3_CYC_EXT_TX3_CYC_EXT_REG36 0x330D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cycOutAmpGainC:10;	// Tx3 Amplitude gain factor c
		uint32 reserved0:6;
		uint32 cycOutAmpGainD:10;	// Tx3 Amplitude gain factor d
		uint32 reserved1:6;
	} bitFields;
} RegPhyTx3CycExtTx3CycExtReg36_u;



#endif // _PHY_TX3_CYC_EXT_REGS_H_
