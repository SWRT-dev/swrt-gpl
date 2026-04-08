
/***********************************************************************************
File:				QManagerRxRegs.h
Module:				qManagerRx
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _Q_MANAGER_RX_REGS_H_
#define _Q_MANAGER_RX_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define Q_MANAGER_RX_BASE_ADDRESS                             MEMORY_MAP_UNIT_44_BASE_ADDRESS
#define	REG_Q_MANAGER_RX_SW_MASTER2_CTL            (Q_MANAGER_RX_BASE_ADDRESS + 0x400)
#define	REG_Q_MANAGER_RX_SW_MASTER2_ADDR           (Q_MANAGER_RX_BASE_ADDRESS + 0x404)
#define	REG_Q_MANAGER_RX_SW_MASTER2_RETURN_ADDR    (Q_MANAGER_RX_BASE_ADDRESS + 0x408)
#define	REG_Q_MANAGER_RX_SW_MASTER2_GCLK_BYPASS    (Q_MANAGER_RX_BASE_ADDRESS + 0x40C)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_Q_MANAGER_RX_SW_MASTER2_CTL 0x400 */
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
} RegQManagerRxSwMaster2Ctl_u;

/*REG_Q_MANAGER_RX_SW_MASTER2_ADDR 0x404 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 topAddr:16;	// Top address shifted right 2 bits – Word aligned
		uint32 bottomAddr:16;	// Bottom address shifted right 2 bits – Word aligned
	} bitFields;
} RegQManagerRxSwMaster2Addr_u;

/*REG_Q_MANAGER_RX_SW_MASTER2_RETURN_ADDR 0x408 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rtnHeadPdAddr:16;	// Requested head address.
		uint32 rtnTailPdAddr:16;	// Requested tail address.
	} bitFields;
} RegQManagerRxSwMaster2ReturnAddr_u;

/*REG_Q_MANAGER_RX_SW_MASTER2_GCLK_BYPASS 0x40C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 regGclkBypassEn:1;	// Registers access gated clock bypass
		uint32 reserved0:31;
	} bitFields;
} RegQManagerRxSwMaster2GclkBypass_u;



#endif // _Q_MANAGER_RX_REGS_H_
