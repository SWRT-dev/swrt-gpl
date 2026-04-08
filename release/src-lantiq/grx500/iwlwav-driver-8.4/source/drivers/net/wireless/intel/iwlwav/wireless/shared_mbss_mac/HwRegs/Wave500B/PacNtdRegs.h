/***********************************************************************************
File:				PacNtdRegs.h
Module:				PacNtd
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_NTD_REGS_H_
#define _PAC_NTD_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h" 

#define PAC_NTD_BASE_ADDRESS                             MEMORY_MAP_UNIT_11_BASE_ADDRESS
#define	REG_PAC_NTD_EVENT_RESP_ENABLE                       (PAC_NTD_BASE_ADDRESS + 0x0)
#define	REG_PAC_NTD_RX_EVENT_RESP_ENABLE                    (PAC_NTD_BASE_ADDRESS + 0x4)
#define	REG_PAC_NTD_RX_EVENT_RESP_MULTI_SHOT                (PAC_NTD_BASE_ADDRESS + 0x8)
#define	REG_PAC_NTD_RX_EVENT_RESP_PRIORITY                  (PAC_NTD_BASE_ADDRESS + 0xC)
#define	REG_PAC_NTD_RX_EVENT_RESP_REQUEUE_OVERRIDE          (PAC_NTD_BASE_ADDRESS + 0x10)
#define	REG_PAC_NTD_RX_EVENT_NAV_LOADA                      (PAC_NTD_BASE_ADDRESS + 0x14)
#define	REG_PAC_NTD_RX_EVENT_NAV_LOADB                      (PAC_NTD_BASE_ADDRESS + 0x18)
#define	REG_PAC_NTD_RX_SW_EVENT_ENABLE                      (PAC_NTD_BASE_ADDRESS + 0x1C)
#define	REG_PAC_NTD_STATUS                                  (PAC_NTD_BASE_ADDRESS + 0x20)
#define	REG_PAC_NTD_STATUS_PEEK                             (PAC_NTD_BASE_ADDRESS + 0x24)
#define	REG_PAC_NTD_CONTROL                                 (PAC_NTD_BASE_ADDRESS + 0x28)
#define	REG_PAC_NTD_RX_EVENT_RESP_AUTO_DISABLE              (PAC_NTD_BASE_ADDRESS + 0x2C)
#define	REG_PAC_NTD_NTD_EVENT_STATUS                        (PAC_NTD_BASE_ADDRESS + 0x30)
#define	REG_PAC_NTD_STATUS_FIFO_POINTERS                    (PAC_NTD_BASE_ADDRESS + 0x34)
#define	REG_PAC_NTD_STATUS_PEEK_RXC_RX_BUFFER_IN_POINTER    (PAC_NTD_BASE_ADDRESS + 0x38)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_NTD_EVENT_RESP_ENABLE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tsfHwEvent0Enable:1;	// NTD  tsf hw event0
		uint32 tsfHwEvent1Enable:1;	// NTD  tsf hw event1
		uint32 reserved0:14;
		uint32 swTxReq0Set:1;	// NTD  sw tx req0 set
		uint32 swTxReq1Set:1;	// NTD  sw tx req1 set
		uint32 reserved1:2;
		uint32 swTxReq0Reset:1;	// NTD  sw tx req0 reset
		uint32 swTxReq1Reset:1;	// NTD  sw tx req1 reset
		uint32 reserved2:2;
		uint32 noTxReq0Set:1;	// NTD  no tx req0 set
		uint32 noTxReq1Set:1;	// NTD  no tx req1 set
		uint32 reserved3:2;
		uint32 noTxReq0Reset:1;	// NTD  no tx req0 reset
		uint32 noTxReq1Reset:1;	// NTD  no tx req1 reset
		uint32 reserved4:2;
	} bitFields;
} RegPacNtdEventRespEnable_u;

/*REG_PAC_NTD_RX_EVENT_RESP_ENABLE 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEventRespEnable:30;	// NTD  rx event resp enable
		uint32 respTimeouta:1;	// NTD  resp timeouta
		uint32 respTimeoutb:1;	// NTD  resp timeoutb
	} bitFields;
} RegPacNtdRxEventRespEnable_u;

/*REG_PAC_NTD_RX_EVENT_RESP_MULTI_SHOT 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEventRespMultiShot:30;	// NTD  rx event resp multi shot
		uint32 reserved0:1;
		uint32 respTimeoutMultiShot:1;	// NTD  resp timeout
	} bitFields;
} RegPacNtdRxEventRespMultiShot_u;

/*REG_PAC_NTD_RX_EVENT_RESP_PRIORITY 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEventRespPriority:30;	// NTD  rx event resp priority
		uint32 reserved0:1;
		uint32	respTimeoutPriority:1;                            /*  */ 
	} bitFields;
} RegPacNtdRxEventRespPriority_u;

/*REG_PAC_NTD_RX_EVENT_RESP_REQUEUE_OVERRIDE 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEventRespRequeueOverride:30;	// NTD  rx event resp requeue override
		uint32 reserved0:2;
	} bitFields;
} RegPacNtdRxEventRespRequeueOverride_u;

/*REG_PAC_NTD_RX_EVENT_NAV_LOADA 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEventNavLoada:32;	// NTD  rx event nav loada
	} bitFields;
} RegPacNtdRxEventNavLoada_u;

/*REG_PAC_NTD_RX_EVENT_NAV_LOADB 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEventNavLoadb:28;	// NTD  rx event nav loadb
		uint32 reserved0:4;
	} bitFields;
} RegPacNtdRxEventNavLoadb_u;

/*REG_PAC_NTD_RX_SW_EVENT_ENABLE 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxSwEventEnable:30;	// NTD  rx sw event enable
		uint32 unrecognised:1;	// NTD  unrecognised
		uint32 respTimeoutEnable:1;	// NTD  resp timeout
	} bitFields;
} RegPacNtdRxSwEventEnable_u;

/*REG_PAC_NTD_STATUS 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 respRxEventStatus:6;	// NTD  resp rx event status peek
		uint32 reserved0:1;
		uint32 respRxStored:1;	// NTD  resp rx stored peek
		uint32 ptcsReq:7;	// NTD  ptcs req
		uint32 reserved1:1;
		uint32 ptcsEventStatus:2;	// NTD  ptcs event status
		uint32 chain:2;	// NTD  chain
		uint32 ptcsAbortStatus:3;	// NTD  ptcs abort status
		uint32 reserved2:1;
		uint32 respRxEvent:1;	// NTD  resp rx event
		uint32 ptcsEvent:1;	// NTD  ptcs event
		uint32 tsfHwEvent0:1;	// NTD  tsf hw event0
		uint32 tsfHwEvent1:1;	// NTD  tsf hw event1
		uint32 tsfReqSwitch:1;	// NTD  tsf req switch
		uint32 tsfReqSwitchStatus:1;	// NTD  tsf req switch status
		uint32 overflow:1;	// NTD  overflow
		uint32 empty:1;	// NTD  empty
	} bitFields;
} RegPacNtdStatus_u;

/*REG_PAC_NTD_STATUS_PEEK 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 respRxEventStatusPeek:6;	// NTD  resp rx event status
		uint32 reserved0:1;
		uint32 respRxStoredPeek:1;	// NTD  resp rx stored
		uint32 ptcsReqPeek:7;	// NTD  ptcs req
		uint32 reserved1:1;
		uint32 ptcsEventStatusPeek:2;	// NTD  ptcs event status
		uint32 chainPeek:2;	// NTD  chain
		uint32 ptcsAbortStatusPeek:3;	// NTD  ptcs abort status
		uint32 reserved2:1;
		uint32 respRxEventPeek:1;	// NTD  resp rx event
		uint32 ptcsEventPeek:1;	// NTD  ptcs event
		uint32 tsfHwEvent0Peek:1;	// NTD  tsf hw event0
		uint32 tsfHwEvent1Peek:1;	// NTD  tsf hw event1
		uint32 tsfReqSwitchPeek:1;	// NTD  tsf req switch
		uint32 tsfReqSwitchStatusPeek:1;	// NTD  tsf req switch status
		uint32 overflowPeek:1;	// NTD  overflow
		uint32 emptyPeek:1;	// NTD  empty
	} bitFields;
} RegPacNtdStatusPeek_u;

/*REG_PAC_NTD_CONTROL 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoNoTx0Ena:1;	// NTD  auto no tx0 ena
		uint32 autoNoTx1Ena:1;	// NTD  auto no tx1 ena
		uint32 enableExtraRows:1;	// NTD  enable extra rows
		uint32 enableTxcPtcsStatus:1;	// NTD  enable txc ptcs status
		uint32 reserved0:4;
		uint32 ntdState:5;	// NTD  ntd state
		uint32 reserved1:3;
		uint32 rxLowReq:5;	// NTD  rx low req
		uint32 respTimeoutReq:1;	// NTD  resp timeout req
		uint32 tsfHw1Req:1;	// NTD  tsf hw1 req
		uint32 tsfHw0Req:1;	// NTD  tsf hw0 req
		uint32 rxHighReq:5;	// NTD  rx high req
		uint32 reserved2:3;
	} bitFields;
} RegPacNtdControl_u;

/*REG_PAC_NTD_RX_EVENT_RESP_AUTO_DISABLE 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxEventRespAutoDisable:30;	// NTD rx event response auto disable
		uint32 rxEventRespAutoDisableTimeouta:1;	// NTD rx event response auto disable timeout A
		uint32 rxEventRespAutoDisableTimeoutb:1;	// NTD rx event response auto disable timeout B
	} bitFields;
} RegPacNtdRxEventRespAutoDisable_u;

/*REG_PAC_NTD_NTD_EVENT_STATUS 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 eventStatus:1;	// NTD event status
		uint32 reserved0:31;
	} bitFields;
} RegPacNtdNtdEventStatus_u;

/*REG_PAC_NTD_STATUS_FIFO_POINTERS 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusFifoWriteAddr:4;	// NTD status FIFO write pointer
		uint32 statusFifoReadAddr:4;	// NTD status FIFO read pointer
		uint32 reserved0:24;
	} bitFields;
} RegPacNtdStatusFifoPointers_u;

/*REG_PAC_NTD_STATUS_PEEK_RXC_RX_BUFFER_IN_POINTER 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusPeekRxcRxBufferInPointer:20;	// NTD status FIFO RxC buffer in pointer peek
		uint32 reserved0:12;
	} bitFields;
} RegPacNtdStatusPeekRxcRxBufferInPointer_u;



#endif // _PAC_NTD_REGS_H_
