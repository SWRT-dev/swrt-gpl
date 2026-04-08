
/***********************************************************************************
File:				PhyRxTdIfRiscPage0TxbRegs.h
Module:				phyRxTdIfRiscPage0Txb
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_RX_TD_IF_RISC_PAGE_0_TXB_REGS_H_
#define _PHY_RX_TD_IF_RISC_PAGE_0_TXB_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C0                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10700)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C1                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10704)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C2                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10708)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C4                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10710)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C5                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10714)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C6                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10718)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG_SW_RESET_N_GEN         (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1071C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C8                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10720)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C9                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10724)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG_TXB_RISC_INT_STATUS    (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10728)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1CB                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1072C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1CC                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10730)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG_GRISC_STATE_IND        (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10738)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1CF                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1073C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D0                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10740)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D1                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10744)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D2                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10748)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D3                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1074C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D4                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10750)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D5                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10754)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D6                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10758)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D7                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1075C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D8                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10760)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D9                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10764)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DA                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10768)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DB                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1076C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DC                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10770)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DD                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10774)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DE                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10778)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DF                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1077C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E0                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10780)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E2                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10788)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E3                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1078C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E4                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10790)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E5                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10794)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E6                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10798)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E7                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1079C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E9                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107A4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1EC                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107B0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1EF                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107BC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F4                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107D0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F5                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107D4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F6                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107D8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F7                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107DC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F8                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107E0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_FFT_OVERFLOW                            (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107E4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_ERROR_INDICATION                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107E8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_TSSI_A                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107EC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_TSSI_A                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107F0)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_TSSI_A                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107F4)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_TSSI_A                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107F8)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_TSSI_B                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x107FC)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_TSSI_B                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10800)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_TSSI_B                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10804)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_TSSI_B                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10808)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_TSSI_C                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1080C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_TSSI_C                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10810)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_TSSI_C                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10814)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_TSSI_C                          (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10818)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_ANALOG_GAIN                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1081C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_ANALOG_GAIN                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10820)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_ANALOG_GAIN                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10824)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_ANALOG_GAIN                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10828)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TR_TX_SWITCH_VECT                       (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1082C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_DATA_ERROR                           (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10830)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U1_PARAM1                         (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10834)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U1_PARAM2                         (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10838)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U2_PARAM1                         (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1083C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U2_PARAM2                         (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10840)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U3_PARAM1                         (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10844)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U3_PARAM2                         (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10848)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_LDPC_AIR_TIME_OVERRIDE                  (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x1084C)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_ERROR_INDICATION                  (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10850)
#define	REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_TRAINING_DONE                     (PHY_RX_TD_IF_RISC_PAGE_0_TXB_BASE_ADDRESS + 0x10854)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C0 0x10700 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 trcN1:3;	// TRC fix value
		uint32 trcMult3:1;	// TRC mult 3 of fix input
		uint32 rrcN1Ant0:3;	//  antenna 0 RRC fix value
		uint32 reserved0:1;
		uint32 rrcN1Ant1:3;	//  antenna 1 RRC fix value
		uint32 reserved1:1;
		uint32 rrcN1Ant2:3;	//  antenna 2 RRC fix value
		uint32 reserved2:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C1 0x10704 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dccStep:5;	// DC Canceler step size
		uint32 reserved0:1;
		uint32 dccMode:2;	// DC Canceler mode
		uint32 dccUpdateRate:4;	// DC Canceler update rate
		uint32 reserved1:2;
		uint32 rxFreqShiftEn:1;	// RX Frequency shift enable
		uint32 rxFreqShiftMode:1;	// RX Frequency shift mode
		uint32 reserved2:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C2 0x10708 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dccRef:16;	// DC Canceler reference value
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C4 0x10710 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorThreshold:7;	// Detectors threshold
		uint32 reserved0:1;
		uint32 det11BEnBypass:1;	// 11B detectors bypass enable
		uint32 riscIf2BiuGclkBypassEn:1;	// grisc interface to BIU gclk bypass enable
		uint32 reserved1:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C5 0x10714 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg:10;	// SW Reset register
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C6 0x10718 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sublockEnable:10;	// Block enable
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG_SW_RESET_N_GEN 0x1071C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNGen:10;	// SW Reset generate register
		uint32 ofdmTxSwResetNGen:1;	// ofdm_tx_sw_reset_n_gen
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscRegSwResetNGen_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C8 0x10720 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmResetGen44:4;	// gsm_reset_en
		uint32 gsmResetTxGen:6;	// gsm_reset_en
		uint32 gsmResetRxGen:6;	// gsm_reset_en
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C8_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1C9 0x10724 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txbRiscIntEnable:16;	// Interrupt enable register
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1C9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG_TXB_RISC_INT_STATUS 0x10728 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txbRiscIntStatusClear_txbRiscIntStatus:16;	// Interrupt status register
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscRegTxbRiscIntStatus_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1CB 0x1072C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 dtRasChooseAnt:1;	// Indicate RAS to choose antenna
		uint32 dtRasBlackoutEn:1;	// Indicate RAS to Enter into blackout period
		uint32 dtBestEn:1;	// Enable RAS best detection.
		uint32 dtRasChooseAntTh:5;	// Timeout to automatically choose antenna
		uint32 dtRasPreDetThr:3;	// RAS pre det samples threshold
		uint32 reserved1:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Cb_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1CC 0x10730 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtRasBlackoutPeriod:5;	// RAS blackout period 
		uint32 dtRasTimeoutTh:5;	// RAS timout threshold
		uint32 dtRasThr2:3;	// RAS samples threshold
		uint32 dtRasSumNum:3;	// RAS sum number
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Cc_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG_GRISC_STATE_IND 0x10738 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscStateInd:2;	// Set State Machine to state
		uint32 reserved0:2;
		uint32 clkCtrl11BState:2;	// Read 11B clock control state
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscRegGriscStateInd_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1CF 0x1073C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr0PhyMode:2;	// tcr0_phy_mode
		uint32 tcr0Cbw:2;	// tcr0_cbw
		uint32 tcr0RfPower:6;	// tcr0_rf_power
		uint32 tcr0BfMode:2;	// tcr0_bf_mode
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Cf_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D0 0x10740 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr0MacAntSelect:8;	// tcr0_mac_ant_select
		uint32 tcr0GenRisc0:4;	// tcr0_gen_risc0
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D1 0x10744 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr1Ldpc:1;	// tcr1_ldpc
		uint32 tcr1Stbc:2;	// tcr1_stbc
		uint32 tcr1PartialAid:9;	// tcr1_partial_aid
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D2 0x10748 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr1TxopPsNotAllowed:1;	// tcr1_txop_ps_not_allowed
		uint32 tcr1GroupId:6;	// tcr1_group_id
		uint32 tcr1Reserve:2;	// tcr1_reserve
		uint32 tcr1MuTrainingGrouping:1;	// tcr1_mu_training_grouping
		uint32 tcr1MuTraining:1;	// tcr1_mu_training
		uint32 tcr1MuPhyNdp:1;	// tcr1_mu_phy_ndp
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D3 0x1074C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr2CddAnt1:4;	// tcr2_cdd_ant1
		uint32 tcr2CddAnt2:4;	// tcr2_cdd_ant2
		uint32 tcr2CddAnt3:4;	// tcr2_cdd_ant3
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D3_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D4 0x10750 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr2BwChange:1;
		uint32 tcr2PowerBoost:8;	// power boost
		uint32 tcr2GenRisc1:3;	// tcr2_gen_risc1
		uint32 reserved0:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D5 0x10754 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr3Aggregate:1;	// tcr3_aggregate
		uint32 tcr3NotSounding:1;	// tcr3_not_sounding
		uint32 tcr3BfSmoothing:1;	// tcr3_bf_smoothing
		uint32 tcr3DynBw:1;	// tcr3_dyn_bw
		uint32 tcr3Reserved0:4;	// tcr3_reserved0
		uint32 tcr3Mcs:6;	// tcr3_mcs
		uint32 tcr3McsNss:2;	// tcr3_mcs_nss
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D6 0x10758 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr3ShortCp:1;	// tcr3_short_cp
		uint32 tcr3Reserved1:2;	// tcr3_reserved1
		uint32 tcr3Reserved2:7;
		uint32 reserved0:22;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D7 0x1075C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr4LengthLow:16;	// tcr4_length_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D7_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D8 0x10760 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcr4LengthHigh:4;	// tcr4_length_high
		uint32 tcr4Reserved:4;	// tcr4_reserved
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D8_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1D9 0x10764 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrCnt:3;	// tcr_cnt
		uint32 reserved0:1;
		uint32 bfHeaderValid:1;	// bf_header_valid
		uint32 u1TcrCnt:3;	// tcr_cnt
		uint32 u2TcrCnt:3;	// tcr_cnt
		uint32 u3TcrCnt:3;	// tcr_cnt
		uint32 reserved1:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1D9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DA 0x10768 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfHeaderReportLength:9;	// bf_header_report_length
		uint32 bfHeaderSounding:1;	// bf_header_sounding
		uint32 bfHeaderImplicit:1;	// bf_header_implicit
		uint32 bfHeaderSpare:4;	// bf_header_spare
		uint32 bfHeaderVht:1;	// bf_header_vht
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Da_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DB 0x1076C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mimoCtrlNc:3;	// mimo_ctrl_nc
		uint32 reserved0:1;
		uint32 mimoCtrlNr:3;	// mimo_ctrl_nr
		uint32 reserved1:1;
		uint32 mimoCtrlChannelWidth:2;	// mimo_ctrl_channel_width
		uint32 reserved2:2;
		uint32 mimoCtrlGrouping:2;	// mimo_ctrl_grouping
		uint32 reserved3:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Db_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DC 0x10770 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mimoCtrlCoeffSize:2;	// mimo_ctrl_coeff_size
		uint32 reserved0:2;
		uint32 mimoCtrlCodebook:2;	// mimo_ctrl_codebook
		uint32 reserved1:2;
		uint32 mimoCtrlReserve:4;	// mimo_ctrl_reserve
		uint32 reserved2:20;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Dc_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DD 0x10774 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfMeanSnrSs0:8;	// bf_mean_snr_ss0
		uint32 bfMeanSnrSs1:8;	// bf_mean_snr_ss1
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Dd_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DE 0x10778 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfMeanSnrSs2:8;	// bf_mean_snr_ss2
		uint32 bfMeanSnrSs3:8;	// bf_mean_snr_ss3
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1De_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1DF 0x1077C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfCalGainIndexAnt0:4;	// calibration gain number
		uint32 bfCalGainIndexAnt1:4;	// calibration gain number
		uint32 bfCalGainIndexAnt2:4;	// calibration gain number
		uint32 bfCalGainIndexAnt3:4;	// calibration gain number
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Df_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E0 0x10780 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bestDtTxAnt:1;	// Best antenna for TX 11b
		uint32 bestDtRxAnt:2;	// Best antenna for RX 11b
		uint32 reserved0:1;
		uint32 phyBeRxBErrorstat:2;	// Error status to MAC
		uint32 reserved1:26;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E0_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E2 0x10788 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcNCw:15;	// Standard LDPC number of codewords
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E3 0x1078C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcNData:11;	// Standard LDPC number of data bits in the first ldpc_rem_nshrt_ncw codewords
		uint32 reserved0:1;
		uint32 ldpcCwType:2;	// Standard LDPC Codeword type 0 - (Z = 27), 1 - (Z = 54), 2 - (Z = 81) , 
		uint32 reserved1:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E3_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E4 0x10790 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dccFreqShiftEn:1;	// DC Canceler Frequency enable
		uint32 dccFreqShiftMode:1;	// DC Canceler Frequency mode
		uint32 reserved0:30;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E5 0x10794 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 coreEvmI:14;	// Core EVM I
		uint32 reserved0:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E6 0x10798 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 coreEvmQ:14;	// Core EVM Q
		uint32 reserved0:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E7 0x1079C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 corePsdu:16;	// Core PSDU duration
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E7_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1E9 0x107A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpo:16;	// Spare Registers
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1E9_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1EC 0x107B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rasFinishToRxDly:8;	// DETECT to RX state transition
		uint32 rasFinishToRxDlyEn:1;	// DETECT to RX state transition
		uint32 reserved0:23;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Ec_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1EF 0x107BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIfTxState:4;	// 11B PHY-MAC IF TX State
		uint32 phyMacIfRxState:4;	// 11B PHY-MAC IF RX State
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1Ef_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F4 0x107D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcRemNshrtNcw:15;	// Standard LDPC Shortening: Remainder of Nshrt/Ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1F4_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F5 0x107D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcNParity:11;	// Standard LDPC number of parity bits in the first ldpc_rem_npunc_ncw codewords 
		uint32 reserved0:21;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1F5_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F6 0x107D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcRemNpuncNcw:15;	// Standard LDPC Puncturing: Remainder of Npunc/Ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1F6_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F7 0x107DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcNRepeat:13;	// Standard LDPC number of repeat bits in the first ldpc_rem_nrep_ncw codewords 
		uint32 reserved0:19;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1F7_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TXB_RISC_REG1F8 0x107E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcRemNrepNcw:15;	// Standard LDPC Repeating: Remainder of Nrep/Ncw
		uint32 reserved0:17;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTxbRiscReg1F8_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_FFT_OVERFLOW 0x107E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fftRx0Ov:1;	// fft_rx0_ov
		uint32 fftRx1Ov:1;	// fft_rx1_ov
		uint32 fftRx2Ov:1;	// fft_rx2_ov
		uint32 fftRx3Ov:1;	// fft_rx3_ov
		uint32 fftTx0Ov:1;	// fft_tx0_ov
		uint32 fftTx1Ov:1;	// fft_tx1_ov
		uint32 fftTx2Ov:1;	// fft_tx2_ov
		uint32 fftTx3Ov:1;	// fft_tx3_ov
		uint32 reserved0:24;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbFftOverflow_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_ERROR_INDICATION 0x107E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTcrError:4;	// tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4
		uint32 txBfError:9;	// bit 0 - packet is stbc + bf moving to flat. , bit 1 - time out during bf heaader. , bit 2 - time out during bf body. , bit 3 - bf header length is 0. , bit 4 -  implicit and report_nr < number of active antennas , bit 5 -  bf_report_nc<tx_nss;  , bit 6 - report BW is less than TCR BW , bit 7 - report BW is more than actual TX BW , bit 8 - report BW is 80Mhz and packet type is legacy
		uint32 txAirTimeError:1;	// tx_air_time_error
		uint32 txAntennaSelectionError:2;	// bit 13 - active antennas are less than the number of spatial streams. , bit 14 - number of prog model antennas are less than active antennas.
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxErrorIndication_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_TSSI_A 0x107EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0TssiA:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx0TssiA_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_TSSI_A 0x107F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1TssiA:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx1TssiA_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_TSSI_A 0x107F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2TssiA:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx2TssiA_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_TSSI_A 0x107F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3TssiA:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx3TssiA_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_TSSI_B 0x107FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0TssiB:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx0TssiB_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_TSSI_B 0x10800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1TssiB:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx1TssiB_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_TSSI_B 0x10804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2TssiB:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx2TssiB_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_TSSI_B 0x10808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3TssiB:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx3TssiB_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_TSSI_C 0x1080C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0TssiC:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx0TssiC_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_TSSI_C 0x10810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1TssiC:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx1TssiC_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_TSSI_C 0x10814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2TssiC:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx2TssiC_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_TSSI_C 0x10818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3TssiC:16;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx3TssiC_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX0_ANALOG_GAIN 0x1081C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx0PgcGain:2;
		uint32 tx0PaDrv:4;
		uint32 tx0S2DOffset:5;
		uint32 tx0S2DGain:4;
		uint32 tx0RfOn:1;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx0AnalogGain_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX1_ANALOG_GAIN 0x10820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx1PgcGain:2;
		uint32 tx1PaDrv:4;
		uint32 tx1S2DOffset:5;
		uint32 tx1S2DGain:4;
		uint32 tx1RfOn:1;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx1AnalogGain_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX2_ANALOG_GAIN 0x10824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx2PgcGain:2;
		uint32 tx2PaDrv:4;
		uint32 tx2S2DOffset:5;
		uint32 tx2S2DGain:4;
		uint32 tx2RfOn:1;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx2AnalogGain_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_PHY_TX3_ANALOG_GAIN 0x10828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx3PgcGain:2;
		uint32 tx3PaDrv:4;
		uint32 tx3S2DOffset:5;
		uint32 tx3S2DGain:4;
		uint32 tx3RfOn:1;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbPhyTx3AnalogGain_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TR_TX_SWITCH_VECT 0x1082C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxAntennaStby:4;
		uint32 txAntennaStby:4;
		uint32 txAntennaReserved:4;
		uint32 ssiAntennaStby:4;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTrTxSwitchVect_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_DATA_ERROR 0x10830 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDataError:4;	// valid is low during data state of the tx phy mac
		uint32 txBfErrorFull:10;	// bit 0 - packet is stbc + bf moving to flat. , bit 1 - time out during bf heaader. , bit 2 - time out during bf body. , bit 3 - bf header length is 0. , bit 4 -  implicit and report_nr < number of active antennas , bit 5 -  bf_report_nc<tx_nss;  , bit 6 - report BW is less than TCR BW , bit 7 - report BW is more than actual TX BW , bit 8 - report BW is 80Mhz and packet type is legacy , bit 9 - bf_type is bf in tx ndp
		uint32 reserved0:18;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxDataError_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U1_PARAM1 0x10834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u1Ldpc:1;	// u1_ldpc
		uint32 reserved0:3;
		uint32 u1Mcs:4;	// u1_mcs
		uint32 u1Rank:1;	// u1_rank
		uint32 reserved1:3;
		uint32 u1LengthHigh:4;	// u1_length_high
		uint32 reserved2:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuU1Param1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U1_PARAM2 0x10838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u1LengthLow:16;	// u1_length_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuU1Param2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U2_PARAM1 0x1083C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u2Ldpc:1;	// u2_ldpc
		uint32 reserved0:3;
		uint32 u2Mcs:4;	// u2_mcs
		uint32 u2Rank:1;	// u2_rank
		uint32 reserved1:3;
		uint32 u2LengthHigh:4;	// u2_length_high
		uint32 reserved2:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuU2Param1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U2_PARAM2 0x10840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u2LengthLow:16;	// u2_length_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuU2Param2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U3_PARAM1 0x10844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u3Ldpc:1;	// u3_ldpc
		uint32 reserved0:3;
		uint32 u3Mcs:4;	// u3_mcs
		uint32 u3Rank:1;	// u3_rank
		uint32 reserved1:3;
		uint32 u3LengthHigh:4;	// u3_length_high
		uint32 reserved2:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuU3Param1_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_U3_PARAM2 0x10848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u3LengthLow:16;	// u3_length_low
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuU3Param2_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_LDPC_AIR_TIME_OVERRIDE 0x1084C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ldpcTxRiscOverride:1;	// ldpc_tx_risc_override
		uint32 ldpcParamReady:1;	// Standard LDPC parmeters ready pulse indication
		uint32 ldpcAirTimeUserSelect:2;	// user select for update
		uint32 ldpcAirTimeUserSelectLatch:1;	// user select latch pulse
		uint32 reserved0:27;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbLdpcAirTimeOverride_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_ERROR_INDICATION 0x10850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u1TxTcrError:4;	// user1: tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4
		uint32 u2TxTcrError:4;	// user2: tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4
		uint32 u3TxTcrError:4;	// user3: tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4
		uint32 txMuTcrError:4;
		uint32 reserved0:16;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuErrorIndication_u;

/*REG_PHY_RX_TD_IF_RISC_PAGE_0_TXB_TX_MU_TRAINING_DONE 0x10854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingDone:1;	// done pulse trigger for mu training sequence
		uint32 reserved0:31;
	} bitFields;
} RegPhyRxTdIfRiscPage0TxbTxMuTrainingDone_u;



#endif // _PHY_RX_TD_IF_RISC_PAGE_0_TXB_REGS_H_
