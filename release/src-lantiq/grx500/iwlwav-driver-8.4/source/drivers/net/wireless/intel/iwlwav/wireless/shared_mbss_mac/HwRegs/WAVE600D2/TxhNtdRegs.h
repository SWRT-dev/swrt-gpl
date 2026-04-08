
/***********************************************************************************
File:				TxhNtdRegs.h
Module:				TxhNtd
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TXH_NTD_REGS_H_
#define _TXH_NTD_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TXH_NTD_BASE_ADDRESS                             MEMORY_MAP_UNIT_54_BASE_ADDRESS
#define	REG_TXH_NTD_SW_REQ_TCS_PART0     (TXH_NTD_BASE_ADDRESS + 0x60)
#define	REG_TXH_NTD_SW_REQ_TCS_PART1     (TXH_NTD_BASE_ADDRESS + 0x64)
#define	REG_TXH_NTD_SW_REQ_SET_CLEAR     (TXH_NTD_BASE_ADDRESS + 0x68)
#define	REG_TXH_NTD_SW_REQ_STATUS        (TXH_NTD_BASE_ADDRESS + 0x6C)
#define	REG_TXH_NTD_SW_REQ_IRQ_ENABLE    (TXH_NTD_BASE_ADDRESS + 0x70)
#define	REG_TXH_NTD_SW_REQ_IRQ_CLEAR     (TXH_NTD_BASE_ADDRESS + 0x74)
#define	REG_TXH_NTD_SW_REQ_IRQ_STATUS    (TXH_NTD_BASE_ADDRESS + 0x78)
#define	REG_TXH_NTD_NTD_ARBITER_DEBUG    (TXH_NTD_BASE_ADDRESS + 0x80)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TXH_NTD_SW_REQ_TCS_PART0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqRecipePtr : 25; //SW request recipe pointer, reset value: 0x0, access type: RW
		uint32 swReqCcaAbort : 1; //SW request CCA primary abort, reset value: 0x0, access type: RW
		uint32 swReqRxNavAbort : 1; //SW request Rx NAV abort, reset value: 0x0, access type: RW
		uint32 swReqTxNavAbort : 1; //SW request Tx NAV abort, reset value: 0x0, access type: RW
		uint32 swReqCcaSensitive : 1; //SW request CCA primary sensitive, reset value: 0x0, access type: RW
		uint32 swReqRxNavSensitive : 1; //SW request Rx NAV sensitive, reset value: 0x0, access type: RW
		uint32 swReqTxNavSensitive : 1; //SW request Tx NAV sensitive, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
	} bitFields;
} RegTxhNtdSwReqTcsPart0_u;

/*REG_TXH_NTD_SW_REQ_TCS_PART1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqIfsn : 4; //SW request IFSN, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
		uint32 swReqBackoff : 15; //SW request Backoff, reset value: 0x0, access type: RW
		uint32 reserved1 : 7;
		uint32 swReqIrqClient : 2; //SW request IRQ client, reset value: 0x0, access type: RW
	} bitFields;
} RegTxhNtdSwReqTcsPart1_u;

/*REG_TXH_NTD_SW_REQ_SET_CLEAR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqSet : 1; //SW request set, reset value: 0x0, access type: WO
		uint32 swReqClear : 1; //SW request Clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegTxhNtdSwReqSetClear_u;

/*REG_TXH_NTD_SW_REQ_STATUS 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqTxReq : 1; //SW request Tx request, reset value: 0x0, access type: RO
		uint32 swReqPendingClrTxReq : 1; //SW request pending clear Tx request, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 swReqCurrIfsn : 4; //SW request current IFSN, reset value: 0x0, access type: RO
		uint32 swReqCurrBackoff : 15; //SW request current Backoff, reset value: 0x0, access type: RO
		uint32 reserved1 : 9;
	} bitFields;
} RegTxhNtdSwReqStatus_u;

/*REG_TXH_NTD_SW_REQ_IRQ_ENABLE 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqIrqEnable : 5; //SW request IRQ enable, reset value: 0x1f, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegTxhNtdSwReqIrqEnable_u;

/*REG_TXH_NTD_SW_REQ_IRQ_CLEAR 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqIrqClear : 5; //SW request IRQ clear, reset value: 0x0, access type: WO
		uint32 reserved0 : 27;
	} bitFields;
} RegTxhNtdSwReqIrqClear_u;

/*REG_TXH_NTD_SW_REQ_IRQ_STATUS 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqTxDoneIrq : 1; //SW request Transmission done, reset value: 0x0, access type: RO
		uint32 swReqClearDoneIrq : 1; //SW request Clear done, reset value: 0x0, access type: RO
		uint32 swReqCcaAbortDoneIrq : 1; //SW request CCA primary abort done, reset value: 0x0, access type: RO
		uint32 swReqRxNavAbortDoneIrq : 1; //SW request Rx NAV abort done, reset value: 0x0, access type: RO
		uint32 swReqTxNavAbortDoneIrq : 1; //SW request Tx NAV abort done, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegTxhNtdSwReqIrqStatus_u;

/*REG_TXH_NTD_NTD_ARBITER_DEBUG 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ntdArbLastClient : 2; //Tx last client, reset value: 0x0, access type: RO
		uint32 reserved0 : 2;
		uint32 currAutoRespTxReq : 1; //Current Auto Response Tx request, reset value: 0x0, access type: RO
		uint32 currSequencerTxReq : 1; //Current Sequencer Tx request, reset value: 0x0, access type: RO
		uint32 currSwReqTxReq : 1; //Current SW req Tx request, reset value: 0x0, access type: RO
		uint32 currTxhMapTxReq : 1; //Current TXH MAP Tx request, reset value: 0x0, access type: RO
		uint32 reserved1 : 24;
	} bitFields;
} RegTxhNtdNtdArbiterDebug_u;



#endif // _TXH_NTD_REGS_H_
