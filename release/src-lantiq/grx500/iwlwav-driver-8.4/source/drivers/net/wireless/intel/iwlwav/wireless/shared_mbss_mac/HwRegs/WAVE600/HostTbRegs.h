
/***********************************************************************************
File:				HostTbRegs.h
Module:				hostTb
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HOST_TB_REGS_H_
#define _HOST_TB_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HOST_TB_BASE_ADDRESS                             MEMORY_MAP_UNIT_3_BASE_ADDRESS
#define	REG_HOST_TB_CONST_VAL_REG_ADDR    (HOST_TB_BASE_ADDRESS + 0x0)
#define	REG_HOST_TB_INIT_REG_ADDR         (HOST_TB_BASE_ADDRESS + 0x4)
#define	REG_HOST_TB_SEED_REG_ADDR         (HOST_TB_BASE_ADDRESS + 0x8)
#define	REG_HOST_TB_SHIFT_NUM_REG_ADDR    (HOST_TB_BASE_ADDRESS + 0xC)
#define	REG_HOST_TB_ECW_REG_ADDR          (HOST_TB_BASE_ADDRESS + 0x10)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HOST_TB_CONST_VAL_REG_ADDR 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 constRdVal : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 15;
		uint32 constWrVal : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved1 : 7;
	} bitFields;
} RegHostTbConstValRegAddr_u;

/*REG_HOST_TB_INIT_REG_ADDR 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 initBlock : 1; //no description, reset value: 0x0, access type: WO
		uint32 enPrbs : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 30;
	} bitFields;
} RegHostTbInitRegAddr_u;

/*REG_HOST_TB_SEED_REG_ADDR 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 seed : 32; //no description, reset value: 0x12345678, access type: RW
	} bitFields;
} RegHostTbSeedRegAddr_u;

/*REG_HOST_TB_SHIFT_NUM_REG_ADDR 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shiftNum : 5; //no description, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegHostTbShiftNumRegAddr_u;

/*REG_HOST_TB_ECW_REG_ADDR 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ecw : 4; //no description, reset value: 0x5, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegHostTbEcwRegAddr_u;



#endif // _HOST_TB_REGS_H_
