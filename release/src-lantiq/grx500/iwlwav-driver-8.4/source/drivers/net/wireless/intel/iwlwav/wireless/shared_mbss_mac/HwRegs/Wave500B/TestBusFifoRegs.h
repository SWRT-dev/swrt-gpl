
/***********************************************************************************
File:				TestBusFifoRegs.h
Module:				testBusFifo
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TEST_BUS_FIFO_REGS_H_
#define _TEST_BUS_FIFO_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TEST_BUS_FIFO_BASE_ADDRESS                             MEMORY_MAP_UNIT_50_BASE_ADDRESS
#define	REG_TEST_BUS_FIFO_PHY_RXTD_FIFO0_0000    (TEST_BUS_FIFO_BASE_ADDRESS + 0x38000)
#define	REG_TEST_BUS_FIFO_PHY_RXTD_FIFO0_00FF    (TEST_BUS_FIFO_BASE_ADDRESS + 0x383FC)
#define	REG_TEST_BUS_FIFO_PHY_RXTD_FIFO1_0000    (TEST_BUS_FIFO_BASE_ADDRESS + 0x38400)
#define	REG_TEST_BUS_FIFO_PHY_RXTD_FIFO1_00FF    (TEST_BUS_FIFO_BASE_ADDRESS + 0x387FC)
#define	REG_TEST_BUS_FIFO_PHY_RXTD_FIFO2_0000    (TEST_BUS_FIFO_BASE_ADDRESS + 0x38800)
#define	REG_TEST_BUS_FIFO_PHY_RXTD_FIFO2_00FF    (TEST_BUS_FIFO_BASE_ADDRESS + 0x38BFC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TEST_BUS_FIFO_PHY_RXTD_FIFO0_0000 0x38000 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo0StartAddr:20;	// Rx Td fifo 0 Start Address
		uint32 reserved0:12;
	} bitFields;
} RegTestBusFifoPhyRxtdFifo00000_u;

/*REG_TEST_BUS_FIFO_PHY_RXTD_FIFO0_00FF 0x383FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo0EndAddr:20;	// Rx Td fifo 0 End Address
		uint32 reserved0:12;
	} bitFields;
} RegTestBusFifoPhyRxtdFifo000Ff_u;

/*REG_TEST_BUS_FIFO_PHY_RXTD_FIFO1_0000 0x38400 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo1StartAddr:20;	// Rx Td fifo 1 Start Address
		uint32 reserved0:12;
	} bitFields;
} RegTestBusFifoPhyRxtdFifo10000_u;

/*REG_TEST_BUS_FIFO_PHY_RXTD_FIFO1_00FF 0x387FC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo1EndAddr:20;	// Rx Td fifo 1 End Address
		uint32 reserved0:12;
	} bitFields;
} RegTestBusFifoPhyRxtdFifo100Ff_u;

/*REG_TEST_BUS_FIFO_PHY_RXTD_FIFO2_0000 0x38800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo2StartAddr:20;	// Rx Td fifo 2 Start Address
		uint32 reserved0:12;
	} bitFields;
} RegTestBusFifoPhyRxtdFifo20000_u;

/*REG_TEST_BUS_FIFO_PHY_RXTD_FIFO2_00FF 0x38BFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 fifo2EndAddr:20;	// Rx Td fifo 2 End Address
		uint32 reserved0:12;
	} bitFields;
} RegTestBusFifoPhyRxtdFifo200Ff_u;



#endif // _TEST_BUS_FIFO_REGS_H_
