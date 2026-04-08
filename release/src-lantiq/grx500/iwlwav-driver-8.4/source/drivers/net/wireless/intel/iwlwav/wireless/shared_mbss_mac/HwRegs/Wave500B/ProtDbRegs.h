
/***********************************************************************************
File:				ProtDbRegs.h
Module:				protDb
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PROT_DB_REGS_H_
#define _PROT_DB_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PROT_DB_BASE_ADDRESS                             MEMORY_MAP_UNIT_46_BASE_ADDRESS
#define	REG_PROT_DB_ENTITY0_PROT_SHADOW_MEM_8B_OFFSET    (PROT_DB_BASE_ADDRESS + 0x0)
#define	REG_PROT_DB_ENTITY0_PROT_START_COMMAND           (PROT_DB_BASE_ADDRESS + 0x4)
#define	REG_PROT_DB_ENTITY0_PROT_IRQ                     (PROT_DB_BASE_ADDRESS + 0x8)
#define	REG_PROT_DB_ENTITY0_PROT_CLEAR_IRQ               (PROT_DB_BASE_ADDRESS + 0xC)
#define	REG_PROT_DB_ENTITY0_PROT_BUSY                    (PROT_DB_BASE_ADDRESS + 0x10)
#define	REG_PROT_DB_ENTITY1_PROT_SHADOW_MEM_8B_OFFSET    (PROT_DB_BASE_ADDRESS + 0x20)
#define	REG_PROT_DB_ENTITY1_PROT_START_COMMAND           (PROT_DB_BASE_ADDRESS + 0x24)
#define	REG_PROT_DB_ENTITY1_PROT_IRQ                     (PROT_DB_BASE_ADDRESS + 0x28)
#define	REG_PROT_DB_ENTITY1_PROT_CLEAR_IRQ               (PROT_DB_BASE_ADDRESS + 0x2C)
#define	REG_PROT_DB_ENTITY1_PROT_BUSY                    (PROT_DB_BASE_ADDRESS + 0x30)
#define	REG_PROT_DB_PROT_DB_DEBUG                        (PROT_DB_BASE_ADDRESS + 0x40)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PROT_DB_ENTITY0_PROT_SHADOW_MEM_8B_OFFSET 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtShadowMem8BOffset:4;	// entity0 prot shadow mem 8Bytes offset
		uint32 reserved0:28;
	} bitFields;
} RegProtDbEntity0ProtShadowMem8BOffset_u;

/*REG_PROT_DB_ENTITY0_PROT_START_COMMAND 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 entity0ProtDestBaseAddr:22;	// entity0 prot destination base address
		uint32 entity0ProtWordsNum:5;	// entity0 prot words num
		uint32 reserved1:3;
	} bitFields;
} RegProtDbEntity0ProtStartCommand_u;

/*REG_PROT_DB_ENTITY0_PROT_IRQ 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtIrq:1;	// entity0 prot interrupt
		uint32 reserved0:31;
	} bitFields;
} RegProtDbEntity0ProtIrq_u;

/*REG_PROT_DB_ENTITY0_PROT_CLEAR_IRQ 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtClearIrqStb:1;	// entity0 prot clear interrupt
		uint32 reserved0:31;
	} bitFields;
} RegProtDbEntity0ProtClearIrq_u;

/*REG_PROT_DB_ENTITY0_PROT_BUSY 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtBusy:1;	// entity0 prot busy indication
		uint32 reserved0:31;
	} bitFields;
} RegProtDbEntity0ProtBusy_u;

/*REG_PROT_DB_ENTITY1_PROT_SHADOW_MEM_8B_OFFSET 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtShadowMem8BOffset:4;	// entity1 prot shadow mem 8Bytes offset
		uint32 reserved0:28;
	} bitFields;
} RegProtDbEntity1ProtShadowMem8BOffset_u;

/*REG_PROT_DB_ENTITY1_PROT_START_COMMAND 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 entity1ProtDestBaseAddr:22;	// entity1 prot destination base address
		uint32 entity1ProtWordsNum:5;	// entity1 prot words num
		uint32 reserved1:3;
	} bitFields;
} RegProtDbEntity1ProtStartCommand_u;

/*REG_PROT_DB_ENTITY1_PROT_IRQ 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtIrq:1;	// entity1 prot interrupt
		uint32 reserved0:31;
	} bitFields;
} RegProtDbEntity1ProtIrq_u;

/*REG_PROT_DB_ENTITY1_PROT_CLEAR_IRQ 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtClearIrqStb:1;	// entity1 prot clear interrupt
		uint32 reserved0:31;
	} bitFields;
} RegProtDbEntity1ProtClearIrq_u;

/*REG_PROT_DB_ENTITY1_PROT_BUSY 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtBusy:1;	// entity1 prot busy indication
		uint32 reserved0:31;
	} bitFields;
} RegProtDbEntity1ProtBusy_u;

/*REG_PROT_DB_PROT_DB_DEBUG 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 protWrSmcWordsCount:5;	// prot wr smc words count
		uint32 reserved0:3;
		uint32 protDbSm:2;	// Protected DB state machine
		uint32 reserved1:22;
	} bitFields;
} RegProtDbProtDbDebug_u;



#endif // _PROT_DB_REGS_H_
