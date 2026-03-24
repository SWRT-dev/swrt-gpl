
/***********************************************************************************
File:				HobRegs.h
Module:				Hob
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HOB_REGS_H_
#define _HOB_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HOB_BASE_ADDRESS                             MEMORY_MAP_UNIT_9900000_BASE_ADDRESS
#define	REG_HOB_CF_ACK_SFR                                     (HOB_BASE_ADDRESS + 0x10)
#define	REG_HOB_LEGACY_TSF_FIELD_SFR_LOW                       (HOB_BASE_ADDRESS + 0x14)
#define	REG_HOB_LEGACY_TSF_FIELD_SFR_HIGH                      (HOB_BASE_ADDRESS + 0x18)
#define	REG_HOB_TXOP_JUMP_TABLE_BASE                           (HOB_BASE_ADDRESS + 0x20)
#define	REG_HOB_AUTO_REPLY_DUR                                 (HOB_BASE_ADDRESS + 0x50)
#define	REG_HOB_TSF_FIELD_SFR_LOW                              (HOB_BASE_ADDRESS + 0x54)
#define	REG_HOB_TSF_FIELD_SFR_HIGH                             (HOB_BASE_ADDRESS + 0x58)
#define	REG_HOB_TSF_FIELD_SFR1_LOW                             (HOB_BASE_ADDRESS + 0x5C)
#define	REG_HOB_TSF_FIELD_SFR1_HIGH                            (HOB_BASE_ADDRESS + 0x60)
#define	REG_HOB_TSF_FIELD_SFR2_LOW                             (HOB_BASE_ADDRESS + 0x64)
#define	REG_HOB_TSF_FIELD_SFR2_HIGH                            (HOB_BASE_ADDRESS + 0x68)
#define	REG_HOB_TSF_FIELD_SFR3_LOW                             (HOB_BASE_ADDRESS + 0x6C)
#define	REG_HOB_TSF_FIELD_SFR3_HIGH                            (HOB_BASE_ADDRESS + 0x70)
#define	REG_HOB_TSF_FIELD_SFR4_LOW                             (HOB_BASE_ADDRESS + 0xB0)
#define	REG_HOB_TSF_FIELD_SFR4_HIGH                            (HOB_BASE_ADDRESS + 0xB4)
#define	REG_HOB_TSF_OFFSET_INDEX                               (HOB_BASE_ADDRESS + 0xB8)
#define	REG_HOB_TSF_FIELD_SFR5_LOW                             (HOB_BASE_ADDRESS + 0xBC)
#define	REG_HOB_TSF_FIELD_SFR5_HIGH                            (HOB_BASE_ADDRESS + 0xC0)
#define	REG_HOB_AUTO_REPLY_BF_REPORT_DUR                       (HOB_BASE_ADDRESS + 0xC4)
#define	REG_HOB_RX_ADDR1_IDX                                   (HOB_BASE_ADDRESS + 0x104)
#define	REG_HOB_TX_MPDU_DUR_FIELD                              (HOB_BASE_ADDRESS + 0x108)
#define	REG_HOB_TX_ADDR3_SELECT                                (HOB_BASE_ADDRESS + 0x10C)
#define	REG_HOB_TX_NAV_TIMER_AT_TX_EN_RISE_15BITS              (HOB_BASE_ADDRESS + 0x110)
#define	REG_HOB_AUTO_REPLY_DUR_NO_AIR_TIME_15BITS              (HOB_BASE_ADDRESS + 0x114)
#define	REG_HOB_RX_PHY_STATUS_BUFFER                           (HOB_BASE_ADDRESS + 0x118)
#define	REG_HOB_AUTO_REPLY_DUR_BF_REPORT_NO_AIR_TIME_15BITS    (HOB_BASE_ADDRESS + 0x11C)
#define	REG_HOB_BFEE_STA                                       (HOB_BASE_ADDRESS + 0x200)
#define	REG_HOB_LENGTH_WITH_DELIMITER_FCS                      (HOB_BASE_ADDRESS + 0x204)
#define	REG_HOB_LENGTH_WITHOUT_DELIMITER                       (HOB_BASE_ADDRESS + 0x208)
#define	REG_HOB_RD_BF_RPT_VAP_TID0_DB_ADDR                     (HOB_BASE_ADDRESS + 0x20C)
#define	REG_HOB_BFEE_PHY_MODE                                  (HOB_BASE_ADDRESS + 0x210)
#define	REG_HOB_RXC_PHY_NDP_BW_SMP                             (HOB_BASE_ADDRESS + 0x214)
#define	REG_HOB_TX_RECIPE_BASE_ADDR                            (HOB_BASE_ADDRESS + 0x218)
#define	REG_HOB_TXC_SECTION_A_BASE_ADDR                        (HOB_BASE_ADDRESS + 0x21C)
#define	REG_HOB_TXC_SECTION_B_BASE_ADDR                        (HOB_BASE_ADDRESS + 0x220)
#define	REG_HOB_TXC_SECTION_C_BASE_ADDR                        (HOB_BASE_ADDRESS + 0x224)
#define	REG_HOB_TXC_SECTION_A_BYTE_LEN                         (HOB_BASE_ADDRESS + 0x228)
#define	REG_HOB_TXC_SECTION_B_BYTE_LEN                         (HOB_BASE_ADDRESS + 0x22C)
#define	REG_HOB_TXC_SECTION_C_BYTE_LEN                         (HOB_BASE_ADDRESS + 0x230)
#define	REG_HOB_ACTION_AND_CATEGORY                            (HOB_BASE_ADDRESS + 0x234)
#define	REG_HOB_BFEE_GEN_FCS_VALID                             (HOB_BASE_ADDRESS + 0x23C)
#define	REG_HOB_BFEE_MIMO_CONTROL_SU                           (HOB_BASE_ADDRESS + 0x244)
#define	REG_HOB_BFEE_SM                                        (HOB_BASE_ADDRESS + 0x24C)
#define	REG_HOB_BFEE_INVALID_TX_DATA                           (HOB_BASE_ADDRESS + 0x250)
#define	REG_HOB_BFEE_STATIC_INDIACTIONS                        (HOB_BASE_ADDRESS + 0x27C)
#define	REG_HOB_LATCHED_TST_FIME_LOW                           (HOB_BASE_ADDRESS + 0x400)
#define	REG_HOB_LATCHED_TST_FIME_HIGH                          (HOB_BASE_ADDRESS + 0x404)
#define	REG_HOB_TCC_CRC                                        (HOB_BASE_ADDRESS + 0x600)
#define	REG_HOB_HOB_DEL_COUNTER0_EVENT                         (HOB_BASE_ADDRESS + 0x800)
#define	REG_HOB_HOB_DEL_COUNTER1_EVENT                         (HOB_BASE_ADDRESS + 0x804)
#define	REG_HOB_NOP_WAIT_EVENT_VECTOR                          (HOB_BASE_ADDRESS + 0x808)
#define	REG_HOB_TX_DATA_STRUCTURE_ADDR                         (HOB_BASE_ADDRESS + 0x80C)
#define	REG_HOB_DEL2GENRISC_START_TX_INT                       (HOB_BASE_ADDRESS + 0x810)
#define	REG_HOB_PRBS_DATA                                      (HOB_BASE_ADDRESS + 0x818)
#define	REG_HOB_PHY_MODE                                       (HOB_BASE_ADDRESS + 0x81C)
#define	REG_HOB_BF_TX_MAX_THRESHOLD_TIMER                      (HOB_BASE_ADDRESS + 0x820)
#define	REG_HOB_ZLD_VAL                                        (HOB_BASE_ADDRESS + 0x824)
#define	REG_HOB_DEL_TXC_RESP_TIMEOUT_VALUE                     (HOB_BASE_ADDRESS + 0x828)
#define	REG_HOB_RX_LAST_MPDU_IN_PTR                            (HOB_BASE_ADDRESS + 0x82C)
#define	REG_HOB_CTR_DATA                                       (HOB_BASE_ADDRESS + 0x834)
#define	REG_HOB_LOGGER_HOB_INFO                                (HOB_BASE_ADDRESS + 0x838)
#define	REG_HOB_HT_DELIMITER_DATA_IN                           (HOB_BASE_ADDRESS + 0x83C)
#define	REG_HOB_VHT_DELIMITER_DATA_IN                          (HOB_BASE_ADDRESS + 0x840)
#define	REG_HOB_VHT_EOF_DELIMITER_DATA_IN                      (HOB_BASE_ADDRESS + 0x844)
#define	REG_HOB_DELIMITER_DATA_OUT                             (HOB_BASE_ADDRESS + 0x848)
#define	REG_HOB_DELIA_EVENT_THRESHOLD_TIMER                    (HOB_BASE_ADDRESS + 0x84C)
#define	REG_HOB_TIM_DEL_EVENT_TIMER_EXPIRED                    (HOB_BASE_ADDRESS + 0x850)
#define	REG_HOB_TCR_TXOP_DUR_FROM_MAC                          (HOB_BASE_ADDRESS + 0x854)
#define	REG_HOB_DELIA_TX_MPDU_DUR_FIELD                        (HOB_BASE_ADDRESS + 0x85C)
#define	REG_HOB_PAC_PHY_TX_LENGTH_ZERO                         (HOB_BASE_ADDRESS + 0x860)
#define	REG_HOB_DELIMITER_VALUE_PRE_TX_DATA                    (HOB_BASE_ADDRESS + 0x864)
#define	REG_HOB_NUM_OF_USER_TCRS                               (HOB_BASE_ADDRESS + 0x868)
#define	REG_HOB_NUM_OF_USER_TCR_BYTES                          (HOB_BASE_ADDRESS + 0x86C)
#define	REG_HOB_EXPECTED_RCRS                                  (HOB_BASE_ADDRESS + 0x870)
#define	REG_HOB_NUM_OF_USER_RCR_BYTES                          (HOB_BASE_ADDRESS + 0x874)
#define	REG_HOB_TX_CIR_BUF_BASE_ADDR                           (HOB_BASE_ADDRESS + 0x878)
#define	REG_HOB_REG2HOB_GP0                                    (HOB_BASE_ADDRESS + 0x87C)
#define	REG_HOB_REG2HOB_GP1                                    (HOB_BASE_ADDRESS + 0x880)
#define	REG_HOB_TCR_BASE_ADDR1                                 (HOB_BASE_ADDRESS + 0x884)
#define	REG_HOB_TCR_BASE_ADDR2                                 (HOB_BASE_ADDRESS + 0x888)
#define	REG_HOB_RCR_BASE_ADDR                                  (HOB_BASE_ADDRESS + 0x88C)
#define	REG_HOB_SENDER_MPDU_DURATION                           (HOB_BASE_ADDRESS + 0x890)
#define	REG_HOB_ALL_ONES_PADDING                               (HOB_BASE_ADDRESS + 0x894)
#define	REG_HOB_DEL_DESC_FRAME_LEN                             (HOB_BASE_ADDRESS + 0xA00)
#define	REG_HOB_DEL_DESC_FRAME_PTR                             (HOB_BASE_ADDRESS + 0xA04)
#define	REG_HOB_DEL_DESC_DUR_TYPE                              (HOB_BASE_ADDRESS + 0xA08)
#define	REG_HOB_DEL_TCR_FRAME_LEN                              (HOB_BASE_ADDRESS + 0xA0C)
#define	REG_HOB_DEL_MAC_ADDR_RA0                               (HOB_BASE_ADDRESS + 0xA10)
#define	REG_HOB_DEL_MAC_ADDR_RA1                               (HOB_BASE_ADDRESS + 0xA14)
#define	REG_HOB_DEL_MAC_ADDR_TA0                               (HOB_BASE_ADDRESS + 0xA18)
#define	REG_HOB_DEL_MAC_ADDR_TA1                               (HOB_BASE_ADDRESS + 0xA1C)
#define	REG_HOB_AUTO_RESP_TCR_BASE_ADDR                        (HOB_BASE_ADDRESS + 0xA20)
#define	REG_HOB_AUTO_RESP_HE_MU_TCR_BASE_ADDR                  (HOB_BASE_ADDRESS + 0xA24)
#define	REG_HOB_AUTO_RESP_TX_BW                                (HOB_BASE_ADDRESS + 0xA28)
#define	REG_HOB_TIM_IE_CONTROL                                 (HOB_BASE_ADDRESS + 0xC00)
#define	REG_HOB_TIM_IE_BITMAP_CONTROL                          (HOB_BASE_ADDRESS + 0xC04)
#define	REG_HOB_TIM_IE_MPS_GROUP_MSB                           (HOB_BASE_ADDRESS + 0xC08)
#define	REG_HOB_TIM_IE_BITMAP0                                 (HOB_BASE_ADDRESS + 0xC0C)
#define	REG_HOB_TIM_IE_BITMAP1                                 (HOB_BASE_ADDRESS + 0xC10)
#define	REG_HOB_TIM_IE_BITMAP2                                 (HOB_BASE_ADDRESS + 0xC14)
#define	REG_HOB_TIM_IE_BITMAP3                                 (HOB_BASE_ADDRESS + 0xC18)
#define	REG_HOB_TIM_IE_BITMAP4                                 (HOB_BASE_ADDRESS + 0xC1C)
#define	REG_HOB_TIM_IE_BITMAP5                                 (HOB_BASE_ADDRESS + 0xC20)
#define	REG_HOB_TIM_IE_BITMAP6                                 (HOB_BASE_ADDRESS + 0xC24)
#define	REG_HOB_TIM_IE_BITMAP7                                 (HOB_BASE_ADDRESS + 0xC28)
#define	REG_HOB_TIM_IE_BITMAP8                                 (HOB_BASE_ADDRESS + 0xC2C)
#define	REG_HOB_TIM_IE_BITMAP9                                 (HOB_BASE_ADDRESS + 0xC30)
#define	REG_HOB_TIM_IE_BITMAP10                                (HOB_BASE_ADDRESS + 0xC34)
#define	REG_HOB_TIM_IE_HOB_RECIPE_FIELDS0                      (HOB_BASE_ADDRESS + 0xC38)
#define	REG_HOB_TIM_IE_HOB_RECIPE_FIELDS1                      (HOB_BASE_ADDRESS + 0xC3C)
#define	REG_HOB_TIM_IE_HOB_DEBUG_FIELDS0                       (HOB_BASE_ADDRESS + 0xC40)
#define	REG_HOB_TIM_IE_HOB_DEBUG_FIELDS1                       (HOB_BASE_ADDRESS + 0xC44)
#define	REG_HOB_TIM_IE_HOB_DEBUG_FIELDS2                       (HOB_BASE_ADDRESS + 0xC48)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HOB_CF_ACK_SFR 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 12;
		uint32 pmSfr : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 19;
	} bitFields;
} RegHobCfAckSfr_u;

/*REG_HOB_LEGACY_TSF_FIELD_SFR_LOW 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfrLow : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobLegacyTsfFieldSfrLow_u;

/*REG_HOB_LEGACY_TSF_FIELD_SFR_HIGH 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfrHigh : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobLegacyTsfFieldSfrHigh_u;

/*REG_HOB_TXOP_JUMP_TABLE_BASE 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 6;
		uint32 txopJumpTableBase : 18; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 8;
	} bitFields;
} RegHobTxopJumpTableBase_u;

/*REG_HOB_AUTO_REPLY_DUR 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyDur : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobAutoReplyDur_u;

/*REG_HOB_TSF_FIELD_SFR_LOW 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfrLow : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfrLow_u;

/*REG_HOB_TSF_FIELD_SFR_HIGH 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfrHigh : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfrHigh_u;

/*REG_HOB_TSF_FIELD_SFR1_LOW 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr1Low : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr1Low_u;

/*REG_HOB_TSF_FIELD_SFR1_HIGH 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr1High : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr1High_u;

/*REG_HOB_TSF_FIELD_SFR2_LOW 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr2Low : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr2Low_u;

/*REG_HOB_TSF_FIELD_SFR2_HIGH 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr2High : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr2High_u;

/*REG_HOB_TSF_FIELD_SFR3_LOW 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr3Low : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr3Low_u;

/*REG_HOB_TSF_FIELD_SFR3_HIGH 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr3High : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr3High_u;

/*REG_HOB_TSF_FIELD_SFR4_LOW 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr4Low : 32; //no description, reset value: 0x0, access type: RW
	} bitFields;
} RegHobTsfFieldSfr4Low_u;

/*REG_HOB_TSF_FIELD_SFR4_HIGH 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr4High : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr4High_u;

/*REG_HOB_TSF_OFFSET_INDEX 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfOffsetIndex : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegHobTsfOffsetIndex_u;

/*REG_HOB_TSF_FIELD_SFR5_LOW 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr5Low : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr5Low_u;

/*REG_HOB_TSF_FIELD_SFR5_HIGH 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldSfr5High : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTsfFieldSfr5High_u;

/*REG_HOB_AUTO_REPLY_BF_REPORT_DUR 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyBfReportDur : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobAutoReplyBfReportDur_u;

/*REG_HOB_RX_ADDR1_IDX 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxAddr1Idx : 5; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegHobRxAddr1Idx_u;

/*REG_HOB_TX_MPDU_DUR_FIELD 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMpduDurField : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobTxMpduDurField_u;

/*REG_HOB_TX_ADDR3_SELECT 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txAddr3Select : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobTxAddr3Select_u;

/*REG_HOB_TX_NAV_TIMER_AT_TX_EN_RISE_15BITS 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNavTimerAtTxEnRise15Bits : 15; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegHobTxNavTimerAtTxEnRise15Bits_u;

/*REG_HOB_AUTO_REPLY_DUR_NO_AIR_TIME_15BITS 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyDurNoAirTime15Bits : 15; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegHobAutoReplyDurNoAirTime15Bits_u;

/*REG_HOB_RX_PHY_STATUS_BUFFER 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxPhyStatusBufferId : 1; //Rx PHY status buffer ID of the last Rx session, reset value: 0x0, access type: RO
		uint32 rxPhyStatusBufferValid : 1; //Rx PHY status buffer valid of the last Rx session, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobRxPhyStatusBuffer_u;

/*REG_HOB_AUTO_REPLY_DUR_BF_REPORT_NO_AIR_TIME_15BITS 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyDurBfReportNoAirTime15Bits : 15; //Auto-reply BF report duration without the Air time of Tx BF report - used for TCR TXOP duration field, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegHobAutoReplyDurBfReportNoAirTime15Bits_u;

/*REG_HOB_BFEE_STA 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxStaIdxUsr0Smp : 9; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 rxcVapIdxUsr0Smp : 5; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 11;
	} bitFields;
} RegHobBfeeSta_u;

/*REG_HOB_LENGTH_WITH_DELIMITER_FCS 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lengthWithDelimiterFcs : 13; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegHobLengthWithDelimiterFcs_u;

/*REG_HOB_LENGTH_WITHOUT_DELIMITER 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lengthWithoutDelimiter : 13; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegHobLengthWithoutDelimiter_u;

/*REG_HOB_RD_BF_RPT_VAP_TID0_DB_ADDR 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdBfRptVapTid0DbAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobRdBfRptVapTid0DbAddr_u;

/*REG_HOB_BFEE_PHY_MODE 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpPhyModeIsHt : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 mpPhyMode : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 txPhyMode : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 21;
	} bitFields;
} RegHobBfeePhyMode_u;

/*REG_HOB_RXC_PHY_NDP_BW_SMP 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcPhyNdpBwSmp : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobRxcPhyNdpBwSmp_u;

/*REG_HOB_TX_RECIPE_BASE_ADDR 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txRecipeBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobTxRecipeBaseAddr_u;

/*REG_HOB_TXC_SECTION_A_BASE_ADDR 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionABaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobTxcSectionABaseAddr_u;

/*REG_HOB_TXC_SECTION_B_BASE_ADDR 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionBBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobTxcSectionBBaseAddr_u;

/*REG_HOB_TXC_SECTION_C_BASE_ADDR 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionCBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobTxcSectionCBaseAddr_u;

/*REG_HOB_TXC_SECTION_A_BYTE_LEN 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionAByteLen : 13; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegHobTxcSectionAByteLen_u;

/*REG_HOB_TXC_SECTION_B_BYTE_LEN 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionBByteLen : 13; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegHobTxcSectionBByteLen_u;

/*REG_HOB_TXC_SECTION_C_BYTE_LEN 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txcSectionCByteLen : 13; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegHobTxcSectionCByteLen_u;

/*REG_HOB_ACTION_AND_CATEGORY 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 category : 8; //no description, reset value: 0x0, access type: RO
		uint32 action : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobActionAndCategory_u;

/*REG_HOB_BFEE_GEN_FCS_VALID 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeGenFcsValid : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobBfeeGenFcsValid_u;

/*REG_HOB_BFEE_MIMO_CONTROL_SU 0x244 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeMimoControlSu : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobBfeeMimoControlSu_u;

/*REG_HOB_BFEE_SM 0x24C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeBfrSuccess : 1; //no description, reset value: 0x0, access type: RO
		uint32 bfBfeeSmIdle : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobBfeeSm_u;

/*REG_HOB_BFEE_INVALID_TX_DATA 0x250 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeInvalidTxData : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobBfeeInvalidTxData_u;

/*REG_HOB_BFEE_STATIC_INDIACTIONS 0x27C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfeeLengthReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 txcSectionAReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 txcSectionBReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 txcSectionCReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegHobBfeeStaticIndiactions_u;

/*REG_HOB_LATCHED_TST_FIME_LOW 0x400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latchedTstFimeLow : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobLatchedTstFimeLow_u;

/*REG_HOB_LATCHED_TST_FIME_HIGH 0x404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latchedTstFimeHigh : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobLatchedTstFimeHigh_u;

/*REG_HOB_TCC_CRC 0x600 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tccCrc : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTccCrc_u;

/*REG_HOB_HOB_DEL_COUNTER0_EVENT 0x800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobDelCounter0Event : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegHobHobDelCounter0Event_u;

/*REG_HOB_HOB_DEL_COUNTER1_EVENT 0x804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobDelCounter1Event : 12; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 20;
	} bitFields;
} RegHobHobDelCounter1Event_u;

/*REG_HOB_NOP_WAIT_EVENT_VECTOR 0x808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nopWaitEventVector : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobNopWaitEventVector_u;

/*REG_HOB_TX_DATA_STRUCTURE_ADDR 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDataStructureAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobTxDataStructureAddr_u;

/*REG_HOB_DEL2GENRISC_START_TX_INT 0x810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 del2GenriscStartTxInt : 6; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegHobDel2GenriscStartTxInt_u;

/*REG_HOB_PRBS_DATA 0x818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 prbsData : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegHobPrbsData_u;

/*REG_HOB_PHY_MODE 0x81C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhy11AgMode : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhy11BMode : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhy11NMode : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhy11AcMode : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhyHeSu : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhyHeExSu : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhyHeMuTf : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhyHeMu : 1; //no description, reset value: 0x0, access type: RO
		uint32 pacPhyVhtHe : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegHobPhyMode_u;

/*REG_HOB_BF_TX_MAX_THRESHOLD_TIMER 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfTxMaxThresholdTimer : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegHobBfTxMaxThresholdTimer_u;

/*REG_HOB_ZLD_VAL 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 zldVal : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobZldVal_u;

/*REG_HOB_DEL_TXC_RESP_TIMEOUT_VALUE 0x828 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delTxcRespTimeoutValue : 13; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 19;
	} bitFields;
} RegHobDelTxcRespTimeoutValue_u;

/*REG_HOB_RX_LAST_MPDU_IN_PTR 0x82C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLastMpduInPtr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobRxLastMpduInPtr_u;

/*REG_HOB_CTR_DATA 0x834 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctrData : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegHobCtrData_u;

/*REG_HOB_LOGGER_HOB_INFO 0x838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerHobInfo : 8; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegHobLoggerHobInfo_u;

/*REG_HOB_HT_DELIMITER_DATA_IN 0x83C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delimiterDataIn : 16; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegHobHtDelimiterDataIn_u;

/*REG_HOB_VHT_DELIMITER_DATA_IN 0x840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delimiterDataIn : 16; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegHobVhtDelimiterDataIn_u;

/*REG_HOB_VHT_EOF_DELIMITER_DATA_IN 0x844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delimiterDataIn : 16; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegHobVhtEofDelimiterDataIn_u;

/*REG_HOB_DELIMITER_DATA_OUT 0x848 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delimiterDataOut : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobDelimiterDataOut_u;

/*REG_HOB_DELIA_EVENT_THRESHOLD_TIMER 0x84C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaEventThresholdTimer : 24; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegHobDeliaEventThresholdTimer_u;

/*REG_HOB_TIM_DEL_EVENT_TIMER_EXPIRED 0x850 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timDelEventTimerExpired : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobTimDelEventTimerExpired_u;

/*REG_HOB_TCR_TXOP_DUR_FROM_MAC 0x854 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrTxopDurFromMac : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobTcrTxopDurFromMac_u;

/*REG_HOB_DELIA_TX_MPDU_DUR_FIELD 0x85C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMpduDurField : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobDeliaTxMpduDurField_u;

/*REG_HOB_PAC_PHY_TX_LENGTH_ZERO 0x860 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxLengthZero : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobPacPhyTxLengthZero_u;

/*REG_HOB_DELIMITER_VALUE_PRE_TX_DATA 0x864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delimiterValuePreTxData : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobDelimiterValuePreTxData_u;

/*REG_HOB_NUM_OF_USER_TCRS 0x868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numOfUserTcrs : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegHobNumOfUserTcrs_u;

/*REG_HOB_NUM_OF_USER_TCR_BYTES 0x86C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numOfUserTcrBytes : 12; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegHobNumOfUserTcrBytes_u;

/*REG_HOB_EXPECTED_RCRS 0x870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 expectedRcrs : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegHobExpectedRcrs_u;

/*REG_HOB_NUM_OF_USER_RCR_BYTES 0x874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 numOfUserRcrBytes : 12; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 20;
	} bitFields;
} RegHobNumOfUserRcrBytes_u;

/*REG_HOB_TX_CIR_BUF_BASE_ADDR 0x878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txCirBufBaseAddr : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTxCirBufBaseAddr_u;

/*REG_HOB_REG2HOB_GP0 0x87C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reg2HobGp0 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobReg2HobGp0_u;

/*REG_HOB_REG2HOB_GP1 0x880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reg2HobGp1 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobReg2HobGp1_u;

/*REG_HOB_TCR_BASE_ADDR1 0x884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrBaseAddr2 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobTcrBaseAddr1_u;

/*REG_HOB_TCR_BASE_ADDR2 0x888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrBaseAddr2 : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobTcrBaseAddr2_u;

/*REG_HOB_RCR_BASE_ADDR 0x88C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rcrBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobRcrBaseAddr_u;

/*REG_HOB_SENDER_MPDU_DURATION 0x890 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderMpduDuration : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobSenderMpduDuration_u;

/*REG_HOB_ALL_ONES_PADDING 0x894 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 allOnesPadding : 32; //no description, reset value: 0xffffffff, access type: RO
	} bitFields;
} RegHobAllOnesPadding_u;

/*REG_HOB_DEL_DESC_FRAME_LEN 0xA00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delDescFrameLen : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegHobDelDescFrameLen_u;

/*REG_HOB_DEL_DESC_FRAME_PTR 0xA04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delDescFramePtr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobDelDescFramePtr_u;

/*REG_HOB_DEL_DESC_DUR_TYPE 0xA08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delDescDurType : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobDelDescDurType_u;

/*REG_HOB_DEL_TCR_FRAME_LEN 0xA0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delTcrFrameLen : 10; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegHobDelTcrFrameLen_u;

/*REG_HOB_DEL_MAC_ADDR_RA0 0xA10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrRa310 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobDelMacAddrRa0_u;

/*REG_HOB_DEL_MAC_ADDR_RA1 0xA14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrRa4732 : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobDelMacAddrRa1_u;

/*REG_HOB_DEL_MAC_ADDR_TA0 0xA18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrTa310 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobDelMacAddrTa0_u;

/*REG_HOB_DEL_MAC_ADDR_TA1 0xA1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delMacAddrTa4732 : 16; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobDelMacAddrTa1_u;

/*REG_HOB_AUTO_RESP_TCR_BASE_ADDR 0xA20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTcrBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobAutoRespTcrBaseAddr_u;

/*REG_HOB_AUTO_RESP_HE_MU_TCR_BASE_ADDR 0xA24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespHeMuTcrBaseAddr : 25; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegHobAutoRespHeMuTcrBaseAddr_u;

/*REG_HOB_AUTO_RESP_TX_BW 0xA28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTxBw : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 30;
	} bitFields;
} RegHobAutoRespTxBw_u;

/*REG_HOB_TIM_IE_CONTROL 0xC00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 elementId : 8; //no description, reset value: 0x0, access type: RO
		uint32 length : 8; //no description, reset value: 0x0, access type: RO
		uint32 dtimCount : 8; //no description, reset value: 0x0, access type: RO
		uint32 dtimPeriod : 8; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeControl_u;

/*REG_HOB_TIM_IE_BITMAP_CONTROL 0xC04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapControl : 8; //bitmap_control, reset value: 0x0, access type: RO
		uint32 mpsGroup : 24; //MBSSID - MPS group field value. , SBSSID - 0 , reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmapControl_u;

/*REG_HOB_TIM_IE_MPS_GROUP_MSB 0xC08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpsGroupMsbByte : 8; //MBSSID - MPS group field value. , SBSSID - 0 , reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegHobTimIeMpsGroupMsb_u;

/*REG_HOB_TIM_IE_BITMAP0 0xC0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB3B0 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap0_u;

/*REG_HOB_TIM_IE_BITMAP1 0xC10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB7B4 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap1_u;

/*REG_HOB_TIM_IE_BITMAP2 0xC14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB11B8 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap2_u;

/*REG_HOB_TIM_IE_BITMAP3 0xC18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB15B12 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap3_u;

/*REG_HOB_TIM_IE_BITMAP4 0xC1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB19B16 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap4_u;

/*REG_HOB_TIM_IE_BITMAP5 0xC20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB23B20 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap5_u;

/*REG_HOB_TIM_IE_BITMAP6 0xC24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB27B24 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap6_u;

/*REG_HOB_TIM_IE_BITMAP7 0xC28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB31B28 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap7_u;

/*REG_HOB_TIM_IE_BITMAP8 0xC2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB35B32 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap8_u;

/*REG_HOB_TIM_IE_BITMAP9 0xC30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB39B36 : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobTimIeBitmap9_u;

/*REG_HOB_TIM_IE_BITMAP10 0xC34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapB40 : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegHobTimIeBitmap10_u;

/*REG_HOB_TIM_IE_HOB_RECIPE_FIELDS0 0xC38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobReadLength : 8; //no description, reset value: 0x0, access type: RO
		uint32 hobReadOffset : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobTimIeHobRecipeFields0_u;

/*REG_HOB_TIM_IE_HOB_RECIPE_FIELDS1 0xC3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timIeBitmapControlLength : 3; //Length of bitmap_control read: , SBSSID - 1 , MBSSID - 1 + group size in bytes., reset value: 0x0, access type: RO
		uint32 reserved0 : 29;
	} bitFields;
} RegHobTimIeHobRecipeFields1_u;

/*REG_HOB_TIM_IE_HOB_DEBUG_FIELDS0 0xC40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timEmpty : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 timReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 7;
		uint32 timIeIsLegacy : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 15;
	} bitFields;
} RegHobTimIeHobDebugFields0_u;

/*REG_HOB_TIM_IE_HOB_DEBUG_FIELDS1 0xC44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaReadDtimVapIdx : 5; //Length of bitmap_control read: , SBSSID - 1 , MBSSID - 1 + group size in bytes., reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegHobTimIeHobDebugFields1_u;

/*REG_HOB_TIM_IE_HOB_DEBUG_FIELDS2 0xC48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaReadDtimCount : 8; //no description, reset value: 0x0, access type: RO
		uint32 deliaReadDtimPeriod : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegHobTimIeHobDebugFields2_u;



#endif // _HOB_REGS_H_
