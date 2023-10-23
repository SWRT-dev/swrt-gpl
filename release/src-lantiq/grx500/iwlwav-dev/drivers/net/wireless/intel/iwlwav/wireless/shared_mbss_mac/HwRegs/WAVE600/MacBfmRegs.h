
/***********************************************************************************
File:				MacBfmRegs.h
Module:				MacBfm
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_BFM_REGS_H_
#define _MAC_BFM_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_BFM_BASE_ADDRESS                             MEMORY_MAP_UNIT_11_BASE_ADDRESS
#define	REG_MAC_BFM_MBFM_GLOBAL_EN                 (MAC_BFM_BASE_ADDRESS + 0x4)
#define	REG_MAC_BFM_MBFM_CTRL1                     (MAC_BFM_BASE_ADDRESS + 0x8)
#define	REG_MAC_BFM_MBFM_TSF_LATCH                 (MAC_BFM_BASE_ADDRESS + 0xC)
#define	REG_MAC_BFM_MBFM_CFG_HE_ACT                (MAC_BFM_BASE_ADDRESS + 0x20)
#define	REG_MAC_BFM_MBFM_CFG_DB_UPDATE             (MAC_BFM_BASE_ADDRESS + 0x24)
#define	REG_MAC_BFM_MBFM_CFG_PER_BFR               (MAC_BFM_BASE_ADDRESS + 0x28)
#define	REG_MAC_BFM_MBFM_CFG_OT                    (MAC_BFM_BASE_ADDRESS + 0x2C)
#define	REG_MAC_BFM_MBFM_CFG_HE_RU_END             (MAC_BFM_BASE_ADDRESS + 0x30)
#define	REG_MAC_BFM_MBFM_ISR_CTRL                  (MAC_BFM_BASE_ADDRESS + 0x34)
#define	REG_MAC_BFM_MBFM_EXPECTED_ORIG_USERS0      (MAC_BFM_BASE_ADDRESS + 0x38)
#define	REG_MAC_BFM_MBFM_EXPECTED_ORIG_USERS1      (MAC_BFM_BASE_ADDRESS + 0x3C)
#define	REG_MAC_BFM_MBFM_EXPECTED_USERS0           (MAC_BFM_BASE_ADDRESS + 0x40)
#define	REG_MAC_BFM_MBFM_EXPECTED_USERS1           (MAC_BFM_BASE_ADDRESS + 0x44)
#define	REG_MAC_BFM_MBFM_AUTH_VALID_USERS0         (MAC_BFM_BASE_ADDRESS + 0x48)
#define	REG_MAC_BFM_MBFM_AUTH_VALID_USERS1         (MAC_BFM_BASE_ADDRESS + 0x4C)
#define	REG_MAC_BFM_MBFM_AUTH_CHECKED_USERS0       (MAC_BFM_BASE_ADDRESS + 0x50)
#define	REG_MAC_BFM_MBFM_AUTH_CHECKED_USERS1       (MAC_BFM_BASE_ADDRESS + 0x54)
#define	REG_MAC_BFM_MBFM_IN_PROCESS_USERS0         (MAC_BFM_BASE_ADDRESS + 0x58)
#define	REG_MAC_BFM_MBFM_IN_PROCESS_USERS1         (MAC_BFM_BASE_ADDRESS + 0x5C)
#define	REG_MAC_BFM_MBFM_MIMO_CHECKED_USERS0       (MAC_BFM_BASE_ADDRESS + 0x60)
#define	REG_MAC_BFM_MBFM_MIMO_CHECKED_USERS1       (MAC_BFM_BASE_ADDRESS + 0x64)
#define	REG_MAC_BFM_MBFM_MIMO_VALID_USERS0         (MAC_BFM_BASE_ADDRESS + 0x68)
#define	REG_MAC_BFM_MBFM_MIMO_VALID_USERS1         (MAC_BFM_BASE_ADDRESS + 0x6C)
#define	REG_MAC_BFM_MBFM_FINAL_ENDED_USERS0        (MAC_BFM_BASE_ADDRESS + 0x70)
#define	REG_MAC_BFM_MBFM_FINAL_ENDED_USERS1        (MAC_BFM_BASE_ADDRESS + 0x74)
#define	REG_MAC_BFM_MBFM_SU_PASSED_USERS0          (MAC_BFM_BASE_ADDRESS + 0x78)
#define	REG_MAC_BFM_MBFM_SU_PASSED_USERS1          (MAC_BFM_BASE_ADDRESS + 0x7C)
#define	REG_MAC_BFM_MBFM_USERS0_STATIC_IND         (MAC_BFM_BASE_ADDRESS + 0x80)
#define	REG_MAC_BFM_MBFM_CONCLUSION_REG            (MAC_BFM_BASE_ADDRESS + 0x84)
#define	REG_MAC_BFM_MBFM_FIFO_BASE_ADDR            (MAC_BFM_BASE_ADDRESS + 0xA0)
#define	REG_MAC_BFM_MBFM_FIFO_DEPTH_MINUS_ONE      (MAC_BFM_BASE_ADDRESS + 0xA4)
#define	REG_MAC_BFM_MBFM_FIFO_CLEAR_STRB           (MAC_BFM_BASE_ADDRESS + 0xA8)
#define	REG_MAC_BFM_MBFM_FIFO_RD_ENTRIES_NUM       (MAC_BFM_BASE_ADDRESS + 0xAC)
#define	REG_MAC_BFM_MBFM_FIFO_NUM_ENTRIES_COUNT    (MAC_BFM_BASE_ADDRESS + 0xB0)
#define	REG_MAC_BFM_MBFM_FIFO_DEBUG                (MAC_BFM_BASE_ADDRESS + 0xB4)
#define	REG_MAC_BFM_MBFM_FIFO_CFG                  (MAC_BFM_BASE_ADDRESS + 0xC0)
#define	REG_MAC_BFM_MBFM_RO_ERRORS                 (MAC_BFM_BASE_ADDRESS + 0xC4)
#define	REG_MAC_BFM_MBFM_LOGGER_CTRL               (MAC_BFM_BASE_ADDRESS + 0xF0)
#define	REG_MAC_BFM_MBFM_LOGGER_CFG                (MAC_BFM_BASE_ADDRESS + 0xF4)
#define	REG_MAC_BFM_MBFM_CLR_OVERFLOW              (MAC_BFM_BASE_ADDRESS + 0x100)
#define	REG_MAC_BFM_MBFM_SPARE_REG1                (MAC_BFM_BASE_ADDRESS + 0x1C0)
#define	REG_MAC_BFM_MBFM_SPARE_REG2                (MAC_BFM_BASE_ADDRESS + 0x1C4)
#define	REG_MAC_BFM_MBFM_SPARE_REG3                (MAC_BFM_BASE_ADDRESS + 0x1C8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_BFM_MBFM_GLOBAL_EN 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfGlobalEn : 1; //Global enable for MBFM. , Software should clear this bit while configuring the MBFM for the upcoming BFRs. , After configuring the MBFM set this bit. , 1'b0 - MBFM is off , 1'b1 - MBFM is on, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacBfmMbfmGlobalEn_u;

/*REG_MAC_BFM_MBFM_CTRL1 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfGlobalSmOut : 4; //Read current global state machine state, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
		uint32 swClrStaticIndPulse : 1; //write 1 to this bit (self cleared) to clear all external static indications., reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 swClrExpectedBfrPulse : 1; //write 1 to this bit (self cleared) to clear all expected BFR user vector during sounding initialization, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 swForceGoToIdlePulse : 1; //force state machine to go to idle. Use this only in case the MBFM is stuck, might cause hazards., reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegMacBfmMbfmCtrl1_u;

/*REG_MAC_BFM_MBFM_TSF_LATCH 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLatchTsfPulse : 1; //Write 1'b1 to this field when either NDPA/NDP for the sounding sequence occurs. This causes the MBFM to latch the global TSF time in order to use it on the upcoming BFRs., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacBfmMbfmTsfLatch_u;

/*REG_MAC_BFM_MBFM_CFG_HE_ACT 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swHeActionField : 8; //802.11 spec v 1.1 doesn't specify the required value for the HE mode BFR Action field. This register is a one time configuration to match this field when spec has this value, reset value: 0x1E, access type: RW
		uint32 swForceHeActionField : 1; //When this bit is cleared (1'b0) the MAC checks the Action field in MPDU to match for HT/VHT/HE. , When this bit is set the MAC matches the action field to be: , 7 - HT , 21-VHT , Any other value - HE. , , reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegMacBfmMbfmCfgHeAct_u;

/*REG_MAC_BFM_MBFM_CFG_DB_UPDATE 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMuUpdateSu : 1; //whether to update the SU database for a station in addition to the MU in case of MU BFR for this station , 1'b1 - update the SU , 1'b0 - don't update the SU, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 swMuUpdateMu : 1; //whether to update the MU database for a station in case of MU BFR for this station , 1'b1 - update the MU , 1'b0 - don't update the MU, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 swHeMuUpdateCqi : 1; //whether to update the CQI database for a station in addition to other DBs in case of MU BFR for this station , 1'b1 - update the CQI , 1'b0 - don't update the CQI, reset value: 0x1, access type: RW
		uint32 reserved2 : 23;
	} bitFields;
} RegMacBfmMbfmCfgDbUpdate_u;

/*REG_MAC_BFM_MBFM_CFG_PER_BFR 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swGlbSoundingMode : 1; //Sender configures the MBFM to expect the following on the upcoming BFR: , 0 - NON TB , 1 - TB, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 swGlbVhtTrainingInd : 1; //Sender configures the MBFM to expect the following BFR is for VHT training only , 1'b1 - VHT training only , 1'b0 - regular BFR, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 swGlbBw : 2; //Sender configures the MBFM to expect the following on the upcoming BFR: , 0 – 20MHz , 1 – 40MHz , 2 – 80MHz , 3 – 160MHz, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 swGlbNr : 2; //Sender configures the MBFM to expect the following on the upcoming BFR: , 0 for NSS = 1 , 1 for NSS = 2 , 2 for NSS = 3 , 3 for NSS = 4, reset value: 0x2, access type: RW
		uint32 reserved3 : 2;
		uint32 swEnableMcsFeedbackUpdate : 1; //Enable the MCS feedback update in the STA DB , 1'b1 - enable MBFM to update the MCS feedback field , 1'b0 - disable MBFM from update the MCS feedback field, reset value: 0x1, access type: RW
		uint32 reserved4 : 3;
		uint32 swExpectedPhyMode : 2; //Sender configures MBFM to expect the following phy_mode in the coming BFR , MODE_HT   : 2'b00 , MODE_VHT  : 2'b10 , MODE_HE   : 2'b11, reset value: 0x0, access type: RW
		uint32 reserved5 : 10;
	} bitFields;
} RegMacBfmMbfmCfgPerBfr_u;

/*REG_MAC_BFM_MBFM_CFG_OT 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swCheckFeedbackSegments : 1; //Software configures whether to verify that feedback segments fields are 0 as part of MIMO control validation , 1'b1 - verify the feedback segments fields  , 1'b0 - ignore the feedback segments fields , reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 swCheckHeRuEnd : 1; //Software configures whether to verify the RU END field for HE BFRs as part of MIMO control validation , 1'b1 - verify the RU END field  , 1'b0 - ignore the RU END field , reset value: 0x1, access type: RW
		uint32 swCheckHeRuStart : 1; //Software configures whether to verify the RU START field for HE BFRs as part of MIMO control validation , 1'b1 - verify the RU START field is 0 , 1'b0 - ignore the RU START field , reset value: 0x1, access type: RW
		uint32 swCheckBwInHt : 1; //Software configures whether to verify the exact BW in HT mode as part of MIMO control validation , 1'b1 - verify the BW field  , 1'b0 - ignore the BW field, reset value: 0x1, access type: RW
		uint32 swCheckBw : 1; //Software configures whether to verify the exact BW for all phy modes as part of MIMO control validation , 1'b1 - verify the BW field  , 1'b0 - ignore the BW field, reset value: 0x1, access type: RW
		uint32 swCheckSoundingModeHe : 1; //whether to verify that each MIMO control in TB response is indeed HE format , 1 - check this match , 0 - Ignore checking and allow also VHT/HT formats, reset value: 0x1, access type: RW
		uint32 swLengthCheckEn : 1; //whether to check BFR MPDU length calculated by RXC (from delimiter) to the MPDU length calculated by the MBFM according to mimo control fields.  , Software must modify this bit prior to setting sw_clr_expected_bfr_pulse , 1'b1 - check length , 1'b0 - don't check length., reset value: 0x1, access type: RW
		uint32 swCheckPhyMode : 1; //Software configures whether to verify the exact phy mode as part of MIMO control validation , 1'b1 - verify the PHY mode field  , 1'b0 - ignore the PHY mode field, and use Action + Category to decide the phy mode, reset value: 0x1, access type: RW
		uint32 swCheckNgHeMu : 1; //Software configures whether to verify the exact value of received Ng in HE MU report as part of MIMO control validation , 1'b1 - verify the Ng field  , 1'b0 - ignore the Ng field check, reset value: 0x1, access type: RW
		uint32 swCheckCodebookHeMu : 1; //Software configures whether to verify the exact value of received codebook in HE MU report as part of MIMO control validation , 1'b1 - verify the codebook field  , 1'b0 - ignore the codebook field check, reset value: 0x1, access type: RW
		uint32 swCheckFeedbackType : 1; //This field manipulates the feedback type field matching as part of MIMO control check , 1'b1 - validate exact match , 1'b0 - allow MBFM to parse SU report in case of requested MU report. This SU report will only update the SU database and not MU DB., reset value: 0x1, access type: RW
		uint32 swMuBfrWithoutDSnr : 1; //802.11 spec expects Delta SNR after the phases stage. Write 1'b0 to this field to support MU report with no delta SNR., reset value: 0x0, access type: RW
		uint32 swHeSuBfrWithDSnr : 1; //802.11 spec HE from ver 1.2 and above expects Delta SNR after the phases stage in HE SU. Write 1'b1 to this field to support length check for HE SU report with delta SNR., reset value: 0x1, access type: RW
		uint32 swLimitPipeFlowControl : 2; //Flow control on the pipe throughput. , Value of 2 is the calculated optimum. , use 3 in order to fasten the pipe process time (but this can cause bugs in BFR process) , use 1 in order to slow the pipe, this doesn't cause any bug but will surely reduce throughput and RXC will be chocked. , 2'b00 - unused , 2'b01 - MBFM process slowly , 2'b10 - Optimum configuration , 2'b11 - MBFM process fast with bugs, reset value: 0x2, access type: RW
		uint32 reserved1 : 2;
		uint32 swAuthCheckEn : 1; //whether to check ADDR1 & ADDR2 in the MPDU as part of the BFR validity checks. This is a global configuration for all the BFRs processed by the MBFM.  , Software must modify this bit prior to setting sw_clr_expected_bfr_pulse , 1'b1 - check those fields , 1'b0 - don't check those fields., reset value: 0x1, access type: RW
		uint32 reserved2 : 3;
		uint32 swShiftRegTimeout : 5; //timeout for shift register. The maximal amount of cycles for a new data from RXC to be in the shift register until it is replaced by a new one., reset value: 0x15, access type: RW
		uint32 reserved3 : 1;
		uint32 swEnableStaDbUpdate : 1; //whether to update the STA DB at the end of each user's BFR update. , This bit should be cleared only while debugging the module in case of low throughput to the shared RAM., reset value: 0x1, access type: RW
		uint32 reserved4 : 1;
	} bitFields;
} RegMacBfmMbfmCfgOt_u;

/*REG_MAC_BFM_MBFM_CFG_HE_RU_END 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swHeRuEnd20Mhz : 7; //RU END field value to match during HE MIMO control validation for BW=20MHz, reset value: 0x8, access type: RW
		uint32 reserved0 : 1;
		uint32 swHeRuEnd40Mhz : 7; //RU END field value to match during HE MIMO control validation for BW=40MHz, reset value: 0x11, access type: RW
		uint32 reserved1 : 1;
		uint32 swHeRuEnd80Mhz : 7; //RU END field value to match during HE MIMO control validation for BW=80MHz, reset value: 0x24, access type: RW
		uint32 reserved2 : 1;
		uint32 swHeRuEnd160Mhz : 7; //RU END field value to match during HE MIMO control validation for BW=160MHz, reset value: 0x49, access type: RW
		uint32 reserved3 : 1;
	} bitFields;
} RegMacBfmMbfmCfgHeRuEnd_u;

/*REG_MAC_BFM_MBFM_ISR_CTRL 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mbfmGlobalErrIrq : 1; //Error Interrupt from MBFM, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 mbfmGlobalErrIrqClr : 1; //Clear the IRQ. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 reserved1 : 23;
	} bitFields;
} RegMacBfmMbfmIsrCtrl_u;

/*REG_MAC_BFM_MBFM_EXPECTED_ORIG_USERS0 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swExpectBfrCfgOrig0 : 32; //Read the first 32 users status - whether sender had configured MBFM to expect a BFR from this user, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmExpectedOrigUsers0_u;

/*REG_MAC_BFM_MBFM_EXPECTED_ORIG_USERS1 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swExpectBfrCfgOrig1 : 4; //Read the last 4 users status - whether sender had configured MBFM to expect a BFR from this user, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmExpectedOrigUsers1_u;

/*REG_MAC_BFM_MBFM_EXPECTED_USERS0 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swExpectBfrCfg0 : 32; //Read the first 32 users status - whether sender had configured MBFM to expect a BFR from this user, these bits are cleared by MBFM while receiving BFRs., reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmExpectedUsers0_u;

/*REG_MAC_BFM_MBFM_EXPECTED_USERS1 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swExpectBfrCfg1 : 4; //Read the last 4 users status - whether sender had configured MBFM to expect a BFR from this user, these bits are cleared by MBFM while receiving BFRs., reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmExpectedUsers1_u;

/*REG_MAC_BFM_MBFM_AUTH_VALID_USERS0 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swAuthValid0 : 32; //Read the first 32 users status - whether RXC had set this user authentication check valid. The value in this register is valid only if the compatible bit in MBFM_AUTH_CHECKED_USERS0 is set, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmAuthValidUsers0_u;

/*REG_MAC_BFM_MBFM_AUTH_VALID_USERS1 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swAuthValid1 : 4; //Read the last 4 users status - whether RXC had set this user authentication check valid. The value in this register is valid only if the compatible bit in MBFM_AUTH_CHECKED_USERS1 is set, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmAuthValidUsers1_u;

/*REG_MAC_BFM_MBFM_AUTH_CHECKED_USERS0 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swAuthChecked0 : 32; //Read the first 32 users status - whether RXC had finished this user authentication check, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmAuthCheckedUsers0_u;

/*REG_MAC_BFM_MBFM_AUTH_CHECKED_USERS1 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swAuthChecked1 : 4; //Read the last 4 users status - whether RXC had finished this user authentication check, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmAuthCheckedUsers1_u;

/*REG_MAC_BFM_MBFM_IN_PROCESS_USERS0 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swInProcessUser0 : 32; //Read the first 32 users status - whether RXC sent the START command but had not yet sent the END command for this user, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmInProcessUsers0_u;

/*REG_MAC_BFM_MBFM_IN_PROCESS_USERS1 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swInProcessUser1 : 4; //Read the last 4 users status - whether RXC sent the START command but had not yet sent the END command for this user, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmInProcessUsers1_u;

/*REG_MAC_BFM_MBFM_MIMO_CHECKED_USERS0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacBfmSt1MimoChecked0 : 32; //Read the first 32 users status - whether MIMO control was checked for this user, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmMimoCheckedUsers0_u;

/*REG_MAC_BFM_MBFM_MIMO_CHECKED_USERS1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacBfmSt1MimoChecked1 : 4; //Read the last 4 users status -  whether MIMO control was checked for this user, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmMimoCheckedUsers1_u;

/*REG_MAC_BFM_MBFM_MIMO_VALID_USERS0 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacBfmSt1MimoValid0 : 32; //Read the first 32 users status - whether MIMO control is valid for this user. The value in this register is valid only if the compatible bit in MBFM_MIMO_CHECKED_USERS0 is set, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmMimoValidUsers0_u;

/*REG_MAC_BFM_MBFM_MIMO_VALID_USERS1 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacBfmSt1MimoValid1 : 4; //Read the last 4 users status - whether MIMO control is valid for this user. The value in this register is valid only if the compatible bit in MBFM_MIMO_CHECKED_USERS1 is set, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmMimoValidUsers1_u;

/*REG_MAC_BFM_MBFM_FINAL_ENDED_USERS0 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacBfmSt3FinalEnded0 : 32; //Read the first 32 users status - whether BFR was finished for this user. , reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmFinalEndedUsers0_u;

/*REG_MAC_BFM_MBFM_FINAL_ENDED_USERS1 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMacBfmSt3FinalEnded1 : 4; //Read the last 4 users status - whether BFR was finished for this user. , reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmFinalEndedUsers1_u;

/*REG_MAC_BFM_MBFM_SU_PASSED_USERS0 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macBfmSt3SuPassed0 : 32; //Read the first 32 users status - whether BFR was valid for this user. The value in this register is valid only if the compatible bit in MBFM_FINAL_ENDED_USERS0 is set, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmSuPassedUsers0_u;

/*REG_MAC_BFM_MBFM_SU_PASSED_USERS1 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macBfmSt3SuPassed1 : 4; //Read the last 4 users status - whether BFR was valid for this user. The value in this register is valid only if the compatible bit in MBFM_FINAL_ENDED_USERS1 is set, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmSuPassedUsers1_u;

/*REG_MAC_BFM_MBFM_USERS0_STATIC_IND 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macBfmSt1Bw : 2; //static indication for user0 BW, reset value: 0x0, access type: RO
		uint32 macBfmSt1PhyMode : 2; //static indication for user0 phy mode, reset value: 0x0, access type: RO
		uint32 macBfmSt1Nc : 2; //static indication for user0 Nc, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 macBfmSt1HtcValid : 1; //static indication for user0 HTC valid indication, reset value: 0x0, access type: RO
		uint32 macBfmSt1McsFeedback : 7; //static indication for user0 MCS feedback field, reset value: 0x0, access type: RO
		uint32 reserved1 : 16;
	} bitFields;
} RegMacBfmMbfmUsers0StaticInd_u;

/*REG_MAC_BFM_MBFM_CONCLUSION_REG 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 macBfmSt4AllPassed : 1; //All BFR for the last session passed, reset value: 0x0, access type: RO
		uint32 macBfmSt4AllFailed : 1; //All BFR for the last session failed, reset value: 0x0, access type: RO
		uint32 macBfmSt4AtLeastOnePassed : 1; //At least one of the BFR for the last session passed, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 macBfmSt4ConclusionValid : 1; //valid bit for st4, reset value: 0x0, access type: RO
		uint32 reserved1 : 27;
	} bitFields;
} RegMacBfmMbfmConclusionReg_u;

/*REG_MAC_BFM_MBFM_FIFO_BASE_ADDR 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMbfmFifoBaseAddr : 22; //MBFM FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegMacBfmMbfmFifoBaseAddr_u;

/*REG_MAC_BFM_MBFM_FIFO_DEPTH_MINUS_ONE 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMbfmFifoDepthMinusOne : 8; //MBFM FIFO depth minus one, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacBfmMbfmFifoDepthMinusOne_u;

/*REG_MAC_BFM_MBFM_FIFO_CLEAR_STRB 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMbfmFifoClearStrb : 1; //Clear MBFM FIFO. , By write '1'., reset value: 0x0, access type: WO
		uint32 swMbfmFifoClearFullDropCtrStrb : 1; //Clear MBFM FIFO full drop counter. , By write '1'., reset value: 0x0, access type: WO
		uint32 swMbfmFifoClearDecLessThanZeroStrb : 1; //Clear MBFM FIFO decrement amount less than zero indication. , By write '1'.  , , reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegMacBfmMbfmFifoClearStrb_u;

/*REG_MAC_BFM_MBFM_FIFO_RD_ENTRIES_NUM 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMbfmFifoRdEntriesNum : 8; //MBFM FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacBfmMbfmFifoRdEntriesNum_u;

/*REG_MAC_BFM_MBFM_FIFO_NUM_ENTRIES_COUNT 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMbfmFifoNumEntriesCount : 9; //MBFM FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegMacBfmMbfmFifoNumEntriesCount_u;

/*REG_MAC_BFM_MBFM_FIFO_DEBUG 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMbfmFifoWrPtr : 8; //MBFM FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
		uint32 swMbfmFifoNotEmpty : 1; //MBFM FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 swMbfmFifoFull : 1; //MBFM FIFO full indication, reset value: 0x0, access type: RO
		uint32 swMbfmFifoDecrementLessThanZero : 1; //MBFM setting FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 swMbfmFifoFullDropCtr : 8; //MBFM setting FIFO full drop counter, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmFifoDebug_u;

/*REG_MAC_BFM_MBFM_FIFO_CFG 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swMbfmErrorFifoEn : 1; //Enable the error fifo update , 0b1 - enable , 0b0 - disable, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 swFifoNcMismatchEn : 1; //Enable the case of nc mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoNrMismatchEn : 1; //Enable the case of nr mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoBwMismatchEn : 1; //Enable the case of bw mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoNgMismatchEn : 1; //Enable the case of ng mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoCodebookMismatchEn : 1; //Enable the case of codebook mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoFeedbackTypeMismatchEn : 1; //Enable the case of feedback type mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoBfrLengthMismatchEn : 1; //Enable the case of bfr length mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoAuthErrorEn : 1; //Enable the case of auth error to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoBfrVariantEn : 1; //Enable the case of BFR variant mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoRuStartEn : 1; //Enable the case of RU START mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoRuEndEn : 1; //Enable the case of RU END mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoSegmentEn : 1; //Enable the case of segment field mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoTimeoutEventEn : 1; //Enable the case of timeout event to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoTypeMismatchEn : 1; //Enable the case of type or sub-type mismatch to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoGeneralErrEn : 1; //Enable the case of uncpecified error to update the error fifo, reset value: 0x1, access type: RW
		uint32 swFifoFcsErrorEn : 1; //Enable the case of fcs error to update the error fifo, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
		uint32 mbfmErrorFifoIrq : 1; //Indicates error fifo not empty, reset value: 0x0, access type: RO
		uint32 reserved2 : 7;
	} bitFields;
} RegMacBfmMbfmFifoCfg_u;

/*REG_MAC_BFM_MBFM_RO_ERRORS 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ruStartLatestError : 8; //holds latest error of RU start. in case MSB of this field is 0 there was no error since last clear, reset value: 0x0, access type: RO
		uint32 ruEndLatestError : 10; //holds latest error of RU start. in case MSB of this field is 0 there was no error since last clear. , 6 LSBs are the received RU end. Then 2 bits [7:6] are the BW., reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 feedbackSegmentsLatestError : 5; //holds latest error of Feedback segments. in case MSB of this field is 0 there was no error since last clear., reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
		uint32 swClearRoErrors : 1; //write 1'b1 to this bit to clear the errors in this register, reset value: 0x0, access type: WO
	} bitFields;
} RegMacBfmMbfmRoErrors_u;

/*REG_MAC_BFM_MBFM_LOGGER_CTRL 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLoggerEn : 1; //Enable Logger activity. Support soft activation / de-activation, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 mbfmLoggerActiveInd : 1; //Poll this bit to actually verify the logger machine status: 0b0 - machine is off (no logger transactions) 0b1 - machine is on (module generates logger transactions), reset value: 0x0, access type: RO
		uint32 reserved1 : 3;
		uint32 swLoggerPriority : 2; //Set the priority of the MBFM towards the logger, reset value: 0x0, access type: RW
		uint32 reserved2 : 6;
		uint32 swLoggerStreamEn : 16; //Each bit set to 1 enables a data stream to the logger, reset value: 0x0, access type: RW
	} bitFields;
} RegMacBfmMbfmLoggerCtrl_u;

/*REG_MAC_BFM_MBFM_LOGGER_CFG 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLoggerCfg : 16; //general control for logger data in the stream, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacBfmMbfmLoggerCfg_u;

/*REG_MAC_BFM_MBFM_CLR_OVERFLOW 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swClrInputFifoOverflow : 1; //This field clears the internal MBFM fifo in case of overflow event , write 1'b1 to this bit to clear the fifo., reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 inputFifoOverflow : 1; //Sticky overflow indication , 1'b1 - There was overflow , 1'b0 - There wasn't overflow , Use the sw_clr_input_fifo_overflow bit to clear the fifo., reset value: 0x0, access type: RO
		uint32 reserved1 : 23;
	} bitFields;
} RegMacBfmMbfmClrOverflow_u;

/*REG_MAC_BFM_MBFM_SPARE_REG1 0x1C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareField0 : 32; //spare, reset value: 0xFF000000, access type: RW
	} bitFields;
} RegMacBfmMbfmSpareReg1_u;

/*REG_MAC_BFM_MBFM_SPARE_REG2 0x1C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareField1 : 8; //spare, reset value: 0xF0, access type: RW
		uint32 spareField2 : 4; //spare, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 spareField3 : 4; //spare, reset value: 0x0, access type: RW
		uint32 reserved1 : 4;
		uint32 spareField4 : 4; //spare, reset value: 0x0, access type: RW
		uint32 reserved2 : 4;
	} bitFields;
} RegMacBfmMbfmSpareReg2_u;

/*REG_MAC_BFM_MBFM_SPARE_REG3 0x1C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareField5 : 4; //spare, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfmMbfmSpareReg3_u;



#endif // _MAC_BFM_REGS_H_
