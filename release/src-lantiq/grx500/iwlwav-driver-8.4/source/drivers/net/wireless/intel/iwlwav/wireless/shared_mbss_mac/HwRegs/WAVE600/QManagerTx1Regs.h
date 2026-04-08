
/***********************************************************************************
File:				QManagerTx1Regs.h
Module:				qManagerTx1
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _Q_MANAGER_TX1_REGS_H_
#define _Q_MANAGER_TX1_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define Q_MANAGER_TX1_BASE_ADDRESS                             MEMORY_MAP_UNIT_30_BASE_ADDRESS
#define	REG_Q_MANAGER_TX1_CTL                 (Q_MANAGER_TX1_BASE_ADDRESS + 0x0)
#define	REG_Q_MANAGER_TX1_PUSH_ADDR_TOP       (Q_MANAGER_TX1_BASE_ADDRESS + 0x4)
#define	REG_Q_MANAGER_TX1_PUSH_ADDR_BOTTOM    (Q_MANAGER_TX1_BASE_ADDRESS + 0x8)
#define	REG_Q_MANAGER_TX1_POP_ADDR_TOP        (Q_MANAGER_TX1_BASE_ADDRESS + 0xC)
#define	REG_Q_MANAGER_TX1_POP_ADDR_BOTTOM     (Q_MANAGER_TX1_BASE_ADDRESS + 0x10)
#define	REG_Q_MANAGER_TX1_FIFO_STATUS         (Q_MANAGER_TX1_BASE_ADDRESS + 0x18)
#define	REG_Q_MANAGER_TX1_CLEAN_FIFO          (Q_MANAGER_TX1_BASE_ADDRESS + 0x1C)
#define	REG_Q_MANAGER_TX1_FIFO_EN             (Q_MANAGER_TX1_BASE_ADDRESS + 0x20)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_Q_MANAGER_TX1_CTL 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ctlPrimaryAddr : 8; //Primary address, reset value: 0x0, access type: RW
		uint32 ctlSecondaryAddr : 3; //Secondary address, reset value: 0x0, access type: RW
		uint32 ctlMgmt : 1; //Mgmt TID. When set Mgmt TID will be accessed and secondary field will be ignored., reset value: 0x0, access type: RW
		uint32 ctlSetNull : 1; //Set Null to the "Next descriptor field" of the pushed PD, reset value: 0x0, access type: RW
		uint32 reserved0 : 3;
		uint32 ctlDplIdx : 8; //Descriptor Pointer List index number. Values can be 0-31, reset value: 0x0, access type: RW
		uint32 ctlDlmIdx : 4; //DLM index number. Values can be 0-8: , 0x0: TX Data DLM , 0x1: TX Lists DLM , 0x2: RX Data DLM , 0x3: RX Lists DLM , 0x4: DMA Lists DLM , 0x5: RX MPDU Descriptor Band0 Lists DLM , 0x6: RX MPDU Descriptor Band1 Lists DLM , 0x7: TX MPDU Descriptor Band0 Lists DLM , 0x8: TX MPDU Descriptor Band1 Lists DLM , , reset value: 0x0, access type: RW
		uint32 ctlReq : 4; //DLM Request, reset value: 0x0, access type: RW
	} bitFields;
} RegQManagerTx1Ctl_u;

/*REG_Q_MANAGER_TX1_PUSH_ADDR_TOP 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pushAddrTop : 24; //Top address pointer, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegQManagerTx1PushAddrTop_u;

/*REG_Q_MANAGER_TX1_PUSH_ADDR_BOTTOM 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pushAddrBottom : 24; //Bottom address pointer, reset value: 0x0, access type: RW
		uint32 reserved0 : 8;
	} bitFields;
} RegQManagerTx1PushAddrBottom_u;

/*REG_Q_MANAGER_TX1_POP_ADDR_TOP 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 popAddrTop : 24; //Requested pop head address pointer., reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegQManagerTx1PopAddrTop_u;

/*REG_Q_MANAGER_TX1_POP_ADDR_BOTTOM 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 popAddrBottom : 24; //Requested pop tail address pointer., reset value: 0x0, access type: RO
		uint32 reserved0 : 8;
	} bitFields;
} RegQManagerTx1PopAddrBottom_u;

/*REG_Q_MANAGER_TX1_FIFO_STATUS 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swFifoHfull : 1; //RX Lists DLM Not Empty interrupt enable., reset value: 0x0, access type: RO
		uint32 swFifoFull : 1; //TX Lists DLM Not Empty interrupt enable., reset value: 0x0, access type: RO
		uint32 swFifoEmpty : 1; //RX Lists DLM Empty interrupt enable., reset value: 0x1, access type: RO
		uint32 swFifoPushWhileFull : 1; //TX Lists DLM Empty interrupt enable., reset value: 0x0, access type: RO
		uint32 swFifoPopWhileEmpty : 1; //FIFO gated clock bypass, reset value: 0x0, access type: RO
		uint32 reserved0 : 27;
	} bitFields;
} RegQManagerTx1FifoStatus_u;

/*REG_Q_MANAGER_TX1_CLEAN_FIFO 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cleanFifo : 1; //Registers access gated clock bypass, reset value: 0x0, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegQManagerTx1CleanFifo_u;

/*REG_Q_MANAGER_TX1_FIFO_EN 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifoEn : 1; //Registers access gated clock bypass, reset value: 0x1, access type: RW
		uint32 reserved0 : 31;
	} bitFields;
} RegQManagerTx1FifoEn_u;



#endif // _Q_MANAGER_TX1_REGS_H_
