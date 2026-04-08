
/***********************************************************************************
File:				TxPdAccRegs.h
Module:				txPdAcc
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _TX_PD_ACC_REGS_H_
#define _TX_PD_ACC_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define TX_PD_ACC_BASE_ADDRESS                             MEMORY_MAP_UNIT_76_BASE_ADDRESS
#define	REG_TX_PD_ACC_TX_PD_ACC_ENABLE               (TX_PD_ACC_BASE_ADDRESS + 0x0)
#define	REG_TX_PD_ACC_TX_PD_ACC_PTD_CONTROL          (TX_PD_ACC_BASE_ADDRESS + 0x4)
#define	REG_TX_PD_ACC_TX_PD_ACC_START                (TX_PD_ACC_BASE_ADDRESS + 0x8)
#define	REG_TX_PD_ACC_USP0_PTD2PD_COUNTER            (TX_PD_ACC_BASE_ADDRESS + 0x10)
#define	REG_TX_PD_ACC_USP1_PTD2PD_COUNTER            (TX_PD_ACC_BASE_ADDRESS + 0x14)
#define	REG_TX_PD_ACC_USP2_PTD2PD_COUNTER            (TX_PD_ACC_BASE_ADDRESS + 0x18)
#define	REG_TX_PD_ACC_USP3_PTD2PD_COUNTER            (TX_PD_ACC_BASE_ADDRESS + 0x1C)
#define	REG_TX_PD_ACC_USP0_CURR_SEQUENCE_NUMBER      (TX_PD_ACC_BASE_ADDRESS + 0x20)
#define	REG_TX_PD_ACC_USP1_CURR_SEQUENCE_NUMBER      (TX_PD_ACC_BASE_ADDRESS + 0x24)
#define	REG_TX_PD_ACC_USP2_CURR_SEQUENCE_NUMBER      (TX_PD_ACC_BASE_ADDRESS + 0x28)
#define	REG_TX_PD_ACC_USP3_CURR_SEQUENCE_NUMBER      (TX_PD_ACC_BASE_ADDRESS + 0x2C)
#define	REG_TX_PD_ACC_USP0_CURR_PACKET_NUMBER_1_0    (TX_PD_ACC_BASE_ADDRESS + 0x30)
#define	REG_TX_PD_ACC_USP0_CURR_PACKET_NUMBER_5_2    (TX_PD_ACC_BASE_ADDRESS + 0x34)
#define	REG_TX_PD_ACC_USP1_CURR_PACKET_NUMBER_1_0    (TX_PD_ACC_BASE_ADDRESS + 0x38)
#define	REG_TX_PD_ACC_USP1_CURR_PACKET_NUMBER_5_2    (TX_PD_ACC_BASE_ADDRESS + 0x3C)
#define	REG_TX_PD_ACC_USP2_CURR_PACKET_NUMBER_1_0    (TX_PD_ACC_BASE_ADDRESS + 0x40)
#define	REG_TX_PD_ACC_USP2_CURR_PACKET_NUMBER_5_2    (TX_PD_ACC_BASE_ADDRESS + 0x44)
#define	REG_TX_PD_ACC_USP3_CURR_PACKET_NUMBER_1_0    (TX_PD_ACC_BASE_ADDRESS + 0x48)
#define	REG_TX_PD_ACC_USP3_CURR_PACKET_NUMBER_5_2    (TX_PD_ACC_BASE_ADDRESS + 0x4C)
#define	REG_TX_PD_ACC_PD2PTD_MPDU_INDEX              (TX_PD_ACC_BASE_ADDRESS + 0x50)
#define	REG_TX_PD_ACC_PTD_DEBUG                      (TX_PD_ACC_BASE_ADDRESS + 0x54)
#define	REG_TX_PD_ACC_ACCUMULATOR_DEBUG              (TX_PD_ACC_BASE_ADDRESS + 0x58)
#define	REG_TX_PD_ACC_TX_PD_ACC_SM                   (TX_PD_ACC_BASE_ADDRESS + 0x5C)
#define	REG_TX_PD_ACC_PENDING_PTD_ACC_DEC_NUM        (TX_PD_ACC_BASE_ADDRESS + 0x60)
#define	REG_TX_PD_ACC_RELEASE_PTD_ACC_ADD_NUM        (TX_PD_ACC_BASE_ADDRESS + 0x64)
#define	REG_TX_PD_ACC_PTD2PD_MPDU_COUNT              (TX_PD_ACC_BASE_ADDRESS + 0x68)
#define	REG_TX_PD_ACC_TX_PD_ACC_LOGGER_CONTROL       (TX_PD_ACC_BASE_ADDRESS + 0x6C)
#define	REG_TX_PD_ACC_TX_PD_ACC_LOGGER_ACTIVE        (TX_PD_ACC_BASE_ADDRESS + 0x70)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_TX_PD_ACC_TX_PD_ACC_ENABLE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccEnable:1;	// Tx PD accelerator enable
		uint32 manipulateRetryFieldsEn:1;	// Enable manipulate retry bit and retry count
		uint32 reserved0:30;
	} bitFields;
} RegTxPdAccTxPdAccEnable_u;

/*REG_TX_PD_ACC_TX_PD_ACC_PTD_CONTROL 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 maxPtdsMinusOne:6;	// Max PTDs minnus one
		uint32 reserved0:2;
		uint32 ptdWordSize:4;	// PTD word size
		uint32 reserved1:20;
	} bitFields;
} RegTxPdAccTxPdAccPtdControl_u;

/*REG_TX_PD_ACC_TX_PD_ACC_START 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccStartStrb:1;	// Tx PD accelerator start strobe, by wite '1'
		uint32 pendingPtdAccClrStrb:1;	// Pending PTD accumulator clear strobe, by write '1'
		uint32 occupiedPtdAccClrStrb:1;	// Occupied PTD accumulator clear strobe, by write '1'
		uint32 releasePtdAccClrStrb:1;	// Release PTD accumulator clear strobe, by write '1'
		uint32 skipMiddlePdInAMsduEn:1;	// Enable Skip middle PDs in A-MSDU
		uint32 ptd2PdUseOccupiedAccNotEmptyEn:1;	// Enable PTD to PD module to use occupied accumulator not empty indication instead of release accumulator
		uint32 reserved0:26;
	} bitFields;
} RegTxPdAccTxPdAccStart_u;

/*REG_TX_PD_ACC_USP0_PTD2PD_COUNTER 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp0Ptd2PdCounter:16;	// Total PTD to PD counter for USP0
		uint32 reserved0:16;
	} bitFields;
} RegTxPdAccUsp0Ptd2PdCounter_u;

/*REG_TX_PD_ACC_USP1_PTD2PD_COUNTER 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp1Ptd2PdCounter:16;	// Total PTD to PD counter for USP1
		uint32 reserved0:16;
	} bitFields;
} RegTxPdAccUsp1Ptd2PdCounter_u;

/*REG_TX_PD_ACC_USP2_PTD2PD_COUNTER 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp2Ptd2PdCounter:16;	// Total PTD to PD counter for USP2
		uint32 reserved0:16;
	} bitFields;
} RegTxPdAccUsp2Ptd2PdCounter_u;

/*REG_TX_PD_ACC_USP3_PTD2PD_COUNTER 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp3Ptd2PdCounter:16;	// Total PTD to PD counter for USP3
		uint32 reserved0:16;
	} bitFields;
} RegTxPdAccUsp3Ptd2PdCounter_u;

/*REG_TX_PD_ACC_USP0_CURR_SEQUENCE_NUMBER 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp0CurrSequenceNumber:12;	// Current Sequence Number for USP0
		uint32 reserved0:20;
	} bitFields;
} RegTxPdAccUsp0CurrSequenceNumber_u;

/*REG_TX_PD_ACC_USP1_CURR_SEQUENCE_NUMBER 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp1CurrSequenceNumber:12;	// Current Sequence Number for USP1
		uint32 reserved0:20;
	} bitFields;
} RegTxPdAccUsp1CurrSequenceNumber_u;

/*REG_TX_PD_ACC_USP2_CURR_SEQUENCE_NUMBER 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp2CurrSequenceNumber:12;	// Current Sequence Number for USP2
		uint32 reserved0:20;
	} bitFields;
} RegTxPdAccUsp2CurrSequenceNumber_u;

/*REG_TX_PD_ACC_USP3_CURR_SEQUENCE_NUMBER 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp3CurrSequenceNumber:12;	// Current Sequence Number for USP3
		uint32 reserved0:20;
	} bitFields;
} RegTxPdAccUsp3CurrSequenceNumber_u;

/*REG_TX_PD_ACC_USP0_CURR_PACKET_NUMBER_1_0 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 usp0CurrPacketNumber10:16;	// Current packet number bytes 1:0 for USP0
	} bitFields;
} RegTxPdAccUsp0CurrPacketNumber10_u;

/*REG_TX_PD_ACC_USP0_CURR_PACKET_NUMBER_5_2 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp0CurrPacketNumber52:32;	// Current packet number bytes 5:2 for USP0
	} bitFields;
} RegTxPdAccUsp0CurrPacketNumber52_u;

/*REG_TX_PD_ACC_USP1_CURR_PACKET_NUMBER_1_0 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 usp1CurrPacketNumber10:16;	// Current packet number bytes 1:0 for USP1
	} bitFields;
} RegTxPdAccUsp1CurrPacketNumber10_u;

/*REG_TX_PD_ACC_USP1_CURR_PACKET_NUMBER_5_2 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp1CurrPacketNumber52:32;	// Current packet number bytes 5:2 for USP1
	} bitFields;
} RegTxPdAccUsp1CurrPacketNumber52_u;

/*REG_TX_PD_ACC_USP2_CURR_PACKET_NUMBER_1_0 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 usp2CurrPacketNumber10:16;	// Current packet number bytes 1:0 for USP2
	} bitFields;
} RegTxPdAccUsp2CurrPacketNumber10_u;

/*REG_TX_PD_ACC_USP2_CURR_PACKET_NUMBER_5_2 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp2CurrPacketNumber52:32;	// Current packet number bytes 5:2 for USP2
	} bitFields;
} RegTxPdAccUsp2CurrPacketNumber52_u;

/*REG_TX_PD_ACC_USP3_CURR_PACKET_NUMBER_1_0 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 usp3CurrPacketNumber10:16;	// Current packet number bytes 1:0 for USP3
	} bitFields;
} RegTxPdAccUsp3CurrPacketNumber10_u;

/*REG_TX_PD_ACC_USP3_CURR_PACKET_NUMBER_5_2 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp3CurrPacketNumber52:32;	// Current packet number bytes 5:2 for USP3
	} bitFields;
} RegTxPdAccUsp3CurrPacketNumber52_u;

/*REG_TX_PD_ACC_PD2PTD_MPDU_INDEX 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp0Pd2PtdMpduIndex:7;	// MPDU index of USP0 in PD to PTD module
		uint32 reserved0:1;
		uint32 usp1Pd2PtdMpduIndex:7;	// MPDU index of USP1 in PD to PTD module
		uint32 reserved1:1;
		uint32 usp2Pd2PtdMpduIndex:7;	// MPDU index of USP2 in PD to PTD module
		uint32 reserved2:1;
		uint32 usp3Pd2PtdMpduIndex:7;	// MPDU index of USP3 in PD to PTD module
		uint32 reserved3:1;
	} bitFields;
} RegTxPdAccPd2PtdMpduIndex_u;

/*REG_TX_PD_ACC_PTD_DEBUG 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rdPtdIndex:6;	// Read PTD index
		uint32 wrPtdIndex:6;	// Write PTD index
		uint32 reserved0:20;
	} bitFields;
} RegTxPdAccPtdDebug_u;

/*REG_TX_PD_ACC_ACCUMULATOR_DEBUG 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingPtdAccCount:7;	// Accumulator of number pending PTDs
		uint32 reserved0:1;
		uint32 occupiedPtdAccCount:7;	// Accumulator of number occupied PTDs
		uint32 reserved1:1;
		uint32 releasePtdAccCount:7;	// Accumulator of number release PTDs
		uint32 reserved2:9;
	} bitFields;
} RegTxPdAccAccumulatorDebug_u;

/*REG_TX_PD_ACC_TX_PD_ACC_SM 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp0TxPdAccCoreSm:5;	// Tx PD accelerator core state machine
		uint32 usp1TxPdAccCoreSm:5;	// Tx PD accelerator core state machine
		uint32 usp2TxPdAccCoreSm:5;	// Tx PD accelerator core state machine
		uint32 usp3TxPdAccCoreSm:5;	// Tx PD accelerator core state machine
		uint32 txPdAccMainSm:3;	// Tx PD accelerator main state machine
		uint32 txPdAccPtd2PdSm:4;	// Tx PD accelerator PTD to PD state machine
		uint32 reserved0:5;
	} bitFields;
} RegTxPdAccTxPdAccSm_u;

/*REG_TX_PD_ACC_PENDING_PTD_ACC_DEC_NUM 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pendingPtdAccDecNum:7;	// Number pending PTDs to decrement
		uint32 reserved0:25;
	} bitFields;
} RegTxPdAccPendingPtdAccDecNum_u;

/*REG_TX_PD_ACC_RELEASE_PTD_ACC_ADD_NUM 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 releasePtdAccAddNum:7;	// Number release PTDs to add
		uint32 reserved0:25;
	} bitFields;
} RegTxPdAccReleasePtdAccAddNum_u;

/*REG_TX_PD_ACC_PTD2PD_MPDU_COUNT 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 usp0Ptd2PdMpduCount:7;	// Number of MPDUs copied by PTD to PD module for USP0
		uint32 reserved0:1;
		uint32 usp1Ptd2PdMpduCount:7;	// Number of MPDUs copied by PTD to PD module for USP1
		uint32 reserved1:1;
		uint32 usp2Ptd2PdMpduCount:7;	// Number of MPDUs copied by PTD to PD module for USP2
		uint32 reserved2:1;
		uint32 usp3Ptd2PdMpduCount:7;	// Number of MPDUs copied by PTD to PD module for USP3
		uint32 reserved3:1;
	} bitFields;
} RegTxPdAccPtd2PdMpduCount_u;

/*REG_TX_PD_ACC_TX_PD_ACC_LOGGER_CONTROL 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccLoggerEn:1;	// Tx PD accelerator logger enable
		uint32 txPdAccLoggerPriority:2;	// Tx PD accelerator logger priority
		uint32 txPdAccLoggerNumMsgMinusOne:4;	// Tx PD accelerator logger number of messages minus one in a report
		uint32 reserved0:25;
	} bitFields;
} RegTxPdAccTxPdAccLoggerControl_u;

/*REG_TX_PD_ACC_TX_PD_ACC_LOGGER_ACTIVE 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txPdAccLoggerActive:1;	// Tx PD accelerator logger active
		uint32 reserved0:31;
	} bitFields;
} RegTxPdAccTxPdAccLoggerActive_u;



#endif // _TX_PD_ACC_REGS_H_
