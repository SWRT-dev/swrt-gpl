
/***********************************************************************************
File:				DmacWrapperRegs.h
Module:				dmacWrapper
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _DMAC_WRAPPER_REGS_H_
#define _DMAC_WRAPPER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define DMAC_WRAPPER_BASE_ADDRESS                             MEMORY_MAP_UNIT_38_BASE_ADDRESS
#define	REG_DMAC_WRAPPER_BA_REG                         (DMAC_WRAPPER_BASE_ADDRESS + 0x800)
#define	REG_DMAC_WRAPPER_CONFIG_REG                     (DMAC_WRAPPER_BASE_ADDRESS + 0x804)
#define	REG_DMAC_WRAPPER_EXTERNAL_REQ_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0x808)
#define	REG_DMAC_WRAPPER_ABORT_REG                      (DMAC_WRAPPER_BASE_ADDRESS + 0x80C)
#define	REG_DMAC_WRAPPER_MISC_REG                       (DMAC_WRAPPER_BASE_ADDRESS + 0x810)
#define	REG_DMAC_WRAPPER_JOB_PUSH_WHILE_FULL_CLR_IND    (DMAC_WRAPPER_BASE_ADDRESS + 0x814)
#define	REG_DMAC_WRAPPER_STAT_IND_REG                   (DMAC_WRAPPER_BASE_ADDRESS + 0x820)
#define	REG_DMAC_WRAPPER_SENDER_ALIGNER_EN              (DMAC_WRAPPER_BASE_ADDRESS + 0x824)
#define	REG_DMAC_WRAPPER_AGENT_0_INT_MASK_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA00)
#define	REG_DMAC_WRAPPER_AGENT_0_PEND_INT_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA04)
#define	REG_DMAC_WRAPPER_AGENT_0_INT_CLEAR_REG          (DMAC_WRAPPER_BASE_ADDRESS + 0xA08)
#define	REG_DMAC_WRAPPER_AGENT_1_INT_MASK_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA10)
#define	REG_DMAC_WRAPPER_AGENT_1_PEND_INT_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA14)
#define	REG_DMAC_WRAPPER_AGENT_1_INT_CLEAR_REG          (DMAC_WRAPPER_BASE_ADDRESS + 0xA18)
#define	REG_DMAC_WRAPPER_AGENT_2_INT_MASK_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA20)
#define	REG_DMAC_WRAPPER_AGENT_2_PEND_INT_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA24)
#define	REG_DMAC_WRAPPER_AGENT_2_INT_CLEAR_REG          (DMAC_WRAPPER_BASE_ADDRESS + 0xA28)
#define	REG_DMAC_WRAPPER_AGENT_3_INT_MASK_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA30)
#define	REG_DMAC_WRAPPER_AGENT_3_PEND_INT_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA34)
#define	REG_DMAC_WRAPPER_AGENT_3_INT_CLEAR_REG          (DMAC_WRAPPER_BASE_ADDRESS + 0xA38)
#define	REG_DMAC_WRAPPER_AGENT_4_INT_MASK_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA40)
#define	REG_DMAC_WRAPPER_AGENT_4_PEND_INT_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA44)
#define	REG_DMAC_WRAPPER_AGENT_4_INT_CLEAR_REG          (DMAC_WRAPPER_BASE_ADDRESS + 0xA48)
#define	REG_DMAC_WRAPPER_AGENT_5_INT_MASK_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA50)
#define	REG_DMAC_WRAPPER_AGENT_5_PEND_INT_REG           (DMAC_WRAPPER_BASE_ADDRESS + 0xA54)
#define	REG_DMAC_WRAPPER_AGENT_5_INT_CLEAR_REG          (DMAC_WRAPPER_BASE_ADDRESS + 0xA58)
#define	REG_DMAC_WRAPPER_CH1_JOB_DONE_LAST_ACC          (DMAC_WRAPPER_BASE_ADDRESS + 0xB00)
#define	REG_DMAC_WRAPPER_CH1_JOB_DONE_LAST_ACC_DEC      (DMAC_WRAPPER_BASE_ADDRESS + 0xB04)
#define	REG_DMAC_WRAPPER_CH1_JOB_DONE_LAST_ACC_CLR      (DMAC_WRAPPER_BASE_ADDRESS + 0xB08)
#define	REG_DMAC_WRAPPER_CH0_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC00)
#define	REG_DMAC_WRAPPER_CH0_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xC04)
#define	REG_DMAC_WRAPPER_CH0_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC08)
#define	REG_DMAC_WRAPPER_CH0_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xC0C)
#define	REG_DMAC_WRAPPER_CH1_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC10)
#define	REG_DMAC_WRAPPER_CH1_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xC14)
#define	REG_DMAC_WRAPPER_CH1_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC18)
#define	REG_DMAC_WRAPPER_CH1_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xC1C)
#define	REG_DMAC_WRAPPER_CH2_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC20)
#define	REG_DMAC_WRAPPER_CH2_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xC24)
#define	REG_DMAC_WRAPPER_CH2_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC28)
#define	REG_DMAC_WRAPPER_CH2_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xC2C)
#define	REG_DMAC_WRAPPER_CH3_CONFIG_REG                 (DMAC_WRAPPER_BASE_ADDRESS + 0xC30)
#define	REG_DMAC_WRAPPER_CH3_FIFO_PUSH_SRC_ADDR         (DMAC_WRAPPER_BASE_ADDRESS + 0xC34)
#define	REG_DMAC_WRAPPER_CH3_FIFO_PUSH_DEST_ADDR        (DMAC_WRAPPER_BASE_ADDRESS + 0xC38)
#define	REG_DMAC_WRAPPER_CH3_FIFO_PUSH_C_REG            (DMAC_WRAPPER_BASE_ADDRESS + 0xC3C)
#define	REG_DMAC_WRAPPER_CH0_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF00)
#define	REG_DMAC_WRAPPER_CH0_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF04)
#define	REG_DMAC_WRAPPER_CH0_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF08)
#define	REG_DMAC_WRAPPER_CH1_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF10)
#define	REG_DMAC_WRAPPER_CH1_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF14)
#define	REG_DMAC_WRAPPER_CH1_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF18)
#define	REG_DMAC_WRAPPER_CH2_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF20)
#define	REG_DMAC_WRAPPER_CH2_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF24)
#define	REG_DMAC_WRAPPER_CH2_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF28)
#define	REG_DMAC_WRAPPER_CH3_JOB_CNT_REG                (DMAC_WRAPPER_BASE_ADDRESS + 0xF30)
#define	REG_DMAC_WRAPPER_CH3_BATCH_CNT_REG              (DMAC_WRAPPER_BASE_ADDRESS + 0xF34)
#define	REG_DMAC_WRAPPER_CH3_STAT_IND_REG               (DMAC_WRAPPER_BASE_ADDRESS + 0xF38)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_DMAC_WRAPPER_BA_REG 0x800 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 memBa:16;	// base address bits[31:16] of ram where dmac wrapper's data structure reside 
	} bitFields;
} RegDmacWrapperBaReg_u;

/*REG_DMAC_WRAPPER_CONFIG_REG 0x804 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 chManipBa:16;	// offset to base of manipulation lists [units: 64b]
		uint32 chCodeAlloc:8;	// dmac microcode block size per channel [units: 64b]
		uint32 reserved0:8;
	} bitFields;
} RegDmacWrapperConfigReg_u;

/*REG_DMAC_WRAPPER_EXTERNAL_REQ_REG 0x808 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 extStartReq:1;	// Request a DMAGO
		uint32 extStopReq:1;	// Request a DMAKILL
		uint32 extSigReq:1;	// Request a DMASEV
		uint32 reserved0:1;
		uint32 extReqArg:28;	// External request argument (channel# or signal#)
	} bitFields;
} RegDmacWrapperExternalReqReg_u;

/*REG_DMAC_WRAPPER_ABORT_REG 0x80C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:8;
		uint32 fifoClearReq:4;	// fifo clear
		uint32 reserved1:20;
	} bitFields;
} RegDmacWrapperAbortReg_u;

/*REG_DMAC_WRAPPER_MISC_REG 0x810 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 monHdrOffset:6;	// header monitor: offset to last header byte
		uint32 reserved0:2;
		uint32 monChSel:3;	// header monitor: selection of monitorred channel
		uint32 monHdrEn:1;	// header monitor enable
		uint32 lastJobIdChSel:3;	// selects which channel is connected to the "last job id" interrupts
		uint32 reserved1:1;
		uint32 txBufferFullMargin:10;	// Tx buffer full margin
		uint32 wordAlign:1;	// Word align mode: 32-bit aligned transfers would be done as 32-bit transfers (not bursts of 8-bit transfers)
		uint32 reserved2:5;
	} bitFields;
} RegDmacWrapperMiscReg_u;

/*REG_DMAC_WRAPPER_JOB_PUSH_WHILE_FULL_CLR_IND 0x814 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 clearPushWhileFullInd:4;
		uint32 reserved0:28;
	} bitFields;
} RegDmacWrapperJobPushWhileFullClrInd_u;

/*REG_DMAC_WRAPPER_STAT_IND_REG 0x820 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 statusDmacInterrupts:8;	// status dmac interrupts
		uint32 statusWrapLastJobIndicationsTog:8;	// status wrapper last job indications (toggling)
		uint32 statusJobFifoFullIndications:8;	// status job fifo full indications
		uint32 statusManipulationSm:5;	// status manipulation state machine
		uint32 statusManipulationSelectedChannel:2;	// status manipulation selected channel
		uint32 statusDmacAbortInterrupt:1;	// status dmac abort interrupt
	} bitFields;
} RegDmacWrapperStatIndReg_u;

/*REG_DMAC_WRAPPER_SENDER_ALIGNER_EN 0x824 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderAlignerEn:1;
		uint32 reserved0:31;
	} bitFields;
} RegDmacWrapperSenderAlignerEn_u;

/*REG_DMAC_WRAPPER_AGENT_0_INT_MASK_REG 0xA00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent0DmacBits4To7IntEn:4;	// agent 0 (GenriscTX) enable interrupts DMAC bits[7:4]
		uint32 agent0DmacAbortIntEn:1;	// agent 0 (GenriscTX) enable interrupts DMAC abort
		uint32 agent0RsvdIntEn:3;	// agent 0 (GenriscTX) enable interrupts Reserved
		uint32 agent0LastJobIdIntEn:4;	// agent 0 (GenriscTX) enable interrupts "last job id"
		uint32 agent0LastJobDoneIntEn:4;	// agent 0 (GenriscTX) enable interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent0IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_0_PEND_INT_REG 0xA04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent0DmacBits4To7PendInt:4;	// agent 0 (GenriscTX) pending interrupts DMAC bits[7:4]
		uint32 agent0DmacAbortPendInt:1;	// agent 0 (GenriscTX) pending interrupts DMAC abort
		uint32 agent0RsvdPendInt:3;	// agent 0 (GenriscTX) pending interrupts Reserved
		uint32 agent0LastJobIdPendInt:4;	// agent 0 (GenriscTX) pending interrupts "last job id"
		uint32 agent0LastJobDonePendInt:4;	// agent 0 (GenriscTX) pending interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent0PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_0_INT_CLEAR_REG 0xA08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent0DmacBits4To7IntClear:4;	// agent 0 (GenriscTX) interrupt clear DMAC bits[7:4]
		uint32 agent0DmacAbortIntClear:1;	// agent 0 (GenriscTX) interrupt clear DMAC abort
		uint32 agent0RsvdIntClear:3;	// agent 0 (GenriscTX) interrupt clear Reserved
		uint32 agent0LastJobIdIntClear:4;	// agent 0 (GenriscTX) interrupt clear "last job id"
		uint32 agent0LastJobDoneIntClear:4;	// agent 0 (GenriscTX) interrupt clear "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent0IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_1_INT_MASK_REG 0xA10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent1DmacBits4To7IntEn:4;	// agent 1 (GenriscRX) enable interrupts DMAC bits[7:4]
		uint32 agent1DmacAbortIntEn:1;	// agent 1 (GenriscRX) enable interrupts DMAC abort
		uint32 agent1RsvdIntEn:3;	// agent 1 (GenriscRX) enable interrupts Reserved
		uint32 agent1LastJobIdIntEn:4;	// agent 1 (GenriscRX) enable interrupts "last job id"
		uint32 agent1LastJobDoneIntEn:4;	// agent 1 (GenriscRX) enable interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent1IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_1_PEND_INT_REG 0xA14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent1DmacBits4To7PendInt:4;	// agent 1 (GenriscRX) pending interrupts DMAC bits[7:4]
		uint32 agent1DmacAbortPendInt:1;	// agent 1 (GenriscRX) pending interrupts DMAC abort
		uint32 agent1RsvdPendInt:3;	// agent 1 (GenriscRX) pending interrupts Reserved
		uint32 agent1LastJobIdPendInt:4;	// agent 1 (GenriscRX) pending interrupts "last job id"
		uint32 agent1LastJobDonePendInt:4;	// agent 1 (GenriscRX) pending interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent1PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_1_INT_CLEAR_REG 0xA18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent1DmacBits4To7IntClear:4;	// agent 1 (GenriscRX) interrupt clear DMAC bits[7:4]
		uint32 agent1DmacAbortIntClear:1;	// agent 1 (GenriscRX) interrupt clear DMAC abort
		uint32 agent1RsvdIntClear:3;	// agent 1 (GenriscRX) interrupt clear Reserved
		uint32 agent1LastJobIdIntClear:4;	// agent 1 (GenriscRX) interrupt clear "last job id"
		uint32 agent1LastJobDoneIntClear:4;	// agent 1 (GenriscRX) interrupt clear "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent1IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_2_INT_MASK_REG 0xA20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent2DmacBits4To7IntEn:4;	// agent 2 (GenriscHost) enable interrupts DMAC bits[7:4]
		uint32 agent2DmacAbortIntEn:1;	// agent 2 (GenriscHost) enable interrupts DMAC abort
		uint32 agent2RsvdIntEn:3;	// agent 2 (GenriscHost) enable interrupts Reserved
		uint32 agent2LastJobIdIntEn:4;	// agent 2 (GenriscHost) enable interrupts "last job id"
		uint32 agent2LastJobDoneIntEn:4;	// agent 2 (GenriscHost) enable interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent2IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_2_PEND_INT_REG 0xA24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent2DmacBits4To7PendInt:4;	// agent 2 (GenriscHost) pending interrupts DMAC bits[7:4]
		uint32 agent2DmacAbortPendInt:1;	// agent 2 (GenriscHost) pending interrupts DMAC abort
		uint32 agent2RsvdPendInt:3;	// agent 2 (GenriscHost) pending interrupts Reserved
		uint32 agent2LastJobIdPendInt:4;	// agent 2 (GenriscHost) pending interrupts "last job id"
		uint32 agent2LastJobDonePendInt:4;	// agent 2 (GenriscHost) pending interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent2PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_2_INT_CLEAR_REG 0xA28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent2DmacBits4To7IntClear:4;	// agent 2 (GenriscHost) interrupt clear DMAC bits[7:4]
		uint32 agent2DmacAbortIntClear:1;	// agent 2 (GenriscHost) interrupt clear DMAC abort
		uint32 agent2RsvdIntClear:3;	// agent 2 (GenriscHost) interrupt clear Reserved
		uint32 agent2LastJobIdIntClear:4;	// agent 2 (GenriscHost) interrupt clear "last job id"
		uint32 agent2LastJobDoneIntClear:4;	// agent 2 (GenriscHost) interrupt clear "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent2IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_3_INT_MASK_REG 0xA30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent3DmacBits4To7IntEn:4;	// agent 3 (Mips) enable interrupts DMAC bits[7:4]
		uint32 agent3DmacAbortIntEn:1;	// agent 3 (Mips) enable interrupts DMAC abort
		uint32 agent3RsvdIntEn:3;	// agent 3 (Mips) enable interrupts Reserved
		uint32 agent3LastJobIdIntEn:4;	// agent 3 (Mips) enable interrupts "last job id"
		uint32 agent3LastJobDoneIntEn:4;	// agent 3 (Mips) enable interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent3IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_3_PEND_INT_REG 0xA34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent3DmacBits4To7PendInt:4;	// agent 3 (MIPS) pending interrupts DMAC bits[7:4]
		uint32 agent3DmacAbortPendInt:1;	// agent 3 (MIPS) pending interrupts DMAC abort
		uint32 agent3RsvdPendInt:3;	// agent 3 (MIPS) pending interrupts Reserved
		uint32 agent3LastJobIdPendInt:4;	// agent 3 (MIPS) pending interrupts "last job id"
		uint32 agent3LastJobDonePendInt:4;	// agent 3 (MIPS) pending interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent3PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_3_INT_CLEAR_REG 0xA38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent3DmacBits4To7IntClear:4;	// agent 3 (MIPS) interrupt clear DMAC bits[7:4]
		uint32 agent3DmacAbortIntClear:1;	// agent 3 (MIPS) interrupt clear DMAC abort
		uint32 agent3RsvdIntClear:3;	// agent 3 (MIPS) interrupt clear Reserved
		uint32 agent3LastJobIdIntClear:4;	// agent 3 (MIPS) interrupt clear "last job id"
		uint32 agent3LastJobDoneIntClear:4;	// agent 3 (MIPS) interrupt clear "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent3IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_4_INT_MASK_REG 0xA40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent4DmacBits4To7IntEn:4;	// agent 4 enable interrupts DMAC bits[7:4]
		uint32 agent4DmacAbortIntEn:1;	// agent 4 enable interrupts DMAC abort
		uint32 agent4RsvdIntEn:3;	// agent 4 enable interrupts Reserved
		uint32 agent4LastJobIdIntEn:4;	// agent 4 enable interrupts "last job id"
		uint32 agent4LastJobDoneIntEn:4;	// agent 4 enable interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent4IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_4_PEND_INT_REG 0xA44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent4DmacBits4To7PendInt:4;	// agent 4 pending interrupts DMAC bits[7:4]
		uint32 agent4DmacAbortPendInt:1;	// agent 4 pending interrupts DMAC abort
		uint32 agent4RsvdPendInt:3;	// agent 4 pending interrupts Reserved
		uint32 agent4LastJobIdPendInt:4;	// agent 4 pending interrupts "last job id"
		uint32 agent4LastJobDonePendInt:4;	// agent 4 pending interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent4PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_4_INT_CLEAR_REG 0xA48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent4DmacBits4To7IntClear:4;	// agent 4 interrupt clear DMAC bits[7:4]
		uint32 agent4DmacAbortIntClear:1;	// agent 4 interrupt clear DMAC abort
		uint32 agent4RsvdIntClear:3;	// agent 4 interrupt clear Reserved
		uint32 agent4LastJobIdIntClear:4;	// agent 4 interrupt clear "last job id"
		uint32 agent4LastJobDoneIntClear:4;	// agent 4 interrupt clear "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent4IntClearReg_u;

/*REG_DMAC_WRAPPER_AGENT_5_INT_MASK_REG 0xA50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent5DmacBits4To7IntEn:4;	// agent 5 enable interrupts DMAC bits[7:4]
		uint32 agent5DmacAbortIntEn:1;	// agent 5 enable interrupts DMAC abort
		uint32 agent5RsvdIntEn:3;	// agent 5 enable interrupts Reserved
		uint32 agent5LastJobIdIntEn:4;	// agent 5 enable interrupts "last job id"
		uint32 agent5LastJobDoneIntEn:4;	// agent 5 enable interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent5IntMaskReg_u;

/*REG_DMAC_WRAPPER_AGENT_5_PEND_INT_REG 0xA54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent5DmacBits4To7PendInt:4;	// agent 5 pending interrupts DMAC bits[7:4]
		uint32 agent5DmacAbortPendInt:1;	// agent 5 pending interrupts DMAC abort
		uint32 agent5RsvdPendInt:3;	// agent 5 pending interrupts Reserved
		uint32 agent5LastJobIdPendInt:4;	// agent 5 pending interrupts "last job id"
		uint32 agent5LastJobDonePendInt:4;	// agent 5 pending interrupts "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent5PendIntReg_u;

/*REG_DMAC_WRAPPER_AGENT_5_INT_CLEAR_REG 0xA58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 agent5DmacBits4To7IntClear:4;	// agent 5 interrupt clear DMAC bits[7:4]
		uint32 agent5DmacAbortIntClear:1;	// agent 5 interrupt clear DMAC abort
		uint32 agent5RsvdIntClear:3;	// agent 5 interrupt clear Reserved
		uint32 agent5LastJobIdIntClear:4;	// agent 5 interrupt clear "last job id"
		uint32 agent5LastJobDoneIntClear:4;	// agent 5 interrupt clear "last job is done"
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperAgent5IntClearReg_u;

/*REG_DMAC_WRAPPER_CH1_JOB_DONE_LAST_ACC 0xB00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1JobDoneLastAcc:8;	// Accumulator of job done last of Channel1
		uint32 reserved0:23;
		uint32 ch1JobDoneLastAccExceed:1;	// Accumulator of job done last of Channel1 exceed the maximum value indication (or decrement below the zero) – Clear by write to ch1_job_done_last_acc_clr
	} bitFields;
} RegDmacWrapperCh1JobDoneLastAcc_u;

/*REG_DMAC_WRAPPER_CH1_JOB_DONE_LAST_ACC_DEC 0xB04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1JobDoneLastAccDec:1;	// Write 0x1 - Decrement by one the accumulator of job done last of Channel1 
		uint32 reserved0:31;
	} bitFields;
} RegDmacWrapperCh1JobDoneLastAccDec_u;

/*REG_DMAC_WRAPPER_CH1_JOB_DONE_LAST_ACC_CLR 0xB08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1JobDoneLastAccClr:1;	// Write 0x1 - Clear the accumulator of job done last of Channel1 
		uint32 reserved0:31;
	} bitFields;
} RegDmacWrapperCh1JobDoneLastAccClr_u;

/*REG_DMAC_WRAPPER_CH0_CONFIG_REG 0xC00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch0Priority:2;	// ch0 priority (0=top)
		uint32 ch0EndianSwap:2;	// ch0 endian swap (0=none;1=within shorts;2=within words;3=within doublewords)
		uint32 ch0ExtFcEnable:1;	// ch0 external flow control enable
		uint32 ch0TxbufFcEnable:1;	// ch0 tx buf flow control enable
		uint32 ch0WmbMode:1;	// ch0 wmb enable
		uint32 ch0Opt:1;	// ch0 enable load optimizations
		uint32 ch0PrgLen:8;	// ch0 size of each program
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperCh0ConfigReg_u;

/*REG_DMAC_WRAPPER_CH0_FIFO_PUSH_SRC_ADDR 0xC04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch0FifoPushSrcAddr:32;	// ch0 fifo push source address
	} bitFields;
} RegDmacWrapperCh0FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_CH0_FIFO_PUSH_DEST_ADDR 0xC08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch0FifoPushDestAddr:32;	// ch0 fifo push destination address
	} bitFields;
} RegDmacWrapperCh0FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_CH0_FIFO_PUSH_C_REG 0xC0C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch0FifoPushLength:15;	// ch0 fifo push length
		uint32 reserved0:1;
		uint32 ch0FifoPushEnd:1;	// ch0 fifo push end
		uint32 ch0FifoPushLast:1;	// ch0 fifo push last
		uint32 ch0FifoPushIdentifier:2;	// ch0 fifo push identifier
		uint32 reserved1:5;
		uint32 ch0FifoPushLastClrFc:1;	// ch0 fifo push last clear flow control
		uint32 reserved2:6;
	} bitFields;
} RegDmacWrapperCh0FifoPushCReg_u;

/*REG_DMAC_WRAPPER_CH1_CONFIG_REG 0xC10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1Priority:2;	// ch1 priority (0=top)
		uint32 ch1EndianSwap:2;	// ch1 endian swap (0=none;1=within shorts;2=within words;3=within doublewords)
		uint32 ch1ExtFcEnable:1;	// ch1 external flow control enable
		uint32 ch1TxbufFcEnable:1;	// ch1 tx buf flow control enable
		uint32 ch1WmbMode:1;	// ch1 wmb enable
		uint32 ch1Opt:1;	// ch1 enable load optimizations
		uint32 ch1PrgLen:8;	// ch1 size of each program
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperCh1ConfigReg_u;

/*REG_DMAC_WRAPPER_CH1_FIFO_PUSH_SRC_ADDR 0xC14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1FifoPushSrcAddr:32;	// ch1 fifo push source address
	} bitFields;
} RegDmacWrapperCh1FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_CH1_FIFO_PUSH_DEST_ADDR 0xC18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1FifoPushDestAddr:32;	// ch1 fifo push destination address
	} bitFields;
} RegDmacWrapperCh1FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_CH1_FIFO_PUSH_C_REG 0xC1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1FifoPushLength:15;	// ch1 fifo push length
		uint32 reserved0:1;
		uint32 ch1FifoPushEnd:1;	// ch1 fifo push end
		uint32 ch1FifoPushLast:1;	// ch1 fifo push last
		uint32 ch1FifoPushIdentifier:2;	// ch1 fifo push identifier
		uint32 reserved1:4;
		uint32 ch1FifoPushSkip:1;	// ch1 fifo skip
		uint32 reserved2:7;
	} bitFields;
} RegDmacWrapperCh1FifoPushCReg_u;

/*REG_DMAC_WRAPPER_CH2_CONFIG_REG 0xC20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch2Priority:2;	// ch2 priority (0=top)
		uint32 ch2EndianSwap:2;	// ch2 endian swap (0=none;1=within shorts;2=within words;3=within doublewords)
		uint32 ch2ExtFcEnable:1;	// ch2 external flow control enable
		uint32 ch2TxbufFcEnable:1;	// ch2 tx buf flow control enable
		uint32 ch2WmbMode:1;	// ch2 wmb enable
		uint32 ch2Opt:1;	// ch2 enable load optimizations
		uint32 ch2PrgLen:8;	// ch2 size of each program
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperCh2ConfigReg_u;

/*REG_DMAC_WRAPPER_CH2_FIFO_PUSH_SRC_ADDR 0xC24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch2FifoPushSrcAddr:32;	// ch2 fifo push source address
	} bitFields;
} RegDmacWrapperCh2FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_CH2_FIFO_PUSH_DEST_ADDR 0xC28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch2FifoPushDestAddr:32;	// ch2 fifo push destination address
	} bitFields;
} RegDmacWrapperCh2FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_CH2_FIFO_PUSH_C_REG 0xC2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch2FifoPushLength:15;	// ch2 fifo push length
		uint32 reserved0:1;
		uint32 ch2FifoPushEnd:1;	// ch2 fifo push end
		uint32 ch2FifoPushLast:1;	// ch2 fifo push last
		uint32 ch2FifoPushIdentifier:3;	// ch2 fifo push identifier
		uint32 reserved1:11;
	} bitFields;
} RegDmacWrapperCh2FifoPushCReg_u;

/*REG_DMAC_WRAPPER_CH3_CONFIG_REG 0xC30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch3Priority:2;	// ch3 priority (0=top)
		uint32 ch3EndianSwap:2;	// ch3 endian swap (0=none;1=within shorts;2=within words;3=within doublewords)
		uint32 ch3ExtFcEnable:1;	// ch3 external flow control enable
		uint32 ch3TxbufFcEnable:1;	// ch3 tx buf flow control enable
		uint32 ch3WmbMode:1;	// ch3 wmb enable
		uint32 ch3Opt:1;	// ch3 enable load optimizations
		uint32 ch3PrgLen:8;	// ch3 size of each program
		uint32 reserved0:16;
	} bitFields;
} RegDmacWrapperCh3ConfigReg_u;

/*REG_DMAC_WRAPPER_CH3_FIFO_PUSH_SRC_ADDR 0xC34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch3FifoPushSrcAddr:32;	// ch3 fifo push source address
	} bitFields;
} RegDmacWrapperCh3FifoPushSrcAddr_u;

/*REG_DMAC_WRAPPER_CH3_FIFO_PUSH_DEST_ADDR 0xC38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch3FifoPushDestAddr:32;	// ch3 fifo push destination address
	} bitFields;
} RegDmacWrapperCh3FifoPushDestAddr_u;

/*REG_DMAC_WRAPPER_CH3_FIFO_PUSH_C_REG 0xC3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch3FifoPushLength:15;	// ch3 fifo push length
		uint32 reserved0:1;
		uint32 ch3FifoPushEnd:1;	// ch3 fifo push end
		uint32 ch3FifoPushLast:1;	// ch3 fifo push last
		uint32 ch3FifoPushIdentifier:2;	// ch3 fifo push identifier
		uint32 reserved1:12;
	} bitFields;
} RegDmacWrapperCh3FifoPushCReg_u;

/*REG_DMAC_WRAPPER_CH0_JOB_CNT_REG 0xF00 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch0JobCnt:10;	// ch0 jobs counter
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh0JobCntReg_u;

/*REG_DMAC_WRAPPER_CH0_BATCH_CNT_REG 0xF04 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch0BatchCnt:10;	// ch0 batch counter (cleared on read)
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh0BatchCntReg_u;

/*REG_DMAC_WRAPPER_CH0_STAT_IND_REG 0xF08 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch0StatusJobFifoCount:8;	// ch0 status job fifo count
		uint32 ch0StatusJobFifoEmpty:1;	// ch0 status job fifo empty
		uint32 ch0StatusJobFifoFull:1;	// ch0 status job fifo full
		uint32 ch0StatusLastFifoPushIgnored:1;	// ch0 status last fifo push ignored
		uint32 ch0StatusLastJobIndicationTog:1;	// ch0 status last job indication (toggling)
		uint32 ch0StatusLastIdJob:3;	// ch0 status last id job
		uint32 ch0StatusFirstProgramReady:1;	// ch0 status first program ready
		uint32 ch0StatusSecondProgramReady:1;	// ch0 status second program ready
		uint32 ch0StatusChannelControllerSm:3;	// ch0 status channel controller state machine
		uint32 ch0StatusAbortSm:2;	// ch0 status abort state machine
		uint32 ch0StatusIsIdle:1;	// ch0 status channel controller is idle indication
		uint32 ch0StatusIsDead:1;	// ch0 status channel controller is dead indication
		uint32 ch0StatusDmaChannelActive:1;	// ch0 status DMAC channel active indication
		uint32 reserved0:7;
	} bitFields;
} RegDmacWrapperCh0StatIndReg_u;

/*REG_DMAC_WRAPPER_CH1_JOB_CNT_REG 0xF10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1JobCnt:10;	// ch1 jobs counter
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh1JobCntReg_u;

/*REG_DMAC_WRAPPER_CH1_BATCH_CNT_REG 0xF14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1BatchCnt:10;	// ch1 batch counter (cleared on read)
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh1BatchCntReg_u;

/*REG_DMAC_WRAPPER_CH1_STAT_IND_REG 0xF18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch1StatusJobFifoCount:8;	// ch1 status job fifo count
		uint32 ch1StatusJobFifoEmpty:1;	// ch1 status job fifo empty
		uint32 ch1StatusJobFifoFull:1;	// ch1 status job fifo full
		uint32 ch1StatusLastFifoPushIgnored:1;	// ch1 status last fifo push ignored
		uint32 ch1StatusLastJobIndicationTog:1;	// ch1 status last job indication (toggling)
		uint32 ch1StatusLastIdJob:3;	// ch1 status last id job
		uint32 ch1StatusFirstProgramReady:1;	// ch1 status first program ready
		uint32 ch1StatusSecondProgramReady:1;	// ch1 status second program ready
		uint32 ch1StatusChannelControllerSm:3;	// ch1 status channel controller state machine
		uint32 ch1StatusAbortSm:2;	// ch1 status abort state machine
		uint32 ch1StatusIsIdle:1;	// ch1 status channel controller is idle indication
		uint32 ch1StatusIsDead:1;	// ch1 status channel controller is dead indication
		uint32 ch1StatusDmaChannelActive:1;	// ch1 status DMAC channel active indication
		uint32 reserved0:7;
	} bitFields;
} RegDmacWrapperCh1StatIndReg_u;

/*REG_DMAC_WRAPPER_CH2_JOB_CNT_REG 0xF20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch2JobCnt:10;	// ch2 jobs counter
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh2JobCntReg_u;

/*REG_DMAC_WRAPPER_CH2_BATCH_CNT_REG 0xF24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch2BatchCnt:10;	// ch2 batch counter (cleared on read)
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh2BatchCntReg_u;

/*REG_DMAC_WRAPPER_CH2_STAT_IND_REG 0xF28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch2StatusJobFifoCount:8;	// ch2 status job fifo count
		uint32 ch2StatusJobFifoEmpty:1;	// ch2 status job fifo empty
		uint32 ch2StatusJobFifoFull:1;	// ch2 status job fifo full
		uint32 ch2StatusLastFifoPushIgnored:1;	// ch2 status last fifo push ignored
		uint32 ch2StatusLastJobIndicationTog:1;	// ch2 status last job indication (toggling)
		uint32 ch2StatusLastIdJob:3;	// ch2 status last id job
		uint32 ch2StatusFirstProgramReady:1;	// ch2 status first program ready
		uint32 ch2StatusSecondProgramReady:1;	// ch2 status second program ready
		uint32 ch2StatusChannelControllerSm:3;	// ch2 status channel controller state machine
		uint32 ch2StatusAbortSm:2;	// ch2 status abort state machine
		uint32 ch2StatusIsIdle:1;	// ch2 status channel controller is idle indication
		uint32 ch2StatusIsDead:1;	// ch2 status channel controller is dead indication
		uint32 ch2StatusDmaChannelActive:1;	// ch2 status DMAC channel active indication
		uint32 reserved0:7;
	} bitFields;
} RegDmacWrapperCh2StatIndReg_u;

/*REG_DMAC_WRAPPER_CH3_JOB_CNT_REG 0xF30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch3JobCnt:10;	// ch3 jobs counter
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh3JobCntReg_u;

/*REG_DMAC_WRAPPER_CH3_BATCH_CNT_REG 0xF34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch3BatchCnt:10;	// ch3 batch counter (cleared on read)
		uint32 reserved0:22;
	} bitFields;
} RegDmacWrapperCh3BatchCntReg_u;

/*REG_DMAC_WRAPPER_CH3_STAT_IND_REG 0xF38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ch3StatusJobFifoCount:8;	// ch3 status job fifo count
		uint32 ch3StatusJobFifoEmpty:1;	// ch3 status job fifo empty
		uint32 ch3StatusJobFifoFull:1;	// ch3 status job fifo full
		uint32 ch3StatusLastFifoPushIgnored:1;	// ch3 status last fifo push ignored
		uint32 ch3StatusLastJobIndicationTog:1;	// ch3 status last job indication (toggling)
		uint32 ch3StatusLastIdJob:3;	// ch3 status last id job
		uint32 ch3StatusFirstProgramReady:1;	// ch3 status first program ready
		uint32 ch3StatusSecondProgramReady:1;	// ch3 status second program ready
		uint32 ch3StatusChannelControllerSm:3;	// ch3 status channel controller state machine
		uint32 ch3StatusAbortSm:2;	// ch3 status abort state machine
		uint32 ch3StatusIsIdle:1;	// ch3 status channel controller is idle indication
		uint32 ch3StatusIsDead:1;	// ch3 status channel controller is dead indication
		uint32 ch3StatusDmaChannelActive:1;	// ch3 status DMAC channel active indication
		uint32 reserved0:7;
	} bitFields;
} RegDmacWrapperCh3StatIndReg_u;



#endif // _DMAC_WRAPPER_REGS_H_
