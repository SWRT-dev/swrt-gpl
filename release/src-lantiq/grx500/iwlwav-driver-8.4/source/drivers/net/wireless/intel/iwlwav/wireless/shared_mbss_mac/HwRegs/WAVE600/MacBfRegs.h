
/***********************************************************************************
File:				MacBfRegs.h
Module:				MacBf
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MAC_BF_REGS_H_
#define _MAC_BF_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MAC_BF_BASE_ADDRESS                             MEMORY_MAP_UNIT_14_BASE_ADDRESS
#define	REG_MAC_BF_BF_ENABLE                                         (MAC_BF_BASE_ADDRESS + 0x0)
#define	REG_MAC_BF_BF_DB_BASE_ADDR                                   (MAC_BF_BASE_ADDRESS + 0x4)
#define	REG_MAC_BF_TX_BFEE_BUFFER_ADDR                               (MAC_BF_BASE_ADDRESS + 0x8)
#define	REG_MAC_BF_BFER_MAX_NR_SUPPORTED                             (MAC_BF_BASE_ADDRESS + 0xC)
#define	REG_MAC_BF_STA_DB_BF_WORDS_OFFSET                            (MAC_BF_BASE_ADDRESS + 0x10)
#define	REG_MAC_BF_AGING_CHECK_TIMER_THRESHOLD                       (MAC_BF_BASE_ADDRESS + 0x14)
#define	REG_MAC_BF_AGING_TIMER_THRESHOLD0                            (MAC_BF_BASE_ADDRESS + 0x18)
#define	REG_MAC_BF_AGING_TIMER_THRESHOLD1                            (MAC_BF_BASE_ADDRESS + 0x1C)
#define	REG_MAC_BF_AGING_TIMER                                       (MAC_BF_BASE_ADDRESS + 0x20)
#define	REG_MAC_BF_BFEE_MAC_HDR_LENGTH                               (MAC_BF_BASE_ADDRESS + 0x24)
#define	REG_MAC_BF_BF_DEBUG_MODE                                     (MAC_BF_BASE_ADDRESS + 0x28)
#define	REG_MAC_BF_BF_SM_DEBUG_REGISTER                              (MAC_BF_BASE_ADDRESS + 0x2C)
#define	REG_MAC_BF_BF_INDEXES_DEBUG_REGISTER                         (MAC_BF_BASE_ADDRESS + 0x30)
#define	REG_MAC_BF_BF_AGING_DEBUG_REGISTER                           (MAC_BF_BASE_ADDRESS + 0x34)
#define	REG_MAC_BF_NDP_HEADER_FRAME_CONTROL                          (MAC_BF_BASE_ADDRESS + 0x38)
#define	REG_MAC_BF_MAX_NDPA_NDP_TIMER                                (MAC_BF_BASE_ADDRESS + 0x3C)
#define	REG_MAC_BF_BF_INDEXES_TABLE_BASE_ADDR                        (MAC_BF_BASE_ADDRESS + 0x40)
#define	REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX0                     (MAC_BF_BASE_ADDRESS + 0x44)
#define	REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX1                     (MAC_BF_BASE_ADDRESS + 0x48)
#define	REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX2                     (MAC_BF_BASE_ADDRESS + 0x4C)
#define	REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX3                     (MAC_BF_BASE_ADDRESS + 0x50)
#define	REG_MAC_BF_BF_RPT_AUTO_REPLY_ABORT_PHY_MODE                  (MAC_BF_BASE_ADDRESS + 0x54)
#define	REG_MAC_BF_BF_RPT_AUTO_REPLY_ABORT_PHY_MODE_RD               (MAC_BF_BASE_ADDRESS + 0x58)
#define	REG_MAC_BF_BF_RPT_AUTO_REPLY_ABORT_PHY_MODE_CLR              (MAC_BF_BASE_ADDRESS + 0x5C)
#define	REG_MAC_BF_BF_RPT_AUTO_REPLY_MCS_DEGRADATION_VAL_VAP0TO7     (MAC_BF_BASE_ADDRESS + 0x60)
#define	REG_MAC_BF_BF_RPT_AUTO_REPLY_MCS_DEGRADATION_VAL_VAP8TO15    (MAC_BF_BASE_ADDRESS + 0x64)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR0_OVERRIDE_MASK                 (MAC_BF_BASE_ADDRESS + 0x68)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR1_OVERRIDE_MASK                 (MAC_BF_BASE_ADDRESS + 0x6C)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR2_OVERRIDE_MASK                 (MAC_BF_BASE_ADDRESS + 0x70)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR3_OVERRIDE_MASK                 (MAC_BF_BASE_ADDRESS + 0x74)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR0_OVERRIDE_VALUE                (MAC_BF_BASE_ADDRESS + 0x78)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR1_OVERRIDE_VALUE                (MAC_BF_BASE_ADDRESS + 0x7C)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR2_OVERRIDE_VALUE                (MAC_BF_BASE_ADDRESS + 0x80)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR3_OVERRIDE_VALUE                (MAC_BF_BASE_ADDRESS + 0x84)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR0                               (MAC_BF_BASE_ADDRESS + 0x88)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR1                               (MAC_BF_BASE_ADDRESS + 0x8C)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR2                               (MAC_BF_BASE_ADDRESS + 0x90)
#define	REG_MAC_BF_BFEE_TX_REPORT_TCR3                               (MAC_BF_BASE_ADDRESS + 0x94)
#define	REG_MAC_BF_BF_LOGGER_CONTROL                                 (MAC_BF_BASE_ADDRESS + 0x98)
#define	REG_MAC_BF_BF_LOGGER_ACTIVE                                  (MAC_BF_BASE_ADDRESS + 0x9C)
#define	REG_MAC_BF_BF_DB_UPDATE_CTR0                                 (MAC_BF_BASE_ADDRESS + 0xA0)
#define	REG_MAC_BF_BF_DB_UPDATE_CTR1                                 (MAC_BF_BASE_ADDRESS + 0xA4)
#define	REG_MAC_BF_BF_DB_UPDATE_CTR_CLR                              (MAC_BF_BASE_ADDRESS + 0xA8)
#define	REG_MAC_BF_BFER_RX_BF_RPT_PARAMS                             (MAC_BF_BASE_ADDRESS + 0xAC)
#define	REG_MAC_BF_BFER_RX_BF_RPT_SNR                                (MAC_BF_BASE_ADDRESS + 0xB0)
#define	REG_MAC_BF_MU_BF_EN                                          (MAC_BF_BASE_ADDRESS + 0x100)
#define	REG_MAC_BF_MU_BF_EXPECTED_NC                                 (MAC_BF_BASE_ADDRESS + 0x104)
#define	REG_MAC_BF_MU_BF_EXPECTED_NR_BW                              (MAC_BF_BASE_ADDRESS + 0x108)
#define	REG_MAC_BF_MU_BF_CHECKS_EN                                   (MAC_BF_BASE_ADDRESS + 0x10C)
#define	REG_MAC_BF_MU_BF_DEBUG_MODE                                  (MAC_BF_BASE_ADDRESS + 0x110)
#define	REG_MAC_BF_MU_BF_SM_STATUS                                   (MAC_BF_BASE_ADDRESS + 0x114)
#define	REG_MAC_BF_MU_BF_MIMO_CTRL                                   (MAC_BF_BASE_ADDRESS + 0x118)
#define	REG_MAC_BF_MU_BF_FAIL_REPORT                                 (MAC_BF_BASE_ADDRESS + 0x120)
#define	REG_MAC_BF_MU_BF_AVG_SNR                                     (MAC_BF_BASE_ADDRESS + 0x124)
#define	REG_MAC_BF_MU_BF_LOGGER_CFG                                  (MAC_BF_BASE_ADDRESS + 0x128)
#define	REG_MAC_BF_MU_BF_LOGGER_BUSY                                 (MAC_BF_BASE_ADDRESS + 0x12C)
#define	REG_MAC_BF_MU_BFEE_MAX_NC_INDEX_SUPPORTED                    (MAC_BF_BASE_ADDRESS + 0x130)
#define	REG_MAC_BF_VAP_DB_BSSID_AID_4B_OFFSET                        (MAC_BF_BASE_ADDRESS + 0x134)
#define	REG_MAC_BF_MAX_TIMER_CLR_PAC_PHY_RX_MU_NDP                   (MAC_BF_BASE_ADDRESS + 0x138)
#define	REG_MAC_BF_MAC_MU_GOOD_REPORT_COUNTER                        (MAC_BF_BASE_ADDRESS + 0x13C)
#define	REG_MAC_BF_MAC_MU_BAD_REPORT_COUNTER                         (MAC_BF_BASE_ADDRESS + 0x140)
#define	REG_MAC_BF_MAC_MU_REPORT_COUNTER_CLR                         (MAC_BF_BASE_ADDRESS + 0x144)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MAC_BF_BF_ENABLE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfEnable : 1; //BF enable, reset value: 0x0, access type: RW
		uint32 agingNonstandardBfEnable : 1; //Aging Routine enable of non-standard STAs, reset value: 0x0, access type: RW
		uint32 agingStandardBfEnable : 1; //Aging Routine enable of standard STAs, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegMacBfBfEnable_u;

/*REG_MAC_BF_BF_DB_BASE_ADDR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfDbBaseAddr : 24; //BF DB base addr, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfDbBaseAddr_u;

/*REG_MAC_BF_TX_BFEE_BUFFER_ADDR 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txBfeeBufferAddr : 24; //Tx Bfee buffer addr, reset value: 0x181000, access type: RW
		uint32 reserved0 : 7;
		uint32 txBfeeBufferPointsTxCyclicBuf : 1; //Tx Bfee buffer points to Tx circular buffer, reset value: 0x1, access type: RW
	} bitFields;
} RegMacBfTxBfeeBufferAddr_u;

/*REG_MAC_BF_BFER_MAX_NR_SUPPORTED 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bferSuMaxNrSupported204080 : 3; //Max Nr supported in BW 20/40/80MHz in case of SU BFer, reset value: 0x3, access type: RW
		uint32 bferSuMaxNrSupported160 : 3; //Max Nr supported in BW 160MHz in case of SU BFer, reset value: 0x0, access type: RW
		uint32 bferMuMaxNrSupported204080 : 3; //Max Nr supported in BW 20/40/80MHz in case of MU BFer, reset value: 0x3, access type: RW
		uint32 bferMuMaxNrSupported160 : 3; //Max Nr supported in BW 160MHz in case of MU BFer, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegMacBfBferMaxNrSupported_u;

/*REG_MAC_BF_STA_DB_BF_WORDS_OFFSET 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staDbBfWordsOffset : 5; //STA DB BF words offset, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegMacBfStaDbBfWordsOffset_u;

/*REG_MAC_BF_AGING_CHECK_TIMER_THRESHOLD 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agingMaxTimerThreshold : 16; //Aging max timer threshold, resolution of 256[us], reset value: 0x8000, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacBfAgingCheckTimerThreshold_u;

/*REG_MAC_BF_AGING_TIMER_THRESHOLD0 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agingTimerThresholdFast : 16; //Aging timer threshold fast, resolution of 256[us], reset value: 0x28, access type: RW
		uint32 agingTimerThresholdMedium1 : 16; //Aging timer threshold medium1, resolution of 256[us], reset value: 0x28, access type: RW
	} bitFields;
} RegMacBfAgingTimerThreshold0_u;

/*REG_MAC_BF_AGING_TIMER_THRESHOLD1 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agingTimerThresholdMedium2 : 16; //Aging timer threshold medium2, resolution of 256[us], reset value: 0x28, access type: RW
		uint32 agingTimerThresholdSlow : 16; //Aging timer threshold slow, resolution of 256[us], reset value: 0x28, access type: RW
	} bitFields;
} RegMacBfAgingTimerThreshold1_u;

/*REG_MAC_BF_AGING_TIMER 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agingTimer : 24; //Aging timer, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfAgingTimer_u;

/*REG_MAC_BF_BFEE_MAC_HDR_LENGTH 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMacHdrLength : 6; //tx BF frame report mac hdr length, reset value: 0x1c, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegMacBfBfeeMacHdrLength_u;

/*REG_MAC_BF_BF_DEBUG_MODE 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyRxDebugMode : 1; //PHY Rx BF debug mode, reset value: 0x0, access type: RW
		uint32 phyDebugModeOverrideMacPhySignals : 1; //PHY debug mode override MAC-PHY signals, reset value: 0x0, access type: RW
		uint32 reserved0 : 6;
		uint32 phyDebugStaTxMaxNss : 2; //phy debug override sta tx max nss, reset value: 0x0, access type: RW
		uint32 phyDebugRxMuNdp : 1; //phy debug override Rx MU NDP signal, reset value: 0x0, access type: RW
		uint32 reserved1 : 21;
	} bitFields;
} RegMacBfBfDebugMode_u;

/*REG_MAC_BF_BF_SM_DEBUG_REGISTER 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 groupingSrcSm : 2; //Grouping Source SM, reset value: 0x0, access type: RO
		uint32 groupingDestSm : 3; //Grouping Destination SM, reset value: 0x0, access type: RO
		uint32 phyBfMatricesSm : 3; //PHY BF matrices SM, reset value: 0x0, access type: RO
		uint32 agingSm : 4; //Aging SM, reset value: 0x0, access type: RO
		uint32 bfMainRxSm : 5; //BF Main Rx SM, reset value: 0x0, access type: RO
		uint32 ndpaNdpSm : 2; //NDPA NDP SM, reset value: 0x0, access type: RO
		uint32 bfSmcArbSm : 2; //BF SMC arbiter state machine, reset value: 0x0, access type: RO
		uint32 bfeeRdTcrsSm : 3; //Beamformee read TCRs for auto reply  BF report state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfSmDebugRegister_u;

/*REG_MAC_BF_BF_INDEXES_DEBUG_REGISTER 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staDbIndex : 7; //STA DB index, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 bfDbTempIndex : 8; //BF DB temp index, reset value: 0x0, access type: RO
		uint32 vapDbIndex : 4; //VAP DB index, reset value: 0x0, access type: RO
		uint32 reserved1 : 12;
	} bitFields;
} RegMacBfBfIndexesDebugRegister_u;

/*REG_MAC_BF_BF_AGING_DEBUG_REGISTER 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agingStaDbIndex : 8; //Aging STA DB index, reset value: 0x0, access type: RO
		uint32 agingBfDbIndex : 9; //Aging BF DB index, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 agingTimerExpired : 1; //Aging timer expired, reset value: 0x0, access type: RO
		uint32 tsfAgingThresholdExpired : 1; //TSF Aging threshold expired, reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
	} bitFields;
} RegMacBfBfAgingDebugRegister_u;

/*REG_MAC_BF_NDP_HEADER_FRAME_CONTROL 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ndpHeaderFrameControl : 16; //NDP header frame control, reset value: 0x4, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegMacBfNdpHeaderFrameControl_u;

/*REG_MAC_BF_MAX_NDPA_NDP_TIMER 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxNdpaNdpTimer : 8; //Max NDPA NDP timer, resolution of [us], reset value: 0xb0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacBfMaxNdpaNdpTimer_u;

/*REG_MAC_BF_BF_INDEXES_TABLE_BASE_ADDR 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfIndexesTableBaseAddr : 24; //BF indexes table base addr, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfIndexesTableBaseAddr_u;

/*REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX0 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx0CtsNeverUpdate : 1; //idx0 CTS never update, reset value: 0x0, access type: RW
		uint32 idx0CtsVerifyBwToDb : 1; //idx0 CTS verify bw to db, reset value: 0x0, access type: RW
		uint32 idx0CtsVerifyNssToDb : 1; //idx0 CTS verify nss to db, reset value: 0x0, access type: RW
		uint32 idx0CtsVerifyNssToMaxTxNss : 1; //idx0 CTS verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx0CtsAgingIgnoreBwDbRules : 1; //idx0 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx0CtsAgingIgnoreNssDbRules : 1; //idx0 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx0CtsVerifyBwToBwScrambler : 1; //idx0 CTS verify bw to bw scrambler, reset value: 0x0, access type: RW
		uint32 idx0CtsVerifyBwScramblerToDb : 1; //idx0 CTS verify bw scrambler to db, reset value: 0x0, access type: RW
		uint32 idx0AckBaNeverUpdate : 1; //idx0 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx0AckBaVerifyBwToDb : 1; //idx0 ACK/BA verify bw to db, reset value: 0x0, access type: RW
		uint32 idx0AckBaVerifyNssToDb : 1; //idx0 ACK/BA verify nss to db, reset value: 0x0, access type: RW
		uint32 idx0AckBaVerifyNssToMaxTxNss : 1; //idx0 ACK/BA verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx0AckBaAgingIgnoreBwDbRules : 1; //idx0 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx0AckBaAgingIgnoreNssDbRules : 1; //idx0 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx0OtherRxNeverUpdate : 1; //idx0 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx0OtherRxVerifyBwToDb : 1; //idx0 other Rx verify bw to db, reset value: 0x0, access type: RW
		uint32 idx0OtherRxVerifyNssToDb : 1; //idx0 other Rx verify nss to db, reset value: 0x0, access type: RW
		uint32 idx0OtherRxVerifyNssToMaxTxNss : 1; //idx0 other Rx verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx0OtherRxAgingIgnoreBwDbRules : 1; //idx0 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx0OtherRxAgingIgnoreNssDbRules : 1; //idx0 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfBfNonstandardImpRulesIdx0_u;

/*REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX1 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx1CtsNeverUpdate : 1; //idx1 CTS never update, reset value: 0x0, access type: RW
		uint32 idx1CtsVerifyBwToDb : 1; //idx1 CTS verify bw to db, reset value: 0x0, access type: RW
		uint32 idx1CtsVerifyNssToDb : 1; //idx1 CTS verify nss to db, reset value: 0x0, access type: RW
		uint32 idx1CtsVerifyNssToMaxTxNss : 1; //idx1 CTS verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx1CtsAgingIgnoreBwDbRules : 1; //idx1 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx1CtsAgingIgnoreNssDbRules : 1; //idx1 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx1CtsVerifyBwToBwScrambler : 1; //idx1 CTS verify bw to bw scrambler, reset value: 0x0, access type: RW
		uint32 idx1CtsVerifyBwScramblerToDb : 1; //idx1 CTS verify bw scrambler to db, reset value: 0x0, access type: RW
		uint32 idx1AckBaNeverUpdate : 1; //idx1 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx1AckBaVerifyBwToDb : 1; //idx1 ACK/BA verify bw to db, reset value: 0x0, access type: RW
		uint32 idx1AckBaVerifyNssToDb : 1; //idx1 ACK/BA verify nss to db, reset value: 0x0, access type: RW
		uint32 idx1AckBaVerifyNssToMaxTxNss : 1; //idx1 ACK/BA verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx1AckBaAgingIgnoreBwDbRules : 1; //idx1 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx1AckBaAgingIgnoreNssDbRules : 1; //idx1 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx1OtherRxNeverUpdate : 1; //idx1 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx1OtherRxVerifyBwToDb : 1; //idx1 other Rx verify bw to db, reset value: 0x0, access type: RW
		uint32 idx1OtherRxVerifyNssToDb : 1; //idx1 other Rx verify nss to db, reset value: 0x0, access type: RW
		uint32 idx1OtherRxVerifyNssToMaxTxNss : 1; //idx1 other Rx verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx1OtherRxAgingIgnoreBwDbRules : 1; //idx1 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx1OtherRxAgingIgnoreNssDbRules : 1; //idx1 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfBfNonstandardImpRulesIdx1_u;

/*REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX2 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx2CtsNeverUpdate : 1; //idx2 CTS never update, reset value: 0x0, access type: RW
		uint32 idx2CtsVerifyBwToDb : 1; //idx2 CTS verify bw to db, reset value: 0x0, access type: RW
		uint32 idx2CtsVerifyNssToDb : 1; //idx2 CTS verify nss to db, reset value: 0x0, access type: RW
		uint32 idx2CtsVerifyNssToMaxTxNss : 1; //idx2 CTS verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx2CtsAgingIgnoreBwDbRules : 1; //idx2 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx2CtsAgingIgnoreNssDbRules : 1; //idx2 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx2CtsVerifyBwToBwScrambler : 1; //idx2 CTS verify bw to bw scrambler, reset value: 0x0, access type: RW
		uint32 idx2CtsVerifyBwScramblerToDb : 1; //idx2 CTS verify bw scrambler to db, reset value: 0x0, access type: RW
		uint32 idx2AckBaNeverUpdate : 1; //idx2 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx2AckBaVerifyBwToDb : 1; //idx2 ACK/BA verify bw to db, reset value: 0x0, access type: RW
		uint32 idx2AckBaVerifyNssToDb : 1; //idx2 ACK/BA verify nss to db, reset value: 0x0, access type: RW
		uint32 idx2AckBaVerifyNssToMaxTxNss : 1; //idx2 ACK/BA verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx2AckBaAgingIgnoreBwDbRules : 1; //idx2 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx2AckBaAgingIgnoreNssDbRules : 1; //idx2 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx2OtherRxNeverUpdate : 1; //idx2 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx2OtherRxVerifyBwToDb : 1; //idx2 other Rx verify bw to db, reset value: 0x0, access type: RW
		uint32 idx2OtherRxVerifyNssToDb : 1; //idx2 other Rx verify nss to db, reset value: 0x0, access type: RW
		uint32 idx2OtherRxVerifyNssToMaxTxNss : 1; //idx2 other Rx verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx2OtherRxAgingIgnoreBwDbRules : 1; //idx2 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx2OtherRxAgingIgnoreNssDbRules : 1; //idx2 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfBfNonstandardImpRulesIdx2_u;

/*REG_MAC_BF_BF_NONSTANDARD_IMP_RULES_IDX3 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 idx3CtsNeverUpdate : 1; //idx3 CTS never update, reset value: 0x0, access type: RW
		uint32 idx3CtsVerifyBwToDb : 1; //idx3 CTS verify bw to db, reset value: 0x0, access type: RW
		uint32 idx3CtsVerifyNssToDb : 1; //idx3 CTS verify nss to db, reset value: 0x0, access type: RW
		uint32 idx3CtsVerifyNssToMaxTxNss : 1; //idx3 CTS verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx3CtsAgingIgnoreBwDbRules : 1; //idx3 CTS aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx3CtsAgingIgnoreNssDbRules : 1; //idx3 CTS aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 idx3CtsVerifyBwToBwScrambler : 1; //idx3 CTS verify bw to bw scrambler, reset value: 0x0, access type: RW
		uint32 idx3CtsVerifyBwScramblerToDb : 1; //idx3 CTS verify bw scrambler to db, reset value: 0x0, access type: RW
		uint32 idx3AckBaNeverUpdate : 1; //idx3 ACK/BA never update, reset value: 0x0, access type: RW
		uint32 idx3AckBaVerifyBwToDb : 1; //idx3 ACK/BA verify bw to db, reset value: 0x0, access type: RW
		uint32 idx3AckBaVerifyNssToDb : 1; //idx3 ACK/BA verify nss to db, reset value: 0x0, access type: RW
		uint32 idx3AckBaVerifyNssToMaxTxNss : 1; //idx3 ACK/BA verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx3AckBaAgingIgnoreBwDbRules : 1; //idx3 ACK/BA aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx3AckBaAgingIgnoreNssDbRules : 1; //idx3 ACK/BA aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 idx3OtherRxNeverUpdate : 1; //idx3 other Rx never update, reset value: 0x0, access type: RW
		uint32 idx3OtherRxVerifyBwToDb : 1; //idx3 other Rx verify bw to db, reset value: 0x0, access type: RW
		uint32 idx3OtherRxVerifyNssToDb : 1; //idx3 other Rx verify nss to db, reset value: 0x0, access type: RW
		uint32 idx3OtherRxVerifyNssToMaxTxNss : 1; //idx3 other Rx verify nss to max tx nss, reset value: 0x0, access type: RW
		uint32 idx3OtherRxAgingIgnoreBwDbRules : 1; //idx3 other Rx aging ignore bw db rules, reset value: 0x0, access type: RW
		uint32 idx3OtherRxAgingIgnoreNssDbRules : 1; //idx3 other Rx aging ignore nss db rules, reset value: 0x0, access type: RW
		uint32 reserved1 : 10;
	} bitFields;
} RegMacBfBfNonstandardImpRulesIdx3_u;

/*REG_MAC_BF_BF_RPT_AUTO_REPLY_ABORT_PHY_MODE 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAutoReplyAbortPhyMode11Ag : 1; //Abort auto reply of BF report in case the PHY mode in the TCRs is 11a/g, reset value: 0x1, access type: RW
		uint32 bfRptAutoReplyAbortPhyMode11B : 1; //Abort auto reply of BF report in case the PHY mode in the TCRs is 11b, reset value: 0x1, access type: RW
		uint32 bfRptAutoReplyAbortPhyMode11N : 1; //Abort auto reply of BF report in case the PHY mode in the TCRs is 11n, reset value: 0x0, access type: RW
		uint32 bfRptAutoReplyAbortPhyMode11Ac : 1; //Abort auto reply of BF report in case the PHY mode in the TCRs is 11ac, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfBfRptAutoReplyAbortPhyMode_u;

/*REG_MAC_BF_BF_RPT_AUTO_REPLY_ABORT_PHY_MODE_RD 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAutoReplyAbortPhyModeCtr : 8; //Counter of BF report auto reply abort due to PHY mode, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 bfRptAutoReplyAbortPhyModeIrq : 1; //IRQ of BF report auto reply abort due to PHY mode, reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
		uint32 bfRptAutoReplyAbortPhyModeStaIdx : 7; //Sample the STA index on BF report auto reply abort due to PHY mode, reset value: 0x0, access type: RO
		uint32 reserved2 : 9;
	} bitFields;
} RegMacBfBfRptAutoReplyAbortPhyModeRd_u;

/*REG_MAC_BF_BF_RPT_AUTO_REPLY_ABORT_PHY_MODE_CLR 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAutoReplyAbortPhyModeCtrClr : 1; //Clear Counter of BF report auto reply abort due to PHY mode, by write '1', reset value: 0x0, access type: WO
		uint32 bfRptAutoReplyAbortPhyModeIrqClr : 1; //Clear IRQ of BF report auto reply abort due to PHY mode, by write '1', reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacBfBfRptAutoReplyAbortPhyModeClr_u;

/*REG_MAC_BF_BF_RPT_AUTO_REPLY_MCS_DEGRADATION_VAL_VAP0TO7 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAutoReplyMcsDegradationValVap0To7 : 32; //BF report auto reply MCS degradation value, for vap indexes 0to7, 4 bits per VAP, reset value: 0x0, access type: RW
	} bitFields;
} RegMacBfBfRptAutoReplyMcsDegradationValVap0To7_u;

/*REG_MAC_BF_BF_RPT_AUTO_REPLY_MCS_DEGRADATION_VAL_VAP8TO15 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfRptAutoReplyMcsDegradationValVap8To15 : 32; //BF report auto reply MCS degradation value, for vap indexes 8to15, 4 bits per VAP, reset value: 0x0, access type: RW
	} bitFields;
} RegMacBfBfRptAutoReplyMcsDegradationValVap8To15_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR0_OVERRIDE_MASK 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr0OverrideMask : 24; //BFee Tx report TCR0 override mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr0OverrideMask_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR1_OVERRIDE_MASK 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr1OverrideMask : 24; //BFee Tx report TCR1 override mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr1OverrideMask_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR2_OVERRIDE_MASK 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr2OverrideMask : 24; //BFee Tx report TCR2 override mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr2OverrideMask_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR3_OVERRIDE_MASK 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr3OverrideMask : 24; //BFee Tx report TCR3 override mask, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr3OverrideMask_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR0_OVERRIDE_VALUE 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr0OverrideValue : 24; //BFee Tx report TCR0 override value, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr0OverrideValue_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR1_OVERRIDE_VALUE 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr1OverrideValue : 24; //BFee Tx report TCR1 override value, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr1OverrideValue_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR2_OVERRIDE_VALUE 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr2OverrideValue : 24; //BFee Tx report TCR2 override value, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr2OverrideValue_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR3_OVERRIDE_VALUE 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr3OverrideValue : 24; //BFee Tx report TCR3 override value, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr3OverrideValue_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR0 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr0 : 24; //BFee Tx report TCR0, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr0_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR1 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr1 : 24; //BFee Tx report TCR1, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr1_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR2 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr2 : 24; //BFee Tx report TCR2, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr2_u;

/*REG_MAC_BF_BFEE_TX_REPORT_TCR3 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeTxReportTcr3 : 24; //BFee Tx report TCR3, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegMacBfBfeeTxReportTcr3_u;

/*REG_MAC_BF_BF_LOGGER_CONTROL 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfNdpaLoggerEn : 1; //BF NDPA logger enable, reset value: 0x0, access type: RW
		uint32 bfPhyNdpLoggerEn : 1; //BF PHY NDP logger enable, reset value: 0x0, access type: RW
		uint32 bfImplicitLoggerEn : 1; //BF Implicit logger enable, reset value: 0x0, access type: RW
		uint32 bfReportLoggerEn : 1; //BF report logger enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 bfNdpaLoggerPriority : 2; //BF logger report priority in case of Rx NDPA, reset value: 0x0, access type: RW
		uint32 bfPhyNdpLoggerPriority : 2; //BF logger report priority in case of PHY NDP, reset value: 0x0, access type: RW
		uint32 bfImplicitLoggerPriority : 2; //BF logger report priority in case of Implicit BF, reset value: 0x0, access type: RW
		uint32 bfReportLoggerPriority : 2; //BF logger report priority in case of Rx BF report, reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegMacBfBfLoggerControl_u;

/*REG_MAC_BF_BF_LOGGER_ACTIVE 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfLoggerActive : 1; //BF logger active, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacBfBfLoggerActive_u;

/*REG_MAC_BF_BF_DB_UPDATE_CTR0 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfExplicitBferDbUpdateCtr : 16; //Counter of number BF DB updates due to Rx BF report (as BFer), reset value: 0x0, access type: RO
		uint32 bfExplicitBfeeHtTempDbUpdateCtr : 16; //Counter of number BF temporary entry DB updates due to Rx HT PHY NDP (as BFee), reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfBfDbUpdateCtr0_u;

/*REG_MAC_BF_BF_DB_UPDATE_CTR1 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfImplicitDbUpdateCtr : 16; //Counter of number BF DB updates due to BF implicit, reset value: 0x0, access type: RO
		uint32 bfExplicitBfeeVhtBufUpdateCtr : 16; //Counter of number BFee buffer updates due to Rx VHT PHY NDP (as BFee), reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfBfDbUpdateCtr1_u;

/*REG_MAC_BF_BF_DB_UPDATE_CTR_CLR 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfExplicitBferDbUpdateCtrClr : 1; //Clear BF explicit bfer DB update counter, by write '1', reset value: 0x0, access type: WO
		uint32 bfExplicitBfeeHtTempDbUpdateCtrClr : 1; //Clear BF explicit HT bfee temporary DB update counter, by write '1', reset value: 0x0, access type: WO
		uint32 bfImplicitDbUpdateCtrClr : 1; //Clear BF implicit DB update counter, by write '1', reset value: 0x0, access type: WO
		uint32 bfExplicitBfeeVhtBufUpdateCtrClr : 1; //Clear BF explicit VHT bfee buffer update counter, by write '1', reset value: 0x0, access type: WO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfBfDbUpdateCtrClr_u;

/*REG_MAC_BF_BFER_RX_BF_RPT_PARAMS 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxBfRptVhtHtN : 1; //BF report PHY mode based on Action code and category: 1-VHT, 0-HT, reset value: 0x0, access type: RO
		uint32 rxBfRptMimoNc : 2; //Nc from BF report MIMO control, reset value: 0x0, access type: RO
		uint32 rxBfRptMfb : 7; //BF report MCS feedback, reset value: 0x0, access type: RO
		uint32 rxBfRptMfbValid : 1; //BF report MCS feeback valid indication, reset value: 0x0, access type: RO
		uint32 rxBfRptMimoBw : 2; //BW from BF report MIMO control, reset value: 0x0, access type: RO
		uint32 rxBfRptOrderBit : 1; //BF report order bit, reset value: 0x0, access type: RO
		uint32 rxBfRptHtcVhtHtN : 1; //BF report HTC VHT/HT format, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
		uint32 rxBfRptMimoCtrl : 16; //BF report MIMO control, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfBferRxBfRptParams_u;

/*REG_MAC_BF_BFER_RX_BF_RPT_SNR 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bferRxBfRptSnr : 32; //BF report SNR 0-3, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfBferRxBfRptSnr_u;

/*REG_MAC_BF_MU_BF_EN 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBfEn : 1; //MU BF module enable, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegMacBfMuBfEn_u;

/*REG_MAC_BF_MU_BF_EXPECTED_NC 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 expectedNc : 3; //Expected Nc value, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegMacBfMuBfExpectedNc_u;

/*REG_MAC_BF_MU_BF_EXPECTED_NR_BW 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 expectedNr : 3; //Expected Nr value, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 expectedChBw : 2; //Expected channel bandwidth value, reset value: 0x0, access type: RW
		uint32 reserved1 : 26;
	} bitFields;
} RegMacBfMuBfExpectedNrBw_u;

/*REG_MAC_BF_MU_BF_CHECKS_EN 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 expectMuBfReportCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 frameControlTypeCheckEn : 1; //BFee Tx report TCR1 override value, reset value: 0x1, access type: RW
		uint32 frameControlSubtypeCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 frameControlProtectedframeCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 muCategoryCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 muVhtActionCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 ncCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 nrCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 chBwCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 channelFeedbackTypeCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 firstFeedbackCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 remainFeedbackCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 frameTooLongCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 frameTooShortCheckEn : 1; //no description, reset value: 0x1, access type: RW
		uint32 reserved0 : 18;
	} bitFields;
} RegMacBfMuBfChecksEn_u;

/*REG_MAC_BF_MU_BF_DEBUG_MODE 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 debugBfIndex : 4; //BF index for debug mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
		uint32 debugModeEn : 1; //Debug mode enable. Used to insert debug BF index value, reset value: 0x0, access type: RW
	} bitFields;
} RegMacBfMuBfDebugMode_u;

/*REG_MAC_BF_MU_BF_SM_STATUS 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBfMainSm : 4; //Main SM of module, reset value: 0x0, access type: RO
		uint32 dataRdWrSm : 4; //Data read from SMC and write to Phy SM, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegMacBfMuBfSmStatus_u;

/*REG_MAC_BF_MU_BF_MIMO_CTRL 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mimoCtrl : 16; //MIMO control fields as sent to Phy., reset value: 0x0, access type: RO
		uint32 mimoCtrlValid : 1; //Valid indication of mimo_ctrl field., reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegMacBfMuBfMimoCtrl_u;

/*REG_MAC_BF_MU_BF_FAIL_REPORT 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBfFail : 4; //MU BF report fail. Bit per UP. Values are valid until next RX., reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegMacBfMuBfFailReport_u;

/*REG_MAC_BF_MU_BF_AVG_SNR 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 avgSnr : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegMacBfMuBfAvgSnr_u;

/*REG_MAC_BF_MU_BF_LOGGER_CFG 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBfLoggerPriority : 2; //no description, reset value: 0x0, access type: RW
		uint32 muBfLoggerEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegMacBfMuBfLoggerCfg_u;

/*REG_MAC_BF_MU_BF_LOGGER_BUSY 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBfLoggerBusy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegMacBfMuBfLoggerBusy_u;

/*REG_MAC_BF_MU_BFEE_MAX_NC_INDEX_SUPPORTED 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 muBfeeMaxNcIndexSupported : 2; //MU BFee - max Nc index. , Used to limit the NDPA STA info Nc index., reset value: 0x1, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegMacBfMuBfeeMaxNcIndexSupported_u;

/*REG_MAC_BF_VAP_DB_BSSID_AID_4B_OFFSET 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 vapDbBssidAid4BOffset : 8; //Offset of BSSID MAC address and AID in VAP DB, resolution of 4 bytes., reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacBfVapDbBssidAid4BOffset_u;

/*REG_MAC_BF_MAX_TIMER_CLR_PAC_PHY_RX_MU_NDP 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxTimerClrPacPhyRxMuNdp : 8; //Maximum timer to clear pac_phy_rx_mu_ndp signal. , The timer starts count at the start of Rx ready negation. , Resolution of [us], reset value: 0x1a, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegMacBfMaxTimerClrPacPhyRxMuNdp_u;

/*REG_MAC_BF_MAC_MU_GOOD_REPORT_COUNTER 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 goodReportCount : 16; //no description, reset value: 0x0, access type: RO
		uint32 goodReportCountOverflow : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegMacBfMacMuGoodReportCounter_u;

/*REG_MAC_BF_MAC_MU_BAD_REPORT_COUNTER 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 badReportCount : 16; //no description, reset value: 0x0, access type: RO
		uint32 badReportCountOverflow : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegMacBfMacMuBadReportCounter_u;

/*REG_MAC_BF_MAC_MU_REPORT_COUNTER_CLR 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 goodReportPulseClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 badReportPulseClr : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegMacBfMacMuReportCounterClr_u;



#endif // _MAC_BF_REGS_H_
