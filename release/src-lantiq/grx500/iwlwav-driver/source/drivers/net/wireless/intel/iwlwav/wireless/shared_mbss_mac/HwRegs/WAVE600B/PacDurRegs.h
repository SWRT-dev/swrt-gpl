
/***********************************************************************************
File:				PacDurRegs.h
Module:				PacDur
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_DUR_REGS_H_
#define _PAC_DUR_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PAC_DUR_BASE_ADDRESS                             MEMORY_MAP_UNIT_10_BASE_ADDRESS
#define	REG_PAC_DUR_MISC                                  (PAC_DUR_BASE_ADDRESS + 0xC)
#define	REG_PAC_DUR_TSF_FIELD_TIME                        (PAC_DUR_BASE_ADDRESS + 0x1C)
#define	REG_PAC_DUR_TXOP_JUMP_TABLE_BASE                  (PAC_DUR_BASE_ADDRESS + 0x50)
#define	REG_PAC_DUR_TX_MPDU_DUR_FIELD                     (PAC_DUR_BASE_ADDRESS + 0x54)
#define	REG_PAC_DUR_TX_NAV_TIMER_AT_TX_EN_RISE            (PAC_DUR_BASE_ADDRESS + 0x58)
#define	REG_PAC_DUR_TSF_FIELD_TIME1                       (PAC_DUR_BASE_ADDRESS + 0x84)
#define	REG_PAC_DUR_TSF_FIELD_TIME2                       (PAC_DUR_BASE_ADDRESS + 0x88)
#define	REG_PAC_DUR_TSF_FIELD_TIME3                       (PAC_DUR_BASE_ADDRESS + 0x8C)
#define	REG_PAC_DUR_PHY_PAC_TX_AIR_TIME_RESULT            (PAC_DUR_BASE_ADDRESS + 0x94)
#define	REG_PAC_DUR_DURATION_CALCULATION_RESULT           (PAC_DUR_BASE_ADDRESS + 0x9C)
#define	REG_PAC_DUR_TSF_ADJUST_DATA_IN                    (PAC_DUR_BASE_ADDRESS + 0xA0)
#define	REG_PAC_DUR_PHY_DEBUG_OVERRIDE_MAC_PHY_SIGNALS    (PAC_DUR_BASE_ADDRESS + 0xF0)
#define	REG_PAC_DUR_TSF_OFFSET_INDEX                      (PAC_DUR_BASE_ADDRESS + 0xF4)
#define	REG_PAC_DUR_DUR_TIMER_COMPENSATION                (PAC_DUR_BASE_ADDRESS + 0xF8)
#define	REG_PAC_DUR_TX_FRAME_ADDR3_SELECT_VAP             (PAC_DUR_BASE_ADDRESS + 0xFC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_DUR_MISC 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerMgmt : 1; //DUR  power mgmt, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegPacDurMisc_u;

/*REG_PAC_DUR_TSF_FIELD_TIME 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime : 9; //DUR  tsf field time, reset value: 0x0, access type: RW
		uint32 reserved0 : 23;
	} bitFields;
} RegPacDurTsfFieldTime_u;

/*REG_PAC_DUR_TXOP_JUMP_TABLE_BASE 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 6;
		uint32 txopJumpTableBase : 18; //TXOP jump table base, reset value: 0x0, access type: RW
		uint32 reserved1 : 8;
	} bitFields;
} RegPacDurTxopJumpTableBase_u;

/*REG_PAC_DUR_TX_MPDU_DUR_FIELD 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMpduDurField : 16; //Tx MPDU duration field, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegPacDurTxMpduDurField_u;

/*REG_PAC_DUR_TX_NAV_TIMER_AT_TX_EN_RISE 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNavTimerAtTxEnRise : 18; //Sample Tx NAV at the rise of MAC-PHY Tx enable, reset value: 0x0, access type: RO
		uint32 reserved0 : 14;
	} bitFields;
} RegPacDurTxNavTimerAtTxEnRise_u;

/*REG_PAC_DUR_TSF_FIELD_TIME1 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime1 : 25; //TSF field time 1 (multiple BSSID), reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPacDurTsfFieldTime1_u;

/*REG_PAC_DUR_TSF_FIELD_TIME2 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime2 : 25; //TSF field time 2 (multiple BSSID), reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPacDurTsfFieldTime2_u;

/*REG_PAC_DUR_TSF_FIELD_TIME3 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime3 : 25; //TSF field time 3 (multiple BSSID), reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
	} bitFields;
} RegPacDurTsfFieldTime3_u;

/*REG_PAC_DUR_PHY_PAC_TX_AIR_TIME_RESULT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPacTxAirTimeResult : 17; //PHY-MAC Tx air time result, reset value: 0x0, access type: RO
		uint32 reserved0 : 15;
	} bitFields;
} RegPacDurPhyPacTxAirTimeResult_u;

/*REG_PAC_DUR_DURATION_CALCULATION_RESULT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyDur : 16; //duration calculation result, reset value: 0x0, access type: RO
		uint32 autoReplyBfReportDur : 16; //BF report duration calculation result, reset value: 0x0, access type: RO
	} bitFields;
} RegPacDurDurationCalculationResult_u;

/*REG_PAC_DUR_TSF_ADJUST_DATA_IN 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfAdjustDataIn : 24; //tsf adjust data in, reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegPacDurTsfAdjustDataIn_u;

/*REG_PAC_DUR_PHY_DEBUG_OVERRIDE_MAC_PHY_SIGNALS 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyDebugOverrideLastVapIdxEn : 1; //Enable override MAC PHY last VAP index, reset value: 0x0, access type: RW
		uint32 reserved0 : 7;
		uint32 phyDebugOverrideLastVapIdx : 4; //The override value of MAC PHY last VAP index, reset value: 0x0, access type: RW
		uint32 reserved1 : 20;
	} bitFields;
} RegPacDurPhyDebugOverrideMacPhySignals_u;

/*REG_PAC_DUR_TSF_OFFSET_INDEX 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfOffsetIndex : 5; //TSF offset index from HOB, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegPacDurTsfOffsetIndex_u;

/*REG_PAC_DUR_DUR_TIMER_COMPENSATION 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 durTimerRxrdyCompensation : 5; //Dur timer RxRdy compensation, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 durTimerTxenCompensation : 5; //Dur timer TxEn compensation, reset value: 0x0, access type: RW
		uint32 reserved1 : 19;
	} bitFields;
} RegPacDurDurTimerCompensation_u;

/*REG_PAC_DUR_TX_FRAME_ADDR3_SELECT_VAP 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txFrameAddr3SelectVap : 32; //Tx frame address 3 selection, 1bit per VAP: , 0: the same as address2 (i.e. AP mode). , 1: the same as address1 (i.e. STA mode). , , reset value: 0x0, access type: RW
	} bitFields;
} RegPacDurTxFrameAddr3SelectVap_u;



#endif // _PAC_DUR_REGS_H_
