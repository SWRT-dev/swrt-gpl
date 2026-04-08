
/***********************************************************************************
File:				TxHandlerRegs.h
Module:				txHandler
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TX_HANDLER_REGS_H_
#define _TX_HANDLER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TX_HANDLER_BASE_ADDRESS                             MEMORY_MAP_UNIT_23_BASE_ADDRESS
#define	REG_TX_HANDLER_AC_ID_PRIORITY                      (TX_HANDLER_BASE_ADDRESS + 0x20)
#define	REG_TX_HANDLER_WLAN_RETRY_CTR_LIMIT                (TX_HANDLER_BASE_ADDRESS + 0x24)
#define	REG_TX_HANDLER_AC_EXT_CW_REMAPPING                 (TX_HANDLER_BASE_ADDRESS + 0x28)
#define	REG_TX_HANDLER_LCG_RAND_FUNC_SEED                  (TX_HANDLER_BASE_ADDRESS + 0x2C)
#define	REG_TX_HANDLER_LCG_RAND_FUNC_SHIFT_NUM             (TX_HANDLER_BASE_ADDRESS + 0x30)
#define	REG_TX_HANDLER_TXH_MAP_CONTROL                     (TX_HANDLER_BASE_ADDRESS + 0x34)
#define	REG_TX_HANDLER_TX_REQ_VAP_AC_BE                    (TX_HANDLER_BASE_ADDRESS + 0x40)
#define	REG_TX_HANDLER_TX_REQ_VAP_AC_BK                    (TX_HANDLER_BASE_ADDRESS + 0x44)
#define	REG_TX_HANDLER_TX_REQ_VAP_AC_VI                    (TX_HANDLER_BASE_ADDRESS + 0x48)
#define	REG_TX_HANDLER_TX_REQ_VAP_AC_VO                    (TX_HANDLER_BASE_ADDRESS + 0x4C)
#define	REG_TX_HANDLER_TX_REQ_VAP_AC_GPLP                  (TX_HANDLER_BASE_ADDRESS + 0x50)
#define	REG_TX_HANDLER_TX_REQ_VAP_AC_GPHP                  (TX_HANDLER_BASE_ADDRESS + 0x54)
#define	REG_TX_HANDLER_TX_REQ_VAP_AC_BEACON                (TX_HANDLER_BASE_ADDRESS + 0x58)
#define	REG_TX_HANDLER_LAST_SLOT_VAP_AC_BE                 (TX_HANDLER_BASE_ADDRESS + 0x5C)
#define	REG_TX_HANDLER_LAST_SLOT_VAP_AC_BK                 (TX_HANDLER_BASE_ADDRESS + 0x60)
#define	REG_TX_HANDLER_LAST_SLOT_VAP_AC_VI                 (TX_HANDLER_BASE_ADDRESS + 0x64)
#define	REG_TX_HANDLER_LAST_SLOT_VAP_AC_VO                 (TX_HANDLER_BASE_ADDRESS + 0x68)
#define	REG_TX_HANDLER_LAST_SLOT_VAP_AC_GPLP               (TX_HANDLER_BASE_ADDRESS + 0x6C)
#define	REG_TX_HANDLER_LAST_SLOT_VAP_AC_GPHP               (TX_HANDLER_BASE_ADDRESS + 0x70)
#define	REG_TX_HANDLER_LAST_SLOT_VAP_AC_BEACON             (TX_HANDLER_BASE_ADDRESS + 0x74)
#define	REG_TX_HANDLER_LAST_VAP_WINNER_AC_ID               (TX_HANDLER_BASE_ADDRESS + 0x78)
#define	REG_TX_HANDLER_WINNER_RESULT                       (TX_HANDLER_BASE_ADDRESS + 0x7C)
#define	REG_TX_HANDLER_UNFREEZE_REPORT_SET                 (TX_HANDLER_BASE_ADDRESS + 0x80)
#define	REG_TX_HANDLER_UNFREEZE_REPORT                     (TX_HANDLER_BASE_ADDRESS + 0x84)
#define	REG_TX_HANDLER_TXH_MAP_DBG                         (TX_HANDLER_BASE_ADDRESS + 0x88)
#define	REG_TX_HANDLER_RECIPE_PTR_AC_ID0TO3                (TX_HANDLER_BASE_ADDRESS + 0x8C)
#define	REG_TX_HANDLER_RECIPE_PTR_AC_GPLP                  (TX_HANDLER_BASE_ADDRESS + 0x90)
#define	REG_TX_HANDLER_RECIPE_PTR_AC_GPHP                  (TX_HANDLER_BASE_ADDRESS + 0x94)
#define	REG_TX_HANDLER_RECIPE_PTR_AC_BEACON                (TX_HANDLER_BASE_ADDRESS + 0x98)
#define	REG_TX_HANDLER_TXH_NTD_TX_REQ_PARAMS               (TX_HANDLER_BASE_ADDRESS + 0x9C)
#define	REG_TX_HANDLER_TXH_EPSILON_MICRO_PRESCALER_NUM     (TX_HANDLER_BASE_ADDRESS + 0xA0)
#define	REG_TX_HANDLER_TXH_EPSILON_MICRO_NUM               (TX_HANDLER_BASE_ADDRESS + 0xA4)
#define	REG_TX_HANDLER_TXH_LOGGER                          (TX_HANDLER_BASE_ADDRESS + 0xA8)
#define	REG_TX_HANDLER_TXH_LOGGER_ACTIVE                   (TX_HANDLER_BASE_ADDRESS + 0xAC)
#define	REG_TX_HANDLER_TXH_PAUSER_COMMAND                  (TX_HANDLER_BASE_ADDRESS + 0x100)
#define	REG_TX_HANDLER_TXH_PAUSER_REP_CYCLE_COUNT          (TX_HANDLER_BASE_ADDRESS + 0x104)
#define	REG_TX_HANDLER_TXH_PAUSER_REP_DURATION             (TX_HANDLER_BASE_ADDRESS + 0x108)
#define	REG_TX_HANDLER_TXH_PAUSER_STATUS                   (TX_HANDLER_BASE_ADDRESS + 0x10C)
#define	REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BE                (TX_HANDLER_BASE_ADDRESS + 0x110)
#define	REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BK                (TX_HANDLER_BASE_ADDRESS + 0x114)
#define	REG_TX_HANDLER_TXH_PAUSER_VAP_AC_VI                (TX_HANDLER_BASE_ADDRESS + 0x118)
#define	REG_TX_HANDLER_TXH_PAUSER_VAP_AC_VO                (TX_HANDLER_BASE_ADDRESS + 0x11C)
#define	REG_TX_HANDLER_TXH_PAUSER_VAP_AC_GPLP              (TX_HANDLER_BASE_ADDRESS + 0x120)
#define	REG_TX_HANDLER_TXH_PAUSER_VAP_AC_GPHP              (TX_HANDLER_BASE_ADDRESS + 0x124)
#define	REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BEACON            (TX_HANDLER_BASE_ADDRESS + 0x128)
#define	REG_TX_HANDLER_TXH_PAUSER_DBG                      (TX_HANDLER_BASE_ADDRESS + 0x12C)
#define	REG_TX_HANDLER_TXH_PAUSER_IRQ                      (TX_HANDLER_BASE_ADDRESS + 0x130)
#define	REG_TX_HANDLER_TXH_PAUSER_CLEAR_IRQ                (TX_HANDLER_BASE_ADDRESS + 0x134)
#define	REG_TX_HANDLER_TXH_PAUSER_GENERATE_IRQ_COMMANDS    (TX_HANDLER_BASE_ADDRESS + 0x138)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TX_HANDLER_AC_ID_PRIORITY 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 acIdPriority0:3;	// AC_ID priority0
		uint32 reserved0:1;
		uint32 acIdPriority1:3;	// AC_ID priority1
		uint32 reserved1:1;
		uint32 acIdPriority2:3;	// AC_ID priority2
		uint32 reserved2:1;
		uint32 acIdPriority3:3;	// AC_ID priority3
		uint32 reserved3:1;
		uint32 acIdPriority4:3;	// AC_ID priority4
		uint32 reserved4:1;
		uint32 acIdPriority5:3;	// AC_ID priority5
		uint32 reserved5:1;
		uint32 acIdPriority6:3;	// AC_ID priority6
		uint32 reserved6:5;
	} bitFields;
} RegTxHandlerAcIdPriority_u;

/*REG_TX_HANDLER_WLAN_RETRY_CTR_LIMIT 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wlanShortRetryCtrLimit:8;	// WLAN Short Retry limit
		uint32 wlanLongRetryCtrLimit:8;	// WLAN Long Retry limit
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerWlanRetryCtrLimit_u;

/*REG_TX_HANDLER_AC_EXT_CW_REMAPPING 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 acGplpCwRemapping:3;	// AC_GPLP CW remapping
		uint32 reserved0:1;
		uint32 acGphpCwRemapping:3;	// AC_GPHP CW remapping
		uint32 reserved1:1;
		uint32 acBeaconCwRemapping:3;	// AC_BEACON CW remapping
		uint32 reserved2:21;
	} bitFields;
} RegTxHandlerAcExtCwRemapping_u;

/*REG_TX_HANDLER_LCG_RAND_FUNC_SEED 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcgRandFuncSeed:32;	// LCG random function seed
	} bitFields;
} RegTxHandlerLcgRandFuncSeed_u;

/*REG_TX_HANDLER_LCG_RAND_FUNC_SHIFT_NUM 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcgRandFuncShiftNum:5;	// LCG random function shift number
		uint32 reserved0:27;
	} bitFields;
} RegTxHandlerLcgRandFuncShiftNum_u;

/*REG_TX_HANDLER_TXH_MAP_CONTROL 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhMapEnable:1;	// Tx Handler MAP enable
		uint32 freezeInAggBuilderErrorEn:1;	// freeze in AGG builder error enable
		uint32 punishOnInternalCollisionEn:1;	// Punish on internal collision enable
		uint32 increaseOnIntColSsrcSlrcN:1;	// Increase retry counter on interanl collision
		uint32 txGroupAddressedSetEcwminEn:1;	// Tx group addressed set ECWmin enable
		uint32 retryCtrLimitZeroedCtrEn:1;	// Retry ctr limit zeroed ctr enable
		uint32 reserved0:26;
	} bitFields;
} RegTxHandlerTxhMapControl_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_BE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcBe:16;	// Tx request VAP AC_BE vector internal
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxReqVapAcBe_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_BK 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcBk:16;	// Tx request VAP AC_BK vector internal
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxReqVapAcBk_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_VI 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcVi:16;	// Tx request VAP AC_VI vector internal
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxReqVapAcVi_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_VO 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcVo:16;	// Tx request VAP AC_VO vector internal
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxReqVapAcVo_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_GPLP 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcGplp:16;	// Tx request VAP AC_GPLP vector internal
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxReqVapAcGplp_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_GPHP 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcGphp:16;	// Tx request VAP AC_GPHP vector internal
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxReqVapAcGphp_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_BEACON 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcBeacon:16;	// Tx request VAP AC_BEACON vector internal
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxReqVapAcBeacon_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_BE 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcBe:16;	// Last slot VAP AC_BE vector
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerLastSlotVapAcBe_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_BK 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcBk:16;	// Last slot VAP AC_BK vector
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerLastSlotVapAcBk_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_VI 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcVi:16;	// Last slot VAP AC_VI vector
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerLastSlotVapAcVi_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_VO 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcVo:16;	// Last slot VAP AC_VO vector
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerLastSlotVapAcVo_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_GPLP 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcGplp:16;	// Last slot VAP AC_GPLP vector
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerLastSlotVapAcGplp_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_GPHP 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcGphp:16;	// Last slot VAP AC_GPHP vector
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerLastSlotVapAcGphp_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_BEACON 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcBeacon:16;	// Last slot VAP AC_BEACON vector
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerLastSlotVapAcBeacon_u;

/*REG_TX_HANDLER_LAST_VAP_WINNER_AC_ID 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastVapWinnerAcBe:4;	// Last VAP winner for AC_BE
		uint32 lastVapWinnerAcBk:4;	// Last VAP winner for AC_BK
		uint32 lastVapWinnerAcVi:4;	// Last VAP winner for AC_VI
		uint32 lastVapWinnerAcVo:4;	// Last VAP winner for AC_VO
		uint32 lastVapWinnerAcGplp:4;	// Last VAP winner for AC_GPLP
		uint32 lastVapWinnerAcGphp:4;	// Last VAP winner for AC_GPHP
		uint32 lastVapWinnerAcBeacon:4;	// Last VAP winner for AC_BEACON
		uint32 reserved0:4;
	} bitFields;
} RegTxHandlerLastVapWinnerAcId_u;

/*REG_TX_HANDLER_WINNER_RESULT 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 winnerVapId:4;	// winner VAP ID
		uint32 winnerAcId:3;	// winner AC ID
		uint32 reserved0:25;
	} bitFields;
} RegTxHandlerWinnerResult_u;

/*REG_TX_HANDLER_UNFREEZE_REPORT_SET 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 unfreezeCommand:1;	// Set Unfreeze command
		uint32 ctsRxInfoUpdate:1;	// CTS Rx information update
		uint32 txResultUpdate:1;	// Tx result update
		uint32 reserved0:5;
		uint32 ctsRxInfo:2;	// CTS Rx information
		uint32 reserved1:6;
		uint32 txResult:2;	// Tx result
		uint32 txFrameLength:1;	// Tx frame length
		uint32 reserved2:13;
	} bitFields;
} RegTxHandlerUnfreezeReportSet_u;

/*REG_TX_HANDLER_UNFREEZE_REPORT 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 ctsRxInfoRd:2;	// CTS Rx information read
		uint32 reserved1:6;
		uint32 txResultRd:2;	// Tx result read
		uint32 txFrameLengthRd:1;	// Tx frame length read
		uint32 reserved2:13;
	} bitFields;
} RegTxHandlerUnfreezeReport_u;

/*REG_TX_HANDLER_TXH_MAP_DBG 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhMapSm:5;	// TXH MAP state machine
		uint32 reserved0:3;
		uint32 pauseFdbkSm:2;	// pause fdbk state machine
		uint32 reserved1:2;
		uint32 txhMapHandlerSm:4;	// TXH MAP handler state machine
		uint32 txhMapInFreeze:1;	// TXH MAP  in freeze
		uint32 lastSlotMaskedVapAc:1;	// last slot masked vap_ac
		uint32 txReqVapAc:1;	// Tx req vap_ac
		uint32 txSelTxhdVapAc:1;	// tx_sel txhd vap_ac
		uint32 mapHandlerOperation:4;	// map handler operation
		uint32 reserved2:8;
	} bitFields;
} RegTxHandlerTxhMapDbg_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_ID0TO3 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcId0To3:22;	// Recipe pointer AC_ID 0to3
		uint32 reserved0:10;
	} bitFields;
} RegTxHandlerRecipePtrAcId0To3_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_GPLP 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcGplp:22;	// Recipe pointer AC_ID GPLP
		uint32 reserved0:10;
	} bitFields;
} RegTxHandlerRecipePtrAcGplp_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_GPHP 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcGphp:22;	// Recipe pointer AC_ID GPHP
		uint32 reserved0:10;
	} bitFields;
} RegTxHandlerRecipePtrAcGphp_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_BEACON 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcBeacon:22;	// Recipe pointer AC_ID Beacon
		uint32 reserved0:10;
	} bitFields;
} RegTxHandlerRecipePtrAcBeacon_u;

/*REG_TX_HANDLER_TXH_NTD_TX_REQ_PARAMS 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhNtdCcaSensitive:1;	// TXH to NTD CCA sensitive
		uint32 txhNtdRxNavSensitive:1;	// TXH to NTD Rx NAV sensitive
		uint32 txhNtdTxNavSensitive:1;	// TXH to NTD Tx NAV sensitive
		uint32 reserved0:29;
	} bitFields;
} RegTxHandlerTxhNtdTxReqParams_u;

/*REG_TX_HANDLER_TXH_EPSILON_MICRO_PRESCALER_NUM 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhEpsilonMicroPrescalerNum:10;	// TxH HW epsilon micro prescaler num
		uint32 reserved0:6;
		uint32 txhEpsilonMicroPrescalerNumFreqReduced:10;	// TxH HW epsilon micro prescaler num freq reduced
		uint32 reserved1:6;
	} bitFields;
} RegTxHandlerTxhEpsilonMicroPrescalerNum_u;

/*REG_TX_HANDLER_TXH_EPSILON_MICRO_NUM 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhEpsilonMicroNum:8;	// TxH HW epsilon micro num
		uint32 reserved0:24;
	} bitFields;
} RegTxHandlerTxhEpsilonMicroNum_u;

/*REG_TX_HANDLER_TXH_LOGGER 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhLoggerEn:1;	// Tx Handler logger enable
		uint32 reserved0:7;
		uint32 txhLoggerPriority:2;	// Tx Handler logger priority
		uint32 reserved1:22;
	} bitFields;
} RegTxHandlerTxhLogger_u;

/*REG_TX_HANDLER_TXH_LOGGER_ACTIVE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhLoggerActive:1;	// Tx Handler logger active
		uint32 reserved0:31;
	} bitFields;
} RegTxHandlerTxhLoggerActive_u;

/*REG_TX_HANDLER_TXH_PAUSER_COMMAND 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserCommand:4;	// TXH Pauser command
		uint32 txhPauserRepetitionType:1;	// TXH Pauser repetition type
		uint32 reserved0:3;
		uint32 txhPauserAcVector:8;	// TXH Pauser AC vector
		uint32 txhPauserVapVector:16;	// TXH Pauser VAP vector
	} bitFields;
} RegTxHandlerTxhPauserCommand_u;

/*REG_TX_HANDLER_TXH_PAUSER_REP_CYCLE_COUNT 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserRepCycleCount:8;	// TXH Pauser repetition cycle count
		uint32 reserved0:24;
	} bitFields;
} RegTxHandlerTxhPauserRepCycleCount_u;

/*REG_TX_HANDLER_TXH_PAUSER_REP_DURATION 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserRepPauseDur:16;	// TXH Pauser repetition pause duration
		uint32 txhPauserRepResumeDur:16;	// TXH Pauser repetition resume duration
	} bitFields;
} RegTxHandlerTxhPauserRepDuration_u;

/*REG_TX_HANDLER_TXH_PAUSER_STATUS 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcInUsed:1;	// TXH Pauser vap ac in used
		uint32 txhPauserWaitForConfirm:1;	// TXH Pauser wait for confirm
		uint32 txhPauserRepModeSeq:1;	// TXH pauser repetition mode seq
		uint32 txhPauserRepLimitedInfiniteN:1;	// TXH Pauser repetition limited infinite_n
		uint32 txhPauserRepPausePeriod:1;	// TXH Pauser repetition pause period
		uint32 reserved0:3;
		uint32 txhPauserRepCounter:8;	// TXH Pauser repetition counter
		uint32 reserved1:16;
	} bitFields;
} RegTxHandlerTxhPauserStatus_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BE 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcBe:16;	// TXH Pauser VAP AC_BE
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserVapAcBe_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BK 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcBk:16;	// TXH Pauser VAP AC_BK
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserVapAcBk_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_VI 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcVi:16;	// TXH Pauser VAP AC_VI
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserVapAcVi_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_VO 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcVo:16;	// TXH Pauser VAP AC_VO
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserVapAcVo_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_GPLP 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcGplp:16;	// TXH Pauser VAP AC_GPLP
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserVapAcGplp_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_GPHP 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcGphp:16;	// TXH Pauser VAP AC_GPHP
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserVapAcGphp_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BEACON 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcBeacon:16;	// TXH Pauser VAP AC_BEACON
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserVapAcBeacon_u;

/*REG_TX_HANDLER_TXH_PAUSER_DBG 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserSm:4;	// TXH Pauser state machine
		uint32 reserved0:28;
	} bitFields;
} RegTxHandlerTxhPauserDbg_u;

/*REG_TX_HANDLER_TXH_PAUSER_IRQ 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserIrq:1;	// TXH Pauser IRQ
		uint32 reserved0:31;
	} bitFields;
} RegTxHandlerTxhPauserIrq_u;

/*REG_TX_HANDLER_TXH_PAUSER_CLEAR_IRQ 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserClearIrq:1;	// TXH Pauser clear IRQ
		uint32 reserved0:31;
	} bitFields;
} RegTxHandlerTxhPauserClearIrq_u;

/*REG_TX_HANDLER_TXH_PAUSER_GENERATE_IRQ_COMMANDS 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserGenerateIrqCommands:16;	// TXH Pauser generate IRQ per command 
		uint32 reserved0:16;
	} bitFields;
} RegTxHandlerTxhPauserGenerateIrqCommands_u;



#endif // _TX_HANDLER_REGS_H_
