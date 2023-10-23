
/***********************************************************************************
File:				LoggerRegs.h
Module:				logger
SOC Revision:		
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
#define	REG_LOGGER_RAM_PARAMETERS_3                      (LOGGER_BASE_ADDRESS + 0xC)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST0       (LOGGER_BASE_ADDRESS + 0x14)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST1       (LOGGER_BASE_ADDRESS + 0x18)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST2       (LOGGER_BASE_ADDRESS + 0x1C)
#define	REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST3       (LOGGER_BASE_ADDRESS + 0x20)
#define	REG_LOGGER_NUM_MESSAGES_1                        (LOGGER_BASE_ADDRESS + 0x24)
#define	REG_LOGGER_NUM_MESSAGES_2                        (LOGGER_BASE_ADDRESS + 0x28)
#define	REG_LOGGER_NUM_MESSAGES_3                        (LOGGER_BASE_ADDRESS + 0x2C)
#define	REG_LOGGER_NUM_MESSAGES_4                        (LOGGER_BASE_ADDRESS + 0x30)
#define	REG_LOGGER_NUM_MESSAGES_5                        (LOGGER_BASE_ADDRESS + 0x34)
#define	REG_LOGGER_NUM_MESSAGES_6                        (LOGGER_BASE_ADDRESS + 0x38)
#define	REG_LOGGER_NUM_MESSAGES_7                        (LOGGER_BASE_ADDRESS + 0x3C)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_1                   (LOGGER_BASE_ADDRESS + 0x4C)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_2                   (LOGGER_BASE_ADDRESS + 0x50)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_3                   (LOGGER_BASE_ADDRESS + 0x54)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_4                   (LOGGER_BASE_ADDRESS + 0x58)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_5                   (LOGGER_BASE_ADDRESS + 0x5C)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_6                   (LOGGER_BASE_ADDRESS + 0x60)
#define	REG_LOGGER_NUM_BYTES_IN_FIFO_7                   (LOGGER_BASE_ADDRESS + 0x64)
#define	REG_LOGGER_UMAC_DATA_IN                          (LOGGER_BASE_ADDRESS + 0x6C)
#define	REG_LOGGER_UMAC_FAST_MES_DATA_IN                 (LOGGER_BASE_ADDRESS + 0x70)
#define	REG_LOGGER_UMAC_MESAGE_DONE                      (LOGGER_BASE_ADDRESS + 0x74)
#define	REG_LOGGER_LMAC1_DATA_IN                         (LOGGER_BASE_ADDRESS + 0x78)
#define	REG_LOGGER_LMAC1_FAST_MES_DATA_IN                (LOGGER_BASE_ADDRESS + 0x7C)
#define	REG_LOGGER_LMAC1_MESAGE_DONE                     (LOGGER_BASE_ADDRESS + 0x80)
#define	REG_LOGGER_HOST_IF_DATA_IN                       (LOGGER_BASE_ADDRESS + 0x84)
#define	REG_LOGGER_HOST_IF_FAST_MES_DATA_IN              (LOGGER_BASE_ADDRESS + 0x88)
#define	REG_LOGGER_HOST_IF_MESAGE_DONE                   (LOGGER_BASE_ADDRESS + 0x8C)
#define	REG_LOGGER_RX_HANDLER1_DATA_IN                   (LOGGER_BASE_ADDRESS + 0x90)
#define	REG_LOGGER_RX_HANDLER1_FAST_MES_DATA_IN          (LOGGER_BASE_ADDRESS + 0x94)
#define	REG_LOGGER_RX_HANDLER1_MESAGE_DONE               (LOGGER_BASE_ADDRESS + 0x98)
#define	REG_LOGGER_SENDER1_DATA_IN                       (LOGGER_BASE_ADDRESS + 0x9C)
#define	REG_LOGGER_SENDER1_FAST_MES_DATA_IN              (LOGGER_BASE_ADDRESS + 0xA0)
#define	REG_LOGGER_SENDER1_MESAGE_DONE                   (LOGGER_BASE_ADDRESS + 0xA4)
#define	REG_LOGGER_LMAC2_DATA_IN                         (LOGGER_BASE_ADDRESS + 0xA8)
#define	REG_LOGGER_LMAC2_FAST_MES_DATA_IN                (LOGGER_BASE_ADDRESS + 0xAC)
#define	REG_LOGGER_LMAC2_MESAGE_DONE                     (LOGGER_BASE_ADDRESS + 0xB0)
#define	REG_LOGGER_RX_HANDLER2_DATA_IN                   (LOGGER_BASE_ADDRESS + 0xB4)
#define	REG_LOGGER_RX_HANDLER2_FAST_MES_DATA_IN          (LOGGER_BASE_ADDRESS + 0xB8)
#define	REG_LOGGER_RX_HANDLER2_MESAGE_DONE               (LOGGER_BASE_ADDRESS + 0xBC)
#define	REG_LOGGER_SENDER2_DATA_IN                       (LOGGER_BASE_ADDRESS + 0xC0)
#define	REG_LOGGER_SENDER2_FAST_MES_DATA_IN              (LOGGER_BASE_ADDRESS + 0xC4)
#define	REG_LOGGER_SENDER2_MESAGE_DONE                   (LOGGER_BASE_ADDRESS + 0xC8)
#define	REG_LOGGER_FIFO_ENABLE                           (LOGGER_BASE_ADDRESS + 0xCC)
#define	REG_LOGGER_LIST_ID                               (LOGGER_BASE_ADDRESS + 0xD0)
#define	REG_LOGGER_LIST_CONVERTER                        (LOGGER_BASE_ADDRESS + 0xD4)
#define	REG_LOGGER_BUFFER_LENGHT                         (LOGGER_BASE_ADDRESS + 0xD8)
#define	REG_LOGGER_BUFFER0_1THR                          (LOGGER_BASE_ADDRESS + 0xDC)
#define	REG_LOGGER_BUFFER2_3THR                          (LOGGER_BASE_ADDRESS + 0xE0)
#define	REG_LOGGER_START_DATA_TRIGGER_LOW                (LOGGER_BASE_ADDRESS + 0xE4)
#define	REG_LOGGER_START_DATA_TRIGGER_HIGH               (LOGGER_BASE_ADDRESS + 0xE8)
#define	REG_LOGGER_START_MASK_TRIGGER_LOW                (LOGGER_BASE_ADDRESS + 0xEC)
#define	REG_LOGGER_START_MASK_TRIGGER_HIGH               (LOGGER_BASE_ADDRESS + 0xF0)
#define	REG_LOGGER_STOP_DATA_TRIGGER_LOW                 (LOGGER_BASE_ADDRESS + 0xF4)
#define	REG_LOGGER_STOP_DATA_TRIGGER_HIGH                (LOGGER_BASE_ADDRESS + 0xF8)
#define	REG_LOGGER_STOP_MASK_TRIGGER_LOW                 (LOGGER_BASE_ADDRESS + 0xFC)
#define	REG_LOGGER_STOP_MASK_TRIGGER_HIGH                (LOGGER_BASE_ADDRESS + 0x100)
#define	REG_LOGGER_TIME_TO_RECORDER_AFTER_STOP           (LOGGER_BASE_ADDRESS + 0x104)
#define	REG_LOGGER_HW_UNIT1_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x108)
#define	REG_LOGGER_HW_UNIT2_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x10C)
#define	REG_LOGGER_HW_UNIT3_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x110)
#define	REG_LOGGER_HW_UNIT4_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x114)
#define	REG_LOGGER_HW_UNIT5_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x118)
#define	REG_LOGGER_HW_UNIT6_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x11C)
#define	REG_LOGGER_HW_UNIT7_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x120)
#define	REG_LOGGER_HW_UNIT8_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x124)
#define	REG_LOGGER_HW_UNIT9_FIRST_HEADER_WORD            (LOGGER_BASE_ADDRESS + 0x128)
#define	REG_LOGGER_HW_UNIT10_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x12C)
#define	REG_LOGGER_HW_UNIT11_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x130)
#define	REG_LOGGER_HW_UNIT12_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x134)
#define	REG_LOGGER_HW_UNIT13_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x138)
#define	REG_LOGGER_HW_UNIT14_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x13C)
#define	REG_LOGGER_HW_UNIT15_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x140)
#define	REG_LOGGER_HW_UNIT16_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x144)
#define	REG_LOGGER_HW_UNIT1_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x148)
#define	REG_LOGGER_HW_UNIT3_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x14C)
#define	REG_LOGGER_HW_UNIT5_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x150)
#define	REG_LOGGER_HW_UNIT7_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x154)
#define	REG_LOGGER_HW_UNIT9_SECOND_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x158)
#define	REG_LOGGER_HW_UNIT11_SECOND_HEADER_WORD          (LOGGER_BASE_ADDRESS + 0x15C)
#define	REG_LOGGER_HW_UNIT13_SECOND_HEADER_WORD          (LOGGER_BASE_ADDRESS + 0x160)
#define	REG_LOGGER_HW_UNIT15_SECOND_HEADER_WORD          (LOGGER_BASE_ADDRESS + 0x164)
#define	REG_LOGGER_PHY_FIFO1_SECOND_HEADER_WORD          (LOGGER_BASE_ADDRESS + 0x168)
#define	REG_LOGGER_PHY_FIFO1_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x16C)
#define	REG_LOGGER_PHY_FIFO2_FIRST_HEADER_WORD           (LOGGER_BASE_ADDRESS + 0x170)
#define	REG_LOGGER_PHY_FIFO1_MESAGE_LENGHT               (LOGGER_BASE_ADDRESS + 0x174)
#define	REG_LOGGER_SHRAM_DIRECT_MODE                     (LOGGER_BASE_ADDRESS + 0x178)
#define	REG_LOGGER_UMAC_LMAC_FAST_MES_SEC_WORD           (LOGGER_BASE_ADDRESS + 0x17C)
#define	REG_LOGGER_HOST_IF_HANDLER_FAST_MES_SEC_WORD     (LOGGER_BASE_ADDRESS + 0x180)
#define	REG_LOGGER_SENDER_FAST_MES_SEC_WORD              (LOGGER_BASE_ADDRESS + 0x184)
#define	REG_LOGGER_LMAC_HANDLER2_FAST_MES_SEC_WORD       (LOGGER_BASE_ADDRESS + 0x188)
#define	REG_LOGGER_SENDER_PHY_RISC2_FAST_MES_SEC_WORD    (LOGGER_BASE_ADDRESS + 0x18C)
#define	REG_LOGGER_BUFFER_CLOSE_TIME_THR                 (LOGGER_BASE_ADDRESS + 0x190)
#define	REG_LOGGER_LOGGER_HW_UNIT_CFG_MUX                (LOGGER_BASE_ADDRESS + 0x194)
#define	REG_LOGGER_LOGGER_HW2_UNIT_CFG_MUX               (LOGGER_BASE_ADDRESS + 0x198)
#define	REG_LOGGER_CLEAR                                 (LOGGER_BASE_ADDRESS + 0x19C)
#define	REG_LOGGER_SHRAM_DIRECT_WORDS_IN                 (LOGGER_BASE_ADDRESS + 0x1A0)
#define	REG_LOGGER_OID_MESAGE_DROP                       (LOGGER_BASE_ADDRESS + 0x1A4)
#define	REG_LOGGER_PACKAGER_BUFFER_CTRL_STM              (LOGGER_BASE_ADDRESS + 0x1A8)
#define	REG_LOGGER_POP_WHILE_EMPTY_LATCH                 (LOGGER_BASE_ADDRESS + 0x1AC)
#define	REG_LOGGER_PUSH_WHILE_FULL_LATCH                 (LOGGER_BASE_ADDRESS + 0x1B0)
/*---------------------------------------------------------------------------------
/						Data Type Definition										
/----------------------------------------------------------------------------------*/
/*REG_LOGGER_MODE 0x0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 listConverterBlockEn : 1; //list_converter_block_en, reset value: 0x0, access type: RW
		uint32 loggerLcGclkBypass : 1; //logger_lc_gclk_bypass, reset value: 0x0, access type: RW
		uint32 loggerGclkBypassEn : 1; //logger_gclk_bypass_en, reset value: 0x0, access type: RW
		uint32 swMesageStall : 1; //sw_mesage_stall, reset value: 0x0, access type: RW
		uint32 reserved0 : 28;
	} bitFields;
} RegLoggerMode_u;

/*REG_LOGGER_RAM_PARAMETERS 0x4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 reserved0 : 4;
		uint32 wrapperRamMemRm : 3; //wrapper_ram_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 swUnitMemRm : 3; //sw_unit_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 hwUnit1MemRm : 3; //hw_unit1_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 hwUnit2MemRm : 3; //hw_unit2_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 hwUnit3MemRm : 3; //hw_unit3_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 hwUnit4MemRm : 3; //hw_unit4_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 hwUnit5MemRm : 3; //hw_unit5_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegLoggerRamParameters_u;

/*REG_LOGGER_RAM_PARAMETERS_2 0x8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit6MemRm : 3; //hw_unit6_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 hwUnit7MemRm : 3; //hw_unit7_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 hwUnit8MemRm : 3; //hw_unit8_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 hwUnit9MemRm : 3; //hw_unit9_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 hwUnit10MemRm : 3; //hw_unit10_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved4 : 1;
		uint32 hwUnit11MemRm : 3; //hw_unit11_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved5 : 1;
		uint32 hwUnit12MemRm : 3; //hw_unit12_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved6 : 1;
		uint32 hwUnit13MemRm : 3; //hw_unit13_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegLoggerRamParameters2_u;

/*REG_LOGGER_RAM_PARAMETERS_3 0xC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit14MemRm : 3; //hw_unit14_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved0 : 1;
		uint32 hwUnit15MemRm : 3; //hw_unit15_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved1 : 1;
		uint32 hwUnit16MemRm : 3; //hw_unit16_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved2 : 1;
		uint32 phyRisc1MemRm : 3; //phy_risc0_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved3 : 1;
		uint32 phyRisc2MemRm : 3; //phy_risc1_mem_rm, reset value: 0x3, access type: RW
		uint32 reserved4 : 13;
	} bitFields;
} RegLoggerRamParameters3_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST0 0x14 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst0 : 32; //sw_pre_calculated_checksum_dst0, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst0_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST1 0x18 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst1 : 32; //sw_pre_calculated_checksum_dst1, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst1_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST2 0x1C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst2 : 32; //sw_pre_calculated_checksum_dst2, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst2_u;

/*REG_LOGGER_SW_PRE_CALCULATED_CHECKSUM_DST3 0x20 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 swPreCalculatedChecksumDst3 : 32; //sw_pre_calculated_checksum_dst3, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSwPreCalculatedChecksumDst3_u;

/*REG_LOGGER_NUM_MESSAGES_1 0x24 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacNumMessages : 8; //umac_num_messages, reset value: 0x0, access type: RO
		uint32 lmac1NumMessages : 8; //lmac1_num_messages, reset value: 0x0, access type: RO
		uint32 hostIfNumMessages : 8; //host_if_num_messages, reset value: 0x0, access type: RO
		uint32 rxHandler1NumMessages : 8; //rx_handler1_num_messages, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumMessages1_u;

/*REG_LOGGER_NUM_MESSAGES_2 0x28 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender1NumMessages : 8; //sender1_num_messages, reset value: 0x0, access type: RO
		uint32 phyRisc1NumMessages : 8; //phy_risc1_num_messages, reset value: 0x0, access type: RO
		uint32 hw1NumMessages : 8; //hw1_num_messages, reset value: 0x0, access type: RO
		uint32 hw2NumMessages : 8; //hw2_num_messages, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumMessages2_u;

/*REG_LOGGER_NUM_MESSAGES_3 0x2C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw3NumMessages : 8; //hw3_num_messages, reset value: 0x0, access type: RO
		uint32 hw4NumMessages : 8; //hw4_num_messages, reset value: 0x0, access type: RO
		uint32 hw5NumMessages : 8; //hw5_num_messages, reset value: 0x0, access type: RO
		uint32 hw6NumMessages : 8; //hw6_num_messages, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumMessages3_u;

/*REG_LOGGER_NUM_MESSAGES_4 0x30 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw7NumMessages : 8; //hw7_num_messages, reset value: 0x0, access type: RO
		uint32 hw8NumMessages : 8; //hw8_num_messages, reset value: 0x0, access type: RO
		uint32 hw9NumMessages : 8; //hw9_num_messages, reset value: 0x0, access type: RO
		uint32 hw10NumMessages : 8; //hw10_num_messages, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumMessages4_u;

/*REG_LOGGER_NUM_MESSAGES_5 0x34 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw11NumMessages : 8; //hw11_num_messages, reset value: 0x0, access type: RO
		uint32 hw12NumMessages : 8; //hw12_num_messages, reset value: 0x0, access type: RO
		uint32 hw13NumMessages : 8; //hw13_num_messages, reset value: 0x0, access type: RO
		uint32 hw14NumMessages : 8; //hw14_num_messages, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumMessages5_u;

/*REG_LOGGER_NUM_MESSAGES_6 0x38 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw15NumMessages : 8; //hw15_num_messages, reset value: 0x0, access type: RO
		uint32 hw16NumMessages : 8; //hw16_num_messages, reset value: 0x0, access type: RO
		uint32 lmac2NumMessages : 8; //lmac2_num_messages, reset value: 0x0, access type: RO
		uint32 rxHandler2NumMessages : 8; //rx_handler2_num_messages, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumMessages6_u;

/*REG_LOGGER_NUM_MESSAGES_7 0x3C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender2NumMessages : 8; //sender2_num_messages, reset value: 0x0, access type: RO
		uint32 phyRisc2NumMessages : 8; //phy_risc2_num_messages, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegLoggerNumMessages7_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_1 0x4C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacNumBytesInFifo : 8; //umac_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 lmac1NumBytesInFifo : 8; //lmac1_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hostIfNumBytesInFifo : 8; //host_if_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 rxHandler1NumBytesInFifo : 8; //rx_handler1_num_bytes_in_fifo, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumBytesInFifo1_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_2 0x50 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender1NumBytesInFifo : 8; //sender1_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 phyRisc1NumBytesInFifo : 8; //phy_risc1_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw1NumBytesInFifo : 8; //hw1_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw2NumBytesInFifo : 8; //hw2_num_bytes_in_fifo, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumBytesInFifo2_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_3 0x54 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw3NumBytesInFifo : 8; //hw3_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw4NumBytesInFifo : 8; //hw4_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw5NumBytesInFifo : 8; //hw5_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw6NumBytesInFifo : 8; //hw6_num_bytes_in_fifo, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumBytesInFifo3_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_4 0x58 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw7NumBytesInFifo : 8; //hw7_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw8NumBytesInFifo : 8; //hw8_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw9NumBytesInFifo : 8; //hw9_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw10NumBytesInFifo : 8; //hw10_num_bytes_in_fifo, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumBytesInFifo4_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_5 0x5C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw11NumBytesInFifo : 8; //hw11_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw12NumBytesInFifo : 8; //hw12_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw13NumBytesInFifo : 8; //hw13_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw14NumBytesInFifo : 8; //hw14_num_bytes_in_fifo, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumBytesInFifo5_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_6 0x60 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hw15NumBytesInFifo : 8; //hw15_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 hw16NumBytesInFifo : 8; //hw16_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 lmac2NumBytesInFifo : 8; //lmac2_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 rxHandler2NumBytesInFifo : 8; //rx_handler2_num_bytes_in_fifo, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerNumBytesInFifo6_u;

/*REG_LOGGER_NUM_BYTES_IN_FIFO_7 0x64 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender2NumBytesInFifo : 8; //sender2_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 phyRisc2NumBytesInFifo : 8; //phy_risc2_num_bytes_in_fifo, reset value: 0x0, access type: RO
		uint32 reserved0 : 16;
	} bitFields;
} RegLoggerNumBytesInFifo7_u;

/*REG_LOGGER_UMAC_DATA_IN 0x6C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacDataIn : 32; //umac_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerUmacDataIn_u;

/*REG_LOGGER_UMAC_FAST_MES_DATA_IN 0x70 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacFastMesDataIn : 32; //umac_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerUmacFastMesDataIn_u;

/*REG_LOGGER_UMAC_MESAGE_DONE 0x74 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacMesageDone : 1; //umac_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerUmacMesageDone_u;

/*REG_LOGGER_LMAC1_DATA_IN 0x78 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac1DataIn : 32; //lmac1_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerLmac1DataIn_u;

/*REG_LOGGER_LMAC1_FAST_MES_DATA_IN 0x7C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac1FastMesDataIn : 32; //lmac1_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerLmac1FastMesDataIn_u;

/*REG_LOGGER_LMAC1_MESAGE_DONE 0x80 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac1MesageDone : 1; //lmac1_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerLmac1MesageDone_u;

/*REG_LOGGER_HOST_IF_DATA_IN 0x84 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfDataIn : 32; //host_if_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerHostIfDataIn_u;

/*REG_LOGGER_HOST_IF_FAST_MES_DATA_IN 0x88 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfFastMesDataIn : 32; //host_if_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerHostIfFastMesDataIn_u;

/*REG_LOGGER_HOST_IF_MESAGE_DONE 0x8C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfMesageDone : 1; //host_if_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerHostIfMesageDone_u;

/*REG_LOGGER_RX_HANDLER1_DATA_IN 0x90 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandler1DataIn : 32; //rx_handler1_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerRxHandler1DataIn_u;

/*REG_LOGGER_RX_HANDLER1_FAST_MES_DATA_IN 0x94 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandler1FastMesDataIn : 32; //rx_handler1_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerRxHandler1FastMesDataIn_u;

/*REG_LOGGER_RX_HANDLER1_MESAGE_DONE 0x98 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandler1MesageDone : 1; //rx_handler1_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerRxHandler1MesageDone_u;

/*REG_LOGGER_SENDER1_DATA_IN 0x9C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender1DataIn : 32; //sender1_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSender1DataIn_u;

/*REG_LOGGER_SENDER1_FAST_MES_DATA_IN 0xA0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender1FastMesDataIn : 32; //sender1_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSender1FastMesDataIn_u;

/*REG_LOGGER_SENDER1_MESAGE_DONE 0xA4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender1MesageDone : 1; //sender1_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerSender1MesageDone_u;

/*REG_LOGGER_LMAC2_DATA_IN 0xA8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac2DataIn : 32; //lmac2_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerLmac2DataIn_u;

/*REG_LOGGER_LMAC2_FAST_MES_DATA_IN 0xAC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac2FastMesDataIn : 32; //lmac2_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerLmac2FastMesDataIn_u;

/*REG_LOGGER_LMAC2_MESAGE_DONE 0xB0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac2MesageDone : 1; //lmac2_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerLmac2MesageDone_u;

/*REG_LOGGER_RX_HANDLER2_DATA_IN 0xB4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandler2DataIn : 32; //rx_handler2_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerRxHandler2DataIn_u;

/*REG_LOGGER_RX_HANDLER2_FAST_MES_DATA_IN 0xB8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandler2FastMesDataIn : 32; //rx_handler2_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerRxHandler2FastMesDataIn_u;

/*REG_LOGGER_RX_HANDLER2_MESAGE_DONE 0xBC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 rxHandler2MesageDone : 1; //rx_handler2_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerRxHandler2MesageDone_u;

/*REG_LOGGER_SENDER2_DATA_IN 0xC0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender2DataIn : 32; //sender2_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSender2DataIn_u;

/*REG_LOGGER_SENDER2_FAST_MES_DATA_IN 0xC4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender2FastMesDataIn : 32; //sender2_fast_mes_data_in, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSender2FastMesDataIn_u;

/*REG_LOGGER_SENDER2_MESAGE_DONE 0xC8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender2MesageDone : 1; //sender2_mesage_done, reset value: 0x0, access type: WO
		uint32 reserved0 : 31;
	} bitFields;
} RegLoggerSender2MesageDone_u;

/*REG_LOGGER_FIFO_ENABLE 0xCC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacFifoEnable : 1; //umac_fifo_enable, reset value: 0x0, access type: RW
		uint32 lmac1FifoEnable : 1; //lmac1_fifo_enable, reset value: 0x0, access type: RW
		uint32 hostIfFifoEnable : 1; //host_if_fifo_enable, reset value: 0x0, access type: RW
		uint32 rxHandler1FifoEnable : 1; //rx_handler1_fifo_enable, reset value: 0x0, access type: RW
		uint32 sender1FifoEnable : 1; //sender1_fifo_enable, reset value: 0x0, access type: RW
		uint32 phyRisc1FifoEnable : 1; //phy_risc1_fifo_enable, reset value: 0x0, access type: RW
		uint32 phyFifo1Enable : 1; //phy_fifo1_enable, reset value: 0x0, access type: RW
		uint32 hw1FifoEnable : 1; //hw1_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw2FifoEnable : 1; //hw2_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw3FifoEnable : 1; //hw3_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw4FifoEnable : 1; //hw4_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw5FifoEnable : 1; //hw5_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw6FifoEnable : 1; //hw6_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw7FifoEnable : 1; //hw7_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw8FifoEnable : 1; //hw8_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw9FifoEnable : 1; //hw9_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw10FifoEnable : 1; //hw10_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw11FifoEnable : 1; //hw11_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw12FifoEnable : 1; //hw12_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw13FifoEnable : 1; //hw13_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw14FifoEnable : 1; //hw14_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw15FifoEnable : 1; //hw15_fifo_enable, reset value: 0x0, access type: RW
		uint32 hw16FifoEnable : 1; //hw16_fifo_enable, reset value: 0x0, access type: RW
		uint32 lmac2FifoEnable : 1; //lmac2_fifo_enable, reset value: 0x0, access type: RW
		uint32 rxHandler2FifoEnable : 1; //rx_handler2_fifo_enable, reset value: 0x0, access type: RW
		uint32 sender2FifoEnable : 1; //sender2_fifo_enable, reset value: 0x0, access type: RW
		uint32 phyRisc2FifoEnable : 1; //phy_risc2_fifo_enable, reset value: 0x0, access type: RW
		uint32 phyFifo2Enable : 1; //phy_fifo2_enable, reset value: 0x0, access type: RW
		uint32 reserved0 : 4;
	} bitFields;
} RegLoggerFifoEnable_u;

/*REG_LOGGER_LIST_ID 0xD0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferEmtyTxDplIdx : 6; //buffer_emty_tx_dpl_idx, reset value: 0x0, access type: RW
		uint32 reserved0 : 2;
		uint32 bufferFullTxDplIdx : 6; //buffer_full_tx_dpl_idx, reset value: 0x0, access type: RW
		uint32 reserved1 : 2;
		uint32 loggerRdEmtyRxDplIdx : 6; //logger_rd_emty_rx_dpl_idx, reset value: 0x0, access type: RW
		uint32 reserved2 : 2;
		uint32 loggerRdFullRxDplIdx : 6; //logger_rd_full_rx_dpl_idx, reset value: 0x0, access type: RW
		uint32 reserved3 : 2;
	} bitFields;
} RegLoggerListId_u;

/*REG_LOGGER_LIST_CONVERTER 0xD4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerLcHaltWork : 1; //logger_lc_halt_work, reset value: 0x0, access type: RW
		uint32 ethType : 2; //eth_type, reset value: 0x0, access type: RW
		uint32 loggerLcWorking : 1; //logger_lc_working, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegLoggerListConverter_u;

/*REG_LOGGER_BUFFER_LENGHT 0xD8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferLenght : 16; //buffer_lenght, reset value: 0x500, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegLoggerBufferLenght_u;

/*REG_LOGGER_BUFFER0_1THR 0xDC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 buffer0Thr : 16; //buffer0_thr, reset value: 0x500, access type: RW
		uint32 buffer1Thr : 16; //buffer1_thr, reset value: 0x500, access type: RW
	} bitFields;
} RegLoggerBuffer01Thr_u;

/*REG_LOGGER_BUFFER2_3THR 0xE0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 buffer2Thr : 16; //buffer2_thr, reset value: 0x500, access type: RW
		uint32 buffer3Thr : 16; //buffer3_thr, reset value: 0x500, access type: RW
	} bitFields;
} RegLoggerBuffer23Thr_u;

/*REG_LOGGER_START_DATA_TRIGGER_LOW 0xE4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startDataTriggerLow : 32; //start_data_trigger_low, reset value: 0x500, access type: RW
	} bitFields;
} RegLoggerStartDataTriggerLow_u;

/*REG_LOGGER_START_DATA_TRIGGER_HIGH 0xE8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startDataTriggerHigh : 16; //start_data_trigger_high, reset value: 0x500, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegLoggerStartDataTriggerHigh_u;

/*REG_LOGGER_START_MASK_TRIGGER_LOW 0xEC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startMaskTriggerLow : 32; //start_mask_trigger_low, reset value: 0x500, access type: RW
	} bitFields;
} RegLoggerStartMaskTriggerLow_u;

/*REG_LOGGER_START_MASK_TRIGGER_HIGH 0xF0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 startMaskTriggerHigh : 16; //start_mask_trigger_high, reset value: 0x500, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegLoggerStartMaskTriggerHigh_u;

/*REG_LOGGER_STOP_DATA_TRIGGER_LOW 0xF4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopDataTriggerLow : 32; //stop_data_trigger_low, reset value: 0x500, access type: RW
	} bitFields;
} RegLoggerStopDataTriggerLow_u;

/*REG_LOGGER_STOP_DATA_TRIGGER_HIGH 0xF8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopDataTriggerHigh : 16; //stop_data_trigger_high, reset value: 0x500, access type: RW
		uint32 reserved0 : 16;
	} bitFields;
} RegLoggerStopDataTriggerHigh_u;

/*REG_LOGGER_STOP_MASK_TRIGGER_LOW 0xFC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopMaskTriggerLow : 32; //stop_mask_trigger_low, reset value: 0x500, access type: RW
	} bitFields;
} RegLoggerStopMaskTriggerLow_u;

/*REG_LOGGER_STOP_MASK_TRIGGER_HIGH 0x100 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 stopMaskTriggerHigh : 16; //stop_mask_trigger_high, reset value: 0x500, access type: RW
		uint32 logLevelToForward : 3; //log_level_to_forward, reset value: 0x7, access type: RW
		uint32 reserved0 : 13;
	} bitFields;
} RegLoggerStopMaskTriggerHigh_u;

/*REG_LOGGER_TIME_TO_RECORDER_AFTER_STOP 0x104 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 timeToRecorderAfterStop : 32; //time_to_recorder_after_stop, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerTimeToRecorderAfterStop_u;

/*REG_LOGGER_HW_UNIT1_FIRST_HEADER_WORD 0x108 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit1FirstHeaderWord : 32; //hw_unit1_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit1FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT2_FIRST_HEADER_WORD 0x10C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit2FirstHeaderWord : 32; //hw_unit2_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit2FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT3_FIRST_HEADER_WORD 0x110 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit3FirstHeaderWord : 32; //hw_unit3_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit3FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT4_FIRST_HEADER_WORD 0x114 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit4FirstHeaderWord : 32; //hw_unit4_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit4FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT5_FIRST_HEADER_WORD 0x118 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit5FirstHeaderWord : 32; //hw_unit5_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit5FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT6_FIRST_HEADER_WORD 0x11C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit6FirstHeaderWord : 32; //hw_unit6_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit6FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT7_FIRST_HEADER_WORD 0x120 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit7FirstHeaderWord : 32; //hw_unit7_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit7FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT8_FIRST_HEADER_WORD 0x124 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit8FirstHeaderWord : 32; //hw_unit8_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit8FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT9_FIRST_HEADER_WORD 0x128 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit9FirstHeaderWord : 32; //hw_unit9_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit9FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT10_FIRST_HEADER_WORD 0x12C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit10FirstHeaderWord : 32; //hw_unit10_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit10FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT11_FIRST_HEADER_WORD 0x130 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit11FirstHeaderWord : 32; //hw_unit11_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit11FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT12_FIRST_HEADER_WORD 0x134 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit12FirstHeaderWord : 32; //hw_unit12_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit12FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT13_FIRST_HEADER_WORD 0x138 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit13FirstHeaderWord : 32; //hw_unit13_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit13FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT14_FIRST_HEADER_WORD 0x13C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit14FirstHeaderWord : 32; //hw_unit14_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit14FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT15_FIRST_HEADER_WORD 0x140 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit15FirstHeaderWord : 32; //hw_unit15_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit15FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT16_FIRST_HEADER_WORD 0x144 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit16FirstHeaderWord : 32; //hw_unit16_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit16FirstHeaderWord_u;

/*REG_LOGGER_HW_UNIT1_SECOND_HEADER_WORD 0x148 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit1SecondHeaderWord : 16; //hw_unit1_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit2SecondHeaderWord : 16; //hw_unit2_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit1SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT3_SECOND_HEADER_WORD 0x14C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit3SecondHeaderWord : 16; //hw_unit3_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit4SecondHeaderWord : 16; //hw_unit4_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit3SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT5_SECOND_HEADER_WORD 0x150 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit5SecondHeaderWord : 16; //hw_unit5_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit6SecondHeaderWord : 16; //hw_unit6_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit5SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT7_SECOND_HEADER_WORD 0x154 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit7SecondHeaderWord : 16; //hw_unit7_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit8SecondHeaderWord : 16; //hw_unit8_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit7SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT9_SECOND_HEADER_WORD 0x158 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit9SecondHeaderWord : 16; //hw_unit9_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit10SecondHeaderWord : 16; //hw_unit10_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit9SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT11_SECOND_HEADER_WORD 0x15C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit11SecondHeaderWord : 16; //hw_unit11_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit12SecondHeaderWord : 16; //hw_unit12_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit11SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT13_SECOND_HEADER_WORD 0x160 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit13SecondHeaderWord : 16; //hw_unit13_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit14SecondHeaderWord : 16; //hw_unit14_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit13SecondHeaderWord_u;

/*REG_LOGGER_HW_UNIT15_SECOND_HEADER_WORD 0x164 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hwUnit15SecondHeaderWord : 16; //hw_unit15_second_header_word, reset value: 0x7, access type: RW
		uint32 hwUnit16SecondHeaderWord : 16; //hw_unit16_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerHwUnit15SecondHeaderWord_u;

/*REG_LOGGER_PHY_FIFO1_SECOND_HEADER_WORD 0x168 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifo1SecondHeaderWord : 16; //phy_fifo1_second_header_word, reset value: 0x7, access type: RW
		uint32 phyFifo2SecondHeaderWord : 16; //phy_fifo2_second_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerPhyFifo1SecondHeaderWord_u;

/*REG_LOGGER_PHY_FIFO1_FIRST_HEADER_WORD 0x16C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifo1FirstHeaderWord : 32; //phy_fifo1_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerPhyFifo1FirstHeaderWord_u;

/*REG_LOGGER_PHY_FIFO2_FIRST_HEADER_WORD 0x170 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifo2FirstHeaderWord : 32; //phy_fifo2_first_header_word, reset value: 0x7, access type: RW
	} bitFields;
} RegLoggerPhyFifo2FirstHeaderWord_u;

/*REG_LOGGER_PHY_FIFO1_MESAGE_LENGHT 0x174 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifo1MessageLenght : 16; //phy_fifo1_message_lenght, reset value: 0x30, access type: RW
		uint32 phyFifo2MessageLenght : 16; //phy_fifo2_message_lenght, reset value: 0x30, access type: RW
	} bitFields;
} RegLoggerPhyFifo1MesageLenght_u;

/*REG_LOGGER_SHRAM_DIRECT_MODE 0x178 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shramDirectMode : 1; //shram_direct_mode, reset value: 0x0, access type: RW
		uint32 loggerExtMemMode : 3; //, [0]: map ShRAM upper 128K, [1]: map ShRAM upper 256K, [2]: map ShRAM 384K and part of CPUs IRAM, reset value: 0x0, access type: RW
		uint32 shramDirectCyclicMode : 1; //shram_direct_cyclic_mode, reset value: 0x0, access type: RW
		uint32 reserved0 : 27;
	} bitFields;
} RegLoggerShramDirectMode_u;

/*REG_LOGGER_UMAC_LMAC_FAST_MES_SEC_WORD 0x17C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 umacFastMesSecWord : 16; //umac_fast_mes_sec_word, reset value: 0x0, access type: RW
		uint32 lmac1FastMesSecWord : 16; //lmac1_fast_mes_sec_word, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerUmacLmacFastMesSecWord_u;

/*REG_LOGGER_HOST_IF_HANDLER_FAST_MES_SEC_WORD 0x180 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 hostIfFastMesSecWord : 16; //host_if_fast_mes_sec_word, reset value: 0x0, access type: RW
		uint32 rxHandler1FastMesSecWord : 16; //rx_handler1_fast_mes_sec_word, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerHostIfHandlerFastMesSecWord_u;

/*REG_LOGGER_SENDER_FAST_MES_SEC_WORD 0x184 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender1FastMesSecWord : 16; //sender1_fast_mes_sec_word, reset value: 0x0, access type: RW
		uint32 phyRisc1FastMesSecWord : 16; //phy_risc1_fast_mes_sec_word, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSenderFastMesSecWord_u;

/*REG_LOGGER_LMAC_HANDLER2_FAST_MES_SEC_WORD 0x188 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 lmac2FastMesSecWord : 16; //lmac2_fast_mes_sec_word, reset value: 0x0, access type: RW
		uint32 rxHandler2FastMesSecWord : 16; //rx_handler2_fast_mes_sec_word, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerLmacHandler2FastMesSecWord_u;

/*REG_LOGGER_SENDER_PHY_RISC2_FAST_MES_SEC_WORD 0x18C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 sender2FastMesSecWord : 16; //sender2_fast_mes_sec_word, reset value: 0x0, access type: RW
		uint32 phyRisc2FastMesSecWord : 16; //phy_risc2_fast_mes_sec_word, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerSenderPhyRisc2FastMesSecWord_u;

/*REG_LOGGER_BUFFER_CLOSE_TIME_THR 0x190 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 bufferCloseTimeThr : 32; //buffer_close_time_thr, reset value: 0x0, access type: RW
	} bitFields;
} RegLoggerBufferCloseTimeThr_u;

/*REG_LOGGER_LOGGER_HW_UNIT_CFG_MUX 0x194 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerHwUnit1CfgMux : 3; //logger_hw_unit1_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 loggerHwUnit2CfgMux : 3; //logger_hw_unit2_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 loggerHwUnit3CfgMux : 3; //logger_hw_unit3_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 loggerHwUnit4CfgMux : 3; //logger_hw_unit4_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved3 : 1;
		uint32 loggerHwUnit5CfgMux : 3; //logger_hw_unit5_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved4 : 1;
		uint32 loggerHwUnit6CfgMux : 3; //logger_hw_unit6_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved5 : 1;
		uint32 loggerHwUnit7CfgMux : 3; //logger_hw_unit7_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved6 : 1;
		uint32 loggerHwUnit8CfgMux : 3; //logger_hw_unit8_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegLoggerLoggerHwUnitCfgMux_u;

/*REG_LOGGER_LOGGER_HW2_UNIT_CFG_MUX 0x198 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 loggerHwUnit9CfgMux : 3; //logger_hw_unit9_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved0 : 1;
		uint32 loggerHwUnit10CfgMux : 3; //logger_hw_unit10_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved1 : 1;
		uint32 loggerHwUnit11CfgMux : 3; //logger_hw_unit11_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved2 : 1;
		uint32 loggerHwUnit12CfgMux : 3; //logger_hw_unit12_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved3 : 1;
		uint32 loggerHwUnit13CfgMux : 3; //logger_hw_unit13_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved4 : 1;
		uint32 loggerHwUnit14CfgMux : 3; //logger_hw_unit14_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved5 : 1;
		uint32 loggerHwUnit15CfgMux : 3; //logger_hw_unit15_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved6 : 1;
		uint32 loggerHwUnit16CfgMux : 3; //logger_hw_unit16_cfg_mux, reset value: 0x0, access type: RW
		uint32 reserved7 : 1;
	} bitFields;
} RegLoggerLoggerHw2UnitCfgMux_u;

/*REG_LOGGER_CLEAR 0x19C */
typedef union
{
	uint32 val;
	struct
	{
		uint32 phyFifo1ClearPulse : 1; //phy_fifo1_clear_pulse, reset value: 0x0, access type: WO
		uint32 loggerClearPulse : 1; //logger_clear_pulse, reset value: 0x0, access type: WO
		uint32 clearPopWhileEmpty : 1; //clear_pop_while_empty, reset value: 0x0, access type: WO
		uint32 clearPushWhileFull : 1; //clear_push_while_full, reset value: 0x0, access type: WO
		uint32 phyFifo2ClearPulse : 1; //no description, reset value: 0x0, access type: WO
		uint32 reserved0 : 27;
	} bitFields;
} RegLoggerClear_u;

/*REG_LOGGER_SHRAM_DIRECT_WORDS_IN 0x1A0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 shDirectWordIn : 17; //sh_direct_word_in, reset value: 0x0, access type: RO
		uint32 shDirectAligner : 5; //sh_direct_aligner, reset value: 0x0, access type: RO
		uint32 shDirectFullRam : 1; //sh_direct_full_ram, reset value: 0x0, access type: RO
		uint32 reserved0 : 9;
	} bitFields;
} RegLoggerShramDirectWordsIn_u;

/*REG_LOGGER_OID_MESAGE_DROP 0x1A4 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 oidMesageDrop : 5; //oid_mesage_drop, reset value: 0x0, access type: RW
		uint32 oidMesageDropOveride : 1; //oid_mesage_drop_overide, reset value: 0x0, access type: RW
		uint32 reserved0 : 26;
	} bitFields;
} RegLoggerOidMesageDrop_u;

/*REG_LOGGER_PACKAGER_BUFFER_CTRL_STM 0x1A8 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 packagerBufferCtrlStm : 4; //packager_buffer_ctrl_stm, reset value: 0x0, access type: RO
		uint32 reserved0 : 28;
	} bitFields;
} RegLoggerPackagerBufferCtrlStm_u;

/*REG_LOGGER_POP_WHILE_EMPTY_LATCH 0x1AC */
typedef union
{
	uint32 val;
	struct
	{
		uint32 popWhileEmptyLatch : 32; //pop_while_empty_latch, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerPopWhileEmptyLatch_u;

/*REG_LOGGER_PUSH_WHILE_FULL_LATCH 0x1B0 */
typedef union
{
	uint32 val;
	struct
	{
		uint32 pushWhileFullLatch : 32; //push_while_full_latch, reset value: 0x0, access type: RO
	} bitFields;
} RegLoggerPushWhileFullLatch_u;



#endif // _LOGGER_REGS_H_
