
/***********************************************************************************
File:				QManagerTxRegs.h
Module:				qManagerTx
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _Q_MANAGER_TX_REGS_H_
#define _Q_MANAGER_TX_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define Q_MANAGER_TX_BASE_ADDRESS                             MEMORY_MAP_UNIT_44_BASE_ADDRESS
#define	REG_Q_MANAGER_TX_SW_MASTER0_CTL            (Q_MANAGER_TX_BASE_ADDRESS + 0x0)
#define	REG_Q_MANAGER_TX_SW_MASTER0_ADDR           (Q_MANAGER_TX_BASE_ADDRESS + 0x4)
#define	REG_Q_MANAGER_TX_SW_MASTER0_RETURN_ADDR    (Q_MANAGER_TX_BASE_ADDRESS + 0x8)
#define	REG_Q_MANAGER_TX_SW_MASTER0_GCLK_BYPASS    (Q_MANAGER_TX_BASE_ADDRESS + 0xC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_Q_MANAGER_TX_SW_MASTER0_CTL 0x0 */
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
} RegQManagerTxSwMaster0Ctl_u;

/*REG_Q_MANAGER_TX_SW_MASTER0_ADDR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 topAddr:16;	// Top address shifted right 2 bits – Word aligned
		uint32 bottomAddr:16;	// Bottom address shifted right 2 bits – Word aligned
	} bitFields;
} RegQManagerTxSwMaster0Addr_u;

/*REG_Q_MANAGER_TX_SW_MASTER0_RETURN_ADDR 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtnHeadPdAddr:16;	// Requested head address.
		uint32 rtnTailPdAddr:16;	// Requested tail address.
	} bitFields;
} RegQManagerTxSwMaster0ReturnAddr_u;

/*REG_Q_MANAGER_TX_SW_MASTER0_GCLK_BYPASS 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regGclkBypassEn:1;	// Registers access gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegQManagerTxSwMaster0GclkBypass_u;



#endif // _Q_MANAGER_TX_REGS_H_
