
/***********************************************************************************
File:				PacTimRegs.h
Module:				pacTim
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_TIM_REGS_H_
#define _PAC_TIM_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PAC_TIM_BASE_ADDRESS                             MEMORY_MAP_UNIT_13_BASE_ADDRESS
#define	REG_PAC_TIM_ONE_MICRO_PRESCALER                          (PAC_TIM_BASE_ADDRESS + 0x0)
#define	REG_PAC_TIM_EIGHTH_MICRO_PRESCALER                       (PAC_TIM_BASE_ADDRESS + 0x4)
#define	REG_PAC_TIM_TSF_HW_EVENT0                                (PAC_TIM_BASE_ADDRESS + 0x8)
#define	REG_PAC_TIM_TSF_HW_EVENT1                                (PAC_TIM_BASE_ADDRESS + 0xC)
#define	REG_PAC_TIM_TSF_REQ_SWITCH                               (PAC_TIM_BASE_ADDRESS + 0x10)
#define	REG_PAC_TIM_TSF_SW_EVENT2                                (PAC_TIM_BASE_ADDRESS + 0x14)
#define	REG_PAC_TIM_TSF_SW_EVENT3                                (PAC_TIM_BASE_ADDRESS + 0x18)
#define	REG_PAC_TIM_TSF_NAV_LOAD                                 (PAC_TIM_BASE_ADDRESS + 0x1C)
#define	REG_PAC_TIM_TSF_NAV_LOAD_DURATION                        (PAC_TIM_BASE_ADDRESS + 0x24)
#define	REG_PAC_TIM_TICK_TIMER                                   (PAC_TIM_BASE_ADDRESS + 0x2C)
#define	REG_PAC_TIM_SLOT_TIMER                                   (PAC_TIM_BASE_ADDRESS + 0x30)
#define	REG_PAC_TIM_TIMER_CONTROL                                (PAC_TIM_BASE_ADDRESS + 0x34)
#define	REG_PAC_TIM_TIMER_CONTROL_RD                             (PAC_TIM_BASE_ADDRESS + 0x38)
#define	REG_PAC_TIM_NAV_TIMER                                    (PAC_TIM_BASE_ADDRESS + 0x40)
#define	REG_PAC_TIM_TICK                                         (PAC_TIM_BASE_ADDRESS + 0x44)
#define	REG_PAC_TIM_SLOT                                         (PAC_TIM_BASE_ADDRESS + 0x48)
#define	REG_PAC_TIM_IFS_TIMER                                    (PAC_TIM_BASE_ADDRESS + 0x50)
#define	REG_PAC_TIM_RESP_TIMER                                   (PAC_TIM_BASE_ADDRESS + 0x54)
#define	REG_PAC_TIM_TSF_TIMER_LOW_WPORT                          (PAC_TIM_BASE_ADDRESS + 0x58)
#define	REG_PAC_TIM_TSF_TIMER_HIGH_WPORT                         (PAC_TIM_BASE_ADDRESS + 0x5C)
#define	REG_PAC_TIM_TSF_TIMER_LOW_RPORT                          (PAC_TIM_BASE_ADDRESS + 0x60)
#define	REG_PAC_TIM_TSF_TIMER_HIGH_RPORT                         (PAC_TIM_BASE_ADDRESS + 0x64)
#define	REG_PAC_TIM_TSF_HW_EVENT2                                (PAC_TIM_BASE_ADDRESS + 0x68)
#define	REG_PAC_TIM_TTL                                          (PAC_TIM_BASE_ADDRESS + 0x6C)
#define	REG_PAC_TIM_TXOP_LIMIT_COUNTER                           (PAC_TIM_BASE_ADDRESS + 0x70)
#define	REG_PAC_TIM_LSP_MODE_ENABLE                              (PAC_TIM_BASE_ADDRESS + 0x78)
#define	REG_PAC_TIM_LSP_MCS_EVENT_CTR                            (PAC_TIM_BASE_ADDRESS + 0x7C)
#define	REG_PAC_TIM_LSP_FCS_EVENT_CTR                            (PAC_TIM_BASE_ADDRESS + 0x80)
#define	REG_PAC_TIM_LSP_DURATION                                 (PAC_TIM_BASE_ADDRESS + 0x84)
#define	REG_PAC_TIM_QBSS_LOAD_INTEGRATOR_WR                      (PAC_TIM_BASE_ADDRESS + 0x88)
#define	REG_PAC_TIM_QBSS_LOAD_INTEGRATOR_RD                      (PAC_TIM_BASE_ADDRESS + 0x8C)
#define	REG_PAC_TIM_REDUCED_FREQ_CHANGE_LATCHED_TSF_TIMER_LOW    (PAC_TIM_BASE_ADDRESS + 0x90)
#define	REG_PAC_TIM_RX_DUR_TIMER                                 (PAC_TIM_BASE_ADDRESS + 0x98)
#define	REG_PAC_TIM_DELIA_TIMER                                  (PAC_TIM_BASE_ADDRESS + 0x9C)
#define	REG_PAC_TIM_DELIA_MAX_LIMIT_TIMER                        (PAC_TIM_BASE_ADDRESS + 0xA0)
#define	REG_PAC_TIM_TXH_PAUSER_REP_TIMER                         (PAC_TIM_BASE_ADDRESS + 0xA4)
#define	REG_PAC_TIM_RTS_CLR_RX_NAV_TIMER_ENABLE                  (PAC_TIM_BASE_ADDRESS + 0xA8)
#define	REG_PAC_TIM_RTS_CLR_RX_NAV_TIMER                         (PAC_TIM_BASE_ADDRESS + 0xAC)
#define	REG_PAC_TIM_TX_NAV_TIMER_CFG_SET_VALUE                   (PAC_TIM_BASE_ADDRESS + 0xB0)
#define	REG_PAC_TIM_TX_NAV_TIMER                                 (PAC_TIM_BASE_ADDRESS + 0xB4)
#define	REG_PAC_TIM_TX_NAV_TIMER_IFS_COMPENSATION                (PAC_TIM_BASE_ADDRESS + 0xB8)
#define	REG_PAC_TIM_TIMER_CONTROL2                               (PAC_TIM_BASE_ADDRESS + 0xBC)
#define	REG_PAC_TIM_QBSS2_LOAD_INTEGRATOR_WR                     (PAC_TIM_BASE_ADDRESS + 0xC0)
#define	REG_PAC_TIM_QBSS2_LOAD_INTEGRATOR_RD                     (PAC_TIM_BASE_ADDRESS + 0xC4)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_TIM_ONE_MICRO_PRESCALER 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 oneMicroPrescaler:10;	// TIM one micro prescaler
		uint32 reserved0:6;
		uint32 oneMicroPrescalerFreqReduced:10;	// TIM one micro prescaler reduced clk
		uint32 reserved1:6;
	} bitFields;
} RegPacTimOneMicroPrescaler_u;

/*REG_PAC_TIM_EIGHTH_MICRO_PRESCALER 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eighthMicroPrescaler:10;	// TIM eighth micro prescaler
		uint32 reserved0:6;
		uint32 eighthMicroPrescalerFreqReduced:10;	// TIM eighth micro prescaler reduced clk
		uint32 reserved1:6;
	} bitFields;
} RegPacTimEighthMicroPrescaler_u;

/*REG_PAC_TIM_TSF_HW_EVENT0 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfHwEvent0Time:24;	// TIM  time
		uint32 reserved0:4;
		uint32 tsfHwEvent0HwEnableRd:1;	// TIM  hw enable RD
		uint32 tsfHwEvent0SwEnableRd:1;	// TIM  sw enable RD
		uint32 tsfHwEvent0HwEnableWr:1;	// TIM  hw enable WR
		uint32 tsfHwEvent0SwEnableWr:1;	// TIM  sw enable WR
	} bitFields;
} RegPacTimTsfHwEvent0_u;

/*REG_PAC_TIM_TSF_HW_EVENT1 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfHwEvent1Time:24;	// TIM  time
		uint32 reserved0:4;
		uint32 tsfHwEvent1HwEnableRd:1;	// TIM  hw enable RD
		uint32 tsfHwEvent1SwEnableRd:1;	// TIM  sw enable RD
		uint32 tsfHwEvent1HwEnableWr:1;	// TIM  hw enable WR
		uint32 tsfHwEvent1SwEnableWr:1;	// TIM  sw enable WR
	} bitFields;
} RegPacTimTsfHwEvent1_u;

/*REG_PAC_TIM_TSF_REQ_SWITCH 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfReqSwitchTime:24;	// TIM  time
		uint32 reserved0:2;
		uint32 tsfReqSwitchHwEnableRd:1;	// TIM  hw enable RD
		uint32 tsfReqSwitchSwEnableRd:1;	// TIM  sw enable RD
		uint32 reserved1:1;
		uint32 tsfReqSwitchTo1:1;	// TIM  to 1
		uint32 tsfReqSwitchHwEnableWr:1;	// TIM  hw enable WR
		uint32 tsfReqSwitchSwEnableWr:1;	// TIM  sw enable WR
	} bitFields;
} RegPacTimTsfReqSwitch_u;

/*REG_PAC_TIM_TSF_SW_EVENT2 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfSwEvent2Time:24;	// TIM  time event 2
		uint32 reserved0:6;
		uint32 tsfSwEvent2EnableRd:1;	// TIM  enable event 2 RD
		uint32 tsfSwEvent2EnableWr:1;	// TIM  enable event 2 WR
	} bitFields;
} RegPacTimTsfSwEvent2_u;

/*REG_PAC_TIM_TSF_SW_EVENT3 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfSwEvent3Time:24;	// TIM  time event 3
		uint32 reserved0:6;
		uint32 tsfSwEvent3EnableRd:1;	// TIM  enable event 3 RD
		uint32 tsfSwEvent3EnableWr:1;	// TIM  enable event 3 WR
	} bitFields;
} RegPacTimTsfSwEvent3_u;

/*REG_PAC_TIM_TSF_NAV_LOAD 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfNavLoadTime:24;	// TIM  time
		uint32 reserved0:5;
		uint32 tsfNavLoadHwEnableRd:1;	// TSF NAV load RD
		uint32 tsfNavLoadHwEnableWr:1;	// TSF NAV load WR
		uint32 reserved1:1;
	} bitFields;
} RegPacTimTsfNavLoad_u;

/*REG_PAC_TIM_TSF_NAV_LOAD_DURATION 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfNavLoadDuration:16;	// TIM  tsf nav load duration
		uint32 reserved0:16;
	} bitFields;
} RegPacTimTsfNavLoadDuration_u;

/*REG_PAC_TIM_TICK_TIMER 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tickTimerPeriod:16;	// TIM  period
		uint32 reserved0:15;
		uint32 tickTimerEnable:1;	// TIM  enable
	} bitFields;
} RegPacTimTickTimer_u;

/*REG_PAC_TIM_SLOT_TIMER 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 slotTimerPeriod:8;	// Slot period
		uint32 reserved1:8;
	} bitFields;
} RegPacTimSlotTimer_u;

/*REG_PAC_TIM_TIMER_CONTROL 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 navEnable:1;	// TIM  nav enable
		uint32 tsfEnable:1;	// TIM  tsf enable
		uint32 tsfNudgeWr:1;	// TIM  tsf nudge WR
		uint32 reserved0:1;
		uint32 tsfNudgeRd:1;	// TIM  tsf nudge RD
		uint32 integrateTxTimeToQbss:1;	// Integrate Tx time to QBSS load integrator
		uint32 integrateNavTimeToQbss:1;	// Integrate NAV time to QBSS load integrator
		uint32 integrateCcaTimeToQbss:1;	// Integrate CCA time to QBSS load integrator
		uint32 tsfAdj:9;	// TIM  tsf adj
		uint32 reserved1:1;
		uint32 txopLoadAdj:6;	// TIM  tsf adj
		uint32 tsfSwEvent0Wr:1;	// TIM  tsf sw event0 WR
		uint32 tsfSwEvent1Wr:1;	// TIM  tsf sw event1 WR
		uint32 tsfSwEvent2Wr:1;	// TIM  tsf sw event2 WR
		uint32 tsfSwEvent3Wr:1;	// TIM  tsf sw event3 WR
		uint32 tsfSwEvent4Wr:1;	// TIM  tsf sw event4 WR
		uint32 tsfSwEvent5Wr:1;	// TIM  tsf sw event5 WR
		uint32 reserved2:2;
	} bitFields;
} RegPacTimTimerControl_u;

/*REG_PAC_TIM_TIMER_CONTROL_RD 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:24;
		uint32 tsfSwEvent0Rd:1;	// TIM  tsf sw event0 RD
		uint32 tsfSwEvent1Rd:1;	// TIM  tsf sw event1 RD
		uint32 tsfSwEvent2Rd:1;	// TIM  tsf sw event2 RD
		uint32 tsfSwEvent3Rd:1;	// TIM  tsf sw event3 RD
		uint32 tsfSwEvent4Rd:1;	// TIM  tsf sw event4 RD
		uint32 tsfSwEvent5Rd:1;	// TIM  tsf sw event5 RD
		uint32 reserved1:2;
	} bitFields;
} RegPacTimTimerControlRd_u;

/*REG_PAC_TIM_NAV_TIMER 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 navTimer:16;	// TIM  nav timer
		uint32 reserved0:16;
	} bitFields;
} RegPacTimNavTimer_u;

/*REG_PAC_TIM_TICK 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timerTick:16;	// TIM  timer
		uint32 reserved0:16;
	} bitFields;
} RegPacTimTick_u;

/*REG_PAC_TIM_SLOT 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 slotTimer:8;	// Slot timer
		uint32 reserved1:8;
	} bitFields;
} RegPacTimSlot_u;

/*REG_PAC_TIM_IFS_TIMER 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ifsTimer:13;	// TIM  ifs timer
		uint32 reserved0:19;
	} bitFields;
} RegPacTimIfsTimer_u;

/*REG_PAC_TIM_RESP_TIMER 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 respTimer:13;	// TIM  resp timer
		uint32 reserved0:19;
	} bitFields;
} RegPacTimRespTimer_u;

/*REG_PAC_TIM_TSF_TIMER_LOW_WPORT 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfTimerLowWport:32;	// TIM  tsf timer low WR
	} bitFields;
} RegPacTimTsfTimerLowWport_u;

/*REG_PAC_TIM_TSF_TIMER_HIGH_WPORT 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfTimerHighWport:32;	// TIM  tsf timer high WR
	} bitFields;
} RegPacTimTsfTimerHighWport_u;

/*REG_PAC_TIM_TSF_TIMER_LOW_RPORT 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfTimerLowRport:32;	// TIM  tsf timer low RD
	} bitFields;
} RegPacTimTsfTimerLowRport_u;

/*REG_PAC_TIM_TSF_TIMER_HIGH_RPORT 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfTimerHighRport:32;	// TIM  tsf timer high RD
	} bitFields;
} RegPacTimTsfTimerHighRport_u;

/*REG_PAC_TIM_TSF_HW_EVENT2 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfHwEvent2:24;	// TSF HW event 2
		uint32 reserved0:4;
		uint32 tsfHwEvent2HwEnableRd:1;	// TSF HW event 2 enable RD
		uint32 tsfHwEvent2SwEnableRd:1;	// TSF SW event 2 enable RD
		uint32 tsfHwEvent2HwEnableWr:1;	// TSF HW event 2 enable WR
		uint32 tsfHwEvent2SwEnableWr:1;	// TSF SW event 2 enable WR
	} bitFields;
} RegPacTimTsfHwEvent2_u;

/*REG_PAC_TIM_TTL 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ttl:7;	// TTL value - Quantized to 256K uS
		uint32 reserved0:25;
	} bitFields;
} RegPacTimTtl_u;

/*REG_PAC_TIM_TXOP_LIMIT_COUNTER 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txopLimitCounter:14;	// TXOP limit counter
		uint32 reserved0:18;
	} bitFields;
} RegPacTimTxopLimitCounter_u;

/*REG_PAC_TIM_LSP_MODE_ENABLE 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lspEnableModeMcs:1;	// LSIG protection enable upon unsupported MCS
		uint32 lspEnableModeFcs:1;	// LSIG protection enable upon subframe FCS error
		uint32 reserved0:30;
	} bitFields;
} RegPacTimLspModeEnable_u;

/*REG_PAC_TIM_LSP_MCS_EVENT_CTR 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lspMcsEventCtr:16;	// NAV loads count due to unsupported MCS
		uint32 reserved0:16;
	} bitFields;
} RegPacTimLspMcsEventCtr_u;

/*REG_PAC_TIM_LSP_FCS_EVENT_CTR 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lspFcsEventCtr:16;	// NAV loads count due to FCS errors
		uint32 reserved0:16;
	} bitFields;
} RegPacTimLspFcsEventCtr_u;

/*REG_PAC_TIM_LSP_DURATION 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lspDuration:16;	// Latched LSIG protection duration
		uint32 reserved0:16;
	} bitFields;
} RegPacTimLspDuration_u;

/*REG_PAC_TIM_QBSS_LOAD_INTEGRATOR_WR 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qbssLoadIntegratorWr:28;	// QBSS load integrator WR
		uint32 reserved0:4;
	} bitFields;
} RegPacTimQbssLoadIntegratorWr_u;

/*REG_PAC_TIM_QBSS_LOAD_INTEGRATOR_RD 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qbssLoadIntegratorRd:28;	// QBSS load integrator RD
		uint32 reserved0:4;
	} bitFields;
} RegPacTimQbssLoadIntegratorRd_u;

/*REG_PAC_TIM_REDUCED_FREQ_CHANGE_LATCHED_TSF_TIMER_LOW 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reducedFreqChangeLatchedTsfTimerLow:32;	// reduced_freq_change_latched_tsf_timer_low
	} bitFields;
} RegPacTimReducedFreqChangeLatchedTsfTimerLow_u;

/*REG_PAC_TIM_RX_DUR_TIMER 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDurTimer:8;	// rx duration timer
		uint32 reserved0:24;
	} bitFields;
} RegPacTimRxDurTimer_u;

/*REG_PAC_TIM_DELIA_TIMER 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaTimer:24;	// delia timer
		uint32 reserved0:8;
	} bitFields;
} RegPacTimDeliaTimer_u;

/*REG_PAC_TIM_DELIA_MAX_LIMIT_TIMER 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaMaxLimitTimer:24;	// delia max limit timer
		uint32 reserved0:8;
	} bitFields;
} RegPacTimDeliaMaxLimitTimer_u;

/*REG_PAC_TIM_TXH_PAUSER_REP_TIMER 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txhPauserRepTimer:16;	// TxH Pauser repetition timer
		uint32 reserved0:16;
	} bitFields;
} RegPacTimTxhPauserRepTimer_u;

/*REG_PAC_TIM_RTS_CLR_RX_NAV_TIMER_ENABLE 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtsClrRxNavTimerEnable:1;	// Enable reset Rx NAV timer in case RTS duration timer expired
		uint32 reserved0:31;
	} bitFields;
} RegPacTimRtsClrRxNavTimerEnable_u;

/*REG_PAC_TIM_RTS_CLR_RX_NAV_TIMER 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtsClrRxNavTimer:11;	// RTS duration timer in order to reset Rx NAV timer
		uint32 reserved0:5;
		uint32 rtsClrRxNavTimerSm:2;	// RTS duration clear Rx NAV timer state machine
		uint32 reserved1:14;
	} bitFields;
} RegPacTimRtsClrRxNavTimer_u;

/*REG_PAC_TIM_TX_NAV_TIMER_CFG_SET_VALUE 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNavTimerCfgSetValue:18;	// Setting Tx NAV timer value
		uint32 reserved0:14;
	} bitFields;
} RegPacTimTxNavTimerCfgSetValue_u;

/*REG_PAC_TIM_TX_NAV_TIMER 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNavTimer:18;	// Tx NAV timer
		uint32 reserved0:14;
	} bitFields;
} RegPacTimTxNavTimer_u;

/*REG_PAC_TIM_TX_NAV_TIMER_IFS_COMPENSATION 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txNavIfsCompensation:16;	// Tx NAV timer IFS compensation [resolution in cycles]
		uint32 reducedFreqTxNavIfsCompensation:16;	// Tx NAV timer IFS compensation in mode of reduced frequency [resolution in cycles]
	} bitFields;
} RegPacTimTxNavTimerIfsCompensation_u;

/*REG_PAC_TIM_TIMER_CONTROL2 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 integrateTxTimeToQbss2:1;	// Integrate Tx time to QBSS load integrator
		uint32 integrateNavTimeToQbss2:1;	// Integrate NAV time to QBSS load integrator
		uint32 integrateCcaTimeToQbss2:1;	// Integrate CCA time to QBSS load integrator
		uint32 reserved0:29;
	} bitFields;
} RegPacTimTimerControl2_u;

/*REG_PAC_TIM_QBSS2_LOAD_INTEGRATOR_WR 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qbss2LoadIntegratorWr:28;	// QBSS load integrator WR
		uint32 reserved0:4;
	} bitFields;
} RegPacTimQbss2LoadIntegratorWr_u;

/*REG_PAC_TIM_QBSS2_LOAD_INTEGRATOR_RD 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qbss2LoadIntegratorRd:28;	// QBSS load integrator RD
		uint32 reserved0:4;
	} bitFields;
} RegPacTimQbss2LoadIntegratorRd_u;



#endif // _PAC_TIM_REGS_H_
