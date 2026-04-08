
/***********************************************************************************
File:				PacDurRegs.h
Module:				pacDur
SOC Revision:		843
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
#define	REG_PAC_DUR_CFP_END_TIME                                (PAC_DUR_BASE_ADDRESS + 0x4)
#define	REG_PAC_DUR_TSF_START_TIME_ADJUST                       (PAC_DUR_BASE_ADDRESS + 0x8)
#define	REG_PAC_DUR_MISC                                        (PAC_DUR_BASE_ADDRESS + 0xC)
#define	REG_PAC_DUR_NAV_ADJUST                                  (PAC_DUR_BASE_ADDRESS + 0x10)
#define	REG_PAC_DUR_ACK_CONTROL                                 (PAC_DUR_BASE_ADDRESS + 0x18)
#define	REG_PAC_DUR_TSF_FIELD_TIME                              (PAC_DUR_BASE_ADDRESS + 0x1C)
#define	REG_PAC_DUR_AUTO_REPLY_BW_TABLE                         (PAC_DUR_BASE_ADDRESS + 0x20)
#define	REG_PAC_DUR_AUTO_REPLY_MCS_DEGRADATION_VAL              (PAC_DUR_BASE_ADDRESS + 0x24)
#define	REG_PAC_DUR_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP0TO9      (PAC_DUR_BASE_ADDRESS + 0x28)
#define	REG_PAC_DUR_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP10TO15    (PAC_DUR_BASE_ADDRESS + 0x2C)
#define	REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA0TO31                (PAC_DUR_BASE_ADDRESS + 0x30)
#define	REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA32TO63               (PAC_DUR_BASE_ADDRESS + 0x34)
#define	REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA64TO95               (PAC_DUR_BASE_ADDRESS + 0x38)
#define	REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA96TO127              (PAC_DUR_BASE_ADDRESS + 0x3C)
#define	REG_PAC_DUR_TXOP_JUMP_TABLE_BASE                        (PAC_DUR_BASE_ADDRESS + 0x50)
#define	REG_PAC_DUR_TX_MPDU_DUR_FIELD                           (PAC_DUR_BASE_ADDRESS + 0x54)
#define	REG_PAC_DUR_TX_NAV_TIMER_AT_TX_EN_RISE                  (PAC_DUR_BASE_ADDRESS + 0x58)
#define	REG_PAC_DUR_TXOP_DURATION_COMPENSATE                    (PAC_DUR_BASE_ADDRESS + 0x5C)
#define	REG_PAC_DUR_FRAME_DURATION                              (PAC_DUR_BASE_ADDRESS + 0x60)
#define	REG_PAC_DUR_TXOP_BUDGET_COMPENSATE                      (PAC_DUR_BASE_ADDRESS + 0x64)
#define	REG_PAC_DUR_TXOP_BUDGET_AVAILABLE                       (PAC_DUR_BASE_ADDRESS + 0x68)
#define	REG_PAC_DUR_LATCHED_TXOP_LIMIT_COUNTER                  (PAC_DUR_BASE_ADDRESS + 0x6C)
#define	REG_PAC_DUR_SNOOP_FIFO_ADDR                             (PAC_DUR_BASE_ADDRESS + 0x70)
#define	REG_PAC_DUR_AUTO_REPLY_OVERRIDE_PARAMS_DBG              (PAC_DUR_BASE_ADDRESS + 0x80)
#define	REG_PAC_DUR_TSF_FIELD_TIME1                             (PAC_DUR_BASE_ADDRESS + 0x84)
#define	REG_PAC_DUR_TSF_FIELD_TIME2                             (PAC_DUR_BASE_ADDRESS + 0x88)
#define	REG_PAC_DUR_TSF_FIELD_TIME3                             (PAC_DUR_BASE_ADDRESS + 0x8C)
#define	REG_PAC_DUR_PHY_PAC_TX_AIR_TIME_RESULT                  (PAC_DUR_BASE_ADDRESS + 0x94)
#define	REG_PAC_DUR_DURATION_CALCULATION_RESULT                 (PAC_DUR_BASE_ADDRESS + 0x9C)
#define	REG_PAC_DUR_TSF_ADJUST_DATA_IN                          (PAC_DUR_BASE_ADDRESS + 0xA0)
#define	REG_PAC_DUR_TXOP_HOLDER_ADDR_BITS0TO31                  (PAC_DUR_BASE_ADDRESS + 0xAC)
#define	REG_PAC_DUR_TXOP_HOLDER_ADDR_BITS32TO47                 (PAC_DUR_BASE_ADDRESS + 0xB0)
#define	REG_PAC_DUR_SIFS_PLUS_PHY_RX_READY_AIR_TIME_DUR         (PAC_DUR_BASE_ADDRESS + 0xBC)
#define	REG_PAC_DUR_RX_NAV_TIMER_LOAD_RXD_EVENTS_A              (PAC_DUR_BASE_ADDRESS + 0xC0)
#define	REG_PAC_DUR_RX_NAV_TIMER_LOAD_RXD_EVENTS_B              (PAC_DUR_BASE_ADDRESS + 0xC4)
#define	REG_PAC_DUR_RX_NAV_TIMER_LOAD_RXD_EVENTS_C              (PAC_DUR_BASE_ADDRESS + 0xC8)
#define	REG_PAC_DUR_RX_TO_ME_AIR_TIME_CTR_WR                    (PAC_DUR_BASE_ADDRESS + 0xD0)
#define	REG_PAC_DUR_RX_TO_ME_AIR_TIME_CTR_RD                    (PAC_DUR_BASE_ADDRESS + 0xD4)
#define	REG_PAC_DUR_PHY_DEBUG_OVERRIDE_MAC_PHY_SIGNALS          (PAC_DUR_BASE_ADDRESS + 0xF0)
#define	REG_PAC_DUR_TSF_OFFSET_INDEX                            (PAC_DUR_BASE_ADDRESS + 0xF4)
#define	REG_PAC_DUR_DUR_TIMER_COMPENSATION                      (PAC_DUR_BASE_ADDRESS + 0xF8)
#define	REG_PAC_DUR_TX_FRAME_ADDR3_SELECT_VAP                   (PAC_DUR_BASE_ADDRESS + 0xFC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_DUR_CFP_END_TIME 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cfpEndTime:16;	// DUR  cfp end time
		uint32 reserved0:16;
	} bitFields;
} RegPacDurCfpEndTime_u;

/*REG_PAC_DUR_TSF_START_TIME_ADJUST 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfStartTimeAdjust:12;	// DUR  tsf start time adjust
		uint32 reserved0:20;
	} bitFields;
} RegPacDurTsfStartTimeAdjust_u;

/*REG_PAC_DUR_MISC 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 powerMgmt:1;	// DUR  power mgmt
		uint32 reserved0:31;
	} bitFields;
} RegPacDurMisc_u;

/*REG_PAC_DUR_NAV_ADJUST 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 navAdjust:7;	// DUR  nav adjust
		uint32 reserved0:25;
	} bitFields;
} RegPacDurNavAdjust_u;

/*REG_PAC_DUR_ACK_CONTROL 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ackControlNoAck:4;	// DUR  no ack
		uint32 reserved0:28;
	} bitFields;
} RegPacDurAckControl_u;

/*REG_PAC_DUR_TSF_FIELD_TIME 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime:9;	// DUR  tsf field time
		uint32 reserved0:23;
	} bitFields;
} RegPacDurTsfFieldTime_u;

/*REG_PAC_DUR_AUTO_REPLY_BW_TABLE 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyBwTable:8;	// Auto reply BW table
		uint32 reserved0:24;
	} bitFields;
} RegPacDurAutoReplyBwTable_u;

/*REG_PAC_DUR_AUTO_REPLY_MCS_DEGRADATION_VAL 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyMcsDegradationVal:4;	// Auto reply degradation value for MCS in
		uint32 reserved0:28;
	} bitFields;
} RegPacDurAutoReplyMcsDegradationVal_u;

/*REG_PAC_DUR_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP0TO9 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrDegradationValVap0To9:30;	// Auto reply degradation value for power, 3 bit per VAP
		uint32 reserved0:2;
	} bitFields;
} RegPacDurAutoReplyPwrDegradationValVap0To9_u;

/*REG_PAC_DUR_AUTO_REPLY_PWR_DEGRADATION_VAL_VAP10TO15 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrDegradationValVap10To15:18;	// Auto reply degradation value for power, 3 bit per VAP
		uint32 reserved0:14;
	} bitFields;
} RegPacDurAutoReplyPwrDegradationValVap10To15_u;

/*REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA0TO31 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta0To31:32;	// Auto reply STA is near indication, one bit per STA
	} bitFields;
} RegPacDurAutoReplyPwrNearSta0To31_u;

/*REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA32TO63 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta32To63:32;	// Auto reply STA is near indication, one bit per STA
	} bitFields;
} RegPacDurAutoReplyPwrNearSta32To63_u;

/*REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA64TO95 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta64To95:32;	// Auto reply STA is near indication, one bit per STA
	} bitFields;
} RegPacDurAutoReplyPwrNearSta64To95_u;

/*REG_PAC_DUR_AUTO_REPLY_PWR_NEAR_STA96TO127 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPwrNearSta96To127:32;	// Auto reply STA is near indication, one bit per STA
	} bitFields;
} RegPacDurAutoReplyPwrNearSta96To127_u;

/*REG_PAC_DUR_TXOP_JUMP_TABLE_BASE 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:6;
		uint32 txopJumpTableBase:18;	// TXOP jump table base
		uint32 reserved1:8;
	} bitFields;
} RegPacDurTxopJumpTableBase_u;

/*REG_PAC_DUR_TX_MPDU_DUR_FIELD 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txMpduDurField:16;	// Tx MPDU duration field
		uint32 reserved0:16;
	} bitFields;
} RegPacDurTxMpduDurField_u;

/*REG_PAC_DUR_TX_NAV_TIMER_AT_TX_EN_RISE 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNavTimerAtTxEnRise:18;	// Sample Tx NAV at the rise of MAC-PHY Tx enable
		uint32 reserved0:14;
	} bitFields;
} RegPacDurTxNavTimerAtTxEnRise_u;

/*REG_PAC_DUR_TXOP_DURATION_COMPENSATE 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopDurationCompensate:14;	// TxOP duration compensate
		uint32 reserved0:18;
	} bitFields;
} RegPacDurTxopDurationCompensate_u;

/*REG_PAC_DUR_FRAME_DURATION 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 frameDuration:14;	// Frame duration
		uint32 reserved0:18;
	} bitFields;
} RegPacDurFrameDuration_u;

/*REG_PAC_DUR_TXOP_BUDGET_COMPENSATE 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopBudgetCompensate:14;	// TxOP budget compensation
		uint32 reserved0:18;
	} bitFields;
} RegPacDurTxopBudgetCompensate_u;

/*REG_PAC_DUR_TXOP_BUDGET_AVAILABLE 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopBudgetAvailable:1;	// TxOP budget available
		uint32 reserved0:31;
	} bitFields;
} RegPacDurTxopBudgetAvailable_u;

/*REG_PAC_DUR_LATCHED_TXOP_LIMIT_COUNTER 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latchedTxopLimitCounter:14;	// Latched TxOP limit counter
		uint32 reserved0:18;
	} bitFields;
} RegPacDurLatchedTxopLimitCounter_u;

/*REG_PAC_DUR_SNOOP_FIFO_ADDR 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 snoopFifo:8;	// Snoop FIFO data
		uint32 reserved0:8;
		uint32 snoopCounter:4;	// Snoop FIFO data
		uint32 reserved1:12;
	} bitFields;
} RegPacDurSnoopFifoAddr_u;

/*REG_PAC_DUR_AUTO_REPLY_OVERRIDE_PARAMS_DBG 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyPhyMode:2;	// auto reply phy mode
		uint32 autoReplyBw:2;	// auto reply bw
		uint32 autoReplyMcs:6;	// auto_ eply mcs
		uint32 autoReplyTxPower:6;	// auto reply tx power
		uint32 reserved0:16;
	} bitFields;
} RegPacDurAutoReplyOverrideParamsDbg_u;

/*REG_PAC_DUR_TSF_FIELD_TIME1 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime1:25;	// TSF field time 1 (multiple BSSID)
		uint32 reserved0:7;
	} bitFields;
} RegPacDurTsfFieldTime1_u;

/*REG_PAC_DUR_TSF_FIELD_TIME2 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime2:25;	// TSF field time 2 (multiple BSSID)
		uint32 reserved0:7;
	} bitFields;
} RegPacDurTsfFieldTime2_u;

/*REG_PAC_DUR_TSF_FIELD_TIME3 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfFieldTime3:25;	// TSF field time 3 (multiple BSSID)
		uint32 reserved0:7;
	} bitFields;
} RegPacDurTsfFieldTime3_u;

/*REG_PAC_DUR_PHY_PAC_TX_AIR_TIME_RESULT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyPacTxAirTimeResult:17;	// PHY-MAC Tx air time result
		uint32 reserved0:15;
	} bitFields;
} RegPacDurPhyPacTxAirTimeResult_u;

/*REG_PAC_DUR_DURATION_CALCULATION_RESULT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoReplyDur:16;	// duration calculation result
		uint32 autoReplyBfReportDur:16;	// BF report duration calculation result
	} bitFields;
} RegPacDurDurationCalculationResult_u;

/*REG_PAC_DUR_TSF_ADJUST_DATA_IN 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfAdjustDataIn:24;	// tsf adjust data in
		uint32 reserved0:8;
	} bitFields;
} RegPacDurTsfAdjustDataIn_u;

/*REG_PAC_DUR_TXOP_HOLDER_ADDR_BITS0TO31 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopHolderAddrBits0To31:32;	// TXOP holder addr[31:0]
	} bitFields;
} RegPacDurTxopHolderAddrBits0To31_u;

/*REG_PAC_DUR_TXOP_HOLDER_ADDR_BITS32TO47 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopHolderAddrBits32To47:16;	// TXOP holder addr[47:32]
		uint32 txopHolderAddrValid:1;	// TXOP holder addr valid bit
		uint32 reserved0:15;
	} bitFields;
} RegPacDurTxopHolderAddrBits32To47_u;

/*REG_PAC_DUR_SIFS_PLUS_PHY_RX_READY_AIR_TIME_DUR 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sifsPlusPhyRxDelayAirTime11Ag:8;	// “SIFS + PHY RX Delay” when the incoming Rx frame is PHY mode 11a/g/n/ac [microseconds].
		uint32 sifsPlusPhyRxDelayAirTime11BShort:8;	// “SIFS + PHY RX Delay” when the incoming Rx frame is PHY mode 11b short preamble [microseconds]
		uint32 sifsPlusPhyRxDelayAirTime11BLong:8;	// “SIFS + PHY RX Delay” when the incoming Rx frame is PHY mode 11b long preamble [microseconds]
		uint32 reserved0:8;
	} bitFields;
} RegPacDurSifsPlusPhyRxReadyAirTimeDur_u;

/*REG_PAC_DUR_RX_NAV_TIMER_LOAD_RXD_EVENTS_A 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxNavTimerLoadRxdEventsA:32;	// Rx NAV timer load configuration: , 00: No update. , 10: According to 802.11 NAV special function if value is higher than NAV timer. , 11: Set to zero. ,  , 2bits per RXD line (lines 0to15).
	} bitFields;
} RegPacDurRxNavTimerLoadRxdEventsA_u;

/*REG_PAC_DUR_RX_NAV_TIMER_LOAD_RXD_EVENTS_B 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxNavTimerLoadRxdEventsB:32;	// Rx NAV timer load configuration: , 00: No update. , 10: According to 802.11 NAV special function if value is higher than NAV timer. , 11: Set to zero. ,  , 2bits per RXD line (lines 16to31).
	} bitFields;
} RegPacDurRxNavTimerLoadRxdEventsB_u;

/*REG_PAC_DUR_RX_NAV_TIMER_LOAD_RXD_EVENTS_C 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxNavTimerLoadRxdEventsC:4;	// Rx NAV timer load configuration: , 00: No update. , 10: According to 802.11 NAV special function if value is higher than NAV timer. , 11: Set to zero. ,  , 2bits per RXD line (lines 32to33).
		uint32 reserved0:28;
	} bitFields;
} RegPacDurRxNavTimerLoadRxdEventsC_u;

/*REG_PAC_DUR_RX_TO_ME_AIR_TIME_CTR_WR 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxToMeAirTimeCtrWr:28;	// Rx to me air time counter, write value
		uint32 reserved0:4;
	} bitFields;
} RegPacDurRxToMeAirTimeCtrWr_u;

/*REG_PAC_DUR_RX_TO_ME_AIR_TIME_CTR_RD 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxToMeAirTimeCtrRd:28;	// Rx to me air time counter
		uint32 reserved0:4;
	} bitFields;
} RegPacDurRxToMeAirTimeCtrRd_u;

/*REG_PAC_DUR_PHY_DEBUG_OVERRIDE_MAC_PHY_SIGNALS 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyDebugOverrideLastVapIdxEn:1;	// Enable override MAC PHY last VAP index
		uint32 reserved0:7;
		uint32 phyDebugOverrideLastVapIdx:4;	// The override value of MAC PHY last VAP index
		uint32 reserved1:20;
	} bitFields;
} RegPacDurPhyDebugOverrideMacPhySignals_u;

/*REG_PAC_DUR_TSF_OFFSET_INDEX 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfOffsetIndex:5;	// TSF offset index from HOB
		uint32 reserved0:27;
	} bitFields;
} RegPacDurTsfOffsetIndex_u;

/*REG_PAC_DUR_DUR_TIMER_COMPENSATION 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 durTimerRxrdyCompensation:5;	// Dur timer RxRdy compensation
		uint32 reserved0:3;
		uint32 durTimerTxenCompensation:5;	// Dur timer TxEn compensation
		uint32 reserved1:19;
	} bitFields;
} RegPacDurDurTimerCompensation_u;

/*REG_PAC_DUR_TX_FRAME_ADDR3_SELECT_VAP 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txFrameAddr3SelectVap:16;	// Tx frame address 3 selection, 1bit per VAP: , 0: the same as address2 (i.e. AP mode). , 1: the same as address1 (i.e. STA mode). , 
		uint32 reserved0:16;
	} bitFields;
} RegPacDurTxFrameAddr3SelectVap_u;



#endif // _PAC_DUR_REGS_H_
