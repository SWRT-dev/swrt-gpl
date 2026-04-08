
/***********************************************************************************
File:				LoggerRegs.h
Module:				logger
SOC Revision:		843
Purpose:
Description:		This File was auto generated using SOC Online

************************************************************************************/
#ifndef _LOGGER_REGS_H_
#define _LOGGER_REGS_H_

/*---------------------------------------------------------------------------------
/						Registers Addresses													 
/----------------------------------------------------------------------------------*/
#include "HwMemoryMap.h"

#define LOGGER_BASE_ADDRESS                             MEMORY_MAP_UNIT_61_BASE_ADDRESS
#define	REG_LOGGER_MODE                                  (LOGGER_BASE_ADDRESS + 0x0)
#define	REG_LOGGER_RAM_PARAMETERS                        (LOGGER_BASE_ADDRESS + 0x4)
#define	REG_LOGGER_RAM_PARAMETERS_2                      (LOGGER_BASE_ADDRESS + 0x8)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST0       (LOGGER_BASE_ADDRESS + 0xC)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST1       (LOGGER_BASE_ADDRESS + 0x10)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST2       (LOGGER_BASE_ADDRESS + 0x14)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST3       (LOGGER_BASE_ADDRESS + 0x18)
#define	REG_LOGGER_NUM_MESSAGES_1                        (LOGGER_BASE_ADDRESS + 0x1C)
#define	REG_LOGGER_NUM_MESSAGES_2                        (LOGGER_BASE_ADDRESS + 0x20)
#define	REG_LOGGER_NUM_MESSAGES_3                        (LOGGER_BASE_ADDRESS + 0x24)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_1                   (LOGGER_BASE_ADDRESS + 0x28)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_2                   (LOGGER_BASE_ADDRESS + 0x2C)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_3                   (LOGGER_BASE_ADDRESS + 0x30)
#define	REG_LOGGER_UMAC_DATA_IN                          (LOGGER_BASE_ADDRESS + 0x44)
#define	REG_LOGGER_UMAC_FAST_MES_DATA_IN                 (LOGGER_BASE_ADDRESS + 0x48)
#define	REG_LOGGER_UMAC_MESAGE_DONE                      (LOGGER_BASE_ADDRESS + 0x4C)
#define	REG_LOGGER_LMAC_DATA_IN                          (LOGGER_BASE_ADDRESS + 0x50)
#define	REG_LOGGER_LMAC_FAST_MES_DATA_IN                 (LOGGER_BASE_ADDRESS + 0x54)
#define	REG_LOGGER_LMAC_MESAGE_DONE                      (LOGGER_BASE_ADDRESS + 0x58)
#define	REG_LOGGER_HOST_IF_DATA_IN                       (LOGGER_BASE_ADDRESS + 0x5C)
#define	REG_LOGGER_HOST_IF_FAST_MES_DATA_IN              (LOGGER_BASE_ADDRESS + 0x60)
#define	REG_LOGGER_HOST_IF_MESAGE_DONE                   (LOGGER_BASE_ADDRESS + 0x64)
#define	REG_LOGGER_RX_HANDLER_DATA_IN                    (LOGGER_BASE_ADDRESS + 0x68)
#define	REG_LOGGER_RX_HANDLER_FAST_MES_DATA_IN           (LOGGER_BASE_ADDRESS + 0x6C)
#define	REG_LOGGER_RX_HANDLER_MESAGE_DONE                (LOGGER_BASE_ADDRESS + 0x70)
#define	REG_LOGGER_SENDER_DATA_IN                        (LOGGER_BASE_ADDRESS + 0x74)
#define	REG_LOGGER_SENDER_FAST_MES_DATA_IN               (LOGGER_BASE_ADDRESS + 0x78)
#define	REG_LOGGER_SENDER_MESAGE_DONE                    (LOGGER_BASE_ADDRESS + 0x7C)
#define	REG_LOGGER_FIFO_ENABLE                           (LOGGER_BASE_ADDRESS + 0x80)
#define	REG_LOGGER_LIST_ID                               (LOGGER_BASE_ADDRESS + 0x84)
#define	REG_LOGGER_LIST_CONVERTER                        (LOGGER_BASE_ADDRESS + 0x88)
#define	REG_LOGGER_BUFFER_LENGHT                         (LOGGER_BASE_ADDRESS + 0x8C)
#define	REG_LOGGER_BUFFER0_1THR                          (LOGGER_BASE_ADDRESS + 0x90)
#define	REG_LOGGER_BUFFER2_3THR                          (LOGGER_BASE_ADDRESS + 0x94)
#define	REG_LOGGER_START_DATA_TRIGGER_LOW                (LOGGER_BASE_ADDRESS + 0x98)
#define	REG_LOGGER_START_DATA_TRIGGER_HIGH               (LOGGER_BASE_ADDRESS + 0x9C)
#define	REG_LOGGER_START_MASK_TRIGGER_LOW                (LOGGER_BASE_ADDRESS + 0xA0)
#define	REG_LOGGER_START_MASK_TRIGGER_HIGH               (LOGGER_BASE_ADDRESS + 0xA4)
#define	REG_LOGGER_STOP_DATA_TRIGGER_LOW                 (LOGGER_BASE_ADDRESS + 0xA8)
#define	REG_LOGGER_STOP_DATA_TRIGGER_HIGH                (LOGGER_BASE_ADDRESS + 0xAC)
#define	REG_LOGGER_STOP_MASK_TRIGGER_LOW                 (LOGGER_BASE_ADDRESS + 0xB0)
#define	REG_LOGGER_STOP_MASK_TRIGGER_HIGH                (LOGGER_BASE_ADDRESS + 0xB4)
#define	REG_LOGGER_TIME_TO_RECORDER_AFTER_STOP           (LOGGER_BASE_ADDRESS + 0xB8)
#define	REG_LOGGER_HW_UNIT1_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0xBC)
#define	REG_LOGGER_HW_UNIT2_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0xC0)
#define	REG_LOGGER_HW_UNIT3_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0xC4)
#define	REG_LOGGER_HW_UNIT4_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0xC8)
#define	REG_LOGGER_HW_UNIT5_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0xCC)
#define	REG_LOGGER_HW_UNIT6_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0xD0)
#define	REG_LOGGER_HW_UNIT1_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0xE0)
#define	REG_LOGGER_HW_UNIT3_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0xE4)
#define	REG_LOGGER_HW_UNIT5_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0xE8)
#define	REG_LOGGER_PHY_FIFO_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0xF0)
#define	REG_LOGGER_PHY_FIFO_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0xF4)
#define	REG_LOGGER_PHY_FIFO_MESAGE_LENGHT                (LOGGER_BASE_ADDRESS + 0xF8)
#define	REG_LOGGER_SHRAM_DIRECT_MODE                     (LOGGER_BASE_ADDRESS + 0xFC)
#define	REG_LOGGER_UMAC_LMAC_FAST_MES_SEC_WORD           (LOGGER_BASE_ADDRESS + 0x100)
#define	REG_LOGGER_HOST_IF_HANDLER_FAST_MES_SEC_WORD     (LOGGER_BASE_ADDRESS + 0x104)
#define	REG_LOGGER_SENDER_FAST_MES_SEC_WORD              (LOGGER_BASE_ADDRESS + 0x108)
#define	REG_LOGGER_BUFFER_CLOSE_TIME_THR                 (LOGGER_BASE_ADDRESS + 0x10C)
#define	REG_LOGGER_LOGGER_HW_UNIT_CFG_MUX                (LOGGER_BASE_ADDRESS + 0x110)
#define	REG_LOGGER_CLEAR                                 (LOGGER_BASE_ADDRESS + 0x114)
#define	REG_LOGGER_SHRAM_DIRECT_WORDS_IN                 (LOGGER_BASE_ADDRESS + 0x118)
#define	REG_LOGGER_OID_MESAGE_DROP                       (LOGGER_BASE_ADDRESS + 0x11C)
#define	REG_LOGGER_PACKAGER_BUFFER_CTRL_STM              (LOGGER_BASE_ADDRESS + 0x120)
#define	REG_LOGGER_POP_WHILE_EMPTY_OR_PUSH_FULL_LATCH    (LOGGER_BASE_ADDRESS + 0x124)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_LOGGER_MODE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 listConverterBlockEn:1;	// list_converter_block_en
		uint32 loggerLcGclkBypass:1;	// logger_lc_gclk_bypass
		uint32 loggerGclkBypassEn:1;	// logger_gclk_bypass_en
		uint32 swMesageStall:1;	// sw_mesage_stall
		uint32 reserved0:28;
	} bitFields;
} RegLoggerMode_u;

/*REG_LOGGER_RAM_PARAMETERS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:4;
		uint32 wrapperRamMemRm:3;	// wrapper_ram_mem_rm
		uint32 reserved1:1;
		uint32 swUnitMemRm:3;	// sw_unit_mem_rm
		uint32 reserved2:1;
		uint32 hwUnit1MemRm:3;	// hw_unit1_mem_rm
		uint32 reserved3:1;
		uint32 hwUnit2MemRm:3;	// hw_unit2_mem_rm
		uint32 reserved4:1;
		uint32 hwUnit3MemRm:3;	// hw_unit3_mem_rm
		uint32 reserved5:1;
		uint32 hwUnit4MemRm:3;	// hw_unit4_mem_rm
		uint32 reserved6:1;
		uint32 hwUnit5MemRm:3;	// hw_unit5_mem_rm
		uint32 reserved7:1;
	} bitFields;
} RegLoggerRamParameters_u;

/*REG_LOGGER_RAM_PARAMETERS_2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit6MemRm:3;	// hw_unit6_mem_rm
		uint32 reserved0:1;
		uint32 phyRiscMemRm:3;	// phy_risc_mem_rm
		uint32 reserved1:25;
	} bitFields;
} RegLoggerRamParameters2_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST0 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst0:32;	// sw_pre_calculated_checksum_dst0
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst0_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST1 0x10 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst1:32;	// sw_pre_calculated_checksum_dst1
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst1_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST2 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst2:32;	// sw_pre_calculated_checksum_dst2
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst2_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST3 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst3:32;	// sw_pre_calculated_checksum_dst3
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst3_u;

/*REG_LOGGER_NUM_MESSAGES_1 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacNumMessages:8;	// umac_num_messages
		uint32 lmacNumMessages:8;	// lmac_num_messages
		uint32 hostIfNumMessages:8;	// host_if_num_messages
		uint32 rxHandlerNumMessages:8;	// rx_handler_num_messages
	} bitFields;
} RegLoggerNumMessages1_u;

/*REG_LOGGER_NUM_MESSAGES_2 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderNumMessages:8;	// sender_num_messages
		uint32 phyRiscNumMessages:8;	// phy_risc_num_messages
		uint32 hw1NumMessages:8;	// hw1_num_messages
		uint32 hw2NumMessages:8;	// hw2_num_messages
	} bitFields;
} RegLoggerNumMessages2_u;

/*REG_LOGGER_NUM_MESSAGES_3 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw3NumMessages:8;	// hw3_num_messages
		uint32 hw4NumMessages:8;	// hw4_num_messages
		uint32 hw5NumMessages:8;	// hw5_num_messages
		uint32 hw6NumMessages:8;	// hw6_num_messages
	} bitFields;
} RegLoggerNumMessages3_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_1 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacNumBytesInFifo:8;	// umac_num_bytes_in_fifo
		uint32 lmacNumBytesInFifo:8;	// lmac_num_bytes_in_fifo
		uint32 hostIfNumBytesInFifo:8;	// host_if_num_bytes_in_fifo
		uint32 rxHandlerNumBytesInFifo:8;	// rx_handler_num_bytes_in_fifo
	} bitFields;
} RegLoggerNumBytesInFifo1_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_2 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderNumBytesInFifo:8;	// sender_num_bytes_in_fifo
		uint32 phyRiscNumBytesInFifo:8;	// phy_risc_num_bytes_in_fifo
		uint32 hw1NumBytesInFifo:8;	// hw1_num_bytes_in_fifo
		uint32 hw2NumBytesInFifo:8;	// hw2_num_bytes_in_fifo
	} bitFields;
} RegLoggerNumBytesInFifo2_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_3 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw3NumBytesInFifo:8;	// hw3_num_bytes_in_fifo
		uint32 hw4NumBytesInFifo:8;	// hw4_num_bytes_in_fifo
		uint32 hw5NumBytesInFifo:8;	// hw5_num_bytes_in_fifo
		uint32 hw6NumBytesInFifo:8;	// hw6_num_bytes_in_fifo
	} bitFields;
} RegLoggerNumBytesInFifo3_u;

/*REG_LOGGER_UMAC_DATA_IN 0x44 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacDataIn:32;	// umac_data_in
	} bitFields;
} RegLoggerUmacDataIn_u;

/*REG_LOGGER_UMAC_FAST_MES_DATA_IN 0x48 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacFastMesDataIn:32;	// umac_fast_mes_data_in
	} bitFields;
} RegLoggerUmacFastMesDataIn_u;

/*REG_LOGGER_UMAC_MESAGE_DONE 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacMesageDone:1;	// umac_mesage_done
		uint32 reserved0:31;
	} bitFields;
} RegLoggerUmacMesageDone_u;

/*REG_LOGGER_LMAC_DATA_IN 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacDataIn:32;	// lmac_data_in
	} bitFields;
} RegLoggerLmacDataIn_u;

/*REG_LOGGER_LMAC_FAST_MES_DATA_IN 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacFastMesDataIn:32;	// lmac_fast_mes_data_in
	} bitFields;
} RegLoggerLmacFastMesDataIn_u;

/*REG_LOGGER_LMAC_MESAGE_DONE 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmacMesageDone:1;	// lmac_mesage_done
		uint32 reserved0:31;
	} bitFields;
} RegLoggerLmacMesageDone_u;

/*REG_LOGGER_HOST_IF_DATA_IN 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfDataIn:32;	// host_if_data_in
	} bitFields;
} RegLoggerHostIfDataIn_u;

/*REG_LOGGER_HOST_IF_FAST_MES_DATA_IN 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfFastMesDataIn:32;	// host_if_fast_mes_data_in
	} bitFields;
} RegLoggerHostIfFastMesDataIn_u;

/*REG_LOGGER_HOST_IF_MESAGE_DONE 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfMesageDone:1;	// host_if_mesage_done
		uint32 reserved0:31;
	} bitFields;
} RegLoggerHostIfMesageDone_u;

/*REG_LOGGER_RX_HANDLER_DATA_IN 0x68 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandlerDataIn:32;	// rx_handler_data_in
	} bitFields;
} RegLoggerRxHandlerDataIn_u;

/*REG_LOGGER_RX_HANDLER_FAST_MES_DATA_IN 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandlerFastMesDataIn:32;	// rx_handler_fast_mes_data_in
	} bitFields;
} RegLoggerRxHandlerFastMesDataIn_u;

/*REG_LOGGER_RX_HANDLER_MESAGE_DONE 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandlerMesageDone:1;	// rx_handler_mesage_done
		uint32 reserved0:31;
	} bitFields;
} RegLoggerRxHandlerMesageDone_u;

/*REG_LOGGER_SENDER_DATA_IN 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderDataIn:32;	// sender_data_in
	} bitFields;
} RegLoggerSenderDataIn_u;

/*REG_LOGGER_SENDER_FAST_MES_DATA_IN 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderFastMesDataIn:32;	// sender_fast_mes_data_in
	} bitFields;
} RegLoggerSenderFastMesDataIn_u;

/*REG_LOGGER_SENDER_MESAGE_DONE 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderMesageDone:1;	// sender_mesage_done
		uint32 reserved0:31;
	} bitFields;
} RegLoggerSenderMesageDone_u;

/*REG_LOGGER_FIFO_ENABLE 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacFifoEnable:1;	// umac_fifo_enable
		uint32 lmacFifoEnable:1;	// lmac_fifo_enable
		uint32 hostIfFifoEnable:1;	// host_if_fifo_enable
		uint32 rxHandlerFifoEnable:1;	// rx_handler_fifo_enable
		uint32 senderFifoEnable:1;	// sender_fifo_enable
		uint32 phyRiscFifoEnable:1;	// phy_risc_fifo_enable
		uint32 phyFifoEnable:1;	// phy_fifo_enable
		uint32 hw1FifoEnable:1;	// hw1_fifo_enable
		uint32 hw2FifoEnable:1;	// hw2_fifo_enable
		uint32 hw3FifoEnable:1;	// hw3_fifo_enable
		uint32 hw4FifoEnable:1;	// hw4_fifo_enable
		uint32 hw5FifoEnable:1;	// hw5_fifo_enable
		uint32 hw6FifoEnable:1;	// hw6_fifo_enable
		uint32 reserved0:19;
	} bitFields;
} RegLoggerFifoEnable_u;

/*REG_LOGGER_LIST_ID 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferEmtyTxDplIdx:5;	// buffer_emty_tx_dpl_idx
		uint32 reserved0:3;
		uint32 bufferFullTxDplIdx:5;	// buffer_full_tx_dpl_idx
		uint32 reserved1:3;
		uint32 loggerRdEmtyRxDplIdx:5;	// logger_rd_emty_rx_dpl_idx
		uint32 reserved2:3;
		uint32 loggerRdFullRxDplIdx:5;	// logger_rd_full_rx_dpl_idx
		uint32 reserved3:3;
	} bitFields;
} RegLoggerListId_u;

/*REG_LOGGER_LIST_CONVERTER 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerLcHaltWork:1;	// logger_lc_halt_work
		uint32 ethType:2;	// eth_type
		uint32 loggerLcWorking:1;	// logger_lc_working
		uint32 reserved0:28;
	} bitFields;
} RegLoggerListConverter_u;

/*REG_LOGGER_BUFFER_LENGHT 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferLenght:16;	// buffer_lenght
		uint32 reserved0:16;
	} bitFields;
} RegLoggerBufferLenght_u;

/*REG_LOGGER_BUFFER0_1THR 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 buffer0Thr:16;	// buffer0_thr
		uint32 buffer1Thr:16;	// buffer1_thr
	} bitFields;
} RegLoggerBuffer01Thr_u;

/*REG_LOGGER_BUFFER2_3THR 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 buffer2Thr:16;	// buffer2_thr
		uint32 buffer3Thr:16;	// buffer3_thr
	} bitFields;
} RegLoggerBuffer23Thr_u;

/*REG_LOGGER_START_DATA_TRIGGER_LOW 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startDataTriggerLow:32;	// start_data_trigger_low
	} bitFields;
} RegLoggerStartDataTriggerLow_u;

/*REG_LOGGER_START_DATA_TRIGGER_HIGH 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startDataTriggerHigh:16;	// start_data_trigger_high
		uint32 reserved0:16;
	} bitFields;
} RegLoggerStartDataTriggerHigh_u;

/*REG_LOGGER_START_MASK_TRIGGER_LOW 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startMaskTriggerLow:32;	// start_mask_trigger_low
	} bitFields;
} RegLoggerStartMaskTriggerLow_u;

/*REG_LOGGER_START_MASK_TRIGGER_HIGH 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startMaskTriggerHigh:16;	// start_mask_trigger_high
		uint32 reserved0:16;
	} bitFields;
} RegLoggerStartMaskTriggerHigh_u;

/*REG_LOGGER_STOP_DATA_TRIGGER_LOW 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopDataTriggerLow:32;	// stop_data_trigger_low
	} bitFields;
} RegLoggerStopDataTriggerLow_u;

/*REG_LOGGER_STOP_DATA_TRIGGER_HIGH 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopDataTriggerHigh:16;	// stop_data_trigger_high
		uint32 reserved0:16;
	} bitFields;
} RegLoggerStopDataTriggerHigh_u;

/*REG_LOGGER_STOP_MASK_TRIGGER_LOW 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopMaskTriggerLow:32;	// stop_mask_trigger_low
	} bitFields;
} RegLoggerStopMaskTriggerLow_u;

/*REG_LOGGER_STOP_MASK_TRIGGER_HIGH 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopMaskTriggerHigh:16;	// stop_mask_trigger_high
		uint32 logLevelToForward:3;	// log_level_to_forward
		uint32 reserved0:13;
	} bitFields;
} RegLoggerStopMaskTriggerHigh_u;

/*REG_LOGGER_TIME_TO_RECORDER_AFTER_STOP 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timeToRecorderAfterStop:32;	// time_to_recorder_after_stop
	} bitFields;
} RegLoggerTimeToRecorderAfterStop_u;

/*REG_LOGGER_HW_UNIT1_FIRST_HEADER_WORD 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit1FirstHeaderWord:32;	// hw_unit1_first_header_word
	} bitFields;
} RegLoggerHwUnit1FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT2_FIRST_HEADER_WORD 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit2FirstHeaderWord:32;	// hw_unit2_first_header_word
	} bitFields;
} RegLoggerHwUnit2FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT3_FIRST_HEADER_WORD 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit3FirstHeaderWord:32;	// hw_unit3_first_header_word
	} bitFields;
} RegLoggerHwUnit3FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT4_FIRST_HEADER_WORD 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit4FirstHeaderWord:32;	// hw_unit4_first_header_word
	} bitFields;
} RegLoggerHwUnit4FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT5_FIRST_HEADER_WORD 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit5FirstHeaderWord:32;	// hw_unit5_first_header_word
	} bitFields;
} RegLoggerHwUnit5FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT6_FIRST_HEADER_WORD 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit6FirstHeaderWord:32;	// hw_unit6_first_header_word
	} bitFields;
} RegLoggerHwUnit6FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT1_SECOND_HEADER_WORD 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit1SecondHeaderWord:16;	// hw_unit1_second_header_word
		uint32 hwUnit2SecondHeaderWord:16;	// hw_unit2_second_header_word
	} bitFields;
} RegLoggerHwUnit1SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT3_SECOND_HEADER_WORD 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit3SecondHeaderWord:16;	// hw_unit3_second_header_word
		uint32 hwUnit4SecondHeaderWord:16;	// hw_unit4_second_header_word
	} bitFields;
} RegLoggerHwUnit3SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT5_SECOND_HEADER_WORD 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit5SecondHeaderWord:16;	// hw_unit5_second_header_word
		uint32 hwUnit6SecondHeaderWord:16;	// hw_unit6_second_header_word
	} bitFields;
} RegLoggerHwUnit5SecondHeaderWord_u;

/*REG_LOGGER_PHY_FIFO_SECOND_HEADER_WORD 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0:16;
		uint32 phyFifoSecondHeaderWord:16;	// phy_fifo_second_header_word
	} bitFields;
} RegLoggerPhyFifoSecondHeaderWord_u;

/*REG_LOGGER_PHY_FIFO_FIRST_HEADER_WORD 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifoFirstHeaderWord:32;	// phy_fifo_first_header_word
	} bitFields;
} RegLoggerPhyFifoFirstHeaderWord_u;

/*REG_LOGGER_PHY_FIFO_MESAGE_LENGHT 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifoMessageLenght:16;	// phy_fifo_mesage_lenght
		uint32 reserved0:16;
	} bitFields;
} RegLoggerPhyFifoMesageLenght_u;

/*REG_LOGGER_SHRAM_DIRECT_MODE 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramDirectMode:1;	// shram_direct_mode
		uint32 loggerExtMemMode:3;	// , [0]: map ShRAM upper 128K, [1]: map ShRAM upper 256K, [2]: map ShRAM 384K and part of CPUs IRAM
		uint32 shramDirectCyclicMode:1;	// shram_direct_cyclic_mode
		uint32 reserved0:27;
	} bitFields;
} RegLoggerShramDirectMode_u;

/*REG_LOGGER_UMAC_LMAC_FAST_MES_SEC_WORD 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacFastMesSecWord:16;	// umac_fast_mes_sec_word
		uint32 lmacFastMesSecWord:16;	// lmac_fast_mes_sec_word
	} bitFields;
} RegLoggerUmacLmacFastMesSecWord_u;

/*REG_LOGGER_HOST_IF_HANDLER_FAST_MES_SEC_WORD 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfFastMesSecWord:16;	// host_if_fast_mes_sec_word
		uint32 rxHandlerFastMesSecWord:16;	// rx_handler_fast_mes_sec_word
	} bitFields;
} RegLoggerHostIfHandlerFastMesSecWord_u;

/*REG_LOGGER_SENDER_FAST_MES_SEC_WORD 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 senderFastMesSecWord:16;	// sender_fast_mes_sec_word
		uint32 phyRiscFastMesSecWord:16;	// phy_risc_fast_mes_sec_word
	} bitFields;
} RegLoggerSenderFastMesSecWord_u;

/*REG_LOGGER_BUFFER_CLOSE_TIME_THR 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferCloseTimeThr:32;	// buffer_close_time_thr
	} bitFields;
} RegLoggerBufferCloseTimeThr_u;

/*REG_LOGGER_LOGGER_HW_UNIT_CFG_MUX 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerHwUnit1CfgMux:3;	// logger_hw_unit1_cfg_mux
		uint32 reserved0:1;
		uint32 loggerHwUnit2CfgMux:3;	// logger_hw_unit2_cfg_mux
		uint32 reserved1:1;
		uint32 loggerHwUnit3CfgMux:3;	// logger_hw_unit3_cfg_mux
		uint32 reserved2:1;
		uint32 loggerHwUnit4CfgMux:3;	// logger_hw_unit4_cfg_mux
		uint32 reserved3:1;
		uint32 loggerHwUnit5CfgMux:3;	// logger_hw_unit5_cfg_mux
		uint32 reserved4:1;
		uint32 loggerHwUnit6CfgMux:3;	// logger_hw_unit6_cfg_mux
		uint32 reserved5:9;
	} bitFields;
} RegLoggerLoggerHwUnitCfgMux_u;

/*REG_LOGGER_CLEAR 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifoClearPulse:1;	// phy_fifo_clear_pulse
		uint32 loggerClearPulse:1;	// logger_clear_pulse
		uint32 clearPopWhileEmpty:1;	// clear_pop_while_empty
		uint32 clearPushWhileFull:1;	// clear_push_while_full
		uint32 reserved0:28;
	} bitFields;
} RegLoggerClear_u;

/*REG_LOGGER_SHRAM_DIRECT_WORDS_IN 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shDirectWordIn:17;	// sh_direct_word_in
		uint32 shDirectAligner:5;	// sh_direct_aligner
		uint32 shDirectFullRam:1;	// sh_direct_full_ram
		uint32 reserved0:9;
	} bitFields;
} RegLoggerShramDirectWordsIn_u;

/*REG_LOGGER_OID_MESAGE_DROP 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 oidMesageDrop:5;	// oid_mesage_drop
		uint32 oidMesageDropOveride:1;	// oid_mesage_drop_overide
		uint32 reserved0:26;
	} bitFields;
} RegLoggerOidMesageDrop_u;

/*REG_LOGGER_PACKAGER_BUFFER_CTRL_STM 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packagerBufferCtrlStm:4;	// packager_buffer_ctrl_stm
		uint32 reserved0:28;
	} bitFields;
} RegLoggerPackagerBufferCtrlStm_u;

/*REG_LOGGER_POP_WHILE_EMPTY_OR_PUSH_FULL_LATCH 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 popWhileEmptyLatch:12;	// pop_while_empty_latch
		uint32 pushWhileFullLatch:12;	// push_while_full_latch
		uint32 reserved0:8;
	} bitFields;
} RegLoggerPopWhileEmptyOrPushFullLatch_u;



#endif // _LOGGER_REGS_H_
