
/***********************************************************************************
File:				QManagerLmacRegs.h
Module:				qManagerLmac
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _Q_MANAGER_LMAC_REGS_H_
#define _Q_MANAGER_LMAC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define Q_MANAGER_LMAC_BASE_ADDRESS                             MEMORY_MAP_UNIT_44_BASE_ADDRESS
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_CTL                          (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA00)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_ADDR                         (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA04)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_RETURN_ADDR                  (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA08)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_GCLK_BYPASS                  (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA0C)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_FIFO_STATUS                  (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA10)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_CLEAN_FIFO                   (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA14)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_FIFO_EN                      (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA18)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_INT_CTL                      (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA1C)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_INT_ADDR                     (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA20)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_INT_RETURN_ADDR              (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA24)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_ERR                          (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA28)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_ERR_CLR                      (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA2C)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_INT                          (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA30)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_TX_LISTS_EMPTY_INT_EN        (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA38)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_TX_LISTS_NOT_EMPTY_INT_EN    (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA3C)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_RX_LISTS_EMPTY_INT_EN        (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA40)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_RX_LISTS_NOT_EMPTY_INT_EN    (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA44)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_TX_DONE_INT                  (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA48)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_RX_DONE_INT                  (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA4C)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_LO_PRI_TX_READY_INT          (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA50)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_LO_PRI_RX_READY_INT          (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA54)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_HI_PRI_TX_READY_INT          (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA58)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_HI_PRI_RX_READY_INT          (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA5C)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_DESC_POOL_INT                (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA60)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_TX_INT_CLR                   (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA64)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_RX_INT_CLR                   (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA68)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_DISABLE_NOT_EMPTY_INT_EN     (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA6C)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_DISABLE_INT_CLR              (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA70)
#define	REG_Q_MANAGER_LMAC_SW_MASTER4_DISABLE_INT                  (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA74)
#define	REG_Q_MANAGER_LMAC_LMAC_NULL_PUSH_ERR                      (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA78)
#define	REG_Q_MANAGER_LMAC_LMAC_NULL_PUSH_ERR_CLR                  (Q_MANAGER_LMAC_BASE_ADDRESS + 0xA7C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_Q_MANAGER_LMAC_SW_MASTER4_CTL 0xA00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctlPrimaryAddr:7;	// Primary address
		uint32 ctlRxMgmt:1;
		uint32 ctlSecondaryAddr:3;	// Secondary address
		uint32 reserved0:5;
		uint32 ctlDplIdx:5;	// Descriptor Pointer List index number. Values can be 0-31
		uint32 reserved1:3;
		uint32 ctlDlmIdx:3;	// DLM index number. Values can be 0-3
		uint32 ctlSetNull:1;	// Set Null to the "Next descriptor field" of the pushed PD
		uint32 ctlReq:4;	// DLM Request
	} bitFields;
} RegQManagerLmacSwMaster4Ctl_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_ADDR 0xA04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 topAddr:16;	// Top address shifted right 2 bits – Word aligned
		uint32 bottomAddr:16;	// Bottom address shifted right 2 bits – Word aligned
	} bitFields;
} RegQManagerLmacSwMaster4Addr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_RETURN_ADDR 0xA08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtnHeadPdAddr:16;	// Requested head address.
		uint32 rtnTailPdAddr:16;	// Requested tail address.
	} bitFields;
} RegQManagerLmacSwMaster4ReturnAddr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_GCLK_BYPASS 0xA0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regGclkBypassEn:1;	// Registers access gated clock bypass
		uint32 fifoGclkBypassEn:1;	// FIFO gated clock bypass
		uint32 reserved0:30;
	} bitFields;
} RegQManagerLmacSwMaster4GclkBypass_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_FIFO_STATUS 0xA10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swFifoHfull:1;	// SW FIFO half full indication
		uint32 swFifoFull:1;	// SW FIFO full indication
		uint32 swFifoEmpty:1;	// SW FIFO empty indication
		uint32 swFifoPushWhileFull:1;	// SW FIFO overflow – push when FIFO is full
		uint32 swFifoPopWhileEmpty:1;	// SW FIFO underflow – pop when there is no data
		uint32 reserved0:27;
	} bitFields;
} RegQManagerLmacSwMaster4FifoStatus_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_CLEAN_FIFO 0xA14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cleanFifo:1;	// Cleans FIFO
		uint32 reserved0:31;
	} bitFields;
} RegQManagerLmacSwMaster4CleanFifo_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_FIFO_EN 0xA18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifoEn:1;	// Enables FIFO
		uint32 reserved0:31;
	} bitFields;
} RegQManagerLmacSwMaster4FifoEn_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_INT_CTL 0xA1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intCtlPrimaryAddr:7;	// Primary address
		uint32 intCtlRxMgmt:1;
		uint32 intCtlSecondaryAddr:3;	// Secondary address
		uint32 reserved0:5;
		uint32 intCtlDplIdx:5;	// Descriptor Pointer List index number. Values can be 0-31
		uint32 reserved1:3;
		uint32 intCtlDlmIdx:3;	// DLM index number. Values can be 0-3
		uint32 intCtlSetNull:1;	// Set Null to the "Next descriptor field" of the pushed PD
		uint32 intCtlReq:4;	// DLM Request
	} bitFields;
} RegQManagerLmacSwMaster4IntCtl_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_INT_ADDR 0xA20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intTopAddr:16;	// Top address shifted right 2 bits – Word aligned
		uint32 intBottomAddr:16;	// Bottom address shifted right 2 bits – Word aligned
	} bitFields;
} RegQManagerLmacSwMaster4IntAddr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_INT_RETURN_ADDR 0xA24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intRtnHeadPdAddr:16;	// Requested head address.
		uint32 intRtnTailPdAddr:16;	// Requested tail address.
	} bitFields;
} RegQManagerLmacSwMaster4IntReturnAddr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_ERR 0xA28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDataErrPopDisabled:1;	// Pop to a disabled client
		uint32 txDataErrPopDisabled:1;	// Pop to a disabled client
		uint32 errFifo:1;	// FIFO underflow/overflow error
		uint32 errDoubleBlockingReq:1;	// Pop while another pop takes place
		uint32 reserved0:28;
	} bitFields;
} RegQManagerLmacSwMaster4Err_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_ERR_CLR 0xA2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errFifoClr:1;	// Clears FIFO error
		uint32 errDoubleBlockingReqClr:1;	// Clears double pop error
		uint32 reserved0:30;
	} bitFields;
} RegQManagerLmacSwMaster4ErrClr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_INT 0xA30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDoneListsInt:1;	// Interrupt coming from the High priority TXPD Done lists
		uint32 rxDoneListsInt:1;	// Interrupt coming from the RXPD Done lists
		uint32 loPriTxReadyListsInt:1;	// Interrupt coming from the Low priority TX Ready lists
		uint32 loPriRxReadyListsInt:1;	// Interrupt coming from the Low priority RX Ready lists
		uint32 hiPriTxReadyListsInt:1;	// Interrupt coming from the High priority TX Ready lists
		uint32 hiPriRxReadyListsInt:1;	// Interrupt coming from the High priority RX Ready lists
		uint32 descPoolInt:1;	// Interrupt coming from the TX/RX pool lists
		uint32 reserved0:25;
	} bitFields;
} RegQManagerLmacSwMaster4Int_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_TX_LISTS_EMPTY_INT_EN 0xA38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txListsEmptyIntEn:32;	// Empty interrupt Enable
	} bitFields;
} RegQManagerLmacSwMaster4TxListsEmptyIntEn_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_TX_LISTS_NOT_EMPTY_INT_EN 0xA3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txListsNotEmptyIntEn:32;	// Not empty interrupt Enable
	} bitFields;
} RegQManagerLmacSwMaster4TxListsNotEmptyIntEn_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_RX_LISTS_EMPTY_INT_EN 0xA40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxListsEmptyIntEn:32;	// Empty interrupt Enable
	} bitFields;
} RegQManagerLmacSwMaster4RxListsEmptyIntEn_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_RX_LISTS_NOT_EMPTY_INT_EN 0xA44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxListsNotEmptyIntEn:32;	// Not empty interrupt Enable
	} bitFields;
} RegQManagerLmacSwMaster4RxListsNotEmptyIntEn_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_TX_DONE_INT 0xA48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDoneListsEmptyInt:8;	// TX done lists empty Interrupts
		uint32 reserved0:8;
		uint32 txDoneListsNotEmptyInt:8;	// TX done lists not empty Interrupts
		uint32 reserved1:8;
	} bitFields;
} RegQManagerLmacSwMaster4TxDoneInt_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_RX_DONE_INT 0xA4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDoneListsEmptyInt:8;	// RX done lists empty Interrupts
		uint32 reserved0:8;
		uint32 rxDoneListsNotEmptyInt:8;	// RX done lists not empty Interrupts
		uint32 reserved1:8;
	} bitFields;
} RegQManagerLmacSwMaster4RxDoneInt_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_LO_PRI_TX_READY_INT 0xA50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txLoPriReadyEmptyInt:8;	// TX Low priority ready lists empty Interrupts
		uint32 reserved0:8;
		uint32 txLoPriReadyNotEmptyInt:8;	// TX Low priority ready lists not empty Interrupts
		uint32 reserved1:8;
	} bitFields;
} RegQManagerLmacSwMaster4LoPriTxReadyInt_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_LO_PRI_RX_READY_INT 0xA54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxLoPriReadyEmptyInt:8;	// RX Low priority ready lists empty Interrupts
		uint32 reserved0:8;
		uint32 rxLoPriReadyNotEmptyInt:8;	// RX Low priority ready lists not empty Interrupts
		uint32 reserved1:8;
	} bitFields;
} RegQManagerLmacSwMaster4LoPriRxReadyInt_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_HI_PRI_TX_READY_INT 0xA58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txHiPriReadyEmptyInt:8;	// TX high priority ready lists empty Interrupts
		uint32 reserved0:8;
		uint32 txHiPriReadyNotEmptyInt:8;	// TX high priority ready lists not empty Interrupts
		uint32 reserved1:8;
	} bitFields;
} RegQManagerLmacSwMaster4HiPriTxReadyInt_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_HI_PRI_RX_READY_INT 0xA5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHiPriReadyEmptyInt:8;	// RX high priority ready lists empty Interrupts
		uint32 reserved0:8;
		uint32 rxHiPriReadyNotEmptyInt:8;	// RX high priority ready lists not empty Interrupts
		uint32 reserved1:8;
	} bitFields;
} RegQManagerLmacSwMaster4HiPriRxReadyInt_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_DESC_POOL_INT 0xA60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 descTxPoolEmptyInt:8;	// Descriptor pool TX lists empty Interrupts
		uint32 descRxPoolEmptyInt:8;	// Descriptor pool RX lists empty Interrupts
		uint32 descTxPoolNotEmptyInt:8;	// Descriptor pool TX lists not empty Interrupts
		uint32 descRxPoolNotEmptyInt:8;	// Descriptor pool RX lists not empty Interrupts
	} bitFields;
} RegQManagerLmacSwMaster4DescPoolInt_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_TX_INT_CLR 0xA64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDoneListsIntClr:8;	// Clear High priority TXPD Done lists interrupt
		uint32 hiPriTxReadyListsIntClr:8;	// Clear High priority TX Ready lists interrupt
		uint32 loPriTxReadyListsIntClr:8;	// Clear Low priority TX Ready lists interrupt
		uint32 descPoolIntClrTx:8;	// Clear TX/RX pool lists interrupt
	} bitFields;
} RegQManagerLmacSwMaster4TxIntClr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_RX_INT_CLR 0xA68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDoneListsIntClr:8;	// Clear High priority TXPD Done lists interrupt
		uint32 hiPriRxReadyListsIntClr:8;	// Clear High priority TX Ready lists interrupt
		uint32 loPriRxReadyListsIntClr:8;	// Clear Low priority TX Ready lists interrupt
		uint32 descPoolIntClrRx:8;	// Clear TX/RX pool lists interrupt
	} bitFields;
} RegQManagerLmacSwMaster4RxIntClr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_DISABLE_NOT_EMPTY_INT_EN 0xA6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacDisQueueNotEmptyIntEn:8;	// Not empty interrupt Enable
		uint32 reserved0:24;
	} bitFields;
} RegQManagerLmacSwMaster4DisableNotEmptyIntEn_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_DISABLE_INT_CLR 0xA70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacDisQueueNotEmptyIntClr:8;	// Clear High priority TXPD Done lists interrupt
		uint32 reserved0:24;
	} bitFields;
} RegQManagerLmacSwMaster4DisableIntClr_u;

/*REG_Q_MANAGER_LMAC_SW_MASTER4_DISABLE_INT 0xA74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacDisQueueNotEmptyInt:8;	// TX done lists empty Interrupts
		uint32 reserved0:24;
	} bitFields;
} RegQManagerLmacSwMaster4DisableInt_u;

/*REG_Q_MANAGER_LMAC_LMAC_NULL_PUSH_ERR 0xA78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacSwMaster0NullPushErr:1;	// Pop to a disabled client
		uint32 lmacSwMaster1NullPushErr:1;	// Pop to a disabled client
		uint32 lmacSwMaster2NullPushErr:1;	// FIFO underflow/overflow error
		uint32 lmacSwMaster3NullPushErr:1;	// Pop while another pop takes place
		uint32 lmacSwMaster4NullPushErr:1;
		uint32 lmacSwMaster3IntNullPushErr:1;
		uint32 lmacSwMaster4IntNullPushErr:1;
		uint32 reserved0:25;
	} bitFields;
} RegQManagerLmacLmacNullPushErr_u;

/*REG_Q_MANAGER_LMAC_LMAC_NULL_PUSH_ERR_CLR 0xA7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacSwMaster0NullPushErrClr:1;	// Clears FIFO error
		uint32 lmacSwMaster1NullPushErrClr:1;	// Clears double pop error
		uint32 lmacSwMaster2NullPushErrClr:1;
		uint32 lmacSwMaster3NullPushErrClr:1;
		uint32 lmacSwMaster4NullPushErrClr:1;
		uint32 lmacSwMaster3IntNullPushErrClr:1;
		uint32 lmacSwMaster4IntNullPushErrClr:1;
		uint32 reserved0:25;
	} bitFields;
} RegQManagerLmacLmacNullPushErrClr_u;



#endif // _Q_MANAGER_LMAC_REGS_H_
