
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
#define	REG_MAC_BFM_MBFM_GLOBAL_EN                   (MAC_BFM_BASE_ADDRESS + 0x4)
#define	REG_MAC_BFM_MBFM_CTRL1                       (MAC_BFM_BASE_ADDRESS + 0x8)
#define	REG_MAC_BFM_MBFM_TSF_LATCH                   (MAC_BFM_BASE_ADDRESS + 0xC)
#define	REG_MAC_BFM_MBFM_IMP_CTRL                    (MAC_BFM_BASE_ADDRESS + 0x10)
#define	REG_MAC_BFM_MBFM_HOOKS                       (MAC_BFM_BASE_ADDRESS + 0x14)
#define	REG_MAC_BFM_MBFM_CFG_HE_ACT                  (MAC_BFM_BASE_ADDRESS + 0x20)
#define	REG_MAC_BFM_MBFM_CFG_DB_UPDATE               (MAC_BFM_BASE_ADDRESS + 0x24)
#define	REG_MAC_BFM_MBFM_CFG_PER_BFR                 (MAC_BFM_BASE_ADDRESS + 0x28)
#define	REG_MAC_BFM_MBFM_CFG_OT                      (MAC_BFM_BASE_ADDRESS + 0x2C)
#define	REG_MAC_BFM_MBFM_CFG_HE_RU_END               (MAC_BFM_BASE_ADDRESS + 0x30)
#define	REG_MAC_BFM_MBFM_EXPECTED_ORIG_USERS0        (MAC_BFM_BASE_ADDRESS + 0x38)
#define	REG_MAC_BFM_MBFM_EXPECTED_ORIG_USERS1        (MAC_BFM_BASE_ADDRESS + 0x3C)
#define	REG_MAC_BFM_MBFM_EXPECTED_USERS0             (MAC_BFM_BASE_ADDRESS + 0x40)
#define	REG_MAC_BFM_MBFM_EXPECTED_USERS1             (MAC_BFM_BASE_ADDRESS + 0x44)
#define	REG_MAC_BFM_MBFM_AUTH_VALID_USERS0           (MAC_BFM_BASE_ADDRESS + 0x48)
#define	REG_MAC_BFM_MBFM_AUTH_VALID_USERS1           (MAC_BFM_BASE_ADDRESS + 0x4C)
#define	REG_MAC_BFM_MBFM_AUTH_CHECKED_USERS0         (MAC_BFM_BASE_ADDRESS + 0x50)
#define	REG_MAC_BFM_MBFM_AUTH_CHECKED_USERS1         (MAC_BFM_BASE_ADDRESS + 0x54)
#define	REG_MAC_BFM_MBFM_IN_PROCESS_USERS0           (MAC_BFM_BASE_ADDRESS + 0x58)
#define	REG_MAC_BFM_MBFM_IN_PROCESS_USERS1           (MAC_BFM_BASE_ADDRESS + 0x5C)
#define	REG_MAC_BFM_MBFM_MIMO_CHECKED_USERS0         (MAC_BFM_BASE_ADDRESS + 0x60)
#define	REG_MAC_BFM_MBFM_MIMO_CHECKED_USERS1         (MAC_BFM_BASE_ADDRESS + 0x64)
#define	REG_MAC_BFM_MBFM_MIMO_VALID_USERS0           (MAC_BFM_BASE_ADDRESS + 0x68)
#define	REG_MAC_BFM_MBFM_MIMO_VALID_USERS1           (MAC_BFM_BASE_ADDRESS + 0x6C)
#define	REG_MAC_BFM_MBFM_FINAL_ENDED_USERS0          (MAC_BFM_BASE_ADDRESS + 0x70)
#define	REG_MAC_BFM_MBFM_FINAL_ENDED_USERS1          (MAC_BFM_BASE_ADDRESS + 0x74)
#define	REG_MAC_BFM_MBFM_SU_PASSED_USERS0            (MAC_BFM_BASE_ADDRESS + 0x78)
#define	REG_MAC_BFM_MBFM_SU_PASSED_USERS1            (MAC_BFM_BASE_ADDRESS + 0x7C)
#define	REG_MAC_BFM_MBFM_USERS0_STATIC_IND           (MAC_BFM_BASE_ADDRESS + 0x80)
#define	REG_MAC_BFM_MBFM_CONCLUSION_REG              (MAC_BFM_BASE_ADDRESS + 0x84)
#define	REG_MAC_BFM_MBFM_FIFO_BASE_ADDR              (MAC_BFM_BASE_ADDRESS + 0xA0)
#define	REG_MAC_BFM_MBFM_FIFO_DEPTH_MINUS_ONE        (MAC_BFM_BASE_ADDRESS + 0xA4)
#define	REG_MAC_BFM_MBFM_FIFO_CLEAR_STRB             (MAC_BFM_BASE_ADDRESS + 0xA8)
#define	REG_MAC_BFM_MBFM_FIFO_RD_ENTRIES_NUM         (MAC_BFM_BASE_ADDRESS + 0xAC)
#define	REG_MAC_BFM_MBFM_FIFO_NUM_ENTRIES_COUNT      (MAC_BFM_BASE_ADDRESS + 0xB0)
#define	REG_MAC_BFM_MBFM_FIFO_DEBUG                  (MAC_BFM_BASE_ADDRESS + 0xB4)
#define	REG_MAC_BFM_MBFM_FIFO_CFG                    (MAC_BFM_BASE_ADDRESS + 0xC0)
#define	REG_MAC_BFM_MBFM_RO_ERRORS                   (MAC_BFM_BASE_ADDRESS + 0xC4)
#define	REG_MAC_BFM_MBFM_LOGGER_CTRL1                (MAC_BFM_BASE_ADDRESS + 0xF0)
#define	REG_MAC_BFM_MBFM_LOGGER_CTRL2                (MAC_BFM_BASE_ADDRESS + 0xF4)
#define	REG_MAC_BFM_MBFM_LOGGER_CFG                  (MAC_BFM_BASE_ADDRESS + 0xF8)
#define	REG_MAC_BFM_MBFM_CLR_OVERFLOW                (MAC_BFM_BASE_ADDRESS + 0x100)
#define	REG_MAC_BFM_MBFM_IMP_THRSHLD_CFG1            (MAC_BFM_BASE_ADDRESS + 0x120)
#define	REG_MAC_BFM_MBFM_IMP_THRSHLD_CFG2            (MAC_BFM_BASE_ADDRESS + 0x124)
#define	REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX0    (MAC_BFM_BASE_ADDRESS + 0x144)
#define	REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX1    (MAC_BFM_BASE_ADDRESS + 0x148)
#define	REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX2    (MAC_BFM_BASE_ADDRESS + 0x14C)
#define	REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX3    (MAC_BFM_BASE_ADDRESS + 0x150)
#define	REG_MAC_BFM_MBFM_BFEE_CTRL1                  (MAC_BFM_BASE_ADDRESS + 0x170)
#define	REG_MAC_BFM_MBFM_BFEE_TXC_POINTER_A          (MAC_BFM_BASE_ADDRESS + 0x174)
#define	REG_MAC_BFM_MBFM_BFEE_TXC_POINTER_B          (MAC_BFM_BASE_ADDRESS + 0x178)
#define	REG_MAC_BFM_MBFM_BFEE_TXC_POINTER_C          (MAC_BFM_BASE_ADDRESS + 0x17C)
#define	REG_MAC_BFM_MBFM_TX_RECIPE_CTRL              (MAC_BFM_BASE_ADDRESS + 0x180)
#define	REG_MAC_BFM_MBFM_BFEE_STAT                   (MAC_BFM_BASE_ADDRESS + 0x184)
#define	REG_MAC_BFM_MBFM_BFEE_TX_CTRL1               (MAC_BFM_BASE_ADDRESS + 0x188)
#define	REG_MAC_BFM_MBFM_BFEE_TX_CTRL2               (MAC_BFM_BASE_ADDRESS + 0x18C)
#define	REG_MAC_BFM_MBFM_BFEE_TX_CTRL3               (MAC_BFM_BASE_ADDRESS + 0x190)
#define	REG_MAC_BFM_MBFM_BFEE_TX_CTRL4               (MAC_BFM_BASE_ADDRESS + 0x198)
#define	REG_MAC_BFM_MBFM_BFEE_TX_CTRL5               (MAC_BFM_BASE_ADDRESS + 0x19C)
#define	REG_MAC_BFM_MBFM_HOB_CTRL                    (MAC_BFM_BASE_ADDRESS + 0x1A0)
#define	REG_MAC_BFM_MBFM_HOB_RD_DATA                 (MAC_BFM_BASE_ADDRESS + 0x1A4)
#define	REG_MAC_BFM_MBFM_BFEE_SW_GEN                 (MAC_BFM_BASE_ADDRESS + 0x1B0)
#define	REG_MAC_BFM_MBFM_BFEE_CNTR_CTRL              (MAC_BFM_BASE_ADDRESS + 0x1B8)
#define	REG_MAC_BFM_MBFM_BFEE_CNTR_VAL               (MAC_BFM_BASE_ADDRESS + 0x1BC)
#define	REG_MAC_BFM_MBFM_ISR_CTRL                    (MAC_BFM_BASE_ADDRESS + 0x1D0)
#define	REG_MAC_BFM_MBFM_ISR_CTRL_CLR                (MAC_BFM_BASE_ADDRESS + 0x1D4)
#define	REG_MAC_BFM_MBFM_SPARE_REG1                  (MAC_BFM_BASE_ADDRESS + 0x1F0)
#define	REG_MAC_BFM_MBFM_SPARE_REG2                  (MAC_BFM_BASE_ADDRESS + 0x1F4)
#define	REG_MAC_BFM_MBFM_SPARE_REG3                  (MAC_BFM_BASE_ADDRESS + 0x1F8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_BFM_MBFM_GLOBAL_EN 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfGlobalEn : 1; //Global enable for MBFM. , Software must write '1' to this bit in order for the MBFM to be active. , 1'b0 - MBFM is off , 1'b1 - MBFM is on, reset value: 0x0, access type: RW
		uint32 swBfeeGlobalEn : 1; //Global enable for MBFM to detect NDPA from RXC , Software must write '1' to this bit in order for the MBFM to support Bfee mode. , 1'b0 - MBFM doesn't latch NDPA fields , 1'b1 - MBFM Bfee is on, reset value: 0x0, access type: RW
		uint32 swImplicitGlobalEn : 1; //Enable the implicit mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegMacBfmMbfmGlobalEn_u;

/*REG_MAC_BFM_MBFM_CTRL1 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfGlobalSmOut : 4; //Read current global state machine state, reset value: 0x0, access type: RO
		uint32 bfGlobalState : 2; //Read current global state: 0 - EXPLICIT 1 - Implicit 2 -Bfee, reset value: 0x1, access type: RO
		uint32 reserved0 : 2;
		uint32 mpStationState : 5; //Read current user state in the machine, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
		uint32 bfBfeeSmOut : 5; //Read currentBFee state in the machine, reset value: 0x0, access type: RO
		uint32 reserved2 : 1;
		uint32 swClrStaticIndPulse : 1; //write 1 to this bit (self cleared) to clear all external static indications., reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
		uint32 swClrExpectedBfrPulse : 1; //write 1 to this bit (self cleared) to clear all expected BFR user vector during sounding initialization, reset value: 0x0, access type: RW
		uint32 reserved4 : 2;
		uint32 pendingTxCircSmcReq : 1; //Indicate pending SMC access is active to TX Circ RAM, reset value: 0x0, access type: RO
		uint32 swForceGoToIdlePulse : 1; //force state machine to go to idle. Use this only in case the MBFM is stuck, might cause hazards., reset value: 0x0, access type: RW
		uint32 reserved5 : 1;
		uint32 swOverrideRxcBfrStartPulse : 1; //Start explicit session. Use this only in case there is a bug in RXC explicit global start indication., reset value: 0x0, access type: RW
		uint32 pendingShrdRamSmcReq : 1; //Indicate pending SMC access is active to shared RAM, reset value: 0x0, access type: RO
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

/*REG_MAC_BFM_MBFM_IMP_CTRL 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swImplicitModeHtEn : 1; //Enable the implicit mode for any phy mode HT DB update, reset value: 0x1, access type: RW
		uint32 swImplicitModeVhtEn : 1; //Enable the implicit mode for any phy mode VHT DB update, reset value: 0x1, access type: RW
		uint32 reserved0 : 2;
		uint32 swImplicitModeLengthCheckEn : 1; //Enable the length check (mimo control length calculation Versus phy length calculation) for implicit update DB, reset value: 0x1, access type: RW
		uint32 reserved1 : 27;
	} bitFields;
} RegMacBfmMbfmImpCtrl_u;

/*REG_MAC_BFM_MBFM_HOOKS 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDetectBfeeErrFifoCollision : 1; //Enable the MBFM to go into Bfee session in case the NDPA was detected while former session didn't end yet. For example if former session error fifo still didn't received the smc_aready but NDPA was detected, main state machine waits for error fifo to end but the Bfee sm already detected the NDPA, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 swClearSmIdleExplicitPulse : 1; //Clear states and control flops related to explicit (Bfer mode). This register is a backdoor hook, don't use it unless there is a fatal bug in the MBFM. Using this register might cause SMC protocol violation, reset value: 0x0, access type: RW
		uint32 swClearSmIdleBfeePulse : 1; //Clear states and control flops related to Bfee. This register is a backdoor hook, don't use it unless there is a fatal bug in the MBFM. Using this register might cause SMC protocol violation, reset value: 0x0, access type: RW
		uint32 reserved1 : 26;
	} bitFields;
} RegMacBfmMbfmHooks_u;

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
		uint32 swDropFeedbackSegments0111 : 1; //Software configures whether to drop the case feedback segments subfield is 0 and the Remaining Feedback Segments subfield is 7 in case of VHT. This field decision is independent to the sw_check_feedback_segments setting , 1'b1 - drop this BFR  , 1'b0 - try to parse the BFR , reset value: 0x1, access type: RW
		uint32 reserved0 : 2;
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
		uint32 swMuBfrWithoutDSnr : 1; //802.11 spec expects Delta SNR after the phases stage. Write 1'b1 to this field to support MU report with no delta SNR., reset value: 0x0, access type: RW
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
		uint32 swMbfmFifoRdEntriesNum : 9; //MBFM FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
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
		uint32 swFifoBfeeEn : 1; //Enable the case of Bfee error to update the error fifo, reset value: 0x1, access type: RW
		uint32 reserved1 : 3;
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

/*REG_MAC_BFM_MBFM_LOGGER_CTRL1 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLoggerEn : 1; //Enable Logger activity. Support soft activation / de-activation, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 mbfmLoggerActiveInd : 1; //Poll this bit to actually verify the logger machine status: 0b0 - machine is off (no logger transactions) 0b1 - machine is on (module generates logger transactions), reset value: 0x0, access type: RO
		uint32 swLoggerAllowValidWhileDone : 1; //Enable Logge to send "valid" event during "done" event, reset value: 0x0, access type: RW
		uint32 swLoggerPriority : 2; //Set the priority of the MBFM towards the logger, reset value: 0x0, access type: RW
		uint32 swLoggerLimit : 7; //limit the number of "valid" events between 2 "done" events, reset value: 0x7D, access type: RW
		uint32 reserved1 : 17;
	} bitFields;
} RegMacBfmMbfmLoggerCtrl1_u;

/*REG_MAC_BFM_MBFM_LOGGER_CTRL2 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLoggerStreamEn : 24; //Each bit set to 1 enables a data stream to the logger, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfmMbfmLoggerCtrl2_u;

/*REG_MAC_BFM_MBFM_LOGGER_CFG 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swLoggerCfg : 8; //general control for logger data in the stream, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
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

/*REG_MAC_BFM_MBFM_IMP_THRSHLD_CFG1 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx0TsfImplicitUpdateThreshold : 10; //idx0 old report threshold * 32 mSec, reset value: 0xD, access type: RW
		uint32 reserved0 : 6;
		uint32 idx1TsfImplicitUpdateThreshold : 10; //idx1 old report threshold * 32 mSec, reset value: 0xD, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegMacBfmMbfmImpThrshldCfg1_u;

/*REG_MAC_BFM_MBFM_IMP_THRSHLD_CFG2 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx2TsfImplicitUpdateThreshold : 10; //idx2 old report threshold * 32 mSec, reset value: 0xD, access type: RW
		uint32 reserved0 : 6;
		uint32 idx3TsfImplicitUpdateThreshold : 10; //idx3 old report threshold * 32 mSec, reset value: 0xD, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegMacBfmMbfmImpThrshldCfg2_u;

/*REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX0 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx0CtsNeverUpdate : 1; //idx0 CTS never update, reset value: 0x0, access type: RW
		uint32 idx0CtsVerifyBwToDb : 1; //idx0 CTS verify bw to db, reset value: 0x1, access type: RW
		uint32 idx0CtsVerifyNssToDb : 1; //idx0 CTS verify nss to db, reset value: 0x1, access type: RW
		uint32 idx0CtsVerifyNssToMaxTxNss : 1; //idx0 CTS verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx0CtsAgingIgnoreBwDbRules : 1; //idx0 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx0CtsAgingIgnoreNssDbRules : 1; //idx0 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx0CtsVerifyBwToBwScrambler : 1; //idx0 CTS verify bw to bw scrambler, reset value: 0x1, access type: RW
		uint32 idx0CtsVerifyBwScramblerToDb : 1; //idx0 CTS verify bw scrambler to db, reset value: 0x1, access type: RW
		uint32 idx0AckBaNeverUpdate : 1; //idx0 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx0AckBaVerifyBwToDb : 1; //idx0 ACK/BA verify bw to db, reset value: 0x1, access type: RW
		uint32 idx0AckBaVerifyNssToDb : 1; //idx0 ACK/BA verify nss to db, reset value: 0x1, access type: RW
		uint32 idx0AckBaVerifyNssToMaxTxNss : 1; //idx0 ACK/BA verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx0AckBaAgingIgnoreBwDbRules : 1; //idx0 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx0AckBaAgingIgnoreNssDbRules : 1; //idx0 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx0OtherRxNeverUpdate : 1; //idx0 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx0OtherRxVerifyBwToDb : 1; //idx0 other Rx verify bw to db, reset value: 0x1, access type: RW
		uint32 idx0OtherRxVerifyNssToDb : 1; //idx0 other Rx verify nss to db, reset value: 0x1, access type: RW
		uint32 idx0OtherRxVerifyNssToMaxTxNss : 1; //idx0 other Rx verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx0OtherRxAgingIgnoreBwDbRules : 1; //idx0 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx0OtherRxAgingIgnoreNssDbRules : 1; //idx0 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfmBfNonstandardImpRulesIdx0_u;

/*REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX1 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx1CtsNeverUpdate : 1; //idx1 CTS never update, reset value: 0x0, access type: RW
		uint32 idx1CtsVerifyBwToDb : 1; //idx1 CTS verify bw to db, reset value: 0x1, access type: RW
		uint32 idx1CtsVerifyNssToDb : 1; //idx1 CTS verify nss to db, reset value: 0x1, access type: RW
		uint32 idx1CtsVerifyNssToMaxTxNss : 1; //idx1 CTS verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx1CtsAgingIgnoreBwDbRules : 1; //idx1 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx1CtsAgingIgnoreNssDbRules : 1; //idx1 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx1CtsVerifyBwToBwScrambler : 1; //idx1 CTS verify bw to bw scrambler, reset value: 0x1, access type: RW
		uint32 idx1CtsVerifyBwScramblerToDb : 1; //idx1 CTS verify bw scrambler to db, reset value: 0x1, access type: RW
		uint32 idx1AckBaNeverUpdate : 1; //idx1 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx1AckBaVerifyBwToDb : 1; //idx1 ACK/BA verify bw to db, reset value: 0x1, access type: RW
		uint32 idx1AckBaVerifyNssToDb : 1; //idx1 ACK/BA verify nss to db, reset value: 0x1, access type: RW
		uint32 idx1AckBaVerifyNssToMaxTxNss : 1; //idx1 ACK/BA verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx1AckBaAgingIgnoreBwDbRules : 1; //idx1 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx1AckBaAgingIgnoreNssDbRules : 1; //idx1 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx1OtherRxNeverUpdate : 1; //idx1 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx1OtherRxVerifyBwToDb : 1; //idx1 other Rx verify bw to db, reset value: 0x1, access type: RW
		uint32 idx1OtherRxVerifyNssToDb : 1; //idx1 other Rx verify nss to db, reset value: 0x1, access type: RW
		uint32 idx1OtherRxVerifyNssToMaxTxNss : 1; //idx1 other Rx verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx1OtherRxAgingIgnoreBwDbRules : 1; //idx1 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx1OtherRxAgingIgnoreNssDbRules : 1; //idx1 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfmBfNonstandardImpRulesIdx1_u;

/*REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX2 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx2CtsNeverUpdate : 1; //idx2 CTS never update, reset value: 0x0, access type: RW
		uint32 idx2CtsVerifyBwToDb : 1; //idx2 CTS verify bw to db, reset value: 0x1, access type: RW
		uint32 idx2CtsVerifyNssToDb : 1; //idx2 CTS verify nss to db, reset value: 0x1, access type: RW
		uint32 idx2CtsVerifyNssToMaxTxNss : 1; //idx2 CTS verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx2CtsAgingIgnoreBwDbRules : 1; //idx2 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx2CtsAgingIgnoreNssDbRules : 1; //idx2 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx2CtsVerifyBwToBwScrambler : 1; //idx2 CTS verify bw to bw scrambler, reset value: 0x1, access type: RW
		uint32 idx2CtsVerifyBwScramblerToDb : 1; //idx2 CTS verify bw scrambler to db, reset value: 0x1, access type: RW
		uint32 idx2AckBaNeverUpdate : 1; //idx2 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx2AckBaVerifyBwToDb : 1; //idx2 ACK/BA verify bw to db, reset value: 0x1, access type: RW
		uint32 idx2AckBaVerifyNssToDb : 1; //idx2 ACK/BA verify nss to db, reset value: 0x1, access type: RW
		uint32 idx2AckBaVerifyNssToMaxTxNss : 1; //idx2 ACK/BA verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx2AckBaAgingIgnoreBwDbRules : 1; //idx2 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx2AckBaAgingIgnoreNssDbRules : 1; //idx2 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx2OtherRxNeverUpdate : 1; //idx2 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx2OtherRxVerifyBwToDb : 1; //idx2 other Rx verify bw to db, reset value: 0x1, access type: RW
		uint32 idx2OtherRxVerifyNssToDb : 1; //idx2 other Rx verify nss to db, reset value: 0x1, access type: RW
		uint32 idx2OtherRxVerifyNssToMaxTxNss : 1; //idx2 other Rx verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx2OtherRxAgingIgnoreBwDbRules : 1; //idx2 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx2OtherRxAgingIgnoreNssDbRules : 1; //idx2 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfmBfNonstandardImpRulesIdx2_u;

/*REG_MAC_BFM_BF_NONSTANDARD_IMP_RULES_IDX3 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx3CtsNeverUpdate : 1; //idx3 CTS never update, reset value: 0x0, access type: RW
		uint32 idx3CtsVerifyBwToDb : 1; //idx3 CTS verify bw to db, reset value: 0x1, access type: RW
		uint32 idx3CtsVerifyNssToDb : 1; //idx3 CTS verify nss to db, reset value: 0x1, access type: RW
		uint32 idx3CtsVerifyNssToMaxTxNss : 1; //idx3 CTS verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx3CtsAgingIgnoreBwDbRules : 1; //idx3 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx3CtsAgingIgnoreNssDbRules : 1; //idx3 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx3CtsVerifyBwToBwScrambler : 1; //idx3 CTS verify bw to bw scrambler, reset value: 0x1, access type: RW
		uint32 idx3CtsVerifyBwScramblerToDb : 1; //idx3 CTS verify bw scrambler to db, reset value: 0x1, access type: RW
		uint32 idx3AckBaNeverUpdate : 1; //idx3 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx3AckBaVerifyBwToDb : 1; //idx3 ACK/BA verify bw to db, reset value: 0x1, access type: RW
		uint32 idx3AckBaVerifyNssToDb : 1; //idx3 ACK/BA verify nss to db, reset value: 0x1, access type: RW
		uint32 idx3AckBaVerifyNssToMaxTxNss : 1; //idx3 ACK/BA verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx3AckBaAgingIgnoreBwDbRules : 1; //idx3 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx3AckBaAgingIgnoreNssDbRules : 1; //idx3 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx3OtherRxNeverUpdate : 1; //idx3 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx3OtherRxVerifyBwToDb : 1; //idx3 other Rx verify bw to db, reset value: 0x1, access type: RW
		uint32 idx3OtherRxVerifyNssToDb : 1; //idx3 other Rx verify nss to db, reset value: 0x1, access type: RW
		uint32 idx3OtherRxVerifyNssToMaxTxNss : 1; //idx3 other Rx verify nss to max tx nss, reset value: 0x1, access type: RW
		uint32 idx3OtherRxAgingIgnoreBwDbRules : 1; //idx3 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx3OtherRxAgingIgnoreNssDbRules : 1; //idx3 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfmBfNonstandardImpRulesIdx3_u;

/*REG_MAC_BFM_MBFM_BFEE_CTRL1 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeAllowNdpDetectionAllPhyModes : 1; //Allow receive NDP on all phy modes.  , 1'b1 - receive all phy modes in NDP and treat the phy mode as the PHY calim in the START event , 1'b0 - receive only HT, VHT SU and HE SU as valid Bfee sequence, reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 swReplaceAddr3Sel : 1; //SW Hook to modify the way MBFM builds ADDR3 for Bfee.  , 1'b0 - take the value the same as DUR module does , 1'b1 - take the opposite value from DUR module, reset value: 0x0, access type: RW
		uint32 swReplaceAddr1N2 : 1; //SW Hook to modify the way MBFM builds ADDR1&2 for Bfee.  , 1'b0 - take the value the same as RXC module does , 1'b1 - take the opposite value from RXC module, reset value: 0x0, access type: RW
		uint32 swAdd1ToAid : 1; //use STA_ID +1 as AID. , 1'b0: AID is sta_id , 1'b1: AID is sta_id + 1, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 swNdpHtUpdates11AgTx : 1; //whether HT NDP Bfee is allowed for 11 a/g TX phy mode, reset value: 0x0, access type: RW
		uint32 swNdpHtUpdates11BTx : 1; //whether HT NDP Bfee is allowed for 11b TX phy mode, reset value: 0x1, access type: RW
		uint32 swNdpVhtUpdates11AgTx : 1; //whether VHT NDP Bfee is allowed for 11 a/g TX phy mode, reset value: 0x0, access type: RW
		uint32 swNdpVhtUpdates11BTx : 1; //whether VHT NDP Bfee is allowed for 11b TX phy mode, reset value: 0x0, access type: RW
		uint32 swNdpVhtUpdatesHtTx : 1; //whether VHT NDP Bfee is allowed for HT TX phy mode, reset value: 0x0, access type: RW
		uint32 swNdpHeUpdates11AgTx : 1; //whether HE NDP Bfee is allowed for 11 a/g TX phy mode, reset value: 0x0, access type: RW
		uint32 swNdpHeUpdates11BTx : 1; //whether HE NDP Bfee is allowed for 11b TX phy mode, reset value: 0x0, access type: RW
		uint32 swNdpHeUpdatesHtTx : 1; //whether HE NDP Bfee is allowed for HT TX phy mode, reset value: 0x0, access type: RW
		uint32 reserved2 : 16;
	} bitFields;
} RegMacBfmMbfmBfeeCtrl1_u;

/*REG_MAC_BFM_MBFM_BFEE_TXC_POINTER_A 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTxcSectionA4BBaseAddr : 11; //4B address for base address for section A data in the TX circular buffer user 0, reset value: 0x0, access type: RW
		uint32 reserved0 : 21;
	} bitFields;
} RegMacBfmMbfmBfeeTxcPointerA_u;

/*REG_MAC_BFM_MBFM_BFEE_TXC_POINTER_B 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTxcSectionB4BBaseAddr : 11; //4B address for base address for section B data in the TX circular buffer user 0, reset value: 0x200, access type: RW
		uint32 reserved0 : 21;
	} bitFields;
} RegMacBfmMbfmBfeeTxcPointerB_u;

/*REG_MAC_BFM_MBFM_BFEE_TXC_POINTER_C 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTxcSectionC4BBaseAddr : 11; //4B address for base address for section C data in the TX circular buffer user 0, reset value: 0x400, access type: RW
		uint32 reserved0 : 21;
	} bitFields;
} RegMacBfmMbfmBfeeTxcPointerC_u;

/*REG_MAC_BFM_MBFM_TX_RECIPE_CTRL 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swTxRecipe4BBaseAddr : 11; //base address in TX recipe for TCRs to be written by the MBFM for BFee mode. , The MBFM adds the TX_RECIPES_BASE_ADDR , This address is 4B address (32bits aligned), reset value: 0x0, access type: RW
		uint32 reserved0 : 21;
	} bitFields;
} RegMacBfmMbfmTxRecipeCtrl_u;

/*REG_MAC_BFM_MBFM_BFEE_STAT 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStaIdxUsr0Smp : 9; //Read latest sta ID sampled by MBFM for the Bfee sequence, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 rxcVapIdxUsr0Smp : 5; //Read latest VAP ID sampled by MBFM for the Bfee sequence, reset value: 0x0, access type: RO
		uint32 reserved1 : 11;
	} bitFields;
} RegMacBfmMbfmBfeeStat_u;

/*REG_MAC_BFM_MBFM_BFEE_TX_CTRL1 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeTcrTxBfForceLegacy : 1; //Control whether the TXBF bit in the TCR will be: , 1'b0 - taken from STA DB , 1'b1 - set to 1'b0 (no sounding), reset value: 0x1, access type: RW
		uint32 swBfeeTcrTxBfForce11B : 1; //Control whether the TXBF bit in the TCR will be: , 1'b0 - taken from STA DB , 1'b1 - set to 1'b0 (no sounding), reset value: 0x1, access type: RW
		uint32 swBfeeTcrTxBfForceHt : 1; //Control whether the TXBF bit in the TCR will be: , 1'b0 - taken from STA DB , 1'b1 - set to 1'b0 (no sounding), reset value: 0x1, access type: RW
		uint32 swBfeeTcrTxBfForceVht : 1; //Control whether the TXBF bit in the TCR will be: , 1'b0 - taken from STA DB , 1'b1 - set to 1'b0 (no sounding), reset value: 0x1, access type: RW
		uint32 swBfeeTcrTxBfForceHe : 1; //Control whether the TXBF bit in the TCR will be: , 1'b0 - taken from STA DB , 1'b1 - set to 1'b0 (no sounding), reset value: 0x1, access type: RW
		uint32 reserved0 : 3;
		uint32 swHeForce1Nss : 1; //Control whether to force TX in 1 NSS for HE in BW grater or equal to sw_he_force_1nss_bw_threshold: , 1'b0 - don't force special extra Nss degradation , 1'b1 - force to 1Nss, reset value: 0x1, access type: RW
		uint32 swHeForce1NssBwThreshold : 2; //select the BW threshold for the force 1NSS. Any BW grater or equal to this BW will be forced to 1 NSS in case sw_he_force_1nss is set, reset value: 0x3, access type: RW
		uint32 reserved1 : 5;
		uint32 swHeLtfManipultion : 4; //bit per {LTF,CP} set. Bit values: , 1'b1 - force TX TCR {LTF,CP} to {2,2}  , 1'b0 - don't modify {LTF,CP}  , bit 0 - {0,0} , bit 1 - {1,0} , bit 2 - {1,1} , bit 3 - {2,0}, reset value: 0xF, access type: RW
		uint32 reserved2 : 4;
		uint32 swVerifyNpdaVsNdpPhyMode : 3; //bit per phy mode to enable/disable phy mode match check between NDPA format and the NDP. Bit [0]: HT, bit [1]: VHT, bit [2] : HE , 1'b1 - check those fields  , 1'b0 - don't check , reset value: 0x0, access type: RW
		uint32 reserved3 : 5;
	} bitFields;
} RegMacBfmMbfmBfeeTxCtrl1_u;

/*REG_MAC_BFM_MBFM_BFEE_TX_CTRL2 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeMcsDegradationFactor0 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeMcsDegradationFactor1 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeMcsDegradationFactor2 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeMcsDegradationFactor3 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeMcsDegradationFactor4 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
		uint32 swBfeeMcsDegradationFactor5 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
		uint32 swBfeeMcsDegradationFactor6 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
		uint32 swBfeeMcsDegradationFactor7 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
	} bitFields;
} RegMacBfmMbfmBfeeTxCtrl2_u;

/*REG_MAC_BFM_MBFM_BFEE_TX_CTRL3 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeMcsDegradationFactor8 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
		uint32 swBfeeMcsDegradationFactor9 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
		uint32 swBfeeMcsDegradationFactor10 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
		uint32 swBfeeMcsDegradationFactor11 : 4; //set the MCS degradation logic to use. each selection causes a different MCS degradation logic. Value 0 means no MCS degradation., reset value: 0x2, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacBfmMbfmBfeeTxCtrl3_u;

/*REG_MAC_BFM_MBFM_BFEE_TX_CTRL4 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeNssDegradationFactor0 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x0, access type: RW
		uint32 swBfeeNssDegradationFactor1 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x0, access type: RW
		uint32 swBfeeNssDegradationFactor2 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x0, access type: RW
		uint32 swBfeeNssDegradationFactor3 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x0, access type: RW
		uint32 swBfeeNssDegradationFactor4 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeNssDegradationFactor5 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeNssDegradationFactor6 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeNssDegradationFactor7 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeNssDegradationFactor8 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeNssDegradationFactor9 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegMacBfmMbfmBfeeTxCtrl4_u;

/*REG_MAC_BFM_MBFM_BFEE_TX_CTRL5 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeNssDegradationFactor10 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 swBfeeNssDegradationFactor11 : 3; //set the NSS degradation logic to use. each selection causes a different NSS degradation logic. Value 0 means no NSS degradation., reset value: 0x1, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegMacBfmMbfmBfeeTxCtrl5_u;

/*REG_MAC_BFM_MBFM_HOB_CTRL 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDelHobAddrOvrd : 6; //word address for HOB control interface, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 delHobAddrOvrdPulse : 1; //write 1'b1 to this field to latch the read data from HOB register address sw_del_hob_addr_ovrd. After this SW can read the MBFM_HOB_RD_DATA register, reset value: 0x0, access type: WO
		uint32 reserved1 : 23;
	} bitFields;
} RegMacBfmMbfmHobCtrl_u;

/*REG_MAC_BFM_MBFM_HOB_RD_DATA 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swDelHobLatchedData : 32; //Read value from the HOB control interface, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmHobRdData_u;

/*REG_MAC_BFM_MBFM_BFEE_SW_GEN 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swOverrideRxcBfeeNdpaDetect : 1; //Generate NDPA detection pulse. Use this only in case there is a bug in RXC NDPA indication. , Sequence: write the STA ID & VAP ID while this bit is cleared then write them again while setting this bit, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 swOverrideRxcBfeeStaId : 9; //write the STA ID for the SW NDPA mechanism, reset value: 0x0, access type: RW
		uint32 reserved1 : 7;
		uint32 swOverrideRxcBfeeVapId : 5; //write the VAP ID for the SW NDPA mechanism, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 swOverrideRxcBfeeDialogTokenP : 1; //write thePHY mode in the sounding dialog token for the SW NDPA mechanism. 1'b0 - VHT 1'b1 - HE, reset value: 0x0, access type: RW
		uint32 reserved3 : 3;
	} bitFields;
} RegMacBfmMbfmBfeeSwGen_u;

/*REG_MAC_BFM_MBFM_BFEE_CNTR_CTRL 0x1B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeCntrNdpClr : 1; //Clear the Bfee counter that counts received NDPs, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 swBfeeCntrNdpaClr : 1; //Clear the Bfee counter that counts received NDPAs, reset value: 0x0, access type: RW
		uint32 reserved1 : 3;
		uint32 swBfeeCntrNotSupportClr : 1; //Clear the Bfee counter that counts all the "Not supported" stations, reset value: 0x0, access type: RW
		uint32 reserved2 : 3;
		uint32 swBfeeCntrPhyErrClr : 1; //Clear the Bfee counter that counts the Bfee phy error sequences, reset value: 0x0, access type: RW
		uint32 reserved3 : 19;
	} bitFields;
} RegMacBfmMbfmBfeeCntrCtrl_u;

/*REG_MAC_BFM_MBFM_BFEE_CNTR_VAL 0x1BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swBfeeCntrNdpVal : 8; //counter that counts received NDPs                   , reset value: 0x0, access type: RO
		uint32 swBfeeCntrNdpaVal : 8; //counter that counts received NDPAs                  , reset value: 0x0, access type: RO
		uint32 swBfeeCntrNotSupportVal : 8; //counter that counts all the "Not supported" stations, reset value: 0x0, access type: RO
		uint32 swBfeeCntrPhyErrVal : 8; //counter that counts the Bfee phy error sequences    , reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfmMbfmBfeeCntrVal_u;

/*REG_MAC_BFM_MBFM_ISR_CTRL 0x1D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mbfmGlobalErrIrq : 1; //Error Interrupt from MBFM, reset value: 0x0, access type: RO
		uint32 bfRptAutoReplyAbortPhyModeIrq : 1; //Error Interrupt from MBFM for the case that TX phy mode disables Bfee transmit, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 mbfmGlobalErrIrqCause : 10; //Cause interrupt: , bit[0] - Error fifo overflow        , bit[1] - BFee length 0              , bit[2] - BFer authentication failed , bit[3] - BFee NTX error             , bit[4] - BFee TX phy_mode error     , bit[5] - HT MCS&NSS 32 or HE DCM    , bit[6] - BFer general error , bit[7] - BFee unexpected RXC access , bit[8] - BFee lost NDP, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 mbfmGlobalErrIrqEn : 10; //enable interrupts, reset value: 0x3ff, access type: RW
		uint32 reserved2 : 6;
	} bitFields;
} RegMacBfmMbfmIsrCtrl_u;

/*REG_MAC_BFM_MBFM_ISR_CTRL_CLR 0x1D4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 1;
		uint32 bfRptAutoReplyAbortPhyModeIrqClr : 1; //Clear the BFee IRQ. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 reserved1 : 2;
		uint32 mbfmGlobalErrIrq0Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq1Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq2Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq3Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq4Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq5Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq6Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq7Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq8Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 mbfmGlobalErrIrq9Clr : 1; //Clear the main IRQ cause. This is a pulse signal, write 1'b1 to this field to clear the interrupt, reset value: 0x0, access type: WO
		uint32 reserved2 : 18;
	} bitFields;
} RegMacBfmMbfmIsrCtrlClr_u;

/*REG_MAC_BFM_MBFM_SPARE_REG1 0x1F0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareField0 : 32; //spare, reset value: 0xFF000000, access type: RW
	} bitFields;
} RegMacBfmMbfmSpareReg1_u;

/*REG_MAC_BFM_MBFM_SPARE_REG2 0x1F4 */
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

/*REG_MAC_BFM_MBFM_SPARE_REG3 0x1F8 */
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
