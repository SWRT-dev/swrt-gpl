
/***********************************************************************************
File:				QManagerHostRegs.h
Module:				qManagerHost
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _Q_MANAGER_HOST_REGS_H_
#define _Q_MANAGER_HOST_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define Q_MANAGER_HOST_BASE_ADDRESS                             MEMORY_MAP_UNIT_44_BASE_ADDRESS
#define	REG_Q_MANAGER_HOST_SW_MASTER1_CTL            (Q_MANAGER_HOST_BASE_ADDRESS + 0x200)
#define	REG_Q_MANAGER_HOST_SW_MASTER1_ADDR           (Q_MANAGER_HOST_BASE_ADDRESS + 0x204)
#define	REG_Q_MANAGER_HOST_SW_MASTER1_RETURN_ADDR    (Q_MANAGER_HOST_BASE_ADDRESS + 0x208)
#define	REG_Q_MANAGER_HOST_SW_MASTER1_GCLK_BYPASS    (Q_MANAGER_HOST_BASE_ADDRESS + 0x20C)
#define	REG_Q_MANAGER_HOST_SW_MASTER1_FIFO_STATUS    (Q_MANAGER_HOST_BASE_ADDRESS + 0x210)
#define	REG_Q_MANAGER_HOST_SW_MASTER1_CLEAN_FIFO     (Q_MANAGER_HOST_BASE_ADDRESS + 0x214)
#define	REG_Q_MANAGER_HOST_SW_MASTER1_FIFO_EN        (Q_MANAGER_HOST_BASE_ADDRESS + 0x218)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_Q_MANAGER_HOST_SW_MASTER1_CTL 0x200 */
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
} RegQManagerHostSwMaster1Ctl_u;

/*REG_Q_MANAGER_HOST_SW_MASTER1_ADDR 0x204 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 topAddr:16;	// Requested head address.
		uint32 bottomAddr:16;	// Requested tail address.
	} bitFields;
} RegQManagerHostSwMaster1Addr_u;

/*REG_Q_MANAGER_HOST_SW_MASTER1_RETURN_ADDR 0x208 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtnHeadPdAddr:16;	// There is a not Empty interrupt pending at the RX Lists DLM.
		uint32 rtnTailPdAddr:16;	// There is a not Empty interrupt pending at the TX Lists DLM.
	} bitFields;
} RegQManagerHostSwMaster1ReturnAddr_u;

/*REG_Q_MANAGER_HOST_SW_MASTER1_GCLK_BYPASS 0x20C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regGclkBypassEn:1;	// There is an Empty interrupt pending at the RX Lists DLM.
		uint32 fifoGclkBypassEn:1;	// There is an Empty interrupt pending at the TX Lists DLM.
		uint32 reserved0:30;
	} bitFields;
} RegQManagerHostSwMaster1GclkBypass_u;

/*REG_Q_MANAGER_HOST_SW_MASTER1_FIFO_STATUS 0x210 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swFifoHfull:1;	// RX Lists DLM Not Empty interrupt enable.
		uint32 swFifoFull:1;	// TX Lists DLM Not Empty interrupt enable.
		uint32 swFifoEmpty:1;	// RX Lists DLM Empty interrupt enable.
		uint32 swFifoPushWhileFull:1;	// TX Lists DLM Empty interrupt enable.
		uint32 swFifoPopWhileEmpty:1;	// FIFO gated clock bypass
		uint32 reserved0:27;
	} bitFields;
} RegQManagerHostSwMaster1FifoStatus_u;

/*REG_Q_MANAGER_HOST_SW_MASTER1_CLEAN_FIFO 0x214 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cleanFifo:1;	// Registers access gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegQManagerHostSwMaster1CleanFifo_u;

/*REG_Q_MANAGER_HOST_SW_MASTER1_FIFO_EN 0x218 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifoEn:1;	// Registers access gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegQManagerHostSwMaster1FifoEn_u;



#endif // _Q_MANAGER_HOST_REGS_H_
