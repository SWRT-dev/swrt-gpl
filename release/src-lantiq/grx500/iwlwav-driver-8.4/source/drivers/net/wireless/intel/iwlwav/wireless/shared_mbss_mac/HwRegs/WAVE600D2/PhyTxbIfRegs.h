
/***********************************************************************************
File:				PhyTxbIfRegs.h
Module:				PhyTxbIf
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PHY_TXB_IF_REGS_H_
#define _PHY_TXB_IF_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PHY_TXB_IF_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C0                     (PHY_TXB_IF_BASE_ADDRESS + 0x1700)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C1                     (PHY_TXB_IF_BASE_ADDRESS + 0x1704)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C2                     (PHY_TXB_IF_BASE_ADDRESS + 0x1708)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C4                     (PHY_TXB_IF_BASE_ADDRESS + 0x1710)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C5                     (PHY_TXB_IF_BASE_ADDRESS + 0x1714)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C6                     (PHY_TXB_IF_BASE_ADDRESS + 0x1718)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG_SW_RESET_N_GEN         (PHY_TXB_IF_BASE_ADDRESS + 0x171C)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C8                     (PHY_TXB_IF_BASE_ADDRESS + 0x1720)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C9                     (PHY_TXB_IF_BASE_ADDRESS + 0x1724)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG_TXB_RISC_INT_STATUS    (PHY_TXB_IF_BASE_ADDRESS + 0x1728)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1CB                     (PHY_TXB_IF_BASE_ADDRESS + 0x172C)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1CC                     (PHY_TXB_IF_BASE_ADDRESS + 0x1730)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG_GRISC_STATE_IND        (PHY_TXB_IF_BASE_ADDRESS + 0x1738)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E0                     (PHY_TXB_IF_BASE_ADDRESS + 0x1780)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E4                     (PHY_TXB_IF_BASE_ADDRESS + 0x1790)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E5                     (PHY_TXB_IF_BASE_ADDRESS + 0x1794)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E6                     (PHY_TXB_IF_BASE_ADDRESS + 0x1798)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E7                     (PHY_TXB_IF_BASE_ADDRESS + 0x179C)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E9                     (PHY_TXB_IF_BASE_ADDRESS + 0x17A4)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1EC                     (PHY_TXB_IF_BASE_ADDRESS + 0x17B0)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1EF                     (PHY_TXB_IF_BASE_ADDRESS + 0x17BC)
#define	REG_PHY_TXB_IF_TR_TX_SWITCH_VECT                       (PHY_TXB_IF_BASE_ADDRESS + 0x182C)
#define	REG_PHY_TXB_IF_TX_MU_TRAINING_DONE                     (PHY_TXB_IF_BASE_ADDRESS + 0x1854)
#define	REG_PHY_TXB_IF_SW_GCLK_BYPASS                          (PHY_TXB_IF_BASE_ADDRESS + 0x1858)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x185C)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1860)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_4_5                (PHY_TXB_IF_BASE_ADDRESS + 0x1864)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_6_7                (PHY_TXB_IF_BASE_ADDRESS + 0x1868)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_8_9                (PHY_TXB_IF_BASE_ADDRESS + 0x186C)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1870)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1874)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_4_5                (PHY_TXB_IF_BASE_ADDRESS + 0x1878)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_6_7                (PHY_TXB_IF_BASE_ADDRESS + 0x187C)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_8_9                (PHY_TXB_IF_BASE_ADDRESS + 0x1880)
#define	REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_CORREL               (PHY_TXB_IF_BASE_ADDRESS + 0x1884)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR0_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x1888)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR0_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x188C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR1_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x1890)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR1_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x1894)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR2_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x1898)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR2_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x189C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR3_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x18A0)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR3_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x18A4)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR4_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x18A8)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR4_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x18AC)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR5_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x18B0)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR5_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x18B4)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR6_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x18B8)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR6_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x18BC)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR7_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x18C0)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR7_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x18C4)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR8_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x18C8)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR8_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x18CC)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR9_0_1                 (PHY_TXB_IF_BASE_ADDRESS + 0x18D0)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR9_2_3                 (PHY_TXB_IF_BASE_ADDRESS + 0x18D4)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR10_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x18D8)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR10_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x18DC)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR11_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x18E0)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR11_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x18E4)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR12_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x18E8)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR12_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x18EC)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR13_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x18F0)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR13_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x18F4)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR14_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x18F8)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR14_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x18FC)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR15_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1900)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR15_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1904)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR16_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1908)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR16_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x190C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR17_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1910)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR17_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1914)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR18_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1918)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR18_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x191C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR19_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1920)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR19_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1924)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR20_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1928)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR20_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x192C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR21_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1930)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR21_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1934)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR22_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1938)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR22_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x193C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR23_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1940)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR23_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1944)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR24_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1948)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR24_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x194C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR25_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1950)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR25_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1954)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR26_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1958)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR26_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x195C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR27_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1960)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR27_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1964)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR28_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1968)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR28_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x196C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR29_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1970)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR29_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1974)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR30_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1978)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR30_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x197C)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR31_0_1                (PHY_TXB_IF_BASE_ADDRESS + 0x1980)
#define	REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR31_2_3                (PHY_TXB_IF_BASE_ADDRESS + 0x1984)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C0 0x1700 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 trcN1 : 3; //TRC fix value, reset value: 0x2, access type: RW
		uint32 trcMult3 : 1; //TRC mult 3 of fix input, reset value: 0x0, access type: RW
		uint32 rrcN1Ant0 : 3; // antenna 0 RRC fix value, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 rrcN1Ant1 : 3; // antenna 1 RRC fix value, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 rrcN1Ant2 : 3; // antenna 2 RRC fix value, reset value: 0x3, access type: RW
		uint32 reserved2 : 17;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C0_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C1 0x1704 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dccStep : 5; //DC Canceler step size, reset value: 0x4, access type: RW
		uint32 reserved0 : 1;
		uint32 dccMode : 2; //DC Canceler mode, reset value: 0x0, access type: RW
		uint32 dccUpdateRate : 4; //DC Canceler update rate, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 rxFreqShiftEn : 1; //RX Frequency shift enable, reset value: 0x0, access type: RW
		uint32 rxFreqShiftMode : 1; //RX Frequency shift mode, reset value: 0x0, access type: RW
		uint32 reserved2 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C1_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C2 0x1708 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dccRef : 16; //DC Canceler reference value, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C2_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C4 0x1710 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 detectorThreshold : 7; //Detectors threshold, reset value: 0x1e, access type: RW
		uint32 reserved0 : 1;
		uint32 det11BEnBypass : 1; //11B detectors bypass enable, reset value: 0x0, access type: RW
		uint32 riscIf2BiuGclkBypassEn : 1; //grisc interface to BIU gclk bypass enable, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C4_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C5 0x1714 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNReg : 10; //SW Reset register, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C5_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C6 0x1718 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sublockEnable : 10; //Block enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C6_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG_SW_RESET_N_GEN 0x171C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swResetNGen : 10; //SW Reset generate register, reset value: 0x0, access type: RW
		uint32 ofdmTxSwResetNGen : 1; //ofdm_tx_sw_reset_n_gen, reset value: 0x0, access type: WO
		uint32 reserved0 : 21;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscRegSwResetNGen_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C8 0x1720 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 gsmResetGen44 : 4; //gsm_reset_en, reset value: 0x0, access type: RW
		uint32 gsmResetTxGen : 6; //gsm_reset_en, reset value: 0x0, access type: RW
		uint32 gsmResetRxGen : 6; //gsm_reset_en, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C8_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1C9 0x1724 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txbRiscIntEnable : 16; //Interrupt enable register, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1C9_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG_TXB_RISC_INT_STATUS 0x1728 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txbRiscIntStatusClear_txbRiscIntStatus : 16; //Interrupt status register, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscRegTxbRiscIntStatus_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1CB 0x172C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 dtRasChooseAnt : 1; //Indicate RAS to choose antenna, reset value: 0x0, access type: RW
		uint32 dtRasBlackoutEn : 1; //Indicate RAS to Enter into blackout period, reset value: 0x1, access type: RW
		uint32 dtBestEn : 1; //Enable RAS best detection., reset value: 0x0, access type: RW
		uint32 dtRasChooseAntTh : 5; //Timeout to automatically choose antenna, reset value: 0x6, access type: RW
		uint32 dtRasPreDetThr : 3; //RAS pre det samples threshold, reset value: 0x2, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Cb_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1CC 0x1730 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dtRasBlackoutPeriod : 5; //RAS blackout period , reset value: 0x15, access type: RW
		uint32 dtRasTimeoutTh : 5; //RAS timout threshold, reset value: 0xe, access type: RW
		uint32 dtRasThr2 : 3; //RAS samples threshold, reset value: 0x3, access type: RW
		uint32 dtRasSumNum : 3; //RAS sum number, reset value: 0x4, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Cc_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG_GRISC_STATE_IND 0x1738 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 griscStateInd : 2; //Set State Machine to state, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 clkCtrl11BState : 2; //Read 11B clock control state, reset value: 0x0, access type: RO
		uint32 reserved1 : 26;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscRegGriscStateInd_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E0 0x1780 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bestDtTxAnt : 1; //Best antenna for TX 11b, reset value: 0x0, access type: RO
		uint32 bestDtRxAnt : 2; //Best antenna for RX 11b, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 phyBeRxBErrorstat : 2; //Error status to MAC, reset value: 0x0, access type: RO
		uint32 reserved1 : 26;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1E0_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E4 0x1790 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dccFreqShiftEn : 1; //DC Canceler Frequency enable, reset value: 0x0, access type: RW
		uint32 dccFreqShiftMode : 1; //DC Canceler Frequency mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1E4_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E5 0x1794 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 coreEvmI : 14; //Core EVM I, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1E5_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E6 0x1798 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 coreEvmQ : 14; //Core EVM Q, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1E6_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E7 0x179C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 corePsdu : 16; //Core PSDU duration, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1E7_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E9 0x17A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hypTxGpo : 16; //Spare Registers, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1E9_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1EC 0x17B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rasFinishToRxDly : 8; //DETECT to RX state transition, reset value: 0x14, access type: RW
		uint32 rasFinishToRxDlyEn : 1; //DETECT to RX state transition, reset value: 0x1, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Ec_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1EF 0x17BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyMacIfTxState : 4; //11B PHY-MAC IF TX State, reset value: 0x0, access type: RO
		uint32 phyMacIfRxState : 4; //11B PHY-MAC IF RX State, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Ef_u;

/*REG_PHY_TXB_IF_TR_TX_SWITCH_VECT 0x182C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxAntennaOn : 4; //no description, reset value: 0x0, access type: RO
		uint32 txAntennaOn : 4; //no description, reset value: 0x0, access type: RO
		uint32 ssiAntennaOn : 4; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegPhyTxbIfTrTxSwitchVect_u;

/*REG_PHY_TXB_IF_TX_MU_TRAINING_DONE 0x1854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muTrainingDone : 1; //done pulse trigger for mu training sequence, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxbIfTxMuTrainingDone_u;

/*REG_PHY_TXB_IF_SW_GCLK_BYPASS 0x1858 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swGclkBypass : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPhyTxbIfSwGclkBypass_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_0_1 0x185C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK0 : 6; //Modem 802.11B Detectors K coefficent 0, reset value: 0x16, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffK1 : 6; //Modem 802.11B Detectors K coefficent 1, reset value: 0x9, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffK01_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_2_3 0x1860 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK2 : 6; //Modem 802.11B Detectors K coefficent 2, reset value: 0x6, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffK3 : 6; //Modem 802.11B Detectors K coefficent 3, reset value: 0x6, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffK23_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_4_5 0x1864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK4 : 6; //Modem 802.11B Detectors K coefficent 4, reset value: 0x6, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffK5 : 6; //Modem 802.11B Detectors K coefficent 5, reset value: 0x6, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffK45_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_6_7 0x1868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK6 : 6; //Modem 802.11B Detectors K coefficent 6, reset value: 0x6, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffK7 : 6; //Modem 802.11B Detectors K coefficent 7, reset value: 0x6, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffK67_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_K_8_9 0x186C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffK8 : 6; //Modem 802.11B Detectors K coefficent 8, reset value: 0x6, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffK9 : 6; //Modem 802.11B Detectors K coefficent 9, reset value: 0x6, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffK89_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_0_1 0x1870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG0 : 6; //Modem 802.11B Detectors G coefficent 0, reset value: 0x10, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffG1 : 6; //Modem 802.11B Detectors G coefficent 1, reset value: 0x7, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffG01_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_2_3 0x1874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG2 : 6; //Modem 802.11B Detectors G coefficent 2, reset value: 0x4, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffG3 : 6; //Modem 802.11B Detectors G coefficent 3, reset value: 0x4, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffG23_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_4_5 0x1878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG4 : 6; //Modem 802.11B Detectors G coefficent 4, reset value: 0x4, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffG5 : 6; //Modem 802.11B Detectors G coefficent 5, reset value: 0x4, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffG45_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_6_7 0x187C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG6 : 6; //Modem 802.11B Detectors G coefficent 6, reset value: 0x4, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffG7 : 6; //Modem 802.11B Detectors G coefficent 7, reset value: 0x4, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffG67_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_G_8_9 0x1880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffG8 : 6; //Modem 802.11B Detectors G coefficent 8, reset value: 0x4, access type: CONST
		uint32 reserved0 : 2;
		uint32 modem11BDtCoeffG9 : 6; //Modem 802.11B Detectors G coefficent 9, reset value: 0x4, access type: CONST
		uint32 reserved1 : 18;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffG89_u;

/*REG_PHY_TXB_IF_MODEM_11B_DT_COEFF_CORREL 0x1884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 modem11BDtCoeffCorrel0 : 5; //Modem 802.11B Detectors corelator coefficent 0, reset value: 0x3, access type: CONST
		uint32 reserved0 : 3;
		uint32 modem11BDtCoeffCorrel1 : 5; //Modem 802.11B Detectors corelator coefficent 1, reset value: 0x1D, access type: CONST
		uint32 reserved1 : 19;
	} bitFields;
} RegPhyTxbIfModem11BDtCoeffCorrel_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR0_0_1 0x1888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr00 : 7; //tx_11b_lut_iq_addr0_0, reset value: 0x77, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr01 : 7; //tx_11b_lut_iq_addr0_1, reset value: 0x77, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr001_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR0_2_3 0x188C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr02 : 7; //tx_11b_lut_iq_addr0_2, reset value: 0x77, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr03 : 7; //tx_11b_lut_iq_addr0_3, reset value: 0x77, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr023_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR1_0_1 0x1890 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr10 : 7; //tx_11b_lut_iq_addr1_0, reset value: 0x78, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr11 : 7; //tx_11b_lut_iq_addr1_1, reset value: 0x79, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr101_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR1_2_3 0x1894 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr12 : 7; //tx_11b_lut_iq_addr1_2, reset value: 0x7A, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr13 : 7; //tx_11b_lut_iq_addr1_3, reset value: 0x7B, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr123_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR2_0_1 0x1898 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr20 : 7; //tx_11b_lut_iq_addr2_0, reset value: 0x76, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr21 : 7; //tx_11b_lut_iq_addr2_1, reset value: 0x6A, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr201_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR2_2_3 0x189C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr22 : 7; //tx_11b_lut_iq_addr2_2, reset value: 0x55, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr23 : 7; //tx_11b_lut_iq_addr2_3, reset value: 0x3C, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr223_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR3_0_1 0x18A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr30 : 7; //tx_11b_lut_iq_addr3_0, reset value: 0x77, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr31 : 7; //tx_11b_lut_iq_addr3_1, reset value: 0x4B, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr301_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR3_2_3 0x18A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr32 : 7; //tx_11b_lut_iq_addr3_2, reset value: 0x7D, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr33 : 7; //tx_11b_lut_iq_addr3_3, reset value: 0x3, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr323_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR4_0_1 0x18A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr40 : 7; //tx_11b_lut_iq_addr4_0, reset value: 0x27, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr41 : 7; //tx_11b_lut_iq_addr4_1, reset value: 0x1E, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr401_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR4_2_3 0x18AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr42 : 7; //tx_11b_lut_iq_addr4_2, reset value: 0x27, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr43 : 7; //tx_11b_lut_iq_addr4_3, reset value: 0x3C, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr423_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR5_0_1 0x18B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr50 : 7; //tx_11b_lut_iq_addr5_0, reset value: 0x27, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr51 : 7; //tx_11b_lut_iq_addr5_1, reset value: 0x20, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr501_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR5_2_3 0x18B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr52 : 7; //tx_11b_lut_iq_addr5_2, reset value: 0x29, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr53 : 7; //tx_11b_lut_iq_addr5_3, reset value: 0x3F, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr523_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR6_0_1 0x18B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr60 : 7; //tx_11b_lut_iq_addr6_0, reset value: 0x26, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr61 : 7; //tx_11b_lut_iq_addr6_1, reset value: 0x11, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr601_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR6_2_3 0x18BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr62 : 7; //tx_11b_lut_iq_addr6_2, reset value: 0x4, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr63 : 7; //tx_11b_lut_iq_addr6_3, reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr623_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR7_0_1 0x18C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr70 : 7; //tx_11b_lut_iq_addr7_0, reset value: 0x26, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr71 : 7; //tx_11b_lut_iq_addr7_1, reset value: 0x13, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr701_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR7_2_3 0x18C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr72 : 7; //tx_11b_lut_iq_addr7_2, reset value: 0x7, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr73 : 7; //tx_11b_lut_iq_addr7_3, reset value: 0x3, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr723_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR8_0_1 0x18C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr80 : 7; //tx_11b_lut_iq_addr8_0, reset value: 0x55, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr81 : 7; //tx_11b_lut_iq_addr8_1, reset value: 0x6a, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr801_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR8_2_3 0x18CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr82 : 7; //tx_11b_lut_iq_addr8_2, reset value: 0x76, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr83 : 7; //tx_11b_lut_iq_addr8_3, reset value: 0x7b, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr823_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR9_0_1 0x18D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr90 : 7; //tx_11b_lut_iq_addr9_0, reset value: 0x55, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr91 : 7; //tx_11b_lut_iq_addr9_1, reset value: 0x6b, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr901_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR9_2_3 0x18D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr92 : 7; //tx_11b_lut_iq_addr9_2, reset value: 0x79, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr93 : 7; //tx_11b_lut_iq_addr9_3, reset value: 0x7E, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr923_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR10_0_1 0x18D8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr100 : 7; //tx_11b_lut_iq_addr10_0, reset value: 0x54, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr101 : 7; //tx_11b_lut_iq_addr10_1, reset value: 0x5D, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1001_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR10_2_3 0x18DC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr102 : 7; //tx_11b_lut_iq_addr10_2, reset value: 0x54, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr103 : 7; //tx_11b_lut_iq_addr10_3, reset value: 0x3F, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1023_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR11_0_1 0x18E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr110 : 7; //tx_11b_lut_iq_addr11_0, reset value: 0x55, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr111 : 7; //tx_11b_lut_iq_addr11_1, reset value: 0x5E, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1101_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR11_2_3 0x18E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr112 : 7; //tx_11b_lut_iq_addr11_2, reset value: 0x57, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr113 : 7; //tx_11b_lut_iq_addr11_3, reset value: 0x42, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1123_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR12_0_1 0x18E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr120 : 7; //tx_11b_lut_iq_addr12_0, reset value: 0x4, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr121 : 7; //tx_11b_lut_iq_addr12_1, reset value: 0x11, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1201_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR12_2_3 0x18EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr122 : 7; //tx_11b_lut_iq_addr12_2, reset value: 0x26, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr123 : 7; //tx_11b_lut_iq_addr12_3, reset value: 0x3F, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1223_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR13_0_1 0x18F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr130 : 7; //tx_11b_lut_iq_addr13_0, reset value: 0x5, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr131 : 7; //tx_11b_lut_iq_addr13_1, reset value: 0x13, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1301_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR13_2_3 0x18F4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr132 : 7; //tx_11b_lut_iq_addr13_2, reset value: 0x29, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr133 : 7; //tx_11b_lut_iq_addr13_3, reset value: 0x42, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1323_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR14_0_1 0x18F8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr140 : 7; //tx_11b_lut_iq_addr14_0, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr141 : 7; //tx_11b_lut_iq_addr14_1, reset value: 0x4, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1401_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR14_2_3 0x18FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr142 : 7; //tx_11b_lut_iq_addr14_2, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr143 : 7; //tx_11b_lut_iq_addr14_3, reset value: 0x3, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1423_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR15_0_1 0x1900 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr150 : 7; //tx_11b_lut_iq_addr15_0, reset value: 0x4, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr151 : 7; //tx_11b_lut_iq_addr15_1, reset value: 0x5, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1501_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR15_2_3 0x1904 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr152 : 7; //tx_11b_lut_iq_addr15_2, reset value: 0x6, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr153 : 7; //tx_11b_lut_iq_addr15_3, reset value: 0x7, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1523_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR16_0_1 0x1908 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr160 : 7; //tx_11b_lut_iq_addr16_0, reset value: 0x7A, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr161 : 7; //tx_11b_lut_iq_addr16_1, reset value: 0x79, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1601_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR16_2_3 0x190C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr162 : 7; //tx_11b_lut_iq_addr16_2, reset value: 0x78, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr163 : 7; //tx_11b_lut_iq_addr16_3, reset value: 0x77, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1623_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR17_0_1 0x1910 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr170 : 7; //tx_11b_lut_iq_addr17_0, reset value: 0x7B, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr171 : 7; //tx_11b_lut_iq_addr17_1, reset value: 0x7A, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1701_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR17_2_3 0x1914 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr172 : 7; //tx_11b_lut_iq_addr17_2, reset value: 0x7B, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr173 : 7; //tx_11b_lut_iq_addr17_3, reset value: 0x7B, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1723_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR18_0_1 0x1918 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr180 : 7; //tx_11b_lut_iq_addr18_0, reset value: 0x79, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr181 : 7; //tx_11b_lut_iq_addr18_1, reset value: 0x6b, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1801_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR18_2_3 0x191C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr182 : 7; //tx_11b_lut_iq_addr18_2, reset value: 0x55, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr183 : 7; //tx_11b_lut_iq_addr18_3, reset value: 0x3C, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1823_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR19_0_1 0x1920 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr190 : 7; //tx_11b_lut_iq_addr19_0, reset value: 0x7A, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr191 : 7; //tx_11b_lut_iq_addr19_1, reset value: 0x6D, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1901_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR19_2_3 0x1924 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr192 : 7; //tx_11b_lut_iq_addr19_2, reset value: 0x58, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr193 : 7; //tx_11b_lut_iq_addr19_3, reset value: 0x3F, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr1923_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR20_0_1 0x1928 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr200 : 7; //tx_11b_lut_iq_addr20_0, reset value: 0x29, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr201 : 7; //tx_11b_lut_iq_addr20_1, reset value: 0x20, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2001_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR20_2_3 0x192C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr202 : 7; //tx_11b_lut_iq_addr20_2, reset value: 0x27, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr203 : 7; //tx_11b_lut_iq_addr20_3, reset value: 0x3C, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2023_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR21_0_1 0x1930 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr210 : 7; //tx_11b_lut_iq_addr21_0, reset value: 0x2A, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr211 : 7; //tx_11b_lut_iq_addr21_1, reset value: 0x21, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2101_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR21_2_3 0x1934 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr212 : 7; //tx_11b_lut_iq_addr21_2, reset value: 0x2A, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr213 : 7; //tx_11b_lut_iq_addr21_3, reset value: 0x3F, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2123_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR22_0_1 0x1938 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr220 : 7; //tx_11b_lut_iq_addr22_0, reset value: 0x29, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr221 : 7; //tx_11b_lut_iq_addr22_1, reset value: 0x13, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2201_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR22_2_3 0x193C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr222 : 7; //tx_11b_lut_iq_addr22_2, reset value: 0x5, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr223 : 7; //tx_11b_lut_iq_addr22_3, reset value: 0x0, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2223_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR23_0_1 0x1940 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr230 : 7; //tx_11b_lut_iq_addr23_0, reset value: 0x29, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr231 : 7; //tx_11b_lut_iq_addr23_1, reset value: 0x14, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2301_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR23_2_3 0x1944 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr232 : 7; //tx_11b_lut_iq_addr23_2, reset value: 0x8, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr233 : 7; //tx_11b_lut_iq_addr23_3, reset value: 0x3, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2323_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR24_0_1 0x1948 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr240 : 7; //tx_11b_lut_iq_addr24_0, reset value: 0x58, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr241 : 7; //tx_11b_lut_iq_addr24_1, reset value: 0x6B, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2401_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR24_2_3 0x194C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr242 : 7; //tx_11b_lut_iq_addr24_2, reset value: 0x77, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr243 : 7; //tx_11b_lut_iq_addr24_3, reset value: 0x7B, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2423_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR25_0_1 0x1950 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr250 : 7; //tx_11b_lut_iq_addr25_0, reset value: 0x58, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr251 : 7; //tx_11b_lut_iq_addr25_1, reset value: 0x6D, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2501_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR25_2_3 0x1954 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr252 : 7; //tx_11b_lut_iq_addr25_2, reset value: 0x7A, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr253 : 7; //tx_11b_lut_iq_addr25_3, reset value: 0x7E, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2523_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR26_0_1 0x1958 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr260 : 7; //tx_11b_lut_iq_addr26_0, reset value: 0x57, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr261 : 7; //tx_11b_lut_iq_addr26_1, reset value: 0x5E, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2601_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR26_2_3 0x195C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr262 : 7; //tx_11b_lut_iq_addr26_2, reset value: 0x55, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr263 : 7; //tx_11b_lut_iq_addr26_3, reset value: 0x3F, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2623_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR27_0_1 0x1960 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr270 : 7; //tx_11b_lut_iq_addr27_0, reset value: 0x57, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr271 : 7; //tx_11b_lut_iq_addr27_1, reset value: 0x60, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2701_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR27_2_3 0x1964 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr272 : 7; //tx_11b_lut_iq_addr27_2, reset value: 0x57, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr273 : 7; //tx_11b_lut_iq_addr27_3, reset value: 0x42, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2723_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR28_0_1 0x1968 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr280 : 7; //tx_11b_lut_iq_addr28_0, reset value: 0x7, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr281 : 7; //tx_11b_lut_iq_addr28_1, reset value: 0x13, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2801_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR28_2_3 0x196C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr282 : 7; //tx_11b_lut_iq_addr28_2, reset value: 0x26, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr283 : 7; //tx_11b_lut_iq_addr28_3, reset value: 0x3F, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2823_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR29_0_1 0x1970 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr290 : 7; //tx_11b_lut_iq_addr29_0, reset value: 0x8, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr291 : 7; //tx_11b_lut_iq_addr29_1, reset value: 0x14, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2901_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR29_2_3 0x1974 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr292 : 7; //tx_11b_lut_iq_addr29_2, reset value: 0x29, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr293 : 7; //tx_11b_lut_iq_addr29_3, reset value: 0x42, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr2923_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR30_0_1 0x1978 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr300 : 7; //tx_11b_lut_iq_addr30_0, reset value: 0x6, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr301 : 7; //tx_11b_lut_iq_addr30_1, reset value: 0x5, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr3001_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR30_2_3 0x197C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr302 : 7; //tx_11b_lut_iq_addr30_2, reset value: 0x4, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr303 : 7; //tx_11b_lut_iq_addr30_3, reset value: 0x3, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr3023_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR31_0_1 0x1980 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr310 : 7; //tx_11b_lut_iq_addr31_0, reset value: 0x7, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr311 : 7; //tx_11b_lut_iq_addr31_1, reset value: 0x7, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr3101_u;

/*REG_PHY_TXB_IF_TX_11B_LUT_IQ_ADDR31_2_3 0x1984 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tx11BLutIqAddr312 : 7; //tx_11b_lut_iq_addr31_2, reset value: 0x7, access type: RW
		uint32 reserved0 : 1;
		uint32 tx11BLutIqAddr313 : 7; //tx_11b_lut_iq_addr31_3, reset value: 0x7, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegPhyTxbIfTx11BLutIqAddr3123_u;



#endif // _PHY_TXB_IF_REGS_H_
