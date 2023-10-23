
/***********************************************************************************
File:				HobPacTimRegs.h
Module:				hobPacTim
SOC Revision:		
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _HOB_PAC_TIM_REGS_H_
#define _HOB_PAC_TIM_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define HOB_PAC_TIM_BASE_ADDRESS                             MEMORY_MAP_UNIT_502_BASE_ADDRESS
#define	REG_HOB_PAC_TIM_LATCHED_TST_FIME_LOW     (HOB_PAC_TIM_BASE_ADDRESS + 0x0)
#define	REG_HOB_PAC_TIM_LATCHED_TST_FIME_HIGH    (HOB_PAC_TIM_BASE_ADDRESS + 0x4)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_HOB_PAC_TIM_LATCHED_TST_FIME_LOW 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latchedTstFimeLow : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobPacTimLatchedTstFimeLow_u;

/*REG_HOB_PAC_TIM_LATCHED_TST_FIME_HIGH 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 latchedTstFimeHigh : 32; //no description, reset value: 0x0, access type: RO
	} bitFields;
} RegHobPacTimLatchedTstFimeHigh_u;



#endif // _HOB_PAC_TIM_REGS_H_
