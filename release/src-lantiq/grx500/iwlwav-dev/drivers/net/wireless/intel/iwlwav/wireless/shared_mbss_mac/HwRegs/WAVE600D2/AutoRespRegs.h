
/***********************************************************************************
File:				AutoRespRegs.h
Module:				autoResp
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _AUTO_RESP_REGS_H_
#define _AUTO_RESP_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define AUTO_RESP_BASE_ADDRESS                             MEMORY_MAP_UNIT_25_BASE_ADDRESS
#define	REG_AUTO_RESP_AUTO_RESP_OUTOF_IDLE_EN                         (AUTO_RESP_BASE_ADDRESS + 0x0)
#define	REG_AUTO_RESP_AUTO_RESP_SW_CLR                                (AUTO_RESP_BASE_ADDRESS + 0x4)
#define	REG_AUTO_RESP_AUTO_RESP_MISC_CONFIG                           (AUTO_RESP_BASE_ADDRESS + 0x8)
#define	REG_AUTO_RESP_AUTO_RESP_LOGGER                                (AUTO_RESP_BASE_ADDRESS + 0xC)
#define	REG_AUTO_RESP_AUTO_RESP_RXC_FIFO_STAT                         (AUTO_RESP_BASE_ADDRESS + 0x10)
#define	REG_AUTO_RESP_AUTO_RESP_RXC_FIFO_FULL_CNTR                    (AUTO_RESP_BASE_ADDRESS + 0x14)
#define	REG_AUTO_RESP_AUTO_RESP_RXC_FIFO_FULL_CNTR_CLR                (AUTO_RESP_BASE_ADDRESS + 0x18)
#define	REG_AUTO_RESP_AUTO_RESP_SM_STAT_DEBUG                         (AUTO_RESP_BASE_ADDRESS + 0x1C)
#define	REG_AUTO_RESP_AUTO_RESP_RATE_ADPTV_EN                         (AUTO_RESP_BASE_ADDRESS + 0x24)
#define	REG_AUTO_RESP_AUTO_RESP_CBA_BITMAP_LEN                        (AUTO_RESP_BASE_ADDRESS + 0x28)
#define	REG_AUTO_RESP_AUTO_RESP_MBA_BITMAP_LEN                        (AUTO_RESP_BASE_ADDRESS + 0x2C)
#define	REG_AUTO_RESP_CONTROL_ACK_CONFIG_TABLE                        (AUTO_RESP_BASE_ADDRESS + 0x30)
#define	REG_AUTO_RESP_CONTROL_BA_CONFIG_TABLE                         (AUTO_RESP_BASE_ADDRESS + 0x34)
#define	REG_AUTO_RESP_MANGMNT_ACK_CONFIG_TABLE                        (AUTO_RESP_BASE_ADDRESS + 0x38)
#define	REG_AUTO_RESP_MANAGMNT_BA_CONFIG_TABLE                        (AUTO_RESP_BASE_ADDRESS + 0x3C)
#define	REG_AUTO_RESP_STA_DB_MULI_TID_EN_OFFSET                       (AUTO_RESP_BASE_ADDRESS + 0x40)
#define	REG_AUTO_RESP_BAG_UPDT_FIFO_STAT                              (AUTO_RESP_BASE_ADDRESS + 0x44)
#define	REG_AUTO_RESP_AUTO_RESP_CTR                                   (AUTO_RESP_BASE_ADDRESS + 0x70)
#define	REG_AUTO_RESP_MPDU_DESC_DLM_IDX                               (AUTO_RESP_BASE_ADDRESS + 0x74)
#define	REG_AUTO_RESP_AUTO_REPLY_BW_TABLE                             (AUTO_RESP_BASE_ADDRESS + 0x78)
#define	REG_AUTO_RESP_AUTO_REPLY_MCS_DEGRADATION_VAL                  (AUTO_RESP_BASE_ADDRESS + 0x7C)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP0TO9          (AUTO_RESP_BASE_ADDRESS + 0x80)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP10TO19        (AUTO_RESP_BASE_ADDRESS + 0x84)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP20TO29        (AUTO_RESP_BASE_ADDRESS + 0x8C)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP30TO31        (AUTO_RESP_BASE_ADDRESS + 0x90)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA0TO31                    (AUTO_RESP_BASE_ADDRESS + 0x94)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA32TO63                   (AUTO_RESP_BASE_ADDRESS + 0x98)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA64TO95                   (AUTO_RESP_BASE_ADDRESS + 0x9C)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA96TO127                  (AUTO_RESP_BASE_ADDRESS + 0x100)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA128TO159                 (AUTO_RESP_BASE_ADDRESS + 0x104)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA160TO191                 (AUTO_RESP_BASE_ADDRESS + 0x108)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA192TO223                 (AUTO_RESP_BASE_ADDRESS + 0x10C)
#define	REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA224TO255                 (AUTO_RESP_BASE_ADDRESS + 0x110)
#define	REG_AUTO_RESP_AUTO_REPLY_OVERRIDE_PARAMS_DBG                  (AUTO_RESP_BASE_ADDRESS + 0x114)
#define	REG_AUTO_RESP_AUTO_REPLY_BW_TABLE_MAX_CFG                     (AUTO_RESP_BASE_ADDRESS + 0x118)
#define	REG_AUTO_RESP_AUTO_RESP_FRAME_GEN_STAT_DEBUG                  (AUTO_RESP_BASE_ADDRESS + 0x11C)
#define	REG_AUTO_RESP_AUTO_RESP_TCR_BASE_ADDR                         (AUTO_RESP_BASE_ADDRESS + 0x120)
#define	REG_AUTO_RESP_AUTO_RESP_MPDU_NULL_PTR_STAT                    (AUTO_RESP_BASE_ADDRESS + 0x124)
#define	REG_AUTO_RESP_AUTO_RESP_TID_VAL_FOR_ACK_FRAME                 (AUTO_RESP_BASE_ADDRESS + 0x128)
#define	REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_BASE_ADDR                    (AUTO_RESP_BASE_ADDRESS + 0x12C)
#define	REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_DEPTH_MINUS_ONE              (AUTO_RESP_BASE_ADDRESS + 0x130)
#define	REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_CLEAR_STRB                   (AUTO_RESP_BASE_ADDRESS + 0x134)
#define	REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_RD_ENTRIES_NUM               (AUTO_RESP_BASE_ADDRESS + 0x138)
#define	REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_NUM_ENTRIES_COUNT            (AUTO_RESP_BASE_ADDRESS + 0x13C)
#define	REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_DEBUG                        (AUTO_RESP_BASE_ADDRESS + 0x140)
#define	REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_EN                           (AUTO_RESP_BASE_ADDRESS + 0x144)
#define	REG_AUTO_RESP_AUTO_RESP_HE_SU_ER_CFG                          (AUTO_RESP_BASE_ADDRESS + 0x150)
#define	REG_AUTO_RESP_AUTO_RESP_HE_SU_ER_BW_RU_TABLE                  (AUTO_RESP_BASE_ADDRESS + 0x158)
#define	REG_AUTO_RESP_AUTO_RESP_SPARE_REG                             (AUTO_RESP_BASE_ADDRESS + 0x15C)
#define	REG_AUTO_RESP_AUTO_RESP_HE_DURATION_UPDATE_MODE               (AUTO_RESP_BASE_ADDRESS + 0x160)
#define	REG_AUTO_RESP_AUTO_RESP_HE_MU_TCR_BASE_ADDR                   (AUTO_RESP_BASE_ADDRESS + 0x164)
#define	REG_AUTO_RESP_USER_TX_WAS_ACTIVE_STATUS0                      (AUTO_RESP_BASE_ADDRESS + 0x168)
#define	REG_AUTO_RESP_USER_TX_WAS_ACTIVE_STATUS1                      (AUTO_RESP_BASE_ADDRESS + 0x16C)
#define	REG_AUTO_RESP_AUTO_RESP_NO_RESP_DEBUG_CNTR0                   (AUTO_RESP_BASE_ADDRESS + 0x170)
#define	REG_AUTO_RESP_AUTO_RESP_NO_RESP_DEBUG_CNTR1                   (AUTO_RESP_BASE_ADDRESS + 0x174)
#define	REG_AUTO_RESP_AUTO_RESP_NO_RESP_DEBUG_CNTR_CLR                (AUTO_RESP_BASE_ADDRESS + 0x178)
#define	REG_AUTO_RESP_AUTO_RESP_HE_SU_TID_AGG_LIMIT                   (AUTO_RESP_BASE_ADDRESS + 0x17C)
#define	REG_AUTO_RESP_AUTO_RESP_CTR_CONT                              (AUTO_RESP_BASE_ADDRESS + 0x180)
#define	REG_AUTO_RESP_INT_ERR_STATUS                                  (AUTO_RESP_BASE_ADDRESS + 0x184)
#define	REG_AUTO_RESP_INT_ERR_EN                                      (AUTO_RESP_BASE_ADDRESS + 0x188)
#define	REG_AUTO_RESP_CLEAR_HISTORY_PER_TID_EN                        (AUTO_RESP_BASE_ADDRESS + 0x190)
#define	REG_AUTO_RESP_AUTO_RESP_TCS0                                  (AUTO_RESP_BASE_ADDRESS + 0x200)
#define	REG_AUTO_RESP_AUTO_RESP_TCS1                                  (AUTO_RESP_BASE_ADDRESS + 0x204)
#define	REG_AUTO_RESP_AUTO_RESP_TCS2                                  (AUTO_RESP_BASE_ADDRESS + 0x208)
#define	REG_AUTO_RESP_AUTO_RESP_TCS3                                  (AUTO_RESP_BASE_ADDRESS + 0x20C)
#define	REG_AUTO_RESP_AUTO_RESP_TCS4                                  (AUTO_RESP_BASE_ADDRESS + 0x210)
#define	REG_AUTO_RESP_AUTO_RESP_TCS5                                  (AUTO_RESP_BASE_ADDRESS + 0x214)
#define	REG_AUTO_RESP_AUTO_RESP_TCS6                                  (AUTO_RESP_BASE_ADDRESS + 0x218)
#define	REG_AUTO_RESP_AUTO_RESP_TCS7                                  (AUTO_RESP_BASE_ADDRESS + 0x21C)
#define	REG_AUTO_RESP_AUTO_RESP_TCS8                                  (AUTO_RESP_BASE_ADDRESS + 0x220)
#define	REG_AUTO_RESP_AUTO_RESP_TCS9                                  (AUTO_RESP_BASE_ADDRESS + 0x224)
#define	REG_AUTO_RESP_AUTO_RESP_TCS10                                 (AUTO_RESP_BASE_ADDRESS + 0x228)
#define	REG_AUTO_RESP_AUTO_RESP_TCS11                                 (AUTO_RESP_BASE_ADDRESS + 0x22C)
#define	REG_AUTO_RESP_AUTO_RESP_TCS12                                 (AUTO_RESP_BASE_ADDRESS + 0x230)
#define	REG_AUTO_RESP_AUTO_RESP_TCS13                                 (AUTO_RESP_BASE_ADDRESS + 0x234)
#define	REG_AUTO_RESP_AUTO_RESP_TCS14                                 (AUTO_RESP_BASE_ADDRESS + 0x238)
#define	REG_AUTO_RESP_AUTO_RESP_TCS15                                 (AUTO_RESP_BASE_ADDRESS + 0x23C)
#define	REG_AUTO_RESP_AUTO_RESP_STAT_DEBUG1                           (AUTO_RESP_BASE_ADDRESS + 0x280)
#define	REG_AUTO_RESP_AUTO_RESP_STAT_DEBUG2                           (AUTO_RESP_BASE_ADDRESS + 0x284)
#define	REG_AUTO_RESP_AUTO_RESP_TF_CFG                                (AUTO_RESP_BASE_ADDRESS + 0x290)
#define	REG_AUTO_RESP_AUTO_RESP_TF_DEBUG                              (AUTO_RESP_BASE_ADDRESS + 0x29C)
#define	REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_BASE_ADDR            (AUTO_RESP_BASE_ADDRESS + 0x2B0)
#define	REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_DEPTH_MINUS_ONE      (AUTO_RESP_BASE_ADDRESS + 0x2B4)
#define	REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_CLEAR_STRB           (AUTO_RESP_BASE_ADDRESS + 0x2B8)
#define	REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_RD_ENTRIES_NUM       (AUTO_RESP_BASE_ADDRESS + 0x2BC)
#define	REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_NUM_ENTRIES_COUNT    (AUTO_RESP_BASE_ADDRESS + 0x2C0)
#define	REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_DEBUG                (AUTO_RESP_BASE_ADDRESS + 0x2C4)
#define	REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_EN                   (AUTO_RESP_BASE_ADDRESS + 0x2C8)
#define	REG_AUTO_RESP_STD_DB_GENERAL                                  (AUTO_RESP_BASE_ADDRESS + 0x300)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_AUTO_RESP_AUTO_RESP_OUTOF_IDLE_EN 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 outOfIdleEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegAutoRespAutoRespOutofIdleEn_u;

/*REG_AUTO_RESP_AUTO_RESP_SW_CLR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcFifoClrReq : 1; //SW writes 1'b1 in order to clear the fifo between RXC and auto response module , reset value: 0x0, access type: WO
		uint32 bagUpdtFifoClrReq : 1; //SW writes 1'b1 in order to clear the fifo between auto response top and BAG bitmap update logic , reset value: 0x0, access type: WO
		uint32 userDbClrPulse : 1; //SW writes 1'b1 in order to clear auto resp user DB. Should be used each time sender writes the tx user ID and tid aggregation limit to th DB , reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegAutoRespAutoRespSwClr_u;

/*REG_AUTO_RESP_AUTO_RESP_MISC_CONFIG 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 dynamicBitmapDisable : 1; //enable/disable of bitmap resizing: , 0 - use dynamic bitmap sizing. i.e.- change the window size to values less or equal to negotiate window bitmap size , 1 - use only the negotiate window bitmap size, reset value: 0x0, access type: RW
		uint32 barNoBaAgrAckPolicy : 1; //BAR BA agreement ACK policy – defined if to response with ACK to BAR with no BA Agreement: , '0' - do not respond with ACK to BAR with no BA Agreement  , '1' - respond with ACK to BAR with no BA Agreement  , , reset value: 0x0, access type: RW
		uint32 barNumBitsCfg : 1; //0 - Use the fragments bits as in BA , 1 - use the window size of BA Agreement i.e. in case window size = 120 use 128 , , reset value: 0x1, access type: RW
		uint32 trafficBaAgreementRecipientEn : 1; //enable BA agreement recipient I/F. 0 - disable, 1 - enable (default) , , reset value: 0x1, access type: RW
		uint32 swUsrDbClr : 1; //sw writes 1'b1 in order to force user DB clear at the start of the next RX session , , reset value: 0x0, access type: RW
		uint32 acceptFirstSnOutOfBoundsInKeepHist : 1; //'0' - ignore first SN out of bounds in a new session with keep history. '1' - accept first SN out of bounds in a new session with keep history, reset value: 0x1, access type: RW
		uint32 heSuUseMbaInstdAck : 1; //0' - do not replace ACK with M-BA TID ACK in HE SU, '1' - replace ACK with M-BA TID ACK  in HE SU, reset value: 0x0, access type: RW
		uint32 heMuUseMbaInstdAck : 1; //0' - do not replace ACK with M-BA TID ACK in HE MU, '1' - replace ACK with M-BA TID ACK  in HE MU, reset value: 0x1, access type: RW
		uint32 supportMultiTidBar : 1; //When set to '1' multi TID BAR is supported. Otherwise ignored., reset value: 0x1, access type: RW
		uint32 supportTbMuBar : 1; //When set to '1' TB MU BAR is supported. Otherwise ignored., reset value: 0x1, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegAutoRespAutoRespMiscConfig_u;

/*REG_AUTO_RESP_AUTO_RESP_LOGGER 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespLoggerEn : 1; //Auto response logger enable bit, reset value: 0x0, access type: RW
		uint32 autoRespLoggerPrio : 2; //Auto response logger priority bits, reset value: 0x0, access type: RW
		uint32 loggerAllCfg : 1; //1 bit indication if to send all to logger - '1' - send all, '0' - send only rx start/end & tx, reset value: 0x1, access type: RW
		uint32 loggerCounterLimitCfg : 8; //limit of number of words sent to logger before done indication (default 'd100), reset value: 0x64, access type: RW
		uint32 loggerActive : 1; //logger active status. '1'- active, '0' - not active , reset value: 0x0, access type: RO
		uint32 reserved0 : 19;
	} bitFields;
} RegAutoRespAutoRespLogger_u;

/*REG_AUTO_RESP_AUTO_RESP_RXC_FIFO_STAT 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcMainFifoByteCount : 3; //no description, reset value: 0x0, access type: RO
		uint32 rxcMainFifoFull : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxcMainFifoEmpty : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxcMainFifoPushIgnored : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxcMainFifoPopIgnored : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 9;
		uint32 rxcBufFifoByteCount : 2; //no description, reset value: 0x0, access type: RO
		uint32 rxcBufFifoFull : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxcBufFifoEmpty : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxcBufFifoPushIgnored : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxcBufFifoPopIgnored : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 10;
	} bitFields;
} RegAutoRespAutoRespRxcFifoStat_u;

/*REG_AUTO_RESP_AUTO_RESP_RXC_FIFO_FULL_CNTR 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcFifoFullCntr : 8; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegAutoRespAutoRespRxcFifoFullCntr_u;

/*REG_AUTO_RESP_AUTO_RESP_RXC_FIFO_FULL_CNTR_CLR 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxcFifoFullCntrClrReq : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegAutoRespAutoRespRxcFifoFullCntrClr_u;

/*REG_AUTO_RESP_AUTO_RESP_SM_STAT_DEBUG 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMainFsm : 5; //no description, reset value: 0x0, access type: RO
		uint32 baAgrRdFsm : 3; //no description, reset value: 0x0, access type: RO
		uint32 usrStaRdFsm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 tidBmapFsm : 4; //no description, reset value: 0x0, access type: RO
		uint32 arespMtidSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 arespMuBarSm : 2; //no description, reset value: 0x0, access type: RO
		uint32 reserved2 : 10;
	} bitFields;
} RegAutoRespAutoRespSmStatDebug_u;

/*REG_AUTO_RESP_AUTO_RESP_RATE_ADPTV_EN 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rateAdptvRptEn : 1; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegAutoRespAutoRespRateAdptvEn_u;

/*REG_AUTO_RESP_AUTO_RESP_CBA_BITMAP_LEN 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cbaBitmapLen64 : 4; //fragment number subfield bits setting in case of bitmap length 64 bit, reset value: 0x0, access type: RW
		uint32 cbaBitmapLen128 : 4; //fragment number subfield bits setting in case of bitmap length 128 bit, reset value: 0x2, access type: RW
		uint32 cbaBitmapLen256 : 4; //fragment number subfield bits setting in case of bitmap length 128 bit, reset value: 0x4, access type: RW
		uint32 cbaBitmapLen32 : 4; //fragment number subfield bits setting in case of bitmap length 32 bit, reset value: 0x6, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegAutoRespAutoRespCbaBitmapLen_u;

/*REG_AUTO_RESP_AUTO_RESP_MBA_BITMAP_LEN 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mbaBitmapLen64 : 4; //fragment number subfield bits setting in case of bitmap length 64 bit, reset value: 0x0, access type: RW
		uint32 mbaBitmapLen128 : 4; //fragment number subfield bits setting in case of bitmap length 128 bit, reset value: 0x2, access type: RW
		uint32 mbaBitmapLen256 : 4; //fragment number subfield bits setting in case of bitmap length 256 bit, reset value: 0x4, access type: RW
		uint32 mbaBitmapLen32 : 4; //fragment number subfield bits setting in case of bitmap length 32 bit, reset value: 0x6, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegAutoRespAutoRespMbaBitmapLen_u;

/*REG_AUTO_RESP_CONTROL_ACK_CONFIG_TABLE 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 controlAckConfig : 16; //no description, reset value: 0x400, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegAutoRespControlAckConfigTable_u;

/*REG_AUTO_RESP_CONTROL_BA_CONFIG_TABLE 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 controlBaConfig : 16; //no description, reset value: 0x400, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegAutoRespControlBaConfigTable_u;

/*REG_AUTO_RESP_MANGMNT_ACK_CONFIG_TABLE 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mangmntAckConfig : 16; //no description, reset value: 0xBFFF, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegAutoRespMangmntAckConfigTable_u;

/*REG_AUTO_RESP_MANAGMNT_BA_CONFIG_TABLE 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mangmntBaConfig : 16; //no description, reset value: 0xBFFF, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegAutoRespManagmntBaConfigTable_u;

/*REG_AUTO_RESP_STA_DB_MULI_TID_EN_OFFSET 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 staDbMultiTidEn4BOffset : 8; //sta tx multi tid enalbed field address offset inside sta db in 4 bytes alignment, reset value: 0xA, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegAutoRespStaDbMuliTidEnOffset_u;

/*REG_AUTO_RESP_BAG_UPDT_FIFO_STAT 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bagUpdtFifoByteCount : 4; //no description, reset value: 0x0, access type: RO
		uint32 bagUpdtFifoFull : 1; //no description, reset value: 0x0, access type: RO
		uint32 bagUpdtFifoEmpty : 1; //no description, reset value: 0x0, access type: RO
		uint32 bagUpdtFifoPushIgnored : 1; //no description, reset value: 0x0, access type: RO
		uint32 bagUpdtFifoPopIgnored : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegAutoRespBagUpdtFifoStat_u;

/*REG_AUTO_RESP_AUTO_RESP_CTR 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTxCtr : 8; //Auto response Tx counter, reset value: 0x0, access type: RO
		uint32 autoRespRxNavAbortCtr : 8; //Auto response Rx NAV abort counter, reset value: 0x0, access type: RO
		uint32 autoRespTxNavAbortCtr : 8; //Auto response Tx NAV abort counter, reset value: 0x0, access type: RO
		uint32 autoRespStaticBwAbortCtr : 8; //Auto response Static BW abort counter, reset value: 0x0, access type: RO
	} bitFields;
} RegAutoRespAutoRespCtr_u;

/*REG_AUTO_RESP_MPDU_DESC_DLM_IDX 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduDescFreeListIdx : 8; //sets the MPDU descriptor free list index for Auto response, reset value: 0x0, access type: RW
		uint32 perUserTxBaseDlmListIdx : 8; //sets the MPDU per user descriptor base index for Auto response , reset value: 0x0, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegAutoRespMpduDescDlmIdx_u;

/*REG_AUTO_RESP_AUTO_REPLY_BW_TABLE 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyBwTable : 8; //Auto reply BW table, reset value: 0xe4, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegAutoRespAutoReplyBwTable_u;

/*REG_AUTO_RESP_AUTO_REPLY_MCS_DEGRADATION_VAL 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyMcsDegradationVal : 4; //Auto reply degradation value for MCS in, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegAutoRespAutoReplyMcsDegradationVal_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP0TO9 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrDegradationValVap0To9 : 30; //Auto reply degradation value for power, 3 bit per VAP, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegAutoRespAutoReplyPwrDegradationValVap0To9_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP10TO19 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrDegradationValVap10To19 : 30; //Auto reply degradation value for power, 3 bit per VAP, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegAutoRespAutoReplyPwrDegradationValVap10To19_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP20TO29 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrDegradationValVap20To29 : 30; //Auto reply degradation value for power, 3 bit per VAP, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
	} bitFields;
} RegAutoRespAutoReplyPwrDegradationValVap20To29_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP30TO31 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrDegradationValVap30To31 : 6; //Auto reply degradation value for power, 3 bit per VAP, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegAutoRespAutoReplyPwrDegradationValVap30To31_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA0TO31 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta0To31 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta0To31_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA32TO63 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta32To63 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta32To63_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA64TO95 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta64To95 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta64To95_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA96TO127 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta96To127 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta96To127_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA128TO159 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta128To159 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta128To159_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA160TO191 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta160To191 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta160To191_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA192TO223 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta192To223 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta192To223_u;

/*REG_AUTO_RESP_AUTO_REPLY_PWR_NEAR_STA224TO255 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta224To255 : 32; //Auto reply STA is near indication, one bit per STA, reset value: 0x0, access type: RW
	} bitFields;
} RegAutoRespAutoReplyPwrNearSta224To255_u;

/*REG_AUTO_RESP_AUTO_REPLY_OVERRIDE_PARAMS_DBG 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPhyMode : 3; //auto reply phy mode, reset value: 0x0, access type: RO
		uint32 autoReplyBw : 2; //auto reply bw, reset value: 0x0, access type: RO
		uint32 autoReplyMcs : 8; //auto_ reply mcs, reset value: 0x0, access type: RO
		uint32 autoReplyTxPower : 8; //auto reply tx power, reset value: 0x0, access type: RO
		uint32 autoReplyRuSize : 3; //auto reply ru size, reset value: 0x0, access type: RO
		uint32 autoReplyLdpc : 1; //auto reply ldpc, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
	} bitFields;
} RegAutoRespAutoReplyOverrideParamsDbg_u;

/*REG_AUTO_RESP_AUTO_REPLY_BW_TABLE_MAX_CFG 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyBwTableMaxCfg : 2; //auto reply BW select max val (deafult 2'd3 --> 160 MHz). Represents the Maximum BW that Auto response can use for response frame. it is compared to the BW calculation logic and used only if calculated value is grater then it., reset value: 0x3, access type: RW
		uint32 reserved0 : 30;
	} bitFields;
} RegAutoRespAutoReplyBwTableMaxCfg_u;

/*REG_AUTO_RESP_AUTO_RESP_FRAME_GEN_STAT_DEBUG 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bag1StStageSmOut : 3; //no description, reset value: 0x0, access type: RO
		uint32 bag2NdStageSmOut : 4; //no description, reset value: 0x0, access type: RO
		uint32 bagFrameGenClkEn : 1; //no description, reset value: 0x0, access type: RO
		uint32 selectedUser : 8; //no description, reset value: 0x0, access type: RO
		uint32 selectedTid : 4; //no description, reset value: 0x0, access type: RO
		uint32 selectedAckType : 3; //no description, reset value: 0x0, access type: RO
		uint32 dbWrEn : 1; //no description, reset value: 0x0, access type: RO
		uint32 dbDataOutAvailable : 1; //no description, reset value: 0x0, access type: RO
		uint32 readRaTaCount : 2; //no description, reset value: 0x0, access type: RO
		uint32 bagFrameGenToRxcSmcSmcReq : 1; //no description, reset value: 0x0, access type: RO
		uint32 bagFrameGenToUpMpduDlmValid : 1; //no description, reset value: 0x0, access type: RO
		uint32 bagFrameGenToMpduDescRamSmcSmcReq : 1; //no description, reset value: 0x0, access type: RO
		uint32 bagFrameGenToTxCircBufSmcSmcReq : 1; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 1;
	} bitFields;
} RegAutoRespAutoRespFrameGenStatDebug_u;

/*REG_AUTO_RESP_AUTO_RESP_TCR_BASE_ADDR 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTcrBaseAddr : 25; //Base address of the first common TCR in SU TXOP (byte aligned)), reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegAutoRespAutoRespTcrBaseAddr_u;

/*REG_AUTO_RESP_AUTO_RESP_MPDU_NULL_PTR_STAT 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nullMpduCnt : 8; //mpdu pointer null counter. Counts up each time received a null pointer rom q_manager when requesting a free mpdu pointer, reset value: 0x0, access type: RO
		uint32 nullMpduCntOvflw : 1; //mpdu pointer null counter overflow. Asserts if counter reaches saturation, reset value: 0x0, access type: RO
		uint32 reserved0 : 7;
		uint32 nullMpduCntClr : 1; //SW writes 1'b1 in order to clear the mpdu null ptr counter status register , reset value: 0x0, access type: WO
		uint32 reserved1 : 15;
	} bitFields;
} RegAutoRespAutoRespMpduNullPtrStat_u;

/*REG_AUTO_RESP_AUTO_RESP_TID_VAL_FOR_ACK_FRAME 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpduTidFieldVal : 4; //This is the TID val that is put in the TID field of the ACK frame sent. Default is 4'hF (control), reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegAutoRespAutoRespTidValForAckFrame_u;

/*REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_BASE_ADDR 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoBaseAddr : 22; //AUTO RESP ERROR FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegAutoRespAutoRespErrFifoBaseAddr_u;

/*REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_DEPTH_MINUS_ONE 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoDepthMinusOne : 8; //AUTO RESP ERROR FIFO depth minus one, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegAutoRespAutoRespErrFifoDepthMinusOne_u;

/*REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_CLEAR_STRB 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoClearStrb : 1; //Clear AUTO RESP ERROR FIFO. , By write '1'., reset value: 0x0, access type: WO
		uint32 autoRespErrFifoClearFullDropCtrStrb : 1; //Clear AUTO RESP ERROR FIFO full drop counter. , By write '1'., reset value: 0x0, access type: WO
		uint32 autoRespErrFifoClearDecLessThanZeroStrb : 1; //Clear AUTO RESP ERROR FIFO decrement amount less than zero indication. , By write '1'.  , , reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegAutoRespAutoRespErrFifoClearStrb_u;

/*REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_RD_ENTRIES_NUM 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoRdEntriesNum : 9; //AUTO RESP ERROR FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegAutoRespAutoRespErrFifoRdEntriesNum_u;

/*REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_NUM_ENTRIES_COUNT 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoNumEntriesCount : 9; //AUTO RESP ERROR FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegAutoRespAutoRespErrFifoNumEntriesCount_u;

/*REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_DEBUG 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoWrPtr : 8; //AUTO RESP ERROR FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
		uint32 autoRespErrFifoNotEmpty : 1; //AUTO RESP ERROR FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 autoRespErrFifoFull : 1; //AUTO RESP ERROR FIFO full indication, reset value: 0x0, access type: RO
		uint32 autoRespErrFifoDecrementLessThanZero : 1; //AUTO RESP ERROR FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 autoRespErrFifoFullDropCtr : 8; //AUTO RESP ERROR FIFO full drop counter, reset value: 0x0, access type: RO
	} bitFields;
} RegAutoRespAutoRespErrFifoDebug_u;

/*REG_AUTO_RESP_AUTO_RESP_ERR_FIFO_EN 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoMltiTidEn : 1; //AUTO RESP ERROR FIFO multi TID error enable., reset value: 0x0, access type: RW
		uint32 autoRespErrFifoBarErrEn : 1; //AUTO RESP ERROR FIFO BAR error enable., reset value: 0x0, access type: RW
		uint32 autoRespErrFifoSnOutBoundEn : 1; //AUTO RESP ERROR FIFO NUM SN out of bounds error enable., reset value: 0x0, access type: RW
		uint32 autoRespErrFifoMultiTidBarLengthEn : 1; //AUTO RESP ERROR FIFO MULTI TID BAR LENGTH error enable., reset value: 0x0, access type: RW
		uint32 autoRespErrFifoImmToNonImmEn : 1; //AUTO RESP ERROR FIFO Immediate to Non-Immediate error enable., reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegAutoRespAutoRespErrFifoEn_u;

/*REG_AUTO_RESP_AUTO_RESP_HE_SU_ER_CFG 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSuErTxRuSizeRespForHeSu : 1; //Defines the transmitted HE SU ER RU Size when received HE SU PPDU with BW = 20MHz. , 0 - RU242 , 1 - RU106, reset value: 0x0, access type: RW
		uint32 heSuErTxRuSizeRespForHeSuErRu242 : 1; //Defines the transmitted HE SU ER RU Size when received HE SU ER PPDU with RU=242. , 0 - RU242 , 1 - RU106, reset value: 0x0, access type: RW
		uint32 heSuErTxRuSizeRespForHeSuErRu106 : 1; //Defines the transmitted HE SU ER RU Size when received HE SU ER PPDU with RU=106. , 0 - RU242 , 1 - RU106, reset value: 0x1, access type: RW
		uint32 heSuErTxDcmRespForHeSuOrHeSuErDcm1 : 1; //Defines how to set the HE SU ER DCM when the received PPDU is HE SU BW20Mhz or HE SU ER with DCM. , 0 - response without DCM , 1 - response with DCM, reset value: 0x1, access type: RW
		uint32 heSuErTxDcmRespForHeSuDcm0 : 1; //Defines how to set the HE SU ER DCM when the received PPDU is HE SU BW20Mhz without DCM. , 0 - response without DCM , 1 - response with DCM, reset value: 0x1, access type: RW
		uint32 heSuErTxDcmRespForHeSuErDcm0 : 1; //Defines how to set the HE SU ER DCM when the received PPDU is HE SU ER without DCM. , 0 - response without DCM , 1 - response with DCM, reset value: 0x1, access type: RW
		uint32 heSuErUnknownStaRespType : 1; //Defines the ACK PHY Mode respond when received HE SU ER PPDU from Unknown STA. , 0 – 11A/G , 1 – HE SU ER, RU Size = 242, reset value: 0x1, access type: RW
		uint32 heSuErTxDcmRespForUnknownStaDcm0 : 1; //Defines how to set the HE SU ER DCM when the received PPDU is from HE SU ER Unknown STA without DCM. , 0 - response without DCM , 1 - response with DCM, reset value: 0x1, access type: RW
		uint32 heSuErTxDcmRespForUnknownStaDcm1 : 1; //Defines how to set the HE SU ER DCM when the received PPDU is from HE SU ER Unknown STA with DCM. , 0 - response without DCM , 1 - response with DCM, reset value: 0x1, access type: RW
		uint32 readOmiHeErDisableBitFromSelector : 1; //Read OMI HE ER Disable Bit from Selector bitmap, defines if to use the OMI HE ER Disable bit from the Selector to disable HE SU ER. , 0 - don't read OMI HE ER Disable Bit , 1 - read OMI HE ER Disable bit and use it for enable/disable HE SU ER, reset value: 0x1, access type: RW
		uint32 reserved0 : 22;
	} bitFields;
} RegAutoRespAutoRespHeSuErCfg_u;

/*REG_AUTO_RESP_AUTO_RESP_HE_SU_ER_BW_RU_TABLE 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSuErRuSize4Bw10 : 3; //Auto Resp HE SU ER RU size value for received BW of 10 MHz. default = 3'd2 - 106 , reset value: 0x2, access type: RW
		uint32 heSuErRuSize4Bw20 : 3; //Auto Resp HE SU ER RU size value for received BW of 20 MHz. default = 3'd3 - 242 , reset value: 0x3, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegAutoRespAutoRespHeSuErBwRuTable_u;

/*REG_AUTO_RESP_AUTO_RESP_SPARE_REG 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespSpareReg : 32; //Auto resp spare register, reset value: 0x88888888, access type: RW
	} bitFields;
} RegAutoRespAutoRespSpareReg_u;

/*REG_AUTO_RESP_AUTO_RESP_HE_DURATION_UPDATE_MODE 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespHeDurationUpdateMode : 1; //one bit en/disable. '1' -full TXOP.  '0' - partial duration. In this case auto resp in HE MU selects partial duration base mode for deila duration source , reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegAutoRespAutoRespHeDurationUpdateMode_u;

/*REG_AUTO_RESP_AUTO_RESP_HE_MU_TCR_BASE_ADDR 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespHeMuTcrBaseAddr : 25; //Base address of the first common TCR in HE MU TXOP (byte aligned)), reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegAutoRespAutoRespHeMuTcrBaseAddr_u;

/*REG_AUTO_RESP_USER_TX_WAS_ACTIVE_STATUS0 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 user310WasActive : 32; //user31 - user0 tx was active bus reflected to debug status register , reset value: 0x0, access type: RO
	} bitFields;
} RegAutoRespUserTxWasActiveStatus0_u;

/*REG_AUTO_RESP_USER_TX_WAS_ACTIVE_STATUS1 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 user3532WasActive : 4; //user35 - user32 tx was active bus reflected to debug status register , reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegAutoRespUserTxWasActiveStatus1_u;

/*REG_AUTO_RESP_AUTO_RESP_NO_RESP_DEBUG_CNTR0 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyErrNoRespDebugCntr : 8; //No response due to phy error indcation debug counter. Counter is incremented by one for each RX session that ended with no Auto resp due to phy error, reset value: 0x0, access type: RO
		uint32 seqAbortNoRespDebugCntr : 8; //No response due to sequencer abort indcation debug counter. Counter is incremented by one for each RX session that ended with no Auto resp due to sequencer abort, reset value: 0x0, access type: RO
		uint32 validStaNoRespDebugCntr : 8; //No response due to no valid station debug counter. Counter is incremented by one for each RX session that ended with no auto resp due to no valid station indication in RX session, reset value: 0x0, access type: RO
		uint32 unicastBitNoRespDebugCntr : 8; //No response due to unicast bit not set debug counter. Counter is incremented by one for each RX session that ended with no auto resp due to unicast bit never set in RX session, reset value: 0x0, access type: RO
	} bitFields;
} RegAutoRespAutoRespNoRespDebugCntr0_u;

/*REG_AUTO_RESP_AUTO_RESP_NO_RESP_DEBUG_CNTR1 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ntdAbortNoRespDebugCntr : 8; //No response due to NTD abort debug counter. Counter is incremented by one for each RX session that ended with no Auto resp due to NTD abort, reset value: 0x0, access type: RO
		uint32 bagPushNoRespDebugCntr : 8; //No response due to no ackable mpdu debug counter. Counter is incremented by one for each RX session that ended with no Auto resp due to no ackable MPDU received from RXC, reset value: 0x0, access type: RO
		uint32 noRespStatus : 6; //No response status bus {no_push_to_bag_fifo,ntd_decision_is_fail,unicast_bit_was_never_set,no_valid_sta_in_rx,seq_abort_no_resp,phy_err_no_resp} , reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 rxcPhyErr : 1; //indication phy error was asserted, reset value: 0x0, access type: RO
		uint32 rxNavZero : 1; //rx nav zero indicaiton reflected from module input, reset value: 0x0, access type: RO
		uint32 txNavZero : 1; //tx nav zero indicaiton reflected from module input, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
	} bitFields;
} RegAutoRespAutoRespNoRespDebugCntr1_u;

/*REG_AUTO_RESP_AUTO_RESP_NO_RESP_DEBUG_CNTR_CLR 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyErrDebugCntrClr : 1; //SW writes 1'b1 in order to clear phy error no resp debug counter , reset value: 0x0, access type: WO
		uint32 seqAbortDebugCntrClr : 1; //SW writes 1'b1 in order to clear sequencer abort no resp debug counter , reset value: 0x0, access type: WO
		uint32 validStaDebugCntrClr : 1; //SW writes 1'b1 in order to clear no valid STA no resp debug counter , reset value: 0x0, access type: WO
		uint32 unicastBitDebugCntrClr : 1; //SW writes 1'b1 in order to clear unicast bit no resp debug counter , reset value: 0x0, access type: WO
		uint32 ntdAbortDebugCntrClr : 1; //SW writes 1'b1 in order to clear ntd abort no resp debug counter , reset value: 0x0, access type: WO
		uint32 bagPushDebugCntrClr : 1; //SW writes 1'b1 in order to clear bag push no resp debug counter , reset value: 0x0, access type: WO
		uint32 reserved0 : 26;
	} bitFields;
} RegAutoRespAutoRespNoRespDebugCntrClr_u;

/*REG_AUTO_RESP_AUTO_RESP_HE_SU_TID_AGG_LIMIT 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 heSuTidAggLimit : 3; //TID aggregation limit in HE SU. This is the max num if TIDs that can be received and answered with ACK/BA. The value set is considered as N-1, reset value: 0x7, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegAutoRespAutoRespHeSuTidAggLimit_u;

/*REG_AUTO_RESP_AUTO_RESP_CTR_CONT 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespBfAbortCtr : 8; //Auto response BF abort counter, reset value: 0x0, access type: RO
		uint32 autoRespTfCcaAbortCtr : 8; //Auto response CCA abort in TB frames, reset value: 0x0, access type: RO
		uint32 autoRespTxhFreezeAbortCtr : 8; //Auto response TxH Freeze abort, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegAutoRespAutoRespCtrCont_u;

/*REG_AUTO_RESP_INT_ERR_STATUS 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespErrFifoFullDropIrq : 1; //Auto response Error FIFO drop while full IRQ, reset value: 0x0, access type: RO
		uint32 autoRespErrFifoDecrementLessThanZeroIrq : 1; //Auto response Error FIFO decrement less than zero IRQ, reset value: 0x0, access type: RO
		uint32 autoRespMubarHwSwFifoFullDropIrq : 1; //Auto response MU-BAR Hw-SW FIFO drop while full IRQ, reset value: 0x0, access type: RO
		uint32 autoRespMubarHwSwFifoDecrementLessThanZeroIrqAgain : 1; //Auto response MU-BAR Hw-SW FIFO decrement less than zero IRQ, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegAutoRespIntErrStatus_u;

/*REG_AUTO_RESP_INT_ERR_EN 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 enAutoRespErrFifoFullDropIrq : 1; //Enable auto_resp_err_fifo_full_drop_irq., reset value: 0x1, access type: RW
		uint32 enAutoRespErrFifoDecrementLessThanZeroIrq : 1; //enable auto_resp_err_fifo_decrement_less_than_zero_irq., reset value: 0x1, access type: RW
		uint32 enAutoRespMubarHwSwFifoFullDropIrq : 1; //Enable auto_resp_mubar_hw_sw_fifo_full_drop_irq., reset value: 0x1, access type: RW
		uint32 enAutoRespMubarHwSwFifoDecrementLessThanZeroIrq : 1; //enable auto_resp_mubar_hw_sw_fifo_decrement_less_than_zero_irq., reset value: 0x1, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegAutoRespIntErrEn_u;

/*REG_AUTO_RESP_CLEAR_HISTORY_PER_TID_EN 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearHistoryPerTidEn : 1; //configuration if to clear history per TID or not: , 0 - clear history is done to all TIDs together , 1 - clear history is done per TID  , reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegAutoRespClearHistoryPerTidEn_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS0 0x200 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp0RecipePtr : 13; //Auto response 0 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp0RxNavAbort : 1; //Auto response 0 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp0TxNavAbort : 1; //Auto response 0 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp0StaticBwAbort : 1; //Auto response 0 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp0BfAbort : 1; //Auto response 0 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp0TxhFreezeAbort : 1; //Auto response 0 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs0_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS1 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp1RecipePtr : 13; //Auto response 1 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp1RxNavAbort : 1; //Auto response 1 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp1TxNavAbort : 1; //Auto response 1 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp1StaticBwAbort : 1; //Auto response 1 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp1BfAbort : 1; //Auto response 1 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp1TxhFreezeAbort : 1; //Auto response 1 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs1_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS2 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp2RecipePtr : 13; //Auto response 2 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp2RxNavAbort : 1; //Auto response 2 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp2TxNavAbort : 1; //Auto response 2 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp2StaticBwAbort : 1; //Auto response 2 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp2BfAbort : 1; //Auto response 2 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp2TxhFreezeAbort : 1; //Auto response 2 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs2_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS3 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp3RecipePtr : 13; //Auto response 3 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp3RxNavAbort : 1; //Auto response 3 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp3TxNavAbort : 1; //Auto response 3 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp3StaticBwAbort : 1; //Auto response 3 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp3BfAbort : 1; //Auto response 3 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp3TxhFreezeAbort : 1; //Auto response 3 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs3_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS4 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp4RecipePtr : 13; //Auto response 4 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp4RxNavAbort : 1; //Auto response 4 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp4TxNavAbort : 1; //Auto response 4 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp4StaticBwAbort : 1; //Auto response 4 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp4BfAbort : 1; //Auto response 4 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp4TxhFreezeAbort : 1; //Auto response 4 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs4_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS5 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp5RecipePtr : 13; //Auto response 5 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp5RxNavAbort : 1; //Auto response 5 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp5TxNavAbort : 1; //Auto response 5 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp5StaticBwAbort : 1; //Auto response 5 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp5BfAbort : 1; //Auto response 5 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp5TxhFreezeAbort : 1; //Auto response 5 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs5_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS6 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp6RecipePtr : 13; //Auto response 6 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp6RxNavAbort : 1; //Auto response 6 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp6TxNavAbort : 1; //Auto response 6 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp6StaticBwAbort : 1; //Auto response 6 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp6BfAbort : 1; //Auto response 6 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp6TxhFreezeAbort : 1; //Auto response 6 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs6_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS7 0x21C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp7RecipePtr : 13; //Auto response 7 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp7RxNavAbort : 1; //Auto response 7 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp7TxNavAbort : 1; //Auto response 7 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp7StaticBwAbort : 1; //Auto response 7 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp7BfAbort : 1; //Auto response 7 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp7TxhFreezeAbort : 1; //Auto response 7 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs7_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS8 0x220 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp8RecipePtr : 13; //Auto response 8 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp8RxNavAbort : 1; //Auto response 8 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp8TxNavAbort : 1; //Auto response 8 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp8StaticBwAbort : 1; //Auto response 8 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp8BfAbort : 1; //Auto response 8 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp8TxhFreezeAbort : 1; //Auto response 8 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs8_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS9 0x224 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp9RecipePtr : 13; //Auto response 9 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp9RxNavAbort : 1; //Auto response 9 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp9TxNavAbort : 1; //Auto response 9 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp9StaticBwAbort : 1; //Auto response 9 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp9BfAbort : 1; //Auto response 9 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp9TxhFreezeAbort : 1; //Auto response 9 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs9_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS10 0x228 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp10RecipePtr : 13; //Auto response 10 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp10RxNavAbort : 1; //Auto response 10 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp10TxNavAbort : 1; //Auto response 10 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp10StaticBwAbort : 1; //Auto response 10 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp10BfAbort : 1; //Auto response 10 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp10TxhFreezeAbort : 1; //Auto response 10 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs10_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS11 0x22C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp11RecipePtr : 13; //Auto response 11 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp11RxNavAbort : 1; //Auto response 11 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp11TxNavAbort : 1; //Auto response 11 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp11StaticBwAbort : 1; //Auto response 11 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp11BfAbort : 1; //Auto response 11 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp11TxhFreezeAbort : 1; //Auto response 11 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs11_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS12 0x230 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp12RecipePtr : 13; //Auto response 12 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp12RxNavAbort : 1; //Auto response 12 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp12TxNavAbort : 1; //Auto response 12 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp12StaticBwAbort : 1; //Auto response 12 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp12BfAbort : 1; //Auto response 12 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp12TxhFreezeAbort : 1; //Auto response 12 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs12_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS13 0x234 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp13RecipePtr : 13; //Auto response 13 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp13RxNavAbort : 1; //Auto response 13 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp13TxNavAbort : 1; //Auto response 13 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp13StaticBwAbort : 1; //Auto response 13 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp13BfAbort : 1; //Auto response 13 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp13TxhFreezeAbort : 1; //Auto response 13 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs13_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS14 0x238 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp14RecipePtr : 13; //Auto response 14 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp14RxNavAbort : 1; //Auto response 14 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp14TxNavAbort : 1; //Auto response 14 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp14StaticBwAbort : 1; //Auto response 14 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp14BfAbort : 1; //Auto response 14 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp14TxhFreezeAbort : 1; //Auto response 14 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs14_u;

/*REG_AUTO_RESP_AUTO_RESP_TCS15 0x23C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp15RecipePtr : 13; //Auto response 15 recipe pointer in TX Recipe area. Auto resp will automatically add the TX Recipe base address to this value, reset value: 0x0, access type: RW
		uint32 reserved0 : 12;
		uint32 autoResp15RxNavAbort : 1; //Auto response 15 RX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp15TxNavAbort : 1; //Auto response 15 TX NAV abort, reset value: 0x0, access type: RW
		uint32 autoResp15StaticBwAbort : 1; //Auto response 15 Static BW abort, reset value: 0x0, access type: RW
		uint32 autoResp15BfAbort : 1; //Auto response 15 BF abort, reset value: 0x0, access type: RW
		uint32 autoResp15TxhFreezeAbort : 1; //Auto response 15 TxH Freeze abort, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
	} bitFields;
} RegAutoRespAutoRespTcs15_u;

/*REG_AUTO_RESP_AUTO_RESP_STAT_DEBUG1 0x280 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tcrLengthNoFcsNoDelToHob : 12; //no description, reset value: 0x0, access type: RO
		uint32 autoRespTypeSmp : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved0 : 4;
		uint32 keepHistoryInd : 1; //no description, reset value: 0x0, access type: RO
		uint32 selectRecipeType : 4; //no description, reset value: 0x0, access type: RO
		uint32 ntdTransmitSuccessLvl : 1; //no description, reset value: 0x0, access type: RO
		uint32 sequencerAutoRespAbortSmp : 1; //no description, reset value: 0x0, access type: RO
		uint32 txhNtdAutoRespTxActive : 1; //no description, reset value: 0x0, access type: RO
		uint32 rxcRxDoneLvl : 1; //no description, reset value: 0x0, access type: RO
		uint32 phyMode : 3; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 1;
	} bitFields;
} RegAutoRespAutoRespStatDebug1_u;

/*REG_AUTO_RESP_AUTO_RESP_STAT_DEBUG2 0x284 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 arespUserTxWasActvieValid : 1; //Valid indication for tx was active bus, reset value: 0x0, access type: RO
		uint32 deliaTcrLenReady : 1; //TCR length ready indication to Delia, reset value: 0x0, access type: RO
		uint32 reserved0 : 13;
		uint32 ntdRecipeType : 5; //no description, reset value: 0x0, access type: RO
		uint32 reserved1 : 8;
		uint32 deliaFrameReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 deliaTbFrameReady : 1; //no description, reset value: 0x0, access type: RO
		uint32 baAgrRdSmcReq : 1; //no description, reset value: 0x0, access type: RO
		uint32 staDbSmcReq : 1; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegAutoRespAutoRespStatDebug2_u;

/*REG_AUTO_RESP_AUTO_RESP_TF_CFG 0x290 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespQosNullLen : 7; //Length of qos-null frame without delimiter. This is the minimal limit check for TZ in case of TB TX., reset value: 0x22, access type: RW
		uint32 reserved0 : 9;
		uint32 swNegateHePuncturingNav : 1; //negate all he_puncturing_nav bits coming from TF decoder module. Only set this field in case there is a bug in polarity between TF decoder and Auto resp, reset value: 0x0, access type: RW
		uint32 swNegateCcaBandsFree : 1; //negate all cca_bands_free bits coming from PHY. Only set this field in case there is a bug in polarity between PHY and Auto resp, reset value: 0x0, access type: RW
		uint32 swIgnoreTfCsRequired : 1; //Ignore CS required check by the Auto resp module, reset value: 0x0, access type: RW
		uint32 reserved1 : 13;
	} bitFields;
} RegAutoRespAutoRespTfCfg_u;

/*REG_AUTO_RESP_AUTO_RESP_TF_DEBUG 0x29C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tbVapIndex : 5; //VAP ID captured on last TB ackable frame, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 tbDecisionType : 2; //TB decision made according to latest recieved TF , reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
		uint32 ackableTf : 1; //Ackable TF detected, reset value: 0x0, access type: RO
		uint32 tfLengthCheckValid : 1; //static indication after TF reception if the TF length check was valid or not, reset value: 0x0, access type: RO
		uint32 reserved2 : 14;
	} bitFields;
} RegAutoRespAutoRespTfDebug_u;

/*REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_BASE_ADDR 0x2B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMubarHwSwFifoBaseAddr : 22; //AUTO RESP MUBAR HW-SW FIFO base address, reset value: 0x0, access type: RW
		uint32 reserved0 : 10;
	} bitFields;
} RegAutoRespAutoRespMubarHwSwFifoBaseAddr_u;

/*REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_DEPTH_MINUS_ONE 0x2B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMubarHwSwFifoDepthMinusOne : 8; //AUTO RESP MUBAR HW-SW FIFO depth minus one, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegAutoRespAutoRespMubarHwSwFifoDepthMinusOne_u;

/*REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_CLEAR_STRB 0x2B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMubarHwSwFifoClearStrb : 1; //Clear AUTO RESP MUBAR HW-SW FIFO. , By write '1'., reset value: 0x0, access type: WO
		uint32 autoRespMubarHwSwFifoClearFullDropCtrStrb : 1; //Clear AUTO RESP MUBAR HW-SW FIFO full drop counter. , By write '1'., reset value: 0x0, access type: WO
		uint32 autoRespMubarHwSwFifoClearDecLessThanZeroStrb : 1; //Clear AUTO RESP MUBAR HW-SW FIFO decrement amount less than zero indication. , By write '1'.  , , reset value: 0x0, access type: WO
		uint32 reserved0 : 29;
	} bitFields;
} RegAutoRespAutoRespMubarHwSwFifoClearStrb_u;

/*REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_RD_ENTRIES_NUM 0x2BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMubarHwSwFifoRdEntriesNum : 9; //AUTO RESP MUBAR HW-SW FIFO number of entries to decrement., reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegAutoRespAutoRespMubarHwSwFifoRdEntriesNum_u;

/*REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_NUM_ENTRIES_COUNT 0x2C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMubarHwSwFifoNumEntriesCount : 9; //AUTO RESP MUBAR HW-SW FIFO number of entries count, reset value: 0x0, access type: RO
		uint32 reserved0 : 23;
	} bitFields;
} RegAutoRespAutoRespMubarHwSwFifoNumEntriesCount_u;

/*REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_DEBUG 0x2C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMubarHwSwFifoWrPtr : 8; //AUTO RESP MUBAR HW-SW FIFO write pointer index, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
		uint32 autoRespMubarHwSwFifoNotEmpty : 1; //AUTO RESP MUBAR HW-SW FIFO not empty indication, reset value: 0x0, access type: RO
		uint32 autoRespMubarHwSwFifoFull : 1; //AUTO RESP MUBAR HW-SW FIFO full indication, reset value: 0x0, access type: RO
		uint32 autoRespMubarHwSwFifoDecrementLessThanZero : 1; //AUTO RESP MUBAR HW-SW FIFO amount has been decremented less than zero, reset value: 0x0, access type: RO
		uint32 reserved1 : 5;
		uint32 autoRespMubarHwSwFifoFullDropCtr : 8; //AUTO RESP MUBAR HW-SW FIFO full drop counter, reset value: 0x0, access type: RO
	} bitFields;
} RegAutoRespAutoRespMubarHwSwFifoDebug_u;

/*REG_AUTO_RESP_AUTO_RESP_MUBAR_HW_SW_FIFO_EN 0x2C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespMubarHwSwFifoEn : 1; //AUTO RESP MUBAR HW-SW FIFO enabled., reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegAutoRespAutoRespMubarHwSwFifoEn_u;

/*REG_AUTO_RESP_STD_DB_GENERAL 0x300 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 unicastBroadcastN : 1; //unicast/broadcast format - sets transmission format in HE MU to be Unicast(=1) or Broadcast (=0 default), reset value: 0x0, access type: RW
		uint32 primaryTransmittedVapid : 5; //Primary transmitted BSSID VAP ID, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegAutoRespStdDbGeneral_u;



#endif // _AUTO_RESP_REGS_H_
