
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
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1D9                     (PHY_TXB_IF_BASE_ADDRESS + 0x1764)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DA                     (PHY_TXB_IF_BASE_ADDRESS + 0x1768)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DB                     (PHY_TXB_IF_BASE_ADDRESS + 0x176C)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DC                     (PHY_TXB_IF_BASE_ADDRESS + 0x1770)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DD                     (PHY_TXB_IF_BASE_ADDRESS + 0x1774)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DE                     (PHY_TXB_IF_BASE_ADDRESS + 0x1778)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E0                     (PHY_TXB_IF_BASE_ADDRESS + 0x1780)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E4                     (PHY_TXB_IF_BASE_ADDRESS + 0x1790)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E5                     (PHY_TXB_IF_BASE_ADDRESS + 0x1794)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E6                     (PHY_TXB_IF_BASE_ADDRESS + 0x1798)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E7                     (PHY_TXB_IF_BASE_ADDRESS + 0x179C)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1E9                     (PHY_TXB_IF_BASE_ADDRESS + 0x17A4)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1EC                     (PHY_TXB_IF_BASE_ADDRESS + 0x17B0)
#define	REG_PHY_TXB_IF_PHY_TXB_RISC_REG1EF                     (PHY_TXB_IF_BASE_ADDRESS + 0x17BC)
#define	REG_PHY_TXB_IF_TX_ERROR_INDICATION                     (PHY_TXB_IF_BASE_ADDRESS + 0x17E8)
#define	REG_PHY_TXB_IF_TR_TX_SWITCH_VECT                       (PHY_TXB_IF_BASE_ADDRESS + 0x182C)
#define	REG_PHY_TXB_IF_TX_DATA_ERROR                           (PHY_TXB_IF_BASE_ADDRESS + 0x1830)
#define	REG_PHY_TXB_IF_TX_MU_ERROR_INDICATION                  (PHY_TXB_IF_BASE_ADDRESS + 0x1850)
#define	REG_PHY_TXB_IF_TX_MU_TRAINING_DONE                     (PHY_TXB_IF_BASE_ADDRESS + 0x1854)
#define	REG_PHY_TXB_IF_SW_GCLK_BYPASS                          (PHY_TXB_IF_BASE_ADDRESS + 0x1858)
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

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1D9 0x1764 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 bfHeaderValid : 1; //bf_header_valid, reset value: 0x0, access type: RO
		uint32 reserved1 : 27;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1D9_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DA 0x1768 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfHeaderReportLength : 9; //bf_header_report_length, reset value: 0x0, access type: RO
		uint32 bfHeaderSounding : 1; //bf_header_sounding, reset value: 0x0, access type: RO
		uint32 bfHeaderImplicit : 1; //bf_header_implicit, reset value: 0x0, access type: RO
		uint32 bfHeaderSpare : 4; //bf_header_spare, reset value: 0x0, access type: RO
		uint32 bfHeaderVht : 1; //bf_header_vht, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Da_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DB 0x176C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mimoCtrlNc : 3; //mimo_ctrl_nc, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 mimoCtrlNr : 3; //mimo_ctrl_nr, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 mimoCtrlChannelWidth : 2; //mimo_ctrl_channel_width, reset value: 0x0, access type: RO
		uint32 reserved2 : 2;
		uint32 mimoCtrlGrouping : 2; //mimo_ctrl_grouping, reset value: 0x0, access type: RO
		uint32 reserved3 : 18;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Db_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DC 0x1770 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mimoCtrlCoeffSize : 2; //mimo_ctrl_coeff_size, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 mimoCtrlCodebook : 2; //mimo_ctrl_codebook, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 mimoCtrlReserve : 4; //mimo_ctrl_reserve, reset value: 0x0, access type: RO
		uint32 reserved2 : 20;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Dc_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DD 0x1774 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfMeanSnrSs0 : 8; //bf_mean_snr_ss0, reset value: 0x0, access type: RO
		uint32 bfMeanSnrSs1 : 8; //bf_mean_snr_ss1, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1Dd_u;

/*REG_PHY_TXB_IF_PHY_TXB_RISC_REG1DE 0x1778 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfMeanSnrSs2 : 8; //bf_mean_snr_ss2, reset value: 0x0, access type: RO
		uint32 bfMeanSnrSs3 : 8; //bf_mean_snr_ss3, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfPhyTxbRiscReg1De_u;

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

/*REG_PHY_TXB_IF_TX_ERROR_INDICATION 0x17E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txTcrError : 4; //tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4, reset value: 0x0, access type: RO
		uint32 txBfError : 9; //bit 0 - packet is stbc + bf moving to flat. , bit 1 - time out during bf heaader. , bit 2 - time out during bf body. , bit 3 - bf header length is 0. , bit 4 -  implicit and report_nr < number of active antennas , bit 5 -  bf_report_nc<tx_nss;  , bit 6 - report BW is less than TCR BW , bit 7 - report BW is more than actual TX BW , bit 8 - report BW is 80Mhz and packet type is legacy, reset value: 0x0, access type: RO
		uint32 txAirTimeError : 1; //tx_air_time_error, reset value: 0x0, access type: RO
		uint32 txAntennaSelectionError : 2; //bit 13 - active antennas are less than the number of spatial streams. , bit 14 - number of prog model antennas are less than active antennas., reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfTxErrorIndication_u;

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

/*REG_PHY_TXB_IF_TX_DATA_ERROR 0x1830 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDataError : 4; //valid is low during data state of the tx phy mac, reset value: 0x0, access type: RO
		uint32 txBfErrorFull : 10; //bit 0 - packet is stbc + bf moving to flat. , bit 1 - time out during bf heaader. , bit 2 - time out during bf body. , bit 3 - bf header length is 0. , bit 4 -  implicit and report_nr < number of active antennas , bit 5 -  bf_report_nc<tx_nss;  , bit 6 - report BW is less than TCR BW , bit 7 - report BW is more than actual TX BW , bit 8 - report BW is 80Mhz and packet type is legacy , bit 9 - bf_type is bf in tx ndp, reset value: 0x0, access type: RO
		uint32 reserved0 : 18;
	} bitFields;
} RegPhyTxbIfTxDataError_u;

/*REG_PHY_TXB_IF_TX_MU_ERROR_INDICATION 0x1850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 u1TxTcrError : 4; //user1: tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4, reset value: 0x0, access type: RO
		uint32 u2TxTcrError : 4; //user2: tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4, reset value: 0x0, access type: RO
		uint32 u3TxTcrError : 4; //user3: tx_tcr_error[0] - tcr0 , tx_tcr_error[1] - tcr1+2 , tx_tcr_error[2] - tcr3 , tx_tcr_error[3] - tcr4, reset value: 0x0, access type: RO
		uint32 txMuTcrError : 4; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPhyTxbIfTxMuErrorIndication_u;

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



#endif // _PHY_TXB_IF_REGS_H_
