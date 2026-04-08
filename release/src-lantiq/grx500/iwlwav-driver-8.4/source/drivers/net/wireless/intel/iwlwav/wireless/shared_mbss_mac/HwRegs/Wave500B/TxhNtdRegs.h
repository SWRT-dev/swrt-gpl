
/***********************************************************************************
File:				TxhNtdRegs.h
Module:				txhNtd
SOC Revision:		843
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
#define	REG_TXH_NTD_AUTO_RESP_TCS0             (TXH_NTD_BASE_ADDRESS + 0x10)
#define	REG_TXH_NTD_AUTO_RESP_TCS1             (TXH_NTD_BASE_ADDRESS + 0x14)
#define	REG_TXH_NTD_AUTO_RESP_TCS2             (TXH_NTD_BASE_ADDRESS + 0x18)
#define	REG_TXH_NTD_AUTO_RESP_TCS3             (TXH_NTD_BASE_ADDRESS + 0x1C)
#define	REG_TXH_NTD_AUTO_RESP_TCS4             (TXH_NTD_BASE_ADDRESS + 0x20)
#define	REG_TXH_NTD_AUTO_RESP_TCS5             (TXH_NTD_BASE_ADDRESS + 0x24)
#define	REG_TXH_NTD_AUTO_RESP_TCS6             (TXH_NTD_BASE_ADDRESS + 0x28)
#define	REG_TXH_NTD_AUTO_RESP_TCS7             (TXH_NTD_BASE_ADDRESS + 0x2C)
#define	REG_TXH_NTD_AUTO_RESP_RXD_LINES_A      (TXH_NTD_BASE_ADDRESS + 0x30)
#define	REG_TXH_NTD_AUTO_RESP_RXD_LINES_B      (TXH_NTD_BASE_ADDRESS + 0x34)
#define	REG_TXH_NTD_AUTO_RESP_RXD_LINES_C      (TXH_NTD_BASE_ADDRESS + 0x38)
#define	REG_TXH_NTD_AUTO_RESP_RXD_LINES_D      (TXH_NTD_BASE_ADDRESS + 0x3C)
#define	REG_TXH_NTD_AUTO_RESP_RXD_LINES_E      (TXH_NTD_BASE_ADDRESS + 0x40)
#define	REG_TXH_NTD_AUTO_RESP_CTR              (TXH_NTD_BASE_ADDRESS + 0x50)
#define	REG_TXH_NTD_SW_REQ_TCS_PART0           (TXH_NTD_BASE_ADDRESS + 0x60)
#define	REG_TXH_NTD_SW_REQ_TCS_PART1           (TXH_NTD_BASE_ADDRESS + 0x64)
#define	REG_TXH_NTD_SW_REQ_SET_CLEAR           (TXH_NTD_BASE_ADDRESS + 0x68)
#define	REG_TXH_NTD_SW_REQ_STATUS              (TXH_NTD_BASE_ADDRESS + 0x6C)
#define	REG_TXH_NTD_SW_REQ_IRQ_ENABLE          (TXH_NTD_BASE_ADDRESS + 0x70)
#define	REG_TXH_NTD_SW_REQ_IRQ_CLEAR           (TXH_NTD_BASE_ADDRESS + 0x74)
#define	REG_TXH_NTD_SW_REQ_IRQ_STATUS          (TXH_NTD_BASE_ADDRESS + 0x78)
#define	REG_TXH_NTD_NTD_ARBITER_DEBUG          (TXH_NTD_BASE_ADDRESS + 0x80)
#define	REG_TXH_NTD_FILTER_EXP_RX_CTS          (TXH_NTD_BASE_ADDRESS + 0x90)
#define	REG_TXH_NTD_FILTER_EXP_RX_ACK_BA       (TXH_NTD_BASE_ADDRESS + 0x94)
#define	REG_TXH_NTD_FILTER_EXP_RX_SU_BF_RPT    (TXH_NTD_BASE_ADDRESS + 0x98)
#define	REG_TXH_NTD_FILTER_EXP_RX_MU_BF_RPT    (TXH_NTD_BASE_ADDRESS + 0x9C)
#define	REG_TXH_NTD_FILTER_EXP_RX_IDX          (TXH_NTD_BASE_ADDRESS + 0xA0)
#define	REG_TXH_NTD_FILTER_EXP_RX_CONTROL      (TXH_NTD_BASE_ADDRESS + 0xA4)
#define	REG_TXH_NTD_FILTER_EXP_RX_DBG          (TXH_NTD_BASE_ADDRESS + 0xA8)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TXH_NTD_AUTO_RESP_TCS0 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp0RecipePtr:22;	// Auto response 0 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp0RxNavAbort:1;	// Auto response 0 RX NAV abort
		uint32 autoResp0TxNavAbort:1;	// Auto response 0 TX NAV abort
		uint32 autoResp0StaticBwAbort:1;	// Auto response 0 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs0_u;

/*REG_TXH_NTD_AUTO_RESP_TCS1 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp1RecipePtr:22;	// Auto response 1 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp1RxNavAbort:1;	// Auto response 1 RX NAV abort
		uint32 autoResp1TxNavAbort:1;	// Auto response 1 TX NAV abort
		uint32 autoResp1StaticBwAbort:1;	// Auto response 1 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs1_u;

/*REG_TXH_NTD_AUTO_RESP_TCS2 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp2RecipePtr:22;	// Auto response 2 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp2RxNavAbort:1;	// Auto response 2 RX NAV abort
		uint32 autoResp2TxNavAbort:1;	// Auto response 2 TX NAV abort
		uint32 autoResp2StaticBwAbort:1;	// Auto response 2 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs2_u;

/*REG_TXH_NTD_AUTO_RESP_TCS3 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp3RecipePtr:22;	// Auto response 3 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp3RxNavAbort:1;	// Auto response 3 RX NAV abort
		uint32 autoResp3TxNavAbort:1;	// Auto response 3 TX NAV abort
		uint32 autoResp3StaticBwAbort:1;	// Auto response 3 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs3_u;

/*REG_TXH_NTD_AUTO_RESP_TCS4 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp4RecipePtr:22;	// Auto response 4 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp4RxNavAbort:1;	// Auto response 4 RX NAV abort
		uint32 autoResp4TxNavAbort:1;	// Auto response 4 TX NAV abort
		uint32 autoResp4StaticBwAbort:1;	// Auto response 4 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs4_u;

/*REG_TXH_NTD_AUTO_RESP_TCS5 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp5RecipePtr:22;	// Auto response 5 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp5RxNavAbort:1;	// Auto response 5 RX NAV abort
		uint32 autoResp5TxNavAbort:1;	// Auto response 5 TX NAV abort
		uint32 autoResp5StaticBwAbort:1;	// Auto response 5 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs5_u;

/*REG_TXH_NTD_AUTO_RESP_TCS6 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp6RecipePtr:22;	// Auto response 6 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp6RxNavAbort:1;	// Auto response 6 RX NAV abort
		uint32 autoResp6TxNavAbort:1;	// Auto response 6 TX NAV abort
		uint32 autoResp6StaticBwAbort:1;	// Auto response 6 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs6_u;

/*REG_TXH_NTD_AUTO_RESP_TCS7 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoResp7RecipePtr:22;	// Auto response 7 recipe pointer
		uint32 reserved0:3;
		uint32 autoResp7RxNavAbort:1;	// Auto response 7 RX NAV abort
		uint32 autoResp7TxNavAbort:1;	// Auto response 7 TX NAV abort
		uint32 autoResp7StaticBwAbort:1;	// Auto response 7 Static BW abort
		uint32 reserved1:4;
	} bitFields;
} RegTxhNtdAutoRespTcs7_u;

/*REG_TXH_NTD_AUTO_RESP_RXD_LINES_A 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespRxdLinesA:32;	// Auto response RXD lines 0 to 7
	} bitFields;
} RegTxhNtdAutoRespRxdLinesA_u;

/*REG_TXH_NTD_AUTO_RESP_RXD_LINES_B 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespRxdLinesB:32;	// Auto response RXD lines 8 to 15
	} bitFields;
} RegTxhNtdAutoRespRxdLinesB_u;

/*REG_TXH_NTD_AUTO_RESP_RXD_LINES_C 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespRxdLinesC:32;	// Auto response RXD lines 16 to 23
	} bitFields;
} RegTxhNtdAutoRespRxdLinesC_u;

/*REG_TXH_NTD_AUTO_RESP_RXD_LINES_D 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespRxdLinesD:32;	// Auto response RXD lines 24 to 31
	} bitFields;
} RegTxhNtdAutoRespRxdLinesD_u;

/*REG_TXH_NTD_AUTO_RESP_RXD_LINES_E 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespRxdLinesE:8;	// Auto response RXD lines 32 to 33
		uint32 reserved0:24;
	} bitFields;
} RegTxhNtdAutoRespRxdLinesE_u;

/*REG_TXH_NTD_AUTO_RESP_CTR 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoRespTxCtr:8;	// Auto response Tx counter
		uint32 autoRespRxNavAbortCtr:8;	// Auto response Rx NAV abort counter
		uint32 autoRespTxNavAbortCtr:8;	// Auto response Tx NAV abort counter
		uint32 autoRespStaticBwAbortCtr:8;	// Auto response Static BW abort counter
	} bitFields;
} RegTxhNtdAutoRespCtr_u;

/*REG_TXH_NTD_SW_REQ_TCS_PART0 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqRecipePtr:22;	// SW request recipe pointer
		uint32 reserved0:2;
		uint32 swReqCcaAbort:1;	// SW request CCA primary abort
		uint32 swReqRxNavAbort:1;	// SW request Rx NAV abort
		uint32 swReqTxNavAbort:1;	// SW request Tx NAV abort
		uint32 swReqCcaSensitive:1;	// SW request CCA primary sensitive
		uint32 swReqRxNavSensitive:1;	// SW request Rx NAV sensitive
		uint32 swReqTxNavSensitive:1;	// SW request Tx NAV sensitive
		uint32 reserved1:2;
	} bitFields;
} RegTxhNtdSwReqTcsPart0_u;

/*REG_TXH_NTD_SW_REQ_TCS_PART1 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqIfsn:4;	// SW request IFSN
		uint32 reserved0:4;
		uint32 swReqBackoff:15;	// SW request Backoff
		uint32 reserved1:1;
		uint32 swReqUspCirBufAvail:4;	// User positions circular buffers available
		uint32 swReqMuActAllDelia:1;	// Activate all the 4 Delias - set for MU
		uint32 reserved2:1;
		uint32 swReqIrqClient:2;	// SW request IRQ client
	} bitFields;
} RegTxhNtdSwReqTcsPart1_u;

/*REG_TXH_NTD_SW_REQ_SET_CLEAR 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqSet:1;	// SW request set
		uint32 swReqClear:1;	// SW request Clear
		uint32 reserved0:30;
	} bitFields;
} RegTxhNtdSwReqSetClear_u;

/*REG_TXH_NTD_SW_REQ_STATUS 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqTxReq:1;	// SW request Tx request
		uint32 swReqPendingClrTxReq:1;	// SW request pending clear Tx request
		uint32 reserved0:2;
		uint32 swReqCurrIfsn:4;	// SW request current IFSN
		uint32 swReqCurrBackoff:15;	// SW request current Backoff
		uint32 reserved1:9;
	} bitFields;
} RegTxhNtdSwReqStatus_u;

/*REG_TXH_NTD_SW_REQ_IRQ_ENABLE 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqIrqEnable:5;	// SW request IRQ enable
		uint32 reserved0:27;
	} bitFields;
} RegTxhNtdSwReqIrqEnable_u;

/*REG_TXH_NTD_SW_REQ_IRQ_CLEAR 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqIrqClear:5;	// SW request IRQ clear
		uint32 reserved0:27;
	} bitFields;
} RegTxhNtdSwReqIrqClear_u;

/*REG_TXH_NTD_SW_REQ_IRQ_STATUS 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swReqTxDoneIrq:1;	// SW request Transmission done
		uint32 swReqClearDoneIrq:1;	// SW request Clear done
		uint32 swReqCcaAbortDoneIrq:1;	// SW request CCA primary abort done
		uint32 swReqRxNavAbortDoneIrq:1;	// SW request Rx NAV abort done
		uint32 swReqTxNavAbortDoneIrq:1;	// SW request Tx NAV abort done
		uint32 reserved0:27;
	} bitFields;
} RegTxhNtdSwReqIrqStatus_u;

/*REG_TXH_NTD_NTD_ARBITER_DEBUG 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ntdArbLastClient:2;	// Tx last client
		uint32 reserved0:2;
		uint32 currAutoRespTxReq:1;	// Current Auto Response Tx request
		uint32 currSequencerTxReq:1;	// Current Sequencer Tx request
		uint32 currSwReqTxReq:1;	// Current SW req Tx request
		uint32 currTxhMapTxReq:1;	// Current TXH MAP Tx request
		uint32 reserved1:24;
	} bitFields;
} RegTxhNtdNtdArbiterDebug_u;

/*REG_TXH_NTD_FILTER_EXP_RX_CTS 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filterExpRxCtsType:2;	// filter expected Rx CTS type
		uint32 reserved0:2;
		uint32 filterExpRxCtsSubtype:4;	// filter expected Rx CTS subtype
		uint32 filterExpRxCtsAddr1Lf0En:1;	// filter expected Rx CTS compare long filter0 enable
		uint32 filterExpRxCtsAddr1IdxEn:1;	// filter expected Rx CTS compare Addr1 Idx enable
		uint32 filterExpRxCtsAddr2IdxEn:1;	// filter expected Rx CTS compare Addr2 idx enable
		uint32 reserved1:5;
		uint32 informTxhMapRxCtsStatusEn:1;	// Inform TXH MAP Rx cts status enable
		uint32 reserved2:15;
	} bitFields;
} RegTxhNtdFilterExpRxCts_u;

/*REG_TXH_NTD_FILTER_EXP_RX_ACK_BA 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filterExpRxAckType:2;	// filter expected Rx ACK type
		uint32 reserved0:2;
		uint32 filterExpRxAckSubtype:4;	// filter expected Rx ACK subtype
		uint32 filterExpRxAckAddr1Lf0En:1;	// filter expected Rx ACK compare long filter0 enable
		uint32 filterExpRxAckAddr1IdxEn:1;	// filter expected Rx ACK compare Addr1 Idx enable
		uint32 filterExpRxAckAddr2IdxEn:1;	// filter expected Rx ACK compare Addr2 idx enable
		uint32 reserved1:5;
		uint32 filterExpRxBaType:2;	// filter expected Rx BA type
		uint32 reserved2:2;
		uint32 filterExpRxBaSubtype:4;	// filter expected Rx BA subtype
		uint32 filterExpRxBaAddr1Lf0En:1;	// filter expected Rx BA compare long filter0 enable
		uint32 filterExpRxBaAddr1IdxEn:1;	// filter expected Rx BA compare Addr1 Idx enable
		uint32 filterExpRxBaAddr2IdxEn:1;	// filter expected Rx BA compare Addr2 idx enable
		uint32 reserved3:5;
	} bitFields;
} RegTxhNtdFilterExpRxAckBa_u;

/*REG_TXH_NTD_FILTER_EXP_RX_SU_BF_RPT 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filterExpRxSuBfRptType:2;	// filter expected Rx SU BF report type
		uint32 reserved0:2;
		uint32 filterExpRxSuBfRptSubtype1:4;	// filter expected Rx SU BF report subtype1
		uint32 filterExpRxSuBfRptSubtype2:4;	// filter expected Rx SU BF report subtype2
		uint32 filterExpRxSuBfRptAddr1Lf0En:1;	// filter expected Rx SU BF report compare long filter0 enable
		uint32 filterExpRxSuBfRptAddr1IdxEn:1;	// filter expected Rx SU BF report compare Addr1 Idx enable
		uint32 filterExpRxSuBfRptAddr2IdxEn:1;	// filter expected Rx SU BF report compare Addr2 idx enable
		uint32 filterExpRxSuBfRptHwBfEn:1;	// filter expected Rx SU BF report verify BF HW result enable
		uint32 filterExpRxSuBfRptEarlySeqActEn:1;	// filter expected Rx SU BF report, enable activatation of Tx Sequencer when the BF header is ready and valid (i.e. without waiting to CRC result).
		uint32 reserved1:15;
	} bitFields;
} RegTxhNtdFilterExpRxSuBfRpt_u;

/*REG_TXH_NTD_FILTER_EXP_RX_MU_BF_RPT 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filterExpRxMuBfRptType:2;	// filter expected Rx MU BF report type
		uint32 reserved0:2;
		uint32 filterExpRxMuBfRptSubtype:4;	// filter expected Rx MU BF report subtype1
		uint32 reserved1:4;
		uint32 filterExpRxMuBfRptAddr1Lf0En:1;	// filter expected Rx MU BF report compare long filter0 enable
		uint32 filterExpRxMuBfRptAddr1IdxEn:1;	// filter expected Rx MU BF report compare Addr1 Idx enable
		uint32 filterExpRxMuBfRptAddr2IdxEn:1;	// filter expected Rx MU BF report compare Addr2 idx enable
		uint32 filterExpRxMuBfRptHwBfEn:1;	// filter expected Rx MU BF report verify BF HW result enable
		uint32 filterExpRxMuBfRptEarlySeqActEn:1;	// filter expected Rx MU BF report, enable activatation of Tx Sequencer when the BF header is ready and valid (i.e. without waiting to CRC result).
		uint32 reserved2:15;
	} bitFields;
} RegTxhNtdFilterExpRxMuBfRpt_u;

/*REG_TXH_NTD_FILTER_EXP_RX_IDX 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 filterExpRxAddr1Idx:4;	// filter expected Rx Addr1 index
		uint32 reserved0:4;
		uint32 filterExpRxAddr2Idx:8;	// filter expected Rx Addr2 index
		uint32 filterExpRxAddr2IdxValid:1;	// filter expected Rx Addr2 index valid
		uint32 reserved1:15;
	} bitFields;
} RegTxhNtdFilterExpRxIdx_u;

/*REG_TXH_NTD_FILTER_EXP_RX_CONTROL 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 expRespOperationSetStb:1;	// Expected Response operation set strobe
		uint32 filterExpRxCtsEn:1;	// Filter expected Rx CTS enable 
		uint32 filterExpRxAckEn:1;	// Filter expected Rx ACK enable 
		uint32 filterExpRxBaEn:1;	// Filter expected Rx BA enable 
		uint32 filterExpRxSuBfRpt1En:1;	// Filter expected Rx SU BF report1 enable 
		uint32 filterExpRxSuBfRpt2En:1;	// Filter expected Rx SU BF report2 enable 
		uint32 filterExpRxMuBfRptEn:1;	// Filter expected Rx MU BF report enable 
		uint32 reserved0:25;
	} bitFields;
} RegTxhNtdFilterExpRxControl_u;

/*REG_TXH_NTD_FILTER_EXP_RX_DBG 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 expRespOperationSet:1;	// Expected Response operation set
		uint32 expRxRespCtsResult:1;	// Expected Rx Response CTS result
		uint32 expRxRespAckResult:1;	// Expected Rx Response ACK result
		uint32 expRxRespBaResult:1;	// Expected Rx Response BA result
		uint32 expRxRespSuBfRptResult:1;	// Expected Rx Response SU BF Report result
		uint32 expRxRespMuBfRptResult:1;	// Expected Rx Response MU BF Report result
		uint32 expRxRespOtherRxResult:1;	// Expected Rx Response other Rx result
		uint32 expRxRespTimeoutResult:1;	// Expected Rx Response timeout result
		uint32 reserved0:24;
	} bitFields;
} RegTxhNtdFilterExpRxDbg_u;



#endif // _TXH_NTD_REGS_H_
