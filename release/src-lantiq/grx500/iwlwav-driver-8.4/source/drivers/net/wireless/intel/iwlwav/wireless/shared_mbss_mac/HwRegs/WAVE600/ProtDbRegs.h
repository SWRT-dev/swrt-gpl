
/***********************************************************************************
File:				ProtDbRegs.h
Module:				protDb
SOC Revision:		
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
#define	REG_PROT_DB_ENTITY0_PROT_SHADOW_MEM_16B_OFFSET    (PROT_DB_BASE_ADDRESS + 0x0)
#define	REG_PROT_DB_ENTITY0_PROT_START_COMMAND            (PROT_DB_BASE_ADDRESS + 0x4)
#define	REG_PROT_DB_ENTITY0_PROT_IRQ                      (PROT_DB_BASE_ADDRESS + 0x8)
#define	REG_PROT_DB_ENTITY0_PROT_CLEAR_IRQ                (PROT_DB_BASE_ADDRESS + 0xC)
#define	REG_PROT_DB_ENTITY0_PROT_BUSY                     (PROT_DB_BASE_ADDRESS + 0x10)
#define	REG_PROT_DB_ENTITY1_PROT_SHADOW_MEM_16B_OFFSET    (PROT_DB_BASE_ADDRESS + 0x20)
#define	REG_PROT_DB_ENTITY1_PROT_START_COMMAND            (PROT_DB_BASE_ADDRESS + 0x24)
#define	REG_PROT_DB_ENTITY1_PROT_IRQ                      (PROT_DB_BASE_ADDRESS + 0x28)
#define	REG_PROT_DB_ENTITY1_PROT_CLEAR_IRQ                (PROT_DB_BASE_ADDRESS + 0x2C)
#define	REG_PROT_DB_ENTITY1_PROT_BUSY                     (PROT_DB_BASE_ADDRESS + 0x30)
#define	REG_PROT_DB_ENTITY2_PROT_SHADOW_MEM_16B_OFFSET    (PROT_DB_BASE_ADDRESS + 0x40)
#define	REG_PROT_DB_ENTITY2_PROT_START_COMMAND            (PROT_DB_BASE_ADDRESS + 0x44)
#define	REG_PROT_DB_ENTITY2_PROT_IRQ                      (PROT_DB_BASE_ADDRESS + 0x48)
#define	REG_PROT_DB_ENTITY2_PROT_CLEAR_IRQ                (PROT_DB_BASE_ADDRESS + 0x4C)
#define	REG_PROT_DB_ENTITY2_PROT_BUSY                     (PROT_DB_BASE_ADDRESS + 0x50)
#define	REG_PROT_DB_PROT_DB_DEBUG                         (PROT_DB_BASE_ADDRESS + 0x70)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PROT_DB_ENTITY0_PROT_SHADOW_MEM_16B_OFFSET 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtShadowMem16BOffset : 5; //entity0 prot shadow mem 16Bytes offset, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegProtDbEntity0ProtShadowMem16BOffset_u;

/*REG_PROT_DB_ENTITY0_PROT_START_COMMAND 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 entity0ProtDestBaseAddr : 20; //entity0 prot destination base address, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 entity0ProtWordsNum : 8; //entity0 prot words num, reset value: 0x0, access type: RW
	} bitFields;
} RegProtDbEntity0ProtStartCommand_u;

/*REG_PROT_DB_ENTITY0_PROT_IRQ 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtIrq : 1; //entity0 prot interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity0ProtIrq_u;

/*REG_PROT_DB_ENTITY0_PROT_CLEAR_IRQ 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtClearIrqStb : 1; //entity0 prot clear interrupt, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity0ProtClearIrq_u;

/*REG_PROT_DB_ENTITY0_PROT_BUSY 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity0ProtBusy : 1; //entity0 prot busy indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity0ProtBusy_u;

/*REG_PROT_DB_ENTITY1_PROT_SHADOW_MEM_16B_OFFSET 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtShadowMem16BOffset : 5; //entity1 prot shadow mem 16Bytes offset, reset value: 0x8, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegProtDbEntity1ProtShadowMem16BOffset_u;

/*REG_PROT_DB_ENTITY1_PROT_START_COMMAND 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 entity1ProtDestBaseAddr : 20; //entity1 prot destination base address, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 entity1ProtWordsNum : 8; //entity1 prot words num, reset value: 0x0, access type: RW
	} bitFields;
} RegProtDbEntity1ProtStartCommand_u;

/*REG_PROT_DB_ENTITY1_PROT_IRQ 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtIrq : 1; //entity1 prot interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity1ProtIrq_u;

/*REG_PROT_DB_ENTITY1_PROT_CLEAR_IRQ 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtClearIrqStb : 1; //entity1 prot clear interrupt, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity1ProtClearIrq_u;

/*REG_PROT_DB_ENTITY1_PROT_BUSY 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity1ProtBusy : 1; //entity1 prot busy indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity1ProtBusy_u;

/*REG_PROT_DB_ENTITY2_PROT_SHADOW_MEM_16B_OFFSET 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity2ProtShadowMem16BOffset : 5; //entity2 prot shadow mem 16Bytes offset, reset value: 0x10, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegProtDbEntity2ProtShadowMem16BOffset_u;

/*REG_PROT_DB_ENTITY2_PROT_START_COMMAND 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 2;
		uint32 entity2ProtDestBaseAddr : 20; //entity2 prot destination base address, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 entity2ProtWordsNum : 8; //entity2 prot words num, reset value: 0x0, access type: RW
	} bitFields;
} RegProtDbEntity2ProtStartCommand_u;

/*REG_PROT_DB_ENTITY2_PROT_IRQ 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity2ProtIrq : 1; //entity2 prot interrupt, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity2ProtIrq_u;

/*REG_PROT_DB_ENTITY2_PROT_CLEAR_IRQ 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity2ProtClearIrqStb : 1; //entity2 prot clear interrupt, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity2ProtClearIrq_u;

/*REG_PROT_DB_ENTITY2_PROT_BUSY 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 entity2ProtBusy : 1; //entity2 prot busy indication, reset value: 0x0, access type: RO
		uint32 reserved0 : 31;
	} bitFields;
} RegProtDbEntity2ProtBusy_u;

/*REG_PROT_DB_PROT_DB_DEBUG 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 protWrSmcWordsCount : 8; //prot wr smc words count, reset value: 0x0, access type: RO
		uint32 protDbSm : 2; //Protected DB state machine, reset value: 0x0, access type: RO
		uint32 reserved0 : 22;
	} bitFields;
} RegProtDbProtDbDebug_u;



#endif // _PROT_DB_REGS_H_
