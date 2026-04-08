
/***********************************************************************************
File:				Modem11BFeRegs.h
Module:				modem11BFe
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _MODEM_11B_FE_REGS_H_
#define _MODEM_11B_FE_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define MODEM_11B_FE_BASE_ADDRESS                             MEMORY_MAP_UNIT_52_BASE_ADDRESS
#define	REG_MODEM_11B_FE_FRONTENDCTRL    (MODEM_11B_FE_BASE_ADDRESS + 0xC0E0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_MODEM_11B_FE_FRONTENDCTRL 0xC0E0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 intDcdisb:1;	// disable the dc offset estimation 
		uint32 intScaldisb:1;	// disable scaling
		uint32 intMultscaldisb:1;	// disable amplifier
		uint32 intMultscal:7;	// setting the amplifier's values of scaling_mult and scaling_shift , 
		uint32 intDcesttime:1;	// DC estimation time selection , 
		uint32 reserved0:21;
	} bitFields;
} RegModem11BFeFrontendctrl_u;



#endif // _MODEM_11B_FE_REGS_H_
