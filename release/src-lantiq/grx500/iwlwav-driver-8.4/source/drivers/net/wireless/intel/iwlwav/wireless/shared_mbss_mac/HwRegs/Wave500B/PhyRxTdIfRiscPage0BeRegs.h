
/***********************************************************************************
File:				PhyRxTdIfRiscPage0BeRegs.h
Module:				phyRxTdIfRiscPage0Be
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_TD_IF_RISC_PAGE_0_BE_REGS_H_
#define _PHY_RX_TD_IF_RISC_PAGE_0_BE_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS                             MEMORY_MAP_UNIT_49_BASE_ADDRESS
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF180               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10600)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF181               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10604)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF182               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10608)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF183               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1060C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF184               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10610)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF185               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10614)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF186               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10618)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF187               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1061C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF188               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10620)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF189               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10624)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF18D               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10634)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF18E               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10638)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF190               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10640)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF192               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10648)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF193               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1064C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF194               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10650)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF195               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10654)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF_LDPC_ITER_VAL    (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1065C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19B               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1066C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19C               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10670)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19D               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10674)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19E               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10678)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19F               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1067C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A0               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10680)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A1               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10684)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF_RISC_INT         (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10688)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A3               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1068C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A4               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10690)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A5               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10694)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A6               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10698)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A7               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1069C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A8               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106A0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A9               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106A4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AA               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106A8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AB               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106AC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AC               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106B0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AD               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106B4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AE               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106B8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B0               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106C0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B2               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106C8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B3               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106CC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B4               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106D0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B5               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106D4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B6               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106D8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B8               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106E0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B9               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106E4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BA               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106E8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BB               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106EC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BC               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106F0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BD               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106F4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BE               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106F8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BF               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x106FC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C0               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10700)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C1               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10704)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C2               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10708)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C3               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1070C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C4               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10710)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C5               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10714)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C6               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10718)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C7               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1071C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C8               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10720)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C9               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10724)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CA               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10728)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CB               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1072C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CC               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10730)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CD               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10734)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CE               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10738)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CF               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1073C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D0               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10740)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D1               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10744)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D2               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10748)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D3               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x1074C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D4               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10750)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D5               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10754)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D6               (PHY_RX_TD_IF_RISC_PAGE_0_BE_BASE_ADDRESS + 0x10758)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF180 0x10600 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg:12;	// BE sub blocks sw reset
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf180_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF181 0x10604 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 blocksEn:12;	// BE_sub blocks block en
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf181_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF182 0x10608 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetGenerate:12;	// sw_reset_generate
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf182_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF183 0x1060C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmSwResetMask:12;	// gsm_sw_reset_mask
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf183_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF184 0x10610 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant1AdRssi:8;	// antenna1 ad rssi
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf184_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF185 0x10614 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant2AdRssi:8;	// antenna2 ad rssi
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf185_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF186 0x10618 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant3AdRssi:8;	// antenna3 ad rssi
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf186_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF187 0x1061C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant4AdRssi:8;	// antenna4 ad rssi
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf187_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF188 0x10620 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscSpare0:16;	// Gen Risc spare register 0
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf188_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF189 0x10624 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 genRiscSpare1:16;	// Gen Risc spare register 1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf189_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF18D 0x10634 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant0RfGain:8;	// ant0_rf_gain
		uint32 ant1RfGain:8;	// ant1_rf_gain
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf18D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF18E 0x10638 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant2RfGain:8;	// ant2_rf_gain
		uint32 ant3RfGain:8;	// ant3_rf_gain
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf18E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF190 0x10640 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 riscModulation:3;	// risc_modulation
		uint32 reserved0:1;
		uint32 riscCodeRate:2;	// risc_code_rate
		uint32 reserved1:2;
		uint32 riscViterbiNes:2;	// risc_viterbi_nes
		uint32 reserved2:2;
		uint32 riscOverride:1;	// risc_override
		uint32 riscMcsValid:1;	// risc_mcs_valid
		uint32 reserved3:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf190_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF192 0x10648 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant0NoiseEst:8;	// antenna0 noise estimation
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf192_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF193 0x1064C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant1NoiseEst:8;	// antenna1 noise estimation
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf193_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF194 0x10650 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant2NoiseEst:8;	// antenna2 noise estimation
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf194_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF195 0x10654 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ant3NoiseEst:8;	// antenna3 noise estimation
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf195_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF_LDPC_ITER_VAL 0x1065C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcIterationsNumber:8;	// ldpc_iterations_number
		uint32 reserved0:3;
		uint32 ldpcIterationsStopSignal:1;	// ldpc_iterations_number
		uint32 reserved1:3;
		uint32 ldpcIterationsNumberValid:1;	// ldpc iterations number valid
		uint32 reserved2:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIfLdpcIterVal_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19B 0x1066C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 aLength:12;	// a length
		uint32 aRate:4;	// a rate
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf19B_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19C 0x10670 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lSigReserved:1;	// L-SIG reserved bit
		uint32 sig1Parity:1;	// signal1 parity
		uint32 reserved0:13;
		uint32 sig1InfoValid:1;	// signal1 info valid
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf19C_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19D 0x10674 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nLength:16;	// n packet length
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf19D_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19E 0x10678 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nss:3;	// Nss
		uint32 reserved0:1;
		uint32 bw:2;	// bw
		uint32 stbc:2;	// STBC
		uint32 reserved1:1;
		uint32 aggregation:1;	// Aggregation
		uint32 shortGi:1;	// short GI
		uint32 fecType:1;	// fec type
		uint32 reserved2:1;
		uint32 smoothing:1;	// Smoothing
		uint32 sounding:1;	// Sounding
		uint32 htSig2Reserved:1;	// HT-SIG2 reserved bit
		uint32 reserved3:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf19E_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF19F 0x1067C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcTx:8;	// Tx crc
		uint32 reserved0:2;
		uint32 numHtLtf:3;	// Num HT LTF
		uint32 reserved1:2;
		uint32 sig3InfoValid:1;	// signal3 info valid
		uint32 reserved2:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf19F_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A0 0x10680 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sig1ParityErr:1;	// Signal 1 parity Error
		uint32 sig1InvalidRate:1;	// Signal 1 Invalid Rate
		uint32 sig1MinLengthErr:1;	// Signal 1 minimum length Error
		uint32 sig1ReservedErr:1;	// Signal 1 reserved bit Error
		uint32 sig23CrcErr:1;	// signal23 Crc Error
		uint32 sig23ReservedErr:1;	// signal23 reserved bits Error
		uint32 sig3McsErr:1;	// sig3_mcs_err
		uint32 sig3LengthErr:1;	// sig3_length_err
		uint32 mcs:7;	// MCS
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A1 0x10684 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcMaxChecks:8;	// LDPC max checks number
		uint32 ldpcMaxSpare:8;	// LDPC max spare number
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF_RISC_INT 0x10688 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxbeClearRiscCauseInt_rxbeRiscCauseInt:11;	// Rx BE Risc Cause int
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIfRiscInt_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A3 0x1068C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxbeRiscIntEnable:11;	// Rx BE Risc Int Enable
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A3_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A4 0x10690 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swGclkBypass:14;	// sw_gclk_bypass
		uint32 reserved0:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A5 0x10694 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcNData:11;	// ldpc_n_data
		uint32 reserved0:1;
		uint32 ldpcCwType:2;	// ldpc_cw_type
		uint32 reserved1:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A6 0x10698 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearCounters:1;	// clear counters
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A7 0x1069C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsErrPacketCnt:16;	// number of packets with PRBS error
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A7_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A8 0x106A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 crcErrPacketCnt:16;	// number of packets with CRC error
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A8_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1A9 0x106A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypRxbeCtrlStm:3;	// RxBE state machine
		uint32 phyMacIfMainStm:5;	// Phy MAC IF state machine
		uint32 reserved0:1;
		uint32 phyPacRxAggregate:1;	// aggregate bit connected to the mac
		uint32 phyPacRxPhyReady:1;	// ready bit connected to the mac
		uint32 reserved1:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1A9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AA 0x106A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 ignoreSig1Error:1;	// RxBE general purpose register
		uint32 reserved1:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Aa_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AB 0x106AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPacRxPhyError:1;	// Rx phy error
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Ab_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AC 0x106B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muNdp:1;	// mu_ndp
		uint32 reserved0:3;
		uint32 ndpMaxSupportedNss:2;	// mac control for ndp max allowed Nss
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Ac_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AD 0x106B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcNCw:15;	// ldpc_n_cw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Ad_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1AE 0x106B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcRemNshrtNcw:15;	// ldpc_rem_nshrt_ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Ae_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B0 0x106C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 ldpcNParity:11;	// ldpc_n_parity
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B2 0x106C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcNRepeat:13;	// ldpc_n_repeat
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B3 0x106CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcRemNrepNcw:15;	// ldpc_rem_nrep_ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B3_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B4 0x106D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lSigDuration:16;	// L-SIG Duration
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B5 0x106D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mcsUnsupported:1;	// MCS unsupported
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B6 0x106D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwMcsUnsupported:1;	// HW decision MCS unsupported
		uint32 reserved0:3;
		uint32 hwMcsUnsupportedCause:9;	// HW decision MCS unsupported cause
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B8 0x106E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcRemNpuncNcw:15;	// ldpc_rem_npunc_ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B8_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1B9 0x106E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigA1GroupId:6;	// vht_sig_a1_group_id
		uint32 reserved0:2;
		uint32 vhtMuPacket:1;	// vht_mu_packet
		uint32 reserved1:23;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1B9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BA 0x106E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigA1PartialAid:9;	// vht_sig_a1_partial_aid
		uint32 reserved0:3;
		uint32 vhtSigA1TxopPsNotAllowed:1;	// vht_sig_a1_txop_ps_not_allowed
		uint32 reserved1:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Ba_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BB 0x106EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:1;
		uint32 vhtSigA2B1:1;	// vht_sig_a2_b1
		uint32 reserved1:1;
		uint32 vhtSigA2B3:1;	// vht_sig_a2_b3
		uint32 reserved2:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Bb_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BC 0x106F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigAReserved:3;	// vht_sig_a_reserved
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Bc_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BD 0x106F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigBLengthLow:16;	// vht_sig_b_length_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Bd_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BE 0x106F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vhtSigBLengthHigh:5;	// vht_sig_b_length_high
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Be_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1BF 0x106FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dynBandwidthInNonHt:1;	// dyn_bandwidth_in_non_ht
		uint32 reserved0:3;
		uint32 chBandwidthInNonHt:2;	// ch_bandwidth_in_non_ht
		uint32 reserved1:2;
		uint32 vhtSigBCrcErr:1;	// vht_sig_b_crc_err
		uint32 vhtSigbErr:1;	// vht_sigb_err
		uint32 vhtSigbDone:1;	// vht_sigb_done
		uint32 reserved2:4;
		uint32 serviceValid:1;	// service_valid
		uint32 reserved3:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Bf_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C0 0x10700 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeViterbiNes:2;	// air_time_viterbi_nes
		uint32 reserved0:2;
		uint32 airTimeCodeRate:2;	// air_time_code_rate
		uint32 reserved1:2;
		uint32 airTimeModulation:3;	// air_time_modulation
		uint32 reserved2:4;
		uint32 airTimeResultHigh:1;	// air_time_result_high
		uint32 reserved3:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C1 0x10704 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeResultLow:16;	// air_time_result_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C2 0x10708 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimePhyRate:15;	// air_time_phy_rate
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C3 0x1070C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeNumSym:15;	// air_time_num_sym
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C3_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C4 0x10710 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcNData:11;	// air_time_ldpc_n_data
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C5 0x10714 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcRemNshrtNcw:15;	// air_time_ldpc_rem_nshrt_ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C6 0x10718 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcNParity:11;	// air_time_ldpc_n_parity
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C7 0x1071C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcRemNpuncNcw:15;	// air_time_ldpc_rem_npunc_ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C7_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C8 0x10720 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcNRepeat:13;	// air_time_ldpc_n_repeat
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C8_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1C9 0x10724 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcRemNrepNcw:15;	// air_time_ldpc_rem_nrep_ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1C9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CA 0x10728 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcNCw:15;	// air_time_ldpc_n_cw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Ca_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CB 0x1072C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 airTimeLdpcCwType:2;	// air_time_ldpc_cw_type
		uint32 reserved0:13;
		uint32 airTimeDone:1;	// air_time_done
		uint32 reserved1:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Cb_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CC 0x10730 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 baseLdpcMaxChecks:8;	// base_ldpc_max_checks
		uint32 baseLdpcMaxSpare:8;	// base_ldpc_max_spare
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Cc_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CD 0x10734 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enableDynamicMaxCheck:1;	// enable_dynamic_max_check
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Cd_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CE 0x10738 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extraLdpcCycles:12;	// extra_ldpc_cycles
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Ce_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1CF 0x1073C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyStationId:8;	// station_id
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1Cf_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D0 0x10740 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyAbort:1;	// abort
		uint32 pacPhyHalt:1;	// halt
		uint32 pacPhyGpReq:1;	// gp request
		uint32 reserved0:29;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1D0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D1 0x10744 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcMinChecks:8;	// ldpc_min_checks
		uint32 ldpcExtraChecks:8;	// ldpc_extra_checks
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1D1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D2 0x10748 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcMaxRemCw:8;	// ldpc_max_rem_cw
		uint32 ldpcLastCw:8;	// ldpc_last_cw
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1D2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D3 0x1074C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxop:1;	// pac_phy_txop
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1D3_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D4 0x10750 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscGpAck:1;	// grisc_gp_ack
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1D4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D5 0x10754 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastVapIdx:4;	// last_vap_idx
		uint32 reserved0:28;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1D5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_BE_PHY_RXBE_IF1D6 0x10758 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extendAirTime:1;	// extend_air_time
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0BePhyRxbeIf1D6_u;



#endif // _PHY_RX_TD_IF_RISC_PAGE_0_BE_REGS_H_
