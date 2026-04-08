
/***********************************************************************************
File:				PacDelRegs.h
Module:				pacDel
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _PAC_DEL_REGS_H_
#define _PAC_DEL_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define PAC_DEL_BASE_ADDRESS                             MEMORY_MAP_UNIT_8_BASE_ADDRESS
#define	REG_PAC_DEL_BASE_0                                 (PAC_DEL_BASE_ADDRESS + 0x0)
#define	REG_PAC_DEL_BASE_1                                 (PAC_DEL_BASE_ADDRESS + 0x4)
#define	REG_PAC_DEL_BASE_2                                 (PAC_DEL_BASE_ADDRESS + 0x8)
#define	REG_PAC_DEL_BASE_3                                 (PAC_DEL_BASE_ADDRESS + 0xC)
#define	REG_PAC_DEL_BASE_4                                 (PAC_DEL_BASE_ADDRESS + 0x10)
#define	REG_PAC_DEL_BASE_5                                 (PAC_DEL_BASE_ADDRESS + 0x14)
#define	REG_PAC_DEL_BASE_6                                 (PAC_DEL_BASE_ADDRESS + 0x18)
#define	REG_PAC_DEL_RETURN_ADDRESS                         (PAC_DEL_BASE_ADDRESS + 0x1C)
#define	REG_PAC_DEL_RECIPE_POINTER                         (PAC_DEL_BASE_ADDRESS + 0x20)
#define	REG_PAC_DEL_DELIA_STATUS                           (PAC_DEL_BASE_ADDRESS + 0x24)
#define	REG_PAC_DEL_IFS_COMPENSATION                       (PAC_DEL_BASE_ADDRESS + 0x28)
#define	REG_PAC_DEL_DELIA_CONTROL                          (PAC_DEL_BASE_ADDRESS + 0x30)
#define	REG_PAC_DEL_INDEXED_JUMP_ADDR                      (PAC_DEL_BASE_ADDRESS + 0x34)
#define	REG_PAC_DEL_MP_TX_DATA_WORD_COUNT                  (PAC_DEL_BASE_ADDRESS + 0x38)
#define	REG_PAC_DEL_DELIA_LAST_INST_ADDR                   (PAC_DEL_BASE_ADDRESS + 0x3C)
#define	REG_PAC_DEL_AUTO_REPLY_MODE                        (PAC_DEL_BASE_ADDRESS + 0x40)
#define	REG_PAC_DEL_RECIPE_POINTER_DEBUG_WR                (PAC_DEL_BASE_ADDRESS + 0x48)
#define	REG_PAC_DEL_TX_ADDR1_31TO0                         (PAC_DEL_BASE_ADDRESS + 0x4C)
#define	REG_PAC_DEL_TX_ADDR1_47TO32                        (PAC_DEL_BASE_ADDRESS + 0x50)
#define	REG_PAC_DEL_AUTO_FILL_ENABLE                       (PAC_DEL_BASE_ADDRESS + 0x54)
#define	REG_PAC_DEL_AUTO_FILL_PACKET_LENGTH                (PAC_DEL_BASE_ADDRESS + 0x58)
#define	REG_PAC_DEL_CPU_DEL_WR_EVENTS                      (PAC_DEL_BASE_ADDRESS + 0x5C)
#define	REG_PAC_DEL_CPU_DEL_RD_EVENTS                      (PAC_DEL_BASE_ADDRESS + 0x60)
#define	REG_PAC_DEL_CPU_DEL_WR_CNTRO_EVENT                 (PAC_DEL_BASE_ADDRESS + 0x64)
#define	REG_PAC_DEL_CPU_DEL_RD_CNTR0_EVENT                 (PAC_DEL_BASE_ADDRESS + 0x68)
#define	REG_PAC_DEL_CPU_DEL_WR_CNTR1_EVENT                 (PAC_DEL_BASE_ADDRESS + 0x6C)
#define	REG_PAC_DEL_CPU_DEL_RD_CNTR1_EVENT                 (PAC_DEL_BASE_ADDRESS + 0x70)
#define	REG_PAC_DEL_LAST_CPU_DEL_CNTR_EVENT                (PAC_DEL_BASE_ADDRESS + 0x74)
#define	REG_PAC_DEL_NOP_WAIT_EVENT_VECTOR_DBG              (PAC_DEL_BASE_ADDRESS + 0x78)
#define	REG_PAC_DEL_LAST_RD_ADDR_TRANSFER                  (PAC_DEL_BASE_ADDRESS + 0x7C)
#define	REG_PAC_DEL_MP_TX_TCR_BF_WORD_COUNT                (PAC_DEL_BASE_ADDRESS + 0x80)
#define	REG_PAC_DEL_MP_FIFO_DEBUG                          (PAC_DEL_BASE_ADDRESS + 0x84)
#define	REG_PAC_DEL_TX_DATA_STRUCTURE_ADDR                 (PAC_DEL_BASE_ADDRESS + 0x88)
#define	REG_PAC_DEL_EMU_PRBS_GCLK_BYPASS                   (PAC_DEL_BASE_ADDRESS + 0x8C)
#define	REG_PAC_DEL_BF_TX_MAX_THRESHOLD_TIMER              (PAC_DEL_BASE_ADDRESS + 0x90)
#define	REG_PAC_DEL_DEL2GENRISC_START_TX_INT               (PAC_DEL_BASE_ADDRESS + 0x94)
#define	REG_PAC_DEL_DEL2GENRISC_START_TX_CLR_INT           (PAC_DEL_BASE_ADDRESS + 0x98)
#define	REG_PAC_DEL_HOB_CNTR_EVENT_AUTO_FILL               (PAC_DEL_BASE_ADDRESS + 0x9C)
#define	REG_PAC_DEL_AUTO_FILL_VALUE                        (PAC_DEL_BASE_ADDRESS + 0xA0)
#define	REG_PAC_DEL_AUTO_FILL_CTR_LIMIT                    (PAC_DEL_BASE_ADDRESS + 0xA4)
#define	REG_PAC_DEL_AUTO_FILL_CTR                          (PAC_DEL_BASE_ADDRESS + 0xA8)
#define	REG_PAC_DEL_AUTO_FILL_SUBFRAMES_CTR                (PAC_DEL_BASE_ADDRESS + 0xAC)
#define	REG_PAC_DEL_AUTO_FILL_SUBFRAMES_STATUS_31TO0       (PAC_DEL_BASE_ADDRESS + 0xB0)
#define	REG_PAC_DEL_AUTO_FILL_SUBFRAMES_STATUS_63TO32      (PAC_DEL_BASE_ADDRESS + 0xB4)
#define	REG_PAC_DEL_AUTO_FILL_WR_CTR                       (PAC_DEL_BASE_ADDRESS + 0xB8)
#define	REG_PAC_DEL_DELIA_LOGGER_CONTROL                   (PAC_DEL_BASE_ADDRESS + 0xBC)
#define	REG_PAC_DEL_DELIA_LOGGER_ACTIVE                    (PAC_DEL_BASE_ADDRESS + 0xC0)
#define	REG_PAC_DEL_TX_END_STATUS                          (PAC_DEL_BASE_ADDRESS + 0xC4)
#define	REG_PAC_DEL_TX_END_REMAIN_FIFO_IN_PACKET_LENGTH    (PAC_DEL_BASE_ADDRESS + 0xC8)
#define	REG_PAC_DEL_REMAIN_FIFO_OUT_PACKET_LENGTH          (PAC_DEL_BASE_ADDRESS + 0xCC)
#define	REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL0                (PAC_DEL_BASE_ADDRESS + 0xD0)
#define	REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL1                (PAC_DEL_BASE_ADDRESS + 0xD4)
#define	REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL2                (PAC_DEL_BASE_ADDRESS + 0xD8)
#define	REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL3                (PAC_DEL_BASE_ADDRESS + 0xDC)
#define	REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL4                (PAC_DEL_BASE_ADDRESS + 0xE0)
#define	REG_PAC_DEL_DEL2GENRISC_START_TX_ENABLE_INT        (PAC_DEL_BASE_ADDRESS + 0xE4)
#define	REG_PAC_DEL_CPU_DEL_EVENTS_COMMON_EN               (PAC_DEL_BASE_ADDRESS + 0xE8)
#define	REG_PAC_DEL_DELIMITER_VALUE_PRE_TX_DATA            (PAC_DEL_BASE_ADDRESS + 0xEC)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_PAC_DEL_BASE_0 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 base0:32;	// DEL  base 0
	} bitFields;
} RegPacDelBase0_u;

/*REG_PAC_DEL_BASE_1 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 base1:32;	// DEL  base 1
	} bitFields;
} RegPacDelBase1_u;

/*REG_PAC_DEL_BASE_2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 base2:32;	// DEL  base 2
	} bitFields;
} RegPacDelBase2_u;

/*REG_PAC_DEL_BASE_3 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 base3:32;	// DEL  base 3
	} bitFields;
} RegPacDelBase3_u;

/*REG_PAC_DEL_BASE_4 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 base4:32;	// DEL  base 4
	} bitFields;
} RegPacDelBase4_u;

/*REG_PAC_DEL_BASE_5 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 base5:32;	// DEL  base 5
	} bitFields;
} RegPacDelBase5_u;

/*REG_PAC_DEL_BASE_6 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 base6:32;	// DEL  base 6
	} bitFields;
} RegPacDelBase6_u;

/*REG_PAC_DEL_RETURN_ADDRESS 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 returnAddress:21;	// DEL  return address
		uint32 reserved1:9;
	} bitFields;
} RegPacDelReturnAddress_u;

/*REG_PAC_DEL_RECIPE_POINTER 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 recipePointer:21;	// DEL  recipe pointer
		uint32 reserved1:9;
	} bitFields;
} RegPacDelRecipePointer_u;

/*REG_PAC_DEL_DELIA_STATUS 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaStatusBusy:1;	// DEL  delia status
		uint32 reserved0:3;
		uint32 deliaState:4;	// DEL  delia state
		uint32 reserved1:24;
	} bitFields;
} RegPacDelDeliaStatus_u;

/*REG_PAC_DEL_IFS_COMPENSATION 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 ifsCompensationOfdm:16;	// DEL ifs compensation when the last Tx PHY mode is OFDM
		uint32 ifsCompensation11B:16;	// DEL ifs compensation when the last Tx PHY mode is 11B
	} bitFields;
} RegPacDelIfsCompensation_u;

/*REG_PAC_DEL_DELIA_CONTROL 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyAutoStart:1;	// Phy auto start
		uint32 reserved0:31;
	} bitFields;
} RegPacDelDeliaControl_u;

/*REG_PAC_DEL_INDEXED_JUMP_ADDR 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 indexedJumpAddr:21;	// Indexed jump address
		uint32 reserved1:9;
	} bitFields;
} RegPacDelIndexedJumpAddr_u;

/*REG_PAC_DEL_MP_TX_DATA_WORD_COUNT 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpTxDataWordCtr:18;	// Tx data word count
		uint32 reserved0:14;
	} bitFields;
} RegPacDelMpTxDataWordCount_u;

/*REG_PAC_DEL_DELIA_LAST_INST_ADDR 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastFetchedInstruction:32;	// Last fetched instruction
	} bitFields;
} RegPacDelDeliaLastInstAddr_u;

/*REG_PAC_DEL_AUTO_REPLY_MODE 0x40 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoPowerMode:1;	// Auto Tx power mode for auto frames
		uint32 reserved0:31;
	} bitFields;
} RegPacDelAutoReplyMode_u;

/*REG_PAC_DEL_RECIPE_POINTER_DEBUG_WR 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:2;
		uint32 recipePointerDebugWr:21;	// DEL  write debug recipe pointer
		uint32 reserved1:9;
	} bitFields;
} RegPacDelRecipePointerDebugWr_u;

/*REG_PAC_DEL_TX_ADDR1_31TO0 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txAddr131To0:32;	// tx addr1 latch 31to0
	} bitFields;
} RegPacDelTxAddr131To0_u;

/*REG_PAC_DEL_TX_ADDR1_47TO32 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txAddr147To32:16;	// tx addr1 latch 47to32
		uint32 reserved0:16;
	} bitFields;
} RegPacDelTxAddr147To32_u;

/*REG_PAC_DEL_AUTO_FILL_ENABLE 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillEnable:1;	// auto fill enable
		uint32 reserved0:31;
	} bitFields;
} RegPacDelAutoFillEnable_u;

/*REG_PAC_DEL_AUTO_FILL_PACKET_LENGTH 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillPacketLength:20;	// auto fill packet length
		uint32 reserved0:12;
	} bitFields;
} RegPacDelAutoFillPacketLength_u;

/*REG_PAC_DEL_CPU_DEL_WR_EVENTS 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuDelWrEvents:6;	// CPU delia wr events
		uint32 reserved0:26;
	} bitFields;
} RegPacDelCpuDelWrEvents_u;

/*REG_PAC_DEL_CPU_DEL_RD_EVENTS 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuDelRdEvents:6;	// CPU delia rd events
		uint32 reserved0:26;
	} bitFields;
} RegPacDelCpuDelRdEvents_u;

/*REG_PAC_DEL_CPU_DEL_WR_CNTRO_EVENT 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuDelWrCntr0Event:12;	// CPU delia wr event counter0
		uint32 reserved0:20;
	} bitFields;
} RegPacDelCpuDelWrCntroEvent_u;

/*REG_PAC_DEL_CPU_DEL_RD_CNTR0_EVENT 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuDelRdCntr0Event:12;	// CPU delia rd event counter0
		uint32 reserved0:4;
		uint32 hobDelRdCntr0Event:12;	// HOB delia rd event counter0
		uint32 reserved1:4;
	} bitFields;
} RegPacDelCpuDelRdCntr0Event_u;

/*REG_PAC_DEL_CPU_DEL_WR_CNTR1_EVENT 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuDelWrCntr1Event:12;	// CPU delia wr event counter1
		uint32 reserved0:20;
	} bitFields;
} RegPacDelCpuDelWrCntr1Event_u;

/*REG_PAC_DEL_CPU_DEL_RD_CNTR1_EVENT 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuDelRdCntr1Event:12;	// CPU delia rd event counter1
		uint32 reserved0:4;
		uint32 hobDelRdCntr1Event:12;	// HOB delia rd event counter1
		uint32 reserved1:4;
	} bitFields;
} RegPacDelCpuDelRdCntr1Event_u;

/*REG_PAC_DEL_LAST_CPU_DEL_CNTR_EVENT 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastCpuDelCntr0Event:12;	// last CPU delia event counter0
		uint32 lastCpuDelCntr1Event:12;	// last CPU delia event counter1
		uint32 reserved0:8;
	} bitFields;
} RegPacDelLastCpuDelCntrEvent_u;

/*REG_PAC_DEL_NOP_WAIT_EVENT_VECTOR_DBG 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 nopWaitEventVector:16;	// nop wait event vector
		uint32 lastNopWaitEventVector:16;	// last nop wait event vector
	} bitFields;
} RegPacDelNopWaitEventVectorDbg_u;

/*REG_PAC_DEL_LAST_RD_ADDR_TRANSFER 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lastRdAddrTransfer:23;	// last rd ShRAM addr transfer
		uint32 reserved0:9;
	} bitFields;
} RegPacDelLastRdAddrTransfer_u;

/*REG_PAC_DEL_MP_TX_TCR_BF_WORD_COUNT 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpTxTcrWordCtr:3;	// Tx TCR word count
		uint32 reserved0:5;
		uint32 mpTxBfWordCtr:6;	// Tx BF word count
		uint32 reserved1:18;
	} bitFields;
} RegPacDelMpTxTcrBfWordCount_u;

/*REG_PAC_DEL_MP_FIFO_DEBUG 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 mpFifoFull:1;	// MP FIFO full
		uint32 mpFifoEmpty:1;	// MP FIFO empty
		uint32 autoFillLatch:1;	// Auto fill active latch
		uint32 mpDataDone:1;	// MP data done
		uint32 ovenAlign:2;	// Num bytes in Oven
		uint32 writeToUpperOven:1;	// next write to upper oven
		uint32 reserved0:1;
		uint32 mpFifoNumBytes:7;	// Num bytes in MP FIFO
		uint32 reserved1:1;
		uint32 mpFifoRdPtr:6;	// MP FIFO read pointer
		uint32 reserved2:2;
		uint32 mpFifoWrPtr:6;	// MP FIFO write pointer
		uint32 reserved3:2;
	} bitFields;
} RegPacDelMpFifoDebug_u;

/*REG_PAC_DEL_TX_DATA_STRUCTURE_ADDR 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txDataStructureAddr:24;	// Tx data structure addr
		uint32 reserved0:8;
	} bitFields;
} RegPacDelTxDataStructureAddr_u;

/*REG_PAC_DEL_EMU_PRBS_GCLK_BYPASS 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 emuPrbsGclkBypass:1;	// PRBS gclk bypass
		uint32 reserved0:31;
	} bitFields;
} RegPacDelEmuPrbsGclkBypass_u;

/*REG_PAC_DEL_BF_TX_MAX_THRESHOLD_TIMER 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bfTxMaxThresholdTimer11Ag:10;	// Maximum time (in resolution of 1/8 [us]) that Delia must start transmit BF matrix in PHY mode 11ag
		uint32 bfTxMaxThresholdTimer11N:10;	// Maximum time (in resolution of 1/8 [us]) that Delia must start transmit BF matrix in PHY mode 11n
		uint32 bfTxMaxThresholdTimer11Ac:10;	// Maximum time (in resolution of 1/8 [us]) that Delia must start transmit BF matrix in PHY mode 11ac
		uint32 reserved0:2;
	} bitFields;
} RegPacDelBfTxMaxThresholdTimer_u;

/*REG_PAC_DEL_DEL2GENRISC_START_TX_INT 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 del2GenriscStartTxInt:6;	// Delia to GenRisc interrupts
		uint32 reserved0:26;
	} bitFields;
} RegPacDelDel2GenriscStartTxInt_u;

/*REG_PAC_DEL_DEL2GENRISC_START_TX_CLR_INT 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 del2GenriscStartTxClrInt:6;	// Delia to GenRisc clear interrupts
		uint32 reserved0:26;
	} bitFields;
} RegPacDelDel2GenriscStartTxClrInt_u;

/*REG_PAC_DEL_HOB_CNTR_EVENT_AUTO_FILL 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hobDelCntr0EventAutoFill:12;	// hob del cntr0 event auto fill latch
		uint32 reserved0:4;
		uint32 hobDelCntr1EventAutoFill:12;	// hob del cntr1 event auto fill latch
		uint32 reserved1:4;
	} bitFields;
} RegPacDelHobCntrEventAutoFill_u;

/*REG_PAC_DEL_AUTO_FILL_VALUE 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillValue:32;	// Auto fill value
	} bitFields;
} RegPacDelAutoFillValue_u;

/*REG_PAC_DEL_AUTO_FILL_CTR_LIMIT 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillCtrLimit:18;	// Auto fill counter limit
		uint32 reserved0:14;
	} bitFields;
} RegPacDelAutoFillCtrLimit_u;

/*REG_PAC_DEL_AUTO_FILL_CTR 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillCtr:18;	// Auto fill counter
		uint32 reserved0:14;
	} bitFields;
} RegPacDelAutoFillCtr_u;

/*REG_PAC_DEL_AUTO_FILL_SUBFRAMES_CTR 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillSubframesCtr:7;	// Auto fill subframes counter
		uint32 reserved0:25;
	} bitFields;
} RegPacDelAutoFillSubframesCtr_u;

/*REG_PAC_DEL_AUTO_FILL_SUBFRAMES_STATUS_31TO0 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillSubframesStatus31To0:32;	// Auto fill subframes status 31to0
	} bitFields;
} RegPacDelAutoFillSubframesStatus31To0_u;

/*REG_PAC_DEL_AUTO_FILL_SUBFRAMES_STATUS_63TO32 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillSubframesStatus63To32:32;	// Auto fill subframes status 63to32
	} bitFields;
} RegPacDelAutoFillSubframesStatus63To32_u;

/*REG_PAC_DEL_AUTO_FILL_WR_CTR 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 autoFillWrCtr:18;	// Number of 8bytes words which filled by auto fill mechanism
		uint32 reserved0:14;
	} bitFields;
} RegPacDelAutoFillWrCtr_u;

/*REG_PAC_DEL_DELIA_LOGGER_CONTROL 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaLoggerEn:1;	// Delia logger enable
		uint32 reserved0:7;
		uint32 deliaLoggerPriority:2;	// Delia logger priority
		uint32 reserved1:22;
	} bitFields;
} RegPacDelDeliaLoggerControl_u;

/*REG_PAC_DEL_DELIA_LOGGER_ACTIVE 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 deliaLoggerActive:1;	// Delia logger active
		uint32 reserved0:31;
	} bitFields;
} RegPacDelDeliaLoggerActive_u;

/*REG_PAC_DEL_TX_END_STATUS 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txEndRemainFifoInPacketLengthNotZero:1;	// Tx end remaining FIFO in packet length not zero
		uint32 remainFifoOutPacketLengthNotZero:1;	// Remaining FIFO out packet length not zero
		uint32 txEndDeliaRecipeAborted:1;	// Tx end delia recipe aborted
		uint32 txEndAutoFillOccured:1;	// Tx end auto fill occured
		uint32 txEndAutoFillPayloadOccured:1;	// Tx end auto fill occurred only on the payload section
		uint32 fifoRdWhenRemainFifoOutZero:1;	// MAC-PHY FIFO read data when remain_fifo_out_packet_length equals zero
		uint32 fifoWrWhenRemainFifoInZero:1;	// MAC-PHY FIFO write data when remain_fifo_in_packet_length equals zero
		uint32 reserved0:25;
	} bitFields;
} RegPacDelTxEndStatus_u;

/*REG_PAC_DEL_TX_END_REMAIN_FIFO_IN_PACKET_LENGTH 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 txEndRemainFifoInPacketLength:20;	// Tx end remaining FIFO in packet length
		uint32 reserved0:4;
		uint32 mpFifoBytesInFifoWhenTxEnd:8;	// MAC-PHY FIFO number of words, sampled at Tx end 
	} bitFields;
} RegPacDelTxEndRemainFifoInPacketLength_u;

/*REG_PAC_DEL_REMAIN_FIFO_OUT_PACKET_LENGTH 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 remainFifoOutPacketLength:20;	// Remaining FIFO out packet length
		uint32 reserved0:12;
	} bitFields;
} RegPacDelRemainFifoOutPacketLength_u;

/*REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL0 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxPhyControl0:24;	// MAC-PHY Transmit control 0
		uint32 reserved0:8;
	} bitFields;
} RegPacDelPacPhyTxPhyControl0_u;

/*REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL1 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxPhyControl1:24;	// MAC-PHY Transmit control 1
		uint32 reserved0:8;
	} bitFields;
} RegPacDelPacPhyTxPhyControl1_u;

/*REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL2 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxPhyControl2:24;	// MAC-PHY Transmit control 2
		uint32 reserved0:8;
	} bitFields;
} RegPacDelPacPhyTxPhyControl2_u;

/*REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL3 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxPhyControl3:24;	// MAC-PHY Transmit control 3
		uint32 reserved0:8;
	} bitFields;
} RegPacDelPacPhyTxPhyControl3_u;

/*REG_PAC_DEL_PAC_PHY_TX_PHY_CONTROL4 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pacPhyTxPhyControl4:24;	// MAC-PHY Transmit control 4
		uint32 reserved0:8;
	} bitFields;
} RegPacDelPacPhyTxPhyControl4_u;

/*REG_PAC_DEL_DEL2GENRISC_START_TX_ENABLE_INT 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 del2GenriscStartTxEnableInt:6;	// Delia to GenRisc enable interrupts
		uint32 reserved0:26;
	} bitFields;
} RegPacDelDel2GenriscStartTxEnableInt_u;

/*REG_PAC_DEL_CPU_DEL_EVENTS_COMMON_EN 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 cpuDelEventsCommonEn:6;	// CPU delia events enable, 1 bit per event: , 1 - Use common events (OR on the events of the all active Delias). , 0 - Use per Delia events.
		uint32 reserved0:26;
	} bitFields;
} RegPacDelCpuDelEventsCommonEn_u;

/*REG_PAC_DEL_DELIMITER_VALUE_PRE_TX_DATA 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 delimiterValuePreTxData:32;	// Delimiter value in case need to transmit at the start of the PSDU,  , Can be read by HOB.
	} bitFields;
} RegPacDelDelimiterValuePreTxData_u;



#endif // _PAC_DEL_REGS_H_
