
/***********************************************************************************
File:				TxHandlerRegs.h
Module:				TxHandler
SOC Revision:		
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
#define	REG_TX_HANDLER_LAST_VAP_WINNER_AC_ID0TO3           (TX_HANDLER_BASE_ADDRESS + 0x18)
#define	REG_TX_HANDLER_LAST_VAP_WINNER_AC_ID4TO7           (TX_HANDLER_BASE_ADDRESS + 0x1C)
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
#define	REG_TX_HANDLER_TXH_PAUSER_COMMAND_VAP_VECTOR       (TX_HANDLER_BASE_ADDRESS + 0x13C)
#define	REG_TX_HANDLER_TXH_PAUSER_COMMAND                  (TX_HANDLER_BASE_ADDRESS + 0x140)
#define	REG_TX_HANDLER_SPARE_REGISTERS                     (TX_HANDLER_BASE_ADDRESS + 0x1FC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TX_HANDLER_LAST_VAP_WINNER_AC_ID0TO3 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastVapWinnerAcBe : 5; //Last VAP winner for AC_BE, reset value: 0x0, access type: RO
		uint32 lastVapWinnerAcBk : 5; //Last VAP winner for AC_BK, reset value: 0x0, access type: RO
		uint32 lastVapWinnerAcVi : 5; //Last VAP winner for AC_VI, reset value: 0x0, access type: RO
		uint32 lastVapWinnerAcVo : 5; //Last VAP winner for AC_VO, reset value: 0x0, access type: RO
		uint32 reserved0 : 12;
	} bitFields;
} RegTxHandlerLastVapWinnerAcId0To3_u;

/*REG_TX_HANDLER_LAST_VAP_WINNER_AC_ID4TO7 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastVapWinnerAcGplp : 5; //Last VAP winner for AC_GPLP, reset value: 0x0, access type: RO
		uint32 lastVapWinnerAcGphp : 5; //Last VAP winner for AC_GPHP, reset value: 0x0, access type: RO
		uint32 lastVapWinnerAcBeacon : 5; //Last VAP winner for AC_BEACON, reset value: 0x0, access type: RO
		uint32 reserved0 : 17;
	} bitFields;
} RegTxHandlerLastVapWinnerAcId4To7_u;

/*REG_TX_HANDLER_AC_ID_PRIORITY 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 acIdPriority0 : 3; //AC_ID priority0, reset value: 0x5, access type: RW
		uint32 reserved0 : 1;
		uint32 acIdPriority1 : 3; //AC_ID priority1, reset value: 0x1, access type: RW
		uint32 reserved1 : 1;
		uint32 acIdPriority2 : 3; //AC_ID priority2, reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 acIdPriority3 : 3; //AC_ID priority3, reset value: 0x2, access type: RW
		uint32 reserved3 : 1;
		uint32 acIdPriority4 : 3; //AC_ID priority4, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 acIdPriority5 : 3; //AC_ID priority5, reset value: 0x6, access type: RW
		uint32 reserved5 : 1;
		uint32 acIdPriority6 : 3; //AC_ID priority6, reset value: 0x7, access type: RW
		uint32 reserved6 : 5;
	} bitFields;
} RegTxHandlerAcIdPriority_u;

/*REG_TX_HANDLER_WLAN_RETRY_CTR_LIMIT 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 wlanShortRetryCtrLimit : 8; //WLAN Short Retry limit, reset value: 0x7, access type: RW
		uint32 wlanLongRetryCtrLimit : 8; //WLAN Long Retry limit, reset value: 0x4, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegTxHandlerWlanRetryCtrLimit_u;

/*REG_TX_HANDLER_AC_EXT_CW_REMAPPING 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 acGplpCwRemapping : 3; //AC_GPLP CW remapping, reset value: 0x5, access type: RW
		uint32 reserved0 : 1;
		uint32 acGphpCwRemapping : 3; //AC_GPHP CW remapping, reset value: 0x6, access type: RW
		uint32 reserved1 : 1;
		uint32 acBeaconCwRemapping : 3; //AC_BEACON CW remapping, reset value: 0x7, access type: RW
		uint32 reserved2 : 21;
	} bitFields;
} RegTxHandlerAcExtCwRemapping_u;

/*REG_TX_HANDLER_LCG_RAND_FUNC_SEED 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcgRandFuncSeed : 32; //LCG random function seed, reset value: 0x12345678, access type: RW
	} bitFields;
} RegTxHandlerLcgRandFuncSeed_u;

/*REG_TX_HANDLER_LCG_RAND_FUNC_SHIFT_NUM 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lcgRandFuncShiftNum : 5; //LCG random function shift number, reset value: 0x10, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegTxHandlerLcgRandFuncShiftNum_u;

/*REG_TX_HANDLER_TXH_MAP_CONTROL 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhMapEnable : 1; //Tx Handler MAP enable, reset value: 0x1, access type: RW
		uint32 freezeInAggBuilderErrorEn : 1; //freeze in AGG builder error enable, reset value: 0x0, access type: RW
		uint32 punishOnInternalCollisionEn : 1; //Punish on internal collision enable, reset value: 0x1, access type: RW
		uint32 increaseOnIntColSsrcSlrcN : 1; //Increase retry counter on interanl collision, reset value: 0x1, access type: RW
		uint32 txGroupAddressedSetEcwminEn : 1; //Tx group addressed set ECWmin enable, reset value: 0x1, access type: RW
		uint32 retryCtrLimitZeroedCtrEn : 1; //Retry ctr limit zeroed ctr enable, reset value: 0x1, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegTxHandlerTxhMapControl_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_BE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcBe : 32; //Tx request VAP AC_BE vector internal, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerTxReqVapAcBe_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_BK 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcBk : 32; //Tx request VAP AC_BK vector internal, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerTxReqVapAcBk_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_VI 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcVi : 32; //Tx request VAP AC_VI vector internal, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerTxReqVapAcVi_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_VO 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcVo : 32; //Tx request VAP AC_VO vector internal, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerTxReqVapAcVo_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_GPLP 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcGplp : 32; //Tx request VAP AC_GPLP vector internal, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerTxReqVapAcGplp_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_GPHP 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcGphp : 32; //Tx request VAP AC_GPHP vector internal, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerTxReqVapAcGphp_u;

/*REG_TX_HANDLER_TX_REQ_VAP_AC_BEACON 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txReqVapAcBeacon : 32; //Tx request VAP AC_BEACON vector internal, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerTxReqVapAcBeacon_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_BE 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcBe : 32; //Last slot VAP AC_BE vector, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerLastSlotVapAcBe_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_BK 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcBk : 32; //Last slot VAP AC_BK vector, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerLastSlotVapAcBk_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_VI 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcVi : 32; //Last slot VAP AC_VI vector, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerLastSlotVapAcVi_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_VO 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcVo : 32; //Last slot VAP AC_VO vector, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerLastSlotVapAcVo_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_GPLP 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcGplp : 32; //Last slot VAP AC_GPLP vector, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerLastSlotVapAcGplp_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_GPHP 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcGphp : 32; //Last slot VAP AC_GPHP vector, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerLastSlotVapAcGphp_u;

/*REG_TX_HANDLER_LAST_SLOT_VAP_AC_BEACON 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastSlotVapAcBeacon : 32; //Last slot VAP AC_BEACON vector, reset value: 0x0, access type: RO
	} bitFields;
} RegTxHandlerLastSlotVapAcBeacon_u;

/*REG_TX_HANDLER_WINNER_RESULT 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 winnerVapId : 5; //winner VAP ID, reset value: 0x0, access type: RO
		uint32 winnerAcId : 3; //winner AC ID, reset value: 0x0, access type: RO
		uint32 reserved0 : 24;
	} bitFields;
} RegTxHandlerWinnerResult_u;

/*REG_TX_HANDLER_UNFREEZE_REPORT_SET 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 unfreezeCommand : 1; //Set Unfreeze command, reset value: 0x0, access type: WO
		uint32 ctsRxInfoUpdate : 1; //CTS Rx information update, reset value: 0x0, access type: WO
		uint32 txResultUpdate : 1; //Tx result update, reset value: 0x0, access type: WO
		uint32 reserved0 : 5;
		uint32 ctsRxInfo : 2; //CTS Rx information, reset value: 0x0, access type: WO
		uint32 reserved1 : 6;
		uint32 txResult : 2; //Tx result, reset value: 0x0, access type: WO
		uint32 txFrameLength : 1; //Tx frame length, reset value: 0x0, access type: WO
		uint32 reserved2 : 13;
	} bitFields;
} RegTxHandlerUnfreezeReportSet_u;

/*REG_TX_HANDLER_UNFREEZE_REPORT 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 8;
		uint32 ctsRxInfoRd : 2; //CTS Rx information read, reset value: 0x0, access type: RO
		uint32 reserved1 : 6;
		uint32 txResultRd : 2; //Tx result read, reset value: 0x0, access type: RO
		uint32 txFrameLengthRd : 1; //Tx frame length read, reset value: 0x0, access type: RO
		uint32 reserved2 : 13;
	} bitFields;
} RegTxHandlerUnfreezeReport_u;

/*REG_TX_HANDLER_TXH_MAP_DBG 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhMapSm : 5; //TXH MAP state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 pauseFdbkSm : 2; //pause fdbk state machine, reset value: 0x0, access type: RO
		uint32 reserved1 : 2;
		uint32 txhMapHandlerSm : 4; //TXH MAP handler state machine, reset value: 0x0, access type: RO
		uint32 txhMapInFreeze : 1; //TXH MAP  in freeze, reset value: 0x0, access type: RO
		uint32 lastSlotMaskedVapAc : 1; //last slot masked vap_ac, reset value: 0x0, access type: RO
		uint32 txReqVapAc : 1; //Tx req vap_ac, reset value: 0x0, access type: RO
		uint32 txSelTxhdVapAc : 1; //tx_sel txhd vap_ac, reset value: 0x0, access type: RO
		uint32 mapHandlerOperation : 4; //map handler operation, reset value: 0x0, access type: RO
		uint32 reserved2 : 8;
	} bitFields;
} RegTxHandlerTxhMapDbg_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_ID0TO3 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcId0To3 : 25; //Recipe pointer AC_ID 0to3, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegTxHandlerRecipePtrAcId0To3_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_GPLP 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcGplp : 25; //Recipe pointer AC_ID GPLP, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegTxHandlerRecipePtrAcGplp_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_GPHP 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcGphp : 25; //Recipe pointer AC_ID GPHP, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegTxHandlerRecipePtrAcGphp_u;

/*REG_TX_HANDLER_RECIPE_PTR_AC_BEACON 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 recipePtrAcBeacon : 25; //Recipe pointer AC_ID Beacon, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegTxHandlerRecipePtrAcBeacon_u;

/*REG_TX_HANDLER_TXH_NTD_TX_REQ_PARAMS 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhNtdCcaSensitive : 1; //TXH to NTD CCA sensitive, reset value: 0x1, access type: RW
		uint32 txhNtdRxNavSensitive : 1; //TXH to NTD Rx NAV sensitive, reset value: 0x1, access type: RW
		uint32 txhNtdTxNavSensitive : 1; //TXH to NTD Tx NAV sensitive, reset value: 0x1, access type: RW
		uint32 reserved0 : 29;
	} bitFields;
} RegTxHandlerTxhNtdTxReqParams_u;

/*REG_TX_HANDLER_TXH_EPSILON_MICRO_PRESCALER_NUM 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhEpsilonMicroPrescalerNum : 10; //TxH HW epsilon micro prescaler num, reset value: 0x9f, access type: RW
		uint32 reserved0 : 6;
		uint32 txhEpsilonMicroPrescalerNumFreqReduced : 10; //TxH HW epsilon micro prescaler num freq reduced, reset value: 0x0, access type: RW
		uint32 reserved1 : 6;
	} bitFields;
} RegTxHandlerTxhEpsilonMicroPrescalerNum_u;

/*REG_TX_HANDLER_TXH_EPSILON_MICRO_NUM 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhEpsilonMicroNum : 8; //TxH HW epsilon micro num, reset value: 0x5, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegTxHandlerTxhEpsilonMicroNum_u;

/*REG_TX_HANDLER_TXH_LOGGER 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhLoggerEn : 1; //Tx Handler logger enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 txhLoggerPriority : 2; //Tx Handler logger priority, reset value: 0x0, access type: RW
		uint32 reserved1 : 22;
	} bitFields;
} RegTxHandlerTxhLogger_u;

/*REG_TX_HANDLER_TXH_LOGGER_ACTIVE 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhLoggerActive : 1; //Tx Handler logger active, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegTxHandlerTxhLoggerActive_u;

/*REG_TX_HANDLER_TXH_PAUSER_REP_CYCLE_COUNT 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserRepCycleCount : 8; //TXH Pauser repetition cycle count, reset value: 0x0, access type: RW
		uint32 reserved0 : 24;
	} bitFields;
} RegTxHandlerTxhPauserRepCycleCount_u;

/*REG_TX_HANDLER_TXH_PAUSER_REP_DURATION 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserRepPauseDur : 16; //TXH Pauser repetition pause duration, reset value: 0x0, access type: RW
		uint32 txhPauserRepResumeDur : 16; //TXH Pauser repetition resume duration, reset value: 0x0, access type: RW
	} bitFields;
} RegTxHandlerTxhPauserRepDuration_u;

/*REG_TX_HANDLER_TXH_PAUSER_STATUS 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcInUsed : 1; //TXH Pauser vap ac in used, reset value: 0x0, access type: RO
		uint32 txhPauserWaitForConfirm : 1; //TXH Pauser wait for confirm, reset value: 0x0, access type: RO
		uint32 txhPauserRepModeSeq : 1; //TXH pauser repetition mode seq, reset value: 0x0, access type: RO
		uint32 txhPauserRepLimitedInfiniteN : 1; //TXH Pauser repetition limited infinite_n, reset value: 0x0, access type: RO
		uint32 txhPauserRepPausePeriod : 1; //TXH Pauser repetition pause period, reset value: 0x0, access type: RO
		uint32 reserved0 : 3;
		uint32 txhPauserRepCounter : 8; //TXH Pauser repetition counter, reset value: 0x0, access type: RO
		uint32 reserved1 : 16;
	} bitFields;
} RegTxHandlerTxhPauserStatus_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BE 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcBe : 32; //TXH Pauser VAP AC_BE, reset value: 0xffff, access type: RO
	} bitFields;
} RegTxHandlerTxhPauserVapAcBe_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BK 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcBk : 32; //TXH Pauser VAP AC_BK, reset value: 0xffff, access type: RO
	} bitFields;
} RegTxHandlerTxhPauserVapAcBk_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_VI 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcVi : 32; //TXH Pauser VAP AC_VI, reset value: 0xffff, access type: RO
	} bitFields;
} RegTxHandlerTxhPauserVapAcVi_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_VO 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcVo : 32; //TXH Pauser VAP AC_VO, reset value: 0xffff, access type: RO
	} bitFields;
} RegTxHandlerTxhPauserVapAcVo_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_GPLP 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcGplp : 32; //TXH Pauser VAP AC_GPLP, reset value: 0xffff, access type: RO
	} bitFields;
} RegTxHandlerTxhPauserVapAcGplp_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_GPHP 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcGphp : 32; //TXH Pauser VAP AC_GPHP, reset value: 0xffff, access type: RO
	} bitFields;
} RegTxHandlerTxhPauserVapAcGphp_u;

/*REG_TX_HANDLER_TXH_PAUSER_VAP_AC_BEACON 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapAcBeacon : 32; //TXH Pauser VAP AC_BEACON, reset value: 0xffff, access type: RO
	} bitFields;
} RegTxHandlerTxhPauserVapAcBeacon_u;

/*REG_TX_HANDLER_TXH_PAUSER_DBG 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserSm : 4; //TXH Pauser state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegTxHandlerTxhPauserDbg_u;

/*REG_TX_HANDLER_TXH_PAUSER_IRQ 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserIrq : 1; //TXH Pauser IRQ, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegTxHandlerTxhPauserIrq_u;

/*REG_TX_HANDLER_TXH_PAUSER_CLEAR_IRQ 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserClearIrq : 1; //TXH Pauser clear IRQ, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegTxHandlerTxhPauserClearIrq_u;

/*REG_TX_HANDLER_TXH_PAUSER_GENERATE_IRQ_COMMANDS 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserGenerateIrqCommands : 16; //TXH Pauser generate IRQ per command , reset value: 0xffff, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegTxHandlerTxhPauserGenerateIrqCommands_u;

/*REG_TX_HANDLER_TXH_PAUSER_COMMAND_VAP_VECTOR 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserVapVector : 32; //TXH Pauser VAP vector, reset value: 0x0, access type: RW
	} bitFields;
} RegTxHandlerTxhPauserCommandVapVector_u;

/*REG_TX_HANDLER_TXH_PAUSER_COMMAND 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserCommand : 4; //TXH Pauser command, reset value: 0x0, access type: RW
		uint32 txhPauserRepetitionType : 1; //TXH Pauser repetition type, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 txhPauserAcVector : 8; //TXH Pauser AC vector, reset value: 0x0, access type: RW
		uint32 reserved1 : 16;
	} bitFields;
} RegTxHandlerTxhPauserCommand_u;

/*REG_TX_HANDLER_SPARE_REGISTERS 0x1FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 spareRegisters : 16; //Spare registers, reset value: 0x8888, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegTxHandlerSpareRegisters_u;



#endif // _TX_HANDLER_REGS_H_
