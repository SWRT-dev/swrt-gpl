
/***********************************************************************************
File:				QManagerUmacRegs.h
Module:				qManagerUmac
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _Q_MANAGER_UMAC_REGS_H_
#define _Q_MANAGER_UMAC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define Q_MANAGER_UMAC_BASE_ADDRESS                             MEMORY_MAP_UNIT_44_BASE_ADDRESS
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_CTL                             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x800)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_ADDR                            (Q_MANAGER_UMAC_BASE_ADDRESS + 0x804)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_RETURN_ADDR                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x808)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_GCLK_BYPASS                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x80C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_FIFO_STATUS                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x810)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_CLEAN_FIFO                      (Q_MANAGER_UMAC_BASE_ADDRESS + 0x814)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_FIFO_EN                         (Q_MANAGER_UMAC_BASE_ADDRESS + 0x818)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_INT_CTL                         (Q_MANAGER_UMAC_BASE_ADDRESS + 0x81C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_INT_ADDR                        (Q_MANAGER_UMAC_BASE_ADDRESS + 0x820)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_INT_RETURN_ADDR                 (Q_MANAGER_UMAC_BASE_ADDRESS + 0x824)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_ERR                             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x828)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_ERR_CLR                         (Q_MANAGER_UMAC_BASE_ADDRESS + 0x82C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_INT                             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x830)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_TX_LISTS_EMPTY_INT_EN           (Q_MANAGER_UMAC_BASE_ADDRESS + 0x838)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_TX_LISTS_NOT_EMPTY_INT_EN       (Q_MANAGER_UMAC_BASE_ADDRESS + 0x83C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_RX_LISTS_EMPTY_INT_EN           (Q_MANAGER_UMAC_BASE_ADDRESS + 0x840)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_RX_LISTS_NOT_EMPTY_INT_EN       (Q_MANAGER_UMAC_BASE_ADDRESS + 0x844)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_TX_DONE_INT                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x848)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_RX_DONE_INT                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x84C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_LO_PRI_TX_READY_INT             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x850)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_LO_PRI_RX_READY_INT             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x854)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_HI_PRI_TX_READY_INT             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x858)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_HI_PRI_RX_READY_INT             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x85C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_DESC_POOL_INT                   (Q_MANAGER_UMAC_BASE_ADDRESS + 0x860)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_TX_INT_CLR                      (Q_MANAGER_UMAC_BASE_ADDRESS + 0x864)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_RX_INT_CLR                      (Q_MANAGER_UMAC_BASE_ADDRESS + 0x868)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_DISABLE_NOT_EMPTY_INT_EN        (Q_MANAGER_UMAC_BASE_ADDRESS + 0x86C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_DISABLE_INT_CLR                 (Q_MANAGER_UMAC_BASE_ADDRESS + 0x870)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_DISABLE_INT                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x874)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_EN                         (Q_MANAGER_UMAC_BASE_ADDRESS + 0x878)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_CFG                        (Q_MANAGER_UMAC_BASE_ADDRESS + 0x87C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_LISTS_CFG                  (Q_MANAGER_UMAC_BASE_ADDRESS + 0x880)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_SINGLE_CHECK_CFG           (Q_MANAGER_UMAC_BASE_ADDRESS + 0x884)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_SINGLE_CHECK_ACTIVATE      (Q_MANAGER_UMAC_BASE_ADDRESS + 0x888)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_CHECK_STATUS               (Q_MANAGER_UMAC_BASE_ADDRESS + 0x88C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_DONE_COUNTER               (Q_MANAGER_UMAC_BASE_ADDRESS + 0x890)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_TRY_FAIL_COUNTER           (Q_MANAGER_UMAC_BASE_ADDRESS + 0x894)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_COUNTER_CLR                (Q_MANAGER_UMAC_BASE_ADDRESS + 0x898)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_AC_LEAVE_CRITERIA          (Q_MANAGER_UMAC_BASE_ADDRESS + 0x89C)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_AC_THRESH_CRITERIA_0_1     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8A0)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_AC_THRESH_CRITERIA_2_3     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8A4)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_SINGLE_PD_LIMIT            (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8A8)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA0_31_STATUS             (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8AC)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA32_63_STATUS            (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8B0)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA64_95_STATUS            (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8B4)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA96_127_STATUS           (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8B8)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_VAP_STATUS                 (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8BC)
#define	REG_Q_MANAGER_UMAC_Q_MANAGER_SPARE                            (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8C0)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_CFG                      (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8C4)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_DATA_SPEC_CLIENT_CFG     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8C8)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_LISTS_SPEC_CLIENT_CFG    (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8CC)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_BUSY                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8D0)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_IRQ_CLR                    (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8E0)
#define	REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_TMP_LIST_CFG               (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8E4)
#define	REG_Q_MANAGER_UMAC_UMAC_NULL_PUSH_ERR                         (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8E8)
#define	REG_Q_MANAGER_UMAC_UMAC_NULL_PUSH_ERR_CLR                     (Q_MANAGER_UMAC_BASE_ADDRESS + 0x8EC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_Q_MANAGER_UMAC_SW_MASTER3_CTL 0x800 */
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
} RegQManagerUmacSwMaster3Ctl_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_ADDR 0x804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 topAddr:16;	// Top address shifted right 2 bits – Word aligned
		uint32 bottomAddr:16;	// Bottom address shifted right 2 bits – Word aligned
	} bitFields;
} RegQManagerUmacSwMaster3Addr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_RETURN_ADDR 0x808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtnHeadPdAddr:16;	// Requested head address.
		uint32 rtnTailPdAddr:16;	// Requested tail address.
	} bitFields;
} RegQManagerUmacSwMaster3ReturnAddr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_GCLK_BYPASS 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regGclkBypassEn:1;	// Registers access gated clock bypass
		uint32 fifoGclkBypassEn:1;	// FIFO gated clock bypass
		uint32 qManagerAgerGclkBypassEn:1;	// Ager gated clock bypass
		uint32 dlmRegGclkBypassEn:1;	// DLMs gated clock bypass
		uint32 reserved0:28;
	} bitFields;
} RegQManagerUmacSwMaster3GclkBypass_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_FIFO_STATUS 0x810 */
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
} RegQManagerUmacSwMaster3FifoStatus_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_CLEAN_FIFO 0x814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cleanFifo:1;	// Cleans FIFO
		uint32 reserved0:31;
	} bitFields;
} RegQManagerUmacSwMaster3CleanFifo_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_FIFO_EN 0x818 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifoEn:1;	// Enables FIFO
		uint32 reserved0:31;
	} bitFields;
} RegQManagerUmacSwMaster3FifoEn_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_INT_CTL 0x81C */
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
} RegQManagerUmacSwMaster3IntCtl_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_INT_ADDR 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intTopAddr:16;	// Top address shifted right 2 bits – Word aligned
		uint32 intBottomAddr:16;	// Bottom address shifted right 2 bits – Word aligned
	} bitFields;
} RegQManagerUmacSwMaster3IntAddr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_INT_RETURN_ADDR 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intRtnHeadPdAddr:16;	// Requested head address.
		uint32 intRtnTailPdAddr:16;	// Requested tail address.
	} bitFields;
} RegQManagerUmacSwMaster3IntReturnAddr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_ERR 0x828 */
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
} RegQManagerUmacSwMaster3Err_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_ERR_CLR 0x82C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 errFifoClr:1;	// Clears FIFO error
		uint32 errDoubleBlockingReqClr:1;	// Clears double pop error
		uint32 reserved0:30;
	} bitFields;
} RegQManagerUmacSwMaster3ErrClr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_INT 0x830 */
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
} RegQManagerUmacSwMaster3Int_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_TX_LISTS_EMPTY_INT_EN 0x838 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txListsEmptyIntEn:32;	// Empty interrupt Enable
	} bitFields;
} RegQManagerUmacSwMaster3TxListsEmptyIntEn_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_TX_LISTS_NOT_EMPTY_INT_EN 0x83C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txListsNotEmptyIntEn:32;	// Not empty interrupt Enable
	} bitFields;
} RegQManagerUmacSwMaster3TxListsNotEmptyIntEn_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_RX_LISTS_EMPTY_INT_EN 0x840 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxListsEmptyIntEn:32;	// Empty interrupt Enable
	} bitFields;
} RegQManagerUmacSwMaster3RxListsEmptyIntEn_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_RX_LISTS_NOT_EMPTY_INT_EN 0x844 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxListsNotEmptyIntEn:32;	// Not empty interrupt Enable
	} bitFields;
} RegQManagerUmacSwMaster3RxListsNotEmptyIntEn_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_TX_DONE_INT 0x848 */
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
} RegQManagerUmacSwMaster3TxDoneInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_RX_DONE_INT 0x84C */
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
} RegQManagerUmacSwMaster3RxDoneInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_LO_PRI_TX_READY_INT 0x850 */
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
} RegQManagerUmacSwMaster3LoPriTxReadyInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_LO_PRI_RX_READY_INT 0x854 */
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
} RegQManagerUmacSwMaster3LoPriRxReadyInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_HI_PRI_TX_READY_INT 0x858 */
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
} RegQManagerUmacSwMaster3HiPriTxReadyInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_HI_PRI_RX_READY_INT 0x85C */
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
} RegQManagerUmacSwMaster3HiPriRxReadyInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_DESC_POOL_INT 0x860 */
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
} RegQManagerUmacSwMaster3DescPoolInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_TX_INT_CLR 0x864 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDoneListsIntClr:8;	// Clear High priority TXPD Done lists interrupt
		uint32 loPriTxReadyListsIntClr:8;	// Clear Low priority TX Ready lists interrupt
		uint32 hiPriTxReadyListsIntClr:8;	// Clear High priority TX Ready lists interrupt
		uint32 descPoolIntClrTx:8;	// Clear TX/RX pool lists interrupt
	} bitFields;
} RegQManagerUmacSwMaster3TxIntClr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_RX_INT_CLR 0x868 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxDoneListsIntClr:8;	// Clear High priority TXPD Done lists interrupt
		uint32 loPriRxReadyListsIntClr:8;	// Clear Low priority TX Ready lists interrupt
		uint32 hiPriRxReadyListsIntClr:8;	// Clear High priority TX Ready lists interrupt
		uint32 descPoolIntClrRx:8;	// Clear TX/RX pool lists interrupt
	} bitFields;
} RegQManagerUmacSwMaster3RxIntClr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_DISABLE_NOT_EMPTY_INT_EN 0x86C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacDisQueueNotEmptyIntEn:8;	// Not empty interrupt Enable
		uint32 reserved0:24;
	} bitFields;
} RegQManagerUmacSwMaster3DisableNotEmptyIntEn_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_DISABLE_INT_CLR 0x870 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacDisQueueNotEmptyIntClr:8;	// Clear High priority TXPD Done lists interrupt
		uint32 reserved0:24;
	} bitFields;
} RegQManagerUmacSwMaster3DisableIntClr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_DISABLE_INT 0x874 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacDisQueueNotEmptyInt:8;	// Descriptor pool TX lists empty Interrupts
		uint32 reserved0:24;
	} bitFields;
} RegQManagerUmacSwMaster3DisableInt_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_EN 0x878 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agerEn:1;
		uint32 reserved0:7;
		uint32 agerSwHalt:1;
		uint32 reserved1:23;
	} bitFields;
} RegQManagerUmacSwMaster3AgerEn_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_CFG 0x87C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agerPeriod:7;
		uint32 reserved0:1;
		uint32 shramWaitState:7;
		uint32 reserved1:1;
		uint32 clientWaitState:8;
		uint32 retryDiscardLock:1;
		uint32 reserved2:7;
	} bitFields;
} RegQManagerUmacSwMaster3AgerCfg_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_LISTS_CFG 0x880 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pdTypeUmacSel:16;
		uint32 liberatorListIdx:5;
		uint32 reserved0:3;
		uint32 umacListIdx:5;
		uint32 reserved1:3;
	} bitFields;
} RegQManagerUmacSwMaster3AgerListsCfg_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_SINGLE_CHECK_CFG 0x884 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startStaIdx:8;
		uint32 forceLock:1;
		uint32 reserved0:23;
	} bitFields;
} RegQManagerUmacSwMaster3AgerSingleCheckCfg_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_SINGLE_CHECK_ACTIVATE 0x888 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ageSingleCheckWaitRelease:1;
		uint32 reserved0:31;
	} bitFields;
} RegQManagerUmacSwMaster3AgerSingleCheckActivate_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_CHECK_STATUS 0x88C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 singleCheckInWait:1;
		uint32 singleCheckDone:1;
		uint32 backgroundCheckDone:1;
		uint32 reserved0:1;
		uint32 agerSm:4;
		uint32 lastStaChecked:8;
		uint32 lastTidChecked:3;
		uint32 reserved1:1;
		uint32 mostPdTid:3;
		uint32 reserved2:1;
		uint32 mostPdSta:8;
	} bitFields;
} RegQManagerUmacSwMaster3AgerCheckStatus_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_DONE_COUNTER 0x890 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacCounter:16;
		uint32 libCounter:16;
	} bitFields;
} RegQManagerUmacSwMaster3AgerDoneCounter_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_TRY_FAIL_COUNTER 0x894 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 tryFailCounter:16;
		uint32 reserved0:16;
	} bitFields;
} RegQManagerUmacSwMaster3AgerTryFailCounter_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_COUNTER_CLR 0x898 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacClear:1;
		uint32 libClear:1;
		uint32 tryFailClear:1;
		uint32 reserved0:29;
	} bitFields;
} RegQManagerUmacSwMaster3AgerCounterClr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_AC_LEAVE_CRITERIA 0x89C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beCriteria:7;
		uint32 reserved0:1;
		uint32 bkCriteria:7;
		uint32 reserved1:1;
		uint32 viCriteria:7;
		uint32 reserved2:1;
		uint32 voCriteria:7;
		uint32 reserved3:1;
	} bitFields;
} RegQManagerUmacSwMaster3AgerAcLeaveCriteria_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_AC_THRESH_CRITERIA_0_1 0x8A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 beThreshCriteria:14;
		uint32 reserved0:2;
		uint32 bkThreshCriteria:14;
		uint32 reserved1:2;
	} bitFields;
} RegQManagerUmacSwMaster3AgerAcThreshCriteria01_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_AC_THRESH_CRITERIA_2_3 0x8A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 viThreshCriteria:14;
		uint32 reserved0:2;
		uint32 voThreshCriteria:14;
		uint32 reserved1:2;
	} bitFields;
} RegQManagerUmacSwMaster3AgerAcThreshCriteria23_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_SINGLE_PD_LIMIT 0x8A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 singlePdDiscardLimit:14;
		uint32 reserved0:2;
		uint32 singlePdDiscardLimitEn:1;
		uint32 reserved1:15;
	} bitFields;
} RegQManagerUmacSwMaster3AgerSinglePdLimit_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA0_31_STATUS 0x8AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ageCheckSta031Status:32;
	} bitFields;
} RegQManagerUmacSwMaster3AgerSta031Status_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA32_63_STATUS 0x8B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ageCheckSta3263Status:32;
	} bitFields;
} RegQManagerUmacSwMaster3AgerSta3263Status_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA64_95_STATUS 0x8B4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ageCheckSta6495Status:32;
	} bitFields;
} RegQManagerUmacSwMaster3AgerSta6495Status_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_STA96_127_STATUS 0x8B8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ageCheckSta96127Status:32;
	} bitFields;
} RegQManagerUmacSwMaster3AgerSta96127Status_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_VAP_STATUS 0x8BC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ageCheckVapStatus:16;
		uint32 reserved0:16;
	} bitFields;
} RegQManagerUmacSwMaster3AgerVapStatus_u;

/*REG_Q_MANAGER_UMAC_Q_MANAGER_SPARE 0x8C0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qManagerSpare:32;
	} bitFields;
} RegQManagerUmacQManagerSpare_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_CFG 0x8C4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qmAgerLoggerPriority:2;
		uint32 qmAgerLoggerEn:1;
		uint32 reserved0:1;
		uint32 qmTxDataDlmLoggerPriority:2;
		uint32 qmTxDataDlmLoggerEn:1;
		uint32 reserved1:1;
		uint32 qmTxListsDlmLoggerPriority:2;
		uint32 qmTxListsDlmLoggerEn:1;
		uint32 reserved2:1;
		uint32 qmRxDataDlmLoggerPriority:2;
		uint32 qmRxDataDlmLoggerEn:1;
		uint32 reserved3:1;
		uint32 qmRxListsDlmLoggerPriority:2;
		uint32 qmRxListsDlmLoggerEn:1;
		uint32 reserved4:1;
		uint32 maxMessageCount:5;
		uint32 reserved5:7;
	} bitFields;
} RegQManagerUmacSwMaster3LoggerCfg_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_DATA_SPEC_CLIENT_CFG 0x8C8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerTxDataDlmPrimary:8;
		uint32 loggerTxDataDlmSecondary:3;
		uint32 reserved0:1;
		uint32 loggerTxDataDlmDpl:3;
		uint32 qmTxDataDlmLoggerSpecificClient:1;
		uint32 loggerRxDataDlmPrimary:8;
		uint32 loggerRxDataDlmSecondary:3;
		uint32 reserved1:4;
		uint32 qmRxDataDlmLoggerSpecificClient:1;
	} bitFields;
} RegQManagerUmacSwMaster3LoggerDataSpecClientCfg_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_LISTS_SPEC_CLIENT_CFG 0x8CC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerTxListsDlmDpl:5;
		uint32 reserved0:3;
		uint32 qmTxListsDlmLoggerSpecificClient:1;
		uint32 reserved1:7;
		uint32 loggerRxListsDlmDpl:5;
		uint32 reserved2:3;
		uint32 qmRxListsDlmLoggerSpecificClient:1;
		uint32 reserved3:7;
	} bitFields;
} RegQManagerUmacSwMaster3LoggerListsSpecClientCfg_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_LOGGER_BUSY 0x8D0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 qmAgerLoggerBusy:1;
		uint32 reserved0:7;
		uint32 qmTxDataDlmLoggerBusy:1;
		uint32 qmTxListsDlmLoggerBusy:1;
		uint32 qmRxDataDlmLoggerBusy:1;
		uint32 qmRxListsDlmLoggerBusy:1;
		uint32 reserved1:20;
	} bitFields;
} RegQManagerUmacSwMaster3LoggerBusy_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_IRQ_CLR 0x8E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agerDoneIrqClr:1;
		uint32 reserved0:31;
	} bitFields;
} RegQManagerUmacSwMaster3AgerIrqClr_u;

/*REG_Q_MANAGER_UMAC_SW_MASTER3_AGER_TMP_LIST_CFG 0x8E4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacTmpListIdx:5;
		uint32 reserved0:27;
	} bitFields;
} RegQManagerUmacSwMaster3AgerTmpListCfg_u;

/*REG_Q_MANAGER_UMAC_UMAC_NULL_PUSH_ERR 0x8E8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacSwMaster0NullPushErr:1;
		uint32 umacSwMaster1NullPushErr:1;
		uint32 umacSwMaster2NullPushErr:1;
		uint32 umacSwMaster3NullPushErr:1;
		uint32 umacSwMaster4NullPushErr:1;
		uint32 umacSwMaster3IntNullPushErr:1;
		uint32 umacSwMaster4IntNullPushErr:1;
		uint32 reserved0:25;
	} bitFields;
} RegQManagerUmacUmacNullPushErr_u;

/*REG_Q_MANAGER_UMAC_UMAC_NULL_PUSH_ERR_CLR 0x8EC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacSwMaster0NullPushErrClr:1;
		uint32 umacSwMaster1NullPushErrClr:1;
		uint32 umacSwMaster2NullPushErrClr:1;
		uint32 umacSwMaster3NullPushErrClr:1;
		uint32 umacSwMaster4NullPushErrClr:1;
		uint32 umacSwMaster3IntNullPushErrClr:1;
		uint32 umacSwMaster4IntNullPushErrClr:1;
		uint32 reserved0:25;
	} bitFields;
} RegQManagerUmacUmacNullPushErrClr_u;



#endif // _Q_MANAGER_UMAC_REGS_H_
